CXX = g++
CXX_FLAGS = -Wall -Wextra -Werror -std=c++98

SRC_DIR = src
SRC_FILES = main.cpp Request.cpp Response.cpp
SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS_DIR = objs
OBJS = $(addprefix $(OBJS_DIR)/, $(notdir $(SRC:.cpp=.o)))

# Target executable
TARGET = webserv

# Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXX_FLAGS) -o $(TARGET) $(OBJS)

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJS_DIR)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -rf $(TARGET)

re: fclean all

.PHONY: all clean fclean re