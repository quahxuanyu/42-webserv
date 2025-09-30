#include "../../include/webserv.hpp"

void handle_cgi(Request &request, Response &response, const Server &server, const Location *location)
{
    processLocationRequest(server, request, location);

    // Execute the CGI script and get the response
    std::string cgi_response = cgi(request);

    // Check for errors in the CGI response
    if (cgi_response.find("Error") != std::string::npos)
        handle_response_error(response, server.getPage(500), 500);
    else {
        // infinite loop in CGI
        if (cgi_response.empty())
        {
            std::cout << RED << " * Error - CGI response is empty" << RESET << std::endl;
            handle_response_error(response, server.getPage(504), 504);
        }
        //execve failed: incorrect path
        else if (cgi_response == "500")
        {
            std::cout << RED << " * Error - CGI execve failed" << RESET << std::endl;
            handle_response_error(response, server.getPage(500), 500);
        }
        else
        {
            std::cout << GREEN << " * CGI executed successfully" << RESET << std::endl;
            parse_cgi_response(response, cgi_response); // Parse the CGI response and return it
        }
    }
}