/******************************************************
 * Objects for Castle levels						  *
 ******************************************************/

#include "g_local.h"
#include "g_obj.h"

#define HLTH_CASTLE_ARMOR				500
#define HLTH_CASTLE_BUST				1000
#define HLTH_CASTLE_CANNON				1000
#define HLTH_CASTLE_CHAIR				500
#define HLTH_CASTLE_CHAIR_BIG			500
#define HLTH_CASTLE_GLOBE				1000
#define HLTH_CASTLE_HEDGE				500
#define HLTH_CASTLE_TREE				1000


//  misc_castle..
void SP_misc_castle_armor (edict_t *ent);
void SP_misc_castle_bust (edict_t *ent);
void SP_misc_castle_cannon (edict_t *ent);
void SP_misc_castle_chair (edict_t *ent);
void SP_misc_castle_chair_big (edict_t *ent);
void SP_misc_castle_globe (edict_t *ent);
void SP_misc_castle_hedge (edict_t *ent);
void SP_misc_castle_tree (edict_t *ent);

spawn_t castleSpawns[] =
{
	//	misc_castle...
	{"misc_castle_armor",			SP_misc_castle_armor},
	{"misc_castle_bust",			SP_misc_castle_bust},
	{"misc_castle_cannon",			SP_misc_castle_cannon},
	{"misc_castle_chair",			SP_misc_castle_chair},
	{"misc_castle_chair_big",		SP_misc_castle_chair_big},
	{"misc_castle_globe",			SP_misc_castle_globe},
	{"misc_castle_hedge",			SP_misc_castle_hedge},
	{"misc_castle_tree",			SP_misc_castle_tree},

	{NULL,								NULL},
};

typedef enum
{
	OBJ_ARMORSUIT = 0,
	OBJ_BUST,
	OBJ_CANNON,
	OBJ_CHAIR,
	OBJ_CHAIR_LEG_FRONT,
	OBJ_CHAIR_LEG_BACK,
	OBJ_CHAIR_SEATBACK,
	OBJ_CHAIR_BIG,
	OBJ_CHAIR_BIG_LEG,
	OBJ_CHAIR_BIG_ARM,
	OBJ_CHAIR_BIG_SEATBACK,
	OBJ_GLOBE,
	OBJ_GLOBE_BASE,
	OBJ_HEDGE,
	OBJ_TREE,
	MAX_OBJS
};

modelSpawnData_t castleModelData[MD_CASTLE_SIZE] =
{	
//      dir							file				surfaceType			material			health						solid			material file  cnt  scale
"objects/castle/armor",			"armor",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_CASTLE_ARMOR,			SOLID_BBOX,		NULL,			0,	DEBRIS_SM,	NULL,	// OBJ_ARMORSUIT
"objects/castle/bust",			"bust",					SURF_STONE_DGREY,	MAT_ROCK_DGREY,		HLTH_CASTLE_BUST,			SOLID_BBOX,		"bust",			0,	DEBRIS_SM,	NULL,	// OBJ_BUST
"objects/castle/cannon",		"cannon",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_CASTLE_CANNON,			SOLID_BBOX,		NULL,			0,	DEBRIS_SM,	NULL,	// OBJ_CANNON
"objects/castle/chair",			"seat01",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_CASTLE_CHAIR,			SOLID_BBOX,		NULL,			0,	DEBRIS_SM,	NULL,	// OBJ_CHAIR
"objects/castle/chair",			"leg01",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_CASTLE_CHAIR,			SOLID_BBOX,		NULL,			0,	DEBRIS_SM,	NULL,	// OBJ_CHAIR_LEG_FRONT
"objects/castle/chair",			"leg02",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_CASTLE_CHAIR,			SOLID_BBOX,		NULL,			0,	DEBRIS_SM,	NULL,	// OBJ_CHAIR_LEG_BACK
"objects/castle/chair",			"back",					SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_CASTLE_CHAIR,			SOLID_BBOX,		NULL,			0,	DEBRIS_SM,	NULL,	// OBJ_CHAIR_SEATBACK
"objects/castle/chair_big",		"seat01",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_CASTLE_CHAIR_BIG,		SOLID_BBOX,		NULL,			0,	DEBRIS_SM,	NULL,	// OBJ_CHAIR_BIG
"objects/castle/chair_big",		"leg01",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_CASTLE_CHAIR_BIG,		SOLID_BBOX,		NULL,			0,	DEBRIS_SM,	NULL,	// OBJ_CHAIR_BIG_LEG
"objects/castle/chair_big",		"arm01",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_CASTLE_CHAIR_BIG,		SOLID_BBOX,		NULL,			0,	DEBRIS_SM,	NULL,	// OBJ_CHAIR_BIG_ARM
"objects/castle/chair_big",		"back01",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_CASTLE_CHAIR_BIG,		SOLID_BBOX,		NULL,			0,	DEBRIS_SM,	NULL,	// OBJ_CHAIR_BIG_SEATBACK
"objects/castle/globe",			"globe",				SURF_DEFAULT,		MAT_WALL_GREY,		HLTH_CASTLE_GLOBE,			SOLID_BBOX,		NULL,			0,	DEBRIS_SM,	NULL,	// OBJ_GLOBE
"objects/castle/globe",			"base",					SURF_DEFAULT,		MAT_WALL_GREY,		HLTH_CASTLE_GLOBE,			SOLID_BBOX,		NULL,			0,	DEBRIS_SM,	NULL,	// OBJ_GLOBE_BASE
"objects/castle/hedge",			"hedge",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_CASTLE_HEDGE,			SOLID_BBOX,		NULL,			25,	DEBRIS_SM,	NULL,	// OBJ_HEDGE
"objects/castle/tree",			"tree",					SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_CASTLE_TREE,			SOLID_BBOX,		NULL,			35,	DEBRIS_MED,	NULL,	// OBJ_TREE
};


