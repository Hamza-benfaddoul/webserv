/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rakhsas <rakhsas@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:15:12 by rakhsas           #+#    #+#             */
/*   Updated: 2023/11/19 12:30:14 by rakhsas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/serverBlock.hpp"
#include "cstdlib"
#include "../../includes/main.hpp"
serverBlock::serverBlock(): locations() {
	port = -1;
	autoIndex = 0;
	client_max_body_size = 1048576;
	// host = 2130706433;
}
// void serverBlock::setLocation(std::map<std::string, std::string> vec) { this->locations.push_back(vec); }
void serverBlock::setAttribute(std::string key, std::string value) { this->attributes[key] = value; }
void	serverBlock::setLocation( const Location &loc ) {
	this->locations.push_back(loc);
}
std::string serverBlock::getRoot(void) const { return this->root; }
uint32_t serverBlock::getHost(void) const {
	uint32_t res = 0;
	std::string parsed;
	std::stringstream input_stringstream(host);
	if (host.length() == 0)
		return 2130706433;
	int count = 24;
	while (getline(input_stringstream,parsed,'.'))
	{
		// 127.0.0.1
		res |= atoi(parsed.c_str()) << (count);
		count -= 8;
	}
	// std::cout << res << std::endl;
	return res;
}
int     serverBlock::getPort(void) const { return port; }
bool	serverBlock::getAutoIndex( void ) const { return autoIndex; }
void    serverBlock::parseBlock(  )
{
	// std::cout << "Server Block Attributes:" << std::endl;
	std::map<std::string, std::string> attributes = getAttributes();
	for (std::map<std::string, std::string>::iterator attr_it = attributes.begin(); attr_it != attributes.end(); ++attr_it) {
		// if (attr_it->first.compare("server_name") == 0)
		// 	parseServerName(attr_it->second);
		if (attr_it->first.compare("listen") == 0)
			parsePortNumber(attr_it->second);
		else if (attr_it->first.compare("root") == 0)
			parseRoot(attr_it->second);
		else if (attr_it->first.compare("host") == 0)
			parseHost(attr_it->second);
		else if (attr_it->first.compare("autoindex") == 0)
			parseAutoIndex(attr_it->second);
		else if (attr_it->first.compare("client_max_body_size") == 0)
			parseClientMaxBodySize(attr_it->second);
	}
	if (getPort() == -1)
		port = 0;
	if (getRoot().length() == 0)
		root = "www";
	// Location
	// std::cout << "END OF SERVER INFOS\n\n\n";

}

void	serverBlock::parseClientMaxBodySize( std::string value )
{
	char acceptedchars[15] = "0123456789kmg";
	size_t y = 0;
	bool	t = false;
	while (y < value.length())
	{
		for (size_t i = 0; i < 15; i++)
		{
			if (value[y] == acceptedchars[i])
			{
				t = true;
				break;
			}
		}
		if (t == false)
			throw std::invalid_argument("ERROR: invalid argument in client_max_body_size: `" +value+ "`");
		t = false;
		y++;
	}
	client_max_body_size = convertToBytes(value);
}

void serverBlock::parseRoot(std::string value) {
	// std::cout << value << std::endl;
	std::ifstream file;
	if (value.length() > 0)
		file.open( value.c_str() );
	else
		file.open( value.c_str() );
	if (file.is_open())
	{
		this->root = value;
		// close(file);
	}
	else
	{
		std::string a = "ERROR: root: `" + value + "` Not Found !!!.";
		throw std::runtime_error(a);
	}
}

bool containsOnlyAlphabets(const std::string &str) {
	for (size_t i = 0; i < str.length(); ++i) {
		if (!isalpha(str[i])) {
			return false;
		}
	}
	return true;
}
std::string serverBlock::trim(const std::string& str, std::string sep) {
	size_t start = str.find_first_not_of(sep);
	size_t end = str.find_last_not_of(sep);
	if (start != std::string::npos && end != std::string::npos) {
		return str.substr(start, end - start + 1);
	}
	return "";
}
void    serverBlock::parsePortNumber(std::string value) {
	if (containsOnlyDigits(value))
		this->port = std::atoi(value.c_str());
	else
		throw std::runtime_error("ERROR: PORT MUST CONTAIN ONLY DIGITS !!!.");
	if (port <= 0 || port > 65535)
		throw std::runtime_error("ERROR: PORT MUST BE POSITIF AND < 65535 !!!.");
	// std::cout << this->port << std::endl;
}
void    serverBlock::parseHost(std::string value)
{
	int points = 0;
	char str[] = "0123456789.";
	for (size_t i = 0; i < value.length(); i++)
	{
		for (size_t y = 0; y <= strlen(str); y++)
		{
			if (value.at(i) == '.' && value.at(i) == str[y])
			{
				// std::cout << value.at(i) << std::endl;
				points++;
			}
			if (value.at(i) == str[y])
				break ;
			else if (y == strlen(str) && value.at(i) != str[y])
				throw std::runtime_error("ERROR: IP address !!!. ");
			else if (value.at(i) != str[y])
				continue;
		}
	}
	// std::cout << points << std::endl;
	if (points > 3)
		throw std::runtime_error("ERROR: IP address Must Contain only three dots !!!. ");
	std::string parsed;
	std::stringstream input_stringstream(value);

	int	count = 0;
	while (getline(input_stringstream,parsed,'.'))
	{
		count++;
		if (count > 4)
			throw std::runtime_error("ERROR: the host IP address invalid !!!.");
		if (atoi(parsed.c_str()) < 0 || atoi(parsed.c_str()) > 255)
			throw std::runtime_error("ERROR: The IP address out of range !!!.");
	}
	if (count < 4)
		throw std::runtime_error("ERROR: Invalid IP address");
	host = value;
}
void serverBlock::parseAutoIndex(std::string value)
{
	if (std::string("on").compare(value) != 0 && std::string("off").compare(value) != 0 )
		throw std::runtime_error("ERROR: Auto Index EXPECTS just `on` or `off` !!!.");
	else if (value == "on")
		autoIndex = true;
	else
		autoIndex = false;
}

Location	serverBlock::getLocationByPath(std::string path)
{
	for (size_t i = 0; i != locations.size(); i++) {
		Location location = locations.at(i);
		if (path == location.getLocationPath())
		{
			return location;
		}
	}
	throw std::runtime_error("No Location Found");
}
