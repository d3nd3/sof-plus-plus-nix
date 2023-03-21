#ifndef __CWEAPONINFO_H_
#define __CWEAPONINFO_H_

#include "../qcommon/matrix4.h"
#include "../ghoul/ighoul.h"
#include "../qcommon/w_types.h"

//---------------------------------------------------------------------------------------


// 12/27/99 kef -- this is old school item management. it worked, but barely. also, it sucked. now
//we're going to use thePickupList (see declaration below of class PickupList).
struct itemSpawnInfo_s
{
	char	*spawnName;
	int		type;
	int		spawnParm;
	int		defaultCount;
	int		index;
	char	*worldName;
	int		pickupstringindex;
	int		ammopickupstringindex;
	int		itemfullstringindex;
};

typedef struct itemSpawnInfo_s itemSpawnInfo_t;


//
// kef -- I'm real fuzzy on the definition and use of spawnflags for pickups, I know there's
// SF_PICKUP_RESPAWN in g_local.h, but I really don't want to touch g_local right now. or ever, actually.
//
#define SF_PICKUP_START_CRATED		8	

//---------------------------------------------------------------------------------------


// 12/27/99 kef -- we're going to use instances of class Pickup (stored in thePickupList) instead of 
//itemSpawnInfo_t.
//
//	m_spawnName				name used by the editor to identify this item. this is what designers see in Radiant.
//	m_type					what type of item this is (e.g. weapon). pickup_t is defined in qcommon/w_types.h.
//	m_spawnParm				one of several possible enums, depending on the m_type. all possible values are defined in qcommon/w_types.h.
//	m_defaultCount			the default amount of this item you receive when you pick it up
//	m_defaultMaxCount		the default maximum amount of this item that can be carried
//  m_pickupListIndex		uniquely identifies this item. use this value as an index into the global list of Pickups, thePickupList.
//	m_worldName				what gets displayed during gameplay when you drop this item. need to be string-packaged. this may be a legacy comment, but FIXME.
//	m_pickupStringIndex 	displayed during gameplay when you pickup an item of this type. 
//	m_ammoPickupStringIndex	displayed during gameplay when you pickup a weapon of this type but you already have it so all you get is ammo.
//	m_itemFullStringIndex	displayed during gameplay when you touch an item of this type but can't carry any more.
//	m_modelSpawnData		this is the model data (name of model, skin, etc.) needed to create a pickup of this type.
//	m_fadeInSeq				this is the sequence that's called when a deathmatch pickup respawns

							/*
								NOTE:	I'm so, so sorry to do it this way, but apparently the save/load stuff in 
										g_save.cpp needs an array of model info for the pickups so I have to store
										each pickup's data in a big array somewhere and only keep a pointer to it
										(hence, m_modelSpawnData) in each Pickup instance. 

										still, this is better than it was...in the dark times... 
							*/

//
// ----------> TO ADD NEW ITEMS <-----------
//
// you'll need to add a modelSpawnData entry to pickupModelData[] in CWeaponInfo.cpp, which means increasing the 
//size of MD_ITEM_SIZE accordingly. also, you'll need to add an entry to PickupList::m_pickups in PickupList::PickupList(...)
//which also resides in CWeaponInfo.cpp. finally (I think), add an entry to the objType_t enum type in qcommon/w_types.h
// if the item you've added has any special stuff associated with it (needs bbox adjustment to lay flat, has a special
//skin, needs to be scaled up or down, etc.) see the helpers like PickupList::SetPickupSkin in CWeaponInfo.cpp.
//

#define MAXCHARSFORPICKUPNAME		64
#define CLIP_SHOULD_BE_FILLED		-2

class Pickup
{
protected:
	char				m_spawnName[MAXCHARSFORPICKUPNAME];
	pickup_t			m_type;
	int					m_spawnParm;
	int					m_defaultCount;
	int					m_defaultMaxCount;
	objType_t			m_pickupListIndex;
	char				m_worldName[MAXCHARSFORPICKUPNAME];
	int					m_pickupStringIndex;
	int					m_ammoPickupStringIndex;
	int					m_itemFullStringIndex;
	modelSpawnData_t	*m_modelSpawnData;
	int					m_fadeStatus;
	float				m_respawnTime;

