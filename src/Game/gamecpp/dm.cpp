// *****************************************************************************
// dm.cpp
// *****************************************************************************

#include "g_local.h"
#include "dm_private.h"
#include "../player/w_weapons.h"
#include "../strings/dm_generic.h"
#include "ai_private.h"	// so we know what a bodyhuman_c is
#include "../qcommon/configstring.h"

gamemode_ic *dm = NULL;

dmnone_c		nodm;
dmstandard_c	standard;
dmassassin_c	assassin;
dmarsenal_c		arsenal;
dmctf_c			ctf;
dmreal_c		real;

gamerules_c	*gamerules[] =
{
	&nodm,
	&standard,
	&assassin,
	&arsenal,
	&ctf,
	&real
};

extern void W_InitInv(edict_t &ent);
extern void sendRestartPrediction(edict_t *ent);

/*
=================
countPlayers

Returns the number of players in the current game.
=================
*/

int countPlayers(void)
{
	int		i;
	edict_t	*e2;
	int		num = 0;

	for(i = 1; i <= game.maxclients; i++)
	{
		e2 = &g_edicts[i];

		if (!e2->inuse)
			continue;

		if (!e2->client)
			continue;

		if (e2->client->pers.spectator)
			continue;

		num++;
	}
	
	return(num);
}

/*
=================
IsFemale
=================
*/

qboolean IsFemale(edict_t *ent)
{
	if(!ent->client || !ent->client->body)
		return false;

	if (ent->client->body->GetClassCode()==BODY_FEMALE)
	{
		return (true);
	}

	return(false);
}

/*
==============
setPlayernameColor
==============
*/

void setPlayernameColor(edict_t &ent,edict_t &other,byte color)
{
	if(!ent.client)
		return;

	if(other.inuse)
		ent.client->playernameColors[other.s.number-1]=color;
}

/*
=================
InitDeathmatchSystem
=================
*/

void InitDeathmatchSystem(void)
{
	if(!dm)
	{
		dm = new gamemode_c;
	}

#ifndef _DEMO_
	dm->setDMMode(((deathmatch->value>=DM_NONE)&&(deathmatch->value<=DM_REAL))?deathmatch->value:0);
#else
	dm->setDMMode(((deathmatch->value>=DM_NONE)&&(deathmatch->value<=DM_STANDARD))?deathmatch->value:0);
#endif

	gi.SP_Register("dm_generic");

	// Generic DM specific precaching and stuff.

	if(dm->isDM())
	{
		// Precache generic DM effects.

//		gi.effectindex("environ/invuln");
		gi.effectindex("environ/irespawn");
		gi.effectindex("environ/prespawn");

		// Precache generic DM sounds.

		gi.soundindex("dm/armorpu.wav");
		gi.soundindex("dm/irespawn.wav");
		gi.soundindex("dm/frag.wav");
		gi.soundindex("dm/selffrag.wav");
		gi.soundindex("dm/healthpu.wav");
		gi.soundindex("dm/prespawn.wav");
	}

	// Clear the CTF team color config strings, used to tell clients what blue
	// and red team names are.

	gi.configstring(CS_CTF_BLUE_TEAM, "");
	gi.configstring(CS_CTF_RED_TEAM, "");

	dm->preLevelInit();

}

/*
=================
DeleteDeathmatchSystem
=================
*/

void DeleteDeathmatchSystem(void)
{
	if(dm)
	{
		delete dm;

		dm=0;
	}
}

/*
==================
respawnUntouchedItem
==================

  This item was dropped at some point and hasn't been touched in a while. Move it
  to its designer-specified spawn point.
*/

void respawnUntouchedItem(edict_t* ent)
{
	dm->respawnUntouchedItem(ent);
}


/*
===============
ClearEffects
===============
*/


void ClearEffects (edict_t *ent)
{	
	// This also makes the item solid.

	fxRunner.clearContinualEffects(ent);
}


/*
===============
DoRespawn
===============
*/

