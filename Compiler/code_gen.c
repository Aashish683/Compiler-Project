/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

/** Command to execute in Nasm (Updated) **/
/** nasm -felf64 code.asm && gcc code.o && ./a.out **/

#include "interface.h"
#include "symbol_table.h"
#include "code_gen.h"
#include "type_checker.h"
#include <string.h>

// A global loop counter to assign unique labels to loops
int loopCounter = 0;
// A global conditional counter to assign unique labels to conditions
int condCounter = 0;
// A global counter to assign unique labels during comparisions
int ltCounter = 0;
int leCounter = 0;
int gtCounter = 0;
int geCounter = 0;
int eqCounter = 0;
int neCounter = 0;

void printLeaf(ASTNode* leafNode,FILE* f) {

    // If there is no field ID
    if(leafNode->LABEL == astId && leafNode->AST_NODE_TYPE.AST_ID.FIELD_ID == NULL)
        fprintf(f,"%s" ,leafNode->AST_NODE_TYPE.AST_ID.ID->LEXEME);
    // If there is a field id
    else if(leafNode->LABEL == astId) {
        // TODO HANDLE LATER
        SymbolEntry* variableEntry = lookupSymbolEntry(leafNode->SCOPED_TABLE,leafNode->AST_NODE_TYPE.AST_ID.ID);
        SymbolEntry* recordEntry = lookupSymbolEntry(leafNode->SCOPED_TABLE,variableEntry->SYMBOL_ENTRY_TYPE.VARIABLE_ENTRY.DATA_TYPE);
        Token** fields = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.FIELDS;
        int itr = 0;
        for(itr = 0; itr < recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS; itr++) {
            if(strcmp(fields[itr]->LEXEME,leafNode->AST_NODE_TYPE.AST_ID.FIELD_ID->LEXEME) == 0) {
                // Match found
                break;
            }
        }

        if(itr == recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS) {
            printf("Field not present in record, semantic error should have been thrown\n");
        }

        fprintf(f,"%s + 2*%d" ,leafNode->AST_NODE_TYPE.AST_ID.ID->LEXEME,itr);

    }
    else if(leafNode->LABEL == astNum) {
        fprintf(f,"%s" ,leafNode->AST_NODE_TYPE.AST_NUM.VALUE->LEXEME);
    }
    else if(leafNode->LABEL == astRnum) {
        fprintf(f,"%s" ,leafNode->AST_NODE_TYPE.AST_RNUM.VALUE->LEXEME);
    }
    else {
        printf("printLeaf called on a non leaf,not correct\n");
    }

}

