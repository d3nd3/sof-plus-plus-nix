#include "q_shared.h"
#include "..\ghoul\ighoul.h"
#include "w_public.h"
#include "w_types.h"
#include "w_weapons.h"
#include "w_utils.h"
#include "w_network.h"
#include "player.h"
#include "../gamecpp/game.h"

extern player_com_import_t	picom;
extern player_cl_import_t	picl;
extern player_sv_import_t	pisv;

extern int isClient;

int fireEvent;
int	altfireEvent;

void inven_c::getServerState(void)
{
	// Clip. 
	
	curWeapon()->setClip(owner->edict->client->ps.gunClip);

	// Ammo pool.

	weapon_c *weaponToFill=&weapons[curWeaponSlot];
	int type = weaponToFill->getWeaponInfo()->getAmmoType(0);
	ammo[type]=owner->edict->client->ps.gunAmmo;
}

void inven_c::setServerState(void)
{
	// Clip.
	
	owner->edict->client->ps.gunClip=curWeapon()->getClip();

	// Ammo pool.

	owner->edict->client->ps.gunAmmo=getCurAmmo();
}

inven_c::inven_c(void)
{
	canHandleInput = 1;
	setCurWeapon(0);
	setReadiedWeapon(0);
	setWInfo(1);
	setDroppingWeapon(-1);
	model = 0;
	readyModel = 0;
	memset(&gunEnd, 0, sizeof(Matrix4));	// Should really be gunEnd.Zero()
	wAnim.setHandsInUse(2);
	wAnim.setWeaponLifted(0);
	wAnim.setTransition(1);
	wAnim.setBloody(0);
	wAnim.setHandSide(0);
	wAnim.setDown(0);
	wAnim.setFirstShot(1);
	wAnim.setInZoom(0);
	wAnim.setInZoomView(0);
	wAnim.setInteresting(0);
	wAnim.setRezoom(0);
	wAnim.setKnifeStatus(0);
	wAnim.setDisguise(0);
	wAnim.setlastShootTime(0);
	nextCommand = PWC_NOCOMMAND;
	clientOverrideCmd=0;
	newInstRxd=0;
	rulesWeaponsUseAmmo=-1;
	rulesDropWeapons=-1;
	rulesNonDroppableWeaponTypes=(1<<SFW_HURTHAND)|(1<<SFW_THROWHAND)|(1<<SFW_EMPTYSLOT);
	rulesWeaponsReload=-1;
	rulesWeaponReloadAutomatic=-1;
	rulesDropInvenOnDeath=0;
	rulesSelectWithNoAmmo=0;
	rulesReloadEmptiesWeapon=0;
	strcpy(rulesBestWeaponMode,"safe");
	rulesFreelySelectWeapon=-1;
	selectedWeaponSlot = -1;
	armor = 0;
	weaponsOwned = 0;
	wAnim.clearFireLoop();
	nextSelectSlot = 0;
	curWeaponID = 0;
	readiedWeaponID = 0;
	readiedWeaponType = 0;
	pendingChange = 0;
	clientPredicting =0;
	wAnim.setAnimType(WANIM_NORMAL);
	wAnim.setSeqName("unused viewmodel");
	serverRICBuffer=NULL;
	clientRICBuffer=NULL;

	if(!isClient)
	{
		serverRICBuffer=new serverRICBuf;
		serverRICBuffer->setOwner(this);
	}
	else
	{
		clientRICBuffer=new clientRICBuf;
		clientRICBuffer->setOwner(this);
	}

	int i;

	if(isClient)
	{
		// Client always has all the weapons available.

		for(i=SFW_KNIFE;i<SFW_NUM_WEAPONS;i++)
			addWeaponType(i,0);
	}
	else
	{
		for(i=0;i<MAXWEAPONS;i++)
			weapons[i].setType(SFW_EMPTYSLOT);
	}

	for(i=0;i<MAXITEMS;i++)
		items.clearSlot(i);

	for(i=0;i<MAXAMMOS;i++)
		ammo[i]=0;

}

inven_c::~inven_c(void)
{
	if(serverRICBuffer)
		delete serverRICBuffer;
	
	if(clientRICBuffer)
		delete clientRICBuffer;
}

void inven_c::getGunEnd(vec3_t worldSpot)
{
/*
#if 1
	VectorAdd(owner->edict->s.origin, owner->edict->client->ps.viewoffset, worldSpot);
#else
	Matrix4 etow,null2world;
	Vect3 NullPos;
	vec3_t pos;

	gunEnd.GetRow(3, NullPos);
	VectorAdd(owner->s.origin, owner->client->ps.viewoffset, pos);

	EntToWorldMatrix(pos,owner->client->ps.viewangles,etow);
	null2world.Concat(gunEnd,etow);

	Vect3 curPos(0,0,0);
	null2world.XFormPoint(NullPos,curPos);

	worldSpot[0] = NullPos.x();
	worldSpot[1] = NullPos.y();
	worldSpot[2] = NullPos.z();
#endif
*/
}

void inven_c::initNewWeapon(void)
{
	if(!isClient)
	{
		owner->edict->client->ps.gunType=curWeaponID;
		owner->edict->client->ps.gunClip=curWeapon()->getClip();
		owner->edict->client->ps.gunAmmo=getCurAmmo();
	}

	SetVWeapGhoul(owner, getViewModelName());

	if(!wAnim.disguised())
		idle();
	
	if(rulesDoWeaponsUseAmmo())
	{
		if(!isClient)
			curWeapon()->getWeaponInfo()->turnOffAmmoParts(*owner,curWeapon()->getClip(),true);
		else
			curWeapon()->getWeaponInfo()->turnOffAmmoParts(*owner,owner->edict->client->ps.gunClip,true);
	}
}

int inven_c::addWeaponType(int weapon, int fullClip)
{
	int i;

	// See if there already is one in our inventory.

	for(i = 0; i < MAXWEAPONS; i++)
	{	
		if(weapons[i].getType() == weapon)
		{
			return 0;
		}
	}

	// Ok, don't have one, so add it to the correct slot.

	static int weaponBinds[]=
	{
		SFW_KNIFE,
		SFW_PISTOL1,
		SFW_PISTOL2,
		SFW_SHOTGUN,
		SFW_SNIPER,
		SFW_MACHINEPISTOL,
		SFW_ASSAULTRIFLE,
		SFW_MACHINEGUN,
		SFW_AUTOSHOTGUN,
		SFW_ROCKET,
		SFW_FLAMEGUN,
		SFW_MICROWAVEPULSE,
		SFW_HURTHAND,
		SFW_THROWHAND,
		SFW_NUM_WEAPONS
	};

	i=0;

	while(weaponBinds[i]!=SFW_NUM_WEAPONS)
	{
		if(weaponBinds[i]==weapon)
		{
			if(!rulesDoWeaponsUseAmmo())
				fullClip=0;

			weapons[i].setType(weapon, fullClip);
			addWeaponToOwnList(weapon);
			return(1);
		}

		i++;
	}
	
	return(0);
}

int	inven_c::getEncumbrance(void)
{
	int encumbrance=0;

	for(int i=0; i<MAXSELECTABLEWEAPONS; i++)
		if(weapons[i].getType()!=SFW_EMPTYSLOT)
			encumbrance+=weapons[i].getWeaponInfo()->getSlotsNeeded();

	return(encumbrance);
}

int	inven_c::getEncumbranceByType(int weapon)
{
	return(weapInfo[weapon]->getSlotsNeeded());
}

int	inven_c::addItem(int type, int amount, int maxAmount/*= -1*/)
{
	bool	bWasEmpty = items.getCurSlot()?(items.getCurSlot()->getSlotType()==SFE_EMPTYSLOT):true;
	int		nRet = items.addItemType(type, amount, maxAmount);

	if(bWasEmpty && !!nRet)
		items.setNextCommand(PEC_ITEMNEXT);

	return nRet;
}

void inven_c::readyNextWeapon(void)
{
	if(nextSelectSlot == -1)
		return;

	readiedWeaponID = weapons[nextSelectSlot].getType();
	setReadiedWeapon(nextSelectSlot);
	setReadiedWInfo(readiedWeapon()->getWeaponInfo()->getType());
	readiedWeaponSlot = nextSelectSlot;

	ReadyVWeapGhoulServer(owner, getReadyModelName());

	// Needed to 'kick-start' the new inst to the client straight-away.

	idleReadied();

	// Needed by client.

	owner->edict->client->ps.gunType=readiedWeaponID;
	owner->edict->client->ps.gunClip=readiedWeapon()->getClip();
	owner->edict->client->ps.gunAmmo=getReadiedAmmo();
}

void inven_c::SelectRediedWeapon(void)
{
	SetReadiedVWeapGhoulServer(owner,"");

	curWeaponID = readiedWeaponID;
 	setCurWeapon(readiedWeaponSlot);
	setWInfo(readiedWeapon()->getWeaponInfo()->getType());
	selectedWeaponSlot = readiedWeaponSlot;
	readiedWeaponID=0;
}

