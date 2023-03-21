/******************************************************
 * Objects for Bosnia level							  *
 ******************************************************/

#include "g_local.h"
#include "g_obj.h"

#define SEARCHLIGHT_CIRCLE		4
#define SEARCHLIGHT_RANDOM		8

#define TRUCK_TRIGGERED		4
#define TRUCK_WHEELS_ONLY	16

#define TRUCK_BRAKE_FACTOR	3

#define HLTH_BOSNIA_ANTI_JACK		500
#define HLTH_BOSNIA_FLAG_WALL		100
#define HLTH_BOSNIA_LANDMINE		1
#define HLTH_BOSNIA_RUBBLE1			1000
#define HLTH_BOSNIA_RUBBLE2			1000
#define HLTH_BOSNIA_SRCHBEAM		1000
#define HLTH_BOSNIA_SRCHLIGHT		1000
#define HLTH_BOSNIA_TANK_JACK		500
#define HLTH_BOSNIA_TRUCK_CHUNKS	500
#define HLTH_BOSNIA_TRUCK_OLD		1000

void breakable_brush_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);

//  misc_bosnia..
void SP_misc_bosnia_antipersonnel_jack (edict_t *ent);
void SP_misc_bosnia_flag_wall (edict_t *ent);
void SP_misc_bosnia_landmine (edict_t *ent);
void SP_misc_bosnia_searchbeam (edict_t *ent);
void SP_misc_bosnia_searchlight (edict_t *ent);
void SP_misc_bosnia_rubble1 (edict_t *ent);
void SP_misc_bosnia_rubble2 (edict_t *ent);
void SP_misc_bosnia_tank_jack (edict_t *ent);
void SP_misc_bosnia_truck_chunks (edict_t *ent);
void SP_misc_bosnia_truck_old (edict_t *ent);

spawn_t bosniaSpawns[] =
{
	//	misc_bosnia...
	{"misc_bosnia_antipersonnel_jack",	SP_misc_bosnia_antipersonnel_jack},
	{"misc_bosnia_flag_wall",			SP_misc_bosnia_flag_wall},
	{"misc_bosnia_landmine",			SP_misc_bosnia_landmine},
	{"misc_bosnia_searchbeam",			SP_misc_bosnia_searchbeam},
	{"misc_bosnia_searchlight",			SP_misc_bosnia_searchlight},
	{"misc_bosnia_rubble1",				SP_misc_bosnia_rubble1},
	{"misc_bosnia_rubble2",				SP_misc_bosnia_rubble2},
	{"misc_bosnia_tank_jack",			SP_misc_bosnia_tank_jack},
	{"misc_bosnia_truck_chunks",		SP_misc_bosnia_truck_chunks},
	{"misc_bosnia_truck_old",			SP_misc_bosnia_truck_old},

	{NULL,								NULL},
};

typedef enum
{
	OBJ_ANTIPERSONNEL_JACK = 0,
	OBJ_FLAG_WALL,
	OBJ_LANDMINE,
	OBJ_RUBBLE1,
	OBJ_RUBBLE2,
	OBJ_SEARCHBEAMBASE,
	OBJ_SEARCHBEAM,
	OBJ_SEARCHLIGHT,
	OBJ_SEARCHLIGHT2,
	OBJ_SEARCHLIGHT3,
	OBJ_TANK_JACK,
	OBJ_TRUCK_CHUNKS,
	OBJ_TRUCK_OLD,
	OBJ_TRUCK_OLD_WHEEL,
	MAX_OBJS
};

