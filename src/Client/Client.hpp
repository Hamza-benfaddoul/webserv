/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rakhsas <rakhsas@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:59:30 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/15 21:53:08 by rakhsas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../includes/main.hpp"
#include "Request.hpp"
#define ERROR403 "www/error/403.html"
#define ERROR404 "www/error/404.html"
class serverBlock;

class Client {
	private:
		Client();
		std::string _responseBuffer;
		size_t		_fd;
		Request	*request;
		// Response	*response;
		serverBlock *_serverBlock;
		long	read;

		bool	getMethodHandler(void);
		bool	postMethodHandler(void);
		bool	receiveResponse(void);
		bool	checkIfDirectoryIsLocation( std::string );

		void	sendResponse(void);
		void	sendErrorResponse( int, std::string, std::string );
		void	sendResponse1(std::string , int , std::string );
		void	closeConnection(void);
		void	serveImage(std::string);
		void	sendImageResponse(const std::string&, const std::string&);
		void	readFile( const std::string path );
		void	handleRequestFromRoot();
		void	handleRequestFromLocation( std::string );

		std::string	getMimeTypeFromExtension(const std::string& path);


	public:
		Client(size_t fd, serverBlock *serverBlock);
		~Client();

		bool	run(void);
};