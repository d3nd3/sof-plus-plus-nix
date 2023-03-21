// *****************************************************************************
// dm_standard.cpp
// *****************************************************************************

#include "g_local.h"
#include "dm_private.h"
#include "w_weapons.h"

const int DF_STANDARD_NO_HEALTH			= (1<<0);
const int DF_STANDARD_NO_ITEMS			= (1<<1);
const int DF_STANDARD_WEAPONS_STAY		= (1<<2);
const int DF_STANDARD_NO_FALLING		= (1<<3);
const int DF_STANDARD_REALISTIC_DAMAGE	= (1<<4);
const int DF_STANDARD_SAME_LEVEL		= (1<<5);
const int DF_STANDARD_NOWEAPRELOAD		= (1<<6);
const int DF_STANDARD_TEAMPLAY			= (1<<7);
const int DF_STANDARD_NO_FRIENDLY_FIRE	= (1<<8);
const int DF_STANDARD_FORCE_RESPAWN		= (1<<9);
const int DF_STANDARD_NO_ARMOR			= (1<<10);
const int DF_STANDARD_INFINITE_AMMO		= (1<<11);
const int DF_STANDARD_SPINNINGPICKUPS	= (1<<12);
const int DF_STANDARD_BULLET_WPNS_ONLY	= (1<<13);

/*
==================
dmstandard_c::getGameName
==================
*/

char *dmstandard_c::getGameName(void)
{
	if(dmRule_TEAMPLAY())
		return("TeamPlay DM");

	return("standard DM");
}

/*
==================
dmstandard_c::levelInit
==================
*/

void dmstandard_c::levelInit(void)
{
	// Make certain all the weapons given to clients at respawn are precached.

	// View weapons.

	AddWeaponTypeForPrecache(SFW_KNIFE);
	AddWeaponTypeForPrecache(SFW_PISTOL1);
	AddWeaponTypeForPrecache(SFW_HURTHAND);
	AddWeaponTypeForPrecache(SFW_THROWHAND);

	// Bolt-ons / pickups. Eugh... but necessary to precache. Knife doesn't
	// need to be precached for some reason - it's always there.. hmmm.

	edict_t *ent=G_Spawn();
	
	//I_Spawn(ent,thePickupList.GetPickupFromType(PU_WEAPON,SFW_KNIFE));
	I_Spawn(ent,thePickupList.GetPickupFromType(PU_WEAPON,SFW_PISTOL1));
	
	G_FreeEdict(ent);
}

/*
==================
dmstandard_c::checkItemSpawn
==================
*/

int	dmstandard_c::checkItemSpawn(edict_t *ent,Pickup **pickup)
{

	if ((*pickup)->GetPickupListIndex() == OBJ_CTF_FLAG)
	{
		G_FreeEdict (ent);
		return(0);
	}

	if(dmRule_NO_HEALTH())
	{
		if ((*pickup)->GetType() == PU_HEALTH)
		{
			G_FreeEdict (ent);
			return(0);
		}
	}

	if(dmRule_NO_ITEMS())
	{
		if ((*pickup)->GetType() == PU_INV)
		{
			G_FreeEdict (ent);
			return(0);
		}
	}

	if(dmRule_NO_ARMOR())
	{
		if ((*pickup)->GetType() == PU_ARMOR)
		{
			G_FreeEdict (ent);
			return(0);
		}
	}
	
	if(dmRule_INFINITE_AMMO())
	{
		if ((*pickup)->GetType() == PU_AMMO)
		{
			G_FreeEdict (ent);
			return(0);
		}
	}

	if(dmRule_BULLET_WPNS_ONLY())
	{
		if ( ((*pickup)->GetType() == PU_WEAPON) && (!(*pickup)->IsBulletWpn()) ||
			 ((*pickup)->GetType() == PU_AMMO) && (!(*pickup)->IsBulletAmmo())	)
		{
			G_FreeEdict (ent);
			return(0);
		}
	}

	return(-1);
}

/*
==================
dmstandard_c::dmRule_xxx
==================
*/

int	dmstandard_c::dmRule_NO_HEALTH(void)
{
	return((int)dmflags->value&DF_STANDARD_NO_HEALTH);
}

int	dmstandard_c::dmRule_NO_ITEMS(void)
{
	return((int)dmflags->value&DF_STANDARD_NO_ITEMS);
}

int	dmstandard_c::dmRule_WEAPONS_STAY(void)
{
	return((int)dmflags->value&DF_STANDARD_WEAPONS_STAY);
}

int	dmstandard_c::dmRule_NO_FALLING(void)
{
	return((int)dmflags->value&DF_STANDARD_NO_FALLING);
}

int	dmstandard_c::dmRule_REALISTIC_DAMAGE(void)
{
	return((int)dmflags->value&DF_STANDARD_REALISTIC_DAMAGE);
}

