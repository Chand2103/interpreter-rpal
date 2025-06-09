/**
 * Recursive Descent Parser Header
 * This header defines the interface for a recursive descent parser that constructs
 * Abstract Syntax Trees (AST) for a functional programming language. The parser
 * processes tokens from a lexical analyzer and builds tree structures using
 * stack-based operations following defined grammar rules.
 */

#include <string>
#include <vector>
#include <stack>
#include <cstdlib>
#include "Token.h"
#include "TreeNode.h"
#include "Lexer.h"

#ifndef PARSER_H_
#define PARSER_H_

using namespace std;

class Parser {
private:
    Lexer* lexer;
    stack<TreeNode*> stk;
    Token nextToken;
    bool tokensLeft;
    
    /**
     * Utility functions for tree construction and display
     */
    void displayFormattedToken(Token t, string dots);
    void traversePreOrder(TreeNode* t, string dots);
    void consumeToken(Token token);
    void constructTreeNode(Token token, int numOfNodes);
    void attachRightChild(TreeNode* t);
    
    /**
     * Expression parsing functions - ordered by precedence
     */
    void processMainExpression();
    void processWhereExpression();
    void processTupleExpression();
    void processAugmentExpression();
    void processConditionalExpression();
    void processBooleanExpression();
    void processBooleanTerm();
    void processBooleanStatement();
    void processBooleanPrimary();
    void processArithmeticExpression();
    void processArithmeticTerm();
    void processArithmeticFactor();
    void processArithmeticPrimary();
    void processRationalExpression();
    void processRationalNode();
    
    /**
     * Declaration parsing functions
     */
    void processDeclaration();
    void processAndDeclaration();
    void processRecursiveDeclaration();
    void processBasicDeclaration();
    void processVariableBinding();
    void processVariableList();
    
    /**
     * Helper functions for specific parsing tasks
     */
    void processComparisonHelper(Token nextToken, string tokenValue);
    void processRationalHelper(Token t, string value);

public:
    Parser(Lexer* analyzer);
    virtual ~Parser();
    
    /**
     * Public interface functions
     */
    void parse();
    TreeNode* getTree();
    void printTree();
    
    /**
     * Static token type constants
     */
    static string ID;
    static string STR;
    static string INT;
    static string KEY;
    static string OPT;
};

#endif /* PARSER_H_ */