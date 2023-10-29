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

// c headers
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

// config parser
#include "configParser.hpp"

// server
#include "../src/Server/Server.hpp"
#include "../src/Server/Cluster.hpp"


