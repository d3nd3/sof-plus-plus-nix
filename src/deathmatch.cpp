/*
	------------------------------------------------------------------
	gamemode_c is an interface classs that forwards to the mode specific routine.
	rules is set to the current game mode.
	eg. rules->SomeFunc.

	Whatever rules is set to, it gets called by gamemode_c inside SoF.

	So we change the rules pointer to control which code is being ran.
	------------------------------------------------------------------

	gamerules_c is the BASE CLASS for all game modes.

	dm_ctf_C is the DERIVED CLASS from gamerules_c for CTF game mode.

	void gamemode_c::setDMMode(int newtype)
	{
		gametype = newtype;
		rules = gamerules[newtype];
		gi.configstring(CS_SHOWNAMES,rules->arePlayerNamesOn()?"1":"0");
		gi.configstring(CS_SHOWTEAMS,rules->arePlayerTeamsOn()?"1":"0");

		rules->registerPlayerIcons();
	}
	Look in dm_always.cpp for more understanding.
	------------------------------------------------------------------
	The functions defined in this file are the base class implementation.
	Meaning any functions that are not defined in your Derived class,
	will eventually make their way here. Where the in-memory version of the function will be called.
	------------------------------------------------------------------
	This file should mostly be left alone, because the code is not guaranteed to run , if the DerivedClass implements the function eg. This file just allows fallback.
	------------------------------------------------------------------
*/

#include "common.h"

//dm_sofree_c
dm_sofree_c dm_sofree;
// dm_none_c dm_none;
dm_standard_c dm_standard;
// dm_assassin_c dm_assassin;
// dm_arsenal_c dm_arsenal;
dm_ctf_c dm_ctf;
dm_realistic_c dm_realistic;
// dm_control_c dm_control;
// dm_ctb_c dm_ctb;

gamerules_c * currentGameMode = NULL;


gamerules_c *game_modes[] = {
	&dm_sofree
};

gamerules_c * orig_modes[] = {
	NULL, // &dm_none
	&dm_standard,
	NULL,// &dm_assassin,
	NULL,//&dm_arsenal,
	&dm_ctf,
	&dm_realistic,
	// &dm_control,
	// &dm_ctb
};


gamerules_c * intercept = NULL;
gamerules_c base_gamerules_c;

gamerules_c * sof_mem_modes = NULL;

void applyDeathmatchHooks(unsigned int base_addr)
{
	orig_setDMMode = createDetour(base_addr + 0x0015EFF0,my_setDMMode,5);

	// This is the gamerules_c base class. (Used vtable reference to point.)
	*(unsigned int*)((void*)(&base_gamerules_c) + 4) = base_addr + 0x00296320;
	// It is set as the orig in some cases. eg. ( sofree game modes ).

	// sof_mem_modes = base_addr + 0x002bd2e8;
	sof_mem_modes = base_addr + 0x0028d040;

}

void freeDeathmatchHooks(void)
{
	free(orig_setDMMode);
}

/*
This is called by player_die too .. but i nopped it temporarily
*/
void my_Cmd_Score_f (edict_t *ent) {
	#if 0
	int player = get_player_slot_from_ent(ent);
	show_score[player] = !show_score[player];
	// orig_Com_Printf("SCORE TOGGLED FOR PLAYER : %i\n",player);
	#endif
}

