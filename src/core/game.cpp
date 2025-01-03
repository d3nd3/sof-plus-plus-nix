#include "common.h"

game_export_t * game_exports = NULL;

static void my_die_Cmd_Score_f (edict_t *ent);


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
	//PF_centerprintf - center of screen
	void	(*centerprintf) (edict_t *ent, char *fmt, ...); = 0x154
	//PF_welcomeprint - bottom of screen
	void	(*welcomeprint) (edict_t *ent); // 44 = 0x158
	//PF_clprintf - Team Say
	void	(*clprintf) (edict_t *ent, edict_t *from, int color, char *fmt, ...); = 0x15C
	//PF_cprintf - All Say if PRINT_CHAT printlevel used.
	void	(*cprintf) (edict_t *ent, int printlevel, char *fmt, ...); = 0x160
	//PF_dprintf - Debug Printf
	void	(*dprintf) (char *fmt, ...); = 0x164
	//PF_bprintf - Broadcast Printf
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
	int		(*effectindex) (const char *name); = 190
	//SV_SoundIndex
	int		(*soundindex) (const char *name); = 194
	//SV_ModelIndex
	int		(*modelindex) (const char *name); = 198


	//These numbers are back to front. 0x198-Above to get.
*/

void *base_addr = NULL;
game_export_t * my_Sys_GetGameAPI (void *params) {

	SOFPPNIX_DEBUG("--------------Sys_GetGameAPI------------");
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


	//reverse order.
	// SOFPPNIX_DEBUG("AHA  : %08X",0x198 - stget(params,4) );
	orig_clprintf = stget(params,0x198-0x15C);
	orig_cprintf = stget(params,0x198-0x160);
	orig_bprintf = stget(params,0x198-0x168);

	orig_cinprintf = stget(params,0x198-0x150);
	orig_welcomeprint = stget(params,0x198-0x158);
	//0x128 == 0x70 , 0x128 - 0x70 = B8

	//0x38 == 0x168 , 0x168 - 0x38 = 130

	// USE fixed address on reusable detours because they will be re-created after removed.

	// I am not editing the return, I am detouring original, but still.
	// Ctrl method of inline patching defeats detours. Take caution.

	//Entity modification. Level load.
	orig_SpawnEntities = createDetour(game_exports->SpawnEntities, my_SpawnEntities,5);
	//Cleanup.
	orig_ShutdownGame = createDetour(game_exports->Shutdown, my_ShutdownGame,5);
	
	//Client enters level.
	orig_ClientBegin = createDetour(game_exports->ClientBegin, my_ClientBegin,5);
	//Client disconnects.
	orig_ClientDisconnect = createDetour(game_exports->ClientDisconnect, my_ClientDisconnect,5);
	orig_ClientUserinfoChanged = createDetour(game_exports->ClientUserinfoChanged, my_ClientUserinfoChanged,9);
	// orig_G_RunFrame = createDetour(game_exports->RunFrame, my_G_RunFrame,5);

	//Used by pages and .commands
	orig_Cmd_Say_f = createDetour((int)base_addr + 0x00188DB0,my_Cmd_Say_f,9);

	// Client respawns
	orig_PutClientInServer = createDetour((int)base_addr + 0x00238BE8, my_PutClientInServer,6);

	// orig_itemArmorTouch = createDetour((int)base_addr + 0x001BE828,my_itemArmorTouch,5);

	//Stats - armours picked.
	orig_PB_AddArmor = createDetour((int)base_addr + 0x00237658,my_PB_AddArmor,5);

	//Throatshots, nutshots, headshots, as used by singleplayer?.
	orig_GetSequenceForGoreZoneDeath = createDetour((int)base_addr + 0x000EF278 , my_GetSequenceForGoreZoneDeath,6);

	//Scoreboard Modify.
	orig_G_SetStats = createDetour((int)base_addr + 0x0023D324 , my_G_SetStats,6);
	//.commands
	orig_ClientCommand = createDetour(game_exports->ClientCommand , my_ClientCommand,5);
	//Fall Damage modify, extra gore - uses oldDir, obtained below.
	orig_T_Damage = createDetour(base_addr + 0x0018B460,my_T_Damage,6);
	//extra gore
	orig_fireSpas = createDetour(base_addr + 0x0009CEDC,my_fireSpas,9);

	orig_G_Spawn = (int)base_addr + 0x001E5DD0;
	orig_CFXSender_exec = base_addr + 0x001822F4;
	// memfxRunner = stget(stget(base_addr + 0x002ACB34,0),0);
	memfxRunner = stget(base_addr + 0x002ACB34,0);

	//my_setDMMode - allows for more game modes and access to gamemode classes.
	applyDeathmatchHooks(base_addr);

	#ifdef USE_PYTHON
		if ( _nix_py_pages->value ) {
			//------------------------call dm->clientScoreboardMessage every frame. (unlocks layout)-----------------------------
			memoryAdjust(base_addr + 0xa9bdc,1,0x00); //ClientEndServerFrame
			memoryAdjust(base_addr + 0xA9BD2,2,0x90);

			// --------DISABLE SCOREBOARD CLEARS IN ORIGINAL FUNCS SO WE HANDLE CLEAR-------------
			memoryAdjust(base_addr + 0x0015CF14,2,0x90);
			memoryAdjust(base_addr + 0x00163286,2,0x90);
			memoryAdjust(base_addr + 0x001625D3,2,0x90);
			memoryAdjust(base_addr + 0x00164FA9,2,0x90);
			memoryAdjust(base_addr + 0x001673F9,2,0x90);

			//-----------------------player_die Cmd_Score_f() hook disables showing score on death-----------------
			callE8Patch(base_addr + 0x00237F17,&my_die_Cmd_Score_f);

			//----------------------NOP dm->clientScoreboardMessage() in clientEndServerFrame--------------
			// We now call this in G_SetStats hook because it allows control during intermission
			memoryAdjust(base_addr + 0x000A9BFC,2,0x90);
		}
		if ( _nix_py_killfeed->value ) {
			// Usage. Killfeed.
			// ----------------------ATTEMPT REMOVE DM RANKING HUD----------------------------
			memoryAdjust(base_addr + 0x0023D663,7,0x90);
		}
	#endif

	if ( _nix_snd_no_feet->value ) {
		//-------------------------DISABLE FOOTSTEPS--------------------------------------------
		memoryAdjust(base_addr + 0x000E23A8,5,0x90);
		memoryAdjust(base_addr + 0x000E250C,5,0x90);
	}
	
	if ( _nix_snd_no_jump->value ) {
		//--------------------------DISABLE JUMP SOUND--------------------------------
		memoryAdjust(base_addr + 0x0023B6BA,2,0x90);
	}

	if ( _nix_snd_no_weap_touch->value ) {
		//-----------------------DISABLE WEAP TOUCH SOUND------------------------------
		memoryAdjust(base_addr + 0x001BE4ED,2,0x90);
		memoryAdjust(base_addr + 0x001BE308,2,0x90);
		memoryAdjust(base_addr + 0x001BE16B,2,0x90);
	}


	// ---------------------------WP EDIT disable spam-------------------------
	memoryAdjust(base_addr + 0x14964E,5,0x90);
	memoryAdjust(base_addr + 0x1496D0,5,0x90);

	//-----------------------REMOVE SEXIST FEMALE DAMAGE-----------------------------
	memoryAdjust(base_addr + 0x00237355,4,0x90);
	memoryAdjust(base_addr + 0x00237355,1,0xdd);
	memoryAdjust(base_addr + 0x00237356,1,0xdf);


	// -----------------DISABLE PLAYERS COMMAND LEAKS IP----------------------------
	memoryAdjust(base_addr + 0x00189F7B,5,0x90);

	if ( _nix_extra_shotgun_gore->value ) {
		//extra gore Attempt to re-enable shotgun gore that was removed.
		// -------------(fireSpas() w_fire.cpp) Get OLDDIR for Shotgun Gore restoration----------------------
		callE8Patch(base_addr + 0x0009D10B,&fake_SV_Trace);
		memoryAdjust(base_addr + 0x009D110,3,0x90);
	}

	//-----------------------Q2 player bbox-----------------------------
	// void * pp = base_addr + 0x0027BF3C;
	// memoryUnprotect(pp);
	// *(float*)(pp) = 32.0f;
	// memoryProtect(pp);

	
	return game_exports;
}

