/**
 * Recursive Descent Parser Implementation
 * This code implements a recursive descent parser for a functional programming language.
 * It processes tokens from a lexical analyzer and constructs an Abstract Syntax Tree (AST)
 * using a stack-based approach, following grammar rules for expressions, declarations,
 * and various language constructs like let-expressions, functions, and conditionals.
 */

#include <iostream>
#include "Parser.h"
#include "Token.h"
#include <string>
#include <cstdlib>

using namespace std;

string Parser::ID = "IDENTIFIER";
string Parser::STR = "STRING";
string Parser::INT = "INTEGER";
string Parser::KEY = "KEYWORD";
string Parser::OPT = "OPERATOR";

Parser::Parser(Lexer* analyzer) {
    this->lexer = analyzer;
    this->tokensLeft = true;
}

Parser::~Parser() {
}

/**
 * Main parsing entry point - initializes token stream and begins AST construction
 * Starts the parsing process by getting the first token and invoking expression parsing
 */
void Parser::parse() {
    nextToken = lexer->getNextToken();
    processMainExpression();
}

/**
 * Token matching and consumption utility - validates expected tokens
 * Reads the current token, builds tree nodes for terminals, and advances to next token
 */
void Parser::consumeToken(Token token) {
    if(token.value != nextToken.value)
        exit(1);

    switch(token.type[0]) {
        case 'I': // INTEGER or IDENTIFIER
            if(token.type == "INTEGER" || token.type == "IDENTIFIER")
                constructTreeNode(token, 0);
            break;
        case 'S': // STRING
            if(token.type == "STRING")
                constructTreeNode(token, 0);
            break;
        default:
            break;
    }
    
    nextToken = lexer->getNextToken();
    
    if(nextToken.value == "END" || tokensLeft == false)
        throw "All tokens parsed";
}

/**
 * AST node construction utility - creates tree nodes and manages stack operations
 * Builds tree nodes with specified number of children popped from the stack
 */
void Parser::constructTreeNode(Token tokenVal, int popTreeCnt) {
    TreeNode* tempNode = new TreeNode;
    tempNode->value = tokenVal;
    
    if(popTreeCnt != 0) {
        while(!stk.empty() && popTreeCnt > 1) {
            TreeNode* curr = stk.top();
            stk.pop();
            attachRightChild(curr);
            popTreeCnt--;
        }
        TreeNode* top = stk.top();
        stk.pop();
        if(top != NULL)
            tempNode->left = top;
    }
    stk.push(tempNode);
    return;
}

/**
 * Tree structure utility - attaches nodes as right children
 * Manages the right child attachment during tree construction
 */
void Parser::attachRightChild(TreeNode* node) {
    TreeNode* parentNode = stk.top();
    stk.pop();
    parentNode->right = node;
    stk.push(parentNode);
}

/**
 * Primary expression parser - handles let-expressions, functions, and where-expressions
 * Processes the main expression types in the grammar
 */
void Parser::processMainExpression() {
    if(nextToken.value == "let") {
        Token letToken("let", KEY);
        consumeToken(letToken);
        processDeclaration();
        Token inToken("in", KEY);
        consumeToken(inToken);
        processMainExpression();
        Token nodeToken("let", "let");
        constructTreeNode(nodeToken, 2);
    } else if(nextToken.value == "fn") {
        Token fnToken("fn", "fn");
        consumeToken(fnToken);
        int n = 0;
        do {
            processVariableBinding();
            n++;
        } while(nextToken.type == ID || nextToken.type == "(");
        Token dotToken(".", OPT);
        consumeToken(dotToken);
        processMainExpression();
        Token nodeToken("lambda", "lambda");
        constructTreeNode(nodeToken, n + 1);
    } else {
        processWhereExpression();
    }
}

/**
 * Where-expression parser - handles expressions with where clauses
 * Processes tuple expressions and optional where clauses
 */
void Parser::processWhereExpression() {
    processTupleExpression();
    if(nextToken.value == "where") {
        Token t("where", KEY);
        consumeToken(t);
        processRecursiveDeclaration();
        Token nodeToken("where", "where");
        constructTreeNode(nodeToken, 2);
    }
}

