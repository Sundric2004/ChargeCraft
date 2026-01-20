#include <stdio.h>
#include <stdlib.h>

#include "events.h"
#include "queue.h"
#include "slist.h"
#include "station_index.h"
#include "csv_loader.h"
#include "json_loader.h"
#include "nary.h"

#define MAX_VEH 100
#define MRU_CAP 5

SList VEH_MRU[MAX_VEH];

/*
 * Fonction : add_to_mru
 * Description : Ajoute une station à l'historique MRU d'un véhicule
 *               Évite les doublons et maintient une taille maximale
 * Paramètres :
 *   - veh_id : identifiant du véhicule
 *   - station_id : identifiant de la station visitée
 * Complexité temps : O(n) où n = MRU_CAP (typiquement 5)
 *                    Suppression d'éventuel doublon + insertion en tête + éventuelle suppression en queue
 * Complexité espace : O(1) - Pas d'allocation supplémentaire si capacité non dépassée
 */
void add_to_mru(int veh_id, int station_id){
    if(veh_id<0 || veh_id>=MAX_VEH) return;

    // Supprimer la station si elle existe déjà (éviter doublons)
    ds_slist_remove_value(&VEH_MRU[veh_id], station_id);

    // Insérer en tête (station la plus récente)
    ds_slist_insert_head(&VEH_MRU[veh_id], station_id);

    // Si la capacité est dépassée, supprimer la station la plus ancienne (en queue)
    int drop;
    int count=0;
    SNode* c=VEH_MRU[veh_id].head;
    while(c){ count++; c=c->next; }
    if(count>MRU_CAP) ds_slist_remove_tail(&VEH_MRU[veh_id], &drop);
}

/*
 * Fonction : process_events
 * Description : Traite une file d'événements (branchement/débranchement de véhicules)
 *               Met à jour le MRU et l'état des stations dans l'AVL
 * Paramètres :
 *   - q : queue d'événements à traiter
 *   - idx : index AVL des stations
 * Complexité temps : O(k * log n) où k = nombre d'événements, n = nombre de stations
 *                    Chaque événement nécessite une recherche O(log n) et une insertion O(log n)
 * Complexité espace : O(log n) - Pile de récursion pour l'AVL
 */
void process_events(Queue* q, StationIndex* idx){
    Event e;
    while(q_dequeue(q, &e)){
        // Mettre à jour le MRU du véhicule
        add_to_mru(e.vehicle_id, e.station_id);

        // Récupérer l'état actuel de la station (ou créer avec valeurs par défaut)
        StationNode* sn = si_find(idx->root, e.station_id);
        StationInfo info;
        if(sn){
            info=sn->info;
        }
        else {
            // Valeurs par défaut pour une nouvelle station
            info.power_kW=50;
            info.price_cents=300;
            info.slots_free=2;
            info.last_ts=0;
        }

        // Mettre à jour selon l'action
        if(e.action==1) { // Branchement (plug_in)
            if(info.slots_free>0) info.slots_free--;
        }
        if(e.action==0) { // Débranchement (plug_out)
            info.slots_free++;
        }

        info.last_ts = e.ts;

        // Mettre à jour dans l'AVL
        si_add(idx, e.station_id, info);
    }
}

/* Déclaration de la fonction d'évaluation des règles postfix (définie dans rules.c) */
int eval_rule_postfix(char* toks[], int n, StationInfo* info);

/*
 * Fonction : demo_query_top_n
 * Description : Démonstration d'une requête Top-N avec filtrage par règle postfix
 *               Répond à la consigne 6 du projet
 * Paramètres :
 *   - idx : index AVL des stations
 *   - rule : tableau de tokens représentant la règle postfix
 *   - rule_len : nombre de tokens dans la règle
 *   - n : nombre maximum de résultats à afficher
 *
 * Algorithme :
 *   1) Récupérer toutes les stations triées par ID (parcours in-order de l'AVL)
 *   2) Filtrer avec la règle postfix
 *   3) Sélectionner les N premiers résultats
 *
 * Complexité temps : O(m + k) où m = nombre total de stations, k = stations matchées
 *                    Parcours in-order O(m) + filtrage O(m) + affichage O(min(n,k))
 * Complexité espace : O(m) - Tableau temporaire pour stocker tous les IDs
 */
