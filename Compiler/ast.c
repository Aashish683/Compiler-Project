/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "interface.h"
#include "parser.h"
#include "ast.h"


int astNodeCount;
int astMemory;
// A map to print the labels
char* LabelMap[] = {
    "astProgram",
    "astFunction",
    "astInputParams",
    "astOutputParams",
    "astDatatype",
    "astStmts",
    "astTypeDefintion",
    "astFieldDefinition",
    "astDeclaration",
    "astAssignmentStmt",
    "astFunCallStmt",
    "astIterativeStmt",
    "astConditionalStmt",
    "astElsePart",
    "astIOStmtRead",
    "astIOStmtWrite",
    "astReturnStmt",
    "astInputArgs",
    "astOutputArgs",
    "astArithmeticExpression",
    "astBooleanExpression",
    "astId",
    "astNum",
    "astRnum",
};

char* getLabel(int l) {
    return LabelMap[l];
}

// Function to create an ASTNode according to a label
ASTNode* createASTNode(int isLeaf, Label l) {
    ASTNode* astNode = (ASTNode*)malloc(sizeof(ASTNode));
    astNode->IS_LEAF = isLeaf;
    astNode->LABEL = l;
    astNode->CHILDREN_COUNT = 0;
    astNode->next = NULL;
    astNode->parent = NULL;
    astNode->children = NULL;
    astNode->tail = NULL;

    astNodeCount += 1;
    astMemory += sizeof(ASTNode);
    return astNode;
}

// Adding a single node as the child
void addASTChild(ASTNode* node, ASTNode* newChild) {
    // Case when the node has no child
    if(node->children == NULL) {
        newChild->parent = node;
        node->children = newChild;
        node->tail = newChild;
        node->CHILDREN_COUNT++;
        return;
    }

    // Case when the node has one child
    if(node->children->next == NULL) {
        newChild->parent = node;
        node->children->next = newChild;
        node->tail = newChild;
        node->CHILDREN_COUNT++;
        return;
    }

    // Case when node has more than one child, insert at end
    newChild->parent = node;
    node->tail->next = newChild;
    node->tail = newChild;
    node->CHILDREN_COUNT++;
    return;
}

// Adding a list of node as children
void addASTChildren(ASTNode* node, ASTNode* ls) {
    ASTNode* trav = ls;
    while(trav != NULL) {
        addASTChild(node,trav);
        trav = trav->next;
    }
}

// Function to create the base AST Node
AST* initializeAST() {
    astNodeCount = *((int*)malloc(sizeof(int)));
    astMemory = *((int*)malloc(sizeof(int)));
    AST* ast = (AST*)malloc(sizeof(AST));
    ast->root = NULL;
    return ast;
}


// getType operates on rules which infer datatypes, returns the lexeme in form a string
// Should always be called with a non terminal
Token* getType(NaryTreeNode* parseTreeNode) {

    if(parseTreeNode->IS_LEAF_NODE == 1) {
        printf("getType called with a terminal, not correct!\n");
        return NULL;
    }

    int ruleNumber = parseTreeNode->NODE_TYPE.NL.RULE_NO;
    switch(ruleNumber) {
        // Case of datatype --> primitiveDataType or datatype --> constructedDataType
        case 10:
        case 11: {
            return getType(parseTreeNode->NODE_TYPE.NL.child);
            break;
        }
        // Case of primitiveDataType --> TK_INT
        case 12: {
            return parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
            break;
        }
        // Case of primitiveDataType --> TK_REAL
        case 13: {
            return parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
            break;
        }
        // Case of constructedDataType --> TK_RECORD TK_RECORDID
        case 14: {
            return parseTreeNode->NODE_TYPE.NL.child->next->NODE_TYPE.L.TK;
            break;
        }
    }
}

int isGlobal(NaryTreeNode* parseTreeNode) {

    if(parseTreeNode->IS_LEAF_NODE == 1) {
        printf("isGlobal called with a terminal, not correct!\n");
        return -1;
    }

    int ruleNumber = parseTreeNode->NODE_TYPE.NL.RULE_NO;
    switch(ruleNumber) {
        case 28: {
            // Case of global_or_not --> TK_COLON TK_GLOBAL
            return 1;
            break;
        }
        case 29: {
            // Case of global_or_not --> TK_EPS
            return 0;
            break;
        }
    }
}



