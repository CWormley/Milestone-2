# Makefile for C++ project

# Compiler
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2

# Source and object files
SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)

# Output executable name
TARGET := m2

# Default rule
all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

