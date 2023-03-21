// g_utils.c -- misc utility functions for game module

#include "g_local.h"
#include "matrix4.h"
#include "fields.h"
#include "..\qcommon\ef_flags.h"

void G_ProjectSource (vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result)
{
	result[0] = point[0] + forward[0] * distance[0] + right[0] * distance[1];
	result[1] = point[1] + forward[1] * distance[0] + right[1] * distance[1];
	result[2] = point[2] + forward[2] * distance[0] + right[2] * distance[1] + distance[2];
}


/*
=============
G_Find

Searches all active entities for the next one that holds
the matching string at fieldofs (use the FOFS() macro) in the structure.

Searches beginning at the edict after from, or the beginning if NULL
NULL will be returned if the end of the list is reached.

chars to compare is the number of characters to compare for a match (like strncmp) -
	set to 0 to mimic old behavior

=============
*/
edict_t *G_Find (edict_t *from, int fieldofs, char *match, int charsToCompare)
{
	char	*s;

	if (!match)
	{
		return NULL;
	}

	if (!from)
		from = g_edicts;
	else
		from++;

	for ( ; from < &g_edicts[globals.num_edicts] ; from++)
	{
		if (!from->inuse)
			continue;
		s = *(char **) ((byte *)from + fieldofs);
		if (!s)
			continue;
		if(charsToCompare)
		{
			if (!strnicmp(s, match, charsToCompare))
				return from;
		}
		else
		{
			if (!stricmp (s, match))
				return from;
		}
	}

	return NULL;
}

/*
=================
findradius

Returns entities that have origins within a spherical area

findradius (origin, radius)
=================
*/
//#if 1
//fast version

CRadiusContent::CRadiusContent(vec3_t center, float radius, areatype_t desiredSolidTypes)
{
	numFound = 0;

	float	max2;
	vec3_t	min;
	vec3_t	max;
	vec3_t	eorg;
	int		j;
	edict_t *attachTouchList[MAX_RADIUS_FIND_SIZE];
	int		numTrigFound = 0;

	max2=radius*radius;
	VectorCopy(center,min);
	VectorCopy(center,max);
	for (j=0 ; j<3 ; j++)
	{
		min[j]-=radius;
		max[j]+=radius;
	}

	switch (desiredSolidTypes)
	{
	case RADAREA_SOLIDS:
		numFound = gi.BoxEdicts(min,max, touchlist, MAX_RADIUS_FIND_SIZE, AREA_SOLID);
		break;
	case RADAREA_TRIGGERS:
		numFound = gi.BoxEdicts(min,max, touchlist, MAX_RADIUS_FIND_SIZE, AREA_TRIGGERS);
		break;
	case RADAREA_SOLIDSANDTRIGGERS:
		numFound = gi.BoxEdicts(min,max, touchlist, MAX_RADIUS_FIND_SIZE, AREA_SOLID);
		numTrigFound = gi.BoxEdicts(min,max, attachTouchList, MAX_RADIUS_FIND_SIZE, AREA_TRIGGERS);
		break;
	default:
	case RADAREA_NONE:
		return;
		break;
	}

	for (int i = 0; i < numTrigFound; i++)
	{
		touchlist[numFound+i] = attachTouchList[i];
	}
	numFound += numTrigFound;

	// if this returns MAX_RADIUS_FIND_SIZE, you've just searched an area with at least 128 ents around - wow!  If so, we've got
	// problems...  That's just too much.
	assert(numFound < MAX_RADIUS_FIND_SIZE);

	// The following two steps eliminate invalid guys

	int foundNum = numFound;

	for( i = 0; i < foundNum; i++)
	{
		if(!touchlist[i]->inuse)
		{
			touchlist[i] = 0;
			numFound--;
		}
		else
		{
			for (j = 0 ; j<3 ; j++)
			{
				eorg[j] = center[j] - (touchlist[i]->s.origin[j] + (touchlist[i]->mins[j] + touchlist[i]->maxs[j])*0.5);
			}
			if(VectorLengthSquared(eorg) > max2)
			{
				touchlist[i] = 0;
				numFound--;
			}
		}
	}

	edict_t **start = &touchlist[0];
	edict_t **end = &touchlist[foundNum-1];

	do
	{
		while((*start)&&(start < end))
		{
			start++;
		}
		while((!(*end))&&(end > start))
		{
			end--;
		}

		// this should be the first set of empty / fulls we can find
		*start = *end;
		start++;
		end--;
	}while(start < end);
}

