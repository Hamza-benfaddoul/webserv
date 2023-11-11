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
#include <unistd.h>
#include <vector>


Cluster::Cluster() {}

Cluster::Cluster( std::vector<serverBlock> serverBlocks)
{
    // create a servers
    for (std::vector<serverBlock>::iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it)
    {
        servers.push_back(new Server(it->getHost(), it->getPort(), &serverBlocks));
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
#include <sys/epoll.h>
#include <cstdlib>
#define MAX_EVENTS  10
void Cluster::run(void)
{

	//int		max_fd;
	fd_set	readfds;
	std::set<int> fd;

	FD_ZERO(&readfds);
	for (size_t i = 0; i < servers.size(); i++)
	{
//		if (fork())
		{
			servers[i]->initServerSocket();
			servers[i]->listenToClient();
			// add file discription of the client socket to set_fd 
			FD_SET(servers[i]->getFd(), &readfds);
			fd.insert(servers[i]->getFd());
		}
	}
	
	{
		struct epoll_event ev, events[MAX_EVENTS];
		int listen_sock = 3, conn_sock, nfds, epollfd, n;

		// Code to set up listening socket, 'listen_sock',
		//(socket(), bind(), listen()) omitted.  

		epollfd = epoll_create(1);
		if (epollfd == -1) {
			perror("epoll_create1");
			exit(EXIT_FAILURE);
		}

		ev.events = EPOLLIN;
		ev.data.fd = listen_sock;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
			perror("epoll_ctl: listen_sock");
			exit(EXIT_FAILURE);
		}

		ev.events = EPOLLIN;
		ev.data.fd = 4;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, 4, &ev) == -1) {
			perror("epoll_ctl: listen_sock");
			exit(EXIT_FAILURE);
		}

		for (;;) {
			nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
			if (nfds == -1) {
				perror("epoll_wait");
				exit(EXIT_FAILURE);
			}

			for (n = 0; n < nfds; ++n) {
				if (events[n].data.fd == listen_sock) {
					conn_sock = accept(listen_sock,NULL, NULL);
					if (conn_sock == -1) {
						perror("accept");
						exit(EXIT_FAILURE);
					}
					std::cout << "client socket " << conn_sock << std::endl;
				  	servers[0]->_clients.push_back(new Client(conn_sock, readfds,NULL));
					//setnonblocking(conn_sock);
					ev.events = EPOLLIN | EPOLLET;
					ev.data.fd = conn_sock;
					if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
								&ev) == -1) {
						perror("epoll_ctl: conn_sock");
						exit(EXIT_FAILURE);
					}
				}
				else if (events[n].data.fd == 4) {
					conn_sock = accept(4,NULL, NULL);
					if (conn_sock == -1) {
						perror("accept");
						exit(EXIT_FAILURE);
					}
					std::cout << "client socket " << conn_sock << std::endl;

				  	servers[0]->_clients.push_back(new Client(conn_sock, readfds,NULL));
					//setnonblocking(conn_sock);
					ev.events = EPOLLIN | EPOLLET;
					ev.data.fd = conn_sock;
					if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
						perror("epoll_ctl: conn_sock");
						exit(EXIT_FAILURE);
					}
				}
				else {
					std::cout << events[n].data.fd  << std::endl;
					std::cout << "n "<< n  << std::endl;
					servers.at(0)->_clients.at(events[n].data.fd - 6)->run();
					// epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, &ev);
					// close(events[n].data.fd);
					// delete servers.at(0)->_clients.at(events[n].data.fd - 6);
				}
			}
		}
	}
}