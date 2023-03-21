#include "g_local.h"

#include "ai_private.h"
#include "ai_pathfinding.h"

#define	STEPSIZE	18

monster_global	gmonster;

extern cvar_t	*maxclients;

//============================================================================


/*
=================
AI_SetSightClient

Called once each frame to set level.sight_client to the
player to be checked for in findtarget.

If all clients are either dead or in notarget, sight_client
will be null.

In coop games, sight_client will cycle between the clients.
=================
*/

void monster_global::SetSightClient (void)
{
	edict_t	*ent;
	int		start, check;

	if (level.sight_client == NULL)
		start = 1;
	else
		start = level.sight_client - g_edicts;

	check = start;
	while (1)
	{
		check++;
		//this only checks clients
		if (check > game.maxclients)
			check = 1;
		ent = &g_edicts[check];

		if (ent->inuse
			//&& ent->health > 0//allow seeing dead guys
			/*&& !(ent->flags & FL_NOTARGET)*/)
		{
			level.sight_client = ent;
			break;		// got one
		}
		if (check == start)
		{
			level.sight_client = NULL;
			break;		// nobody to see
		}
	}

	if (level.sight_monster == NULL)
		start = game.maxclients;
	else
		start = level.sight_monster - g_edicts;

	check = start;

	while (1)
	{
		check++;

		//this checks all non-client ents
		if (check >= globals.num_edicts)
			check = game.maxclients;
		ent = &g_edicts[check];

		//allow only monsters--make sure current ent is monster
		if ((ent->inuse
			&& ent->health > 0
			&& !(ent->flags & FL_NOTARGET))
			&& (ent->ai)
			&& (ent->ai->IsActive()))
		{
			level.sight_monster = ent;
			break;		// got one
		}
		if (check == start)
		{
			level.sight_monster = NULL;
			break;		// nobody to see
		}
	}
}

//============================================================================
/*
.enemy
Will be world if not currently angry at anyone.

.movetarget
The next path spot to walk toward.  If .enemy, ignore .movetarget.
When an enemy is killed, the monster will try to return to it's path.

.hunt_time
Set to time + something when the player is in sight, but movement straight for
him is blocked.  This causes the monster to use wall following code for
movement direction instead of sighting on the player.

.ideal_yaw
A yaw angle of the intended direction, which will be turned towards at up
to 45 deg / state.  If the enemy is in view and hunt_time is not active,
this will be the exact line towards the enemy.

.pausetime
A monster will leave it's stand state and head towards it's .movetarget when
time > .pausetime.

walkmove(angle, speed) primitive is all or nothing
*/

/*
=============
range

can pass in the distance to be considered, if you want (default set-up will compute it for you).

returns the range catagorization of an entity reletive to self
0	melee range, will become hostile even if back is turned
1	visibility and infront, or visibility and show hostile
2	infront and show hostile
3	only triggered by damage
=============
*/

int monster_global::Range (edict_t *self, edict_t *other, float dist_sq)
{
	vec3_t	v;
	float	len;

	if (dist_sq>0)
	{
		len = dist_sq;
	}
	else
	{
		VectorSubtract (self->s.origin, other->s.origin, v);
		len = VectorLengthSquared (v);
	}

	if (len < MELEE_DISTANCE*MELEE_DISTANCE)
		return RANGE_MELEE;
	if (len < 250000)//500
		return RANGE_NEAR;
	if (len < 2250000)//1500
		return RANGE_MID;
	return RANGE_FAR;
}

/*
=============
visible

returns 1 if the entity is visible to self, even if not infront ()
=============
*/
qboolean monster_global::Visible (edict_t *self, edict_t *other)
{
	vec3_t	spot1;
	vec3_t	spot2;
	trace_t	trace;

	VectorCopy (self->s.origin, spot1);
	spot1[2] += self->viewheight;
	VectorCopy (other->s.origin, spot2);
	spot2[2] += other->viewheight;

	if (!gi.inPVS(spot1,spot2))
	{
		return false;
	}

	gi.trace (spot1, vec3_origin, vec3_origin, spot2, self, MASK_OPAQUE, &trace);
	
	if (trace.fraction == 1.0)
		return true;
	return false;
}


