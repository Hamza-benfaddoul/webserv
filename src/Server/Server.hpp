/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/25 09:55:20 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/10 08:34:25 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../includes/main.hpp"
#include "../Client/Client.hpp"
#include <vector>
class Client;
class serverBlock;
class Server {
	public:
		Server(uint32_t ip=INADDR_ANY, unsigned short port=80, std::vector<serverBlock> *serverBlock=NULL);
		~Server();
		void run(void);
	private:
		Server();

		void	initServerSocket(void);
		void	listenToClient(void);
		void	acceptClientRequest(void);
		void	getIp(void);

		uint32_t			_ip;
		unsigned short		_port;
		size_t				_socketfd;   
		struct sockaddr_in	_server_address;
		struct sockaddr_in	_client_address;
		std::vector<serverBlock> *_serverBlock;

		std::vector<Client*>	_clients;
};