void inven_c::selectNULLWeapon(void)
{
	if(!isClient)
	{
		owner->edict->client->ps.gun=0;
		owner->edict->client->ps.gunType=0;
		owner->edict->client->ps.gunClip=0;
		owner->edict->client->ps.gunAmmo=0;
	}

	setCurWeapon(0);
	setReadiedWeapon(0);
	setWInfo(1);
	model = 0;
	readyModel = 0;
	selectedWeaponSlot = -1;
	wAnim.clearFireLoop();
	nextSelectSlot = 0;
	curWeaponID = 0;
	readiedWeaponID = 0;
	readiedWeaponType = 0;
	pendingChange = 0;
	wAnim.setAnimType(WANIM_NORMAL);
	wAnim.setSeqName("unused viewmodel");
}

void inven_c::SelectPendingWeapon(void)
{
	if(nextSelectSlot == -1)return;

	curWeaponID = weapons[nextSelectSlot].getType();
	setCurWeapon(nextSelectSlot);
	setWInfo(curWeapon()->getWeaponInfo()->getType());
	selectedWeaponSlot = nextSelectSlot;

	initNewWeapon();
}

int inven_c::selectWeapon(int weapon)
{
	int i;

	for(i = 0; i < MAXSELECTABLEWEAPONS; i++)
	{
		if(weapons[i].getType() == weapon)
		{
			curWeaponID = weapon;
			setCurWeapon(i);
			setWInfo(weapons[i].getWeaponInfo()->getType());
			selectedWeaponSlot = i;

			initNewWeapon();
			return 1;
		}
	}
	return 0;
}

void inven_c::selectWeaponSlot(int weaponNum)
{
	curWeaponID = weapons[weaponNum].getType();
	setCurWeapon(weaponNum);
	setWInfo(curWeaponID);
	selectedWeaponSlot = weaponNum;

	initNewWeapon();
}

void inven_c::selectBestWeapon(void)
{
	selectWeaponSlot(getBestSlot(!strcmp(rulesGetBestWeaponMode(),"safe")));
}

void inven_c::takeOutWeapon(int type)
{
	for(int i=0; i<MAXSELECTABLEWEAPONS; i++)
	{
		if(weapons[i].getType()==type)
		{
			nextCommand=PWC_WEAPSELECTFIRST+i;
			break;
		}
	}
}

void inven_c::takeOutWeaponSlot(int weaponNum)
{
	nextCommand=PWC_WEAPSELECTFIRST+weaponNum;
}

void inven_c::takeOutBestWeapon(void)
{
	nextCommand=PWC_WEAPSELECTFIRST+getBestSlot(!strcmp(rulesGetBestWeaponMode(),"safe"));
}

//AMMO_NONE = 0,
//AMMO_KNIFE,
//AMMO_44,
//AMMO_9MM,
//AMMO_SHELLS,
//AMMO_556,
//AMMO_ROCKET,
//AMMO_MWAVE,
//AMMO_FTHROWER,
//AMMO_SLUG,

int ammoMaxes[] = 
{
	  0,	
	  6,	
	 90,	
	200,	
	 50,	
	300,	
	 20,	
	150,	
	 60,	
	100,	
};

int inven_c::addAmmoType(int type, int amount)
{
	if(type==AMMO_FTHROWER)
	{
		// Flamegun is special 'cos it doesn't work on clip & ammo pool model.
		// Just has a clip.

		int curClip=weapons[10].getClip();

		if(curClip >= ammoMaxes[type])
		{
			// NO PICK UP!!!
	
			return 0;
		}

		if(curClip+amount>ammoMaxes[type])
			amount=ammoMaxes[type] - curClip;

		weapons[10].setClip(curClip+amount);

		return 1;
	}

	if(ammo[type] >= ammoMaxes[type])
	{
		// NO PICK UP!!!

		return 0;
	}

	if(ammo[type] + amount > ammoMaxes[type])
		amount = ammoMaxes[type] - ammo[type];

	ammo[type] += amount;

	return 1;
}

int ammoMinPickup[] = 
{
	0,	//AMMO_NONE = 0,
	1,	//AMMO_KNIFE,
	4,	//AMMO_44,
	5,	//AMMO_9MM,
	2,	//AMMO_SHELLS,
	15, //AMMO_556,
	1,	//AMMO_ROCKET,
	5,	//AMMO_MWAVE,
	5,	//AMMO_FTHROWER,
	5,	//AMMO_SLUG,
	8,	//special 9mm machine pistol ammo AMMO_MACHPIS9MM
};

int ammoMaxPickup[] = 
{
	0,	//AMMO_NONE = 0,
	1,	//AMMO_KNIFE,
	9,	//AMMO_44,
	8,	//AMMO_9MM,
	4,	//AMMO_SHELLS,
	20, //AMMO_556,
	3,	//AMMO_ROCKET,
	10,	//AMMO_MWAVE,
	10,	//AMMO_FTHROWER,
	10,	//AMMO_SLUG,
	15, //special 9mm machine pistol ammo AMMO_MACHPIS9MM
};

int inven_c::addAmmoByGunType(int type, int amount)
{
	// Default to one, I guess.

	int ammoType = weapInfo[type]->getAmmoType(0);

	int addedAmount;

	if(amount)
	{
		addedAmount = amount;
	}
	else if(type == SFW_MACHINEPISTOL)
	{
		ammoType = AMMO_MACHPIS9MM; // special amount for machine pistol 9mm
		addedAmount = ClientServerRand(ammoMinPickup[ammoType],ammoMaxPickup[ammoType]);
		return addAmmoType(AMMO_9MM, addedAmount);
	}
	else
	{
		addedAmount = ClientServerRand(ammoMinPickup[ammoType],ammoMaxPickup[ammoType]);
	}

	return addAmmoType(ammoType, addedAmount);
}

int inven_c::stripAmmoFromGun(int type, int amount)
{
	// For single player
	// Default to one, I guess.

	int ammoType = weapInfo[type]->getAmmoType(0);

	return addAmmoType(ammoType, amount);
}

void inven_c::clearInv(bool ctf_remove)
{
	int i;

	for(i=0; i<MAXWEAPONS; i++)
		weapons[i].setType(SFW_EMPTYSLOT);
	
	for(i=0; i<MAXAMMOS; i++)
		ammo[i]=0;
	
	for(i=0; i<MAXITEMS; i++)
	{
		itemSlot_c *curSlot = items.getSlot(i);

		if (ctf_remove || (curSlot->getSlotType() != SFE_CTFFLAG))
		{
			items.clearSlot(i);
		}
	}

	armor=0;
	
	nextCommand=PWC_NOCOMMAND;
}

void inven_c::removeSpecificItem(int type)
{
	int	i;
	for(i=0; i<MAXITEMS; i++)
	{
		itemSlot_c *curSlot = items.getSlot(i);

		if (curSlot->getSlotType() == type)
		{
			items.clearSlot(i);
		}
	}
}


void inven_c::handlePlayerDeath(void)
{	
	if(isClient)
		return;

	deactivateInventory();

	if(curWeaponID!=SFW_EMPTYSLOT)
	{
		// This will stop weapons that are looping their firing sequences.

		idle();

		// Shut down current.

		curInfo()->shutdownWeapon(*owner);
	}
	
	// Drop current weapon(s)? PWC_DROP can only be set if:
	// a) rulesCanDropWeapons() returns non-zero or
	// b) removeCurrentWeapon() has been called.

	if(rulesCanDropWeapons()||(nextCommand==PWC_DROP))
	{
		if(curWeaponID!=SFW_EMPTYSLOT)
		{
			// Ensure we drop any weapon we are in the process of dropping or losing.

			if((nextCommand==PWC_DROP)||(nextCommand==PWC_LOSE))
				setDroppingWeapon(curWeaponSlot);

			// Ensure we drop current weapon if we haven't already started dropping it.

			if(!droppingWeapon())
				setDroppingWeapon(curWeaponSlot);

			// Actually drop it.

			if(!(rulesGetNonDroppableWeaponTypes()&(1<<droppingWeapon()->getType())))
				pisv.WeaponDrop(this->owner->edict,droppingWeapon()->getType(), droppingWeapon()->getClip());

			removeWeaponFromOwnList(droppingWeapon()->getType());
			droppingWeapon()->setType(SFW_EMPTYSLOT);

			setDroppingWeapon(-1);
		}

		// In realistic DM, we need to drop any special weapons in our inventory.

		if(rulesCanDropInvenOnDeath())
		{
			dropAllWeapons();
		}
	}
}

void inven_c::dropAllWeapons(void)
{
	if(isClient)
		return;

	for(int i = 0; i < MAXSELECTABLEWEAPONS; i++)
	{
		if(weapons[i].getType()!=SFW_EMPTYSLOT)
		{
			if(!(rulesGetNonDroppableWeaponTypes()&(1<<weapons[i].getType())))
				pisv.WeaponDrop(this->owner->edict,weapons[i].getType(), weapons[i].getClip());
			
			removeWeaponFromOwnList(weapons[i].getType());
			weapons[i].setType(SFW_EMPTYSLOT);
		}
	}
}

