/**
 * Abstract Syntax Tree Standardizer Implementation
 * 
 * This implementation provides standardization transformations for functional
 * programming language constructs. The standardizer converts various syntactic
 * forms into their canonical representations using lambda calculus principles.
 * It processes abstract syntax trees by applying transformation rules that
 * eliminate syntactic sugar and convert expressions to standard forms.
 */

#include "Standardizer.h"
#include <iostream>
#include <queue>
#include <stack>

using namespace std;

void displayTreeNodes(TreeNode* node);

TreeStandardizer::TreeStandardizer() {
    this->gammaToken = Token("gamma", "gamma");
    this->lambdaToken = Token("lambda", "lambda");
}

TreeStandardizer::~TreeStandardizer() {
}

/**
 * Main tree standardization method - processes entire tree recursively
 * Traverses the tree and applies appropriate transformation rules
 * based on the node type encountered
 */
TreeNode* TreeStandardizer::standardizeTree(TreeNode* rootNode) {
    TreeNode* currentRoot = rootNode;
    TreeNode* nextChild = currentRoot->left;
    bool isFirstChild = true;
    
    while(nextChild != NULL) {
        TreeNode* transformedChild = standardizeTree(nextChild);
        
        if(isFirstChild == true) {
            currentRoot->left = transformedChild;
            currentRoot = currentRoot->left;
            isFirstChild = false;
        } else {
            currentRoot->right = transformedChild;
            currentRoot = currentRoot->right;
        }
        nextChild = nextChild->right;
    }
    
    Token nodeToken = rootNode->value;
    string tokenValue = nodeToken.value;
    
    switch(tokenValue[0]) {
        case 'l':
            if(tokenValue == "let") {
                rootNode = processLetExpression(rootNode);
            } else if(tokenValue == "lambda") {
                rootNode = processLambdaExpression(rootNode);
            }
            break;
        case 'w':
            if(tokenValue == "where") {
                rootNode = processWhereExpression(rootNode);
            } else if(tokenValue == "within") {
                rootNode = processWithinExpression(rootNode);
            }
            break;
        case 'f':
            if(tokenValue == "function_form") {
                rootNode = processFunctionForm(rootNode);
            }
            break;
        case 'a':
            if(tokenValue == "and") {
                rootNode = processAndExpression(rootNode);
            } else if(tokenValue == "@") {
                rootNode = processAtExpression(rootNode);
            }
            break;
        case 'r':
            if(tokenValue == "rec") {
                rootNode = processRecExpression(rootNode);
            }
            break;
    }
    
    return rootNode;
}

/**
 * Transforms LET expressions into standard lambda-gamma form
 * let X = E1 in E2 becomes gamma(lambda X.E2)(E1)
 */
TreeNode* TreeStandardizer::processLetExpression(TreeNode* letNode) {
    TreeNode* lambdaNode = new TreeNode(this->lambdaToken);
    TreeNode* gammaNode = new TreeNode(this->gammaToken);
    
    gammaNode->left = lambdaNode;
    lambdaNode->right = letNode->left->left->right;
    lambdaNode->left = letNode->left->left;
    lambdaNode->left->right = letNode->left->right;
    
    return gammaNode;
}

/**
 * Transforms WHERE expressions into standard lambda-gamma form
 * E1 where X = E2 becomes gamma(lambda X.E1)(E2)
 */
TreeNode* TreeStandardizer::processWhereExpression(TreeNode* whereNode) {
    TreeNode* lambdaNode = new TreeNode(this->lambdaToken);
    TreeNode* gammaNode = new TreeNode(this->gammaToken);
    
    gammaNode->left = lambdaNode;
    gammaNode->left->right = whereNode->left->right->left->right;
    gammaNode->left->left = whereNode->left->right->left;
    gammaNode->left->left->right = whereNode->left;
    gammaNode->left->left->right->right = NULL;
    
    return gammaNode;
}

/**
 * Transforms multi-parameter lambda expressions into curried form
 * lambda X1 X2 ... Xn.E becomes lambda X1.(lambda X2.(...(lambda Xn.E)...))
 */
TreeNode* TreeStandardizer::processLambdaExpression(TreeNode* lambdaNode) {
    if(lambdaNode->left->right->right == NULL)
        return lambdaNode;
    
    stack<TreeNode*> nodeStack;
    TreeNode* currentChild = lambdaNode->left;
    
    while(currentChild != NULL) {
        nodeStack.push(currentChild);
        TreeNode* nextChild = currentChild->right;
        currentChild->right = NULL;
        currentChild = nextChild;
    }
    
    while(nodeStack.size() != 1) {
        TreeNode* rightChild = nodeStack.top();
        nodeStack.pop();
        TreeNode* leftChild = nodeStack.top();
        nodeStack.pop();
        TreeNode* newLambdaNode = new TreeNode(this->lambdaToken);
        newLambdaNode->left = leftChild;
        leftChild->right = rightChild;
        nodeStack.push(newLambdaNode);
    }
    
    TreeNode* resultNode = nodeStack.top();
    nodeStack.pop();
    return resultNode;
}

/**
 * Transforms function form declarations into standard lambda form
 * f X1 X2 ... Xn = E becomes f = lambda X1.(lambda X2.(...(lambda Xn.E)...))
 */
