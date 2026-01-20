#ifndef DS_NARY_H
#define DS_NARY_H

/*
 * Structure de nœud pour l'arbre n-aire
 * Utilisé pour représenter une hiérarchie géographique:
 *   - Pays (ex: France)
 *   - Régions (ex: Île-de-France, PACA)
 *   - Villes (ex: Paris, Marseille)
 *   - Groupes de stations (ex: Tour Eiffel, Vieux Port)
 */
typedef struct NNode {
    int id;                    /* Identifiant unique du nœud */
    int items_count;           /* Nombre d'items à ce niveau (ex: slots libres) */
    struct NNode** child;      /* Tableau dynamique de pointeurs vers les enfants */
    int child_count;           /* Nombre d'enfants actuels */
    int child_cap;             /* Capacité actuelle du tableau d'enfants */
} NNode;

/* Crée un nouveau nœud - O(1) */
NNode* n_create(int id);

/* Attache un enfant à un parent - O(1) amorti */
int n_attach(NNode* parent, NNode* child);

/* Affiche l'arbre en parcours BFS - O(n) */
void n_bfs_print(NNode* root);

/* Libère toute la mémoire de l'arbre - O(n) */
void n_clear(NNode* root);

/* Agrégation récursive des items_count - O(n) */
int n_aggregate(NNode* root);

#endif