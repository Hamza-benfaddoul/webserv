/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rakhsas <rakhsas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:15:12 by rakhsas           #+#    #+#             */
/*   Updated: 2023/10/30 10:06:15 by rakhsas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/serverBlock.hpp"

serverBlock::serverBlock() {

}
void serverBlock::setLocation(std::map<std::string, std::string> vec) { this->locations.push_back(vec); }
void serverBlock::setAttribute(std::string key, std::string value) { this->attributes[key] = value; }
std::string serverBlock::getServerName(void) const { return this->serverName; }
std::string serverBlock::getRoot(void) const { return this->root; }
std::string serverBlock::getHost(void) const { return host; }
int     serverBlock::getPort(void) const { return port; }

void    serverBlock::parseBlock(  )
{
    std::cout << "Server Block Attributes:" << std::endl;
    std::map<std::string, std::string> attributes = getAttributes();
    for (std::map<std::string, std::string>::iterator attr_it = attributes.begin(); attr_it != attributes.end(); ++attr_it) {
        if (attr_it->first.compare("server_name") == 0)
            parseServerName(attr_it->second);
        else if (attr_it->first.compare("listen") == 0)
            parsePortNumber(attr_it->second);
        else if (attr_it->first.compare("root") == 0)
            parseRoot(attr_it->second);
        else if (attr_it->first.compare("host") == 0)
            parseHost(attr_it->second);
    }
    // std::cout << "Server Block Locations:" << std::endl;
    // std::vector<std::map<std::string, std::string> > locations = getLocations();
    // for (std::vector<std::map<std::string, std::string> >::iterator loc_it = locations.begin(); loc_it != locations.end(); ++loc_it) {
    //     std::cout << "Location Attributes:" << std::endl;
    //     for (std::map<std::string, std::string>::iterator loc_attr_it = loc_it->begin(); loc_attr_it != loc_it->end(); ++loc_attr_it) {
    //         std::cout << loc_attr_it->first << ": " << loc_attr_it->second << std::endl;
    //     }
    // }
    // std::cout << "END OF SERVER INFOS\n\n\n";

}

void serverBlock::parseRoot(std::string value) const {
    // std::cout << value << std::endl;
    std::ifstream file;
    if (value.length() > 0)
        file.open( value );
    else
        file.open( value );
	if (file.is_open())
	{
		// if (!regFile(getConfigFilePath()))
		// 	throw notARegularFile();
		// else if (getFileSize( getConfigFilePath()) == 0)
		// 	throw configFileIsEmpty();
    }
}

bool containsOnlyDigits(const std::string &str) {
    for (size_t i = 0; i < str.length(); ++i) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
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
void    serverBlock::parseServerName(std::string value) {
    if (value.find("[[") != std::string::npos || value.find("]]") != std::string::npos || value.find("][") != std::string::npos || value.find("[]") != std::string::npos)
        throw std::runtime_error("ERROR: Found an invalid pattern related to []!!!.");
    if (value.at(0) == '[')
    {
        value = trim(value, " []");
        if (value.find(",,") != std::string::npos)
            throw std::runtime_error("ERROR: Found an invalid pattern related to ,,!!!.");
        std::stringstream ss(value);
        std::string word, word1;
        while (ss >> word)
        {
            if (word == ",")
                word = word.substr(1);
            word1.append(trim(word, " ,"));
            word1.append(" ");
        }
        this->serverName = word1;
    } else {
        if (containsOnlyAlphabets(value))
            this->serverName = value;
        else
            throw std::runtime_error("ERROR: `server_name` MUST CONTAIN ONLY ALPHABETS!!!.");
    }
}
void    serverBlock::parsePortNumber(std::string value) {
    if (containsOnlyDigits(value))
        this->port = atoi(value.c_str());
    else
        throw std::runtime_error("ERROR: PORT MUST CONTAIN ONLY DIGITS !!!.");
    if (port <= 0 || port > 65535)
        throw std::runtime_error("ERROR: PORT MUST BE POSITIF AND < 65535 !!!.");
}
void    serverBlock::parseHost(std::string value) const
{
    (void)value;
    // std::cout << value << std::endl;
    std::string parsed;
    std::stringstream input_stringstream(value);

    while (getline(input_stringstream,parsed,'.'))
    {
        std::cout << parsed << std::endl;
    }
}
