#include "common.h"

/*
	https://docs.python.org/3.8/c-api/arg.html?highlight=pyarg_parsetuple#c.PyArg_ParseTuple
	Then scroll up, for ParseTuple symbols.

	It's the default assumption that the reference passed to a C function is borrowed, 
	and if the function needs to save a reference, it will increment the refcount appropriately
	
	For returned objects see the docs for that API, whether it returns new reference.
	https://docs.python.org/3.8/c-api/list.html

	Always return Py_RETURN_NONE when nothing is returned.
*/

/*
ppnix.print("Hello World")
*/

static PyObject * py_ppnix_print(PyObject * self, PyObject * args);
static PyObject * py_ppnix_get_time(PyObject * self, PyObject * args);
static PyObject * py_ppnix_get_chat_vectors(PyObject * self, PyObject * args);
static PyObject * py_ppnix_register_configstring_ghoul(PyObject * self, PyObject * args);
static PyObject * py_ppnix_read_cvar(PyObject * self, PyObject * args);


//name,c_func,flags,docstring
static PyMethodDef PpnixMethods[] = {
	{"print", py_ppnix_print, METH_VARARGS,"Print to console."},
	{"get_time", py_ppnix_get_time, METH_VARARGS,"Get level time."},
	{"get_chat_vectors",py_ppnix_get_chat_vectors,METH_VARARGS,"Get chat vectors."},
	{"register_file",py_ppnix_register_configstring_ghoul,METH_VARARGS,"Register a file for download as ghoul configstring. Must be in a sub-directory."},
	{"read_cvar",py_ppnix_read_cvar,METH_VARARGS,"Read the cvar as a string."},
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
	x - how many recent chat vectors.
*/

static PyObject * py_ppnix_get_chat_vectors(PyObject * self, PyObject * args)
{
	unsigned int x;
	if (!PyArg_ParseTuple(args,"I",&x)) {
		error_exit("Python: Failed to parse args for py_ppnix_get_chat_vectors");
	}

	PyObject* pyList = PyList_New(0);
	int startIndex = chatVectors.size() - x;
	if (startIndex < 0 ) startIndex = 0;
	// SOFPPNIX_DEBUG("startIndex = %i",startIndex);
	for (int i = startIndex; i < chatVectors.size(); ++i) {
		// SOFPPNIX_DEBUG(" i= %i",i);
		// PyObject* line = PyUnicode_DecodeFSDefault(chatVectors[i].c_str());
		PyObject * line = PyUnicode_Decode(chatVectors[i].c_str(),chatVectors[i].length(),"latin1","strict");
		// PyObject* line = Py_BuildValue("y", chatVectors[i].c_str());
		// if ( line == NULL ) {
		// 	error_exit("Unable to build string in get_chat_vectors");
		// }
		PyList_Append(pyList, line);
		Py_DECREF(line);
	}

	return pyList;
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

/*
	s - normal c NULL terminated string [const char *]
		const char *name;
	    if (!PyArg_ParseTuple(args, "s", &name)) {
	        return NULL; // Error parsing arguments
	    }
	    printf("Hello, %s!\n", name);
	    Py_RETURN_NONE;
	s* - str or bytes like [Py_buffer]
	    Py_buffer buf;
        if (!PyArg_ParseTuple(args, "s*", &buf)) {
            return NULL;
        }
        printf("Received data of length %zd: ", buf.len);
        for (Py_ssize_t i = 0; i < buf.len; ++i) {
            printf("%02x ", ((unsigned char*)buf.buf)[i]);
        }
        printf("\n");
        PyBuffer_Release(&buf); // Important: release the buffer when done
        Py_RETURN_NONE;
	s# - READONLY. str or bytes like Py_buffer [const char *, int or Py_ssize_t]
	    Like s*, this format allows for embedded null bytes within the data.
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

static PyObject * py_ppnix_register_configstring_ghoul(PyObject * self, PyObject * args)
{
	const char * filename;
	if (!PyArg_ParseTuple(args, "s", &filename)) {
	    return NULL; // Error parsing arguments
	}
	orig_SV_GhoulFileIndex(filename);
	Py_RETURN_NONE;
}

static PyObject * py_ppnix_read_cvar(PyObject * self, PyObject * args)
{
    const char * cvarname;
    const char * cvar_value; // To hold the C string result

    if (!PyArg_ParseTuple(args, "s", &cvarname)) {
        return NULL; // Error parsing arguments
    }

    // Call your original C function
    cvar_value = orig_Cvar_VariableString(cvarname);

    // Check if the C function returned a valid string
    if (cvar_value == NULL) {
        // Depending on what orig_Cvar_VariableString does on failure,
        // you might want to raise a Python exception here.
        // For now, let's return Py_None, indicating no value.
        Py_RETURN_NONE;
    }

    // Convert the C string to a Python string object
    return PyUnicode_Decode(cvar_value, strlen(cvar_value), "latin-1", "ignore");
    // Alternatively, using Py_BuildValue:
    // return Py_BuildValue("s", cvar_value);
}