modelSpawnData_t bosniaModelData[MD_BOSNIA_SIZE] =
{	
//      dir							file				surfaceType			material			health						solid			material file  cnt  scale
"objects/bosnia/antipersonnel_jack","jack",				SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_BOSNIA_ANTI_JACK,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_ANITPERSONEL_JACK
"objects/bosnia/flag_wall",			"flag_wall",		SURF_WOOD_DBROWN,	MAT_WOOD_DBROWN,	HLTH_BOSNIA_ANTI_JACK,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_FLAG_WALL
"objects/bosnia/landmine",			"landmine",			SURF_METAL,			MAT_ROCK_BROWN,		HLTH_BOSNIA_LANDMINE,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_LANDMINE
"objects/bosnia/rubble1",			"rubble1",			SURF_STONE_LGREY,	MAT_ROCK_LGREY,		HLTH_BOSNIA_RUBBLE1,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_RUBBLE1
"objects/bosnia/rubble2",			"rubble2",			SURF_DEFAULT,		MAT_ROCK_DGREY,		HLTH_BOSNIA_RUBBLE2,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_RUBBLE2
"objects/bosnia/searchbeam",		"null",				SURF_DEFAULT,		MAT_DEFAULT,		HLTH_BOSNIA_SRCHBEAM,		SOLID_NOT,		NULL,			0,	0.0,	NULL,	// OBJ_SRCHBEAMBASE
"objects/bosnia/searchbeam",		"beam",				SURF_DEFAULT,		MAT_DEFAULT,		HLTH_BOSNIA_SRCHBEAM,		SOLID_NOT,		NULL,			0,	0.0,	NULL,	// OBJ_SRCHBEAM
"objects/bosnia/searchlight",		"base",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_BOSNIA_SRCHLIGHT,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_SEARCHLIGHT
"objects/bosnia/searchlight",		"arm",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_BOSNIA_SRCHLIGHT,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_SEARCHLIGHT2
"objects/bosnia/searchlight",		"lamp",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_BOSNIA_SRCHLIGHT,		SOLID_BBOX,		"glass",		0,	0.0,	NULL,	// OBJ_SEARCHLIGHT3
"objects/bosnia/tank_jack",			"jack",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_BOSNIA_TANK_JACK,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_TANK_JACK
"objects/bosnia/truck_chunks",		"chunks",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_BOSNIA_TRUCK_CHUNKS,	SOLID_BBOX,		"chunks",		0,	0.0,	NULL,	// OBJ_TRUCK_CHUNKS
"objects/bosnia/truck_old",			"truck",			SURF_METAL,			MAT_METAL_DGREY,	HLTH_BOSNIA_TRUCK_OLD,		SOLID_BBOX,		"truck",		0,	0.0,	NULL,	// OBJ_TRUCK_OLD
"objects/bosnia/truck_old",			"tire",				SURF_METAL,			MAT_METAL_DGREY,	HLTH_BOSNIA_TRUCK_OLD,		SOLID_BBOX,		NULL,			0,	0.0,	NULL,	// OBJ_TRUCK_OLD_WHEEL
};



void apjack_touch (edict_t *ent, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	float		otherbottom, jacktop;

//	origin lies 13 units above it's bottom and 13 below it's top
	otherbottom = other->s.origin[2] + other->mins[2];
	jacktop = ent->s.origin[2] + ent->maxs[2];	
	if ((otherbottom - jacktop) > -2) //FIXME? right now this is pretty arbitrary
	{	
		vec3_t		backoff, hurtpoint;
	
		VectorCopy(other->s.origin, hurtpoint);
		hurtpoint[2] += other->mins[2];
		VectorSubtract(other->s.origin, ent->s.origin, backoff);
		T_Damage(other, ent, ent, backoff, hurtpoint, ent->s.origin, 1, 3, DT_EXPLODE, MOD_EXPLOSIVE);
	}

}

/*QUAKED misc_bosnia_antipersonnel_jack (1 .5 0) (-15 -17 -10) (15 13 16) INVULNERABLE  NOPUSH x x x x FLUFF
Tri-pod shaped anti-personnel pointy things. 
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- hurts player when touching it
*/
void SP_misc_bosnia_antipersonnel_jack (edict_t *ent)
{
	ent->touch = apjack_touch;

	VectorSet (ent->mins, -15, -17, -10);
	VectorSet (ent->maxs,  15,  13,  16);

	SimpleModelInit2(ent,&bosniaModelData[OBJ_ANTIPERSONNEL_JACK],NULL,NULL);
}


/*QUAKED misc_bosnia_flag_wall (1 .5 0) (-45 -18 -25) (-39 42 77) INVULNERABLE  NOPUSH x x x x FLUFF
An animated banner hanging from a wall mounted arm.  
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_bosnia_flag_wall (edict_t *ent)
{
	VectorSet (ent->mins, -45, -18, -25);
	VectorSet (ent->maxs, -39,  42,  77);

	SimpleModelInit2(ent,&bosniaModelData[OBJ_FLAG_WALL],NULL,NULL);
}


void landmine_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	Obj_explode(self,&self->s.origin,1,1);
	FX_MakeDecalBelow(self->s.origin, FXDECAL_SCORCHMARK, 0); // pipe
	BecomeDebris (self,inflictor,attacker,damage,point);
}


void landmine_touch(edict_t *ent, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	landmine_die(ent,other,other,999,ent->s.angles);
}

void landmine_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	landmine_die(ent,other,other,999,wherehit);
}

/*QUAKED misc_bosnia_landmine (1 .5 0) (-13 -13 -2) (13 13 2) INVULNERABLE  NOPUSH x x x x FLUFF
A landmine surrounded by dirt
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
------ SPECIALS ------
- blows up when touched
*/
void SP_misc_bosnia_landmine (edict_t *ent)
{
	VectorSet (ent->mins, -13, -13, -2);
	VectorSet (ent->maxs,  13,  13,  2);

	SimpleModelInit2(ent,&bosniaModelData[OBJ_LANDMINE],NULL,NULL);

	ent->touch = landmine_touch;
	ent->die = landmine_die;

	gi.effectindex("environ/onfireburst");
}

