// *****************************************************************************
// dm_arsenal.cpp
// *****************************************************************************

#include "g_local.h"
#include "dm_private.h"
#include "w_weapons.h"
#include "../strings/dm_arsenal.h"

const int DF_ARSENAL_NO_HEALTH			= (1<<0);
const int DF_ARSENAL_NO_ITEMS			= (1<<1);
const int DF_ARSENAL_NO_FALLING			= (1<<3);
const int DF_ARSENAL_REALISTIC_DAMAGE	= (1<<4);
const int DF_ARSENAL_SAME_LEVEL			= (1<<5);
const int DF_ARSENAL_FORCE_RESPAWN		= (1<<9);
const int DF_ARSENAL_NO_ARMOR			= (1<<10);

void sendRestartPrediction(edict_t *ent);

/*
=================
getPlayerDMInfo
=================
*/

static player_dmarsenalInfo_c *getPlayerDMInfo(const edict_t *ent)
{
	return((player_dmarsenalInfo_c *)ent->client->dmInfo);
}

/*
==================
SendStatusInfo
==================
*/

static void SendStatusInfo(edict_t *ent)
{
	if(level.intermissiontime)
		return;

	if(ent->client->pers.spectator)
		return;

	gi.SP_Print(ent,DM_ARSENAL_LAYOUT_STATUS,ent->client->inv->countWeapons());
}

/*
==================
SendStatusInfo2
==================
*/

static void SendStatusInfo2(edict_t *ent)
{
	if(level.intermissiontime)
		return;

	if(ent->client->pers.spectator)
		return;

	ent->client->showinventory=false;
	ent->client->showscores=false;
	ent->client->showhelp_time=level.time+5.0;
	SendStatusInfo(ent);
	gi.unicast(ent,true);
}

/*
==================
dmarsenal_c::checkEvents
==================
*/

void dmarsenal_c::checkEvents(void)
{
	if(resetGame==-1)
	{
		resetGame=0;

		int		i;
		edict_t	*e2;

		// Reset the weapons for everyone as a new round is about to start.

		setWeapons();

		for (i = 1; i <= game.maxclients; i++)
		{
			e2 = &g_edicts[i];
			
			if (!e2->inuse)
				continue;
			
			if (!e2->client)
				continue;
			
			assert(e2->client->inv);

			sharedEdict_t	sh;
			sh.inv = (inven_c *)e2->client->inv;
			sh.edict = e2;
			sh.inv->setOwner(&sh);
			e2->client->inv->deactivateCurrentWeapon();
			e2->client->inv->deactivateInventory();

			giveClientWeapons(*e2,true);

			SendStatusInfo2(e2);
		}
	}
}

/*
==================
dmarsenal_c::setWeapons
==================
*/

void dmarsenal_c::setWeapons(void)
{
	int count,
		bit,
		tot,
		max;

	count=0;
	tot=0;
	max=SFW_NUM_WEAPONS-3;

	while(1)
	{
		if((bit=gi.flrand(1,max+1))>max)
			bit=max;

		tot|=1<<(bit-1);
	
		count=0;

		for(int i=1;i<=max;i++)
		{
			if(tot&(1<<(i-1)))
			{
				weaponForSlot[count]=i;
				count++;
			}
		}

		if(count>=ARSENAL_MAXWEAPONS)
			break;
	}

	firstToOneHit=0;
	resetGame=0;
}

/*
==================
dmarsenal_c::giveClientWeapons
==================
*/

