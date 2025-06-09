/**
 * Control Stack Environment (CSE) Machine Builder Implementation
 * 
 * This module implements the control structure building phase of the CSE machine.
 * It processes the abstract syntax tree and creates the delta control structures
 * that will be used during the evaluation phase. The builder handles lambda expressions,
 * conditional statements, and other language constructs by creating appropriate
 * control sequences.
 */

#include <algorithm>
#include <iostream>
#include "CSEMachine.h"
#include "Lexer.h"

using namespace std;

/**
 * Primary control structure creation method
 * Builds delta control structures from input tree using breadth-first processing
 * @param root - Root node of the abstract syntax tree
 */
void CSEMachine::createControlStructures(TreeNode* root) {
    pendingDeltaQueue.push(root);
    while (!pendingDeltaQueue.empty()) {
        vector<Token> currentDelta;
        TreeNode* node = pendingDeltaQueue.front(); 
        pendingDeltaQueue.pop();
        buildDeltaSequence(node, currentDelta);
        deltaMap[currDeltaNum++] = currentDelta;
    }
}

/**
 * Tree traversal method for building control structures
 * Performs pre-order traversal to create delta sequences based on node types
 * @param root - Current node being processed
 * @param currentDelta - Reference to current delta sequence being built
 */
void CSEMachine::buildDeltaSequence(TreeNode* root, vector<Token>& currentDelta) {
    switch (root->value.type[0]) {
        case 'l': // lambda
            if (root->value.type == "lambda") {
                if (root->left->value.value != ",") {
                    currentDelta.emplace_back("lambdaClosure", root->left->value.value, ++deltaCounter);
                } else {
                    TreeNode* c = root->left->left;
                    string tuple;
                    while (c) { 
                        tuple += c->value.value + ","; 
                        c = c->right; 
                    }
                    Token lambdaClosure("lambdaClosure", tuple, ++deltaCounter);
                    lambdaClosure.isTuple = true;
                    currentDelta.push_back(lambdaClosure);
                }
                pendingDeltaQueue.push(root->left->right);
                if (root->right) buildDeltaSequence(root->right, currentDelta);
                return;
            }
            break;
    }
    
    if (root->value.value == "->") {
        currentDelta.emplace_back("beta", deltaCounter + 1, deltaCounter + 2);
        pendingDeltaQueue.push(root->left->right);
        pendingDeltaQueue.push(root->left->right->right);
        root->left->right->right = nullptr;
        root->left->right = nullptr;
        deltaCounter += 2;
        if (root->left) buildDeltaSequence(root->left, currentDelta);
        if (root->right) buildDeltaSequence(root->right, currentDelta);
    } else {
        currentDelta.push_back(root->value);
        if (root->left) buildDeltaSequence(root->left, currentDelta);
        if (root->right) buildDeltaSequence(root->right, currentDelta);
    }
}

/**
 * Utility method for integer to string conversion
 * @param val - Integer value to convert
 * @return String representation of the integer
 */
string CSEMachine::convertIntToString(int val) {
    return to_string(val);
}

/**
 * String tokenization utility method
 * Splits input string by delimiter character
 * @param input - String to be tokenized  
 * @param delim - Delimiter character for splitting
 * @return Vector containing split string tokens
 */
vector<string> CSEMachine::tokenizeString(string input, char delim) {
    vector<string> result;
    string token;
    for (char ch : input) {
        switch (ch == delim) {
            case true:
                result.push_back(token);
                token.clear();
                break;
            case false:
                token += ch;
                break;
        }
    }
    result.push_back(token);
    return result;
}

/**
 * Built-in function identification method
 * Determines if identifier represents a user-defined function
 * @param id - Identifier string to check
 * @return True if user-defined function, false if built-in
 */
bool CSEMachine::isUserDefinedFunction(string id) {
    static const vector<string> builtins = {
        "Stem", "stem", "Stern", "stern", "Print", "print", "Conc", "conc",
        "Istuple", "Isinteger", "Isfunction", "Istruthvalue", "Isdummy", "Order", "Null"
    };
    return find(builtins.begin(), builtins.end(), id) == builtins.end();
}

/**
 * Parameter resolution method
 * Checks if token represents a parameter accessible in current environment chain
 * @param currToken - Token to check for parameter status
 * @return True if token is resolvable parameter, false otherwise
 */
bool CSEMachine::resolveParameterStatus(Token currToken) {
    string var = currToken.value;
    int env = currEnv;
    key_pair key(env, var);
    auto it = paramMap.find(key);
    while (it == paramMap.end() && env >= 0) {
        env = envMap[env];
        key.first = env;
        it = paramMap.find(key);
    }
    return it != paramMap.end();
}

/**
 * String unescaping utility method
 * Processes escape sequences in string literals
 * @param s - Input string containing escape sequences
 * @return Processed string with escape sequences resolved
 */
string CSEMachine::processEscapeSequences(const string& s) {
    string res;
    auto it = s.begin();
    while (it != s.end()) {
        char c = *it++;
        if (c == '\\' && it != s.end()) {
            switch (*it++) {
                case '\\': 
                    c = '\\'; 
                    break;
                case 'n': 
                    c = '\n'; 
                    break;
                case 't': 
                    c = '\t'; 
                    break;
                default: 
                    continue;
            }
        }
        res += c;
    }
    return res;
}

/**
 * Tuple printing utility method
 * Handles formatted display of tuple data structures
 * @param t - Token representing tuple to display
 */
void CSEMachine::displayTupleStructure(Token t) {
    vector<Token>& v = t.tuple;
    cout << "(";
    for (size_t i = 0; i < v.size(); i++) {
        if (i > 0) cout << ", ";
        switch (v[i].type[0]) {
            case 'S': // STR type
                if (v[i].type == Lexer::STR)
                    cout << processEscapeSequences(v[i].value.substr(1, v[i].value.size() - 2));
                else
                    cout << v[i].value;
                break;
            case 't': // tuple type
                if (v[i].type == "tuple")
                    displayTupleStructure(v[i]);
                else
                    cout << v[i].value;
                break;
            default:
                cout << v[i].value;
                break;
        }
    }
    cout << ")";
}