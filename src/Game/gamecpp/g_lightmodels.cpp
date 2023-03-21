/******************************************************
 * Objects which are Lights                           *
 ******************************************************/

#include "g_local.h"
#include "g_obj.h"
#include "callback.h"
#include "..\qcommon\configstring.h"

#define HANGING_ONTABLE				8
#define HANGING_NOANIM				16


#define HLTH_LGHT_ALLEY_WALL			500
#define HLTH_LGHT_ALLEY_WALL_OLD		500
#define HLTH_LGHT_CAGED					500
#define HLTH_LGHT_CANDELABRA			500
#define HLTH_LGHT_CANDELABRA_SMALL		500
#define HLTH_LGHT_CHANDELIER			500
#define HLTH_LGHT_DESK_LAMP				500
#define HLTH_LGHT_EMERGENCY				500
#define HLTH_LGHT_EMERGENCY_BLUE		500
#define HLTH_LGHT_HALFSHELL				500
#define HLTH_LGHT_HANGING				500
#define HLTH_LGHT_LANTERN				500
#define HLTH_LGHT_LANTERN2				500
#define HLTH_LGHT_LANTERN3				500
#define HLTH_LGHT_MILITARY_DOOR			500
#define HLTH_LGHT_MILITARY_DOOR_OUTSIDE	500
#define HLTH_LGHT_SHOP					500
#define HLTH_LGHT_STREET_MODERN			500
#define HLTH_LGHT_STREET_OLD			500
#define HLTH_LAMP_HANGING				500


//	light_generic...
void SP_light (edict_t *self);
void SP_light_generic_alley_wall (edict_t *ent);
void SP_light_generic_alley_wall_old (edict_t *ent);
void SP_light_generic_caged (edict_t *ent);
void SP_light_generic_candelabra(edict_t *ent);
void SP_light_generic_candelabra_small(edict_t *ent);
void SP_light_generic_chandelier (edict_t *ent);
void SP_light_generic_desk_lamp (edict_t *ent);
void SP_light_generic_emergency (edict_t *ent);
void SP_light_generic_emergency_blue (edict_t *ent);
void SP_light_generic_halfshell (edict_t *ent);
void SP_light_generic_hanging (edict_t *ent);
void SP_light_generic_lantern (edict_t *ent);
void SP_light_generic_lantern2 (edict_t *ent);
void SP_light_generic_lantern3 (edict_t *ent);
void SP_light_generic_military_door (edict_t *ent);
void SP_light_generic_military_door_outside (edict_t *ent);
void SP_light_generic_shop (edict_t *ent);
void SP_light_generic_street_modern (edict_t *ent);
void SP_light_generic_street_old (edict_t *ent);
void SP_light_generic_lamp_hang(edict_t *ent);

spawn_t lightSpawns[] =
{
	//	light_generic...
	{"light", SP_light},
	{"light_generic_alley_wall",			SP_light_generic_alley_wall},
	{"light_generic_alley_wall_old",		SP_light_generic_alley_wall_old},
	{"light_generic_caged",					SP_light_generic_caged},
	{"light_generic_candelabra",			SP_light_generic_candelabra},
	{"light_generic_candelabra_small",		SP_light_generic_candelabra_small},
	{"light_generic_chandelier",			SP_light_generic_chandelier},
	{"light_generic_desk_lamp",				SP_light_generic_desk_lamp},
	{"light_generic_emergency",				SP_light_generic_emergency},
	{"light_generic_emergency_blue",		SP_light_generic_emergency_blue},
	{"light_generic_halfshell",				SP_light_generic_halfshell},
	{"light_generic_hanging",				SP_light_generic_hanging},
	{"light_generic_lantern",				SP_light_generic_lantern},
	{"light_generic_lantern2",				SP_light_generic_lantern2},
	{"light_generic_lantern3",				SP_light_generic_lantern3},
	{"light_generic_military_door",			SP_light_generic_military_door},
	{"light_generic_military_door_outside",	SP_light_generic_military_door_outside},
	{"light_generic_shop",					SP_light_generic_shop},
	{"light_generic_street_modern",			SP_light_generic_street_modern},
	{"light_generic_street_old",			SP_light_generic_street_old},
	{"light_generic_lamp_hang",				SP_light_generic_lamp_hang},
	{NULL,									NULL},
};

typedef enum
{
	OBJ_ALLEY_WALL = 0,
	OBJ_ALLEY_WALL_OLD,
	OBJ_CAGED,
	OBJ_CANDLE,
	OBJ_CANDLE2,
	OBJ_CANDLE3,
	OBJ_CANDELABRA,
	OBJ_CANDELABRA_SMALL,
	OBJ_CHANDELIER,
	OBJ_DESK_LAMP,
	OBJ_EMERGENCY,
	OBJ_EMERGENCY_BLUE,
	OBJ_HALFSHELL,
	OBJ_HANGING,
	OBJ_LANTERN,
	OBJ_LANTERN2,
	OBJ_LANTERN3,
	OBJ_MILITARY_DOOR,
	OBJ_MILITARY_DOOR_OUTSIDE,
	OBJ_SHOP,
	OBJ_STREET_MODERN,
	OBJ_STREET_OLD,
	OBJ_LAMP_HANG,
	MAX_OBJS
};


