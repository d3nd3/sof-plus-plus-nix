#include "common.h"

extern gamerules_c * currentGameMode;
always_gamerules_c dm_always;

/*
------------------------------------------------------------------
currentGameMode is within our memory

orig is within sof.exe memory
------------------------------------------------------------------
all code leads back to the BASE gamerules_c.
Only if function is not implemented.

dm_always-> ( extra code to always run )
	currentGameMode-> ( your gamerules_c class )
		inheritedGameMode-> ( another gamerules_c class )
			BASE gamerules_c ( if not implemented ).
------------------------------------------------------------------
orig is strictly handled by BASE gamerules_c.
If you implement a function, it is your job to call orig.
------------------------------------------------------------------
setOrigPointer is only ever called by currentGameMode.
So its only valid to call getOrigPointer on currentGameMode.
------------------------------------------------------------------
Let currentGameMode file also only ever call getOrigPointer because if its defined a function, its disabling its base class's version, which would access the orig pointer.
------------------------------------------------------------------
If you currentGameMode->getOrigPointer()->yourFunc() here ( in dm_always.cpp ), it can be called twice, because the functions here always call the currentGameMode. TLDR: Dont want 2 function calls to same function.
------------------------------------------------------------------
-> orig PROPERTY IS NOT SET HERE !! DO NOT USE IT !!!
------------------------------------------------------------------
This file's only job is to call currentGameMode->yourFunc()
Its a PASS-THROUGH.
currentGameMode->yourFunc() MUST be called here..
------------------------------------------------------------------
If you want fallback/default, just don't implement the function in the game mode class
and it will automatically be enabled because of inheritance virtual.

*/