int inven_c::removeCurrentWeapon(void)
{
	if(isClient)
		return(0);

	nextCommand = PWC_DROP;
	return(1);
}

int	inven_c::countWeapons(void)
{
	int i;
	int val = 0;

	for(i = 0; i < MAXSELECTABLEWEAPONS; i++)
	{
		if(weapons[i].getType() != SFW_EMPTYSLOT)val++;
	}

	return(val);
}

void inven_c::deactivateCurrentWeapon(void)
{
	curInfo()->shutdownWeapon(*owner);
}

void inven_c::deactivateInventory(void)
{
	items.deactivate(*owner);
}

int inven_c::refillCurClip(int curSlot, int ignoreSingle, int noFillForSingle)
{
	weapon_c *weaponToFill = &weapons[curSlot];
	int type = weaponToFill->getWeaponInfo()->getAmmoType(0);

	if(type == AMMO_NONE)return 0;
	
	if(ammo[type] == 0)return 0;

	if(weaponToFill->getWeaponInfo()->isSingleReload() && (!ignoreSingle) && rulesDoWeaponsReload())
	{
		if(weaponToFill->getClip() >= weaponToFill->getWeaponInfo()->getClipSize())
		{
			// Already full.

			return 0;
		}

		if(noFillForSingle)
		{
			// Strangeness with shotgun.

			return 1;
		}

		if(ammo[type])
		{
			ammo[type] --;
			weaponToFill->setClip(weaponToFill->getClip() + 1);
		}
	}
	else
	{
		if(rulesDoReloadEmptiesWeapon())
		{
			 // Clip simulator for realistic stuff.

			if(ammo[type] > weaponToFill->getWeaponInfo()->getClipSize())
			{
				ammo[type] -= weaponToFill->getWeaponInfo()->getClipSize();
				weaponToFill->setClip(weaponToFill->getWeaponInfo()->getClipSize());
			}
			else
			{
				weaponToFill->setClip(ammo[type]);
				ammo[type] = 0;
			}
		}
		else if(ammo[type] > weaponToFill->getWeaponInfo()->getClipSize() - weaponToFill->getClip())
		{
			ammo[type] -= weaponToFill->getWeaponInfo()->getClipSize() - weaponToFill->getClip();
			weaponToFill->setClip(weaponToFill->getWeaponInfo()->getClipSize());
		}
		else
		{
			weaponToFill->setClip(weaponToFill->getClip() + ammo[type]);
			ammo[type] = 0;
		}
	}
	return 1;
}

int inven_c::adjustDamageByArmor(int initDamage, float penetrate, float absorb)
{
	if(!armor)
	{
		return initDamage;
	}

	// Take absorbtion into account.

	initDamage -= (initDamage * absorb);

	if(penetrate)
	{
		int extraDmg = initDamage * 2;

		if(armor > extraDmg)
		{
			armor -= extraDmg;
			extraDmg = 0;
		}
		else
		{
			extraDmg -= armor;
			armor = 0;
		}

		// So it just does well against armor, that's all.

		return extraDmg * .5;
	}
	else
	{
		if(armor > initDamage)
		{
			armor -= initDamage;
			initDamage = 0;
		}
		else
		{
			initDamage -= armor;
			armor = 0;
		}
		return initDamage;
	}
	
	return 0;
}

void inven_c::becomeDisguised(void)
{
	if(!isClient)
		serverRICBuffer->AddRIC(RIC_HIDEWEAPONFORDISGUISE,"");

	if(wAnim.disguised())
	{	
		// 'Tis a toggle.

		wAnim.setDisguise(0);
	}
	else
	{
		wAnim.setDisguise(1);
		putAway();

		// In case we pull it right back out.
		
		nextSelectSlot = selectedWeaponSlot;
	}
}

//------------------------------------------------------------------------------------------------------------------------
//													ANIM FUNCS
//------------------------------------------------------------------------------------------------------------------------

int inven_c::fire(void)
{
	char *fireSeq;

	if((fireSeq = curInfo()->getFireSeq(*owner)) == NULL)
		return 0;
	
	if((!curWeapon()->handleAmmo(*owner, 0)))
	{
		if((!isClient)&&isClientPredicting())
			pisv.clearFireEvent(owner->edict);

		if(isClient)
			fireEvent=1;	

		return 1;
	}

	// If ammo is okay, initiate the firing seq.

	setInputStatus(0);
	wAnim.setAnimType(WANIM_FIRE);

	if(!isClient)
	{
		if(isClientPredicting())
		{
			if((curWeaponID==SFW_ASSAULTRIFLE)||(curWeaponID==SFW_MACHINEPISTOL))
				RunVWeapAnim(owner, fireSeq, getCurWeapon()->getWeaponInfo()->getLoopType());
			else
				RunVWeapAnim(owner, fireSeq, getCurWeapon()->getWeaponInfo()->getLoopType(),pisv.getFireEvent(owner->edict));			

			pisv.clearFireEvent(owner->edict);
		}
		else
		{
			RunVWeapAnim(owner, fireSeq, getCurWeapon()->getWeaponInfo()->getLoopType());
		}
	}
	else
	{
		RunVWeapAnim(owner, fireSeq, getCurWeapon()->getWeaponInfo()->getLoopType());
		fireEvent=1;
	}

	return 1;
}

int inven_c::altFire(void)
{
	if(!curInfo()->hasAltFire())
	{
		// Well, not everything can do this.

		return 0;
	}

	char *altFireSeq;

	if((altFireSeq = curInfo()->getAltFireSeq(*owner)) == NULL)
		return 0;

	if(!curWeapon()->handleAmmo(*owner, 1))
	{
		if((!isClient)&&isClientPredicting())
			pisv.clearAltfireEvent(owner->edict);

		if(isClient)
			altfireEvent=1;	

		return 1;
	}

	// If ammo is okay, initiate the altfiring seq.

	setInputStatus(0);
	wAnim.setAnimType(WANIM_ALTFIRE);

	if(!isClient)
	{
		if(isClientPredicting())
		{
			if(curWeaponID==SFW_ROCKET)
				RunVWeapAnim(owner, altFireSeq, IGhoulInst::Hold);
			else
				RunVWeapAnim(owner, altFireSeq, IGhoulInst::Hold,pisv.getAltfireEvent(owner->edict));

			pisv.clearAltfireEvent(owner->edict);
		}
		else
		{
			RunVWeapAnim(owner, altFireSeq, IGhoulInst::Hold);
		}
	}
	else
	{
		RunVWeapAnim(owner, altFireSeq, IGhoulInst::Hold);
		altfireEvent=1;
	}

	return 1;
}

int inven_c::clientReload(void)
{
	fireEvent=0;	
	altfireEvent=0;	

	char *reloadSeq;

	if((reloadSeq = curInfo()->getReloadSeq(*owner)) == NULL)
		return 0;

	// Reloading recenters the weapon.

	wAnim.clearFireLoop();

	if(curInfo()->testReloadAtInit(*owner))
	{
		if(!refillCurClip(curWeaponSlot))
		{	
			dryfire();

			if(!curWeapon()->getClip())
			{
				// Totally out of ammo, so do nothing and wait for server to
				// change our weapon.

				;
			}

			return 0;
		}
	}

	if(rulesDoWeaponsReload())
	{
		if(!isClient)
			pisv.ShowReload(owner->edict);

		setInputStatus(0);
		wAnim.setAnimType(WANIM_RELOAD);
		RunVWeapAnim(owner, reloadSeq, IGhoulInst::Hold);
	}
	else
		idle();

	return 1;
}

int inven_c::reload(void)
{
	if(isClient)
	{
		// Client always told to do this via clientReload().

		return 0;
	}
	
	pisv.clearFireEvent(owner->edict);
	pisv.clearAltfireEvent(owner->edict);

	char *reloadSeq;

	if((curWeapon()->getClip() == curInfo()->getClipSize()) && (!rulesDoReloadEmptiesWeapon()))
	{
		// Already full....

		return 0;
	}

	if((reloadSeq = curInfo()->getReloadSeq(*owner)) == NULL)
		return 0;

	// Reloading recenters the weapon.

	wAnim.clearFireLoop();
	
	if(curInfo()->testReloadAtInit(*owner))
	{
		if(!refillCurClip(curWeaponSlot))
		{	
			dryfire();

			if(!curWeapon()->getClip())
			{
				// Totally out of ammo, so change weapons (if possible).

				addCommand("weaponbestsafe",true);
				if(getWeaponNum(nextCommand)!=-1)
					return 0;
			}

			// Tell client to reload.

			owner->edict->client->ps.gunReload++;

			return 0;
		}
	}

	if(rulesDoWeaponsReload())
	{
		if(!isClient)
			pisv.ShowReload(owner->edict);

		setInputStatus(0);
		wAnim.setAnimType(WANIM_RELOAD);
		RunVWeapAnim(owner, reloadSeq, IGhoulInst::Hold);
	}
	else
		idle();

	// Tell client to reload.

	owner->edict->client->ps.gunReload++;

	return 1;
}

