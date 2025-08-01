#include "../../include/webserv.hpp"

std::string read_file(std::ifstream &src) {
    std::string content;
    std::string line;

    while (std::getline(src, line)) { // Loop through each line of the file
        content += line;

        if (!src.eof()) { // Only add a newline if it's not the last line
            content += "\n";
        }
    }
    return (content);
}

//find the longest matching location block path
const Location* matchLocation(const std::vector<Location>& locations, const std::string& uri)
{
	const Location* best_match = NULL;
	size_t best_len = 0;

	for (size_t i = 0; i < locations.size(); i++)
	{
		const std::string& path = locations[i].getPath();
		//handle file/dir URI
		if (uri.find(path) == 0)
		{ // starts with path
			if (path.length() > best_len)
			{
				best_match = &locations[i];
				best_len = path.length();
			}
		}
	}
	return best_match; // may be null if no match
}

std::string getPath(Server &server, const Location *location, std::string uri)
{
	std::string file_path;
	
	if (location == NULL)
		return server.getPage(404);

	else if (location->hasRoot() || location->hasAlias() || server.hasRoot())
	{
		if (location->hasRoot())
			file_path = location->getRoot() + uri;
		else if (location->hasAlias())
		{
			//get substring after location path (replace location path)
			std::string suffix = uri.substr(location->getPath().length());
			file_path = location->getAlias() + suffix;
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
			return server.getPage(404);
	}
	//file_path should be a valid path now
	if (!isFileNoCwd(file_path))
		return server.getPage(404);
	
	return file_path;
}

/**
 * @brief Reads the content of a file and sets it in the response body.
 */
void process_request(Server &server, Response &response, Request &request)
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
		file_path = server.getPage(405);
	else
		file_path = getPath(server, location, uri);

	std::cout << CYAN << "Path : " << file_path << RESET << std::endl;

	std::ifstream src(file_path.c_str(), std::ios::binary);
	if (!src) {
		// If the file cannot be opened, set the response status to 404
		src.close();
		std::ifstream src("error_page/404.html", std::ios::binary);
		response.setStatusCode(404);
		response.setStatusMessage("Not Found");
		response.setBody(read_file(src));
		src.close();
		return ;
	}
	file_content = read_file(src);
	src.close();
	// response.addHeader("Content-Length", to_string(file_content.length()));
	response.setBody(file_content);
	response.setStatusCode(200);
	response.setStatusMessage("OK");
}

Response &handle_get_request(std::vector<Server> &servers, Request &request)
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


	//Find matching server
	Server server = servers[0];
	std::cout << "request host: " << request.getHeader("Host") << std::endl;
	if (servers.size() > 1)
	{
		for (size_t i = 0; i < servers.size(); i++)
		{
			std::cout << "server_name :" << servers[i].getServerName() << std::endl;
			if (servers[i].getServerName() == request.getHeader("Host"))
				server = servers[i];
		}
	}

	// CGI DIRECTIVE
	if (request.getUri().find(".cgi") != std::string::npos) {
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
		process_request(server, *response, request);
		set_headers(*response, request);
		return *response;
	}
}