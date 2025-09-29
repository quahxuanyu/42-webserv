#include "../../include/webserv.hpp"

Response &generate_response(std::vector<Server> &servers, Request &request)
{
	//Find matching server - use the first server by default
	Server server = servers[0];
	request.setUri(urlDecode(request.getUri()));
	if (servers.size() > 1)
	{
		for (size_t i = 0; i < servers.size(); i++)
		{
			if (servers[i].getServerName() == request.getHeader("Host"))
				server = servers[i];
		}
	}
	if (request.isBad())
	{
		Response *response = new Response();
		handle_response_error(*response, server.getPage(400), 400);
		return *response;
	}
	if (request.getMethod() == "GET") {
		return handle_get_request(server, request);
	}
	else if (request.getMethod() == "POST") {
		return handle_post_request(server, request);
	}
	else if (request.getMethod() == "DELETE")
	{
		return handle_delete_request(server, request);
	}
	else
	{
		// If the method is not suppsten 127.0.0.1:4243, return a 501 Not Implemented response
		Response *response = new Response("HTTP/1.1", 501, "Not Implemented");
		response->addHeader("Content-Type", "text/plain");
		response->setBody("Method Not Implemented");
		return *response;
	}
}