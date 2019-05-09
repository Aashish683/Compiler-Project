/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

// This file handles the printing utilities required by driver

#include "interface.h"
#include "keyword_table.h"
#include "lexer.h"
#include "parser.h"
#include "nary_tree.h"
#include "ast.h"
#include "symbol_table.h"
#include "type_checker.h"
#include "error_handler.h"
#include "semantic_analyzer.h"
#include "code_gen.h"
#include "printer.h"


Queue* createQueue() {
  Queue* q = (Queue*)malloc(sizeof(Queue));
  q->head = NULL;
  q->tail = NULL;
  q->size = 0;
}

node* createNodee(ASTNode* n, int depth,char* parent) {
    node* na = (node*)malloc(sizeof(node));
    na->v = n;
    na->depth = depth;
    na->parent = parent;
    na->next = NULL;
    return na;
}

void enqueue(Queue* q,ASTNode* v,int depth,char* parent) {
  if(q == NULL)
    return;

  node* n = createNodee(v,depth,parent);

  if(q->head == NULL) {
    q->head = n;
    q->tail = n;
    q->size += 1;
    return;
  }

  q->tail->next = n;
  q->tail = n;
  q->size += 1;
}

node* dequeue(Queue* q) {
  if(q == NULL)
    return NULL;

  if(q->head == NULL)
    return NULL;


  node* v = q->head;
  q->head = q->head->next;
  q->size -= 1;
  return v;
}

int isEmptyQ(Queue* q) {
  if(q->size == 0)
    return 1;
  return 0;
}

void levelPrint(ASTNode* root) {
    Queue* q = createQueue();
    int currentDepth = 0;
    printf("------LEVEL 0-----\n");
    enqueue(q,root,0,NULL);
    while(!isEmptyQ(q)) {
        node* top = dequeue(q);
        ASTNode* n = top->v;
        int depth = top->depth;
        if(depth > currentDepth) {
            printf("\n");
            printf("\n");
            printf("-----LEVEL %d-----\n" ,depth);
            currentDepth = depth;
        }
        Scope scope = (n->SCOPED_TABLE != NULL) ? n->SCOPED_TABLE->SCOPE : "-1";
        if(currentDepth == 0)
            printf("astProgram , Scope = %s \n" ,scope);
        else {
            printf("(%s,Parent = %s" , getLabel(top->v->LABEL),top->parent);
            if(n->LABEL == astId) {
                printf(", ID = %s " ,n->AST_NODE_TYPE.AST_ID.ID->LEXEME);

                if(n->AST_NODE_TYPE.AST_ID.FIELD_ID != NULL)
                    printf(", FIELD_ID = %s ", n->AST_NODE_TYPE.AST_ID.FIELD_ID->LEXEME);
                if(n->AST_NODE_TYPE.AST_ID.DATA_TYPE != NULL)
                    printf(", DATA_TYPE = %s " ,n->AST_NODE_TYPE.AST_ID.DATA_TYPE->LEXEME);
                else
                    printf(", DATA_TYPE = -1 ");
            }
            else if(n->LABEL == astNum) {
                printf(", NUM = %s " ,n->AST_NODE_TYPE.AST_NUM.VALUE->LEXEME);
            }
            else if(n->LABEL == astRnum) {
                printf(", RNUM = %s ", n->AST_NODE_TYPE.AST_RNUM.VALUE->LEXEME);
            }
            else if(n->LABEL == astArithmeticExpression) {
                printf(", Operator = %s " ,n->AST_NODE_TYPE.AST_ARITHMETIC_EXPRESSION.OPERATOR->LEXEME);
            }
            else if(n->LABEL == astBooleanExpression) {
                printf(", Operator = %s " ,n->AST_NODE_TYPE.AST_BOOLEAN_EXPRESSION.OPERATOR->LEXEME);
            }
            else if(n->LABEL == astIterativeStmt) {
                printf(", START LINE = %d, END LINE = %d " ,n->AST_NODE_TYPE.AST_ITERATIVE_STMT.LINE_NO_START,n->AST_NODE_TYPE.AST_ITERATIVE_STMT.LINE_NO_END);
            }

            printf(", Scope = %s\n" ,scope);
        }
        ASTNode* trav = n->children;
        while(trav != NULL) {
            enqueue(q,trav,depth+1,getLabel(n->LABEL));
            trav = trav->next;
        }
    }
}


