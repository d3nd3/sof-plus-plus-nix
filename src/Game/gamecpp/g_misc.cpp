// g_misc.c
#include "g_local.h"
#include "fields.h"
#include "..\qcommon\configstring.h"
#include "..\qcommon\ef_flags.h"

#define		INVINCIBLE		1
#define		GLASS			2

#define		AREA_PORTAL_START_OPEN		1

#define		GLASS_PAINCHUNKS			2
#define		BREAKABLE_NOT_IN_CAM		8

void FindCorrectOrigin(edict_t *ent, vec3_t origin)
{
	// to accomodate BMODELS, which are dumb...
	if ((ent->solid == SOLID_BSP) && (VectorCompare(ent->s.origin, vec3_origin)))
	{
		VectorScale(ent->absmax, .5, origin);
		VectorMA(origin, .5, ent->absmin, origin);
	}
	else
	{
		VectorAdd(ent->mins, ent->maxs, origin);
		VectorMA(ent->s.origin, .5, origin, origin);
	}
	return;
}

/*QUAKED func_group (0 0 0) ?
Used to group brushes together just for editor convenience.
*/

//=====================================================

void Use_Areaportal (edict_t *ent, edict_t *other, edict_t *activator)
{
	ent->count ^= 1;		// toggle state
//	gi.dprintf ("portalstate: %i = %i\n", ent->style, ent->count);
	gi.SetAreaPortalState (ent->style, ent->count);
}

/*QUAKED func_areaportal (0 0 0) ?

This is a non-visible object that divides the world into
areas that are seperated when this portal is not activated.
Usually enclosed in the middle of a door.
*/
void SP_func_areaportal (edict_t *ent)
{
//	ent->use = Use_Areaportal;  according to our crack design staff,
//  areaportals are ALWAYS triggered by doors.  This stuff is already handled in door_use_areaportals,
//	so we don' need it n'more
	ent->count = 0;		// always start closed;

}

//=====================================================


/*
=================
Misc functions
=================
*/
void VelocityForDamage (int damage, vec3_t v)
{
	v[0] = gi.flrand(-100.0F, 100.0F);
	v[1] = gi.flrand(-100.0F, 100.0F);
	v[2] = gi.flrand(100.0F, 300.0F);

	if (damage < 50)
		VectorScale (v, 0.7, v);
	else 
		VectorScale (v, 1.2, v);
}

void ClipGibVelocity (edict_t *ent)
{
	if (ent->velocity[0] < -300)
		ent->velocity[0] = -300;
	else if (ent->velocity[0] > 300)
		ent->velocity[0] = 300;
	if (ent->velocity[1] < -300)
		ent->velocity[1] = -300;
	else if (ent->velocity[1] > 300)
		ent->velocity[1] = 300;
	if (ent->velocity[2] < 200)
		ent->velocity[2] = 200;	// always some upwards
	else if (ent->velocity[2] > 500)
		ent->velocity[2] = 500;
}


/*
=================
gibs
=================
*/
void gib_think (edict_t *self)
{
	self->s.frame++;
	self->nextthink = level.time + FRAMETIME;

	if (self->s.frame == 10)
	{
		self->think = G_FreeEdict;
		self->nextthink = level.time + gi.flrand(8.0F, 18.0F);
	}
}

void gib_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t	normal_angles, right;

	if (!self->groundentity)
		return;

	self->touch = NULL;

	if (plane)
	{
		gi.sound (self, CHAN_VOICE, gi.soundindex ("misc/fhit3.wav"), .6, ATTN_NORM, 0);

		vectoangles (plane->normal, normal_angles);
		AngleVectors (normal_angles, NULL, right, NULL);
		vectoangles (right, self->s.angles);

		if (self->s.modelindex == sm_meat_index)
		{
			self->s.frame++;
			self->think = gib_think;
			self->nextthink = level.time + FRAMETIME;
		}
	}
}

void gib_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	G_FreeEdict (self);
}

void ThrowGib (edict_t *self, char *gibname, int damage, int type)
{
	edict_t *gib;
	vec3_t	vd;
	vec3_t	origin;
	vec3_t	size;
	float	vscale;

	gib = G_Spawn();

	VectorScale (self->size, 0.5, size);
	VectorAdd (self->absmin, size, origin);
	gib->s.origin[0] = origin[0] + gi.flrand(-size[0], size[0]);
	gib->s.origin[1] = origin[1] + gi.flrand(-size[1], size[1]);
	gib->s.origin[2] = origin[2] + gi.flrand(-size[2], size[2]);

	gi.setmodel (gib, gibname);
	gib->solid = SOLID_NOT;
//	gib->s.effects |= EF_GIB;
	gib->flags |= FL_NO_KNOCKBACK;
	gib->takedamage = DAMAGE_YES;
	gib->die = gib_die;

	if (type == GIB_ORGANIC)
	{
		gib->movetype = MOVETYPE_TOSS;
		gib->touch = gib_touch;
		vscale = 0.5;
	}
	else
	{
		gib->movetype = MOVETYPE_BOUNCE;
		vscale = 1.0;
	}

	VelocityForDamage (damage, vd);
	VectorMA (self->velocity, vscale, vd, gib->velocity);
	ClipGibVelocity (gib);
	gib->avelocity[0] = gi.flrand(0.0F, 600.0F);
	gib->avelocity[1] = gi.flrand(0.0F, 600.0F);
	gib->avelocity[2] = gi.flrand(0.0F, 600.0F);

	gib->think = G_FreeEdict;
	gib->nextthink = level.time + gi.flrand(10.0F, 20.0F);

	gi.linkentity (gib);
}

void ThrowHead (edict_t *self, char *gibname, int damage, int type)
{
	vec3_t	vd;
	float	vscale;

	self->s.skinnum = 0;
	self->s.frame = 0;
	VectorClear (self->mins);
	VectorClear (self->maxs);

	gi.setmodel (self, gibname);
	self->solid = SOLID_NOT;
	self->s.sound = 0;
	self->flags |= FL_NO_KNOCKBACK;
	self->svflags &= ~SVF_MONSTER;
	self->takedamage = DAMAGE_YES;
	self->die = gib_die;

	if (type == GIB_ORGANIC)
	{
		self->movetype = MOVETYPE_TOSS;
		self->touch = gib_touch;
		vscale = 0.5;
	}
	else
	{
		self->movetype = MOVETYPE_BOUNCE;
		vscale = 1.0;
	}

	VelocityForDamage (damage, vd);
	VectorMA (self->velocity, vscale, vd, self->velocity);
	ClipGibVelocity (self);

	self->avelocity[YAW] = gi.flrand(-600.0F, 600.0F);

	self->think = G_FreeEdict;
	self->nextthink = level.time + gi.flrand(10.0F, 20.0F);

	gi.linkentity (self);
}

