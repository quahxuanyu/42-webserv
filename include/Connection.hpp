#ifndef CONNECTION_HPP
#define CONNECTION_HPP


class Connection {
private:
	int _fd_count;
	std::vector<Server> _servers;
	std::map<std::pair<std::string, std::string>, std::vector<Server> > _addr_server; // map of {[ip, port], servers}
	//std::vector<BindSocket> _listening_sockets; //match : ip, port - socket fd - servers
	std::vector<pollfd> _pfds;  //list of sockets poll is watching
	std::set<int> listening_fds;	//set of listening sockets
	std::map<int, Client> _clients;   //map of {fd : client}
	

public:
	Connection(std::vector<Server> servers);
	void runServers();

private:
	void group_servers();
	int get_listener_socket(const std::string &ip, const std::string &port);
	void add_listener_socket();
	void process_connections();
	void accept_connection(int listen_fd);
	void add_to_pfds(int client_fd);
	void handle_client_read(int client_fd);
	void handle_client_write(int client_fd);
	void close_connection(int fd);
	void close_all_sockets();
};

#endif

