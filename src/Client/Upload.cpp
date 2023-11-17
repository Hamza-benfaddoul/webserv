#include "Upload.hpp"
#include "../../includes/main.hpp"

Upload::Upload(Request *req, int in_cpt) : request(req), cpt(in_cpt)
{
}

Upload::~Upload()
{
	unlink(this->filename.c_str());
	std::remove(this->filename.c_str());
}

void	Upload::createFile()
{
	std::stringstream ss;
	ss << this->cpt;
	std::string cptAsString = ss.str();
	filename = "www/bodyFiles/content" + cptAsString;
	this->bodyContent.open(filename.c_str(), std::ios::in | std::ios::out | std::ios::trunc);
	if (!this->bodyContent.is_open())
	{
		std::cout << "<< !!!! >> the file was not created successfuly" << std::endl;
		return;
	}
}

void Upload::start()
{
    // std::map <std::string, std::string> headers = this->request->getHeaders();
	// std::map<std::string , std::string>::const_iterator it = headers.find("Transfer-Encoding");
	
	// int pid = fork();
	// // REDIRECT_STATUS=100, SCRIPT_FILENAME, REQUEST_METHOD, QUERY_STRING, HTTP_COOKIE, HTTP_CONTENT_TYPE, CONTENT_TYPE
	// char *arr[] = 
	// {

	// };
	// if (pid == 0)
	// {
	// 	if (execve("cgi-bin/upload.php", arr, NULL) < 0)
	// 		std::cout << "wait what the script not found but realy ......................" << std::endl;
	// }
	// bodyContent.close();
	// unlink("www/bodyFiles/bodyContent");
}

void	Upload::writeToFile(const std::string &content)
{
	bodyContent << content;
}