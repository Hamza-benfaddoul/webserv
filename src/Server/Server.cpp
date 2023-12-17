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
#include <vector>


Server::Server(uint32_t ip, unsigned short port,std::string serverName, serverBlock *serverBlock) :
    _ip(ip), _port(port), _serverName(serverName) {
		_serverBlock = serverBlock;
	};

Server::~Server() {
	close(_socketfd);
};


int    Server::run(void)
{
	try
	{
		initServerSocket();
		listenToClient();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (0);
	}
	return (1);
}

void    Server::initServerSocket()
{
	_socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketfd < 0)
		throw std::runtime_error("could not create socket");
	this->setupIp();
	int opt = 1;
	if (setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
	{
		perror("setsockopt failed");
		close(_socketfd);
		exit(EXIT_FAILURE);
	}
	// bind the IP and port to the server
	if (bind(_socketfd, (const struct sockaddr *)&_server_address, (socklen_t)sizeof(_server_address)) < 0)
	{
		close(_socketfd);
		std::stringstream ss;
		ss << (getIp() >> 24) << "." << ((getIp() >> 16) & 255) << "." << ((getIp() >> 8) & 255) << "." << (getIp() & 255) << ":" << getPort();
		throw std::runtime_error("\033[1;31mhis ip:port is already in use: " + ss.str() + "\033[0m");
	}
}

void    Server::listenToClient()
{
	// listen at the port
	
	if (listen(_socketfd, MAX_CONNECTIONS) < 0)
	{
		std::stringstream ss;
		ss << _socketfd << " on port " << getPort() << "";
		throw std::runtime_error("Could not listen at socket " + ss.str());
	}
	std::cout << "server listening on ";
	std::cout << (getIp()	>> 24)			<< ".";
	std::cout << ((getIp()	>> 16)	& 255)	<< ".";
	std::cout << ((getIp()	>> 8)	& 255)	<< ".";
	std::cout << (getIp() 			& 255)	<< ":";
	std::cout <<  getPort() 				<< std::endl;
}


int	Server::getFd() const { return (_socketfd);}
int Server::getIp() const { return (_ip); };
int Server::getPort() const { return (_port); };

void	Server::setupIp() {
	memset(&_server_address, 0, sizeof(_server_address));
	_server_address.sin_family = AF_INET;
	_server_address.sin_port = htons(_port);
	_server_address.sin_addr.s_addr = htonl(_ip);
};
