/*
---------------------Executable---------------------
*/

// -------------------Server-----------------

extern void (*orig_GhoulReliableHitwire)(int slot, int frameNum);
extern void my_GhoulReliableHitwire(int slot, int frameNum);

extern int (*orig_GhoulPackReliable)(int slot,int frameNum, char * packInto, int freeSpace,int * written);
extern int my_GhoulPackReliable(int slot,int frameNum, char * packInto, int freeSpace,int * written);

extern int (*orig_GhoulPack)(int slot, int frameNum, float baseTime, unsigned char* dest, int freeSpace);
extern int my_GhoulPack(int slot, int frameNum, float baseTime, unsigned char* dest, int freeSpace);

extern void (*orig_GhoulServerFlushClients)(void);

extern void (*orig_SV_WriteFrameToClient) (client_t *client, sizebuf_t *msg);
extern void my_SV_WriteFrameToClient (client_t *client, sizebuf_t *msg);

extern void (*orig_SV_New_f) (void);
extern void my_SV_New_f(void);

// Builds the string that is sent as heartbeats and status replies
// {E8 DIRECT OVERRIDE}
extern char * (*orig_SV_StatusString)(void);

extern void (*orig_SV_Map_f)(void);
extern void my_SV_Map_f(void);

extern void (*orig_SV_Map) (qboolean attractloop, char *levelstring, qboolean loadgame);

extern void (*orig_SV_ClientThink) (void *cl, usercmd_t *cmd);
extern void my_SV_ClientThink(void *cl, usercmd_t *cmd);

extern void ( *orig_PF_Configstring)(int index, char * string);

extern void (*orig_SV_RunGameFrame) (void);
extern void my_SV_RunGameFrame(void);

extern void (*orig_Cmd_Say_f) (edict_t *ent, qboolean team, qboolean arg0);
extern void my_Cmd_Say_f(edict_t *ent, qboolean team, qboolean arg0);

extern int (*orig_SV_GhoulFileIndex) (char *name);

extern void (*orig_SV_ExecuteUserCommand) (char *s);
void my_SV_ExecuteUserCommand (char *s);

extern void (*orig_SV_SpawnServer) (char *server, char *spawnpoint, server_state_t serverstate, qboolean attractloop, qboolean loadgame);
void my_SV_SpawnServer(char *server, char *spawnpoint, server_state_t serverstate, qboolean attractloop, qboolean loadgame);


extern int (*orig_SV_EffectIndex)( char * name);

extern qboolean (*orig_SV_SendClientDatagram) (client_t *client);
extern qboolean my_SV_SendClientDatagram (client_t *client);

extern void (*orig_SV_ExecuteClientMessage) (client_t *cl);
extern void my_SV_ExecuteClientMessage (client_t *cl);

// -------------------Client------------------

extern int (*orig_HasAttachedEffects)(entity_state_t *curState);
extern void (*orig_CL_Init) (void);
extern void my_CL_Init (void);

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

extern void (*orig_MSG_WriteDeltaUsercmd) (sizebuf_t *buf, usercmd_t *from, usercmd_t *cmd);
extern void (*orig_PAK_WriteDeltaUsercmd) (void *out_packet, usercmd_t *from, usercmd_t *cmd);
extern void my_PAK_WriteDeltaUsercmd(void *out_packet, usercmd_t *from, usercmd_t *cmd);


extern qboolean (*orig_SV_Trace) (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passent, int contentmask, trace_t *trace);

extern void my_CL_ClearState(void);
extern void (*orig_CL_ClearState)(void);

extern void my_CalcFov(void);
extern void (*orig_CalcFov)(void);

extern void (*orig_V_RenderView)( float stereo_separation );
extern void my_V_RenderView( float stereo_separation );

// --Shared--

extern char *(*orig_Cvar_VariableString) (char *var_name);

