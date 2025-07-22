#include "../include/webserv.hpp"

void print_request(Request &request) {
	std::cout << request.getMethod() << " "
			  << request.getUri() << " "
			  << request.getVersion() << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
}

// int main(int argc, char *argv[], char *env[])
// {
// 	(void) argc;
// 	(void) argv;
// 	(void) env;
// 	std::cout << "----------Request-----------" << std::endl;
// 	Request request("GET", "/", "HTTP/1.1");
// 	request.addHeader("Host", "localhost");
// 	request.addHeader("User-Agent", "WebClient/1.0");
// 	print_request(request);

// 	std::cout << "\n----------Response-----------" << std::endl;
// 	Response &response = handle_get_response(request);
// 	std::cout << response.toString();
// 	delete &response;

// 	// std::cout << "----------CGI TEST-----------" << std::endl;
// 	// execute("./cgi-bin/test.cgi", env);
// }

Location parseLocation(std::vector<std::string> tokens, size_t *i)
{
	Location location;
	location.setPath(tokens[*i + 1]);
	*i += 3;
	while (*i < tokens.size() && tokens[*i] != "}")
	{
		if (tokens[*i] == "index")
		{
			location.setIndex(tokens[*i + 1]);
			*i += 2;
		}
		else if (tokens[*i] == "autoindex")
		{
			if (tokens[*i + 1] ==  "on")
				location.setAutoIndex(1);
			else if (tokens[*i + 1] ==  "off")
				location.setAutoIndex(0);
			*i += 2;
		}
		else if (tokens[*i] == "root")
		{
			location.setRoot(tokens[*i + 1]);
			*i += 2;
		}
		else if (tokens[*i] == "methods")
		{
			size_t j = *i;
			while (++j < tokens.size() && tokens[j] != ";")
			{
				if (tokens[j] == "GET" || tokens[j] == "POST" || tokens[j] == "DELETE")
					location.setMethod(tokens[j]);
			}
			*i = j + 1;
		}
		else if (tokens[*i] == "return")
		{
			location.setRedirectCode(atoi(tokens[*i + 1].c_str()));
			location.setRedirectUrl(tokens[*i + 2]);
			*i += 4;
		}
		else if (tokens[*i] == "cgi")
		{
			if (tokens[*i + 1] ==  "on")
				location.setCGI(1);
			else if (tokens[*i + 1] ==  "off")
				location.setCGI(0);
			*i += 2;
		}
		else
			(*i)++;
	}
	(*i)++;
	return location;
}

std::vector<Server> parseServer(std::vector<std::string> tokens)
{
	size_t i = 0;
	std::vector<Server> servers;

	while (i < tokens.size())
	{
		if ((tokens[i] == "server") && (tokens[i + 1] == "{"))
		{
			Server server_1;
			i += 2;
			while (i < tokens.size() && tokens[i] != "}")
			{
				if (tokens[i] == "listen")
				{
					size_t colon = tokens[i + 1].find(':');
					std::string IP = tokens[i + 1].substr(0, colon);
					std::string port = tokens[i + 1].substr(colon + 1);
					server_1 = Server(IP, port);
					i += 3;
				}
				else if (tokens[i] == "server_name")
				{
					server_1.setServerName(tokens[i + 1]);
					i += 3;
				}
				else if (tokens[i] == "root")
				{
					server_1.setRoot(tokens[i + 1]);
					i += 3;
				}
				else if (tokens[i] == "client_max_body_size")
				{
					server_1.setBodySize(strtol(tokens[i + 1].c_str(), NULL, 10));
					i += 3;
				}
				else if (tokens[i] == "error_page")
				{
					server_1.setErrorPage(atoi(tokens[i + 1].c_str()), tokens[i + 2]);
					i += 4; 
				}
				else if (tokens[i] == "location")
				{
					Location location = parseLocation(tokens, &i);
					server_1.addLocation(location);
				}
				else
					i++;
			}
			servers.push_back(server_1);
			i++;
		}
	}
	return (servers);
}

