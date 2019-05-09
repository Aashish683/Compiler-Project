/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "nary_treeDef.h"

ParseTree* initialiseParseTree();
NaryTreeNode* createLeafNode(int enumId);
NaryTreeNode* createNonLeafNode(int enumId);
NaryTreeNode* createNode(int isTerminal, SymbolType type,NaryTreeNode* parent);
void addRuleToParseTree(NaryTreeNode* ntn, Rule* r);
void printTree(ParseTree* pt);
void printNaryTree(NaryTreeNode* nt);

int getParseTreeNodeCount();
int getParseTreeMemory();
