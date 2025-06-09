#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdexcept>
#include <exception>

#include "Lexer.h"
#include "Standardizer.h"
#include "TreeNode.h"
#include "CSEMachine.h"
#include "Parser.h"

using namespace std;

void preOrder(TreeNode* t, std::string dots);
void formattedPrint(Token t,std::string dots);

// Enhanced file opening with comprehensive error handling
string openFile(char* fileName){
	if (fileName == nullptr || strlen(fileName) == 0) {
		cerr << "Error: Null or empty filename provided" << endl;
		return "";
	}

	string file_content;
	string next_line;
	ifstream input_file;

	try {
		input_file.open(fileName);

		if(input_file.fail()) {
			cerr << "Error: Problem opening input file '" << fileName << "'" << endl;
			cerr << "Please check if the file exists and you have read permissions." << endl;
			return "";
		}

		if(input_file.is_open()){
			while(input_file.good()){
				getline(input_file,next_line);
				file_content +=  next_line + "\n";
			}
		} else {
			cerr << "Error: Failed to open file '" << fileName << "'" << endl;
			return "";
		}

		input_file.close();

		if (file_content.empty()) {
			cerr << "Warning: File '" << fileName << "' is empty" << endl;
		}

	} catch (const ios_base::failure& e) {
		cerr << "Error: I/O exception while reading file '" << fileName << "': " << e.what() << endl;
		if (input_file.is_open()) {
			input_file.close();
		}
		return "";
	} catch (const exception& e) {
		cerr << "Error: Exception while reading file '" << fileName << "': " << e.what() << endl;
		if (input_file.is_open()) {
			input_file.close();
		}
		return "";
	}

	return file_content;
}

// Safe parsing with error handling
bool safeParseAndProcess(const string& code_string, bool ast_switch, bool st_switch, bool evaluate_only) {
	try {
		// Lexical Analysis Phase
		Lexer* lexer = new Lexer(code_string);
		if (!lexer) {
			cerr << "Error: Failed to create lexer" << endl;
			return false;
		}

		// Parsing Phase
		Parser* parser = new Parser(lexer);
		if (!parser) {
			cerr << "Error: Failed to create parser" << endl;
			delete lexer;
			return false;
		}

		try {
			parser->parse();
		} catch (const exception& e) {
			cerr << "Error: Parsing failed - " << e.what() << endl;
			cerr << "Please check your program syntax." << endl;
			delete parser;
			delete lexer;
			return false;
		}

		TreeNode* root = nullptr;
		try {
			root = parser->getTree();
			if (!root) {
				cerr << "Error: Parser returned null tree" << endl;
				delete parser;
				delete lexer;
				return false;
			}
		} catch (const exception& e) {
			cerr << "Error: Failed to get parse tree - " << e.what() << endl;
			delete parser;
			delete lexer;
			return false;
		}

		// AST Display (if requested)
		if (ast_switch) {
			try {
				cout << "Abstract Syntax Tree:" << endl;
				preOrder(root, "");
				cout << endl;
			} catch (const exception& e) {
				cerr << "Error: Failed to display AST - " << e.what() << endl;
			}
		}

		// Standardization Phase
		TreeNode* transformedRoot = nullptr;
		try {
			TreeStandardizer transformer;
			transformedRoot = transformer.standardizeTree(root);

			if (!transformedRoot) {
				cerr << "Error: Tree standardization failed" << endl;
				delete parser;
				delete lexer;
				return false;
			}
		} catch (const std::exception& e) {
			cerr << "Error: Standardization failed - " << e.what() << endl;
			delete parser;
			delete lexer;
			return false;
		}

		// Standardized Tree Display (if requested)
		if (st_switch) {
			try {
				cout << "Standardized Tree:" << endl;
				preOrder(transformedRoot, "");
				cout << endl;
			} catch (const exception& e) {
				cerr << "Error: Failed to display standardized tree - " << e.what() << endl;
			}
		}

		// Evaluation Phase
		if (evaluate_only || (!ast_switch && !st_switch)) {
			try {
				CSEMachine* machine = new CSEMachine(transformedRoot);
				if (!machine) {
					cerr << "Error: Failed to create CSE machine" << endl;
					delete parser;
					delete lexer;
					return false;
				}

				machine->evaluateTree();
				delete machine;
			} catch (const exception& e) {
				cerr << "Error: Evaluation failed - " << e.what() << endl;
				cerr << "This could be due to runtime errors in your program." << endl;
				delete parser;
				delete lexer;
				return false;
			}
		}

		delete parser;
		delete lexer;
		return true;

	} catch (const bad_alloc& e) {
		cerr << "Error: Memory allocation failed - " << e.what() << endl;
		cerr << "The program may be too large or system is out of memory." << endl;
		return false;
	} catch (const exception& e) {
		cerr << "Error: Unexpected exception during processing - " << e.what() << endl;
		return false;
	} catch (...) {
		cerr << "Error: Unknown exception occurred during processing" << endl;
		return false;
	}
}

