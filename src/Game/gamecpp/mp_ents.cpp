#include "g_local.h"
#include "ai_pathfinding.h"

#define DEFAULT(a, b) ((a) ? (a):(b))

//	deathmatch
void SP_dm_KOTH_targetzone (edict_t *ent);

spawn_t dmSpawns[] =
{
	{"dm_KOTH_targetzone",				SP_dm_KOTH_targetzone},
	{NULL,								NULL},
};

edict_t	*SV_TestEntityPosition (edict_t *ent);

void SetGenericInvisible(edict_t *ent)
{	// get some good defaults here...
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	ent->clipmask = MASK_MONSTERSOLID|MASK_PLAYERSOLID; 
	VectorSet (ent->mins, -5, -5, -7);
	VectorSet (ent->maxs, 5, 5, 12);
	ent->s.modelindex = 0;
	BboxRotate(ent);
	gi.linkentity (ent);
}

void SetGenericInvisVBBox(edict_t *ent)
{
	SetGenericInvisible(ent);

	gi.setmodel (ent, ent->model);
	ent->s.modelindex = 0;

	ent->s.origin[0] = (ent->absmax[0] + ent->absmin[0])*.5;
	ent->s.origin[1] = (ent->absmax[1] + ent->absmin[1])*.5;
	ent->s.origin[2] = (ent->absmax[2] + ent->absmin[2])*.5;
}


//----------------------------------------------------------------------------------------------------

/*QUAKED dm_KOTH_targetzone (0 1 0) ? START_OFF
*/

void SP_dm_KOTH_targetzone (edict_t *ent)
{
	SetGenericInvisVBBox(ent);

	//blah...
}

/*QUAKED spawner (0 1 0) (-16 -16 -32) (16 16 32) START_OFF SINGLE_TYPE
"soundName" door sound to be played when spawner is activated
"count"		number of ents to be spawned at a single time (default 1)
"delay"		frequency to spawn an ent in normal circumstances (default 30)
"wait"		frequency to spawn an ent in triggered/active circumstances (default 10)
"mass"		maximum number of ents owned by this trigger that can be in the world at once (default 1) ... -1 is unlimited
"health"	number of items to spawn before removing - -1 is unlimited (default -1)
"lip"		minimum radius to spawn guys in when you're not facing the spawner (defaults to 450)
"spawn1"
...
"spawn3"		name of the ent to spawn from the ent list
*/

/*QUAKED spawner_monster (0 1 0) (-16 -16 -32) (16 16 32) START_OFF SINGLE_TYPE TARGET_PLAYER IGNORE_VISIBLITY IGNORE_LIMITS
"target"	if this is set, this will be run when the spawner is out of guys
"soundName" door sound to be played when spawner is activated
"count"		number of ents to be spawned at a single time (default 1)
"delay"		frequency to spawn an ent in normal circumstances (default 30) - set to -1 to NEVER spawn in non-alert mode
"wait"		frequency to spawn an ent in triggered/active circumstances (default 10)
"mass"		maximum number of ents owned by this trigger that can be in the world at once (default 1) ... -1 is unlimited
"health"	number of items to spawn before removing - -1 is unlimited (default -1)
"lip"		minimum radius to spawn guys in when you're not facing the spawner (defaults to 450)
"spawn1"
...
"spawn3"		name of the ent to spawn from the ent list

TARGET_PLAYER makes spawned guys default to attacking the player immediately
*/

/*QUAKED spawner_boosterpack (0 1 0) (-16 -16 -32) (16 16 32)
"health"	number of items to spawn before removing - default is 8
"spawn1"
...
"spawn3"		name of the ent to spawn from the ent list
*/


#define START_OFF 1
#define SINGLE_TYPE 2
#define TARGET_PLAYER 4
#define IGNORE_VISIBLITY 8
#define IGNORE_LIMITS 16
#define ACTIVE_BOOSTER 32
#define IS_ENEMY 64
#define IS_BOOSTER 128
#define USEDBOOSTER 256

int canSpawnEnemies(void)
{
	//fixme - what SHOULD this be limited to?

	if(gmonster.GetNumNearbyEnemies() >= 7)
	{
		return 0;
	}
	return 1;
}

