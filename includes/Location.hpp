#pragma once

// #include "main.hpp"
#include "iostream"
#include "map"
#include "sys/stat.h"
#include <sstream>
#include <string>
#include <vector>

class Location {
private:
	std::string	locationPath;
	std::string	root;
	std::string	directoryIndex;
	std::string	methods;
	std::string	fastCgiPass;
	std::string	fastCgiindex;
	std::string	fastCgiParam;
	std::string	include;
	bool		autoIndex;
	std::map<std::string, std::string>	locationAttributes;
public:
	bool		isEmpty;
	std::string	directory;
	bool		GET;
	bool		POST;
	bool		DELETE;
	std::string	index;
	bool		hasIndex;
	bool		hasCGI;
	int	proxy_read_time_out;
	std::vector<std::pair<std::string, std::string> > cgi;
	Location();
	void			setAttribute( const std::string &key, std::string value ) ;
	std::map<std::string, std::string>	getLocationAttributes() const { return locationAttributes; }
	void			parseLocations( void );
	void			parseMethods( const std::string & );
	void			parseRoot( const std::string & );
	void			parseAutoIndex( const std::string & );
	void			parseIndex( std::string );
	void			parseCGI(  );
	void			parseProxyReadTimeOut( std::string );
	std::string		getKeyFromAttributes( std::string );
	void	toString( void ) const;

	void			exceptionsManager( std::string c );

	std::string&	rtrim(std::string& s, const char* t);
	std::string&	ltrim(std::string& s, const char* t);
	std::string&	trim(std::string& s);
	const std::string &getRoot( void ) const { return root; };
	// const std::string &getIndex( void ) const { return index}
	const std::string	&getMethods( void ) const { return methods; };
	bool				getAutoIndex( void ) const { return autoIndex; };
	const	std::string			&getLocationPath( void ) const { return locationPath; }

};