void dmarsenal_c::giveClientWeapons(edict_t &ent,qboolean newInv)
{
	if(newInv)
	{
		W_InitInv(ent);
		sendRestartPrediction(&ent);
	}

	sharedEdict_t	sh;

	sh.inv = (inven_c *)ent.client->inv;
	sh.edict = &ent;
	sh.inv->setOwner(&sh);

	sh.inv->clearInv(true);

	sh.inv->rulesSetWeaponsUseAmmo(0);
	sh.inv->rulesSetDropWeapons(0);
	sh.inv->rulesSetWeaponsReload(0);
	sh.inv->rulesSetBestWeaponMode("unsafe");
	sh.inv->rulesSetFreelySelectWeapon(0);

	if(newInv)
	{
		for(int i = 0; i < ARSENAL_MAXWEAPONS; i++)
			sh.inv->addWeaponType(weaponForSlot[i],0);

		// Save off weapons and inventory counts.

		sh.inv->buildInvString(getPlayerDMInfo((const edict_t *)&ent)->invString(),true,false,false,false);
	}
	else
	{
		// Reconstitute saved inventory.

		int	weaponsAvailable;
		sh.inv->extractInvFromString(&weaponsAvailable,getPlayerDMInfo((const edict_t *)&ent)->invString());
	}

	sh.inv->addWeaponType(SFW_THROWHAND);
	
	sh.inv->selectBestWeapon();
}

/*
==================
dmarsenal_c::levelInit
==================
*/

void dmarsenal_c::levelInit(void)
{
	gi.SP_Register("dm_arsenal");
	
	// Precache sounds.

	gi.soundindex("dm/arsenal/bigwinner.wav");
	gi.soundindex("dm/arsenal/lastweapon.wav");

	// Make certain all the weapons needed during the game are precached,
	// because it's possible for all the weapon types to be used during
	// a game.

	for(int i=SFW_KNIFE;i<=SFW_THROWHAND;i++)
	{
		// View weapons.

		AddWeaponTypeForPrecache(i);

		//  Bolt-ons / pickups. Eugh... but necessary to precache.

		if(Pickup *p=thePickupList.GetPickupFromType(PU_WEAPON,i))
		{
			edict_t *ent=G_Spawn();
			I_Spawn(ent,p);
			G_FreeEdict(ent);
		}
	}

	// Determine the available weapons at the start of the game.

	setWeapons();
}

/*
==================
dmarsenal_c::checkItemSpawn
==================
*/

int	dmarsenal_c::checkItemSpawn(edict_t *ent,Pickup **pickup)
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
	
	if ((*pickup)->GetType() == PU_AMMO)
	{
		G_FreeEdict (ent);
		return(0);
	}

	if ((*pickup)->GetType() == PU_WEAPON)
	{
		G_FreeEdict (ent);
		return(0);
	}

	return(-1);
}

/*
==================
dmarsenal_c::dmRule_xxx
==================
*/

int	dmarsenal_c::dmRule_NO_HEALTH(void)
{
	return((int)dmflags->value&DF_ARSENAL_NO_HEALTH);
}

int	dmarsenal_c::dmRule_NO_ITEMS(void)
{
	return((int)dmflags->value&DF_ARSENAL_NO_ITEMS);
}

int	dmarsenal_c::dmRule_NO_FALLING(void)
{
	return((int)dmflags->value&DF_ARSENAL_NO_FALLING);
}

int	dmarsenal_c::dmRule_REALISTIC_DAMAGE(void)
{
	return((int)dmflags->value&DF_ARSENAL_REALISTIC_DAMAGE);
}

int	dmarsenal_c::dmRule_SAME_LEVEL(void)
{
	return((int)dmflags->value&DF_ARSENAL_SAME_LEVEL);
}

int	dmarsenal_c::dmRule_FORCE_RESPAWN(void)
{
	return((int)dmflags->value&DF_ARSENAL_FORCE_RESPAWN);
}

int	dmarsenal_c::dmRule_NO_ARMOR(void)
{
	return((int)dmflags->value&DF_ARSENAL_NO_ARMOR);
}

/*
==================
dmarsenal_c::initPlayerDMInfo
==================
*/

void dmarsenal_c::initPlayerDMInfo(edict_t &ent)
{
	if(!ent.client->dmInfo)
		ent.client->dmInfo = new player_dmarsenalInfo_c;
}

/*
==================
dmarsenal_c::clientConnect
==================
*/

void dmarsenal_c::clientConnect(edict_t &ent)
{
	gamerules_c::clientConnect(ent);
	giveClientWeapons(ent,true);
}

