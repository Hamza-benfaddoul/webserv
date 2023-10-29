#ifndef CONFIGLOCATION_HPP
#define CONFIGLOCATION_HPP

#include "header.hpp"

class configLocation
{
private:
    std::map<std::string, std::string> params;
public:
    configLocation(/* args */);
    ~configLocation();
};

configLocation::configLocation(/* args */)
{
}

configLocation::~configLocation()
{
}



#endif