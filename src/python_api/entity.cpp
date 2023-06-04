#include "common.h"

/*
c4 = ent.spawn("item_c4")
*/
static PyObject * py_ent_spawn(PyObject * self, PyObject * classname);
static PyObject * py_ent_from_slot(PyObject * self, PyObject * args);
static PyObject * py_ent_get_slot(PyObject * self, PyObject * args);

//name,c_func,flags,docstring
static PyMethodDef EntMethods[] = {
	{"spawn", py_ent_spawn, METH_VARARGS,"Spawn a new entity."},
	{"from_slot", py_ent_from_slot, METH_VARARGS,"Returns an entity from slot."},
	{"get_slot", py_ent_get_slot, METH_VARARGS,"Returns the slot of this entity."},

	{NULL, NULL, 0, NULL}
};
//base,name,doc,size,methods
PyModuleDef EntModule = {
	PyModuleDef_HEAD_INIT, "ent", "a module for manipulating entities.", -1, EntMethods,
	NULL, NULL, NULL, NULL
};

// Symbol aware function.
PyMODINIT_FUNC PyInit_ent(void) {
	return PyModule_Create(&EntModule);
}

static PyObject * py_ent_spawn(PyObject * self, PyObject * args)
{
	edict_t * ent = orig_G_Spawn();


	// PyObject * ret = PyLong_FromVoidPtr(ent);
	PyObject * ret = createEntDict(ent);
	return ret;
	// Py_RETURN_NONE;
}

static PyObject * py_ent_from_slot(PyObject * self, PyObject * args)
{
	unsigned int slot;
	if (!PyArg_ParseTuple(args,"I",&slot)) {
		error_exit("Python: Failed to parse args in py_player_draw_text");
	}

	client_t * client = getClientX(slot);
	int state = *(int*)(client);
	if (state != cs_spawned ) {
		Py_RETURN_NONE;
	}
	edict_t * ent = stget(client,CLIENT_ENT);
	PyObject * ret = createEntDict(ent);
	return ret;
	// Py_RETURN_NONE;
}

static PyObject * py_ent_get_slot(PyObject * self, PyObject * args)
{
	
	EntDict * who;
	if (!PyArg_ParseTuple(args,"O",&who)) {
		error_exit("Python: Failed to parse args in py_ent_get_slot");
	}

	PyObject * ret = PyLong_FromLong(slot_from_ent(who->c_ent));
	if ( ret == NULL ) {
		error_exit("Python: Failed to create return value");
	}
	return ret;
}

/*
----------------------------------------CUSTOM ENT DICT--------------------------------------
*/

// // Structure for defining getter and setter functions
// typedef struct {
// 	const char *name;    // Name of the attribute
// 	getter get;          // Getter function pointer
// 	setter set;          // Setter function pointer
// 	const char *doc;     // Documentation string for the attribute
// 	void *closure;       // Additional data for the getter/setter
// } PyGetSetDef;



// --------------------------------------------------------------------------------------
// EntDict struct moved to common.h


/*
	constructor.
	yourClass() = _call ... _new ... _init
*/
static int EntDict_init(EntDict *self, PyObject *args, PyObject *kwds) {
	if (PyDict_Type.tp_init((PyObject *)self, args, kwds) < 0)
			return -1;
		// self->state = 0;
	return 0;
}

static PyTypeObject EntDict_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	"EntDict",             /* tp_name */
	sizeof(EntDict), /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,                         /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr - tp_getattro using strings.*/
	0,                         /* tp_setattr - tp_setattro using strings.*/
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /*
	 tp_getattro
	  - Get an attribute’s value -
	   __getattribute__ __getattr__
	   Called when an attribute lookup has not found the attribute in the usual places
	*/
	0,                         /* tp_setattro - Set an attribute’s value or remove the attribute. - __setattr__ __delattr__*/
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_DICT_SUBCLASS,        /* tp_flags */
	"SoF1 Entity",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,             /* tp_methods */
	0,             				/* tp_members */
	0,                         /* tp_getset - An optional pointer to a static NULL-terminated array of PyGetSetDef structures, declaring computed attributes of instances of this type. However, please note that tp_getset is used to define attribute access for object attributes, not for dictionary-like access using the [] operator.*/
	&PyDict_Type,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)EntDict_init,      /* tp_init */
	0,                         /* tp_alloc */
	0,                 /* tp_new */
};

/*
-------------------------------------CREATE THE ENT DICT INSTANCE-----------------------------------------
PyObject* l = PyLong_FromLong(123)
	// Finally, add some items to the dictionary
	PyDict_SetItemString((PyObject*)ent_dict, "lookupthiskey", );

	MemoryItem::entityProperties is a unordered_map of entity_property type and offsets.

*/

void initEntDictType(void)
{
	EntDict_Type.tp_base = &PyDict_Type;
	// EntDict_Type.tp_getset = entity_getset_list;

	// uses memory_getter memory_setter from memory.cpp
	EntDict_Type.tp_as_mapping = &entity_mapping_methods;

	if (PyType_Ready(&EntDict_Type) < 0) {
		PyErr_Print();
		error_exit("Failed to Type Ready");
	}
}
static void addEntProperties(PyObject* ent_dict,edict_t * c_ent)
{
	/*
		Why add them if setters getters work.
		Because then the script users cannot iterate the dict?
		The setters and getters have to update the dictionary too.

		Ensuring synchronization between SoF memory and the python dict representation.
		When the ent dict is created by converting edict_t pointer at event cb. That is the moment of synchronization.
	*/
	
	// Read from the entities memory using get() and store inside the ent dict.
	// first = Key, second = Value
	for( const auto &pair : MemoryItem::entityProperties ) {
		PyObject* val = NULL;
		pair.second->get((void*)c_ent,val); //n
		PyDict_SetItemString(ent_dict, pair.first.c_str() , val);//b
		Py_DECREF(val);
	}
}

/*
	The first field of the type item structure is the inherited object type.

	A dict item holding a hidden __ent__ field which is the c ent pointer
	The dict also has overriden set/get functions which access the memory directly.
*/
PyObject* createEntDict(edict_t * c_ent)
{

	// override some things that were 'inherited' by PyType_Ready

	// This is like new EntDict.

	if ( c_ent ) {
		PyObject *ent_dict = PyObject_CallObject((PyObject *) &EntDict_Type, NULL);
		if ( ent_dict) {
			// no reason to use capsules.
			EntDict* ed = (EntDict*)ent_dict;
			// crash?
			ed->c_ent = c_ent;

			// Add every other ent specific object
			addEntProperties(ent_dict,c_ent);
			// Py_DECREF(ent_dict);
		}
		
		return ent_dict;
	}
	Py_RETURN_NONE;
}