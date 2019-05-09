/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#ifndef PARSE_DEF_
#define PARSE_DEF_

#include "lexerDef.h"
#include "nary_treeDef.h"

// Enum which stores the Non-terminals
typedef enum NonTerminal {
	program,
    mainFunction,
	otherFunctions,
	function,
	input_par,
	output_par,
	parameter_list,
	dataType,
	primitiveDatatype,
	constructedDatatype,
	remaining_list,
	stmts,
	typeDefinitions,
	typeDefinition,
	fieldDefinitions,
	fieldDefinition,
	moreFields,
	declarations,
	declaration,
	global_or_not,
	otherStmts,
	stmt,
	assignmentStmt,
	singleOrRecId,
    C,
	funCallStmt,
	outputParameters,
	inputParameters,
	iterativeStmt,
	conditionalStmt,
	B,
	ioStmt,
	arithmeticExpression,
	ex2,
	term,
	term2,
	factor,
	op1,
	op2,
	booleanExpression,
    allVar,
	logicalOp,
	relationalOp,
	returnStmt,
	optionalReturn,
	idList,
	more_ids,
} NonTerminal;

typedef TokenName Terminal; // The tokens will be representing the terminals

// Struct to store the first and follow sets of each non terminal
typedef struct FirstAndFollow {
	int** FIRST; // Bit vector to store first sets of each non terminal
 	int** FOLLOW; // Bit vector to store follow sets of each non terminal
} FirstAndFollow;

//UNION has either of the two, so symboltype can be either terminal or non terminal
typedef union SymbolType {
    Terminal TERMINAL; // If the symbol is a terminal
    NonTerminal NON_TERMINAL; // If the symbol is a non terminal
} SymbolType;

typedef struct Symbol {
    SymbolType TYPE; // Stores the type number of the terminal/non-terminal
	int IS_TERMINAL; // Stores whether Symbol is a terminal or not
    struct Symbol* next; // Pointer to the next symbol node in the linked list. A Rule is a linked list and Symbol is a node of that linked list
} Symbol;

//SymbolList stores the head of the linked list and the length of the list i.e the rule.
typedef struct SymbolList {
    Symbol* HEAD_SYMBOL; // Indicates the symbol which represents the start of the rule , the LHS non terminal
	Symbol* TAIL_SYMBOL; // Indicates the symbol at the tail, used for appending symbols
    int RULE_LENGTH; // Stores the length of the rule i.e number of symbols
} SymbolList;

// Struct which stores the start and ending rule number corresponding to the rule indexed in the array by its enum id.
typedef struct NonTerminalRuleRecords {
	int start;
	int end;
} NonTerminalRuleRecords;

// Struct representing a single rule
typedef struct Rule {
    SymbolList* SYMBOLS; // Linked list of symbols (DOUBT => Can make this a dynamically allocated array as well, ask ma'am)
    int RULE_NO; // Rule number
} Rule;

// Struct for the grammar which will be extracted for the txt file
 typedef struct Grammar {
    int GRAMMAR_RULES_SIZE; // Keep track of the size of the array below
    Rule** GRAMMAR_RULES; // An array containg the rules of the grammar
} Grammar;

// Struct for the parsing table
typedef struct ParsingTable {
	int** entries;
} ParsingTable;

#endif
