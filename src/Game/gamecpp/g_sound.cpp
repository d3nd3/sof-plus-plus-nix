#include "g_local.h"
#include "ef_flags.h"
#include "music.h"

typedef unsigned char qlenString_t[MAX_QPATH];

#define MAX_GENERAL_SETS		256
#define MAX_LOCAL_SETS			256
#define MAX_BMODEL_SETS			256
#define MAX_AMBWAVES			1024
#define MAX_WAVES_PER_GROUP		16
#define NOWAVE_VAL				0xffff

#define CUR_DMS_FORMAT			11

typedef struct ambientGroup_s
{
	char			setName[MAX_QPATH];
	unsigned short	loopWaveNum;
	byte			numWavesUsed;
	unsigned short	wavesUsed[MAX_WAVES_PER_GROUP];
}ambientServerGroup_t;

typedef struct ambientInfo_s
{
	// loaded info from .ams

	qlenString_t			waveNames[MAX_AMBWAVES];
	ambientServerGroup_t	gen[MAX_GENERAL_SETS];
	ambientServerGroup_t	loc[MAX_LOCAL_SETS];
	ambientServerGroup_t	bmodel[MAX_BMODEL_SETS];
	int						numWaves;
	int						numGenSets;
	int						numLocalSets;
	int						numBModelSets;
}ambientServerInfo_t;

ambientServerInfo_t aSet;

static void getDataAfterString(char **string)
{
	while(**string != 0)
	{
		(*string)++;
	}
	(*string)++;
}

byte *LoadServerSets(byte *curPos, int numSets, ambientServerGroup_t *group)
{
	int i, j;

	if(numSets == 0xff)return curPos;//empty - has no sets...

	for(i = 0; i < numSets; i++)
	{
		strcpy(group[i].setName, (const char *)curPos);
		getDataAfterString((char **)&curPos);
		curPos+=10;
		group[i].loopWaveNum = *((unsigned short *)curPos);
		curPos+=2;
		group[i].numWavesUsed = *((unsigned short *)curPos);
		curPos+=2;
		for(j = 0; j < group[i].numWavesUsed; j++)
		{
			group[i].wavesUsed[j] = *((unsigned short *)curPos);
			curPos+=2;
		}
		curPos += 6;
	}

	return curPos;
}

void LoadAmbientInfo(void)
{
	byte	*setInfo;
	byte	*curPos;
	int		fileLen;
	char	version;
	int		i;

	fileLen = gi.FS_LoadFile ("sound/sound.ams", (void **)&setInfo);
	if (fileLen == -1)
	{
		Com_Printf("Unable to locate sound/sound.ams - all ambient sounds will be disabled\n");
		return;
	}

	curPos = setInfo;

	version = *curPos++;
	if(version != CUR_DMS_FORMAT)
	{
		Com_Printf ("Invalid version (%d) for Ambient Sound Set (Should be %d)\n", version, CUR_DMS_FORMAT);
		gi.FS_FreeFile(setInfo);
		return;
	}

	aSet.numWaves = *((unsigned long *)curPos);
	curPos +=4;
	aSet.numGenSets = *((unsigned long *)curPos);
	curPos +=4;
	aSet.numLocalSets = *((unsigned long *)curPos);
	curPos +=4;
	aSet.numBModelSets = *((unsigned long *)curPos);
	curPos +=4;

	for(i = 0; i < aSet.numWaves; i++)
	{
		strcpy((char *)aSet.waveNames[i], (const char *)curPos);
		getDataAfterString((char **)&curPos);
	}

	curPos = LoadServerSets(curPos, aSet.numGenSets, &aSet.gen[0]);
	curPos = LoadServerSets(curPos, aSet.numLocalSets, &aSet.loc[0]);
	curPos = LoadServerSets(curPos, aSet.numBModelSets, &aSet.bmodel[0]);

	gi.FS_FreeFile(setInfo);
}

// pretty much everything I'm gonna do in here should be moved and rearranged and stuff...

int FindSetsSounds(edict_t *ent, ambientServerGroup_t *group, int num)
{
	int i, j;

	for(i = 0; i < num; i++)
	{
		if(!stricmp(ent->soundName, group[i].setName))
		{

			if(group[i].loopWaveNum != NOWAVE_VAL)
			{
				gi.soundindex((char *)aSet.waveNames[group[i].loopWaveNum]);
			}

			//tell it to precache everything involved with this...
			for(j = 0; j < group[i].numWavesUsed; j++)
			{
				gi.soundindex((char *)aSet.waveNames[group[i].wavesUsed[j]]);
			}

			return 1;
		}
	}
	return 0;
}

