#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <iostream>
#include <heaplayers.h>

static HL::PosixLock init_lock = HL::PosixLock();
static HL::SpinLock* spin_lock;
static std::atomic<bool> initialized;

static PyObject* mmap_hllock(PyObject* self, PyObject* args) {
    Py_buffer o;
    if(! PyArg_ParseTuple(args, "s*", &o))
        return NULL;
    // std::cout << sizeof(HL::SpinLock) << std::endl;
    if (! initialized) {
        init_lock.lock();
        if(! initialized) {
            // This assumes that we'll
            spin_lock = new(o.buf) HL::SpinLock();
            initialized = true;
        }
        init_lock.unlock();
    }
    spin_lock->lock();
    // memcpy(o.buf, "XYZ", strlen("XYZ"));
//    *((char*) o.buf) = 'a'; 
    // PyTypeObject* t = o->ob_type;
    // const char* c = t->tp_name;
    // std::cout << o << std::endl;
    Py_RETURN_NONE;
}

static PyObject* mmap_hlunlock(PyObject* self, PyObject* args) {
    Py_buffer o;
    if(! PyArg_ParseTuple(args, "s*", &o))
        return NULL;
    spin_lock->unlock();
    Py_RETURN_NONE;
}

static PyMethodDef MmapHlSpinlockMethods[] = {
    {"mmap_hllock", mmap_hllock, METH_VARARGS, "Initializes (if not present) and locks a lock in a buffered region. This should not be used in production code."},
    {"mmap_hlunlock", mmap_hlunlock, METH_VARARGS, "Unlocks a lock. Note that this does not check that the unlocker is the holder of the lock (so please be good)"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef mmaphlspinlockmodule = {
    PyModuleDef_HEAD_INIT,
    "mmap_hl_spinlock",
    NULL,
    -1,
    MmapHlSpinlockMethods
};

PyMODINIT_FUNC PyInit_mmap_hl_spinlock(void) {
    return PyModule_Create(&mmaphlspinlockmodule);
}
