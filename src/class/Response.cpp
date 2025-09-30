#include "../../include/webserv.hpp"

Response::Response() {}

Response::Response(std::string version, int statusCode, std::string statusMessage)
	: _version(version), _statusCode(statusCode), _statusMessage(statusMessage) {}

Response::~Response() {}

// Setters

void Response::setPath(const std::string path) {
	_path = path;
}

void Response::setVersion(const std::string version) {
	_version = version;
}
void Response::setStatusCode(int code) {
	_statusCode = code;
}

void Response::setStatusMessage(const std::string mesg) {
	_statusMessage = mesg;
}

void Response::addHeader(const std::string key, const std::string value) {
	_headers[key] = value;
}

void Response::setBody(const std::string body) {
	_body = body;
}

// Getter

const std::string &Response::getPath() const {
	return _path;
}

const std::string &Response::getVersion() const {
	return _version;
}

int Response::getStatusCode() const {
	return _statusCode;
}

const std::string &Response::getStatusMessage() const {
	return _statusMessage;
}

const std::map<std::string, std::string> &Response::getHeaders() const
{
	return _headers;
}

const std::string &Response::getHeader(std::string key) const {
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end()) {
		return it->second;
	}
	static const std::string empty_string; // Return an empty string if the header is not found
	return empty_string;
}


const std::string &Response::getBody() const {
	return _body;
}

std::string Response::toString() const {
	std::string response_str;
	response_str += _version + " " + to_string(_statusCode) + " " + _statusMessage + "\r\n";
	std::map<std::string, std::string>::const_iterator i;
	for (i = _headers.begin(); i != _headers.end(); ++i) {
		response_str += i->first + ": " + i->second + "\r\n";
	}
	response_str += "\r\n"+ _body;
	return response_str;
}

void Response::printTerminal() const
{
    // ─── Box Top ─────────────────────────────────────
    std::cout << GREEN 
              << "┌───────────────────── RESPONSE ─────────────────────┐" 
              << RESET << std::endl;

    // Version
    std::cout << GREEN << "│ " << RESET 
              << "Version       : " << CYAN << getVersion() << RESET << std::endl;

    // Status code with color
    std::string codeColor = GREEN; // default (2xx)
    if (_statusCode >= 300 && _statusCode < 400) codeColor = YELLOW;
    else if (_statusCode >= 400) codeColor = RED;

    std::cout << GREEN << "│ " << RESET 
              << "Status Code   : " << codeColor << getStatusCode() << RESET << std::endl;

    // Status message
    std::cout << GREEN << "│ " << RESET 
              << "Status Msg    : " << YELLOW << getStatusMessage() << RESET << std::endl;

    // Path
    std::cout << GREEN << "│ " << RESET 
              << "Path          : " << MAGENTA << getPath() << RESET << std::endl;

    // Headers
    std::cout << GREEN << "│ " << RESET << "Headers:" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); 
         it != _headers.end(); ++it)
    {
        std::cout << GREEN << "│   " << RESET 
                  << CYAN << it->first << RESET 
                  << " : " << it->second << std::endl;
    }

	// Body
    // std::cout << GREEN << "│ " << RESET 
    //           << "Body:" << std::endl;

    // std::istringstream iss(getBody());
    // std::string line;
    // while (std::getline(iss, line)) {
    //     std::cout << GREEN << "│   " << RESET << line << std::endl;
    // }

    // ─── Box Bottom ──────────────────────────────────
    std::cout << GREEN 
              << "└───────────────────────────────────────────────────┘" 
              << RESET << std::endl;
}