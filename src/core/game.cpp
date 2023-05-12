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

void *base_addr = NULL;
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
	base_addr = info.dli_fbase;
	// SOFPPNIX_DEBUG("Base address: %08X\n", base_addr);
	dlclose(handle);

	// I am not editing the return, I am detouring original, but still.
	// Ctrl method of inline patching defeats detours. Take caution.
	orig_SpawnEntities = createDetour(game_exports->SpawnEntities, my_SpawnEntities,5);
	orig_ShutdownGame = createDetour(game_exports->Shutdown, my_ShutdownGame,5);
	
	orig_ClientBegin = createDetour(game_exports->ClientBegin, my_ClientBegin,5);
	orig_ClientDisconnect = createDetour(game_exports->ClientDisconnect, my_ClientDisconnect,5);
	orig_ClientUserinfoChanged = createDetour(game_exports->ClientUserinfoChanged, my_ClientUserinfoChanged,9);
	orig_G_RunFrame = createDetour(game_exports->RunFrame, my_G_RunFrame,5);

	// Must use fixed address on reusable detours.
	orig_PutClientInServer = createDetour((int)base_addr + 0x00238BE8, my_PutClientInServer,6);
	orig_G_Spawn = (int)base_addr + 0x001E5DD0;
	orig_PB_AddArmor = (int)base_addr + 0x00237658;

	applyDeathmatchHooks(base_addr);

	// ---------------------------WP EDIT---------------------------------
	memoryAdjust(base_addr + 0x14964E,5,0x90);
	memoryAdjust(base_addr + 0x1496D0,5,0x90);

	//----------------------------SCOREBOARD ALWAYS PRINT (unlocks layout)-----------------------------
	memoryAdjust(base_addr + 0xa9bdc,1,0x00);
	memoryAdjust(base_addr + 0xA9BD2,2,0x90);

	return game_exports;
}

