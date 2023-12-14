#include "g_local.h"
#include "w_weapons.h"
#include "ds.h"

#include "p_body.h"
#include "ai.h"
#include "m_generic.h"
#include "q_sh_interface.h"
#include "../strings/dm_generic.h"
#include "ef_flags.h"
#include "configstring.h"
#include "music.h"
#include "p_heights.h"

//yuck :/
// and how! :<

void ClientUserinfoChanged (edict_t *ent, char *userinfo, bool not_first_time);
void ClientUserinfoChangedBasic (edict_t *ent, char *userinfo);
void clUse (edict_t *ent);
void clearFireEvent(edict_t *ent);

bool IsInnocent(edict_t *ent);

//dunno if this is a good final resting place for these guys, but...

/*QUAKED info_merc_start (1 .1 .1) (-16 -16 -24) (16 16 32)
Starting point for merc buddies.
--------------------------------
merctype - the default merc type (can be overridden by player during mission setup)
	0 - GRUNT
	1 - DEMO
	2 - MEDIC
	3 - SNIPER
	4 - HEAVY
	5 - TECH

mercnum - the merc's id number (0 to 3)
*/

/*void SP_info_merc_start(edict_t *self)
{
// HACK for demo!
// end hack
	
	generic_monster_spawnnow(self);

	switch (st.merctype)
	{
	case 1:
		self->ai = ai_c::Create(AI_GENERIC_MERC_DEMO, self, "enemy/meso", "mercdemo1");//new merc_demo_ai(self);
		self->think = generic_grunt_init;
		break;
	case 3:
	default:
		self->ai = ai_c::Create(AI_GENERIC_MERC_SNIPER, self, "enemy/meso", "mercsnipe1");//new merc_sniper_ai(self);
		self->think = generic_grunt_init;
		break;
	}
}
*/

/*QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 32)
The normal starting point for a level.
*/

void SP_info_player_start(edict_t *self)
{
/*	IPathInst	*Test;
	vec3_t		dest, source;
	PathPoint_s	Point;

	source[0] = -1444.625;
	source[1] = 1551.375;
	source[2] = 32.125;
	dest[0] = -1667.456421;
	dest[1] = 968.364624;
	dest[2] = 40.03125;


	Test = (IPathInst *)gi.GetPath();

	Test->SetSource(source);
	Test->SetDest(dest);
	if (Test->FindSimplePath())
	{
		while( Test->GetPoint(Point))
		{
			Com_Printf("[%5.0f %5.0f %5.0f] - [%5.0f %5.0f %5.0f]\n",
				Point.points[0][0],Point.points[0][1],Point.points[0][2],
				Point.points[1][0],Point.points[1][1],Point.points[1][2]);
		}
	}
*/
}

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -24) (16 16 32)
potential spawning position for deathmatch games
*/

void SP_info_player_deathmatch(edict_t *self)
{
	if (!dm->isDM())
	{
		G_FreeEdict (self);
		return;
	}
	self->solid = SOLID_NOT;
	self->svflags |= SVF_NOCLIENT;
}

/*QUAKED info_player_team1 (1 0 0) (-16 -16 -24) (16 16 32)
potential spawning position for team deathmatch games (team1 players)
*/

void SP_info_player_team1(edict_t *self)
{
	if(!dm->isDM())
	{
		G_FreeEdict(self);
		return;
	}
	self->solid = SOLID_NOT;
	self->svflags |= SVF_NOCLIENT;
}

/*QUAKED info_player_team2 (1 0 0) (-16 -16 -24) (16 16 32)
potential spawning position for team deathmatch games (team2 players)
*/

void SP_info_player_team2(edict_t *self)
{
	if(!dm->isDM())
	{
		G_FreeEdict(self);
		return;
	}
	self->solid = SOLID_NOT;
	self->svflags |= SVF_NOCLIENT;
}

/*QUAKED info_player_intermission (1 0 1) (-16 -16 -24) (16 16 32)
The deathmatch intermission point will be at one of these
Use 'angles' instead of 'angle', so you can set pitch or roll as well as yaw.  'pitch yaw roll'
*/
void script_use(edict_t *ent, edict_t *other, edict_t *activator);

void SP_info_player_intermission(edict_t *ent)
{
/*	// copied right out of SP_scriptrunner
	char	temp[MAX_PATH];
	int		i;

	sprintf(temp,"ds/%s.os",st.script);
	ent->Script = new CScript(temp, ent);
	Scripts.push_back(ent->Script);

	for(i=0;i<NUM_PARMS;i++)
	{
		if (st.parms[i])
		{
			ent->Script->SetParameter(st.parms[i]);
		}
		else
		{
			break;
		}
	}

	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	ent->svflags |= SVF_NOCLIENT;
	ent->use = script_use;*/
}

//=======================================================================

void player_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	// player pain is handled at the end of the frame in P_DamageFeedback
}

/*
==================
LookAtKiller
==================
*/
void LookAtKiller (edict_t *self, edict_t *inflictor, edict_t *attacker)
{
	vec3_t		dir;

	if (attacker && attacker != world && attacker != self)
	{
		VectorSubtract (attacker->s.origin, self->s.origin, dir);
	}
	else if (inflictor && inflictor != world && inflictor != self)
	{
		VectorSubtract (inflictor->s.origin, self->s.origin, dir);
	}
	else
	{
		self->client->killer_yaw = self->s.angles[YAW];

		return;
	}

	self->client->killer_yaw = 180/M_PI*atan2(dir[1], dir[0]);
}

/*
==================
player_die
==================
*/

void player_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	VectorClear (self->avelocity);

	self->takedamage = DAMAGE_YES;
	self->movetype = MOVETYPE_TOSS;
	self->solid = SOLID_CORPSE;
//	self->clipmask = MASK_SOLID;	// Use this only if we want corpses to fall through things...

	self->s.angles[0] = 0;
	self->s.angles[2] = 0;
	self->s.sound = 0;

	self->maxs[2] = -8;

	self->svflags |= SVF_DEADMONSTER;

	// Even though we are dead - we can still be 'killed' again... so ensure
	// some things can only happen the 1st time we are killed.

	if(!self->deadflag)
	{
		if(dm->isDM())
			self->client->respawn_time = level.time + 1.0;
		else
			self->client->respawn_time = level.time + RESPAWN_DELAY;
		
		LookAtKiller(self, inflictor, attacker);
		self->client->ps.pmove.pm_type = PM_DEAD;
		dm->clientObituary(self, inflictor, attacker);

		sharedEdict_t sh;
		sharedEdict_t *saved;

		// Do mod-specific weapon handling on death of player.

		sh.inv=(inven_c *)self->client->inv;
		saved=sh.inv->getOwner();
		dm->clientDieWeaponHandler(self);
		sh.inv->setOwner(saved);

		// Do mod-specific death stuff.

		sh.inv=(inven_c *)self->client->inv;
		saved=sh.inv->getOwner();
		if (!attacker)
		{
			attacker = self;
		}
		if (!inflictor)
		{
			inflictor = self;
		}
		dm->clientDie(*self, *inflictor, *attacker);
		sh.inv->setOwner(saved);

		// Start a death animation.

		self->client->anim_priority = ANIM_DEATH;
		
		if(dm->isDM())
		{
			if(attacker&&attacker->client)
			{	
				// The attacker died.

				if (attacker == self)
				{	
					// The attacker killed himself.

					gi.sound(attacker,CHAN_VOICE,gi.soundindex("dm/selffrag.wav"),1.0,ATTN_STATIC,0,SND_LOCALIZE_CLIENT);
				}			
				else if (OnSameTeam(attacker, self))
				{
					// The attacker killed a teammate.

					gi.sound(attacker,CHAN_VOICE,gi.soundindex("dm/selffrag.wav"),1.0,ATTN_STATIC,0,SND_LOCALIZE_CLIENT);
				}
				else
				{	
					if(deathmatch->value!=2) // Hack - assassin has it's own kill sounds.
					{
						// Play a "got a kill" sound.

						gi.sound(attacker,CHAN_VOICE,gi.soundindex("dm/frag.wav"),1.0,ATTN_STATIC,0,SND_LOCALIZE_CLIENT);
					}
				}
			}

			// Show scoreboard.

			Cmd_Score_f(self);
		}
	}

	self->deadflag = DEAD_DEAD;

	gi.linkentity (self);

	// We recycle to fade to black...

	self->health = -1;
}

