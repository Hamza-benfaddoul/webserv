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
#include "Upload.hpp"

// those are not exist :   414 - 413 - 301 - 201 - 405

#define ERROR403 "www/error/403.html"
#define ERROR404 "www/error/404.html"
#define ERROR400 "www/error/400.html"
#define ERROR501 "www/error/501.html"

class serverBlock;

class Client {
	private:
		Client();
		std::string 	_responseBuffer;
		size_t			_fd;
		Request			*request;
		Upload			*upload;
		static int 		cpt;
		long			readd;
		bool			_readHeader;
		serverBlock		*_serverBlock;
		bool			errorCheck;
		bool			fileCreated;
		bool			canIRead;
		int				totalBytesRead;
		int				Content_Length;

		bool			getMethodHandler(void);
		bool			postMethodHandler(void);
		bool			receiveResponse(void);
		bool			checkIfDirectoryIsLocation( std::string );
		void			sendResponse(void);
		void			sendErrorResponse( int, std::string, std::string );
		void			sendResponse1(std::string , int , std::string );
		void			closeConnection(void);
		void			serveImage(std::string);
		void			sendImageResponse(const std::string&, const std::string&);
		void			readFile( const std::string path );
		void			readChunkedBody();
		void			readBody();
		int				is_request_well_formed();
		void			handleRequestFromRoot();
		void			handleRequestFromLocation( std::string );
		std::string		getMimeTypeFromExtension(const std::string& path);


	public:
		Client(size_t fd, serverBlock *serverBlock);
		~Client();

		bool	run(void);
};