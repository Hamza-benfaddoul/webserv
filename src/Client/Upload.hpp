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
    int cpt;
    std::string filename;
public:
    Upload(Request *req, int in_cpt);
    ~Upload();
    void	writeToFile(const char *ptr, size_t size);
    void	createFile();
    void    start();
};


#endif