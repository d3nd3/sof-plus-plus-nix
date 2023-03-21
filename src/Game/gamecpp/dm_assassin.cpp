// *****************************************************************************
// dm_assassin.cpp
// *****************************************************************************

#include "g_local.h"
#include "dm_private.h"
#include "w_weapons.h"
#include "../strings/dm_assassin.h"
#include "..\qcommon\configstring.h"

const int DF_ASSASSIN_NO_HEALTH			= (1<<0);
const int DF_ASSASSIN_NO_ITEMS			= (1<<1);
const int DF_ASSASSIN_WEAPONS_STAY		= (1<<2);
const int DF_ASSASSIN_NO_FALLING		= (1<<3);
const int DF_ASSASSIN_REALISTIC_DAMAGE	= (1<<4);
const int DF_ASSASSIN_SAME_LEVEL		= (1<<5);
const int DF_ASSASSIN_NOWEAPRELOAD		= (1<<6);
const int DF_ASSASSIN_FORCE_RESPAWN		= (1<<9);
const int DF_ASSASSIN_NO_ARMOR			= (1<<10);
const int DF_ASSASSIN_INFINITE_AMMO		= (1<<11);
const int DF_ASSASSIN_SPINNINGPICKUPS	= (1<<12);
const int DF_ASSASSIN_BULLET_WPNS_ONLY	= (1<<13);

extern PickupList thePickupList;

/*
=================
getPlayerDMInfo
=================
*/

static player_dmassassinInfo_c *getPlayerDMInfo(const edict_t *ent)
{
	return((player_dmassassinInfo_c *)ent->client->dmInfo);
}

/*
=================
decideColor

Decide what color source uses to draw other's name.
=================
*/

void decideColor(const edict_t *source,const edict_t *other)
{
	if((!source)||(!other))
		return;

	if(source->client->pers.spectator)
	{
		// We're Spectating so we don't see any icons.

		setPlayernameColor((edict_t &)*source,(edict_t &)*other,TC_BLACK);
		return;
	}

	if(getPlayerDMInfo(source)->getTarget()!=other)
	{
		// Source is not targetting other...

		if(getPlayerDMInfo(other)->getTarget()!=source)
		{
			// ...and other isn't targetting source, so source sees other in BRIGHT_GREEN.

			setPlayernameColor((edict_t &)*source,(edict_t &)*other,TC_BRIGHT_GREEN);
		}
		else
		{
			// ...and other is targetting source, so source sees other in BRIGHT_YELLOW.

			setPlayernameColor((edict_t &)*source,(edict_t &)*other,TC_BRIGHT_YELLOW);
		}
	}
	else 
	{	
		// Source is targetting other, so source sees other in BRIGHT_RED.

		setPlayernameColor((edict_t &)*source,(edict_t &)*other,TC_BRIGHT_RED);
	}
}

/*
==================
SendStatusInfo
==================
*/

static void SendStatusInfo(edict_t *ent,qboolean showEnemyTargeters)
{
	if(level.intermissiontime)
		return;

	if(ent->client->pers.spectator)
		return;

	const edict_t *target;

	target=getPlayerDMInfo(ent)->getTarget();

	if(target)
		gi.SP_Print(ent,DM_ASSASSIN_LAYOUT_TARGET,target->s.number);
	else
		gi.SP_Print(ent,DM_ASSASSIN_LAYOUT_NO_TARGET);

	const edict_t	*e2;
	int				count;

	count=0;

	for(int i=1;i<=game.maxclients;i++)
	{
		e2 = &g_edicts[i];

		if (!e2->inuse)
			continue;

		if (!e2->client)
			continue;

		if (getPlayerDMInfo(e2)->getTarget() != ent)
			continue;

		count++;
	}

	gi.SP_Print(ent,DM_ASSASSIN_LAYOUT_NUM_ENEMIES,count);
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
	SendStatusInfo(ent,false);
	gi.unicast(ent,true);
}

