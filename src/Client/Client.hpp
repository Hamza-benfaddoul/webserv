/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:59:30 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/16 16:29:28 by hbenfadd         ###   ########.fr       */
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
		bool		_readHeader;

		Request		*request;
		serverBlock	*_serverBlock;
		bool	getMethodHandler(void);
		bool	postMethodHandler(void);
		bool	receiveResponse(void);
		void	sendResponse(void);
		void	sendErrorResponse( int, std::string, std::string );
		void	sendResponse1(std::string , int , std::string );
		void	closeConnection(void);
		void	serveImage(std::string);
		void	sendImageResponse(const std::string&, const std::string&);
		void	readFile( const std::string path );

		std::string	getMimeTypeFromExtension(const std::string& path);


	public:
		Client(size_t fd, serverBlock *serverBlock);
		~Client();

		bool	run(void);
};