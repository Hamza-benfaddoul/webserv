/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rakhsas <rakhsas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 11:35:06 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/10/30 09:30:43 by rakhsas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(size_t fd, fd_set &readfds) :
	_fd(fd), _readfds(readfds) {};

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
			sendResponse();
		}
		break;
	}
}

std::string readFile( const std::string path )
{
	std::string content, line;
	std::ifstream file;
	// file = ope
	// if (file.is_ope)
	// {

	// }
}

void	Client::getMethodHandler(void){
	std::string fullPath = (this->request->getPath().compare("/") == 0) ? "/www/index.html" : "/www/" + this->request->getPath();

    // // Read the content of the file
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
