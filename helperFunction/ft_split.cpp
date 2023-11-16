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

std::string& rtrim(std::string& s, std::string t)
{
    s.erase(s.find_last_not_of(t.c_str()) + 1);
    return s;
}

std::string& ltrim(std::string& s, std::string t)
{
    s.erase(0, s.find_first_not_of(t.c_str()));
    return s;
}

std::string& advanced_trim(std::string& s, std::string trimSep)
{
    return ltrim(rtrim(s, trimSep.c_str()), trimSep.c_str());
}

// bool	regFile(std::string path)
// {
// 	struct stat st;

// 	if (stat(path.c_str(), &st) != 0)
// 		return false;
// 	return S_ISREG(st.st_mode);
// }
