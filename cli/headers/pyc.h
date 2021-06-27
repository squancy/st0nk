#ifndef PYC_H
#define PYC_H

#include <Python.h>

void call_python_func(PyObject* param, char* module_name, char* func_name);

#endif
