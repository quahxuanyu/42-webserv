#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <unordered_map>
#include <vector>
#include <poll.h>
#include "Client.hpp"

#define BLACK     "\033[0;30m"
#define RED       "\033[0;31m"
#define GREEN     "\033[0;32m"
#define YELLOW    "\033[0;33m"
#define BLUE      "\033[0;34m"
#define MAGENTA   "\033[0;35m"
#define CYAN      "\033[0;36m"
#define WHITE     "\033[0;37m"
#define RESET		"\033[0m"

class Server {
	private:
		int _listener_fd;  //listener's socket fd
		int _port;	//port listening to
		std::string _IP;  //IP to bind
		//std::vector<location> locations;  //location configs
		std::string server_name; 
		std::unordered_map<int, Client> _clients;   //map of {fd : client}
		std::string _root;	//server root dir (from config file)
		int _body_len;	//content length (from header)
		std::vector<pollfd> *_pfds;  //list of fd the poll is watching

	public:
		void get_listener_socket(void);
		void process_connections(int *fd_count, int *fd_size, std::vector<pollfd> *pfds);
		void accept_connection(int *fd_count, int *fd_size);
		void close_connection(int *fd_count, int *pfd_i);

		void add_to_pfds(int new_fd, int *fd_count, int *fd_size);
		void handle_client_read(int *fd_count, int *pfd_i);
		void handle_client_write(int *fd_count, int *pfd_i);
		void multiplexing(void);
};

#endif