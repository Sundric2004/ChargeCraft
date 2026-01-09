#include "csv_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_station_id(const char* s){
    if(!s) return -1;
    const char* p = strrchr(s, '_');
    if(!p || !*(p+1)) return -1;
    return atoi(p+1);
}

static int split_csv_line(char* line, char* out[], int max_cols){
    int n = 0;
    char* tok = strtok(line, ",");
    while(tok && n < max_cols){
        char* end = tok + strlen(tok) - 1;
        while(end >= tok && (*end == '\n' || *end == '\r')) { *end = '\0'; end--; }
        out[n++] = tok;
        tok = strtok(NULL, ",");
    }
    return n;
}

int ds_load_stations_from_csv(const char* path, StationIndex* idx){
    FILE* f = fopen(path, "r");
    if(!f) return -1;

    char buf[2048];
    if(!fgets(buf, sizeof buf, f)){ fclose(f); return -1; }

    int inserted = 0;
    while(fgets(buf, sizeof buf, f)){
        char* cols[16];
        int n = split_csv_line(buf, cols, 16);
        if(n < 10) continue;

        int station_id = parse_station_id(cols[0]);
        if(station_id < 0) continue;

        StationInfo info;
        info.power_kW    = atoi(cols[5]);
        info.price_cents = 300;
        info.slots_free  = atoi(cols[6]);
        info.last_ts     = 0;

        si_add(idx, station_id, info);
        inserted++;
    }
    fclose(f);
    return inserted;
}