/*
==================
dmassassin_c::getRandomPlayer
==================
*/

const edict_t *dmassassin_c::getRandomPlayer(edict_t &source)
{
	const edict_t	*newTarget=NULL;

	// Find new target.

	if(countPlayers()>=2)
	{
		int begin=(rand()%game.maxclients)+1;

		do
		{	
			newTarget=&g_edicts[begin];
		
			begin++;
			if (begin > game.maxclients)
			{
				begin = 1;
			}
		}while((!newTarget->inuse)||(newTarget->client->pers.spectator)||(!newTarget->client)||(newTarget == &source));
	}

	assert(newTarget!=&source);

	return(newTarget);
}

/*
==================
dmassassin_c::levelInit
==================
*/

void dmassassin_c::levelInit(void)
{
	gi.SP_Register("dm_assassin");

	// Precache sounds.

	gi.soundindex("dm/assassin/killtarget.wav");
	gi.soundindex("dm/assassin/badkill.wav");

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

	// Ensure that player info always shows up, even in darkness.

	gi.configstring(CS_SHOWINFOINDARK,"1");
}

/*
==================
dmassassin_c::checkItemSpawn
==================
*/

int	dmassassin_c::checkItemSpawn(edict_t *ent,Pickup **pickup)
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
dmassassin_c::dmRule_xxx
==================
*/

int	dmassassin_c::dmRule_NO_HEALTH(void)
{
	return((int)dmflags->value&DF_ASSASSIN_NO_HEALTH);
}

int	dmassassin_c::dmRule_NO_ITEMS(void)
{
	return((int)dmflags->value&DF_ASSASSIN_NO_ITEMS);
}

int	dmassassin_c::dmRule_WEAPONS_STAY(void)
{
	return((int)dmflags->value&DF_ASSASSIN_WEAPONS_STAY);
}

int	dmassassin_c::dmRule_NO_FALLING(void)
{
	return((int)dmflags->value&DF_ASSASSIN_NO_FALLING);
}

int	dmassassin_c::dmRule_REALISTIC_DAMAGE(void)
{
	return((int)dmflags->value&DF_ASSASSIN_REALISTIC_DAMAGE);
}

int	dmassassin_c::dmRule_SAME_LEVEL(void)
{
	return((int)dmflags->value&DF_ASSASSIN_SAME_LEVEL);
}

int	dmassassin_c::dmRule_NOWEAPRELOAD(void)
{
	return((int)dmflags->value&DF_ASSASSIN_NOWEAPRELOAD);
}

int	dmassassin_c::dmRule_FORCE_RESPAWN(void)
{
	return((int)dmflags->value&DF_ASSASSIN_FORCE_RESPAWN);
}

int	dmassassin_c::dmRule_NO_ARMOR(void)
{
	return((int)dmflags->value&DF_ASSASSIN_NO_ARMOR);
}

int	dmassassin_c::dmRule_INFINITE_AMMO(void)
{
	return((int)dmflags->value&DF_ASSASSIN_INFINITE_AMMO);
}

int	dmassassin_c::dmRule_SPINNINGPICKUPS(void)
{
	return((int)dmflags->value&DF_ASSASSIN_SPINNINGPICKUPS);
}

int	dmassassin_c::dmRule_BULLET_WPNS_ONLY(void)
{
	return((int)dmflags->value&DF_ASSASSIN_BULLET_WPNS_ONLY);
}

/*
==================
dmassassin_c::initPlayerDMInfo
==================
*/

void dmassassin_c::initPlayerDMInfo(edict_t &ent)
{
	if(!ent.client->dmInfo)
		ent.client->dmInfo = new player_dmassassinInfo_c;
}

/*
==================
dmassassin_c::registerPlayerIcons
==================
*/

