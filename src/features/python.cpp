#include "common.h"

extern PyModuleDef PlayerModule;
extern PyModuleDef EntModule;
extern PyModuleDef PpnixModule;
extern PyModuleDef EventModule;
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

static void pythonLoadUserScripts(void);


PyObject *pGlobals;
void pythonInit(void)
{
	// Register the module with the interpreter check return value

	// if ( PyImport_AppendInittab("sof_event", &PyInit_c_decorator_events) == -1 )
	// {
	// 	error_exit("Failed to add c_decorator_events to interpreter");
	// 	return;
	// }

	// clean up previous.
	Py_Finalize();
	Py_Initialize();

	// Add the current directory to script load path
	PyObject* sysPath = PySys_GetObject("path");//b
	PyObject* curDir = PyUnicode_FromString(".");//n
	PyList_Append(sysPath, curDir);//b
	Py_DECREF(curDir);

	initEntDictType();
	MemoryItem::initMemoryItems();
	
	PyObject* mod_ppnix = PyModule_Create(&PpnixModule);
	PyObject* mod_event = PyModule_Create(&EventModule);

	PyObject* mod_ent = PyModule_Create(&EntModule);
	PyObject* mod_player = PyModule_Create(&PlayerModule);


// ----------------------------------------------------------------

	pGlobals = PyDict_New();//n
	PyDict_SetItemString(pGlobals, "ppnix", mod_ppnix);
	PyDict_SetItemString(pGlobals, "event", mod_event);

	PyDict_SetItemString(pGlobals, "ent", mod_ent);
	PyDict_SetItemString(pGlobals, "player", mod_player);
		
// ----------------------------------------------------------------

	// Clear Python callbacks.
	removeDecoratorCallbacks();
	pythonLoadUserScripts();
}
static int functionExists(const char* functionName, PyObject* pGlobals) {
	// Check if the function exists in the current namespace or global namespace
	PyObject* pFunc = PyDict_GetItemString(pGlobals, functionName);
	if (pFunc == NULL) {
		PyErr_Clear();  // Clear the error if the function doesn't exist
		return 0;       // Function does not exist
	}

	// Function exists
	Py_DECREF(pFunc);
	return 1;
}

/*
	Loads all .py python scripts within user/python directory.
	Any decorator defined in these files is appended to the decorator callbacks.
	And removed at the end of each map, so you can edit the python file inbetween maps.
	And they are reloaded at the start of each map again.
*/
static void pythonLoadUserScripts(void)
{


	std::string python_folder(orig_FS_Userdir() + std::string("/python/"));
	// std::vector<std::string> pythonFiles;
	DIR* dir;
	struct dirent* entry;
	dir = opendir(python_folder.c_str());
	if (dir == NULL) {
		// Dir probably doesnt' exist.
		create_file_dir_if_not_exists(python_folder.c_str());

		// Successfully created directory, open it
		dir = opendir(python_folder.c_str());
		if (dir == NULL) {
			error_exit("Error opening python directory");
		}
	}

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_REG && strcmp(entry->d_name + strlen(entry->d_name) - 3, ".py") == 0) {
			// Construct the full file path
			char file_path[PATH_MAX];
			snprintf(file_path, sizeof(file_path), "%s%s", python_folder.c_str(), entry->d_name);  // Replace "directory_path" with the actual directory path

			// Load the Python file
			FILE *fp = fopen(file_path, "r");
			if (fp != NULL) {
				// Do something with python file.
				// (FILE *fp, const char *filename, int start, PyObject *globals, PyObject *locals)
				PyObject *pResult = PyRun_File(fp, file_path, Py_file_input, pGlobals, NULL);
				if (pResult == NULL) {
					// An error occurred
					PyErr_Print();  // Print the error message
					PyErr_Clear();  // Clear the error
					// Handle the error or take appropriate actions
				}


				fclose(fp);
				
			} else {
				// Failed to open the file
				error_exit("Problem opening one of the python script files in python userdir.");
			}
		}
	}
	closedir(dir);

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
	// We want to keep this. TODO: cleanup callback if remove.
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