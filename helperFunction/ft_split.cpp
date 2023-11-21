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

bool	regFile(std::string path)
{
	struct stat st;

	if (stat(path.c_str(), &st) != 0)
		return false;
	return S_ISREG(st.st_mode);
}


bool endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) {
        return false; // The string is shorter than the suffix, can't end with it
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool endsWithString(const char* str, const char* suffix) {
    size_t strLen = std::strlen(str);
    size_t suffixLen = std::strlen(suffix);

    // Check if the string is at least as long as the suffix
    if (strLen >= suffixLen) {
        // Compare the last 'suffixLen' characters with the provided suffix
        return std::strcmp(str + strLen - suffixLen, suffix) == 0;
    }

    // If the string is shorter than the suffix, it cannot end with the suffix
    return false;
}

int isInclude(const std::vector <char> & source, const char *needed)
{
    for (int i = 0; i < (int) source.size();)
    {
        int j = 0;
        if (source.at(i) == needed[j])
        {
            i++;
            for (int k = j + 1; k < (int)strlen(needed);)
            {
                if (needed[k] == source.at(i))
                {    
                    k++;
                    i++;
                }
                else
                    break;
                if (k == (int)strlen(needed) - 1)
                    return i - (strlen(needed) - 1);
            }
        }
        else
            i++;
    }
    return -1;
}