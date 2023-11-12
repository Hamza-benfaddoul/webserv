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

#define MAX_EVENTS  100

void Cluster::run(void)
{
	servers[0]->_clients.resize(MAX_EVENTS);
	struct epoll_event	ev, events[MAX_EVENTS];
	int					conn_sock, nfds, epollfd, n;

	epollfd = epoll_create(1);
	if (epollfd == -1) {
		throw std::runtime_error("epoll_create");
	}

	for (size_t i = 0; i < servers.size(); i++) {
		servers[i]->initServerSocket();
		servers[i]->listenToClient();
		ev.events = EPOLLIN;
		ev.data.fd = servers[i]->getFd();
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, servers[i]->getFd(), &ev) == -1) {
			throw std::runtime_error("epoll_ctl");
		}
	}
	for (;;) {
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (nfds == -1) {
			perror("epoll_wait");
			exit(EXIT_FAILURE);
		}

		for (n = 0; n < nfds; ++n) {
			if (events[n].data.fd <= servers.at(servers.size()- 1)->getFd()) {
				conn_sock = accept(events[n].data.fd,NULL, NULL);
				if (conn_sock == -1) {
					throw std::runtime_error("could not accept client");
				}
				std::cout << " conn_sock " << conn_sock << std::endl;
				servers[0]->_clients.at(conn_sock) = new Client(conn_sock,NULL);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = conn_sock;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
					throw std::runtime_error("epoll_ctl");
				}
			}
			else {
				std::cout << "run = " << events[n].data.fd<< std::endl;
				if (servers.at(0)->_clients.at(events[n].data.fd)->run()) // return true when client close the connection
				{
					epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, &ev);
					close(events[n].data.fd);
					delete servers.at(0)->_clients.at(events[n].data.fd);
				}
			}
		}
	}
}