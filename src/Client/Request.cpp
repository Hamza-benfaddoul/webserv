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

Request::Request(std::string req, std::vector<char> bv) : request(req), bodyVector(bv),bad(0)
{
}

bool Request::getBad() const
{
    return this->bad;
}

Request::~Request()
{
}

void    Request::parseRequest()
{
    std::string part1;
    std::string part2;
    std::vector<std::string> elements;
    std::vector<std::string> firstLine;
    size_t pos = this->request.find("\r\n\r\n");
    int i = 1;

    // part1 = this->request.substr(0, pos);
    part1 = this->request;
    // part2 = this->request.substr(pos + 4, this->request.length());
    elements = ft_split(part1, "\r\n");
    firstLine = ft_split(elements.at(0), " ");
    if ((int)firstLine.size() > 3)
    {
        bad = 1;
        return;
    }
    this->method = firstLine.at(0);
    this->path = firstLine.at(1);
    std::string reqMethod = firstLine.at(0);
    while (i < (int)elements.size())
    {
        pos = elements.at(i).find(": ");
        if (pos != std::string::npos)
        {
            std::vector<std::string> mapElements;
            mapElements = ft_split(elements.at(i), ": ");
            this->headers[mapElements.at(0)] = mapElements.at(1);
        }
        else
            break;
        i++;
    }
    elements.clear();
    // this->bodyString = part2;
}

void    Request::printRequest() const
{
    std::ofstream req("request.txt");
    req << this->request << std::endl << std::endl << std::endl;
    req << "the method is: " << this->method << std::endl;
    req << "the path is: " << this->path << std::endl;
    req << "the headers are: " << std::endl;
    std::map<std::string, std::string>::const_iterator it;
    for (it = this->headers.begin(); it != this->headers.end(); ++it)
    {
        req << it->first << ": " << it->second << std::endl;
    }
    req << std::endl << std::endl;
    // req << "-------------------------------------------------------" << std::endl << std::endl;
    req << this->bodyString;
    req << "in index 0 we have: ";
    req.put(bodyVector.at(0));
    // for (int i = 0; i < (int) bodyVector.size(); i++)
    // {
    //     if (bodyVector.at(i) == '\r' || bodyVector.at(i) == '\n')
    //         req.put('-');
    //     else
    //         req.put(bodyVector.at(i));
    // }
    req.close();
}

const std::string &Request::getMethod() const { return method; }
const std::string &Request::getPath() const { return path; }
const std::map<std::string, std::string> &Request::getHeaders() const {return this->headers; }
const std::vector<std::string> & Request::getBody() const { return this->body; }
const std::string &Request::getBodyString() const { return this->bodyString; }
const std::string &Request::getMimeType()
{
    std::istringstream iss(headers.find("Accept")->second);
    getline(iss, mimeType, ',');
    return mimeType;
}