/*
=============
infront

returns 1 if the entity is in front (in sight) of self
=============
*/
qboolean monster_global::Infront (edict_t *self, edict_t *other)
{
	vec3_t	vec;
	float	dot;
	vec3_t	forward;
	float	threshold=0.75;
	float otherSpeedSq;
	
	if (self->ai)
	{
		self->ai->GetLookVector(forward);
		attention_index curAtLevel = self->ai->GetAttention(*self);

		switch(curAtLevel)
		{
		case ATTENTION_DISTRACTED:
			threshold = .85;
			break;
		case ATTENTION_IDLE:
			threshold = .75;
			break;
		case ATTENTION_ALERT:
			threshold = -.2;	//this will actually give guys a greater than 180 field of view - but for motion, this is fine
			break;
		}
	}
	else
	{
		AngleVectors (self->s.angles, forward, NULL, NULL);
	}
	VectorSubtract (other->s.origin, self->s.origin, vec);
	VectorNormalize (vec);
	dot = DotProduct (vec, forward);

	if (other->ai)
	{
		otherSpeedSq = VectorLengthSquared(other->ai->velocity);
	}
	if (other->client)
	{
		otherSpeedSq = VectorLengthSquared(other->velocity);
	}

	//if my senses are muted, pretend the guy's moving slower than he really is--brings thresholds in further
	if (self->spawnflags & SPAWNFLAG_SENSE_MUTE)
	{
		otherSpeedSq*=0.01;
//		otherSpeedSq=0;
		threshold+=0.225;
	}

	if (otherSpeedSq>10000)//walking
	{
//		gi.dprintf("player vel: %4.3f\n",sqrt(otherSpeedSq));
		threshold-=0.25;
		if (otherSpeedSq>40000)//running
		{
			threshold-=0.25;
		}
	}
	
	if (dot > threshold)
		return true;
	return false;
}


/*
=============
CheckBottom

Returns false if any part of the bottom of the entity is off an edge that
is not a staircase.

=============
*/

qboolean monster_global::CheckBottom (edict_t *ent)
{
	vec3_t	mins, maxs, start, stop;
	trace_t	trace;
	int		x, y;
	float	mid, bottom;
	
	VectorAdd (ent->s.origin, ent->mins, mins);
	VectorAdd (ent->s.origin, ent->maxs, maxs);

// if all of the points under the corners are solid world, don't bother
// with the tougher checks
// the corners must be within 16 of the midpoint
	start[2] = mins[2] - 1;
	for	(x=0 ; x<=1 ; x++)
		for	(y=0 ; y<=1 ; y++)
		{
			start[0] = x ? maxs[0] : mins[0];
			start[1] = y ? maxs[1] : mins[1];
			if (gi.pointcontents (start) != CONTENTS_SOLID)
				goto realcheck;
		}

	return true;		// we got out easy

realcheck:
//
// check it for real...
//
	start[2] = mins[2];
	
// the midpoint must be within 16 of the bottom
	start[0] = stop[0] = (mins[0] + maxs[0])*0.5;
	start[1] = stop[1] = (mins[1] + maxs[1])*0.5;
	stop[2] = start[2] - 2*STEPSIZE;
	gi.trace (start, vec3_origin, vec3_origin, stop, ent, MASK_MONSTERSOLID, &trace);

	if (trace.fraction == 1.0)
		return false;
	mid = bottom = trace.endpos[2];
	
// the corners must be within 16 of the midpoint	
	for	(x=0 ; x<=1 ; x++)
		for	(y=0 ; y<=1 ; y++)
		{
			start[0] = stop[0] = x ? maxs[0] : mins[0];
			start[1] = stop[1] = y ? maxs[1] : mins[1];
			
			gi.trace (start, vec3_origin, vec3_origin, stop, ent, MASK_MONSTERSOLID, &trace);
			
			if (trace.fraction != 1.0 && trace.endpos[2] > bottom)
				bottom = trace.endpos[2];
			if (trace.fraction == 1.0 || mid - trace.endpos[2] > STEPSIZE)
				return false;
		}

	return true;
}



