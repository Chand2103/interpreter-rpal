/**
 * Token Header File - Token Class Definition
 * 
 * This header defines the Token class which represents various types of tokens
 * used in functional programming language evaluation. The Token class supports
 * different token types including simple values, lambda closures, environments,
 * and conditional constructs, providing a flexible structure for language processing.
 */

#include <string>
#include <vector>

#ifndef TOKEN_H_
#define TOKEN_H_

class Token {
public:
    /**
     * Default constructor - initializes empty token
     */
    Token();
    
    /**
     * Lambda closure constructor - creates token for lambda expressions
     * @param type - token type identifier
     * @param lamdaParam - parameter name for lambda
     * @param lamdaNum - lambda identifier number
     */
    Token(std::string type, std::string lamdaParam, int lamdaNum);
    
    /**
     * Environment constructor - creates token for environment context
     * @param type - token type identifier
     * @param envNum - environment identifier number
     */
    Token(std::string type, int envNum);
    
    /**
     * Simple token constructor - creates basic value token
     * @param value - token value content
     * @param type - token type identifier
     */
    Token(std::string value, std::string type);
    
    /**
     * Conditional constructor - creates token for beta reduction
     * @param type - token type identifier
     * @param betaIfDeltaNum - if branch delta number
     * @param betaElseDeltaNum - else branch delta number
     */
    Token(std::string type, int betaIfDeltaNum, int betaElseDeltaNum);
    
    /**
     * Destructor - cleans up token resources
     */
    virtual ~Token();
    
    /**
     * Initialize tuple vector - sets up internal data structure
     */
    void construct();
    
    // Token data members
    std::string value;              // Token value content
    std::string type;               // Token type identifier
    int lambdaNum;                  // Lambda identifier number
    std::string lambdaParam;        // Lambda parameter name
    int envNum;                     // Environment identifier
    int betaIfDeltaNum;            // Beta if branch number
    int betaElseDeltaNum;          // Beta else branch number
    int tauCount;                   // Tau count for tuples
    bool isTuple;                   // Tuple flag indicator
    std::vector<Token> tuple;       // Tuple container
    int lambdaEnv;                  // Lambda environment reference

};

#endif /* TOKEN_H_ */