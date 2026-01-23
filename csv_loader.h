#ifndef DS_CSV_LOADER_H
#define DS_CSV_LOADER_H
#include "station_index.h"

/*
 * @brief Charge les stations depuis un fichier CSV et les insère dans l'index AVL.
 * @details Lit le fichier ligne par ligne, ignore l'en-tête et parse les données utiles.
 * @param  Chemin vers le fichier CSV (ex: "izivia_tp_subset.csv")[cite: 93].
 * @param   Pointeur vers l'index AVL où stocker les stations.
 * @return     Le nombre de stations insérées, ou -1 si le fichier est inaccessible.
 * * Complexité Temps : O(N * log S) - Où N est le nombre de lignes du CSV et S le nombre
 * de stations dans l'AVL. Chaque insertion est en O(log S).
 * Complexité Espace : O(L + log S) - L est la taille du buffer de ligne, log S est la pile
 * de récursion de l'insertion AVL.
 */
int ds_load_stations_from_csv(const char* path, StationIndex* idx);

#endif
