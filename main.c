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

void add_to_mru(int veh_id, int station_id){
    if(veh_id<0 || veh_id>=MAX_VEH) return;
    ds_slist_remove_value(&VEH_MRU[veh_id], station_id);
    ds_slist_insert_head(&VEH_MRU[veh_id], station_id);
    int drop;
    int count=0; SNode* c=VEH_MRU[veh_id].head; while(c){ count++; c=c->next; }
    if(count>MRU_CAP) ds_slist_remove_tail(&VEH_MRU[veh_id], &drop);
}

void process_events(Queue* q, StationIndex* idx){
    Event e;
    while(q_dequeue(q, &e)){
        add_to_mru(e.vehicle_id, e.station_id);
        StationNode* sn = si_find(idx->root, e.station_id);
        StationInfo info;
        if(sn){ info=sn->info; }
        else { info.power_kW=50; info.price_cents=300; info.slots_free=2; info.last_ts=0; }
        if(e.action==1) { if(info.slots_free>0) info.slots_free--; }
        if(e.action==0) { info.slots_free++; }
        info.last_ts = e.ts;
        si_add(idx, e.station_id, info);
    }
}

int eval_rule_postfix(char* toks[], int n, StationInfo* info); /* from rules.c */

/* Fonction de démonstration pour la consigne 6 */
void demo_query_top_n(StationIndex* idx, char* rule[], int rule_len, int n) {
    printf("\n=== Query: Top-%d stations matching rule ===\n", n);

    // Étape 1 : Récupérer TOUTES les stations triées par ID (Parcours In-Order de l'AVL)
    // On alloue un buffer temporaire assez grand pour la démo
    int buffer_cap = 1024;
    int* ids = (int*)malloc(sizeof(int) * buffer_cap);
    if (!ids) return;

    int total_stations = si_to_array(idx->root, ids, buffer_cap); //

    // Étape 2 & 3 : Filtrer et Sélectionner les N premiers
    int found_count = 0;
    for (int i = 0; i < total_stations && found_count < n; i++) {
        StationNode* s = si_find(idx->root, ids[i]); //

        // Application de la règle Postfix
        if (s && eval_rule_postfix(rule, rule_len, &s->info)) {
            printf("  [MATCH #%d] Station %d : Power=%dkW, Slots=%d, Price=%d\n",
                   found_count + 1, s->station_id, s->info.power_kW, s->info.slots_free, s->info.price_cents);
            found_count++;
        }
    }

    if (found_count == 0) {
        printf("  No stations match this rule.\n");
    }

    free(ids);
}

int main(void){
    for(int i=0;i<MAX_VEH;i++) ds_slist_init(&VEH_MRU[i]);

    StationIndex idx; si_init(&idx);
    Queue q; q_init(&q);

    /* Load provided datasets (place files next to binary or run from project root) */
    int c1 = ds_load_stations_from_csv("izivia_tp_subset.csv", &idx);
    printf("CSV loaded: %d stations\n", c1);
    int c2 = ds_load_stations_from_json("izivia_tp_min.json", &idx);
    printf("JSON loaded: %d stations (optional)\n", c2);

    for(int i=0;i<DS_EVENTS_COUNT;i++) q_enqueue(&q, DS_EVENTS[i]);
    process_events(&q, &idx);

    printf("\nAVL stations (sideways):\n");
    si_print_sideways(idx.root);

    /* Simple rule: power>=50 && slots>=1 */
    char* rule[] = { "slots","1",">=","power","50",">=","&&" };
    int ids[64]; int k = si_to_array(idx.root, ids, 64);
    printf("\nStations satisfying rule: ");
    for(int i=0;i<k;i++){
        StationNode* s = si_find(idx.root, ids[i]);
        if(s && eval_rule_postfix(rule, 7, &s->info)) {
            printf("%d ", ids[i]);
        }
    }
    printf("\n");

    printf("MRU vehicle 3: "); ds_slist_print(&VEH_MRU[3]);




    /* --- PARTIE 5 : Arbre N-aire (Géographie) --- */
    printf("\n=== Geographic Hierarchy (N-ary Tree) ===\n");

    /* --- DEMO 1 : Affichage "Pretty" de l'AVL (Sideways) --- */
    printf("\nAVL Tree Structure (Sideways):\n");
    si_print_sideways(idx.root);

    /* --- DEMO 2 : Arbre N-aire (Géographie & Agrégation) --- */
    printf("\n[DEMO 2] Geographic Hierarchy (N-ary Tree):\n");

    // Construction de la taxonomie
    NNode* country = n_create(1);   // France
    NNode* reg_idf = n_create(11);  // Île-de-France
    NNode* reg_paca = n_create(12); // PACA
    n_attach(country, reg_idf);
    n_attach(country, reg_paca);

    NNode* city_paris = n_create(111);
    NNode* city_marseille = n_create(121);
    n_attach(reg_idf, city_paris);
    n_attach(reg_paca, city_marseille);

    // Feuilles (Groupes de stations)
    NNode* group_eiffel = n_create(11101); group_eiffel->items_count = 10;
    n_attach(city_paris, group_eiffel);

    NNode* group_louvre = n_create(11102); group_louvre->items_count = 5;
    n_attach(city_paris, group_louvre);

    NNode* group_vieuxport = n_create(12101); group_vieuxport->items_count = 8;
    n_attach(city_marseille, group_vieuxport);

    // Affichage BFS
    printf("-> Topology BFS:\n");
    n_bfs_print(country);

    // Agrégation (Test de la fonction ajoutée en partie 5)
    int total_free = n_aggregate(country);
    printf("\n-> Aggregation Results:\n");
    printf("   Total slots free in Country: %d\n", total_free);
    printf("   Total slots free in Paris Region: %d\n", n_aggregate(reg_idf));


    /* --- DEMO 3 : Historique MRU --- */
    printf("\n[DEMO 3] Vehicle MRU History (Last visited stations):\n");
    // On affiche l'historique pour quelques véhicules clés
    for(int v=1; v<=3; v++){
        printf("   Vehicle %d visited: ", v);
        ds_slist_print(&VEH_MRU[v]);
    }


    /* --- DEMO 4 : Requête Top-N avec Règle Postfix --- */
    // Règle : "Avoir au moins 1 place libre ET puissance >= 22kW"
    // Notation Postfix : slots 1 >= power 22 >= &&
    char* my_rule[] = { "slots", "1", ">=", "power", "22", ">=", "&&" };
    int rule_len = 7;

    // On demande les 5 premières stations qui valident cette règle
    demo_query_top_n(&idx, my_rule, rule_len, 5);

    si_clear(&idx);
    for(int i=0; i<MAX_VEH; i++) ds_slist_clear(&VEH_MRU[i]);

    printf("\nSimulation finished cleanly.\n");

    n_clear(country);
    si_clear(&idx);
    return 0;
}
