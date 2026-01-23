#ifndef DS_SLIST_H
#define DS_SLIST_H
typedef struct SNode { int value; struct SNode* next; } SNode;
typedef struct SList { SNode* head; } SList;


/**
 * @brief Initialise une liste chaînée simple à vide.
 * @param l    Pointeur vers la liste à initialiser.
 * * Complexité Temps : O(1) - Initialisation d'un pointeur à NULL.
 * Complexité Espace : O(1) - Aucune allocation.
 */
void ds_slist_init(SList* l);                  /* O(1) */

/**
 * @brief Insère un nouvel identifiant de station en tête de liste.
 * @param    Pointeur vers la liste.
 * @param     Identifiant de la station à ajouter.
 * @return     1 si l'insertion a réussi, 0 en cas d'échec d'allocation.
 * * Complexité Temps : O(1) - Insertion directe en tête de liste.
 * Complexité Espace : O(1) - Allocation d'un seul nœud.
 */
int  ds_slist_insert_head(SList* l, int v);    /* O(1) */

/**
 * @brief Recherche et supprime une valeur spécifique dans la liste.
 * @param     Pointeur vers la liste.
 * @param     Valeur (ID de station) à supprimer.
 * @return     1 si la valeur a été trouvée et supprimée, 0 sinon.
 * * Complexité Temps : O(n) - Parcours séquentiel pour trouver la valeur.
 * Complexité Espace : O(1) - Simple manipulation de pointeurs.
 */
int  ds_slist_remove_value(SList* l, int v);   /* O(n) */

/*
 * @brief Supprime l'élément en fin de liste (le plus ancien de l'historique).
 * @param     Pointeur vers la liste.
 * @param  Pointeur pour récupérer la valeur supprimée (optionnel).
 * @return     1 si un élément a été supprimé, 0 si la liste était vide.
 * * Complexité Temps : O(n) - Parcours obligatoire de toute la liste pour atteindre la queue.
 * Complexité Espace : O(1) - Libération d'un seul nœud.
 */
int  ds_slist_remove_tail(SList* l, int* out); /* O(n) */

/*
 * @brief Affiche le contenu de la liste sous forme visuelle [v1 -> v2 -> NULL].
 * @param    Pointeur vers la liste à afficher.
 * * Complexité Temps : O(n) - Parcours de tous les éléments pour l'impression.
 * Complexité Espace : O(1) - Pas d'utilisation de mémoire supplémentaire.
 */
void ds_slist_print(SList* l);                 /* O(n) */

/*
 * @brief Supprime tous les nœuds de la liste et libère la mémoire.
 * @param     Pointeur vers la liste à vider.
 * * Complexité Temps : O(n) - Parcours et libération de chaque nœud.
 * Complexité Espace : O(1) - Utilise une variable de parcours temporaire.
 */
void ds_slist_clear(SList* l);                 /* O(n) */

#endif