float PlayersRangeFromSpot (edict_t *spot);

int spawnDistValid(edict_t *ent)
{
	int val = PlayersRangeFromSpot(ent);

	// Is this reasonable?

	if(val > 768)return 0;
	return 1;
}

int visibleToPlayer(edict_t *spawner)
{
	if(!level.sight_client)
	{
		return 0;
	}

	if(spawner->spawnflags & IGNORE_VISIBLITY)
	{
		return 0;
	}

	if((spawner->spawnflags & (IS_BOOSTER|USEDBOOSTER)) == (IS_BOOSTER|USEDBOOSTER))
	{
		return 1;//these guys are no good
	}

	vec3_t dif;
	vec3_t lookDir;

	VectorSubtract(spawner->s.origin, level.sight_client->s.origin, dif);
	AngleVectors(level.sight_client->client->ps.viewangles, lookDir, 0, 0);

	if(((DotProduct(dif, lookDir) < 0)&&(DotProduct(dif, dif) > spawner->gib_health*spawner->gib_health))
		||(!gi.inPVS(level.sight_client->s.origin, spawner->s.origin)))
	{	// only if far away does the behind-check work
		return 0;
	}

	if(spawner->spawnflags & IS_BOOSTER)
	{
		trace_t tr;

		gi.trace(spawner->s.origin, vec3_origin, vec3_origin, level.sight_client->s.origin, level.sight_client, MASK_SOLID, &tr);
		if(tr.fraction < .99)
		{
			return 0;
		}
		else
		{
			spawner->spawnflags |= USEDBOOSTER;
		}
	}
	return 1;
}

void spawnerUse(edict_t *ent, edict_t *other, edict_t *activator)
{
	if(ent->spawnflags & IS_BOOSTER)
	{	//these are one shots - they don't get turned off
		ent->spawnflags |= ACTIVE_BOOSTER;
	}
	else
	{
		if(ent->nextthink > 0)
		{
			ent->nextthink = 0;
		}
		else
		{
			ent->nextthink = level.time + 1.0;
		}
	}
}

int curSpawnCountIsTooLarge(edict_t *ent, int testVal)
{
	int num = 0;
	edict_t	 *curTest = g_edicts;

	if(testVal == -1)return 0;

	for ( curTest = g_edicts; curTest < &g_edicts[globals.num_edicts] ; curTest++)
	{
		if (!curTest->inuse)continue;
		if (!(curTest->flags & FL_SPAWNED_IN))continue;
		if ((curTest->ai)&&(curTest->health <= 0))continue;

		if(curTest->spawnerID == ent - g_edicts)num++;
		if(num >= testVal)return 1;
	}

	return 0;
}

int IsNextSpawnTime(edict_t *ent)
{
	if(ent->spawnflags & IS_BOOSTER)
	{
		return 1;//it's always... booster time!
	}
	if(level.alertedStatus)
	{
		if(ent->wait == -1)
		{
			return 0;
		}
		else
		{
			return (level.time - ent->health > ent->wait);
		}
	}
	else
	{
		if(ent->delay == -1)
		{
			return 0;
		}
		else
		{
			return (level.time - ent->health > ent->delay);
		}
	}

	return 0;//msdev is a ninny
}

void debug_drawbox(edict_t* self,vec3_t vOrigin, vec3_t vMins, vec3_t vMaxs, int nColor);

