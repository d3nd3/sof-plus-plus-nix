// g_phys.c

#include "g_local.h"
#include "..\qcommon\ef_flags.h"
#include "ai_private.h"

#define	STOP_EPSILON	1.0
#define ZSTOP_EPSILON	100.0 // this is bigger to stop stupid vertical oscillations when things

typedef struct
{
	edict_t	*ent;
	vec3_t	origin;
	vec3_t	angles;
	float	deltayaw;
} pushed_t;
pushed_t	pushed[MAX_EDICTS], *pushed_p;
void SV_CheckGround (edict_t *ent);
void G_UpdateFrameEffects(edict_t *ent);
extern void debug_drawbox(edict_t* self,vec3_t vOrigin, vec3_t vMins, vec3_t vMaxs, int nColor);

								// bounce close to the ground
/*


pushmove objects do not obey gravity, and do not interact with each other or trigger 
fields, but block normal movement and push normal objects when they move.

onground is set for toss objects when they come to a complete rest.  
it is set for steping or walking objects 

doors, plats, etc are SOLID_BSP, and MOVETYPE_PUSH
bonus items are SOLID_TRIGGER touch, and MOVETYPE_TOSS
corpses are SOLID_NOT and MOVETYPE_TOSS
crates are SOLID_BBOX and MOVETYPE_TOSS
walking monsters are SOLID_SLIDEBOX and MOVETYPE_STEP
flying/floating monsters are SOLID_SLIDEBOX and MOVETYPE_FLY

solid_edge items only clip against bsp models.

*/
void SV_Physics_Step (edict_t *ent);
void SV_HandleGenericGroundEffect(edict_t *ent);

/*
============
SV_TestEntityPosition

============
*/
edict_t	*SV_TestEntityPosition (edict_t *ent)
{
	trace_t	trace;
	int		mask;

	if (ent->clipmask)
		mask = ent->clipmask;
	else
		mask = MASK_SOLID;
	gi.trace (ent->s.origin, ent->mins, ent->maxs, ent->s.origin, ent, mask, &trace);
	
	if (trace.startsolid)
		return g_edicts;
		
	return NULL;
}


/*
================
SV_CheckVelocity
================
*/
void SV_CheckVelocity (edict_t *ent)
{
	int		i;

//
// bound velocity
//
	for (i=0 ; i<3 ; i++)
	{
		if (ent->velocity[i] > sv_maxvelocity->value)
			ent->velocity[i] = sv_maxvelocity->value;
		else if (ent->velocity[i] < -sv_maxvelocity->value)
			ent->velocity[i] = -sv_maxvelocity->value;
	}
}

void Lean_TouchTriggers(edict_t *ent)
{
	edict_t	*playerEnt = GetBuddyOwner(ent);
	vec3_t	fwd, right, up;

	AngleVectors(playerEnt->client->ps.viewangles, fwd, right, up);
	VectorMA(playerEnt->s.origin, -30 * playerEnt->viewside, right, ent->s.origin);
	VectorAdd(ent->s.origin, ent->mins, ent->absmin);
	VectorAdd(ent->s.origin, ent->maxs, ent->absmax);
	G_TouchTriggers(playerEnt);
	VectorClear(ent->s.origin);
}

/*
=============
SV_RunThink

Runs thinking code for this frame if necessary
=============
*/
qboolean SV_RunThink (edict_t *ent)
{
	float	thinktime;

	thinktime = ent->nextthink;
	if (thinktime <= 0 || (!ent->think && !ent->ai))
	{
		if ((ent->flags & FL_LEAN_PLAYER) && (GetBuddyOwner(ent)))
		{	// this is a leanbuddy
			Lean_TouchTriggers(ent);
		}
		return true;
	}
	if (thinktime > level.time+0.001)
	{
		return true;
	}
	
	ent->nextthink = 0;
	if (ent->ai)
	{
		//need to set think to NULL in order for ai to do anything
		if (ent->ai->IsFirstFrameTime())
		{
			ent->ai->FirstFrame(ent);
			ent->think(ent);
		}
		else
		{
			ent->ai->Think(*ent);
			G_TouchTriggers(ent);
		}
	}
	else
	{
// rjr shouldn't be able to occur
//		if (!ent->think)
//			gi.error ("NULL ent->think for item '%s'", ent->classname);
		ent->think (ent);
	}

	// find this ghosting bug.
	assert(!((ent->s.renderfx & RF_GHOUL) && !(ent->ghoulInst)));
	return false;
}

/*
==================
SV_Impact

Two entities have touched, so run their touch functions
==================
*/
int attachmentNotCircular(edict_t *e1)
{
	edict_t *next;

	next = e1;

	while(next->attachChain)
	{
		next = &g_edicts[next->attachChain];
		if(next == e1)return 0;
	}
	return 1;
}

//moves all general world objects that are attached to a guy... but if a guy is of type push, he might have to be returned to a prior position
void updateAttachments(edict_t *e, int isTypePush, vec3_t move, vec3_t amove)
{
	edict_t *curChain;
	vec3_t	preRotate;
	vec3_t	postRotate;
	int		depth = 0;

	if(!(e->attachChain))return;

	curChain = e;
	while(curChain->attachChain)
	{
		curChain = &g_edicts[curChain->attachChain];

		if(isTypePush)
		{
			pushed_p->ent = curChain;
			VectorCopy(curChain->s.origin, pushed_p->origin);
			VectorCopy(curChain->s.angles, pushed_p->angles);
			pushed_p++;
		}

		VectorAdd(curChain->s.origin, move, curChain->s.origin);
		VectorAdd(curChain->s.angles, amove, curChain->s.angles);

		if(amove[YAW])//need to handle other rotations - FIXME
		{	// angular stuff :/

			VectorSubtract(curChain->s.origin, e->s.origin, preRotate);

			postRotate[0] = cos(-amove[YAW] * M_PI / 180.0) * preRotate[0] + sin(-amove[YAW] * M_PI / 180.0) * preRotate[1];
			postRotate[1] = -sin(-amove[YAW] * M_PI / 180.0) * preRotate[0] + cos(-amove[YAW] * M_PI / 180.0) * preRotate[1];
			postRotate[2] = preRotate[2];

			VectorAdd(e->s.origin, postRotate, curChain->s.origin);
		}

		depth++;
		assert(depth < 10);//circular list?  ACK!!!
	}
}