void DoRespawn (edict_t *ent)
{
	ent->flags &= ~FL_RESPAWN;
	ent->svflags &= ~SVF_NOCLIENT;
	ent->solid = SOLID_NOT;
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
	gi.sound(ent, CHAN_ITEM, gi.soundindex("dm/irespawn.wav"), .6, ATTN_NORM, 0,SND_LOCALIZE_GLOBAL);

	ent->think = ClearEffects;
	ent->nextthink = level.time + ITEM_RESPAWN_FADEIN;
}


/*
================
itemDropToFloor

Adjust the Z component of an ent's origin of so that it lies on the floor.
================
*/

void itemDropToFloor (edict_t *ent)
{
	trace_t		tr;
	vec3_t		vec,dest;

	VectorSet(vec,0.0,0.0,-128.0);

	VectorAdd (ent->s.origin, vec, dest);

	gi.trace (ent->s.origin, ent->mins, ent->maxs, dest, ent, MASK_SOLID, &tr);

	if (tr.startsolid)
	{
		gi.dprintf ("itemDropToFloor: %s startsolid at %s\n", ent->classname, vtos(ent->s.origin));
		
		G_FreeEdict (ent);
		
		return;
	}

	VectorCopy (tr.endpos, ent->s.origin);

	ent->nextthink = level.time + FRAMETIME;
	ent->think = DoRespawn;
}

/*
===============
gamerules_c::setRespawn
===============
*/

void gamerules_c::setRespawn (edict_t *ent, float delay)
{
	float adjusted_delay;
	int	  clients;
	ent->flags |= FL_RESPAWN;
	ent->svflags |= SVF_NOCLIENT;
	ent->solid = SOLID_NOT;

	if (deathmatch->value)
	{
		clients=(float)level.num_clients;
		if (clients<2.0)
			clients=2.0;
		adjusted_delay = delay * sqrt(2.0/clients);		// Spawn more frequently when more players.
		// Lemme see here:  sqrt(2/2) = sqrt(1) = 1
		//					sqrt(2/8) = sqrt(1/4) = 1/2
		//					sqrt(2/32) = sqrt(1/16) = 1/4
	}
	else
	{
		adjusted_delay = delay;
	}

	ent->nextthink = level.time + adjusted_delay;
	ent->think = DoRespawn;

	gi.linkentity (ent);
}

/*
==================
gamerules_c::openLogFile
==================
*/

void gamerules_c::openLogFile(void)
{
	if(log_file_name->string[0])
	{
		char name[MAX_QPATH];

		// Try to open the log-file.
		// do we need a counter value?
		Com_sprintf (name, sizeof(name), "%s", log_file_name->string);
		if (log_file_counter->value)
		{
			char	exten[10];

			// copy the extension somewhere
			strcpy(exten, COM_FileExtension(name));
			// remove the extension from the file name
			COM_StripExtension(name, name);
			// add in number
			Com_sprintf (name, sizeof(name), "%s%d.", name, (int)log_file_counter->value);
			gi.cvar_setvalue("log_file_counter", log_file_counter->value +1.0);
			// re-add extension
			strcat(name, exten);

		}
		Com_Printf ("Dumping end game log to %s\n", name);

		gi.FS_CreatePath (name);
		// decide what we need to do, append or overwrite
		if (!stricmp(log_file_mode->string, "o"))
		{
			filePtr = fopen (name, "w");
		}
		else
		{
			filePtr = fopen (name, "a");
		}
		
		// If we managed to open the log-file, write out the game details, else
		// print an error message.

		if(filePtr)
		{
			int		hours,secs,mins;
			cvar_t	*host_name;

			secs = level.time;
			hours = secs / (60*60);
			secs -= hours * (60*60);
			mins = secs / 60;
			secs -= mins * 60;
			host_name = gi.cvar("hostname", "", 0,NULL);

			fprintf (filePtr, "%s\n", log_file_header->string);
			fprintf (filePtr, "%sMap Name : %s\n", log_file_line_header->string, level.mapname);
			fprintf (filePtr, "%sHost Name : %s\n", log_file_line_header->string, host_name->string);
			fprintf (filePtr, "%sGame type : %s\n", log_file_line_header->string, getGameName());
			fprintf (filePtr, "%sGame Duration : %02i:%02i:%02i\n%s\n", log_file_line_header->string, hours, mins, secs, log_file_line_header->string);
		}
		else
		{
			Com_Printf("ERROR: couldn't open game log-file.\n");
		}
	}
}