extern void (*orig_MSG_WriteByte) (sizebuf_t *sb, int c);
extern void (*orig_MSG_WriteLong) (sizebuf_t *sb, int c);
extern void (*orig_MSG_WriteString) (sizebuf_t *sb, char *s);
extern void (*orig_MSG_WriteShort) (sizebuf_t *sb, int c);
extern void (*orig_WriteConfigString)(sizebuf_t *sb, char *s);
extern void (*orig_MSG_WriteDeltaEntity)(struct entity_state_s *from, struct entity_state_s *to
						   , sizebuf_t *msg, qboolean force);

extern void (*orig_SV_Nextserver) (void);
extern void (*orig_SZ_Write) (sizebuf_t *buf, void *data, int length);
extern int (*orig_FS_LoadFile) (char *name, void **buf, bool OverridePak = false);
extern char *(*orig_CopyString) (char *in);
extern void (*orig_Z_FreeTags) (int tag);
extern void (*orig_Z_Free) (void *ptr);
extern void (*orig_Sys_Quit) (void); 
extern void (*orig_Sys_Error) (char *error, ...);

extern void (*orig_SP_Register)(const char *Package);
extern int (*orig_SP_GetStringID)(char * instr);
extern const char	*(*orig_SP_GetStringText)(unsigned short ID);
extern void (*orig_SP_Print)(edict_t *ent, unsigned short ID, ...);
extern int (*orig_SP_SPrint)(char *buffer, int buffer_size, unsigned short ID, ...);


//EXEC_NOW = ExecuteString
//EXEC_INSERT = Cbuf_InsertText
//EXEC_APPEND = Cbuf_AddText
extern void (*orig_Cbuf_Execute)(void);
extern void my_Cbuf_Execute(void);
extern void ( * orig_Cbuf_AddText)(char * text);
extern void (*orig_Cmd_ExecuteString)(const char * string);


extern void (*orig_Cmd_TokenizeString) (char *text, qboolean macroExpand);
extern int     (*orig_Cmd_Argc) (void);
extern char    *(*orig_Cmd_Argv) (int i);
extern char    *(*orig_Cmd_Args) (void);

extern void (*orig_Qcommon_Init)(int one, char** two);
extern void my_Qcommon_Init(int one , char ** two);

extern qboolean (*orig_Cbuf_AddLateCommands)(void);
extern qboolean (my_Cbuf_AddLateCommands)(void);

extern void (*orig_Qcommon_Frame)(int msec);
extern void my_Qcommon_Frame(int msec);

extern void (*orig_Qcommon_Shutdown)(void);
extern void my_Qcommon_Shutdown(void);

extern void (*orig_Cmd_AddCommand)(char * cmd, void * callback);
extern void my_Cmd_AddCommand(char * cmd, void * callback);

extern void (*orig_Cmd_RemoveCommand)(char * cmd);
extern void my_Cmd_RemoveCommand(char * cmd);

extern void (*orig_Com_Printf) (char *msg, ...);
extern void my_Com_Printf(char *msg,...);

extern int (*orig_FS_LoadFile)(char * name,void ** something, bool overidePak=false);
extern char * (*orig_FS_Userdir)(void);

extern unsigned char (*orig_COM_BlockSequenceCheckByte)(unsigned char * src , int len, int out_seq_id);
extern unsigned char my_COM_BlockSequenceCheckByte(unsigned char * src , int len, int out_seq_id);


extern void (*orig_PAK_ReadDeltaUsercmd)(void *in_packet, usercmd_t *from, usercmd_t *move);
extern void my_PAK_ReadDeltaUsercmd (void *in_packet, usercmd_t *from, usercmd_t *move);

extern void (*orig_NET_SendPacket) (netsrc_t sock, int length, void *data, netadr_t to);

extern qboolean	(*orig_NET_StringToAdr) (char *s, netadr_t *a);


//Cvar_Get
extern cvar_t *(*orig_Cvar_Get)(const char * name, const char * value, int flags, cvarcommand_t command = NULL);

// Clipboard
// {E8 DIRECT OVERRIDE}
extern char *my_Sys_GetClipboardData(void);

// ServerSendToStatus

extern char	* (*orig_va)(char *format, ...);

