#include "g_local.h"
#include "ai_private.h"

sense_c aSenseC;
sound_sense aSoundSenseC;
sight_sense aSightSenseC;
alarm_sense aAlarmSenseC;

sense_c *sense_c::NewClassForCode(int code)
{
	switch (code)
	{
	default:
		gi.dprintf("ERROR: invalid sense class code: %d\n",code);
	case SENSE:
		return new sense_c;
	case ALARM_SENSE:
		return new alarm_sense;
	case SIGHT_SENSE:
		return new sight_sense;
	case NORMALSIGHT_SENSE:
		return new normalsight_sense;
	case SOUND_SENSE:
		return new sound_sense;
	case OMNISCIENCE_SENSE:
		return new omniscience_sense;
	case MAGICSIGHT_SENSE:
		return new magicsight_sense;
	}
}

sense_c::sense_c(vec3_t new_origin, float new_time)
{
	VectorCopy(new_origin, origin);
	time = new_time;
	mute_starttime = level.time-1;
	mute_endtime = level.time-1;
	mute_degree = 0;
	mute_recovercode = smute_recov_instant;

	sensed_client.time = - 10000.0;
	sensed_client.ent = NULL;
	sensed_client.senseType = AI_SENSETYPE_UNKNOWN;

	sensed_monster.time = - 10000.0;
	sensed_monster.ent = NULL;
	sensed_monster.senseType = AI_SENSETYPE_UNKNOWN;
}

sense_c::sense_c(void)
{
	VectorClear(origin);

	time = level.time;
	mute_starttime = level.time-1;
	mute_endtime = level.time-1;
	mute_degree = 0;
	mute_recovercode = smute_recov_instant;

	sensed_client.time = - 10000.0;
	VectorClear(sensed_client.pos);
	sensed_client.ent = NULL;
	sensed_client.senseType = AI_SENSETYPE_UNKNOWN;

	sensed_monster.time = - 10000.0;
	VectorClear(sensed_monster.pos);
	sensed_monster.ent = NULL;
	sensed_monster.senseType = AI_SENSETYPE_UNKNOWN;
}

float sense_c::CurrentMutedLevel(void)
{
	float timeVal;

	//if the muting has timed out, then forget it
	if ((mute_recovercode != smute_recov_none) && (mute_endtime <= level.time || mute_starttime >= level.time))
	{
		return 0;
	}

	switch(mute_recovercode)
	{
	default:
		gi.dprintf("unknown sense muting recovery code: %d!\n", mute_recovercode);
	case smute_recov_none:
		return mute_degree;
	case smute_recov_instant:
		return mute_degree;
	case smute_recov_linear:
		return mute_degree*(mute_endtime-level.time)/(mute_endtime-mute_starttime);
	case smute_recov_exp:
		timeVal=(mute_endtime-level.time)/(mute_endtime-mute_starttime);
		return mute_degree*timeVal*timeVal;
	}
}

int sense_c::Range(edict_t *monster, edict_t *other)
{
	vec3_t to_other;
	float dist_sq,mutetemp;

	VectorSubtract(other->s.origin, monster->s.origin, to_other);
	dist_sq = VectorLengthSquared(to_other);
	mutetemp = (1+CurrentMutedLevel());
	mutetemp *= mutetemp;
	dist_sq *= mutetemp;

	return gmonster.Range(monster,other,dist_sq);
}

int sense_c::Range(edict_t *monster, vec3_t where)
{
	vec3_t to_other;
	float dist_sq,mutetemp;

	VectorSubtract(where, monster->s.origin, to_other);
	dist_sq = VectorLengthSquared(to_other);
	mutetemp = (1+CurrentMutedLevel());
	mutetemp *= mutetemp;
	dist_sq *= mutetemp;

	return gmonster.Range(monster,monster,dist_sq);
}

