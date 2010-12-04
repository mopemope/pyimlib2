#include "image.h"

inline int 
CheckImageObject(PyObject *obj)
{
    ImageObject *img;
    if (Py_TYPE(obj) != &ImageObjectType){
        return 0;
    }
    img = (ImageObject *)obj;
    if(!img->image){
        return 0;
    }
    return 1;    
}

inline ImageObject* 
ImageObject_New(Imlib_Image *image)
{
    ImageObject *self;

    self = PyObject_NEW(ImageObject, &ImageObjectType);
    if(self == NULL){
        return NULL;
    }
    self->image = image;
    return self;
}

static inline void
ImageObject_dealloc(ImageObject* self)
{
    imlib_context_set_image(self->image);
    imlib_free_image();
    PyObject_DEL(self);
}

static inline PyObject * 
ImageObject_clone_image(PyObject* self, PyObject *args)
{
    imlib_context_set_image(((ImageObject *)self)->image);
    Imlib_Image image = imlib_clone_image();
    if(image){
        return NULL;
    }
    return (PyObject *)ImageObject_New(image);
}

static inline PyObject * 
ImageObject_cropped_image(PyObject* self, PyObject *args)
{
    int x, y, w, h;
    if (!PyArg_ParseTuple(args, "iiii:cropped_image", &x, &y, &w, &h)){
        return NULL;
    }
    
    imlib_context_set_image(((ImageObject *)self)->image);
    int maxw = imlib_image_get_width();
    if(maxw < w){
        w = maxw;
    }
    int maxh = imlib_image_get_height();
    if(maxh < h){
        h = maxh;
    }

    Imlib_Image image = imlib_create_cropped_image(x, y, w, h);
    if(!image){
        return NULL;
    }
    return (PyObject *)ImageObject_New(image);
}


static inline PyObject * 
ImageObject_cropped_scaled_image(PyObject* self, PyObject *args)
{
    int x, y, w, h, dw, dh;
    if (!PyArg_ParseTuple(args, "iiiiii:cropped_scaled_image", &x, &y, &w, &h, &dw, &dh)){
        return NULL;
    }
    imlib_context_set_image(((ImageObject *)self)->image);
    int maxw = imlib_image_get_width();
    if(maxw < w){
        w = maxw;
    }
    int maxh = imlib_image_get_height();
    if(maxh < h){
        h = maxh;
    }
    Imlib_Image image = imlib_create_cropped_scaled_image(x, y, w, h, dw, dh);
    if(!image){
        return NULL;
    }
    return (PyObject *)ImageObject_New(image);
}

static inline PyObject * 
ImageObject_scaled_image(PyObject* self, PyObject *args)
{
    int x = 0, y = 0, w = 0, h = 0, dw = 0, dh = 0;
    if (!PyArg_ParseTuple(args, "ii:cropped_scaled_image", &dw, &dh)){
        return NULL;
    }
    imlib_context_set_image(((ImageObject *)self)->image);
    w = imlib_image_get_width();
    h = imlib_image_get_height();
    Imlib_Image image = imlib_create_cropped_scaled_image(x, y, w, h, dw, dh);
    if(!image){
        return NULL;
    }
    return (PyObject *)ImageObject_New(image);
}

static inline PyObject * 
ImageObject_blend_image(PyObject* self, PyObject *args, PyObject *kwargs)
{
    int merge_alpha = 1;
    PyObject *src = NULL;
    int src_x = 0, src_y = 0, src_h = -1, src_w = -1;
    int dest_x = 0, dest_y = 0, dest_h = -1, dest_w = -1;
    Imlib_Image *src_img;

	static char *keywords[] = {"img", "src", "dest", "alpha", NULL};
	
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|(iiii)(iiii)i:blend_image", keywords, &src,
                &src_x, &src_y, &src_w, &src_h,
                &dest_x, &dest_y, &dest_w, &dest_h, &merge_alpha)){
		return NULL;
    }
    DEBUG("src = %d %d %d %d ", src_x, src_y, src_w, src_h);
    DEBUG("dest = %d %d %d %d ", dest_x, dest_y, dest_w, dest_h);
    DEBUG("alpha = %d ", merge_alpha);
    
    //check
    if(!CheckImageObject(src)){
        //TODO raise TypeError
        return NULL;
    }
    

    src_img = (((ImageObject *)src)->image);
    imlib_context_set_image(src_img);
    if(src_w == -1){
        src_w = imlib_image_get_width();
    }
    if(src_h == -1){
        src_h = imlib_image_get_height();
    }
    if(dest_w == -1){
        dest_w = src_w;
    }
    if(dest_h == -1){
        dest_h = src_h;
    }
    
    
    imlib_context_set_image(((ImageObject *)self)->image);

    imlib_context_set_blend(1);
    imlib_blend_image_onto_image(src_img, merge_alpha,
                     src_x, src_y, src_w, src_h,
                     dest_x, dest_y, dest_w, dest_h);
    Py_RETURN_NONE;
}

static inline PyObject * 
ImageObject_save(PyObject* self, PyObject *args)
{
    char *filename, *ext;
    Imlib_Load_Error error = IMLIB_LOAD_ERROR_NONE;

    if (!PyArg_ParseTuple(args, "ss:save", &filename, &ext)){
        return NULL;
    }

    imlib_context_set_image(((ImageObject *)self)->image);

    imlib_image_set_format(ext);
    imlib_save_image_with_error_return(filename, &error);

    CHECK_LOAD_ERROR(error);
    
    Py_RETURN_NONE;

}

