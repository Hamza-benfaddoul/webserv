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

class serverBlock
{
    private:
        // std::string serverName;
        std::string host;
        std::string root;
        int         port;
        bool        autoIndex;
        std::map<std::string, std::string> attributes;
        std::vector<std::map<std::string, std::string> > locations;
    public:
        void        parseBlock( );
        serverBlock();
        // serverBlock(int, std::string);
        void    setLocation(std::map<std::string, std::string>) ;
        void    setAttribute(std::string, std::string) ;
        std::string trim(const std::string& str, std::string sep);
        std::vector<std::map<std::string, std::string> > getLocations() const {
            return locations;
        }
        std::map<std::string, std::string> getAttributes() const {
            return attributes;
        }
        // void    parseServerName( std::string value );
        void    parsePortNumber( std::string value );
        void    parseRoot(std::string value) ;
        void    parseHost(std::string value);
        void    parseAutoIndex(std::string value);

        int     getPort(void) const;
        bool    getAutoIndex(void) const;
        std::string getRoot(void) const;
        // std::string getServerName(void) const;
        uint32_t getHost(void) const;
};

#endif