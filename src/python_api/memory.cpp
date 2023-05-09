#include "common.h"

std::unordered_map<std::string,void *> type_getters;
std::unordered_map<std::string,void *> type_setters;

// The list passed to the ent dict creation to override set get.
// PyGetSetDef *entity_getset_list;

PyObject* memory_getter(PyObject *self, PyObject * key);
int memory_setter(PyObject *self, PyObject * key, PyObject *value);
PyMappingMethods entity_mapping_methods = {
	.mp_subscript = (binaryfunc)memory_getter,
	.mp_ass_subscript = (objobjargproc)memory_setter,
};

class FloatMemoryItem : public MemoryItem {
public:
	FloatMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"float",offset,info) {}

	void get(void* baseAddress, PyObject* &value) {
		value = PyFloat_FromDouble(*(float*)(baseAddress + offset));
	}
	void get(void* baseAddress, float &value) {
		value = *(float*)(baseAddress + offset);
	}
	void set(void* baseAddress,float value) {
		*(float*)(baseAddress + offset) = value;
	}
	void set(void* baseAddress, PyObject* value) {
		float floatValue = PyFloat_AsDouble(value);
		*(float*)((char*)baseAddress + offset) = floatValue;
	}
};
class IntMemoryItem : public MemoryItem {
public:
	IntMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"int",offset,info) {}

	void get(void* baseAddress, PyObject* &value) {
		value = PyLong_FromLong(*(int*)(baseAddress + offset));
	}
	void get(void* baseAddress, int &value) {
		value = *(int*)(baseAddress + offset);
	}
	void set(void* baseAddress,int value) {
		*(int*)(baseAddress + offset) = value;
	}
	void set(void* baseAddress, PyObject* value) {
		int intValue = PyLong_AsLong(value);
		*(int*)((char*)baseAddress + offset) = intValue;
	}
};
class VectorMemoryItem : public MemoryItem {
public:
	VectorMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"vector",offset,info) {}

	void get(void* baseAddress, PyObject* &value) {
		vec3_t vec;
		VectorCopy(*(vec3_t*)(baseAddress + offset),vec);
		value = Py_BuildValue("(fff)",vec[0],vec[1],vec[2]);
	}
	void get(void* baseAddress, float* &value) {
		value = (float*)(baseAddress + offset);
	}
	void set(void* baseAddress,vec3_t value) {
		VectorCopy(value,*(vec3_t*)(baseAddress + offset));
	}
	void set(void* baseAddress,PyObject * value) {
		if (PyList_Check(value) && PyList_Size(value) == 3) {
			PyObject* xObj = PyList_GetItem(value, 0);
			PyObject* yObj = PyList_GetItem(value, 1);
			PyObject* zObj = PyList_GetItem(value, 2);

			if ( PyFloat_Check(xObj) && PyFloat_Check(yObj) && PyFloat_Check(zObj) ) {
				float x = (float)PyFloat_AsDouble(xObj);
				float y = (float)PyFloat_AsDouble(yObj);
				float z = (float)PyFloat_AsDouble(zObj);

				*(vec_t*)((char*)baseAddress + offset) = x;
				*(vec_t*)((char*)baseAddress + offset + sizeof(vec_t)) = y;
				*(vec_t*)((char*)baseAddress + offset + 1*sizeof(vec_t)) = z;
			}
		}
	}
};
typedef vec_t vec2_t[2];
class Vector2DMemoryItem : public MemoryItem {
public:
	Vector2DMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"vector2d",offset,info) {}

	void get(void* baseAddress, PyObject* &value) {
		
		value = Py_BuildValue("(ff)",*(vec_t*)(baseAddress + offset),*(vec_t*)(baseAddress + offset + sizeof(vec_t)));
	}
	void get(void* baseAddress, float* &value) {
		value = (float*)(baseAddress + offset);
	}
	void set(void* baseAddress, unsigned int offset, vec2_t value) {
		*(vec_t*)((char*)baseAddress + offset) = value[0];
		*(vec_t*)((char*)baseAddress + offset + sizeof(vec_t)) = value[1];
	}
	void set(void* baseAddress,PyObject * value) {
		if (PyList_Check(value) && PyList_Size(value) == 2) {
			PyObject* xObj = PyList_GetItem(value, 0);
			PyObject* yObj = PyList_GetItem(value, 1);

			if ( PyFloat_Check(xObj) && PyFloat_Check(yObj) ) {
				float x = (float)PyFloat_AsDouble(xObj);
				float y = (float)PyFloat_AsDouble(yObj);

				*(vec_t*)((char*)baseAddress + offset) = x;
				*(vec_t*)((char*)baseAddress + offset + sizeof(vec_t)) = y;
			}
		}
	}
};
class ShortVector2DMemoryItem : public MemoryItem {
public:
	ShortVector2DMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"shortvector2d",offset,info) {}

	void get(void* baseAddress, PyObject* &value) {
		value = Py_BuildValue("(hh)",*(short*)(baseAddress + offset), *(short*)(baseAddress + offset + sizeof(short)));
	}
	void get(void* baseAddress, short &value) {
		value = (short*)(baseAddress + offset);
	}
	void set(void* baseAddress,short* value) {
		*(short*)(baseAddress + offset) = value;
	}
	void set(void* baseAddress,PyObject * value) {
		if (PyList_Check(value) && PyList_Size(value) == 2) {
			PyObject* xObj = PyList_GetItem(value, 0);
			PyObject* yObj = PyList_GetItem(value, 1);

			if ( PyLong_Check(xObj) && PyLong_Check(yObj) ) {
				int x = (int)PyLong_AsLong(xObj);
				int y = (int)PyLong_AsLong(yObj);

				*(short*)((char*)baseAddress + offset) = x;
				*(short*)((char*)baseAddress + offset + sizeof(short)) = y;
			}
		}
	}
};

