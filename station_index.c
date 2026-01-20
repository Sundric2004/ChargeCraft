#include "station_index.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * Fonction auxiliaire : h (height)
 * Description : Retourne la hauteur d'un nœud (ou -1 si NULL)
 * Complexité temps : O(1)
 * Complexité espace : O(1)
 */
static int h(StationNode* n){ return n? n->height : -1; }

/*
 * Fonction auxiliaire : upd (update height)
 * Description : Met à jour la hauteur d'un nœud après modification
 * Complexité temps : O(1) - Lecture de 2 hauteurs et mise à jour
 * Complexité espace : O(1)
 */
static void upd(StationNode* n){
    int hl=h(n->left), hr=h(n->right);
    n->height=(hl>hr?hl:hr)+1;
}

/*
 * Fonction auxiliaire : mk (make node)
 * Description : Crée un nouveau nœud de station
 * Paramètres :
 *   - id : identifiant de la station
 *   - in : informations de la station (puissance, prix, slots, timestamp)
 * Complexité temps : O(1) - Allocation et initialisation
 * Complexité espace : O(1) - Un seul nœud alloué
 */
static StationNode* mk(int id, StationInfo in){
    StationNode* n=(StationNode*)malloc(sizeof*n);
    if(!n) return 0;
    n->station_id=id; n->info=in; n->left=n->right=0; n->height=0;
    return n;
}

/*
 * Fonction auxiliaire : rotR (rotation droite)
 * Description : Effectue une rotation droite pour rééquilibrer l'AVL
 *               Utilisée quand le sous-arbre gauche est trop lourd
 * Complexité temps : O(1) - Modification de 3 pointeurs
 * Complexité espace : O(1) - Pas d'allocation
 */
static StationNode* rotR(StationNode* y){
    StationNode* x=y->left;
    StationNode* t2=x->right;
    x->right=y; y->left=t2;
    upd(y); upd(x);
    return x;
}

/*
 * Fonction auxiliaire : rotL (rotation gauche)
 * Description : Effectue une rotation gauche pour rééquilibrer l'AVL
 *               Utilisée quand le sous-arbre droit est trop lourd
 * Complexité temps : O(1) - Modification de 3 pointeurs
 * Complexité espace : O(1) - Pas d'allocation
 */
static StationNode* rotL(StationNode* x){
    StationNode* y=x->right;
    StationNode* t2=y->left;
    y->left=x; x->right=t2;
    upd(x); upd(y);
    return y;
}

/*
 * Fonction : rebalance
 * Description : Rééquilibre un nœud si son facteur d'équilibre est invalide (|bf| > 1)
 *               Effectue une rotation simple ou double selon le cas
 * Paramètre :
 *   - n : nœud à rééquilibrer
 * Retour : Nouvelle racine du sous-arbre après rééquilibrage
 * Complexité temps : O(1) - Maximum 2 rotations
 * Complexité espace : O(1) - Pas d'allocation supplémentaire
 */
static StationNode* rebalance(StationNode* n){
    upd(n);
    int bf=h(n->left)-h(n->right);
    // Cas gauche-gauche ou gauche-droite
    if(bf>1){
        if(h(n->left->right)>h(n->left->left))
            n->left=rotL(n->left); // Rotation gauche-droite (double rotation)
        return rotR(n);
    }
    // Cas droite-droite ou droite-gauche
    if(bf<-1){
        if(h(n->right->left)>h(n->right->right))
            n->right=rotR(n->right); // Rotation droite-gauche (double rotation)
        return rotL(n);
    }
    return n;
}

/*
 * Fonction : si_init
 * Description : Initialise un index de stations vide
 * Paramètre :
 *   - idx : pointeur vers la structure StationIndex
 * Complexité temps : O(1)
 * Complexité espace : O(1)
 */
void si_init(StationIndex* idx){ idx->root=0; }