bool SV_Impact (edict_t *e1, trace_t *trace)
{
	edict_t		*e2;

	e2 = trace->ent;

	// 1/11/99 kef -- we shouldn't be able to shoot our own LeanBuddy, NugBuddy, or WeaponBuddy
	if (e2->svflags & SVF_BUDDY)
	{
		if(e2->owner == e1->owner)
		{	// an impact with our own buddy
			return false;
		}
		else if(e1->health == 31337)
		{
			// a knife hit a buddy
		}
		else
		{	// I don't know of a pretty way to fix this off hand, but basically we don't want you
			//to be able to land a c4 or a thrown knife on a buddy
			if (e1->flags & FL_THROWN_WPN)
			{	// hit something like a weaponbuddy, so bounce off of it
				if (trace->plane.normal && (DotProduct(trace->plane.normal, vec3_up) > .8) )
				{	// landed on a buddy
					e1->velocity[2] *= -1;
					VectorScale(e1->velocity, .4, e1->velocity);
				}
				else
				{
					VectorScale(e1->velocity, -.2, e1->velocity);
				}

				if (e1->touch && (e1->solid != SOLID_NOT || e1->movetype == MOVETYPE_TOUCHNOTSOLID))
					e1->touch (e1, e2, &trace->plane, trace->surface);
		
				return false;
			}
		}
	}
	// If entity1 is a MOVETYPE_TOUCHNOTSOLID, like a missile, 
	// then it should run its touch function, solid or not.
	if (e1->touch && (e1->solid != SOLID_NOT || e1->movetype == MOVETYPE_TOUCHNOTSOLID))
		e1->touch (e1, e2, &trace->plane, trace->surface);
	
	if (e2->touch && e2->solid != SOLID_NOT)
		e2->touch (e2, e1, NULL, NULL);

	return true;
}


/*
==================
ClipVelocity

Slide off of the impacting object
returns the blocked flags (1 = floor, 2 = step / wall)
==================
*/

int ClipVelocity (vec3_t in, vec3_t normal, vec3_t out, float overbounce)
{
	float	backoff;
	float	change;
	int		i, blocked;
	
	blocked = 0;
	if (normal[2] > 0)
		blocked |= 1;		// floor
	if (!normal[2])
		blocked |= 2;		// step
	
	backoff = DotProduct (in, normal) * overbounce;

	if (backoff == 0)//if plane is parallel to velocity, physics are screwy, so bounce off with equal weight given to plane & vel
	{
		VectorScale(in,0.5,in);
		backoff = VectorLength(in)*overbounce;
	}

	for (i=0 ; i<3 ; i++)
	{
		change = normal[i]*backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0;
	}
	
	return blocked;
}

void ClipVelocity_Dan (vec3_t in, vec3_t normal, vec3_t out, float elasticity)
// Calculate the velocity resulting from an impact with a plane surface.
// ************************************************************************************************
{
	float	DotProd;
	vec3_t	InverseIn,
			WorkVec;
	int		i;

	// Calculation for reflection in a plane is 2*(N.D)*N-D, where D is the inverse of the incident
	// ray.

	if (elasticity == -1)
	{
		VectorCopy(in, out);
		return;
	}

	VectorScale(in,-1.0 * elasticity,InverseIn);

	DotProd=DotProduct(normal,InverseIn);

	VectorScale(normal,DotProd*1.8,WorkVec); // not using 2.0 because I need to fudge
	
	VectorSubtract(WorkVec,InverseIn,out);

	for (i=0 ; i<2 ; i++)
	{
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
		{
			out[i] = 0;
		}
	}
	if (out[2] > -ZSTOP_EPSILON && out[2] < ZSTOP_EPSILON)
	{
		out[2] = 0;
	}

}
	

