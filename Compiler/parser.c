/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "interface.h"
#include "parserDef.h"
#include "lexer.h"
#include "nary_tree.h"
#include "stack.h"
#include <string.h>
#define GRAMMAR_FILE "GRAMMAR.txt"
#define TOTAL_GRAMMAR_NONTERMINALS 49 // TODO get the actual number of nonterminals
#define TOTAL_GRAMMAR_TERMINALS 56
#define TOTAL_GRAMMAR_RULES 87 //TODO actual number of rules

Grammar* g; // g is a record that keeps track of the Grammar

NonTerminalRuleRecords** ntrr; // Mantains records of the starting rule number and ending rule number of the non terminal indicated by it's array index
int checkIfDone[TOTAL_GRAMMAR_NONTERMINALS] = {0}; // A global structure to check if the First has been calculated for the corresponding non terminal
int vectorSize = TOTAL_GRAMMAR_TERMINALS+1; // Calculate the size of vectors for first and follow

int syntaxErrorFlag;
int lexicalErrorFlag;

// An array of strings which stores the terminals in the same order as the enum, this is used to get the enum identifier of the TerminalID
char* TerminalID[] = {
    "TK_ASSIGNOP",
    "TK_COMMENT",
    "TK_ID",
    "TK_NUM",
    "TK_RNUM",
    "TK_FIELDID",
    "TK_FUNID",
    "TK_RECORDID",
    "TK_WITH",
    "TK_PARAMETERS",
    "TK_END",
    "TK_WHILE",
    "TK_TYPE",
    "TK_MAIN",
    "TK_GLOBAL",
    "TK_PARAMETER",
    "TK_LIST",
    "TK_SQL",
    "TK_SQR",
    "TK_INPUT",
    "TK_OUTPUT",
    "TK_INT",
    "TK_REAL",
    "TK_COMMA",
    "TK_SEM",
    "TK_COLON",
    "TK_DOT",
    "TK_ENDWHILE",
    "TK_OP",
    "TK_CL",
    "TK_IF",
    "TK_THEN",
    "TK_ENDIF",
    "TK_READ",
    "TK_WRITE",
    "TK_RETURN",
    "TK_PLUS",
    "TK_MINUS",
    "TK_MUL",
    "TK_DIV",
    "TK_CALL",
    "TK_RECORD",
    "TK_ENDRECORD",
    "TK_ELSE",
    "TK_AND",
    "TK_OR",
    "TK_NOT",
    "TK_LT",
    "TK_LE",
    "TK_EQ",
    "TK_GT",
    "TK_GE",
    "TK_NE",
    "TK_EPS",
    "TK_DOLLAR",
    "TK_ERR"
};

// An array of strings which stores the non terminals in the same order as the enum, this is used to get the enum identifier of the NonTerminalID
char* NonTerminalID[] = {
    "program",
    "mainFunction",
	"otherFunctions",
	"function",
	"input_par",
	"output_par",
	"parameter_list",
	"dataType",
	"primitiveDatatype",
	"constructedDatatype",
	"remaining_list",
	"stmts",
	"typeDefinitions",
	"typeDefinition",
	"fieldDefinitions",
	"fieldDefinition",
	"moreFields",
	"declarations",
	"declaration",
	"global_or_not",
	"otherStmts",
	"stmt",
	"assignmentStmt",
	"singleOrRecId",
    "new_24",
	"funCallStmt",
	"outputParameters",
	"inputParameters",
	"iterativeStmt",
	"conditionalStmt",
	"elsePart",
	"ioStmt",
	"arithmeticExpression",
	"expPrime",
	"term",
	"termPrime",
	"factor",
	"highPrecedenceOperators",
	"lowPrecedenceOperators",
    "all",
	"temp",
    "booleanExpression",
    "var",
	"logicalOp",
	"relationalOp",
	"returnStmt",
	"optionalReturn",
	"idList",
	"more_ids"
};

//Utility function to copy a lexeme
char* copyLexeme(char* str) {
    int len = strlen(str);
    char* lex = (char*)malloc(sizeof(char)*(len+1));
    for(int i=0; i < len; i++)
        lex[i] = str[i];

    lex[len] = '\0';
    return lex;
}

// Utility function to append a character to symbol string
char* appendToSymbol(char* str, char c) {
    int len = strlen(str);
    char* strConcat = (char*)malloc(sizeof(char)*(len+2));
    for(int i=0; i < len; i++)
        strConcat[i] = str[i];

    strConcat[len] = c;
    strConcat[len+1] = '\0';
    return strConcat;
}

