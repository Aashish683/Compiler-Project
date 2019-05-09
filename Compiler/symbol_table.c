/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "interface.h"
#include "symbol_table.h"
#include "ast.h"
#include "error_handler.h"
#include <string.h>

// Function to initialize symbol table
SymbolTable* initializeSymbolTable(int numberSlots, Scope scope) {
    SymbolTable* st = (SymbolTable*)malloc(sizeof(SymbolTable));
    st->SCOPE = scope;
    st->NUMBER_SLOTS = numberSlots;
    st->SYMBOL_SLOTS = (SymbolEntry**)malloc(st->NUMBER_SLOTS*sizeof(SymbolEntry));
    st->CURRENT_OFFSET = 0;
    st->parent = NULL;
    return st;
}

// Hash function which hashes according to the lexeme
int symbolHashFunction(SymbolTable* st ,char* str) {

    /* Hash function djb2 and mod */
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return (hash%st->NUMBER_SLOTS);

}

// Create a symbol entry
SymbolEntry* createSymbolEntry(Token* symbolToken,SymbolLabel symbolLabel) {
    SymbolEntry* symbolEntry = (SymbolEntry*)malloc(sizeof(SymbolEntry));
    symbolEntry->SYMBOL_TOKEN = symbolToken;
    symbolEntry->SYMBOL_LABEL = symbolLabel;
    symbolEntry->next = NULL;
    return symbolEntry;
}

// Function to add a symbol entry to linked list of entires
SymbolEntry* addEntryToList(SymbolEntry* list, SymbolEntry* s) {

    // Case when list is empty
    if(list == NULL)
        return s;

    // Case when list is not empty
    s->next = list;
    return s;
}

// Add symbol entry to symbol table
void addSymbolEntry(SymbolTable* st, SymbolEntry* entry,ErrorList* els) {

    // Check if the entry already exists in the table or not
    SymbolEntry* existingEntry = lookupSymbolEntry(st,entry->SYMBOL_TOKEN);

    // Case when the entry exists in the current table, throw error and return
    if(existingEntry != NULL) {
        throwMultipleDefinitionsError(entry->SYMBOL_TOKEN,els);
        return;
    }
    // Case when the entry does not exist in the current table
    else {

        // If the current table is the global table, then no clashes observed
        // Continue with installing the entry
        if(st->parent == NULL) {
            ;
        }
        // If the current table is a scoped table, then check the global table for any clashes
        else {
            SymbolEntry* existingGlobalEntry = lookupSymbolEntry(st->parent,entry->SYMBOL_TOKEN);

            // If there is a global entry, then throw error
            if(existingGlobalEntry != NULL) {
                throwClashingGlobalDefinitionError(entry->SYMBOL_TOKEN,els);
                return;
            }
            // No clashes in the global table as well, continue with installing the entry
            else {
                ;
            }
        }
    }

    // To be done in a separate pass
    // // Set the identifier of the entry to the current identifier
    // entry->SYMBOL_OFFSET = st->CURRENT_OFFSET;

    // // Increment current identifier of table
    // // st->CURRENT_OFFSET++;

    int hashIndex = symbolHashFunction(st,entry->SYMBOL_TOKEN->LEXEME);
    st->SYMBOL_SLOTS[hashIndex] = addEntryToList(st->SYMBOL_SLOTS[hashIndex],entry);
}

// Searches for a symbol entry in the list which has the same lexeme as the token provided
SymbolEntry* searchSymbolEntry(SymbolEntry* ls, Token* token) {
    SymbolEntry* trav = ls;

    while(trav != NULL) {
        // If found return pointer to entry
        if(strcmp(trav->SYMBOL_TOKEN->LEXEME,token->LEXEME) == 0)
            return trav;
        trav = trav->next;
    }

    // Return NULL otherwise
    return NULL;
}

// Searches the symbol table for an entry which has the same lexeme as the provided token
// If a match is not found in the current table, it searches the global table
SymbolEntry* lookupSymbolEntry(SymbolTable* st, Token* token) {
    // Hash according to the lexeme present in the token
    int hashIndex = symbolHashFunction(st,token->LEXEME);
    // Search in the slot

    SymbolEntry* entry = searchSymbolEntry(st->SYMBOL_SLOTS[hashIndex],token);

    // Case when there is no entry found in the table
    if(entry == NULL) {
        // Check the global table (if the current table is a scoped table)
        if(st->parent != NULL) {
            return lookupSymbolEntry(st->parent,token);
        }
        // If the table was itself the global table return NULL
        else
            return NULL;
    }
    // Case when the entry is found
    else
        return entry;

}

