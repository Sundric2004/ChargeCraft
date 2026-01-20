#include "advanced_queries.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * ============================================================================
 * MODULE A1 : RANGE QUERIES SUR AVL
 * ============================================================================
 */

/*
 * Fonction auxiliaire récursive pour si_range_ids
 * Parcours in-order avec élagage intelligent des branches
 */
static int range_rec(StationNode* r, int lo, int hi, int* out, int cap, int idx) {
    if (!r || idx >= cap) return idx;

    // Si le nœud courant est > lo, explorer le sous-arbre gauche
    if (r->station_id > lo) {
        idx = range_rec(r->left, lo, hi, out, cap, idx);
    }

    // Si le nœud courant est dans [lo, hi], l'ajouter
    if (r->station_id >= lo && r->station_id <= hi && idx < cap) {
        out[idx++] = r->station_id;
    }

    // Si le nœud courant est < hi, explorer le sous-arbre droit
    if (r->station_id < hi) {
        idx = range_rec(r->right, lo, hi, out, cap, idx);
    }

    return idx;
}

int si_range_ids(StationNode* r, int lo, int hi, int* out, int cap) {
    if (!r || !out || cap <= 0) return 0;
    return range_rec(r, lo, hi, out, cap, 0);
}

/*
 * Fonction auxiliaire récursive pour si_count_ge_power
 * Compte le nombre de stations avec power_kW >= P
 */
static int count_power_rec(StationNode* r, int P) {
    if (!r) return 0;

    int count = 0;

    // Compter récursivement dans les sous-arbres
    count += count_power_rec(r->left, P);
    count += count_power_rec(r->right, P);

    // Ajouter 1 si ce nœud satisfait la condition
    if (r->info.power_kW >= P) {
        count++;
    }

    return count;
}

int si_count_ge_power(StationNode* r, int P) {
    return count_power_rec(r, P);
}

/*
 * ============================================================================
 * MODULE A2 : TOP-K PAR SCORE (MIN-HEAP LOCAL)
 * ============================================================================
 */

/*
 * Structure du min-heap
 */
typedef struct {
    ScoredStation* data;  // Tableau de (ID, score)
    int size;             // Taille actuelle
    int capacity;         // Capacité maximale (k)
} MinHeap;

/*
 * Fonction auxiliaire : initialiser un min-heap
 */
static MinHeap* heap_create(int k) {
    MinHeap* h = (MinHeap*)malloc(sizeof(MinHeap));
    if (!h) return NULL;
    h->data = (ScoredStation*)malloc(sizeof(ScoredStation) * k);
    if (!h->data) { free(h); return NULL; }
    h->size = 0;
    h->capacity = k;
    return h;
}

/*
 * Fonction auxiliaire : libérer un heap
 */
static void heap_destroy(MinHeap* h) {
    if (h) {
        free(h->data);
        free(h);
    }
}

/*
 * Fonction auxiliaire : échanger deux éléments
 */
static void heap_swap(ScoredStation* a, ScoredStation* b) {
    ScoredStation tmp = *a;
    *a = *b;
    *b = tmp;
}

/*
 * Fonction auxiliaire : heapify vers le bas (pour maintenir propriété min-heap)
 */
static void heap_heapify_down(MinHeap* h, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < h->size && h->data[left].score < h->data[smallest].score) {
        smallest = left;
    }
    if (right < h->size && h->data[right].score < h->data[smallest].score) {
        smallest = right;
    }

    if (smallest != i) {
        heap_swap(&h->data[i], &h->data[smallest]);
        heap_heapify_down(h, smallest);
    }
}

/*
 * Fonction auxiliaire : heapify vers le haut
 */
static void heap_heapify_up(MinHeap* h, int i) {
    if (i == 0) return;
    int parent = (i - 1) / 2;
    if (h->data[i].score < h->data[parent].score) {
        heap_swap(&h->data[i], &h->data[parent]);
        heap_heapify_up(h, parent);
    }
}

/*
 * Fonction auxiliaire : insérer dans le heap
 * Si heap plein et nouveau score > min, remplacer le min
 */
static void heap_insert(MinHeap* h, int station_id, int score) {
    // Si le heap n'est pas plein, ajouter directement
    if (h->size < h->capacity) {
        h->data[h->size].station_id = station_id;
        h->data[h->size].score = score;
        heap_heapify_up(h, h->size);
        h->size++;
    }
    // Si le heap est plein mais le nouveau score est meilleur que le min
    else if (score > h->data[0].score) {
        // Remplacer la racine (minimum)
        h->data[0].station_id = station_id;
        h->data[0].score = score;
        heap_heapify_down(h, 0);
    }
}

/*
 * Fonction auxiliaire : calculer le score d'une station
 */
static int calculate_score(StationInfo* info, int alpha, int beta, int gamma) {
    return info->slots_free * alpha + info->power_kW * beta - info->price_cents * gamma;
}

/*
 * Fonction auxiliaire récursive : parcourir l'AVL et maintenir le top-k
 */
static void topk_traverse(StationNode* r, MinHeap* h, int alpha, int beta, int gamma) {
    if (!r) return;

    // Parcours in-order (mais l'ordre n'importe pas ici)
    topk_traverse(r->left, h, alpha, beta, gamma);

    // Calculer le score de la station courante
    int score = calculate_score(&r->info, alpha, beta, gamma);

    // Insérer dans le heap
    heap_insert(h, r->station_id, score);

    topk_traverse(r->right, h, alpha, beta, gamma);
}

/*
 * Fonction auxiliaire : trier le heap par score décroissant pour l'extraction
 */
static int compare_scored_desc(const void* a, const void* b) {
    ScoredStation* sa = (ScoredStation*)a;
    ScoredStation* sb = (ScoredStation*)b;
    return sb->score - sa->score;  // Ordre décroissant
}

int si_top_k_by_score(StationNode* r, int k, int* out_ids,
                      int alpha, int beta, int gamma) {
    if (!r || k <= 0 || !out_ids) return 0;

    // Créer un min-heap de taille k
    MinHeap* h = heap_create(k);
    if (!h) return 0;

    // Parcourir l'arbre et maintenir le top-k
    topk_traverse(r, h, alpha, beta, gamma);

    // Trier les résultats par score décroissant
    qsort(h->data, h->size, sizeof(ScoredStation), compare_scored_desc);

    // Extraire les IDs
    int count = h->size;
    for (int i = 0; i < count; i++) {
        out_ids[i] = h->data[i].station_id;
    }

    heap_destroy(h);
    return count;
}