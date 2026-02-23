# Compiler and flags
CXX := clang++
CXXFLAGS := -std=c++23 -Wall -Wextra -O2 -Iinclude
LDFLAGS := 

# Project directories
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
TARGET := todo

# Find all .cpp files recursively
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
# Map source files to object files in build/
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
# Map object files to dependency files
DEPS := $(OBJS:.o=.d)

# Default target
all: $(TARGET)

# Link executable
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compile .cpp to .o with dependency generation
# Uses -MMD -MP to generate .d files automatically
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Include dependency files if they exist
-include $(DEPS)

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean