#include "../../include/webserv.hpp"

Response &handle_post_request(Request &request) {
	// Check if request is a CGI request
	if (request.getUri().find(".cgi") != std::string::npos) {
		std::string cgi_response = cgi(request);

		if (cgi_response.find("Error") != std::string::npos) {
			// If CGI execution failed, return an error response
			Response *error_response = new Response("HTTP/1.1", 500, "Internal Server Error");
			error_response->setBody("CGI execution failed.");
			return *error_response;
		}
		else {
			Response *response = new Response("HTTP/1.1", 200, "OK");
			response->setBody(get_body(cgi_response)); // Extract body from CGI output
			response->addHeader("Connection", "keep-alive");
			response->addHeader("Content-Length", to_string(response->getBody().length()));
			response->addHeader("Content-Type", get_content_type(cgi_response)); // Extract content type from CGI output
			response->addHeader("Date", get_current_time());
			response->addHeader("Server", "Webserv/1.0"); // ** TEMPORARY, wait until config file is implemented **
			return *response;
		}
	}
	else {
		// Handle non-CGI POST requests **TEMPORARY**
		Response *response = new Response("HTTP/1.1", 200, "OK");
		set_headers(*response, request);
		response->setBody("POST request received.");
		return *response;
	}
}