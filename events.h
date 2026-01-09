#ifndef DS_EVENTS_H
#define DS_EVENTS_H

typedef struct Event {
    int ts;         /* timestamp simulé */
    int vehicle_id; /* identifiant véhicule */
    int station_id; /* identifiant station */
    int action;     /* 1=plug_in, 0=plug_out, -1=fault/reset ... */
} Event;

extern Event DS_EVENTS[];
extern int   DS_EVENTS_COUNT;

#endif
