/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "semantic_analyzerDef.h"

void semanticAnalysis(AST* ast);

void captureErrors(AST* ast, ErrorList* els);
void captureErrorsHelper(ASTNode* node, ErrorList* els);

TokenListItem* initializeTokenListItem(Token* tk);
TokenListItem* getConditionals(ASTNode* astBooleanExpressionNode);
TokenListItem* mergeConditionals(TokenListItem* leftConditionals, TokenListItem* rightConditionals);
int searchConditionals(TokenListItem* tls, Token* tk);
int searchIterativeChildren(ASTNode* astIterativeStmtNode, TokenListItem* conditionals);
TokenListItem* mergeConditionals(TokenListItem* leftConditionals, TokenListItem* rightConditionals);