qboolean monster_global::CheckBottom (ai_c &which_ai, edict_t *ent, vec3_t pos, int motion_allowed)
{
	vec3_t	mins, maxs, start, stop;
	trace_t	trace;
	int		x, y;
	float	mid, bottom;
	float	checkheight;
	
	//does the motion mask passed in allow falling or stepping or neither?
	if (motion_allowed & fall_movemask)
	{
//		return true;
//		checkheight = which_ai.GetFallHeight();
		checkheight = which_ai.GetStepHeight();
	}
	//check stepheight under position
	else if (motion_allowed & step_movemask)
	{
		checkheight = which_ai.GetStepHeight();
	}
	//just check tiny distance under position
	else
	{
		checkheight = 2;
	}

	VectorAdd (pos, ent->mins, mins);
	VectorAdd (pos, ent->maxs, maxs);

// if all of the points under the corners are solid world, don't bother
// with the tougher checks
// the corners must be within 16 of the midpoint
	start[2] = mins[2] - 1;
	for	(x=0 ; x<=1 ; x++)
		for	(y=0 ; y<=1 ; y++)
		{
			start[0] = x ? maxs[0] : mins[0];
			start[1] = y ? maxs[1] : mins[1];
			if (gi.pointcontents (start) != CONTENTS_SOLID)
			{
				goto realcheck;
			}
		}

	return true;		// we got out easy

realcheck:
//
// check it for real...
//
	start[2] = mins[2];
	
// the midpoint must be within 16 of the bottom
	start[0] = stop[0] = (mins[0] + maxs[0])*0.5;
	start[1] = stop[1] = (mins[1] + maxs[1])*0.5;
	stop[2] = start[2] - 2*checkheight;
	gi.trace (start, vec3_origin, vec3_origin, stop, ent, MASK_MONSTERSOLID, &trace);

	//midpoint trace didn't hit anything solid
	if (trace.fraction == 1.0)
	{
		return false;
	}
	mid = bottom = trace.endpos[2];
	
// the corners must be within 16 of the midpoint	
	for	(x=0 ; x<=1 ; x++)
		for	(y=0 ; y<=1 ; y++)
		{
			start[0] = stop[0] = x ? maxs[0] : mins[0];
			start[1] = stop[1] = y ? maxs[1] : mins[1];
			
			gi.trace (start, vec3_origin, vec3_origin, stop, ent, MASK_MONSTERSOLID, &trace);
			
			//update lowest point in groundcheck
			if (trace.fraction != 1.0 && trace.endpos[2] > bottom)
			{
				bottom = trace.endpos[2];
			}
			//if any of the corners trace down farther than i'm allowed to fall, this position not valid
			if (trace.fraction == 1.0 || mid - trace.endpos[2] > checkheight)
			{
				return false;
			}
		}

	return true;
}