void sense_c::UpdateSensedClient(unsigned mask, sensedEntInfo_t &best_ent)
{
	if (!MaskOK(mask))
	{
		return;
	}
	if (best_ent.time > sensed_client.time)
	{
		return;
	}
	best_ent.ent = sensed_client.ent;
	VectorCopy(sensed_client.pos, best_ent.pos);
	best_ent.time = sensed_client.time;
	best_ent.senseType = sensed_client.senseType;
}

void sense_c::UpdateSensedMonster(unsigned mask, sensedEntInfo_t &best_ent)
{
	if (!MaskOK(mask))
	{
		return;
	}
	if (best_ent.time > sensed_monster.time)
	{
		return;
	}
	best_ent.ent = sensed_monster.ent;
	VectorCopy(sensed_monster.pos, best_ent.pos);
	best_ent.time = sensed_monster.time;
	best_ent.senseType = sensed_monster.senseType;
}

sense_c::sense_c(sense_c *orig)
{
	VectorCopy(orig->origin, origin);

	time				= orig->time;
	sensed_monster		= orig->sensed_monster;
	sensed_client		= orig->sensed_client;

	mute_recovercode	= orig->mute_recovercode;
	mute_starttime		= orig->mute_starttime;
	mute_endtime		= orig->mute_endtime;
	mute_degree			= orig->mute_degree;

	*(int *)&sensed_monster.ent = GetEdictNum(orig->sensed_monster.ent);
	*(int *)&sensed_client.ent = GetEdictNum(orig->sensed_client.ent);
}

void sense_c::Evaluate(sense_c *orig)
{
	VectorCopy(orig->origin, origin);

	time				= orig->time;
	sensed_monster		= orig->sensed_monster;
	sensed_client		= orig->sensed_client;

	mute_recovercode	= orig->mute_recovercode;
	mute_starttime		= orig->mute_starttime;
	mute_endtime		= orig->mute_endtime;
	mute_degree			= orig->mute_degree;

	sensed_monster.ent = GetEdictPtr((int)sensed_monster.ent);
	sensed_client.ent = GetEdictPtr((int)sensed_client.ent);  
}

/**********************************************************************************
 **********************************************************************************/

alarm_sense::alarm_sense(vec3_t new_origin, float new_time, float new_radius)
:sense_c(new_origin, new_time)
{
	radius = new_radius;
}

qboolean alarm_sense::Evaluate(unsigned mask, ai_c &owner_ai, edict_t &monster)
{
	if (!(mask & alarm_mask))
	{
		return false;
	}
	return true;
}

alarm_sense::alarm_sense(alarm_sense *orig)
: sense_c(orig)
{
	radius = orig->radius;
}

void alarm_sense::Evaluate(alarm_sense *orig)
{
	radius = orig->radius;

	sense_c::Evaluate(orig);
}

void alarm_sense::Read()
{
	char	loaded[sizeof(alarm_sense)];

	gi.ReadFromSavegame('AIAS', loaded, sizeof(alarm_sense));
	Evaluate((alarm_sense *)loaded);
}

