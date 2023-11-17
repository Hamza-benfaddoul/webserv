/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rakhsas <rakhsas@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 11:35:06 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/17 16:02:03 by rakhsas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <string>
#include <vector>
#include <sys/stat.h>
#include "dirent.h"

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

void	Client::directoryListing(std::string path)
{

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

bool	Client::postMethodHandler(void){
	std::cout << "hey from post\n";
	return  true;
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
