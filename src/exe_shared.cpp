#include "common.h"

CURL * curl_handle = NULL;

void my_Qcommon_Init(int one , char ** two) {
	SOFPPNIX_PRINT("Qcommon_Init\n");

	// loadHttpCache();
	curl_global_init(CURL_GLOBAL_NOTHING);

	SOFPPNIX_PRINT("Curl Version : %s\n",curl_version());
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
	SOFPPNIX_PRINT("Qcommon_Shutdown\n");

	curl_global_cleanup();

	orig_Qcommon_Shutdown();
}
