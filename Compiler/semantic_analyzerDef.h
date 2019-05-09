/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#ifndef SEMANTIC_ANALYZER_
#define SEMANTIC_ANALYZER_

#include "lexerDef.h"
// We work with tokens for type checking
// Actually we use the TOKEN_NAME field inside to compare
// Tokens chosen because they directly feed the line number.

// A structure which is used to form a linked list of tokens
typedef struct TokenListItem {
    Token* TK;
    struct TokenListItem* next;
} TokenListItem;

#endif