// Returns the Enum ID of the string in the NonTerminal ID map if found, otherwise return -1
int findInTerminalMap(char* str) {
    for(int i=0; i < TOTAL_GRAMMAR_TERMINALS; i++) {
        if(strcmp(str,TerminalID[i]) == 0)
            return i;
    }

    return -1;
}

// Returns the Enum ID of the string in the NonTerminalID map if found, otherwise returns -1
int findInNonTerminalMap(char* str) {
    for(int i=0; i < TOTAL_GRAMMAR_NONTERMINALS; i++) {
        if(strcmp(str,NonTerminalID[i]) == 0)
            return i;
    }

    return -1;
}


// Returns the string corresponding to the enumId (Required when printing is too be done outside parser.c)

char* getTerminal(int enumId) {
    return TerminalID[enumId];
}

char* getNonTerminal(int enumId) {
    return NonTerminalID[enumId];
}

ParsingTable* initialiseParsingTable() {
    ParsingTable* pt = (ParsingTable*)malloc(sizeof(ParsingTable));
    pt->entries = (int**)malloc(TOTAL_GRAMMAR_NONTERMINALS*sizeof(int*));
    for(int i=0; i < TOTAL_GRAMMAR_NONTERMINALS; i++) {
        // Calloc used to initialise with 0 by default, if left empty => error state
        pt->entries[i] = (int*)calloc(TOTAL_GRAMMAR_TERMINALS,sizeof(int));
    }
    return pt;
}

// initialise the Grammar according to the number of non terminals and total rules
int initialiseGrammar() {

    g = (Grammar*)malloc(sizeof(Grammar));
    g->GRAMMAR_RULES_SIZE = TOTAL_GRAMMAR_RULES+1; // 1 added as 0 index is left as NULL to provide direct mapping by rule number to the rule
    g->GRAMMAR_RULES = (Rule**)malloc(sizeof(Rule*)*g->GRAMMAR_RULES_SIZE);
    g->GRAMMAR_RULES[0] = NULL;
}

// Initialises a symbol structure based on the symbol string extracted from the grammar file
Symbol* intialiseSymbol(char* symbol) {

    Symbol* s = (Symbol*)malloc(sizeof(Symbol));
        // Search for enum IDs in both maps
        int idNonTerminal, idTerminal;
        idNonTerminal = findInNonTerminalMap(symbol);
        // If idNonTerminal is found, assign it as the symbol type
        if(idNonTerminal != -1) {
            s->TYPE.NON_TERMINAL = idNonTerminal;
            s->IS_TERMINAL = 0;
        }
        else {
            idTerminal = findInTerminalMap(symbol);
            if(idTerminal != -1) {
                s->TYPE.TERMINAL = idTerminal;
                s->IS_TERMINAL = 1;
            }
        }

    s->next = NULL;

    return s;
}

SymbolList* initialiseSymbolList() {
    SymbolList* sl = (SymbolList*)malloc(sizeof(SymbolList));
    sl->HEAD_SYMBOL = NULL;
    sl->TAIL_SYMBOL = NULL;
    sl->RULE_LENGTH = 0;
    return sl;
}

Rule* initialiseRule(SymbolList* sl, int ruleCount) {
    Rule* r = (Rule*)malloc(sizeof(Rule));
    r->SYMBOLS = sl;
    r->RULE_NO = ruleCount;
    return r;
}

NonTerminalRuleRecords** intialiseNonTerminalRecords() {
    NonTerminalRuleRecords** ntrr = (NonTerminalRuleRecords**)malloc(sizeof(NonTerminalRuleRecords*)*TOTAL_GRAMMAR_NONTERMINALS);
    return ntrr;
}

void initialiseCheckIfDone() {
    for(int i=0; i < TOTAL_GRAMMAR_NONTERMINALS; i++)
        checkIfDone[i] = 0;
}

FirstAndFollow* initialiseFirstAndFollow() {
    FirstAndFollow* fafl = (FirstAndFollow*)malloc(sizeof(FirstAndFollow));

    // Initialize the array of vectors to be equal to the total number of Non terminals
    fafl->FIRST = (int**)malloc(sizeof(int*)*TOTAL_GRAMMAR_NONTERMINALS);
    fafl->FOLLOW = (int**)malloc(sizeof(int*)*TOTAL_GRAMMAR_NONTERMINALS);


    for(int i=0; i < TOTAL_GRAMMAR_NONTERMINALS; i++) {
        // Calloc used to initialize the vectors to 0
        fafl->FIRST[i] = (int*)calloc(vectorSize,sizeof(int));
        fafl->FOLLOW[i] = (int*)calloc(vectorSize,sizeof(int));
    }

    return fafl;

}

