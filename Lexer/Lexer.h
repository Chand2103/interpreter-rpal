/**
 * Lexical Analyzer Implementation
 * This code implements a lexical analyzer (tokenizer) for a functional programming language.
 * It processes input strings by breaking them down into tokens including identifiers, keywords,
 * operators, integers, strings, and punctuation marks, maintaining token classification
 * and providing sequential access to parsed tokens for further compilation phases.
 */

#include <string>
#include <vector>
#include <unordered_set>
#include <cstdlib>
#include "Token.h"

#ifndef LEXICALANALYZER_H_
#define LEXICALANALYZER_H_

using namespace std;

class Lexer {
    unordered_set<char> operator_set {
        '+', '-', '*', '<', '>', '&', '.', '@', '/', ':', '=', '~', '|', '$',
        '!', '#', '%', '^', '_', '[', ']', '{', '}', '"', '`', '?'
    };

    unordered_set<string> keyword_set {
        "let","in","fn","where","aug","or","not","gr","ge","ls","le","eq","ne",
        "true","false","nil","dummy","within","and","rec","list"
    };

private:
    string inputString;
    int size;
    int currentPosition;
    int tokenPointer;
    vector<Token> tokens;
    bool hasRemainingTokens;
    bool additionalTokensExist;

public:
    /**
     * Constructor - initializes lexer with input string
     * @param codeString - source code to tokenize
     */
    Lexer(string codeString);
    
    /**
     * Default constructor
     */
    Lexer();
    
    /**
     * Destructor
     */
    virtual ~Lexer();
    
    /**
     * Token processing methods - handle different token types
     */
    Token processAlphabeticToken(char ch);
    Token processNumericToken(char ch);
    Token processOperatorToken(char ch);
    Token processStringLiteral(char ch);
    Token processPunctuationMark(char ch);
    
    /**
     * Main tokenization method - converts input string to tokens
     */
    void convertStringToTokens();
    
    /**
     * Token access methods
     */
    Token retrieveNextToken();
    Token previewNextToken();
    
    /**
     * Character classification methods
     */
    bool isOperatorChar(char c);
    bool isReservedKeyword(string tokenValue);
    bool isEscapeSequence(char c);
    bool isPunctuationChar(char c);
    
    /**
     * Static token type identifiers
     */
    static string ID;
    static string STR;
    static string INT;
    static string KEY;
    static string OPT;
};

#endif