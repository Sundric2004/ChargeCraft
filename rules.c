#include "station_index.h"
#include "stack.h"
#include <string.h>
#include <stdlib.h>

/*
 * Fonction : eval_rule_postfix
 * Description : Évalue une règle de filtrage en notation postfix (polonaise inversée)
 *               Utilise une pile pour évaluer les expressions
 *
 * Notation postfix : les opérateurs suivent les opérandes
 * Exemple : "slots 1 >= power 50 >= &&"
 *   Signifie : (slots >= 1) AND (power >= 50)
 *
 * Tokens supportés :
 *   - Champs : "power", "price", "slots"
 *   - Opérateurs de comparaison : ">=", "<=", ">", "<", "=="
 *   - Opérateurs logiques : "&&" (AND), "||" (OR)
 *   - Nombres : convertis en entiers
 *
 * Paramètres :
 *   - toks : tableau de chaînes de caractères représentant les tokens
 *   - n : nombre de tokens
 *   - info : pointeur vers les informations de la station à évaluer
 *
 * Retour : 1 si la station satisfait la règle, 0 sinon
 *
 * Complexité temps : O(n) où n = nombre de tokens
 *                    Chaque token est traité exactement une fois
 * Complexité espace : O(n) dans le pire cas
 *                     La pile peut contenir jusqu'à n éléments
 *
 * Exemples d'utilisation :
 *   1) Règle : "slots 1 >="
 *      Stack : [slots_value] → [slots_value, 1] → [result]
 *
 *   2) Règle : "power 50 >= price 250 <= &&"
 *      Évalue : (power >= 50) AND (price <= 250)
 */
int eval_rule_postfix(char* toks[], int n, StationInfo* info){
    Stack st;
    st_init(&st);

    // Parcourir chaque token de la règle
    for(int i=0; i<n; i++){
        char* t = toks[i];

        // Champs de la station → empiler leur valeur
        if(strcmp(t,"power")==0)
            st_push(&st, info->power_kW);
        else if(strcmp(t,"price")==0)
            st_push(&st, info->price_cents);
        else if(strcmp(t,"slots")==0)
            st_push(&st, info->slots_free);

        // Opérateurs de comparaison → dépiler 2 valeurs, comparer, empiler résultat
        else if(strcmp(t,">=")==0){
            int b,a;
            st_pop(&st,&b);
            st_pop(&st,&a);
            st_push(&st, a>=b);
        }
        else if(strcmp(t,"<=")==0){
            int b,a;
            st_pop(&st,&b);
            st_pop(&st,&a);
            st_push(&st, a<=b);
        }
        else if(strcmp(t,">")==0){
            int b,a;
            st_pop(&st,&b);
            st_pop(&st,&a);
            st_push(&st, a>b);
        }
        else if(strcmp(t,"<")==0){
            int b,a;
            st_pop(&st,&b);
            st_pop(&st,&a);
            st_push(&st, a<b);
        }
        else if(strcmp(t,"==")==0){
            int b,a;
            st_pop(&st,&b);
            st_pop(&st,&a);
            st_push(&st, a==b);
        }

        // Opérateurs logiques → dépiler 2 booléens, calculer, empiler résultat
        else if(strcmp(t,"&&")==0){
            int b,a;
            st_pop(&st,&b);
            st_pop(&st,&a);
            st_push(&st, a&&b);
        }
        else if(strcmp(t,"||")==0){
            int b,a;
            st_pop(&st,&b);
            st_pop(&st,&a);
            st_push(&st, a||b);
        }

        // Nombre littéral → empiler sa valeur
        else {
            st_push(&st, atoi(t));
        }
    }

    // Le résultat final est au sommet de la pile
    int ok=0;
    st_pop(&st,&ok);
    st_clear(&st);
    return ok!=0;
}