#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Request.hpp"
#include <iostream>
#include <vector>
#include <poll.h>
#include <netdb.h> // addrinfo 
#include <cstring>
#include <unistd.h>

#define BLACK     "\033[0;30m"
#define RED       "\033[0;31m"
#define GREEN     "\033[0;32m"
#define YELLOW    "\033[0;33m"
#define BLUE      "\033[0;34m"
#define MAGENTA   "\033[0;35m"
#define CYAN      "\033[0;36m"
#define WHITE     "\033[0;37m"
#define RESET		"\033[0m"


class Server;

class Client {
	private:
		int _fd;		//client socket fd
		int _port;		//port used by client;
		std::string _IP;		//client IP addr
		std::string recv_buf;	//store incoming request data
		std::string send_buf;	//store outgoing response
		Request request;	//store HTTP request (obj)
		Server *server;		//server that client connects to 
		time_t last_activity;	//timeout handling
		bool header_parsed;		//check if header has fully received
		bool keep_alive;	//check if the connection should be closed

	public:
		bool recv_data(std::vector<pollfd> *pfds, int pfd_i);
		bool send_data(std::vector<pollfd> *pfds, int pfd_i);
		void parse_request();
		void generate_response();
		bool is_time_out();

	public:
		Client();
		Client(int fd);
};


#endif