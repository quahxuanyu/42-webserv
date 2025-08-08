#include "../../include/webserv.hpp"

std::string get_content_type(const std::string &cgi_output)
{
	size_t header_start = cgi_output.find("Content-Type:");
	if (header_start == std::string::npos)
		return ""; // Not found

	size_t colon_pos = cgi_output.find(":", header_start);
	size_t line_end = cgi_output.find("\r\n\r\n", header_start);

	if (colon_pos == std::string::npos || line_end == std::string::npos)
		return "";

	// Extract and trim the value
	std::string value = cgi_output.substr(colon_pos + 1, line_end - colon_pos - 1);

	// Trim whitespace (left and right)
	size_t start = value.find_first_not_of(" \t\r");
	size_t end = value.find_last_not_of(" \t\r");
	if (start != std::string::npos && end != std::string::npos)
		return value.substr(start, end - start + 1);
	else
		return "";
}

std::string get_body(const std::string &cgi_output)
{
	size_t body_start = cgi_output.find("\r\n\r\n");
	if (body_start == std::string::npos)
		return ""; // Not found

	// Extract the body
	std::string body = cgi_output.substr(body_start + 4); // +4 to skip the "\r\n\r\n"

	// Trim leading and trailing whitespace (including newlines)
	size_t start = body.find_first_not_of(" \t\r\n");
	size_t end = body.find_last_not_of(" \t\r\n");
	if (start != std::string::npos && end != std::string::npos)
		return body.substr(start, end - start + 1);
	else
		return ""; // Body is empty or contains only whitespace
}
