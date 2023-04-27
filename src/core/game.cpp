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
	SV_Map->
		SV_InitGame
			SV_InitGameProgs [ctrl hooks this one and modifies the exported functions]
				Sys_GetGameAPI [so if we used sofplus, if he doesnt call orig, our func is not executed]

	// the init function will only be called when a game starts,
	// not each time a level is loaded.  Persistant data for clients
	// and the server can be allocated in init
	int APIVERSION;
	void        (*Init) (void);
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
	int         (*GameAllowASave) (void);
	void        (*SavesLeft) (void);
	void        (*GetGameStats) (void);
	void        (*UpdateInven) (void);
	const char  *(*GetDMGameName) (void);
	byte        (*GetCinematicFreeze) (void);
	void        (*SetCinematicFreeze) (byte cf);
	float       (*RunFrame) (int serverframe);
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
