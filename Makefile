CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

SRC = src/main.cpp src/lexer.cpp src/parser.cpp src/symboltable.cpp src/semantic.cpp src/codegen.cpp src/codegen_wasm.cpp src/error.cpp src/ast.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = compiler.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /f /q src\*.o $(TARGET)