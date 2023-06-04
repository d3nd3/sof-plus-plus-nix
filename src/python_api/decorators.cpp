#include "common.h"

/*
	THIS MODULE IS INSERTED INTO GLOBAL DICT DIRECTLY.
	C REALM CALLS PYTHON FUNCTION THAT HAVE BEEN REGISTERED BECAUSE OF CALLBACKS REGISTERED HERE. ( THEY USED DECORATORS TO REGISTER THEIR CALLBACKS in python_api/decorators.cpp )

	DECORATORS == REGISTER CALLBACK.

	@event.die
	@event.connect
	@event.disconnect
	@event.respawn
	@event.say
	@event.frame_early
	@event.map_spawn
	@event.page("pagename")
*/

static PyObject* py_on_player_die(PyObject* self,PyObject* args);
static PyObject* py_on_player_respawn(PyObject* self,PyObject* args);
static PyObject* py_on_player_connect(PyObject* self,PyObject* args);
static PyObject* py_on_player_disconnect(PyObject* self,PyObject* args);
static PyObject* py_on_player_say(PyObject* self,PyObject* args);

static PyObject* py_on_frame_early(PyObject* self,PyObject* args);

static PyObject* py_on_map_spawn(PyObject* self,PyObject* args);

static PyObject* py_on_draw_page(PyObject* self,PyObject* args);



//name,c_func,flags,docstring
static PyMethodDef EventMethods[] = {
		{"die", py_on_player_die, METH_VARARGS, "player die event decorator."},
		{"connect", py_on_player_connect, METH_VARARGS, "player connect event decorator."},
		{"disconnect", py_on_player_disconnect, METH_VARARGS, "player disconnect event decorator."},
		{"respawn", py_on_player_respawn, METH_VARARGS, "player respawn event decorator."},
		{"say",py_on_player_say,METH_VARARGS,"player chat event decorator"},

		{"frame_early", py_on_frame_early, METH_VARARGS, "early game frame event decorator."},
		{"map_spawn", py_on_map_spawn, METH_VARARGS, "map spawned event decorator."},
		{"draw_page",py_on_draw_page,METH_VARARGS,"page displayed event decorator."},
		{NULL, NULL, 0, NULL} //sentinel
};
//base,name,doc,size,methods
PyModuleDef EventModule = {
	PyModuleDef_HEAD_INIT, "event", "decorator event callbacks", -1, EventMethods,
	NULL, NULL, NULL, NULL //sentinel
};
PyMODINIT_FUNC PyInit_event(void) {

	return PyModule_Create(&EventModule);
}
/*
	Below are decorators of the form :
	@event.connect
	def connect_original(ent):
		print("CONNECT")
*/
std::vector<std::vector<PyObject*>*> decorators;
std::vector<PyObject*> player_die_callbacks;
std::vector<PyObject*> player_connect_callbacks;
std::vector<PyObject*> player_disconnect_callbacks;
std::vector<PyObject*> player_respawn_callbacks;
std::vector<PyObject*> player_say_callbacks;

std::vector<PyObject*> frame_early_callbacks;
std::vector<PyObject*> map_spawn_callbacks;

void removeDecoratorCallbacks(void)
{
	for ( const auto &decorator : decorators  ) {
		decorator->clear();
	}
}


/*
args:
	ent=dieing_player
	inflictor=rocket
	killer=attacker_player
	
*/
static PyObject* py_on_player_die( PyObject* self,PyObject* args) {
	return register_callback("die",player_die_callbacks,args);
}
/*
args:
	ent=connecting player
*/
static PyObject* py_on_player_connect( PyObject* self,PyObject* args) {
	return register_callback("connect",player_connect_callbacks,args);
}
/*
args:
	ent=disconnecting player
*/
static PyObject* py_on_player_disconnect(PyObject* self,PyObject* args) {
	return register_callback("disconnect",player_disconnect_callbacks,args);
}
/*
args:
	ent=respawning player
*/
static PyObject* py_on_player_respawn(PyObject* self,PyObject* args) {
	return register_callback("respawn",player_respawn_callbacks,args);
}
/*
called by my_SV_RunGameFrame
*/
static PyObject* py_on_frame_early(PyObject* self,PyObject* args) {
	return register_callback("frame_early",frame_early_callbacks,args);
}
/*
called by levelInit
*/
static PyObject* py_on_map_spawn(PyObject* self,PyObject* args) {
	return register_callback("map_spawn",map_spawn_callbacks,args);
}
/*
called by cmd_say_f
*/
static PyObject* py_on_player_say(PyObject* self,PyObject* args) {
	return register_callback("say",player_say_callbacks,args);
}


std::string PAGE_NAME_PASSOVER;
static PyObject* py_on_draw_page_deco(PyObject* self,PyObject* args) {
	// SOFPPNIX_DEBUG("DrawPageDeco2");
	PyObject * callback = NULL;

	if (!PyArg_ParseTuple(args,"O",&callback)) {
		error_exit("Python: Failed to parse args for page decorator");
		return NULL;
	}

	// Only one routine per page. Overwrites.
	py_page_draw_routines[PAGE_NAME_PASSOVER] = callback;

	Py_INCREF(callback);
	return callback;
}
/*
	Slightly different callback usage, a map instead of a vector.
	Instead of calling of them, it selects one, using the current page.

	event.page("chat")
	== (decoratedFN,"chat")


	Apparently, decorator with arguments is equivalent to:
		@decorator_with_args(arg)
		def foo(*args, **kwargs):
			pass

		foo = decorator_with_args(arg)(foo)
*/
// Define a PyMethodDef structure
static PyMethodDef my_module_methods[] = {
	{"py_on_draw_page_deco", py_on_draw_page_deco, METH_VARARGS, "py_on_draw_page_deco"},
	{NULL, NULL, 0, NULL}  // Sentinel indicating the end of the method list
};
static PyObject* py_on_draw_page(PyObject* self,PyObject* args) {
	
	// SOFPPNIX_DEBUG("DrawPageDeco1");
	char * msg;
	Py_ssize_t length;
	if (!PyArg_ParseTuple(args,"s#",&msg,&length)) {
		error_exit("Python: Failed to parse args for page decorator");
		return NULL;
	}
	char input[256];
	snprintf(input,256,"%.*s",(int)length,msg);

	PAGE_NAME_PASSOVER = input;

	// This needs to return another function which will be the decorator without args.
	 // Create a C function that represents a Python function
	PyCFunctionObject* cfunc = (PyCFunctionObject*)PyCFunction_New(&my_module_methods[0], NULL);
	
	// Return the C function object
	return (PyObject*)cfunc;
}