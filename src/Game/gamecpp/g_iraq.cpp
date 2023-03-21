/******************************************************
 * Objects between Iraq and a hard place (if you think about it, it really is funny)  *
 ******************************************************/

#include "g_local.h"
#include "g_obj.h"
#include "callback.h"

#define FRUITSTAND_MELONS  4
#define FRUITSTAND_ORANGES 8
#define FRUITSTAND_NOFRUIT 16

#define HLTH_IRAQ_BASKET			50
#define HLTH_IRAQ_CART_CAMEL		300
#define HLTH_IRAQ_FLATBED			500
#define HLTH_IRAQ_FLATBED_WHEEL		500
#define HLTH_IRAQ_FRUIT_MELON		50
#define HLTH_IRAQ_FRUIT_ORANGE		10
#define HLTH_IRAQ_FRUIT_STAND		125
#define HLTH_IRAQ_PARTITION			400
#define HLTH_IRAQ_POTTERY			25
#define HLTH_IRAQ_SPRINKLER			500
#define HLTH_IRAQ_STEALTH			5000
#define HLTH_IRAQ_TREE_PALM			1000
#define HLTH_IRAQ_VALVE				300


#define FLATBED_TRIGGERED	4
#define FLATBED_LIGHTS		8
#define FLATBED_WHEELS_ONLY	16

#define FLATBED_BRAKE_FACTOR	3


#define VALVE_PORTCULLIS			(1<<7)

extern void debug_drawbox(edict_t* self,vec3_t vOrigin, vec3_t vMins, vec3_t vMaxs, int nColor);

extern void drawthink(edict_t *ent);


//  misc_iraq...
void SP_misc_iraq_basket (edict_t *ent);
void SP_misc_iraq_cart_camel (edict_t *ent);
void SP_misc_iraq_flatbed (edict_t *ent);
void SP_misc_iraq_fruit_melon (edict_t *ent);
void SP_misc_iraq_fruit_orange (edict_t *ent);
void SP_misc_iraq_fruit_stand (edict_t *ent);
void SP_misc_iraq_partition (edict_t *ent);
void SP_misc_iraq_pottery (edict_t *ent);
void SP_misc_iraq_sprinkler (edict_t *ent);
void SP_misc_iraq_stealth (edict_t *ent);
void SP_misc_iraq_tree_palm (edict_t *ent);
void SP_misc_iraq_valve (edict_t *ent);

spawn_t iraqSpawns[] =
{
	//	misc_iraq...
	{"misc_iraq_basket",				SP_misc_iraq_basket},
	{"misc_iraq_cart_camel",			SP_misc_iraq_cart_camel},
	{"misc_iraq_flatbed",				SP_misc_iraq_flatbed},
	{"misc_iraq_fruit_melon",			SP_misc_iraq_fruit_melon},
	{"misc_iraq_fruit_orange",			SP_misc_iraq_fruit_orange},
	{"misc_iraq_fruit_stand",			SP_misc_iraq_fruit_stand},
	{"misc_iraq_partition",				SP_misc_iraq_partition},
	{"misc_iraq_pottery",				SP_misc_iraq_pottery},
	{"misc_iraq_sprinkler",				SP_misc_iraq_sprinkler},
	{"misc_iraq_stealth",				SP_misc_iraq_stealth},
	{"misc_iraq_tree_palm",				SP_misc_iraq_tree_palm},
	{"misc_iraq_valve",					SP_misc_iraq_valve},
	{NULL,								NULL},
};

typedef enum
{
	OBJ_BASKET = 0,
	OBJ_CART_CAMEL,
	OBJ_FLATBED_OFF,
	OBJ_FLATBED_WHEEL,
	OBJ_FLATBED_ON,
	OBJ_FRUIT_MELON,
	OBJ_FRUIT_ORANGE,
	OBJ_FRUIT_STAND,
	OBJ_LIGHT_BEAM,
	OBJ_PARTITION,
	OBJ_POTTERY,
	OBJ_SPRINKLER,
	OBJ_STEALTH,
	OBJ_TREE_PALM,
	OBJ_VALVE,
	MAX_OBJS
};