CRadiusContent::CRadiusContent(vec3_t center, float radius, int useAIPoints, int nodeID, int squashZ)
{
	numFound = 0;

	if(useAIPoints)
	{
		// this no longer uses the AIpoints but instead something a bit quicker, easier, and more consistent
		gmonster.FindGuysInRadius(center, radius, this, squashZ);
		return;
	}

	float	max2;
	vec3_t	min;
	vec3_t	max;
	vec3_t	eorg;
	int		j;

	max2=radius*radius;
	VectorCopy(center,min);
	VectorCopy(center,max);
	for (j=0 ; j<3 ; j++)
	{
		min[j]-=radius;
		max[j]+=radius;
	}

	numFound = gi.BoxEdicts(min,max, touchlist, MAX_RADIUS_FIND_SIZE, AREA_SOLID);

	// if this returns MAX_RADIUS_FIND_SIZE, you've just searched an area with at least 128 ents around - wow!  If so, we've got
	// problems...  That's just too much.
	assert(numFound < MAX_RADIUS_FIND_SIZE);

	// The following two steps eliminate invalid guys

	int foundNum = numFound;

	for(int i = 0; i < foundNum; i++)
	{
		if(!touchlist[i]->inuse)
		{
			touchlist[i] = 0;
			numFound--;
		}
		else
		{
			for (j = 0 ; j<3 ; j++)
			{
				eorg[j] = center[j] - (touchlist[i]->s.origin[j] + (touchlist[i]->mins[j] + touchlist[i]->maxs[j])*0.5);
			}
			if(VectorLengthSquared(eorg) > max2)
			{
				touchlist[i] = 0;
				numFound--;
			}
		}
	}

	edict_t **start = &touchlist[0];
	edict_t **end = &touchlist[foundNum-1];

	do
	{
		while((*start)&&(start < end))
		{
			start++;
		}
		while((!(*end))&&(end > start))
		{
			end--;
		}

		// this should be the first set of empty / fulls we can find
		*start = *end;
		start++;
		end--;
	}while(start < end);
}

/********************/

edict_t *performTriggerSearch(edict_t *ent, vec3_t source, float useRange)
{
	edict_t		*curSearch = NULL;

	VectorCopy(ent->s.origin, source);

	// if our bbox is intersecting with multiple triggers, that's a 
	//design error in the map. if we find a trigger, use it.
	while( (curSearch = findradius(curSearch, source, useRange, AREA_TRIGGERS)) )
	{
		if (curSearch->plUse && strstr(curSearch->classname, "trigger"))
		{
			return curSearch;
		}
	}
	return NULL;
}

edict_t *performEntitySearch(edict_t *ent, vec3_t source, float useRange)
{
	vec3_t		toOther,looking;
	float		bestDp, curDp, dpMin;
	edict_t		*bestSearch = NULL, *curSearch = NULL;
	vec3_t		tempOrg;
	trace_t		tr;

	dpMin = bestDp = 0.2; // this value affects how similar the players view vector and the vector
	//from the player to the used entity have to be
	AngleVectors(ent->client->ps.viewangles,looking,NULL,NULL);

	VectorCopy(ent->s.origin, source);
	source[2] += ent->client->ps.viewoffset[2];

	while(curSearch = findradius(curSearch, source, useRange, AREA_SOLID))
	{
		if (!curSearch->plUse)
		{
			continue;
		}

		FindCorrectOrigin(curSearch, tempOrg);

		VectorSubtract(tempOrg, ent->s.origin, toOther);
		toOther[2] = 0; // we're gonna ignore the z component so there!
		looking[2] = 0;
		VectorNormalize(looking);
		VectorNormalize(toOther);
		curDp = DotProduct(toOther, looking);

		if(curDp <= bestDp)continue;

		gi.trace(source, NULL, NULL, tempOrg, ent, MASK_ALL, &tr);
		if((tr.ent != curSearch) && (tr.fraction != 1.0))continue;

		bestDp = curDp;
		bestSearch = curSearch;
	}
	return bestSearch;
}

