#include "Upload.hpp"
#include "../../includes/main.hpp"

Upload::Upload(Request *req, int in_cpt, Location in_location, int in_fd) : request(req), cpt(in_cpt), location(in_location), fd_socket(in_fd)
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
	this->bodyContent.open(filename.c_str(), std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
	// this->bodyContent.open(filename.c_str(), std::ios::in | std::ios::out | std::ios::trunc);
	if (!this->bodyContent.is_open())
	{
		std::cout << "<< !!!! >> the file was not created successfuly" << std::endl;
		return;
	}
}

// start the proccess of uploading files ...


void	Upload::sendResponse(int CODE, std::string TYPE, std::string content, std::string c_type) 
{
	std::stringstream response;
	response << "HTTP/1.1 " << CODE << " " << TYPE << "\r\n";
	response << "Content-Type: " << c_type <<"; charset=UTF-8\r\n";
	response << "Content-Length: " << content.length() << "\r\n";
	response << "\r\n";
	response << content;

	write(this->fd_socket, response.str().c_str(), response.str().length());
}



bool Upload::start()
{
	std::map<std::string, std::string> ourLocations = location.getLocationAttributes();
	std::map<std::string, std::string> ourHeaders = this->request->getHeaders();
	std::map<std::string, std::string>::const_iterator it = ourHeaders.find("Content-Type");
	double timeUsed;
	std::string content_type;
	// bool	returnValue;

	if (it != ourHeaders.end())
		content_type = it->second;
	// the cgi case.
	if (ourLocations.find("cgi") != ourLocations.end() && ourLocations["cgi"] == "on")
	{
		// Create an array of envirment that cgi need.
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
		std::string cgi_path = "cgi-bin/upload.php"; // update this fromthe config file
		char* argv[] = 
		{
			strdup(std::string("/usr/bin/php").c_str()),
			strdup(cgi_path.c_str()),
			NULL
		};

		// create the file where the out of cgi get stored
		std::stringstream ss;
		ss << this->cpt;
		std::string cptAsString = ss.str();
		cgi_output_filename = "www/TempFiles/cgi_output" + cptAsString;
		int	cgi_output_fd = open(cgi_output_filename.c_str(), O_RDWR | O_CREAT);
		if (cgi_output_fd < 0)
			throw std::ios_base::failure("Failed to open file");
		// Execute the PHP script << fork, dup and execve >>
		pid_t pid = fork();
		int	fd_file = open(this->filename.data(), O_RDONLY);
		if (fd_file < 0)
			throw std::ios_base::failure("Failed to open file");
		if (pid == 0) // the child proccess
		{
			dup2(cgi_output_fd, 1);
			dup2(fd_file, 0);
			close(fd_file);
			timeUsed = ((double)clock() / CLOCKS_PER_SEC);
			if (execve("/usr/bin/php", argv, env) == -1) {
				std::cerr << "Error executing PHP script.\n";
			}
		}
		close(fd_file);
		close(cgi_output_fd);
		if (waitpid(pid, NULL, 0) == pid) // 0 for no hange, Success case ==> build response and send it.
		{
			// ...
		}
		else // calculate the time to live of the child proccess if > 5 means timeout();
		{
			double currentTime = ((double)clock() / CLOCKS_PER_SEC);
			if (currentTime - timeUsed > 5)
			{
				kill(pid, SIGKILL);
				this->bodyContent.close();
				// send respone time out !!!!!
			}
		}

		// free all ressources of argv and env.
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
		// remove the file when pass to cgi (files that have name: file{0, +inf}).
		std::remove(this->filename.c_str());
		std::cout << "send it " << std::endl;
		sendResponse(200, "OK", "<html><body><h1>200 Success</h1></body></html>", "text/html");
		return (true);
		// std::remove(cgi_output_filename.c_str());
	}
	// the case where the cgi is of but the upload is on.
	else if (ourLocations.find("upload") != ourLocations.end() && ourLocations["upload"] == "on")
	{
		std::vector<std::string> splitedContentType = ft_split(content_type, "/");
		std::string extension = splitedContentType.at(1);
		std::string newFileName = this->filename + "." + extension;
		int	resRename = std::rename(this->filename.c_str(), newFileName.c_str());
		if (resRename != 0)
			throw std::runtime_error("Failed to upload file");
		sendResponse(200, "OK", "<html><body><h1>200 Success</h1></body></html>", "text/html");
		return (true);
	}
	return (false);
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

