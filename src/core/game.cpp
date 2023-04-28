#include "common.h"

game_export_t * game_exports = NULL;


/*
Calls GetRefAPI from ref_gl.so
also passes in pointers for ref_gl library. ri
*/
/*qboolean VID_LoadRefresh(char *name){
}
*/

/*void * GetRefAPI (void * rimp ) {

}*/

/*
------------------------------------------------------------------------------------
	----------------EXPORTED FROM THE GAME DLL INTO BINARY-----------------
------------------------------------------------------------------------------------
	SV_Map->
		SV_InitGame
			SV_InitGameProgs [ctrl hooks this one and modifies the exported functions]
				Sys_GetGameAPI [so if we used sofplus, if he doesnt call orig, our func is not executed]

	// the init function will only be called when a game starts,
	// not each time a level is loaded.  Persistant data for clients
	// and the server can be allocated in init
	int APIVERSION;
	// internal name = InitGame
	void        (*Init) (void);
	// internal name = ShutdownGame
	void        (*Shutdown) (void);
	// each new level entered will cause a call to SpawnEntities
	void        (*SpawnEntities) (char *mapname, char *entstring, char *spawnpoint);
	// Read/Write Game is for storing persistant cross level information
	// about the world state and the clients.
	// WriteGame is called every time a level is exited.
	// ReadGame is called on a loadgame.
	void        (*WriteGame) (bool autosave);
	bool        (*ReadGame) (bool autosave);
	// ReadLevel is called after the default map information has been
	// loaded with SpawnEntities
	void        (*WriteLevel) (void);
	void        (*ReadLevel) (void);
	void    (*ClientPreConnect) (void*);
	qboolean    (*ClientConnect) (edict_t *ent, char *userinfo);
	void        (*ClientBegin) (edict_t *ent);
	void        (*ClientUserinfoChanged) (edict_t *ent, char *userinfo, bool not_first_time);
	void        (*ClientDisconnect) (edict_t *ent);
	void        (*ClientCommand) (edict_t *ent);
	void        (*ClientThink) (edict_t *ent, usercmd_t *cmd);
	void        (*ResetCTFTeam) (edict_t *ent);
	// internal name G_GameAllowASave
	int         (*GameAllowASave) (void);
	// internal name G_SavesLeft
	void        (*SavesLeft) (void);
	// internal name G_GetGameStats
	void        (*GetGameStats) (void);
	// internal name G_UpdateInven
	void        (*UpdateInven) (void);
	// internal name G_GetDMGameName
	const char  *(*GetDMGameName) (void);
	// internal name G_GetCinematicFreeze
	byte        (*GetCinematicFreeze) (void);
	// internal name G_SetCinematicFreeze
	void        (*SetCinematicFreeze) (byte cf);
	// intenrla name G_RunFrame
	float       (*RunFrame) (int serverframe);

------------------------------------------------------------------------------------
	----------------IMPORTED INTO THE GAME DLL FROM BINARY-----------------
------------------------------------------------------------------------------------
	//FS_FDoesFileExist
	FS_FDoesFileExist = 0x14
	void	(*FS_CreatePath) (char *path); = 0x18
	void	(*FS_FreeFile) (void *buf); = 0x20
	int		(*FS_LoadFile) (char *name, void **buf, bool OverridePak = false); = 0x24

	//Cvar_VariableValue
	float	(*cvar_variablevalue)(const char *var_name); = 0x28
	//Cvar_Info
	char	*(*cvar_info)(int flag); = 0x2C
	//Cvar_ForceSet
	cvar_t	*(*cvar_forceset) (const char *var_name, const char *value); = 0x30
	//Cvar_SetValue
	void	(*cvar_setvalue) (const char *var_name, float value); = 0x34
	//Cvar_Set
	cvar_t	*(*cvar_set) (const char *var_name, const char *value); = 0x38
	//Cvar_Get
	cvar_t	*(*cvar_get) (const char *var_name, const char *value, int flags, cvarcommand_t command = NULL); = 0x3C
	
	//SG_Read
	int			(*ReadFromSavegame)(unsigned long chid, void *address, int length, void **addressptr = NULL); = 0x40
	//SG_Append
	qboolean	(*AppendToSavegame)(unsigned long chid, void *data, int length); = 0x44


	//Z_FreeTags
	void	(*FreeTags) (int tag); = 0x48
	//Z_Free
	void	(*TagFree) (void *block); = 0x4C
	//Z_TagMalloc
	void	*(*TagMalloc) (int size, int tag); = 0x50


	void	(*Pmove) (pmove_t *pmove);		// player movement code common with client prediction = 0x54
	//SV_AreaEdicts
	int		(*BoxEdicts) (vec3_t mins, vec3_t maxs, edict_t **list,	int maxcount, int areatype); = 0x58
	//SV_UnlinkEdict
	void	(*unlinkentity) (edict_t *ent);		// call before removing an interactive edict = 0x5C
	//SV_LinkEdict
	void	(*linkentity) (edict_t *ent); = 0x60

	int		(*irand)(int min, int max); = 0x64
	float	(*flrand)(float min, float max); = 0x68

	void	(*Sys_UnloadPlayer)(int isClient); = 0x6C
	void	*(*Sys_GetPlayerAPI)(void *parmscom,void *parmscl,void *parmssv,int isClient); = 0x70
	void	(*Sys_ConsoleOutput)(char *string); = 0x74

	//PF_error
	void	(*error) (char *fmt, ...); = 0x78

	void	(*SZ_Write)(sizebuf_t *buf, const void *data, int length); = 0x7C
	void	(*SZ_Clear)(sizebuf_t *buf); = 0x80
	void	(*SZ_Init)(sizebuf_t *buf, byte *data, int length); = 0x84

	//PF_Configstring
	void	(*configstring) (int num, char *string); = 0x88

	//sv_isClient
	isClient = 0x8C


	void	(*TurnOffPartsFromGSQFile)(char *dirname, char *poff_file, IGhoulObj *this_object, IGhoulInst *this_inst); = 0x90
	int		(*RegisterGSQSequences)(char *gsqdir, char *subclass, IGhoulObj *object); = 0x94
	void	(*PrecacheGSQFile)(char *dirname, char *gsq_file, IGhoulObj *object); = 0x98
	bool	(*ReadGsqEntry)(int &filesize, char **tdata, char *seqname); = 0x9C
	int		(*FindGSQFile)(char *gsqdir, char *gsqfile, void **buffer); = 0xA0
	IPlayerModelInfoC	*(*NewPlayerModelInfo)(char *modelname); = 0xA4
	void	*(*GetGhoul)(void); = 0xA8


	//CM_AreasConnected
	qboolean (*AreasConnected) (int area1, int area2); = 0xAC
	//CM_SetAreaPortalState
	void	 (*SetAreaPortalState) (int portalnum, qboolean open); = 0xB0
	//PF_inPHS
	qboolean (*inPHS) (vec3_t p1, vec3_t p2); = 0xB4
	//PF_inPVS
	qboolean (*inPVS) (vec3_t p1, vec3_t p2); = 0xB8
	//SV_CheckRegionDistance
	float	 (*RegionDistance) (vec3_t vert); = 0xBC
	//SV_PointContents
	int		 (*pointcontents) (vec3_t point); = 0xC0
	//SV_PolyTrace
	qboolean (*polyTrace) (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passent, int contentmask, trace_t *trace, int (*PolyHitFunc)(trace_t *tr, vec3_t start, vec3_t end, int clipMask)); = 0xC4
	//SV_Trace
	qboolean (*trace) (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passent, int contentmask, trace_t *trace); = 0xC8


	const char	*(*SP_GetStringText)(unsigned short ID); = 0xCC
	int			(*SP_SPrint)(char *buffer, int buffer_size, unsigned short ID, ...); = 0xD0
	void		(*SP_Print_Obit)(edict_t *ent, unsigned short ID, ...); = 0xD4
	void		(*SP_Print)(edict_t *ent, unsigned short ID, ...); = 0xD8
	void		(*SP_Register)(const char *Package); = 0xDC
	void	(*sendPlayernameColors)(edict_t *ent,int len,int numClients,byte *clientColors); = 0xE0


	//PF_GetNearestByteNormal
	int		(*GetNearestByteNormal) (vec3_t dir); = 0xE4
	//SV_ReliableWriteDataToClient
	void	(*ReliableWriteDataToClient)(const void *data,int length,int clientNum); = 0xE8
	//SV_ReliableWriteByteToClient
	void	(*ReliableWriteByteToClient)(byte b,int clientNum); = 0xEC
	//MSG_WriteLong
	void	(*WriteLongSizebuf)(sizebuf_t *sz,int i); = 0xF0
	//MSG_WriteShort
	void	(*WriteShortSizebuf)(sizebuf_t *sz,int s); = 0xF4
	//MSG_WriteByte
	void	(*WriteByteSizebuf)(sizebuf_t *sz,int c); = 0xF8
	//PF_WriteAngle
	void	(*WriteAngle) (float f); = 0xFC
	//PF_WriteDir
	void	(*WriteDir) (vec3_t pos);		// single byte encoded, very coarse = 0x100
	//PF_WritePosition
	void	(*WritePosition) (vec3_t pos);	// some fractional bits = 0x104
	//PF_WriteString
	void	(*WriteString) (char *s); = 0x108
	//PF_WriteFloat
	void	(*WriteFloat) (float f); = 0x10C
	//PF_WriteLong
	void	(*WriteLong) (int c); = 0x110
	//PF_WriteShort
	void	(*WriteShort) (int c); = 0x114
	//PF_WriteByte
	void	(*WriteByte) (int c); = 0x118
	//PF_WriteChar
	void	(*WriteChar) (int c); = 0x11C

	//PF_Unicast
	void	(*unicast) (edict_t *ent, qboolean reliable); = 0x120
	//PF_Multicast
	void	(*multicast) (vec3_t origin, multicast_t to); = 0x124

	//SCR_UpdateLoading
	void	(*Update)(float percent, bool server); = 0x12C
	//DT_GetSurfaceTypes
	int		(*SurfaceTypeList) (byte *mat_list, int max_size); = 0x130
	//DT_DoDamage
	bool	(*DamageTexture) (struct mtexinfo_s *surface, int amount); = 0x134

	//SV_StartSound
	void	(*positioned_sound) (vec3_t origin, edict_t *ent, int channel, int soundinedex, float volume, float attenuation, float timeofs, int localize=SND_LOCALIZE_GLOBAL); = 0x13C
	//PF_StartSound
	void	(*sound) (edict_t *ent, int channel, int soundindex, float volume, float attenuation, float timeofs, int localize=SND_LOCALIZE_GLOBAL); = 0x140


	void	(*Con_ClearNotify) (void); = 0x144
	//PF_captionprintf
	void	(*captionprintf) (edict_t *ent, unsigned short ID); = 0x148
	//SV_BroadcastCaption
	void	(*bcaption) (int printlevel, unsigned short ID); = 0x14C
	//PF_cinprintf
	void	(*cinprintf) (edict_t *ent,int x, int y, int textspeed, char *text); = 0x150
	//PF_centerprintf
	void	(*centerprintf) (edict_t *ent, char *fmt, ...); = 0x154
	//PF_welcomeprint
	void	(*welcomeprint) (edict_t *ent); // 44 = 0x158
	//PF_clprintf
	void	(*clprintf) (edict_t *ent, edict_t *from, int color, char *fmt, ...); = 0x15C
	//PF_cprintf
	void	(*cprintf) (edict_t *ent, int printlevel, char *fmt, ...); = 0x160
	//PF_dprintf
	void	(*dprintf) (char *fmt, ...); = 0x164
	//PF_bprintf
	void	(*bprintf) (int printlevel, char *fmt, ...) = 0x168

	//Cmd_Args
	char	*(*args) (void); = 0x16C
	//Cmd_Argv
	char	*(*argv) (int n); = 0x170
	//Cmd_Argc
	int		(*argc) (void); = 0x174

	//PF_SetRenderModel
	void	(*setrendermodel) (edict_t *ent, char *name); = 0x178
	//PF_SetModel
	void	(*setmodel) (edict_t *ent, char *name); = 0x17C

	//PF_SetGhoulConfigStrings
	void	(*CreateGhoulConfigStrings) (void); = 0x180

	//SV_FilterPacket
	qboolean (*FilterPacket) (char *from); = 0x184
	//SV_UnloadSound
	void	(*unload_sound) (const char *name); = 0x188
	// The *index functions create configstrings and some internal server state.

	//SV_ImageIndex
	int		(*imageindex) (const char *name); = 18C
	//SV_EffectIndex
	int		(*effectindex) (const char *name);fectIndex = 190
	//SV_SoundIndex
	int		(*soundindex) (const char *name); = 194
	//SV_ModelIndex
	int		(*modelindex) (const char *name); = 198
*/
bool first_load = true;
game_export_t * my_Sys_GetGameAPI (void *params) {

	/*
		This function will call dlopen and pass params to GetGameAPI.
		Its some functions that the game library imports.
	*/
	game_exports = orig_Sys_GetGameAPI(params);
	/*
		gamex86.so is loaded here
		ret is a list of function address which are exported to the main executable.
	*/

	/*
		acquire base address.
	*/
	Dl_info info;
	void *handle = dlopen(NULL, RTLD_LAZY);
	if (handle == NULL) {
		error_exit("Failed to load current library\n");
	}
	int rc = dladdr((void*)game_exports->Shutdown, &info);
	if (rc == 0) {
		error_exit("Failed to get current library information\n");
	}
	void *base_addr = info.dli_fbase;
	SOFPPNIX_DEBUG("Base address: %08X\n", base_addr);
	dlclose(handle);

	// I am not editing the return, I am detouring original, but still.
	// Ctrl method of inline patching defeats detours. Take caution.
	orig_SpawnEntities = createDetour(game_exports->SpawnEntities, my_SpawnEntities,5);
	orig_ShutdownGame = createDetour(game_exports->Shutdown, my_ShutdownGame,5);
	
	orig_ClientBegin = createDetour(game_exports->ClientBegin, my_ClientBegin,5);
	orig_ClientDisconnect = createDetour(game_exports->ClientDisconnect, my_ClientDisconnect,5);

	// Must use fixed address on reusable detours.
	orig_PutClientInServer = createDetour((int)base_addr + 0x00238BE8, my_PutClientInServer,6);
	orig_G_Spawn = (int)base_addr + 0x001E5DD0;

	applyDeathmatchHooks(base_addr);

	// ---------------------------WP EDIT---------------------------------
	memoryAdjust(base_addr + 0x14964E,5,0x90);
	memoryAdjust(base_addr + 0x1496D0,5,0x90);

	//-------------------------------SCOREBOARD ALWAYS PRINT-------------------------------------
	memoryAdjust(base_addr + 0xa9bdc,1,0x00);

	// sv_map -> sv_spawnserver ( AFTER SV_InitGame ) -> spawnentities -> fx_init -> addCommand("fx_save");
	// So we are before they attempt to add the commands.
	// fx_init called by qcommon_init double add.
	if( first_load ) {
		first_load = false;
		orig_Cmd_RemoveCommand("fx_save");
		orig_Cmd_RemoveCommand("fx_load");
	}
	return game_exports;
}

