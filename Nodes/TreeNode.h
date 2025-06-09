/**
  Binary Tree Node Implementation
  
  This code implements a generic tree node structure for building binary trees.
  Each node contains a Token value and pointers to left and right children, along with
  a next pointer for potential linking operations. The implementation provides proper
  memory management through recursive deletion of child nodes.
 */

#ifndef TREENODE_H_
#define TREENODE_H_

#include "Token.h"

class TreeNode {
public:
	TreeNode(Token token);              // Parameterized constructor - creates node with given token
	TreeNode();                         // Default constructor - creates empty node
	virtual ~TreeNode();                // Destructor - handles recursive cleanup of child nodes
	
	Token value;                        // Node data - contains the token value
	TreeNode* right;                    // Right child pointer - points to right subtree
	TreeNode* left;                     // Left child pointer - points to left subtree
	TreeNode* next;                     // Next node pointer - for linking operations
	
private:
	void cleanupNode(TreeNode* node);   // Memory cleanup helper - safely deletes node if not null
};

#endif /* TREENODE_H_ */