void constructSymbolTableHelper(ASTNode* node, SymbolTable* st,ErrorList* els) {

    // Case when we have reached NULL
    if(node == NULL)
        return;

    Label nodeLabel = node->LABEL;

    switch(nodeLabel) {
        case astProgram: {
            // Global table is set for astProgram
            node->SCOPED_TABLE = st;
            break;
        }
        case astFunction: {
            SymbolEntry* function = createSymbolEntry(node->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN,symbolFunction);
            addSymbolEntry(st,function,els);

            // Create new table for the function
            function->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.SCOPED_TABLE = initializeSymbolTable(SYMBOL_TABLE_SLOTS,node->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN->LEXEME);
            // Set the scope of the new table as the function
            function->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.SCOPED_TABLE->SCOPE = node->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN->LEXEME;
            // Set the function's scoped symbol table's parent as the main symbol table
            function->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.SCOPED_TABLE->parent = st;
            // Set the scope of this node to the new table created
            node->SCOPED_TABLE = function->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.SCOPED_TABLE;
            // For all children under astFunction we should populate the new table
            st = function->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.SCOPED_TABLE;
            break;

        }
        case astInputParams: {

            // Get the parent function node
            ASTNode* functionNode = node->parent;
            // Find the entry of the function definition in the symbol table
            SymbolEntry* functionEntry = lookupSymbolEntry(st->parent,functionNode->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN);
            functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.NUMBER_INPUT_PARAMS = node->CHILDREN_COUNT;
            functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.INPUT_TYPES = (Token**)malloc(sizeof(Token*)*functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.NUMBER_INPUT_PARAMS);

            int index = 0;

            // Get children (The IDs constituting the input params of the node)
            ASTNode* trav = node->children;
            while(trav != NULL) {

                if(trav->LABEL != astId) {
                    printf("Child of astInputParams detected not be an astIdNode in the symbol table construction phase , not correct\n");
                }

                Token* dataType = trav->AST_NODE_TYPE.AST_ID.DATA_TYPE;
                Token* variable = trav->AST_NODE_TYPE.AST_ID.ID;
                // Create entry
                SymbolEntry* entry = createSymbolEntry(variable,symbolParameter);
                // Set data type
                entry->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.DATA_TYPE = dataType;

                // Set input parameter or output parameter
                entry->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.IS_INPUT = 1;
                addSymbolEntry(st,entry,els);

                functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.INPUT_TYPES[index] = dataType;

                trav = trav->next;
                index++;
            }

            node->SCOPED_TABLE = st;
            break;
        }
        case astOutputParams: {

            ASTNode* functionNode = node->parent;
            // Find the entry of the function definition in the symbol table
            SymbolEntry* functionEntry = lookupSymbolEntry(st->parent,functionNode->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN);
            functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.NUMBER_OUTPUT_PARAMS = node->CHILDREN_COUNT;
            functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.OUTPUT_TYPES = (Token**)malloc(sizeof(Token*)*functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.NUMBER_OUTPUT_PARAMS);

            int index = 0;

            ASTNode* trav = node->children;

            while(trav != NULL) {

                if(trav->LABEL != astId) {
                    printf("Child of astOutputParams detected not be an astIdNode in the symbol table construction phase , not correct\n");
                }

                Token* dataType = trav->AST_NODE_TYPE.AST_ID.DATA_TYPE;
                Token* variable = trav->AST_NODE_TYPE.AST_ID.ID;
                // Create entry
                SymbolEntry* entry = createSymbolEntry(variable,symbolParameter);
                // Set data type
                entry->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.DATA_TYPE = dataType;

                // Set input parameter or output parameter
                entry->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.IS_INPUT = 0;
                addSymbolEntry(st,entry,els);

                functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.OUTPUT_TYPES[index] = dataType;

                trav = trav->next;
                index++;
            }

            node->SCOPED_TABLE = st;
            break;
        }
        case astDatatype: {
            break;
        }
        case astStmts: {
            node->SCOPED_TABLE = st;
            break;
        }
        case astTypeDefintion: {

            // Create symbolEntry for corresponding RECORDID
            SymbolEntry* typeDefinition = createSymbolEntry(node->AST_NODE_TYPE.AST_TYPE_DEFINITION.RECORD_ID,symbolRecord);

            // Get the number of field definitions beneath this type definition node and store it
            int numberChildren = node->CHILDREN_COUNT;
            typeDefinition->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS = numberChildren;

            // Allocate space for corresponding number of fields
            typeDefinition->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.DATA_TYPE = (Token**)malloc(numberChildren*sizeof(Token*));
            typeDefinition->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.FIELDS = (Token**)malloc(numberChildren*sizeof(Token*));


            // Traverse all children of type definition which should be field definition node
            ASTNode* trav = node->children;
            int count = 0;
            int calculateOffset = 0;
            while(trav != NULL) {

                if(trav->LABEL != astFieldDefinition) {
                    printf("Type definition node considering a node other than a field definition, not correct!\n");
                }

                // Construct data type in cartesian representation
                typeDefinition->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.DATA_TYPE[count] = trav->AST_NODE_TYPE.AST_FIELD_DEFINITION.DATA_TYPE;
                typeDefinition->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.FIELDS[count] = trav->AST_NODE_TYPE.AST_FIELD_DEFINITION.FIELD_ID;

                if(typeDefinition->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.DATA_TYPE[count]->TOKEN_NAME == TK_INT)
                    calculateOffset += 2;
                else if(typeDefinition->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.DATA_TYPE[count]->TOKEN_NAME == TK_REAL)
                    calculateOffset += 4;
                else {
                    // TODO REPORT ERROR IF IT IS AN ERROR, ASK MA'AM
                    ;
                }

                count++;
                trav = trav->next;
            }


            // Set the offset in the entry
            typeDefinition->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.TOTAL_OFFSET = calculateOffset;
            // Install entry in the global symbol table
            addSymbolEntry(st->parent,typeDefinition,els);
            node->SCOPED_TABLE = st->parent;

            break;
        }
        case astFieldDefinition: {
            // As field definition will always belong to a record entry which is always in a global table
            node->SCOPED_TABLE = st->parent;
            break;
        }
        case astDeclaration: {

            // Create a dummy entry which will be populated in the code below
            SymbolEntry* variable = createSymbolEntry(NULL,symbolVariable);
            int isGlobal = node->AST_NODE_TYPE.AST_DECLARATION.IS_GLOBAL;

            ASTNode* trav = node->children;
            if(trav != NULL) {
                // Each child must be an astID node
                if(trav->LABEL != astId) {
                    printf("Declaration not involving an astId node, not correct\n");
                }

                variable->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE = trav->AST_NODE_TYPE.AST_ID.DATA_TYPE;
                variable->SYMBOL_TOKEN = trav->AST_NODE_TYPE.AST_ID.ID;
                trav = trav->next;
            }

            // If it is a global variable , add the entry to the global symbol table
            if(isGlobal == 1) {
                addSymbolEntry(st->parent,variable,els);
                node->SCOPED_TABLE = st->parent;
            }
            // Else add the entry to the function's scoped symbol table
            else {
                addSymbolEntry(st,variable,els);
                node->SCOPED_TABLE = st;
            }

            break;
        }
        case astAssignmentStmt: {
            node->SCOPED_TABLE = st;
            break;
        }
        case astFunCallStmt: {

            // Check if the function has been defined prior to being called
            // Check for a function definition
            SymbolEntry* functionEntry = lookupSymbolEntry(st->parent,node->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN);

            if(functionEntry == NULL) {
                throwMissingFunctionDefinitionError(node->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN,els);
            }

            node->SCOPED_TABLE = st;
            break;
        }
        case astIterativeStmt: {
            node->SCOPED_TABLE = st;
            break;
        }
        case astConditionalStmt: {
            node->SCOPED_TABLE = st;
            break;
        }
        case astElsePart: {
            node->SCOPED_TABLE = st;
            break;
        }
        case astIOStmtRead: {
            node->SCOPED_TABLE = st;
            break;
        }
        case astIOStmtWrite: {
            node->SCOPED_TABLE = st;
            break;
        }
        case astReturnStmt: {
            node->SCOPED_TABLE = st;
            break;
        }
        case astInputArgs: {
            node->SCOPED_TABLE = st;
            break;
        }
        case astOutputArgs: {
            node->SCOPED_TABLE = st;
            break;
        }
        case astArithmeticExpression: {
            // No installation needed beyond this node
            node->SCOPED_TABLE = st;
            break;
        }
        case astBooleanExpression: {
            // No installaton needed beyond this node
            node->SCOPED_TABLE = st;
            break;
        }
        case astId: {

            // Redundant as we will be handling input and output params at their respective places

            // In case the ID is part of input parameters or output parameters, it needs to be installed in the symbol table
            // if(node->parent->LABEL == astInputParams || node->parent->LABEL == astOutputParams) {
            //     // Input and output parameters must be established in this scope
            //     Token* dataType = node->AST_NODE_TYPE.AST_ID.DATA_TYPE;
            //     Token* variable = node->AST_NODE_TYPE.AST_ID.ID;
            //     // Case when the data type is a primitive type
            //     SymbolEntry* entry = createSymbolEntry(variable,symbolParameter);
            //     // Set data type
            //     entry->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.DATA_TYPE = dataType;

            //     // Set input parameter or output parameter
            //     entry->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.IS_INPUT = ((node->parent->LABEL == astInputParams) ? 1 : 0);
            //     addSymbolEntry(st,entry,els);


            // }

            // Adjust scope for this node
            node->SCOPED_TABLE = st;

            break;
        }
        case astNum: {
            node->SCOPED_TABLE = st;
            break;
        }
        case astRnum: {
            node->SCOPED_TABLE = st;
            break;
        }
    }

    // Inform group members that this code is reachable only if the above swith case does not return, which is the case in some cases
    // Traverse children
    ASTNode* trav  = node->children;
    while(trav != NULL) {
        constructSymbolTableHelper(trav,st,els);
        trav = trav->next;
    }


}


// Creates the symbol table by using the declarations and function definitions to populate slots in the appropriate table
// Also populates the scope field of the AST nodes, so that a node's scoped table can directly be referred in the next step
SymbolTable* constructSymbolTable(AST* ast,ErrorList* els) {

    // Initialize symbol table with number of slots and scope as global
    SymbolTable* st = initializeSymbolTable(SYMBOL_TABLE_SLOTS,"global");

    constructSymbolTableHelper(ast->root,st,els);
    return st;

}
