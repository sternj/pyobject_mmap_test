#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <iostream>
#include <heaplayers.h>

class InitializeMe
{
    static constexpr int MAX_FILE_SIZE = 4096 * 65536;
public:
    InitializeMe()
    {
        _signal_fd = open("/tmp/test-signal", flags, perms);
        _lock_fd = open("/tmp/test-lock", flags, perms);
        ftruncate(_signal_fd, MAX_FILE_SIZE);
        ftruncate(_lock_fd, 4096);
        _mmap = reinterpret_cast<char *>(mmap(0, MAX_FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, _signal_fd, 0));
        _lastpos = reinterpret_cast<uint64_t *>(mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, _lock_fd, 0));
        if (_mmap == MAP_FAILED)
        {
            tprintf::tprintf("Scalene: internal error = @\n", errno);
            abort();
        }
        if (_lastpos == MAP_FAILED)
        {
            tprintf::tprintf("Scalene: internal error = @\n", errno);
            abort();
        }
        _spin_lock = new(_lastpos + sizeof(uint64_t)) HL::SpinLock();
    }
    void lock() {
        *((uint64_t*) _lastpos) = 5;
        _spin_lock->lock();
        std::cout << "Locking " << *((uint64_t*) _lastpos) << std::endl;
    }
    void unlock() {
        std::cout << "Unlocking " << *((uint64_t*) _lastpos) << std::endl;
        _spin_lock->unlock();
    }
private:
    int _signal_fd;
    int _lock_fd;
    char* _mmap;
    uint64_t* _lastpos;
    HL::SpinLock* _spin_lock;
    static constexpr auto flags = O_RDWR | O_CREAT;
    static constexpr auto perms = S_IRUSR | S_IWUSR;
};

static InitializeMe initme;

static PyObject *lock(PyObject* self, PyObject* args) {
    initme.lock();
    Py_RETURN_NONE;
}

static PyObject* unlock(PyObject* self, PyObject* args) {
    initme.unlock();
    Py_RETURN_NONE;
}

static PyObject *mmap_hllock(PyObject *self, PyObject *args)
{
    Py_buffer o;
    if (!PyArg_ParseTuple(args, "s*", &o))
        return NULL;
    HL::SpinLock* spin_lock = (HL::SpinLock*) (((char*)o.buf) + sizeof(uint64_t));
    std::cout << "locking python " << *((uint64_t*) o.buf) << std::endl;
    *((uint64_t*) o.buf) = 10;
    spin_lock->lock();
    Py_RETURN_NONE;
}

static PyObject *mmap_hlunlock(PyObject *self, PyObject *args)
{
    Py_buffer o;
    if (!PyArg_ParseTuple(args, "s*", &o))
        return NULL;
    HL::SpinLock* spin_lock = (HL::SpinLock*) (((char*)o.buf) + sizeof(uint64_t));
    spin_lock->unlock();
    Py_RETURN_NONE;
}

static PyMethodDef MmapHlSpinlockMethods[] = {
    {"lock", lock, METH_VARARGS, "aaa"},
    {"unlock", unlock, METH_VARARGS, "bbb"},
    {"mmap_hllock", mmap_hllock, METH_VARARGS, "Initializes (if not present) and locks a lock in a buffered region. This should not be used in production code."},
    {"mmap_hlunlock", mmap_hlunlock, METH_VARARGS, "Unlocks a lock. Note that this does not check that the unlocker is the holder of the lock (so please be good)"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef mmaphlspinlockmodule = {
    PyModuleDef_HEAD_INIT,
    "mmap_hl_spinlock",
    NULL,
    -1,
    MmapHlSpinlockMethods};

PyMODINIT_FUNC PyInit_mmap_hl_spinlock(void)
{
    return PyModule_Create(&mmaphlspinlockmodule);
}
