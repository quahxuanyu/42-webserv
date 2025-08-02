#include "../../include/webserv.hpp"


Response &handle_post_request(Server &server, Request &request)
{
	Response *response = new Response();

	std::vector<Location> locations = server.getLocations();
	const Location *location = matchLocation(locations, request.getUri());

	if (location == NULL)
	{	
		handle_response_error(*response, server.getPage(404), 404);
		return *response;
	}
	else if(!location->getMethods().count("POST"))
	{	
		handle_response_error(*response, server.getPage(405), 405);
		return *response;
	}

	else if (atol(request.getHeader("Content-Length").c_str()) > server.getBodySizeLimit())
	{	
		handle_response_error(*response, server.getPage(413), 413);
		return *response;
	}

	
	// Check if request is a CGI request
	else if (request.getUri().find(".cgi") != std::string::npos)
	{
		printf("\nDEBUGGGGGG\n");
		delete response;
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
		return parse_noncgi_response();
		// Handle non-CGI POST requests **TEMPORARY**
		// delete response;
		// Response *response = new Response("HTTP/1.1", 200, "OK");
		// set_headers(*response, request);
		// response->setBody("POST request received.");
		// return *response;
	}
}