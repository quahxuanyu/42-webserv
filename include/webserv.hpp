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
#include <dirent.h>
#include <algorithm>
#include <signal.h>
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

int g_signal;

//HTTP_REQUEST_HANDLER
Response &handle_get_request(Server &server, Request &request); // Handles GET requests and returns a Response object
Response &handle_post_request(Server &server, Request &request); // Handles POST requests and returns a Response object
Response &handle_delete_request(Server &server, Request &request); // Handles DELETE requests and returns a Response object
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
extern std::map<std::string, Session> sessions;
std::map<int, std::string> createErrorMap();

std::vector<Server> parseConfigFile(char *file);

// HELPER.CPP
std::string to_string(int value); // Converts an integer to a string
std::string get_current_time();
std::string find_mime(std::string uri);
void set_headers(Response &response);
/**
 * @brief Modify the response to direct to an error page
 * @param response The response object to set the error
 * @param path The path to the error page
 * @param error_code The HTTP error code to set in the response
 */
void handle_response_error(Response &response, std::string path, int error_code);
/**
 * @brief Decodes a URL-encoded string.
 * Changes %XX to the corresponding character and + to space.
 */
std::string urlDecode(const std::string &src);

//EXECUTE.CPP
std::string execute(Request &request, char **env);
std::string cgi(Request &request);

// CGI_HELPER.CPP
std::string get_content_type(const std::string &cgi_output); // Extracts the Content
std::string get_body(const std::string &cgi_output); // Extracts the body from
Response &parse_cgi_response(Response &response, std::string cgi_response);
/**
 * @brief Trim trailing slash on the URI
 * @param string The string to trim
 */
std::string trimTrailingSlash(const std::string &string);

// AUTOINDEX.CPP
void autoindex(Response &response, Request &request, std::string dir_path);

// REDIRECTION.CPP
void redirection(Response &response, Request &request, Location location);
Response &parse_noncgi_response();

//READ.CPP
/**
 * @brief Reads the content of a file and sets it in the response body.
 * @param response The response object to set the file content
 * @param request The request object to get the URI
 * @param server The server object to get the file path
 * @param file_path The path to the file to read
 */
void normal_file_response(Response &response, const Server &server, std::string file_path);

bool isFile(const std::string &path);
bool isDirectory(const std::string &path);
bool isFileNoCwd(const std::string &path);
const Location* matchLocation(const std::vector<Location>& locations, const std::string& uri);
void handle_response_error(Response &response, std::string path, int error_code);
std::string replaceAll(std::string str, const std::string &src, const std::string &target);
std::string extractSessionID(std::string cookie);
std::string  generateSessionID();
Session createSession(Request &request);
void updateSession(Session *session, Request &request);
void printSessionData(Session &Session);
void printAllSessionData();
std::string trim (std::string str);
std::string defaultErrorPage(int error_code);

//READ.CPP
/**
 * @brief Reads the content of a file and returns it as a string.
 * @param src The input file stream to read from.
 */
std::string read_file(std::ifstream &src);
#endif