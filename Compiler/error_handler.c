/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

// This file contains the error management modules in semantic analysis

#include "interface.h"
#include "error_handler.h"
#include "type_checker.h"
#include <string.h>


// Function to initialize error list
ErrorList* initializeErrorList() {
    ErrorList* els = (ErrorList*)malloc(sizeof(ErrorList));
    els->head = NULL;
    els->tail = NULL;
    els->MESSAGE_MAX_LENGTH = ERROR_MAX_LENGTH;
    return els;
}

// Function to generate error at given line number and error message
Error* generateError(int lineNumber, int iterativeLineEnd,char* errorMessage) {
    Error* error = (Error*)malloc(sizeof(Error));
    error->LINE_NUMBER = lineNumber;
    error->ITERATIVE_LINE_END = iterativeLineEnd;
    error->ERROR_MESSAGE = errorMessage;
    error->next = NULL;
    return error;
}

// Function to add error to the list of errors
void addErrorToList(ErrorList* els, int lineNumber, char* errorMessage) {

    // Add the error only if it has not been reported before
    Error* trav = els->head;
    int found = 0;
    while(trav != NULL) {
        if(strcmp(trav->ERROR_MESSAGE,errorMessage) == 0 && trav->LINE_NUMBER == lineNumber) {
            found = 1;
            break;
        }
        trav = trav->next;
    }
    if(found == 1)
        return;

    Error* error = generateError(lineNumber,-1,errorMessage);

    // In case the list inside errorList is empty
    if(els->head == NULL) {
        els->head = error;
        els->tail = error;
        return;
    }

    els->tail->next = error;
    els->tail = els->tail->next;
}

void addIterativeErrorToList(ErrorList* els, int lineStart, int lineEnd, char* errorMessage) {
    Error* error = generateError(lineStart,lineEnd,errorMessage);
    // In case the list inside errorList is empty
    if(els->head == NULL) {
        els->head = error;
        els->tail = error;
        return;
    }

    els->tail->next = error;
    els->tail = els->tail->next;
}

void printErrors(ErrorList* els) {
    Error* trav = els->head;
    if(trav == NULL) {
        printf("No errors reported during Semantic Analysis \n");
        printf("Code compiles Successfully\n");
        return;
    }

    while(trav != NULL) {
        if(trav->ITERATIVE_LINE_END == -1)
            printf("Line %d : %s\n" ,trav->LINE_NUMBER,trav->ERROR_MESSAGE);
        else
            printf("Line %d-%d : %s\n", trav->LINE_NUMBER,trav->ITERATIVE_LINE_END,trav->ERROR_MESSAGE);
        trav = trav->next;
    }
}

void throwTypeMismatchError(Token* lhsType, Token* rhsType,ErrorList* els,int lineNumber) {
    char* message = (char*)malloc(sizeof(char)*els->MESSAGE_MAX_LENGTH);
    sprintf(message,"Types %s and %s not compatible for this operation\n", getDataType(lhsType),getDataType(rhsType));
    addErrorToList(els,lineNumber,message);
}

void throwMissingDeclarationError(Token* errorVariable,ErrorList* els) {
    char* message = (char*)malloc(sizeof(char)*els->MESSAGE_MAX_LENGTH);
    sprintf(message,"The variable %s is not declared\n" , errorVariable->LEXEME);
    addErrorToList(els,errorVariable->LINE_NO,message);
}

void throwMultipleDefinitionsError(Token* errorIdentifier, ErrorList* els) {
    char* message = (char*)malloc(sizeof(char)*els->MESSAGE_MAX_LENGTH);
    sprintf(message,"The identifier %s is declared more than once\n" , errorIdentifier->LEXEME);
    addErrorToList(els,errorIdentifier->LINE_NO,message);
}

void throwClashingGlobalDefinitionError(Token* errorIdentifier, ErrorList* els) {
    char* message = (char*)malloc(sizeof(char)*els->MESSAGE_MAX_LENGTH);
    sprintf(message,"The identifier %s is global anad cannot be declared again\n" , errorIdentifier->LEXEME);
    addErrorToList(els,errorIdentifier->LINE_NO,message);
}

