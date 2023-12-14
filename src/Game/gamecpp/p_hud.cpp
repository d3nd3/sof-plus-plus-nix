#include "g_local.h"
#include "q_sh_interface.h"
#include "fields.h"
#include "..\qcommon\ef_flags.h"

extern float s_levelStatusBegin;	// declared in g_cmds.cpp


/*
==================
MoveClientToIntermission
==================
*/

void MoveClientToIntermission(edict_t *ent, qboolean log_file)
{
	if (dm->isDM())
		ent->client->showscores = true;

	VectorCopy (level.intermission_origin, ent->s.origin);
	ent->client->ps.pmove.origin[0] = level.intermission_origin[0]*8;
	ent->client->ps.pmove.origin[1] = level.intermission_origin[1]*8;
	ent->client->ps.pmove.origin[2] = level.intermission_origin[2]*8;
	VectorCopy (level.intermission_angle, ent->client->ps.viewangles);
	
	if(ent->client->pers.spectator)
		ent->client->ps.pmove.pm_type = PM_SPECTATOR_FREEZE;
	else
		ent->client->ps.pmove.pm_type = PM_FREEZE;

	ent->client->ps.blend[3] = 0;
	ent->client->ps.rdflags &= ~RDF_UNDERWATER;

	ent->viewheight = 0;
	ent->s.modelindex = 0;
	ent->s.effects = 0;
	ent->s.sound = 0;
	ent->solid = SOLID_NOT;
	ent->s.renderfx &= ~(RF_GHOUL);

	// Add the layout.

	if (dm->isDM())
		dm->clientScoreboardMessage(ent,NULL,log_file);

	// Need to clear ps.gun or we'll end up trying to use an invalid ghoul
	// instance in SV_BuildClientFrame().

	ent->client->ps.gun=0;
}

/*
==================
BeginIntermission
==================
*/

void BeginIntermission (edict_t *targ)
{
	int		i;
	edict_t	*ent,*client;

	if (level.intermissiontime)
	{
		// Intermission already activated.

		return;
	}

	game.autosaved = false;

	// Respawn any dead clients.

	for (i=0 ; i < (int)maxclients->value ; i++)
	{
		client = g_edicts + 1 + i;

		if (!client->inuse)
			continue;
		
		if (client->health <= 0)
			respawn(client);
	}

	level.intermissiontime = level.time;
	level.changemap = targ->map;

	if(strstr(level.changemap, "*"))
	{
		// New unit started. What are we doing in SoF? New mission?
	}
	else
	{
		if (!dm->isDM())
		{
			// Go immediately to the next level.

			level.exitintermission = 1;

			// Ensure player's gun is turned off.

			g_edicts[1].client->ps.gun=0;

			return;
		}
	}

	level.exitintermission = 0;

	// Find an intermission spot.

	ent = G_Find (NULL, FOFS(classname), "info_player_intermission");
	
	if (!ent)
	{	
		// The map creator forgot to put in an intermission point.

		ent = G_Find (NULL, FOFS(classname), "info_player_start");
		
		if (!ent)
			ent = G_Find (NULL, FOFS(classname), "info_player_deathmatch");
	}
	else
	{	
		// Chose one of four spots.

		i = rand() & 3;
		
		while (i--)
		{
			ent = G_Find (ent, FOFS(classname), "info_player_intermission");
			if (!ent)
			{
				// Wrap around the list.

				ent = G_Find (ent, FOFS(classname), "info_player_intermission");
			}
		}
	}

	VectorCopy (ent->s.origin, level.intermission_origin);
	VectorCopy (ent->s.angles, level.intermission_angle);

	// Move all clients to the intermission point.

	for (i=0 ; i < (int)maxclients->value ; i++)
	{
		client = g_edicts + 1 + i;
	
		if (!client->inuse)
			continue;

		if (!i)
			MoveClientToIntermission(client, true);
		else
			MoveClientToIntermission(client, false);
	}
}

/*
==================
Cmd_Score_f

Display the scoreboard.
==================
*/

void Cmd_Score_f (edict_t *ent)
{
	ent->client->showinventory = false;
	ent->client->showhelp_time = level.time;

	if(ent->client->showscores)
	{
		ent->client->showscores = false;
		return;
	}

	ent->client->showscores = true;
	dm->clientScoreboardMessage(ent,ent->enemy,false);
}

/*
==================
Cmd_Help_f

Display the help message.
==================
*/

void Cmd_Help_f (edict_t *ent)
{
	if(level.intermissiontime)
		return;

	ent->client->showinventory = false;
	ent->client->showscores = false;

	if(ent->client->showhelp_time > level.time)
	{
		ent->client->showhelp_time = level.time;
		return;
	}

	ent->client->showhelp_time = level.time+30.0;
	dm->clientHelpMessage(ent);
}

//=======================================================================

/*
===============
G_SetStats
===============
*/

