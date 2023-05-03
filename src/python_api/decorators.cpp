#include "common.h"

/*
	THIS MODULE IS INSERTED INTO GLOBAL DICT DIRECTLY.
	C REALM CALLS PYTHON FUNCTION THAT HAVE BEEN REGISTERED BECAUSE OF CALLBACKS REGISTERED HERE. ( THEY USED DECORATORS TO REGISTER THEIR CALLBACKS in python_api/decorators.cpp )

	DECORATORS == REGISTER CALLBACK.
*/

static PyObject* py_on_player_die(PyObject* self,PyObject* args);
static PyObject* py_on_player_respawn(PyObject* self,PyObject* args);
static PyObject* py_on_player_connect(PyObject* self,PyObject* args);
static PyObject* py_on_player_disconnect(PyObject* self,PyObject* args);

// func names
static PyMethodDef c_decorator_events_methods[] = {
		{"die", py_on_player_die, METH_VARARGS, "player die event decorator."},
		{"connect", py_on_player_connect, METH_VARARGS, "player connect event decorator."},
		{"disconnect", py_on_player_disconnect, METH_VARARGS, "player disconnect event decorator."},
		{"respawn", py_on_player_respawn, METH_VARARGS, "player respawn event decorator."},
		{NULL, NULL, 0, NULL}
};
// module name
PyModuleDef c_decorator_events = {
	PyModuleDef_HEAD_INIT, "sof_events", "decorator event callbacks", -1, c_decorator_events_methods,
	NULL, NULL, NULL, NULL
};


std::vector<PyObject*> player_die_callbacks;
static PyObject* py_on_player_die( PyObject* self,PyObject* args) {
	return register_callback("die",player_die_callbacks,args);
}
std::vector<PyObject*> player_connect_callbacks;
static PyObject* py_on_player_connect( PyObject* self,PyObject* args) {
	return register_callback("connect",player_connect_callbacks,args);
}
std::vector<PyObject*> player_disconnect_callbacks;
static PyObject* py_on_player_disconnect(PyObject* self,PyObject* args) {
	return register_callback("disconnect",player_disconnect_callbacks,args);
}
std::vector<PyObject*> player_respawn_callbacks;
static PyObject* py_on_player_respawn(PyObject* self,PyObject* args) {
	return register_callback("respawn",player_respawn_callbacks,args);
}
/*
	Python Interpreter is aware of c function symbol names apparently.
*/
PyMODINIT_FUNC PyInit_c_decorator_events(void) {
	return PyModule_Create(&c_decorator_events);
}