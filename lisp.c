#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "int.h"

int list_length(Data* n){
    int i=0;
    while(n!=nil && n){
        assure(n->type==NODE,ERR_TYPE);
        i++;
    
        n=n->next;
    }
    return i;
}

Data* new_data(){
    return calloc(1,sizeof(Data));
}

Data* insert_node(Data** list,Data* elt){
    if(!*list || *list==nil){
        *list = new_data();
        (*list)->type=NODE;
        (*list)->elt=elt;
        (*list)->next=nil;
        return *list;
    }

    Data* e=*list;

    while(e->next!=nil && e->next){
        e=e->next;
    }
    e->next=new_data();
    e->next->type=NODE;
    e->next->next=nil;
    e->next->elt=elt;
    return e->next;
}

Data* car(Data* data){
    assure(data->type==NODE,ERR_TYPE);
    return data->elt;
}

Data* compare(Data* a,Data* b,Data* env){
    if(a->type!=b->type) return nil;

    switch(a->type){
        case NODE:
            if(list_length(a)!=list_length(b)) return nil;
            while(a!=nil && a){
                if(compare(a->elt,b->elt,env)==nil) return nil;
                a=a->next;
                b=b->next;
            }
            break;
        case NUMBER: return a->value==b->value ? T:nil;
        case BLOB: return a->blob==b->blob ? T:nil;
        case STRING: return strcmp(a->str,b->str)==0 ? T:nil;
        case SYMBOL: return strcmp(a->symbol,b->symbol)==0 ? T:nil;
        default:return nil;
    }
    return T;
}

Data* copy(Data* list,Data* var_list __attribute__((unused))){
    if(list==nil || list==T) return list;

    Data* e=nil;

    if(list->type==NUMBER) return make_number(list->value);
    else if(list->type==STRING) return make_string(list->str);
    else if(list->type==PRIMITIVE) return make_primitive(list->fn);
    else if(list->type==SYMBOL) return make_symbol(list->symbol);
    else if(list->type==NODE){
        while(list!=nil){
            insert_node(&e,copy(list->elt,var_list));
            list=list->next;
        }
    }
    else{
        fprintf(stderr,"Cant copy -%d-\n",list->type);
        exit(1);
    }
    return e;
}

void delete_list(Data** list){
    if(*list==NULL || *list==nil) return;

    Data* e=*list,*next=nil;

    if(e->type == NUMBER || e->type == SYMBOL || e->type == STRING || e->type == PRIMITIVE){
        free(e);
        *list=nil;
        return;
    }

        while(e!=nil && e!=NULL){
            next=e->next;
        if(e->elt->type==NODE)    delete_list(&e->elt);
        else if(e->elt->type==VAR){ delete_list(&e->elt->data);free(e->elt);}

            free(e);
            e=next;
        }
        *list=nil;
}

Data* make_number(int num){
    Data* e = new_data();
    e->type=NUMBER;
    e->value=num;
    return e;
}

Data* make_symbol(char* str){
    Data* e=new_data();
    e->type=SYMBOL;
    e->symbol = strdup(str);
    return e;
}

Data* make_quote(Data* data){
    Data* e = new_data();
    e->type = QUOTE;
    e->quote = data;

    return e;
}

Data* make_array(Data* lst){
    assure(lst->type==NODE,ERR_TYPE);
    Data* arr = new_data();
    arr->type=ARRAY;

    arr->size = list_length(lst);
    arr->array = calloc(arr->size,sizeof(Data*));

    for(unsigned int i=0;i<arr->size;i++){
        arr->array[i]=lst->elt;
        lst=lst->next;
    }
    return arr;
}

Data* make_primitive(Primitive fn){
    Data* e = new_data();
    e->type=PRIMITIVE;
    e->fn=fn;
    return e;
}

Data* make_lambda(Data* args,Data* body){
    Data* e = new_data();
    e->type=LAMBDA;
    insert_node(&e->elt,args);
    insert_node(&e->elt,body);
    return e;
}

Data* make_macro(Macro mac){
    Data* e = new_data();
    e->type=MACRO;
    e->mac = mac;
    return e;
}

Data* make_variable(char* str,Data* data){
    Data* e=new_data();
    e->type=VAR;
    e->name = strdup(str);
    e->data=data;
    return e;
}

Data* make_string(char* str){
        Data* e=new_data();
        e->type=STRING;
        e->str=strdup(str);
    return e;
}

Data* make_blob(void* stuff){
        Data* e=new_data();
        e->type=BLOB;
        e->blob=stuff;
    return e;
}

/*void* lisp_data_to_param(Data* data){
    switch(data->type){
        case STRING:
            return data->str;
        case SYMBOL:
            return data->symbol;
        case BLOB:
            return data->blob;
        case NUMBER:
            return data->value;
        case ARRAY:
        {
            void** array = malloc(sizeof(void*) * data->size);

            for(int i = 0;i < data->size; i++){
                array[i] = lisp_data_to_param(data->array[i]);
            }

            return array;
        }
        case PRIMITIVE:
            return data->fn;
        case MACRO:
            return data->mac;
        default:
            fprintf(stderr,"oops cant convert type to native");
    }
}*/