void my_ShutdownGame(void)
{
	SOFPPNIX_DEBUG("Shutting down game.so\n");
	orig_ShutdownGame();


	/*
		free all detour mallocs
	*/

	free(orig_SpawnEntities);
	free(orig_ShutdownGame);
	free(orig_ClientBegin);
	free(orig_ClientDisconnect);
	free(orig_ClientUserinfoChanged);
	free(orig_G_RunFrame);
	free(orig_PutClientInServer);

	freeDeathmatchHooks();

	base_addr = NULL;
}
/*
===========Can Replace entities here.================
{
"_fade" ".35"
"color" "1.000000 1.000000 0.658824"
"light" "200"
"_color" "1.000000 1.000000 0.658824"
"origin" "384.49 -487.5 248"
"classname" "light"
}
*/
void my_SpawnEntities(char *mapname, char *entstring, char *spawnpoint)
{

	std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>> outer_map;

	std::string input(entstring);
	std::size_t start = input.find('{');
	while (start != std::string::npos) {
		std::size_t end = input.find('}', start);
		if (end == std::string::npos) {
			std::cerr << "Error: Invalid input (missing closing brace)\n";
			return 1;
		}

		// Extract the substring between the braces
		std::string entry_str = input.substr(start + 1, end - start - 1);

		// Parse the attributes in the entry string
		std::unordered_map<std::string, std::string> inner_map;
		std::size_t attr_start = 0;
		while (attr_start < entry_str.size() ) {
			// Find opening double quote
			std::size_t attr_name_start = entry_str.find('"', attr_start);
			if (attr_name_start == std::string::npos) {
				break;
			}
			// Find the attribute name
			std::size_t attr_name_end = entry_str.find('"', attr_name_start + 1);
			if (attr_name_end == std::string::npos) {
				std::cerr << "Error: Invalid input (missing closing quote)\n";
				return 1;
			}
			std::string attr_name = entry_str.substr(attr_name_start + 1, attr_name_end - attr_name_start - 1);

			// Find the attribute value
			std::size_t value_start = entry_str.find('"', attr_name_end + 1);
			if (value_start == std::string::npos) {
				std::cerr << "Error: Invalid input (missing opening quote)\n";
				return 1;
			}
			std::size_t value_end = entry_str.find('"', value_start + 1);
			if (value_end == std::string::npos) {
				std::cerr << "Error: Invalid input (missing closing quote)\n";
				return 1;
			}

			// Add the attribute name-value pair to the inner map
			inner_map[attr_name] = entry_str.substr(value_start + 1, value_end - value_start - 1);

			// Move the start index to the end of the attribute value
			attr_start = value_end + 1;
		}
		// Add the inner map to the outer map using the "classname" attribute value as the key
		std::string classname = inner_map["classname"];
		outer_map[classname].push_back(inner_map);

		// Move the start index to the next '{'
		start = input.find('{', end + 1);
	}

	std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>> new_entries;
	for (const auto& [classname, entries] : outer_map) {
		if (classname == "info_player_team1" || classname == "info_player_team2") {
			for (const auto& entry : entries) {
				std::unordered_map<std::string, std::string> new_entry;
				new_entry["classname"] = "info_player_deathmatch";
				new_entry["origin"] = entry["origin"];
				new_entries["info_player_deathmatch"].push_back(new_entry);
			}
		}
	}
	
	// Apply the modifications
	for (const auto& [classname, entries] : new_entries) {
		outer_map[classname].insert(outer_map[classname].end(), entries.begin(), entries.end());
	}
	
	// Now convert it back into a string.
	std::string new_entstring;
	// The first entry will be "worldspawn"
	new_entstring += "{\n";
	// iterate the attributes in the worldspawn entry
	for (const auto& [attr_name, attr_value] : outer_map["worldspawn"][0]) {
		new_entstring += "\"" + attr_name + "\" \"" + attr_value + "\"\n";
	}
	new_entstring += "}\n";
	for (const auto& [classname, entries] : outer_map) {
		if (classname == "worldspawn") {
			continue;
		}
		for (const auto& entry : entries) {
			new_entstring += "{\n";
			for (const auto& [attr_name, attr_value] : entry) {
				new_entstring += "\"" + attr_name + "\" \"" + attr_value + "\"\n";
			}
			new_entstring += "}\n";
		}
	}
	
	// Print the contents of the outer map
	// for (const auto& [classname, entries] : outer_map) {
	// 	std::cout << "Classname: " << classname << '\n';
	// 	for (const auto& entry : entries) {
	// 		std::cout << "  Entry:\n";
	// 		for (const auto& [attr_name, attr_value] : entry) {
	// 			std::cout << "    " << attr_name << ": " << attr_value << '\n';
	// 		}
	// 	}
	// }

	// std::cout << *new_entstring << std::endl;

	// SOFPPNIX_DEBUG("SpawnEntities!");
	// SOFPPNIX_DEBUG("SpawnEntities! %s",entstring);
	orig_SpawnEntities(mapname,new_entstring.c_str(),spawnpoint);
	// orig_SpawnEntities(mapname,entstring,spawnpoint);

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

	prev_showscores[slot] = false;

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
void my_ClientDisconnect(edict_t * ent)
{
	orig_ClientDisconnect(ent);

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


	int slot = ent->s.skinnum;
	return;
	detect_max_pitch[slot] = 0;
	detect_max_yaw[slot] = 0;
	distractor[slot] = NULL;

}


void my_ClientUserinfoChanged (edict_t *ent, char *userinfo, bool not_first_time)
{
	// SOFPPNIX_DEBUG("User info is : %s\n",userinfo);
	orig_ClientUserinfoChanged(ent,userinfo,not_first_time);
}

float my_G_RunFrame (int serverframe)
{
	// SOFPPNIX_DEBUG("G_RunFrame!\n");
	float ret = orig_G_RunFrame(serverframe);
	/*
		Only place after ClientEndFrames, because ClientScoreboardMessage is calling clear.
		Lets draw layout here.
	*/

	for ( int i = 0 ; i < maxclients->value;i++ ) {
		void * client_t = getClientX(i);
		int state = *(int*)(client_t);
		if (state != cs_spawned )
			continue;
		edict_t * ent = stget(client_t,CLIENT_ENT);
		orig_SP_Print(ent,0x0700,layoutstring);
	}
	return ret;
}