// Calculates the First of the Symbol s and it's corresponding bit vector is populated by using the enum_id
void calculateFirst(int** firstVector, int enumId) {

    // printf("Stack overflow being caused by %s\n" , NonTerminalID[enumId]);
    int start = ntrr[enumId]->start;
    int end = ntrr[enumId]->end;
    int producesNull = 0; // Flag which tracks whether the non terminal produces NULL

    for(int i=start; i <= end; i++) {
        Rule* r = g->GRAMMAR_RULES[i];
        Symbol* s = r->SYMBOLS->HEAD_SYMBOL;
        Symbol* trav = s;
        Symbol* nextSymbol = trav->next;
        int ruleYieldsEpsilon = 1;
        while(nextSymbol != NULL) {

            // Case when a terminal is encountered in the RHS
            if(nextSymbol->IS_TERMINAL == 1) {
                if(nextSymbol->TYPE.TERMINAL != TK_EPS) {
                    ruleYieldsEpsilon = 0;
                    firstVector[enumId][nextSymbol->TYPE.TERMINAL] = 1;
                }
                break;
            }

            // Case when it is a Non-terminal

            // Check if it's First has been calculated already, if not calculate it
            // In case of stack overflow, for debugging add a condition that nextSymbol should not habe the same ID as the enumId
            if(checkIfDone[nextSymbol->TYPE.NON_TERMINAL] == 0) {
                calculateFirst(firstVector,nextSymbol->TYPE.NON_TERMINAL);
            }

            for(int j=0; j < vectorSize; j++) {
                if(firstVector[nextSymbol->TYPE.NON_TERMINAL][j] == 1)
                    firstVector[s->TYPE.NON_TERMINAL][j] = 1;
            }

            if(firstVector[nextSymbol->TYPE.NON_TERMINAL][TK_EPS] == 0) {
                ruleYieldsEpsilon = 0;
                break;
            }

            nextSymbol = nextSymbol->next;
        }

        if(ruleYieldsEpsilon)
            producesNull = 1;
    }

    if(producesNull)
        firstVector[enumId][TK_EPS] = 1;
    else
        firstVector[enumId][TK_EPS] = 0;

    checkIfDone[enumId] = 1;

}

void populateFirst(int** firstVector, Grammar* g) {

    // Traversal is done by enum_id (which is iterator i in this case)
    // Grammar Rules are written in GRAMMAR_FILE in the same order as enum name as per convention

    for(int i=0; i < TOTAL_GRAMMAR_NONTERMINALS; i++) {
        if(checkIfDone[i] == 0)
            calculateFirst(firstVector,i);
    }
}

void populateFollow(int** followVector, int** firstVector, Grammar* g) {


    for(int i=1; i <= TOTAL_GRAMMAR_RULES; i++) {
        Rule* r = g->GRAMMAR_RULES[i];
        Symbol* head = r->SYMBOLS->HEAD_SYMBOL;
        Symbol* trav = head->next;
        int epsilonIdentifier = 0;
        while(trav != NULL) {

            if(trav->IS_TERMINAL == 0) {
                Symbol* followTrav = trav->next;
                while(followTrav != NULL) {
                    if(followTrav->IS_TERMINAL == 1 && followTrav->TYPE.TERMINAL != TK_EPS) {
                        followVector[trav->TYPE.NON_TERMINAL][followTrav->TYPE.TERMINAL] = 1;
                        break;
                    }
                    else {

                        for(int j=0; j < vectorSize; j++)
                            if(firstVector[followTrav->TYPE.NON_TERMINAL][j] == 1 && j != TK_EPS)
                                followVector[trav->TYPE.NON_TERMINAL][j] = 1;

                        if(firstVector[followTrav->TYPE.NON_TERMINAL][TK_EPS] == 0)
                            break;

                    }
                    followTrav = followTrav->next;
                }

                // Case when we need to take LHS Non terminal
                // Venkat => followTrav != NULL && followTrav->next == NULL
                if(trav->next == NULL || (followTrav == NULL)) {
                    for(int j=0; j < vectorSize; j++)
                        if(followVector[head->TYPE.NON_TERMINAL][j] == 1 && j != TK_EPS)
                            followVector[trav->TYPE.NON_TERMINAL][j] = 1;
                }

            }


            trav = trav->next;
        }
    }
}

