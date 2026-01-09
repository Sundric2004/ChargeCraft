#ifndef DS_STACK_H
#define DS_STACK_H
typedef struct StNode{ int v; struct StNode* next; } StNode;
typedef struct Stack{ StNode* top; } Stack;

void st_init(Stack* s);            /* O(1) */
int  st_push(Stack* s, int v);     /* O(1) */
int  st_pop(Stack* s, int* out);   /* O(1) */
int  st_is_empty(Stack* s);        /* O(1) */
void st_clear(Stack* s);           /* O(n) */

#endif
