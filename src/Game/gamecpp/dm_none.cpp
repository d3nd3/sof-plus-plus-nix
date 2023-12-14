// *****************************************************************************
// dm_none.cpp
// *****************************************************************************

#include "g_local.h"
#include "dm_private.h"
#include "w_weapons.h"
#include "../strings/dm_generic.h"
#include "p_body.h"
#include "q_sh_interface.h"

extern void sendRestartPrediction(edict_t *ent);

/*
==================
dmnone_c::getMaxEncumbrance
==================
*/

int	dmnone_c::getMaxEncumbrance(void)
{
	return(game.playerSkills.getPlayerMaxEncumbrance());
}

/*
==================
dmnone_c::clientDropItem
==================
*/

void dmnone_c::clientDropItem(edict_t *ent,int type,int ammoCount)
{
	Pickup	*pickup = NULL;

	if (pickup = thePickupList.GetPickupFromType(PU_INV, type))
	{
		edict_t	*dropped;
		vec3_t	dir;

		// Whatever weapon player is holding, throw one off.

		dropped=G_Spawn();
		dropped->spawnflags|=DROPPED_ITEM;
		I_Spawn(dropped,pickup);

		dropped->enemy=ent;
		dropped->touch_debounce_time=level.time+2.0;

		AngleVectors(ent->client->ps.viewangles,dir,NULL,NULL);
		dir[2] = 0;
		VectorNormalize(dir);
		VectorScale(dir,250.0,dropped->velocity);
		VectorMA(ent->s.origin, 20.0, dir, dropped->s.origin);
		dropped->velocity[2]+=150.0;
		dropped->health = ammoCount;

	}
}

/*
==================
dmnone_c::clientDropWeapon
==================
*/

void dmnone_c::clientDropWeapon(edict_t *ent,int type, int clipSize)
{
	Pickup	*pickup = NULL;

	if (pickup = thePickupList.GetPickupFromType(PU_WEAPON, type))
	{
		edict_t		*dropped;
		vec3_t		dir;

		// Whatever weapon player is holding, throw one off.

		dropped=G_Spawn();
		dropped->spawnflags|=DROPPED_ITEM;
		I_Spawn(dropped,pickup);

		dropped->enemy=ent;
		dropped->touch_debounce_time=level.time+2.0;
		
		VectorCopy(ent->s.origin,dropped->s.origin);
		AngleVectors(ent->client->ps.viewangles,dir,NULL,NULL);
		dir[2] = 0;
		VectorNormalize(dir);


		// 2/3/00 dk -- We need to see if we can spawn the weapon at a point in front of the player.
		// If we can't, just spawn it as close as we can.  Without this test, a player with his face against the wall may end
		// up dropping it on the other side of that wall.  
		vec3_t		spawnPoint;
		trace_t		trace;
		

		VectorMA(dropped->s.origin, 20.0, dir, spawnPoint);
		dropped->owner = ent;  // don't want player to interfere with trace
		gi.trace(dropped->s.origin, dropped->mins, dropped->maxs, spawnPoint,  dropped, MASK_SOLID, &trace);
		if (trace.fraction < 1)
		{
			if (trace.startsolid) // ugh, we're probably in a corner trying to drop a weapon that won't fit.
								  // well, try behind us a bit
			{
				vec3_t		backPoint;

				VectorMA(dropped->s.origin, -10, dir, backPoint);
				gi.trace(backPoint, dropped->mins, dropped->maxs, dropped->s.origin,  dropped, MASK_SOLID, &trace);
				VectorMA(backPoint, 10.0 * trace.fraction, dir, dropped->s.origin);
			}
			else
			{
				VectorMA(dropped->s.origin, 20.0 * trace.fraction, dir, dropped->s.origin);
			}
		}
		else
		{
			VectorCopy(spawnPoint, dropped->s.origin);
		}
		dropped->owner = NULL;

		
		VectorScale(dir,250.0,dropped->velocity);
		dropped->velocity[2]+=150.0;
		dropped->health = clipSize;

		const char	*weapon = gi.SP_GetStringText(pickup->GetDroppedStringIndex());
		gi.SP_Print(ent,DM_GENERIC_TEXT_DROPPED_WEAPON,weapon);
	}
}

/*
==================
lazy
==================
*/

