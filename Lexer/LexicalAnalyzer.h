/*
 * LexicalAnalyzer.h
 *
 *  Created on: Oct 14, 2011
 *      Author: Saurabh Saxena
 *      // UFID        : 21817195
 */

#include <string>
#include <vector>
#include <unordered_set>
#include <cstdlib>
#include "Token.h"

#ifndef LEXICALANALYZER_H_
#define LEXICALANALYZER_H_

using namespace std;

class LexicalAnalyzer {
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
    int presentVal;
    int currPtr;
    vector<Token> tokens;
    bool tokensLeft;
    bool moreTokens;

public:
    LexicalAnalyzer(string codeString);
    LexicalAnalyzer();
    virtual ~LexicalAnalyzer();
    
    // Tokenization methods
    void tokenizeStr();
    Token tokenizeIdentifier(char ch);
    Token tokenizeInteger(char ch);
    Token tokenizeOperator(char ch);
    Token tokenizeString(char ch);
    Token tokenizePunctuation(char ch);
    
    // Token access methods
    Token getNextToken();
    Token peekNextToken();
    
    // Utility methods
    bool anOperator(char c);
    bool aKeyword(string tokenValue);
    bool aComment(char c);
    bool aPunctuation(char c);
    
    // Static constants
    static string ID;
    static string STR;
    static string INT;
    static string KEY;
    static string OPT;
};

#endif /* LEXICALANALYZER_H_ */