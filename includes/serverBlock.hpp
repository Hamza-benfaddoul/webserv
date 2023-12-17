#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <exception>
#include <cstring>
#include <vector>
#include <map>
#include <sys/stat.h>
#include <fstream>
#include "stdint.h"
#include "unistd.h"
#include "Location.hpp"

class serverBlock
{
	private:
		std::string root;
		int         port;
		bool        autoIndex;
		std::string	serverName;
		std::map<std::string, std::string> attributes;
	public:
		std::string host;
		long	client_max_body_size;
		std::vector<Location> locations;
		std::vector<std::pair<int, std::string> > errorPages;
		serverBlock();
		Location getLocationByPath(std::string);
		void    setLocation( const Location &e ) ;
		void    setAttribute(std::string, std::string) ;
		std::vector<Location> getLocations() const {
			return locations;
		}
		std::string trim(const std::string& str, std::string sep);
		void    parseBlock( );
		void    parsePortNumber( std::string value );
		void    parseRoot( std::string value );
		void    parseHost( std::string value );
		void    parseServerName( std::string value );
		
		void    parseAutoIndex( std::string value );
		void	parseClientMaxBodySize( std::string value );

		int     getPort(void) const;
		bool    getAutoIndex(void) const;
		std::string getRoot(void) const;
		std::string getServerName(void) const;
		uint32_t getHost(void) const;

		std::map<std::string, std::string> getAttributes() const {
			return attributes;
		}
		bool    isEmpty() const;
};

#endif