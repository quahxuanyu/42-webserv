#include "../../include/webserv.hpp"

// Converts an integer to a string
std::string to_string(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
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