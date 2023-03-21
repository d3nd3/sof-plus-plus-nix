// m_heliai.cpp
#include "g_local.h"
#include "ai_private.h"
#include "m_generic.h"
#include "m_heliai.h"
#include "g_obj.h"				// for aiming the gun and the heads
#include "callback.h"
#include "..\qcommon\ef_flags.h"

//#include "m_heli.h"
 
//#include <windows.h>	// temp, purely for OutputDebugString(). Will be removed.

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define HELI_DIFFICULTY 3

typedef enum
{
	HELIOBJ_HELI = 0,
	HELIOBJ_BARREL,
	HELIOBJ_BARREL_NULL,
	HELIOBJ_HEAD,
	HELIOBJ_ROTOR,
	HELIOBJ_NAV_LITE1,
	HELIOBJ_NAV_LITE2,
	HELIOBJ_NAV_LITE3,
	HELIOBJ_TAIL_LITE,
	HELIOBJ_CANOPY,
	HELIOBJ_HIND,
	HELIOBJ_HIND_CANOPY,
	HELIOBJ_MAXOBJS
};

modelSpawnData_t heliModelData[MD_HELI_SIZE] =
{	
//      dir					file			surfaceType			material			health				solid			material file		   cnt  scale
"enemy/chopper",			"cobrass",		SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		"cobra",				0,	DEBRIS_SM,	NULL,	// HELIOBJ_HELI = 0,
"enemy/chopper",			"barrel",		SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		"cobra",				0,	DEBRIS_SM,	NULL,	// HELIOBJ_BARREL,
"objects/generic/gun_auto",	"null",			SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		NULL,					0,	DEBRIS_SM,	NULL,	// HELIOBJ_BARREL_NULL
"enemy/chopper",			"head",			SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		"cobra",				0,	DEBRIS_SM,	NULL,	// HELIOBJ_HEAD,
"enemy/chopper",			"rotor",		SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		"cobra",				0,	DEBRIS_SM,	NULL,	// HELIOBJ_ROTOR,
"enemy/chopper",			"nav_lite",		SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		"cobra",				0,	DEBRIS_SM,	NULL,	// HELIOBJ_NAV_LITE1,
"enemy/chopper",			"nav_lite2",	SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		"cobra",				0,	DEBRIS_SM,	NULL,	// HELIOBJ_NAV_LITE2,
"enemy/chopper",			"nav_lite3",	SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		"cobra",				0,	DEBRIS_SM,	NULL,	// HELIOBJ_NAV_LITE3,
"enemy/chopper",			"tail_lite",	SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		"cobra",				0,	DEBRIS_SM,	NULL,	// HELIOBJ_TAIL_LITE,
"enemy/chopper",			"canopy",		SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		"cobra",				0,	DEBRIS_SM,	NULL,	// HELIOBJ_CANOPY,
"enemy/hind",				"hind",			SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		"hind",					0,	DEBRIS_SM,	NULL,	// HELIOBJ_HIND,
"enemy/hind",				"canopy",		SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		"hind",					0,	DEBRIS_SM,	NULL,	// HELIOBJ_HIND_CANOPY,
};

// kef -- copy of FX_ fns of the same name on the client
int GetGhoulChildPosDir2(vec3_t sourcePos, vec3_t sourceAng, IGhoulInst *inst, GhoulID partID,
						  IGhoulInst *childInst, GhoulID childPartID, char *name, char *name2, 
						  vec3_t pos, vec3_t dir, vec3_t right, vec3_t up);
int GetGhoulPosDir2(vec3_t sourcePos, vec3_t sourceAng, IGhoulInst *inst,
					   GhoulID partID, char *name, vec3_t pos, vec3_t dir, vec3_t right,
					   vec3_t up);

/// Someone please put this in the right header
void EntToWorldMatrix(vec3_t org, vec3_t angles, Matrix4 &m);



MainRotor2Callback theMainRotor2Callback;
MainHeliCallback2 theMainHeliCallback2;

bool MainRotor2Callback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	edict_t* self = (edict_t*)ent;
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = NULL;

	if (ai)
	{
		body = ai->GetHeliBody();
	}
	if (!body)
	{
		return false;
	}
	body->SetRotorEOS(true);
	return true;
}

bool MainHeliCallback2::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	edict_t* self = (edict_t*)ent;
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = NULL;

	if (ai)
	{
		body = ai->GetHeliBody();
	}
	if (!body)
	{
		return false;
	}
	body->SetFuselageEOS(true);
	return true;
}

// ensure angle is in range -179..0..180
//
_inline float angle2posneg2 (float _angle)
{
	_angle = anglemod(_angle);	// ensure known-range
	if (_angle>180)
		_angle-=360;

	return _angle;
}


float generic_ghoul_heli_ai::fHeliXWorldMin2 = -10000;
float generic_ghoul_heli_ai::fHeliXWorldMax2 = 10000;
float generic_ghoul_heli_ai::fHeliYWorldMin2 = -10000;
float generic_ghoul_heli_ai::fHeliYWorldMax2 = 10000;
float generic_ghoul_heli_ai::fHeliZWorldMin2 = -10000;
float generic_ghoul_heli_ai::fHeliZWorldMax2 = 10000;


void debug_drawbox(edict_t* self,vec3_t vOrigin, vec3_t vMins, vec3_t vMaxs, int nColor)
{
	vec3_t vO;
	vec3_t mns, mxs;
	if (self)
	{
		VectorCopy(self->mins, mns);
		VectorCopy(self->maxs, mxs);
	}
	else
	{
		VectorCopy(vMins, mns);
		VectorCopy(vMaxs, mxs);
	}
	// corpse stuff override
//	mns[0] = -8;
//	mns[1] = -8;
//	mxs[0] = 8;
//	mxs[1] = 8;
//	mxs[2] = 8;
	
	vec3_t corners[] = {
		{mns[0], mns[1], mns[2]},
		{mns[0], mns[1], mxs[2]},
		{mns[0], mxs[1], mns[2]},
		{mns[0], mxs[1], mxs[2]},
		{mxs[0], mns[1], mns[2]},
		{mxs[0], mns[1], mxs[2]},
		{mxs[0], mxs[1], mns[2]},
		{mxs[0], mxs[1], mxs[2]}
	};

	if (self)
	{
		VectorCopy(self->s.origin, vO);
	}
	else
	{
		VectorCopy(vOrigin, vO);
	}
	paletteRGBA_t p;
	float colors[] = {
		0,	0,	64,
		0,	0,	128,
		0,	0,	192,
		0,	0,	255,
		0,	64,	0,
		0,	128,0,
		0,	192,0,
		0,	255,0,
		64,	0,	0,
		128,0,	0,
		192,0,	0,
		255,0,	0
	};
	int lines[] = {
		0,	1,
		0,	2,
		0,	4,
		7,	6,
		7,	5,
		7,	3,
		2,	3,
		2,	6,
		6,	4,
		4,	5,
		5,	1,
		1,	3
	};
	vec3_t c1, c2;
	int i = 0;
	p.a = 250;
	p.r = p.g = p.b = 0;

	switch (nColor)
	{
	case 1:
		p.r = 255;
		break;
	case 2:
		p.g = 255;
		break;
	default:
		if (nColor)
		{
			p.b = 255;
		}
		break;
	}
	for (i = 0; i < 12; i++)
	{
		if (0 == nColor)
		{
			p.r = colors[i*3];
			p.g = colors[i*3+1];
			p.b = colors[i*3+2];
		}
		VectorAdd(vO, corners[lines[i*2]], c1);
		VectorAdd(vO, corners[lines[i*2+1]], c2);
		FX_MakeLine(c1, c2, p); // need a better one than this
	}
}

void generic_ghoul_heli_ai::Activate(edict_t &monster)
{
	isActive=true;
}

void generic_ghoul_heli_ai::BuildHeliModel(edict_t *self, char *ghoulname, char *subclass)
{
	// fuselage
	ggObjC *cheli=NULL;
	// bolt located on the fuselage (aka "bolter")
	GhoulID cBolterBolt=0;
	// bolt located on the bolted-on item (aka "boltee")
	GhoulID cBolteeBolt=0;
	ggOinstC	*t=NULL;
	ggBinstC	*cBolteeBolted=NULL, *bInstBarrelNULL = NULL;
	IGhoulInst	*pBoltInst=NULL;
	Matrix4	mat,mat1,mat2;
	GhoulID		tempNote=0;
	bool		bHind = (0 == strcmp("hind", subclass));	// heh heh. bHind. heh heh.
	float		fTailRotorScale = bHind?0.375:0.25f,
				fMainRotorScale = bHind?1.875:1.25f;


	// create the helicopter fuselage, init ghoul stuff, sequence

	// gotta do this to make setsimpleghoulmodel happy
	ent->s.renderfx &= ~RF_GHOUL;

	if (bHind)
	{
		SimpleModelInit2(self, &heliModelData[HELIOBJ_HIND], NULL, NULL);
	}
	else
	{
		SimpleModelInit2(self, &heliModelData[HELIOBJ_HELI], NULL, NULL);
	}
	if (self->ghoulInst)
	{
		self->ghoulInst->SetUserData(self);

		if (GetHeliBody())
		{
			//GetHeliBody()->SetFuselageObj(cheli);
			//let body know about my inst, too
			GetHeliBody()->SetHeliEdict(self);
		}
	}
	else
	{
		gi.dprintf("generic_ghoul_heli_ai::Init failed to create heli fuselage\n");
		return;
	}
	// create some navigation lights
	if (bHind)
	{
	}
	else
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HELI], "nav_lite", 
			heliModelData[HELIOBJ_NAV_LITE1], "to_nav_lite", NULL);
		if (cBolteeBolted)
		{	
			SimpleModelSetSequence2(cBolteeBolted->GetInstPtr(),"nav_lite", SMSEQ_LOOP);
		}
		else
		{
			gi.dprintf("generic_ghoul_heli_ai::Init failed to create nav_lite bolton\n");
		}
	}

	if (bHind)
	{
	}
	else
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HELI], "nav_lite2", 
			heliModelData[HELIOBJ_NAV_LITE2], "to_nav_lite2", NULL);
		if (cBolteeBolted)
		{	
			SimpleModelSetSequence2(cBolteeBolted->GetInstPtr(),"nav_lite2", SMSEQ_LOOP);
		}
		else
		{
			gi.dprintf("generic_ghoul_heli_ai::Init failed to create nav_lite2 bolton\n");
		}
	}

	if (bHind)
	{
	}
	else
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HELI], "nav_lite3", 
			heliModelData[HELIOBJ_NAV_LITE3], "to_nav_lite3", NULL);
		if (cBolteeBolted)
		{	
			SimpleModelSetSequence2(cBolteeBolted->GetInstPtr(),"nav_lite3", SMSEQ_LOOP);
		}
		else
		{
			gi.dprintf("generic_ghoul_heli_ai::Init failed to create nav_lite3 bolton\n");
		}
	}

	// create a tail light
	if (bHind)
	{
	}
	else
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HELI], "tail_lite", 
			heliModelData[HELIOBJ_TAIL_LITE], "to_tail_lite", NULL);
		if (NULL == cBolteeBolted)
		{
			gi.dprintf("generic_ghoul_heli_ai::Init failed to create tail_lite bolton\n");
		}
	}

	// create the gun
	//
	// step 1 -- create a NULL, attach it to the heli. this will allow yaw.
	if (bHind)
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HIND], "barrel_base", 
			heliModelData[HELIOBJ_BARREL_NULL], "DUMMY01", NULL);
	}
	else
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HELI], "barrel_base", 
			heliModelData[HELIOBJ_BARREL_NULL], "DUMMY01", NULL);
	}
	if (cBolteeBolted)
	{
		bInstBarrelNULL = cBolteeBolted;
		GetHeliBody()->SetBarrelNULLInst(cBolteeBolted);
		if ((pBoltInst = cBolteeBolted->GetInstPtr()) && pBoltInst->GetGhoulObject())
		{
			GetHeliBody()->SetBarrelNULLBolt(pBoltInst->GetGhoulObject()->FindPart("DUMMY01"));
			// the NULL actually has some geometry in it (ghoul made me do it :< ) so turn it off
			SimpleModelTurnOnOff(pBoltInst, false);
		}
	}
	else
	{
		gi.dprintf("generic_ghoul_heli_ai::Init failed to create NULL (for barrel) bolton\n");
	}

	// step 2 -- create the barrel, attach it to the NULL. this will allow pitch.
	cBolteeBolted = ComplexModelAddBolt(bInstBarrelNULL, heliModelData[HELIOBJ_BARREL_NULL], "DUMMY01", 
		heliModelData[HELIOBJ_BARREL], "to_barrel_base", NULL);
	if (cBolteeBolted)
	{
		GetHeliBody()->SetBarrelInst(cBolteeBolted);
		if ((pBoltInst = cBolteeBolted->GetInstPtr()) && pBoltInst->GetGhoulObject())
		{
			GetHeliBody()->SetBarrelBolt(pBoltInst->GetGhoulObject()->FindPart("to_barrel_base"));
		}
	}
	else
	{
		gi.dprintf("generic_ghoul_heli_ai::Init failed to create barrel bolton\n");
	}

	// create the gunner's head
	if (bHind)
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HIND], "body1", 
			heliModelData[HELIOBJ_HEAD], "to_body", NULL);
	}
	else
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HELI], "body1", 
			heliModelData[HELIOBJ_HEAD], "to_body", NULL);
	}
	if (cBolteeBolted)
	{
		GetHeliBody()->SetGunnerInst(cBolteeBolted);
		if ((pBoltInst = cBolteeBolted->GetInstPtr()) && pBoltInst->GetGhoulObject())
		{
			GetHeliBody()->SetGunnerBolt(pBoltInst->GetGhoulObject()->FindPart("to_body"));
			GetHeliBody()->SetGunBoltOffset( 0/*84.795f*/, 0.0f, 6.411f);
		}
	}
	else
	{
		gi.dprintf("generic_ghoul_heli_ai::Init failed to create gunner's head bolton\n");
	}

	// create the pilot's head
	if (bHind)
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HIND], "body2", 
			heliModelData[HELIOBJ_HEAD], "to_body", NULL);
	}
	else
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HELI], "body2", 
			heliModelData[HELIOBJ_HEAD], "to_body", NULL);
	}
	if (cBolteeBolted)
	{
		GetHeliBody()->SetPilotInst(cBolteeBolted);
		if ((pBoltInst = cBolteeBolted->GetInstPtr()) && pBoltInst->GetGhoulObject())
		{
			GetHeliBody()->SetPilotBolt(pBoltInst->GetGhoulObject()->FindPart("to_body"));
			GetHeliBody()->SetPilotBoltOffset(22/*44.215f*/, 0.0f, 12.935f);
		}
	}
	else
	{
		gi.dprintf("generic_ghoul_heli_ai::Init failed to create pilot's head bolton\n");
	}

	// create the tail rotor
	if (bHind)
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HIND], "rotor_tail", 
			heliModelData[HELIOBJ_ROTOR], "to_rotor", NULL);
	}
	else
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HELI], "rotor_tail", 
			heliModelData[HELIOBJ_ROTOR], "to_rotor", NULL);
	}
	if (cBolteeBolted)
	{
		GetHeliBody()->SetTailRotorBoltInst(cBolteeBolted);
		GetHeliBody()->SetTailRotorInst(cBolteeBolted);
		if (pBoltInst = cBolteeBolted->GetInstPtr())
		{
			pBoltInst->GetXForm(mat);
			mat1=mat2=mat;
			mat2.Scale(fTailRotorScale);
			mat.Concat(mat1, mat2);
			pBoltInst->SetXForm(mat);
		}
		if (self->spawnflags & IN_AIR)
			SimpleModelSetSequence2(pBoltInst,"rotor_anim", SMSEQ_LOOP);
		else
			SimpleModelSetSequence2(pBoltInst,"rotor", SMSEQ_LOOP);
	}
	else
	{
		gi.dprintf("generic_ghoul_heli_ai::Init failed to create tail rotor bolton\n");
	}

	// create the main rotor
	if (bHind)
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HIND], "rotor_main", 
			heliModelData[HELIOBJ_ROTOR], "to_rotor", NULL);
	}
	else
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HELI], "rotor_main", 
			heliModelData[HELIOBJ_ROTOR], "to_rotor", NULL);
	}
	if (cBolteeBolted)
	{
		GetHeliBody()->SetMainRotorBoltInst(cBolteeBolted);
		GetHeliBody()->SetMainRotorInst(cBolteeBolted);
		if (pBoltInst = cBolteeBolted->GetInstPtr())
		{
			pBoltInst->GetXForm(mat);
			mat1=mat2=mat;
			mat2.Scale(fMainRotorScale);
			mat.Concat(mat1, mat2);
			pBoltInst->SetXForm(mat);
		}
		GetHeliBody()->SetRotorEOS(false);
		if (self->spawnflags & IN_AIR)
			SimpleModelSetSequence2(pBoltInst,"rotor_anim", SMSEQ_LOOP);
		else
			SimpleModelSetSequence2(pBoltInst,"rotor", SMSEQ_LOOP);
		// register a callback for the main rotor's sequence
		tempNote=pBoltInst->GetGhoulObject()->FindNoteToken("EOS");
		if (tempNote && pBoltInst)
		{
			pBoltInst->AddNoteCallBack(&theMainRotor2Callback,tempNote);
		}
	}
	else
	{
		gi.dprintf("generic_ghoul_heli_ai::Init failed to create main rotor bolton\n");
	}

	// create the canopy for the cockpit
	if (bHind)
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HIND], "canopy", 
			heliModelData[HELIOBJ_HIND_CANOPY], "to_canopy", NULL);
	}
	else
	{
		cBolteeBolted = SimpleModelAddBolt(self, heliModelData[HELIOBJ_HELI], "canopy", 
			heliModelData[HELIOBJ_CANOPY], "to_canopy", NULL);
	}
	if (cBolteeBolted)
	{
		GetHeliBody()->SetCockpitInst(cBolteeBolted);
		if (pBoltInst = cBolteeBolted->GetInstPtr())
		{
			SimpleModelSetSequence2(pBoltInst,"canopy", SMSEQ_HOLD);
		}
	}
	else
	{
		gi.dprintf("generic_ghoul_heli_ai::Init failed to create canopy bolton\n");
	}

	// create our rotor sound
	self->s.sound =  gi.soundindex("Enemy/Helicopter/Blade.wav");
	self->s.sound_data = (255 & ENT_VOL_MASK) | SND_FARATTN;
}

void generic_ghoul_heli_ai::Init(edict_t *self, char *ghoulname, char* subclass)
{
	m_bActionDone = false;
	m_bTimeToDie = false;
	m_bPilotDead = false;
	m_bGunnerDead = false;
	m_attacker = NULL;
	m_nActionSize = 0;
	m_ScriptActionCounter = m_LastThinkingActionID = 0;

	m_waypoints = NULL;
	m_nNumWaypoints = m_nNumWaypointPts = m_nNumWaypointVols = 0;

	BuildHeliModel(self, ghoulname, subclass);
}

void generic_ghoul_heli_ai::ChangeBody(edict_t *self, char *ghoulname, char* subclass)
{
	float fMin = min(self->mins[0],self->mins[1]);
	float fMax = max(self->maxs[0],self->maxs[1]);
	float fXYRadius	= max(fabs(fMin),fMax);						// max radius dist from centre
	float fZRadius	= (max(fabs(self->mins[2]),self->maxs[2]))*2;	// max radius dist from centre
	float		fAggressiveness = 0;
	vec3_t		vPad = {0,0,0}, vDeathDest = {0,0,0};

	body_heli* oldbody = GetHeliBody(), *newbody = NULL;

	if (oldbody)
	{
		fAggressiveness = oldbody->m_fAIAggressiveness;
		VectorCopy(oldbody->m_v3HeliPadRestPos, vPad);
		VectorCopy(oldbody->m_v3HeliDeathDest, vDeathDest);
	}
	body.Destroy();
	game_ghoul.RemoveObjectInstances(self);
	MyGhoulObj = NULL;
	AddBody(self);
	BuildHeliModel(self, ghoulname, subclass);

	newbody = GetHeliBody();

	if (newbody)
	{
		VectorSet(newbody->m_v3HeliTraceMins,-fXYRadius,-fXYRadius,-fZRadius);
		VectorSet(newbody->m_v3HeliTraceMaxs, fXYRadius, fXYRadius, fZRadius);
		VectorCopy(vPad,  newbody->m_v3HeliPadRestPos);
		VectorCopy(vDeathDest,		newbody->m_v3HeliDeathDest);									
		newbody->m_fAIAggressiveness = fAggressiveness; 
	}
	self->flags	   |= FL_NO_KNOCKBACK;
	self->movetype	= MOVETYPE_FLY;
	self->solid		= SOLID_BBOX;
	self->takedamage= DAMAGE_YES;
	self->pain		= NULL;//CobraThink_Pain;
	self->die		= NULL;//CobraThink_Die;
}


generic_ghoul_heli_ai::~generic_ghoul_heli_ai()
{
	heliWaypoint* temp = NULL;
	while (m_waypoints)
	{
		temp = m_waypoints;
		m_waypoints = m_waypoints->m_next;
		m_nNumWaypoints--;
		delete temp;
	}
}

void generic_ghoul_heli_ai::AddBody(edict_t *monster)
{
	if (!monster)
	{
		return;
	}
	if (!body)
	{
		body = new body_heli(monster);
	}
}

action_c *generic_ghoul_heli_ai::MoveAction(decision_c *od, action_c *oa,
	ai_c* ai, mmove_t *newanim, int nCommand, vec3_t vPos, edict_t* target, float fArg)
{
	// uniquely identify each action generated by the script so we can poll it later in 
	//HelicopterDoneEvent::Process() to find out when it's done
	m_ScriptActionCounter++;
	return new helimove_action(od, oa, ai, newanim, nCommand, vPos, target, fArg, m_ScriptActionCounter);
}

action_c *generic_ghoul_heli_ai::FaceAction(decision_c *od, action_c *oa,
	ai_c* ai, mmove_t *newanim, int nCommand, vec3_t vPos, edict_t* target, float fArg)
{
	// uniquely identify each action generated by the script so we can poll it later in 
	//HelicopterDoneEvent::Process() to find out when it's done
	m_ScriptActionCounter++;
	return new heliface_action(od, oa, ai, newanim, nCommand, vPos, target, fArg, m_ScriptActionCounter);
}

action_c *generic_ghoul_heli_ai::AttackAction(decision_c *od, action_c *oa,
	ai_c* ai, mmove_t *newanim, int nCommand, vec3_t vPos, edict_t* target, float fArg)
{
	// uniquely identify each action generated by the script so we can poll it later in 
	//HelicopterDoneEvent::Process() to find out when it's done
	m_ScriptActionCounter++;
	return new heliattack_action(od, oa, ai, newanim, nCommand, vPos, target, fArg, m_ScriptActionCounter);
}

void generic_ghoul_heli_ai::SetCurrentActionThink(void	(*think)(edict_t *self))
{
	if (current_action)
	{
		((heli_action*)(action_c*)current_action)->SetThink(think);
	}
}

void generic_ghoul_heli_ai::SetCurrentActionNextThink(void	(*think)(edict_t *self))
{
	if (current_action)
	{
		((heli_action*)(action_c*)current_action)->SetNextThink(think);
	}
}
void EntToWorldMatrix(vec3_t org, vec3_t angles, Matrix4 &m);

static float s_fNextThink = 0; // only used when the heli dies in jpn3

void exploding_heli_final(edict_t *self)
{
	G_FreeEdict(self);
}

void exploding_heli_remove(edict_t *self)
{
	vec3_t		vPos, vF;
	generic_ghoul_heli_ai	*ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli				*body = ai->GetHeliBody();

	// big explosion inside the heli fuselage
	GetGhoulPosDir2(self->s.origin, self->s.angles, self->ghoulInst, 0, "ROTOR_MAIN1", vPos, vF, NULL, NULL);
	fxRunner.exec("heli_expl", vPos, MULTICAST_ALL);
	self->s.effects |= EF_EXPLODING;
	// make the fuselage disappear
	//SimpleModelTurnOnOff(self->ghoulInst, false);

	// make the rotor smoke go away
	body->UpdateSmoke(self, -1);

	// in a few seconds make the whole deal go away
	s_fNextThink = level.time + 5;
	self->nextthink = level.time + FRAMETIME;
	self->think = exploding_heli_final;
}