//-------------------------------------------------------------
// For objects that break apart
//-------------------------------------------------------------

void chair2_legs_partpain (edict_t *self,int partLost, vec3_t boltPos);
//void chair_arm_partpain (edict_t *self,int partLost, vec3_t boltPos);
void chair_back_partpain (edict_t *self,int partLost, vec3_t boltPos);

// Castle Parts Data enum
typedef enum
{
	CPD_CHAIR = 0,
	CPD_CHAIR_BIG,
	CPD_MAX,
};

// Castle Parts Data for each object
objBreak_t castleObjBreak [CPD_MAX] = 
{ 
// # of parts   objParts ptr
	5,			NULL,			// CPD_CHAIR
	5,			NULL,			// CPD_CHAIR_BIG
};


// Castle Parts Data for each part of each object 
// WARNING: when updating this be sure to update castleObjBreak if more parts are added
objParts_t castleObjBoltPartsData[10] =
{	
	// CPD_CHAIR							// as you face the chair...
	"DUMMY01", 1,	chair2_legs_partpain,	// Leg1 -- left front
	"DUMMY02", 2,	chair2_legs_partpain,	// Leg2 -- right front
	"DUMMY03", 3,	chair2_legs_partpain,	// Leg3 -- left rear
	"DUMMY04", 4,	chair2_legs_partpain,	// Leg4 -- right rear
	"DUMMY05", 5,	NULL/*chair_back_partpain*/,	// back of the chair

	// CPD_CHAIR_BIG
	"DUMMY01", 1,	chair2_legs_partpain,	// Leg1 -- left
	"DUMMY02", 2,	chair2_legs_partpain,	// Leg2 -- right
	"DUMMY03", 3,	NULL/*chair_arm_partpain*/,		// Arm1 -- left
	"DUMMY04", 4,	NULL/*chair_arm_partpain*/,		// Arm2 -- right
	"DUMMY05", 5,	chair_back_partpain,	// back of the chair
};

