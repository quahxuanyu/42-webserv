#include <iostream>
#include <vector>
#include <poll.h>
#include <netdb.h> // addrinfo 
#include <cstring>
#include <unistd.h>
#include <unordered_map>

#define BLACK     "\033[0;30m"
#define RED       "\033[0;31m"
#define GREEN     "\033[0;32m"
#define YELLOW    "\033[0;33m"
#define BLUE      "\033[0;34m"
#define MAGENTA   "\033[0;35m"
#define CYAN      "\033[0;36m"
#define WHITE     "\033[0;37m"
#define RESET		"\033[0m"

int get_listener_socket()
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
	{
		std::cerr << "pollserver: " << gai_strerror(status) << std::endl;
		exit(1);
	}

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
		return (-1);

	//done with addrinfo
	freeaddrinfo(ai);

	//listen
	if (listen(listener, 10) == -1)
		return -1;

	return listener;
}

void add_to_pfds(std::vector<pollfd> *pfds, int new_fd, int *fd_count, int *fd_size)
{
	if (*fd_count == *fd_size)
	{	
		std::cout << MAGENTA << "Expanding capacity" << RESET << std::endl;
		*fd_size *= 2; //double the size
	}
	(*pfds)[*fd_count].fd = new_fd;
	(*pfds)[*fd_count].events = POLLIN; //notified when socket has incoming data
	(*pfds)[*fd_count].revents = 0;

	(*fd_count)++;
	std::cout << "number of socket: " << *fd_count << std::endl;
}

void handle_new_connection(int listener, int *fd_count, int *fd_size, std::vector<pollfd> *pfds)
{
	struct sockaddr_storage client_addr;
	socklen_t addr_len;
	int new_fd;

	addr_len = sizeof(client_addr);
	new_fd = accept(listener, (struct sockaddr *)&client_addr, &addr_len);

	if (new_fd == -1)
		std::cerr << "Error :Accept" << std::endl;
	else
	{
		add_to_pfds(pfds, new_fd, fd_count, fd_size);
		std::cout << GREEN << "Server: new connection from fd " << new_fd << GREEN << std::endl;
	}
}

void del_from_pfds(std::vector<pollfd> *pfds, int i, int *fd_count)
{
	pfds[i] = pfds[*fd_count - 1];
	(*fd_count)--;
}

void handle_client_read(int listener, std::unordered_map<int, std::string> *requests, std::unordered_map<int, std::string> *responses, int *fd_count, std::vector<pollfd> *pfds, int *pfd_i)
{
	std::cout << YELLOW << "Server reading from fd " << (*pfds)[*pfd_i].fd << RESET << std::endl;
	char buf[256];
	int nbytes = recv((*pfds)[*pfd_i].fd, buf, sizeof(buf), 0);
	int sender_fd = (*pfds)[*pfd_i].fd;

	if (nbytes <= 0)
	{
		if (nbytes == 0)
			std::cout << "pollserver: socket " << sender_fd << " hungup" << std::endl;
		else
			std::cerr << "Error : recv" << std::endl;
		close(sender_fd);
		del_from_pfds(pfds, *pfd_i, fd_count);
		(*pfd_i)--;
	}
	else
	{
		(*requests)[sender_fd] += std::string(buf);
		if ((*requests)[sender_fd].find("\r\n\r\n") != std::string::npos)
		{
			std::cout << "Server : recv from fd " << sender_fd << ": \n - REQUEST -\n" << BLUE << (*requests)[sender_fd] << RESET << std::endl;
			(*requests).erase(sender_fd);
			//generate response
			std::string body = "Hello !!!!";
			std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Length: " + std::to_string(body.length()) + "\r\n\r\n" + body;

			//store response to map
			(*responses)[sender_fd] = response;

			//allow send()
			(*pfds)[*pfd_i].events |= POLLOUT;
		}
	}
}

void handle_client_write(int listener, std::unordered_map<int, std::string> *requests, std::unordered_map<int, std::string> *responses, int *fd_count, std::vector<pollfd> *pfds, int *pfd_i)
{
	std::cout << YELLOW << "Server writing to fd " << (*pfds)[*pfd_i].fd << RESET << std::endl;

	int sender_fd = (*pfds)[*pfd_i].fd;
	std::string &response = (*responses)[sender_fd];

	int bytes_sent = send(sender_fd, response.c_str(), response.length(), 0);
	if (bytes_sent > 0)
		response.erase(0, bytes_sent);
	
	std::cout << "successfully send" << std::endl;
	if (response.empty())
		(*pfds)[*pfd_i].events &= ~POLLOUT;
}


void close_connection(int *fd_count, std::vector<pollfd> *pfds, int *pfd_i)
{
	std::cout << RED << "Closing connection to fd " << (*pfds)[*pfd_i].fd << RESET << std::endl;
	close((*pfds)[*pfd_i].fd);
	del_from_pfds(pfds, *pfd_i, fd_count);
}

//process existing connections
void process_connections(int listener, std::unordered_map<int, std::string> *requests, std::unordered_map<int, std::string> *responses, int *fd_count, int *fd_size, std::vector<pollfd> *pfds)
{
	for (int i = 0; i < *fd_count; i++)
	{
		if ((*pfds)[i].fd == listener && (*pfds)[i].revents & POLLIN)
			handle_new_connection(listener, fd_count, fd_size, pfds);

		else if ((*pfds)[i].revents & POLLHUP)
			close_connection(fd_count, pfds, &i);
		else if ((*pfds)[i].revents & POLLIN)
			handle_client_read(listener, requests, responses, fd_count, pfds, &i);
		else if ((*pfds)[i].revents & POLLOUT)
			handle_client_write(listener, requests, responses, fd_count, pfds, &i);

	}
}

int main()
{
	int listener; //listening socket fd

	int sockfd_size = 5;
	int sockfd_count = 0;

	std::vector<pollfd> pfds(sockfd_size);
	listener = get_listener_socket();
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
		{
			std::cerr << "Error : poll" << std::endl;
			return (1);
		}
		process_connections(listener, &requests, &responses, &sockfd_count, &sockfd_size, &pfds);
	}

	return 0;
}






/* int main()
{
	std::vector<pollfd> pollfds(1);

	pollfds[0].fd = 0; //stdin
	pollfds[0].events  = POLLIN;

	std::cout << "it RETURN or wait 2.5 seconds for timeout" << std::endl;

	int num_events = poll(pollfds.data(), pollfds.size(), 3000);

	if (num_events == 0)
		std::cout << "POLL timeout" << std::endl;
	else
	{
		int pollin_happened = pollfds[0].revents & POLLIN;
		if (pollin_happened)
			std::cout << "fd" << pollfds[0].fd << "is ready to send" << std::endl;
		else
			std::cout << "Unexpected event occured" << pollfds[0].revents << std::endl;
	}
	return (0);
} */