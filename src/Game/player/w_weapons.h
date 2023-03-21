#ifndef __W_WEAPONS_H
#define __W_WEAPONS_H

#include "w_public.h"

#define MASK_PROJ	(CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER|CONTENTS_SHOT)
#define MASK_PROJ2	(MASK_PROJ|CONTENTS_WINDOW)

typedef struct sharedEdict_s sharedEdict_t;
typedef struct edict_s edict_t;

class clientRICBuf;
class serverRICBuf;

const int MAXWEAPONS			= 14;
const int MAXSELECTABLEWEAPONS	= 12;
const int MAXITEMS				= 6;
const float ITEM_USE_TIME		= .4;

enum
{
	WANIM_NORMAL,
	WANIM_RELOAD,
	WANIM_FIRE,
	WANIM_ALTFIRE,
	WANIM_PUTAWAY,
	WANIM_LOSE,
	WANIM_TYPES
};

typedef enum
{
	PWC_NOCOMMAND,
	PWC_RELOAD,
	PWC_WEAPNEXT,
	PWC_WEAPPREV,
	PWC_WEAPONBEST_SAFE,
	PWC_WEAPONBEST_UNSAFE,
	PWC_LOSE,
	PWC_DROP,
	PWC_WEAPSELECTFIRST,
	PWC_NUMCMDS				= PWC_WEAPSELECTFIRST + MAXSELECTABLEWEAPONS,
} pweapCmds_t;

typedef enum
{
	PEC_NOCOMMAND,
	PEC_ITEMNEXT,
	PEC_ITEMPREV,
	PEC_DROP,
	PEC_USEITEM,
	PEC_USEITEMFIRST,
	PEC_NUMCMDS				= PEC_USEITEMFIRST + MAXITEMS,
} pequipCmds_t;

typedef struct attackInfoExtra_s
{
	int			ammoType;
	float		noiseRad;
	float		waver;		// waver should be in terms of degrees per consecutive shot...
	float		waverMax;
	float		kickAmount;
	attacks_e	id;
}attackInfoExtra_t;

typedef struct weaponInfoExtra_s
{
	char				*name;
	int					slotsNeeded;
	int					clipsize;
	attackInfoExtra_t	*a1;
	attackInfoExtra_t	*a2;
} weaponInfoExtra_t;

enum
{
	FIRE_NORMAL = 0,
	FIRE_ALT,
};

//------------------------------------------------------------------------
//
// weaponInfo_c
//
//------------------------------------------------------------------------

class weaponInfo_c
{
private:
protected:

	int						clip;
	int						slotsNeeded;
	int						ammoType[2];
	float					noiseRad[2];
	float					waver[2];
	float					waverMax[2];
	char					modelName[128];//hardcodedness...
	GhoulID					soundToken;
	float					kick[2];
	attacks_e				atkID[2];
	IGhoulObj				*obj;
						
public:

	weaponInfo_c(void){soundToken = NULL_GhoulID;obj = 0;}
	weaponInfo_c(weaponInfoExtra_t *w);
	~weaponInfo_c(void){}

	int						getClipSize(void) {return clip;}
	int						getSlotsNeeded(void) {return slotsNeeded;}
	int						getAmmoType(int mode) {return ammoType[mode];}
	float					getWaver(int mode){return waver[mode];}
	float					getWaverMax(int mode){return waverMax[mode];}
	float					getKick(int mode){return kick[mode];}
	attacks_e				getAtkID(int mode){return atkID[mode];}
					
	char					*getWeaponName(void){return modelName;}

	virtual	char			*getFireSeq(sharedEdict_t &ent){return NULL;}
	virtual char			*getAltFireSeq(sharedEdict_t &ent){return NULL;}
	virtual	char			*getIdleSeq(sharedEdict_t &ent){return NULL;}
	virtual char			*getReloadSeq(sharedEdict_t &ent){return NULL;}
	virtual char			*getTakeOutSeq(sharedEdict_t &ent){return NULL;}
	virtual char			*getPutAwaySeq(sharedEdict_t &ent){return NULL;}
	virtual char			*getDryfireSeq(sharedEdict_t &ent){return NULL;}

	virtual char			*getName(void){return "";}
	virtual GhoulSpeed		getFireSpeed(void){return gsOne;}

	virtual void			shutdownWeapon(sharedEdict_t &ent){};
	virtual void			noattack(sharedEdict_t &ent){};

	virtual int				handleEOS(sharedEdict_t &ent){return 0;}
	virtual void			handleFireLoop(sharedEdict_t &ent){}
	virtual void			handleAltfireLoop(sharedEdict_t &ent){}
	virtual void			handleSpecialNotes(char *string, sharedEdict_t &ent){}

	virtual int				useAmmo1(sharedEdict_t &ent, int *skipIdle = 0);
	virtual int				useAmmo2(sharedEdict_t &ent, int *skipIdle = 0);
	virtual	void			turnOffAmmoParts(sharedEdict_t &ent,int clipLeft,qboolean forceAll=false){};