/*
 * Fonction : si_find
 * Description : Recherche une station par son ID dans l'AVL
 * Paramètres :
 *   - r : racine de l'arbre (ou sous-arbre)
 *   - id : identifiant de la station recherchée
 * Retour : Pointeur vers le nœud trouvé, ou NULL si absent
 * Complexité temps : O(log n) - L'arbre AVL est équilibré, hauteur = O(log n)
 *                    On descend niveau par niveau jusqu'à trouver ou atteindre NULL
 * Complexité espace : O(1) - Pas de récursion, parcours itératif
 */
StationNode* si_find(StationNode* r, int id){
    while(r){
        if(id<r->station_id) r=r->left;
        else if(id>r->station_id) r=r->right;
        else return r;
    }
    return 0;
}

/*
 * Fonction auxiliaire récursive : insert_rec
 * Description : Insère ou met à jour une station dans l'AVL de manière récursive
 * Paramètres :
 *   - r : racine du sous-arbre courant
 *   - id : identifiant de la station
 *   - in : informations de la station
 * Retour : Nouvelle racine du sous-arbre après insertion et rééquilibrage
 * Complexité temps : O(log n) - Descente récursive + rééquilibrage O(1) par niveau
 * Complexité espace : O(log n) - Pile de récursion de profondeur log(n)
 */
static StationNode* insert_rec(StationNode* r, int id, StationInfo in){
    if(!r) return mk(id,in);
    if(id<r->station_id) r->left=insert_rec(r->left,id,in);
    else if(id>r->station_id) r->right=insert_rec(r->right,id,in);
    else { r->info=in; return r; } /* Mise à jour si déjà existant */
    return rebalance(r);
}

/*
 * Fonction : si_add
 * Description : Ajoute ou met à jour une station dans l'index AVL
 * Paramètres :
 *   - idx : index des stations
 *   - id : identifiant de la station
 *   - in : informations de la station
 * Complexité temps : O(log n) - Insertion dans un AVL équilibré
 * Complexité espace : O(log n) - Pile de récursion
 */
void si_add(StationIndex* idx, int id, StationInfo in){
    idx->root=insert_rec(idx->root,id,in);
}

/*
 * Fonction auxiliaire : min_node
 * Description : Trouve le nœud avec l'ID minimum dans un sous-arbre
 *               (nœud le plus à gauche)
 * Paramètre :
 *   - r : racine du sous-arbre
 * Retour : Nœud avec l'ID minimum
 * Complexité temps : O(log n) - Descente vers la gauche
 * Complexité espace : O(1) - Parcours itératif
 */
static StationNode* min_node(StationNode* r){
    StationNode* c=r;
    while(c&&c->left) c=c->left;
    return c;
}

/*
 * Fonction auxiliaire récursive : delete_rec
 * Description : Supprime une station de l'AVL de manière récursive
 * Paramètres :
 *   - r : racine du sous-arbre courant
 *   - id : identifiant de la station à supprimer
 *   - found : pointeur vers un flag indiquant si la suppression a réussi
 * Retour : Nouvelle racine du sous-arbre après suppression et rééquilibrage
 * Complexité temps : O(log n) - Recherche + suppression + rééquilibrage
 * Complexité espace : O(log n) - Pile de récursion
 */
static StationNode* delete_rec(StationNode* r, int id, int* found){
    if(!r) return 0;
    if(id<r->station_id) r->left=delete_rec(r->left,id,found);
    else if(id>r->station_id) r->right=delete_rec(r->right,id,found);
    else{
        *found=1;
        // Cas 1 : Nœud feuille (pas d'enfants)
        if(!r->left && !r->right){ free(r); return 0; }
        // Cas 2 : Un seul enfant (droite)
        else if(!r->left){ StationNode* t=r->right; free(r); return t; }
        // Cas 3 : Un seul enfant (gauche)
        else if(!r->right){ StationNode* t=r->left;  free(r); return t; }
        // Cas 4 : Deux enfants - remplacer par le successeur (min du sous-arbre droit)
        else {
            StationNode* s=min_node(r->right);
            r->station_id=s->station_id;
            r->info=s->info;
            r->right=delete_rec(r->right,s->station_id,found);
        }
    }
    return rebalance(r);
}

