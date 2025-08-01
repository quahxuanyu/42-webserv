#include "../../include/webserv.hpp"

Response &generate_response(std::vector<Server> &servers, Request &request)
{
	//Find matching server
	Server server = servers[0];
	std::cout << "request host: " << request.getHeader("Host") << std::endl;
	if (servers.size() > 1)
	{
		for (size_t i = 0; i < servers.size(); i++)
		{
			std::cout << "server_name :" << servers[i].getServerName() << std::endl;
			if (servers[i].getServerName() == request.getHeader("Host"))
				server = servers[i];
		}
	}
	if (request.getMethod() == "GET") {
		return handle_get_request(server, request);
	}
	else if (request.getMethod() == "POST") {
		return handle_post_request(server, request);
	}
	else
	{
		// If the method is not suppsten 127.0.0.1:4243;orted, return a 501 Not Implemented response
		Response *response = new Response("HTTP/1.1", 501, "Not Implemented");
		response->addHeader("Content-Type", "text/plain");
		response->setBody("Method Not Implemented");
		return *response;
	}
}