void G_SetStats (edict_t *ent)
{
	float		fMaxObjectivesDisplayTime = 5.0;

	vec3_t holdorigin;
	trace_t tr;

	//
	// health
	//

	ent->client->ps.stats[STAT_HEALTH] = ent->health;

	//
	// ammo
	//

	ent->client->ps.stats[STAT_CLIP_AMMO] = ent->client->inv->getCurClip();
	ent->client->ps.stats[STAT_CLIP_MAX] = ent->client->inv->getClipMax();
	ent->client->ps.stats[STAT_AMMO] = ent->client->inv->getCurAmmo();
	ent->client->ps.stats[STAT_WEAPON] = (!level.intermissiontime)?ent->client->inv->getCurWeaponType():0;
	ent->client->ps.stats[STAT_INV_TYPE] = ent->client->inv->getCurItemType();
	ent->client->ps.stats[STAT_INV_COUNT] = ent->client->inv->getCurItemAmount();
	ent->client->ps.stats[STAT_ARMOR] = ent->client->inv->getArmorCount();
	ent->client->ps.stats[STAT_FLAGS] = (ent->client->ps.stats[STAT_FLAGS] & 0xf0) | (ent->client->inv->scopeIsActive() ? STAT_FLAG_SCOPE : 0);


	// We kill all effects on the entity when respawned.  This checks for and releases it.
	if (ent->client->respawn_time+0.5 <= level.time && ent->client->RemoteCameraLockCount<=0)
	{
		ent->s.effects &= ~EF_KILL_EFT;
	}

#if 1 
	if(ent->client->respawn_invuln_time>level.time)
	{
		if(!ent->client->ghosted)
		{
			IGhoulInst *inst=ent->ghoulInst;

			if(inst)
			{
				float r,g,b,a;

				inst->GetTint(&r,&g,&b,&a);
				a=0.5F;
				inst->SetTint(r, g, b, a);

				ent->client->ghosted=true;
			}
		}
	}
	else
	{
		if(ent->client->ghosted)
		{
			IGhoulInst *inst=ent->ghoulInst;

			if(inst)
			{
				float r,g,b,a;

				inst->GetTint(&r,&g,&b,&a);
				a=1.0F;
				inst->SetTint(r, g, b, a);

				ent->client->ghosted=false;
			}	
		}
	}

#else	// The fancier client-side version has a problem, since when a faded actor goes out of the PVS, it doesn't unfade.
	
	// Put the invulnerability effect on the player
	if(ent->client->respawn_invuln_time>level.time)
	{
		if(!ent->client->ghosted)
		{
			// There may be a chance that we are still in a fade, which would interfere with continual fx.
			fxRunner.clearContinualEffects(ent);

			// Add the sparkly invulnerable effect
//			fxRunner.execContinualEffect("environ/invuln", ent);

			// Also add the EF_FLAG to make the model pulse.
			ent->s.effects |= EF_INVIS_PULSE;

			ent->client->ghosted = true;
		}
	}
	else
	{
		if(ent->client->ghosted)
		{
			// Remove the sparkly invulnerable effect
//			fxRunner.stopContinualEffect("environ/invuln", ent);

			// Remove the EF_FLAG that makes the model pulse.
			ent->s.effects &= ~EF_INVIS_PULSE;

			// Make sure the client side tinting is reset.
			FX_SetEvent(ent, EV_TINTCLEAR);

			ent->client->ghosted = false;
		}
	}
#endif

	//
	// layouts
	//

	ent->client->ps.stats[STAT_LAYOUTS] = 0;

	if(dm->isDM())
	{
		// Dead, or end of game, or showing scoreboard or help layouts?

		if(ent->client->pers.health <= 0 || level.intermissiontime ||
		   ent->client->showscores || ent->client->showhelp_time>level.time)
		{
			ent->client->ps.stats[STAT_LAYOUTS] |= 1;
		}

		// DM ranking (not a layout tho' - part of HUD instead).

		ent->client->ps.stats[STAT_LAYOUTS] |= 2;	
	}
	else
	{
		// Showing scoreboard, or help layouts?

		if(ent->client->showscores || ent->client->showhelp)
			ent->client->ps.stats[STAT_LAYOUTS] |= 1;
	}

	//
	// frags
	//

	ent->client->ps.stats[STAT_FRAGS] = ent->client->resp.score;

//	ent->client->ps.stats[STAT_DAMAGELOC] = (short) (ent->client->ps.damageLoc);

	ent->client->ps.damageLoc = 0;

	ent->client->ps.stats[STAT_DAMAGEDIR] = (short) (ent->client->ps.damageDir);
	ent->client->ps.damageDir = 0;

	if (ent->client->ps.stats[STAT_FLAGS])
	{
		VectorCopy(ent->s.origin,holdorigin);
		holdorigin[2] += 10000;
		gi.trace(ent->s.origin, ent->mins, ent->maxs, holdorigin, ent, MASK_MONSTERSOLID, &tr);

		if(tr.surface->flags & SURF_SKY)
		{
			ent->client->ps.stats[STAT_FLAGS] |= STAT_FLAG_WIND;
		}
	}

	// "mission failed"
	if ((level.maxDeadHostages != -1) &&
		(level.deadHostages >= level.maxDeadHostages))
	{
		ent->client->ps.stats[STAT_FLAGS] |= STAT_FLAG_MISSIONFAIL;
	}

	// our countdown timer has reached zero. bad things ensue.
	if (level.countdownEnded)
	{
		ent->client->ps.stats[STAT_FLAGS] |= STAT_FLAG_COUNTDOWN;
	}

	// "mission accomplished","nearing end of mission",etc.
	switch (level.missionStatus)
	{
	case MISSION_ACCOMPLISHED:
		ent->client->ps.stats[STAT_FLAGS] |= STAT_FLAG_MISSIONACC;
		break;
	case MISSION_EXIT:
		ent->client->ps.stats[STAT_FLAGS] |= STAT_FLAG_MISSIONEXIT;
		break;
	case MISSION_OBJECTIVES:
		if ( (level.time - s_levelStatusBegin) < fMaxObjectivesDisplayTime)
		{
			ent->client->ps.stats[STAT_FLAGS] |= STAT_FLAG_OBJECTIVES;
		}
		break;
	}

	if(stealth->value)
		dm->clientStealthAndFatigueMeter(ent);

	if (level.forceHUD)
	{
		ent->client->ps.stats[STAT_FORCEHUD] = 1;
	}

	if(ent->client->blinding_alpha>0.5)
		ent->client->ps.stats[STAT_FLAGS] |= STAT_FLAG_HIDECROSSHAIR;
}
