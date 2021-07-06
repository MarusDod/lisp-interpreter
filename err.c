#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "int.h"

void throw_error(ERROR e,const char* fmt,...){
    va_list list;
    va_start(list,fmt);

    err_t q;
    for(unsigned int i=0;i<ERR_T_SIZE;i++){
        if(e==ERROR_OPCODES[i].e){
            q = ERROR_OPCODES[i];
            break;
        }
    }
    fprintf(stderr,"ERROR: %s ",q.message);
    vfprintf(stderr,fmt,list);

    va_end(list);
    exit(1);
}

void d_free(char** a){
    free(*a);
}
