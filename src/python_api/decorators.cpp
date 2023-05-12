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

static PyObject* py_on_frame_early(PyObject* self,PyObject* args);

static PyObject* py_on_map_spawn(PyObject* self,PyObject* args);

//name,c_func,flags,docstring
static PyMethodDef EventMethods[] = {
		{"die", py_on_player_die, METH_VARARGS, "player die event decorator."},
		{"connect", py_on_player_connect, METH_VARARGS, "player connect event decorator."},
		{"disconnect", py_on_player_disconnect, METH_VARARGS, "player disconnect event decorator."},
		{"respawn", py_on_player_respawn, METH_VARARGS, "player respawn event decorator."},

		{"frame_early", py_on_frame_early, METH_VARARGS, "early game frame."},
		{"map_spawn", py_on_map_spawn, METH_VARARGS, "map spawned."},
		{NULL, NULL, 0, NULL} //sentinel
};
//base,name,doc,size,methods
PyModuleDef EventModule = {
	PyModuleDef_HEAD_INIT, "events", "decorator event callbacks", -1, EventMethods,
	NULL, NULL, NULL, NULL //sentinel
};

/*
	Below are decorators of the form :
	@event.connect
	def connect_original(ent):
		print("CONNECT")
*/

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

std::vector<PyObject*> frame_early_callbacks;
static PyObject* py_on_frame_early(PyObject* self,PyObject* args) {
	return register_callback("frame_early",frame_early_callbacks,args);
}

std::vector<PyObject*> map_spawn_callbacks;
static PyObject* py_on_map_spawn(PyObject* self,PyObject* args) {
	return register_callback("map_spawn",map_spawn_callbacks,args);
}