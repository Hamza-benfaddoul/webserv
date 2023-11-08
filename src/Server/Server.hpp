/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/25 09:55:20 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/08 12:05:53 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../includes/main.hpp"
#include "../Client/Client.hpp"
class Client;
class Server {
	public:
		Server(uint32_t ip=INADDR_ANY, unsigned short port=80);
		~Server();

		int	getFd() const;
		int	getIp1() const{
			return (_ip);
		};
		int	getPort() const{
			return (_port);
		};

		void run(void);
		void	initServerSocket(void);
		void	listenToClient(void);
		std::vector<Client*>	_clients;
	private:
		Server();

		void	acceptClientRequest(void);
		void	getIp(void);

		uint32_t			_ip;
		unsigned short		_port;
		int					_socketfd;   
		struct sockaddr_in	_server_address;
		struct sockaddr_in	_client_address;
};
