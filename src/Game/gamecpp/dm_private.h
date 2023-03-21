// *****************************************************************************
// dm_private.h
// ------------
// Deathmatch stuff that's for the private use of dm.cpp and dm_????.cpp.
// *****************************************************************************



// Player icon enumerations.

enum
{	
	TC_BLACK,
	TC_BRIGHT_GREEN,
	TC_BRIGHT_YELLOW,
	TC_BRIGHT_RED,	
};

const float	REALISTIC_MOVESCALE	 = 0.5625;

extern int countPlayers(void);
extern qboolean IsFemale(edict_t *ent);
extern void setPlayernameColor(edict_t &ent,edict_t &other,byte color);

// 12/28/99 kef -- declared in CWeaponInfo.h, class Pickup replaces itemSpawnInfo_t. and there was much rejoicing.
class Pickup;

/*
============
player_dmInfo_c
============
*/

class player_dmInfo_c:public player_dmInfo_ic
{
public:
};

/*
============
player_dmassassinInfo_c
============
*/

class player_dmassassinInfo_c:public player_dmInfo_c
{
private:
	edict_t	const *assassinTarget;
	edict_t	const *deferedAssassinTarget;

public:
	
	player_dmassassinInfo_c(){assassinTarget=deferedAssassinTarget=NULL;}

	edict_t const *getTarget(void){return((assassinTarget)?(assassinTarget):NULL);}
	void	setTarget(edict_t const *Targ){assassinTarget = deferedAssassinTarget = Targ;}
	edict_t const *getDeferedTarget(void){return((deferedAssassinTarget)?(deferedAssassinTarget):NULL);}
	void	setDeferedTarget(edict_t const *deferedTarg){deferedAssassinTarget = deferedTarg;}
};

/*
============
player_dmarsenalInfo_c
============
*/

class player_dmarsenalInfo_c:public player_dmInfo_c
{
private:

	char	inventory[1024];

public:

	player_dmarsenalInfo_c(){memset(inventory,0,sizeof(inventory));}
	
	char *invString(void){return(inventory);}
};

/*
============
player_dmrealInfo_c
============
*/

class player_dmrealInfo_c:public player_dmInfo_c
{
private:

protected:

	int			fatigue;
	qboolean	i_am_fatigued;

public:

	player_dmrealInfo_c(){fatigue=0;i_am_fatigued=false;}

	int getFatigueVal(void){return (fatigue);}
	void setFatigueVal(int newFatigue){fatigue=newFatigue;}
	qboolean getFatigued(void){return (i_am_fatigued);}
	void setFatigued(qboolean fatigued){i_am_fatigued=fatigued;}
};

/*
============
gamerules_c
============
*/

class gamerules_c
{
protected:

	FILE	*filePtr;

	virtual void	clientSetDroppedItemThink(edict_t *ent);	
	virtual void	clientSetDroppedWeaponThink(edict_t *ent);

public:
	
	virtual int		isDM(void){return(1);}
	virtual void	preLevelInit(void){}
	virtual void	levelInit(void){}
	virtual char	*getGameName(void){return("");}
	virtual int		checkItemSpawn(edict_t *ent,Pickup **pickup){return(0);}
	virtual int		checkItemAfterSpawn(edict_t *ent,Pickup *pickup){return(0);}
	virtual void	checkEvents(void){};
	virtual	void	respawnUntouchedItem(edict_t *ent) {}
	virtual void	setRespawn(edict_t *ent, float delay);
	virtual int		getMaxEncumbrance(void){return 0;}
	virtual void 	AssignTeam(edict_t *ent, char *userinfo){};
	virtual void	ClearFlagCount(void){};
	virtual void	FlagCaptured(team_t team){};
	virtual bool	CaptureLimitHit(int limit){return false;};

	virtual	void	openLogFile(void);
	virtual	void	closeLogFile(void);