/*
	Only called when map command issued.
	End of level loads do not trigger this.
*/
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
	// free(orig_G_RunFrame);
	free(orig_Cmd_Say_f);
	free(orig_PutClientInServer);
	free(orig_PB_AddArmor);
	free(orig_GetSequenceForGoreZoneDeath);
	free(orig_G_SetStats);
	free(orig_ClientCommand);
	free(orig_T_Damage);
	free(orig_fireSpas);

	freeDeathmatchHooks();

	// chatVectors.clear();

	base_addr = NULL;
}
//entities[classname] of vector[] of attributes[field] of value
//vector because there is 1:many with same classname.
void entstring_to_map(char * entstring,std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>& outer_map)
{

	//parse entstring

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

}

//entities[classname] of vector[] of attributes[field] of value
//vector because there is 1:many with same classname.
void map_to_entstring(std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>& outer_map, std::string& out)
{
	
	// The first entry will be "worldspawn"
	out += "{\n";
	// iterate the attributes in the worldspawn entry
	for (const auto& [attr_name, attr_value] : outer_map["worldspawn"][0]) {
		out += "\"" + attr_name + "\" \"" + attr_value + "\"\n";
	}
	out += "}\n";
	for (const auto& [classname, entries] : outer_map) {
		if (classname == "worldspawn") {
			continue;
		}
		for (const auto& entry : entries) {
			out += "{\n";
			for (const auto& [attr_name, attr_value] : entry) {
				out += "\"" + attr_name + "\" \"" + attr_value + "\"\n";
			}
			out += "}\n";
		}
	}
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
//entities[classname] of vector[] of attributes[field] of value
//vector because there is 1:many with same classname.
void my_SpawnEntities(char *mapname, char *entstring, char *spawnpoint)
{

	std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>> entMap;

	entstring_to_map(entstring,entMap);

	//create info_player_deathmatch spawns for every red/blue spawn
	std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>> new_entries;
	for (const auto& [classname, entries] : entMap) {
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
		   entMap[classname].insert(entMap[classname].end(), entries.begin(), entries.end());
	}
	
	// Print the contents of the outer map
	// for (const auto& [classname, entries] : entMap) {
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
	std::string new_entstring;
	map_to_entstring(entMap,new_entstring);
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

	// int slot = ent->s.skinnum;
	int slot = slot_from_ent(ent);

	#ifdef USE_PYTHON
	prev_showscores[slot] = false;
	#endif
	// Distract.
	//spawnDistraction(ent,slot);
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

client is set to 0 inhere and re-initialised.
Thus everytime you die, you re-initialised.
memset (client, 0, sizeof(*client));
*/
void my_PutClientInServer (edict_t *ent)
{
	// careful using ent->s.skinnum here, ojnly set in PutClientInServer
	orig_PutClientInServer(ent);
	// int slot = ent->s.skinnum;
	int slot = slot_from_ent(ent);

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

/*
Disabled this because it floods the clients and they overflow on vid_restart
*/
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
		//instead it only refreshes when it has to.
		// orig_SP_Print(ent,0x0700,layoutstring);
	}
	return ret;
}

/*
TOOD: Filter weaponselect etc?
*/
void my_Cmd_Say_f(edict_t *ent, qboolean team, qboolean arg0)
{
	int slot = slot_from_ent(ent);
	// char	text[151];

	/*
	if ( arg0 ) {
		snprintf(text,151,"%s %s\n",orig_Cmd_Argv(0),orig_Cmd_Args())
	} else {
		snprintf(text,151,"%s\n",orig_Cmd_Args())
	}
	*/
	int i,first;
	int chars_spoken = 0;
	if ( arg0 ) {
		// concatenate argv(0) with cmd_args
		// command line sent as argv parts
		// SOFPPNIX_DEBUG("ARG0");
		first = 0;
		chars_spoken += strlen(orig_Cmd_Argv(0));
		chars_spoken += strlen(orig_Cmd_Args());

		void * client_t = getClientX(slot);
		int state = *(int*)(client_t);
			
		if ( state != cs_spawned || chars_spoken <= 0 ) return;
	} else {
		// Just use cmd_Args here
		//text here its sent all in say "argv[1]"
		first = 1;
		chars_spoken += strlen(orig_Cmd_Args());
		if ( chars_spoken == 0 || (chars_spoken == 2 && !strcmp(orig_Cmd_Args(),"\"\"") ) ) 
			return;

		if ( orig_Cmd_Args()[0] == '"' && orig_Cmd_Args()[chars_spoken-1] == '"' )
			chars_spoken -=2;
	}
	// SOFPPNIX_DEBUG("Chars spoken : %i",chars_spoken);
	i = first;
	#ifdef USE_PYTHON
		// Create an empty list
		PyObject* pyList = PyList_New(0);
	#endif
	char chatline[64];
	char tmp[sizeof(chatline)];
	tmp[0] = 0x00;
	chatline[0] = 0x00;
	bool once = false;
	// SOFPPNIX_DEBUG("argc == %i\n",orig_Cmd_Argc());
	while(i < orig_Cmd_Argc()) {

		#ifdef USE_PYTHON
			// Create a new string object
			PyObject* pyString = PyUnicode_DecodeFSDefault(orig_Cmd_Argv(i));
			if (  pyString != NULL ) {
				// Append the string to the list
				PyList_Append(pyList, pyString);
				Py_DECREF(pyString);
			}
		#endif
		char * use;
		char * empty = "";
		char * space = " ";
		if ( !once  ) {
			once = true;
			use = empty;	
		}
		else {
			use = space;
		}

		snprintf(chatline,sizeof(chatline),"%s%s%s",tmp,use,orig_Cmd_Argv(i));
		strcpy(tmp,chatline);
		// SOFPPNIX_DEBUG("inebetween :%s",chatline);
		// strcat(chatline,orig_Cmd_Argv(i));
		i++;
	}

	void * gcl = stget(ent,EDICT_GCLIENT);
	unsigned char * name = gcl + GCLIENT_PERS_NETNAME;

	// SOFPPNIX_DEBUG("Name = %.*s",16,name);
	char final[2*sizeof(chatline)];
	int color_count = 0;
	for ( int i = 0; i < 16; i++ ) {
		// SOFPPNIX_DEBUG("NAME : %02X",name[i]);
		if ( name[i] == 0x00 ) break;
		if (name[i] < 32) color_count++;
	}
	// colr-less name
	// Need to give more buffer space to colored tags, so pads evenly
	if ( name[0] > 31 || name[0] == 0x00 ) {
		color_count+=1;
		// give them cyan colour name.
		snprintf(final,sizeof(chatline)+color_count,P_CYAN"%s [%i] %s",name,slot,chatline);
	} else {
		snprintf(final,sizeof(chatline)+color_count,"%s [%i] %s",name,slot,chatline);
	}
	
	// SOFPPNIX_DEBUG("final = %i, color_count %i",strlen(final),color_count);

	#ifdef USE_PYTHON
		PyObject* who = NULL;	
		who = createEntDict(ent);
		if (!who) return;
		// buffers are copied internally.
		for ( int i = 0; i < player_say_callbacks.size(); i++ ) {
			PyObject* result = PyObject_CallFunction(player_say_callbacks[i], "OO", who,pyList);
			if (result == NULL) {
				// Error occurred during the function call
				PyErr_Print();  // Print the error information
				// Handle the error
			} else {
				// Process the result
				// ...
				Py_XDECREF(result);  // Release the reference to the result object
			}
		}
		Py_XDECREF(who);
		Py_XDECREF(pyList);
	#endif
#if 0
	if ( text[0] == '.'' ) {
		//Both clprintf and cprintf make beep sounds ( if PRINT_CHAT used ).
		// 0 = all say, inserts name
		// 1 = team say , inserts name
		// orig_clprintf(ent,ent,1,"%s\n","Just A Test");
		// orig_cprintf(ent,PRINT_CHAT,"%s","Just a Test");
		orig_cprintf(ent,PRINT_HIGH,P_RED ".players is command is disabled\n");
		return;
	}
#endif
	// Don't "say" commands. 
	if ( chatline[0] == '.'  )
		return;
	
	//pages requires python.
	#ifdef USE_PYTHON
		chatVectors.push_back(std::string(final));
		for ( int i = 0 ; i < maxclients->value;i++ ) {
			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;
			edict_t * ent = stget(client,CLIENT_ENT);
			refreshScreen(ent);
		}
	#endif
	orig_Cmd_Say_f(ent,team,arg0);
	// orig_cprintf(NULL,PRINT_CHAT,"msg from [%i] \"%s\" ...\n",ent->s.skinnum,name);
	// orig_bprintf(PRINT_CHAT,"msg from [%i] \"%s\" ...\n",ent->s.skinnum,name);
}


void my_itemArmorTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	
	orig_itemArmorTouch(self,other,plane,surf);
}

