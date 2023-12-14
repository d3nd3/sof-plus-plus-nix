#include "q_shared.h"
#include "..\ghoul\ighoul.h"
#include "w_public.h"
#include "w_types.h"
#include "w_weapons.h"
#include "w_utils.h"
#include "player.h"
#include "../gamecpp/game.h"

extern player_com_import_t	picom;
extern player_sv_import_t	pisv;
extern player_cl_import_t	picl;

#define LEANINGLEFT(a)	(a.viewside > 0.0)
#define LEANINGRIGHT(a) (a.viewside < 0.0)

extern int isClient;
extern int fireEvent;
extern int altfireEvent;

//----------------------------------------------------------------------------------------------------

knifeInfo::knifeInfo(weaponInfoExtra_t *w):weaponInfo_c(w){}

void knifeInfo::precacheResources(void)
{
	weaponInfo_c::precacheResources();
}

void knifeInfo::setBloody(sharedEdict_t *self)
{
	SetWSkin(self,"knife","knifeblood");
}

char *knifeInfo::getSlashLeft(void)
{
	int rVal = ClientServerRand(0, 100);

	if(rVal < 33)
	{
		return "fire1_2sl_to_sr";
	}
	else if(rVal < 67)
	{
		return "fire1_2dl_to_ur";
	}
	else
	{
		return "fire1_2ul_to_dr";
	}
	return NULL;
}

char *knifeInfo::getSlashRight(void)
{
	int rVal = ClientServerRand(0, 100);

	if(rVal < 33)
	{
		return "fire1_2sr_to_sl";
	}
	else if(rVal < 67)
	{
		return "fire1_2dr_to_ul";
	}
	else
	{
		return "fire1_2ur_to_dl";
	}
	return NULL;
}

char *knifeInfo::getFireSeq(sharedEdict_t &ent)
{
	if(ClientServerRand(0, 100) < 75)
	{
		return "fire1_2a";
	}
	else
	{
		return "fire1_1a";
	}
}

char *knifeInfo::getIdleSeq(sharedEdict_t &ent)
{
	int rVal = ClientServerRand(0, 100);

	if(wAnim(ent)->getLastWasInteresting())
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}

	if(rVal < 20)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_1";
	}
	else if (rVal < 40)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_2";
	}
	else if (rVal < 60)
	{
		if(wAnim(ent)->getBloody())
		{
			wAnim(ent)->setInteresting(1);
			return "idleclean";
		}
		wAnim(ent)->setInteresting(0);
		return "idle";
	}
	else
	{
		return "idle";
	}

	return NULL;
}

char *knifeInfo::getAltFireSeq(sharedEdict_t &ent)
{
	return "fire2_1";	
}

char *knifeInfo::getReloadSeq(sharedEdict_t &ent)
{
	return NULL;
}

char *knifeInfo::getTakeOutSeq(sharedEdict_t &ent)
{
	return "ready";
}

char *knifeInfo::getPutAwaySeq(sharedEdict_t &ent)
{
	if(wAnim(ent)->getBloody())
		wAnim(ent)->setBloody(0);

	return "done";
}

int	knifeInfo::handleEOS(sharedEdict_t &ent)
{
	char *newAnim = NULL;
	char *curAnim = wAnim(ent)->getSeqName();

	if(!strcmp(curAnim, "fire1_1a"))
	{
		newAnim = "fire1_1b";
	}
	else if(!strcmp(curAnim, "fire2_1"))
	{
		wAnim(ent)->setBloody(0);

		SetWSkin(&ent, "knife", "knife");
		newAnim = "fire2_2";
	}
	else if(!strcmp(curAnim, "fire1_2a"))
	{
		// So that the looping mechanism works correctly.
		wAnim(ent)->clearFireLoop();

		newAnim = getSlashRight();
	}
	else if(!strcmp(curAnim, "fire1_2l_to_idle"))
	{
		newAnim = "fire1_2a";
	}
	else if(!strcmp(curAnim, "fire2_2"))
	{
		if(inven(ent)->rulesDoWeaponsUseAmmo())
		{
			if(inven(ent)->getAmmoAmount(AMMO_KNIFE) <= 1)
				inven(ent)->idle();
		}
	}

	if(newAnim)
	{
		RunVWeapAnim(&ent, newAnim, IGhoulInst::Hold);
		return 1;
	}

	return 0;
}

void knifeInfo::handleFireLoop(sharedEdict_t &ent)
{
	char *newAnim = NULL;

	if(!strcmp(wAnim(ent)->getSeqName(), "fire1_1b"))
	{
		RunVWeapAnim(&ent, getFireSeq(ent), IGhoulInst::Hold);
		return;
	}

	switch(wAnim(ent)->getFireLoop()%2)
	{
	case 1:
		newAnim = getSlashLeft();
		break;
	case 0:
	case 2:
		newAnim = getSlashRight();
		break;
//	case 3:
//		newAnim = "fire1_2l_to_idle";
		break;
	}

	RunVWeapAnim(&ent, newAnim, IGhoulInst::Hold);
}

void knifeInfo::handleAltfireLoop(sharedEdict_t &ent)
{
	if(wAnim(ent)->getKnifeStatus() > 0)
	{
		RunVWeapAnim(&ent, getAltFireSeq(ent), IGhoulInst::Hold);
	}
	else 
	{
		// If out of ammo, just idle.

		inven(ent)->idle();
	}
}

void knifeInfo::handleSpecialNotes(char *string, sharedEdict_t &ent)
{
	if(wAnim(ent)->getBloody())
	{
		wAnim(ent)->setBloody(0);
		SetWSkin(&ent, "knife", "knife");
	}
}

//----------------------------------------------------------------------------------------------------

pistol2Info::pistol2Info(weaponInfoExtra_t *w):weaponInfo_c(w){}

void pistol2Info::precacheResources(void)
{
	weaponInfo_c::precacheResources();
	pisv.EffectIndex("weapons/playermz/pistol2");
}


char *pistol2Info::getFireSeq(sharedEdict_t &ent)
{
	wAnim(ent)->setWeaponLifted(0);
	if(wAnim(ent)->getHandSide())
	{
		return "fire1_c";
	}
	return "fire1_a";
}

char *pistol2Info::getDryfireSeq(sharedEdict_t &ent)
{
	wAnim(ent)->setWeaponLifted(0);
	if(wAnim(ent)->getHandSide())
	{
		return "dryfire_lhand";
	}
	return "dryfire_rhand";
}

char *pistol2Info::getIdleSeq(sharedEdict_t &ent)
{
	int rVal = ClientServerRand(0, 100);

	if(ent.leanLeft)
	{	
		// Left.

		if(!(wAnim(ent)->getHandSide()))
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setWeaponLifted(0);
			wAnim(ent)->setHandSide(1);
			return "idle_a_to_idle_c";
		}
	}
	else if(ent.leanRight)
	{
		// Right.

		if(wAnim(ent)->getHandSide() == 1)
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setHandSide(0);
			wAnim(ent)->setWeaponLifted(0);
			return "idle_c_to_idle_a";
		}
	}

	if(wAnim(ent)->getHandSide())
	{	
		// Anims for the left side.

		if(wAnim(ent)->getLastWasInteresting())
		{
			wAnim(ent)->setTransition(0);
			wAnim(ent)->setInteresting(0);
			return "idle_c";
		}

		if(rVal < 80)
		{
			wAnim(ent)->setInteresting(0);
			wAnim(ent)->setTransition(0);
			return "idle_c";
		}
		else
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(0);
			return "idlefinger_c";
		}
	}
	if(wAnim(ent)->getWeaponLifted())
	{	
		// Idle anims for lifted on the right.

		if(wAnim(ent)->getLastWasInteresting())
		{
			wAnim(ent)->setInteresting(0);
			wAnim(ent)->setTransition(0);
			return "idle_d";
		}

		if(rVal < 10)
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setWeaponLifted(0);
			return "idle_d_to_idle_a";

		}
		else
		{
			wAnim(ent)->setInteresting(0);
			wAnim(ent)->setTransition(0);
			return "idle_d";
		}
	}
	else
	{	
		// Idle anims for the right side when not lifted.

		if(wAnim(ent)->getLastWasInteresting())
		{
			wAnim(ent)->setTransition(0);
			wAnim(ent)->setInteresting(0);
			return "idle_a";
		}

		if(rVal < 10)
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setWeaponLifted(1);
			return "idle_a_to_idle_d";
		}
		else if(rVal < 90)
		{
			wAnim(ent)->setInteresting(0);
			wAnim(ent)->setTransition(0);
			return "idle_a";
		}
		else
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(0);
			return "idlefinger_a";
		}
	}
	return "idle_a";
}

