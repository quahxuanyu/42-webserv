#ifndef CGI_HPP
#define CGI_HPP
#include "webserv.hpp"

class Cgi {
	private:
		Request		&_request;
		Response	*_response;
		char 		*_envp[];
	public:
		Cgi();
		Cgi(Request &req);
		~Cgi();
		Response &execute();
};

#endif