	// extra stuff that a given pickup may or may not need
	char				m_skin[MAXCHARSFORPICKUPNAME];
	float				m_scale;
	vec3_t				m_bboxAdjustMins;
	modelSpawnData_t	*m_lowPolyModelSpawnData;
	char				m_fadeInSeq[MAXCHARSFORPICKUPNAME];
	int					m_droppedStringIndex; // because Keith was naughty and didn't package the worldnames


public:
				Pickup();

				// i anticipate this is the only constructor we'll ever use...a list of 'em should be in the 
				//constructor for class PickupList. therefore thePickupList will have to call Init on each Pickup prior to
				//spawning any items, probably in SpawnEntities.
				Pickup(	char		*yourSpawnNameHere,
						pickup_t	yourTypeHere,
						int			yourSpawnParmHere,
						float		yourRespawnTimeHere,
						int			yourDefaultCountHere,
						int			yourDefaultMaxCountHere,
						objType_t	yourPickupListIndexHere,
						char		*yourWorldNameHere,
						int			yourPickupStringIndexHere,
						int			yourAmmoPickupStringIndexHere,
						int			yourItemFullStringIndexHere
						);
	virtual		~Pickup();

	// call this before spawning any pickups
	int			Init();

	char				*GetSpawnName() { return m_spawnName; }
	pickup_t			GetType() { return m_type; }
	int					GetSpawnParm() { return m_spawnParm; }
	int					GetDefaultCount() { return m_defaultCount; }
	virtual int			GetDefaultMaxCount();
	float				GetRespawnTime() { return m_respawnTime; }
	objType_t			GetPickupListIndex() { return m_pickupListIndex; }
	char				*GetWorldName();
	int					GetPickupStringIndex() { return m_pickupStringIndex; }
	int					GetAmmoPickupStringIndex() { return m_ammoPickupStringIndex; }
	int					GetItemFullStringIndex() { return m_itemFullStringIndex; }
	modelSpawnData_t	*GetModelSpawnData() { return m_modelSpawnData; }
	char				*GetModelName() { return m_modelSpawnData?m_modelSpawnData->file:NULL; }
	char				*GetModelDir() { return m_modelSpawnData?m_modelSpawnData->dir:NULL; }
	int					GetDroppedStringIndex() {return m_droppedStringIndex;}
						
	void				SetSkin(char *skin) { strncpy(m_skin, skin, MAXCHARSFORPICKUPNAME); }
	char				*GetSkin() { return (m_skin[0] == 0?NULL:m_skin); }
	void				SetScale(float scale) { m_scale = scale; }
	float				GetScale() { return m_scale; }
	void				SetBBoxAdjustMins(vec3_t adj) { VectorCopy(adj, m_bboxAdjustMins); }
	void				GetBBoxAdjustMins(vec3_t putAdjHere) { VectorCopy(m_bboxAdjustMins, putAdjHere); }
	void				SetLowPolyModel(modelSpawnData_t *modelData) { m_lowPolyModelSpawnData = modelData; }
	modelSpawnData_t	*GetLowPolyModel() { return m_lowPolyModelSpawnData; }
	char				*GetFadeInSeq() { return m_fadeInSeq; }
	void				SetFadeInSeq(char *seq);
	void				SetGSQ(char *gsq);
	void				SetDroppedStringIndex(int dropIndex) { m_droppedStringIndex = dropIndex; }
						
	bool				ComparePickupType(pickup_t type) { return (m_type == type); }
	bool				CompareEquipmentType(equipment_t type) { return ( (PU_INV==m_type)?(m_spawnParm == type):false ); }
	bool				CompareWeaponType(weapons_t type) { return ( (PU_WEAPON==m_type)?(m_spawnParm == type):false ); }
	bool				CompareSpawnParm(int spawnParm) { return (spawnParm == m_spawnParm); }

