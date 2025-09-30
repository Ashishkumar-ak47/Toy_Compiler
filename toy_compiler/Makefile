
CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall
SRC = src/main.cpp
BIN = toy_compiler
all: $(BIN)
$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)
clean:
	rm -f $(BIN)
