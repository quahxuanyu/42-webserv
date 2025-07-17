#include "../include/webserv.hpp"

Response::Response() {}

Response::Response(std::string version, int statusCode, std::string statusMessage)
	: _version(version), _statusCode(statusCode), _statusMessage(statusMessage) {}

Response::~Response() {}

// Setters
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
	response_str += "\r\n" + _body;
	return response_str;
}