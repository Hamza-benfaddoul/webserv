#ifndef UPLOAD_HPP
#define UPLOAD_HPP
#include <fstream>
#include <vector>

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
    void	writeToFile(const std::vector<char> & source);
    void    writeToFileString(const std::string &source);
    void    endLine();
    void	createFile();
    void    start();
};


#endif