modelSpawnData_t lightModelData[MD_LIGHT_SIZE] =
{	
//      dir							file				surfaceType			material			health						solid			material file  cnt  scale
"objects/light/alley_wall",			"light",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_ALLEY_WALL,		SOLID_NOT,		NULL,			0,	0.0,	NULL,	// OBJ_ALLEY_WALL
"objects/light/alley_wall_old",		"light",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_ALLEY_WALL_OLD,	SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_ALLEY_WALL_OLD
"objects/light/caged",				"caged",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_CAGED,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_CAGED
"objects/light/candle",				"candle",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_CANDELABRA,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_CANDLE
"objects/light/candle",				"candle2",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_CANDELABRA,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_CANDLE2
"objects/light/candle",				"candle3",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_CANDELABRA,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_CANDLE3
"objects/light/candle",				"base",				SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_CANDELABRA,		SOLID_BBOX,		"candle",		0,	0.0,	NULL,	// OBJ_CANDELABRA
"objects/light/candle2",			"base",				SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_CANDELABRA,		SOLID_BBOX,		"candle",		0,	0.0,	NULL,	// OBJ_CANDELABRA_SMALL
"objects/light/chandelier",			"swing",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_CHANDELIER,		SOLID_BBOX,		"light",		0,	0.0,	NULL,	// OBJ_CHANDELIER
"objects/light/desk_lamp",			"lamp",				SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_DESK_LAMP,		SOLID_BBOX,		"light",		0,	0.0,	NULL,	// OBJ_DESK_LAMP
"objects/light/emergency",			"emergency",		SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_DESK_LAMP,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_EMERGENCY
"objects/light/emergency_blue",		"emergency",		SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_DESK_LAMP,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_EMERGENCY_BLUE
"objects/light/halfshell",			"shell",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_HANGING,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_HALFSHELL
"objects/light/hanging",			"hanging",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_HANGING,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_HANGING
"objects/light/lantern",			"lantern",			SURF_METAL,			MAT_WALL_WHITE,		HLTH_LGHT_LANTERN,			SOLID_BBOX,		NULL,			8,	DEBRIS_SM,	NULL,	// OBJ_LANTERN
"objects/light/lantern2",			"lantern2",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_LANTERN2,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_LANTERN2
"objects/light/lantern3",			"lantern3",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_LANTERN3,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_LANTERN3
"objects/light/military_door",		"light",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_MILITARY_DOOR,	SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_MILITARY_DOOR
"objects/light/military_door_outside","light",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_MILITARY_DOOR_OUTSIDE,SOLID_BBOX,	NULL,			0,	0.0,	NULL,	// OBJ_MILITARY_DOOR_OUTSIDE
"objects/light/shop",				"shop",				SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_SHOP,				SOLID_BBOX,		"light",		0,	0.0,	NULL,	// OBJ_SHOP
"objects/light/street_modern",		"modern",			SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_STREET_MODERN,	SOLID_NOT,		NULL,			0,	0.0,	NULL,	// OBJ_STREET_MODERN
"objects/light/street_old",			"old",				SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LGHT_STREET_OLD,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_STREET_OLD
"objects/light/lamp_hang",			"lamp",				SURF_METAL,			MAT_METAL_RUSTY,	HLTH_LAMP_HANGING,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_LAMP_HANG
};

//-------------------------------------------------------------
// For objects that break apart
//-------------------------------------------------------------
void candelabra_pain (edict_t *self,int partLost, vec3_t boltPos);

// Breaking Light Parts Data enum
typedef enum
{
	BLPD_CANDELABRA = 0,
	BLPD_MAX
};

// Breaking Light Parts Data for each object
objBreak_t lightObjBreak [BLPD_MAX] = 
{ 
// # of parts   objParts ptr
	3,			NULL,			// BLPD_CANDELABRA
};


// Breaking Light Parts Data for each part of each object 
// WARNING: when updating this be sure to update lightObjBreak if more parts are added
objParts_t lightObjBoltPartsData[15] =
{	
	// BLPD_CANDELABRA
	"DUMMY01", 1,	candelabra_pain,	// candle 1
	"DUMMY02", 2,	candelabra_pain,	// candle 2
	"DUMMY03", 3,	candelabra_pain,	// candle 3
};


void kill_lamp(edict_t *ent)
{
	gi.configstring (CS_LIGHTS+ent->style, "a");
	ent->health = 50;
	ent->die = LightBecomeDebris;
}

void light_die (edict_t *ent, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point,vec3_t spray)
{

	// Big damage completely takes out model
	if ((ent->health - damage) < -100)
	{
		FX_MakeSparks(ent->s.origin, spray, 2);
		LightBecomeDebris(ent,inflictor,attacker,damage,point);
		return;
	}

	// Lesser damage just kills lightbulb
	if (!(ent->spawnflags & SF_LIGHT_START_OFF))	// Light is on
	{
		FX_MakeSparks(ent->s.origin, spray, 2);
		gi.configstring (CS_LIGHTS+ent->style, "z");
	}

	ent->nextthink = level.time + FRAMETIME*2;
	ent->think = kill_lamp;

}


static void light_use2 (edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->spawnflags & SF_LIGHT_START_OFF)
	{
		gi.configstring (CS_LIGHTS+self->style, "m");
		self->spawnflags &= ~SF_LIGHT_START_OFF;
	}
	else
	{
		gi.configstring (CS_LIGHTS+self->style, "a");
		self->spawnflags |= SF_LIGHT_START_OFF;
	}
}



