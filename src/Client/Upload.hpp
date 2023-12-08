#ifndef UPLOAD_HPP
#define UPLOAD_HPP
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>

#include "../../includes/serverBlock.hpp"
class Request;
// class Client;

class Upload
{
private:
    Upload();
    std::fstream	bodyContent;
    Request *request;
    serverBlock *_serverBlock;
    int cpt;
    std::string filename;
    std::string cgi_output_filename;
    Location location;
    int     fd_socket;
    bool    forked;
    int	cgi_output_fd;
    pid_t pid;
    clock_t start_c;
    std::string cgi_output;
    clock_t end;
    bool hasCgi;
    std::string cgi_path;

    // Client client;

public:
    // Upload(Request *req, int in_cpt, Client in_client);
    // Upload(Request *req, int in_cpt, Location in_location, int in_fd, bool in_hasCgi);
    Upload(Request *req, int in_cpt, Location in_location, int in_fd, std::string in_cgi_path, serverBlock *serverBlock);
    // Upload(Request *req, int in_cpt);
    ~Upload();
    void	writeToFile(const std::vector<char> & source);
    void    writeToFileString(const std::string &source, size_t size);
    void    writeToFileString(const std::string &source);
    void    writeToFile(const std::vector<char> & source, size_t end);
    void    sendResponse(int CODE, std::string TYPE, std::string content, std::string c_type) ;
    void    endLine();
    void	createFile();
    bool    start();
    std::string	checkType(std::string path);
    std::string	getErrorPage( int errorCode );

};


#endif