void alarm_sense::Write()
{
	alarm_sense	*savable;

	savable = new alarm_sense(this);
	gi.AppendToSavegame('AIAS', savable, sizeof(*savable));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/

//general fixme for sense evaluation: need a solid set of criteria for when to
//replace the enemy in monster's ai, and when to just leave what's there.
//whether this should be in senses or ai or what remains to be seen.

//visibility is poor, but to simulate monsters anticipating player movement, allow position update--
//but no new sighting, and no updating of sight time
void sight_sense::PoorVisibilityUpdate(ai_c &owner_ai, edict_t *monster, edict_t *sight_ent, float leeway_time)
{
	//do for both clients and monsters
	if (sensed_client.ent == sight_ent && level.time - sensed_client.time < leeway_time)
	{
		VectorCopy(sight_ent->s.origin, sensed_client.pos);
		sensed_client.senseType = AI_SENSETYPE_SIGHT_OBSTRUCTED;
	}
	else if (sensed_monster.ent == sight_ent && level.time - sensed_monster.time < leeway_time)
	{
		VectorCopy(sight_ent->s.origin, sensed_monster.pos);
		sensed_monster.senseType = AI_SENSETYPE_SIGHT_OBSTRUCTED;
	}
}

void sight_sense::Look(ai_c &owner_ai, edict_t *monster, edict_t *sight_ent)
{
	edict_t *client=sight_ent;
	int r;

	if (!client)
		return;	// no clients to get mad at

	// if the entity went away, forget it
	if (!client->inuse)
		return;

	//i see myself??? this is a thing that makes no sense.
	if (client == monster)
	{
		return;
	}

	if (client->client)
	{
		if (client->flags & FL_NOTARGET || (OnSameTeam(monster,client) && !owner_ai.GetAbusedByTeam()))// i don't play with cheaters.
			return;
	}
	else if (!client->ai)
	{
		return;
	}

	if((client->spawnflags & SPAWNFLAG_HOSTAGE)&&(monster->flags & FL_SPAWNED_IN))
	{	// spawned guys have no interest in hostages - just in nosy players
		return;
	}

	if((!strcmp(client->classname, "m_x_mmerc")))
	{	// people don't target Hawk - they're too afraid of him.
		return;
	}

	r = Range (monster, client);

	if (r == RANGE_FAR)
		return;

// this is where we would check invisibility

	if (r != RANGE_MELEE)//if e's close enuff for melee, i just Know where e is, ok?
	{
		// is client in an spot too dark to be seen?
		//no technical reason for commenting this, but test levels aren't generally lit well--gets in way
		//addendum--monsters don't have their light level set validly, i don't think--gotten from client

		//fixme: need to do something similar for monsters
/*
		if (client->client && client->light_level <= 5)
		{
			PoorVisibilityUpdate(owner_ai, monster, client, 6.0);
			return;
		}
*/

		//take care of stealth biz
		if (client->svflags & SVF_ISHIDING)
		{
			PoorVisibilityUpdate(owner_ai, monster, client, 4.0);
			return;
		}

		if (!gmonster.Infront (monster, client))
		{
			PoorVisibilityUpdate(owner_ai, monster, client, 4.0);
			return;
		}
	}

	//took this out of non-melee checks--can't allow superclose range bypass visibility check, have guys shootin through walls
	if (!gmonster.Visible (monster, client))
	{
		PoorVisibilityUpdate(owner_ai, monster, client, 0.25);
		return;
	}

	//update appropriate slot for monster
	if (client->client)
	{
		sensed_client.ent = client;
		sensed_client.time = level.time;
		VectorCopy(client->s.origin, sensed_client.pos);
		sensed_client.senseType = AI_SENSETYPE_SIGHT_CLEAR;
	}
	else
	{
		sensed_monster.ent = client;
		sensed_monster.time = level.time;
		VectorCopy(client->s.origin, sensed_monster.pos);
		sensed_monster.senseType = AI_SENSETYPE_SIGHT_CLEAR;
	}
}

qboolean sight_sense::Evaluate(unsigned mask, ai_c &owner_ai, edict_t &monster)
{
	if (!(mask & sight_mask))
	{
		return false;
	}

	Look(owner_ai,&monster,level.sight_client); // level will continually update who we can be mad at
	Look(owner_ai,&monster,level.sight_monster); // level will continually update who we can be mad at

	return false;
}

void sight_sense::Mute(unsigned mask, float degree, smute_recovery recovery_code, float recovery_time)
{
	if (!(mask & sight_mask))
	{
		return;
	}
	mute_starttime=level.time;
	mute_endtime=level.time+recovery_time;
	mute_degree=degree;
	mute_recovercode=recovery_code;
}

sight_sense::sight_sense(sight_sense *orig)
: sense_c(orig)
{
}

void sight_sense::Evaluate(sight_sense *orig)
{
	sense_c::Evaluate(orig);
}

void sight_sense::Read()
{
	char	loaded[sizeof(sight_sense)];

	gi.ReadFromSavegame('AISS', loaded, sizeof(sight_sense));
	Evaluate((sight_sense *)loaded);
}

void sight_sense::Write()
{
	sight_sense	*savable;

	savable = new sight_sense(this);
	gi.AppendToSavegame('AISS', savable, sizeof(*savable));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/

normalsight_sense::normalsight_sense(normalsight_sense *orig)
: sight_sense(orig)
{
}

void normalsight_sense::Evaluate(normalsight_sense *orig)
{
	sight_sense::Evaluate(orig);
}

void normalsight_sense::Write()
{
	normalsight_sense	*savable;

	savable = new normalsight_sense(this);
	gi.AppendToSavegame('AINS', savable, sizeof(*this));
	delete savable;
}

void normalsight_sense::Read()
{
	char	loaded[sizeof(normalsight_sense)];

	gi.ReadFromSavegame('AINS', loaded, sizeof(normalsight_sense));
	Evaluate((normalsight_sense *)loaded);
}

/**********************************************************************************
 **********************************************************************************/

void sound_sense::RegisterEvent(vec3_t event_origin, float event_time, edict_t *event_edict, ai_sensetype_e event_code)
{
	if (numsounds >= MAX_AISOUNDS)
	{
		return;
	}

	VectorCopy(event_origin, sounds[numsounds].heardLocation);
	VectorCopy(event_edict->s.origin, sounds[numsounds].origin);
	sounds[numsounds].ent = event_edict;
	sounds[numsounds].code = event_code;
	sounds[numsounds].time = event_time;

	numsounds++;
}

void sound_sense::Mute(unsigned mask, float degree, smute_recovery recovery_code, float recovery_time)
{
	if (!(mask & sound_mask))
	{
		return;
	}
	mute_starttime=level.time+0.3;
	mute_endtime=level.time+recovery_time+0.3;
	mute_degree=degree;
	mute_recovercode=recovery_code;
}

qboolean sound_sense::Evaluate(unsigned mask, ai_c &owner_ai, edict_t &monster)
{
	int r,i;

	//these fellas were here to test more realistic reactions, but i ended up focusing more on Getting a reaction, thus they're commented out but they should be fine
	vec3_t	toHeardLocation;
	float	toHeardLocationDistSq;

	if (!(mask & sound_mask))
	{
		return false;
	}

	for (i=0;i<numsounds;i++)
	{
		if (sounds[i].ent->flags & FL_NOTARGET)
		{
			continue;
		}

		//always hear magical sounds...
		if (sounds[i].code == AI_SENSETYPE_SOUND_MAGICAL)
		{
			if (sounds[i].ent->client)
			{
				sensed_client.time = level.time;
				VectorCopy(sounds[i].origin, sensed_client.pos);
				sensed_client.ent = sounds[i].ent;
				sensed_client.senseType = sounds[i].code;
			}
			else
			{
				sensed_monster.time = level.time;
				VectorCopy(sounds[i].origin, sensed_monster.pos);
				sensed_monster.ent = sounds[i].ent;
				sensed_monster.senseType = sounds[i].code;
			}
			continue;
		}

		r = Range (&monster, sounds[i].heardLocation);

		//din't hear it
		if (r==RANGE_FAR)
		{
			continue;
		}

		//wretch! clean me up! fixme! cleaner i say!
		//update appropriate info depending on whether sound is coming from player or monster
		if (sounds[i].ent)
		{
			VectorSubtract(sounds[i].heardLocation, monster.s.origin, toHeardLocation);
			toHeardLocationDistSq=VectorLengthSquared(toHeardLocation);

			//if it's a wake-up that's really close, it was probably me--ignore it
			if (sounds[i].code==AI_SENSETYPE_SOUND_WAKEUP && toHeardLocationDistSq < 400)
			{
				continue;
			}

			if (sounds[i].ent->client)
			{
				//fixme: if i don't have an enemy, should update position, but differently
				if (!sensed_client.ent || level.time - sensed_client.time > 10.0 || sensed_client.ent == sounds[i].ent)
				{
					if (sounds[i].code == AI_SENSETYPE_SOUND_SELF || sounds[i].code == AI_SENSETYPE_SOUND_WEAPON)
					{
						sensed_client.time = level.time-5.0;
//						if (toHeardLocationDistSq<40000)
//						{
							VectorCopy(sounds[i].origin, sensed_client.pos);
//						}
//						else
//						{
//							VectorCopy(sounds[i].heardLocation, sensed_client.pos);
//						}
						sensed_client.ent = sounds[i].ent;
						sensed_client.senseType = sounds[i].code;
					}
					else if (!sensed_client.ent || level.time - sensed_client.time > 20.0)//weapon impact or whiz
					{
						sensed_client.time = level.time-10.0;
//						if (toHeardLocationDistSq<40000)
//						{
							VectorCopy(sounds[i].origin, sensed_client.pos);
//						}
//						else
//						{
//							VectorCopy(sounds[i].heardLocation, sensed_client.pos);
//						}
						sensed_client.ent = sounds[i].ent;
						sensed_client.senseType = sounds[i].code;
					}
				}
			}
			else
			{
				//fixme: if i don't have an enemy, should update position, but differently
				if (!sensed_monster.ent || level.time - sensed_monster.time > 10.0)
				{
					if (sounds[i].code == AI_SENSETYPE_SOUND_SELF || sounds[i].code == AI_SENSETYPE_SOUND_WEAPON)
					{
						sensed_monster.time = level.time-5.0;
//						if (toHeardLocationDistSq<40000)
//						{
							VectorCopy(sounds[i].origin, sensed_monster.pos);
//						}
//						else
//						{
//							VectorCopy(sounds[i].heardLocation, sensed_monster.pos);
//						}
						sensed_monster.ent = sounds[i].ent;
						sensed_client.senseType = sounds[i].code;
					}
					else if (!sensed_monster.ent || level.time - sensed_monster.time > 20.0)//weapon impact or whiz
					{
						sensed_monster.time = level.time-10.0;
//						if (toHeardLocationDistSq<40000)
//						{
							VectorCopy(sounds[i].origin, sensed_monster.pos);
//						}
//						else
//						{
//							VectorCopy(sounds[i].heardLocation, sensed_monster.pos);
//						}
						sensed_monster.ent = sounds[i].ent;
						sensed_monster.senseType = sounds[i].code;
					}
				}
			}
		}
	}

	numsounds=0;
	return false;
}

sound_sense::sound_sense(sound_sense *orig)
: sense_c(orig)
{
	int		i;

	numsounds = orig->numsounds;
	for(i = 0; i < numsounds; i++)
	{
		sounds[i] = orig->sounds[i];
		*(int *)&sounds[i].ent = GetEdictNum(orig->sounds[i].ent);
	}
	for( ; i < MAX_AISOUNDS; i++)
	{
		VectorClear(sounds[i].heardLocation);
		VectorClear(sounds[i].origin);
		sounds[i].ent = NULL;
		sounds[i].code = AI_SENSETYPE_UNKNOWN;
		sounds[i].time = 0.0F;
	}
}

void sound_sense::Evaluate(sound_sense *orig)
{
	int			i;

	numsounds = orig->numsounds;
	for(i = 0; i < numsounds; i++)
	{
		sounds[i] = orig->sounds[i];
		sounds[i].ent = GetEdictPtr((int)sounds[i].ent);
	}
	sense_c::Evaluate(orig);
}

void sound_sense::Read()
{
	char	loaded[sizeof(sound_sense)];

	gi.ReadFromSavegame('AISO', loaded, sizeof(sound_sense));
	Evaluate((sound_sense *)loaded);
}

void sound_sense::Write()
{
	sound_sense	*savable;

	savable = new sound_sense(this);
	gi.AppendToSavegame('AISO', savable, sizeof(*savable));
	delete savable;
}

/////////////////////////////////////////////////////////////////////////////////


void omniscience_sense::Look(ai_c &owner_ai, edict_t *monster, edict_t *sight_ent)
{
	edict_t *client=sight_ent;

	if (!client)
		return;	// no clients to get mad at

	// if the entity went away, forget it
	if (!client->inuse)
		return;

	//i see myself??? this is a thing that makes no sense.
	if (client == monster)
	{
		return;
	}

	if (client->client)
	{
		if (client->flags & FL_NOTARGET || (OnSameTeam(monster,client) && !owner_ai.GetAbusedByTeam()))// i don't play with cheaters.
			return;
	}
	else if (!client->ai)
	{
		return;
	}

	//no checks--i see all

	//update appropriate slot for monster
	if (client->client)
	{

		sensed_client.ent = client;
		sensed_client.time = level.time;
		VectorCopy(client->s.origin, sensed_client.pos);
		sensed_client.senseType = AI_SENSETYPE_MAGIC;
	}
	else
	{
		sensed_monster.ent = client;
		sensed_monster.time = level.time;
		VectorCopy(client->s.origin, sensed_monster.pos);
		sensed_monster.senseType = AI_SENSETYPE_MAGIC;
	}
}

omniscience_sense::omniscience_sense(omniscience_sense *orig)
: sight_sense(orig)
{
}

void omniscience_sense::Evaluate(omniscience_sense *orig)
{
	sight_sense::Evaluate(orig);
}

void omniscience_sense::Read()
{
	char	loaded[sizeof(omniscience_sense)];

	gi.ReadFromSavegame('AINS', loaded, sizeof(omniscience_sense));
	Evaluate((omniscience_sense *)loaded);
}

void omniscience_sense::Write()
{
	omniscience_sense	*savable;

	savable = new omniscience_sense(this);
	gi.AppendToSavegame('AINS', savable, sizeof(*this));
	delete savable;
}

void omniscience_sense::Mute(unsigned mask, float degree, smute_recovery recovery_code, float recovery_time)
{
	return;
}

/////////////////////////////////////////////////////////////////////////////////

//geh. this could prolly be smooshed into regular sight, but this is the quick way to get it working for blind guys
void magicsight_sense::Look(ai_c &owner_ai, edict_t *monster, edict_t *sight_ent)
{
	edict_t *client=sight_ent;

	if (!client)
		return;	// no clients to get mad at

	// if the entity went away, forget it
	if (!client->inuse)
		return;

	//i see myself??? this is a thing that makes no sense.
	if (client == monster)
	{
		return;
	}

	if (client->client)
	{
		if (client->flags & FL_NOTARGET || (OnSameTeam(monster,client) && !owner_ai.GetAbusedByTeam()))// i don't play with cheaters.
			return;
	}
	else// if (!client->ai)//no magically seeing other monsters!
	{
		return;
	}

	//simple dist check--i see all
	vec3_t to_seen;
	float toSeenDistSq;
	VectorSubtract(client->s.origin, monster->s.origin, to_seen);
	toSeenDistSq=VectorLengthSquared(to_seen);
	if (toSeenDistSq>distSq)
	{
		return;
	}

	//update appropriate slot for monster
	if (client->client)
	{
		sensed_client.ent = client;
		sensed_client.time = level.time;
		VectorCopy(client->s.origin, sensed_client.pos);
	}
	else
	{
		sensed_monster.ent = client;
		sensed_monster.time = level.time;
		VectorCopy(client->s.origin, sensed_monster.pos);
	}
}

magicsight_sense::magicsight_sense(magicsight_sense *orig)
: sight_sense(orig)
{
	distSq = orig->distSq;
}

void magicsight_sense::Evaluate(magicsight_sense *orig)
{
	distSq = orig->distSq;

	sight_sense::Evaluate(orig);
}

void magicsight_sense::Read()
{
	char	loaded[sizeof(magicsight_sense)];

	gi.ReadFromSavegame('AINS', loaded, sizeof(loaded));
	Evaluate((magicsight_sense *)loaded);
}

void magicsight_sense::Write()
{
	magicsight_sense	*savable;

	savable = new magicsight_sense(this);
	gi.AppendToSavegame('AINS', savable, sizeof(*this));
	delete savable;
}

void magicsight_sense::Mute(unsigned mask, float degree, smute_recovery recovery_code, float recovery_time)
{
	return;
}

//end