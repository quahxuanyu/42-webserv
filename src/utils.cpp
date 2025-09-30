
#include "../include/webserv.hpp"


bool isFile(const std::string &path)
{
	char cwd[256];
	if (!getcwd(cwd, sizeof(cwd)))
		throw std::runtime_error("Get cdw failed");
	std::string const cwd_str = cwd;

	std::string ab_path = (cwd_str + path);
	std::cout << MAGENTA << "path:" << ab_path << RESET << std::endl;

	struct stat sb;
	if (stat(ab_path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
		return true;
	return false;
}

bool isFileNoCwd(const std::string &path)
{
	std::cout << MAGENTA << "file path : " << path << RESET << std::endl;
	struct stat sb;
	if (stat(path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
		return true;
	return false;
}

bool isDirectory(const std::string &path)
{
	std::cout << MAGENTA << "dir path : " << path << RESET << std::endl;
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

std::map<int, std::string> createErrorMap()
{
	std::map<int, std::string> m;
	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[405] = "Method Not Allowed";
	m[408] = "Request Timeout";
	m[411] = "Length Required";
	m[413] = "Payload Too Large";
	m[500] = "Internal Server Error";
	m[501] = "Not Implemented";
	m[504] = "Gateway Timeout";
	m[505] = "HTTP Version Not Supported";
	return m;
}
std::map<int, std::string> httpErrorMessages = createErrorMap();