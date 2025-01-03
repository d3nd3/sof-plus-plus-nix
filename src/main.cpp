#include "common.h"


// Doesn't like tabs.
const char * SOFREESP = "VERSION 1\n\
ID 7\n\
REFERENCE ++NIX\n\
DESCRIPTION \"SoF Plus PLus Linux.\"\n\
COUNT 5\n\
INDEX 0\n\
{\n\
  REFERENCE LAYOUT_CUSTOM\n\
  FLAGS SP_FLAG_LAYOUT\n\
  TEXT_ENGLISH \"%s\"\n\
}\n\
INDEX 1\n\
{\n\
  REFERENCE CREDIT_CUSTOM\n\
  FLAGS SP_FLAG_CREDIT\n\
  TEXT_ENGLISH \"%s\"\n\
}\n\
INDEX 2\n\
{\n\
  REFERENCE CENTER_LOWER_CUSTOM\n\
  FLAGS SP_FLAG_CAPTIONED SP_FLAG_ALWAYS_PRINT\n\
  TEXT_ENGLISH \"%s\"\n\
}\n\
INDEX 3\n\
{\n\
  REFERENCE CINEMATIC_CUSTOM\n\
  FLAGS SP_FLAG_TYPEAMATIC \n\
  TEXT_ENGLISH \"%s\"\n\
}\n\
INDEX 4\n\
{\n\
  REFERENCE CENTER_CUSTOM\n\
  FLAGS SP_FLAG_CENTERED\n\
  TEXT_ENGLISH \"%s\"\n\
}\n";

void setupMemory(void);

int chdir(const char *path)
{
	// std::cout << "Lol!!" << __builtin_return_address(0);
	static bool initalized = false;
	static int (*orig_chdir)(const char * p);
	if ( !initalized ) {
		orig_chdir = dlsym(RTLD_NEXT,"chdir");
		initalized = true;
		setupMemory();
	}

	return orig_chdir(path);   
}