void monster_global::CheckGround (edict_t *ent)
{
	vec3_t		point;
	trace_t		trace;
	edict_t		*oldground = ent->groundentity;

	if (ent->flags & (FL_SWIM|FL_FLY))
		return;

	if (ent->velocity[2] > ESCAPE_VELOCITY) // moving up, so it's not on the ground!
	{
		ent->groundentity = NULL;
		return;
	}

// if the hull point one-quarter unit down is solid the entity is on ground
	point[0] = ent->s.origin[0];
	point[1] = ent->s.origin[1];
	point[2] = ent->s.origin[2] - 0.25;
	if (ent->groundentity)
	{
		if (ent->groundentity->solid == SOLID_GHOUL)
		{
			point[2] = ent->s.origin[2] - 2.5;
		}
	}

	gi.trace (ent->s.origin, ent->mins, ent->maxs, point, ent, MASK_MONSTERSOLID, &trace);

	// check steepness
	if ( (trace.plane.normal[2] < FLOOR_STEEPNESS) && !trace.startsolid)
	{
		ent->groundentity = NULL; // slide down this...
		return;
	}

//	ent->groundentity = trace.ent;
//	ent->groundentity_linkcount = trace.ent->linkcount;
//	if (!trace.startsolid && !trace.allsolid)
//		VectorCopy (trace.endpos, ent->s.origin);
	if (!trace.startsolid && !trace.allsolid)
	{
		VectorCopy (trace.endpos, ent->s.origin);
		ent->groundentity = trace.ent;
		ent->groundentity_linkcount = trace.ent->linkcount;

		//landed--stop falling
		if (ent->groundentity)
		{
			if ((ent->groundentity != oldground) && ent->groundentity->touch)
			{
					ent->groundentity->touch(ent->groundentity, ent, NULL, NULL);
			}
			ent->velocity[2] = 0;
		}
	}
}


void monster_global::CatagorizePosition (edict_t *ent)
{
	vec3_t		point;
	int			cont;

//
// get waterlevel
//
	point[0] = ent->s.origin[0];
	point[1] = ent->s.origin[1];
	point[2] = ent->s.origin[2] + ent->mins[2] + 1;	
	cont = gi.pointcontents (point);

	if (!(cont & MASK_WATER))
	{
		ent->waterlevel = 0;
		ent->watertype = 0;
		return;
	}

	ent->watertype = cont;
	ent->waterlevel = 1;
	point[2] += 26;
	cont = gi.pointcontents (point);
	if (!(cont & MASK_WATER))
		return;

	ent->waterlevel = 2;
	point[2] += 22;
	cont = gi.pointcontents (point);
	if (cont & MASK_WATER)
		ent->waterlevel = 3;
}


void monster_global::WorldEffects (edict_t *ent)
{
	int		dmg;

	if (ent->health > 0)
	{
		if (!(ent->flags & FL_SWIM))
		{
			if (ent->waterlevel < 3)
			{
				ent->air_finished = level.time + 12;
			}
			else if (ent->air_finished < level.time)
			{	// drown!
				if (ent->pain_debounce_time < level.time)
				{
					dmg = 2 + 2 * floor(level.time - ent->air_finished);
					if (dmg > 15)
						dmg = 15;
					T_Damage (ent, world, world, vec3_origin, ent->s.origin, ent->s.origin, dmg, 0, DAMAGE_NO_ARMOR|DT_WATER, MOD_WATER);
					ent->pain_debounce_time = level.time + 1;
				}
			}
		}
		else
		{
			if (ent->waterlevel > 0)
			{
				ent->air_finished = level.time + 9;
			}
			else if (ent->air_finished < level.time)
			{	// suffocate!
				if (ent->pain_debounce_time < level.time)
				{
					dmg = 2 + 2 * floor(level.time - ent->air_finished);
					if (dmg > 15)
						dmg = 15;
					T_Damage (ent, world, world, vec3_origin, ent->s.origin, ent->s.origin, dmg, 0, DAMAGE_NO_ARMOR|DT_WATER, MOD_WATER);
					ent->pain_debounce_time = level.time + 1;
				}
			}
		}
	}
	
	if (ent->waterlevel == 0)
	{
		if (ent->flags & FL_INWATER)
		{	
			// 9/14/99 kef -- Chia wants this out
//			gi.sound (ent, CHAN_BODY, gi.soundindex("player/watr_out.wav"), .6, ATTN_NORM, 0);
			ent->flags &= ~FL_INWATER;
		}
		return;
	}

	if ((ent->watertype & CONTENTS_LAVA) && !(ent->flags & FL_IMMUNE_LAVA))
	{
		if (ent->damage_debounce_time < level.time)
		{
			ent->damage_debounce_time = level.time + 0.2;
			T_Damage (ent, world, world, vec3_origin, ent->s.origin, ent->s.origin, 10*ent->waterlevel, 0, DAMAGE_NO_ARMOR|DT_FIRE, MOD_LAVA);
		}
	}
	if ((ent->watertype & CONTENTS_SLIME) && !(ent->flags & FL_IMMUNE_SLIME))
	{
		if (ent->damage_debounce_time < level.time)
		{
			ent->damage_debounce_time = level.time + 1;
			T_Damage (ent, world, world, vec3_origin, ent->s.origin, ent->s.origin, 4*ent->waterlevel, 0, DAMAGE_NO_ARMOR|DT_WATER, MOD_SLIME);
		}
	}
	
	if ( !(ent->flags & FL_INWATER) )
	{	
		if (!(ent->svflags & SVF_DEADMONSTER))
		{
/*			if (ent->watertype & CONTENTS_LAVA)
				if (random() <= 0.5)
					gi.sound (ent, CHAN_BODY, gi.soundindex("player/lava1.wav"), .6, ATTN_NORM, 0);
				else
					gi.sound (ent, CHAN_BODY, gi.soundindex("player/lava2.wav"), .6, ATTN_NORM, 0);
			else if (ent->watertype & CONTENTS_SLIME)
				gi.sound (ent, CHAN_BODY, gi.soundindex("player/watr_in.wav"), .6, ATTN_NORM, 0);
			else if (ent->watertype & CONTENTS_WATER)
				gi.sound (ent, CHAN_BODY, gi.soundindex("player/watr_in.wav"), .6, ATTN_NORM, 0);*/
		}

		ent->flags |= FL_INWATER;
		ent->damage_debounce_time = 0;
	}
}