//=======================================================================

/*
==============
InitClientPersistant

This is only called when the game first initializes in single player,
but is called after each death and level change in deathmatch
==============
*/

void InitClientPersistant (gclient_t *client)
{
	memset(&client->pers, 0, sizeof(client->pers));

	client->pers.health		= 100;
	client->pers.max_health	= 100;
	client->pers.curWeaponType = -1;
	client->pers.connected	= true;
}

/*
==============
InitClientResp
==============
*/

void InitClientResp (gclient_t *client)
{
	// do NOT fuck around with team, since this is set elsewhere
	team_t team = client->resp.team;
	memset(&client->resp, 0, sizeof(client->resp));
	
	client->resp.team = team;
	client->resp.enterframe = level.framenum;
}

/*
==================
PreserveClientData

Some information that should be persistant, like health, 
is still stored in the edict structure, so it needs to
be mirrored out to the client structure before all the
edicts are wiped.
=========1=========
*/

void PreserveClientData (void)
{
	int		i;
	edict_t	*ent;

	for(i = 0; i < game.maxclients; i++)
	{
		ent = g_edicts + i + 1;
		if (!ent->inuse)
		{
			continue;
		}
		game.clients[i].pers.health = ent->health;
		game.clients[i].pers.max_health = ent->max_health;
		game.clients[i].pers.curWeaponType = ent->client->inv->getCurWeaponType();
		game.clients[i].pers.fov=ent->client->ps.fov;
	}
}

/*
==================
RestoreClientData
==================
*/

void RestoreClientData (edict_t *ent)
{
	ent->health = ent->client->pers.health;
	ent->max_health = ent->client->pers.max_health;
}

/*
=======================================================================

  SelectSpawnPoint

=======================================================================
*/

/*
================
PlayersRangeFromSpot

Returns the distance to the nearest player from the given spot
================
*/

float	PlayersRangeFromSpot (edict_t *spot)
{
	edict_t	*player;
	float	bestplayerdistance;
	vec3_t	v;
	int		n;
	float	playerdistance;


	bestplayerdistance = 9999999;

	for (n = 1; n <= (int)maxclients->value; n++)
	{
		player = &g_edicts[n];

		if (!player->inuse)
			continue;

		if (player->health <= 0)
			continue;

		VectorSubtract (spot->s.origin, player->s.origin, v);
		playerdistance = VectorLength (v);

		if (playerdistance < bestplayerdistance)
			bestplayerdistance = playerdistance;
	}

	return bestplayerdistance;
}

/*
================
SelectRandomDeathmatchSpawnPoint

Go to a random point, but NOT the two points closest to other players.
================
*/

edict_t *SelectRandomDeathmatchSpawnPoint (void)
{
	edict_t	*spot, *spot1, *spot2;
	int		count = 0;
	int		selection;
	float	range, range1, range2;

	spot = NULL;
	range1 = range2 = 99999;
	spot1 = spot2 = NULL;

	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL)
	{
		count++;
		range = PlayersRangeFromSpot(spot);
		if (range < range1)
		{
			range1 = range;
			spot1 = spot;
		}
		else if (range < range2)
		{
			range2 = range;
			spot2 = spot;
		}
	}

	if (!count)
		return NULL;

	if (count <= 2)
	{
		spot1 = spot2 = NULL;
	}
	else
		count -= 2;

	selection = rand() % count;

	spot = NULL;
	do
	{
		spot = G_Find (spot, FOFS(classname), "info_player_deathmatch");
		if (spot == spot1 || spot == spot2)
			selection++;
	} while(selection--);

	return spot;
}

/*
================
SelectFarthestDeathmatchSpawnPoint
================
*/

edict_t *SelectFarthestDeathmatchSpawnPoint (void)
{
	edict_t	*bestspot;
	float	bestdistance, bestplayerdistance;
	edict_t	*spot;


	spot = NULL;
	bestspot = NULL;
	bestdistance = 0;
	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL)
	{
		bestplayerdistance = PlayersRangeFromSpot (spot);

		if (bestplayerdistance > bestdistance)
		{
			bestspot = spot;
			bestdistance = bestplayerdistance;
		}
	}

	if (bestspot)
	{
		return bestspot;
	}

	// if there is a player just spawned on each and every start spot
	// we have no choice to turn one into a telefrag meltdown
	spot = G_Find (NULL, FOFS(classname), "info_player_deathmatch");

	return spot;
}

/*
================
SelectTeamDeathmatchSpawnPoint
================
*/

edict_t *SelectTeamDeathmatchSpawnPoint (edict_t *ent)
{
	edict_t	*spot, *spot1, *spot2;
	int		count = 0;
	int		selection;
	float	range, range1, range2;
	char	*teamname,*cname;

	// we may have a team already assigned to us
	switch (ent->client->resp.team)
	{
		case TEAM1:
			cname = "info_player_team1";
			break;

		case TEAM2:
			cname = "info_player_team2";
			break;

		default:
			teamname=Info_ValueForKey(ent->client->pers.userinfo,"teamname");
			if(!stricmp(teamname,"blue"))
				cname = "info_player_team1";
			else if(!stricmp(teamname,"red"))
				cname = "info_player_team2";
			else
				return(SelectRandomDeathmatchSpawnPoint());
			break;
	}

	// Same as before....

	spot = NULL;
	range1 = range2 = 99999;
	spot1 = spot2 = NULL;

	while ((spot = G_Find (spot, FOFS(classname), cname)) != NULL)
	{
		count++;
		range = PlayersRangeFromSpot(spot);

		if (range < range1)
		{
			range1 = range;
			spot1 = spot;
		}
		else if (range < range2)
		{
			range2 = range;
			spot2 = spot;
		}
	}

	if (!count)
		return SelectRandomDeathmatchSpawnPoint();

	// Ok, we know there are spots out there, so let's pick one.

	if (count <= 2)
	{
		spot1 = spot2 = NULL;
	}
	else
		count -= 2;

	selection = rand() % count;

	spot = NULL;

	do
	{
		spot = G_Find (spot, FOFS(classname), cname);

		if((spot == spot1 || spot == spot2))
			selection++;
	} while(selection--);

	return(spot);
}

edict_t *SelectDeathmatchSpawnPoint (void)
{
	return SelectFarthestDeathmatchSpawnPoint ();
}

/*
===========
SelectSpawnPoint

Chooses a player start, deathmatch start, coop start, etc
============
*/

void SelectSpawnPoint (edict_t *ent, vec3_t origin, vec3_t angles)
{
	edict_t	*spot = NULL;

	if (dm->isDM())
	{
		if (dm->arePlayerTeamsOn())
		{
			spot = SelectTeamDeathmatchSpawnPoint(ent);
		}
		else
		{
			spot = SelectDeathmatchSpawnPoint ();
		}
		
#ifndef _DEBUG
		if(!spot)
		{
			gi.error ("Couldn't find deathmatch spawn point");
		}
#endif
	}

	// Find a single player start spot.
	if (!spot)
	{
		while ((spot = G_Find (spot, FOFS(classname), "info_player_start")) != NULL)
		{
			if (!game.spawnpoint[0] && !spot->targetname)
				break;

			if (!game.spawnpoint[0] || !spot->targetname)
				continue;

			if (stricmp(game.spawnpoint, spot->targetname) == 0)
				break;
		}

		if (!spot)
		{
			if (!game.spawnpoint[0])
			{	
				// There wasn't a spawnpoint without a target, so use any.

				spot = G_Find (spot, FOFS(classname), "info_player_start");
			}
			
			if (!spot)
			{
				gi.error ("Couldn't find spawn point %s", game.spawnpoint);
			}
		}
	}

	VectorCopy (spot->s.origin, origin);
	origin[2] += 9;
	VectorCopy (spot->s.angles, angles);
}

//======================================================================

void InitBodyQue (void)
{
	int		i;
	edict_t	*ent;

	level.body_que = 0;
	for (i=0; i<BODY_QUEUE_SIZE ; i++)
	{
		ent = G_Spawn();
		ent->classname = "bodyque";
	}
}

