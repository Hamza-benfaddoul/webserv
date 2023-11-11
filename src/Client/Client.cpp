/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 11:35:06 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/10 08:44:42 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <vector>

Client::Client(size_t fd, fd_set &readfds, std::vector<serverBlock> *serverBlock) :
	_fd(fd), _readfds(readfds), _serverBlock(serverBlock) {};

void	Client::receiveResponse(void)
{
	char buffer[1024] = {0};
	int bytesRead;
	while ((bytesRead = read(_fd, buffer, 1024)))
	{
		std::cout << "bytesRead: " << bytesRead << std::endl;
		if (bytesRead < 0)
			throw std::runtime_error("Could not read from socket");
		this->_responseBuffer += std::string(buffer, bytesRead);
		if (std::string(buffer, bytesRead).find("\r\n\r\n") != std::string::npos)
		{
			this->request = new Request(_responseBuffer);
			this->request->parseRequest();
			this->request->printRequest();
			if (this->request->getMethod().compare("GET") == 0)
				getMethodHandler();
			else if (this->request->getMethod().compare("POST") == 0)
				postMethodHandler();
			// sendResponse();
		}
		break;
	}
}

std::string Client::readFile( const std::string path )
{
	std::cout << path << std::endl;
	std::ifstream file(path.c_str());
	if (file.is_open())
	{
		std::string line,content;
		while (std::getline(file, line)) {
			content.append(line);
		}
		sendResponse1(content, strlen(content.c_str()), this->request->getMimeType());
		file.close();
	}else
	{
        sendErrorResponse(404, "Not Found", "<html><body><h1>404 Not Found</h1></body></html>");
	}
	return "";
}

void	Client::sendErrorResponse( int CODE, std::string ERRORTYPE, std::string ERRORMESSAGE)
{
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


void Client::serveImage(std::string path) {
    std::ifstream faviconFile(path.c_str(), std::ios::binary);
    if (faviconFile.is_open()) {
        std::stringstream content;
        content << faviconFile.rdbuf();
        faviconFile.close();
        // sendImageResponse(request->getMimeType(), content.str());
		sendResponse1(content.str(), content.str().length(), "image/jpg");
    } else {
        sendErrorResponse(404, "Not Found", "<html><body><h1>404 Not Found</h1></body></html>");
    }
}


void	Client::getMethodHandler(void){
	std::cout << "path is: " <<  this->request->getPath() << std::endl;
	std::cout << "mime TYpe is " << this->request->getMimeType() << std::endl;

		std::string fullPath = (this->request->getPath().compare("/") == 0) ? "www/index.html" : "www" + this->request->getPath();
		std::cout << fullPath << std::endl;
		if (this->request->getMimeType().find("image") != std::string::npos)
			serveImage(fullPath);
		else
			std::string content = readFile(fullPath);
}
void	Client::postMethodHandler(void){
	std::cout << "hey from post\n";
}
#include <string> 

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
void    Client::sendResponse(void)
{ 
	// static bool a = false;
	// // std::cout << _responseBuffer << std::endl;
	// if (a)
	// {
	// 	write(_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 12 \r\n\r\nhello world", 92);
	// 	a = false;
	// }
	// else
	// {
	// 	write(_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 12 \r\n\r\nhello hamza", 92);
	// 	a = true;
	// }
}

void Client::run(void)  
{
	this->receiveResponse();
	std::cout << "run..." << std::endl;
}

Client::~Client()
{
	delete request;
	close(_fd);
	FD_CLR(_fd, &_readfds);
}
