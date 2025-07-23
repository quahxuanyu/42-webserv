// #include "../../include/Cgi.hpp"

// Cgi::Cgi() : _request(*(new Request())), _response(new Response()), _envp(NULL) {`

// }

// Cgi::Cgi(Request &req) : _request(req), _response (new Response()), _envp(NULL) {

// }

// Cgi::~Cgi() {
// 	delete _response;
// }

// Response &Cgi::execute() {
// 	int pipefd[2];
// 	std::string cgi_output;

// 	pipe(pipefd); // pipefd[0] = read, pipefd[1] = write
// 	std::string cgi_path = _request.getUri();
// 	char *argv[] = {const_cast<char *>(cgi_path.c_str()), NULL};
// 	if (fork() == 0)
// 	{
// 		dup2(pipefd[0], STDIN_FILENO);
// 		dup2(pipefd[1], STDOUT_FILENO);
// 		close(pipefd[0]);
// 		close(pipefd[1]);
// 		if (execve(cgi_path.c_str(), argv, this->_envp) == -1)
// 			std::cerr << "ERROR: cannot execute " << cgi_path << std::endl;
// 	}
// 	else
// 	{
// 		write(pipefd[1], _request.getBody().c_str(), _request.getBody().length());
// 		close(pipefd[1]);
// 		char buffer[1024];

// 		ssize_t bytes_read;
// 		waitpid(-1, NULL, WUNTRACED);

// 		while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0)
// 		{
// 			buffer[bytes_read] = '\0'; // Null-terminate
// 			cgi_output += buffer;
// 		}
// 	}
// 	close(pipefd[0]);
// 	return cgi_output;
// }