int main(int argc,char *argv[]) {
	try {
		char* file_name = nullptr;
		bool ast_switch = false;
		bool st_switch = false;

		if(argc == 4){
			if(((string) argv[1]).compare("-st") == 0)
				st_switch = true;
			if(((string) argv[2]).compare("-ast") == 0)
				ast_switch = true;
			if(((string) argv[2]).compare("-st") == 0)
				st_switch = true;
			if(((string) argv[1]).compare("-ast") == 0)
				ast_switch = true;
			file_name = argv[3];
		}
		else if(argc == 3){
			if(((string) argv[1]).compare("-ast") == 0)
				ast_switch = true;
			if(((string) argv[1]).compare("-st") == 0)
				st_switch = true;
			file_name = argv[2];
		}
		else if(argc == 2){
			file_name = argv[1];
		}
		else {
			cerr << "Usage: " << argv[0] << " [-ast] [-st] <filename>" << endl;
			cerr << "  -ast: Display Abstract Syntax Tree" << endl;
			cerr << "  -st:  Display Standardized Tree" << endl;
			return 1;
		}

		if (!file_name || strlen(file_name) == 0) {
			cerr << "Error: No filename provided or filename is empty" << endl;
			return 1;
		}

		string code_string;
		try {
			code_string = openFile(file_name);
		} catch (const exception& e) {
			cerr << "Error: Exception while opening file - " << e.what() << endl;
			return 1;
		}

		if(code_string.size() == 0) {
			cerr << "Error: File is empty or could not be read" << endl;
			return 1;
		}

		bool success = false;
		if (argc == 2) {
			success = safeParseAndProcess(code_string, false, false, true);
		} else {
			success = safeParseAndProcess(code_string, ast_switch, st_switch, false);
		}

		if (!success) {
			cerr << "Program execution failed. Please check your input file and try again." << endl;
			return 1;
		}

		return 0;

	} catch (const exception& e) {
		cerr << "Fatal Error: " << e.what() << endl;
		return 1;
	} catch (...) {
		cerr << "Fatal Error: Unknown exception in main" << endl;
		return 1;
	}
}

void preOrder(TreeNode* t, std::string dots){
	if (t == nullptr) {
		cout << dots << "[NULL]" << endl;
		return;
	}

	try {
		formattedPrint(t->value, dots);
		string dots1 = "." + dots;

		if(t->left != nullptr) {
			try {
				preOrder(t->left, dots1);
			} catch (...) {
				cerr << "Error in left subtree traversal" << endl;
			}
		}
		if(t->right != nullptr) {
			try {
				preOrder(t->right, dots);
			} catch (...) {
				cerr << "Error in right subtree traversal" << endl;
			}
		}
	} catch (const exception& e) {
		cerr << "Error in preOrder traversal: " << e.what() << endl;
	}
}

void formattedPrint(Token t, std::string dots){
	try {
		if(t.type == "IDENTIFIER"){
			cout << dots << "<ID:" << t.value << '>' << endl;
		} else if(t.type == "INTEGER"){
			cout << dots << "<INT:" << t.value << '>' << endl;
		} else if(t.type == "STRING"){
			cout << dots << "<STR:" << t.value << '>' << endl;
		} else if(t.value == "true" || t.value == "false" || t.value == "nil" || t.value == "dummy"){
			cout << dots << '<' << t.value << '>' << endl;
		} else if(t.value == "YSTAR"){
			cout << dots << "<Y*>" << endl;
		} else {
			cout << dots << t.value << endl;
		}
	} catch (const exception& e) {
		cerr << "Error in formattedPrint: " << e.what() << endl;
		cout << dots << "[ERROR_PRINTING_TOKEN]" << endl;
	}
}
