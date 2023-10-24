#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP


#include <iostream>
#include <fstream>

class configParser
{
    private:
        std::string confFilePath;
        std::string workingDir;
        std::string logFile;
        int         maxConnections;
        int         port;
        bool        autoIndex;
        bool        loadFile();
        void        parseLine( const std::string & );
    public:
        configParser( void );
        configParser( const std::string & );
        ~configParser(  );
        std::string getWorkingDir( void ) const;
        std::string getLogFile( void ) const;
        int         getMaxConnections( void ) const;
        int         getPort( void ) const;
        bool        getAutoIndex( void ) const;
        void        setLogFile( std::string );
        void        setWorkingDir( std::string );
        void        setMaxConnections( int );
        void        setPort( int );
        void        setAutoIndex( bool );
};



#endif