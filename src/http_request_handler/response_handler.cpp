#include "../../include/webserv.hpp"

Response &generate_response(std::vector<Server> &servers, Request &request)
{
	if (request.getMethod() == "GET") {
		return handle_get_request(servers, request);
	}
	else if (request.getMethod() == "POST") {
		return handle_post_request(request);
	}
	else
	{
		// If the method is not suppsten 127.0.0.1:4243;orted, return a 501 Not Implemented response
		Response *response = new Response("HTTP/1.1", 501, "Not Implemented");
		response->addHeader("Content-Type", "text/plain");
		response->setBody("Method Not Implemented");
		return *response;
	}
}