char *pistol2Info::getAltFireSeq(sharedEdict_t &ent)
{
	return NULL;
}

char *pistol2Info::getReloadSeq(sharedEdict_t &ent)
{
	wAnim(ent)->setWeaponLifted(0);
	if(wAnim(ent)->getHandSide())
	{
		return "reload_c";
	}
	return "reload_a";
}

char *pistol2Info::getTakeOutSeq(sharedEdict_t &ent)
{
	wAnim(ent)->setWeaponLifted(0);
	wAnim(ent)->setHandSide(0);
	return "ready_a";
}

char *pistol2Info::getPutAwaySeq(sharedEdict_t &ent)
{
	if(wAnim(ent)->getHandSide())
	{
		return "done_c";
	}
	return "done_a";
}

//----------------------------------------------------------------------------------------------------

pistol1Info::pistol1Info(weaponInfoExtra_t *w):weaponInfo_c(w){}

void pistol1Info::precacheResources(void)
{
	weaponInfo_c::precacheResources();
	pisv.EffectIndex("weapons/playermz/pistol1");
}


char *pistol1Info::getFireSeq(sharedEdict_t &ent)
{
	wAnim(ent)->setWeaponLifted(0);
	return (wAnim(ent)->getHandsInUse() == 1) ? "fire1_a" : "fire1_b";
}

char *pistol1Info::getAltFireSeq(sharedEdict_t &ent)
{
	return "fire1_a";
}

char *pistol1Info::getDryfireSeq(sharedEdict_t &ent)
{
	wAnim(ent)->setWeaponLifted(0);
	return (wAnim(ent)->getHandsInUse() == 1) ? "dryfire_1hand" : "dryfire_2hands";
}

char *pistol1Info::getIdleSeq(sharedEdict_t &ent)
{
	int rVal = ClientServerRand(0, 100);

	if(ent.leanLeft)
	{	// left
		if((wAnim(ent)->getHandsInUse() == 1)||(wAnim(ent)->getWeaponLifted()))
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setHandsInUse(2);
			wAnim(ent)->setWeaponLifted(0);
			return "idle_a_to_idle_b";
		}
	}
	else if(ent.leanRight)
	{
		if(wAnim(ent)->getHandsInUse() == 2)
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setHandsInUse(1);
			return "idle_b_to_idle_a";
		}
	}

	if(wAnim(ent)->getWeaponLifted() == 1)
	{
		if(wAnim(ent)->getLastWasInteresting())
		{
			wAnim(ent)->setTransition(0);
			wAnim(ent)->setInteresting(0);
			return "idle_c";
		}

		if((rVal < 30)&&(!wAnim(ent)->getJustTransitioned())&&(!ent.leanLeft))
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setHandsInUse(2);
			wAnim(ent)->setWeaponLifted(0);
			return "idle_c_to_idle_b";
		}
		else if(rVal < 90)
		{
			wAnim(ent)->setInteresting(0);
			wAnim(ent)->setTransition(0);
			return "idle_c";
		}
		else
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(0);
			return "idlefinger_c";
		}
	}
	else if(wAnim(ent)->getHandsInUse() == 1)
	{
		if(wAnim(ent)->getLastWasInteresting())
		{
			wAnim(ent)->setTransition(0);
			wAnim(ent)->setInteresting(0);
			return "idle_a";
		}

		if((rVal < 20)&&(!wAnim(ent)->getJustTransitioned())&&(!(ent.leanLeft)))
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setHandsInUse(2);
			return "idle_a_to_idle_b";
		}
		else if(rVal < 80)
		{
			wAnim(ent)->setInteresting(0);
			wAnim(ent)->setTransition(0);
			return "idle_a";
		}
		else
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(0);
			return "idlefinger_a";
		}
	}
	else
	{
		if(wAnim(ent)->getLastWasInteresting())
		{
			wAnim(ent)->setTransition(0);
			wAnim(ent)->setInteresting(0);
			return "idle_b";
		}

		if((rVal < 10)&&(!wAnim(ent)->getJustTransitioned())&&(!(ent.leanRight)))
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setWeaponLifted(1);
			return "idle_b_to_idle_c";
		}
		else if((rVal < 30)&&(!wAnim(ent)->getJustTransitioned())&&(!(ent.leanRight)))
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setHandsInUse(1);
			return "idle_b_to_idle_a";
		}
		else if(rVal < 95)
		{
			wAnim(ent)->setTransition(0);
			return "idle_b";
		}
		else
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(0);
			return "idlefinger_b";
		}
	}
	return NULL;
}

char *pistol1Info::getReloadSeq(sharedEdict_t &ent)
{
	return (wAnim(ent)->getHandsInUse() == 1) ? "reload_a":"reload_b";
}

char *pistol1Info::getTakeOutSeq(sharedEdict_t &ent)
{
	wAnim(ent)->setHandsInUse(1);
	wAnim(ent)->setWeaponLifted(0);
	return "ready_a";
}

int pistol1Info::handleEOS(sharedEdict_t &ent)
{
	return 0;
}

void pistol1Info::handleAltfireLoop(sharedEdict_t &ent)
{
	wAnim(ent)->setFirstShot(1);
	inven(ent)->altFire();
}

char *pistol1Info::getPutAwaySeq(sharedEdict_t &ent)
{
	return (wAnim(ent)->getHandsInUse() == 1) ? "done_a":"done_b";
}


int pistol1Info::atMomentOfFire(sharedEdict_t &ent)
{	
	if(strcmp(wAnim(ent)->getSeqName(), "fire2_b"))
		return 1;

	int val = wAnim(ent)->firstShot();
	wAnim(ent)->setFirstShot(0);
	if(val)
		return 1;

	inven(ent)->getAnimInfo()->incFireLoop();
	return(inven(ent)->getCurWeapon()->handleAmmo(ent, 0));
}

//----------------------------------------------------------------------------------------------------

machinePistolInfo::machinePistolInfo(weaponInfoExtra_t *w):weaponInfo_c(w){}

void machinePistolInfo::precacheResources(void)
{
	weaponInfo_c::precacheResources();
	pisv.EffectIndex("weapons/playermz/machinepistol");
}

char *machinePistolInfo::getFireSeq(sharedEdict_t &ent)
{
	return "fire2_1";
}

char *machinePistolInfo::getIdleSeq(sharedEdict_t &ent)
{
	if(wAnim(ent)->getLastWasInteresting())
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}

	if(ClientServerRand(0, 100) < 20)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_a";
	}
	else
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}
}

char *machinePistolInfo::getAltFireSeq(sharedEdict_t &ent)
{
	return "fire1";

}

char *machinePistolInfo::getReloadSeq(sharedEdict_t &ent)
{
	return "reload";
}

char *machinePistolInfo::getTakeOutSeq(sharedEdict_t &ent)
{
	return "ready";
}

char *machinePistolInfo::getDryfireSeq(sharedEdict_t &ent)
{
	return "dryfire";
}

int machinePistolInfo::handleEOS(sharedEdict_t &ent)
{
	if(!strcmp(wAnim(ent)->getSeqName(), "fire2_1"))
	{
		if(!ent.attack)
		{
			RunVWeapAnim(&ent, "fire2_2", IGhoulInst::Hold);
			return 1;
		}
	}
	return 0;
}

