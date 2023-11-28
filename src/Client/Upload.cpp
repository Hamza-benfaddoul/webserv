#include "Upload.hpp"
#include "../../includes/main.hpp"

Upload::Upload(Request *req, int in_cpt, Location in_location, int in_fd) : request(req), cpt(in_cpt), location(in_location), fd_socket(in_fd)
{
	forked = false;
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
	std::string newContent = content.substr(0, content.length());
	response << newContent;

	write(this->fd_socket, response.str().c_str(), response.str().length());
}



bool Upload::start()
{
	std::map<std::string, std::string> ourLocations = location.getLocationAttributes();
	std::map<std::string, std::string> ourHeaders = this->request->getHeaders();
	std::map<std::string, std::string>::const_iterator it = ourHeaders.find("Content-Type");
	std::string content_type;
	// bool	returnValue;

	if (it != ourHeaders.end())
		content_type = it->second;
	// the cgi case.
	
	if (ourLocations.find("cgi") != ourLocations.end() && ourLocations["cgi"] == "on")
	{
		if (forked == false)
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
			cgi_output_fd = open(cgi_output_filename.c_str(), O_RDWR | O_CREAT, 0644);
			std::cout << "not here " << cgi_output_fd << std::endl;
			if (cgi_output_fd < 0)
				throw std::ios_base::failure("Failed to open file");
			// Execute the PHP script << fork, dup and execve >>
			int	fd_file = open(this->filename.data(), O_RDONLY);
			if (fd_file < 0)
				throw std::ios_base::failure("Failed to open file");
			pid = fork();
			start_c = clock();
			if (pid == 0) // the child proccess
			{
				dup2(cgi_output_fd, 1);
				dup2(fd_file, 0);
				close(fd_file);
				if (execve("/usr/bin/php", argv, env) == -1) {
					std::cerr << "Error executing PHP script.\n";
				}
				exit(0);
			}
			close(fd_file);
			forked = true;
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
		}
		if (forked == true)
		{
			int retPid = waitpid(pid, NULL, WNOHANG);
			if (retPid == pid) // the child is done ==> the response could be success could be failed
			{
				char	buffer[1024];
				int	readed;
				close(cgi_output_fd);
				int new_fd = open(this->cgi_output_filename.c_str(), O_RDONLY);
				while ((readed = read(new_fd, buffer, 1024)) > 0)
				{
					cgi_output.append(buffer, readed);
				}
				size_t pos = cgi_output.find("\n\n");
				std::string body_cgi = cgi_output.substr(pos + 2, cgi_output.length());
				
				std::vector<std::string> splited_cgi_output = ft_split(cgi_output.substr(0, pos), "\n");
				for (int i = 0; i < (int)splited_cgi_output.size(); i++)
				{
					write(this->fd_socket, splited_cgi_output.at(i).c_str(), splited_cgi_output.at(i).length());
					write(this->fd_socket, "\r\n", 2);
				}
				write(this->fd_socket, "\r\n", 2);
				splited_cgi_output.clear();
				splited_cgi_output = ft_split(body_cgi, "\n");
				for (int i = 0; i < (int)splited_cgi_output.size(); i++)
				{
					write(this->fd_socket, splited_cgi_output.at(i).c_str(), splited_cgi_output.at(i).length());
					write(this->fd_socket, "\r\n", 2);
				}
			}
			else // calculate the time to live of the child proccess if > 20 means timeout();
			{
				// double currentTime = ((double)clock() / CLOCKS_PER_SEC);
				end = clock();
				if (((double)(end - start_c)) / CLOCKS_PER_SEC > 20.0)
				{
					// send respone time out !!!!!
					kill(pid, SIGKILL);
					close(cgi_output_fd);
					this->bodyContent.close();
					std::remove(this->filename.c_str());
					std::remove(cgi_output_filename.c_str());
					sendResponse(408, "Request Timeout", "<html><body><h1>408 Request Timeout</h1></body></html>", "text/html");
					return (true);
				}
				return false;
			}
		}
		// remove the file when pass to cgi (files that have name: file{0, +inf}).
		close(cgi_output_fd);
		this->bodyContent.close();
		std::remove(this->filename.c_str());
		std::remove(cgi_output_filename.c_str());
		return (true);
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
	else
	{
		// 403 Forbidden
		sendResponse(403, "Forbidden", ERROR403, "text/html");
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

