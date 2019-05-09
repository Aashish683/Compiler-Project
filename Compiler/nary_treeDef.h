/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#ifndef NARY_
#define NARY_

#include "lexerDef.h"
#include "parserDef.h"
#include "astDef.h"

typedef struct NaryTreeNode NaryTreeNode;

typedef struct NonLeafNode {
    int ENUM_ID; // The enum identifier of the symbol
    int NUMBER_CHILDREN; // Number of children for this non terminal
    int RULE_NO; // Rule number used to generate children
    NaryTreeNode* child; // Points to the starting child of this node
} NonLeafNode;

typedef struct LeafNode {
    int ENUM_ID;
    Token* TK; // This field will be populated when the input is being parsed
} LeafNode;

typedef union NodeType {
    NonLeafNode NL;
    LeafNode L;
} NodeType;

typedef struct NaryTreeNode {
    NodeType NODE_TYPE;
    int IS_LEAF_NODE;
    struct NaryTreeNode* parent;
    struct NaryTreeNode* next; // Points to the next node of the list which this node is a part of

} NaryTreeNode;

typedef struct ParseTree {
    NaryTreeNode* root;
} ParseTree;

#endif
