/*
 * LexicalAnalyzer.cpp
 *
 *  Created on: Oct 14, 2011
 *      Author: saurabh Saurabh
 *      // UFID        : 21817195
 */
#include <iostream>
#include "LexicalAnalyzer.h"
#include "Token.h"
#include <string>
#include <cstdlib>

using namespace std;

LexicalAnalyzer::LexicalAnalyzer(string inputString) {
    this->inputString = inputString;
    this->size = inputString.size();
    this->presentVal = 0;
    this->currPtr = 0;
    this->tokensLeft = true;
    this->moreTokens = true;
    // Automatically tokenize the input string
    tokenizeStr();
}

LexicalAnalyzer::~LexicalAnalyzer() {
}

LexicalAnalyzer::LexicalAnalyzer() {
}

// Method to tokenize the string and generate tokens
void LexicalAnalyzer::tokenizeStr() {
    while(presentVal < size) {
        Token token;
        
        char c = inputString.at(presentVal++);

        if(isspace(c) || c == '\t' || c == '\n') {
            continue;
        } else if(isalpha(c)) {
            token = tokenizeIdentifier(c);
        } else if(isdigit(c)) {
            token = tokenizeInteger(c);
        } else if(anOperator(c)) {
            token = tokenizeOperator(c);
        } else if(c == '\'') {
            token = tokenizeString(c);			
        } else if(c == '(' || c == ')' || c == ';' || c == ',') {
            token = tokenizePunctuation(c);
        }
        tokens.push_back(token);
    }
}

// Method to check the char is among the rpal keyword set
bool LexicalAnalyzer::aKeyword(string st) {
    return keyword_set.find(st) != keyword_set.end();
}

// Method to check the char is among the rpal operator set
bool LexicalAnalyzer::anOperator(char c) {
    return operator_set.find(c) != operator_set.end();
}

// Method to check the char is comment or tab or new line
bool LexicalAnalyzer::aComment(char c) {
    if(c == '\\' || c == '\'' || c == 't' || c == 'n')
        return true;
    return false;
}

// Method to check the char is punctuation or alpha or digit or operator
bool LexicalAnalyzer::aPunctuation(char c) {
    if(c == '(' || c == ')' || c == ';' || c == ',' || c == ' ' || 
       isalpha(c) || isdigit(c) || anOperator(c))
        return true;
    return false;
}

// Method for creating token if the char is identifier
Token LexicalAnalyzer::tokenizeIdentifier(char c) {
    Token t;
    t.value += c;
    while(true) {
        if(presentVal != size) {
            c = inputString.at(presentVal++);
            if(!isalpha(c) && !isdigit(c) && c != '_') {
                presentVal--;
                break;
            } else {
                t.value += c;
            }
        } else {
            break;
        }
    }
    if(!aKeyword(t.value)) {
        t.type = "IDENTIFIER";
    } else {
        t.type = "KEYWORD";
    }
    return t;
}

// Method for creating token if the char is string start
Token LexicalAnalyzer::tokenizeString(char c) {
    Token t;
    t.value = c;
    while(true) {
        c = inputString.at(presentVal++);
        
        if(aPunctuation(c)) {
            t.value += c;
        } else if(c == '\'') {
            t.type = "STRING";
            t.value += c;
            break;
        } else if(c == '\\') {
            char nextc = inputString.at(presentVal++);
            if(aComment(nextc)) {
                t.value += c;
                t.value += nextc;
            } else {
                throw "unable to create string token";
            }
        }
    }
    return t;
}

// Method for creating token if the char is operator
Token LexicalAnalyzer::tokenizeOperator(char c) {
    Token t;

    if(c == '/') {
        if(inputString.at(presentVal++) == '/') {
            while(true) {
                c = inputString.at(presentVal++);
                
                if(anOperator(c) || aPunctuation(c) || aComment(c)) {
                    continue;
                } else if(c == '\n') {
                    presentVal--;
                    break;
                }
            }
            tokenizeStr();
        } else {
            presentVal--;
        }
    }
    t.value += c;
    while(true) {
        if(presentVal == size) {
            break;
        } else {
            c = inputString.at(presentVal++);
            if(!anOperator(c)) {
                presentVal--;
                break;
            } else {
                t.value += c;
            }
        }
    }
    t.type = "OPERATOR";
    return t;
}

// Method for creating token if the char is integer
Token LexicalAnalyzer::tokenizeInteger(char c) {
    Token t;
    t.value += c;
    while(true) {
        if(presentVal == size) {
            break;
        } else {
            c = inputString.at(presentVal++);
            if(!isdigit(c)) {
                presentVal--;
                break;
            } else {
                t.value += c;
            }
        }
    }
    t.type = "INTEGER";
    return t;
}

// Method for creating token if the char is punctuation
Token LexicalAnalyzer::tokenizePunctuation(char c) {
    Token t;
    t.type = c;
    t.value = c;
    return t;
}

// Method to get the next token by moving pointer ahead
Token LexicalAnalyzer::getNextToken() {
    Token t;
    if(currPtr < tokens.size())
        t = tokens[currPtr++];
    else
        Token t("End", "End");
    return t;
}

// Method to peek the token from the vector
Token LexicalAnalyzer::peekNextToken() {
    Token t = tokens[currPtr];
    return t;
}

string LexicalAnalyzer::ID = "IDENTIFIER";
string LexicalAnalyzer::STR = "STRING";
string LexicalAnalyzer::INT = "INTEGER";
string LexicalAnalyzer::KEY = "KEYWORD";
string LexicalAnalyzer::OPT = "OPERATOR";