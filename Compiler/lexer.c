/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "interface.h"
#include "lexerDef.h"
#include "keyword_table.h"
#include <string.h>

#define BUFFER_SIZE 4096 // Buffer Size kept same as the size of a disk block

// Pair of buffers for input
char* buffPair[2];
// Keeps track of the next buffer to be loaded
int bufferToBeLoaded = 0;
// Keeps track of the buffer currently being operated on, directly related to above variable, declared for readability
int currentBuffer = 1;

// Flag to check if input is exhausted, used to return EOF directly if getStream has depleted the input file
int inputExhausted = 0;

// File descriptor of the file which contains the code
int fp;

// Pointer which points to the beginning of the lexeme to which a token is to be assigned
char* lexemeBegin = NULL;
// Pointer which functions as a lookahead to determine the token of the current lexeme
char* forward = NULL;

// State of the dfa
int dfa_state = 0;

// Stores the line count
int lineCount = 1;
// A flag variable which indicates whether the next character to be read is being called after a retraction => It is needed to prevent a double count of \n after a retraction
int nextCharacterReadAfterRetraction = 0;

KeywordTable* kt;

void initializeBuffers(int f) {
    fp = f;
    buffPair[0] = (char*)malloc(BUFFER_SIZE*sizeof(char));
    buffPair[1] = (char*)malloc(BUFFER_SIZE*sizeof(char));
    lexemeBegin = NULL;
    forward = NULL;
    currentBuffer = 1;
    bufferToBeLoaded = 0;
    inputExhausted = 0;
    lineCount = 1;
    nextCharacterReadAfterRetraction = 0;
    dfa_state = 0;
}

// Takes the file descritor as input and initializes the buffer pairs
void initializeLexer(int f) {
    initializeBuffers(f);
    kt = initializeTable();
}

// Brings an input stream into the corresponding buffer, returns EOF on depletion of input.
int getInputStream() {
    if(inputExhausted != 0)
        return EOF;

    // Initialize the buffer to be loaded with EOFs
    memset(buffPair[bufferToBeLoaded],EOF ,BUFFER_SIZE);

    int res = read(fp,buffPair[bufferToBeLoaded],BUFFER_SIZE);
    if(res == 0 || res < BUFFER_SIZE) {
        inputExhausted = 1;
    }

    bufferToBeLoaded = 1 - bufferToBeLoaded;
    currentBuffer = 1 - currentBuffer;
    if(res == -1) {
        printf("Error: Input Buffers failed to be loaded\n");
        return -1;
    }
    return res;
}

// Returns the character being pointed by forward and then increments it.
// Also initializes lexemeBegin and forward in the beginning
// int ad = 0;
char nextChar() {
    // ad++;
    // printf("--- %d %d \n" , ad,lineCount);
    if(lexemeBegin == NULL && forward == NULL) {

        // Case when lexemeBegin and forward have not been initialized, call getInputStream for the first time
        int res = getInputStream();
        if(res == -1) {
            return EOF;
        }
        lexemeBegin = buffPair[currentBuffer];
        forward = buffPair[currentBuffer];
        char* curr_forward = forward;
        forward++;

        // Only increment lineCount if the character read is \n and it is not being double counted due to a retraction
        if(nextCharacterReadAfterRetraction == 0 && *curr_forward == '\n') {
            lineCount++;
        }

        // Unset the flag if is set
        if(nextCharacterReadAfterRetraction == 1)
            nextCharacterReadAfterRetraction = 0;

        // printf("HERDDFDF %c \n", *curr_forward);

        return *curr_forward;
    }

    // Increment forward and send the previous value
    char* curr_forward = forward;

    // printf("HERDDFDF %c \n", *curr_forward);

    // Checking buffer overflow prior to incrementing forward
    if(curr_forward - buffPair[currentBuffer] == BUFFER_SIZE-1) {
        int res = getInputStream();
        if(res == -1) {
            // Error in getting input stream!
            return EOF;
        }
        forward = buffPair[currentBuffer];
    }
    else if(*curr_forward == EOF) {
        return EOF;
    }
    else
        forward++;

    // Only increment lineCount if the character read is \n and it is not being double counted due to a retraction
    if(nextCharacterReadAfterRetraction == 0 && *curr_forward == '\n') {
        lineCount++;
    }

    // Unset the flag if it is set
    if(nextCharacterReadAfterRetraction == 1)
        nextCharacterReadAfterRetraction = 0;

    return *curr_forward;
}

