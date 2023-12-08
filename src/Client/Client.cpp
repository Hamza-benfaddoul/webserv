/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 11:35:06 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/12/05 15:32:13 by hbenfadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
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
	content_length = 0;
	hasCgi = false;
	isChunkComplete = true;
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
		if (_responseBuffer.find("\r\n\r\n") != std::string::npos)
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
			if (is_request_well_formed() == -1)
				return true;
		}
		else
			return false;
	}
	if (!_readHeader)
	{
		if (this->request->getMethod().compare("GET") == 0)
			return getMethodHandler();
		else if (this->request->getMethod().compare("POST") == 0)
		{
			std::cout << "post is here" << std::endl;
			return postMethodHandler();
		}
		else if (this->request->getMethod().compare("DELETE") == 0)
			return deleteMethodHandler();
	}
	return false;
}

void Client::del(const char *path, bool &isDeleted)
{
    DIR *dir = opendir(path);
    if (dir)
    {
        struct dirent *centent;
        while ((centent = readdir(dir)) != NULL)
        {
            if (strcmp(centent->d_name, ".") != 0 && strcmp(centent->d_name, "..") != 0)
            {
                std::string re(path);
                re += "/";
                re += centent->d_name;
                del(re.c_str(), isDeleted);
            }
        }
		rmdir(path);
    }
    else
    {
        if (access(path, W_OK) == 0)
        {
            if (std::remove(path) != 0)
                throw std::runtime_error("cant remove file");
        }
        else
			isDeleted = false;
    }
}
bool Client::deleteMethodHandler(void)
{
	bool isDeleted = true;
	std::string requestedPath = this->request->getPath();
	if (access((location.getRoot() + requestedPath).c_str(), R_OK) == -1)
	{
		 sendErrorResponse(404, "Not Found", getErrorPage(404),_fd);
		return (true);
	}
	else
		del((location.getRoot() + requestedPath).c_str(), isDeleted);
	if (isDeleted)
		write(_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nResource deleted successfully", 74);
	else
		write(_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nFailed to delete resource", 70);
	return true;
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
	if (requestedPath[requestedPath.length() -1 ] != '/')
	{
		sendRedirectResponse(301, "Moved Permanently", requestedPath + "/");
	}else {
		if (location.index.length() > 0)
		{
			std::cout << "second\n";
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
			sendErrorResponse(403, "Forbidden", getErrorPage(403), _fd);
		} else if (location.getAutoIndex() == true)
		{
			std::cout << "in case autoIndex: off:\t" << location.getRoot() + location.directory << "\n";
			directoryListing(location.getRoot() + location.directory );
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

std::string	Client::createNewFile(std::string prefix, size_t start, std::string suffix)
{
	std::stringstream ss, filename;
	ss << start;
	filename << prefix << start << suffix;
	std::cout << filename.str().c_str() << std::endl;
	std::fstream fd;
	fd.open(filename.str().c_str(), std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
	if (!fd.is_open())
	{
		throw std::ios_base::failure("ERROR: check Folder `www/TempFiles` existence or permissions for Write.\n");
	}
	close(!fd);
	return filename.str();
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
		std::stringstream ss;
		ss <<  _serverBlock->getPort();
		char *env[] =
		{
			strdup(std::string("REDIRECT_STATUS=200").c_str()),
			strdup(std::string("SCRIPT_FILENAME=" + path).c_str()),
			strdup(std::string("CACHE_CONTROL=no-cache").c_str()),
			(path == location.getRoot() + request->getPath()) ? strdup(std::string("QUERY_STRING=").c_str()): strdup(std::string("QUERY_STRING=" + request->getPath().substr(position+1)).c_str()),
			strdup(std::string("REQUEST_METHOD=" + request->getMethod()).c_str()),
			strdup(std::string("SERVER_NAME=localhost").c_str()),
			strdup(std::string("SERVER_PORT=" + ss.str()).c_str()),
			strdup(std::string("SERVER_PROTOCOL=HTTP/1.1").c_str()),
			// strdup(std::string("SERVER_SOFTWARE=Weebserv/1.0").c_str()),
			strdup(std::string("CONTENT_TYPE=" + request->getMimeType()).c_str()),
			// strdup(std::string("CONTENT_LENGTH=" + request->getMimeType()).c_str()),
			strdup(std::string("REDIRECT_STATUS=200").c_str()),
			strdup(std::string("HTTP_COOKIE=" + request->getCookie()).c_str()),
			NULL
		};
		// std::cout << tmpFile << "\n";
		size_t start = get_time('s');
		tmpFile = createNewFile("www/TempFiles/", start, "_cgi");
		// std::cout << "cgi path: " << cgi_path << "the path is: " << path << std::endl;
		// std::cout << "------> " << location.getRoot() + request->getPath() << std::endl;
		int fd = fork();
		char *argv[] = {
			strdup(cgi_path.c_str()),
			// (extension == ".go") ? strdup("run"): strdup(""),
			strdup((location.getRoot() + request->getPath()).c_str()),
			NULL
		};
		if (fd == 0) {
			if (freopen(tmpFile.c_str(), "w", stdout) == NULL)
				throw std::ios_base::failure("Failed to open File");
			execve(argv[0], argv, env);
			exit(0);
		} else {
			int state;
			while (waitpid(fd, &state, WNOHANG) == 0)
			{
				if (get_time('s') > start + location.proxy_read_time_out)
					kill(fd, SIGSEGV);
				usleep(10000);
			}
			readFromCgi();

			ltrim(content, "\r\n");
			size_t pos = content.find("\r\n\r\n");
			std::string bodyCgi;
			if (pos == std::string::npos)
			{	
				pos = content.find("\n\n");
				bodyCgi = content.substr(pos + 2);
			}
			else
				bodyCgi = content.substr(pos + 4);
			// std::cout << "the pos is: " << pos << std::endl;
			std::string headers = content.substr(0, pos);
			// std::cout << "the cgi body: " << bodyCgi << std::endl;
			std::stringstream result;
			std::vector<std::string> splitedHeaders = ft_split(headers, "\r\n");
			if (state == 0)
			{
				result << "HTTP/1.1 200 OK\r\n";
				for (int i = 0; i < (int)splitedHeaders.size(); i++)
				{	
					if (splitedHeaders.at(i) != "\n")
					{
						result << splitedHeaders.at(i);
					}
					result << "\r\n";
				}
				result << "\r\n";
				result << bodyCgi;
			}
			else
			{
				result << "HTTP/1.1 500 Internal Server Error\r\n";
				for (int i = 0; i < (int)splitedHeaders.size(); i++)
				{	
					if (splitedHeaders.at(i) != "\n")
					{
						result << splitedHeaders.at(i);
					}
					result << "\r\n";
				}
				result << "\r\n";
				result << bodyCgi;
			}
			write(_fd, result.str().c_str(), result.str().length());
			std::remove(tmpFile.c_str());
			return true;
		}
		return true;
	} else {
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

void	Client::readFromCgi()
{
	std::fstream cgi_output_content;
	cgi_output_content.open(tmpFile.c_str(), std::ios::in);
	if(!cgi_output_content.is_open())
		throw std::ios_base::failure("Failed to open file");
	char buffer[1024];
	size_t found = 0;
	while (1) {
		cgi_output_content.read(buffer, 1024);
		if (cgi_output_content.gcount() <= 0)
			break;
		content.append(buffer, cgi_output_content.gcount());
		if (cgi_output_content.eof())
			break;
		found = content.find("\r\n\r\n");
		if (found != std::string::npos)
		{
			found += 4;
			content.substr(0, found);
			break;
		}
	}
	file_ouptut.close();
	file_ouptut.open(tmpFile.c_str());
	file_ouptut.seekg(0, std::ios::end);
	content_length = file_ouptut.tellg();
    content_length -= found;
    file_ouptut.seekg(found, std::ios::beg);
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
		sendErrorResponse(404, "Not Found", getErrorPage(404), _fd);
	}
	else if (checkType() == true)
	{
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

// void	Client::sendErrorResponse( int CODE, std::string ERRORTYPE, std::string errorTypeFilePath, int _fd) {
// 	std::ifstream file(errorTypeFilePath.c_str());
// 	std::string content;
// 	if (file.is_open())
// 	{
// 		{
// 			std::string line;
// 			while (getline(file, line))
// 			{
// 				content += line;
// 			}
// 		}
// 	}
// 	file.close();
// 	std::stringstream response;
// 	response << "HTTP/1.1 " << CODE << " " << ERRORTYPE << "\r\n";
// 	response << "Content-Type: text/html; charset=UTF-8\r\n";
// 	response << "Content-Length: " << content.length() << "\r\n";
// 	response << "\r\n";
// 	response << content;

// 	write(_fd, response.str().c_str(), response.str().length());
// }

void	Client::sendRedirectResponse( int CODE, std::string ERRORTYPE, std::string location) {
	std::stringstream response;
	response << "HTTP/1.1 " << CODE << " " << ERRORTYPE << "\r\n";
	std::cout << "location:\t" << location << "\n";
	response << "Location: " << location << "\r\n";
	// response << "Connection: close\r\n";
	response << "\r\n";

	write(_fd, response.str().c_str(), response.str().length());
}

// ======================= POST method ==========================================

int	Client::is_request_well_formed()
{

	// ***** FOR WALID {*******
	// hanta checker if (location.isEmpty = true)
		// sendError404
	// ***** }          *******

	if (location.isEmpty == true)
	{
		sendErrorResponse(404, "Not Found", getErrorPage(404), _fd);
		return (-1);
	}
	std::string path = this->request->getPath();
	std::string charAllowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	int	badChar = 0;

	for (int i = 0; i < (int)path.length(); i++)
	{
		size_t pos = charAllowed.find(path[i]);
		if (pos == std::string::npos)
		{
			badChar = 1;
			break;
		}
	}

	std::map<std::string, std::string> ourHeaders = this->request->getHeaders();
	std::map<std::string, std::string>::iterator it = ourHeaders.find("Transfer-Encoding");
	//bad request
	if (badChar == 1 || this->request->getBad() == 1 || (request->getMethod() == "POST" && it == ourHeaders.end() && ourHeaders.find("Content-Length") == ourHeaders.end()))
	{
		std::cout << badChar << " - " << this->request->getBad() << std::endl;
		sendErrorResponse(400, "Bad Request", getErrorPage(400), _fd);
		return (-1);
	}
	// transfer encoding is equal to chunk"Moved Permanently"ed
	if (ourHeaders.find("Transfer-Encoding") != ourHeaders.end() && ourHeaders["Transfer-Encoding"] != "chunked")
	{
		sendErrorResponse(501, "Not Implemented", getErrorPage(501), _fd);
		return (-1);
	}
	// request uri containe more that 2048 char
	if (path.length() > 2048)
	{
		sendErrorResponse(414, "Request-URI Too Long", getErrorPage(414), _fd);
		return (-1);
	}
	if (location.returnStatus)
	{
		sendRedirectResponse(location.returnstatusCode, "Moved Permanently", location.returnPath);
		return -1;
	}
	if ((request->getMethod() == "GET" && location.GET == false) ||
		(request->getMethod() == "POST" && location.POST == false) ||
			(request->getMethod() == "DELETE" && location.DELETE == false))
		{
			sendErrorResponse(405, "405 Method Not Allowed", getErrorPage(405), _fd);
			return -1;
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
		std::string extension;
		std::string path = request->getPath();
		size_t posDot = path.rfind(".");
		if (posDot != std::string::npos)
		{
			extension = path.substr(posDot, path.length());
			if (this->getCgiPath(extension).length() > 0)
				hasCgi = true;
		}
		this->upload = new Upload(this->request, this->cpt, location, _fd, this->getCgiPath(extension), _serverBlock);
		this->upload->createFile();
		totalBytesRead = body.length();
		if (Headers.find("Content-Length") != Headers.end() && totalBytesRead >= Content_Length)
		{
			this->upload->writeToFileString(body.data(), body.length());
			this->upload->endLine();
			fileCreated = true;
			this->upload->setTotalBodySize(totalBytesRead);
			return this->upload->start();
			// sendErrorResponse(200, "OK", "<html><body><h1>200 Success</h1></body></html>");
			// return true;
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
				totalBytesRead += chunkSizeInt;
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
	else if (Headers.find("Transfer-Encodgin") != Headers.end() && Headers["Transfer-Encoding"] != "chunked")
	{
		std::cout << "here the probleme" << std::endl;
		sendErrorResponse(501, "Not Implemented", getErrorPage(501), _fd);
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
		sendErrorResponse(400, "Bad Request", getErrorPage(400), _fd);
		return true;
	}
	this->upload->endLine();
	this->upload->setTotalBodySize(totalBytesRead);
	return this->upload->start();
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