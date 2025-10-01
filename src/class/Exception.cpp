#include "../../include/webserv.hpp"

ParseException::ParseException(std::string msg)
{
	_msg = msg;
}

const char* ParseException::what() const throw()
{
	return _msg.c_str();
}

ParseException::~ParseException() throw (){}

SyntaxError::SyntaxError(const std::string &msg) : std::runtime_error(msg) {}