class StringMemoryItem : public MemoryItem {
public:
	StringMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"string",offset,info) {}
	void get(void* baseAddress, PyObject* &value) {
		std::cout << "get_string" << std::endl;
		char * in_str = *(char**)(baseAddress + offset);

		if ( in_str == NULL ) {
			std::cout << "Reading Null String pointer" << std::endl;
			in_str = "";
		} else
			SOFPPNIX_DEBUG("HM : %s\n",in_str);
		value = PyUnicode_FromString(in_str);
	}
	void get(void* baseAddress, std::string &value) {
		value = (char*)(baseAddress + offset);
	}
	void set(void* baseAddress,std::string value) {
		strlcpy((char*)(baseAddress + offset),value.c_str(),value.length());
	}
	void set(void* baseAddress, PyObject* value) {
		if (PyUnicode_Check(value)) {
			const char* strValue = PyUnicode_AsUTF8(value);
			std::cout << "setting str to :" << strValue << std::endl;
			size_t length = strlen(strValue);
			/*
			This caches the UTF-8 representation of the string in the Unicode object, and subsequent calls will return a pointer to the same buffer. The caller is not responsible for deallocating the buffer. The buffer is deallocated and pointers to it become invalid when the Unicode object is garbage collected.
			*/
			//Write char * pointer.
			*(char**)(baseAddress + offset) = strValue;
		}
	}
};
class PointerMemoryItem : public MemoryItem {
public:
	PointerMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"pointer",offset,info) {}

	void get(void* baseAddress, PyObject* &value) {
		value = PyLong_FromVoidPtr(*(void**)(baseAddress + offset));
	}
	void get(void* baseAddress, unsigned int &value) {
		value = *(unsigned int*)(baseAddress + offset);
	}
	void set(void* baseAddress,void* value) {
		*(unsigned int*)(baseAddress + offset) = (unsigned int)value;
	}
	void set(void* baseAddress,PyObject* value) {
		long longValue = PyLong_AsLong(value);
		*(int*)((char*)baseAddress + offset) = (int)longValue;
	}
};
class ShortMemoryItem : public MemoryItem {
public:
	ShortMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"short",offset,info) {}
	void get(void* baseAddress, PyObject* &value) {
		value = PyLong_FromLong(*(short*)(baseAddress + offset));
	}
	void get(void* baseAddress, short &value) {
		value = *(short*)(baseAddress + offset);
	}
	void set(void* baseAddress,short value) {
		*(short*)(baseAddress + offset) = value;
	}
	void set(void* baseAddress,PyObject* value) {
		short shortValue = PyLong_AsLong(value);
		*(short*)((char*)baseAddress + offset) = shortValue;
	}
};
class ByteMemoryItem : public MemoryItem {
public:
	ByteMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"byte",offset,info) {}
	void get(void* baseAddress, PyObject* &value) {
		value = PyLong_FromLong(*(char*)(baseAddress + offset));
	}
	void get(void* baseAddress, char &value) {
		value = *(char*)(baseAddress + offset);
	}
	void set(void* baseAddress,char value) {
		*(char*)(baseAddress + offset) = value;
	}
	void set(void* baseAddress,PyObject* value) {
		char byteValue = PyLong_AsLong(value);
		*(short*)((char*)baseAddress + offset) = byteValue;
	}
};

