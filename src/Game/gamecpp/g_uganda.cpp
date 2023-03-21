/******************************************************
 * Objects for Uganda/Sudan/Africa level              *
 ******************************************************/

#include "g_local.h"
#include "g_obj.h"
#include "callback.h"

#define HLTH_UGANDA_AFRICAN_TREE		1000
#define HLTH_UGANDA_CHAINSAW			500
#define HLTH_UGANDA_COW					400
#define HLTH_UGANDA_COW_HEAD			100
#define HLTH_UGANDA_GORE_PILE			300
#define HLTH_UGANDA_GUTTED_COW			1000
#define HLTH_UGANDA_HOOK				1000
#define HLTH_UGANDA_NUKE				1000
#define HLTH_UGANDA_ROCKET_CORE			3000
#define HLTH_UGANDA_ROCKET_ENGINE		4000
#define HLTH_UGANDA_SIDE_O_BEEF			500
#define HLTH_UGANDA_TRAIN_ARM			500
#define HLTH_UGANDA_TRAIN_BUMPER		500
#define HLTH_UGANDA_TRAIN_COUPLING		500
#define HLTH_UGANDA_TRAIN_WHEEL			500



#define GUTTED_COW_BLOOD_POOL	4
#define	GUTTED_COW_BLOOD_DRIP	8
#define	GUTTED_COW_SWING		16

#define COW_HEAD_BLOOD_POOL		4

#define CHAINSAW_RUNNING		4

#define SIDEOBEEF_SWING	4


//  misc_uganda...
void SP_misc_uganda_african_tree (edict_t *ent);
void SP_misc_uganda_chainsaw (edict_t *ent);
void SP_misc_uganda_cow (edict_t *ent);
void SP_misc_uganda_cow_head (edict_t *ent);
void SP_misc_uganda_gore_pile (edict_t *ent);
void SP_misc_uganda_gutted_cow (edict_t *ent);
void SP_misc_uganda_hook (edict_t *ent);
void SP_misc_uganda_nuke (edict_t *ent);
void SP_misc_uganda_rocket_core (edict_t *ent);
void SP_misc_uganda_rocket_engine (edict_t *ent);
void SP_misc_uganda_side_o_beef (edict_t *ent);
void SP_misc_uganda_train_arm (edict_t *ent);
void SP_misc_uganda_train_bumper (edict_t *ent);
void SP_misc_uganda_train_coupling (edict_t *ent);
void SP_misc_uganda_train_wheel (edict_t *ent);

spawn_t ugandaSpawns[] =
{
	// Uganda items
	{"misc_uganda_african_tree",		SP_misc_uganda_african_tree},
	{"misc_uganda_chainsaw",			SP_misc_uganda_chainsaw},
	{"misc_uganda_cow",					SP_misc_uganda_cow},
	{"misc_uganda_cow_head",			SP_misc_uganda_cow_head},
	{"misc_uganda_gore_pile",			SP_misc_uganda_gore_pile},
	{"misc_uganda_gutted_cow",			SP_misc_uganda_gutted_cow},
	{"misc_uganda_hook",				SP_misc_uganda_hook},
	{"misc_uganda_nuke",				SP_misc_uganda_nuke},
	{"misc_uganda_rocket_core",			SP_misc_uganda_rocket_core},
	{"misc_uganda_rocket_engine",		SP_misc_uganda_rocket_engine},
	{"misc_uganda_side_o_beef",			SP_misc_uganda_side_o_beef},
	{"misc_uganda_train_arm",			SP_misc_uganda_train_arm},
	{"misc_uganda_train_bumper",		SP_misc_uganda_train_bumper},
	{"misc_uganda_train_coupling",		SP_misc_uganda_train_coupling},
	{"misc_uganda_train_wheel",			SP_misc_uganda_train_wheel},

	{NULL,								NULL},
};



typedef enum
{
	OBJ_AFRICAN_TREE = 0,
	OBJ_CHAINSAW,
	OBJ_COW,
	OBJ_COW_HEAD,
	OBJ_GORE_PILE,
	OBJ_GUTTED_COW,
	OBJ_HOOK,
	OBJ_NUKE,
	OBJ_ROCKET_CORE,
	OBJ_ROCKET_ENGINE,
	OBJ_SIDE_O_BEEF,
	OBJ_TRAIN_ARM,
	OBJ_TRAIN_BUMPER,
	OBJ_TRAIN_COUPLING,
	OBJ_TRAIN_WHEEL,
	MAX_OBJS
};



