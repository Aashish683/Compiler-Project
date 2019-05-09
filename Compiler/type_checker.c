/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "interface.h"
#include "symbol_table.h"
#include "type_checker.h"
#include "error_handler.h"
#include <string.h>


// Function prints out the Type as a string
// Takes TK_INT TK_NUM TK_REAL TK_RNUM TK_RECORDID
char* getDataType(Token* t) {

    TokenName type = t->TOKEN_NAME;

    switch(type) {
        case TK_INT:
        case TK_NUM:{
            return "int";
            break;
        }
        case TK_REAL:
        case TK_RNUM: {
            return "real";
            break;
        }
        case TK_RECORDID: {
            return t->LEXEME;
        }
    }
}


// This function extracts the data type of an ID entry from the symbol table
// It takes the AST Node for the identifier and an errorList to report errors
// CRUCIAL NOTE => This returns the Datatype head, so for a recorf it sends a token of type TK_RECORDID, actual type is token->LEXEME
Token* extractDataTypeFromSymbolTable(ASTNode* astIdNode, ErrorList* els) {

    Token* type;
    SymbolEntry* entry = lookupSymbolEntry(astIdNode->SCOPED_TABLE,astIdNode->AST_NODE_TYPE.AST_ID.ID);

    if(entry == NULL) {
        // Throw a missing declaration error and return NULL
        throwMissingDeclarationError(astIdNode->AST_NODE_TYPE.AST_ID.ID,els);
        return NULL;
    }

    // Case when the entry on the left is a variabale
    if(entry->SYMBOL_LABEL == symbolVariable) {

        Token* fieldId = astIdNode->AST_NODE_TYPE.AST_ID.FIELD_ID;

        // If there is no fieldId set datatype to the record type
        if(fieldId == NULL)
            type = entry->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE;
        // If a field is being accessed, set data type to the field type
        else {
            Token* recordId = entry->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE;
            type = extractFieldDataType(astIdNode->SCOPED_TABLE->parent,recordId,fieldId,els);
        }

    }
    else if(entry->SYMBOL_LABEL == symbolParameter) {

        Token* fieldId = astIdNode->AST_NODE_TYPE.AST_ID.FIELD_ID;
        // If there is no fieldId set datatype to the record type
        if(fieldId == NULL)
            type = entry->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.DATA_TYPE;
        // If a field is being accessed, set data type to the field type
        else {
            Token* recordId = entry->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.DATA_TYPE;
            type = extractFieldDataType(astIdNode->SCOPED_TABLE->parent,recordId,fieldId,els);
        }

    }
    else {
        // LHS not an identifier or a parameter, so not valid throw error
        printf("Symbol table entry which is an ASTId has been mapped to neither a parameter not a variable, not correct\n");
        type = NULL;
    }

    return type;
}

// Extracts the data type of a field in a record, if not found returns NULL
// Note that st HAS TO be the global symbol table
Token* extractFieldDataType(SymbolTable* st,Token* recordId,Token* fieldId,ErrorList* els) {

    SymbolEntry* recordEntry = lookupSymbolEntry(st,recordId);

    // No record entry exists
    if(recordEntry == NULL) {
        throwMissingRecordDefinitionError(recordId,els);
        return NULL;
    }

    int index = 0;
    int numberFields = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS;
    Token** fields = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.FIELDS;

    for(index=0; index < numberFields; index++) {
        // Found index of entry
        if(strcmp(fields[index]->LEXEME,fieldId->LEXEME) == 0)
            break;
    }

    if(index == numberFields)
        return NULL;

    // Return the data type at the same index
    return recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.DATA_TYPE[index];
}

