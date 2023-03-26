#include "common.h"


//---------------------------------EXECUTABLE----------------------------

// ---CLIENT-----
void (*orig_CL_Frame) (int msec) = 0x080C84F8;

char * (*orig_SV_StatusString)(void) = 0x080A97A4;

void (*orig_CL_PingServers)(void) = 0x080C5DE8;

void (*orig_menu_AddServer)(netadr_t addr,char *data) = 0x080E08B0;

void (*orig_SND_Load)(void) = NULL;

qboolean (*orig_CL_CheckOrDownloadFile)(char * filepath) = NULL;
void (*orig_CL_Precache_f)(void) = 0x080CDD48;

void (*orig_CL_RequestNextDownload) (void) = 0x080CCC74;

// Used as 'success' into server event Will call `curl_easy_cleanup()` in both.
void (*orig_CL_RegisterEffects)(void) = 0x0814461C;
void (*orig_CL_Disconnect)(short unknown) = 0x080C5670;

// ---SERVER-----

void (*orig_SV_Map_f)(void) = 0x080AE3E8;

// ----SHARED-----
int (*orig_FS_LoadFile)(char * name,void ** something, bool button) = 0x08124724;
char * (*orig_FS_Userdir)(void) = 0x08126BE0;

void (*orig_Cmd_ExecuteString)(const char * string) = 0x08118388;
int     (*orig_Cmd_Argc) (void) = 0x081194FC;
char    *(*orig_Cmd_Argv) (int i) = 0x081194C8;
char    *(*orig_Cmd_Args) (void) = 0x081194B0;
void (*orig_Qcommon_Init)(int one, char** two) = 0x0811E6E8;
void (*orig_Qcommon_Shutdown)(void) = 0x081211E0;

void (*orig_Cmd_AddCommand)(char * cmd, void * callback) = 0x08119514;
void (*orig_Cmd_RemoveCommand)(char * cmd) = 0x081195BC;

void (*orig_Com_Printf) (char *msg, ...) = 0x0811C8F4;

cvar_t *(*orig_Cvar_Get)(const char * name, const char * value, int flags, cvarcommand_t command = NULL) = 0x08121568;

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
void * (*orig_Sys_GetGameAPI)(void * params) = 0x08209C50;
void (*orig_ShutdownGame)(void) = NULL;