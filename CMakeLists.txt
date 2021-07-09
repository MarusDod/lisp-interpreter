cmake_minimum_required(VERSION 3.18)
project(lisp-interpreter)

set(CMAKE_EXE_LINKER_FLAGS "-Wl,-export-dynamic")
set(CMAKE_INSTALL_PREFIX /usr/local/)

include_directories(include)

find_library(READLINE_LOCATION NAMES libreadline.so )
add_library(readline SHARED IMPORTED)
set_target_properties(readline PROPERTIES IMPORTED_LOCATION ${READLINE_LOCATION})

find_library(DL_LOCATION NAMES libdl.so )
add_library(dl SHARED IMPORTED)
set_target_properties(dl PROPERTIES IMPORTED_LOCATION ${DL_LOCATION})



add_subdirectory(src)
add_executable(lisp src/main.c)

target_link_libraries(lisp PUBLIC readline dl lisp_lib)

#add_subdirectory(test)


install(
    TARGETS lisp 
    RUNTIME DESTINATION bin
    #CONFIGURATIONS Release
    )