void respawn (edict_t *self)
{
	if (dm->isDM())
	{
		PB_MakeCorpse(self);		
		PutClientInServer (self);

		if(!self->client->pers.spectator)
		{
			// Add a teleportation effect.
			FX_SetEvent(self, EV_PLAYER_TELEPORT);

			// Hold in place briefly.
			self->client->ps.pmove.pm_flags = PMF_TIME_TELEPORT;
			self->client->ps.pmove.pm_time = 14;
		}

		// Kill any existing client effects on the player when dying...
		self->s.effects |= EF_KILL_EFT;
		
		self->client->respawn_time = level.time;
		return;
	}

	// Restart the entire server.

	gi.AddCommandString ("respawn\n");
}

/*
===========
spectator_respawn

Called when pers.spectator changes. Note that resp.spectator should be the
opposite of pers.spectator here.
============
*/

void spectator_respawn (edict_t *ent)
{
	int i, numspec;

	// If the user wants to become a spectator, make sure he doesn't exceed 
	// max_spectators.

	if (ent->client->pers.spectator)
	{
		char *value = Info_ValueForKey (ent->client->pers.userinfo, "spectator_password");
		
		if (*spectator_password->string && strcmp(spectator_password->string, "none") && 
			strcmp(spectator_password->string, value))
		{
			gi.SP_Print(ent,DM_GENERIC_TEXT_SPECTATOR_BAD_PWD);
			ent->client->pers.spectator = false;
			
			// Reset client's spectator var.

			gi.WriteByte (svc_stufftext);
			gi.WriteString ("spectator 0\n");
			gi.unicast(ent, true);

			return;
		}

		// Count spectators.

		for (i = 1, numspec = 0; i <= (int)maxclients->value; i++)
			if (g_edicts[i].inuse && g_edicts[i].client->pers.spectator)
				numspec++;

		// Too many?

		if(numspec > maxspectators->value)
		{
			gi.SP_Print(ent,DM_GENERIC_TEXT_SPECTATOR_LIMIT_FULL);
			ent->client->pers.spectator = false;
			
			// Reset client's spectator var.

			gi.WriteByte (svc_stufftext);
			gi.WriteString ("spectator 0\n");
			gi.unicast(ent, true);

			return;
		}
	}
	else
	{
		// Was a spectator and wants to join the game so must have the right password.

		char *value = Info_ValueForKey (ent->client->pers.userinfo, "password");
	
		if (*password->string && strcmp(password->string, "none") && 
			strcmp(password->string, value))
		{
			gi.SP_Print(ent,DM_GENERIC_TEXT_BAD_PWD);
			ent->client->pers.spectator = true;
			
			// Set client's spectator var.

			gi.WriteByte (svc_stufftext);
			gi.WriteString ("spectator 1\n");
			gi.unicast(ent, true);
			
			return;
		}
	}

	// clear score on respawn
	ent->client->resp.score = 0;

	ent->svflags &= ~SVF_NOCLIENT;
	PutClientInServer (ent);

	// add a teleportation effect
	if (!ent->client->pers.spectator)
	{
		// Add a teleportation effect.

		FX_SetEvent(ent, EV_PLAYER_TELEPORT);

		// hold in place briefly
		ent->client->ps.pmove.pm_flags = PMF_TIME_TELEPORT;
		ent->client->ps.pmove.pm_time = 14;
	}

	// Kill any existing client effects on the player...
	ent->s.effects |= EF_KILL_EFT;

	ent->client->respawn_time = level.time;

	if (ent->client->pers.spectator) 
		gi.SP_Print(ent,DM_GENERIC_TEXT_SPECTATOR_TO_SIDELINES,ent->s.number);
	else
		gi.SP_Print(ent,DM_GENERIC_TEXT_SPECTATOR_TO_GAME,ent->s.number);
}

//==============================================================

void sendRestartPrediction(edict_t *ent)
{
	ent->client->restart_count++;

	gi.ReliableWriteByteToClient(svc_restart_predn,ent->s.number-1);
	gi.ReliableWriteByteToClient(ent->client->restart_count,ent->s.number-1);
}

void RebuildPredictedClientInv(edict_t *ent)
{
	gi.ReliableWriteByteToClient(svc_rebuild_pred_inv, ent->s.number - 1);
	ent->client->inv->NetWrite(ent->s.number - 1);
}

/*
===========
PutClientInServer

Called when a player connects to a server or respawns in a deathmatch.
============
*/

#define RESPAWN_INVULN_TIME		4.0F

bool PB_InitBody(edict_t &ent,char *userinfo);

void PutClientInServer (edict_t *ent)
{
	vec3_t	mins = {-16, -16, -24};
	vec3_t	maxs = {16, 16, 40};
	int		index;
	vec3_t	spawn_origin, spawn_angles;
	gclient_t	*client;
	int		i;
	client_persistant_t	saved;
	client_respawn_t	resp;
	byte				saved_restart_count;
	invPub_c			*savedI;
	player_dmInfo_ic	*savedDMInfo;
	//body info is persistant (sp?)
	body_c				*savedB;
	IGhoulInst			*savedG;
	byte				savedPlayernameColors[MAX_CLIENTS];
	byte				savedOldPlayernameColors[MAX_CLIENTS];
	char				savedOldName[128];
	char				savedOldSkin[MAX_SKINNAME_LENGTH];
	char				savedOldTeamname[MAX_TEAMNAME_LENGTH];

	// Find a spawn point. Do it before setting health back up, so farthest
	// ranging doesn't count this client.

	SelectSpawnPoint (ent, spawn_origin, spawn_angles);

	index = ent-g_edicts-1;
	client = ent->client;

	// Deathmatch wipes most client data every spawn.

	if (dm->isDM())
	{
		char userinfo[MAX_INFO_STRING];

		resp = client->resp;
		memcpy (userinfo, client->pers.userinfo, sizeof(userinfo));
		InitClientPersistant (client);
		ClientUserinfoChangedBasic (ent, userinfo );
	}
	else
	{
		memset (&resp, 0, sizeof(resp));
	}

	// Clear everything except the persistant data (pers), dmInfo, inventory,
	// body and ghoulinst. NOTE: ghoulinst is restored later, after playerstate
	// is cleared - this is only an issue for 3d-person hack).

	saved = client->pers;
	savedDMInfo=client->dmInfo;
	savedI = client->inv;
	savedB = client->body;
	savedG = client->ps.bod;
	memcpy(savedPlayernameColors,client->playernameColors,sizeof(client->playernameColors));
	memcpy(savedOldPlayernameColors,client->oldPlayernameColors,sizeof(client->oldPlayernameColors));
	memcpy(savedOldName, client->oldNetName, sizeof(client->oldNetName));
	memcpy(savedOldSkin, client->oldSkinRequest, sizeof(client->oldSkinRequest));
	memcpy(savedOldTeamname, client->oldTeamnameRequest, sizeof(client->oldTeamnameRequest));
	saved_restart_count=client->restart_count;

	memset (client, 0, sizeof(*client));

	memcpy(client->playernameColors,savedPlayernameColors,sizeof(client->playernameColors));
	memcpy(client->oldPlayernameColors,savedOldPlayernameColors,sizeof(client->oldPlayernameColors));
	memcpy(client->oldNetName, savedOldName, sizeof(client->oldNetName));
	memcpy(client->oldSkinRequest, savedOldSkin, sizeof(client->oldSkinRequest));
	memcpy(client->oldTeamnameRequest, savedOldTeamname, sizeof(client->oldTeamnameRequest));
	client->body = savedB;
	client->inv = savedI;
	client->pers = saved;
	client->dmInfo = savedDMInfo;
	client->restart_count=saved_restart_count;

	// If we died...

	if (client->pers.health <= 0)
		InitClientPersistant(client);
	client->resp = resp;

	client->musicTime = -40;


	// copy some data from the client to the entity
	RestoreClientData (ent);

	// clear entity values
	ent->groundentity = NULL;
	ent->client = &game.clients[index];
	ent->takedamage = DAMAGE_AIM;
	ent->movetype = MOVETYPE_WALK;
	ent->viewheight = EYEBALL_HEIGHT;
	ent->inuse = true;
	ent->classname = "player";
	ent->mass = 200;
	ent->solid = SOLID_BBOX;
	ent->deadflag = DEAD_NO;
	ent->gibbed = false;
	ent->air_finished = level.time + 12;
	ent->clipmask = MASK_PLAYERSOLID;
	ent->pain = player_pain;
	ent->die = player_die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags &= ~FL_NO_KNOCKBACK;
	ent->svflags &= ~SVF_DEADMONSTER;
	ent->burntime = 0;
	ent->phosburntime = 0;
	ent->burninflictor = 0;
	ent->zapfxtime = 0;
	ent->zapdmgtime = 0;

	VectorCopy (mins, ent->mins);
	VectorCopy (maxs, ent->maxs);
	VectorClear (ent->velocity);

	// clear playerstate values
	memset (&ent->client->ps, 0, sizeof(client->ps));

	//restore ghoulinst Now.
	client->ps.bod = savedG;

	client->ps.pmove.origin[0] = spawn_origin[0]*8;
	client->ps.pmove.origin[1] = spawn_origin[1]*8;
	client->ps.pmove.origin[2] = spawn_origin[2]*8;

	client->ps.remote_id=-1;

	client->ps.fov = 95;

	// Mod specific CP stuff.

	dm->clientHandlePredn(*ent);

	// Mod specific respawn stuff.

	dm->clientRespawn(*ent);

	// Fill up clips for all your weapons from the inventory ammo pool.

	sharedEdict_t	sh;
	sh.inv = (inven_c *)ent->client->inv;
	sh.edict = ent;
	sh.inv->setOwner(&sh);
	sh.inv->stockWeapons();

	// Turn all the bits of the ghoul view-weapon model ON.

	IGhoulInst	*gun = client->ps.gun;

	if(gun)
		gun->SetAllPartsOnOff(true);

	// The game will take it from here.

	ent->client->ps.musicID = MUSIC_UNSET;

	// Clear entity state values.

	FX_ClearEvent(ent);

	ent->s.modelindex = 255;		// will use the skin specified model

	// sknum is player num
	ent->s.skinnum = ent - g_edicts - 1;
	
	VectorCopy (spawn_origin, ent->s.origin);
	ent->s.origin[2] += 1;	// make sure off ground

	// Set the delta angles.

	for(i=0;i<3;i++)
		client->ps.pmove.delta_angles[i]=ANGLE2SHORT(spawn_angles[i] - client->resp.cmd_angles[i]);

	ent->s.angles[PITCH] = 0;
	ent->s.angles[YAW] = spawn_angles[YAW];
	ent->s.angles[ROLL] = 0;
	VectorCopy (ent->s.angles, client->ps.viewangles);
	VectorCopy (ent->s.angles, client->v_angle);

	//  I'm a spectator?

	if(client->pers.spectator)
	{
		// Yes.
	
		client->chase_target = NULL;
		client->resp.spectator = true;

		ent->movetype = MOVETYPE_SPECTATOR;
		ent->solid = SOLID_NOT;
		ent->clipmask = 0;					// Prevent spectators from being shot/stabbed/exploded.

		ent->svflags |= SVF_NOCLIENT;
		ent->client->ps.gun = 0;
		gi.linkentity (ent);
	} 
	else
	{
		// No.

		ent->svflags &= ~SVF_NOCLIENT;
		ent->clipmask = MASK_PLAYERSOLID;	// Allow previous spectators to be shot/stabbed/exploded.

		client->resp.spectator = false;

		if (!KillBox (ent))
		{
			// could't spawn in?
		}

		gi.linkentity (ent);

		// Give me 2 seconds of invulnerable time in deathmatch play. During this
		// time I won't be able to fire weapons or use items.

		if(dm->isDM())
			ent->client->respawn_invuln_time=level.time + RESPAWN_INVULN_TIME;

		// If we are playing netplay...

		if(dm->isDM())
		{
			// ...confirm that there are no residual effects on the entity after death...

			fxRunner.clearContinualEffects(ent);
			ent->s.effects &= ~EF_INVIS_PULSE;
			ent->client->ghosted = false;
			
			// ...then put a spawn effect around the player.

			fxRunner.exec("environ/prespawn", ent->s.origin);
		
			gi.sound(ent, CHAN_BODY, gi.soundindex("dm/prespawn.wav"), .6, ATTN_NORM, 0);
		}
	}

	// initialise amount of money the player has (this most likely needs to go into client_persistant)
//	client->m_PlayerStats.Init();
//	game.GameStats->AddClientToStatsList(ent);
	//for buying stuff before a mission--should start up before the whole dam map gets loaded in, and only do it for appropriate maps.
//	game.GameStats->EnterBuyMode(ent);
	
	// Ensure that the primary fire button works normally. Respawning modifies
	// this behaviour.
	
	client->oktofire=true;

	// Start level music playing.

	if(level.baseSong)
		StartMusic(ent, level.baseSong);

	// Reset any damage incurred movement rate reduction.

	ent->client->moveScale=0.0;
}

/*
=====================
ClientBeginDeathmatch

A client has just connected to the server in deathmatch mode, so clear
everything out before starting them.
=====================
*/

void flushPlayernameColors(edict_t &ent);

void ClientBeginDeathmatch (edict_t *ent)
{
	G_InitEdict (ent);

	InitClientResp (ent->client);

	// When first connecting, we always want to start with a zero init'd
	// inventory and issue a (reliable) instruction to restart the player's
	// client-side inventory system.

	W_InitInv(*ent);
	sendRestartPrediction(ent);

	// Flush these out.

	flushPlayernameColors((edict_t &)*ent);

	// Special DM mod-dependent stuff - DO NOT FREAKING MOVE THIS OR YOU WILL FEEL MY WRATH!!!!

	dm->clientConnect(*ent);

	// Drop us into the game.

	PB_KillBody(*ent);

	PutClientInServer (ent);
	gi.SP_Print(NULL, DM_GENERIC_ENTERED, ent->client->pers.netname);	// client might not know this name yet
	gi.welcomeprint(ent);

	// Make sure all view stuff is valid.

	ClientEndServerFrame (ent);
}

/*
===========
ClientBegin

Called when a client has finished connecting, and is ready to be placed into
the game. This will happen every level load.
============
*/

void ClientBegin (edict_t *ent)
{
	int		i;

	ent->client = game.clients + (ent - g_edicts - 1);

	// Special for DM.

	if (dm->isDM())
	{
		ClientBeginDeathmatch (ent);
		return;
	}

	// 1/11/00 kef -- it was the case that replaying a level would allow you to see the wonderful
	//'Mission Objectives Updated' message at the top of the screen during the second showing of
	//the intro cinematic. not good. I hope and hope and hope it's okay to zero out the missionStatus
	//here, cuz I don't want to have to figure out a way to save it during the intro.
	if (level.missionStatus)
	{
		level.missionStatus = 0;
	}

	// If this ever gets written to do anything - do not move this without checking it's
	// safe to do so!!!

	dm->clientConnect(*ent);

	// If there is already a body waiting for us (a loadgame), just take it,
	// otherwise spawn one from scratch.

	if (ent->inuse)
	{
		// The client has cleared the client side viewangles upon connecting
		// to the server, which is different than the state when the game is
		// saved, so we need to compensate with deltaangles.

		for (i=0 ; i<3 ; i++)
			ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(ent->client->ps.viewangles[i]);

		// Issue a (reliable) instruction to restart the player's client-side
		// inventory system then issue a (reliable) instruction to re-sync the
		// player's client-side inventory with the player's server side inventory
		// system.

		sendRestartPrediction(ent);
		RebuildPredictedClientInv(ent);
		W_RefreshWeapon(*ent);
		ent->client->ps.fov=ent->client->pers.fov;
	}
	else
	{
		// A spawn point will completely reinitialize the entity except for the
		// persistant data that was initialized at ClientConnect() time.

		G_InitEdict (ent);
		ent->classname = "player";
		InitClientResp (ent->client);

		if(!ent->client->inv)
		{
			// When first connecting, we always want to start with a zero init'd
			// inventory and issue a (reliable) instruction to restart the player's
			// client-side inventory system.

			W_InitInv(*ent);
			sendRestartPrediction(ent);
		}
		else
		{
			// Loading from an autosave... we need to restore pers.curWeaponType.

			if(ent->client->inv->getCurWeaponType())
				ent->client->pers.curWeaponType=ent->client->inv->getCurWeaponType();
		}

		// Drop us into the game.

		PutClientInServer (ent);
	}

	if (level.intermissiontime)
	{
		MoveClientToIntermission (ent, false);
	}
	else
	{
		// Send welcome message  if in a multiplayer game.
		if (game.maxclients > 1)
		{
			gi.welcomeprint(ent);
		}
	}

	// Make sure all view stuff is valid.

	ClientEndServerFrame (ent);

	extern edict_t*	WorldSpawnScriptRunner;

	if (WorldSpawnScriptRunner) // run script specified in worldspawn key/values
	{
		WorldSpawnScriptRunner->use(WorldSpawnScriptRunner, WorldSpawnScriptRunner, ent);
	}

}


