#include "../includes/main.hpp"

std::vector<std::string> ft_split(std::string str, std::string needed)
{
    std::vector<std::string> res;
    size_t  pos;
    std::string token;

    while ((pos = str.find(needed)) != std::string::npos)
    {
        if (pos == 0)
        {
            str = str.substr(needed.length(), str.length());
            continue;
        }
        token = str.substr(0, pos);
        res.push_back(token);
        str = str.substr(pos + needed.length(), str.length());
    }
    token = str.substr(0, str.length());
    res.push_back(token);
    return res;
}