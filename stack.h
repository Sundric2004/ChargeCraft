#ifndef DS_STACK_H
#define DS_STACK_H
typedef struct StNode{ int v; struct StNode* next; } StNode;
typedef struct Stack{ StNode* top; } Stack;

/*
 * @brief Initialise une pile à l'état vide.
 * @param    Pointeur vers la structure Stack à initialiser.
 * * Complexité Temps :  O(1) - Simple mise à zéro d'un pointeur.
 * Complexité Espace : O(1) - Aucune allocation mémoire.
 */
void st_init(Stack* s);            /* O(1) */

/*
 * @brief Empile un nouvel entier au sommet de la pile (opération Push).
 * @param    Pointeur vers la pile.
 * @param     Valeur entière à stocker sur la pile.
 * @return     1 si l'élément a été ajouté, 0 en cas d'échec de l'allocation.
 * * Complexité Temps :  O(1) - Insertion en tête de liste chaînée.
 * Complexité Espace : O(1) - Allocation d'un unique nœud par appel.
 */
int  st_push(Stack* s, int v);     /* O(1) */

/*
 * @brief Retire et renvoie l'élément situé au sommet de la pile (opération Pop).
 * @param    Pointeur vers la pile.
 * @param   Pointeur pour stocker la valeur retirée (facultatif).
 * @return     1 si un élément a été retiré, 0 si la pile était vide.
 * * Complexité Temps :  O(1) - Suppression en tête de liste chaînée.
 * Complexité Espace : O(1) - Libération d'un seul nœud mémoire.
 */
int  st_pop(Stack* s, int* out);   /* O(1) */

/*
 * @brief Indique si la pile contient des éléments ou non.
 * @param   Pointeur vers la pile.
 * @return     1 si la pile est vide, 0 sinon.
 * * Complexité Temps :  O(1) - Test de nullité sur le pointeur de sommet.
 * Complexité Espace : O(1) - Pas d'utilisation de mémoire supplémentaire.
 */
int  st_is_empty(Stack* s);        /* O(1) */

/*
 * @brief Vide la pile de tous ses éléments et libère la mémoire associée.
 * @param s    Pointeur vers la pile à nettoyer.
 * * Complexité Temps :  O(n) - Parcourt et libère chaque élément un par un.
 * Complexité Espace : O(1) - Utilise une variable temporaire pour le parcours.
 */
void st_clear(Stack* s);           /* O(n) */

#endif