void	always_gamerules_c::clientSetDroppedItemThink(edict_t *ent){
	currentGameMode->clientSetDroppedItemThink(ent);
}
void	always_gamerules_c::clientSetDroppedWeaponThink(edict_t *ent){
	currentGameMode->clientSetDroppedWeaponThink(ent);
}
int		always_gamerules_c::isDM(void){
	return currentGameMode->isDM();
}
/*
	SpawnEntities
		Called by InitDeathmatchSystem() dm.cpp
*/
void	always_gamerules_c::preLevelInit(void){
	#ifdef USE_PYTHON
		// Decorators registered. .py files loaded. Interpreter Reset.
		pythonInit();
	#endif
	currentGameMode->preLevelInit();
}
/*
called by:
SV_DemoMap_f,SV_GameMap_f (console commands).
  SV_Map()
    SV_SpawnServer() 
      ge->SpawnEntities()
        dm->levelInit()
sofplus on_map_begin is also a spawnentities hook but console code is pushed AFTER spawnentities is called so later than this.

There is a strange feature. Called "Defer". In SV_Map @sv_init.c Cbuf_CopyToDefer ();
and SV_Begin_f @sv_user.c Cbuf_InsertFromDefer(). Any console commands inserted on same frame as map change.
Are saved and unloaded when a client connects.

Use Cbuf_ExecuteString here if you don't want deffered commands.
*/
bool intermission_active[32] = {false};
void	always_gamerules_c::levelInit(void){

	next_available_ID = 0;
	std::string sp_name = "nix";
	
	std::string sp_file_path = "strip/" + sp_name + ".sp";
	FILE* depend = nullptr;

	void * buffer = NULL;
	int len = 0;
	if ( (len = orig_FS_LoadFile(sp_file_path.c_str(),&buffer,false)) == -1 ) {
		sp_file_path = orig_FS_Userdir() + std::string("/") + sp_file_path;
		create_file_dir_if_not_exists(sp_file_path.c_str());
		depend = fopen(sp_file_path.c_str(), "wb");
		if (depend == nullptr) {
			error_exit("WARNING: failed to write to %s\n",sp_file_path.c_str());
		}
		else {
			// doesnt' exist
			fprintf(depend, "%s", SOFREESP);
			fclose(depend);
		}
	} else {
		
		std::string crc_current;
		crc_checksum(buffer,crc_current,len);

		std::string crc_inbuilt;
		crc_checksum(SOFREESP, crc_inbuilt,strlen(SOFREESP));
		if ( crc_inbuilt != crc_current)  {
			sp_file_path = orig_FS_Userdir() + std::string("/") + sp_file_path;
			depend = fopen(sp_file_path.c_str(), "wb");
			if (depend == nullptr) {
				error_exit("WARNING: failed to write to %s\n",sp_file_path.c_str());
			}
			else {
				// doesnt' exist
				fprintf(depend, "%s", SOFREESP);
				fclose(depend);
			}
		}
		orig_Z_Free(buffer);
	}

	spackage_register(std::string(sp_name + ".sp").c_str());

	//Python dependant code/resources.
	#ifdef USE_PYTHON
		for ( int i = 0; i < map_spawn_callbacks.size(); i++ ) {
			PyObject* result = PyObject_CallFunction(map_spawn_callbacks[i],"");
			// returns None
			Py_XDECREF(result);
		}

		// chat background
		orig_SV_GhoulFileIndex("c/c.m32");
		// killfeed background
		// orig_SV_GhoulFileIndex("c/k.m32");
		// shotgun
		orig_SV_GhoulFileIndex("c/s1.m32");
		// headshot icon
		orig_SV_GhoulFileIndex("c/hs.m32");
		// skull and cross bones
		orig_SV_GhoulFileIndex("c/sb.m32");
		// C4
		orig_SV_GhoulFileIndex("c/c4.m32");

		// For detecting first frame into intermission.
		for ( int i = 0 ; i < 32 ; i ++ ) {
			intermission_active[i] = false;
		}

		killFeedList.clear();
		killFeedLength = 0;
	#endif

	// dm specific levelinit fallback func
	currentGameMode->levelInit();
}
char	*always_gamerules_c::getGameName(void){
	return currentGameMode->getGameName();
}
int		always_gamerules_c::checkItemSpawn(edict_t *ent,Pickup **pickup){
	return currentGameMode->checkItemSpawn(ent,pickup);
}
int		always_gamerules_c::checkItemAfterSpawn(edict_t *ent,Pickup *pickup){
	return currentGameMode->checkItemAfterSpawn(ent,pickup);
}
/*
Ideal place to put SP clear. Before other SP commands.
Issue: The clear clears the scoreboard too regularly, so can't have scoreboard on slow 3s timer.
Rather want clear to be conditional, based on whether content was drawn.
*/
void	always_gamerules_c::checkEvents(void){

	// void * svs_clients = *(unsigned int*)(*(unsigned int*)(0x0829D134) + 0x10C);
	// for ( int i = 0 ; i < maxclients->value;i++ ) {
	// 	void * a_client_t = svs_clients+i*0xd2ac;
	// 	int state = stget(a_client_t,0);
	// 	if (state != cs_spawned )
	// 		continue;
	// 	// Clear every frame.
	// 	edict_t * ent = stget(a_client_t,CLIENT_ENT);
	// 	// orig_SP_Print(ent,0x0700,"*");
	// }

	// void * svs_clients = *(unsigned int*)(*(unsigned int*)(0x0829D134) + 0x10C);
	// for ( int i = 0 ; i < maxclients->value;i++ ) {
	// 	void * client_t = svs_clients + i * 0xd2ac;
	// 	int state = *(int*)(client_t);
	// 	if (state != cs_spawned )
	// 		continue;
	// 	player_count +=1;
	// }

	/*
		Timelimit by default requires a client to be connected to trigger intermission.
		Because it measures from time = 0. If all players disconnected, it would reset the timelimit.
	*/
	
	float* intermissiontime = stget(base_addr+0x002ACB1C,0) + 0x4F0;
	float * level_time = stget(base_addr+0x002ACB1C,0) + 0x4;
	int* exitintermission = stget(base_addr+0x002ACB1C,0) + 0x4F8;
	// End intermission if has been 60 seconds. Should fix game-freeze issue (leak?).
	if (*intermissiontime && *level_time > *intermissiontime + 60 )
	{
		*exitintermission = 1;
	}

	unsigned int level_framenum = stget(base_addr + 0x002B2500,0);
	// Enter Intermission if timelimit reached, regardless of if player is connected.
	if ( !*intermissiontime && level_framenum >= (timelimit->value * 600) )
	{
		orig_SP_Print (NULL, 0x003c, (short)timelimit->value);

		void (*end_dm_level)(void) = base_addr+0x1c5328;
		end_dm_level(); //BeginIntermission(mapname_from_maplist)
	}

	currentGameMode->checkEvents();
}
void	always_gamerules_c::respawnUntouchedItem(edict_t *ent) {
	currentGameMode->respawnUntouchedItem(ent);
}
/*
 Look in CWeaponInfo.cpp for the PickupList, it defines:
 spawnName,type,spawnParm,respawnTime,defaultCount,defaultMaxCount,index,worldname,pickupstringindex,ammopickupstringindex,itemfullstringindex,flags. For each item.
 eg. armour spawnName is "item_equip_armor" and has respawn time of 30 seconds.
*/
void	always_gamerules_c::setRespawn(edict_t *ent, float delay){
	currentGameMode->setRespawn(ent,delay);

	#if 0
	if ( _sf_sv_force_item_respawntime->value ) {
		// orig_Com_Printf("level_time is %f\n",*level_time);
		ent->nextthink = *level_time + _sf_sv_force_item_respawntime->value;
	}
	#endif
	
}
int		always_gamerules_c::getMaxEncumbrance(void){
	return currentGameMode->getMaxEncumbrance();
}
void 	always_gamerules_c::AssignTeam(edict_t *ent, char *userinfo){
	currentGameMode->AssignTeam(ent,userinfo);
}
void	always_gamerules_c::ClearFlagCount(void){
	currentGameMode->ClearFlagCount();
}
void	always_gamerules_c::FlagCaptured(team_t team){
	currentGameMode->FlagCaptured(team);
}
bool	always_gamerules_c::CaptureLimitHit(int limit){
	return currentGameMode->CaptureLimitHit(limit);
}

