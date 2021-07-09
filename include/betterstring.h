#ifndef BETTERSTRING_H
#define BETTERSTRING_H

typedef struct {
    unsigned int len,capacity;
    char* data;
}String;

String string_with(unsigned int cap);
String string_new();
String string_upgrade(char* str);
String string_from(char* str);
String string_from_till(char* str,char* end);
String string_from_while(char* str,bool(*fn)(char));
bool string_isempty(String str);
String string_copy(String string);
void string_append(String* string,char* str);
void string_trim(String* string);
void string_append_char(String* string,char c);
bool is_escape_char(char c);
String string_escape(String string);
bool string_equal(String string,char* str);
bool string_equal_at(String string,char* str,unsigned int at);
bool string_cmp_at(String string,char* str,unsigned int at);
int string_has_char(String string,char c);
int string_substr(String string,char* str);
const char* string_cstr(String string);
void string_destroy(String string);

#endif