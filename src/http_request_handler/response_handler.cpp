#include "../../include/webserv.hpp"

// Find matching server, return the first server by default
Server &matchServer(std::vector<Server> &servers, Request &request)
{
	request.setUri(urlDecode(request.getUri()));
	if (servers.size() > 1)
	{
		for (size_t i = 0; i < servers.size(); i++)
		{
			if (servers[i].getServerName() == request.getHeader("Host"))
				return servers[i];
		}
	}
	return servers[0];
}

/**
 * @brief Generate a corresponding response for the given client request
 * @param servers Vector of servers from the config file
 * @param request The client request
 */
Response &generate_response(std::vector<Server> &servers, Request &request)
{
	Server &server = matchServer(servers, request);
	if (request.isBad())
	{
		Response *response = new Response();
		handle_response_error(*response, server.getPage(400), 400);
		return *response;
	}
	else if (request.getMethod() == "GET")
		return handle_get_request(server, request);
	else if (request.getMethod() == "POST")
		return handle_post_request(server, request);
	else if (request.getMethod() == "DELETE")
		return handle_delete_request(server, request);
	else
	{
		// If the method is not supported, return a 501 Not Implemented response
		Response *response = new Response();
		handle_response_error(*response, server.getPage(501), 501);
		return *response;
	}
}