edict_t *findinbounds(edict_t *from, vec3_t min, vec3_t max, int nAreatype)
{
	static edict_t *touchlist[MAX_EDICTS];
	static int index=-1;
	static int num;

	if (!from)
	{
		num = gi.BoxEdicts(min,max, touchlist, MAX_EDICTS, nAreatype/*usually AREA_SOLID*/);
		index=0;
	}
	else
	{
		assert(touchlist[index]==from);
		// you cannot adjust the pointers yourself...
		// this means you did not call it with the previous edict
		index++;
	}
	for (;index<num;index++)
	{
		if (!touchlist[index]->inuse)
			continue;
		return touchlist[index];
	}
	return NULL;
}

edict_t *findradius (edict_t *from, vec3_t org, float rad, int nAreatype /*AREA_SOLID*/)
{
	static float max2;
	static vec3_t min;
	static vec3_t max;
	vec3_t	eorg;
	int		j;
	float elen;

	if (!from)
	{
		max2=rad*rad;
		VectorCopy(org,min);
		VectorCopy(org,max);
		for (j=0 ; j<3 ; j++)
		{
			min[j]-=rad;
			max[j]+=rad;
		}
	}
	while (1)
	{
		from=findinbounds(from,min,max, nAreatype);
		if (!from)
		{
			return 0;
		}
		if (!from->inuse)
			continue;
		// if we did an AREA_TRIGGERS search and came back with a trigger, there's no need
		//to check anything else. just return the trigger.
		if (AREA_TRIGGERS == nAreatype)
		{
			return from;
		}
		for (j=0 ; j<3 ; j++)
			eorg[j] = org[j] - (from->s.origin[j] + (from->mins[j] + from->maxs[j])*0.5);
		elen = DotProduct(eorg,eorg);
		if (elen > max2)
			continue;
		return from;
	} 
}

//#else
//slow version
edict_t *oldfindradius (edict_t *from, vec3_t org, float rad)
{
	vec3_t	eorg;
	int		j;

	if (!from)
		from = g_edicts;
	else
		from++;
	for ( ; from < &g_edicts[globals.num_edicts]; from++)
	{
		if (!from->inuse)
			continue;
		if (from->solid == SOLID_NOT)
			continue;
		for (j=0 ; j<3 ; j++)
			eorg[j] = org[j] - (from->s.origin[j] + (from->mins[j] + from->maxs[j])*0.5);
		if (VectorLength(eorg) > rad)
			continue;
		return from;
	}

	return NULL;
}
//#endif

/*
=============
G_PickTarget

Searches all active entities for the next one that holds
the matching string at fieldofs (use the FOFS() macro) in the structure.

Searches beginning at the edict after from, or the beginning if NULL
NULL will be returned if the end of the list is reached.

=============
*/
#define MAXCHOICES	8

edict_t *G_PickTarget (char *targetname)
{
	edict_t	*ent = NULL;
	int		num_choices = 0;
	edict_t	*choice[MAXCHOICES];

	if (!targetname)
	{
		gi.dprintf("G_PickTarget called with NULL targetname\n");
		return NULL;
	}

	while(1)
	{
		ent = G_Find (ent, FOFS(targetname), targetname);
		if (!ent)
			break;
		choice[num_choices++] = ent;
		if (num_choices == MAXCHOICES)
			break;
	}

	if (!num_choices)
	{
		gi.dprintf("G_PickTarget: target %s not found\n", targetname);
		return NULL;
	}

	return choice[rand() % num_choices];
}



void Think_Delay (edict_t *ent)
{
	G_UseTargets (ent, ent->activator);
	G_FreeEdict (ent);
}

