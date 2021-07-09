add_library(betterstring OBJECT betterstring.c)

add_library(lisp_lib STATIC
    data.c
    err.c
    eval.c
    func.c
    parser.c
)

target_link_libraries(lisp_lib PUBLIC betterstring readline dl)

#add_library(data OBJECT data.c)
#add_library(err OBJECT err.c)
#add_library(eval OBJECT eval.c)
#add_library(func OBJECT func.c)
#add_library(parser OBJECT parser.c)

#add_library(lisp_lib STATIC betterstring data err eval func parser)