modelSpawnData_t iraqModelData[MD_IRAQ_SIZE] =
{	
//      dir							file				surfaceType			material			health						solid			material file  cnt  scale
"objects/iraq/basket",				"basket",			SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_IRAQ_BASKET,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_BASKET
"objects/iraq/cart_camel",			"cart",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_IRAQ_CART_CAMEL,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_CART_CAMEL
"objects/iraq/flatbed",				"off",				SURF_METAL,			MAT_METAL_LGREY,	HLTH_IRAQ_FLATBED,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_FLATBED_OFF
"objects/iraq/flatbed",				"tire",				SURF_METAL,			MAT_METAL_LGREY,	HLTH_IRAQ_FLATBED_WHEEL,	SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_FLATBED_WHEEL
"objects/iraq/flatbed",				"on",				SURF_METAL,			MAT_METAL_LGREY,	HLTH_IRAQ_FLATBED,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_FLATBED_ON
"objects/iraq/fruit_melon",			"melon",			SURF_LIQUID_RED,	MAT_DEFAULT,		HLTH_IRAQ_FRUIT_MELON,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_FRUIT_MELON
"objects/iraq/fruit_orange",		"orange",			SURF_LIQUID_ORANGE,	MAT_DEFAULT,		HLTH_IRAQ_FRUIT_MELON,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_FRUIT_ORANGE
"objects/iraq/fruit_stand",			"fruit_stand",		SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_IRAQ_FRUIT_STAND,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_FRUIT_STAND
"objects/generic/beam",				"beam",				SURF_NONE,			MAT_NONE,			0,							SOLID_NOT,		NULL,			0,	0.0,	NULL,	// OBJ_LIGHT_BEAM
"objects/iraq/partition",			"partition",		SURF_NONE,			MAT_DEFAULT,		HLTH_IRAQ_PARTITION,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_PARTITION
"objects/iraq/pottery",				"pottery",			SURF_SAND_LBROWN,	MAT_DEFAULT,		HLTH_IRAQ_POTTERY,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_POTTERY
"objects/iraq/sprinklers",			"sprinklers",		SURF_METAL,			MAT_METAL_LGREY,	HLTH_IRAQ_SPRINKLER,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_SPRINKLER
"objects/iraq/stealth",				"stealth",			SURF_METAL,			MAT_METAL_LGREY,	HLTH_IRAQ_STEALTH,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_STEALTH
"objects/iraq/tree_palm",			"tree",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_IRAQ_TREE_PALM,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_TREE_PALM
"objects/iraq/valve",				"valve",			SURF_METAL,			MAT_METAL_LGREY,	HLTH_IRAQ_VALVE,			SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_VALVE
};

void EntToWorldMatrix(vec3_t org, vec3_t angles, Matrix4 &m);

