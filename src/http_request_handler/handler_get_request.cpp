#include "../../include/webserv.hpp"



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
			//get substring after location path (replace location path)
			std::string suffix = uri.substr(location->getPath().length());
			file_path = location->getAlias() + "/" + suffix;
		}
		else if (server.hasRoot())
			file_path = server.getRoot() + uri;
	}
	
	if (isDirectory(file_path))
	{  
		std::cout << CYAN << "DIRECTORY URI" << RESET << std::endl;
		if (location->hasRedirectUrl())
		{
			//handle redirection
		}
		else if (location->hasIndex())
		{
			file_path += location->getIndex();
		}
		else if (location->autoindex)
		{
			//handle autoindex
		}
		else //what to return?
			return "";
	}

	//file_path should be a valid path now
	if (!isFileNoCwd(file_path))
		return "";
	
	return file_path;
}

void handle_response_error(Response &response, std::string path, int error_code)
{
	response.setPath(path);
	if (path[0] == '/')
        path = "." + path;
	std::ifstream src(path.c_str(), std::ios::binary);
	response.setBody(read_file(src));
	src.close();
	response.setStatusCode(error_code);
	response.setStatusMessage(httpErrorMessages[error_code]);
	return ;
}

/**
 * @brief Reads the content of a file and sets it in the response body.
 */
void process_get_request(const Server &server, Response &response, Request &request)
{
	std::string uri = request.getUri();
	std::string file_path;
	std::string file_content;

	// ** TEMPORARY URI file path handling (need to wait for config file implementation) **
	// if (uri == "/")
	//     file_path = "html/upload.html";
	// else
	//     file_path = "." + uri; //wrong

	std::vector<Location> locations = server.getLocations();
	const Location *location = matchLocation(locations, uri);

	if(!location->getMethods().count("GET"))
		return (handle_response_error(response, server.getPage(405), 405));
	else
	{	
		file_path = getPath(server, location, uri);
		response.setPath(file_path);
		if (file_path.empty())
			return (handle_response_error(response, server.getPage(404), 404));
	}

    // if (file_path[0] == '/')
    // {    file_path = "." + file_path;}

	std::cout << CYAN << "Path : " << file_path << RESET << std::endl;

	std::ifstream src(file_path.c_str(), std::ios::binary);
	if (!src)
    {
		// If the file cannot be opened, set the response status to 404
		return (handle_response_error(response, server.getPage(404), 404));
	}

	//NO ISSUE/ERROR
	file_content = read_file(src);
	src.close();
	// response.addHeader("Content-Length", to_string(file_content.length()));
	response.setBody(file_content);
	response.setStatusCode(200);
	response.setStatusMessage("OK");
}

Response &handle_get_request(Server &server, Request &request)
{
	//**CHECKING WITH THE LOCATION BLOCKS**/
		// 1. See if match with any location block
		// ---- check all directives
		// - CGI
		// - Redirect
		// - Else: index, 
		// - Else: autoindex, methods, root, alias
		// - Else routing/redirection
		// 2. Else, return error 404

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
		process_get_request(server, *response, request);
		set_headers(*response, request);
		return *response;
	}
}