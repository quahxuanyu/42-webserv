
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

bool isDirectory(const std::string &path)
{
	
	struct stat sb;
	if (stat((path).c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		return true;
	return false;
}