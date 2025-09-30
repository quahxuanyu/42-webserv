#include "../../include/Client.hpp"
#include "../../include/webserv.hpp"

std::map<std::string, Session> sessions;

Client::Client() {}

Client::Client(int fd) : _fd(fd), _start_time(0) {}

//check for existing session or create a new session
//increment to visit count
void Client::getSession()
{
	std::string cookie = request.getHeader("Cookie");
	std::string session_id = extractSessionID(cookie);
	//printAllSessionData();

	//both assign session to client
	if (!session_id.empty() && sessions.count(session_id))
	{
		//store session data in client
		_session_id = session_id;
		_session = &sessions[session_id];
		updateSession(_session, request);
		//printSessionData(*_session);
	}
	else
	{	//generate new session, add to map
		session_id = generateSessionID();
		_session_id = session_id;
		sessions[session_id] = createSession(request);
		_session = &sessions[session_id];
	}
}

void Client::addSessionData()
{
	//add cookie to response
	_response->addHeader("Set-Cookie", "session_id=" + _session_id);

	//update visit count
	std::string body = _response->getBody();
	body = replaceAll(body, "{{VISIT_COUNT}}", to_string(_session->_visit_count));
	body = replaceAll(body, "{{USERNAME}}", _session->_data["username"]);

	_response->setBody(body);

	//update content length
	_response->addHeader("Content-Length", to_string(_response->getBody().length()));

}

void Client::processRequest(std::vector<pollfd> *pfds, int pfd_i)
{
	//request.printRequest();
	getSession();
	_response = &generate_response(socket_to_servers[socket_fd], request);
	addSessionData();

	send_buf = _response->toString();
	// delete _response;

	std::cout << "== RESPONSE: ==\n" << send_buf << std::endl;
	recv_buf.clear();
	(*pfds)[pfd_i].events |= POLLOUT;
	_start_time = 0;
}

Response *handle_408_error()
{
	Response *response = new Response();
	std::string path = "/error_page/408.html";
	response->setPath(path);
	if (path[0] == '/')
        path = "." + path;
	std::ifstream src(path.c_str(), std::ios::binary);
	std::string body = read_file(src);
	response->setBody(body);
	src.close();
	response->setVersion("HTTP/1.1");
	response->setStatusCode(408);
	response->setStatusMessage(httpErrorMessages[408]);
	set_headers(*response);
	response->addHeader("Connection", "closed");
	response->close_connection = true;

	return response;
}

bool Client::recv_data(std::vector<pollfd> *pfds, int pfd_i)
{
	int sender_fd = (*pfds)[pfd_i].fd;
	std::cout << YELLOW << "Server reading from fd " << sender_fd<< RESET << std::endl;
	char buf[256];
	int nbytes = recv(sender_fd, buf, sizeof(buf), 0);
	
	if (nbytes <= 0)
	{
		if (nbytes == 0)
			std::cout << "pollserver: socket " << sender_fd << " hungup" << std::endl;
		else
			std::cerr << "Error : recv" << std::endl; //throw 
		return false;
	}
	else
	{
		//if its the start of a request
		if (!_start_time)
			_start_time = time(0);
		else
		{
			if (time(NULL) - _start_time >= 10)
			{
				(*pfds)[pfd_i].events &= ~ POLLIN;

				std::cout << RED << "Request takes too long" << RESET << std::endl;
				_response = handle_408_error();

				send_buf = _response->toString();
				// delete _response;
				std::cout << "== RESPONSE: ==\n" << send_buf << std::endl;
				recv_buf.clear();
				(*pfds)[pfd_i].events |= POLLOUT;
				//sleep(10);
				return true;
			}
		}

		recv_buf.append(buf, nbytes);

		if (recv_buf.find("\r\n\r\n") != std::string::npos) // header parsed
		{
			// std::cout << BLUE << "recv_data:" << recv_buf << RESET << std::endl; 
			parse_request();
			if (request.getMethod() == "POST")
			{
				size_t body_start = recv_buf.find("\r\n\r\n");
				std::string body = recv_buf.substr(body_start + 4); //exclude headers
				size_t content_length = atoi(request.getHeader("Content-Length").c_str());

				//finish reading post request
				if (body.length() >= content_length)
				{
					std::cout << GREEN << "Finished Receiving Data!" << RESET << std::endl;
					processRequest(pfds, pfd_i);
				}
			}
			//finish reading get request
			else
				processRequest(pfds, pfd_i);
		}
		return true;
	}
}

bool Client::send_data(std::vector<pollfd> *pfds, int pfd_i)
{
	std::cout << YELLOW << "Server writing to fd " << (*pfds)[pfd_i].fd << RESET << std::endl;

	int bytes_sent = send(_fd, send_buf.c_str(), send_buf.length(), 0);
	if (bytes_sent < 0)
	{
		std::cerr << "send() failed for fd " << _fd << std::endl;
		return false;
	}
	send_buf.erase(0, bytes_sent);

	if (send_buf.empty())
	{	
		//std::cout << "Successfully sent data to fd " << _fd << std::endl;
		(*pfds)[pfd_i].events &= ~POLLOUT;
	}

	//if connection set to closed
	if ((_response->getHeader("Connection")) == "closed")
	{	
		delete _response;
		return false;
	}
	delete _response;
	return true;
}

void Client::parse_request()
{
	/* PARSE REQUEST LINE */
	size_t line_end = recv_buf.find("\r\n"); 

	if (line_end == std::string::npos)
		return request.setisBad(true);

	std::string line = recv_buf.substr(0, line_end);
	std::stringstream stream(line);
	std::string method, uri, version;
	if (!(stream >> method >> uri >> version)) //skip space and asign to string
		return request.setisBad(true);

	request.setMethod(method);
	request.setUri(trimTrailingSlash(uri));
	request.setVersion(version);

	size_t line_start = line_end + 2;

	//store headers, split by '\r\n', then ':'
	while (1)
	{
		line_end = recv_buf.find("\r\n", line_start);

		if (line_end == std::string::npos || line_end == line_start)
			break;

		std::string header_line = recv_buf.substr(line_start, line_end - line_start);
		size_t split_pos = header_line.find(':');

		std::string key = trim(header_line.substr(0, split_pos));
		std::string value = trim(header_line.substr(split_pos + 1));
		request.addHeader(key, value);

		line_start = line_end + 2;
	}

	/* PARSE BODY */
	size_t body_start = recv_buf.find("\r\n\r\n");
	size_t content_length = atoi(request.getHeader("Content-Length").c_str());
	std::string body = recv_buf.substr(body_start + 4, content_length);
	request.setBody(body);

	request.printRequest();
}
