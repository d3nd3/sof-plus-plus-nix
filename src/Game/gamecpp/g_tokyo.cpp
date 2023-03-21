/******************************************************
 * Objects for Tokyo                                  *
 ******************************************************/

#include "g_local.h"
#include "g_obj.h"

#define HELIFLAG_STARTOFF				(1<<2)


#define HLTH_TOKYO_BOWL					100
#define HLTH_TOKYO_BUDDHA				2000
#define HLTH_TOKYO_FAN					50
#define HLTH_TOKYO_GONG					1000
#define HLTH_TOKYO_GYM_EQUIP1			2000
#define HLTH_TOKYO_GYM_EQUIP2			2000
#define HLTH_TOKYO_PLANT_BONSAI			100
#define HLTH_TOKYO_SAMURAI_SUIT			1000
#define HLTH_TOKYO_SAMURAI_SUIT_SM		1000
#define HLTH_TOKYO_SHOE					100
#define HLTH_TOKYO_STONE_LIGHT			1000


// misc_tokyo
void SP_misc_tokyo_bowl (edict_t *ent);
void SP_misc_tokyo_buddha (edict_t *ent);
void SP_misc_tokyo_fan (edict_t *ent);
void SP_misc_tokyo_gong (edict_t *ent);
void SP_misc_tokyo_gym_equipment1 (edict_t *ent);
void SP_misc_tokyo_gym_equipment2 (edict_t *ent);
void SP_misc_tokyo_plant_bonsai (edict_t *ent);
void SP_misc_tokyo_samurai_suit (edict_t *ent);
void SP_misc_tokyo_samurai_suit_small (edict_t *ent);
void SP_misc_tokyo_shoe (edict_t *ent);
void SP_misc_tokyo_stone_light (edict_t *ent);
void SP_misc_exploding_heli(edict_t *ent);

spawn_t tokyoSpawns[] =
{
	// misc_tokyo
	{"misc_tokyo_bowl",					SP_misc_tokyo_bowl},
	{"misc_tokyo_buddha",				SP_misc_tokyo_buddha},
	{"misc_tokyo_fan",					SP_misc_tokyo_fan},
	{"misc_tokyo_gong",					SP_misc_tokyo_gong},
	{"misc_tokyo_gym_equipment1",		SP_misc_tokyo_gym_equipment1},
	{"misc_tokyo_gym_equipment2",		SP_misc_tokyo_gym_equipment2},
	{"misc_tokyo_plant_bonsai",			SP_misc_tokyo_plant_bonsai},
	{"misc_tokyo_samurai_suit",			SP_misc_tokyo_samurai_suit},
	{"misc_tokyo_samurai_suit_small",	SP_misc_tokyo_samurai_suit_small},
	{"misc_tokyo_shoe",					SP_misc_tokyo_shoe},
	{"misc_tokyo_stone_light",			SP_misc_tokyo_stone_light},
	{"misc_exploding_heli",				SP_misc_exploding_heli},
	
	{NULL,								NULL},
};



typedef enum
{
	OBJ_BOWL = 0,
	OBJ_BUDDHA,
	OBJ_FAN,
	OBJ_GONG,
	OBJ_GYM_EQUIP1,
	OBJ_GYM_EQUIP2,
	OBJ_PLANT_BONSAI,
	OBJ_SAMURAI_SUIT,
	OBJ_SAMURAI_SUIT_SM,
	OBJ_SHOE,
	OBJ_STONE_LIGHT,
	OBJ_EXPLODING_HELI,
	MAX_OBJS
};