/*QUAKED misc_bosnia_rubble1 (1 .5 0) (-13 -10 -5) (13 10 5) INVULNERABLE  NOPUSH x x x x FLUFF
Some broken bricks
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_bosnia_rubble1 (edict_t *ent)
{
	VectorSet (ent->mins, -13, -10, -5);
	VectorSet (ent->maxs,  13,  10,  5);

	SimpleModelInit2(ent,&bosniaModelData[OBJ_RUBBLE1],NULL,NULL);
}


/*QUAKED misc_bosnia_rubble2 (1 .5 0) (-31 -30 -9) (45 22 36) INVULNERABLE  NOPUSH x x x x FLUFF
Big twisted pipes
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A can't ever be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_bosnia_rubble2 (edict_t *ent)
{
	VectorSet (ent->mins, -31, -30, -9);
	VectorSet (ent->maxs,  45,  22,  36);

	ent->spawnflags |= SF_NOPUSH;

	SimpleModelInit2(ent,&bosniaModelData[OBJ_RUBBLE2],NULL,NULL);

}

void searchlight_pain2 (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t	holdorigin;

	VectorCopy(ent->s.origin,holdorigin);
	holdorigin[2] += 80;
	FX_LittleExplosion(holdorigin, 100, 0);	

}

void searchlight_pain (edict_t *ent, edict_t *other, float kick, int damage, vec3_t wherehit)
{
	vec3_t			holdorigin;

	IGhoulObj*		lightObj = NULL;
	IGhoulInst*		lampInst = NULL;

	// need the ghoulInst of the lamp
	if ( !(lampInst = SimpleModelGetBolt(ent, 2)) )
	{
		return;
	}

	// only break when our health is below 85% of maximum
	if (ent->health > (int)(ent->max_health*.85))
	{
		return;
	}

	lightObj = lampInst->GetGhoulObject();
	if (NULL == lightObj)
	{
		return;
	}

	GhoulID matID = lightObj->FindMaterial("glass");
	GhoulID skinID = 0;

	skinID = lightObj->FindSkin(matID, "bkglass");

	if (!matID && !skinID)
		return;

	lampInst->SetFrameOverride(matID, skinID);
	SimpleModelRemoveObject2(lampInst,"_BEAM");
	SimpleModelRemoveObject2(lampInst,"WORLD_OMNI");

	VectorCopy(ent->s.origin,holdorigin);
	holdorigin[2] += 80;
	FX_LittleExplosion(holdorigin, 100, 0);	

	ent->pain = searchlight_pain2;

//	IGhoulInst*					boltInst = NULL;
//	GhoulID						boltID = 0;

//	boltInst = SimpleModelGetBolt(ent, 1);
//	boltID = boltInst->GetGhoulObject()->FindPart("DUMMY01");
//	fxRunner.execContinualEffect("environ/tv_smoke", ent,ent->ghoulInst,boltID);

	fxRunner.execContinualEffect("environ/tv_smoke", ent);
}




void searchbeam_think(edict_t* ent)
{
	IGhoulInst*					boltInst = NULL;
	GhoulID						boltID = 0;
	boltonOrientation_c			boltonInfo;
	Matrix4						matRot, matOld, matNew;
	IGhoulInst*					lampInst = NULL;

	if ( !(lampInst = SimpleModelGetBolt(ent, 2)) )
	{
		return;
	}

	// ent->pos1 is the current orientation of the lamp
	//ent->pos2 is our next desired orientation
	if ((boltInst = SimpleModelGetBolt(ent, 1)) &&
		boltInst->GetGhoulObject())
	{
		boltID = boltInst->GetGhoulObject()->FindPart("DUMMY01");

	// this would have the searchlight moving randomly within its range of motion
		if (1.0f == ent->bouyancy)
		{
			// get new dest 
			VectorSet(ent->pos2, gi.flrand(-100, 100), gi.flrand(-100, 100), gi.flrand(200, 300));
			VectorAdd(ent->pos2, ent->s.origin, ent->pos2);
			ent->bouyancy = 0;
		}

		// turn the arm to face the target (only pivots horizontally)
		boltonInfo.root = ent;
		boltonInfo.boltonInst = boltInst;
		boltonInfo.boltonID = boltID;
		boltonInfo.parentInst = ent->ghoulInst;
		VectorCopy(ent->pos2, boltonInfo.vTarget);
		boltonInfo.fMinPitch = -4000;
		boltonInfo.fMaxPitch = 4000;
		boltonInfo.fMinYaw = -4000;
		boltonInfo.fMaxYaw = 4000;
		boltonInfo.fMaxTurnSpeed = 0.03;
		boltonInfo.bUsePitch = false;
		boltonInfo.bUseYaw = true;
		boltonInfo.bToRoot = true;
		boltonInfo.OrientBolton();

		// turn the lamp to face the target (only pivots vertically)
		
		boltInst = lampInst;
		if (boltInst->GetGhoulObject() && boltInst->GetParent() &&
			boltInst->GetParent()->GetGhoulObject())
		{
			boltonInfo.boltonInst = boltInst;
			boltonInfo.boltonID = boltInst->GetGhoulObject()->FindPart("DUMMY01");
			boltonInfo.parentInst = boltInst->GetParent();
			boltonInfo.parentID = boltInst->GetParent()->GetGhoulObject()->FindPart("DUMMY02");
			VectorCopy(ent->pos2, boltonInfo.vTarget);
			boltonInfo.fMinPitch = -M_PI;
			boltonInfo.fMaxPitch = M_PI;
			boltonInfo.fMinYaw = -4000;
			boltonInfo.fMaxYaw = 4000;
			boltonInfo.fMaxTurnSpeed = 0.03;
			boltonInfo.bUsePitch = true;
			boltonInfo.bUseYaw = false;
			boltonInfo.bToRoot = true;
			boltonInfo.OrientBolton();
		}
		
		ent->bouyancy += 0.02f;
		if (ent->bouyancy > 1.0f)
		{
			ent->bouyancy = 1.0f;
		}
	}

	ent->nextthink = level.time + FRAMETIME;
}

/*QUAKED misc_bosnia_searchbeam (1 .5 0)  (-8 -8 -8) (8 8 8) INVULNERABLE  NOPUSH x x x x FLUFF
A searchlight beam to be placed in the distance so it can scan the skies.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_bosnia_searchbeam (edict_t *ent)
{	
	ggBinstC	*cBolteeBolted=NULL;

	// Bbox is small because the model is so damn big.
	VectorSet (ent->mins, -8, -8, -8);
	VectorSet (ent->maxs,  8,  8,  8);

	ent->spawnflags |= SF_NOPUSH;

	// Need something to bolt the beam to
	SimpleModelInit2(ent,&bosniaModelData[OBJ_SEARCHBEAM],NULL,NULL);

//	cBolteeBolted = SimpleModelAddBolt(ent,bosniaModelData[OBJ_SEARCHBEAMBASE],"DUMMY01",
//						bosniaModelData[OBJ_SEARCHBEAM],"DUMMY01",NULL);

	SimpleModelSetSequence2(ent->ghoulInst,"beam",SMSEQ_LOOP);
//	cBolteeBolted = SimpleModelAddBolt(ent,bosniaModelData[OBJ_SEARCHBEAMBASE],"DUMMY01",
//						bosniaModelData[OBJ_SEARCHBEAMBASE],"DUMMY01",NULL);
/*
	if (cBolteeBolted)
	{
		cBolteeBolted = ComplexModelAddBolt(cBolteeBolted,
			bosniaModelData[OBJ_SEARCHBEAMBASE], "DUMMY02", 
			bosniaModelData[OBJ_SEARCHBEAM], "DUMMY01", NULL);
	}
*/
	ent->think = searchbeam_think;
	ent->nextthink = level.time + FRAMETIME;

}

