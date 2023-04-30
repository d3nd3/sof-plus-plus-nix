class gamerules_c;

#define SOFREE_SCOREBOARD 1.0
#define SOFREE_MINECRAFT 2.0

extern void applyDeathmatchHooks(unsigned int base_addr);
extern void freeDeathmatchHooks(void);
extern gamerules_c * currentGameMode;


typedef int gz_code;
class Pickup;
/*
============
gamerules_c
============
	make sure all of these are empty, they guna route to another.
*/
class gamerules_c
{
	
protected:

	FILE	*filePtr;
	gamerules_c * orig;
public:

	virtual void	clientSetDroppedItemThink(edict_t *ent); //0
	virtual void	clientSetDroppedWeaponThink(edict_t *ent);//4

	void setOrigPointer(void * in_p) {
		orig = in_p;
	}

	void * getOrigPointer() {
		return orig;
	}
	
	//4
	virtual int		isDM(void); // 8
	//8
	virtual void	preLevelInit(void); //c
	//c
	virtual void	levelInit(void);// 10
	//10
	virtual char	*getGameName(void); // 14
	//14
	virtual int		checkItemSpawn(edict_t *ent,Pickup **pickup); //18
	//18
	virtual int		checkItemAfterSpawn(edict_t *ent,Pickup *pickup); //1c
	//1c
	virtual void	checkEvents(void); //20
	//20
	virtual	void	respawnUntouchedItem(edict_t *ent); // 24
	//24
	virtual void	setRespawn(edict_t *ent, float delay);//28
	//28
	virtual int		getMaxEncumbrance(void);//2c
	//2c
	virtual void 	AssignTeam(edict_t *ent, char *userinfo);//30
	//30
	virtual void	ClearFlagCount(void);//34
	//34
	virtual void	FlagCaptured(team_t team);//38
	//38
	virtual bool	CaptureLimitHit(int limit);//3c

	////THE WRAPPER DOESNT CONTAIN THESE 2
	//3c
	virtual	void	openLogFile(void);
	//40
	virtual	void	closeLogFile(void); //44

	//44
	virtual int		dmRule_NO_HEALTH(void); // 48

	//48
	virtual int		dmRule_NO_ITEMS(void); //4C
	//4c
	virtual int		dmRule_WEAPONS_STAY(void);//50
	//50
	virtual int		dmRule_NO_FALLING(void);//54
	//54
	virtual int		dmRule_REALISTIC_DAMAGE(void);//58
	//58
	virtual int		dmRule_SAME_LEVEL(void);//5C
	//5c
	virtual int		dmRule_NOWEAPRELOAD(void);//60
	//60
	virtual int		dmRule_TEAMPLAY(void);//64
	//64
	virtual int		dmRule_NO_FRIENDLY_FIRE(void);//68
	//68
	virtual int		dmRule_FORCE_RESPAWN(void); //6C
	//6C
	virtual int		dmRule_NO_ARMOR(void); // 70

	//70
	virtual int		dmRule_INFINITE_AMMO(void);// 74

	//74
	virtual int		dmRule_SPINNINGPICKUPS(void); // 78

	//78
	virtual int		dmRule_FORCEJOIN(void); // 7C

	//7C
	virtual int		dmRule_ARMOR_PROTECT(void); // 80

	//80
	virtual int		dmRule_BULLET_WPNS_ONLY(void); //84
	
	//84
	virtual int dmRule_TEAM_REDBLUE(void); // 88

	//88
	virtual int		arePlayerNamesOn(void); // 8C

	//8c
	virtual int		arePlayerTeamsOn(void); // 90  

	//90
	virtual int		allowAutoWeaponSwitch(void);// 94 //xx


	////THE WRAPPER DOESNT CONTAIN THESE 2
	//94
	virtual	void	initPlayerDMInfo(edict_t *ent);// 98
	//98
	virtual void registerPlayerIcons(void); //9C // xx

	//9c
	virtual	float	clientCalculateWaver(edict_t *ent, int atkID);


	//a0
	virtual void	clientConnect(edict_t *ent);

	//a4
	virtual void	clientDie(edict_t *ent, edict_t *inflictor, edict_t *killer);

	//ac
	virtual void	clientDieWeaponHandler(edict_t *ent);

	//b0
	virtual void	clientPreDisconnect(edict_t *ent);

	//b4
	virtual void	clientDisconnect(edict_t *ent);

	//b8
	virtual void	clientDropItem(edict_t *ent,int type,int ammoCount);