	virtual int				canSelectWithNoAmmo(void){return 0;}

	virtual void			setBloody(sharedEdict_t *self){}

	virtual	int				testInput(sharedEdict_t &ent){return 0;}

	void					setSoundToken(GhoulID newID){soundToken = newID;}
	GhoulID					getSoundToken(void){return soundToken;}
	void					setGhoulObj(IGhoulObj *newObj){obj = newObj;}
	IGhoulObj				*getGhoulObj(void){return obj;}

	virtual int				atMomentOfFire(sharedEdict_t &ent){return 1;}
	virtual int				isSingleReload(void){return 0;}

	float					getNoiseVal(int num){return noiseRad[num];}

	virtual void			handleFall(sharedEdict_t &ent){};
	virtual int				testReloadAtInit(sharedEdict_t &ent){return 1;}
	virtual int				checkReloadAtEOS(sharedEdict_t &ent);

	virtual int				hasAltFire(void){return 0;}
	virtual int				getType(void){return SFW_EMPTYSLOT;}
	virtual int				fireFromFlash(void){return 0;}
	virtual qboolean		aimAtCrosshair(void){return true;}
	virtual void			precacheResources(void);

	virtual IGhoulInst::EndCondition getLoopType(void){return IGhoulInst::Hold;}

//	virtual void			frameUpdate(sharedEdict_t &ent){};
};

//------------------------------------------------------------------------
//
// weapon_c
//
//------------------------------------------------------------------------

class weapon_c
{
private:

	int				curClip;
	int				type;
	int				cannotFire;

public:

	weapon_c(void);
	weapon_c(weaponInfo_c *wInfo);
	~weapon_c(void){};

	int				getType(void){return type;}
	void			setType(int newType, int startWithFullClip = 0);
	weaponInfo_c	*getWeaponInfo(void);
	int				handleAmmo(sharedEdict_t &ent, int mode, int *skipIdle = 0);
	int				canSelect(sharedEdict_t &ent,int withNoAmmo=0);
	int				getClip(void){return curClip;}
	void			setClip(int clip){curClip = clip;}
};

//------------------------------------------------------------------------
//
// itemSlot_c
//
//------------------------------------------------------------------------

class itemSlot_c
{
private:

	int itemType;
	int	ammo;
	int	status;
	float nextupdate;

public:

	itemSlot_c(void);
	~itemSlot_c(void){}

	void	clear(void);
	int		getSlotType(void){return itemType;}
	int		getSlotCount(void){return ammo;}
	int		getSlotStatus(void){return status;}
	float	getSlotUpdateTime(void){return nextupdate;}
	
	void	setSlotType(int newVal){itemType = newVal;}
	void	setSlotCount(int newVal){ammo = newVal;}
	void	setSlotStatus(int newVal){status = newVal;}
	void	setSlotUpdateTime(float newFloat){nextupdate = newFloat;}

	void	useAmmo(sharedEdict_t &ent);
};

//------------------------------------------------------------------------
//
// item_c
//
//------------------------------------------------------------------------

class item_c
{
private:
	
	int			curSlotNum;
	int			nextCommand;
	int			lastUseTime;
	itemSlot_c	slots[MAXITEMS];

	void		HandleInput(sharedEdict_t &ent);

public:

	item_c(void);
	~item_c(void){}

	itemSlot_c	*getCurSlot(void){return &slots[curSlotNum];}
	itemSlot_c	*getSlot(int i){return &slots[i];}
	int			getSlotNumFromName(char *itemName);
	void		frameUpdate(sharedEdict_t &ent);
	void		deactivate(sharedEdict_t &ent);
	int			addItemType(int type, int amount, int maxAmount);
	int			hasItemType(int type);
	void		setNextCommand(int cmd){nextCommand = cmd;}
	int			handleDamage(sharedEdict_t &ent, int initDamage);
	void		clearSlot(int slotNum){slots[slotNum].setSlotType(SFE_EMPTYSLOT);slots[slotNum].setSlotCount(0);}
	int			getCurType(void){return getCurSlot()->getSlotType();}
	int			getCurAmmo(void){return getCurSlot()->getSlotCount();}
	int			getNextCommand(void){return nextCommand;}
};

//------------------------------------------------------------------------
//
// wAnim_c
//
//------------------------------------------------------------------------

class wAnim_c
{
private:

	int			handsOnWeapon;
	int			weaponLifted;
	int			justTransitioned;
	int			bloodyKnife;
	int			handSide;
	char		curSeqName[MAX_QPATH];
	int			fireLoopNum;
	int			animType;
	int			knifeThrow;		// needed because of the split knife throwing anim...
	int			isDown;
	int			firstShotInSeq;	// for damn ingram
	int			inZoom;
	int			inZoomView;
	int			interesting;
	int			rezoom;
	int			inDisguise;
	float		lastShootTime;	//can't shoot twice in a frame

public:

