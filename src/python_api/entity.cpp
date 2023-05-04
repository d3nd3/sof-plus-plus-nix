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


/*
----------------------------------------CUSTOM ENT DICT--------------------------------------
*/

#if 0
typedef struct {
	PyDictObject dict; // inherit from PyDictObject
	// add any custom fields here
} EntDict;

// define a custom __setitem__ method
static int
EntDict_setitem(EntDict *self, PyObject *key, PyObject *value)
{
	// print a message when an item is added
	printf("Adding item to EntDict: %s\n", PyUnicode_AsUTF8(key));
	// call the base class method
	return PyDict_Type.tp_as_mapping->mp_ass_subscript((PyObject *)self, key, value);
}

// define the type slots
static PyType_Slot EntDict_slots[] = {
	// {Py_tp_base, &PyDict_Type}, // set the base class to PyDict_Type
	{Py_mp_ass_subscript, EntDict_setitem}, // override the __setitem__ method
	{0, 0}
};

// define the type specification
static PyType_Spec EntDict_spec = {
	"EntDict", // the name of the type
	sizeof(EntDict), // the size of the type object
	0, // the size of the type object
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_DICT_SUBCLASS, // base flags,
	EntDict_slots // the slots
};

// create the type object
static PyObject *
EntDict_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	// calls PyType_Ready internally.
	// return PyType_FromSpec(&EntDict_spec);
	PyObject * bases = PyTuple_Pack(1, &PyDict_Type);
	return PyType_FromSpecWithBases(&EntDict_spec,bases);
}
#else
typedef struct {
	// order matters here. dict is at top.
	// PyObject_HEAD
	PyDictObject dict;
} MyDict;


static PyObject* MyDict_get(PyObject *self, PyObject *key) {
	return PyDict_GetItem(self, key);
}

static int MyDict_set(PyObject *self, PyObject *key, PyObject *value) {
	return PyDict_SetItem(self, key, value);
}

static PyMethodDef MyDict_methods[] = {
	// {"get", (PyCFunction)MyDict_get, METH_O, "Get a value from the dictionary."},
	// {"set", (PyCFunction)MyDict_set, METH_VARARGS, "Set a value in the dictionary."},
	{NULL,NULL}  /* Sentinel */
};

/*
	constructor.
	yourClass() = _call ... _new ... _init
*/
static int MyDict_init(MyDict *self, PyObject *args, PyObject *kwds) {
	if (PyDict_Type.tp_init((PyObject *)self, args, kwds) < 0)
			return -1;
		// self->state = 0;
	return 0;
}

static PyTypeObject MyDict_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	"MyDict",             /* tp_name */
	sizeof(MyDict), /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,                         /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_DICT_SUBCLASS,        /* tp_flags */
	"Noddy objects",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	MyDict_methods,             /* tp_methods */
	0,             				/* tp_members */
	0,                         /* tp_getset */
	&PyDict_Type,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)MyDict_init,      /* tp_init */
	0,                         /* tp_alloc */
	0,                 /* tp_new */
};
#endif


/*
-------------------------------------CREATE THE ENT DICT INSTANCE-----------------------------------------
PyObject* l = PyLong_FromLong(123)
	// Finally, add some items to the dictionary
	PyDict_SetItemString((PyObject*)ent_dict, "lookupthiskey", );
*/
#if 0
static void addEntProperties(EntDict* ent_dict,edict_t * backing_ent)
{
	// Read from the entities memory using get() and store inside the ent dict.
	for( const auto &pair : MemoryItem::entityProperties ) {
		PyObject* val = pair.second->get_py(backing_ent);
		PyDict_SetItemString((PyObject *)ent_dict, pair.first.c_str() , val);//b
		Py_DECREF(val);
	}
}
#endif
PyObject* createEntDict(edict_t * backing_ent)
{
	MyDict_Type.tp_base = &PyDict_Type;
	std::cout << "Further0" << std::endl;
	if (PyType_Ready(&MyDict_Type) < 0) {
		PyErr_Print();
		error_exit("Failed to Type Ready");
	}
	// Py_INCREF(&MyDict_Type);
		
	std::cout << "Further1" << std::endl;
	// PyObject* ent_dict = MyDict_new(&MyDict_Type, NULL, NULL);

	PyObject *ent_dict = PyObject_CallObject((PyObject *) &MyDict_Type, NULL);


	// Create the ent_dict instance
	// PyObject* ent_dict = EntDict_new(NULL, NULL, NULL);
	// if ( ent_dict == NULL ) {
	// 	PyErr_Print();
	// 	error_exit("Failed to create ent dict");
	// }
	// PyObject* real_dict = PyDict_New();
	// if ( real_dict == NULL ) {
	// 	PyErr_Print();
	// 	error_exit("Failed to create real_dict");
	// }
	std::cout << "Further2" << std::endl;
	// Insert the backing_ent into the dictionary
	PyObject *value = PyCapsule_New(backing_ent, NULL, NULL);
	if ( value == NULL ) {
		PyErr_Print();
		error_exit("Failed to create capsule");
	}

	// Generate an integer
	// PyObject* l = PyLong_FromLong(123);

	/*
		So its not the value that is the problem.
		Its not the key name that is the problem.
		Its the ent_dict.

		PyDict_Check returning false.
	*/
	std::cout << "Further3" << std::endl;
	
	//b
	// Py_INCREF(value);
	// Py_INCREF(ent_dict);
	if ( PyDict_SetItemString(ent_dict, "entent", value) == -1 ) {
		// PyErr_SetString(PyExc_RuntimeError, "Failed to set __ent__");
		std::cout << "BAD!" << std::endl;
		PyErr_Print();
		error_exit("Failed to set __ent__");
	}
	// Py_DECREF(value);

	std::cout << "Further4" << std::endl;
	// Add every other ent specific object
	// addEntProperties(ent_dict,backing_ent);

	// Py_DECREF(ent_dict);
	return ent_dict;
}