int stats_headShots[32];
int stats_throatShots[32];
int stats_nutShots[32];
/*
	T_Damage() @g_combat.cpp
		PB_Damage() @p_body.cpp
			PB_PlaySequenceForDeath() @p_body.cpp
				GetSequenceForDeath() @ai_bodyhuman.cpp

*/
mmove_t	* my_GetSequenceForGoreZoneDeath(void * self,edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	int attack_slot = slot_from_ent(attacker);
	void * level = stget(base_addr + 0x002ACB1C,0);
	int throatshots_before = stget(level,0x580);
	int nutshots_before = stget(level,0x584);
	int headshots_before = stget(level,0x588);
	
	mmove_t * ret;
	ret = orig_GetSequenceForGoreZoneDeath(self,monster,inflictor,attacker,damage,point,dflags,goal_bbox,ideal_move,reject_actionflags);

	int throatshots_after = stget(level,0x580);
	int nutshots_after = stget(level,0x584);
	int headshots_after = stget(level,0x588);

	if ( throatshots_after > throatshots_before ) {
		stats_throatShots[attack_slot] +=1;
	}
	if ( nutshots_after > nutshots_before ) {
		stats_nutShots[attack_slot] +=1;
	}
	if ( headshots_after > headshots_before ) {
		t_damage_was_heashot = true;
		stats_headShots[attack_slot] +=1;
	}

	// SOFPPNIX_DEBUG("Headshots before : %i, Headshots after : %i",headshots_before,headshots_after);

	return ret;
}
int stats_armorsPicked[32];
int my_PB_AddArmor(edict_t *ent, int amount)
{
	int slot = slot_from_ent(ent);
	int ret = orig_PB_AddArmor(ent,amount);
	if (ret) {
		stats_armorsPicked[slot] +=1;
	}
	return ret;
}