/*
==================
gamerules_c::closeLogFile
==================
*/

void gamerules_c::closeLogFile(void)
{
	if(filePtr)
	{
		fprintf(filePtr,"%s\n",log_file_footer->string);
		fclose(filePtr);
	}
}

/*
==================
gamerules_c::clientConnect
==================
*/

void gamerules_c::clientConnect(edict_t &ent)
{

	// I see my own playername in TC_BLACK.

	setPlayernameColor(ent,ent,0);

	// I see all other players' playernames in TC_BLACK.

	edict_t *other;
	int		i;

	for(i=1;i<=game.maxclients;i++)
	{
		other=&g_edicts[i];
	
		if(!other->inuse)
			continue;

		if(!other->client)
			continue;
		
		if(other!=&ent)
			setPlayernameColor(ent,(edict_t &)*other,0);
	}

	// All other players see my playername in TC_BLACK.

	for(i=1;i<=game.maxclients;i++)
	{
		other=&g_edicts[i];
	
		if(!other->inuse)
			continue;

		if(!other->client)
			continue;

		if(other!=&ent)
			setPlayernameColor((edict_t &)*other,ent,0);
	}
}

/*
==================
gamerules_c::clientSetDroppedItemThink
==================
*/

static void droppedItemThink(edict_t *ent)
{
//	fxRunner.exec("environ/teleport7", ent->s.origin);
	G_FreeEdict(ent);
	return;
}

void gamerules_c::clientSetDroppedItemThink(edict_t *ent)
{
	ent->think=droppedItemThink;
	ent->nextthink=level.time+20.0F;
}

/*
==================
gamerules_c::clientDropItem
==================
*/

void gamerules_c::clientDropItem(edict_t *ent,int type,int ammoCount)
{
	if (!ent->ghoulInst)
	{
		return;
	}

	Pickup	*pickup = NULL;

	if (pickup = thePickupList.GetPickupFromType(PU_INV, type))
	{
		edict_t		*dropped;
		Matrix4		ZoneMatrix;
		Vect3		zonePos;
		vec3_t		handPos,
					dir;

		// Whatever weapon player is holding, throw one off.

		dropped=G_Spawn();
		dropped->spawnflags|=DROPPED_ITEM;
		I_Spawn(dropped,pickup);

		dropped->enemy=ent;
		dropped->touch_debounce_time=level.time+2.0;

		ent->ghoulInst->
			GetBoltMatrix(level.time,
						  ZoneMatrix,
						  ent->ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"),
						  IGhoulInst::Entity);
		
		ZoneMatrix.GetRow(3,zonePos);

		handPos[0]=zonePos[0];
		handPos[1]=zonePos[1];
		handPos[2]=zonePos[2];

		VectorAdd(ent->s.origin,handPos,dropped->s.origin);
		AngleVectors(ent->client->ps.viewangles,dir,NULL, NULL);
		dir[2] = 0;
		VectorNormalize(dir);
		VectorMA(dropped->s.origin, 20.0, dir, dropped->s.origin);
		VectorScale(dir,250.0,dropped->velocity);
		dropped->velocity[2]+=150.0;
		dropped->health = ammoCount;

		clientSetDroppedItemThink(dropped);

	}
}

/*
==================
gamerules_c::clientSetDroppedWeaponThink
==================
*/

static void droppedWeaponThink(edict_t *ent)
{
//	fxRunner.exec("environ/teleport7", ent->s.origin);
	G_FreeEdict(ent);
	return;
}

void gamerules_c::clientSetDroppedWeaponThink(edict_t *ent)
{
	ent->think=droppedWeaponThink;
	ent->nextthink=level.time+20.0F;
}

