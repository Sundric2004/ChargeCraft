#include <stdio.h>
#include "events.h"
#include "queue.h"
#include "slist.h"
#include "station_index.h"
#include "csv_loader.h"
#include "json_loader.h"

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

    si_clear(&idx);


    return 0;
}
