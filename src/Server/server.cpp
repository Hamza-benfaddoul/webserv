/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/25 10:29:43 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/10/25 15:28:31by hbenfadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(unsigned long ip, unsigned short port) :
    _ip(ip), _port(port) {};

Server::~Server() {
    close(_socketfd);
};


void    Server::run(void)
{
    initServerSocket();
    std::cout << "Server is running on port " << _port << std::endl;
    listenToClient();
    std::cout << "Server is listening to client" << std::endl;
    acceptClientRequest();
    std::cout << "Server accepted client request" << std::endl;
}   

void    Server::initServerSocket()
{
    _socketfd = socket( AF_INET, SOCK_STREAM, 0);
    if (_socketfd < 0)
        throw std::runtime_error("could not create socket");
    getIp();
    
    // bind the IP and port to the server
    if (bind(_socketfd, (const struct sockaddr *)&_server_address, (socklen_t)sizeof(_server_address)) < 0)
        throw std::runtime_error("Could not bind the address");
};

void    Server::listenToClient()
{
    // listen at the port
    if (listen(_socketfd, MAX_CONNECTIONS) < 0)
        throw std::runtime_error("Could not listen at the port");
}

void    Server::acceptClientRequest(void)
{
    while (true)
    {
            int clientFd = accept(_socketfd, NULL, NULL);
            if (clientFd < 0)
                throw std::runtime_error("could not create socket for client");
            std::cout << "request accepted" << std::endl;
            if(fork())
            {
                char buffer[255];
                read(clientFd, buffer, sizeof(buffer));
                write(clientFd, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 13 \r\n\r\nhello world\n", 100);
                write(1, buffer, strlen(buffer));
                close(clientFd);
                exit(0);
            }
            close(clientFd);
    }
}

void    Server::getIp() {
    memset(&_server_address, 0, sizeof(_server_address));
    _server_address.sin_family = AF_INET;
    _server_address.sin_port = htons(_port);
    _server_address.sin_addr.s_addr = htonl(_ip);
};