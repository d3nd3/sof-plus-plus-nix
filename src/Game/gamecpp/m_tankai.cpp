// m_tankai.cpp

#include "g_local.h"
#include "ai_private.h"
#include "m_generic.h"
#include "m_tankai.h"
#include "g_obj.h"			// for OrientBolton() and modelSpawnData stuff
#include "callback.h"
#include "..\qcommon\ef_flags.h"

#define TANK_INACTIVE		(1<<0)

typedef enum
{
	TANKOBJ_TANK = 0,
	TANKOBJ_TURRET,
	TANKOBJ_CANNON,
	TANKOBJ_NULL,
	TANKOBJ_MACHGUN,
	TANKOBJ_MAXOBJS
};

enum
{
	MAINGUN_FALSE = 0,
	MAINGUN_TRUE,
	MAINGUN_ERROR
};

void debug_drawbox(edict_t* self,vec3_t vOrigin, vec3_t vMins, vec3_t vMaxs, int nColor);

modelSpawnData_t tankModelData[MD_TANK_SIZE] =
{	
//      dir					file			surfaceType			material			health				solid			material file      cnt  scale
"enemy/tank",				"tank",			SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		"tank",				0,	DEBRIS_SM,	NULL,	// TANKOBJ_TANK
"enemy/tank",				"turret",		SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// TANKOBJ_TURRET
"enemy/tank",				"canon",		SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// TANKOBJ_CANON
"objects/generic/gun_auto",	"null",			SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// TANKOBJ_NULL
"enemy/tank",				"machine_gun",	SURF_METAL,			MAT_METAL_DGREY,	100,				SOLID_BBOX,		NULL,				0,	DEBRIS_SM,	NULL,	// TANKOBJ_MACHGUN
};

TankTreadCallback theTankTreadCallback;

bool TankTreadCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	edict_t* self = (edict_t*)ent;
	generic_ghoul_tank_ai* ai = (generic_ghoul_tank_ai*)(ai_public_c*)self->ai;
	body_tank* body = NULL;

	if (ai)
	{
		body = ai->GetTankBody();
	}
	if (!body)
	{
		return false;
	}
	if (body->GetTreadFire())
	{
		// we're at the end of a firing sequence for the tank body, so switch
		//back to the appropriate non-firing tread sequence (slightly complicated
		//by the rotating texture coords for the treads)
		body->SetTreadFire(false);
		if (body->GetTreads())
		{
			SimpleModelSetSequence2(body->GetTankInst(), "tank", SMSEQ_LOOP);
		}
		else
		{
			SimpleModelSetSequence2(body->GetTankInst(), "tank", SMSEQ_HOLDFRAME);
		}
	}
	return true;
}

int GetGhoulPosDir2(vec3_t sourcePos, vec3_t sourceAng, IGhoulInst *inst,
					   GhoulID partID, char *name, vec3_t pos, vec3_t dir, vec3_t right,
					   vec3_t up);


static void FinalTankDeathFX(edict_t &monster, body_tank *tank)
{
	IGhoulObj	*obj = (tank->GetTankInst()?tank->GetTankInst()->GetGhoulObject():NULL);
	GhoulID		smoke1 = (obj?obj->FindPart("bolt_turret"):NULL);

	// stop the tank
	VectorClear(monster.velocity);
	VectorClear(monster.avelocity);

	FX_VehicleExplosion(monster.s.origin, 255);
	BlindingLight(monster.s.origin, 5000, 0.9, 0.5);

	// stop treads
	tank->SetTreads(false);

	// turn off main turret, cannon, and machine gun
	SimpleModelTurnOnOff(tank->GetTurretInst(), false);
	SimpleModelTurnOnOff(tank->GetCannonInst(), false);
	SimpleModelTurnOnOff(tank->GetMachGunInst(), false);

	// if this is a snowcat, turn off the other mach gun barrel
	if (tank->GetClassCode() == BODY_SNOWCAT)
	{
		SimpleModelTurnOnOff( ((body_snowcat*)tank)->GetMachGun2Inst(), false);
		((body_snowcat*)tank)->SetTreads(false);
	}

	// switch to damage skin
	SetSkin2(tank->GetTankInst(), "enemy/tank", "tank", "tank","tank_d");

	// toss some debris
	FX_ThrowDebris(monster.s.origin, vec3_up, 30, DEBRIS_MED, MAT_METAL_DGREY, 0, 0, 0, SURF_METAL);

	// smoke 'em if you got 'em
	fxRunner.execContinualEffect("environ/helismoke", &monster, smoke1);
	fxRunner.editContinualEffect("environ/helismoke", &monster, smoke1, 1.5);

	// no more thinking for you
	tank->Deactivate(true);
}

generic_ghoul_tank_ai::~generic_ghoul_tank_ai()
{
}

