#include "Upload.hpp"
#include "../../includes/main.hpp"

Upload::Upload(Request *req, int in_cpt) : request(req), cpt(in_cpt)
{
}

Upload::~Upload()
{
	// unlink(this->filename.c_str());
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
    std::map <std::string, std::string> headers = this->request->getHeaders();
	std::map<std::string , std::string>::const_iterator it = headers.find("Transfer-Encoding");
	
	if (it != headers.end())
		readChunkedBody();
	else
		readBody();
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

void Upload::readChunkedBody()
{
	std::vector<std::string> body = this->request->getBody();
	std::string	mergedBody;
	int	bodyLength;

	for (int i = 1; i < (int)body.size(); i++)
	{
		if (i != (int)body.size() - 1 && i % 2 != 0)
			mergedBody += body.at(i);
	}
	bodyLength = mergedBody.length();
	if (bodyLength < 1024 * 2)
	{
		bodyContent << mergedBody;
	}
	else
	{
		int	iter = bodyLength / 1024;
		std::cout << "the iter is : " << iter << std::endl;
		for (int i = 0; i < iter; i++)
		{
			if (mergedBody.length() > 1024 * 2)
			{
				std::string toWrite = mergedBody.substr(0, 1024);
				bodyContent << toWrite;
				mergedBody = mergedBody.substr(1024);
			}
			else
			{
				bodyContent << mergedBody;
				break;
			}
		}
		bodyContent << std::endl;
	}
}

void Upload::readBody()
{
	std::map<std::string, std::string> headers = this->request->getHeaders();
	int	content_length = strtod(headers["Content-Length"].c_str(), NULL);

	if (content_length <= 1024 * 2)
	{
		bodyContent << this->request->getBodyString() << std::endl;
	}
	else
	{
		std::string cloneBody = this->request->getBodyString();
		int	iter = content_length / 1024;
		for (int i = 0; i < iter; i++)
		{
			if (cloneBody.length() > 1024 * 2)
			{
				std::string toWrite = cloneBody.substr(0, 1024);
				bodyContent << toWrite;
				cloneBody = cloneBody.substr(1024);
			}
			else
			{
				bodyContent << cloneBody;
				break;
			}
		}
		bodyContent << std::endl;
	}

}


void	Upload::chunked()
{

}

void	Upload::binary()
{

}