class ShortVectorMemoryItem : public MemoryItem {
public:
	ShortVectorMemoryItem(std::string name,unsigned int offset,std::string info) : MemoryItem(name,"shortvector",offset,info) {}
	void get(void* baseAddress, PyObject* &value) {
		value = Py_BuildValue("(hhh)",*(short*)(baseAddress + offset), *(short*)(baseAddress + offset + sizeof(short)), *(short*)(baseAddress + offset + sizeof(short) * 2));
	}
	void get(void* baseAddress, short* &value) {
		value = (short*)(baseAddress + offset);
	}
	void set(void* baseAddress,short* value) {
		*(short*)(baseAddress + offset) = value;
	}
	void set(void* baseAddress,PyObject * value) {
		if (PyList_Check(value) && PyList_Size(value) == 3) {
			PyObject* xObj = PyList_GetItem(value, 0);
			PyObject* yObj = PyList_GetItem(value, 1);
			PyObject* zObj = PyList_GetItem(value, 2);

			if ( PyLong_Check(xObj) && PyLong_Check(yObj) && PyLong_Check(zObj)) {
				short x = (short)PyLong_AsLong(xObj);
				short y = (short)PyLong_AsLong(yObj);
				short z = (short)PyLong_AsLong(zObj);

				*(short*)((char*)baseAddress + offset) = x;
				*(short*)((char*)baseAddress + offset + sizeof(short)) = y;
				*(short*)((char*)baseAddress + offset + 2*sizeof(short)) = z;
			}
		}
	}
};

// ----------------------------------------------------------------------------------------------
/*
	The function that is overriden in the PyDict as setter/getter

	Should we allow the python script to store things inside the ent dict?
	Would they even be persistent? Not atm because dict is re-created on each event.
	
	What represents the handle of ent? I'm not sure how reliable skinnum is as a handle.
	G_Find function.
	
*/
//--Generic
static PyObject* memory_getter(PyObject *self, PyObject * key) {
	std::cout << "Generic getter called" << std::endl;

	// Convert PyObject* key to char*
	std::string key_str;
	char * c = NULL;
	if (PyUnicode_Check(key)) {
		c = PyUnicode_AsUTF8(key);
		if (c == NULL) {
			PyErr_Print();
			error_exit("Cannot decode key");
		}
		key_str = c;
		EntDict * ed = (EntDict*)self;

		// Reads SoF Memory
		PyObject* value = NULL;
		MemoryItem::entityProperties[key_str]->get(ed->c_ent, value);
		return value;
	}
	// Key is not a string? Let Original Handle it.

	// Call original getter.
	PyObject* result = PyObject_GetAttr(self, PyUnicode_FromString("__getitem__"));
	if (result == NULL) {
		PyErr_Print();
		error_exit("Error occurred while retrieving the getter");
	}
	PyObject* value = PyObject_CallFunctionObjArgs(result, key, NULL);
	Py_DECREF(result);
	if (value == NULL) {
		PyErr_Print();
		error_exit("Error occurred while calling the getter");
	}
	// Py_DECREF(value);
	
	return value;
}

static int memory_setter(PyObject *self, PyObject * key, PyObject *value) {
	std::cout << "Generic setter called" << std::endl;

	// Convert PyObject* key to char*
	std::string key_str;
	char * c = NULL;
	if (PyUnicode_Check(key)) {
		c = PyUnicode_AsUTF8(key);
		if (c == NULL) {
			PyErr_Print();
			error_exit("Cannot decode key");
		}
		key_str = c;
		EntDict * ed = (EntDict*)self;

		// Writes SoF Memory.
		MemoryItem::entityProperties[key_str]->set(ed->c_ent, value);
		return 0; //success
	}
	// Key is not a string? Let Original Handle it.

	// Call the original dictionary setter
	PyObject* result = PyObject_GetAttr(self, PyUnicode_FromString("__setitem__"));
	if (result == NULL) {
		PyErr_Print();
		error_exit("Error occurred while retrieving the setter");
	}
	PyObject* args = PyTuple_Pack(2, key, value);
	PyObject* setResult = PyObject_CallObject(result, args);
	Py_DECREF(result);
	Py_DECREF(args);
	if (setResult == NULL) {
		PyErr_Print();
		error_exit("Error occurred while calling the setter");
	}

	Py_DECREF(setResult);
	return 0; // Return 0 to indicate success
}