/*QUAKED misc_iraq_basket (1 .5 0) (-6 -7 -18) (7 6 15)   INVULNERABLE  NOPUSH x x x x FLUFF
A tall thin wicker basket
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_iraq_basket (edict_t *ent)
{
	VectorSet (ent->mins, -6, -7, -18);
	VectorSet (ent->maxs,  7,  6,  15);

	SimpleModelInit2(ent,&iraqModelData[OBJ_BASKET],NULL,NULL);
}

/*QUAKED misc_iraq_cart_camel (1 .5 0) (-20 -48 -24) (20 48 24)   INVULNERABLE  NOPUSH x x x x FLUFF
A large two wheeled wooden cart
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_iraq_cart_camel (edict_t *ent)
{
	VectorSet (ent->mins, -20, -48, -24);
	VectorSet (ent->maxs,  20,  48,  24);

	SimpleModelInit2(ent,&iraqModelData[OBJ_CART_CAMEL],NULL,NULL);
}



void flatbed_stop (edict_t *self)
{
	GrabStuffOnTop (self);
	VectorClear(self->velocity);
	self->movetype = MOVETYPE_NONE;
	self->s.sound = 0;
	self->nextthink = -1;
}

void flatbed_deccel (edict_t *self)
{

	float	distanceLeft;
	float	thisDist;
	float	curSpeed;
	vec3_t	newVel;
	float	xDiff, yDiff;

	GrabStuffOnTop (self);
	xDiff = self->s.origin[0] - self->volume;
	yDiff = self->s.origin[1] - self->attenuation;
	distanceLeft = sqrt(xDiff*xDiff + yDiff*yDiff);
	
	curSpeed = sqrt(self->velocity[0]*self->velocity[0] + self->velocity[1]*self->velocity[1]);
	thisDist = curSpeed * .1;

	if (curSpeed < 5)
	{
			newVel[0] = self->velocity[0] * (distanceLeft/thisDist);	
			newVel[1] = self->velocity[1] * (distanceLeft/thisDist);	
			newVel[2] = 0;
			VectorCopy(newVel, self->velocity);
			self->think = flatbed_stop;
			self->nextthink = level.time + .1;
			return;
	}
			
	newVel[0] = self->velocity[0] * (distanceLeft/((FLATBED_BRAKE_FACTOR+1)*thisDist));	
	newVel[1] = self->velocity[1] * (distanceLeft/((FLATBED_BRAKE_FACTOR+1)*thisDist));	
	newVel[2] = 0;
	VectorCopy(newVel, self->velocity);
	self->nextthink = level.time + .1;
}

qboolean FlatbedEndofLineCheck (edict_t *self)
{
	float	distanceLeft;
	float	thisDist;
	float	curSpeed;
	float	xDiff, yDiff;

	xDiff = self->s.origin[0] - self->volume;
	yDiff = self->s.origin[1] - self->attenuation;
	distanceLeft = sqrt(xDiff*xDiff + yDiff*yDiff);
	
	curSpeed = sqrt(self->velocity[0]*self->velocity[0] + self->velocity[1]*self->velocity[1]);
	thisDist = curSpeed * .1;
	
	if (VectorCompare(vec3_origin, self->velocity))
	{
		return true;
	}
	
//	if (self->spawnflags & TRAIN_DECCELERATE)
	{
		if (distanceLeft < (FLATBED_BRAKE_FACTOR*thisDist))
		{
			vec3_t			newVel;
		
			//adjust velocity then stop
			newVel[0] = self->velocity[0] * (distanceLeft/((FLATBED_BRAKE_FACTOR+1)*thisDist));	
			newVel[1] = self->velocity[1] * (distanceLeft/((FLATBED_BRAKE_FACTOR+1)*thisDist));	
			newVel[2] = 0;
			VectorCopy(newVel, self->velocity);
			self->think = flatbed_deccel;
			self->nextthink = level.time + .1;
			return false;
		}
		else
		{
			return true;
		}
	}

}

void flatbed_WheelSpin (edict_t *self)
{
	
	ggBinstC		*wheel1,*wheel2,*wheel3,*wheel4;
	vec3_t			angVel = {0,0,0};
	eft_rotate_t	*rotInfo = NULL;

	int tire_circ;
	float speed,turns,radians;


	tire_circ = (30 * M_PI);	
	speed = self->speed;
	turns = speed / tire_circ;
	radians = DEGTORAD * turns;
	radians = M_PI * turns;

	wheel1 = SimpleModelFindBolt(self, iraqModelData[OBJ_FLATBED_WHEEL],"DUMMY01");
	wheel2 = SimpleModelFindBolt(self, iraqModelData[OBJ_FLATBED_WHEEL],"DUMMY02");
	wheel3 = SimpleModelFindBolt(self, iraqModelData[OBJ_FLATBED_WHEEL],"DUMMY03");
	wheel4 = SimpleModelFindBolt(self, iraqModelData[OBJ_FLATBED_WHEEL],"DUMMY04");

	if (wheel1)
	{
		if (rotInfo = FXA_GetRotate(self))
		{
			// adding to an existing rotation
			rotInfo->xRot = radians*512;//512 accounts for the shift induced by FXA_SetRotate()
		}
		else
		{
			// need to create a rotating effect
			angVel[PITCH] = radians;
			FXA_SetRotate(self, angVel, 
				wheel1->GetInstPtr(),wheel2->GetInstPtr(),
				wheel3->GetInstPtr(),wheel4->GetInstPtr());
		}
	}	
}

void flatbed_linecheck (edict_t *self)
{

	GrabStuffOnTop (self);
	FlatbedEndofLineCheck(self);
	self->nextthink = level.time + .1;
	flatbed_WheelSpin (self);
}

void flatbed_accel (edict_t *self)
{
	int				chance;
	
	GrabStuffOnTop (self);
	if ((double)(self->velocity[0]*self->velocity[0] + self->velocity[1]*self->velocity[1]) < (self->speed*self->speed)) //speed is max speed
	{
		self->velocity[1] += (-1 * self->accel * sin(self->s.angles[1] * DEGTORAD));
		self->velocity[0] += (self->accel * cos(self->s.angles[1] * DEGTORAD));
		self->nextthink = level.time + .1;
	}
	else
	{
		self->think = flatbed_linecheck;
		self->nextthink = level.time + .1;
	}

	chance = gi.irand(0,5);
	FlatbedEndofLineCheck(self);
	flatbed_WheelSpin (self);
}

void flatbed_go (edict_t *self, edict_t *other, edict_t *activator)
{
	// flatbeds with facing 0 are "pointing" east (pos x)
	self->movetype = MOVETYPE_DAN;
	GrabStuffOnTop (self);
	self->velocity[1] = -1 * self->accel * sin(self->s.angles[1] * DEGTORAD);
	self->velocity[0] = self->accel * cos(self->s.angles[1] * DEGTORAD);
	self->friction = 0;
	self->gravity = 0;
	self->nextthink = level.time + .1;
	self->think = flatbed_accel;
	self->use = NULL;
	self->plUse = NULL;

//	self->s.sound = gi.soundindex("Ambient/Locs/Subway/Train/Move.wav");	
	
	FlatbedEndofLineCheck(self);
}

void flatbedwheels_stop (edict_t *self)
{
	FXA_RemoveRotate(self);
}

void flatbedwheels_roll (edict_t *self)
{
	ggBinstC		*wheel1,*wheel2,*wheel3,*wheel4;
	vec3_t			angVel = {0,0,0};
	eft_rotate_t	*rotInfo = NULL;

	int tire_circ;
	float speed,turns,radians;


	tire_circ = (30 * M_PI);	
	speed = self->speed;
	turns = speed / tire_circ;
	radians = DEGTORAD * turns;
	radians = M_PI * turns;

	wheel1 = SimpleModelFindBolt(self, iraqModelData[OBJ_FLATBED_WHEEL],"DUMMY01");
	wheel2 = SimpleModelFindBolt(self, iraqModelData[OBJ_FLATBED_WHEEL],"DUMMY02");
	wheel3 = SimpleModelFindBolt(self, iraqModelData[OBJ_FLATBED_WHEEL],"DUMMY03");
	wheel4 = SimpleModelFindBolt(self, iraqModelData[OBJ_FLATBED_WHEEL],"DUMMY04");

	if (wheel1)
	{
		if (rotInfo = FXA_GetRotate(self))
		{
			// adding to an existing rotation
			rotInfo->xRot = radians*512;//512 accounts for the shift induced by FXA_SetRotate()
		}
		else
		{
			// need to create a rotating effect
			angVel[PITCH] = radians;
			FXA_SetRotate(self, angVel, 
				wheel1->GetInstPtr(),wheel2->GetInstPtr(),
				wheel3->GetInstPtr(),wheel4->GetInstPtr());
		}
	}	

//	self->think = flatbedwheels_roll;
//	self->nextthink = level.time + .2;
}

void flatbedwheels_use (edict_t *self, edict_t *other, edict_t *activator)
{

	if (!self->count)
	{
		self->count = 1;	// Let 'em know we're moving	
		flatbedwheels_roll(self);
	}
	else
	{
		self->count = 0;	// Let 'em know we're not moving
		flatbedwheels_stop(self);
	}

}

void flatbed_remove(edict_t *self)
{
	IGhoulObj				*obj = NULL;
	GhoulID					idEngine = 0;
	float					xattenuation = ATTN_NORM, xvolume = 1.0;

	if (self->ghoulInst && (obj = self->ghoulInst->GetGhoulObject()))
	{
		if (idEngine = obj->FindPart("ENGINE"))
		{
			fxRunner.stopContinualEffect("environ/normalsmoke", self, idEngine);
		}
	}
}

void flatbed_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	IGhoulObj				*obj = NULL;
	GhoulID					idEngine = 0;
	float					xattenuation = ATTN_NORM, xvolume = 1.0;

	if (self->ghoulInst && (obj = self->ghoulInst->GetGhoulObject()))
	{
		if (idEngine = obj->FindPart("ENGINE"))
		{
			fxRunner.execContinualEffect("environ/normalsmoke", self, idEngine);
			gi.positioned_sound (self->s.origin, g_edicts, 0, gi.soundindex("impact/explosion/med.wav"), xvolume, xattenuation, 0);
			self->think = flatbed_remove;
			self->nextthink = level.time + 5;
			self->die = NULL;
			if (self->spawnflags & FLATBED_LIGHTS)
			{	// turn off headlights
				SimpleModelTurnOnOff(SimpleModelGetBolt(self,1),false);
				SimpleModelTurnOnOff(SimpleModelGetBolt(self,2), false);
				SimpleModelRemoveObject(self, "HEADLIGHT1");
				SimpleModelRemoveObject(self, "HEADLIGHT2");
			}
		}
	}
}

/*QUAKED misc_iraq_flatbed (1 .5 0) (-95 -40 -42) (95 40 45)   INVULNERABLE  NOPUSH  TRIGGERED LIGHTS WHEELS_ONLY x FLUFF
A flatbed truck that can have a tank on the back
------ KEYS ------
Style : 
0 - no tank - just a flatbed (default)
1 - has a tank on the back
Distance - distance the truck will move when triggered
Speed - max speed (default 200)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
TRIGGERED - trigger will cause the truck to move forward a specified amount
LIGHTS - the headlights are on
WHEELS_ONLY - wheels turn when used, stop turning when used again.
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- moves forward or backward
- wheels turn as it moves
*/

