#include "common.h"

/*
# player.draw_text_at(100,100,"Edit!")
# player.draw_typeamatic(ent,text)
# player.draw_centered(ent,text)
# player.draw_centered_lower(ent,text)
# player.con_print(ent,text)

# player.equip_armor(ent,100)
*/
static PyObject * py_player_equip_armor(PyObject * self, PyObject * args);
static PyObject * py_player_set_page(PyObject * self, PyObject * args);

static PyObject * py_player_draw_text_at(PyObject * self, PyObject * args);
static PyObject * py_player_draw_img_at(PyObject * self, PyObject * args);
static PyObject * py_player_draw_direct(PyObject * self, PyObject * args);
static PyObject * py_player_draw_centered(PyObject * self, PyObject * args);
static PyObject * py_player_draw_credit(PyObject * self, PyObject * args);
static PyObject * py_player_draw_lower(PyObject * self, PyObject * args);
static PyObject * py_player_draw_typeamatic(PyObject * self, PyObject * args);
static PyObject * py_player_clear_layout(PyObject * self, PyObject * args);
static PyObject * py_player_refresh_layout(PyObject * self, PyObject * args);
static PyObject * py_player_con_print(PyObject * self, PyObject * args);
static PyObject * py_player_get_stats(PyObject * self, PyObject * args);
static PyObject * py_player_raw_sp(PyObject * self, PyObject * args);
static PyObject * py_player_get_layout(PyObject * self, PyObject * args);
static PyObject * py_ppnix_orig_scoreboard(PyObject * self, PyObject * args);



//name,c_func,flags,docstring
static PyMethodDef PlayerMethods[] = {
	{"set_page", py_player_set_page, METH_VARARGS,"Set current layout page"},
	{"equip_armor", py_player_equip_armor, METH_VARARGS,"Give a player armor"},
	{"draw_text_at", py_player_draw_text_at, METH_VARARGS,"Draw text at pos on player screen"},
	{"draw_img_at", py_player_draw_img_at, METH_VARARGS,"Draw img at pos on player screen"},
	{"draw_direct", py_player_draw_direct, METH_VARARGS,"Insert draw text directly into FLAG_LAYOUT."},
	{"draw_credit", py_player_draw_credit, METH_VARARGS,"Insert draw pic directly into FLAG_CREDIT."},
	{"draw_centered", py_player_draw_centered, METH_VARARGS,"Draw centered text on player screen"},
	{"draw_lower", py_player_draw_lower, METH_VARARGS,"Draw centered lower text on player screen"},
	{"draw_typeamatic", py_player_draw_typeamatic, METH_VARARGS,"Draw cinematic text on player screen"},
	{"clear_layout", py_player_clear_layout, METH_VARARGS,"Clear the text that is drawn on player screen"},
	{"refresh_layout", py_player_refresh_layout, METH_VARARGS,"Refresh the text that is drawn on player screen"},
	{"con_print", py_player_con_print, METH_VARARGS,"Print console text on player screen"},
	{"get_stats",py_player_get_stats,METH_VARARGS,"Get stats for this player"},
	{"raw_sp_print",py_player_raw_sp,METH_VARARGS,"Send stringpackage packet to player"},
	{"get_layout",py_player_get_layout,METH_VARARGS,"Get the custom layout string for player"},
	{"orig_scoreboard", py_ppnix_orig_scoreboard, METH_VARARGS,"Print orig scoreboard for player."},

	{NULL, NULL, 0, NULL} //sentinel
};

//base,name,docstring,size,methods
PyModuleDef PlayerModule = {
	PyModuleDef_HEAD_INIT, "player", "a module for interacting with clients.", -1, PlayerMethods,
	NULL, NULL, NULL, NULL //sentinel
};
PyMODINIT_FUNC PyInit_player(void) {

	return PyModule_Create(&PlayerModule);
}


