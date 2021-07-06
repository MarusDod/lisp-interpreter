#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <assert.h>
#include "int.h"
#include <ctype.h>

void skip_comment(char** text){
	(*text)++;
	while(**text!='\n' && **text && **text!=EOF) (*text)++;
	if(**text=='\n') (*text)++;
}

Data* parse_input(char** input){
	Data* list=nil;
	Data* e=nil;
	int a;
	char ptr[30];

	while(**input){
		if(**input!=32 && **input!='\n'){
   		if(**input==';') skip_comment(input);
            if(sscanf(*input," '%[a-zA-Z()0-9 #\"\'.-]s ",ptr)==1){
               (*input)++; 
               e=nil;
               if(sscanf(*input,"(%[a-zA->0-9 #\"\'-.]s) ",ptr)){
                (*input)++;
				insert_node(&e,make_symbol("quote"));
				insert_node(&e,parse_input(input));
				insert_node(&list,e);
               }
               else if(sscanf(*input,"%20[-a-zA-Z.^_#]s ",ptr)==1){
                (*input)+=strlen(ptr);
				insert_node(&e,make_symbol("quote"));
                insert_node(&e,make_symbol(ptr));
                insert_node(&list,e);
               }
            }
			else if(**input=='('){
                (*input)++;
                insert_node(&list,parse_input(input));
			}
            else if(sscanf(*input,"#(%[A-Za-z <>0-9.-]s) ",ptr)==1){
                (*input)+=2;
                e = parse_input(input);
                insert_node(&list,make_array(e));
            }
			else if(sscanf(*input," \"%[A-Za-z <>\'0-9.-]s\" ",ptr)==1){
				(*input)+=strlen(ptr)+2;
				insert_node(&list,make_string(ptr));
			}
			else if(sscanf(*input,"%u",&a)==1){
				insert_node(&list,make_number(a));	
				sprintf(ptr,"%d",a);
				(*input)+=strlen(ptr);
			}
			else if(**input==')'){
				(*input)++;
				return list;
			}
			else if(sscanf(*input," %[a-zA-Z+-*/%<>.]s ",ptr)==1){
				if(strcasecmp("NIL",ptr)==0) insert_node(&list,nil);
				else if(strcasecmp("T",ptr)==0) insert_node(&list,T);
				else insert_node(&list,make_symbol(ptr));
				(*input)+=strlen(ptr);
			}
		}
		else (*input)++;
	}
	return list;
}

void list_to_string(Data* list){

	switch(list->type){
	case symbol:	printf("%s",list->symbol);return;
	case string:	printf("%s",list->str);return;
	case number:	printf("%d",list->value);return;
	case var:	
                 printf(" %s:",list->name);
                 list_to_string(list->data);
                 return;
     case array: printf("#(");
                 for(unsigned int i=0;i<list->size;i++){
                    list_to_string(list->array[i]);
                    if(i<list->size-1) printf(" ");
                 }
                    printf(")");
                return;
	case primitive:	printf("primitive");return;
    case lambda:	printf("lambda");return;
    case macro: printf("macro");return;
	default:break;
	}

    assert(list->type==node);
	putchar('(');
	while(list!=nil){
		list_to_string(list->elt);
		if(list->next!=nil && list->next->type==node) putchar(' ');
		list=list->next;
	}

	putchar(')');
	fflush(stdout);
}

Data* track_list(Data* list){

	if(list==T) {puts("global:T");return list;}
	else if(list==nil) {puts("global:NIL");return list;}

	switch(list->type){
        case symbol:	printf("symbol:%s",list->symbol);return list;
        case string:	printf("string:\"%s\"",list->str);return list;
        case number:	printf("integer:%d",list->value);return list;
        case array:     printf("array: [");
                        for(unsigned int i=0;i<list->size;i++){
                            track_list(list->array[i]);
                        }
                        printf("]");
                        return list;
	    case var:	
			printf("var:%s:",list->name);
			track_list(list->data);
			return list;
    	case primitive:	printf("primitive ");return list;
        case macro: printf("macro ");
	    default:break;
	}

    Data* ptr = list;

	if((list->type!=node && list->type!=lambda)|| list==nil) return list;
	putchar('{');
	while(list!=nil){
		track_list(list->elt);
		if(list->next!=nil && list->next->type==node) putchar(' ');
		list=list->next;
	}

	putchar('}');
	fflush(stdout);
    return ptr;
}

int ext(char* string){
	while(*string) if(strncmp(string++,".lisp",5)==0) return 1;
	return 0;
}

int get_next_paren(char** string){
	while(**string && **string!='(' && **string!=EOF) (*string)++;
	if(**string==EOF || **string==0)	return 0;
	return 1;
}

