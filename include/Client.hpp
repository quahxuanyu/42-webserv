#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Request.hpp"
#include "Response.hpp"
#include <iostream>
#include <vector>
#include <poll.h>
#include <netdb.h> // addrinfo 
#include <cstring>
#include <unistd.h>


class Server;

class Client {
	private:
		int _fd;		//client socket fd
		int _port;		//port used by client;
		std::string _IP;		//client IP addr
		std::string recv_buf;	//store incoming request data
		std::string send_buf;	//store outgoing response
		Request request;	//store HTTP request (obj)
		Response response;
		
		// time_t last_activity;	//timeout handling
		// bool header_parsed;		//check if header has fully received
		// bool keep_alive;	//check if the connection should be closed

		// USER DATA //
		std::string _session_id;
		Session *_session;

	public:
		int socket_fd;		//server that client connects to 
		bool recv_data(std::vector<pollfd> *pfds, int pfd_i);
		bool send_data(std::vector<pollfd> *pfds, int pfd_i);
		void parse_request();
		//void store_data();
		void getSession();
		// void generate_response();
		//bool is_time_out();

		//std::string getData(const std::string key);

		Client();
		Client(int fd);
};


#endif