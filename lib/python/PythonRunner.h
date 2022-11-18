#ifndef PYTHON_RUNNER_H
#define PYTHON_RUNNER_H

#undef slots

#include "pyembed.h"

#include <map>
#include <string>
#include <set>

#if 0
            PyObject *argument_size       = NULL;
            PyObject *exception_string    = NULL;
            PyObject *exception_traceback = NULL;
            PyObject *exception_type      = NULL;
            char *error_string            = NULL;
            char *type_string            = NULL;
            char *tb_string            = NULL;

            exception_string = PyObject_Repr(pvalue);
            exception_traceback = PyObject_Repr(ptraceback);
            exception_type = PyObject_Repr(ptype);

            type_string = (char*)PyUnicode_AsUTF8(exception_type);
            tb_string = (char*)PyUnicode_AsUTF8(exception_traceback);
            error_string = (char*)PyUnicode_AsUTF8(exception_string);

#endif

int python_quit(void *);

struct python_runner
{
    explicit python_runner();
    virtual ~python_runner();

    std::string formatPythonException(PyObject* exceptionObj, PyObject* valueObj, PyObject* tracebackObj);
    std::string pyObjectToString(PyObject* b);
    std::string run(const std::map<std::string, std::string>& kps, std::string between, std::map<std::string, std::string>& all_variables);

private:
    PyObject* pymodule = nullptr;
    PyObject *main = nullptr;

};

#endif // PYTHON_RUNNER_H
