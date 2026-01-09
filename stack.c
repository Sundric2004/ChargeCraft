#include "stack.h"
#include <stdlib.h>
void st_init(Stack* s){ s->top=0; }
int  st_push(Stack* s, int v){
    StNode* n=(StNode*)malloc(sizeof*n); if(!n) return 0;
    n->v=v; n->next=s->top; s->top=n; return 1;
}
int  st_pop(Stack* s, int* out){
    StNode* t=s->top; if(!t) return 0; if(out) *out=t->v; s->top=t->next; free(t); return 1;
}
int  st_is_empty(Stack* s){ return s->top==0; }
void st_clear(Stack* s){ int tmp; while(st_pop(s,&tmp)){} }
