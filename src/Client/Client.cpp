/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rakhsas <rakhsas@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 11:35:06 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/15 13:50:27 by rakhsas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <string>
#include <vector>
#include <sys/stat.h>

Client::Client(size_t fd, serverBlock *serverBlock) :
	_fd(fd), _serverBlock(serverBlock) {};

bool	Client::receiveResponse(void)
{
	char buffer[1024] = {0};
	int bytesRead;
	while ((bytesRead = read(_fd, buffer, 1024)))
	{
		// std::cout << "bytesRead: " << bytesRead << std::endl;
		if (bytesRead < 0)
			throw std::runtime_error("Could not read from socket");
		this->_responseBuffer += std::string(buffer, bytesRead);
		if (std::string(buffer, bytesRead).find("\r\n\r\n") != std::string::npos)
		{
			this->request = new Request(_responseBuffer);
			this->request->parseRequest();
			this->request->printRequest();
			if (this->request->getMethod().compare("GET") == 0)
			{
				return getMethodHandler();
			}
			else if (this->request->getMethod().compare("POST") == 0)
				return postMethodHandler();
			// sendResponse();
		}
		break;
	}
	return false;
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

void Client::sendImageResponse(const std::string& contentType, const std::string& imageData) {
	std::stringstream ss;
	ss << imageData.size();

	write(_fd, "HTTP/1.1 200 OK\r\n", 17);
	write(_fd, "Content-Type: ", 14);
	write(_fd, contentType.c_str(), contentType.length());
	write(_fd, "\r\nContent-Length: ", 19);
	write(_fd, ss.str().c_str(), ss.str().length());
	write(_fd, "\r\n\r\n", 4);
	write(_fd, imageData.c_str(), imageData.size());
	fsync(_fd);

	if (write(_fd, "Connection: close\r\n", 19) == -1) {
		perror("Error writing connection close header");
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

void Client::serveImage(std::string path) {
	std::ifstream imageFile(path.c_str(), std::ios::binary);
	if (imageFile.is_open()) {
		// Determine the MIME type based on the file extension
		std::string contentType = getMimeTypeFromExtension(path);
		// Prepare headers
		std::stringstream headers;
		headers << "HTTP/1.1 200 OK\r\n";
		headers << "Content-Type: " << contentType << "\r\n";
		headers << "Transfer-Encoding: chunked\r\n";
		headers << "Content-Disposition: inline\r\n";  // Add this line
		headers << "Connection: close\r\n";
		headers << "\r\n";  // Add an extra newline to indicate the end of headers

		// Write headers to socket
		write(_fd, headers.str().c_str(), headers.str().length());

		// Write file content to socket in chunks
		size_t chunkSize = 1024;
		char buffer[chunkSize];
		while (!imageFile.eof()) {
			imageFile.read(buffer, chunkSize);
			int bytesRead = imageFile.gcount();

			// Send the current chunk
			std::stringstream chunkHeader;
			chunkHeader << std::hex << bytesRead << "\r\n";
			write(_fd, chunkHeader.str().c_str(), chunkHeader.str().length());
			write(_fd, buffer, bytesRead);
			write(_fd, "\r\n", 2);
		}
		write(_fd, "0\r\n\r\n", 5);
		close(_fd);
		imageFile.close();
	} else {
		// Handle file not found
		sendErrorResponse(404, "Not Found", ERROR404);
	}
}

#include <map>
#include <string>

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
	// Find the last dot in the path
	size_t dotPos = path.find_last_of('.');
	if (dotPos != std::string::npos) {
		std::string extension = path.substr(dotPos);
		if (!extension.empty()) {
		std::map<std::string, std::string>::iterator it = extensionToMimeType.find(extension);
		std::cout << "video extension: " << extension << "\n";
		if (it != extensionToMimeType.end()) {
			return it->second; // Return the corresponding MIME type
		}
	}
	}
	return "application/octet-stream";
}

bool Client::checkIfDirectoryIsLocation( std::string path )
{
	struct stat st;

			// std::cout << "location PATH: "<< locationPath << "\n";
			std::cout << "PATH: "<< path << "\n";
	if (stat(path.c_str(), &st) != 0)
		return false;
	if (S_ISDIR(st.st_mode)) {
		for (size_t i = 0; i != this->_serverBlock->getLocations().size(); i++) // LOcations
		{
			std::string locationPath, initialPath;
			initialPath = _serverBlock->getLocations().at(i).getKeyFromAttributes("path");
			initialPath = advanced_trim(initialPath, "\"");
			if (this->_serverBlock->getLocations().at(i).getKeyFromAttributes("root").length() > 0)
				locationPath = this->_serverBlock->getLocations().at(i).getKeyFromAttributes("root") + initialPath;
			else
				locationPath = this->_serverBlock->getRoot() + initialPath;
			if (locationPath == path)
				return true;
		}
	}
	return false;
}

void	Client::handleRequestFromRoot()
{
	std::cout << "ana f root\n";
	std::string fullPath = this->_serverBlock->getRoot() + this->request->getPath();
	struct stat st;

	stat(fullPath.c_str(), &st);
	if (access(fullPath.c_str(), R_OK))
		sendErrorResponse(404, "Not Found", ERROR404);
	if (S_ISREG(st.st_mode)) {
		if (getMimeTypeFromExtension(fullPath).find("image") != std::string::npos ||
			getMimeTypeFromExtension(fullPath).find("video") != std::string::npos) {
			serveImage(fullPath);
		} else {
			readFile(fullPath);
		}
	}
}

void	handleFolderRequest( std::string fullPath)
{
	std::cout << fullPath << "\n";
}

void Client::handleRequestFromLocation(std::string dir) {
	std::string path;

	dir = "/" + dir;
	for (size_t i = 0; i < this->_serverBlock->getLocations().size(); i++) {
		path = this->_serverBlock->getLocations().at(i).getLocationPath();
			// std::cout << "Dir: " << dir << std::endl;
			// std::cout << "Patho:\t" << path << "\n";
		// path = trim(path, "\"");
		// Check if the requested path matches the configured location
		if ( dir == path || (dir.size() > path.size() && dir.compare(0, path.size(), path) == 0 && dir[path.size()] == '/')) {
			// Check if the request path refers to a specific file or a folder
			std::string fullPath = this->_serverBlock->getRoot() + this->request->getPath();
			std::cout << fullPath << "\n";
			bool isFile = regFile(fullPath);
			if (isFile) {
				if (getMimeTypeFromExtension(fullPath).find("image") != std::string::npos ||
					getMimeTypeFromExtension(fullPath).find("video") != std::string::npos) {
					serveImage(fullPath);
				} else {
					readFile(fullPath);
				}
			} else {
				// Handle request for a folder (apply index logic, etc.)
				handleFolderRequest(fullPath);
			}

			// Break the loop since you found a matching location
			break;
		}
	}
}


bool Client::getMethodHandler(void) {
	std::string requestedPath = this->request->getPath();
	// std::cout << "the requestedPath:\t" << this->request->getPath() << "\n";
	try {
		// Check if the requested path has a directory after the hostname
		if (requestedPath == "/") {
			handleRequestFromRoot();
		} else {
			// Extract directory and handle request from location
			size_t directoryEndPos = requestedPath.find("/", 1);
			std::string directory = (directoryEndPos != std::string::npos) ? requestedPath.substr(1, directoryEndPos - 1) : requestedPath.substr(1);
			// if (directory.empty()) {
			// 	directory = "/";
			// }
			if (checkIfDirectoryIsLocation(this->_serverBlock->getRoot() + "/" + directory))
				handleRequestFromLocation(directory);
			else
				sendErrorResponse(404, "NOT FOUND", ERROR404);
		}
	} catch ( const std::exception &e )
	{
		sendErrorResponse(403, "Forbidden", ERROR403);
	}
	return true;
}

// return true;
// if (this->_serverBlock)
//     std::cout << "Root is\t" << this->_serverBlock->getRoot() << std::endl;
// std::cout << "path is: " << this->request->getPath() << std::endl;
// std::cout << "mime Type is " << this->request->getMimeType() << std::endl;

// std::string fullPath = _serverBlock->getRoot() + this->request->getPath();
// std::cout << fullPath << std::endl;

// std::cout << getMimeTypeFromExtension(fullPath) << std::endl;

// if (getMimeTypeFromExtension(fullPath).find("image") != std::string::npos ||
//     getMimeTypeFromExtension(fullPath).find("video") != std::string::npos) {
//     serveImage(fullPath);
// } else {
//     readFile(fullPath);
// }

// return true;


bool	Client::postMethodHandler(void){
	std::cout << "hey from post\n";
	return  true;
}

void    Client::sendResponse1(std::string content, int len, std::string ctype)
{
	std::stringstream ss;
	ss << len;

	std::string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: " + ctype + "; charset=UTF-8\r\n"
		"Content-Length: " + ss.str() + "\r\n"
		"Connection: close\r\n\r\n";

	write(_fd, response.c_str(), response.length());
	write(_fd, content.c_str(), len);

	fsync(_fd);

	if (write(_fd, "Connection: close\r\n", 19) == -1) {
		perror("Error writing connection close header");
	}
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