modelSpawnData_t ugandaModelData[MD_UGANDA_SIZE] =
{	
//      dir							file				surfaceType			material			health						solid			material file  cnt  scale
"objects/uganda/african_tree",		"tree",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_UGANDA_AFRICAN_TREE,	SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_AFRICAN_TREE
"objects/uganda/chainsaw",			"chainsaw",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_UGANDA_CHAINSAW,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_CHAINSAW
"objects/uganda/cow",				"cow",				SURF_LIQUID_RED,	MAT_DEFAULT,		HLTH_UGANDA_COW,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_COW
"objects/uganda/cow_head",			"head",				SURF_LIQUID_RED,	MAT_DEFAULT,		HLTH_UGANDA_COW_HEAD,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_COW_HEAD
"objects/uganda/gore_pile",			"gore_pile",		SURF_LIQUID_RED,	MAT_DEFAULT,		HLTH_UGANDA_GORE_PILE,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_GORE_PILE
"objects/uganda/gutted_cow",		"gutted_cow",		SURF_LIQUID_RED,	MAT_ROCK_FLESH,		HLTH_UGANDA_GUTTED_COW,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_GUTTED_COW
"objects/uganda/hook",				"hook",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_UGANDA_HOOK,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_HOOK
"objects/uganda/nuke",				"nuke",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_UGANDA_NUKE,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_NUKE
"objects/uganda/rocket_core",		"core",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_UGANDA_TRAIN_COUPLING,	SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_ROCKET_CORE
"objects/uganda/rocket_engine",		"engine",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_UGANDA_TRAIN_COUPLING,	SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_ROCKET_ENGINE
"objects/uganda/side_o_beef",		"beef",				SURF_LIQUID_RED,	MAT_ROCK_FLESH,		HLTH_UGANDA_SIDE_O_BEEF,	SOLID_BBOX,		"beef",			0,	0.0,	NULL,	// OBJ_SIDE_O_BEEF
"objects/uganda/train_arm",			"arm",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_UGANDA_TRAIN_BUMPER,	SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_TRAIN_ARM
"objects/uganda/train_bumper",		"bumper",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_UGANDA_TRAIN_BUMPER,	SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_TRAIN_BUMPER
"objects/uganda/train_coupling",	"coupling",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_UGANDA_TRAIN_COUPLING,	SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_TRAIN_COUPLING
"objects/uganda/train_wheel",		"wheel",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_UGANDA_TRAIN_WHEEL,	SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_TRAIN_WHEEL
};


/*QUAKED misc_uganda_african_tree (1 .5 0) (-42 -77 -76) (42 77 76) INVULNERABLE  NOPUSH  x x x x FLUFF
Tree with funky sprite-ish foliage.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_uganda_african_tree (edict_t *ent)
{
	VectorSet (ent->mins, -42, -77, -76);
	VectorSet (ent->maxs,  42,  77,  76);

	SimpleModelInit2(ent,&ugandaModelData[OBJ_AFRICAN_TREE],NULL,NULL);
}


/*QUAKED misc_uganda_chainsaw (1 .5 0) (-14 -4 -5) (14 4 5) INVULNERABLE  NOPUSH  RUNNING  x x x FLUFF
a chainsaw
------ KEYS ------
Skin :
0 - very dirty
1 - mildly dirty
2 - clean
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
RUNNING - shake and smoke
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- will smoke if running flag is set
*/
void SP_misc_uganda_chainsaw (edict_t *ent)
{
	VectorSet (ent->mins, -14, -4, -5);
	VectorSet (ent->maxs,  14,  4,  5);

	if (ent->spawnflags & CHAINSAW_RUNNING)
	{
		fxRunner.execContinualEffect("environ/normalsmoke", ent);
	}

	SimpleModelInit2(ent,&ugandaModelData[OBJ_CHAINSAW],NULL,NULL);
}