void exploding_heli_think2(edict_t *self)
{
	IGhoulObj				*obj = NULL;
	GhoulID					idBase = 0;
	float					xattenuation = ATTN_NORM, xvolume = 1.0;

	// explosion at base of rotor
	if(obj = self->ghoulInst->GetGhoulObject())
	{
		idBase = obj->FindPart("ROTOR_MAIN");
		fxRunner.execWithInst("weapons/world/autogundeath", self, 
				self->ghoulInst, idBase);
		gi.positioned_sound (self->s.origin, g_edicts, 0, gi.soundindex("impact/explosion/exp1.wav"), xvolume, xattenuation, 0);
	}

	s_fNextThink = level.time + 1;
	self->nextthink = level.time + FRAMETIME;
	self->think = exploding_heli_remove;
}

void exploding_heli_think(edict_t *self)
{
	generic_ghoul_heli_ai	*ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli				*body = ai->GetHeliBody();
	IGhoulObj				*obj = NULL;
	GhoulID					idCockpit = 0;
	float					xattenuation = ATTN_NORM, xvolume = 1.0;

	if (body)
	{
		// remove pilot and gunner
		if (body->GetGunnerInst())
		{
			body->GetGunnerInst()->SetOnOff(false,level.time);
		}
		if (body->GetPilotInst())
		{
			body->GetPilotInst()->SetOnOff(false,level.time);
		}
	}

	// explosion inside cockpit
	if(obj = self->ghoulInst->GetGhoulObject())
	{
		idCockpit = obj->FindPart("BODY1");
		fxRunner.execWithInst("weapons/world/autogundeath", self, 
				self->ghoulInst, idCockpit);
		gi.positioned_sound (self->s.origin, g_edicts, 0, gi.soundindex("impact/explosion/med.wav"), xvolume, xattenuation, 0);
	}

	s_fNextThink = level.time + .2;
	self->nextthink = level.time + FRAMETIME;
	self->think = exploding_heli_think2;
}

void generic_ghoul_heli_ai::Think(edict_t &monster)
{
	int		i = 0, nDifficulty = HELI_DIFFICULTY; // right now this is the number of shots fired per attack

	// if we're dead, do some special stuff
	monster.nextthink = level.time + FRAMETIME;
	if (monster.deadflag == DEAD_DEAD)
	{
		monster.s.effects &= ~EF_EXPLODING;
		monster.movetype = MOVETYPE_FLY;
		monster.gravity = 0;
		monster.velocity[2] = 0;
		if (level.time >= s_fNextThink)
		{
			monster.think(&monster);
		}
		return;
	}

	if (ai_freeze->value)
	{
		return;
	}

//	debug_drawbox(&monster, NULL, NULL, NULL, 0);
//	Com_Printf("%3.2f%%, %d hit points\n", (float)monster.health/(float)monster.max_health, monster.health);
	// that Greg...
	if (GetHeliBody() && GetHeliBody()->IsFiring())
	{
		IGhoulInst		*instMuzzle = GetHeliBody()->GetBarrelInst();
		IGhoulInst		*instHeli = GetHeliBody()->GetHeliInst();
		IGhoulObj		*obj = NULL;
		GhoulID			idEndOfMuzzle = 0, idBoltHeliToMuzzle = 0;
		if(instHeli && instMuzzle && (obj = instMuzzle->GetGhoulObject()) )
		{
			idEndOfMuzzle = obj->FindPart("flash_barrel");
			fxRunner.execWithInst("weapons/othermz/heli", &monster, 
				instMuzzle, idEndOfMuzzle);
			// actually attack our current target
			vec3_t vWaver, vFirePoint, vFireDir, vFireDir2;
			obj = instHeli->GetGhoulObject();
			if(obj)
			{
				idBoltHeliToMuzzle = obj->FindPart("barrel_base");

				GetGhoulChildPosDir2(monster.s.origin, monster.s.angles,
					instHeli,
					idBoltHeliToMuzzle,
					instMuzzle, 
					idEndOfMuzzle,
					NULL, NULL, vFirePoint, vFireDir, NULL, NULL);	// this is way too long ;(
				VectorScale(vFireDir, 100, vFireDir);
//				for (i = 0; i < nDifficulty; i++)
				{
					VectorSet(vWaver, gi.irand(-10,10), gi.irand(-10,10), gi.irand(-10,10));
					VectorAdd(vFireDir, vWaver, vFireDir2);
					weapons.attack(ATK_HELIGUN, &monster, vFirePoint, vFireDir2);//same damage, but bigger impact sound
				}
				//gi.sound (self, CHAN_WEAPON, gi.soundindex("weapons/assault/fire.wav"), .9, .5, 0);//tihs wasn't here before - do we want it here?
			}
		}
	}
	EvaluateSenses(monster);
#if 0
//#ifdef _DEBUG
	if (monster.spawnflags & DEBUG_WAYPOINTS)
	{
		// loop-limit number is how many to draw per cycle...
		for (int iPlexPass=0; iPlexPass<1; iPlexPass++)
		{
			vec3_t	vOrigin = {0,0,0},
				vMins = {fHeliXWorldMin2,fHeliYWorldMin2,fHeliZWorldMin2},
					vMaxs = {fHeliXWorldMax2,fHeliYWorldMax2,fHeliZWorldMax2};


			static int iMasterPlex=0;	
			int iThisPlex=-1;

			// draw sky box
			if (++iThisPlex==iMasterPlex)
				debug_drawbox(NULL, vOrigin, vMins, vMaxs, 1);

			// draw heli box
			if (++iThisPlex==iMasterPlex)
				debug_drawbox(&monster, vOrigin, vMins, vMaxs, 0);

			// draw volumes
			int i = 0, n = GetNumWaypointVols();
			heliWaypoint* waypt = NULL;

			for (i = 0; i < n; i++)
			{
				if (++iThisPlex==iMasterPlex)
				{
					waypt = GetWaypointVol(i);
					if (waypt)
					{
						waypt->GetVolumeMins(vMins);
						waypt->GetVolumeMaxs(vMaxs);
						debug_drawbox(NULL, vOrigin, vMins, vMaxs, 2);
					}
				}
			}

			n = GetNumWaypointPts();	
			for (i = 0; i < n; i++)
			{
				if (++iThisPlex==iMasterPlex)
				{
					waypt = GetWaypointPt(i);
					if (waypt)
					{
						VectorSet(vMins,-10,-10,-10);
						VectorSet(vMaxs, 10, 10, 10);			
						debug_drawbox(NULL, waypt->m_v, vMins, vMaxs, 3);
					}
				}
			}
			if (++iMasterPlex == 2 + GetNumWaypointVols() + GetNumWaypointPts())
				iMasterPlex = 0;
		}// for (int iPlexPass=0; iPlexPass<5; iPlexPass++)
	}// if (monster.spawnflags & DEBUG_WAYPOINTS)
#endif
	// if someone just capped the pilot or gunner, we want the heli 
	//to go out of control and crash regardless of the current action.
	if (GetHeliBody())
	{
		if (GetHeliBody()->IsPilotDead())
		{
			if (GetHeliBody()->IsGunnerDead())
			{
				CrewMemberKilled(&monster, HC_SKIN_BOTH);
			}
			else
			{
				CrewMemberKilled(&monster, HC_SKIN_PILOT);
			}
		}
		else if (GetHeliBody()->IsGunnerDead())
		{
			CrewMemberKilled(&monster, HC_SKIN_GUNNER);
		}
	}
	if (!current_action)
	{	// do some thinking - this is not correct
		if (actions.size())
		{
			NextAction(monster);
		}
	}
	m_nActionSize = actions.size();
#if 0
#else
	if (current_action)
	{
		m_LastThinkingActionID = ((heli_action*)(action_c*)current_action)->GetID();
		if (current_action->Think(*this, monster))
		{ 
			NextAction(monster);
			if (GetHeliBody())
			{
				GetHeliBody()->ResetScriptParams();
			}
		}
	}
#endif
	gi.linkentity(&monster);

	if (monster.linkcount != linkcount)
	{
		linkcount = monster.linkcount;
		gmonster.CheckGround (&monster);
	}
	gmonster.CatagorizePosition (&monster);
	gmonster.WorldEffects (&monster);//leaving this in for drowning, lava damage, etc., but it should prolly be handled in ai class somewhere

	//if monster is dead, consider removing it
	if (m_bTimeToDie)
	{
		//get ai to poll actions, decisions, & senses to check if removal is ok
		if (SafeToRemove(monster))
		{
			Die(monster, m_attacker, m_attacker, monster.max_health, vec3_origin);
			if (monster.killtarget && (strcmp("musicscript", monster.killtarget)))
			{	// hacked in ending for jpn3
				vec3_t pos = {-1000, 744, 986};
				VectorSet(monster.velocity, -10, 40, 0);
				VectorCopy(pos,monster.s.origin);
				monster.s.angles[ROLL] = -10;
				s_fNextThink = level.time + 1;
				monster.think = exploding_heli_think;
				monster.nextthink = level.time + FRAMETIME;
			}
			else
			{
				G_FreeEdict (&monster);
			}
		}
	}
}

void generic_ghoul_heli_ai::CrewMemberKilled(edict_t* self, int nSkin)
{
	// make sure we're solid so we can hit something and explode now that we're out of control
	self->movetype = MOVETYPE_FLY;
	if ( !(GetHeliBody()->m_flags & HELIFLAG_ISDEADACKNOWLEDGED) )
	{
		// get rid of all actions currently available
		list<action_c_ptr>::iterator	ia;
		vec3_t vPos = {0,0,0};
		for (ia=actions.begin();ia!=actions.end();ia++)
		{
			(*ia).Destroy();
		}
		while (actions.size())
		{
			actions.pop_front();
		}

		//this may be unnecessary, but it makes me nervous seeing current_action just set to NULL--ss
		if (current_action)
		{
			current_action.Destroy();
		}
		current_action = NULL;

		// only send the heli out of control if it's not landed
		if (self->s.origin[0] != GetHeliBody()->m_v3HeliPadRestPos[0] ||
			self->s.origin[1] != GetHeliBody()->m_v3HeliPadRestPos[1] ||
			self->s.origin[2] != GetHeliBody()->m_v3HeliPadRestPos[2])
		{
			// add a new action for going out of control
			decision_c*					DecisionPointer;
			if (!(DecisionPointer = ((ai_c*)(this))->FindScriptedDecision(self)))
			{
				NewDecision(DecisionPointer = new scripted_decision(NULL,SCRIPTED_PRIORITY_ROOT,0,self),self);
			}
			NewAction( MoveAction(DecisionPointer,
									NULL, (ai_c*)this, &MMoves[15], 
									generic_ghoul_heli_ai::hse_OUT_OF_CONTROL, 
									vPos, NULL, 0),self, true); 

			// alert our body to its imminent demise
			GetHeliBody()->m_flags |= HELIFLAG_ISDEADACKNOWLEDGED;
		}
		if (lock_blood)
		{
			// no exploding heads for you
			return;
		}
		// change skin on the cockpit
		GetHeliBody()->ChangeCockpitSkin(nSkin);
		// make the afflicted head disappear
		if (nSkin == HC_SKIN_PILOT &&
			GetHeliBody()->GetPilotInst())
		{
			GetHeliBody()->GetPilotInst()->SetOnOff(false,level.time);
		}
		else if (nSkin == HC_SKIN_GUNNER &&
			GetHeliBody()->GetGunnerInst())
		{
			GetHeliBody()->GetGunnerInst()->SetOnOff(false,level.time);
		}
		else if (nSkin == HC_SKIN_BOTH &&
			GetHeliBody()->GetGunnerInst() &&
			GetHeliBody()->GetPilotInst())
		{
			GetHeliBody()->GetGunnerInst()->SetOnOff(false,level.time);
			GetHeliBody()->GetPilotInst()->SetOnOff(false,level.time);
		}
	}
}

qboolean generic_ghoul_heli_ai::Damage (edict_t &monster, edict_t *inflictor, 
										edict_t *attacker, vec3_t dir, vec3_t point, 
										vec3_t origin, int damage, int knockback, 
										int dflags, int mod, float penetrate, float absorb)
{
	// fixme: centralize skill level stuff
	int			take;

	VectorNormalize(dir);

	take = damage;

	if (mod == MOD_C4)
	{
		return false;
	}
	if (body && take)
	{
		if(dflags&(DT_PROJECTILE|DT_EXPLODE))
		{
			take=damage=body->ShowDamage(monster, inflictor, attacker, dir, point, origin, damage, knockback, dflags, mod, penetrate, absorb);
		}
	}

// do the damage
	if (take)
	{
		monster.health = monster.health - take;

		if (monster.health <= 0)
		{
			m_attacker = attacker;
			monster.flags |= FL_NO_KNOCKBACK;
			if (GetHeliBody())
			{
				GetHeliBody()->m_flags |= HELIFLAG_ISDEAD;
			}
			monster.takedamage = DAMAGE_NO;
			monster.nextthink = level.time + FRAMETIME;	// should break heli out of PAUSE command if in one, but only private heli pause, not script main pause (unless that breaks out when you get shot anyway)
			return true;
		}

		if(dflags&(DT_PROJECTILE|DT_EXPLODE))
		{
			Pain (monster, attacker, knockback, take);
		}
		return true;
	}

	return false;//no hit!
}

void generic_ghoul_heli_ai::Pain(edict_t &monster, edict_t *other, float kick, int damage)
{
	float	fHealthPercentage = (float)monster.health/(float)monster.max_health;

	if (GetHeliBody())
	{
		GetHeliBody()->UpdateSmoke(&monster, fHealthPercentage);
	}
}

void generic_ghoul_heli_ai::Die(edict_t &monster, edict_t *inflictor, edict_t *attacker,
								int damage, vec3_t point)
{
	vec3_t	forward,to_dam;
	edict_t	*killTarget = NULL;
	bool	bSpecialDeath = false;

	if (monster.health < -999)
	{
		monster.health = -999;
	}

	if (monster.killtarget)
	{
		bSpecialDeath = true;
		while ((killTarget = G_Find (killTarget, FOFS(targetname), ent->killtarget)))
		{
			killTarget->use(killTarget, &monster, &monster);
		}
	}

	monster.enemy = attacker;//for awarding credit?
	// CHECKME possible issues:
	// If monster is killed by barrel, should we chain to find who was the barrel's killer?
	// what about being clever and using architecture to kill monsters (like 16 ton weights)

	if (monster.deadflag != DEAD_DEAD)
	{
		vec3_t					facedir;
		list<action_c_ptr>::iterator	ia;
		list<sense_c_ptr>::iterator		is;
		list<decision_c_ptr>::iterator	id;


/*	if (attacker && game.GameStats->CanBuy(attacker))
		{
			//changed this to be conditional so it wouldn't crash --ss
			if(CMonsterStats	*MonsterStats = game.GameStats->GetMonsterStats(&monster))
			{
				GetPlayerStats(attacker)->AdjustCashToBeAwarded(MonsterStats->GetKilledValue());
			}
			else
			{
				gi.dprintf("Error: no stats for monster!\n");
			}
		// this stuff gets put into bank account at "end" of mission
		}
*/
//fixme: do need the npc check, but do it a good way
//		if (!(monster.monsterinfo.aiflags & AI_GOOD_GUY))
//		{
			level.killed_monsters++;
/*			if (coop->value && attacker && attacker->client)
			{
				attacker->client->resp.score++;
			}*/
//		}

		// huh? should have list-eating separated out?

		for (is=senses.begin();is != senses.end();is++)
		{
			(*is).Destroy();
		}
		for (ia=actions.begin();ia!=actions.end();ia++)
		{
			(*ia).Destroy();
		}
		for (id=decisions.begin();id!=decisions.end();id++)
		{
			(*id).Destroy();
		}

		//now ditch nodes

		while (senses.size())
		{
			senses.pop_front();
		}

		while (actions.size())
		{
			actions.pop_front();
		}
		while (decisions.size())
		{
			decisions.pop_front();
		}

		if (current_action)
		{
			current_action.Destroy();
		}

		if (recycle_action)
		{
			recycle_action.Destroy();
		}

		AngleVectors(monster.s.angles, forward, NULL, NULL);
		VectorSubtract(point, monster.s.origin, to_dam);
		VectorNormalize(to_dam);

		monster.touch = NULL;//do we need this here?
		monster.deadflag = DEAD_DEAD;

		VectorSubtract(point,monster.s.origin,facedir);
		VectorCopy(monster.s.angles, ideal_angles);
		gi.linkentity (&monster);
	}

	if (bSpecialDeath && (strcmp("musicscript", monster.killtarget)))
	{	// this means we had a kill target (like, say, in jpn3) and fired a script or something,
		//so don't do the regular explosion
	}
	else
	{
		monster.s.effects |= EF_EXPLODING;
		//FX_VehicleExplosion(monster.s.origin, 255);
		fxRunner.exec("heli_expl", monster.s.origin);
		//BlindingLight(monster.s.origin, 5000, 0.9, 0.5);
	}
}

heliWaypoint* generic_ghoul_heli_ai::GetWaypointPt(int nWay)
{
	if (nWay < 0 || nWay > m_nNumWaypointPts || 0 == m_nNumWaypoints)
	{
		return NULL;
	}
	int nPts = 0;
	heliWaypoint* temp = m_waypoints;
	while (temp)
	{
		if (temp->IsPoint())
		{
			if (nPts == nWay)
			{
				return temp;
			}
			nPts++;
		}
		temp = temp->m_next;
	}
	return NULL;
}

heliWaypoint* generic_ghoul_heli_ai::GetWaypointVol(int nWay)
{
	if (nWay < 0 || nWay > m_nNumWaypointVols || 0 == m_nNumWaypoints)
	{
		return NULL;
	}
	int nVols = 0;
	heliWaypoint* temp = m_waypoints;
	while (temp)
	{
		// if the heliWaypoint is, in fact, a volume and not just a point
		if (!temp->IsPoint())
		{
			if (nVols == nWay)
			{
				return temp;
			}
			nVols++;
		}
		temp = temp->m_next;
	}
	return NULL;
}

int generic_ghoul_heli_ai::AddWaypoint(vec3_t v)
{
	heliWaypoint* newPt = new heliWaypoint(v), *temp = m_waypoints;
	if (0 == m_nNumWaypoints)
	{
		m_waypoints = newPt;
	}
	else
	{
		while (temp->m_next)
		{
			temp = temp->m_next;
		}
		temp->m_next = newPt;
	}
	m_nNumWaypointPts++;
	return ++m_nNumWaypoints;
}

qboolean generic_ghoul_heli_ai::SetWaypointPtVolume(int nWay, vec3_t v)
{
	heliWaypoint* pt = GetWaypointPt(nWay);
	vec3_t vClipMins, vClipMaxs;

	if (pt && ent)
	{
		if (pt->IsPoint())
		{
			// turning a point into a volume
			m_nNumWaypointPts--;
		}
		// perform any necessary clipping to reduce volume to a space
		//in which we can place our body's origin, not just our bbox
		for (int i = 0; i < 3; i++)
		{
			if (v[i]*2 >= (ent->maxs[i] - ent->mins[i]))
			{
				vClipMins[i] = -v[i] - ent->mins[i];
				vClipMaxs[i] = v[i] - ent->maxs[i];
			}
			else 
			{
				return false;
			}
		}
		m_nNumWaypointVols++;
		VectorAdd(pt->m_v, vClipMins, vClipMins); 
		VectorAdd(pt->m_v, vClipMaxs, vClipMaxs); 
		pt->SetVolume(vClipMins, vClipMaxs);
		return true;
	}
	else
	{
		return false;
	}
}

int generic_ghoul_heli_ai::GetCurrentActionID()
{
	if (current_action)
	{
		return ((heli_action*)(action_c*)current_action)->GetID();
	}
	else
	{
		return 0;
	}
}

int generic_ghoul_heli_ai::GetMostRecentlyAddedActionID()
{
	return m_ScriptActionCounter;
}



////////////////////////////////////////////////////////////////////////
//	begin: stuff copied directly from m_heli.cpp
////////////////////////////////////////////////////////////////////////
/*
#define HELIFLAG_DOINGSCRIPTEDLANDING	0x00000001	// #
#define HELIFLAG_ROTORSCHANGINGSPEED	0x00000002
#define HELIFLAG_SIDESTRAFEISTOLEFT		0x00000004
#define HELIFLAG_ROCKETSENABLED			0x00000008
#define HELIFLAG_CHAINGUNENABLED		0x00000010
#define HELIFLAG_MGUN_GHOULSEQ_RUNNING	0x00000020	// special use thingy
#define HELIFLAG_AUTOFIRE				0x00000040
#define HELIFLAG_DOINGSCRIPTEDHOVER		0x00000080	// #
#define HELIFLAG_HOVERISAGGRESIVE		0x00000100
#define HELIFLAG_DOINGSCRIPTEDFIREAT	0x00000200	// #
*/

void Cobra2Think_TakeOff(edict_t *self)
{
	ggBinstC* boltMain=NULL, *boltTail=NULL;
	ggObjC* objMain=NULL, *objTail=NULL;
	IGhoulInst* instMainRotor = NULL, *instTailRotor = NULL;
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	// were we waiting for rotors to windup to speed?
	//
	if (self->spawnflags & IN_AIR)
	{
#ifdef _DEBUG
#ifdef SHOW_DESIGNER_ERRORS	
		gi.dprintf("### HELISCRIPT: Can't takeoff when spawned in air!\n");
#endif
#endif
		ai->ActionIsDone(true);
		return;
	}
	instMainRotor = body->GetMainRotorInst();
	instTailRotor = body->GetTailRotorInst();
	if (body->m_flags & HELIFLAG_ROTORSCHANGINGSPEED)
	{
		if (!body->GetRotorEOS())
			return;
		
		// rotors at full speed, switch to normal looping anim...
		//
		//if (objMain && boltMain && objTail && boltTail)
		if (instMainRotor && instTailRotor)
		{
			SimpleModelSetSequence2(instMainRotor, "rotor_anim", SMSEQ_LOOP);
			SimpleModelSetSequence2(instTailRotor, "rotor_anim", SMSEQ_LOOP);
		}
		body->m_flags&= ~HELIFLAG_ROTORSCHANGINGSPEED;		
	}
	else
	{
		// time to start rotor windup...
		//
		if (instMainRotor && instTailRotor)
		{
			SimpleModelSetSequence2(instMainRotor, "rotor_start", SMSEQ_HOLD);
			SimpleModelSetSequence2(instTailRotor, "rotor_start", SMSEQ_HOLD);
			body->SetRotorEOS(false);
			body->m_flags |= HELIFLAG_ROTORSCHANGINGSPEED;
		}
		return;
	}
		
	// set dest as being a pos above us...
	//	
	vec3_t vDest;
	VectorCopy(self->s.origin,vDest);
	vDest[2] += 56*4;	// climb to 4 man-heights above pad initially
	Cobra2Think_SetDumbFlyDest(self,	vDest);
}// void Cobra2Think_TakeOff(edict_t *self)

void Cobra2_InitTurnStuff(edict_t *self, vec3_t v3)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	body->SetPosStop(v3);
	body->SetStartYaw(self->s.angles[HELI_YAW]);
}