/*
===========
ClientUserInfoChanged

Called whenever the player updates a userinfo variable. The game can override
any of the settings in place (forcing skins or names, etc) before copying it off.
============
*/

void ClientUserinfoChangedBasic (edict_t *ent, char *userinfo)
{
	char	*s;
	int		playernum;

	// Check for malformed or illegal info strings.
	
	if (!Info_Validate(userinfo))
		strcpy (userinfo, "\\name\\badinfo\\skin\\mullins");

	// Set name.

	s = Info_ValueForKey (userinfo, "name");
	strncpy (ent->client->pers.netname, s, sizeof(ent->client->pers.netname)-1);

	// Set spectator mode.

	s = Info_ValueForKey (userinfo, "spectator");
	
	if (deathmatch->value && *s && strcmp(s, "0"))
		ent->client->pers.spectator = true;
	else
		ent->client->pers.spectator = false;

	// Save bestweapon value (off, safe or unsafe)?

	s = Info_ValueForKey (userinfo, "bestweap");
	strncpy (ent->client->pers.bestweap,s,sizeof(ent->client->pers.bestweap)-1);

	// Init the player's body. If not everything went as planned, adjust skin
	// and teaminfo accordingly. Single player always defaults to mullins,
	// regardless of what the client's skin and teamname are set to.

	if((ent->client->pers.connected)||(!dm->isDM()))
	{
   		char skin[128],teamname[128];

   		Com_sprintf(teamname,sizeof(teamname),Info_ValueForKey(userinfo,"teamname"));
   		Com_sprintf(skin,sizeof(skin),Info_ValueForKey(userinfo,"skin"));

   		if(!PB_InitBody(*ent,userinfo))
   		{
   			// Wern't able to use skin / teamname combination, so get correct skin
   			// and teamname...

   			char newSkin[128],newTeamname[128];

   			PB_GetActualSkinName(ent,newSkin);
   			PB_GetActualTeamName(ent,newTeamname);

   			if(dm->isDM())
   			{
   				// ... and make sure skin and teamname are valid on the client
   				// - flaggin them as corrected (*) in order for the client to
   				// display a warning message in DM.

   				if(stricmp(newSkin,skin))
   				{
   					skin[0]='*';
   					skin[1]=0;
   					strcat(skin+1,newSkin);	//FIXME:
   					Info_SetValueForKey(userinfo,"skin",newSkin);
   				}

   				if(strcmp(newTeamname,teamname))
   				{
   					teamname[0]='*';
   					teamname[1]=0;
   					strcat(teamname+1,newTeamname);	//FIXME:
   					Info_SetValueForKey(userinfo,"teamname",newTeamname);
   				}
   			}
   			else
   			{
   				// ... but in Single player we don't care, so just make sure skin
   				// and teamname are valid on the client.

   				strcpy(skin,newSkin);	//FIXME:
   				strcpy(teamname,newTeamname);//FIXME:
   				Info_SetValueForKey(userinfo,"skin",newSkin);
   				Info_SetValueForKey(userinfo,"teamname",newTeamname);
   			}

   			// Combine name, teamname and skin into a configstring.

   			playernum = ent-g_edicts-1;

   			gi.configstring(CS_PLAYERSKINS+playernum,va("%s\\%s\\%s",ent->client->pers.netname,teamname,skin));
		}
	}


	// Save off the userinfo in case we want to check something later.

	strncpy (ent->client->pers.userinfo, userinfo, sizeof(ent->client->pers.userinfo)-1);

	if(ent->client->inv)
	{
		sharedEdict_t	sh;
	
		sh.inv = (inven_c *)ent->client->inv;
		sh.edict = ent;
		sh.inv->setOwner(&sh);

		sh.inv->rulesSetBestWeaponMode(Info_ValueForKey(ent->client->pers.userinfo,"bestweap"));
	}
}



/*
===========
ClientUserInfoChanged

Called whenever the player updates a userinfo variable. The game can override
any of the settings in place (forcing skins or names, etc) before copying it off.
============
*/

void ClientUserinfoChanged (edict_t *ent, char *userinfo, bool not_first_time)
{
	char	*s;
	int		playernum;
	char	tempSkin[128],tempTeamname[128];

	// Check for malformed or illegal info strings.
	
	if (!Info_Validate(userinfo))
		strcpy (userinfo, "\\name\\badinfo\\skin\\mullins");

	// Set name.

	s = Info_ValueForKey (userinfo, "name");
	if(stricmp(ent->client->pers.netname, s))
	{
		strncpy (ent->client->pers.netname, s, sizeof(ent->client->pers.netname)-1);
		playernum = ent-g_edicts-1;

		PB_GetActualSkinName(ent,tempSkin);
		PB_GetActualTeamName(ent,tempTeamname);

		gi.configstring(CS_PLAYERSKINS+playernum,va("%s\\%s\\%s",ent->client->pers.netname,tempTeamname,tempSkin));
	}

	// Set spectator mode.

	s = Info_ValueForKey (userinfo, "spectator");
	
	if (deathmatch->value && *s && strcmp(s, "0"))
		ent->client->pers.spectator = true;
	else
		ent->client->pers.spectator = false;

	// Save bestweapon value (off, safe or unsafe)?

	s = Info_ValueForKey (userinfo, "bestweap");
	strncpy (ent->client->pers.bestweap,s,sizeof(ent->client->pers.bestweap)-1);

	// Init the player's body. If not everything went as planned, adjust skin
	// and teaminfo accordingly. Single player always defaults to mullins,
	// regardless of what the client's skin and teamname are set to.

	if((ent->client->pers.connected)||(!dm->isDM()))
	{
		if (dm->isDM() && (deathmatch->value == DM_CTF))
		{
			dm->AssignTeam(ent, userinfo);
		}
		else
		{
			char skin[128],teamname[128];

			Com_sprintf(teamname,sizeof(teamname),Info_ValueForKey(userinfo,"teamname"));
			Com_sprintf(skin,sizeof(skin),Info_ValueForKey(userinfo,"skin"));

			if(!PB_InitBody(*ent,userinfo))
			{
				// Wern't able to use skin / teamname combination, so get correct skin
				// and teamname...

				char newSkin[128],newTeamname[128];

				PB_GetActualSkinName(ent,newSkin);
				PB_GetActualTeamName(ent,newTeamname);

				if(dm->isDM())
				{

					// ... and make sure skin and teamname are valid on the client
					// - flaggin them as corrected (*) in order for the client to
					// display a warning message in DM.

					if(stricmp(newSkin,skin))
					{
						skin[0]='*';
						skin[1]=0;
						strcat(skin+1,newSkin);	//FIXME:
						Info_SetValueForKey(userinfo,"skin",newSkin);
					}

					if(strcmp(newTeamname,teamname))
					{
						teamname[0]='*';
						teamname[1]=0;
						strcat(teamname+1,newTeamname);	//FIXME:
						Info_SetValueForKey(userinfo,"teamname",newTeamname);
					}
					
					// If something changed, and we are in teamplay mode, and it's 
					// not our first time through AND we're not specatating, then 
					// we'd better gib the player.

					if ((teamname[0] =='*' || skin[0] == '*') && (dm->dmRule_TEAMPLAY()) && not_first_time &&
						(ent->client->pers.spectator==false))
					{
						ent->flags &= ~FL_GODMODE;
						ent->health = 0;
						meansOfDeath = MOD_SUICIDE;
						player_die (ent, ent, ent, 100000, vec3_origin);
						// Don't even bother waiting for death frames before respawning.
						ent->deadflag = DEAD_DEAD;
					}
				}
				else
				{
					// ... but in Single player we don't care, so just make sure skin
					// and teamname are valid on the client.

					strcpy(skin,newSkin);	//FIXME:
					strcpy(teamname,newTeamname);//FIXME:
					Info_SetValueForKey(userinfo,"skin",newSkin);
					Info_SetValueForKey(userinfo,"teamname",newTeamname);
				}

				// Combine name, teamname and skin into a configstring.

				playernum = ent-g_edicts-1;

				gi.configstring(CS_PLAYERSKINS+playernum,va("%s\\%s\\%s",ent->client->pers.netname,teamname,skin));
			}
 			
			// If the teamname changed, and we are in teamplay mode, and it's
			// not our first time through AND we're not spectating, then we'd 
			// better gib the player.

 			if (dm->isDM() && (dm->dmRule_TEAMPLAY()) && (not_first_time && teamname[0] == '*') && 
			    (ent->client->pers.spectator==false))
 			{
 				ent->flags &= ~FL_GODMODE;
 				ent->health = 0;
 				meansOfDeath = MOD_SUICIDE;
 				player_die (ent, ent, ent, 100000, vec3_origin);	
				// Don't even bother waiting for death frames before respawning.
 				ent->deadflag = DEAD_DEAD;
 			}
		}
	}

	// Save off the userinfo in case we want to check something later.

	strncpy (ent->client->pers.userinfo, userinfo, sizeof(ent->client->pers.userinfo)-1);

	if(ent->client->inv)
	{
		sharedEdict_t	sh;
	
		sh.inv = (inven_c *)ent->client->inv;
		sh.edict = ent;
		sh.inv->setOwner(&sh);

		sh.inv->rulesSetBestWeaponMode(Info_ValueForKey(ent->client->pers.userinfo,"bestweap"));
	}
}