/*QUAKED misc_castle_armor (1 .5 0) (-4 -11 -38) (11 17 50)  INVULNERABLE  NOPUSH x x x x FLUFF
A suit of armor
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_castle_armor (edict_t *ent)
{
	VectorSet (ent->mins, -4, -11, -38);
	VectorSet (ent->maxs, 11,  17,  50);

	SimpleModelInit2(ent,&castleModelData[OBJ_ARMORSUIT],NULL,NULL);

}


/*QUAKED misc_castle_bust (1 .5 0) (-4 -6  -1) (4 3 13)  INVULNERABLE  NOPUSH x x x x FLUFF
A bust of a person
------ KEYS ------
skin - 
0 - guy bust
1 - girl bust 
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_castle_bust (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins,-4,-6, -1);
	VectorSet (ent->maxs, 4, 3, 13);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "bust2";
		break;
	default :
		skinname = "bust";
		break;
	}

	SimpleModelInit2(ent,&castleModelData[OBJ_BUST],skinname,NULL);

}


/*QUAKED misc_castle_cannon (1 .5 0) (-27 -11 -13) (55 11 20)  INVULNERABLE  NOPUSH x x x x FLUFF
A cannon on wheels
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_castle_cannon (edict_t *ent)
{
	VectorSet (ent->mins,-27,-11, -13);
	VectorSet (ent->maxs, 55, 11,  20);

	SimpleModelInit2(ent,&castleModelData[OBJ_CANNON],NULL,NULL);

}

void chair2_legs_flat(edict_t *self)
{
	VectorCopy(self->moveinfo.start_angles,self->s.angles);
	VectorSet (self->mins, -10, -9,-2);
	VectorSet (self->maxs,  10,  9, 18);
	gi.linkentity(self);
}

// roll front
void chair2_legs_roll1(edict_t *self)
{
	vec3_t		fwd, right, up, vEnd = {0,0,0};
	float		checkDist = 20.0f;
	trace_t		trace;

	AngleVectors(self->s.angles,fwd,right,up);
	
	VectorMA(self->s.origin, checkDist, fwd, vEnd);
	gi.trace (self->s.origin, NULL, NULL, vEnd, self, self->clipmask, &trace);
	// check to see if we're blocked before we roll
	if (trace.fraction < 1.0f)
	{
		// we're blocked so move back a little
		VectorMA(self->s.origin, -trace.fraction*checkDist, fwd, self->s.origin);
	}
	self->s.angles[PITCH] += 40;
	VectorSet (self->mins, -10, -9,-6);
	VectorSet (self->maxs,  10,  9, 18);
	gi.linkentity(self);
}

// roll right
void chair2_legs_roll2(edict_t *self)
{
	vec3_t		fwd, right, vEnd = {0,0,0};
	float		checkDist = -30.0f;
	trace_t		trace;
	bool		bTwoLegs = false;

	bTwoLegs = (strcmp(self->classname, "misc_castle_chair_big") == 0);
	AngleVectors(self->s.angles,fwd,right,NULL);
	
	VectorMA(self->s.origin, checkDist, right, vEnd);
	gi.trace (self->s.origin, NULL, NULL, vEnd, self, self->clipmask, &trace);
	// check to see if we're blocked before we roll
	if (trace.fraction < 1.0f)
	{
		// we're blocked so move back a little
		VectorMA(self->s.origin, -trace.fraction*checkDist, fwd, self->s.origin);
	}
	self->s.angles[ROLL] -= bTwoLegs?40:40;
	VectorSet (self->mins, -10, -9,bTwoLegs?-8.7:-6);
	VectorSet (self->maxs,  10,  9, 18);
	gi.linkentity(self);
}

// roll left
void chair2_legs_roll3(edict_t *self)
{
	vec3_t		fwd, right, vEnd = {0,0,0};
	float		checkDist = 30.0f;
	trace_t		trace;
	bool		bTwoLegs = false;

	bTwoLegs = (strcmp(self->classname, "misc_castle_chair_big") == 0);
	AngleVectors(self->s.angles,fwd,right,NULL);
	
	VectorMA(self->s.origin, checkDist, right, vEnd);
	gi.trace (self->s.origin, NULL, NULL, vEnd, self, self->clipmask, &trace);
	// check to see if we're blocked before we roll
	if (trace.fraction < 1.0f)
	{
		// we're blocked so move back a little
		VectorMA(self->s.origin, -trace.fraction*checkDist, fwd, self->s.origin);
	}
	self->s.angles[ROLL] += bTwoLegs?40:40;
	VectorSet (self->mins, -10, -9,bTwoLegs?-8.7:-6);
	VectorSet (self->maxs,  10,  9, 18);
	gi.linkentity(self);
}

// roll back
void chair2_legs_roll4(edict_t *self)
{
	vec3_t		fwd, right, vEnd = {0,0,0};
	float		checkDist = -30.0f;
	trace_t		trace;

	AngleVectors(self->s.angles,fwd,right,NULL);
	
	VectorMA(self->s.origin, checkDist, fwd, vEnd);
	gi.trace (self->s.origin, NULL, NULL, vEnd, self, self->clipmask, &trace);
	// check to see if we're blocked before we roll
	if (trace.fraction < 1.0f)
	{
		// we're blocked so move forward a little
		VectorMA(self->s.origin, -trace.fraction*checkDist, fwd, self->s.origin);
	}
	self->s.angles[PITCH] -= 37;
	VectorSet (self->mins, -10, -9,-5.5);
	VectorSet (self->maxs,  10,  9, 18);
	gi.linkentity(self);
}

void chair2_legs_partpain (edict_t *self,int partLost, vec3_t boltPos)
{
	bool		bTwoLegs = false, B1, B2, B3, B4;
	boltInstInfo_c *boltInfo = NULL;

	if (!self->objInfo || !(boltInfo = (boltInstInfo_c*)self->objInfo->GetInfo(OIT_BOLTINST)) )
	{
		return;
	}
	B1 = boltInfo->IsOn(1);
	B2 = boltInfo->IsOn(2);
	B3 = boltInfo->IsOn(3);
	B4 = boltInfo->IsOn(4);

	bTwoLegs = (strcmp(self->classname, "misc_castle_chair_big") == 0);

	if (!B1 && !B2 && !B3 && !B4)
	{	
		chair2_legs_flat(self);
		return;
	}

	// These are given as you face the chair
	// bolt1 - left front leg
	// bolt2 - right front leg
	// bolt3 - right back leg
	// bolt4 - left back leg

	// chair2_legs_roll1 - roll forward
	// chair2_legs_roll2 - roll right
	// chair2_legs_roll3 - roll left
	// chair2_legs_roll4 - roll back

	switch (partLost)
	{
	case 1:	// left front leg
		if (bTwoLegs)
		{
			if (B2)
			{
				// missing the left legs
				chair2_legs_roll3(self);
			}
			else
			{
				// missing all legs
				chair2_legs_flat(self);
			}
		}
		else
		{
			if (!B2 && !B3)
			{
				// missing three legs
				Obj_partkill (self,4,true);	// Remove leg
				chair2_legs_flat(self);
			}
			else if (!B3 && !B4)
			{
				// missing three legs
				Obj_partkill (self,2,true);	// Remove leg
				chair2_legs_flat(self);
			}
			else if (!B4 && !B2)
			{
				// missing three legs
				Obj_partkill (self,3,true);	// Remove leg
				chair2_legs_flat(self);
			}
			else if (!B2)
			{
				// missing two legs
				chair2_legs_roll1(self);
			}
			else if (!B3)
			{
				// missing two legs
				chair2_legs_roll3(self);
			}
			else if (!B4)
			{
				// missing two legs
				Obj_partkill (self,2,true);	// Remove leg
				Obj_partkill (self,3,true);	// Remove leg
				chair2_legs_flat(self);
			}
		}
		break;
	case 2:	// right front leg
		if (bTwoLegs)
		{
			if (B1)
			{
				// missing the right legs
				chair2_legs_roll2(self);
			}
			else
			{
				// missing all legs
				chair2_legs_flat(self);
			}
		}
		else
		{
			if (!B1 && !B3)
			{
				// missing three legs
				Obj_partkill (self,4,true);	// Remove leg
				chair2_legs_flat(self);
			}
			else if (!B3 && !B4)
			{
				// missing three legs
				Obj_partkill (self,1,true);	// Remove leg
				chair2_legs_flat(self);
			}
			else if (!B4 && !B1)
			{
				// missing three legs
				Obj_partkill (self,3,true);	// Remove leg
				chair2_legs_flat(self);
			}
			else if (!B1)
			{
				// missing two legs
				chair2_legs_roll1(self);
			}
			else if (!B4)
			{
				// missing two legs
				chair2_legs_roll2(self);
			}
			else if (!B3)
			{
				// missing two legs
				Obj_partkill (self,1,true);	// Remove leg
				Obj_partkill (self,4,true);	// Remove leg
				chair2_legs_flat(self);
			}
		}
		break;
	case 3:	// right back leg
		if (bTwoLegs)
		{
			// how'd you get here with only two legs?
		}
		else
		{
			if (!B1 && !B2)
			{
				// missing three legs
				Obj_partkill (self,4,true);	// Remove leg
				chair2_legs_flat(self);
			}
			else if (!B2 && !B4)
			{
				// missing three legs
				Obj_partkill (self,1,true);	// Remove leg
				chair2_legs_flat(self);
			}
			else if (!B4 && !B1)
			{
				// missing three legs
				Obj_partkill (self,2,true);	// Remove leg
				chair2_legs_flat(self);
			}
			else if (!B1)
			{
				// missing two legs
				chair2_legs_roll3(self);
			}
			else if (!B4)
			{
				// missing two legs
				chair2_legs_roll4(self);
			}
			else if (!B2)
			{
				// missing two legs
				Obj_partkill (self,1,true);	// Remove leg
				Obj_partkill (self,4,true);	// Remove leg
				chair2_legs_flat(self);
			}
		}
		break;
	case 4:	// left back leg
		if (bTwoLegs)
		{
			// how'd you get here with only two legs?
		}
		else
		{
			if (!B1 && !B2)
			{
				// missing three legs
				Obj_partkill (self,3,true);	// Remove leg
				chair2_legs_flat(self);
			}
			else if (!B2 && !B3)
			{
				// missing three legs
				Obj_partkill (self,1,true);	// Remove leg
				chair2_legs_flat(self);
			}
			else if (!B3 && !B1)
			{
				// missing three legs
				Obj_partkill (self,2,true);	// Remove leg
				chair2_legs_flat(self);
			}
			else if (!B2)
			{
				// missing two legs
				chair2_legs_roll2(self);
			}
			else if (!B3)
			{
				// missing two legs
				chair2_legs_roll4(self);
			}
			else if (!B1)
			{
				// missing two legs
				Obj_partkill (self,2,true);	// Remove leg
				Obj_partkill (self,3,true);	// Remove leg
				chair2_legs_flat(self);
			}
		}
		break;
	default :
		break;
	}
}


/*QUAKED misc_castle_chair (1 .5 0) (-17 -14 -15) (11 13 38)  INVULNERABLE  NOPUSH x x x x FLUFF
A chair with a high back
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- falls apart when shot
*/
void SP_misc_castle_chair (edict_t *ent)
{
	ggBinstC		*bInstC = NULL;
	IGhoulInst		*boltInst = NULL;
	IGhoulInst		*boltInst2 = NULL;

	VectorSet (ent->mins,-17, -14, -15);
	VectorSet (ent->maxs, 11,  13,  38);

	Obj_partbreaksetup (CPD_CHAIR,&castleModelData[OBJ_CHAIR],
		castleObjBoltPartsData, castleObjBreak);

	SimpleModelInit2(ent,&castleModelData[OBJ_CHAIR],NULL,NULL);

	// These are given as you face the chair
	// bolt1 - left front leg
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY01",
						castleModelData[OBJ_CHAIR_LEG_FRONT],"DUMMY01",NULL);
	// bolt2 - right front leg
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY02",
						castleModelData[OBJ_CHAIR_LEG_FRONT],"DUMMY01",NULL);
	// bolt3 - left back leg
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY03",
							castleModelData[OBJ_CHAIR_LEG_BACK],"DUMMY01",NULL);
	// bolt4 - right back leg
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY04",
							castleModelData[OBJ_CHAIR_LEG_BACK],"DUMMY01",NULL);
	// bolt5 - back
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY05",
							castleModelData[OBJ_CHAIR_SEATBACK],"DUMMY05",NULL);
	ent->pain = Obj_partpain;

	// gonna need these if the chair ever has all of its legs shot off
	VectorCopy(ent->s.angles,ent->moveinfo.start_angles);
}

