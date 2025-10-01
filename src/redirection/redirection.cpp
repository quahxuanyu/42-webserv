#include "../../include/webserv.hpp"

void redirection(Response &response, Request &request, Location location) {
    response.setVersion(request.getVersion());
    response.setStatusCode(location.getRedirectCode());
    // response.setStatusMessage("Moved Permanently");
    switch (location.getRedirectCode()) {
        case 301:
            response.setStatusMessage("Moved Permanently");
            break;
        case 302:
            response.setStatusMessage("Found");
            break;
        case 303:
            response.setStatusMessage("See Other");
            break;
        case 307:
            response.setStatusMessage("Temporary Redirect");
            break;
        case 308:
            response.setStatusMessage("Permanent Redirect");
            break;
        default:
            response.setStatusMessage("Redirect");
    }
    response.addHeader("Connection", "keep-alive");
    response.addHeader("Content-Length", to_string(response.getBody().length()));
    response.addHeader("Content-Type", "text/html");
    response.addHeader("Date", get_current_time());
    response.addHeader("Server", "Webserv/1.0");
    response.addHeader("Location", location.getRedirectUrl());
}