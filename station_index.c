#include "station_index.h"
#include <stdlib.h>
#include <stdio.h>

static int h(StationNode* n){ return n? n->height : -1; }
static void upd(StationNode* n){ int hl=h(n->left), hr=h(n->right); n->height=(hl>hr?hl:hr)+1; }
static StationNode* mk(int id, StationInfo in){
    StationNode* n=(StationNode*)malloc(sizeof*n); if(!n) return 0;
    n->station_id=id; n->info=in; n->left=n->right=0; n->height=0; return n;
}
static StationNode* rotR(StationNode* y){ StationNode* x=y->left; StationNode* t2=x->right; x->right=y; y->left=t2; upd(y); upd(x); return x; }
static StationNode* rotL(StationNode* x){ StationNode* y=x->right; StationNode* t2=y->left; y->left=x; x->right=t2; upd(x); upd(y); return y; }

static StationNode* rebalance(StationNode* n){
    upd(n);
    int bf=h(n->left)-h(n->right);
    if(bf>1){ if(h(n->left->right)>h(n->left->left)) n->left=rotL(n->left); return rotR(n); }
    if(bf<-1){ if(h(n->right->left)>h(n->right->right)) n->right=rotR(n->right); return rotL(n); }
    return n;
}

void si_init(StationIndex* idx){ idx->root=0; }
StationNode* si_find(StationNode* r, int id){
    while(r){ if(id<r->station_id) r=r->left; else if(id>r->station_id) r=r->right; else return r; } return 0;
}

static StationNode* insert_rec(StationNode* r, int id, StationInfo in){
    if(!r) return mk(id,in);
    if(id<r->station_id) r->left=insert_rec(r->left,id,in);
    else if(id>r->station_id) r->right=insert_rec(r->right,id,in);
    else { r->info=in; return r; } /* update */
    return rebalance(r);
}
void si_add(StationIndex* idx, int id, StationInfo in){ idx->root=insert_rec(idx->root,id,in); }

static StationNode* min_node(StationNode* r){ StationNode* c=r; while(c&&c->left) c=c->left; return c; }
static StationNode* delete_rec(StationNode* r, int id, int* found){
    if(!r) return 0;
    if(id<r->station_id) r->left=delete_rec(r->left,id,found);
    else if(id>r->station_id) r->right=delete_rec(r->right,id,found);
    else{
        *found=1;
        if(!r->left && !r->right){ free(r); return 0; }
        else if(!r->left){ StationNode* t=r->right; free(r); return t; }
        else if(!r->right){ StationNode* t=r->left;  free(r); return t; }
        else { StationNode* s=min_node(r->right); r->station_id=s->station_id; r->info=s->info; r->right=delete_rec(r->right,s->station_id,found); }
    }
    return rebalance(r);
}
int si_delete(StationIndex* idx, int id){ int f=0; idx->root=delete_rec(idx->root,id,&f); return f; }

int si_to_array(StationNode* r, int* ids, int cap){
    if(!r || cap<=0) return 0;
    int w=0, wl=si_to_array(r->left, ids, cap);
    w+=wl; if(w<cap){ ids[w++]=r->station_id; }
    if(w<cap){ int wr=si_to_array(r->right, ids+w, cap-w); w+=wr; }
    return w;
}
static void free_post(StationNode* r){ if(!r) return; free_post(r->left); free_post(r->right); free(r); }
void si_clear(StationIndex* idx){ free_post(idx->root); idx->root=0; }

void si_print_sideways(StationNode* r){
    static int depth=0; if(!r) return;
    depth++; si_print_sideways(r->right);
    for(int i=1;i<depth;i++) printf("    "); printf("%d(h=%d) P=%dKW Price=%dc Slots=%d\n", r->station_id, r->height, r->info.power_kW, r->info.price_cents, r->info.slots_free);
    si_print_sideways(r->left); depth--;
}
