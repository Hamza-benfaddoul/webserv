#pragma once

// #define DEFAULT_PATH "./config/"
#define DEFAULT_PATH "./config/default.yaml"
#define DEFAULT_WORKING_DIR "./www/"

// define
#define MAX_CONNECTIONS 5

// c++ headers
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <exception>
#include <cstring>
#include <vector>
#include <map>
#include <sys/stat.h>
#include <fstream>
#include <set>
#include <cstdlib>

// c headers
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/epoll.h>
// config parser
#include "configParser.hpp"

// server
#include "../src/Server/Server.hpp"
#include "../src/Server/Cluster.hpp"

// helper function

std::vector<std::string>	ft_split(std::string str, std::string needed);
std::string&				rtrim(std::string& s, std::string t);
std::string&				ltrim(std::string& s, std::string t);
std::string&                advanced_trim(std::string& s, std::string trimSep);
bool                        regFile(std::string path);
bool containsOnlyDigits(const std::string &str);
std::string generateDirectoryListing(const std::string& directoryPath);
std::string	getMimeTypeFromExtension(const std::string& path);