// TODO => RENAME THIS FUNCTION BECAUSE IT PERFORMS MULTIPLE ACTIVITIES, BETTER CLARITY
// Populate the offset
// Node should be an astIdNode
// This Function operates on all id nodes which are successors of astoutputParams astInputParams astDecls
// Given the above point, this function also acts as a leverage to report missing record entries
void populateOffset(ASTNode* astIdNode, SymbolEntry* idEntry,SymbolTable* scopedTable,ErrorList* els) {

    if(astIdNode == NULL) {
        printf("astDeclaration having a NULL child, detected in type checking not correct\n");
        return;
    }

    if(astIdNode->LABEL != astId) {
        printf("Offset population being called for on a node which is not astId, not correct\n");
    }

    // Removed as we have to store global offsets
    // // If it's a global entry, store -1 and return
    // if(astIdNode->SCOPED_TABLE->parent == NULL) {
    //     idEntry->SYMBOL_OFFSET = -1;
    //     return;
    // }


    Token* type;

    type = extractDataTypeFromSymbolTable(astIdNode,els);

    if(type == NULL) {
        printf("type is neither TK_INT TK_REAL OR TK_RECORDID, not correct\n");
        return;
    }

    if(type->TOKEN_NAME == TK_INT) {
        idEntry->SYMBOL_OFFSET = scopedTable->CURRENT_OFFSET;
        scopedTable->CURRENT_OFFSET += 2;
    }
    else if(type->TOKEN_NAME == TK_REAL) {
        idEntry->SYMBOL_OFFSET = scopedTable->CURRENT_OFFSET;
        scopedTable->CURRENT_OFFSET += 4;
    }
    else if(type->TOKEN_NAME == TK_RECORDID) {
        SymbolEntry* recordEntry = lookupSymbolEntry(scopedTable,type);
        if(recordEntry == NULL) {
            // Same thing would (ideally, if correctly implemented) have already been detected above
            // A TODO Type could not be found, RECORD NOT PRESENT
            throwMissingRecordDefinitionError(type,els);
            return;
        }
        int recordOffset = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.TOTAL_OFFSET;
        idEntry->SYMBOL_OFFSET = scopedTable->CURRENT_OFFSET;
        scopedTable->CURRENT_OFFSET += recordOffset;
    }
    else {
        printf("Unknown type, encountered while populating offset, not correct\n");
    }

}

// Function which compares two data types and an operator, returns the token which the result has if compatible, else returns NULL
Token* compatibleDataTypes(Token* t1, Token* t2,Token* operator) {

    // Case when both are not TK_RECORDID
    if(t1->TOKEN_NAME != TK_RECORDID && t2->TOKEN_NAME != TK_RECORDID) {
        // Compare TokenNames

        // Case when names match exactly
        if(t1->TOKEN_NAME == t2->TOKEN_NAME)
            return t1;
        // TK_INT data type represents TK_NUM as well
        else if(t1->TOKEN_NAME == TK_INT && (t2->TOKEN_NAME == TK_NUM || t2->TOKEN_NAME == TK_RNUM))
            return t1;
        else if(t2->TOKEN_NAME == TK_INT && (t1->TOKEN_NAME == TK_NUM || t1->TOKEN_NAME == TK_RNUM))
            return t2;
        // TK_REAL data type represents TK_RNUM as well
        else if(t1->TOKEN_NAME == TK_REAL && (t2->TOKEN_NAME == TK_RNUM || t2->TOKEN_NAME == TK_NUM))
            return t1;

        else if(t2->TOKEN_NAME == TK_REAL && (t1->TOKEN_NAME == TK_RNUM || t1->TOKEN_NAME == TK_NUM))
            return t2;
        // No match found
        else
            return NULL;
    }

    // Case when both are TK_RECORDID
    else if(t1->TOKEN_NAME == TK_RECORDID && t2->TOKEN_NAME == TK_RECORDID) {
        // Compare RECORDIDs
        if(strcmp(t1->LEXEME,t2->LEXEME) == 0 && (operator->TOKEN_NAME == TK_PLUS || operator->TOKEN_NAME == TK_MINUS) )
            return t1;
        else {
            return NULL;
        }
    }

    // Case when only one is TK_RECORDID, obviously a mismatch
    else {
        // Record with scalar in multiplication and division is valid
        // Record being the LHS
        // Note that both mul and div are valid as <record>*4 and <record>/4 are both correct
        if((operator->TOKEN_NAME == TK_MUL || operator->TOKEN_NAME == TK_DIV) && t1->TOKEN_NAME == TK_RECORDID && (t2->TOKEN_NAME == TK_INT || t2->TOKEN_NAME == TK_REAL || t2->TOKEN_NAME == TK_NUM || t2->TOKEN_NAME == TK_RNUM))
            return t1;
        // Record with scalar in multiplication is valid
        // Record being the RHS
        // Note that only multiplication is valid as 4/<record> is invalid (Ask ma'am)
        else if((operator->TOKEN_NAME == TK_MUL) && t2->TOKEN_NAME == TK_RECORDID && (t1->TOKEN_NAME == TK_INT || t1->TOKEN_NAME == TK_REAL || t1->TOKEN_NAME == TK_NUM || t1->TOKEN_NAME == TK_RNUM))
            return t2;
        // Invalid
        else
            return NULL;
    }
}