int	dmstandard_c::dmRule_SAME_LEVEL(void)
{
	return((int)dmflags->value&DF_STANDARD_SAME_LEVEL);
}

int	dmstandard_c::dmRule_NOWEAPRELOAD(void)
{
	return((int)dmflags->value&DF_STANDARD_NOWEAPRELOAD);
}

int	dmstandard_c::dmRule_TEAMPLAY(void)
{
	return((int)dmflags->value&DF_STANDARD_TEAMPLAY);
}

int	dmstandard_c::dmRule_NO_FRIENDLY_FIRE(void)
{
	return((int)dmflags->value&DF_STANDARD_NO_FRIENDLY_FIRE);
}

int	dmstandard_c::dmRule_FORCE_RESPAWN(void)
{
	return((int)dmflags->value&DF_STANDARD_FORCE_RESPAWN);
}

int	dmstandard_c::dmRule_NO_ARMOR(void)
{
	return((int)dmflags->value&DF_STANDARD_NO_ARMOR);
}

int	dmstandard_c::dmRule_INFINITE_AMMO(void)
{
	return((int)dmflags->value&DF_STANDARD_INFINITE_AMMO);
}

int	dmstandard_c::dmRule_SPINNINGPICKUPS(void)
{
	return((int)dmflags->value&DF_STANDARD_SPINNINGPICKUPS);
}

int	dmstandard_c::dmRule_BULLET_WPNS_ONLY(void)
{
	return((int)dmflags->value&DF_STANDARD_BULLET_WPNS_ONLY);
}

/*
==================
dmstandard_c::clientCalculateWaver
==================
*/

float dmstandard_c::clientCalculateWaver(edict_t &ent, int atkID)
{
	float		waverAmount = 0;

	// check for Sniper rifle, add waver based on zoom
	if((atkID == ATK_SNIPER)/* || (atkID == ATK_SNIPER_ALT)*/)
	{
//		This waver based on a desired maximum waver of 4.5 when not zoomed (fov == 95),
//		then a linearly decreasing slope from 2 at minimum zoom (fov == 60),
//		to no waver when maximally zoomed (fov == 6).  Thus:
//				 (CurrentFOV - FOV@MaxZoom) * MaxWaver		(CurrentFOV - 6) * 2
//		waver = --------------------------------------- = --------------------------
//				     (FOV@MinZoom - FOV@MaxZoom)				  (60 - 6) 
	
		if(ent.client->ps.fov > 60)
		{
			waverAmount = 4.5;
		}
		else
		{
			waverAmount = 0;//((ent.client->ps.fov - 6) * .037);
		}
	}

	return waverAmount;
}


/*
==================
dmstandard_c::clientDie
==================
*/

void dmstandard_c::clientDie(edict_t &ent, edict_t &inflictor, edict_t &killer)
{
	// Lose a frag?

	if((!killer.client)||(&killer==&ent))
	{	
		// I was killed by non player or killed myself.


		if (&killer==&ent)
		{
			// the killer is me
			ent.client->resp.score -= sv_suicidepenalty->value;
		}
		else
		{
			ent.client->resp.score--;
		}

		return;
	}

	// Must've been killed by another player.

	if(meansOfDeath & MOD_FRIENDLY_FIRE)
		killer.client->resp.score--;
	else
		killer.client->resp.score++;
}

/*
==================
dmstandard_c::clientRespawn
==================
*/

void dmstandard_c::clientRespawn(edict_t &ent)
{
	sharedEdict_t	sh;

	sh.inv = (inven_c *)ent.client->inv;
	sh.edict = &ent;
	sh.inv->setOwner(&sh);

	sh.inv->clearInv(true);

	sh.inv->rulesSetWeaponsUseAmmo(dm->dmRule_INFINITE_AMMO()?0:-1);	
	sh.inv->rulesSetWeaponsReload(dm->dmRule_NOWEAPRELOAD()?0:-1);
	sh.inv->rulesSetBestWeaponMode(Info_ValueForKey(ent.client->pers.userinfo,"bestweap"));
	sh.inv->rulesSetDropWeapons(!dm->dmRule_WEAPONS_STAY());
	sh.inv->rulesSetNonDroppableWeaponTypes(1<<SFW_KNIFE);
	if(dm->dmRule_REALISTIC_DAMAGE())
		sh.inv->rulesSetNonDroppableWeaponTypes(1<<SFW_PISTOL1);

	sh.inv->addWeaponType(SFW_KNIFE);
	sh.inv->addWeaponType(SFW_PISTOL1);
	sh.inv->addWeaponType(SFW_HURTHAND);
	sh.inv->addWeaponType(SFW_THROWHAND);

	if(!dm->dmRule_INFINITE_AMMO())
	{
		sh.inv->addAmmoType(AMMO_KNIFE,6);	
		sh.inv->addAmmoType(AMMO_9MM,150);
	}

	sh.inv->selectWeapon(SFW_PISTOL1);
}