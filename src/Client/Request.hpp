#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>


class Request
{
private:
    Request();
    std::string request;
    std::string method;
    std::string path;
    std::string mimeType;
    std::map<std::string, std::string> headers;
    std::vector<std::string> body;

public:
    Request(std::string req);
    void parseRequest();
    void    printRequest() const;
    const std::string &getMethod() const;
    const std::string &getPath() const;
    const std::string &getMimeType();
    ~Request();
};

#endif