# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11

# Add all folders that contain headers
INCLUDES = -ILexer -ITokens -INodes -IStandardizer -ICSEMachine -IParser

# Output binary name
TARGET = myrpal

# Source files
SRC = Interpreter.cpp \
      Lexer/LexicalAnalyzer.cpp \
      Tokens/Token.cpp \
      Nodes/TreeNode.cpp \
      Standardizer/Standardizer.cpp \
      CSEMachine/CSEMachineCore.cpp \
      CSEMachine/CSEMachineBuilder.cpp \
      Parser/Parser.cpp

# Build target
all:
	$(CXX) $(SRC) $(CXXFLAGS) $(INCLUDES) -o $(TARGET)

# Clean target
cl:
	rm -f *.o $(TARGET)