/*
============
SV_FlyMove

The basic solid body movement clip that slides along multiple planes
Returns the clipflags if the velocity was modified (hit something solid)
1 = floor
2 = wall / step
4 = dead stop
============
*/
#define	MAX_CLIP_PLANES	5
int SV_FlyMove (edict_t *ent, float time, int mask)
{
	edict_t		*hit;
	int			bumpcount, numbumps;
	vec3_t		dir;
	float		d;
	int			numplanes;
	vec3_t		planes[MAX_CLIP_PLANES];
	vec3_t		primal_velocity, original_velocity, new_velocity;
	int			i, j;
	trace_t		trace;
	vec3_t		end;
	float		time_left;
	int			blocked;

	// corpse stuff
	vec3_t		mins, maxs;
	// end corpse stuff

	
	numbumps = 4;
	
	blocked = 0;
	VectorCopy (ent->velocity, original_velocity);
	VectorCopy (ent->velocity, primal_velocity);
	numplanes = 0;
	
	time_left = time;

	ent->groundentity = NULL;
	for (bumpcount=0 ; bumpcount<numbumps ; bumpcount++)
	{
		for (i=0 ; i<3 ; i++)
			end[i] = ent->s.origin[i] + time_left * ent->velocity[i];
// trace isn't picking up models as entities!

		//corpse stuff
		if (ent->solid == SOLID_CORPSE)
		{
			VectorCopy(ent->mins, mins);
			VectorCopy(ent->maxs, maxs);
			mins[0] = -8;
			mins[1] = -8;
			maxs[0] = 8;
			maxs[1] = 8;
			maxs[2] = 8;
			gi.trace (ent->s.origin, mins, maxs, end, ent, mask, &trace);
		}
		// end corpse stuff
		else
		{
			gi.trace (ent->s.origin, ent->mins, ent->maxs, end, ent, mask, &trace);
		}

//		Com_Printf("%s %d Tracing from (%f,%f,%f) to (%f,%f,%f)\n", ent->classname, ent->s.number, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2],
//			end[0], end[1], end[2]);

		

		if (trace.allsolid)
		{	// entity is trapped in another solid
			VectorCopy (vec3_origin, ent->velocity);
			return 3;
		}

		hit = trace.ent;

/*		if (trace.fraction != 1)
		{
			Com_Printf("Stopped at (%f,%f,%f)\n", trace.endpos[0], trace.endpos[1], trace.endpos[2]);
		}
		Com_Printf("%s %d Trace hit entity %s %d \n", ent->classname, ent->s.number, hit->classname, hit->s.number); 
*/

		{//test only--did last trace take me into a solid?!!!!!!!!!
//			static int i98=0;
//			Com_Printf("Horrible extra trace in FLYMOVE (%d)\n",i98++);
			trace_t dbtrace;

			//corpse stuff
			if (ent->solid == SOLID_CORPSE)
			{
				gi.trace (trace.endpos, mins, maxs, trace.endpos, ent, mask, &dbtrace);
			}
			//end corpse stuff
			else
			{
				gi.trace (trace.endpos, ent->mins, ent->maxs, trace.endpos, ent, mask, &dbtrace);
			}
			if (dbtrace.startsolid || dbtrace.allsolid)
			{
				if (trace.fraction > 0 && trace.fraction < 1 && trace.plane.normal[2] > 0.7)
				{
					if (( hit->solid == SOLID_BSP) || (hit->solid == SOLID_GHOUL))
					{
						ent->groundentity = trace.ent;
						ent->groundentity_linkcount = trace.ent->linkcount;
					}
				}
				VectorNormalize (ent->velocity);
				VectorScale (ent->velocity, -1, ent->velocity);//back outta here
				for (i=0;i<3;i++)
					ent->velocity[i] += gi.flrand(-1.0F, 1.0F);
				return 7;//trace failed!!!!! we're FUKD!!!!!!!
			}
		}


		if (trace.fraction > 0)
		{	// actually covered some distance
			VectorCopy (trace.endpos, ent->s.origin);
			VectorCopy (ent->velocity, original_velocity);
			numplanes = 0;
		}

		if (trace.fraction == 1)
			 break;		// moved the entire distance


		if (trace.plane.normal[2] > 0.7)
		{
			blocked |= 1;		// floor
			if (( hit->solid == SOLID_BSP) || (hit->solid == SOLID_GHOUL))
			{
				ent->groundentity = hit;
				ent->groundentity_linkcount = hit->linkcount;
				SV_HandleGenericGroundEffect(ent);
			}
		}
		if (!trace.plane.normal[2])
		{
			blocked |= 2;		// step
		}

//
// run the impact function
//
		if (!SV_Impact (ent, &trace))
		{
			break;
		}
		if (!ent->inuse)
			break;		// removed by the impact function

		
		time_left -= time_left * trace.fraction;
		
	// cliped to another plane
		if (numplanes >= MAX_CLIP_PLANES)
		{	// this shouldn't really happen
			VectorCopy (vec3_origin, ent->velocity);
			return 3;
		}

		VectorCopy (trace.plane.normal, planes[numplanes]);
		numplanes++;

//
// modify original_velocity so it parallels all of the clip planes
//
		if (ent->movetype != MOVETYPE_NOCLIP)
		{

			for (i=0 ; i<numplanes ; i++) 
			{
				if (!trace.fraction)
					ClipVelocity (original_velocity, planes[i], new_velocity, 1.1);//a little extra bounce to pop me out of any corners i might get stuck in
				else
					ClipVelocity (original_velocity, planes[i], new_velocity, 1);//a little extra bounce to pop me out of any corners i might get stuck in

				for (j=0 ; j<numplanes ; j++)
					if (j != i)
					{
						if (DotProduct (new_velocity, planes[j]) <= 0)//parallel or against vel n/ ok
							break;	// not ok
					}
				if (j == numplanes)
					break;
			}
			
			if (i != numplanes)
			{	// go along this plane
				VectorCopy (new_velocity, ent->velocity);
			}
			else
			{	// go along the crease
				if (numplanes != 2)
				{
	//				gi.dprintf ("clip velocity, numplanes == %i\n",numplanes);
					VectorCopy (vec3_origin, ent->velocity);
					return 7;
				}
				CrossProduct (planes[0], planes[1], dir);
				d = DotProduct (dir, ent->velocity);
				VectorScale (dir, d, ent->velocity);
			}
		}
//
// if original velocity is against the original velocity, stop dead
// to avoid tiny occilations in sloping corners
//
/*		if (DotProduct (ent->velocity, primal_velocity) < 0)//don't zero me out if plane is parallel to vel
		{
			VectorCopy (vec3_origin, ent->velocity);
			return blocked;
		}*/
//  ^^ this is crap.  essentially, it says, if something is going to bounce back the way it came,
//  just stop it...   I think the "tiny occillations" were vertical, which could be gotten rid
// of with:
		if ((ent->velocity[2] * primal_velocity[2]) < 0)
		{
			ent->velocity[2] = 0;
			return blocked;
		}
		

	}

	return blocked;
}


/*
============
SV_AddGravity

============
*/
void SV_AddGravity (edict_t *ent)
{
	vec3_t		gravdir;

	gravdir[0] = sv_gravityx->value;
	gravdir[1] = sv_gravityy->value;
	gravdir[2] = sv_gravityz->value; // note that positive z is downward
//	VectorNormalize(gravdir);

	ent->velocity[0] += ent->gravity * sv_gravity->value * FRAMETIME * gravdir[0];
	ent->velocity[1] += ent->gravity * sv_gravity->value * FRAMETIME * gravdir[1];
	ent->velocity[2] -= ent->gravity * sv_gravity->value * FRAMETIME * gravdir[2];
	if (ent->waterlevel == 1)
	{
		ent->velocity[2] += ent->bouyancy * sv_gravity->value * FRAMETIME;
	}
}

/*
===============================================================================

PUSHMOVE

===============================================================================
*/

/*
============
SV_PushEntity

Does not change the entities velocity at all
============
*/
trace_t SV_PushEntity (edict_t *ent, vec3_t push)
{
	trace_t	trace;
	vec3_t	start;
	vec3_t	end;
	int		mask;

	VectorCopy (ent->s.origin, start);
	VectorAdd (start, push, end);

retry:
	if (ent->clipmask)
		mask = ent->clipmask;
	else
		mask = MASK_SOLID;

	gi.trace (start, ent->mins, ent->maxs, end, ent, mask, &trace);
	
	// If we start in an object, we shouldn't move.  We need this mainly for corpses, so to be safe, we only use it on MOVETYPE_TOSS
	if (trace.allsolid && ent->movetype == MOVETYPE_TOSS)
	{	// Don't move at all
		gi.linkentity (ent);
	}
	else
	{
		VectorCopy (trace.endpos, ent->s.origin);

		gi.linkentity (ent);

		if (trace.fraction != 1.0) // it's gonna hit something
		{
			SV_Impact (ent, &trace);

			// if the pushed entity went away and the pusher is still there
			if (!trace.ent->inuse && ent->inuse)
			{
				// move the pusher back and try again
				VectorCopy (start, ent->s.origin);
				gi.linkentity (ent);
				goto retry;
			}
		}
	}

	if (ent->inuse)
		G_TouchTriggers (ent);

	return trace;
}					

edict_t	*obstacle;

