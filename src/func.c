#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <strings.h>
#include <assert.h>
#include <unistd.h>

#include <dlfcn.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "lisp.h"
#include "betterstring.h"

int isnumber(Data* node){
    return node->type==NUMBER;
}

Data* numberp(Data* node,Data* var_list){
    return node->elt->type==NUMBER ? T:nil;    
}

Data* type_of(Data* data,Data* env){
    char* str = NULL;

    switch(data->elt->type){
        case STRING:
            str = "STRING";
            break;
        case NUMBER:
            str = "NUMBER";
            break;
        case QUOTE:
            str = "QUOTE";
            break;
        case LAMBDA:
            str = "LAMBDA";
            break;
        case BLOB:
            str = "BLOB";
            break;
        case SYMBOL:
            str = "SYMBOL";
            break;
        case ARRAY:
            str = "ARRAY";
            break;
        case NODE:
            str = "NODE";
            break;
        case VAR:
            str = "VAR";
            break;
        case MACRO:
            str = "MACRO";
            break;
        case PRIMITIVE:
            str = "PRIMITIVE";
            break;
    }

    return make_string(str);
}

Data* setf(Data* arg,Data* env){

    if(list_length(arg)!=2 || arg==nil){
        fprintf(stderr,"cannot make environment variable");
        return nil;
    }

    Data* var = arg->elt;

    if(var->type==NUMBER || var->type==STRING) throw_error(ERR_TYPE,"lvalue not setfable");

    if(var->type!=SYMBOL) var = eval(var,env);

    Data* e = eval(arg->next->elt,env);
    if(var->type==SYMBOL){
        Data* s = search_symbol(env,arg->elt->symbol);
        if(s) memcpy(s,e,sizeof(Data));
        else insert_node(&env,make_variable(arg->elt->symbol,e));
    }
    else{
        memcpy(var,e,sizeof(Data));
    }

    return e;
}

Data* lambda_(Data* list,Data* env){

    assure(list_length(list)>=2,ERR_LEN_MISMATCH);

    Data* args=list->elt;
    Data* body=list->next,*aux=body;

    while(aux && aux!=nil){
        aux->elt = try_eval(aux->elt,env);
        aux=aux->next;
    }
    return make_lambda(args,body);
}

Data* defun(Data* list,Data* env){

    Data* name=list->elt;
    Data* s = search_symbol(env,name->symbol);
    if(s) memcpy(s,lambda_(list->next,env),sizeof(Data));
    else insert_node(&env,make_variable(name->symbol,lambda_(list->next,env)));
    return name;
}

Data* car_(Data* list,Data* env __attribute__ ((unused))){
    return list->elt->elt;
}

Data* cdr_(Data* list,Data* env __attribute__ ((unused))){
    return list->elt->next;
}

Data* cons(Data* list,Data* env __attribute((unused))){
    assure(list_length(list)==2,ERR_LEN_MISMATCH);

    list->next = list->next->elt;

    return list;
}


Data* nullp(Data* list,Data* env __attribute__((unused))){
    assure(list_length(list)==1,ERR_LEN_MISMATCH);
    return list->elt==nil ? T:nil;
}


Data* print(Data* list,Data* env __attribute((unused))){
    list_to_string(list->elt);
    putchar('\n');
    return list->elt;
}


Data* eval_(Data* node,Data* var_list){
    return eval(node->elt,var_list);
}


Data* mult(Data* args,Data* var_list __attribute__ ((unused))){
    int holder=1;

    for(;args && args!=nil;args=args->next){
        holder*=args->elt->value;
    }
    return make_number(holder);
}

Data* resto(Data* arg,Data* env __attribute__ ((unused))){

    assure(list_length(arg)==2,ERR_LEN_MISMATCH);

    Data* e=arg->elt,*f=arg->next->elt;

    int a = e->value,b = f->value;

    if(b==0) return nil;

    return make_number(a%b);
}


Data* division(Data* arg,Data* env __attribute__ ((unused))){

    assure(list_length(arg)==2,ERR_LEN_MISMATCH);

    int a = arg->elt->value,b = arg->next->elt->value;

    if(b==0) return nil;

    return make_number(a/b);
}

