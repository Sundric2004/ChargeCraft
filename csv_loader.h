#ifndef DS_CSV_LOADER_H
#define DS_CSV_LOADER_H
#include "station_index.h"

int ds_load_stations_from_csv(const char* path, StationIndex* idx);

#endif