void alley_wall_use (edict_t *ent, edict_t *other, edict_t *activator)
{

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is off, but it will be on
	{
		SimpleModelSetSequence(ent,"light",SMSEQ_HOLD);

		SimpleModelAddObject(ent,"BEAM");

	}
	else
	{
		SimpleModelSetSequence(ent,"light_off",SMSEQ_HOLD);

		SimpleModelRemoveObject(ent,"BEAM");
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_alley_wall (0 1 0) (-48 -41 -53)(38 41 21)  START_OFF  VULNERABLE  MOVEABLE
A light on the end of a long horizontal pole which should be mounted to a wall.
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - N/A, can't ever move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_alley_wall (edict_t *ent)
{
	VectorSet (ent->mins, -48, -41, -53);
	VectorSet (ent->maxs,  38,  41,  21);

	SimpleModelInit2(ent,&lightModelData[OBJ_ALLEY_WALL],NULL,NULL);

	LightInit(ent);

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is off
	{
		SimpleModelSetSequence(ent,"light_off",SMSEQ_HOLD);

		SimpleModelRemoveObject(ent,"BEAM");
	}

	ent->use = alley_wall_use;

}

void alley_wall_old_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	char *partname = "SPRITEBALL_FLASH01";

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is turning on
	{
		SimpleModelAddObject(ent,partname);
	}
	else										// Light is turning off
	{
		SimpleModelSetSequence(ent,"light_off",SMSEQ_HOLD);
		SimpleModelRemoveObject(ent,partname);
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_alley_wall_old (0 1 0) (-28 -4 -13) (4 4 9)  START_OFF  VULNERABLE  MOVEABLE
An old-fashioned lamp on an arm which is to be mounted on a wall.
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - N/A, can't ever move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_alley_wall_old (edict_t *ent)
{
	VectorSet (ent->mins, -28, -4, -13);
	VectorSet (ent->maxs,  4,   4,  9);

	SimpleModelInit2(ent,&lightModelData[OBJ_ALLEY_WALL_OLD],NULL,NULL);

	LightInit(ent);

	ent->use = alley_wall_old_use;

}

void caged_die (edict_t *ent, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	vec3_t	spray;
	char *partname = "SPRITEBALL_FLASH01";
	char *dir = "objects/light/caged_light";
	char *file = "caged_light";

	VectorSet (spray, 0, 0, 80);	// Goes up

	light_die (ent,inflictor,attacker,damage,point,spray);

	if (!(ent->spawnflags & SF_LIGHT_START_OFF))	// Light is on
	{
		SimpleModelRemoveObject(ent,partname);	// Remove light flare
	}
}


void caged_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	char *partname = "SPRITEBALL_FLASH02";
	char *part2name = "LIGHTBULB01";

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is off, but it will be on
	{
		SimpleModelAddObject(ent,partname);
		SimpleModelAddObject(ent,part2name);
	}
	else
	{
		SimpleModelRemoveObject(ent,partname);
		SimpleModelRemoveObject(ent,part2name);
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_caged (0 1 0) (-3 -3 -10) (3 3 5)  START_OFF  VULNERABLE  MOVEABLE
A small square light which hangs from the wall. It's in a metal cage.
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - N/A, can't ever move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_caged (edict_t *ent)
{
	solid_t solid;
	char *partname = "SPRITEBALL_FLASH02";
	char *part2name = "LIGHTBULB01";

	VectorSet (ent->mins, -3, -3, -10);
	VectorSet (ent->maxs, 3, 3, 5);

	SimpleModelInit2(ent,&lightModelData[OBJ_CAGED],NULL,NULL);

	if (ent->spawnflags & SF_LIGHT_START_OFF)
	{
		SimpleModelRemoveObject(ent,partname);
		SimpleModelRemoveObject(ent,part2name);
	}
	if (ent->spawnflags & SF_LIGHT_VULNERABLE)
		solid = SOLID_BBOX;
	else
		solid = SOLID_NOT;

	LightInit(ent);

	ent->use = caged_use;
	ent->die = caged_die;
}

void chandelier_spin (edict_t *self)
{
	eft_rotate_t	*rotInfo = NULL;
	
	if (rotInfo = FXA_GetRotate(self))
	{
		rotInfo->zRot *= 0.75f;	// Add friction
		if (abs(rotInfo->zRot) < (0.3f * 512))//512 accounts for the shift induced by FXA_SetRotate()
		{
			// stop rotating
			FXA_RemoveRotate(self);
			self->think = NULL;
		}
		else
		{
			self->nextthink = level.time + 0.5f;
		}
	}
}

#define CHANDELIER_MAXSPEED 5

void chandelier_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	eft_rotate_t	*rotInfo = NULL;
	float			fMult;
	vec3_t			towherehit, cross, pathdir, up, angVel = {0,0,0};

	SimpleModelSetSequence(ent,lightModelData[OBJ_CHANDELIER].file,SMSEQ_HOLD);

	fMult = 0.1f * damage;

	VectorSet(up, 0, 0, 1);
	VectorSubtract(other->s.origin, ent->s.origin, pathdir);
	VectorNormalize(pathdir);
	VectorSubtract(wherehit,ent->s.origin, towherehit);
	CrossProduct(pathdir, towherehit, cross);
	if (DotProduct(cross, up) < 0)
	{
		if (rotInfo = FXA_GetRotate(ent))
		{
			// adding to an existing rotation
			rotInfo->zRot += fMult*512;//512 accounts for the shift induced by FXA_SetRotate()
		}
		else
		{
			// need to create a rotating effect
			angVel[ROLL] = CHANDELIER_MAXSPEED;
			FXA_SetRotate(ent, angVel, ent->ghoulInst);
		}
	}
	else if (DotProduct(cross, up) > 0)
	{
		if (rotInfo = FXA_GetRotate(ent))
		{
			// adding to an existing rotation
			rotInfo->zRot += (-fMult)*512;//512 accounts for the shift induced by FXA_SetRotate()
		}
		else
		{
			// need to create a rotating effect
			angVel[ROLL] = -CHANDELIER_MAXSPEED;
			FXA_SetRotate(ent, angVel, ent->ghoulInst);
		}
	}

	if (rotInfo)
	{
		if (rotInfo->zRot > (CHANDELIER_MAXSPEED * 512))
			rotInfo->zRot = CHANDELIER_MAXSPEED * 512;
		else if (rotInfo->zRot < (-CHANDELIER_MAXSPEED * 512))
			rotInfo->zRot = -CHANDELIER_MAXSPEED * 512;
	}

	// need to think pretty often cuz that's where we'll slow down the light's rotation to simulate friction
	ent->think = chandelier_spin;
	ent->nextthink = level.time + 0.5f;
}

void chandelier_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is off, but it will be on
	{
		SimpleModelAddObject(ent,"SPRITEBALL_FLASH01");
		SimpleModelAddObject(ent,"SPRITEBALL_FLASH02");
		SimpleModelAddObject(ent,"SPRITEBALL_FLASH03");
		SimpleModelAddObject(ent,"SPRITEBALL_FLASH04");
	}
	else
	{
		SimpleModelRemoveObject(ent,"SPRITEBALL_FLASH01");
		SimpleModelRemoveObject(ent,"SPRITEBALL_FLASH02");
		SimpleModelRemoveObject(ent,"SPRITEBALL_FLASH03");
		SimpleModelRemoveObject(ent,"SPRITEBALL_FLASH04");
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_chandelier (0 1 0) (-24 -21 -0) (25 21 51)  START_OFF  VULNERABLE  MOVEABLE
A chandelier light which hangs from the ceiling.
------ KEYS ------
Light - how bright it is (default 300)
Skin - 
0 - gold chandelier
1 - iron chandelier
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - N/A, can't ever move
------ SPECIALS ------
- If targeted, will toggle between on and off.
- Swings and spins when shot
*/
void SP_light_generic_chandelier (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -24, -21, -0);
	VectorSet (ent->maxs,  25,  21, 51);

	ent->pain = chandelier_pain;

	if (!(ent->spawnflags & SF_LIGHT_VULNERABLE))
		ent->health = 999999;

	ent->spawnflags |= SF_LIGHT_VULNERABLE;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "light2";
		break;
	default :
		skinname = "light";
		break;
	}

	SimpleModelInit2(ent,&lightModelData[OBJ_CHANDELIER],skinname,NULL);

	LightInit(ent);

	ent->ghoulInst->Pause(level.time);

	ent->use = chandelier_use;
}


void desk_lamp_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	char *partname = "SPRITEBALL_FLASH01";

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is off, but it will be on
	{
		SimpleModelSetSequence(ent,"lamp",SMSEQ_HOLD);
		SimpleModelAddObject(ent,partname);
	}
	else
	{
		SimpleModelSetSequence(ent,"lamp_off",SMSEQ_HOLD);
		SimpleModelRemoveObject(ent,partname);
	}

	light_use2(ent,other,activator);

}



/*QUAKED light_generic_desk_lamp (0 1 0) (-6 -6 -5) (6 6 5)  START_OFF  VULNERABLE  MOVEABLE
A small desk lamp.
------ KEYS ------ 
Light - how bright it is (default 300)
Skin - skin of model
0 - bronze
1 - silver
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - will move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_desk_lamp (edict_t *ent)
{
	char * skinname;

	VectorSet (ent->mins, -6, -6, -5);
	VectorSet (ent->maxs, 6, 6, 5);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "lightsilver";
		break;
	default :
		skinname = "light";
		break;
	}

	SimpleModelInit2(ent,&lightModelData[OBJ_DESK_LAMP],skinname,NULL);

	LightInit(ent);

	ent->use = desk_lamp_use;
}

void emergency_light_sequence (edict_t *ent)
{
	ggObjC *simpObj;
	GhoulID simpSeq;
	ggOinstC* myInstance;

	if (!ent->ghoulInst)
	{
		return;
	}

	simpObj = game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());
	if (!simpObj)
	{
		return;
	}

	//changed this to false, can't cache new seqs in after instances are created --ss
	simpSeq = simpObj->FindSequence(lightModelData[OBJ_EMERGENCY].file);

	if (!simpSeq)
	{
		return;
	}

	myInstance = simpObj->FindOInst(ent->ghoulInst);
	if (!myInstance)
	{
		return;
	}

	// Play sequence
	myInstance->PlaySequence(simpSeq, level.time);

}

void emergency_use (edict_t *ent, edict_t *other, edict_t *activator)
{

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is off, but it will be on
	{
		SimpleModelSetSequence(ent,"emergency",SMSEQ_LOOP);
		ent->s.sound = gi.soundindex("Ambient/Models/Alarm/alarm1.wav");
		ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		ent->think = emergency_light_sequence;
		ent->nextthink = level.time + (rand() % 10) * .1;
	}
	else
	{
		SimpleModelSetSequence(ent,"emergency_off",SMSEQ_HOLD);
		ent->s.sound = 0;
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_emergency (0 1 0) (-6 -6 -5) (6 6 5)  START_OFF  VULNERABLE  MOVEABLE
A flashing emergency light
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - will move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_emergency (edict_t *ent)
{
	VectorSet (ent->mins, -6, -6, -5);
	VectorSet (ent->maxs, 6, 6, 5);

	SimpleModelInit2(ent,&lightModelData[OBJ_EMERGENCY],NULL,NULL);

	LightInit(ent);

	gi.soundindex("Ambient/Models/Alarm/alarm1.wav");
	if (ent->spawnflags & SF_LIGHT_START_OFF)
	{
		SimpleModelSetSequence(ent,"emergency_off",SMSEQ_HOLD);
	}
	else
	{
		ent->s.sound = gi.soundindex("Ambient/Models/Alarm/alarm1.wav");
		ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		ent->think = emergency_light_sequence;
		ent->nextthink = level.time + (rand() % 5) * .1;
	}

	SimpleModelScale(ent,1.75f);

	ent->use = emergency_use;

}
void emergency_light_blue_sequence (edict_t *ent)
{
	ggObjC *simpObj;
	GhoulID simpSeq;
	ggOinstC* myInstance;

	if (!ent->ghoulInst)
	{
		return;
	}

	simpObj = game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());
	if (!simpObj)
	{
		return;
	}

	//changed this to false, can't cache new seqs in after instances are created --ss
	simpSeq = simpObj->FindSequence(lightModelData[OBJ_EMERGENCY_BLUE].file);

	if (!simpSeq)
	{
		return;
	}

	myInstance = simpObj->FindOInst(ent->ghoulInst);
	if (!myInstance)
	{
		return;
	}

	// Play sequence
	myInstance->PlaySequence(simpSeq, level.time);

}

void emergency_blue_use (edict_t *ent, edict_t *other, edict_t *activator)
{

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is off, but it will be on
	{
		SimpleModelSetSequence(ent,"emergency",SMSEQ_LOOP);
		ent->s.sound = gi.soundindex("Ambient/Models/Alarm/alarm1.wav");
		ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		ent->think = emergency_light_sequence;
		ent->nextthink = level.time + (rand() % 10) * .1;
	}
	else
	{
		SimpleModelSetSequence(ent,"emergency_off",SMSEQ_HOLD);
		ent->s.sound = 0;
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_emergency_blue (0 1 0) (-6 -6 -5) (6 6 5)  START_OFF  VULNERABLE  MOVEABLE
A flashing blue emergency light
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - will move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_emergency_blue (edict_t *ent)
{
	VectorSet (ent->mins, -6, -6, -5);
	VectorSet (ent->maxs, 6, 6, 5);

	SimpleModelInit2(ent,&lightModelData[OBJ_EMERGENCY_BLUE],NULL,NULL);

	LightInit(ent);
	gi.soundindex("Ambient/Models/Alarm/alarm1.wav");
	if (ent->spawnflags & SF_LIGHT_START_OFF)
	{
		gi.soundindex("Ambient/Models/Alarm/alarm1.wav");
		SimpleModelSetSequence(ent,"emergency_off",SMSEQ_HOLD);
	}
	else
	{
		ent->s.sound = gi.soundindex("Ambient/Models/Alarm/alarm1.wav");
		ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		ent->think = emergency_light_blue_sequence;
		ent->nextthink = level.time + (rand() % 5) * .1;
	}

	SimpleModelScale(ent,1.75f);

	ent->use = emergency_blue_use;

}

/*QUAKED light_generic_halfshell (0 1 0) (-2 -7 -2) (2 7 2)  START_OFF  VULNERABLE  MOVEABLE
An old-fashioned lamp on an arm which is to be mounted on a wall.
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - N/A, can't ever move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_halfshell (edict_t *ent)
{

	VectorSet (ent->mins, -2, -7, -2);
	VectorSet (ent->maxs,  2,  7,  2);

	SimpleModelInit2(ent,&lightModelData[OBJ_HALFSHELL],NULL,NULL);

	LightInit(ent);

}


void hanging_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	int per;
	char soundName[MAX_QPATH];

	per = gi.irand(1,3);
	Com_sprintf(soundName,MAX_QPATH,"Weapons/FX/Rics/ric%d.wav",per);

	gi.sound (ent, CHAN_VOICE, gi.soundindex(soundName), .6, ATTN_NORM, 0);

	if (ent->spawnflags & SF_LIGHT_START_OFF)
	{
		SimpleModelSetSequence(ent,"hanging_off",SMSEQ_HOLD);
	}
	else
	{
		SimpleModelSetSequence(ent,"hanging",SMSEQ_HOLD);
	}

}


void hanging_use (edict_t *ent, edict_t *other, edict_t *activator)
{

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is off, but it will be on
	{
		SimpleModelSetSequence(ent,"hanging",SMSEQ_HOLD);

		ent->ghoulInst->Pause(level.time);

		SimpleModelAddObject(ent,"SPRITEBALL_FLASH01");

	}
	else
	{
		SimpleModelSetSequence(ent,"hanging_off",SMSEQ_HOLD);

		ent->ghoulInst->Pause(level.time);

		SimpleModelRemoveObject(ent,"SPRITEBALL_FLASH01");
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_hanging (0 1 0)  (-10 -10 -6) (10 10 44)  START_OFF  VULNERABLE  MOVEABLE
Square light hanging from a cord.  Attach to ceiling.
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - will move
------ SPECIALS ------
- If targeted, will toggle between on and off.
- Swings when shot
*/
void SP_light_generic_hanging (edict_t *ent)
{


	VectorSet (ent->mins, -10, -10, -6);
	VectorSet (ent->maxs,  10,  10,  44);

	ent->pain = hanging_pain;

	if (!(ent->spawnflags & SF_LIGHT_VULNERABLE))
		ent->health = 999999;

	ent->spawnflags |= SF_LIGHT_VULNERABLE;

	SimpleModelInit2(ent,&lightModelData[OBJ_HANGING],NULL,NULL);

	LightInit(ent);

	if (ent->spawnflags & SF_LIGHT_START_OFF)
	{
		SimpleModelSetSequence(ent,"hanging_off",SMSEQ_HOLD);

		ent->ghoulInst->Pause(level.time);

		SimpleModelRemoveObject(ent,"SPRITEBALL_FLASH01");
	}
	else
	{
		ent->ghoulInst->Pause(level.time);
	}

	ent->use = hanging_use;
}


void lantern_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is off
		SimpleModelSetSequence(ent,"lantern_off",SMSEQ_HOLD);
	else
		SimpleModelSetSequence(ent,lightModelData[OBJ_LANTERN].file,SMSEQ_HOLD);
}