Data* minus(Data* args,Data* var_list __attribute__((unused))){
    int eax=args->elt->value;

    for(args=args->next;args && args!=nil;args=args->next){
        assert(args->elt->type==NUMBER);
        eax-=args->elt->value;
    }
    return make_number(eax);
}

Data* sum(Data* args,Data* var_list){
    int eax=0;
    int holder=0;
    Data* e=nil;

    for(;args!=nil;args=args->next){
        if(isnumber(args->elt)) holder=args->elt->value;
        else if((e=handle_symbol(var_list,args->elt))->type==NUMBER) holder= e->value;
        else continue;
        eax+=holder;
    }
    return make_number(eax);
}

Data* eq(Data* args,Data* env){
    Data* e = compare(args->elt,args->next->elt,env);
    return e;
}

Data* lower(Data* args,Data* env __attribute__ ((unused))){
    if(!(args->elt->type == NUMBER && NUMBER == args->next->elt->type)) throw_error(ERR_TYPE,"both arguments have to be numbers");
    
    return args->elt->value < args->next->elt->value ? T : nil;
}

Data* higher(Data* args,Data* env){
    return lower(args,env) == T ? nil : T;
}

Data* aref(Data* args,Data* env __attribute__ ((unused))){
    Data* i = args->elt,*arr=args->next->elt;

    assure(list_length(args)==2,ERR_LEN_MISMATCH);
    assert(i->type==NUMBER);
    assure(i->type==NUMBER,ERR_TYPE);
    assure(arr->type==ARRAY,ERR_TYPE);

    unsigned int a = (unsigned) i->value;
    if(a>=arr->size){
        fprintf(stderr,"Out-of-bounds index %u, max %u\n",a,arr->size);
        return nil;
    }
    return arr->array[a];
}

Data* array_dimensions(Data* args,Data* env __attribute__ ((unused))){
    assure(args->type==ARRAY,ERR_TYPE);
    assure(list_length(args)==1,ERR_LEN_MISMATCH);
    return make_number(args->elt->size);
}

Data* make_arr(Data* args,Data* env __attribute__ ((unused))){
    return make_array(args->elt);
}



Data* if_(Data* args,Data* env){
    assure(list_length(args)<=3,ERR_LEN_MISMATCH);
       if(eval(args->elt,env)!=nil) return eval(args->next->elt,env);
    if(args->next->next!=nil) return eval(args->next->next->elt,env);
    return nil;
}

Data* or(Data* args,Data* env){
    Data* d = nil;
    while(args && args!=nil){
        d=eval(args->elt,env);
        if(d!=nil) return d;
        args=args->next;
    }
    return nil;
}


Data* and(Data* args,Data* env){
    Data* d = nil;
    while(args && args!=nil){
        d=eval(args->elt,env);
        if(d==nil) return nil;
        args=args->next;
    }
    return d;
}


Data* length_(Data* args,Data* var_list __attribute__ ((unused))){
    assure(args->elt->type==NODE,ERR_TYPE);
    return make_number(list_length(args->elt));
}


Data* quote(Data* list,Data* var_list __attribute__ ((unused))){
    return car(list);
}

Data* read_line(Data* args __attribute__((unused)),Data* env){
    char *str=NULL,*aux=NULL;
    Data* list = NULL;

    while(1){
        str = readline("User>");


       String string = string_upgrade(str);

        string_trim(&string);

        if(string_isempty(string)){
            string_destroy(string);
            continue;
        }

        add_history(string.data);
    
        aux=string.data;
        list=parse_input(&aux);
        string_destroy(string);

        break;
    }

    return eval(list,env);
}

Data* loop(Data* args,Data* env){
    Data* e = nil;
    while(e!=Abort){
        e=progn(args,env);
    }
    return e;
}

Data* sleep_(Data* args,Data* env __attribute__((unused))){
    assure(args->elt->type==NUMBER,ERR_TYPE);
    assure(list_length(args)==2,ERR_LEN_MISMATCH);
    sleep(args->elt->value);

    return nil;
}

Data* load(Data* args,Data* env){
    assure(args && args!=nil,ERR_NOT_FOUND);
    char a[20];

    while(args && args!= nil){
        assure(args->elt->type==SYMBOL,ERR_TYPE);
        strcpy(a,args->elt->symbol);
        load_file(a,env);
        args=args->next;
    }

    return make_symbol(a);
}


