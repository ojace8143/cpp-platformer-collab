# Compiler and Flags
CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -O2

# SFML Linker Modules
LIBS     := -lsfml-graphics -lsfml-window -lsfml-system

# Directories
SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := bin

# Executable output name
TARGET   := $(BIN_DIR)/platformer

# Source and Object matching
SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(TARGET)

# Link step
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

# Compilation step
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
