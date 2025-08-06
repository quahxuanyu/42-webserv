#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>

class Request {
	private:
		std::string _method;
		std::string _uri;
		std::string _version;
		std::map<std::string, std::string> _headers;
		std::string _body;
		bool _hasHeader;
		

	public:
		bool _isbad;
		bool has_body;
		Request();
		Request(std::string method, std::string uri, std::string version);
		~Request();

		void sethasHeader(bool has);
		void setMethod(const std::string method);
		void setUri(const std::string uri);
		void setVersion(const std::string version);
		void addHeader(const std::string key, const std::string value);
		void setBody(const std::string body);

		const bool &hasHeader() const;
		const std::string &getMethod() const;
		const std::string &getUri() const;
		const std::string &getVersion() const;
		const std::map<std::string, std::string> &getHeaders() const;
		const std::string &getHeader(std::string key) const;
		const std::string &getBody() const;

		void printRequest() const;

};

#endif