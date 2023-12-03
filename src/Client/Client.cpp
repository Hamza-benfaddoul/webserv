/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rakhsas <rakhsas@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 11:35:06 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/20 22:29:48 by rakhsas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <string>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include "dirent.h"

int Client::cpt = 0;
Client::Client(size_t fd, serverBlock *serverBlock) :
	_fd(fd), location(), _serverBlock(serverBlock)
{
	_fdFile = -1;
	this->_readHeader = true;
	this->request = NULL;
	this->upload = NULL;
	errorCheck = false;
	fileCreated = false;
	totalBytesRead = 0;
	isRead = false;
	controller = false;
}

bool	Client::receiveResponse(void)
{
	if (_readHeader)
	{
		char	buffer[1024] = {0};
		int		bytesRead;
		bytesRead = read(_fd, buffer, 1024);
		if (bytesRead < 0)
			throw std::runtime_error("Could not read from socket");
		_responseBuffer.append(buffer, bytesRead);
		int pos = _responseBuffer.find("\r\n\r\n");
		if (pos != -1)
		{
			this->request = new Request(_responseBuffer);
			this->request->parseRequest();
			this->request->printRequest();
			this->body = this->request->getBodyString();
			std::map<std::string, std::string> Oheaders = this->request->getHeaders();
			if (Oheaders.find("Content-Length") != Oheaders.end())
			{
				std::string C_Length = Oheaders["Content-Length"];
				Content_Length = strtod(C_Length.c_str(), NULL);
			}
			location = getCurrentLocation();
			_readHeader = false;
		}
		else
		{
			_responseBuffer += std::string(_responseBufferVector.begin(), _responseBufferVector.end());
			_responseBufferVector.clear();
		}
	}
	if (!_readHeader)
	{
		// get_match_location_for_request_uri(this->request->getPath()); // get the desired location from the uri ("check the boolean called isLocationExist, true->exit, flae->!exist")
		if (is_request_well_formed() == -1)
			return true;
		if (this->request->getMethod().compare("GET") == 0)
			return getMethodHandler();
		else if (this->request->getMethod().compare("POST") == 0)
		{
			bool check = postMethodHandler();
			return check;
		}
	}
	return false;
}

Location	Client::getCurrentLocation()
{
	std::string requestedPath = this->request->getPath();
	size_t directoryEndPos = requestedPath.find("/", 1);
	std::string directory = (directoryEndPos != std::string::npos) ? requestedPath.substr(0, directoryEndPos) : requestedPath;
	for (size_t i = 0; i != this->_serverBlock->getLocations().size(); i++) {
		Location test = this->_serverBlock->getLocations().at(i);
		if (regFile(test.getRoot() + directory))
		{
			directory = "/";
		}
		if (directory == test.getLocationPath())
		{
			test.directory = directory;
			return test;
		}
	}
	while (directory.length() > 0)
	{
		directory = directory.substr(0, directory.length() - 1);
		for (size_t i = 0; i != this->_serverBlock->getLocations().size(); i++)
		{
			Location test = this->_serverBlock->getLocations().at(i);
			if (regFile(test.getRoot() + directory))
			{
				directory = "/";
			}
			if (directory == test.getLocationPath())
			{
				test.directory = directory;
				return test;
			}
		}
	}
	return Location();
}

bool	Client::checkRequestPath( std::string path )
{
	return (path[path.length() - 1] == '/') ? 1 : 0;
}

bool	Client::checkType()
{
	std::string requestedPath = this->request->getPath();
	DIR *pDir;

	pDir = opendir ((_serverBlock->getRoot() + requestedPath).c_str());
	if (pDir == NULL) {
		return false;
	}
	closedir (pDir);
	return true;
}

bool	Client::checkDir( std::string path )
{
	// std::cout << "path:\t" << path << "\n";
	for (size_t i = 0; i != this->_serverBlock->getLocations().size(); i++) // LOcations
	{
		std::string locationPath, initialPath;
		initialPath = _serverBlock->getLocations().at(i).getLocationPath();
		if (initialPath == path)
			return true;
	}
	return false;
}

