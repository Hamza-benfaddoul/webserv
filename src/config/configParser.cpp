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
			getline(file, line);
			if  (line != "server:")
				throw std::runtime_error("conf file must begin with `server:`");
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
				} else if (line.find("  location:") != std::string::npos)
					parseLocation(file, currentServer);
				else {
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
			// serverBlocks.erase(serverBlocks.begin());

				// std::cout << "servers:\t" << serverBlocks.size() << std::endl;
			for (std::vector<serverBlock>::iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it) {
					it->parseBlock();
				std::cout << "Locations Size:\t" << it->getLocations().size() << std::endl;
				for (size_t i = 0; i != it->getLocations().size(); i++) // LOcations
				{
					Location test = it->getLocations().at(i);
					test.parseLocations();
				}
				std::cout << "\n";
			}

		}
		return true;
	}
	return false;
}

void	configParser::parseLocation( std::ifstream& file, serverBlock &currentServer )
{
	Location	currentLocation;
	std::string line;

    while (std::getline(file, line)) {
        if (line.find("location:") != std::string::npos ) {
            exceptionsManager("Location Blocks Must be differentiated with a new line!!!.");
        } else if (line.empty()) {
            break;
        } else if (line.find("server:") != std::string::npos ) {
            exceptionsManager("Server Blocks Must be differentiated with a new line!!!");
        } else {
			// std::cout << line << std::endl;
			if (line.find("    - ") == 0)
			{
				size_t colonPos = line.find(":");
				if (colonPos != std::string::npos) {
					std::string key = trim(line.substr(6, colonPos-6));
					std::string value = trim(line.substr(colonPos + 1));
					checkKeyValue(key, value);
					currentLocation.setAttribute(key, value);
					// std::cout << currentLocation.getLocationAttributes().find(key)->second << std::endl;
				}
			}
            else {
                exceptionsManager("Invalid attribute format inside the location block.!!!");
            }
        }
    }
    currentServer.setLocation(currentLocation);
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

}

std::string configParser::getConfigFilePath(void) const
{
	return this->confFilePath;
}

void configParser::setConfigFilePath(std::string confFilePath)
{
	this->confFilePath = confFilePath;
}

const char *configParser::notARegularFile::what() const throw()
{
    return "ERROR: is Not a Regular File!!!.";
}
const char *configParser::configFileIsEmpty::what() const throw()
{
    return "ERROR: Config File is Empty!!!.";
}

void	configParser::exceptionsManager( std::string c )
{
	throw std::runtime_error("Error: " + c);
}
const char *configParser::NotParsedWell::what() const throw()
{
    return "ERROR: Config File !!!.";
}

