#ifndef DS_QUEUE_H
#define DS_QUEUE_H
#include "events.h"

typedef struct QNode { Event e; struct QNode* next; } QNode;
typedef struct Queue { QNode* head; QNode* tail; } Queue;

/*
 * @brief Initialise une file vide.
 * @param     Pointeur vers la structure Queue à initialiser.
 * * Complexité Temps : O(1) - Simple mise à zéro des pointeurs de tête et de queue.
 * Complexité Espace : O(1) - Aucune allocation mémoire initiale.
 */
void q_init(Queue* q);                  /* O(1) */

/*
 * @brief Vérifie si la file ne contient aucun événement.
 * @param     Pointeur vers la file.
 * @return     1 si la file est vide, 0 sinon.
 * * Complexité Temps : O(1) - Test direct sur le pointeur de tête.
 * Complexité Espace : O(1) - Aucun espace supplémentaire requis.
 */
int  q_is_empty(Queue* q);              /* O(1) */

/*
 * @brief Ajoute un événement à la fin de la file (Enfiler).
 * @param     Pointeur vers la file.
 * @param     L'événement (Event) à ajouter (ts, vehicle_id, station_id, action).
 * @return     1 si l'ajout a réussi, 0 en cas d'échec d'allocation mémoire.
 * * Complexité Temps : O(1) - Accès direct à la fin via le pointeur 'tail'.
 * Complexité Espace : O(1) - Allocation d'un unique nœud par événement.
 */
int  q_enqueue(Queue* q, Event e);      /* O(1) */

/*
 * @brief Retire l'événement le plus ancien en tête de file (Défiler).
 * @param     Pointeur vers la file.
 * @param   Pointeur pour récupérer l'événement retiré.
 * @return     1 si un événement a été retiré, 0 si la file était vide.
 * * Complexité Temps : O(1) - Suppression directe en tête de file.
 * Complexité Espace : O(1) - Libération d'un seul nœud mémoire.
 */
int  q_dequeue(Queue* q, Event* out);   /* O(1) */

/*
 * @brief Vide intégralement la file et libère la mémoire associée.
 * @param     Pointeur vers la file à nettoyer.
 * * Complexité Temps : O(n) - Parcourt et libère chaque nœud de la file.
 * Complexité Espace : O(1) - Utilise une variable temporaire pour le défilage.
 */
void q_clear(Queue* q);                 /* O(n) */

#endif
