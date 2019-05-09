/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "keyword_tableDef.h"

// Functions for keyword hashtable
int hashFunction(char* str);
void addEntry(KeywordTable* kt,TokenName tn, char* lexeme);
Node* lookUp(KeywordTable* kt,char* lexeme);
KeywordTable* initializeTable();
void printHashTable(KeywordTable* kt);

// Functions for List
Node* addToList(Node* ls, TokenName tn, char* lexeme);
int searchList(Node* ls, char* lexeme);
int printList(Node* ls);
