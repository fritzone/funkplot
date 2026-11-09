#include "pyembed.h"

#include <new>

PyObject *emb::Stdout_write(PyObject *self, PyObject *args)
{
    std::size_t written(0);
    Stdout* selfimpl = reinterpret_cast<Stdout*>(self);
    if (selfimpl->write)
    {
        char* data;
        if (!PyArg_ParseTuple(args, "s", &data))
            return 0;

        std::string str(data);
        selfimpl->write(str);
        written = str.size();
    }
    return PyLong_FromSize_t(written);
}

PyObject* emb::Stdout_flush(PyObject *, PyObject *)
{
    // no-op
    return Py_BuildValue("");
}

PyObject* emb::Stdout_new(PyTypeObject* type, PyObject* /*args*/, PyObject* /*kwds*/)
{
    PyObject* self = type->tp_alloc(type, 0);
    if (self)
    {
        new (&reinterpret_cast<Stdout*>(self)->write) stdout_write_type();
    }
    return self;
}

void emb::Stdout_dealloc(PyObject* self)
{
    reinterpret_cast<Stdout*>(self)->write.~stdout_write_type();
    Py_TYPE(self)->tp_free(self);
}

void emb::set_stdout(stdout_write_type write)
{
    if (!g_stdout)
    {
        // PySys_GetObject returns a borrowed reference, and replacing sys.stdout
        // below may drop the last one, so claim our own for the lifetime of the
        // redirection.
        g_stdout_saved = PySys_GetObject("stdout");
        Py_XINCREF(g_stdout_saved);

        g_stdout = StdoutType.tp_new(&StdoutType, 0, 0);
        if (!g_stdout)
        {
            Py_CLEAR(g_stdout_saved);
            return;
        }
    }

    Stdout* impl = reinterpret_cast<Stdout*>(g_stdout);
    impl->write = write;
    PySys_SetObject("stdout", g_stdout);
}

void emb::reset_stdout()
{
    if (!g_stdout)
    {
        return; // nothing installed, nothing to restore
    }

    if (g_stdout_saved)
    {
        PySys_SetObject("stdout", g_stdout_saved);
    }

    // Drop the callback before letting go of the object: the caller owns whatever
    // the callback captured, and Python may still hold a reference to the object
    // itself, which would otherwise keep calling into the caller's dead frame.
    reinterpret_cast<Stdout*>(g_stdout)->write = nullptr;

    Py_CLEAR(g_stdout_saved);
    Py_CLEAR(g_stdout);
}

PyMODINIT_FUNC emb::PyInit_emb(void)
{
    g_stdout = 0;
    g_stdout_saved = 0;

    StdoutType.tp_new = Stdout_new;
    StdoutType.tp_dealloc = Stdout_dealloc;
    if (PyType_Ready(&StdoutType) < 0)
        return 0;

    PyObject* m = PyModule_Create(&embmodule);
    if (m)
    {
        Py_INCREF(&StdoutType);
        PyModule_AddObject(m, "Stdout", reinterpret_cast<PyObject*>(&StdoutType));
    }
    return m;
}