// Utility function to print SymbolEntryList
void printSymbolEntryList(SymbolEntry* ls, int isGlobalTable) {
    SymbolEntry* trav = ls;
    while(trav != NULL) {
        if(trav->SYMBOL_LABEL == symbolVariable) {
            printf("Entry is a variable of type %s\n" ,trav->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE->LEXEME);
            printf("Variable is %s, it is %s\n" ,trav->SYMBOL_TOKEN->LEXEME,((isGlobalTable == 1) ? "global" : "not global"));
            printf("Offset is %d\n" ,trav->SYMBOL_OFFSET);
            printf("\n");
        }
        else if(trav->SYMBOL_LABEL == symbolRecord) {
            printf("Entry is a record of type %s\n", trav->SYMBOL_TOKEN->LEXEME);
            Token** dataType = trav->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.DATA_TYPE;
            int numFields = trav->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS;
            for(int i=0; i < numFields; i++) {
                printf("The field is %s of type %s\n" , trav->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.FIELDS[i]->LEXEME,trav->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.DATA_TYPE[i]->LEXEME);
                printf("\n");
            }
            printf("\n");
        }
        else if(trav->SYMBOL_LABEL == symbolFunction) {
            printf("Entry is a function\n");
            printf("Function name is %s\n" , trav->SYMBOL_TOKEN->LEXEME);
            printf("The Scope of this function is indicated by %p\n" , trav->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.SCOPED_TABLE);
            // Recursive call (Indirect) to printSymbolTable
            printf("---------------------------------\n");
            printSymbolTable(trav->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.SCOPED_TABLE,0);
            printf("---------------------------------\n");
        }
        else if(trav->SYMBOL_LABEL == symbolParameter) {
            printf("Entry is a parameter of type %s\n" ,trav->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.DATA_TYPE->LEXEME);
            printf("Entry is an %s parameter\n" , ((trav->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.IS_INPUT == 1) ? "Input" : "Output") );
            printf("Parameter is %s\n" ,trav->SYMBOL_TOKEN->LEXEME);
            printf("Offset is %d\n" ,trav->SYMBOL_OFFSET);
            printf("\n");
        }

        trav = trav->next;
    }
}

void printGlobals(SymbolTable* st) {
    printf("Name                      Type                          Offset\n");
    for(int i=0; i < st->NUMBER_SLOTS; i++) {
        SymbolEntry* trav = st->SYMBOL_SLOTS[i];
        while(trav != NULL) {
            if(trav->SYMBOL_LABEL == symbolVariable) {
                printf("%s           %s              %d\n" ,trav->SYMBOL_TOKEN->LEXEME, trav->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE->LEXEME, trav->SYMBOL_OFFSET);
            }
            trav = trav->next;
        }
    }
}

void printFunctions(SymbolTable* st) {

    printf("Name                                       Memory\n");
    for(int i=0; i < st->NUMBER_SLOTS; i++) {
        SymbolEntry* trav = st->SYMBOL_SLOTS[i];
        while(trav != NULL) {
            if(trav->SYMBOL_LABEL == symbolFunction) {
                printf("%s                                  %d\n" ,trav->SYMBOL_TOKEN->LEXEME, trav->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.SCOPED_TABLE->CURRENT_OFFSET);
            }
            trav = trav->next;
        }
    }

}

void printRecords(SymbolTable* st) {
    printf("Name                  Data types                         Width\n");
    for(int i=0; i < st->NUMBER_SLOTS; i++) {
        SymbolEntry* trav = st->SYMBOL_SLOTS[i];
        while(trav != NULL) {
            if(trav->SYMBOL_LABEL == symbolRecord) {
                printf("%s                " ,trav->SYMBOL_TOKEN->LEXEME);
                Token** dataTypes = trav->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.DATA_TYPE;
                int numberFields = trav->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS;
                for(int i=0; i < numberFields; i++) {
                    printf("%s," ,dataTypes[i]->LEXEME);
                }
                printf("                                  %d\n" , trav->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.TOTAL_OFFSET);
            }
            trav = trav->next;
        }
    }
}