void lantern_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is off, but it will be on
	{
		SimpleModelSetSequence(ent,"lantern",SMSEQ_HOLD);
		ent->ghoulInst->Pause(level.time);
	}
	else
	{
		SimpleModelSetSequence(ent,"lantern_off",SMSEQ_HOLD);
		ent->ghoulInst->Pause(level.time);
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_lantern (0 1 0) (-7 -6 -22) (7 6 22)  START_OFF  VULNERABLE  MOVEABLE
A hanging Chinese lantern.
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - will move
------ SPECIALS ------
- If targeted, will toggle between on and off.
- Swings when shot
*/
void SP_light_generic_lantern (edict_t *ent)
{
	VectorSet (ent->mins, -7, -6, -22);
	VectorSet (ent->maxs, 7, 6, 22);

	ent->pain = lantern_pain;

	if (!(ent->spawnflags & SF_LIGHT_VULNERABLE))
		ent->health = 999999;

	ent->spawnflags |= SF_LIGHT_VULNERABLE;

	SimpleModelInit2(ent,&lightModelData[OBJ_LANTERN],NULL,NULL);

	LightInit(ent);

	ent->ghoulInst->Pause(level.time);

	ent->use = lantern_use;
}

void lantern2_die (edict_t *ent, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	vec3_t	spray;
	char *partname = "SPRITEBALL_FLASH01";

	VectorSet (spray, 0, 0, 80);	// Goes up

	light_die (ent,inflictor,attacker,damage,point,spray);

	if (!(ent->spawnflags & SF_LIGHT_START_OFF))	// Light is on
	{
		SimpleModelRemoveObject(ent,partname);	// Remove light flare
	}
}


void lantern2_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	char *partname = "SPRITEBALL_FLASH01";

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is on
	{
		SimpleModelSetSequence(ent,"lantern2",SMSEQ_HOLD);
		SimpleModelAddObject(ent,partname);
	}
	else
	{
		SimpleModelSetSequence(ent,"lantern2_off",SMSEQ_HOLD);
		SimpleModelRemoveObject(ent,partname);
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_lantern2 (0 1 0) (-8 -7 -9) (4 7 8)  START_OFF  VULNERABLE  MOVEABLE
A square light attached to the wall by a small pole.
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - will move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_lantern2 (edict_t *ent)
{
	solid_t solid;
	char *partname = "SPRITEBALL_FLASH01";

	VectorSet (ent->mins, -8, -7, -9);
	VectorSet (ent->maxs, 8, 7, 8);

	SimpleModelInit2(ent,&lightModelData[OBJ_LANTERN2],NULL,NULL);

	if (ent->spawnflags & SF_LIGHT_START_OFF)
		SimpleModelRemoveObject(ent,partname);

	if (ent->spawnflags & SF_LIGHT_VULNERABLE)
		solid = SOLID_BBOX;
	else
		solid = SOLID_NOT;

	LightInit(ent);

	ent->use = lantern2_use;
	ent->die = lantern2_die;
}


void lantern3_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	char *partname = "SPRITEBALL_FLASH01";

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is on
	{
		SimpleModelSetSequence(ent,"lantern3",SMSEQ_HOLD);
		SimpleModelAddObject(ent,partname);
	}
	else
	{
		SimpleModelSetSequence(ent,"lantern3",SMSEQ_HOLD);
		SimpleModelRemoveObject(ent,partname);
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_lantern3 (0 1 0) (-7 -7 -13) (4 7 8)  START_OFF  VULNERABLE  MOVEABLE
A square light attached to the wall by a small pole. Low polycount.
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - will move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_lantern3 (edict_t *ent)
{
	solid_t solid;
	char *partname = "SPRITEBALL_FLASH01";

	VectorSet (ent->mins, -7, -7, -13);
	VectorSet (ent->maxs, 4, 7, 8);

	SimpleModelInit2(ent,&lightModelData[OBJ_LANTERN3],NULL,NULL);

	if (ent->spawnflags & SF_LIGHT_START_OFF)
		SimpleModelRemoveObject(ent,partname);

	if (ent->spawnflags & SF_LIGHT_VULNERABLE)
		solid = SOLID_BBOX;
	else
		solid = SOLID_NOT;

	LightInit(ent);

	ent->use = lantern3_use;
}

void military_door_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	char *partname = "SPRITEBALL_FLASH01";
	char *partname2 = "LIGHT_BULB01";

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is on
	{
		SimpleModelAddObject(ent,partname);
		SimpleModelAddObject(ent,partname2);
	}
	else
	{
		SimpleModelRemoveObject(ent,partname);
		SimpleModelRemoveObject(ent,partname2);
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_military_door (0 1 0) (-6 -6 -2) (6 9 3)  START_OFF  VULNERABLE  MOVEABLE
A little bent light to be hung above doors.
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
--------SPAWNFLAGS----------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - N/A, won't ever move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_military_door (edict_t *ent)
{
	solid_t solid;

	VectorSet (ent->mins, -6, -6, -2);
	VectorSet (ent->maxs,  6,  9,  3);

	SimpleModelInit2(ent,&lightModelData[OBJ_MILITARY_DOOR],NULL,NULL);

	if (ent->spawnflags & SF_LIGHT_VULNERABLE)
		solid = SOLID_BBOX;
	else
		solid = SOLID_NOT;

	LightInit(ent);

	ent->use = military_door_use;
}


void military_door_outside_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	char *partname = "SPRITEBALL_FLASH01";
	char *partname2 = "BEAM01";
	char *partname3 = "LIGHT_BULB01";

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is on
	{
		SimpleModelAddObject(ent,partname);
		SimpleModelAddObject(ent,partname2);
		SimpleModelAddObject(ent,partname3);
	}
	else
	{
		SimpleModelRemoveObject(ent,partname);
		SimpleModelRemoveObject(ent,partname2);
		SimpleModelRemoveObject(ent,partname3);
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_military_door_outside (0 1 0) (-19 -22 -8) (19 27 10)  START_OFF  VULNERABLE  MOVEABLE
A big bent light to be hung above doors.
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - N/A, won't ever move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_military_door_outside (edict_t *ent)
{
	solid_t solid;

	VectorSet (ent->mins, -19, -22, -8);
	VectorSet (ent->maxs,  19,  27,  10);

	SimpleModelInit2(ent,&lightModelData[OBJ_MILITARY_DOOR_OUTSIDE],NULL,NULL);

	if (ent->spawnflags & SF_LIGHT_VULNERABLE)
		solid = SOLID_BBOX;
	else
		solid = SOLID_NOT;

	LightInit(ent);

	ent->use = military_door_outside_use;
}

void shop_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	int per;
	char soundName[60];

	per = gi.irand(1,3);
	sprintf(soundName,"Weapons/FX/Rics/ric%d.wav",per);

	gi.sound (ent, CHAN_VOICE, gi.soundindex(soundName), .6, ATTN_NORM, 0);

	if (ent->spawnflags & SF_LIGHT_START_OFF)
	{
		SimpleModelSetSequence(ent,"shop_off",SMSEQ_HOLD);
	}
	else
	{
		SimpleModelSetSequence(ent,"shop",SMSEQ_HOLD);
	}

}


void shop_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is on
	{
		SimpleModelSetSequence(ent,"shop",SMSEQ_HOLD);
		ent->ghoulInst->Pause(level.time);
	}
	else
	{
		SimpleModelSetSequence(ent,"shop_off",SMSEQ_HOLD);
		ent->ghoulInst->Pause(level.time);
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_shop (0 1 0) (-19 -23 -4)  (19 22 32)  START_OFF  VULNERABLE  MOVEABLE
Rectangular light hanging from two poles.  Attach to ceiling.
------ KEYS ------
Light - how bright it is (default 300)
Skin - 
0 - normal
1 - wire mesh over it
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
--------SPAWNFLAGS----------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - N/A, can't ever move
------ SPECIALS ------
- If targeted, will toggle between on and off.
- Swings when shot
*/
void SP_light_generic_shop (edict_t *ent)
{
	char * skinname;

	VectorSet (ent->mins, -19, -23,  -4);
	VectorSet (ent->maxs,  19,  22,  32);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "light2";
		break;
	default :
		skinname = "light";
		break;
	}

	if (!(ent->spawnflags & SF_LIGHT_VULNERABLE))
		ent->health = 999999;

	ent->spawnflags |= SF_LIGHT_VULNERABLE;

	SimpleModelInit2(ent,&lightModelData[OBJ_SHOP],skinname,NULL);

	LightInit(ent);

	ent->ghoulInst->Pause(level.time);

	ent->use = shop_use;
	ent->pain = shop_pain;
}

void street_modern_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	char *partname = "SPRITEBALL_GLOW";
	char *partname2 = "BEAM";

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is on
	{
		SimpleModelAddObject(ent,partname);
		SimpleModelAddObject(ent,partname2);
		SimpleModelSetSequence(ent,"modern",SMSEQ_HOLD);
	}
	else
	{
		SimpleModelSetSequence(ent,"modern_off",SMSEQ_HOLD);
		SimpleModelRemoveObject(ent,partname);
		SimpleModelRemoveObject(ent,partname2);
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_street_modern (0 1 0)  (-88 -75 -218)  (75 75 17)  START_OFF  VULNERABLE  MOVEABLE
A tall street light.  Sits on ground.
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - N/A, can't ever move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_street_modern (edict_t *ent)
{
	VectorSet (ent->mins, -88, -75, -218);
	VectorSet (ent->maxs,  75,  75,   17);

	SimpleModelInit2(ent,&lightModelData[OBJ_STREET_MODERN],NULL,NULL);

	LightInit(ent);

	ent->use = street_modern_use;
}

void street_old_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	char *partname = "SPRITEBALL_FLASH";

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is on
	{
		SimpleModelAddObject(ent,partname);
		SimpleModelSetSequence(ent,"old",SMSEQ_HOLD);
	}
	else
	{
		SimpleModelSetSequence(ent,"off",SMSEQ_HOLD);
		SimpleModelRemoveObject(ent,partname);
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_street_old (0 1 0) (-7 -7 -48) (7 7 48)  START_OFF  VULNERABLE  MOVEABLE
An old fashioned street light (vertical pole/lamp)
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - N/A, can't ever move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_street_old (edict_t *ent)
{
	VectorSet (ent->mins, -7, -7, -48);
	VectorSet (ent->maxs, 7, 7, 48);

	SimpleModelInit2(ent,&lightModelData[OBJ_STREET_OLD],NULL,NULL);

	LightInit(ent);

	ent->use = street_old_use;
}

void lamp_hang_swing (edict_t *ent)
{
	if (ent->spawnflags & HANGING_NOANIM)	// No animating
	{
		ent->ghoulInst->Pause(level.time);
	}
	else 
	{
		SimpleModelSetSequence(ent,lightModelData[OBJ_LAMP_HANG].file,SMSEQ_LOOP);
	}
}

HanginLightCallback  theHanginLightCallback;

bool HanginLightCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	edict_t* self = (edict_t*)ent;

	lamp_hang_swing(self);

	return true;
}

void lamp_hang_painstop (edict_t *ent)
{
	GhoulID	tempNote=0;
	ggObjC	*cLight;

	if (!ent->ghoulInst)
	{
		return;
	}

	cLight = game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());
	tempNote=cLight->GetMyObject()->FindNoteToken("EOS");

	ent->ghoulInst->RemoveNoteCallBack(&theHanginLightCallback,tempNote);
	lamp_hang_swing (ent);
}

void lamp_hang_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	GhoulID	tempNote=0;
	ggObjC	*cLight;

	if (!ent->ghoulInst)
	{
		return;
	}

	if (ent->spawnflags & SF_LIGHT_START_OFF)	
		SimpleModelSetSequence(ent,"shot_off",SMSEQ_HOLD);
	else
		SimpleModelSetSequence(ent,"shot",SMSEQ_HOLD);

	// register a callback for the end of the pain sequence
	cLight = game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());
	tempNote=cLight->GetMyObject()->FindNoteToken("EOS");

	if (tempNote && ent->ghoulInst)
	{
		ent->ghoulInst->AddNoteCallBack(&theHanginLightCallback,tempNote);
	}

}

void lamp_hang_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	char *partname = "SPRITEBALL_FLASH01";
	char *partname2 = "SPRITEBALL_FLASH02";

	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is on
	{
		SimpleModelAddObject(ent,partname);
		SimpleModelAddObject(ent,partname2);
		if (ent->spawnflags & HANGING_NOANIM)	// No animating
			SimpleModelSetSequence(ent,"lamp",SMSEQ_HOLD);
		else
			SimpleModelSetSequence(ent,"lamp",SMSEQ_LOOP);
	}
	else
	{
		SimpleModelRemoveObject(ent,partname);
		SimpleModelRemoveObject(ent,partname2);
		if (ent->spawnflags & HANGING_NOANIM)	// No animating
			SimpleModelSetSequence(ent,"lamp_off",SMSEQ_HOLD);
		else
			SimpleModelSetSequence(ent,"lamp_off",SMSEQ_LOOP);
	}

	light_use2(ent,other,activator);
}

/*QUAKED light_generic_lamp_hang (0 1 0) (-6 -7 -10) (5 7 15)  START_OFF  VULNERABLE  MOVEABLE  TABLE  NOANIM
A train swinging lamp. Can be hung from ceiling or placed on a table.
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - N/A, can't ever move
TABLE - can be placed on a table
NOANIM - won't animate
------ SPECIALS ------
- If targeted, will toggle between on and off.
- Swings when shot
*/
void SP_light_generic_lamp_hang(edict_t *ent)
{
	VectorSet (ent->mins, -6, -7, -10);
	VectorSet (ent->maxs,  5,  7,  15);

	if (!(ent->spawnflags & SF_LIGHT_VULNERABLE))
		ent->health = 999999;

	ent->spawnflags |= SF_LIGHT_VULNERABLE;

	ent->pain = lamp_hang_pain;

	SimpleModelInit2(ent,&lightModelData[OBJ_LAMP_HANG],NULL,NULL);

	LightInit(ent);

	if (ent->spawnflags & HANGING_ONTABLE)	// Sit still on a table
	{
		SimpleModelRemoveObject(ent,"HANDLE_LANTERN");

		SimpleModelRemoveObject(ent,"ROPE_LANTERN");

		ent->ghoulInst->Pause(level.time);

		ent->pain = NULL;
		gi.linkentity (ent);

	}
	else if (ent->spawnflags & HANGING_NOANIM)	// No animating
	{
		ent->ghoulInst->Pause(level.time);
	}
	else
	{
		ent->think = lamp_hang_swing;
		ent->nextthink = level.time + (rand() % 5);
	}

	ent->use = lamp_hang_use;
}

// called when a candle is shot off of the candelabra
void candelabra_pain (edict_t *self,int partLost, vec3_t boltPos)
{
	// Throw debris and make it go away
	FX_ThrowDebris(boltPos, vec3_up, 5, DEBRIS_SM, MAT_WALL_WHITE, 0,0,0, SURF_BLOOD);
	FX_SmokePuff(boltPos,120,120,120,200);
}



void candelabra_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	char			*partname = "SPRITEBALL_FLASH03";
	char			*partname2 = "SPRITEBALL_FLAME01";
	IGhoulInst*		inst = NULL;
	int				i = 0;
	boltInstInfo_c	*instInfo = NULL;

	if (!ent->objInfo || !(instInfo = (boltInstInfo_c*)ent->objInfo->GetInfo(OIT_BOLTINST)) )
	{
		return;
	}
	if (ent->spawnflags & SF_LIGHT_START_OFF)	// Light is turning on
	{
		for (i = 1; i <= 3; i++)
		{
			if ( instInfo && instInfo->IsOn(i) && (inst = SimpleModelGetBolt(ent, i)) )
			{
				SimpleModelAddObject2(inst,partname);
				SimpleModelAddObject2(inst,partname2);
			}
		}
	}
	else										// Light is turning off
	{
		for (i = 1; i <= 3; i++)
		{
			if (inst = SimpleModelGetBolt(ent, i))
			{
				SimpleModelRemoveObject2(inst,partname);
				SimpleModelRemoveObject2(inst,partname2);
			}
		}
	}

	light_use2(ent,other,activator);
}