/*
==================
gamerules_c::clientDieWeaponHandler
==================
*/

void gamerules_c::clientDieWeaponHandler(edict_t *ent)
{
	sharedEdict_t sh;

	sh.inv=(inven_c *)ent->client->inv;
	sh.edict=ent;
	sh.inv->setOwner(&sh);
	sh.inv->rulesSetDropWeapons(-1);
	sh.inv->handlePlayerDeath();

	if(IGhoulInst *gun=ent->client->ps.gun)
		gun->SetAllPartsOnOff(false);
}

/*
==================
gamerules_c::clientDropWeapon
==================
*/

void gamerules_c::clientDropWeapon(edict_t *ent,int type, int clipSize)
{
	if (!ent->ghoulInst)
	{
		return;
	}

	Pickup	*pickup = NULL;

	if (pickup = thePickupList.GetPickupFromType(PU_WEAPON, type))
	{
		edict_t		*dropped;
		Matrix4		ZoneMatrix;
		Vect3		zonePos;
		vec3_t		handPos,
					dir;

		// Whatever weapon player is holding, throw one off.

		dropped=G_Spawn();
		dropped->spawnflags|=DROPPED_ITEM;
		I_Spawn(dropped,pickup);

		dropped->enemy=ent;
		dropped->touch_debounce_time=level.time+2.0;

		ent->ghoulInst->
			GetBoltMatrix(level.time,
						  ZoneMatrix,
						  ent->ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r"),
						  IGhoulInst::Entity);
		
		ZoneMatrix.GetRow(3,zonePos);

		handPos[0]=zonePos[0];
		handPos[1]=zonePos[1];
		handPos[2]=zonePos[2];

		VectorAdd(ent->s.origin,handPos,dropped->s.origin);
		AngleVectors(ent->client->ps.viewangles,dir,NULL, NULL);
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

		clientSetDroppedWeaponThink(dropped);

		if(ent->health>0)
		{
			const char	*weapon = gi.SP_GetStringText(pickup->GetDroppedStringIndex());
			gi.SP_Print(ent,DM_GENERIC_TEXT_DROPPED_WEAPON,weapon);
		}
	}
}

/*
==================
gamerules_c::clientHandlePredn
==================
*/

void gamerules_c::clientHandlePredn(edict_t &ent)
{
	// Start with a zero init'd inventory and issue a (reliable) instruction to
	// restart the player's client-side inventory system.

	W_InitInv(ent);
	sendRestartPrediction(&ent);
}

/*
==================
gamerules_c::clientObituary
==================
*/

