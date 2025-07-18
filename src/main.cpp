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

// int main(int argc, char *argv[], char *env[])
// {
// 	(void) argc;
// 	(void) argv;
// 	(void) env;
// 	std::cout << "----------Request-----------" << std::endl;
// 	Request request("GET", "/", "HTTP/1.1");
// 	request.addHeader("Host", "localhost");
// 	request.addHeader("User-Agent", "WebClient/1.0");
// 	print_request(request);

// 	std::cout << "\n----------Response-----------" << std::endl;
// 	Response &response = handle_get_response(request);
// 	std::cout << response.toString();
// 	delete &response;

// 	// std::cout << "----------CGI TEST-----------" << std::endl;
// 	// execute("./cgi-bin/test.cgi", env);
// }

void parseConfigFile(char *file)
{
	std::string line;
	std::ifstream ConfigFile(file);

	while(getline(ConfigFile, line))
	{
		if (line.find("server") != std::string::npos)
		{
			
		}

	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
		return (std::cout << "Incorrect number of arguments" << std::endl, 2);
	parseConfigFile(argv[1]);
    try 
	{
        Server server("0.0.0.0", "8080");
        server.multiplexing();  // Run forever
    } 
	catch(const std::exception &e)
	{
		std::cerr << RED << "Server initialization failed: " << e.what() << RESET << std::endl;
		return 1;
	}
}


