RPAL Interpreter
This is an interpreter for RPAL (Right-reference Pure Applicative Language). It processes .txt files containing RPAL source code and can output results in several modes, including full evaluation, abstract syntax tree (AST), and standardized tree (ST).

Features
Parses and evaluates RPAL programs.

Generates:

Abstract Syntax Tree (AST)

Standardized Tree (ST)

Supports combined output modes (AST followed by ST).

Build Instructions
To compile the project, simply run:

make

This will build the myrpal executable.

Usage
Once built, the interpreter can be run from the command line with the following options:

1. Evaluate the RPAL program
./myrpal t1.txt

Outputs the evaluation result of the RPAL program in t1.txt.

2. Output the Standardized Tree only
./myrpal -st t1.txt

Prints only the standardized version of the parse tree.

3. Output the Abstract Syntax Tree only
./myrpal -ast t1.txt

Prints only the abstract syntax tree.

4. Output both AST and ST
./myrpal -ast -st t1.txt
or
./myrpal -st -ast t1.txt

Prints the abstract syntax tree first, followed by the standardized tree.

Example
./myrpal -ast -st examples/factorial.txt

Notes
Ensure input files are formatted according to RPAL syntax.

No external dependencies are required beyond a standard C++ compiler and make.
