/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#ifndef AST_
#define AST_

#include "lexerDef.h"
#include "symbol_tableDef.h"

typedef struct ASTNode ASTNode;
typedef struct ASTProgram ASTProgram;
typedef struct ASTInputParams ASTInputParams;
typedef struct ASTOutputParams ASTOutputParams;
typedef struct ASTDatatype ASTDatatype;
typedef struct ASTStmts ASTStmts;
typedef struct ASTTypedefinition ASTTypedefinition;
typedef struct ASTFieldDefinition ASTFieldDefinition;
typedef struct ASTDeclaration ASTDeclaration;
typedef struct ASTAssignmentStmt ASTAssignmentStmt;
typedef struct ASTFunCallStmt ASTFunCallStmt;
typedef struct ASTIterativeStmt ASTIterativeStmt;
typedef struct ASTConditionalStmt ASTConditionalStmt;
typedef struct ASTIOStmt ASTIOStmt;
typedef struct ASTStmt ASTStmt;
typedef struct ASTArithmeticExpression ASTArithmeticExpression;
typedef struct ASTBooleanExpression ASTBooleanExpression;
typedef struct ASTID ASTID;

// Defining some snippets to make coding easier

#define AST_STMT_ITERATIVE AST_STMT.AST_STMT_TYPE.AST_ITERATIVE_STMT
#define AST_STMT_ASSIGNMENT AST_STMT.AST_STMT_TYPE.AST_ASSIGNMENT_STMT
#define AST_STMT_FUN_CALL AST_STMT.AST_STMT_TYPE.AST_FUN_CALL_STMT
#define AST_STMT_CONDITIONAL AST_STMT.AST_STMT_TYPE.AST_CONDITIONAL_STMT
#define AST_STMT_IO AST_STMT.AST_STMT_TYPE.AST_IO_STMT

// A struct which is used to label nodes
typedef enum Label {
    astProgram,
    astFunction,
    astInputParams,
    astOutputParams,
    astDatatype,
    astStmts,
    astTypeDefintion,
    astFieldDefinition,
    astDeclaration,
    astAssignmentStmt,
    astFunCallStmt,
    astIterativeStmt,
    astConditionalStmt,
    astElsePart,
    astIOStmtRead,
    astIOStmtWrite,
    astReturnStmt,
    astInputArgs,
    astOutputArgs,
    astArithmeticExpression,
    astBooleanExpression,
    astId,
    astNum,
    astRnum,
} Label;

/**
 *  -----Plan-------
 *
**/
typedef struct ASTProgram {
    // A field for a linked list of ASTFunction (represents otherFunctions non terminal)
    // A field for the main function
    // struct ASTNode* AST_FUNCTION_HEAD;
    // struct ASTNode* AST_MAIN_FUNCTION;

} ASTProgram;

typedef struct ASTFunction {
    // A field to store the name of the function
    Token* FUNCTION_TOKEN;
    // A field for inputparams // Nullable
    // struct ASTNode* INPUT_PARAMS;
    // A field for outputparams // Non-nullable
    // struct ASTNode* OUTPUT_PARAMS;
    // A linked list of ASTStmt
    // struct ASTNode* STMT_HEAD;
    // A pointer to the next ASTFunction node
} ASTFunction;

/*** Rendered obsolete because of ASTID ***/
typedef struct ASTInputParams {
    // A linked list of two-tuples(<dataType,TK_ID>) represents parameter list non terminal
} ASTInputParams;

typedef struct ASTOutputParams {
    // A linked list of two-tuples(<dataType,TK_ID>) represents parameter list non terminal
} ASTOutputParams;
/******/

// Represents the element of a list of parameters
typedef struct ASTID {
    // Stores datatype of TK_ID
    // Stores TK_ID
    Token* DATA_TYPE;
    Token* ID;
    Token* FIELD_ID;
} ASTID;


typedef struct ASTDatatype {
    // A field which stores the values (INT,REAL or RECORD_ID)
} ASTDatatype;

typedef struct ASTStmts {
    // A linked list of each element being a typedefinition
    // A linked list of each elelemnt being a declaration

    // A linked list of each element being a stmt (AssignmentStmt,iterative,conditional, funCallStmt and IOStmt)
    // Implement this by making a custom function which checks wheter ASTNode has the label accoridng to these 4

   // A pointer to the return stmt ASTNode (which is a linked list of the IDs which we are returning, NULL if none)
} ASTStmts;

typedef struct ASTTypedefinition {
    // A field having a RECORD_ID or a pointer to the RECORD_ID node
    // A linked list of each element being a FieldDefinition, linked list has a size of at least 2
    Token* RECORD_ID;
} ASTTypedefinition;

typedef struct ASTFieldDefinition {
    // A field storing either INT OR REAL, pointer to ASTDatatypr
    // A field storing fieldID
    Token* DATA_TYPE;
    Token* FIELD_ID;
} ASTFieldDefinition;

typedef struct ASTDeclaration {
    // A field storing INT OR REAL OR RECORDID, pointer to ASTDataType
    // A field storing the ID
    // A field storing whether it is global or not
    int IS_GLOBAL;
} ASTDeclaration;

typedef struct ASTReturnStmt {
    int RETURN_LINE_NO; // Required in case nothing is being returned and an error is present, We cannot consult the IDs for the line number as nothing is being returned so no IDs
} ASTReturnStmt;

