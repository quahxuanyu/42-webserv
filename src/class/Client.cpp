#include "../../include/Client.hpp"
#include "../../include/webserv.hpp"
#include <sstream>

Client::Client() {}

Client::Client(int fd) : _fd(fd) {}

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
		if (recv_buf.find("\r\n\r\n") != std::string::npos)
		{
			std::cout << "Server : recv from fd " << sender_fd << ": \n - REQUEST -\n" << BLUE << recv_buf << RESET << std::endl;
			recv_buf.clear();

			//generate response
			send_buf = 
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Length: 10\r\n\r\n Hello !!!";

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