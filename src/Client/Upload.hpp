#ifndef UPLOAD_HPP
#define UPLOAD_HPP
#include <fstream>

class Request;

class Upload
{
private:
    Upload();
    std::fstream	bodyContent;
    Request *request;
public:
    Upload(Request *req);
    ~Upload();
    void    start();
    void    readChunkedBody();
    void    readBody();
};


#endif