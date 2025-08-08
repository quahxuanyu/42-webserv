#include "../../include/webserv.hpp"

/**
 * @brief After matching location block, returns proper path for the request based on the location block
 * @param server The server object containing the root directory
 * @param location The matched location block
 * @param uri The URI of the request
 */
std::string getPath(const Server &server, const Location *location, std::string uri)
{
	std::string file_path;
	
	if (location == NULL)
		return "";
	else if (location->hasRoot() || location->hasAlias() || server.hasRoot())
	{
		if (location->hasRoot())
			file_path = location->getRoot() + uri;
		else if (location->hasAlias())
		{
			std::string suffix = uri.substr(location->getPath().length()); // get substring after location path (replace location path)
			if (suffix.empty() || suffix[0] != '/')
				suffix = "/" + suffix; // ensure it starts with a slash
			file_path = location->getAlias() + suffix;
		}
		else if (server.hasRoot())
			file_path = server.getRoot() + uri;
	}
	if (isDirectory(file_path) && location->hasIndex())
			file_path += location->getIndex();

	//file_path should be a valid path now
	if (!isFileNoCwd(file_path) && !location->autoindex)
		return "";

	return file_path;
}

/**
 * @brief Process a non-CGI GET request.
 * @param server The server object containing the configuration.
 * @param response The response object to set the response.
 * @param request The request object containing the request details.
 */
void processGetRequest(const Server &server, Response &response, Request &request)
{
	std::string uri = request.getUri();
	std::string file_path;
	std::vector<Location> locations = server.getLocations();
	const Location *location = matchLocation(locations, uri); //1. Match the location block

	// 2. Check if method is allowed on location block
	if(!location->getMethods().count("GET"))
		return (handle_response_error(response, server.getPage(405), 405));
	else
	{
		//3. Get the file path based on the location block
		file_path = getPath(server, location, uri);

		// 4. Check appropriate action according to the location block
		if (location->hasRedirectUrl()) // Redirection
		{
			redirection(response, request, *location);
		}
		else if (location->autoindex && isDirectory(file_path)) // Autoindex
		{
			autoindex(response, request, file_path);
		}
		else // Return Normal File
		{
			response.setPath(file_path);
			normal_file_response(response, request, server, file_path);
		}
	}
}

Response &handle_get_request(Server &server, Request &request)
{
	// CGI DIRECTIVE
	if (request.getUri().find(".cgi") != std::string::npos) 
    {
		// If it is a CGI request, execute the CGI script
		std::string cgi_response = cgi(request);

		if (cgi_response.find("Error") != std::string::npos) {
			// If CGI execution failed, return an error response
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
		// Handle non-CGI GET requests (read and return contents of the file)
		Response *response = new Response();
		response->setVersion(request.getVersion());
		processGetRequest(server, *response, request);
		return *response;
	}
}