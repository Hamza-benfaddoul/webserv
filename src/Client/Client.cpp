/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 11:35:06 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/16 16:29:40 by hbenfadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <string>
#include <vector>
#include <sys/stat.h>

Client::Client(size_t fd, serverBlock *serverBlock) :
	_fd(fd), _readHeader(true), _serverBlock(serverBlock) {};

bool	Client::receiveResponse(void)
{
	if (_readHeader)
	{
		char	buffer[1024] = {0};
		int		bytesRead;
		bytesRead = read(_fd, buffer, 1024);
		if (bytesRead < 0)
			throw std::runtime_error("Could not read from socket");
		this->_responseBuffer += std::string(buffer, bytesRead);
		if (std::string(buffer, bytesRead).find("\r\n\r\n") != std::string::npos)
		{
			this->request = new Request(_responseBuffer);
			this->request->parseRequest();
			this->request->printRequest();
			_readHeader = false;
		}
	}
	if (!_readHeader)
	{
		if (this->request->getMethod().compare("GET") == 0)
			return getMethodHandler();
		else if (this->request->getMethod().compare("POST") == 0)
			return postMethodHandler();
	}
	return false;
}


void	Client::sendErrorResponse( int CODE, std::string ERRORTYPE, std::string ERRORMESSAGE) {
	std::stringstream response;
	response << "HTTP/1.1 " << CODE << " " << ERRORTYPE << "\r\n";
	response << "Content-Type: text/html; charset=UTF-8\r\n";
	response << "Content-Length: " << ERRORMESSAGE.length() << "\r\n";
	response << "\r\n";
	response << ERRORMESSAGE;

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
		std::cout << "-------- Response Header -------->" << std::endl;
		std::cout << headers.str() << std::endl;
		std::cout << "-------- Response Header -------->" << std::endl;

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
		sendErrorResponse(404, "Not Found", "<html><body><h1>404 Not Found</h1></body></html>");
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
		if (it != extensionToMimeType.end()) {
			return it->second; // Return the corresponding MIME type
		}
	}
	}
	return "application/octet-stream";
}


bool	Client::getMethodHandler(void){
	if (this->_serverBlock)
		std::cout << "daz mn hna" /*this->_serverBlock->size()*/ << std::endl;
	else
		std::cout << "_serverBlock is NULL\n";
	std::cout << "path is: " <<  this->request->getPath() << std::endl;
	std::cout << "mime TYpe is " << this->request->getMimeType() << std::endl;
	std::string fullPath = (this->request->getPath().compare("/") == 0) ? "www/index.html" : "www" + this->request->getPath();
	std::cout << fullPath << std::endl;
	std::cout << getMimeTypeFromExtension(fullPath) << std::endl;
	if (getMimeTypeFromExtension(fullPath).find("image") != std::string::npos
		|| getMimeTypeFromExtension(fullPath).find("video") != std::string::npos)
	{
		serveImage(fullPath);
	}
	else
		readFile(fullPath);
	return true;
}
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