/*
class gamemode_c:public gamemode_ic
{
private:
	
	int			gametype;
	gamerules_c	*rules;

	gamemode_c is calling all of the shots.
	it will call the rules pointer use as class.
	this is set by us.
*/
// __attribute__((thiscall))
void  my_setDMMode(void * self,int dmmode)
{
	// SOFPPNIX_DEBUG("my_setDMMode %08X\n",self);
	// orig_setDMMode(self,dmmode);
	
	int sofree_dmmode = (int)(_nix_deathmatch->value);
	SOFPPNIX_DEBUG("One\n");
	if ( sofree_dmmode > 0 ) {
		//sofree custom gamemode
		int temp_fix = 1;
		currentGameMode = game_modes[temp_fix-1];

		// GIVES DM_ALWAYS THE POWER TO ROUTE.
		// rules = &dm_always;
		stset(self,8,&dm_always);

		stset(self,4,7 + sofree_dmmode);// gametype

		// use gamerules_c BASE as original.
		currentGameMode->setOrigPointer((void*)(&base_gamerules_c));
		
	} else {
		SOFPPNIX_DEBUG("Two %i\n",dmmode);
		//save rules pointer

		intercept = stget((void*)sof_mem_modes+(4*dmmode),0);

		SOFPPNIX_DEBUG("Intercept : %08X\n",intercept);

		//if dm free for all hm.
		if ( dmmode == 1 || dmmode == 5 || dmmode == 4){
			//rules == my_rules. 8 = gamerules_c * rules
			// Which gamerules_c to call later.
			currentGameMode = orig_modes[dmmode];

			// GIVES DM_ALWAYS THE POWER TO ROUTE.
			// rules = &dm_always;
			stset(self,8,&dm_always);

			// save the original gamerules_c in SoF memory.
			currentGameMode->setOrigPointer((void*)(intercept));
			
		}
		else {
			// rules = gamerules[newtype];
			// no hooks applied.
			stset(self,8,intercept);
		}
		
		stset(self,4,dmmode);
	}
	orig_Com_Printf("pre applyHooks\n");
	dm_always.applyHooks();
	SOFPPNIX_DEBUG("The end2\n");
	// orig_Com_Printf("arePLayerNamesOn PRE\n");
	// orig_PF_Configstring(CS_SHOWNAMES,dm_always.arePlayerNamesOn()?"1":"0");
	// orig_PF_Configstring(CS_SHOWTEAMS,dm_always.arePlayerTeamsOn()?"1":"0");
	SOFPPNIX_DEBUG("The end1\n");
	dm_always.registerPlayerIcons();
	//18
	//rules->checkItemSpawn(NULL,NULL);

	SOFPPNIX_DEBUG("The end\n");
}


/*
orig is a pointer to gamerules_c
*/
void	gamerules_c::clientSetDroppedItemThink(edict_t *ent){
	orig->clientSetDroppedItemThink(ent);
}
void	gamerules_c::clientSetDroppedWeaponThink(edict_t *ent){
	orig->clientSetDroppedWeaponThink(ent);
}
int		gamerules_c::isDM(void){
	return orig->isDM();
}
void	gamerules_c::preLevelInit(void){
	orig->preLevelInit();
}
/*
called by SpawnEntities
sofplus on_map_begin is also a spawnentities hook but console code is pushed AFTER spawnentities is called so later than this.
*/
void	gamerules_c::levelInit(void){

	// dm specific levelinit fallback func
	orig->levelInit();
}
char	*gamerules_c::getGameName(void){
	return orig->getGameName();
}
int		gamerules_c::checkItemSpawn(edict_t *ent,Pickup **pickup){
	return orig->checkItemSpawn(ent,pickup);
}
int		gamerules_c::checkItemAfterSpawn(edict_t *ent,Pickup *pickup){
	return orig->checkItemAfterSpawn(ent,pickup);
}
void	gamerules_c::checkEvents(void){
	orig->checkEvents();
}
void	gamerules_c::respawnUntouchedItem(edict_t *ent) {
	orig->respawnUntouchedItem(ent);
}
void	gamerules_c::setRespawn(edict_t *ent, float delay){
	orig->setRespawn(ent,delay);
}
int		gamerules_c::getMaxEncumbrance(void){
	return orig->getMaxEncumbrance();
}
void 	gamerules_c::AssignTeam(edict_t *ent, char *userinfo){
	orig->AssignTeam(ent,userinfo);
}
void	gamerules_c::ClearFlagCount(void){
	orig->ClearFlagCount();
}
void	gamerules_c::FlagCaptured(team_t team){
	orig->FlagCaptured(team);
}
bool	gamerules_c::CaptureLimitHit(int limit){
	return orig->CaptureLimitHit(limit);
}

