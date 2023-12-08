#include "Upload.hpp"
#include "../../includes/main.hpp"

Upload::Upload(Request *req, int in_cpt, Location in_location, int in_fd, std::string in_cgi_path, serverBlock *serverBlock) :request(req), _serverBlock(serverBlock) ,cpt(in_cpt), location(in_location), fd_socket(in_fd), cgi_path(in_cgi_path)
{
	// std::cout << "*****************************************construcot" << std::endl;
	forked = false;
}

Upload::~Upload()
{
	std::cout << "the file should be removed is: " << this->filename << std::endl;
	unlink(this->filename.c_str());
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
	// this->bodyContent.open(filename.c_str(), std::ios::in | std::ios::out | std::ios::trunc);
	if (!this->bodyContent.is_open())
	{
		throw std::ios_base::failure("Failed to open file");
		// std::cout << "<< !!!! >> the file was not created successfuly" << std::endl;
		// return;
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
	// std::cout << "the path to cgi script: |||" << std::endl;
	// std::string readTimeOut = ourLocations["proxy_read_time_out"];
	
	if (cgi_path.length() > 0)
	{
		if (forked == false)
		{
			// std::cout << "readTimeOut: " << readTimeOut << std::endl;
			size_t sizeOfFile = FileSize(this->filename);
			std::stringstream streamFileSize;
			streamFileSize << sizeOfFile;
			forked = true;
			// std::cout << "the location path: " << location.getLocationPath() << std::endl;
			std::string uri = request->getPath();
			std::string cgi_path_script = location.getRoot() + uri;
			// std::cout << "cgi path scritp: " << cgi_path_script << std::endl;
			// Create an array of envirment that cgi need.
			// std::cout << "the content type is: " << content_type << std::endl;
			char *env[] = 
			{
				strdup(std::string("REDIRECT_STATUS=200").c_str()),
				strdup(std::string("SCRIPT_FILENAME=" + cgi_path_script).c_str()),
				strdup(std::string("REQUEST_METHOD=" + this->request->getMethod()).c_str()),
				strdup(std::string("QUERY_STRING=").c_str()),
				strdup(std::string("HTTP_COOKIE=").c_str()),
				strdup(std::string("HTTP_CONTENT_TYPE=" + content_type).c_str()),
				strdup(std::string("CONTENT_TYPE=" + content_type).c_str()),
				strdup(std::string("CONTENT_LENGTH=" + streamFileSize.str()).c_str()),
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
					{
						free(env[i]);
					}
				}
				this->bodyContent.close();
				std::remove(this->filename.c_str());
				// sendResponse(404, "Not Found", "<html><body> <h1> 404 Not Found</h1> </body></html>", "text/html");
				sendErrorResponse(404, "Request-URI Too Long", getErrorPage(404), this->fd_socket);
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
			ss << this->cpt;
			std::string cptAsString = ss.str();
			cgi_output_filename = "www/TempFiles/cgi_output" + cptAsString;
			//std::cout << "cgi_path: " << cgi_path << " cgi path script: " << cgi_path_script << std::endl;
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
			// std::cout << "redPid: " << retPid << " and pid: " << pid << std::endl;
			if (retPid == pid) // the child is done ==> the response could be success could be failed (depend on cgi output)
			{
				char	buffer[1024];
				std::fstream cgi_output_content;
				cgi_output_content.open(cgi_output_filename.c_str(), std::ios::in);
				if(!cgi_output_content.is_open())
					throw std::ios_base::failure("Failed to open file");
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
				std::cout << "--" << cgi_output << "--" << std::endl;
				size_t pos = cgi_output.find("\n\n");
				std::string body_cgi;
				if (pos == std::string::npos)
				{
					pos = cgi_output.find("\r\n\r\n");
					body_cgi = cgi_output.substr(pos + 4, cgi_output.length());
				}
				else
					body_cgi = cgi_output.substr(pos + 2, cgi_output.length());
				std::cout << "the pos: " << pos << std::endl;
				std::string headers = cgi_output.substr(0, pos);
				std::vector<std::string> splited_cgi_output;
				if (headers.find("\r\n") != std::string::npos)
					splited_cgi_output = ft_split(headers, "\r\n");
				else if (headers.find("\n") != std::string::npos)
					splited_cgi_output = ft_split(headers, "\n");

				for (int i = 0; i < (int)splited_cgi_output.size(); i++)
				{
					rtrim(splited_cgi_output.at(i), "\r\n");
					ltrim(splited_cgi_output.at(i), "\r\n");
					if (i == 0)
					{
						std::string http = std::string("HTTP/1.1 ");
						std::string lineRes = splited_cgi_output.at(0);
						lineRes = lineRes.substr(8, lineRes.length());
						http.append(lineRes);
						lineRes = http;
						splited_cgi_output.at(0) = lineRes;
					}
					// std::cout << "--> " << splited_cgi_output.at(i) << "--" << std::endl;
					if (splited_cgi_output.at(i).length() > 2)
					{
						write(this->fd_socket, splited_cgi_output.at(i).c_str(), splited_cgi_output.at(i).length());
						write(this->fd_socket, "\r\n", 2);
					}
				}
				write(this->fd_socket, "\r\n", 2);
				// std::cout << "body cgi -----------------------------> " << body_cgi << "*-*-*-*-*-*-*-*" << std::endl;
				write(this->fd_socket ,body_cgi.c_str(), body_cgi.length());
			}
			else // calculate the time to live of the child proccess if > 60 means timeout();
			{
				end = clock();
				if (((double)(end - start_c)) / CLOCKS_PER_SEC > (double)location.proxy_read_time_out)
				{
					// send respone time out !!!!!
					kill(pid, SIGKILL);
					close(cgi_output_fd);
					this->bodyContent.close();
					std::remove(this->filename.c_str());
					std::remove(cgi_output_filename.c_str());
					// sendResponse(408, "Request Timeout", "<html><body><h1>408 Request Timeout</h1></body></html>", "text/html");
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
		// sendResponse(200, "OK", "<html><body><h1>200 Success</h1></body></html>", "text/html");
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

