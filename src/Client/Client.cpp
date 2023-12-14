/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbenfadd <hbenfadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 11:35:06 by hbenfadd          #+#    #+#             */
/*   Updated: 2023/12/05 15:32:13 by hbenfadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <fcntl.h>
#include "dirent.h"

int Client::cpt = 0;
Client::Client(size_t fd, serverBlock *serverBlock) : _fd(fd), location(), _serverBlock(serverBlock)
{
	_fdFile = -1;
	this->_readHeader = true;
	this->request = NULL;
	this->upload = NULL;
	errorCheck = false;
	fileCreated = false;
	totalBytesRead = 0;
	isRead = false;
	controller = false;
	content_length = 0;
	hasCgi = false;
	isChunkComplete = true;
	forked = false;
}

bool Client::receiveResponse(void)
{
	if (_readHeader)
	{
		char buffer[1024] = {0};
		int bytesRead;
		bytesRead = read(_fd, buffer, 1024);
		if (bytesRead < 0)
			throw std::runtime_error("Could not read from socket");
		_responseBuffer.append(buffer, bytesRead);
		if (_responseBuffer.find("\r\n\r\n") != std::string::npos)
		{
			this->request = new Request(_responseBuffer);
			this->request->parseRequest();
			this->request->printRequest();
			this->body = this->request->getBodyString();
			std::map<std::string, std::string> Oheaders = this->request->getHeaders();
			if (Oheaders.find("Content-Length") != Oheaders.end())
			{
				std::string C_Length = Oheaders["Content-Length"];
				Content_Length = strtod(C_Length.c_str(), NULL);
			}
			location = getCurrentLocation();
			_readHeader = false;
			if (is_request_well_formed() == -1)
				return true;
		}
		else
			return false;
	}
	if (!_readHeader)
	{
		if (this->request->getMethod().compare("GET") == 0)
		{
			return getMethodHandler();
		}
		else if (this->request->getMethod().compare("POST") == 0)
		{
			return postMethodHandler();
		}
		else if (this->request->getMethod().compare("DELETE") == 0)
			return deleteMethodHandler();
		else if (this->request->getMethod().compare("HEAD") == 0)
		{
			sendHeadErrorResponse(501, "Not Implemented", _fd);
			return true;
		} else
		{
			sendErrorResponse(501, "Not Implemented", getErrorPage(501), _fd);
			return true;
		}
	}
	return false;
}

void Client::del(const char *path, bool &isDeleted)
{
	DIR *dir = opendir(path);
	if (dir)
	{
		struct dirent *centent;
		while ((centent = readdir(dir)) != NULL)
		{
			if (strcmp(centent->d_name, ".") != 0 && strcmp(centent->d_name, "..") != 0)
			{
				std::string re(path);
				re += "/";
				re += centent->d_name;
				del(re.c_str(), isDeleted);
			}
		}
		rmdir(path);
	}
	else
	{
		if (access(path, W_OK) == 0)
		{
			if (std::remove(path) != 0)
				throw std::runtime_error("cant remove file");
		}
		else
			isDeleted = false;
	}
}
bool Client::deleteMethodHandler(void)
{
	bool isDeleted = true;
	std::string requestedPath = this->request->getPath();
	if (access((location.getRoot() + requestedPath).c_str(), R_OK) == -1)
	{
		sendErrorResponse(404, "Not Found", getErrorPage(404), _fd);
		return (true);
	}
	else
		del((location.getRoot() + requestedPath).c_str(), isDeleted);
	if (isDeleted)
		write(_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nResource deleted successfully", 74);
	else
		write(_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nFailed to delete resource", 70);
	return true;
}

Location Client::getCurrentLocation()
{
	std::string requestedPath = this->request->getPath();
	std::string directory;

	if (regFile(location.getRoot() + requestedPath))
	{
		size_t directoryEndPos = requestedPath.find("/", 1);
		directory = (directoryEndPos != std::string::npos) ? requestedPath.substr(0, directoryEndPos) : requestedPath;
	}
	else
	{
		directory = (requestedPath.length() > 1 && requestedPath.at(requestedPath.length() - 1) == '/' && isdigit(requestedPath.at(requestedPath.length() - 2))) ? requestedPath.substr(0, requestedPath.length() - 1) : requestedPath;
	}
	for (size_t i = 0; i != this->_serverBlock->getLocations().size(); i++)
	{
		Location test = this->_serverBlock->getLocations().at(i);
		if (regFile(test.getRoot() + directory))
		{
			directory = "/";
		}
		if (directory == test.getLocationPath())
		{
			test.directory = directory;
			return test;
		}
	}
	while (directory.length() > 0)
	{
		directory = directory.substr(0, directory.length() - 1);
		for (size_t i = 0; i != this->_serverBlock->getLocations().size(); i++)
		{
			Location test = this->_serverBlock->getLocations().at(i);
			if (regFile(test.getRoot() + directory))
			{
				directory = "/";
			}
			if (directory == test.getLocationPath())
			{
				test.directory = directory;
				return test;
			}
		}
	}
	return Location();
}

bool Client::checkRequestPath(std::string path)
{
	return (path[path.length() - 1] == '/') ? 1 : 0;
}

bool Client::checkType()
{
	std::string requestedPath = this->request->getPath();
	DIR *pDir;

	pDir = opendir((_serverBlock->getRoot() + requestedPath).c_str());
	if (pDir == NULL)
	{
		return false;
	}
	closedir(pDir);
	return true;
}

bool Client::checkDir(std::string path)
{
	for (size_t i = 0; i != this->_serverBlock->getLocations().size(); i++) // LOcations
	{
		std::string locationPath, initialPath;
		initialPath = _serverBlock->getLocations().at(i).getLocationPath();
		if (initialPath == path)
			return true;
	}
	return false;
}

bool Client::handleDirs()
{
	std::string requestedPath = this->request->getPath();
	if (requestedPath[requestedPath.length() - 1] != '/')
	{
		sendRedirectResponse(301, "Moved Permanently", requestedPath + "/");
	}
	else
	{
		if (location.index.length() > 0 && location.hasIndex == true)
		{
			
			std::stringstream index(location.index);
			std::string iter;
			while (getline(index, iter, ','))
			{
				iter = advanced_trim(iter, " ");
				struct dirent *pDirent;
				DIR *pDir;
				pDir = opendir((location.getRoot() + "/" + location.directory).c_str());
				if (pDir == NULL)
				{
					std::cout << "Cannot open directory\n";
					return 1;
				}
				int fileCount = 0;
				while ((pDirent = readdir(pDir)) != NULL)
				{
					if (strcmp(iter.c_str(), pDirent->d_name) == 0)
					{
						handleFiles(location.getRoot() + "/" + location.directory + "/" + iter);
						return true;
					}
					fileCount++;
				}
				if (fileCount <= 2)
				{
					sendErrorResponse(403, "Forbidden", getErrorPage(403), _fd);
					return true;
				}
				closedir(pDir);
			}
		}
		else if (location.getAutoIndex() == false)
		{
			sendErrorResponse(403, "Forbidden", getErrorPage(403), _fd);
		}
		else if (location.getAutoIndex() == true)
		{
			directoryListing(location.getRoot() + location.directory);
		}
	}
	return true;
}
#include <unistd.h>
#include <sys/wait.h>

std::string Client::getCgiPath(std::string extension)
{
	for (size_t i = 0; i < location.cgi.size(); i++)
	{
		if (location.cgi.at(i).first == extension)
			return location.cgi.at(i).second;
	}
	return "";
}

std::string extractBodyFromContent(const std::string &content, std::string &header)
{
	// Find the end of headers (CRLFCRLF)
	size_t found = content.find("\r\n\r\n");
	// If headers were found, return the substring after the headers
	if (found != std::string::npos)
	{
		header = content.substr(0, found + 4);
		// std::cout  << header ;
		return content.substr(found + 4);
	}
	// Headers not found, return an empty string or handle accordingly
	return "";
}

std::string Client::createNewFile(std::string prefix, size_t start, std::string suffix)
{
	std::stringstream ss, filename;
	ss << start;
	filename << prefix << start << suffix;
	std::fstream fd;
	fd.open(filename.str().c_str(), std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
	if (!fd.is_open())
	{
		throw std::ios_base::failure("ERROR: check Folder `www/TempFiles` existence or permissions for Write.\n");
	}
	fd.close();
	return filename.str();
}


bool Client::handleFiles(std::string path)
{
	size_t dotPos = path.find_last_of('.');
	size_t markPos = path.find_last_of('?');
	std::string extension;
	if (dotPos != std::string::npos && (markPos == std::string::npos || dotPos > markPos))
	{
		extension = path.substr(dotPos);
	}
	size_t position = this->request->getPath().find('?');
	std::string cgi_path = getCgiPath(extension);
	if (cgi_path.length() > 0 && (extension == ".php" || extension == ".py" || extension == ".pl" || extension == ".go" || extension == ".sh"))
	{
		if (forked == false)
		{
			forked = true;
			std::stringstream ss;
			ss << _serverBlock->getPort();
			char *env[] =
				{
					strdup(std::string("REDIRECT_STATUS=200").c_str()),
					strdup(std::string("SCRIPT_FILENAME=" + path).c_str()),
					strdup(std::string("CACHE_CONTROL=no-cache").c_str()),
					(path == location.getRoot() + request->getPath()) ? strdup(std::string("QUERY_STRING=").c_str()) : strdup(std::string("QUERY_STRING=" + request->getPath().substr(position + 1)).c_str()),
					strdup(std::string("REQUEST_METHOD=" + request->getMethod()).c_str()),
					strdup(std::string("SERVER_NAME=localhost").c_str()),
					strdup(std::string("SERVER_PORT=" + ss.str()).c_str()),
					strdup(std::string("SERVER_PROTOCOL=HTTP/1.1").c_str()),
					strdup(std::string("SERVER_SOFTWARE=Weebserv/1.0").c_str()),
					strdup(std::string("CONTENT_TYPE=" + request->getMimeType()).c_str()),
					strdup(std::string("REDIRECT_STATUS=200").c_str()),
					strdup(std::string("HTTP_COOKIE=" + request->getCookie()).c_str()),
					NULL};
			tmpFile = createNewFile("www/TempFiles/", clock() / CLOCKS_PER_SEC, "_cgi");
			start_c = clock();
			fd = fork();
			char *argv[] = {
				strdup(cgi_path.c_str()),
				// (extension == ".go") ? strdup("run"): strdup(""),
				strdup((location.getRoot() + request->getPath()).c_str()),
				NULL};
			if (fd == 0)
			{
				if (freopen(tmpFile.c_str(), "w", stdout) == NULL)
					throw std::ios_base::failure("Failed to open File");
				execve(argv[0], argv, env);
				perror("execve:");
				exit(0);
			}
			for (size_t i = 0; env[i]; i++)
			{
				free(env[i]);
			}
			for (size_t i = 0; argv[i]; i++)
			{
				free(argv[i]);
			}
		}
		if (forked == true)
		{
			int state;
			ss1 = waitpid(fd, &state, WNOHANG);
			if (ss1 == fd)
			{
				readFromCgi();
				ltrim(content, "\r\n");
				size_t pos = content.find("\r\n\r\n");
				std::string bodyCgi;
				if (pos == std::string::npos)
				{
					pos = content.find("\n\n");
					if (pos != std::string::npos)
						bodyCgi = content.substr(pos + 2);
				}
				else
					bodyCgi = content.substr(pos + 4);
				std::string headers = content.substr(0, pos);
				std::stringstream result;
				std::vector<std::string> splitedHeaders = ft_split(headers, "\r\n");
				if (state == 0 && headers.find("Location: ") != std::string::npos)
				{
					parseHeaderLocation(splitedHeaders);
					fsync(_fd);
					std::remove(tmpFile.c_str());
					return true;
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
				write(_fd, result.str().c_str(), result.str().length());
				std::remove(tmpFile.c_str());
				fsync(_fd);
				return true;
			}else{
				end = clock();
				double elapsed_secs = static_cast<double>(end - start_c) / CLOCKS_PER_SEC;
				if (elapsed_secs > (location.proxy_read_time_out))
				{
					std::cout<< "elapsed_secs: " << elapsed_secs << "\n";
					std::cout<< "location.proxy_read_time_out: " << location.proxy_read_time_out<< "\n";
					kill(fd, SIGKILL);
					std::remove(tmpFile.c_str());
					sendErrorResponse(408, "Request Timeout", getErrorPage(408), _fd);
					return true;
				}
				return false;
				// usleep(100000);
			}
		}
		return true;
	}
	else
	{
		if (_fdFile == -1)
			_fdFile = open(path.c_str(), O_RDONLY);
		if (_fdFile > -1)
		{
			std::string mimeType = getMimeTypeFromExtension(path);
			std::stringstream headers;
			if (isRead == false)
			{
				headers << "HTTP/1.1 200 OK\r\n";
				headers << "Content-Type: " << mimeType << "\r\n";
				headers << "Transfer-Encoding: chunked\r\n";
				headers << "Content-Disposition: inline\r\n";
				headers << "Connection: close\r\n\r\n";
				isRead = true;
			}
			if (isRead == true)
			{
				std::string head = headers.str();
				return serveImage(head);
			}
		}
		// return readFile(path);
		return true;
	}
}

void Client::parseHeaderLocation(std::vector<std::string> headers)
{
	std::stringstream response;
	std::vector<std::string>::iterator it = headers.begin();
	char *status = NULL;
	if (it->find("Status: ") != std::string::npos)
		status = (char *)it->c_str() + 8;
	std::stringstream ss(status);
	std::string word;
	getline(ss, word, ' ');
	int statusCode = atoi(word.c_str());
	getline(ss, word, '\r');
	std::string errorStatus = word;
	response << "HTTP/1.1 " << statusCode << " " << errorStatus << "\r\n";
	it++;
	if (it != headers.end() && it->find("Location: ") != std::string::npos)
		status = (char *)it->c_str() + 10;
	response << "Location: " << status << "\r\n\r\n";
	write(_fd, response.str().c_str(), response.str().length());
}
void Client::readFromCgi()
{
	std::fstream cgi_output_content;
	cgi_output_content.open(tmpFile.c_str(), std::ios::in);
	if (!cgi_output_content.is_open())
		throw std::ios_base::failure("Failed to open file cgi");
	char buffer[1024];
	size_t found = 0;
	while (1)
	{
		cgi_output_content.read(buffer, 1024);
		if (cgi_output_content.gcount() <= 0)
			break;
		content.append(buffer, cgi_output_content.gcount());
		if (cgi_output_content.eof())
			break;
		// found = content.find("\r\n\r\n");
		// if (found != std::string::npos)
		// {
		// 	found += 4;
		// 	content.substr(0, found);
		// 	break;
		// }
	}
	file_ouptut.close();
	file_ouptut.open(tmpFile.c_str());
	file_ouptut.seekg(0, std::ios::end);
	content_length = file_ouptut.tellg();
	content_length -= found;
	file_ouptut.seekg(found, std::ios::beg);
}

bool Client::serveImage(std::string &headers)
{
	bool endOfFile = false;
    if (_fdFile > -1)
    {
        size_t chunkSize = 1024;
        char buffer[chunkSize];
        size_t bytesRead = read(_fdFile, buffer, chunkSize);
        if (bytesRead > 0)
		{
			std::stringstream chunkHeader;
			chunkHeader << std::hex << bytesRead << "\r\n";
			headers.append(chunkHeader.str());
			headers.append(buffer, bytesRead);
			headers.append("\r\n");
			if (bytesRead < chunkSize)
			{
				headers.append("0\r\n\r\n");
				close(_fdFile);
				isRead = false;
				_fdFile = -1;
				endOfFile = true;
			}
		}
		else if (bytesRead == 0)
		{
			headers.append("0\r\n\r\n");
			close(_fdFile);
			isRead = false;
			_fdFile = -1;
			endOfFile = true;
		}
    }
	write(_fd, headers.c_str(), headers.length());
	fsync(_fd);
    return endOfFile;
}
std::string Client::getErrorPage(int errorCode)
{
	for (size_t i = 0; i < _serverBlock->errorPages.size(); i++)
	{
		if (_serverBlock->errorPages.at(i).first == errorCode)
			return _serverBlock->errorPages.at(i).second;
	}
	std::stringstream ss;
	ss << errorCode;
	return "www/error/" + ss.str() + ".html";
}

bool Client::getMethodHandler(void)
{
	std::string requestedPath = this->request->getPath();
	size_t queryStringPos = requestedPath.find('?');
	std::string filePath = (queryStringPos != std::string::npos) ? requestedPath.substr(0, queryStringPos) : requestedPath;
	// std::cout << location.getRoot() + filePath << std::endl;
	std::cout << location.getRoot() << std::endl;
	std::cout << filePath << std::endl;
	if (access((location.getRoot() + filePath).c_str(), R_OK) == -1)
	{
		sendErrorResponse(404, "Not Found", getErrorPage(404), _fd);
	}
	else if (checkType() == true)
	{
		std::cout << "handleDirs" << std::endl;
		return handleDirs();
	}
	else if (checkType() == false)
	{
		return handleFiles(location.getRoot() + filePath);
	}
	return true;
}

bool Client::run(void)
{
	return this->receiveResponse();
}

Client::~Client()
{
	delete request;
	close(_fd);
}

// void	Client::sendErrorResponse( int CODE, std::string ERRORTYPE, std::string errorTypeFilePath, int _fd) {
// 	std::ifstream file(errorTypeFilePath.c_str());
// 	std::string content;
// 	if (file.is_open())
// 	{
// 		{
// 			std::string line;
// 			while (getline(file, line))
// 			{
// 				content += line;
// 			}
// 		}
// 	}
// 	file.close();
// 	std::stringstream response;
// 	response << "HTTP/1.1 " << CODE << " " << ERRORTYPE << "\r\n";
// 	response << "Content-Type: text/html; charset=UTF-8\r\n";
// 	response << "Content-Length: " << content.length() << "\r\n";
// 	response << "\r\n";
// 	response << content;

// 	write(_fd, response.str().c_str(), response.str().length());
// }

void Client::sendRedirectResponse(int CODE, std::string ERRORTYPE, std::string location)
{
	std::stringstream response;
	response << "HTTP/1.1 " << CODE << " " << ERRORTYPE << "\r\n";
	response << "Location: " << location << "\r\n";
	// response << "Connection: close\r\n";
	response << "\r\n";

	write(_fd, response.str().c_str(), response.str().length());
}

// ======================= POST method ==========================================

int Client::is_request_well_formed()
{

	// ***** FOR WALID {*******
	// hanta checker if (location.isEmpty = true)
	// sendError404
	// ***** }          *******

	if (location.isEmpty == true)
	{
		sendErrorResponse(404, "Not Found", getErrorPage(404), _fd);
		return (-1);
	}
	std::string path = this->request->getPath();
	std::string charAllowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	int badChar = 0;

	for (int i = 0; i < (int)path.length(); i++)
	{
		size_t pos = charAllowed.find(path[i]);
		if (pos == std::string::npos)
		{
			badChar = 1;
			break;
		}
	}

	std::map<std::string, std::string> ourHeaders = this->request->getHeaders();
	std::map<std::string, std::string>::iterator it = ourHeaders.find("Transfer-Encoding");
	// bad request
	if (badChar == 1 || this->request->getBad() == 1 || (request->getMethod() == "POST" && it == ourHeaders.end() && ourHeaders.find("Content-Length") == ourHeaders.end()))
	{
		sendErrorResponse(400, "Bad Request", getErrorPage(400), _fd);
		return (-1);
	}
	// transfer encoding is equal to chunk"Moved Permanently"ed
	if (ourHeaders.find("Transfer-Encoding") != ourHeaders.end() && ourHeaders["Transfer-Encoding"] != "chunked")
	{
		sendErrorResponse(501, "Not Implemented", getErrorPage(501), _fd);
		return (-1);
	}
	// request uri containe more that 2048 char
	if (path.length() > 2048)
	{
		sendErrorResponse(414, "Request-URI Too Long", getErrorPage(414), _fd);
		return (-1);
	}
	if (location.returnStatus)
	{
		sendRedirectResponse(location.returnstatusCode, "Moved Permanently", location.returnPath);
		return -1;
	}
	if ((request->getMethod() == "GET" && location.GET == false) ||
		(request->getMethod() == "POST" && location.POST == false) ||
		(request->getMethod() == "DELETE" && location.DELETE == false))
	{
		sendErrorResponse(405, "405 Method Not Allowed", getErrorPage(405), _fd);
		return -1;
	}
	return (true);
}

// true -> close, flase continue;
bool Client::postMethodHandler(void)
{
	std::map<std::string, std::string> Headers = this->request->getHeaders();
	char buffer[1024] = {0};
	int bytesRead;

	if (fileCreated == false)
	{
		std::string extension;
		std::string path = request->getPath();
		size_t posDot = path.rfind(".");
		if (posDot != std::string::npos)
		{
			extension = path.substr(posDot, path.length());
			if (this->getCgiPath(extension).length() > 0)
				hasCgi = true;
		}
		this->upload = new Upload(this->request, this->cpt, location, _fd, this->getCgiPath(extension), _serverBlock);
		this->upload->createFile();
		totalBytesRead = body.length();
		if (Headers.find("Content-Length") != Headers.end() && totalBytesRead >= Content_Length)
		{
			this->upload->writeToFileString(body.data(), body.length());
			this->upload->endLine();
			fileCreated = true;
			this->upload->setTotalBodySize(totalBytesRead);
			return this->upload->start();
			// sendErrorResponse(200, "OK", "<html><body><h1>200 Success</h1></body></html>");
			// return true;
		}
		fileCreated = true;
		this->cpt++;
	}
	// read until body is complte (chunk by chunk)
	if (controller == false && Headers.find("Transfer-Encoding") != Headers.end() && Headers["Transfer-Encoding"] == "chunked") // ============> chunk type
	{
		if (isChunkComplete == true)
		{
			ltrim(this->body, "\r\n");
			pos = body.find("\r\n");
			chunkSizeString.append(body.substr(0, pos));
			std::istringstream iss(chunkSizeString);
			iss >> std::hex >> chunkSizeInt;
			if (chunkSizeInt != 0)
			{
				// totalBytesRead += chunkSizeInt;
				chunkSizeString.clear();
				body.erase(0, pos + 2);
				isChunkComplete = false;
			}
		}
		if (body.find("0\r\n\r\n") != std::string::npos)
		{
			body.erase(body.length() - 5, body.length());
			this->upload->writeToFileString(body);
		}
		else
		{
			size_t len  = body.length();
			if (chunkSizeInt > len)
			{
				bytesRead = read(_fd, buffer, 1024);
				body.append(buffer, bytesRead);
			}
			else
			{
				totalBytesRead += chunkSizeInt;
				this->upload->writeToFileString(body, chunkSizeInt);
				body.erase(0, chunkSizeInt);
				isChunkComplete = true;
				bytesRead = read(_fd, buffer, 1024);
				body.append(buffer, bytesRead);
			}
			return false;
		}
		controller = true;
		// this->upload->endLine();
	}
	else if (Headers.find("Transfer-Encodgin") != Headers.end() && Headers["Transfer-Encoding"] != "chunked")
	{
		sendErrorResponse(501, "Not Implemented", getErrorPage(501), _fd);
		return true;
	}
	else if (controller == false && Headers.find("Content-Length") != Headers.end()) // ============> binary type
	{
		if (totalBytesRead < this->Content_Length)
		{
			bytesRead = read(_fd, buffer, 1024);
			this->totalBytesRead += bytesRead;
			this->body.append(buffer, bytesRead);
			// if (totalBytesRead >= this->Content_Length)
			this->upload->writeToFileString(body);
			this->upload->endLine();
			this->body.clear();
			if (totalBytesRead < this->Content_Length)
				return false; // keep reading
			controller = true;
		}
	}
	else if (controller == false)
	{
		sendErrorResponse(400, "Bad Request", getErrorPage(400), _fd);
		return true;
	}
	this->upload->endLine();
	this->upload->setTotalBodySize(totalBytesRead);
	return this->upload->start();
}

void Client::directoryListing(std::string path)
{
	std::string html = generateDirectoryListing(path);

	// Prepare headers
	std::stringstream headers;
	headers << "HTTP/1.1 200 OK\r\n";
	headers << "Content-Type: text/html\r\n";
	headers << "Content-Length: " << html.length() << "\r\n";
	headers << "Connection: close\r\n\r\n";

	// Write headers to socket
	write(_fd, headers.str().c_str(), headers.str().length());

	// Write HTML content to socket
	write(_fd, html.c_str(), html.length());
}

std::string Client::generateDirectoryListing(const std::string& directoryPath)
{
    std::stringstream html;
    html << "<style> h2 {"
         << "color: #aaa;"
         << "font-size: 30px;"
         << "line-height: 40px;"
         << "font-style: italic;"
         << "font-weight: 200;"
         << "margin: 40px;"
         << "text-align: center;"
         << "text-shadow: 1px 1px 1px rgba(255, 255, 255, 0.7);"
         << "}"
         << ".box {"
         << "background: #fff;"
         << "border-radius: 2px;"
         << "box-shadow: 0 0 50px rgba(0, 0, 0, 0.1);"
         << "margin: 30px 5%;"
         << "padding: 5%;"
         << "}"
         << "@media (min-width: 544px) {"
         << ".box {"
         << "margin: 40px auto;"
         << "max-width: 440px;"
         << "padding: 40px;"
         << "}"
         << "}"
         << ".directory-list ul {"
         << "margin-left: 10px;"
         << "padding-left: 20px;"
         << "border-left: 1px dashed #ddd;"
         << "}"
         << ".directory-list li {"
         << "list-style: none;"
         << "color: #888;"
         << "font-size: 17px;"
         << "font-style: italic;"
         << "font-weight: normal;"
         << "}"
         << ".directory-list a {"
         << "border-bottom: 1px solid transparent;"
         << "color: #888;"
         << "text-decoration: none;"
         << "transition: all 0.2s ease;"
         << "}"
         << ".directory-list a:hover {"
         << "border-color: #eee;"
         << "color: #000;"
         << "}"
         << ".directory-list .folder,"
         << ".directory-list .folder > a {"
         << "color: #777;"
         << "font-weight: bold;"
         << "}"
         << ".directory-list li:before {"
         << "margin-right: 10px;"
         << "content: \"\";"
         << "height: 20px;"
         << "vertical-align: middle;"
         << "width: 20px;"
         << "background-repeat: no-repeat;"
         << "display: inline-block;"
         << "background-image: url(\"data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 100 100'><path fill='lightgrey' d='M85.714,42.857V87.5c0,1.487-0.521,2.752-1.562,3.794c-1.042,1.041-2.308,1.562-3.795,1.562H19.643 c-1.488,0-2.753-0.521-3.794-1.562c-1.042-1.042-1.562-2.307-1.562-3.794v-75c0-1.487,0.521-2.752,1.562-3.794 c1.041-1.041,2.306-1.562,3.794-1.562H50V37.5c0,1.488,0.521,2.753,1.562,3.795s2.307,1.562,3.795,1.562H85.714z M85.546,35.714 H57.143V7.311c3.05,0.558,5.505,1.767,7.366,3.627l17.41,17.411C83.78,30.209,84.989,32.665,85.546,35.714z' /></svg>');background-position: center 2px;background-size: 60% auto;background-size: 60% auto;</style><body><h2>Directory List</h2><div class=\"box\"><ul class=\"directory-list\">";

    DIR* dir;
    struct dirent* entry;
    struct stat entryStat;
    // Open the directory
    dir = opendir(directoryPath.c_str());
    if (dir != NULL) {
        // Read directory entries
        while ((entry = readdir(dir)) != NULL) {
            std::string name = entry->d_name;
            // Skip current and parent directory entries
            if (name != "." && name != "..") {
                std::string fullPath = directoryPath + "/" + name;
                stat(fullPath.c_str(), &entryStat);
                // Check if the entry is a file or a directory
                std::string href;
                std::string listItemClass;
                std::string iconSrc;
                if (S_ISDIR(entryStat.st_mode)) {
                    href =  location.directory + "/" + name + "/";
                    listItemClass = "folder";
                    iconSrc = "https://img.icons8.com/material-rounded/24/folder-invoices.png";
                } else if(S_ISREG(entryStat.st_mode)) {
                    href = location.directory + "/" + name;
                    listItemClass = "file";
                    iconSrc = "https://img.icons8.com/doodle/24/file--v1.png";
                }
                // Add an entry to the HTML list
                html << "<li style='height: 40px !important;' class=\"" << listItemClass << "\"><a href=\"" << href << "\"><img width='24' height='24'  src=\"" << iconSrc << "\" alt=\"\" />" << name << "</a></li>";
            }
        }
        closedir(dir);
    } else {
        // Handle directory open error
        std::cerr << "Error opening directory: " << strerror(errno) << std::endl;
    }

    html << "</ul></div>";
    return html.str();
}
