Choix des Fonctionnalités Avancées :

Nous avons implémenté 3 modules avancés afin de couvrir différents aspects de l'algorithmique : la recherche, le tri optimisé et la gestion de structure de données bornée.

1) Module A1 (Range Queries sur AVL) :
Ce module est essentiel pour les requêtes géographiques (si les IDs sont géolocalisés) ou pour la pagination. Il permet d'isoler un sous-ensemble de données très rapidement.
Lors de la descente récursive dans l'AVL : Si le nœud actuel est < min, on ne visite pas le sous-arbre gauche (inutile). / Si le nœud actuel est > max, on ne visite pas le sous-arbre droit.
- Complexité : O(k + \log n) où k est le nombre d'éléments trouvés. C'est bien plus efficace qu'un O(n) complet.

2) Module A2 : Top-K par Score (Min-Heap Local) :
Dans un système réel, l'utilisateur ne veut pas voir toutes les stations, ni les voir triées arbitrairement par ID. Il veut les K meilleures selon ses critères (prix, puissance, disponibilité).
Trier l'intégralité de l'AVL ($O(n \log n)$) serait du gaspillage pour n'afficher que 5 résultats.
Nous avons utilisé un Tas Min (Min-Heap) de taille fixe $K$ pour filtrer les stations "à la volée" lors du parcours.
- Algorithme : On parcourt l'AVL. Pour chaque station, on compare son score au minimum actuel du Top-K (la racine du Heap). Si le score est meilleur, on remplace la racine et on réorganise le tas.
- Complexité : O(n \log k). C'est optimal car k est négligeable devant n.

3) Module A4 : MRU Capée (Sans Doublons) :
La gestion simple de l'historique (Partie 1) avait deux défauts majeurs pour un usage réel :
- Elle autorisait les doublons (voir "Station 101" trois fois de suite est inutile).
- Elle pouvait croître indéfiniment (fuite mémoire potentielle) sans une limite stricte.
- Nous avons encapsulé la liste chaînée (SList) dans une logique de gestion stricte :
- Unicité : Suppression préventive de l'ID s'il existe déjà (remove_value).
- Fraîcheur : Insertion systématique en tête (insert_head).
- Capacité : Vérification de la taille et suppression de la queue (remove_tail) si dépassement.
- Complexité : $O(L)$ où $L$ est la capacité (ex: 5). Comme $L$ est une petite constante, cela revient à du $O(1)$ en pratique.
