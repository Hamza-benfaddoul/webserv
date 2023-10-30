#include "../../includes/configParser.hpp"
#include <iostream>
#include <fstream>
#include <string>
configParser::configParser()
{
	setConfigFilePath(DEFAULT_PATH);
	parse();
	// loadFile();

}

configParser::configParser(const std::string &)
{
}
configParser::~configParser()
{
}

bool	regFile(std::string path)
{
	struct stat st;

	if (stat(path.c_str(), &st) != 0)
		return false;
	return S_ISREG(st.st_mode);
}

long	getFileSize( std::string path ) {
	struct stat st;
	int rc = stat(path.c_str(), &st);
	return rc == 0 ? st.st_size : -1;
}

// trim from end of string (right)
std::string& rtrim(std::string& s, const char* t)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
std::string& ltrim(std::string& s, const char* t)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
std::string& trim(std::string& s)
{
    return ltrim(rtrim(s, " \t\n\r\f\v"), " \t\n\r\f\v");
}

bool configParser::loadFile()
{
	std::ifstream file(getConfigFilePath().c_str());
	if (file.is_open())
	{
		if (!regFile(getConfigFilePath()))
			throw notARegularFile();
		else if (getFileSize( getConfigFilePath()) == 0)
			throw configFileIsEmpty();
		else {
			std::string line;
			// std::vector<std::map<std::string, std::string> > att;
			serverBlock currentServer;
			// std::vector<std::map<std::string, std::string> > locations;
			while (std::getline(file, line)) {
				size_t commentPos = line.find('#');
				if (commentPos != std::string::npos) {
					line = line.substr(0, commentPos);
				}
				if (line.empty()) {
					continue;
				}
				if (line.find("server:") == 0) {
					serverBlocks.push_back(currentServer);
					currentServer = serverBlock();
				} else if (line.find("  location:") == 0) {
					std::map<std::string, std::string> locationAttributes;
					while (std::getline(file, line)) {
						if (line.empty())
							break;
						// size_t commentPos = line.find('#');
						// if (commentPos != std::string::npos) {
						// 	line = line.substr(0, commentPos);
						// }
						if (line.find("server:") == 0)
							exceptionsManager("Server Block Must be differenciated with new line!!!.");
						size_t colonPos = line.find(":");
						if (colonPos != std::string::npos) {
							std::string key = line.substr(0, colonPos);
							std::string value = line.substr(colonPos + 1);
							key = trim(key);
							value = trim(value);
							locationAttributes[key] = value;
						}
					}
					currentServer.setLocation(locationAttributes);
				} else {
					if (line.find("::") != std::string::npos)
        				throw std::runtime_error("Error: Found an invalid pattern related to ::!!!.");
					size_t colonPos = line.find(":");
					if (colonPos != std::string::npos) {
						std::string key = line.substr(0, colonPos);
						std::string value = line.substr(colonPos + 1);
						key = trim(key);
						value = trim(value);
						checkKeyValue(key, value);
						currentServer.setAttribute(key, value);
					}
				}
			}
			serverBlocks.push_back(currentServer);
			// std::cout << serverBlocks.size() << std::endl;
			for (std::vector<serverBlock>::iterator it = serverBlocks.begin() + 1; it != serverBlocks.end(); ++it) {
				it->parseBlock();
				// std::cout << it->getServerName() << std::endl;
				// std::cout << it->getPort() << std::endl;
				// std::cout << it->getRoot() << std::endl;
			}
		}
		return true;
	}
	return false;
}

void	configParser::checkKeyValue(const std::string &key, const std::string &value)
{
	if (key.length() == 0)
	{
		std::string a = "ERROR: !!!.";
		throw std::runtime_error(a);
	}
	else if (value.length() == 0 && key.length() > 0)
	{
		std::string a = "ERROR: `" + key + "` Must have a value!!!.";
		throw std::runtime_error(a);
	}
}

std::string configParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    if (start != std::string::npos && end != std::string::npos) {
        return str.substr(start, end - start + 1);
    }
    return "";
}

void	configParser::parseBlock(const std::string &block)
{
	(void)block;
}

void configParser::parse(void)
{
	if ( !loadFile())
	{
		std::string error = "ERROR: `" + std::string(DEFAULT_PATH) + "` cannot be opened !!!.";
		throw std::runtime_error(error);
	}
	// return true;
	
}

std::string configParser::getConfigFilePath(void) const
{
	return this->confFilePath;
}
// std::string configParser::getWorkingDir(void) const
// {
// 	return this->workingDir;
// }

// std::string configParser::getLogFile(void) const
// {
// 	return this->logFile;
// }

// int configParser::getMaxConnections(void) const
// {
// 	return this->maxConnections;
// }

// int configParser::getPort(void) const
// {
// 	return this->port;
// }

// bool configParser::getAutoIndex(void) const
// {
// 	return this->autoIndex;
// }

// void configParser::setLogFile(std::string logFilePath)
// {
// 	this->logFile = logFilePath;
// }

void configParser::setConfigFilePath(std::string confFilePath)
{
	this->confFilePath = confFilePath;
}

// void configParser::setWorkingDir(std::string workingDir)
// {
// 	this->workingDir = workingDir;
// }

// void configParser::setMaxConnections(int maxConnections)
// {
// 	this->maxConnections = maxConnections;
// }

// void configParser::setPort(int port)
// {
// 	this->port = port;
// }

// void configParser::setAutoIndex(bool autoIndex)
// {
// 	this->autoIndex = autoIndex;
// }
const char *configParser::notARegularFile::what() const throw()
{
    return "ERROR: is Not a Regular File!!!.";
}
const char *configParser::configFileIsEmpty::what() const throw()
{
    return "ERROR: Config File is Empty!!!.";
}
// const char *configParser::duplicatedEntry::what() const throw()
// {
//     return "ERROR: Duplicated Entry!!!.";
// }
void	configParser::exceptionsManager( std::string c )
{
	throw std::runtime_error(c);
}
const char *configParser::NotParsedWell::what() const throw()
{
    return "ERROR: Config File !!!.";
}