/*
==============================
G_UseTargets

the global "activator" should be set to the entity that initiated the firing.

If self.delay is set, a DelayedUse entity will be created that will actually
do the SUB_UseTargets after that many seconds have passed.

Centerprints any self.message to the activator.

Search for (string)targetname in all entities that
match (string)self.target and call their .use function

==============================
*/
void G_UseTargets (edict_t *ent, edict_t *activator)
{
	edict_t		*t;

//
// check for a delay
//
	if (ent->delay)
	{
	// create a temp object to fire at a later time
		t = G_Spawn();
		t->classname = "DelayedUse";
		t->nextthink = level.time + ent->delay;
		t->think = Think_Delay;
		t->activator = activator;
		if (!activator)
			gi.dprintf ("Think_Delay with no activator\n");
		t->message = ent->message;
		t->target = ent->target;
		t->killtarget = ent->killtarget;
		t->killfacing = ent->killfacing;		
		return;
	}
	
	
//
// print the message
//
	if ((ent->message || ent->sp_message) && !(activator->svflags & SVF_MONSTER))
	{
		if (ent->sp_message)
		{
			gi.SP_Print(activator, ent->sp_message);
		}
		else
		{
			gi.centerprintf (activator, "%s", ent->message);
		}
		if (ent->noise_index)
		{
			gi.sound (activator, CHAN_AUTO, ent->noise_index, .6, ATTN_NORM, 0);
		}
		else
		{
			gi.sound (activator, CHAN_AUTO, gi.soundindex ("Misc/Talk.wav"), .6, ATTN_NORM, 0);
		}
	}

//
// kill killtargets
//
	if (ent->killtarget)
	{
		t = NULL;
		while ((t = G_Find (t, FOFS(targetname), ent->killtarget)))
		{
			G_FreeEdict (t);
			if (!ent->inuse)
			{
				gi.dprintf("entity was removed while using killtargets\n");
				return;
			}
		}
	}

//
// fire targets
//
	if (ent->target)
	{
		t = NULL;
		while ((t = G_Find (t, FOFS(targetname), ent->target)))
		{
			// doors fire area portals in a specific way
			if (!stricmp(t->classname, "func_areaportal") &&
				(!stricmp(ent->classname, "func_door") || !stricmp(ent->classname, "func_door_rotating")))
				continue;

			if (t == ent)
			{
				gi.dprintf ("WARNING: Entity used itself.\n");
			}
			else
			{
				if (t->use)
					t->use (t, ent, activator);
			}
			if (!ent->inuse)
			{
				gi.dprintf("entity was removed while using targets\n");
				return;
			}
		}
	}
}


/*
=============
TempVector

This is just a convenience function
for making temporary vectors for function calls
=============
*/
float	*tv (float x, float y, float z)
{
	static	int		index;
	static	vec3_t	vecs[8];
	float	*v;

	// use an array so that multiple tempvectors won't collide
	// for a while
	v = vecs[index];
	index = (index + 1)&7;

	v[0] = x;
	v[1] = y;
	v[2] = z;

	return v;
}


/*
=============
VectorToString

This is just a convenience function
for printing vectors
=============
*/
char	*vtos (vec3_t v)
{
	static	int		index;
	static	char	str[8][32];
	char	*s;

	// use an array so that multiple vtos won't collide
	s = str[index];
	index = (index + 1)&7;

	Com_sprintf (s, 32, "(%i %i %i)", (int)v[0], (int)v[1], (int)v[2]);

	return s;
}


vec3_t VEC_UP		= {0, -1, 0};
vec3_t MOVEDIR_UP	= {0, 0, 1};
vec3_t VEC_DOWN		= {0, -2, 0};
vec3_t MOVEDIR_DOWN	= {0, 0, -1};

void G_SetMovedir (vec3_t angles, vec3_t movedir)
{
	if (VectorCompare (angles, VEC_UP))
	{
		VectorCopy (MOVEDIR_UP, movedir);
	}
	else if (VectorCompare (angles, VEC_DOWN))
	{
		VectorCopy (MOVEDIR_DOWN, movedir);
	}
	else
	{
		AngleVectors (angles, movedir, NULL, NULL);
	}

	VectorClear (angles);
}


