#ifndef STACK_H
#define STACK_H

#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#pragma pack(push, 1)

//#define UNUSED(x) (void)(x)
//#define LAMBDA(ret,body) ({ret _ body _;})

#define STRING_MAX 20

typedef struct _data Data;

typedef Data* (*Primitive)(Data*,Data*);
typedef Data* (*Macro)(Data*,Data*);

typedef enum{
    number=1,
    node=2,
    string=4,
    symbol=8,
    var=16,
    primitive=32,
    lambda=64,
    macro=128,
    array=256
}Type;

typedef enum{
    ERR_DIV,
    ERR_INT_OVERFLOW,
    ERR_NOT_FOUND,
    ERR_LEN_MISMATCH,
    ERR_OUT_OF_BOUNDS,
    ERR_TYPE,
    ERR_PARSING,
    ERR_MEMORY
}ERROR;

typedef struct{
    ERROR e;
    char message[50];
}err_t;


extern err_t ERROR_OPCODES[];
extern unsigned long ERR_T_SIZE;

#define assure(b,e) if(!(b)) throw_error(e,"at %s %s line %d\n",__FILE__,__FUNCTION__,__LINE__)

struct _data{
	Type type;
	union{
		struct{
			struct _data* elt;
			struct _data* next;
		};
		int value;
		char str[STRING_MAX];
		char symbol[STRING_MAX];
        struct{
            Data** array;
            unsigned int size;
        };
		Primitive fn;
        Macro mac;
		struct{
			char name[STRING_MAX];
			Data* data;
		};
	};
};

extern Data* T;
extern Data* nil;
extern Data* clean_list;
extern Data* Abort;

//lisp.c

Data* load_list(char*);
void delete_list(Data**);
Data* insert_node(Data**,Data*);
Data* make_number(int);
Data* make_symbol(char*);
Data* make_primitive(Primitive);
Data* make_macro(Macro);
Data* make_array(Data* lst);
Data* make_variable(char*,Data*);
Data* make_lambda(Data*,Data*);
void insert_elt_variable(Data**,char*,Data*);
Data* make_string(char*);
void shift_car(Data**);
Data* car(Data*);
int list_length(Data*);
Data* copy(Data*,Data*);
Data* compare(Data*,Data*,Data*);
__attribute__((destructor)) void collect();
Data* wrap(Data* l);

//parser.c

Data* parse_input(char**);
void list_to_string(Data*);
int ext(char*);
int get_next_paren(char**);
Data* track_list(Data* list);

//lambda.c

Data* eval(Data*,Data*);
Data* eval_args(Data*,Data*);
Data* load_symbols();
Data* eval_primitive(Data*,Data*);
Data* eval_lambda(Data*,Data*,Data*);
Data* handle_symbol(Data* var_list,Data* s);
Data* search_symbol(Data* var_list,char* string);
Data* progn(Data* args,Data* env);
void load_file(char* name,Data* env);
Data* try_eval(Data* data,Data* env);

//foo.c

Data* load_symbols();

//err.c

void throw_error(ERROR e,const char* fmt,...);
void d_free(char** a);

#endif


