#include "nary.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct QN { NNode* v; struct QN* next; } QN;
typedef struct { QN* head; QN* tail; } Q;
static void qi(Q* q){ q->head=q->tail=0; }
static int  qe(Q* q, NNode* n){ QN* x=(QN*)malloc(sizeof* x); if(!x) return 0; x->v=n; x->next=0; if(!q->tail) q->head=q->tail=x; else { q->tail->next=x; q->tail=x; } return 1; }
static int  qd(Q* q, NNode** out){ QN* h=q->head; if(!h) return 0; if(out) *out=h->v; q->head=h->next; if(!q->head) q->tail=0; free(h); return 1; }

NNode* n_create(int id){
    NNode* n=(NNode*)malloc(sizeof* n); if(!n) return 0;
    n->id=id; n->items_count=0; n->child=0; n->child_count=0; n->child_cap=0; return n;
}
int n_attach(NNode* parent, NNode* child){
    if(!parent||!child) return 0;
    if(parent->child_count==parent->child_cap){
        int nc = parent->child_cap? parent->child_cap*2 : 4;
        NNode** nb = (NNode**)realloc(parent->child, sizeof(NNode*)*nc);
        if(!nb) return 0;
        parent->child = nb; parent->child_cap = nc;
    }
    parent->child[parent->child_count++] = child;
    return 1;
}
void n_bfs_print(NNode* root){
    if(!root){ printf("(empty n-ary)\n"); return; }
    Q q; qi(&q); qe(&q, root);
    while(q.head){
        NNode* cur; qd(&q,&cur);
        printf("Node %d (items=%d) -> children: ", cur->id, cur->items_count);
        for(int i=0;i<cur->child_count;i++){ printf("%d ", cur->child[i]->id); qe(&q, cur->child[i]); }
        printf("\n");
    }
}
static void n_clear_rec(NNode* r){
    if(!r) return;
    for(int i=0;i<r->child_count;i++) n_clear_rec(r->child[i]);
    free(r->child);
    free(r);
}
void n_clear(NNode* root){ n_clear_rec(root); }