void gamerules_c::clientObituary(edict_t *self, edict_t *inflictor, edict_t *attacker)
{
	int			mod,
				message;

	mod = meansOfDeath & ~MOD_FRIENDLY_FIRE;
	message = -1;

	switch (mod)
	{
		case MOD_WATER:
			message = DM_GENERIC_TEXT_OBT_MOD_WATER;
			break;

		case MOD_SLIME:
			message = DM_GENERIC_TEXT_OBT_MOD_SLIME;
			break;

		case MOD_CRUSH:
			message = DM_GENERIC_TEXT_OBT_MOD_CRUSH;
			break;

		case MOD_FALLING:
			message = DM_GENERIC_TEXT_OBT_MOD_FALLING;
			break;

		case MOD_SUICIDE:
			message = DM_GENERIC_TEXT_OBT_MOD_SUICIDE;
			break;
														
		case MOD_EXPLOSIVE:
			message = DM_GENERIC_TEXT_OBT_MOD_EXPLOSIVE;
			break;

		case MOD_FIRE:
			message = DM_GENERIC_TEXT_OBT_MOD_FIRE;
			break;

		case MOD_LAVA:
			message = DM_GENERIC_TEXT_OBT_MOD_LAVA;
			break;

		case MOD_EXIT:
			message = DM_GENERIC_TEXT_OBT_MOD_EXIT;
			break;

		case MOD_BARBWIRE:
			message = DM_GENERIC_TEXT_OBT_MOD_BARBWIRE;
			break;

		case MOD_DOGBITE:
			message = DM_GENERIC_TEXT_OBT_MOD_DOGBITE;
			break;
	}

	if(attacker == self)
	{
		switch (mod)
		{
			case MOD_PHOS_GRENADE:
				message = DM_GENERIC_TEXT_OBT_MOD_PHOS_GRENADE;
				break;

			case MOD_ROCKET_SPLASH:
				if (IsFemale(self))
					message = DM_GENERIC_TEXT_OBT_MOD_ROCKET_SPLASH_F;
				else
					message = DM_GENERIC_TEXT_OBT_MOD_ROCKET_SPLASH_M;
				break;

			case MOD_C4:
				if (IsFemale(self))
					message = DM_GENERIC_TEXT_OBT_MOD_C4_F;
				else
					message = DM_GENERIC_TEXT_OBT_MOD_C4_M;
				break;

			case MOD_CLAYMORE:
				message = DM_GENERIC_TEXT_OBT_MOD_CLAYMORE;
				break;

			case MOD_NEURAL_GRENADE:
				if (IsFemale(self))
					message = DM_GENERIC_TEXT_OBT_MOD_NEURAL_GRENADE_F;
				else
					message = DM_GENERIC_TEXT_OBT_MOD_NEURAL_GRENADE_M;
				break;
				
			case MOD_GRENADE:
				if (IsFemale(self))
					message = DM_GENERIC_TEXT_OBT_MOD_GRENADE_F;
				else
					message = DM_GENERIC_TEXT_OBT_MOD_GRENADE_M;
				break;

			default:
				if (IsFemale(self))
					message = DM_GENERIC_TEXT_OBT_F;
				else
					message = DM_GENERIC_TEXT_OBT_M;
				break;
			}
	}

	if(message>-1)
	{
		gi.SP_Print_Obit(0,message,self->s.number);

		if (dedicated->value)
		{
			char	consoleMsg[MAX_OSPATH];
			
			gi.SP_SPrint(consoleMsg, MAX_OSPATH, message, self->s.number);
			gi.Sys_ConsoleOutput (consoleMsg);	
		}

		self->enemy = NULL;

		return;
	}

	self->enemy = attacker;

	if(attacker && attacker->client)
	{
		switch (mod)
		{
			case MOD_KNIFE_SLASH:
				message = DM_GENERIC_TEXT_OBT_MOD_KNIFE_SLASH;
				break;

			case MOD_KNIFE_THROWN:
				message = DM_GENERIC_TEXT_OBT_MOD_KNIFE_THROWN;
				break;

			case MOD_PISTOL1:
				message = DM_GENERIC_TEXT_OBT_MOD_PISTOL1;
				break;

			case MOD_PISTOL2:
				message = DM_GENERIC_TEXT_OBT_MOD_PISTOL2;
				break;

			case MOD_MPISTOL:
				message = DM_GENERIC_TEXT_OBT_MOD_MPISTOL;
				break;

			case MOD_ASSAULTRIFLE:
				message = DM_GENERIC_TEXT_OBT_MOD_ASSAULTRIFLE;
				break;

			case MOD_SNIPERRIFLE:
				message = DM_GENERIC_TEXT_OBT_MOD_SNIPERRIFLE;
				break;

			case MOD_AUTOSHOTGUN:
				message = DM_GENERIC_TEXT_OBT_MOD_AUTOSHOTGUN;
				break;

			case MOD_SHOTGUN:
				message = DM_GENERIC_TEXT_OBT_MOD_SHOTGUN;
				break;

			case MOD_MACHINEGUN:
				message = DM_GENERIC_TEXT_OBT_MOD_MACHINEGUN;
				break;

			case MOD_PHOS_GRENADE:
				message = DM_GENERIC_TEXT_OBT_MOD_PHOS_GRENADE_2;
				break;

			case MOD_ROCKET:
				message = DM_GENERIC_TEXT_OBT_MOD_ROCKET;
				break;

			case MOD_ROCKET_SPLASH:
				message = DM_GENERIC_TEXT_OBT_MOD_ROCKET_SPLASH;
				break;

			case MOD_MPG:
				message = DM_GENERIC_TEXT_OBT_MOD_MPG;
				break;

			case MOD_FLAMEGUN:
				message = DM_GENERIC_TEXT_OBT_MOD_FLAMEGUN;
				break;

			case MOD_FLAMEGUN_NAPALM:
				message = DM_GENERIC_TEXT_OBT_MOD_FLAMEGUN_NAPALM;
				break;

			case MOD_C4:
				message = DM_GENERIC_TEXT_OBT_MOD_C4;
				break;

			case MOD_CLAYMORE:
				message = DM_GENERIC_TEXT_OBT_MOD_CLAYMORE_2;
				break;

			case MOD_NEURAL_GRENADE:
				message = DM_GENERIC_TEXT_OBT_MOD_NEURAL_GRENADE;
				break;
			
			case MOD_TELEFRAG:
				message = DM_GENERIC_TEXT_OBT_MOD_TELEFRAG;
				break;

			case MOD_GRENADE:
				message = DM_GENERIC_TEXT_OBT_MOD_GRENADE_2;
				break;

			case MOD_CONC_GRENADE:
				message = DM_GENERIC_TEXT_OBT_MOD_CONC_GRENADE;
				break;
		}

		if(message>-1)
		{
			gi.SP_Print_Obit(0,message,self->s.number,attacker->s.number);						
			if (dedicated->value)
			{
				char	consoleMsg[MAX_OSPATH];
				gi.SP_SPrint(consoleMsg, MAX_OSPATH, message, self->s.number, attacker->s.number);
				gi.Sys_ConsoleOutput (consoleMsg);	
			}
			return;
		}
	}

	gi.SP_Print_Obit(0,DM_GENERIC_TEXT_OBT_DIED,self->s.number);

	if (dedicated->value)
	{
		char		consoleMsg[MAX_OSPATH];

		if (message>-1)
		{
			gi.SP_SPrint(consoleMsg, MAX_OSPATH, message, self->s.number);
			gi.Sys_ConsoleOutput (consoleMsg);	
		}
		else
		{
			gi.SP_SPrint(consoleMsg, MAX_OSPATH, DM_GENERIC_TEXT_OBT_DIED, self->s.number);
			gi.Sys_ConsoleOutput (consoleMsg);	
		}
	}

}