/*QUAKED light_generic_candelabra (0 1 0) (-11 -11 -55) (11 11 23)  START_OFF  VULNERABLE  MOVEABLE
Tall candelabra, intended for floor placement in only the finest Transylvanian homes.
------ KEYS ------
Skin - 
0 - gold candelabra
1 - iron candelabra
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - N/A, won't ever move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_candelabra(edict_t *ent)
{
	Matrix4			mat1,mat2;
	char			*skinname;
	int				nRand = gi.irand(0, 2);

	VectorSet (ent->mins, -11, -11, -55);
	VectorSet (ent->maxs,  11,  11,  23);

	ent->spawnflags |= SF_LIGHT_VULNERABLE;

	if (ent->spawnflags & SF_INVULNERABLE)
	{
		ent->takedamage = DAMAGE_NO;
	}
	else
	{
		ent->takedamage = DAMAGE_YES;
	}

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "candle2";
		break;
	default :
		skinname = "candle";
		break;
	}

	// Set up object to break apart when shot
	Obj_partbreaksetup (BLPD_CANDELABRA,&lightModelData[OBJ_CANDELABRA],
		lightObjBoltPartsData, lightObjBreak);

	SimpleModelInit2(ent,&lightModelData[OBJ_CANDELABRA],skinname,NULL);

	// bolt the three different candle models on in random order
	SimpleModelAddBolt(ent,lightModelData[OBJ_CANDELABRA],"DUMMY01",
						lightModelData[OBJ_CANDLE + (nRand++ % 3)],"DUMMY01",NULL);
	SimpleModelAddBolt(ent,lightModelData[OBJ_CANDELABRA],"DUMMY02",
						lightModelData[OBJ_CANDLE + (nRand++ % 3)],"DUMMY01",NULL);
	SimpleModelAddBolt(ent,lightModelData[OBJ_CANDELABRA],"DUMMY03",
						lightModelData[OBJ_CANDLE + (nRand % 3)],"DUMMY01",NULL);
	ent->pain = Obj_partpain;


	LightInit(ent);

	ent->use = candelabra_use;
}

/*QUAKED light_generic_candelabra_small (0 1 0)  (-10 -5 -17)  (10 5 0)  START_OFF  VULNERABLE  MOVEABLE
Candelabra for a table
------ KEYS ------
Light - how bright it is (default 300)
Style - type of light given off  
0 - Normal, unflickering
1 - Flickering 1
2 - Slow strong Pulse
3 - Candle 1
4 - Fast Stribe
5 - Gentle Pulse 1
6 - Flicker  2
7 - Candle 2
8 - Candle 3
9 - Slow Strobe
10 - Flourscent Flicker
11 - Slow Pulse not Fade to Black
12 - Fast Pulse
13 - Special sky lightning
------ SPAWNFLAGS ------
START_OFF - Will turn on if triggered
VULNERABLE - can be destroyed
MOVEABLE - N/A, can't ever move
------ SPECIALS ------
- If targeted, will toggle between on and off.
*/
void SP_light_generic_candelabra_small (edict_t *ent)
{
	VectorSet (ent->mins, -10, -5, -17);
	VectorSet (ent->maxs,  10,  5,   0);


	SimpleModelInit2(ent,&lightModelData[OBJ_CANDELABRA_SMALL],NULL,NULL);

	LightInit(ent);
}
