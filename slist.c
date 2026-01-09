#include "slist.h"
#include <stdlib.h>
#include <stdio.h>

void ds_slist_init(SList* l){ l->head = 0; }
int ds_slist_insert_head(SList* l, int v){
    SNode* n = (SNode*)malloc(sizeof *n); if(!n) return 0;
    n->value = v; n->next = l->head; l->head = n; return 1;
}
int ds_slist_remove_value(SList* l, int v){
    SNode* p=0; SNode* c=l->head;
    while(c && c->value!=v){ p=c; c=c->next; }
    if(!c) return 0;
    if(!p) l->head=c->next; else p->next=c->next; free(c); return 1;
}
int ds_slist_remove_tail(SList* l, int* out){
    if(!l->head) return 0;
    if(!l->head->next){ if(out) *out=l->head->value; free(l->head); l->head=0; return 1; }
    SNode* p=0; SNode* c=l->head;
    while(c->next){ p=c; c=c->next; }
    if(out) *out=c->value; p->next=0; free(c); return 1;
}
void ds_slist_print(SList* l){
    SNode* c=l->head; printf("[");
    while(c){ printf("%d", c->value); if(c->next) printf(" -> "); c=c->next; }
    printf("]\n");
}
void ds_slist_clear(SList* l){
    SNode* c=l->head; while(c){ SNode* n=c->next; free(c); c=n; } l->head=0;
}