// Should only be called on the non terminal singleOrRecordId
int isSingleOrRecord(NaryTreeNode* parseTreeNode) {

    if(parseTreeNode->IS_LEAF_NODE == 1) {
        printf("isSingleOrRecord called with a terminal, not correct!\n");
        return -1;
    }

    // Case when new_24 --> TK_EPS
    if(parseTreeNode->NODE_TYPE.NL.child->next->NODE_TYPE.NL.child == NULL)
        return 0;
    // Case when new_24 --> TK_DOT TK_FIELDID
    else
        return 1;
}


// Phase this function out
// Takes non terminals singleOrRecordID and new_24,all and temp, returns TK_ID/TK_FIELDID respectively
Token* getIdentifier(NaryTreeNode* parseTreeNode) {

    if(parseTreeNode->IS_LEAF_NODE == 1) {
        printf("getIdentifier called with a terminal, not correct!\n");
        return NULL;
    }

    int ruleNumber = parseTreeNode->NODE_TYPE.NL.RULE_NO;
    switch(ruleNumber) {
        // Case for singleOrRecordID --> TK_ID new24
        case 38: {
            return parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
            break;
        }
        // Case for new_24 --> TK_DOT TK_FIELDID
        case 39: {
            return parseTreeNode->NODE_TYPE.NL.child->next->NODE_TYPE.L.TK;
            break;
        }
        // Case for new_24 --> TK_EPS
        case 40: {
            return NULL;
            break;
        }
        // Case for all --> TK_NUM
        case 63:
        // Case for all --> TK_RNUM
        case 64:
        // Case for all --> TK_ID temp
        case 65:
        // Case for var --> TK_ID
        case 71:
        // Case for var --> TK_NUM
        case 72:
        // Case for var --> TK_RNUM
        case 73:
         {
            return parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
            break;
        }
        // Case for temp --> TK_EPS
        case 66: {
            return NULL;
            break;
        }
        // Case for temp --> TK_DOT TK_FIELDID
        case 67: {
            return parseTreeNode->NODE_TYPE.NL.child->next->NODE_TYPE.L.TK;
            break;
        }
    }
}


// Returns the operator for an expression
Token* getOperator(NaryTreeNode* parseTreeNode) {

    if(parseTreeNode->IS_LEAF_NODE == 1) {
        printf("getOperator called with a terminal, not correct!\n");
        return NULL;
    }

    int ruleNumber = parseTreeNode->NODE_TYPE.NL.RULE_NO;

    return parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
    // Phasing out implementation after return type changed to Token*
    // switch(ruleNumber) {

    //     // highPrecedenceOperators --> TK_MUL
    //     case 59: {
    //         return "*";
    //         break;
    //     }
    //     // highPrecedenceOperators --> TK_DIV
    //     case 60: {
    //         return "/";
    //         break;
    //     }
    //     // lowPrecedenceOperators --> TK_PLUS
    //     case 61: {
    //         return "+";
    //         break;
    //     }
    //     // lowPrecedenceOperators --> TK_MINUS
    //     case 62: {
    //         return "-";
    //         break;
    //     }
    //     // logicalOp --> TK_AND
    //     case 74: {
    //         return "&";
    //         break;
    //     }
    //     // logicalOP --> TK_OR
    //     case 75: {
    //         return "|";
    //         break;
    //     }
    //     // relationalOp --> TK_LT
    //     case 76: {
    //         return "<";
    //         break;
    //     }
    //     // relationalOp --> TK_LE
    //     case 77: {
    //         return "<=";
    //         break;
    //     }
    //     // relationalOp --> TK_EQ
    //     case 78: {
    //         return "==";
    //         break;
    //     }
    //     // relationalOp --> TK_GT
    //     case 79: {
    //         return ">";
    //         break;
    //     }
    //     // relationalOp --> TK_GE
    //     case 80: {
    //         return ">=";
    //         break;
    //     }
    //     // relationalOp --> TK_NE
    //     case 81: {
    //         return "!=";
    //         break;
    //     }
    // }

}


// Function to construct AST from parse tree
AST* constructAST(ParseTree* pt) {
    AST* ast = initializeAST();
    ast->root = constructASTHelper(pt->root);
    return ast;
}

