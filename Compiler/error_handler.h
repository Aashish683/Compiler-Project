/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "error_handlerDef.h"

ErrorList* initializeErrorList();

Error* generateError(int lineNumber, int iterativeLineEnd,char* errorMessage);
void addErrorToList(ErrorList* els, int lineNumber, char* errorMessage);

// There are 14 types of errors, but in some cases more than one kind of error is captured
// Overall we are at LEVEL 4 of semantic analysis

// Type mismatch covers 2 errors in itself including the normal typeMismatch, like a division of a scalar by a record
void throwTypeMismatchError(Token* lhsType, Token* rhsType,ErrorList* els, int lineNumber);
void throwMissingDeclarationError(Token* errorVariable,ErrorList* els);
// Multiple definitions covers all clashing cases eg - functions, variables and records.
void throwMultipleDefinitionsError(Token* errorIdentifier, ErrorList* els);
void throwClashingGlobalDefinitionError(Token* errorIdentifier, ErrorList* els);
void throwMissingFunctionDefinitionError(Token* errorFunCall, ErrorList* els);
void throwRecursiveFunctionCallError(Token* errorFunCall, ErrorList* els);
void throwMissingRecordDefinitionError(Token* errorRecord,ErrorList* els);
void throwInvalidNumberOfInputArgsError(Token* errorFunCall, int actualNumber, int expectedNumber, ErrorList* els);
void throwInvalidNumberOfOutputArgsError(Token* errorFunCall, int actualNumber, int expectedNumber, ErrorList* els);
void throwInputArgumentTypeMismatchError(Token* errorFunCall, Token* typeExpected, Token* typePassed,int index,ErrorList* els);
void throwOutputArgumentTypeMismatchError(Token* errorFunCall, Token* typeExpected, Token* typeRecieved,int index,ErrorList* els);
void throwInvalidNumberOfReturnVariablesError(int lineNumber,int actualNumber, int expectedNumber,ErrorList* els);
void throwReturnTypeMismatchError(Token* errorId, Token* typeReturned, Token* typeExpected, ErrorList* els);
void throwNoIterationUpdateError(int lineStart, int lineEnd, ErrorList* els);

void printErrors(ErrorList* els);