#if 0
// ----String----
static PyObject* string_getter(PyObject *self, void *closure) {
	StringMemoryItem* item = (StringMemoryItem*)closure;

	// Perform some custom logic to get the attribute value
	int value = 42;

	// Convert the value to a Python object and return it
	return PyLong_FromLong(value);
}
static int string_setter(PyObject *self, PyObject *value, void *closure) {
	StringMemoryItem* item = (StringMemoryItem*)closure;
	// Perform some custom logic to set the attribute value
	if (!PyLong_Check(value)) {
		PyErr_SetString(PyExc_TypeError, "Attribute value must be an integer");
		return -1;
	}

	int new_value = PyLong_AsLong(value);
	// Set the attribute value using 'new_value'

	return 0;  // Successful
}
// Int
static PyObject* int_getter(PyObject *self, void *closure) {
	IntMemoryItem* item = (IntMemoryItem*)closure;

	// Perform some custom logic to get the attribute value
	int value = 42;

	// Convert the value to a Python object and return it
	return PyLong_FromLong(value);
}
static int int_setter(PyObject *self, PyObject *value, void *closure) {
	IntMemoryItem* item = (IntMemoryItem*)closure;

	// Perform some custom logic to set the attribute value
	if (!PyLong_Check(value)) {
		PyErr_SetString(PyExc_TypeError, "Attribute value must be an integer");
		return -1;
	}

	int new_value = PyLong_AsLong(value);
	// Set the attribute value using 'new_value'

	return 0;  // Successful
}

// ----Float----
static PyObject* float_getter(PyObject *self, void *closure) {
	FloatMemoryItem* item = (FloatMemoryItem*)closure;

	// Perform some custom logic to get the attribute value
	int value = 42;

	// Convert the value to a Python object and return it
	return PyLong_FromLong(value);
}
static int float_setter(PyObject *self, PyObject *value, void *closure) {
	FloatMemoryItem* item = (FloatMemoryItem*)closure;

	// Perform some custom logic to set the attribute value
	if (!PyLong_Check(value)) {
		PyErr_SetString(PyExc_TypeError, "Attribute value must be an integer");
		return -1;
	}

	int new_value = PyLong_AsLong(value);
	// Set the attribute value using 'new_value'

	return 0;  // Successful
}

// ----Vector----
static PyObject* vector_getter(PyObject *self, void *closure) {
	VectorMemoryItem* item = (VectorMemoryItem*)closure;

	// Perform some custom logic to get the attribute value
	int value = 42;

	// Convert the value to a Python object and return it
	return PyLong_FromLong(value);
}
static int vector_setter(PyObject *self, PyObject *value, void *closure) {
	VectorMemoryItem* item = (VectorMemoryItem*)closure;

	// Perform some custom logic to set the attribute value
	if (!PyLong_Check(value)) {
		PyErr_SetString(PyExc_TypeError, "Attribute value must be an integer");
		return -1;
	}

	int new_value = PyLong_AsLong(value);
	// Set the attribute value using 'new_value'

	return 0;  // Successful
}

// ----Pointer----
static PyObject* pointer_getter(PyObject *self, void *closure) {
	PointerMemoryItem* item = (PointerMemoryItem*)closure;

	// Perform some custom logic to get the attribute value
	int value = 42;

	// Convert the value to a Python object and return it
	return PyLong_FromLong(value);
}
static int pointer_setter(PyObject *self, PyObject *value, void *closure) {
	PointerMemoryItem* item = (PointerMemoryItem*)closure;

	// Perform some custom logic to set the attribute value
	if (!PyLong_Check(value)) {
		PyErr_SetString(PyExc_TypeError, "Attribute value must be an integer");
		return -1;
	}

	int new_value = PyLong_AsLong(value);
	// Set the attribute value using 'new_value'

	return 0;  // Successful
}
#endif