int inven_c::putAway(void)
{
	curInfo()->shutdownWeapon(*owner);

	char *putAwaySeq;

	if((putAwaySeq = curInfo()->getPutAwaySeq(*owner)) == NULL)
	{
		SelectPendingWeapon();
		takeOut();
		return 0;
	}

	if((!isClient)&&isClientPredicting())
		readyNextWeapon();

	setInputStatus(0);
	wAnim.setAnimType(WANIM_PUTAWAY);
	RunVWeapAnim(owner, putAwaySeq, IGhoulInst::Hold);
	
	return 1;
}

int inven_c::takeOut(void)
{
	// Ensure no weapon fires on taking new weapon out.

	if(!isClient)
	{
		pisv.clearFireEvent(getOwner()->edict);
		pisv.clearAltfireEvent(getOwner()->edict);
	}
	else
	{
		fireEvent=0;
		altfireEvent=0;
	}

	if(isClient)
		newInstRxd=0;

	char *takeOutSeq;
	
	if((takeOutSeq = curInfo()->getTakeOutSeq(*owner)) == NULL)
	{
		idle();
		return 0;
	}
	
	setInputStatus(0);
	wAnim.setAnimType(WANIM_NORMAL);
	RunVWeapAnim(owner, takeOutSeq, IGhoulInst::Hold);
	
	return 1;
}

int inven_c::idle(void)
{
	char *idleSeq;

	if((idleSeq = curInfo()->getIdleSeq(*owner)) == NULL)
		return 0;

	setInputStatus(1);
	wAnim.setAnimType(WANIM_NORMAL);
	RunVWeapAnim(owner, idleSeq, IGhoulInst::Hold);

	return 1;
}

void inven_c::idleReadied(void)
{
	char *idleSeq;

	if((idleSeq = weapInfo[readiedWeaponType]->getIdleSeq(*owner)) == NULL)
		return;

	setInputStatus(1);
	wAnim.setAnimType(WANIM_NORMAL);
	RunReadiedVWeapAnim(owner, idleSeq, IGhoulInst::Hold);
}

int inven_c::dryfire(void)
{
	char *dryfireSeq;

	if((dryfireSeq = curInfo()->getDryfireSeq(*owner)) == NULL)
	{
		idle();
		return 1;
	}

	setInputStatus(0);
	wAnim.setAnimType(WANIM_NORMAL);
	RunVWeapAnim(owner, dryfireSeq, IGhoulInst::Hold);

	return 1;
}

int inven_c::loseWeapon(void)
{
	curInfo()->shutdownWeapon(*owner);

	// Drop what we're holding.

	setDroppingWeapon(curWeaponSlot);
	if(!isClient)
		pisv.WeaponDrop(this->owner->edict,droppingWeapon()->getType(), droppingWeapon()->getClip());

	if(!isClient)
	{
		removeWeaponFromOwnList(droppingWeapon()->getType());
		droppingWeapon()->setType(SFW_EMPTYSLOT);
	}

	setDroppingWeapon(-1);

	// Select 'hand-pain' weapon.

	SelectPendingWeapon();

	// Make sure client is ready for next weapon when it comes down the wire.

	if(isClient)
		newInstRxd=0;

	// Play the takeout sequence for this weapon. On the server ONLY, this ends
	// with the issue of a PWC_NEXTWEAP.

	char *takeOutSeq = curInfo()->getTakeOutSeq(*owner);

	setInputStatus(0);
	wAnim.setAnimType(WANIM_LOSE);
	RunVWeapAnim(owner, takeOutSeq, IGhoulInst::Hold);

	// So we know we are now empty handed.

	curWeaponID=0;
	
	return 1;
}

static int weaponPrioritySafe[]=
{
	SFW_MICROWAVEPULSE,
	SFW_MACHINEGUN,
	SFW_ASSAULTRIFLE,
	SFW_MACHINEPISTOL,
	SFW_SHOTGUN,
	SFW_PISTOL2,
	SFW_PISTOL1,
	SFW_SNIPER,
	SFW_KNIFE,
	SFW_ROCKET,
	SFW_AUTOSHOTGUN,
	SFW_FLAMEGUN,
	SFW_EMPTYSLOT
};

static int weaponPriorityUnsafe[]=
{
	SFW_MICROWAVEPULSE,
	SFW_ROCKET,
	SFW_AUTOSHOTGUN,
	SFW_FLAMEGUN,
	SFW_MACHINEGUN,
	SFW_ASSAULTRIFLE,
	SFW_MACHINEPISTOL,
	SFW_SHOTGUN,
	SFW_PISTOL2,
	SFW_PISTOL1,
	SFW_SNIPER,
	SFW_KNIFE,
	SFW_EMPTYSLOT
};

static int weaponUnsafeList[]=
{
	SFW_ROCKET,
	SFW_AUTOSHOTGUN,
	SFW_FLAMEGUN,
	SFW_EMPTYSLOT
};

int inven_c::getBestSlot(int safe)
{
	int curTest = 0;
	int returnWeap = -1;
	int *weaponPriority;
	qboolean unsafe=false;

	// Okay, here's the scoop.  If the "safe" param is >= 2, then we've got a
	// pickup situation. In that case, the pickups act like a "safe best" 
	// weapon pick, except if there is an unsafe weapon up, don't override it.
	if (safe >= 2)
	{
		// Check if the current weapon is unsafe first.
		while((weaponUnsafeList[curTest] != SFW_EMPTYSLOT) && (!unsafe))
		{
			if (getCurWeaponType() == weaponUnsafeList[curTest])
			{
				unsafe=true;
			}

			curTest++;
		}

		if (unsafe)
		{	
			// Return the current weapon...
			return	getCurSlot();
		}
		else
		{	
			// Otherwise act like a safe weapon.
			curTest = 0;
			while((weaponPrioritySafe[curTest] != SFW_EMPTYSLOT) && (returnWeap == -1))
			{
				for(int i = 0; (i < MAXSELECTABLEWEAPONS) && (returnWeap == -1); i++)
				{
					if(weapons[i].canSelect(*owner))
					{
						if(weapons[i].getType() == weaponPrioritySafe[curTest])
						{
							returnWeap = i;
						}
					}
				}

				curTest++;
			}
		}
	}
	else
	{
		weaponPriority=(safe)?weaponPrioritySafe:weaponPriorityUnsafe;

		while((weaponPriority[curTest] != SFW_EMPTYSLOT) && (returnWeap == -1))
		{
			for(int i = 0; (i < MAXSELECTABLEWEAPONS) && (returnWeap == -1); i++)
			{
				if(weapons[i].canSelect(*owner))
				{
					if(weapons[i].getType() == weaponPriority[curTest])
					{
						returnWeap = i;
					}
				}
			}

			curTest++;
		}
	}

	return returnWeap;
}

int inven_c::getBestWeaponType(void)
{
	if (!strcmp(rulesGetBestWeaponMode(),"safe"))
	{	
		// The mode is for the unusual safe selection.
		return(weapons[getBestSlot(2)].getType());
	}
	else
	{	
		// Go for non-safe weaponry normally.
		return(weapons[getBestSlot(0)].getType());
	}
}

int	inven_c::getClipMaxByType(int weapon)
{
	return(weapInfo[weapon]->getClipSize());
}

int inven_c::getWeaponNum(int cmd)
{
	if(!isClient)
	{
		// No point finding a new-weap if we have no weapons.

		if(!(weaponsOwned&((1<<SFW_HURTHAND)-1)))
			return(-1);
	}

	int startWeap,returnWeap;

	startWeap=(selectedWeaponSlot>=MAXSELECTABLEWEAPONS)?0:selectedWeaponSlot;
	returnWeap=startWeap;

	if(cmd == PWC_WEAPNEXT)
	{
		do
		{
			returnWeap++;
			if(returnWeap >= MAXSELECTABLEWEAPONS)
			{
				returnWeap = 0;
			}
		}while((!weapons[returnWeap].canSelect(*owner,rulesCanSelectWithNoAmmo()))&&(returnWeap != startWeap));
	}
	else if(cmd == PWC_WEAPPREV)
	{
		do
		{
			returnWeap--;
			if(returnWeap <= -1)
			{
				returnWeap = MAXSELECTABLEWEAPONS - 1;
			}
		}while((!weapons[returnWeap].canSelect(*owner,rulesCanSelectWithNoAmmo()))&&(returnWeap != startWeap));
	}
	else if(cmd == PWC_WEAPONBEST_SAFE)
	{
		returnWeap = getBestSlot(1);
	}
	else if(cmd == PWC_WEAPONBEST_UNSAFE)
	{
		returnWeap = getBestSlot(0);
	}
	else if(cmd == PWC_LOSE)
	{
		returnWeap=MAXSELECTABLEWEAPONS;
	}
	else if(cmd == PWC_DROP)
	{
		returnWeap=MAXSELECTABLEWEAPONS+1;
	}
	else
	{	
		// Then must be a raw weapon number.

		returnWeap = cmd - PWC_WEAPSELECTFIRST;
		if(!weapons[returnWeap].canSelect(*owner,1))
		{
			returnWeap = -1;
		}
	}

	// New weapon same as old?

	if((startWeap==selectedWeaponSlot)&&(returnWeap==startWeap))
	{
		// Ok, no change.

		returnWeap = -1;
	}

	return returnWeap;
}