void SP_misc_iraq_flatbed (edict_t *ent)
{
	VectorSet (ent->mins, -95, -40, -42);
	VectorSet (ent->maxs,  95,  40,  45);

	gi.effectindex("environ/normalsmoke");
	gi.soundindex("impact/explosion/med.wav");

	SimpleModelInit2(ent,&iraqModelData[OBJ_FLATBED_ON],NULL,NULL);

	if (ent->style==0)	// No tank
	{
		SimpleModelRemoveObject(ent,"OILTANK");
	}

	if (ent->spawnflags & FLATBED_LIGHTS)
	{
		SimpleModelAddBolt(ent,*ent->objSpawnData,"headlight1",
							iraqModelData[OBJ_LIGHT_BEAM],"to_headlight",NULL);

		SimpleModelAddBolt(ent,*ent->objSpawnData,"headlight2",
							iraqModelData[OBJ_LIGHT_BEAM],"to_headlight",NULL);

		SimpleModelAddObject(ent, "top1");
		SimpleModelAddObject(ent, "top2");
		SimpleModelAddObject(ent, "back1");
		SimpleModelAddObject(ent, "back2");
		SimpleModelAddObject(ent, "frt1");
		SimpleModelAddObject(ent, "frt2");
		SimpleModelAddObject(ent, "frt3");
		SimpleModelAddObject(ent, "frt4");
	}
	else
	{
		SimpleModelRemoveObject(ent, "top1");
		SimpleModelRemoveObject(ent, "top2");
		SimpleModelRemoveObject(ent, "back1");
		SimpleModelRemoveObject(ent, "back2");
		SimpleModelRemoveObject(ent, "frt1");
		SimpleModelRemoveObject(ent, "frt2");
		SimpleModelRemoveObject(ent, "frt3");
		SimpleModelRemoveObject(ent, "frt4");
	}

	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY01",
						iraqModelData[OBJ_FLATBED_WHEEL],"DUMMY05",NULL);
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY02",
						iraqModelData[OBJ_FLATBED_WHEEL],"DUMMY05",NULL);
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY03",
						iraqModelData[OBJ_FLATBED_WHEEL],"DUMMY05",NULL);
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY04",
						iraqModelData[OBJ_FLATBED_WHEEL],"DUMMY05",NULL);

	// Turn wheels without moving it?
	if (ent->spawnflags & FLATBED_WHEELS_ONLY)
	{
		ent->count = 0;	// Let 'em know we're not moving
		ent->use = flatbedwheels_use;

		if (!ent->speed)
		{
			ent->speed = 200;
		}
	}
	// Turn wheels and apply velocity
	else if (ent->spawnflags & FLATBED_TRIGGERED)
	{
		ent->use = flatbed_go;
		ent->s.origin[2] += 1; // hack to avoid the sliding into floor brushes problem
//		ent->touch = flatbed_smush;  mcnutt said he won't need this
		ent->elasticity = -1;
		ent->flags |= FL_NO_KNOCKBACK;

		if (!ent->speed)
		{
			ent->speed = 200;
		}

		ent->accel = 30;

		// using volume and attenuation for storage of finishing position
		ent->volume = ent->s.origin[0] + st.distance * cos(ent->s.angles[1] * DEGTORAD);
		ent->attenuation = ent->s.origin[1] + st.distance * sin(ent->s.angles[1] * DEGTORAD);
	}

	ent->die = flatbed_die;
}