#if 0
// Definition of the getset list for the object type
PyGetSetDef example_getset[] = {
	// vectors
	{"origin", vector_getter, vector_setter, entityProperties["origin"]->info, entityProperties["origin"]},
	{"angles", vector_getter, vector_setter, entityProperties["angles"]->info, entityProperties["angles"]},
	{"mins", vector_getter, vector_setter, entityProperties["mins"]->info, entityProperties["mins"]},
	{"maxs", vector_getter, vector_setter, entityProperties["maxs"]->info, entityProperties["maxs"]},
	{"velocity", vector_getter, vector_setter, entityProperties["velocity"]->info, entityProperties["velocity"]},
	{"pos1", vector_getter, vector_setter, entityProperties["pos1"]->info, entityProperties["pos1"]},

	// int
	{"max_health", int_getter, int_setter, entityProperties["max_health"]->info,entityProperties["max_health"]},
	{"health", int_getter, int_setter, entityProperties["health"]->info,entityProperties["health"]},
	{"solid", int_getter, int_setter, entityProperties["solid"]->info,entityProperties["solid"]},
	{"inuse", int_getter, int_setter, entityProperties["inuse"]->info,entityProperties["inuse"]},
	{"clipmask", int_getter, int_setter, entityProperties["clipmask"]->info,entityProperties["clipmask"]},
	{"movetype", int_getter, int_setter, entityProperties["movetype"]->info,entityProperties["movetype"]},
	{"skinnum", int_getter, int_setter, entityProperties["skinnum"]->info,entityProperties["skinnum"]},
	{"mass", int_getter, int_setter, entityProperties["mass"]->info,entityProperties["mass"]},
	{"flags", int_getter, int_setter, entityProperties["flags"]->info,entityProperties["flags"]},

	// string
	{"model", string_getter, string_setter, entityProperties["model"]->info,entityProperties["model"]},
	{"classname", string_getter, string_setter, entityProperties["classname"]->info,entityProperties["classname"]},
	{"targetname", string_getter, string_setter, entityProperties["targetname"]->info,entityProperties["targetname"]},
	{"target", string_getter, string_setter, entityProperties["target"]->info,entityProperties["target"]},
	{"soundname", string_getter, string_setter, entityProperties["soundname"]->info,entityProperties["soundname"]},

	// pointer void*
	{"script", pointer_getter, pointer_setter, entityProperties["script"]->info,entityProperties["script"]},
	{"ghoulinst", pointer_getter, pointer_setter, entityProperties["ghoulinst"]->info,entityProperties["ghoulinst"]},
	{"enemy", pointer_getter, pointer_setter, entityProperties["enemy"]->info,entityProperties["enemy"]},


	// float
	{"stopspeed", float_getter, float_setter, entityProperties["stopspeed"]->info,entityProperties["stopspeed"]},
	{"friction",}
	{NULL}  // Sentinel
	
};
#endif



/*
if key not exist in unordered map, [] creates empty val at key. Use find or count or at

malloc once, never need to dealloc.

Not needed anymore - only applies to class object.
*/
#if 0
static void generateGetterSetters(void)
{
	int i = 0;
	PyGetSetDef* current = NULL;
	for( const auto &pair : MemoryItem::entityProperties ) {
		i++;
		// grow the array blob of PyGetSetDef
		current = fast_realloc(current,sizeof(PyGetSetDef) * i);
		current[i-1].name = pair.first.c_str();
		try {
			// use the data type specific getter setter.
			#if 0
			current->get = type_getters.at(pair.second->type);
			current->set = type_setters.at(pair.second->type);
			#endif
			current[i-1].get = memory_getter;
			current[i-1].set = memory_setter;
		}
		catch ( const std::out_of_range &e ) {
			error_exit("Invalid Key when generating setter getters");
		}
		current[i-1].doc = pair.second->info.c_str();
		current[i-1].closure = MemoryItem::entityProperties[pair.first];
	}
	// null terminated
	current = fast_realloc(current,sizeof(PyGetSetDef) * (i+1));
	memset(current+i,0x00,sizeof(PyGetSetDef));
	
	entity_getset_list = current;

}
#endif

// -----------------------------------------------------------------------------------------------


