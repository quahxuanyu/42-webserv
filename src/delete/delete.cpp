#include "../../include/webserv.hpp"

bool delete_file(std::string path)
{
	if (std::remove(path.c_str()) != 0) {
		return false; // Deletion failed
	}
	return true; // Deletion successful
}

Response &handle_delete_request(Server &server, Request &request)
{
	Response *response = new Response();
	std::string path;

	std::vector<Location> locations = server.getLocations();
	const Location *location = matchLocation(locations, request.getUri());

	if (location == NULL)
	{	
		handle_response_error(*response, server.getPage(404), 404);
		return *response;
	}
	else if(!location->getMethods().count("DELETE"))
	{	
		handle_response_error(*response, server.getPage(405), 405);
		return *response;
	}
	else if (atol(request.getHeader("Content-Length").c_str()) > server.getBodySizeLimit())
	{	
		handle_response_error(*response, server.getPage(413), 413);
		return *response;
	}

    // Check Alias & Root
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

    if (delete_file(path) == true)
    {
        std::cout << GREEN << "DELETE Success: " << path << RESET << std::endl;
        response->setVersion("HTTP/1.1");
        response->setStatusCode(200);
        response->setBody("File deleted successfully.");
    }
    else
    {
        std::cout << RED << "DELETE Error: " << path << RESET << std::endl;
        response->setVersion("HTTP/1.1");
        response->setStatusCode(404);
        response->setBody("File deletion failed.");
    }    
    return *response;
}