void Cobra2Think_HeadToDest(edict_t *self)
{
	// interesting bugette here, if I set the pilot's head to look at the dest target every cycle then when 
	//	we get to the end of a move he suddenly tries to look backwards over his right shoulder because he himself
	//	is beyond the dest point (I think that's why, anyway), so to compensate we should project a vector further 
	//	along (past the end point) for him to look at...(the vector being at least the size of the heli)
	//
	// note that the value worked out is constant, ie could be determined at move start rather than every cycle, but
	//	apparently field-space within entities is at a premium so trade it off for CPU work instead...
	//
	vec3_t v3Gradient;
	vec3_t v3HeliDims;
	vec3_t v3LookDest;
	float fHeliLength;

	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	Vector_GetHalfWayPoint(body->m_v3HeliDesiredPos_Start, body->m_v3HeliDesiredPos_Stop, body->m_v3HeliDesiredPos_Halfway);
	VectorSubtract (body->m_v3HeliDesiredPos_Stop,body->m_v3HeliDesiredPos_Start,v3Gradient);
	VectorNormalize(v3Gradient);
	VectorSubtract (self->maxs, self->mins, v3HeliDims);
	fHeliLength = VectorLength(v3HeliDims);
	VectorMA(body->m_v3HeliDesiredPos_Stop, fHeliLength, v3Gradient, v3LookDest);
	Cobra2AimPilotAtPos(self, v3LookDest);	
	// aim the gunner/gun as well, but only if we're going to fire at something on arrival... :-)
	//
	if (body->m_flags & HELIFLAG_DOINGSCRIPTEDFIREAT)
		Cobra2AimGunAtPos(self, body->m_v3HeliFireAtCoords, true);	// qboolean bSynchGunner
	else
	{
		if ( (body->m_flags & HELIFLAG_AUTOFIRE) && (body->m_flags & HELIFLAG_CHAINGUNENABLED) )	// do not OR these 2 into one "(flag & val)" check
		{
			// keith's seek-to-45-degree-pitch code...
			//
			vec3_t	vF;

			AngleVectors(self->s.angles, vF, NULL, NULL);
			vF[2]=0;
			VectorNormalize(vF);
			vec3_t v3;
			VectorSet(v3,0,0,-1);
			VectorAdd(vF,v3,v3);
			Cobra2AimGunInDirection(self, v3, true); // qboolean bSynchGunner
		}
		else
			Cobra2ResetGunner(self);
	}

	// Helicopters can (and do) head towards things then start turning to face them once they're on the way, so
	//	head in that dir regardless, do the turn-to-face stuff as you go...
	//	

	// have we reached our dest?...
	//
	if (fabs (self->s.origin[0] - body->m_v3HeliDesiredPos_Stop[0]) <= body->m_fHeliMoveSpeed &&
		fabs (self->s.origin[1] - body->m_v3HeliDesiredPos_Stop[1]) <= body->m_fHeliMoveSpeed &&
		fabs (self->s.origin[2] - body->m_v3HeliDesiredPos_Stop[2]) <= body->m_fHeliMoveSpeed
		)
	{
		// we're reached it (or close enough), so do final position adjustement to make it exact...
		//
		VectorCopy(body->m_v3HeliDesiredPos_Stop,self->s.origin);
		body->m_fHeliMoveSpeed	= 0;		

		// now detonate if this is the right time...
		//
		if (body->m_flags & HELIFLAG_ISDEADFINALGOTO)
		{
			ai->SetCurrentActionNextThink(Cobra2_Explode);
			return;
		}

		ai->SetCurrentActionNextThink(Cobra2Think_MovementFinish);
	}
	else
	{// not reached dest...

		//
		// acceleration/pitch logic here...
		//
		qboolean qbBeyondHalfWay = 
			WithinOrBeyondVectors2(self->s.origin, body->m_v3HeliDesiredPos_Halfway,
									body->m_v3HeliDesiredPos_Stop);
		if (!qbBeyondHalfWay || (body->m_flags & HELIFLAG_ISDEADFINALGOTO))	// keep accelerating during death plunge
		{
			// accel...
			//
			body->m_fHeliMoveSpeed += HELI_COBRA_ACCEL;

			// pitch. First, how far are we along the first half of the journey as a %age?...
			//
			vec3_t v3TotLengthVect,v3LengthVect;
			VectorSubtract(body->m_v3HeliDesiredPos_Halfway, body->m_v3HeliDesiredPos_Start,	v3TotLengthVect);	// journey dist
			VectorSubtract(body->m_v3HeliDesiredPos_Halfway, self->s.origin,			v3LengthVect);		// dist so far
			v3TotLengthVect[2] = v3LengthVect[2] = 0;	// lose the Z component

			// get the inverse percentage...
			// (broken into if-checked lines because of sqrt() returning float-errors occasionally when zeros occur)
			float fTotLen = VectorLength(v3TotLengthVect);
			if (fTotLen>0)
			{
				float fSoFarLen = VectorLength(v3LengthVect);
				if (fSoFarLen>0)
				{
					float fDistPercent = 100-((100/fTotLen) * fSoFarLen);	// 100%= start, 0 = reached halfway (end of accel)
					float fIdealPitch = -(HELI_COBRA_MAXPITCH-((fDistPercent * HELI_COBRA_MAXPITCH)/100));	// HELI_COBRA_MAXPITCH..0
					if (qbBeyondHalfWay)
						fIdealPitch = 0;
					fIdealPitch *= -1; // kef

					// now just adjust our pitch to suit...
					//
					if (self->s.angles[HELI_PITCH] + HELI_COBRA_PITCHADJSPEED <= fIdealPitch) // kef
						self->s.angles[HELI_PITCH] +=HELI_COBRA_PITCHADJSPEED;
					else
					if (self->s.angles[HELI_PITCH] - HELI_COBRA_PITCHADJSPEED >= fIdealPitch) // kef
						self->s.angles[HELI_PITCH] -=HELI_COBRA_PITCHADJSPEED;
				}
			}
		}
		else
		{
			// decel...
			//
			body->m_fHeliMoveSpeed -= HELI_COBRA_ACCEL;
			if (body->m_fHeliMoveSpeed < HELI_COBRA_MIN_HEAD2DESTSPEED)	// arb, should be z, but this ensure we keep moving if we fall a little short
				body->m_fHeliMoveSpeed = HELI_COBRA_MIN_HEAD2DESTSPEED;

			// pitch. First, how far are we along the 2nd half of the journey as a %age?...
			//
			vec3_t v3TotLengthVect,v3LengthVect;
			VectorSubtract(body->m_v3HeliDesiredPos_Stop, body->m_v3HeliDesiredPos_Halfway,	v3TotLengthVect);	// journey dist
			VectorSubtract(body->m_v3HeliDesiredPos_Stop, self->s.origin,			v3LengthVect);		// dist so far
			v3TotLengthVect[2] = v3LengthVect[2] = 0;	// lose the Z component

			// get the percentage... 
			// (broken into if-checked lines because of sqrt() returning float-errors occasionally when zeros occur)
			float fTotLen = VectorLength(v3TotLengthVect);
			if (fTotLen>0)
			{
				float fSoFarLen = VectorLength(v3LengthVect);
				if (fSoFarLen>0)
				{
					float fDistPercent = (100/fTotLen) * fSoFarLen;
					float fIdealPitch = HELI_COBRA_MAXPITCH_DECEL-((fDistPercent * HELI_COBRA_MAXPITCH_DECEL)/100);	// 0..HELI_COBRA_MAXPITCH_DECEL
					if (body->m_fHeliMoveSpeed < 4)
					{
						fIdealPitch = 0;
					}
					fIdealPitch *= -1; // kef

					// now just adjust our pitch to suit...
					//
					self->s.angles[HELI_PITCH] = angle2posneg (self->s.angles[HELI_PITCH]);
					if (self->s.angles[HELI_PITCH] + HELI_COBRA_PITCHADJSPEED_DECEL <= fIdealPitch) // kef
						self->s.angles[HELI_PITCH] +=HELI_COBRA_PITCHADJSPEED_DECEL;
					else
					if (self->s.angles[HELI_PITCH] - HELI_COBRA_PITCHADJSPEED_DECEL >= fIdealPitch) // kef
						self->s.angles[HELI_PITCH] -=HELI_COBRA_PITCHADJSPEED_DECEL;
				}
			}
		}

		
		// actual move...
		//
		vec3_t v;

		VectorSubtract(body->m_v3HeliDesiredPos_Stop,self->s.origin,v);
		VectorNormalize(v);
		VectorMA(self->s.origin, (body->m_fHeliMoveSpeed<HELI_COBRA_MAXSPEED || (body->m_flags & HELIFLAG_ISDEADFINALGOTO))?
			body->m_fHeliMoveSpeed:HELI_COBRA_MAXSPEED, v, self->s.origin);

		// swing round to face it as we move... (and record for firing decision later)
		//
		float fDegreesToGo;
		Cobra2_TurnToFace(self, body->m_v3HeliDesiredPos_Stop,
							false,			// qbRoll
							false,			// qbYAWDictatesFiring
							&fDegreesToGo	// float *pfDegreesToGo
						);

		// should we be firing?...
		//
		if (body->GetFlags() & HELIFLAG_AUTOFIRE)
		{
			// check YAW is within (say) 20 degrees either side of desired final ,
			//	and that heli is facing downwards (not up into air on decel phase)......
			//
			// (update, greg wants it firing almost all the time, so check based on speed instead)
			//
			if (fDegreesToGo<20 && //self->s.angles[HELI_PITCH]<0		
				body->m_fHeliMoveSpeed>(HELI_COBRA_MAXSPEED/10)
				)
			{
				// (-ve pitch is nose-down)
				Cobra2_Fire(self, (self->s.angles[HELI_PITCH]<=0)?			// +ve pitch is nose-up
																 true:false	// qboolean qbInihibitRockets
							);	
			}
			else
				Cobra2_Fire_OFF(self);

		}// if (dwHeliFlags & HELIFLAG_AUTOFIRE)

	}// not reached dest...
}// void Cobra2Think_HeadToDest(edict_t *self)

int Cobra2AimHeadAtPos(edict_t *self, vec3_t vTarget, IGhoulInst* head, GhoulID bolt,
					   vec3_t vOffset)
{
	generic_ghoul_heli_ai*		ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli*					body = ai->GetHeliBody();
	int							bRet = AIM_TRUE;
	boltonOrientation_c			boltonInfo;

	if (!head || !bolt)
		return AIM_ERROR;

	boltonInfo.root = self;
	boltonInfo.boltonInst = head;
	boltonInfo.boltonID = bolt;
	boltonInfo.parentInst = self->ghoulInst;
	VectorCopy(vTarget, boltonInfo.vTarget);
	boltonInfo.fMinPitch = -GUN_PITCH_UP;
	boltonInfo.fMaxPitch = GUN_PITCH_DOWN;
	boltonInfo.fMinYaw = GUN_YAW_CCW;
	boltonInfo.fMaxYaw = GUN_YAW_CW;
	boltonInfo.fMaxTurnSpeed = AIM_MAX_TURN_DELTA;
	boltonInfo.bUsePitch = false;
	boltonInfo.bUseYaw = true;
	boltonInfo.bToRoot = true;

	return boltonInfo.OrientBolton();

}

int Cobra2AimGunnerAtPos(edict_t *self, vec3_t vTarget)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	int bRet = AIM_TRUE;

	vec3_t vTargetCopy, vOffset;
	
	VectorCopy(vTarget, vTargetCopy);
	body->GetGunnerBoltOffset(vOffset);
	bRet = Cobra2AimHeadAtPos(self, vTargetCopy, body->GetGunnerInst(),
		body->GetGunnerBolt(), vOffset);
	return bRet;
}

int Cobra2AimPilotAtPos(edict_t *self, vec3_t vTarget)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	int bRet = AIM_TRUE;

	vec3_t vTargetCopy, vOffset;
	
	VectorCopy(vTarget, vTargetCopy);
	body->GetPilotBoltOffset(vOffset);
	bRet = Cobra2AimHeadAtPos(self, vTargetCopy, body->GetPilotInst(),
		body->GetPilotBolt(), vOffset);
	return bRet;
}

// set self->think to this funct when a movement such as turntoface, strafetodest, headtodest etc has finished
//	and this'll take care of ensuring pitch/roll/pilot head levelled out...
//
void Cobra2Think_MovementFinish(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	int bRet = AIM_TRUE;

	if (body->GetFlags() & HELIFLAG_AUTOFIRE)
		Cobra2_Fire_OFF(self);

	if (Cobra2MovementFinished(self))
	{
		ai->ActionIsDone(true);
	}
}// void Cobra2Think_MovementFinish(edict_t *self)


// called at the end of a firing sequence, basically just used to clean up some ghoul anim stuff...
//
void Cobra2_Fire_OFF(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	if (body->GetFlags() & HELIFLAG_CHAINGUNENABLED)	
		Cobra2FireGun_Off(self);	
}
	
// Do not try and use the helicopter after you've called this!!!!
void Cobra2_Explode(edict_t *self)
{
	// let the ai know it needs to explode and go away now
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	ai->TimeToDie(true);
	ai->ActionIsDone(true);
}

// called by above, kept seperate so it can also be called by landing code...
//
qboolean Cobra2MovementFinished(edict_t *self)
{
	// we must do these calls even if we early-ret later...
	//
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	qboolean qbRollFinished = Cobra2_EnsureRollFinished(self);
	int		 iPilotStatus	= Cobra2ResetPilot(self);
	int		 iGunnerStatus	= (	(body->m_flags & HELIFLAG_AUTOFIRE) &&		// do not OR these 2 into one "(flag & val)" check
								(body->m_flags & HELIFLAG_CHAINGUNENABLED)
								)
								?Cobra2ResetGunner(self):AIM_TRUE;
	//
	// level off before going into hover... 
	//
	if (self->s.angles[HELI_PITCH] > 180)
	{
		self->s.angles[HELI_PITCH] = 360 - self->s.angles[HELI_PITCH];
	}
	else if (self->s.angles[HELI_PITCH] < -180)
	{
		self->s.angles[HELI_PITCH] = self->s.angles[HELI_PITCH] + 360;
	}
	if ( (self->s.angles[HELI_PITCH] >= 0 && self->s.angles[HELI_PITCH] <= HELI_COBRA_PITCHADJSPEED) ||
		 (self->s.angles[HELI_PITCH] <= 0 && self->s.angles[HELI_PITCH] >= -HELI_COBRA_PITCHADJSPEED)
		)
		self->s.angles[HELI_PITCH] = 0;
	else
	{
		// pitch needs leveling...
		//			
		if (self->s.angles[HELI_PITCH] < -HELI_COBRA_PITCHADJSPEED)
		{
			self->s.angles[HELI_PITCH] += HELI_COBRA_PITCHADJSPEED; // kef
		}
		else
		{
			self->s.angles[HELI_PITCH] -= HELI_COBRA_PITCHADJSPEED; // kef
		}

		return false;
	}

	if (!qbRollFinished)// || iPilotStatus == AIM_FALSE || iGunnerStatus == AIM_FALSE)
		return false;

	// levelled off pitch and roll, and pilot yaw'd back to default...
	//
	return true;

}// qboolean Cobra2MovementFinished(edict_t *self)

// adjusts heli YAW by a certain amount, returns TRUE if now facing the desired direction...
//
qboolean Cobra2_TurnToFace(edict_t *self, vec3_t v3WorldPos, qboolean qbRoll, qboolean qbYAWDictatesFiring, float *pfDegreesToGo)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	qboolean qbTurningLeft;
	vec3_t v;

	// new bit: special check, if target dir differs only in Z then just return, saves the effect of zero-diff always causing
	//	it to face east when changing Z position only during HeadToDest...
	//
	if (self->s.origin[0] == v3WorldPos[0] && self->s.origin[1] == v3WorldPos[1])
	{
		if (pfDegreesToGo)
			*pfDegreesToGo = 0;
		return true;
	}

	// gradually alter our facing angle (YAW) to suit travel dir...
	//		
	VectorSubtract(v3WorldPos,self->s.origin,v);

	vec3_t	v3DirToFace;
	vectoangles(v,v3DirToFace);

//	v3DirToFace[HELI_YAW] -= 90;
//	v3DirToFace[HELI_YAW] = anglemod(v3DirToFace[HELI_YAW]);

	// this line is needed to avoid tail-skittering when on autofire in HeadToDest (which converts the YAW to -180..0..180)
	//
//	self->s.angles[HELI_YAW] = anglemod(self->s.angles[HELI_YAW]);
	if (self->s.angles[HELI_YAW] > 180)
	{
		self->s.angles[HELI_YAW] -= 360;
	}

	float fMove = v3DirToFace[HELI_YAW] - self->s.angles[HELI_YAW];

	if (fabs(fMove) <= HELI_YAWADJ_SPEED)		
	{
		self->s.angles[HELI_YAW] = v3DirToFace[HELI_YAW];
		if (pfDegreesToGo)
			*pfDegreesToGo = 0;
		return true;
	}
	else
	{	
		if (pfDegreesToGo)
			*pfDegreesToGo = fabs(fMove);
		
		fMove = anglemod(fMove);

		if (fMove<180)
		{
			// turn to left...
			//
			self->s.angles[HELI_YAW] += HELI_YAWADJ_SPEED;
			qbTurningLeft = true;
		}
		else
		{
			// turn to right...
			//
			self->s.angles[HELI_YAW] -= HELI_YAWADJ_SPEED;
			qbTurningLeft = false;
		}

		self->s.angles[HELI_YAW] = anglemod(self->s.angles[HELI_YAW]);		

		if (qbRoll)
		{
			// now handle ROLL...  
			// (note that this can be much simpler if accel/decel max roll angles are forced to be the same, but they're not, so...
			//
			// work out how far we are along the turn, and therefore our ideal ROLL angle...
			//
			float	fDegreesInTurn	= v3DirToFace[HELI_YAW] - body->m_fHeliTurnStartYaw;
					fDegreesInTurn	= anglemod(fDegreesInTurn);
			if (fDegreesInTurn > 180)
				fDegreesInTurn = fabs(360-fDegreesInTurn);
			float	fHalfWay		= fDegreesInTurn/2;
			float	fCurrentTurnDegs= self->s.angles[HELI_YAW] - body->m_fHeliTurnStartYaw;
					fCurrentTurnDegs= anglemod(fCurrentTurnDegs);
			if (fCurrentTurnDegs > 180)
				fCurrentTurnDegs = fabs(360-fCurrentTurnDegs);

			// how far are we from midpoint in the turn as a percentage?... (with 0% at centre, 100% at edges)
			//
			float fPercent = (100/fHalfWay) * (fabs(fCurrentTurnDegs-fHalfWay));

			// (should we be firing, by the way?)
			//
			if (qbYAWDictatesFiring && (body->m_flags & HELIFLAG_AUTOFIRE))
			{
				if (fPercent<90)
					Cobra2_Fire(self,false);		// qboolean qbInihibitRockets);	
				else
					Cobra2_Fire_OFF(self);
			}

			// so are we on the accel or decel phase?...
			//
			if (fCurrentTurnDegs<fHalfWay)
			{			
				// accel phase...
				//			
				float fIdealRoll = (fPercent * HELI_COBRA_MAXROLL)/100;	// 0..HELI_COBRA_MAXROLL		

				if (qbTurningLeft)
					fIdealRoll = -fIdealRoll;

				// make sure 0..360 angle is -180..0..180...
				//
				if (self->s.angles[HELI_ROLL]>180)
					self->s.angles[HELI_ROLL]-= 360;

				if (fabs(self->s.angles[HELI_ROLL] - fIdealRoll) <= HELI_ROLLADJ_SPEED)
					self->s.angles[HELI_ROLL] = fIdealRoll;
				else
				{
					if (self->s.angles[HELI_ROLL] < fIdealRoll)
						self->s.angles[HELI_ROLL] += HELI_ROLLADJ_SPEED;
					else
						self->s.angles[HELI_ROLL] -= HELI_ROLLADJ_SPEED;
				}
			}
			else
			{
				// decel phase...
				//
				float fIdealRoll = (fPercent * HELI_COBRA_MAXROLL_DECEL)/100;	// 0..HELI_COBRA_MAXROLL_DECEL

				if (!qbTurningLeft)
					fIdealRoll = -fIdealRoll;

				// make sure 0..360 angle is -180..0..180...
				//
				if (self->s.angles[HELI_ROLL]>180)
					self->s.angles[HELI_ROLL]-= 360;

				if (fabs(self->s.angles[HELI_ROLL] - fIdealRoll) <= HELI_ROLLADJ_SPEED)
					self->s.angles[HELI_ROLL] = fIdealRoll;
				else
				{
					if (self->s.angles[HELI_ROLL] < fIdealRoll)
						self->s.angles[HELI_ROLL] += HELI_ROLLADJ_SPEED;
					else
						self->s.angles[HELI_ROLL] -= HELI_ROLLADJ_SPEED;
				}
			}
		}// if (qbRoll)
	}

	return false;

}// qboolean Cobra2_TurnToFace(edict_t *self, vec3_t v3WorldPos, qboolean qbRoll, qboolean qbYAWDictatesFiring, float *pfDegreesToGo)

// feed this our current pos, the halfway point along our root, and the end point
//	returns FALSE if not yet halfway, else TRUE for anything else.
//
qboolean WithinOrBeyondVectors2(vec3_t v3Me, vec3_t v3Halfway, vec3_t v3End)
{
	// sigh, we need to cope with stupid rounding errors, because of when the debugger shows (eg) 452.50 & 452.50 and yet
	//	says that one is bigger than the other because of some tiny rounding errors that it doesn't display.	

	// X...
	//
	if (fabs(v3Halfway[0]-v3End[0]) > 1.0f)
	{
		if (v3Halfway[0] < v3End[0])
		{
			if (v3Me[0]<v3Halfway[0])
				return false;
		}
		else
		{
			if (v3Me[0]>v3Halfway[0])
				return false;
		}
	}

	// Y...
	//
	if (fabs(v3Halfway[1]-v3End[1]) > 1.0f)
	{
		if (v3Halfway[1] < v3End[1])
		{
			if (v3Me[1]<v3Halfway[1])
				return false;
		}
		else
		{
			if (v3Me[1]>v3Halfway[1])
				return false;
		}
	}

	// Z...
	//
	if (fabs(v3Halfway[2]-v3End[2]) > 1.0f)
	{
		if (v3Halfway[2] < v3End[2])
		{
			if (v3Me[2]<v3Halfway[2])
				return false;
		}
		else
		{
			if (v3Me[2]>v3Halfway[2])
				return false;
		}
	}

	return true;

}// qboolean WithinOrBeyondVectors(vec3_t v3Me, vec3_t v3Halfway, vec3_t v3End)

void Cobra2FireRockets(edict_t *self, int nLeftRockets, int nRightRockets)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	IGhoulInst *instHeli = NULL;
	GhoulID idBoltLeftRocketPod, idBoltRightRocketPod;
	IGhoulObj		*obj = NULL;
	//vec3_t vWaver;
	vec3_t	vTemp, vF, vR, vU, vFirePoint, vFireDir;
	int i = 0;
	
	instHeli = self->ghoulInst;
	if (NULL == instHeli)
	{
		return;
	}

	obj = instHeli->GetGhoulObject();
	if(NULL == obj)
	{
		return;
	}

	AngleVectors(self->s.angles, vF, vR, vU);
	VectorScale(vF, 5.0, vF);
	VectorScale(vR, 5.0, vR);
	VectorScale(vU, 5.0, vU);

	// firing rocket from left pod
	if (nLeftRockets > 0)
	{
		// get id of bolt on the front of left rocket pod
		idBoltLeftRocketPod = obj->FindPart("FLASH_ROCKET1");


		// actually attack our current target

		GetGhoulPosDir2(self->s.origin, self->s.angles,
			instHeli,
			idBoltLeftRocketPod,
			NULL, vTemp, vFireDir, NULL, NULL);
		{
			switch(nLeftRockets)
			{
			case 1:
				VectorAdd(vTemp, vR, vFirePoint);
				VectorAdd(vFirePoint, vU, vFirePoint);
				break;
			case 2:
				VectorAdd(vTemp, vR, vFirePoint);
				VectorSubtract(vFirePoint, vU, vFirePoint);
				break;
			case 3:
				VectorSubtract(vTemp, vR, vFirePoint);
				VectorSubtract(vFirePoint, vU, vFirePoint);
				break;
			case 4:
				VectorSubtract(vTemp, vR, vFirePoint);
				VectorAdd(vFirePoint, vU, vFirePoint);
				break;
			}

			// launch the rocket

			// instead of pitching the heli, we'll just fire the rocket at the target
			//regardless of z difference
			edict_t *targ = body->m_AITarget?body->m_AITarget:Cobra2_GetEntToTarget(self);
			VectorSubtract(targ->s.origin, self->s.origin, vFireDir);
			VectorNormalize(vFireDir);
			fireHeliRocket2(self, vFirePoint, vFireDir);
		}
	}
	// firing rocket from right pod
	if (nRightRockets > 0)
	{
		// get id of bolt on the front of right rocket pod
		idBoltRightRocketPod = obj->FindPart("FLASH_ROCKET2");

		// actually attack our current target
		GetGhoulPosDir2(self->s.origin, self->s.angles,
			instHeli,
			idBoltRightRocketPod,
			NULL, vTemp, vFireDir, NULL, NULL);
		{
			switch(nRightRockets)
			{
			case 1:
				VectorAdd(vTemp, vR, vFirePoint);
				VectorAdd(vFirePoint, vU, vFirePoint);
				break;
			case 2:
				VectorAdd(vTemp, vR, vFirePoint);
				VectorSubtract(vFirePoint, vU, vFirePoint);
				break;
			case 3:
				VectorSubtract(vTemp, vR, vFirePoint);
				VectorSubtract(vFirePoint, vU, vFirePoint);
				break;
			case 4:
				VectorSubtract(vTemp, vR, vFirePoint);
				VectorAdd(vFirePoint, vU, vFirePoint);
				break;
			}

			// launch the rocket

			// instead of pitching the heli, we'll just fire the rocket at the target
			//regardless of z difference
			edict_t *targ = body->m_AITarget?body->m_AITarget:Cobra2_GetEntToTarget(self);
			VectorSubtract(targ->s.origin, self->s.origin, vFireDir);
			VectorNormalize(vFireDir);
			fireHeliRocket2(self, vFirePoint, vFireDir);
		}
	}
}

