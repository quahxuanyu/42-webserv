#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string.h>
#include <fstream>
#include <iostream>
#include <ctime>
#include <sstream>
#include "Request.hpp"
#include "Response.hpp"

Response &handle_get_response(Request &request); // Handles GET requests and returns a Response object
std::string to_string(int value); // Converts an integer to a string

#endif