#include "../../include/webserv.hpp"

std::string execute(Request &request, char **env) {
	int stdin_pipe[2];  // Parent writes, child reads
	int stdout_pipe[2]; // Child writes, parent reads
	std::string cgi_output;

	if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1) {
		std::cerr << "Error: Pipe failed" << std::endl;
		return "";
	}

	std::string cgi_path = request.getUri();
	char *argv[] = {const_cast<char*>(cgi_path.c_str()), NULL};

	pid_t pid = fork();
	if (pid == -1) {
		perror("fork failed");
		return "";
	}
	if (pid == 0) {
		// Child process
		dup2(stdin_pipe[0], STDIN_FILENO);   // Read from parent
		dup2(stdout_pipe[1], STDOUT_FILENO); // Write to parent

		close(stdin_pipe[1]);
		close(stdout_pipe[0]);
		close(stdin_pipe[0]);
		close(stdout_pipe[1]);

		if (execve(cgi_path.c_str(), argv, env) == -1) {
			perror("execve failed");
			exit(1);
		}
	} else {
		// Parent process
		close(stdin_pipe[0]);  // Close unused read end
		close(stdout_pipe[1]); // Close unused write end

		// Write request body to CGI's stdin
		write(stdin_pipe[1], request.getBody().c_str(), request.getBody().length());
		close(stdin_pipe[1]); // EOF to child

		// Read CGI output from child's stdout
		char buffer[1024];
		ssize_t bytes_read;
		while ((bytes_read = read(stdout_pipe[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytes_read] = '\0';
			cgi_output += buffer;
		}
		close(stdout_pipe[0]);

		// Wait for CGI to finish
		waitpid(pid, NULL, 0);
	}
	return cgi_output;
}

std::string cgi(Request &request) {

	std::cout << MAGENTA << request.getMethod() << RESET << std::endl;
	std::cout << MAGENTA << request.getHeader("Content-Type") << RESET << std::endl;
	std::cout << MAGENTA << to_string(request.getBody().length()) << RESET << std::endl;

	//Make environment variables for CGI execution
	std::string method_env = "REQUEST_METHOD=" + request.getMethod();
	std::string content_type = "CONTENT_TYPE= " + request.getHeader("Content-Type");
	std::string content_length = "CONTENT_LENGTH=" + to_string(request.getBody().length());
	char *envp[] = {
		const_cast<char *>(method_env.c_str()),
		const_cast<char *>(content_type.c_str()),
		const_cast<char *>(content_length.c_str()),
		NULL
	};
	std::cout << "CGI Starting!" << std::endl;
	return execute(request, envp);;
}