#include "../../include/webserv.hpp"

Response &handle_post_request(Request &request) {
	// Check if request is a CGI request
	if (request.getUri().find(".cgi") != std::string::npos) {
		std::string cgi_response = cgi(request); // Execute the CGI script

		// If CGI execution failed, return an error response
		if (cgi_response.find("Error") != std::string::npos) {
			Response *error_response = new Response("HTTP/1.1", 500, "Internal Server Error");
			error_response->setBody("CGI execution failed.");
			return *error_response;
		}
		else {
			return parse_cgi_response(cgi_response); // Parse the CGI response and return it
		}
	}
	else 
	{
		// Handle non-CGI POST requests **TEMPORARY**
		Response *response = new Response("HTTP/1.1", 200, "OK");
		set_headers(*response, request);
		response->setBody("POST request received.");
		return *response;
	}
}