Data* dll_open(Data* args,Data* env){
    //return type_of(args->elt,env);
    assure(args && args!=nil,ERR_NOT_FOUND);

    assure( args->elt->type == STRING,ERR_TYPE);

    char* handle_name = args->elt->str;

    void* handle = dlopen(handle_name,RTLD_LAZY);

    assert(handle);


    return handle ? make_blob(handle) : nil;
}

Data* dll_load(Data* args,Data* env){
    assure(args && args!=nil,ERR_NOT_FOUND);
    assure( args->elt->type == BLOB,ERR_TYPE);
    assure( args->next->elt->type == STRING,ERR_TYPE);

    void* handle = args->elt->blob;
    args = args->next;
    char* foo_name = args->elt->str;

    void* foo = dlsym(handle,foo_name);


    return foo ?  make_blob(foo) : nil;
}

Data* dll_apply(Data* args,Data* env){
    Primitive foo = args->elt->blob;

    args = args->next;

    return foo(args,env);
}

Data* dll_close(Data* args,Data* env){
    assure(args && args!=nil,ERR_NOT_FOUND);
    assure( args->elt->type == BLOB,ERR_TYPE);

    return dlclose(args->elt->blob) == 0 ? T : nil;
}

Data* load_symbols(){
    Data *list=nil;

    insert_node(&list,make_variable("T",T));
    insert_node(&list,make_variable("NIL",nil));
    insert_node(&list,make_variable("ABORT",Abort));
    insert_node(&list,make_variable("+",make_primitive(sum)));
    insert_node(&list,make_variable("-",make_primitive(minus)));
    insert_node(&list,make_variable("*",make_primitive(mult)));
    insert_node(&list,make_variable("/",make_primitive(division)));
    insert_node(&list,make_variable("%%",make_primitive(resto)));
    insert_node(&list,make_variable("eq",make_primitive(eq)));
    insert_node(&list,make_variable("<",make_primitive(lower)));
    insert_node(&list,make_variable(">",make_primitive(higher)));
    insert_node(&list,make_variable("car",make_primitive(car_)));
    insert_node(&list,make_variable("cdr",make_primitive(cdr_)));
    insert_node(&list,make_variable("sleep",make_primitive(sleep_)));
    insert_node(&list,make_variable("load",make_primitive(load)));
    insert_node(&list,make_variable("numberp",make_primitive(numberp)));
    insert_node(&list,make_variable("null",make_primitive(nullp)));
    insert_node(&list,make_variable("a",make_number(3)));
    insert_node(&list,make_variable("progn",make_macro(progn)));
    insert_node(&list,make_variable("cons",make_primitive(cons)));
    insert_node(&list,make_variable("length",make_primitive(length_)));
    insert_node(&list,make_variable("aref",make_primitive(aref)));
    insert_node(&list,make_variable("array-dimensions",make_primitive(array_dimensions)));
    insert_node(&list,make_variable("make-array",make_primitive(make_arr)));
    insert_node(&list,make_variable("eval",make_primitive(eval_)));
    insert_node(&list,make_variable("print",make_primitive(print)));
//these are just primitive functions whose arguments dont get evaluated, not actually macros
    insert_node(&list,make_variable("quote",make_macro(quote)));
    insert_node(&list,make_variable("readline",make_macro(read_line)));
    insert_node(&list,make_variable("setf",make_macro(setf)));
    insert_node(&list,make_variable("defun",make_macro(defun)));
    insert_node(&list,make_variable("lambda",make_macro(lambda_)));
    insert_node(&list,make_variable("if",make_macro(if_)));
    insert_node(&list,make_variable("and",make_macro(and)));
    insert_node(&list,make_variable("or",make_macro(and)));
    insert_node(&list,make_variable("loop",make_macro(loop)));
    insert_node(&list,make_variable("list",make_macro(eval_args)));

    MAKE_VARIABLE("type",make_primitive(type_of));

    MAKE_VARIABLE("dll-open",make_primitive(dll_open));
    MAKE_VARIABLE("dll-load",make_primitive(dll_load));
    MAKE_VARIABLE("dll-apply",make_primitive(dll_apply));
    MAKE_VARIABLE("dll-close",make_primitive(dll_close));

    return list;
}
