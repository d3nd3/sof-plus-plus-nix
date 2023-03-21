// *****************************************************************************
// dm_real.cpp
// *****************************************************************************

#include "g_local.h"
#include "dm_private.h"
#include "w_weapons.h"
#include "ai_private.h"	// so we know what a bodyhuman_c is
#include "q_sh_interface.h"

const int DF_REAL_SAME_LEVEL		= (1<<5);
const int DF_REAL_TEAMPLAY			= (1<<7);
const int DF_REAL_FORCE_RESPAWN		= (1<<9);
const int DF_REAL_SPINNINGPICKUPS	= (1<<12);
const int DF_REAL_BULLET_WPNS_ONLY	= (1<<13);

/*
=================
getPlayerDMInfo
=================
*/

static player_dmrealInfo_c *getPlayerDMInfo(const edict_t *ent)
{
	return((player_dmrealInfo_c *)ent->client->dmInfo);
}

/*
==================
IsRespawningItem
==================
*/

int	IsRespawningItem(edict_t *ent)
{	// we want to know if ent is an item that should respawn once picked up. stuff
	//that falls into this category would be ammo and...uh...ammo.
	Pickup			*pickup = NULL;
	int				i = 0;

	if (pickup = thePickupList.GetPickupFromEdict(ent))
	{
			if ( (pickup->GetType() == PU_AMMO) ||
				 (pickup->GetType() == PU_ARMOR) ||
				 (pickup->GetType() == PU_INV))
			{
				return true;
			}
			else
			{
				return false;
			}
	}
	return false;
}

/*
==================
isSpawnPointFree
==================

  is there room at this spawn point for me? if there's another item there, this
  spot ain't free. if there's either nothing or a player there, go ahead and bamf.
*/

bool isSpawnPointFree(edict_t *ent, vec3_t& searchAt)
{
	edict_t *found = NULL;
	float	radius = 128;	// should be safe
	vec3_t	absMinsItem, absMaxsItem, absMinsFound, absMaxsFound;

	CRadiusContent rad(searchAt, radius, RADAREA_SOLIDSANDTRIGGERS);
	VectorAdd(searchAt, ent->mins, absMinsItem);
	VectorAdd(searchAt, ent->maxs, absMaxsItem);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		found = rad.foundEdict(i);
		VectorAdd(found->s.origin, found->mins, absMinsFound);
		VectorAdd(found->s.origin, found->maxs, absMaxsFound);

		// does the bbox of found intersect with the bbox of ent?

		// x dimension
		if ( (absMinsFound[0] < absMaxsItem[0]) && (absMaxsFound[0] > absMinsItem[0]) &&
			 (absMinsFound[1] < absMaxsItem[1]) && (absMaxsFound[1] > absMinsItem[1]) &&
			 (absMinsFound[2] < absMaxsItem[2]) && (absMaxsFound[2] > absMinsItem[2]) )
		{
			return false;
		}
	}




	return true;
}


/*
==================
itemReappear
==================
*/

void itemReappear(edict_t *ent)
{
	ent->svflags &= ~SVF_NOCLIENT;
	ent->solid = SOLID_TRIGGER;
	ent->touch_debounce_time = 0.0;

	gi.linkentity(ent);

	// Send a respawning effect.
	fxRunner.exec("environ/irespawn", ent->s.origin);

	// 1/4/00 kef -- play a "fade in" anim. at the end of that anim, switch back to non-fade anim.
	PickupInst	*pickupInst = thePickupList.GetPickupInstFromEdict(ent);

	if (pickupInst)
	{
		pickupInst->FadeIn();
	}

	// Respawn sound. FIXME: Make part of FX? Not sure about PHS issues here tho'?
	gi.sound(ent, CHAN_ITEM, gi.soundindex("dm/irespawn.wav"), .6, ATTN_NORM, 0);

	ent->think = ClearEffects;
	ent->nextthink = level.time + ITEM_RESPAWN_FADEIN;
}


/*
==================
moveItemToSpawnPoint
==================
*/

void moveItemToSpawnPoint(edict_t *ent, vec3_t& dest)
{
	ent->svflags |= SVF_NOCLIENT;
	ent->solid = SOLID_NOT;

	// Send a respawning effect.
	fxRunner.exec("environ/irespawn", ent->s.origin);

	// actually move the thing
	VectorCopy(dest, ent->s.origin);
	ent->think = itemReappear;
	ent->nextthink = level.time + .1;
}


