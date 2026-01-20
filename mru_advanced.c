#include "mru_advanced.h"
#include <stdio.h>

/*
 * ============================================================================
 * MODULE A4 : MRU CAPÉE SANS DOUBLONS - IMPLÉMENTATION
 * ============================================================================
 */

void mru_add_station(SList* mru, int station_id, int MRU_CAP) {
    if (!mru || MRU_CAP <= 0) return;
    
    // Étape 1 : Supprimer la station si elle existe déjà (éviter doublons)
    ds_slist_remove_value(mru, station_id);
    
    // Étape 2 : Insérer la station en tête (plus récente)
    ds_slist_insert_head(mru, station_id);
    
    // Étape 3 : Si la capacité est dépassée, supprimer la queue (plus ancienne)
    int length = mru_get_length(mru);
    if (length > MRU_CAP) {
        int dropped;
        ds_slist_remove_tail(mru, &dropped);
    }
}

int mru_get_length(SList* mru) {
    if (!mru) return 0;
    
    int count = 0;
    SNode* current = mru->head;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

int mru_contains(SList* mru, int station_id) {
    if (!mru) return 0;
    
    SNode* current = mru->head;
    while (current) {
        if (current->value == station_id) {
            return 1;  // Trouvé
        }
        current = current->next;
    }
    return 0;  // Pas trouvé
}