/*
============
SV_Push

Objects need to be moved back on a failed push,
otherwise riders would continue to slide.
============
*/
qboolean SV_Push (edict_t *pusher, vec3_t move, vec3_t amove)
{
	int			i, e;
	edict_t		*check, *block;
	vec3_t		mins, maxs;
	pushed_t	*p;
	vec3_t		org, org2, move2, forward, right, up;

	// clamp the move to 1/8 units, so the position will
	// be accurate for client side prediction
/*
	for (i=0 ; i<3 ; i++)
	{
		float	temp;
		temp = move[i]*8.0;
		if (temp > 0.0)
			temp += 0.5;
		else
			temp -= 0.5;
		move[i] = 0.125 * (int)temp;
	}
*/

	// find the bounding box
	for (i=0 ; i<3 ; i++)
	{
		mins[i] = pusher->absmin[i] + move[i];
		maxs[i] = pusher->absmax[i] + move[i];
	}

// we need this for pushing things later
	VectorSubtract (vec3_origin, amove, org);
	AngleVectors (org, forward, right, up);

// save the pusher's original position
	pushed_p->ent = pusher;
	VectorCopy (pusher->s.origin, pushed_p->origin);
	VectorCopy (pusher->s.angles, pushed_p->angles);
	if (pusher->client)
		pushed_p->deltayaw = pusher->client->ps.pmove.delta_angles[YAW];
	pushed_p++;

// move the pusher to it's final position
	VectorAdd (pusher->s.origin, move, pusher->s.origin);
	VectorAdd (pusher->s.angles, amove, pusher->s.angles);
	gi.linkentity (pusher);

	//FIXME - NOT WORTH BOTHERING WITH - JUST MAKE KNIVES AND WHAT NOT BOUNCE OFF OF DOORS
//	updateAttachments(pusher, 1, move, amove);

// see if any solid entities are inside the final position
	check = g_edicts+1;
	for (e = 1; e < globals.num_edicts; e++, check++)
	{
		if (!check->inuse)
			continue;
		if (check->movetype == MOVETYPE_PUSH
		|| check->movetype == MOVETYPE_STOP
		|| check->movetype == MOVETYPE_NONE
		|| check->movetype == MOVETYPE_NOCLIP
		|| check->s.solid == SOLID_NOT)
			continue;

		if(check->attachOwner)continue;//these guys are attached - let's leave 'em alone for now and let 'em rest...

		if (!check->area.prev)
			continue;		// not linked in anywhere

	// if the entity is standing on the pusher, it will definitely be moved
		if (check->groundentity != pusher)
		{
			// see if the ent needs to be tested
			if ( check->absmin[0] >= maxs[0]
			|| check->absmin[1] >= maxs[1]
			|| check->absmin[2] >= maxs[2]
			|| check->absmax[0] <= mins[0]
			|| check->absmax[1] <= mins[1]
			|| check->absmax[2] <= mins[2] )
				continue;

			//sfs--i put in this bit to fix the stuck trapdoors on the train, but until i can run a map with elevators on it, i can't test it and it shouldn't be in.
			//pushers should be able to go through dead monsters (WHO AREN'T STANDING ON THE PUSHER!)
//			if (check->svflags & SVF_DEADMONSTER)
//				continue;

			// see if the ent's bbox is inside the pusher's final position
			if (!SV_TestEntityPosition (check))
				continue;
		}

		if ((pusher->movetype == MOVETYPE_PUSH) || (check->groundentity == pusher))
		{
			// move this entity
			pushed_p->ent = check;
			VectorCopy (check->s.origin, pushed_p->origin);
			VectorCopy (check->s.angles, pushed_p->angles);
			pushed_p++;

			// try moving the contacted entity 
			VectorAdd (check->s.origin, move, check->s.origin);
			if (check->client)
			{	// FIXME: doesn't rotate monsters?
				check->client->ps.pmove.delta_angles[YAW] += amove[YAW];
			}

			// figure movement due to the pusher's amove
			VectorSubtract (check->s.origin, pusher->s.origin, org);
			org2[0] = DotProduct (org, forward);
			org2[1] = -DotProduct (org, right);
			org2[2] = DotProduct (org, up);
			VectorSubtract (org2, org, move2);
// rjr EmergencySnapPosition() took care of this
//			VectorScale(move2, 2.0, move2);
			VectorAdd (check->s.origin, move2, check->s.origin);

			// may have pushed them off an edge
			if (check->groundentity != pusher)
				check->groundentity = NULL;

			block = SV_TestEntityPosition (check);
			if (!block)
			{	// pushed ok
				gi.linkentity (check);
				// impact?
				continue;
			}

			// if it is ok to leave in the old position, do it
			// this is only relevent for riding entities, not pushed
			// FIXME: this doesn't acount for rotation
			VectorSubtract (check->s.origin, move, check->s.origin);
			block = SV_TestEntityPosition (check);
			if (!block)
			{
				pushed_p--;
				continue;
			}
		}
		
		// save off the obstacle so we can call the block function
		obstacle = check;

		// move back any entities we already moved
		// go backwards, so if the same entity was pushed
		// twice, it goes back to the original position
		for (p=pushed_p-1 ; p>=pushed ; p--)
		{
			VectorCopy (p->origin, p->ent->s.origin);
			VectorCopy (p->angles, p->ent->s.angles);
			if (p->ent->client)
			{
				p->ent->client->ps.pmove.delta_angles[YAW] = p->deltayaw;
			}
			gi.linkentity (p->ent);
		}
		return false;
	}

//FIXME: is there a better way to handle this?
	// see if anything we moved has touched a trigger
	for (p=pushed_p-1 ; p>=pushed ; p--)
		G_TouchTriggers (p->ent);

	return true;
}

/*
================
SV_Physics_Pusher

Bmodel objects don't interact with each other, but
push all box objects
================
*/
void SV_Physics_Pusher (edict_t *ent)
{
	vec3_t		move, amove;
	edict_t		*part, *mv;

	// if not a team captain, so movement will be handled elsewhere
	if ( ent->flags & FL_TEAMSLAVE)
		return;

	// make sure all team slaves can move before commiting
	// any moves or calling any think functions
	// if the move is blocked, all moved objects will be backed out
//retry:
	pushed_p = pushed;
	for (part = ent ; part ; part=part->teamchain)
	{
		if (part->velocity[0] || part->velocity[1] || part->velocity[2] ||
			part->avelocity[0] || part->avelocity[1] || part->avelocity[2]
			)
		{	// object is moving
			VectorScale (part->velocity, FRAMETIME, move);
			VectorScale (part->avelocity, FRAMETIME, amove);

			if (!SV_Push (part, move, amove))
				break;	// move was blocked
		}
	}
	if (pushed_p > &pushed[MAX_EDICTS])
		gi.error (ERR_FATAL, "pushed_p > &pushed[MAX_EDICTS], memory corrupted");

	if (part)
	{
		// the move failed, bump all nextthink times and back out moves
		for (mv = ent ; mv ; mv=mv->teamchain)
		{
			if (mv->nextthink > 0)
				mv->nextthink += FRAMETIME;
		}

		// if the pusher has a "blocked" function, call it
		// otherwise, just stay in place until the obstacle is gone
		if (part->blocked)
			part->blocked (part, obstacle);
#if 0
		// if the pushed entity went away and the pusher is still there
		if (!obstacle->inuse && part->inuse)
			goto retry;
#endif
	}
	else
	{
		// the move succeeded, so call all think functions
		for (part = ent ; part ; part=part->teamchain)
		{
			SV_RunThink (part);
		}
	}
}

