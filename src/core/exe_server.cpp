#include "common.h"

char saved_map_arg1[MAX_STRING_CHARS];
bool map_saving = false;
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
	if ( map_saving ) {
		// How do know if this was called by callback or by a user.
		if ( orig_Cmd_Argc >= 3 ) {
			if ( !strcmp(orig_Cmd_Argv(2), "fgdd46hg") ) {
				// DOWNLOAD ALREADY DONE.
				// SOFPPNIX_PRINT("Calling original map_f p2\n");
				*cmd_argc = 2;
				map_saving = false;
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

#if 1
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
#else

/*
 realized I can exploit the olddelta inside the same clc_move.

 i really want to have a hook at an earlier time in the function, but it's not possible.

  latched buttons = any button that is on now, but wasn't on in previous buttons
*/
void my_SV_ClientThink(void *cl, usercmd_t *cmd)
{
	if ( _nix_poorman->value ) {

		int player = getPlayerSlot(cl);
		// SOFPPNIX_DEBUG("player slot: %d\n", player);

		if ( player == -1 ) error_exit("Illegal player slot");

		// increment frame counter, don't skip valuable button changes, OR defer them to next frame
		if ( ( client_framecounters[player]++ % 2 ) ) {
			// skip this frame
			// Defer all data from previous frame to this one.
			return;
		}
		orig_SV_ClientThink(cl,(void*)cmd+0x18);

	}
	// usercmd_t * oldcmd = (void*)cmd+0x18;
	// SOFPPNIX_DEBUG("forwardmove: %hu\n", cmd->forwardmove);
	// hexdump((void*)cmd-0x18,(void*)cmd+0x18);
	orig_SV_ClientThink(cl,cmd);
}

#endif

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