static int shakeIntensity[] = {7, 17, 35, 55, 80};

void ShakeCameras (vec3_t origin, int amount, int radius, int delta)
{
	edict_t *curSearch = NULL;
	float shakeAmount;
	vec3_t dist;
	float	distf;
	int i, j = 0;

	CRadiusContent rad(origin, radius);

	for(i = 0; i < rad.getNumFound(); i++)
	{
		curSearch = rad.foundEdict(i);

		if(curSearch->client != NULL)
		{
			VectorSubtract(curSearch->s.origin, origin, dist);
			distf = VectorLength(dist);
			shakeAmount =  (1.0 - (distf/radius)) * amount;

			j = 4;
			while (j > 0)
			{
				if(shakeAmount > shakeIntensity[j])break;

				j--;
			}

			FX_SetEvent_Data(curSearch, EV_CAMERA_SHAKE_VERYLIGHT + j, delta);
		}
	}
}

void BlindingLight(vec3_t origin, int intensity, float maxalpha, float delta)
{
	edict_t *curBlinded = NULL;
	trace_t	tr;
	vec3_t	endPoint, tempVect;
	float	dist;
	vec3_t	fwd;
	float	val;

	while ((curBlinded = G_Find (curBlinded, FOFS(classname), "player")) != NULL)
	{
		VectorSubtract(origin, curBlinded->s.origin, tempVect);
		dist = VectorLength(tempVect);
		if(dist > intensity)continue;

		VectorCopy(curBlinded->s.origin, endPoint);

		//ss--i think this is valid viewheight
		endPoint[2] += curBlinded->viewheight;// 26; // is there an actual value describing the player's view height?

		gi.trace (origin, NULL, NULL, endPoint, curBlinded, MASK_SHOT, &tr);

		if(tr.fraction <= .99)continue;

		AngleVectors(curBlinded->client->ps.viewangles, fwd, NULL, NULL);
		VectorNormalize(tempVect);
		val = DotProduct(fwd, tempVect);
		if(val > 0)
		{
			if (curBlinded->client->goggles_on)
			{	// Using the light intensifier goggles.	 All flashes are more intense.
				float	blindalpha;

				blindalpha = intensity*.03/dist*val;
				if (blindalpha < 0.4)
				{
					blindalpha += 0.4;
				}
				else
				{
					blindalpha *= 2.0;
				}
				curBlinded->client->blinding_alpha += blindalpha;
			}
			else
			{	// Normal blinding level.
				curBlinded->client->blinding_alpha += intensity*.03/dist*val;
			}
		}

		if (curBlinded->client->goggles_on)
		{	// Flashes should last longer with goggles.
			delta *= 0.5;
			maxalpha += (1.0-maxalpha)*0.5;
		}

		// Cap the alpha
		if(curBlinded->client->blinding_alpha > maxalpha) 
		{
			curBlinded->client->blinding_alpha = maxalpha;
		}

		// add in the delta at which we think the screen flash should go down.
		// use the slowed blind decay time you can...
		if (curBlinded->client->blinding_alpha_delta <= 0 || delta < curBlinded->client->blinding_alpha_delta)
		{
			curBlinded->client->blinding_alpha_delta = delta;
		}
	}

	curBlinded = NULL;

	CRadiusContent rad(origin, intensity);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		curBlinded = rad.foundEdict(i);

		if (!curBlinded->ai)continue;

		VectorCopy(curBlinded->s.origin, endPoint);
		endPoint[2] += curBlinded->viewheight;
		gi.trace (origin, NULL, NULL, endPoint, curBlinded, MASK_SHOT, &tr);

		if(tr.fraction <= .99)continue;

		curBlinded->ai->GetLookVector(fwd);

		VectorSubtract(origin, curBlinded->s.origin, tempVect);
		dist = VectorNormalize(tempVect);

		val = DotProduct(fwd, tempVect);

//		if(val > -0.75)
		{
			float monster_blind = ((float)intensity*0.5)/dist*(val+1.25);
			if (monster_blind > 25)
			{
				monster_blind = 25;
			}
			curBlinded->ai->MuteSenses(sight_mask, monster_blind, smute_recov_linear, monster_blind);
		}
	}
}

void TearGasEnt(edict_t *ent, float intensity)
{
	if(!strcmp(ent->classname, "player"))
	{
		ent->client->gas_blend += intensity;
		if(ent->client->gas_blend > 1.0)ent->client->gas_blend = 1.0;
	}
	else if(ent->ai)
	{
		ent->ai->MuteSenses(sight_mask, intensity*50, smute_recov_linear, intensity*50);
	}
}

/*
=================
debris
=================
*/
void debris_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	G_FreeEdict (self);
}

void ThrowDebris (edict_t *self, char *modelname, float speed, vec3_t origin)
{
	edict_t	*chunk;
	vec3_t	v;

	chunk = G_Spawn();
	VectorCopy (origin, chunk->s.origin);
	gi.setmodel (chunk, modelname);
	v[0] = gi.flrand(-100.0F, 100.0F);
	v[1] = gi.flrand(-100.0F, 100.0F);
	v[2] = gi.flrand(0.0F, 200.0F);
	VectorMA (self->velocity, speed, v, chunk->velocity);
	chunk->movetype = MOVETYPE_BOUNCE;
	chunk->solid = SOLID_NOT;
	chunk->avelocity[0] = gi.flrand(0.0F, 600.0F);
	chunk->avelocity[1] = gi.flrand(0.0F, 600.0F);
	chunk->avelocity[2] = gi.flrand(0.0F, 600.0F);
	chunk->think = G_FreeEdict;
	chunk->nextthink = level.time + gi.flrand(5.0F, 10.0F);
	chunk->s.frame = 0;
	chunk->flags = 0;
	chunk->classname = "debris";
	chunk->takedamage = DAMAGE_YES;
	chunk->die = debris_die;
	gi.linkentity (chunk);
}


