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


Server::Server(uint32_t ip, unsigned short port) :
    _ip(ip), _port(port) {};

Server::~Server() {
    close(_socketfd);
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        delete *it;
};


void    Server::run(void)
{
    initServerSocket();
    // std::cout << "Server is running on port " << _port << std::endl;
    listenToClient();
    // std::cout << "Server is listening to client" << std::endl;
    acceptClientRequest();
    // std::cout << "Server accepted client request" << std::endl;
}   

void    Server::initServerSocket()
{
    _socketfd = socket( AF_INET, SOCK_STREAM, 0);
    // if (_socketfd < 0)
    //     throw std::runtime_error("could not create socket");
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
    size_t max_fd = _socketfd;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(_socketfd, &readfds);


    while (true)
    {
        if(select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0)
            throw std::runtime_error("could not select");
        for (size_t i = 0; i <= max_fd; i++)
        {
            if(FD_ISSET(i, &readfds))
            {
                size_t clientFd;
                if(i == _socketfd)
                {
                    clientFd = accept(_socketfd, NULL, NULL);
                    // if (clientFd < 0)
                    //     throw std::runtime_error("could not create socket for client");
                    FD_SET(clientFd, &readfds);
                    _clients.push_back(new Client(clientFd, readfds));
                    if (clientFd > max_fd)
                        max_fd = clientFd;
                }
                else 
                {
                    _clients.at(i - _socketfd - 1)->run();
                }
            }
        }
    }
}

void    Server::getIp() {
    memset(&_server_address, 0, sizeof(_server_address));
    _server_address.sin_family = AF_INET;
    _server_address.sin_port = htons(_port);
    _server_address.sin_addr.s_addr = htonl(_ip);
};