void monster_global::DropToFloor (edict_t *ent)
{
	vec3_t		end;
	trace_t		trace;

	ent->s.origin[2] += 1;
	VectorCopy (ent->s.origin, end);
	end[2] -= 256;
	
	gi.trace (ent->s.origin, ent->mins, ent->maxs, end, ent, MASK_MONSTERSOLID, &trace);

	if (trace.fraction == 1 || trace.allsolid)
		return;

	VectorCopy (trace.endpos, ent->s.origin);

	gi.linkentity (ent);
	CheckGround (ent);
	CatagorizePosition (ent);
}

void monster_global::MakeSound(edict_t *listener, vec3_t origin, float time, edict_t *noisemaker, ai_sensetype_e type)
{
	if (listener->ai)
	{
		listener->ai->RegisterSenseEvent(sound_mask, origin, time, noisemaker, type);
//		listener->ai->NewSense(new sound_sense(origin, time, noisemaker, type), listener);
	}
}


void monster_global::RadiusDeafen (edict_t *inflictor, float degree, float radius)
{
	float	points;
	edict_t	*ent = NULL;
	vec3_t	v;
	vec3_t	inflictor_origin;

//	This computes a pseudo origin for bmodels, such as func_breakable_brush, which
//	don't have an origin.
	FindCorrectOrigin(inflictor, inflictor_origin); 

//	look for targets within (radius) from inflictor_origin
	CRadiusContent rad(inflictor_origin, radius);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		ent = rad.foundEdict(i);

		if(!ent->ai)
		{
			continue;
		}
		if(ent->health <= 0)
		{
			continue;
		}
		if(ent == world)
		{
			continue;
		}

		VectorAdd (ent->mins, ent->maxs, v);
		VectorMA (ent->s.origin, 0.5, v, v);
		VectorSubtract (inflictor_origin, v, v);
		points = degree * (1- (VectorLength(v) / radius));
		if (points > 0)
		{
			if (points > 100)
			{
				points = 100;
			}
			ent->ai->MuteSenses(sound_mask, points, smute_recov_linear, points);
		}
	}
}

