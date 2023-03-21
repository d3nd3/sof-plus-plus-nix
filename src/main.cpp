#include "common.h"

#define NOP 0x90
void __attribute__ ((constructor)) begin() {
    printf("Constructor\n");

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

//---------------SERVER TALK TO MASTER---------------
    // Enable Server To Send Master Heartbeats @ Master_Heartbeat_F calls "SV_StatusString".
    // Assume it builds the string : SOF+%16s+%8s+%i/%i/%i+%s+%i+%i\n
    callE8Patch(0x80ABF6C, &my_SendToMasters);

    // once you are on the server list
    // you might want to edit the response from an info request
    // at SVC_Info

//---------------------- S_NOSOUND 1 CVAR NOT CRASH -------------------------
    // stop calling Init.
    // memoryAdjust(0x080C8A37,2,0x90);

////////////////////////////////////////////////////
    
    orig_Cmd_AddCommand = createDetour(0x08119514, &my_Cmd_AddCommand,5);
    orig_Cmd_RemoveCommand = createDetour(0x081195BC , &my_Cmd_RemoveCommand, 5);
    orig_Com_Printf = createDetour(0x0811C8F4, &my_Com_Printf,9);

    // Apply game detours here
    orig_Sys_GetGameAPI = createDetour(0x08209C50,&my_Sys_GetGameAPI,9);

    // Apply executable command creation here ( server & client )
    orig_Qcommon_Init = createDetour(0x0811E6E8,&my_Qcommon_Init,5);

    // Might be needed for adjusting the info return payload DM -> 0 CTF -> 4.
    orig_menu_AddServer = createDetour(0x080E08B0,&my_menu_AddServer,5);

}



// __builtin_return_address(0)