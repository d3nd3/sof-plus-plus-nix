/******************************************************
 * Objects for New York                               *
 ******************************************************/

#include "g_local.h"
#include "g_obj.h"

//  misc_newyork...
void SP_misc_newyork_burned_car (edict_t *ent);
void SP_misc_newyork_gumball (edict_t *ent);
void SP_misc_newyork_news_machine (edict_t *ent);
void SP_misc_newyork_trash_pile (edict_t *ent);
void SP_misc_newyork_trashcan (edict_t *ent);

#define HLTH_NY_BURNED_CAR			10000
#define HLTH_NY_GUMBALL				1000
#define HLTH_NY_NEWS_MACHINE		600
#define HLTH_NY_TRASH_PILE			300
#define HLTH_NY_TRASHCAN			300

float	s_lastTrashcanPainTime = 0;

spawn_t newyorkSpawns[] =
{
	// misc_newyork. . .
	{"misc_newyork_burned_car",			SP_misc_newyork_burned_car},
	{"misc_newyork_gumball",			SP_misc_newyork_gumball},
	{"misc_newyork_news_machine",		SP_misc_newyork_news_machine},
	{"misc_newyork_trash_pile",			SP_misc_newyork_trash_pile},
	{"misc_newyork_trashcan",			SP_misc_newyork_trashcan},
	{NULL,								NULL},
};

typedef enum
{
	OBJ_BURNED_CAR = 0,
	OBJ_GUMBALL,
	OBJ_NEWS_MACHINE,
	OBJ_TRASH_PILE,
	OBJ_TRASHCAN,
	MAX_OBJS
};


modelSpawnData_t nyModelData[MD_NY_SIZE] =
{	
//      dir							file				surfaceType			material			health						solid			material file  cnt  scale
"objects/newyork/burned_car",		"burned_car",		SURF_METAL,			MAT_METAL_RUSTY,	HLTH_NY_BURNED_CAR,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_BURNED_CAR
"objects/newyork/gumball",			"gumball",			SURF_METAL,			MAT_METAL_SHINY,	HLTH_NY_GUMBALL,			SOLID_BBOX,		"gumball",		0,	0.0,	NULL,	// OBJ_GUMBALL
"objects/newyork/news_machine",		"news",				SURF_METAL,			MAT_METAL_SHINY,	HLTH_NY_GUMBALL,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_NEWS_MACHINE
"objects/newyork/trash_pile",		"trash_pile",		SURF_METAL,			MAT_METAL_RUSTY,	HLTH_NY_TRASH_PILE,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_TRASH_PILE
"objects/newyork/trashcan",			"trashcan",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_NY_TRASHCAN,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_TRASHCAN
};


/*QUAKED misc_newyork_burned_car (1 .5 0) (-91 -33 -25) (91 33 25) INVULNERABLE  NOPUSH x x x x FLUFF
Burned out car.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, can't ever be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_newyork_burned_car (edict_t *ent)
{
	VectorSet (ent->mins, -91, -33, -25);
	VectorSet (ent->maxs,  91,  33,  25);
	
	SimpleModelInit2(ent,&nyModelData[OBJ_BURNED_CAR],NULL,NULL);
}


void gumball_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t	vec1, gumballorigin;
	vec3_t	debrisNorm;

	if (self->s.skinnum == 1)
	{
		return;
	}

	SetSkin(self,nyModelData[OBJ_GUMBALL].dir,nyModelData[OBJ_GUMBALL].file, 
		 nyModelData[OBJ_GUMBALL].materialfile,"gumball2", 0);

	VectorCopy(self->s.origin, gumballorigin);
	gumballorigin[2] += 10;
	
	VectorClear(debrisNorm);
	FX_ThrowDebris(gumballorigin,debrisNorm, 5,DEBRIS_SM, MAT_GLASS, 0,0,0, SURF_GLASS);

	VectorSet(vec1, gi.irand(-21,20), gi.irand(-21,20), 0);
	FX_MakeGumballs(gumballorigin, vec1, 0);

	VectorSet(vec1, gi.irand(-21,20), gi.irand(-21,20), 0);
	FX_MakeCoins(gumballorigin, vec1, 0);

	VectorSet(vec1, gi.irand(-21,20), gi.irand(-21,20), 0);
	FX_MakeGumballs(gumballorigin, vec1, 0);

	gi.sound (self, CHAN_ENT1, gi.soundindex("Impact/GlassBreak/GBSmall.wav"), .6, ATTN_NORM, 0);

	self->s.skinnum = 1;
	self->health = 1;
}

/*QUAKED misc_newyork_gumball (1 .5 0) (-8 -8 -19) (8 8 19)  INVULNERABLE  NOPUSH x x x x FLUFF
A gumball machine
------ KEYS ------
Skin - 
0 - intact, full of gumballs
1 - broken, no gumballs
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- when shot gumballs come out
*/
void SP_misc_newyork_gumball (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -8, -8, -19);
	VectorSet (ent->maxs, 8, 8, 19);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "gumball2";
		break;
	default:
		skinname = "gumball";
		break;
	}

	ent->pain = gumball_pain;

	SimpleModelInit2(ent,&nyModelData[OBJ_GUMBALL],NULL,NULL);

	SetSkin(ent,nyModelData[OBJ_GUMBALL].dir,nyModelData[OBJ_GUMBALL].file, 
		 nyModelData[OBJ_GUMBALL].materialfile,skinname, 0);

	// cache debris CLGHL_CHUNKGLASSSM
	entDebrisToCache[CLGHL_CHUNKGLASSSM] = DEBRIS_YES;
	gi.soundindex("Impact/GlassBreak/GBSmall.wav"); // precache sound
}

