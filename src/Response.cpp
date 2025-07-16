#include "../include/Response.hpp"

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