//==================================================================

/*
=============
SV_Physics_None

Non moving objects can only think
=============
*/
void SV_Physics_None (edict_t *ent)
{
// regular thinking
	SV_RunThink (ent);
}

/*
=============
SV_Physics_Noclip

A moving object that doesn't obey physics
=============
*/
void SV_Physics_Noclip (edict_t *ent)
{

	VectorMA (ent->s.angles, FRAMETIME, ent->avelocity, ent->s.angles);
	VectorMA (ent->s.origin, FRAMETIME, ent->velocity, ent->s.origin);
	gi.linkentity (ent);	
	// regular thinking
	if (!SV_RunThink (ent))
		return;
	


}

/*
==============================================================================

TOSS / BOUNCE

==============================================================================
*/

/*
=============
SV_Physics_Toss

Toss, bounce, and fly movement.  When onground, do nothing.
=============
*/
void SV_Physics_Toss (edict_t *ent)
{
	trace_t		trace;
	vec3_t		move;
	float		backoff;
	edict_t		*slave;
	qboolean	wasinwater;
	qboolean	isinwater;
	vec3_t		old_origin;

// regular thinking
	SV_RunThink (ent);

	// if not a team captain, so movement will be handled elsewhere
	if ( ent->flags & FL_TEAMSLAVE)
		return;

	if (ent->velocity[2] > 0)
		ent->groundentity = NULL;

// check for the groundentity going away
	if (ent->groundentity)
		if (!ent->groundentity->inuse)
			ent->groundentity = NULL;

// if onground, return without moving
	if ( ent->groundentity )
		return;

	VectorCopy (ent->s.origin, old_origin);

	SV_CheckVelocity (ent);

// add gravity
	if (ent->movetype != MOVETYPE_FLY
	&& ent->movetype != MOVETYPE_FLYMISSILE)
		SV_AddGravity (ent);

// move angles
	VectorMA (ent->s.angles, FRAMETIME, ent->avelocity, ent->s.angles);

// move origin
	VectorScale (ent->velocity, FRAMETIME, move);
	trace = SV_PushEntity (ent, move);
	if (!ent->inuse)
		return;

	if (trace.fraction < 1) // if it will hit something
	{
		if (ent->movetype == MOVETYPE_BOUNCE)
			backoff = 1.5;
		else
			backoff = 1;

		ClipVelocity (ent->velocity, trace.plane.normal, ent->velocity, backoff);

	// stop if on ground
		if (trace.plane.normal[2] > 0.7)
		{		
			if (ent->velocity[2] < 30 || ent->movetype != MOVETYPE_BOUNCE )
//	fixme:	if (ent->velocity[2] < 60 || ent->movetype != MOVETYPE_BOUNCE )
//  this hardcoded 60 (or 30) is bad...
			{
				ent->groundentity = trace.ent;
				ent->groundentity_linkcount = trace.ent->linkcount;
				ent->movetype = MOVETYPE_STEP;
				ent->velocity[2] = 0;
				SV_Physics_Step (ent);
		//		VectorCopy (vec3_origin, ent->velocity);
		//		VectorCopy (vec3_origin, ent->avelocity);
			}
			SV_HandleGenericGroundEffect(ent);
		}

//		if (ent->touch)
//			ent->touch (ent, trace.ent, &trace.plane, trace.surface);
	}
	
// check for water transition
	wasinwater = (ent->watertype & MASK_WATER);
	ent->watertype = gi.pointcontents (ent->s.origin);
	isinwater = ent->watertype & MASK_WATER;

	if (isinwater)
		ent->waterlevel = 1;
	else
		ent->waterlevel = 0;

/*
	if (!wasinwater && isinwater)
	{
		gi.positioned_sound (old_origin, g_edicts, CHAN_AUTO, gi.soundindex("misc/h2ohit1.wav"), .6, 1, 0);
	}
	else if (wasinwater && !isinwater)
	{
		gi.positioned_sound (ent->s.origin, g_edicts, CHAN_AUTO, gi.soundindex("misc/h2ohit1.wav"), .6, 1, 0);
	}
*/

// move teamslaves
	for (slave = ent->teamchain; slave; slave = slave->teamchain)
	{
		VectorCopy (ent->s.origin, slave->s.origin);
		gi.linkentity (slave);
	}
}

/*
===============================================================================

STEPPING MOVEMENT

===============================================================================
*/

/*
=============
SV_Physics_Step

Monsters freefall when they don't have a ground entity, otherwise
all movement is done with discrete steps.

This is also used for objects that have become still on the ground, but
will fall if the floor is pulled out from under them.
FIXME: is this true?
=============
*/

#define sv_waterfriction	1

void SV_AddRotationalFriction (edict_t *ent, qboolean onground)
{
	int		n;
	float	adjustment;

	if (onground)
	{
		adjustment = FRAMETIME * ent->friction * FRICTION_MULT * .2;
	}
	else
	{
		adjustment = FRAMETIME * ent->airresistance * AIRRESISTANCE_MULT;
	}

	for (n = 0; n < 3; n++)
	{
		if (ent->avelocity[n] > 0)
		{
			ent->avelocity[n] -= adjustment;
			if (ent->avelocity[n] < 0)
				ent->avelocity[n] = 0;
		}
		else
		{
			ent->avelocity[n] += adjustment;
			if (ent->avelocity[n] > 0)
				ent->avelocity[n] = 0;
		}
	}
}

void SV_Rotate (edict_t *ent)
{
	int		i;


	
	for (i = 0; i < 3; i++)
	{
		if ((ent->avelocity[i] > -STOP_EPSILON) && (ent->avelocity[i] < STOP_EPSILON))
		{
			ent->avelocity[i] = 0;
		}
	}

	VectorMA (ent->s.angles, FRAMETIME, ent->avelocity, ent->s.angles);	

}