void demo_query_top_n(StationIndex* idx, char* rule[], int rule_len, int n) {
    printf("\n=== Query: Top-%d stations matching rule ===\n", n);

    // Étape 1 : Récupérer TOUTES les stations triées par ID (Parcours In-Order de l'AVL)
    int buffer_cap = 1024;
    int* ids = (int*)malloc(sizeof(int) * buffer_cap);
    if (!ids) return;

    int total_stations = si_to_array(idx->root, ids, buffer_cap);

    // Étape 2 & 3 : Filtrer et Sélectionner les N premiers
    int found_count = 0;
    for (int i = 0; i < total_stations && found_count < n; i++) {
        StationNode* s = si_find(idx->root, ids[i]);

        // Application de la règle Postfix
        if (s && eval_rule_postfix(rule, rule_len, &s->info)) {
            printf("  [MATCH #%d] Station %d : Power=%dkW, Slots=%d, Price=%d centimes\n",
                   found_count + 1, s->station_id, s->info.power_kW,
                   s->info.slots_free, s->info.price_cents);
            found_count++;
        }
    }

    if (found_count == 0) {
        printf("  No stations match this rule.\n");
    }

    free(ids);
}

/*
 * Fonction principale
 * Description : Point d'entrée du programme, orchestre toutes les démonstrations
 *
 * Démonstrations effectuées :
 *   1) Chargement des données (CSV + JSON)
 *   2) Ingestion d'événements et mise à jour des états
 *   3) Affichage de l'AVL (visualisation de l'équilibrage)
 *   4) Application de règles postfix
 *   5) Hiérarchie géographique (arbre n-aire) avec agrégation
 *   6) Historiques MRU des véhicules
 *   7) Requête Top-N avec filtrage
 */
