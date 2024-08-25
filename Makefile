# Variables
CXX = g++ -O3 -fopenmp
CXXFLAGS = -std=c++14 -O3 -Wall -Wextra
LDFLAGS = -lglfw -lGL -lGLU -lglut -ldl -lX11 -pthread
SRC = main.cpp utils.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = main
# Default target
all: $(TARGET)

# Compile target
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

# Object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean run