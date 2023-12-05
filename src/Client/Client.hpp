/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rakhsas <rakhsas@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:59:30 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/11/19 17:38:38 by rakhsas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../includes/main.hpp"
#include "../../includes/Location.hpp"
#include "Request.hpp"
#include "Upload.hpp"
#include "../../includes/Location.hpp"




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
		Location	location;
		int		_fdFile;
		int		pipefd[2];
		std::stringstream tmpFile;
		long content_length;
		std::ifstream file_ouptut;
		std::string content;
		// Response	*response;
		std::string postRequest;
		std::vector<char> _responseBufferVector;
		static int 		cpt;
		long			readd;
		int				totalRead;
		bool	isRead;
		bool			errorCheck;
		bool			fileCreated;
		bool			canIStart;
		int				totalBytesRead;
		int				Content_Length;
		bool			_readHeader;
		serverBlock		*_serverBlock;
		bool 			isLocationExist;
		std::string 	body;
		std::map<std::string, std::string>	ourLocations;
		int	rest;
		std::string chunkSizeString;
		size_t chunkSizeInt;
		int pos;
		bool	isChunkComplete;
		bool	controller;

		bool	checkRequestPath(std::string);
		bool	getMethodHandler(void);
		bool	postMethodHandler(void);
		bool	deleteMethodHandler(void);
		bool	receiveResponse(void);
		bool	checkIfDirectoryIsLocation( std::string );
		bool	checkDir( std::string );
		bool	handleFiles( std::string );
		bool	handleDirs();
		bool	checkType();
		void	directoryListing(std::string);
		void	parseChunk();
		bool	readFile( const std::string, std::ifstream &);
		bool	serveImage();

		void	readFromCgi();
		void	sendResponse(void);
		void	sendRedirectResponse( int CODE, std::string ERRORTYPE, std::string location);
		void	sendErrorResponse( int, std::string, std::string );
		void	sendResponse1(std::string , int , std::string );
		void	closeConnection(void);
		void	sendImageResponse(const std::string&, const std::string&);
		void	readChunkedBody();
		void	readBody();
		int		is_request_well_formed();
		void	handleRequestFromRoot();
		void	handleRequestFromLocation( std::string );
		std::string	getErrorPage( int );
		std::string getCgiPath( std::string );
		Location	getCurrentLocation();

	public:
		Client(size_t fd, serverBlock *serverBlock);
		std::map<std::string, std::string> getOurLocations() const;
		~Client();

		bool	run(void);
};