#include "common.h"

/*
player.equip_armor
*/
static PyObject * py_player_equip_armor(PyObject * self, PyObject * args);

//name,c_func,flags,docstring
static PyMethodDef PlayerMethods[] = {
	{"equip_armor", py_player_equip_armor, METH_VARARGS,"Give a player armor"},

	{NULL, NULL, 0, NULL} //sentinel
};

//base,name,docstring,size,methods
PyModuleDef PlayerModule = {
	PyModuleDef_HEAD_INIT, "player", "a module for interacting with clients.", -1, PlayerMethods,
	NULL, NULL, NULL, NULL //sentinel
};


/*
	edict_t *ent
	int amount
*/
static PyObject * py_player_equip_armor(PyObject * self, PyObject * args)
{
	
	EntDict * who;
	unsigned int amount;
	if (!PyArg_ParseTuple(args,"OI",&who,&amount)) {
		error_exit("Python: Failed to parse args in print");
	}
	
	int newArmor = orig_PB_AddArmor(who->c_ent,amount);

	PyObject * ret = PyLong_FromLong(newArmor);
	if ( ret == NULL ) {
		error_exit("Python: Failed to create return value");
	}
	return ret;
}