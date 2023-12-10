#include "../../includes/Location.hpp"
#include "../../includes/main.hpp"
Location::Location()
{
	// root = "www";
	locationPath = "/";
	GET = true;
	POST = true;
	DELETE = true;
	isEmpty = true;
	hasCGI = false;
	proxy_read_time_out = 60;
	hasIndex = false;
	returnStatus = false;
}

void	Location::parseReturn( std::string value )
{
	this->returnStatus = true;
	std::stringstream ss(value);
	std::string word;
	int len = 0;
	while (getline(ss, word, ' '))
	{
		word = advanced_trim(word, " ");
		if (word.length() > 0)
			len++;
	}
	std::cout << len << "\n";
	if (len > 2)
		throw std::invalid_argument("ERROR: Invalid argument in return `" + value + "`");
	else {
		std::stringstream ss(value);
		std::string word;
		getline(ss, word, ' ');
		if (containsOnlyDigits(word))
		{
			returnstatusCode = atoi(word.c_str());
			if (returnstatusCode < 300 || returnstatusCode > 399)
				throw std::invalid_argument("ERROR: return statusCode must be between 300 and 399 `" + value + "`");
		}
		else
			throw std::invalid_argument("ERROR: Invalid argument in return status Code `" + value + "`");
		getline(ss, word, ' ');
		word = advanced_trim(word, " ");
		while (getline(ss, word, ' '))
		{
			word = advanced_trim(word, " ");
			if (word.length() > 0)
				returnPath = word;
		}
	}
}

void    Location::setAttribute( const std::string &key, std::string value )
{
	//  this->locationAttributes[key] = value;
	 this->locationAttributes.insert(std::pair<std::string, std::string>(key, advanced_trim(value, "\"")));
}

std::string Location::getKeyFromAttributes(std::string key)
{
	std::map<std::string, std::string>::iterator it = locationAttributes.find(key);
	if (it != locationAttributes.end() && it->second.length() > 0) {
		return it->second;
	} else {
		return std::string("");
	}
}


void    Location::parseLocations( void )
{
	std::map<std::string, std::string> location = getLocationAttributes();
	for (std::map<std::string, std::string>::iterator iterator = location.begin(); iterator != location.end(); iterator++)
	{
		if (iterator->first == "path")
		{
			this->locationPath = advanced_trim(iterator->second, "\"");
		}
		else if (iterator->first == "root")
			parseRoot(iterator->second);
		else if (iterator->first == "methods")
			parseMethods(iterator->second);
		else if (iterator->first == "autoindex")
			parseAutoIndex(iterator->second);
		else if (iterator->first == "index")
			parseIndex(iterator->second);
		else if (iterator->first == "proxy_read_time_out")
			parseProxyReadTimeOut(iterator->second);
		else if (iterator->first == "return")
			parseReturn(iterator->second);
		
			// parsePortNumber(iterator->second);
		// std::cout << iterator->first << ": " << iterator->second << std::endl;
	}
	isEmpty = false;
	if (hasCGI)
		parseCGI();
}

void	Location::parseProxyReadTimeOut( std::string timeOut )
{
	std::istringstream iss(timeOut);
    int value;
    std::string unit;
    
    if (!(iss >> value >> unit) || unit.length() > 1) {
        throw std::invalid_argument("Invalid time format");
    }

    switch (unit.at(0)) {
        case 's':
            proxy_read_time_out = value;
			return ;
        case 'm':
            proxy_read_time_out = value * 60;
			return ;
        case 'h':
            proxy_read_time_out = value * 3600;
			return ;
        default:
            throw std::invalid_argument("Invalid time unit");
    }
}

void Location::parseIndex( std::string value)
{
	if (value.length() > 0)
	{
		this->index = trim(value);
		hasIndex = true;
	}

}

void	Location::parseCGI()
{
	for (size_t i = 0; i < cgi.size(); i++)
	{
		std::string path = cgi.at(i).second;
		if (access(path.c_str(), F_OK) == -1)
		{
			throw std::runtime_error("ERROR: Check CGI file is exist in your file system! `" + path + "`");
		}
	}
}

void Location::parseAutoIndex( const std::string &value)
{
	// std::cout << value.find("on") << std::endl;
	if (std::string("on").compare(value) != 0 && std::string("off").compare(value) != 0 )
		throw std::runtime_error("ERROR: Auto Index EXPECTS just `on` or `off` !!!.");
	else if (value == "on")
		autoIndex = true;
	else
		autoIndex = false;
	// else if ( std::string("off").compare(value) != 0 )
}

void    Location::parseRoot( const std::string &root )
{
	// std::cout << root << std::endl;
	if (!root.empty())
	{
		struct stat s;
		if( stat( root.c_str(), &s ) != 0 )
			exceptionsManager("cannot access " + root );
		else if( s.st_mode & S_IFDIR ){
			this->root = root;
			return ;
		}  // S_ISDIR() doesn't exist on my windows
			// exceptionsManager(root + " is a directory");
		else
			exceptionsManager(root +  " is not a directory");
	}
	this->root = "www";
}

void	Location::parseMethods( const std::string &methods )
{
	GET = false;
	POST = false;
	DELETE = false;
	std::string method;
	std::stringstream input_stringstream(methods);
	while (getline(input_stringstream,method,','))
	{
		method = trim(method);
		if (method.compare("GET") == 0)
			GET = true;
		else if (method.compare("POST") == 0)
			POST = true;
		else if (method.compare("DELETE") == 0)
			DELETE = true;
		else
			exceptionsManager("Unexpectod Method");
	}
}

// trim from end of string (right)
std::string& Location::rtrim(std::string& s, const char* t)
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

// trim from beginning of string (left)
std::string& Location::ltrim(std::string& s, const char* t)
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

std::string& Location::trim(std::string& s)
{
	return ltrim(rtrim(s, " []"), " []");
}

void    Location::toString(void) const
{
	std::cout << "Location Content \n" << "Root: " << getRoot() << "; AutoIndex: " << getAutoIndex() << "; Methods: " << getMethods() << "\n";
}
void Location::exceptionsManager(std::string c)
{
	throw std::runtime_error("Error: " + c);
}