int inven_c::checkCommandInput(void)
{
	if(nextCommand == PWC_NOCOMMAND)
		return 0;

	int	retVal=0;

	if(nextCommand == PWC_RELOAD)
	{
		// This will only happen on the server.

		if(rulesDoWeaponsReload()&&reload())
			retVal=1;
	}
	else if((nextCommand >= PWC_WEAPNEXT)&&(nextCommand < PWC_WEAPSELECTFIRST + MAXSELECTABLEWEAPONS))
	{
		if(isClient&&(!clientOverrideCmd))
		{
			idle();

			return(0);
		}
		else
		{
			if(isClient)
				clientOverrideCmd=0;
	
			int weaponNum=getWeaponNum(nextCommand);

			if(curWeaponID)
			{
				// Ok, we currently have a weapon out...

				if(weaponNum!=-1)
				{
					// ...and we have a valid next weapon so proceed.

					nextSelectSlot = weaponNum;
					
					switch(nextCommand)
					{
						case PWC_LOSE:
						case PWC_DROP:

							if(rulesGetNonDroppableWeaponTypes()&(1<<curWeaponID))
							{
								nextCommand = PWC_NOCOMMAND;
								return 0;
							}
								
							loseWeapon();
							break;

						default:

							putAway();
							break;
					}

					retVal=1;
				}
			}
			else
			{
				// Ok, we don't currently have a weapon...

				if(weaponNum!=-1)
				{
					// ...but we have selected a valid next weapon, so proceed.

					if((nextCommand!=PWC_LOSE)&&(nextCommand!=PWC_DROP))
					{
						nextSelectSlot = weaponNum;

						SelectPendingWeapon();
						takeOut();

						retVal=1;
					}
					else
					{
						// ... and we tried to throw it - a big no no!

						selectNULLWeapon();
					}
				}
				else
				{
					// ...and we are here, which means we dropped / lost our
					// previous weapon then tried to select another weapon but
					// couldn't find one.

					selectNULLWeapon();
				}
			}
		}
	}

	nextCommand = PWC_NOCOMMAND;

	return retVal;
}

void inven_c::handleInput(void)
{
	if(wAnim.disguised())
	{	
		// Special - my weapon is not here.

		if((owner->attack)||(owner->altattack))
		{
			wAnim.setDisguise(0);
		}
		else
		{
			// Swap stuff around properly.

			if((nextCommand >= PWC_WEAPNEXT)&&(nextCommand < PWC_WEAPSELECTFIRST + MAXSELECTABLEWEAPONS))
			{
				switch(nextCommand)
				{
					case PWC_LOSE:
					case PWC_DROP:

						break;

					default:

						int weaponNum=getWeaponNum(nextCommand);
						
						if(weaponNum != -1)
							nextSelectSlot = weaponNum;
					
						break;
				}
				
				nextCommand=PWC_NOCOMMAND;
			}
		}
	}

	if(owner->cinematicFreeze)
	{	
		// Clear out input to make everything behave.

		owner->altattack = 0;
		owner->attack = 0;
		owner->leanLeft = 0;
		owner->leanRight = 0;
		owner->weap3 = 0;
		owner->weap4 = 0;
	}

	curInfo()->testInput(*owner);

	// Always return during cinematics.

	if(owner->cinematicFreeze)
		return;

	// Active sequences (e.g. fire) may prevent command input.

	if(!isClient)
	{
		// Server always wants to return if this is the case...

		if(!canHandleInput)
			return;
	}
	else 
	{
		// Client may need to overide current predicted sequence (e.g. fire) if
		// instructed to do so by server.

		if((!clientOverrideCmd)&&(!canHandleInput))
			return;
	}
	
	if(checkCommandInput())
	{
		// Check for inventory command input.

		return;
	}

	if(!model)
		return;

	// Check for attack.

	qboolean doAttack=false;

	if((!isClient)&&(!isClientPredicting()))
	{
		if(owner->attack)
			doAttack=true;
	}
	else 
	{
		if((isClient&&(owner->attack))||((!isClient)&&(pisv.getFireEvent(owner->edict))))
			doAttack=true;
	}
	
	if(doAttack)
	{
		if(fire())
			return;
	}

	// Check for altattack.

	qboolean doAltAttack=false;

	if((!isClient)&&(!isClientPredicting()))
	{
		if(owner->altattack)
			doAltAttack=true;
	}
	else 
	{
		if((isClient&&(owner->altattack))||((!isClient)&&(pisv.getAltfireEvent(owner->edict))))
			doAltAttack=true;
	}

	if(doAltAttack)
	{
		if(altFire())
			return;
	}
}

void inven_c::handleModelChange()
{
	if(wAnim.disguised())
	{	
		// Can't get out our new weapon until we are no longer being sneaky.

		return;
	}

	if(isClient)
	{
		// Client side version...

		if(((IGhoul *)picl.GetGhoul())->FindClientInst(owner->edict->client->ps.gunUUID))
		{
			// We need this test because of the latency involved in receiving new
			// ghoul instances from server. I.e. we want to be sure that we have
			// the new instance from the server before proceeding to create a new
			// PRIVATE client view weapon (by cloning the server owned instance).
			// In the event that the new instance has not been recieved yet, the
			// weapon code will remain in a 'supended' state as long as the member
			// variable 'pendingChange' is non zero.

			if(!owner->edict->client->ps.gun->GetUserData())
			{
				if(pendingChange)
				{
					// The new weapon has been selected (and the previous one 
					// put away), so take out the new weapon.

					SelectPendingWeapon();
					takeOut();
					pendingChange = 0;

					if(droppingWeapon())
						setDroppingWeapon(-1);
				}
				else if(newInstRxd==0)
				{
					// We have just gotten a new weapon instance from the server, so
					// issue a command to select this weapon.

					if(owner->edict->client->ps.gunType<SFW_HURTHAND)
					{
						// Weapon.

						for(int i=0;i<MAXSELECTABLEWEAPONS;i++)
						{
							if(weapons[i].getType()==owner->edict->client->ps.gunType)
								break;
						}

						weapon_c *weaponToFill = &weapons[i];
						int type = weaponToFill->getWeaponInfo()->getAmmoType(0);
						ammo[type]=owner->edict->client->ps.gunAmmo;
						weaponToFill->setClip(owner->edict->client->ps.gunClip);
						nextCommand=PWC_WEAPSELECTFIRST+i;
					}
					else
					{	
						// Special "throw weapon away" animation.

						nextCommand=PWC_LOSE+(owner->edict->client->ps.gunType-SFW_HURTHAND);
					}

					clientOverrideCmd=1;
					newInstRxd=1;
					
					if(model)
						idle();

				}
			}
		}
	}
	else
	{
		// Server side version...

		if(pendingChange)
		{
			if(isClientPredicting())
				SelectRediedWeapon();
			else
				SelectPendingWeapon();

			takeOut();
			pendingChange = 0;

			if(droppingWeapon())
			{
				if(!isClient)
					pisv.WeaponDrop(this->owner->edict,droppingWeapon()->getType(), droppingWeapon()->getClip());

				removeWeaponFromOwnList(droppingWeapon()->getType());
				droppingWeapon()->setType(SFW_EMPTYSLOT);
				setDroppingWeapon(-1);
			}
		}
	}
}

void inven_c::frameUpdate(void)
{
	if(isClient)
	{
		fireEvent=0;
		altfireEvent=0;

		if(owner->doReload)
		{
			owner->doReload=0;
			clientReload();
		}

		getServerState();		
	}

	handleModelChange();
	handleInput();

	sharedEdict_t *saved=owner;

	sharedEdict_t sh;
	sh.inv=(inven_c *)this;

	if(model)
	{
		if(!isClient)
			model->ServerUpdate(*(pisv.levelTime) + .001);
		else
			model->ServerUpdate(*(picl.levelTime) + .001);
	}

	sh.inv->setOwner(saved);

	if((!isClient)&&(!readiedWeaponID))
		setServerState();

	items.frameUpdate(*owner);
}