void printSymbolTableHelper(SymbolTable* scopedTable) {
    for(int i=0; i < scopedTable->NUMBER_SLOTS; i++) {
        SymbolEntry* trav = scopedTable->SYMBOL_SLOTS[i];
        while(trav != NULL) {
            if(trav->SYMBOL_LABEL == symbolVariable) {

                if(trav->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE->TOKEN_NAME == TK_RECORDID) {
                    SymbolEntry* recordEntry = lookupSymbolEntry(scopedTable->parent,trav->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE);
                    // No record of that lexeme exists
                    if(recordEntry == NULL) {
                        trav = trav->next;
                        continue;
                    }
                    printf("%s           ",trav->SYMBOL_TOKEN->LEXEME);
                    Token** dataTypes = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.DATA_TYPE;
                    int numberFields = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS;
                    for(int i=0; i < numberFields; i++) {
                        printf("%s," ,dataTypes[i]->LEXEME);
                    }
                    printf("               %s                   %d\n" ,scopedTable->SCOPE,trav->SYMBOL_OFFSET);
                }
                else {
                    printf("%s           ",trav->SYMBOL_TOKEN->LEXEME);
                    printf("%s" ,trav->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE->LEXEME);
                    printf("               %s                   %d\n" ,scopedTable->SCOPE,trav->SYMBOL_OFFSET);
                }

            }
            else if(trav->SYMBOL_LABEL == symbolParameter) {

                if(trav->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.DATA_TYPE->TOKEN_NAME == TK_RECORDID) {
                    SymbolEntry* recordEntry = lookupSymbolEntry(scopedTable->parent,trav->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.DATA_TYPE);
                    // No record of that lexeme exists
                    if(recordEntry == NULL) {
                        trav = trav->next;
                        continue;
                    }
                    printf("%s           ",trav->SYMBOL_TOKEN->LEXEME);
                    Token** dataTypes = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.DATA_TYPE;
                    int numberFields = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS;
                    for(int i=0; i < numberFields; i++) {
                        printf("%s," ,dataTypes[i]->LEXEME);
                    }
                    printf("               %s                   %d\n" ,scopedTable->SCOPE,trav->SYMBOL_OFFSET);
                }
                else {
                    printf("%s           ",trav->SYMBOL_TOKEN->LEXEME);
                    printf("%s" ,trav->SYMBOL_ENTRY_TYPE.PARAMETER_ENTRY.DATA_TYPE->LEXEME);
                    printf("               %s                   %d\n" ,scopedTable->SCOPE,trav->SYMBOL_OFFSET);
                }

            }
            trav = trav->next;
        }
    }
}
// Utility function to print Symbol table
void printSymbolTable(SymbolTable* st,int isGlobalTable) {

    printf("Lexeme                  type              scope                 offset\n");
    for(int i=0; i < st->NUMBER_SLOTS; i++) {
        SymbolEntry* trav = st->SYMBOL_SLOTS[i];
        while(trav != NULL) {
            if(trav->SYMBOL_LABEL == symbolVariable) {

                if(trav->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE->TOKEN_NAME == TK_RECORDID) {
                    SymbolEntry* recordEntry = lookupSymbolEntry(st,trav->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE);
                    // No record of that lexeme exists
                    if(recordEntry == NULL) {
                        trav = trav->next;
                        continue;
                    }
                    printf("%s           ",trav->SYMBOL_TOKEN->LEXEME);
                    Token** dataTypes = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.DATA_TYPE;
                    int numberFields = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS;
                    for(int i=0; i < numberFields; i++) {
                        printf("%s," ,dataTypes[i]->LEXEME);
                    }
                    printf("               global                   ---\n");
                }
                else {
                    printf("%s           ",trav->SYMBOL_TOKEN->LEXEME);
                    printf("%s" ,trav->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE->LEXEME);
                    printf("               global                   ---\n");
                }

            }
            trav = trav->next;
        }
    }


    for(int i=0; i < st->NUMBER_SLOTS; i++) {
        SymbolEntry* trav = st->SYMBOL_SLOTS[i];
        while(trav != NULL) {
            if(trav->SYMBOL_LABEL == symbolFunction) {
                printSymbolTableHelper(trav->SYMBOL_ENTRY_TYPE.FUNCTION_ENTRY.SCOPED_TABLE);
            }
            trav = trav->next;
        }
    }
}


// int main(int argc, char* argv[]) {
//     Grammar* g = extractGrammar();
//     FirstAndFollow* fafl = computeFirstAndFollowSets(g);
//     ParsingTable* pTable = initialiseParsingTable();
//     createParseTable(fafl,pTable);
//     ParseTree* pt = parseInputSourceCode(argv[1],pTable,fafl);
//     AST* ast = constructAST(pt);
//     levelPrint(ast->root);
//     printf("\n");
//     printf("---PRINTED AST----\n");
//     printf("\n");
//     printf("\n");
//     printf("\n");
//     printf("\n");
//     printf("\n");

//     printf("-------MAKING AND PRINTING SYMBOL TABLE------\n");

//     ErrorList* els = initializeErrorList();

//     SymbolTable* st = constructSymbolTable(ast,els);

//     printSymbolTable(st,1);

//     printf("\n");
//     printf("\n");
//     printf("\n");
//     printf("----PRINTING AST TO SEE IF SCOPE FIELD HAS BEEN POPULATED-----\n");

//     levelPrint(ast->root);

//     printf("\n");
//     printf("\n");
//     printf("\n");
//     printf("----COMMENCING TYPE CHECKING----\n");


//     captureErrors(ast,els);

//     printf("----TYPE CHECKING COMPLETE-----\n");


//     printf("\n");
//     printf("\n");

//     printf("---PRINTING SYMBOL TABLE WITH OFFSETS CALCULATED---\n");
//     printSymbolTable(st,1);
//     printf("---PRINTING SYMBOL TABLE COMPLETE---\n");

//     printf("----PRINTING AST AGAIN-----\n");
//     printf("\n");
//     printf("\n");
//     printf("\n");
//     levelPrint(ast->root);
//     printf("\n");
//     printf("----PRINTING AST COMPLETED----\n");

//     printf("\n");
//     printf("\n");
//     printf("----PRINTING ERRORS-----\n");
//     printErrors(els);

//     printf("---PRINTING ERRORS COMPLETE----\n");

//     FILE* f = fopen("code1.asm","w");
//     codeGeneration(ast,st,f);
// }
