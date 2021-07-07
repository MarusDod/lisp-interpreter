#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include "int.h"


Data* T = &(Data) {.type=SYMBOL,.symbol="T"};
Data* nil = &(Data) {.type=SYMBOL,.symbol="NIL"};
Data* Abort = &(Data) {.type=SYMBOL,.symbol="ABORT"};

err_t ERROR_OPCODES[] = {
    {ERR_DIV,"Division by 0"},
    {ERR_INT_OVERFLOW,"Integer overflow"},
    {ERR_NOT_FOUND,"Symbol not found"},
    {ERR_LEN_MISMATCH,"Length mismatch"},
    {ERR_OUT_OF_BOUNDS,"Out of bounds index"},
    {ERR_TYPE,"Incompatible type"},
    {ERR_PARSING,"Could not read line"},
    {ERR_MEMORY,"Could not allocate memory"}
};

size_t ERR_T_SIZE = sizeof(ERROR_OPCODES)/sizeof(err_t);



int main(int argc,char* argv[]){

    Data* env = load_symbols();

    if(argc<2){ 
        char* a = "(loop (print \"User>\") (print (readline)))";
        Data* e = parse_input(&a);
        eval(e->elt,env);
    }

    else{
        char* filename=argv[1];
        if(!ext(filename)){
            fprintf(stderr,"Needs to be a lisp file\n");
            exit(1);
        }
        
        load_file(filename,env);
    }

return 0;
}