/*
==================
dmarsenal_c::clientDie
==================
*/

void dmarsenal_c::clientDie(edict_t &ent, edict_t &inflictor, edict_t &killer)
{
	if(ent.client->pers.spectator)
		return;

	// Save off weapons and inventory counts.

	sharedEdict_t	sh;

	sh.inv = (inven_c *)ent.client->inv;
	sh.edict = &ent;
	sh.inv->setOwner(&sh);

	sh.inv->buildInvString(getPlayerDMInfo((const edict_t *)&ent)->invString(),true,false,false,false);

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
	else
	{
		// I was killed by another player.

		int weaponsCount;

		// Award my killer a frag.

		killer.client->resp.score+=1;

		// Telefragging, hand grenades and c4 don't count towards the big winner bonus.

		if((meansOfDeath!=MOD_TELEFRAG)&&(meansOfDeath!=MOD_GRENADE)&&(meansOfDeath!=MOD_C4))
		{
			// My killer used their current weapon to kill me, so they should drop
			// it and take out the next one.

			killer.client->inv->removeCurrentWeapon();

			// When counting the number of weapons, we need to subtract -1 because
			// the inventory system takes a while to actually drop the weapon. In
			// other words, we can't just set the current weapon slot to empty in
			// removeCurrentWeapon()... and then count the weapons - that screws
			// the whole weapon changing thing up. So I guess this is not the worst
			// or the best hack, but re-writing the weapons code to avoid the above
			// scenario just isn't worth it at this late stage.

			weaponsCount=killer.client->inv->countWeapons()-1;

			// Check to see how many weapons my killer has left...

			if(weaponsCount==0)
			{
				// My killer used all his weapons up so award them a frag plus the big winner bonus.

				killer.client->resp.score+=10;
				gi.positioned_sound(vec3_origin,g_edicts,CHAN_BODY,gi.soundindex("dm/arsenal/bigwinner.wav"),0.6,ATTN_NONE,0);
				gi.SP_Print(0,DM_ARSENAL_TEXT_BIG_WINNER,killer.s.number);

				// Time to reset game!

				resetGame=-1;
			}
			else if((!firstToOneHit)&&(weaponsCount==1))
			{
				// Somehas become the first player to have just one weapon left to
				// use, so annouce this to everyone.

				firstToOneHit=1;
				gi.positioned_sound(vec3_origin,g_edicts,CHAN_BODY,gi.soundindex("dm/arsenal/lastweapon.wav"),0.6,ATTN_NONE,0);
				gi.SP_Print(0,DM_ARSENAL_TEXT_LAST_WEAPON,killer.s.number);
			}
		}
	}
}

/*
==================
dmarsenal_c::clientDieWeaponHandler
==================
*/

void dmarsenal_c::clientDieWeaponHandler(edict_t *ent)
{
	sharedEdict_t sh;

	sh.inv=(inven_c *)ent->client->inv;
	sh.edict=ent;
	sh.inv->setOwner(&sh);
	sh.inv->rulesSetDropWeapons(0);
	sh.inv->handlePlayerDeath();

	if(IGhoulInst *gun=ent->client->ps.gun)
		gun->SetAllPartsOnOff(false);
}

/*
==================
dmarsenal_c::clientHelpMessage
==================
*/

void dmarsenal_c::clientHelpMessage(edict_t *ent)
{
	// Display my status info.

	SendStatusInfo2(ent);
}

/*
==================
dmarsenal_c::clientRespawn
==================
*/

void dmarsenal_c::clientRespawn(edict_t &ent)
{
	giveClientWeapons(ent,ent.client->pers.spectator);

	// Send status update.

	SendStatusInfo2(&ent);
}

/*
==================
dmarsenal_c::clientScoreboardMessage
==================
*/

void dmarsenal_c::clientScoreboardMessage (edict_t *ent, edict_t *killer, qboolean log_file)
{
	gamerules_c::clientScoreboardMessage(ent,killer,log_file);
}