	virtual bool		IsBulletWpn()		{ return false; }
	virtual bool		IsProjectileWpn()	{ return false; }
	virtual bool		IsExplosiveWpn()	{ return false; }
	virtual bool		IsBulletAmmo()		{ return false; }
	virtual bool		IsProjectileAmmo()	{ return false; }
	virtual bool		IsExplosiveAmmo()	{ return false; }
};


//---------------------------------------------------------------------------------------

class MedKitPickup : public Pickup
{
public:
				MedKitPickup();

				// i anticipate this is the only constructor we'll ever use...a list of 'em should be in the 
				//constructor for class PickupList. therefore thePickupList will have to call Init on each Pickup prior to
				//spawning any items, probably in SpawnEntities.
				MedKitPickup(	char		*yourSpawnNameHere,
								pickup_t	yourTypeHere,
								int			yourSpawnParmHere,
								float		yourRespawnTimeHere,
								int			yourDefaultCountHere,
								int			yourDefaultMaxCountHere,
								objType_t	yourPickupListIndexHere,
								char		*yourWorldNameHere,
								int			yourPickupStringIndexHere,
								int			yourAmmoPickupStringIndexHere,
								int			yourItemFullStringIndexHere
							);
	virtual		~MedKitPickup();
		int		GetDefaultMaxCount();
};

//---------------------------------------------------------------------------------------

class LightGogglesPickup : public Pickup
{
public:
	LightGogglesPickup();

				// i anticipate this is the only constructor we'll ever use...a list of 'em should be in the 
				//constructor for class PickupList. therefore thePickupList will have to call Init on each Pickup prior to
				//spawning any items, probably in SpawnEntities.
	LightGogglesPickup(	char		*yourSpawnNameHere,
						pickup_t	yourTypeHere,
						int			yourSpawnParmHere,
						float		yourRespawnTimeHere,
						int			yourDefaultCountHere,
						int			yourDefaultMaxCountHere,
						objType_t	yourPickupListIndexHere,
						char		*yourWorldNameHere,
						int			yourPickupStringIndexHere,
						int			yourAmmoPickupStringIndexHere,
						int			yourItemFullStringIndexHere
						);
		int		GetDefaultMaxCount();
};

//---------------------------------------------------------------------------------------

class WeaponPickup : public Pickup
{
public:

	enum
	{
		wpnFlag_NONE =				0,
		wpnFlag_BULLET =			(1<<0),
		wpnFlag_PROJECTILE =		(1<<1),
		wpnFlag_EXPLOSIVE =			(1<<2)
	} wpnPickupFlags_e;

private:
	int				m_wpnFlags;

public:
				WeaponPickup();

				// i anticipate this is the only constructor we'll ever use...a list of 'em should be in the 
				//constructor for class PickupList. therefore thePickupList will have to call Init on each Pickup prior to
				//spawning any items, probably in SpawnEntities.
				WeaponPickup(	char		*yourSpawnNameHere,
								pickup_t	yourTypeHere,
								int			yourSpawnParmHere,
								float		yourRespawnTimeHere,
								int			yourDefaultCountHere,
								int			yourDefaultMaxCountHere,
								objType_t	yourPickupListIndexHere,
								char		*yourWorldNameHere,
								int			yourPickupStringIndexHere,
								int			yourAmmoPickupStringIndexHere,
								int			yourItemFullStringIndexHere,
								int			yourWpnFlagsHere = 0
							);

	virtual bool	IsBulletWpn()		{ return !!(m_wpnFlags & wpnFlag_BULLET); }
	virtual bool	IsProjectileWpn()	{ return !!(m_wpnFlags & wpnFlag_PROJECTILE); }
	virtual bool	IsExplosiveWpn()	{ return !!(m_wpnFlags & wpnFlag_EXPLOSIVE); }
};

//---------------------------------------------------------------------------------------

