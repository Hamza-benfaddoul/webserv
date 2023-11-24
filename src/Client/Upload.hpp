#ifndef UPLOAD_HPP
#define UPLOAD_HPP
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../../includes/Location.hpp"
class Request;
// class Client;

class Upload
{
private:
    Upload();
    std::fstream	bodyContent;
    Request *request;
    int cpt;
    std::string filename;
    Location location;
    // Client client;

public:
    // Upload(Request *req, int in_cpt, Client in_client);
    Upload(Request *req, int in_cpt, Location in_location);
    // Upload(Request *req, int in_cpt);
    ~Upload();
    void	writeToFile(const std::vector<char> & source);
    void    writeToFileString(const std::string &source, size_t size);
    void    writeToFileString(const std::string &source);
    void    writeToFile(const std::vector<char> & source, size_t end);
    void    endLine();
    void	createFile();
    void    start();
};


#endif