char *machinePistolInfo::getPutAwaySeq(sharedEdict_t &ent)
{
	return "done";
}

void machinePistolInfo::handleFireLoop(sharedEdict_t &ent)
{
	wAnim(ent)->setFirstShot(1);
}

int machinePistolInfo::atMomentOfFire(sharedEdict_t &ent)
{
	float curTime;

	if(!isClient)
		curTime = *pisv.levelTime;
	else
		curTime = *picl.levelTime;

	if(wAnim(ent)->getlastShootTime() == curTime)
		return 0;

	wAnim(ent)->setlastShootTime(curTime);
	int val = wAnim(ent)->firstShot();
	wAnim(ent)->setFirstShot(0);
	if(val)
		return 1;

	inven(ent)->getAnimInfo()->incFireLoop();
	return (inven(ent)->getCurWeapon()->handleAmmo(ent, 0));
}

int machinePistolInfo::testInput(sharedEdict_t &ent)
{
	if(wAnim(ent)->getAnimType() == WANIM_FIRE)
	{
		if(!ent.attack)
		{
			// Stopped firing - interrupt anim.

			inven(ent)->idle();
		}
	}

	return 1;
}


//----------------------------------------------------------------------------------------------------

assaultRifleInfo::assaultRifleInfo(weaponInfoExtra_t *w):weaponInfo_c(w){}

void assaultRifleInfo::precacheResources(void)
{
	weaponInfo_c::precacheResources();
	pisv.EffectIndex("weapons/playermz/assaultrifle");
}

char *assaultRifleInfo::getFireSeq(sharedEdict_t &ent)
{
	wAnim(ent)->setFirstShot(1);

	if(wAnim(ent)->getDown())
	{
		return "fire1_b";
	}
	else
	{
		return "fire1_a";
	}

	return "fire1_a";
}

char *assaultRifleInfo::getIdleSeq(sharedEdict_t &ent)
{
	int rVal = ClientServerRand(0, 100);

	if(ent.leanRight)
	{	
		// Right.

		if(wAnim(ent)->getDown())
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setDown(0);
			return "idle_b_to_idle_a";
		}
	}
	else if(ent.leanLeft)
	{
		// Left.

		if(!wAnim(ent)->getDown())
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setDown(1);
			return "idle_a_to_idle_b";
		}
	}


	if(wAnim(ent)->getDown())
	{
		if(wAnim(ent)->getLastWasInteresting())
		{
			wAnim(ent)->setTransition(0);
			wAnim(ent)->setInteresting(0);
			return "idle_b";
		}

		if((rVal < 30)&&(!wAnim(ent)->getJustTransitioned()))
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setDown(0);
			wAnim(ent)->setTransition(1);
			return "idle_b_to_idle_a";
		}
		else if(rVal < 50)
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(0);
			return "idlefinger_b";
		}
		else
		{
			wAnim(ent)->setInteresting(0);
			wAnim(ent)->setTransition(0);
			return "idle_b";
		}
	}
	else
	{
		if(wAnim(ent)->getLastWasInteresting())
		{
			wAnim(ent)->setTransition(0);
			wAnim(ent)->setInteresting(0);
			return "idle_a";
		}

		if((rVal < 10)&&(!wAnim(ent)->getJustTransitioned()))
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setDown(1);
			return "idle_a_to_idle_b";
		}
		else if(rVal < 20)
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(0);
			return "idlefinger_a";
		}
		else
		{
			wAnim(ent)->setInteresting(0);
			wAnim(ent)->setTransition(0);
			return "idle_a";
		}
	}
	return "idle_a";
}

char *assaultRifleInfo::getAltFireSeq(sharedEdict_t &ent)
{
	return NULL;
}

char *assaultRifleInfo::getReloadSeq(sharedEdict_t &ent)
{
	if(wAnim(ent)->getDown())
	{
		return "reload_b";
	}
	else
	{
		return "reload_a";
	}
	return "reload";
}

char *assaultRifleInfo::getTakeOutSeq(sharedEdict_t &ent)
{
	return "ready";
}

char *assaultRifleInfo::getPutAwaySeq(sharedEdict_t &ent)
{
	if(wAnim(ent)->getDown())
	{
		return "done_b";
	}
	else
	{
		return "done_a";
	}
	return "done";
}

char *assaultRifleInfo::getDryfireSeq(sharedEdict_t &ent)
{
	if(wAnim(ent)->getDown())
	{
		return "dryfire_side";
	}
	else
	{
		return "dryfire_up";
	}

	return "dryfire";
}

int assaultRifleInfo::atMomentOfFire(sharedEdict_t &ent)
{	
	float curTime;

	if(!isClient)
		curTime = *pisv.levelTime;
	else
		curTime = *picl.levelTime;

	if(wAnim(ent)->getlastShootTime() == curTime)
	{
		return 0;
	}
	wAnim(ent)->setlastShootTime(curTime);

	int val = wAnim(ent)->firstShot();
	wAnim(ent)->setFirstShot(0);
	if(val)return 1;

	inven(ent)->getAnimInfo()->incFireLoop();
	return (inven(ent)->getCurWeapon()->handleAmmo(ent, 0));
}

int assaultRifleInfo::testInput(sharedEdict_t &ent)
{
	if(wAnim(ent)->getAnimType() == WANIM_FIRE)
	{
		if(!ent.attack)
		{
			// Stopped firing - interrupt anim.

			inven(ent)->idle();
		}
	}
	else
	{
		if(!inven(ent)->getInputStatus())return 1;
		if(wAnim(ent)->getJustTransitioned())return 1;

		if(ent.weap3)
		{
			if(!wAnim(ent)->getDown())
			{
				wAnim(ent)->setInteresting(1);
				wAnim(ent)->setTransition(1);
				wAnim(ent)->setDown(1);
				RunVWeapAnim(&ent, "idle_a_to_idle_b", IGhoulInst::Hold);
			}
		}
		if(ent.weap4)
		{
			if(wAnim(ent)->getDown())
			{
				wAnim(ent)->setInteresting(1);
				wAnim(ent)->setTransition(1);
				wAnim(ent)->setDown(0);
				RunVWeapAnim(&ent, "idle_b_to_idle_a", IGhoulInst::Hold);
			}
		}
		return 1;

	}
	return 1;
}

void assaultRifleInfo::turnOffAmmoParts(sharedEdict_t &ent,int clipLeft,qboolean forceAll)
{
	IGhoulInst	*inst=(!isClient)?ent.edict->client->ps.gun:ent.inv->getViewModel();

	if(!inst)
		return;

	IGhoulObj	*obj=inst->GetGhoulObject();

	if(!obj)
		return;

	char	buf[32];
	GhoulID	part;

	if((clipLeft>2)&&(clipLeft<16))
	{
		if((!isClient)&&(!forceAll))
		{	
			sprintf(buf, "%d", clipLeft + 1);

			if(part=obj->FindPart(buf))	
				inst->SetPartOnOff(part, false);
		}
		else
		{
			for(;clipLeft<16;clipLeft++)
			{
				sprintf(buf, "%d", clipLeft + 1);

				if(part=obj->FindPart(buf))	
					inst->SetPartOnOff(part, false);
			}
		}
	}
}

int assaultRifleInfo::useAmmo1(sharedEdict_t &ent, int *skipIdle)
{
	int retVal=weaponInfo_c::useAmmo1(ent);

	if(inven(ent)->rulesDoWeaponsUseAmmo())
		turnOffAmmoParts(ent,inven(ent)->getCurWeapon()->getClip());

	return(retVal);
}

