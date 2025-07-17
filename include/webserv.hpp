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

Response &handle_get_response(Request &request); // Handles GET requests and returns a Response object

//HELPER.CPP
std::string to_string(int value); // Converts an integer to a string
std::string get_current_time();
std::string find_mime(std::string uri);

//EXECUTE.CPP
void execute(std::string uri, char **env);

#endif