class AmmoPickup : public Pickup
{
public:

	enum
	{
		ammoFlag_NONE =				0,
		ammoFlag_BULLET =			(1<<0),
		ammoFlag_PROJECTILE =		(1<<1),
		ammoFlag_EXPLOSIVE =		(1<<2)
	} ammoPickupFlags_e;

private:
	int				m_ammoFlags;

public:
				AmmoPickup();

				// i anticipate this is the only constructor we'll ever use...a list of 'em should be in the 
				//constructor for class PickupList. therefore thePickupList will have to call Init on each Pickup prior to
				//spawning any items, probably in SpawnEntities.
				AmmoPickup(		char		*yourSpawnNameHere,
								pickup_t	yourTypeHere,
								int			yourSpawnParmHere,
								float		yourRespawnTimeHere,
								int			yourDefaultCountHere,
								int			yourDefaultMaxCountHere,
								objType_t	yourPickupListIndexHere,
								char		*yourWorldNameHere,
								int			yourPickupStringIndexHere,
								int			yourAmmoPickupStringIndexHere,
								int			yourItemFullStringIndexHere,
								int			yourAmmoFlagsHere = 0
							);

	virtual bool	IsBulletAmmo()		{ return !!(m_ammoFlags & ammoFlag_BULLET); }
	virtual bool	IsProjectileAmmo()	{ return !!(m_ammoFlags & ammoFlag_PROJECTILE); }
	virtual bool	IsExplosiveAmmo()	{ return !!(m_ammoFlags & ammoFlag_EXPLOSIVE); }
};

//---------------------------------------------------------------------------------------

//
// class PickupInst
//
// this is an instance of a Pickup. 
//

class PickupInst
{
private:
	int				m_pickup;
	edict_t			*m_edict;
	int				m_fadeStatus;

public:
	// used to indicate whether or not a PickupInst is fading in 
	enum
	{
		fading_NONE = -1,		// never done any fading...only init'd
		fading_FALSE,			// finished fading in
		fading_TRUE,			// in the process of fading in
		fading_REMOVECALLBACK,	// I just finished fading, please remove my callback
		fading_MAX				// don't move this value. keep it at the end of the list.
	};

				PickupInst(edict_t *ent);
				PickupInst(void) { m_edict = NULL; }
	virtual		~PickupInst();

	IGhoulInst			*GetGhoulInst();
	edict_t				*GetEdict() { return m_edict; }
	Pickup				*GetPickup();
	bool				SetFadeStatus(int status) { if ((status>=fading_NONE)&&(status<fading_MAX))m_fadeStatus = status;return(m_fadeStatus == status); }
	int					GetFadeStatus() { return m_fadeStatus; }
	void				FadeIn();
	void				RespawnCallback();
	bool				RemoveCallBack();
	bool				EdictCompare(edict_t *ent);

	void				Write(void);
	void				Read(void);
};


//
// class PickupInstInfo
//
// this class is only used when pickupinfo is nonzero. it stores various pieces of information about the use of
//the pickups in a given level. a PickupInstInfo corresponds to a PickupInst, kind of.
//
class PickupInstInfo
{
public:
	enum
	{
		type_NONE = 0,
		type_LOADTIME,			// this inst was created at loadtime (placed in the editor)
		type_RUNTIME			// this inst was created at runtime (during gameplay, e.g. dropped weapon)
	};

private:
	PickupInst		*m_pickUpInst;
	int				m_pickedUpCount;
	float			m_lastRespawnTime;
	float			m_lastPickedUpTime;
	float			m_totalPickedUpDelay;
	float			m_avgPickedUpDelay;	// average amount of time since last picked up
	float			m_totalRespawnPickUpDelay;
	float			m_avgRespawnPickUpDelay; // avg amount of time between respawn and getting picked up
	int				m_touchCount;	// folks can touch a pickup without picking it up
	float			m_lastTouchTime;
	float			m_totalTouchDelay;
	float			m_avgTouchDelay;
	int				m_type;

public:
						PickupInstInfo(PickupInst *inst, int type = type_LOADTIME);
	virtual				~PickupInstInfo();