void segfault_handler(int signal) {
	std::cout << "Caught signal " << signal << std::endl;

	void *bt[10];
	size_t size = backtrace(bt, 10);
	backtrace_symbols_fd(bt, size, STDERR_FILENO);

	exit(1);
}
void __attribute__ ((constructor)) begin() {
	// printf("sof++nix_DEBUG: LD_PRELOAD Constructor\n");

	// Prevent SDL from having ugly message? (seg deploy parachute)
	signal(SIGSEGV, segfault_handler);

	// setupMemory();
}


 void setupMemory() {
	printf("sof++nix_DEBUG: First Init patches + detours\n");


//-----------------ALWAYS 1.07f CLIENT----------------- NO need to use patch
	// # CL_HandleChallenge - change protocol from 32 to protocol 33
	memoryAdjust(0x80C4E4B,1,0x21);
	// CL_ParseServerData - change protocol from 32 to protocol 33
	memoryAdjust(0x80C9AB0,1,0x21);
	// ---------------------MORE 33--------------------
	memoryAdjust(0x080C94B7,1,0x21);
	memoryAdjust(0x80C4FD4,1,0x21);
	// CL_ConnectionlessPacket - disable cert authentication - don't act upon 3rd Argument.
	memoryAdjust(0x80C62CC,7,0x90);
	memoryAdjust(0x80C62D3,1,0xBF);
	memoryAdjust(0x80C62D4,1,0x05);
	memoryAdjust(0x80C62D5,3,0x00);
	// CL_ConnectionlessPacket - challenger handler old style - don't act upon 2nd Argument.
	memoryAdjust(0x80C62F4,12,0x90);

//-----------------ALWAYS 1.07f SERVER TOO ( this is equivalent to patchit_server.sh )--------------------
	// SVC_DirectConnect - server has received `connect` packet from client
	// Do not be Strict about argV return length
	memoryAdjust(0x80AA3DE,1,0x21);
	
	// SVC_Info_f - server sends `info` packet to client
	// Do not be Strict about argV return length
	memoryAdjust(0x80A99D7,1,0x21);

	// SV_New_f Protocol changed from `32` to `33`
	memoryAdjust(0x80B031A,1,0x21);

	// fix original in case used bad patch.
	memoryAdjust(0x80AA49D,1,0x06);
	memoryAdjust(0x80AA46C,1,0x05);

	// ignore connmode check - SVC_DirectConnect , some authorization check
	memoryAdjust(0x80AA6B7,1,0x75);

	// ignore arg4
	memoryAdjust(0x80AA6A9,6,NOP);

//---------------CLIPBOARD---------------
	callE8Patch(0x08113315,&my_Sys_GetClipboardData);

//---------------info %s 33!---------------
	// Receive info packets from protocol 33
	memoryAdjust(0x080A99D7,1,0x21);
	// Send info packets with protocol 33
	memoryAdjust(0x080C5E7C,1,0x21);
	memoryAdjust(0x080C5F02,1,0x21);
	memoryAdjust(0x080C5FE4,1,0x21);
	memoryAdjust(0x080C51B1,1,0x21);

//---------------WON DISABLE---------------
	// NOP WON Stuff?
	//Qcommon_Init ... cWON_General:: cWon_General.
	memoryAdjust(0x0811ED2B,5,NOP);
	//cWon_General Destructor
	memoryAdjust(0x08121212,5,NOP);
	memoryAdjust(0x0811CD3E,5,NOP);
	//Qcommon_INit ... cWON_General:: Register Commands
	memoryAdjust(0x0811ED39,5,NOP);
	//SV_InitGame ... cWon_Server
	memoryAdjust(0x080A89A3,5,NOP);
	//CL_Init ... cWon_Client
	memoryAdjust(0x80C89C0,5,NOP);
	//CL_Init ... cWon_Client:RegisterCommands
	memoryAdjust(0x080C89CE,5,NOP);
	//won_markdisconnection called in menus
	memoryAdjust(0x0825EF65,1,0x00);
	//NewAuthorititveRequest
	memoryAdjust(0x080A9CD9,5,NOP);
	memoryAdjust(0x080A9DA7,5,NOP);
	//FreeAUthorititiveReqwuest
	memoryAdjust(0x080A9CBF,5,NOP);
	memoryAdjust(0x080A9D8D,5,NOP);
	memoryAdjust(0x080AA315,5,NOP);
	memoryAdjust(0x080AA71D,5,NOP);
	memoryAdjust(0x080AA746,5,NOP);
	//cWon removeServer
	memoryAdjust(0x080ACA22,5,NOP);
	memoryAdjust(0x080AD008,5,NOP);
	//cWon Server Destructor
	memoryAdjust(0x08201402,5,NOP);
	memoryAdjust(0x080ACB06,5,NOP);
	//cWon Client Destructor
	memoryAdjust(0x080C8B6A,5,NOP);

//---------------MASTER SERVER LIST---------------
	
	// Temporary allow pass
	memoryAdjust(0x080E4F99,1,0x74);
	// CL_PingServers_f -> allow pass ( no_won check )
	memoryAdjust(0x080C6037,2,NOP);
	// Implement our GetServers
	// Assume it fetches the string : SOF+%16s+%8s+%i/%i/%i+%s+%i+%i\n
	callE8Patch(0x080C6045,&GetServerList);

	// Although, SOF+%16s+%8s+%i/%i/%i+%s+%i+%i\n this is the format that the
	// Menu:ServerList wants
	// Also its the output of the  `pingservers` command.


	// Don't check no_won cvar.0xEB = 2 BYTE JMP!
	memoryAdjust(0x080C5D09,1,0xEB);
	memoryAdjust(0x080C5D2C,1,0xEB);


	// CL_ParseInfoRequest SILENCE COM_PRINTF
	memoryAdjust(0x080C5DB6,5,NOP);
	// Silence CL_Connectionless packets
	memoryAdjust(0x080C60E7,5,NOP);

	// Silence status response (print?)
	// memoryAdjust(0x080C623A,5,NOP);


//---------------------- S_NOSOUND 1 CVAR NOT CRASH -------------------------
	// stop calling Init.
	// memoryAdjust(0x080C8A37,2,0x90);



// -----------------------NON-BLOCKING-SERVER-LIST-------------------------
	callE8Patch(0x0811F183, &nonBlockingServerResponses);


// -------------------------GAMESPY HEARTBEAT-------------------------
	callE8Patch(0x080ABF6C,&GamespyHeartbeat);


	// useless test inside sv_executeclientmessage
	// memoryAdjust(0x080B1780,1,0xEB);


//-------------------------------CONSOLE INPUT DEBUG-----------------
	// memoryAdjust(0x08209ae0,2,NOP);


//----------------------SV_VIOLENCE CL_VIOLENCE DONT REJECT------------------------
	memoryAdjust(0x080AA7B8,1,0xEB);
//---------------------SV_VIOLENCE PURE = TRUE SV_SpawnServer-------------------
	memoryAdjust(0x080A86EF,1,0xEB);
	memoryAdjust(0x080A8703,1,0xEB);


//--------------------------SILENCE CMD_REMOVECOMMAND DONT CARE-------------------------------
	memoryAdjust(0x081195E9,5,NOP);


//---------------------------DISABLE OVERFLOW RELIABLE---------------------------------
	memoryAdjust(0x080AFDA3,5,NOP);
	memoryAdjust(0x080AFDB1,5,NOP);
	//------------------------------TEST RAMP BOOST--------------------------------
	//memoryUnprotect(0x08262F88);
	//*(float*)0x08262F88 = 500.0f;
	//memoryProtect(0x08262F88);

	//----------------------DISABLE SETTING VERTICAL FOV (SoF force horiz 95.0, with dynamic Vert--------------------------
	//@Scr_UpdateScreen() -> V_RenderView()
	// memoryAdjust(0x080D6984,1, 0xDB);
	// memoryAdjust(0x080D6985,1,0xE0);
	// memoryAdjust(0x080D6986,1,0x90);
	// memoryAdjust(0x080D6987,1,0x90);
	// memoryAdjust(0x080D6988,1,0x90);
	// memoryAdjust(0x080D6989,1,0x90);



////////////////////////////////////////////////////



	
	// --------------------------------Shared----------------------------------------
	// printf("first deotur\n");
	//orig_Cmd_AddCommand = createDetour(orig_Cmd_AddCommand, &my_Cmd_AddCommand,5);
	// printf("second deotur\n");
	//orig_Cmd_RemoveCommand = createDetour( orig_Cmd_RemoveCommand, &my_Cmd_RemoveCommand, 5);
	// orig_Com_Printf = createDetour(0x0811C8F4, &my_Com_Printf,9);

	// Apply executable command creation here ( server & client )
	orig_Qcommon_Init = createDetour(orig_Qcommon_Init,&my_Qcommon_Init,5);

	// This is used as the reliable startup function.
	orig_Cbuf_AddLateCommands = createDetour(orig_Cbuf_AddLateCommands,&my_Cbuf_AddLateCommands,5);

	// Cleanup
	orig_Qcommon_Shutdown = createDetour(orig_Qcommon_Shutdown , &my_Qcommon_Shutdown, 5);

	// Useful per frame operations
	orig_Qcommon_Frame = createDetour(orig_Qcommon_Frame , &my_Qcommon_Frame, 5);

	// Can monitor/debug all console usage here.
	//orig_Cbuf_Execute = createDetour(orig_Cbuf_Execute , &my_Cbuf_Execute, 6);

	//orig_COM_BlockSequenceCheckByte = createDetour(orig_COM_BlockSequenceCheckByte, &my_COM_BlockSequenceCheckByte, 5);
	// callE8Patch(0x080B1809,&my_test);

	//detour doesnt work !! related to esp first line?
	//orig_Netchan_Transmit = createDetour(0x0812AFCC, &my_Netchan_Transmit,6);



//-------------------------------------------Client-------------------------------------

	//Used for http downloading.
	orig_CL_Precache_f = createDetour(orig_CL_Precache_f , &my_CL_Precache_f,5);

	//Used for Gamespy serverlist.
	// Might be needed for adjusting the info return payload DM -> 0 CTF -> 4.
	orig_menu_AddServer = createDetour(orig_menu_AddServer,&my_menu_AddServer,5);

	//Useful per frame client.
	orig_CL_Frame = createDetour(orig_CL_Frame , &my_CL_Frame,5);
	//Useful per client init?
	orig_CL_Init = createDetour(orig_CL_Init , &my_CL_Init,5);

	//Debugging packet data.
	//orig_PAK_WriteDeltaUsercmd = createDetour(orig_PAK_WriteDeltaUsercmd , &my_PAK_WriteDeltaUsercmd,5);


	//Video Library Hooking.
	// orig_VID_LoadRefresh = createDetour(orig_VID_LoadRefresh , &my_VID_LoadRefresh, 6);

	orig_VID_GetModeInfo = createDetour(orig_VID_GetModeInfo , &my_VID_GetModeInfo, 7);
	// callE8Patch(0x080C9A85,&my_CL_ClearState);
	// orig_CL_Disconnect = createDetour(orig_CL_Disconnect, &my_CL_Disconnect, 5);
	callE8Patch(0x080CFCB8,&my_CalcFov); //There is special LerpFov that is gun specific.
	orig_V_RenderView = createDetour(orig_V_RenderView, &my_V_RenderView,5);

//-----------------------------------------Server------------------------------------------
	//Server-side http download.
	orig_SV_Map_f = createDetour( orig_SV_Map_f , &my_SV_Map_f, 6);
	// void * addr = 0x080A3E9D;
	// memoryUnprotect(addr);
	// unsigned char * p = addr;
	// p[0] = 0xB8;
	// // *(int*)(p+1) = &my_SV_Map_f - (int)addr - 5;
	// *(int*)(p+1) = &my_SV_Map_f;
	// p[5] = 0x90;
	// memoryProtect(addr);

	//Debugging packet data.
	//orig_PAK_ReadDeltaUsercmd = createDetour(orig_PAK_ReadDeltaUsercmd , &my_PAK_ReadDeltaUsercmd,5);

	//orig_PF_Configstring = createDetour( orig_PF_Configstring, &my_PF_Configstring, 5);

	//Server frame early event. killfeed/python.
	orig_SV_RunGameFrame = createDetour(orig_SV_RunGameFrame,&my_SV_RunGameFrame,5);


	//Debugging clc_stringcmd received from clients.
	//orig_SV_ExecuteUserCommand = createDetour(orig_SV_ExecuteUserCommand,&my_SV_ExecuteUserCommand,5);

	// Used by disableDefaultRelBuffer in serverside demos.
	orig_Netchan_Transmit = createDetour(orig_Netchan_Transmit,&my_Netchan_Transmit,6);
	

	// Part of Serverside Demo when demomap.
	orig_SV_SpawnServer = createDetour(orig_SV_SpawnServer,&my_SV_SpawnServer,5);

	//orig_SV_WriteFrameToClient = createDetour(orig_SV_WriteFrameToClient,&my_SV_WriteFrameToClient,5);
	//callE8Patch(0x080AF8F0,&my_SV_WriteFrameToClient);


	//When serverside demo recording, this sets demoWaiting false. Because a frame is being sent to clients.
	//orig_SV_SendClientDatagram = createDetour(orig_SV_SendClientDatagram,&my_SV_SendClientDatagram,9);
	callE8Patch(0x080AFE74,&my_SV_SendClientDatagram);

	//Used by serverside demos. Modify the received lastframe from client to -1.
	//orig_SV_ExecuteClientMessage = createDetour(orig_SV_ExecuteClientMessage,&my_SV_ExecuteClientMessage,9);
	callE8Patch(0x080AB677,&my_SV_ExecuteClientMessage);

	//Used by serverside demos. So that netchan doesn't get overriden until new connection formed.
	orig_SV_New_f = createDetour(orig_SV_New_f,&my_SV_New_f,6);

	//Reliable ghouldata received during connect is always saved. Because Serverside demos need them.
	orig_GhoulPackReliable = createDetour(orig_GhoulPackReliable,&my_GhoulPackReliable,5);

	//Debugging ghoulpack for serverside demos (can be disabled).
	//orig_GhoulPack = createDetour(orig_GhoulPack,&my_GhoulPack,5);
	callE8Patch(0x080AFAE3,&my_GhoulPack);


	//Debug purposes.
	//orig_GhoulReliableHitwire = createDetour(orig_GhoulReliableHitwire, &my_GhoulReliableHitwire, );
	//callE8Patch(0x080AFB3F,&my_GhoulReliableHitwire);
//-----------------------------------------GAME------------------------------------------
	// Many Game hooks here. GameDetours applied in getgameapi using relative address.
	orig_Sys_GetGameAPI = createDetour(0x08209C50,&my_Sys_GetGameAPI,9);
	// callE8Patch(0x080A736F,&my_Sys_GetGameAPI);

	//-----------------------------SV_CLIENTTHINK now call--------------------------------------
	// orig_SV_ClientThink = createDetour(orig_SV_ClientThink , &my_SV_ClientThink, 5);
	// callE8Patch(0x080B1922,&my_SV_ClientThink);
}

// __builtin_return_address(0)