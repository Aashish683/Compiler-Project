/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "code_genDef.h"


void printLeaf(ASTNode* leafNode,FILE* f);

void codeGenerationHelper(ASTNode* node, SymbolTable* st, FILE* f,int recordArithmetic); // recordArithmetic(1) indicates a record operation
void codeGeneration(AST* ast, SymbolTable* st, FILE* f);
