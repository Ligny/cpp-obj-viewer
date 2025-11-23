CXX     = g++
SFML_PREFIX ?= /opt/homebrew/opt/sfml

CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -Iinclude -I$(SFML_PREFIX)/include
LDFLAGS  = -L$(SFML_PREFIX)/lib -lsfml-graphics -lsfml-window -lsfml-system

SRC_DIR = src
OBJ_DIR = obj

SRC = $(SRC_DIR)/main.cpp \
      $(SRC_DIR)/Math.cpp \
      $(SRC_DIR)/Model.cpp \
      $(SRC_DIR)/Renderer.cpp \
      $(SRC_DIR)/App.cpp

OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

NAME = viewer

all: $(NAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)
	rm -rf $(OBJ_DIR)

re: fclean all

.PHONY: all clean fclean re