void W_KillInv(edict_t &ent)
{
	if(ent.client->inv)
		pe->KillInv((inven_c *)ent.client->inv);

	ent.client->inv=NULL;
}

void W_InitInv(edict_t &ent)
{
	W_KillInv(ent);

	ent.client->inv=pe->NewInv();
}

void W_RefreshWeapon(edict_t &ent)
{
	sharedEdict_t	sh;

	sh.inv = (inven_c *)ent.client->inv;
	sh.edict = &ent;
	sh.inv->setOwner(&sh);

	pe->RefreshWeapon((inven_c *)ent.client->inv);
}

/*
===========
ClientConnect

Called when a player begins connecting to the server.
The game can refuse entrance to a client by returning false.
If the client is allowed, the connection process will continue
and eventually get to ClientBegin()
Changing levels will NOT cause this to be called again, but
loadgames will.
============
*/

qboolean ClientConnect (edict_t *ent, char *userinfo)
{
	char	*value;

	// check to see if they are on the banned IP list
	value = Info_ValueForKey (userinfo, "ip");
	if (gi.FilterPacket(value))
	{
		Info_SetValueForKey(userinfo, "rejmsg", "rejected_banned");
		return false;
	}

	// Check for a spectator.

	value = Info_ValueForKey (userinfo, "spectator");
	
	if (deathmatch->value && *value && strcmp(value, "0")) 
	{
		int i, numspec;

		if (*spectator_password->string && strcmp(spectator_password->string, "none") &&
			strcmp(spectator_password->string, Info_ValueForKey (userinfo, "spectator_password")))
		{
			Info_SetValueForKey(userinfo, "rejmsg", "rejected_spectator_password");
			return false;
		}

		// Count spectators.

		for (i = numspec = 0; i < (int)maxclients->value; i++)
			if (g_edicts[i+1].inuse && g_edicts[i+1].client->pers.spectator)
				numspec++;

		if (numspec > maxspectators->value)
		{
			Info_SetValueForKey(userinfo, "rejmsg", "rejected_spectator_limit_exeeded");
			return false;
		}
	} 
	else
	{
		// Check for a password.

		value = Info_ValueForKey (userinfo, "password");

		if (*password->string && strcmp(password->string, "none") && 
			strcmp(password->string, value))
		{
			Info_SetValueForKey(userinfo, "rejmsg", "rejected_password");
			return false;
		}
	}

	// they can connect
	ent->client = game.clients + (ent - g_edicts - 1);

	// if there is already a body waiting for us (a loadgame), just
	// take it, otherwise spawn one from scratch
	if (ent->inuse == false)
	{
		// clear the respawning variables
		ent->client->resp.team = NOTEAM;
		InitClientResp (ent->client);
		InitClientPersistant (ent->client);
	}

	// InitClientPersistant() sets this true - I want it false just this 1st
	// time round... it's already set true below anyhow -MW.
	ent->client->pers.connected = false;

	ClientUserinfoChanged (ent, userinfo, false);

	if (game.maxclients > 1)
		gi.dprintf ("%s connected\n", ent->client->pers.netname);

	ent->client->pers.connected = true;

	return true;
}

/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.
============
*/

void ClientDisconnect (edict_t *ent)
{
	if(!ent->client)
		return;

	gi.bprintf(PRINT_HIGH, "%s disconnected\n", ent->client->pers.netname);


	// Need to drop weapons in Realistic DM before our GhoulInsts are hosed

	dm->clientPreDisconnect(*ent);	

	// 1/13/00 kef -- clean up our buddies
	RemoveLeanBuddy(ent);
	RemoveNugBuddy(ent);
	RemoveWeaponBuddy(ent);

	// Remove any ghoul instances I owned.

	PB_KillBody(*ent);

	//	Here's the safety-valve remover:
	game_ghoul.RemoveObjectInstances(ent);

	// Don't want to be hanging around causing invisible agro do we!!

	gi.unlinkentity(ent);		// Unlink me before making change to ent->solid.
	ent->solid = SOLID_NOT;		// What it says.	
	gi.linkentity(ent);			// Relink to update my world presence.
	gi.unlinkentity(ent);		// Unlink me again before I say bye-bye to all.

	// Reset any other required stuff.

	ent->s.modelindex = 0;
	ent->inuse = false;
	ent->classname = "disconnected";
	ent->client->pers.connected = false;
	gi.configstring(CS_PLAYERSKINS+ent-g_edicts-1, "");

	// Do any rules specific disconnect stuff - DO NOT FREAKING MOVE THIS OR YOU WILL FEEL MY WRATH!!!!

	dm->clientDisconnect(*ent);
}


//==============================================================


edict_t	*pm_passent;

// pmove doesn't need to know about passent and contentmask
trace_t	PM_trace (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end)
{
	trace_t	trace;

	if (pm_passent->health > 0)
		gi.trace (start, mins, maxs, end, pm_passent, MASK_PLAYERSOLID, &trace);
	else
		gi.trace (start, mins, maxs, end, pm_passent, MASK_DEADSOLID, &trace);

	return(trace);
}

void runWeapon(edict_t *ent)
{
	sharedEdict_t sh;

	sh.inv = (inven_c *)ent->client->inv;
	sh.edict = ent;
	sh.inv->setOwner(&sh);

	sh.attack = ((ent->client->latched_buttons|ent->client->buttons)&BUTTON_ATTACK)&&ent->client->oktofire;
	sh.altattack = (ent->client->latched_buttons|ent->client->buttons)&BUTTON_ALTATTACK;
	sh.weap3 = (ent->client->latched_buttons|ent->client->buttons)&BUTTON_WEAP3;
	sh.weap4 = (ent->client->latched_buttons|ent->client->buttons)&BUTTON_WEAP4;
	sh.leanLeft = (ent->viewside > 0.0);
	sh.leanRight = (ent->viewside < 0.0);
	sh.weaponkick_angles=ent->client->weaponkick_angles;
	sh.cinematicFreeze = ent->client->ps.cinematicfreeze;
	sh.rejectSniper = (ent->client->ps.remote_type != REMOTE_TYPE_FPS);
	sh.inv->setClientPredicting(strcmp(Info_ValueForKey(ent->client->pers.userinfo,"predicting"),"0"));
	sh.framescale=1.0;

	sh.inv->frameUpdate();
	
	// Attacking removes any existing invulnerabilty due to respawning.

	if(dm->isDM()&&(sh.attack|sh.altattack))	
		ent->client->respawn_invuln_time=0.0F;
}

/*
==============
g_checkOnOtherPlayer
==============
*/

int	g_checkOnOtherPlayer(unsigned int groundEntity)
{
	int entnum=((edict_t *)groundEntity)->s.number;

	if((entnum>=1)&&(entnum<=maxclients->value))
	{
		//gi.dprintf("SV: On player %\n",entnum);
		return 1;
	}

	return 0;
}

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame.
==============
*/

