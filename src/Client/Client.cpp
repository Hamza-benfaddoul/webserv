/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rakhsas <rakhsas@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 11:35:06 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/17 21:10:54 by rakhsas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <string>
#include <vector>
#include <sys/stat.h>
#include "dirent.h"

int Client::cpt = 0;
Client::Client(size_t fd, serverBlock *serverBlock) :
	_fd(fd), _serverBlock(serverBlock)
{
	this->_readHeader = true;
	this->request = NULL;
	this->upload = NULL;
	errorCheck = false;
	fileCreated = false;
	canIStart = false;
	isLocationExist = false;
	totalBytesRead = 0;
	Content_Length = -1;
	rest = 0;
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
		for (int i = 0; i < bytesRead; i++)
			_responseBufferVector.push_back(buffer[i]);
		// this->_responseBuffer += std::string(buffer, bytesRead);
		// postRequest = _responseBuffer;
		// if (std::string(buffer, bytesRead).find("\r\n\r\n") != std::string::npos)
		int	pos = isInclude(_responseBufferVector, "\r\n\r\n");
		if (pos != -1)
		{
			_responseBuffer += std::string(_responseBufferVector.begin(), _responseBufferVector.begin() + pos);
			_responseBufferVector.erase(_responseBufferVector.begin(), _responseBufferVector.begin() + pos + 4);
			this->request = new Request(_responseBuffer, _responseBufferVector);
			this->request->parseRequest();
			this->request->printRequest();
			std::map<std::string, std::string> Oheaders = this->request->getHeaders();
			if (Oheaders.find("Content-Length") != Oheaders.end())
			{
				std::string C_Length = Oheaders["Content-Length"];
				Content_Length = strtod(C_Length.c_str(), NULL);
			}
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
		// if (is_request_well_formed() == -1)
		// 	return true;
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
	std::cout << "path:\t" << path << "\n";
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
	// std::cout << requestedPath << "\n";
	if (requestedPath[requestedPath.length() - 1] != '/')
		sendRedirectResponse(301, "Moved Permanently", requestedPath + '/');
	else {
		for (size_t i = 0; i != this->_serverBlock->getLocations().size(); i++) {
			Location test = this->_serverBlock->getLocations().at(i);
			size_t directoryEndPos = requestedPath.find("/", 1);
			std::string directory = (directoryEndPos != std::string::npos) ? requestedPath.substr(1, directoryEndPos -1) : requestedPath.substr(1);
			if ("/" + directory == test.getLocationPath())
			{
				// here check for index files
				if (test.index.length() > 0)
				{
					std::stringstream index(test.index);
					std::string iter;
					while (getline(index,iter,','))
					{
						iter = advanced_trim(iter, " ");
						struct dirent *pDirent;
						DIR *pDir;
						pDir = opendir ((_serverBlock->getRoot() + "/" + directory).c_str());
						if (pDir == NULL) {
							printf ("Cannot open directory\n");
							return 1;
						}
						while ((pDirent = readdir(pDir)) != NULL) {
							std::cout << "iter:\t*" << iter << "*\n";
							std::cout << "pDir:\t*" << pDirent->d_name << "*\n";
							if (strcmp(iter.c_str(), pDirent->d_name) == 0)
							{
								handleFiles(_serverBlock->getRoot() + "/" + directory + "/" + iter);
								return true;
							}
						}
					}
				} else if(test.getAutoIndex() == false) {
					// if server doesn't support auto index
					sendErrorResponse(403, "Forbidden", ERROR403);
				} else if (test.getAutoIndex() == true)
				{
					// process listing
					directoryListing(_serverBlock->getRoot() + "/" + directory );
				}
			}
		}
	}
	return true;
}

std::string generateDirectoryListing(const std::string& directoryPath) {
    std::stringstream html;
    html << "<html><head><title>Directory Listing</title>";
    html << "<style>body { margin: 0; font-family: \"HelveticaNeue-Light\", \"Helvetica Neue Light\", \"Helvetica Neue\", Helvetica, Arial, \"Lucida Grande\", sans-serif;";
    html << "font-weight: 300; color: #404040; }";
    html << "h2 { width: 93%; margin-left: auto; margin-right: auto; }";
    html << "table { width: 100%; background: white; border: 0; table-layout: auto; }";
    html << "table caption { background: transparent; color: #222222; font-size: 1rem; font-weight: bold; }";
    html << "table thead { background: whitesmoke; }";
    html << "table thead tr th, table thead tr td { padding: 0.5rem 0.625rem 0.625rem; font-size: 0.875rem; font-weight: bold; color: #222222; }";
    html << "table tfoot { background: whitesmoke; }";
    html << "table tfoot tr th, table tfoot tr td { padding: 0.5rem 0.625rem 0.625rem; font-size: 0.875rem; font-weight: bold; color: #222222; }";
    html << "table tr th, table tr td { padding: 0.5625rem 0.625rem; font-size: 0.875rem; color: #222222; text-align: left; }";
    html << "table tr.alt, table tr:nth-of-type(even) { background: #f9f9f9; }";
    html << "table thead tr th, table tfoot tr th, table tfoot tr td, table tbody tr th, table tbody tr td, table tr td { display: table-cell; line-height: 1.125rem; }";
    html << "a { text-decoration: none; color: #3498db; }";
    html << "a:hover { text-decoration: underline; }";
    html << "a:visited { color: #8e44ad; }";
    html << ".img-wrap { vertical-align: middle; display: inline-block; margin-right: 8px; margin-bottom: 2px; width: 16px; }";
    html << "td img { display: block; width: 100%; height: auto; }";
    html << "@media (max-width: 600px) { table tr > *:nth-child(2), table tr > *:nth-child(3), table tr > *:nth-child(4) { display: none; }";
    html << "h1 { font-size: 1.5em; } }";
    html << "@media (max-width: 400px) { h1 { font-size: 1.125em; } }</style></head>";
    html << "<body><h2>Directory Listing</h2><table id=\"indexlist\">";
    html << "<tr class=\"indexhead\"><th class=\"indexcolicon\"><span></span></th><th class=\"indexcolname\"><a>Name</a></th></tr>";
    html << "<tr class=\"indexbreakrow\"><th colspan=\"5\"><hr /></th></tr>";

    DIR* dir;
    struct dirent* entry;

    // Open the directory
    dir = opendir(directoryPath.c_str());

    if (dir != NULL) {
        // Read directory entries
        while ((entry = readdir(dir)) != NULL) {
            std::string name = entry->d_name;

            // Skip current and parent directory entries
            if (name != "." && name != "..") {
                // Add an entry to the HTML table
                html << "<tr class=\"even\"><td class=\"indexcolname\"><a href=\"" << name << "\">" << name << "</a></td></tr>";
                html << "<tr class=\"indexbreakrow\"><th colspan=\"5\"><hr /></th></tr>";
            }
        }

        // Close the directory
        closedir(dir);
    } else {
        // Handle directory open error
        std::cerr << "Error opening directory: " << strerror(errno) << std::endl;
    }

    html << "</table></body></html>";
    return html.str();
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

bool	Client::handleFiles( std::string path) {
	std::cout << "hellofrom handlefiles\n" << "path is:\t" << path << "\n";
	return true;
}

bool Client::getMethodHandler(void) {
	std::string requestedPath = this->request->getPath();
	if (access((_serverBlock->getRoot() + requestedPath).c_str(), R_OK) == -1)
		sendErrorResponse(404, "Not Found", ERROR404);
	else if (checkType() == true)
		handleDirs();
	else if (checkType() == false)
		handleFiles(_serverBlock->getRoot() + requestedPath);
	return true;
}


bool Client::run(void)
{
	return this->receiveResponse();
}

Client::~Client()
{
	// delete upload;
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

void Client::get_match_location_for_request_uri(const std::string &uri)
{
	/*
		if ((request->getMethod() == "GET" && ourLocation.GET == false) ||
			(request->getMethod() == "POST" && ourLocation.POST == false) ||
			(request->getMethod() == "DELETE" && ourLocation.DELETE == false))
		{
			sendErrorResponse(405, "405 Method Not Allowed", ERROR405);
		}
	*/
	std::vector<Location> allLocations = this->_serverBlock->getLocations();
	std::string locationPath;
	Location location;

	for (int i = 0; i < (int)allLocations.size(); i++)
	{
		location = allLocations.at(i);
		locationPath = location.getLocationPath();
		if (locationPath == uri)
		{
			ourLocation = location.getLocationAttributes();
			isLocationExist = true;
			break;
		}
	}
}

void Client::readFile(const std::string path) {
	std::ifstream file(path.c_str(), std::ios::binary);
	if (file.is_open()) {
		// Get file size
		file.seekg(0, std::ios::end);
		const size_t fileSize = file.tellg();
		file.seekg(0, std::ios::beg);
		// Prepare headers
		std::stringstream headers;
		headers << "HTTP/1.1 200 OK\r\n";
		headers << "Content-Type: " << this->request->getMimeType() << "\r\n";
		headers << "Content-Length: " << fileSize << "\r\n";
		headers << "Connection: close\r\n\r\n";
		// Write headers to socket
		write(_fd, headers.str().c_str(), headers.str().length());
		// Write file content to socket
		char buffer[1024];
		while (!file.eof()) {
			file.read(buffer, sizeof(buffer));
			int bytesRead = file.gcount();
			if (bytesRead > 0) {
				write(_fd, buffer, bytesRead);
			}
		}
		file.close();
		fsync(_fd);
	} else {
		// Handle file not found
		sendErrorResponse(404, "Not Found", "<html><body><h1>404 Not Found</h1></body></html>");
	}
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
	std::string path = this->request->getPath();
	std::string charAllowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#$%&'()*+,/:;=?@[]";
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
	// bad request
	if (badChar == 1 || this->request->getBad() == 1 || (it == ourHeaders.end() && ourHeaders.find("Content-Length") == ourHeaders.end()))
	{
	// 	// std::cout << badChar << " - " << this->request->getBad() << std::endl;
	// 	sendErrorResponse(400, "Bad Request", "<html><body><h1>400 Bad Request</h1></body></html>");
	// 	return (-1);
	 }
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
	// if no location match the request uri ==> 404 not found
	if (isLocationExist == false)
	{
	// 	sendErrorResponse(404, "Not Found", ERROR404);
	// 	return (-1);
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
	std::map<std::string, std::string> Headers = this->request->getHeaders();
	char	buffer[1024] = {0};
	int		bytesRead;

	if (fileCreated == false)
	{
		std::string firstBody = this->request->getBodyString();
		this->upload = new Upload(this->request, this->cpt);
		this->upload->createFile();
		totalBytesRead = _responseBufferVector.size();
		if (Headers.find("Content-Length") != Headers.end() && totalBytesRead >= Content_Length)
		{
			this->upload->writeToFile(_responseBufferVector);
			this->upload->endLine();
			sendErrorResponse(200, "OK", "<html><body><h1>200 Success</h1></body></html>");
			return true;
		}
		fileCreated = true;
		this->cpt++;
	}
	// read until body is complte (chunk by chunk)
	if (Headers.find("Transfer-Encoding") != Headers.end() && Headers["Transfer-Encoding"] == "chunked") // ============> chunk type
	{
		// isInclude();
		if (isChunkComplete == true)
		{
			// pos = isInclude(_responseBufferVector, "\r\n");
			std::string result(_responseBufferVector.begin(), _responseBufferVector.end());
			pos = result.find("\r\n");
			std::cout << "the pos is: " << pos << std::endl;
			chunkSizeString.append(_responseBufferVector.begin(), _responseBufferVector.begin() + pos);
			std::istringstream iss(chunkSizeString);
			iss >> std::hex >> chunkSizeInt;
			_responseBufferVector.erase(_responseBufferVector.begin(), _responseBufferVector.begin() + pos + 2);
			isChunkComplete = false;
		}
		std::string result(_responseBufferVector.begin(), _responseBufferVector.end());
		if (result.find("0\r\n\r\n") != std::string::npos)
		{
		// }
		// if (isInclude(_responseBufferVector, "0\r\n\r\n") != -1)
		// {
			_responseBufferVector.erase(_responseBufferVector.end() - 5, _responseBufferVector.end());
			this->upload->writeToFile(_responseBufferVector, _responseBufferVector.size());
		}
		else
		{
			if (chunkSizeInt > _responseBufferVector.size())
			{
				this->upload->writeToFile(_responseBufferVector);
				_responseBufferVector.clear();
			}
			else
			{
				this->upload->writeToFile(_responseBufferVector, chunkSizeInt);
				if (chunkSizeInt < _responseBufferVector.size())
					_responseBufferVector.erase(_responseBufferVector.begin(), _responseBufferVector.begin() + chunkSizeInt);
				isChunkComplete = true;
			}
			bytesRead = read(_fd, buffer, 1024);
			_responseBufferVector.insert(_responseBufferVector.end(), buffer, buffer + bytesRead);
			this->totalBytesRead += bytesRead;
			return (false);
		}

		

		// this->upload->writeToFile(_responseBufferVector);
		// this->upload->endLine(); 
		// tomorrow start writing what you need from each chunk now you have the size of the chunk and vector of char need to be writed
		// in you vector you have 400 char and the size of chunk is 4000 so 4000 - 400 = 3600, the next time you read 1024 you will write
		// 3600 - 1024 = 2576, and so on good luck lah lmo3in
	}
	else // ============> binary type
	{
		if (totalBytesRead < this->Content_Length)
		{
			bytesRead = read(_fd, buffer, 1024);
			_responseBufferVector.insert(_responseBufferVector.end(), buffer, buffer + bytesRead);
			this->totalBytesRead += bytesRead;
			this->upload->writeToFile(_responseBufferVector);
			_responseBufferVector.clear();
			if (totalBytesRead < this->Content_Length)
				return false; // keep reading
		}
	}
	this->upload->endLine();
	this->upload->start();
	std::cout << "!!!! exit from reading" << std::endl;
	sendErrorResponse(200, "OK", "<html><body><h1>200 Success</h1></body></html>");
	return  true; // close the connection
}
