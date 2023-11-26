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
#include "dirent.h"
#include <fcntl.h>

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
	canIRead = true;
	totalBytesRead = 0;
	isRead = false;
}

bool	Client::receiveResponse(void)
{
	if (_readHeader)
	{
		char	buffer[1024] = {0};
		int		bytesRead;
		bytesRead = read(_fd, buffer, 1024);
		// this->totalBytesRead += bytesRead;
		if (bytesRead < 0)
			throw std::runtime_error("Could not read from socket");
		this->_responseBuffer += std::string(buffer, bytesRead);
		if (std::string(buffer, bytesRead).find("\r\n\r\n") != std::string::npos)
		{
			//std::cout << _responseBuffer << std::endl;
			// std::cout << "request: " << _responseBuffer << std::endl;
			this->request = new Request(_responseBuffer);
			this->request->parseRequest();
			this->request->printRequest();
			std::map<std::string, std::string> Oheaders = this->request->getHeaders();
			std::string C_Length = Oheaders["Content-Length"];
			Content_Length = strtod(C_Length.c_str(), NULL);
			location = getCurrentLocation();
			_readHeader = false;
		}
	}
	if (!_readHeader)
	{
		if (is_request_well_formed() == -1)
			return true;

		if (this->request->getMethod().compare("GET") == 0)
			return getMethodHandler();
		else if (this->request->getMethod().compare("POST") == 0)
			return postMethodHandler();
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
		for (size_t i = 0; i != this->_serverBlock->getLocations().size(); i++) {
			Location test = this->_serverBlock->getLocations().at(i);
			size_t directoryEndPos = requestedPath.find("/", 1);
			std::string directory = (directoryEndPos != std::string::npos) ? requestedPath.substr(1, directoryEndPos -1) : requestedPath.substr(1);
			if ("/" + directory == test.getLocationPath())
			{
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
							std::cout << "Cannot open directory\n";
							return 1;
						}
						while ((pDirent = readdir(pDir)) != NULL) {
							if (strcmp(iter.c_str(), pDirent->d_name) == 0)
							{
								handleFiles(_serverBlock->getRoot() + "/" + directory + "/" + iter);
								return true;
							}
						}
					}
				} else if(test.getAutoIndex() == false) {
					sendErrorResponse(403, "Forbidden", ERROR403);
				} else if (test.getAutoIndex() == true)
				{
					directoryListing(_serverBlock->getRoot() + "/" + directory );
				}
			}
		}
	}
	return true;
}