TreeNode* TreeStandardizer::processFunctionForm(TreeNode* functionNode) {
    Token equalToken("=", "=");
    TreeNode* equalNode = new TreeNode(equalToken);
    int parameterCount = 0;
    stack<TreeNode*> nodeStack;
    
    nodeStack.push(functionNode->left);
    TreeNode* currentChild = functionNode->left->right;
    
    while(currentChild != NULL) {
        parameterCount++;
        nodeStack.push(currentChild);
        currentChild = currentChild->right;
    }
    
    parameterCount = parameterCount - 1;
    
    while(parameterCount != 0) {
        TreeNode* firstNode = nodeStack.top();
        nodeStack.pop();
        TreeNode* secondNode = nodeStack.top();
        nodeStack.pop();
        
        secondNode->right = firstNode;
        TreeNode* lambdaNode = new TreeNode(this->lambdaToken);
        lambdaNode->left = secondNode;
        nodeStack.push(lambdaNode);
        parameterCount--;
    }
    
    TreeNode* firstNode = nodeStack.top();
    nodeStack.pop();
    TreeNode* secondNode = nodeStack.top();
    nodeStack.pop();
    
    secondNode->right = firstNode;
    equalNode->left = secondNode;
    
    return equalNode;
}

/**
 * Transforms WITHIN expressions into nested lambda-gamma structures
 * E1 within X2 = E2 becomes X2 = gamma(lambda X1.E2)(E1)
 */
TreeNode* TreeStandardizer::processWithinExpression(TreeNode* withinNode) {
    Token equalToken("=", "=");
    TreeNode* equalNode = new TreeNode(equalToken);
    TreeNode* gammaNode = new TreeNode(this->gammaToken);
    TreeNode* lambdaNode = new TreeNode(this->lambdaToken);
    
    gammaNode->left = lambdaNode;
    lambdaNode->right = withinNode->left->left->right;
    lambdaNode->left = withinNode->left->left;
    lambdaNode->left->right = withinNode->left->right->left->right;
    equalNode->left = withinNode->left->right->left;
    equalNode->left->right = gammaNode;
    
    return equalNode;
}

/**
 * Transforms AT expressions (@) into gamma applications
 * E1 @ E2 E3 becomes gamma(gamma E2 E1) E3
 */
TreeNode* TreeStandardizer::processAtExpression(TreeNode* atNode) {
    TreeNode* gammaNode1 = new TreeNode(this->gammaToken);
    TreeNode* gammaNode2 = new TreeNode(this->gammaToken);
    
    gammaNode1->left = gammaNode2;
    TreeNode* e2 = atNode->left->right->right;
    TreeNode* n = atNode->left->right;
    n->right = NULL;
    TreeNode* e1 = atNode->left;
    e1->right = NULL;
    gammaNode2->left = n;
    n->right = e1;
    gammaNode2->right = e2;
    
    return gammaNode1;
}

/**
 * Transforms AND expressions into simultaneous binding structures
 * Converts multiple bindings into comma-separated tuples with tau
 */
TreeNode* TreeStandardizer::processAndExpression(TreeNode* andNode) {
    Token equalToken("=", "=");
    TreeNode* equalNode = new TreeNode(equalToken);
    Token commaToken(",", ",");
    TreeNode* commaNode = new TreeNode(commaToken);
    Token tauToken("tau", "tau");
    TreeNode* tauNode = new TreeNode();
    
    equalNode->left = commaNode;
    commaNode->right = tauNode;
    TreeNode* andChild = andNode->left;
    queue<TreeNode*> equalQueue;
    int tauCount = 0;
    
    while(andChild != NULL) {
        equalQueue.push(andChild);
        andChild = andChild->right;
        tauCount++;
    }
    
    tauToken.tauCount = tauCount;
    tauNode->value = tauToken;
    TreeNode* currentParam = NULL;
    TreeNode* currentValue = NULL;
    
    while(!equalQueue.empty()) {
        TreeNode* eqNode = equalQueue.front();
        equalQueue.pop();
        TreeNode* param = eqNode->left;
        TreeNode* value = eqNode->left->right;
        param->right = NULL;
        
        if(currentParam == NULL) {
            currentParam = param;
            currentValue = value;
            commaNode->left = currentParam;
            tauNode->left = currentValue;
        } else {
            currentParam->right = param;
            currentValue->right = value;
            currentParam = param;
            currentValue = value;
        }
    }
    
    return equalNode;
}

/**
 * Transforms REC expressions (recursive definitions) using Y combinator
 * rec X = E becomes X = YSTAR(lambda X.E)
 */
TreeNode* TreeStandardizer::processRecExpression(TreeNode* recNode) {
    Token equalToken("=", "=");
    TreeNode* equalNode = new TreeNode(equalToken);
    TreeNode* gammaNode = new TreeNode(this->gammaToken);
    TreeNode* lambdaNode = new TreeNode(this->lambdaToken);
    Token ystarToken("YSTAR", "YSTAR");
    TreeNode* ystarNode = new TreeNode(ystarToken);
    
    TreeNode* expression = recNode->left->left->right;
    TreeNode* variable1 = recNode->left->left;
    variable1->right = NULL;
    TreeNode* variable2 = new TreeNode(variable1->value);
    
    equalNode->left = variable1;
    variable1->right = gammaNode;
    gammaNode->left = ystarNode;
    ystarNode->right = lambdaNode;
    lambdaNode->left = variable2;
    variable2->right = expression;
    
    return equalNode;
}

/**
 * Creates a copy of a tree node with the same value
 */
TreeNode* TreeStandardizer::createNodeCopy(TreeNode* sourceNode) {
    return new TreeNode(sourceNode->value);
}

/**
 * Utility function to display tree nodes recursively
 * Used for debugging and visualization purposes
 */
void displayTreeNodes(TreeNode* node) {
    cout << node->value.value << endl;
    if(node->left != NULL)
        displayTreeNodes(node->left);
    if(node->right != NULL)
        displayTreeNodes(node->right);
}