#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>

void call_python_func(PyObject* param, char* module_name, char* func_name) {
  setenv("PYTHONPATH", ".", 1);
  PyObject *p_name, *p_module, *p_dict, *p_func, *presult;
  
  Py_Initialize();
  p_name = PyUnicode_FromString(module_name);
  p_module = PyImport_Import(p_name);
  p_dict = PyModule_GetDict(p_module);
  p_func = PyDict_GetItemString(p_dict, func_name);

  if (PyCallable_Check(p_func)) {
    PyErr_Print();
    presult = PyObject_CallObject(p_func, param);
    PyErr_Print();
  } else {
    PyErr_Print();
  }

  Py_DECREF(p_module);

  Py_FinalizeEx();
}