int inven_c::addCommand(char *string,qboolean internal)
{
	char *s=picom.Cmd_Args();

	if(!isClient)
	{
		// Server...

		if(getInputStatus()<0)
		{
			// Don't want any crappy messages popping up as chat, just 'cos the
			// system isn't ready to accept inventory commands.

			return 1;
		}

		// Sometimes we want to prevent sources outside of the player.dll from
		// executing weapon select commands (e.g. arsenal).

		int denied=((!internal)&&(!rulesCanFreelySelectWeapon()))?-1:0;

		if (stricmp (string, "reload") == 0)
		{
			nextCommand = PWC_RELOAD;
		}
		else if (stricmp (string, "weapprev") == 0)
		{
			if(denied)
				return 1;

			nextCommand = PWC_WEAPPREV;
		}
		else if (stricmp (string, "weapnext") == 0)
		{
			if(denied)
				return 1;

			nextCommand = PWC_WEAPNEXT;
		}
		else if (stricmp (string, "weaponselect") == 0)
		{
			if(denied)
				return 1;

			if(s[0])
				nextCommand = PWC_WEAPSELECTFIRST + atoi(s)-1;
		}
		else if (stricmp (string, "weapondrop") == 0) 
		{
			if(rulesCanDropWeapons())
			{
				nextCommand = PWC_DROP;
			}
			else
			{
				// Command weapondrop is valid but not allowed to drop weapons.
				// So we don't want anything printed as an unrecognised command.

				return 1;
			}
		}
		else if (stricmp (string, "weaponlose") == 0)
		{
			if(rulesCanDropWeapons())
			{
				nextCommand = PWC_LOSE;
			}
			else
			{
				// Command weaponloose is valid but not allowed to drop weapons.
				// So we don't want anything printed as an unrecognised command.

				return 1;
			}
		}
		else if (stricmp (string, "weaponbestsafe") == 0)
		{
			if(denied)
				return 1;

			nextCommand = PWC_WEAPONBEST_SAFE;
		}
		else if (stricmp (string, "weaponbestunsafe") == 0)
		{
			if(denied)
				return 1;

			nextCommand = PWC_WEAPONBEST_UNSAFE;
		}
		else if (stricmp (string, "itemuse") == 0)
		{
			if(s[0])
			{
				// Try to use named item (shortcut key).
				
				int i=items.getSlotNumFromName(s);

				if(i>=0)
				{
					// Ok, use specified item.

					items.setNextCommand(PEC_USEITEMFIRST+i);
				}
				else if(i==-2)
				{
					// Named item doesn't exist (no such item type).

					return 0;
				}
			}
			else
			{
				// Use currently selected item.

				items.setNextCommand(PEC_USEITEM);
			}
		}
		else if (stricmp (string, "itemnext") == 0)
		{
			items.setNextCommand(PEC_ITEMNEXT);
		}
		else if (stricmp (string, "itemprev") == 0)
		{
			items.setNextCommand(PEC_ITEMPREV);
		}
		else if (stricmp (string, "itemdrop") == 0)
		{
			items.setNextCommand(PEC_DROP);
		}
		else 
		{
			return 0;
		}
	}
	else
	{	
		// Client... (does nowt).

		return 0;
	}

	return 1;
}

/*
List of weapons (with ammo)
Knife - aknife
Pistol1 - a9mm
Pistol2 - a9mm (possibly a44)
Mpistol - a9mm
Assault - a556
Sniper - a556
Slugger - Shells
Shotgun - Shells
Machinegun - a556
Flamegun - Fuel
Rocket - Rockets
Mpg - Battery

Items
Flashpak
C4
Armor
Neural
Claymore
goggles

Ammo
aknife
a9mm
a556
a44
Shells
Rockets
Battery
Fuel
*/

char *weaponList[] =
{	"nothing",
	"knife",
	"pistol2",
	"pistol1",
	"mpistol",
	"assault",
	"sniper",
	"slugger",
	"shotgun",
	"machinegun",
	"rocket",
	"mpg",
	"flamegun",
	0
};

	//AMMO_NONE = 0,
	//AMMO_KNIFE,
	//AMMO_44,
	//AMMO_9MM,
	//AMMO_SHELLS,
	//AMMO_556,
	//AMMO_ROCKET,
	//AMMO_MWAVE,
	//AMMO_FTHROWER,
	//AMMO_SLUG,


char *ammoList[] =
{	"nothing",
	"aknife",
	"a44",
	"a9mm",
	"shells",
	"a556",
	"rockets",
	"battery",
	"fuel",
	"slug",
	0
};

//armor is a special case, I suppose
char *itemList[] =
{	"nothing",
	"fpak",
//	"neural",
	"c4",
	"goggles",
	"claymore",
	"medkit",
	"grenade",
	0
};

#define ORDERLISTSIZE 12

int weaponOrder[] =
{
	SFW_KNIFE,
	SFW_PISTOL1,
	SFW_PISTOL2,
	SFW_SHOTGUN,
	SFW_SNIPER,
	SFW_MACHINEPISTOL,
	SFW_ASSAULTRIFLE,
	SFW_MACHINEGUN,
	SFW_AUTOSHOTGUN,
	SFW_ROCKET,
	SFW_MICROWAVEPULSE,
	SFW_FLAMEGUN,
};

//health parm is the health extracted from the menu as we don't have access to the game-side health edict field
int inven_c::extractInvFromMenu(int *weaponsAvailable, int *health)
{
	char *curSpot;
	char *curTokSpot;
	char token[256];
	char temp[256];
	int  value;
	int  weaponsToAdd = 0;
	char buffer[1024];

	if(isClient)
	{
		return 0; // None of this for you!!
	}

	//assume curSpot is somewhere decent

	curSpot = pisv.CvarInfo(CVAR_WEAPON | CVAR_ITEM | CVAR_AMMO | CVAR_MISC);

	if(!curSpot || (!strcmp(curSpot, "")))
	{	// if there is nothing, plug in the defaults
		return 0;
	}

	while(*curSpot)
	{
		curTokSpot = token;

		if(*curSpot == '\\')
		{
			curSpot++;
		}

		while(*curSpot && (*curSpot != '\\'))
		{
			*curTokSpot++ = *curSpot++;
		}
		*curTokSpot = 0;

		assert(*curSpot);

		if(*curSpot == '\\')
		{
			curSpot++;
		}

		curTokSpot = temp;
		while((*curSpot) && (*curSpot != '\\'))
		{
			*curTokSpot++ = *curSpot++;
		}
		*curTokSpot = 0;
		value = atoi(temp);

		// okay, we have the two tokens... let's do stuff with them now...


		char *testString;
		int curCheck;
		int found = 0;



		// look for weapons

		curCheck = 0;

		do
		{
			testString = weaponList[curCheck];

			if(!stricmp(token, testString))
			{	// hey, I have one of these!  Cool!
				if(value)
				{// add the weapon
					//addWeaponType(curCheck);
					weaponsToAdd |= (1<<curCheck);
					if(weaponsAvailable)
					{
						*weaponsAvailable |= (1<<curCheck);
					}

					sprintf(buffer, "set %s 0 w;", weaponList[curCheck]);
					pisv.AddCommand(buffer);
				}

				found = 1;
				break;
			}

			curCheck++;
		}while(weaponList[curCheck]);

		if(found)
		{
			continue;
		}
		

		// look for ammo

		curCheck = 0;

		do
		{
			testString = ammoList[curCheck];

			if(!stricmp(token, testString))
			{	// add in some ammo, then
				addAmmoType(curCheck, value);

				// Hack for flamegun as it doesn't have an ammo reserve.
				if(curCheck==AMMO_FTHROWER)
					ammo[AMMO_FTHROWER]=value;

				sprintf(buffer, "set %s 0 a;", ammoList[curCheck]);
				pisv.AddCommand(buffer);

				found = 1;
				break;
			}

			curCheck++;
		}while(ammoList[curCheck]);

		if(found)
		{
			continue;
		}



		// look for items

		curCheck = 0;

		do
		{
			testString = itemList[curCheck];

			if(!stricmp(token, testString))
			{	// add in some ammo, then
				found = 1;
				if(value)
				{
					addItem(curCheck, value);

					sprintf(buffer, "set %s 0 i;", itemList[curCheck]);
					pisv.AddCommand(buffer);
				}

				break;
			}

			curCheck++;
		}while(itemList[curCheck]);

		if(found)
		{
			continue;
		}

		
		// is it armor, maybe?

		if(!stricmp("armor", token))
		{
			// add armor
			if(value)
			{
				addArmor(value);
			}
		}

		sprintf(buffer, "set armor 0 i;");
		pisv.AddCommand(buffer);

		// is it health, maybe?

		if(!stricmp("health", token))
		{
			if(value)
			{
				if(health)
				{
					*health = value;
				}

				sprintf(buffer, "set health 0 m;");
				pisv.AddCommand(buffer);
			}
		}
	}

	int weaponsWereAdded = 0;

	for(int i = 0; i < ORDERLISTSIZE; i++)
	{	// this will keep the weapons in order
		if(weaponsToAdd & (1<<weaponOrder[i]))
		{
			addWeaponType(weaponOrder[i]);
			weaponsWereAdded = 1;
		}
	}

	// no knife ammo means no defaults.  This stuff isn't going so well in conjunction with the menus
	return ammo[AMMO_KNIFE];
}

