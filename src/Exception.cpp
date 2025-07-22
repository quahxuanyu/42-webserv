#include "../include/webserv.hpp"

ParseException::ParseException(std::string msg)
{
	_msg = msg;
}

const char* ParseException::what() const throw()
{
	return _msg.c_str();
}

ParseException::~ParseException() throw (){}

explicit SyntaxError::SyntaxError(const std::string &msg) : std::runtime_error("Syntax Error: " + msg) {}