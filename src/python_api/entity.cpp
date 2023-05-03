#include "common.h"

void py_ent_spawn(PyObject * self, PyObject * classname);

static PyMethodDef EntMethods[] = {
	{"spawn", py_ent_spawn, METH_VARARGS,"Spawn a new entity."},

	{NULL, NULL, 0, NULL}
};

PyModuleDef EntModule = {
	PyModuleDef_HEAD_INIT, "ent", "a module for manipulating entities.", -1, EntMethods,
	NULL, NULL, NULL, NULL
};

// Symbol aware function.
PyMODINIT_FUNC PyInit_EntModule(void) {
	return PyModule_Create(&EntModule);
}

// python script calls this. ent_spawn("classname")
void py_ent_spawn(PyObject * self, PyObject * classname)
{
	//parse classname as python string
	char * str = PyUnicode_AsUTF8(classname);


	Py_RETURN_NONE;
}



