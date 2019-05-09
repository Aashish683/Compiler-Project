/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "interface.h"
#include "error_handler.h"
#include "symbol_table.h"
#include "type_checker.h"
#include "semantic_analyzer.h"
#include <string.h>

TokenListItem* initializeTokenListItem(Token* tk) {
    TokenListItem* tls = (TokenListItem*)malloc(sizeof(TokenListItem));
    tls->TK = tk;
    tls->next = NULL;
    return tls;
}

int searchConditionals(TokenListItem* tls, Token* tk) {
    TokenListItem* trav = tls;
    while(trav != NULL) {
        if(strcmp(trav->TK->LEXEME,tk->LEXEME) == 0)
            return 1;
        trav = trav->next;
    }
    return 0;
}

TokenListItem* mergeConditionals(TokenListItem* leftConditionals, TokenListItem* rightConditionals) {
    TokenListItem* trav = leftConditionals;
    while(trav != NULL && trav->next != NULL) {
        trav = trav->next;
    }

    trav->next = rightConditionals;
    return leftConditionals;
}

TokenListItem* getConditionals(ASTNode* astBooleanExpressionNode) {

    if(astBooleanExpressionNode->LABEL != astBooleanExpression) {
        printf("getConditionals called on a node which is not astBooleanExpression\n");
        return NULL;
    }

    ASTNode* lhsNode = astBooleanExpressionNode->children;
    ASTNode* rhsNode = astBooleanExpressionNode->children->next;

    // Case for astBool --> TK_NOT astBool
    if(lhsNode->LABEL == astBooleanExpression && rhsNode == NULL)
        return getConditionals(lhsNode);
    // Case for astBool --> astBool TK_LOGICAL astBool
    else if(lhsNode->LABEL == astBooleanExpression && rhsNode->LABEL == astBooleanExpression) {
        TokenListItem* leftConditionals = getConditionals(lhsNode);
        TokenListItem* rightConditionals = getConditionals(rhsNode);
        return mergeConditionals(leftConditionals,rightConditionals);
    }
    else if(lhsNode->LABEL == astId && rhsNode->LABEL == astId) {
        TokenListItem* tls1 = initializeTokenListItem(lhsNode->AST_NODE_TYPE.AST_ID.ID);
        TokenListItem* tls2 = initializeTokenListItem(rhsNode->AST_NODE_TYPE.AST_ID.ID);
        tls1->next = tls2;
        return tls1;
    }
    else if(lhsNode->LABEL == astId) {
        TokenListItem* tls1 = initializeTokenListItem(lhsNode->AST_NODE_TYPE.AST_ID.ID);
        return tls1;
    }
    else if(rhsNode->LABEL == astId) {
        TokenListItem* tls2 = initializeTokenListItem(rhsNode->AST_NODE_TYPE.AST_ID.ID);
        return tls2;
    }
    else {
        // No astIds involved in condition, only ast_num and stuff
        // Give warning message for this ^^ ask ma'am
        return NULL;
    }
}

int searchIterativeChildren(ASTNode* astIterativeStmtNode, TokenListItem* conditionals) {

    if(astIterativeStmtNode->LABEL != astIterativeStmt) {
        printf("Iterative search on a non iterative node, not correct\n");
    }

    // Search the statements beneatht the iterative
    ASTNode* trav = astIterativeStmtNode->children->next;
    while(trav != NULL) {
        if(trav->LABEL == astAssignmentStmt) {
            ASTNode* astIdNode = trav->children;
            int itrRes = searchConditionals(conditionals,astIdNode->AST_NODE_TYPE.AST_ID.ID);
            if(itrRes == 1)
                return 1;
        }
        else if(trav->LABEL == astFunCallStmt) {
            // Get the starting ID node for the output args
            ASTNode* innerTrav = trav->children->children;
            while(innerTrav != NULL) {
                int itrRes = searchConditionals(conditionals,innerTrav->AST_NODE_TYPE.AST_ID.ID);
                if(itrRes == 1)
                    return 1;
                innerTrav = innerTrav->next;
            }

        }
        else if(trav->LABEL == astIOStmtRead) {
            ASTNode* astIdNode = trav->children;
            int itrRes = searchConditionals(conditionals,astIdNode->AST_NODE_TYPE.AST_ID.ID);
            if(itrRes == 1)
                return 1;
        }
        else if(trav->LABEL == astIterativeStmt) {
            int itrRes =  searchIterativeChildren(trav,conditionals);
            if(itrRes == 1)
                return 1;
        }

        trav = trav->next;
    }
    return 0;
}

