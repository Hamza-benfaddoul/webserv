/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 17:58:17 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/10/29 18:40:56by hbenfadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/main.hpp"
#include <arpa/inet.h>


Cluster::Cluster() {}

Cluster::Cluster( std::vector<serverBlock> serverBlocks)
{
	// create a servers
	for (std::vector<serverBlock>::iterator it = serverBlocks.begin() + 1; it != serverBlocks.end(); ++it)
	{
		servers.push_back(new Server(it->getHost(), it->getPort()));
	}
	// run all servers
	run();
}

Cluster::~Cluster()
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		delete servers[i];
	}
}

void Cluster::run(void)
{

	int		max_fd;
	fd_set	readfds;
	std::set<int> fd;

	FD_ZERO(&readfds);
	for (size_t i = 0; i < servers.size(); i++)
	{
//		if (fork())
		{
			servers[i]->initServerSocket();
			servers[i]->listenToClient();
			// print message on the console 'server listening on ip:port'
			std::cout << "server listening on ";
			std::cout << (servers[i]->getIp1()	>> 24)			<< ".";
			std::cout << ((servers[i]->getIp1()	>> 16)	& 255)	<< ".";
			std::cout << (( servers[i]->getIp1() >> 8)	& 255)	<< ".";
			std::cout << (servers[i]->getIp1() 			& 255)	<< ":";
			std::cout << servers[i]->getPort() 					<< std::endl;
			// add file discription of the client socket to set_fd 
			FD_SET(servers[i]->getFd(), &readfds);
			fd.insert(servers[i]->getFd());
		}
	}
	max_fd = servers[servers.size() - 1]->getFd();
	std::cout << servers[servers.size() - 1]->getFd() << std::endl;
	std::cout << servers[servers.size() - 2]->getFd() << std::endl;

	// run the servers
	{

		while (true) {
			if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0)
			  throw std::runtime_error("could not select");
			for (int i = 3; i <= max_fd; i++) {
			  if (FD_ISSET(i, &readfds)) {
				int clientFd;
				if (i == 3 || i == 4) {
				  clientFd = accept(i, NULL, NULL);
				  if (clientFd < 0)
					throw std::runtime_error(
						"could not create socket for client");
				  FD_SET(clientFd, &readfds);
				  servers[0]->_clients.push_back(new Client(clientFd, readfds));
					FD_SET(servers[i - 3]->getFd(), &readfds);
					fd.insert(servers[i - 3]->getFd());
					max_fd = clientFd;
				} else {
				  servers[0]->_clients.at(0/*i - _socketfd - 1*/)->run();
				}
			  }
			}
		}
	}
}