modelSpawnData_t tokyoModelData[MD_TOKYO_SIZE] =
{	
//      dir							file				surfaceType			material			health						solid			material file  cnt  scale
"objects/tokyo/bowl",				"bowl",				SURF_WOOD_LBROWN,	MAT_WOOD_LBROWN,	HLTH_TOKYO_BOWL,			SOLID_BBOX,		NULL,			6,	DEBRIS_SM,	NULL,	// OBJ_BOWL
"objects/tokyo/buddha",				"buddha",			SURF_METAL,			MAT_METAL_SHINY,	HLTH_TOKYO_BUDDHA,			SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_BUDDHA
"objects/tokyo/fan",				"fan",				SURF_WOOD_LBROWN,	SURF_WOOD_LBROWN,	HLTH_TOKYO_FAN,				SOLID_BBOX,		NULL,			0,	DEBRIS_TINY,		NULL,	// OBJ_FAN
"objects/tokyo/gong",				"gong",				SURF_METAL,			MAT_METAL_SHINY,	HLTH_TOKYO_GONG,			SOLID_BBOX,		NULL,			6,	0.5,		NULL,	// OBJ_GONG
"objects/tokyo/gym_equip1",			"equip1",			SURF_METAL,			MAT_METAL_SHINY,	HLTH_TOKYO_GYM_EQUIP1,		SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_GYM_EQUIP1
"objects/tokyo/gym_equip2",			"equip2",			SURF_METAL,			MAT_METAL_SHINY,	HLTH_TOKYO_GYM_EQUIP2,		SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_GYM_EQUIP2
"objects/tokyo/plant_bonsai",		"bonsai",			SURF_WOOD_LBROWN,	MAT_WOOD_LBROWN,	HLTH_TOKYO_PLANT_BONSAI,	SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_PLANT_BONSAI
"objects/tokyo/samurai_suit_small",	"samurai_sm",		SURF_METAL,			MAT_METAL_RUSTY,	HLTH_TOKYO_SAMURAI_SUIT,	SOLID_BBOX,		NULL,			0,	DEBRIS_MED,	NULL,	// OBJ_SAMURIA_SUIT
"objects/tokyo/samurai_suit_small",	"samurai_sm",		SURF_METAL,			MAT_METAL_RUSTY,	HLTH_TOKYO_SAMURAI_SUIT_SM,	SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_SAMURIA_SUIT_SM
"objects/tokyo/shoe",				"shoe",				SURF_WOOD_LBROWN,	MAT_WOOD_LBROWN,	HLTH_TOKYO_SHOE,			SOLID_BBOX,		NULL,			4,	DEBRIS_SM,		NULL,	// OBJ_SHOE
"objects/tokyo/stone_light",		"light",			SURF_STONE_LGREY,	MAT_ROCK_LGREY,		HLTH_TOKYO_STONE_LIGHT,		SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_STONE_LIGHT
"objects/generic/stunt_chopper",	"stunt_chopper",	SURF_METAL,			MAT_METAL_SHINY,	1,							SOLID_BBOX,		NULL,			0,	0.0,		NULL,	// OBJ_EXPLODING_HELI
};


/*QUAKED misc_tokyo_bowl (1 .5 0) (-5 -5 -1) (5 5 2)   INVULNERABLE  NOPUSH x x x x FLUFF
A small bowl
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_tokyo_bowl (edict_t *ent)
{
	VectorSet (ent->mins, -5, -5,-1);
	VectorSet (ent->maxs,  5,  5, 2);

	SimpleModelInit2(ent,&tokyoModelData[OBJ_BOWL],NULL,NULL);
}

/*QUAK-ED misc_tokyo_buddha (1 .5 0) (-14 -24 -32) (19 24 32)   INVULNERABLE  NOPUSH x x x x FLUFF
A chest high statue of buddha
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_tokyo_buddha (edict_t *ent)
{
	VectorSet (ent->mins, -14, -24, -32);
	VectorSet (ent->maxs, 19, 24, 32);

	SimpleModelInit2(ent,&tokyoModelData[OBJ_BUDDHA],NULL,NULL);
}

/*QUAKED misc_tokyo_fan (1 .5 0) (0 -9 -4) (1 10 6)   INVULNERABLE  NOPUSH  x x x x FLUFF
A small hand held fan
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_tokyo_fan (edict_t *ent)
{
	VectorSet (ent->mins, -14, -24, -32);
	VectorSet (ent->maxs, 19, 24, 32);

	SimpleModelInit2(ent,&tokyoModelData[OBJ_FAN],NULL,NULL);

}

void gong_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{

	SimpleModelSetSequence(ent,tokyoModelData[OBJ_GONG].file,SMSEQ_HOLD);
}

/*QUAKED misc_tokyo_gong (1 .5 0) (-2 -29 -33) (6 29 35)   INVULNERABLE  NOPUSH  x x x x FLUFF
Large gold gong. 
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- swings when shot
*/
void SP_misc_tokyo_gong (edict_t *ent)
{	

	VectorSet (ent->mins, -3, -29, -34);
	VectorSet (ent->maxs,  3,  29,  34);

	SimpleModelInit2(ent,&tokyoModelData[OBJ_GONG],NULL,NULL);
	ent->ghoulInst->Pause(level.time);

	ent->pain = gong_pain;

}