void chair_back_partpain (edict_t *self,int partLost, vec3_t boltPos)
{
	// if the back is shot off of a big chair, blow the arms off, too
	ggOinstC*		myInstance = NULL;
	ggObjC			*MyGhoulObj = NULL;
	vec3_t			armPos;
	bool		bTwoLegs = false, B3, B4;
	boltInstInfo_c *boltInfo = NULL;
	vec3_t debrisNorm;

	if (!self->objInfo || !(boltInfo = (boltInstInfo_c*)self->objInfo->GetInfo(OIT_BOLTINST)) )
	{
		return;
	}
	B3 = boltInfo->IsOn(3);
	B4 = boltInfo->IsOn(4);

	// first, deal with the back of the chair
	GetGhoulPosDir(self->s.origin, self->s.angles, self->ghoulInst,
			   NULL, "DUMMY05", armPos, NULL, NULL, NULL);
	VectorClear(debrisNorm);
	FX_ThrowDebris(armPos,debrisNorm, 5, DEBRIS_SM, self->material, 0,0,0, self->surfaceType);
	FX_SmokePuff(armPos,120,120,120,200);

	// now deal with the arms
	if (!B3 && !B4)
	{
		return;
	}
	if (self->ghoulInst && self->ghoulInst->GetGhoulObject())
	{
		if (MyGhoulObj=game_ghoul.FindObject(self->ghoulInst->GetGhoulObject()))
		{
			if (myInstance = MyGhoulObj->FindOInst(self->ghoulInst))
			{
				// Throw debris and make it go away

				// yes, I'm hardcoding dummy names rather than going through 27 levels of
				//indirection to get to the array that holds the names. I mean, just look at this...
				//
				// boltPartData = self->objSpawnData->objBreakData->boltPartData;
				//
				//ridiculous. absolutely ridiculous. like I'm gonna mess with that.
				GetGhoulPosDir(self->s.origin, self->s.angles, self->ghoulInst,
						   NULL, "DUMMY03", armPos, NULL, NULL, NULL);
				FX_ThrowDebris(armPos,debrisNorm, 5, DEBRIS_SM, self->material, 0,0,0, self->surfaceType);
				FX_SmokePuff(armPos,120,120,120,200);
				GetGhoulPosDir(self->s.origin, self->s.angles, self->ghoulInst,
						   NULL, "DUMMY04", armPos, NULL, NULL, NULL);
				FX_ThrowDebris(armPos,debrisNorm, 5, DEBRIS_SM, self->material, 0,0,0, self->surfaceType);
				FX_SmokePuff(armPos,120,120,120,200);

				RecursiveTurnOff(SimpleModelGetBolt(self, 3));
				RecursiveTurnOff(SimpleModelGetBolt(self, 4));
			}
		}
	}
}

