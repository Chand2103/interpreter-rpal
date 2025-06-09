/**
 * Token Implementation File - Token Class Methods
 * 
 * This implementation provides the Token class functionality for representing
 * various types of tokens in functional programming language evaluation.
 * The class supports multiple constructor patterns for different token types
 * including simple values, lambda closures, environments, and conditional constructs.
 */

#include "Token.h"
#include <vector>

using namespace std;

/**
 * Initialize tuple vector - sets up internal data structure
 * This method initializes the tuple vector container for the token
 */
void Token::construct() {
    this->tuple = vector<Token>();
}

/**
 * Default constructor - initializes empty token
 * Creates a basic token with default values and tuple initialization
 */
Token::Token() {
    isTuple = false;
    construct();
}

/**
 * Lambda closure constructor - creates token for lambda expressions
 * @param type - token type identifier
 * @param lambdaParam - parameter name for lambda function
 * @param lambdaNum - unique identifier number for lambda
 */
Token::Token(std::string type, std::string lambdaParam, int lambdaNum) {
    this->type = type;
    this->lambdaParam = lambdaParam;
    this->lambdaNum = lambdaNum;
    isTuple = false;
    construct();
}

/**
 * Environment constructor - creates token for environment context
 * @param type - token type identifier  
 * @param envNum - unique environment identifier number
 */
Token::Token(std::string type, int envNum) {
    this->type = type;
    this->envNum = envNum;
    isTuple = false;
    construct();
}

/**
 * Simple token constructor - creates basic value token
 * @param value - actual content/value of the token
 * @param type - token type classification
 */
Token::Token(std::string value, std::string type) {
    this->value = value;
    this->type = type;
    isTuple = false;
    construct();
}

/**
 * Conditional constructor - creates token for beta reduction
 * @param type - token type identifier
 * @param betaIfDeltaNum - delta number for if branch
 * @param betaElseDeltaNum - delta number for else branch  
 */
Token::Token(std::string type, int betaIfDeltaNum, int betaElseDeltaNum) {
    this->type = type;
    this->betaIfDeltaNum = betaIfDeltaNum;
    this->betaElseDeltaNum = betaElseDeltaNum;
    isTuple = false;
    construct();
}

/**
 * Destructor - cleans up token resources
 * Virtual destructor for proper cleanup in inheritance scenarios
 */
Token::~Token() {
    // Token cleanup handled automatically by vector destructor
}