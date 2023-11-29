#include "Upload.hpp"
#include "../../includes/main.hpp"

Upload::Upload(Request *req, int in_cpt, Location in_location, int in_fd, std::string in_cgi_path) : request(req), cpt(in_cpt), location(in_location), fd_socket(in_fd), cgi_path(in_cgi_path)
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

std::string	Upload::checkType(std::string path)
{
	DIR *pDir;

	pDir = opendir ((path).c_str());
	if (pDir == NULL) {
		return std::string("file");
	}
	closedir (pDir);
	return std::string("folder");
}

bool Upload::start()
{
	std::map<std::string, std::string> ourLocations = location.getLocationAttributes();
	std::map<std::string, std::string> ourHeaders = this->request->getHeaders();
	std::map<std::string, std::string>::const_iterator it = ourHeaders.find("Content-Type");
	std::string content_type;
	std::string cgi_program_name;

	if (it != ourHeaders.end())
		content_type = it->second;
	// the cgi case.
	if (cgi_path.length() > 0)
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
			// discover the path of cgi script.
			// std::string cgi_path = "www/cgi-bin/upload.php"; // update this fromthe config file
			std::cout << "the location path: " << location.getLocationPath() << std::endl;
			std::string uri = request->getPath();
			std::string cgi_path_script = location.getRoot() + uri;
			int	cgi_fd = open(cgi_path_script.c_str(), O_RDONLY);
			if (cgi_fd < 0)
			{
				bool envBool = true;
				for (int i = 0; env[i]; i++)
				{
					if (envBool && env[i])
					{
						free(env[i]);
						envBool = false;
					}
				}

				this->bodyContent.close();
				sendResponse(404, "Not Found", "<html><body> <h1> 404 Not Found</h1> </body></html>", "text/html");
				return true;
			}
			close(cgi_fd);
			// Create an array of arguments for execve
			char* argv[] = 
			{
				strdup(cgi_path.c_str()),
				strdup(cgi_path_script.c_str()),
				NULL
			};

			// create the file where the out of cgi get stored
			std::stringstream ss;
			forked = true;
			ss << this->cpt;
			std::string cptAsString = ss.str();
			cgi_output_filename = "www/TempFiles/cgi_output" + cptAsString;
			cgi_output_fd = open(cgi_output_filename.c_str(), O_RDWR | O_CREAT, 0644);
			if (cgi_output_fd < 0)
				throw std::ios_base::failure("Failed to open file");
			// Execute the PHP script << fork, dup and execve >>
			int	fd_file = open(this->filename.data(), O_RDONLY);
			if (fd_file < 0)
				throw std::ios_base::failure("Failed to open file");
			start_c = clock();
			pid = fork();
			if (pid == 0) // the child proccess
			{
				dup2(cgi_output_fd, 1);
				dup2(fd_file, 0);
				close(fd_file);
				if (execve(cgi_path.c_str(), argv, env) == -1) {
					std::cerr << "Error executing PHP script.\n";
				}
				exit(0);
			}
			close(fd_file);
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
		if (forked == true) // here the cgi is allready runing so we must wait for hem until finished or (time out in case of error), also we must WNOHANG its a webserv you know :)
		{
			int retPid = waitpid(pid, NULL, WNOHANG);
			if (retPid == pid) // the child is done ==> the response could be success could be failed (depend on cgi output)
			{
				char	buffer[1024];
				int	readed;
				close(cgi_output_fd);
				cgi_output_fd = open(this->cgi_output_filename.c_str(), O_RDONLY);
				std::cout << "cgi_output_fd: " << cgi_output_fd << std::endl;
				while ((readed = read(cgi_output_fd, buffer, 1024)) > 0)
				{
					cgi_output.append(buffer, readed);
				}
				// std::cout << "--" << cgi_output << "--" << std::endl;
				size_t pos = cgi_output.find("\n\n");
				// std::cout << "the pos: " << pos << std::endl;
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
			else // calculate the time to live of the child proccess if > 60 means timeout();
			{
				end = clock();
				if (((double)(end - start_c)) / CLOCKS_PER_SEC > 60.0)
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