/*QUAK-ED misc_iraq_fruit_melon (1 .5 0) (-8 -4 -4) (8 4 4)   INVULNERABLE  NOPUSH  x x x x FLUFF
Nice melons
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_iraq_fruit_melon (edict_t *ent)
{
	VectorSet (ent->mins, -8, -4, -4);
	VectorSet (ent->maxs,  8,  4,  4);

	SimpleModelInit2(ent,&iraqModelData[OBJ_FRUIT_MELON],NULL,NULL);
}

/*QUAK-ED misc_iraq_fruit_orange (1 .5 0) (-2 -2 -2) (2 2 2)   INVULNERABLE  NOPUSH  x x x x FLUFF
Nice oranges
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_iraq_fruit_orange (edict_t *ent)
{
	VectorSet (ent->mins, -2, -2, -2);
	VectorSet (ent->maxs,  2,  2,  2);

	SimpleModelInit2(ent,&iraqModelData[OBJ_FRUIT_ORANGE],NULL,NULL);
}

void SimpleModelSetOriginRelative(edict_t *original,edict_t *dupe,float addforward,float addright,float addup);


void iraq_fruit_stand_throw(edict_t *self)
{
	self->velocity[0] = gi.flrand(-100,100);
	self->velocity[1] = gi.flrand(-100,100);
	self->velocity[2] = gi.flrand(200,500);

	self->avelocity[2]= gi.flrand(-100,100);
	self->die = BecomeDebris;
}

// Have fruit thrown before stand dies.
void iraq_fruit_stand_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{

	edict_t *enemy[10],*holdent;
	int i,total;

	if (self->goalentity == NULL)	// No fruit on table, kill it.
	{
		BecomeDebris(self,inflictor,attacker,damage,point);
		return;
	}

	if ((self->spawnflags & FRUITSTAND_MELONS) && (self->spawnflags & FRUITSTAND_ORANGES))
		total = 10;
	else
		total = 5;

	holdent = self->goalentity;

	for (i=0;i<total;++i)
	{
		enemy[i] = holdent;
		holdent = holdent->goalentity;
		enemy[i]->teammaster = NULL;	// So they know they've been thrown
	}

	if (total == 10)
	{
		iraq_fruit_stand_throw(enemy[9]);
		iraq_fruit_stand_throw(enemy[8]);
		iraq_fruit_stand_throw(enemy[7]);
		iraq_fruit_stand_throw(enemy[6]);
		iraq_fruit_stand_throw(enemy[5]);
	}

	iraq_fruit_stand_throw(enemy[4]);
	iraq_fruit_stand_throw(enemy[3]);

	enemy[2]->think = iraq_fruit_stand_throw;
	enemy[2]->nextthink = level.time + FRAMETIME;

	enemy[1]->think = iraq_fruit_stand_throw;
	enemy[1]->nextthink = level.time + FRAMETIME;

	enemy[0]->think = iraq_fruit_stand_throw;
	enemy[0]->nextthink = level.time + FRAMETIME;

	BecomeDebris(self,inflictor,attacker,damage,point);
	
}

// One fruit dies, all fruit dies.
void iraq_fruit_stand_fruit_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	edict_t *table,*holdent,*holdent2;
	int i,total;

	if (!self->teammaster)	// table's dead. Leave other fruit alone.
		return;


	table = self->teammaster;
	holdent = table->goalentity;

	table->goalentity = NULL;

	if ((table->spawnflags & FRUITSTAND_MELONS) && (table->spawnflags & FRUITSTAND_ORANGES))
		total = 10;
	else
		total = 5;

	// Kill all fruit from table
	for (i=0;i<total;++i)
	{
		holdent2 = holdent->goalentity;
		BecomeDebris(holdent,inflictor,attacker,999,point);
		holdent = holdent2;
	}
}

/*QUAKED misc_iraq_fruit_stand (1 .5 0) (-8 -16 -16) (8 16 16)   INVULNERABLE  NOPUSH  MELONS ORANGES NOFRUIT x FLUFF
Nice fruit stand. If none of the spawnflags are used, the table will have both types of fruit.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
MELONS - only melons on table
ORANGES - only oranges
NOFRUIT - no fruit on table
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- fruti is thrown when table is shot
*/
void SP_misc_iraq_fruit_stand (edict_t *ent)
{
	edict_t *melon,*melon2,*melon3,*melon4,*melon5;
	edict_t *orange,*orange2,*orange3,*orange4,*orange5;

	VectorSet (ent->mins,  -8, -16, -16);
	VectorSet (ent->maxs,   8,  16,  16);

	SimpleModelInit2(ent,&iraqModelData[OBJ_FRUIT_STAND],NULL,NULL);

	if (ent->spawnflags & FRUITSTAND_NOFRUIT)	// No fruit??
		return;

	// Didn't choose either so they must want both.
	if (!(ent->spawnflags & FRUITSTAND_MELONS) && !(ent->spawnflags & FRUITSTAND_ORANGES))
	{
		ent->spawnflags |= FRUITSTAND_MELONS | FRUITSTAND_ORANGES;
	}

	// Melons on table??
	if (ent->spawnflags & FRUITSTAND_MELONS)
	{
		// Melon1
		melon = G_Spawn();
		melon->s.angles[1] = ent->s.angle_diff; 
		SP_misc_iraq_fruit_melon(melon);
		melon->die = iraq_fruit_stand_fruit_die;
		SimpleModelSetOriginRelative(ent,melon,0,0,20);

		// Melon2
		melon2 = G_Spawn();
		melon2->s.angles[1] = ent->s.angle_diff; 
		SP_misc_iraq_fruit_melon(melon2);
		melon2->die = iraq_fruit_stand_fruit_die;
		SimpleModelSetOriginRelative(ent,melon2,0,-8,20);

		// Melon3
		melon3 = G_Spawn();
		melon3->s.angles[1] = ent->s.angle_diff; 
		SP_misc_iraq_fruit_melon(melon3);
		melon3->die = iraq_fruit_stand_fruit_die;
		SimpleModelSetOriginRelative(ent,melon3,0,-16,20);

		// Melon4
		melon4 = G_Spawn();
		melon4->s.angles[1] = ent->s.angle_diff; 
		SP_misc_iraq_fruit_melon(melon4);
		melon4->die = iraq_fruit_stand_fruit_die;
		SimpleModelSetOriginRelative(ent,melon4,0,-4,27);

		// Melon5
		melon5 = G_Spawn();
		melon5->s.angles[1] = ent->s.angle_diff; 
		SP_misc_iraq_fruit_melon(melon5);
		melon5->die = iraq_fruit_stand_fruit_die;

		// If there are no oranges, but a melon where the orangs go
		if (ent->spawnflags & FRUITSTAND_ORANGES)	// There are oranges
			SimpleModelSetOriginRelative(ent,melon5,0,-12,27);
		else
		{
			SimpleModelSetOriginRelative(ent,melon5,0,12,20);
			melon5->s.angle_diff += 25;
		}
	}

	// Oranges on table???
	if (ent->spawnflags & FRUITSTAND_ORANGES)	
	{
		orange = G_Spawn();
		orange->s.angles[1] = ent->s.angle_diff; 
		SP_misc_iraq_fruit_orange(orange);
		orange->die = iraq_fruit_stand_fruit_die;
		SimpleModelSetOriginRelative(ent,orange,2,8,18);

		// Orange2
		orange2 = G_Spawn();
		orange2->s.angles[1] = ent->s.angle_diff; 
		SP_misc_iraq_fruit_orange(orange2);
		orange2->die = iraq_fruit_stand_fruit_die;
		SimpleModelSetOriginRelative(ent,orange2,-2,8,18);

		// Orange3
		orange3 = G_Spawn();
		orange3->s.angles[1] = ent->s.angle_diff; 
		SP_misc_iraq_fruit_orange(orange3);
		orange3->die = iraq_fruit_stand_fruit_die;
		SimpleModelSetOriginRelative(ent,orange3,0,12,18);

		// Orange4
		orange4 = G_Spawn();
		orange4->s.angles[1] = ent->s.angle_diff; 
		SP_misc_iraq_fruit_orange(orange4);
		orange4->die = iraq_fruit_stand_fruit_die;
		SimpleModelSetOriginRelative(ent,orange4,4,12,18);

		// Orange5
		orange5 = G_Spawn();
		orange5->s.angles[1] = ent->s.angle_diff; 
		SP_misc_iraq_fruit_orange(orange5);
		orange5->die = iraq_fruit_stand_fruit_die;
		SimpleModelSetOriginRelative(ent,orange5,-8,18,18);

	}

	ent->die = iraq_fruit_stand_die;

	// Setup fruit and table to point to each other in case of death or dismemberment.
	if ((ent->spawnflags & FRUITSTAND_MELONS) && (ent->spawnflags & FRUITSTAND_ORANGES))
	{
		ent->goalentity = melon;
		melon->goalentity = melon2;
		melon2->goalentity = melon3;
		melon3->goalentity = melon4;
		melon4->goalentity = melon5;

		// Point fruit back to table so if a single fruit dies, all fruit dies.
		melon->teammaster = ent;
		melon2->teammaster = ent;
		melon3->teammaster = ent;
		melon4->teammaster = ent;
		melon5->teammaster = ent;

		melon5->goalentity = orange;
		orange->goalentity = orange2;
		orange2->goalentity = orange3;
		orange3->goalentity = orange4;
		orange4->goalentity = orange5;

		// Point fruit back to table so if a single fruit dies, all fruit dies.
		orange->teammaster = ent;
		orange2->teammaster = ent;
		orange3->teammaster = ent;
		orange4->teammaster = ent;
		orange5->teammaster = ent;
	}
	else if (ent->spawnflags & FRUITSTAND_MELONS)	// Just melons
	{
		ent->goalentity = melon;
		melon->goalentity = melon2;
		melon2->goalentity = melon3;
		melon3->goalentity = melon4;
		melon4->goalentity = melon5;

		// Point fruit back to table so if one fruit dies, all fruit dies.
		melon->teammaster = ent;
		melon2->teammaster = ent;
		melon3->teammaster = ent;
		melon4->teammaster = ent;
		melon5->teammaster = ent;
	}
	else if (ent->spawnflags & FRUITSTAND_ORANGES)	// Just oranges
	{
		ent->goalentity = orange;
		orange->goalentity = orange2;
		orange2->goalentity = orange3;
		orange3->goalentity = orange4;
		orange4->goalentity = orange5;

		// Point fruit back to table so if a single fruit dies, all fruit dies.
		orange->teammaster = ent;
		orange2->teammaster = ent;
		orange3->teammaster = ent;
		orange4->teammaster = ent;
		orange5->teammaster = ent;
	}
}