	virtual int		dmRule_NO_HEALTH(void){return(0);}
	virtual int		dmRule_NO_ITEMS(void){return(0);}
	virtual int		dmRule_WEAPONS_STAY(void){return(0);}
	virtual int		dmRule_NO_FALLING(void){return(0);}
	virtual int		dmRule_REALISTIC_DAMAGE(void){return(0);}
	virtual int		dmRule_SAME_LEVEL(void){return(0);}
	virtual int		dmRule_NOWEAPRELOAD(void){return(0);}
	virtual int		dmRule_TEAMPLAY(void){return(0);};
	virtual int		dmRule_NO_FRIENDLY_FIRE(void){return(0);}
	virtual int		dmRule_FORCE_RESPAWN(void){return(0);}
	virtual int		dmRule_NO_ARMOR(void){return(0);}
	virtual int		dmRule_INFINITE_AMMO(void){return(0);}
	virtual int		dmRule_SPINNINGPICKUPS(void){return(0);}
	virtual int		dmRule_FORCEJOIN(void){return(0);}
	virtual int		dmRule_ARMOR_PROTECT(void){return(0);}
	virtual int		dmRule_BULLET_WPNS_ONLY(void){return(0);}
	
	virtual int		arePlayerNamesOn(void){return(0);}
	virtual int		arePlayerTeamsOn(void){return(0);}
	virtual int		allowAutoWeaponSwitch(void){return(1);}

	virtual	void	initPlayerDMInfo(edict_t &ent){};
	virtual void	registerPlayerIcons(void){};

	virtual	float	clientCalculateWaver(edict_t &ent, int atkID);
	virtual void	clientConnect(edict_t &ent);
	virtual void	clientDie(edict_t &ent, edict_t &inflictor, edict_t &killer){};
	virtual void	clientDieWeaponHandler(edict_t *ent);
	virtual void	clientPreDisconnect(edict_t &ent){};
	virtual void	clientDisconnect(edict_t &ent){closeLogFile();};
	virtual void	clientDropItem(edict_t *ent,int type,int ammoCount);
	virtual	void	clientDropWeapon(edict_t *ent,int type, int clipSize);
	virtual float	clientGetMovescale(edict_t *ent);
	virtual void	clientEndFrame(edict_t *ent){};
	virtual	void	clientHandlePredn(edict_t &ent);
	virtual void	clientHelpMessage(edict_t *ent){};
	virtual void	clientObituary(edict_t *self, edict_t *inflictor, edict_t *attacker);
	virtual void	clientRespawn(edict_t &ent){};
	virtual void	clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file);
	virtual void	clientStealthAndFatigueMeter(edict_t *ent){};

	virtual bool	IsUnprotectedGoreZone(gz_code zone);
	virtual bool	IsLimbZone(gz_code zone);

	virtual void	HitByAttacker(edict_t *body, edict_t *attacker){};
	virtual bool	IsShootableWeapon(char *weaponname);

};

/*
============
dmnone_c
============
*/

class dmnone_c:public gamerules_c
{
public:

	int		isDM(void){return 0;}
	void	levelInit(void){gi.SP_Register("singleplr");}
	char	*getGameName(void){return("single player");}
	int		checkItemSpawn(edict_t *ent,Pickup **pickup){return(-1);}
	int		getMaxEncumbrance(void);

	void	openLogFile(void){};
	void	closeLogFile(void){};

	void	clientConnect(edict_t &ent){};
	void	clientDropItem(edict_t *ent,int type,int ammoCount);
	void	clientDropWeapon(edict_t *ent,int type, int clipSize);
	float	clientGetMovescale(edict_t *ent);
	void	clientObituary(edict_t *self, edict_t *inflictor, edict_t *attacker){};
	void	clientRespawn(edict_t &ent);
	void	clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file){};
	void	clientStealthAndFatigueMeter(edict_t *ent);
	float	clientCalculateWaver(edict_t &ent, int atkID);
};

/*
============
dmstandard_c
============
*/

class dmstandard_c:public gamerules_c
{
public:
	
	void	levelInit(void);
	char	*getGameName(void);
	int		checkItemSpawn(edict_t *ent,Pickup **pickup);

	int		dmRule_NO_HEALTH(void);
	int		dmRule_NO_ITEMS(void);
	int		dmRule_WEAPONS_STAY(void);
	int		dmRule_NO_FALLING(void);
	int		dmRule_REALISTIC_DAMAGE(void);
	int		dmRule_SAME_LEVEL(void);
	int		dmRule_NOWEAPRELOAD(void);
	int		dmRule_TEAMPLAY(void);
	int		dmRule_NO_FRIENDLY_FIRE(void);
	int		dmRule_FORCE_RESPAWN(void);
	int		dmRule_NO_ARMOR(void);
	int		dmRule_INFINITE_AMMO(void);
	int		dmRule_SPINNINGPICKUPS(void);
	int		dmRule_BULLET_WPNS_ONLY(void);

