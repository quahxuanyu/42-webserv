#include "../include/Request.hpp"

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