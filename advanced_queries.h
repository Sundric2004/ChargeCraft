#ifndef ADVANCED_QUERIES_H
#define ADVANCED_QUERIES_H

#include "station_index.h"

/*
 * ============================================================================
 * MODULE A1 : RANGE QUERIES SUR AVL
 * ============================================================================
 */

/*
 * Fonction : si_range_ids
 * Description : Extrait les IDs de stations dans un intervalle [lo, hi]
 *               Utilise le parcours in-order avec élagage intelligent
 * Paramètres :
 *   - r : racine de l'arbre AVL
 *   - lo : borne inférieure (inclusive)
 *   - hi : borne supérieure (inclusive)
 *   - out : tableau destination pour stocker les IDs
 *   - cap : capacité maximale du tableau
 * Retour : Nombre d'IDs écrits dans le tableau
 * Complexité temps : O(k + log n) où k = nombre d'éléments dans [lo, hi]
 *                    On coupe les branches inutiles (< lo ou > hi)
 * Complexité espace : O(log n) - Pile de récursion
 */
int si_range_ids(StationNode* r, int lo, int hi, int* out, int cap);

/*
 * Fonction : si_count_ge_power
 * Description : Compte le nombre de stations avec puissance >= P
 *               Parcours complet de l'arbre pour compter
 * Paramètre :
 *   - r : racine de l'arbre AVL
 *   - P : seuil de puissance en kW
 * Retour : Nombre de stations satisfaisant la condition
 * Complexité temps : O(n) - Visite chaque nœud
 * Complexité espace : O(log n) - Pile de récursion
 */
int si_count_ge_power(StationNode* r, int P);

/*
 * ============================================================================
 * MODULE A2 : TOP-K PAR SCORE (MIN-HEAP LOCAL)
 * ============================================================================
 */

/*
 * Structure pour stocker (ID, score)
 */
typedef struct {
    int station_id;
    int score;
} ScoredStation;

/*
 * Fonction : si_top_k_by_score
 * Description : Trouve les K stations avec les meilleurs scores
 *               Score = slots_free*alpha + power_kW*beta - price_cents*gamma
 *               Utilise un min-heap de taille k pour efficacité
 * Paramètres :
 *   - r : racine de l'arbre AVL
 *   - k : nombre de top stations à retourner
 *   - out_ids : tableau destination pour les IDs (taille >= k)
 *   - alpha : poids pour slots_free
 *   - beta : poids pour power_kW
 *   - gamma : poids pour price_cents (soustrait)
 * Retour : Nombre d'IDs écrits (min(k, nb_stations))
 * Complexité temps : O(n log k) - Parcours n stations, heap de taille k
 * Complexité espace : O(k + log n) - Heap de taille k + pile récursion
 * 
 * Exemple :
 *   alpha=2, beta=1, gamma=1
 *   Station A: slots=3, power=50, price=200 → score = 3*2 + 50*1 - 200*1 = -144
 *   Station B: slots=5, power=150, price=300 → score = 5*2 + 150*1 - 300*1 = -140
 *   → B meilleur que A
 */
int si_top_k_by_score(StationNode* r, int k, int* out_ids,
                      int alpha, int beta, int gamma);

#endif