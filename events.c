#include "events.h"

/* Tiny demo dataset */
Event DS_EVENTS[] = {
    {  1, 3, 101, 1 },   /* veh 3 plugs into station 101 */
    {  2, 7, 102, 1 },
    {  3, 3, 101, 0 },   /* veh 3 unplugs */
    {  4, 3, 102, 1 },   /* veh 3 goes to 102 */
    {  5, 9, 101, 1 },
    {  6, 7, 102, 0 },
    {  7, 3, 102, 0 },
    {  8, 9, 101, 0 },
    {  9, 3, 103, 1 },
    { 10, 3, 103, 0 },
};
int DS_EVENTS_COUNT = sizeof(DS_EVENTS)/sizeof(DS_EVENTS[0]);
