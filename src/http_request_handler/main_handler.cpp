#include "../../include/webserv.hpp"

Response &generate_response(Request &request) {
	if (request.getMethod() == "GET") {
		return handle_get_request(request);
	}
	else if (request.getMethod() == "POST") {
		return handle_post_request(request);
	}
	else
	{
		// If the method is not supported, return a 501 Not Implemented response
		Response *response = new Response("HTTP/1.1", 501, "Not Implemented");
		response->addHeader("Content-Type", "text/plain");
		response->setBody("Method Not Implemented");
		return *response;
	}
}