typedef struct eSort_s
{
	float dist;
	int	  index;
}eSort_t;

int eCmp(void const *a, void const *b)
{
	eSort_t *esA, *esB;

	esA = (eSort_t *)a;
	esB = (eSort_t *)b;

	//ahem. sort from LEAST to GREATEST. --ss
	return (esA->dist < esB->dist) ? -1:1;
}

//fixme - base this off of skill level somehow?
#define ENEMIES_NUM_HIGH 7
#define ENEMIES_NUM_MEDIUM 13
#define PRIORITY_UPDATE_TIME .5

void monster_global::UpdateEnemyPriority(void)
{
	eSort_t	elist[512];//fixme - um, huh?  Is that really bad?  We BETTER not exceed this...
	int		count = 0;
	edict_t *ent = &g_edicts[0];
	float	*source;

	if(!level.sight_client)return;

	numNearbyEnemies = 0;
	numSeenEnemies = 0;

	aiPoints.setupClientPathInfo(&clientNode, &clientRegion);

	for (int i = 0 ; i<globals.num_edicts && numNearbyEnemies<MAX_SEARCHABLE_ENEMIES; i++, ent++)
	{	//go through the edict list and add all active ai's...
		if (!ent->inuse)continue;
		if (!ent->ai)continue;
		if (ent->health < 1)continue;//these guys don't count
		if(ent->spawnflags&SPAWNFLAG_TRIGGER_SPAWN)continue;

		vec3_t dif;
		VectorSubtract(ent->s.origin, level.sight_client->s.origin, dif);
		float val = (dif[0] * dif[0]) + (dif[1] * dif[1]) + (dif[2]*dif[2]*16);//sort of an oval, really

		if(ent->ai->IsActive() || (val < (1024*1024)))
		//if(val < (1024*1024))
		{
			nearbyEnemies[numNearbyEnemies] = i;
			numNearbyEnemies++;
		}
		if (!ent->ai->IsActive())continue;

		if(gi.inPVS(level.sight_client->s.origin,ent->s.origin))
		{
			numSeenEnemies++;
		}

		// all enemies should always have their positional stuff handled
		ent->ai->UpdatePathPosition(ent);
	}

	//something has gone very wrong with priorities--update now
	if (lastPriorityUpdate > level.time + 0.1)
	{
		lastPriorityUpdate = level.time - PRIORITY_UPDATE_TIME - 0.1;
	}

	// only do this every 5 frames (is this too infrequent?)
	if(lastPriorityUpdate > level.time - PRIORITY_UPDATE_TIME)
	{
		return;
	}

	//this is not very good criteria - can we change this around and make it good and tasty?

	source = level.sight_client->s.origin;

	ent = &g_edicts[0];

	float closestDist = 999999;
	int closestGuy = -1;

	int highAdded = 0;
	for (i = 0 ; i<globals.num_edicts ; i++, ent++)
	{	//go through the edict list and add all active ai's...
		if (!ent->inuse ||
			!ent->ai ||
			ent->health < 1 ||
			!ent->ai->IsActive())
		{
			continue;
		}

		float curDist = (source[0] - ent->s.origin[0])*(source[0] - ent->s.origin[0]) + 
							(source[1] - ent->s.origin[1])*(source[1] - ent->s.origin[1]) + 
							((source[2] - ent->s.origin[2])*(source[2] - ent->s.origin[2])*9);//vertical differences count thrice as much

		if(i && curDist < closestDist)
		{
			closestGuy = i;
			closestDist = curDist;
		}

		if(gi.inPVS(level.sight_client->s.origin,ent->s.origin))
		{
			ent->ai->SetPriority(PRIORITY_HIGH);
			highAdded++;
			continue;
		}

		if(ai_highpriority->value)
		{
			ent->ai->SetPriority(PRIORITY_HIGH);
		}
		else
		{
			ent->ai->SetPriority(PRIORITY_LOW);
		}

		elist[count].index = i;
//		if(!elist[count].dist)
		{	//err... just guess then
			elist[count].dist = curDist;
		}
		count++;
	}

	//bad?  I dunno
	qsort((void *)elist, count, sizeof(eSort_t), eCmp);

	if(highAdded < ENEMIES_NUM_HIGH)
	{
		for(i = 0; i < count && i < (ENEMIES_NUM_HIGH-highAdded); i++)
		{
			g_edicts[elist[i].index].ai->SetPriority(PRIORITY_HIGH);
		}
	}

	if(closestGuy != -1)
	{
		closestEnemy = closestGuy;
	}
	else
	{
		closestEnemy = 0;
	}

	lastPriorityUpdate = level.time;
}


