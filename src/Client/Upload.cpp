#include "Upload.hpp"
#include "../../includes/main.hpp"

Upload::Upload(Request *req, int in_cpt) : request(req), cpt(in_cpt)
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
	
}

void	Upload::writeToFile(const char *ptr, size_t size)
{
	bodyContent.write(ptr, size);
}