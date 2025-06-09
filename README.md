# RPAL Interpreter

This is an interpreter for the RPAL (Right-reference Programming Algorithmic Language), implemented in C++. It reads source code from input files, parses it into an Abstract Syntax Tree (AST), standardizes the AST into a Standardized Tree (ST), and evaluates the resulting expression.

## Build Instructions

To build the interpreter, ensure you have `make` installed. Then run:

```bash
make

./myrpal <filename>

./myrpal -st <filename>

./myrpal -ast <filename>

./myrpal -st -ast <filename>

./myrpal -ast -st <filename>

./myrpal t1.txt

./myrpal -ast t1.txt

./myrpal -st -ast t1.txt