	//bc
	virtual	void	clientDropWeapon(edict_t *ent,int type, int clipSize);

	//c0
	virtual float	clientGetMovescale(edict_t *ent);

	//c4
	virtual void	clientEndFrame(edict_t *ent);

	//c8
	virtual	void	clientHandlePredn(edict_t *ent);

	//cc
	virtual void	clientHelpMessage(edict_t *ent);

	//d0
	virtual void	clientObituary(edict_t *self, edict_t *inflictor, edict_t *attacker);

	//d4
	virtual void	clientRespawn(edict_t *ent);
	//d8
	virtual void	clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file);

	//dc
	virtual void	clientStealthAndFatigueMeter(edict_t *ent);
	
	//e0
	virtual void clientTouchConquerTrigger(void * one, void * two);
	
	//e4
	virtual bool	IsUnprotectedGoreZone(gz_code zone);
	
	//e8
	virtual bool	IsLimbZone(gz_code zone);

	//ec
	virtual void	HitByAttacker(edict_t *body, edict_t *attacker);

	//7 extra functions at end
	//f0 /*we need to go up to 108*/
	virtual bool	IsShootableWeapon(char *weaponname);
	
	//f4
	virtual void unsetControlFlag(void * one);
	//f8
	virtual void setControlFlag(void * one);
	//fc
	virtual int tooManyFlags(void);
	//100
	virtual void addControlFlag(void* one);
	//104
	virtual void  registerConquerTrigger(edict_t * ent);
	//108
	virtual int isPointInConquerTrigger(void* one);



	//my addwed functions must be at end
	virtual void removeHooks(void);
	virtual void applyHooks(void);

};



class always_gamerules_c:public gamerules_c
{
	public:
		void	clientSetDroppedItemThink(edict_t *ent); //0
		void	clientSetDroppedWeaponThink(edict_t *ent);//4
		
		//4
		 int		isDM(void); // 8
		//8
		 void	preLevelInit(void); //c
		//c
		 void	levelInit(void);// 10
		//10
		 char	*getGameName(void); // 14
		//14
		 int		checkItemSpawn(edict_t *ent,Pickup **pickup); //18
		//18
		 int		checkItemAfterSpawn(edict_t *ent,Pickup *pickup); //1c
		//1c
		 void	checkEvents(void); //20
		//20
			void	respawnUntouchedItem(edict_t *ent); // 24
		//24
		 void	setRespawn(edict_t *ent, float delay);//28
		//28
		 int		getMaxEncumbrance(void);//2c
		//2c
		 void 	AssignTeam(edict_t *ent, char *userinfo);//30
		//30
		 void	ClearFlagCount(void);//34
		//34
		 void	FlagCaptured(team_t team);//38
		//38
		 bool	CaptureLimitHit(int limit);//3c

		////THE WRAPPER DOESNT CONTAIN THESE 2
		//3c
			void	openLogFile(void);
		//40
			void	closeLogFile(void); //44

		//44
		 int		dmRule_NO_HEALTH(void); // 48

		//48
		 int		dmRule_NO_ITEMS(void); //4C
		//4c
		 int		dmRule_WEAPONS_STAY(void);//50
		//50
		 int		dmRule_NO_FALLING(void);//54
		//54
		 int		dmRule_REALISTIC_DAMAGE(void);//58
		//58
		 int		dmRule_SAME_LEVEL(void);//5C
		//5c
		 int		dmRule_NOWEAPRELOAD(void);//60
		//60
		 int		dmRule_TEAMPLAY(void);//64
		//64
		 int		dmRule_NO_FRIENDLY_FIRE(void);//68
		//68
		 int		dmRule_FORCE_RESPAWN(void); //6C
		//6C
		 int		dmRule_NO_ARMOR(void); // 70

		//70
		 int		dmRule_INFINITE_AMMO(void);// 74

		//74
		 int		dmRule_SPINNINGPICKUPS(void); // 78

		//78
		 int		dmRule_FORCEJOIN(void); // 7C

		//7C
		 int		dmRule_ARMOR_PROTECT(void); // 80

		//80
		 int		dmRule_BULLET_WPNS_ONLY(void); //84
		
		//84
		 int 		dmRule_TEAM_REDBLUE(void);

		//88
		 int		arePlayerNamesOn(void); // 88

		//8c
		 int		arePlayerTeamsOn(void); // 8C  

		//90
		 int		allowAutoWeaponSwitch(void);// 90