int getValidSpawnPoint(edict_t *ent, vec3_t outspot, edict_t **outGuy)
{
	edict_t *test;

	int numSpots = 0;

	for (test = ent; test; test = test->teamchain)numSpots++;//get the number in the chain

	edict_t *start;
	int offset = gi.irand(0, numSpots - 1);

	start = ent;
	while(offset)
	{
		start = start->teamchain;
		offset--;
	}

	test = start;
	for (int i = 0; i < numSpots; i++)
	{
		test = test->teamchain;
		if(!test)
		{
			test = ent;//restart from the beginning
		}

		if(!spawnDistValid(test))
		{	// must be within a certain range
			continue;
		}
		if(visibleToPlayer(test))
		{
			continue;
		}

		vec3_t spotAdd;
		VectorCopy(test->s.origin, spotAdd);
		spotAdd[2] += 8.0;
		trace_t trace;

		vec3_t down;
		VectorCopy(test->s.origin, down);
		down[2] -= 256;
		gi.trace(test->s.origin, vec3_origin, vec3_origin, down, ent, MASK_SOLID, &trace);
		VectorCopy(trace.endpos, spotAdd);
		spotAdd[2] += 12;

		vec3_t testMin = {-16, -16, 0};
		vec3_t testMax = {16, 16, 73};
		gi.trace (spotAdd, testMin, testMax, spotAdd, 0, MASK_MONSTERSOLID, &trace);
		
		if(trace.startsolid)
		{
			continue;
		}

		*outGuy = test;
		VectorCopy(spotAdd, outspot);
		return 1;
	}
	return 0;
}

void spawnerThink(edict_t *ent)
{
	edict_t		*newEnt;
	int			i;
	char		*newEntClass;

	if(ent->flags & FL_TEAMSLAVE)
	{	// no thinking for you!
		ent->nextthink = 0;
		return;
	}

/*	if(ent->s.sound)
	{
		if (ent->moveinfo.sound_end != -1)
			gi.sound (ent, CHAN_NO_PHS_ADD+CHAN_VOICE, ent->moveinfo.sound_end, .8, .4, 0);
		ent->s.sound = 0;
	}*/

	if ((!ent->spawn1) && (!ent->spawn2) && (!ent->spawn3))
	{
		gi.dprintf("spawner set to spawn nothing!\n");
		ent->nextthink=level.time+99999.9;
		return;
	}

	ent->nextthink = level.time + 1.0;

	if(ent->max_health == 0)
	{
		// if I have a target name, I need to trigger it.  yeah.
		edict_t *test;
		int free = 1;
		for (test = ent; test; test = test->teamchain)
		{
			if(test->killtarget)
			{
				if(!curSpawnCountIsTooLarge(ent, 1))
				{	// this means that additionally I have no childrenses
					if(ei_show->value)Com_Printf("Level spawner running kill target\n");

					ent->count = 0;
					ent->delay = 0;
					ent->wait = 0;
					ent->mass = 0;
					ent->health = 0;
					ent->target = ent->killtarget; //don't really wanna kill
					ent->killtarget = 0;

					G_UseTargets(ent, ent);//activator?  hrm.
				}
				else
				{
					free = 0;
				}
			}
		}

		if(free)
		{
			edict_t *next;
			for(test = ent; test; test = next)
			{
				next = test->teamchain;
				G_FreeEdict(test);
			}
			return;
		}
	}

	if(!IsNextSpawnTime(ent))
	{
		return;
	}

	if((!canSpawnEnemies()) && (!(ent->spawnflags & IGNORE_LIMITS)))
	{	// must not have too many active enemies or too many enemies nearby
		if(ei_show->value)Com_Printf("Level spawner can't spawn - too many enemies nearby\n");
		ent->nextthink = level.time + 1.0;
		return;
	}


	for(i = 0; (i < ent->count) && ent->max_health; i++)
	{
		if(curSpawnCountIsTooLarge(ent, ent->mass) || game.cinematicfreeze)//no spawn during cine's!!!
		{
			if(ei_show->value)Com_Printf("Level spawner can't spawn - either has too many kids or is in cinematics\n");
			ent->nextthink = level.time + 1.0;
			return;
		}

		vec3_t outspot;
		edict_t *spotEnt = 0;
		if(!getValidSpawnPoint(ent, outspot, &spotEnt))
		{
			if(ei_show->value)Com_Printf("Level spawner can't spawn - no valid spots\n");
			ent->nextthink = level.time + 1.0;
			return;
		}

		newEnt = G_Spawn();
		//positional stuff?
		if((i == 0)||(!(ent->spawnflags & SINGLE_TYPE)))
		{
			do
			{
				switch(gi.irand(0, 2))
				{
				case 0:
					newEntClass = ent->spawn1;
					break;
				case 1:
					newEntClass = ent->spawn2;
					break;
				case 2:
					newEntClass = ent->spawn3;
					break;
				}
			}while((!newEntClass)||(!strcmp(newEntClass, "")));
		}
		newEnt->classname = newEntClass;
		VectorCopy(outspot, newEnt->s.origin);//?? this can't work yet
		newEnt->s.origin[2] += 35.0;//eh
		//newEnt->s.origin[2] += 96.0;
		VectorCopy(spotEnt->s.angles, newEnt->s.angles);
		newEnt->target = spotEnt->target;

		VectorSet(newEnt->mins, -16, -16, -32);
		VectorSet(newEnt->maxs, 16, 16, 41);

		int oldmask = newEnt->clipmask;
		newEnt->clipmask = MASK_MONSTERSOLID;

		//make certain that the spot isn't already occupied..
		if(SV_TestEntityPosition(newEnt))
		{
			G_FreeEdict(newEnt);
			ent->nextthink = level.time + 1.0;
			if(ei_show->value)Com_Printf("Level spawner can't spawn - space is occupied\n");
			return;//eh?
		}

		ED_CallSpawn (newEnt);

		if (newEnt->inuse)
		{
			newEnt->spawnerID = ent - g_edicts;

			newEnt->clipmask = oldmask;
			newEnt->flags |= FL_SPAWNED_IN;
			newEnt->ai->Activate(*newEnt);
			newEnt->ai->SetPriority(PRIORITY_HIGH);

			if((ent->spawnflags & IS_BOOSTER) || (ent->spawnflags & TARGET_PLAYER))
			{
				//fixme - they should really come out to the person who spotted the guy
				PlayerNoise(level.sight_client, newEnt->s.origin, AI_SENSETYPE_SOUND_WAKEUP, 0, 90, 7);
				newEnt->ai->SetTargetTime(level.time, level.sight_client, level.sight_client->s.origin);
			}

			//if (spotEnt->moveinfo.sound_start != -1)
			//	gi.sound (spotEnt, CHAN_NO_PHS_ADD+CHAN_VOICE, spotEnt->moveinfo.sound_start, .8, .4, 0);
			/*if (spotEnt->moveinfo.sound_middle != -1)
			{
				spotEnt->s.sound = spotEnt->moveinfo.sound_middle;
				spotEnt->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
			}*/

			if(ent->max_health > 0)
			{	//max health is the total number of guys the spawner can ever spawn...
				ent->max_health--;
			}


			ent->health = level.time;
			if(level.alertedStatus)
			{
				ent->nextthink = level.time + ent->wait;
			}
			else
			{
				ent->nextthink = level.time + ent->delay;
			}
			if(ei_show->value)Com_Printf("Level spawner spawned a guy\n");
		}
		else
		{
			Com_Printf("Couldn't spawn entity type %s!\n", newEntClass);
		}
	}
}