void searchlight_think(edict_t* ent)
{
	IGhoulInst*					boltInst = NULL;
	GhoulID						boltID = 0;
	boltonOrientation_c			boltonInfo;
	Matrix4						matRot, matOld, matNew;
	IGhoulInst*					lampInst = NULL;

	// need the ghoulInst of the lamp
	if ( !(lampInst = SimpleModelGetBolt(ent, 2)) )
	{
		return;
	}

	// should probably be a spawnflag for static vs. random motion
	if (true)
	{
		// ent->pos1 is the current orientation of the lamp
		//ent->pos2 is our next desired orientation
		if ((boltInst = SimpleModelGetBolt(ent, 1)) &&
			boltInst->GetGhoulObject())
		{
			boltID = boltInst->GetGhoulObject()->FindPart("DUMMY01");

			// this would have the searchlight moving randomly within its range of motion
			if (ent->spawnflags & SEARCHLIGHT_RANDOM)
			{	
				if (1.0f == ent->bouyancy)
				{
					// get new dest 
					VectorSet(ent->pos2, gi.flrand(-100, 100), gi.flrand(-100, 100), gi.flrand(-100, 0));
					VectorAdd(ent->pos2, ent->s.origin, ent->pos2);
					ent->bouyancy = 0;
				}
				// turn the arm to face the target (only pivots horizontally)
				boltonInfo.root = ent;
				boltonInfo.boltonInst = boltInst;
				boltonInfo.boltonID = boltID;
				boltonInfo.parentInst = ent->ghoulInst;
				VectorCopy(ent->pos2, boltonInfo.vTarget);
				boltonInfo.fMinPitch = -4000;
				boltonInfo.fMaxPitch = 4000;
				boltonInfo.fMinYaw = -4000;
				boltonInfo.fMaxYaw = 4000;
				boltonInfo.fMaxTurnSpeed = 0.03;
				boltonInfo.bUsePitch = false;
				boltonInfo.bUseYaw = true;
				boltonInfo.bToRoot = true;
				boltonInfo.OrientBolton();

				// turn the lamp to face the target (only pivots vertically)
				boltInst = lampInst;
				if (boltInst->GetGhoulObject() && boltInst->GetParent() &&
					boltInst->GetParent()->GetGhoulObject())
				{
					boltonInfo.boltonInst = boltInst;
					boltonInfo.boltonID = boltInst->GetGhoulObject()->FindPart("DUMMY02");
					boltonInfo.parentInst = boltInst->GetParent();
					boltonInfo.parentID = boltInst->GetParent()->GetGhoulObject()->FindPart("DUMMY01");
					VectorCopy(ent->pos2, boltonInfo.vTarget);
					boltonInfo.fMinPitch = -M_PI;
					boltonInfo.fMaxPitch = M_PI;
					boltonInfo.fMinYaw = -4000;
					boltonInfo.fMaxYaw = 4000;
					boltonInfo.fMaxTurnSpeed = 0.03;
					boltonInfo.bUsePitch = true;
					boltonInfo.bUseYaw = false;
					boltonInfo.bToRoot = true;
					boltonInfo.OrientBolton();
				}
				ent->bouyancy += 0.02f;
				if (ent->bouyancy > 1.0f)
				{
					ent->bouyancy = 1.0f;
				}
			}	
			else if (ent->spawnflags & SEARCHLIGHT_CIRCLE)
			{	
				// scott sez, "have it rotate in a circle"
				if (boltInst = SimpleModelGetBolt(ent, 1))
				{
					boltInst->GetXForm(matOld);
					matRot.Rotate(2, 0.03f);
					matNew.Concat(matOld, matRot);
					boltInst->SetXForm(matNew);
				}
			}
		}

		ent->nextthink = level.time + FRAMETIME;
	}
	else
	{
		// targeting something specific
		ent->nextthink = level.time + FRAMETIME;
	}
}

