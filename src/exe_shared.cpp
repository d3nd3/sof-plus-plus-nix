#include "common.h"

CURL * curl_handle = NULL;

void my_Qcommon_Init(int one , char ** two) {
	orig_Qcommon_Init(one,two);
	
	SOFPPNIX_PRINT("Initialised.\n");

	// loadHttpCache();
	curl_global_init(CURL_GLOBAL_NOTHING);

	SOFPPNIX_PRINT("Curl Version : %s\n",curl_version());
	
	// Init has been done!
	// ------------------- CREATING COMMANDS --------------------
	// my_Cmd_AddCommand("testaddserver",&testaddserver);

	// my_Cmd_RemoveCommand("pingservers");
	// my_Cmd_AddCommand("pingservers",&my_CL_PingServers);

	CreateCommands();
	CreateCvars();

	if ( sv_public->value ) {
		SOFPPNIX_PRINT("Server is public.\n");
		// Gamespy Port Init
		gs_select_sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (gs_select_sock < 0) {
			error("Error: Failed to create gamespy socket.\n");
		}
		// Non-blocking
		int flags = fcntl(gs_select_sock, F_GETFL, 0);
		fcntl(gs_select_sock, F_SETFL, flags | O_NONBLOCK);
		// Bind to port
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(gamespyport->value);
		addr.sin_addr.s_addr = INADDR_ANY;

		if (bind(gs_select_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			error("Unable to bind to gamespy port");
		}
	}

	// master checks using src port 28904

	// memset 0 the master
	memset(&sof1master_ip, 0, sizeof(sof1master_ip));
	orig_NET_StringToAdr("sof1master.megalag.org:27900", &sof1master_ip);
	// orig_NET_StringToAdr("192.168.0.54", &sof1master_ip);
	// orig_NET_StringToAdr("172.22.130.228", &sof1master_ip);

}

void my_Qcommon_Shutdown(void)
{

	curl_global_cleanup();

	orig_Qcommon_Shutdown();

	// close the gamespy socket
	if ( sv_public->value && gs_select_sock ) {
		close(gs_select_sock);
	}
}

unsigned char my_COM_BlockSequenceCheckByte(unsigned char * src , int len, int out_seq_id )
{

	unsigned char checksum =  orig_COM_BlockSequenceCheckByte(src,len,out_seq_id);

	int out_seq = *(unsigned int*)((*(unsigned int*)0x0829D494) + 0x4F4);
	SOFPPNIX_PRINT("COM_BlockSequenceCheckByte : %02X %i %i\n",checksum,out_seq_id, out_seq);
	return checksum;
}
