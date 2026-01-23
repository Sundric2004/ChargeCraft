Scénario Applicatif : B1 - Heure de Pointe :

Ce scénario simule une affluence massive et soudaine de véhicules sur le réseau de bornes de recharge, typique des créneaux horaires 8h-9h ou 18h-19h.

Le déroulement est le suivant :

1) État Initial : Analyse du réseau au calme (Top-K des meilleures stations).
2) La Rafale (Burst) : Injection massive d'événements (connexions/déconnexions) dans la file d'attente (Queue) sur un temps très court.
3) Traitement : Consommation rapide de ces événements et mise à jour dynamique de l'index AVL.
4) État Final : Analyse post-crise pour identifier les stations saturées et celles qui restent disponibles.

Nous avons privilégié ce scénario pour trois raisons principales qui valorisent les aspects techniques du projet :
1) Le scénario "Heure de Pointe" est le meilleur moyen de justifier l'utilisation d'un Arbre AVL plutôt qu'une liste chaînée. Lors de la rafale, nous effectuons des centaines de recherches (si_find) et de mises à jour (si_add) en quelques millisecondes. La complexité en O(log n) de l'AVL garantit que le simulateur ne ralentit pas, même si le nombre de stations augmente considérablement.
2) Ce scénario est celui qui exploite le mieux les modules développés : Il permet de comparer "l'avant/après". Avant la crise, les stations les mieux notées sont celles avec le plus de places. Après la crise, le classement change drastiquement car les places libres (slots_free) chutent. / Il permet d'observer l'impact localisé. Nous l'utilisons pour voir si une zone géographique précise a été totalement saturée par la demande.
3) Contrairement à une simple baisse de prix (Scénario B3), l'heure de pointe met en jeu la dynamique temporelle du système. Elle force l'interaction entre : La Queue (qui absorbe le choc des arrivées). / Le MRU (qui mémorise les passages fréquents des véhicules). / L'Index (qui doit rester cohérent en temps réel).


