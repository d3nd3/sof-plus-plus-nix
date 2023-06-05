#include "common.h"


std::unordered_map<std::string,bool> inv_cmd_exist;

void init_inv_cmd_list(void)
{
	inv_cmd_exist["reload"] = true;
	inv_cmd_exist["weapnext"] = true;
	inv_cmd_exist["weapprev"] = true;
	inv_cmd_exist["weaponselect"] = true;
	inv_cmd_exist["weapondrop"] = true;
	inv_cmd_exist["weaponlose"] = true;
	inv_cmd_exist["weaponbestsafe"] = true;
	inv_cmd_exist["weaponbestunsafe"] = true;
	inv_cmd_exist["itemuse"] = true;
	inv_cmd_exist["itemnext"] = true;
	inv_cmd_exist["itemprev"] = true;
	inv_cmd_exist["itemdrop"] = true;
}
/*
called by my_Cbuf_AddLateCommands.
Guaranteed to be before game.dll is loaded. (dedicated_start).
*/
void serverInit(void)
{
	if ( dedicated->value == 1.0f ) {

		// Scoreboard page draw callbacks
		// init_pages();
		// Weapon Select command strings ( unordered_map lookup.)
		init_inv_cmd_list();

		// Get the current PYTHONPATH value
		const char* current_path = getenv("PYTHONPATH");

		std::string ppnix_py_path = orig_FS_Userdir() + std::string("/python");
		// Create a new string with the updated PYTHONPATH value
		std::string updated_path;
		if (current_path) {
			// Append the new path to the current PYTHONPATH value
			updated_path = current_path + std::string(":") + ppnix_py_path;
		} else {
			// If PYTHONPATH is not set, use the new path as is
			updated_path = ppnix_py_path;
		}

		// Set the updated PYTHONPATH value, 1 == overwrite.
		setenv("PYTHONPATH", updated_path.c_str(), 1);

		Py_Initialize();

		createServerCvars();
		// Slidefix
		callE8Patch(0x0812E643,&my_PM_StepSlideMove);

		//--------------------------------PYTHON-------------------------------------
		decorators.push_back(&player_die_callbacks);
		decorators.push_back(&player_connect_callbacks);
		decorators.push_back(&player_disconnect_callbacks);
		decorators.push_back(&player_respawn_callbacks);
		decorators.push_back(&player_say_callbacks);
		decorators.push_back(&frame_early_callbacks);
		decorators.push_back(&map_spawn_callbacks);

		SOFPPNIX_PRINT("Python version : %s", Py_GetVersion());


		//------------------------SOFREE STRINGPACKAGE STUFF------------------------
	
		fixupClassesForLinux();

		//------------------------------CUSTOM RCON--------------------------
		cmd_map["fraglimit"] = &rcon_fraglimit;
		cmd_map["timelimit"] = &rcon_timelimit;
		cmd_map["deathmatch"] = &rcon_deathmatch;

		cmd_map["map"] = 0x080AE3E8;
		cmd_map["set_dmflags"] = 0x080A28EC;
		cmd_map["unset_dmflags"] = 0x080A2A90;
		cmd_map["list_dmflags"] = 0x080A25A0;

		
	}
}

char saved_map_arg1[MAX_STRING_CHARS];
bool map_saving = false;

/*
	Calls SV_Map_f again.
*/
void map_continue(void)
{
	// DOWNLOAD DONE.
	char cmd[MAX_STRING_CHARS];
	snprintf(cmd,MAX_STRING_CHARS,"map \"%s\" \"%s\"\n",saved_map_arg1,"fgdd46hg");
	// SOFPPNIX_PRINT("Calling original map_f p1\n");
	orig_Cmd_ExecuteString(cmd);
}
/*
cmd_argv(0) = map
cmd_argv(1) = mapname
*/
int * cmd_argc = 0x083F9200;
void my_SV_Map_f(void)
{
	// SOFPPNIX_DEBUG("SV_Map!");
	if ( map_saving ) {
		// How do know if this was called by callback or by a user.
		if ( orig_Cmd_Argc() >= 3 ) {
			if ( !strcmp(orig_Cmd_Argv(2), "fgdd46hg") ) {
				// DOWNLOAD ALREADY DONE.
				// SOFPPNIX_PRINT("Calling original map_f p2\n");
				*cmd_argc = 2;
				map_saving = false;

				// Surely by this the cmd_argv are not preserved.
				// Have to save the mapname parameter instead.
				orig_SV_Map_f();
				return;
			}
		}
		
		SOFPPNIX_PRINT("Map download in progress, please wait");
		return;
	}
	/*
		Tweak argument to contain .zip for this request.
	*/
	std::string zip_req(std::string(orig_Cmd_Argv(1)) + ".zip");
	if ( ! beginHttpDL(&zip_req, &map_continue,true) ) {
		SOFPPNIX_PRINT("Not the right time to load a map");
		return;
	}
	// empty mapname bug ruins cmd_argc count. Used double quotes in ExecuteString

	// SOFPPNIX_DEBUG("saved mapname == %s",orig_Cmd_Argv(1));
	strcpy(saved_map_arg1,orig_Cmd_Argv(1));
	// http map start here
	map_saving = true;

	// Wait until map is downloaded...
}