void checkSemicolons(std::vector<std::string> tokens)
{
	size_t i = 0;
	while (i < tokens.size() && tokens[i] != "{")
		i++;
	while (i < tokens.size() && tokens[i] != "}")
	{
		
		i++;
	}
}

void checkDirectives(std::vector<std::string> tokens)
{
	std::stack<std::string> stack;
	std::string prev_token;
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] == "server")
		{	
			if (!stack.empty())
				throw SyntaxError("server block must be at top-level");
			stack.push(tokens[i]);
			if (tokens[i + 1] != "{")
				throw SyntaxError("server block must be in '{}'");
		}
		else if (tokens[i] == "location")
		{
			if (stack.empty() || stack.top() != "server")
				throw SyntaxError("location block must be inside server block");
			stack.push(tokens[i]);
			if (tokens[i + 1][0] != '/' )
				throw SyntaxError("location block must be given URI");
			if (tokens[i + 2] != "{")
				throw SyntaxError("location block must be in '{}'");	
		}
		else if (tokens[i] == "{")
		{
			if (!(prev_token == "server" || prev_token == "location")) 
				throw SyntaxError("'{' must follow server or location block");
		} 
		else if  (tokens[i] == "}")
		{
			if (stack.empty())
				throw SyntaxError("Unexpected '}'");
			stack.pop();
		}
		prev_token = tokens[i];
	}
	if (!stack.empty())
		throw SyntaxError("Unmatched '{'");
}

void checkBraces(std::vector<std::string> tokens)
{
	std::stack<std::string> stack;
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] == "{") 
			stack.push(tokens[i]);
		else if (tokens[i] == "}")
		{	if (stack.empty())
				throw SyntaxError("Unmatched closing brace");
			stack.pop();}
	}
	if (!stack.empty())
		throw SyntaxError("Unclosed brace");
}

void checkTokens(std::vector<std::string> tokens)
{
	checkBraces(tokens);
	checkDirectives(tokens);
	checkSemicolons(tokens);

}

void tokenise(std::string content)
{
	std::vector<std::string> tokens;
	size_t i = 0;
	while (i < content.length())
	{
		if (content[i] == ' ')
		{	
			i++;
			continue;
		}
		else if (content[i] == '{' || content[i] == '}' || content[i] == ';')
			tokens.push_back(std::string(1, content[i++]));
		else if (isascii(content[i]))
		{
			size_t word_end = content.find_first_of(" \t{};", i);
			if (word_end == std::string::npos)
				word_end = content.length();
			std::string word = content.substr(i, word_end - i);
			tokens.push_back(word);
			i = word_end;
		}
	}
	for (size_t i = 0; i < tokens.size(); i++)
		std::cout << tokens[i] << std::endl;
	try
	{
		checkTokens(tokens);
		std::vector<Server> servers = parseServer(tokens);
		for (size_t i = 0; i < servers.size(); i++)
			servers[i].printInfo();
	}
	catch (SyntaxError &e)
	{
		std::cerr << "Config file syntax error: " << e.what() << std::endl;
		exit(1);
	}
	catch (ParseException &e)
	{
		std::cerr << "Config file parsing error: " << e.what() << std::endl;
		exit(1);
	}
}

void parseConfigFile(char *file)
{
	std::string content;
	std::string line;
	std::ifstream ConfigFile(file);

	while(getline(ConfigFile, line))
	{
		size_t start = line.find_first_not_of(" \t");
		// if not empty line
		if (start != std::string::npos)
		{
			size_t comment = line.find("#");
			if (comment == std::string::npos)
				comment = line.length();
			content += line.substr(start, comment - start);
		}
	}
	tokenise(content); 
}

int main(int argc, char **argv)
{
	if (argc != 2)
		return (std::cout << "Incorrect number of arguments" << std::endl, 2);
	parseConfigFile(argv[1]);
    // try 
	// {
    //     Server server("0.0.0.0", "8080");
    //     server.multiplexing();  // Run forever
    // } 
	// catch(const std::exception &e)
	// {
	// 	std::cerr << RED << "Server initialization failed: " << e.what() << RESET << std::endl;
	// 	return 1;
	// }
}