void Cobra2FireGun(edict_t *self, qboolean bSporadic)
{
	IGhoulInst *instHeli = NULL, *instMuzzle = NULL;
	GhoulID idBoltHeliToMuzzle, idEndOfMuzzle;
	IGhoulObj		*obj = NULL;
	GhoulID seq = 0;
	vec3_t	vFirePoint, vFireDir, vFireDir2;
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	int i = 0, nDifficulty =HELI_DIFFICULTY;

	if (body->m_nRandGunWait || !bSporadic)
	{
		if (body->m_bGunFiring || !bSporadic)
		{
			instHeli = self->ghoulInst;
			if (NULL == instHeli)
			{
				return;
			}
			instMuzzle = body->GetBarrelInst();
			if (NULL == instMuzzle)
			{
				return;
			}
			if (!(body->m_flags & HELIFLAG_MGUN_GHOULSEQ_RUNNING))
			{
				body->m_flags |= HELIFLAG_MGUN_GHOULSEQ_RUNNING;

				// play the barrel sequence with attached flickering light
				if (body->GetBarrelInst())
				{
					SimpleModelSetSequence2(body->GetBarrelInst(), "barrel", SMSEQ_LOOP);
				}
			}
			// get id of bolt that connects the gun barrel to the heli
			obj = instHeli->GetGhoulObject();
			if(NULL == obj)
			{
				return;
			}
			idBoltHeliToMuzzle = obj->FindPart("barrel_base");

			// get id of the muzzle of the gun barrel
			obj = instMuzzle->GetGhoulObject();
			if(NULL == obj)
			{
				return;
			}
			idEndOfMuzzle = obj->FindPart("flash_barrel");

			// instantaneous muzzle flash
//			FX_HeliGun(self, instHeli, instMuzzle, idBoltHeliToMuzzle, idEndOfMuzzle);
			fxRunner.execWithInst("weapons/othermz/heli", self, instMuzzle, idEndOfMuzzle);

			// actually attack our current target
			vec3_t vWaver;

			GetGhoulChildPosDir2(self->s.origin, self->s.angles,
				instHeli,
				idBoltHeliToMuzzle,
				instMuzzle, 
				idEndOfMuzzle,
				NULL, NULL, vFirePoint, vFireDir, NULL, NULL);	// this is way too long ;(
			VectorScale(vFireDir, 100, vFireDir);
//			for (i = 0; i < nDifficulty; i++)
			if (body->m_fArg != 1337)
			{	// introduce some waver to our accuracy
				VectorSet(vWaver, gi.irand(-10,10), gi.irand(-10,10), gi.irand(-10,10));
				VectorAdd(vFireDir, vWaver, vFireDir2);
			}
			else
			{	// the heck with waver, we want to hit a particular molecule
				VectorCopy(vFireDir, vFireDir2);
			}
			weapons.attack(ATK_HELIGUN, self, vFirePoint, vFireDir2);//same damage, but bigger impact sound
			gi.sound (self, CHAN_WEAPON, gi.soundindex("weapons/assault/fire.wav"), .9, .5, 0);
		}
	}
	else
	{
		body->m_nRandGunWait = gi.irand(2, 10);
		body->m_bGunFiring = !body->m_bGunFiring;
	}
	body->m_nRandGunWait--;

}

// stop the ghoul mgun barrel sequence when I stop firing...
//
void Cobra2FireGun_Off(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	if (body->m_flags & HELIFLAG_MGUN_GHOULSEQ_RUNNING)
	{	
		body->m_flags &= ~HELIFLAG_MGUN_GHOULSEQ_RUNNING;

		if (body->GetBarrelInst())
		{
			SimpleModelSetSequence2(body->GetBarrelInst(), "barrel_still", SMSEQ_HOLD);
		}
	}
}

// returns TRUE when ROLL has leveled off ok, called by both the turn and move code
//
qboolean Cobra2_EnsureRollFinished(edict_t *self)
{
	#define HELI_COBRA_ROLLADJSPEED_LEVELOUT 4

	self->s.angles[HELI_ROLL] = anglemod(self->s.angles[HELI_ROLL]);

	if (	(self->s.angles[HELI_ROLL] == 0) ||
			(self->s.angles[HELI_ROLL] + HELI_COBRA_ROLLADJSPEED_LEVELOUT > 360) ||
			(self->s.angles[HELI_ROLL] - HELI_COBRA_ROLLADJSPEED_LEVELOUT < 0)
		)
	{
		self->s.angles[HELI_ROLL] = 0;
		return true;
	}

	if (self->s.angles[HELI_ROLL] > 180)
		self->s.angles[HELI_ROLL] += HELI_COBRA_ROLLADJSPEED_LEVELOUT;
	else
		self->s.angles[HELI_ROLL] -= HELI_COBRA_ROLLADJSPEED_LEVELOUT;

	return false;
}

int Cobra2ResetHead(edict_t *self, IGhoulInst* head, GhoulID bolt, vec3_t vOffset)
{
	vec3_t vF, vR, vPos;

	AngleVectors(self->s.angles, vF, vR, NULL);
	VectorMA(self->s.origin, 100, vF, vPos);
	return Cobra2AimHeadAtPos(self, vPos, head, bolt, vOffset);
}

int Cobra2ResetPilot(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	vec3_t vOffset;

	body->GetPilotBoltOffset(vOffset);
	return Cobra2ResetHead(self, body->GetPilotInst(), body->GetPilotBolt(), vOffset);
}


int GetGhoulChildPosDir2(vec3_t sourcePos, vec3_t sourceAng, IGhoulInst *inst, GhoulID partID,
						  IGhoulInst *childInst, GhoulID childPartID, char *name, char *name2, vec3_t pos, vec3_t dir, vec3_t right, vec3_t up)
{
	IGhoulObj *obj = inst->GetGhoulObject();
	IGhoulObj *obj2 = childInst->GetGhoulObject();

	if(obj && obj2)
	{
		GhoulID part = (partID) ? partID:obj->FindPart(name);
		GhoulID part2 = (childPartID) ? childPartID:obj2->FindPart(name2);

		if(part && part2)
		{
			Matrix4 EntityToWorld;
			EntToWorldMatrix(sourcePos,sourceAng,EntityToWorld);

			Matrix4 childToEntity;
			childInst->GetBoltMatrix(level.time, childToEntity, part2, IGhoulInst::MatrixType::Entity);

			Matrix4 BoltToWorld;
			BoltToWorld.Concat(childToEntity, EntityToWorld);
			

			Vect3 ChunkLoc;
			BoltToWorld.GetRow(3,ChunkLoc);  //inital pos of chunk, will be unchanged.
			// this works too BoltToWorld.XFormPoint(ChunkLoc,Vect3(0.0f,0.0f,0.0f));

			VectorCopy((float *)&ChunkLoc, pos);

			if (dir)
			{
				Vect3 zaxisV;
				BoltToWorld.GetRow(2,zaxisV);
				VectorCopy((float *)&zaxisV, dir);
			}

			if(right)
			{
				Vect3 yaxisV;
				BoltToWorld.GetRow(1,yaxisV);
				VectorCopy((float *)&yaxisV, right);
			}
			if(up)
			{
				Vect3 xaxisV;
				BoltToWorld.GetRow(0,xaxisV);
				VectorCopy((float *)&xaxisV, up);
			}
			return 1;
		}
	}
	VectorClear(pos);
	VectorClear(dir);
	return 0;
}

int GetGhoulPosDir2(vec3_t sourcePos, vec3_t sourceAng, IGhoulInst *inst,
					   GhoulID partID, char *name, vec3_t pos, vec3_t dir, vec3_t right, vec3_t up)
{
	IGhoulObj *obj = inst->GetGhoulObject();

	if(obj)
	{
		GhoulID part = (partID) ? partID:obj->FindPart(name);

		if(part)
		{
			Matrix4 EntityToWorld;
			EntToWorldMatrix(sourcePos,sourceAng,EntityToWorld);

			Matrix4 BoltToWorld;
			Matrix4 BoltToEntity;

			inst->GetBoltMatrix(level.time, BoltToEntity, part, IGhoulInst::MatrixType::Entity);
			BoltToWorld.Concat(BoltToEntity,EntityToWorld);

			Vect3 ChunkLoc;
			BoltToWorld.GetRow(3,ChunkLoc);

			VectorCopy((float *)&ChunkLoc, pos);

			Vect3 zaxisV;
			BoltToWorld.GetRow(2,zaxisV);
			VectorCopy((float *)&zaxisV, dir);

			if(right)
			{
				Vect3 yaxisV;
				BoltToWorld.GetRow(1,yaxisV);
				VectorCopy((float *)&yaxisV, right);
			}
			if(up)
			{
				Vect3 xaxisV;
				BoltToWorld.GetRow(0,xaxisV);
				VectorCopy((float *)&xaxisV, up);
			}
			return 1;
		}
	}
	VectorClear(pos);
	VectorClear(dir);
	return 0;
}

qboolean Cobra2TargetWithinArc(vec3_t vTargetPos, vec3_t vHeliAngles, vec3_t vHeliPos)
{
	// check the angle between heli's forward vector and the
	//target direction vector

// default is a 160deg firing arc (80deg to either side of forward vector)
	float fArcCos = 0.173f; // cos(80degrees)

	float fDot = fArcCos;
	vec3_t vForward;
	vec3_t vTargetDir;
	int i = 0;

	// make sure angles are in the range [0, 360)
	for (i=0;i<3;i++)
	{
		vHeliAngles[i]=anglemod(vHeliAngles[i]);
	}

	VectorSubtract(vTargetPos, vHeliPos, vTargetDir);
	VectorNormalize(vTargetDir);
	AngleVectors(vHeliAngles, vForward, NULL, NULL);
	fDot = DotProduct(vForward, vTargetDir);
	if (fDot >= fArcCos && fDot <= 1.0f)
	{
		// current target is within our firing arc
		return true;
	}
	return false;
}
	
// we already know when this is called that the target is valid, ie a live player or merc...
//
qboolean Cobra2HasTargetLOS(edict_t *self, edict_t *target, qboolean qbAccountForArc)
{
	trace_t tr;

	vec3_t	v3TargetEyeLevel;
	VectorCopy(target->s.origin, v3TargetEyeLevel);
	v3TargetEyeLevel[2] += target->viewheight;	

	gi.trace(self->s.origin, NULL, NULL, v3TargetEyeLevel, self, MASK_OPAQUE, &tr);
	if (tr.fraction == 1.0 && (!qbAccountForArc || Cobra2TargetWithinArc(tr.endpos, self->s.angles, self->s.origin)))
		return true;
	
	return false;
}

int Cobra2AimGunAtPos(edict_t *self, vec3_t vTarget, qboolean bSynchGunner)
{
	generic_ghoul_heli_ai*		ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli*					body = ai->GetHeliBody();
	int							bRet = AIM_TRUE;
	IGhoulInst*					barrelInst = body->GetBarrelInst();
	GhoulID						barrelBolt = body->GetBarrelBolt();
	boltonOrientation_c			boltonInfo;

	if (!barrelInst || !barrelBolt)
		return AIM_ERROR;
	if (bSynchGunner)
	{
		Cobra2AimGunnerAtPos(self, vTarget);
	}

	// first, yaw the NULL to which the barrel is attached
	boltonInfo.root = self;
	boltonInfo.boltonInst = body->GetBarrelNULLInst();
	boltonInfo.boltonID = body->GetBarrelNULLBolt();
	boltonInfo.parentInst = self->ghoulInst;
	VectorCopy(vTarget, boltonInfo.vTarget);
	boltonInfo.fMinPitch = 0;
	boltonInfo.fMaxPitch = 0;
	boltonInfo.fMinYaw = GUN_YAW_CCW;
	boltonInfo.fMaxYaw = GUN_YAW_CW;
	boltonInfo.fMaxTurnSpeed = AIM_MAX_TURN_DELTA;
	boltonInfo.bUsePitch = false;
	boltonInfo.bUseYaw = true;
	boltonInfo.bToRoot = true;
	boltonInfo.OrientBolton();

	// now pitch the barrel itself
	boltonInfo.parentInst = boltonInfo.boltonInst;
	boltonInfo.parentID = boltonInfo.boltonID;
	boltonInfo.boltonInst = body->GetBarrelInst();
	boltonInfo.boltonID = body->GetBarrelBolt();
	boltonInfo.fMinPitch = -GUN_PITCH_UP;
	boltonInfo.fMaxPitch = GUN_PITCH_DOWN;
	boltonInfo.fMinYaw = 0;
	boltonInfo.fMaxYaw = 0;
	boltonInfo.fMaxTurnSpeed = AIM_MAX_TURN_DELTA;
	boltonInfo.bUsePitch = true;
	boltonInfo.bUseYaw = false;
	boltonInfo.bToRoot = true;

	return boltonInfo.OrientBolton();
}

int Cobra2AimGunInDirection(edict_t *self, vec3_t vTargetDir, qboolean bSynchGunner)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	int bRet = AIM_TRUE;
	IGhoulInst* barrelInst = body->GetBarrelInst();
	GhoulID barrelBolt = body->GetBarrelBolt();
	boltonOrientation_c			boltonInfo;
	vec3_t						vPos, vF;

	if (!barrelInst || !barrelBolt)
		return AIM_ERROR;
	if (bSynchGunner)
	{
		GetGhoulPosDir2(self->s.origin, self->s.angles, body->GetGunnerInst(), 0, "to_body", vPos, vF, NULL, NULL);
		VectorMA(vPos, 1000, vTargetDir, vPos);
		Cobra2AimGunnerAtPos(self, vPos);
	}

	GetGhoulPosDir2(self->s.origin, self->s.angles, body->GetBarrelNULLInst(), 0, "DUMMY01", vPos, vF, NULL, NULL);
	VectorMA(vPos, 1000, vTargetDir, vPos);
	return Cobra2AimGunAtPos(self, vPos, bSynchGunner);
}

void HeliMissileRemove2(edict_t *self)
{
	G_FreeEdict(self);
}

void HeliMissileCollide2(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if(surf)
	{
		if(surf->flags & SURF_SKY)
		{
			G_FreeEdict(self);
			return;
		}
	}
	T_RadiusDamage (self, self->owner, 130, self, 150, 0);
	gmonster.RadiusDeafen(self, 150, 250);
	fxRunner.exec("weapons/world/rocketexplode", self->s.origin);
	ShakeCameras (self->s.origin, 100, 300, DEFAULT_JITTER_DELTA);
	G_FreeEdict(self);
}

void fireHeliRocket2(edict_t *self, vec3_t v3FirePoint, vec3_t v3FireDir)
{
	edict_t *missile;

	missile = G_Spawn();
	missile->movetype = MOVETYPE_DAN;
	missile->solid = SOLID_BBOX;
	missile->clipmask = MASK_SHOT;
	VectorCopy(v3FirePoint, missile->s.origin);
	VectorScale(v3FireDir, 600.0, missile->velocity);
	VectorSet (missile->mins, -4, -2, -4);
	VectorSet (missile->maxs, 3, 2, 4);
	missile->health = 100;
	missile->think = HeliMissileRemove2;
	missile->nextthink = level.time + 10.0;
	missile->touch = HeliMissileCollide2;
	missile->gravity = .02;
	vectoangles(v3FireDir, missile->s.angles);
	gi.linkentity(missile);
	missile->owner = self;

	game_ghoul.SetSimpleGhoulModel(missile, "Weapon/Projectiles/Rocket", "Rocket");

	fxRunner.execContinualEffect("weapons/world/rockettrail2", missile);
}

int Cobra2AimGunnerInDirection(edict_t *self, vec3_t vTargetDir)
{
	int bRet = AIM_TRUE;
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	IGhoulInst* gunnerInst = body->GetGunnerInst();
	GhoulID gunnerBolt = body->GetGunnerBolt();

	if (!gunnerInst || !gunnerBolt)
		return AIM_ERROR;

	vec3_t vTargetDirCopy, vOffset, vPos;
	
	VectorCopy(vTargetDir, vTargetDirCopy);
	body->GetGunnerBoltOffset(vOffset);
	VectorMA(self->s.origin, 10, vTargetDirCopy, vPos);
	bRet = Cobra2AimHeadAtPos(self, vPos, gunnerInst, gunnerBolt, vOffset);
	return bRet;
}

int Cobra2AimPilotInDirection(edict_t *self, vec3_t vTargetDir)
{
	int bRet = AIM_TRUE;
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	IGhoulInst* pilotInst = body->GetPilotInst();
	GhoulID pilotBolt = body->GetPilotBolt();

	if (!pilotInst || !pilotBolt)
		return AIM_ERROR;

	vec3_t vTargetDirCopy, vOffset, vPos;
	
	VectorCopy(vTargetDir, vTargetDirCopy);
	body->GetPilotBoltOffset(vOffset);
	VectorMA(self->s.origin, 10, vTargetDirCopy, vPos);
	bRet = Cobra2AimHeadAtPos(self, vPos, pilotInst, pilotBolt, vOffset);
	return bRet;
}

void Cobra2InitHead(edict_t *self, vec3_t vTargetDir, IGhoulInst* head, 
							 GhoulID bolt, vec3_t vOffset)
{
	vec3_t	vOrigin, vF, vR, vU;

	if (!head || !bolt)
	{
		return;
	}
	// the origin we use is the NULL at the base of the gunner's head, which
	//is calc'd (heli's origin + offset)
	AngleVectors(self->s.angles, vF, vR, vU);
	VectorScale(vR, vOffset[1], vR);
	VectorScale(vF, vOffset[0], vF);
	VectorScale(vU, vOffset[2], vU);
	VectorAdd(self->s.origin, vF, vOrigin);
	VectorAdd(vOrigin, vR, vOrigin);
	VectorAdd(vOrigin, vU, vOrigin);

	Matrix4 EntityToWorld;
	EntToWorldMatrix(vOrigin,self->s.angles,EntityToWorld);

	Matrix4 BoltToEntity;
	self->ghoulInst->GetBoltMatrix(level.time,BoltToEntity,bolt,IGhoulInst::MatrixType::Entity);

	Matrix4 BoltToWorld;
	BoltToWorld.Concat(BoltToEntity,EntityToWorld);

	Matrix4 WorldToBolt;
	WorldToBolt.Inverse(BoltToWorld);

	Vect3 TargetDir=*(Vect3 *)vTargetDir;

	Vect3 TargetDirInBoltSpace;
	WorldToBolt.XFormVect(TargetDirInBoltSpace,TargetDir);

	Matrix4 matR1, matR2, matNew;
	float angle1=-atan2(TargetDirInBoltSpace.y(),sqrt(TargetDirInBoltSpace.z()*TargetDirInBoltSpace.z()+TargetDirInBoltSpace.x()*TargetDirInBoltSpace.x()));
	float angle2=atan2(TargetDirInBoltSpace.z(),TargetDirInBoltSpace.x());
	float fMin1 = -HEAD_PITCH_UP, fMax1 = HEAD_PITCH_DOWN,
		fMin2 = -HEAD_YAW_CW, fMax2 = HEAD_YAW_CCW;
	float fDelta1 = 0.0f, fDelta2 = 0.0f;

	//limit the angles here to a reasonble range

	if (angle1 < fMin1 - FLOAT_ZERO_EPSILON)
	{
		angle1 = fMin1;
	}
	else if (angle1 > fMax1 + FLOAT_ZERO_EPSILON)
	{
		angle1 = fMax1;
	}
	if (angle2 < fMin2 - FLOAT_ZERO_EPSILON || (angle2 >= M_PI/2.0 && angle2 <= M_PI))
	{
		// trying to look at quadrant2 or quadrant3 (looking straight down at the head)
		angle2 = fMin2;
	}
	else if (angle2 > fMax2 + FLOAT_ZERO_EPSILON && angle2 <= M_PI/2.0)
	{
		// trying to look at quadrant1 or quadrant4 (looking straight down at the head)
		angle2 = fMax2;
	}

	matR1.Rotate(2, angle1);
	matR2.Rotate(1, angle2);
	matNew.Concat(matR1,matR2);
	head->SetXForm(matNew);
}

int Cobra2ResetGunner(edict_t *self)
{
	vec3_t vF, vR;

	AngleVectors(self->s.angles, vF, vR, NULL);
	VectorMA(self->s.origin, 500, vF, vF);
	return Cobra2AimGunAtPos(self, vF, true);
}

// called once per script fireat command...
//
void Cobra2Think_SetFireAt(edict_t *self, vec3_t v3FireDest, float fTime)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	VectorCopy(v3FireDest,body->m_v3HeliFireAtCoords);
	VectorCopy(v3FireDest,body->m_v3HeliFireAtCoords_Original);
	body->m_fHeliFireAtTimer = fTime;	// store timer for now, we're not necessarily in a position to fire
	body->m_flags |= HELIFLAG_DOINGSCRIPTEDFIREAT;
#ifdef _DEBUG
#ifdef SHOW_DESIGNER_ERRORS
	if (body->m_fHeliFireAtTimer<0 && !(body->m_flags & HELIFLAG_ROCKETSENABLED))
	{
		gi.dprintf("\n###### HELI SCRIPT ERROR:\n");
		gi.dprintf("Rocket fire wanted, but rockets disabled!\n\n");
	}
#endif		
#endif		

	// this'll look nicer, if doing fire from an aggressive hover, and rockets enabled, then change hover param
	//	so it'll do a pitch-fire, rather than Z match...
	//
	// (also done now from scripted AI, so it doesn't lose carefully set up Z points by doing a Z match for >2 rockets firing)
	//
	if ( (body->m_flags & HELIFLAG_ROCKETSENABLED) && body->m_iHeliRocketsLeft)
	{
		// we have rockets available for firing...
		//
		if (body->m_flags & (HELIFLAG_DOINGSCRIPTEDHOVER|HELIFLAG_DOINGSCRIPTEDAI))
			body->m_fHeliFireAtTimer = (gi.irand(0,10)>=5)?-2:-1;	// fire 1 or 2 rockets at player, dipping to fire, not hori-z-match
	}

	ai->SetCurrentActionNextThink(Cobra2Think_GotoFireAt);
	self->nextthink = level.time + FRAMETIME;
}