void ClientThink (edict_t *ent, usercmd_t *ucmd)
{
	gclient_t	*client;
	edict_t	*other;
	int		i, j;
	pmove_t	pm;

	level.current_entity = ent;
	client = ent->client;

	if (level.intermissiontime)
	{
		if(ent->client->pers.spectator)
			ent->client->ps.pmove.pm_type = PM_SPECTATOR_FREEZE;
		else
			ent->client->ps.pmove.pm_type = PM_FREEZE;

		// Can exit intermission after ten seconds.

		if (level.time > level.intermissiontime + 10.0 && (ucmd->buttons & BUTTON_ATTACK) )
			level.exitintermission = true;

		return;
	}

	pm_passent = ent;

	if (ent->client->chase_target)
	{
		client->resp.cmd_angles[0] = SHORT2ANGLE(ucmd->angles[0]);
		client->resp.cmd_angles[1] = SHORT2ANGLE(ucmd->angles[1]);
		client->resp.cmd_angles[2] = SHORT2ANGLE(ucmd->angles[2]);
	} 
	else 
	{
		// Quake2 doesn't handle low client framerates very well. This can be seen
		// in the way jumpheights vary depending on framerate. If client framerate
		// is <30fps, run some extra simulation steps. Note: this doesn't mean more
		// CPU time 'cos if the client was running quick enough to avoid any problems
		// with framerate, we'd be getting more move commands and thus doing more
		// pmoves anyhow.

		int msec=ucmd->msec;	// Save the msec.
		int iterations=0;

		if((msec>33)&&ent->velocity[2])
		{	
			ucmd->msec=10;
			iterations=msec/10;
			ucmd->msec+=msec%10;
		}
		else
		{
			ucmd->msec=msec;
			iterations=1;
		}

		for(int ic=0;ic<iterations;ic++)
		{
			// Set up for pmove.
			memset (&pm, 0, sizeof(pm));

			if (ent->movetype == MOVETYPE_NOCLIP)
				client->ps.pmove.pm_type = PM_NOCLIP;
			else if (ent->movetype == MOVETYPE_SPECTATOR)
				client->ps.pmove.pm_type = PM_SPECTATOR;
			else if (ent->s.modelindex != 255)
				client->ps.pmove.pm_type = PM_GIB;
			else if (ent->deadflag)
				client->ps.pmove.pm_type = PM_DEAD;
			else if (client->ps.remote_type > REMOTE_TYPE_TPS)
			{
				// We're in a remote camera view, so set pm_type to show this.
				client->ps.pmove.pm_type = PM_CAMERA_FREEZE;
			}
			else
				client->ps.pmove.pm_type = PM_NORMAL;

			client->ps.pmove.gravity = sv_gravity->value;
			pm.s = client->ps.pmove;

			for (i=0 ; i<3 ; i++)
			{
				pm.s.origin[i] = ent->s.origin[i]*8;
				pm.s.velocity[i] = ent->velocity[i]*8;
			}

			if(memcmp(&client->old_pmove, &pm.s, sizeof(pm.s)))//&&(ic==0))
			{
				pm.snapinitial = true;
			//		gi.dprintf ("pmove changed!\n");
			}

			pm.cmd = *ucmd;

			// Handle run key.

			ent->client->running=false;
			if((pm.cmd.buttons&BUTTON_RUN)&&(!(ent->client->ps.pmove.pm_flags&PMF_FATIGUED)))
			{
				pm.cmd.forwardmove*=2;
				pm.cmd.sidemove*=2;
				pm.cmd.upmove*=2;
				ent->client->running=true;
			}

			// Handle movement scaling for limping etc.

			float moveScale=((float)((byte)(dm->clientGetMovescale(ent)*255)))/255;
			pm.cmd.forwardmove*=moveScale;
			pm.cmd.sidemove*=moveScale;

			pm.trace = PM_trace;	// adds default parms
			pm.pointcontents = gi.pointcontents;
			pm.checkOnOtherPlayer = g_checkOnOtherPlayer;

			// Friction can be reduced on the server side by something like a knockback...
			if (client->friction_time > level.time)
			{
				pm.friction_loss = client->friction_time - level.time;
			}

			// Do the move.
			gi.Pmove (&pm);

			// Save results of pmove.
			client->ps.pmove = pm.s;
			client->old_pmove = pm.s;

			for (i=0 ; i<3 ; i++)
			{
				ent->s.origin[i] = pm.s.origin[i]*0.125;
				ent->velocity[i] = pm.s.velocity[i]*0.125;
			}
		
			VectorCopy (pm.mins, ent->mins);
			VectorCopy (pm.maxs, ent->maxs);

			client->resp.cmd_angles[0] = SHORT2ANGLE(ucmd->angles[0]);
			client->resp.cmd_angles[1] = SHORT2ANGLE(ucmd->angles[1]);
			client->resp.cmd_angles[2] = SHORT2ANGLE(ucmd->angles[2]);

			if (ent->groundentity && !pm.groundentity && (pm.cmd.upmove >= 10) && (pm.waterlevel == 0))
			{
					gi.sound(ent, CHAN_VOICE, gi.soundindex("player/jump.wav"), .6, ATTN_NORM, 0);
			//		PlayerNoise(ent, ent->s.origin, PNOISE_SELF);
			}

			ent->viewside = pm.cmd.lean;
			ent->viewheight = pm.viewheight;
			ent->waterlevel = pm.waterlevel;
			ent->watertype = pm.watertype;
			ent->groundentity = pm.groundentity;
			if (pm.groundentity)
				ent->groundentity_linkcount = pm.groundentity->linkcount;

			if (ent->deadflag)
			{
				client->ps.viewangles[ROLL] = 40;
				client->ps.viewangles[PITCH] = -15;
				client->ps.viewangles[YAW] = client->killer_yaw;
			}
			else// if (client->ps.remote_id < 0)
			{
				// 1st and 3rd person views, but NOT remote camera views, allow us to mess with the angles.

				VectorCopy (pm.viewangles, client->v_angle);
				VectorCopy (pm.viewangles, client->ps.viewangles);
			}

			gi.linkentity (ent);

			ent->svflags &= ~SVF_ISHIDING;

			if ((ent->movetype != MOVETYPE_NOCLIP)&&(ent->movetype != MOVETYPE_SPECTATOR))
				G_TouchTriggers (ent);

			// touch other objects
			for (i=0 ; i<pm.numtouch ; i++)
			{
				other = pm.touchents[i];
				for (j=0 ; j<i ; j++)
					if (pm.touchents[j] == other)
						break;
				if (j != i)
					continue;	// duplicated
				if (!other->touch)
					continue;
				other->touch (other, ent, NULL, NULL);
			}

			ucmd->msec=10;
		}

		ucmd->msec=msec;	// Restore the msec.
	}

	client->oldbuttons = client->buttons;
	client->buttons = ucmd->buttons;
	client->latched_buttons |= client->buttons & ~client->oldbuttons;

	if(ucmd->fireEvent)
		ent->client->fireEvent=ucmd->fireEvent;

	if(ucmd->altfireEvent)
		ent->client->altfireEvent=ucmd->altfireEvent;

	if(!client->oktofire)
	{
		if((client->buttons&(client->buttons^client->oldbuttons))&BUTTON_ATTACK)
			client->oktofire=true;
		else
			ent->client->fireEvent=0;
	}

	// Save light level the player is standing on for monster sighting AI.

	ent->light_level = ucmd->lightlevel;

	IsPlayerTargetSameTeam(ent);

	// Very poorly named!

	clUse(ent);

	if(client->resp.spectator)
	{
		if(client->latched_buttons & BUTTON_ATTACK)
		{
			// Toggle between normal spectator mode / chase cam.

			if (client->chase_target)
			{
				// Ensure we don't get stuck in BSP when coming out of chasecam.
				
				vec3_t	mins={-20,-20,-24},maxs={20,20,40};// Don't screw with these!!!
				trace_t trace;

				gi.trace(ent->s.origin,mins,maxs,ent->s.origin,ent,MASK_SOLID,&trace);

				if(trace.startsolid)
					VectorCopy(client->chase_target->s.origin,ent->s.origin);

				// Clear chasecam stuff.

				client->chase_target = NULL;
				client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
			} 
			else
			{
				GetChaseTarget(ent);
			}
		}
		else if(client->latched_buttons & BUTTON_USE)
		{
			// Select next / prev chase cam target (or select one if not in chase cam).	

			if (client->chase_target)
				ChaseNext(ent);
			else
				GetChaseTarget(ent);
		}

		client->latched_buttons = 0;
	}

	// Update chase cam(s) if I'm being followed.

	for(i=1; i <= (int)maxclients->value; i++)
	{
		other = g_edicts + i;

		if (other->inuse && other->client->chase_target == ent)
			UpdateChaseCam(other);
	}
}