//health parm is the health extracted from the menu as we don't have access to the game-side health edict field
void inven_c::buildInvForMenu(int includeClipAmmo, int health)
{
	// first put all the clip ammo back into the ammo pools...
	// and list what weapons are currently being carried

	char buffer[1024];

	if(isClient)
	{	// client should have nothing to do with inventory-between-levels-stuff
		return;
	}

	for(int i = 0; i < MAXSELECTABLEWEAPONS; i++)
	{
		int curWeapType = weapons[i].getType();

		if(curWeapType != SFW_EMPTYSLOT)
		{
			assert(weapons[i].getWeaponInfo()->getAmmoType(0) < MAXAMMOS);
			assert(weapons[i].getWeaponInfo()->getAmmoType(0) > -1);
			if(includeClipAmmo)
			{	//end of level needs this - within level does not
				ammo[weapons[i].getWeaponInfo()->getAmmoType(0)] += weapons[i].getClip();
			}

			sprintf(buffer, "set %s 1 w;", weaponList[curWeapType]);

			pisv.AddCommand(buffer);
		}
	}


	// now list what ammo is currently being carried
	for(i = 1; i < MAXAMMOS; i++)
	{
		sprintf(buffer, "set %s %d a;", ammoList[i], ammo[i]);
		pisv.AddCommand(buffer);
	}


	// now list what items, and their amounts, are being carried

	for(i = 0; i < MAXITEMS; i++)
	{
		itemSlot_c *curSlot = items.getSlot(i);

		if(curSlot->getSlotType() != SFE_EMPTYSLOT)
		{
			sprintf(buffer, "set %s %d i;", itemList[curSlot->getSlotType()],
				curSlot->getSlotCount());
			pisv.AddCommand(buffer);
		}
	}

	// write out armor too, I suppose
	sprintf(buffer, "set armor %d i;", armor);
	pisv.AddCommand(buffer);

	// write out health too, I suppose
	sprintf(buffer, "set health %d m;", health);
	pisv.AddCommand(buffer);

}

int inven_c::extractInvFromString(int *weaponsAvailable,char *invString)
{
	char *curSpot;
	char *curTokSpot;
	char token[256];
	char temp[256];
	int  value;
	int  weaponsToAdd = 0;

	if(isClient)
	{
		 // None of this for you Mr Client side!!

		return 0;
	}

	curSpot = invString;

	if(!curSpot || (!strcmp(curSpot, "")))
	{	
		// If there is nothing, do nothing.

		return 0;
	}

	while(*curSpot)
	{
		curTokSpot = token;

		if(*curSpot == '\\')
		{
			curSpot++;
		}

		while(*curSpot && (*curSpot != '\\'))
		{
			*curTokSpot++ = *curSpot++;
		}
		*curTokSpot = 0;

		if(*curSpot == '\\')
		{
			curSpot++;
		}

		curTokSpot = temp;
		while((*curSpot) && (*curSpot != '\\'))
		{
			*curTokSpot++ = *curSpot++;
		}
		*curTokSpot = 0;
		value = atoi(temp);

		// Okay, we have the two tokens... let's do stuff with them now...

		char *testString;
		int curCheck;
		int found = 0;

		// Look for weapons.

		curCheck = 0;

		do
		{
			testString = weaponList[curCheck];

			if(!stricmp(token, testString))
			{	
				// Hey, I have one of these!  Cool!

				if(value)
				{
					// Add the weapon.

					weaponsToAdd |= (1<<curCheck);
					if(weaponsAvailable)
					{
						*weaponsAvailable |= (1<<curCheck);
					}
				}
				found = 1;
				break;
			}

			curCheck++;
		}while(weaponList[curCheck]);

		if(found)
		{
			continue;
		}
		
		// Look for ammo.

		curCheck = 0;

		do
		{
			testString = ammoList[curCheck];

			if(!stricmp(token, testString))
			{	
				// Add in some ammo.

				addAmmoType(curCheck, value);

				found = 1;
				break;
			}

			curCheck++;
		}while(ammoList[curCheck]);

		if(found)
		{
			continue;
		}

		// Look for items.

		curCheck = 0;

		do
		{
			testString = itemList[curCheck];

			if(!stricmp(token, testString))
			{	// add in some ammo, then
				found = 1;
				if(value)
				{
					addItem(curCheck, value);
				}
				break;
			}

			curCheck++;
		}while(itemList[curCheck]);

		if(found)
		{
			continue;
		}

		
		// Is it armor, maybe?

		if(!stricmp("armor", token))
		{
			if(value)
			{	
				// Add armor.

				addArmor(value);
			}
		}
	}

	for(int i = 0; i < ORDERLISTSIZE; i++)
	{	
		// This will keep the weapons in order.

		if(weaponsToAdd & (1<<weaponOrder[i]))
		{
			addWeaponType(weaponOrder[i]);
		}
	}

	return 1;
}

void inven_c::buildInvString(char *string,qboolean xtractWeapons,qboolean xtractAmmo,qboolean xtractItems,qboolean xtractArmor)
{
	char buffer[1024];

	if(isClient)
	{	
		// Client should have nothing to do with this stuff.

		return;
	}

	sprintf(string,"");

	if(xtractWeapons)
	{
		// Now list what weapons are currently being carried.

		for(int i = 0; i < MAXSELECTABLEWEAPONS; i++)
		{
			int curWeapType = weapons[i].getType();

			if(curWeapType != SFW_EMPTYSLOT)
			{
				assert(weapons[i].getWeaponInfo()->getAmmoType(0) < MAXAMMOS);
				assert(weapons[i].getWeaponInfo()->getAmmoType(0) > -1);
				ammo[weapons[i].getWeaponInfo()->getAmmoType(0)] += weapons[i].getClip();

				sprintf(buffer, "%s\\1\\", weaponList[curWeapType]);
				strcat(string,buffer);	//FIXME
			}
		}
	}

	if(xtractAmmo)
	{
		// Now list what ammo is currently being carried.

		for(int i = 1; i < MAXAMMOS; i++)
		{
			sprintf(buffer, "%s\\%d\\", ammoList[i], ammo[i]);
			strcat(string,buffer);//FIXME
		}
	}
	
	if(xtractItems)
	{
		// Now list what items, and their amounts, are being carried.

		for(int i = 0; i < MAXITEMS; i++)
		{
			itemSlot_c *curSlot = items.getSlot(i);

			if(curSlot->getSlotType() != SFE_EMPTYSLOT)
			{
				sprintf(buffer, "%s\\%d\\", itemList[curSlot->getSlotType()],
					curSlot->getSlotCount());
				
				strcat(string,buffer);//FIXME
			}
		}
	}

	if(xtractArmor)
	{
		// Write out armor too, I suppose.

		sprintf(buffer, "armor\\%d\\", armor);
		strcat(string,buffer);//FIXME
	}
}

void inven_c::stockWeapons(void)
{	
	// Go through all your possessions and ensure that they are filled with tasty ammo.

	for(int i = 0; i < MAXSELECTABLEWEAPONS; i++)
	{
		if(weapons[i].getType() != SFW_EMPTYSLOT)
			refillCurClip(i, 1, 0);
	}
}

void inven_c::setBloodyWeapon(int isBloody)
{
	if(!wAnim.getBloody())
	{
		if(!isClient)
			serverRICBuffer->AddRIC(RIC_SETBLOODYWEAPON,"i",isBloody);

		curInfo()->setBloody(owner);
		wAnim.setBloody(1);
	}
}

int inven_c::rulesDoWeaponsUseAmmo(void)
{
	return(rulesWeaponsUseAmmo);
}

void inven_c::rulesSetWeaponsUseAmmo(int useAmmo)
{
	if(!isClient)
		serverRICBuffer->AddRIC(RIC_RULESSETWEAPONSUSEAMMO,"i",useAmmo);

	rulesWeaponsUseAmmo=useAmmo;
}

int inven_c::rulesCanDropWeapons(void)
{
	return(rulesDropWeapons);
}

void inven_c::rulesSetDropWeapons(int dropWeapons)
{
	rulesDropWeapons=dropWeapons;
}

int inven_c::rulesGetNonDroppableWeaponTypes(void)
{
	return(rulesNonDroppableWeaponTypes);
}

void inven_c::rulesSetNonDroppableWeaponTypes(int nonDropMask)
{
	rulesNonDroppableWeaponTypes=(nonDropMask|(1<<SFW_HURTHAND)|(1<<SFW_THROWHAND)|(1<<SFW_EMPTYSLOT));
}

int	inven_c::rulesDoWeaponsReload(void)
{
	return(rulesWeaponsReload);
}

void inven_c::rulesSetWeaponsReload(int weaponsReload)
{
	if(!isClient)
		serverRICBuffer->AddRIC(RIC_RULESSETWEAPONSRELOAD,"i",weaponsReload);

	rulesWeaponsReload=weaponsReload;
}

int	inven_c::rulesIsWeaponReloadAutomatic(void)
{
	return(rulesWeaponReloadAutomatic);
}

void inven_c::rulesSetWeaponReloadAutomatic(int reloadAutomatic)
{
	if(!isClient)
		serverRICBuffer->AddRIC(RIC_RULESSETWEAPONRELOADAUTOMATIC,"i",reloadAutomatic);

	rulesWeaponReloadAutomatic=reloadAutomatic;
}

int	inven_c::rulesDoReloadEmptiesWeapon(void)
{
	return(rulesReloadEmptiesWeapon);
}

