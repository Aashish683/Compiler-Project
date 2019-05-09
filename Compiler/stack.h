/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "stackDef.h"

StackNode* createStackNode(NaryTreeNode* ntn);

// Stack operations
void push(Stack* st,NaryTreeNode* ntn);
NaryTreeNode* top(Stack* st);
void pop(Stack* st);
Stack* initialiseStack(ParseTree* pt);
void pushTreeChildren(Stack* st,NaryTreeNode* ntn);
