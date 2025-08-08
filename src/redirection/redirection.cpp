#include "../../include/webserv.hpp"

void redirection(Response &response, Request &request, Location location) {
    response.setVersion(request.getVersion());
    response.setStatusCode(location.getRedirectCode());
    response.setStatusMessage("Moved Permanently");
    response.addHeader("Connection", "keep-alive");
    response.addHeader("Content-Length", to_string(response.getBody().length()));
    response.addHeader("Content-Type", "text/html"); //**Might not be 100% correct */
    response.addHeader("Date", get_current_time());
    response.addHeader("Server", "Webserv/1.0"); // ** TEMPORARY, wait until config file is implemented **
    response.addHeader("Location", location.getRedirectUrl());
}