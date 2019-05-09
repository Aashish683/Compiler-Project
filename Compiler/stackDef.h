/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#ifndef STACK_
#define STACK_
#include "nary_treeDef.h"

typedef struct StackNode {
    NaryTreeNode* TREE_NODE;
    struct StackNode* next;
} StackNode;

typedef struct Stack {
    StackNode* HEAD;
    int NUM_NODES;
} Stack;

#endif