void SndInitWorldSounds(void)
{
	edict_t *checkedEnt;

	LoadAmbientInfo();//so aSet should be nice and populated at this point...

/*
	// wouldn't this be easier? This way you don't miss any of the sounds inside of the ambient sound set
	// it would appear that the aim of all that is below, and the routine above is simply to register the sounds
	// referenced by the gen, loc and bmode sound set types, but since there are obviously some within the 
	// sound set that *aren't* referenced this way, we can either
	// a) tidy up the ambient sound sets so they *do* only contain sounds that are referenced or
	// b) The lazy way, just register everything. This will work for now, but is NOT a good answer
	for(int i = 0; i < aSet.numWaves; i++)
	{
		gi.soundindex((char *)aSet.waveNames[i]);
		
	}
*/

	for (checkedEnt = g_edicts; checkedEnt < &g_edicts[globals.num_edicts]; checkedEnt++)
	{
		if(!checkedEnt->inuse)
			continue;
		if(!checkedEnt->soundName)
			continue;
		if(!stricmp(checkedEnt->soundName, ""))
			continue;

		if(FindSetsSounds(checkedEnt, &aSet.gen[0], aSet.numGenSets))continue;
		if(FindSetsSounds(checkedEnt, &aSet.loc[0], aSet.numLocalSets))continue;
		if(FindSetsSounds(checkedEnt, &aSet.bmodel[0], aSet.numBModelSets))continue;
		
//		Com_Printf("Couldn't find a sound group \"%s\"\n", checkedEnt->soundName);
	}
}

int GetIDForSoundSet(char *setName, ambientServerGroup_t *group, int num)
{
	int i;

	if(!setName)
	{
//		Com_Printf("soundset with NULL set name!!!\n");
		return -1;
	}

	for(i = 0; i < num; i++)
	{
		if(!stricmp(setName, group[i].setName))
		{
			return i;
		}
	}
	return -1;//this is bad...
}

int SND_getBModelSoundIndex(char *name, int spot)
{
	int i;

	if(!name)return -1;

	if(!stricmp(name, ""))return -1;

	for(i = 0; i < MAX_BMODEL_SETS; i++)
	{
		if(!stricmp(name, aSet.bmodel[i].setName))
		{
			return gi.soundindex((char *)aSet.waveNames[aSet.bmodel[i].wavesUsed[spot]]);
		}
	}
	return -1;
}



























//fixme all - unicast vs multicast - what's up with this?
void SetMusicIntensity(edict_t *ent, int newLevel)
{
	assert(ent);
	assert(ent->client);
	assert((newLevel == MUSIC_ACTION)||(newLevel == MUSIC_WANDER));

	ent->client->ps.musicID = newLevel;
}

void StopMusic(edict_t *ent, short numSecs)
{
	assert(ent);
	assert(ent->client);

	ent->client->ps.musicID = MUSIC_NONE;
}

void RestartMusic(edict_t *ent)
{
	assert(ent);
	assert(ent->client);

	ent->client->ps.musicID = MUSIC_WANDER;//?
}

void StartMusic(edict_t *ent, byte songID)
{
	assert(ent);
	assert(ent->client);
	assert(songID < 250);//irk - because of offset

	ent->client->ps.musicID = songID + MUSIC_OFFSET;//?
}

void SetAmbientSoundSet (edict_t *ent, int setID)
{
	if(!ent)return;
	if(!ent->inuse)return;
	if(!ent->client)return;

	ent->client->ps.soundID = setID;
}

void SetAmbientSoundSet (edict_t *ent, char *setName)
{
	int id;

	id = GetIDForSoundSet(setName, &aSet.gen[0], MAX_GENERAL_SETS);
	if (id == -1 || !ent)
	{
		return;
	}

	if(!ent)return;
	if(!ent->inuse)return;
	if(!ent->client)return;

	ent->client->ps.soundID = id;
}

/*QUAKED func_ambientset (0 .5 .8) (-8 -8 -8) (8 8 8)
sets the current world ambient sound set to be played:
set soundName to be the name of the sound group you would like to use...
*/

void func_ambientSetUse(edict_t *ent, edict_t *other, edict_t *activator)
{
	SetAmbientSoundSet (activator, ent->soundName);
}

void SP_func_ambientSet (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	ent->clipmask = MASK_MONSTERSOLID|MASK_PLAYERSOLID; 
	VectorSet (ent->mins, -5, -5, -7);
	VectorSet (ent->maxs, 5, 5, 12);
	ent->s.modelindex = 0;
	ent->use = func_ambientSetUse;
	gi.linkentity (ent);

}

/*QUAKED func_musicControl (0 .5 .8) (-8 -8 -8) (8 8 8)
sets the current state of music...
"sounds"
	-2 = turn music off
	-1 = let game take over music again
	0-255 = ID number of specific song to be played...
*/

