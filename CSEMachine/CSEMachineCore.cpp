/**
 * Control Stack Environment (CSE) Machine Core Implementation
 * 
 * This module implements the core evaluation engine of the CSE machine.
 * It processes tokens from the control stack and applies the appropriate
 * transition rules based on the CSE machine semantics. The engine handles
 * arithmetic operations, function applications, environment management,
 * and built-in function evaluations.
 */

#include "CSEMachine.h"
#include "Lexer.h"
#include <iostream>
#include <math.h>
using namespace std;

/**
 * Default constructor for CSE machine
 */
CSEMachine::CSEMachine() {}

/**
 * Destructor for CSE machine
 */
CSEMachine::~CSEMachine() {}

/**
 * Parameterized constructor for CSE machine
 * Initializes all necessary data structures and counters
 * @param input - Root node of the abstract syntax tree to evaluate
 */
CSEMachine::CSEMachine(TreeNode* input){
    this->inputTree = input;
    this->deltaCounter = 0;
    this->currDeltaNum = 0;
    this->envCounter = 0;
    this->envStack.push(0);
    this->currEnv = 0;
    this->envMap = map<int,int>();
    this->printCalled = false;
}

/**
 * Main evaluation method - executes the CSE machine
 * Creates control structures and processes them until completion
 */
void CSEMachine::evaluateTree() {
    createControlStructures(this->inputTree);
    Token envToken("env", envCounter);
    stack<Token> controlStack;
    stack<Token> executionStack;
    controlStack.push(envToken);
    envMap[0] = -1;
    for(auto& token : deltaMap[0]) controlStack.push(token);
    executionStack.push(envToken);
    
    int whileCount = 0;
    while(controlStack.size() > 1) {
        Token currToken = controlStack.top(); 
        controlStack.pop();
        executeTokenTransition(currToken, controlStack, executionStack);
        if(whileCount++ > 5000) break;
    }
    
    if(!printCalled) {
        vector<Token> stackContents;
        
        // Copy stack contents to vector (since we can't iterate through stack)
        while (!executionStack.empty()) {
            stackContents.push_back(executionStack.top());
            executionStack.pop();
        }
        
        // Find the first non-env token (should be the result)
        for (const Token& token : stackContents) {
            if (token.type != "env") {
                cout << token.value << endl;
                break;
            }
        }
    }
}
/**
 * Arithmetic and logical operator application method
 * Performs binary operations on operands based on operator type
 * @param firstToken - First operand token
 * @param secondToken - Second operand token  
 * @param currToken - Operator token
 * @return Result token after applying operation
 */
Token CSEMachine::performOperation(Token firstToken, Token secondToken, Token currToken){
    string op = currToken.value;
    
    switch (firstToken.type[0]) {
        case 'I': // INT type
            if (firstToken.type == Lexer::INT) {
                int a = stoi(firstToken.value), b = stoi(secondToken.value), res;
                switch (op[0]) {
                    case '*':
                        if (op == "*") res = a * b;
                        else if (op == "**") res = pow(a, b);
                        return Token(to_string(res), Lexer::INT);
                    case '+':
                        res = a + b;
                        return Token(to_string(res), Lexer::INT);
                    case '-':
                        res = a - b;
                        return Token(to_string(res), Lexer::INT);
                    case '/':
                        res = a / b;
                        return Token(to_string(res), Lexer::INT);
                    case 'g':
                        if (op == "gr") return a > b ? Token("true","true") : Token("false","false");
                        else if (op == "ge") return a >= b ? Token("true","true") : Token("false","false");
                        break;
                    case 'l':
                        if (op == "ls") return a < b ? Token("true","true") : Token("false","false");
                        else if (op == "le") return a <= b ? Token("true","true") : Token("false","false");
                        break;
                    case 'e':
                        return a == b ? Token("true","true") : Token("false","false");
                    case 'n':
                        return a != b ? Token("true","true") : Token("false","false");
                }
            }
            break;
            
        case 'S': // STR type
            if (firstToken.type == Lexer::STR) {
                switch (op[0]) {
                    case 'e':
                        return firstToken.value == secondToken.value ? Token("true","true") : Token("false","false");
                    case 'n':
                        return firstToken.value != secondToken.value ? Token("true","true") : Token("false","false");
                }
            }
            break;
            
        case 't': // true type
        case 'f': // false type
            if (firstToken.type == "true" || firstToken.type == "false") {
                switch (op[0]) {
                    case 'o':
                        return (firstToken.type == "true" || secondToken.type == "true") ? Token("true","true") : Token("false","false");
                    case '&':
                        return (firstToken.type == "true" && secondToken.type == "true") ? Token("true","true") : Token("false","false");
                    case 'e':
                        return (firstToken.type == secondToken.type) ? Token("true","true") : Token("false","false");
                    case 'n':
                        return (firstToken.type != secondToken.type) ? Token("true","true") : Token("false","false");
                }
            }
            break;
    }
    return Token("", "");
}