	int		arePlayerTeamsOn(void){return(dmRule_TEAMPLAY()?-1:0);}

	void	clientDie(edict_t &ent, edict_t &inflictor, edict_t &killer);
	void	clientRespawn(edict_t &ent);
	float	clientCalculateWaver(edict_t &ent, int atkID);
};

/*
============
dmassassin_c
============
*/

class dmassassin_c:public gamerules_c
{
	const	edict_t *getRandomPlayer(edict_t &ent);
	void	clientConnectAux(edict_t &ent);
	void	clientDisconnectAux(edict_t &ent);

public:

	void	levelInit(void);
	char	*getGameName(void){return("assassin DM");}
	int		checkItemSpawn(edict_t *ent,Pickup **pickup);

	int		dmRule_NO_HEALTH(void);
	int		dmRule_NO_ITEMS(void);
	int		dmRule_WEAPONS_STAY(void);
	int		dmRule_NO_FALLING(void);
	int		dmRule_REALISTIC_DAMAGE(void);
	int		dmRule_SAME_LEVEL(void);
	int		dmRule_NOWEAPRELOAD(void);
	int		dmRule_FORCE_RESPAWN(void);
	int		dmRule_NO_ARMOR(void);
	int		dmRule_INFINITE_AMMO(void);
	int		dmRule_SPINNINGPICKUPS(void);
	int		dmRule_BULLET_WPNS_ONLY(void);

	int		arePlayerNamesOn(void){return 1;}

	void	initPlayerDMInfo(edict_t &ent);
	void	registerPlayerIcons(void);

	void	clientConnect(edict_t &ent);
	void	clientDie(edict_t &ent, edict_t &inflictor, edict_t &killer);
	void	clientDisconnect(edict_t &ent);
	void	clientEndFrame(edict_t *ent);
	void	clientHelpMessage(edict_t *ent);
	void	clientRespawn(edict_t &ent);
	void	clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file);
};

/*
============
dmarsenal_c
============
*/

class dmarsenal_c:public gamerules_c
{
	#define ARSENAL_MAXWEAPONS 6

	int		weaponForSlot[ARSENAL_MAXWEAPONS];
	int		firstToOneHit;
	int		resetGame;

	void	setWeapons(void);
	void	giveClientWeapons(edict_t &ent,qboolean newInv);

public:

	void	levelInit(void);
	char	*getGameName(void){return("arsenal DM");}
	int		checkItemSpawn(edict_t *ent,Pickup **pickup);
	void	checkEvents(void);

	int		dmRule_NO_HEALTH(void);
	int		dmRule_NO_ITEMS(void);
	int		dmRule_NO_FALLING(void);
	int		dmRule_REALISTIC_DAMAGE(void);
	int		dmRule_SAME_LEVEL(void);
	int		dmRule_NOWEAPRELOAD(void) { return(1); }
	int		dmRule_FORCE_RESPAWN(void);
	int		dmRule_NO_ARMOR(void);
	int		dmRule_INFINITE_AMMO(void) { return(1); }

	void	initPlayerDMInfo(edict_t &ent);

	void	clientConnect(edict_t &ent);
	void	clientDie(edict_t &ent, edict_t &inflictor, edict_t &killer);
	void	clientDieWeaponHandler(edict_t *ent);
	void	clientDropItem(edict_t *ent,int type,int ammoCount){};
	void	clientDropWeapon(edict_t *ent,int type, int clipSize){};
	void	clientHelpMessage(edict_t *ent);
	void	clientRespawn(edict_t &ent);
	void	clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file);
};

/*
============
dmreal_c
============
*/

class dmreal_c:public gamerules_c
{
	const edict_t *getRandomPlayer(edict_t &ent);

private:
	float					m_maxUntouchedTime;
	float					m_itemRetryTime;
	multimap<int, Vect3>	m_spawnPoints;

protected:
	void	clientSetDroppedWeaponThink(edict_t *ent);

public:
	virtual	~dmreal_c();

	void	preLevelInit(void);
	void	levelInit(void);
	char	*getGameName(void){return("real DM");}
	int		checkItemSpawn(edict_t *ent,Pickup **pickup);
	int		checkItemAfterSpawn(edict_t *ent,Pickup *pickup);
	void	respawnUntouchedItem(edict_t *ent);
	void	setRespawn(edict_t *ent, float delay);
	int		getMaxEncumbrance(void){return dmRule_REALISTIC_DAMAGE()?4:3;}

