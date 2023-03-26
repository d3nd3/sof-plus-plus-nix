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
	SV_Map
	SV_InitGame
	SV_InitGameProgs
	Sys_GetGameAPI
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

	orig_ShutdownGame = createDetour(game_exports->Shutdown, my_ShutdownGame,5);


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
	SOFPPNIX_PRINT("Shutting down game!!!\n");
	orig_ShutdownGame();


	orig_Cmd_RemoveCommand("fx_save");
	orig_Cmd_RemoveCommand("fx_load");
	/*
		free all detour mallocs
	*/
	free(orig_ShutdownGame);
}