void assaultRifleInfo::handleSpecialNotes(char *string, sharedEdict_t &ent)
{
	IGhoulInst	*inst=(!isClient)?ent.edict->client->ps.gun:ent.inv->getViewModel();

	if(!inst)
		return;

	IGhoulObj	*obj= inst->GetGhoulObject();
	
	if(!obj)
		return;

	GhoulID	part;

	int clip;

	if(inven(ent)->rulesDoWeaponsUseAmmo())
		clip=inven(ent)->getCurClip();
	else
		clip=16;

	for(int i=4; (i<17)&&(i<=clip); i++)
	{
		char buf[10];

		sprintf(buf, "%d", i);
		part = obj->FindPart(buf);
		if(part)inst->SetPartOnOff(part, true);
	}
}

//----------------------------------------------------------------------------------------------------

sniperInfo::sniperInfo(weaponInfoExtra_t *w):weaponInfo_c(w){}

void sniperInfo::precacheResources(void)
{
	weaponInfo_c::precacheResources();
	pisv.EffectIndex("weapons/playermz/sniper");
}


char *sniperInfo::getFireSeq(sharedEdict_t &ent)
{
	return "fire1";
}

char *sniperInfo::getDryfireSeq(sharedEdict_t &ent)
{
	return "dryfire";
}

char *sniperInfo::getIdleSeq(sharedEdict_t &ent)
{
	int rVal = ClientServerRand(0, 100);

	if(wAnim(ent)->getDown())
	{
		if(wAnim(ent)->getLastWasInteresting())
		{
			wAnim(ent)->setInteresting(0);
			return "idle_a";
		}

		else if(rVal < 20)
		{
			wAnim(ent)->setInteresting(1);
			return "idlefinger";
		}
		else
		{
			wAnim(ent)->setInteresting(0);
			return "idle_a";
		}
	}

	return "idle_a";
}

char *sniperInfo::getAltFireSeq(sharedEdict_t &ent)
{
	if(wAnim(ent)->getInZoom())return NULL;

	if(ent.edict->client->ps.fov < 95)
	{
		IGhoulInst	*gun;

		if(!isClient)
			gun = ent.edict->client->ps.gun;
		else
			gun = ent.inv->getViewModel();

		if(gun)
		{
			gun->SetAllPartsOnOff(true);
		}

		ent.edict->client->ps.fov = 95;
		wAnim(ent)->setRezoom(0);
		wAnim(ent)->setInZoomView(0);
		return "scopeout";
	}
	else
	{
		return "scopein";
	}

	return "scopein";
}

char *sniperInfo::getReloadSeq(sharedEdict_t &ent)
{
	if(!inven(ent)->rulesDoWeaponsReload())
		return "idle";

	IGhoulInst	*gun;

	wAnim(ent)->setDown(1);
	wAnim(ent)->setTransition(1);

	if(ent.edict->client->ps.fov < 95)
	{
		if(!isClient)
			gun = ent.edict->client->ps.gun;
		else
			gun = ent.inv->getViewModel();

		if(gun)
		{
			gun->SetAllPartsOnOff(true);
		}

		ent.edict->client->ps.fov = 95;
		wAnim(ent)->setInZoomView(0);
	}

	return "reload";
}

char *sniperInfo::getTakeOutSeq(sharedEdict_t &ent)
{
	wAnim(ent)->setDown(1);
	wAnim(ent)->setTransition(1);
	wAnim(ent)->setRezoom(0);
	return "ready";
}

int sniperInfo::handleEOS(sharedEdict_t &ent)
{
	char		*newSeq = 0;
	IGhoulInst	*gun;

	if(!strcmp(wAnim(ent)->getSeqName(), "scopein"))
	{
		ent.edict->client->ps.fov = 60;
		wAnim(ent)->setInZoom(1);
		wAnim(ent)->setInZoomView(1);
		wAnim(ent)->setRezoom(ent.edict->client->ps.fov);

		if(!isClient)
			gun = ent.edict->client->ps.gun;
		else
			gun = ent.inv->getViewModel();

		if(gun)
		{
			gun->SetAllPartsOnOff(false);
		}

		inven(ent)->idle(); //fixme change
		return 1;
	}
	else if(!strcmp(wAnim(ent)->getSeqName(), "scopeout"))
	{
		inven(ent)->idle();
		return 1;
	}
	else if(!strcmp(wAnim(ent)->getSeqName(), "reload"))
	{
		if(wAnim(ent)->getRezoom())
		{
			ent.edict->client->ps.fov = wAnim(ent)->getRezoom();
			wAnim(ent)->setInZoom(1);
			wAnim(ent)->setInZoomView(1);

			if(!isClient)
				gun = ent.edict->client->ps.gun;
			else
				gun = ent.inv->getViewModel();
		
			if(gun)
			{
				gun->SetAllPartsOnOff(false);
			}

			inven(ent)->idle();
		}
	}

	if(newSeq)
	{
		RunVWeapAnim(&ent, newSeq, IGhoulInst::Hold);
		return 1;
	}
	return 0;
}

void sniperInfo::handleFireLoop(sharedEdict_t &ent)
{
	RunVWeapAnim(&ent, getFireSeq(ent), IGhoulInst::Hold);
}

void sniperInfo::handleAltfireLoop(sharedEdict_t &ent)
{
	inven(ent)->idle();
}

char *sniperInfo::getPutAwaySeq(sharedEdict_t &ent)
{
	if(wAnim(ent)->getDown())return "done_a";
	return "done_b";
}


//----------------------------------------------------------------------------------------------------

autoshotgunInfo::autoshotgunInfo(weaponInfoExtra_t *w):weaponInfo_c(w){}

void autoshotgunInfo::precacheResources(void)
{
	weaponInfo_c::precacheResources();
	pisv.EffectIndex("weapons/playermz/autoshotgun");
}


char *autoshotgunInfo::getFireSeq(sharedEdict_t &ent)
{
	// Set the firing speed, the autoshot is slower than the input anim...
//	ent.inv->getViewModel()->SetSpeed(getFireSpeed());

	return "fire1";
}

char *autoshotgunInfo::getIdleSeq(sharedEdict_t &ent)
{
	int rVal = ClientServerRand(0, 130);

	if(!strcmp(ent.inv->getAnimInfo()->getSeqName(), "ready"))
	{	
		// Special idle that only ever follows the ready animation.

		wAnim(ent)->setInteresting(1);
		return "ready_idle";
	}

	if(wAnim(ent)->getLastWasInteresting())
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}

	if(rVal < 10)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_a";
	}
	else if(rVal < 25)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_b";
	}
	else
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}
	return "idle";
}

char *autoshotgunInfo::getAltFireSeq(sharedEdict_t &ent)
{
	return "fire2";
}

char *autoshotgunInfo::getDryfireSeq(sharedEdict_t &ent)
{
	return "dryfire";
}

char *autoshotgunInfo::getReloadSeq(sharedEdict_t &ent)
{
	return "reload";
}

int autoshotgunInfo::handleEOS(sharedEdict_t &ent)
{
	// Reset to normal speed, because the autoshot's mainfire is slowed.
//	ent.inv->getViewModel()->SetSpeed(gsOne);

	return 0;
}

char *autoshotgunInfo::getTakeOutSeq(sharedEdict_t &ent)
{
	return "ready";
}

char *autoshotgunInfo::getPutAwaySeq(sharedEdict_t &ent)
{
	return "done";
}

//----------------------------------------------------------------------------------------------------

shotgunInfo::shotgunInfo(weaponInfoExtra_t *w):weaponInfo_c(w){}

void shotgunInfo::precacheResources(void)
{
	weaponInfo_c::precacheResources();
	pisv.EffectIndex("weapons/playermz/shotgun");
}


char *shotgunInfo::getFireSeq(sharedEdict_t &ent)
{
	return "fire1";
}

char *shotgunInfo::getDryfireSeq(sharedEdict_t &ent)
{
	return "dryfire";
}

char *shotgunInfo::getIdleSeq(sharedEdict_t &ent)
{
	int rVal = ClientServerRand(0, 100);

	if(wAnim(ent)->getLastWasInteresting())
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}

	if(rVal < 10)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_a";
	}
	else
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}
	return "idle";
}

char *shotgunInfo::getAltFireSeq(sharedEdict_t &ent)
{
	return "fire2";
}

