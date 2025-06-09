/**
 * Abstract Syntax Tree Standardizer Header
 * 
 * This header defines the TreeStandardizer class which implements
 * standardization transformations for functional programming language constructs.
 * It converts various syntactic forms (let, where, lambda, etc.) into their
 * standard representations using gamma and lambda abstractions.
 * The standardizer processes abstract syntax trees by applying specific
 * transformation rules to each node type.
 */

#ifndef TREESTANDARDIZER_H_
#define TREESTANDARDIZER_H_

#include "TreeNode.h"

void displayTreeNodes(TreeNode* node);

class TreeStandardizer {
private:
    TreeNode* createNodeCopy(TreeNode* sourceNode);
    
public:
    TreeStandardizer();
    virtual ~TreeStandardizer();
    
    /**
     * Main transformation entry point - processes entire tree recursively
     * Applies appropriate standardization rules based on node type
     */
    TreeNode* standardizeTree(TreeNode* rootNode);
    
    /**
     * Transformation methods for specific language constructs
     * Each method handles a particular syntactic form
     */
    TreeNode* processLetExpression(TreeNode* letNode);
    TreeNode* processWhereExpression(TreeNode* whereNode);
    TreeNode* processLambdaExpression(TreeNode* lambdaNode);
    TreeNode* processFunctionForm(TreeNode* functionNode);
    TreeNode* processWithinExpression(TreeNode* withinNode);
    TreeNode* processAtExpression(TreeNode* atNode);
    TreeNode* processAndExpression(TreeNode* andNode);
    TreeNode* processRecExpression(TreeNode* recNode);
    
    Token lambdaToken;
    Token gammaToken;
};

#endif /* TREESTANDARDIZER_H_ */