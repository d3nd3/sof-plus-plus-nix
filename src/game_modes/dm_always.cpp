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
void	always_gamerules_c::preLevelInit(void){
	currentGameMode->preLevelInit();
}
/*
called by SpawnEntities
sofplus on_map_begin is also a spawnentities hook but console code is pushed AFTER spawnentities is called so later than this.

There is a strange feature. Called "Defer". In SV_Map @sv_init.c Cbuf_CopyToDefer ();
and SV_Begin_f @sv_user.c Cbuf_InsertFromDefer(). Any console commands inserted on same frame as map change.
Are saved and unloaded when a client connects.

Use Cbuf_ExecuteString here if you don't want deffered commands.
*/
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

	std::string cmd = "++nix_spackage_register " + sp_name + ".sp\n";
	// spackage_register handles crc and ID correction.
	orig_Cmd_ExecuteString(cmd.c_str());
	// sets build number bottom right
	orig_Cmd_ExecuteString("++nix_draw_clear\n");

	for ( int i = 0; i < map_spawn_callbacks.size(); i++ ) {
		PyObject* result = PyObject_CallFunction(map_spawn_callbacks[i],"");
		// returns None
		Py_XDECREF(result);
	}

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
Issue: The clear clears the scoreboard, so can't have scoreboard on slow timer.
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
 spectator_respawn ->
 ClientBeginDeathmatch ->
 ClientBegin ->
	PutClientInServer ->
		DM->ClientRespawn
