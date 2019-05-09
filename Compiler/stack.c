/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "interface.h"
#include "stackDef.h"
#include "nary_tree.h"

StackNode* createStackNode(NaryTreeNode* ntn) {
    StackNode* stn = (StackNode*)malloc(sizeof(StackNode));
    stn->TREE_NODE = ntn;
    stn->next = NULL;
    return stn;
}

NaryTreeNode* top(Stack* st) {
    if(st->HEAD == NULL)
        return NULL;
    else
        return st->HEAD->TREE_NODE;
}

void push(Stack* st,NaryTreeNode* ntn) {
    StackNode* stn = createStackNode(ntn);
    StackNode* head = st->HEAD;

    // Case when stack is empty
    if(head == NULL) {
        st->HEAD = stn;
        st->NUM_NODES++;
        return;
    }

    stn->next = head;
    st->HEAD = stn;
    st->NUM_NODES++;
    return;
}

void pop(Stack* st) {
    StackNode* head = st->HEAD;

    // Case when stack is already empty
    if(head == NULL)
        return;

    st->HEAD = st->HEAD->next;
    st->NUM_NODES--;
}

// Function recursively pushes children on the stack
void pushTreeChildren(Stack* st,NaryTreeNode* ntn) {
    if(ntn == NULL)
        return;
    pushTreeChildren(st,ntn->next);
    push(st,ntn);
}

// Initialise the stack with TK_DOLLAr as the end and program as the startting non terminal
Stack* initialiseStack(ParseTree* pt) {
    Stack* st = (Stack*)malloc(sizeof(Stack));
    st->HEAD = NULL;
    st->NUM_NODES = 0;

    SymbolType sType;
    sType.TERMINAL = TK_DOLLAR;
    NaryTreeNode* ntn = createNode(1,sType,NULL);
    push(st,ntn);
    push(st,pt->root);
    return st;
}
