CXX = g++
CXX_FLAGS = -Wall -Wextra -Werror -std=c++98

SRC_DIR = src
# Use find to locate all .cpp files in src and its subdirectories
SRC = $(shell find $(SRC_DIR) -type f -name '*.cpp')

OBJS_DIR = objs
# Replace src/ with objs/ and .cpp with .o for object files
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJS_DIR)/%.o, $(SRC))

# Target executable
TARGET = webserv

# Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXX_FLAGS) -o $(TARGET) $(OBJS)

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@) # Create subdirectories in objs/ as needed
	$(CXX) $(CXX_FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -rf $(TARGET)

re: fclean all

.PHONY: all clean fclean re