/*QUAKED misc_uganda_cow (1 .5 0) (-13 -38 -29) (13 43 28)  INVULNERABLE  NOPUSH  x x x x FLUFF
A cow.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_uganda_cow (edict_t *ent)
{
	VectorSet (ent->mins, -13, -38, -29);
	VectorSet (ent->maxs,  13,  43,  28);

	SimpleModelInit2(ent,&ugandaModelData[OBJ_COW],NULL,NULL);
}


/*QUAKED misc_uganda_cow_head (1 .5 0) (-10 -13 -8) (10 13 8) INVULNERABLE  NOPUSH BLOOD_POOL  x x x FLUFF
A severed cow head
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
BLOOD_POOL - places a blood pool underneath
FLUFF - won't show if gl_pictip is set
*/
void makebloodpool (edict_t *self);

void SP_misc_uganda_cow_head (edict_t *ent)
{
	VectorSet (ent->mins, -10, -13, -8);
	VectorSet (ent->maxs,  10,  13,  8);

	if (ent->spawnflags & COW_HEAD_BLOOD_POOL)
	{
		ent->think = makebloodpool;
		ent->nextthink = level.time + 2;		
	}	

	SimpleModelInit2(ent,&ugandaModelData[OBJ_COW_HEAD],NULL,NULL);
}


void gore_pile_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t		splatDir;

	VectorSubtract(wherehit, other->s.origin, splatDir);
	FX_MakeBloodSpray(wherehit, splatDir, 200, 1, self);
}

/*QUAKED misc_uganda_gore_pile (1 .5 0) (-26 -23 -3) (26 23 3) INVULNERABLE  NOPUSH  x x x x FLUFF
Heart, intestines, and assorted gore in a pile.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- splatters blood when shot
*/
void SP_misc_uganda_gore_pile (edict_t *ent)
{
	VectorSet (ent->mins, -26, -23, -3);
	VectorSet (ent->maxs,  26,  23,  3);

	ent->pain = gore_pile_pain;

	SimpleModelInit2(ent,&ugandaModelData[OBJ_GORE_PILE],NULL,NULL);

}


void makebloodpool (edict_t *self)
{
	FX_MakeDecalBelow(self->s.origin, FXDECAL_BLOODPOOL, 0);
	self->think = NULL;
	self->nextthink = 0;
}

void guttedcow_swing(edict_t *self)
{
	SimpleModelSetSequence(self,"gutted_cow",SMSEQ_LOOP);
}	



GuttedCowCallback  theGuttedCowCallback;

bool GuttedCowCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	edict_t* self = (edict_t*)ent;

	if (self->spawnflags & GUTTED_COW_SWING)	// Swing!
		guttedcow_swing(self);

	return true;
}



void guttedcow_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	GhoulID	tempNote=0;
	ggObjC	*cCow;
	vec3_t	vec_to_other;
	vec3_t	myfacing;
	float dirToFace,holdDir;
	char *seq;

	VectorSubtract(other->s.origin,ent->s.origin,vec_to_other);
	vectoangles(vec_to_other,myfacing);
	dirToFace = anglemod(myfacing[YAW]);
	holdDir = dirToFace - ent->s.angles[YAW]; 
	holdDir = anglemod(holdDir);

	if ((holdDir < 30) || (holdDir > 340))		// In front
		seq = "swing1";
	else if ((holdDir > 150) && (holdDir < 210))		// Behind you
		seq = "swing1";
	else
		seq = "swing2";

	if (!ent->ghoulInst)
	{
		return;
	}

	SimpleModelSetSequence(ent,seq,SMSEQ_HOLD);

	// register a callback for the end of the pain sequence
	cCow = game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());
	tempNote=cCow->GetMyObject()->FindNoteToken("EOS");

	if (tempNote && ent->ghoulInst)
	{
		ent->ghoulInst->AddNoteCallBack(&theGuttedCowCallback,tempNote);
	}

}