void generic_ghoul_tank_ai::Think(edict_t &monster)
{
	if (ai_freeze->value)
	{
		return;
	}

//	gi.dprintf("tank health = %3.2f percent\n", (float)monster.health*100/(float)monster.max_health);
//	debug_drawbox(&monster, NULL, NULL, NULL, 0);
	if (GetTankBody() && GetTankBody()->IsDeactivated())
	{
		// deactivate
//		monster.nextthink = 0;
		GetTankBody()->SetTreads(false);
		monster.s.sound = 0;
		monster.s.sound_data = 0;
		return;
	}

	// if our machine gun has been set to auto attack...
	if (m_bMachGunAuto)
	{
		Tank_MachGunAuto(&monster);
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
		m_LastThinkingActionID = ((tank_action*)(action_c*)current_action)->GetID();
		if (current_action->Think(*this, monster))
		{ 
			NextAction(monster);
			if (GetTankBody())
			{
				GetTankBody()->ResetScriptParams();
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

void generic_ghoul_tank_ai::Init(edict_t *self, char *ghoulname, char* subclass)
{
	m_ScriptActionCounter = m_LastThinkingActionID = 0;
	m_bTimeToDie = false;
	m_attacker = NULL;
	m_bMachGunAuto = false;
	m_bMoveBackward = false;
	m_bCheckMachGunLOS = true;
	m_bRetVal = false;
	// build the tank here

	// tank (root object)
	ggObjC *cTank = NULL;
	// turret, cannon, machine gun
	ggObjC	*cTurret = NULL,
			*cCannon = NULL,
			*cNull = NULL,
			*cMachGun = NULL;
	// anims for fuselage and boltons
	GhoulID	cTurretSeq=0,
			cTankSeq=0,
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

	if (!GetTankBody())
	{
		return;
	}
	// create the tank body

	// gotta remove this flag or SimpleModelInit2 bombs out
	ent->s.renderfx &= ~RF_GHOUL;
	SimpleModelInit2(ent,&tankModelData[TANKOBJ_TANK],NULL,NULL);
	if (ent->ghoulInst)
	{
		ent->movetype = MOVETYPE_PUSH;
		GetTankBody()->SetTankEdict(ent);
		SimpleModelSetSequence2(GetTankBody()->GetTankInst(), "tank", SMSEQ_HOLDFRAME);
	}

	// bolt on the turret
	if ( cBolteeBolted = SimpleModelAddBolt(ent, tankModelData[TANKOBJ_TANK], "bolt_turret", 
			tankModelData[TANKOBJ_TURRET], "to_bolt_turret", NULL) )
	{
		bInstTurret = cBolteeBolted;
		pInst = cBolteeBolted->GetInstPtr();
		GetTankBody()->SetTurretInst(cBolteeBolted);

		if (pInst && (boltObj = pInst->GetGhoulObject()) )
		{
			GetTankBody()->SetTurretBolt(boltObj->FindPart("to_bolt_turret"));
			// register a callback for the tank tread sequence
			tempNote=pInst->GetGhoulObject()->FindNoteToken("EOS");
			if (tempNote)
			{
				pInst->AddNoteCallBack(&theTankTreadCallback,tempNote);
			}
		}
	}

	// bolt the cannon onto the turret
	if (cBolteeBolted = ComplexModelAddBolt(bInstTurret, tankModelData[TANKOBJ_TURRET],
					"bolt_canon", tankModelData[TANKOBJ_CANNON], "to_bolt_canon", NULL))
	{
		pInst = cBolteeBolted->GetInstPtr();
		GetTankBody()->SetCannonInst(cBolteeBolted);
	}
	if (pInst && pInst->GetGhoulObject())
	{
		GetTankBody()->SetCannonBolt(pInst->GetGhoulObject()->FindPart("to_bolt_canon"));
	}

	// bolt the machine gun base (just a null) onto the turret
	if (cBolteeBolted = ComplexModelAddBolt(bInstTurret, tankModelData[TANKOBJ_TURRET],
					"bolt_machine_gun", tankModelData[TANKOBJ_NULL], "DUMMY01", NULL))
	{
		bInstMachGunBase = cBolteeBolted;
		pInst = bInstMachGunBase->GetInstPtr();
		GetTankBody()->SetMachGunNullInst(bInstMachGunBase);
		if (pInst && pInst->GetGhoulObject())
		{
			GetTankBody()->SetMachGunNullBolt(pInst->GetGhoulObject()->FindPart("DUMMY01"));
			// the NULL actually has some geometry in it (ghoul made me do it :< ) so turn it off
			SimpleModelTurnOnOff(pInst, false);
		}
	}

	// bolt the machine gun onto its base
	if (cBolteeBolted = ComplexModelAddBolt(bInstMachGunBase, tankModelData[TANKOBJ_NULL],
					"DUMMY01", tankModelData[TANKOBJ_MACHGUN], "to_bolt_machine_gun", NULL))
	{
		GetTankBody()->SetMachGunInst(cBolteeBolted);
		pInst = cBolteeBolted->GetInstPtr();
		if (pInst && pInst->GetGhoulObject())
		{
			GetTankBody()->SetMachGunBolt(pInst->GetGhoulObject()->FindPart("to_bolt_machine_gun"));
		}
	}
}

void generic_ghoul_tank_ai::Activate(edict_t &monster)
{
	isActive=true;
}

void generic_ghoul_tank_ai::AddBody(edict_t *monster)
{
	if (!monster)
	{
		return;
	}
	if (!body)
	{
		body = new body_tank(monster);
	}
}

int generic_ghoul_tank_ai::AimMainGun(vec3_t vTarget)
{
	body_tank*					body = GetTankBody();
	IGhoulInst*					gunInst = body->GetTurretInst();
	GhoulID						gunBolt = body->GetTurretBolt();
	boltonOrientation_c			boltonInfo;
	edict_t*					self = NULL;
	boltonOrientation_c::retCode_e ret1, ret2;

	if (!body || !body->GetTankInst() || !(self = (edict_t*)body->GetTankInst()->GetUserData()) )
	{
		return MAINGUN_ERROR;
	}
	if (!gunInst || !gunInst->GetGhoulObject())
	{
		return MAINGUN_ERROR;
	}

	// turn the turret to face the player (only pivots horizontally)

	boltonInfo.root = self;
	boltonInfo.boltonInst = gunInst;
	boltonInfo.boltonID = gunBolt;
	boltonInfo.parentInst = self->ghoulInst;
	VectorCopy(vTarget, boltonInfo.vTarget);
	boltonInfo.vTarget[2] += 12;
	boltonInfo.fMinPitch = 0;
	boltonInfo.fMaxPitch = 0;
	boltonInfo.fMinYaw = -4000;
	boltonInfo.fMaxYaw = 4000;
	boltonInfo.fMaxTurnSpeed = body->GetTurretSpeed();
	boltonInfo.bUsePitch = false;
	boltonInfo.bUseYaw = true;
	boltonInfo.bToRoot = true;

	ret1 = boltonInfo.OrientBolton();

	// turn the cannon to face the player (only pivots vertically)
	boltonInfo.boltonInst = body->GetCannonInst();
	boltonInfo.boltonID = body->GetCannonBolt();
	boltonInfo.parentInst = gunInst;
	boltonInfo.parentID = gunInst->GetGhoulObject()->FindPart("to_bolt_turret");
	// determine min yaw based on turret facing
	if (boltonInfo.fRetYaw > -0.13 && boltonInfo.fRetYaw < 0.13)
	{
		boltonInfo.fMinPitch = -0.05;
	}
	else if (boltonInfo.fRetYaw > 0.8 && boltonInfo.fRetYaw < 2.4)
	{
		boltonInfo.fMinPitch = -0.05;
	}
	else if (boltonInfo.fRetYaw < -0.8 && boltonInfo.fRetYaw > -2.4)
	{
		boltonInfo.fMinPitch = -0.05;
	}
	else
	{
		boltonInfo.fMinPitch = -0.02;
	}
	
	boltonInfo.fMaxPitch = .2;
	boltonInfo.fMinYaw = -4000;
	boltonInfo.fMaxYaw = 4000;
	boltonInfo.fMaxTurnSpeed = body->GetCannonSpeed();
	boltonInfo.bUsePitch = true;
	boltonInfo.bUseYaw = false;
	boltonInfo.bToRoot = true;
	ret2 = boltonInfo.OrientBolton();
	if ( (boltonOrientation_c::retCode_e::ret_TRUE == ret1) && (ret2 == ret1) )
	{	// pointing at our target
		return MAINGUN_TRUE;
	}
	else if (	(boltonOrientation_c::retCode_e::ret_ERROR == ret1 && boltonOrientation_c::retCode_e::ret_TRUE == ret2) ||
				(boltonOrientation_c::retCode_e::ret_ERROR == ret2 && boltonOrientation_c::retCode_e::ret_TRUE == ret1) ||
				(boltonOrientation_c::retCode_e::ret_ERROR == ret1 && boltonOrientation_c::retCode_e::ret_ERROR == ret2) )
	{	// either the pitch or the yaw is outside of our range of motion. don't return error until we've 
		//gotten as close as we can.
		return MAINGUN_ERROR;
	}
	else
	{	// we just plain haven't got there yet
		return MAINGUN_FALSE;
	}
}

bool generic_ghoul_tank_ai::AimMachGun(vec3_t vTarget)
{
	body_tank*					body = GetTankBody();
	IGhoulInst					*gunInst = body->GetMachGunInst(),
								*baseInst = body->GetMachGunNullInst(),
								*turretInst = body->GetTurretInst();
	GhoulID						gunBolt = body->GetMachGunBolt(),
								baseBolt = body->GetMachGunNullBolt();
	boltonOrientation_c			boltonInfo;
	edict_t*					self = NULL;
	boltonOrientation_c::retCode_e ret1, ret2;

	if (!body ||
		!body->GetTankInst() ||
		!(self = (edict_t*)body->GetTankInst()->GetUserData()) ||
		(game.playerSkills.getEnemyValue() == 0))
	{
		return false;
	}
	if (!turretInst || !turretInst->GetGhoulObject())
	{
		return false;
	}
	// turn the mach gun's base (just a null) to face the player (only pivots horizontally)
	if (!gunInst || !gunBolt || !baseInst || !baseBolt)
	{
		return false;
	}
	boltonInfo.root = self;
	boltonInfo.boltonInst = baseInst;
	boltonInfo.boltonID = baseBolt;
	boltonInfo.parentInst = turretInst;
	boltonInfo.parentID = turretInst->GetGhoulObject()->FindPart("to_bolt_turret");
	VectorCopy(vTarget, boltonInfo.vTarget);
	boltonInfo.vTarget[2] += 20;
	boltonInfo.fMinPitch = 0;
	boltonInfo.fMaxPitch = 0;
	boltonInfo.fMinYaw = -4000;
	boltonInfo.fMaxYaw = 4000;
	boltonInfo.fMaxTurnSpeed = body->GetMachGunSpeed();
	boltonInfo.bUsePitch = false;
	boltonInfo.bUseYaw = true;
	boltonInfo.bToRoot = true;
	ret1 = boltonInfo.OrientBolton();
	// turn the mach gun itself (only pivots vertically)
	boltonInfo.boltonInst = gunInst;
	boltonInfo.boltonID = gunBolt;
	boltonInfo.parentInst = baseInst;
	boltonInfo.parentID = baseBolt;
	boltonInfo.fMinPitch = -M_PI*0.10;
	boltonInfo.fMaxPitch = M_PI*0.25;
	boltonInfo.fMinYaw = 0;
	boltonInfo.fMaxYaw = 0;
	boltonInfo.fMaxTurnSpeed = body->GetMachGunSpeed();
	boltonInfo.bUsePitch = true;
	boltonInfo.bUseYaw = false;
	boltonInfo.bToRoot = true;
	ret2 = boltonInfo.OrientBolton();
	return ( (boltonOrientation_c::retCode_e::ret_TRUE == ret1) && (ret2 == ret1) );
}

action_c *generic_ghoul_tank_ai::TankAction(decision_c *od, action_c *oa,
	ai_c* ai, mmove_t *newanim, int nCommand, vec3_t vPos, edict_t* target, float fArg)
{
	// uniquely identify each action generated by the script so we can poll it later in 
	//HelicopterDoneEvent::Process() to find out when it's done
	m_ScriptActionCounter++;
	return new tank_action(od, oa, ai, newanim, nCommand, vPos, target, fArg, m_ScriptActionCounter);
}

int generic_ghoul_tank_ai::GetCurrentActionID()
{
	if (current_action)
	{
		return ((tank_action*)(action_c*)current_action)->GetID();
	}
	else
	{
		return 0;
	}
}

int generic_ghoul_tank_ai::GetMostRecentlyAddedActionID()
{
	return m_ScriptActionCounter;
}

qboolean generic_ghoul_tank_ai::Damage (edict_t &monster, edict_t *inflictor, 
										edict_t *attacker, vec3_t dir, vec3_t point, 
										vec3_t origin, int damage, int knockback, 
										int dflags, int mod, float penetrate, float absorb)
{
	// fixme: centralize skill level stuff
	int			take;
	edict_t		*killTarget = NULL;

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
			if (monster.killtarget)
			{
				while ((killTarget = G_Find (killTarget, FOFS(targetname), monster.killtarget)))
				{
					killTarget->use(killTarget, &monster, &monster);
				}
			}
			else
			{
				m_attacker = attacker;
				monster.flags |= FL_NO_KNOCKBACK;
//				monster.takedamage = DAMAGE_NO;
				if (monster.deadflag != DEAD_DEAD)
				{
					Die(monster, inflictor, attacker, damage, point);
				}
				monster.nextthink = level.time + FRAMETIME;	// should break heli out of PAUSE command if in one, but only private heli pause, not script main pause (unless that breaks out when you get shot anyway)
			}
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

void generic_ghoul_tank_ai::Pain(edict_t &monster, edict_t *other, float kick, int damage)
{
	float	fHealthPercentage = (float)monster.health/(float)monster.max_health;

	if (GetTankBody())
	{
		GetTankBody()->UpdateSmoke(&monster, fHealthPercentage);
	}
}

void generic_ghoul_tank_ai::Die(edict_t &monster, edict_t *inflictor, edict_t *attacker,
								int damage, vec3_t point)
{
	vec3_t	forward,to_dam;
	if (monster.health < -999)
	{
		monster.health = -999;
	}


	// make sure our movement sound is off
	monster.s.sound = 0;
	monster.s.sound_data = 0;

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

//	monster.s.effects |= EF_EXPLODING;
	if (GetTankBody())
	{
		FinalTankDeathFX(monster, GetTankBody());
	}
}

void generic_ghoul_tank_ai::SetCurrentActionNextThink(void	(*think)(edict_t *ent))
{
	if (current_action)
	{
		((tank_action*)(action_c*)current_action)->SetNextThink(think);
	}
}

void tanktouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t		up = {0,0,1};
	vec3_t		point;
	vec3_t		pushVel;

	if ((other->s.origin[2] + other->mins[2]) > self->s.origin[2])
	{
		VectorSet(point, other->s.origin[0], other->s.origin[1], other->absmin[2]);
		T_Damage(other, self, self, up, point, point, 10, 100, DT_SHOTGUN, 0);
		VectorSubtract(other->s.origin, self->s.origin, pushVel);
		VectorNormalize(pushVel);
		VectorScale(pushVel, 100, pushVel);
		if (pushVel[2] < 10)
		{
			pushVel[2] = 10;
		}
		VectorAdd(other->velocity, pushVel, other->velocity);
	}
}

void TankUse(edict_t *self, edict_t *other, edict_t *activator)
{
	bool		bDeactivate = false;

	// toggle the tank on and off
	if (self->spawnflags & TANK_INACTIVE)
	{
		self->spawnflags &= ~TANK_INACTIVE;
	}
	else
	{
		self->spawnflags |= TANK_INACTIVE;
		bDeactivate = true;
	}
	if (self->ai)
	{
		body_tank* body = ((generic_ghoul_tank_ai*)(ai_public_c*)self->ai)->GetTankBody();
		if (body)
		{
			body->Deactivate(bDeactivate);
		}
	}
}

/*QUAKED m_x_tank (1 .5 0) (-152 -85 -44) (152 85 80) INACTIVE

  Not much AI right now, but you can disable what's there with the INACTIVE flag.

--------SPAWNFLAGS----------
INACTIVE - just a tank. no AI.

--------KEYS------------
*/


void generic_tank_spawnnow (edict_t *self, char *subclass)
{
	bool bDeactivate = false;

	self->s.renderfx = RF_GHOUL;

	VectorSet (self->mins, -152,-85,-44);
	VectorSet (self->maxs,  152, 85, 80);

	if (self->spawnflags & TANK_INACTIVE)
	{
		bDeactivate = true;
		// set a new bbox, aligned with the tank (BBoxRotate will get called by SimpleModelInit2)
//		VectorSet (self->mins, -154,-78,-44);
//		VectorSet (self->maxs,  267, 78, 24);
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
	self->use		= TankUse;					// get it? Tank Use? Like, tank use very much?
	self->touch		= tanktouch;

	self->health = self->max_health = 500 + (game.playerSkills.getEnemyValue()*500);	

	gi.linkentity (self);


	self->ai = ai_c::Create(AI_TANK, self, "enemy/tank", subclass);//new generic_ghoul_heli_ai(self, subclass);

	if (self->ai)
	{
		body_tank* body = ((generic_ghoul_tank_ai*)(ai_public_c*)self->ai)->GetTankBody();
		if (body)
		{
			body->Deactivate(bDeactivate);
		}
	}
}

void generic_tank_init (edict_t *self)
{
	// this is the last time we have to set the nextthink. from now on the ai
	//will take care of it.
	self->nextthink = level.time + FRAMETIME;
	self->ai->NewDecision(new base_decision(), self);
}

void SP_m_x_tank (edict_t *self)
{
	generic_tank_spawnnow(self,"tank");
	self->think = generic_tank_init;

	gi.soundindex("Ambient/Models/Tank/tanklp.wav");

	// in case some moron attacks a tank with a knife
	gi.soundindex("impact/surfs/metal1.wav");
	gi.soundindex("impact/surfs/metal2.wav");
	gi.soundindex("impact/surfs/metal3.wav");
	gi.effectindex("environ/wallspark");

	gi.effectindex("environ/helismoke");
	gi.effectindex("weapons/othermz/tank");
	gi.effectindex("weapons/world/rocketexplode");
	gi.effectindex("weapons/othermz/machinegun");
	gi.effectindex("environ/machgun_smoke2");
	game_ghoul.FindObject("effects/explosion", "explode80");
}


//
// tank helpers
//

bool generic_ghoul_tank_ai::TankH_FaceCoords(edict_t *entity)
{
	body_tank*	body = GetTankBody();
	vec3_t vDest, vDestDir, vDestAngles; 

	if (!body)
	{
		return false;
	}
	VectorCopy(body->m_vPos, vDest);
	VectorSubtract(vDest, ent->s.origin, vDestDir);
	if (m_bMoveBackward)
	{	//	negate direction but leave z alone
		vDestDir[0] = -vDestDir[0];
		vDestDir[1] = -vDestDir[1];
	}
	vectoangles(vDestDir, vDestAngles);
	if (ent->s.angles[YAW] > 180)
	{
		ent->s.angles[YAW] -= 360;
	}

	float fMove = vDestAngles[YAW] - ent->s.angles[YAW];
	float fAbsMove = 0;
	float fMaxYawSpeed = body->GetMaxYawSpeed();

	ent->s.angles[YAW] = anglemod(ent->s.angles[YAW]);		
	fAbsMove = fabs(fMove);
	fMove = anglemod(fMove);

	if ( fAbsMove < 1.5 )
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

bool generic_ghoul_tank_ai::TankH_MoveForward(edict_t *entity)
{
	// already facing our destination, so move toward it
	body_tank*	body = GetTankBody();
	edict_t		*ent = NULL;
	trace_t		tr;
	vec3_t		vDest, vFwd;

	if (body && (ent = body->GetTankEdict()) )
	{
		VectorCopy(body->m_vPos, vDest);
		// am I really close to my dest?
		if (fabs(vDest[0] - ent->s.origin[0]) < 5 &&
			fabs(vDest[1] - ent->s.origin[1]) < 5)// &&
			//fabs(vDest[2] - ent->s.origin[2]) < 5)
		{
			// pretty close. stop the tank and stop the treads
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
				if (m_bMoveBackward)
				{
					VectorInverse(ent->velocity);
				}
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

bool generic_ghoul_tank_ai::TankH_AimCannon(edict_t *entity)
{
	body_tank*	body = GetTankBody();
	int			aimRet = MAINGUN_ERROR;

	if (body)
	{
		aimRet = AimMainGun(body->m_vPos);
		if ( (aimRet == MAINGUN_TRUE) || (aimRet == MAINGUN_ERROR) )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool generic_ghoul_tank_ai::TankH_FireCannon(edict_t *entity)
{
	body_tank*	body = GetTankBody();
	IGhoulInst	*instCannon = NULL;
	IGhoulObj	*obj = NULL;
	GhoulID		idEndOfCannon = 0;

	// fire!
	if (body && (instCannon = body->GetCannonInst()) )
	{
		if (obj = instCannon->GetGhoulObject())
		{
			// play a nice firing sound
			FX_LargeExplosion(entity->s.origin, 1/*doesn't matter for sound-only effect*/, 2/*2 == sound only*/);
			// perform the visual effect
			idEndOfCannon = obj->FindPart("flash_canon");
			fxRunner.execWithInst("weapons/othermz/tank", body->GetTankEdict(), 
				instCannon, idEndOfCannon);
			// play the recoil sequence for the cannon
			SimpleModelSetSequence2(instCannon, "canon", SMSEQ_HOLD);
			// play the recoil sequence for the turret
			SimpleModelSetSequence2(body->GetTurretInst(), "turret", SMSEQ_HOLD);
			// play the recoil sequence for the tank body
			if (body->GetTreads())
			{
				body->SetTreadFire(true);
				SimpleModelSetSequence2(body->GetTankInst(), "tank_fire", SMSEQ_HOLD);
			}
			else
			{
				body->SetTreadFire(true);
				SimpleModelSetSequence2(body->GetTankInst(), "tank_still", SMSEQ_HOLD);
			}
			// start the attack
			if (TankH_CannonAttack(entity))
			{
				// finished the attack
				return true;
			}
			else
			{
				// need to continue the attack
				SetCurrentActionNextThink(TankW_CannonAttack);
				return false;
			}
		}
	}

	return true;
}

bool generic_ghoul_tank_ai::TankH_CannonAttack(edict_t *entity)
{
	// the raison d'etre for this fn is that we want to depict the cannon firing as a
	//projectile weapon (as opposed to an instantaneous weapon like a pistol) but we don't want to 
	//show a moving projectile (like the rocket launcher does)
	vec3_t		vFireDir, vFirePos, vEndPos;
	body_tank*	body = GetTankBody();
	edict_t		*ent = NULL;
	IGhoulInst	*instCannon = NULL;
	IGhoulObj	*obj = NULL;
	GhoulID		idEndOfCannon = 0;
	trace_t		tr;
	float		fProjectileDist = 0;

	if (!body || !(instCannon = body->GetCannonInst()) || !(obj = instCannon->GetGhoulObject()) ||
		!(ent = body->GetTankEdict()) )
	{
		body->SetCurProjectileDist(0); // just to make sure
		return true;
	}

	// have we launched our shell yet?
	if ( !(fProjectileDist = body->GetCurProjectileDist()) )
	{
		// get id of bolt on the front of the cannon
		idEndOfCannon = obj->FindPart("flash_canon");

		// find range to target
		GetGhoulPosDir2(ent->s.origin, ent->s.angles,
			instCannon,
			idEndOfCannon,
			NULL, vFirePos, vFireDir, NULL, NULL);
		VectorMA(vFirePos, 4096, vFireDir, vEndPos);
		gi.trace(vFirePos, NULL, NULL, vEndPos, ent, MASK_SOLID, &tr);
		if (1 == tr.fraction)
		{
			// freakin' a. this thing just shot 4096 units and didn't hit a thing.
			return true;
		}
		fProjectileDist = tr.fraction * 4000;	// shave off a little to make sure we hit the correct
												//side of the architecture
		body->SetGroundZero(tr.endpos);
	}
	// wherever it hits, generate an explosion. if it hits something a fair distance away, though,
	//wait a frame or two before the damage occurs (projectile moves at 500 units a frame)
	if ( fProjectileDist > 500 )
	{
		// shell is still moving
		body->SetCurProjectileDist(fProjectileDist - 500);
		return false;
	}
	else
	{ // shell will land this frame
		body->SetCurProjectileDist(0);

		// damage! (stolen from MissileCollide())
		vec3_t	pos;
		edict_t	*tempXP = G_Spawn();

		body->GetGroundZero(pos);
		pos[2] += 10;
		tempXP->owner = ent;
		VectorCopy(pos, tempXP->s.origin);

		fxRunner.exec("weapons/world/rocketexplode", pos);

		T_RadiusDamage (tempXP, tempXP->owner, 130, tempXP, 150, 0, DT_MANGLE);
		gmonster.RadiusDeafen(tempXP, 150, 250);
		ShakeCameras (tempXP->s.origin, 100, 300, DEFAULT_JITTER_DELTA);
		G_FreeEdict(tempXP);

		return true;
	}
}

bool generic_ghoul_tank_ai::TankH_FireMachGun(edict_t *entity)
{
	// at this point our machine gun should already be pointing at our target, so
	//just fire the dumb thing
	vec3_t		vFireDir, vFirePos, vTemp;
	body_tank*	body = GetTankBody();
	IGhoulInst	*instMachGun = NULL;
	IGhoulObj	*obj = NULL;
	GhoulID		idEndOfMachGun = 0;
	trace_t		tr;
	bool		bFire = true;

	if ( !body || !(instMachGun = body->GetMachGunInst()) || !(obj = instMachGun->GetGhoulObject()) )
	{
		return true;
	}
	idEndOfMachGun = obj->FindPart("flash_machine_gun");
	GetGhoulPosDir2(entity->s.origin, ent->s.angles, instMachGun, idEndOfMachGun, NULL,
		vFirePos, vFireDir, NULL, NULL);
	if (m_bCheckMachGunLOS)
	{
		gi.trace(vFirePos, NULL, NULL, m_vMachGunTarget, entity, MASK_SOLID, &tr);
		if (tr.fraction < .95)
		{	// something's blocking our line of sight to our target. if it's an entity that's
			//not on our team, shoot anyway.
			if (tr.ent && (tr.ent != &g_edicts[0]))
			{
				if ( (NULL == (ai_public_c*)tr.ent->ai) || (!OnSameTeam(tr.ent, entity)) )
				{	// hit a non-ai entity (like maybe a crate) or an entity that's not on our team.
					//either way, shoot it.
					bFire = true;
				}
				else
				{	// I suppose it's conceivable that our trace hit an entity that's on our team. don't shoot.
					bFire = false;
				}
			}
			else
			{	// probably hit architecture. that'll stop a bullet.
				bFire = false;
			}
		}
	}
	if (bFire)
	{
		bool bBursting = ((level.time - m_fLastBurst) < ( (gi.flrand(0.0, 0.2)*game.playerSkills.getEnemyValue()) + 0.2) );
		// perform the effect 
		if (((level.time - m_fLastBurst) > 0) && bBursting)
		{
			float waver = (5 - game.playerSkills.getEnemyValue())*0.03;
			fxRunner.execWithInst("weapons/othermz/machinegun", entity, instMachGun, idEndOfMachGun);
			VectorSet(vTemp, gi.flrand(-waver, waver), gi.flrand(-waver, waver), gi.flrand(-waver, waver));
			VectorAdd(vFireDir, vTemp, vFireDir);
			// do the damage
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
// tank action wrappers -- these get called from tank_actions, then they call the corresponding
//						member function for the ai
//

void TankW_GotoCoords(edict_t* ent)
{
	generic_ghoul_tank_ai* ai = (generic_ghoul_tank_ai*)(ai_public_c*)ent->ai;
	body_tank*	body = NULL;

	if (ai && (body = ai->GetTankBody()) )
	{
		ai->MoveBackward(body->m_fArg == 1.0);
	}
	ai->SetRetVal(ai->Tank_GotoCoords(ent));
}

void TankW_FireCannonAtCoords(edict_t* ent)
{
	generic_ghoul_tank_ai* ai = (generic_ghoul_tank_ai*)(ai_public_c*)ent->ai;
	ai->SetRetVal(ai->Tank_FireCannonAtCoords(ent));
}

void TankW_CannonAttack(edict_t *ent)
{
	generic_ghoul_tank_ai* ai = (generic_ghoul_tank_ai*)(ai_public_c*)ent->ai;
	ai->SetRetVal(ai->TankH_CannonAttack(ent));
}

void TankW_MachGunAuto(edict_t *ent)
{
	// wherever the player is, attack him with the machine gun ( or stop attacking him)
	generic_ghoul_tank_ai* ai = (generic_ghoul_tank_ai*)(ai_public_c*)ent->ai;
	body_tank*	body = NULL;

	if (ai && (body = ai->GetTankBody()) )
	{
		ai->SetMachGunAuto(0 != body->m_fArg);
	}
	ai->SetRetVal(true);
}

void TankW_Die(edict_t *ent)
{
	// perform some spectacular exploding, dying, blammo effects
	generic_ghoul_tank_ai* ai = (generic_ghoul_tank_ai*)(ai_public_c*)ent->ai;

	if (ai)
	{
		ai->Die(*ent, NULL, ent, 10000, vec3_origin);
	}
	ai->SetRetVal(true);
}

void TankW_AimTurret(edict_t *ent)
{
	generic_ghoul_tank_ai* ai = (generic_ghoul_tank_ai*)(ai_public_c*)ent->ai;
	ai->SetRetVal(ai->TankH_AimCannon(ent));
}

//
// tank action functions
//

bool generic_ghoul_tank_ai::Tank_GotoCoords(edict_t *entity)
{
	body_tank*	body = GetTankBody();
	trace_t		tr;
	vec3_t		vDest;
	edict_t		*ent = NULL;

	if (body && (ent = body->GetTankEdict()) )
	{
		VectorCopy(body->m_vPos, vDest);
		gi.trace(ent->s.origin, ent->mins, ent->maxs, vDest, ent, MASK_SOLID, &tr);
		if (1 == tr.fraction)
		{ 
			// turn to face our destination. turn on our movement sound
			entity->s.sound =  gi.soundindex("Ambient/Models/Tank/tanklp.wav");
			entity->s.sound_data = (255 & ENT_VOL_MASK) | SND_FARATTN;
			if (TankH_FaceCoords(entity))
			{
				// head toward destination
				if (TankH_MoveForward(entity))
				{
					// reached destination. turn off our movement sound.
				///	entity->s.sound = 0;
				//	entity->s.sound_data = 0;
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
	entity->s.sound = 0;
	entity->s.sound_data = 0;
	return true;
}

bool generic_ghoul_tank_ai::Tank_FireCannonAtCoords(edict_t *entity)
{
	body_tank*	body = GetTankBody();

	if (body)
	{
		if (TankH_AimCannon(entity))
		{
			return TankH_FireCannon(entity);
		}
		return false;
	}
	return true;
}

bool generic_ghoul_tank_ai::Tank_MachGunAuto(edict_t *entity)
{
	// wherever the player is, attack him with the machine gun
	body_tank*	body = GetTankBody();
	edict_t		*target = NULL;

	if (body && body->IsMachGunFunctioning())
	{
		target = &g_edicts[1];
		if (AimMachGun(target->s.origin))
		{
			// make sure the gun has LOS
			VectorCopy(target->s.origin, m_vMachGunTarget);
			TankH_FireMachGun(entity);
			return true;
		}
		return false;
	}
	return true;
}

//
// end of tank_action stuff
//

generic_ghoul_tank_ai::generic_ghoul_tank_ai()
{
	m_ScriptActionCounter = -1;
	m_LastThinkingActionID = -1;
	m_attacker = NULL;
	m_bTimeToDie = false;
	m_bMoveBackward = false;
	VectorClear(m_vCannonTarget);
	m_bMachGunAuto = false;
	m_bCheckMachGunLOS = true;
	m_bRetVal = false;
	m_fLastBurst = 0;
	VectorClear(m_vMachGunTarget);
}

generic_ghoul_tank_ai::generic_ghoul_tank_ai(generic_ghoul_tank_ai *orig)
{
	m_ScriptActionCounter = orig->m_ScriptActionCounter;
	m_LastThinkingActionID = orig->m_LastThinkingActionID;
	*(int *)&m_attacker = GetEdictNum(orig->m_attacker);
	m_bTimeToDie = orig->m_bTimeToDie;
	m_bMoveBackward = orig->m_bMoveBackward;
	VectorCopy(orig->m_vCannonTarget, m_vCannonTarget);
	m_bMachGunAuto = orig->m_bMachGunAuto;
	m_bCheckMachGunLOS = orig->m_bCheckMachGunLOS;
	m_bRetVal = orig->m_bRetVal;
	m_fLastBurst = orig->m_fLastBurst;
	VectorCopy(orig->m_vMachGunTarget, m_vMachGunTarget);
}

void generic_ghoul_tank_ai::Evaluate(generic_ghoul_tank_ai *orig)
{
	m_ScriptActionCounter = orig->m_ScriptActionCounter;
	m_LastThinkingActionID = orig->m_LastThinkingActionID;
	m_attacker = GetEdictPtr((int)orig->m_attacker);
	m_bTimeToDie = orig->m_bTimeToDie;
	m_bMoveBackward = orig->m_bMoveBackward;
	VectorCopy(orig->m_vCannonTarget, m_vCannonTarget);
	m_bMachGunAuto = orig->m_bMachGunAuto;
	m_bCheckMachGunLOS = orig->m_bCheckMachGunLOS;
	m_bRetVal = orig->m_bRetVal;
	m_fLastBurst = orig->m_fLastBurst;
	VectorCopy(orig->m_vMachGunTarget, m_vMachGunTarget);
	// Don't go down the Evaluate heirachy as this is a special case
}

void generic_ghoul_tank_ai::Read()
{
	char	loaded[sizeof(generic_ghoul_tank_ai)];

	gi.ReadFromSavegame('AITA', loaded + GGTA_SAVE_START, GGTA_SAVE_END - GGTA_SAVE_START);
	Evaluate((generic_ghoul_tank_ai *)loaded);

	ai_c::Read();
}

void generic_ghoul_tank_ai::Write()
{
	byte					*save_start;
	generic_ghoul_tank_ai	*savable;

	savable = new generic_ghoul_tank_ai(this);
	save_start = (byte *)savable;
	gi.AppendToSavegame('AITA', save_start + GGTA_SAVE_START, GGTA_SAVE_END - GGTA_SAVE_START);
	delete savable;

	ai_c::Write();
}




