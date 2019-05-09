/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "interface.h"
#include "nary_treeDef.h"
#include "parser.h"

int parseTreeNodeCount;
int parseTreeMemory;

NaryTreeNode* createLeafNode(int enumId) {
    NaryTreeNode* ntn = (NaryTreeNode*)malloc(sizeof(NaryTreeNode));
    ntn->IS_LEAF_NODE = 1;
    ntn->NODE_TYPE.L.ENUM_ID = enumId;
    ntn->next = NULL;
    parseTreeMemory += sizeof(NaryTreeNode);
    return ntn;
}

NaryTreeNode* createNonLeafNode(int enumId) {
    NaryTreeNode* ntn = (NaryTreeNode*)malloc(sizeof(NaryTreeNode));
    ntn->IS_LEAF_NODE = 0;
    ntn->NODE_TYPE.NL.ENUM_ID = enumId;
    ntn->NODE_TYPE.NL.NUMBER_CHILDREN = 0;
    ntn->next = NULL;
    ntn->NODE_TYPE.NL.child = NULL;
    parseTreeMemory += sizeof(NaryTreeNode);
    return ntn;
}

NaryTreeNode* createNode(int isTerminal, SymbolType type,NaryTreeNode* parent) {

    NaryTreeNode* ntn;
    if(isTerminal == 1) {
        ntn = createLeafNode(type.TERMINAL);
        ntn->parent = parent;
    }
    else {
        ntn = createNonLeafNode(type.NON_TERMINAL);
        ntn->parent = parent;
    }

    parseTreeNodeCount += 1;
    return ntn;
}

ParseTree* initialiseParseTree() {
    parseTreeNodeCount = *((int*)malloc(sizeof(int)));
    parseTreeMemory = *((int*)malloc(sizeof(int)));
    ParseTree* pt = (ParseTree*)malloc(sizeof(ParseTree));
    pt->root = createNonLeafNode(program); // Initialising the tree with the root node
    pt->root->parent = NULL;
    return pt;
}

void addRuleToParseTree(NaryTreeNode* ntn, Rule* r) {

    // For debugging if such a situation happens
    if(ntn->IS_LEAF_NODE == 1) {
        printf("TERMINALS CANNOT HAVE CHILDREN! \n");
        return;
    }

    // Start from RHS of the RULE
    int numberChild = 0;
    Symbol* trav = r->SYMBOLS->HEAD_SYMBOL->next;
    NaryTreeNode* childHead = NULL;
    NaryTreeNode* childTrav = NULL;
    while(trav != NULL) {
        if(childHead == NULL) {
            childHead = createNode(trav->IS_TERMINAL,trav->TYPE,ntn);
            childTrav = childHead;
        }
        else {
            childTrav->next = createNode(trav->IS_TERMINAL,trav->TYPE,ntn);
            childTrav = childTrav->next;
        }
        numberChild++;
        trav = trav->next;
    }

    ntn->NODE_TYPE.NL.RULE_NO = r->RULE_NO;
    ntn->NODE_TYPE.NL.child = childHead;
    ntn->NODE_TYPE.NL.NUMBER_CHILDREN = numberChild;
}

void printNaryTree(NaryTreeNode* nt) {
    if(nt->IS_LEAF_NODE == 1) {
        printf("%s " ,getTerminal(nt->NODE_TYPE.L.ENUM_ID));
        return;
    }

    printf("%s\n" , getNonTerminal(nt->NODE_TYPE.NL.ENUM_ID));

    NaryTreeNode* childTrav = nt->NODE_TYPE.NL.child;
    while(childTrav != NULL) {

        if(childTrav->IS_LEAF_NODE == 1)
            printf("%s " ,getTerminal(childTrav->NODE_TYPE.L.ENUM_ID));
        else
            printf("%s " ,getNonTerminal(childTrav->NODE_TYPE.NL.ENUM_ID));

        childTrav = childTrav->next;
    }

    printf("\n");

    childTrav = nt->NODE_TYPE.NL.child;
    while(childTrav != NULL) {
        if(childTrav->IS_LEAF_NODE == 0)
            printNaryTree(childTrav);
        childTrav = childTrav->next;
    }
}

void printTree(ParseTree* pt) {
    NaryTreeNode* nt = pt->root;
    printNaryTree(nt);
}

int getParseTreeNodeCount() {
    return parseTreeNodeCount;
}

int getParseTreeMemory() {
    return parseTreeMemory;
}
