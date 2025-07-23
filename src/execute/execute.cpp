#include "../../include/webserv.hpp"

std::string execute(Request &request, char **env) {
    int pipefd[2];
	std::string cgi_output;

	pipe(pipefd);  // pipefd[0] = read, pipefd[1] = write
    std::string cgi_path = request.getUri();
    char *argv[] = {const_cast<char*>(cgi_path.c_str()), NULL};
    if (fork() == 0)
    {
		dup2(pipefd[0], STDIN_FILENO);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        if (execve(cgi_path.c_str(), argv, env) == -1)
            return "Error executing CGI script: " + std::string(strerror(errno));
    }
    else
    {
		write(pipefd[1], request.getBody().c_str(), request.getBody().length());
		close(pipefd[1]);
		char buffer[1024];
	
		ssize_t bytes_read;
		waitpid(-1, NULL, WUNTRACED);

		while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0)
		{
			buffer[bytes_read] = '\0'; // Null-terminate
			cgi_output += buffer;
		}
	}
	close(pipefd[0]);
	return cgi_output;
}

std::string cgi(Request &request) {
	//Make environment variables for CGI execution
	std::string method_env = "REQUEST_METHOD=" + request.getMethod();
	std::string content_type = "CONTENT_TYPE=" + request.getHeader("Content-Type");
	std::string content_length = "CONTENT_LENGTH=" + to_string(request.getBody().length());
	char *envp[] = {
		const_cast<char *>(method_env.c_str()),
		const_cast<char *>(content_type.c_str()),
		const_cast<char *>(content_length.c_str()),
		NULL
	};
	return execute(request, envp);;
}