extern int notargglobalcheck;

/*
==============
ClientBeginServerFrame

This will be called once for each server frame, before running any other entities
in the world.
==============
*/

void ClientBeginServerFrame (edict_t *ent)
{
	gclient_t	*client;
	int			buttonMask;

	if (level.intermissiontime)
		return;

	UpdateMusicLevel(ent);

	client = ent->client;

	if(deathmatch->value &&
	   client->pers.spectator != client->resp.spectator &&
	   (level.time - client->respawn_time) >= 5)
	{
		spectator_respawn(ent);
		return;
	}

	if (ent->deadflag)
	{
		// Wait for any button just going down.

		if ( level.time > client->respawn_time)
		{
			// In deathmatch, only wait for attack button.

			if (dm->isDM())
				buttonMask = BUTTON_ATTACK;
			else
				buttonMask = -1;

			if((client->latched_buttons & buttonMask)||(dm->isDM()&&dm->dmRule_FORCE_RESPAWN()))
			{
				respawn(ent);

				client->latched_buttons = 0;

				// When the fire button is depressed to respawn, we need to ensure that
				// the respawning player's weapon won't fire at the same time. Only when
				// they press the fire button again will weapon firing operate.

				client->oktofire=false;
				client->buttons|=BUTTON_ATTACK;
				client->oldbuttons|=BUTTON_ATTACK;
			}
		}

		return;
	}

	// Run weapon animations (if not a lactator, er.. spectator I mean).
	
	 if(!client->resp.spectator)
		runWeapon(ent);

	if(client->inv)
	{
		if(client->inv->inDisguise())
		{
			ent->flags |= FL_NOTARGET;
		}
		else if(!notargglobalcheck)
		{
			ent->flags &= ~(FL_NOTARGET);
		}
	}

	// Add player trail so monsters can follow.

	if (!dm->isDM())
		if (!gmonster.Visible (ent, PlayerTrail_LastSpot() ) )
			PlayerTrail_Add (ent->s.origin);

	client->latched_buttons = 0;

	// 1/12/00 kef -- update thePickupList every frame. explains the function name, doesn't it?
	thePickupList.FrameUpdate();
}

void clUse (edict_t *ent)
{
	float		useRange;
	edict_t		*curSearch=NULL;
	vec3_t		looking;
	edict_t		*bestSearch = NULL;
	vec3_t		source;
	trace_t		tr;

	if (!ent->client)return;

	//this func only gets called when a button is released...
	if((!(ent->client->oldbuttons & BUTTON_USE))||(ent->client->buttons & BUTTON_USE))return;

	useRange = 80;
	AngleVectors(ent->client->ps.viewangles,looking,NULL,NULL);

	VectorCopy(ent->s.origin, source);
	source[2] += ent->client->ps.viewoffset[2];

	//search around me for the nearest ent that i can use
	//gack! this is not ideal for bmodels!
/*	CRadiusContent rad(source, useRange);

	for(int i = 0; i < rad.getNumFound(); i++) // this doesn't work here so don't freakin' use it
	{
		curSearch = rad.foundEdict(i);

*/
	// first, check if our bbox is intersecting with a trigger
	if (curSearch = performTriggerSearch(ent, source, (ent->maxs[0] - ent->mins[0])*0.5))
	{	// yup. we're in a trigger.
	}
	else
	{	// not in a trigger. check for nearby entities, then.
		curSearch = performEntitySearch(ent, source, useRange);
	}
	bestSearch = curSearch;

	//if i found someone with the search, use im
	if (bestSearch && bestSearch->plUse)
	{
		bestSearch->plUse(bestSearch,ent,ent);
	}
	else
	{//if i didn't find anything, do a traceline to find something to use (to catch bmodels that have weird origins)
		VectorScale(looking, 75, looking);
		VectorAdd(looking, source, looking);
		
		gi.trace(source, NULL, NULL, looking, ent, MASK_PLAYERSOLID, &tr);

		if((tr.fraction < 1.0)&&(tr.ent == world))
		{	// I would like this to happen only on double taps since use is also a lean sort of thing
			FX_StrikeWall(tr.endpos, tr.surface->flags>>24);
			PlayerNoise(ent, ent->s.origin, AI_SENSETYPE_SOUND_INVESTIGATE, 0, 400, gmonster.GetClientNode());//base radius off surface type?
		}
		else if (tr.ent && tr.ent->plUse)
		{
			tr.ent->plUse(tr.ent,ent,ent);
		}
	}
}

/*
==============
GetPlayerStats
==============
*/
/*
CPlayerStats *GetPlayerStats(edict_t *ent)
{
	return(&ent->client->m_PlayerStats);
}
*/

void IsPlayerTargetSameTeam(edict_t *source)
{
	vec3_t start;
	vec3_t fwd;
	vec3_t end;

	assert(source->client);

	VectorCopy(source->s.origin, start);
	start[2] += 26;//?

	vec3_t right;
	AngleVectors(source->client->ps.viewangles, fwd, right, 0);

	VectorMA(start, 2.5 * source->client->ps.kick_angles[ROLL], right, start);//account for lean

	VectorMA(start, 2000, fwd, end);

	trace_t tr;

	gi.trace(start, 0, 0, end, source, MASK_PROJ, &tr);

	// 1/5/00 kef -- if our trace hit someone's LeanBuddy(TM) or NugBuddy(TM), treat it like the 
	//trace actually hit the buddy's owner
	if (tr.ent && (tr.ent->svflags & SVF_BUDDY) && GetBuddyOwner(tr.ent))
	{
		tr.ent = GetBuddyOwner(tr.ent);

		//make sure the owner of this buddy is really infront of me
		vec3_t toBuddyOwnerDir;
		VectorSubtract(tr.ent->s.origin, source->s.origin, toBuddyOwnerDir);
		VectorNormalize(toBuddyOwnerDir);
		//yikes! not in front of me!
		if (DotProduct(fwd, toBuddyOwnerDir)<0)
		{
			source->client->ps.stats[STAT_FLAGS] &= ~(STAT_FLAG_TEAM|STAT_FLAG_HIT);
			return ;
		}
	}

	// If we're looking at a dead thing, we don't wanna color the crosshair.

	if (tr.ent && (tr.ent->health <= 0))
	{
		source->client->ps.stats[STAT_FLAGS] &= ~(STAT_FLAG_TEAM|STAT_FLAG_HIT);
		return;
	}

	if (OnSameTeam(source, tr.ent))
	{
		source->client->ps.stats[STAT_FLAGS] |= STAT_FLAG_HIT|STAT_FLAG_TEAM;
		return;
	}

	// don't color the crosshair for iraqi civilians and the like
	if ((IsInnocent(tr.ent))&&(!dm->isDM()))
	{
		source->client->ps.stats[STAT_FLAGS] &= ~(STAT_FLAG_TEAM|STAT_FLAG_HIT);
		return;
	}

	if ((tr.ent->takedamage)&&((tr.ent->ai)||(tr.ent->client)))
	{
		source->client->ps.stats[STAT_FLAGS] &= ~STAT_FLAG_TEAM;
		source->client->ps.stats[STAT_FLAGS] |= STAT_FLAG_HIT;
		return;
	}
	source->client->ps.stats[STAT_FLAGS] &= ~(STAT_FLAG_TEAM|STAT_FLAG_HIT);

	return ;
}

void AddWeaponTypeForPrecache(int type)
{
	level.weaponsAvailable |= (1<<type);
}

void incMovescale(edict_t *ent,float inc)
{
	// kef -- if you get here without an edict, well, that's just plain bad. if you get here
	//with an edict but no client, a corpse was shot
	if (ent && ent->client)
	{
		ent->client->moveScale=((ent->client->moveScale+inc)>0.5)?0.5:ent->client->moveScale+inc;
	}
}

void resetMovescale(edict_t *ent,float newScale)
{
	if (ent && ent->client)
	{
		ent->client->moveScale=newScale;
	}
}

float getFireEvent(edict_t *ent)
{
	return(ent->client->fireEvent);
}

void clearFireEvent(edict_t *ent)
{
	ent->client->fireEvent=0;
}

float getAltfireEvent(edict_t *ent)
{
	return(ent->client->altfireEvent);
}

void clearAltfireEvent(edict_t *ent)
{
	ent->client->altfireEvent=0;
}