// called repeatedly until the chopper is in a position to fire at it's assigned target...
//
// (after consulting with Greg, it's been decided that the heli will still goto a suitable firing position first,
//	even if it was able to work out that it wouldn't be able to fire even before it set off (eg no rockets for a 
//	rocket attack).
//
void Cobra2Think_GotoFireAt(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	trace_t tr;

	self->nextthink	= level.time + FRAMETIME;

	// ok, there's a clear line of sight to the target, but can we bring our weapons to bear at this angle?...
	//
	// (this means checking if missiles are to be used, if so, the entire heli body needs to be brought to bear,
	//	if not, then we can just swivel the minigun round to the target)
	//
	qboolean qbWholeBodyMustMatch = ( body->m_fHeliFireAtTimer<0 || (body->m_flags & HELIFLAG_ROCKETSENABLED && body->m_iHeliRocketsLeft));

	if (qbWholeBodyMustMatch)
	{
		// YAW the heli round to face the target if nec...
		//
		float fDegreesToGo;
		Cobra2_TurnToFace(self,	body->m_v3HeliFireAtCoords,	// vec3_t v3WorldPos
								false,				// qboolean qbRoll
								false,				// qboolean qbYAWDictatesFiring
								&fDegreesToGo		// float *pfDegreesToGo
						);

		if (fDegreesToGo)
		{
			// we need to swing the heli round...
			//
			Cobra2Think_SetTurnDest(self, body->m_v3HeliFireAtCoords);
			return;
		}

		// Missile yaw ok, checking Z...


		// if we've specifically asked for either one or 2 rockets to be fired, then we'll do the pitch adjust,
		//	any other number gets done via a Z match and a long debounce-timed-stream of them...
		//
		if (body->m_fHeliFireAtTimer == -1 || body->m_fHeliFireAtTimer == -2)
		{
			// Ideally the chopper should pitch forward/back enough to fire missiles at target, but with deadlines
			//	pressing I'll do a Z match instead.  But for now, the basic algorithm is as follows...
			//
			float fYlen = fabs(self->s.origin[2] - body->m_v3HeliFireAtCoords[2]);
			vec3_t v3DiffVect;
			VectorSubtract(body->m_v3HeliFireAtCoords,self->s.origin,v3DiffVect);
			v3DiffVect[2]=0;
			float fXlen = VectorLength(v3DiffVect);
			float fFudgeFactor = 1.0;
			float fPitch = atan2(fYlen, fXlen) * RADTODEG * fFudgeFactor;
			if (body->m_v3HeliFireAtCoords[2] > self->s.origin[2])
				fPitch = -fPitch;

			if (fabs(fPitch) < HELI_MAX_FIRE_PITCH)
			{
				// we can hit this target with a pitch change...
				//
				ai->SetCurrentActionNextThink(Cobra2Think_DipFireAt);

				self->nextthink	= level.time + FRAMETIME;
				body->m_flags &= ~HELIFLAG_DOINGSCRIPTEDFIREAT;	// important!
				body->m_flags &= ~(HELIFLAG_FIREDIP1DONE | HELIFLAG_FIREDIP2DONE);
				body->m_fHeliFirePitch = fPitch*.1;   

				return;
			}
			// target beyond pitch-adjust, so do normal Z-match version...
		}

		// code to match heli Z height to ideal flat-launch pos...
		//
		if (self->s.origin[2] != body->m_v3HeliFireAtCoords[2])
		{
			// height needs adjusting...
			//
			vec3_t v3;

			VectorCopy(self->s.origin,v3);
			v3[2] = body->m_v3HeliFireAtCoords[2];

			// note, technically the heli should use its real mins/maxs box for this bit, but if I do, it might not be able
			//	to rise up again from the firing point if it was juat about level with the ground (eg if it was shooting at
			//	th player), so to avoid getting stuck when lifting up again and using the standard size collision box, I
			//	have to use the extended one all thtime, even if it means that occasionally it misses a more obvious shot.
			//
			// (this shouldn't matter too much in the final game since on either the train or tokyo level you never actually
			//	walk around on the very bottom level of the map, it's only in the choppertest map that it stops some shots)
			//
			
			if (gi.trace(self->s.origin, _HELI_TRACEPARAM_MIN2, 
									_HELI_TRACEPARAM_MAX2, v3, self,MASK_SOLID, &tr))
			{
				// it's clear, so head for it...
				//
				Cobra2Think_SetDumbFlyDest(self, v3);
				return;
			}

			// path blocked, so let's just go as far as we can and launch anyway...
			//
			// (lift up a bit from whatever we trace-hit so we don't (eg) touch the floor)...
			//
			VectorCopy(tr.endpos,v3);
			v3[2] += 40;	// arb., roughly 2/3 of a man height
			//
			// limit-check this new dest, just in case we're in a really strange map area...
			//
			// (see reasoning on trace call above)
			//
			gi.trace(self->s.origin, _HELI_TRACEPARAM_MIN2, _HELI_TRACEPARAM_MAX2,
							v3, self,MASK_SOLID, &tr);
			if (tr.fraction < 1.0)
				VectorCopy(tr.endpos,v3);

			body->m_v3HeliFireAtCoords[2] = v3[2];	// so code will see it as a match next time and not do this again
			Cobra2Think_SetDumbFlyDest(self, v3);
			return;
		}

		// Z coords match!

		// success (ready to fire), so fall through...
		//
	}// if (qbWholeBodyMustMatch)
	else
	{
		// just try aiming the chaingun at it..
		//
		int iGunAim = Cobra2AimGunAtPos(self, body->m_v3HeliFireAtCoords, true);		// qboolean bSynchGunner

		switch (iGunAim)
		{
			case AIM_FALSE:

				return;	// gun still moving, let's wait and see

			case AIM_ERROR:
			{
				// bugger, gun has hit edge-stop and can't bear on target, so move heli body...
				//
				// slightly weird usage this, but it does the job... (will cause YAW movement towards target if not already there)
				//
				float fDegreesToGo;
				Cobra2_TurnToFace(self,	body->m_v3HeliFireAtCoords,	// vec3_t v3WorldPos
										false,				// qboolean qbRoll
										false,				// qboolean qbYAWDictatesFiring
										&fDegreesToGo		// float *pfDegreesToGo
								);

				if (fDegreesToGo)
				{
					// we need to swing the heli round...
					//
					Cobra2Think_SetTurnDest(self, body->m_v3HeliFireAtCoords);
					return;					
				}
				else
				{
					// forget it, although we've got an LOS to the target, we can't get the mgun to bear on it from this
					//	position even though the YAW is correct, ergo it must be too far underneath us.
					//
					//  this is really more of a scripting error (ie heli not put in suitable place), so for now I'm just 
					//	going to fail this command, though later it could be improved to change position...
					//
					ai->ActionIsDone(true);
					body->m_flags &= ~HELIFLAG_DOINGSCRIPTEDFIREAT;	
					return;
				}
			}

		}// switch (iGunAim)
	}

	// we're in a suitable firing position here, so...	
	//
	ai->SetCurrentActionNextThink(Cobra2Think_FireAt);
	self->nextthink	= level.time + FRAMETIME;
	body->m_flags &= ~HELIFLAG_DOINGSCRIPTEDFIREAT;	// important!

	// duration arg is either a time (if +ve), else a -ve rocket count...
	//
	if (body->m_fHeliFireAtTimer>0)	
		body->m_fHeliFireAtTimer += level.time;

}// void CobraThink_GotoFireAt(edict *self)



// we're trying to fire at something that involves pitching up/down to aim at it (because of missiles)...
//
void Cobra2Think_DipFireAt(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	self->nextthink	= level.time + FRAMETIME;

	Cobra2AimPilotAtPos(self, body->m_v3HeliFireAtCoords);	
	Cobra2AimGunAtPos(self, body->m_v3HeliFireAtCoords, true);	// qboolean bSynchGunner

	if ( (body->m_flags & (HELIFLAG_FIREDIP1DONE|HELIFLAG_FIREDIP2DONE)) != (HELIFLAG_FIREDIP1DONE|HELIFLAG_FIREDIP2DONE) )
		_Cobra2Fire_Chaingun(self);
	else
		Cobra2_Fire_OFF(self);

	if (!(body->m_flags & HELIFLAG_FIREDIP1DONE))
	{
		// still heading towards ideal pitch pos...
		//
		self->s.angles[HELI_PITCH] = angle2posneg2(self->s.angles[HELI_PITCH]);

		if (fabs(self->s.angles[HELI_PITCH] - body->m_fHeliFirePitch) <= HELI_COBRA_DIPFIRE_PITCHADJSPEED)
		{
			self->s.angles[HELI_PITCH] = body->m_fHeliFirePitch;
			body->m_flags |= HELIFLAG_FIREDIP1DONE;

			// fire missiles...(fHeliFireAtTimer == -1 or -2 here)
			//
			_Cobra2Fire_Rockets(self, -body->m_fHeliFireAtTimer);	// iRocketsToForceFire			
			body->m_fHeliFirePitch = -(body->m_fHeliFirePitch/2);	// so we pitch back to the other extent afterwards (looks nicer)			
		}
		else
		{
			// still heading there...
			//
			if (self->s.angles[HELI_PITCH] > body->m_fHeliFirePitch) // kef
				self->s.angles[HELI_PITCH] -= HELI_COBRA_DIPFIRE_PITCHADJSPEED;
			else
				self->s.angles[HELI_PITCH] += HELI_COBRA_DIPFIRE_PITCHADJSPEED;			

		}
		return;
	}

	if (!(body->m_flags & HELIFLAG_FIREDIP2DONE))
	{
		// heading back towards over-compensate backpitch pos...
		//
		self->s.angles[HELI_PITCH] = angle2posneg2(self->s.angles[HELI_PITCH]);

		if (fabs(self->s.angles[HELI_PITCH] - body->m_fHeliFirePitch) <= HELI_COBRA_DIPFIRE_PITCHADJSPEED)
		{
			self->s.angles[HELI_PITCH] = body->m_fHeliFirePitch;
			body->m_flags |= HELIFLAG_FIREDIP2DONE;
		}
		else
		{
			// still heading there...
			//
			if (self->s.angles[HELI_PITCH] > body->m_fHeliFirePitch) // kef
				self->s.angles[HELI_PITCH] -= HELI_COBRA_DIPFIRE_PITCHADJSPEED;
			else
				self->s.angles[HELI_PITCH] += HELI_COBRA_DIPFIRE_PITCHADJSPEED;			
		}
		return;
	}

	// all pitch stuff done (though we still need to level out again, so...
	//
	ai->SetCurrentActionNextThink(Cobra2Think_MovementFinish);
}// void CobraThink_DipFireAt(edict_t *self)





// called repeatedly while actually firing at something...
//
void Cobra2Think_FireAt(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	qboolean qbFireDone = false;

	self->nextthink	= level.time + FRAMETIME;

	// do minor YAW adj here as well for when gun is aimed at target ok, but body still has a bit to go...
	//			
	Cobra2_TurnToFace(self,	body->m_v3HeliFireAtCoords,	// vec3_t v3WorldPos
							false,				// qboolean qbRoll
							false,				// qboolean qbYAWDictatesFiring
							NULL				// float *pfDegreesToGo
							);
	Cobra2AimPilotAtPos(self, body->m_v3HeliFireAtCoords);	// same reasoning.
	//
	// if heli YAW'd, gun will be off-target...
	//
	Cobra2AimGunAtPos(self, body->m_v3HeliFireAtCoords_Original, true);	// qboolean bSynchGunner

	int iRocketsFired = Cobra2_Fire(self, false);	// qboolean qbInihibitRockets

	// check whether our time is a duration, or a rocket count...
	//
	if (body->m_fHeliFireAtTimer<0)	// if -ve, it's a rocket count...
	{
#ifdef _DEBUG
#ifdef SHOW_DESIGNER_ERRORS
		if (!(body->m_flags & HELIFLAG_ROCKETSENABLED))
		{  
			gi.dprintf("### HELI SCRIPT: aborting attack, rockets disabled!\n");
			Cobra2_Fire_OFF(self);
			ai->ActionIsDone(true);
			return;
		}
#endif		
#endif		
		body->m_fHeliFireAtTimer += iRocketsFired;	// dec rockets-to-fire count (-ve)

		// this is a specific rocket attack, so cease firing if the required number have been fired
		//	or if we've run out...
		//
		if (body->m_iHeliRocketsLeft==0 || body->m_fHeliFireAtTimer==0.0f)
			qbFireDone = true;
	}
	else
	{// end timer must be a real timer then, so...
		
		if (level.time > body->m_fHeliFireAtTimer)
			qbFireDone = true;
	}

	if (qbFireDone || (body->m_flags & HELIFLAG_ISDEAD))
	{
		Cobra2_Fire_OFF(self);
		ai->ActionIsDone(true);
	}

}// void CobraThink_FireAt(edict_t *self)

// set journey dest but don't worry about any obstructions, just do straight line...
//
void Cobra2Think_SetDumbFlyDest(edict_t *self, vec3_t v3)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	Cobra2_InitTurnStuff(self,v3);

	VectorCopy(self->s.origin, body->m_v3HeliDesiredPos_Start);

	ai->SetCurrentActionNextThink(Cobra2Think_HeadToDest);
	self->nextthink = level.time + FRAMETIME;
}


// Set journey dest, and ensure it gets there safely even if you have to do several calls to the dumb one to work
//	your way round obstacles.  If you set a dest that's inside a solid area then it sends out a ring of check positions
//	further and further out until one of them is in a clear spot, then changes the dest to that.
//
#define _RING_ENTRIES 18
const vec3_t fRingTable[_RING_ENTRIES]=
{
	// for speed reasons, this entry is tried first, not in it's logical place below
	{  0, 0, 1},	// special entry, searches upwards (copes with common hover-point mistakes by designers)

	{ -1, 1, 0},	
	{  0, 1, 0},
	{  1, 1, 0},

	{ -1, 0, 0},
	{  1, 0, 0},

	{ -1,-1, 0},
	{  0,-1, 0},
	{  1,-1, 0},

	// unlikely this'll be needed, but just in case we're told to fly too high...
	{  0, 0,-1},	

	// extra search ring, the surrounding 8 compass points but moving upwards at the same time

	{ -1, 1, 1},	
	{  0, 1, 1},
	{  1, 1, 1},

	{ -1, 0, 1},
	{  1, 0, 1},

	{ -1,-1, 1},
	{  0,-1, 1},
	{  1,-1, 1},

};

qboolean Cobra2_VecWithinWorld(edict_t *self, vec3_t v3)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	if (ai)
	{
		return (
				v3[0] < ai->GetHeliXWorldMin2() ||
				v3[0] > ai->GetHeliXWorldMax2() ||
				v3[1] < ai->GetHeliYWorldMin2() ||
				v3[1] > ai->GetHeliYWorldMax2() ||
				v3[2] < ai->GetHeliZWorldMin2() ||
				v3[2] > ai->GetHeliZWorldMax2()
				)?false:true;
	}
	return false;
}


// ensure that a vector isn't in a wall or off the edge of the map, adjust until legal, then return it...
//
void Cobra2_LegaliseVector(edict_t *self, vec3_t v3in, vec3_t v3Out, qboolean qbUseWorldClip)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	trace_t tr;

	vec3_t v3Dest;
	int iSafetyLoopCount=0;

	VectorCopy(v3in,v3Dest);
//	Cobra2_ClipVecToWorld(self, v3Dest);
	// new bit, let's see if it's impossible to get to this area because the dest is inside a solid
	//	(this can happen with both hover-twitching and entity-relative goto commands)
	//
	// This is achieved by tracing back from the dest and seeing if the starting point was solid...
	//
	gi.trace(v3Dest, _HELI_TRACEPARAM_MIN2, _HELI_TRACEPARAM_MAX2, self->s.origin, 
		self, MASK_SOLID, &tr);	

	// if smart dest pos is inside a wall or off the edge of the map, then we need to change it!...
	//
	if (tr.startsolid)
	{
		// dest point is unreachable by any means because it's inside a solid, so we've got to find a new dest pos.
		//
		// Unfortunately the trace command doesn't work how I'd like (ie. if you start off solid then the endpos is the first
		//	non-solid point) so I'm going to put 8 points around the dest, then keep expanding them in a ring until I get a
		//	valid destination...(aaarrggh!!!!)
		//
		#define HELI_WORLDSPACE_STEPUNITS 100	// step this many units per scan until we find a blank
		
		int iRingNumber=-1;
		int iPassMult=1;
		while (1)	
		{
			if (++iSafetyLoopCount>100)
			{
				// sod it, something's seriously shagged, probably heli is in a highly-cluttered world (unlikely), or
				//	some dozy twat of a designer has given it a dest coord miles off beyond the map edge. So all we can do is
				//	forget this goto, since the server's going to take too long to resolve it...
				//
				// (once the game is at release, this part of the code should never be hit, but it's here JIC)
				//
				VectorCopy(self->s.origin, v3Dest);
				break;	// out of while loop
			}

			if (++iRingNumber==_RING_ENTRIES)
			{
				iRingNumber=0;
				iPassMult++;
			}

			vec3_t v3;

			VectorSet(v3,	fRingTable[iRingNumber][0] * (iPassMult * HELI_WORLDSPACE_STEPUNITS),
							fRingTable[iRingNumber][1] * (iPassMult * HELI_WORLDSPACE_STEPUNITS),
							fRingTable[iRingNumber][2] * (iPassMult * HELI_WORLDSPACE_STEPUNITS)
						);
			Vec3AddAssign(v3Dest, v3);

			// update, as a speed issue, I can skip known world edges to avoid spurious traces...
			//
			if (Cobra2_VecWithinWorld(self, v3))
			{// within designer-specified world limits...
				gi.trace(v3, _HELI_TRACEPARAM_MIN2, _HELI_TRACEPARAM_MAX2,
					self->s.origin,self,MASK_SOLID, &tr);	

				if (!tr.startsolid)
				{
					// we've maybe found a clear bit, but we have to make sure it isn't off the edge of the map as well, so...
					// (reverse src/dest params and try again)
					gi.trace(self->s.origin, _HELI_TRACEPARAM_MIN2, _HELI_TRACEPARAM_MAX2,
						v3,self,MASK_SOLID, &tr);

					if (tr.fraction != 0.0f)	// gotta have  dest that allows *some* movement
					{
						// at last, we've found a clear bit, so let's use this as the final destination...
						//
						VectorCopy(v3,v3Dest);
						break;	// out of while loop
					}
				}
			}// within designer-specified world limits...
		}// while (1)	
	}// if smart dest is in a wall
#ifdef _DEBUG
#ifdef SHOW_DESIGNER_ERRORS
	// doesn't actually appear on screen, but wtf?
	if (!VectorCompare(v3Dest,v3in))	
		gi.dprintf("Vector Legalised (%d retries needed)\n",iSafetyLoopCount);
#endif
#endif
	VectorCopy(v3Dest,v3Out);

}// void Cobra_LegaliseVector(edict_t *self, vec3_t v3in, vec3_t v3Out, qboolean qbUseWorldClip)

void Cobra2_ClipVecToWorld(edict_t* self, vec3_t v3)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	if (ai)
	{
		if (v3[0] < ai->GetHeliXWorldMin2())
			v3[0] = ai->GetHeliXWorldMin2();
		else if (v3[0] > ai->GetHeliXWorldMax2())
			v3[0] = ai->GetHeliXWorldMax2();

		if (v3[1] < ai->GetHeliYWorldMin2())
			v3[1] = ai->GetHeliYWorldMin2();
		else if (v3[1] > ai->GetHeliYWorldMax2())
			v3[1] = ai->GetHeliYWorldMax2();
		
		if (v3[2] < ai->GetHeliZWorldMin2())
			v3[2] = ai->GetHeliZWorldMin2();
		else if (v3[2] > ai->GetHeliZWorldMax2())
			v3[2] = ai->GetHeliZWorldMax2();
	}
}


void Cobra2Think_SetSmartFlyDest(edict_t *self, vec3_t v3)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	VectorCopy(v3,body->m_v3HeliDesiredPos_Smart);
	body->m_flags |= HELIFLAG_DOINGSMARTGOTO;

	Cobra2_LegaliseVector(self, body->m_v3HeliDesiredPos_Smart, 
		body->m_v3HeliDesiredPos_Smart, true);

	ai->SetCurrentActionNextThink(Cobra2Think_SmartHeadToDest);
	self->nextthink = level.time + FRAMETIME;

}// void CobraThink_SetSmartFlyDest(edict_t *self, vec3_t v3)

// every time we get here we should either stop the smart-goto, or set another dumb-goto target...
//
void Cobra2Think_SmartHeadToDest(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	vec3_t v3Dest;	// so we can modify it to work our way round things

	VectorCopy(body->m_v3HeliDesiredPos_Smart,v3Dest);

	// are we actually there?...
	//
	if (VectorCompare(self->s.origin,v3Dest))
	{
		ai->ActionIsDone(true);
		self->nextthink = level.time + FRAMETIME;
		body->m_flags &= ~HELIFLAG_DOINGSMARTGOTO;	// amazingly important to do this
		return;
	}

	// we're not there yet, but can we see it from here?...
	trace_t tr;
		
	{
		gi.trace(self->s.origin, _HELI_TRACEPARAM_MIN2, _HELI_TRACEPARAM_MAX2, v3Dest, self,MASK_SOLID, &tr);	
	}

	if (tr.fraction == 1.0)
	{
		// yep, so head for it...
		//
		Cobra2Think_SetDumbFlyDest(self, v3Dest);
		if (VectorLength(body->m_v3HeliDeathDest) && 
			VectorCompare(body->m_v3HeliDesiredPos_Smart,body->m_v3HeliDeathDest))
		{
			body->m_flags |= HELIFLAG_ISDEADFINALGOTO;
		}
		return;
	}

	// ------------------
	// quick test, try tracing from the target back to us (which will be blocked), then set a dest position 3/4 up in the sky 
	//	above the block point, and see if we can see that...
	//	
	vec3_t v3InitialBlockage;
	VectorCopy(tr.endpos,v3InitialBlockage);	// preserve this collision point	
	gi.trace(v3Dest, _HELI_TRACEPARAM_MIN2, _HELI_TRACEPARAM_MAX2, self->s.origin,
		self,MASK_SOLID, &tr);	
	if (tr.fraction != 1.0)	// sanity check, should never happen because of collision in code above
	{
		VectorCopy(tr.endpos,v3Dest);	// preserve this collision point	
		v3Dest[2] = ai->GetHeliZWorldMin2() + (((ai->GetHeliZWorldMax2() - ai->GetHeliZWorldMin2())/4)*3);
		//
		// ok, can we see this sugested point from our current pos...
		//
		
		if (gi.trace(self->s.origin, _HELI_TRACEPARAM_MIN2, _HELI_TRACEPARAM_MAX2, 
						v3Dest, self,MASK_SOLID, &tr))
		{
			// yep, so head for it...
			//
			Cobra2Think_SetDumbFlyDest(self, v3Dest);
			return;
		}
		// oh well, worth a try...
	}
	VectorCopy(v3InitialBlockage,tr.endpos);	// restore old collision point	
	// ------------------

	// we can't see the smart-dest from here, so for now let's try and aim at the blockage point x,y, but set the Z to be 
	//	3/4 up in the sky...
	//
	VectorCopy(tr.endpos,v3Dest);
	v3Dest[2] = ai->GetHeliZWorldMin2() + ((ai->GetHeliZWorldMax2() - ai->GetHeliZWorldMin2())*0.75);
	//
	// can we see this point?
	//
	
	if (gi.trace(self->s.origin, _HELI_TRACEPARAM_MIN2, _HELI_TRACEPARAM_MAX2, v3Dest,
		self,MASK_SOLID, &tr))
	{
		// yes, so let's go with this...
		//
		// (1st, a fix for things we can't reach by flying over the top of stuff...)
		//
		if (VectorCompare(self->s.origin, v3Dest))
		{
			// let's do a 50-50 thing...
			//
			if (gi.flrand(0,1)>0.5)
			{
				Cobra2Think_SetDumbFlyDest(self, *Cobra2Think_GetRandomPatrolDestPos(self));
				return;
			}
#ifdef _DEBUG
#ifdef SHOW_DESIGNER_ERRORS	
			gi.dprintf("### HELISCRIPT ERROR: Can't reach dest by climbing over! Aborting...\n");
#endif
#endif
			ai->ActionIsDone(true);
			self->nextthink = level.time + FRAMETIME;
			body->m_flags &= ~HELIFLAG_DOINGSMARTGOTO;	// amazingly important to do this
			return;
		}		

		Cobra2Think_SetDumbFlyDest(self, v3Dest);
		return;
	}

	// damn, we must be in a really blocked area, how about straight up?...
	//
	VectorCopy(self->s.origin,v3Dest);
	v3Dest[2] = ai->GetHeliZWorldMin2() + ((ai->GetHeliZWorldMax2() - ai->GetHeliZWorldMin2())*0.75);

	if (gi.trace(self->s.origin, _HELI_TRACEPARAM_MIN2, _HELI_TRACEPARAM_MAX2, v3Dest,
		self,MASK_SOLID, &tr))
	{
		// yes, so let's go with this...
		//
		Cobra2Think_SetDumbFlyDest(self, v3Dest);
		return;
	}

	// hmmm, wherever we are we must be *really* blocked in, so fall out and allow normal code to pick a new random pos...
	//
	Cobra2Think_SetDumbFlyDest(self, *Cobra2Think_GetRandomPatrolDestPos(self));

}// void CobraThink_SmartHeadToDest(edict_t *self)

void Cobra2Think_SetTurnDest(edict_t *self, vec3_t v3)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	Cobra2_InitTurnStuff(self,v3);

	ai->SetCurrentActionNextThink(Cobra2Think_TurnToFace);
	self->nextthink = level.time + FRAMETIME;
}

