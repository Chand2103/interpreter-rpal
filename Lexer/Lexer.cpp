/**
 * Lexical Analyzer Implementation
 * This code implements a lexical analyzer (tokenizer) for a functional programming language.
 * It processes input strings by breaking them down into tokens including identifiers, keywords,
 * operators, integers, strings, and punctuation marks, maintaining token classification
 * and providing sequential access to parsed tokens for further compilation phases.
 */

#include <iostream>
#include "Lexer.h"
#include "Token.h"
#include <string>
#include <cstdlib>

using namespace std;

/**
 * Constructor - initializes lexer with input string and begins tokenization
 * @param inputString - source code string to be tokenized
 */
Lexer::Lexer(string inputString) {
    this->inputString = inputString;
    this->size = inputString.size();
    this->currentPosition = 0;
    this->tokenPointer = 0;
    this->hasRemainingTokens = true;
    this->additionalTokensExist = true;
    convertStringToTokens();
}

/**
 * Default constructor
 */
Lexer::Lexer() {
}

/**
 * Destructor
 */
Lexer::~Lexer() {
}

/**
 * Character classification method - checks if character is an escape sequence
 * @param c - character to check
 * @return true if character is escape sequence, false otherwise
 */
bool Lexer::isEscapeSequence(char c) {
    switch(c) {
        case '\\':
        case '\'':
        case 't':
        case 'n':
            return true;
        default:
            return false;
    }
}

/**
 * Character classification method - checks if character is punctuation
 * @param c - character to check
 * @return true if character is punctuation, false otherwise
 */
bool Lexer::isPunctuationChar(char c) {
    switch(c) {
        case '(':
        case ')':
        case ';':
        case ',':
        case ' ':
            return true;
        default:
            return (isalpha(c) || isdigit(c) || isOperatorChar(c));
    }
}

/**
 * Character classification method - checks if character is an operator
 * @param c - character to check
 * @return true if character is operator, false otherwise
 */
bool Lexer::isOperatorChar(char c) {
    return operator_set.find(c) != operator_set.end();
}

/**
 * String classification method - checks if string is a reserved keyword
 * @param st - string to check
 * @return true if string is keyword, false otherwise
 */
bool Lexer::isReservedKeyword(string st) {
    return keyword_set.find(st) != keyword_set.end();
}

/**
 * Token processing method - handles punctuation marks
 * @param c - punctuation character
 * @return Token object representing punctuation
 */
Token Lexer::processPunctuationMark(char c) {
    Token t;
    t.type = c;
    t.value = c;
    return t;
}

/**
 * Token processing method - handles numeric literals
 * @param c - first digit character
 * @return Token object representing integer
 */
Token Lexer::processNumericToken(char c) {
    Token t;
    t.value += c;
    while(true) {
        if(currentPosition == size) {
            break;
        } else {
            c = inputString.at(currentPosition++);
            if(!isdigit(c)) {
                currentPosition--;
                break;
            } else {
                t.value += c;
            }
        }
    }
    t.type = "INTEGER";
    return t;
}

/**
 * Token processing method - handles operators and comments
 * @param c - operator character
 * @return Token object representing operator
 */
Token Lexer::processOperatorToken(char c) {
    Token t;

    if(c == '/') {
        if(inputString.at(currentPosition++) == '/') {
            while(true) {
                c = inputString.at(currentPosition++);
                
                if(isOperatorChar(c) || isPunctuationChar(c) || isEscapeSequence(c)) {
                    continue;
                } else if(c == '\n') {
                    currentPosition--;
                    break;
                }
            }
            convertStringToTokens();
        } else {
            currentPosition--;
        }
    }
    t.value += c;
    while(true) {
        if(currentPosition == size) {
            break;
        } else {
            c = inputString.at(currentPosition++);
            if(!isOperatorChar(c)) {
                currentPosition--;
                break;
            } else {
                t.value += c;
            }
        }
    }
    t.type = "OPERATOR";
    return t;
}

/**
 * Token processing method - handles string literals
 * @param c - opening quote character
 * @return Token object representing string
 */
Token Lexer::processStringLiteral(char c) {
    Token t;
    t.value = c;
    while(true) {
        c = inputString.at(currentPosition++);
        
        if(isPunctuationChar(c)) {
            t.value += c;
        } else if(c == '\'') {
            t.type = "STRING";
            t.value += c;
            break;
        } else if(c == '\\') {
            char nextc = inputString.at(currentPosition++);
            if(isEscapeSequence(nextc)) {
                t.value += c;
                t.value += nextc;
            } else {
                throw "unable to create string token";
            }
        }
    }
    return t;
}

/**
 * Token processing method - handles identifiers and keywords
 * @param c - first alphabetic character
 * @return Token object representing identifier or keyword
 */
Token Lexer::processAlphabeticToken(char c) {
    Token t;
    t.value += c;
    while(true) {
        if(currentPosition != size) {
            c = inputString.at(currentPosition++);
            if(!isalpha(c) && !isdigit(c) && c != '_') {
                currentPosition--;
                break;
            } else {
                t.value += c;
            }
        } else {
            break;
        }
    }
    if(!isReservedKeyword(t.value)) {
        t.type = "IDENTIFIER";
    } else {
        t.type = "KEYWORD";
    }
    return t;
}

/**
 * Main tokenization method - processes entire input string into tokens
 * Uses character classification to determine token types and delegates to appropriate handlers
 */
void Lexer::convertStringToTokens() {
    while(currentPosition < size) {
        Token token;
        
        char c = inputString.at(currentPosition++);

        switch(c) {
            case ' ':
            case '\t':
            case '\n':
                continue;
            case '(':
            case ')':
            case ';':
            case ',':
                token = processPunctuationMark(c);
                break;
            case '\'':
                token = processStringLiteral(c);
                break;
            default:
                if(isalpha(c)) {
                    token = processAlphabeticToken(c);
                } else if(isdigit(c)) {
                    token = processNumericToken(c);
                } else if(isOperatorChar(c)) {
                    token = processOperatorToken(c);
                }
                break;
        }
        tokens.push_back(token);
    }
}

/**
 * Token access method - retrieves next token from sequence
 * @return next Token object in sequence
 */
Token Lexer::retrieveNextToken() {
    Token t;
    if(tokenPointer < tokens.size())
        t = tokens[tokenPointer++];
    else
        Token t("End", "End");
    return t;
}

/**
 * Token access method - previews next token without advancing pointer
 * @return next Token object without consuming it
 */
Token Lexer::previewNextToken() {
    Token t = tokens[tokenPointer];
    return t;
}

/**
 * Static token type identifiers for external reference
 */
string Lexer::ID = "IDENTIFIER";
string Lexer::STR = "STRING";
string Lexer::INT = "INTEGER";
string Lexer::KEY = "KEYWORD";
string Lexer::OPT = "OPERATOR";