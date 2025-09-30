#include "../include/webserv.hpp"


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
		else if (tokens[*i] == "alias")
		{
			location.setAlias(tokens[*i + 1]);
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
			location.setRedirectCode(strtol(tokens[*i + 1].c_str(), NULL, 10));
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

bool isDirective(std::string keyword)
{
	if (keyword == "listen" || keyword == "server_name" || keyword == "root" || keyword == "error_page" 
	|| keyword == "client_body_size_buf" || keyword == "index" || keyword == "methods" || keyword == "return" 
	|| keyword == "cgi" || keyword == "autoindex" || keyword == "alias")
		return true;
	else 
		return false;
}

void checkSemicolons(std::vector<std::string> tokens)
{
	size_t i = 0;
	while (i < tokens.size())
	{
		if (tokens[i] == "server")
			i += 2;
		else if (tokens[i] == "location")
			i += 3;
		else if (isDirective(tokens[i]))
		{
			std::string directive = tokens[i];
			i++;

			//stop if reach ';' // else raise error 
			while (!(i >= tokens.size() || tokens[i] == ";" || tokens[i] == "{" || tokens[i] == "}" || isDirective(tokens[i])
			|| tokens[i] == "server" || tokens[i] == "location"))
				i++;
			if (i >= tokens.size() || tokens[i] != ";")
				throw SyntaxError("Missing ';' after " + directive);

			i++; //skip semicolon
		}
		else if (tokens[i] == "{" || tokens[i] == "}")
			i++;
		else
			throw SyntaxError("Unknown keyword : " + tokens[i]);
	}
}

void checkDirectives(std::vector<std::string> tokens)
{
	std::stack<std::string> stack;
	std::string prev_token;
	std::string prev2_token;
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
			if (!(prev_token == "server" || (prev2_token == "location" && prev_token[0] == '/'))) 
				throw SyntaxError("'{' must follow server or location block");
		} 
		else if  (tokens[i] == "}")
		{
			if (stack.empty())
				throw SyntaxError("Unexpected '}'");
			stack.pop();
		}
		prev2_token = prev_token;
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
		throw SyntaxError("Unclosed opening brace");
}

void checkTokens(std::vector<std::string> tokens)
{
	//check for balance of '{}'
	checkBraces(tokens);
	// check if server and location followed by {, server comes before location
	checkDirectives(tokens);

	checkSemicolons(tokens);
}


void validateConfig(std::vector<Server> &servers)
{
	std::cout << "Validating config..." << std::endl;
	std::set<std::string> IP_Port_Server;
	for (size_t i = 0; i < servers.size(); i++)
	{
		//check error page
		std::map<int, std::string> error_pages = servers[i].getErrorPages();
		std::map<int, std::string>::const_iterator it;
		for (it = error_pages.begin(); it != error_pages.end(); ++it)
		{
			if (!isFile(it->second))
				throw ParseException("Error page must be a file");}

		//check body size limit
		if (servers[i].getBodySizeLimit() <= 0)
			throw ParseException("Client body size limit must be a positive number");

		std::pair<std::set<std::string>::iterator, bool> is_inserted = IP_Port_Server.insert(servers[i].getServerInfo());
		if (!is_inserted.second)
			throw ParseException("Duplicate server block");
	}
}

std::vector<Server> tokenise(std::string content)
{
	std::vector<std::string> tokens;
	size_t i = 0;
	
	while (i < content.length())
	{
		if (content[i] == ' ')
			i++;
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
	// for (size_t i = 0; i < tokens.size(); i++)
	// 	std::cout << "token [" << i << "] : "<<tokens[i] << std::endl;
	try
	{
		checkTokens(tokens);
		std::vector<Server> servers = parseServer(tokens);
		// for (size_t i = 0; i < servers.size(); i++)
		// 	servers[i].printInfo();
		validateConfig(servers);
		std::cout << GREEN << "No issue!" << RESET << std::endl;
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

std::vector<Server> parseConfigFile(char *file)
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
			content += " ";
		}
	}
	std::vector<Server> servers = tokenise(content); 
	return (servers);
}