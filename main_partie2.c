#include <stdio.h>
#include <stdlib.h>
#include "station_index.h"
#include "csv_loader.h"
#include "advanced_queries.h"
#include "mru_advanced.h"
#include "scenario_rush_hour.h"
#include "slist.h"

/*
 * ============================================================================
 * ChargeCraft - PARTIE 2 : Modules Avancés et Scénario Applicatif
 * ============================================================================
 * 
 * Modules implémentés (Menu A) :
 *   - A1 : Range queries sur AVL (si_range_ids, si_count_ge_power)
 *   - A2 : Top-K par score avec min-heap (si_top_k_by_score)
 *   - A4 : MRU capée sans doublons (mru_add_station)
 * 
 * Scénario applicatif (Menu B) :
 *   - B1 : Heure de pointe (run_rush_hour_scenario)
 * 
 * Compilation :
 *   make partie2
 * 
 * Exécution :
 *   ./ev_partie2
 */

void print_separator(const char* title) {
    printf("\n");
    printf("################################################################################\n");
    printf("  %s\n", title);
    printf("################################################################################\n");
    printf("\n");
}

/*
 * Démonstration des modules A1 et A2 individuellement
 */
void demo_modules_standalone() {
    print_separator("DEMONSTRATION DES MODULES AVANCES");
    
    // Chargement CSV
    StationIndex idx;
    si_init(&idx);
    
    printf("Chargement du dataset...\n");
    int loaded = ds_load_stations_from_csv("izivia_tp_subset.csv", &idx);
    if (loaded <= 0) {
        printf("[ERREUR] Impossible de charger le CSV\n");
        return;
    }
    printf("%d stations chargees\n\n", loaded);
    
    // ========== MODULE A1 : RANGE QUERIES ==========
    printf("MODULE A1 : Range Queries sur AVL\n\n");
    
    // Test 1 : Range query [1100, 1150]
    printf("  Test 1 : Extraction des stations dans [1100, 1150]\n");
    int range_ids[100];
    int count = si_range_ids(idx.root, 1100, 1150, range_ids, 100);
    printf("    Resultat : %d stations trouvees\n", count);
    printf("    Premières IDs : ");
    for (int i = 0; i < count && i < 10; i++) {
        printf("%d ", range_ids[i]);
    }
    printf("%s\n\n", count > 10 ? "..." : "");
    
    // Test 2 : Comptage stations haute puissance
    printf("  Test 2 : Comptage des stations avec puissance >= 100 kW\n");
    int high_power_count = si_count_ge_power(idx.root, 100);
    printf("    Resultat : %d stations haute puissance\n\n", high_power_count);
    
    // Test 3 : Comptage stations >= 50 kW
    printf("  Test 3 : Comptage des stations avec puissance >= 50 kW\n");
    int medium_power_count = si_count_ge_power(idx.root, 50);
    printf("    Resultat : %d stations\n\n", medium_power_count);
    
    // ========== MODULE A2 : TOP-K PAR SCORE ==========
    printf(" MODULE A2 : Top-K par score avec min-heap\n\n");
    
    // Test 1 : Top-5 avec pondération équilibrée
    printf("  Test 1 : Top-5 (score = 2*slots + 1*power - 1*price)\n");
    int top5[5];
    int k = si_top_k_by_score(idx.root, 5, top5, 2, 1, 1);
    for (int i = 0; i < k; i++) {
        StationNode* s = si_find(idx.root, top5[i]);
        if (s) {
            int score = s->info.slots_free * 2 + s->info.power_kW * 1 - s->info.price_cents * 1;
            printf("    #%d - Station %d (score=%d, slots=%d, power=%dkW, price=%dc)\n",
                   i + 1, s->station_id, score, s->info.slots_free,
                   s->info.power_kW, s->info.price_cents);
        }
    }
    printf("\n");
    
    // Test 2 : Top-3 avec forte pondération sur disponibilité
    printf("  Test 2 : Top-3 (score = 10*slots + 1*power - 1*price)\n");
    printf("           (Favorise les stations les plus disponibles)\n");
    int top3[3];
    k = si_top_k_by_score(idx.root, 3, top3, 10, 1, 1);
    for (int i = 0; i < k; i++) {
        StationNode* s = si_find(idx.root, top3[i]);
        if (s) {
            int score = s->info.slots_free * 10 + s->info.power_kW * 1 - s->info.price_cents * 1;
            printf("    #%d - Station %d (score=%d, slots=%d)\n",
                   i + 1, s->station_id, score, s->info.slots_free);
        }
    }
    printf("\n");
    
    // ========== MODULE A4 : MRU AVANCÉE ==========
    printf(" MODULE A4 : MRU capee sans doublons\n\n");
    
    SList mru;
    ds_slist_init(&mru);
    int MRU_CAP = 5;
    
    printf("  Test : Ajout de 7 stations dans un MRU de capacite %d\n", MRU_CAP);
    
    // Ajouter des stations
    int test_stations[] = {101, 102, 103, 104, 105, 106, 107};
    for (int i = 0; i < 7; i++) {
        mru_add_station(&mru, test_stations[i], MRU_CAP);
        printf("    Apres ajout %d : ", test_stations[i]);
        ds_slist_print(&mru);
    }
    
    printf("\n  Test : Ajout d'une station deja presente (102)\n");
    mru_add_station(&mru, 102, MRU_CAP);
    printf("    Resultat : ");
    ds_slist_print(&mru);
    printf("    102 remonte en tete (pas de doublon)\n\n");
    
    // Vérification contains
    printf("  Test : Verification de presence\n");
    printf("    Station 102 dans MRU ? %s\n", mru_contains(&mru, 102) ? "OUI" : "NON");
    printf("    Station 101 dans MRU ? %s\n", mru_contains(&mru, 101) ? "OUI" : "NON");
    printf("    Station 999 dans MRU ? %s\n\n", mru_contains(&mru, 999) ? "OUI" : "NON");
    
    // Nettoyage
    ds_slist_clear(&mru);
    si_clear(&idx);
}

int main(void) {
    printf("\n");
    printf("================================================================================\n");
    printf("                                                                                \n");
    printf("                          ChargeCraft - PARTIE 2                                \n");
    printf("                    Modules Avances & Scenario Applicatif                       \n");
    printf("                                                                                \n");
    printf("================================================================================\n");
    printf("\n");
    
    // Démonstration des modules individuellement
    demo_modules_standalone();
    
    // Scénario applicatif complet B1
    run_rush_hour_scenario();
    
    printf("\n");
    printf("================================================================================\n");
    printf("                     FIN DE LA DEMONSTRATION PARTIE 2                          \n");
    printf("================================================================================\n");
    printf("\n");
    
    return 0;
}