/*QUAKED misc_iraq_partition (1 .5 0) (-63 -1  0) (62 2 35) INVULNERABLE  NOPUSH x x x x FLUFF
Chrome pole with a felt line.  Used by movie theaters use to keep people in line.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_iraq_partition (edict_t *ent)
{
	VectorSet (ent->mins, -63, -1,  0);
	VectorSet (ent->maxs,  62,  2, 35);

	SimpleModelInit2(ent,&iraqModelData[OBJ_PARTITION],NULL,NULL);
}

/*QUAKED misc_iraq_pottery (1 .5 0) (-8 -8 -9) (8 8 9) INVULNERABLE  NOPUSH x x x x FLUFF
A knee high pot for storing dates and figs and such.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_iraq_pottery (edict_t *ent)
{
	VectorSet (ent->mins, -8, -8, -9);
	VectorSet (ent->maxs,  8,  8,  9);

	SimpleModelInit2(ent,&iraqModelData[OBJ_POTTERY],NULL,NULL);
}

/*QUAKED misc_iraq_sprinkler (1 .5 0) (-7 -63 -2) (6 64 2) INVULNERABLE  NOPUSH x x x x FLUFF
A six headed cattle sprinkler.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_iraq_sprinkler (edict_t *ent)
{
	VectorSet (ent->mins, -7, -63, -2);
	VectorSet (ent->maxs,  6,  64,  2);

	ent->spawnflags |= SF_NOPUSH;	// Can't ever be pushed

	SimpleModelInit2(ent,&iraqModelData[OBJ_SPRINKLER],NULL,NULL);

}


// kef 10/21/99 -- jersey needs the bbox smaller so Hawk can stand near it
//
// was (-285 -177 -54) (285 177 53)
//

/*QUAKED misc_iraq_stealth (1 .5 0) (-32 -32 -54) (32 32 53)   INVULNERABLE  NOPUSH  x x x x FLUFF
A big stealth plane
------ SPAWNFLAGS ------
INVULNERABLE - N/A can't be damaged.
NOPUSH - N/A can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_iraq_stealth (edict_t *ent)
{

	VectorSet (ent->mins, -32, -32, -54);
	VectorSet (ent->maxs,  32,  32,  53);
	ent->spawnflags |= SF_INVULNERABLE;

	ent->s.angles[YAW] = 270;

	SimpleModelInit2(ent,&iraqModelData[OBJ_STEALTH],NULL,NULL);

}

/*QUAKED misc_iraq_tree_palm (1 .5 0) (-30 -9 -67) (-6 23 67)   INVULNERABLE  NOPUSH  x x x x FLUFF
A tall palm tree
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A, can't ever be pushed
FLUFF - won't show if gl_pictip is set

  hey, designers! you need to put a clip brush around this joker cuz it defies physics. tree is non-solid, too.
*/
void SP_misc_iraq_tree_palm (edict_t *ent)
{
//	VectorSet (ent->mins, -65, -47, -67);
//	VectorSet (ent->maxs,  18,  69,  67);
	VectorSet (ent->mins, -30, -9, -67);
	VectorSet (ent->maxs,  -6,  23,  67);

	ent->spawnflags |= SF_NOPUSH;	// Can't ever be pushed

	SimpleModelInit2(ent,&iraqModelData[OBJ_TREE_PALM],NULL,NULL);

	ent->solid = SOLID_NOT;
//	ent->think = drawthink;
//	ent->nextthink = level.time + .1;
}

