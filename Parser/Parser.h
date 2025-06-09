/*
 * Parser.h
 *
 *  Created on: Oct 14, 2011
 *      Author: Saurabh Saxena
 *      // UFID        : 21817195
 */

#include <string>
#include <vector>
#include <stack>
#include <cstdlib>
#include "Token.h"
#include "TreeNode.h"
#include "LexicalAnalyzer.h"

#ifndef PARSER_H_
#define PARSER_H_

using namespace std;

class Parser {
private:
    LexicalAnalyzer* lexer;
    stack<TreeNode*> stk;
    Token nextToken;
    bool tokensLeft;
    
    void formattedPrint(Token t, string dots);
    void preOrder(TreeNode* t, string dots);
    void read(Token token);
    void buildTree(Token token, int numOfNodes);
    void makeRightNode(TreeNode* t);
    
    // Grammar parsing methods
    void parseE();
    void parseEw();
    void parseT();
    void parseTa();
    void parseTc();
    void parseB();
    void parseBt();
    void parseBs();
    void parseBp();
    void parseA();
    void parseAt();
    void parseAf();
    void parseAp();
    void parseR();
    void parseRn();
    void parseD();
    void parseDa();
    void parseDr();
    void parseDb();
    void parseVb();
    void parseV1();
    void parseBpHelper(Token nextToken, string tokenValue);
    void parseRHelper(Token t, string value);

public:
    Parser(LexicalAnalyzer* analyzer);
    virtual ~Parser();
    
    void parse();
    TreeNode* getTree();
    void printTree();
    
    static string ID;
    static string STR;
    static string INT;
    static string KEY;
    static string OPT;
};

#endif /* PARSER_H_ */