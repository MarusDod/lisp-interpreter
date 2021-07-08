#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

#include "betterstring.h"
#include "int.h"

typedef Data* (*Parser)(char**);

Data* parse_string_literal(char** input);
Data* parse_symbol(char** input);
Data* parse_quote(char** input);
Data* parse_array(char** input);
Data* parse_number(char** input);
Data* parse_list(char** input);
Data* parse_input(char** input);

static const Parser parsers[] = {
    parse_number,
    parse_list,
    parse_symbol,
    parse_quote,
    parse_array,
    parse_string_literal,
};


void skip_comment(char** text){
    if(**text != ';') return;

    (*text)++;
    while(**text!='\n' && **text && **text!=EOF) (*text)++;
    if(**text=='\n') (*text)++;
}

void skip_empty(char** text){
    while(**text){
        switch(**text){
            case '\n':
            case '\t':
            case ' ':
            (*text)++;
            default:break;
        }

        if(**text == ';'){
            (*text)++;
            while(**text!='\n' && **text && **text!=EOF) (*text)++;
            if(**text=='\n') (*text)++;
        }
        else break;

    }
}

Data* parse_input(char** input){
    Data* data = NULL;

    skip_empty(input);

    for (size_t i = 0; i < sizeof(parsers) / sizeof(Parser); i++)
    {
        if((data = parsers[i](input))){
            return data;
        }
    }
    
    return NULL;
}


Data* parse_number(char** input){
    int a,n;

    if(sscanf(*input,"%i%n",&a,&n)==1){
        (*input)+=n;
        return make_number(a);
    }

    return NULL;
}

Data* parse_string_literal(char** input){
    if(**input != '\"'){
        return NULL;
    }

    (*input)++;

    char* end_ptr = *input;

    while(*end_ptr && end_ptr){
        if(*end_ptr == '\"'){
            break;
        }

        if(*end_ptr == '\\'){
            end_ptr++;
        }

        end_ptr++;
    }

    String str = string_from_till(*input,end_ptr);

    String ret = string_escape(str);

    string_destroy(str);
    (*input) = end_ptr + 1;

    return make_string(ret.data);
}

bool symbol_callback(char c){
    static const char symbol_chars[] = {
        '+',
        '-',
        '*',
        '_',
        '/',
        ':',
        '<',
        '>',
        '.',
    };

    for (size_t i = 0; i < sizeof(symbol_chars)/sizeof(char); i++)
    {
        if(c == symbol_chars[i]){
            return true;
        }
    }
    
    return (c >= 'a'&& c <= 'z') || (c >= 'A'&& c <= 'Z');
};

Data* parse_symbol(char** input){

    String str = string_from_while(*input,symbol_callback);

    if(string_isempty(str)) return NULL;

    (*input) += str.len;

    return make_symbol(str.data);
}

Data* parse_quote(char** input){
    if(**input != '\''){
        return NULL;
    }

    (*input)++;

    Data* e = NULL;

    if((e = parse_list(input))){
    }
    else if((e = parse_symbol(input))){
    }
    else{
        return nil;
    }


    return make_quote(e);
}

Data* parse_array(char** input){
    if(**input != '#'){
        return NULL;
    }

    (*input)++;

    Data* e = parse_list(input);

    if(!e){
        return NULL;
    }

    return make_array(e);
}

Data* parse_list(char** input){
    if(**input != '('){
        return NULL;
    }

    (*input)++;

    Data* list = nil,*data = NULL;

    do{
        data = parse_input(input);
        if(data){
            insert_node(&list,data);
        }
    }while(data);

    skip_empty(input);

    if(**input != ')'){
        return NULL;
    }

    (*input)++;

    return list;
}

/*
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
            else if(sscanf(*input," \"%[A-Za-z <>\'0-9/.-]s\" ",ptr)==1){
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
            else if(sscanf(*input," %[a-zA-Z+-*%/<>.]s ",ptr)==1){
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
*/

void list_to_string(Data* list){

    switch(list->type){
    case SYMBOL:    printf("%s",list->symbol);return;
    case STRING:    printf("%s",list->str);return;
    case BLOB:      printf("BINARY");return;
    case NUMBER:    printf("%d",list->value);return;
    case VAR:    
                 printf(" %s:",list->name);
                 list_to_string(list->data);
                 return;
     case ARRAY: printf("#(");
                 for(unsigned int i=0;i<list->size;i++){
                    list_to_string(list->array[i]);
                    if(i<list->size-1) printf(" ");
                 }
                    printf(")");
                return;
    case PRIMITIVE:    printf("primitive");return;
    case LAMBDA:    printf("lambda");return;
    case MACRO: printf("macro");return;
    default:break;
    }

    assert(list->type==NODE);
    putchar('(');
    while(list!=nil){
        list_to_string(list->elt);
        if(list->next!=nil && list->next->type==NODE) putchar(' ');
        list=list->next;
    }

    putchar(')');
    fflush(stdout);
}

Data* track_list(Data* list){

    if(list==T) {puts("global:T");return list;}
    else if(list==nil) {puts("global:NIL");return list;}

    switch(list->type){
        case SYMBOL:    
            printf("symbol:%s",list->symbol);
            return list;
        case STRING:    
            printf("string:\"%s\"",list->str);
            return list;
        case NUMBER:    
            printf("integer:%d",list->value);
            return list;
        case BLOB:    
            printf("BLOB");
            return list;
        case ARRAY:     
            printf("array: [");
            for(unsigned int i=0;i<list->size;i++){
                track_list(list->array[i]);
            }
            printf("]");
            return list;
        case VAR:    
            printf("var:%s:",list->name);
            track_list(list->data);
            return list;
        case PRIMITIVE:    
            printf("primitive ");
            return list;
        case MACRO: 
            printf("macro ");
        default:
            break;
    }

    Data* ptr = list;

    if((list->type!=NODE && list->type!=LAMBDA)|| list==nil) return list;
    putchar('{');
    while(list!=nil){
        track_list(list->elt);
        if(list->next!=nil && list->next->type==NODE) putchar(' ');
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
    if(**string==EOF || **string==0)    return 0;
    return 1;
}