	int			getJustTransitioned(void){return justTransitioned;}
	void		setTransition(int val){assert(val > -1);assert(val < 2);justTransitioned = val;}
	int			getHandsInUse(void){return handsOnWeapon;}
	void		setHandsInUse(int val){assert(val > 0);assert(val < 3);handsOnWeapon = val;}
	int			getWeaponLifted(void){return weaponLifted;}
	void		setWeaponLifted(int val){assert(val > -1);assert(val < 2);weaponLifted = val;}
	void		setBloody(int state){bloodyKnife = state;}
	int			getBloody(void){return bloodyKnife;}
	void		setHandSide(int newSide){handSide = newSide;}
	int			getHandSide(void){return handSide;}
	void		setSeqName(char *name){strncpy(curSeqName, name, MAX_QPATH);}
	char		*getSeqName(void){return curSeqName;}//eh?
	int			getFireLoop(void){return fireLoopNum;}
	void		incFireLoop(void){fireLoopNum++;}
	void		clearFireLoop(void){fireLoopNum = 0;}
	int			getAnimType(void){return animType;}
	void		setAnimType(int val){assert(val < WANIM_TYPES);assert(val > -1);animType = val;}
	int			getKnifeStatus(void){return knifeThrow;}
	void		setKnifeStatus(int val){knifeThrow = val;}
	int			getDown(void){return isDown;}
	void		setDown(int val){isDown = val;}
	int			firstShot(void){return firstShotInSeq;}
	void		setFirstShot(int val){firstShotInSeq = val;}
	int			getInZoom(void){return inZoom;}
	void		setInZoom(int val){inZoom = val;}
	int			getInZoomView(void){return inZoomView;}
	void		setInZoomView(int val){inZoomView = val;}
	int			getLastWasInteresting(void){return interesting;}
	void		setInteresting(int val){(val) ? interesting = 2:interesting--; if(interesting < 0)interesting = 0;}
	int			getRezoom(void){return rezoom;}
	void		setRezoom(int val){rezoom = val;}
	int			disguised(void){return inDisguise;}
	void		setDisguise(int val){inDisguise = val;}
	void		setlastShootTime(float val){lastShootTime = val;}
	float		getlastShootTime(void){return lastShootTime;}
};

//------------------------------------------------------------------------
//
// inven_c
//
//------------------------------------------------------------------------

class inven_c:public invPub_c
{
private:

	clientRICBuf	*clientRICBuffer;
	serverRICBuf	*serverRICBuffer;

#define INVEN_SAVE_START	offsetof(inven_c, nextCommand)

	int				nextCommand;
	int				clientOverrideCmd;
	int				newInstRxd;
	int				nextSelectSlot;
	int				curWeaponID;
	int				curWeaponType;
	int				curWeaponSlot;
	
	int				readiedWeaponID;
	int				readiedWeaponType;
	int				readiedWeaponSlot;
	
	int				droppingSlot;
	weapon_c		weapons[MAXWEAPONS];
	qboolean		rulesWeaponsUseAmmo;
	qboolean		rulesDropWeapons;
	int				rulesNonDroppableWeaponTypes;
	qboolean		rulesWeaponsReload;
	qboolean		rulesReloadEmptiesWeapon;
	qboolean		rulesWeaponReloadAutomatic;
	qboolean		rulesDropInvenOnDeath;
	qboolean		rulesSelectWithNoAmmo;
	char			rulesBestWeaponMode[5];
	qboolean		rulesFreelySelectWeapon;
	int				ammo[MAXAMMOS];
	int				canHandleInput;
	IGhoulInst		*model;
	
	IGhoulInst		*readyModel;

	int				selectedWeaponSlot;
	int				armor;
	int				weaponsOwned;
	int				pendingChange;
	int				clientPredicting;

	Matrix4			gunEnd;
	sharedEdict_t	*owner;

	item_c			items;

	wAnim_c			wAnim;

	void			handleInput(void);
	void			handleModelChange(void);
	int				getWeaponNum(int cmd);

public:

	inven_c(void);
	~inven_c(void);

	// -------------------
	// Inherited functions
	// -------------------
	// These are the interface to the invenory / weapon system and are callable
	// outside the player DLL.
	// -------------------

	void		initNewWeapon(void);
	void		clearInv(bool);
	void		removeSpecificItem(int type);

	void		handlePlayerDeath(void);
	void		frameUpdate(void);

	int			addCommand(char *string,qboolean internal=false);
	void		stockWeapons(void);

	int			selectWeapon(int type);
	void		selectWeaponSlot(int weaponNum);
	void		selectBestWeapon(void);
	void		takeOutWeapon(int type);
	void		takeOutWeaponSlot(int weaponNum);
	void		takeOutBestWeapon(void);