// Utility function to copy a string between two char* pointers
char* copyString(char* start, char* end) {
    char* store = (char*)malloc((end-start+2)*sizeof(char));
    char* trav = start;
    int i = 0;
    // Note <= was erroneous , corrected to <
    while(trav < end) {
        store[i] = *trav;
        ++trav;
        ++i;
    }

    // DOUBT - As per the instructor, the lexeme should not include the end marker
    store[i] = '\0';
    return store;
}

// Accept by making lexemeBegin equal to forward
void accept() {
    lexemeBegin = forward;
}

// Perform retraction of forward by the input specified.
void retract(int amt) {
    while(amt > 0) {
        --forward;
        --amt;
    }

    nextCharacterReadAfterRetraction = 1;
}

// To check if a characters within a range of characters
int rangeMatch(char ch,char start, char end) {
    if(ch >= start && ch <= end)
        return 1;
    return 0;
}

// To match a character with a single character
int singleMatch(char ch, char chToEqual) {
    if(ch == chToEqual)
        return 1;
    return 0;
}

// Populates the fields of a token
Token* populateToken(Token* t,TokenName tokenName,char* lexeme,int lineNumber,int isNumber,Value* value) {
    t->TOKEN_NAME = tokenName;
    t->LINE_NO = lineNumber;
    t->IS_NUMBER = isNumber;
    t->LEXEME = lexeme;
    t->VALUE =value;
    return t;
}

// Use sscanf to convert to Integer and Float
int stringToInteger(char* str) {
    int num;
    sscanf(str,"%d",&num);
    return num;
}

float stringToFloat(char* str) {
    float f;
    sscanf(str,"%f",&f);
    return f;
}

