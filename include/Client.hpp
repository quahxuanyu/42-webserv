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


struct Session 
{
	bool logged_in;
	std::map<std::string, std::string> _data;
	int _visit_count;
};

class Server;

class Client {
	private:
		int _fd;		//client socket fd
		std::string _IP;		//client IP addr
		std::string recv_buf;	//store incoming request data
		std::string send_buf;	//store outgoing response
		Request request;	//store HTTP request (obj)
		Response *_response;
		time_t _start_time;	//timeout handling
		
		// USER DATA //
		std::string _session_id;
		Session *_session;

	public:
		int socket_fd;		//server that client connects to 

		void addSessionData();
		void processRequest(std::vector<pollfd> *pfds, int pfd_i);
		bool recv_data(std::vector<pollfd> *pfds, int pfd_i);
		bool send_data(std::vector<pollfd> *pfds, int pfd_i);
		void parse_request();
		void getSession();
		bool check_timeout(std::vector<pollfd> *pfds, int pfd_i);


		Client();
		Client(int fd);
};


#endif