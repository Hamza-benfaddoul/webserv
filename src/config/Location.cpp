#include "../../includes/Location.hpp"

Location::Location()
{
    root = "www";
    locationPath = "/";
}

void    Location::setAttribute( const std::string &key, const std::string &value ) { this->locationAttributes[key] = value; }

void    Location::parseLocations( void )
{
    std::map<std::string, std::string> location = getLocationAttributes();
    for (std::map<std::string, std::string>::iterator iterator = location.begin(); iterator != location.end(); iterator++)
    {
        // std::cout << "*" << iterator->first<< "*" << std::endl;
        if (iterator->first == "path")
            this->locationPath = iterator->second;
        else if (iterator->first == "root")
            parseRoot(iterator->second);
        else if (iterator->first == "methods")
            parseMethods(iterator->second);
        else if (iterator->first == "autoindex")
			parseAutoIndex(iterator->second);
        else if (iterator->first == "index")
            parseIndex(iterator->second);
			// parsePortNumber(iterator->second);
    	// std::cout << iterator->first << ": " << iterator->second << std::endl;
    }
}

void Location::parseIndex( const std::string &value)
{
    
}

void Location::parseAutoIndex( const std::string &value)
{
	// std::cout << value.find("on") << std::endl;
	if (std::string("on").compare(value) != 0 && std::string("off").compare(value) != 0 )
		throw std::runtime_error("ERROR: Auto Index EXPECTS just `on` or `off` !!!.");
	else
		autoIndex = true;
	// else if ( std::string("off").compare(value) != 0 )
}

void    Location::parseRoot( const std::string &root )
{
    std::cout << root << std::endl;
    if (!root.empty())
    {
        struct stat s;
        if( stat( root.c_str(), &s ) != 0 )
            exceptionsManager("cannot access " + root );
        else if( s.st_mode & S_IFDIR ){}  // S_ISDIR() doesn't exist on my windows 
            // exceptionsManager(root + " is a directory");
        else
            exceptionsManager(root +  " is not a directory");
    }
}

void    Location::parseMethods( const std::string &methods )
{
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