void	always_gamerules_c::openLogFile(void){
	currentGameMode->openLogFile();
}
void	always_gamerules_c::closeLogFile(void){
	currentGameMode->closeLogFile();
}
int		always_gamerules_c::dmRule_NO_HEALTH(void){
	return currentGameMode->dmRule_NO_HEALTH();
}
int		always_gamerules_c::dmRule_NO_ITEMS(void){
	return currentGameMode->dmRule_NO_ITEMS();
}
int		always_gamerules_c::dmRule_WEAPONS_STAY(void){
	return currentGameMode->dmRule_WEAPONS_STAY();
}
int		always_gamerules_c::dmRule_NO_FALLING(void){
	return currentGameMode->dmRule_NO_FALLING();
}
int		always_gamerules_c::dmRule_REALISTIC_DAMAGE(void){
	return currentGameMode->dmRule_REALISTIC_DAMAGE();
}
int		always_gamerules_c::dmRule_SAME_LEVEL(void){
	return currentGameMode->dmRule_SAME_LEVEL();
}
int		always_gamerules_c::dmRule_NOWEAPRELOAD(void){
	return currentGameMode->dmRule_NOWEAPRELOAD();
}
int		always_gamerules_c::dmRule_TEAMPLAY(void){
	return currentGameMode->dmRule_TEAMPLAY();
}
int		always_gamerules_c::dmRule_NO_FRIENDLY_FIRE(void){
	return currentGameMode->dmRule_NO_FRIENDLY_FIRE();
}
int		always_gamerules_c::dmRule_FORCE_RESPAWN(void){
	return currentGameMode->dmRule_FORCE_RESPAWN();
}
int		always_gamerules_c::dmRule_NO_ARMOR(void){
	return currentGameMode->dmRule_NO_ARMOR();
}
int		always_gamerules_c::dmRule_INFINITE_AMMO(void){
	return currentGameMode->dmRule_INFINITE_AMMO();
}
int		always_gamerules_c::dmRule_SPINNINGPICKUPS(void){
	return currentGameMode->dmRule_SPINNINGPICKUPS();
}
int		always_gamerules_c::dmRule_FORCEJOIN(void){
	return currentGameMode->dmRule_FORCEJOIN();
}
int		always_gamerules_c::dmRule_ARMOR_PROTECT(void){
	return currentGameMode->dmRule_ARMOR_PROTECT();
}
int		always_gamerules_c::dmRule_BULLET_WPNS_ONLY(void){
	return currentGameMode->dmRule_BULLET_WPNS_ONLY();
}
//88
int 	always_gamerules_c::dmRule_TEAM_REDBLUE(void) {
	return currentGameMode->dmRule_TEAM_REDBLUE();
}
//8c
int		always_gamerules_c::arePlayerNamesOn(void){
	// orig_Com_Printf("arePlayerNamesOn? Before\n");
	return currentGameMode->arePlayerNamesOn();
	// orig_Com_Printf("arePlayerNamesOn? After\n");
}
//90
int		always_gamerules_c::arePlayerTeamsOn(void){
	return currentGameMode->arePlayerTeamsOn();
}
//94
int		always_gamerules_c::allowAutoWeaponSwitch(void){
	return currentGameMode->allowAutoWeaponSwitch();
}
//98
void	always_gamerules_c::initPlayerDMInfo(edict_t *ent){
	currentGameMode->initPlayerDMInfo(ent);
}
//9c
void	always_gamerules_c::registerPlayerIcons(void){
	currentGameMode->registerPlayerIcons();
}
//a0
float	always_gamerules_c::clientCalculateWaver(edict_t *ent, int atkID){
	return currentGameMode->clientCalculateWaver(ent,atkID);
}
//a4
// Is actually called from inside ClientBegin