static PyObject * py_ppnix_orig_scoreboard(PyObject * self, PyObject * args)
{
	EntDict * who;
	EntDict * who_killer;
	// Not null-terminated.
	if (!PyArg_ParseTuple(args,"OO",&who,&who_killer)) {
		error_exit("Python: Failed to parse args in py_ppnix_orig_scoreboard");
	}
	currentGameMode->clientScoreboardMessage(who->c_ent,who_killer->c_ent,0);
	Py_RETURN_NONE;
}
/*
	ent
*/
static PyObject * py_player_get_layout(PyObject * self, PyObject * args)
{
	EntDict * who;
	if (!PyArg_ParseTuple(args,"O",&who)) {
		error_exit("Python: Failed to parse args for py_player_get_layout");
	}
	int slot = slot_from_ent(who->c_ent);
	// Copied buffer. New reference created.
	return Py_BuildValue("s", strip_layouts[slot]);
}
/*
ent
id
msg
*/
static PyObject * py_player_raw_sp(PyObject * self, PyObject * args)
{
	// SOFPPNIX_DEBUG("Setting page...");
	char * msg;
	Py_ssize_t length;
	unsigned int SP_ID;
	EntDict * who;
	if (!PyArg_ParseTuple(args,"OIs#",&who,&SP_ID,&msg,&length)) {
		error_exit("Python: Failed to parse args for raw_sp_print");
	}
	char input[256];
	snprintf(input,256,"%.*s",(int)length,msg);

	orig_SP_Print(who->c_ent,SP_ID,input);

	Py_RETURN_NONE;
}

/*
	str - name

	Take a slot, instead of an entity.
*/
static PyObject * py_player_set_page(PyObject * self, PyObject * args)
{
	// SOFPPNIX_DEBUG("Setting page...");
	char * msg;
	Py_ssize_t length;
	unsigned int slot;
	if (!PyArg_ParseTuple(args,"Is#",&slot,&msg,&length)) {
		error_exit("Python: Failed to parse args for set_page");
	}
	char input[256];
	snprintf(input,256,"%.*s",(int)length,msg);

	current_page[slot] = input;

	Py_RETURN_NONE;
}

static PyObject * py_player_get_stats(PyObject * self, PyObject * args)
{
	EntDict * who;
	if (!PyArg_ParseTuple(args,"O",&who)) {
		error_exit("Python: Failed to parse args in py_player_equip_armor");
	}

	edict_t * ent = who->c_ent;
	int slot = slot_from_ent(ent);
	PyObject* pyDict = PyDict_New();
	PyObject* stats_armor = PyLong_FromLong(stats_armorsPicked[slot]);
	PyObject* stats_head = PyLong_FromLong(stats_headShots[slot]);
	PyObject* stats_throat = PyLong_FromLong(stats_throatShots[slot]);
	PyObject* stats_nut = PyLong_FromLong(stats_nutShots[slot]);

	PyDict_SetItemString(pyDict, "armorpickups", stats_armor);
	PyDict_SetItemString(pyDict, "headshots", stats_head);
	PyDict_SetItemString(pyDict, "throatshots", stats_throat);
	PyDict_SetItemString(pyDict, "nutshots", stats_nut);

	Py_DECREF(stats_armor);
	Py_DECREF(stats_head);
	Py_DECREF(stats_throat);
	Py_DECREF(stats_nut);
	
	return pyDict;
}
/*
	edict_t *ent
	int amount
*/
static PyObject * py_player_equip_armor(PyObject * self, PyObject * args)
{
	
	EntDict * who;
	unsigned int amount;
	if (!PyArg_ParseTuple(args,"OI",&who,&amount)) {
		error_exit("Python: Failed to parse args in py_player_equip_armor");
	}
	
	int newArmor = orig_PB_AddArmor(who->c_ent,amount);

	PyObject * ret = PyLong_FromLong(newArmor);
	if ( ret == NULL ) {
		error_exit("Python: Failed to create return value");
	}
	return ret;
}

