/*
 * Parser.cpp
 *
 *  Created on: Oct 14, 2011
 *      Author: Saurabh Saxena
 *      // UFID        : 21817195
 */

#include <iostream>
#include "Parser.h"
#include "Token.h"
#include <string>
#include <cstdlib>

using namespace std;

Parser::Parser(LexicalAnalyzer* analyzer) {
    this->lexer = analyzer;
    this->tokensLeft = true;
}

Parser::~Parser() {
}

// Main method for parsing and AST creation
void Parser::parse() {
    nextToken = lexer->getNextToken();
    parseE();
}

string Parser::ID = "IDENTIFIER";
string Parser::STR = "STRING";
string Parser::INT = "INTEGER";
string Parser::KEY = "KEYWORD";
string Parser::OPT = "OPERATOR";

void Parser::read(Token token) {
    if(token.value != nextToken.value)
        exit(1);

    if(token.type == "INTEGER" || token.type == "IDENTIFIER" || token.type == "STRING")
        buildTree(token, 0);
    
    nextToken = lexer->getNextToken();
    
    if(nextToken.value == "END" || tokensLeft == false)
        throw "All tokens parsed";
}

void Parser::buildTree(Token tokenVal, int popTreeCnt) {
    TreeNode* tempNode = new TreeNode;
    tempNode->value = tokenVal;
    
    if(popTreeCnt != 0) {
        while(!stk.empty() && popTreeCnt > 1) {
            TreeNode* curr = stk.top();
            stk.pop();
            makeRightNode(curr);
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

void Parser::makeRightNode(TreeNode* node) {
    TreeNode* parentNode = stk.top();
    stk.pop();
    parentNode->right = node;
    stk.push(parentNode);
}

void Parser::parseE() {
    if(nextToken.value == "let") {
        Token letToken("let", KEY);
        read(letToken);
        parseD();
        Token inToken("in", KEY);
        read(inToken);
        parseE();
        Token nodeToken("let", "let");
        buildTree(nodeToken, 2);
    } else if(nextToken.value == "fn") {
        Token fnToken("fn", "fn");
        read(fnToken);
        int n = 0;
        do {
            parseVb();
            n++;
        } while(nextToken.type == ID || nextToken.type == "(");
        Token dotToken(".", OPT);
        read(dotToken);
        parseE();
        Token nodeToken("lambda", "lambda");
        buildTree(nodeToken, n + 1);
    } else {
        parseEw();
    }
}

void Parser::parseEw() {
    parseT();
    if(nextToken.value == "where") {
        Token t("where", KEY);
        read(t);
        parseDr();
        Token nodeToken("where", "where");
        buildTree(nodeToken, 2);
    }
}

void Parser::parseT() {
    parseTa();
    if(nextToken.value == ",") {
        int n = 0;
        do {
            read(nextToken);
            parseTa();
            n++;
        } while(nextToken.value == ",");
        Token tauToken("tau", "tau");
        tauToken.tauCount = n + 1;
        buildTree(tauToken, n + 1);
    }
}

void Parser::parseTa() {
    parseTc();
    while(nextToken.value == "aug") {
        Token temp = nextToken;
        read(nextToken);
        parseTc();
        buildTree(temp, 2);
    }
}

void Parser::parseTc() {
    parseB();
    if(nextToken.value == "->") {
        read(nextToken);
        parseTc();
        Token elseToken("|", OPT);
        read(elseToken);
        parseTc();
        Token nodeToken("->", "->");
        buildTree(nodeToken, 3);
    }
}

void Parser::parseB() {
    parseBt();
    while(nextToken.value == "or") {
        nextToken.type = Parser::OPT;
        Token temp = nextToken;
        read(nextToken);
        parseBt();
        buildTree(temp, 2);
    }
}

void Parser::parseBt() {
    parseBs();
    while(nextToken.value == "&") {
        nextToken.type = Parser::OPT;
        Token temp = nextToken;
        read(nextToken);
        parseBs();
        buildTree(temp, 2);
    }
}

void Parser::parseBs() {
    if(nextToken.value == "not") {
        nextToken.type = "not";
        Token temp = nextToken;
        read(nextToken);
        parseBp();
        buildTree(temp, 1);
    } else {
        parseBp();
    }
}

void Parser::parseBp() {
    parseA();
    if(nextToken.value == "gr" || nextToken.value == ">") {
        nextToken.type = Parser::OPT;
        parseBpHelper(nextToken, "gr");
    } else if(nextToken.value == "ls" || nextToken.value == "<") {
        nextToken.type = Parser::OPT;
        parseBpHelper(nextToken, "ls");
    } else if(nextToken.value == "ge" || nextToken.value == ">=") {
        nextToken.type = Parser::OPT;
        parseBpHelper(nextToken, "ge");
    } else if(nextToken.value == "le" || nextToken.value == "<=") {
        nextToken.type = Parser::OPT;
        parseBpHelper(nextToken, "le");
    } else if(nextToken.value == "eq") {
        nextToken.type = Parser::OPT;
        parseBpHelper(nextToken, "eq");
    } else if(nextToken.value == "ne") {
        nextToken.type = Parser::OPT;
        parseBpHelper(nextToken, "ne");
    }
}

void Parser::parseBpHelper(Token nextToken, string tokenValue) {
    read(nextToken);
    parseA();
    Token t(tokenValue, Parser::OPT);
    buildTree(t, 2);
}

void Parser::parseA() {
    if(nextToken.value == "-") {
        Token negToken("-", OPT);
        read(negToken);
        parseAt();
        Token nodeToken("neg", "neg");
        buildTree(nodeToken, 1);
    } else if(nextToken.value == "+") {
        Token posToken("+", OPT);
        read(posToken);
        parseAt();
    } else {
        parseAt();
    }
    while(nextToken.value == "+" || nextToken.value == "-") {
        Token temp = nextToken;
        read(nextToken);
        parseAt();
        buildTree(temp, 2);
    }
}

void Parser::parseAt() {
    parseAf();
    while(nextToken.value == "*" || nextToken.value == "/") {
        Token temp = nextToken;
        read(nextToken);
        parseAf();
        buildTree(temp, 2);
    }
}

void Parser::parseAf() {
    parseAp();
    while(nextToken.value == "**") {
        Token temp = nextToken;
        read(nextToken);
        parseAf();
        buildTree(temp, 2);
    }
}

void Parser::parseAp() {
    parseR();
    while(nextToken.value == "@") {
        Token temp = nextToken;
        read(nextToken); // Read '@'
        if(nextToken.type != ID)
            throw "Expected Identifier found in parseAp()";
        read(nextToken); // Read identifier
        parseR();
        buildTree(temp, 3);
    }
}

void Parser::parseR() {
    parseRn();
    while(nextToken.type == ID || nextToken.type == STR || nextToken.type == INT ||
          nextToken.value == "true" || nextToken.value == "false" || nextToken.value == "nil" ||
          nextToken.value == "(" || nextToken.value == "dummy") {
        parseRn();
        Token nodeToken("gamma", "gamma");
        buildTree(nodeToken, 2);
    }
}

void Parser::parseRn() {
    if(nextToken.type == ID || nextToken.type == STR || nextToken.type == INT) {
        read(nextToken);
    } else if(nextToken.value == "true") {
        parseRHelper(nextToken, "true");
    } else if(nextToken.value == "false") {
        parseRHelper(nextToken, "false");
    } else if(nextToken.value == "nil") {
        parseRHelper(nextToken, "nil");
    } else if(nextToken.value == "dummy") {
        parseRHelper(nextToken, "dummy");
    } else if(nextToken.value == "(") {
        read(nextToken);
        parseE();
        Token t(")", ")");
        read(t);
    }
}

void Parser::parseRHelper(Token t, string value) {
    read(t);
    Token nodeToken(value, value);
    buildTree(nodeToken, 0);
}

void Parser::parseD() {
    parseDa();
    if(nextToken.value == "within") {
        read(nextToken);
        parseD();
        Token nodeToken("within", "within");
        buildTree(nodeToken, 2);
    }
}

void Parser::parseDa() {
    parseDr();
    if(nextToken.value == "and") {
        int n = 1;
        Token temp = nextToken;
        while(nextToken.value == "and") {
            read(nextToken);
            parseDr();
            n++;
        }
        buildTree(temp, n);
    }
}

void Parser::parseDr() {
    if(nextToken.value == "rec") {
        Token temp = nextToken;
        read(nextToken);
        parseDb();
        buildTree(temp, 1);
    } else {
        parseDb();
    }
}

void Parser::parseDb() {
    if(nextToken.value == "(") {
        read(nextToken);
        parseD();
        Token t(")", OPT);
        read(t);
    } else if(nextToken.type == ID && (lexer->peekNextToken().value == "," || lexer->peekNextToken().value == "=")) {
        parseV1();
        Token t("=", OPT);
        read(t);
        parseE();
        Token nodeToken("=", "=");
        buildTree(nodeToken, 2);
    } else {
        read(nextToken);
        int n = 1;
        parseVb();
        while(nextToken.type == ID || nextToken.value == "(") {
            n++;
            parseVb();
        }
        Token t("=", OPT);
        read(t);
        parseE();
        Token nodeToken("function_form", "function_form");
        buildTree(nodeToken, n + 2);
    }
}

void Parser::parseVb() {
    if(nextToken.type == ID) {
        read(nextToken);
    } else if(nextToken.value == "(") {
        read(nextToken);
        if(nextToken.value == ")") {
            read(nextToken);
            Token nodeToken("()", "()");
            buildTree(nodeToken, 0);
        } else {
            parseV1();
            Token t(")", ")");
            read(t);
        }
    }
}

void Parser::parseV1() {
    read(nextToken);
    int n = 1;
    if(nextToken.value == ",") {
        while(nextToken.value == ",") {
            n++;
            read(nextToken); // Reading ,
            read(nextToken); // Reading ID
        }
        Token nodeToken(",", ",");
        buildTree(nodeToken, n);
    }
}

void Parser::printTree() {
    TreeNode* t = stk.top();
    stk.pop();
    stk.push(t);
    preOrder(t, std::string(""));
}

void Parser::preOrder(TreeNode* t, std::string dots) {
    formattedPrint(t->value, dots);
    string dots1 = "." + dots;
    if(t->left != NULL)
        preOrder(t->left, dots1);
    if(t->right != NULL)
        preOrder(t->right, dots);
}

void Parser::formattedPrint(Token t, std::string dots) {
    if(t.type == ID) {
        // cout << dots << "<ID:" << t.value << '>' << endl;
    } else if(t.type == INT) {
        // cout << dots << "<INT:" << t.value << '>' << endl;
    } else if(t.type == STR) {
        // cout << dots << "<STR:" << t.value << '>' << endl;
    } else if(t.value == "true" || t.value == "false" || t.value == "nil" || t.value == "dummy") {
        // cout << dots << '<' << t.value << '>' << endl;
    } else {
        // cout << dots << t.value << endl;
    }
}

TreeNode* Parser::getTree() {
    TreeNode* s = stk.top();
    stk.pop();
    return s;
}