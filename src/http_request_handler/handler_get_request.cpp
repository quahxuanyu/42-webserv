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
        file_path = "html/sample.html";
    else
        file_path = "." + uri;

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
    // **Check if its a CGI request**
    
    // Create a response object
    Response *response = new Response();
    response->setVersion(request.getVersion()); // set the version (always HTTP/1.1)

    // Set the body of the response
    process_request(*response, request);

    // Set the headers of the response
    set_headers(*response, request);
    return *response;
}