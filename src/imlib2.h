#ifndef IMLIB2_H
#define IMLIB2_H

#include <Python.h>
#include <Imlib2.h>


#ifdef DEVELOP
#define DEBUG(...) \
    do { \
        printf(__VA_ARGS__); \
        printf("\n"); \
    } while(0)
#else
#define DEBUG(...) do{}while(0)
#endif

extern PyObject* img_load_error;

inline int 
raise_load_error(Imlib_Load_Error error);

#define CHECK_LOAD_ERROR(x) \
    if(raise_load_error(x)){ \
        return NULL; \
    } \



#endif 
