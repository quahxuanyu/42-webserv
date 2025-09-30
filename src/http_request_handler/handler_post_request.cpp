#include "../../include/webserv.hpp"


void processPostRequest(Server &server, Request &request, Response &response)
{
	// Find the matching location for the request URI
	std::vector<Location> locations = server.getLocations();
	const Location *location = matchLocation(locations, request.getUri());

	if (location == NULL)																	// No matching location
	{	
		handle_response_error(response, server.getPage(404), 404);
	}
	else if(!location->getMethods().count("POST"))											// Method Not Allowed
	{	
		handle_response_error(response, server.getPage(405), 405);
	}
	else if (atol(request.getHeader("Content-Length").c_str()) > server.getBodySizeLimit())	// Exceeds body size limit
	{	
		handle_response_error(response, server.getPage(413), 413);
	}
	else if (request.getUri().find(".cgi") != std::string::npos)							// CGI
	{
		handle_cgi(request, response, server, location);
	}
	else 
	{
		handle_response_error(response, server.getPage(501), 501); // Not Implemented for non-CGI POST requests
	}
}

Response &handle_post_request(Server &server, Request &request)
{
	Response *response = new Response();

	response->setVersion(request.getVersion());
	processPostRequest(server, request, *response);
	return *response;
}