// Checks if type of token t1 can be assigned to type of token t2
// Valid inputs must have token names out of TK_INT , TK_REAL or TK_RECORDID, TK_NUM or TK_RNUM
int assignableDataTypes(Token* t1,Token* t2) {

    // Case when both are equal
    if(t1->TOKEN_NAME == t2->TOKEN_NAME) {

        // Case when both are TK_RECORDS
        if(t1->TOKEN_NAME == TK_RECORDID) {
            // If both represent the same record
            if(strcmp(t1->LEXEME,t2->LEXEME) == 0)
                return 1;
            else
                return 0;
        }
        else {
            return 1;
        }
    }
    // Case when both do not match
    else {
        // Case when exactly one of them is a record
        if(t1->TOKEN_NAME == TK_RECORDID || t2->TOKEN_NAME == TK_RECORDID)
            return 0;
        else if(t1->TOKEN_NAME == TK_INT && t2->TOKEN_NAME == TK_NUM)
            return 1;
        else if(t1->TOKEN_NAME == TK_REAL && t2->TOKEN_NAME == TK_RNUM)
            return 1;
        else
            return 0;
    }


}

// Function evaluates the Type returned by an arithmeticExpressionNode
// Input should only be an ASTNode with type astArithmeticExpressiin
Token* getArithmeticExpressionType(ASTNode* astArithmeticExpressionNode, ErrorList* els) {

    Token* operator = astArithmeticExpressionNode->AST_NODE_TYPE.AST_ARITHMETIC_EXPRESSION.OPERATOR;
    ASTNode* lhsNode = astArithmeticExpressionNode->children;
    ASTNode* rhsNode = astArithmeticExpressionNode->children->next;

    Token* lhsType;
    Token* rhsType;

    // Return pointer to TK_NUM
    if(lhsNode->LABEL == astNum)
        lhsType = lhsNode->AST_NODE_TYPE.AST_NUM.VALUE;

    // Return pointer to TK_RNUM
    else if(lhsNode->LABEL == astRnum)
        lhsType = lhsNode->AST_NODE_TYPE.AST_RNUM.VALUE;

    else if(lhsNode->LABEL == astId) {

        SymbolEntry* s = lookupSymbolEntry(lhsNode->SCOPED_TABLE,lhsNode->AST_NODE_TYPE.AST_ID.ID);

        // If s is not found in the symbol table throw a missing declaration error and return
        if(s == NULL) {
            throwMissingDeclarationError(lhsNode->AST_NODE_TYPE.AST_ID.ID,els);
            return NULL;
        }

        // Check if the ID has a fieldID
        Token* fieldId = lhsNode->AST_NODE_TYPE.AST_ID.FIELD_ID;

        // If there is no fieldId set datatype to the record type
        if(fieldId == NULL)
            lhsType = s->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE;
        // If a field is being accessed, set data type to the field type
        else {
            Token* recordId = s->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE;
            lhsType = extractFieldDataType(lhsNode->SCOPED_TABLE->parent,recordId,fieldId,els);
        }



    }

    else if(lhsNode->LABEL == astArithmeticExpression) {

        lhsType = getArithmeticExpressionType(lhsNode,els);

    }

    else
        printf("Type of arithmetic expression is being calculated for a node which is not part of the paradigm, not correct!\n");


    if(rhsNode->LABEL == astNum)
        rhsType = rhsNode->AST_NODE_TYPE.AST_NUM.VALUE;

    else if(rhsNode->LABEL == astRnum)
        rhsType = rhsNode->AST_NODE_TYPE.AST_RNUM.VALUE;

    else if(rhsNode->LABEL == astId) {
        SymbolEntry* s = lookupSymbolEntry(rhsNode->SCOPED_TABLE,rhsNode->AST_NODE_TYPE.AST_ID.ID);

        // If s is not found in the symbol table throw a missing declaration error and return
        if(s == NULL) {
            throwMissingDeclarationError(rhsNode->AST_NODE_TYPE.AST_ID.ID,els);
            return NULL;
        }

        // Check if the ID has a fieldID
        Token* fieldId = rhsNode->AST_NODE_TYPE.AST_ID.FIELD_ID;

        // If there is no fieldId set datatype to the record type
        if(fieldId == NULL)
            rhsType = s->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE;
        // If a field is being accessed, set data type to the field type
        else {
            Token* recordId = s->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE;
            rhsType = extractFieldDataType(rhsNode->SCOPED_TABLE->parent,recordId,fieldId,els);
        }

    }

    else if(rhsNode->LABEL == astArithmeticExpression) {
        rhsType = getArithmeticExpressionType(rhsNode,els);
    }

    else
        printf("Type of arithmetic expression is being calculated for a node which is not part of the paradigm, not correct!\n");


    // Case when either one of them is an ERROR
    if(lhsType == NULL || rhsType == NULL) {
        return NULL;
    }
    Token* resultType = compatibleDataTypes(lhsType,rhsType,operator);
    // Case when both are compatible and neither of them is an error
    if(resultType != NULL)
        return resultType;
    // Case when they do not match, happens first, the upper case happens later
    else if(resultType == NULL) {
        throwTypeMismatchError(lhsType,rhsType,els,operator->LINE_NO);
        return NULL;
    }
}