/*QUAKED misc_bosnia_searchlight (1 .5 0) (-40 -40 -8) (40 40 100) INVULNERABLE  NOPUSH CIRCLE RANDOM x x FLUFF
A searchlight which scans the sky.
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - N/A can't ever be pushed
CIRCLE - moves in circle 
RANDOM - move randomly 
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_bosnia_searchlight (edict_t *ent)
{
	// base (root object)
	ggObjC *cBase = NULL;
	// arm
	ggObjC	*cArm = NULL,
			*cLamp = NULL;
	GhoulID	cBaseSeq=0,
			cLampSeq=0,
			cArmSeq=0;
	// bolt located on the base (aka "bolter")
	GhoulID cBolterBolt=0;
	// bolt located on the bolted-on item (aka "boltee")
	GhoulID cBolteeBolt=0;
	ggOinstC	*t=NULL;
	ggBinstC	*cBolteeBolted=NULL;
	IGhoulInst	*lampInst = NULL;
	GhoulID		tempMaterial = 0;
	Matrix4		matOld;

	VectorSet (ent->mins, -40, -40, -8);
	VectorSet (ent->maxs,  40,  40,  100);
	SimpleModelInit2(ent,&bosniaModelData[OBJ_SEARCHLIGHT],NULL,NULL);
	ent->max_health = ent->health;
	ent->touch = NULL;
	cBolteeBolted = SimpleModelAddBolt(ent,bosniaModelData[OBJ_SEARCHLIGHT],"DUMMY01",
						bosniaModelData[OBJ_SEARCHLIGHT2],"DUMMY01",NULL);

	gi.effectindex("environ/tv_smoke");
	if (cBolteeBolted)
	{
		if ((ent->spawnflags & SEARCHLIGHT_CIRCLE) || (ent->spawnflags & SEARCHLIGHT_RANDOM))
		{	// play a looping sequence
			SimpleModelSetSequence2(cBolteeBolted->GetInstPtr(), "arm", SMSEQ_LOOP);
		}
		cBolteeBolted = ComplexModelAddBolt(cBolteeBolted,
			bosniaModelData[OBJ_SEARCHLIGHT2], "DUMMY02", 
			bosniaModelData[OBJ_SEARCHLIGHT3], "DUMMY02", NULL);
	}

	ent->pain = searchlight_pain;

#if 0 // kef -- 10/12/99 server-side rotation is a net-hassle, evidently. we'll just animate the bolts.

	if ((ent->spawnflags & SEARCHLIGHT_CIRCLE) || (ent->spawnflags & SEARCHLIGHT_RANDOM))
	{
		ent->think = searchlight_think;
		ent->nextthink = level.time + FRAMETIME;
	}

	// NOTE: using edict_t::pos1 to store current target of the beam and pos2 for next target
	//use bouyancy to store random orientation info
	ent->bouyancy = 1.0f;
	VectorClear(ent->pos1);
	VectorClear(ent->pos2);
	// scott sez, "have it rotate in a circle"
	if (lampInst)
	{
		lampInst->GetXForm(matOld);
		matOld.Rotate(1, M_PI*0.25f);
		lampInst->SetXForm(matOld);
	}
#endif
}

void tank_jack_touch (edict_t *ent, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	float fVelSquared =	other->client->ps.pmove.velocity[0]*other->client->ps.pmove.velocity[0] +
						other->client->ps.pmove.velocity[1]*other->client->ps.pmove.velocity[1] +
						other->client->ps.pmove.velocity[2]*other->client->ps.pmove.velocity[2];
	if (other && other->client && (fVelSquared > 250000) )
	{
		vec3_t		backoff, hurtpoint;
	
		VectorCopy(other->s.origin, hurtpoint);
		hurtpoint[2] += other->mins[2];
		VectorSubtract(other->s.origin, ent->s.origin, backoff);
		T_Damage(other, ent, ent, backoff, hurtpoint, ent->s.origin, 1, 3, DT_EXPLODE, MOD_EXPLOSIVE);
	}
}

/*QUAKED misc_bosnia_tank_jack (1 .5 0) (-28 -28 -19) (28 28 23) INVULNERABLE  NOPUSH x x x x FLUFF
A tank jack
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_bosnia_tank_jack (edict_t *ent)
{
	VectorSet (ent->mins, -28, -28, -21);
	VectorSet (ent->maxs, 28, 28, 21);

	SimpleModelInit2(ent,&bosniaModelData[OBJ_TANK_JACK],NULL,NULL);
	ent->touch = tank_jack_touch; // sounds like a country/western dance
}

/*QUAKED misc_bosnia_truck_chunks (1 .5 0) (-15 -18 -27) (15 18 27) INVULNERABLE  NOPUSH x x x x FLUFF
WWII-style covered transport truck chunks
------ KEYS ------
Skin - skin type
0 - green
1 - red
2 - tan
Style - which chunk to display
0 - grill
1 -door
2 - panel
3 - tire
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_bosnia_truck_chunks (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins,-15, -18, -27);
	VectorSet (ent->maxs, 15,  18,  27);

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "chunks_red";
		break;
	case 2:
		skinname = "chunks_tan";
		break;
	default:
		skinname = "chunks";
		break;
	}	

	SimpleModelInit2(ent,&bosniaModelData[OBJ_TRUCK_CHUNKS],skinname,NULL);

	switch (ent->style)
	{
	case 1:
		SimpleModelSetObject(ent,"_DOOR");
		break;
	case 2:
		SimpleModelSetObject(ent,"_PANEL");
		break;
	case 3:
		SimpleModelSetObject(ent,"_TIRE");
		break;
	default:
		SimpleModelSetObject(ent,"_GRILL");
		break;
	}	
}

void truck_old_totally_explode (edict_t *self)
{
	edict_t		*part;
	char *skinname;
	int i;
	vec3_t	debrisNorm;

	fxRunner.exec("weapons/world/airexplode", self->s.origin);
	FX_C4Explosion(self);

	BlindingLight(self->s.origin, self->health*10, 0.9, 0.5);
	ShakeCameras (self->s.origin, 100, 300, DEFAULT_JITTER_DELTA);

	VectorClear(debrisNorm);
	FX_ThrowDebris(self->s.origin,debrisNorm, 8,DEBRIS_LRG, self->material, 0,0,0, 0);

	switch (self->s.skinnum)
	{
	case 1:
		skinname = "chunks_red";
		break;
	case 2:
		skinname = "chunks_tan";
		break;
	default:
		skinname = "chunks";
		break;
	}	
	for (i=0;i<4;++i)
	{
		part = G_Spawn();
		VectorSet (part->mins, -10, -11, -5);
		VectorSet (part->maxs, 10, 11, 6);

		SimpleModelInit2(part,&bosniaModelData[OBJ_TRUCK_CHUNKS],skinname,NULL);

		VectorCopy(self->s.origin, part->s.origin);
		part->s.origin[2] += 25;
		VectorCopy(self->s.angles, part->s.angles);
		part->velocity[2] = 400;
		part->velocity[1] = gi.irand(-200,200);
		part->velocity[0] = gi.irand(-200,200);

		part->avelocity[1] = gi.irand(-50,50);

		switch (i)
		{
		case 1:
			part->s.origin[0] += 20;
			part->s.origin[1] += 20;
			SimpleModelSetObject(part,"_DOOR");
			break;
		case 2:
			part->s.origin[0] += 20;
			part->s.origin[1] -= 20;
			SimpleModelSetObject(part,"_PANEL");
			break;
		case 3:
			part->s.origin[0] += 5;
			part->s.origin[1] -= 5;
			SimpleModelSetObject(part,"_GRILL");
			break;
		case 4:
			part->s.origin[0] += 35;
			part->s.origin[1] -= 35;
			SimpleModelSetObject(part,"_TIRE");
			break;
		default:
			part->s.origin[0] += 35;
			part->s.origin[1] -= 35;
			SimpleModelSetObject(part,"_TIRE");

		}	
	}


	G_FreeEdict(self);	// Remove original truck

}

void truck_old_think (edict_t *self)
{
	if (self->s.angles[2] > 90)
	{
		self->avelocity[2] = 0;
		self->think = truck_old_totally_explode;
		self->nextthink = level.time + 1;
	}
	else
	{
		self->think = truck_old_think;
		self->nextthink = level.time + .1;
	}
}

void truck_old_explode (edict_t *self, edict_t *other, edict_t *activator)
{
	T_RadiusDamage (self, self, 100, self, 100, 0);

	gmonster.RadiusDeafen(self, 200, 200);

	fxRunner.exec("weapons/world/airexplode", self->s.origin);
	FX_C4Explosion(self);

	self->movetype = MOVETYPE_DAN;	// Can be pushed around

	self->velocity[2] = 400;
	self->avelocity[2] = 110;

	self->think = truck_old_think;
	self->nextthink = level.time + .1;
}





void bed_stop (edict_t *self)
{
	GrabStuffOnTop (self);
	VectorClear(self->velocity);
	self->movetype = MOVETYPE_NONE;
	self->s.sound = 0;
	self->nextthink = -1;
}

void bed_deccel (edict_t *self)
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
			self->think = bed_stop;
			self->nextthink = level.time + .1;
			return;
	}
			
	newVel[0] = self->velocity[0] * (distanceLeft/((TRUCK_BRAKE_FACTOR+1)*thisDist));	
	newVel[1] = self->velocity[1] * (distanceLeft/((TRUCK_BRAKE_FACTOR+1)*thisDist));	
	newVel[2] = 0;
	VectorCopy(newVel, self->velocity);
	self->nextthink = level.time + .1;
}

qboolean bedEndofLineCheck (edict_t *self)
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
		if (distanceLeft < (TRUCK_BRAKE_FACTOR*thisDist))
		{
			vec3_t			newVel;
		
			//adjust velocity then stop
			newVel[0] = self->velocity[0] * (distanceLeft/((TRUCK_BRAKE_FACTOR+1)*thisDist));	
			newVel[1] = self->velocity[1] * (distanceLeft/((TRUCK_BRAKE_FACTOR+1)*thisDist));	
			newVel[2] = 0;
			VectorCopy(newVel, self->velocity);
			self->think = bed_deccel;
			self->nextthink = level.time + .1;
			return false;
		}
		else
		{
			return true;
		}
	}

}

void bed_WheelSpin (edict_t *self)
{
	
	ggBinstC		*wheel1,*wheel2,*wheel3,*wheel4;
	vec3_t			angVel = {0,0,0};
	eft_rotate_t	*rotInfo = NULL;

	int tire_circ;
	float speed,turns,radians;


	tire_circ = (30 * M_PI);	
	speed = VectorLength (self->velocity);
	turns = speed / tire_circ;
	radians = DEGTORAD * turns;
	radians = M_PI * turns;

	wheel1 = SimpleModelFindBolt(self, bosniaModelData[OBJ_TRUCK_OLD_WHEEL],"DUMMY01");
	wheel2 = SimpleModelFindBolt(self, bosniaModelData[OBJ_TRUCK_OLD_WHEEL],"DUMMY02");
	wheel3 = SimpleModelFindBolt(self, bosniaModelData[OBJ_TRUCK_OLD_WHEEL],"DUMMY03");
	wheel4 = SimpleModelFindBolt(self, bosniaModelData[OBJ_TRUCK_OLD_WHEEL],"DUMMY04");

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

			FXA_SetRotate(self, angVel, wheel1->GetInstPtr());
			FXA_SetRotate(self, angVel, wheel2->GetInstPtr());
			FXA_SetRotate(self, angVel, wheel3->GetInstPtr());
			FXA_SetRotate(self, angVel, wheel4->GetInstPtr());
		}
	}	
}

void bed_linecheck (edict_t *self)
{

	GrabStuffOnTop (self);
	bedEndofLineCheck(self);
	self->nextthink = level.time + .1;
	bed_WheelSpin (self);
}

void bed_accel (edict_t *self)
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
		self->think = bed_linecheck;
		self->nextthink = level.time + .1;
	}

	chance = gi.irand(0,5);
	bedEndofLineCheck(self);
	bed_WheelSpin (self);
}

void bed_go (edict_t *self, edict_t *other, edict_t *activator)
{
	// beds with facing 0 are "pointing" east (pos x)
	self->movetype = MOVETYPE_DAN;
	GrabStuffOnTop (self);
	self->velocity[1] = -1 * self->accel * sin(self->s.angles[1] * DEGTORAD);
	self->velocity[0] = self->accel * cos(self->s.angles[1] * DEGTORAD);
	self->friction = 0;
	self->gravity = 0;
	self->nextthink = level.time + .1;
	self->think = bed_accel;
	self->use = NULL;
	self->plUse = NULL;

//	self->s.sound = gi.soundindex("Ambient/Locs/Subway/Train/Move.wav");	
	
	bedEndofLineCheck(self);
}

void bedwheels_stop (edict_t *self)
{
	FXA_RemoveRotate(self);
}

void bedwheels_roll (edict_t *self)
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

	wheel1 = SimpleModelFindBolt(self, bosniaModelData[OBJ_TRUCK_OLD_WHEEL],"DUMMY01");
	wheel2 = SimpleModelFindBolt(self, bosniaModelData[OBJ_TRUCK_OLD_WHEEL],"DUMMY02");
	wheel3 = SimpleModelFindBolt(self, bosniaModelData[OBJ_TRUCK_OLD_WHEEL],"DUMMY03");
	wheel4 = SimpleModelFindBolt(self, bosniaModelData[OBJ_TRUCK_OLD_WHEEL],"DUMMY04");

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

void bedwheels_use (edict_t *self, edict_t *other, edict_t *activator)
{

	if (!self->count)
	{
		self->count = 1;	// Let 'em know we're moving	
		bedwheels_roll(self);
	}
	else
	{
		self->count = 0;	// Let 'em know we're not moving
		bedwheels_stop(self);
	}
}

void truck_touch (edict_t *ent, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	// the normal of impact seems to originate from 'other'
	vec3_t vOtherDir, vel, normal;

	if ( (NULL == plane) || (NULL == plane->normal) )
	{
		return;
	}
	VectorSubtract(ent->s.origin, other->s.origin, vOtherDir);
	VectorCopy(plane->normal, normal);
	VectorNormalize(normal);
	VectorNormalize(vOtherDir);
	// ignoring z-values. I hope the truck only moves horizontally...
	vOtherDir[2] = 0;
	if (DotProduct(vOtherDir, normal) > .9)
	{	// 'other' is in our path. bump him.
		VectorScale(ent->velocity, 15, vel);
		VectorAdd(other->velocity, vel, other->velocity);
		other->velocity[2] += 25; // up a little
		// do some damage, too. heh.
		if (other->health > 10)
		{
			other->health -= 5;
		}
	}
}

void bosnia_truck_remove(edict_t *self)
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

void bosnia_truck_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
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
			self->think = bosnia_truck_remove;
			self->nextthink = level.time + 5;
			self->die = NULL;
		}
	}
}


/*QUAKED misc_bosnia_truck_old (1 .5 0) (-96 -39 -45) (96 39 45) INVULNERABLE  NOPUSH TRIGGERED LIGHTS WHEELS_ONLY x FLUFF
WWII-style covered transport truck
------ KEYS ------
Skin - type of truck
0 - green WWII skin
1 - red Iraq skin
2 - rusted ACME truck
Distance - distance the truck will move when triggered
Speed - max speed (default 200)
------ SPAWNFLAGS ------
INVULNERABLE - can't be damaged.
NOPUSH - can't be pushed
WHEELS_ONLY - wheels turn when used, stop turning when used again.
FLUFF - won't show if gl_pictip is set
*/
void SP_misc_bosnia_truck_old (edict_t *ent)
{
	char *skinname;

	VectorSet (ent->mins, -96, -39, -45);
	VectorSet (ent->maxs, 96, 39, 45);

	ent->spawnflags |= SF_NOPUSH;

	switch (ent->s.skinnum)
	{
	case 1:
		skinname = "truck2";
		break;
	case 2:
		skinname = "truck3";
		break;
	default :
		skinname = "truck";
		break;
	}

	gi.effectindex("environ/normalsmoke");
	gi.soundindex("impact/explosion/med.wav");

	SimpleModelInit2(ent,&bosniaModelData[OBJ_TRUCK_OLD],skinname,NULL);

	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY01",
						bosniaModelData[OBJ_TRUCK_OLD_WHEEL],"DUMMY01",NULL);
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY02",
						bosniaModelData[OBJ_TRUCK_OLD_WHEEL],"DUMMY01",NULL);
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY03",
						bosniaModelData[OBJ_TRUCK_OLD_WHEEL],"DUMMY01",NULL);
	SimpleModelAddBolt(ent,*ent->objSpawnData,"DUMMY04",
						bosniaModelData[OBJ_TRUCK_OLD_WHEEL],"DUMMY01",NULL);

//	ent->use = truck_old_explode;

	if (ent->spawnflags & TRUCK_WHEELS_ONLY)
	{
		ent->count = 0;	// Let 'em know we're not moving
		ent->use = bedwheels_use;
		ent->touch = truck_touch;

		if (!ent->speed)
		{
			ent->speed = 200;
		}
	}
	if (ent->spawnflags & TRUCK_TRIGGERED)
	{
		ent->use = bed_go;
		ent->s.origin[2] += 1; // hack to avoid the sliding into floor brushes problem
//		ent->touch = bed_smush;  mcnutt said he won't need this
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
	ent->die = bosnia_truck_die;
}