char *shotgunInfo::getReloadSeq(sharedEdict_t &ent)
{
	return "reload_1";
}

int	shotgunInfo::handleEOS(sharedEdict_t &ent)
{
	if(!strcmp(wAnim(ent)->getSeqName(), "reload_loop"))
	{
		if(inven(ent)->refillCurClip(inven(ent)->getCurSlot(), 0, 0))
		{
			RunVWeapAnim(&ent, "reload_loop", IGhoulInst::Hold);			
			
			// Looping reload sequence can be interrupted at any time.
			inven(ent)->setInputStatus(1);
		}
		else
		{
			RunVWeapAnim(&ent, "reload_2", IGhoulInst::Hold);
		}
		return 1;
	}
	else if(!strcmp(wAnim(ent)->getSeqName(), "reload_1"))
	{
		RunVWeapAnim(&ent, "reload_loop", IGhoulInst::Hold);
		return 1;
	}

	return 0;
}

char *shotgunInfo::getTakeOutSeq(sharedEdict_t &ent)
{
	return "ready";
}

char *shotgunInfo::getPutAwaySeq(sharedEdict_t &ent)
{
	return "done";
}

int shotgunInfo::useAmmo1(sharedEdict_t &ent, int *skipIdle)
{
	if(!inven(ent)->rulesDoWeaponsUseAmmo())
		return 1;

	if(!inven(ent)->getCurClip())
	{
		if(inven(ent)->rulesIsWeaponReloadAutomatic())
			inven(ent)->reload();
		else
			inven(ent)->dryfire();

		return 0;
	}

	inven(ent)->setCurClip(inven(ent)->getCurClip() - 1);
	return 1;
}

//----------------------------------------------------------------------------------------------------

machinegunInfo::machinegunInfo(weaponInfoExtra_t *w):weaponInfo_c(w){}

void machinegunInfo::precacheResources(void)
{
	weaponInfo_c::precacheResources();
	pisv.EffectIndex("weapons/playermz/machinegun");
}


char *machinegunInfo::getFireSeq(sharedEdict_t &ent)
{
	wAnim(ent)->setFirstShot(1);//great

	// Set the firing speed, which might be slower or fast on some weapons.
//	ent.inv->getViewModel()->SetSpeed(getFireSpeed());

	return "fire1_1";
}

char *machinegunInfo::getDryfireSeq(sharedEdict_t &ent)
{
	return "dryfire";
}

char *machinegunInfo::getIdleSeq(sharedEdict_t &ent)
{
	int rVal = ClientServerRand(0, 100);

	if(wAnim(ent)->getLastWasInteresting())
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}

	if(rVal < 20)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_a";
	}
	else if(rVal < 40)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_b";
	}
	else
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}
	return "idle_a";
}

char *machinegunInfo::getAltFireSeq(sharedEdict_t &ent)
{

	// Reset to normal speed, because the machinegun's mainfire is accelerated.
//	ent.inv->getViewModel()->SetSpeed(gsOne);
	
	return "fire2";
}

char *machinegunInfo::getReloadSeq(sharedEdict_t &ent)
{

	// Reset to normal speed, because the machinegun's mainfire is accelerated.
//	ent.inv->getViewModel()->SetSpeed(gsOne);
	
	return "reload";
}

char *machinegunInfo::getTakeOutSeq(sharedEdict_t &ent)
{
	return "ready";
}

int machinegunInfo::handleEOS(sharedEdict_t &ent)
{
	if(!strcmp(wAnim(ent)->getSeqName(), "fire1_1"))
	{
		wAnim(ent)->setFirstShot(1);//great

		if(!ent.attack)
		{
			RunVWeapAnim(&ent, "fire1_2", IGhoulInst::Hold);
			return 1;
		}

		// Reset to normal speed, because the machinegun's mainfire is accelerated.
//		ent.inv->getViewModel()->SetSpeed(gsOne);
		
	}
	else if(!strcmp(wAnim(ent)->getSeqName(), "fire1_2"))
	{
		inven(ent)->idle();

		// Reset to normal speed, because the machinegun's mainfire is accelerated.
//		ent.inv->getViewModel()->SetSpeed(gsOne);
		
	}

	return 0;
}

char *machinegunInfo::getPutAwaySeq(sharedEdict_t &ent)
{
	return "done";
}

void machinegunInfo::handleSpecialNotes(char *string, sharedEdict_t &ent)
{
	IGhoulInst	*inst=(!isClient)?ent.edict->client->ps.gun:ent.inv->getViewModel();
	IGhoulObj	*obj;
	GhoulID		part;

	if(!inst)return;

	obj = inst->GetGhoulObject();
	
	if(!obj)return;

	int clip;

	if(inven(ent)->rulesDoWeaponsUseAmmo())
		clip=inven(ent)->getCurClip();
	else
		clip=8;

	for(int i=1; (i<9)&&(i<=clip); i++)
	{
		char buf[256];
		sprintf(buf, "BULLET%d", i);

		part = obj->FindPart(buf);
		if(part)inst->SetPartOnOff(part, true);
	}
}

int machinegunInfo::atMomentOfFire(sharedEdict_t &ent)
{
	float curTime;

	if(!isClient)
		curTime = *pisv.levelTime;
	else
		curTime = *picl.levelTime;

	if(wAnim(ent)->getlastShootTime() >= curTime - .195)
	{
		return 0;
	}
	wAnim(ent)->setlastShootTime(curTime);


	if(wAnim(ent)->getAnimType() == WANIM_FIRE)
	{
		int val = wAnim(ent)->firstShot();
		wAnim(ent)->setFirstShot(0);
		if(val)
		{
			return 1;
		}

		inven(ent)->getAnimInfo()->incFireLoop();
		return (inven(ent)->getCurWeapon()->handleAmmo(ent, 0));
	}
	else
	{
		return 1;
	}
}

int machinegunInfo::testInput(sharedEdict_t &ent)
{
	if(wAnim(ent)->getAnimType() == WANIM_FIRE)
	{
		if(!ent.attack)
		{
			// Stopped firing - interrupt anim.

			inven(ent)->idle();
		}
	}

	return 1;
}


//----------------------------------------------------------------------------------------------------

rocketInfo::rocketInfo(weaponInfoExtra_t *w):weaponInfo_c(w){}

void rocketInfo::precacheResources(void)
{
	weaponInfo_c::precacheResources();
	pisv.EffectIndex("weapons/playermz/rocket");
	pisv.EffectIndex("weapons/playermz/rocket_alt");
}


char *rocketInfo::getFireSeq(sharedEdict_t &ent)
{
	return "fire1";
}

char *rocketInfo::getIdleSeq(sharedEdict_t &ent)
{
	int rVal = ClientServerRand(0, 100);

	if(wAnim(ent)->getLastWasInteresting())
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}

	if(rVal < 10)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_a";
	}
	else if(rVal < 20)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_b";
	}
	else
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}
	return "idle";
}

char *rocketInfo::getAltFireSeq(sharedEdict_t &ent)
{
	wAnim(ent)->setFirstShot(1);
	return "fire2";
}

char *rocketInfo::getDryfireSeq(sharedEdict_t &ent)
{
	return "dryfire";
}

char *rocketInfo::getReloadSeq(sharedEdict_t &ent)
{
	return "reload";
}

char *rocketInfo::getTakeOutSeq(sharedEdict_t &ent)
{
	return "ready";
}

char *rocketInfo::getPutAwaySeq(sharedEdict_t &ent)
{
	return "done";
}

void rocketInfo::handleFireLoop(sharedEdict_t &ent)
{
	wAnim(ent)->setFirstShot(1);
}

