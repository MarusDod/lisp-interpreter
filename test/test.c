#include "int.h"

Data* foo(Data* args,Data* var_list){
    int eax=0;
    Data* e=nil;

    for(;args!=nil;args=args->next){
        if(args->elt->type == NUMBER) eax += args->elt->value;
        else{
            return Abort;
        }
    }
    return make_number(eax);
}