	PickupInst			*GetPickUpInst() { return m_pickUpInst; }
	bool				Compare(PickupInst* pickupInst);
	bool				Compare(edict_t *ent);
	void				PickedUp(float fTime);
	void				Touch(float fTime);
	int					GetPickedUpCount() { return m_pickedUpCount; }
	float				GetAvgPickedUpDelay() { return m_avgPickedUpDelay; }
	float				GetAvgRespawnPickUpDelay() { return m_avgRespawnPickUpDelay; }
	float				GetAvgTouchDelay() { return m_avgTouchDelay; }
	int					GetTouchCount() { return m_touchCount; }
	int					GetType() { return m_type; }
	void				SetRespawnTime(float fTime) { m_lastRespawnTime = fTime; }
};


//
// class PickupInfo
//
// tracks all of the PickupInstInfo's for a given type of pickup. corresponds to a Pickup.
//
class PickupInfo
{
private:
	objType_t				m_objType;
	list<PickupInstInfo*>	m_insts;

public:
						PickupInfo(objType_t type);
	virtual				~PickupInfo();

	PickupInstInfo		*AddInst(PickupInst *pickupInst, int type = PickupInstInfo::type_LOADTIME);
	PickupInstInfo		*Find(edict_t *ent);
	PickupInstInfo		*Find(PickupInst *pickupInst);

	bool				PickedUp(PickupInst *pickupInst, float fTime);
	bool				SetRespawnTime(PickupInst *pickupInst, float fTime);
	bool				Touch(PickupInst *pickupInst, float fTime);
};


//
// class PickupInfoList
//
// PickupInfoList corresponds to PickupList...it keeps track of all of the PickupInfo's.
//
class PickupInfoList
{
public:
	// this enum represents various events that we might want to keep track of for pickups
	enum
	{
		event_UNKNOWN = -1,
		event_NONE = 0,
		event_CREATE_LOADTIME,
		event_CREATE_RUNTIME,
		event_PICKEDUP,
		event_RESPAWN,
		event_TOUCHED,
		event_MAXEVENTS
	};

private:
	map<objType_t, PickupInfo*>	m_info;
	int							m_failedMsgs[event_MAXEVENTS];

	// file pointer for dumping info
	FILE						*m_infoFile;
	bool						m_bInfoFileOpened;

private:
	PickupInfo*			AddPickupInst(PickupInst *pickupInst, int type = PickupInstInfo::type_LOADTIME);
	PickupInstInfo		*Find(PickupInst *pickupInst);

public:
						PickupInfoList();
	virtual				~PickupInfoList();

	bool				HandleEvent(int nEvent, PickupInst *pickupInst);
	int					GetPickedUpCount(PickupInst *pickupInst);
	float				GetAvgPickedUpDelay(PickupInst *pickupInst);
	float				GetAvgRespawnPickUpDelay(PickupInst *pickupInst);
	float				GetAvgTouchDelay(PickupInst *pickupInst);
	int					GetTouchCount(PickupInst *pickupInst);
	int					GetType(PickupInst *pickupInst);

	void				WriteLine(char *text);
};


//---------------------------------------------------------------------------------------

// CratePickupInsts are just PickupInsts that become available to the player when a crate is broken open.
//this class mostly serves as a node in a linked list stack.
class CratePickupInst
{
public:
	CratePickupInst		*m_next;
	
	PickupInst			*m_pickupInst;

public:
	CratePickupInst(PickupInst *inst) { m_next = NULL; m_pickupInst = inst; }
	virtual ~CratePickupInst() { m_next = NULL; m_pickupInst = NULL; }

};

// CratePickupInstList is the manager for the CratePickupInsts being created when a crate is broken open.
//mostly a wrapper for a linked list stack.
class CratePickupInstList
{
public:
	CratePickupInst		*m_head;

