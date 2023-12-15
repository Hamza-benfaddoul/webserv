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


std::string generateDirectoryListing(const std::string& directoryPath)
{
    std::stringstream html;
    html << "<style> h2 {"
         << "color: #aaa;"
         << "font-size: 30px;"
         << "line-height: 40px;"
         << "font-style: italic;"
         << "font-weight: 200;"
         << "margin: 40px;"
         << "text-align: center;"
         << "text-shadow: 1px 1px 1px rgba(255, 255, 255, 0.7);"
         << "}"
         << ".box {"
         << "background: #fff;"
         << "border-radius: 2px;"
         << "box-shadow: 0 0 50px rgba(0, 0, 0, 0.1);"
         << "margin: 30px 5%;"
         << "padding: 5%;"
         << "}"
         << "@media (min-width: 544px) {"
         << ".box {"
         << "margin: 40px auto;"
         << "max-width: 440px;"
         << "padding: 40px;"
         << "}"
         << "}"
         << ".directory-list ul {"
         << "margin-left: 10px;"
         << "padding-left: 20px;"
         << "border-left: 1px dashed #ddd;"
         << "}"
         << ".directory-list li {"
         << "list-style: none;"
         << "color: #888;"
         << "font-size: 17px;"
         << "font-style: italic;"
         << "font-weight: normal;"
         << "}"
         << ".directory-list a {"
         << "border-bottom: 1px solid transparent;"
         << "color: #888;"
         << "text-decoration: none;"
         << "transition: all 0.2s ease;"
         << "}"
         << ".directory-list a:hover {"
         << "border-color: #eee;"
         << "color: #000;"
         << "}"
         << ".directory-list .folder,"
         << ".directory-list .folder > a {"
         << "color: #777;"
         << "font-weight: bold;"
         << "}"
         << ".directory-list li:before {"
         << "margin-right: 10px;"
         << "content: \"\";"
         << "height: 20px;"
         << "vertical-align: middle;"
         << "width: 20px;"
         << "background-repeat: no-repeat;"
         << "display: inline-block;"
         << "background-image: url(\"data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 100 100'><path fill='lightgrey' d='M85.714,42.857V87.5c0,1.487-0.521,2.752-1.562,3.794c-1.042,1.041-2.308,1.562-3.795,1.562H19.643 c-1.488,0-2.753-0.521-3.794-1.562c-1.042-1.042-1.562-2.307-1.562-3.794v-75c0-1.487,0.521-2.752,1.562-3.794 c1.041-1.041,2.306-1.562,3.794-1.562H50V37.5c0,1.488,0.521,2.753,1.562,3.795s2.307,1.562,3.795,1.562H85.714z M85.546,35.714 H57.143V7.311c3.05,0.558,5.505,1.767,7.366,3.627l17.41,17.411C83.78,30.209,84.989,32.665,85.546,35.714z' /></svg>');background-position: center 2px;background-size: 60% auto;background-size: 60% auto;</style><body><h2>Directoryory List</h2><div class=\"box\"><ul class=\"directory-list\">";

    DIR* dir;
    struct dirent* entry;
    struct stat entryStat;

    // Open the directory
    dir = opendir(directoryPath.c_str());

    if (dir != NULL) {
        // Read directory entries
        while ((entry = readdir(dir)) != NULL) {
            std::string name = entry->d_name;

            // Skip current and parent directory entries
            if (name != "." && name != "..") {
                std::string fullPath = directoryPath + "/" + name;
                stat(fullPath.c_str(), &entryStat);

                // Check if the entry is a file or a directory
                std::string href;
                std::string listItemClass;
                std::string iconSrc;
                if (S_ISDIR(entryStat.st_mode)) {
                    href = "/" + name + "/";
                    listItemClass = "folder";
                    iconSrc = "https://img.icons8.com/material-rounded/24/folder-invoices.png";  // Replace with the actual path to your folder icon
                } else {
                    href = "/" + name;
                    listItemClass = "";
                    iconSrc = "https://img.icons8.com/doodle/24/file--v1.png";  // Replace with the actual path to your file icon
                }

                // Add an entry to the HTML list
                html << "<li style='height: 40px !important;' class=\"" << listItemClass << "\"><a href=\"" << href << "\"><img width='24' height='24'  src=\"" << iconSrc << "\" alt=\"\" />" << name << "</a></li>";
            }
        }
        closedir(dir);
    } else {
        // Handle directory open error
        std::cerr << "Error opening directory: " << strerror(errno) << std::endl;
    }

    html << "</ul></div>";
    return html.str();
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