// Finds the type being given by a boolean expression node
// Returns 1 for it being either TRUE or FALSE, -1 for being an ERROR
int checkBooleanExpressionType(ASTNode* astBooleanExpressionNode, SymbolTable* st, ErrorList* els) {

    Token* operator = astBooleanExpressionNode->AST_NODE_TYPE.AST_BOOLEAN_EXPRESSION.OPERATOR;
    ASTNode* lhsNode = astBooleanExpressionNode->children;
    ASTNode* rhsNode = astBooleanExpressionNode->children->next;

    Token* lhsType;
    Token* rhsType;

    // Case when lhs is a boolean and rhs is NULL (bool -> TK_NOT bool)
    if(lhsNode->LABEL == astBooleanExpression && rhsNode == NULL) {
        return checkBooleanExpressionType(lhsNode,st,els);
    }
    // Base case when lhs is an id and rhs is also an id
    else if(lhsNode->LABEL == astId && rhsNode->LABEL == astId) {

        lhsType = extractDataTypeFromSymbolTable(lhsNode,els);
        rhsType = extractDataTypeFromSymbolTable(rhsNode,els);

        if(lhsType == NULL || rhsType == NULL) {
            // Entry not found in symbol table, missing declaration error already thrown in function
            return -1;
        }

        // The data type heads match
        if(lhsType->TOKEN_NAME == rhsType->TOKEN_NAME) {
            // Check whether they are records, records are not allowed
            if(lhsType->TOKEN_NAME == TK_RECORDID) {
                throwTypeMismatchError(lhsType,rhsType,els,lhsNode->AST_NODE_TYPE.AST_ID.ID->LINE_NO);
                return -1;
            }
            else
                return 1;

        }
        // No match return error
        else {
            throwTypeMismatchError(lhsType,rhsType,els,lhsNode->AST_NODE_TYPE.AST_ID.ID->LINE_NO);
            return -1;
        }
    }
    // Case when lhs is a boolean and rhs is also a boolean
    else if(lhsNode->LABEL == astBooleanExpression && rhsNode->LABEL == astBooleanExpression) {
        int isBooleanLhs = checkBooleanExpressionType(lhsNode,st,els);
        int isBooleanRhs = checkBooleanExpressionType(rhsNode,st,els);

        if(isBooleanLhs == -1 || isBooleanRhs == -1)
            return -1;
        else
            return 1;
    }
    else if(lhsNode->LABEL == astNum && rhsNode->LABEL == astRnum)
        return -1;
    else if(lhsNode->LABEL == astRnum && rhsNode->LABEL == astNum)
        return -1;
    else if(lhsNode->LABEL == astNum && rhsNode->LABEL == astNum)
        return 1;
    else if(lhsNode->LABEL == astRnum && rhsNode->LABEL == astRnum)
        return 1;
    else if(lhsNode->LABEL == astId) {
        Token* tk =  extractDataTypeFromSymbolTable(lhsNode,els);
        // If record, it is false
        if(tk->TOKEN_NAME == TK_RECORDID)
            return -1;
        else if(tk->TOKEN_NAME == TK_INT && rhsNode->LABEL == astNum)
            return 1;
        else if(tk->TOKEN_NAME == TK_INT && rhsNode->LABEL == astRnum)
            return -1;
        else if(tk->TOKEN_NAME == TK_REAL && rhsNode->LABEL == astNum)
            return -1;
        else if(tk->TOKEN_NAME == TK_REAL && rhsNode->LABEL == astRnum)
            return 1;

    }
    else if(rhsNode->LABEL == astId) {
        Token* tk =  extractDataTypeFromSymbolTable(rhsNode,els);
        // If record, it is false
        if(tk->TOKEN_NAME == TK_RECORDID)
            return -1;
        else if(tk->TOKEN_NAME == TK_INT && lhsNode->LABEL == astNum)
            return 1;
        else if(tk->TOKEN_NAME == TK_INT && lhsNode->LABEL == astRnum)
            return -1;
        else if(tk->TOKEN_NAME == TK_REAL && lhsNode->LABEL == astNum)
            return -1;
        else if(tk->TOKEN_NAME == TK_REAL && lhsNode->LABEL == astRnum)
            return 1;
    }
    else {
        printf("Incorrect case hit in vboolean type checking!\n");
        return -1;
    }

}
