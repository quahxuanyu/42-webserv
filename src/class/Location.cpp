#include "../../include/webserv.hpp"

Location::Location()
{
	_redirect_code = 0;
	_autoindex = 0;
	cgi = 0;
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

std::string Location::getRedirectUrl() const
{
	return _redirect_url;
}

size_t Location::getRedirectCode() const
{
	return _redirect_code;
}

bool Location::getAutoIndex() const
{
	return _autoindex;
}

bool Location::hasRoot () const
{
	return (!_root.empty());
}

bool Location::hasAlias () const
{
	return (!_alias.empty());
}

bool Location::hasIndex () const
{
	return (!_index.empty());
}

bool Location::hasRedirectUrl () const
{
	return (!_redirect_url.empty());
}


std::string Location::getRoot() const
{
	return _root;
}
std::string Location::getAlias() const
{
	return _alias;
}
std::string Location::getIndex() const
{
	return _index;
}

std::set<std::string> Location::getMethods() const
{
	return _methods;
}

std::string Location::getPath() const
{
	return _path;
}

void Location::printInfo() const
{
	std::cout << "=== Location's info ===" << std::endl;
	std::cout << "Path : " << _path << std::endl;
	std::cout << "Root : " << _root << std::endl;
	std::cout << "Alias : " << _alias << std::endl;
	std::cout << "Index : " << _index << std::endl;
	std::cout << "CGI : " << cgi << std::endl;
	std::cout << "AutoIndex : " << _autoindex << std::endl;
	std::cout << "Redirect Code : " << _redirect_code << std::endl;
	std::cout << "Redirect Url : " << _redirect_url << std::endl;
	std::cout << "Methods : " << std::endl;

	std::set<std::string>::const_iterator it;
	for (it = _methods.begin(); it != _methods.end(); ++it)
		std::cout << *it << std::endl;

}