/*
 * Fonction : si_delete
 * Description : Supprime une station de l'index AVL
 * Paramètres :
 *   - idx : index des stations
 *   - id : identifiant de la station à supprimer
 * Retour : 1 si la station a été trouvée et supprimée, 0 sinon
 * Complexité temps : O(log n) - Suppression dans un AVL équilibré
 * Complexité espace : O(log n) - Pile de récursion
 */
int si_delete(StationIndex* idx, int id){
    int f=0;
    idx->root=delete_rec(idx->root,id,&f);
    return f;
}

/*
 * Fonction : si_to_array
 * Description : Convertit l'AVL en tableau trié (parcours in-order)
 * Paramètres :
 *   - r : racine de l'arbre
 *   - ids : tableau destination pour les IDs
 *   - cap : capacité maximale du tableau
 * Retour : Nombre d'IDs écrits dans le tableau
 * Complexité temps : O(n) - Visite chaque nœud une fois (parcours in-order)
 * Complexité espace : O(log n) - Pile de récursion
 */
int si_to_array(StationNode* r, int* ids, int cap){
    if(!r || cap<=0) return 0;
    int w=0, wl=si_to_array(r->left, ids, cap);
    w+=wl;
    if(w<cap){ ids[w++]=r->station_id; }
    if(w<cap){ int wr=si_to_array(r->right, ids+w, cap-w); w+=wr; }
    return w;
}

/*
 * Fonction auxiliaire récursive : free_post
 * Description : Libère récursivement tous les nœuds (parcours post-order)
 * Paramètre :
 *   - r : racine du sous-arbre à libérer
 * Complexité temps : O(n) - Visite chaque nœud pour le libérer
 * Complexité espace : O(log n) - Pile de récursion
 */
static void free_post(StationNode* r){
    if(!r) return;
    free_post(r->left);
    free_post(r->right);
    free(r);
}

/*
 * Fonction : si_clear
 * Description : Libère toute la mémoire de l'index AVL
 * Paramètre :
 *   - idx : index des stations
 * Complexité temps : O(n) - Libération de tous les nœuds
 * Complexité espace : O(log n) - Pile de récursion
 */
void si_clear(StationIndex* idx){
    free_post(idx->root);
    idx->root=0;
}

/*
 * Fonction : si_print_sideways
 * Description : Affiche l'arbre AVL couché sur le côté (droite en haut, gauche en bas)
 *               Utile pour visualiser la structure et vérifier l'équilibrage
 * Paramètre :
 *   - r : racine de l'arbre
 * Complexité temps : O(n) - Visite chaque nœud
 * Complexité espace : O(log n) - Pile de récursion + variable statique depth
 */
void si_print_sideways(StationNode* r){
    static int depth=0;
    if(!r) return;
    depth++;
    si_print_sideways(r->right);
    for(int i=1;i<depth;i++) printf("    ");
    printf("%d(h=%d) P=%dKW Price=%dc Slots=%d\n",
           r->station_id, r->height, r->info.power_kW,
           r->info.price_cents, r->info.slots_free);
    si_print_sideways(r->left);
    depth--;
}

/*
 * Fonction : si_min
 * Description : Trouve la station avec l'ID minimum dans l'AVL
 * Paramètre :
 *   - r : racine de l'arbre
 * Retour : Nœud avec l'ID minimum, ou NULL si arbre vide
 * Complexité temps : O(log n) - Descente vers le nœud le plus à gauche
 * Complexité espace : O(1) - Parcours itératif
 */
StationNode* si_min(StationNode* r){
    if (!r) return NULL;
    while (r->left) {
        r = r->left;
    }
    return r;
}

/*
 * Fonction : si_max
 * Description : Trouve la station avec l'ID maximum dans l'AVL
 * Paramètre :
 *   - r : racine de l'arbre
 * Retour : Nœud avec l'ID maximum, ou NULL si arbre vide
 * Complexité temps : O(log n) - Descente vers le nœud le plus à droite
 * Complexité espace : O(1) - Parcours itératif
 */
StationNode* si_max(StationNode* r){
    if (!r) return NULL;
    while (r->right) {
        r = r->right;
    }
    return r;
}