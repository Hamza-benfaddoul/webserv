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
    std::string path;
    std::string mimeType;
    std::map<std::string, std::string> headers;
    std::vector<std::string> body;
    std::string bodyString;
    bool    bad;

public:
    std::string method;
    Request(std::string req);
    void parseRequest();
    void    printRequest() const;
    const std::string &getMethod() const;
    const std::string &getPath() const;
    const std::string &getMimeType();
    const std::map<std::string, std::string > &getHeaders() const;
    const std::vector<std::string> & getBody() const;
    const std::string& getBodyString() const;
    int getCounter();
    bool getBad() const;
    ~Request();
};

#endif