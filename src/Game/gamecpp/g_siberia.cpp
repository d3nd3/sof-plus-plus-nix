/******************************************************
 * Objects for Siberia                              *
 ******************************************************/

#include "g_local.h"
#include "g_obj.h"

//  For snowcat
#define	SNOWCAT_LIGHTS			4

#define	FLARE_STARTOFF			4
#define	FLARE_BLUE				8


#define HLTH_GEN_LIGHT_BEAM				250	
#define HLTH_SIBERIA_CARTON1			500
#define HLTH_SIBERIA_CARTON2			10000
#define HLTH_SIBERIA_CARTON3			10000
#define HLTH_SIBERIA_CARTON4			10000
#define HLTH_SIBERIA_FLAG				500
#define HLTH_SIBERIA_ICICLES			10000
#define HLTH_SIBERIA_ROADFLARE			500
#define HLTH_SIBERIA_SEARCHLIGHT		500
#define HLTH_SIBERIA_SNOWCAT			10000

//  misc_siberia...
void SP_misc_siberia_carton1 (edict_t *ent);
void SP_misc_siberia_carton2 (edict_t *ent);
void SP_misc_siberia_carton3 (edict_t *ent);
void SP_misc_siberia_carton4 (edict_t *ent);
void SP_misc_siberia_flag (edict_t *ent);
void SP_misc_siberia_icicles (edict_t *ent);
void SP_misc_siberia_roadflare (edict_t *ent);
void SP_misc_siberia_searchlight (edict_t *ent);
void SP_misc_siberia_snowcat (edict_t *ent);

spawn_t siberiaSpawns[] =
{
	// misc_siberia. . .
	{"misc_siberia_carton1",			SP_misc_siberia_carton1},
	{"misc_siberia_carton2",			SP_misc_siberia_carton2},
	{"misc_siberia_carton3",			SP_misc_siberia_carton3},
	{"misc_siberia_carton4",			SP_misc_siberia_carton4},
	{"misc_siberia_flag",				SP_misc_siberia_flag},
	{"misc_siberia_icicles",			SP_misc_siberia_icicles},
	{"misc_siberia_roadflare",			SP_misc_siberia_roadflare},
	{"misc_siberia_searchlight",		SP_misc_siberia_searchlight},
	{"misc_siberia_snowcat",			SP_misc_siberia_snowcat},
	
	{NULL,								NULL},
};


typedef enum
{
	OBJ_CARTON1 = 0,
	OBJ_CARTON2,
	OBJ_CARTON3,
	OBJ_CARTON4,
	OBJ_FLAG,
	OBJ_ICICLES,
	OBJ_LIGHT_BEAM,
	OBJ_ROADFLARE,
	OBJ_SEARCHLIGHT,
	OBJ_SEARCHLIGHTARM,
	OBJ_SNOWCAT,
	MAX_OBJS
};


modelSpawnData_t siberiaModelData[MD_SIBERIA_SIZE] =
{	
//      dir							file				surfaceType			material			health						solid			material file  cnt  scale
"objects/siberia/carton1",			"carton1",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_SIBERIA_CARTON1,		SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_CARTON1
"objects/siberia/carton2",			"carton2",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_SIBERIA_CARTON2,		SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_CARTON2
"objects/siberia/carton3",			"carton3",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_SIBERIA_CARTON3,		SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_CARTON3
"objects/siberia/carton4",			"carton4",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_SIBERIA_CARTON4,		SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_CARTON4
"objects/siberia/flag",				"flag",				SURF_DEFAULT,		MAT_DEFAULT,		HLTH_SIBERIA_FLAG,			SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_FLAG
"objects/siberia/icicles",			"icicles",			SURF_DEFAULT,		MAT_DEFAULT,		HLTH_SIBERIA_ICICLES,		SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_ICICLES
"objects/generic/beam",				"beam",				SURF_NONE,			MAT_NONE,			HLTH_GEN_LIGHT_BEAM,		SOLID_NOT,		NULL,			0,	0.0,		NULL,	// OBJ_LIGHT_BEAM
"objects/siberia/flare",			"null",				SURF_NONE,			MAT_NONE,			HLTH_SIBERIA_ROADFLARE,		SOLID_NOT,		NULL,			0,	0.0,		NULL,	// OBJ_ROADFLARE
"objects/siberia/searchlight",		"lamp",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_SIBERIA_SNOWCAT,		SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_SEARCHLIGHT
"objects/siberia/searchlight",		"arm",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_SIBERIA_SNOWCAT,		SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_SEARCHLIGHT_ARM
"objects/siberia/snowcat",			"snowcat",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_SIBERIA_SNOWCAT,		SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_SNOWCAT
};



