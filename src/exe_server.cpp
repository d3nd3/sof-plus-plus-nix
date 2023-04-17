#include "common.h"

char saved_map_arg1[MAX_STRING_CHARS];
bool map_saving = false;
void map_continue(void)
{
	// DOWNLOAD DONE.
	char cmd[MAX_STRING_CHARS];
	snprintf(cmd,MAX_STRING_CHARS,"map \"%s\" \"%s\"\n",saved_map_arg1,"fgdd46hg");
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

				*cmd_argc = 2;
				map_saving = false;
				orig_SV_Map_f();
				return;
			}
		}
		
		SOFPPNIX_PRINT("Map download in progress, please wait\n");
		return;
	}
	/*
		Tweak argument to contain .zip for this request.
	*/
	std::string zip_req(std::string(orig_Cmd_Argv(1)) + ".zip");
	if ( ! beginHttpDL(&zip_req, &map_continue) ) {
		SOFPPNIX_PRINT("Not the right time to load a map\n");
		return;
	}
	// empty mapname bug ruins cmd_argc count. Used double quotes in ExecuteString
	strcpy(saved_map_arg1,orig_Cmd_Argv(1));
	// http map start here
	map_saving = true;
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
std::array<usercmd_t, 32> client_last_non_skipped_cmd;
std::array<usercmd_t, 32> client_last_last_non_skipped_cmd;
std::array<usercmd_t, 32> client_last_skipped_cmd;
std::array<usercmd_t, 32> client_last_cmd;
std::array<usercmd_t, 32> client_last_last_cmd;
std::array<int, 32> client_toggle_state = {0};

#define NO_TOGGLE 0
#define TOGGLE_DOWN 1
#define TOGGLE_UP 2
#if 0
void my_SV_ClientThink(void *cl, usercmd_t *cmd)
{

	if ( _nix_poorman->value ) {
		bool shouldSkip = false;
		int player = getPlayerSlot(cl);
		// SOFPPNIX_DEBUG("player slot: %d\n", player);

		if ( player == -1 ) error_exit("Illegal player slot");

		// increment frame counter, don't skip valuable button changes, OR defer them to next frame
		if ( ( client_framecounters[player]++ % 2 ) ) {
			// skip this frame
			// Defer all data from previous frame to this one.
			shouldSkip = true;
		}

		// Maybe only focus on '0 1 0'
		// And '1 0 1' Situations.
		// The moment state changes :: 
		// we are replaying a detected toggle, tough luck.
		if (client_toggle_state[player] != NO_TOGGLE && 
			!(client_last_last_cmd[player].buttons & BUTTON_ATTACK) &&
			client_last_cmd[player].buttons & BUTTON_ATTACK &&
			!(cmd->buttons & BUTTON_ATTACK) ) {

			// 0 1 0
			// toggle down
			client_toggle_state[player] = TOGGLE_DOWN;
		}
			
		// reapply the toggle over non-skipped frames
		// depends on what our last value we 'sent' to the game-engine was.
		// if it was a 1, we have to begin with a 0. Cheeky method, check even earlier?
		if ( client_toggle_state[player] != NO_TOGGLE ) {
			// we want to 'write' 0 1 0.
			// If we already have 0 and 1, we only need to write a 0.
			if ( !(client_last_last_non_skipped_cmd[player].buttons & BUTTON_ATTACK) && 
				client_last_non_skipped_cmd[player].buttons & BUTTON_ATTACK ) {
				// 0 1
				// complete
				cmd->buttons &= ~BUTTON_ATTACK;
				client_toggle_state[player] = NO_TOGGLE;
			} else if ( !(client_last_last_non_skipped_cmd[player].buttons & BUTTON_ATTACK) ) {
				// last sent was 0, only have to write 1 and 0
				// completed next frame
				cmd->buttons |= BUTTON_ATTACK;
			} else {
				// we write it all ourselves.
				// 0 1 0
				cmd->buttons &= ~BUTTON_ATTACK;
				// completed in 2 frames.
			}
		}


		// last for both
		client_last_last_cmd[player] = client_last_cmd[player];
		client_last_cmd[player] = *cmd;
		if ( shouldSkip ) {
			// last for skipped
			client_last_skipped_cmd[player] = *cmd;
			return;
		}
		// last for non-skipped
		client_last_last_non_skipped_cmd[player] = client_last_non_skipped_cmd[player];
		client_last_non_skipped_cmd[player] = *cmd;
		// Adjust msec by factor of 2
		cmd->msec *= 2;
	}
	orig_SV_ClientThink(cl,cmd);

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
		orig_SV_ClientThink(cl,cmd-0x18);
	}
	orig_SV_ClientThink(cl,cmd);
}

#endif