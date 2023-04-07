#include "common.h"

CURL * curl_handle = NULL;

void my_Qcommon_Init(int one , char ** two) {
	orig_Qcommon_Init(one,two);
	
	SOFPPNIX_PRINT("Initialised.\n");
	SOFPPNIX_DEBUG("Qcommon_Init\n");

	// loadHttpCache();
	curl_global_init(CURL_GLOBAL_NOTHING);

	SOFPPNIX_PRINT("Curl Version : %s\n",curl_version());
	


	// Init has been done!
	// ------------------- CREATING COMMANDS --------------------
	// my_Cmd_AddCommand("testaddserver",&testaddserver);

	// my_Cmd_RemoveCommand("pingservers");
	// my_Cmd_AddCommand("pingservers",&my_CL_PingServers);

	CreateCommands();
}

void my_Qcommon_Shutdown(void)
{
	SOFPPNIX_PRINT("Shutting down.\n");
	SOFPPNIX_DEBUG("Qcommon_Shutdown\n");

	curl_global_cleanup();

	orig_Qcommon_Shutdown();
}

unsigned char my_COM_BlockSequenceCheckByte(unsigned char * src , int len, int out_seq_id )
{

	unsigned char checksum =  orig_COM_BlockSequenceCheckByte(src,len,out_seq_id);

	int out_seq = *(unsigned int*)((*(unsigned int*)0x0829D494) + 0x4F4);
	SOFPPNIX_PRINT("COM_BlockSequenceCheckByte : %02X %i %i\n",checksum,out_seq_id, out_seq);
	return checksum;
}
