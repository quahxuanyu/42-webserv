#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string.h>
#include <fstream>
#include <iostream>
#include <ctime>
#include <sstream>
#include "Request.hpp"
#include "Response.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//HTTP_REQUEST_HANDLER
Response &handle_get_request(Request &request); // Handles GET requests and returns a Response object
Response &handle_post_request(Request &request, char **env); // Handles POST requests and returns a Response object
Response &generate_response(Request &request, char **env);	 // Processes the request and returns a Response object

// HELPER.CPP
std::string to_string(int value); // Converts an integer to a string
std::string get_current_time();
std::string find_mime(std::string uri);
void set_headers(Response &response, Request &request);

//EXECUTE.CPP
void execute(std::string uri, char **env);

#endif