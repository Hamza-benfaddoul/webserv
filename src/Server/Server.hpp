/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/25 09:55:20 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/13 12:49:56 by hamza            ###   ########.fr       */
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
		serverBlock				*_serverBlock;
	private:
		Server();

		void	setupIp(void);
		void	initServerSocket(void);
		void	listenToClient(void);

		uint32_t			_ip;
		unsigned short		_port;
		int					_socketfd;   
		struct sockaddr_in	_server_address;
		struct sockaddr_in	_client_address;
};
