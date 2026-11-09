#include "PythonRunner.h"
#include "pyembed.h"
#include "PyObjectHandle.h"
#include "util.h"

#include <QDebug>

static bool exited = false;

namespace
{

// Undoes what python_runner::run() sets up, on every path out of it: the stdout
// redirection holds a reference to a local of that function, and the GIL has to go
// back to where it was even when the script failed to compile or raised.
struct python_run_cleanup
{
    explicit python_run_cleanup(PyGILState_STATE state) : m_state(state) {}

    ~python_run_cleanup()
    {
        emb::reset_stdout();
        PyGILState_Release(m_state);
    }

    python_run_cleanup(const python_run_cleanup&) = delete;
    python_run_cleanup& operator=(const python_run_cleanup&) = delete;

private:
    PyGILState_STATE m_state;
};

} // namespace

static int python_exiter(void*)
{

    qInfo() << "Entered";
    if(exited)
    {
        qInfo() << "Stopping";
        PyErr_SetString(PyExc_KeyboardInterrupt, "User stopped the execution.");
    }

    return 0;
}


int python_quit(void *)
{
    qInfo() << "Called";
    PyErr_SetString(PyExc_KeyboardInterrupt, "User stopped the execution.");

    exited = true;
    return 0;
}

python_runner::python_runner()
{
    // The table of built-in modules outlives Py_Finalize(), so "emb" is appended
    // once per process however many runners get created.
    static bool embRegistered = false;
    if(!embRegistered)
    {
        PyImport_AppendInittab("emb", emb::PyInit_emb);
        embRegistered = true;
    }

    Py_Initialize();

    pymodule = PyImport_ImportModule("emb");    // new reference
    main = PyImport_AddModule("__main__");      // borrowed
    Py_XINCREF(main);                           // ours from here until the destructor
}

std::string python_runner::formatPythonException(PyObject *exceptionObj, PyObject *valueObj, PyObject *tracebackObj)
{
    PyErr_NormalizeException(&exceptionObj, &valueObj, &tracebackObj);

    PyObjectHandle exception(exceptionObj);
    PyObjectHandle value(valueObj);
    PyObjectHandle traceback(tracebackObj);

    // value and traceback may be null
    if (!value)
    {
        value.reset(PyObjectHandle::INCREF, Py_None);
    }
    if (!traceback)
    {
        traceback.reset(PyObjectHandle::INCREF, Py_None);
    }

    PyObjectHandle tbModule(PyImport_ImportModule("traceback"));
    if (!tbModule)
    {
        throw funkplot::syntax_error_exception(ERRORCODE(68), "Python internal error: import traceback failed");
    }

    PyObject* tbDict = PyModule_GetDict(tbModule.get());  // borrowed
    if (!tbDict)
    {
        throw funkplot::syntax_error_exception(ERRORCODE(68), "Python internal error: no dict in traceback module");
    }

    // borrowed
    PyObject* formatFunc = PyDict_GetItemString(tbDict, "format_exception");
    if (!formatFunc)
    {
        throw funkplot::syntax_error_exception(ERRORCODE(68), "Python internal error: no format_exception in traceback module");
    }

    PyObjectHandle formatted(PyObject_CallFunction(
        formatFunc, const_cast<char*>("OOO"), exception.get(), value.get(),
        traceback.get()));
    if (!formatted)
    {
        throw funkplot::syntax_error_exception(ERRORCODE(68), "Python internal error: traceback.format_exception error");
    }

    if (!PyList_Check(formatted.get()))
    {
        throw funkplot::syntax_error_exception(ERRORCODE(68), "Python internal error: traceback.format_exception didn't return a list");
    }

    auto fg = formatted.get();
    int tbls = PyList_GET_SIZE(fg);
    std::string out;
    for (Py_ssize_t i = 0; i < tbls; ++i)
    {
        PyObject* obj = PyList_GET_ITEM(fg, i);  // borrowed

        PyObjectHandle exception_string(PyObject_Str(obj));
        if(exception_string)
        {
            // points into exception_string, so it must not outlive the handle
            const char* data = PyUnicode_AsUTF8(exception_string.get());
            if(data) out += data;
        }
    }

    return out;
}