int rocketInfo::atMomentOfFire(sharedEdict_t &ent)
{	//grrr.......
	float curTime;
	if(!isClient)
		curTime = *pisv.levelTime;
	else
		curTime = *picl.levelTime;

	if(wAnim(ent)->getlastShootTime() == curTime)
	{
		return 0;
	}
	wAnim(ent)->setlastShootTime(curTime);


	if(wAnim(ent)->getAnimType() == WANIM_ALTFIRE)
	{
		int val = wAnim(ent)->firstShot();
		wAnim(ent)->setFirstShot(0);
		if(val)return 1;

		inven(ent)->getAnimInfo()->incFireLoop();
		return (inven(ent)->getCurWeapon()->handleAmmo(ent, 1));
	}
	else
	{
		return 1;
	}
}


//----------------------------------------------------------------------------------------------------

microInfo::microInfo(weaponInfoExtra_t *w):weaponInfo_c(w){}

void microInfo::precacheResources(void)
{
	weaponInfo_c::precacheResources();
	pisv.EffectIndex("weapons/playermz/mpg");
}


char *microInfo::getFireSeq(sharedEdict_t &ent)
{
	if(!strcmp(wAnim(ent)->getSeqName(), "fire2"))
	{
		return "fire2_loop";
	}
	if(!strcmp(wAnim(ent)->getSeqName(), "fire2_loop"))
	{
		return "fire2_loop";
	}
	return "fire2";
}

char *microInfo::getIdleSeq(sharedEdict_t &ent)
{
	int rVal = ClientServerRand(0, 100);

	if(wAnim(ent)->getLastWasInteresting())
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}

	if(rVal < 15)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_a";
	}
	else if(rVal < 30)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_b";
	}
	else
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}
	return "idle";
}

char *microInfo::getAltFireSeq(sharedEdict_t &ent)
{
	return "fire1";
}

char *microInfo::getReloadSeq(sharedEdict_t &ent)
{
	return "reload";
}

char *microInfo::getDryfireSeq(sharedEdict_t &ent)
{
	return "dryfire";
}

int microInfo::handleEOS(sharedEdict_t &ent)
{
	if(!strcmp(ent.inv->getAnimInfo()->getSeqName(), "fire1"))
	{
		RunVWeapAnim(&ent, "charge", IGhoulInst::Hold);
		return 1;
	}
	return 0;
}

char *microInfo::getTakeOutSeq(sharedEdict_t &ent)
{
	return "ready";
}

char *microInfo::getPutAwaySeq(sharedEdict_t &ent)
{
	return "done";
}

int microInfo::testInput(sharedEdict_t &ent)
{
	if(wAnim(ent)->getAnimType() == WANIM_FIRE)
	{
		if(!ent.attack)
		{
			// Stopped firing - interrupt anim.

			inven(ent)->idle();
			RunVWeapAnim(&ent, "fire2_end", IGhoulInst::Hold);
		}
/*		else if(!strcmp(ent.inv->getAnimInfo()->getSeqName(), "fire2_loop"))
		{	
			// Still looping through this - should fire the weak beam.
			// FIXME:Does this use ammo?

			if(!isClient)
				pisv.AltfireHelper(ent, *ent.edict, *ent.inv);
		}
*/
	}

	return 1;
}

int microInfo::useAmmo2(sharedEdict_t &ent, int *skipIdle)
{
	if(!inven(ent)->rulesDoWeaponsUseAmmo())
		return 1;

	if(inven(ent)->getCurClip() < 10 )
	{
		if(inven(ent)->rulesIsWeaponReloadAutomatic())
		{
			inven(ent)->reload();
			if(skipIdle)
			{
				// This makes the relaod work right for the grenades.

				//*skipIdle = 1;
			}
		}
		else
		{
			inven(ent)->dryfire();
		}

		return 0;
	}

	inven(ent)->setCurClip(inven(ent)->getCurClip() - 10);
	return 1;
}

/*
void microInfo::frameUpdate(sharedEdict_t &ent)
{
	static float lastidle=0;
	float	difftime;

	if(wAnim(ent)->getAnimType() == WANIM_NORMAL)
	{
		if (isClient)
		{
			difftime = *picl.levelTime - lastidle;
			if (difftime < -1.5)
			{
				difftime = 1.6;
			}

			if (difftime > 1.5)
			{
				picl.WeaponEffectHelper("weapons/playermz/mpgcharge flash");
			}
			else
			{
				picl.WeaponEffectHelper("weapons/playermz/mpgcharge flash", (difftime/1.5));
			}
			lastidle = *picl.levelTime;

			if(difftime > 1.35)
			{
				float tVal = difftime - 1.35;//this should be the amount of time idling

				if(!((int)(tVal*10)%7))//eh?
				{
					picl.WeapSoundHelper ("weapons/mpg/idle.wav");
				}
			}

			if(difftime < 0.2)
			{
	//			picl.WeapSoundHelper (NULL);
			}
		}
	}
}
*/

//----------------------------------------------------------------------------------------------------

flamegunInfo::flamegunInfo(weaponInfoExtra_t *w):weaponInfo_c(w){}

void flamegunInfo::precacheResources(void)
{
	weaponInfo_c::precacheResources();
}


char *flamegunInfo::getFireSeq(sharedEdict_t &ent)
{
	if(!strcmp(wAnim(ent)->getSeqName(), "fire1_1"))
	{
		return "fire1_idle";
	}
	else if(!strcmp(wAnim(ent)->getSeqName(), "fire1_idle"))
	{
		return "fire1_idle";
	}

	if (!isClient)
	{
   		ent.edict->s.sound = pisv.SoundIndex("weapons/flamegun/fire.wav");
   		ent.edict->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
	}

	return "fire1_1";
}

char *flamegunInfo::getIdleSeq(sharedEdict_t &ent)
{
	int rVal = ClientServerRand(0, 100);

	// Make sure to kill any sounds that are engaged during the idle.
	if(!isClient)
	{
   		// If we have the CTF flag, then resume the klaxon.
   		if (pisv.DoWeHaveTheFlag(ent.edict))
   		{
   			ent.edict->s.sound = pisv.SoundIndex("player/flaglp.wav");
   			ent.edict->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
   		}
   		else
   		{
   			ent.edict->s.sound = 0;
   			ent.edict->s.sound_data = 0;
   		}
	}
	
	if(wAnim(ent)->getLastWasInteresting())
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}

	if(rVal < 10)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_a";
	}
	else if(rVal < 20)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_b";
	}
	else if(rVal < 30)
	{
		wAnim(ent)->setInteresting(1);
		return "idlefinger_c";
	}
	else
	{
		wAnim(ent)->setInteresting(0);
		return "idle";
	}

	return "idle_a";
}

char *flamegunInfo::getAltFireSeq(sharedEdict_t &ent)
{
	return "fire2_1";
}

char *flamegunInfo::getReloadSeq(sharedEdict_t &ent)
{
	return "reload_a";//doesn't happen
}

char *flamegunInfo::getTakeOutSeq(sharedEdict_t &ent)
{
	return "ready";
}

char *flamegunInfo::getPutAwaySeq(sharedEdict_t &ent)
{
	if (!isClient)
	{
   		if (pisv.DoWeHaveTheFlag(ent.edict))
   		{
   			ent.edict->s.sound = pisv.SoundIndex("player/flaglp.wav");
   			ent.edict->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
   		}
   		else
   		{
   			ent.edict->s.sound = 0;
   			ent.edict->s.sound_data = 0;
   		}
	}

	return "done";
}

char *flamegunInfo::getDryfireSeq(sharedEdict_t &ent)
{
	return "dryfire";
}


int flamegunInfo::handleEOS(sharedEdict_t &ent)
{
	if(!strcmp(wAnim(ent)->getSeqName(), "fire2_2"))
	{
		inven(ent)->idle();
		return 1;
	}

	if(!strcmp(wAnim(ent)->getSeqName(), "fire2_1"))
	{
		RunVWeapAnim(&ent, "fire2_2", IGhoulInst::Hold);
		return 1;
	}

	if(!strcmp(wAnim(ent)->getSeqName(), "fire2_idle"))
	{
		RunVWeapAnim(&ent, "fire2_2", IGhoulInst::Hold);

		return 1;
	}

	return 0;
}