bool	Client::handleDirs() {
	std::string requestedPath = this->request->getPath();
	if (requestedPath[requestedPath.length() - 1] != '/')
		sendRedirectResponse(301, "Moved Permanently", requestedPath + '/');
	else {
		if (location.index.length() > 0)
		{
			std::stringstream index(location.index);
			std::string iter;
			while (getline(index,iter,','))
			{
				iter = advanced_trim(iter, " ");
				struct dirent *pDirent;
				DIR *pDir;
				pDir = opendir ((location.getRoot() + "/" + location.directory).c_str());
				if (pDir == NULL) {
					std::cout << "Cannot open directory\n";
					return 1;
				}
				while ((pDirent = readdir(pDir)) != NULL) {
					if (strcmp(iter.c_str(), pDirent->d_name) == 0)
					{
						handleFiles(location.getRoot() + "/" + location.directory + "/" + iter);
						return true;
					}
				}
			}
		} else if(location.getAutoIndex() == false) {
			sendErrorResponse(403, "Forbidden", ERROR403);
		} else if (location.getAutoIndex() == true)
		{
			directoryListing(location.getRoot() + "/" + location.directory );
		}
	}
	return true;
}
#include <unistd.h>
#include <sys/wait.h>

std::string Client::getCgiPath( std::string extension)
{
	for (size_t i = 0; i < location.cgi.size(); i++)
	{
		if (location.cgi.at(i).first == extension)
			return location.cgi.at(i).second;
	}
	return "";
}

std::string extractBodyFromContent(const std::string& content, std::string &header) {
    // Find the end of headers (CRLFCRLF)
    size_t found = content.find("\r\n\r\n");
    // If headers were found, return the substring after the headers
    if (found != std::string::npos) {
		header = content.substr(0, found + 4);
		// std::cout  << header ;
        return content.substr(found + 4);
    }
    // Headers not found, return an empty string or handle accordingly
    return "";
}

