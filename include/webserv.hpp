#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string.h>
#include <fstream>
#include <iostream>
#include <ctime>
#include <sstream>
#include <iostream>
#include <vector>
#include <poll.h>
#include <netdb.h> // addrinfo 
#include <cstring>
#include <unistd.h>
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Location.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BLACK     "\033[0;30m"
#define RED       "\033[0;31m"
#define GREEN     "\033[0;32m"
#define YELLOW    "\033[0;33m"
#define BLUE      "\033[0;34m"
#define MAGENTA   "\033[0;35m"
#define CYAN      "\033[0;36m"
#define WHITE     "\033[0;37m"
#define RESET		"\033[0m"

Response &handle_get_response(Request &request); // Handles GET requests and returns a Response object

//HELPER.CPP
std::string to_string(int value); // Converts an integer to a string
std::string get_current_time();
std::string find_mime(std::string uri);

//EXECUTE.CPP
void execute(std::string uri, char **env);

#endif