	int			addAmmoByGunType(int type, int amount);
	int			stripAmmoFromGun(int type, int amount);
	int			addAmmoType(int type, int amount);
	int			addWeaponType(int type, int fullClip = 0);
	int			getEncumbrance(void);
	int			getEncumbranceByType(int weapon);
	int			addItem(int type, int amount, int maxAmount = -1);//{return items.addItemType(type, amount, maxAmount);}
	int			addArmor(int amount){if((armor==100&&amount>=0)||(armor==0&&amount<=0))return 0;armor += amount;if(armor>100)armor=100;else if(armor<0)armor=0;return armor;} // Biessman sez, "max of 100"

	int			hasWeaponType(int type){return (weaponsOwned & (1<<type));}
	int			hasItemType(int type){return items.hasItemType(type);}
	int			getCurWeaponType(void){return curWeapon()->getType();}

	int			getReadiedWeaponType(void){return readiedWeapon()->getType();}

	int			getBestWeaponType(void);
	int			getCurItemType(void){return items.getCurType();}
	int			getCurItemAmount(void){return items.getCurAmmo();}
	int			getCurAmmo(void){return ammo[curInfo()->getAmmoType(0)];}
	int			getReadiedAmmo(void){return ammo[readiedInfo()->getAmmoType(0)];}
	int			getCurClip(void){return curWeapon()->getClip();}
	int			getClipMax(void){return curInfo()->getClipSize();}
	int			getClipMaxByType(int weapon);
	int			adjustDamageByArmor(int initDamage, float penetrate, float absorb);
	int			getArmorCount(void){return armor;}

	int			scopeIsActive(void){return wAnim.getInZoomView();}
	void		setBloodyWeapon(int isBloody);
	int			inDisguise(void){return wAnim.disguised();}
	void		becomeDisguised(void);

	int			rulesDoWeaponsUseAmmo(void);
	void		rulesSetWeaponsUseAmmo(int useAmmo);
	int			rulesCanDropWeapons(void);
	void		rulesSetDropWeapons(int dropWeapons);
	int			rulesGetNonDroppableWeaponTypes(void);
	void		rulesSetNonDroppableWeaponTypes(int nonDropMask);
	int			rulesDoWeaponsReload(void);
	void		rulesSetWeaponsReload(int weaponsReload);
	int			rulesIsWeaponReloadAutomatic(void);
	void		rulesSetWeaponReloadAutomatic(int reloadAutomatic);
	int			rulesDoReloadEmptiesWeapon(void);
	void		rulesSetReloadEmptiesWeapon(int reloadEmptiesWeapon);
	char		*rulesGetBestWeaponMode(void);
	void		rulesSetBestWeaponMode(char *bestWeaponMode);
	int			rulesCanDropInvenOnDeath(void);
	void		rulesSetDropInvenOnDeath(int dropInvenOnDeath);
	int			rulesCanSelectWithNoAmmo(void);
	void		rulesSetSelectWithNoAmmo(int selectWithNoAmmo);
	int			rulesCanFreelySelectWeapon(void);
	void		rulesSetFreelySelectWeapon(int freelySelectWeapon);

	int			removeCurrentWeapon(void);
	int			countWeapons(void);
	void		setOwner(void *newOwner){owner = (sharedEdict_t *)newOwner;}
	void		dropAllWeapons(void);

	void		handleFall(void){curInfo()->handleFall(*owner);}
	
	int			extractInvFromMenu(int *weaponsAvailable, int *health);
	void		buildInvForMenu(int includeClipAmmo, int health);

	int			extractInvFromString(int *weaponsAvailable,char *invString);
	void		buildInvString(char *string,qboolean xtractWeapons,qboolean xtractAmmo,qboolean xtractItems,qboolean xtractArmor);

	void		clientClearRICs(void);
	void		clientReadRICs(void);
	void		clientProcessRICs(void);
	void		serverClearRICs(void);
	void		serverWriteRICs(void);

	void		setClientPredicting(int predicting){clientPredicting=predicting;}
	int			isClientPredicting(void){return(clientPredicting);}

	// ---------------
	// Everything else
	// ---------------
	// IMPORTANT! Do not call these functions outside of the player DLL! Doing
	// so will totally screw any chance of the weapon / inventory system being
	// portable to the client.
	// ---------------

	// general weapon stuff

	weapon_c	*getCurWeapon(void){return curWeapon();}//legacy... sigh...
	int			getCurWeaponID(void) {return curWeaponID;}

	weapon_c	*getReadiedWeapon(void){return readiedWeapon();}//legacy... sigh...
	int			getReadiedWeaponID(void) {return readiedWeaponID;}

	item_c		*getItems(void) {return &items;}
	void		addWeaponToOwnList(int weapon){weaponsOwned |= (1<<weapon);}
	void		removeWeaponFromOwnList(int weapon){weaponsOwned &= ~(1<<weapon);}
	void		setPendingChange(void){pendingChange = 1;}
	void		deactivateCurrentWeapon(void);
	void		deactivateInventory(void);

	sharedEdict_t	*getOwner(void){return owner;}

	// input stuff

