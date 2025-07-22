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

int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

	std::cout << "----------GET Request-----------" << std::endl;
	Request GET_request("GET", "/", "HTTP/1.1");
	GET_request.addHeader("Host", "localhost");
	GET_request.addHeader("User-Agent", "WebClient/1.0");
	print_request(GET_request);

	std::cout << "\n----------GET Response-----------" << std::endl;
	Response &GET_response = generate_response(GET_request);
	std::cout << GET_response.toString();
	delete &GET_response;

	std::cout << "\n----------POST Request CGI: Uplaod HTML File-----------" << std::endl;
	Request POST_request("POST", "cgi-bin/test.cgi", "HTTP/1.1");
	POST_request.addHeader("Host", "localhost");
	POST_request.addHeader("User-Agent", "WebClient/1.0");
	POST_request.addHeader("Content-Type", "multipart/form-data; boundary=---------------------------2858016734199026723269487220");
	POST_request.setBody("-----------------------------2858016734199026723269487220\r\n"
						"Content-Disposition: form-data; name=\"file\"; filename=\"test3.html\"\r\n"
						"Content-Type: text/html\r\n\r\n"
						"<html><body>TESTING TESTIING!!</body></html>\r\n"
						"-----------------------------2858016734199026723269487220--\r\n");
	POST_request.addHeader("Content-Length", to_string(POST_request.getBody().length()));
	print_request(POST_request);

	std::cout << "----------POST Response CGI-----------" << std::endl;
	Response &POST_response = generate_response(POST_request);
	std::cout << POST_response.toString();
	delete &POST_response;

	std::cout << "\n----------GET Request CGI: Get JSON Files-----------" << std::endl;
	Request GET_request_cgi("GET", "cgi-bin/js_test.cgi", "HTTP/1.1");
	GET_request_cgi.addHeader("Host", "localhost");
	GET_request_cgi.addHeader("User-Agent", "WebClient/1.0");
	print_request(GET_request_cgi);

	std::cout << "----------GET Response CGI-----------" << std::endl;
	Response &GET_response_cgi = generate_response(GET_request_cgi);
	std::cout << GET_response_cgi.toString();
	delete &GET_response_cgi;
	
}
