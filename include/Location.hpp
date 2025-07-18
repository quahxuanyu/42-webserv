#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <set>

class Location {

	private:
		std::string path;
		std::string root;
		std::string alias;
		std::string index;

		bool autoindex;
		bool cgi;
		bool redirect;

		std::set<std::string> methods;
		std::string redirect_url;

		size_t max_body_size;
		size_t redirect_code;

	public:
		Location();
		

};

#endif