	void		setInputStatus(int i){canHandleInput = i;}
	int			getInputStatus(void) {return canHandleInput;}
	int			checkCommandInput(void);
	int			pendingCommand(void){return (nextCommand != PWC_NOCOMMAND);};
	void		SelectPendingWeapon(void);
	void		readyNextWeapon(void);
	void		SelectRediedWeapon(void);
	void		selectNULLWeapon(void);

	// viewmodel stuff

	IGhoulInst	*getViewModel(void){return model;}
	void		setViewModel(IGhoulInst *newInst){model = newInst;}
	char		*getViewModelName(void){return curWeapon()->getWeaponInfo()->getWeaponName();}

	IGhoulInst	*getReadyModel(void){return readyModel;}
	void		setReadyModel(IGhoulInst *newInst){readyModel = newInst;}
	char		*getReadyModelName(void){return readiedWeapon()->getWeaponInfo()->getWeaponName();}

	void		setGunEnd(Matrix4 &source){gunEnd = source;}//fixme
	void		getGunEnd(vec3_t worldSpot);

	// ammo stuff

	int			refillCurClip(int curSlot, int ignoreSingle = 0, int noFillForSingle = 1);
	int			hasAmmo(int type){ assert(type >= 0); assert(type < MAXAMMOS); return (ammo[type] != 0);}
	int			getAmmoAmount(int type){assert(type >= 0); assert(type < MAXAMMOS); return(ammo[type]);}
	void		setAmmoAmount(int type, int newVal){assert(type >= 0); assert(type < MAXAMMOS); ammo[type] = newVal;}
	void		setCurClip(int val){curWeapon()->setClip(val);}

	// armor stuff

	void		setArmor(int amount){armor = amount;}

	// attack funcs

	int			fire();		// return value indicates whether the weapon can actually fire at this point
	int			altFire();	// ditto
	int			reload();
	int			clientReload();
	int			putAway();
	int			takeOut();
	int			idle(void);
	void		idleReadied(void);
	int			dryfire(void);
	int			loseWeapon(void);

	wAnim_c		*getAnimInfo(void){return &wAnim;}

	weaponInfo_c *curInfo(void);
	void		setWInfo(int newType){curWeaponType = newType;}
	weapon_c	*curWeapon(void);
	void		setCurWeapon(int slot){curWeaponSlot = slot;}

	weaponInfo_c *readiedInfo(void);
	void		setReadiedWInfo(int newType){readiedWeaponType = newType;}
	weapon_c	*readiedWeapon(void);
	void		setReadiedWeapon(int slot){readiedWeaponSlot = slot;}

	weapon_c	*droppingWeapon(void){if(droppingSlot== -1)return 0; return &weapons[droppingSlot];}
	void		setDroppingWeapon(int slot){droppingSlot = slot;}
	int			getCurSlot(void){return curWeaponSlot;}
	int			getBestSlot(int safe);

		   		inven_c(inven_c *orig);
	void		Evaluate(inven_c *orig);
	void		Write(void);
	void		Read(void);
	void		NetRead(struct sizebuf_s *net_message_ptr);
	void		NetWrite(int clientnum);
	void		getServerState(void);
	void		setServerState(void);
};

//------------------------------------------------------------------------
//
// Callback Declarations
//
//------------------------------------------------------------------------

class WeapSoundCallBack:public IGhoulCallBack
{
public:

	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

class GunFireCallBack:public IGhoulCallBack
{
public:

	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

class AltfireCallBack:public IGhoulCallBack
{
public:

	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

class EOSCallBack:public IGhoulCallBack
{
public:

	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

class KnifeCleanCallBack:public IGhoulCallBack
{
public:

	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

class MinimiReloadCallBack:public IGhoulCallBack
{
public:

	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

class EffectCallBack:public IGhoulCallBack
{
public:

	virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data);
};

//------------------------------------------------------------------------
//
// External Callbacks
//
//------------------------------------------------------------------------

extern WeapSoundCallBack	theWeapSoundCallback;
extern GunFireCallBack		theGunFireCallback;
extern AltfireCallBack		theAltfireCallback;
extern EOSCallBack			theEOSCallback;
extern KnifeCleanCallBack	theKnifeCleanCallback;
extern MinimiReloadCallBack theMinimiReloadCallback;
extern EffectCallBack		theEffectCallback;

//------------------------------------------------------------------------
//
// Inherited Weapon Definitions
//
//------------------------------------------------------------------------

class knifeInfo : public weaponInfo_c
{
private:
public:

	knifeInfo(weaponInfoExtra_t *w);

	char	*getFireSeq(sharedEdict_t &ent);
	char	*getIdleSeq(sharedEdict_t &ent);
	char	*getAltFireSeq(sharedEdict_t &ent);
	char	*getReloadSeq(sharedEdict_t &ent);
	char	*getTakeOutSeq(sharedEdict_t &ent);
	char	*getPutAwaySeq(sharedEdict_t &ent);

	char	*getName(void){return "Knife";}

	int		useAmmo1(sharedEdict_t &ent, int *skipIdle = 0){return 1;}
	int		useAmmo2(sharedEdict_t &ent, int *skipIdle = 0);