/*QUAKED misc_castle_chair_big (1 .5 0) (-18 -15 -18) (12 15 39)  INVULNERABLE  NOPUSH x x x x FLUFF
A big high back chair and arms
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- falls apart when shot
*/
void SP_misc_castle_chair_big (edict_t *ent)
{
	ggBinstC		*bInstC = NULL;
	IGhoulInst		*boltInst = NULL;
	IGhoulInst		*boltInst2 = NULL;

	VectorSet (ent->mins,-17, -14, -17);
	VectorSet (ent->maxs, 12,  14,  38);

	Obj_partbreaksetup (CPD_CHAIR_BIG,&castleModelData[OBJ_CHAIR_BIG],
		castleObjBoltPartsData, castleObjBreak);

	// heh. simple model. heh heh.
	SimpleModelInit2(ent,&castleModelData[OBJ_CHAIR_BIG],NULL,NULL);

	// These are given as you face the chair
	// bolt1 - left legs
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY01",
						castleModelData[OBJ_CHAIR_BIG_LEG],"DUMMY01",NULL);
	// bolt2 - right legs
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY02",
						castleModelData[OBJ_CHAIR_BIG_LEG],"DUMMY01",NULL);
	// bolt3 - left arm
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY03",
							castleModelData[OBJ_CHAIR_BIG_ARM],"DUMMY03",NULL);
	// bolt4 - right arm
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY04",
								castleModelData[OBJ_CHAIR_BIG_ARM],"DUMMY03",NULL);
	// bolt5 - back
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY05",
									castleModelData[OBJ_CHAIR_BIG_SEATBACK],"DUMMY05",NULL);
	ent->pain = Obj_partpain;

	// gonna need these if the chair ever has all of its legs shot off
	VectorCopy(ent->s.angles,ent->moveinfo.start_angles);
}

