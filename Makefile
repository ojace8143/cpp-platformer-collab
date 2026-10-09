# ?= lets you override from the terminal
CXX      ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic
LDFLAGS  ?=
LDLIBS   ?= -lsfml-graphics -lsfml-window -lsfml-system

MODE  ?= release

ifeq ($(MODE),debug)
  CXXFLAGS += -O0 -g3
else
  CXXFLAGS += -O2
endif

# directories and targets
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
TARGET  := $(BIN_DIR)/platformer

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:%.o=%.d)

RM := rm -rf

.PHONY: all clean help run
.DELETE_ON_ERROR:   # delete a target if its recipe fails

all: $(TARGET)

# link step
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# compile step
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# pull in the auto-generated dependency files
-include $(DEPS)

# safety
guard-clean:
	@[ -n "$(OBJ_DIR)" ] && [ "$(OBJ_DIR)" != / ] \
		&& [ -n "$(BIN_DIR)" ] && [ "$(BIN_DIR)" != / ] \
		|| { echo "Refusing to clean unsafe path" >&2; exit 1; }
.PHONY: guard-clean

clean: guard-clean
	$(RM) $(BIN_DIR)/* $(OBJ_DIR)/*

run: all
	./$(TARGET)

help:
	@echo "Targets:"
	@echo "  all          Build the game (default)"
	@echo "  run          Build and launch it"
	@echo "  clean        Wipe bin/ and obj/ contents"
	@echo "  help         Show this message"
	@echo ""
	@echo "Variables:"
	@echo "  MODE=debug   Build with -O0 -g3 (symbols)"
	@echo "  MODE=release Build with -O2 (default)"
	@echo "  CXX=clang++  Override the compiler"
