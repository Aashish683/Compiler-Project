/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "interface.h"
#include "parserDef.h"

/**Grammar Functions**/
int initialiseGrammar();
Grammar* extractGrammar();
FirstAndFollow* computeFirstAndFollowSets (Grammar* g);
void populateFirst(int** firstVector, Grammar* g);
void calculateFirst(int** firstVector, int enum_id);
void populateFollow(int** followBitVector, int ** firstSet, Grammar* g);
void populateFollowTillStable(int** followVector, int** firstVector, Grammar* g);
ParsingTable* initialiseParsingTable();
void createParseTable(FirstAndFollow* fafl, ParsingTable* pt);
ParseTree* parseInputSourceCode(char *testcaseFile, ParsingTable* pTable,FirstAndFollow* fafl);
void printParseTree(ParseTree* pt, char* outfile);
void printParseTreeHelper(NaryTreeNode * pt, FILE* f);

// Function to initialise checkIfDone global variable
void initialiseCheckIfDone();

/**Functions to map the string to it's enum id**/
int findInNonTerminalMap(char* str);
int findInTerminalMap(char* str);

/**Rule and Rules function**/
NonTerminalRuleRecords** intialiseNonTerminalRecords();
Rule* initialiseRule(SymbolList* sl, int ruleCount);

/**Symbol and SymbolList Functions**/
Symbol* intialiseSymbol(char* symbol);
SymbolList* initialiseSymbolList();
void addToSymbolList(SymbolList* ls, Symbol* s);

/*Utility functions*/
char* getTerminal(int enumId);
char* getNonTerminal(int enumId);
char* appendToSymbol(char* str, char c);
char* copyLexeme(char* str);

/**Utility functions to print**/
void printSymbol(Symbol* ls);
void printRule(Rule* r);
void printGrammarStructure();
void printNonTerminalRuleRecords();
void printFirstSets(FirstAndFollow* fafl);
void printFollowSets(FirstAndFollow* fafl);
void printParseTable(ParsingTable* pt);
int getErrorStatus();
