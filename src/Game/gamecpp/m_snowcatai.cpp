// m_snowcatai.cpp

#include "g_local.h"
#include "ai_private.h"
#include "m_generic.h"
#include "m_snowcatai.h"
#include "g_obj.h"			// for OrientBolton() and modelSpawnData stuff
#include "callback.h"
#include "..\qcommon\ef_flags.h"

#define SNOWCAT_INACTIVE		(1<<0)

typedef enum
{
	SNOWCATOBJ_SNOWCAT = 0,
	SNOWCATOBJ_TURRET,
	SNOWCATOBJ_GUN,
	SNOWCATOBJ_MAXOBJS
};

modelSpawnData_t snowcatModelData[MD_SNOWCAT_SIZE] =
{	
//      dir					file			surfaceType			material			health				solid			material file      cnt  scale
"enemy/snowcat",			"body",			SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// SNOWCATOBJ_SNOWCAT
"enemy/snowcat",			"turret",		SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// SNOWCATOBJ_TURRET
"enemy/snowcat",			"gun",			SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// SNOWCATOBJ_GUN
};

modelSpawnData_t headlight =
{
"objects/generic/beam",		"beam",			SURF_NONE,			MAT_NONE,			0,					SOLID_NOT,		NULL,				0,	0.0,		NULL,	// SNOWCATOBJ_HEADLIGHTBEAM
};

SnowcatTreadCallback theSnowcatTreadCallback;

bool SnowcatTreadCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	edict_t* self = (edict_t*)ent;
	generic_ghoul_snowcat_ai* ai = (generic_ghoul_snowcat_ai*)(ai_public_c*)self->ai;
	body_snowcat* body = NULL;

	if (ai)
	{
		body = ai->GetSnowcatBody();
	}
	if (!body)
	{
		return false;
	}
	if (body->GetTreadFire())
	{
		// we're at the end of a firing sequence for the snowcat body, so switch
		//back to the appropriate non-firing tread sequence (slightly complicated
		//by the rotating texture coords for the treads)
		body->SetTreadFire(false);
		if (body->GetTreads())
		{
			SimpleModelSetSequence2(body->GetTankInst(), "snowcat", SMSEQ_LOOP);
		}
		else
		{
			SimpleModelSetSequence2(body->GetTankInst(), "snowcat", SMSEQ_HOLDFRAME);
		}
	}
	return true;
}

void debug_drawbox(edict_t* self,vec3_t vOrigin, vec3_t vMins, vec3_t vMaxs, int nColor);

int GetGhoulPosDir2(vec3_t sourcePos, vec3_t sourceAng, IGhoulInst *inst,
					   GhoulID partID, char *name, vec3_t pos, vec3_t dir, vec3_t right,
					   vec3_t up);

generic_ghoul_snowcat_ai::~generic_ghoul_snowcat_ai()
{
}

void generic_ghoul_snowcat_ai::Think(edict_t &monster)
{
	//debug_drawbox(&monster, NULL, NULL, NULL, 0);
	if (ai_freeze->value)
	{
		return;
	}

//	gi.dprintf("tank health = %3.2f percent\n", (float)monster.health/(float)monster.max_health);
	if (GetSnowcatBody() && GetSnowcatBody()->IsDeactivated())
	{
		// deactivate
		monster.nextthink = 0;
		GetSnowcatBody()->SetTreads(false);
		return;
	}

	// if our machine gun has been set to auto attack...
	if (m_bMachGunAuto)
	{
		Snowcat_MachGunAuto(&monster);
	}
	monster.nextthink = level.time + FRAMETIME;
	EvaluateSenses(monster);

	if (!current_action)
	{	// do some thinking - this is not correct
		if (actions.size())
		{
			NextAction(monster);
		}
	}

	if (current_action)
	{
		m_LastThinkingActionID = ((snowcat_action*)(action_c*)current_action)->GetID();
		if (current_action->Think(*this, monster))
		{ 
			NextAction(monster);
			if (GetSnowcatBody())
			{
				GetSnowcatBody()->ResetScriptParams();
			}
		}
	}
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
			G_FreeEdict (&monster);
		}
	}
