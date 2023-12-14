#include "g_local.h"
#include "../strings/dm_generic.h"

void UpdateChaseCam(edict_t *ent)
{
	edict_t *targ;
	vec3_t	forward,right,up,
			o,ownerv,goal;
	vec3_t	angles;
	vec3_t	min={-1,-1,-1},max={1,1,1};
	trace_t trace;

	// Has our chase target gone?

	if(!ent->client->chase_target->inuse||ent->client->chase_target->client->resp.spectator) 
	{
		edict_t *old = ent->client->chase_target;
		
		ChaseNext(ent);
		
		if (ent->client->chase_target == old)
		{
			ent->client->chase_target = NULL;
			ent->client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
			
			return;
		}
	}

	targ=ent->client->chase_target;

	// Base viewing angles.

	VectorCopy(targ->client->v_angle,angles);
	
	if(angles[PITCH] > 56)
		angles[PITCH] = 56;
	else if(angles[PITCH] < -56)
		angles[PITCH] = -56;

	// Get focal point.

	AngleVectors(angles,forward,right,up);
	VectorScale(up,targ->viewheight+16,ownerv);
	VectorAdd(targ->s.origin,ownerv,ownerv);
	gi.trace(targ->s.origin,min,max,ownerv,targ,MASK_SOLID,&trace);
	VectorCopy(trace.endpos,ownerv);

	// Get camera point.

	VectorNormalize(forward);
	VectorMA(ownerv,-35,forward,o);
	
	if (!targ->groundentity)
		o[2] += 16;

	gi.trace(ownerv, min, max, o, targ, MASK_SOLID, &trace);
	VectorCopy(trace.endpos, goal);

	// Set camera origin and angles.
	
	VectorCopy(goal, ent->s.origin);
	
	for (int i=0 ; i<3 ; i++)
		ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(angles[i] - ent->client->resp.cmd_angles[i]);

	if (targ->deadflag)
	{
		ent->client->ps.pmove.pm_type = PM_DEAD;

		ent->client->ps.viewangles[ROLL] = 40;
		ent->client->ps.viewangles[PITCH] = -15;
		ent->client->ps.viewangles[YAW] = targ->client->killer_yaw;
	} 
	else 
	{
		ent->client->ps.pmove.pm_type = PM_SPECTATOR_FREEZE;

		VectorCopy(angles, ent->client->ps.viewangles);
		VectorCopy(angles, ent->client->v_angle);
	}

	// Other misc. crap. and then re-link us.

	ent->client->ps.spectatorId=targ->s.number;
	ent->viewheight=0;
	ent->client->ps.pmove.pm_flags |= PMF_NO_PREDICTION;
	gi.linkentity(ent);
}

void ChaseNext(edict_t *ent)
{
	int i;
	edict_t *e;

	if (!ent->client->chase_target)
		return;

	i = ent->client->chase_target - g_edicts;
	do {
		i++;
		if (i > (int)maxclients->value)
			i = 1;
		e = g_edicts + i;
		if (!e->inuse)
			continue;
		if (!e->client->resp.spectator)
			break;
	} while (e != ent->client->chase_target);

	ent->client->chase_target = e;
	ent->client->update_chase = true;
}

void ChasePrev(edict_t *ent)
{
	int		i;
	edict_t *e;

	if (!ent->client->chase_target)
		return;

	i = ent->client->chase_target - g_edicts;

	do
	{
		i--;
		
		if (i < 1)
			i = (int)maxclients->value;

		e = g_edicts + i;
		
		if (!e->inuse)
			continue;
		
		if (!e->client->resp.spectator)
			break;
	} while (e != ent->client->chase_target);

	ent->client->chase_target = e;
	ent->client->update_chase = true;
}

void GetChaseTarget(edict_t *ent)
{
	int		i;
	edict_t *other;

	for (i = 1; i <= (int)maxclients->value; i++) 
	{
		other = g_edicts + i;

		if (other->inuse && !other->client->resp.spectator)
		{
			ent->client->chase_target = other;
			ent->client->update_chase = true;
			UpdateChaseCam(ent);
			return;
		}
	}
	
	gi.SP_Print(ent,DM_GENERIC_TEXT_NO_MORE_CHASE);
}