/**
 * Core token processing method - implements CSE machine transition rules
 * Handles all token types and applies appropriate transformations
 * @param currToken - Current token being processed
 * @param controlStack - Reference to control stack
 * @param executionStack - Reference to execution stack
 */
void CSEMachine::executeTokenTransition(Token &currToken, stack<Token> &controlStack, stack<Token> &executionStack){
    switch (currToken.type[0]) {
        case 'O': // OPT type - operators
            if(currToken.type == Lexer::OPT) {
                Token firstToken = executionStack.top(); executionStack.pop();
                Token secondToken = executionStack.top(); executionStack.pop();
                Token resultToken = performOperation(firstToken, secondToken, currToken);
                executionStack.push(resultToken);
                return;
            }
            break;
            
        case 'n': // neg and not operations
            if(currToken.type == "neg") {
                Token firstToken = executionStack.top(); executionStack.pop();
                int paramVal = atoi(firstToken.value.c_str());
                paramVal = -paramVal;
                Token newToken(convertIntToString(paramVal), Lexer::INT);
                executionStack.push(newToken);
                return;
            } else if(currToken.type == "not") {
                Token firstToken = executionStack.top(); executionStack.pop();
                executionStack.push(firstToken.value == "true" ? Token("false","false") : Token("true","true"));
                return;
            }
            break;
            
        case 'I': // ID type - identifiers
            if(currToken.type == Lexer::ID && resolveParameterStatus(currToken)) {
                string varName = currToken.value;
                int temp = currEnv;
                pair<int,string> keyPair(temp,varName);
                auto it = paramMap.find(keyPair);
                while(it == paramMap.end() && temp >= 0) {
                    temp = envMap[temp];
                    keyPair.first = temp;
                    it = paramMap.find(keyPair);
                }
                if(it != paramMap.end()) {
                    executionStack.push(it->second);
                }
                return;
            }
            break;
            
        case 'g': // gamma operations
            if(currToken.type == "gamma") {
                handleGammaOperation(controlStack, executionStack);
                return;
            }
            break;
            
        case 'e': // env operations
            if(currToken.type == "env") {
                Token topToken = executionStack.top(); executionStack.pop();
                executionStack.pop();
                executionStack.push(topToken);
                envStack.pop();
                currEnv = envStack.top();
                return;
            }
            break;
            
        case 'b': // beta operations
            if(currToken.type == "beta") {
                Token topToken = executionStack.top(); executionStack.pop();
                vector<Token> delta;
                if(topToken.value == "true") {
                    delta = deltaMap[currToken.betaIfDeltaNum];
                } else {
                    delta = deltaMap[currToken.betaElseDeltaNum];
                }
                for(unsigned int i=0; i<delta.size(); i++) {
                    controlStack.push(delta[i]);
                }
                return;
            }
            break;
            
        case 'l': // lambdaClosure operations
            if(currToken.type == "lambdaClosure") {
                currToken.lambdaEnv = currEnv;
                executionStack.push(currToken);
                return;
            }
            break;
    }
    
    // Handle special value operations
    handleSpecialValueOperations(currToken, controlStack, executionStack);
}

/**
 * Gamma operation handler - processes function applications
 * Handles lambda closures, built-in functions, and tuple operations
 * @param controlStack - Reference to control stack
 * @param executionStack - Reference to execution stack
 */
void CSEMachine::handleGammaOperation(stack<Token> &controlStack, stack<Token> &executionStack) {
    Token topExeToken = executionStack.top();
    executionStack.pop();
    
    switch (topExeToken.type[0]) {
        case 'l': // lambdaClosure
            if(topExeToken.type == "lambdaClosure") {
                handleLambdaApplication(topExeToken, controlStack, executionStack);
                return;
            }
            break;
            
        case 'Y': // YSTAR
            if(topExeToken.type == "YSTAR") {
                Token nextToken = executionStack.top();
                executionStack.pop();
                nextToken.type = "eta";
                executionStack.push(nextToken);
                return;
            }
            break;
            
        case 'e': // eta
            if(topExeToken.type == "eta") {
                Token lambdaToken = topExeToken;
                lambdaToken.type = "lambdaClosure";
                executionStack.push(topExeToken);
                executionStack.push(lambdaToken);
                Token gammaToken("gamma","gamma");
                controlStack.push(gammaToken);
                controlStack.push(gammaToken);
                return;
            }
            break;
    }
    
    // Handle built-in functions and tuple operations
    handleBuiltInFunctions(topExeToken, controlStack, executionStack);
}

