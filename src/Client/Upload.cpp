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
	std::map<std::string, std::string> ourHeaders = this->request->getHeaders();
	std::map<std::string, std::string>::const_iterator it = ourHeaders.find("Content-Type");

	std::string content_type;
	if (it != ourHeaders.end())
		content_type = it->second;
	std::cout << "the content type is: " << content_type << std::endl;
	if (ourLocations.find("cgi") != ourLocations.end() && ourLocations["cgi"] == "on")
	{
		char *env[] = 
		{
			strdup(std::string("REDIRECT_STATUS=200").c_str()),
			strdup(std::string("SCRIPT_FILENAME=" + this->request->getPath()).c_str()),
			strdup(std::string("REQUEST_METHOD=" + this->request->getMethod()).c_str()),
			strdup(std::string("QUERY_STRING=").c_str()),
			strdup(std::string("HTTP_COOKIE=").c_str()),
			strdup(std::string("HTTP_CONTENT_TYPE=" + content_type).c_str()),
			strdup(std::string("CONTENT_TYPE=" + content_type).c_str()),
			NULL
		};
		// Create an array of arguments for execve
		char* argv[] = 
		{
			strdup(std::string("/usr/bin/php").c_str()),
			strdup(std::string("cgi-bin/upload.php").c_str()), // here i need to take the path of cgi from the config file
			NULL
		};

		// Execute the PHP script
		pid_t pid = fork();
		int	fd_file = open(this->filename.data(), O_RDONLY);
		if (fd_file < 0)
			throw std::ios_base::failure("Failed to open file");
		if (pid == 0)
		{
			dup2(fd_file, 0);
			if (execve("/usr/bin/php", argv, env) == -1) {
				std::cerr << "Error executing PHP script.\n";
			}
		}
		bool envBool, argvBool = true;
		for (int i = 0; env[i] || argv[i]; i++)
		{
			if (envBool && env[i])
			{
				free(env[i]);
				envBool = false;
			}
			if (argvBool && argv[i])
			{
				free(argv[i]);
				argvBool = false;
			}	
		}
		// remove the file when pass to cgi
		// unlink(this->filename.c_str());
		std::remove(this->filename.c_str());
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