void my_ShutdownGame(void)
{
	SOFPPNIX_DEBUG("Shutting down game!!!\n");
	orig_ShutdownGame();


	orig_Cmd_RemoveCommand("fx_save");
	orig_Cmd_RemoveCommand("fx_load");
	/*
		free all detour mallocs
	*/

	free(orig_SpawnEntities);
	free(orig_ShutdownGame);
	
	free(orig_ClientBegin);
	free(orig_ClientDisconnect);

	free(orig_PutClientInServer);

	freeDeathmatchHooks();
}

void my_SpawnEntities(char *mapname, char *entstring, char *spawnpoint)
{

	SOFPPNIX_DEBUG("SpawnEntities!\n");
	orig_SpawnEntities(mapname,entstring,spawnpoint);

	GamespyHeartbeatCtrlNotify();

}

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the game.  This will happen every level load.
============
*/
void my_ClientBegin(edict_t * ent)
{
	
	orig_ClientBegin(ent);

	int slot = ent->s.skinnum;

	SOFPPNIX_DEBUG("Client Begin!\n");

	// Distract.
	spawnDistraction(ent,slot);
}

/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.
============
*/
void my_ClientDisconnect(void)
{
	// free edict
	// if ( distractor[i] ) 
}

/*
===========
PutClientInServer

Called when a player connects to a server or respawns in
a deathmatch.
============
*/
void my_PutClientInServer (edict_t *ent)
{
	orig_PutClientInServer(ent);

	SOFPPNIX_DEBUG("PutClientInServer!\n");

	int slot = ent->s.skinnum;
	SOFPPNIX_DEBUG("Slot: %d\n",slot);
	return;
	detect_max_pitch[slot] = 0;
	detect_max_yaw[slot] = 0;
	distractor[slot] = NULL;

}