float vectoyaw (vec3_t vec)
{
	float	yaw;
	
	if (vec[YAW] == 0 && vec[PITCH] == 0)
		yaw = 0;
	else
	{
		yaw = (int) (atan2(vec[YAW], vec[PITCH]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;
	}

	return yaw;
}


vec_t VectorNormalize2 (vec3_t v, vec3_t out)
{
	float	length;
	float	ilength = 1;

	length = sqrt(DotProduct(v, v));

	if (length)
	{
		ilength /= length;
		out[0] = v[0] * ilength;
		out[1] = v[1] * ilength;
		out[2] = v[2] * ilength;
	}
		
	return length;
}

void LocalToWorldVect(const vec3_t angles,const vec3_t in, vec3_t out)
{
	Matrix4 m;
	m.Rotate(-angles[2],-angles[0],angles[1]);
	m.XFormVect(*(Vect3 *)out,*(const Vect3 *)in);
}

void AnglesFromDir(vec3_t direction, vec3_t angles)
{
	angles[YAW] = atan2(direction[1], direction[0]);
	angles[PITCH] = -atan2(direction[2], sqrt(direction[0]*direction[0] + direction[1]*direction[1]));
	angles[ROLL] = 0;
}

void vectoangles(vec3_t in, vec3_t out)
{
//	vec3_t	temp;

//	VectorNormalize2(in, temp);
	AnglesFromDir(in, out);
	VectorRadiansToDegrees(out, out);
}


char *G_CopyString (char *in)
{
	char	*out;
	
	out = (char*)gi.TagMalloc (strlen(in)+1, TAG_LEVEL);
	strcpy (out, in);
	return out;
}


void G_InitEdict (edict_t *e)
{
	e->inuse = true;
	e->classname = "noclass";
	e->s.number = e - g_edicts;

	e->gravity = 1.0;
	e->friction = 1.0;
	e->airresistance = 1.0;
	e->bouyancy = 0.0;
	e->elasticity = 0.5;

//	rjr	e->rendermodel = NULL;
	e->s.renderindex = -1;

	e->ghoulInst= 0;

	// reset some CTF variables
    e->count = e->ctf_flags = e->ctf_hurt_carrier = 0;

	// set these prev fields to any silly values that won't match. There are no sensible #defines to use, so...
	//		
	VectorSet(e->prev_mins,   1000, 1000, 1000);
	VectorSet(e->prev_maxs,  -1000,-1000,-1000);
	VectorSet(e->prev_origin,10000,10000,10000);
	e->prev_solid = SOLID_NOMATCH;
}

/*
=================
G_Spawn

Either finds a free edict, or allocates a new one.
Try to avoid reusing an entity that was recently freed, because it
can cause the client to think the entity morphed into something else
instead of being removed and recreated, which can cause interpolated
angles and bad trails.
=================
*/
edict_t *G_Spawn (void)
{
	int			i;
	edict_t		*e;

	e = &g_edicts[(int)maxclients->value + 1];
	for (i = (int)maxclients->value + 1; i < globals.num_edicts; i++, e++)
	{
		// the first couple seconds of server time can involve a lot of
		// freeing and allocating, so relax the replacement policy
		if (!e->inuse && (e->freetime < 2 || ((level.time - e->freetime) > 0.5)))
		{
			G_InitEdict (e);
			return(e);
		}
	}
	
	if (i == game.maxentities)
	{
		gi.error ("ED_Alloc: no free edicts");
	}
	globals.num_edicts++;
	G_InitEdict (e);
	return e;
}

/*
=================
FreeEdictBoltData

deallocate any model-specific data we stored in the edict_t 
=================
*/
void FreeEdictBoltData(edict_t *ed)
{
	if (ed->objInfo)
	{
		delete ed->objInfo;
		ed->objInfo = NULL;
	}
}

/*
=================
G_FreeEdict

Marks the edict as free
=================
*/
void G_FreeEdict (edict_t *ed)
{
	// if this was a pickup of some sort, it might have been registered with thePickupList
	if (ed->flags & FL_PICKUP)
	{
		thePickupList.Unregister(ed);
	}

	// if this edict is a complex model of some sort we need to get rid of its
	//bolton-related info
	FreeEdictBoltData(ed);

	game_ghoul.RemoveObjectInstances(ed);
/*
	if (ed->ghoulInst)
	{
		ed->ghoulInst->Destroy();
		ed->ghoulInst=0;
	}
*/
	gi.unlinkentity (ed);		// unlink from world

	if (ed->ai)
	{
		ed->ai.Destroy();
	}

	PHYS_ClearAttachList(ed);

	if ((ed - g_edicts) <= ((int)maxclients->value + BODY_QUEUE_SIZE))
	{
//		gi.dprintf("tried to free special edict\n");
		ed->s.renderfx=0;
		return;
	}

	memset (ed, 0, sizeof(*ed));
	ed->classname = "freed";
	ed->freetime = level.time;
	ed->inuse = false;
}

void G_Explode(edict_t *ent, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
//	Com_Printf("The explosion is being triggered now.\n");
	ent->nextthink = level.time + .2;
	ent->s.effects |= EF_EXPLODING;
	ent->think = G_FreeEdict;
}

void G_FreeAllEdicts()
{
	edict_t	*ent;
	int		i;

	ent = g_edicts;
	for(i = 0; i < game.maxentities; i++, ent++)
	{
		if(ent->inuse)
		{
			G_FreeEdict(ent);
		}
	}
	memset (g_edicts, 0, game.maxentities * sizeof (g_edicts[0]));

	// This is just being ultra safe - the above should have cleared out everything
	game_ghoul.LevelCleanUp();
}

/*
============
G_TouchTriggers

============
*/
void	G_TouchTriggers (edict_t *ent)
{
	int			i, num;
	edict_t		*touch[MAX_EDICTS], *hit;

	// dead things don't activate triggers!
	if ((ent->client || (ent->svflags & SVF_MONSTER)) && (ent->health <= 0))
		return;

	num = gi.BoxEdicts (ent->absmin, ent->absmax, touch
		, MAX_EDICTS, AREA_TRIGGERS);

	// be careful, it is possible to have an entity in this
	// list removed before we get to it (killtriggered)
	for (i=0 ; i<num ; i++)
	{
		hit = touch[i];
		if (!hit->inuse)
			continue;
		if (!hit->touch)
			continue;
		hit->touch (hit, ent, NULL, NULL);
	}
}

/*
============
G_TouchSolids

Call after linking a new trigger in during gameplay
to force all entities it covers to immediately touch it
============
*/
void	G_TouchSolids (edict_t *ent)
{
	int			i, num;
	edict_t		*touch[MAX_EDICTS], *hit;

	num = gi.BoxEdicts (ent->absmin, ent->absmax, touch
		, MAX_EDICTS, AREA_SOLID);

	// be careful, it is possible to have an entity in this
	// list removed before we get to it (killtriggered)
	for (i=0 ; i<num ; i++)
	{
		hit = touch[i];
		if (!hit->inuse)
			continue;
		if (ent->touch)
			ent->touch (hit, ent, NULL, NULL);
		if (!ent->inuse)
			break;
	}
}




/*
==============================================================================

Kill box

==============================================================================
*/

/*
=================
KillBox

Kills all entities that would touch the proposed new positioning
of ent.  Ent should be unlinked before calling this!
=================
*/
qboolean KillBox (edict_t *ent)
{
	trace_t		tr;

	while (1)
	{
		gi.trace (ent->s.origin, ent->mins, ent->maxs, ent->s.origin, NULL, MASK_PLAYERSOLID, &tr);
		if (!tr.ent)
			break;

		// nail it
		T_Damage (tr.ent, ent, ent, vec3_origin, ent->s.origin, ent->s.origin, 100000, 0, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);

		// if we didn't kill it, fail
		if (tr.ent->solid)
			return false;
	}

	return true;		// all clear
}

int GetGhoulPosDir(vec3_t sourcePos, vec3_t sourceAng, IGhoulInst *inst,
					   GhoulID partID, char *name, vec3_t pos, vec3_t dir, vec3_t right, vec3_t up)
{
	//what the hell?  This doesn't seem to work correctly for bolt-ons ;(
	//perhaps I'm doing something wrong?  I'm quite good at that...

	IGhoulObj *obj = inst->GetGhoulObject();

	if(obj)
	{
		GhoulID part = (partID) ? partID:obj->FindPart(name);

		if(part)
		{
			Matrix4 EntityToWorld;
			EntToWorldMatrix(sourcePos,sourceAng,EntityToWorld);

			Matrix4 BoltToWorld;
			Matrix4 BoltToEntity;

			inst->GetBoltMatrix(level.time, BoltToEntity, part, IGhoulInst::MatrixType::Entity);
			BoltToWorld.Concat(BoltToEntity,EntityToWorld);

			Vect3 ChunkLoc;
			BoltToWorld.GetRow(3,ChunkLoc);

			if (pos)
				VectorCopy((float *)&ChunkLoc, pos);

			if (dir)
				BoltToWorld.GetRow(2,*(Vect3 *)dir);

			if(right)
			{
				BoltToWorld.GetRow(1,*(Vect3 *)right);
			}
			if(up)
			{
				BoltToWorld.GetRow(0,*(Vect3 *)up);
			}
			return 1;
		}
	}

	if (pos)
		VectorClear(pos);
	if (dir)
		VectorClear(dir);
	return 0;
}

void EntToWorldMatrix(vec3_t org, vec3_t angles, Matrix4 &m)
{
	vec3_t	fwd, right, up;

	AngleVectors(angles, fwd, right, up);
	m.Identity();
	(*(Vect3 *)right)*=-1.0f;
	m.SetRow(0,*(Vect3 *)fwd);
	m.SetRow(1,*(Vect3 *)right);
	m.SetRow(2,*(Vect3 *)up);
	m.SetRow(3,*(Vect3 *)org);
	m.CalcFlags();
}

void G_UpdateFrameEffects(edict_t *self)
{
	if (!(self->takedamage))
	{
		return;
	}

	// Check for burning effects, but ONLY on dead people!
	if (self->burntime > level.time && self->health <= 0)
	{
		// Only every .3 seconds.
		if ((int)(level.time * 10.0)%3 == 0)
		{
			IGhoulInst *inst = self->ghoulInst;
			float r, g, b, a;

			if (self->burninflictor == NULL)
				self->burninflictor = world;
			T_RadiusDamage (self, self->burninflictor, 10, self, 80, MOD_FIRE, DAMAGE_NO_ARMOR);

			if (inst)
			{
				inst->GetTint(&r, &g, &b, &a);
				if (r>0.2 || g>0.2 || b>0.2)
				{
					r-=.05;
					g-=.05;
					b-=.05;
					inst->SetTintOnAll(r, g, b, a);
				}
			}
		}
	}

	// Check for white phosphorous slow burn
	if (self->phosburntime > level.time)
	{
		// Only every .3 seconds, and only if dead.
		if (((int)(level.time * 10.0) % 3) == 0)
		{
			if (self->health <= 0)
			{
				IGhoulInst *inst = self->ghoulInst;
				float r, g, b, a;

				if (inst)
				{
					inst->GetTint(&r, &g, &b, &a);
					if (r>0.2 || g>0.2 || b>0.2)
					{
						r-=.05;
						g-=.05;
						b-=.05;
						inst->SetTintOnAll(r, g, b, a);
					}
				}
			}

			if (self->burninflictor == NULL)
				self->burninflictor = world;
			// Also apply some nasty slow damage to the mix.
			T_Damage(self, self, self->burninflictor, vec3_up, self->s.origin, self->s.origin, 
					(self->phosburntime - level.time)*5.0, 0, (int)DAMAGE_NO_ARMOR, (int)MOD_FIRE, 0, 0);
		}
	}
}


void Ignite(edict_t *target, edict_t *damager, float dmgAmount)
{
	assert(target);		// we got a crash reported here, so I'll make it not crash in release...
	assert(damager);	// If anyone gets stuck here, please tell me(Nathan)

	if (lock_deaths)
	{	// Adult lockout
		return;
	}

	// kef -- I'm sorry to have to do this, but Sabre needs to be invincible if his count isn't 100
	// dpk -- woo hoo, Dekker too!
	bool	bSabre = target->classname && (0 == strcmp("m_x_mskinboss", target->classname));
	bool	bDekker = target->classname && ((0 == strcmp("m_x_mraiderboss1", target->classname)) ||
		(0 == strcmp("m_x_mraiderboss2", target->classname)));

	if ((bSabre || bDekker) && (target->count != 100))
	{
		return;
	}
	
	if ((target->takedamage == DAMAGE_NO) || (target->flags & FL_GODMODE)) 
	{	//no burning of dead stuff?  I dunno.
		return;
	}

	if (target->burntime <= level.time + 2.0 && target->health <= 0)	// Only if dead.
	{
		if(target->ghoulInst)
		{
			fxRunner.exec("environ/onfireburst", target, 0);
		}

		target->burntime = level.time + 4.0;
	}
	else
	{
		if(target->ghoulInst)
		{
			fxRunner.exec("environ/quickfireburst", target, 0);
		}
	}
	target->burninflictor = damager;
}


void Electrocute(edict_t *target, edict_t *damager)
{
	assert(target);		// we got a crash reported here, so I'll make it not crash in release...
	assert(damager);	// If anyone gets stuck here, please tell me(Nathan)


	// kef -- I'm sorry to have to do this, but Sabre needs to be invincible if his count isn't 100
	// dpk -- woo hoo, Dekker too!
	bool	bSabre = target->classname && (0 == strcmp("m_x_mskinboss", target->classname));
	bool	bDekker = target->classname && ((0 == strcmp("m_x_mraiderboss1", target->classname)) ||
		(0 == strcmp("m_x_mraiderboss2", target->classname)));

	if ((bSabre || bDekker) && (target->count != 100))
	{
		return;
	}
	
	if ((target->takedamage == DAMAGE_NO) || (target->flags & FL_GODMODE)) 
	{	//no burning of dead stuff?  I dunno.
		return;
	}

	if(target->health <= 0)
	{	// char dead folks
		if (!lock_deaths)
		{
			if (target->burntime <= level.time)
			{
				if(target->ghoulInst)
				{
					fxRunner.exec("environ/onfireburst", target, 0);
				}
				else
				{
					fxRunner.exec("environ/onfireburst", target);
				}
				target->burntime = level.time + 2.0;
			}
		}
	}
	else if (target->zapfxtime <= level.time)
	{
		if(target->ghoulInst)
		{
			fxRunner.exec("weapons/world/mpgzap", target, 0);
		}
		else
		{
			fxRunner.exec("weapons/world/mpgzap", target);
		}
		target->burntime = level.time + 1.0;
	}
	target->burninflictor = damager;

	// Blind, shake target.
	if (target->client)
	{	// Was a player!
		target->client->blinding_alpha += 0.3;
		if (target->client->blinding_alpha > 0.9)
			target->client->blinding_alpha = 0.9;
		
		if (target->client->blinding_alpha_delta <= 0 || target->client->blinding_alpha_delta > 0.2)
		{
			target->client->blinding_alpha_delta = 0.2;
		}

		FX_SetEvent_Data(target, EV_CAMERA_SHAKE_VERYHEAVY, DEFAULT_JITTER_DELTA);
	}
	else if (target->ai)
	{	// Tis a monster!
		target->ai->MuteSenses(sight_mask, 25, smute_recov_linear, 25);
	}		
}



void RadiusBurn(edict_t *source, float radius)
{
	edict_t *target = NULL;
	trace_t los;

	CRadiusContent rad(source->s.origin, radius);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		target = rad.foundEdict(i);

		gi.trace(source->s.origin, vec3_origin, vec3_origin, target->s.origin, source, MASK_SOLID, &los);
		if (los.fraction > 0.99)
		{
			Ignite(target, source, 3);
		}
	}
}

// this requires 3 sqrts :(
int pointLineIntersect(vec3_t start, vec3_t end, vec3_t point, float rad)
{
	vec3_t	line1, line2, line3;
	float	len;
	float	cosVal;
	float	sinVal;
	float	minDistToLine;

	VectorSubtract(end, start, line1);
	VectorSubtract(point, start, line2);
	VectorSubtract(point, end, line3);

	// prelim quick tests to see if we're 'tween these two planes
	if(DotProduct(line1, line2) < 0)
	{
		return 0;
	}

	if(DotProduct(line3, line1) > 0)
	{
		return 0;
	}

	VectorNormalize(line1);
	len = VectorNormalize(line2);

	cosVal = DotProduct(line1, line2);

	sinVal = sqrt(1 - (cosVal * cosVal));

	minDistToLine = len * sinVal;

	if(minDistToLine < rad)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