/**
 * Tuple expression parser - handles comma-separated expressions
 * Processes expressions that can form tuples with tau operator
 */
void Parser::processTupleExpression() {
    processAugmentExpression();
    if(nextToken.value == ",") {
        int n = 0;
        do {
            consumeToken(nextToken);
            processAugmentExpression();
            n++;
        } while(nextToken.value == ",");
        Token tauToken("tau", "tau");
        tauToken.tauCount = n + 1;
        constructTreeNode(tauToken, n + 1);
    }
}

/**
 * Augment expression parser - handles augmentation operations
 * Processes expressions with augment operator (aug)
 */
void Parser::processAugmentExpression() {
    processConditionalExpression();
    while(nextToken.value == "aug") {
        Token temp = nextToken;
        consumeToken(nextToken);
        processConditionalExpression();
        constructTreeNode(temp, 2);
    }
}

/**
 * Conditional expression parser - handles conditional statements
 * Processes if-then-else style conditional expressions
 */
void Parser::processConditionalExpression() {
    processBooleanExpression();
    if(nextToken.value == "->") {
        consumeToken(nextToken);
        processConditionalExpression();
        Token elseToken("|", OPT);
        consumeToken(elseToken);
        processConditionalExpression();
        Token nodeToken("->", "->");
        constructTreeNode(nodeToken, 3);
    }
}

/**
 * Boolean expression parser - handles logical OR operations
 * Processes boolean expressions with OR operator
 */
void Parser::processBooleanExpression() {
    processBooleanTerm();
    while(nextToken.value == "or") {
        nextToken.type = Parser::OPT;
        Token temp = nextToken;
        consumeToken(nextToken);
        processBooleanTerm();
        constructTreeNode(temp, 2);
    }
}

/**
 * Boolean term parser - handles logical AND operations
 * Processes boolean terms with AND operator
 */
void Parser::processBooleanTerm() {
    processBooleanStatement();
    while(nextToken.value == "&") {
        nextToken.type = Parser::OPT;
        Token temp = nextToken;
        consumeToken(nextToken);
        processBooleanStatement();
        constructTreeNode(temp, 2);
    }
}

/**
 * Boolean statement parser - handles NOT operations
 * Processes boolean statements with NOT operator
 */
void Parser::processBooleanStatement() {
    if(nextToken.value == "not") {
        nextToken.type = "not";
        Token temp = nextToken;
        consumeToken(nextToken);
        processBooleanPrimary();
        constructTreeNode(temp, 1);
    } else {
        processBooleanPrimary();
    }
}

/**
 * Boolean primary parser - handles comparison operations
 * Processes comparison operators like greater than, less than, equal, etc.
 */
void Parser::processBooleanPrimary() {
    processArithmeticExpression();
    
    switch(nextToken.value[0]) {
        case 'g':
            if(nextToken.value == "gr" || nextToken.value == ">=") {
                nextToken.type = Parser::OPT;
                processComparisonHelper(nextToken, nextToken.value == "gr" ? "gr" : "ge");
            }
            break;
        case 'l':
            if(nextToken.value == "ls" || nextToken.value == "<=") {
                nextToken.type = Parser::OPT;
                processComparisonHelper(nextToken, nextToken.value == "ls" ? "ls" : "le");
            }
            break;
        case 'e':
            if(nextToken.value == "eq") {
                nextToken.type = Parser::OPT;
                processComparisonHelper(nextToken, "eq");
            }
            break;
        case 'n':
            if(nextToken.value == "ne") {
                nextToken.type = Parser::OPT;
                processComparisonHelper(nextToken, "ne");
            }
            break;
        case '>':
            nextToken.type = Parser::OPT;
            processComparisonHelper(nextToken, "gr");
            break;
        case '<':
            nextToken.type = Parser::OPT;
            processComparisonHelper(nextToken, "ls");
            break;
        default:
            break;
    }
}

/**
 * Comparison helper utility - processes comparison operations
 * Helper function for handling various comparison operators
 */
void Parser::processComparisonHelper(Token nextToken, string tokenValue) {
    consumeToken(nextToken);
    processArithmeticExpression();
    Token t(tokenValue, Parser::OPT);
    constructTreeNode(t, 2);
}

