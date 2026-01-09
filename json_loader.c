#include "json_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int find_int_field(const char* s, const char* key, int* out) {
    char pat[128];
    snprintf(pat, sizeof pat, "\"%s\"", key);
    const char* p = strstr(s, pat);
    if(!p) return 0;
    p = strchr(p, ':'); if(!p) return 0;
    p++;
    while(*p==' '||*p=='\t') p++;
    *out = atoi(p);
    return 1;
}
static int find_str_suffix_id(const char* s, const char* key) {
    char pat[128];
    snprintf(pat, sizeof pat, "\"%s\"", key);
    const char* p = strstr(s, pat);
    if(!p) return -1;
    p = strchr(p, ':'); if(!p) return -1;
    p++;
    while(*p==' '||*p=='\t'||*p=='\"') p++;
    const char* end = strchr(p, '\"'); if(!end) end = p + strlen(p);
    const char* us = strrchr(p, '_');
    if(!us || us>=end) return -1;
    return atoi(us+1);
}

int ds_load_stations_from_json(const char* path, StationIndex* idx){
    FILE* f = fopen(path, "r");
    if(!f) return -1;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    char* buf = (char*)malloc(sz+1);
    if(!buf){ fclose(f); return -1; }
    if(fread(buf, 1, sz, f)!=(size_t)sz){ free(buf); fclose(f); return -1; }
    buf[sz]=0; fclose(f);

    int inserted = 0;
    char* p = buf;
    while((p = strchr(p, '{'))){
        char* q = strchr(p, '}');
        if(!q) break;
        *q = 0;
        int id = find_str_suffix_id(p, "id_station_itinerance");
        int power=0, slots=0;
        find_int_field(p, "puissance_nominale", &power);
        find_int_field(p, "nbre_pdc", &slots);

        if(id > 0){
            StationInfo info;
            info.power_kW    = power ? power : 50;
            info.price_cents = 300;
            info.slots_free  = slots ? slots : 2;
            info.last_ts     = 0;
            si_add(idx, id, info);
            inserted++;
        }
        p = q+1;
    }
    free(buf);
    return inserted;
}