extern void (*orig_Netchan_OutOfBandPrint) (int net_socket, netadr_t adr, char *format, ...);


extern void (*orig_PM_StepSlideMove) (int num);
extern void my_PM_StepSlideMove(int num);

extern void (*orig_Netchan_Transmit) (netchan_t *chan, int length, byte *data);
void my_Netchan_Transmit (netchan_t *chan, int length, byte *data);
void my_Netchan_Transmit_Save (netchan_t *chan, int length, byte *data);
void my_Netchan_Transmit_Playback (netchan_t *chan, int length, byte *data);
void my_Netchan_Patch(netchan_t *chan);


/*
---------------------Ref(Video) Library---------------------
*/
extern void * (*orig_GetRefAPI ) (void* rimp );
extern void * GetRefAPI(void* rimp);


extern qboolean (*orig_VID_LoadRefresh)(char *name);
extern qboolean my_VID_LoadRefresh(char *name);

extern qboolean (*orig_VID_GetModeInfo)( int *width, int *height, int mode );
extern qboolean my_VID_GetModeInfo( int *width, int *height, int mode );


/*
---------------------Game Library---------------------
*/

extern void * (*orig_GetGameAPI) (void * import);
extern void *GetGameAPI (void *import);

extern void * (*orig_Sys_GetGameAPI)(void * params);
extern game_export_t * my_Sys_GetGameAPI (void *params);

extern void (*orig_ShutdownGame)(void);
extern void my_ShutdownGame(void);

extern void (*orig_SpawnEntities) (char *mapname, char *entstring, char *spawnpoint);
extern void my_SpawnEntities (char *mapname, char *entstring, char *spawnpoint);


extern void (*orig_ClientBegin) (edict_t *ent);
extern void my_ClientBegin(edict_t * ent);

extern void (*orig_PutClientInServer) (edict_t *ent);
extern void my_PutClientInServer (edict_t *ent);

extern edict_t * (*orig_G_Spawn) (void);

extern void (*orig_ClientDisconnect) (edict_t *ent);
extern void my_ClientDisconnect(edict_t *ent);

extern void (*orig_ClientUserinfoChanged) (edict_t *ent, char *userinfo, bool not_first_time);
extern void my_ClientUserinfoChanged (edict_t *ent, char *userinfo, bool not_first_time);


// __attribute__((thiscall))
extern void ( *orig_setDMMode)(void * self,int newtype);
extern void my_setDMMode(void * self,int newtype);

extern float (*orig_G_RunFrame) (int serverframe);
extern float my_G_RunFrame (int serverframe);

extern int (*orig_PB_AddArmor)(edict_t *ent, int amount);
extern int my_PB_AddArmor(edict_t *ent, int amount);

extern void (*orig_itemArmorTouch)(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);
extern void my_itemArmorTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf);

extern void	(*orig_clprintf) (edict_t *ent, edict_t *from, int color, char *fmt, ...);
extern void	(*orig_cprintf) (edict_t *ent, int printlevel, char *fmt, ...);
extern void	(*orig_bprintf) (int printlevel, char *fmt, ...);

extern void	(*orig_cinprintf) (edict_t *ent,int x, int y, int textspeed, char *text);
extern void (*orig_welcomeprint) (edict_t * ent);

extern mmove_t	* (*orig_GetSequenceForGoreZoneDeath)(void * self,edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags);
extern mmove_t	* my_GetSequenceForGoreZoneDeath(void * self,edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags);


extern void (*orig_G_SetStats)(edict_t * ent);
extern void my_G_SetStats(edict_t * ent);

extern void (*orig_ClientCommand) (edict_t *ent);
extern void my_ClientCommand (edict_t *ent);


extern void (*orig_T_Damage) (edict_t *targ, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb);
extern void my_T_Damage(edict_t *targ, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb);

extern void (*orig_fireSpas)(weaponFireInfo_t &wf);
extern void my_fireSpas(weaponFireInfo_t &wf);

extern void (*orig_CFXSender_exec)(void * self,int, edict_t *, multicast_t, edict_t *);