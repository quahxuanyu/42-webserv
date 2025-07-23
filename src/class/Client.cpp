#include "../../include/Client.hpp"
#include "../../include/webserv.hpp"

Client::Client() {}

Client::Client(int fd) : _fd(fd) {}

void Client::parse_request()
{
	size_t line_end = recv_buf.find("\r\n");
	std::string line = recv_buf.substr(0, line_end);

	size_t first_space = line.find(' ');
	std::string method = line.substr(0, first_space);

	size_t second_space = line.find(' ', first_space + 1);
	std::string uri = line.substr(first_space + 1, second_space - first_space - 1);

	std::string version = line.substr(second_space + 1);

	request.setMethod(method);
	request.setUri(uri);
	request.setVersion(version);

	size_t line_start = line_end + 2;
	while (1)
	{
		line_end = recv_buf.find("\r\n", line_start);

		if (line_end == std::string::npos || line_end == line_start)
			break;

		std::string header_line = recv_buf.substr(line_start, line_end - line_start);
		size_t split_pos = header_line.find(':');

		std::string key = header_line.substr(0, split_pos);
		std::string value = header_line.substr(split_pos + 1);
		request.addHeader(key, value);

		line_start = line_end + 2;
	}
	request.printRequest();
}

bool Client::recv_data(std::vector<pollfd> *pfds, int pfd_i)
{
	std::cout << YELLOW << "Server reading from fd " << (*pfds)[pfd_i].fd << RESET << std::endl;
	char buf[256];
	int nbytes = recv((*pfds)[pfd_i].fd, buf, sizeof(buf), 0);
	int sender_fd = (*pfds)[pfd_i].fd;

	if (nbytes <= 0)
	{
		if (nbytes == 0)
			std::cout << "pollserver: socket " << sender_fd << " hungup" << std::endl;
		else
			std::cerr << "Error : recv" << std::endl;
		return false;
	}
	else
	{
		recv_buf += std::string(buf);
		std::cout << "[DEBUG] Current recv_buf content:\n" << recv_buf << std::endl;
		if (recv_buf.find("\r\n\r\n") != std::string::npos)
		{
			std::cout << "Server : recv from fd " << sender_fd << ": \n - REQUEST -\n" << BLUE << recv_buf << RESET << std::endl;
		
			//generate response
			parse_request();
			response = generate_response(request);
			send_buf = response.toString();
			recv_buf.clear();

			//allow send()
			(*pfds)[pfd_i].events |= POLLOUT;
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
		std::cout << "Successfully sent data to fd " << _fd << std::endl;
		(*pfds)[pfd_i].events &= ~POLLOUT;
	}
	return true;
}