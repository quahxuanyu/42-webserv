#include "../../include/webserv.hpp"

void execute(std::string uri, char **env) {
    int pipefd[2];
	pipe(pipefd);  // pipefd[0] = read, pipefd[1] = write

	(void) env; // Unused in this example, but could be used for environment variables
    std::string cgi_path = uri;
    char *argv[] = {const_cast<char*>(cgi_path.c_str()), NULL};
    if (fork() == 0)
    {
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);  // Close read end in child
		close(pipefd[1]);  // Close write end in child
        if (execve(cgi_path.c_str(), argv, env) == -1)
            std::cerr << "Error: cannot execute " << cgi_path << std::endl;
    }
    else
    {
		close(pipefd[1]);
		char buffer[1024];
		std::string cgi_output;
		ssize_t bytes_read;

		while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0)
		{
			buffer[bytes_read] = '\0'; // Null-terminate
			cgi_output += buffer;
		}

		close(pipefd[0]); // Close read end

		waitpid(-1, NULL, WUNTRACED);
		std::cout << "CGI executed: " << cgi_path << std::endl;
		std::cout << "CGI Output:\n" << cgi_output << std::endl;
	}
	close(pipefd[0]);
}