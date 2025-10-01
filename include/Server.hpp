#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <map>
#include <vector>
#include <poll.h>
#include "Client.hpp"
#include "Location.hpp"

class Server {
	private:
		std::string _IP;  //IP to bind
		std::string _port;	//port listening to
		std::string _server_name; 
		std::string _root;	//server root dir (from config file)
		long _body_size;	//content length (from header)
		std::map<int, std::string> _error_pages;
		std::vector<Location> _locations;  //location configs
		std::vector<pollfd> _pfds;  //list of fd the poll is watching
		std::map<int, Client> _clients;   //map of {fd : client}
		std::map<int, std::string> status_codes;  // status code -> status msg

		public:
		Server();
		Server(std::string IP, std::string port);
		void setServerName(std::string server_name);
		void setRoot(std::string root);
		void setBodySize(long size);
		void setErrorPage(int status, std::string page);
		void addLocation(Location location);

		std::string getIp() const;
		std::string getPort() const;
		std::string getRoot() const;
		std::string getServerInfo() const;
		std::map<int, std::string> getErrorPages() const;
		std::string getPage(int error_code) const;
		long getBodySizeLimit() const;
		std::vector<Location> getLocations() const;
		std::string getServerName() const;

		bool hasRoot () const;

		void printInfo() const;
		
		~Server();
};

// int get_listener_socket(const std::string &ip, const std::string &port);
// void runServers(void);
// void process_connections();

#endif