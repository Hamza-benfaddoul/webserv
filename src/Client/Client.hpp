/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamza <hamza@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:59:30 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/10 08:45:44 by hamza            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../includes/main.hpp"
#include "Request.hpp"
class serverBlock;

class Client {
	private:
		Client();
		std::string _responseBuffer;
		size_t		_fd;
		fd_set		&_readfds;
		Request	*request;
		// Response	*response;
		std::vector<serverBlock> *_serverBlock;
		void	getMethodHandler(void);
		void	postMethodHandler(void);
		void	receiveResponse(void);
		void	sendResponse(void);
		void	sendErrorResponse( int, std::string, std::string );
		void    sendResponse1(std::string , int , std::string );
		void	closeConnection(void);
		void	serveImage(std::string);
		void sendImageResponse(const std::string&, const std::string&);
		std::string readFile( const std::string path );

		

	public:
		Client(size_t fd, fd_set &readfds, std::vector<serverBlock> *serverBlock);
		~Client();
		void	run(void);
};