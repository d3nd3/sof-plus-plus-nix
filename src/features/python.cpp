#include "common.h"

extern PyModuleDef EntModule;
/*
s: C string to str
s#: C string, length to str
y: C string to bytes
y#: C string, length to bytes
z: C string to str
z#: C string, length to str
u: Unicode buffer to str
u#: Unicode buffer, length to str
U: C string to str
U#: C string, length to str
i: int to int
b: char to int
h: short int to int
l: long int to int
B: unsigned char to int
H: unsigned short int to int
I: unsigned int to int
k: unsigned long to int
L: long long to int
K: unsigned long long to int
n: Py_ssize_t to int
c: int to bytes
C: int to str
d: double to float
f: float to float
D: Py_complex to complex
O: object to object
S: object to object
N: object to object (no ref count)
O&: anything to object (via converter)
(items): C values to tuple
[items]: C values to list
{items}: C values to dict
*/
/*
	Python decorators are 
	ordinary = decorateMe(ordinary)

	@decorateMe
	def ordinary:

	so decorateMe is meant to return an inner function which calls the ordinary function.

	I thought to use this to register callbacks.
	decorateMe would be an internal c function which returns a function which error exits because it shouldn't be called by them.
	eg.
	@player_die
	def on_die(ent):
		pass

	The internal player_die function would register the on_die function to a std::vector of callbacks.
	Then when the c realm calls the player_die function, it would iterate through the std::vector and call each callback.
*/
void pythonInit(void)
{
	// Register the module with the interpreter check return value

	// if ( PyImport_AppendInittab("sof_event", &PyInit_c_decorator_events) == -1 )
	// {
	// 	error_exit("Failed to add c_decorator_events to interpreter");
	// 	return;
	// }
	Py_Initialize();

	// Add the current directory to script load path
	PyObject* sysPath = PySys_GetObject("path");//b
	PyObject* curDir = PyUnicode_FromString(".");//n
	PyList_Append(sysPath, curDir);//b
	Py_DECREF(curDir);


/*
	If docs say :
	Return value: New reference
	Then you have to call DECREF on it. When done.
*/	
// ----------------------------------------------------------------
	PyObject* sof_events_module = PyInit_c_decorator_events();//n

	// Add sof_events module to the built-in sys.modules dictionary
	PyObject* sys_dict = PyImport_GetModuleDict();//b
	if ( PyDict_SetItemString(sys_dict, "events", sof_events_module) == -1 ) { //b
		error_exit("Failed to add sof_events module to sys.modules");
		return;
	}

	PyObject *pGlobals = PyDict_New();//n
	PyDict_SetItemString(pGlobals, "event", sof_events_module);

	// PyObject *pModuleDict = PyModule_GetDict(sof_events_module);//b

	// // Iterate over the items of pModuleDict
	// PyObject *key, *value;
	// Py_ssize_t pos = 0;
	// while (PyDict_Next(pModuleDict, &pos, &key, &value)) { //b
	// 	// Check if the value is a function
	// 	if (PyCallable_Check(value)) {
	// 		// Get the key as a C string
	// 		const char *name = PyUnicode_AsUTF8(key);//tied to key.
	// 		// Add the value to the global dictionary with the same key
	// 		PyDict_SetItemString(pGlobals, name, value); //b
	// 	}
	// }

	
// ----------------------------------------------------------------
	// Open the Python file
	FILE *fp = fopen("sofppnix.py", "r");
	// (FILE *fp, const char *filename, int start, PyObject *globals, PyObject *locals)
	PyObject *pResult = PyRun_File(fp, "sofppnix.py", Py_file_input, pGlobals, NULL);
	fclose(fp);

	Py_DECREF(pGlobals);
	Py_DECREF(sof_events_module);


	MemoryItem::initMemoryItems();
}

/*
	PYTHON REALM SUMMONING C REALM IS IN THE PYTHON_API FOLDER.
*/



/*
	Python decorator at init calls this and passes the original python function to be saved for later.
*/
PyObject* register_callback(const char * name ,std::vector<PyObject*> &callbacks,PyObject* args) {
	PyObject * callback = NULL;
	// RefCount NOT increased
	if (!PyArg_ParseTuple(args,"O",&callback)) {
		error_exit("Python: Failed to parse args for %s",name);
		return NULL;
	}
	SOFPPNIX_DEBUG("Python: Registering %s callback",name);
	callbacks.push_back(callback);

	// const char * func_type_name = Py_TYPE(callback)->tp_name;
	// SOFPPNIX_DEBUG("Argument type: %s", func_type_name);

	// Py_RETURN_NONE;
	// We want to keep this.
	Py_INCREF(callback);
	return callback;
}


void list_refcount(void)
{
	// Import the sys module
	PyObject *sys_module = PyImport_ImportModule("sys");//n
	if (!sys_module) {
		PyErr_Print();
		return 1;
	}
	// Get a pointer to the gettotalrefcount() function
	PyObject *gettotalrefcount_func = PyObject_GetAttrString(sys_module, "gettotalrefcount");//n
	if (!gettotalrefcount_func || !PyCallable_Check(gettotalrefcount_func)) {
		PyErr_Print();
		return 1;
	}

	// Call the gettotalrefcount() function
	PyObject *result = PyObject_CallObject(gettotalrefcount_func, NULL);//n
	if (!result) {
		PyErr_Print();
		return 1;
	}

	// Print the result
	printf("Total reference count: %ld\n", PyLong_AsLong(result));

	// Clean up
	Py_DECREF(result);
	Py_DECREF(gettotalrefcount_func);
	Py_DECREF(sys_module);
}