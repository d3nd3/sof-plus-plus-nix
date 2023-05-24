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

char layoutstring[1024];
int layoutstring_len = 0;

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

/*
	8 bytes of padding at start of vtable.
	vtable pointer lives at classObj+4
*/
void fixupClassesForLinux(void)
{
	// rules-> entry point. requires gcc linux compatibility.
	*(unsigned int*)(&dm_always) = *(unsigned int*)(&dm_always) - 8;
}

gamerules_c * intercept = NULL;
gamerules_c base_gamerules_c;

gamerules_c * sof_mem_modes = NULL;

void applyDeathmatchHooks(unsigned int base_addr)
{
	orig_setDMMode = createDetour(base_addr + 0x0015EFF0,my_setDMMode,5);

	// This is the gamerules_c base class. (Used vtable reference to point.)
	*(unsigned int*)((void*)(&base_gamerules_c)) = base_addr + 0x00296320;
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
	int sofree_dmmode = (int)(_nix_deathmatch->value);

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
		//save rules pointer
		intercept = stget(sof_mem_modes,4*dmmode);

		//if dm free for all hm.
		if ( dmmode == 1 || dmmode == 5 || dmmode == 4){
			//rules == my_rules. 8 = gamerules_c * rules
			// Which gamerules_c to call later.
			currentGameMode = orig_modes[dmmode];

			// GIVES DM_ALWAYS THE POWER TO ROUTE.
			// rules = &dm_always;
			// Since the sof library was compiled with a gcc which uses the +4 as vtable
			// We have to give it -4 as pointer.
			// The vtable pointer also needs to start 8 bytes back. :/
			stset(self,8,(void*)(&dm_always)-4);

			// save the original gamerules_c in SoF memory.
			// VTable at +4 ? And has 8 bytes padding in front.
			// I dont' want the vtable.
			currentGameMode->setOrigPointer(intercept);
			
		}
		else {
			// rules = gamerules[newtype];
			// no hooks applied.
			stset(self,8,intercept);
		}
		
		stset(self,4,dmmode);
	}

	// void (*applyHooks)(void * self) = stget(stget(&dm_always,0)+8+0x110,0);
	// applyHooks(&dm_always);
	dm_always.applyHooks();
	
	// int (*arePlayerNamesOn)(void * self) = stget(stget(&dm_always,0)+8+0x8c,0);
	// int (*arePlayerTeamsOn)(void * self) = stget(stget(&dm_always,0)+8+0x90,0);
	// int isOn = arePlayerNamesOn(&dm_always);
	// orig_PF_Configstring(CS_SHOWNAMES,isOn?"1":"0");
	// isOn = arePlayerTeamsOn(&dm_always);
	// orig_PF_Configstring(CS_SHOWTEAMS,isOn?"1":"0");
	orig_PF_Configstring(CS_SHOWNAMES,dm_always.arePlayerNamesOn() ?"1":"0");
	orig_PF_Configstring(CS_SHOWTEAMS,dm_always.arePlayerTeamsOn() ?"1":"0");

	// void (*registerPlayerIcons)(void * self) = stget(stget(&dm_always,0)+8+0x9c,0);
	// registerPlayerIcons(&dm_always);
	dm_always.registerPlayerIcons();

}


