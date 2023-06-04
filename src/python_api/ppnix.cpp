#include "common.h"

/*
ppnix.print("Hello World")
*/

static PyObject * py_ppnix_print(PyObject * self, PyObject * args);
static PyObject * py_ppnix_get_time(PyObject * self, PyObject * args);


//name,c_func,flags,docstring
static PyMethodDef PpnixMethods[] = {
	{"print", py_ppnix_print, METH_VARARGS,"Print to console."},
	{"get_time", py_ppnix_get_time, METH_VARARGS,"Get level time."},
	{NULL, NULL, 0, NULL} //sentinel
};

//base,name,docstring,size,methods
PyModuleDef PpnixModule = {
	PyModuleDef_HEAD_INIT, "ppnix", "a module containing generic util functions.", -1, PpnixMethods,
	NULL, NULL, NULL, NULL //sentinel
};
// Define the module
PyMODINIT_FUNC PyInit_ppnix(void) {

	return PyModule_Create(&PpnixModule);
}


/*
	Where should this 'timer' callback be called from?
	After entity game processing?
	before entity game processing?
	Start of frame?

	Before SV_Frame NET_Sleep = reading packets. NET_Sleep = wake up if packet received.

	What if the ent values are used inbetween server-frames within ClientThink?

	SV_RunGameFrame @sv_main.c
	G_RunFrame @g_main.c

	Issue/Problem: Setting some ent variables will not have impact, because they are overridden/set afterwards.

	Lets try just allowing a precision of callback every X server frames. Max = Every Server Frame.

	event.frame_begin ?

*/


static PyObject * py_ppnix_print(PyObject * self, PyObject * args)
{
	char * msg;
	Py_ssize_t length;
	// Not null-terminated.
	if (!PyArg_ParseTuple(args,"s#",&msg,&length)) {
		error_exit("Python: Failed to parse args in print");
	}
	orig_Com_Printf("[Python] %.*s\n",(int)length,msg);
	Py_RETURN_NONE;
}

static PyObject * py_ppnix_get_time(PyObject * self, PyObject * args)
{
	void * level = stget(base_addr + 0x002ACB1C,0);
	float * ptime = level+4;
	PyObject * ret = PyFloat_FromDouble(*ptime);
	if ( ret == NULL ) {
		error_exit("Python: Failed to create return value");
	}
	return ret;
}