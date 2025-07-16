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

int main(void)
{
	std::cout << "----------Request-----------" << std::endl;
	Request request("GET", "/", "HTTP/1.1");
	request.addHeader("Host", "localhost");
	request.addHeader("User-Agent", "WebClient/1.0");
	print_request(request);

	std::cout << "\n----------Response-----------" << std::endl;
	Response &response = handle_get_response(request);
	std::cout << response.toString();
}