bool	Client::handleFiles( std::string path) {
	size_t dotPos = path.find_last_of('.');
	size_t markPos = path.find_last_of('?');
	std::string extension;
	if (dotPos != std::string::npos && (markPos == std::string::npos || dotPos > markPos)) {
		extension = path.substr(dotPos);
	}
	// std::cout << extension << "\n";
	size_t position = this->request->getPath().find('?');
	std::string cgi_path = getCgiPath(extension);
	if (cgi_path.length() > 0){
		// std::cout << "path: " << path << "\n";
		// std::cout << "QUERY_STRING=" + request->getPath().substr(position+1) << "\n";
		// std::cout << request->getCookie() << "\n";
		char *env[] =
		{
			strdup(std::string("REDIRECT_STATUS=200").c_str()),
			strdup(std::string("SCRIPT_FILENAME=" + path).c_str()),
			strdup(std::string("CACHE_CONTROL=no-cache").c_str()),
			(path == location.getRoot() + request->getPath()) ? strdup(std::string("QUERY_STRING=").c_str()): strdup(std::string("QUERY_STRING=" + request->getPath().substr(position+1)).c_str()),
			strdup(std::string("REQUEST_METHOD=" + request->getMethod()).c_str()),
			strdup(std::string("SERVER_NAME=localhost").c_str()),
			strdup(std::string("SERVER_PORT=" + _serverBlock->getPort()).c_str()),
			strdup(std::string("SERVER_PROTOCOL=HTTP/1.1").c_str()),
			strdup(std::string("SERVER_SOFTWARE=Weebserv/1.0").c_str()),
			strdup(std::string("CONTENT_TYPE=" + request->getMimeType()).c_str()),
			strdup(std::string("REDIRECT_STATUS=200").c_str()),
			strdup(std::string("HTTP_COOKIE=" + request->getCookie()).c_str()),
			NULL
		};
		// std::cout << env[1] << "\n";
		// std::cout << env[2] << "\n";
		int pipefd[2];
		size_t start = get_time('s');
		if (pipe(pipefd) == -1) {
			std::cerr << "Error creating pipe.\n";
			return false;
		}
		int fd = fork();
		if (fd == -1) {
			std::cerr << "Fork failed.\n";
			close(pipefd[0]);
			close(pipefd[1]);
			return false;
		}
			// std::cout << request->getPath() << ": path\n";
		if (fd == 0) {
			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);
			char *argv[] = {
				strdup(cgi_path.c_str()),
				strdup((location.getRoot() + request->getPath()).c_str()),
				NULL
			};
			execve(argv[0], argv, env);
		} else {
			close(pipefd[1]);
			char buffer[1024];
			std::string content;
			content.clear();
			while (1) {
				int bytesRead = read(pipefd[0], buffer, sizeof(buffer));
				if (bytesRead <= 0)
					break;
				content.append(buffer, bytesRead);
			}
			close(pipefd[0]);
			int status;
			waitpid(fd, &status, WNOHANG);
			while ( == 0)
			{
				if (get_time('s') > start + location.proxy_read_time_out)
					kill(_fd, SIGSEGV);
				usleep(10000);
			}
				std::cout << "PHP execution was successful.\n";
				std::string contentType;
				content = extractBodyFromContent(content, contentType);
				content = advanced_trim(content, " \n\r");
				std::stringstream headers;
				headers << "HTTP/1.1 200 OK\r\n";
				headers << "Content-Length: " << content.length() << "\r\n";
				headers << contentType;
				write(_fd, headers.str().c_str(), headers.str().length());
				write(_fd, content.c_str(), content.length());
				fsync(_fd);
			// else {
			// 	std::cerr << "PHP execution failed.\n";
			// }
			return true;
		}
		return true;
	} else {
		std::cout << "here\n";
		if (_fdFile == -1)
			_fdFile = open(path.c_str(), O_RDONLY);
		if (_fdFile > -1) {
			std::string mimeType = getMimeTypeFromExtension(path);
			if (isRead == false)
			{
				std::stringstream headers;
				headers << "HTTP/1.1 200 OK\r\n";
				headers << "Content-Type: " << mimeType << "\r\n";
				headers << "Transfer-Encoding: chunked\r\n";
				headers << "Content-Disposition: inline\r\n";
				headers << "Connection: close\r\n\r\n";
				write(_fd, headers.str().c_str(), headers.str().length());
				isRead = true;
			}
			if (isRead == true)
			{
				return serveImage();
			}
		}
		// return readFile(path);
		return true;
	}
}

bool Client::serveImage() {
	if (_fdFile > -1){
		size_t chunkSize = 1024;
		char buffer[chunkSize];
		int	bytesRead = read(_fdFile, buffer, chunkSize);
		if ( bytesRead > 0){
			std::stringstream chunkHeader;
			chunkHeader << std::hex << bytesRead << "\r\n";
			chunkHeader.write(buffer, bytesRead);
			chunkHeader << "\r\n";
			ssize_t bytes_written = write(_fd, chunkHeader.str().c_str(), chunkHeader.str().length());
			if (bytes_written == -1) {
				return true;
			}
			fsync(_fd);
			return false;
		}else{
			write(_fd, "0\r\n\r\n", 5);
			close(_fdFile);
			_fdFile = -1;
			return true;
		}
	}
	return true;
}

std::string	Client::getErrorPage( int errorCode ) {
	for(size_t i = 0; i < _serverBlock->errorPages.size(); i++)
	{
		if (_serverBlock->errorPages.at(i).first == errorCode)
			return _serverBlock->errorPages.at(i).second;
	}
	std::stringstream ss;
	ss << errorCode;
	return "www/error/" + ss.str() + ".html";
}

bool Client::getMethodHandler(void) {
	std::string requestedPath = this->request->getPath();
	size_t queryStringPos = requestedPath.find('?');
	std::string filePath = (queryStringPos != std::string::npos) ? requestedPath.substr(0, queryStringPos) : requestedPath;
	if (access((location.getRoot() + filePath).c_str(), R_OK) == -1)
	{
		sendErrorResponse(404, "Not Found", getErrorPage(404));
	}
	else if (checkType() == true)
	{
		std::cout << "dir\n";
		return handleDirs();
	}
	else if (checkType() == false)
	{
		return handleFiles(location.getRoot() + filePath);
	}
	return true;
}