std::string python_runner::pyObjectToString(PyObject *b)
{
    if(!strcmp(Py_TYPE(b)->tp_name, "Point"))
    {
        std::string result;
        if(PyObject_HasAttrString(b, X))
        {
            PyObject* ax = PyObject_GetAttrString(b, X);
            std::string v = pyObjectToString(ax);
            result = "x:" + v;
            Py_DECREF(ax);
        }
        if(PyObject_HasAttrString(b, Y))
        {
            PyObject* ay = PyObject_GetAttrString(b, Y);
            std::string v = pyObjectToString(ay);
            result += "&y:" + v;
            Py_DECREF(ay);
        }
        return result;
    }
    else
        if(!strcmp(Py_TYPE(b)->tp_name, "dict"))
        {
            if(PyDict_Check(b))
            {
                std::string result;
                PyObjectHandle keys(PyDict_Keys(b));
                int s = PyList_Size(keys.get());
                for (int i = 0; i < s; ++i)
                {
                    PyObject* a = PyList_GetItem(keys.get(), i);
                    if(a)
                    {
                        PyObjectHandle keyname_a(PyUnicode_AsEncodedString( a, "UTF-8", "strict" ));
                        if(keyname_a)
                        {
                            std::string k = PyBytes_AS_STRING( keyname_a.get() );
                            PyObject* b_b = PyDict_GetItem(b, a);
                            std::string v = pyObjectToString(b_b);
                            result += k + ":" + v;
                            if(i < s - 1)
                            {
                                result += "&";
                            }
                        }
                    }
                }
                return result;
            }
        }
        else
            if(!strcmp(Py_TYPE(b)->tp_name, "str"))
            {
                PyObjectHandle temp_bytes_b(PyUnicode_AsEncodedString( b, "UTF-8", "strict" ));
                if (temp_bytes_b)
                {
                    std::string v = PyBytes_AS_STRING( temp_bytes_b.get() );
                    return v;
                }
            }
            else if(!strcmp(Py_TYPE(b)->tp_name, "float"))
            {
                if(PyFloat_Check(b))
                {
                    double temp_= PyFloat_AsDouble( b);
                    return toString(temp_);
                }
            }
            else if(!strcmp(Py_TYPE(b)->tp_name, "int"))
            {
                if(PyLong_Check(b))
                {
                    long temp_= PyLong_AsLong(b);
                    return toString(temp_);
                }
            }

    return "";
}

std::string python_runner::run(const std::map<std::string, std::string> &kps, std::string between, std::map<std::string, std::string> &all_variables)
{
    exited = false;

    // Declared before the cleanup object below so that it outlives it: the stdout
    // redirection writes into this buffer and the cleanup is what uninstalls it.
    std::string buffer;

    PyGILState_STATE state = PyGILState_Ensure();

    emb::stdout_write_type write = [&buffer] (const std::string& s) { buffer += s; };
    emb::set_stdout(write);

    const python_run_cleanup cleanup(state);

    // generate a list of assignments to kp/kv values from the kps vector
    std::set<std::string> set_variables;
    for(const auto& [kp, kv] : kps)
    {
        std::string cmd = kp + "=" + kv + "\n";
        between = cmd + between;
        set_variables.insert(kp);
    }


    between = std::string("        self.y = y\n") + between;
    between = std::string("        self.x = x\n") + between;
    between = std::string("    def __init__(self, x, y):\n") + between;
    between = std::string("class Point(object):\n") + between;

    qInfo() << "TO RUN:\n" << between.c_str();

    //Py_AddPendingCall(&python_exiter, NULL);

    PyObjectHandle co(Py_CompileString(between.c_str(), "<stdin>", Py_file_input));
    if (!co)
    {
        if (PyErr_Occurred())
        {
            PyObject *ptype, *pvalue, *ptraceback;
            PyErr_Fetch(&ptype, &pvalue, &ptraceback);

            return formatPythonException(ptype, pvalue, ptraceback);
        }
    }

    // PyModule_GetDict returns a borrowed reference. Taking it without the INCREF
    // made this handle decref __main__'s dict on the way out without ever owning
    // it, leaving the dict one reference short; Py_Finalize()'s collection then
    // freed it while the module still pointed at it and crashed walking it.
    PyObjectHandle globals (PyObjectHandle::INCREF, PyModule_GetDict(main));
    PyObjectHandle ret(PyEval_EvalCode(co.get(), globals.get(), globals.get()));

    if (PyErr_Occurred())
    {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);

        return formatPythonException(ptype, pvalue, ptraceback);
    }

    if(globals)
    {
        PyObjectHandle keys(PyDict_Keys(globals.get()));
        int s = PyList_Size(keys.get());
        for (int i = 0; i < s; ++i)
        {
            PyObject* a = PyList_GetItem(keys.get(), i);
            if(a)
            {
                PyObjectHandle keyname_a(PyUnicode_AsEncodedString( a, "UTF-8", "strict" ));
                if(keyname_a)
                {
                    std::string k = PyBytes_AS_STRING( keyname_a.get() );

                    PyObject*b = PyDict_GetItem(globals.get(), a);

                    if(b)
                    {

                        qDebug() << " ................ b:" << Py_TYPE(b)->tp_name ;

                        if(!strcmp(Py_TYPE(b)->tp_name, "list"))
                        {
                            if(PyList_Check(b))
                            {
                                std::vector<std::string> tmp;
                                Py_ssize_t n = PyList_Size(b);
                                PyObject* item;
                                for (int i = 0; i < n; i++)
                                {
                                    item = PyList_GetItem(b, i);
                                    tmp.push_back(pyObjectToString(item));
                                }
                                if(!tmp.empty())
                                {
                                    all_variables[k] = toString(tmp, ",");
                                }
                            }

                        }
                        else
                        {
                            all_variables[k] = pyObjectToString(b);
                        }

                    }
                }

            }
            else
            {
                qDebug() << " no A ";
            }
        }

        qDebug() << "............Done";

    }

    return buffer;
}

python_runner::~python_runner()
{
    // Py_Initialize() left the GIL held by this thread, so the interpreter is still
    // ours to touch here.
    Py_CLEAR(pymodule);
    Py_CLEAR(main);

    qDebug() << "Finalizing";
    Py_Finalize();
}