int main(void){
    // Initialisation des MRU pour tous les véhicules
    for(int i=0;i<MAX_VEH;i++) ds_slist_init(&VEH_MRU[i]);

    // Initialisation de l'index AVL et de la queue d'événements
    StationIndex idx; si_init(&idx);
    Queue q; q_init(&q);

    /* ========== CHARGEMENT DES DONNÉES ========== */
    printf("=== Loading Datasets ===\n");

    // Chargement du CSV (300 stations réelles)
    int c1 = ds_load_stations_from_csv("izivia_tp_subset.csv", &idx);
    printf("CSV loaded: %d stations\n", c1);

    // Chargement du JSON (10 stations - optionnel)
    int c2 = ds_load_stations_from_json("izivia_tp_min.json", &idx);
    if(c2 > 0){
        printf("JSON loaded: %d stations\n", c2);
    } else {
        printf("JSON loading skipped or failed (optional)\n");
    }

    /* ========== INGESTION D'ÉVÉNEMENTS ========== */
    printf("\n=== Processing Events ===\n");
    for(int i=0;i<DS_EVENTS_COUNT;i++) {
        q_enqueue(&q, DS_EVENTS[i]);
    }
    process_events(&q, &idx);
    printf("Processed %d events\n", DS_EVENTS_COUNT);

    /* ========== DÉMONSTRATION 1 : AVL SIDEWAYS ========== */
    printf("\n=== AVL Tree Structure (Sideways View) ===\n");
    printf("Legend: ID(height=h) Power=XkW Price=Yc Slots=Z\n\n");
    si_print_sideways(idx.root);

    /* ========== DÉMONSTRATION 2 : RÈGLE POSTFIX SIMPLE ========== */
    printf("\n=== Rule Filtering: power >= 50 && slots >= 1 ===\n");
    char* rule1[] = { "slots","1",">=","power","50",">=","&&" };
    int ids[64];
    int k = si_to_array(idx.root, ids, 64);
    printf("Matching stations (first 40): ");
    int displayed = 0;
    for(int i=0; i<k && displayed<40; i++){
        StationNode* s = si_find(idx.root, ids[i]);
        if(s && eval_rule_postfix(rule1, 7, &s->info)) {
            printf("%d ", ids[i]);
            displayed++;
        }
    }
    printf("\n(Total: %d/%d stations in AVL)\n", k, k);

    /* ========== DÉMONSTRATION 3 : HISTORIQUE MRU ========== */
    printf("\n=== Vehicle MRU History (Last Visited Stations) ===\n");
    for(int v=1; v<=3; v++){
        printf("Vehicle %d visited: ", v);
        ds_slist_print(&VEH_MRU[v]);
    }

    /* ========== DÉMONSTRATION 4 : ARBRE N-AIRE (GÉOGRAPHIE) ========== */
    printf("\n=== Geographic Hierarchy (N-ary Tree) ===\n");

    // Construction de la taxonomie
    NNode* country = n_create(1);   // France (ID=1)
    NNode* reg_idf = n_create(11);  // Île-de-France (ID=11)
    NNode* reg_paca = n_create(12); // PACA (ID=12)
    n_attach(country, reg_idf);
    n_attach(country, reg_paca);

    NNode* city_paris = n_create(111);     // Paris (ID=111)
    NNode* city_marseille = n_create(121); // Marseille (ID=121)
    n_attach(reg_idf, city_paris);
    n_attach(reg_paca, city_marseille);

    // Feuilles (Groupes de stations avec nombre de slots libres)
    NNode* group_eiffel = n_create(11101);
    group_eiffel->items_count = 10;
    n_attach(city_paris, group_eiffel);

    NNode* group_louvre = n_create(11102);
    group_louvre->items_count = 5;
    n_attach(city_paris, group_louvre);

    NNode* group_vieuxport = n_create(12101);
    group_vieuxport->items_count = 8;
    n_attach(city_marseille, group_vieuxport);

    // Affichage BFS de la topologie
    printf("\n-> Topology (BFS traversal):\n");
    n_bfs_print(country);

    // Agrégation (somme récursive des items_count)
    printf("\n-> Aggregation Results:\n");
    printf("   Total slots free in Country (France): %d\n", n_aggregate(country));
    printf("   Total slots free in Île-de-France: %d\n", n_aggregate(reg_idf));
    printf("   Total slots free in Paris: %d\n", n_aggregate(city_paris));
    printf("   Total slots free in PACA: %d\n", n_aggregate(reg_paca));

    /* ========== DÉMONSTRATION 5 : REQUÊTE TOP-N ========== */
    // Règle : "Avoir au moins 1 place libre ET puissance >= 22kW"
    // Notation Postfix : slots 1 >= power 22 >= &&
    char* my_rule[] = { "slots", "1", ">=", "power", "22", ">=", "&&" };
    int rule_len = 7;
    demo_query_top_n(&idx, my_rule, rule_len, 5);

    /* ========== DÉMONSTRATION 6 : FONCTIONS MIN/MAX ========== */
    printf("\n=== AVL Min/Max Stations ===\n");
    StationNode* min_station = si_min(idx.root);
    StationNode* max_station = si_max(idx.root);
    if(min_station){
        printf("Min station ID: %d (Power=%dkW, Slots=%d)\n",
               min_station->station_id, min_station->info.power_kW,
               min_station->info.slots_free);
    }
    if(max_station){
        printf("Max station ID: %d (Power=%dkW, Slots=%d)\n",
               max_station->station_id, max_station->info.power_kW,
               max_station->info.slots_free);
    }

    /* ========== NETTOYAGE ========== */
    printf("\n=== Cleanup ===\n");
    n_clear(country);
    si_clear(&idx);
    for(int i=0; i<MAX_VEH; i++) ds_slist_clear(&VEH_MRU[i]);

    printf("Simulation finished cleanly. All memory freed.\n");
    return 0;
}