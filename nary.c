#include "nary.h"
#include <stdlib.h>
#include <stdio.h>

/* Structure de nœud pour la queue BFS */
typedef struct QN { NNode* v; struct QN* next; } QN;

/* Structure de queue pour le parcours BFS */
typedef struct { QN* head; QN* tail; } Q;

/*
 * Fonction auxiliaire : qi (queue init)
 * Description : Initialise une queue vide
 * Complexité temps : O(1)
 * Complexité espace : O(1)
 */
static void qi(Q* q){ q->head=q->tail=0; }

/*
 * Fonction auxiliaire : qe (queue enqueue)
 * Description : Ajoute un nœud à la fin de la queue
 * Paramètres :
 *   - q : queue
 *   - n : nœud à ajouter
 * Retour : 1 si succès, 0 si échec d'allocation
 * Complexité temps : O(1)
 * Complexité espace : O(1) - Un seul nœud de queue alloué
 */
static int qe(Q* q, NNode* n){
    QN* x=(QN*)malloc(sizeof* x);
    if(!x) return 0;
    x->v=n; x->next=0;
    if(!q->tail) q->head=q->tail=x;
    else { q->tail->next=x; q->tail=x; }
    return 1;
}

/*
 * Fonction auxiliaire : qd (queue dequeue)
 * Description : Retire et retourne le premier nœud de la queue
 * Paramètres :
 *   - q : queue
 *   - out : pointeur vers où stocker le nœud retiré
 * Retour : 1 si succès, 0 si queue vide
 * Complexité temps : O(1)
 * Complexité espace : O(1)
 */
static int qd(Q* q, NNode** out){
    QN* h=q->head;
    if(!h) return 0;
    if(out) *out=h->v;
    q->head=h->next;
    if(!q->head) q->tail=0;
    free(h);
    return 1;
}

/*
 * Fonction : n_create
 * Description : Crée un nouveau nœud de l'arbre n-aire
 * Paramètre :
 *   - id : identifiant du nœud (ex: code pays, région, ville)
 * Retour : Pointeur vers le nouveau nœud, ou NULL si échec d'allocation
 * Complexité temps : O(1)
 * Complexité espace : O(1) - Un seul nœud alloué
 */
NNode* n_create(int id){
    NNode* n=(NNode*)malloc(sizeof* n);
    if(!n) return 0;
    n->id=id;
    n->items_count=0;
    n->child=0;
    n->child_count=0;
    n->child_cap=0;
    return n;
}

/*
 * Fonction : n_attach
 * Description : Attache un nœud enfant à un nœud parent dans l'arbre n-aire
 *               Gère le redimensionnement dynamique du tableau d'enfants
 * Paramètres :
 *   - parent : nœud parent
 *   - child : nœud enfant à attacher
 * Retour : 1 si succès, 0 si échec
 * Complexité temps : O(1) amorti - Doublement de capacité si nécessaire
 * Complexité espace : O(k) où k = nombre d'enfants (pour le tableau dynamique)
 */
int n_attach(NNode* parent, NNode* child){
    if(!parent||!child) return 0;
    // Si le tableau d'enfants est plein, on double sa capacité
    if(parent->child_count==parent->child_cap){
        int nc = parent->child_cap? parent->child_cap*2 : 4;
        NNode** nb = (NNode**)realloc(parent->child, sizeof(NNode*)*nc);
        if(!nb) return 0;
        parent->child = nb;
        parent->child_cap = nc;
    }
    parent->child[parent->child_count++] = child;
    return 1;
}

/*
 * Fonction : n_bfs_print
 * Description : Affiche l'arbre n-aire en parcours BFS (niveau par niveau)
 *               Utile pour visualiser la hiérarchie géographique
 * Paramètre :
 *   - root : racine de l'arbre
 * Complexité temps : O(n) où n = nombre total de nœuds
 *                    Chaque nœud est visité une fois
 * Complexité espace : O(w) où w = largeur maximale de l'arbre
 *                     (nombre maximum de nœuds à un même niveau)
 */
void n_bfs_print(NNode* root){
    if(!root){ printf("(empty n-ary)\n"); return; }
    Q q; qi(&q); qe(&q, root);
    while(q.head){
        NNode* cur; qd(&q,&cur);
        printf("Node %d (items=%d) -> children: ", cur->id, cur->items_count);
        for(int i=0;i<cur->child_count;i++){
            printf("%d ", cur->child[i]->id);
            qe(&q, cur->child[i]);
        }
        printf("\n");
    }
}

/*
 * Fonction auxiliaire récursive : n_clear_rec
 * Description : Libère récursivement tous les nœuds d'un sous-arbre (post-order)
 * Paramètre :
 *   - r : racine du sous-arbre à libérer
 * Complexité temps : O(n) - Visite chaque nœud pour le libérer
 * Complexité espace : O(h) où h = hauteur de l'arbre (pile de récursion)
 */
static void n_clear_rec(NNode* r){
    if(!r) return;
    // Libérer d'abord tous les enfants
    for(int i=0;i<r->child_count;i++)
        n_clear_rec(r->child[i]);
    // Puis libérer le tableau d'enfants et le nœud lui-même
    free(r->child);
    free(r);
}

/*
 * Fonction : n_clear
 * Description : Libère toute la mémoire de l'arbre n-aire
 * Paramètre :
 *   - root : racine de l'arbre
 * Complexité temps : O(n) - Libération de tous les nœuds
 * Complexité espace : O(h) - Pile de récursion
 */
void n_clear(NNode* root){
    n_clear_rec(root);
}

/*
 * Fonction : n_aggregate
 * Description : Calcule la somme récursive des items_count dans un sous-arbre
 *               Utilisé pour agréger les statistiques géographiques
 *               (ex: total de slots libres dans une région)
 * Paramètre :
 *   - root : racine du sous-arbre
 * Retour : Somme totale des items_count du nœud et de tous ses descendants
 * Exemple : Si Paris contient Eiffel(10 slots) et Louvre(5 slots),
 *           n_aggregate(Paris) retournera 15
 * Complexité temps : O(n) où n = nombre de nœuds dans le sous-arbre
 *                    Chaque nœud est visité exactement une fois
 * Complexité espace : O(h) où h = hauteur du sous-arbre
 *                     Pile de récursion proportionnelle à la profondeur
 */
int n_aggregate(NNode* root){
    if(!root) return 0;
    // Commencer avec la valeur du nœud courant
    int total = root->items_count;
    // Ajouter récursivement les valeurs de tous les enfants
    for(int i=0; i < root->child_count; i++){
        total += n_aggregate(root->child[i]);
    }
    return total;
}