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
    html << "<html><head><title>Directory Listing</title>";
    html << "<style>body { margin: 0; font-family: \"HelveticaNeue-Light\", \"Helvetica Neue Light\", \"Helvetica Neue\", Helvetica, Arial, \"Lucida Grande\", sans-serif;";
    html << "font-weight: 300; color: #404040; }";
    html << "h2 { width: 93%; margin-left: auto; margin-right: auto; }";
    html << "table { width: 100%; background: white; border: 0; table-layout: auto; }";
    html << "table caption { background: transparent; color: #222222; font-size: 1rem; font-weight: bold; }";
    html << "table thead { background: whitesmoke; }";
    html << "table thead tr th, table thead tr td { padding: 0.5rem 0.625rem 0.625rem; font-size: 0.875rem; font-weight: bold; color: #222222; }";
    html << "table tfoot { background: whitesmoke; }";
    html << "table tfoot tr th, table tfoot tr td { padding: 0.5rem 0.625rem 0.625rem; font-size: 0.875rem; font-weight: bold; color: #222222; }";
    html << "table tr th, table tr td { padding: 0.5625rem 0.625rem; font-size: 0.875rem; color: #222222; text-align: left; }";
    html << "table tr.alt, table tr:nth-of-type(even) { background: #f9f9f9; }";
    html << "table thead tr th, table tfoot tr th, table tfoot tr td, table tbody tr th, table tbody tr td, table tr td { display: table-cell; line-height: 1.125rem; }";
    html << "a { text-decoration: none; color: #3498db; }";
    html << "a:hover { text-decoration: underline; }";
    html << "a:visited { color: #8e44ad; }";
    html << ".img-wrap { vertical-align: middle; display: inline-block; margin-right: 8px; margin-bottom: 2px; width: 16px; }";
    html << "td img { display: block; width: 100%; height: auto; }";
    html << "@media (max-width: 600px) { table tr > *:nth-child(2), table tr > *:nth-child(3), table tr > *:nth-child(4) { display: none; }";
    html << "h1 { font-size: 1.5em; } }";
    html << "@media (max-width: 400px) { h1 { font-size: 1.125em; } }</style></head>";
    html << "<body><h2>Directory Listing</h2><table id=\"indexlist\">";
    html << "<tr class=\"indexhead\"><th class=\"indexcolicon\"><span></span></th><th class=\"indexcolname\"><a>Name</a></th></tr>";
    html << "<tr class=\"indexbreakrow\"><th colspan=\"5\"><hr /></th></tr>";

    DIR* dir;
    struct dirent* entry;

    // Open the directory
    dir = opendir(directoryPath.c_str());

    if (dir != NULL) {
        // Read directory entries
        while ((entry = readdir(dir)) != NULL) {
            std::string name = entry->d_name;

            // Skip current and parent directory entries
            if (name != "." && name != "..") {
                // Add an entry to the HTML table
                html << "<tr class=\"even\"><td class=\"indexcolname\"><a href=\"" << name << "\">" << name << "</a></td></tr>";
                html << "<tr class=\"indexbreakrow\"><th colspan=\"5\"><hr /></th></tr>";
            }
        }

        // Close the directory
        closedir(dir);
    } else {
        // Handle directory open error
        std::cerr << "Error opening directory: " << strerror(errno) << std::endl;
    }

    html << "</table></body></html>";
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