void news_machine_pain2 (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{

	FX_PaperCloud(self->s.origin, 3);
}

void news_machine_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t	forward,right,holdPos;
	vec3_t	debrisNorm;

	AngleVectors(self->s.angles, forward, right, NULL);
	VectorMA(self->s.origin, 10, forward, holdPos);

	FX_PaperCloud(holdPos, 3);
	VectorClear(debrisNorm);
	FX_ThrowDebris(holdPos,debrisNorm, 5,DEBRIS_SM, MAT_GLASS,0,0,0, SURF_GLASS);

	VectorMA(self->s.origin, 10, forward, holdPos);
	VectorMA(self->s.origin, 15, right, holdPos);
	holdPos[2] += 10;
	FX_MakeCoins(holdPos, forward, 0);

	SimpleModelSetSequence(self,nyModelData[OBJ_NEWS_MACHINE].file,SMSEQ_HOLD);

	self->pain = news_machine_pain2;

}

/*QUAK-ED misc_newyork_news_machine (1 .5 0) (-21 -35 -20) (21 12 19)  INVULNERABLE  NOPUSH  x x x x FLUFF
A newspaper machine
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- when shot front opens up and newspapers float out
*/
void SP_misc_newyork_news_machine (edict_t *ent)
{
	VectorSet (ent->mins, -21, -35, -20);
	VectorSet (ent->maxs,  21,  12,  19);

	ent->pain = news_machine_pain;
	SimpleModelInit2(ent,&nyModelData[OBJ_NEWS_MACHINE],NULL,NULL);

	ent->ghoulInst->Pause(level.time);

	// cache debris CLGHL_CHUNKGLASSSM, CLGHL_PAPER
	entDebrisToCache[CLGHL_CHUNKGLASSSM] = DEBRIS_YES;
	entDebrisToCache[CLGHL_PAPER] = DEBRIS_YES;
}


void trashcan_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	// don't perform pain stuff more often than 1 second
	if ((level.time - s_lastTrashcanPainTime) > 0.5)
	{
		// if we're hit in the flap, don't take damage
		if ((wherehit[2] - self->s.origin[2]) > 10)
		{
			// play through the can's flip/flop sequence once
			SimpleModelSetSequence(self,nyModelData[OBJ_TRASHCAN].file,SMSEQ_HOLD);

			// put our health back (so you can keep shooting the flap and you won't destroy the trashcan)
			self->health += damage;
			// play the flippity-flippity sound
			FX_TrashcanHit(self->s.origin);
		}
		s_lastTrashcanPainTime = level.time;
	}
}

