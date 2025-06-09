/**
 * Control Stack Environment (CSE) Machine Implementation Header
 * 
 * This header defines the CSE machine class which is an abstract machine
 * for evaluating functional programming languages. The machine processes expressions by maintaining
 * three components: a control stack (expressions to evaluate), a value stack (computed results), 
 * and environment chain (variable bindings), executing transition rules until the final result is computed.
 */

#ifndef CSEMACHINE_H_
#define CSEMACHINE_H_

#include <map>
#include <stack>
#include "Token.h"
#include "TreeNode.h"
#include <list>
#include <vector>
#include <queue>
#include <sstream>
#include <utility>

using namespace std;
typedef map<string,Token> envMap;
typedef pair<int,string> keyPair;

class CSEMachine {
public:
	typedef std::pair<int, std::string> key_pair;
	
	/**
	 * Default constructor for CSE machine
	 */
	CSEMachine();
	
	/**
	 * Constructor with input tree for CSE machine
	 * @param input - Root node of the abstract syntax tree to evaluate
	 */
	CSEMachine(TreeNode* input);
	
	/**
	 * Destructor for CSE machine
	 */
	virtual ~CSEMachine();
	
	/**
	 * Main evaluation method - starts the CSE machine execution
	 * Processes the input tree and produces the final result
	 */
	void evaluateTree();
	
private:
	/**
	 * Core data structures for CSE machine operation
	 */
	map<int, vector<Token> > deltaMap;          // Control structures storage
	ostringstream oss;                          // Output string stream
	int deltaCounter;                           // Counter for delta numbering
	int currDeltaNum;                          // Current delta number being processed
	int envCounter;                            // Environment counter
	queue<TreeNode*> pendingDeltaQueue;        // Queue for pending delta processing
	TreeNode* inputTree;                       // Input abstract syntax tree
	map<keyPair,Token> paramMap;               // Parameter mapping storage
	map<int,int> envMap;                       // Environment mapping
	stack<int> envStack;                       // Environment stack
	int currEnv;                               // Current environment identifier
	bool printCalled;                          // Flag to track print function calls
	
	/**
	 * Primary control structure creation method
	 * Builds the delta control structures from the input tree
	 * @param root - Root node of the tree to process
	 */
	void createControlStructures(TreeNode* root);
	
	/**
	 * Tree traversal method for building control structures
	 * Performs pre-order traversal to create delta sequences
	 * @param root - Current node being processed
	 * @param currentDelta - Reference to current delta being built
	 */
	void buildDeltaSequence(TreeNode* root, vector<Token> &currentDelta);
	
	/**
	 * Core token processing method
	 * Handles individual token evaluation based on CSE machine rules
	 * @param currToken - Token currently being processed
	 * @param controlStack - Reference to control stack
	 * @param executionStack - Reference to execution stack
	 */
	void executeTokenTransition(Token &currToken, stack<Token> &controlStack, stack<Token> &executionStack);
	
	/**
	 * Arithmetic and logical operator application method
	 * Applies binary operators to operands
	 * @param firstToken - First operand
	 * @param secondToken - Second operand
	 * @param currToken - Operator token
	 * @return Result token after operation
	 */
	Token performOperation(Token firstToken, Token secondToken, Token currToken);
	
	/**
	 * Utility method for integer to string conversion
	 * @param intValue - Integer value to convert
	 * @return String representation of the integer
	 */
	string convertIntToString(int intValue);
	
	/**
	 * String tokenization utility method
	 * Splits string by delimiter into vector of strings
	 * @param inputString - String to split
	 * @param delimiter - Character delimiter
	 * @return Vector of split string tokens
	 */
	vector<string> tokenizeString(string inputString, char delimiter);
	
	/**
	 * Built-in function identification method
	 * Checks if identifier is not a built-in function
	 * @param value - Identifier value to check
	 * @return True if not a built-in function, false otherwise
	 */
	bool isUserDefinedFunction(string value);
	
	/**
	 * Parameter resolution method
	 * Checks if token represents a parameter in current environment
	 * @param currToken - Token to check
	 * @return True if token is a parameter, false otherwise
	 */
	bool resolveParameterStatus(Token currToken);
	
	/**
	 * String unescaping utility method
	 * Processes escape sequences in strings
	 * @param s - Input string with escape sequences
	 * @return Processed string with escape sequences resolved
	 */
	string processEscapeSequences(const string& s);
	
	/**
	 * Tuple printing utility method
	 * Handles formatted output of tuple structures
	 * @param t - Token representing tuple to print
	 */
	void displayTupleStructure(Token t);

	// 🔽 Added missing method declarations below 🔽

	void handleGammaOperation(stack<Token> &controlStack, stack<Token> &executionStack);
	void handleLambdaApplication(Token topExeToken, stack<Token> &controlStack, stack<Token> &executionStack);
	void handleBuiltInFunctions(Token topExeToken, stack<Token> &controlStack, stack<Token> &executionStack);
	void handleSpecialValueOperations(Token &currToken, stack<Token> &controlStack, stack<Token> &executionStack);
	void handleStringFunctions(string funcName, stack<Token> &executionStack);
	void handleConcatenation(stack<Token> &controlStack, stack<Token> &executionStack);
	void handleTypeFunctions(string funcName, stack<Token> &executionStack);
	void handlePrintFunction(stack<Token> &executionStack);
	void handleOrderFunction(stack<Token> &executionStack);
	void handleNullFunction(stack<Token> &executionStack);
	void handleTupleIndexing(Token topExeToken, stack<Token> &executionStack);
};

#endif /* CSEMACHINE_H_ */
