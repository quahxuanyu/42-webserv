#include "../../include/webserv.hpp"

Response &handle_post_request(Request &request, char **env) {
	// ** WORK IN PROGRESS (continue here) **
	execute(request.getUri(), env);

	// Generate response
	Response *response = new Response("HTTP/1.1", 200, "OK");
	set_headers(*response, request);
	response->setBody("");
	return *response;
}