/*
orig is a pointer to gamerules_c

linux has some strange 4 byte offset to the vtable.
*/
void	gamerules_c::clientSetDroppedItemThink(edict_t *ent){
	void (*o)(void * self,edict_t *ent) = stget(stget(orig,4)+8+0x00,0);

	o(orig,ent);

}
void	gamerules_c::clientSetDroppedWeaponThink(edict_t *ent){
	void (*o)(void * self,edict_t *ent) = stget(stget(orig,4)+8+0x04,0);

	o(orig,ent);
}
// the linux library gamex86 is also having 8 bytes at front of vtable. :'('
int		gamerules_c::isDM(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x08,0);
	return o(orig);
}
void	gamerules_c::preLevelInit(void){
	void (*o)(void * self) = stget(stget(orig,4)+8+0x0c,0);

	o(orig);
}
/*
called by SpawnEntities
sofplus on_map_begin is also a spawnentities hook but console code is pushed AFTER spawnentities is called so later than this.
*/
void	gamerules_c::levelInit(void){
	void (*o)(void * self) = stget(stget(orig,4)+8+0x10,0);

	// dm specific levelinit fallback func
	o(orig);

}
char	*gamerules_c::getGameName(void){
	char *(*o)(void * self) = stget(stget(orig,4)+8+0x14,0);
	char * gn = o(orig);
	SOFPPNIX_DEBUG("Gamename is : %s",gn);
	return gn;
}
int		gamerules_c::checkItemSpawn(edict_t *ent,Pickup **pickup){
	int (*o)(void * self,edict_t *ent,Pickup **pickup) = stget(stget(orig,4)+8+0x18,0);

	return o(orig,ent,pickup);

}
int		gamerules_c::checkItemAfterSpawn(edict_t *ent,Pickup *pickup){
	int (*o)(void * self,edict_t *ent,Pickup *pickup) = stget(stget(orig,4)+8+0x1c,0);

	return o(orig,ent,pickup);

}
void	gamerules_c::checkEvents(void){
	void (*o)(void * self) = stget(stget(orig,4)+8+0x20,0);

	o(orig);

}
void	gamerules_c::respawnUntouchedItem(edict_t *ent) {
	void (*o)(void * self,edict_t *ent) = stget(stget(orig,4)+8+0x24,0);

	o(orig,ent);

}
void	gamerules_c::setRespawn(edict_t *ent, float delay){
	void (*o)(void * self,edict_t *ent, float delay) = stget(stget(orig,4)+8+0x28,0);

	o(orig,ent,delay);
}
int		gamerules_c::getMaxEncumbrance(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x2c,0);

	return o(orig);
}
void 	gamerules_c::AssignTeam(edict_t *ent, char *userinfo){
	void (*o)(void * self,edict_t *ent, char *userinfo) = stget(stget(orig,4)+8+0x30,0);

	o(orig,ent,userinfo);
}
void	gamerules_c::ClearFlagCount(void){
	void (*o)(void * self) = stget(stget(orig,4)+8+0x34,0);

	o(orig);
}
void	gamerules_c::FlagCaptured(team_t team){
	void (*o)(void * self,team_t team) = stget(stget(orig,4)+8+0x38,0);
	o(orig,team);
}
bool	gamerules_c::CaptureLimitHit(int limit){
	bool (*o)(void * self,int limit) = stget(stget(orig,4)+8+0x3C,0);

	return o(orig,limit);
}