static inline PyObject * 
ImageObject_draw_rectangle(PyObject* self, PyObject *args, PyObject *kwargs)
{
    int x, y, w, h;
    
    int r=-1, g, b, alpha, fill=0;
	
    static char *keywords[] = {"x", "y", "w", "h", "color", "fill", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iiii|(iiii)i:draw_rectangle", keywords, 
                &x, &y, &w, &h, &r, &g, &b, &alpha, &fill)){
        return NULL;
    }

    imlib_context_set_image(((ImageObject *)self)->image);
    if(r >= 0){
        //set color
        imlib_context_set_color(r, g, b, alpha);
    }
    DEBUG("fill %d", fill);    
    if(fill){
        imlib_image_fill_rectangle(x, y, w, h);
    }else{
        imlib_image_draw_rectangle(x, y, w, h);
    }
    Py_RETURN_NONE;
}

static inline PyObject * 
ImageObject_draw_ellipse(PyObject* self, PyObject *args, PyObject *kwargs)
{
    int xc, yc, a, b;
    
    int red=-1, green, blue, alpha, fill=0;
	
    static char *keywords[] = {"xc", "yc", "a", "b", "color", "fill", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iiii|(iiii)i:draw_ellipsm", keywords, 
                &xc, &yc, &a, &b, &red, &green, &blue, &alpha, &fill)){
        return NULL;
    }

    imlib_context_set_image(((ImageObject *)self)->image);
    if(red >= 0){
        //set color
        imlib_context_set_color(red, green, blue, alpha);
    }
    if(fill){
        imlib_image_fill_ellipse(xc, yc, a, b);
    }else{
        imlib_image_draw_ellipse(xc, yc, a, b);
    }
    Py_RETURN_NONE;
}

static inline PyObject * 
ImageObject_set_color(PyObject* self, PyObject *args)
{
    int red, green, blue, alpha=255;
    if (!PyArg_ParseTuple(args, "(iiii):set_color", &red, &green, &blue, &alpha)){
        return NULL;
    }

    imlib_context_set_image(((ImageObject *)self)->image);
    
    imlib_context_set_color(red, green, blue, alpha);
    
    Py_RETURN_NONE;

}

static inline PyObject * 
ImageObject_attach_value(PyObject* self, PyObject *args)
{
    char *key;
    int value;
    if (!PyArg_ParseTuple(args, "si:attach_value", &key, &value)){
        return NULL;
    }

    imlib_context_set_image(((ImageObject *)self)->image);
    imlib_image_attach_data_value(key, NULL, value, NULL);
    Py_RETURN_NONE;
}

static inline PyObject * 
ImageObject_orientate(PyObject* self, PyObject *args)
{
    int orientate;
    if (!PyArg_ParseTuple(args, "i:orientate", &orientate)){
        return NULL;
    }
    imlib_context_set_image(((ImageObject *)self)->image);
    imlib_image_orientate(orientate);
    Py_RETURN_NONE;

}

static inline PyObject * 
ImageObject_get_width(PyObject* self, PyObject *args)
{
    imlib_context_set_image(((ImageObject *)self)->image);
    int w = imlib_image_get_width();
    return Py_BuildValue("i", w);
}

static inline PyObject * 
ImageObject_get_height(PyObject* self, PyObject *args)
{
    imlib_context_set_image(((ImageObject *)self)->image);
    int h = imlib_image_get_height();
    return Py_BuildValue("i", h);
}

static PyMethodDef ImageObject_methods[] = {
    {"clone_image", ImageObject_clone_image, METH_NOARGS, 0},
    {"cropped_image", ImageObject_cropped_image, METH_VARARGS, 0},
    {"cropped_scaled_image", ImageObject_cropped_scaled_image, METH_VARARGS, 0},
    {"scaled_image", ImageObject_scaled_image, METH_VARARGS, 0},
    {"blend_image", (PyCFunction)ImageObject_blend_image, METH_VARARGS|METH_KEYWORDS, 0},
    {"draw_rectangle", (PyCFunction)ImageObject_draw_rectangle, METH_VARARGS|METH_KEYWORDS, 0},
    {"draw_ellipse", (PyCFunction)ImageObject_draw_ellipse, METH_VARARGS|METH_KEYWORDS, 0},
    {"orientate", ImageObject_orientate, METH_VARARGS, 0},
    {"get_width", ImageObject_get_width, METH_NOARGS, 0},
    {"get_height", ImageObject_get_height, METH_VARARGS, 0},
    
    {"attach_value", ImageObject_attach_value, METH_VARARGS, 0},
    {"set_color", ImageObject_set_color, METH_VARARGS, 0},
    {"save", ImageObject_save, METH_VARARGS, 0},
    {NULL, NULL}
};

PyTypeObject ImageObjectType = {
	PyObject_HEAD_INIT(&PyType_Type)
    0,
    "pyimlib2.Image",             /*tp_name*/
    sizeof(ImageObject), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ImageObject_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "ImageObject",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    ImageObject_methods,             /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                      /* tp_init */
    0,                         /* tp_alloc */
    0,                           /* tp_new */
};

