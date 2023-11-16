/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rakhsas <rakhsas@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 11:35:06 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/16 23:56:00 by rakhsas          ###   ########.fr       */
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

bool	Client::checkRequestPath( std::string path )
{
	return (path[path.length() - 1] == '/')? 1 : 0;
}

bool	Client::checkDir( std::string path )
{
	std::cout << "path:\t" << path << "\n";
	for (size_t i = 0; i != this->_serverBlock->getLocations().size(); i++) // LOcations
	{
		std::string locationPath, initialPath;
		initialPath = _serverBlock->getLocations().at(i).getLocationPath();
		initialPath = advanced_trim(initialPath, "\"");
		if (initialPath == path)
			return true;
	}
	return false;
}

bool Client::getMethodHandler(void) {
	std::string requestedPath = this->request->getPath();
	bool	checker = checkRequestPath(requestedPath);
	if (checker == true)
	{
		if (!checkDir(requestedPath))
			sendErrorResponse(404, "Not Found", ERROR404);
		else
		{
			std::cout << "test\n";
			for (size_t i = 0; i != this->_serverBlock->getLocations().size(); i++) // LOcations
			{
				Location test = this->_serverBlock->getLocations().at(i);
				if (test.getKeyFromAttributes("return").length() > 0)
				{
					std::string returnValue = test.getKeyFromAttributes("return");
					std::stringstream str(returnValue);
					std::string value;
					getline(str, value, ' ');
					if (atoi(value.c_str()) == 301)
						sendErrorResponse(301, "301 Moved Permanently", ERROR301);
					else if (atoi(value.c_str()) == 302)
						sendErrorResponse(302, "302 Found", ERROR302);
				}
				if ((request->getMethod() == "GET" && test.GET == false))
				{
					sendErrorResponse(405, "405 Moved Permanently", ERROR405);
				}
			}

		}
	} else
		std::cout << "is File\n";
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
