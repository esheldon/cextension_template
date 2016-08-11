/*

   Example C extension

   Implement a nontrivial python class representing an array
   of integers.

   Implement module level functions.

   be python2 and python3 compatible
*/

#include <string.h>
#include <Python.h>

/*

   first some helper functions

*/

// uncomment to check for when an object is sent as an arg
/*
static int is_python_string(const PyObject* obj)
{
#if PY_MAJOR_VERSION >= 3
    return PyUnicode_Check(obj) || PyBytes_Check(obj);
#else
    return PyUnicode_Check(obj) || PyString_Check(obj);
#endif
}
*/


static PyObject* make_python_string(const char *str)
{
    return Py_BuildValue("s",str);
}

static PyObject* make_python_integer(long value)
{
    return Py_BuildValue("l", value);
}


/*

   define a new class representing an integer array

*/

struct PyIntArrayObject {
    PyObject_HEAD

    long size;
    long *data;

};

static int
PyIntArrayObject_init(struct PyIntArrayObject* self, PyObject *args, PyObject *kwds)
{
    long size=0;

    if (!PyArg_ParseTuple(args, (char*)"l", &size)) {
        return -1;
    }

    if (size <= 0) {
        PyErr_Format(
            PyExc_ValueError,
            "array dimensions must be greater than 0, got %ld",
            size
        );
        return -1;
    }

    self->data = calloc((size_t)size, sizeof(long));
    if (NULL == self->data) {
        PyErr_SetString(PyExc_MemoryError,"out of memory");
        return -1;
    }
    self->size = size;

    return 0;
}


static PyObject *
PyIntArrayObject_repr(struct PyIntArrayObject* self) {

    char repr[256];

    sprintf(repr, "int array[%ld]\n", self->size);

    return make_python_string(repr);
}

static void
PyIntArrayObject_dealloc(struct PyIntArrayObject* self)
{

    if (NULL != self->data) {
        free(self->data);
        self->data=NULL;
    }

#if PY_MAJOR_VERSION >= 3
    // introduced in python 2.6
    Py_TYPE(self)->tp_free((PyObject*)self);
#else
    // old way, removed in python 3
    self->ob_type->tp_free((PyObject*)self);
#endif

}


static PyObject *
PyIntArrayObject_get_size(struct PyIntArrayObject* self) {
    return make_python_integer(self->size);
}

// check the index and set an error if needed
static int check_index(struct PyIntArrayObject* self, long index)
{
    if (index < 0 || index > (self->size-1)) {
        PyErr_Format(
            PyExc_IndexError,
            "index %ld is out of bounds [0,%ld)",
            self->size
        );
        return 0;
    } else {
        return 1;
    }
}

static PyObject *
PyIntArrayObject_set(struct PyIntArrayObject* self, PyObject *args, PyObject *kwds) {
    long index=0, value=0;
    if (!PyArg_ParseTuple(args, (char*)"ll", &index, &value)) {
        return NULL;
    }

    if (!check_index(self, index)) {
        return NULL;
    }

    self->data[index] = value;

    Py_RETURN_NONE;
}

static PyObject *
PyIntArrayObject_get(struct PyIntArrayObject* self, PyObject *args, PyObject *kwds) {
    long index=0, value=0;
    if (!PyArg_ParseTuple(args, (char*)"l", &index)) {
        return NULL;
    }

    if (!check_index(self, index)) {
        return NULL;
    }

    value = self->data[index];

    return make_python_integer(value);
}



static PyMethodDef PyIntArrayObject_methods[] = {
    {"get_size",             (PyCFunction)PyIntArrayObject_get_size,             METH_NOARGS,  "get the size of the array."},
    {"set",             (PyCFunction)PyIntArrayObject_set,             METH_VARARGS,  "set an array element"},
    {"get",             (PyCFunction)PyIntArrayObject_get,             METH_VARARGS,  "get an array element"},

    {NULL}  /* Sentinel */
};

static PyTypeObject PyIntArrayType = {
#if PY_MAJOR_VERSION >= 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
#endif
    "_example.IntArray",             /*tp_name*/
    sizeof(struct PyIntArrayObject), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyIntArrayObject_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    //0,                         /*tp_repr*/
    (reprfunc)PyIntArrayObject_repr,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Integer array Class",           /* tp_doc */
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    0,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    PyIntArrayObject_methods,             /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    //0,     /* tp_init */
    (initproc)PyIntArrayObject_init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,                 /* tp_new */
};

static PyObject* PyExample_get_version(void)
{
    const char *version="0.9.0";
    return make_python_string(version);
}

// module methods
static PyMethodDef example_methods[] = {
    {"get_version",      (PyCFunction)PyExample_get_version,      METH_NOARGS,  "Get the version string"},
    {NULL}  /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3
    static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_example",      /* m_name */
        "Defines the integer array class and some module methods",  /* m_doc */
        -1,                  /* m_size */
        example_methods,    /* m_methods */
        NULL,                /* m_reload */
        NULL,                /* m_traverse */
        NULL,                /* m_clear */
        NULL,                /* m_free */
    };
#endif

#ifndef PyMODINIT_FUNC  /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
#if PY_MAJOR_VERSION >= 3
PyInit__example(void) 
#else
init_example(void) 
#endif
{
    PyObject* m;

    PyIntArrayType.tp_new = PyType_GenericNew;

#if PY_MAJOR_VERSION >= 3
    if (PyType_Ready(&PyIntArrayType) < 0) {
        return NULL;
    }
    m = PyModule_Create(&moduledef);
    if (m==NULL) {
        return NULL;
    }

#else
    if (PyType_Ready(&PyIntArrayType) < 0) {
        return;
    }
    m = Py_InitModule3("_example", example_methods, "Define the integer array class and some module methods.");
    if (m==NULL) {
        return;
    }
#endif

    Py_INCREF(&PyIntArrayType);
    PyModule_AddObject(m, "IntArray", (PyObject *)&PyIntArrayType);

    // if using numpy, use this function here
    //import_array();
#if PY_MAJOR_VERSION >= 3
    return m;
#endif
}