	int		handleEOS(sharedEdict_t &ent);
	void	handleFireLoop(sharedEdict_t &ent);
	void	handleAltfireLoop(sharedEdict_t &ent);
	int		checkReloadAtEOS(sharedEdict_t &ent){return 0;}//this is silly.  No.

	void	handleSpecialNotes(char *string, sharedEdict_t &ent);

	char	*getSlashLeft(void);
	char	*getSlashRight(void);

	int		canSelectWithNoAmmo(void){return 1;}

	void	setBloody(sharedEdict_t *self);

	int		getType(void){return SFW_KNIFE;}

	int		hasAltFire(void){return 1;}
	void	precacheResources(void);

	int		testInput(sharedEdict_t &ent);
};

class pistol2Info : public weaponInfo_c
{
private:
public:

	pistol2Info(weaponInfoExtra_t *w);

	char	*getFireSeq(sharedEdict_t &ent);
	char	*getIdleSeq(sharedEdict_t &ent);
	char	*getAltFireSeq(sharedEdict_t &ent);
	char	*getReloadSeq(sharedEdict_t &ent);
	char	*getTakeOutSeq(sharedEdict_t &ent);
	char	*getPutAwaySeq(sharedEdict_t &ent);
	char	*getDryfireSeq(sharedEdict_t &ent);

	char	*getName(void){return "Desert Eagle";}

	int		testInput(sharedEdict_t &ent);

	int		getType(void){return SFW_PISTOL2;}

	int		canSelectWithNoAmmo(void){return 0;}
	void	precacheResources(void);
};

class pistol1Info: public weaponInfo_c
{
private:
public:

	pistol1Info(weaponInfoExtra_t *w);

	char	*getFireSeq(sharedEdict_t &ent);
	char	*getIdleSeq(sharedEdict_t &ent);
	char	*getAltFireSeq(sharedEdict_t &ent);
	char	*getReloadSeq(sharedEdict_t &ent);
	char	*getTakeOutSeq(sharedEdict_t &ent);
	char	*getPutAwaySeq(sharedEdict_t &ent);
	char	*getDryfireSeq(sharedEdict_t &ent);

	char	*getName(void){return "Glock 9mm";}

	int		handleEOS(sharedEdict_t &ent);
	void	handleAltfireLoop(sharedEdict_t &ent);

	int		getType(void){return SFW_PISTOL1;}

	int		atMomentOfFire(sharedEdict_t &ent);
	int		testInput(sharedEdict_t &ent);
	void	precacheResources(void);
};

class machinePistolInfo : public weaponInfo_c
{
private:
public:

	machinePistolInfo(weaponInfoExtra_t *w);

	char	*getFireSeq(sharedEdict_t &ent);
	char	*getIdleSeq(sharedEdict_t &ent);
	char	*getAltFireSeq(sharedEdict_t &ent);
	char	*getReloadSeq(sharedEdict_t &ent);
	char	*getTakeOutSeq(sharedEdict_t &ent);
	char	*getPutAwaySeq(sharedEdict_t &ent);
	char	*getDryfireSeq(sharedEdict_t &ent);

	char	*getName(void){return "Ingram m11";}

	int		handleEOS(sharedEdict_t &ent);
	void	handleFireLoop(sharedEdict_t &ent);
	int		atMomentOfFire(sharedEdict_t &ent);

	int		getType(void){return SFW_MACHINEPISTOL;}

	int		testInput(sharedEdict_t &ent);
	void	precacheResources(void);

	IGhoulInst::EndCondition getLoopType(void){return IGhoulInst::Hold;}
};

class assaultRifleInfo : public weaponInfo_c
{
private:
public:

	assaultRifleInfo(weaponInfoExtra_t *w);

	char	*getFireSeq(sharedEdict_t &ent);
	char	*getIdleSeq(sharedEdict_t &ent);
	char	*getAltFireSeq(sharedEdict_t &ent);
	char	*getReloadSeq(sharedEdict_t &ent);
	char	*getTakeOutSeq(sharedEdict_t &ent);
	char	*getPutAwaySeq(sharedEdict_t &ent);
	char	*getDryfireSeq(sharedEdict_t &ent);

	char	*getName(void){return "HK53";}

	int		getType(void){return SFW_ASSAULTRIFLE;}

	int		atMomentOfFire(sharedEdict_t &ent);

	int		useAmmo1(sharedEdict_t &ent, int *skipIdle = 0);
	void	turnOffAmmoParts(sharedEdict_t &ent,int clipLeft,qboolean forceAll=false);

	void	handleSpecialNotes(char *string, sharedEdict_t &ent);

	int		testInput(sharedEdict_t &ent);
	void	precacheResources(void);

	IGhoulInst::EndCondition getLoopType(void){return IGhoulInst::Hold;}
};

class sniperInfo : public weaponInfo_c
{
private:
public:

	sniperInfo(weaponInfoExtra_t *w);

	void	shutdownWeapon(sharedEdict_t &ent);