/*
===================
SV_GiveMsec

Every few frames, gives all clients an allotment of milliseconds
for their command moves.  If they exceed it, assume cheating.
===================

cl->commandMsec = 1800;		// 1600 + some slop
*/

/*
	poorman msec adjustment

	orig_SV_ClientThink performs : cl->commandMsec -= cmd->msec;

	clc_move packet just came in, burn some cpu time.
	typedef struct usercmd_s
	{
		byte	msec;
		byte	buttons;
		byte	lightlevel;		// light level the player is standing on
		char	lean;			// -1 or 1
		short	angles[3];
		short	forwardmove;
		short	sidemove;
		short	upmove;
		float	fireEvent;
		float	altfireEvent;
	} usercmd_t;

	BUTTON_ATTACK=1
	BUTTON_USE=2
	BUTTON_ALTATTACK=4
	BUTTON_WEAP3=8
	BUTTON_WEAP4=16
	BUTTON_RUN=32
	BUTTON_ACTION=64
*/

std::array<int, 32> client_framecounters = {0};

void my_SV_ClientThink(void *cl, usercmd_t *cmd)
{
	edict_t * ent = *(unsigned int*)(cl + CLIENT_ENT);
	gclient_t * gcl = ent->client;
	// short * delta_angles = gcl->ps.pmove.delta_angles;
	// SOFPPNIX_DEBUG("delta_angles: %d %d %d\n", delta_angles[0], delta_angles[1], delta_angles[2]);
	// validateInputs(getPlayerSlot(cl),cmd,(void*)cmd+0x18);
	orig_SV_ClientThink(cl,cmd);

	// short * origin = gcl->ps.pmove.origin;
	// SOFPPNIX_DEBUG("origin: %d %d %d\n", origin[0], origin[1], origin[2]);

	// short * velocity = gcl->ps.pmove.velocity;
	// SOFPPNIX_DEBUG("velocity: %hd %hd %hd\n", velocity[0], velocity[1], velocity[2]);
	
	// SOFPPNIX_DEBUG("VelocityFloat: %f %f %f\n",ent->velocity[0],ent->velocity[1],ent->velocity[2]);
	
	// vec_t vel = VectorLength(ent->velocity);
	// SOFPPNIX_DEBUG("Velocity: %f",vel);
}


/*
	from used as base for move contents.
	memcpy(move,from)

*/
void my_PAK_ReadDeltaUsercmd (void *in_packet, usercmd_t *from, usercmd_t *move)
{
	orig_PAK_ReadDeltaUsercmd(in_packet,from,move);

	// SOFPPNIX_DEBUG("FROM : \n");
	// dump_usercmd(*from);
	// SOFPPNIX_DEBUG("TO : \n");
	// dump_usercmd(*move);
}

void rcon_fraglimit(void)
{
	SOFPPNIX_PRINT("FRAGLIMIT!");
}
void rcon_timelimit(void)
{
	SOFPPNIX_PRINT("TIMELIMIT!");
}
void rcon_deathmatch(void)
{
	SOFPPNIX_PRINT("DEATHMATCH!");
}

// generate a std dictionary // map
// typedef void (*cmd)(void);


std::unordered_map<std::string,rcon_cmd_type> cmd_map;
void public_rcon_command(void)
{

	// Its already been tokenized with macro expansion disabled.

	std::regex r("^(fraglimit|timelimit|map|deathmatch|set_dmflags|unset_dmflags|list_dmflags)$");
	std::string s(orig_Cmd_Argv(1));
	std::smatch m;
	if (std::regex_search(s, m, r)) {
		// print the match
		// std::cout << "MATCH : " << m.str() << std::endl;
		// check if cmd_argv(1) is a key into cmd_map, if it is, execute the function

		// SOFPPNIX_PRINT("before : %s",orig_Cmd_Args());

		std::string rcon_cmd = orig_Cmd_Args();
		orig_Cmd_TokenizeString(rcon_cmd.c_str(),false);
		// SOFPPNIX_PRINT("total : %s",orig_Cmd_Args());
		cmd_map[m.str()]();
	}
}

/*
	SV_Frame
		SV_RunGameFrame
			ge->RunFrame
*/
void my_SV_RunGameFrame(void)
{
	for ( int i = 0 ; i < maxclients->value;i++ ) {
		void * client_t = getClientX(i);
		int state = *(int*)(client_t);
		if (state != cs_spawned )
			continue;
		edict_t * ent = stget(client_t,CLIENT_ENT);
		// layout_clear(ent);
	}
	
	// Trying to use this for events that run every frame.
	// Although quake probably wanted you to use `think` callbacks.
	for ( int i = 0; i < frame_early_callbacks.size(); i++ ) {
		PyObject* result = PyObject_CallFunction(frame_early_callbacks[i],"");
		// returns None
		Py_XDECREF(result);
	}

	orig_SV_RunGameFrame();
}

void my_SV_ExecuteUserCommand (char *s)
{
	SOFPPNIX_DEBUG("Command : %s",s);
	orig_SV_ExecuteUserCommand(s);
}