// RecordArithmetic is a flag parameter which is 0 if it is not record arithmetic, and n (number of fields) when it is
void codeGenerationHelper(ASTNode* node, SymbolTable* st, FILE* f, int recordArithmetic) {

    Label label = node->LABEL;
    switch(label) {
        case astProgram: {
            fprintf(f,"section .text\n");
            fprintf(f,"\tglobal main\n");
            fprintf(f,"\textern scanf\n");
            fprintf(f,"\textern printf\n\n");
			fprintf(f,"section .data\n");
            fprintf(f,"\tinpformat:  db \"%%hd\",0\n");
            fprintf(f,"\toutformat:  db \"%%hd\",10,0,\n");
            break;
        }
        case astFunction: {
            // No action
            break;
        }
        case astInputParams: {
            printf("Code gen cases should not involve astInputParams\n");
            break;
        }
        case astOutputParams: {
            printf("Code gen cases should not involve astOutputParams\n");
            break;
        }
        case astDatatype: {
            break;
        }
        case astStmts: {

            // Add a main prior to handling statements
            ASTNode* trav = node->children;
            int firstFlag = 0;
			int maxFields = 0; // A variable to store the maximum fields in a struct for the given program  								//(Used to define a temporary region for record arithmetic)
            while(trav != NULL) {
                if(firstFlag == 0 && ((trav->LABEL == astAssignmentStmt) || (trav->LABEL == astConditionalStmt) || (trav->LABEL == astIterativeStmt) || (trav->LABEL == astIOStmtRead) || (trav->LABEL == astIOStmtWrite) || (trav->LABEL == astReturnStmt) )) {
                    firstFlag = 1;

					// Declare buffer region if maxFields is > 0
					if(maxFields > 0) {
						fprintf(f,"\tbuffer : \n");
						for(int i = 0; i < maxFields; i++) {
							fprintf(f,"\t\tdw 0\n");
						}
					}
					// Print main to indicate start of the main routine
                    fprintf(f,"\n\nmain:\n");
                }

				// If it is a type definition evaluate the maximum number of fields
				if(trav->LABEL == astTypeDefintion) {
					if(trav->CHILDREN_COUNT > maxFields)
						maxFields = trav->CHILDREN_COUNT;
				}

                codeGenerationHelper(trav,st,f,0);
                trav = trav->next;
            }

            return;
            break;
        }

        case astTypeDefintion: {
            // Case handled in astDeclarationCase
            return;
            break;
        }
        case astFieldDefinition: {
            // No action
            return;
            break;
        }
        case astDeclaration: {
            ASTNode* astIdNode = node->children;
            if(astIdNode->AST_NODE_TYPE.AST_ID.DATA_TYPE->TOKEN_NAME != TK_RECORDID)
                fprintf(f,"\t%s:\tdw\t1\n",astIdNode->AST_NODE_TYPE.AST_ID.ID->LEXEME);
            else {
                SymbolEntry* recordEntry = lookupSymbolEntry(node->SCOPED_TABLE,astIdNode->AST_NODE_TYPE.AST_ID.DATA_TYPE);
                int numberFields = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS;
                fprintf(f,"\t%s:\n",astIdNode->AST_NODE_TYPE.AST_ID.ID->LEXEME);
                for(int i = 0; i < numberFields; i++)
                    fprintf(f,"\t\tdw 0\n");
            }

            return;
            break;
        }
        case astAssignmentStmt: {

            fprintf(f,"push ax\n"); // Store contents of ax on stack

			// Evaluate the case when the lhs is a record type ID
			if(node->children->LABEL == astId && node->children->AST_NODE_TYPE.AST_ID.DATA_TYPE->TOKEN_NAME == TK_RECORDID) {
				if(node->children->next->LABEL == astArithmeticExpression) {
					SymbolEntry* recordEntry = lookupSymbolEntry(node->SCOPED_TABLE,node->children->AST_NODE_TYPE.AST_ID.DATA_TYPE);
					int numberFields = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS;
					codeGenerationHelper(node->children->next,st,f,numberFields);
				}
				else if(node->children->next->LABEL == astId) {
					ASTNode* astIdNode = node->children->next;
					SymbolEntry* recordEntry = lookupSymbolEntry(node->SCOPED_TABLE,astIdNode->AST_NODE_TYPE.AST_ID.DATA_TYPE);
					int numberFields = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS;
					// Push fields in the stack
					for(int i = 0; i < numberFields; i++) {
						fprintf(f,"push word [%s + 2*%d]\n" ,astIdNode->AST_NODE_TYPE.AST_ID.ID->LEXEME,i);
					}
				}
				else {
					printf("Code gen,Record arithmetic should involve an id or an arithmetic expression on the RHS\n");
				}

				// The calls above must have populated the stack in order of the fields
				// Thus the fields will be popped in reverse order
				SymbolEntry* recordEntry = lookupSymbolEntry(node->SCOPED_TABLE,node->children->AST_NODE_TYPE.AST_ID.DATA_TYPE);
				int numberFields = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS;
				int itr = numberFields-1;
				// Pop all fields into ax and move it into the record
				for(int i=itr; i >= 0; i--) {
					fprintf(f,"pop ax\n");
					fprintf(f,"mov [%s + 2*%d],ax\n" ,node->children->AST_NODE_TYPE.AST_ID.ID->LEXEME,i);
				}

				return;
				break;
			}

			// Cases when the lhs is not a record type ID
            // Evaluate arithmetic expression
            else if(node->children->next->LABEL == astArithmeticExpression)
                codeGenerationHelper(node->children->next,st,f,0);
            // Print if leaf astNum astRnum astId
            else if(node->children->next->LABEL == astNum) {
                fprintf(f,"mov ax, %s\n" ,node->children->next->AST_NODE_TYPE.AST_NUM.VALUE->LEXEME);
            }
            else if(node->children->next->LABEL == astRnum) {
                // Case not in test cases
            }
            else if(node->children->next->LABEL == astId) {
                fprintf(f,"mov ax, %s\n" ,node->children->next->AST_NODE_TYPE.AST_ID.ID->LEXEME);
            }

            fprintf(f,"mov ");
            fprintf(f,"[");
			printLeaf(node->children,f);
            fprintf(f,"], ");
            fprintf(f," ax\n");
            fprintf(f,"pop ax\n\n\n"); // Restore ax
            return;
            break;
        }
        case astFunCallStmt: {
            printf("Code gen cases should not involve astFunCallStmt\n");
            break;
        }
        case astIterativeStmt: {
            int currentLoop = loopCounter;
            loopCounter++;
            fprintf(f, "\nLOOP%d:\n", currentLoop);
            codeGenerationHelper(node->children,st,f,0); // Evaluate the boolean expression
            fprintf(f, "cmp ax,0\n");
            fprintf(f,"je ENDLOOP%d\n" ,currentLoop); // If the condition is now unsatisified (ax is zero) go to endloop
           	ASTNode* temp = node->children->next;
           	while(temp!=NULL){
            	codeGenerationHelper(temp,st,f,0);
               	temp = temp->next;
            }
            fprintf(f,"jmp LOOP%d\n" ,currentLoop);
            fprintf(f, "ENDLOOP%d: \n", currentLoop);

            return;
            break;
        }
        case astConditionalStmt: {
            int currentCond = condCounter; // Save value as it might get updated for statements inside
            condCounter++;
			codeGenerationHelper(node->children,st,f,0); // Evaluate boolean expression
			fprintf(f, "\ncmp ax, 0\n" ); // Compare to see if it is false
			fprintf(f, "je ELSE%d\n",currentCond); // If it is, just to an else clause

            ASTNode* trav = node->children->next;
            ASTNode* prev = trav; // Prev will store the pointer to elsePart in the end of the below loop
            // Printing the if part
            while(trav != NULL) {

                // Do not handle the else part here, that will be handled below
                if(trav->LABEL != astElsePart)
			        codeGenerationHelper(trav,st,f,0);

                prev = trav;
                trav = trav->next;
            }

            fprintf(f,"jmp ENDIF%d" ,currentCond); // After running the if statememts skip the else ones.

			fprintf(f, "\nELSE%d:\n",currentCond); // Starting the else part

            // Checking if else part exists (prev is storing an else)
            // If it does not, return!
            if(prev->LABEL != astElsePart) {
                fprintf(f,"\nENDIF%d:\n" ,currentCond);
                return;
            }

            // If it does
            // Printing the else part
            trav = prev->children;
            while(trav != NULL) {
                codeGenerationHelper(trav,st,f,0);
                trav = trav->next;
            }

            fprintf(f,"\nENDIF%d:\n" ,currentCond);
            return;
            break;
        }
        case astElsePart: {
            //no action
            break;
        }
        case astIOStmtRead: {

            fprintf(f,"push rsi\n");
            fprintf(f,"push rdi\n");
            fprintf(f,"push ax\n");
            fprintf(f,"mov rsi, ");
            printLeaf(node->children,f);
            fprintf(f,"\n");
            fprintf(f,"mov rdi, inpformat\n");
            fprintf(f,"mov al, 0\n");
            fprintf(f,"call scanf\n");
            fprintf(f,"pop ax\n");
            fprintf(f,"pop rdi\n");
            fprintf(f,"pop rsi\n\n\n");
            return;
            break;
        }
        case astIOStmtWrite: {

            fprintf(f,"push rdi\n");
            fprintf(f,"push rsi\n");
            fprintf(f,"push rax\n");
            fprintf(f,"push rcx\n");
            fprintf(f,"push ax\n");
            fprintf(f,"mov rsi, ");
            if(node->children->LABEL == astNum)
                printLeaf(node->children,f);
            else {
                fprintf(f,"[");
                printLeaf(node->children,f);
                fprintf(f,"]");
            }
            fprintf(f,"\n");
            fprintf(f,"mov rdi, outformat\n");
            fprintf(f,"mov al,0\n");
            fprintf(f,"call printf\n");
            fprintf(f,"pop ax\n");
            fprintf(f,"pop rcx\n");
            fprintf(f,"pop rax\n");
            fprintf(f,"pop rsi\n");
            fprintf(f,"pop rdi\n\n\n");
            return;
            break;
        }
        case astReturnStmt: {
            fprintf(f,"\n\nret\n");    // Set eax to write
            break;
        }
        case astInputArgs: {
            printf("Code gen cases should not involve astInputArgs\n");
            break;
        }
        case astOutputArgs: {
            printf("Code gen cases should not involve astOutputArgs\n");
            break;
        }
        case astArithmeticExpression: {

            // Saving contents of ax and bx prior to using them further
            fprintf(f,"push bx\n");

            // The final value of the expression is to be stored in arithmetic expression
            ASTNode* lhsNode = node->children;
            ASTNode* rhsNode = node->children->next;


			// Case when the arithmetic expression does not involve records
			if(recordArithmetic == 0) {

		        if(lhsNode->LABEL == astArithmeticExpression) {
		            codeGenerationHelper(lhsNode,st,f,0);
		        }
		        else if(lhsNode->LABEL == astNum) {
		            fprintf(f,"mov ax, %s\n" ,lhsNode->AST_NODE_TYPE.AST_NUM.VALUE->LEXEME);
		        }
		        else if(lhsNode->LABEL == astId) {

		            if(lhsNode->AST_NODE_TYPE.AST_ID.FIELD_ID == NULL) {
		                fprintf(f,"mov ax, [");
		                printLeaf(lhsNode,f);
		                fprintf(f,"]\n");
		            }
		            else {
		                fprintf(f,"mov ax, [");
		                printLeaf(lhsNode,f);
		                fprintf(f,"]\n");
		            }
		        }

		        // Store first intermediate result on stack
		        fprintf(f,"push ax\n");

		        if(rhsNode->LABEL == astArithmeticExpression) {
		            codeGenerationHelper(rhsNode,st,f,0);
		        }
		        else if(rhsNode->LABEL == astNum) {
		            fprintf(f,"mov ax, %s\n" ,rhsNode->AST_NODE_TYPE.AST_NUM.VALUE->LEXEME);
		        }
		        else if(rhsNode->LABEL == astId) {

		            if(rhsNode->AST_NODE_TYPE.AST_ID.FIELD_ID == NULL) {
		                fprintf(f,"mov ax, [");
		                printLeaf(rhsNode,f);
		                fprintf(f,"]\n");
		            }
		            else {
		                fprintf(f,"mov ax, [");
		                printLeaf(rhsNode,f);
		                fprintf(f,"]\n");
		            }
		        }

		        // Storing second intermediate result on stack
		        fprintf(f,"push ax\n");

		        // Storing first intermediate in ax and second intermediate in bx
		        fprintf(f,"pop bx\n");
		        fprintf(f,"pop ax\n");


		        Token* operator = node->AST_NODE_TYPE.AST_ARITHMETIC_EXPRESSION.OPERATOR;

		        if(operator->TOKEN_NAME == TK_MUL) {
		            fprintf(f,"push rbx\n");
		            fprintf(f,"mul rbx\n");
		            fprintf(f,"pop rbx\n");
		        }
		        else if(operator->TOKEN_NAME == TK_DIV) {
		            fprintf(f,"push rbx\n");
		            fprintf(f,"xor rdx,rdx\n"); // rdx must be zero prior to a divide operation
		            fprintf(f,"div rbx\n");
		            fprintf(f,"pop rbx\n");
		        }
		        else if(operator->TOKEN_NAME == TK_PLUS) {
		            fprintf(f,"add ax,bx\n");
		        }
		        else if(operator->TOKEN_NAME == TK_MINUS) {
		            fprintf(f,"sub ax,bx\n");
		        }

			}
			else {


				Token* operator = node->AST_NODE_TYPE.AST_ARITHMETIC_EXPRESSION.OPERATOR;
				
				if(operator->TOKEN_NAME == TK_PLUS) {

					if(lhsNode->LABEL == astArithmeticExpression) {
						// Get result of the lhs on the stack
						codeGenerationHelper(lhsNode,st,f,recordArithmetic);
					}
					else if(lhsNode->LABEL == astId) {
						SymbolEntry* recordEntry = lookupSymbolEntry(node->SCOPED_TABLE,lhsNode->AST_NODE_TYPE.AST_ID.DATA_TYPE);
						int numberFields = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS;
						// Push fields in the stack
						for(int i = 0; i < numberFields; i++) {
							fprintf(f,"push word [%s + 2*%d]\n" ,lhsNode->AST_NODE_TYPE.AST_ID.ID->LEXEME,i);
						}	
					}
					else {
						printf("Record arithmetic should involve records or arithmeticExpressions\n");
					}

					// Unload left operand into the temporary buffer in order (To do so we have to iterate in reverse)
					int numberFields = recordArithmetic;
					int iterator = numberFields-1;
					for(iterator = numberFields-1; iterator >= 0; iterator--) {
						fprintf(f,"pop ax\n");
						fprintf(f,"mov [buffer + 2*%d], ax\n",iterator);
					}

					if(rhsNode->LABEL == astArithmeticExpression) {
						codeGenerationHelper(rhsNode,st,f,recordArithmetic);
					}
					else if(rhsNode->LABEL == astId) {
						SymbolEntry* recordEntry = lookupSymbolEntry(node->SCOPED_TABLE,lhsNode->AST_NODE_TYPE.AST_ID.DATA_TYPE);
						int numberFields = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS;
						// Push fields in the stack
						for(int i = 0; i < numberFields; i++) {
							fprintf(f,"push word [%s + 2*%d]\n" ,rhsNode->AST_NODE_TYPE.AST_ID.ID->LEXEME,i);
						}	
					}
					else {
						printf("Record arithmetic should involve records or arithmeticExpressions\n");
					}

					// Evaluate and store back in the temporary region
					iterator = numberFields-1;
					for(iterator = numberFields-1; iterator >= 0; iterator--) {
						fprintf(f,"pop ax\n");
						fprintf(f,"add [buffer + 2*%d],ax\n",iterator);
					}

					// Push result in stack
					for(iterator = 0; iterator < numberFields; iterator++) {
						fprintf(f,"push word [buffer + 2*%d]\n",iterator);
					}

				}
				else if(operator->TOKEN_NAME == TK_MINUS) {

					if(lhsNode->LABEL == astArithmeticExpression) {
						// Get result of the lhs on the stack
						codeGenerationHelper(lhsNode,st,f,recordArithmetic);
					}
					else if(lhsNode->LABEL == astId) {
						SymbolEntry* recordEntry = lookupSymbolEntry(node->SCOPED_TABLE,lhsNode->AST_NODE_TYPE.AST_ID.DATA_TYPE);
						int numberFields = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS;
						// Push fields in the stack
						for(int i = 0; i < numberFields; i++) {
							fprintf(f,"push word [%s + 2*%d]\n" ,lhsNode->AST_NODE_TYPE.AST_ID.ID->LEXEME,i);
						}	
					}
					else {
						printf("Record arithmetic should involve records or arithmeticExpressions\n");
					}

					// Unload left operand into the temporary buffer in order (To do so we have to iterate in reverse)
					int numberFields = recordArithmetic;
					int iterator = numberFields-1;
					for(iterator = numberFields-1; iterator >= 0; iterator--) {
						fprintf(f,"pop ax\n");
						fprintf(f,"mov [buffer + 2*%d], ax\n",iterator);
					}

					if(rhsNode->LABEL == astArithmeticExpression) {
						codeGenerationHelper(rhsNode,st,f,recordArithmetic);
					}
					else if(rhsNode->LABEL == astId) {
						SymbolEntry* recordEntry = lookupSymbolEntry(node->SCOPED_TABLE,lhsNode->AST_NODE_TYPE.AST_ID.DATA_TYPE);
						int numberFields = recordEntry->SYMBOL_ENTRY_TYPE.RECORD_ENTRY.NUMBER_FIELDS;
						// Push fields in the stack
						for(int i = 0; i < numberFields; i++) {
							fprintf(f,"push word [%s + 2*%d]\n" ,rhsNode->AST_NODE_TYPE.AST_ID.ID->LEXEME,i);
						}	
					}
					else {
						printf("Record arithmetic should involve records or arithmeticExpressions\n");
					}

					// Evaluate and store back in the temporary region
					iterator = numberFields-1;
					for(iterator = numberFields-1; iterator >= 0; iterator--) {
						fprintf(f,"pop ax\n");
						fprintf(f,"sub [buffer + 2*%d],ax\n",iterator);
					}

					// Push result in stack
					for(iterator = 0; iterator < numberFields; iterator++) {
						fprintf(f,"push word [buffer + 2*%d]\n",iterator);
					}


				}
				else if(operator->TOKEN_NAME == TK_MUL) {
					// Case when both lhs and rhs are arithmetic expressions
					if(lhsNode->LABEL == astArithmeticExpression && rhsNode->LABEL == astArithmeticExpression) {
						Token* lhsType = getArithmeticExpressionType(lhsNode,NULL); // Passing NULL as code gen assumes no erros
						Token* rhsType = getArithmeticExpressionType(rhsNode,NULL); // Passing NULL as code gen assumes no errors
						// Case when the lhs yields the record
						if(lhsType->TOKEN_NAME == TK_RECORDID) {
							codeGenerationHelper(lhsNode,st,f,recordArithmetic); // Get result from lhs on stack
							int numberFields = recordArithmetic;
							int iterator;
							for(iterator = numberFields-1; iterator >= 0; iterator--) {
								fprintf(f,"pop ax\n");
								fprintf(f,"mov [buffer + 2*%d],ax\n" ,iterator);
							}

							codeGenerationHelper(rhsNode,st,f,0); // Get multiplier from rhs in ax

							for(iterator = numberFields-1; iterator >= 0; iterator--) {
								fprintf(f,"push ax\n"); // Save the multiplier as it will get replaced by the answer
								fprintf(f,"push bx\n"); // Save contents of rbx
								fprintf(f,"xor bx,bx\n"); // Clear register bx
								fprintf(f,"mov bx,[buffer + 2*%d]\n",iterator); // Move number from buffer to bx
								fprintf(f,"mul bx\n"); // Multiply bx with ax
								fprintf(f,"mov [buffer + 2*%d], ax\n",iterator); // Move the result in ax to the location again
								fprintf(f,"pop bx\n"); // Restore contents of bx
								fprintf(f,"pop ax\n"); // Restore multiplier in ax
							}


							// Store the result in the stack
							for(iterator = 0; iterator < numberFields; iterator++) {
								fprintf(f,"push word [buffer + 2*%d]\n" , iterator);
							}
						}
						// Case when the rhs yields the record
						else {
							codeGenerationHelper(rhsNode,st,f,recordArithmetic); // Get result from rhs on stack
							int numberFields = recordArithmetic;
							int iterator;
							for(iterator = numberFields-1; iterator >= 0; iterator--) {
								fprintf(f,"pop ax\n");
								fprintf(f,"mov [buffer + 2*%d],ax\n" ,iterator);
							}

							codeGenerationHelper(lhsNode,st,f,0); // Get multiplier from lhs in ax

							for(iterator = numberFields-1; iterator >= 0; iterator--) {
								fprintf(f,"push ax\n"); // Save the multiplier as it will get replaced by the answer
								fprintf(f,"push bx\n"); // Save contents of rbx
								fprintf(f,"xor bx,bx\n"); // Clear register bx
								fprintf(f,"mov bx,[buffer + 2*%d]\n",iterator); // Move number from buffer to bx
								fprintf(f,"mul bx\n"); // Multiply bx with ax
								fprintf(f,"mov [buffer + 2*%d], ax\n",iterator); // Move the result in ax to the location again
								fprintf(f,"pop bx\n"); // Restore contents of bx
								fprintf(f,"pop ax\n"); // Restore multiplier in ax
							}

							// Store the result in the stack
							for(iterator = 0; iterator < numberFields; iterator++) {
								fprintf(f,"push word [buffer + 2*%d]\n" , iterator);
							}

						}
					}
					// Case when the lhs is a record type
					else if(lhsNode->LABEL == astId && lhsNode->AST_NODE_TYPE.AST_ID.DATA_TYPE->TOKEN_NAME == TK_RECORDID) {
						int numberFields = recordArithmetic;
						int iterator;
						
						// Push contents of the record in the stack
						for(iterator = 0; iterator < numberFields; iterator++) {
							fprintf(f,"push word [%s + 2*%d]\n" ,lhsNode->AST_NODE_TYPE.AST_ID.ID->LEXEME,iterator);
						}

						for(iterator = numberFields-1; iterator >= 0; iterator--) {
							fprintf(f,"pop ax\n");
							fprintf(f,"mov [buffer + 2*%d],ax\n" ,iterator);
						}

						codeGenerationHelper(rhsNode,st,f,0); // Get multiplier from rhs in ax

						for(iterator = numberFields-1; iterator >= 0; iterator--) {
							fprintf(f,"push ax\n"); // Save the multiplier as it will get replaced by the answer
							fprintf(f,"push bx\n"); // Save contents of rbx
							fprintf(f,"xor bx,bx\n"); // Clear register bx
							fprintf(f,"mov bx,[buffer + 2*%d]\n",iterator); // Move number from buffer to bx
							fprintf(f,"mul bx\n"); // Multiply bx with ax
							fprintf(f,"mov [buffer + 2*%d], ax\n",iterator); // Move the result in ax to the location again
							fprintf(f,"pop bx\n"); // Restore contents of bx
							fprintf(f,"pop ax\n"); // Restore multiplier in ax
						}


						// Store the result in the stack
						for(iterator = 0; iterator < numberFields; iterator++) {
							fprintf(f,"push word [buffer + 2*%d]\n" , iterator);
						}				
					}
					// Case when the rhs is a record type
					else if(rhsNode->LABEL == astId && rhsNode->AST_NODE_TYPE.AST_ID.DATA_TYPE->TOKEN_NAME == TK_RECORDID) {
						int numberFields = recordArithmetic;
						int iterator;
						
						// Push contents of the record into the stack
						for(iterator = 0; iterator < numberFields; iterator++) {
							fprintf(f,"push word [%s+2*%d]\n",rhsNode->AST_NODE_TYPE.AST_ID.ID->LEXEME,iterator);
						}

						for(iterator = numberFields-1; iterator >= 0; iterator--) {
							fprintf(f,"pop ax\n");
							fprintf(f,"mov [buffer + 2*%d],ax\n" ,iterator);
						}

						codeGenerationHelper(lhsNode,st,f,0); // Get multiplier from lhs in ax

						for(iterator = numberFields-1; iterator >= 0; iterator--) {
							fprintf(f,"push ax\n"); // Save the multiplier as it will get replaced by the answer
							fprintf(f,"push bx\n"); // Save contents of rbx
							fprintf(f,"xor bx,bx\n"); // Clear register bx
							fprintf(f,"mov bx,[buffer + 2*%d]\n",iterator); // Move number from buffer to bx
							fprintf(f,"mul bx\n"); // Multiply bx with ax
							fprintf(f,"mov [buffer + 2*%d], ax\n",iterator); // Move the result in ax to the location again
							fprintf(f,"pop bx\n"); // Restore contents of bx
							fprintf(f,"pop ax\n"); // Restore multiplier in ax
						}

						// Store the result in the stack
						for(iterator = 0; iterator < numberFields; iterator++) {
							fprintf(f,"push word [buffer + 2*%d]\n" , iterator);
						}
					}
					// Case when only the lhs is an arithmeticExpression and rhs is NOT a record type => lhs will yeild a record
					else if(lhsNode->LABEL == astArithmeticExpression) {
						codeGenerationHelper(lhsNode,st,f,recordArithmetic); // Get result from lhs on stack
						int numberFields = recordArithmetic;
						int iterator;
						for(iterator = numberFields-1; iterator >= 0; iterator--) {
							fprintf(f,"pop ax\n");
							fprintf(f,"mov [buffer + 2*%d],ax\n" ,iterator);
						}

						codeGenerationHelper(rhsNode,st,f,0); // Get multiplier from rhs in ax

						for(iterator = numberFields-1; iterator >= 0; iterator--) {
							fprintf(f,"push ax\n"); // Save the multiplier as it will get replaced by the answer
							fprintf(f,"push bx\n"); // Save contents of rbx
							fprintf(f,"xor bx,bx\n"); // Clear register bx
							fprintf(f,"mov bx,[buffer + 2*%d]\n",iterator); // Move number from buffer to bx
							fprintf(f,"mul bx\n"); // Multiply bx with ax
							fprintf(f,"mov [buffer + 2*%d], ax\n",iterator); // Move the result in ax to the location again
							fprintf(f,"pop bx\n"); // Restore contents of bx
							fprintf(f,"pop ax\n"); // Restore multiplier in ax
						}


						// Store the result in the stack
						for(iterator = 0; iterator < numberFields; iterator++) {
							fprintf(f,"push word [buffer + 2*%d]\n" , iterator);
						}
					}
					// Case when only the rhs is an arithmeticExpression and lhs is NOT a record type => rhs will yield a record
					else if(rhsNode->LABEL == astArithmeticExpression) {
						codeGenerationHelper(rhsNode,st,f,recordArithmetic); // Get result from rhs on stack
						int numberFields = recordArithmetic;
						int iterator;
						for(iterator = numberFields-1; iterator >= 0; iterator--) {
							fprintf(f,"pop ax\n");
							fprintf(f,"mov [buffer + 2*%d],ax\n" ,iterator);
						}

						codeGenerationHelper(lhsNode,st,f,0); // Get multiplier from lhs in ax

						for(iterator = numberFields-1; iterator >= 0; iterator--) {
							fprintf(f,"push ax\n"); // Save the multiplier as it will get replaced by the answer
							fprintf(f,"push bx\n"); // Save contents of rbx
							fprintf(f,"xor bx,bx\n"); // Clear register bx
							fprintf(f,"mov bx,[buffer + 2*%d]\n",iterator); // Move number from buffer to bx
							fprintf(f,"mul bx\n"); // Multiply bx with ax
							fprintf(f,"mov [buffer + 2*%d], ax\n",iterator); // Move the result in ax to the location again
							fprintf(f,"pop bx\n"); // Restore contents of bx
							fprintf(f,"pop ax\n"); // Restore multiplier in ax
						}

						// Store the result in the stack
						for(iterator = 0; iterator < numberFields; iterator++) {
							fprintf(f,"push word [buffer + 2*%d]\n" , iterator);
						}
					}
					else {
						printf("Invalid case in record multiplication\n");
					}	
				}
				else if(operator->TOKEN_NAME == TK_DIV) {
					codeGenerationHelper(lhsNode,st,f,recordArithmetic); // Get result from lhs on stack
					int numberFields = recordArithmetic;
					int iterator;
					for(iterator = numberFields-1; iterator >= 0; iterator--) {
						fprintf(f,"pop ax\n");
						fprintf(f,"mov [buffer + 2*%d],ax\n" ,iterator);
					}

					codeGenerationHelper(rhsNode,st,f,0); // Get divider from rhs in ax

					for(iterator = numberFields-1; iterator >= 0; iterator--) {
						fprintf(f,"push ax\n"); // Save the divider as it will get replaced by the answer
						fprintf(f,"push bx\n"); // Save contents of rbx
						fprintf(f,"xor bx,bx\n"); // Clear register bx
						fprintf(f,"xor dx,dx\n"); // Clear register dx
						fprintf(f,"mov bx,[buffer + 2*%d]\n",iterator); // Move number from buffer to bx
						fprintf(f,"div bx\n"); // Multiply bx with ax
						fprintf(f,"mov [buffer + 2*%d], ax\n",iterator); // Move the result in ax to the location again
						fprintf(f,"pop bx\n"); // Restore contents of bx
						fprintf(f,"pop ax\n"); // Restore divider in ax
					}


					// Store the result in the stack
					for(iterator = 0; iterator < numberFields; iterator++) {
						fprintf(f,"push word [buffer + 2*%d]\n" , iterator);
					}
				}
				else {
					printf("Undefined operator in evaluating record-type arithmetic expression\n");
				}

			}

            // Restoring contents of bx
            fprintf(f,"pop bx\n");
            return;
            break;
        }
        case astBooleanExpression: {
            fprintf(f,"push bx\n");

            ASTNode* lhsNode = node->children;
            ASTNode* rhsNode = node->children->next;

            Token* operator = node->AST_NODE_TYPE.AST_BOOLEAN_EXPRESSION.OPERATOR;

            // bool --> bool logicalOp bool
            if(lhsNode->LABEL == astBooleanExpression && rhsNode->LABEL == astBooleanExpression) {
                codeGenerationHelper(lhsNode,st,f,0);
                fprintf(f,"push ax\n"); // Push the current value of ax
                codeGenerationHelper(rhsNode,st,f,0);
                fprintf(f,"push ax\n"); // Push the current value of ax

                fprintf(f,"pop ax\n");
                fprintf(f,"pop bx\n");

                // Will store ffffh in ax if both have that
                if(operator->TOKEN_NAME == TK_AND)
                    fprintf(f,"and ax,bx\n");
                // Will store ffffh in ax if at least one has that
                else if(operator->TOKEN_NAME == TK_OR)
                    fprintf(f,"or ax,bx\n");
                else
                    printf("Not supposed to reach here\n");

                fprintf(f,"pop bx\n");
                return;
            }
            // bool --> TK_NOT bool
            else if(rhsNode == NULL) {
                codeGenerationHelper(rhsNode,st,f,0);
                // Negate contents of ax
                fprintf(f,"not ax\n");
                fprintf(f,"pop bx\n");
                return;
            }
            // bool --> astId relOp astId
            else if(lhsNode->LABEL == astId && rhsNode->LABEL == astId){
                fprintf(f,"mov ax,[");
                printLeaf(lhsNode,f);
                fprintf(f,"]\n");
                fprintf(f,"mov bx,[");
                printLeaf(rhsNode,f);
                fprintf(f,"]\n");
            }
            // Left operand is an ID
            else if(lhsNode->LABEL == astId) {
                fprintf(f,"mov ax,[");
                printLeaf(lhsNode,f);
                fprintf(f,"]\n");
                fprintf(f,"mov bx,%s\n" ,rhsNode->AST_NODE_TYPE.AST_NUM.VALUE->LEXEME);
            }
            // Right operand is an ID
            else if(rhsNode->LABEL == astId) {
                fprintf(f,"mov ax,%s\n" ,lhsNode->AST_NODE_TYPE.AST_NUM.VALUE->LEXEME);
                fprintf(f,"mov bx,[");
                printLeaf(rhsNode,f);
                fprintf(f,"]\n");
            }
            // Both operands are numbers
            else {
                fprintf(f,"mov ax,%s\n" ,lhsNode->AST_NODE_TYPE.AST_NUM.VALUE->LEXEME);
                fprintf(f,"mov bx,%s\n", rhsNode->AST_NODE_TYPE.AST_NUM.VALUE->LEXEME);
            }


            /**Only come into this region for case of relationOps, otherwise return**/

            if(operator->TOKEN_NAME == TK_LT) {
                fprintf(f,"cmp ax,bx\n");
                fprintf(f,"jl LT%d\n" ,ltCounter);
                fprintf(f,"mov ax,0\n");
                fprintf(f,"jmp LT%d\n" ,ltCounter+1);
                fprintf(f,"LT%d : mov ax,0ffffh\n",ltCounter);
                fprintf(f,"LT%d : nop\n",ltCounter+1);
                ltCounter += 2;
            }
            else if(operator->TOKEN_NAME == TK_GT) {
                fprintf(f,"cmp ax,bx\n");
                fprintf(f,"jg GT%d\n" ,gtCounter);
                fprintf(f,"mov ax,0\n");
                fprintf(f,"jmp GT%d\n" ,gtCounter+1);
                fprintf(f,"GT%d : mov ax,0ffffh\n",gtCounter);
                fprintf(f,"GT%d : nop\n" ,gtCounter+1);
                gtCounter += 2;
            }
            else if(operator->TOKEN_NAME == TK_LE) {
                fprintf(f,"cmp ax,bx\n");
                fprintf(f,"jle LE%d\n" ,leCounter);
                fprintf(f,"mov ax,0\n");
                fprintf(f,"jmp LE%d\n" ,leCounter+1);
                fprintf(f,"LE%d : mov ax,0ffffh\n",leCounter);
                fprintf(f,"LE%d : nop\n" ,leCounter+1);
                leCounter += 2;
            }
            else if(operator->TOKEN_NAME == TK_GE) {
                fprintf(f,"cmp ax,bx\n");
                fprintf(f,"jge GE%d\n" ,geCounter);
                fprintf(f,"mov ax,0\n");
                fprintf(f,"jmp GE%d\n",geCounter+1);
                fprintf(f,"GE%d : mov ax,0ffffh\n",geCounter);
                fprintf(f,"GE%d : nop\n" ,geCounter+1);
                geCounter += 2;
            }
            else if(operator->TOKEN_NAME == TK_EQ) {
                fprintf(f,"cmp ax,bx\n");
                fprintf(f,"je EQ%d\n" ,eqCounter);
                fprintf(f,"mov ax,0\n");
                fprintf(f,"jmp EQ%d\n" ,eqCounter+1);
                fprintf(f,"EQ%d : mov ax,0ffffh\n",eqCounter);
                fprintf(f,"EQ%d : nop\n" ,eqCounter+1);
                eqCounter += 2;
            }
            else if(operator->TOKEN_NAME == TK_NE) {
                fprintf(f,"cmp ax,bx\n");
                fprintf(f,"jne NE%d\n" ,neCounter);
                fprintf(f,"mov ax,0\n");
                fprintf(f,"jmp NE%d\n",neCounter+1);
                fprintf(f,"NE%d : mov ax,0ffffh\n",neCounter);
                fprintf(f,"NE%d : nop\n" ,neCounter+1);
                neCounter += 2;
            }

            fprintf(f,"pop bx\n");
            return;
            break;
        }
        case astId: {
            // A primitive identifier
            if(node->AST_NODE_TYPE.AST_ID.FIELD_ID == NULL) {
                // fprintf(f,"%s",node->AST_NODE_TYPE.AST_ID.ID->LEXEME);
                fprintf(f,"mov ax, [%s]\n" ,node->AST_NODE_TYPE.AST_ID.ID->LEXEME);
            }
            // Case when ID is a field of a record
            else {
				fprintf(f,"mov ax, [");
				printLeaf(node,f);
				fprintf(f,"]\n");
            }

            break;
        }
        case astNum: {
            // Move the number to eax
            fprintf(f,"mov ax, %s\n" ,node->AST_NODE_TYPE.AST_NUM.VALUE->LEXEME);
            break;
        }
        case astRnum: {
            // Case won't be present in testcases
            break;
        }
    }

    // A general while loop which handles the children after the parent has been handled
    // In cases where the children are handled in the switch case itself, we return from the switch case itself
    ASTNode* trav = node->children;
    while(trav != NULL) {
        codeGenerationHelper(trav,st,f,0);
        trav = trav->next;
    }
}

// Takes as input the AST and outputs the result in an outfile
void codeGeneration(AST* ast, SymbolTable* st, FILE* f) {
    codeGenerationHelper(ast->root,st,f,0);
}
