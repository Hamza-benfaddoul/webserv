#include "../includes/main.hpp"
#include "dirent.h"

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

bool containsOnlyDigits(const std::string &str) {
	for (size_t i = 0; i < str.length(); ++i) {
		if (!isdigit(str[i])) {
			return false;
		}
	}
	return true;
}



std::string	getMimeTypeFromExtension(const std::string& path)
{
	std::map<std::string, std::string> extensionToMimeType;
		extensionToMimeType[".jpg"] = "image/jpeg";
		extensionToMimeType[".jpeg"] = "image/jpeg";
		extensionToMimeType[".png"] = "image/png";
		extensionToMimeType[".gif"] = "image/gif";
		extensionToMimeType[".bmp"] = "image/bmp";
		extensionToMimeType[".ico"] = "image/x-icon";
		extensionToMimeType[".mp4"] = "video/mp4";
		extensionToMimeType[".ogg"] = "video/ogg";
		extensionToMimeType[".avi"] = "video/x-msvideo";
		extensionToMimeType[".mov"] = "video/quicktime";
		extensionToMimeType[".3gp"] = "video/3gpp";
		extensionToMimeType[".wmv"] = "video/x-ms-wmv";
		extensionToMimeType[".ts"] = "video/mp2t";
		extensionToMimeType[".pdf"] = "application/pdf";
		extensionToMimeType[".html"] = "text/html";
		extensionToMimeType[".css"] = "text/css";
		extensionToMimeType[".php"] = "text/html";
		extensionToMimeType[".js"] = "application/javascript";
	// Find the last dot in the path
	size_t dotPos = path.find_last_of('.');
	if (dotPos != std::string::npos) {
		std::string extension = path.substr(dotPos);
		if (!extension.empty()) {
			std::map<std::string, std::string>::iterator it = extensionToMimeType.find(extension);
			// std::cout << "video extension: " << extension << "\n";
			if (it != extensionToMimeType.end()) {
				return it->second; // Return the corresponding MIME type
			}
		}
	}
	return "application/octet-stream";
}

size_t	get_time(char tmp)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	if (tmp == 'm')
		return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
	else if (tmp == 's')
		return (tv.tv_sec);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	sendErrorResponse( int CODE, std::string ERRORTYPE, std::string errorTypeFilePath, int _fd) {
	std::ifstream file(errorTypeFilePath.c_str());
	std::string content;
	if (file.is_open())
	{
		{
			std::string line;
			while (getline(file, line))
			{
				content += line;
			}
		}
	}
	file.close();
	std::stringstream response;
	response << "HTTP/1.1 " << CODE << " " << ERRORTYPE << "\r\n";
	response << "Content-Type: text/html; charset=UTF-8\r\n";
	response << "Content-Length: " << content.length() << "\r\n";
	response << "\r\n";
    response << content;

	write(_fd, response.str().c_str(), response.str().length());
}
void	sendHeadErrorResponse( int CODE, std::string ERRORTYPE, int _fd) {
	std::stringstream response;
	response << "HTTP/1.1 " << CODE << " " << ERRORTYPE << "\r\n";
	response << "Content-Type: text/html; charset=UTF-8\r\n";
	response << "\r\n";

	write(_fd, response.str().c_str(), response.str().length());
}


size_t FileSize(std::string filename) {
    FILE* file = fopen(filename.c_str(), "rb");
    if (file == NULL) {
        // handle file open error
        return -1;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);

    fclose(file);

    return size;
}

bool isValidClientMaxBodySize(const char *value) {
    char *endptr;
    long size = strtol(value, &endptr, 10);
    if (endptr == value) {
        return false;
    }
	(void) size;
    while (std::isspace(*endptr)) {
        ++endptr;
    }
    if (*endptr != '\0') {
        char unit = std::tolower(*endptr);
        if (unit == 'k' || unit == 'm' || unit == 'g') {
        } else {
            return false;
        }
    }
    while (std::isspace(*++endptr)) {
    }
    return *endptr == '\0';
}

long convertToBytes(const char *value) {
    char *endptr;
    long long size = strtol(value, &endptr, 10);

    if (endptr == value) {
        return -1;
    }
    while (std::isspace(*endptr)) {
        ++endptr;
    }
    if (*endptr != '\0') {
        char unit = std::tolower(*endptr);
        if (unit == 'k') {
            size *= 1024;
        } else if (unit == 'm') {
            size *= 1024 * 1024;
        } else if (unit == 'g') {
            size *= 1024 * 1024 * 1024;
        } else {
            return -1;
        }
    }
    return size;
}

long	convertToBytes( std::string value )
{
	bool status = isValidClientMaxBodySize(value.c_str());
	long size = 0;
	if (status == true)
	{
        size = convertToBytes(value.c_str());
	    if (size == -1)
    	    throw std::invalid_argument("ERROR: invalid argument in client_max_body_size: `" +value+ "`");
    }
    else
        throw std::invalid_argument("ERROR: invalid argument in client_max_body_size: `" +value+ "`");
    return size;
}
