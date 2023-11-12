#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>

class Response
{
private:
    Response();
    const Request request;
    int status_code;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    Response(const Request &req);
    ~Response();
};

#endif