void SV_AddLinearFriction (edict_t *ent, qboolean onground)
{
	int		n;
	float	adjustment;

	if (onground)
	{
		adjustment = FRAMETIME * ent->friction * FRICTION_MULT;
	}
	else
	{
		adjustment = FRAMETIME * ent->airresistance * AIRRESISTANCE_MULT;
	} // fixme: make airresistance quadratic?

	for (n = 0; n < 3; n++)
	{
		if (ent->velocity[n] > 0)
		{
			ent->velocity[n] -= adjustment;
			if (ent->velocity[n] < 0)
				ent->velocity[n] = 0;
		}
		else
		{
			ent->velocity[n] += adjustment;
			if (ent->velocity[n] > 0)
				ent->velocity[n] = 0;
		}
	}
}

void SV_Physics_Step (edict_t *ent)
{
	qboolean	wasonground;
	qboolean	hitsound = false;
	float		*vel;
	float		speed, newspeed, control;
	float		friction;
	edict_t		*groundentity;
	int			mask;
	
	if (!ent->stopspeed)
	{
		ent->stopspeed = 100;
	}

	// airborn monsters should always check for ground
//	if (!ent->groundentity)
//		M_CheckGround (ent);

// corpse stuff
	if (ent->solid == SOLID_CORPSE)
	{
		SV_CheckGround(ent);
	}


	groundentity = ent->groundentity;

	SV_CheckVelocity (ent);

	if (groundentity)
		wasonground = true;
	else
		wasonground = false;
	
	//only do movement if i'm not an inactive monster...--ss
	if (!ent->ai || ent->ai->IsActive())
	{
		if (ent->avelocity[0] || ent->avelocity[1] || ent->avelocity[2])
		{
			SV_Rotate(ent);
			SV_AddRotationalFriction (ent, wasonground);
		}
		// add gravity except:
		//   flying monsters
		//   swimming monsters who are in the water
		if (! wasonground)
			if (!(ent->flags & FL_FLY))
				if (!((ent->flags & FL_SWIM) && (ent->waterlevel > 2)))
				{
					if (ent->velocity[2] < sv_gravity->value*-0.1)
						hitsound = true;
					if (ent->waterlevel == 0)
						SV_AddGravity (ent);
				}

		// friction for flying monsters that have been given vertical velocity
		if ((ent->flags & FL_FLY) && (ent->velocity[2] != 0))
		{
			speed = fabs(ent->velocity[2]);
			control = speed < ent->stopspeed ? ent->stopspeed : speed;
			friction = (ent->friction) * FRICTION_MULT/3;
			newspeed = speed - (FRAMETIME * control * friction);
			if (newspeed < 0)
				newspeed = 0;
			newspeed /= speed;
			ent->velocity[2] *= newspeed;
		}

		// friction for flying monsters that have been given vertical velocity
		if ((ent->flags & FL_SWIM) && (ent->velocity[2] != 0))
		{
			speed = fabs(ent->velocity[2]);
			control = speed < ent->stopspeed ? ent->stopspeed : speed;
			newspeed = speed - (FRAMETIME * control * sv_waterfriction * ent->waterlevel);
			if (newspeed < 0)
				newspeed = 0;
			newspeed /= speed;
			ent->velocity[2] *= newspeed;
		}

		if (ent->velocity[2] || ent->velocity[1] || ent->velocity[0])// || (ent->solid == SOLID_CORPSE))
		{
//			Com_Printf("%s %d inside Step Physics -- velocity not 0 \n", ent->classname, ent->s.number); 

			// apply friction
			// let dead monsters who aren't completely onground slide
			if ((wasonground) || (ent->flags & (FL_SWIM|FL_FLY)))
				if (!(ent->health <= 0.0 && !gmonster.CheckBottom(ent)))
				{
					vel = ent->velocity;
					speed = sqrt(vel[0]*vel[0] +vel[1]*vel[1]);
					if (speed)
					{
						friction = ent->friction * FRICTION_MULT;

//						control = speed < ent->stopspeed ? ent->stopspeed : speed;

//						newspeed = speed - FRAMETIME*control*friction;  This is the old line.  It stopped guys
//						from sliding more often than not.
						
						if (ent->health < 0)
						{
							newspeed = speed - FRAMETIME * friction * 5;
						}
						else
						{
							newspeed = speed - FRAMETIME * friction * ent->stopspeed;
						}

						if (newspeed < 0)
							newspeed = 0;
						newspeed /= speed;

						vel[0] *= newspeed;
						vel[1] *= newspeed;
					}
				}

			if (ent->ai)
				mask = MASK_MONSTERSOLID;
			else
				mask = MASK_SOLID; // dpk -- should this be changed for corpses?
			if ((ent->solid == SOLID_CORPSE) && wasonground)
			{	// we're dead and we're on the ground. don't slide so much. certainly not uphill.
				body_c	*body = NULL;
				ai_c	*myAI = (ai_c*)(ai_public_c*)ent->ai;
				if (myAI &&
					(body = (body_c*)myAI->GetBody()) && 
					(((level.time - body->GetInitialKilledTime()) > 1.5f) ||
					(body->IsInStationaryDeathAnim())) )
				{
					if (ent->velocity[2] > 0)
					{
						VectorScale(ent->velocity, 0.2, ent->velocity);
					}
					else
					{
						VectorScale(ent->velocity, 0.5, ent->velocity);
					}
				}
			}
			SV_FlyMove (ent, FRAMETIME, mask);

			gi.linkentity (ent);
			if (ent->solid != SOLID_CORPSE)
			{
				G_TouchTriggers (ent);
			}

	//		if (ent->groundentity)
	//			if (!wasonground)
	//				if (hitsound)
	//					gi.sound (ent, 0, gi.soundindex("world/land.wav"), .6, 1, 0);
		}
/*		else
		{
			Com_Printf("%s %d inside Step Physics -- velocity IS 0 \n", ent->classname, ent->s.number); 
		}
*/
	}

	if (ent->ai)
	{
		G_UpdateFrameEffects(ent);
	}


// regular thinking
	SV_RunThink (ent);
}