/*QUAKED misc_generic_sprinkler (1 .5 0) (-4 -4 -4) (4 4 4)   INVULNERABLE  NOPUSH  x x x x FLUFF
A ceiling water sprinkler 
  DO NOT USE YET!
*/
void SP_misc_generic_sprinkler (edict_t *ent)
{
	VectorSet (ent->mins, -4, -4, -4);
	VectorSet (ent->maxs, 4, 4, 4);

	ent->surfaceType = SURF_STONE_WHITE;
	ent->health = 10;
	ent->spawnflags |= SF_NOPUSH;

	game_ghoul.SetSimpleGhoulModel (ent,"objects/generic/urn","urn");
	SimpleModelInit(ent,SOLID_BBOX);

	fxRunner.execContinualEffect("environ/sprinkler", ent);
}

valveCallback theValveCallback;

bool valveCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	edict_t* self = (edict_t*)ent;

	self->ghoulInst->Pause(level.time);

	self->s.sound = 0;

	return true;
}

// sadly, I just copied these from g_func.cpp rather than take the 20 or 30 hours 
//to figure out how to #include them in a header somewhere
#define	STATE_TOP			0
#define	STATE_BOTTOM		1
#define STATE_UP			2
#define STATE_DOWN			3

void valve_use_portcullis (edict_t *ent, edict_t *other, edict_t *activator)
{
	// based on the moveinfo.state of 'other', either turn the valve clockwise, ccw, or stop its turning
	switch(other->moveinfo.state)
	{
	case STATE_TOP:
	case STATE_BOTTOM:	// stop
		ent->avelocity[0] = 0.0;
		break;
	case STATE_UP:		// cw
		ent->avelocity[0] = 40.0;
		break;
	case STATE_DOWN:	// ccw
		ent->avelocity[0] = -40.0;
		break;
	default:	// stop
		ent->avelocity[0] = 0.0;
		break;
	}
	gi.linkentity(ent);
}

