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

/**
 * @brief Reads the content of a file and sets it in the response body.
 */
void process_request(Response &response, Request &request) {
    std::string uri = request.getUri();
    std::string file_path;
    std::string file_content;

    // ** TEMPORARY URI file path handling (need to wait for config file implementation) **
    if (uri == "/")
        file_path = "html/upload.html";
    else
        file_path = "." + uri; //wrong

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

Response &handle_get_request(Request &request) {

    //**CHECKING WITH THE LOCATION BLOCKS AND CORRECTS THE FILE_PATH**/
        // 1. See if match with any location block
        // ---- check all directives
        // - CGI
        // - Redirect
        // - Else: index, 
        // - Else: autoindex, methods, root, alias
        // - Else routing/redirection
        // 2. Else, return error 404

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
    else if (request.getUri().find("/storage") != std::string::npos)  // Check if the request is for the storage directory
    {
        std::cout << BLUE << "Got Into Storage For AutoIndex" << RESET << std::endl;
        Response *response = new Response();
        autoindex(*response, request, "/home/quahxuanyu/42KL/42-webserv/cgi-bin/storage"); // **TEMPORARY access storage folder
		return *response;
    }
    else if (request.getUri().find("/redirect") != std::string::npos)
    {
        std::cout << BLUE << "Got Into Redirect" << RESET << std::endl;
        Location location;
        location.setRedirectCode(301);
        location.setRedirectUrl("/storage"); // **TEMPORARY redirect URL
        Response *response = new Response();
        redirection(*response, request, location);
        return *response;
    }
	else
	{
		// Handle non-CGI GET requests (read and return contents of the file)
		Response *response = new Response();
		response->setVersion(request.getVersion());
		process_request(*response, request);
		set_headers(*response, request);
		return *response;
	}
}