	char	*getFireSeq(sharedEdict_t &ent);
	char	*getIdleSeq(sharedEdict_t &ent);
	char	*getAltFireSeq(sharedEdict_t &ent);
	char	*getReloadSeq(sharedEdict_t &ent);
	char	*getTakeOutSeq(sharedEdict_t &ent);
	char	*getPutAwaySeq(sharedEdict_t &ent);
	char	*getDryfireSeq(sharedEdict_t &ent);

	char	*getName(void){return "Sig2000G";}

	int		handleEOS(sharedEdict_t &ent);
	void	handleFireLoop(sharedEdict_t &ent);
	void	handleAltfireLoop(sharedEdict_t &ent);

	int		testInput(sharedEdict_t &ent);

	int		getType(void){return SFW_SNIPER;}

	int		hasAltFire(void){return 1;}

	void	precacheResources(void);
};

class autoshotgunInfo : public weaponInfo_c
{
private:
public:

	autoshotgunInfo(weaponInfoExtra_t *w);

	char	*getFireSeq(sharedEdict_t &ent);
	char	*getIdleSeq(sharedEdict_t &ent);
	char	*getAltFireSeq(sharedEdict_t &ent);
	char	*getReloadSeq(sharedEdict_t &ent);
	char	*getTakeOutSeq(sharedEdict_t &ent);
	char	*getPutAwaySeq(sharedEdict_t &ent);
	char	*getDryfireSeq(sharedEdict_t &ent);

	char	*getName(void){return "Jackhammer";}
	GhoulSpeed	getFireSpeed(void){return gsOne;}

	int		useAmmo2(sharedEdict_t &ent, int *skipIdle = 0);

	int		handleEOS(sharedEdict_t &ent);

	int		getType(void){return SFW_AUTOSHOTGUN;}

	int		hasAltFire(void){return 1;}
	int		fireFromFlash(void){return 1;}
	void	precacheResources(void);
};

class shotgunInfo : public weaponInfo_c
{
private:
public:

	shotgunInfo(weaponInfoExtra_t *w);

	char	*getFireSeq(sharedEdict_t &ent);
	char	*getIdleSeq(sharedEdict_t &ent);
	char	*getAltFireSeq(sharedEdict_t &ent);
	char	*getReloadSeq(sharedEdict_t &ent);
	char	*getTakeOutSeq(sharedEdict_t &ent);
	char	*getPutAwaySeq(sharedEdict_t &ent);
	char	*getDryfireSeq(sharedEdict_t &ent);

	char	*getName(void){return "Spas15";}
	int		useAmmo1(sharedEdict_t &ent, int *skipIdle =0);

	int		isSingleReload(void){return 1;}
	int		handleEOS(sharedEdict_t &ent);

	int		getType(void){return SFW_SHOTGUN;}

	int		testInput(sharedEdict_t &ent);
	int		testReloadAtInit(sharedEdict_t &ent){return 1;}
	void	precacheResources(void);
};

class machinegunInfo : public weaponInfo_c
{
private:
public:

	machinegunInfo(weaponInfoExtra_t *w);

	char	*getFireSeq(sharedEdict_t &ent);
	char	*getIdleSeq(sharedEdict_t &ent);
	char	*getAltFireSeq(sharedEdict_t &ent);
	char	*getReloadSeq(sharedEdict_t &ent);
	char	*getTakeOutSeq(sharedEdict_t &ent);
	char	*getPutAwaySeq(sharedEdict_t &ent);
	char	*getDryfireSeq(sharedEdict_t &ent);

	char	*getName(void){return "Minimi";}
	GhoulSpeed	getFireSpeed(void){return gsOne;}

	int		handleEOS(sharedEdict_t &ent);
	int		useAmmo1(sharedEdict_t &ent, int *skipIdle = 0);
	int		useAmmo2(sharedEdict_t &ent, int *skipIdle = 0);
	void	turnOffAmmoParts(sharedEdict_t &ent,int clipLeft,qboolean forceAll=false);

	void	handleSpecialNotes(char *string, sharedEdict_t &ent);

	void	handleFall(sharedEdict_t &ent);

	int		getType(void){return SFW_MACHINEGUN;}

	int		atMomentOfFire(sharedEdict_t &ent);
	int		testInput(sharedEdict_t &ent);

	int		hasAltFire(void){return 1;}
	void	precacheResources(void);

	IGhoulInst::EndCondition getLoopType(void){return IGhoulInst::Hold;}
};

class rocketInfo : public weaponInfo_c
{
private:
public:

	rocketInfo(weaponInfoExtra_t *w);

	char	*getFireSeq(sharedEdict_t &ent);
	char	*getIdleSeq(sharedEdict_t &ent);
	char	*getAltFireSeq(sharedEdict_t &ent);
	char	*getReloadSeq(sharedEdict_t &ent);
	char	*getTakeOutSeq(sharedEdict_t &ent);
	char	*getPutAwaySeq(sharedEdict_t &ent);
	char	*getDryfireSeq(sharedEdict_t &ent);