void iraq_valve_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	ggObjC *simpObj;
	GhoulID simpSeq;
	ggOinstC* myInstance;
	GhoulID	tempNote=0;

	if (!ent->ghoulInst)
	{
		return;
	}

	if (ent->spawnflags & VALVE_PORTCULLIS)
	{
		valve_use_portcullis(ent, other, activator);
		return;
	}
	else
	{
		gi.sound(ent, CHAN_VOICE, gi.soundindex("Ambient/Models/Valve/ValveTurn.wav"), .6, ATTN_NORM, 0);
	}

	simpObj = game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());
	if (!simpObj)
	{
		return;
	}

	//changed this to false, can't cache new seqs in after instances are created --ss
	simpSeq = simpObj->FindSequence(iraqModelData[OBJ_VALVE].file);

	if (!simpSeq)
	{
		return;
	}

	myInstance = simpObj->FindOInst(ent->ghoulInst);
	if (!myInstance)
	{
		return;
	}

	// Play turning sequence
	myInstance->PlaySequence(simpSeq, level.time);

	// Setup stop frame when done playing sequence
	tempNote = simpObj->GetMyObject()->FindNoteToken("EOS");
	if (tempNote)
	{
		ent->ghoulInst->AddNoteCallBack(&theValveCallback,tempNote);
	}

}

/*QUAKED misc_iraq_valve (1 .5 0) (-12 -3 -10) (12 1 11)   INVULNERABLE  NOPUSH  x x x x FLUFF PORTCULLIS
A circular valve that will spin when used.
------ SPAWNFLAGS ------
INVULNERABLE - N/A, can't ever be damaged.
NOPUSH - N/A, can't ever be pushed
FLUFF - won't show if gl_pictip is set
PORTCULLIS - used by a func_door_portcullis
*/
void SP_misc_iraq_valve (edict_t *ent)
{
	VectorSet (ent->mins, -12, -3, -10);
	VectorSet (ent->maxs,  12,  1,  11);

	ent->spawnflags |= SF_INVULNERABLE;
	ent->spawnflags |= SF_NOPUSH;	// Can't ever be pushed

	SimpleModelInit2(ent,&iraqModelData[OBJ_VALVE],NULL,NULL);

	ent->ghoulInst->Pause(level.time);
	ent->movetype = MOVETYPE_DAN;
	ent->gravity = 0.0;

	ent->use = iraq_valve_use;
	gi.soundindex("Ambient/Models/Valve/ValveTurn.wav");
}

