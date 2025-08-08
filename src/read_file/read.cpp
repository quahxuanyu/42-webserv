#include "../../include/webserv.hpp"

void normal_file_response(Response &response, Request &request, const Server &server, std::string file_path) {
	std::string file_content;
	//READING FILE CONTENT
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
	set_headers(response, request);
}