/*
==================
dmreal_c::~dmreal_c
==================
*/

dmreal_c::~dmreal_c()
{
	m_spawnPoints.clear();
}

/*
==================
dmreal_c::preLevelInit
==================
*/

void dmreal_c::preLevelInit(void)
{
	m_spawnPoints.clear();
}

/*
==================
dmreal_c::levelInit
==================
*/

void dmreal_c::levelInit(void)
{
	// gi.SP_Register("dm_real");
	
	// Precache sounds.

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

	// This is the amount of time that may pass without a player picking up a
	// weapon before said weapon will return to its designer-placed spawn point.
	// (in seconds...like you didn't know.)
	m_maxUntouchedTime = 30.0;
	m_itemRetryTime = 10.0;
}


/*
==================
dmreal_c::checkItemSpawn
==================
*/

int	dmreal_c::checkItemSpawn(edict_t *ent,Pickup **pickup)
{

	if ((*pickup)->GetPickupListIndex() == OBJ_CTF_FLAG)
   	{
		G_FreeEdict (ent);
		return(0);
	}

	if(dmRule_NO_ARMOR())
	{
		if ((*pickup)->GetType() == PU_ARMOR)
		{
			G_FreeEdict(ent);
			return(0);
		}
	}

	if(dmRule_BULLET_WPNS_ONLY())
	{
		if ( ((*pickup)->GetType() == PU_WEAPON) && (!(*pickup)->IsBulletWpn()) ||
			 ((*pickup)->GetType() == PU_AMMO) && (!(*pickup)->IsBulletAmmo())	)
		{
			G_FreeEdict(ent);
			return(0);
		}
	}

	if((*pickup)->GetType() == PU_HEALTH)
	{
		switch ((*pickup)->GetSpawnParm())
		{
		case SFH_LARGE_HEALTH:		// big healths become MedKits
			{
				(*pickup) = thePickupList.GetPickupFromSpawnName("item_equip_medkit");
			}
			break;
		case SFH_SMALL_HEALTH:		// no small healths
			G_FreeEdict(ent);
			return (0);
			break;
		default: 		// other health type (a Medkit, maybe?), spawn it "just in case"
			break;
		}
	}


	// need to keep track of the spawnpoints for weapons and items
	m_spawnPoints.insert(pair<int, Vect3>((*pickup)->GetPickupListIndex(), ent->s.origin));

	return(-1);
}

/*
==================
dmreal_c::clientSetDroppedWeaponThink
==================
*/

void dmreal_c::clientSetDroppedWeaponThink(edict_t *ent)
{
}

/*
==================
dmreal_c::checkItemAfterSpawn
==================

  Perform a separate check _after_ we call I_Spawn on this item.
  In dm_real any dropped weapon needs to return to its designer-placed spawn point after a
  predetermined nobody-has-picked-me-up time. To accomplish this, we'll give all dropped items
  a think and nextthink right here.
*/
int dmreal_c::checkItemAfterSpawn(edict_t *ent, Pickup *pickup)
{
	if ( !IsRespawningItem(ent) && (ent->spawnflags & DROPPED_ITEM) )
	{	// if this is one of the default weapons (knife or small pistol) treat it like
		//vanilla dm does...make it go away if it remains untouched for too long
		if ( (pickup->GetPickupListIndex() == OBJ_AMMO_KNIFE) ||
			 (pickup->GetPickupListIndex() == OBJ_AMMO_PISTOL) )
		{
			gamerules_c::clientSetDroppedWeaponThink(ent);
		}
		else
		{
			// set the nextthink to whatever the nobody-has-picked-me-up-time happens to be.
			ent->think = ::respawnUntouchedItem;
			ent->nextthink = level.time + m_maxUntouchedTime;
		}
	}
	return 1;
}

/*
==================
dmreal_c::respawnUntouchedItem
==================

  This item was dropped at some point and hasn't been touched in a while. Move it
  to its designer-specified spawn point.
*/

