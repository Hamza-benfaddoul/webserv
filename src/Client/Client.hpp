/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rakhsas <rakhsas@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:59:30 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/17 21:04:04 by rakhsas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../includes/main.hpp"
#include "Request.hpp"
#include "Upload.hpp"

// those are not exist :   414 - 413 - 301 - 201 - 405

#define ERROR403 "www/error/403.html"
#define ERROR404 "www/error/404.html"
#define ERROR301 "www/error/301.html"
#define ERROR302 "www/error/302.html"
#define ERROR405 "www/error/405.html"
#define ERROR400 "www/error/400.html"
#define ERROR501 "www/error/501.html"

class serverBlock;

class Client {
	private:
		Client();
		std::string _responseBuffer;
		size_t		_fd;
		Request	*request;
		Upload	*upload;
		// Response	*response;
		std::string postRequest;
		std::vector<char> _responseBufferVector;
		static int 		cpt;
		long			readd;
		int				totalRead;
		bool			errorCheck;
		bool			fileCreated;
		bool			canIStart;
		int				totalBytesRead;
		int				Content_Length;
		bool			_readHeader;
		serverBlock		*_serverBlock;
		bool 			isLocationExist;
		std::string 	body;
		std::map<std::string, std::string>	ourLocation;
		int	rest;
		std::string chunkSizeString;
		size_t chunkSizeInt;
		int pos;
		bool	isChunkComplete;

		bool	checkRequestPath(std::string);
		bool	getMethodHandler(void);
		bool	postMethodHandler(void);
		bool	receiveResponse(void);
		bool	checkIfDirectoryIsLocation( std::string );
		bool	checkDir( std::string );
		bool	handleFiles( std::string );
		bool	handleDirs();
		bool	checkType();
		void	directoryListing(std::string);
		void	parseChunk();

		void	sendResponse(void);
		void	sendRedirectResponse( int CODE, std::string ERRORTYPE, std::string location);
		void	sendErrorResponse( int, std::string, std::string );
		void	sendResponse1(std::string , int , std::string );
		void	closeConnection(void);
		void	serveImage(std::string);
		void	sendImageResponse(const std::string&, const std::string&);
		void	readFile( const std::string path );
		void	readChunkedBody();
		void	readBody();
		int		is_request_well_formed();
		void	handleRequestFromRoot();
		void	handleRequestFromLocation( std::string );
		void 	get_match_location_for_request_uri(const std::string &uri);
		std::string	getMimeTypeFromExtension(const std::string& path);


	public:
		Client(size_t fd, serverBlock *serverBlock);
		~Client();

		bool	run(void);
};