typedef struct ASTAssignmentStmt {
    // POSSIBLY a flag to store whether a single variable or a record id
    // A fieled to store the TK_ID or TK_ID.TK_FIELDID
    // A field to store the arithmeticExpression

    int SINGLE_OR_RECORD;
} ASTAssignmentStmt;

typedef struct ASTFunCallStmt {
    // A field to store Output parameters (i.e => a linked list of ids idlist)
    // A field to store Input Parameters (i.e => a linked list of ids idlist)
    // A field to store the FUN_ID of the function being called
    Token* FUN_ID;
} ASTFunCallStmt;

typedef struct ASTIterativeStmt {
    // A field to store booleanExpression
    // A field to store linked list of ASTStmt of length at least one
    int LINE_NO_START; // Line number where the iterative starts (Needef for error reporting)
    int LINE_NO_END; // Line number where the iterative ends (Needed for error reporting)
} ASTIterativeStmt;

typedef struct ASTConditionalStmt {
    // A field to store the boolean expression
    // A field to store the linked list of ASTStmt of length at least one
    // A field to store linekd list of ASTStmt which are in the elsePart, NULL if no elsepart
} ASTConditionalStmt;

typedef struct ASTIOStmt {
    // A flag to indicatate whether it is a Read or a Write (will be usef prior to accessing other fields)
    // POSSIBLY a flag to store whether a single variable or a record id
    // A fieled to store the TK_ID or TK_ID.TK_FIELDID
    // A field to store whether the output guy is 1) Number, 2) Real Number 3) TK_ID 4) TK_ID.TK_FIELDID
    // A field to store the output guy
    int IS_READ; // 1 if the IOStmt is a read, 0 if it is a write
    int SINGLE_OR_RECORD; // To be accessed only if it is a Read
    char* ID; // To be accessed if it is a read/ a write involving an indentifier
    char* FIELD_ID; // To be accessed if it is a read/ a write involving a Record identifier
    int IS_NUMBER; // Field to store if the entity being written is a number
    char* VALUE; // To be accessd if it is a write involving a number
} ASTIOStmt;


typedef struct ASTArithmeticExpression {
    // A flag to store whether it is a single terminal or not (decides access of below 2 fields if it is, otherwise the rest)
    // A field to store whether the output guy is 1) Number, 2) Real Number 3) TK_ID 4) TK_ID.TK_FIELDID
    // A field to store the output guy
    // A field to store the operator
    // A field which stores the pointer to the ArithmeticExpression on the left
    // A field which stores the pointer to the ArithmeticExpression on the right
    Token* OPERATOR;
} ASTArithmeticExpression;

typedef struct ASTBooleanExpression {
    // A field to store whether it is a 1) Logical 2) Relational 3) Negation
    // If logical, the logical op
    // If logical a pointer to store the boolean expression on the left
    // If logical a pointer to store the boolean expression on the right
    // If relational, the relational Op
    // If relational stores the left variable which can be a TK_ID,TK_NUM or TK_RNUM
    // If relational stores the right variable which can be a TK_ID, TK_NUM or TK_RNUM
    // If negation, a pointer to the boolean expression underneath
    Token* OPERATOR;
} ASTBooleanExpression;

typedef struct ASTNum {
    Token* VALUE;
} ASTNum;

typedef struct ASTRNum {
    Token* VALUE;
} ASTRNum;


typedef union ASTNodeType {
    ASTProgram AST_PROGRAM;
    ASTFunction AST_FUNCTION;
    ASTDatatype AST_DATA_TYPE;
    ASTStmts AST_STMTS;
    ASTTypedefinition AST_TYPE_DEFINITION;
    ASTFieldDefinition AST_FIELD_DEFINITION;
    ASTDeclaration AST_DECLARATION;
    ASTAssignmentStmt AST_ASSIGNMENT_STMT;
    ASTFunCallStmt AST_FUN_CALL_STMT;
    ASTIterativeStmt AST_ITERATIVE_STMT;
    ASTConditionalStmt AST_CONDITIONAL_STMT;
    ASTIOStmt AST_IO_STMT;
    ASTReturnStmt AST_RETURN_STMT;
    ASTArithmeticExpression AST_ARITHMETIC_EXPRESSION;
    ASTBooleanExpression AST_BOOLEAN_EXPRESSION;
    ASTID AST_ID;
    ASTNum AST_NUM;
    ASTRNum AST_RNUM;
} ASTNodeType;

typedef struct ASTNode {
    int IS_LEAF; // Whether the node is a leaf or not
    Label LABEL; // Label of the AST Node
    ASTNodeType AST_NODE_TYPE; // Represents the actual type of ASTNode beneath this generic AST Node
    int CHILDREN_COUNT; // Counts the number of children in a node
    SymbolTable* SCOPED_TABLE; // Stores the pointer to the symbol table, this node is scoped in
    struct ASTNode* parent; // Points to the parent of the current node
    struct ASTNode* next; // Points to the next node of the list which this node is a part of
    struct ASTNode* children; // Head of the Linked list of the children of this node (To make traversal conevenient)
    struct ASTNode* tail; // Tail of the linked list of the children of this node (To make addition of child O(1))
} ASTNode;

typedef struct AST {
    ASTNode* root;
} AST;

#endif
