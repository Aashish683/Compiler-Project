/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "interface.h"
#include "keyword_table.h"
#include <string.h>
#define NUMBER_KEYWORDS 24

// Commenting this line as the keyword table is declared as global in the header file
// Uncommenting it due to compile time errors

int hashFunction(char* str) {

    /*Hash function string sum and mod */
    // int sum = 0;
    // for(int i=0; i < strlen(str); i++) {
    //     sum += str[i]-'0';
    // }
    // return (sum%NUMBER_KEYWORDS);

    /* Hash function djb2 and mod */
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return (hash%NUMBER_KEYWORDS);
}

void addEntry(KeywordTable* kt,TokenName tn, char* lexeme) {
    int hash =  hashFunction(lexeme);
    // printf("Hash is %d for the keyword %s\n" , hash, lexeme);
    kt->KEYWORDS[hash].keywords = addToList(kt->KEYWORDS[hash].keywords,tn,lexeme);
}

Node* lookUp(KeywordTable* kt,char* lexeme) {
    int hash = hashFunction(lexeme);
    Node* trav = kt->KEYWORDS[hash].keywords;
    while(trav != NULL) {
        if(strcmp(lexeme,trav->LEXEME) == 0)
            return trav;
        trav = trav->next;
    }

    return NULL;
}

KeywordTable* initializeTable() {
    KeywordTable* kt = (KeywordTable*)malloc(sizeof(KeywordTable));
    kt->KEYWORDS = (KeywordNode*)malloc(NUMBER_KEYWORDS*sizeof(KeywordNode));

    // Initialize each keyword slot as NULL initially
    for(int i=0; i < NUMBER_KEYWORDS; i++) {
        kt->KEYWORDS[i].keywords = NULL;
    }

    addEntry(kt,TK_WITH,"with");
    addEntry(kt,TK_PARAMETERS,"parameters");
    addEntry(kt,TK_END,"end");
    addEntry(kt,TK_WHILE,"while");
    addEntry(kt,TK_TYPE,"type");
    addEntry(kt,TK_MAIN,"_main");
    addEntry(kt,TK_GLOBAL,"global");
    addEntry(kt,TK_PARAMETER,"parameter");
    addEntry(kt,TK_LIST,"list");
    addEntry(kt,TK_INPUT,"input");
    addEntry(kt,TK_OUTPUT,"output");
    addEntry(kt,TK_INT,"int");
    addEntry(kt,TK_REAL,"real");
    addEntry(kt,TK_ENDWHILE,"endwhile");
    addEntry(kt,TK_IF,"if");
    addEntry(kt,TK_THEN,"then");
    addEntry(kt,TK_ENDIF,"endif");
    addEntry(kt,TK_READ,"read");
    addEntry(kt,TK_WRITE,"write");
    addEntry(kt,TK_RETURN,"return");
    addEntry(kt,TK_CALL,"call");
    addEntry(kt,TK_RECORD,"record");
    addEntry(kt,TK_ENDRECORD,"endrecord");
    addEntry(kt,TK_ELSE,"else");

    return kt;

}

/*** List functions ***/

// Returns the head of the modified list
Node* addToList(Node* ls, TokenName tn, char* lexeme) {

    // Case when list is empty
    if(ls == NULL) {
        Node* n  = (Node*)malloc(sizeof(Node));
        n->LEXEME = lexeme;
        n->TOKEN_NAME = tn;
        n->next = NULL;
        return n;
    }

    // Insert at front O(1)
    Node* n = (Node*)malloc(sizeof(Node));
    n->LEXEME = lexeme;
    n->TOKEN_NAME = tn;
    n->next = ls;
    return n;
}


// Function to lookup a lexeme in a list
int searchList(Node* ls, char* lexeme) {
    Node* trav = ls;
    while(trav != NULL) {
        if(strcmp(lexeme,trav->LEXEME))
            return 1;
        trav = trav->next;
    }

    return 0;
}


/** Temporary Utility functions **/

// Temporary function print a list and also returns the number of elements in a list
int printList(Node* ls) {
    Node* trav = ls;
    int len = 0;
    if(ls == NULL) {
        printf("This slot is not occupied!\n");
        printf("\n");
        return 0;
    }

    while(trav != NULL) {
        printf("Keyword: %s " ,trav->LEXEME);
        trav = trav->next;
        len++;
    }
    printf("\n");
    printf("\n");
    return len;
}

// Temporary function to print hash table
void printHashTable(KeywordTable* kt) {
    int empty = 0;
    int collisions = 0;
    for(int i=0; i < NUMBER_KEYWORDS; i++) {
        int len = printList(kt->KEYWORDS[i].keywords);
        if(len == 0)
            empty++;
        if(len > 1)
            collisions += len-1;
    }

    // Aim for as less a load factor as possible, JAVA 10 specs says 0.75

    printf("\n");
    printf("Calculating laod-factor\n");
    printf("%f\n" ,((float)empty)/NUMBER_KEYWORDS);

    // Aim for as less collision as possible

    printf("\n");
    printf("Calculating total collisions\n");
    printf("%d\n" , collisions);


}