	int		dmRule_REALISTIC_DAMAGE(void){return 1;}
	int		dmRule_SAME_LEVEL(void);
	int		dmRule_TEAMPLAY(void);
	int		dmRule_FORCE_RESPAWN(void);
	int		dmRule_SPINNINGPICKUPS(void);
	int		dmRule_BULLET_WPNS_ONLY(void);

	int		allowAutoWeaponSwitch(void){return(0);}

	void	initPlayerDMInfo(edict_t &ent);

	float	clientCalculateWaver(edict_t &ent, int atkID);
	void	clientDie(edict_t &ent, edict_t &inflictor, edict_t &killer);
	float	clientGetMovescale(edict_t *ent);
	void	clientRespawn(edict_t &ent);
	void	clientStealthAndFatigueMeter(edict_t *ent);
	void	clientPreDisconnect(edict_t &ent);
	void	clientDisconnect(edict_t &ent);

	int		arePlayerTeamsOn(void){return(dmRule_TEAMPLAY()?-1:0);}


};

/*
============
dmctf_c
============
*/

class dmctf_c:public gamerules_c
{
private:
	edict_t	*FindFlagEdict(int team);
	int		num_blue_skins;
	int		num_red_skins;
	team_t	CompareSkin(char *skinname, char* teamname);
	int		NumOfSkins(char *teamname);
	void	FindSkin(char *teamname, int skinnum);
	char	NewSkin[100];
	int		blue_collected;
	int		red_collected;

public:

	char	*getGameName(void){return("CTF DM");}
	int		checkItemSpawn(edict_t *ent,Pickup **pickup);
	void	levelInit(void);
	void	AssignTeam(edict_t *ent, char *userinfo);
	void	FlagCaptured(team_t team);
	void	ClearFlagCount(void);
	bool	CaptureLimitHit(int limit);


	int		dmRule_NO_HEALTH(void);
	int		dmRule_NO_ITEMS(void);
	int		dmRule_WEAPONS_STAY(void);
	int		dmRule_NO_FALLING(void);
	int		dmRule_REALISTIC_DAMAGE(void);
	int		dmRule_SAME_LEVEL(void);
	int		dmRule_NOWEAPRELOAD(void);
	int		dmRule_TEAMPLAY(void){return (1);};
	int		dmRule_NO_FRIENDLY_FIRE(void);
	int		dmRule_FORCE_RESPAWN(void);
	int		dmRule_NO_ARMOR(void);
	int		dmRule_INFINITE_AMMO(void);
	int		dmRule_SPINNINGPICKUPS(void);
	int		dmRule_BULLET_WPNS_ONLY(void);
	int		dmRule_FORCEJOIN(void);

	int		arePlayerTeamsOn(void){return(1);}

	void	clientDie(edict_t &ent, edict_t &inflictor, edict_t &killer);
	void	clientRespawn(edict_t &ent);	
	void	clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file);
	void	clientDropItem(edict_t *ent,int type,int ammoCount);
	void	clientDisconnect(edict_t &ent);
	void	HitByAttacker(edict_t *body, edict_t *attacker);
};

/*
============
gamemode_c
============
*/

class gamemode_c:public gamemode_ic
{
private:
	
	int			gametype;
	gamerules_c	*rules;

public:

