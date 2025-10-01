#include "../include/webserv.hpp"

std::map<int, std::vector<Server> > socket_to_servers;

Connection::Connection(std::vector<Server> servers) : _fd_count(0), _servers(servers) {}

//group servers with the same IP port , ip and port combo is the unique key
void Connection::group_servers()
{
	for (std::vector<Server>::iterator itr = _servers.begin(); itr != _servers.end(); ++itr)
	{
		std::pair<std::string, std::string> addr(itr->getIp(), itr->getPort());
		_addr_server[addr].push_back(*itr);
	}
}

//create new poll fd and add to pfds vector
void Connection::add_to_pfds(int client_fd)
{	
	pollfd new_fd;
	new_fd.fd = client_fd;
	new_fd.events = POLLIN; //notified when socket has incoming data
	new_fd.revents = 0;

	_pfds.push_back(new_fd);
	_fd_count++;
	std::cout << " * Number of socket: " << _fd_count << std::endl;
}

// Accept and register new client
void Connection::accept_connection(int listener_fd)
{
	struct sockaddr_storage client_addr;
	socklen_t addr_len;
	int new_fd;

	addr_len = sizeof(client_addr);
	new_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &addr_len);

	if (new_fd == -1)
		throw std::runtime_error("Accept failed");
	else
	{
		// Create a new Client object associated with this file descriptor
		_clients[new_fd] = Client(new_fd);

		 // Store which listening socket accepted this client
		_clients[new_fd].socket_fd = listener_fd;

		// Add the new client socket to the pollfd list so poll() monitors it
		add_to_pfds(new_fd);
		std::cout << GREEN << " * new connection from fd " << new_fd << GREEN << std::endl;
	}
}

// Clean up and close client connection
void Connection::close_connection(int pfd_i)
{
	int fd = _pfds[pfd_i].fd;
	std::cout << RED << " * Closing connection fd " << fd << RESET << std::endl;
	_clients.erase(fd);	//remove client from map
	close(fd);	//close client fd

	// Remove this fd from the pollfd vector 
	_pfds.erase(_pfds.begin() + pfd_i);
	_fd_count--;
}

void Connection::handle_client_read(int pfd_i)
{
	int client_fd = _pfds[pfd_i].fd;
	if (!_clients[client_fd].recv_data(&_pfds, pfd_i))
		close_connection(pfd_i);
}

void Connection::handle_client_write(int pfd_i)
{
	int client_fd = _pfds[pfd_i].fd;
	if (!_clients[client_fd].send_data(&_pfds, pfd_i))
		close_connection(pfd_i);
}


/**
 * @brief Create and returns a listening socket bound to the given ip:port.
 * @param ip   The IP address to bind (use "0.0.0.0" for all interfaces).
 * @param port The port number as a string (e.g. "8080").
 * @return     File descriptor of the listening socket.
 * @throws     std::runtime_error if any step fails.
 */

// reserves an IP + port on your machine
// allows the OS to accept incoming TCP connections here
// sets up the socket so other devices can connect to it.
int Connection::get_listener_socket(const std::string &ip, const std::string &port)
{
	int listener;
	int yes = 1;
	int status;

	struct addrinfo hints;
	struct addrinfo *ai = NULL;
	struct addrinfo *p = NULL;

	memset(&hints, 0 , sizeof(hints));
	hints.ai_family = AF_INET; //IPv4
	hints.ai_socktype = SOCK_STREAM; //TCP
	hints.ai_flags = AI_PASSIVE; // bind with local address

	status = getaddrinfo(ip.c_str(), port.c_str() , &hints, &ai);
	if (status)
		throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(status)));

	// Loop through results and try to create + bind a socket
	for (p = ai; p != NULL; p = p->ai_next)
	{
		//look for a valid socket
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0)
			continue;

		// allow addr reuse
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		// bind socket to address
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

	// Put socket into listening state
	if (listen(listener, 10) == -1)
	{
		close(listener);
		throw std::runtime_error("Listen failed");
	}

	return listener;
}

//add listener fd and servers to '_socket_to_servers' map
void Connection::add_listener_socket()
{
	//group servers according to Ip & Port
	group_servers();

	//get listener socket for each unique key (ip-port)
	std::map<std::pair<std::string, std::string>, std::vector<Server> >::iterator itr ;
	for (itr = _addr_server.begin(); itr != _addr_server.end(); ++itr)
	{
		const std::string &ip = itr->first.first;
		const std::string &port = itr->first.second;
		int socket_fd = get_listener_socket(ip, port);

		socket_to_servers[socket_fd] = itr->second;
	}
}

//iterate through all the sockets in pfds
void Connection::process_connections()
{
	// Iterate through all monitored file descriptors
	for (int i = 0; i < _fd_count; i++)
	{
		int fd = (_pfds)[i].fd;

		// Case 1: A listening socket has incoming data (new client connection)
		// Accept and register new client
		if (listening_fds.count(fd) && (_pfds)[i].revents & POLLIN)
			accept_connection(fd);

		// Case 2: A client has disconnected (hang up event)
		else if ((_pfds)[i].revents & POLLHUP)
			close_connection(i);

		// Case 3: A client socket is ready to read (incoming request data)
		else if ((_pfds)[i].revents & POLLIN)
			handle_client_read(i);

		// Case 4: A client socket is ready to write (send response data)
		else if ((_pfds)[i].revents & POLLOUT)
			handle_client_write(i);
	}
}

int g_signal = 0;


void	ft_signal(int signum)
{
	(void)signum;
	g_signal = 1;
	// _exit(1);
}

void Connection::close_all_sockets()
{
	for (int i = 0; i < _fd_count; i++)
	{
		int fd = (_pfds)[i].fd;
		close(fd);
	}
}

// poll : wait for events on multiple file descriptors
void Connection::runServers()
{
	//_socket_to_servers ready
	add_listener_socket();

	//add listener to the vector
	std::map<int, std::vector<Server> >::iterator it;
	for (it = socket_to_servers.begin(); it != socket_to_servers.end(); ++it)
	{
		pollfd listener;
		listener.fd = it->first;
		listener.events = POLLIN;

		_pfds.push_back(listener);	//add to pfds for poll
		listening_fds.insert(listener.fd);	//add to listening socket set

		//iterate through all the servers
		for (size_t i = 0; i < it->second.size(); i++)
		{	
			std::cout << CYAN << "Listener fd: " << listener.fd << RESET <<std::endl;
			// it->second[i].printInfo();
		}
	}
	// Number of fds currently being tracked by poll()
	_fd_count = _pfds.size();
	std::cout << LIME << "-----------------------------------------" << RESET << std::endl;	
	std::cout << GREEN << "42 Webserv: Start up succesful!" << RESET << std::endl;
	std::cout << YELLOW << "Waiting for Connections..." << RESET << std::endl;
	std::cout << LIME << "-----------------------------------------" << RESET << std::endl;

	// Main server loop: run until signal (Ctrl+C) stops the server
	while (!g_signal)
	{
		signal(SIGINT, ft_signal);

		// Wait for activity on any socket (blocking, -1 = no timeout)
		int poll_count = poll(_pfds.data(), _fd_count, -1);
		
		// If poll() fails
		if (poll_count == -1)
		{
			if (errno == EINTR) // signal detected
				continue;
			else
				throw std::runtime_error("poll failed");
		}
		// Handle any sockets that are ready
		process_connections();
	}
	std::cout << std::endl << RED << "Shutting down.\nCleaning up..." << RESET <<std::endl;
	close_all_sockets();
}