/*QUAK-ED misc_tokyo_gym_equipment1 (1 .5 0) (-36 -13 -27) (36 14 27) INVULNERABLE  NOPUSH  x x x x FLUFF
A treadmill. 
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_tokyo_gym_equipment1 (edict_t *ent)
{
	VectorSet (ent->mins, -36, -13, -27);
	VectorSet (ent->maxs,  36,  14,  27);

	SimpleModelInit2(ent,&tokyoModelData[OBJ_GYM_EQUIP1],NULL,NULL);
}

/*QUAK-ED misc_tokyo_gym_equipment2 (1 .5 0) (-48 -28 -34) (58 33 35)  INVULNERABLE  NOPUSH x x x x FLUFF
A weight machine. 
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_tokyo_gym_equipment2 (edict_t *ent)
{
	VectorSet (ent->mins, -48, -28, -34);
	VectorSet (ent->maxs,  58,  33,  35);

	SimpleModelInit2(ent,&tokyoModelData[OBJ_GYM_EQUIP2],NULL,NULL);
}

/*QUAKED misc_tokyo_plant_bonsai (1 .5 0) (-5 -10 -8) (5 10 8)  INVULNERABLE  NOPUSH x x x x FLUFF
A small bonsai tree
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_tokyo_plant_bonsai (edict_t *ent)
{
	VectorSet (ent->mins, -5, -10, -8);
	VectorSet (ent->maxs,  5,  10,  8);

	SimpleModelInit2(ent,&tokyoModelData[OBJ_PLANT_BONSAI],NULL,NULL);
}

/*QUAKED misc_tokyo_samurai_suit (1 .5 0) (-6 -18 -37) (9 16 37)  INVULNERABLE  NOPUSH x x x x FLUFF
A samurai suit of armor, man sized.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_tokyo_samurai_suit (edict_t *ent)
{
	VectorSet (ent->mins, -6, -18, -37);
	VectorSet (ent->maxs,  9,  16,  37);

	SimpleModelInit2(ent,&tokyoModelData[OBJ_SAMURAI_SUIT],NULL,NULL);
	SimpleModelScale(ent,1.1f);
}

/*QUAKED misc_tokyo_samurai_suit_small (1 .5 0) (-3 -8 -17) (4 7 17)  INVULNERABLE  NOPUSH x x x x FLUFF
A doll-sized samurai suit of armor.  
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_tokyo_samurai_suit_small (edict_t *ent)
{
	VectorSet (ent->mins, -3, -8, -17);
	VectorSet (ent->maxs,  4,  7,  17);

	SimpleModelInit2(ent,&tokyoModelData[OBJ_SAMURAI_SUIT_SM],NULL,NULL);
	SimpleModelScale(ent,0.50f);
}

/*QUAKED misc_tokyo_shoe (1 .5 0) (-6 -3 -2) (6 3 2)  INVULNERABLE  NOPUSH x x x x FLUFF
a brown sandal
------ SPAWNFLAGS -------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_tokyo_shoe (edict_t *ent)
{
	VectorSet (ent->mins, -6, -3, -2);
	VectorSet (ent->maxs,  6,  3,  2);

	SimpleModelInit2(ent,&tokyoModelData[OBJ_SHOE],NULL,NULL);
}

/*QUAKED misc_tokyo_stone_light (1 .5 0) (-15 -13 -14) (15 13 14)  INVULNERABLE  NOPUSH x x x x FLUFF
A waist high column of stone 
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_tokyo_stone_light (edict_t *ent)
{
	VectorSet (ent->mins, -15, -13, 14);
	VectorSet (ent->maxs,  15,  13, 14);

	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&tokyoModelData[OBJ_STONE_LIGHT],NULL,NULL);
}

/*QUAKED misc_exploding_heli (1 .5 0) (-280 -280 -50) (280 280 70)	INVULNERABLE  NOPUSH START_OFF
NOT TO BE USED!!!!!!!!
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
*/
void SP_misc_exploding_heli (edict_t *ent)
{
}