/**
 * Lambda application handler - manages function calls and environment setup
 * @param topExeToken - Lambda closure token
 * @param controlStack - Reference to control stack
 * @param executionStack - Reference to execution stack
 */
void CSEMachine::handleLambdaApplication(Token topExeToken, stack<Token> &controlStack, stack<Token> &executionStack) {
    Token env("env", ++envCounter);
    envMap[envCounter] = topExeToken.lambdaEnv;
    envStack.push(envCounter);
    currEnv = envCounter;
    
    if(!topExeToken.isTuple) {
        string paramName = topExeToken.lambdaParam;
        Token paramToken = executionStack.top();
        executionStack.pop();
        pair<int,string> keyPair(envCounter, paramName);
        paramMap[keyPair] = paramToken;
    } else {
        string tuple = topExeToken.lambdaParam;
        vector<string> params = tokenizeString(tuple, ',');
        Token valueTuple = executionStack.top();
        executionStack.pop();
        vector<Token> tupleVector = valueTuple.tuple;
        for(unsigned int i=0; i<params.size(); i++) {
            if(params[i] != "") {
                pair<int,string> keyPair(envCounter, params[i].c_str());
                paramMap[keyPair] = tupleVector[i];
            }
        }
    }
    
    controlStack.push(env);
    executionStack.push(env);
    int lambdaNum = topExeToken.lambdaNum;
    vector<Token> delta = deltaMap[lambdaNum];
    for(int i=0; i<delta.size(); i++) {
        controlStack.push(delta[i]);
    }
}

/**
 * Built-in function handler - processes system functions
 * @param topExeToken - Function token
 * @param controlStack - Reference to control stack
 * @param executionStack - Reference to execution stack
 */
void CSEMachine::handleBuiltInFunctions(Token topExeToken, stack<Token> &controlStack, stack<Token> &executionStack) {
    string funcName = topExeToken.value;
    
    switch (funcName[0]) {
        case 'S': // Stern, Stem
            handleStringFunctions(funcName, executionStack);
            break;
        case 'C': // Conc
            if (funcName == "Conc" || funcName == "conc") {
                handleConcatenation(controlStack, executionStack);
            }
            break;
        case 'I': // ItoS, Isinteger, etc.
            handleTypeFunctions(funcName, executionStack);
            break;
        case 'P': // Print
            if (funcName == "Print" || funcName == "print") {
                handlePrintFunction(executionStack);
            }
            break;
        case 'O': // Order
            if (funcName == "Order") {
                handleOrderFunction(executionStack);
            }
            break;
        case 'N': // Null
            if (funcName == "Null") {
                handleNullFunction(executionStack);
            }
            break;
        default:
            handleTupleIndexing(topExeToken, executionStack);
            break;
    }
}

/**
 * Special value operations handler - processes tau, nil, aug operations
 * @param currToken - Current token being processed
 * @param controlStack - Reference to control stack
 * @param executionStack - Reference to execution stack
 */
void CSEMachine::handleSpecialValueOperations(Token &currToken, stack<Token> &controlStack, stack<Token> &executionStack) {
    if(currToken.value == "tau") {
        int tauCount = currToken.tauCount;
        string tuple="(";
        vector<Token> tupleVector;
        for(int i=0; i<tauCount; i++) {
            Token t = executionStack.top();
            tupleVector.push_back(t);
            executionStack.pop();
            if(i == tauCount -1)
                tuple += t.value;
            else
                tuple += t.value +", ";
        }
        tuple +=")";
        Token newToken(tuple,"tuple");
        newToken.tuple = tupleVector;
        newToken.isTuple = true;
        executionStack.push(newToken);
    } else if(currToken.value == "nil") {
        currToken.isTuple = true;
        executionStack.push(currToken);
    } else if(currToken.value == "aug") {
        Token tuple = executionStack.top(); executionStack.pop();
        Token toAdd = executionStack.top(); executionStack.pop();
        if(tuple.value == "nil") {
            Token newToken(toAdd.value,"tuple");
            newToken.isTuple = true;
            newToken.tuple = vector<Token>();
            newToken.tuple.push_back(toAdd);
            executionStack.push(newToken);
        } else {
            tuple.tuple.push_back(toAdd);
            executionStack.push(tuple);
        }
    } else {
        executionStack.push(currToken);
    }
}

