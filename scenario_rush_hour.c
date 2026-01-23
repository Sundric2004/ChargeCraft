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

/*
 * @brief Fonction utilitaire pour segmenter la demonstration.
 * Nettoie le buffer et attend une action de l'utilisateur.
 */
static void wait_user() {
    printf("\n[Appuyez sur ENTREE pour continuer...]");
    fflush(stdout);
    while (getchar() != '\n');
}

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
    printf("\n================================================================================\n");
    printf("                    SCENARIO B1 : SIMULATION HEURE DE POINTE                    \n");
    printf("================================================================================\n");

    // ETAPE 1 : CHARGEMENT [cite: 97, 101]
    printf("\n>>> ETAPE 1 : Chargement du dataset CSV...\n");
    StationIndex idx;
    si_init(&idx);
    int loaded = ds_load_stations_from_csv("izivia_tp_subset.csv", &idx);
    if (loaded <= 0) {
        printf("[ERREUR] Impossible de charger le fichier CSV.\n");
        return;
    }
    printf("    -> %d stations chargees dans l'index AVL.\n", loaded);
    wait_user();

    // ==================ETAPE 2 : ETAT INITIAL=============================
    printf(">>> ETAPE 2 : Etat AVANT l'heure de pointe\n");

    // Paramètres de scoring : slots=2, power=1, price=1
    int alpha = 2, beta = 1, gamma = 1;
    print_top_k_stations(&idx, 5, alpha, beta, gamma);

    // Statistiques initiales
    int high_power = si_count_ge_power(idx.root, 100);
    printf("\n  Statistiques :\n");
    printf("    - Stations haute puissance (>= 100kW) : %d\n", high_power);

    // Range query : zone 1100-1150
    int range_ids[100];
    int range_count = si_range_ids(idx.root, 1100, 1150, range_ids, 100);
    printf("    - Stations dans la zone [1100-1150] : %d\n", range_count);
    printf("\n");
    wait_user();

    // ========== ÉTAPE 3 : SIMULATION HEURE DE POINTE ==========
    printf(">>> ETAPE 3 : Simulation d'une RAFALE d'evenements (heure de pointe)\n");

    Queue q;
    q_init(&q);

    int num_events = 150;  // 150 événements
    int generated = generate_rush_hour_events(&q, num_events);
    printf("     %d evenements generes\n", generated);

    // Traiter tous les événements
    Event e;
    int processed = 0;
    while (q_dequeue(&q, &e)) {
        process_single_event(e, &idx);
        processed++;
    }

    printf("     %d evenements traites\n\n", processed);
    wait_user();

    // ========== ÉTAPE 4 : ÉTAT APRÈS LA RAFALE ==========
    printf(">>> ETAPE 4 : Etat APRES l'heure de pointe\n");

    print_top_k_stations(&idx, 5, alpha, beta, gamma);

    // Nouvelles statistiques
    high_power = si_count_ge_power(idx.root, 100);
    printf("\n  Statistiques :\n");
    printf("    - Stations haute puissance (>= 100kW) : %d\n", high_power);

    range_count = si_range_ids(idx.root, 1100, 1150, range_ids, 100);
    printf("    - Stations dans la zone [1100-1150] : %d\n", range_count);
    wait_user();

    // ========== ÉTAPE 5 : ANALYSE DÉTAILLÉE D'UNE ZONE ==========
    printf("\n>>> ETAPE 5 : Analyse detaillee de la zone [1100-1110]\n");

    int zone_ids[20];
    int zone_count = si_range_ids(idx.root, 1100, 1110, zone_ids, 20);

    printf("    Stations trouvees dans cette zone : %d\n", zone_count);
    printf("    Details :\n");

    for (int i = 0; i < zone_count && i < 10; i++) {
        StationNode* s = si_find(idx.root, zone_ids[i]);
        if (s) {
            printf("      - Station %d : %d slots libres, %dkW\n",
                   s->station_id, s->info.slots_free, s->info.power_kW);
        }
    }

    wait_user();

    // ========== ÉTAPE 6 : RECOMMANDATIONS ==========
    printf("\n>>> ETAPE 6 : Recommandations pour les utilisateurs\n");

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

    // Nettoyage
    si_clear(&idx);
}