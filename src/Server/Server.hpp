/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/25 09:55:20 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/10/25 14:56:39 by hbenfadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../includes/main.hpp"

class Server {
	public:
		Server(unsigned long ip=INADDR_ANY, unsigned short port=80);
		~Server();
		void run(void);
	private:
		Server();

		void	initServerSocket(void);
		void	listenToClient(void);
		void	acceptClientRequest(void);
		void	getIp(void);

		unsigned long		_ip;
		unsigned short		_port;
		int					_socketfd;   
		struct sockaddr_in	_server_address;
		struct sockaddr_in	_client_address;
};
