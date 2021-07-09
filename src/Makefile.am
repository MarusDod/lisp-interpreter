bin_PROGRAMS = lisp

lisp_SOURCES = *.c
lisp_LDADD = -lreadline -ldl
lisp_LDFLAGS = -export-dynamic

include_HEADERS = int.h betterstring.h

AM_CFLAGS = -Wall -g

clean-local:
        @rm config.status configure config.log
        @rm Makefile
        @rm -r autom4te.cache/
        @rm aclocal.m4
        @rm compile install-sh missing Makefile.in