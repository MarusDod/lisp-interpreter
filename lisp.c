#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "int.h"

int list_length(Data* n){
	int i=0;
	while(n!=nil && n){
		assure(n->type==node,ERR_TYPE);
		i++;
	
		n=n->next;
    }
	return i;
}

void* alloc(size_t x){
        Data* q=malloc(x);
		Data* e=NULL;
		e=malloc(sizeof(Data));
        assure(e,ERR_MEMORY);
		e->type=node;
		e->elt=q;
		e->next=clean_list;
		clean_list=e;
        return q;
}

Data* new_data(){
    Data* list = alloc(sizeof(Data));
    list->next=nil;

    return list;
}

Data* insert_node(Data** list,Data* elt){
	if(!*list || *list==nil){
		*list = new_data();
		(*list)->type=node;
		(*list)->elt=elt;
		return *list;
	}

	Data* e=*list;

	while(e->next!=nil && e->next){
		e=e->next;
	}
	e->next=new_data();
	e->next->type=node;
	e->next->next=nil;
	e->next->elt=elt;
	return e->next;
}

Data* car(Data* data){
    assure(data->type==node,ERR_TYPE);
	return data->elt;
}

Data* compare(Data* a,Data* b,Data* env){
	if(a->type!=b->type) return nil;

	switch(a->type){
		case node:
			if(list_length(a)!=list_length(b)) return nil;
			while(a!=nil && a){
				if(compare(a->elt,b->elt,env)==nil) return nil;
				a=a->next;
				b=b->next;
			}
			break;
		case number: return a->value==b->value ? T:nil;
		case string: return strcmp(a->str,b->str)==0 ? T:nil;
		case symbol: return strcmp(a->symbol,b->symbol)==0 ? T:nil;
		default:return nil;
	}
	return T;
}

Data* copy(Data* list,Data* var_list __attribute__((unused))){
	if(list==nil || list==T) return list;

	Data* e=nil;

	if(list->type==number) return make_number(list->value);
	else if(list->type==string) return make_string(list->str);
	else if(list->type==primitive) return make_primitive(list->fn);
	else if(list->type==symbol) return make_symbol(list->symbol);
	else if(list->type==node){
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

	if(e->type == number || e->type == symbol || e->type == string || e->type == primitive){
		free(e);
		*list=nil;
		return;
	}

    	while(e!=nil && e!=NULL){
        	next=e->next;
		if(e->elt->type==node)	delete_list(&e->elt);
		else if(e->elt->type==var){ delete_list(&e->elt->data);free(e->elt);}

        	free(e);
        	e=next;
    	}
    	*list=nil;
}

void collect(){
	Data* e=clean_list,*next=NULL;
	while(e){
		next=e->next;
		free(e->elt);
		free(e);
		e=next;
	}
}

Data* make_number(int num){
	Data* e = new_data();
	e->type=number;
	e->value=num;
	return e;
}

Data* make_symbol(char* str){
	Data* e=new_data();
	e->type=symbol;
    memset(e->symbol,0,STRING_MAX);
	strcpy(e->symbol,str);
	return e;
}

Data* make_array(Data* lst){
    assure(lst->type==node,ERR_TYPE);
    Data* arr = new_data();
    arr->type=array;

    arr->size = list_length(lst);
    arr->array = alloc(arr->size*sizeof(Data*));
    for(unsigned int i=0;i<arr->size;i++){
        arr->array[i]=lst->elt;
        lst=lst->next;
    }
    return arr;
}

Data* make_primitive(Primitive fn){
	Data* e = new_data();
	e->type=primitive;
	e->fn=fn;
	return e;
}

Data* make_lambda(Data* args,Data* body){
	Data* e = new_data();
	e->type=lambda;
	insert_node(&e->elt,args);
	insert_node(&e->elt,body);
	return e;
}

Data* make_macro(Macro mac){
	Data* e = new_data();
	e->type=macro;
    e->mac = mac;
	return e;
}

Data* make_variable(char* str,Data* data){
	Data* e=new_data();
	e->type=var;
    memset(e->name,0,STRING_MAX);
	strcpy(e->name,str);
	e->data=data;
	return e;
}

Data* make_string(char* str){
        Data* e=new_data();
        e->type=string;
        memset(e->str,0,STRING_MAX);
        strcpy(e->str,str);
	return e;
}
