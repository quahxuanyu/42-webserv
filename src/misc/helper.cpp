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

std::string trim (std::string str)
{
	size_t start = str.find_first_not_of(" \t\n\r");
	if (start == std::string::npos)
		return "";
	size_t end = str.find_last_not_of(" \t\n\r");
	return str.substr(start, end - start + 1);
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
void set_headers(Response &response)
{
	response.addHeader("Connection", "keep-alive");
    response.addHeader("Content-Length", to_string(response.getBody().length()));
	response.addHeader("Content-Type", find_mime(response.getPath())); //**Might not be 100% correct */
	response.addHeader("Date", get_current_time());
	response.addHeader("Server", "Webserv/1.0");
}

/***
 * @brief Reads the printed value fo the execve CGI file and returns parse into a response
 */
Response &parse_cgi_response(Response &response, std::string cgi_response) {
	response.setVersion("HTTP/1.1");
	response.setStatusCode(200);
	response.setStatusMessage("OK");
	
	response.setBody(get_body(cgi_response)); // Extract body from CGI output
	response.addHeader("Connection", "keep-alive");
	response.addHeader("Content-Length", to_string(response.getBody().length()));
	response.addHeader("Content-Type", get_content_type(cgi_response)); // Extract content type from CGI output
	response.addHeader("Date", get_current_time());
	response.addHeader("Server", "Webserv/1.0");
	return response;
}

std::string extractSessionID(std::string cookie)
{
	std::istringstream stream(cookie);
	std::string token;
	while(std::getline(stream, token, ';')) // if no coockie, loop ends
	{
		// Remove leading spaces (common in cookies)
		token.erase(0, token.find_first_not_of(" \t"));

		size_t pos = token.find('=');
		if (pos != std::string::npos)
		{
			std::string key = token.substr(0, pos);
			std::string value = token.substr(pos + 1);
			if (key == "session_id")
				return value;
		}
	}
	return "";
}

//generate a string of 50 random char
std::string generateSessionID()
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

void parse_noncgi_response(Response &response)
{
	response.setVersion("HTTP/1.1");
	response.setStatusCode(200);
	response.setStatusMessage("OK");
	std::ifstream src("html/loggedin.html", std::ios::binary);
	std::string body = read_file(src);
	response.setBody(body);
	src.close();
	response.addHeader("Connection", "keep-alive");
	response.addHeader("Content-Length", to_string(response.getBody().length()));
	response.addHeader("Content-Type", find_mime("html/loggedin.html")); // Extract content type from CGI output
	response.addHeader("Date", get_current_time());
	response.addHeader("Server", "Webserv/1.0");
}

Session createSession(Request &request)
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
	std::cout << RED << session._data["username"] << RESET <<std::endl; 
	return session;
}

//
void updateSession(Session *session, Request &request)
{
	std::string body = request.getBody();
	std::istringstream stream(body);
	std::string pair;
	// printSessionData(*session);

	while (std::getline(stream, pair, '&'))
	{
		size_t equal = pair.find('=');
		if (equal != std::string::npos)
		{
			std::string key = pair.substr(0, equal);
			std::string value = pair.substr(equal + 1);
			session->_data[key] = value;
		}
	}
	session->_visit_count += 1;
	// printSessionData(*session);
}


/**
 * @brief Handles the response error response
 * @param response The response object to set the error
 * @param path The path to the error page
 * @param error_code The HTTP error code to set in the response
 */
void handle_response_error(Response &response, std::string path, int error_code)
{
	set_headers(response);
	if (error_code == 504)
	{	
		response.addHeader("Connection", "closed");
	}
	response.setVersion("HTTP/1.1");
	response.setStatusCode(error_code);
	response.setStatusMessage(httpErrorMessages[error_code]);
	response.addHeader("Content-Type", "text/html");

	if (!path.empty())
	{
		response.setPath(path);
		if (path[0] == '/')
			path = "." + path;
		std::ifstream src(path.c_str(), std::ios::binary);
		response.setBody(read_file(src));
		src.close();
	}
	else
		response.setBody(defaultErrorPage(error_code));
	return ;
}

std::string trimTrailingSlash(const std::string &string) {
	if (string.length() > 1 && string[string.length() - 1] == '/') {
		return string.substr(0, string.length() - 1);
	}
	else {
		return string;
	}
}

std::string defaultErrorPage(int error_code)
{
	std::string html;
	html = "<!DOCTYPE html>\n<html>\n";
	html.append("<head>\n<title>Default Error Page</title>\n</head>\n");
	html.append("<body>\n");
	html.append("<h1>" + to_string(error_code) + "</h1>\n");
	html.append("<p>" + httpErrorMessages[error_code] + "</p>\n");
	html.append("</body>\n");
    html.append("</html>\n");

	return html;
}

/**
 * @brief Decodes a URL-encoded string.
 * Changes %XX to the corresponding character and + to space.
 */
std::string urlDecode(const std::string &src) {
    std::string ret;
    char ch;
    int i, ii;
    for (i = 0; i < (int)src.length(); i++) {
        if (src[i] == '%') {
            sscanf(src.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i += 2;
        } else if (src[i] == '+') {
            ret += ' '; // convert + to space
        } else {
            ret += src[i];
        }
    }
    return ret;
}

void processLocationRequest(const Server &server, Request &request, const Location *location)
{
	if (location->hasRoot() || location->hasAlias() || server.hasRoot())
    {
        if (location->hasRoot())
            request.setUri(location->getRoot() + request.getUri());
        else if (location->hasAlias())
        {
            std::string suffix = request.getUri().substr(location->getPath().length()); // get substring after location path (replace location path)
            if (suffix.empty() || suffix[0] != '/')
                suffix = "/" + suffix; // ensure it starts with a slash
            request.setUri(location->getAlias() + suffix);
        }
        else if (server.hasRoot())
            request.setUri(server.getRoot() + request.getUri());
    }
}