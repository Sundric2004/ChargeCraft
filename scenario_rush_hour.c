#include <stdio.h>
#include <stdlib.h>
#include "station_index.h"
#include "queue.h"
#include "events.h"
#include "advanced_queries.h"
#include "csv_loader.h"

/*
 * ============================================================================
 * SCÉNARIO B1 : HEURE DE POINTE
 * ============================================================================
 * 
 * Objectif : Simuler une affluence massive de véhicules (heure de pointe)
 *            et démontrer l'efficacité des requêtes avancées
 * 
 * Étapes :
 *   1. Charger le dataset CSV (300 stations)
 *   2. Afficher l'état initial (Top-5 par score)
 *   3. Simuler une rafale d'événements (100+ arrivées)
 *   4. Afficher l'état après la rafale
 *   5. Utiliser Range Query pour analyser une zone spécifique
 *   6. Compter les stations haute puissance disponibles
 * 
 * Démontre : A1 (range queries), A2 (top-k), gestion d'événements massifs
 */

/*
 * Fonction : process_single_event
 * Description : Traite un événement unique (branchement/débranchement)
 */
static void process_single_event(Event e, StationIndex* idx) {
    // Rechercher la station
    StationNode* sn = si_find(idx->root, e.station_id);
    StationInfo info;
    
    if (sn) {
        info = sn->info;
    } else {
        // Créer avec valeurs par défaut si inexistante
        info.power_kW = 50;
        info.price_cents = 300;
        info.slots_free = 2;
        info.last_ts = 0;
    }
    
    // Mettre à jour selon l'action
    if (e.action == 1) {  // Branchement
        if (info.slots_free > 0) {
            info.slots_free--;
        }
    } else if (e.action == 0) {  // Débranchement
        info.slots_free++;
    }
    
    info.last_ts = e.ts;
    
    // Mettre à jour dans l'AVL
    si_add(idx, e.station_id, info);
}

/*
 * Fonction : generate_rush_hour_events
 * Description : Génère une rafale d'événements simulant une heure de pointe
 * Retour : Nombre d'événements générés
 */
static int generate_rush_hour_events(Queue* q, int num_events) {
    // Générer des événements aléatoires sur les stations 1001-1300
    for (int i = 0; i < num_events; i++) {
        Event e;
        e.ts = 100 + i;
        e.vehicle_id = 10 + (i % 50);  // 50 véhicules différents
        e.station_id = 1001 + (i % 300);  // Répartis sur 300 stations
        e.action = (i % 3 == 0) ? 0 : 1;  // 66% branchements, 33% débranchements
        
        q_enqueue(q, e);
    }
    return num_events;
}

/*
 * Fonction : print_top_k_stations
 * Description : Affiche le Top-K des stations par score
 */
static void print_top_k_stations(StationIndex* idx, int k, int alpha, int beta, int gamma) {
    int* top_ids = (int*)malloc(sizeof(int) * k);
    if (!top_ids) return;
    
    int count = si_top_k_by_score(idx->root, k, top_ids, alpha, beta, gamma);
    
    printf("  Top-%d stations (score = %d*slots + %d*power - %d*price):\n", 
           k, alpha, beta, gamma);
    
    for (int i = 0; i < count; i++) {
        StationNode* s = si_find(idx->root, top_ids[i]);
        if (s) {
            int score = s->info.slots_free * alpha + 
                       s->info.power_kW * beta - 
                       s->info.price_cents * gamma;
            printf("    #%d - Station %d : slots=%d, power=%dkW, price=%dc, score=%d\n",
                   i + 1, s->station_id, s->info.slots_free, 
                   s->info.power_kW, s->info.price_cents, score);
        }
    }
    
    free(top_ids);
}

/*
 * Fonction principale du scénario
 */