void dmreal_c::respawnUntouchedItem(edict_t *ent)
{
	multimap<int, Vect3>::iterator	isp;	// isp. heh heh.
	int					idOfEnt = -1;
	Pickup				*pickup = NULL;

	if (pickup = thePickupList.GetPickupFromEdict(ent))
	{
		idOfEnt = pickup->GetPickupListIndex();
	}
	if (-1 == idOfEnt)
	{
		//bad.
		return;
	}
	for(isp = m_spawnPoints.begin(); isp != m_spawnPoints.end(); isp++) 
	{	// if we find this item's type in isp.first, make sure isp.second is
		//unoccupied and try moving the item there. if isp.second is occupied,
		//continue through the map and try to find another isp.first with an
		//unoccupied isp.second. if there are none available, try again in the
		//near future.
		if ((*isp).first == idOfEnt)
		{
			if (isSpawnPointFree(ent, *(vec3_t*)&((*isp).second)) )
			{
				// reload the item as well (set its health to -2 so that when it's picked up, we know to reload it...)
				ent->health = CLIP_SHOULD_BE_FILLED;
				moveItemToSpawnPoint(ent, *(vec3_t*)&((*isp).second) );
				return;
			}
		}
	}
	ent->nextthink = level.time + m_itemRetryTime;
}

/*
==================
dmreal_c::setRespawn
==================
*/

void dmreal_c::setRespawn(edict_t *ent, float delay)
{	// in vanilla deathmatch this fn is called when a pickup (specifically a non-dropped item)
	//of some sort is touched. when a weapon is picked up in dmreal, though, we don't want it 
	//to respawn...we want it to go away.
	if (!IsRespawningItem(ent))
	{
		G_FreeEdict(ent);
	}
	else
	{
		gamerules_c::setRespawn(ent, delay);
	}
}

/*
==================
dmreal_c::dmRule_xxx
==================
*/

int	dmreal_c::dmRule_SAME_LEVEL(void)
{
	return((int)dmflags->value&DF_REAL_SAME_LEVEL);
}

int	dmreal_c::dmRule_TEAMPLAY(void)
{
	return((int)dmflags->value&DF_REAL_TEAMPLAY);
}

int	dmreal_c::dmRule_FORCE_RESPAWN(void)
{
	return((int)dmflags->value&DF_REAL_FORCE_RESPAWN);
}

int	dmreal_c::dmRule_SPINNINGPICKUPS(void)
{
	return((int)dmflags->value&DF_REAL_SPINNINGPICKUPS);
}

int	dmreal_c::dmRule_BULLET_WPNS_ONLY(void)
{
	return((int)dmflags->value&DF_REAL_BULLET_WPNS_ONLY);
}

/*
==================
dmreal_c::initPlayerDMInfo
==================
*/

void dmreal_c::initPlayerDMInfo(edict_t &ent)
{
	if(!ent.client->dmInfo)
		ent.client->dmInfo = new player_dmrealInfo_c;
}

/*
==================
dmreal_c::clientCalculateWaver
==================
*/

