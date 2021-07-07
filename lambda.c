#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "int.h"

Data* handle_symbol(Data* var_list,Data* s){

    fflush(stdout);
    if(s->type!=SYMBOL) throw_error(ERR_NOT_FOUND,"type: %d",s->type);
    char* string = s->symbol;

    Data* e = search_symbol(var_list,string);
    
    if(!e) throw_error(ERR_NOT_FOUND,"Couldn't find symbol %s",s->symbol);

    return e;
}

Data* search_symbol_lexically(Data* var_list,char* string){

    Data* e=NULL;

    if(var_list->elt->type==NODE){
            e=search_symbol(var_list->elt,string);
            return e;
    }
    if(var_list->elt->data==T) return NULL;

    while(var_list!=nil && var_list){
        if(strcasecmp(var_list->elt->name,string)==0){
            return var_list->elt->data;
        }
        var_list=var_list->next;
    }
    return NULL;
}
Data* search_symbol(Data* var_list,char* string){

    Data* e=NULL;

    while(var_list!=nil && var_list){
        if(var_list->elt->type==NODE){
            if((e=search_symbol(var_list->elt,string))!=NULL)    return e; 
        }
        else if(strcasecmp(var_list->elt->name,string)==0){
            return var_list->elt->data;
        }
        var_list=var_list->next;
    }
    return NULL;
}

Data* try_eval(Data* data,Data* env){
    Data* e=data,*b=data;


    switch(data->type){
        case SYMBOL:
            if((e=search_symbol_lexically(env,data->symbol))) return e;
            else return data;
        case NODE:
            while(data && data!= nil){
                data->elt=try_eval(data->elt,env);
                data=data->next;
            }
            return b;
        case ARRAY:
            for(unsigned int i = 0;i < data->size;i++){
                data->array[i] = try_eval(data->array[i],env);
            }
            return data;
        default:break; 
    }
    return data;
}

Data* eval_lambda(Data* data,Data* list,Data* env){

    Data* args=data->elt,*body=data->next,*local=nil,*var_list=nil;

    assure(list_length(args)==list_length(list),ERR_LEN_MISMATCH);

    while(args!=nil && args){
        insert_node(&local,make_variable(args->elt->symbol,list->elt));
        args=args->next;
        list=list->next;
    }

    env = env->elt->type==NODE ? env->next->elt:env;

    insert_node(&var_list,local);
    insert_node(&var_list,env);

    Data* e=progn(body->elt,var_list);

    return e;
}

Data* eval_args(Data* list,Data* var_list){
    if(list==nil) return list;
    assure(list->type==NODE,ERR_TYPE);

    Data* acons = nil;
    while(list && list!=nil){
        insert_node(&acons,eval(list->elt,var_list));
        list=list->next;
    }
        
    return acons;
}

Data* eval(Data* list,Data* var_list){

    if(list==T || list==nil) return list;

    switch(list->type){
    
    case NUMBER:    return copy(list,var_list);
    case SYMBOL:    return handle_symbol(var_list,list);
    case STRING:    
    case BLOB:    
    case LAMBDA:
    case PRIMITIVE:    
    case MACRO:
                    return list;
    case NODE:;
        Data* e=handle_symbol(var_list,list->elt);

        if(e==nil || (e->type!=PRIMITIVE && e->type!=LAMBDA && e->type!=MACRO)) return nil;

        if (e->type==MACRO) return e->mac(list->next,var_list);
        if(e->type==PRIMITIVE) return e->fn(eval_args(list->next,var_list),var_list);
        if(e->type==LAMBDA) return eval_lambda(e->elt,eval_args(list->next,var_list),var_list);
        break;
    case ARRAY:;
                for(unsigned int i=0;i<list->size;i++){
                    list->array[i] = eval(list->array[i],var_list);
                }
               return list;
    default:
        fprintf(stderr,"eval nao implementado");
        break;
    }
    return nil;                   
}

void load_file(char* name,Data* env){

    FILE* fptr = fopen(name,"r");
    if(!fptr)    throw_error(ERR_NOT_FOUND,"File \"%s\" does not exist\n",name);

    struct stat st;
    stat(name,&st);

    char* buf  = malloc(st.st_size);
    char* aux __attribute__((cleanup (d_free))) = buf;
    assure(buf,ERR_MEMORY);

    memset(buf,0,st.st_size);
    fread(buf,1,st.st_size,fptr);
    fclose(fptr);

    printf("Loaded %s successfully\n",name);

    Data* list = parse_input(&buf);
    eval_args(list,env);
}

Data* progn(Data* args,Data* env){
    assure(args->type==NODE,ERR_TYPE);

    Data* e = eval_args(args,env);
    
    while(e->next!=nil && e->next && e){
        e=e->next;
    }
    return car(e);
}




