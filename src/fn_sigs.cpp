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

void (*orig_MSG_WriteDeltaUsercmd) (sizebuf_t *buf, usercmd_t *from, usercmd_t *cmd) = 0x0811CD90;
void (*orig_PAK_WriteDeltaUsercmd) (void *out_packet, usercmd_t *from, usercmd_t *cmd) = 0x080BA99C;

// ---SERVER-----

void (*orig_SV_Map_f)(void) = 0x080AE3E8;

void (*orig_SV_ClientThink) (void *cl, usercmd_t *cmd) = 0x080B1470;

void ( *orig_PF_Configstring)(int index, char * string) = 0x080a7710;


// ----SHARED-----


char *(*orig_CopyString) (char *in) = 0x08120C3C;
void (*orig_Z_FreeTags) (int tag) = 0x08120DB4;
void (*orig_Z_Free) (void *ptr) = 0x08120CBC;
void (*orig_Sys_Quit) (void) = 0x0820AF40;
void (*orig_Sys_Error) (char *error, ...) = 0x082099A4;

int	(*orig_FS_LoadFile) (char *name, void **buf, bool OverridePak = false) = 0x08124724;
char * (*orig_FS_Userdir)(void) = 0x08126BE0;

void (*orig_SP_Register)(const char *Package) = 0x08131D38;
int (*orig_SP_GetStringID)(char * instr) = 0x081326A4;
const char	*(*orig_SP_GetStringText)(unsigned short ID) = 0x0813279C;//0x0813492C;
void (*orig_SP_Print)(edict_t *ent, unsigned short ID, ...) = 0x081328D0;

int (*orig_SP_SPrint)(char *buffer, int buffer_size, unsigned short ID, ...) = 0x081331D4;

void (*orig_Cbuf_Execute)(void) = 0x08116E38;
void ( * orig_Cbuf_AddText)(char * text) = 0x0811927C;
void (*orig_Cmd_ExecuteString)(const char * string) = 0x08118388;
int     (*orig_Cmd_Argc) (void) = 0x081194B0;
char    *(*orig_Cmd_Argv) (int i) = 0x081194C8;
char    *(*orig_Cmd_Args) (void) = 0x081194FC;
void (*orig_Qcommon_Init)(int one, char** two) = 0x0811E6E8;
void (*orig_Qcommon_Shutdown)(void) = 0x081211E0;

void (*orig_Cmd_AddCommand)(char * cmd, void * callback) = 0x08119514;
void (*orig_Cmd_RemoveCommand)(char * cmd) = 0x081195BC;

void (*orig_Com_Printf) (char *msg, ...) = 0x0811C8F4;

cvar_t *(*orig_Cvar_Get)(const char * name, const char * value, int flags, cvarcommand_t command = NULL) = 0x08121568;

char	* (*orig_va)(char *format, ...) = 0x081ED5DC;
void (*orig_Netchan_OutOfBandPrint) (int net_socket, netadr_t adr, char *format, ...) = 0x0812AEC4;
void (*orig_NET_SendPacket) (netsrc_t sock, int length, void *data, netadr_t to) = 0x0820C940;

qboolean (*orig_NET_StringToAdr) (char *s, netadr_t *a) = 0x0820D710;

unsigned char (*orig_COM_BlockSequenceCheckByte)(unsigned char * src , int len,int out_seq_id ) = 0x08120F7C;

void (*orig_PAK_ReadDeltaUsercmd) (void *in_packet, usercmd_t *from, usercmd_t *move) = 0x080BA5BC;

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
void (*orig_SpawnEntities) (char *mapname, char *entstring, char *spawnpoint) = NULL;

void (*orig_ClientBegin)(edict_t * ent) = NULL;
void (*orig_ClientDisconnect) (edict_t *ent) = NULL;
void (*orig_PutClientInServer) (edict_t *ent) = NULL;

edict_t * (*orig_G_Spawn) (void) = NULL;
// attribute__((thiscall))
void (* orig_setDMMode)(void * self,int newtype) = NULL;