*/
/*
When receiving/extracting arguments, 'O' does not increase ref count.

When creating/calling functions, 'O' does increase ref count.
*/
void	always_gamerules_c::clientConnect(edict_t *ent){
	const char * name = "connect";
	PyObject * connecting_player = createEntDict(ent);
	// Py_INCREF(connecting_player);
	
	for ( int i = 0; i < player_connect_callbacks.size(); i++ ) {
		PyObject* result = PyObject_CallFunction(player_connect_callbacks[i],"O",connecting_player);
		// returns None
		Py_XDECREF(result);
	}
	Py_XDECREF(connecting_player);
	currentGameMode->clientConnect(ent);
}
//a8
/*
	example: ent=dieing_player, inflictor=rocket, killer=attacker_player
*/
void	always_gamerules_c::clientDie(edict_t *ent, edict_t *inflictor, edict_t *killer){
	
	// SOFPPNIX_DEBUG("ent : %08X, inflictor : %08X, killer : %08X",ent,inflictor,killer);	

	const char * name = "die";

	PyObject * died = createEntDict(ent);
	PyObject * inflicting_ent = createEntDict(ent);
	PyObject * killer_player = createEntDict(ent);

	for ( int i = 0; i < player_die_callbacks.size(); i++ ) {

		PyObject* result = PyObject_CallFunction(player_die_callbacks[i],"OOO",died,inflicting_ent,killer_player);
		// returns None
		Py_XDECREF(result);
	}

	Py_XDECREF(died);
	Py_XDECREF(inflicting_ent);
	Py_XDECREF(killer_player);

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
	const char * name = "disconnect";
	PyObject * who = createEntDict(ent);

	for ( int i = 0; i < player_disconnect_callbacks.size(); i++ ) {
		PyObject* result = PyObject_CallFunction(player_disconnect_callbacks[i],"O",who);
		// returns None
		Py_XDECREF(result);
	}
	Py_XDECREF(who);

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

 Clear in dm->checkEvents
 Second Clear in dm->ClientScoreboardMessage
 Extra Drawings Here.
*/
//c4
#define STAT_LAYOUTS 9
void	always_gamerules_c::clientEndFrame(edict_t *ent){
	
	// SOFPPNIX_DEBUG("clientEndFrame\n");

	// SOFPPNIX_DEBUG("Player slot : %i",ent->s.skinnum);
	// correct layering.
	
	// Draw Custom 2D.
	void * client = getClientX(ent->s.skinnum);
	// SOFPPNIX_DEBUG("cclient : %08X",client);
	if ( stget(client,0) == cs_spawned && layoutstring[0] ) {

		// force layouts ON.
		ent->client->ps.stats[STAT_LAYOUTS] |= 1;
	}

	// always show scoreboard during intermission
	// player can toggle scoreboard when its not intermission.
	// if ( !show_score[sendToSlot] && !(*level_intermissiontime) ) return;

	
	// orig_Com_Printf("clientEndFrame\n");
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
void	always_gamerules_c::clientObituary(edict_t *self, edict_t *inflictor, edict_t *attacker){
	currentGameMode->clientObituary(self,inflictor,attacker);
}
//d4
/*
 Code for new clients on connection do once here!

 This function is called from within PutClientInServer
*/
void	always_gamerules_c::clientRespawn(edict_t *ent) {

	const char * name = "respawn";
	PyObject * who = createEntDict(ent);

	for ( int i = 0; i < player_respawn_callbacks.size(); i++ ) {
		PyObject* result = PyObject_CallFunction(player_respawn_callbacks[i],"O",who);
		// returns None
		Py_XDECREF(result);
	}

	Py_XDECREF(who);
	currentGameMode->clientRespawn(ent);

}

/*
Active when player has their scoreboard open.
Called by ClientEndServerFrame, every 32 frames. (3.2 seconds)
if (ent->client->showscores && !(level.framenum & 31))
		dm->clientScoreboardMessage(ent,ent->enemy,false);

Goal : draw even when the scoreboard is not open. Surely I just put code in another function.

The default function clears the screen before writing. This means we cannot draw _before_ it.
Maybe just draw after it?
dmctf_clientScoreboardMessage : NOP Clear.

In SoFree We made the real showscores to always be true, thus making this function always be called.
But I am not doing that here.
checkEvents is instead clearing. Which is earlier than clientEndFrame.

G_SetStats called by ClientEndServerFrame, sets ent->client->ps.stats[STAT_LAYOUTS] |= 1;
Which is required for client to show any scoreboard / layout strings.

Clear in dm->checkEvents ( reliable clear ? )
 Second Clear Here
 Extra Drawings dm->clientEndFrame.
 {ORDER of DRAWING doesn't really matter unless co-ordinates are equal/overlapping.}

 This function still only called if showscores is TRUE.

 ISSUE: The clear in checkEvents wipes away this scoreboard.
 AKA If its not drawn every frame, its not drawn at all, everything has lifetime of 1 frame.

 Test: Only allow this function to call clear? 3.2 second..

 SoF does not use "Clear" to hide contents, it uses ent->client->ps.stats[STAT_LAYOUTS] |= 1;
 In G_setSStats : ent->client->ps.stats[STAT_LAYOUTS] = 0; 
 if ( dm ) ent->client->ps.stats[STAT_LAYOUTS] |= 1; ent->client->ps.stats[STAT_LAYOUTS] |= 2;

G_RunFrame
  ClientBeginServerFrame
  dm->checkEvents();
  ClientEndServerFrames
    ClientEndServerFrame {This function returns early if intermission is displayed. Thats why watermark not visible}
 	  G_SetStats(ent) {if ( dm ) ent->client->ps.stats[STAT_LAYOUTS] |= 1; ent->client->ps.stats[STAT_LAYOUTS] |= 2;}
 	  if (ent->client->showscores && !(level.framenum & 31))
	  	dm->clientScoreboardMessage(ent,ent->enemy,false);
	  dm->clientEndFrame(ent); OVERRIDE G_SETSTATS here.

  Question: How to hide scoreboard whilst keeping LAYOUT = 1. Requires a clear.

  The client has a MEMORY of whats to be displayed. If you send 2 draw command on top without using clear.
  You have multiple layers buildings up. To change content, requires clear. Which will make the scoreboard also have to be redrawn, which makes ping change. TLDR: Draw commands are persistent.

*/
//d8
bool prev_showscores[32] = {0};
void	always_gamerules_c::clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file)
{
	unsigned int level_framenum = stget(base_addr + 0x002B2500,0);
	// SOFPPNIX_DEBUG("framenum is : %i",level_framenum);

	void * gclient = stget(ent, EDICT_GCLIENT);
	int show_scores = stget(gclient, GCLIENT_SHOWSCORES);
	// SOFPPNIX_DEBUG("show_scores is : %i",show_scores);
	int slot = ent->s.skinnum;
	if ( show_scores && ( !prev_showscores[slot] || !(level_framenum & 31) ) ) {
		// every 32 server frames = 3.2 seconds
		// Draw Official Scoreboard ( contains a clear ).
		currentGameMode->clientScoreboardMessage(ent,killer,log_file);
	}
	// turn off.
	if ( !show_scores && prev_showscores[slot] ) {
		// SOFPPNIX_DEBUG("Clearing Screen");
		// Clear Screen.
		orig_SP_Print(ent,0x0700,"*");
	}

	// will only be true
	prev_showscores[slot] = show_scores;
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