void globe_spin (edict_t *self)
{
	eft_rotate_t	*rotInfo = NULL;
	
	if (rotInfo = FXA_GetRotate(self))
	{
		rotInfo->yRot *= 0.75f;	// Add friction
		if (abs(rotInfo->yRot) < (0.3f * 512))//512 accounts for the shift induced by FXA_SetRotate()
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

void globe_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	ggBinstC		*globe;
	vec3_t			towherehit, cross, pathdir, up, angVel = {0,0,0};
	eft_rotate_t	*rotInfo = NULL;
	float fMult = 0.1f * damage;

	globe = SimpleModelFindBolt(self, castleModelData[OBJ_GLOBE],"DUMMY01");

	if (globe)
	{
		// only spin globe portion
		VectorSet(up, 0, 0, 1);
		VectorSubtract(other->s.origin, self->s.origin, pathdir);
		VectorNormalize(pathdir);
		VectorSubtract(wherehit,self->s.origin, towherehit);
		CrossProduct(pathdir, towherehit, cross);
		if (DotProduct(cross, up) < 0)
		{
			if (rotInfo = FXA_GetRotate(self))
			{
				// adding to an existing rotation
				rotInfo->yRot += fMult*512;//512 accounts for the shift induced by FXA_SetRotate()
			}
			else
			{
				// need to create a rotating effect
				angVel[YAW] = (fMult<30)?fMult:30;
				FXA_SetRotate(self, angVel, globe->GetInstPtr());
			}
		}
		else if (DotProduct(cross, up) > 0)
		{
			if (rotInfo = FXA_GetRotate(self))
			{
				// adding to an existing rotation
				rotInfo->yRot += (-fMult)*512;//512 accounts for the shift induced by FXA_SetRotate()
			}
			else
			{
				// need to create a rotating effect
				angVel[YAW] = (fMult<30)?-fMult:-30;
				FXA_SetRotate(self, angVel, globe->GetInstPtr());
			}
		}

		self->think = globe_spin;
		// need to think pretty often cuz that's where we'll slow down the chair's rotation to simulate friction
		self->nextthink = level.time + 0.5f;
	}
}


/*QUAKED misc_castle_globe (1 .5 0) (-7 -9 -26) (15 9 12)  INVULNERABLE  NOPUSH x x x x FLUFF
A globe of the earth.  
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- spins when hit
*/
void SP_misc_castle_globe (edict_t *ent)
{
	VectorSet (ent->mins,  -7,-9, -26);
	VectorSet (ent->maxs,  15, 9,  12);

	SimpleModelInit2(ent,&castleModelData[OBJ_GLOBE_BASE],NULL,NULL);

	SimpleModelAddBolt(ent,castleModelData[OBJ_GLOBE_BASE],"DUMMY01",
					castleModelData[OBJ_GLOBE],"DUMMY01",NULL);

	ent->pain = globe_pain;
}

/*QUAKED misc_castle_hedge (1 .5 0) (-10 -14 -12) (9 11 11)  INVULNERABLE  NOPUSH x x x x FLUFF
A hedge 
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_castle_hedge (edict_t *ent)
{
	VectorSet (ent->mins,  -10,-14, -12);
	VectorSet (ent->maxs,   9, 11,  11);

	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&castleModelData[OBJ_HEDGE],NULL,NULL);

}

/*QUAKED misc_castle_tree (1 .5 0) (-24 -21 -11) (24 21 74)  INVULNERABLE  NOPUSH x x x x FLUFF
A pine tree 
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_castle_tree (edict_t *ent)
{
	VectorSet (ent->mins,  -24,-21, -11);
	VectorSet (ent->maxs,   24, 21,  74);

	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&castleModelData[OBJ_TREE],NULL,NULL);

}

