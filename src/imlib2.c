#include <Python.h>
#include "imlib2.h"
#include "image.h"

PyObject *img_load_error;

static inline ImageObject *
_imlib2_open(char *filename, int use_cache)
{
    Imlib_Image *image;
    Imlib_Load_Error error = IMLIB_LOAD_ERROR_NONE;

    if (use_cache){
      image = imlib_load_image_with_error_return(filename, &error);
    }else{
      image = imlib_load_image_immediately_without_cache(filename);
    }
    CHECK_LOAD_ERROR(error);
#ifdef DEVELOP
    imlib_context_set_image(image);
    DEBUG("width %d", imlib_image_get_width());
    DEBUG("height %d", imlib_image_get_height());
#endif
    return ImageObject_New(image);
}

inline PyObject * 
_pyimlib2_open(PyObject *self, PyObject *args)
{
    char *file;
    ImageObject *image;
    int use_cache = 1;

    if (!PyArg_ParseTuple(args, "s|i", &file, &use_cache))
        return NULL;

    image = _imlib2_open(file, use_cache);
    if (!image){
        return NULL;
    }
    return (PyObject *)image;
}

inline PyObject * 
_pyimlib2_create_image(PyObject *self, PyObject *args)
{
    int w, h;
    if (!PyArg_ParseTuple(args, "ii:create_image", &w, &h)){
        return NULL;
    }
    Imlib_Image image = imlib_create_image(w, h);
    if(!image){
        //TODO Error
        return NULL;
    }
    return (PyObject *)ImageObject_New(image);
}

inline int
raise_load_error(Imlib_Load_Error e)
{
    int ret;
    switch(e){
        case IMLIB_LOAD_ERROR_NONE:
            ret = 0;
            break;
        case IMLIB_LOAD_ERROR_FILE_DOES_NOT_EXIST:
            PyErr_SetString(img_load_error, "File does not exist");
            ret = 1;
            break;
        case IMLIB_LOAD_ERROR_FILE_IS_DIRECTORY:
            PyErr_SetString(img_load_error, "File is directory");
            ret = 1;
            break;
        case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_READ:
            PyErr_SetString(img_load_error, "Permission denied to read");
            ret = 1;
            break;
        case IMLIB_LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT:
            PyErr_SetString(img_load_error, "No loader for file format");
            ret = 1;
            break;
        case IMLIB_LOAD_ERROR_PATH_TOO_LONG:
            PyErr_SetString(img_load_error, "Path too long");
            ret = 1;
            break;
        case IMLIB_LOAD_ERROR_PATH_COMPONENT_NON_EXISTANT:
            PyErr_SetString(img_load_error, "Path component non existant");
            ret = 1;
            break;
        case IMLIB_LOAD_ERROR_PATH_COMPONENT_NOT_DIRECTORY:
            PyErr_SetString(img_load_error, "Path component not directory");
            ret = 1;
            break;
        case IMLIB_LOAD_ERROR_PATH_POINTS_OUTSIDE_ADDRESS_SPACE:
            PyErr_SetString(img_load_error, "Path points outside address space");
            ret = 1;
            break;
        case IMLIB_LOAD_ERROR_TOO_MANY_SYMBOLIC_LINKS:
            PyErr_SetString(img_load_error, "Too many symbolic links");
            ret = 1;
            break;
        case IMLIB_LOAD_ERROR_OUT_OF_MEMORY:
            PyErr_SetString(img_load_error, "Out of memory");
            ret = 1;
            break;
        case IMLIB_LOAD_ERROR_OUT_OF_FILE_DESCRIPTORS:
            PyErr_SetString(img_load_error, "Out of file descriptors");
            ret = 1;
            break;
        case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_WRITE:
            PyErr_SetString(img_load_error, "Permission denied to write");
            ret = 1;
            break;
        case IMLIB_LOAD_ERROR_OUT_OF_DISK_SPACE:
            PyErr_SetString(img_load_error, "Out of disk space");
            ret = 1;
            break;
        case IMLIB_LOAD_ERROR_UNKNOWN:
            PyErr_SetString(img_load_error, "Unknown");
            ret = 1;
            break;
        default:
            ret = 0;
            break;
    }
    return ret;
}


static PyMethodDef PyImlib2Methods[] = {
    { "open", _pyimlib2_open, METH_VARARGS },
    { "create_image", _pyimlib2_create_image, METH_VARARGS },
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


PyMODINIT_FUNC
initpyimlib2(void)
{
    PyObject *m;
    
    m = Py_InitModule3("pyimlib2", PyImlib2Methods, "");
    if(m == NULL){
        return;
    }

    if(PyType_Ready(&ImageObjectType) < 0){ 
        return;
    }
	Py_INCREF((PyObject *)&ImageObjectType);
	if (PyModule_AddObject(m, "Image", (PyObject *)&ImageObjectType) != 0){
		return;
    }
    
    img_load_error = PyErr_NewException("pyimlib2.ImageLoadException",
					  PyExc_IOError, NULL);
	if (img_load_error == NULL){
		return;
    }
	Py_INCREF(img_load_error);
	PyModule_AddObject(m, "ImageLoadException", img_load_error);
}
