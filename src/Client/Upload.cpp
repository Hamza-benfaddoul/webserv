#include "Upload.hpp"
#include "../../includes/main.hpp"

Upload::Upload(Request *req, int in_cpt, Location in_location) : request(req), cpt(in_cpt), location(in_location)
{

}

Upload::~Upload()
{
	std::cout << "the file should be removed is: " << this->filename << std::endl;
	unlink(this->filename.c_str());
	std::remove(this->filename.c_str());
}

void	Upload::createFile()
{
	std::stringstream ss;
	ss << this->cpt;
	std::string cptAsString = ss.str();
	filename = "www/uploads/file" + cptAsString;
	this->bodyContent.open(filename.c_str(), std::ios::in | std::ios::out | std::ios::trunc);
	if (!this->bodyContent.is_open())
	{
		std::cout << "<< !!!! >> the file was not created successfuly" << std::endl;
		return;
	}
}

// start the proccess of uploading files ...

void Upload::start()
{
	std::map<std::string, std::string> ourLocations = location.getLocationAttributes();

	if (ourLocations.find("cgi") != ourLocations.end() && ourLocations["cgi"] == "on")
	{
		char *env[] = 
		{
			strdup(std::string("REDIRECT_STATUS=100").c_str()),
			strdup(std::string("SCRIPT_FILENAME=" + this->filename).c_str()),
			strdup(std::string("REQUEST_METHOD=").c_str()),
			strdup(std::string("QUERY_STRING=").c_str()),
			strdup(std::string("HTTP_COOKIE=").c_str()),
			strdup(std::string("HTTP_CONTENT_TYPE=").c_str()),
			strdup(std::string("CONTENT_TYPE=").c_str()),
			NULL
		};

		

		(void)env;
	}
	else if (ourLocations.find("upload") != ourLocations.end() && ourLocations["upload"] == "on")
	{
		
	}
}

void    Upload::writeToFileString(const std::string &source, size_t size)
{
	bodyContent.write(source.data(), size);
}

void    Upload::writeToFileString(const std::string &source)
{
	this->bodyContent << source;
}

void Upload::writeToFile(const std::vector<char> &source, size_t end)
{
	bodyContent.write(source.data(), end);
}

void	Upload::writeToFile(const std::vector<char> & source)
{
	bodyContent.write(source.data(), source.size());
}

void Upload::endLine()
{
	this->bodyContent.flush();
}

