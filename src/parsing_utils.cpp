#include "../include/webserv.hpp"

bool isDirective(std::string keyword)
{
	if (keyword == "listen" || keyword == "server_name" || keyword == "root" || keyword == "error_page" 
	|| keyword == "client_body_size_buf" || keyword == "index" || keyword == "methods" || keyword == "return" 
	|| keyword == "cgi" || keyword == "autoindex" || keyword == "alias")
		return true;
	else 
		return false;
}

// Check that every directive is followed by a terminating ';'
void checkSemicolons(std::vector<std::string> tokens)
{
	size_t i = 0;
	while (i < tokens.size())
	{
		if (tokens[i] == "server")
			i += 2;  // Skip "server" + "{" (block header)
		else if (tokens[i] == "location")
			i += 3;  // Skip "location" + URI + "{"
		else if (isDirective(tokens[i]))
		{
			std::string directive = tokens[i];
			i++;

			//stop if reach ';' , or any directive/keyword
			while (!(i >= tokens.size() || tokens[i] == ";" || tokens[i] == "{" || tokens[i] == "}" 
			|| isDirective(tokens[i]) || tokens[i] == "server" || tokens[i] == "location"))
				i++;
			// reach end or missing semicolon
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

// Check structural rules for 'server' and 'location' blocks:
// - 'server' must be top-level
// - 'location' must be inside a 'server'
// - '{' must directly follow a block declaration
// - Blocks must close properly   
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
			
			// Server must be followed by '{'
			if (tokens[i + 1] != "{")
				throw SyntaxError("server block must be in '{}'");
		}
		else if (tokens[i] == "location")
		{
			// Location must be inside a server
			if (stack.empty() || stack.top() != "server")
				throw SyntaxError("location block must be inside server block");
			stack.push(tokens[i]);
			// Location must have a URI
			if (tokens[i + 1][0] != '/' )
				throw SyntaxError("location block must be given URI");
			// And must be followed by '{'
			if (tokens[i + 2] != "{")
				throw SyntaxError("location block must be in '{}'");	
		}
		else if (tokens[i] == "{")
		{
			// '{' is only valid right after 'server' or 'location <uri>'
			if (!(prev_token == "server" || (prev2_token == "location" && prev_token[0] == '/'))) 
				throw SyntaxError("'{' must follow server or location block");
		} 
		else if (tokens[i] == "}")
		{
			// '}' must match an open block
			if (stack.empty())
				throw SyntaxError("Unexpected '}'");
			stack.pop();
		}
		prev2_token = prev_token;
		prev_token = tokens[i];
	}
	// some block is not closed
	if (!stack.empty())
		throw SyntaxError("Unmatched '{'");
}

// Check that every '{' has a matching '}' and vice versa
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