Token* getToken() {

    // Set initial state of DFA to 0 for considering token
    dfa_state = 0;
    char c = 1; // Assigning some random value so that EOF does not come as garbage, created error in driver!
    Token* t = (Token*)malloc(sizeof(Token));
    int errorType; // Must be set before control is sent to the error state 55

    while(1) {
        // If nextChar returned EOF terminate getToken call by returning a NULL
        if(c == EOF)
            return NULL;

        // Temporary line to print state of dfa
        // printf("At State %d\n" ,dfa_state);

        // Stores the token which was intended to be sent before lexical error happened
        // Has to be set before passing control to the error state

        switch(dfa_state) {
            case 0: {
                c = nextChar();
                if(singleMatch(c,'<')) {
                    dfa_state = 16;
                }
                else if(singleMatch(c,'#')) {
                    dfa_state = 52;
                }
                else if(rangeMatch(c,'b','d')) {
                    dfa_state = 35;
                }
                else if(singleMatch(c,'a') || rangeMatch(c,'e','z')) {
                    dfa_state = 40;
                }
                else if(singleMatch(c,'_')) {
                    dfa_state = 47;
                }
                else if(singleMatch(c,'[')) {
                    dfa_state = 7;
                }
                else if(singleMatch(c,']')) {
                    dfa_state = 8;
                }
                else if(singleMatch(c,',')) {
                    dfa_state = 9;
                }
                else if(singleMatch(c,';')) {
                    dfa_state = 12;
                }
                else if(singleMatch(c,':')) {
                    dfa_state = 11;
                }
                else if(singleMatch(c,'.')) {
                    dfa_state = 10;
                }
                else if(singleMatch(c,'(')) {
                    dfa_state = 5;
                }
                else if(singleMatch(c,')')) {
                    dfa_state = 6;
                }
                else if(singleMatch(c,'+')) {
                    dfa_state = 1;
                }
                else if(singleMatch(c,'-')) {
                    dfa_state = 2;
                }
                else if(singleMatch(c,'*')) {
                    dfa_state = 3;
                }
                else if(singleMatch(c,'/')) {
                    dfa_state = 4;
                }
                else if(singleMatch(c,'~')) {
                    dfa_state = 13;
                }
                else if(singleMatch(c,'!')) {
                    dfa_state = 14;
                }
                else if(singleMatch(c,'>')) {
                    dfa_state = 22;
                }
                else if(singleMatch(c,'=')) {
                    dfa_state = 25;
                }
                else if(singleMatch(c,'@')) {
                    dfa_state = 27;
                }
                else if(singleMatch(c,'&')) {
                    dfa_state = 30;
                }
                else if(singleMatch(c,'%')) {
                    dfa_state = 33;
                }
                else if(rangeMatch(c,'0','9')) {
                    dfa_state = 42;
                }
                else if(singleMatch(c,' ') || singleMatch(c,'\f') || singleMatch(c,'\r') || singleMatch(c,'\t') || singleMatch(c,'\v')) {
                    // Advance lexemeBegin as the current lexeme should not include this character
                    lexemeBegin++;
                    dfa_state = 0;
                }
                else if(singleMatch(c,'\n')) {
                    // Advance lexemeBegin as the current lexeme should not include this character
                    lexemeBegin++;
                    dfa_state = 0;
                }
                else if(singleMatch(c,EOF)) {
                    // Indicates end of input
                    return NULL;
                }
                else {
                    printf("Line %d : Cannot recognize character %c \n" ,lineCount,c);
                    // Throw lexical error!
                    errorType = 6;
                    dfa_state = 55;
                }
                break;
            }
            case 1: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_PLUS,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 2: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_MINUS,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 3: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_MUL,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 4: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_DIV,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 5: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_OP,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 6: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_CL,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 7: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_SQL,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 8: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_SQR,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 9: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_COMMA,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 10: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_DOT,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 11: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_COLON,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 12: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_SEM,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 13: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_NOT,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 14: {
                c = nextChar();
                if(c == '=') {
                    dfa_state = 15;
                }
                else {
                    // Throw Lexical error
                    char* pattern = copyString(lexemeBegin, forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for != ?\n" ,lineCount,pattern);
                    free(pattern);
                    errorType = 3;
                    dfa_state = 55;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1);
                }
                break;
            }
            case 15: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_NE,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 16: {
                c = nextChar();
                if(c == '-') {
                    dfa_state = 17;
                }
                else if(c == '=') {
                    dfa_state = 20;
                }
                else {
                    dfa_state = 21;
                }
                break;
            }
            case 17: {
                c = nextChar();
                if(c == '-') {
                    dfa_state = 18;
                }
                else {
                    // Throw lexical error.
                    char* pattern = copyString(lexemeBegin, forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for <--- ?\n" ,lineCount,pattern);
                    free(pattern);
                    errorType = 3;
                    dfa_state = 55;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1);
                }
                break;
            }
            case 18: {
                c = nextChar();
                if(c == '-') {
                    dfa_state = 19;
                }
                else {
                    // Throw lexical error
                    char* pattern = copyString(lexemeBegin, forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for <--- ?\n" ,lineCount,pattern);
                    free(pattern);
                    errorType = 3;
                    dfa_state = 55;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1);
                }
                break;
            }
            case 19: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_ASSIGNOP,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 20: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_LE,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 21: {
                retract(1);
                char* lex = copyString(lexemeBegin,forward);

                if(c == '\n')
                    populateToken(t,TK_LT,lex,lineCount-1,0,NULL);
                else
                    populateToken(t,TK_LT,lex,lineCount,0,NULL);

                accept();
                return t;
                break;
            }
            case 22: {
                c = nextChar();
                if(c == '=') {
                    dfa_state = 23;
                }
                else {
                    dfa_state = 24;
                }
                break;
            }
            case 23: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_GE,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 24: {
                retract(1);
                char* lex = copyString(lexemeBegin,forward);
                if(c == '\n')
                    populateToken(t,TK_GT,lex,lineCount-1,0,NULL);
                else
                    populateToken(t,TK_GT,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 25: {
                c = nextChar();
                if(c == '=') {
                    dfa_state = 26;
                }
                else {
                    // Throw lexical error
                    char* pattern = copyString(lexemeBegin, forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for == ?\n" ,lineCount,pattern);
                    free(pattern);
                    errorType = 3;
                    dfa_state = 55;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1);
                }
                break;
            }
            case 26: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_EQ,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 27: {
                c = nextChar();
                if(c == '@') {
                    dfa_state = 28;
                }
                else {
                    // Throw lexical error
                    char* pattern = copyString(lexemeBegin, forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for @@@ ?\n" ,lineCount,pattern);
                    free(pattern);
                    errorType = 3;
                    dfa_state = 55;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1);
                }
                break;
            }
            case 28: {
                c = nextChar();
                if(c == '@') {
                    dfa_state = 29;
                }
                else {
                    // Throw lexical
                    char* pattern = copyString(lexemeBegin,forward);
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for @@@ ?\n" ,lineCount,pattern);
                    free(pattern);
                    errorType = 3;
                    dfa_state = 55;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1);
                }
                break;
            }
            case 29: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_OR,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 30: {
                c = nextChar();
                if(c == '&') {
                    dfa_state = 31;
                }
                else {
                    // Throw lexical error
                    char* pattern = copyString(lexemeBegin, forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for &&& ?\n" ,lineCount,pattern);
                    free(pattern);
                    errorType = 3;
                    dfa_state = 55;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1);
                }
                break;
            }
            case 31: {
                c = nextChar();
                if(c == '&') {
                    dfa_state = 32;
                }
                else {
                    // Throw lexical error
                    char* pattern = copyString(lexemeBegin, forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for &&& ?\n" ,lineCount,pattern);
                    free(pattern);
                    errorType = 3;
                    dfa_state = 55;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1);
                }
                break;
            }
            case 32: {
                char* lex = copyString(lexemeBegin,forward);
                populateToken(t,TK_AND,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 33: {
                c = nextChar();
                while(c != '\n' && c != EOF) {
                    c = nextChar();
                }
                dfa_state = 34;
                break;
            }
            case 34: {
                char* lex = copyString(lexemeBegin,forward);

                // Corner case =>  If the character which ended the comment is a \n, then the comment was one below the current lineCount
                if(c == '\n')
                    populateToken(t,TK_COMMENT,lex,lineCount-1,0,NULL);
                else
                    populateToken(t,TK_COMMENT,lex,lineCount,0,NULL);
                accept();

                return t;
                break;
            }
            case 35: {
                c = nextChar();
                if(rangeMatch(c,'2','7')) {
                    dfa_state = 36;
                }
                else if(rangeMatch(c,'a','z')) {
                    dfa_state = 40;
                }
                else {
                    dfa_state = 41;
                }
                break;
            }
            case 36: {
                c = nextChar();
                while(rangeMatch(c,'b','d'))
                    c = nextChar();

                if(rangeMatch(c,'2','7'))
                    dfa_state = 37;
                else {
                    dfa_state = 39;
                }
                break;
            }
            case 37: {
                c = nextChar();
                while(rangeMatch(c,'2','7'))
                    c = nextChar();

                if(!rangeMatch(c,'2','7') && !rangeMatch(c,'b','d')) {
                    dfa_state = 38;
                }
                else {
                    // Throw lexical error
                    printf("Line %d : two identifers are not allowed back to back without a break ?\n" ,lineCount);
                    errorType = 5;
                    dfa_state = 55;
                }
                break;
            }
            case 38: {
                retract(1);
                int identifierLength = forward - lexemeBegin + 1;
                if(identifierLength < 2) {
                    printf("Line %d : Identifier length is less than 2\n" , lineCount);
                    errorType = 4;
                    dfa_state = 55;
                }
                else if(identifierLength > 20) {
                    printf("Line %d : Identifier length is more than 20\n" ,lineCount);
                    errorType = 4;
                    dfa_state = 55;
                }
                else {
                    char* lex = copyString(lexemeBegin,forward);
                    if(c == '\n')
                        populateToken(t,TK_ID,lex,lineCount-1,0,NULL);
                    else
                        populateToken(t,TK_ID,lex,lineCount,0,NULL);
                    accept();
                    return t;
                }
                break;
            }
            case 39: {
                retract(1);
                char* lex = copyString(lexemeBegin,forward);
                // Corner case => If c is newline character, then the token was one above the current linecount
                if(c == '\n')
                    populateToken(t,TK_ID,lex,lineCount-1,0,NULL);
                else
                    populateToken(t,TK_ID,lex,lineCount,0,NULL);

                accept();
                return t;
                break;
            }
            case 40: {
                c = nextChar();
                while(rangeMatch(c,'a','z'))
                    c = nextChar();

                dfa_state = 41;
                break;
            }
            case 41: {
                // Resolve keyword clash!
                retract(1);
                char* lex = copyString(lexemeBegin,forward);
                Node* n = lookUp(kt,lex);
                if(n == NULL) {
                    if(c == '\n')
                        populateToken(t,TK_FIELDID,lex,lineCount-1,0,NULL);
                    else
                        populateToken(t,TK_FIELDID,lex,lineCount,0,NULL);
                }
                else {
                    if(c == '\n')
                        populateToken(t,n->TOKEN_NAME,lex,lineCount-1,0,NULL);
                    else
                        populateToken(t,n->TOKEN_NAME,lex,lineCount,0,NULL);
                }
                accept();
                return t;
                break;
            }
            case 42: {
                c = nextChar();
                while(rangeMatch(c,'0','9'))
                    c = nextChar();

                if(c == '.') {
                    dfa_state = 44;
                }
                else {
                    dfa_state = 43;
                }
                break;
            }
            case 43: {
                retract(1);
                char* lex = copyString(lexemeBegin,forward);
                Value* val = malloc(sizeof(Value));
                val->INT_VALUE = stringToInteger(lex);
                if(c == '\n')
                    populateToken(t,TK_NUM,lex,lineCount-1,1,val);
                else
                    populateToken(t,TK_NUM,lex,lineCount,1,val);
                accept();
                return t;
                break;
            }
            case 44: {
                c = nextChar();
                if(rangeMatch(c,'0','9')) {
                    dfa_state = 45;
                }
                else {
                    // Throw lexical error
                    char* pattern = copyString(lexemeBegin, forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for a real number ?\n" ,lineCount,pattern);
                    free(pattern);
                    errorType = 3;
                    dfa_state = 55;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1);
                }
                break;
            }
            case 45: {
                c = nextChar();
                if(rangeMatch(c,'0','9')) {
                    dfa_state = 46;
                }
                else {
                    // Throw lexical
                    char* pattern = copyString(lexemeBegin, forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for a real number ?\n" ,lineCount,pattern);
                    free(pattern);
                    errorType = 3;
                    dfa_state = 55;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1);
                }
                break;
            }
            case 46: {
                char* lex = copyString(lexemeBegin,forward);
                Value* val = (Value*)malloc(sizeof(Value));
                val->FLOAT_VALUE = stringToFloat(lex);
                populateToken(t,TK_RNUM,lex,lineCount,2,val);
                accept();
                return t;
                break;
            }
            case 47: {
                c = nextChar();
                if(rangeMatch(c,'a','z') || rangeMatch(c,'A','Z')) {
                    dfa_state = 48;
                }
                else {
                    // throw lexical error
                    char* pattern = copyString(lexemeBegin, forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for a function ID ?\n" ,lineCount,pattern);
                    free(pattern);
                    errorType = 3;
                    dfa_state = 55;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1);
                }
                break;
            }
            case 48: {
                c = nextChar();
                while(rangeMatch(c,'a','z') || rangeMatch(c,'A','Z'))
                    c = nextChar();

                if(rangeMatch(c,'0','9')) {
                    dfa_state = 49;
                }
                else {
                    dfa_state = 51;
                }
                break;
            }
            case 49: {
                c = nextChar();
                while(rangeMatch(c,'0','9'))
                    c = nextChar();

                dfa_state = 50;

                break;
            }
            case 50: {
                retract(1);
                int identifierLength = forward - lexemeBegin + 1;
                if(identifierLength > 30) {
                    printf("Line %d : Function identifier length exceeds 30 characters\n" ,lineCount);
                    errorType = 4;
                    dfa_state = 55;
                }
                else {
                    char* lex = copyString(lexemeBegin,forward);
                    if(c == '\n')
                        populateToken(t,TK_FUNID,lex,lineCount,0,NULL);
                    else
                        populateToken(t,TK_FUNID,lex,lineCount-1,0,NULL);
                    accept();
                    return t;
                }
                break;
            }
            case 51: {
                // Resolve keyword clash here
                retract(1);
                char* lex = copyString(lexemeBegin,forward);
                Node* n = lookUp(kt,lex);
                if(n == NULL) {
                    if(c == '\n')
                        populateToken(t,TK_FUNID,lex,lineCount-1,0,NULL);
                    else
                        populateToken(t,TK_FUNID,lex,lineCount,0,NULL);
                }
                else {
                    if(c == '\n')
                        populateToken(t,n->TOKEN_NAME,lex,lineCount-1,0,NULL);
                    else
                        populateToken(t,n->TOKEN_NAME,lex,lineCount,0,NULL);
                }
                accept();
                return t;
                break;
            }
            case 52: {
                c = nextChar();
                if(rangeMatch(c,'a','z')) {
                    dfa_state = 53;
                }
                else {
                    // Throw lexical error
                    char* pattern = copyString(lexemeBegin, forward-sizeof(char));
                    printf("Line %d : Cannot recognize pattern %s, Were you tring for a record ID ?\n" ,lineCount,pattern);
                    free(pattern);
                    errorType = 3;
                    dfa_state = 55;

                    // Retract because an unforseen character lead the lexer to this state, it can be a correct character which shouldl be included in the next token
                    retract(1);
                }
                break;
            }
            case 53: {
                c = nextChar();
                while(rangeMatch(c,'a','z'))
                    c = nextChar();

                dfa_state = 54;
                break;
            }
            case 54: {
                retract(1);
                char* lex = copyString(lexemeBegin,forward);
                if(c == '\n')
                    populateToken(t,TK_RECORDID,lex,lineCount-1,0,NULL);
                else
                    populateToken(t,TK_RECORDID,lex,lineCount,0,NULL);
                accept();
                return t;
                break;
            }
            case 55: {
                // Error State
                // Rationale 1 => Set a flag that error has reached for this input program so do not tokenize any further
                // Rationale 2 => Try to tokenize to the closest match and continue tokenizing further

                // Chosen Rationale => Panic mode, Travel up till a delimiter

                // Comment this, will bring it back to state 0
                // c = nextChar();
                // while(c != ';' && c !=  EOF && c != '\n') {
                //     c = nextChar();
                // }

                char* lex = copyString(lexemeBegin,forward);

                // A retraction only occurs if the errorType is 3, so check if the character read was a '\n'
                if(errorType == 3 && c == '\n')
                    populateToken(t,TK_ERR,lex,lineCount-1,errorType,NULL);
                else
                    populateToken(t,TK_ERR,lex,lineCount,errorType,NULL);
                accept();

                // Move back to the start state after setting lexemeBegin if an unidentifiable character causes the error
                return t;

                break;
            }
        }
    }

}

// Function to remove comments and leading whitespaces
void removeComments(char* testCaseFile, char* cleanFile) {
    int tcf = open(testCaseFile,O_RDONLY);
    // Commenting this as printing is required in the console
    // int cf = open(cleanFile,O_CREAT|O_WRONLY|O_TRUNC);
    initializeBuffers(tcf);
    // Check has 3 values
    // 1 => Indicates it encountered a newline
    // 0 => Indicates that the line has been confirmed to not be a comment
    // 2 => Indicates that the line is confirmed to be a comment
    int check = 0;
    char c;
    while((c = nextChar()) != EOF) {

        switch(check) {
            case 0: {
                if(c == ' ' || c == '\f' || c == '\r' || c == '\t' || c == '\v') {
                    write(1,&c,1);
                    check = 0;
                }
                else if(c == '%') {
                    check = 3;
                }
                else if(c == '\n') {
                    write(1,&c,1);
                    check = 0;
                }
                else {
                    write(1,&c,1);
                    check = 2;
                }
                break;
            }
            case 2: {
                write(1,&c,1);
                if(c == '\n')
                    check = 0;
                break;
            }
            case 3: {
                if(c == '\n') {
                    write(1,&c,1);
                    check = 0;
                }
                break;
            }
        }

    }

    close(tcf);
}

// Utility function to test the lexer from time to time
void printBuffers() {
    char c;

    while((c = nextChar()) != EOF) {
        printf("%c " ,c);
    }

    printf("\n");
}
