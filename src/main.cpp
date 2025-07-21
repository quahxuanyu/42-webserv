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

void checkTokens(std::vector<std::string> tokens)
{
	size_t i = 0;
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
					size_t colon = tokens[i].find(':');
					std::string IP = tokens[i].substr(0, colon);
					std::string port = tokens[i].substr(colon);
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
					Location location;
					location.setPath(tokens[i + 1]);
					i += 3;
					while (i < tokens.size() && tokens[i] != "}")
					{
						if (tokens[i] == "index")
						{
							location.setIndex(tokens[i + 1]);
							i += 2;
						}
						if (tokens[i] == "autoindex")
						{
							if (tokens[i + 1] ==  "on")
								location.setAutoIndex(1);
							else if (tokens[i + 1] ==  "off")
								location.setAutoIndex(0);
							i += 2;
						}
						if (tokens[i] == "root")
						{
							location.setRoot(tokens[i + 1]);
							i += 2;
						}
						if (tokens[i] == "methods")
						{
							size_t j = i;
							while (++j < tokens.size() && tokens[j] != ";")
							{
								if (tokens[j] == "GET" || tokens[j] == "POST" || tokens[j] == "DELETE")
									location.setMethod(tokens[j]);
							}
							i += 2;
						}
						if (tokens[i] == "return")
						{
							location.setRedirectCode(atoi(tokens[i + 1].c_str()));
							location.setRedirectUrl(tokens[i + 2]);
						}
						if (tokens[i] == "cgi")
						{
							if (tokens[i + 1] ==  "on")
								location.setCGI(1);
							else if (tokens[i + 1] ==  "off")
								location.setCGI(0);
							i += 2;
						}

					}
				}
			}
		}
	}
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
	checkTokens(tokens);
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