void spawnerInit(edict_t *ent)
{
	ent->moveinfo.sound_start = SND_getBModelSoundIndex(ent->soundName, 0);
	ent->moveinfo.sound_middle = SND_getBModelSoundIndex(ent->soundName, 1);
	ent->moveinfo.sound_end = SND_getBModelSoundIndex(ent->soundName, 2);

	ent->think = spawnerThink;
	spawnerThink(ent);
}

void SP_spawner(edict_t *ent)
{
	SetGenericInvisible(ent);
	ent->think = spawnerInit;
	ent->use = spawnerUse;
	ent->nextthink = level.time + 1.0;
	ent->delay = DEFAULT(ent->delay, 30);
	ent->wait = DEFAULT(ent->wait, 10);
	ent->count = DEFAULT(ent->count, 1);
	ent->mass = DEFAULT(ent->mass, 1);
	ent->max_health = DEFAULT(ent->health, -1);
	ent->gib_health = DEFAULT(st.lip, 450);
	ent->health = level.time;//health is the  last time the spawner was used...
	ent->classname = "spawner";
	if(ent->spawnflags & START_OFF)ent->nextthink = 0;

	for(int i = 0; i < 3; i++)
	{
		edict_t *newEnt = G_Spawn();
		char *newEntClass;
		
		switch(i)
		{
		case 0:
			newEntClass = ent->spawn1;
			break;
		case 1:
			newEntClass = ent->spawn2;
			break;
		case 2:
			newEntClass = ent->spawn3;
			break;
		}
		
		if(!newEntClass || (!(*newEntClass)))
		{
			continue;
		}

		newEnt->classname = newEntClass;
		VectorCopy(ent->s.origin, newEnt->s.origin);//?? this can't work yet
		ED_CallSpawn (newEnt);
		G_FreeEdict(newEnt);
	}
}

