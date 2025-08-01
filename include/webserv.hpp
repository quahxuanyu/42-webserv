#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string.h>
#include <fstream>
#include <iostream>
#include <ctime>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <poll.h>
#include <netdb.h> // addrinfo 
#include <cstring>
#include <unistd.h>
#include <cstdlib>
// #include <thread>

#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Location.hpp"
#include "Exception.hpp"
#include "Connection.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

//HTTP_REQUEST_HANDLER
Response &handle_get_request(Server &server, Request &request); // Handles GET requests and returns a Response object
Response &handle_post_request(Server &server, Request &request); // Handles POST requests and returns a Response object
Response &generate_response(std::vector<Server> &servers, Request &request);	 // Processes the request and returns a Response object
#define BLACK     "\033[0;30m"
#define RED       "\033[0;31m"
#define GREEN     "\033[0;32m"
#define YELLOW    "\033[0;33m"
#define BLUE      "\033[0;34m"
#define MAGENTA   "\033[0;35m"
#define CYAN      "\033[0;36m"
#define RESET		"\033[0m"

extern std::map<int, std::vector<Server> > socket_to_servers;  // listen_fd -> matching Server blocks
extern std::map<int, std::string> httpErrorMessages; 
std::map<int, std::string> createErrorMap();

// HELPER.CPP
std::string to_string(int value); // Converts an integer to a string
std::string get_current_time();
std::string find_mime(std::string uri);
void set_headers(Response &response, Request &request);

//EXECUTE.CPP
std::string execute(Request &request, char **env);
std::string cgi(Request &request);

// CGI_HELPER.CPP
std::string get_content_type(const std::string &cgi_output); // Extracts the Content
std::string get_body(const std::string &cgi_output); // Extracts the body from
Response &parse_cgi_response(std::string cgi_response);



bool isFile(const std::string &path);
bool isDirectory(const std::string &path);
bool isFileNoCwd(const std::string &path);
const Location* matchLocation(const std::vector<Location>& locations, const std::string& uri);
std::string read_file(std::ifstream &src);
void handle_response_error(Response &response, std::string path, int error_code);

#endif