// Function to keep populating the followVector until it stabilises
void populateFollowTillStable(int** followVector, int** firstVector, Grammar* g) {
    int** prevFollowVector = (int**)malloc(TOTAL_GRAMMAR_NONTERMINALS*sizeof(int*));

    for(int i=0; i < TOTAL_GRAMMAR_NONTERMINALS; i++) {
        prevFollowVector[i] = (int*)calloc(vectorSize,sizeof(int));
    }

    followVector[program][TK_DOLLAR] = 1;
    prevFollowVector[program][TK_DOLLAR] = 1;

    while(1) {

        populateFollow(followVector,firstVector,g);
        int stable = 1;

        for(int i=0; i < TOTAL_GRAMMAR_NONTERMINALS; i++) {
            for(int j=0; j < vectorSize; j++) {
                if(prevFollowVector[i][j] != followVector[i][j])
                    stable = 0;
            }
        }

        if(stable)
            break;

        for(int i=0; i < TOTAL_GRAMMAR_NONTERMINALS; i++) {
            for(int j=0; j < vectorSize; j++)
                prevFollowVector[i][j] = followVector[i][j];
        }
    }
}


FirstAndFollow* computeFirstAndFollowSets(Grammar* g) {
    FirstAndFollow* fafl = initialiseFirstAndFollow();
    populateFirst(fafl->FIRST,g);
    populateFollowTillStable(fafl->FOLLOW,fafl->FIRST,g);
    return fafl;
}

void createParseTable(FirstAndFollow* fafl, ParsingTable* pt) {

    for(int i=1; i <= TOTAL_GRAMMAR_RULES; i++) {
        Rule* r = g->GRAMMAR_RULES[i];
        int lhsNonTerminal = r->SYMBOLS->HEAD_SYMBOL->TYPE.NON_TERMINAL;

        // THIS IS INCORRECT AS THE TERMINALS BEING IN FIRST FROM OTHER RULES ALSO GET DIRECTED TO THIS RULE!
        // for(int j=0; j < TOTAL_GRAMMAR_TERMINALS; j++) {
        //     if(fafl->FIRST[lhsNonTerminal],[j] == 1) {
        //         // Since it is LL(1), no other non-epsilon producing rule will direct to an entry already filled
        //         // Ask Venkat to verify
        //         // Attempt to correct error, epsilon producing rule directs it back to this entry
        //         if(pt->entries[lhsNonTerminal][j] == 0)
        //             pt->entries[lhsNonTerminal][j] = r->RULE_NO;
        //     }
        // }

        Symbol* rhsHead = r->SYMBOLS->HEAD_SYMBOL->next;
        Symbol* trav = rhsHead;
        int epsilonGenerated = 1;

        while(trav != NULL) {
            // Terminal encountered in RHS => It cannot generate epsilon, break!
            if(trav->IS_TERMINAL == 1 && trav->TYPE.TERMINAL != TK_EPS) {
                epsilonGenerated = 0;
                pt->entries[lhsNonTerminal][trav->TYPE.TERMINAL] = r->RULE_NO;
                break;
            }
            else if(trav->IS_TERMINAL == 1 && trav->TYPE.TERMINAL == TK_EPS) {
                // No action
                epsilonGenerated = 1;
                break;
            }
            else {

                // For all the terminals in the first of this Non terminal set the ParsingTable entry
                // Note, no special treatment for epsilon as it will not be recieved from the input source code
                for(int j=0; j < TOTAL_GRAMMAR_TERMINALS; j++) {
                    if(fafl->FIRST[trav->TYPE.NON_TERMINAL][j] == 1)
                        pt->entries[lhsNonTerminal][j] = r->RULE_NO;
                }

                // Check if epsilon is generated by the first of this Non terminal, if not break, else continue
                if(fafl->FIRST[trav->TYPE.NON_TERMINAL][TK_EPS] == 0) {
                    epsilonGenerated = 0;
                    break;
                }
            }

            trav = trav->next;
        }

        // If epsilon is generated by the RHS string then we need to consider follow set of the LHS Non terminal
        if(epsilonGenerated) {
            for(int j=0; j < TOTAL_GRAMMAR_TERMINALS; j++) {
                if(fafl->FOLLOW[lhsNonTerminal][j] == 1)
                    pt->entries[lhsNonTerminal][j] = r->RULE_NO;
            }
        }
    }

}



// Appending at the tail of the list in O(1) using the tail pointer
void addToSymbolList(SymbolList* ls, Symbol* s) {
    Symbol* h = ls->HEAD_SYMBOL;
    // Case when the List is empty
    if(h == NULL) {
        ls->HEAD_SYMBOL = s;
        ls->TAIL_SYMBOL = s;
        ls->RULE_LENGTH = 1;
        return;
    }

    ls->TAIL_SYMBOL->next = s;
    ls->TAIL_SYMBOL = s;
    ls->RULE_LENGTH += 1;
}










