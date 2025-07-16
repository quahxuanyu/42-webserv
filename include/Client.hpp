#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Request.hpp"
#include "Server.hpp"
#include <iostream>
#include <vector>
#include <poll.h>
#include <netdb.h> // addrinfo 
#include <cstring>
#include <unistd.h>
#include <unordered_map>

class Client {
	private:
		int _fd;		//client socket fd
		int _port;		//port used by client;
		std::string _IP;		//client IP addr
		std::string recv_buf;	//store incoming request data
		std::string send_buf;	//store outgoing response
		Request request;	//store HTTP request (obj)
		Server server;		//server that client connects to 
		time_t last_activity;	//timeout handling
		bool header_parsed;		//check if header has fully received
		bool keep_alive;	//check if the connection should be closed

	public:
		bool recv_data(std::vector<pollfd> *pfds, int *fd_count, int *pfd_i);
		void parse_request();
		void generate_response();
		bool send_data(std::vector<pollfd> *pfds, int *fd_count, int *pfd_i);
		bool is_time_out();

	public:
		Client();
		Client(int fd);
};


#endif