#include "int.h"

Data* sum(Data* args,Data* var_list){
    int eax=0;
    int holder=0;
    Data* e=nil;

    for(;args!=nil;args=args->next){
        if(args->elt->type == NUMBER) holder=args->elt->value;
        else if((e=handle_symbol(var_list,args->elt))->type==NUMBER) holder= e->value;
        else continue;
        eax+=holder;
    }
    return make_number(eax);
}