float dmreal_c::clientCalculateWaver(edict_t &ent, int atkID)
{
	bodyhuman_c	*body = NULL;
	float		waverAmount = 0, speed = 0;

	// check for arm damage.
	if (body = (bodyhuman_c*)ent.client->body)
	{	// IsAimImpeded returns -1 if left arm, 1 if right arm. for now, though, I think
		//all we care about is the fact that we took an arm hit
		if (body->IsAimImpeded())
		{
			waverAmount += 3;	// totally guessing here. additionally, shouldn't be hardcoded.
		}
	}
	// check speed
	speed = DotProduct(ent.velocity, ent.velocity);
	speed *= 3.0 * REALISTIC_MOVESCALE / 80000.0;//max possible miss fire is 30 degrees
	waverAmount += speed;

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
dmreal_c::clientDie
==================
*/

void dmreal_c::clientDie(edict_t &ent, edict_t &inflictor, edict_t &killer)
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
dmreal_c::clientGetMovescale
==================
*/

float dmreal_c::clientGetMovescale(edict_t *ent)
{ 
	// Base move minus any reduction for being crippled.

	return(REALISTIC_MOVESCALE-(REALISTIC_MOVESCALE*ent->client->moveScale));
}

/*
==================
dmreal_c::clientRespawn
==================
*/

void dmreal_c::clientRespawn(edict_t &ent)
{
	sharedEdict_t	sh;

	sh.inv = (inven_c *)ent.client->inv;
	sh.edict = &ent;
	sh.inv->setOwner(&sh);

	sh.inv->clearInv(true);

	sh.inv->rulesSetWeaponsUseAmmo(-1);	
	sh.inv->rulesSetDropWeapons(-1);
	sh.inv->rulesSetNonDroppableWeaponTypes((1<<SFW_KNIFE)|(1<<SFW_PISTOL1));
	sh.inv->rulesSetWeaponsReload(-1);
	sh.inv->rulesSetWeaponReloadAutomatic(0);
	sh.inv->rulesSetBestWeaponMode(Info_ValueForKey(ent.client->pers.userinfo,"bestweap"));
	sh.inv->rulesSetDropInvenOnDeath(-1);
	sh.inv->rulesSetSelectWithNoAmmo(-1);
	sh.inv->rulesSetReloadEmptiesWeapon(-1);

	sh.inv->addWeaponType(SFW_KNIFE);
	sh.inv->addWeaponType(SFW_PISTOL1);
	sh.inv->addWeaponType(SFW_HURTHAND);
	sh.inv->addWeaponType(SFW_THROWHAND);

	sh.inv->addAmmoType(AMMO_KNIFE,6);	
	sh.inv->addAmmoType(AMMO_9MM,72);

	sh.inv->selectWeapon(SFW_PISTOL1);
	
	getPlayerDMInfo(&ent)->setFatigueVal(0);
	getPlayerDMInfo(&ent)->setFatigued(false);
}

/*
==================
dmreal_c::clientStealthAndFatigueMeter
==================
*/

void dmreal_c::clientStealthAndFatigueMeter(edict_t *ent)
{

	if(ent->client->pers.spectator)
	{
		getPlayerDMInfo(ent)->setFatigueVal(0);
		getPlayerDMInfo(ent)->setFatigued(false);

		ent->client->ps.stats[STAT_FLAGS]&=~STAT_FLAG_SHOW_STEALTH;
		ent->client->ps.stats[STAT_STEALTH]=0;
	}
	else
	{
		int running=((ent->client->buttons&BUTTON_RUN)&&(ent->velocity[0]||ent->velocity[1]))?-1:0;

		if(running)
		{
			getPlayerDMInfo(ent)->setFatigueVal(getPlayerDMInfo(ent)->getFatigueVal()+2);

			if(getPlayerDMInfo(ent)->getFatigueVal()>=255)
			{
				getPlayerDMInfo(ent)->setFatigueVal(255);
				getPlayerDMInfo(ent)->setFatigued(true);
			}
		}
		else
		{
			int fat=getPlayerDMInfo(ent)->getFatigueVal();
			
			if(fat)
			{
				getPlayerDMInfo(ent)->setFatigueVal(fat-3);

				if(getPlayerDMInfo(ent)->getFatigueVal()<255)
				{
					getPlayerDMInfo(ent)->setFatigued(false);
					if(getPlayerDMInfo(ent)->getFatigueVal()<=0)
					{	
						getPlayerDMInfo(ent)->setFatigueVal(0);
					}
				}
			}
		}
		
		ent->client->ps.stats[STAT_FLAGS]|=STAT_FLAG_SHOW_STEALTH;
		ent->client->ps.stats[STAT_STEALTH]=((float)getPlayerDMInfo(ent)->getFatigueVal()/255.0)*100;
	}

	if(getPlayerDMInfo(ent)->getFatigued())
		ent->client->ps.pmove.pm_flags|=PMF_FATIGUED;
	else
		ent->client->ps.pmove.pm_flags&=~PMF_FATIGUED;
}

/*
==================
dmreal_c::clientPreDisconnect
==================
*/

void dmreal_c::clientPreDisconnect(edict_t &ent)
{
	sharedEdict_t sh;

	sh.inv=(inven_c *)ent.client->inv;
	sh.edict=&ent;
	sh.inv->setOwner(&sh);
	sh.inv->dropAllWeapons();
}

/*
==================
dmreal_c::clientDisconnect
==================
*/

void dmreal_c::clientDisconnect(edict_t &ent)
{
	// close the log file if we have one open
	closeLogFile();
}

