#include "common.h"


//---------------------------------EXECUTABLE----------------------------

// ---CLIENT-----
char * (*orig_SV_StatusString)(void) = 0x080A97A4;

void (*orig_CL_PingServers)(void) = 0x080C5DE8;

void (*orig_menu_AddServer)(netadr_t addr,char *data) = 0x080E08B0;

// ---SERVER-----


// ----SHARED-----
int     (*orig_Cmd_Argc) (void) = 0x081194FC;
char    *(*orig_Cmd_Argv) (int i) = 0x081194C8;
char    *(*orig_Cmd_Args) (void) = 0x081194B0;
void (*orig_Qcommon_Init)(int one, char** two) = NULL;

void (*orig_Cmd_AddCommand)(char * cmd, void * callback) = NULL;
void (*orig_Cmd_RemoveCommand)(char * cmd) = NULL;

void (*orig_Com_Printf) (char *msg, ...) = NULL;

char	* (*orig_va)(char *format, ...) = 0x081ED5DC;
void (*orig_Netchan_OutOfBandPrint) (int net_socket, netadr_t adr, char *format, ...) = 0x0812AEC4;;

/*
---------------------Ref Library---------------------
*/
void * (*orig_GetRefAPI ) (void* rimp ) = NULL;



/*
---------------------Game Library---------------------
*/
void * (*orig_GetGameAPI) (void * import) = NULL;
void * (*orig_Sys_GetGameAPI)(void * params) = NULL;
void (*orig_ShutdownGame)(void) = NULL;