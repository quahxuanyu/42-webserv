#include "../include/webserv.hpp"

Location::Location()
{
	std::cout << "Location deafult consturctor called" << std::endl;
}

void Location::setPath(std::string path)
{
	_path = path;
}

void Location::setRoot(std::string root)
{
	_root = root;
}

void Location::setAlias(std::string alias)
{
	_alias = alias;
}

void Location::setIndex(std::string index)
{
	_index = index;
}


void Location::setAutoIndex(int on)
{
	_autoindex = on;
}

void Location::setMethod(std::string method)
{
	_methods.insert(method);
}

void Location::setRedirectCode(int code)
{
	_redirect_code = code;
}

void Location::setRedirectUrl(std::string url)
{
	_redirect_url = url;
}

void Location::setCGI(int on)
{
	_cgi = on;
}