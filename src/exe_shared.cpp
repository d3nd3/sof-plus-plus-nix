#include "common.h"

CURL * curl_handle = NULL;

void my_Qcommon_Init(int one , char ** two) {
	orig_Com_Printf("sof++nix_DEBUG: Qcommon_Init\n");

	curl_global_init(CURL_GLOBAL_NOTHING);

	orig_Com_Printf("++nix_DEBUG: Curl Version : %s\n",curl_version());
	orig_Qcommon_Init(one,two);


	// Init has been done!
	// ------------------- CREATING COMMANDS --------------------
	// my_Cmd_AddCommand("testaddserver",&testaddserver);

	// my_Cmd_RemoveCommand("pingservers");
	// my_Cmd_AddCommand("pingservers",&my_CL_PingServers);

	CreateCommands();
}


void my_Qcommon_Shutdown(void)
{
	orig_Com_Printf("sof++nix_DEBUG: Qcommon_Shutdown\n");

	curl_global_cleanup();

	orig_Qcommon_Shutdown();
}
