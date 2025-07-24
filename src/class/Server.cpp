#include "../../include/webserv.hpp"

Server::Server()
{
	//get_listener_socket();
	//get_listener_socket();
	std::cout << "Server default constructor called" << _port << std::endl;
}

Server::Server(std::string IP, std::string port) : _IP(IP), _port(port), _fd_count(0)
{
	// get_listener_socket();
	// std::cout << "Server (" << _IP << " " << _port << ") created, fd: " << _listener_fd << std::endl;
}

void Server::setServerName(std::string server_name)
{
	_server_name = server_name;
}

void Server::setRoot(std::string root)
{
	_root = root;
}

void Server::setBodySize(long size)
{
	_body_size = size;
}

void Server::setErrorPage(int status, std::string page)
{
	_error_pages[status] = page;
}

void Server::addLocation(Location location)
{
	_locations.push_back(location);
}

long Server::getBodySizeLimit() const
{
	return _body_size;
}

std::vector<Location> Server::getLocations() const
{
	return _locations;
}


std::string Server::getIp() const
{
	return _IP;
}
std::string Server::getPort() const
{
	return _port;
}

void Server::printInfo() const
{
	std::cout << "=== Server's Info ===" << std::endl;
	std::cout << "Server's name : " << _server_name << std::endl;
	std::cout << "Ip address : " << _IP << std::endl;
	std::cout << "Port number : " << _port << std::endl;
	std::cout << "Root : " << _root << std::endl;
	std::cout << "Body size : " << _body_size << std::endl;
	std::cout << "Error pages :" << std::endl;

	std::map<int, std::string>::const_iterator it;
	for (it = _error_pages.begin(); it != _error_pages.end(); ++it)
		std::cout << it->first<< " : " << it->second << std::endl;

	std::cout << "Locations :" << std::endl;
	for (size_t i = 0; i < _locations.size(); i++)
		_locations[i].printInfo();
}

std::string Server::getRoot() const
{
	return _root;
}

std::string Server::getServerInfo() const
{
	return (_IP + _port + _server_name);
}

std::map<int, std::string> Server::getErrorPages() const
{
	return _error_pages;
}



//create new poll fd and add to pfds vector
void Server::add_to_pfds(int client_fd)
{	
	pollfd new_fd;
	new_fd.fd = client_fd;
	new_fd.events = POLLIN; //notified when socket has incoming data
	new_fd.revents = 0;

	_pfds.push_back(new_fd);
	_fd_count++;
	std::cout << "number of socket: " << _fd_count << std::endl;
}


void Server::accept_connection()
{
	struct sockaddr_storage client_addr;
	socklen_t addr_len;
	int new_fd;

	addr_len = sizeof(client_addr);
	new_fd = accept(_listener_fd, (struct sockaddr *)&client_addr, &addr_len);

	if (new_fd == -1)
		throw std::runtime_error("Aceept failed");
	else
	{
		//create new client and add to the map
		_clients[new_fd] = Client(new_fd);
		add_to_pfds(new_fd);
		std::cout << GREEN << "Server: new connection from fd " << new_fd << GREEN << std::endl;
	}
}

void Server::close_connection(int pfd_i)
{
	std::cout << RED << "Closing connection fd " << _pfds[pfd_i].fd << RESET << std::endl;

	_clients.erase(_pfds[pfd_i].fd);	//remove client from map

	close(_pfds[pfd_i].fd);	//close client fd
	_pfds[pfd_i] = _pfds[_fd_count - 1];	//replace it with the last pfds (efficiency)
	_fd_count--;
}

void Server::handle_client_read(int pfd_i)
{
	int client_fd = _pfds[pfd_i].fd;
	if (!_clients[client_fd].recv_data(&_pfds, pfd_i))
		close_connection(pfd_i);
}

void Server::handle_client_write(int pfd_i)
{
	int client_fd = _pfds[pfd_i].fd;
	if (!_clients[client_fd].send_data(&_pfds, pfd_i))
		close_connection(pfd_i);
}

void Server::process_connections()
{
	for (int i = 0; i < _fd_count; i++)
	{
		if ((_pfds)[i].fd == _listener_fd && (_pfds)[i].revents & POLLIN)
			accept_connection();
		else if ((_pfds)[i].revents & POLLHUP)
			close_connection(i);
		else if ((_pfds)[i].revents & POLLIN)
		{	
			std::cout << RED << "recv smtg from client" << RESET <<  std::endl;
			handle_client_read(i);}
		else if ((_pfds)[i].revents & POLLOUT)
			handle_client_write(i);
	}
}


Server::~Server() {}


int get_listener_socket(const std::string &ip, const std::string &port)
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

	return listener;
}

void runServers(std::vector<BindSocket> listening_sockets)
{
	//add listener to the vector
	//get_listener_socket();


	pollfd listener;
	listener.
	listener.fd = _listener_fd;
	listener.events = POLLIN;
	_pfds.push_back(listener);
	_fd_count = 1;

	std::cout << "Server: waiting for connections.." << std::endl;
	while (1)
	{
		int poll_count = poll(_pfds.data(), _fd_count, -1);
		if (poll_count == -1)
			throw std::runtime_error("poll failed");
		process_connections();
	}
}