// Extracts the grammar from GRAMMAR_FILE, return 1 on success, 0 on error
// Working rationale of the function
//  => Identify the LHS Non_terminal
//  => Keep making the Symbol List
//  => Extract the enum number of the LHS Non terminal.
Grammar* extractGrammar() {

    int ruleCount = 1; // Variable which will be used in assigning the rule numbers to the extracted rules
    int fd = open(GRAMMAR_FILE,O_RDONLY);
    char c; // Variable to store the character being read
    int actualRead; // Variable to store the number of bytes being read in the system call
    char* symbol = ""; // This will keep track of the current symbol by appending the read in character

    int symbolsRead = 0; // to keep track of the No of symbols read in a particular line or a rule
    int noOfLinesofNonTerminal = 0; // to keep track of the no the no of rules read of a particular non terminal
    Symbol* currentNonTerminal = NULL; // to keep track of the current Non Terminal
    SymbolList* sl = NULL; // Keeps track of the symbol list so far

    initialiseGrammar();
    ntrr = intialiseNonTerminalRecords();
    initialiseCheckIfDone();

    //create starting symbol list

    while((actualRead = read(fd,&c,sizeof(char))) != 0) {

        // If end of file is reached stop reading further
        if(c == EOF) {
            break;
        }

        // If a space is reached, it means a symbol has terminated and hence must be extracted
        if(c == ' ') {
            symbolsRead++;
            Symbol* s = intialiseSymbol(symbol);
            addToSymbolList(sl,s);

            if(symbolsRead == 1 ) {

                // Case when the rules of current non terminal are over
                if(currentNonTerminal == NULL) {
                    ntrr[s->TYPE.NON_TERMINAL] = (NonTerminalRuleRecords*)malloc(sizeof(NonTerminalRuleRecords));
                    ntrr[s->TYPE.NON_TERMINAL]->start = 1;
                }
                // Case when a new LHS Non terminal arrives
                else if(currentNonTerminal != NULL && currentNonTerminal->TYPE.NON_TERMINAL != s->TYPE.NON_TERMINAL) {
                    ntrr[currentNonTerminal->TYPE.NON_TERMINAL]->end = ruleCount-1;
                    ntrr[s->TYPE.NON_TERMINAL] = (NonTerminalRuleRecords*)malloc(sizeof(NonTerminalRuleRecords));
                    ntrr[s->TYPE.NON_TERMINAL]->start = ruleCount;
                }
                currentNonTerminal = s;
            }

            symbol = ""; // to get this variable ready to accept the next symbol
        }

        // A newline indicates the current rule has ended and the next iteration will process a new rule, hence increment rule count
        // Also store the symbol extracted till now
        else if(c == '\n') {
            Symbol* s = intialiseSymbol(symbol);
            addToSymbolList(sl,s);
            Rule* r = initialiseRule(sl,ruleCount);
            g->GRAMMAR_RULES[ruleCount] = r;
            ruleCount++;
            symbolsRead=0;
            symbol = "";
        }
        else {
            if(symbolsRead == 0){
                // Create a fresh Symbol List.
                sl = initialiseSymbolList();
            }

            // Append character to the symbol string
            symbol = appendToSymbol(symbol,c);
        }


    }

    // Capturing the corner case of the last Non terminal record => Note this requires the GRAMMAR_FILE to terminate with a '\n'
    ntrr[currentNonTerminal->TYPE.NON_TERMINAL]->end = ruleCount-1;

    return g;
}