void SP_spawner_monster(edict_t *ent)
{
	SP_spawner(ent);
	ent->spawnflags |= IS_ENEMY;
}

/*"target"	if this is set, this will be run when the spawner is out of guys
"soundName" door sound to be played when spawner is activated
"count"		number of ents to be spawned at a single time (default 1)
"delay"		frequency to spawn an ent in normal circumstances (default 30) - set to -1 to NEVER spawn in non-alert mode
"wait"		frequency to spawn an ent in triggered/active circumstances (default 10)
"mass"		maximum number of ents owned by this trigger that can be in the world at once (default 1) ... -1 is unlimited
"lip"		minimum radius to spawn guys in when you're not facing the spawner (defaults to 450)
"spawn1"
...
"spawn3"		name of the ent to spawn from the ent list
*/

void spawner_booster_defaultthink(edict_t *ent)
{	//make certain we haven't been seen
	if(ent->spawnflags & USEDBOOSTER)
	{
		ent->nextthink = 0;//all done
		return;
	}

	if(ent->spawnflags & ACTIVE_BOOSTER)
	{
		spawnerThink(ent);
	}

	ent->nextthink = level.time + .4;

	if(!level.sight_client)
	{
		return;
	}

	vec3_t dif;
	vec3_t lookDir;

	VectorSubtract(ent->s.origin, level.sight_client->s.origin, dif);
	AngleVectors(level.sight_client->client->ps.viewangles, lookDir, 0, 0);

	if(!gi.inPVS(level.sight_client->s.origin, ent->s.origin))
	{
		return;
	}

	trace_t tr;

	gi.trace(ent->s.origin, vec3_origin, vec3_origin, level.sight_client->s.origin, level.sight_client, MASK_SOLID, &tr);
	if(tr.fraction < .99)
	{
		return;
	}

	ent->spawnflags |= USEDBOOSTER;
}

void SP_spawner_boosterpack(edict_t *ent)
{
	ent->health = DEFAULT(ent->health, 8);
	SP_spawner(ent);
	ent->spawnflags |= IS_ENEMY|IS_BOOSTER;

	ent->count = 1;
	ent->delay = 1;
	ent->wait = 1;
	ent->mass = 20;

	ent->nextthink = level.time + .4;
	ent->think = spawner_booster_defaultthink;
}

















//******************************************************************************
//
//						World Spawning Stuff
//
//******************************************************************************


// amount every 10th of a second to go down = this equals 20 seconds to go from full to nothing
#define SPAWN_CAP 1.0
#define SPAWN_CHECKRAD 600
#define SPAWN_EXCITE_LEVEL .4

edict_t	*SV_TestEntityPosition (edict_t *ent);

