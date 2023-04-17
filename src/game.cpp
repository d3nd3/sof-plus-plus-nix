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

	// I am not editing the return, I am detouring original, but still.
	// Ctrl method of inline patching defeats detours. Take caution.
	orig_ShutdownGame = createDetour(game_exports->Shutdown, my_ShutdownGame,5);
	orig_SpawnEntities = createDetour(game_exports->SpawnEntities, my_SpawnEntities,5);


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
	free(orig_ShutdownGame);
	free(orig_SpawnEntities);
}

void my_SpawnEntities(char *mapname, char *entstring, char *spawnpoint)
{

	SOFPPNIX_DEBUG("SpawnEntities!\n");
	orig_SpawnEntities(mapname,entstring,spawnpoint);

	GamespyHeartbeatCtrlNotify();

	// initialise poorman framecounter
	for (int i = 0; i < client_framecounters.size(); ++i) {
		client_framecounters[i] = 0;
		client_last_non_skipped_cmd[i] = {0};
		client_last_last_non_skipped_cmd = {0};
		client_last_skipped_cmd[i] = {0};
		client_last_cmd[i] = {0};
		client_last_last_cmd = {0};
		client_toggle_state = {0};
	}
}