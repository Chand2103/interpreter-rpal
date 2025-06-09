#include <iostream>
#include "Lexer.h"
#include "Token.h"
#include <string>
#include <cstdlib>

using namespace std;

Lexer::Lexer(string inputString) {
    this->inputString = inputString;
    this->size = inputString.size();
    this->presentVal = 0;
    this->currPtr = 0;
    this->tokensLeft = true;
    this->moreTokens = true;
    tokenizeStr();
}

Lexer::~Lexer() {
}

Lexer::Lexer() {
}

void Lexer::tokenizeStr() {
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

bool Lexer::aKeyword(string st) {
    return keyword_set.find(st) != keyword_set.end();
}

bool Lexer::anOperator(char c) {
    return operator_set.find(c) != operator_set.end();
}

bool Lexer::aComment(char c) {
    if(c == '\\' || c == '\'' || c == 't' || c == 'n')
        return true;
    return false;
}

bool Lexer::aPunctuation(char c) {
    if(c == '(' || c == ')' || c == ';' || c == ',' || c == ' ' || 
       isalpha(c) || isdigit(c) || anOperator(c))
        return true;
    return false;
}

Token Lexer::tokenizeIdentifier(char c) {
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

Token Lexer::tokenizeString(char c) {
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

Token Lexer::tokenizeOperator(char c) {
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

Token Lexer::tokenizeInteger(char c) {
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

Token Lexer::tokenizePunctuation(char c) {
    Token t;
    t.type = c;
    t.value = c;
    return t;
}

Token Lexer::getNextToken() {
    Token t;
    if(currPtr < tokens.size())
        t = tokens[currPtr++];
    else
        Token t("End", "End");
    return t;
}

Token Lexer::peekNextToken() {
    Token t = tokens[currPtr];
    return t;
}

string Lexer::ID = "IDENTIFIER";
string Lexer::STR = "STRING";
string Lexer::INT = "INTEGER";
string Lexer::KEY = "KEYWORD";
string Lexer::OPT = "OPERATOR";