/*QUAKED misc_siberia_carton1 (1 .5 0) (-15 -15 -15) (15 15 15)  INVULNERABLE  NOPUSH x x x x FLUFF
A funky looking carton. 30 x 30 x 30
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_siberia_carton1 (edict_t *ent)
{
	VectorSet (ent->mins, -15, -15, -15);
	VectorSet (ent->maxs, 15, 15, 15);

	SimpleModelInit2(ent,&siberiaModelData[OBJ_CARTON1],NULL,NULL);
}


/*QUAKED misc_siberia_carton2 (1 .5 0) (-8 -8 -7) (8 8 7)  INVULNERABLE  NOPUSH x x x x FLUFF
A square carton.  16 x 16 x 14
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_siberia_carton2 (edict_t *ent)
{
	VectorSet (ent->mins, -8, -8, -7);
	VectorSet (ent->maxs,  8,  8,  7);

	SimpleModelInit2(ent,&siberiaModelData[OBJ_CARTON2],NULL,NULL);

}


/*QUAKED misc_siberia_carton3 (1 .5 0) (-3 -3 -4) (3 3 4)  INVULNERABLE  NOPUSH  x x x x FLUFF
A tiny carton - looks like a soup thermos.  6 x 6 x 8
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_siberia_carton3 (edict_t *ent)
{
	VectorSet (ent->mins, -3, -3, -3); // the -3 here is correct. leave the -4 in the QUAKED line.
	VectorSet (ent->maxs, 3, 3, 4);

	SimpleModelInit2(ent,&siberiaModelData[OBJ_CARTON3],NULL,NULL);
}


/*QUAKED misc_siberia_carton4 (1 .5 0) (-10 -10 -15) (10 10 15)  INVULNERABLE  NOPUSH  x x x x FLUFF
A barrel shaped carton.  20 x 20 x 30
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_siberia_carton4 (edict_t *ent)
{
	VectorSet (ent->mins, -10, -10, -15);
	VectorSet (ent->maxs, 10, 10, 15);

	SimpleModelInit2(ent,&siberiaModelData[OBJ_CARTON4],NULL,NULL);
}

/*QUAKED misc_siberia_icicles (1 .5 0) (-1 -41 -41) (1 41 41)  INVULNERABLE  NOPUSH  NOANIMATE x x x FLUFF
Icicles
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, can't be pushed
NOANIMATE - won't animate
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_siberia_icicles (edict_t *ent)
{
	VectorSet (ent->mins, -1, -41, -41);
	VectorSet (ent->maxs,  1,  41,  41);

	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&siberiaModelData[OBJ_ICICLES],NULL,NULL);
}



/*QUAKED misc_siberia_flag (1 .5 0) (-16 -1 -1) (1 1 79)  INVULNERABLE  NOPUSH  x x x x FLUFF
A flag
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_siberia_flag (edict_t *ent)
{
	VectorSet (ent->mins, -16, -1, -1);
	VectorSet (ent->maxs,  1,   1,  79);

	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&siberiaModelData[OBJ_FLAG],NULL,NULL);

}

void roadflare_use (edict_t *ent, edict_t *other, edict_t *activator)
{

	if (ent->spawnflags & FLARE_STARTOFF)	// It was off
	{
		ent->spawnflags &= ~FLARE_STARTOFF;	// Now it's on

		ent->s.sound =  gi.soundindex("Ambient/Gen/FireFX/FlareLP.wav");
		ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		if (ent->spawnflags & FLARE_BLUE)
		{
			fxRunner.execContinualEffect("environ/roadflare_blue", ent);
		}
		else
		{
			fxRunner.execContinualEffect("environ/roadflare", ent);
		}
	}
	else										// It was on 
	{
		ent->spawnflags |= FLARE_STARTOFF;	// Now it's off

		ent->s.sound = NULL;
		if (ent->spawnflags & FLARE_BLUE)
		{
			fxRunner.stopContinualEffect("environ/roadflare_blue", ent);
		}
		else
		{
			fxRunner.stopContinualEffect("environ/roadflare", ent);
		}
	}

}

/*QUAKED misc_siberia_roadflare (1 .5 0) (-2 -2 -2) (2 2 2)  INVULNERABLE  NOPUSH  STARTOFF  BLUE x x FLUFF
A flaming, sputtering road flare
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, can't be pushed
FLUFF - won't show if gl_pictip is set
BLUE - it'll be blue.  defaults to red if this flag not set
*/
void SP_misc_siberia_roadflare (edict_t *ent)
{
	VectorSet (ent->mins, -2, -2, -2);
	VectorSet (ent->maxs,  2,  2,  2);

	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&siberiaModelData[OBJ_ROADFLARE],NULL,NULL);
	SimpleModelTurnOnOff(ent->ghoulInst, false);

	if (!(ent->spawnflags & FLARE_STARTOFF))
	{
		ent->s.sound =  gi.soundindex("Ambient/Gen/FireFX/FlareLP.wav");
		ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

		if (ent->spawnflags & FLARE_BLUE)
		{
			fxRunner.execContinualEffect("environ/roadflare_blue", ent);
		}
		else
		{
			fxRunner.execContinualEffect("environ/roadflare", ent);
		}
	}

	gi.soundindex("Ambient/Gen/FireFX/FlareLP.wav");
	ent->use = roadflare_use;
}