void SV_CheckGround (edict_t *ent)
{
	vec3_t		point;
	trace_t		trace;

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

	//corpse stuff
	if (ent->solid == SOLID_CORPSE)
	{
		vec3_t		mins, maxs;
		VectorCopy(ent->mins, mins);
		VectorCopy(ent->maxs, maxs);
		mins[0] = -8;
		mins[1] = -8;
		maxs[0] = 8;
		maxs[1] = 8;
		maxs[2] = 8;
//		debug_drawbox(ent, NULL, NULL, NULL, 1);
		gi.trace (ent->s.origin, mins, maxs, point, ent, ent->clipmask, &trace);
	}
	else
	{
		gi.trace (ent->s.origin, ent->mins, ent->maxs, point, ent, ent->clipmask, &trace);
	}

//	If I started in something solid, and it's solid benath me, say I'm on the ground
	if(trace.startsolid && trace.allsolid)
	{
		ent->groundentity = trace.ent;
		ent->groundentity_linkcount = trace.ent->linkcount;
		return;
	}

//	If I started in something solid, and I can fall through, I'm not on the ground
	if(trace.startsolid && !trace.allsolid)
	{
		ent->groundentity = NULL;
		return;
	}


//  If the trace went all the way, I ain't on the ground
	if ( trace.fraction == 1.0 )
	{
		ent->groundentity = NULL;
		return;
	}
//  Ok, I hit something, can I stand on it?
	if ( trace.plane.normal[2] < FLOOR_STEEPNESS)
	{
		ent->groundentity = NULL; // Nope, it is too steep
		return;
	}
	else	
	{
//	Yep, I'm standing on this thing	
		VectorCopy (trace.endpos, ent->s.origin);
		ent->groundentity = trace.ent;
		ent->groundentity_linkcount = trace.ent->linkcount;
//		ent->velocity[2] = 0;
	}
}

void SV_Physics_Dan (edict_t *ent)
{
	edict_t		*groundentity, *hit;
	qboolean	wasonground;
	int			numbumps, bumpcount, blocked, numplanes, i, j;
	vec3_t		original_velocity, primal_velocity, end, new_velocity;
	vec3_t		planes[MAX_CLIP_PLANES], dir;
	float		time_left, d;
	trace_t		trace;

	// first, run regular think
	SV_RunThink(ent);
	if (!ent->inuse)
	{	// were we just deleted?
		return;
	}

	if(ent->attachOwner)return;//if you're connected to something, there's no way you can collide with stuff, right?  Okay, maybe not, but we'll fix that later maybe...

	// Rick's suggested optimisation...(saves approx 600 traces a second on final/nyc)
	//
	if (!(Vec3IsZero(ent->velocity) && ent->groundentity && ent->groundentity->s.number==0))
	{
		// am I on the ground?
		SV_CheckGround (ent);
	}
//	else
//	{
//		static int i99=0;
//		Com_Printf("Skipped %d GroundChecks in PHYSICS_DAN\n",i99++);	// (needs "developer 1" to see this)
//	}

	groundentity = ent->groundentity; // I was standing on this (NULL if air)
	if (groundentity)
	{
		wasonground = true;
	}
	else
	{
		wasonground = false;
	}

	SV_CheckVelocity (ent);  // Make my velocities "in bounds" (defined by the cvar,
							 // sv_maxvelocity, which defaults to 2000.


//  dk  I changed if test to make sure all MOVETYPE_DAN objects are always affected
//	by  non-standard gravity...
	if ((ent->gravity && !wasonground) || ((sv_gravityx->value) || (sv_gravityy->value)
		 || (sv_gravityz->value < 0)))
	{
		SV_AddGravity(ent); // Add gravity if I'm in air (the cvar sv_gravity is multiplied by the
							// entity's gravity.  sv_gravity defaults to 800, entity
							// gravity defaults to 1.
							// If I'm in water, bouyancy will detract from this
	}

	if (ent->velocity[0] || ent->velocity[1] || ent->velocity[2])
	{
	//	setting up for the collision detections
		numbumps = 4;	// maximum of 4 clip planes checked
		blocked = 0;

		VectorCopy (ent->velocity, original_velocity);
		VectorCopy (ent->velocity, primal_velocity);

		numplanes = 0;
		time_left = FRAMETIME;

		ent->groundentity = NULL; // clear the ground entity for the new trace
		
		for (bumpcount=0 ; bumpcount<numbumps ; bumpcount++)
		{
			for (i=0 ; i<3 ; i++)
			{
				end[i] = ent->s.origin[i] + time_left * ent->velocity[i]; // the endpoint of the trace
			}
				// do the trace		from		 ---box--------------  to ignore  mask(for now)
			gi.trace (ent->s.origin, ent->mins, ent->maxs, end, ent, ent->clipmask, &trace);

	//	***Since the trace doesn't use the correct bounding box yet, I'm disabling this for now*** 
	//		if (trace.allsolid)
	//		{	// entity is trapped in another solid, so we'll never move
	//			VectorCopy (vec3_origin, ent->velocity);
	//			return;
	//		}
		
	// did last trace take me into a solid?!  If it did, bad things, man...
			{
//				static int i97=0;
//				Com_Printf("Horrible extra trace in PHYSICS_DAN (%d)\n",i97++);

				trace_t dbtrace;
				vec3_t		dumbmins, dumbmaxs, ones;

				for (i=0;i<3;i++)
					ones[i] = 1;
				VectorCopy(ent->mins, dumbmins);
				VectorAdd(dumbmins, ones, dumbmins);
				VectorCopy(ent->maxs, dumbmaxs);
				VectorSubtract(dumbmaxs, ones, dumbmaxs);
				gi.trace (trace.endpos, dumbmins, dumbmaxs, trace.endpos, ent, ent->clipmask, &dbtrace);

				if (dbtrace.startsolid || dbtrace.allsolid)
				{
					// kef -- if we're starting in a solid, and we're a knife, maybe hit someone's
					//buddy?
					if (dbtrace.ent && (dbtrace.ent->svflags & SVF_BUDDY))
					{	// hit a buddy. try to impact with its owner
						if (NULL == trace.ent)
						{
							trace.ent = dbtrace.ent->owner;
						}
					}
					else
					{
						if (trace.fraction > 0 && trace.fraction < 1 && trace.plane.normal[2] > 0.7)
						{
							if ( dbtrace.ent->solid == SOLID_BSP)
							{
								ent->groundentity = trace.ent;
								ent->groundentity_linkcount = trace.ent->linkcount;
							}
						}

						VectorClear(ent->velocity);	// I added this line - bl

						// Commenting these 4 lines out because they seem rather unneeded - bl
	//					VectorNormalize (ent->velocity);
	//					VectorScale (ent->velocity, -1, ent->velocity);//back outta here
	//					for (i=0;i<3;i++)
	//						ent->velocity[i] +=crandom(); // get me outta here
						return;//trace failed!!!!! muy mal!
					}
				}
			}
			

			if (trace.fraction > 0)
			{	// trace covered at least a portion of the distance
				VectorCopy (trace.endpos, ent->s.origin);	// move me
	/*			if (ent->chain)
				{
					VectorCopy(ent->s.origin, ent->chain->s.origin);
				}
	*/			VectorCopy (ent->velocity, original_velocity);	// store what my velocity was
				numplanes = 0;
			}

			if (trace.fraction == 1)
				 break;		// moved the entire distance of the trace, get out of this loop

			hit = trace.ent; // If we're getting here, we hit something.  store it

			if (trace.plane.normal[2] > 0.7) // we hit floor
			{
				blocked |= 1;
				if (( hit->solid == SOLID_BSP) || (hit->solid == SOLID_GHOUL)) // *** should this include models?  i think so
//				if ( hit->solid == SOLID_BSP )
				{
					ent->groundentity = hit;
					ent->groundentity_linkcount = hit->linkcount;
				}
				SV_HandleGenericGroundEffect(ent);
			}
			if (trace.plane.normal[2] < 0.7)
			{
				blocked |= 2;		// step or wall
			}
/*			if (!trace.plane.normal[2])
			{
				blocked |= 2;		// step or wall
			}
*/
	//
	// run the impact function
	//
			if (!SV_Impact (ent, &trace)) // runs their touch functions
			{	
				break;
			}
			if (!ent->inuse)
				break;		// if we lost our entity due to the impact function, get out

			time_left -= time_left * trace.fraction;
			
	// clipped to another plane
			if (numplanes >= MAX_CLIP_PLANES)
			{	// this shouldn't really happen
				VectorCopy (vec3_origin, ent->velocity);
				return;
			}

			VectorCopy (trace.plane.normal, planes[numplanes]); // store normals of the planes I hit
			numplanes++;

	//
	// modify original_velocity so it parallels all of the clip planes
	//
			for (i=0 ; i<numplanes ; i++) 
			{
	/*			if (!trace.fraction)
				{
					ClipVelocity (original_velocity, planes[i], new_velocity, 1.1);//a little extra bounce to pop me out of any corners i might get stuck in
		// *** make my own here too		
				}
				else
					ClipVelocity (original_velocity, planes[i], new_velocity, 1);
				}
	*/
				
				ClipVelocity_Dan (original_velocity, planes[i], new_velocity, ent->elasticity);
// if elasticity is -1, we want to use whatever the ent's velocity is now, to plow through stuff.	
				if (ent->elasticity == -1)
				{
					VectorCopy(ent->velocity, new_velocity);
				}
				for (j=0 ; j<numplanes ; j++)
					if (j != i)
					{
						if (DotProduct (new_velocity, planes[j]) <= 0)//parallel or against vel n/ ok
							break;	// not ok
					}
				if (j == numplanes)
					break;
			}
		
			if (i != numplanes)
			{	// go along this plane
				VectorCopy (new_velocity, ent->velocity);
			}
			else
			{	// go along the crease
				if (numplanes != 2)
				{
					VectorCopy (vec3_origin, ent->velocity);
					return;
				}
				CrossProduct (planes[0], planes[1], dir);
				d = DotProduct (dir, ent->velocity);
				VectorScale (dir, d, ent->velocity);
			}
		}

		SV_CheckGround (ent);
	}
	
	groundentity = ent->groundentity; // I was standing on this (NULL if air)
	if (groundentity)
	{
		wasonground = true;
	}
	else
	{
		wasonground = false;
	}

	if (ent->avelocity[0] || ent->avelocity[1] || ent->avelocity[2])
	{
		SV_Rotate(ent);								  // Go ahead and rotate for now,
													  // but this might be a good place for
													  // a collision test some day
		SV_AddRotationalFriction (ent, wasonground);  // Add rotational friction or
													  // rotational air resistance
	}

	if (ent->velocity[0] || ent->velocity[1] || ent->velocity[2])
	{
		SV_AddLinearFriction(ent, wasonground);  // Add linear friction or air resistance
	}

	gi.linkentity (ent);
}

