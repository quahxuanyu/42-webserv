#include "../include/webserv.hpp"

// Converts an integer to a string
std::string to_string(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}