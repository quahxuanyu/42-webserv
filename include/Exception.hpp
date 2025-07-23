#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP
#include <iostream>
#include <exception>

class ParseException: public std::exception {
	
	private:
		std::string _msg;

	public:
		ParseException(std::string msg);
		const char* what() const throw();
		~ParseException() throw ();
};

class SyntaxError : public std::runtime_error {

	public:

	//call this only when SyntaxError()
	explicit SyntaxError(const std::string &msg);
};

#endif