/*
==================
gamerules_c::clientScoreboardMessage
==================
*/

int TeamCompare(void const *a, void const *b)
{
	struct TeamInfo_s	*A, *B;

	A = (struct TeamInfo_s *)a;
	B = (struct TeamInfo_s *)b;

	return (A->score < B->score);
}

void gamerules_c::clientScoreboardMessage (edict_t *ent, edict_t *killer, qboolean log_file)
{
	int					i,j,k;
	int					sorted[MAX_CLIENTS];
	int					sortedscores[MAX_CLIENTS];
	struct TeamInfo_s	teams[MAX_CLIENTS];
	int					score,total,total_teams, real_total;
	int					x,y,top;
	gclient_t			*cl;
	edict_t				*cl_ent;

	total=total_teams = 0;

	// Is logging to a log-file enabled?

	if(log_file)
		openLogFile();

	// Sort all the clients by score from highest to lowest.

	for(i=0;i<game.maxclients;i++)
	{
		cl_ent=g_edicts+1+i;

		if(!cl_ent->inuse)
			continue;

		// Spectators get added separately if there's any room left.

		if(cl_ent->client->resp.spectator)
			continue;

		score=game.clients[i].resp.score;

		for(j=0;j<total;j++)
		{
			if(score>sortedscores[j])
			{
				break;
			}
		}

		for(k=total;k>j;k--)
		{
			sorted[k]=sorted[k-1];
			sortedscores[k]=sortedscores[k-1];
		}
		
		sorted[j]=i;
		sortedscores[j]=score;
		total++;

		if(!dm->dmRule_TEAMPLAY())
			continue;

		if(!strcmp(Info_ValueForKey(cl_ent->client->pers.userinfo, "skin"),Info_ValueForKey (cl_ent->client->pers.userinfo, "teamname")))
		{
			// Ensure those clients with no team affiliation (i.e. skin == teamname)
			// don't produce a team aggregate score.

			continue;
		}
		
		for(j=0;j<total_teams;j++)
		{
			if (OnSameTeam(g_edicts + 1 + teams[j].rep, cl_ent))
			{
				teams[j].score += score;
				break;
			}
		}

		if (j == total_teams)
		{
			teams[total_teams].rep = i;
			teams[total_teams].score = score;
			teams[total_teams].teamname = Info_ValueForKey (cl_ent->client->pers.userinfo, "teamname");
			total_teams++;
		}
	}

	qsort((void *)teams, total_teams, sizeof(TeamInfo_s), TeamCompare);

	real_total=total;
	if(total>12)
	{
		total=12;
	}
	if (total_teams > 6)
	{
		total_teams = 6;
	}
	
	// Clear the client's layout.

	top=32;
	gi.SP_Print(ent,DM_GENERIC_LAYOUT_RESET);

	// Handle team aggregate scores if teamplay is on.

	if (dm->dmRule_TEAMPLAY())
	{
		top = -64;
		y = top;

		for(i=0;i<total_teams;i++)
		{		
			x=(i>=3)?160:0;
			
			if(i==3)
			{
				y = top;
			}

			gi.SP_Print(ent,DM_GENERIC_LAYOUT_SCOREBOARD_TEAM,
						(short)x,(short)y,
						teams[i].rep,
						(short)teams[i].score);

			// Write the scores to the open log-file if we have one.

			if(filePtr)
			{
				fprintf(filePtr,"%sTeam %s\n",log_file_line_header->string,teams[i].teamname);
				fprintf(filePtr,"%sScore %i\n",log_file_line_header->string,teams[i].score);
			}

			y+=32;
		}

		if(filePtr)
		{
		 	fprintf(filePtr,"\n");
		}

		if (total_teams >= 3)
		{
			top=64;
		}
		else
		{
			top = 32;
		}
	}


	y = top;

	// Send the scores for all active players to the client's layout.

	for(i=0;i<total;i++)
	{
		cl=&game.clients[sorted[i]];
		cl_ent=g_edicts+1+sorted[i];

		x=(i>=6)?160:0;
		
		if(i==6)
			y=top;

		gi.SP_Print(ent,DM_GENERIC_LAYOUT_SCOREBAORD_CLIENT,
					(short)x,(short)y,
					sorted[i],
					(short)cl->resp.score,
					(unsigned short)cl->ping,
					(unsigned short)((level.framenum-cl->resp.enterframe)/600));

		y+=32;
	}

	// Send the scores for all spectators to the client's layout - if there's
	// any room left on the scoreboard.

	j=0;

	while((j<game.maxclients)&&(i<12))
	{
		cl=&game.clients[j];
		cl_ent=g_edicts+1+j;

		if((!cl_ent->inuse)||(!cl_ent->client->resp.spectator))
		{
			j++;
			continue;
		}

		x=(i>=6)?160:0;
		
		if(i==6)
			y=top;

		gi.SP_Print(ent,DM_GENERIC_LAYOUT_SCOREBAORD_SPECTATOR,
					(short)x,(short)y,
					j,
					(unsigned short)cl->ping,
					(unsigned short)((level.framenum-cl->resp.enterframe)/600));

		y+=32;
		i++;
		j++;
	}

	// Write the scores to the open log-file if we have one.

	if(filePtr)
	{
		// Active players.

		for(i=0;i<real_total;i++)
		{
			cl=&game.clients[sorted[i]];
			cl_ent=g_edicts+1+sorted[i];
			
			fprintf(filePtr,"%sClient %s\n",log_file_line_header->string,cl_ent->client->pers.netname);
			fprintf(filePtr,"%sScore %i\n",log_file_line_header->string,cl->resp.score);
			fprintf(filePtr,"%sPing %i\n",log_file_line_header->string,cl->ping);
			fprintf(filePtr,"%sTime %i\n%s\n",log_file_line_header->string,(level.framenum-cl->resp.enterframe)/600,log_file_line_header->string);
		}

		// Spectators.

		for(i=0;i<game.maxclients;i++)
		{
			cl=&game.clients[i];
			cl_ent=g_edicts+1+i;

			if((!cl_ent->inuse)||(!cl_ent->client->resp.spectator))
				continue;
				
			fprintf(filePtr,"%sClient %s\n",log_file_line_header->string,cl_ent->client->pers.netname);
			fprintf(filePtr,"%s %s\n",log_file_line_header->string,"spectator");
			fprintf(filePtr,"%sPing %i\n",log_file_line_header->string,cl->ping);
			fprintf(filePtr,"%sTime %i\n%s\n",log_file_line_header->string,(level.framenum-cl->resp.enterframe)/600,log_file_line_header->string);
		}
	}

	// Close the open log file if we have one.

	closeLogFile();
}


