#ifndef IMAGE_H
#define IMAGE_H

#include "imlib2.h"

typedef struct {
    PyObject_HEAD
    Imlib_Image *image;
} ImageObject;

extern PyTypeObject ImageObjectType;

inline ImageObject* ImageObject_New(Imlib_Image *image);

inline int CheckImageObject(PyObject *obj);

#endif