bool HandleWorldSpawning(vec3_t origin, edict_t &ent, vec3_t startLookAng, int allowBehind)
{
	vec3_t spot;
	CRadiusContent rad(origin, SPAWN_CHECKRAD, 1, 0, 1);
	if(rad.getNumFound() > game.playerSkills.getSpawnMaxGuys() + 1)
	{
		if(ei_show->value)Com_Printf("World can't spawn - too many guys nearby\n");
		return false;//one of these is the player
	}

	vec3_t lookVect;

	AngleVectors(ent.s.angles, lookVect, 0, 0);

	//if(!aiPoints.getNearNonvisible(gmonster.GetClientNode(), spot, 640, lookVect, ent.s.origin))
	if(!aiPoints.getSpawnPosition(spot, allowBehind))
	{
		if(ei_show->value)Com_Printf("World can't spawn - no spots available for spawning\n");
		return false;
	}

	trace_t trace;
	vec3_t testMin = {-16, -16, -32};
	vec3_t testMax = {16, 16, 41};
	gi.trace (spot, testMin, testMax, spot, 0, MASK_MONSTERSOLID, &trace);
	
	if(trace.startsolid)
	{
		if(ei_show->value)Com_Printf("World spawn trying to spawn, but spot is occupied\n");
		return false;
	}

	if ((!world->spawn1) && (!world->spawn2) && (!world->spawn3))
	{
		return false;
	}

	char *spawnName;

	do
	{
		switch(gi.irand(0, 2))
		{
		case 0:
			spawnName = world->spawn1;
			break;
		case 1:
			spawnName = world->spawn2;
			break;
		case 2:
			spawnName = world->spawn3;
			break;
		}
	}while((!spawnName)||(!strcmp(spawnName, "")));

	if((!spawnName)||(!strcmp(spawnName, "")))
	{
		if(ei_show->value)Com_Printf("World spawn trying to spawn, but no fields set\n");
		return false;
	}

	edict_t *newEnt = G_Spawn();

	newEnt->classname = spawnName;
	VectorCopy(spot, newEnt->s.origin);
	newEnt->s.origin[2] += 8;
	ED_CallSpawn (newEnt);

	//irk!  This was placed before callspawn!
	if(!newEnt->ai)
	{	//er... huh?
		G_FreeEdict(newEnt);
		return false;
	}

	vec3_t dif;
	VectorSubtract(origin, newEnt->s.origin, dif);
	dif[2] = 0;
	VectorNormalize(dif);
	vectoangles(dif, newEnt->s.angles);

//	newEnt->ai->SetTargetTime(level.time, &ent, origin);

//	PlayerNoise(level.sight_client, newEnt->s.origin, AI_SENSETYPE_SOUND_WAKEUP, 0, 90, 7);
//	newEnt->ai->SetTargetTime(level.time, level.sight_client, level.sight_client->s.origin);


//	newEnt->ai->setFirstTargetTime(level.time + 3.0);//spawned guys take a bit longer to get warmed up

	int oldmask = newEnt->clipmask;
	newEnt->clipmask = MASK_MONSTERSOLID;

	//make certain that the spot isn't already occupied..
	//this doesn't appear to do anything even close to working
	if(SV_TestEntityPosition(newEnt))
	{
		if(ei_show->value)Com_Printf("World spawn trying to spawn, but spot is occupied\n");
		G_FreeEdict(newEnt);
		return false;//eh?
	}

	level.lastSpawn = level.time;
	newEnt->clipmask = oldmask;

	newEnt->flags |= FL_SPAWNED_IN;

	newEnt->ai->SetPriority(PRIORITY_HIGH);

	if(ei_show->value)
	{
		Com_Printf("World spawned in a guy\n");
		gi.sound (newEnt, CHAN_AUTO, gi.soundindex ("Weapons/mpg/fire.wav"), 1, 0, 0);
	}
	return true;
}


void IncreaseSpawnIntensity(float amount)
{
	if(level.spawnRushTime + game.playerSkills.getSpawnLullTime() > level.time)//fixme - you know the drill
	{
		//don't bother here
	}
	else
	{
		float spawnMul = ai_spawnfrequency->value * game.playerSkills.getSpawnFreq();
		if(spawnMul < .1)
		{
			spawnMul = .1;
		}
		//.4 is because I'm slowing both the rising and the falling
		level.playerLoudness += amount * .7 * spawnMul;
		if(level.playerLoudness > SPAWN_CAP)level.playerLoudness = SPAWN_CAP;
		if(ai_spawnfrequency->value < .1 || (game.playerSkills.getSpawnFreq() < .1))
		{
			if(level.playerLoudness > SPAWN_CAP * .4)
			{
				level.playerLoudness = SPAWN_CAP * .4;
			}
		}
	}
}