static std::unordered_map<std::string, MemoryItem*> MemoryItem::entityProperties;
void MemoryItem::initMemoryItems(void)
{
	#if 0
	// getters
	type_getters["string"] = (void*)string_getter;
	type_getters["int"] = (void*)int_getter;
	type_getters["float"] = (void*)float_getter;
	type_getters["vector"] = (void*)vector_getter;
	type_getters["pointer"] = (void*)pointer_getter;

	// setters
	type_setters["string"] = (void*)string_setter;
	type_setters["int"] = (void*)int_setter;
	type_setters["float"] = (void*)float_setter;
	type_setters["vector"] = (void*)vector_setter;
	type_setters["pointer"] = (void*)pointer_setter;
	#endif


//--Vector------------------------------------------------------------------------------------------------
	// e.entity_state_t.origin
	entityProperties["origin"] = new VectorMemoryItem("origin",4,"3d location of entity.");
	// e.entity_state_t.angles
	entityProperties["angles"] = new VectorMemoryItem("angles",16,"3d orientation of entity.");
	// e.mins
	entityProperties["mins"] = new VectorMemoryItem("mins",284,"mins is used for collision bounding box creation. It represents the lowest corner of the 3d box.");
	// e.maxs
	entityProperties["maxs"] = new VectorMemoryItem("maxs",296,"maxs is used for collision bounding box creation. It represents the highest corner of the 3d box.");
	// e.velocity
	entityProperties["velocity"] = new VectorMemoryItem("velocity",588,"velocity is the speed an entity is moving.");
	// e.pos1
	entityProperties["pos1"] = new VectorMemoryItem("pos1",564,"pos1 is an extra paramter used for weird things, not sure.");

//--Int--------------------------------------------------------------------------------------------------
	// e.max_health
	entityProperties["health_max"] = new IntMemoryItem("health_max",752,"maximum health of entity.");
	// e.health
	entityProperties["health"] = new IntMemoryItem("health",748,"current health of entity.");
	// e.solid
	entityProperties["solid"] = new IntMemoryItem("solid",344,"solid is collision with other entities.");
	// e.inuse
	entityProperties["inuse"] = new IntMemoryItem("inuse",120,"inuse is whether an entity has been initiated and ready to use or not.");
	// e.clipmask
	entityProperties["clipmask"] = new IntMemoryItem("clipmask",348,"clipmask determines which surfaces this ent should collide with. 0 dont collide with anything.\nLook in q_shared.h file for numbers to use.");
	// e.movetype
	entityProperties["movetype"] = new IntMemoryItem("movetype",412,"movetype is the mode in which the ent should interact with world and physics.");
	//e.entity_state_t.skinnum;
	entityProperties["skinnum"] = new IntMemoryItem("skinnum",44,"skinnum is either the slot num if its a player or an alternative texture num for some entities.");
	// e.mass
	entityProperties["mass"] = new IntMemoryItem("mass",636,"mass some number used in physics calculations.");
	// e.flags
	entityProperties["flags"] = new IntMemoryItem("flags",416,"flags status information for this ent, not sure.");

//--String--------------------------------------------------------------------------------------------------
	// e.model
	entityProperties["model"] = new StringMemoryItem("model",420,"model is a hint to some entities for which ghoulmodel to load.");
	// e.classname
	entityProperties["classname"] = new StringMemoryItem("classname",436,"classname determines which entity spawn function gets run, thus which type of ent this it.");
	// e.targetname
	entityProperties["targetname"] = new StringMemoryItem("targetname",456,"targetname is used as a label for triggers, so they can become targets of other triggers.");
	// e.target
	entityProperties["target"] = new StringMemoryItem("target",452,"target is used to set which entity/trigger you will activate/use.");
	// e.soundName
	entityProperties["soundname"] = new StringMemoryItem("soundname",488,"soundname a variable used for some events, not sure.");

//--Pointer-------------------------------------------------------------------------------------------------
	// e.Script [CScript*]
	entityProperties["script"] = new PointerMemoryItem("script"	,1108,"script is an instance to ds script file object.");

	// e.ghoulInst [IGhoulInst*]
	entityProperties["ghoulInst"] = new PointerMemoryItem("ghoulInst",	356,"ghoulInst is a ghoul object pointer.");

	// e.enemy
	entityProperties["enemy"] = new PointerMemoryItem("enemy",804,"enemy no idea what this is.");

//--Float--------------------------------------------------------------------------------------------------
	// e.stopspeed
	entityProperties["stopspeed"] = new FloatMemoryItem("stopspeed",640,"stopspeed some number used in physics calculations.");
	// e.friction
	entityProperties["friction"] = new FloatMemoryItem("friction",644,"friction is some number used in physics calculations.");
	// e.airresistance
	entityProperties["airresistance"] = new FloatMemoryItem("airresistance",652,"airresistance is some number used in physics caluclations.");
	// e.bouyancy
	entityProperties["bouyancy"] = new FloatMemoryItem("bouyancy",656,"bouyancy is some number used in physics calculations.");
	// e.gravity
	entityProperties["gravity"] = new FloatMemoryItem("gravity",648,"gravity is how fast the entity falls downwards.");
	// e.nextthink
	entityProperties["nextthink"] = new FloatMemoryItem("nextthink",692,"next_think when the entitys think function should run next.");
	//--------------------------------------------------------------------------------------------------------

	/*
	END OF BASIC ENT FIELDS
	*/

	std::unordered_map<std::string, MemoryItem*> gclientProperties;
	//--------------------------------------------------------------------------------------------------------
	//.gclient->player_state_t.gun
	gclientProperties["gunghoulinst"] = new PointerMemoryItem("gunghoulinst",108,"gunghoulinst is the ghoul object pointer for the players gun.");
	//.gclient->player_state_t.viewangles
	gclientProperties["viewangles"] = new VectorMemoryItem("viewangles",28,"viewangles is the players viewing angle. Use sf_sv_vector_set to understand this format.");
	// gclient_t->player_state_t.pmove_state_t.delta_angles[3]
	gclientProperties["delta_angles"] = new ShortVector2DMemoryItem("delta_angles",20,"delta_angles is the players viewing angle. Use sf_sv_vector_set to understand this format.");
	// gclient_t->client_respawn_t.cmd_angles[3]
	gclientProperties["cmd_angles"] = new VectorMemoryItem("cmd_angles",780,"cmd_angles is the players viewing angle. Use sf_sv_vector_set to understand this format.");

	//--------------------------------------------------------------------------------------------------------
	// gclient_t->player_state_t.pmove_state_t.gravity
	// gclientProperties["gravity"] = new MemoryItem("gravity",					"float",18,"gravity is how fast the player falls to the ground.");
	// gclient_t->player_state_t.pmove_state_t.movescale [other movescale is 26]
	gclientProperties["movescale"] = new FloatMemoryItem("movescale",GCLIENT_MOVESCALE,"movescale is how slowed the player's movement is.");
	//--------------------------------------------------------------------------------------------------------
	// gclient_t->client_respawn_t.score
	gclientProperties["score"] = new IntMemoryItem("score",776,"score is the points the player has.");

}

