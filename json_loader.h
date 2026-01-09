#ifndef DS_JSON_LOADER_H
#define DS_JSON_LOADER_H
#include "station_index.h"
int ds_load_stations_from_json(const char* path, StationIndex* idx);
#endif
