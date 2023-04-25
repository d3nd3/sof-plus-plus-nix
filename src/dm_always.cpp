#include "common.h"

extern gamerules_c * currentGameMode;
always_gamerules_c dm_always;

/*

currentGameMode is within our memory

orig is within sof.exe memory

*/


/*
-> orig PROPERTY IS NOT SET HERE !! DO NOT USE IT !!!
currentGameMode->yourFunc() MUST be called here..

calling 
gamerules_c * fallback = currentGameMode->getOrigPointer();
fallback->yourFunc() is NOT allowed here..

because : issue : 2 places where fallback might be called.
	within always(here),
	within currentGameMode

SO: 
	If you want GameModeSpecific AND original behavior, you must make sure to call the orig
	function within your gameModeSpecific class.


If you want fallback/default, just don't implement the function in the game mode class
and it will automatically be enabled because of inheritance virtual.

Its important to note that 'orig' property is only set on the gamemode object

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
*/
void	always_gamerules_c::levelInit(void){

#if 0
	next_available_ID = 0;

	// sofree genetic levelinti code
	char temp[64];
	sprintf(temp,"%s/strip/%s.sp",user->string,"sofree");
	FILE * sofree_strip = fopen(temp,"rb");
	if ( sofree_strip) {
		// already exists
		fclose(sofree_strip);
	} else {
		// create it
		FILE  * depend = fopen(temp,"w+b");
		if ( depend == NULL ) {
			orig_Com_Printf("WARNING: failed to write sofree.sp\n");
			// MessageBox(NULL,"Fix sofree.sp please",NULL,MB_OK);
			// ExitProcess(1);
			orig_Com_Error(ERR_FATAL,"Unable to create sofree string package, contact developer\n");
		} else {
			//write the file
			fprintf(depend,SOFREESP);
			fclose(depend);	
		}	
	}

	orig_Cbuf_AddText("sf_sv_spackage_register sofree.sp\n");

	// sets build number bottom right
	orig_Cbuf_AddText("sf_sv_draw_clear\n");

	// orig_Com_Printf("Before level init\n");
	// dm specific levelinit fallback func
	currentGameMode->levelInit();

	// orig_Com_Printf("Level Init Complete\n");
#endif

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
void	always_gamerules_c::checkEvents(void){
	currentGameMode->checkEvents();
}
void	always_gamerules_c::respawnUntouchedItem(edict_t *ent) {
	currentGameMode->respawnUntouchedItem(ent);
}
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
int 	always_gamerules_c::somethingWeDontUnderstand(void) {
	return currentGameMode->somethingWeDontUnderstand();
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
bool new_client[32];
short client_gravity[32];
void	always_gamerules_c::clientConnect(edict_t *ent){
	#if 0
	int p = get_player_slot_from_ent(ent);
	for ( int j = 0; j < 2; j++ ) {
		// show_score[p] = true;
		reso2d[p][j] = 0;
	}
	new_client[p] = true;
	#endif
	currentGameMode->clientConnect(ent);

}
//a8
void	always_gamerules_c::clientDie(edict_t *ent, edict_t *inflictor, edict_t *killer){
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
//c4
// THis is called by clientBegin and G_RunFrame
void	always_gamerules_c::clientEndFrame(edict_t *ent){
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
void	always_gamerules_c::clientRespawn(edict_t *ent){
	// orig_Com_Printf("Client Respawn!!\n");
	#if 0
	int p = get_player_slot_from_ent(ent);
	ent->client->ps.pmove.gravity = client_gravity[p];
	// enable ui trick force drawing
	ent->client->showscores = 1;

	#endif
	currentGameMode->clientRespawn(ent);

}
//d8
void	always_gamerules_c::clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file){

	#if 0
	// orig_Com_Printf("clientScoreboardMessage\n");
	int sendToSlot = get_player_slot_from_ent(ent);
	// they dont have scoreboard enabled
	
	// orig_Com_Printf(" score : %02X for player %i\n", show_score[sendToSlot], sendToSlot);
	//clear
	orig_SP_Print(ent,0x0700,"*");
	
	if ( layoutstring[0] ) {
		orig_SP_Print(ent,0x0700,layoutstring);
		// orig_Com_Printf("layout string is %s\n",layoutstring);
	}
	if ( !show_score[sendToSlot] && !(*level_intermissiontime) ) return;
	// scoreboard of mod

	#endif
	currentGameMode->clientScoreboardMessage(ent,killer,log_file);
}
//dc
void	always_gamerules_c::clientStealthAndFatigueMeter(edict_t *ent){
	currentGameMode->clientStealthAndFatigueMeter(ent);
}
//e0
void always_gamerules_c::extraFuncOne(void * one, void * two) {
	currentGameMode->extraFuncOne(one,two);
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
void always_gamerules_c::extraFuncTwo(void * one){
	// orig_Com_Printf("extraFuncTwo\n");
	currentGameMode->extraFuncTwo(one);
}
//f8
void always_gamerules_c::extraFuncThree(void * one) {
	// orig_Com_Printf("extraFuncThree\n");
	currentGameMode->extraFuncThree(one);
}
//fc
int always_gamerules_c::extraFuncFour(void) {
	// orig_Com_Printf("extraFour\n");
	return currentGameMode->extraFuncFour();
}
//100
void always_gamerules_c::extraFuncFive(void * one) {
	// orig_Com_Printf("extraFuncFive\n");
	currentGameMode->extraFuncFive(one);
}
//104
void always_gamerules_c::onSpawnTriggerConquer(edict_t * ent){
	currentGameMode->onSpawnTriggerConquer(ent);
}
//108
int always_gamerules_c::extraFuncSeven(void * one) {
	// orig_Com_Printf("extraFuncSeven\n");
	return currentGameMode->extraFuncSeven(one);
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
	// orig_Com_Printf("HELLO APPLY HOOKS ALWAYS?\n");
	#if 0
	orig_Cmd_Score_f = (Cmd_Score_f_type)DetourCreate((LPVOID)0x500F6710,(LPVOID)&my_Cmd_Score_f,DETOUR_TYPE_JMP,6);

	#endif
	currentGameMode->applyHooks();
}