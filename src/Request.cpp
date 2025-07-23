#include "../include/webserv.hpp"

Request::Request() {}

Request::Request(std::string method, std::string uri, std::string version)
	: _method(method), _uri(uri), _version(version) {}

Request::~Request() {}

void Request::setMethod(const std::string method) {
	_method = method;
}

void Request::setUri(const std::string uri) {
	_uri = uri;
}

void Request::setVersion(const std::string version) {
	_version = version;
}

void Request::addHeader(const std::string key, const std::string value) {
	_headers[key] = value;
}

const std::string &Request::getMethod() const {
	return _method;
}

const std::string &Request::getUri() const {
	return _uri;
}

const std::string &Request::getVersion() const {
	return _version;
}

const std::map<std::string, std::string> &Request::getHeaders() const {
	return _headers;
}

void Request::printRequest() const
{
	std::cout << " == REQUEST == " << std::endl;
	std::cout << "Method : " << getMethod() << std::endl;
	std::cout << "URI : " << getUri() << std::endl;
	std::cout << "Version : " << getVersion() << std::endl;
	std::cout << "Headers : " << std::endl;

	std::map<std::string, std::string>::const_iterator it;
	for (it = _headers.begin(); it != _headers.end(); ++it)
	{
		std::cout << it->first<< " : " << it->second << std::endl;
	}
	
}