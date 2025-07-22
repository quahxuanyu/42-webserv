#include "../../include/webserv.hpp"

Response &handle_post_request(Request &request) {
	// ** WORK IN PROGRESS (continue here) **
	std::string method_env = "REQUEST_METHOD=" + request.getMethod();
	std::string content_type = "CONTENT_TYPE=" + request.getHeader("Content-Type");
	std::string content_length = "CONTENT_LENGTH=" + to_string(request.getBody().length());
	char* envp[] = {
		const_cast<char*>(method_env.c_str()),
		const_cast<char*>(content_type.c_str()),
		const_cast<char*>(content_length.c_str()),
		NULL
	};
	std::string cgi_resopnse = execute(request, envp);

	// Generate response
	Response *response = new Response("HTTP/1.1", 200, "OK");
	set_headers(*response, request);
	response->setBody(cgi_resopnse);
	return *response;
}