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
		sendResponse1(content, strlen(content.c_str()));
	}else
		throw std::runtime_error("could not open file `" + path + "`");
	return "";
}

void	Client::getMethodHandler(void){
	// std::cout << this->request->getPath() << std::endl;
	std::string fullPath = (this->request->getPath().compare("/") == 0) ? "www/index.html" : "www" + this->request->getPath();
	std::cout << fullPath << std::endl;

    // // // // Read the content of the file
    std::string content = readFile(fullPath);

    // if (!content.empty()) {
    //     // Assuming your server has a function to send the HTTP response
    //     sendHttpResponse(200, "OK", content, "text/html");
    // } else {
    //     // Assuming your server has a function to send an error response
    //     sendHttpErrorResponse(404, "Not Found");
    // }
}
void	Client::postMethodHandler(void){
	std::cout << "hey from post\n";
}


void    Client::sendResponse1(std::string content, int len)
{ 
	static bool a = false;
	// std::cout << _responseBuffer << std::endl;
	if (a)
	{
		write(_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n", 58);
		write(_fd, "Content-Length: 434 \r\n\r\n", 25);
		write(_fd, content.c_str(), len);
		a = false;
	}
	else
	{
		write(_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n", 58);
		write(_fd, "Content-Length: 434 \r\n\r\n", 25);
		write(_fd, content.c_str(), len);
		a = true;
	}
}
void    Client::sendResponse(void)
{ 
	static bool a = false;
	// std::cout << _responseBuffer << std::endl;
	if (a)
	{
		write(_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 12 \r\n\r\nhello world", 92);
		a = false;
	}
	else
	{
		write(_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 12 \r\n\r\nhello hamza", 92);
		a = true;
	}
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