/**
 * Arithmetic expression parser - handles addition and subtraction
 * Processes arithmetic expressions with + and - operators
 */
void Parser::processArithmeticExpression() {
    switch(nextToken.value[0]) {
        case '-':
            {
                Token negToken("-", OPT);
                consumeToken(negToken);
                processArithmeticTerm();
                Token nodeToken("neg", "neg");
                constructTreeNode(nodeToken, 1);
            }
            break;
        case '+':
            {
                Token posToken("+", OPT);
                consumeToken(posToken);
                processArithmeticTerm();
            }
            break;
        default:
            processArithmeticTerm();
            break;
    }
    
    while(nextToken.value == "+" || nextToken.value == "-") {
        Token temp = nextToken;
        consumeToken(nextToken);
        processArithmeticTerm();
        constructTreeNode(temp, 2);
    }
}

/**
 * Arithmetic term parser - handles multiplication and division
 * Processes arithmetic terms with * and / operators
 */
void Parser::processArithmeticTerm() {
    processArithmeticFactor();
    while(nextToken.value == "*" || nextToken.value == "/") {
        Token temp = nextToken;
        consumeToken(nextToken);
        processArithmeticFactor();
        constructTreeNode(temp, 2);
    }
}

/**
 * Arithmetic factor parser - handles exponentiation
 * Processes arithmetic factors with ** operator
 */
void Parser::processArithmeticFactor() {
    processArithmeticPrimary();
    while(nextToken.value == "**") {
        Token temp = nextToken;
        consumeToken(nextToken);
        processArithmeticFactor();
        constructTreeNode(temp, 2);
    }
}

/**
 * Arithmetic primary parser - handles @ operator
 * Processes arithmetic primaries with @ operator
 */
void Parser::processArithmeticPrimary() {
    processRationalExpression();
    while(nextToken.value == "@") {
        Token temp = nextToken;
        consumeToken(nextToken);
        if(nextToken.type != ID)
            throw "Expected Identifier found in processArithmeticPrimary()";
        consumeToken(nextToken);
        processRationalExpression();
        constructTreeNode(temp, 3);
    }
}

/**
 * Rational expression parser - handles function applications
 * Processes function applications using gamma operator
 */
void Parser::processRationalExpression() {
    processRationalNode();
    while(nextToken.type == ID || nextToken.type == STR || nextToken.type == INT ||
          nextToken.value == "true" || nextToken.value == "false" || nextToken.value == "nil" ||
          nextToken.value == "(" || nextToken.value == "dummy") {
        processRationalNode();
        Token nodeToken("gamma", "gamma");
        constructTreeNode(nodeToken, 2);
    }
}

/**
 * Rational node parser - handles basic expressions and literals
 * Processes identifiers, strings, integers, booleans, nil, dummy, and parenthesized expressions
 */
void Parser::processRationalNode() {
    if(nextToken.type == ID || nextToken.type == STR || nextToken.type == INT) {
        consumeToken(nextToken);
    } else {
        switch(nextToken.value[0]) {
            case 't':
                if(nextToken.value == "true") {
                    processRationalHelper(nextToken, "true");
                }
                break;
            case 'f':
                if(nextToken.value == "false") {
                    processRationalHelper(nextToken, "false");
                }
                break;
            case 'n':
                if(nextToken.value == "nil") {
                    processRationalHelper(nextToken, "nil");
                }
                break;
            case 'd':
                if(nextToken.value == "dummy") {
                    processRationalHelper(nextToken, "dummy");
                }
                break;
            case '(':
            {
                consumeToken(nextToken);
                processMainExpression();
                Token t(")", ")");
                consumeToken(t);
                break;
            }
            default:
                break;
        }
    }
}

/**
 * Rational helper utility - processes literal values
 * Helper function for handling boolean, nil, and dummy literals
 */
void Parser::processRationalHelper(Token t, string value) {
    consumeToken(t);
    Token nodeToken(value, value);
    constructTreeNode(nodeToken, 0);
}

/**
 * Declaration parser - handles within declarations
 * Processes declarations with optional within clauses
 */
void Parser::processDeclaration() {
    processAndDeclaration();
    if(nextToken.value == "within") {
        consumeToken(nextToken);
        processDeclaration();
        Token nodeToken("within", "within");
        constructTreeNode(nodeToken, 2);
    }
}

