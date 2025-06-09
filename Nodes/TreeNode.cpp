/**
  Binary Tree Node Implementation
  
  This code implements a generic tree node structure for building binary trees.
  Each node contains a Token value and pointers to left and right children, along with
  a next pointer for potential linking operations. The implementation provides proper
  memory management through recursive deletion of child nodes.
 */

#include "TreeNode.h"

/**
 * Parameterized constructor - initializes node with given token value
 * Sets all child pointers to NULL for safety
 */
TreeNode::TreeNode(Token token){
	this->value = token;                // Store the token value in this node
	right = NULL;                       // Initialize right child pointer to NULL
	left = NULL;                        // Initialize left child pointer to NULL
}

/**
 * Default constructor - creates empty node with null pointers
 * Used when node value will be set later
 */
TreeNode::TreeNode() {
	right = NULL;                       // Initialize right child pointer to NULL
	left = NULL;                        // Initialize left child pointer to NULL
}

/**
 * Destructor - performs recursive cleanup of all child nodes
 * Uses helper function to safely delete children
 */
TreeNode::~TreeNode() {
	cleanupNode(right);                 // Clean up right subtree recursively
	cleanupNode(left);                  // Clean up left subtree recursively
}

/**
 * Memory cleanup helper - safely deletes node using switch statement
 * Avoids memory leaks by checking for null pointers before deletion
 */
void TreeNode::cleanupNode(TreeNode* node) {
	switch(node != NULL ? 1 : 0) {      // Convert null check to switch-compatible integer
		case 1:                         // Node exists - safe to delete
			delete node;                // Recursively deletes node and its children
			break;
		case 0:                         // Node is NULL
		default:                        // Default case for safety
			break;                      // No action needed for null pointers
	}
}