void lazy(sharedEdict_t &sh, int a)
{
	sh.inv->addWeaponType(a);
	AddWeaponTypeForPrecache(a);
}

/*
==================
DefaultWeapons
==================
*/

void DefaultWeapons(edict_t *ent)
{
	sharedEdict_t	sh;

	sh.inv=(inven_c *)ent->client->inv;

	if(!sh.inv)
	{
		gi.dprintf("hey! no inventory!\n");
		return;
	}

	sh.edict=ent;
	sh.inv->setOwner(&sh);
	sh.inv->deactivateCurrentWeapon();
	sh.inv->deactivateInventory();

	W_InitInv(*ent);
	sh.inv = (inven_c *)ent->client->inv;
	sh.edict = ent;
	sh.inv->setOwner(&sh);

	sendRestartPrediction(ent);
	sh.inv->clearInv(true);

	sh.inv->rulesSetWeaponsUseAmmo(-1);	
	sh.inv->rulesSetBestWeaponMode(Info_ValueForKey(ent->client->pers.userinfo,"bestweap"));

	sh.inv->addAmmoType(AMMO_9MM, 128);
	sh.inv->addAmmoType(AMMO_KNIFE, 6);
	sh.inv->addAmmoType(AMMO_556, 120);
	sh.inv->addAmmoType(AMMO_44, 36);
	sh.inv->addAmmoType(AMMO_SLUG, 40);
	sh.inv->addAmmoType(AMMO_SHELLS, 32);
	sh.inv->addAmmoType(AMMO_ROCKET, 8);
	sh.inv->addAmmoType(AMMO_FTHROWER, 30);
	sh.inv->addAmmoType(AMMO_MWAVE, 60);

	if(!(strncmp(level.mapname, "tsr", 3)))
	{
		lazy(sh, SFW_KNIFE);
		lazy(sh, SFW_PISTOL1);
		lazy(sh, SFW_SHOTGUN);

		sh.inv->addItem(SFE_FLASHPACK, 5);
		sh.inv->addItem(SFE_C4, 2);
	}
	else if(!(strncmp(level.mapname, "trn", 3)))
	{
		sh.inv->addItem(SFE_FLASHPACK, 5);
		sh.inv->addItem(SFE_C4, 2);

		lazy(sh, SFW_KNIFE);
		lazy(sh, SFW_PISTOL1);
		lazy(sh, SFW_SHOTGUN);
	}
	else if(!(strncmp(level.mapname, "kos", 3)))
	{
		sh.inv->addItem(SFE_FLASHPACK, 10);
		sh.inv->addItem(SFE_C4, 5);

		lazy(sh, SFW_KNIFE);
		lazy(sh, SFW_PISTOL1);
		lazy(sh, SFW_SHOTGUN);
		lazy(sh, SFW_ASSAULTRIFLE);
	}
	else if(!(strncmp(level.mapname, "sib", 3)))
	{
		sh.inv->addItem(SFE_FLASHPACK, 10);
		sh.inv->addItem(SFE_C4, 5);

		lazy(sh, SFW_KNIFE);
		lazy(sh, SFW_PISTOL1);
		lazy(sh, SFW_SNIPER);
		lazy(sh, SFW_MACHINEPISTOL);
		lazy(sh, SFW_ASSAULTRIFLE);
	}
	else if(!(strncmp(level.mapname, "irq", 3)))
	{
		if(level.mapname[4] == 'a')
		{
			sh.inv->addItem(SFE_FLASHPACK, 10);
			sh.inv->addItem(SFE_C4, 5);

			lazy(sh, SFW_KNIFE);
			lazy(sh, SFW_PISTOL1);
			lazy(sh, SFW_SNIPER);
			lazy(sh, SFW_MACHINEPISTOL);
		}
		else if(level.mapname[4] == 'b')
		{
			sh.inv->addItem(SFE_FLASHPACK, 10);
			sh.inv->addItem(SFE_C4, 5);
//			sh.inv->addItem(SFE_NEURAL_GRENADE, 3);
			sh.inv->addItem(SFE_GRENADE, 5);

			lazy(sh, SFW_KNIFE);
			lazy(sh, SFW_PISTOL1);
			lazy(sh, SFW_MACHINEGUN);
			lazy(sh, SFW_ROCKET);
		}
	}
	else if(!(strncmp(level.mapname, "nyc", 3)))
	{
		sh.inv->addItem(SFE_FLASHPACK, 10);
		sh.inv->addItem(SFE_C4, 5);
		sh.inv->addItem(SFE_GRENADE, 5);

		lazy(sh, SFW_KNIFE);
		lazy(sh, SFW_PISTOL1);
		lazy(sh, SFW_SHOTGUN);
		lazy(sh, SFW_ASSAULTRIFLE);
	}
	else if(!(strncmp(level.mapname, "sud", 3)))
	{
		sh.inv->addItem(SFE_FLASHPACK, 10);
		sh.inv->addItem(SFE_C4, 5);
		sh.inv->addItem(SFE_GRENADE, 5);

		lazy(sh, SFW_KNIFE);
		lazy(sh, SFW_PISTOL1);
		lazy(sh, SFW_PISTOL2);
		lazy(sh, SFW_SNIPER);

	}
	else if(!(strncmp(level.mapname, "jpn", 3)))
	{
		sh.inv->addItem(SFE_FLASHPACK, 10);
		sh.inv->addItem(SFE_C4, 5);
		sh.inv->addItem(SFE_GRENADE, 5);

		lazy(sh, SFW_KNIFE);
		lazy(sh, SFW_PISTOL1);
		lazy(sh, SFW_MACHINEPISTOL);
		lazy(sh, SFW_ASSAULTRIFLE);
	}
	else if(!(strncmp(level.mapname, "ger", 3)))
	{
		sh.inv->addItem(SFE_FLASHPACK, 10);
		sh.inv->addItem(SFE_C4, 5);
//		sh.inv->addItem(SFE_NEURAL_GRENADE, 3);
		sh.inv->addItem(SFE_GRENADE, 5);

		lazy(sh, SFW_KNIFE);
		lazy(sh, SFW_PISTOL1);
		lazy(sh, SFW_ROCKET);
		lazy(sh, SFW_MACHINEGUN);
	}
	else
	{
		sh.inv->addItem(SFE_FLASHPACK, 5);
		sh.inv->addItem(SFE_C4, 2);

		lazy(sh, SFW_KNIFE);
		lazy(sh, SFW_PISTOL1);
		lazy(sh, SFW_SHOTGUN);
	}
	
	lazy(sh,SFW_HURTHAND);
	lazy(sh,SFW_THROWHAND);

	sh.inv->stockWeapons();

	pe->PrecacheViewWeaponModels(level.weaponsAvailable);

	sh.inv->selectWeapon(SFW_PISTOL1);

	PB_AddArmor(ent,1000);
}

