#ifndef DS_STATION_INDEX_H
#define DS_STATION_INDEX_H

/* Structure contenant les informations d'une station de recharge */
typedef struct StationInfo {
    int power_kW;      /* puissance en kW */
    int price_cents;   /* tarif en centimes d'euro */
    int slots_free;    /* nombre de places libres */
    int last_ts;       /* dernier timestamp de mise à jour */
} StationInfo;

/* Nœud de l'arbre AVL représentant une station */
typedef struct StationNode {
    int station_id;             /* identifiant unique de la station */
    StationInfo info;           /* informations de la station */
    struct StationNode* left;   /* sous-arbre gauche */
    struct StationNode* right;  /* sous-arbre droit */
    int height;                 /* hauteur du nœud (pour équilibrage AVL) */
} StationNode;

/* Index des stations implémenté comme un arbre AVL */
typedef struct StationIndex {
    StationNode* root;  /* racine de l'arbre */
} StationIndex;

/* Initialisation de l'index - O(1) */
void si_init(StationIndex* idx);

/* Recherche d'une station par ID - O(log n) */
StationNode* si_find(StationNode* r, int id);

/* Ajout ou mise à jour d'une station - O(log n) */
void si_add(StationIndex* idx, int id, StationInfo in);

/* Suppression d'une station - O(log n) */
int si_delete(StationIndex* idx, int id);

/* Conversion en tableau trié (parcours in-order) - O(n) */
int si_to_array(StationNode* r, int* ids, int cap);

/* Affichage visuel de l'arbre (debug) - O(n) */
void si_print_sideways(StationNode* r);

/* Libération de la mémoire - O(n) */
void si_clear(StationIndex* idx);

/* Trouve la station avec l'ID minimum - O(log n) */
StationNode* si_min(StationNode* r);

/* Trouve la station avec l'ID maximum - O(log n) */
StationNode* si_max(StationNode* r);

#endif