//	AimMainGun(ent->s.origin);
//	AimMachGun(ent->s.origin);
}

void generic_ghoul_snowcat_ai::Init(edict_t *self, char *ghoulname, char* subclass)
{
	m_ScriptActionCounter = m_LastThinkingActionID = 0;
	m_bTimeToDie = false;
	m_attacker = NULL;
	m_bMachGunAuto = false;
	m_bCheckMachGunLOS = true;
	m_bRetVal = false;
	// build the snowcat here

	// snowcat (root object)
	ggObjC *cSnowcat = NULL;
	// turret, cannon, machine gun
	ggObjC	*cTurret = NULL,
			*cCannon = NULL,
			*cNull = NULL,
			*cMachGun = NULL;
	// anims for fuselage and boltons
	GhoulID	cTurretSeq=0,
			cSnowcatSeq=0,
			cCannonSeq=0,
			cNullSeq=0,
			cMachGunSeq=0;
	// bolt located on the fuselage (aka "bolter")
	GhoulID cBolterBolt=0;
	// bolt located on the bolted-on item (aka "boltee")
	GhoulID cBolteeBolt=0;
	ggOinstC	*t=NULL;
	ggBinstC	*cBolteeBolted=NULL, *bInstTurret = NULL, *bInstMachGunBase = NULL;
	IGhoulInst	*pInst=NULL;
	Matrix4		mat,mat1,mat2;
	GhoulID		tempNote=0, tempMaterial = 0;
	char		materialName[100] = "";
	IGhoulObj	*boltObj = NULL;

	ent->clipmask = MASK_MONSTERSOLID|MASK_PLAYERSOLID; 

	if (!GetSnowcatBody())
	{
		return;
	}
	// create the snowcat body

	// gotta remove this flag or SimpleModelInit2 bombs out
	ent->s.renderfx &= ~RF_GHOUL;
	SimpleModelInit2(ent,&snowcatModelData[SNOWCATOBJ_SNOWCAT],NULL,NULL);
	if (ent->ghoulInst)
	{
		ent->movetype = MOVETYPE_PUSH;
		GetSnowcatBody()->SetTankEdict(ent);
		SimpleModelSetSequence2(GetSnowcatBody()->GetTankInst(), "snowcat", SMSEQ_HOLD);
	}

	// bolt on the turret
	if ( cBolteeBolted = SimpleModelAddBolt(ent, snowcatModelData[SNOWCATOBJ_SNOWCAT], "DUMMY03", 
			snowcatModelData[SNOWCATOBJ_TURRET], "DUMMY03", NULL) )
	{
		bInstTurret = cBolteeBolted;
		pInst = cBolteeBolted->GetInstPtr();
		GetSnowcatBody()->SetTurretInst(cBolteeBolted);

		if (pInst && (boltObj = pInst->GetGhoulObject()) )
		{
			GetSnowcatBody()->SetTurretBolt(boltObj->FindPart("DUMMY03"));
			// register a callback for the snowcat tread sequence
			tempNote=pInst->GetGhoulObject()->FindNoteToken("EOS");
			if (tempNote)
			{
				pInst->AddNoteCallBack(&theSnowcatTreadCallback,tempNote);
			}
		}
	}

	// bolt mach gun #1 onto the turret
	if (cBolteeBolted = ComplexModelAddBolt(bInstTurret, snowcatModelData[SNOWCATOBJ_TURRET],
					"DUMMY01", snowcatModelData[SNOWCATOBJ_GUN], "DUMMY01", NULL))
	{
		pInst = cBolteeBolted->GetInstPtr();
		GetSnowcatBody()->SetMachGunInst(cBolteeBolted);
	}
	if (pInst && pInst->GetGhoulObject())
	{
		GetSnowcatBody()->SetMachGunBolt(pInst->GetGhoulObject()->FindPart("dummy01"));
	}

	// bolt the cannon onto the turret
	if (cBolteeBolted = ComplexModelAddBolt(bInstTurret, snowcatModelData[SNOWCATOBJ_TURRET],
					"DUMMY02", snowcatModelData[SNOWCATOBJ_GUN], "DUMMY01", NULL))
	{
		pInst = cBolteeBolted->GetInstPtr();
		GetSnowcatBody()->SetMachGun2Inst(cBolteeBolted);
	}
	if (pInst && pInst->GetGhoulObject())
	{
		GetSnowcatBody()->SetMachGun2Bolt(pInst->GetGhoulObject()->FindPart("dummy01"));
	}

	// when we deactivate the 'cat we'll turn off the headlights and taillights, but we _do_ want
	//them bolted on

	// light beams
	if ( cBolteeBolted = SimpleModelAddBolt(ent,snowcatModelData[SNOWCATOBJ_SNOWCAT],"headlight_left",
			headlight,"to_headlight",NULL) )
	{
		GetSnowcatBody()->SetLeftHeadLightInst(cBolteeBolted);
		pInst = cBolteeBolted->GetInstPtr();
		if (pInst && (boltObj = pInst->GetGhoulObject()) )
		{
			GetSnowcatBody()->SetLeftHeadLightBolt(boltObj->FindPart("to_headlight"));
		}
	}
	if ( cBolteeBolted = SimpleModelAddBolt(ent,snowcatModelData[SNOWCATOBJ_SNOWCAT],"headlight_right",
			headlight,"to_headlight",NULL) )
	{
		GetSnowcatBody()->SetRightHeadLightInst(cBolteeBolted);
		pInst = cBolteeBolted->GetInstPtr();
		if (pInst && (boltObj = pInst->GetGhoulObject()) )
		{
			GetSnowcatBody()->SetRightHeadLightBolt(boltObj->FindPart("to_headlight"));
		}
	}
	/*
	// headlights
	SimpleModelAddObject(ent, "front_left");
	SimpleModelAddObject(ent, "front_right");
	// taillights
	SimpleModelAddObject(ent, "rear_left");
	SimpleModelAddObject(ent, "rear_right");
	// exhaust effects
	if (ent->ghoulInst && (boltObj = ent->ghoulInst->GetGhoulObject()) )
	{
		// left 
		if (cBolterBolt = boltObj->FindPart("exhuast_left")) // love Joe's spelling of exhaust
		{
			fxRunner.execContinualEffect("environ/kf_exhaust01", ent, cBolterBolt, 0);
		}
		// right 
		if (cBolterBolt = boltObj->FindPart("exhuast_right")) // love Joe's spelling of exhaust
		{
			fxRunner.execContinualEffect("environ/kf_exhaust01", ent, cBolterBolt, 0);
		}
	}
	*/
}

