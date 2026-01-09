#ifndef DS_NARY_H
#define DS_NARY_H
typedef struct NNode {
    int id;
    int items_count;
    struct NNode** child;
    int child_count;
    int child_cap;
} NNode;

NNode* n_create(int id);
int    n_attach(NNode* parent, NNode* child);
void   n_bfs_print(NNode* root);
void   n_clear(NNode* root);

#endif
