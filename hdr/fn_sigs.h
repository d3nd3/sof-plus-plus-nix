/*
---------------------Executable---------------------
*/

// --Server--
// Builds the string that is sent as heartbeats and status replies
// {E8 DIRECT OVERRIDE}
extern char * (*orig_SV_StatusString)(void);

extern void (*orig_SV_Map_f)(void);
extern void my_SV_Map_f(void);

// --Client--
extern void (*orig_CL_Frame) (int msec);
extern void my_CL_Frame(int msec);

extern void my_CL_PingServers(void);
extern void (*orig_CL_PingServers)(void);

extern void my_menu_AddServer(netadr_t addr,char *data);
extern void (*orig_menu_AddServer)(netadr_t addr,char *data);

extern void my_SND_Load(void); //s_nosound doesn't work because null pointers hanging.
extern void (*orig_SND_Load)(void); //use s_initsound 0?

extern qboolean my_CL_CheckOrDownloadFile(char * filepath);
extern qboolean (*orig_CL_CheckOrDownloadFile)(char * filepath);

extern void (*orig_CL_RequestNextDownload) (void);

extern void my_CL_Precache_f(void);
extern void (*orig_CL_Precache_f)(void);

extern void my_CL_RegisterEffects(void);
extern void (*orig_CL_RegisterEffects)(void);

extern void my_CL_Disconnect(short unknown);
extern void (*orig_CL_Disconnect)(short unknown);


// --Shared--
extern void (*orig_Cmd_ExecuteString)(const char * string);
extern int     (*orig_Cmd_Argc) (void);
extern char    *(*orig_Cmd_Argv) (int i);
extern char    *(*orig_Cmd_Args) (void);

extern void (*orig_Qcommon_Init)(int one, char** two);
extern void my_Qcommon_Init(int one , char ** two);

extern void (*orig_Qcommon_Shutdown)(void);
extern void my_Qcommon_Shutdown(void);

extern void (*orig_Cmd_AddCommand)(char * cmd, void * callback);
extern void my_Cmd_AddCommand(char * cmd, void * callback);

extern void (*orig_Cmd_RemoveCommand)(char * cmd);
extern void my_Cmd_RemoveCommand(char * cmd);

extern void (*orig_Com_Printf) (char *msg, ...);
extern void my_Com_Printf(char *msg,...);

extern int (*orig_FS_LoadFile)(char * name,void ** something, bool button);
extern char * (*orig_FS_Userdir)(void);

//Cvar_Get
extern cvar_t *(*orig_Cvar_Get)(const char * name, const char * value, int flags, cvarcommand_t command = NULL);

// Clipboard
// {E8 DIRECT OVERRIDE}
extern char *my_Sys_GetClipboardData(void);

// ServerSendToStatus

extern char	* (*orig_va)(char *format, ...);

extern void (*orig_Netchan_OutOfBandPrint) (int net_socket, netadr_t adr, char *format, ...);


/*
---------------------Ref Library---------------------
*/
extern void * (*orig_GetRefAPI ) (void* rimp );
void * GetRefAPI(void* rimp);


/*
---------------------Game Library---------------------
*/

extern void * (*orig_GetGameAPI) (void * import);
void *GetGameAPI (void *import);

extern void * (*orig_Sys_GetGameAPI)(void * params);
game_export_t * my_Sys_GetGameAPI (void *params);

extern void (*orig_ShutdownGame)(void);
void my_ShutdownGame(void);