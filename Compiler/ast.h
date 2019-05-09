/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "astDef.h"
#include "nary_treeDef.h"

char* getLabel(int l);

ASTNode* createASTNode(int isLeaf, Label l);
void addASTChild(ASTNode* node, ASTNode* newChild);
void addASTChildren(ASTNode* node, ASTNode* ls);

int isGlobal(NaryTreeNode* parseTreeNode);
int isSingleOrRecord(NaryTreeNode* parseTreeNode);
Token* getType(NaryTreeNode* parseTreeNode);
Token* getIdentifier(NaryTreeNode* parseTreeNode);
Token* getOperator(NaryTreeNode* parseTreeNode);

AST* initializeAST();
AST* constructAST(ParseTree* pt);
ASTNode* constructASTHelper(NaryTreeNode* parseTreeNode);
ASTNode* constructASTHelperInherited(NaryTreeNode* parseTreeNode, ASTNode* inherited);
void printAST(ASTNode* astNode);

int getASTNodeCount();
int getASTMemory();