	char	*getName(void){return "M202A2";}

	int		getType(void){return SFW_ROCKET;}

	int		hasAltFire(void){return 1;}
	void	handleFireLoop(sharedEdict_t &ent);
	int		atMomentOfFire(sharedEdict_t &ent);

	int		fireFromFlash(void){return 1;}
	void	precacheResources(void);
};

class microInfo : public weaponInfo_c
{
private:
public:

	microInfo(weaponInfoExtra_t *w);

	char	*getFireSeq(sharedEdict_t &ent);
	char	*getIdleSeq(sharedEdict_t &ent);
	char	*getAltFireSeq(sharedEdict_t &ent);
	char	*getReloadSeq(sharedEdict_t &ent);
	char	*getTakeOutSeq(sharedEdict_t &ent);
	char	*getPutAwaySeq(sharedEdict_t &ent);
	char	*getDryfireSeq(sharedEdict_t &ent);

	char	*getName(void){return "Microwave Pulse";}

	int		getType(void){return SFW_MICROWAVEPULSE;}

	int		hasAltFire(void){return 1;}

	int		useAmmo2(sharedEdict_t &ent, int *skipIdle = 0);

	int		handleEOS(sharedEdict_t &ent);
	int		testInput(sharedEdict_t &ent);

	int		fireFromFlash(void){return 1;}
	qboolean aimAtCrosshair(void){return false;}
	void	precacheResources(void);
};

class flamegunInfo : public weaponInfo_c
{
private:
public:

	flamegunInfo(weaponInfoExtra_t *w);

	void	shutdownWeapon(sharedEdict_t &ent);
	void	noattack(sharedEdict_t &ent);

	char	*getFireSeq(sharedEdict_t &ent);
	char	*getIdleSeq(sharedEdict_t &ent);
	char	*getAltFireSeq(sharedEdict_t &ent);
	char	*getReloadSeq(sharedEdict_t &ent);
	char	*getTakeOutSeq(sharedEdict_t &ent);
	char	*getPutAwaySeq(sharedEdict_t &ent);
	char	*getDryfireSeq(sharedEdict_t &ent);

	char	*getName(void){return "Flamethrower";}

	int		useAmmo1(sharedEdict_t &ent, int *skipIdle = 0);
	int		useAmmo2(sharedEdict_t &ent, int *skipIdle = 0);

	int		handleEOS(sharedEdict_t &ent);
	void	handleFireLoop(sharedEdict_t &ent);
	void	handleAltfireLoop(sharedEdict_t &ent);
	int		checkReloadAtEOS(sharedEdict_t &ent){return 0;}

	int		testInput(sharedEdict_t &ent);

	int		getType(void){return SFW_FLAMEGUN;}

	int		hasAltFire(void){return 1;}
	int		fireFromFlash(void){return 1;}
	qboolean aimAtCrosshair(void){return false;}
	void	precacheResources(void);

//	void	frameUpdate(sharedEdict_t &ent);
};


class hurthandInfo : public weaponInfo_c
{
private:
public:

	hurthandInfo(weaponInfoExtra_t *w);

	char	*getIdleSeq(sharedEdict_t &ent);
	char	*getTakeOutSeq(sharedEdict_t &ent);
	char	*getName(void){return "Hurthand";}
	int		getType(void){return SFW_HURTHAND;}
	int		handleEOS(sharedEdict_t &ent);
};

class throwhandInfo : public weaponInfo_c
{
private:
public:

	throwhandInfo(weaponInfoExtra_t *w);

	char	*getIdleSeq(sharedEdict_t &ent);
	char	*getTakeOutSeq(sharedEdict_t &ent);
	char	*getName(void){return "Throwhand";}
	int		getType(void){return SFW_THROWHAND;}
	int		handleEOS(sharedEdict_t &ent);
};

//------------------------------------------------------------------------
//
// sharedEdict_s
//
//------------------------------------------------------------------------

struct sharedEdict_s
{
	inven_c *inv;
	edict_t *edict;
	int		attack;
	int		altattack;
	int		weap3;
	int		weap4;
	int		leanLeft;
	int		leanRight;
	int		cinematicFreeze;
	int		rejectSniper;
	float	*weaponkick_angles;	// Really, a vec3_t.
	float	framescale;
	int		doReload;
};

//------------------------------------------------------------------------
//
// Weapon inline access type funcs...
//
//------------------------------------------------------------------------

_inline inven_c *inven(sharedEdict_t &ent)
{
	return ent.inv;
}

_inline wAnim_c *wAnim(sharedEdict_t &ent)
{
	return inven(ent)->getAnimInfo();
}

//------------------------------------------------------------------------
//
// Misc
//
//------------------------------------------------------------------------

void W_PrecacheViewWeaponModels(int types);
void W_UncacheViewWeaponModels(void);

extern weaponInfo_c *weapInfo[SFW_NUM_WEAPONS];

#endif // __W_WEAPONS_H