void flamegunInfo::handleFireLoop(sharedEdict_t &ent)
{
	return;
}

void flamegunInfo::handleAltfireLoop(sharedEdict_t &ent)
{
	if(!strcmp(wAnim(ent)->getSeqName(), "fire2_1"))
	{
		RunVWeapAnim(&ent, "fire2_2", IGhoulInst::Hold);
	}
	else if(!strcmp(wAnim(ent)->getSeqName(), "fire2_2"))
	{
		RunVWeapAnim(&ent, "fire2_1", IGhoulInst::Hold);
	}
}

int flamegunInfo::useAmmo1(sharedEdict_t &ent, int *skipIdle)
{
	if(!inven(ent)->rulesDoWeaponsUseAmmo())
		return 1;

	if(!inven(ent)->getCurClip())
	{
		if(!isClient)
		{
   			// If we have the CTF flag, then resume the klaxon.

   			if (pisv.DoWeHaveTheFlag(ent.edict))
   			{
   				ent.edict->s.sound = pisv.SoundIndex("player/flaglp.wav");
   				ent.edict->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
   			}
   			else
   			{
   				ent.edict->s.sound = 0;
   				ent.edict->s.sound_data = 0;
   			}
		}

		if(inven(ent)->rulesIsWeaponReloadAutomatic())
		{
			// Totally out of ammo, so change weapons (if possible).

			inven(ent)->addCommand("weaponbestsafe",true);
		}
		else
		{
			inven(ent)->dryfire();
		}

		return 0;
	}

	inven(ent)->setCurClip(inven(ent)->getCurClip() - 1);
	return 1;
}

int flamegunInfo::useAmmo2(sharedEdict_t &ent, int *skipIdle)
{
	if(!inven(ent)->rulesDoWeaponsUseAmmo())
		return 1;

	if(inven(ent)->getCurClip() < 6)
	{
		if(inven(ent)->rulesIsWeaponReloadAutomatic())
		{
			if(inven(ent)->getCurClip()<=0)
			{
				// Totally out of ammo, so change weapons (if possible).

				if(!isClient)
					inven(ent)->addCommand("weaponbestsafe",true);
			}
		}
		
		inven(ent)->dryfire();
		
		return 0;
	}

	inven(ent)->setCurClip(inven(ent)->getCurClip() - 6);
	return 1;
}

int flamegunInfo::testInput(sharedEdict_t &ent)
{
	if(wAnim(ent)->getAnimType() == WANIM_FIRE)
	{
		if(!ent.attack)
		{
			// Stopped firing - interrupt anim.

			noattack(ent);
			inven(ent)->idle();
			RunVWeapAnim(&ent, "fire1_2", IGhoulInst::Loop);
			if(!isClient)
			{
				// If we have the CTF flag, then resume the klaxon.

				if (pisv.DoWeHaveTheFlag(ent.edict))
				{
					ent.edict->s.sound = pisv.SoundIndex("player/flaglp.wav");
					ent.edict->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
				}
				else
				{
					ent.edict->s.sound = 0;
					ent.edict->s.sound_data = 0;
				}
			}
			else
			{
				// No sound generated on the client anymore.
				// picl.WeapSoundKill();
			}
		}
/*		else
		{
			if(!isClient)
			{
				ent.edict->s.sound = pisv.SoundIndex("Weapons/flamegun/Fire.wav");
				ent.edict->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
			}
		}
*/
	}

	if(!ent.attack)
	{
		// Stopped firing - interrupt anim.

		/*
		if(!isClient)
		{
   			// If we have the CTF flag, then resume the klaxon.

   			if (pisv.DoWeHaveTheFlag(ent.edict))
   			{
   				ent.edict->s.sound = pisv.SoundIndex("player/flaglp.wav");
   				ent.edict->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
   			}
   			else
   			{
   				ent.edict->s.sound = 0;
   				ent.edict->s.sound_data = 0;
   			}
		}
		*/

//		picl.WeapSoundKill();
	}


	return 1;
}


/*
void flamegunInfo::frameUpdate(sharedEdict_t &ent)
{
	if(wAnim(ent)->getAnimType() == WANIM_FIRE)
	{
		if (!isClient)
		{
			pisv.FireHelper(ent, *(ent.edict), *(ent.inv));
		}
	}
}
*/



//----------------------------------------------------------------------------------------------------------
int pistol2Info::testInput(sharedEdict_t &ent)
{
	// The two extra buttons will let the player swap weapon hands.

	if(!inven(ent)->getInputStatus())return 1;
	if(wAnim(ent)->getJustTransitioned())return 1;

	if(ent.weap3)
	{
		if(!(wAnim(ent)->getHandSide()))
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setWeaponLifted(0);
			wAnim(ent)->setHandSide(1);
			RunVWeapAnim(&ent, "idle_a_to_idle_c", IGhoulInst::Hold);
			return 1;
		}

	}
	if(ent.weap4)
	{
		if(wAnim(ent)->getHandSide() == 1)
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setHandSide(0);
			wAnim(ent)->setWeaponLifted(1);
			RunVWeapAnim(&ent, "idle_c_to_idle_a", IGhoulInst::Hold);
		}
	}
	return 1;
}

int knifeInfo::testInput(sharedEdict_t &ent)
{
	if(!inven(ent)->getInputStatus())return 1;

	if(ent.weap3)
	{
		if(!strcmp(wAnim(ent)->getSeqName(), "idlefinger_1"))return 1;
		if(!strcmp(wAnim(ent)->getSeqName(), "idleclean"))return 1;
		RunVWeapAnim(&ent, "idlefinger_1", IGhoulInst::Hold);

		return 1;
	}
	if(ent.weap4)
	{
		if(!strcmp(wAnim(ent)->getSeqName(), "idlefinger_1"))return 1;
		if(!strcmp(wAnim(ent)->getSeqName(), "idleclean"))return 1;
		RunVWeapAnim(&ent, "idleclean", IGhoulInst::Hold);
		return 1;
	}
	return 1;
}

void sniperInfo::shutdownWeapon(sharedEdict_t &ent)
{
	ent.edict->client->ps.fov = 95;
	wAnim(ent)->setInZoomView(0);
	wAnim(ent)->setInZoom(0);
	wAnim(ent)->setRezoom(0);
}

int pistol1Info::testInput(sharedEdict_t &ent)
{
	if(wAnim(ent)->getAnimType() == WANIM_ALTFIRE)
	{
		if(!strcmp(wAnim(ent)->getSeqName(), "fire2_b"))
		{
			if(!ent.altattack)
			{
				// Stopped firing - interrupt anim.

				inven(ent)->idle();
			}
		}
	}

	// The two extra buttons will let the player swap weapon hands.

	if(!inven(ent)->getInputStatus())return 1;
	if(wAnim(ent)->getJustTransitioned())return 1;

	if(ent.weap3)
	{
		if((wAnim(ent)->getHandsInUse() == 1)||(wAnim(ent)->getWeaponLifted()))
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setHandsInUse(2);
			wAnim(ent)->setWeaponLifted(0);
			RunVWeapAnim(&ent, "idle_a_to_idle_b", IGhoulInst::Hold);
			return 1;
		}		
	}
	if(ent.weap4)
	{
		if(wAnim(ent)->getHandsInUse() == 2)
		{
			wAnim(ent)->setInteresting(1);
			wAnim(ent)->setTransition(1);
			wAnim(ent)->setHandsInUse(1);
			RunVWeapAnim(&ent, "idle_b_to_idle_a", IGhoulInst::Hold);
		}
	}

	return 1;
}