/*QUAKED misc_siberia_searchlight (1 .5 0) (-10 -10 -8) (10 10 12)  INVULNERABLE  NOPUSH  STARTOFF x x x FLUFF
A small search light
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_siberia_searchlight (edict_t *ent)
{
	boltonOrientation_c			boltonInfo;
	IGhoulInst*					lampInst = NULL,
								*armInst = NULL;
	GhoulID						lampBolt;

	VectorSet (ent->mins, -10, -10, -8);
	VectorSet (ent->maxs,  10,  10,  12);

	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&siberiaModelData[OBJ_SEARCHLIGHTARM],NULL,NULL);

	ent->touch = NULL;
	SimpleModelAddBolt(ent,siberiaModelData[OBJ_SEARCHLIGHTARM],"DUMMY02",
						siberiaModelData[OBJ_SEARCHLIGHT],"DUMMY02",NULL);

	lampInst = SimpleModelGetBolt(ent, 1);
	armInst = ent->ghoulInst;

// ********************************************************************************************
// Find my target entity and then orient myself to look at it.
// ********************************************************************************************
	ent->targetEnt=G_Find(NULL,FOFS(targetname),ent->target);
	boltonInfo.root = ent;

	if (ent->targetEnt)
	{
		if (lampInst && lampInst->GetGhoulObject())
		{
			lampBolt = lampInst->GetGhoulObject()->FindPart("DUMMY02");
		}
		if (!lampBolt)
		{
			return;
		}
		// yaw the entity
		boltonInfo.boltonInst = armInst;
		boltonInfo.parentInst = NULL;
		VectorCopy(ent->targetEnt->s.origin, boltonInfo.vTarget);
		boltonInfo.fMinPitch = 0;
		boltonInfo.fMaxPitch = 0;
		boltonInfo.fMinYaw = -4000;
		boltonInfo.fMaxYaw = 4000;
		boltonInfo.fMaxTurnSpeed = 4000;
		boltonInfo.bUsePitch = false;
		boltonInfo.bUseYaw = true;
		boltonInfo.bToRoot = true;
		boltonInfo.OrientEnt();
		// pitch the gun
		{
			boltonInfo.boltonInst = lampInst;
			boltonInfo.boltonID = lampBolt;
			boltonInfo.parentInst = lampInst->GetParent();
			boltonInfo.parentID = lampInst->GetParent()->GetGhoulObject()->FindPart("DUMMY02");
			VectorCopy(ent->targetEnt->s.origin, boltonInfo.vTarget);
			boltonInfo.fMinPitch = -4000;
			boltonInfo.fMaxPitch = 4000;
			boltonInfo.fMinYaw = 0;
			boltonInfo.fMaxYaw = 0;
			boltonInfo.fMaxTurnSpeed = 4000;
			boltonInfo.bUsePitch = true;
			boltonInfo.bUseYaw = false;
			boltonInfo.bToRoot = true;
			boltonInfo.OrientBolton();
		}
	}

//	ent->think = searchlight_think;
//	ent->nextthink = level.time + FRAMETIME;
}

/*QUAKED misc_siberia_snowcat (1 .5 0) (-91 -66 -33) (91 66 58)  INVULNERABLE  NOPUSH  LIGHTS x x x FLUFF
Large Immobile Snowcat with turret.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A can't be pushed
LIGHTS - turns on headlights
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_siberia_snowcat (edict_t *ent)
{
	VectorSet (ent->mins, -91, -66, -33);
	VectorSet (ent->maxs,  91,  66,  58);

	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&siberiaModelData[OBJ_SNOWCAT],NULL,NULL);

	if (ent->spawnflags & SNOWCAT_LIGHTS)
	{
		SimpleModelAddBolt(ent,siberiaModelData[OBJ_SNOWCAT],"HEADLIGHT01",
							siberiaModelData[OBJ_LIGHT_BEAM],"to_headlight",NULL);

		SimpleModelAddBolt(ent,siberiaModelData[OBJ_SNOWCAT],"HEADLIGHT02",
							siberiaModelData[OBJ_LIGHT_BEAM],"to_headlight",NULL);
	}	
}