void func_musicControlUse(edict_t *ent, edict_t *other, edict_t *activator)
{
	assert(activator);
	assert(activator->client);

	if(ent->health == -2)
	{
		StopMusic(ent, ent->health);//fixme - I doubt this will work
	}
	else if(ent->health == -1)
	{
		RestartMusic(activator);
	}
	else
	{
		StartMusic(activator, ent->sounds);
	}
}

void SP_func_musicControl (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	ent->clipmask = MASK_MONSTERSOLID|MASK_PLAYERSOLID; 
	VectorSet (ent->mins, -5, -5, -7);
	VectorSet (ent->maxs, 5, 5, 12);
	ent->s.modelindex = 0;
	ent->use = func_musicControlUse;
	gi.linkentity (ent);
}

#define START_OFF 1
/*QUAKED environ_soundgen (0 1 0) (-12 -12 -12) (12 12 12) START_OFF
"soundName" name of the soundset to use...
*/

void sound_gen_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	if(ent->s.effects & EF_AMB_SOUND_SET)
	{
		ent->s.effects &= ~EF_AMB_SOUND_SET;
		ent->s.sound = 0;
	}
	else
	{
		int id;

		id = GetIDForSoundSet(ent->soundName, &aSet.loc[0], MAX_LOCAL_SETS);

		ent->s.effects |= EF_AMB_SOUND_SET;
		ent->s.sound = id;
		ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
	}
}

void soundGenInit(edict_t *ent)
{
	int id;

	id = GetIDForSoundSet(ent->soundName, &aSet.loc[0], MAX_LOCAL_SETS);

	if(id == -1)
	{
		Com_Printf("Soundgen with invalid name!\n");
		G_FreeEdict(ent);
		return;
	}

	ent->s.sound = id;
	ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
	ent->s.effects |= EF_AMB_SOUND_SET;

	ent->think = NULL;
	ent->nextthink = 0;
}

void SP_environ_soundgen (edict_t *ent)
{
	int id;

	id = GetIDForSoundSet(ent->soundName, &aSet.loc[0], MAX_LOCAL_SETS);

	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	ent->clipmask = MASK_MONSTERSOLID|MASK_PLAYERSOLID; 
	VectorSet (ent->mins, -5, -5, -7);
	VectorSet (ent->maxs, 5, 5, 12);
	ent->s.modelindex = 0;
	BboxRotate(ent);
	gi.linkentity (ent);

	if(!ent->spawnflags & START_OFF)
	{	// the sound info isn't loaded up until after everything is spawned in, so we have to wait a bit before it becomes valid...
		ent->think = soundGenInit;
		ent->nextthink = level.time + .3;
	}
	ent->use = sound_gen_use;
}

void SetActionMusic(edict_t &ent)
{
	assert(ent.client);
	if(!ent.client)return;

	if((ent.client->ps.musicID != MUSIC_ACTION)&&(ent.client->ps.musicID != MUSIC_WANDER)&&(ent.client->ps.musicID != MUSIC_UNSET))
	{	// no dynamic music active
		return;
	}

	ent.client->ps.musicID = MUSIC_ACTION;
	ent.client->musicTime = level.time;
}

int classifyMusicSituation(edict_t &ent)
{
	edict_t *search = NULL;
	int		val = 0;
	int		angryGuys = 0;

	CRadiusContent rad(ent.s.origin, 600);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		search = rad.foundEdict(i);

		if (search->ai && search->health > 0)
		{
			//use dist a bit too...
			if(search->ai->GetMood(*search) == EMOTION_MEAN)
			{
				val+=4;
				angryGuys = 1;
			}
			//else val++;

		}
	}
	if(ent.health < 50)val+=2;
	if(ent.health < 25)val+=2;
	if((val > 8)&&(angryGuys))return MUSIC_ACTION;
	return MUSIC_WANDER;
}

void UpdateMusicLevel(edict_t *ent)
{
	int	curSituation;

	assert(ent);
	assert(ent->client);

	if((ent->client->ps.musicID != MUSIC_ACTION)&&(ent->client->ps.musicID != MUSIC_WANDER)&&(ent->client->ps.musicID != MUSIC_UNSET))
	{	// no dynamic music active
		return;
	}

	if((ent->client->ps.musicID == MUSIC_ACTION) &&
		(ent->client->musicTime > level.time - MIN_ACTION_MUSIC_TIME))
	{
		return;//never change too quickly
	}

	curSituation = classifyMusicSituation(*ent);
	
	if(curSituation != ent->client->ps.musicID)
	{
		ent->client->ps.musicID = curSituation;
		ent->client->musicTime = level.time;
	}
}
