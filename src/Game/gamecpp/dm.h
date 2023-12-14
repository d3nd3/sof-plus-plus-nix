// *****************************************************************************
// dm.h
// ----
// Deathmatch stuff visisble to the rest of the game.dll.
// *****************************************************************************

#define ITEM_RESPAWN_FADEIN 0.8F

typedef enum {
	NOTEAM,
	TEAM1,
	TEAM2
} team_t;


struct TeamInfo_s
{
	int	rep;
	int	score;
	char	*teamname;
	int	flags;
	int	team;
};


void ClearEffects (edict_t *ent);

/*
============
player_dmInfo_ic
============
*/

class player_dmInfo_ic
{
public:
};

/*
============
games_t
============
*/

typedef enum
{
	DM_NONE,
	DM_STANDARD,
	DM_ASSASSIN,
	DM_ARSENAL,
	DM_CTF,
	DM_REAL,
}games_t;

/*
============
gamemode_ic
============
*/

// 12/28/99 kef -- declared in CWeaponInfo.h, class Pickup replaces itemSpawnInfo_t. and there was much rejoicing.
class Pickup;

typedef int gz_code;


class gamemode_ic
{
public:
	virtual void	setDMMode(int newtype)=0;
	virtual int		isDM(void)=0;
	virtual void	preLevelInit(void)=0;
	virtual void	levelInit(void)=0;
	virtual char	*getGameName(void)=0;
	virtual int		checkItemSpawn(edict_t *ent,Pickup **pickup)=0;
	virtual int		checkItemAfterSpawn(edict_t *ent,Pickup *pickup)=0;
	virtual void	checkEvents(void)=0;
	virtual	void	respawnUntouchedItem(edict_t *ent)=0;
	virtual void	setRespawn(edict_t *ent, float delay)=0;
	virtual int		getMaxEncumbrance(void)=0;
	virtual void 	AssignTeam(edict_t *ent, char *userinfo) = 0;
	virtual void	ClearFlagCount(void)=0;
	virtual void	FlagCaptured(team_t team)=0;
	virtual bool	CaptureLimitHit(int limit)=0;

	virtual int		dmRule_NO_HEALTH(void)=0;
	virtual int		dmRule_NO_ITEMS(void)=0;
	virtual int		dmRule_WEAPONS_STAY(void)=0;
	virtual int		dmRule_NO_FALLING(void)=0;
	virtual int		dmRule_REALISTIC_DAMAGE(void)=0;
	virtual int		dmRule_SAME_LEVEL(void)=0;
	virtual int		dmRule_NOWEAPRELOAD(void)=0;
	virtual int		dmRule_TEAMPLAY(void)=0;
	virtual int		dmRule_NO_FRIENDLY_FIRE(void)=0;
	virtual int		dmRule_FORCE_RESPAWN(void)=0;
	virtual int		dmRule_NO_ARMOR(void)=0;
	virtual int		dmRule_INFINITE_AMMO(void)=0;
	virtual int		dmRule_SPINNINGPICKUPS(void)=0;
	virtual int		dmRule_FORCEJOIN(void)=0;
	virtual int		dmRule_ARMOR_PROTECT(void)=0;
	virtual int		dmRule_BULLET_WPNS_ONLY(void)=0;

	virtual int		arePlayerNamesOn(void)=0;
	virtual int		arePlayerTeamsOn(void)=0;
	virtual int		allowAutoWeaponSwitch(void)=0;

	virtual	float	clientCalculateWaver(edict_t &ent, int atkID)=0;
	virtual void	clientConnect(edict_t &ent)=0;
	virtual void	clientDie(edict_t &ent, edict_t &inflictor, edict_t &killer)=0;
	virtual void	clientDieWeaponHandler(edict_t *ent)=0;
	virtual void	clientPreDisconnect(edict_t &ent)=0;
	virtual void	clientDisconnect(edict_t &ent)=0;
	virtual void	clientDropItem(edict_t *ent,int type,int ammoCount)=0;
	virtual void	clientDropWeapon(edict_t *ent,int type, int clipSize)=0;
	virtual float	clientGetMovescale(edict_t *ent)=0;
	virtual void	clientEndFrame(edict_t *ent)=0;
	virtual void	clientHandlePredn(edict_t &ent)=0;
	virtual void	clientHelpMessage(edict_t *ent)=0;
	virtual void	clientObituary(edict_t *self, edict_t *inflictor, edict_t *attacker)=0;
	virtual void	clientRespawn(edict_t &ent)=0;
	virtual void	clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file)=0;
	virtual void	clientStealthAndFatigueMeter(edict_t *ent)=0;

	virtual bool	IsUnprotectedGoreZone(gz_code zone)=0;
	virtual bool	IsLimbZone(gz_code zone)=0;
	virtual void	HitByAttacker(edict_t *body, edict_t *attacker)=0;
	virtual bool	IsShootableWeapon(char *weaponname)=0;
};

extern class gamemode_ic *dm;

void InitDeathmatchSystem(void);
void DeleteDeathmatchSystem(void);
void respawnUntouchedItem(edict_t* ent);