// Function which parses the input from the testCaseFile
ParseTree* parseInputSourceCode(char *testcaseFile, ParsingTable* pTable, FirstAndFollow* fafl) {

    int f = open(testcaseFile,O_RDONLY);

    initializeLexer(f);
    ParseTree* pt = initialiseParseTree();
    Stack* st = initialiseStack(pt);

    syntaxErrorFlag = *((int*)malloc(sizeof(int)));
    lexicalErrorFlag = *((int*)malloc(sizeof(int)));

    syntaxErrorFlag = 0;
    lexicalErrorFlag = 0;
    Token* missedToken = NULL;
    Token* inputToken = getToken();
    // Keep continuinng till the lexer return NULL, which means that the input is exhausted
    while(1) {

        // Break if the input has exhausted
        if(inputToken == NULL)
            break;

        // If token is a comment continue process
        if(inputToken->TOKEN_NAME == TK_COMMENT) {
            inputToken = getToken();
            continue;
        }

        if(inputToken->TOKEN_NAME == TK_ERR) {
            lexicalErrorFlag = 1;
        }

        NaryTreeNode* stackTop = top(st);

        // printf(" Input token is %s and lexeme is %s \n" , getTerminal(inputToken->TOKEN_NAME), inputToken->LEXEME);
        // if(stackTop->IS_LEAF_NODE == 1)
        //     printf("Stack top is %s \n" ,getTerminal(stackTop->NODE_TYPE.L.ENUM_ID));
        // else
        //     printf("Stack top is %s \n" , getNonTerminal(stackTop->NODE_TYPE.NL.ENUM_ID));

        // Case when the top of the stack has a terminal
        if(stackTop->IS_LEAF_NODE == 1) {

            // If the token ID of the input and the stack top match
            if(inputToken->TOKEN_NAME == stackTop->NODE_TYPE.L.ENUM_ID) {

                // Populate the parse tree field
                stackTop->NODE_TYPE.L.TK = (Token*)malloc(sizeof(Token));
                stackTop->NODE_TYPE.L.TK->LEXEME = copyLexeme(inputToken->LEXEME);
                stackTop->NODE_TYPE.L.TK->LINE_NO = inputToken->LINE_NO;
                stackTop->NODE_TYPE.L.TK->TOKEN_NAME = inputToken->TOKEN_NAME;
                stackTop->NODE_TYPE.L.TK->IS_NUMBER = inputToken->IS_NUMBER;
                stackTop->NODE_TYPE.L.TK->VALUE = inputToken->VALUE;

                pop(st);
                inputToken = getToken();
                continue;
            }
            else {

                // Throw Error
                syntaxErrorFlag = 1;
                // Terminal-Terminal clash => Just assume that the terminal is the one you wanted

                // No need to print if the token is TK_ERR, lexer handles the printing in this case
                if(inputToken->TOKEN_NAME != TK_ERR)
                    printf("Line %d : The token %s for the lexeme %s does not match with the expected token %s\n" ,inputToken->LINE_NO,getTerminal(inputToken->TOKEN_NAME),inputToken->LEXEME,getTerminal(stackTop->NODE_TYPE.L.ENUM_ID));




                // If the input token is a token error in which the token was identified but did not respect the constraints,
                // then consider that the error token was the one which was expected by the stack top
                if(inputToken->TOKEN_NAME == TK_ERR) {

                    stackTop->NODE_TYPE.L.TK = (Token*)malloc(sizeof(Token));
                    stackTop->NODE_TYPE.L.TK->LEXEME = inputToken->LEXEME;
                    stackTop->NODE_TYPE.L.TK->LINE_NO = inputToken->LINE_NO;
                    stackTop->NODE_TYPE.L.TK->TOKEN_NAME = stackTop->NODE_TYPE.L.ENUM_ID;
                    stackTop->NODE_TYPE.L.TK->IS_NUMBER = 0;
                    stackTop->NODE_TYPE.L.TK->VALUE = NULL;
                    inputToken = getToken();
                    pop(st);
                }
                // Othwerwise assume that the token was missed
                else {
                    stackTop->NODE_TYPE.L.TK = (Token*)malloc(sizeof(Token));
                    stackTop->NODE_TYPE.L.TK->LEXEME = "ERROR_MISSED_LEXEME";
                    stackTop->NODE_TYPE.L.TK->LINE_NO = inputToken->LINE_NO;
                    stackTop->NODE_TYPE.L.TK->TOKEN_NAME = stackTop->NODE_TYPE.L.ENUM_ID;
                    stackTop->NODE_TYPE.L.TK->IS_NUMBER = 0;
                    stackTop->NODE_TYPE.L.TK->VALUE = NULL;
                    missedToken = inputToken;
                    pop(st);
                }


                continue;

            }
        }
        else {

            int ruleNumber = pTable->entries[stackTop->NODE_TYPE.NL.ENUM_ID][inputToken->TOKEN_NAME];

            if(ruleNumber != 0) {

                // printf("Parse table says consult Rule %d \n" ,ruleNumber);
                Rule* r = g->GRAMMAR_RULES[ruleNumber];
                addRuleToParseTree(stackTop,r);

                // Pop the stackTop5
                pop(st);

                // Push children of the rules on the stack
                NaryTreeNode* childNode = stackTop->NODE_TYPE.NL.child;

                // IMPORTANT => DO NOT PUSH EPS ON STACK
                if(childNode->IS_LEAF_NODE == 1 && childNode->NODE_TYPE.L.ENUM_ID == TK_EPS);
                else
                    pushTreeChildren(st,childNode);

            }
            else {
                // Throw error
                syntaxErrorFlag = 1;
                // Keep iterating the input until the input symbol is in the follow set of the non terminal on the top of the stack

                // If input token is TK_ERR, skip and get the next token
                if(inputToken->TOKEN_NAME == TK_ERR) {
                    // printf("Token causing parsing issue is a lexical error , move input ahead! \n");
                    inputToken = getToken();
                    continue;
                }

                // If epsilon lies in the FIRST of the current non terminal, assume the epsilon producing rule of the stack is being used as the default rule
                if(fafl->FIRST[stackTop->NODE_TYPE.NL.ENUM_ID][TK_EPS] == 1) {
                    // printf("Epsilon is present in first of this non terminal, assume that epsilon was generated\n");
                    pop(st);
                    continue;
                }

                // Because the error for the missed token has already been reported once in the terminal-terminal error case, no need to print again
                if(inputToken != missedToken)
                    printf("Line %d : The token %s for the lexeme %s does not match with the Non Terminal %s\n" ,inputToken->LINE_NO,getTerminal(inputToken->TOKEN_NAME),inputToken->LEXEME,getNonTerminal(stackTop->NODE_TYPE.NL.ENUM_ID));

                // Use the follow set of the stackTop to synchronize
                while(inputToken != NULL && fafl->FOLLOW[stackTop->NODE_TYPE.NL.ENUM_ID][inputToken->TOKEN_NAME] == 0) {
                    // printf("Ignoring Token %s\n" , getTerminal(inputToken->TOKEN_NAME));
                    inputToken = getToken();
                }

                // If the input gets depleted then break
                if(inputToken == NULL)
                    break;

                // An input Token is found which is in the followSet, so pop it.
                else {
                    pop(st);
                    continue;
                }

            }


        }

        // printf("-----\n");
        // printf("\n");
    }

    NaryTreeNode* stackTop = top(st);
    if(lexicalErrorFlag == 0 && syntaxErrorFlag == 0 && stackTop->IS_LEAF_NODE == 1 && stackTop->NODE_TYPE.L.ENUM_ID == TK_DOLLAR) {
        printf("\n \nSuccessfully Parsed the whole Input\n");
    }
    else {
        printf("\n \nParsing unsuccesful\n");
    }

    close(f);

    return pt;
}