int sniperInfo::testInput(sharedEdict_t &ent)
{
	float zoomVal = 0;

	if(ent.rejectSniper)
	{
		if(ent.edict->client->ps.fov < 95)
		{
			IGhoulInst	*gun;

			if(!isClient)
				gun = ent.edict->client->ps.gun;
			else
				gun = ent.inv->getViewModel();

			if(gun)
			{
				gun->SetAllPartsOnOff(true);
			}

			ent.edict->client->ps.fov = 95;
			wAnim(ent)->setRezoom(0);
			wAnim(ent)->setInZoomView(0);
		}

		return 1;
	}

	if(wAnim(ent)->getInZoomView())
	{
		if(ent.weap3)
		{
			zoomVal += 2*ent.framescale;
		}

		if(ent.weap4)
		{
			zoomVal -= 2*ent.framescale;
		}
	}

	if((wAnim(ent)->getInZoom())&&(wAnim(ent)->getInZoomView()))
	{
		if(!ent.altattack)
		{
			wAnim(ent)->setInZoom(0);
		}
		else
		{
			zoomVal += 5*ent.framescale;
		}
	}
	
	if(!zoomVal)return 1;

	if (zoomVal > 0)
		ent.edict->client->ps.fov -= 5*ent.framescale;
	else
		ent.edict->client->ps.fov += 5*ent.framescale;
		
	if (ent.edict->client->ps.fov < 6)
	{
		ent.edict->client->ps.fov = 6;
	}

	if (ent.edict->client->ps.fov > 60)
	{
		ent.edict->client->ps.fov = 60;
	}

	wAnim(ent)->setRezoom(ent.edict->client->ps.fov);

	return 1;
}

int shotgunInfo::testInput(sharedEdict_t &ent)
{
	if(!inven(ent)->getInputStatus())return 1;

	if(ent.weap3)
	{
		if(!strcmp(wAnim(ent)->getSeqName(), "idlefinger_b"))return 1;
		if(!strcmp(wAnim(ent)->getSeqName(), "idlefinger_c"))return 1;
		RunVWeapAnim(&ent, "idlefinger_b", IGhoulInst::Hold);

		return 1;
	}
	if(ent.weap4)
	{
		if(!strcmp(wAnim(ent)->getSeqName(), "idlefinger_b"))return 1;
		if(!strcmp(wAnim(ent)->getSeqName(), "idlefinger_c"))return 1;
		RunVWeapAnim(&ent, "idlefinger_c", IGhoulInst::Hold);
		return 1;
	}
	return 1;
}

void flamegunInfo::shutdownWeapon(sharedEdict_t &ent)
{
	if (!isClient)
	{
   		ent.edict->s.sound = 0;
   		ent.edict->s.sound_data = 0;
	}
}

void flamegunInfo::noattack(sharedEdict_t &ent)
{
}

int knifeInfo::useAmmo2(sharedEdict_t &ent, int *skipIdle)
{
	if(inven(ent)->rulesDoWeaponsUseAmmo())
	{
		if(inven(ent)->getAmmoAmount(AMMO_KNIFE) <= 1)
		{
			wAnim(ent)->setKnifeStatus(0);
			
			// All because of the split format of the sequences here.

			return 0;
		}
	}
	
	wAnim(ent)->setKnifeStatus(1);

	if(inven(ent)->rulesDoWeaponsUseAmmo())
		inven(ent)->setAmmoAmount(AMMO_KNIFE, inven(ent)->getAmmoAmount(AMMO_KNIFE)-1);

	return 1;
}

int autoshotgunInfo::useAmmo2(sharedEdict_t &ent, int *skipIdle)
{
	if(!inven(ent)->rulesDoWeaponsUseAmmo())
		return 1;

	if(inven(ent)->getCurClip() < 2)
	{
		if(inven(ent)->rulesIsWeaponReloadAutomatic())
		{
			inven(ent)->reload();
			if(skipIdle)
			{
				// This makes the relaod work right for the grenades.

				//*skipIdle = 1;
			}
		}
		else
		{
			inven(ent)->dryfire();
		}
			
		return 0;
	}

	inven(ent)->setCurClip(inven(ent)->getCurClip() - 2);
	return 1;
}

void machinegunInfo::turnOffAmmoParts(sharedEdict_t &ent,int clipLeft,qboolean forceAll)
{
	IGhoulInst	*inst=(!isClient)?ent.edict->client->ps.gun:ent.inv->getViewModel();

	if(!inst)
		return;

	IGhoulObj	*obj=inst->GetGhoulObject();

	if(!obj)
		return;

	char	buf[32];
	GhoulID	part;

	if((clipLeft>=0)&&(clipLeft<=7))
	{
		if((!isClient)&&(!forceAll))
		{	
			sprintf(buf, "BULLET%d", clipLeft + 1);

			if(part=obj->FindPart(buf))	
				inst->SetPartOnOff(part, false);
		}
		else
		{
			for(;clipLeft<8;clipLeft++)
			{
				sprintf(buf, "BULLET%d", clipLeft + 1);

				if(part=obj->FindPart(buf))	
					inst->SetPartOnOff(part, false);
			}
		}
	}
}

int machinegunInfo::useAmmo1(sharedEdict_t &ent, int *skipIdle)
{
	int retVal=weaponInfo_c::useAmmo1(ent);

	if(inven(ent)->rulesDoWeaponsUseAmmo())
		turnOffAmmoParts(ent,inven(ent)->getCurWeapon()->getClip());

	return(retVal);	
}

int machinegunInfo::useAmmo2(sharedEdict_t &ent, int *skipIdle)
{
	if(!inven(ent)->rulesDoWeaponsUseAmmo())
		return 1;

	if(inven(ent)->getCurClip() < 10)
	{
		if(inven(ent)->rulesIsWeaponReloadAutomatic())
		{
			inven(ent)->reload();
			if(skipIdle)
			{
				// This makes the relaod work right for the grenades.

				//*skipIdle = 1;
			}
		}
		else
		{
			inven(ent)->dryfire();
		}
		
		return 0;
	}

	inven(ent)->setCurClip(inven(ent)->getCurClip() - 10);
	return 1;
}

void machinegunInfo::handleFall(sharedEdict_t &ent)
{
	if(inven(ent)->getInputStatus())
	{
		RunVWeapAnim(&ent, "land", IGhoulInst::Loop);
	}
}


//----------------------------------------------------------------------------------------------------------

hurthandInfo::hurthandInfo(weaponInfoExtra_t *w):weaponInfo_c(w){}

int hurthandInfo::handleEOS(sharedEdict_t &ent)
{
	if(!strcmp(wAnim(ent)->getSeqName(), "hurthand"))
	{
		if(!isClient)
		{
			if(!strcmp(inven(ent)->rulesGetBestWeaponMode(),""))
				inven(ent)->addCommand("weapnext",true);
			else
				inven(ent)->takeOutBestWeapon();
		}

		RunVWeapAnim(&ent, getIdleSeq(ent), IGhoulInst::Hold);
		inven(ent)->setInputStatus(-1);
	}
	else
		inven(ent)->idle();

	return 1;
}

char *hurthandInfo::getIdleSeq(sharedEdict_t &ent)
{
	return NULL;
}

char *hurthandInfo::getTakeOutSeq(sharedEdict_t &ent)
{
	return "hurthand";
}

//----------------------------------------------------------------------------------------------------------

throwhandInfo::throwhandInfo(weaponInfoExtra_t *w):weaponInfo_c(w){}

int throwhandInfo::handleEOS(sharedEdict_t &ent)
{
	if(!strcmp(wAnim(ent)->getSeqName(), "throwhand"))
	{
		if(!isClient)
		{
			if(!strcmp(inven(ent)->rulesGetBestWeaponMode(),""))
				inven(ent)->addCommand("weapnext",true);
			else
				inven(ent)->takeOutBestWeapon();
		}

		RunVWeapAnim(&ent, getIdleSeq(ent), IGhoulInst::Hold);
		inven(ent)->setInputStatus(-1);
	}
	else
		inven(ent)->idle();

	return 1;
}

char *throwhandInfo::getIdleSeq(sharedEdict_t &ent)
{
	return NULL;
}

char *throwhandInfo::getTakeOutSeq(sharedEdict_t &ent)
{
	return "throwhand";
}