void BecomeExplosion1 (edict_t *self)
{
/*	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_EXPLOSION1);
	gi.WritePosition (self->s.origin);
	gi.multicast (self->s.origin, MULTICAST_PVS);*/

	G_FreeEdict (self);
}


void BecomeExplosion2 (edict_t *self)
{
/*	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_EXPLOSION2);
	gi.WritePosition (self->s.origin);
	gi.multicast (self->s.origin, MULTICAST_PVS);*/

	G_FreeEdict (self);
}


/*QUAKED path_corner (.5 .3 0) (-8 -8 -8) (8 8 8) TELEPORT WAITONECYCLE FACEPLAYER
WAITONECYCLE: ignores wait time, goes through waitactions once
FACEPLAYER: will face the player forever, once it reaches this point
Target: next path corner
Pathtarget: gets used when an entity that has
	this path_corner targeted touches it
MoveAction:  animation to play while moving to the next path_corner
Waitaction1: animations to play while waiting
Waitaction2:
Waitaction3:
Waitaction4:
Waitaction5:
Waitaction6:
Wait: time to wait (-1 for forever)
*/

#define MAX_PATHCORNER_WAITACTIONS	6

//for MMoves
#include "ai_private.h"

void SetWaitAction(edict_t *self, int actionNum, char *actionName)
{
	mmove_t **putMoveHere;
	int		i;

	if (actionNum > MAX_PATHCORNER_WAITACTIONS)
	{
		return;
	}

	if (!actionName || !(*actionName))
	{
		return;
	}

	//ahem. NO! fixme, this is a quick thing only
	switch(actionNum)
	{
	case 1:
		putMoveHere = &self->wait_action1;
		break;
	case 2:
		putMoveHere = &self->wait_action2;
		break;
	case 3:
		putMoveHere = &self->wait_action3;
		break;
	case 4:
		putMoveHere = &self->wait_action4;
		break;
	case 5:
		putMoveHere = &self->wait_action5;
		break;
	default:
#if _DEBUG
		gi.dprintf("warning: trying to set invalid path_corner waitaction: %d\n", actionNum);
#endif
	case 6:
		putMoveHere = &self->wait_action6;
		break;
	}

	for (i=0; MMoves[i].ghoulSeqName[0]; i++)//this is yucky too--will prolly need to go through several lists
	{
		//found the move.
		if (!stricmp(MMoves[i].ghoulSeqName, actionName))
		{
			*putMoveHere = &MMoves[i];
			return;
		}
	}
	gi.dprintf("couldn't find path_corner waitaction: %s (using stand)\n", actionName);
	*putMoveHere=&generic_move_stand;
}

void SetMoveAction(edict_t *self, char *actionName)
{
	mmove_t **putMoveHere;
	int		i;

	if (!actionName || !(*actionName))
	{
		return;
	}

	putMoveHere = &self->move_action;

	for (i=0; MMoves[i].ghoulSeqName[0]; i++)//this is yucky too--will prolly need to go through several lists
	{
		//found the move.
		if (!stricmp(MMoves[i].ghoulSeqName, actionName))
		{
			*putMoveHere = &MMoves[i];
			return;
		}
	}
	gi.dprintf("couldn't find path_corner moveaction: %s (using walk)\n", actionName);
	*putMoveHere=&generic_move_walk;
}

mmove_t *GetMoveAction(edict_t *self)
{
	if (!self)
	{
		return NULL;
	}
	return self->move_action;
}

mmove_t *GetWaitAction(edict_t *self, int actionNum)
{
	if ((actionNum > MAX_PATHCORNER_WAITACTIONS) || !self)
	{
		return NULL;
	}

	//ahem. NO! fixme, this is a quick thing only
	switch(actionNum)
	{
	case 1:
		return self->wait_action1;
		break;
	case 2:
		return self->wait_action2;
		break;
	case 3:
		return self->wait_action3;
		break;
	case 4:
		return self->wait_action4;
		break;
	case 5:
		return self->wait_action5;
		break;
	case 6:
		return self->wait_action6;
		break;
	}
#if _DEBUG
		gi.dprintf("warning: trying to get invalid path_corner waitaction: %d\n", actionNum);
#endif
	return NULL;
}

void path_corner_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t		v;
	edict_t		*next;

	if (other->movetarget != self)
		return;
	
	if (other->enemy)
		return;

	if (self->pathtarget)
	{
		char *savetarget;

		savetarget = self->target;
		self->target = self->pathtarget;
		G_UseTargets (self, other);
		self->target = savetarget;
	}

	if (self->target)
		next = G_PickTarget(self->target);
	else
		next = NULL;

	if ((next) && (next->spawnflags & 1))
	{
		VectorCopy (next->s.origin, v);
		v[2] += next->mins[2];
		v[2] -= other->mins[2];
		VectorCopy (v, other->s.origin);
		next = G_PickTarget(next->target);
	}

	other->goalentity = other->movetarget = next;

	if (self->wait)
	{
//		other->monsterinfo.pausetime = level.time + self->wait;
//		other->monsterinfo.stand (other);
		return;
	}

	if (!other->movetarget)
	{
//		other->monsterinfo.pausetime = level.time + 100000000;
//		other->monsterinfo.stand (other);
	}
	else
	{
		VectorSubtract (other->goalentity->s.origin, other->s.origin, v);
	}
	
}

void pathtest_think(edict_t *who)
{
	if (ai_pathtest->value)
	{
//		FX_MakeSparks(who->s.origin, vec3_up, 2);
	}
	who->nextthink=level.time+0.5;
}