void generic_ghoul_snowcat_ai::Activate(edict_t &monster)
{
	isActive=true;
}

void generic_ghoul_snowcat_ai::AddBody(edict_t *monster)
{
	if (!monster)
	{
		return;
	}
	if (!body)
	{
		body = new body_snowcat(monster);
	}
}

int generic_ghoul_snowcat_ai::AimMainGun(vec3_t vTarget)
{
	return AimMachGun(vTarget);
}

bool generic_ghoul_snowcat_ai::AimMachGun(vec3_t vTarget)
{
	body_snowcat*					body = GetSnowcatBody();
	IGhoulInst					*gunInst = body->GetMachGunInst(),
								*turretInst = body->GetTurretInst();
	GhoulID						gunBolt = body->GetMachGunBolt(),
								turretBolt = NULL;
	boltonOrientation_c			boltonInfo;
	edict_t*					self = NULL;
	boltonOrientation_c::retCode_e ret1, ret2;

	if (!body ||
		!body->GetTankInst() ||
		!(self = body->GetTankEdict()) ||
		(game.playerSkills.getEnemyValue() == 0))
	{
		return false;
	}
	if (!turretInst || !turretInst->GetGhoulObject())
	{
		return false;
	}
	// turn the turret to face the player (only pivots horizontally)
	if (!gunInst || !gunBolt || !(turretBolt = turretInst->GetGhoulObject()->FindPart("dummy03")) )
	{
		return false;
	}
	boltonInfo.root = self;
	boltonInfo.boltonInst = turretInst;
	boltonInfo.boltonID = turretBolt;
	boltonInfo.parentInst = self->ghoulInst;
	VectorCopy(vTarget, boltonInfo.vTarget);
	boltonInfo.vTarget[2] += 20;
	boltonInfo.fMinPitch = 0;
	boltonInfo.fMaxPitch = 0;
	boltonInfo.fMinYaw = -4000;
	boltonInfo.fMaxYaw = 4000;
	boltonInfo.fMaxTurnSpeed = body->GetTurretSpeed();
	boltonInfo.bUsePitch = false;
	boltonInfo.bUseYaw = true;
	boltonInfo.bToRoot = true;
	ret1 = boltonInfo.OrientBolton();
	// determine min pitch based on turret facing
	if (boltonInfo.fRetYaw > -0.5 && boltonInfo.fRetYaw < 0.5)
	{
		boltonInfo.fMinPitch = -0.30;
	}
	else if (boltonInfo.fRetYaw > 1.0 && boltonInfo.fRetYaw < 1.65)
	{
		boltonInfo.fMinPitch = -0.2;
	}
	else if (boltonInfo.fRetYaw < -1.0 && boltonInfo.fRetYaw > -1.65)
	{
		boltonInfo.fMinPitch = -0.2;
	}
	else if (boltonInfo.fRetYaw < -2.72 || boltonInfo.fRetYaw > 2.72)
	{
		boltonInfo.fMinPitch = -0.10;
	}
	else
	{
		boltonInfo.fMinPitch = -0.1;
	}

	// turn the first mach gun itself (only pivots vertically)
	boltonInfo.boltonInst = gunInst;
	boltonInfo.boltonID = gunBolt;
	boltonInfo.parentInst = turretInst;
	boltonInfo.parentID = turretBolt;
//	boltonInfo.fMinPitch = -M_PI*0.035;
	boltonInfo.fMaxPitch = M_PI*0.25;
	boltonInfo.fMinYaw = 0;
	boltonInfo.fMaxYaw = 0;
	boltonInfo.fMaxTurnSpeed = body->GetTurretSpeed();
	boltonInfo.bUsePitch = true;
	boltonInfo.bUseYaw = false;
	boltonInfo.bToRoot = true;
	ret2 = boltonInfo.OrientBolton();
	// turn the second mach gun itself (only pivots vertically)
	gunInst = body->GetMachGun2Inst();
	gunBolt = body->GetMachGun2Bolt();
	boltonInfo.boltonInst = gunInst;
	boltonInfo.boltonID = gunBolt;
	boltonInfo.parentInst = turretInst;
	boltonInfo.parentID = turretBolt;
//	boltonInfo.fMinPitch = -M_PI*0.035;
	boltonInfo.fMaxPitch = M_PI*0.25;
	boltonInfo.fMinYaw = 0;
	boltonInfo.fMaxYaw = 0;
	boltonInfo.fMaxTurnSpeed = body->GetTurretSpeed();
	boltonInfo.bUsePitch = true;
	boltonInfo.bUseYaw = false;
	boltonInfo.bToRoot = true;
	ret2 = boltonInfo.OrientBolton();
	return ( (boltonOrientation_c::retCode_e::ret_TRUE == ret1) && (ret2 == ret1) );
}
/*
action_c *generic_ghoul_snowcat_ai::TankAction(decision_c *od, action_c *oa, ai_c* ai, mmove_t *newanim,
									int nCommand, vec3_t vPos, edict_t* target, float fArg)
{
	// uniquely identify each action generated by the script so we can poll it later in 
	//TankDoneEvent::Process() to find out when it's done
	m_ScriptActionCounter++;
	return new snowcat_action(od, oa, ai, newanim, nCommand, vPos, target, fArg, m_ScriptActionCounter);
}
*/
int generic_ghoul_snowcat_ai::GetCurrentActionID()
{
	if (current_action)
	{
		return ((snowcat_action*)(action_c*)current_action)->GetID();
	}
	else
	{
		return 0;
	}
}