//============================================================================
/*
================
G_RunEntity

================
*/
void G_RunEntity (edict_t *ent)
{

	switch ( (int)ent->movetype)
	{
	case MOVETYPE_PUSH:
	case MOVETYPE_STOP:
		SV_Physics_Pusher (ent);
		break;
	case MOVETYPE_NONE:
		SV_Physics_None (ent);
		break;
	case MOVETYPE_NOCLIP:
		SV_Physics_Noclip (ent);
		break;
	case MOVETYPE_STEP:
		SV_Physics_Step (ent);
		break;
	case MOVETYPE_TOSS:
	case MOVETYPE_BOUNCE:
	case MOVETYPE_FLY:
	case MOVETYPE_FLYMISSILE:
		SV_Physics_Toss (ent);
		break;
	case MOVETYPE_DAN:
	case MOVETYPE_TOUCHNOTSOLID:
		SV_Physics_Dan (ent);
		break;
	default:
		gi.error ("SV_Physics: bad movetype %i", (int)ent->movetype);			
	}
}

void SV_HandleGenericGroundEffect(edict_t *ent)
{
	// are velocities negative or positive at this point???
	if(ent->client)
	{	// player floor collision is already handled elsewhere
		return;
	}

	if (ent->health <= 0)
	{
		return;
	}

	if(ent->velocity[2] > -100)
//	if(ent->velocity[2] > -30)
	{
	}
	else if(ent->velocity[2] > -200)
//	else if(ent->velocity[2] > -120)
	{
		FX_SetEvent(ent, EV_OBJECT_COLLIDE_SHORT);
	}
	else if(ent->velocity[2] > -400)
//	else if(ent->velocity[2] > -360)
	{
		FX_SetEvent(ent, EV_OBJECT_COLLIDE_MEDIUM);
	}
	else
	{
		FX_SetEvent(ent, EV_OBJECT_COLLIDE_FAR);
	}
}

void PHYS_ClearAttachList(edict_t *ed)
{
	edict_t *curCheck;
	int		next;

	if(ed->attachOwner)
	{	//remove a guy from the list
		curCheck = &g_edicts[ed->attachOwner];

		while(curCheck->attachChain)
		{
			if(&g_edicts[curCheck->attachChain] == ed)
			{//eliminate this guy from the attach chain list
				curCheck->attachChain = ed->attachChain;
				ed->attachChain = 0;
				break;
			}
			curCheck = &g_edicts[curCheck->attachChain];
		}
		assert(attachmentNotCircular(&g_edicts[ed->attachOwner]));
	}
	else if(ed->attachChain)
	{	//must be the start of a chain - disconnect everyone
		curCheck = &g_edicts[ed->attachChain];
		next = curCheck->attachChain;

		while(next)
		{
			next = curCheck->attachChain;
			curCheck->attachChain = 0;
			curCheck->attachOwner = 0;
			curCheck = &g_edicts[next];
		}

	}
}