void printParseTreeHelper(NaryTreeNode * pt, FILE* f) {

    if(pt == NULL)
        return;

    if(pt->IS_LEAF_NODE == 1) {
        int tokenEnumID = pt->NODE_TYPE.L.ENUM_ID;
        char lexeme[30];
        for(int i=0; i < 29; i++)
            lexeme[i] = ' ';
        lexeme[29] = '\0';

        if(tokenEnumID != TK_EPS) {
            for(int i=0; i < strlen(pt->NODE_TYPE.L.TK->LEXEME); i++)
                lexeme[i] = pt->NODE_TYPE.L.TK->LEXEME[i];
        }
        else {
            char* str = "EPSILON";
            for(int i=0; i < strlen(str); i++)
                lexeme[i] = str[i];
        }

        int lineNumber;
        int isNumber;
        int valueIfInt;
        float valueIfFloat;
        if(tokenEnumID != TK_EPS) {
            lineNumber = pt->NODE_TYPE.L.TK->LINE_NO;
            isNumber = pt->NODE_TYPE.L.TK->IS_NUMBER;
            if(isNumber == 1)
                valueIfInt = pt->NODE_TYPE.L.TK->VALUE->INT_VALUE;
            else if(isNumber == 2)
                valueIfFloat = pt->NODE_TYPE.L.TK->VALUE->FLOAT_VALUE;
        }
        else {
            lineNumber = -1;
        }

        char tokenName[20];
        for(int i=0; i < 19; i++)
            tokenName[i] = ' ';
        tokenName[19] = '\0';

        char* obtainedTokenName = getTerminal(pt->NODE_TYPE.L.ENUM_ID);

        for(int i=0; i < strlen(obtainedTokenName); i++) {
            tokenName[i] = obtainedTokenName[i];
        }

        char parent[30];
        for(int i=0; i < 29; i++)
            parent[i] = ' ';

        parent[29] = '\0';
        char* obtainedParent = getNonTerminal(pt->parent->NODE_TYPE.NL.ENUM_ID);
        for(int i=0; i < strlen(obtainedParent); i++)
            parent[i] = obtainedParent[i];

        char* isLeafNode = "yes";
        char* currentSymbol = "----";
        // char* spaceString = "";

        if(tokenEnumID == TK_EPS || isNumber == 0)
            fprintf(f,"%s      %d    %s %s %s %s %s\n" ,lexeme,lineNumber,tokenName,"----   ",parent,isLeafNode,currentSymbol);
        else if(isNumber == 1)
            fprintf(f,"%s      %d    %s %d          %s %s %s\n" ,lexeme,lineNumber,tokenName,valueIfInt,parent,isLeafNode,currentSymbol);
        else
            fprintf(f,"%s      %d    %s %f    %s %s %s\n" ,lexeme,lineNumber,tokenName,valueIfFloat,parent,isLeafNode,currentSymbol);

    }
    else {
        NaryTreeNode* trav = pt->NODE_TYPE.NL.child;

        if(trav!=NULL) {
            printParseTreeHelper(pt->NODE_TYPE.NL.child, f);
            trav = trav->next;
        }

        char lexeme[30];
        for(int i=0; i < 29; i++)
            lexeme[i] = ' ';
        lexeme[29] = '\0';
        lexeme[0] = '-'; lexeme[1] = '-'; lexeme[2] = '-'; lexeme[3] = '-';

        int lineNumber = -1;
        char* tokenName = "-----------        ";
        char* valueIfNumber = "----   ";
        char* currentSymbol = getNonTerminal(pt->NODE_TYPE.NL.ENUM_ID);
        char parent[30];
        for(int i=0; i < 29; i++)
            parent[i] = ' ';

        parent[29] = '\0';

        char* obtainedParent;
        if(pt->parent != NULL)
            obtainedParent = getNonTerminal(pt->parent->NODE_TYPE.NL.ENUM_ID);
        else
            obtainedParent = "NULL";

        for(int i=0; i < strlen(obtainedParent); i++)
            parent[i] = obtainedParent[i];

        char* isLeafNode = "no";

        fprintf(f,"%s      %d    %s %s %s %s %s\n" ,lexeme,lineNumber,tokenName,valueIfNumber,parent,isLeafNode,currentSymbol);


        while(trav!=NULL){
            printParseTreeHelper(trav,f);
            trav = trav->next;
        }

    }
}

