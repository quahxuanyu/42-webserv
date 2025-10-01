
#include "../include/webserv.hpp"


bool isFile(const std::string &path)
{
	char cwd[256];
	if (!getcwd(cwd, sizeof(cwd)))
		throw std::runtime_error("Get cdw failed");
	std::string const cwd_str = cwd;

	std::string ab_path = (cwd_str + path);
	// std::cout << MAGENTA << "path:" << ab_path << RESET << std::endl;

	struct stat sb;
	if (stat(ab_path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
		return true;
	return false;
}

bool isFileNoCwd(const std::string &path)
{
	// std::cout << MAGENTA << "file path : " << path << RESET << std::endl;
	struct stat sb;
	if (stat(path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
		return true;
	return false;
}

bool isDirectory(const std::string &path)
{
	// std::cout << MAGENTA << "dir path : " << path << RESET << std::endl;
	struct stat sb;
	if (stat((path).c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		return true;
	return false;
}

// Find the longest matching location block path, return NULL if not match with any
const Location* matchLocation(const std::vector<Location> &locations, const std::string& uri)
{
	const Location *best_match = NULL;
	size_t best_len = 0;

	for (size_t i = 0; i < locations.size(); i++)
	{
		const std::string &path = locations[i].getPath();
		//handle file/dir URI
		if (uri.find(path) == 0)
		{ // starts with path
			if (path.length() > best_len)
			{
				best_match = &locations[i];
				best_len = path.length();
			}
		}
	}
	return best_match; // may be null if no match
}

std::string read_file(std::ifstream &src)
{
	std::string content;
	std::string line;

	// Loop through each line of the file
	while (std::getline(src, line))
	{ 
		content += line;

		// Only add a newline if it's not the last line
		if (!src.eof()) 
			content += "\n";
	}
	return (content);
}

void createErrorMap()
{
	httpErrorMessages[400] = "Bad Request";
	httpErrorMessages[401] = "Unauthorized";
	httpErrorMessages[403] = "Forbidden";
	httpErrorMessages[404] = "Not Found";
	httpErrorMessages[405] = "Method Not Allowed";
	httpErrorMessages[408] = "Request Timeout";
	httpErrorMessages[411] = "Length Required";
	httpErrorMessages[413] = "Payload Too Large";
	httpErrorMessages[500] = "Internal Server Error";
	httpErrorMessages[501] = "Not Implemented";
	httpErrorMessages[504] = "Gateway Timeout";
	httpErrorMessages[505] = "HTTP Version Not Supported";
}

void print_request(Request &request)
{
	std::cout << request.getMethod() << " "
				<< request.getUri() << " "
				<< request.getVersion() << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
}

void printAllSessionData()
{
	std::cout << "==== SESSSIONSSSS ====" << std::endl;
	std::map<std::string, Session>::iterator i;
	for (i = sessions.begin(); i != sessions.end(); ++i)
	{
		std::cout << "Session id: [" << i->first << "]" << std::endl;
		std::cout << RED << "USERNAME: " << i->second._data["username"] << RESET <<std::endl; 
		std::cout << RED << "VISIT COUNT: " << i->second._visit_count << RESET <<std::endl; 
	}
}

void printSessionData(Session &session)
{
	std::cout << RED << "USERNAME: " << session._data["username"] << RESET <<std::endl; 
	std::cout << RED << "VISIT COUNT: " << session._visit_count << RESET <<std::endl; 
}