/*
 My hooks come AFTER ctrl's hooks
 Cos of method i use to hook

 PutClientInServer is wiping client memory
 
 Begin Intermission ->
 ClientBeginServerFrame ->
	respawn()
		PutClientInServer ->
			DM->ClientRespawn
 spectator_respawn ->
 ClientBeginDeathmatch ->
 ClientBegin ->
 	DM->ClientConnect
	PutClientInServer ->
		DM->ClientRespawn
*/
/*
When receiving/extracting arguments, 'O' does not increase ref count.

When creating/calling functions, 'O' does increase ref count.

skinnum is set by PutClientInServer

ent->client not initialised here.
*/

// DONT USE skinnum in THIS FUNCTION, ITS TOO EARLY.
// Any function that this calls, Also cannot use skinnum.
void	always_gamerules_c::clientConnect(edict_t *ent){
	
	//SOFPPNIX_DEBUG("Connecting Player");
	int slot = slot_from_ent(ent);

	// SOFPPNIX_DEBUG("ent = %i",slot);
	
	#ifdef USE_PYTHON
		const char * name = "connect";
		PyObject * connecting_player = createEntDict(ent);
		if ( !connecting_player ) return;
		// Py_INCREF(connecting_player);
		
		for ( int i = 0; i < player_connect_callbacks.size(); i++ ) {
			PyObject* result = PyObject_CallFunction(player_connect_callbacks[i],"O",connecting_player);
			// returns None
			Py_XDECREF(result);
		}
		Py_XDECREF(connecting_player);
	#endif
	currentGameMode->clientConnect(ent);

	#ifdef USE_PYTHON
		void * gclient = stget(ent, EDICT_GCLIENT);
		stset(gclient, GCLIENT_SHOWSCORES,0);
		prev_showscores[slot] = 0;
		current_page[slot] = "scoreboard";
		page_should_refresh[slot] = false;

		layout_clear(LayoutMode::hud,ent);
		layout_clear(LayoutMode::page,ent);
		refreshScreen(ent);
	#endif
	stats_headShots[slot] = 0;
	stats_throatShots[slot] = 0;
	stats_nutShots[slot] = 0;
	stats_armorsPicked[slot] = 0;
	
}
//a8
/*
	example: ent=dieing_player, inflictor=rocket, killer=attacker_player
*/
void	always_gamerules_c::clientDie(edict_t *ent, edict_t *inflictor, edict_t *killer){
	
	// SOFPPNIX_DEBUG("ent : %08X, inflictor : %08X, killer : %08X",ent,inflictor,killer);	

	#ifdef USE_PYTHON
		PyObject * died = createEntDict(ent);
		PyObject * inflicting_ent = createEntDict(ent);
		PyObject * killer_player = createEntDict(ent);

		if ( !died || !inflicting_ent || !killer_player ) return;

		for ( int i = 0; i < player_die_callbacks.size(); i++ ) {

			PyObject* result = PyObject_CallFunction(player_die_callbacks[i],"OOO",died,inflicting_ent,killer_player);
			// returns None
			Py_XDECREF(result);
		}

		Py_XDECREF(died);
		Py_XDECREF(inflicting_ent);
		Py_XDECREF(killer_player);
	#endif
	currentGameMode->clientDie(ent,inflictor,killer);
}
//ac
void	always_gamerules_c::clientDieWeaponHandler(edict_t *ent){
	currentGameMode->clientDieWeaponHandler(ent);
}
//b0
void	always_gamerules_c::clientPreDisconnect(edict_t *ent){
	currentGameMode->clientPreDisconnect(ent);
}
//b4
void	always_gamerules_c::clientDisconnect(edict_t *ent){
	
	#ifdef USE_PYTHON
		PyObject * who = createEntDict(ent);
		if ( !who ) return;
		for ( int i = 0; i < player_disconnect_callbacks.size(); i++ ) {
			PyObject* result = PyObject_CallFunction(player_disconnect_callbacks[i],"O",who);
			// returns None
			Py_XDECREF(result);
		}
		Py_XDECREF(who);
	#endif

	int slot = slot_from_ent(ent);
	// demoResetVarsWeak(slot);

	currentGameMode->clientDisconnect(ent);

}
//b8
void	always_gamerules_c::clientDropItem(edict_t *ent,int type,int ammoCount){
	currentGameMode->clientDropItem(ent,type,ammoCount);
}
//bc
void	always_gamerules_c::clientDropWeapon(edict_t *ent,int type, int clipSize){
	currentGameMode->clientDropWeapon(ent,type,clipSize);
}
//c0
float	always_gamerules_c::always_gamerules_c::clientGetMovescale(edict_t *ent){
	return currentGameMode->clientGetMovescale(ent);
}