/**
 * Helper method implementations for specific built-in functions
 */

void CSEMachine::handleStringFunctions(string funcName, stack<Token> &executionStack) {
    Token stringToken = executionStack.top(); executionStack.pop();
    string tokenValue = stringToken.value;
    
    if (funcName == "Stern" || funcName == "stern") {
        tokenValue = tokenValue.substr(2, tokenValue.size()-3);
        tokenValue = "'" + tokenValue + "'";
    } else if (funcName == "Stem" || funcName == "stem") {
        tokenValue = tokenValue.substr(1, 1);
        tokenValue = "'" + tokenValue + "'";
    }
    
    stringToken.value = tokenValue;
    executionStack.push(stringToken);
}

void CSEMachine::handleConcatenation(stack<Token> &controlStack, stack<Token> &executionStack) {
    Token firstToken = executionStack.top(); executionStack.pop();
    Token secondToken = executionStack.top(); executionStack.pop();
    string concatValue = firstToken.value.substr(1, firstToken.value.size()-2) + 
                        secondToken.value.substr(1, secondToken.value.size()-2);
    concatValue = "'" + concatValue + "'";
    Token newToken(concatValue, Lexer::STR);
    executionStack.push(newToken);
    controlStack.pop();
}

void CSEMachine::handleTypeFunctions(string funcName, stack<Token> &executionStack) {
    Token t = executionStack.top(); executionStack.pop();
    
    switch (funcName[1]) {
        case 't': // ItoS
            if (funcName == "ItoS" || funcName == "itos") {
                t.type = Lexer::STR;
                t.value = "'" + t.value + "'";
                executionStack.push(t);
            }
            break;
        case 's': // Isinteger, Isstring, etc.
            switch (funcName[2]) {
                case 'i': // Isinteger
                    executionStack.push(t.type == Lexer::INT ? Token("true","true") : Token("false","false"));
                    break;
                case 't': // Istruthvalue, Istuple
                    if (funcName == "Istruthvalue") {
                        executionStack.push(t.value == "true" || t.value == "false" ? Token("true","true") : Token("false","false"));
                    } else if (funcName == "Istuple") {
                        executionStack.push(t.isTuple == true ? Token("true","true") : Token("false","false"));
                    }
                    break;
                case 's': // Isstring
                    executionStack.push(t.type == Lexer::STR ? Token("true","true") : Token("false","false"));
                    break;
                case 'd': // Isdummy
                    executionStack.push(t.value == "dummy" ? Token("true","true") : Token("false","false"));
                    break;
                case 'f': // Isfunction
                    executionStack.push(t.type == "lambdaClosure" ? Token("true","true") : Token("false","false"));
                    break;
            }
            break;
    }
}

void CSEMachine::handlePrintFunction(stack<Token> &executionStack) {
    printCalled = true;
    Token t = executionStack.top(); executionStack.pop();
    
    if(!t.isTuple) {
        switch (t.type[0]) {
            case 'S': // STR
                if (t.type == Lexer::STR) {
                    string tempStr = processEscapeSequences(t.value.substr(1, t.value.size()-2));
                    cout << tempStr;
                    if(tempStr[tempStr.size()-1] == '\n')
                        cout << endl;
                }
                break;
            case 'l': // lambdaClosure
                if (t.type == "lambdaClosure") {
                    cout << "[lambda closure: " << t.lambdaParam << ": " << t.lambdaNum << "]";
                }
                break;
            default:
                cout << t.value;
                break;
        }
        Token dummyToken("dummy","dummy");
        executionStack.push(dummyToken);
    } else {
        displayTupleStructure(t);
    }
}

void CSEMachine::handleOrderFunction(stack<Token> &executionStack) {
    Token t = executionStack.top(); executionStack.pop();
    executionStack.push(Token(convertIntToString(t.tuple.size()), Lexer::INT));
}

void CSEMachine::handleNullFunction(stack<Token> &executionStack) {
    Token t = executionStack.top(); executionStack.pop();
    executionStack.push((t.value == "nil") ? Token("true","true") : Token("false","false"));
}

void CSEMachine::handleTupleIndexing(Token topExeToken, stack<Token> &executionStack) {
    if(topExeToken.isTuple == true) {
        Token t = executionStack.top(); executionStack.pop();
        if(t.type == Lexer::INT) {
            int indx = atoi(t.value.c_str());
            indx -= 1;
            executionStack.push(topExeToken.tuple[indx]);
        }
    }
}