	// don't allocate space for m_name cuz it just points to the edict_t::target of the crate
	char				*m_name;

public:
	CratePickupInstList() { m_head = NULL; m_name = NULL; }
	virtual ~CratePickupInstList() { Clear(); m_name = NULL; }

	void			Create(char *name);
	void			Destroy() { Clear(); }

	void			Push(PickupInst *inst) { CratePickupInst *newInst = new CratePickupInst(inst); newInst->m_next = m_head; m_head = newInst; }
	PickupInst		*Pop() { CratePickupInst *popped = m_head; if(!popped)return NULL; PickupInst *poppedPickup = popped->m_pickupInst; m_head = m_head->m_next; delete popped; return poppedPickup;}

protected:
	void			Clear() {while (m_head) Pop();}

	bool			Compare(char *name) { if (m_name) return !strcmp(name, m_name); return false; }
};


// 12/27/99 kef -- we're going to have a single, global instance of class PickupList called thePickupList.
//it replaces any and all references to itemSpawns.

class PickupList
{
public:
	enum
	{
		print_NONE = 0,
		print_CONSOLE,
		print_FILE
	};

private:
	// this is the actual data for the global list of items. there simply is no good way to represent it...dynamic
	//sizing is bad, static sizing is bad. dynamic allocation is bad, static allocation is bad. grrr. 
	//since we're using objType_t as a valid, unique index into this list, though, we may as well declare this
	//array in terms of the maximum size of objType_t.
	Pickup			*m_pickups[MAX_ITM_OBJS];

	// this is a list of entities currently using callbacks. we poll them in FrameUpdate to see if they're done
	//with their callback, in which case we remove the callback.
	list<PickupInst*>	m_pickupInsts;

	// if pickupinfo tells us to, create a PickupInfoList in PickupList::Init()
	PickupInfoList	*m_pickupInfoList;

	// have we display help for weaponarena already?
	bool			m_bDisplayedWeaponArenaHelp;


	// manager for pickups stored in crates
	CratePickupInstList	m_crateList;

	// these are some helpers set up to deal with per-pickup modifications
	void			SetPickupSkin(int listIndex);
	void			ModifyPickupBBox(int listIndex);
	void			SetPickupScale(int listIndex);
	void			SetPickupLowPolyModel(int listIndex);
	void			SetGSQ(int listIndex);
	void			SetDroppedStringIndex(int listIndex);

	void			WriteLine(int printDest, char *outLine, ...);

	void			InitCrated(edict_t *ent);

public:
	PickupList();
	virtual ~PickupList();

	// init the modelSpawnData for all pickups. call this from someplace like SpawnEntities.
	int				PreSpawnInit();		

	// call this after spawning pickups
	int				PostSpawnInit();

	void			Destroy();		// call before changing maps/quitting game
	int				FrameUpdate();	// checks the list each frame, mostly for removal of callbacks
	void			DumpList(int printDest = print_CONSOLE);		// output the contents of m_pickups to the console or a file

	Pickup			*GetPickup(int listIndex) { return ((listIndex>OBJ_NONE)&&(listIndex<MAX_ITM_OBJS))?m_pickups[listIndex]:NULL; }
	Pickup			*GetPickupFromSpawnName(char *name);
	Pickup			*GetPickupFromEdict(edict_t *ent);
	int				GetPickupIndexFromEdict(edict_t *ent);
	Pickup			*GetPickupFromType(pickup_t type, int spawnParm);

	PickupInst		*GetPickupInstFromTargetname(char* targetname);
	PickupInst		*GetPickupInstFromEdict(edict_t *ent);
	void			RegisterEdict(edict_t *ent);
	bool			Unregister(edict_t *ent);
	bool			UnregisterAll();

	bool			HandleInfoEvent(int nEvent, edict_t *ent);
	float			WeaponArena(float cvarVal);
	void			WeaponArenaHelp();

	void			BustCrate(edict_t *ent);

	void			Write(void);
	void			Read(void);
};