/*
==================
HandleSkillSettings

Kinda mis-named. Actually inits all the level stats.
==================
*/

void HandleSkillSettings(void)
{
	level.guysKilled = 0;
	level.friendliesKilled = 0;

	level.throatShots = 0;
	level.nutShots = 0;
	level.headShots = 0;
	level.gibs = 0;

	level.savesUsed = 0;
	level.cashEarned = 0;
	level.skillRating = 0;
	level.startTime = level.time;

	level.savesLeft = game.playerSkills.getNumberOfSaves();
}

/*
==================
specialInventory
==================
*/

qboolean specialInventory(edict_t &ent)
{
	// See SP_worldspawn... we may be setting the player's inventory depending
	// on which level we're playing.

	edict_t			*entWorldSpawn=&g_edicts[0];
	baseObjInfo_c	*objInfo=entWorldSpawn->objInfo;
	objFlipInfo_c	*flipInfo=NULL;

	if(objInfo && (flipInfo = (objFlipInfo_c*)objInfo->GetInfo(OIT_FLIP)))
	{
		if (flipInfo->radius == -1)
		{	
			// This is a "no weapons" level -- set via st.noweapons.

			if(!(strncmp(level.mapname, "tut1", 4)))
			{
				AddWeaponTypeForPrecache(SFW_SNIPER);
				AddWeaponTypeForPrecache(SFW_PISTOL2);
				
				pe->PrecacheViewWeaponModels(level.weaponsAvailable);
			}

			return(true);
		}
		else if (flipInfo->radius == -2)
		{	
			// Basic inventory -- set via st.setweapons.

			sharedEdict_t	sh;

			sh.inv=(inven_c *)ent.client->inv;
			sh.edict=&ent;
			sh.inv->setOwner(&sh);
			sh.inv->deactivateCurrentWeapon();
			sh.inv->deactivateInventory();

			sendRestartPrediction(&ent);

			sh.inv->clearInv(true);

			sh.inv->rulesSetWeaponsUseAmmo(-1);
			sh.inv->rulesSetBestWeaponMode(Info_ValueForKey(ent.client->pers.userinfo,"bestweap"));
			sh.inv->rulesSetNonDroppableWeaponTypes(1<<SFW_KNIFE);

			lazy(sh,SFW_KNIFE);
			lazy(sh,SFW_PISTOL1);
			lazy(sh,SFW_SHOTGUN);
			lazy(sh,SFW_HURTHAND);
			lazy(sh,SFW_THROWHAND);

			sh.inv->addAmmoType(AMMO_KNIFE,6);	
			sh.inv->addAmmoType(AMMO_9MM,128);
			sh.inv->addAmmoType(AMMO_SHELLS,32);

			sh.inv->addItem(SFE_C4,2);
			sh.inv->addItem(SFE_FLASHPACK,5);

			pe->PrecacheViewWeaponModels(level.weaponsAvailable);

			sh.inv->selectBestWeapon();

			PB_AddArmor(sh.edict, 100);

			return(true);
		}
	}

	return(false);
}