/*QUAKED misc_uganda_gutted_cow (1 .5 0) (-15 -33 -99) (27 25 1)  INVULNERABLE NOPUSH BLOOD_POOL BLOOD_DRIP SWING  x FLUFF
A gutted cow hanging by its leg from a chain.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
BLOODPOOL - blood underneath
BLOODDRIP - drips blood
SWING - gentle swing around
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- swings when shot
*/
void SP_misc_uganda_gutted_cow (edict_t *ent)
{
	VectorSet (ent->mins, -15, -33, -99);
	VectorSet (ent->maxs,  27,  25,  1);

	SimpleModelInit2(ent,&ugandaModelData[OBJ_GUTTED_COW],NULL,NULL);

	VectorSet (ent->mins, -15, -33, -99);
	VectorSet (ent->maxs,  27,  25,  1);
	gi.linkentity(ent);

	if (ent->spawnflags & GUTTED_COW_BLOOD_POOL)
	{
		ent->think = makebloodpool;
		ent->nextthink = level.time + 2;		
	}

	if (ent->spawnflags & GUTTED_COW_BLOOD_DRIP)
	{
		fxRunner.execContinualEffect("environ/bloodrain", ent);
	}


	if (ent->spawnflags & GUTTED_COW_SWING)	// So they don't swing around in unison
	{
		ent->think = guttedcow_swing;
		ent->nextthink = level.time + (rand() % 10);
	}
	else
	{
		ent->ghoulInst->Pause(level.time);
	}

	ent->pain = guttedcow_pain;
}


/*QUAKED misc_uganda_hook (1 .5 0) (-1 -7 -14) (1 1 0)  INVULNERABLE NOPUSH   x x x x FLUFF
a large meathook meant to hang from the ceiling
------ SPAWNFLAGS ------ 
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_uganda_hook (edict_t *ent)
{
	VectorSet (ent->mins, -1, -4, -7);
	VectorSet (ent->maxs,  1,  4,  7);

	SimpleModelInit2(ent,&ugandaModelData[OBJ_HOOK],NULL,NULL);
}


/*QUAKED misc_uganda_nuke (1 .5 0) (-31 -28 -24) (30 14 25)  INVULNERABLE NOPUSH   x x x x FLUFF
a nuclear bomb
--------SPAWNFLAGS----------
INVULNERABLE - N/A can't be damaged.
NOPUSH - N/A can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_uganda_nuke (edict_t *ent)
{
	VectorSet (ent->mins, -31, -28, -24);
	VectorSet (ent->maxs,  30,  14,  25);

	ent->spawnflags |= SF_INVULNERABLE;
	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&ugandaModelData[OBJ_NUKE],NULL,NULL);

}

/*QUAKED misc_uganda_rocket_core (1 .5 0) (-55 -14 -14) (55 14 14)  INVULNERABLE NOPUSH   x x x x FLUFF
The payload compartment of a rocket
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_uganda_rocket_core (edict_t *ent)
{
	VectorSet (ent->mins, -55, -14, -14);
	VectorSet (ent->maxs,  55,  14,  14);

	SimpleModelInit2(ent,&ugandaModelData[OBJ_ROCKET_CORE],NULL,NULL);
}


/*QUAKED misc_uganda_rocket_engine (1 .5 0) (-76 -28 -27) (76 34 32)  INVULNERABLE NOPUSH   x x x x FLUFF
Rocket engine without the metal skin
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_uganda_rocket_engine (edict_t *ent)
{

	VectorSet (ent->mins, -76, -28, -27);
	VectorSet (ent->maxs,  76,  34,  32);

	SimpleModelInit2(ent,&ugandaModelData[OBJ_ROCKET_ENGINE],NULL,NULL);

}

void sideobeef_swing(edict_t *self)
{
	SimpleModelSetSequence(self,"SLOW",SMSEQ_LOOP);
}	

SideOBeefCallback  theSideOBeefCallback;

bool SideOBeefCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	edict_t* self = (edict_t*)ent;

	if (self->spawnflags & SIDEOBEEF_SWING)	// Swing!
	{
		sideobeef_swing(self);
	}

	return true;
}

void sideobeef_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	GhoulID	tempNote=0;
	ggObjC	*cCow;
	vec3_t	vec_to_other;
	vec3_t	myfacing;
	float dirToFace,holdDir;
	char *seq;

	VectorSubtract(other->s.origin,ent->s.origin,vec_to_other);
	vectoangles(vec_to_other,myfacing);
	dirToFace = anglemod(myfacing[YAW]);
	holdDir = dirToFace - ent->s.angles[YAW]; 
	holdDir = anglemod(holdDir);

	if ((holdDir < 30) || (holdDir > 340))		// In front
		seq = "SWING2";
	else if ((holdDir > 150) && (holdDir < 210))		// Behind you
		seq = "SWING2";
	else
		seq = "SWING1";

	if (!ent->ghoulInst)
	{
		return;
	}

	SimpleModelSetSequence(ent,seq,SMSEQ_HOLD);

	// register a callback for the end of the pain sequence
	cCow = game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());
	tempNote=cCow->GetMyObject()->FindNoteToken("EOS");

	if (tempNote && ent->ghoulInst)
	{
		ent->ghoulInst->AddNoteCallBack(&theSideOBeefCallback,tempNote);
	}

}


/*QUAKED misc_uganda_side_o_beef (1 .5 0) (-13 -16 -55) (19 17 48)  INVULNERABLE NOPUSH SWING  x x x FLUFF
Dressed side of beef which hangs from ceiling
------ KEYS ------
skin :
0 - normal
1 - frozen
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
SWING - swings gently 
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- swings when shot
*/
void SP_misc_uganda_side_o_beef (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -13, -16, -55);
	VectorSet (ent->maxs,  19,  17,  48);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "beef_f";
		break;
	default:
		skinname = "beef";
		break;
	}

	SimpleModelInit2(ent,&ugandaModelData[OBJ_SIDE_O_BEEF],skinname,NULL);

	if (ent->spawnflags & SIDEOBEEF_SWING)	// So they don't swing around in unison
	{
		ent->think = sideobeef_swing;
		ent->nextthink = level.time + gi.irand(1,10);
	}

	ent->pain = sideobeef_pain;
}

