#include "../includes/configParser.hpp"
#include "configParser.hpp"

bool configParser::loadFile()
{
    return false;
}

void configParser::parseLine(const std::string &)
{
}

configParser::configParser()
{
}

configParser::configParser(const std::string &)
{
}
configParser::~configParser()
{
}

std::string configParser::getWorkingDir(void) const
{
    return std::string();
}

std::string configParser::getLogFile(void) const
{
    return std::string();
}

int configParser::getMaxConnections(void) const
{
    return 0;
}

int configParser::getPort(void) const
{
    return 0;
}

bool configParser::getAutoIndex(void) const
{
    return false;
}

void configParser::setLogFile(std::string)
{
}

void configParser::setWorkingDir(std::string)
{
}

void configParser::setMaxConnections(int)
{
}

void configParser::setPort(int)
{
}

void configParser::setAutoIndex(bool)
{
}
