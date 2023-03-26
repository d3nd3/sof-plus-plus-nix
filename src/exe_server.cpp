#include "common.h"

char saved_map_arg1[MAX_STRING_CHARS];
bool map_saved = false;
void map_continue(void)
{
	// DOWNLOAD DONE.
	SOFPPNIX_DEBUG("MAP CONTINUE!!!!!!!!!!!!!!!!!!!\n");
	char cmd[MAX_STRING_CHARS];
	snprintf(cmd,MAX_STRING_CHARS,"map %s %s\n",saved_map_arg1,"fgdd46hg");
	orig_Cmd_ExecuteString(cmd);
}
/*
cmd_argv(0) = map
cmd_argv(1) = mapname
*/
int * cmd_argc = 0x083F9200;
void my_SV_Map_f(void)
{
	if ( map_saved ) {
		// How do know if this was called by callback or by a user.
		if ( orig_Cmd_Argc >= 3 ) {
			if ( !strcmp(orig_Cmd_Argv(2), "fgdd46hg") ) {
				// DOWNLOAD ALREADY DONE.

				*cmd_argc = 2;
				map_saved = false;
				orig_SV_Map_f();
				return;
			}
		}
		
		SOFPPNIX_PRINT("Map download in progress, please wait\n");
		return;
	}
	SOFPPNIX_DEBUG("my_SV_Map_f %s\n",orig_Cmd_Argv(1));
	/*
		Tweak argument to contain .zip for this request.
	*/
	std::string zip_req(std::string(orig_Cmd_Argv(1)) + ".zip");
	if ( ! beginHttpDL(&zip_req, &map_continue) ) {
		SOFPPNIX_PRINT("Not the right time to load a map\n");
		return;
	}

	strcpy(saved_map_arg1,orig_Cmd_Argv(1));
	// http map start here
	map_saved = true;
}