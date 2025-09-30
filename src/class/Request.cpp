#include "../../include/webserv.hpp"

Request::Request() : _isBad(0) {}

Request::Request(std::string method, std::string uri, std::string version)
	: _method(method), _uri(uri), _version(version) {}

Request::~Request() {}


//Setters
void Request::setisBad(bool is)
{
	_isBad = is;
}

void Request::sethasHeader(bool has) {
	_hasHeader = has;
}

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

void Request::setBody(const std::string body) {
	_body = body;
}

//GETTERS

const bool &Request::isBad() const{
	return _isBad;
}

const bool &Request::hasHeader() const {
	return _hasHeader;
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

const std::string &Request::getHeader(std::string key) const {
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end()) {
		return it->second;
	}
	static const std::string empty_string; // Return an empty string if the header is not found
	return empty_string;
}

const std::string &Request::getBody() const {
	return _body;
}

void Request::printRequest() const
{
    // Box top
    std::cout << BLUE << "┌───────────────────── REQUEST ─────────────────────┐" << RESET << std::endl;

    // Body of the block
    std::cout << BLUE << "│" << RESET << " Is Bad   : " << (_isBad ? RED "true" RESET : GREEN "false" RESET) << std::endl;
    std::cout << BLUE << "│" << RESET << " Method   : " << CYAN << getMethod() << RESET << std::endl;
    std::cout << BLUE << "│" << RESET << " URI      : " << YELLOW << getUri() << RESET << std::endl;
    std::cout << BLUE << "│" << RESET << " Version  : " << getVersion() << std::endl;

    std::cout << BLUE << "│" << RESET << " Headers  :" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
    {
		if (it->first == "Cookie")
			continue;
        std::cout << BLUE << "│   " << RESET 
                  << CYAN << it->first << RESET 
                  << " : " << it->second << std::endl;
    }

	//Body
    // std::cout << BLUE << "│" << RESET << " Body     : " << MAGENTA << getBody() << RESET << std::endl;

    // Box bottom
    std::cout << BLUE << "└───────────────────────────────────────────────────┘" << RESET << std::endl;
}
