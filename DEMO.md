#  Démonstration des Modules Avancés — Code Source & Résultats

Ce document présente la **fonction de démonstration** utilisée pour valider les modules **A1, A2 et A4** du projet.  
Les tests sont exécutés de manière **standalone** afin de vérifier le bon fonctionnement des structures et algorithmes implémentés.

---

##  Code source de la démonstration

La fonction suivante charge les données, exécute les tests des modules avancés et affiche les résultats en console.

```c
void demo_modules_standalone() {
    print_separator("DEMONSTRATION DES MODULES AVANCES (INDIVIDUELS)");

    StationIndex idx;
    si_init(&idx); // Initialisation de l'index AVL

    // ---------- ETAPE 0 : CHARGEMENT ----------
    printf("\n>>> ETAPE 0 : Chargement des donnees initiales\n");
    int loaded = ds_load_stations_from_csv("izivia_tp_subset.csv", &idx);
    if (loaded <= 0) {
        printf("[ERREUR] Impossible de charger izivia_tp_subset.csv\n");
        return;
    }
    printf("    -> %d stations chargees avec succes dans l'AVL.\n", loaded);
    wait_user();

    // ---------- MODULE A1 : RANGE QUERIES ----------
    printf("\n>>> MODULE A1 : Requettes par plages (Range Queries)\n");
    printf("    Objectif : Extraire des donnees sans parcourir toute la base.\n\n");

    int range_ids[100];
    int count = si_range_ids(idx.root, 1100, 1150, range_ids, 100);
    printf("  Test 1 : Stations dans la zone ID [1100, 1150]\n");
    printf("    -> %d stations trouvees.\n", count);
    printf("    Premiers IDs : ");
    for (int i = 0; i < count && i < 10; i++) {
        printf("%d ", range_ids[i]);
    }
    printf("%s\n\n", count > 10 ? "..." : "");
    wait_user();

    int high_power = si_count_ge_power(idx.root, 100);
    printf("  Test 2 : Stations Haute Puissance (>= 100 kW)\n");
    printf("    -> %d stations identifiees.\n", high_power);

    printf("  Test 3 : Stations avec puissance >= 50 kW\n");
    int medium_power_count = si_count_ge_power(idx.root, 50);
    printf("    Resultat : %d stations\n\n", medium_power_count);
    wait_user();

    // ---------- MODULE A2 : TOP-K ----------
    printf("\n>>> MODULE A2 : Classement Top-K par Score Dynamique\n");
    printf("    Objectif : Proposer les meilleures stations selon des criteres mixtes.\n\n");

    int top5[5];
    int k = si_top_k_by_score(idx.root, 5, top5, 2, 1, 1);

    for (int i = 0; i < k; i++) {
        StationNode* s = si_find(idx.root, top5[i]);
        if (s) {
            int score = s->info.slots_free * 2
                      + s->info.power_kW
                      - s->info.price_cents;
            printf("    #%d - Station %d (score=%d, slots=%d, power=%dkW, price=%dc)\n",
                   i + 1, s->station_id, score,
                   s->info.slots_free,
                   s->info.power_kW,
                   s->info.price_cents);
        }
    }
    printf("\n");
    wait_user();

    // ---------- MODULE A4 : MRU ----------
    printf(">>> MODULE A4 : MRU capee sans doublons\n\n");

    SList mru;
    ds_slist_init(&mru);
    int MRU_CAP = 5;

    int test_stations[] = {101, 102, 103, 104, 105, 106, 107};
    for (int i = 0; i < 7; i++) {
        mru_add_station(&mru, test_stations[i], MRU_CAP);
        printf("    Apres ajout %d : ", test_stations[i]);
        ds_slist_print(&mru);
    }

    printf("\n  Test : Ajout d'une station deja presente (102)\n");
    mru_add_station(&mru, 102, MRU_CAP);
    printf("    Resultat : ");
    ds_slist_print(&mru);
    printf("    -> 102 remonte en tete (sans doublon)\n\n");

    ds_slist_clear(&mru);
    si_clear(&idx);
    wait_user();
}

````
---

## Sortie console attendue

Le rendu suivant correspond à l’exécution complète des tests :

```

################################################################################
  DEMONSTRATION DES MODULES AVANCES (INDIVIDUELS)
################################################################################

>>> ETAPE 0 : Chargement des donnees initiales
    -> 300 stations chargees avec succes dans l'AVL.

[Appuyez sur ENTREE pour l'etape suivante...]


>>> MODULE A1 : Requettes par plages (Range Queries)
    Objectif : Extraire des donnees sans parcourir toute la base.

  Test 1 : Stations dans la zone ID [1100, 1150]
    -> 51 stations trouves.
    Premieres IDs : 1100 1101 1102 1103 1104 1105 1106 1107 1108 1109 ...


[Appuyez sur ENTREE pour l'etape suivante...]

  Test 2 : Stations Haute Puissance (>= 100 kW)
    -> 103 stations identifiees.
  Test 3 : Comptage des stations avec puissance >= 50 kW
    Resultat : 202 stations


[Appuyez sur ENTREE pour l'etape suivante...]


>>> MODULE A2 : Classement Top-K par Score Dynamique
    Objectif : Proposer les 'N' meilleures bornes selon des criteres mixtes.

  Test 1 : Top-5 (score = 2*slots + 1*power - 1*price)
    #1 - Station 1071 (score=-134, slots=8, power=150kW, price=300c)
    #2 - Station 1021 (score=-134, slots=8, power=150kW, price=300c)
    #3 - Station 1019 (score=-134, slots=8, power=150kW, price=300c)
    #4 - Station 1044 (score=-134, slots=8, power=150kW, price=300c)
    #5 - Station 1104 (score=-134, slots=8, power=150kW, price=300c)

  Test 2 : Top-3 (score = 10*slots + 1*power - 1*price)
           (Favorise les stations les plus disponibles)
    #1 - Station 1019 (score=-70, slots=8)
    #2 - Station 1021 (score=-70, slots=8)
    #3 - Station 1044 (score=-70, slots=8)


[Appuyez sur ENTREE pour l'etape suivante...]

 MODULE A4 : MRU capee sans doublons

  Test : Ajout de 7 stations dans un MRU de capacite 5
    Apres ajout 101 : [101]
    Apres ajout 102 : [102 -> 101]
    Apres ajout 103 : [103 -> 102 -> 101]
    Apres ajout 104 : [104 -> 103 -> 102 -> 101]
    Apres ajout 105 : [105 -> 104 -> 103 -> 102 -> 101]
    Apres ajout 106 : [106 -> 105 -> 104 -> 103 -> 102]
    Apres ajout 107 : [107 -> 106 -> 105 -> 104 -> 103]

  Test : Ajout d'une station deja presente (102)
    Resultat : [102 -> 107 -> 106 -> 105 -> 104]
    102 remonte en tete (pas de doublon)

  Test : Verification de presence
    Station 102 dans MRU ? OUI
    Station 101 dans MRU ? NON
    Station 999 dans MRU ? NON


[Appuyez sur ENTREE pour l'etape suivante...]