/*QUAK-ED misc_uganda_train_arm (1 .5 0) (-99 -5 -110) (99 5 110)  INVULNERABLE  NOPUSH  x x x x FLUFF
A train arm. . .for water or something.
------ SPAWNFLAGS ------
INVULNERABLE - N/A can't be damaged.
NOPUSH - N/A can't ever be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_uganda_train_arm (edict_t *ent)
{
	VectorSet (ent->mins, -99, -5, -110);
	VectorSet (ent->maxs,  99,  5,  110);

	ent->spawnflags |= SF_INVULNERABLE;
	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&ugandaModelData[OBJ_TRAIN_ARM],NULL,NULL);
}
/*QUAKED misc_uganda_train_bumper (1 .5 0) (-32 -15 -16) (32 15 10)  INVULNERABLE  NOPUSH  x x x x FLUFF
A train bumper
------ SPAWNFLAGS ------
INVULNERABLE - N/A can't be damaged.
NOPUSH - N/A can't ever be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_uganda_train_bumper (edict_t *ent)
{
	VectorSet (ent->mins, -32, -15, -16);
	VectorSet (ent->maxs,  32,  15,  10);

	ent->spawnflags |= SF_INVULNERABLE;
	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&ugandaModelData[OBJ_TRAIN_BUMPER],NULL,NULL);
}

/*QUAKED misc_uganda_train_coupling (1 .5 0) (-31 -11 -5) (31 11 5)  INVULNERABLE  NOPUSH  x x x x FLUFF
A train coupling
------ SPAWNFLAGS ------
INVULNERABLE - N/A can't be damaged.
NOPUSH - N/A can't ever be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_uganda_train_coupling (edict_t *ent)
{
	VectorSet (ent->mins, -31, -11, -5);
	VectorSet (ent->maxs,  31,  11,  5);

	ent->spawnflags |= SF_INVULNERABLE;
	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&ugandaModelData[OBJ_TRAIN_COUPLING],NULL,NULL);
}

/*QUAK-ED misc_uganda_train_wheel (1 .5 0) (-14 -1 -14) (14 1 14)  INVULNERABLE  NOPUSH  x x x x FLUFF
A spinning train wheel
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't ever be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_uganda_train_wheel (edict_t *ent)
{
	VectorSet (ent->mins, -14, -1, -14);
	VectorSet (ent->maxs,  14,  1,  14);

	SimpleModelInit2(ent,&ugandaModelData[OBJ_TRAIN_WHEEL],NULL,NULL);
}