void run_rush_hour_scenario(void) {
    printf("\n");
    printf("================================================================================\n");
    printf("                    SCÉNARIO B1 : HEURE DE POINTE                              \n");
    printf("================================================================================\n");
    printf("\n");
    
    // ========== ÉTAPE 1 : CHARGEMENT DES DONNÉES ==========
    printf(">>> ÉTAPE 1 : Chargement du dataset CSV\n");
    
    StationIndex idx;
    si_init(&idx);
    
    int loaded = ds_load_stations_from_csv("izivia_tp_subset.csv", &idx);
    if (loaded <= 0) {
        printf("[ERREUR] Impossible de charger le CSV. Fichier manquant ou vide.\n");
        return;
    }
    
    printf("    ✓ %d stations chargées avec succès\n\n", loaded);
    
    // ========== ÉTAPE 2 : ÉTAT INITIAL ==========
    printf(">>> ÉTAPE 2 : État AVANT l'heure de pointe\n");
    
    // Paramètres de scoring : slots=2, power=1, price=1
    int alpha = 2, beta = 1, gamma = 1;
    print_top_k_stations(&idx, 5, alpha, beta, gamma);
    
    // Statistiques initiales
    int high_power = si_count_ge_power(idx.root, 100);
    printf("\n  Statistiques :\n");
    printf("    - Stations haute puissance (≥100kW) : %d\n", high_power);
    
    // Range query : zone 1100-1150
    int range_ids[100];
    int range_count = si_range_ids(idx.root, 1100, 1150, range_ids, 100);
    printf("    - Stations dans la zone [1100-1150] : %d\n", range_count);
    printf("\n");
    
    // ========== ÉTAPE 3 : SIMULATION HEURE DE POINTE ==========
    printf(">>> ÉTAPE 3 : Simulation d'une RAFALE d'événements (heure de pointe)\n");
    
    Queue q;
    q_init(&q);
    
    int num_events = 150;  // 150 événements
    int generated = generate_rush_hour_events(&q, num_events);
    printf("    ✓ %d événements générés\n", generated);
    
    // Traiter tous les événements
    Event e;
    int processed = 0;
    while (q_dequeue(&q, &e)) {
        process_single_event(e, &idx);
        processed++;
    }
    
    printf("    ✓ %d événements traités\n\n", processed);
    
    // ========== ÉTAPE 4 : ÉTAT APRÈS LA RAFALE ==========
    printf(">>> ÉTAPE 4 : État APRÈS l'heure de pointe\n");
    
    print_top_k_stations(&idx, 5, alpha, beta, gamma);
    
    // Nouvelles statistiques
    high_power = si_count_ge_power(idx.root, 100);
    printf("\n  Statistiques :\n");
    printf("    - Stations haute puissance (≥100kW) : %d\n", high_power);
    
    range_count = si_range_ids(idx.root, 1100, 1150, range_ids, 100);
    printf("    - Stations dans la zone [1100-1150] : %d\n", range_count);
    
    // ========== ÉTAPE 5 : ANALYSE DÉTAILLÉE D'UNE ZONE ==========
    printf("\n>>> ÉTAPE 5 : Analyse détaillée de la zone [1100-1110]\n");
    
    int zone_ids[20];
    int zone_count = si_range_ids(idx.root, 1100, 1110, zone_ids, 20);
    
    printf("    Stations trouvées dans cette zone : %d\n", zone_count);
    printf("    Détails :\n");
    
    for (int i = 0; i < zone_count && i < 10; i++) {
        StationNode* s = si_find(idx.root, zone_ids[i]);
        if (s) {
            printf("      - Station %d : %d slots libres, %dkW\n",
                   s->station_id, s->info.slots_free, s->info.power_kW);
        }
    }
    
    // ========== ÉTAPE 6 : RECOMMANDATIONS ==========
    printf("\n>>> ÉTAPE 6 : Recommandations pour les utilisateurs\n");
    
    // Top-3 avec pondération forte sur les slots disponibles
    int top_available[3];
    int avail_count = si_top_k_by_score(idx.root, 3, top_available, 10, 1, 1);
    
    printf("    Top-3 stations les plus disponibles (score = 10*slots + power - price) :\n");
    for (int i = 0; i < avail_count; i++) {
        StationNode* s = si_find(idx.root, top_available[i]);
        if (s) {
            printf("      %d. Station %d (%d slots libres, %dkW)\n",
                   i + 1, s->station_id, s->info.slots_free, s->info.power_kW);
        }
    }
    
    printf("\n");
    printf("================================================================================\n");
    printf("                         FIN DU SCÉNARIO B1                                    \n");
    printf("================================================================================\n");
    printf("\n");
    
    // Nettoyage
    si_clear(&idx);
}