// Dummy implementations that get overridden by Derived.
void MemoryItem::get(void* baseAddress, short* &value){
	error_exit("Shouldn't call this dummy func");
};
void MemoryItem::get(void* baseAddress, float* &value){error_exit("Shouldn't call this dummy func");};
void MemoryItem::get(void* baseAddress, char &value){error_exit("Shouldn't call this dummy func");};
void MemoryItem::get(void* baseAddress, int &value){error_exit("Shouldn't call this dummy func");};
void MemoryItem::get(void* baseAddress, short &value){error_exit("Shouldn't call this dummy func");};
void MemoryItem::get(void* baseAddress, float &value){error_exit("Shouldn't call this dummy func");};
void MemoryItem::get(void* baseAddress, void* &value){error_exit("Shouldn't call this dummy func");};
void MemoryItem::get(void* baseAddress, std::string &value){error_exit("Shouldn't call this dummy func");};

void MemoryItem::set(void* baseAddress,short* value){error_exit("Shouldn't call this dummy func");};
void MemoryItem::set(void* baseAddress,float* value){error_exit("Shouldn't call this dummy func");};
void MemoryItem::set(void* baseAddress,char value){error_exit("Shouldn't call this dummy func");};
void MemoryItem::set(void* baseAddress,int value){error_exit("Shouldn't call this dummy func");};
void MemoryItem::set(void* baseAddress,short value){error_exit("Shouldn't call this dummy func");};
void MemoryItem::set(void* baseAddress,float value){error_exit("Shouldn't call this dummy func");};
void MemoryItem::set(void* baseAddress,void * value){error_exit("Shouldn't call this dummy func");};
void MemoryItem::set(void* baseAddress,std::string value){error_exit("Shouldn't call this dummy func");};