// Constructs the AST along with passing an argument which is inherited
// Called on expPrime as the node, with term as the inherited on rule 51
ASTNode* constructASTHelperInherited(NaryTreeNode* parseTreeNode, ASTNode* inherited) {

    if(parseTreeNode->IS_LEAF_NODE == 1) {
        printf("constructASTHelperInherited called on leaf node, not correct!\n");
        printf("The node was %s\n" ,getTerminal(parseTreeNode->NODE_TYPE.L.ENUM_ID));
        printf("The parent was %s bearing rule number %d\n" , getNonTerminal(parseTreeNode->parent->NODE_TYPE.NL.ENUM_ID),parseTreeNode->parent->NODE_TYPE.NL.RULE_NO);
        return NULL;
    }

    int ruleNumber = parseTreeNode->NODE_TYPE.NL.RULE_NO;

    switch(ruleNumber) {
        case 52: {
            ASTNode* astArithmeticExpressionNode = createASTNode(0,astArithmeticExpression);
            astArithmeticExpressionNode->AST_NODE_TYPE.AST_ARITHMETIC_EXPRESSION.OPERATOR = getOperator(parseTreeNode->NODE_TYPE.NL.child);
            ASTNode* astTermNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            addASTChildren(astArithmeticExpressionNode,inherited);
            addASTChildren(astArithmeticExpressionNode,astTermNode);
            ASTNode* arithmeticExpressionNode2 = constructASTHelperInherited(parseTreeNode->NODE_TYPE.NL.child->next->next,astArithmeticExpressionNode);
            return arithmeticExpressionNode2;
            break;
        }
        case 53: {
            return inherited;
            break;
        }
        case 55: {
            ASTNode* astTermNode = createASTNode(0,astArithmeticExpression);
            astTermNode->AST_NODE_TYPE.AST_ARITHMETIC_EXPRESSION.OPERATOR = getOperator(parseTreeNode->NODE_TYPE.NL.child);
            ASTNode* astFactorNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            addASTChildren(astTermNode,inherited);
            addASTChildren(astTermNode,astFactorNode);
            ASTNode* astTermNode2 = constructASTHelperInherited(parseTreeNode->NODE_TYPE.NL.child->next->next,astTermNode);
            return astTermNode2;
            break;
        }
        case 56: {
            return inherited;
            break;
        }
    }

}

