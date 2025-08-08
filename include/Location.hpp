#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <set>

class Location {

	private:
		std::string _path;
		std::string _root;
		std::string _alias;
		std::string _index;


		//bool _redirect;
		std::set<std::string> _methods;
		std::string _redirect_url;
		size_t _redirect_code;

	public:

		bool autoindex;
		bool cgi;

		Location();
		void setPath(std::string path);
		void setRoot(std::string root);
		void setAlias(std::string alias);
		void setIndex(std::string index);
		void setAutoIndex(int on);
		void setMethod(std::string);
		void setRedirectCode(int code);
		void setRedirectUrl(std::string url);
		void setCGI(int on);

		bool hasRoot () const;
		bool hasAlias () const;
		bool hasIndex () const;
		bool hasRedirectUrl () const;

		std::set<std::string> getMethods() const;
		std::string getRoot() const;
		std::string getAlias() const;
		std::string getIndex() const;
		std::string getPath() const;
		std::string getRedirectUrl() const;
		size_t getRedirectCode() const;
	


		void printInfo() const;
};

#endif