int checkForIterationUpdate(ASTNode* astIterativeStmtNode) {
    ASTNode* astBooleanExpressionNode = astIterativeStmtNode->children;
    TokenListItem* conditionals = getConditionals(astBooleanExpressionNode);

    if(conditionals == NULL)
        return 1;

    int itrRes =  searchIterativeChildren(astIterativeStmtNode,conditionals);
    return itrRes;
}

void captureErrorsHelper(ASTNode* node, ErrorList* els) {

    if(node == NULL)
        return;

    Label label = node->LABEL;

    switch(label) {
        case astProgram: {
            // No action
            break;
        }
        case astFunction: {
            break;
        }
        case astInputParams: {
            // Populate offsets of all input params
            ASTNode* trav = node->children;
            while(trav != NULL ) {

                if(trav->LABEL != astId) {
                    printf("Child of astInputParams not astId, incorrect\n");
                }

                SymbolEntry* idEntry = lookupSymbolEntry(trav->SCOPED_TABLE,trav->AST_NODE_TYPE.AST_ID.ID);

                if(idEntry == NULL) {
                    printf("Entry of a declaration node not present, detected in type checking, not correct\n");
                }
                populateOffset(trav,idEntry,trav->SCOPED_TABLE,els);
                trav = trav->next;
            }

            break;
        }
        case astOutputParams: {
            // Populate offsets of all output params
            ASTNode* trav = node->children;
            while(trav != NULL ) {

                if(trav->LABEL != astId) {
                    printf("Child of astInputParams not astId, incorrect\n");
                }

                SymbolEntry* idEntry = lookupSymbolEntry(trav->SCOPED_TABLE,trav->AST_NODE_TYPE.AST_ID.ID);

                if(idEntry == NULL) {
                    printf("Entry of a declaration node not present, detected in type checking, not correct\n");
                }

                populateOffset(trav,idEntry,trav->SCOPED_TABLE,els);
                trav = trav->next;
            }

            break;
        }
        case astDatatype: {
            break;
        }
        case astStmts: {
            // No action
            break;
        }
        case astTypeDefintion: {
            // No action
            break;
        }
        case astFieldDefinition: {
            // No action
            break;
        }
        case astDeclaration: {
            // Store offset
            SymbolEntry* idEntry = lookupSymbolEntry(node->SCOPED_TABLE,node->children->AST_NODE_TYPE.AST_ID.ID);

           if(idEntry == NULL) {
                printf("Entry of a declaration node not present, detected in type checking, not correct\n");
            }


            populateOffset(node->children,idEntry,node->SCOPED_TABLE,els);
            break;
        }
        case astAssignmentStmt: {

            ASTNode* astIdNode = node->children;
            ASTNode* astRightNode = node->children->next;


            SymbolEntry* entry = lookupSymbolEntry(astIdNode->SCOPED_TABLE,astIdNode->AST_NODE_TYPE.AST_ID.ID);

            if(entry == NULL) {
                // Throw a missing declaration error and return
                throwMissingDeclarationError(astIdNode->AST_NODE_TYPE.AST_ID.ID,els);
                return;
            }

            Token* lhsType;
            Token* rhsType;

            // Case when the entry on the left is a variabale
            if(entry->SYMBOL_LABEL == symbolVariable) {

                Token* fieldId = astIdNode->AST_NODE_TYPE.AST_ID.FIELD_ID;

                // If there is no fieldId set datatype to the record type
                if(fieldId == NULL)
                    lhsType = entry->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE;
                // If a field is being accessed, set data type to the field type
                else {
                    Token* recordId = entry->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE;
                    lhsType = extractFieldDataType(astIdNode->SCOPED_TABLE->parent,recordId,fieldId,els);
                }

            }
            else if(entry->SYMBOL_LABEL == symbolParameter) {

                Token* fieldId = astIdNode->AST_NODE_TYPE.AST_ID.FIELD_ID;

                // If there is no fieldId set datatype to the record type
                if(fieldId == NULL)
                    lhsType = entry->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.DATA_TYPE;
                // If a field is being accessed, set data type to the field type
                else {
                    Token* recordId = entry->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.DATA_TYPE;
                    lhsType = extractFieldDataType(astIdNode->SCOPED_TABLE->parent,recordId,fieldId,els);
                }

            }
            else {
                // LHS not an identifier or a parameter, so not valid throw error
            }


            // Case when the rhs is an int number
            if(astRightNode->LABEL == astNum)
                rhsType = astRightNode->AST_NODE_TYPE.AST_NUM.VALUE;
            // Case when the rhs is a real number
            else if(astRightNode->LABEL == astRnum)
                rhsType = astRightNode->AST_NODE_TYPE.AST_RNUM.VALUE;
            // Case when the rhs is a variable
            else if(astRightNode->LABEL == astId) {

                SymbolEntry* s = lookupSymbolEntry(astRightNode->SCOPED_TABLE,astRightNode->AST_NODE_TYPE.AST_ID.ID);

                // If s is not found in the symbol table throw a missing declaration error and return
                if(s == NULL) {
                    throwMissingDeclarationError(astRightNode->AST_NODE_TYPE.AST_ID.ID,els);
                    return;
                }

                // Check if the ID has a fieldID
                Token* fieldId = astRightNode->AST_NODE_TYPE.AST_ID.FIELD_ID;

                // If there is no fieldId set datatype to the record type
                if(fieldId == NULL)
                    rhsType = s->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE;
                // If a field is being accessed, set data type to the field type
                else {
                    Token* recordId = s->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE;
                    rhsType = extractFieldDataType(astRightNode->SCOPED_TABLE->parent,recordId,fieldId,els);
                }

            }
            // Case when the rhs is an arithmetic expression
            else if(astRightNode->LABEL == astArithmeticExpression)
                rhsType = getArithmeticExpressionType(astRightNode,els);
            else {
                printf("type checking on assignment statement does not invole astnum astrnum astid or atrithmeticexpression, not correct\n");
            }


            // Case when the RHS has an error in it's arithmeticExpression
            if(rhsType == NULL)
                ; // No action as error would have been reported inside

            // Case when both of them are unequal, throw error
            else if(assignableDataTypes(lhsType,rhsType) == 0) {
                throwTypeMismatchError(lhsType,rhsType,els,astIdNode->AST_NODE_TYPE.AST_ID.ID->LINE_NO);
            }

            break;
        }
        case astFunCallStmt: {

            // Check if a function calls itself, if it is throw error
            if(strcmp(node->SCOPED_TABLE->SCOPE,node->AST_NODE_TYPE.AST_FUN_CALL_STMT.FUN_ID->LEXEME) == 0)
                throwRecursiveFunctionCallError(node->AST_NODE_TYPE.AST_FUN_CALL_STMT.FUN_ID,els);

            break;
        }
        case astIterativeStmt: {
            int itrRes = checkForIterationUpdate(node);
            if(itrRes == 0)
                throwNoIterationUpdateError(node->AST_NODE_TYPE.AST_ITERATIVE_STMT.LINE_NO_START,node->AST_NODE_TYPE.AST_ITERATIVE_STMT.LINE_NO_END,els);

            break;
        }
        case astConditionalStmt: {

            break;
        }
        case astElsePart: {
            break;
        }
        case astIOStmtRead: {

            break;
        }
        case astIOStmtWrite: {

            break;
        }
        case astReturnStmt: {

            // ReturnStmt <-- stmts <-- function
            ASTNode* functionNode = node->parent->parent;

            SymbolEntry* functionEntry = lookupSymbolEntry(node->SCOPED_TABLE->parent,functionNode->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN);

            if(functionEntry == NULL) {
                printf("FUnction in which this return statement belongs has no entry in the symbol table, fishy, not correct\n");
            }

            // Get the number of children being returned
            int numberReturns = node->CHILDREN_COUNT;

            if(numberReturns != functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.NUMBER_OUTPUT_PARAMS) {
                throwInvalidNumberOfReturnVariablesError(node->AST_NODE_TYPE.AST_RETURN_STMT.RETURN_LINE_NO,numberReturns,functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.NUMBER_OUTPUT_PARAMS,els);
                return;
            }

            ASTNode* trav = node->children;
            int index = 0;

            while(trav != NULL) {


                if(trav->LABEL != astId) {
                    printf("Child of astReturnStmt is not astId, detected in type cheking phase, nto correct\n");
                }

                SymbolEntry* idEntry = lookupSymbolEntry(node->SCOPED_TABLE,trav->AST_NODE_TYPE.AST_ID.ID);

                // If ID is not found, throw a missing declaration error
                if(idEntry == NULL) {
                    throwMissingDeclarationError(trav->AST_NODE_TYPE.AST_ID.ID,els);
                    return;
                }

                // If present, evaluate
                Token* datatype1;
                Token* datatype2;

                // Case when the id is a parameter
                if(idEntry->SYMBOL_LABEL == symbolParameter)
                    datatype1 = idEntry->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.DATA_TYPE;
                // Case when the id is a variable
                else
                    datatype1 = idEntry->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE;

                datatype2 = functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.OUTPUT_TYPES[index];

                // If data type's token heads match
                if(datatype1->TOKEN_NAME == datatype2->TOKEN_NAME) {
                    // If the data type head is a record then check if they are the same record
                    if(datatype1->TOKEN_NAME == TK_RECORDID && strcmp(datatype1->LEXEME,datatype2->LEXEME) != 0) {
                        throwReturnTypeMismatchError(trav->AST_NODE_TYPE.AST_ID.ID,datatype1,datatype2,els);
                    }
                    else {
                        ;
                    }
                }
                // If the heads themeselves do not match
                else {
                    throwReturnTypeMismatchError(trav->AST_NODE_TYPE.AST_ID.ID,datatype1,datatype2,els);
                }

                trav = trav->next;
                index++;
            }

            break;
        }
        case astInputArgs: {
            ASTNode* functionNode = node->parent;

            if(functionNode->LABEL != astFunCallStmt) {
                printf("astInputArgs is a child of a node which is not astFunction, detected in type checking phase, not correct\n");
            }

            SymbolEntry* functionEntry = lookupSymbolEntry(node->SCOPED_TABLE->parent,functionNode->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN);

            if(functionEntry == NULL) {
                // Entry of function definition not found, this error would have been handled before, so skip
                return;
            }

            int numberArguments = node->CHILDREN_COUNT;

            if(numberArguments != functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.NUMBER_INPUT_PARAMS) {
                throwInvalidNumberOfInputArgsError(functionNode->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN,numberArguments, functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.NUMBER_INPUT_PARAMS,els);
                return;
            }

            ASTNode* trav = node->children;
            int index = 0;

            while(trav != NULL) {

                if(trav->LABEL != astId) {
                    printf("Child of astInputArgs is not astId, detected in type cheking phase, nto correct\n");
                }

                SymbolEntry* idEntry = lookupSymbolEntry(node->SCOPED_TABLE,trav->AST_NODE_TYPE.AST_ID.ID);

                // If ID is not found, throw a missing declaration error
                if(idEntry == NULL) {
                    throwMissingDeclarationError(trav->AST_NODE_TYPE.AST_ID.ID,els);
                    return;
                }

                // If present, evaluate
                Token* datatype1;
                Token* datatype2;

                // Case when the id is a parameter
                if(idEntry->SYMBOL_LABEL == symbolParameter)
                    datatype1 = idEntry->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.DATA_TYPE;
                // Case when the id is a variable
                else
                    datatype1 = idEntry->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE;

                datatype2 = functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.INPUT_TYPES[index];

                // If data type's token heads match
                if(datatype1->TOKEN_NAME == datatype2->TOKEN_NAME) {
                    // If the data type head is a record then check if they are the same record
                    if(datatype1->TOKEN_NAME == TK_RECORDID && strcmp(datatype1->LEXEME,datatype2->LEXEME) != 0) {
                        throwInputArgumentTypeMismatchError(functionNode->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN,datatype2,datatype1,index,els);
                    }
                    else {
                        ;
                    }
                }
                // If the heads themeselves do not match
                else {
                    throwInputArgumentTypeMismatchError(functionNode->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN,datatype2,datatype1,index,els);
                }

                trav = trav->next;
                index++;
            }

            break;
        }
        case astOutputArgs: {

            ASTNode* functionNode = node->parent;

            if(functionNode->LABEL != astFunCallStmt) {
                printf("astOutputArgs is a child of a node which is not astFunction, detected in type checking phase, not correct\n");
            }


            SymbolEntry* functionEntry = lookupSymbolEntry(node->SCOPED_TABLE->parent,functionNode->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN);

            if(functionEntry == NULL) {
                // Entry of function definition not found, this error would have been handled before, so skip
                return;
            }

            int numberArguments = node->CHILDREN_COUNT;

            if(numberArguments != functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.NUMBER_OUTPUT_PARAMS) {
                throwInvalidNumberOfOutputArgsError(functionNode->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN,numberArguments, functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.NUMBER_OUTPUT_PARAMS,els);
                return;
            }

            ASTNode* trav = node->children;
            int index = 0;

            while(trav != NULL) {

                if(trav->LABEL != astId) {
                    printf("Child of astOutputArgs is not astId, detected in type cheking phase, nto correct\n");
                }

                SymbolEntry* idEntry = lookupSymbolEntry(node->SCOPED_TABLE,trav->AST_NODE_TYPE.AST_ID.ID);

                // If ID is not found, throw a missing declaration error
                if(idEntry == NULL) {
                    throwMissingDeclarationError(trav->AST_NODE_TYPE.AST_ID.ID,els);
                    return;
                }

                // If present, evaluate
                Token* datatype1;
                Token* datatype2;

                // Case when the id is a parameter
                if(idEntry->SYMBOL_LABEL == symbolParameter)
                    datatype1 = idEntry->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.DATA_TYPE;
                // Case when the id is a variable
                else
                    datatype1 = idEntry->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE;

                datatype2 = functionEntry->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.OUTPUT_TYPES[index];

                // If data type's token heads match
                if(datatype1->TOKEN_NAME == datatype2->TOKEN_NAME) {
                    // If the data type head is a record then check if they are the same record
                    if(datatype1->TOKEN_NAME == TK_RECORDID && strcmp(datatype1->LEXEME,datatype2->LEXEME) != 0) {
                        throwOutputArgumentTypeMismatchError(functionNode->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN,datatype2,datatype1,index,els);
                    }
                    else {
                        ;
                    }
                }
                // If the heads themeselves do not match
                else {
                    throwOutputArgumentTypeMismatchError(functionNode->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN,datatype2,datatype1,index,els);
                }

                trav = trav->next;
                index++;
            }

            break;
        }
        case astArithmeticExpression: {

            break;
        }
        case astBooleanExpression: {
            checkBooleanExpressionType(node,node->SCOPED_TABLE,els);
            break;
        }
        case astId: {
            Token* dataType = extractDataTypeFromSymbolTable(node,els);

            // Error already reported in extractDataTypeFromSymbolTable
            if(dataType == NULL)
                ;
            else {
                // Store the data type head so that future passes can get it easily
                node->AST_NODE_TYPE.AST_ID.DATA_TYPE = dataType;
            }
            break;
        }
        case astNum: {

            break;
        }
        case astRnum: {

            break;
        }
    }

    // Consider children
    ASTNode* trav = node->children;
    while(trav != NULL) {
        captureErrorsHelper(trav,els);
        trav = trav->next;
    }
}

// Evaluates the types of expressions in AST like ArithmeticExpressions, BooleanExpressions, Function return stmt
void captureErrors(AST* ast, ErrorList* els) {
    ASTNode* node = ast->root;
    captureErrorsHelper(node,els);
}


void semanticAnalysis(AST* ast) {


    // Initialize Error List
    ErrorList* els = initializeErrorList();

    // Initialize symbol table (First AST Pass)
    SymbolTable* st = constructSymbolTable(ast,els);

    // Capture errors (Second AST Pass)
    captureErrors(ast,els);

}
