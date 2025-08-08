#include "../../include/webserv.hpp"

void autoindex(Response &response, Request &request, std::string dir_path) {
    std::vector<std::string> files;
    std::string html;

    DIR* dir = opendir(dir_path.c_str());
    if (dir == NULL) {
        std::cerr << "Cannot Open Directory: " << dir_path << std::endl;
        return ;
    }
    html = "<!DOCTYPE html>\n<html>\n";
    html.append("<head>\n<title>" + request.getUri() + "</title>\n</head>\n");
    html.append("<body>\n");
    html.append("<h1>" + request.getUri() + "</h1>\n");
    html.append("<ul>");
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        files.push_back(entry->d_name);
    }
    std::sort(files.begin(), files.end());
    for (size_t i = 0; i < files.size(); ++i) {
        if (files[i] == ".") {
            continue; // Skip current directory
        }
        html.append("<li><a href=\"" + request.getUri() + "/" + files[i] + "\">" + files[i] + "</a></li>\n");
    }
    html.append("</ul>\n");
    html.append("</body>\n");
    html.append("</html>\n");
    std::cout << html << std::endl;
    response.setVersion(request.getVersion());
    response.setBody(html);
    response.setStatusCode(200);
    response.setStatusMessage("OK");
    response.addHeader("Connection", "keep-alive");
    response.addHeader("Content-Length", to_string(response.getBody().length()));
    response.addHeader("Content-Type", "text/html"); //**Might not be 100% correct */
    response.addHeader("Date", get_current_time());
    response.addHeader("Server", "Webserv/1.0"); // ** TEMPORARY, wait until config file is implemented **
    closedir(dir);
}