void SP_path_corner (edict_t *self)
{
//	trace_t		tr;
//	vec3_t		dest;

	if (!self->targetname)
	{
		gi.dprintf ("path_corner with no targetname at %s\n", vtos(self->s.origin));
		G_FreeEdict (self);
		return;
	}

	SetWaitAction(self, 1, st.waitAction1);
	SetWaitAction(self, 2, st.waitAction2);
	SetWaitAction(self, 3, st.waitAction3);
	SetWaitAction(self, 4, st.waitAction4);
	SetWaitAction(self, 5, st.waitAction5);
	SetWaitAction(self, 6, st.waitAction6);

	SetMoveAction(self, st.moveAction);

/*	VectorCopy(self->s.origin, dest);
	dest[2]-=1000;
	gi.trace (self->s.origin, NULL, NULL, dest, self, MASK_SOLID, &tr);
	if (!tr.startsolid)
	{
		VectorCopy (tr.endpos, self->s.origin);
		self->s.origin[2]+=10;
	}
*/
	self->solid = SOLID_TRIGGER;
	self->touch = path_corner_touch;
	VectorSet (self->mins, -8, -8, -8);
	VectorSet (self->maxs, 8, 8, 8);

	//oop, this bit was not welcome. i'll just do the pathfinding from a bit higher up.
//	self->s.origin[2]+=8;

	self->svflags |= SVF_NOCLIENT;
//	self->s.effects |= EF_TELEPORTER;
//	self->s.modelindex = gi.modelindex ("sprites/s_bfg1.sp2"); 
	self->friction = 0;
	self->gravity = 0;
	self->airresistance = 0;

	self->think=pathtest_think;
	self->nextthink=level.time+0.5;

	gi.linkentity (self);
}


/*QUAKED point_combat (0.5 0.3 0) (-8 -8 -8) (8 8 8) Hold WAITONECYCLE FACEPLAYER
Makes this the target of a monster and it will head here
when first activated before going after the activator.  If
hold is selected, it will stay here.

WAITONECYCLE: ignores wait time, goes through waitactions once
FACEPLAYER: will face the player forever, once it reaches this point
Target: next point_combat
Pathtarget: gets used when an entity that has
	this path_corner targeted touches it
MoveAction:  animation to play while moving to the next point_combat
Waitaction1: animations to play while waiting
Waitaction2:
Waitaction3:
Waitaction4:
Waitaction5:
Waitaction6:
Wait: time to wait (-1 for forever)
*/
void point_combat_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	edict_t	*activator;

	if (other->movetarget != self)
		return;

	if (self->target)
	{
		other->target = self->target;
		other->goalentity = other->movetarget = G_PickTarget(other->target);
		if (!other->goalentity)
		{
			gi.dprintf("%s at %s target %s does not exist\n", self->classname, vtos(self->s.origin), self->target);
			other->movetarget = self;
		}
		self->target = NULL;
	}
	else if ((self->spawnflags & 1) && !(other->flags & (FL_SWIM|FL_FLY)))
	{
//		other->monsterinfo.pausetime = level.time + 100000000;
//		other->monsterinfo.aiflags |= AI_STAND_GROUND;
//		other->monsterinfo.stand (other);
	}

	if (other->movetarget == self)
	{
		other->target = NULL;
		other->movetarget = NULL;
		other->goalentity = other->enemy;
//		other->monsterinfo.aiflags &= ~AI_COMBAT_POINT;
	}

	if (self->pathtarget)
	{
		char *savetarget;

		savetarget = self->target;
		self->target = self->pathtarget;
		if (other->enemy && other->enemy->client)
			activator = other->enemy;
		else if (other->oldenemy && other->oldenemy->client)
			activator = other->oldenemy;
		else if (other->activator && other->activator->client)
			activator = other->activator;
		else
			activator = other;
		G_UseTargets (self, activator);
		self->target = savetarget;
	}
}

void SP_point_combat (edict_t *self)
{
	if (dm->isDM())
	{
		G_FreeEdict (self);
		return;
	}

	SetWaitAction(self, 1, st.waitAction1);
	SetWaitAction(self, 2, st.waitAction2);
	SetWaitAction(self, 3, st.waitAction3);
	SetWaitAction(self, 4, st.waitAction4);
	SetWaitAction(self, 5, st.waitAction5);
	SetWaitAction(self, 6, st.waitAction6);

	SetMoveAction(self, st.moveAction);

	self->solid = SOLID_TRIGGER;
	self->touch = point_combat_touch;
	VectorSet (self->mins, -8, -8, -16);
	VectorSet (self->maxs, 8, 8, 16);
	self->svflags = SVF_NOCLIENT;
	gi.linkentity (self);
};


/*QUAKED info_null (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for spotlights, etc.
*/
void SP_info_null (edict_t *self)
{
	G_FreeEdict (self);
};


/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for lightning.
*/
void SP_info_notnull (edict_t *self)
{
	VectorCopy (self->s.origin, self->absmin);
	VectorCopy (self->s.origin, self->absmax);
};


/*QUAKED light (0 1 0) (-8 -8 -8) (8 8 8) START_OFF
Non-displayed light.
Default light value is 300.
Style - the light 
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
  	
If targeted, will toggle between on and off.
Default _cone value is 10 (used to set size of light for spotlights)
*/

void light_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->spawnflags & SF_LIGHT_START_OFF)
	{
		gi.configstring (CS_LIGHTS+self->style, "m");
		self->spawnflags &= ~SF_LIGHT_START_OFF;
	}
	else
	{
		gi.configstring (CS_LIGHTS+self->style, "a");
		self->spawnflags |= SF_LIGHT_START_OFF;
	}
}

void LightInit (edict_t *self)
{
	if (self->style >= 32)
	{
		self->use = light_use;
		if (self->spawnflags & SF_LIGHT_START_OFF)
			gi.configstring (CS_LIGHTS+self->style, "a");
		else
			gi.configstring (CS_LIGHTS+self->style, "m");
	}
}


void SP_light (edict_t *self)
{
	if (!self->targetname || dm->isDM())
	{
		G_FreeEdict (self);
		return;
	}

	if (self->style >= 32)
	{
		self->use = light_use;
		if (self->spawnflags & SF_LIGHT_START_OFF)
			gi.configstring (CS_LIGHTS+self->style, "a");
		else
			gi.configstring (CS_LIGHTS+self->style, "m");
	}
}


/*QUAKED func_wall (0 .5 .8) ? TRIGGER_SPAWN TOGGLE START_ON ANIMATED ANIMATED_FAST
This is just a solid wall if not inhibited

TRIGGER_SPAWN	the wall will not be present until triggered
				it will then blink in to existance; it will
				kill anything that was in it's way

TOGGLE			only valid for TRIGGER_SPAWN walls
				this allows the wall to be turned on and off

START_ON		only valid for TRIGGER_SPAWN walls
				the wall will initially be present
*/