// fXStrafeDist is of course +ve to our RT, -ve to LT...
//
void Cobra2Think_SetSidestrafeDest(edict_t *self, float fXStrafeDist)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	trace_t tr;

	vec3_t v3Offset,v3NewRelativeDest, vF, vR;

	AngleVectors(self->s.angles, vF, vR, NULL);
	VectorScale(vR, fXStrafeDist, v3NewRelativeDest);
	if (fXStrafeDist>=0)
	{ // strafing right
		body->m_flags &= ~HELIFLAG_SIDESTRAFEISTOLEFT;
	}
	else
	{
		body->m_flags |=  HELIFLAG_SIDESTRAFEISTOLEFT;
	}
	VectorSet(v3Offset,fXStrafeDist,0,0);

	VectorAdd(self->s.origin,v3NewRelativeDest,body->m_v3HeliDesiredPos_Stop);
	VectorCopy(self->s.origin, body->m_v3HeliDesiredPos_Start);	

	// we need to know if the strafe will send us off the entire world...
	//
	if (!Cobra2_VecWithinWorld(self, body->m_v3HeliDesiredPos_Stop))
	{
		// hmmm, let's have try re-calcing the dest stop pos...
		//
		// notice the use of the Vector_GetHalfWayPoint() call rather than just world-clipping the dest,
		//	this keeps the vector correct...
		//
		for (int iTries=0; iTries<10; iTries++)	// 30 = arb.
		{			
			Vector_GetHalfWayPoint(self->s.origin, body->m_v3HeliDesiredPos_Stop, 
				body->m_v3HeliDesiredPos_Stop);

			if (Cobra2_VecWithinWorld(self, body->m_v3HeliDesiredPos_Stop))
				break;
		}

		if (iTries==10)
		{
			// this is daft, it's well off the edge...
			//
#ifdef _DEBUG
#ifdef SHOW_DESIGNER_ERRORS		
			gi.dprintf("### HELISCRIPT: Sidestrafe would go off world edge!\n");
#endif
#endif

			self->nextthink = level.time + FRAMETIME;
			ai->ActionIsDone(true);
			return;
		}
		// new pos now back within world...
	}

	// vector is within the world, so let's check if it's clear...
	//
	gi.trace(self->s.origin, _HELI_TRACEPARAM_MIN2, _HELI_TRACEPARAM_MAX2,
							body->m_v3HeliDesiredPos_Stop, self,MASK_SOLID, &tr);		
	if (tr.fraction < 1.0)	// did the trace hit something?
		VectorCopy(tr.endpos,body->m_v3HeliDesiredPos_Stop);

	ai->SetCurrentActionNextThink(Cobra2Think_SidestrafeToDest);
	self->nextthink = level.time + FRAMETIME;
}

void Cobra2Think_Hover(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	self->nextthink = level.time + FRAMETIME;	

	// hover anim still playing? (approx 3 seconds)
	//
	if ((body->m_flags & HELIFLAG_PLAYINGHOVERANIM) && !body->GetFuselageEOS())
		return;

	// Hover anim finished. Switch to first frame (normal static position) and hold...
	//
	body->m_flags &= ~HELIFLAG_PLAYINGHOVERANIM;
	SimpleModelSetSequence(self, "cobrass", SMSEQ_HOLD);
	//
	//	Now, either finish hover think, 
	//				or do something nasty (if aggressive)
	//				or start anim off again for another hover cycle.

	// finish hover?...
	//
	if (level.time > body->m_fHeliHoverTimer || (body->m_flags & (HELIFLAG_ISDEAD|HELIFLAG_SCRIPTEDAIJUSTFINISHED)))
	{
		// yep...
		//		
		body->m_flags &= ~HELIFLAG_DOINGSCRIPTEDHOVER;
		ai->SetCurrentActionNextThink(Cobra2Think_MovementFinish);
		return;
	}


	// do something nasty?
	//
	if (body->m_flags & HELIFLAG_HOVERISAGGRESIVE)
	{
		// Shoot at people from time to time...
		//
		// (originally, it only did this on a 20% chance, which was fine when this was called every .1 seconds, but
		//	since it only gets here after a 3 second hover anim has finished it should do it all the time now)
		if (
			(body->m_flags & (HELIFLAG_ROCKETSENABLED|HELIFLAG_CHAINGUNENABLED))	// any weapon enabled
			)
		{
			edict_t *target = Cobra2_GetEntToTarget(self);

			// if doing hover as a result of scripted AI then ignore visibility arc (because it's more aggressive and probably went into hover at a known good waypoint)...
			//
			if (target && Cobra2HasTargetLOS(self, target, (body->m_flags & HELIFLAG_DOINGSCRIPTEDAI)?false:true))	// qboolean qbAccountForArc
			{
				Cobra2Think_SetFireAt(self, target->s.origin, 1.0f);	// 1 second burst
				return;
			}
		}

		// Turn occasionally, just to look more belligerent/alert...
		//
		// (originally, it only did this on a 20% chance, which was fine when this was called every .1 seconds, but
		//	since it only gets here after a 3 second hover anim has finished it should do it all the time now)
		//
		{
			// set random turn dir...
			//

			vec3_t v3;
			VectorSet(v3, gi.flrand(-1000,1000), gi.flrand(-1000,1000), 0);
			Vec3AddAssign(self->s.origin,v3);
			Cobra2Think_SetTurnDest(self, v3);
			return;
		}
	}

	// start hover anim off again...
	//
	SimpleModelSetSequence(self, "cobrass", SMSEQ_HOLD);
	body->m_flags |= HELIFLAG_PLAYINGHOVERANIM;
	body->SetFuselageEOS(false);

}// void CobraThink_Hover(edict_t *self)

void Cobra2_SetHoverParams(edict_t *self, float fTime, qboolean qbAggressive)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	// set flags...
	//
	body->m_flags |= HELIFLAG_DOINGSCRIPTEDHOVER;

	if (qbAggressive)
		body->m_flags |= HELIFLAG_HOVERISAGGRESIVE;
	else
		body->m_flags &=~HELIFLAG_HOVERISAGGRESIVE;
	//
	// set duration...
	//
	body->m_fHeliHoverTimer = level.time + fTime;

	// go...
	//
	self->nextthink = level.time + FRAMETIME;
	ai->SetCurrentActionNextThink(Cobra2Think_Hover);
}

void Cobra2Think_SidestrafeToDest(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	vec3_t v3HeliDesiredPos_Halfway;
	Vector_GetHalfWayPoint(body->m_v3HeliDesiredPos_Start, body->m_v3HeliDesiredPos_Stop, v3HeliDesiredPos_Halfway);

	self->nextthink = level.time + FRAMETIME;	

	Cobra2AimPilotAtPos(self, body->m_v3HeliDesiredPos_Stop);	// should look ok I think
	Cobra2ResetGunner(self);	// jic.


	// have we reached our dest?...
	//
	if (fabs (self->s.origin[0] - body->m_v3HeliDesiredPos_Stop[0]) <= body->m_fHeliMoveSpeed &&
		fabs (self->s.origin[1] - body->m_v3HeliDesiredPos_Stop[1]) <= body->m_fHeliMoveSpeed &&
		fabs (self->s.origin[2] - body->m_v3HeliDesiredPos_Stop[2]) <= body->m_fHeliMoveSpeed
		)
	{
		// we've reached it (or close enough), so do final position adjustement to make it exact...
		//
		//VectorCopy(body->m_v3HeliDesiredPos_Stop,self->s.origin);
		body->m_fHeliMoveSpeed	= 0;		

		// make sure we've turned off the gun barrel sequence
		SimpleModelSetSequence2(body->GetBarrelInst(), "barrel_still", SMSEQ_HOLD);
		ai->SetCurrentActionNextThink(Cobra2Think_MovementFinish);
	}
	else
	{// not reached dest...
		//
		// acceleration/roll logic here...
		//
		if (!WithinOrBeyondVectors2(self->s.origin, v3HeliDesiredPos_Halfway, body->m_v3HeliDesiredPos_Stop))
		{
			// accel...
			//
			body->m_fHeliMoveSpeed += (HELI_COBRA_ACCEL*2.0);

			// First, how far are we along the first half of the journey as a %age?...
			//
			vec3_t v3TotLengthVect,v3LengthVect;
			VectorSubtract(v3HeliDesiredPos_Halfway, body->m_v3HeliDesiredPos_Start,	v3TotLengthVect);	// journey dist
			VectorSubtract(v3HeliDesiredPos_Halfway, self->s.origin,			v3LengthVect);		// dist so far
			v3TotLengthVect[2] = v3LengthVect[2] = 0;	// lose the Z component

			// get the inverse percentage...
			// (broken into if-checked lines because of sqrt() returning float-errors occasionally when zeros occur)
			float fTotLen = VectorLength(v3TotLengthVect);
			if (fTotLen>0)
			{
				float fSoFarLen = VectorLength(v3LengthVect);
				if (fSoFarLen>0)
				{
					float fDistPercent = 100-((100/fTotLen) * fSoFarLen);
					float fIdealRoll = -(HELI_COBRA_MAXROLL-((fDistPercent * HELI_COBRA_MAXROLL)/100));	// HELI_COBRA_MAXROLL..0

					if (!(body->m_flags & HELIFLAG_SIDESTRAFEISTOLEFT))
						fIdealRoll = -fIdealRoll;

					// make sure 0..360 angle is -180..0..180...
					//
					self->s.angles[HELI_ROLL] = angle2posneg (self->s.angles[HELI_ROLL]);

					if (fabs(self->s.angles[HELI_ROLL] - fIdealRoll) <= HELISTRAFE_ROLLADJ_SPEED)
						self->s.angles[HELI_ROLL] = fIdealRoll;
					else
					{
						if (self->s.angles[HELI_ROLL] < fIdealRoll)
							self->s.angles[HELI_ROLL] += HELISTRAFE_ROLLADJ_SPEED;
						else
							self->s.angles[HELI_ROLL] -= HELISTRAFE_ROLLADJ_SPEED;
					}
				}
			}
		}
		else
		{// decel...

			body->m_fHeliMoveSpeed -= (HELI_COBRA_ACCEL*1.5);
			if (body->m_fHeliMoveSpeed < 2)	// arb, should be z, but this ensure we keep moving if we fall a little short
				body->m_fHeliMoveSpeed = 2;

			// pitch. First, how far are we along the 2nd half of the journey as a %age?...
			//
			vec3_t v3TotLengthVect,v3LengthVect;
			VectorSubtract(body->m_v3HeliDesiredPos_Stop, v3HeliDesiredPos_Halfway,	v3TotLengthVect);	// journey dist
			VectorSubtract(body->m_v3HeliDesiredPos_Stop, self->s.origin,			v3LengthVect);		// dist so far
			v3TotLengthVect[2] = v3LengthVect[2] = 0;	// lose the Z component

			// get the percentage... 
			// (broken into if-checked lines because of sqrt() returning float-errors occasionally when zeros occur)
			float fTotLen = VectorLength(v3TotLengthVect);
			if (fTotLen>0)
			{
				float fSoFarLen = VectorLength(v3LengthVect);
				if (fSoFarLen>0)
				{
					float fDistPercent = (100/fTotLen) * fSoFarLen;
					float fIdealRoll = HELI_COBRA_MAXROLL_DECEL-((fDistPercent * HELI_COBRA_MAXROLL_DECEL)/100);	// 0..HELI_COBRA_MAXROLL_DECEL
					if (body->m_fHeliMoveSpeed < 2)
						fIdealRoll = 0;

					// now just adjust our ROLL to suit...
					//
					if (!(body->m_flags & HELIFLAG_SIDESTRAFEISTOLEFT))
						fIdealRoll = -fIdealRoll;

					// make sure 0..360 angle is -180..0..180...
					//
					self->s.angles[HELI_ROLL] = angle2posneg (self->s.angles[HELI_ROLL]);

					if (fabs(self->s.angles[HELI_ROLL] - fIdealRoll) <= HELISTRAFE_ROLLADJ_SPEED)
						self->s.angles[HELI_ROLL] = fIdealRoll;
					else
					{
						if (self->s.angles[HELI_ROLL] < fIdealRoll)
							self->s.angles[HELI_ROLL] += HELISTRAFE_ROLLADJ_SPEED;
						else
							self->s.angles[HELI_ROLL] -= HELISTRAFE_ROLLADJ_SPEED;
					}
				}
			}
		}// decel
		
		// actual move...
		//
		vec3_t v;

		VectorSubtract(body->m_v3HeliDesiredPos_Stop,self->s.origin,v);
		VectorNormalize(v);
		VectorMA(self->s.origin, (body->m_fHeliMoveSpeed<HELI_COBRA_MAXSPEED)?body->m_fHeliMoveSpeed:HELI_COBRA_MAXSPEED, v, self->s.origin);

		// fire!!...
		//		
		if (body->m_fHeliMoveSpeed > HELI_COBRA_MAXSPEED/4)	// looks more natural
			Cobra2_Fire(self,false);		// qboolean qbInihibitRockets
		else
			Cobra2_Fire_OFF(self);

	}// not reached dest...

}// void CobraThink_SidestrafeToDest(edict_t *self)

// called for final vertical descent when over pad...(and rotor winddown)
//
void Cobra2Think_Land(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	IGhoulInst* instMainRotor = NULL, *instTailRotor = NULL;

	self->nextthink = level.time + FRAMETIME;
	instMainRotor = body->GetMainRotorInst();
	instTailRotor = body->GetTailRotorInst();

	// heli pitch and roll will already be zero here, but straighten out heads of pilot and gunner...
	//
	if (!Cobra2MovementFinished(self))
		return;

	// set speed as a percentage of the landing height...
	//
	float fPercent	= ((100.0f/(float)(HELI_COBRA_IDEAL_LANDING_HEIGHT)) * (self->s.origin[2] - body->m_v3HeliPadRestPos[2]));
	float fSpeed	= (fPercent * HELI_MAX_LANDING_SPEED)/100;	// HELI_MAX_LANDING_SPEED..0
	if (fSpeed<2)
		fSpeed=2;	// arb., maintain a certain minimum speed

	body->m_fHeliMoveSpeed = fSpeed;
	
	// since this is only called when X&Y are ok, we'll just check the Z...
	//
	if (fabs (self->s.origin[2] - body->m_v3HeliPadRestPos[2]) < body->m_fHeliMoveSpeed)
	{
		// close enough, we've either just landed or are winding down the rotors...
		// 
		VectorCopy(body->m_v3HeliPadRestPos,self->s.origin);	// force right coords anyway

		// wind down the rotor anim...
		//
		if (body->m_flags & HELIFLAG_ROTORSCHANGINGSPEED)
		{
			if (!body->m_bRotorEOS)
				return;
			
			// rotors wound down to stop, switch to static blade anim...
			//
			if (instMainRotor && instTailRotor)
			{
				SimpleModelSetSequence2(instMainRotor, "rotor", SMSEQ_HOLD);
				SimpleModelSetSequence2(instTailRotor, "rotor", SMSEQ_HOLD);
				body->m_flags&= ~HELIFLAG_ROTORSCHANGINGSPEED;		
			}
		}
		else
		{
			// time to start rotor winddown...  
			//
			// (this is still far faster than a real chopper wind-down, but short of having a ghoul anim 2 zillion frames long
			//	we'll probably have to live with it. 
			//	Let's just pretend that combat choppers have rotor-brakes for fast-shutdown...  <g>)
			//
			if (instMainRotor && instTailRotor)
			{
				SimpleModelSetSequence2(instMainRotor, "rotor_end", SMSEQ_HOLD);
				SimpleModelSetSequence2(instTailRotor, "rotor_end", SMSEQ_HOLD);
				body->m_flags&= ~HELIFLAG_ROTORSCHANGINGSPEED;		
				body->m_flags |= HELIFLAG_ROTORSCHANGINGSPEED;
				body->m_bRotorEOS = false;
			}
			return;
		}

		// assuming the rotor is now stopped...
		//
		ai->SetCurrentActionNextThink(Cobra2Think_OnPad);
		body->m_fHeliMoveSpeed = 0;
		return;
	}

	// still descending...
	//
	self->s.origin[2] -= body->m_fHeliMoveSpeed;	

}// void CobraThink_Land(edict_t *self)

// AI can 'rest' inside this code if no script events pending...
//
void Cobra2Think_OnPad(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	if (body->m_flags & HELIFLAG_ISDEAD)
	{
		ai->SetCurrentActionNextThink(Cobra2_Explode);
	}
	else
	{
		ai->ActionIsDone(true);
	}
}

// should only be called when you know it's on the pad...
//
void Cobra2Think_Repair(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	self->nextthink = level.time + FRAMETIME;

	if (self->health < self->max_health)
	{
		// repairing...
		//
		#define HELI_SECONDS_FOR_FULLREPAIR 5
		self->health += (self->max_health / (HELI_SECONDS_FOR_FULLREPAIR*10));
		if (self->health <= self->max_health)
			return;

		// healed...
		self->health = self->max_health;					
	}

	// done with healing...
	//
	ai->SetCurrentActionNextThink(Cobra2Think_OnPad);
}

vec3_t *Cobra2Think_GetRandomPatrolDestPos(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	static vec3_t v3 = {0,0,0};
	trace_t tr;

	// come up with a random position relative to ourselves...
	//
	v3[0] = self->s.origin[0] + ((gi.flrand(0, 5000))-2500);		// 5000 = roughly 535 feet (assuming 6' man is 56 units high)
	v3[1] = self->s.origin[1] + ((gi.flrand(0, 5000))-2500);		// 
	v3[2] = self->s.origin[2] + ((gi.flrand(0, 500 ))- 250);		// less vert movement

	Cobra2_ClipVecToWorld(self, v3);

	// for aesthetics, don't go too near the ground when picking random heli dest positions...
	//
	if (v3[2] < ai->GetHeliZWorldMin2()+56*2)	// no lower than 2 man heights above the pad
		v3[2] = ai->GetHeliZWorldMin2()+56*2;

	// now trace between current pos and dest pos to check flightpath is clear...
	//
	gi.trace(self->s.origin, _HELI_TRACEPARAM_MIN2, _HELI_TRACEPARAM_MAX2, v3,
		self,MASK_SOLID, &tr);	

	// (as it happens, this also copes with map edges by returning current position as the new dest (when tr.fraction=0.0f),
	//	which would cause a new random dir to be generated next cycle since we'd 'complete' this one instantly)
	//
	if (tr.fraction < 1.0)			
		VectorCopy(tr.endpos,v3);	// route blocked, so adopt collision point as dest pos...

	return &v3;
}

void Cobra2Think_SetPilotlookDest(edict_t *self, vec3_t v3)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	VectorCopy(v3,body->m_v3HeliHeadStop);
	ai->SetCurrentActionNextThink(Cobra2Think_PilotLookat);
	self->nextthink = level.time + FRAMETIME;
}

void Cobra2Think_SetGunnerlookDest(edict_t *self, vec3_t v3)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	VectorCopy(v3,body->m_v3HeliHeadStop);
	ai->SetCurrentActionNextThink(Cobra2Think_GunnerLookat);
	self->nextthink = level.time + FRAMETIME;
}

void Cobra2Think_PilotLookat(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	self->nextthink = level.time + FRAMETIME;

	if (Cobra2AimPilotAtPos(self, body->m_v3HeliHeadStop) != AIM_FALSE)
	{
		ai->ActionIsDone(true);
	}
}

void Cobra2Think_GunnerLookat(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	self->nextthink = level.time + FRAMETIME;

	if (Cobra2AimGunAtPos(self, body->m_v3HeliHeadStop, true) != AIM_FALSE)	// qboolean bSynchGunner);
	{
		ai->ActionIsDone(true);
	}
}


// called from the strafeing code... (ie probably every 10th of a second)
//
// (if you call this, you must call the OFF version below when finished with it)
//
// (return val is a # rockets actually fired)
//
int _Cobra2Fire_Rockets(edict_t *self, int iRocketsToForceFire)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	int iRocketsFired = 0;

	if ((body->m_flags & HELIFLAG_ROCKETSENABLED) && body->m_iHeliRocketsLeft)
	{
		if (iRocketsToForceFire ||
			level.time > body->m_fHeliRocketFireDebounceTime)	// takes care of both debounce time and initial zero value
		{
			do
			{
				int iTubeNumber = ((body->m_iHeliRocketsLeft+1)&~1)/2;	// 1/2/3/4

				Cobra2FireRockets(self,	(body->m_iHeliRocketsLeft&1)?iTubeNumber:0,	// int nLeftRockets 
										(body->m_iHeliRocketsLeft&1)?0:iTubeNumber	// int nRightRockets
								);
				body->m_iHeliRocketsLeft--;
				// re-arm heli when run out, if Greg etc wants it to. Note that I do this instead of removing the -- operator
				//	above because this way the code for which tube to fire out of still works ok
				//
				if (!body->m_iHeliRocketsLeft && (self->spawnflags & INFINITE_ROCKETS))
					body->m_iHeliRocketsLeft = HELI_MAX_ROCKETS;

				body->m_fHeliRocketFireDebounceTime = level.time + 1.0f;	// only fires rockets once a second max
				iRocketsFired++;
			}
			while ((--iRocketsToForceFire)>0);
		}
	}

	return iRocketsFired;
}
void _Cobra2Fire_Chaingun(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	if (body->m_flags & HELIFLAG_CHAINGUNENABLED)	
		Cobra2FireGun(self, false);	// qboolean bSporadic
}


int Cobra2_Fire(edict_t *self, qboolean qbInihibitRockets)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

#ifdef _DEBUG
#ifdef SHOW_DESIGNER_ERRORS
	if (!(body->m_flags & (HELIFLAG_ROCKETSENABLED | HELIFLAG_CHAINGUNENABLED)))
		gi.dprintf("### HELISCRIPT: FIRE called when all weapons disabled!\n");
#endif
#endif

	_Cobra2Fire_Chaingun(self);
	return  (qbInihibitRockets)?0:	_Cobra2Fire_Rockets (self, 0);	// int iRocketsToForceFire)

}// int Cobra_Fire(edict_t *self, qboolean qbInihibitRockets)


void Cobra2Think_TurnToFace(edict_t *self)
{	
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	self->nextthink = level.time + FRAMETIME;

	// aim the pilot at the turn dest, but only aim the gunner/gun there if we're going to fire at the dest on arrival... :-)
	//
	if (body->m_flags & HELIFLAG_DOINGSCRIPTEDFIREAT)
		Cobra2AimGunAtPos(self, body->m_v3HeliFireAtCoords, true);	// qboolean bSynchGunner
	else
		Cobra2ResetGunner(self);

	Cobra2AimPilotAtPos(self, body->m_v3HeliDesiredPos_Stop);

	if (!Cobra2_TurnToFace(self, body->m_v3HeliDesiredPos_Stop,	true,	// qbRoll
														true,			// qbYAWDictatesFiring
														NULL))			// float *pfDegreesToGo
	{
		return;
	}

	ai->SetCurrentActionNextThink(Cobra2Think_MovementFinish);
}



void Cobra2_SetAIParams(edict_t *self, float fTime)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	body->m_flags |= HELIFLAG_DOINGSCRIPTEDAI;

	body->m_fHeliAITimer = level.time + fTime;
	body->m_iHeliAITable = -1;

	self->nextthink = level.time + FRAMETIME;
	ai->SetCurrentActionNextThink(Cobra2Think_AI);
}


/*
findme:mystuff
qboolean Cobra2TargetEntValid(edict_t *self, edict_t *target)
edict_t *Cobra2_GetEntToTarget(edict_t *self)

  	edict_t*	m_AITarget;


	ai logic flow
	=============

	sublogic code:
	{
		before switch():

		if (current target ent no longer valid)
			cease this ai sublogic	
		else
		{
			if (can see target ent)
			{
				switch (rand)
				{
					case:	
				
						if (rand())
						{
							if (missiles enabled)
								fire 1..2 missile(s) at target ent
							break;
						}

						fire for rand() time, qb_rocketinhibit = rand()						
						break;

					case:	
					
						if (rand)
						{
							find waypoint nearest to target ent
							if (waypoint nearer than present origin)
							{
								if !already there, 
									goto it
//								else
//									aggressive hover
							}
//							else 
//								aggressive hover
						}
						else
							aggressive hover;

						break;
				}
			}
			else
			{
				do normal switch-case code
			}
		}
	}


  sublogic caller:

  (after sublogic called, before new table chooser)

  if (current target ent no longer valid)
	if (no new target ent found)
		set random hover




  // rand ai's...
  //
  type 1:  (volume sidestrafe) find volume centre nearest current target, if can Z match him (and X or Y volume bounds exceed player's coord), process for strafe
  type 2:  (waypoint hover)		goto random waypoint, pilot lookat, hover-aggressive for short rand time
  type 3:  (volume wander)  pick random points within volume space, hover-aggressive, pick random within another volume (do as clipped moverel), goto.
  type 4:  (volume zoom)	pick random point within a volume, goto, pick any other point within volume, headtodest

*/