bool Client::run(void)
{
	return this->receiveResponse();
}

Client::~Client()
{
	delete request;
	close(_fd);
}

void	Client::sendErrorResponse( int CODE, std::string ERRORTYPE, std::string errorTypeFilePath) {
	std::ifstream file(errorTypeFilePath.c_str());
	std::string content;
	if (file.is_open())
	{
		{
			std::string line;
			while (getline(file, line))
			{
				content += line;
			}
		}
	}
	file.close();
	std::stringstream response;
	response << "HTTP/1.1 " << CODE << " " << ERRORTYPE << "\r\n";
	response << "Content-Type: text/html; charset=UTF-8\r\n";
	response << "Content-Length: " << content.length() << "\r\n";
	response << "\r\n";
	response << content;

	write(_fd, response.str().c_str(), response.str().length());
}

void	Client::sendRedirectResponse( int CODE, std::string ERRORTYPE, std::string location) {
	std::stringstream response;
	response << "HTTP/1.1 " << CODE << " " << ERRORTYPE << "\r\n";
	response << "Location: " << location << "\r\n";
	response << "Connection: close\r\n";
	response << "\r\n";

	write(_fd, response.str().c_str(), response.str().length());
}


// void    Client::sendResponse1(std::string content, int len, std::string ctype)
// {
// 	std::stringstream ss;
// 	ss << len;

// 	std::string response =
// 		"HTTP/1.1 200 OK\r\n"
// 		"Content-Type: " + ctype + "; charset=UTF-8\r\n"
// 		"Content-Length: " + ss.str() + "\r\n"
// 		"Connection: close\r\n\r\n";

// 	write(_fd, response.c_str(), response.length());
// 	write(_fd, content.c_str(), len);

// 	fsync(_fd);

// 	if (write(_fd, "Connection: close\r\n", 19) == -1) {
// 		perror("Error writing connection close header");
// 	}
// }

// ======================= POST method ==========================================

int	Client::is_request_well_formed()
{

	// ***** FOR WALID {*******
	// hanta checker if (location.isEmpty = true)
		// sendError404
	// ***** }          *******
	std::string path = this->request->getPath();
	std::string charAllowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	// int	badChar = 0;

	// for (int i = 0; i < (int)path.length(); i++)
	// {
	// 	size_t pos = charAllowed.find(path[i]);
	// 	if (pos == std::string::npos)
	// 	{
	// 		badChar = 1;
	// 		break;
	// 	}
	// }

	std::map<std::string, std::string> ourHeaders = this->request->getHeaders();
	// std::map<std::string, std::string>::iterator it = ourHeaders.find("Transfer-Encoding");
	//bad request
	// if (badChar == 1 || this->request->getBad() == 1 || (it == ourHeaders.end() && ourHeaders.find("Content-Length") == ourHeaders.end()))
	// {
	// 	// std::cout << badChar << " - " << this->request->getBad() << std::endl;
	// 	sendErrorResponse(400, "Bad Request", "<html><body><h1>400 Bad Request</h1></body></html>");
	// 	return (-1);
	// }
	// transfer encoding is equal to chunked
	if (ourHeaders.find("Transfer-Encoding") != ourHeaders.end() && ourHeaders["Transfer-Encoding"] != "chunked")
	{
		sendErrorResponse(501, "Not Implemented", "<html><body><h1>501 Not Implemented</h1></body></html>");
		return (-1);
	}
	// request uri containe more that 2048 char
	if (path.length() > 2048)
	{
		sendErrorResponse(414, "Request-URI Too Long", "<html><body><h1>414 Request-URI Too Long</h1></body></html>");
		return (-1);
	}
	if ((request->getMethod() == "GET" && location.GET == false) ||
		(request->getMethod() == "POST" && location.POST == false) ||
			(request->getMethod() == "DELETE" && location.DELETE == false))
		{
			sendErrorResponse(405, "405 Method Not Allowed", ERROR405);
			return -1;
		}
	// the body length larger than the max body size in the config file
	if (true)
	{
		// ...
	}
	return (true);
}

