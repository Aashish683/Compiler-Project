/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#ifndef SYMBOL_TABLE_DEF_
#define SYMBOL_TABLE_DEF_

#include "lexerDef.h"

#define SYMBOL_TABLE_SLOTS 20 // This is a number which we use as a default, a function is present to initialize number of slots as per our choice

typedef char* Scope; // We shall use a string for referring to a scope

typedef struct SymbolEntry SymbolEntry;
typedef struct SymbolTable SymbolTable;

/**
 * Plan
 * The starting symbol table will be the global symbol table, responsible for storing the global variable, type definitions and function names
 * In case of a function, the table entry will be scoped into another table
 * This scoped table of the function will have input/output parameters, the variables defined inside the function and so on
 *
*/

typedef enum SymbolLabel {
    symbolFunction,
    symbolVariable,
    symbolRecord,
    symbolParameter
} SymbolLabel;

typedef struct SymbolTable {
    SymbolEntry** SYMBOL_SLOTS; // A dynamic array of Symbol Slots, each slot stores the head of the linked list of symbols hashed there
    Scope SCOPE; // Scope of the current table
    int NUMBER_SLOTS; // Number of slots
    int CURRENT_OFFSET; // The next entry will be allocated this number, which will then be incremented. (Is also the count of elements in the table)
    SymbolTable* parent; // Parent table of this symbol table (Needed for functions to find if an entry is available in the parent table eg- global entries and type definition)
} SymbolTable;

typedef struct FunctionEntry {

    // This points to a symbol table for that particular function
    // Note that input params being taken to this function must be installed in this symbol table

    // If a parameter is of record type defined in the caller, it must be valid in this table
    // The function's scoped symbol table can access the typeDefinition in the original table (Implement via pointers)
    struct SymbolTable* SCOPED_TABLE;
    Token** INPUT_TYPES; // Stores the data types of input params
    int NUMBER_INPUT_PARAMS; // Stores the number of input params
    Token** OUTPUT_TYPES; // Stores the data types of output params
    int NUMBER_OUTPUT_PARAMS; // Stores the number of output params
} FunctionEntry;

// Note that all global variables should be in the global symbol table
typedef struct VariableEntry {
    Token* DATA_TYPE;
} VariableEntry;

typedef struct RecordEntry {
    Token** DATA_TYPE;
    Token** FIELDS; // The data type in a cartesian format, size based on the number of fields
    int TOTAL_OFFSET;
    int NUMBER_FIELDS; // The number of fields in the data type
} RecordEntry;

typedef struct ParameterEntry {
    Token* DATA_TYPE;
    int IS_INPUT; // Stores whether the parameter is input (1) or output (0)
} ParameterEntry;

typedef union SymbolEntryType {
    FunctionEntry FUNCTION_ENTRY;
    VariableEntry VARIABLE_ENTRY;
    RecordEntry RECORD_ENTRY;
    ParameterEntry PARAMETER_ENTRY;
} SymbolEntryType;


typedef struct SymbolEntry {

    Token* SYMBOL_TOKEN; // Field to store the symbol
    int SYMBOL_OFFSET; // A unique identifier to denote each entry in a symbol table
    SymbolLabel SYMBOL_LABEL; // A label to indicate type of entry in symbol table
    SymbolEntryType SYMBOL_ENTRY_TYPE; // Stores the type of entry in the table
    struct SymbolEntry* next; // Represents a pointer to an entry hashed to the same position
} SymbolEntry;


#endif