// return values:-
//
// 0 = cease the calling AI code function and pick another (probably because target is dead)
// 1 = carry on with normal AI code
// 2 = new logic has been setup in here, so just return  (hmmm, in the end, this turns out to need the same action upon return as retval 0. Oh well)
//
int Cobra2_AISublogic(edict_t *self, generic_ghoul_heli_ai* ai, body_heli* body)
{

	// if current target ent no longer valid then cease this sublogic...
	//
	if (!Cobra2TargetEntValid(self, body->m_AITarget))
	{
		return 0;
	}

	// if not a very aggressive heli, then don't bother trying to fire at target...
	if (!(body->m_fAIAggressiveness > gi.irand(0,10)))
	{
		return 1;
	}

	// if we can't see the target, return and carry on as normal...
	//
	// (check heli aggressiveness to decide whether it can see behind it (actually, this is intelligence really, but
	//	there isn't a seperate variable for that)...
	//
	if (!(body->m_fAIAggressiveness > gi.irand(0,10)))
	{
		// not-so-aggressive helis check line-of-sight...
		//		
		if (!Cobra2HasTargetLOS(self, body->m_AITarget, true))	// qboolean qbAccountForArc
		{
			return 1;
		}
	}

	// now either fire at the target, or move nearer to him to fire later...
	//
	switch (gi.irand(0,1))
	{
		case 0:
		{
			vec3_t v3;

			// if there are any rockets available, then do rocket-specific attack (if you don't check for this then it might
			//	do rocket-firing during a timed attack, which'd cause it to Z-match and change heights (potentially going behind
			//	obstructions etc and losing LOS)
			//
			if ( (body->m_flags & HELIFLAG_ROCKETSENABLED) && body->m_iHeliRocketsLeft)
			{
				if (body->m_flags & HELIFLAG_ROCKETSENABLED)
				{					
					VectorSet(v3,gi.flrand(-10,10),gi.flrand(-10,10),0);	// at least have a chance of missing
					VectorAdd(body->m_AITarget->s.origin, v3, v3);
					v3[2] += body->m_AITarget->viewheight;
					Cobra2Think_SetFireAt(self, v3, (gi.irand(0,10)>=5)?-1:-2);
					break;
				}
			}

			// if we got here then no rocket attack happened, so do a timed attack... (which because of above check will be chaingun only)
			//
			VectorSet(v3,0,0,0); // flrand(-50,50),flrand(-50,50),0);
			v3[2] += body->m_AITarget->viewheight;
			VectorAdd(body->m_AITarget->s.origin, v3, v3);
			Cobra2Think_SetFireAt(self, v3, gi.flrand(2,4));	 
			break;
		}

		case 1:
		{
			// 50-50 either do an aggressive hover, or goto nearest waypoint from which he can see the player...
			//
			int iWayPoints = ai->GetNumWaypointPts();					
			if (iWayPoints && (body->m_fAIAggressiveness > gi.irand(0,10)))
			{
				// find nearest waypoint to target ent...
				//				
				vec3_t v3;				
				float fBestDist = 1000000;	// some suitably huge number bigger than any map (there is some sort of float_max defined, but only for windows, and this is server-side code)
				vec3_t v3BestWayPoint;

				float fBestDist_Visible = 1000000;	// some suitably huge number bigger than any map (there is some sort of float_max defined, but only for windows, and this is server-side code)
				vec3_t v3BestWayPoint_Visible;
				qboolean qbVisibleWayPointFound = false;

				trace_t tr;

				// this will be used several times, so work it out here...
				//
				vec3_t	v3TargetEyeLevel;
				VectorCopy (body->m_AITarget->s.origin, v3TargetEyeLevel);
				v3TargetEyeLevel[2] += body->m_AITarget->viewheight;	

				float fClosestSeePosDist = 1000000;	// some suitably huge number bigger than any map (there is some sort of float_max defined, but only for windows, and this is server-side code)
				vec3_t v3ClosestSeePosToCurrent;
				VectorClear(v3ClosestSeePosToCurrent);	// tested for Z later

				for (int i=0; i<iWayPoints; i++)
				{					
					heliWaypoint* pWayPoint = ai->GetWaypointPt(i);
					
					if (NULL == pWayPoint)
					{
						continue;
					}
					VectorSubtract(body->m_AITarget->s.origin,pWayPoint->m_v,v3);
					float fThisDist = VectorLength(v3);					


					// Can we see the target from here?...
					//					
					if (gi.trace(pWayPoint->m_v, NULL, NULL, v3TargetEyeLevel, self, MASK_OPAQUE, &tr))
					{
						// yes, so keep the best can-see waypoint...
						//
						if (fThisDist<fBestDist_Visible)
						{
							fBestDist_Visible = fThisDist;
							VectorCopy(pWayPoint->m_v,v3BestWayPoint_Visible);
							qbVisibleWayPointFound = true;
						}

						// is this visible waypoint the closest to our current position?...(used later on for logic tweak)
						//
						VectorSubtract(self->s.origin,pWayPoint->m_v,v3);
						if (!Vec3IsZero(v3))	// don't bother if we're already at this position
						{
							fThisDist = VectorLength(v3);
							if (fThisDist<fClosestSeePosDist)
							{
								fClosestSeePosDist = fThisDist;
								VectorCopy(pWayPoint->m_v,v3ClosestSeePosToCurrent);
							}
						}						
					}
					else
					{
						// no, but record it anyway in case none of the waypoint positions can see the target...
						//
						if (fThisDist<fBestDist)
						{
							fBestDist = fThisDist;
							VectorCopy(pWayPoint->m_v,v3BestWayPoint);
						}
					}
				}

				// We'll always have a valid best waypoint here, no need to verify.
				//	Now, if we can already see the target from here, then only adopt the new position if it can also
				//	see the player and is closer, but if we can't see it (and the new position can) then go there even if
				//	it's further away (this will look really intelligent)...
				//
				VectorSubtract(body->m_AITarget->s.origin,self->s.origin,v3);
				float fThisDist = VectorLength(v3);

				// can we see the target already?...
				//
				if (gi.trace(self->s.origin, NULL, NULL, v3TargetEyeLevel, self, MASK_OPAQUE, &tr))
				{
					// yes, did we find another waypoint that could also see the target?...
					//
					if (qbVisibleWayPointFound)
					{
						// is the new waypoint at least 800 (arb.) world units closer?...
						//
						if (fBestDist_Visible+800<fThisDist)
						{
							Cobra2Think_SetSmartFlyDest(self, v3BestWayPoint_Visible);
							break;
						}
					}
				}
				else
				{
					// we can't see the target from here, so should we move to the best waypoint so far?...
					//
					if (qbVisibleWayPointFound)		// if we can see it from somewhere else go to it regardless of distance
					{
						// ok, we can either go to the closest visible waypoint to the target, or to us...
						//
						if (!Vec3IsZero(v3))
						{
							// there is a position closer to us than the closest waypoint (so we'd have to move less), so use it...
							//
							Cobra2Think_SetSmartFlyDest(self, v3ClosestSeePosToCurrent);
							break;
						}

						// goto the waypoint closest the target...
						//
						Cobra2Think_SetSmartFlyDest(self, v3BestWayPoint_Visible);
						break;
					}

					// no visible point found, so only goto this waypoint if it's significantly closer...
					//
					if (fBestDist+800<fThisDist)	// 800 = arb. dist
					{
						Cobra2Think_SetSmartFlyDest(self, v3BestWayPoint);
						break;
					}
				}
			}

			// either there are no waypoints, or we already appear to be closer to the target than any waypoint alternative, so...
			//
			Cobra2_SetHoverParams(self,gi.flrand(1,3),true);	// qboolean qbAggressive
			break;
		}
	}// switch (irand(0,100)%2)

	return 2;

}// int Cobra2_AISublogic(edict_t *self, generic_ghoul_heli_ai* ai, body_heli* body)



// In these AI routines, simply set iHeliAITable to -1 to signal that the AI table is done
//
extern vec3_t	TestWayPoints[];


/*
  // rand ai's...
  //
  type 1:  (volume sidestrafe) find volume centre nearest current target, if can Z match him (and X or Y volume bounds exceed player's coord), process for strafe
  type 2:  (waypoint hover)		goto random waypoint, pilot lookat, hover-aggressive for short rand time
  type 3:  (volume wander)  pick random points within volume space, hover-aggressive, pick random within another volume (do as clipped moverel), goto.
  type 4:  (volume zoom)	pick random point within a volume, goto, pick any other point within volume, headtodest
*/


//  type 1:  (volume sidestrafe) find volume centre nearest current target, if can Z match him (and X or Y volume bounds exceed player's coord), process for strafe
//
void Cobra2AI_0(edict_t *self)
{	
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	int iSublogicRet = Cobra2_AISublogic(self, ai, body);

	switch (iSublogicRet)
	{
		case 0:	body->m_iHeliAITable = -1;	// (and fall through)
		case 2:	return;
	}	

	switch (body->m_iHeliAIIndex++)
	{
		case 0:
		{

			// is there a suitable waypoint volume we can go to for a strafe?...
			//
			int iWayPointVolumes = ai->GetNumWaypointVols();					
			if (iWayPointVolumes)
			{
				//
				// find nearest volume to target ent...
				//				
				vec3_t v3;				
				float fBestDist = 1000000;	// some suitably huge number bigger than any map (there is some sort of float_max defined, but only for windows, and this is server-side code)
				heliWaypoint* pBestWayPoint = NULL;

				for (int i=0; i<iWayPointVolumes; i++)
				{					
					heliWaypoint* pWayPoint = ai->GetWaypointVol(i);
					
					VectorSubtract(body->m_AITarget->s.origin,pWayPoint->m_v,v3);
					float fThisDist = VectorLength(v3);					

					if (fThisDist<fBestDist)
					{
						fBestDist = fThisDist;
						pBestWayPoint = pWayPoint;
					}
				}

				if (pBestWayPoint)	// should always be true, but wtf?
				{
					// Now, can we match target Z inside this volume?...
					//
					vec3_t vMins,vMaxs;
					
					pBestWayPoint->GetVolumeMins(vMins);
					pBestWayPoint->GetVolumeMaxs(vMaxs);

					if (body->m_AITarget->s.origin[2] >= vMins[2] &&
						body->m_AITarget->s.origin[2] <= vMaxs[2]
						)					
					{// volume encompasses target Z
						//
						//  now check either Y or X first...
						//
						int iAxisIndex = gi.irand(0,1);	// 0 or 1
						int iGoodAxis = -1;

						if (vMins[iAxisIndex] < body->m_AITarget->s.origin[iAxisIndex] &&
							vMaxs[iAxisIndex] > body->m_AITarget->s.origin[iAxisIndex]
							)
							iGoodAxis = iAxisIndex;
						else
						{
						iAxisIndex ^=1;	// toggle 0..1 (ie between x and y axis)
						if (vMins[iAxisIndex] < body->m_AITarget->s.origin[iAxisIndex] &&
							vMaxs[iAxisIndex] > body->m_AITarget->s.origin[iAxisIndex]
							)
							iGoodAxis = iAxisIndex;
						}

						if (iGoodAxis != -1)
						{
							// excellent! we've found a suitable strafeing volume...
							//
							// now work out a suitable sidestrafe distance...
							//
							body->m_fSideStrafeDist = min(	fabs(vMins[iGoodAxis] - body->m_AITarget->s.origin[iGoodAxis]),
															fabs(vMaxs[iGoodAxis] - body->m_AITarget->s.origin[iGoodAxis])
															);
							body->m_fSideStrafeDist = min(body->m_fSideStrafeDist,800);	// limit it

							// don't do small strafes, by the time it's ready to fire the run has ended...
							if (body->m_fSideStrafeDist > 100)
							{
								// make up a dest coord within the volume at nearest point to target...(strafe centre)
								//
								vec3_t v3;
								VectorCopy(body->m_AITarget->s.origin,v3);

								if (vMins[iGoodAxis^1]>body->m_AITarget->s.origin[iGoodAxis^1])
								{
									v3[iGoodAxis^1] = vMins[iGoodAxis^1];
									if (iGoodAxis==1)
										body->m_iSideStrafeDir = 3;
									else
										body->m_iSideStrafeDir = 2;
								}
								else
								{
									v3[iGoodAxis^1] = vMaxs[iGoodAxis^1];
									if (iGoodAxis==1)
										body->m_iSideStrafeDir = 1;
									else
										body->m_iSideStrafeDir = 0;
								}

								// now move to one side...
								//
								if (gi.irand(0,10)>=5)
									body->m_fSideStrafeDist = -body->m_fSideStrafeDist;	// random left or right

								const vec3_t v3StrafeDirMults[4] = {
																	{1,0,0},	// facing north, strafeing RT
																	{0,-1,0},	// facing east, strafeing RT
																	{-1,0,0},	// facing south, strafeing RT
																	{0,1,0}		// facing west, strafeing RT
																	};

								for (int i=0; i<3; i++)							
									v3[i] += (v3StrafeDirMults[body->m_iSideStrafeDir][i]) * -body->m_fSideStrafeDist;

								Cobra2Think_SetSmartFlyDest(self, v3);
								return;
							}// if (body->m_fSideStrafeDist > 100)
						}// Cobra2Think_SetSmartFlyDest(self, v3BestWayPoint_Visible);
					}// volume encompasses target Z
				}// if (pBestWayPoint)
			}// if (iWayPointVolumes)
		break;	// no suitable volume found, so break out and invalidate table at func bot.
		}// case 0:

	case 1:
		{
			// we've arrived at one end of the strafe run, so turn to face the proper direction...
			//
			const vec3_t v3StrafeDirAdds[4] = {
												{0,1,0},	// facing north
												{1,0,0},	// facing east
												{0,-1,0},	// facing south
												{-1,0,0}	// facing west
												};
			vec3_t v3;
			VectorCopy(self->s.origin,v3);
			for (int i=0; i<3; i++)							
				v3[i] += v3StrafeDirAdds[body->m_iSideStrafeDir][i] * 1000;

			Cobra2Think_SetTurnDest(self, v3);
			return;			
		}

	case 2:
		{
			// we're now at the correct position, facing the correct dir, so start the strafe...
			//
			Cobra2Think_SetSidestrafeDest(self, body->m_fSideStrafeDist*2);	// *2 because prev value was effectively the 'radius' (ie half)
			return;
		}
	}// switch (body->m_iHeliAIIndex++)

	body->m_iHeliAITable = -1;
}




//  type 2:  (waypoint hover)		goto random waypoint, pilot lookat, hover-aggressive for short rand time
void Cobra2AI_1(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	int iSublogicRet = Cobra2_AISublogic(self, ai, body);

	switch (iSublogicRet)
	{
		case 0:	body->m_iHeliAITable = -1;	// (and fall through)
		case 2:	return;
	}	

	switch (body->m_iHeliAIIndex++)
	{
		case 0:
		{
			// pick random waypoint to head to...
			//
			int iWayPoints = ai->GetNumWaypointPts();					
			if (iWayPoints)
			{
				heliWaypoint* pWayPoint = ai->GetWaypointPt(gi.irand(0,100)%iWayPoints);

				Cobra2Think_SetSmartFlyDest(self, pWayPoint->m_v);
				return;
			}
		break;	// no waypoints, so forget this logic code
		}

		case 1:
		{
			// I'll not bother doing a pilot lookat, it'd get overridden on the first hover-turn anyway,
			//	so just swing round to face the player..
			//			
			Cobra2Think_SetTurnDest(self, body->m_AITarget->s.origin);
			return;
		}

		case 2:
		{
			// hover for a short time...
			//
			Cobra2_SetHoverParams(self,gi.flrand(1,3), true
													//(body->m_fAIAggressiveness > irand(0,10))?true:false);	// qboolean qbAggressive
													);
			return;
		}
	}// switch (body->m_iHeliAIIndex++)

	body->m_iHeliAITable = -1;
}


//  type 3:  (volume wander)  pick random points within volume space, hover-aggressive, repeat sequence, exit
void Cobra2AI_2(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	int iSublogicRet = Cobra2_AISublogic(self, ai, body);

	switch (iSublogicRet)
	{
		case 0:	body->m_iHeliAITable = -1;	// (and fall through)
		case 2:	return;
	}	

	switch (body->m_iHeliAIIndex++)
	{
		case 0:
		case 2:
		{

			int iWayPointVolumes = ai->GetNumWaypointVols();					
			if (iWayPointVolumes)
			{
				heliWaypoint* pVolume = ai->GetWaypointVol(gi.irand(0,100)%iWayPointVolumes);

				// now goto a random position within that volume...
				//
				vec3_t v3,vMins,vMaxs;
				pVolume->GetVolumeMins(vMins);
				pVolume->GetVolumeMaxs(vMaxs);

				VectorSet(v3,gi.flrand(vMins[0],vMaxs[0]),gi.flrand(vMins[1],vMaxs[1]),gi.flrand(vMins[2],vMaxs[2]));
				Cobra2Think_SetSmartFlyDest(self, v3);
				return;
			}
			break;
		}

		case 1:
		case 3:
		{
			// hover aggressive for a short time...
			//
			Cobra2_SetHoverParams(self,gi.flrand(1,3), true);	// qboolean qbAggressive
			return;
		}
	}// switch (body->m_iHeliAIIndex++)

	body->m_iHeliAITable = -1;
}


//  type 4:  (volume zoom)	pick random point within a volume, goto, pick any other point within same volume, headtodest
void Cobra2AI_3(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	edict_t* nextTarget = Cobra2_GetEntToTarget(self);
	int iSublogicRet = Cobra2_AISublogic(self, ai, body);

	switch (iSublogicRet)
	{
		case 0:	body->m_iHeliAITable = -1;	// (and fall through)
		case 2:	return;
	}	

	switch (body->m_iHeliAIIndex++)
	{
		case 0:
		{
			int iWayPointVolumes = ai->GetNumWaypointVols();					
			if (iWayPointVolumes)
			{
				body->m_iWhichRandVolume = gi.irand(0,100)%iWayPointVolumes;
				heliWaypoint* pVolume = ai->GetWaypointVol(body->m_iWhichRandVolume);

				// now goto a random position within that volume...
				//
				vec3_t v3,vMins,vMaxs;
				pVolume->GetVolumeMins(vMins);
				pVolume->GetVolumeMaxs(vMaxs);

				VectorSet(v3,gi.flrand(vMins[0],vMaxs[0]),gi.flrand(vMins[1],vMaxs[1]),gi.flrand(vMins[2],vMaxs[2]));
				Cobra2Think_SetSmartFlyDest(self, v3);
				return;
			}
			break;
		}

		case 1:
		case 2:
		{
			// goto another point within the same volume...
			//
			heliWaypoint* pVolume = ai->GetWaypointVol(body->m_iWhichRandVolume);

			// now goto a random position within that volume...
			//
			vec3_t v3,vMins,vMaxs;
			pVolume->GetVolumeMins(vMins);
			pVolume->GetVolumeMaxs(vMaxs);

			VectorSet(v3,gi.flrand(vMins[0],vMaxs[0]),gi.flrand(vMins[1],vMaxs[1]),gi.flrand(vMins[2],vMaxs[2]));
			Cobra2Think_SetSmartFlyDest(self, v3);
			return;
		}

		case 3:
		{
			// hover aggressive for a short time...
			//
			Cobra2_SetHoverParams(self,2, true);	// qboolean qbAggressive
			return;
		}
	}

	body->m_iHeliAITable = -1;
}

#define NUM_COBRA2_AI_ROUTINES 4	// update this as needed


void Cobra2Think_AI(edict_t *self)
{
	self->nextthink = level.time + FRAMETIME;

	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	int iHeliAITable_Last = body->m_iHeliAITable;

	if (body->m_iHeliAITable != -1)
	{
		switch (body->m_iHeliAITable)
		{
			case 0: Cobra2AI_0(self);	break;
			case 1: Cobra2AI_1(self);	break;
			case 2: Cobra2AI_2(self);	break;
			case 3: Cobra2AI_3(self);	break;
		}
	}

	if (body->m_iHeliAITable == -1)
	{
		// pick a rand AI table (and check it's not the one we've just done) and a new target...
		//
		body->m_AITarget = Cobra2_GetEntToTarget(self);
		body->m_iHeliAITable = gi.irand(0,NUM_COBRA2_AI_ROUTINES)%NUM_COBRA2_AI_ROUTINES;	// because irand() *almost* never returns the max arg
		if (body->m_iHeliAITable == iHeliAITable_Last)
			body->m_iHeliAITable = (body->m_iHeliAITable+1)%NUM_COBRA2_AI_ROUTINES;
		body->m_iHeliAIIndex = 0;
		ai->ActionIsDone(true);

	}

}// void CobraThink_AI(edict_t *self)


// this should be return true if the target is either a live client or merc...
//
qboolean Cobra2TargetEntValid(edict_t *self, edict_t *target)
{		
	// fixme: update this when we decide how to tell a merc from a monster...
	//
	if (!target ||
		!target->inuse ||										// dead merc
		(target->inuse && target->takedamage==DAMAGE_NO)		// dead client
		)
		return false;
	
	return true;
}


// called at every new internal-AI action to decide who to shoot at, currently this will cycle just
//	between player(s), but later will have to be rewritten in order to shoot at non-client teammates (ie mercs in SoF)
//
// (returns NULL if no suitable target found)
//
edict_t *Cobra2_GetEntToTarget(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	edict_t *target;
	edict_t *target_firstfound=NULL;	
	int		 iTargetFirstFoundIndex;

	// time to select a new target?...
	//	
	if (level.time < body->m_fNewTargetTimer && Cobra2TargetEntValid(self, body->m_AITarget))
		return body->m_AITarget;

	body->m_fNewTargetTimer = level.time + 30;	// new target picked every 30 seconds
	body->m_AITarget = NULL;

	// fixme: this loop will probably need expanding when it's finally decided how mercs are implemented...
	for (int i=1; i<MAX_CLIENTS+1; i++)	
	{
		target = &g_edicts[i];

		// (fixme:update this check later to account for hired (non-client) mercs)
		if (!target->client)
			break;		// apparently there's no gap in the client list, so this point is EOL

		if (!Cobra2TargetEntValid(self, target))
			continue;

		// this entity is alive and a viable heli target...
		//
		if (!target_firstfound)
		{
			target_firstfound = target;
			iTargetFirstFoundIndex = i;
		}

		// should we use this as the return value?
		//		
		if (i>body->m_iLastEntFoundIndex)
		{
			body->m_iLastEntFoundIndex=i;
			body->m_AITarget = target;	// found a target further along ent list than last time, so use it
			break;
		}
	}

	// list exhausted, if there was anyone at all then target them...
	//
	if (target_firstfound)
	{
		body->m_iLastEntFoundIndex=iTargetFirstFoundIndex;	
		body->m_AITarget = target_firstfound;
	}

	return body->m_AITarget;

}// edict_t *Cobra2_GetEntToTarget(edict_t *self)




////////////////////////////////////////////////////////////////////////
//	end: stuff copied directly from m_heli.cpp
////////////////////////////////////////////////////////////////////////

void Cobra2_OutOfControl(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	vec3_t vGravity = {0, 0, 0};
	vec3_t vF, vR, vU;
	float fFinalPitch = 20; // kef
	AngleVectors(self->s.angles, vF, vR, vU);
	{
		vF[2] = 0;
		if (self->s.angles[HELI_PITCH] < fFinalPitch)
		{
			self->s.angles[HELI_PITCH] += 2;
		}
		else
		{
			self->s.angles[HELI_PITCH] -= 2;
		}

		if (-fFinalPitch - fabs(self->s.angles[HELI_PITCH]) < 2)
		{
			vGravity[2] = max(-10, -4*(level.time - body->m_fHeliHoverTimer));
		}
		VectorAdd(self->velocity, vGravity, self->velocity);
		VectorMA(self->velocity, self->s.angles[HELI_PITCH]*-2.5f, vF, self->velocity);

		self->avelocity[YAW] = min(self->avelocity[YAW]+45,180);
	}
	ai->SetCurrentActionNextThink(Cobra2_OutOfControl);
}

void Cobra2Touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	if (body->m_flags & HELIFLAG_ISDEADACKNOWLEDGED)
	{
		ai->TimeToDie(true);
	}
}

void Cobra2LandHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	if (self->spawnflags & IN_AIR)
	{
#ifdef _DEBUG
#ifdef SHOW_DESIGNER_ERRORS	
		gi.dprintf("### HELISCRIPT: Can't land when spawned in air! (no pad position)\n");
#endif
#endif
		ai->ActionIsDone(true);
		return;
	}

	// may as well check if we're already at the right XY position... (saves climbing up to v3IdealLandingPos then dropping again)
	//
	if (self->s.origin[0] != body->m_v3HeliPadRestPos[0] ||
		self->s.origin[1] != body->m_v3HeliPadRestPos[1] ||
		self->s.origin[2] >  body->m_v3HeliPadRestPos[2]+HELI_COBRA_IDEAL_LANDING_HEIGHT
		)
	{
		// xy mismatch, or too high, so head to a suitable place above the pad...
		//
		vec3_t v3IdealLandingPos;

		VectorCopy(body->m_v3HeliPadRestPos,v3IdealLandingPos);
		v3IdealLandingPos[2]+=HELI_COBRA_IDEAL_LANDING_HEIGHT;

		Cobra2Think_SetSmartFlyDest(self, v3IdealLandingPos);
	}
	else
	{
		ai->ActionIsDone(true);
	}
	body->m_flags |= HELIFLAG_DOINGSCRIPTEDLANDING;		// the clever bit (well, there has to be one...)
}