bool	Client::handleFiles( std::string path) {
	std::string requestedPath = this->request->getPath();
	for (size_t i = 0; i != this->_serverBlock->getLocations().size(); i++) {
		Location test = this->_serverBlock->getLocations().at(i);
		size_t directoryEndPos = requestedPath.find("/", 1);
		std::string directory = (directoryEndPos != std::string::npos) ? requestedPath.substr(0, directoryEndPos) : requestedPath;
		if (regFile(test.getRoot() + directory))
		{
			std::cout << directory << "\n";
			directory = "/";
		}
		if (directory == test.getLocationPath())
		{
			if (test.getKeyFromAttributes("cgi_path").length() > 0)
			{
				std::cout << path << "\n";
			} else {
				if (_fdFile == -1)
				{
					_fdFile = open(path.c_str(), O_RDONLY);
				}
				if (_fdFile > -1) {
					std::string mimeType = getMimeTypeFromExtension(path);
					std::cout << mimeType << "\n";
					if (isRead == false)
					{
						std::stringstream headers;
						headers << "HTTP/1.1 200 OK\r\n";
						headers << "Content-Type: " << mimeType << "\r\n";
						headers << "Transfer-Encoding: chunked\r\n";
						headers << "Content-Disposition: inline\r\n";
						headers << "Connection: close\r\n\r\n";
						// std::cout << "*" << headers.str();
						// Write headers to socket
						write(_fd, headers.str().c_str(), headers.str().length());
						isRead = true;
					}
					if (isRead == true)
					{
						// std::cout << "ana hna" << std::endl;
						return serveImage();
					}
				}
				// return readFile(path);
			}
		}
	}
	return true;
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
	if (access((_serverBlock->getRoot() + requestedPath).c_str(), R_OK) == -1)
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
		return handleFiles(_serverBlock->getRoot() + requestedPath);
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
	std::string charAllowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#$%&'()*+,/:;=?@[]";
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
	// bad request
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
	std::string requestedPath = this->request->getPath();
	for (size_t i = 0; i != this->_serverBlock->getLocations().size(); i++) {
		Location test = this->_serverBlock->getLocations().at(i);
		size_t directoryEndPos = requestedPath.find("/", 1);
		std::string directory = (directoryEndPos != std::string::npos) ? requestedPath.substr(0, directoryEndPos) : requestedPath;
		if (regFile(test.getRoot() + directory))
		{
			std::cout << directory << "\n";
			directory = "/";
		}
		if (directory == test.getLocationPath())
		{
			if ((request->getMethod() == "GET" && test.GET == false) ||
                    (request->getMethod() == "POST" && test.POST == false) ||
                        (request->getMethod() == "DELETE" && test.DELETE == false))
                {
                    sendErrorResponse(405, "405 Method Not Allowed", ERROR405);
					return -1;
                }
		}
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
	std::string body;

	// if (this->errorCheck == false && is_request_well_formed() == -1)
	// {
	// 	std::cout << "the request is bad .......... i think" << std::endl;
	// 	this->errorCheck = true;
	// 	return true;
	// }

	if (fileCreated == false)
	{
		std::string firstBody = this->request->getBodyString();
		this->upload = new Upload(this->request, this->cpt);
		this->upload->createFile();
		body = ltrim(firstBody, "\r\n");
		totalBytesRead = body.length();
		fileCreated = true;
		this->cpt++;
	}
	// read until body is complte (chunk by chunk)
	if (Headers.find("Transfer-Encoding") != Headers.end() && Headers["Transfer-Encoding"] == "chunked") // ============> chunk type
	{
		std::cout << "its a chunk request" << std::endl;
		// body += std::string(buffer, bytesRead);
		return false;
	}
	else // ============> binary type
	{
		// std::cout << "==> " << totalBytesRead << " " << this->Content_Length << std::endl;
		if (totalBytesRead < this->Content_Length)
		{
			bytesRead = read(_fd, buffer, 1024);
			this->totalBytesRead += bytesRead;
			body += std::string(buffer, bytesRead);
			// std::cout << this->totalBytesRead << std::endl;
			this->upload->writeToFile(body);
			if (totalBytesRead < this->Content_Length)
				return false; // keep reading
		}
		// else
		// {
		// 	sendErrorResponse(200, "OK", "<html><body><h1>200 Success</h1></body></html>");
		// 	return true; // close the connection
		// }
	}
	sendErrorResponse(200, "OK", "<html><body><h1>200 Success</h1></body></html>");
	return  true; // close the connection
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

std::string	Client::getMimeTypeFromExtension(const std::string& path) {
	std::map<std::string, std::string> extensionToMimeType;
		extensionToMimeType[".jpg"] = "image/jpeg";
		extensionToMimeType[".jpeg"] = "image/jpeg";
		extensionToMimeType[".png"] = "image/png";
		extensionToMimeType[".gif"] = "image/gif";
		extensionToMimeType[".bmp"] = "image/bmp";
		extensionToMimeType[".ico"] = "image/x-icon";
		extensionToMimeType[".mp4"] = "video/mp4";
		extensionToMimeType[".ogg"] = "video/ogg";
		extensionToMimeType[".avi"] = "video/x-msvideo";
		extensionToMimeType[".mov"] = "video/quicktime";
		extensionToMimeType[".3gp"] = "video/3gpp";
		extensionToMimeType[".wmv"] = "video/x-ms-wmv";
		extensionToMimeType[".ts"] = "video/mp2t";
		extensionToMimeType[".pdf"] = "application/pdf";
		extensionToMimeType[".html"] = "text/html";
		extensionToMimeType[".css"] = "text/css";
	// Find the last dot in the path
	size_t dotPos = path.find_last_of('.');
	if (dotPos != std::string::npos) {
		std::string extension = path.substr(dotPos);
		if (!extension.empty()) {
			std::map<std::string, std::string>::iterator it = extensionToMimeType.find(extension);
			// std::cout << "video extension: " << extension << "\n";
			if (it != extensionToMimeType.end()) {
				return it->second; // Return the corresponding MIME type
			}
		}
	}
	return "application/octet-stream";
}