/*
args:
	recipient ent handle or 0 for broadcast
	int x_pos
	int y_pos
	char* text
	bool gray.
*/
static PyObject * py_player_draw_text_at(PyObject * self, PyObject * args)
{
	
	char * msg;
	Py_ssize_t length;
	unsigned int x,y;
	// Not null-terminated.
	EntDict * who;
	qboolean gray;
	if (!PyArg_ParseTuple(args,"OIIs#p",&who,&x,&y,&msg,&length,&gray)) {
		error_exit("Python: Failed to parse args in py_player_draw_text");
	}
	// SOFPPNIX_DEBUG("Int : %i, Int : %i, str : %.*s",x,y,length,msg);

	// 256 upper, although the function can discard remaining.
	char input[256];
	snprintf(input,256,"%.*s",(int)length,msg);
	// orig_Cmd_ExecuteString(input);
	edict_t * ent;
	if ( (PyObject*)who == Py_None )
		// broadcast.
		ent = NULL;
	else
		ent = who->c_ent;
	append_layout_string(ent,x,y,input,gray);

	Py_RETURN_NONE;
}
/*
for who ( None = broadcast)
xpos
ypos
img_path
*/
static PyObject * py_player_draw_img_at(PyObject * self, PyObject * args)
{
	
	char * msg;
	Py_ssize_t length;
	unsigned int x,y;
	// Not null-terminated.
	EntDict * who;
	if (!PyArg_ParseTuple(args,"OIIs#",&who,&x,&y,&msg,&length)) {
		error_exit("Python: Failed to parse args in py_player_draw_img_at");
	}
	// SOFPPNIX_DEBUG("Int : %i, Int : %i, str : %.*s",x,y,length,msg);

	// 256 upper, although the function can discard remaining.
	char input[256];
	snprintf(input,256,"%.*s",(int)length,msg);
	// orig_Cmd_ExecuteString(input);
	edict_t * ent;
	if ( (PyObject*)who == Py_None )
		// broadcast.
		ent = NULL;
	else
		ent = who->c_ent;

	append_layout_image(ent,x,y,input);

	Py_RETURN_NONE;
}
/*
args:
	recipient ent handle or 0 for broadcast
	string what
*/
static PyObject * py_player_draw_centered(PyObject * self, PyObject * args)
{
	char * msg;
	Py_ssize_t length;
	// Not null-terminated.
	EntDict * who;
	if (!PyArg_ParseTuple(args,"Os#",&who,&msg,&length)) {
		error_exit("Python: Failed to parse args in py_player_draw_centered");
	}
	// SOFPPNIX_DEBUG("Int : %i, Int : %i, str : %.*s",x,y,length,msg);

	// 256 upper, although the function can discard remaining.
	char input[256];
	snprintf(input,256,"%.*s",(int)length,msg);

	edict_t * ent;
	if ( (PyObject*)who == Py_None )
		// broadcast.
		ent = NULL;
	else
		ent = who->c_ent;


	spackage_print_ref(ent,"++NIX","CENTER_CUSTOM",input,NULL);

	Py_RETURN_NONE;
}