void UpdateWorldSpawningByFrame(void)
{
	int spawn = 0;

	if((!(level.sight_client)))
	{	// no spawning during any of the above cases
		return;
	}

	if((ai_spawnfrequency->value <= 0.01)||
		(game.playerSkills.getSpawnFreq() <= 0.01)||
		dm->isDM()||
		(game.cinematicfreeze))
	{	// no spawning during any of the above cases
		level.playerLoudness -= ((.003 + (.015 * level.playerLoudness)))*.7;//wacky - peaks drop quicker
		return;
	}

	if(level.spawnRushTime > level.time)
	{//overwhelm the player for getting too loud
//		Com_Printf("Rushing!!!\n");

		level.playerLoudness = SPAWN_CAP;

		PlayerNoise(level.sight_client, level.sight_client->s.origin, AI_SENSETYPE_SOUND_WEAPON, NULL, 800, gmonster.GetClientNode());

		if(level.spawnSoundTime + .75 < level.time)
		{
			level.spawnSoundTime = level.time;
			gi.sound (level.sight_client, CHAN_AUTO, gi.soundindex ("misc/padd/max.wav"), 1.0, 0, 0);
		}

		if(level.time> level.lastSpawn + game.playerSkills.getSpawnForceTime())//fixme - make skill level dependent
		{
			bool didSpawn = HandleWorldSpawning(level.sight_client->s.origin,
				*level.sight_client, level.sight_client->s.angles, 1);
		}
	}
	else
	{
		if(level.spawnRushTime + game.playerSkills.getSpawnLullTime() > level.time)//fixme - you know the drill
		{//give the player a breather, poor guy
//			Com_Printf("Resting.\n");
/*			if(level.spawnSoundTime + 1.0 < level.time)
			{
				level.spawnSoundTime = level.time;
				gi.sound (level.sight_client, CHAN_AUTO, gi.soundindex ("misc/padd/max.wav"), .05, 0, 0);
			}*/
			level.playerLoudness = (1.0 - (level.time - level.spawnRushTime)
				/game.playerSkills.getSpawnLullTime())*SPAWN_CAP;
		}
		else
		{

			int peakHit = 0;

			if(level.playerLoudness == SPAWN_CAP)
			{
				peakHit = 1;
			}

			// update the primary variable
			//.4 is because I'm slowing both the rising and the falling
			level.playerLoudness -= ((.003 + (.015 * level.playerLoudness)))*.7;//wacky - peaks drop quicker
			if(level.playerLoudness < 0)
			{
				level.playerLoudness = 0;
			}

			if(peakHit)
			{
				level.spawnRushTime = level.time + game.playerSkills.getSpawnNonLullTime();//fixme - make skill level dependant
			}

			if(level.playerLoudness > SPAWN_CAP * .75)
			{
				float vol = (level.playerLoudness - (SPAWN_CAP * .75))/(SPAWN_CAP * .25);//this yields a range of 0 to 1
				float pause = 1.0 - .8 * vol;
				vol = .4;

				if(level.spawnSoundTime + pause < level.time)
				{
					level.spawnSoundTime = level.time;
					gi.sound (level.sight_client, CHAN_AUTO, gi.soundindex ("misc/padd/warning.wav"), vol, 0, 0);
				}
				if(level.time> level.lastSpawn + game.playerSkills.getSpawnMinTime())//fixme - make skill level dependent
				{
					bool didSpawn = HandleWorldSpawning(level.sight_client->s.origin,
						*level.sight_client, level.sight_client->s.angles, 0);
				}

			}
			else if(level.playerLoudness > SPAWN_CAP * .5)
			{
				float vol = 2 * (level.playerLoudness - (SPAWN_CAP * .5))/(SPAWN_CAP * .5);//this yields a range of 0 to 1
				float pause = 3.0 - 2.0 * vol;
				vol = .4;
				if(level.spawnSoundTime + pause < level.time)
				{
					level.spawnSoundTime = level.time;
					gi.sound (level.sight_client, CHAN_AUTO, gi.soundindex ("misc/padd/warning.wav"), vol, 0, 0);
				}
				if(level.time> level.lastSpawn + game.playerSkills.getSpawnMinTime())//fixme - make skill level dependent
				{
					bool didSpawn = HandleWorldSpawning(level.sight_client->s.origin,
						*level.sight_client, level.sight_client->s.angles, 0);
				}
			}
		}
	}
}