void Cobra2GotoCoordsHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	vec3_t vPos;
	VectorCopy(body->m_vPos,vPos);

	Cobra2Think_SetSmartFlyDest(self, vPos);
}

void Cobra2GotoRelEntHelper(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	vec3_t vPos;
	VectorCopy(body->m_target->s.origin, body->m_v3HeliDesiredPos_Smart);
	if (body->m_vPos[0])
	{
		body->m_v3HeliDesiredPos_Smart[0] = body->m_target->s.origin[0] + body->m_vPos[0];
	}
	if (body->m_vPos[1])
	{
		body->m_v3HeliDesiredPos_Smart[1] = body->m_target->s.origin[1] + body->m_vPos[1];
	}
	if (body->m_vPos[2])
	{
		body->m_v3HeliDesiredPos_Smart[2] = body->m_target->s.origin[2] + body->m_vPos[2];
	}
	VectorCopy(body->m_v3HeliDesiredPos_Smart,vPos);

	Cobra2Think_SetSmartFlyDest(self, vPos);
}

void Cobra2GotoRelHelper(edict_t *self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	vec3_t vPos;
	VectorAdd(body->m_vPos, self->s.origin, vPos);

	Cobra2Think_SetSmartFlyDest(self, vPos);
}

void Cobra2StrafeHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	Cobra2Think_SetSidestrafeDest(self, body->m_fArg);
}

void Cobra2HoverHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	// kinda silly, but I'm using body->m_vPos[0] as a boolean cuz I need
	//a parameter other than body->m_fArg
	Cobra2_SetHoverParams(self, body->m_fArg, body->m_vPos[0]);
}

void Cobra2HamburgerHelper(edict_t* self)
{
	// I couldn't resist...
}

void Cobra2FaceRelEntHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	vec3_t v3;

	VectorAdd(body->m_target->s.origin, body->m_vPos, v3);
	Cobra2Think_SetTurnDest(self, v3);
}

void Cobra2FaceAbsHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	vec3_t v3;

	VectorCopy(body->m_vPos, v3);
	Cobra2Think_SetTurnDest(self, v3);
}

void Cobra2FaceAbsDirHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	vec3_t v3;

	VectorCopy(body->m_vPos, v3);
	VectorNormalize(v3);
	VectorMA(self->s.origin, 1000, v3, v3);
	Cobra2Think_SetTurnDest(self, v3);
}

void Cobra2FaceRelCoordsHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	vec3_t v3;

	VectorAdd(self->s.origin, body->m_vPos, v3);
	Cobra2Think_SetTurnDest(self, v3);
}

void Cobra2HeadFaceRelEntHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	vec3_t v3;

	VectorAdd(body->m_target->s.origin, body->m_vPos, v3);
	if (body->m_fArg)
	{
		Cobra2Think_SetPilotlookDest(self, v3);
	}
	else
	{
		Cobra2Think_SetGunnerlookDest(self, v3);
	}
}

void Cobra2HeadFaceRelCoordsHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	vec3_t v3;

	VectorAdd(self->s.origin, body->m_vPos, v3);
	if (body->m_fArg)
	{
		Cobra2Think_SetPilotlookDest(self, v3);
	}
	else
	{
		Cobra2Think_SetGunnerlookDest(self, v3);
	}
}

void Cobra2HeadFaceAbsCoordsHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	vec3_t v3;

	VectorCopy(body->m_vPos, v3);
	if (body->m_fArg)
	{
		Cobra2Think_SetPilotlookDest(self, v3);
	}
	else
	{
		Cobra2Think_SetGunnerlookDest(self, v3);
	}
}

void Cobra2FireAtRelEntHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	// if told to fire at someone who's dead, then don't bother...
	edict_t* target = body->m_target;

	if (!Cobra2TargetEntValid(self,target))
	{
		ai->ActionIsDone(true);
		return;
	}

	vec3_t v3;
	VectorAdd(target->s.origin, body->m_vPos, v3);
	Cobra2Think_SetFireAt(self, v3, body->m_fArg);
}

void Cobra2FireAtAbsHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	vec3_t v3;

	VectorCopy(body->m_vPos, v3);
	Cobra2Think_SetFireAt(self, v3, body->m_fArg);
}

void Cobra2FireAtRelHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	vec3_t v3;

	VectorAdd(self->s.origin, body->m_vPos, v3);
	Cobra2Think_SetFireAt(self, v3, body->m_fArg);
}

void Cobra2RocketsEnableHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	if (body->m_fArg)
	{
		body->m_flags |= HELIFLAG_ROCKETSENABLED;
	}
	else
	{
		body->m_flags &= ~HELIFLAG_ROCKETSENABLED;
	}
	ai->ActionIsDone(true);
}

void Cobra2ChainGunEnableHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	if (body->m_fArg)
	{
		body->m_flags |= HELIFLAG_CHAINGUNENABLED;
	}
	else
	{
		body->m_flags &= ~HELIFLAG_CHAINGUNENABLED;
		Cobra2_Fire_OFF(self);
	}
	ai->ActionIsDone(true);
}

void Cobra2AutoFireHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	if (body->m_fArg)
	{
		body->m_flags |= HELIFLAG_AUTOFIRE;
	}
	else
	{
		body->m_flags &= ~HELIFLAG_AUTOFIRE;
	}
	ai->ActionIsDone(true);
}

void Cobra2WorldHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	if (body->m_fArg) // 1 == maxs
	{
		ai->SetHeliXWorldMax2(body->m_vPos[0]);
		ai->SetHeliYWorldMax2(body->m_vPos[1]);
		ai->SetHeliZWorldMax2(body->m_vPos[2]);
	}
	else // 0 == mins
	{
		ai->SetHeliXWorldMin2(body->m_vPos[0]);
		ai->SetHeliYWorldMin2(body->m_vPos[1]);
		ai->SetHeliZWorldMin2(body->m_vPos[2]);
	}
	ai->ActionIsDone(true);
}

void Cobra2RearmHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	body->m_iHeliRocketsLeft = HELI_MAX_ROCKETS;

	ai->ActionIsDone(true);
}

void Cobra2DeathDestHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	VectorCopy(body->m_vPos, body->m_v3HeliDeathDest);
	ai->ActionIsDone(true);
}

void Cobra2HealthHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	float fSkillMultiplier = 1; // 1 is the default for skill level 3 -- "challenging"

	//fSkillMultiplier = (.25 * (1 + level.skillLevel));
	fSkillMultiplier = ( 0.33 * (1 + game.playerSkills.getEnemyValue()) );
	if (0 == body->m_vPos[0]) // 0 == health
	{
		if (body->m_fArg < 1)
		{
			self->health = 0;
		}
		else
		{
			self->health = 1000 + (int)(fSkillMultiplier * body->m_fArg);
		}
	}
	else if (1 == body->m_vPos[0]) // 1 == maxhealth
	{
		if (self->max_health < 1)
		{
			self->max_health = 0;
		}
		else
		{
			self->max_health = 1000 + (int)(fSkillMultiplier * body->m_fArg);
		}
	}
	ai->ActionIsDone(true);
}

void Cobra2TraceDimsHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	if (body->m_fArg)
	{
		body->m_flags &= ~HELIFLAG_NOTRACEDIMS;
	}
	else 
	{
		body->m_flags |= HELIFLAG_NOTRACEDIMS;
	}
	ai->ActionIsDone(true);
}

void Cobra2AIHelper(edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();

	Cobra2_SetAIParams(self, body->m_fArg);
	ai->ActionIsDone(true);
}

void Cobra2_ToggleGun(edict_t *self, edict_t *other, edict_t *activator)
{
	// Greg is so demanding. I swear.
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;
	body_heli* body = ai->GetHeliBody();
	
	if (body)
	{
		body->SetFiring(!body->IsFiring());
	}
}

/*QUAKED m_x_chopper_black (1 .5 0) (-280 -280 -50) (280 280 70) IN_AIR ONDIE_HEAD2SPAWN INFINITE_ROCKETS DEBUG_WAYPOINTS ATTACK_CHOPPER

  This is the version that uses the class-based AI code.

--------SPAWNFLAGS----------
IN_AIR:				already in the air, no pad exists (so don't try landing it!!)
ONDIE_HEAD2SPAWN:	when damaged to death, plummet towards spawn pos before exploding
INFINITE_ROCKETS:	go on, guess.
DEBUG_WAYPOINTS:	enable this flag if you want to see waypoints/volumes/world dims drawn in the air
ATTACK_CHOPPER:		this heli will become a Hind attack chopper with a certain script command
ALWAYS_SEEN         draws no matter what

--------KEYS------------
Volume - aggressiveness. You *must* fill this in (values 0 to 10). 10's good for Tokyo, 3 is ok for Train.
*/

/*QUAKED m_x_chopper_green (1 .5 0) (-280 -280 -50) (280 280 70) IN_AIR ONDIE_HEAD2SPAWN INFINITE_ROCKETS DEBUG_WAYPOINTS ATTACK_CHOPPER ALWAYS_SEEN

  This is the version that uses the class-based AI code.

--------SPAWNFLAGS----------
IN_AIR:				already in the air, no pad exists (so don't try landing it!!)
ONDIE_HEAD2SPAWN:	when damaged to death, plummet towards spawn pos before exploding
INFINITE_ROCKETS:	go on, guess.
DEBUG_WAYPOINTS:	enable this flag if you want to see waypoints/volumes/world dims drawn in the air
ATTACK_CHOPPER:		this heli will become a Hind attack chopper with a certain script command
ALWAYS_SEEN         draws no matter what

--------KEYS------------
Volume - aggressiveness. You *must* fill this in (values 0 to 10). 10's good for Tokyo, 3 is ok for Train.
*/

/*QUAKED m_x_chopper_white (1 .5 0) (-280 -280 -50) (280 280 70) IN_AIR ONDIE_HEAD2SPAWN INFINITE_ROCKETS DEBUG_WAYPOINTS ATTACK_CHOPPER ALWAYS_SEEN

  This is the version that uses the class-based AI code.

--------SPAWNFLAGS----------
IN_AIR:				already in the air, no pad exists (so don't try landing it!!)
ONDIE_HEAD2SPAWN:	when damaged to death, plummet towards spawn pos before exploding
INFINITE_ROCKETS:	go on, guess.
DEBUG_WAYPOINTS:	enable this flag if you want to see waypoints/volumes/world dims drawn in the air
ATTACK_CHOPPER:		this heli will become a Hind attack chopper with a certain script command
ALWAYS_SEEN         draws no matter what

--------KEYS------------
Volume - aggressiveness. You *must* fill this in (values 0 to 10). 10's good for Tokyo, 3 is ok for Train.
*/

/*QUAKED m_x_hind (1 .5 0) (-280 -280 -50) (280 280 70) IN_AIR ONDIE_HEAD2SPAWN INFINITE_ROCKETS DEBUG_WAYPOINTS ATTACK_CHOPPER ALWAYS_SEEN

  This is the version that uses the class-based AI code.

--------SPAWNFLAGS----------
IN_AIR:				already in the air, no pad exists (so don't try landing it!!)
ONDIE_HEAD2SPAWN:	when damaged to death, plummet towards spawn pos before exploding
INFINITE_ROCKETS:	go on, guess.
DEBUG_WAYPOINTS:	enable this flag if you want to see waypoints/volumes/world dims drawn in the air
ATTACK_CHOPPER:		this heli will become a Hind attack chopper with a certain script command
ALWAYS_SEEN         draws no matter what

--------KEYS------------
Volume - aggressiveness. You *must* fill this in (values 0 to 10). 10's good for Tokyo, 3 is ok for Train.
*/

void generic_heli_spawnnow (edict_t *self, char *subclass, char* skinname)
{
	vec3_t v3DeathDest;	// unfortunately we need a temp vector since the body doesn't exist till later
	trace_t tr;
	bool	bHind = (0 == strcmp(subclass, "hind"));
	char	*modelName = bHind?"enemy/hind":"enemy/chopper",
			*modelSubname = bHind?"hind":"cobrass";

	// precache sounds and effects
	entSoundsToCache[CLSFX_SHOT_HELI1] = 1;
	entSoundsToCache[CLSFX_SHOT_HELI2] = 1;
	entSoundsToCache[CLSFX_SHOT_HELI3] = 1;

	gi.effectindex("weapons/othermz/heli");
	CacheAttack(ATK_HELIGUN);


//	self->spawnflags |= DEBUG_WAYPOINTS;	///////////'''''''''''''''''''''''''''''''
//	self->spawnflags |= INFINITE_ROCKETS;	///////////'''''''''''''''''''''''''''''''
//	self->volume = 0;	///////////////////////////////'''''''''''''''''''''''''''''''

	self->svflags |= SVF_MONSTER;

	if (self->spawnflags & HELI_ALWAYS_SEEN)
	{
		self->svflags |= SVF_ALWAYS_SEND; // draw this thing all the dang time. forget the pvs.
	}
	self->s.renderfx = RF_GHOUL;
	self->use = Cobra2_ToggleGun;

	VectorSet (self->mins, -280,-280,-50);
	VectorSet (self->maxs,  280, 280, 70);

	VectorCopy(self->s.origin,v3DeathDest);		// default to spawn pos

	if (!(self->spawnflags & IN_AIR))
	{
		// legalise vector BEFORE doing pad Z-match, or it ends up in the air again!

		// fixme: Cobra2_LegaliseVector() doesn't work in this position now that the heli is ai/class based
//findme:		body->m_flags |= HELIFLAG_NOTRACEDIMS
		if (self->ai)
		{
			Cobra2_LegaliseVector(self, self->s.origin, self->s.origin, false);	// qboolean qbUseWorldClip		
		}
//findme:		body->m_flags &= ~HELIFLAG_NOTRACEDIMS

		// Weird bit...because heli doesn't get pulled down with gravity then use a trace to measure ground distance & place properly.
		//
		// (and of course the code I'm calling still doesn't do the right thing - sigh)
		//
		VectorCopy(self->s.origin,v3DeathDest);		// default to legalised spawn pos
		self->s.origin[2]+=200;
		vec3_t v3;
		VectorCopy(self->s.origin,v3);
		v3[2] = -(200*3);

		// first, work out suitable default death dest...
		//		
		gi.trace(self->s.origin, self->mins, self->maxs, v3, self,MASK_SOLID, &tr);				
		if (tr.fraction < 1.0 &&					// if no hit, then this is one *deep* world, and probably a mistake...
			tr.fraction != 0.0f						// but if 0.0 returned, then we've gone outside the world, ie low roof!
			)
			VectorCopy(tr.endpos,v3DeathDest);

		// now work out correct landing pos...
		//
		vec3_t v3LandMins;
		VectorSet(v3LandMins,0,0,self->mins[2]);
		gi.trace(self->s.origin, v3LandMins, NULL, v3, self,MASK_SOLID, &tr);		
		if (tr.fraction < 1.0 &&					// if no hit, then this is one *deep* world, and probably a mistake...
			tr.fraction != 0.0f						// but if 0.0 returned, then we've gone outside the world, ie low roof!
			)
			VectorCopy(tr.endpos,self->s.origin);
		else
			self->s.origin[2]-=200;	// ... so just put it back as it was.
	}

//	body->m_flags		= 0;
//	body->m_fHeliMoveSpeed	= 0;
//	fHeliRocketFireDebounceTime = 0; 
//	iHeliRocketsLeft = HELI_MAX_ROCKETS;

	self->health = self->max_health = 1000;	

	gi.linkentity (self);


	// Update, we need to get the biggest bounding box possible for trace statements since the heli
	//	can turn as it moves. Note that this should only be the X and Y axis (since 99% of the collision is
	//	for the rotor, which is round, and the tail is thin enough that even when it swings to 45 degree points
	//	it shouldn't stick out beyond the widest X or Y point. As for the Z, handle it by just doing twice the
	//	normal height (to cope with pitches etc)
	//
	float fMin = min(self->mins[0],self->mins[1]);
	float fMax = max(self->maxs[0],self->maxs[1]);
	float fXYRadius	= max(fabs(fMin),fMax);						// max radius dist from centre
	float fZRadius	= (max(fabs(self->mins[2]),self->maxs[2]))*2;	// max radius dist from centre
		  
	self->ai = ai_c::Create(AI_HELI, self, modelName, subclass);//new generic_ghoul_heli_ai(self, subclass);

	if (self->ai)
	{
		body_heli* body = ((generic_ghoul_heli_ai*)(ai_public_c*)self->ai)->GetHeliBody();

		if (body)
		{
			VectorSet(body->m_v3HeliTraceMins,-fXYRadius,-fXYRadius,-fZRadius);
			VectorSet(body->m_v3HeliTraceMaxs, fXYRadius, fXYRadius, fZRadius);
			VectorCopy(self->s.origin,  body->m_v3HeliPadRestPos);
			VectorCopy(v3DeathDest,		body->m_v3HeliDeathDest);									
			body->m_fAIAggressiveness = self->volume;	// 0..10 
			// set the skin
			SetSkin(self, modelName, modelSubname, subclass, skinname, false);

			if ( (self->spawnflags & ATTACK_CHOPPER) && (game.playerSkills.getEnemyValue() > 1) )
			{
				body->SetSnipeability(false);
			}
			else
			{
				body->SetSnipeability(true);
			}
		}
	}

	self->flags	   |= FL_NO_KNOCKBACK;
	self->movetype	= MOVETYPE_FLY;
	self->solid		= SOLID_BBOX;
	self->takedamage= DAMAGE_YES;
	self->pain		= NULL;//CobraThink_Pain;
	self->die		= NULL;//CobraThink_Die;
	self->think		= NULL;//CobraThink_OnPad;
	self->touch		= Cobra2Touch;
	self->nextthink = level.time + FRAMETIME;

}

void generic_heli_init (edict_t *self)
{
	// this is the last time we have to set the nextthink. from now on the ai
	//will take care of it.
	self->nextthink = level.time + FRAMETIME;
	self->ai->NewDecision(new base_decision(), self);
	/*
//	self->ai->NewDecision(new formation_mob_decision(NULL), self);
	self->ai->NewDecision(new pursue_decision(), self);
	self->ai->NewDecision(new search_decision(), self);
//	self->ai->NewDecision(new dodge_decision(), self);
//	self->ai->NewDecision(new capture_decision(LEADERFLAG_RECRUIT), self);
	self->ai->NewDecision(new leader_sfile_decision(LEADERFLAG_RECRUIT), self);
	self->ai->NewDecision(new retreat_decision(), self);

	self->ai->SetRank(3);
*/
	gi.effectindex("environ/helismoke");
	gi.effectindex("heli_expl");
	gi.soundindex("impact/explosion/exp1.wav");
	gi.soundindex("impact/explosion/med.wav");
	gi.effectindex("weapons/world/autogundeath");
	CacheAttack(ATK_ROCKET);
	gi.soundindex("weapons/assault/fire.wav");
	gi.effectindex("weapons/world/rockettrail2");

	// if the ATTACK_CHOPPER flag is set, we'll precache the Hind body by creating
	//an edict, using it to spawn a Hind, and then free the edict
	if (self->spawnflags & ATTACK_CHOPPER)
	{
		edict_t *ent = G_Spawn();
		SP_m_x_hind(ent);
		G_FreeEdict(ent);
	}
}





void SP_m_x_chopper_black (edict_t *self)
{

	generic_heli_spawnnow(self,"cobra","m_x_chopper_black");
	self->think = generic_heli_init;
}

void SP_m_x_chopper_green (edict_t *self)
{

	generic_heli_spawnnow(self,"cobra","m_x_chopper_green");
	self->think = generic_heli_init;
}

void SP_m_x_chopper_white (edict_t *self)
{

	generic_heli_spawnnow(self,"cobra","m_x_chopper_white");
	self->think = generic_heli_init;
}

void SP_m_x_hind (edict_t *self)
{

	generic_heli_spawnnow(self,"hind","hind");
	self->think = generic_heli_init;
}

heliWaypoint::heliWaypoint(heliWaypoint *orig)
{
	VectorCopy(orig->m_v, m_v);
	VectorCopy(orig->m_volMins, m_volMins);
	VectorCopy(orig->m_volMaxs, m_volMaxs);
	m_next = NULL;
}

generic_ghoul_heli_ai::generic_ghoul_heli_ai(generic_ghoul_heli_ai *orig)
{
	m_bActionDone = orig->m_bActionDone;
	m_bTimeToDie = orig->m_bTimeToDie;
	m_bPilotDead = orig->m_bPilotDead;
	m_bGunnerDead = orig->m_bGunnerDead;
	*(int *)&m_attacker = GetEdictNum(orig->m_attacker);
	m_ScriptActionCounter = orig->m_ScriptActionCounter;
	m_LastThinkingActionID = orig->m_LastThinkingActionID;

	fHeliXWorldMin2 = orig->fHeliXWorldMin2;
	fHeliXWorldMax2 = orig->fHeliXWorldMax2;
	fHeliYWorldMin2 = orig->fHeliYWorldMin2;
	fHeliYWorldMax2 = orig->fHeliYWorldMax2;
	fHeliZWorldMin2 = orig->fHeliZWorldMin2;
	fHeliZWorldMax2 = orig->fHeliZWorldMax2;

	m_nActionSize = orig->m_nActionSize;

	m_waypoints = NULL;

	m_nNumWaypoints = orig->m_nNumWaypoints;
	m_nNumWaypointPts = orig->m_nNumWaypointPts;
	m_nNumWaypointVols = orig->m_nNumWaypointVols;
}

void generic_ghoul_heli_ai::Evaluate(generic_ghoul_heli_ai *orig)
{
	m_bActionDone = orig->m_bActionDone;
	m_bTimeToDie = orig->m_bTimeToDie;
	m_bPilotDead = orig->m_bPilotDead;
	m_bGunnerDead = orig->m_bGunnerDead;
	m_attacker = GetEdictPtr((int)orig->m_attacker);
	m_ScriptActionCounter = orig->m_ScriptActionCounter;
	m_LastThinkingActionID = orig->m_LastThinkingActionID;

	fHeliXWorldMin2 = orig->fHeliXWorldMin2;
	fHeliXWorldMax2 = orig->fHeliXWorldMax2;
	fHeliYWorldMin2 = orig->fHeliYWorldMin2;
	fHeliYWorldMax2 = orig->fHeliYWorldMax2;
	fHeliZWorldMin2 = orig->fHeliZWorldMin2;
	fHeliZWorldMax2 = orig->fHeliZWorldMax2;

	m_nActionSize = orig->m_nActionSize;

	m_waypoints = NULL;

	m_nNumWaypoints = orig->m_nNumWaypoints;
	m_nNumWaypointPts = orig->m_nNumWaypointPts;
	m_nNumWaypointVols = orig->m_nNumWaypointVols;
	// Don't go down the Evaluate heirachy as this is a special case
}

void generic_ghoul_heli_ai::Read()
{
	char			loaded[sizeof(generic_ghoul_heli_ai)];
	heliWaypoint	*loadable;
	heliWaypoint	**llist;
	int				i;

	gi.ReadFromSavegame('AIHA', loaded + GGHA_SAVE_START, GGHA_SAVE_END - GGHA_SAVE_START);
	Evaluate((generic_ghoul_heli_ai *)loaded);

	ai_c::Read();

	if(GetNumWaypoints())
	{
		llist = &m_waypoints;
		for(i = 0; i < GetNumWaypoints(); i++)
		{
			loadable = new heliWaypoint();
			gi.ReadFromSavegame('HEWP', loadable, sizeof(heliWaypoint));

			*llist = loadable;
			llist = &(loadable->m_next);
		}
	}
}

void generic_ghoul_heli_ai::Write()
{
	byte					*save_start;
	generic_ghoul_heli_ai	*savable;
	heliWaypoint			*wp;
	heliWaypoint			*temp;

	savable = new generic_ghoul_heli_ai(this);
	save_start = (byte *)savable;
	gi.AppendToSavegame('AIHA', save_start + GGHA_SAVE_START, GGHA_SAVE_END - GGHA_SAVE_START);
	delete savable;

	ai_c::Write();

	if(GetNumWaypoints())
	{
		for(wp = m_waypoints; wp; wp = wp->m_next)
		{
			temp = new heliWaypoint(wp);
			gi.AppendToSavegame('HEWP', temp, sizeof(heliWaypoint));
		}
	}
}

// end