/*
==================
gamerules_c::clientCalculateWaver
==================
*/

float gamerules_c::clientCalculateWaver(edict_t &ent, int atkID)
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
	speed *= 3.0 / 80000.0;//max possible miss fire is 30 degrees
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
====================
gamerules_c::clientGetMovescale
====================
*/

float gamerules_c::clientGetMovescale(edict_t *ent)
{
	return(g_movescale->value-(g_movescale->value*ent->client->moveScale));
}

/*
====================
gamerules_c::IsUnprotectedGoreZone
====================
*/

bool gamerules_c::IsUnprotectedGoreZone(gz_code zone)
{
	if ((zone == GZ_SHLDR_RIGHT_FRONT) || (zone == GZ_SHLDR_RIGHT_BACK) || (zone == GZ_SHLDR_LEFT_FRONT) ||
		(zone == GZ_SHLDR_LEFT_BACK) || (zone == GZ_CHEST_FRONT) || (zone == GZ_CHEST_BACK) ||
		(zone == GZ_GUT_FRONT_EXTRA) || (zone == GZ_GUT_BACK_EXTRA))
	{
		return false;
	}	
	return true;
}

/*
====================
gamerules_c::IsLimbZone
====================
*/

bool gamerules_c::IsLimbZone(gz_code zone)
{
	if ((zone == GZ_ARM_UPPER_RIGHT) || (zone == GZ_ARM_UPPER_LEFT) || (zone == GZ_ARM_LOWER_RIGHT) ||
		(zone == GZ_ARM_LOWER_LEFT) || (zone == GZ_LEG_UPPER_RIGHT_FRONT) || (zone == GZ_LEG_UPPER_RIGHT_BACK) ||
		(zone == GZ_LEG_UPPER_LEFT_FRONT) || (zone == GZ_LEG_UPPER_LEFT_BACK) || (zone == GZ_LEG_LOWER_RIGHT) ||
		(zone == GZ_LEG_LOWER_LEFT) || (zone == GZ_FOOT_RIGHT) || (zone == GZ_FOOT_LEFT))
	{
		return true;
	}
	return false;
}

/*
====================
gamerules_c::IsShootableWeapon
====================
*/

bool gamerules_c::IsShootableWeapon(char *weaponname)
{
	if(!strcmp(weaponname, "w_pistol1") || !strcmp(weaponname, "w_knife"))
	{	
		return false;
	}
	return true;
}

/*
==================
gamemode_c::setDMMode
==================
*/

void gamemode_c::setDMMode(int newtype)
{
	gametype = newtype;
	rules = gamerules[newtype];
	gi.configstring(CS_SHOWNAMES,rules->arePlayerNamesOn()?"1":"0");
	gi.configstring(CS_SHOWTEAMS,rules->arePlayerTeamsOn()?"1":"0");

	rules->registerPlayerIcons();
}