int generic_ghoul_snowcat_ai::GetMostRecentlyAddedActionID()
{
	return m_ScriptActionCounter;
}

qboolean generic_ghoul_snowcat_ai::Damage (edict_t &monster, edict_t *inflictor, 
										edict_t *attacker, vec3_t dir, vec3_t point, 
										vec3_t origin, int damage, int knockback, 
										int dflags, int mod, float penetrate, float absorb)
{
	// fixme: centralize skill level stuff
	int			take;

	VectorNormalize(dir);

	take = damage;

	if (attacker && attacker->client && attacker->client->inv)
	{
		// knives can't hurt tanks, silly.
		if (SFW_KNIFE == attacker->client->inv->getCurWeaponType())
		{
			// make some pretty sparks, though
			vec3_t vNormal; // fake this
			VectorSubtract(attacker->s.origin, monster.s.origin, vNormal);
			fxRunner.exec("environ/wallspark", point);
			gi.sound (&monster, CHAN_VOICE, gi.soundindex(va("impact/surfs/metal%d.wav",gi.irand(1,3))), .6, ATTN_NORM, 0);
			return true;
		}
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
			monster.takedamage = DAMAGE_NO;
			if (monster.deadflag != DEAD_DEAD)
			{
				generic_ghoul_tank_ai::Die(monster, inflictor, attacker, damage, point);
			}
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

void generic_ghoul_snowcat_ai::Pain(edict_t &monster, edict_t *other, float kick, int damage)
{
	float	fHealthPercentage = (float)monster.health/(float)monster.max_health;

	if (GetSnowcatBody())
	{
		GetSnowcatBody()->UpdateSmoke(&monster, fHealthPercentage);
	}
}

void generic_ghoul_snowcat_ai::Die(edict_t &monster, edict_t *inflictor, edict_t *attacker,
								int damage, vec3_t point)
{
	vec3_t	forward,to_dam;
	if (monster.health < -999)
	{
		monster.health = -999;
	}

	if (monster.killtarget)
	{
		edict_t *t = NULL;
		while ((t = G_Find (t, FOFS(targetname), ent->killtarget)))
		{
			t->use(t, &monster, &monster);
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


/*		if (attacker && game.GameStats->CanBuy(attacker))
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
		}*/

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

	monster.s.effects |= EF_EXPLODING;
	FX_VehicleExplosion(monster.s.origin, 255);
	BlindingLight(monster.s.origin, 5000, 0.9, 0.5);
}

void generic_ghoul_snowcat_ai::SetCurrentActionNextThink(void	(*think)(edict_t *ent))
{
	if (current_action)
	{
		((snowcat_action*)(action_c*)current_action)->SetNextThink(think);
	}
}

/*QUAKED m_x_snowcat (1 .5 0) (-91 -66 -33) (91 66 58) INACTIVE

  Not much AI right now, but you can disable what's there with the INACTIVE flag.

--------SPAWNFLAGS----------
INACTIVE - just a snowcat. no AI.

--------KEYS------------
*/

void generic_snowcat_spawnnow (edict_t *self, char *subclass)
{
	bool bDeactivate = false;

	self->s.renderfx = RF_GHOUL;

	VectorSet (self->mins, -92,-67,-34);
	VectorSet (self->maxs,  92, 67, 58);

	if (self->spawnflags & SNOWCAT_INACTIVE)
	{
		bDeactivate = true;
		// set a new bbox, aligned with the snowcat (BBoxRotate will get called by SimpleModelInit2)
		VectorSet (self->mins, -91,-66,-33);
		VectorSet (self->maxs,  91, 66, 58);
	}
	self->spawnflags = SF_NOPUSH;
	self->flags	   |= FL_NO_KNOCKBACK;
	self->movetype	= MOVETYPE_DAN;
	self->solid		= SOLID_BBOX;
	self->takedamage= DAMAGE_YES;
	self->pain		= NULL;//CobraThink_Pain;
	self->die		= NULL;//CobraThink_Die;
	self->think		= NULL;//CobraThink_OnPad;
	self->nextthink = level.time + FRAMETIME;

	self->health = self->max_health = 1000;	

	gi.linkentity (self);


	self->ai = ai_c::Create(AI_SNOWCAT, self, "enemy/snowcat", subclass);//new generic_ghoul_heli_ai(self, subclass);

	if (self->ai)
	{
		body_snowcat* body = ((generic_ghoul_snowcat_ai*)(ai_public_c*)self->ai)->GetSnowcatBody();
		if (body)
		{
			body->Deactivate(bDeactivate);
		}
	}
}

void generic_snowcat_init (edict_t *self)
{
	// this is the last time we have to set the nextthink. from now on the ai
	//will take care of it.
	self->nextthink = level.time + FRAMETIME;
	self->ai->NewDecision(new base_decision(), self);
}

void SP_m_x_snowcat (edict_t *self)
{
	generic_snowcat_spawnnow(self,"snowcat");
	self->think = generic_snowcat_init;
	gi.soundindex("Ambient/Models/Tank/tanklp.wav");
	gi.effectindex("environ/helismoke");
	gi.effectindex("weapons/othermz/tank");
	gi.effectindex("weapons/world/rocketexplode");
	gi.effectindex("weapons/othermz/machinegun");
	gi.effectindex("environ/kf_exhaust01");
	gi.effectindex("environ/snowspray");
	gi.effectindex("environ/machgun_smoke2");
	// in case some moron attacks a tank with a knife
	gi.soundindex("impact/surfs/metal1.wav");
	gi.soundindex("impact/surfs/metal2.wav");
	gi.soundindex("impact/surfs/metal3.wav");
	gi.effectindex("environ/wallspark");
	game_ghoul.FindObject("effects/explosion", "explode80");
}

//
// snowcat helpers
//

bool generic_ghoul_snowcat_ai::SnowcatH_FaceCoords(edict_t *entity)
{
	body_snowcat*	body = GetSnowcatBody();
	vec3_t vDest, vDestDir, vDestAngles; 

	if (!body)
	{
		return false;
	}
	VectorCopy(body->m_vPos, vDest);
	VectorSubtract(vDest, ent->s.origin, vDestDir);
	vectoangles(vDestDir, vDestAngles);
	if (ent->s.angles[YAW] > 180)
	{
		ent->s.angles[YAW] -= 360;
	}

	float fMove = vDestAngles[YAW] - ent->s.angles[YAW];
	float fMaxYawSpeed = body->GetMaxYawSpeed();

	ent->s.angles[YAW] = anglemod(ent->s.angles[YAW]);		
	fMove = anglemod(fMove);

	if ( fabs(fMove) < 1 )
	{
		ent->s.angles[YAW] = vDestAngles[YAW];
		ent->avelocity[YAW] = 0;
		return true;
	}
	else
	{	

		if (fMove<180)
		{
			// turn to left...
			//
			ent->avelocity[YAW] = fMaxYawSpeed;
		}
		else
		{
			// turn to right...
			//
			ent->avelocity[YAW] = -fMaxYawSpeed;
		}
	}
	return false;
}

bool generic_ghoul_snowcat_ai::SnowcatH_MoveForward(edict_t *entity)
{
	// already facing our destination, so move toward it
	body_snowcat*	body = GetSnowcatBody();
	edict_t		*ent = NULL;
	trace_t		tr;
	vec3_t		vDest, vFwd;

	if (body && (ent = body->GetTankEdict()) )
	{
		VectorCopy(body->m_vPos, vDest);
		// am I really close to my dest?
		if (fabs(vDest[0] - ent->s.origin[0]) < 30 &&
			fabs(vDest[1] - ent->s.origin[1]) < 30 &&
			fabs(vDest[2] - ent->s.origin[2]) < 30)
		{
			// pretty close. stop the snowcat and stop the treads
			VectorClear(ent->velocity);
			body->SetTreads(false);
			return true;
		}
		gi.trace(ent->s.origin, ent->mins, ent->maxs, vDest, ent, MASK_SOLID, &tr);
		if (1 == tr.fraction)
		{ 
			// if we aren't moving yet, set our velocity
			if (ent->velocity[0] || ent->velocity[1] || ent->velocity[2]) 
			{
				// already moving
				return false;
			}
			else
			{
				// not moving yet, apply our velocity
				AngleVectors(ent->s.angles, vFwd, NULL, NULL);
				VectorNormalize(vFwd); // ouch
				VectorScale(vFwd, body->GetMaxFwdSpeed(), ent->velocity);
				// set the sequence on our treads
				body->SetTreads(true);
				return false;
			}
		}
		else
		{
			// sumthin's in the way. should probably do a huge amount of damage to it and just roll
			//through it
			body->SetTreads(false);
			return true;
		}
	}
	return true;
}

bool generic_ghoul_snowcat_ai::SnowcatH_AimCannon(edict_t *entity)
{
	body_snowcat*	body = GetSnowcatBody();

	if (body)
	{
		return AimMachGun(body->m_vPos);
	}
	return true;
}

bool generic_ghoul_snowcat_ai::SnowcatH_FireCannon(edict_t *entity)
{
	return SnowcatH_FireMachGun(entity);
}

bool generic_ghoul_snowcat_ai::SnowcatH_FireMachGun(edict_t *entity)
{
	// at this point our machine gun should already be pointing at our target, so
	//just fire the dumb thing
	vec3_t		vFireDir, vFirePos, vTemp;
	body_snowcat*	body = GetSnowcatBody();
	IGhoulInst	*instMachGun = NULL, *instMachGun2 = NULL;
	IGhoulObj	*obj = NULL, *obj2 = NULL;
	GhoulID		idEndOfMachGun = NULL_GhoulID, idEndOfMachGun2 = NULL_GhoulID;
	trace_t		tr;
	bool		bFire = true;

	if ( !body || 
		 !(instMachGun = body->GetMachGunInst()) || !(obj = instMachGun->GetGhoulObject()) ||
		 !(instMachGun2 = body->GetMachGun2Inst()) || !(obj2 = instMachGun2->GetGhoulObject()) )
	{
		return true;
	}
	idEndOfMachGun = obj->FindPart("_flash1");
	idEndOfMachGun2 = obj2->FindPart("_flash1");
	GetGhoulPosDir2(entity->s.origin, ent->s.angles, instMachGun, idEndOfMachGun, NULL,
		vFirePos, vFireDir, NULL, NULL);
	if (m_bCheckMachGunLOS)
	{
		gi.trace(vFirePos, NULL, NULL, m_vMachGunTarget, entity, MASK_SOLID, &tr);
		if (tr.fraction < 1.0)
		{
			GetGhoulPosDir2(entity->s.origin, ent->s.angles, instMachGun2, idEndOfMachGun2, NULL,
				vFirePos, vFireDir, NULL, NULL);
			gi.trace(vFirePos, NULL, NULL, m_vMachGunTarget, entity, MASK_SOLID, &tr);
			if (tr.fraction < 1.0)
			{
				bFire = false;
			}
		}
	}
	if (bFire)
	{
		bool bBursting = ((level.time - m_fLastBurst) < ( (gi.flrand(0.0, 0.2)*game.playerSkills.getEnemyValue()) + 0.2) );

		if ( ((level.time - m_fLastBurst) > 0) && bBursting ) // ~1.0-second bursts
		{
			float waver = (5 - game.playerSkills.getEnemyValue())*0.05;
			// perform the effect 
			if (true/*gi.irand(0, 3)*/)
			{
				fxRunner.execWithInst("weapons/othermz/machinegun", entity, instMachGun, idEndOfMachGun);
				fxRunner.execWithInst("weapons/othermz/machinegun", entity, instMachGun2, idEndOfMachGun2);
			}
			// do the damage
			VectorSet(vTemp, gi.flrand(-waver, waver), gi.flrand(-waver, waver), gi.flrand(-waver, waver));
			VectorAdd(vFireDir, vTemp, vFireDir);
			// fire mach gun 1
			weapons.attack(ATK_MACHINEGUN, entity, vFirePos, vFireDir);//same damage, but bigger impact sound
			// fire mach gun 2
			GetGhoulPosDir2(entity->s.origin, ent->s.angles, instMachGun2, idEndOfMachGun2, NULL,
				vFirePos, vFireDir, NULL, NULL);
			weapons.attack(ATK_MACHINEGUN, entity, vFirePos, vFireDir);//same damage, but bigger impact sound
		}
		else if (!bBursting)
		{	// time to reset our burst timer for a new ~0.5 second break (depending on skill) in the firing
			m_fLastBurst = level.time + (gi.flrand(0.3, 0.6) * (5 - game.playerSkills.getEnemyValue()));
		}
	}
	return true;
}

//
// snowcat action wrappers -- these get called from snowcat_actions, then they call the corresponding
//						member function for the ai
//

void SnowcatW_GotoCoords(edict_t* ent)
{
	generic_ghoul_snowcat_ai* ai = (generic_ghoul_snowcat_ai*)(ai_public_c*)ent->ai;
	ai->SetRetVal(ai->Snowcat_GotoCoords(ent));
}

void SnowcatW_FireCannonAtCoords(edict_t* ent)
{
	generic_ghoul_snowcat_ai* ai = (generic_ghoul_snowcat_ai*)(ai_public_c*)ent->ai;
	ai->SetRetVal(ai->Snowcat_FireCannonAtCoords(ent));
}

void SnowcatW_MachGunAuto(edict_t *ent)
{
	// wherever the player is, attack him with the machine gun ( or stop attacking him)
	generic_ghoul_snowcat_ai* ai = (generic_ghoul_snowcat_ai*)(ai_public_c*)ent->ai;
	body_snowcat*	body = NULL;

	if (ai && (body = ai->GetSnowcatBody()) )
	{
		ai->SetMachGunAuto(0 != body->m_fArg);
	}
	ai->SetRetVal(true);
}

//
// snowcat action functions
//

bool generic_ghoul_snowcat_ai::Snowcat_GotoCoords(edict_t *entity)
{
	body_snowcat*	body = GetSnowcatBody();
	trace_t		tr;
	vec3_t		vDest;
	edict_t		*ent = NULL;

	if (body && (ent = body->GetTankEdict()) )
	{
		VectorCopy(body->m_vPos, vDest);
		gi.trace(ent->s.origin, ent->mins, ent->maxs, vDest, ent, MASK_SOLID, &tr);
		if (1 == tr.fraction)
		{ 
			// turn to face our destination
			if (SnowcatH_FaceCoords(entity))
			{
				// head toward destination
				if (SnowcatH_MoveForward(entity))
				{
					// reached destination
					return true;
				}
				else
				{
					// still moving toward destination
				}
			}
			else
			{
				// still turning toward our destination
			}
		}
		return false;
	}
	return true;
}

bool generic_ghoul_snowcat_ai::Snowcat_FireCannonAtCoords(edict_t *entity)
{
	body_snowcat*	body = GetSnowcatBody();

	if (body)
	{
		if (SnowcatH_AimCannon(entity))
		{
			return SnowcatH_FireCannon(entity);
		}
		return false;
	}
	return true;
}

bool generic_ghoul_snowcat_ai::Snowcat_MachGunAuto(edict_t *entity)
{
	// wherever the player is, attack him with the machine gun
	body_snowcat*	body = GetSnowcatBody();
	edict_t		*target = NULL;

	if (body && body->IsMachGunFunctioning())
	{
		target = &g_edicts[1];
		if ( (!(target->flags & FL_NOTARGET)) && AimMachGun(target->s.origin))
		{
			// make sure the gun has LOS
			VectorCopy(target->s.origin, m_vMachGunTarget);
			SnowcatH_FireMachGun(entity);
			return true;
		}
		return false;
	}
	return true;
}

//
// end of snowcat_action stuff
//

generic_ghoul_snowcat_ai::generic_ghoul_snowcat_ai()
{
	m_ScriptActionCounter = -1;
	m_LastThinkingActionID = -1;
	m_attacker = NULL;
	m_bTimeToDie = false;
	VectorClear(m_vCannonTarget);
	m_bMachGunAuto = false;
	m_bCheckMachGunLOS = true;
	m_bRetVal = false;
	m_fLastBurst = 0;
	VectorClear(m_vMachGunTarget);
}

generic_ghoul_snowcat_ai::generic_ghoul_snowcat_ai(generic_ghoul_snowcat_ai *orig)
{
	m_fLastBurst = orig->m_fLastBurst;
}

