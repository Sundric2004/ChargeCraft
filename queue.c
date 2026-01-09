#include "queue.h"
#include <stdlib.h>

void q_init(Queue* q){ q->head=q->tail=0; }
int  q_is_empty(Queue* q){ return q->head==0; }
int  q_enqueue(Queue* q, Event e){
    QNode* n=(QNode*)malloc(sizeof*n); if(!n) return 0;
    n->e=e; n->next=0;
    if(!q->tail) q->head=q->tail=n; else { q->tail->next=n; q->tail=n; }
    return 1;
}
int  q_dequeue(Queue* q, Event* out){
    QNode* h=q->head; if(!h) return 0; if(out) *out=h->e;
    q->head=h->next; if(!q->head) q->tail=0; free(h); return 1;
}
void q_clear(Queue* q){ Event dump; while(q_dequeue(q,&dump)){} }
