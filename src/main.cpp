#include "../include/webserv.hpp"


int main(int argc, char **argv)
{
	if (argc != 2)
		return (std::cout << "Incorrect number of arguments" << std::endl, 2);
	std::vector<Server> servers = parseConfigFile(argv[1]);
	try 
	{
		Connection connection(servers);
		connection.runServers();
	}
	catch (std::runtime_error &e)
	{
		std::cerr << RED << "Runtime Error: " << e.what() << RESET << std::endl;
		_exit(1);
	}
	catch (const std::exception &e)
	{
		std::cerr << RED << "Server initialization failed: " << e.what() << RESET << std::endl;
		return 1;
	}
}
