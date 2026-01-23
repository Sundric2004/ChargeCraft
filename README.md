# EV & Charging Stations — C11 Starter V2 (Data Structures)

This V2 integrates CSV/JSON loaders wired in `Makefile` and `main.c`.
Build & run:
```bash
make
./ev_demo
```
Files:
- events.h/.c — tiny event stream
- slist.h/.c — MRU SList (head-only)
- queue.h/.c — FIFO of Event
- stack.h/.c — stack for postfix rules
- station_index.h/.c — AVL stations index
- nary.h/.c — n-ary tree (skeleton + BFS print)
- rules.c — postfix evaluator (example)
- **csv_loader.h/.c** — load stations from CSV (IRVE-like)
- **json_loader.h/.c** — load stations from JSON (minimal format)
- main.c — demo: load CSV/JSON → ingest events → show AVL/MRU

Rapport Comparatif : BST vs AVL

Sujet : Analyse des performances lors d'insertions défavorables
1. Introduction

Dans le cadre du projet ChargeCraft, le choix de la structure de données pour l'indexation des stations est crucial pour garantir la réactivité du système. Ce rapport compare deux structures d'arbres binaires : l'Arbre Binaire de Recherche classique (BST) et l'Arbre équilibré AVL.
2. Comportement avec des données aléatoires

Dans un scénario idéal où les identifiants de stations sont insérés de manière aléatoire, le BST et l'AVL se comportent de façon similaire.

    Complexité moyenne : Les deux structures offrent une complexité de recherche, d'insertion et de suppression en O(logn).

    Structure : L'arbre se développe de manière naturellement "buissonnante", équilibrant les branches gauche et droite.

3. Le cas critique : Insertions d'identifiants triés

Le problème majeur survient lorsque les données arrivent déjà triées (par exemple, des IDs de stations générés séquentiellement : 101, 102, 103, ...).
L'Arbre Binaire de Recherche (BST)

    Dégénérescence : Sans mécanisme de contrôle, chaque nouvel élément plus grand que le précédent est inséré systématiquement à droite.

    Transformation en liste : L'arbre ne ressemble plus à un arbre mais devient une simple liste chaînée.

    Impact sur la performance : La hauteur de l'arbre devient égale à n (le nombre d'éléments). La complexité de recherche s'effondre de O(logn) à O(n). Pour 100 000 stations, une recherche pourrait nécessiter 100 000 comparaisons au lieu de 17.

L'Arbre AVL

    Auto-équilibrage : L'AVL surveille la différence de hauteur entre les sous-arbres (facteur d'équilibre).

    Rotations : Dès qu'un déséquilibre apparaît lors de l'insertion d'IDs triés, l'AVL effectue des rotations (simples ou doubles) pour réorganiser les nœuds.

    Garantie de performance : Même avec des données triées, la hauteur reste limitée à 1.44×log2​(n). La complexité reste strictement en O(logn).

4. Conclusion pour ChargeCraft

Pour un système de supervision de bornes de recharge, l'AVL est le choix de la rigueur. Bien que l'implémentation de l'AVL soit plus complexe à cause des rotations, elle est indispensable pour éviter que le système ne ralentisse drastiquement si les stations sont enregistrées par blocs d'identifiants consécutifs. L'AVL garantit une fluidité constante des requêtes "Top-N" et des filtrages, quel que soit l'ordre d'arrivée des données.


5. Remarque sur la compilation :

On a créé deux mains, ChargeCraft_V1 et ChargeCraft_V2. la V1 correspond au test des starters (partie 1 du projet), on a alors choisi de créer un autre main pour ne pas avoir un seul main encombré. CMakeLists.txt a la manière de comment on gère ces deux mains, et sur l'IDE on peut basculer sur les deux exécutables facilement.