/**
 * And declaration parser - handles multiple declarations
 * Processes multiple declarations connected by 'and'
 */
void Parser::processAndDeclaration() {
    processRecursiveDeclaration();
    if(nextToken.value == "and") {
        int n = 1;
        Token temp = nextToken;
        while(nextToken.value == "and") {
            consumeToken(nextToken);
            processRecursiveDeclaration();
            n++;
        }
        constructTreeNode(temp, n);
    }
}

/**
 * Recursive declaration parser - handles recursive declarations
 * Processes recursive declarations with 'rec' keyword
 */
void Parser::processRecursiveDeclaration() {
    if(nextToken.value == "rec") {
        Token temp = nextToken;
        consumeToken(nextToken);
        processBasicDeclaration();
        constructTreeNode(temp, 1);
    } else {
        processBasicDeclaration();
    }
}

/**
 * Basic declaration parser - handles variable and function declarations
 * Processes basic declarations including assignments and function definitions
 */
void Parser::processBasicDeclaration() {
    if(nextToken.value == "(") {
        consumeToken(nextToken);
        processDeclaration();
        Token t(")", OPT);
        consumeToken(t);
    } else if(nextToken.type == ID && (lexer->peekNextToken().value == "," || lexer->peekNextToken().value == "=")) {
        processVariableList();
        Token t("=", OPT);
        consumeToken(t);
        processMainExpression();
        Token nodeToken("=", "=");
        constructTreeNode(nodeToken, 2);
    } else {
        consumeToken(nextToken);
        int n = 1;
        processVariableBinding();
        while(nextToken.type == ID || nextToken.value == "(") {
            n++;
            processVariableBinding();
        }
        Token t("=", OPT);
        consumeToken(t);
        processMainExpression();
        Token nodeToken("function_form", "function_form");
        constructTreeNode(nodeToken, n + 2);
    }
}

/**
 * Variable binding parser - handles variable bindings
 * Processes variable bindings including identifiers and parenthesized variable lists
 */
void Parser::processVariableBinding() {
    if(nextToken.type == ID) {
        consumeToken(nextToken);
    } else if(nextToken.value == "(") {
        consumeToken(nextToken);
        if(nextToken.value == ")") {
            consumeToken(nextToken);
            Token nodeToken("()", "()");
            constructTreeNode(nodeToken, 0);
        } else {
            processVariableList();
            Token t(")", ")");
            consumeToken(t);
        }
    }
}

/**
 * Variable list parser - handles comma-separated variable lists
 * Processes lists of variables separated by commas
 */
void Parser::processVariableList() {
    consumeToken(nextToken);
    int n = 1;
    if(nextToken.value == ",") {
        while(nextToken.value == ",") {
            n++;
            consumeToken(nextToken);
            consumeToken(nextToken);
        }
        Token nodeToken(",", ",");
        constructTreeNode(nodeToken, n);
    }
}

/**
 * Tree visualization utility - prints the AST structure
 * Prints the constructed AST in a formatted manner
 */
void Parser::printTree() {
    TreeNode* t = stk.top();
    stk.pop();
    stk.push(t);
    traversePreOrder(t, std::string(""));
}

/**
 * Tree traversal utility - performs pre-order traversal
 * Traverses the AST in pre-order for printing
 */
void Parser::traversePreOrder(TreeNode* t, std::string dots) {
    displayFormattedToken(t->value, dots);
    string dots1 = "." + dots;
    if(t->left != NULL)
        traversePreOrder(t->left, dots1);
    if(t->right != NULL)
        traversePreOrder(t->right, dots);
}

/**
 * Token formatting utility - formats tokens for display
 * Formats different token types for tree visualization
 */
void Parser::displayFormattedToken(Token t, std::string dots) {
    switch(t.type[0]) {
        case 'I':
            break;
        case 'S':
            break;
        default:
            break;
    }
}

/**
 * Tree accessor utility - returns the constructed AST
 * Returns the root of the constructed Abstract Syntax Tree
 */
TreeNode* Parser::getTree() {
    TreeNode* s = stk.top();
    stk.pop();
    return s;
}