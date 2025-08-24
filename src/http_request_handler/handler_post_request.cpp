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
		if (location->hasRoot() || location->hasAlias() || server.hasRoot())
		{
			if (location->hasRoot())
				request.setUri(location->getRoot() + request.getUri());
			else if (location->hasAlias())
			{
				std::string suffix = request.getUri().substr(location->getPath().length()); // get substring after location path (replace location path)
				if (suffix.empty() || suffix[0] != '/')
					suffix = "/" + suffix; // ensure it starts with a slash
				request.setUri(location->getAlias() + suffix);
			}
			else if (server.hasRoot())
				request.setUri(server.getRoot() + request.getUri());
		}
		//delete response;
		std::string cgi_response = cgi(request); // Execute the CGI script

		// If CGI execution failed, return an error response
		if (cgi_response.find("Error") != std::string::npos) {
			handle_response_error(*response, server.getPage(500), 500);
			return *response;
		}
		else {
			if (cgi_response.empty())
			{	
				handle_response_error(*response, server.getPage(504), 504);
				return *response;
			}
			else if (cgi_response == "500")
			{
				handle_response_error(*response, server.getPage(500), 404);
				return (*response);
			}
			else
				return parse_cgi_response(*response, cgi_response); // Parse the CGI response and return it
		}
	}
	else 
	{
		return parse_noncgi_response();
	}
}