/*
G_RunFrame -> ClientEndFrames @g_main.cpp
// THis is called by ClientEndServerFrame
 Its called after Scoreboard, which is useful.

 dm->checkEvents -  Not Active
 dm->ClientScoreboardMessage - Clear if toggle close scoreboard.
 dm->clientEndFrame - overrides G_SetStats. To force Layout on.

 No longer true. Moved ClientScoreboardMessage to G_SetStats hook.
*/
//c4

void	always_gamerules_c::clientEndFrame(edict_t *ent){
	
	currentGameMode->clientEndFrame(ent);
}
//c8
void	always_gamerules_c::clientHandlePredn(edict_t *ent){
	// orig_Com_Printf("clientHandlePredn\n");
	currentGameMode->clientHandlePredn(ent);
}
//cc
void	always_gamerules_c::clientHelpMessage(edict_t *ent){
	// orig_Com_Printf("clientHelpMessage\n");
	currentGameMode->clientHelpMessage(ent);
}
//d0
/*
	The function deals with self-harm first.
*/
void	always_gamerules_c::clientObituary(edict_t *self, edict_t *inflictor, edict_t *attacker){

	int meansOfDeath = stget(stget(base_addr + 0x002AD298,0),0);

	int mod = meansOfDeath & ~MOD_FRIENDLY_FIRE;

	#ifdef USE_PYTHON
		if ( py_killfeed_func ) {
			// SOFPPNIX_DEBUG("MOD : %i" , mod);
			bool generate_death_card = false;
			bool use_default_obit = true;
			switch (mod)
			{
				case MOD_WATER:
				break;

				case MOD_SLIME:
				break;

				case MOD_CRUSH:
				break;

				case MOD_FALLING:
				break;

				case MOD_SUICIDE:
					// kill//swap_team
					// SOFPPNIX_DEBUG("SUICIDE");
				break;
																
				case MOD_EXPLOSIVE:
					// Mb barrel explodes?
					// SOFPPNIX_DEBUG("EXPLOSIVE");
				break;

				case MOD_FIRE:
				break;

				case MOD_LAVA:
				break;

				case MOD_EXIT:

				break;

				case MOD_BARBWIRE:

				break;

				case MOD_DOGBITE:
				break;
			};
			if(attacker == self) {
				generate_death_card = true;
				use_default_obit = false;
				switch (mod)
				{
					case MOD_PHOS_GRENADE:
					break;

					case MOD_ROCKET_SPLASH:
					break;

					case MOD_C4:
						// Works.
						// SOFPPNIX_DEBUG("C4");
					break;

					case MOD_CLAYMORE:
					break;

					case MOD_NEURAL_GRENADE:
					break;
					case MOD_STAR_THROWN:
						// This is grenade
						// SOFPPNIX_DEBUG("NADE");
					break;
					case MOD_GRENADE:
					break;

					default:
						// Generic self death.
					break;
				};
			} else {
				switch (mod)
				{
					case MOD_KNIFE_SLASH:
						
					break;

					case MOD_KNIFE_THROWN:
						
					break;

					case MOD_PISTOL1:
						
					break;

					case MOD_PISTOL2:
						
					break;

					case MOD_MPISTOL:
						
					break;

					case MOD_ASSAULTRIFLE:
						
					break;

					case MOD_SNIPERRIFLE:
						
					break;

					case MOD_AUTOSHOTGUN:
						
					break;

					case MOD_SHOTGUN:
						generate_death_card = true;
						use_default_obit = false;
					break;

					case MOD_MACHINEGUN:
						
					break;

					case MOD_PHOS_GRENADE:
						
					break;

					case MOD_ROCKET:
						
					break;

					case MOD_ROCKET_SPLASH:
						
					break;

					case MOD_MPG:
						
					break;

					case MOD_FLAMEGUN:
						
					break;

					case MOD_FLAMEGUN_NAPALM:
						
					break;

					case MOD_C4:
						generate_death_card = true;
						use_default_obit = false;
					break;

					case MOD_CLAYMORE:
						
					break;

					case MOD_NEURAL_GRENADE:
						
					break;
					
					case MOD_TELEFRAG:
						
					break;

					case MOD_STAR_THROWN:
						// This is grenade
						// SOFPPNIX_DEBUG("NADE");
					break;

					case MOD_GRENADE:
						
					break;

					case MOD_CONC_GRENADE:
						
					break;
				};
			}
			if ( generate_death_card )
				submitDeath(mod,attacker,self);
			if ( use_default_obit )
				currentGameMode->clientObituary(self,inflictor,attacker);
		}
		else {
			currentGameMode->clientObituary(self,inflictor,attacker);
		}
	#else
		currentGameMode->clientObituary(self,inflictor,attacker);
	#endif

}
//d4
/*
 Code for new clients on connection do once here!

 This function is called from within PutClientInServer

  Begin Intermission ->
  ClientBeginServerFrame ->
 	respawn()
 		PutClientInServer ->
 			DM->ClientRespawn
  spectator_respawn ->
  ClientBeginDeathmatch ->
  ClientBegin ->
  	DM->ClientConnect
 	PutClientInServer ->
 		DM->ClientRespawn
*/
void	always_gamerules_c::clientRespawn(edict_t *ent) {
	// client struct is cleared before this. So showscores is always 0.
	// align prev_scores with it.

	#ifdef USE_PYTHON
		int slot = slot_from_ent(ent);
		prev_showscores[slot] = 0;
		refreshScreen(ent);
	#endif

	currentGameMode->clientRespawn(ent);

	#ifdef USE_PYTHON
		PyObject* who = NULL;	
		who = createEntDict(ent);
		if (!who) return;
		// Continue with the code logic using 'who'
		
		for ( int i = 0; i < player_respawn_callbacks.size(); i++ ) {
			PyObject* result = PyObject_CallFunction(player_respawn_callbacks[i], "O", who);
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
	#endif
}

/*
This function was Forced to be called every ClientEndServerFrame.

Active when player has their scoreboard open.
Called by ClientEndServerFrame, every 32 frames. (3.2 seconds)
if (ent->client->showscores && !(level.framenum & 31))
		dm->clientScoreboardMessage(ent,ent->enemy,false);

G_SetStats called by ClientEndServerFrame, sets ent->client->ps.stats[STAT_LAYOUTS] |= 1;
Which is required for client to show any scoreboard / layout strings.

 dm->checkEvents - NotActive.
 clientScorebordMessage - Clear if toggle off.
 dm->clientEndFrame - overrides G_SetStats. To force Layout on.
 {ORDER of DRAWING doesn't really matter unless co-ordinates are equal/overlapping.}

SV_Frame
SV_RunGameFrame
G_RunFrame
  ClientBeginServerFrame
  dm->checkEvents();
  ClientEndServerFrames
	ClientEndServerFrame {This function returns early if intermission is displayed. Thats why watermark not visible}
	  G_SetStats(ent) {if ( dm ) ent->client->ps.stats[STAT_LAYOUTS] |= 1; ent->client->ps.stats[STAT_LAYOUTS] |= 2;}
	  if (ent->client->showscores && !(level.framenum & 31))
		dm->clientScoreboardMessage(ent,ent->enemy,false);
	  dm->clientEndFrame(ent); OVERRIDE G_SETSTATS here.


	refreshScreen() in features/pages.cpp , sets page_should_refresh[slot] = true
	all draw_append functions and layout_clear calls refreshScreen().

	ISSUE: Draw into buffer, vs Draw for refresh. 2 different concepts.

	Should it draw into buffer every frame? I think it has to, it has to be prepared.
	So layout string has to be cleared every frame too.
*/

//d8
#ifdef USE_PYTHON
bool prev_showscores[32] = {0};
std::vector<std::string> chatVectors;

static void runRefreshLayout(edict_t * ent,int slot, int show_scores, edict_t * killer,qboolean log_file)
{
	// SOFPPNIX_DEBUG("%02X",current_page[slot] != std::string("scoreboard"));
	// SOFPPNIX_DEBUG("Page == %s", current_page[slot].c_str());
	// SOFPPNIX_DEBUG("page_should_refresh[slot] = %i",page_should_refresh[slot]);
	// SOFPPNIX_DEBUG("show_scores %i :: prev_showscores[slot] %i",show_scores,prev_showscores[slot]);	

	
	if ( page_should_refresh[slot] )
			page_should_refresh[slot] = false;


	orig_SP_Print(ent,0x0700,"*");

	showScoreboard(ent,slot,show_scores,killer,log_file);
	
	drawKillFeed(ent);


	char total_layout[1024];
	int req_size = snprintf(total_layout,sizeof(total_layout),"%s%s",hud_layout[slot],page_layout[slot]);
	
	if ( req_size >= 1024 ) {
		SOFPPNIX_PRINT("Warning: Truncation occur when drawing.");
	}
	orig_SP_Print(ent,0x0700,total_layout);

	// SOFPPNIX_DEBUG("Layout size : %i for slot : %i",strip_layout_size[slot],slot);

	// If these buffers get cleared, they have to be regenerated fully before next refresh.
	// That is not practical, because it could refresh next frame.

	// Counter argument : they are re-generated by the functions above.
	// Why is this function getting called by python/inbetween complete draws?
	layout_clear(LayoutMode::hud,ent);
	layout_clear(LayoutMode::page,ent);

	prev_showscores[slot] = show_scores;
}
#endif
/*
	We now call this in G_SetStats hook because it allows control during intermission.
	
	The intermission scoreboard shows because of G_SetStats - ent->client->ps.stats[STAT_LAYOUTS] |= 1;
	But because we set that variable directly immediately after, G_SetStats has lost control of the toggle.
*/
void	always_gamerules_c::clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file)
{
	#ifdef USE_PYTHON
		unsigned int level_framenum = stget(base_addr + 0x002B2500,0);
		void * gclient = stget(ent, EDICT_GCLIENT);
		int show_scores = stget(gclient, GCLIENT_SHOWSCORES);

		int slot = slot_from_ent(ent);

		float* intermissiontime = stget(base_addr+0x002ACB1C,0) + 0x4F0;

		if ( (*intermissiontime > 0 && !intermission_active[slot]) || page_should_refresh[slot] || (*intermissiontime == 0 && show_scores != prev_showscores[slot])  ) {
			
			// First frame entering intermission.
			if ( *intermissiontime > 0  && !intermission_active[slot] ) {
				// Show them the scoreboard.
				stset(gclient, GCLIENT_SHOWSCORES,1);
				show_scores = stget(gclient, GCLIENT_SHOWSCORES);

				current_page[slot] = "scoreboard";

				// now last_intermissiontime is intermission.
				// gets reset on levelInit.
				intermission_active[slot] = true;
			}
			
			// General reason to show Scoreboard
			// print_backtrace();
			runRefreshLayout(ent,slot,show_scores,killer,log_file);

		} else if ( *intermissiontime == 0 && show_scores && current_page[slot] == std::string("scoreboard") && !(level_framenum & 31)) {
			// Hard Exception case.

			// print_backtrace();
			runRefreshLayout(ent,slot,show_scores,killer,log_file);
		}
	#else
		currentGameMode->clientScoreboardMessage(ent,killer,log_file);
	#endif
}

