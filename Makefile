# toolchain compiler
CC    := gcc
CXX   := g++
LD    := $(CXX)

# directories
SRC_DIR   := src
INC_DIR   := include
OBJ_DIR	  := obj

# Executable binary
TARGET	  := system_monitor

# Flags
#
# -Wall -Wextra -> enable all compilation warning
#  -std -> language standards
#  -g: generate gnu debug inforation
#  -O2: optimization level
#  -I: local project directory where to find the header files
CFLAGS    := -Wall -Wextra -std=c11 -g -O2 -I $(INC_DIR)
CXXFLAGS  := -Wall -Wextra -std=c++17 -g -O2 -I $(INC_DIR)
# thread programming
LDFLAGS   := -lpthread


# All c/cpp source files
C_SRCS    := $(wildcard $(SRC_DIR)/*.c)
CXX_SRCS  := $(wildcard $(SRC_DIR)/*.cpp)

# Source file -> Object files
# patsubst(3 arguments): pattern, replacement, texts
C_OBJS    := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SRCS))
CXX_OBJS  := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CXX_SRCS))
ALL_OBJS  := $(C_OBJS) $(CXX_OBJS)

.PHONY: all
all: $(TARGET)

# dependancies
# link
# $^ : all dependancies (*.o)
# $@ : target executable binary
$(TARGET): $(ALL_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@
	@echo ">>> Built: $@"

# Compile .c to .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile .cpp to .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

.PHONY: run
run: all
	./$(TARGET)

# Generate .d automatocally - after header file changed, compile it automatically
# -MMD -MP: generate .d, MP: avoid raising errors after header file is deleted
DEPFLAGS = -MMD -MP
-include $(ALL_OBJS:.o=.d)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(TARGET) *.log
	@echo ">>> Cleaned"


# show makefile variable values
.PHONY: info
info:
	@echo "C_SRCS   = $(C_SRCS)"
	@echo "CXX_SRCS = $(CXX_SRCS)"
	@echo "ALL_OBJS = $(ALL_OBJS)"
	@echo "TARGET   = $(TARGET)"

.PHONY: check
check:
	cppcheck --enable=all --std=c11   $(C_SRCS)
	cppcheck --enable=all --std=c++17 $(CXX_SRCS)
