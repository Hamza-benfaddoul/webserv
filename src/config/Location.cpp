#include "../../includes/Location.hpp"
#include "sys/stat.h"
Location::Location()
{
    root = "www";
    locationPath = "/";
}

void    Location::setAttribute( std::string key, std::string value ) { this->locationAttributes[key] = value; }

void    Location::parseLocations( void )
{
    std::map<std::string, std::string> location = getLocationAttributes();
    for (std::map<std::string, std::string>::iterator iterator = location.begin(); iterator != location.end(); iterator++)
    {
        // std::cout << "*" << iterator->first<< "*" << std::endl;
        if (iterator->first.compare("path") == 0)
            this->locationPath = iterator->second;
        else if (iterator->first.compare("root") == 0)
            parseRoot(iterator->second);
        else if (iterator->first.compare("methods") == 0)
            parseMethods(iterator->second);
			// parsePortNumber(iterator->second);
    	// std::cout << iterator->first << ": " << iterator->second << std::endl;
    }
}
void    Location::parseRoot( const std::string &root )
{
    std::cout << root << std::endl;
    if (!root.empty())
    {
        struct stat s;
        if( stat( root.c_str(), &s ) != 0 )
            exceptionsManager("cannot access " + root );
        else if( s.st_mode & S_IFDIR ){}  // S_ISDIR() doesn't exist on my windows 
            // exceptionsManager(root + " is a directory");
        else
            exceptionsManager(root +  " is not a directory");
    }
}

void    Location::parseMethods( const std::string &path )
{
    // remaining here !!!!!!
}

void	Location::exceptionsManager( std::string c )
{
	throw std::runtime_error("Error: " + c);
}