void	gamerules_c::openLogFile(void){
	void (*o)(void * self) = stget(stget(orig,4)+8+0x40,0);
	o(orig);

}
void	gamerules_c::closeLogFile(void){
	void (*o)(void * self) = stget(stget(orig,4)+8+0x44,0);

	o(orig);
}
int		gamerules_c::dmRule_NO_HEALTH(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x48,0);

	return o(orig);
}
int		gamerules_c::gamerules_c::dmRule_NO_ITEMS(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x4c,0);
	return o(orig);
}
int		gamerules_c::dmRule_WEAPONS_STAY(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x50,0);

	return o(orig);

}
int		gamerules_c::dmRule_NO_FALLING(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x54,0);

	return o(orig);
}
int		gamerules_c::dmRule_REALISTIC_DAMAGE(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x58,0);
	return o(orig);
}
int		gamerules_c::dmRule_SAME_LEVEL(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x5C,0);
	return o(orig);
}
int		gamerules_c::dmRule_NOWEAPRELOAD(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x60,0);

	return o(orig);
}
int		gamerules_c::dmRule_TEAMPLAY(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x64,0);
	return o(orig);
}
int		gamerules_c::dmRule_NO_FRIENDLY_FIRE(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x68,0);

	return o(orig);
}
int		gamerules_c::dmRule_FORCE_RESPAWN(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x6c,0);
	return o(orig);
}
int		gamerules_c::dmRule_NO_ARMOR(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x70,0);

	return o(orig);

}
int		gamerules_c::dmRule_INFINITE_AMMO(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x74,0);
	return o(orig);
}
int		gamerules_c::dmRule_SPINNINGPICKUPS(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x78,0);

	return o(orig);
}
int		gamerules_c::dmRule_FORCEJOIN(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x7c,0);

	return o(orig);
}
int		gamerules_c::dmRule_ARMOR_PROTECT(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x80,0);
	return o(orig);
}
int		gamerules_c::dmRule_BULLET_WPNS_ONLY(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x84,0);

	return o(orig);
}
//88
int 	gamerules_c::dmRule_TEAM_REDBLUE(void) {
	int (*o)(void * self) = stget(stget(orig,4)+8+0x88,0);

	return o(orig);
}
//8c
int		gamerules_c::arePlayerNamesOn(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x8C,0);

	return o(orig);
}
//90
int		gamerules_c::arePlayerTeamsOn(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x90,0);
	return o(orig);

}
//94
int		gamerules_c::allowAutoWeaponSwitch(void){
	int (*o)(void * self) = stget(stget(orig,4)+8+0x94,0);

	return o(orig);
}
//98
void	gamerules_c::initPlayerDMInfo(edict_t *ent){
	void (*o)(void * self, edict_t *ent) = stget(stget(orig,4)+8+0x98,0);
	
	o(orig,ent);
}
//9c
void gamerules_c::registerPlayerIcons(void){
	void (*o)(void * self) = stget(stget(orig,4)+8+0x9c,0);

	o(orig);
}
//a0
float	gamerules_c::clientCalculateWaver(edict_t *ent, int atkID){
	float (*o)(void * self, edict_t *ent, int atkID) = stget(stget(orig,4)+8+0xa0,0);
	
	return o(orig,ent,atkID);
}
//a4
void	gamerules_c::clientConnect(edict_t *ent){
	void (*o)(void * self, edict_t *ent) = stget(stget(orig,4)+8+0xa4,0);
	o(orig,ent);

}
//a8
void	gamerules_c::clientDie(edict_t *ent, edict_t *inflictor, edict_t *killer){
	void (*o)(void * self, edict_t *ent, edict_t *inflictor, edict_t *killer) = stget(stget(orig,4)+8+0xa8,0);

	o(orig,ent,inflictor,killer);

}
//ac
void	gamerules_c::clientDieWeaponHandler(edict_t *ent){
	void (*o)(void * self, edict_t *ent) = stget(stget(orig,4)+8+0xac,0);
	
	o(orig,ent);

}
//b0
void	gamerules_c::clientPreDisconnect(edict_t *ent){
	void (*o)(void * self, edict_t *ent) = stget(stget(orig,4)+8+0xb0,0);
	o(orig,ent);

}
//b4
void	gamerules_c::clientDisconnect(edict_t *ent){
	void (*o)(void * self, edict_t *ent) = stget(stget(orig,4)+8+0xb4,0);

	o(orig,ent);

}
//b8
void	gamerules_c::clientDropItem(edict_t *ent,int type,int ammoCount){
	void (*o)(void * self, edict_t *ent,int type,int ammoCount) = stget(stget(orig,4)+8+0xb8,0);
	o(orig,ent,type,ammoCount);


}
//bc
void	gamerules_c::clientDropWeapon(edict_t *ent,int type, int clipSize){
	void (*o)(void * self, edict_t *ent,int type, int clipSize) = stget(stget(orig,4)+8+0xbc,0);

	o(orig,ent,type,clipSize);

}
//c0
float	gamerules_c::gamerules_c::clientGetMovescale(edict_t *ent){
	float (*o)(void * self, edict_t *ent) = stget(stget(orig,4)+8+0xc0,0);
	
	return o(orig,ent);

}
//c4
void	gamerules_c::clientEndFrame(edict_t *ent){
	void (*o)(void * self, edict_t *ent) = stget(stget(orig,4)+8+0xc4,0);
	
	o(orig,ent);


}
//c8
void	gamerules_c::clientHandlePredn(edict_t *ent){
	void (*o)(void * self, edict_t *ent) = stget(stget(orig,4)+8+0xc8,0);
	o(orig,ent);


}
//cc
void	gamerules_c::clientHelpMessage(edict_t *ent){
	void (*o)(void * self, edict_t *ent) = stget(stget(orig,4)+8+0xcc,0);

	o(orig,ent);


}
//d0
void	gamerules_c::clientObituary(edict_t *self, edict_t *inflictor, edict_t *attacker){
	void (*o)(void * this_self, edict_t *self, edict_t *inflictor, edict_t *attacker) = stget(stget(orig,4)+8+0xd0,0);

	o(orig,self,inflictor,attacker);
}
//d4
void	gamerules_c::clientRespawn(edict_t *ent){
	void (*o)(void * self, edict_t *ent) = stget(stget(orig,4)+8+0xd4,0);
	o(orig,ent);

}
//d8
void	gamerules_c::clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file){
	void (*o)(void * self, edict_t *ent, edict_t *killer, qboolean log_file) = stget(stget(orig,4)+8+0xd8,0);
	// orig scoreboard pls
	o(orig,ent,killer,log_file);

}
//dc
void	gamerules_c::clientStealthAndFatigueMeter(edict_t *ent){
	void (*o)(void * self, edict_t *ent) = stget(stget(orig,4)+8+0xdc,0);
	o(orig,ent);
}
//e0
void gamerules_c::clientTouchConquerTrigger(void * one, void * two) {
	void (*o)(void * self, void * one, void * two) = stget(stget(orig,4)+8+0xe0,0);
	o(orig,one,two);
}
//e4
bool	gamerules_c::IsUnprotectedGoreZone(gz_code zone){
	bool (*o)(void * self, gz_code zone) = stget(stget(orig,4)+8+0xe4,0);
	return o(orig,zone);

}
//e8
bool	gamerules_c::IsLimbZone(gz_code zone){
	bool (*o)(void * self, gz_code zone) = stget(stget(orig,4)+8+0xe8,0);
	return o(orig,zone);

}
//ec
void	gamerules_c::HitByAttacker(edict_t *body, edict_t *attacker){
	void (*o)(void * self, edict_t *body, edict_t *attacker) = stget(stget(orig,4)+8+0xec,0);
	// orig_Com_Printf("HitByAttacker	\n");
	o(orig,body,attacker);

}
//f0
bool	gamerules_c::IsShootableWeapon(char *weaponname){
	bool (*o)(void * self, char *weaponname) = stget(stget(orig,4)+8+0xf0,0);

	return o(orig,weaponname);


}
//f4
void gamerules_c::unsetControlFlag(void * one){
	void (*o)(void * self, void * one) = stget(stget(orig,4)+8+0xf4,0);

	// orig_Com_Printf("extraFuncTwo\n");
	o(orig,one);

}
//f8
void gamerules_c::setControlFlag(void * one) {
	void (*o)(void * self, void * one) = stget(stget(orig,4)+8+0xf8,0);


	// orig_Com_Printf("extraFuncThree\n");
	o(orig,one);

}
//fc
int gamerules_c::tooManyFlags(void) {
	int (*o)(void * self) = stget(stget(orig,4)+8+0xfc,0);

	// orig_Com_Printf("extraFour\n");
	return o(orig);

}
//100
void gamerules_c::addControlFlag(void * one) {
	void (*o)(void * self, void * one) = stget(stget(orig,4)+8+0x100,0);

	// orig_Com_Printf("extraFuncFive\n");
	o(orig,one);

}
//104
void gamerules_c::registerConquerTrigger(edict_t * ent){
	void (*o)(void * self, edict_t * ent) = stget(stget(orig,4)+8+0x104,0);

	o(orig,ent);

}
//108
int gamerules_c::isPointInConquerTrigger(void * one) {
	int (*o)(void * self, void * one) = stget(stget(orig,4)+8+0x108,0);


	// orig_Com_Printf("extraFuncSeven\n");
	return o(orig,one);
}


void gamerules_c::removeHooks(void) {
}


void gamerules_c::applyHooks(void) {
	// orig_Com_Printf("ApplyHooks inside fallback gameMode\n");
}