//dc
void	always_gamerules_c::clientStealthAndFatigueMeter(edict_t *ent){
	currentGameMode->clientStealthAndFatigueMeter(ent);
}
//e0
void always_gamerules_c::clientTouchConquerTrigger(void * one, void * two) {
	currentGameMode->clientTouchConquerTrigger(one,two);
}
//e4
bool	always_gamerules_c::IsUnprotectedGoreZone(gz_code zone){
	return currentGameMode->IsUnprotectedGoreZone(zone);
}
//e8
bool	always_gamerules_c::IsLimbZone(gz_code zone){
	return currentGameMode->IsLimbZone(zone);
}
//ec
void	always_gamerules_c::HitByAttacker(edict_t *body, edict_t *attacker){
	// orig_Com_Printf("HitByAttacker	\n");
	currentGameMode->HitByAttacker(body,attacker);
}
//f0
bool	always_gamerules_c::IsShootableWeapon(char *weaponname){
	return currentGameMode->IsShootableWeapon(weaponname);
}
//f4
void always_gamerules_c::unsetControlFlag(void * one){
	// orig_Com_Printf("extraFuncTwo\n");
	currentGameMode->unsetControlFlag(one);
}
//f8
void always_gamerules_c::setControlFlag(void * one) {
	// orig_Com_Printf("extraFuncThree\n");
	currentGameMode->setControlFlag(one);
}
//fc
int always_gamerules_c::tooManyFlags(void) {
	// orig_Com_Printf("extraFour\n");
	return currentGameMode->tooManyFlags();
}
//100
void always_gamerules_c::addControlFlag(void * one) {
	// orig_Com_Printf("extraFuncFive\n");
	currentGameMode->addControlFlag(one);
}
//104
void always_gamerules_c::registerConquerTrigger(edict_t * ent){
	currentGameMode->registerConquerTrigger(ent);
}
//108
int always_gamerules_c::isPointInConquerTrigger(void * one) {
	// orig_Com_Printf("extraFuncSeven\n");
	return currentGameMode->isPointInConquerTrigger(one);
}



/*
	Called by shutdownGame as cleanup code
*/
void always_gamerules_c::removeHooks(void) {

	#if 0
	DetourRemove(orig_Cmd_Score_f);

	#endif
	currentGameMode->removeHooks();
}


void always_gamerules_c::applyHooks(void) {

	#if 0
	orig_Cmd_Score_f = (Cmd_Score_f_type)DetourCreate((LPVOID)0x500F6710,(LPVOID)&my_Cmd_Score_f,DETOUR_TYPE_JMP,6);

	#endif
	currentGameMode->applyHooks();
}