void func_wall_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->solid == SOLID_NOT)
	{
		self->solid = SOLID_BSP;
		self->svflags &= ~SVF_NOCLIENT;
		KillBox (self);
	}
	else
	{
		self->solid = SOLID_NOT;
		self->svflags |= SVF_NOCLIENT;
	}
	gi.linkentity (self);

	if (!(self->spawnflags & 2))
		self->use = NULL;
}

void SP_func_wall (edict_t *self)
{
	self->movetype = MOVETYPE_PUSH;
	gi.setmodel (self, self->model);

	if (self->spawnflags & 8)
		self->s.effects |= EF_ANIM_ALL;
	if (self->spawnflags & 16)
		self->s.effects |= EF_ANIM_ALLFAST;

	// just a wall
	if ((self->spawnflags & 7) == 0)
	{
		self->solid = SOLID_BSP;
		gi.linkentity (self);
		return;
	}

	// it must be TRIGGER_SPAWN
	if (!(self->spawnflags & 1))
	{
//		gi.dprintf("func_wall missing TRIGGER_SPAWN\n");
		self->spawnflags |= 1;
	}

	// yell if the spawnflags are odd
	if (self->spawnflags & 4)
	{
		if (!(self->spawnflags & 2))
		{
//			gi.dprintf("func_wall START_ON without TOGGLE\n");
			self->spawnflags |= 2;
		}
	}

	self->use = func_wall_use;
	if (self->spawnflags & 4)
	{
		self->solid = SOLID_BSP;
	}
	else
	{
		self->solid = SOLID_NOT;
		self->svflags |= SVF_NOCLIENT;
	}
	gi.linkentity (self);
}


/*QUAKED func_object (0 .5 .8) ? TRIGGER_SPAWN ANIMATED ANIMATED_FAST
This is solid bmodel that will fall if it's support it removed.
*/

void func_object_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	// only squash thing we fall on top of
	if (!plane)
		return;
	if (plane->normal[2] < 1.0)
		return;
	if (other->takedamage == DAMAGE_NO)
		return;
	T_Damage (other, self, self, vec3_origin, self->s.origin, self->s.origin, self->dmg, 1, 0, MOD_CRUSH);
}

void func_object_release (edict_t *self)
{
	self->movetype = MOVETYPE_TOSS;
	self->touch = func_object_touch;
}

void func_object_use (edict_t *self, edict_t *other, edict_t *activator)
{
	self->solid = SOLID_BSP;
	self->svflags &= ~SVF_NOCLIENT;
	self->use = NULL;
	KillBox (self);
	func_object_release (self);
}

void SP_func_object (edict_t *self)
{
	gi.setmodel (self, self->model);

	self->mins[0] += 1;
	self->mins[1] += 1;
	self->mins[2] += 1;
	self->maxs[0] -= 1;
	self->maxs[1] -= 1;
	self->maxs[2] -= 1;

	if (!self->dmg)
		self->dmg = 100;

	if (self->spawnflags == 0)
	{
		self->solid = SOLID_BSP;
		self->movetype = MOVETYPE_PUSH;
		self->think = func_object_release;
		self->nextthink = level.time + 2 * FRAMETIME;
	}
	else
	{
		self->solid = SOLID_NOT;
		self->movetype = MOVETYPE_PUSH;
		self->use = func_object_use;
		self->svflags |= SVF_NOCLIENT;
	}

	if (self->spawnflags & 2)
		self->s.effects |= EF_ANIM_ALL;
	if (self->spawnflags & 4)
		self->s.effects |= EF_ANIM_ALLFAST;

	self->clipmask = MASK_MONSTERSOLID;

	gi.linkentity (self);
}

//=====================================================

/*QUAKED target_character (0 0 1) ?
used with target_string (must be on same "team")
"count" is position in the string (starts at 1)
*/

void SP_target_character (edict_t *self)
{
	self->movetype = MOVETYPE_PUSH;
	gi.setmodel (self, self->model);
	self->solid = SOLID_BSP;
	self->s.frame = 12;
	gi.linkentity (self);
	return;
}


/*QUAKED target_string (0 0 1) (-8 -8 -8) (8 8 8)
*/

void target_string_use (edict_t *self, edict_t *other, edict_t *activator)
{
	edict_t *e;
	int		n, l;
	char	c;

	l = strlen(self->message);
	for (e = self->teammaster; e; e = e->teamchain)
	{
		if (!e->count)
			continue;
		n = e->count - 1;
		if (n > l)
		{
			e->s.frame = 12;
			continue;
		}

		c = self->message[n];
		if (c >= '0' && c <= '9')
			e->s.frame = c - '0';
		else if (c == '-')
			e->s.frame = 10;
		else if (c == ':')
			e->s.frame = 11;
		else
			e->s.frame = 12;
	}
}

void SP_target_string (edict_t *self)
{
	if (!self->message)
		self->message = "";
	self->use = target_string_use;
}


/*QUAKED func_clock (0 0 1) (-8 -8 -8) (8 8 8) TIMER_UP TIMER_DOWN START_OFF MULTI_USE
target a target_string with this

The default is to be a time of day clock

TIMER_UP and TIMER_DOWN run for "count" seconds and the fire "pathtarget"
If START_OFF, this entity must be used before it starts

"style"		0 "xx"
			1 "xx:xx"
			2 "xx:xx:xx"
*/

#define	CLOCK_MESSAGE_SIZE	16

// don't let field width of any clock messages change, or it
// could cause an overwrite after a game load

static void func_clock_reset (edict_t *self)
{
	self->activator = NULL;
	if (self->spawnflags & 1)
	{
		self->health = 0;
		self->wait = self->count;
	}
	else if (self->spawnflags & 2)
	{
		self->health = self->count;
		self->wait = 0;
	}
}

static void func_clock_format_countdown (edict_t *self)
{
	if (self->style == 0)
	{
		Com_sprintf (self->message, CLOCK_MESSAGE_SIZE, "%2i", self->health);
		return;
	}

	if (self->style == 1)
	{
		Com_sprintf(self->message, CLOCK_MESSAGE_SIZE, "%2i:%02i", self->health / 60, self->health % 60);
		return;
	}

	if (self->style == 2)
	{
		Com_sprintf(self->message, CLOCK_MESSAGE_SIZE, "%2i:%02i:%02i", self->health / 3600, (self->health - (self->health / 3600) * 3600) / 60, self->health % 60);
		return;
	}
}