void dmassassin_c::registerPlayerIcons(void)
{
	char *iconNames[]=
	{
		"",
		"noshoot",
		"targetyou",
		"destarget",
		"**"
	};

	int		i;
	char	**icon;
	
	for(icon=iconNames,i=0;strcmp(*icon,"**");icon++,i++)
		gi.configstring(CS_PLAYERICONS+i,*icon);
}

/*
==================
dmassassin_c::clientConnectAux
==================
*/

void dmassassin_c::clientConnectAux(edict_t &ent)
{
	int				i;
	const edict_t	*ent2;

	// Ensure my target is cleared out.
	
	getPlayerDMInfo(&ent)->setTarget(NULL);

	// Initially, I see all other players' playernames in GREEN.
	
	for(i=1;i<=game.maxclients;i++)
	{
		ent2=&g_edicts[i];
	
		if(!ent2->inuse)
			continue;

		if (!ent2->client)
			continue;

		decideColor(&ent,ent2);
	}
	
	if(ent.client->pers.spectator)
		return;

	// Select a target whom I will try to assassinate.
	
	getPlayerDMInfo(&ent)->setTarget(getRandomPlayer(ent));
	decideColor(&ent,getPlayerDMInfo(&ent)->getTarget());

	// When a I join a game, we should also look around to see if there are any
	// other players (i.e. besides myself) WITHOUT targets. E.g. when the 2nd
	// player joins a game, the 1st player to have joined will be without a
	// target to assassinate. So set the 1st player up with a target. We need to
	// do a full check of all clients because of the order in which clients can
	// connect and occupy edict/client slots. Kinda nasty really, but necessary.

	for(i=1; i<=game.maxclients; i++)
	{
		ent2 = &g_edicts[i];

		if (!ent2->inuse)
			continue;

		if (!ent2->client)
			continue;

		// Don't check myself - I already have a target.

		if (ent2==&ent)
			continue;

		// What color does other player see my name?

		decideColor(ent2,&ent);

		// Does other player have a target (see note above)? If so skip to
		// next player.

		if (getPlayerDMInfo(ent2)->getTarget())
			continue;

		// Ok, other player is without a target, so select a someone whom they
		// will try to assissinate...

		getPlayerDMInfo(ent2)->setTarget(getRandomPlayer((edict_t &)*ent2));
		decideColor(ent2,getPlayerDMInfo(ent2)->getTarget());

		// ...and let them know their new target's name.

		SendStatusInfo2((edict_t *)ent2);
	}
}

/*
==================
dmassassin_c::clientConnect
==================
*/

void dmassassin_c::clientConnect(edict_t &ent)
{
	clientConnectAux(ent);
}

/*
==================
dmassassin_c::clientDie
==================
*/

void dmassassin_c::clientDie(edict_t &ent, edict_t &inflictor, edict_t &killer)
{
	if(ent.client->pers.spectator)
		return;

	// Lose a frag?

	if((!killer.client)||(&killer==&ent))
	{	
		// I was killed by non player or killed myself.

		ent.client->resp.score--;

		return;
	}

	// Save my last target and my killer's last target.

	const edict_t *myOldTarget,*killersOldTarget;		

	myOldTarget=getPlayerDMInfo(&ent)->getTarget();
	killersOldTarget=getPlayerDMInfo(&killer)->getTarget();

	// What happens next depends on who killed who...

	if(killersOldTarget==&ent)
	{
		// My killer killed their designated target (me).
		
		killer.client->resp.score+=4;
		
		// Confirmation of successful kill.
		
		gi.sound(&killer,CHAN_VOICE,gi.soundindex("dm/assassin/killtarget.wav"),1.0,ATTN_STATIC,0,SND_LOCALIZE_CLIENT);
	}
	else if(myOldTarget!=&killer)
	{
		// My killer killed an 'innocent bystander' (me).
		
		killer.client->resp.score--;

		// Uh-oh... made a bad kill... let them know it too!
		
		gi.sound(&killer,CHAN_VOICE,gi.soundindex("dm/assassin/badkill.wav"),1.0,ATTN_STATIC,0,SND_LOCALIZE_CLIENT);
		gi.SP_Print(&killer,DM_ASSASSIN_TEXT_LOST_FRAG);
	}
	else
	{
		// My killer is my designated target, thus was acting in self defence.
		
		killer.client->resp.score++;
		
		// Confirmation of a self defense kill.

		gi.sound(&killer,CHAN_VOICE,gi.soundindex("dm/frag.wav"),1.0,ATTN_STATIC,0,SND_LOCALIZE_CLIENT);
		return;
	}

	// Set up new targets for killed and killer.

	getPlayerDMInfo(&ent)->setDeferedTarget(&killer);
	
	if(myOldTarget!=&killer)
		getPlayerDMInfo(&killer)->setDeferedTarget(myOldTarget);
	else
		getPlayerDMInfo(&killer)->setDeferedTarget(getRandomPlayer(killer));
}