		////THE WRAPPER DOESNT CONTAIN THESE 2
		//94
			void	initPlayerDMInfo(edict_t *ent);// 94
		//98
		 void registerPlayerIcons(void); //98

		//9c
			float	clientCalculateWaver(edict_t *ent, int atkID);


		//a0
		 void	clientConnect(edict_t *ent);

		//a4
		 void	clientDie(edict_t *ent, edict_t *inflictor, edict_t *killer);

		//ac
		 void	clientDieWeaponHandler(edict_t *ent);

		//b0
		 void	clientPreDisconnect(edict_t *ent);

		//b4
		 void	clientDisconnect(edict_t *ent);

		//b8
		 void	clientDropItem(edict_t *ent,int type,int ammoCount);

		//bc
			void	clientDropWeapon(edict_t *ent,int type, int clipSize);

		//c0
		 float	clientGetMovescale(edict_t *ent);

		//c4
		 void	clientEndFrame(edict_t *ent);

		//c8
			void	clientHandlePredn(edict_t *ent);

		//cc
		 void	clientHelpMessage(edict_t *ent);

		//d0
		 void	clientObituary(edict_t *self, edict_t *inflictor, edict_t *attacker);

		//d4
		 void	clientRespawn(edict_t *ent);
		//d8
		 void	clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file);

		//dc
		 void	clientStealthAndFatigueMeter(edict_t *ent);
		
		//e0
		 void clientTouchConquerTrigger(void * one, void * two);
		
		//e4
		 bool	IsUnprotectedGoreZone(gz_code zone);
		
		//e8
		 bool	IsLimbZone(gz_code zone);

		//ec
		 void	HitByAttacker(edict_t *body, edict_t *attacker);

		//7 extra functions at end
		//f0 /*we need to go up to 108*/
		 bool	IsShootableWeapon(char *weaponname);
		
		//f4
		 void unsetControlFlag(void * one);
		//f8
		 void setControlFlag(void * one);
		//fc
		 int tooManyFlags(void);
		//100
		 void addControlFlag(void* one);
		//104
		 void registerConquerTrigger(edict_t * ent);
		//108
		 int isPointInConquerTrigger(void* one);



		//my addwed functions must be at end
		 void removeHooks(void);
		 void applyHooks(void);

};

/*
============
dm_sofree_c
============
*/

class dm_sofree_c:public gamerules_c
{
public:
	void removeHooks(void);
	void applyHooks(void);

	void levelInit(void);
	void clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file);
	char	*getGameName(void);
	void clientRespawn(edict_t *ent);
	void clientConnect(edict_t * ent);

	int 	dmRule_NO_HEALTH(void); // 48
	int		dmRule_NO_ITEMS(void); //4C
	int		dmRule_WEAPONS_STAY(void);//50
	int		dmRule_NO_FALLING(void);//54
	int		dmRule_REALISTIC_DAMAGE(void);//58
	int		dmRule_SAME_LEVEL(void);//5C
	int		dmRule_NOWEAPRELOAD(void);//60
	int		dmRule_TEAMPLAY(void);//64
	int		dmRule_NO_FRIENDLY_FIRE(void);//68
	int		dmRule_FORCE_RESPAWN(void); //6C
	int		dmRule_NO_ARMOR(void); // 70
	int		dmRule_INFINITE_AMMO(void);// 74
	int		dmRule_SPINNINGPICKUPS(void); // 78
	// int		dmRule_FORCEJOIN(void); // 7C
	// int		dmRule_ARMOR_PROTECT(void); // 80
	int		dmRule_BULLET_WPNS_ONLY(void); //84
};

class dm_standard_c:public gamerules_c
{
public:
	// void levelInit(void);
	void applyHooks(void);
	void removeHooks(void);

};

class dm_realistic_c:public gamerules_c
{
public:

	float clientGetMovescale(edict_t *ent);

};

class dm_ctf_c:public gamerules_c
{
public:
	// void levelInit(void);
	void applyHooks(void);
	void removeHooks(void);


};


extern always_gamerules_c dm_always;
//dm_sofree_c
extern dm_sofree_c dm_sofree;

// dm_none_c dm_none;
extern dm_standard_c dm_standard;
// dm_assassin_c dm_assassin;
// dm_arsenal_c dm_arsenal;
extern dm_ctf_c dm_ctf;
extern dm_realistic_c dm_realistic;
// dm_control_c dm_control;
// dm_ctb_c dm_ctb;