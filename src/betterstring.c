#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "betterstring.h"

#define INITIAL_CAP 10
#define FACTOR 1.2


static const char escaped_chars[] = {
    '\\',
    '\n',
    '\'',
    '\"',
    '\b',
    '\e',
    '\r',
    '\t',
    '\f',
};


static const char empty_chars[] = {
    '\n',
    '\b',
    ' ',
    '\r',
    '\t',
};

unsigned int escaped_len = sizeof(escaped_chars) / sizeof(char);
unsigned int empty_len = sizeof(empty_chars) / sizeof(char);

bool is_empty_char(char c){
    for(int i = 0;i < empty_len;i++){
        char a = empty_chars[i];
        if(c == a){
            return true;
        }
    }

    return false;
}

String string_with(unsigned int cap){
    String string;

    string.len = 0;
    string.capacity = cap;

    string.data = calloc(1,cap);

    return string;
}

String string_new(){
    return string_with(INITIAL_CAP);
}

String string_from(char* str){
    unsigned int len = strlen(str);
    String string = string_with(len);
    string.len = len;

    for (size_t i = 0; i < len; i++)
    {
        string.data[i] = str[i];
    }
    
    return string;
}

String string_upgrade(char* str){
    unsigned int len = str ? strlen(str) : 0;
    String string;   
    string.len = len;
    string.capacity = len+1;
    string.data = str;

    return string;
}

String string_from_till(char* str,char* end){
    size_t len = (size_t) end - (size_t) str;
    String string = string_with(len);
    string.len = len;

    for (size_t i = 0; i < len; i++)
    {
        string.data[i] = str[i];
    }
    
    return string;
}

void string_trim(String* string){
    if(!string || !string->data || !string->len) return;

    int beg=0,end=string->len;

    for (size_t i = beg; i < string->len; i++)
    {
        if(!is_empty_char(string->data[i])){
            beg = i;
            break;
        }
    }
 
    for (size_t i = end-1; i >= 0; i--)
    {
        if(!is_empty_char(string->data[i])){
            end = i+1;
            break;
        }
    }   

    if(end == 0 && beg == string->len -1) return;

    String str = string_from_till(string->data + beg,string->data+end);

    string_destroy(*string);
    (*string) = str;
}

String string_from_while(char* str,bool(*fn)(char)){
    char* end_ptr = str;

    while(fn(*end_ptr)) end_ptr++;

    if(*end_ptr != 0 && *end_ptr != ' ' && *end_ptr != '\n' && * end_ptr != '\t' && *end_ptr != '\r' && *end_ptr != ')'){
        String empty;
        empty.data=NULL;
        empty.len = 0;
        empty.capacity = 0;

        return empty;
    }

    size_t len = (size_t) end_ptr - (size_t) str;
    String string = string_with(len);
    string.len = len;

    for (size_t i = 0; i < len; i++)
    {
        string.data[i] = str[i];
    }
    
    return string;
}

String string_copy(String string){
    String str = string_with(string.len * FACTOR + 2);
    str.len = string.len;

    for(int i = 0; i < string.len;i++){
        str.data[i] = string.data[i];
    }

    return str;
}

bool string_isempty(String str){
    return str.len == 0|| str.data == NULL;
}

static void string_increase_cap_n(String* string,unsigned int num){
    string->capacity +=num;
    string->data = realloc(string->data,string->capacity);
}

static void string_increase_cap(String* string){
    string->capacity = string->len * FACTOR + 1;
    string->data = realloc(string->data,string->capacity);
}


__attribute__((unused))
static void string_decrease_cap(String* string){
    unsigned int n = string->len * FACTOR + 1;

    if(n < string->capacity){
        string->data = realloc(string->data,n);
    }
}

void string_append(String* string,char* str){
    if(!str) return;

    unsigned int len = strlen(str);

    string_increase_cap_n(string,len);

    for (size_t i = 0; i < len; i++)
    {
        string->data[i + string->len] = str[i];
    }

    string->len += len;
}

void string_append_char(String* string,char c){
    if(string->len + 1 < string->capacity){
        string_increase_cap(string);
    }

    string->data[string->len++] = c;
}

bool is_escape_char(char c){
    for(int i = 0;i<escaped_len;i++){
        if(c == escaped_chars[i]){
            return true;
        }
    }

    return false;
}



String string_escape(String string){
    String aux = string_with(string.len);

    for(int i = 0; i < string.len;i++){
        if(is_escape_char(string.data[i])){
            string_append_char(&aux,'\\');
        }

        string_append_char(&aux,string.data[i]);
    }

    return aux;
}

bool string_equal(String string,char* str){
    unsigned int len = strlen(str);

    if(len != string.len ){
        return false;
    }

    for(int i = 0;i < string.len;i++){
        if(string.data[i] != str[i]){
            return false;
        }
    }

    return true;
}

bool string_equal_at(String string,char* str,unsigned int at){
    unsigned int len = strlen(str);

    if(len + at != string.len ){
        return false;
    }

    for(int i = 0;i < len;i++){
        if(string.data[i+at] != str[i]){
            return false;
        }
    }

    return true;
}

bool string_cmp_at(String string,char* str,unsigned int at){
    unsigned int len = strlen(str);

    if(len + at > string.len ){
        return false;
    }

    for(int i = 0;i < len;i++){
        if(string.data[at+i] != str[i]){
            return false;
        }
    }

    return true;
}

int string_has_char(String string,char c){
    for (size_t i = 0; i < string.len; i++)
    {
        if(c == string.data[i]){
            return i;
        }
    }
    
    return -1;
}

int string_substr(String string,char* str){
    unsigned int len = strlen(str);

    if(len > string.len) return -1;

    for(int i = 0; i < string.len; i++){
        if(string_cmp_at(string,str,i)){
            return i;
        }
    }

    return -1;
}

const char* string_cstr(String string){
    return string.data;
}

void string_destroy(String string){
    if(string.data) free(string.data);
}

/*
int main(){
    String string = string_from("  aaaa   ");
    string_trim(&string);
    printf("-%s-\n",string.data);
}*/