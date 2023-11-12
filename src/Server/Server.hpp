/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/25 09:55:20 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/12 22:29:00 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../includes/main.hpp"
#include "../Client/Client.hpp"
class Client;
class serverBlock;
class Server {
	public:
		Server(uint32_t ip=INADDR_ANY, unsigned short port=80, serverBlock *serverBlock=NULL);
		~Server();

		int	getFd() const;
		int	getIp() const;
		int	getPort() const;

		void	run(void);
		void	initServerSocket(void);
		void	listenToClient(void);

		std::vector<Client*>	_clients;
		serverBlock				*_serverBlock;
	private:
		Server();

		void	acceptClientRequest(void);
		void	setupIp(void);

		uint32_t			_ip;
		unsigned short		_port;
		int					_socketfd;   
		struct sockaddr_in	_server_address;
		struct sockaddr_in	_client_address;
};