void func_clock_think (edict_t *self)
{
	if (!self->enemy)
	{
		self->enemy = G_Find (NULL, FOFS(targetname), self->target);
		if (!self->enemy)
			return;
	}

	if (self->spawnflags & 1)
	{
		func_clock_format_countdown (self);
		self->health++;
	}
	else if (self->spawnflags & 2)
	{
		func_clock_format_countdown (self);
		self->health--;
	}
	else
	{
		struct tm	*ltime;
		time_t		gmtime;

		time(&gmtime);
		ltime = localtime(&gmtime);
		Com_sprintf (self->message, CLOCK_MESSAGE_SIZE, "%2i:%02i:%02i", ltime->tm_hour, ltime->tm_min, ltime->tm_sec);
	}

	self->enemy->message = self->message;
	self->enemy->use (self->enemy, self, self);

	if (((self->spawnflags & 1) && (self->health > self->wait)) ||
		((self->spawnflags & 2) && (self->health < self->wait)))
	{
		if (self->pathtarget)
		{
			char *savetarget;
			char *savemessage;

			savetarget = self->target;
			savemessage = self->message;
			self->target = self->pathtarget;
			self->message = NULL;
			G_UseTargets (self, self->activator);
			self->target = savetarget;
			self->message = savemessage;
		}

		if (!(self->spawnflags & 8))
			return;

		func_clock_reset (self);

		if (self->spawnflags & 4)
			return;
	}

	self->nextthink = level.time + 1;
}

void func_clock_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (!(self->spawnflags & 8))
		self->use = NULL;
	if (self->activator)
		return;
	self->activator = activator;
	self->think (self);
}

void SP_func_clock (edict_t *self)
{
	if (!self->target)
	{
		gi.dprintf("%s with no target at %s\n", self->classname, vtos(self->s.origin));
		G_FreeEdict (self);
		return;
	}

	if ((self->spawnflags & 2) && (!self->count))
	{
		gi.dprintf("%s with no count at %s\n", self->classname, vtos(self->s.origin));
		G_FreeEdict (self);
		return;
	}

	if ((self->spawnflags & 1) && (!self->count))
		self->count = 60*60;;

	func_clock_reset (self);

	self->message = (char*)gi.TagMalloc (CLOCK_MESSAGE_SIZE, TAG_LEVEL);

	self->think = func_clock_think;

	if (self->spawnflags & 4)
		self->use = func_clock_use;
	else
		self->nextthink = level.time + 1;
}


//=================================================================================

void FinishRespawn(edict_t *ent)
{
	edict_t *newEnt;

	// fixme:  check to see that the respawn location is unoccupied, esp. for deathmatch!
/*	 if (my space is occupied)
	 {
	
	  nextthink = level.time +.1;
		return;
	   }
*/	
	newEnt = G_Spawn();
	VectorCopy(ent->s.origin, newEnt->s.origin);
	VectorCopy(ent->s.angles, newEnt->s.angles);
	newEnt->health = ent->health;
	newEnt->spawnflags = ent->spawnflags;
	newEnt->wait = ent->wait;
	newEnt->accel = ent->accel;
	newEnt->style = ent->style;
	newEnt->targetname = ent->targetname;
	newEnt->target = ent->target;
	ent->respawnFunc(newEnt);

	G_FreeEdict(ent);
}

void InitiateRespawn(edict_t *ent)
{
	edict_t *newEnt;

	newEnt = G_Spawn();
	VectorCopy(ent->spawnOrigin, newEnt->s.origin);
	VectorCopy(ent->spawnAngles, newEnt->s.angles);
	newEnt->respawnFunc = ent->respawnFunc;
	newEnt->health = ent->spawnHealth;
	newEnt->movetype = MOVETYPE_NONE;
	newEnt->solid = SOLID_NOT;
	newEnt->spawnflags = ent->spawnflags;
	newEnt->wait = ent->wait;
	newEnt->accel = ent->accel;
	newEnt->style = ent->style;
	newEnt->targetname = ent->targetname;
	newEnt->target = ent->target;
	newEnt->think = FinishRespawn;

	newEnt->nextthink = level.time + ent->respawnTime;
}

void SetForRespawn(edict_t *ent, void (*spawnFunc)(edict_t *), float respawnDuration)
{
	VectorCopy(ent->s.origin, ent->spawnOrigin);
	VectorCopy(ent->s.angles, ent->spawnAngles);
	ent->spawnHealth = ent->health;
	ent->respawnFunc = spawnFunc;
	ent->respawnTime = respawnDuration;
}


/*QUAKED test_model (1 .5 0) (-16 -16 -16) (16 16 16)
set model to the name of the model to show
*/
void SP_test_model (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->s.modelindex = gi.modelindex (ent->model);
	VectorSet (ent->mins, -16, -16, -16);
	VectorSet (ent->maxs, 16, 16, 16);
	gi.linkentity (ent);
}


