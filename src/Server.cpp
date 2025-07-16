#include "../include/Server.hpp"
#include <iostream>
#include <vector>
#include <poll.h>
#include <netdb.h> // addrinfo 
#include <cstring>
#include <unistd.h>
#include <unordered_map>
#include "../include/Client.hpp"


void Server::get_listener_socket()
{
	int listener;
	int yes = 1;
	int status;

	struct addrinfo hints;
	struct addrinfo *ai = nullptr;
	struct addrinfo *p = nullptr;

	memset(&hints, 0 , sizeof(hints));
	hints.ai_family = AF_INET; //IPv4
	hints.ai_socktype = SOCK_STREAM; //TCP
	hints.ai_flags = AI_PASSIVE; // bind with local address

	status = getaddrinfo(NULL, "8080", &hints, &ai);
	if (status)
		throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(status)));
	
	for (p = ai; p != NULL; p = p->ai_next)
	{
		//look for a valid socket
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0)
			continue;

		// allow addr reuse
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(listener);
			continue;
		}
		break; // found socket and bind
	}

	//no suitable socket
	if (p == NULL)
	{
		freeaddrinfo(ai);
		throw std::runtime_error("Could not bind with any address");
	}

	//done with addrinfo
	freeaddrinfo(ai);

	//listen
	if (listen(listener, 10) == -1)
	{
		close(listener);
		throw std::runtime_error("Listen failed");
	}

	_listener_fd = listener;
}


void Server::add_to_pfds(int new_fd, int *fd_count, int *fd_size)
{	
	if (*fd_count == *fd_size)
	{	
		std::cout << MAGENTA << "Expanding capacity" << RESET << std::endl;
		*fd_size *= 2; //double the size
	}
	(*_pfds)[*fd_count].fd = new_fd;
	(*_pfds)[*fd_count].events = POLLIN; //notified when socket has incoming data
	(*_pfds)[*fd_count].revents = 0;

	(*fd_count)++;
	std::cout << "number of socket: " << *fd_count << std::endl;
}


void Server::accept_connection(int *fd_count, int *fd_size)
{
	struct sockaddr_storage client_addr;
	socklen_t addr_len;
	int new_fd;

	addr_len = sizeof(client_addr);
	new_fd = accept(_listener_fd, (struct sockaddr *)&client_addr, &addr_len);

	if (new_fd == -1)
		throw std::runtime_error("Accept failed");
	else
	{
		//create new client and add to the map
		_clients[new_fd] = Client(new_fd);
		add_to_pfds(new_fd, fd_count, fd_size);
		std::cout << GREEN << "Server: new connection from fd " << new_fd << GREEN << std::endl;
	}
}

void Server::close_connection(int *fd_count, int *pfd_i)
{
	std::cout << RED << "Closing connection to fd " << (*_pfds)[*pfd_i].fd << RESET << std::endl;
	_clients.erase((*_pfds)[*pfd_i].fd);
	close((*_pfds)[*pfd_i].fd);
	_pfds[*pfd_i] = _pfds[*fd_count - 1];
	(*fd_count)--;
}

void Server::handle_client_read(int *fd_count, int *pfd_i)
{
	int client_fd = (*_pfds)[*pfd_i].fd;
	if (!_clients[client_fd].recv_data(_pfds, fd_count, pfd_i))
		close_connection(fd_count, pfd_i);
}

void Server::handle_client_write(int *fd_count, int *pfd_i)
{
	int client_fd = (*_pfds)[*pfd_i].fd;
	if (!_clients[client_fd].send_data(_pfds, fd_count, pfd_i))
		close_connection(fd_count, pfd_i);
}

void Server::process_connections(int *fd_count, int *fd_size, std::vector<pollfd> *pfds)
{
	for (int i = 0; i < *fd_count; i++)
	{
		if ((*pfds)[i].fd == _listener_fd && (*pfds)[i].revents & POLLIN)
			accept_connection(fd_count, fd_size);
		else if ((*pfds)[i].revents & POLLHUP)
			close_connection(fd_count, &i);
		else if ((*pfds)[i].revents & POLLIN)
			handle_client_read(fd_count, &i);
		else if ((*pfds)[i].revents & POLLOUT)
			handle_client_write(fd_count, &i);
	}
}

void Server::multiplexing()
{
	int listener; //listening socket fd

	int sockfd_size = 5;
	int sockfd_count = 0;

	std::vector<pollfd> pfds(sockfd_size);
	get_listener_socket();
	if (listener == -1)
	{
		std::cerr << "errror getting listening socket" << std::endl;
		exit(1);
	}
	std::cout << "listener fd (localhost, port 8080): " << listener << std::endl;
	pfds[0].fd = listener;
	pfds[0].events = POLLIN;

	sockfd_count = 1;

	std::unordered_map<int, std::string> responses;
	std::unordered_map<int, std::string> requests;

	std::cout << "Server: waiting for connections.." << std::endl;
	while (1)
	{
		int poll_count = poll(pfds.data(), sockfd_count, -1);
		if (poll_count == -1)
			throw std::runtime_error("poll failed");
		process_connections(&sockfd_count, &sockfd_size, &pfds);
	}
}
