/* Group 27
Venkat Nalla Siddartha Reddy                2016A7PS0030P
Arnav Sailesh                               2016A7PS0054P
Gunraj Singh                                2016A7PS0085P
Aashish Singh                               2016A7PS0683P */

#include "astDef.h"

typedef struct node {
  ASTNode* v;
  char* parent;
  int depth;
  struct node* next;
}node;

typedef struct Queue {
  node* head;
  node* tail;
  int size;
}Queue;
