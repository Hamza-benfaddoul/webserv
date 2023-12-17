/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/25 09:55:20 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/12/17 13:01:49 by hbenfadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../includes/main.hpp"
#include "../Client/Client.hpp"
class Client;
class serverBlock;

class Server {
	public:
		Server(uint32_t ip=0, unsigned short port=80,std::string serverName="", serverBlock *serverBlock=NULL);
		~Server();

		int	getFd() const;
		int	getIp() const;
		int	getPort() const;

		int	run(void);
		serverBlock				*_serverBlock;
	private:
		Server();

		void	setupIp(void);
		void	initServerSocket(void);
		void	listenToClient(void);

		uint32_t			_ip;
		unsigned short		_port;
		std::string 		_serverName;
		int					_socketfd;   
		struct sockaddr_in	_server_address;
		struct sockaddr_in	_client_address;
};