void trashcan_death(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	FX_SmokePuff(self->s.origin,200, 200, 200, 250);
	FX_SmokePuff(self->s.origin,200, 200, 200, 250);
	FX_SmokePuff(self->s.origin,200, 200, 200, 250);
	SpecialBecomeDebris(self, inflictor, attacker, damage, point);
}

/*QUAKED misc_newyork_trashcan (1 .5 0) (-10 -10 -19) (10 10 20)  INVULNERABLE  NOPUSH  x x x x FLUFF
A "Pitch In" trash can
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- when shot on upper half, the top spins and generates papers
*/
void SP_misc_newyork_trashcan (edict_t *ent)
{
	VectorSet (ent->mins, -10, -10, -18);
	VectorSet (ent->maxs,  10,  10,  20);
	
	ent->die = trashcan_death;
	ent->pain = trashcan_pain;
	SimpleModelInit2(ent,&nyModelData[OBJ_TRASHCAN],NULL,NULL);
	// default for SimpleModelInit2() is to play the sequence looping. we don't want that.
	SimpleModelSetSequence(ent,nyModelData[OBJ_TRASHCAN].file,SMSEQ_HOLD);
	// cache sound CLSFX_TRASHCAN_FLIP
	entSoundsToCache[CLSFX_TRASHCAN_FLIP] = 1;

	// cache debris CLGHL_CAN, CLGHL_PAPER
	entDebrisToCache[CLGHL_CAN] = DEBRIS_YES;
	entDebrisToCache[CLGHL_PAPER] = DEBRIS_YES;
}

void trash_piece_think(edict_t *ent)
{
	G_FreeEdict(ent);	
}

void trash_pile_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	edict_t *trash;
	vec3_t boltPos;
	char *boltName,*objName;
	char soundName[60];
	int per;

	if (self->count == 0)
	{
		boltName = "DUMMY01";
		objName = "_CAN1";
	}
	else if (self->count == 1)
	{
		boltName = "DUMMY02";
		objName = "_CAN2";
	}
	else if (self->count == 2)
	{
		boltName = "DUMMY03";
		objName = "_SHOE";
	}
	else if (self->count == 3)
	{
		boltName = "DUMMY04";
		objName = "_OILCAN";
	}

	per = gi.irand(1,3);
	sprintf(soundName,"Weapons/FX/Rics/ric%d.wav",per);

	gi.sound (self, CHAN_VOICE, gi.soundindex(soundName), .6, ATTN_NORM, 0);

	// Remove part from main model
	SimpleModelRemoveObject(self,objName);


	// Spawn little piece
	trash = G_Spawn();

	VectorSet (trash->mins, -5, -5, -5);
	VectorSet (trash->maxs,  5,  5,  5);

	VectorCopy(self->s.origin,trash->s.origin);

	trash->s.origin[2] += 30;
	SimpleModelInit2(trash,&nyModelData[OBJ_TRASH_PILE],NULL,NULL);

	SimpleModelSetObject(trash,objName);

	trash->velocity[0] = gi.flrand(-200.0F, 200.0F);
	trash->velocity[1] = gi.flrand(-200.0F, 200.0F);
	trash->velocity[2] = gi.flrand(50.0F, 150.0F);

	trash->nextthink = level.time + gi.flrand(0.0F, 30.0F);
	trash->think = trash_piece_think;
	gi.linkentity (trash);

	GetGhoulPosDir(self->s.origin, self->s.angles, self->ghoulInst,
					   NULL, boltName, boltPos, NULL, NULL, NULL);

	FX_SmokePuff(boltPos,200, 200, 200, 250);

	++self->count;

	if (self->count == 4)
	{
		G_FreeEdict(self);	// Remove original trash
	}
}


/*QUAKED misc_newyork_trash_pile (1 .5 0) (-14 -8 0) (5 14 14)  INVULNERABLE  NOPUSH  x x x x FLUFF
Several beer/oil cans and a gas can.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- when shot one of the cans flys off
*/
void SP_misc_newyork_trash_pile (edict_t *ent)
{
	VectorSet (ent->mins, -10, -11, -6);
	VectorSet (ent->maxs,  10,  11,  18);

	ent->spawnflags |= SF_NOPUSH;

	ent->pain = trash_pile_pain;
	SimpleModelInit2(ent,&nyModelData[OBJ_TRASH_PILE],NULL,NULL);

}