/*QUAKED func_breakable_brush (0 .5 .8) ? INVINCIBLE  PAINCHUNKS DEKKER CANT_DMG_IN_CAMERA_MODE
A breakable brush which has chunk size and type based upon the size and type of the brush.
--------SPAWNFLAGS----------
INVINCIBLE - intended to be used in conjunction with triggers
PAINCHUNKS - spawn debris when hurt (used for windows that shatter when first shot)
DEKKER - only damageable by Dekker, will do damage to folks in the area when breaking
CANT_DMG_IN_CAMERA_MODE -- you can damage this brush normally but it's invincible when player is viewing thru a camera
-------KEYS-----------------
count - # of debris chunks to throw

mass - size of chunks to throw
1 - small chunks
2 - medium chunks
3 - big chunks

health - defaults to 100.  The "dmg" key/value pair will set how much damage will be done on
its destruction (defaults to 0).  The "volume" key/value pair will determine the radius of the
damage.  Damage falls off linearly to zero at this radius.  Volume will default to whatever
"dmg" is set to. 

message - text to print when it dies

material - defines the type of debris to create when the brush breaks
 0 - MAT_NONE, (defaults to stone)
 1 - MAT_BRICK_BROWN,
 2 - MAT_BRICK_DBROWN,
 3 - MAT_BRICK_LBROWN,
 4 - MAT_BRICK_LGREY,
 5 - MAT_BRICK_DGREY,
 6 - MAT_BRICK_RED,

 7 - MAT_GLASS,

 8 - MAT_METAL_LGREY,
 9 - MAT_METAL_DGREY,
10 - MAT_METAL_RUSTY,
11 - MAT_METAL_SHINY,

12 - MAT_ROCK_BROWN,
13 - MAT_ROCK_LBROWN,
14 - MAT_ROCK_DBROWN,
15 - MAT_ROCK_LGREY,
16 - MAT_ROCK_DGREY,

17 - MAT_WOOD_LBROWN,
18 - MAT_WOOD_DBROWN,
19 - MAT_WOOD_LGREY,
20 - MAT_WOOD_DGREY,

21 - MAT_WALL_BLACK,
22 - MAT_WALL_BROWN,
23 - MAT_WALL_DARKBROWN,
24 - MAT_WALL_LIGHTBROWN,
25 - MAT_WALL_GREY,
26 - MAT_WALL_DARKGREY,
27 - MAT_WALL_LIGHTGREY,
28 - MAT_WALL_GREEN,
29 - MAT_WALL_ORANGE,
30 - MAT_WALL_RED,
31 - MAT_WALL_WHITE,
32 - MAT_ROCK_FLESH,
33 - MAT_WALL_STRAW,
34 - MAT_ROCK_SNOW,


surfaceType - defines what sound to use when the brush breaks or takes damage
0 - defaults to stone... sorry Jersey :{ 
14 - wood
17 - stone			
28 - metal
35 - blood (how does blood break? I don't know.)
37 - glass
40 - paper

*/

void breakable_brush_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	float			volume;
	vec3_t			origin;
	byte x_max,y_max,z_max;
	int scale,numchunks;
	vec3_t			debrisNorm;

	VectorClear(debrisNorm);
	volume = self->size[0] * self->size[1] * self->size[2];

	// Calc numchunks???
	numchunks = (byte)((volume / 9000) + 2); 

	if (numchunks > 20)
		numchunks = 20;
	else if (numchunks < 10)
		numchunks = 10;

	// sending a scale of 0 will not give you the debris you want
	scale = DEBRIS_SM;

	// Calc scale on breakable brushes
	if (strcmp(self->classname, "func_breakable_brush") == 0)
	{
		// Find scale of debris to throw
		if (volume > 250000)
			scale = DEBRIS_LRG;
		else if (volume > 40000)
			scale = DEBRIS_MED;
		else 
			scale = DEBRIS_SM;
	}

	VectorAdd(self->absmax,self->absmin, origin);
	VectorScale(origin, .5, origin);

	if (self->material > 0)
	{
		if (strcmp(self->classname, "func_breakable_brush") == 0)
		{
			x_max = self->size[0];
			y_max = self->size[1];
			z_max = self->size[2]*0.5;

		}
		else
		{
			x_max = (byte) self->maxs[0];
			y_max = (byte) self->maxs[1];
			z_max = (byte) self->maxs[2];
		}

		VectorSubtract (other->s.origin, origin, debrisNorm);
		VectorNormalize(debrisNorm);
		FX_ThrowDebris(origin,debrisNorm, numchunks, scale, self->material,x_max,y_max,z_max, self->surfaceType);
	}
}

extern void breakable_brush_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	vec3_t			origin;
	float			volume;

	volume = self->size[0] * self->size[1] * self->size[2];
	
	VectorAdd(self->absmax,self->absmin, origin);
	VectorScale(origin, .5, origin);

	// kef -- possibly spawn some pickups from a broken crate?
	thePickupList.BustCrate(self);
	
	if (self->dmg)
	{
		if (!self->volume)
		{
			self->volume = self->dmg;
		}
		T_RadiusDamage (self, self, self->dmg, self, self->volume, MOD_EXPLOSIVE);	
		gmonster.RadiusDeafen(self, self->dmg, self->dmg);
	}

	BecomeDebris(self,inflictor,attacker,damage,point);
}
	
void breakable_brush_use(edict_t *self, edict_t *other, edict_t *activator)
{
	breakable_brush_die (self, self, other, self->health, vec3_origin);
}

void SP_func_breakable_brush (edict_t *ent)
{
	ent->solid = SOLID_BSP;
	ent->movetype = MOVETYPE_NONE;
	
	if (ent->spawnflags & INVINCIBLE)
	{
		ent->takedamage = DAMAGE_NO;
	}
	else if (ent->spawnflags & BREAKABLE_NOT_IN_CAM)
	{
		ent->takedamage = DAMAGE_YES_BUT_NOT_IN_CAMERA; // 3/1/00 kef -- used to be DAMAGE_YES
	}
	else
	{
		ent->takedamage = DAMAGE_YES;
	}
	
	if ((st.surfaceType > 0) && (st.surfaceType < SURF_NUM))
	{
		ent->surfaceType = st.surfaceType;
	}
	else
	{
		ent->surfaceType = SURF_DEFAULT; // kef -- ARGH!!!
	}

	if (!ent->health)
	{
		if ((ent->material == MAT_GLASS) || (ent->surfaceType == SURF_GLASS))
			ent->health = 50;	// So windows break after one shot.
		else
			ent->health = 100;
	}

	if (ent->targetname)
	{
		ent->use = breakable_brush_use;
	}
	
	ent->die = breakable_brush_die;

	if ((st.material < MATERIAL_NUM) && (st.material >= 0))
		ent->material = st.material;
	else
		ent->material = SURF_NONE;

	if (ent->spawnflags & GLASS_PAINCHUNKS)
	{
		ent->pain = breakable_brush_pain;
	}

	gi.setmodel (ent, ent->model);
	gi.linkentity (ent);
}

/*QUAKED func_trigger_brush (0 .5 .8) ? NO_DRAW START_OFF
A brush that can take damage to a point, then call its triggers.

Health defaults to 100.  

*/

#define		TRIGGER_BRUSH_NO_DRAW			1
#define		TRIGGER_BRUSH_START_OFF			2

extern void trigger_brush_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	G_UseTargets(self,self);
	G_FreeEdict(self);
}
	
