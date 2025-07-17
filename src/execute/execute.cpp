#include "../../include/webserv.hpp"

void execute(std::string uri, char **env) {
    // std::string cgi_path = "cgi-bin/" + uri;
    std::string cgi_path = uri;
    char *argv[] = {const_cast<char*>(cgi_path.c_str()), NULL};
    if (fork() == 0)
    {
        if (execve(cgi_path.c_str(), argv, env) == -1)
        {
            std::cerr << "Error: cannot execute " << cgi_path << std::endl;
        }
    }
    else
    {
        waitpid(-1, NULL, WUNTRACED);
        std::cout << "CGI executed: " << cgi_path << std::endl;
    }
}