void throwMissingFunctionDefinitionError(Token* errorFunCall, ErrorList* els) {
    char* message = (char*)malloc(sizeof(char)*els->MESSAGE_MAX_LENGTH);
    sprintf(message,"The function %s is undefined\n" ,errorFunCall->LEXEME);
    addErrorToList(els,errorFunCall->LINE_NO,message);
}

void throwMissingRecordDefinitionError(Token* errorRecord,ErrorList* els) {
    char* message = (char*)malloc(sizeof(char)*els->MESSAGE_MAX_LENGTH);
    sprintf(message,"The record %s is undefined\n" ,errorRecord->LEXEME);
    addErrorToList(els,errorRecord->LINE_NO,message);
}

void throwInvalidNumberOfInputArgsError(Token* errorFunCall, int actualNumber, int expectedNumber, ErrorList* els) {
    char* message = (char*)malloc(sizeof(char)*els->MESSAGE_MAX_LENGTH);
    sprintf(message,"The number of input arguments passed is %d , it should be %d \n", actualNumber,expectedNumber);
    addErrorToList(els,errorFunCall->LINE_NO,message);
}

void throwInvalidNumberOfOutputArgsError(Token* errorFunCall, int actualNumber, int expectedNumber, ErrorList* els) {
    char* message = (char*)malloc(sizeof(char)*els->MESSAGE_MAX_LENGTH);
    sprintf(message,"The number of output arguments passed is %d , it should be %d \n", actualNumber, expectedNumber);
    addErrorToList(els,errorFunCall->LINE_NO,message);
}

void throwInputArgumentTypeMismatchError(Token* errorFunCall, Token* typeExpected, Token* typePassed,int index,ErrorList* els) {
    char* message = (char*)malloc(sizeof(char)*els->MESSAGE_MAX_LENGTH);
    sprintf(message,"In inputArgs The type at index %d should have been %s rather than %s\n" ,index,typeExpected->LEXEME,typePassed->LEXEME);
    addErrorToList(els,errorFunCall->LINE_NO,message);
}

void throwOutputArgumentTypeMismatchError(Token* errorFunCall, Token* typeExpected, Token* typeRecieved,int index,ErrorList* els) {
    char* message = (char*)malloc(sizeof(char)*els->MESSAGE_MAX_LENGTH);
    sprintf(message,"In outputArgs The type at index %d should have been %s rather than %s\n" ,index,typeExpected->LEXEME,typeRecieved->LEXEME);
    addErrorToList(els,errorFunCall->LINE_NO,message);
}

void throwInvalidNumberOfReturnVariablesError(int lineNumber,int actualNumber, int expectedNumber,ErrorList* els) {
    char* message = (char*)malloc(sizeof(char)*els->MESSAGE_MAX_LENGTH);
    sprintf(message,"The number of variables being returned is %d , instead of %d\n" , actualNumber,expectedNumber);
    addErrorToList(els,lineNumber,message);
}

void throwReturnTypeMismatchError(Token* errorId, Token* typeReturned, Token* typeExpected, ErrorList* els) {
    char* message = (char*)malloc(sizeof(char)*els->MESSAGE_MAX_LENGTH);
    sprintf(message,"The type %s of the variable being returned %s does not match with the expected type %s\n" ,typeReturned->LEXEME,typeReturned->LEXEME,typeExpected->LEXEME);
    addErrorToList(els,errorId->LINE_NO,message);
}

void throwNoIterationUpdateError(int lineStart, int lineEnd, ErrorList* els) {
    char* message = (char*)malloc(sizeof(char)*els->MESSAGE_MAX_LENGTH);
    sprintf(message,"None of the variables in the condition of the while loop get updated");
    addIterativeErrorToList(els,lineStart,lineEnd,message);
}

void throwRecursiveFunctionCallError(Token* errorFunCall, ErrorList* els) {
    char* message = (char*)malloc(sizeof(char)*els->MESSAGE_MAX_LENGTH);
    sprintf(message,"The function call is recursive\n");
    addErrorToList(els,errorFunCall->LINE_NO,message);
}