void trigger_brush_use(edict_t *self, edict_t *other, edict_t *activator)
{
	// 1/7/00 kef -- this is what it used to do.
	//trigger_brush_die (self, self, other, self->health, vec3_origin);

	if (self->takedamage == DAMAGE_NO)
	{
		self->takedamage = DAMAGE_YES;
	}
	else
	{
		self->takedamage = DAMAGE_NO;
	}
}

void SP_func_trigger_brush (edict_t *ent)
{
	ent->solid = SOLID_BSP;
	ent->movetype = MOVETYPE_NONE;
	
	if (!ent->health)
	{
		ent->health = 100;
	}

	ent->takedamage = DAMAGE_YES;
	if (ent->spawnflags & TRIGGER_BRUSH_START_OFF)
	{
		ent->takedamage = DAMAGE_NO;
	}

	if (ent->spawnflags  & TRIGGER_BRUSH_NO_DRAW)
	{
		ent->svflags = SVF_NOCLIENT;
	}

	if (ent->targetname)
	{
		ent->use = trigger_brush_use;
	}
	
	ent->die = trigger_brush_die;

	gi.setmodel (ent, ent->model);
	gi.linkentity (ent);
}

void func_score_use(edict_t *self, edict_t *other, edict_t *activator)
{
	char	*teamname;

	if (activator->client)
	{
		if (self->team)	// Team specific score
		{
			teamname=Info_ValueForKey(activator->client->pers.userinfo,"teamname");
			if (teamname)
			{
				if (!strcmp(self->team,teamname))
				{
					activator->client->resp.score += self->count;
				}
			}
		}
		else
		{
			activator->client->resp.score += self->count;
		}
	}
}

/*QUAKED func_score (1 0 0) (-8 -8 -8) (8 8 8)
Awards certain amount of score to a player
-----KEYS-----
count - amount awarded to player
*/
void SP_func_score (edict_t *ent)
{
	ent->solid = SOLID_NOT;
	ent->takedamage = DAMAGE_NO;
	ent->use = func_score_use;
	gi.linkentity (ent);
}



//ShakeCameras (ent->s.origin, ent->health, ent->health*2);

/*QUAKED func_camerashake (0 .5 .8) (-8 -8 -8) (8 8 8)
Shakes the camera, centered on where the func is
"count" = intensity (from 0 to 100) - default is 30
"health" = radius of shaking (will fall off over distance) - default is 300
"delay" = the duration of the shaking (in seconds) - default is 3
"style" = makes a sound as it shakes the camera - 0 is default
	1 == Dull explosion
*/

#define DEFAULT(a, b) ((a) ? (a):(b))
#define FUNCSHAKE_THINK_TIME .2

void funcshake_think(edict_t *self)
{
	ShakeCameras(self->s.origin, self->count, self->health, DEFAULT_JITTER_DELTA);

	if(self->wait < level.time)
	{
		self->nextthink = 0;
	}
	else
	{
		self->nextthink = level.time + FUNCSHAKE_THINK_TIME;
	}
}

void funcshake_use (edict_t *self, edict_t *other, edict_t *activator)
{
	self->wait = level.time + self->delay;
	self->nextthink = level.time;//eh?

	if(self->style == 1)
	{
		gi.sound (self, CHAN_VOICE, gi.soundindex ("Ambient/Gen/Battle/ExpRvb.wav"), .8, 0, 0);
	}
}

void SP_func_camerashake(edict_t *self)
{
	self->use = funcshake_use;
	self->count = DEFAULT(self->count, 30);
	self->health = DEFAULT(self->health, 300);
	self->delay = DEFAULT(self->delay, 3);

	self->think = funcshake_think;

	if(self->style == 1)
	{
		gi.soundindex("Ambient/Gen/Battle/ExpRvb.wav");
	}
}

/*QUAKED _region (0 1 1) ?

For FOG Areas:
   fog_mode (0 = normal, 1 = additive)
   fog_start (0.0 to infinity) (4.0 recommended)
   fog_end  (fog_start to infinity) (400.0 recommended)
   fog_height (fog_start to infinity)
   fog_density (0.0 to 1.0)
   fog_density_range (-1.0 to 1.0)
   fog_color (vector RGB)
   fog_flags (1 = PULSATE, 2 = HEIGHT, 4 = MAP, 8 = ANTI)
   fog_time (speed)
   fog_distance_cull (distance for culling fog areas)
   fog_chop_size (size bsp should break up (128 default) )
*/


/*QUAKED func_fade (0 .5 .8) (-8 -8 -8) (8 8 8)  REVERSE
"color" color to fade to in red green blue format, from 0.0 to 1.0, (with 1.0 being white like "1.0 1.0 1.0")
"delay" the length of time it'll take to fully fade -- defaults to 5  NOTE: do NOT set this
to zero!  If you want instantaneous, use .1

Reverse flag fades from the color to "normal"

*/

#define FADE_REVERSE	1

void funcfade_think (edict_t *self)
{
	edict_t *player = NULL;

	while ((player = G_Find (player, FOFS(classname), "player")) != NULL)
	{
		if ( self->spawnflags & FADE_REVERSE)
		{
			player->client->fade_alpha -= .1/self->delay;
			if (player->client->fade_alpha < 0)
			{
				player->client->fade_alpha = 0;
				self->nextthink = 0;
			}
			else
			{
				self->nextthink = level.time+.1;
			}
		}
		else
		{
			player->client->fade_alpha += .1/self->delay;
			if (player->client->fade_alpha > 1)
			{
				player->client->fade_alpha = 1;
				self->nextthink = 0;
			}
			else
			{
				self->nextthink = level.time+.1;
			}
		}
	}
}

void funcfade_use (edict_t *self, edict_t *other, edict_t *activator) // put this in g_save
{
	edict_t *player = NULL;

	while ((player = G_Find (player, FOFS(classname), "player")) != NULL)
	{
		VectorCopy(self->intend_velocity, player->client->fade_rgb);
		if (self->spawnflags & FADE_REVERSE)
		{
			player->client->fade_alpha = 1 + .1/self->delay;
		}
		else
		{
			player->client->fade_alpha = 0;//just in case
		}			
	}
	funcfade_think(self);
}

void SP_func_fade(edict_t *self)
{

	VectorClear(self->intend_velocity);
	VectorCopy(st.color, self->intend_velocity);
	self->delay = DEFAULT(self->delay, 5);
	self->use = funcfade_use;
	self->think = funcfade_think;
	self->nextthink = 0;
}