ASTNode* constructASTHelper(NaryTreeNode* parseTreeNode) {

    if(parseTreeNode->IS_LEAF_NODE == 1) {
        printf("constructASTHelperInherited called on leaf node, not correct!\n");
        printf("The node was %s\n" ,getTerminal(parseTreeNode->NODE_TYPE.L.ENUM_ID));
        printf("The parent was %s bearing rule number %d\n" , getNonTerminal(parseTreeNode->parent->NODE_TYPE.NL.ENUM_ID),parseTreeNode->parent->NODE_TYPE.NL.RULE_NO);
        return NULL;
    }

    int ruleNumber = parseTreeNode->NODE_TYPE.NL.RULE_NO;
    switch(ruleNumber) {
        case 1: {
            ASTNode* astProgramNode = createASTNode(0,astProgram);
            ASTNode* functionList = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);
            ASTNode* mainFunctionNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);

            // INVALID ==> APPENDING MAIN AT HEAD DESTORYS THE ORDERING, LEAD TO HARD TO FIND SEG FAULTS
            // ORDER STUFF LIKE THEY ARE SUPPOSED TO!

            // // Append main at the head of the list of functions
            // mainFunction->next = functionList;
            // addASTChildren(astProgramNode,mainFunction);

            addASTChildren(astProgramNode,functionList);
            addASTChildren(astProgramNode,mainFunctionNode);

            return astProgramNode;
            break;
        }
        case 2: {
            ASTNode* astMainFunctionNode = createASTNode(0,astFunction);
            // For main set the FUNCTION_TOKEN explicitly to TK_MAIN
            astMainFunctionNode->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN = parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;

            ASTNode* mainFunctionStmts =  constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            addASTChildren(astMainFunctionNode, mainFunctionStmts);

            return astMainFunctionNode;
            break;
        }
        case 3: {
            ASTNode* astFunctionNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);
            ASTNode* astFunctionListHead = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            // Make the function on the left as the head of the list
            astFunctionNode->next = astFunctionListHead;
            // Return list
            return  astFunctionNode;
            break;
        }
        case 4: {
            return NULL;
            break;
        }
        case 5: {
            ASTNode* astFunctionNode = createASTNode(0,astFunction);
            astFunctionNode->AST_NODE_TYPE.AST_FUNCTION.FUNCTION_TOKEN = parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
            ASTNode* inputParamsNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            ASTNode* outputParamsNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next);
            ASTNode* stmtsNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next->next->next);
            astFunctionNode->next = NULL;

            addASTChildren(astFunctionNode,inputParamsNode);
            addASTChildren(astFunctionNode,outputParamsNode);
            addASTChildren(astFunctionNode,stmtsNode);

            return astFunctionNode;
            break;
        }
        case 6: {
            ASTNode* astInputParamsNode = createASTNode(0,astInputParams);
            ASTNode* astIdListNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next->next->next);
            addASTChildren(astInputParamsNode,astIdListNode);
            return astInputParamsNode;
            break;
        }
        case 7: {
            ASTNode* astOutputParamsNode = createASTNode(0,astOutputParams);
            ASTNode* astIdListNode =  constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next->next->next);
            addASTChildren(astOutputParamsNode,astIdListNode);
            return astOutputParamsNode;
            break;
        }
        case 8: {
            return NULL;
            break;
        }
        case 9: {
            ASTNode* astIdNode = createASTNode(0,astId);
            // Store the identifier
            astIdNode->AST_NODE_TYPE.AST_ID.ID = parseTreeNode->NODE_TYPE.NL.child->next->NODE_TYPE.L.TK;
            astIdNode->AST_NODE_TYPE.AST_ID.FIELD_ID = NULL;
            astIdNode->next = NULL;
            // Get the type of the parameter
            astIdNode->AST_NODE_TYPE.AST_ID.DATA_TYPE = getType(parseTreeNode->NODE_TYPE.NL.child);
            // Get the list of Ids
            ASTNode* astIdList = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next);
            // Make the current ID the head
            astIdNode->next = astIdList;
            return (astIdNode);
            break;
        }
        case 15: {
            ASTNode* astIdNode = createASTNode(0,astId);
            // Store the identifier
            astIdNode->AST_NODE_TYPE.AST_ID.ID = parseTreeNode->NODE_TYPE.NL.child->next->next->NODE_TYPE.L.TK;
            astIdNode->AST_NODE_TYPE.AST_ID.FIELD_ID = NULL;
            astIdNode->next = NULL;
            // Get the type of the parameter
            astIdNode->AST_NODE_TYPE.AST_ID.DATA_TYPE = getType(parseTreeNode->NODE_TYPE.NL.child->next);
            // Get the list of Ids
            ASTNode* astIdList = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next->next);
            // Make the current ID the head
            astIdNode->next = astIdList;
            return (astIdNode);
            break;
        }
        case 16: {
            return NULL;
            break;
        }
        case 17: {
            ASTNode* astStmtsNode = createASTNode(0,astStmts);
            ASTNode* astTypeDefinitionsNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);
            ASTNode* astDeclarationsNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            ASTNode* astOtherStmtsNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next);
            ASTNode* astReturnStmtsNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next->next);
            addASTChildren(astStmtsNode,astTypeDefinitionsNode);
            addASTChildren(astStmtsNode,astDeclarationsNode);
            addASTChildren(astStmtsNode,astOtherStmtsNode);
            addASTChildren(astStmtsNode,astReturnStmtsNode);

            return astStmtsNode;
            break;
        }
        case 18: {
            ASTNode* astTypeDefinitionNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);
            ASTNode* astTypedefinitionList = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            astTypeDefinitionNode->next = astTypedefinitionList;
            return astTypeDefinitionNode;
            break;
        }
        case 19: {
            return NULL;
            break;
        }
        case 20: {
            ASTNode* astTypeDefinitionNode = createASTNode(0,astTypeDefintion);
            astTypeDefinitionNode->AST_NODE_TYPE.AST_TYPE_DEFINITION.RECORD_ID = parseTreeNode->NODE_TYPE.NL.child->next->NODE_TYPE.L.TK;

            ASTNode* astFieldDefinitionList = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next);

            addASTChildren(astTypeDefinitionNode,astFieldDefinitionList);

            return astTypeDefinitionNode;
            break;
        }
        case 21: {
            ASTNode* astFieldDefinitionNode1 = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);
            ASTNode* astFieldDefinitionNode2 = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            astFieldDefinitionNode1->next = astFieldDefinitionNode2;
            ASTNode* astFieldDefinitionList = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next);
            astFieldDefinitionNode2->next = astFieldDefinitionList;
            return astFieldDefinitionNode1;
            break;
        }
        case 22: {
            ASTNode* astFieldDefinitionNode = createASTNode(0,astFieldDefinition);
            astFieldDefinitionNode->AST_NODE_TYPE.AST_FIELD_DEFINITION.DATA_TYPE = getType(parseTreeNode->NODE_TYPE.NL.child->next);
            astFieldDefinitionNode->AST_NODE_TYPE.AST_FIELD_DEFINITION.FIELD_ID = parseTreeNode->NODE_TYPE.NL.child->next->next->next->NODE_TYPE.L.TK;
            return astFieldDefinitionNode;
            break;
        }
        case 23: {
            ASTNode* astFieldDefinitionNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);
            ASTNode* astFieldDefinitionList = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            astFieldDefinitionNode->next = astFieldDefinitionList;
            return astFieldDefinitionNode;
            break;
        }
        case 24: {
            return NULL;
            break;
        }
        case 25: {
            ASTNode* astDeclarationNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);
            ASTNode* astDeclarationList = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            astDeclarationNode->next = astDeclarationList;
            return astDeclarationNode;
            break;
        }
        case 26: {
            return NULL;
            break;
        }
        case 27: {
            ASTNode* astDeclarationNode = createASTNode(0,astDeclaration);
            ASTNode* astIdNode = createASTNode(1,astId);

            astIdNode->AST_NODE_TYPE.AST_ID.DATA_TYPE = getType(parseTreeNode->NODE_TYPE.NL.child->next);
            astIdNode->AST_NODE_TYPE.AST_ID.ID = parseTreeNode->NODE_TYPE.NL.child->next->next->next->NODE_TYPE.L.TK;
            astIdNode->AST_NODE_TYPE.AST_ID.FIELD_ID = NULL;
            addASTChildren(astDeclarationNode,astIdNode);

            astDeclarationNode->AST_NODE_TYPE.AST_DECLARATION.IS_GLOBAL = isGlobal(parseTreeNode->NODE_TYPE.NL.child->next->next->next->next);

            return astDeclarationNode;
            break;
        }
        case 30: {
            ASTNode* astStmtNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);
            ASTNode* astStmtList = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            astStmtNode->next = astStmtList;
            return astStmtNode;
            break;
        }
        case 31: {
            return NULL;
            break;
        }
        case 32:
        case 33:
        case 34:
        case 35:
        case 36: {
            ASTNode* astStmtNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);
            return astStmtNode;
            break;
        }
        case 37: {
            ASTNode* astAssignmentStmtNode = createASTNode(0,astAssignmentStmt);

            // Passing singleOrRecordId
            astAssignmentStmtNode->AST_NODE_TYPE.AST_ASSIGNMENT_STMT.SINGLE_OR_RECORD = isSingleOrRecord(parseTreeNode->NODE_TYPE.NL.child);

            // Unecessary if elses
            // if(astAssignmentStmtNode->AST_NODE_TYPE.AST_ASSIGNMENT_STMT.SINGLE_OR_RECORD == 1) {
            //     // Passing singleOrRecordId
            //     astAssignmentStmtNode->AST_NODE_TYPE.AST_ASSIGNMENT_STMT.ID = getIdentifier(parseTreeNode->NODE_TYPE.NL.child);
            // }
            // else {
            //     // Passing singleOrRecordId
            //     astAssignmentStmtNode->AST_NODE_TYPE.AST_ASSIGNMENT_STMT.ID = getIdentifier(parseTreeNode->NODE_TYPE.NL.child);
            //     // Passing new_24
            //     astAssignmentStmtNode->AST_NODE_TYPE.AST_ASSIGNMENT_STMT.FIELD_ID = getIdentifier(parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.NL.child->next);
            // }

            ASTNode* astIdNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);

            ASTNode* astArithmeticExpressionNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next);
            addASTChildren(astAssignmentStmtNode,astIdNode);
            addASTChildren(astAssignmentStmtNode,astArithmeticExpressionNode);
            return astAssignmentStmtNode;
            break;
        }
        case 38: {
            ASTNode* astIdNode = createASTNode(1,astId);
            astIdNode->AST_NODE_TYPE.AST_ID.ID = parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
            astIdNode->AST_NODE_TYPE.AST_ID.FIELD_ID = getIdentifier(parseTreeNode->NODE_TYPE.NL.child->next);
            astIdNode->next = NULL;
            return astIdNode;
            break;
        }
        case 41: {
            ASTNode* astFunCallStmtNode = createASTNode(0,astFunCallStmt);
            astFunCallStmtNode->AST_NODE_TYPE.AST_FUN_CALL_STMT.FUN_ID = parseTreeNode->NODE_TYPE.NL.child->next->next->NODE_TYPE.L.TK;
            ASTNode* astOutputParamsNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);
            ASTNode* astInputParamsNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next->next->next->next);
            addASTChildren(astFunCallStmtNode,astOutputParamsNode);
            addASTChildren(astFunCallStmtNode,astInputParamsNode);
            return astFunCallStmtNode;
            break;
        }
        case 42: {
            ASTNode* astOutputArgsNode = createASTNode(0,astOutputArgs);
            ASTNode* astIdListNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            addASTChildren(astOutputArgsNode,astIdListNode);
            return astOutputArgsNode;
            break;
        }
        case 43: {
            return NULL;
            break;
        }
        case 44: {
            ASTNode* astInputArgsNode = createASTNode(0,astInputArgs);
            ASTNode* astIdListNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            addASTChildren(astInputArgsNode,astIdListNode);
            return astInputArgsNode;
            break;
        }
        case 45: {
            ASTNode* astIterativeStmtNode = createASTNode(0,astIterativeStmt);
            ASTNode* astBooleanExpressionNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next);
            ASTNode* astStmtNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next->next->next);
            ASTNode* astStmtList = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next->next->next->next);
            astStmtNode->next = astStmtList;
            astIterativeStmtNode->AST_NODE_TYPE.AST_ITERATIVE_STMT.LINE_NO_START = parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK->LINE_NO;
            astIterativeStmtNode->AST_NODE_TYPE.AST_ITERATIVE_STMT.LINE_NO_END = parseTreeNode->NODE_TYPE.NL.child->next->next->next->next->next->next->NODE_TYPE.L.TK->LINE_NO;
            addASTChildren(astIterativeStmtNode,astBooleanExpressionNode);
            addASTChildren(astIterativeStmtNode,astStmtNode);
            return astIterativeStmtNode;
            break;
        }
        case 46: {
            ASTNode* astConditionalStmtNode = createASTNode(0,astConditionalStmt);
            ASTNode* astBooleanExpressionNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next);

            ASTNode* astStmtNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next->next->next->next);
            ASTNode* astStmtList = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next->next->next->next->next);
            astStmtNode->next = astStmtList;

            ASTNode* astElseStmtNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next->next->next->next->next->next);
            addASTChildren(astConditionalStmtNode,astBooleanExpressionNode);
            addASTChildren(astConditionalStmtNode,astStmtNode);
            addASTChildren(astConditionalStmtNode,astElseStmtNode);

            return astConditionalStmtNode;
            break;
        }
        case 47: {
            ASTNode* astElsePartNode = createASTNode(0,astElsePart);
            ASTNode* astStmtNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            ASTNode* astStmtList = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next);
            astStmtNode->next = astStmtList;
            addASTChildren(astElsePartNode,astStmtNode);
            return astElsePartNode;
            break;
        }
        case 48: {
            return NULL;
            break;
        }
        case 49: {
            ASTNode* astIOStmtReadNode = createASTNode(0,astIOStmtRead);

            // Implementataion invalid after IO has been bifurcated into read and write
            // astIOStmtNode->AST_NODE_TYPE.AST_IO_STMT.IS_READ = 1;

            // int singleOrRecordId = isSingleOrRecord(parseTreeNode->NODE_TYPE.NL.child->next->next);
            // astIOStmtNode->AST_NODE_TYPE.AST_IO_STMT.SINGLE_OR_RECORD = singleOrRecId;
            // if(singleOrRecId == 1) {
            //     astIOStmtNode->AST_NODE_TYPE.AST_IO_STMT.ID = getIdentifier(parseTreeNode->NODE_TYPE.NL.child->next->next);
            // }
            // else {
            //     astIOStmtNode->AST_NODE_TYPE.AST_IO_STMT.ID = getIdentifier(parseTreeNode->NODE_TYPE.NL.child->next->next);
            //     astIOStmtNode->AST_NODE_TYPE.AST_IO_STMT.FIELD_ID = getIdentifier(parseTreeNode->NODE_TYPE.NL.child->next->next->NODE_TYPE.NL.child->next);
            // }

            // return astIOStmtNode;

            ASTNode* astIdNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next);
            addASTChildren(astIOStmtReadNode,astIdNode);
            return astIOStmtReadNode;
            break;
        }
        case 50: {

            ASTNode* astIOStmtWriteNode = createASTNode(0,astIOStmtWrite);

            // Implementataion invalid after IO has been bifurcated into read and write
            // astIOStmtNode->AST_NODE_TYPE.AST_IO_STMT.IS_READ = 0;

            // unsigned int tokenEnumID = parseTreeNode->NODE_TYPE.NL.child->next->next->NODE_TYPE.NL.child->NODE_TYPE.L.ENUM_ID;
            // int isNumber = ((tokenEnumID == TK_NUM || tokenEnumID == TK_RNUM) ? 1 : 0);
            // astIOStmtNode->AST_NODE_TYPE.AST_IO_STMT.IS_NUMBER = isNumber;
            // // If it is a number
            // if(isNumber == 1)
            //     astIOStmtNode->AST_NODE_TYPE.AST_IO_STMT.VALUE = getIdentifier(parseTreeNode->NODE_TYPE.NL.child->next->next);
            // else {
            //     astIOStmtNode->AST_NODE_TYPE.AST_IO_STMT.ID = getIdentifier(parseTreeNode->NODE_TYPE.NL.child->next->next);
            //     astIOStmtNode->AST_NODE_TYPE.AST_IO_STMT.FIELD_ID = getIdentifier(parseTreeNode->NODE_TYPE.NL.child->next->next->NODE_TYPE.NL.child->next);
            // }
            ASTNode* leaf = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next);
            addASTChildren(astIOStmtWriteNode,leaf);
            return astIOStmtWriteNode;
            break;
        }
        case 51: {
            ASTNode* astTermNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);

            ASTNode* astArithmeticExpressionNode = constructASTHelperInherited(parseTreeNode->NODE_TYPE.NL.child->next,astTermNode);
            return astArithmeticExpressionNode;
            break;
        }
        case 54: {
            ASTNode* astFactorNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);

            ASTNode* astTermNode = constructASTHelperInherited(parseTreeNode->NODE_TYPE.NL.child->next,astFactorNode);
            return astTermNode;
            break;
        }
        case 57: {
            ASTNode* astArithmeticExpressionNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            return astArithmeticExpressionNode;
            break;
        }
        case 58: {
            ASTNode* astAllNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);
            return astAllNode;
            break;
        }
        case 63: {
            ASTNode* leaf = createASTNode(1,astNum);
            leaf->AST_NODE_TYPE.AST_NUM.VALUE = parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
            return leaf;
            break;
        }
        case 64: {
            ASTNode* leaf = createASTNode(1,astRnum);
            leaf->AST_NODE_TYPE.AST_NUM.VALUE = parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
            return leaf;
            break;
        }
        case 65: {
            // Store the ID for now, will look into field later
            // For later implementation => Should already exist on symbol table
            ASTNode* leaf = createASTNode(1,astId);
            leaf->AST_NODE_TYPE.AST_ID.ID = parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
            leaf->AST_NODE_TYPE.AST_ID.FIELD_ID = getIdentifier(parseTreeNode->NODE_TYPE.NL.child->next);
            leaf->next = NULL;
            return leaf;
            break;
        }
        case 68: {
            ASTNode* astBooleanExpressionNode1 = createASTNode(0,astBooleanExpression);
            ASTNode* astBooleanExpressionNode2 = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            astBooleanExpressionNode1->AST_NODE_TYPE.AST_BOOLEAN_EXPRESSION.OPERATOR = getOperator(parseTreeNode->NODE_TYPE.NL.child->next->next->next);
            ASTNode* astBooleanExpressionNode3 = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next->next->next->next);

            addASTChildren(astBooleanExpressionNode1,astBooleanExpressionNode2);
            addASTChildren(astBooleanExpressionNode1,astBooleanExpressionNode3);

            return astBooleanExpressionNode1;
            break;
        }
        case 69: {
            ASTNode* astBooleanExpressionNode = createASTNode(0,astBooleanExpression);
            ASTNode* astVarNode1 = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child);
            astBooleanExpressionNode->AST_NODE_TYPE.AST_BOOLEAN_EXPRESSION.OPERATOR = getOperator(parseTreeNode->NODE_TYPE.NL.child->next);
            ASTNode* astVarNode2 = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next);

            addASTChildren(astBooleanExpressionNode,astVarNode1);
            addASTChildren(astBooleanExpressionNode,astVarNode2);

            return astBooleanExpressionNode;
            break;
        }
        case 70: {
            ASTNode* astBooleanExpressionNode = createASTNode(0,astBooleanExpression);
            astBooleanExpressionNode->AST_NODE_TYPE.AST_BOOLEAN_EXPRESSION.OPERATOR = parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
            ASTNode* astBooleanExpressionNode2 = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next);

            addASTChildren(astBooleanExpressionNode,astBooleanExpressionNode2);
            return astBooleanExpressionNode;
            break;
        }
        case 71: {
            ASTNode* leaf = createASTNode(1,astId);
            leaf->AST_NODE_TYPE.AST_ID.ID = parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
            leaf->AST_NODE_TYPE.AST_ID.FIELD_ID = NULL;
            return leaf;
            break;
        }
        case 72: {
            ASTNode* leaf = createASTNode(1,astNum);
            leaf->AST_NODE_TYPE.AST_NUM.VALUE = parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
            return leaf;
            break;
        }
        case 73: {
            ASTNode* leaf = createASTNode(1,astRnum);
            leaf->AST_NODE_TYPE.AST_RNUM.VALUE = parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
            return leaf;
            break;
        }
        case 82: {
            ASTNode* astReturnStmtNode = createASTNode(0,astReturnStmt);
            astReturnStmtNode->AST_NODE_TYPE.AST_RETURN_STMT.RETURN_LINE_NO = parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK->LINE_NO;
            ASTNode* idList = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            addASTChildren(astReturnStmtNode,idList);
            return astReturnStmtNode;
            break;
        }
        case 83: {
            ASTNode* astOptionalReturnNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            return astOptionalReturnNode;
            break;
        }
        case 84: {
            return NULL;
            break;
        }
        case 85: {
            ASTNode* astIdNode = createASTNode(0,astId);
            astIdNode->AST_NODE_TYPE.AST_ID.ID = parseTreeNode->NODE_TYPE.NL.child->NODE_TYPE.L.TK;
            astIdNode->AST_NODE_TYPE.AST_ID.FIELD_ID = NULL;
            ASTNode* astMoreIdsNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next);
            astIdNode->next = astMoreIdsNode;
            return astIdNode;
            break;
        }
        case 86: {
            ASTNode* astIdNode = createASTNode(0,astId);
            astIdNode->AST_NODE_TYPE.AST_ID.ID = parseTreeNode->NODE_TYPE.NL.child->next->NODE_TYPE.L.TK;
            astIdNode->AST_NODE_TYPE.AST_ID.FIELD_ID = NULL;
            ASTNode* astMoreIdsNode = constructASTHelper(parseTreeNode->NODE_TYPE.NL.child->next->next);
            astIdNode->next = astMoreIdsNode;
            return astIdNode;
            break;
        }
        case 87: {
            return NULL;
            break;
        }

    }

    free(parseTreeNode);
}

void printAST(ASTNode* root) {
    if(root->IS_LEAF == 1) {
        printf("Label of Node is %s\n" , LabelMap[root->LABEL]);
        return;
    }

    ASTNode* trav = root->children;
    if(trav != NULL) {
        printAST(root->children);
        trav = trav->next;
    }
    printf("Label of Node is %s\n" , LabelMap[root->LABEL]);

    while(trav != NULL) {
        printAST(trav);
        trav = trav->next;
    }

}

int getASTNodeCount() {
    return astNodeCount;
}

int getASTMemory() {
    return astMemory;
}
