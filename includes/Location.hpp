#pragma once

// #include "main.hpp"
#include "iostream"
#include "map"
#include "sys/stat.h"
#include <sstream>
#include <string>

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
	bool		GET;
	bool		POST;
	bool		DELETE;
	bool		autoIndex;
	std::map<std::string, std::string>	locationAttributes;

public:
	Location();
	void			setAttribute( const std::string &key, const std::string &value ) ;
	std::map<std::string, std::string>	getLocationAttributes() const { return locationAttributes; }
	void			parseLocations( void );
	void			parseMethods( const std::string & );
	void			parseRoot( const std::string & );
	void			parseAutoIndex( const std::string & );
	void			parseIndex( const std::string & );

	void	toString( void ) const;

	void			exceptionsManager( std::string c );

	std::string&	rtrim(std::string& s, const char* t);
	std::string&	ltrim(std::string& s, const char* t);
	std::string&	trim(std::string& s);
	const std::string &getRoot( void ) const { return root; };
	// const std::string &getIndex( void ) const { return index}
	const std::string &getMethods( void ) const { return methods; };
	bool	getAutoIndex( void ) const { return autoIndex; };

};
