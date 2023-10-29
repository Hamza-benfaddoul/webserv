/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 11:35:06 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/10/29 16:07:02 by hbenfadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(size_t fd, fd_set &readfds) :
	_fd(fd), _readfds(readfds) {};

void    Client::receiveResponse(void)
{
    char		buffer[1024] = {0};
    std::string	tmp;

    int bytes_read = read(_fd, buffer, 1024);
    if (bytes_read < 0)
        throw std::runtime_error("Could not read from socket");
    tmp = buffer;
    std::cout << tmp << std::endl;
    if(bytes_read >= 0)
    {
        _responseBuffer = buffer;
        if (tmp.find("\r\n\r\n") != std::string::npos)
		{
            std::cout << _responseBuffer << std::endl;
			sendResponse();
		}
    }
    
}

void    Client::sendResponse(void)
{
	write(_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 11 \r\n\r\nhello world", 92);
}

void Client::run(void)  
{
	this->receiveResponse();
}

Client::~Client()
{
	close(_fd);
	FD_CLR(_fd, &_readfds);
}