// true -> close, flase continue;
bool	Client::postMethodHandler(void)
{
	// std::cout << data["upload"] << std::endl;
	std::map<std::string, std::string> Headers = this->request->getHeaders();
	char	buffer[1024] = {0};
	int		bytesRead;

	if (fileCreated == false)
	{
		this->upload = new Upload(this->request, this->cpt, location, _fd);
		this->upload->createFile();
		totalBytesRead = body.length();
		if (Headers.find("Content-Length") != Headers.end() && totalBytesRead >= Content_Length)
		{
			this->upload->writeToFileString(body.data(), body.length());
			this->upload->endLine();
			sendErrorResponse(200, "OK", "<html><body><h1>200 Success</h1></body></html>");
			return true;
		}
		fileCreated = true;
		this->cpt++;
	}
	// read until body is complte (chunk by chunk)
	if (controller == false && Headers.find("Transfer-Encoding") != Headers.end() && Headers["Transfer-Encoding"] == "chunked") // ============> chunk type
	{
		if (isChunkComplete == true)
		{
			ltrim(this->body, "\r\n");
			pos = body.find("\r\n");
			chunkSizeString.append(body.substr(0, pos));
			std::istringstream iss(chunkSizeString);
			iss >> std::hex >> chunkSizeInt;
			if (chunkSizeInt != 0)
			{
				chunkSizeString.clear();
				body.erase(0, pos + 2);
				isChunkComplete = false;
			}
		}
		if (body.find("0\r\n\r\n") != std::string::npos)
		{
			body.erase(body.length() - 5, body.length());
			this->upload->writeToFileString(body);
		}
		else
		{
			if (chunkSizeInt > body.length())
			{
				bytesRead = read(_fd, buffer, 1024);
				body.append(buffer, bytesRead);
			}
			else
			{
				this->upload->writeToFileString(body, chunkSizeInt);
				body.erase(0, chunkSizeInt);
				isChunkComplete = true;
				bytesRead = read(_fd, buffer, 1024);
				body.append(buffer, bytesRead);
			}
			return false;
		}
		controller = true;
		// this->upload->endLine();
	}
	else if (Headers["Transfer-Encoding"] != "chunked")
	{
		sendErrorResponse(501, "Not Implemented", ERROR501);
		return true;
	}
	else if (controller == false &&  Headers.find("Content-Length") != Headers.end()) // ============> binary type
	{
		if (totalBytesRead < this->Content_Length)
		{
			bytesRead = read(_fd, buffer, 1024);
			this->totalBytesRead += bytesRead;
			this->body.append(buffer, bytesRead);
			// if (totalBytesRead >= this->Content_Length)
			this->upload->writeToFileString(body);
			this->upload->endLine();
			this->body.clear();
			if (totalBytesRead < this->Content_Length)
				return false; // keep reading
			controller = true;
		}
	}
	else if (controller == false)
	{
		std::cout << "why" << std::endl;
		sendErrorResponse(400, "Bad Request", ERROR400);
		return true;
	}
	this->upload->endLine();
	return this->upload->start();
	//std::cout << "!!!! exit from reading" << std::endl;
	// sendErrorResponse(200, "OK", "<html><body><h1>200 Success</h1></body></html>");
	// return  true; // close the connection
}

void	Client::directoryListing(std::string path)
{
	std::string html = generateDirectoryListing(path);

	// Prepare headers
	std::stringstream headers;
	headers << "HTTP/1.1 200 OK\r\n";
	headers << "Content-Type: text/html\r\n";
	headers << "Content-Length: " << html.length() << "\r\n";
	headers << "Connection: close\r\n\r\n";

	// Write headers to socket
	write(_fd, headers.str().c_str(), headers.str().length());

	// Write HTML content to socket
	write(_fd, html.c_str(), html.length());
}