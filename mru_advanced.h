#ifndef MRU_ADVANCED_H
#define MRU_ADVANCED_H

#include "slist.h"

/*
 * ============================================================================
 * MODULE A4 : MRU CAPÉE SANS DOUBLONS
 * ============================================================================
 */

/*
 * Fonction : mru_add_station
 * Description : Ajoute une station à l'historique MRU (Most Recently Used)
 *               avec gestion automatique de la capacité et des doublons
 *
 * Algorithme :
 *   1. Supprimer la station si elle existe déjà (éviter doublons)
 *   2. Insérer la station en tête (plus récente)
 *   3. Si la longueur dépasse MRU_CAP, supprimer la queue (plus ancienne)
 *
 * Paramètres :
 *   - mru : pointeur vers la liste MRU
 *   - station_id : identifiant de la station à ajouter
 *   - MRU_CAP : capacité maximale de l'historique (ex: 5)
 *
 * Complexité temps : O(L) où L = longueur actuelle de la liste (≤ MRU_CAP)
 *                    - Suppression doublon : O(L)
 *                    - Insertion tête : O(1)
 *                    - Comptage + suppression queue : O(L)
 *                    En pratique : O(1) car MRU_CAP est petit (5-10)
 *
 * Complexité espace : O(1) - Pas d'allocation supplémentaire
 *
 * Exemple :
 *   MRU initial : [102 -> 101]  (CAP = 5)
 *   mru_add_station(mru, 103, 5) → [103 -> 102 -> 101]
 *   mru_add_station(mru, 102, 5) → [102 -> 103 -> 101]  (102 remonté en tête)
 */
void mru_add_station(SList* mru, int station_id, int MRU_CAP);

/*
 * Fonction : mru_get_length
 * Description : Retourne la longueur actuelle de la liste MRU
 * Paramètre :
 *   - mru : pointeur vers la liste MRU
 * Retour : Nombre d'éléments dans la liste
 * Complexité temps : O(L) - Parcours de la liste
 * Complexité espace : O(1)
 */
int mru_get_length(SList* mru);

/*
 * Fonction : mru_contains
 * Description : Vérifie si une station est dans l'historique MRU
 * Paramètres :
 *   - mru : pointeur vers la liste MRU
 *   - station_id : identifiant de la station à chercher
 * Retour : 1 si trouvée, 0 sinon
 * Complexité temps : O(L) - Parcours de la liste
 * Complexité espace : O(1)
 */
int mru_contains(SList* mru, int station_id);

#endif