/*
==================
dmassassin_c::clientDisconnectAux
==================
*/

void dmassassin_c::clientDisconnectAux(edict_t &ent)
{
	int		i;
	edict_t	*e2;

	// Select new targets for any players who were targeting me.

	for (i = 1; i <= game.maxclients; i++)
	{
		e2 = &g_edicts[i];

		if (!e2->inuse)
			continue;

		if (!e2->client)
			continue;

		// Was this player targeting me?

		if (getPlayerDMInfo(e2)->getTarget() == &ent)
		{
			// Yes, so select a new target whom they will try to assissinate...

			getPlayerDMInfo(e2)->setDeferedTarget(getRandomPlayer(*e2));
		}
	}
}

/*
==================
dmassassin_c::clientDisconnect
==================
*/

void dmassassin_c::clientDisconnect(edict_t &ent)
{
	clientDisconnectAux(ent);

	// Close the log file if we have one open.

	closeLogFile();
}

/*
==================
dmassassin_c::clientEndFrame
==================
*/

void dmassassin_c::clientEndFrame(edict_t *ent)
{
	if(getPlayerDMInfo(ent)->getDeferedTarget()!=getPlayerDMInfo(ent)->getTarget())
	{
		const edict_t *entOldTarget;

		entOldTarget=getPlayerDMInfo(ent)->getTarget();

		getPlayerDMInfo(ent)->setTarget(getPlayerDMInfo(ent)->getDeferedTarget());
		
		decideColor(ent,entOldTarget);
		decideColor(ent,getPlayerDMInfo(ent)->getTarget());
		decideColor(entOldTarget,ent);
		decideColor(getPlayerDMInfo(ent)->getTarget(),ent);

		SendStatusInfo2(ent);
	}
}

/*
==================
dmassassin_c::clientHelpMessage
==================
*/

void dmassassin_c::clientHelpMessage(edict_t *ent)
{
	// Display my status info.

	SendStatusInfo(ent,false);
}

/*
==================
dmassassin_c::clientRespawn
==================
*/

void dmassassin_c::clientRespawn(edict_t &ent)
{
	if(ent.client->pers.spectator)
	{
		// We respawned as a spectator so we need to handle anyone who was
		// targetting me. Also, clear my target out.

		clientDisconnectAux(ent);
		getPlayerDMInfo(&ent)->setTarget(NULL);

		return;
	}
	else if(ent.client->pers.spectator != ent.client->resp.spectator)
	{
		// Respawned as normal player but I was previously a spectator, so I
		// need a target to assassinate. Maybe someone needs a target too.
	
		clientConnectAux(ent);
	}

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

	// If I have a target to assassinate, let me know who it is.

	SendStatusInfo2(&ent);
}

/*
==================
dmassassin_c::clientScoreboardMessage
==================
*/

void dmassassin_c::clientScoreboardMessage (edict_t *ent, edict_t *killer, qboolean log_file)
{
	gamerules_c::clientScoreboardMessage(ent,killer,log_file);
}
