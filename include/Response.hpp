#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include <iostream>
#include <map>

class Response {
	private:
		std::string _version;
		int _statusCode;
		std::string _statusMessage;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _path;

	public:
		Response();
		Response(std::string version, int statusCode, std::string statusMessage);
		~Response();

		void setPath(const std::string path);
		void setVersion(const std::string version);
		void setStatusCode(int code);
		void setStatusMessage(const std::string mesg);
		void addHeader(const std::string key, const std::string value);
		void setBody(const std::string body);

		const std::string &getPath() const;
		const std::string &getVersion() const;
		int getStatusCode() const;
		const std::string &getStatusMessage() const;
		const std::map<std::string, std::string> &getHeaders() const;
		const std::string &getBody() const;
		std::string toString() const;
};

#endif