#include "station_index.h"
#include "stack.h"
#include <string.h>
#include <stdlib.h>

int eval_rule_postfix(char* toks[], int n, StationInfo* info){
    Stack st; st_init(&st);
    for(int i=0;i<n;i++){
        char* t=toks[i];
        if(strcmp(t,"power")==0)      st_push(&st, info->power_kW);
        else if(strcmp(t,"price")==0) st_push(&st, info->price_cents);
        else if(strcmp(t,"slots")==0) st_push(&st, info->slots_free);
        else if(strcmp(t,">=")==0){ int b,a; st_pop(&st,&b); st_pop(&st,&a); st_push(&st, a>=b); }
        else if(strcmp(t,"<=")==0){ int b,a; st_pop(&st,&b); st_pop(&st,&a); st_push(&st, a<=b); }
        else if(strcmp(t,">")==0){ int b,a; st_pop(&st,&b); st_pop(&st,&a); st_push(&st, a>b); }
        else if(strcmp(t,"<")==0){ int b,a; st_pop(&st,&b); st_pop(&st,&a); st_push(&st, a<b); }
        else if(strcmp(t,"==")==0){ int b,a; st_pop(&st,&b); st_pop(&st,&a); st_push(&st, a==b); }
        else if(strcmp(t,"&&")==0){ int b,a; st_pop(&st,&b); st_pop(&st,&a); st_push(&st, a&&b); }
        else if(strcmp(t,"||")==0){ int b,a; st_pop(&st,&b); st_pop(&st,&a); st_push(&st, a||b); }
        else { st_push(&st, atoi(t)); }
    }
    int ok=0; st_pop(&st,&ok); st_clear(&st); return ok!=0;
}