//---------------------------------------------------------------------------------------

class CWeaponInfo;

typedef struct weaponFireInfo_s
{
	edict_t		*ent;
	vec3_t		firePoint;
	vec3_t		fwd;
	IGhoulInst	*inst;
	CWeaponInfo	*weapon;
	void (*hitfunc)(edict_t *ent, void* data);
	float		vel;
	vec3_t		endPos;//for returning info
	int			gunPointSet;
	vec3_t		gunPoint;//not always the same as the fire point
}weaponFireInfo_t;

class CWeaponInfo
{
private:
	float				reloadTime;
	float				dmg;
	float				bulletSize;
	int					damageFlag;
	int					MOD;
	char				mflashView[64];
	char				mflashDM[64];
	char				mflashSingle[64];
	char				tracerFX[64];
	float				sndRange;
	float				addToSpawn;
	float				enemyAddToSpawn;
	float				dPenetrate;
	float				dAbsorb;
	void				(*fire)(weaponFireInfo_t &wf);
	void				(*precache)(void);
	float				effectiveRange;
	int					mflashEfView;
	int					mflashEfDM;
	int					mflashEfSingle;
	int					tracerEf;
	qboolean			bighit;
	int					index;

public:
	CWeaponInfo(char *newMFlashView, char *newMFlashDM, char *newMFlashSingle, float soundRange, float damage, float bSize, 
						 int dFlags, float spawnAdd, float enemySpawnAdd, int MeansODeath, void (*infunc)(weaponFireInfo_t &wf),
						 float newDPenetrate, float newDAbsorb, void (*cacheFunc)(void), float newEfRange, 
						 char *newtracerFX, qboolean newbighit, int newIndex);
	void				attack(edict_t *srcEnt, vec3_t org, vec3_t dir, IGhoulInst *gun = 0, GhoulID myFlashBolt = 0);
	void				attack(edict_t *srcEnt, vec3_t org, vec3_t dir, float vel);
	void				attack(edict_t *srcEnt, vec3_t org, vec3_t dir, void (*hitfunc)(edict_t *ent, void* data));
	
	//	dk - I've moved these access function definitions to CWeaponInfo.cpp so that we can easily modify them based on the current gamemode

	float				getDmg(void);
	int					getDmgFlags(void);
	int					getMOD(void);
	float				getPenetrate(void);
	float				getAbsorb(void);
	float				getBulletSize(void);
	float				getNoiseRad(void);
	int					getMFlashEfView(void);
	int					getMFlashEfDM(void);
	int					getMFlashEfSingle(void);
	int					getTracerEf(void);
	void				cache(void);
	float				getEffectiveRange(void);
	int					getIndex(void);
	qboolean			getBigHit(void);

	//	FILL IN OTHER ACCESS FUNCTIONS HERE...
};

class CWorldWeapons
{
private:
	CWeaponInfo	*weaps;
public:
	CWeaponInfo	&getWeapon(int num){assert(num < ATK_NUMWEAPONS); assert(num >= 0); assert(weaps); return weaps[num];}
	void attack(attacks_e weapID, edict_t *srcEnt, vec3_t org, vec3_t dir, IGhoulInst *gun = 0, GhoulID myFlashBolt = 0);
	void attack(attacks_e weapID, edict_t *srcEnt, vec3_t org, vec3_t dir, void (*bodyfunc)(edict_t *ent, void* data), IGhoulInst *gun = 0);
	void attack(attacks_e weapID, edict_t *srcEnt, vec3_t org, vec3_t dir, IGhoulInst *gun, float *vel);
	void setWeaps(CWeaponInfo *newWeaps){weaps = newWeaps;}
};

extern	CWorldWeapons weapons;
extern	PickupList thePickupList;

void	W_InitWorldWeaponModels(void);
void	W_ShutdownWorldWeaponModels(void);

void	W_InitInv(edict_t &ent);
void	W_RefreshWeapon(edict_t &ent);

#endif // __CWEAPONINFO_H_