void	gamerules_c::openLogFile(void){
	orig->openLogFile();
}
void	gamerules_c::closeLogFile(void){
	orig->closeLogFile();
}
int		gamerules_c::dmRule_NO_HEALTH(void){
	return orig->dmRule_NO_HEALTH();
}
int		gamerules_c::gamerules_c::dmRule_NO_ITEMS(void){
	return orig->dmRule_NO_ITEMS();
}
int		gamerules_c::dmRule_WEAPONS_STAY(void){
	return orig->dmRule_WEAPONS_STAY();
}
int		gamerules_c::dmRule_NO_FALLING(void){
	return orig->dmRule_NO_FALLING();
}
int		gamerules_c::dmRule_REALISTIC_DAMAGE(void){
	return orig->dmRule_REALISTIC_DAMAGE();
}
int		gamerules_c::dmRule_SAME_LEVEL(void){
	return orig->dmRule_SAME_LEVEL();
}
int		gamerules_c::dmRule_NOWEAPRELOAD(void){
	return orig->dmRule_NOWEAPRELOAD();
}
int		gamerules_c::dmRule_TEAMPLAY(void){
	return orig->dmRule_TEAMPLAY();
}
int		gamerules_c::dmRule_NO_FRIENDLY_FIRE(void){
	return orig->dmRule_NO_FRIENDLY_FIRE();
}
int		gamerules_c::dmRule_FORCE_RESPAWN(void){
	return orig->dmRule_FORCE_RESPAWN();
}
int		gamerules_c::dmRule_NO_ARMOR(void){
	return orig->dmRule_NO_ARMOR();
}
int		gamerules_c::dmRule_INFINITE_AMMO(void){
	return orig->dmRule_INFINITE_AMMO();
}
int		gamerules_c::dmRule_SPINNINGPICKUPS(void){
	return orig->dmRule_SPINNINGPICKUPS();
}
int		gamerules_c::dmRule_FORCEJOIN(void){
	return orig->dmRule_FORCEJOIN();
}
int		gamerules_c::dmRule_ARMOR_PROTECT(void){
	return orig->dmRule_ARMOR_PROTECT();
}
int		gamerules_c::dmRule_BULLET_WPNS_ONLY(void){
	return orig->dmRule_BULLET_WPNS_ONLY();
}
//88
int 	gamerules_c::somethingWeDontUnderstand(void) {
	return orig->somethingWeDontUnderstand();
}
//8c
int		gamerules_c::arePlayerNamesOn(void){
	SOFPPNIX_DEBUG("Fallback Base GamesRules_C arePlayerNamesOn %08X\n",orig);
	return orig->arePlayerNamesOn();
}
//90
int		gamerules_c::arePlayerTeamsOn(void){
	return orig->arePlayerTeamsOn();
}
//94
int		gamerules_c::allowAutoWeaponSwitch(void){
	return orig->allowAutoWeaponSwitch();
}
//98
void	gamerules_c::initPlayerDMInfo(edict_t *ent){
	orig->initPlayerDMInfo(ent);
}
//9c
void	gamerules_c::registerPlayerIcons(void){
	SOFPPNIX_DEBUG("Fallback Base GamesRules_C registerPlayerIcons %08X\n",orig);
	orig->registerPlayerIcons();
}
//a0
float	gamerules_c::clientCalculateWaver(edict_t *ent, int atkID){
	return orig->clientCalculateWaver(ent,atkID);
}
//a4
void	gamerules_c::clientConnect(edict_t *ent){
	orig->clientConnect(ent);
}
//a8
void	gamerules_c::clientDie(edict_t *ent, edict_t *inflictor, edict_t *killer){
	orig->clientDie(ent,inflictor,killer);
}
//ac
void	gamerules_c::clientDieWeaponHandler(edict_t *ent){
	orig->clientDieWeaponHandler(ent);
}
//b0
void	gamerules_c::clientPreDisconnect(edict_t *ent){
	orig->clientPreDisconnect(ent);
}
//b4
void	gamerules_c::clientDisconnect(edict_t *ent){
	orig->clientDisconnect(ent);
}
//b8
void	gamerules_c::clientDropItem(edict_t *ent,int type,int ammoCount){
	orig->clientDropItem(ent,type,ammoCount);
}
//bc
void	gamerules_c::clientDropWeapon(edict_t *ent,int type, int clipSize){
	orig->clientDropWeapon(ent,type,clipSize);
}
//c0
float	gamerules_c::gamerules_c::clientGetMovescale(edict_t *ent){
	return orig->clientGetMovescale(ent);
}
//c4
void	gamerules_c::clientEndFrame(edict_t *ent){
	orig->clientEndFrame(ent);
}
//c8
void	gamerules_c::clientHandlePredn(edict_t *ent){
	orig->clientHandlePredn(ent);
}
//cc
void	gamerules_c::clientHelpMessage(edict_t *ent){
	orig->clientHelpMessage(ent);
}
//d0
void	gamerules_c::clientObituary(edict_t *self, edict_t *inflictor, edict_t *attacker){
	orig->clientObituary(self,inflictor,attacker);
}
//d4
void	gamerules_c::clientRespawn(edict_t *ent){
	orig->clientRespawn(ent);

}
//d8
void	gamerules_c::clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file){
	// orig scoreboard pls
	orig->clientScoreboardMessage(ent,killer,log_file);
}
//dc
void	gamerules_c::clientStealthAndFatigueMeter(edict_t *ent){
	orig->clientStealthAndFatigueMeter(ent);
}
//e0
void gamerules_c::extraFuncOne(void * one, void * two) {
	orig->extraFuncOne(one,two);
}
//e4
bool	gamerules_c::IsUnprotectedGoreZone(gz_code zone){
	return orig->IsUnprotectedGoreZone(zone);
}
//e8
bool	gamerules_c::IsLimbZone(gz_code zone){
	return orig->IsLimbZone(zone);
}
//ec
void	gamerules_c::HitByAttacker(edict_t *body, edict_t *attacker){
	// orig_Com_Printf("HitByAttacker	\n");
	orig->HitByAttacker(body,attacker);
}
//f0
bool	gamerules_c::IsShootableWeapon(char *weaponname){
	return orig->IsShootableWeapon(weaponname);
}
//f4
void gamerules_c::extraFuncTwo(void * one){
	// orig_Com_Printf("extraFuncTwo\n");
	orig->extraFuncTwo(one);
}
//f8
void gamerules_c::extraFuncThree(void * one) {
	// orig_Com_Printf("extraFuncThree\n");
	orig->extraFuncThree(one);
}
//fc
int gamerules_c::extraFuncFour(void) {
	// orig_Com_Printf("extraFour\n");
	return orig->extraFuncFour();
}
//100
void gamerules_c::extraFuncFive(void * one) {
	// orig_Com_Printf("extraFuncFive\n");
	orig->extraFuncFive(one);
}
//104
void gamerules_c::onSpawnTriggerConquer(edict_t * ent){
	orig->onSpawnTriggerConquer(ent);
}
//108
int gamerules_c::extraFuncSeven(void * one) {
	// orig_Com_Printf("extraFuncSeven\n");
	return orig->extraFuncSeven(one);
}


void gamerules_c::removeHooks(void) {
}


void gamerules_c::applyHooks(void) {
	// orig_Com_Printf("ApplyHooks inside fallback gameMode\n");
}

