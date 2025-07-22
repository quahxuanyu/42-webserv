#include "../include/webserv.hpp"

void print_request(Request &request) {
	std::cout << request.getMethod() << " "
			  << request.getUri() << " "
			  << request.getVersion() << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
}

int main(int argc, char *argv[], char **env)
{
	(void) argc;
	(void) argv;
	// (void) env;
	std::cout << "----------GET Request-----------" << std::endl;
	Request GET_request("GET", "/", "HTTP/1.1");
	GET_request.addHeader("Host", "localhost");
	GET_request.addHeader("User-Agent", "WebClient/1.0");
	print_request(GET_request);

	std::cout << "\n----------GET Response-----------" << std::endl;
	Response &GET_response = generate_response(GET_request, env);
	std::cout << GET_response.toString();
	delete &GET_response;

	std::cout << "----------POST Request-----------" << std::endl;
	Request POST_request("POST", "cgi-bin/test.cgi", "HTTP/1.1");
	POST_request.addHeader("Host", "localhost");
	POST_request.addHeader("User-Agent", "WebClient/1.0");
	print_request(POST_request);

	std::cout << "----------POST Response-----------" << std::endl;
	Response &POST_response = generate_response(POST_request, env);
	std::cout << POST_response.toString();
	delete &POST_response;
}