void monster_global::FindGuysInRadius(vec3_t center, float radius, CRadiusContent *content, int squashZ)
{
	float radSq = radius*radius;//minor optimization --sfs

	// hopefully in most cases this will be a fairly small number, like 10 or 15
	// also note that due to the radius restrictions of the list, searches greater than 1024 units won't work properly
	for(int i = 0; i < numNearbyEnemies; i++)
	{
		vec3_t dif;

		VectorSubtract(g_edicts[nearbyEnemies[i]].s.origin, center, dif);

		if(squashZ)
		{	// our game is largely 2d, not so much 3d.  Sometimes this is needed
			dif[2] *= 3;//this will narrow the z portion to a 3rd of the x and y portions
		}

		if(DotProduct(dif, dif) < radSq)
		{
			content->addEnt(&g_edicts[nearbyEnemies[i]]);
		}
	}
}

qboolean monster_global::IsReadyForWakeSound(void)
{
	//if wake sound time is greater than level time, something's fucked
	if (lastWakeSound > level.time + 0.1)
	{
		lastWakeSound = level.time - 3.0;
	}

	return lastWakeSound<level.time-2.0;

/*
	return 1;
	*/
}

qboolean monster_global::IsReadyForVoiceSound(void)
{
	//if wake sound time is greater than level time, something's fucked
	if (lastVoiceSound > level.time + 0.1)
	{
		lastVoiceSound = level.time - 3.0;
	}

	return lastVoiceSound<level.time-2.0;
}

void monster_global::SpookEnemies(vec3_t center, float radius, int style)
{
	CRadiusContent rad(center, radius, 1, 0);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		edict_t *curSearch = rad.foundEdict(i);

		if(curSearch->ai)
		{	//try to spook this guy
			curSearch->ai->Spook(style, center);
		}
	}

}

void monster_global::AddSoundTrigger(int val)
{
	for(int i = 0; i < numSoundTriggers; i++)
	{
		if(soundTriggers[i] == val)
		{
			return;
		}
	}
	if(numSoundTriggers == MAX_SOUND_TRIGGERS)
	{
		return;//fixme - error condition?
	}
	soundTriggers[numSoundTriggers] = val;
	numSoundTriggers++;
}

void monster_global::RemoveSoundTrigger(int val)
{
	if(!soundTriggersValid)
	{
		InitSoundTriggers(); 
	}

	for(int i = 0; i < numSoundTriggers; i++)
	{
		//warning - assumes each entry shows up only once (which should be the case)
		if(soundTriggers[i] == val)
		{
			soundTriggers[i] = soundTriggers[numSoundTriggers - 1];
			numSoundTriggers--;
			return;
		}
	}
}

void monster_global::InitSoundTriggers(void)
{
	ClearSoundTriggers();

	edict_t	 *curTest = g_edicts;
	for ( curTest = g_edicts; curTest < &g_edicts[globals.num_edicts] ; curTest++)
	{
		if (!curTest->inuse)continue;
		if(strcmp(curTest->classname, "trigger_sound"))continue;

		AddSoundTrigger(curTest - g_edicts);
	}

	soundTriggersValid = 1;
}

edict_t *monster_global::GetClosestEnemy(void)
{
	return closestEnemy ? &g_edicts[closestEnemy]:0;
}