/*
==================
dmnone_c::clientRespawn
==================
*/

void dmnone_c::clientRespawn(edict_t &ent)
{
	ent.health=100;
	HandleSkillSettings();

	// Set up player's inventory.

	if(specialInventory(ent))
	{
		// Special case inventory setup for certain levels, e.g. tsr1, trn1,
		// where player doesn't get a chance to go to the weapon load-out
		// screen before-hand.

		return;
	}

	sharedEdict_t	sh;

	sh.inv=(inven_c *)ent.client->inv;
	sh.edict=&ent;
	sh.inv->setOwner(&sh);
	sh.inv->deactivateCurrentWeapon();
	sh.inv->deactivateInventory();

	sendRestartPrediction(&ent);

	sh.inv->clearInv(true);

	sh.inv->rulesSetWeaponsUseAmmo(-1);
	sh.inv->rulesSetBestWeaponMode(Info_ValueForKey(ent.client->pers.userinfo,"bestweap"));
	sh.inv->rulesSetNonDroppableWeaponTypes(1<<SFW_KNIFE);

	lazy(sh,SFW_HURTHAND);
	lazy(sh,SFW_THROWHAND);

	// Look to see what we've carried in with us from previous levels.
	// This can include a health value for levels that don't come from the armory.

	sh.inv->extractInvFromMenu(&level.weaponsAvailable, &ent.health);

	// On easier difficulties, the player starts with full health no matter what

	if(game.playerSkills.fullHealthEachLevel())
	{
		ent.health = 100;
	}

	// Make certain all the weapons needed in the level with me are cached.

	pe->PrecacheViewWeaponModels(level.weaponsAvailable);

	// Select a weapon...

	if(((ent.client->pers.curWeaponType==-1)||(!ent.client->pers.curWeaponType))||
	   (!sh.inv->hasWeaponType(ent.client->pers.curWeaponType)))
	{
		// ...the best weapon we have if we don't have a weapon from the prev.
		// level.

		sh.inv->selectBestWeapon();
	}
	else
	{
		// ...the weapon we were using from the last level.
		sh.inv->selectWeapon(ent.client->pers.curWeaponType);
	}
}

/*
==================
dmnone_c::clientStealthAndFatigueMeter
==================
*/

void dmnone_c::clientStealthAndFatigueMeter(edict_t *ent)
{
	ent->client->ps.stats[STAT_FLAGS] |= STAT_FLAG_SHOW_STEALTH;
	ent->client->ps.stats[STAT_STEALTH] = level.playerLoudness*100;
}

/*
==================
dmnone_c::clientGetMovescale
==================
*/

float dmnone_c::clientGetMovescale(edict_t *ent)
{
	if(game.playerSkills.getSpawnValue() == 4)
	{	
		// This game mode goes really damn fast!

		return 1.0;
	}
	else
	{
		return REALISTIC_MOVESCALE;
	}
}

/*
==================
dmnone_c::clientCalculateWaver
==================
*/

float dmnone_c::clientCalculateWaver(edict_t &ent, int atkID)
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