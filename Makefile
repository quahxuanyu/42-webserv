SRC = multiplexing.cpp src/Client.cpp src/Server.cpp src/Request.cpp src/Response.cpp
OBJ = $(SRC:.cpp=.o)

CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98
TARGET = server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re