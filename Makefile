CXX = g++
CXXFLAGS = -std=c++20 -O3 -Wall -Wextra -Iinclude

SRC = src/main.cpp src/core/vfs.cpp src/shell/shell.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = purity-fs

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)