static PyObject * py_player_draw_credit(PyObject * self, PyObject * args)
{
	char * msg;
	Py_ssize_t length;
	// Not null-terminated.
	EntDict * who;
	if (!PyArg_ParseTuple(args,"Os#",&who,&msg,&length)) {
		error_exit("Python: Failed to parse args in py_player_draw_credit");
	}
	// SOFPPNIX_DEBUG("Int : %i, Int : %i, str : %.*s",x,y,length,msg);

	// 256 upper, although the function can discard remaining.
	char input[256];
	snprintf(input,256,"%.*s",(int)length,msg);

	edict_t * ent;
	if ( (PyObject*)who == Py_None )
		// broadcast.
		ent = NULL;
	else
		ent = who->c_ent;


	spackage_print_ref(ent,"++NIX","CREDIT_CUSTOM",input,NULL);

	Py_RETURN_NONE;
}
/*
args:
	recipient ent handle or 0 for broadcast
	string what
*/
static PyObject * py_player_draw_lower(PyObject * self, PyObject * args)
{
	char * msg;
	Py_ssize_t length;
	// Not null-terminated.
	EntDict * who;
	if (!PyArg_ParseTuple(args,"Os#",&who,&msg,&length)) {
		error_exit("Python: Failed to parse args in py_player_draw_centered_lower");
	}
	// SOFPPNIX_DEBUG("Int : %i, Int : %i, str : %.*s",x,y,length,msg);

	// 256 upper, although the function can discard remaining.
	char input[256];
	snprintf(input,256,"%.*s",(int)length,msg);

	edict_t * ent;
	if ( (PyObject*)who == Py_None )
		// broadcast.
		ent = NULL;
	else
		ent = who->c_ent;
	spackage_print_ref(ent,"++NIX","CENTER_LOWER_CUSTOM",input,NULL);

	Py_RETURN_NONE;
}
/*
args:
	recipient ent handle or 0 for broadcast
	string what
*/
static PyObject * py_player_draw_typeamatic(PyObject * self, PyObject * args)
{
	char * msg;
	Py_ssize_t length;
	// Not null-terminated.
	EntDict * who;
	if (!PyArg_ParseTuple(args,"Os#",&who,&msg,&length)) {
		error_exit("Python: Failed to parse args in py_player_draw_typeamatic");
	}
	// SOFPPNIX_DEBUG("Int : %i, Int : %i, str : %.*s",x,y,length,msg);

	// 256 upper, although the function can discard remaining.
	char input[256];
	snprintf(input,256,"%.*s",(int)length,msg);

	edict_t * ent;
	if ( (PyObject*)who == Py_None )
		// broadcast.
		ent = NULL;
	else
		ent = who->c_ent;
	spackage_print_ref(ent,"++NIX","CINEMATIC_CUSTOM",input,NULL);

	Py_RETURN_NONE;
}
/*
args:
	recipient ent handle or 0 for broadcast
	string what
*/
static PyObject * py_player_draw_original(PyObject * self, PyObject * args)
{
	char * msg;
	Py_ssize_t length;
	// Not null-terminated.
	EntDict * who;
	if (!PyArg_ParseTuple(args,"Os#",&who,&msg,&length)) {
		error_exit("Python: Failed to parse args in py_player_draw_typeamatic");
	}
	// SOFPPNIX_DEBUG("Int : %i, Int : %i, str : %.*s",x,y,length,msg);

	// 256 upper, although the function can discard remaining.
	char input[256];
	snprintf(input,256,"%.*s",(int)length,msg);

	edict_t * ent;
	if ( (PyObject*)who == Py_None )
		// broadcast.
		ent = NULL;
	else
		ent = who->c_ent;
	spackage_print_ref(ent,"++NIX","ORIGINAL_CUSTOM",input,NULL);

	Py_RETURN_NONE;
}
/*
args:
	ent who
	string what
*/
static PyObject * py_player_con_print(PyObject * self, PyObject * args)
{
	char * msg;
	Py_ssize_t length;
	EntDict * who;
	// Not null-terminated.
	if (!PyArg_ParseTuple(args,"Os#",&who,&msg,&length)) {
		error_exit("Python: Failed to parse args in py_player_con_print");
	}
	// SOFPPNIX_DEBUG("Int : %i, Int : %i, str : %.*s",x,y,length,msg);

	edict_t * ent;
	if ( (PyObject*)who == Py_None )
		// broadcast.
		ent = NULL;
	else
		ent = who->c_ent;

	orig_cprintf(ent,PRINT_HIGH,"%.*s\n",(int)length,msg);
	Py_RETURN_NONE;
}
static PyObject * py_player_draw_direct(PyObject * self, PyObject * args)
{
	char * msg;
	Py_ssize_t length;
	EntDict * who;
	// Not null-terminated.
	if (!PyArg_ParseTuple(args,"Os#",&who,&msg,&length)) {
		error_exit("Python: Failed to parse args in py_player_con_print");
	}
	// SOFPPNIX_DEBUG("Int : %i, Int : %i, str : %.*s",x,y,length,msg);

	edict_t * ent;
	if ( (PyObject*)who == Py_None )
		// broadcast.
		ent = NULL;
	else
		ent = who->c_ent;

	append_layout_direct(ent,msg);
	Py_RETURN_NONE;
}


static PyObject * py_player_refresh_layout(PyObject * self, PyObject * args)
{

	EntDict * who;
	// Not null-terminated.
	if (!PyArg_ParseTuple(args,"O",&who)) {
		error_exit("Python: Failed to parse args in py_player_refresh_layout");
	}
	// SOFPPNIX_DEBUG("Int : %i, Int : %i, str : %.*s",x,y,length,msg);

	edict_t * ent;
	if ( (PyObject*)who == Py_None )
		// broadcast.
		ent = NULL;
	else
		ent = who->c_ent;

	refreshScreen(ent);
	Py_RETURN_NONE;
}
static PyObject * py_player_clear_layout(PyObject * self, PyObject * args)
{

	EntDict * who;
	// Not null-terminated.
	if (!PyArg_ParseTuple(args,"O",&who)) {
		error_exit("Python: Failed to parse args in py_player_clear_layout");
	}
	// SOFPPNIX_DEBUG("Int : %i, Int : %i, str : %.*s",x,y,length,msg);

	edict_t * ent;
	if ( (PyObject*)who == Py_None )
		// broadcast.
		ent = NULL;
	else
		ent = who->c_ent;


	layout_clear(ent);
	Py_RETURN_NONE;
}
