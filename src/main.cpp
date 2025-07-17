#include "../include/Response.hpp"
#include "../include/Request.hpp"

void print_response(Response &response) {
	std::cout << response.getVersion() << " "
			  << response.getStatusCode() << " "
			  << response.getStatusMessage() << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = response.getHeaders().begin(); it != response.getHeaders().end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	std::cout << response.getBody() << std::endl;
}

int main(void)
{

	Request request("GET", "/", "HTTP/1.1");
	request.addHeader("Host", "localhost");
	request.addHeader("User-Agent", "WebClient/1.0");


	Response response("HTTP/1.1", 200, "OK");
	response.addHeader("Content-Type", "text/html");
	response.setBody("<html><body>Hello, World!</body></html>");
	print_response(response);
}
