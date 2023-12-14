
#include "g_local.h"

void GetOffsetFromEnt(edict_t *ent, vec3_t offset, vec3_t ang, vec3_t sourceNormal, vec3_t sourceHit)
{
	vec3_t	sourcePos, sourceAng;
	float	cosVal, sinVal;

	VectorSubtract(sourceHit, ent->s.origin, offset);

	// y z x
	// roll yaw pitch

	VectorCopy(offset, sourcePos);//need to store it if before modification
	VectorCopy(sourceNormal, ang);
	VectorCopy(ang, sourceAng);//need to store it if before modification

	cosVal = cos(M_PI/180.0 * ent->s.angles[YAW]);
	sinVal = sin(M_PI/180.0 * ent->s.angles[YAW]);

	offset[0] = cosVal*sourcePos[0] + sinVal*sourcePos[1];
	offset[1] = -cosVal*sourcePos[1] + sinVal*sourcePos[0];

	ang[0] = cosVal*sourceAng[0] + sinVal*sourceAng[1];
	ang[1] = cosVal*sourceAng[1] - sinVal*sourceAng[0];
}

void FXMSG_WriteRelativePos(edict_t *ent, vec3_t endpos, vec3_t normal, mtexinfo_t *surf)
{
	vec3_t	spot;

	if(ent == world)
	{
		if(normal == NULL)
		{
			gi.WriteByte(RI_WORLD_NOSURF);
			gi.WritePosition(endpos);
		}
		else
		{
			gi.WriteByte(RI_WORLD);
			gi.WritePosition(endpos);
			WriteDirExp(normal);
		}
	}
	else if(ent->solid == SOLID_BSP)
	{
		vec3_t hitLoc, hitAng;

		GetOffsetFromEnt(ent, hitLoc, hitAng, normal, endpos);

		gi.WriteByte(RI_BMODEL);
		gi.WritePosition(hitLoc);
		WriteDirExp(hitAng);
		gi.WriteShort(ent - g_edicts);
		if(ent->s.renderindex != 0)
		{
			if(ent->surfaceType)
			{
				gi.WriteByte(ent->surfaceType);
			}
			else
			{
				gi.WriteByte(surf->flags>>24);
			}
		}
		else
		{
			gi.WriteByte(surf->flags>>24);
		}
	}
	else
	{
		VectorSubtract(endpos, ent->s.origin, spot);
		//VectorSubtract(ent->s.origin, endpos, spot);
		spot[2] = 0;

		gi.WriteByte(RI_ENT);
		gi.WritePosition(endpos);
		WriteDirExp(spot);
		if((ent->ai)||(ent->client))
		{
			gi.WriteByte(SURF_BLOOD); //fixme: this looks like it could be problematic! bleeding vehicles, etc?
		}
		else
		{
			gi.WriteByte(ent->surfaceType);
		}
	}
}

void FXMSG_WriteRelativePosTR(trace_t &tr)
{
	FXMSG_WriteRelativePos(tr.ent, tr.endpos, tr.plane.normal, tr.surface);
}

void FXMSG_WriteRelativePos_Dir(vec3_t pos, vec3_t dir, edict_t *ignore)
{
	trace_t tr;
	vec3_t start, end;

	VectorMA(pos, 5, dir, start);
	VectorMA(pos, -35, dir, end);

	gi.trace (start, vec3_origin, vec3_origin, end, ignore, MASK_SOLID, &tr);

	if(tr.fraction < .99)
	{
		FXMSG_WriteRelativePosTR(tr);
	}
	else
	{
		FXMSG_WriteRelativePos(world, pos, NULL, NULL);
	}
}

void FXMSG_WriteRelativePos_TestGround(vec3_t pos, edict_t *ignore)
{
	vec3_t dir = {0, 0, 1};

	FXMSG_WriteRelativePos_Dir(pos, dir, ignore);
}


//-------------------------------------------------------------------------
//
//						General Stuff
//
//-------------------------------------------------------------------------

void FX_Init(void)
{
	return;
}

//-------------------------------------------------------------------------
//
//						Event Stuff
//
//-------------------------------------------------------------------------

void FX_SetEvent_Data(edict_t *ent, int eventType,int data)
{	// make this more complex to handle the possibility of multiple events?
	assert(eventType < EV_NUM);

	if(ent->s.event)
	{
		if(ent->s.event2)
		{
			ent->s.event2 |= 0x80;
			ent->s.event3 = eventType;
			ent->s.data3 = data;
		}
		else
		{
			ent->s.event |= 0x80;
			ent->s.event2 = eventType;
			ent->s.data2 = data;
		}
	}
	else
	{
		ent->s.event = eventType;
		ent->s.data = data;
	}
}

void FX_SetEvent(edict_t *ent, int eventType)
{	// make this more complex to handle the possibility of multiple events?
	assert(eventType < EV_NUM);

	if(ent->s.event)
	{
		if(ent->s.event2)
		{
			ent->s.event2 |= 0x80;
			ent->s.event3 = eventType;
		}
		else
		{
			ent->s.event |= 0x80;
			ent->s.event2 = eventType;
		}
	}
	else
	{
		ent->s.event = eventType;
	}
}


void FX_ClearEvent(edict_t *ent)
{
	ent->s.event = 0;
}

