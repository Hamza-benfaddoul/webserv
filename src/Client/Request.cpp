#include "../../includes/main.hpp"
#include "Request.hpp"

/*
    GET / HTTP/1.1
    Host: localhost:8081
    User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/117.0
    Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,;q=0.8
    Accept-Language: en-US,en;q=0.5
    Accept-Encoding: gzip, deflate, br
    Connection: keep-alive
    Upgrade-Insecure-Requests: 1
    Sec-Fetch-Dest: document
    Sec-Fetch-Mode: navigate
    Sec-Fetch-Site: none
    Sec-Fetch-User: ?1


*/

Request::Request(std::string req) : request(req)
{
}

Request::~Request()
{
}

void    Request::parseRequest()
{
    std::vector<std::string> elements;
    std::vector<std::string> firstLine;
    int i = 1;

    elements = ft_split(this->request, "\r\n");
    firstLine = ft_split(elements.at(0), " ");
    std::string reqMethod = firstLine.at(0);
    if (reqMethod == "GET" || reqMethod == "POST" || reqMethod == "DELETE")
        this->method = firstLine.at(0);
    else
        throw std::runtime_error("bad method requested");
    this->path = firstLine.at(1);
   size_t   pos;
    while (i < (int)elements.size())
    {
        pos = elements.at(i).find(": ");
        if (pos != std::string::npos)
        {
            std::vector<std::string> mapElements;
            mapElements = ft_split(elements.at(i), ": ");
            // std::cout << "elements at i: " << elements.at(i) << std::endl;
            this->headers[mapElements.at(0)] = mapElements.at(1);
        }
        else
            break;
        i++;
    }
    while (i < (int)elements.size())
    {
        this->body.push_back(elements.at(i));
        i++;
    }
}

void    Request::printRequest() const
{
    std::ofstream req("request.txt");

    
    req << "the method is: " << this->method << std::endl;
    req << "the path is: " << this->path << std::endl;
    req << "the headers are: " << std::endl;
    std::map<std::string, std::string>::const_iterator it;
    for (it = this->headers.begin(); it != this->headers.end(); ++it)
    {
        req << it->first << ": " << it->second << std::endl;
    }
    req.close();
}

const std::string &Request::getMethod() const { return method; }
const std::string &Request::getPath() const { return path; }
