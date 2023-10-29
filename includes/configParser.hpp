#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP


#include "main.hpp"
#include "serverBlock.hpp"

class configParser
{
    private:
        std::string confFilePath;
        bool        loadFile();
        void        parseBlock( const std::string & );
        void        parse(void);
        std::vector<serverBlock> serverBlocks;
    public:
        configParser( void );
        configParser( const std::string & );
        ~configParser(  );
        std::vector<serverBlock> getServerBlocks( void ) const{
            return serverBlocks;
        }
        std::string getConfigFilePath( void ) const;
        std::string getWorkingDir( void ) const;
        std::string getLogFile( void ) const;
        int         getMaxConnections( void ) const;
        int         getPort( void ) const;
        bool        getAutoIndex( void ) const;
        void        setLogFile( std::string );
        void        setConfigFilePath( std::string );
        void        setWorkingDir( std::string );
        void        setMaxConnections( int );
        void        setPort( int );
        void	checkKeyValue(const std::string &, const std::string &);
        std::string trim(const std::string& str);
        void        setAutoIndex( bool );
        void	exceptionsManager( std::string );
        class       notARegularFile: public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
        class       configFileIsEmpty: public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
        class       NotParsedWell: public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
};

#endif