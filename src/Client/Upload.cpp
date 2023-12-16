#include "Upload.hpp"
#include "../../includes/main.hpp"

Upload::Upload(Request *req, int in_cpt, Location in_location, int in_fd, std::string in_cgi_path, serverBlock *serverBlock)
	: request(req), _serverBlock(serverBlock) ,cpt(in_cpt), fd_socket(in_fd), cgi_path(in_cgi_path) , location(in_location)
{
	forked = false;
	max_body_size = _serverBlock->client_max_body_size;
}

Upload::~Upload()
{
	// unlink(this->filename.c_str());
	std::remove(this->filename.c_str());
}

std::string	Upload::getErrorPage( int errorCode ) {
	for(size_t i = 0; i < _serverBlock->errorPages.size(); i++)
	{
		if (_serverBlock->errorPages.at(i).first == errorCode)
			return _serverBlock->errorPages.at(i).second;
	}
	std::stringstream ss;
	ss << errorCode;
	return "www/error/" + ss.str() + ".html";
}

void	Upload::createFile()
{
	std::stringstream ss;
	ss << this->cpt;
	std::string cptAsString = ss.str();
	filename = "www/uploads/file" + cptAsString;
	this->bodyContent.open(filename.c_str(), std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
	if (!this->bodyContent.is_open())
	{
		throw std::ios_base::failure("Failed to open file");
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
			if ((long)totalBodySize > max_body_size)
			{
				std::remove(this->filename.c_str());
				sendErrorResponse(413, "Request Entity Too Large", ERROR413, this->fd_socket);
				return true;
			}
			std::stringstream streamFileSize;
			streamFileSize << totalBodySize;
			forked = true;
			std::string uri = request->getPath();
			std::string cgi_path_script = location.getRoot() + uri;
			// Create an array of envirment that cgi need.
			char *env[] =
			{
				strdup(std::string("REDIRECT_STATUS=100").c_str()),
				strdup(std::string("SCRIPT_FILENAME=" + cgi_path_script).c_str()),
				strdup(std::string("REQUEST_METHOD=" + this->request->getMethod()).c_str()),
				strdup(std::string("HTTP_COOKIE=").c_str()),
				strdup(std::string("HTTP_CONTENT_TYPE=" + content_type).c_str()),
				strdup(std::string("CONTENT_TYPE=" + content_type).c_str()),
				(content_type == "application/x-www-form-urlencoded") ? strdup(std::string("CONTENT_LENGTH=" + streamFileSize.str()).c_str()) : NULL,
				// strdup(std::string("CONTENT_LENGTH=" + streamFileSize.str()).c_str()),
				NULL
			};
			// discover the path of cgi script.

			// check if the script (cgi) is regular (exist and the path is valid)
			struct stat fileStat;
    		bool is_file = (stat(cgi_path_script.c_str(), &fileStat) == 0) && S_ISREG(fileStat.st_mode);
			if (is_file == false)
			{
				for (int i = 0; env[i]; i++)
				{
					if (env[i])
						free(env[i]);
				}
				this->bodyContent.close();
				std::remove(this->filename.c_str());
				sendErrorResponse(414, "Request-URI Too Long", getErrorPage(414), this->fd_socket);
				return true;
			}
			// Create an array of arguments for execve
			char* argv[] =
			{
				strdup(cgi_path.c_str()),
				strdup(cgi_path_script.c_str()),
				NULL
			};
			// create the file where the out of cgi get stored
			std::stringstream ss;
			// ss << (clock() / CLOCKS_PER_SEC);
			ss << cpt;
			cgi_output_filename = "www/TempFiles/cgi_output" + ss.str();
			start_c = clock();
			pid = fork();
			if (pid == 0) // the child proccess
			{
				if (freopen(cgi_output_filename.c_str(), "w", stdout) == NULL)
					throw std::ios_base::failure("Failed to open file");
				if (freopen(this->filename.c_str(), "r", stdin) == NULL)
					throw std::ios_base::failure("Failed to open file");
				if (execve(cgi_path.c_str(), argv, env) == -1) {
					std::cerr << "Error executing script.\n";
				}
				exit(0);
			}
			// free all ressources of argv and env.
			for (int i = 0; env[i] != NULL; i++)
				free(env[i]);
			for (int i = 0; argv[i] != NULL; i++)
				free(argv[i]);

		}
		if (forked == true) // here the cgi is allready runing so we must wait for hem until finished or (time out in case of error), also we must WNOHANG its a webserv you know :)
		{
			int	state;
			int retPid = waitpid(pid, &state, WNOHANG);
			if (retPid == pid) // the child is done ==> the response could be success could be failed (depend on cgi output)
			{
				char	buffer[1024];
				std::fstream cgi_output_content;
				cgi_output_content.open(cgi_output_filename.c_str(), std::ios::in);
				if(!cgi_output_content.is_open())
					throw std::ios_base::failure("Failed to open fileeeeeeeeeeee");
				// start reading from the file -------------------
				while (1)
				{
					cgi_output_content.read(buffer, 1024);
					cgi_output.append(buffer, cgi_output_content.gcount());
					if (cgi_output_content.eof())
					 	break;
				}
				if (cgi_output_content.fail() && !cgi_output_content.eof()) {
					std::cerr << "Error reading from file." << std::endl;
				}
				// end reading from the file ---------------------
				ltrim(cgi_output, "\r\n");
				size_t pos = cgi_output.find("\r\n\r\n");
				std::string bodyCgi;
				if (pos == std::string::npos)
				{
					pos = cgi_output.find("\n\n");
					bodyCgi = cgi_output.substr(pos + 2);
				}
				else
					bodyCgi = cgi_output.substr(pos + 4);
				std::string headers = cgi_output.substr(0, pos);
				std::stringstream result;
				std::vector<std::string> splitedHeaders = ft_split(headers, "\r\n");
				if (state == 0 && cgi_output.find("Location: ") != std::string::npos)
				{
					std::string status = splitedHeaders.at(0).substr(8);
					result << "HTTP/1.1 ";
					result << status;
					result << "\r\n";
					for (int i = 1; i < (int)splitedHeaders.size(); i++)
					{
						if (splitedHeaders.at(i) != "\n")
						{
							result << splitedHeaders.at(i);
						}
						result << "\r\n";
					}
					result << "\r\n";
					result << bodyCgi;
				}
				else if (state == 0)
				{
					result << "HTTP/1.1 200 OK\r\n";
					for (int i = 0; i < (int)splitedHeaders.size(); i++)
					{
						if (splitedHeaders.at(i) != "\n")
						{
							result << splitedHeaders.at(i);
						}
						result << "\r\n";
					}
					result << "\r\n";
					result << bodyCgi;
				}
				else
				{
					result << "HTTP/1.1 500 Internal Server Error\r\n";
					for (int i = 0; i < (int)splitedHeaders.size(); i++)
					{
						if (splitedHeaders.at(i) != "\n")
						{
							result << splitedHeaders.at(i);
						}
						result << "\r\n";
					}
					result << "\r\n";
					result << bodyCgi;
				}
				write(fd_socket, result.str().c_str(), result.str().length());
			}
			else
			{
				end = clock();
				if (((double)(end - start_c)) / CLOCKS_PER_SEC > (double)location.proxy_read_time_out)
				{
					kill(pid, SIGKILL);
					close(cgi_output_fd);
					this->bodyContent.close();
					std::remove(this->filename.c_str());
					std::remove(cgi_output_filename.c_str());
					sendErrorResponse(408, "Request Timeout", getErrorPage(408), this->fd_socket);
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
		sendErrorResponse(200, "OK", getErrorPage(200), this->fd_socket);
		return (true);
	}
	else
	{
		// 403 Forbidden
		std::remove(this->filename.c_str());
		sendErrorResponse(403, "Forbidden", getErrorPage(403), this->fd_socket);
		return (true);
	}
	return (false);
}

void Upload::setTotalBodySize(long in_total)
{
	this->totalBodySize = in_total;
}

bool    Upload::writeToFileString(const std::string &source, size_t size)
{
	bodyContent.write(source.data(), size);
	this->bodyContent << source;
	if (bodyContent.fail())
		return false;
	return true;
}

bool    Upload::writeToFileString(const std::string &source)
{
	this->bodyContent << source;
	if (bodyContent.fail())
		return false;
	return true;
}

bool Upload::writeToFile(const std::vector<char> &source, size_t end)
{
	bodyContent.write(source.data(), end);
	if (bodyContent.fail())
		return false;
	return true;
}

bool	Upload::writeToFile(const std::vector<char> & source)
{
	bodyContent.write(source.data(), source.size());
	if (bodyContent.fail())
		return false;
	return true;
}

void Upload::endLine()
{
	this->bodyContent.flush();
}

