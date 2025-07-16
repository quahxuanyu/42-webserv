#include "../include/webserv.hpp"

/**
 * @brief Finds the MIME type based on the URI.
 */
std::string find_mime(std::string uri) {
    // For simplicity, we will return a hardcoded MIME type based on the file extension
    if (uri.find(".html") != std::string::npos || uri == "/") {
        return "text/html";
    } else if (uri.find(".css") != std::string::npos) {
        return "text/css";
    } else if (uri.find(".js") != std::string::npos) {
        return "application/javascript";
    } else if (uri.find(".png") != std::string::npos) {
        return "image/png";
    } else if (uri.find(".jpg") != std::string::npos || uri.find(".jpeg") != std::string::npos) {
        return "image/jpeg";
    } else {
        return "application/octet-stream"; // Default MIME type
    }
}

/**
 * @brief Reads the content of a file and sets it in the response body.
 */
void read_file(Response &response, Request &request) {
    std::string uri = request.getUri();
    std::string file_path;
    std::string line;
    std::string content;
    int         content_length;

    // ** TEMPORARY URI file path handling (need to wait for config file implementation) **
    if (uri == "/")
        file_path = "html/sample.html";
    else
        file_path = "error";

    std::ifstream src(file_path.c_str(), std::ios::binary);
    if (!src) {
        // If the file cannot be opened, set the response status to 404
        response.setStatusCode(404);
        response.setStatusMessage("Not Found");
        response.setBody("<html><body><h1>404 Not Found</h1></body></html>");
        return ;
    }

    while (std::getline(src, line)) { // Loop through each line of the file
        content_length += line.length(); // +1 for newline character
        content += line;

        if (!src.eof()) { // Only add a newline if it's not the last line
            content_length += 1; // +1 for newline character
            content += "\n";
        }
    }
    src.close();
    response.addHeader("Content-Length", to_string(content_length));
    response.setBody(content);
    response.setStatusCode(200);
    response.setStatusMessage("OK");
}

/**
 * @brief Gets the current time in GMT format.
 */
std::string get_current_time() {
    std::time_t now = std::time(NULL);       // Get current time
    std::tm* gmt = std::gmtime(&now);        // Convert to UTC/GMT

    char buf[100];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", gmt);
    std::string current_time(buf);

    return (current_time);
}

/**
 * @brief Sets the headers for the response based on the request and response status.
 */
void set_headers(Response &response, Request &request) {
    if (response.getStatusCode() == 200)
        response.addHeader("Connection", "keep-alive");
    else
        response.addHeader("Connection", "close");
    response.addHeader("Content-Type", find_mime(request.getUri()));
    response.addHeader("Date", get_current_time());
    response.addHeader("Server", "Webserv/1.0"); // ** TEMPORARY, wait until config file is implemented **
}

Response &handle_get_response(Request &request) {
    // Create a response object
    Response *response = new Response();
    response->setVersion(request.getVersion()); // set the version (always HTTP/1.1)

    // Set the body of the response
    read_file(*response, request);

    // Set the headers of the response
    set_headers(*response, request);
    return *response;
}