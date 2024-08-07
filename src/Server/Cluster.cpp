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
#include <vector>

Cluster::Cluster() {}

Cluster::Cluster( std::vector<serverBlock> serverBlocks)
{
	// create a _servers
	for (std::vector<serverBlock>::iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it)
	{
		_servers.push_back(new Server(it->getHost(), it->getPort(),it->getServerName(), &(*it)));
	}
	// run all _servers
	this->run();
}

Cluster::~Cluster()
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		delete _servers.at(i);
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients.at(i) != NULL)
			delete _clients.at(i);
	}
}

#define MAX_EVENTS  1024
#include <set>
void Cluster::run(void)
{
	std::set<int> server_fds;
	_clients.resize(MAX_EVENTS);
	struct epoll_event	ev, events[MAX_EVENTS];
	int					client_fd, nfds, epollfd, n;

	epollfd = epoll_create(1);
	if (epollfd == -1) {
		throw std::runtime_error("epoll_create");
	}

	for (size_t i = 0; i < _servers.size(); i++) {
		if (_servers[i]->run())
		{
			server_fds.insert(_servers[i]->getFd());
			ev.events = EPOLLIN | EPOLLOUT;
			ev.data.fd = _servers[i]->getFd();
			if (epoll_ctl(epollfd, EPOLL_CTL_ADD, _servers[i]->getFd(), &ev) == -1)
			{
				throw std::runtime_error("epoll_ctl");
			}
		}
	}
	if(server_fds.empty())
		exit(1);
	for (;;) {
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (nfds == -1) {
			perror("epoll_wait");
			exit(EXIT_FAILURE);
		}

		for (n = 0; n < nfds; ++n) {
			if (server_fds.find(events[n].data.fd) != server_fds.end()){
				client_fd = accept(events[n].data.fd,NULL, NULL);
				if (client_fd == -1) {
					throw std::runtime_error("could not accept client");
				}
				if (client_fd >= (int)_clients.size()) {
					_clients.resize(client_fd + 1);
				}
				_clients.at(client_fd) = new Client(client_fd,_servers, events[n].data.fd - *server_fds.begin()); 
				ev.events = EPOLLIN | EPOLLOUT;
				ev.data.fd = client_fd;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
					throw std::runtime_error("epoll_ctl");
				}
			}
			else if (events[n].data.fd >= 4){
				if (_clients.at(events[n].data.fd)->run()) // return true when client close the connection
				{
					// std::cout << "client allh irahmo\n";
					epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, &ev);
					close(events[n].data.fd);
					delete _clients.at(events[n].data.fd);
					_clients.at(events[n].data.fd) = NULL;
				}
			}
		}
	}
}