void printParseTree(ParseTree* pt, char* outfile) {

    FILE* f;

    // Print on console if no outfile is provided
    if(outfile == NULL)
        f = stdout;
    else
        f = fopen(outfile,"wb");

    if(f == NULL) {
        printf("Error opening the outfile\n");
        return;
    }

    printParseTreeHelper(pt->root,f);

    // Do not close stdout
    if(f != stdout)
        fclose(f);

}

//Utility function to print all symbols in the list
void printSymbolList(SymbolList* ls) {
    Symbol* trav = ls->HEAD_SYMBOL;
    while(trav != NULL) {
        if(trav->IS_TERMINAL == 1)
            printf("%s " ,TerminalID[trav->TYPE.TERMINAL]);
        else
            printf("%s " ,NonTerminalID[trav->TYPE.NON_TERMINAL]);

        trav = trav->next;
    }
}

//Utility function to print a rule
void printRule(Rule* r) {

    if(r == NULL) {
        printf("-------------------------\n");
        return;
    }

    SymbolList* ls = r->SYMBOLS;
    printSymbolList(ls);

    printf("\n");
}


// Utility function to print the grammar structure
void printGrammarStructure() {
    for(int i=0; i < g->GRAMMAR_RULES_SIZE; i++) {
        Rule* r = g->GRAMMAR_RULES[i];
        printRule(r);
    }
}

// Utility function to print the NonTerminalRuleRecords
void printNonTerminalRuleRecords() {
    for(int i=0; i < TOTAL_GRAMMAR_NONTERMINALS; i++) {
        NonTerminalRuleRecords* temp = ntrr[i];
        printf("Rules for Non terminal %s start from %d and end at %d\n" ,NonTerminalID[i],temp->start,temp->end);
    }
}

// Utility function to print the first set
void printFirstSets(FirstAndFollow* fafl) {
    int** firstVector = fafl->FIRST;
    for(int i=0; i < TOTAL_GRAMMAR_NONTERMINALS; i++) {
        printf("First set for non terminals %s are ====> " ,NonTerminalID[i]);

        for(int j=0; j < vectorSize; j++) {
            if(firstVector[i][j] == 1)
                printf("%s " , TerminalID[j]);
        }

        printf("\n");
    }
}

// Utility function to print follow sets
void printFollowSets(FirstAndFollow* fafl) {
    int** followVector = fafl->FOLLOW;
    for(int i=0; i < TOTAL_GRAMMAR_NONTERMINALS; i++) {
        printf("Follow set for non terminals %s are ====> " ,NonTerminalID[i]);

        for(int j=0; j < vectorSize; j++) {
            if(followVector[i][j] == 1)
                printf("%s " , TerminalID[j]);
        }

        printf("\n");
    }
}

// Utility function to print parsing table
void printParseTable(ParsingTable* pt) {
    for(int i=0; i < TOTAL_GRAMMAR_NONTERMINALS; i++) {
        printf("%s\n" ,NonTerminalID[i]);
        for(int j=0; j < TOTAL_GRAMMAR_TERMINALS; j++) {
            // For pretty printing
            for(int k=0; k < 10; k++)
                printf(" ");

            printf("%s ==> " ,TerminalID[j]);
            if(pt->entries[i][j] != 0) {
                printRule(g->GRAMMAR_RULES[pt->entries[i][j]]);
            }
            else
                printf("NULL\n");
        }
    }
}

int getErrorStatus() {
    return (lexicalErrorFlag || syntaxErrorFlag);
}