	void	setDMMode(int newtype);
	int		isDM(void){return rules->isDM();}
	void	preLevelInit(void){rules->preLevelInit();}
	void	levelInit(void){rules->levelInit();}
	char	*getGameName(void){return(rules->getGameName());}
	int		checkItemSpawn(edict_t *ent,Pickup **pickup){return(rules->checkItemSpawn(ent,pickup));}
	int		checkItemAfterSpawn(edict_t *ent,Pickup *pickup){return(rules->checkItemAfterSpawn(ent,pickup));}
	void	checkEvents(void){rules->checkEvents();}
	void	respawnUntouchedItem(edict_t *ent) {rules->respawnUntouchedItem(ent);}
	void	setRespawn(edict_t *ent, float delay) {rules->setRespawn(ent, delay);}
	int		getMaxEncumbrance(void){return (rules->getMaxEncumbrance());}
	void 	AssignTeam(edict_t *ent, char* userinfo){rules->AssignTeam(ent, userinfo);}
	void	ClearFlagCount(void){rules->ClearFlagCount();};
	void	FlagCaptured(team_t team){rules->FlagCaptured(team);};
	bool	CaptureLimitHit(int limit){return(rules->CaptureLimitHit(limit));};

		
	int		dmRule_NO_HEALTH(void){return (rules->dmRule_NO_HEALTH());}
	int		dmRule_NO_ITEMS(void){return (rules->dmRule_NO_ITEMS());}
	int		dmRule_WEAPONS_STAY(void){return (rules->dmRule_WEAPONS_STAY());}
	int		dmRule_NO_FALLING(void){return (rules->dmRule_NO_FALLING());}
	int		dmRule_REALISTIC_DAMAGE(void){return (rules->dmRule_REALISTIC_DAMAGE());}
	int		dmRule_SAME_LEVEL(void){return (rules->dmRule_SAME_LEVEL());}
	int		dmRule_NOWEAPRELOAD(void){return (rules->dmRule_NOWEAPRELOAD());}
	int		dmRule_TEAMPLAY(void){return (rules->dmRule_TEAMPLAY());}
	int		dmRule_NO_FRIENDLY_FIRE(void){return (rules->dmRule_NO_FRIENDLY_FIRE());}
	int		dmRule_FORCE_RESPAWN(void){return (rules->dmRule_FORCE_RESPAWN());}
	int		dmRule_NO_ARMOR(void){return (rules->dmRule_NO_ARMOR());}
	int		dmRule_INFINITE_AMMO(void){return (rules->dmRule_INFINITE_AMMO());}
	int		dmRule_SPINNINGPICKUPS(void){return (rules->dmRule_SPINNINGPICKUPS());}
	int		dmRule_FORCEJOIN(void){return (rules->dmRule_FORCEJOIN());}
	int		dmRule_ARMOR_PROTECT(void){return (rules->dmRule_ARMOR_PROTECT());}
	int		dmRule_BULLET_WPNS_ONLY(void){return (rules->dmRule_BULLET_WPNS_ONLY());}
	
	int		arePlayerNamesOn(void){return (rules->arePlayerNamesOn());}
	int		arePlayerTeamsOn(void){return (rules->arePlayerTeamsOn());}
	int		allowAutoWeaponSwitch(void){return (rules->allowAutoWeaponSwitch());}

	float	clientCalculateWaver(edict_t &ent, int atkID) { return rules->clientCalculateWaver(ent, atkID);}
	void	clientConnect(edict_t &ent){rules->initPlayerDMInfo(ent);rules->clientConnect(ent);}
	void	clientDie(edict_t &ent, edict_t &inflictor, edict_t &killer){rules->clientDie(ent, inflictor, killer);}
	void	clientDieWeaponHandler(edict_t *ent){rules->clientDieWeaponHandler(ent);}
	void	clientPreDisconnect(edict_t &ent){rules->clientPreDisconnect(ent);}
	void	clientDisconnect(edict_t &ent){rules->clientDisconnect(ent);}
	void	clientDropItem(edict_t *ent,int type,int ammoCount){rules->clientDropItem(ent,type,ammoCount);}
	void	clientDropWeapon(edict_t *ent,int type, int clipSize){rules->clientDropWeapon(ent,type, clipSize);}
	void	clientEndFrame(edict_t *ent){rules->clientEndFrame(ent);}
	float	clientGetMovescale(edict_t *ent) { return rules->clientGetMovescale(ent);}
	void	clientHandlePredn(edict_t &ent){rules->clientHandlePredn(ent);}
	void	clientHelpMessage(edict_t *ent){rules->clientHelpMessage(ent);}
	void	clientObituary(edict_t *self, edict_t *inflictor, edict_t *attacker){rules->clientObituary(self,inflictor,attacker);}
	void	clientRespawn(edict_t &ent){rules->clientRespawn(ent);}
	void	clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file){rules->clientScoreboardMessage(ent,killer,log_file);}
	void	clientStealthAndFatigueMeter(edict_t *ent){rules->clientStealthAndFatigueMeter(ent);}

	bool	IsUnprotectedGoreZone(gz_code zone) { return rules->IsUnprotectedGoreZone(zone); }
	bool	IsLimbZone(gz_code zone) { return rules->IsLimbZone(zone); }
	void	HitByAttacker(edict_t *body, edict_t *attacker){rules->HitByAttacker(body, attacker);}
	bool	IsShootableWeapon(char *weaponname){return rules->IsShootableWeapon(weaponname);}
 
};