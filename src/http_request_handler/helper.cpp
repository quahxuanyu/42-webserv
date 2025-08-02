#include "../../include/webserv.hpp"

// Converts an integer to a string
std::string to_string(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string replaceAll(std::string str, const std::string &src, const std::string &target)
{
	size_t start = 0;
	while ((start = str.find(src, start))!= std::string::npos)
	{
		str.replace(start, src.length(), target);
		start += src.length();
	}
	return str;
}

/**
 * @brief Gets the current time in GMT format.
 */
std::string get_current_time() {
    std::time_t now = std::time(NULL);       // Get current time
    std::tm* gmt = std::gmtime(&now);        // Convert to UTC/GMT

    char buf[100];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", gmt);
    std::string current_time(buf);

    return (current_time);
}

/**
 * @brief Finds the MIME type based on the URI.
 */
std::string find_mime(std::string uri) {
    // For simplicity, we will return a hardcoded MIME type based on the file extension
    if (uri.find(".html") != std::string::npos || uri == "/")
        return "text/html";
    else if (uri.find(".css") != std::string::npos)
        return "text/css";
    else if (uri.find(".js") != std::string::npos) 
        return "application/javascript";
    else if (uri.find(".png") != std::string::npos)
        return "image/png";
    else if (uri.find(".jpg") != std::string::npos || uri.find(".jpeg") != std::string::npos)
        return "image/jpeg";
    else if (uri.find(".cgi") != std::string::npos)
        return "text/plain"; // Assuming CGI scripts return plain text
    else
        return "application/octet-stream"; // Default MIME type
}

/**
 * @brief Sets the headers for the response based on the request and response status.
 */
void set_headers(Response &response, Request &request)
{
    (void)request;
	if (response.getStatusCode() == 200)
		response.addHeader("Connection", "keep-alive");
	else
		response.addHeader("Connection", "close");
    response.addHeader("Content-Length", to_string(response.getBody().length()));
	response.addHeader("Content-Type", find_mime(response.getPath())); //**Might not be 100% correct */
	response.addHeader("Date", get_current_time());
	response.addHeader("Server", "Webserv/1.0"); // ** TEMPORARY, wait until config file is implemented **
}

/***
 * @brief Reads the printed value fo the execve CGI file and returns parse into a response
 */
Response &parse_cgi_response(std::string cgi_response) {
	Response *response = new Response("HTTP/1.1", 200, "OK");
	response->setBody(get_body(cgi_response)); // Extract body from CGI output
	response->addHeader("Connection", "keep-alive");
	response->addHeader("Content-Length", to_string(response->getBody().length()));
	response->addHeader("Content-Type", get_content_type(cgi_response)); // Extract content type from CGI output
	response->addHeader("Date", get_current_time());
	response->addHeader("Server", "Webserv/1.0"); // ** TEMPORARY, wait until config file is implemented **
	return *response;
}

std::string extractSessionID(std::string cookie)
{
	std::istringstream stream(cookie);
	std::string token;
	while(std::getline(stream, token, ';'))
	{
		size_t pos = token.find("session_id");
		if (pos != std::string::npos)
			return (token.substr(pos + 11));
	}
	return "";
}

//generate a string of 50 random char
std::string generateSessionId()
{
	static const char alphanum[] =
	"0123456789"
	"!@#$%^&*"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz";

	int stringLength = sizeof(alphanum) - 1;
	std::string session_id;
    srand(time(0));
    for(int z=0; z < 50; z++)
    {
        session_id += alphanum[rand() % stringLength];
    }
	return session_id;
}

Response &parse_noncgi_response()
{
    Response *response = new Response("HTTP/1.1", 200, "OK");
	std::ifstream src("html/loggedin.html", std::ios::binary);
	std::string body = read_file(src);
	// body = replaceAll(body, "{{VISIT_COUNT}}", "1");
	// body = replaceAll(body, "{{USERNAME}}", "Joophang");
	response->setBody(body);
	src.close();
	response->addHeader("Connection", "keep-alive");
	response->addHeader("Content-Length", to_string(response->getBody().length()));
	response->addHeader("Content-Type", find_mime("html/loggedin.html")); // Extract content type from CGI output
	response->addHeader("Date", get_current_time());
	response->addHeader("Server", "Webserv/1.0"); // ** TEMPORARY, wait until config file is implemented **
	return *response;
}

Session createSession(Request request)
{
	Session session;
	std::string body = request.getBody();
	session._visit_count = 1;

	//get request has no body
	if (body.empty())
		return session;
	std::istringstream stream(body);
	std::string pair;
	while (std::getline(stream, pair, '&'))
	{
		size_t equal = pair.find('=');
		if (equal != std::string::npos)
		{
			std::string key = pair.substr(0, equal);
			std::string value = pair.substr(equal + 1);
			session._data[key] = value;
		}
	}
	//std::cout << RED << session._data["username"] << RESET <<std::endl; 
	return session;
}