void inven_c::rulesSetReloadEmptiesWeapon(int reloadEmptiesWeapon)
{
	if(!isClient)
		serverRICBuffer->AddRIC(RIC_RULESSETRELOADEMPTIESWEAPON,"i",reloadEmptiesWeapon);

	rulesReloadEmptiesWeapon=reloadEmptiesWeapon;
}

char *inven_c::rulesGetBestWeaponMode(void)
{
	return(rulesBestWeaponMode);
}

void inven_c::rulesSetBestWeaponMode(char *bestWeaponMode)
{
	int len=strlen(bestWeaponMode);
	len=(len<5)?len:4;
	memcpy(rulesBestWeaponMode,bestWeaponMode,len);
	rulesBestWeaponMode[len]=0;
}

int inven_c::rulesCanDropInvenOnDeath(void)
{
	return(rulesDropInvenOnDeath);
}

void inven_c::rulesSetDropInvenOnDeath(int dropInvenOnDeath)
{
	rulesDropInvenOnDeath=dropInvenOnDeath;
}

int inven_c::rulesCanSelectWithNoAmmo(void)
{
	return(rulesSelectWithNoAmmo);
}

void inven_c::rulesSetSelectWithNoAmmo(int selectWithNoAmmo)
{
	rulesSelectWithNoAmmo=selectWithNoAmmo;
}

int inven_c::rulesCanFreelySelectWeapon(void)
{
	return(rulesFreelySelectWeapon);
}

void inven_c::rulesSetFreelySelectWeapon(int freelySelectWeapon)
{
	rulesFreelySelectWeapon=freelySelectWeapon;
}

weaponInfo_c *inven_c::curInfo(void)
{
	return weapInfo[curWeaponType];
}

weaponInfo_c *inven_c::readiedInfo(void)
{
	return weapInfo[readiedWeaponType];
}

weapon_c *inven_c::curWeapon(void)
{
	return &weapons[curWeaponSlot];
}

weapon_c *inven_c::readiedWeapon(void)
{
	return &weapons[readiedWeaponSlot];
}

void inven_c::clientClearRICs(void)
{
	clientRICBuffer->ClearRICs();
}

void inven_c::clientReadRICs(void)
{
	clientRICBuffer->ReadRICs();
}

void inven_c::clientProcessRICs(void)
{
	clientRICBuffer->ProcessRICs();
}

void inven_c::serverClearRICs(void)
{
	serverRICBuffer->ClearRICs();
}

void inven_c::serverWriteRICs(void)
{
	serverRICBuffer->WriteRICs();
}

inven_c *W_NewInv(void)
{
	return(new inven_c);
}

void W_KillInv(inven_c *inven)
{
	delete(inven);
}

void RefreshWeapon(inven_c *inven)
{
	if(isClient)
		return;

	// Set object to NULL to force a reload.

	inven->getCurWeapon()->getWeaponInfo()->setGhoulObj(NULL);

	// Reload weapon.

	inven->initNewWeapon();

	// Hack for sniper rifle. I don't care anymore thankyou.

	IGhoulInst	*gun=inven->getViewModel();

	if(gun)
		gun->SetAllPartsOnOff(inven->scopeIsActive()?false:true);
}

inven_c::inven_c(inven_c *orig)
{
	int		i;

	nextCommand = orig->nextCommand;
	clientOverrideCmd = orig->clientOverrideCmd;
	newInstRxd = orig->newInstRxd;
	nextSelectSlot = orig->nextSelectSlot;
	curWeaponID = orig->curWeaponID;
	curWeaponType = orig->curWeaponType;
	curWeaponSlot = orig->curWeaponSlot;
	readiedWeaponID = orig->readiedWeaponID;
	readiedWeaponType = orig->readiedWeaponType;
	readiedWeaponSlot = orig->readiedWeaponSlot;
	droppingSlot = orig->droppingSlot;

	for(i = 0; i < MAXWEAPONS; i++)
	{
		weapons[i] = orig->weapons[i];
	}
	rulesWeaponsUseAmmo = orig->rulesWeaponsUseAmmo;
	rulesDropWeapons = orig->rulesDropWeapons;
	rulesNonDroppableWeaponTypes = orig->rulesNonDroppableWeaponTypes;
	rulesWeaponsReload = orig->rulesWeaponsReload;
	rulesReloadEmptiesWeapon = orig->rulesReloadEmptiesWeapon;
	rulesWeaponReloadAutomatic = orig->rulesWeaponReloadAutomatic;
	rulesDropInvenOnDeath = orig->rulesDropInvenOnDeath;
	rulesSelectWithNoAmmo = orig->rulesSelectWithNoAmmo;
	for(i = 0; i < 5; i++)
	{
		rulesBestWeaponMode[i] = orig->rulesBestWeaponMode[i];
	}
	rulesFreelySelectWeapon = orig->rulesFreelySelectWeapon;

	for(i = 0; i < MAXAMMOS; i++)
	{
		ammo[i] = orig->ammo[i];
	}
	canHandleInput = orig->canHandleInput;
	model = NULL;				// This is always recreated from scratch
	readyModel = NULL;
	selectedWeaponSlot = orig->selectedWeaponSlot;
	armor = orig->armor;
	weaponsOwned = orig->weaponsOwned;
	pendingChange = orig->pendingChange;
	clientPredicting = orig->clientPredicting;

	gunEnd = orig->gunEnd;
	owner = NULL;

	items = orig->items;

	wAnim = orig->wAnim;

	clientRICBuffer = NULL;
	serverRICBuffer = NULL;
}

void inven_c::Evaluate(inven_c *orig)
{
	int		i;

	nextCommand = orig->nextCommand;
	clientOverrideCmd = orig->clientOverrideCmd;
	newInstRxd = orig->newInstRxd;
	nextSelectSlot = orig->nextSelectSlot;
	curWeaponID = orig->curWeaponID;
	curWeaponType = orig->curWeaponType;
	curWeaponSlot = orig->curWeaponSlot;
	readiedWeaponID = orig->readiedWeaponID;
	readiedWeaponType = orig->readiedWeaponType;
	readiedWeaponSlot = orig->readiedWeaponSlot;
	droppingSlot = orig->droppingSlot;

	for(i = 0; i < MAXWEAPONS; i++)
	{
		weapons[i] = orig->weapons[i];
	}
	rulesWeaponsUseAmmo = orig->rulesWeaponsUseAmmo;
	rulesDropWeapons = orig->rulesDropWeapons;
	rulesNonDroppableWeaponTypes = orig->rulesNonDroppableWeaponTypes;
	rulesWeaponsReload = orig->rulesWeaponsReload;
	rulesReloadEmptiesWeapon = orig->rulesReloadEmptiesWeapon;
	rulesWeaponReloadAutomatic = orig->rulesWeaponReloadAutomatic;
	rulesDropInvenOnDeath = orig->rulesDropInvenOnDeath;
	rulesSelectWithNoAmmo = orig->rulesSelectWithNoAmmo;
	for(i = 0; i < 5; i++)
	{
		rulesBestWeaponMode[i] = orig->rulesBestWeaponMode[i];
	}
	rulesFreelySelectWeapon = orig->rulesFreelySelectWeapon;

	for(i = 0; i < MAXAMMOS; i++)
	{
		ammo[i] = orig->ammo[i];
	}
	canHandleInput = orig->canHandleInput;
	model = NULL;				// This is always recreated from scratch
	readyModel = NULL;
	selectedWeaponSlot = orig->selectedWeaponSlot;
	armor = orig->armor;
	weaponsOwned = orig->weaponsOwned;
	pendingChange = orig->pendingChange;
	clientPredicting = orig->clientPredicting;

	gunEnd = orig->gunEnd;
	owner = NULL;

	items = orig->items;

	wAnim = orig->wAnim;
}

void inven_c::NetRead(sizebuf_t *net_message_ptr)
{
	char	loaded[sizeof(inven_c)];

	picl.ReadDataSizebuf(net_message_ptr, (byte *)(loaded + INVEN_SAVE_START), sizeof(inven_c) - INVEN_SAVE_START);
	Evaluate((inven_c *)loaded);
	clientPredicting=0;

	// FIXME: check for other fields that need to be cleared out (so that the
	// system will settle back into a stable state).

	model=0;
	readyModel=0;
}

void inven_c::NetWrite(int clientnum)
{
	inven_c		*tosend;
	byte		*save_start;

	tosend = new inven_c(this);
	save_start = (byte *)this;
	pisv.ReliableWriteDataToClient(save_start + INVEN_SAVE_START, sizeof(*this) - INVEN_SAVE_START, clientnum);
	delete tosend;
}

void inven_c::Read(void)
{
	char	loaded[sizeof(inven_c)];

	pisv.ReadFromSavegame('INVN', loaded + INVEN_SAVE_START, sizeof(inven_c) - INVEN_SAVE_START);
	Evaluate((inven_c *)loaded);
}

void inven_c::Write(void)
{
	inven_c		*savable;
	byte		*save_start;

	savable = new inven_c(this);
	save_start = (byte *)savable;
	pisv.AppendToSavegame('INVN', save_start + INVEN_SAVE_START, sizeof(*this) - INVEN_SAVE_START);
	delete savable;
}

// end