static void my_die_Cmd_Score_f (edict_t *ent)
{

}


void my_G_SetStats(edict_t * ent)
{
	orig_G_SetStats(ent);

	#ifdef USE_PYTHON
		int slot = slot_from_ent(ent);
		void * client = getClientX(slot);
		// SOFPPNIX_DEBUG("cclient : %08X",client);
		if ( stget(client,0) == cs_spawned ) {

			// force layouts ON.
			ent->client->ps.stats[STAT_LAYOUTS] |= 1;
		}

	// only way to influence scoreboard during intermission.
	dm_always.clientScoreboardMessage(ent,ent->enemy,false);

	#endif
}



/*
This function by default will not allow any command to become say,
because it returns if intermission_time is active, half-way through.
*/
void my_ClientCommand (edict_t *ent)
{
	
	char * cmd = orig_Cmd_Argv(0);
// SOFPPNIX_DEBUG("Command issued! %s %i",cmd,orig_Cmd_Argc());
	if ( cmd[0] == '.' ) {
		my_Cmd_Say_f(ent,false,true);
		return;
	}
	gclient_t * gclient = stget(ent,EDICT_GCLIENT);
	int spectator = stget(gclient,GCLIENT_RESP_SPECTATOR);
	// SOFPPNIX_DEBUG("Spectator : %i",spectator);
	if ( spectator && inv_cmd_exist.count(cmd) ) {
		// Spectator should not output this command as text.
		return;
	}
	orig_ClientCommand(ent);
}

/*
============
T_Damage

targ		entity that is being damaged, e.g. monster.
inflictor	entity that is causing the damage, e.g. rocket.
attacker	entity that caused the inflictor to damage, e.g. player.
dir			direction of the attack.
point		point at which the damage is being inflicted.
damage		amount of damage being inflicted.
knockback	force to be applied against targ as a result of the damage.
dflags		these flags are used to control how T_Damage works.
mod			the means of death (should the target of the damage happen to die).
============
*/
bool t_damage_was_heashot = false;
void my_T_Damage(edict_t *targ, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb)
{
	t_damage_was_heashot = false;

	// Not sure why fall has seemingly random damage. ( network packets? )
	if (dflags & DT_FALL) {
		damage *= _nix_fall_dmg_scale->value;
	}

	if ( _nix_extra_shotgun_gore->value && shotgunStudy && damage != 12 ) {
		// Apply shotgun gore here.
		applyShotgunGore(targ); //uses oldDir.
	}
	orig_T_Damage(targ,inflictor,attacker,dir,point,origin,damage,knockback,dflags,mod,penetrate,absorb);
}