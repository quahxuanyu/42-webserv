#include "../include/webserv.hpp"


Location parseLocation(std::vector<std::string> tokens, size_t *i)
{
	Location location;
	location.setPath(tokens[*i + 1]);
	*i += 3;

	// Parse tokens until closing brace "}"
	while (*i < tokens.size() && tokens[*i] != "}")
	{
		if (tokens[*i] == "index")
		{
			// Set default file
			location.setIndex(tokens[*i + 1]);
			*i += 2;
		}
		else if (tokens[*i] == "autoindex")
		{
			// Enable/disable autoindexing
			if (tokens[*i + 1] ==  "on")
				location.setAutoIndex(1);
			else if (tokens[*i + 1] ==  "off")
				location.setAutoIndex(0);
			*i += 2;
		}
		else if (tokens[*i] == "root")
		{
			// Set root directory
			location.setRoot(tokens[*i + 1]);
			*i += 2;
		}
		else if (tokens[*i] == "alias")
		{
			// Set alias directory
			location.setAlias(tokens[*i + 1]);
			*i += 2;
		}
		else if (tokens[*i] == "methods")
		{
			// Allowed HTTP methods (GET, POST, DELETE)
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
			location.setRedirectCode(strtol(tokens[*i + 1].c_str(), NULL, 10));
			location.setRedirectUrl(tokens[*i + 2]);
			*i += 4;
		}
		else
			(*i)++;
	}
	(*i)++;
	return location;
}

// Parse a vector of tokens into a list of Server objects.
std::vector<Server> parseServer(std::vector<std::string> tokens)
{
	size_t i = 0;
	std::vector<Server> servers;

	while (i < tokens.size())
	{
		// Detect the start of a server block
		if ((tokens[i] == "server") && (tokens[i + 1] == "{"))
		{
			Server server_1;
			i += 2;  // Skip "server" and "{"

			// Parse directives inside the server block until we reach '}'
			while (i < tokens.size() && tokens[i] != "}")
			{
				if (tokens[i] == "listen")
				{
					// listen <IP>:<PORT>;
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
				else if (tokens[i] == "client_body_size_buf")
				{
					//if char convert to 0
					server_1.setBodySize(strtol(tokens[i + 1].c_str(), NULL, 10));
					i += 3;
				}
				else if (tokens[i] == "error_page")
				{
					server_1.setErrorPage(strtol(tokens[i + 1].c_str(), NULL, 10), tokens[i + 2]);
					i += 4; 
				}
				else if (tokens[i] == "location")
				{
					// create location block and add to the current server
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

// Turn the config string into tokens
std::vector<std::string> tokenise(std::string content)
{
	std::vector<std::string> tokens;
	size_t i = 0;
	
	// Go through the entire content character by character
	while (i < content.length())
	{
		if (content[i] == ' ')
			i++;
		else if (content[i] == '{' || content[i] == '}' || content[i] == ';')
			tokens.push_back(std::string(1, content[i++]));
		else if (isascii(content[i]))
		{
			// extract a "word" token
			size_t word_end = content.find_first_of(" \t{};", i);

			// if no delimiter is found, this is the last word in the content
			if (word_end == std::string::npos)
				word_end = content.length();

			// extract the word between i and word_end
			std::string word = content.substr(i, word_end - i);
			tokens.push_back(word);

			// move to the end of the word
			i = word_end;
		}
	}
	return tokens;
}

void checkTokens(std::vector<std::string> tokens)
{
	// 1. Check balance of '{' and '}'
	checkBraces(tokens);

	// 2. Ensure directives/blocks follow config grammar
	checkDirectives(tokens);

	// 3. Ensure directives are terminated properly with ';'
	checkSemicolons(tokens);
}

void validateConfig(std::vector<Server> &servers)
{
	std::cout << ORANGE << "Validating config..." << RESET<< std::endl;
	// Track unique IP:Port combinations to detect duplicates
	std::set<std::string> IP_Port_Server;
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::cout << RED << " * Validating Server " << i + 1 << "..." << RESET << std::endl;

		// Each error page must be a valid file
		std::map<int, std::string> error_pages = servers[i].getErrorPages();
		std::map<int, std::string>::const_iterator it;
		for (it = error_pages.begin(); it != error_pages.end(); ++it)
		{
			if (!isFile(it->second))
				throw ParseException("Error page must be a file");}
			//check body size limit
			if (servers[i].getBodySizeLimit() <= 0)
				throw ParseException("Client body size limit must be a positive number");

			// Insert (IP:Port) string into set; if already exists, throw error
			std::pair<std::set<std::string>::iterator, bool> is_inserted = IP_Port_Server.insert(servers[i].getServerInfo());
			if (!is_inserted.second)
				throw ParseException("Duplicate server block");
		}
}

//  Read a configuration file into a string
std::string readConfigFile(char *file)
{
	std::string content;
	std::string line;
	std::ifstream ConfigFile(file);

	// Read the file line by line
	while(getline(ConfigFile, line))
	{
		size_t start = line.find_first_not_of(" \t");
		// Process only if the line is not empty or all whitespace
		if (start != std::string::npos)
		{
			// Find the start of a comment
			size_t comment = line.find("#");
			if (comment == std::string::npos)
				comment = line.length();
			content += line.substr(start, comment - start);
			// Add a space to separate tokens between lines
			content += " ";
		}
	}
	return content;
}

// Parse a configuration file into a vector of Server objects
// Validate tokens and servers
std::map<int, std::string> httpErrorMessages;

std::vector<Server> parseConfigFile(char *file)
{
	try
	{
		std::string content = readConfigFile(file);
		std::vector<std::string> tokens = tokenise(content);
		checkTokens(tokens);
		std::vector<Server> servers = parseServer(tokens);
		validateConfig(servers);
		createErrorMap();

		std::cout << GREEN << "Config file OK" << RESET << std::endl;
		return (servers);
	}
	catch (SyntaxError &e)
	{
		std::cerr << RED << "Config file syntax error: " << e.what() << RESET << std::endl;
		_exit(1);
	}
	catch (ParseException &e)
	{
		std::cerr << RED << "Config file parsing error: " << e.what() << RESET << std::endl;
		_exit(1);
	}
}