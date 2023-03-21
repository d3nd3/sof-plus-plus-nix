#include "g_local.h"
#include "matrix4.h"
#include "ai_private.h"
#include "callback.h"
#include "ai_pathfinding.h"
#include "..\qcommon\ef_flags.h"

#define GIB_HEALTH_LIMIT	-300

#define SCREAMNUMBER 8
#define WRITHENUMBER 7

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

bool	lock_sever;
bool	lock_gorezones;
bool	lock_deaths;

extern int ai_loadAllBolts;
void DoPiss(edict_t &monster);

static gz_code s_FrontCode = NUM_HUMANGOREZONES; // arbitrary value, really

gz_code	currentGoreZone = GZ_CHEST_FRONT;	// Store gore zone location of current damage process.

typedef int gunvariety_t;

#define	GUNVARIETY_PNONAUTO	0x00000001
#define	GUNVARIETY_LAUNCHER	0x00000002
#define	GUNVARIETY_RIFLE	0x00000004
#define	GUNVARIETY_SHOTGUN	0x00000008
#define	GUNVARIETY_MACHINE	0x00000010
#define	GUNVARIETY_KNIFE	0x00000020
#define	GUNVARIETY_PAUTO	0x00000040

#define	GUNVARIETY_NONE		0x00000000

#define	GUNVARIETY_PISTOL	(GUNVARIETY_PNONAUTO|GUNVARIETY_PAUTO)
#define GUNVARIETY_MS		(GUNVARIETY_MACHINE|GUNVARIETY_SHOTGUN)
#define GUNVARIETY_MRS		(GUNVARIETY_MS|GUNVARIETY_RIFLE)
#define GUNVARIETY_PK		(GUNVARIETY_PISTOL|GUNVARIETY_KNIFE)

qboolean Skill_NoPain(edict_t *monster)
{
	if(game.playerSkills.getEnemyValue() > 3.9)
	{
		return true;
	}
	return false;
}

//the higher the skill, the lower the disability modifier
float Skill_DisablityModifier(edict_t *monster)
{
	float tempSkill=game.playerSkills.getEnemyValue();
	if (tempSkill > 4.0)
	{
		tempSkill = 4.0;
	}
	if (tempSkill < 0.0)
	{
		tempSkill = 0.0;
	}

	//convert to value between 0 (for skill 4) and 1 (for skill 0)
	tempSkill = (4.0-tempSkill)/4.0;
	return tempSkill;
//	return 3.0/(tempSkill*tempSkill/**tempSkill*/);
}

qboolean DoesVoiceSoundExist(char *soundName)
{
	return gi.FS_FileExists(soundName);

/*	char	*Buffer;
	int		size;

//	FUCK!!!!!!!!!!!!!!!!!
	//this will prevent guys spawned 1)on the fly and 2)by spawners from using voice stuff.  but i don't know any better solution yet. the alternative is to chug the game every time a spawner spits out a guy.
	if (level.time > 2.0)
	{
		return false;
	}

	size = gi.

	size = gi.FS_LoadFile(soundName, (void **)&Buffer);
	if (size == -1)
	{
		return false;
	}

	gi.FS_FreeFile(Buffer);
	return true;*/
}


gunvariety_t GetGunVariety(attacks_e attack)
{
	switch(attack)
	{
	case ATK_PISTOL1:
	case ATK_PISTOL2:
		return GUNVARIETY_PNONAUTO;
	case ATK_MACHINEPISTOL:
		return GUNVARIETY_PAUTO;
	case ATK_MACHINEGUN:
	case ATK_ASSAULTRIFLE:
	case ATK_FLAMEGUN:
		return GUNVARIETY_MACHINE;
	case ATK_SNIPER:
		return GUNVARIETY_RIFLE;
	case ATK_SHOTGUN:
	case ATK_AUTOSHOTGUN:
	case ATK_MICROWAVE:
	case ATK_MICROWAVE_ALT:
	case ATK_DEKKER:
		return GUNVARIETY_SHOTGUN;
	case ATK_ROCKET:
		return GUNVARIETY_LAUNCHER;
	case ATK_KNIFE:
		return GUNVARIETY_KNIFE;
	}
	return GUNVARIETY_NONE;
}

bool FollowGenericPivot::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	edict_t *self=(edict_t *)ent;
	const Matrix4 *m=(const Matrix4 *)matrix;
	vec3_t angs1;

	m->GetRow(3,*(Vect3 *)self->intend_velocity);
	m->GetRow(0,*(Vect3 *)angs1);
	self->intend_velocity[1] = -self->intend_velocity[1];
	if (fabs(atan2(angs1[0],angs1[1]))>0.0000001)
	{
		self->intend_avelocity[YAW]=-VectorLength(angs1)*atan2(angs1[0],angs1[1]);
//		while (self->intend_avelocity[YAW] > 1.0)
//		{
//			self->intend_avelocity[YAW]*=0.3;//hiya!  i like to call this little bit of code...a HACK!  The parts of anims that aren't over the threshold will go too fast, but the other parts should be totally fixed.
//		}
//		if (ai_pathtest->value && self->intend_avelocity[YAW] > 1.0)
//		{
//			gi.dprintf("monster wants to turn %3.3f degrees.\n", self->intend_avelocity[YAW]*RADTODEG);
//		}
	}

	if(self->ai)
	{
		ai_c *my_ai = (ai_c *)((ai_public_c *)self->ai);

		mmove_t *move = my_ai->GetBody()->GetMove();

		if((move == &generic_move_fcornerr_m2)||
			(move == &generic_move_fcornerr_p)||
			(move == &generic_move_fcornerl_m2)||
			(move == &generic_move_fcornerl_p))
		{
			self->intend_velocity[2] = 0;

			if(my_ai->GetBody()->IsAnimationFinished())
			{	// this is very nice
				VectorInverse(self->intend_velocity);
			}
		}
	}

	return true;
}

bool FootRightCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	const Matrix4 *m=(const Matrix4 *)matrix;
	vec3_t curPos;
	edict_t *self=(edict_t *)ent;
	ai_c *my_ai = (ai_c *)((ai_public_c *)self->ai);

	if(self->health <= 0)
	{	//dead folk don't walk much
		return true;
	}

	if (!my_ai)
	{
		return true;
	}

	float	groundPos=my_ai->GetGround();

	m->GetRow(3,*(Vect3 *)curPos);
	if (my_ai->GetRightFootPos() > 9)
	{
		if (curPos[2]-groundPos < 7)
		{
			if (my_ai->GetBody() && (my_ai->GetBody()->GetArmorCode() == ARMOR_FULL))
			{	// these guys are clanky
				FX_SetEvent(self, EV_FOOTSTEPMETALRIGHT);
			}
			else
			{
				FX_SetEvent(self, EV_FOOTSTEPRIGHT);
			}
			my_ai->SetRightFootPos(curPos[2]-groundPos);
			my_ai->GetBody()->RightStep(*self);
		}
	}
	else
	{
		my_ai->SetRightFootPos(curPos[2]-groundPos);
	}
	return true;
}


bool FootLeftCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	const Matrix4 *m=(const Matrix4 *)matrix;
	vec3_t curPos;
	edict_t *self=(edict_t *)ent;
	ai_c *my_ai = (ai_c *)((ai_public_c *)self->ai);

	if(self->health <= 0)
	{	//dead folk don't walk much
		return true;
	}

	if (!my_ai)
	{
		return true;
	}

	float	groundPos=my_ai->GetGround();

	m->GetRow(3,*(Vect3 *)curPos);
	if (my_ai->GetLeftFootPos() > 9)
	{
		if (curPos[2]-groundPos < 7)
		{
			if (my_ai->GetBody() && (my_ai->GetBody()->GetArmorCode() == ARMOR_FULL))
			{	// these guys are clanky
				FX_SetEvent(self, EV_FOOTSTEPMETALLEFT);
			}
			else
			{
				FX_SetEvent(self, EV_FOOTSTEPLEFT);
			}
			my_ai->SetLeftFootPos(curPos[2]-groundPos);
			my_ai->GetBody()->LeftStep(*self);
		}
	}
	else
	{
		my_ai->SetLeftFootPos(curPos[2]-groundPos);
	}
	return true;
}

bool GroundCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	const Matrix4 *m=(const Matrix4 *)matrix;
	vec3_t curPos;
	edict_t *self=(edict_t *)ent;
	ai_c *my_ai = (ai_c *)((ai_public_c *)self->ai);

	m->GetRow(3,*(Vect3 *)curPos);
	if (self->groundentity || (self->solid == SOLID_CORPSE))
	{
		if (my_ai)
		{
			my_ai->SetGround(curPos[2]);
		}
	}
	return true;
}

bool MouthCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	const Matrix4 *m=(const Matrix4 *)matrix;
	vec3_t curPos;
	vec3_t pos,dir,postemp;
	vec3_t forward, right, up;
	edict_t *self=(edict_t *)ent;
	ai_c *my_ai = (ai_c *)((ai_public_c *)self->ai);
	int i;

	m->GetRow(0,*(Vect3 *)curPos);//forward vector, hopefully
	curPos[2] = 0;
	VectorCopy(curPos,dir);
	if (self->ai)
	{
		vectoangles(dir,forward);
		if (my_ai->GetBody() && (my_ai->GetBody()->GetClassCode() == BODY_HUSKY
			|| my_ai->GetBody()->GetClassCode() == BODY_ROTT))
		{
			forward[YAW] -= 90;
		}
//			forward[YAW]-=90;
		VectorAdd(forward,self->s.angles,forward);
		for (i=0;i<3;i++)
		{
			forward[i]=anglemod(forward[i]);
		}
		VectorCopy(forward, dir);
		my_ai->SetLookAngles(dir);
		AngleVectors(forward,dir,NULL,NULL);
	}


	m->GetRow(3,*(Vect3 *)curPos);//translation

	AngleVectors(self->s.angles,forward,right,up);
	VectorScale(forward,curPos[0],pos);
	VectorScale(right,-curPos[1],postemp);
	VectorAdd(postemp,pos,pos);
	VectorScale(up,curPos[2],postemp);
	VectorAdd(postemp,pos,pos);

//		VectorAdd(pos,self->s.origin,pos);
//		pos[2]+=self->mins[2];

//		VectorCopy(curPos,pos);
	self->viewheight = pos[2];//this is my viewheight! use it!
	VectorAdd(self->s.origin, pos, pos);
//		FX_MakeSparks(pos, dir, 1);
	return true;
}


//sets pos1 to the position of the bolt, rotated correctly to the world, but still assuming ent origin to be 0,0,0 (will deal with that later, when pos1 is used)
bool SetVectorCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	const Matrix4 *m=(const Matrix4 *)matrix;
	edict_t *self=(edict_t *)ent;
	vec3_t forward,right,up,curPos,pos,postemp;

	m->GetRow(3,*(Vect3 *)curPos);//forward vector, hopefully

	AngleVectors(self->s.angles,forward,right,up);
	VectorScale(forward,curPos[0],pos);
	VectorScale(right,-curPos[1],postemp);
	VectorAdd(postemp,pos,pos);
	VectorScale(up,curPos[2],postemp);
	VectorAdd(postemp,pos,self->pos1);
	return true;
}

bool MonsterSeqEndCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	edict_t *self=(edict_t *)ent;
	ai_c *my_ai = (ai_c *)((ai_public_c *)self->ai);

	if (self->ai)
	{
		my_ai->FinishMove(*self);
	}
	else if (self->client && self->client->body)
	{
		self->client->body->FinishMove(*self);
	}
	return true;
}

bool MonsterSeqBeginCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	edict_t		*self=(edict_t *)ent;
	ai_c		*my_ai = (ai_c *)((ai_public_c *)self->ai);
	body_c		*body = NULL;

	// if we're playing a sequence in reverse and we hit the first frame, 
	//we've finished the sequence
	if (self->ai)
	{
		body = my_ai->GetBody();
		if (body && body->IsAnimationReversed())
		{
			my_ai->FinishMove(*self);
		}
	}
	return true;
}

bool InAirCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	edict_t *ent = (edict_t *)user;
	ai_c *my_ai = (ai_c *)((ai_public_c *)ent->ai);

	if (my_ai)
	{
		my_ai->InAirFrame(*ent,now);
	}
	return true;
}

bool JumpCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	edict_t *ent = (edict_t *)user;
	ai_c *my_ai = (ai_c *)((ai_public_c *)ent->ai);

	if (my_ai)
	{
		my_ai->JumpFrame(*ent);
	}
	return true;
}

bool ThudCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	edict_t *ent = (edict_t *)user;
	ai_c *my_ai = (ai_c *)((ai_public_c *)ent->ai);

	if (my_ai)
	{
		my_ai->LandFrame(*ent);
	}
	return true;
}

bool KneeCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	edict_t *ent = (edict_t *)user;
	ai_c *my_ai = (ai_c *)((ai_public_c *)ent->ai);

	if (my_ai)
	{
		my_ai->LandFrame(*ent);
	}
	return true;
}

bool FireCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	edict_t *ent = (edict_t *)user;

	ai_c *my_ai = (ai_c *)((ai_public_c *)ent->ai);

	if (!ent || !ent->ai)
	{
		return true;
	}

	// don't set the attack flag here, set it in the callback from the actual
	//actual firing of the attack (i.e. only if we hit someone)
//		my_ai->AttackFrame(*ent);
	body_c *my_body = my_ai->GetBody();

	if (!my_body)
	{
		return true;
	}

//		my_body->FirePrimaryWeapon(*my_ai, *ent);
//		my_body->SetFlags(*my_ai, *ent, FRAMEFLAG_ATTACK);
	my_ai->AttackFrame(*ent);

	return true;
}

bool ThrowCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	edict_t *ent = (edict_t *)user;
	ai_c *my_ai = (ai_c *)((ai_public_c *)ent->ai);

	if (my_ai)
	{
		my_ai->ThrowFrame(*ent);
	}
	return true;
}


bool DropWeaponCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	edict_t		*ent = (edict_t *)user;
	ai_c		*my_ai = ( ent?(ai_c *)((ai_public_c *)ent->ai):NULL );
	bodyorganic_c *body = NULL;

	if (my_ai && (body = (bodyorganic_c*)my_ai->GetBody()) )
	{	// beginning of a death anim. if we're holding a weapon, drop it. if we're sporting
		//the fashionable "no wound" flag, well, it's a little late to be worrying about
		//wounds, isn't it?
		if (ent->spawnflags & SPAWNFLAG_NO_WOUND)
		{
			ent->spawnflags &= ~SPAWNFLAG_NO_WOUND;
		}
		// if we've got the "no weapon" flag, don't drop a weapon
		if ( !(ent->spawnflags & SPAWNFLAG_NO_WEAPONS) )
		{
			body->DropWeapon(ent, me);
		}
	}
	return true;
}





bool MonsterSoundCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	char buffer[256];
	edict_t *self = (edict_t *)user;

	Com_sprintf(buffer, 256, "%s.wav", (char *)data);

	gi.sound(self, CHAN_AUTO, gi.soundindex(buffer), .6, ATTN_NORM, 0); //fixme change?

	return true;
}

class EnemySoundPrecacheCallBack: public IGhoulCallBack
{
	public:virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data)
	{
		char buffer[256];

		Com_sprintf(buffer, sizeof(buffer), "%s.wav", (char *)data);
//		gi.dprintf("HEY! I'M PRECACHING %s DAMMIT!\n",buffer);
		gi.soundindex(buffer);
		return true;
	}
};

bool LimbEOSCallback::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	edict_t *self = (edict_t *)user;

	if (self && self->ghoulInst)
		self->ghoulInst->Pause(level.time); // want limb to in the exact position is was during the sever process

	return true;
}


gz_blown_part ecto_blown_parts[GBLOWN_ECTO_NUM+1]=
{
	//ecto:
	GBLOWN_ECTO_CHEST,				GBLOWN_ECTO_SHOULD_RF,			GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_chest",
	GBLOWN_ECTO_SHOULD_RF,			GBLOWN_ECTO_SHOULD_RB,			GBLOWNAREA_RFSHLD,	GBLOWNCODE_PART,	"",
	GBLOWN_ECTO_SHOULD_RB,			GBLOWN_ECTO_SHOULD_LF,			GBLOWNAREA_RBSHLD,	GBLOWNCODE_PART,	"",
	GBLOWN_ECTO_SHOULD_LF,			GBLOWN_ECTO_SHOULDBOLT_L,		GBLOWNAREA_LFSHLD,	GBLOWNCODE_PART,	"",
	GBLOWN_ECTO_SHOULDBOLT_L,		GBLOWN_ECTO_SHOULDBOLT_R,		GBLOWNAREA_LFSHLD,	GBLOWNCODE_BOLT,	"abolt_shoulder_l",
	GBLOWN_ECTO_SHOULDBOLT_R,		GBLOWN_ECTO_BACKBOLT,			GBLOWNAREA_RFSHLD,	GBLOWNCODE_BOLT,	"abolt_shoulder_r",
	GBLOWN_ECTO_BACKBOLT,			GBLOWN_ECTO_SHOULD_LB,			GBLOWNAREA_CHEST,	GBLOWNCODE_BOLT,	"abolt_back",
	GBLOWN_ECTO_SHOULD_LB,			GBLOWN_ECTO_COATCHEST,			GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"",
	GBLOWN_ECTO_COATCHEST,			GBLOWN_ECTO_CHESTFINISH,		GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_coatchest",

	//shared:
	GBLOWN_ECTO_CHESTFINISH,			GBLOWN_ECTO_NUM,			GBLOWNAREA_CHEST,	GBLOWNCODE_SPECIAL,	"",

	//ecto:
	GBLOWN_ECTO_HEADBALD,			GBLOWN_ECTO_HEADCREW,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_headbald",
	GBLOWN_ECTO_HEADCREW,			GBLOWN_ECTO_HEADBEARD,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_headcrew",
	GBLOWN_ECTO_HEADBEARD,			GBLOWN_ECTO_HEADHELMET,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_headbeard",
	GBLOWN_ECTO_HEADHELMET,			GBLOWN_ECTO_BANGLEFT,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_headhelmet",
	GBLOWN_ECTO_BANGLEFT,			GBLOWN_ECTO_BANGRIGHT,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_lhairbang",
	GBLOWN_ECTO_BANGRIGHT,			GBLOWN_ECTO_HAIRMOHAWK,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_rhairbang",
	GBLOWN_ECTO_HAIRMOHAWK,			GBLOWN_ECTO_HAIRPONYTAIL,		GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_hairmohawk",
	GBLOWN_ECTO_HAIRPONYTAIL,		GBLOWN_ECTO_EARLEFT,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_hairponytail",

	//shared:
	GBLOWN_ECTO_EARLEFT,			GBLOWN_ECTO_EARRIGHT,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_lear",
	GBLOWN_ECTO_EARRIGHT,			GBLOWN_ECTO_HEADBOLT,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_rear",
	GBLOWN_ECTO_HEADBOLT,			GBLOWN_ECTO_NUM,				GBLOWNAREA_HEAD,	GBLOWNCODE_BOLT,	"abolt_head_t",

	//ecto:
	GBLOWN_ECTO_BICEPLEFT,			GBLOWN_ECTO_FOREARMLEFT,		GBLOWNAREA_LBICEP,	GBLOWNCODE_PART,	"_lbicep",

	//ecto:
	GBLOWN_ECTO_FOREARMLEFT,		GBLOWN_ECTO_FOREARMCUFFLEFT,	GBLOWNAREA_LFOREARM,GBLOWNCODE_PART,	"_lforearm",
	GBLOWN_ECTO_FOREARMCUFFLEFT,	GBLOWN_ECTO_FOREARMCUFFCAPLEFT,	GBLOWNAREA_LFOREARM,GBLOWNCODE_PART,	"_lforearmcuff",
	GBLOWN_ECTO_FOREARMCUFFCAPLEFT,	GBLOWN_ECTO_OPENHANDLEFT,		GBLOWNAREA_LFOREARM,GBLOWNCODE_PART,	"_Cap_lforearmcuff_lhand",
	GBLOWN_ECTO_OPENHANDLEFT,		GBLOWN_ECTO_CLOSEDHANDLEFT,		GBLOWNAREA_LHAND,	GBLOWNCODE_PART,	"_lhand_open",
	GBLOWN_ECTO_CLOSEDHANDLEFT,		GBLOWN_ECTO_HANDBOLTLEFT,		GBLOWNAREA_LHAND,	GBLOWNCODE_PART,	"_lhand_closed",

	//shared:
	GBLOWN_ECTO_HANDBOLTLEFT,		GBLOWN_ECTO_NUM,				GBLOWNAREA_LHAND,	GBLOWNCODE_BOLT,	"wbolt_hand_l",

	//ecto:
	GBLOWN_ECTO_BICEPRIGHT,			GBLOWN_ECTO_FOREARMRIGHT,		GBLOWNAREA_RBICEP,	GBLOWNCODE_PART,	"_rbicep",

	//ecto:
	GBLOWN_ECTO_FOREARMRIGHT,		GBLOWN_ECTO_FOREARMCUFFRIGHT,	GBLOWNAREA_RFOREARM,GBLOWNCODE_PART,	"_rforearm",
	GBLOWN_ECTO_FOREARMCUFFRIGHT,	GBLOWN_ECTO_FOREARMCUFFCAPRIGHT,GBLOWNAREA_RFOREARM,GBLOWNCODE_PART,	"_rforearmcuff",
	GBLOWN_ECTO_FOREARMCUFFCAPRIGHT,GBLOWN_ECTO_OPENHANDRIGHT,		GBLOWNAREA_RFOREARM,GBLOWNCODE_PART,	"_Cap_rforearmcuff_rhand",
	GBLOWN_ECTO_OPENHANDRIGHT,		GBLOWN_ECTO_CLOSEDHANDRIGHT,	GBLOWNAREA_RHAND,	GBLOWNCODE_PART,	"_rhand_open",
	GBLOWN_ECTO_CLOSEDHANDRIGHT,	GBLOWN_ECTO_HANDBOLTRIGHT,		GBLOWNAREA_RHAND,	GBLOWNCODE_PART,	"_rhand_closed",

	//shared:
	GBLOWN_ECTO_HANDBOLTRIGHT,		GBLOWN_ECTO_NUM,				GBLOWNAREA_RHAND,	GBLOWNCODE_BOLT,	"wbolt_hand_r",

	//ecto:
	GBLOWN_ECTO_THIGHRIGHT,			GBLOWN_ECTO_THIGHBOLTRIGHT,		GBLOWNAREA_RTHIGH,	GBLOWNCODE_PART,	"_rthigh",

	//shared:
	GBLOWN_ECTO_THIGHBOLTRIGHT,		GBLOWN_ECTO_THIGHRIGHTSPECIAL,	GBLOWNAREA_RTHIGH,	GBLOWNCODE_BOLT,	"abolt_thigh_r",
	GBLOWN_ECTO_THIGHRIGHTSPECIAL,	GBLOWN_ECTO_CALFRIGHT,			GBLOWNAREA_RTHIGH,	GBLOWNCODE_SPECIAL,	"",

	//ecto:
	GBLOWN_ECTO_CALFRIGHT,			GBLOWN_ECTO_CALFCUFFRIGHT,		GBLOWNAREA_RCALF,	GBLOWNCODE_PART,	"_rcalf",
	GBLOWN_ECTO_CALFCUFFRIGHT,		GBLOWN_ECTO_CALFCUFFCAPRIGHT,	GBLOWNAREA_RCALF,	GBLOWNCODE_PART,	"_rcalfcuff",
	GBLOWN_ECTO_CALFCUFFCAPRIGHT,	GBLOWN_ECTO_CALFBOLTRIGHT,		GBLOWNAREA_RCALF,	GBLOWNCODE_PART,	"_cap_rcalfcuff_rfoot",

	//shared:
	GBLOWN_ECTO_CALFBOLTRIGHT,		GBLOWN_ECTO_FOOTRIGHT,			GBLOWNAREA_RCALF,	GBLOWNCODE_BOLT,	"abolt_calf_r",

	//ecto:
	GBLOWN_ECTO_FOOTRIGHT,			GBLOWN_ECTO_NUM,				GBLOWNAREA_RFOOT,	GBLOWNCODE_PART,	"_rfoot",

	//ecto:
	GBLOWN_ECTO_THIGHLEFT,			GBLOWN_ECTO_THIGHBOLTLEFT,		GBLOWNAREA_LTHIGH,	GBLOWNCODE_PART,	"_lthigh",

	//shared:
	GBLOWN_ECTO_THIGHBOLTLEFT,		GBLOWN_ECTO_THIGHLEFTSPECIAL,	GBLOWNAREA_LTHIGH,	GBLOWNCODE_BOLT,	"abolt_thigh_l",
	GBLOWN_ECTO_THIGHLEFTSPECIAL,	GBLOWN_ECTO_CALFLEFT,			GBLOWNAREA_LTHIGH,	GBLOWNCODE_SPECIAL,	"",

	//ecto:
	GBLOWN_ECTO_CALFLEFT,			GBLOWN_ECTO_CALFCUFFLEFT,		GBLOWNAREA_LCALF,	GBLOWNCODE_PART,	"_lcalf",
	GBLOWN_ECTO_CALFCUFFLEFT,		GBLOWN_ECTO_CALFCUFFCAPLEFT,	GBLOWNAREA_LCALF,	GBLOWNCODE_PART,	"_lcalfcuff",
	GBLOWN_ECTO_CALFCUFFCAPLEFT,	GBLOWN_ECTO_CALFBOLTLEFT,		GBLOWNAREA_LCALF,	GBLOWNCODE_PART,	"_cap_lcalfcuff_lfoot",

	//shared:
	GBLOWN_ECTO_CALFBOLTLEFT,		GBLOWN_ECTO_FOOTLEFT,			GBLOWNAREA_LCALF,	GBLOWNCODE_BOLT,	"abolt_calf_l",

	//ecto:
	GBLOWN_ECTO_FOOTLEFT,			GBLOWN_ECTO_NUM,				GBLOWNAREA_LFOOT,	GBLOWNCODE_PART,	"_lfoot",

	GBLOWN_ECTO_GROIN,				GBLOWN_ECTO_NUM,				GBLOWNAREA_GROIN,	GBLOWNCODE_SPECIAL,	"",
	GBLOWN_ECTO_NUM,				GBLOWN_ECTO_NUM,				GBLOWNAREA_FINISH,	GBLOWNCODE_SPECIAL,	"",
};

gz_blown_part female_blown_parts[GBLOWN_FEMALE_NUM+1]=
{
	GBLOWN_FEMALE_CHEST,				GBLOWN_FEMALE_SHOULD_RF,			GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_chest",
	GBLOWN_FEMALE_SHOULD_RF,			GBLOWN_FEMALE_SHOULD_RB,			GBLOWNAREA_RFSHLD,	GBLOWNCODE_PART,	"",
	GBLOWN_FEMALE_SHOULD_RB,			GBLOWN_FEMALE_SHOULD_LF,			GBLOWNAREA_RBSHLD,	GBLOWNCODE_PART,	"",
	GBLOWN_FEMALE_SHOULD_LF,			GBLOWN_FEMALE_SHOULD_LB,			GBLOWNAREA_LFSHLD,	GBLOWNCODE_PART,	"",
	GBLOWN_FEMALE_SHOULD_LB,			GBLOWN_FEMALE_SHOULDBOLT_L,			GBLOWNAREA_LBSHLD,	GBLOWNCODE_PART,	"",
	GBLOWN_FEMALE_SHOULDBOLT_L,			GBLOWN_FEMALE_SHOULDBOLT_R,			GBLOWNAREA_LFSHLD,	GBLOWNCODE_BOLT,	"abolt_shoulder_l",
	GBLOWN_FEMALE_SHOULDBOLT_R,			GBLOWN_FEMALE_BACKBOLT,				GBLOWNAREA_RFSHLD,	GBLOWNCODE_BOLT,	"abolt_shoulder_r",
	GBLOWN_FEMALE_BACKBOLT,				GBLOWN_FEMALE_NECK,					GBLOWNAREA_CHEST,	GBLOWNCODE_BOLT,	"abolt_back",
	GBLOWN_FEMALE_NECK,					GBLOWN_FEMALE_ARMORCHEST,			GBLOWNAREA_NECK,	GBLOWNCODE_PART,	"_neck",
	GBLOWN_FEMALE_ARMORCHEST,			GBLOWN_FEMALE_CHESTFINISH,			GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_armorchest",
	GBLOWN_FEMALE_CHESTFINISH,			GBLOWN_FEMALE_NUM,					GBLOWNAREA_CHEST,	GBLOWNCODE_SPECIAL,	"",

	GBLOWN_FEMALE_HEAD,					GBLOWN_FEMALE_FACE,					GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_headcrew",
	GBLOWN_FEMALE_FACE,					GBLOWN_FEMALE_CREWHAIR,				GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_comface",
	GBLOWN_FEMALE_CREWHAIR,				GBLOWN_FEMALE_MEDHAIR,				GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_crewhair",
	GBLOWN_FEMALE_MEDHAIR,				GBLOWN_FEMALE_BANGLEFT,				GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_medmhairmhead",
	GBLOWN_FEMALE_BANGLEFT,				GBLOWN_FEMALE_BANGRIGHT,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_lbang",
	GBLOWN_FEMALE_BANGRIGHT,			GBLOWN_FEMALE_EARLEFT,				GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_rbang",

	GBLOWN_FEMALE_EARLEFT,				GBLOWN_FEMALE_EARRIGHT,				GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_lear",
	GBLOWN_FEMALE_EARRIGHT,				GBLOWN_FEMALE_HEADBOLT,				GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_rear",
	GBLOWN_FEMALE_HEADBOLT,				GBLOWN_FEMALE_NUM,					GBLOWNAREA_HEAD,	GBLOWNCODE_BOLT,	"abolt_head_t",

	GBLOWN_FEMALE_BICEPLEFT,			GBLOWN_FEMALE_FOREARMLEFT,			GBLOWNAREA_LBICEP,	GBLOWNCODE_PART,	"_ltightbicep",
	GBLOWN_FEMALE_FOREARMLEFT,			GBLOWN_FEMALE_OPENHANDLEFT,			GBLOWNAREA_LFOREARM,GBLOWNCODE_PART,	"_ltightforearm",
	GBLOWN_FEMALE_OPENHANDLEFT,			GBLOWN_FEMALE_CLOSEDHANDLEFT,		GBLOWNAREA_LHAND,	GBLOWNCODE_PART,	"_lopen",
	GBLOWN_FEMALE_CLOSEDHANDLEFT,		GBLOWN_FEMALE_HANDBOLTLEFT,			GBLOWNAREA_LHAND,	GBLOWNCODE_PART,	"_lclosed",
	GBLOWN_FEMALE_HANDBOLTLEFT,			GBLOWN_FEMALE_NUM,					GBLOWNAREA_LHAND,	GBLOWNCODE_BOLT,	"wbolt_hand_l",

	GBLOWN_FEMALE_BICEPRIGHT,			GBLOWN_FEMALE_FOREARMRIGHT,			GBLOWNAREA_RBICEP,	GBLOWNCODE_PART,	"_rtightbicep",
	GBLOWN_FEMALE_FOREARMRIGHT,			GBLOWN_FEMALE_OPENHANDRIGHT,		GBLOWNAREA_RFOREARM,GBLOWNCODE_PART,	"_rtightforearm",
	GBLOWN_FEMALE_OPENHANDRIGHT,		GBLOWN_FEMALE_CLOSEDHANDRIGHT,		GBLOWNAREA_RHAND,	GBLOWNCODE_PART,	"_ropen",
	GBLOWN_FEMALE_CLOSEDHANDRIGHT,		GBLOWN_FEMALE_HANDBOLTRIGHT,		GBLOWNAREA_RHAND,	GBLOWNCODE_PART,	"_rclosed",
	GBLOWN_FEMALE_HANDBOLTRIGHT,		GBLOWN_FEMALE_NUM,					GBLOWNAREA_RHAND,	GBLOWNCODE_BOLT,	"wbolt_hand_r",

	GBLOWN_FEMALE_THIGHRIGHT,			GBLOWN_FEMALE_THIGHBOLTRIGHT,		GBLOWNAREA_RTHIGH,	GBLOWNCODE_PART,	"_rtightthigh",
	GBLOWN_FEMALE_THIGHBOLTRIGHT,		GBLOWN_FEMALE_THIGHRIGHTSPECIAL,	GBLOWNAREA_RTHIGH,	GBLOWNCODE_BOLT,	"abolt_thigh_r",
	GBLOWN_FEMALE_THIGHRIGHTSPECIAL,	GBLOWN_FEMALE_CALFRIGHT,			GBLOWNAREA_RTHIGH,	GBLOWNCODE_SPECIAL,	"",

	GBLOWN_FEMALE_CALFRIGHT,			GBLOWN_FEMALE_CALFBOLTRIGHT,		GBLOWNAREA_RCALF,	GBLOWNCODE_PART,	"_rtightcalf",
	GBLOWN_FEMALE_CALFBOLTRIGHT,		GBLOWN_FEMALE_FOOTRIGHT,			GBLOWNAREA_RCALF,	GBLOWNCODE_BOLT,	"abolt_calf_r",
	GBLOWN_FEMALE_FOOTRIGHT,			GBLOWN_FEMALE_NUM,					GBLOWNAREA_RFOOT,	GBLOWNCODE_PART,	"_rheels",

	GBLOWN_FEMALE_THIGHLEFT,			GBLOWN_FEMALE_THIGHBOLTLEFT,		GBLOWNAREA_LTHIGH,	GBLOWNCODE_PART,	"_ltightthigh",
	GBLOWN_FEMALE_THIGHBOLTLEFT,		GBLOWN_FEMALE_THIGHLEFTSPECIAL,		GBLOWNAREA_LTHIGH,	GBLOWNCODE_BOLT,	"abolt_thigh_l",
	GBLOWN_FEMALE_THIGHLEFTSPECIAL,		GBLOWN_FEMALE_CALFLEFT,				GBLOWNAREA_LTHIGH,	GBLOWNCODE_SPECIAL,	"",

	GBLOWN_FEMALE_CALFLEFT,				GBLOWN_FEMALE_CALFBOLTLEFT,			GBLOWNAREA_LCALF,	GBLOWNCODE_PART,	"_ltightcalf",
	GBLOWN_FEMALE_CALFBOLTLEFT,			GBLOWN_FEMALE_FOOTLEFT,				GBLOWNAREA_LCALF,	GBLOWNCODE_BOLT,	"abolt_calf_l",
	GBLOWN_FEMALE_FOOTLEFT,				GBLOWN_FEMALE_NUM,					GBLOWNAREA_LFOOT,	GBLOWNCODE_PART,	"_lheels",

	GBLOWN_FEMALE_GROIN,				GBLOWN_FEMALE_NUM,					GBLOWNAREA_GROIN,	GBLOWNCODE_SPECIAL,	"",
	GBLOWN_FEMALE_NUM,					GBLOWN_FEMALE_NUM,					GBLOWNAREA_FINISH,	GBLOWNCODE_SPECIAL,	"",
};

gz_blown_part meso_blown_parts[GBLOWN_MESO_NUM+1]=
{
	//meso:
	GBLOWN_MESO_TIGHTCHEST,			GBLOWN_MESO_MASKCHEST,			GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_tightchest",
	GBLOWN_MESO_MASKCHEST,			GBLOWN_MESO_BULKYCHEST,			GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_maskchest",
	GBLOWN_MESO_BULKYCHEST,			GBLOWN_MESO_ARMORCHEST,			GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_bulkychest",
	GBLOWN_MESO_ARMORCHEST,			GBLOWN_MESO_BOSSCHEST,			GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_armorchest",
	GBLOWN_MESO_BOSSCHEST,			GBLOWN_MESO_BTCHEST,			GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_bosschest",
	GBLOWN_MESO_BTCHEST,			GBLOWN_MESO_NECK,				GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_chest_b_t",
	GBLOWN_MESO_NECK,				GBLOWN_MESO_BOSSNECK,			GBLOWNAREA_NECK,	GBLOWNCODE_PART,	"_neck",
	GBLOWN_MESO_BOSSNECK,			GBLOWN_MESO_MASKNECK,			GBLOWNAREA_NECK,	GBLOWNCODE_PART,	"_bossneck",
	GBLOWN_MESO_MASKNECK,			GBLOWN_MESO_HOODNECK,			GBLOWNAREA_NECK,	GBLOWNCODE_PART,	"_maskneck",
	GBLOWN_MESO_HOODNECK,			GBLOWN_MESO_HEADCAP_NECK,		GBLOWNAREA_NECK,	GBLOWNCODE_PART,	"_hoodneck",
	GBLOWN_MESO_HEADCAP_NECK,		GBLOWN_MESO_HEADCAP_BOSSNECK,	GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_cap_head_neck",
	GBLOWN_MESO_HEADCAP_BOSSNECK,	GBLOWN_MESO_SHOULD_RF,			GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_cap_head_neck_boss",
	GBLOWN_MESO_SHOULD_RF,			GBLOWN_MESO_SHOULD_RB,			GBLOWNAREA_RFSHLD,	GBLOWNCODE_PART,	"",
	GBLOWN_MESO_SHOULD_RB,			GBLOWN_MESO_SHOULD_LF,			GBLOWNAREA_RBSHLD,	GBLOWNCODE_PART,	"",
	GBLOWN_MESO_SHOULD_LF,			GBLOWN_MESO_SHOULD_LB,			GBLOWNAREA_LFSHLD,	GBLOWNCODE_PART,	"",
	GBLOWN_MESO_SHOULD_LB,			GBLOWN_MESO_SHOULDBOLT_L,		GBLOWNAREA_LBSHLD,	GBLOWNCODE_PART,	"",
	GBLOWN_MESO_SHOULDBOLT_L,		GBLOWN_MESO_SHOULDBOLT_R,		GBLOWNAREA_LFSHLD,	GBLOWNCODE_BOLT,	"abolt_shoulder_l",
	GBLOWN_MESO_SHOULDBOLT_R,		GBLOWN_MESO_BACKBOLT,			GBLOWNAREA_RFSHLD,	GBLOWNCODE_BOLT,	"abolt_shoulder_r",
	GBLOWN_MESO_BACKBOLT,			GBLOWN_MESO_CHEST_RARMCAP3,		GBLOWNAREA_CHEST,	GBLOWNCODE_BOLT,	"abolt_back",
	GBLOWN_MESO_CHEST_RARMCAP3,		GBLOWN_MESO_CHEST_LARMCAP3,		GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_cap_chest_rbicep_bulky",
	GBLOWN_MESO_CHEST_LARMCAP3,		GBLOWN_MESO_RARMCAP_CHEST3,		GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_cap_chest_lbicep_bulky",
	GBLOWN_MESO_RARMCAP_CHEST3,		GBLOWN_MESO_LARMCAP_CHEST3,		GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_cap_rbicep_chest_bulky",
	GBLOWN_MESO_LARMCAP_CHEST3,		GBLOWN_MESO_CHEST_RARMCAP4,		GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_cap_lbicep_chest_bulky",
	GBLOWN_MESO_CHEST_RARMCAP4,		GBLOWN_MESO_CHEST_LARMCAP4,		GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_cap_chest_rbicep_boss",
	GBLOWN_MESO_CHEST_LARMCAP4,		GBLOWN_MESO_RARMCAP_CHEST4,		GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_cap_chest_lbicep_boss",
	GBLOWN_MESO_RARMCAP_CHEST4,		GBLOWN_MESO_LARMCAP_CHEST4,		GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_cap_rbicep_chest_boss",
	GBLOWN_MESO_LARMCAP_CHEST4,		GBLOWN_MESO_CHESTFINISH,		GBLOWNAREA_CHEST,	GBLOWNCODE_PART,	"_cap_lbicep_chest_boss",

	//shared:
	GBLOWN_MESO_CHESTFINISH,		GBLOWN_MESO_NUM,				GBLOWNAREA_CHEST,	GBLOWNCODE_SPECIAL,	"",

	//meso:
	GBLOWN_MESO_HEADBALD,			GBLOWN_MESO_HEADCREW,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_baldhead",
	GBLOWN_MESO_HEADCREW,			GBLOWN_MESO_HEADBEARD,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_crewhead",
	GBLOWN_MESO_HEADBEARD,			GBLOWN_MESO_HEADHELMET,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_beardhead",
	GBLOWN_MESO_HEADHELMET,			GBLOWN_MESO_HEADHOOD,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_helmethead",
	GBLOWN_MESO_HEADHOOD,			GBLOWN_MESO_HEADMASK,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_hoodhead",
	GBLOWN_MESO_HEADMASK,			GBLOWN_MESO_HEADBOSS,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_maskhead",
	GBLOWN_MESO_HEADBOSS,			GBLOWN_MESO_COMFACE,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_bosshead",
	GBLOWN_MESO_COMFACE,			GBLOWN_MESO_BEARDFACE,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_comface",
	GBLOWN_MESO_BEARDFACE,			GBLOWN_MESO_HELMETFACE,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_beardface",
	GBLOWN_MESO_HELMETFACE,			GBLOWN_MESO_BOSSFACE,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_helmetface",
	GBLOWN_MESO_BOSSFACE,			GBLOWN_MESO_BANGLEFT,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_bossface",
	GBLOWN_MESO_BANGLEFT,			GBLOWN_MESO_BANGRIGHT,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_lbang",
	GBLOWN_MESO_BANGRIGHT,			GBLOWN_MESO_HAIRMOHAWK,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_rbang",
	GBLOWN_MESO_HAIRMOHAWK,			GBLOWN_MESO_HAIRPONYTAIL,		GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_mohawk",
	GBLOWN_MESO_HAIRPONYTAIL,		GBLOWN_MESO_EARLEFT,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_ponytail",

	//shared:
	GBLOWN_MESO_EARLEFT,			GBLOWN_MESO_EARRIGHT,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_lear",
	GBLOWN_MESO_EARRIGHT,			GBLOWN_MESO_BOSSEARLEFT,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_rear",
	GBLOWN_MESO_BOSSEARLEFT,		GBLOWN_MESO_BOSSEARRIGHT,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_lbossear",
	GBLOWN_MESO_BOSSEARRIGHT,		GBLOWN_MESO_HEADBOLT,			GBLOWNAREA_HEAD,	GBLOWNCODE_PART,	"_rbossear",
	GBLOWN_MESO_HEADBOLT,			GBLOWN_MESO_NUM,				GBLOWNAREA_HEAD,	GBLOWNCODE_BOLT,	"abolt_head_t",

	//meso:
	GBLOWN_MESO_BICEPLEFT,			GBLOWN_MESO_BICEPLEFT2,			GBLOWNAREA_LBICEP,	GBLOWNCODE_PART,	"_lbossbicep",
	GBLOWN_MESO_BICEPLEFT2,			GBLOWN_MESO_BICEPLEFT3,			GBLOWNAREA_LBICEP,	GBLOWNCODE_PART,	"_lbicep_t_t",
	GBLOWN_MESO_BICEPLEFT3,			GBLOWN_MESO_BICEPLEFT4,			GBLOWNAREA_LBICEP,	GBLOWNCODE_PART,	"_lbicep_t_b",
	GBLOWN_MESO_BICEPLEFT4,			GBLOWN_MESO_BICEPLEFT5,			GBLOWNAREA_LBICEP,	GBLOWNCODE_PART,	"_lbicep_b_b",
	GBLOWN_MESO_BICEPLEFT5,			GBLOWN_MESO_FOREARMLEFT,		GBLOWNAREA_LBICEP,	GBLOWNCODE_PART,	"_lbicep_b_t",

	//meso:
	GBLOWN_MESO_FOREARMLEFT,		GBLOWN_MESO_FOREARMLEFT2,		GBLOWNAREA_LFOREARM,GBLOWNCODE_PART,	"_lforearm_t_t",
	GBLOWN_MESO_FOREARMLEFT2,		GBLOWN_MESO_FOREARMLEFT3,		GBLOWNAREA_LFOREARM,GBLOWNCODE_PART,	"_lforearm_t_c",
	GBLOWN_MESO_FOREARMLEFT3,		GBLOWN_MESO_FOREARMLEFT4,		GBLOWNAREA_LFOREARM,GBLOWNCODE_PART,	"_lforearm_t_b",
	GBLOWN_MESO_FOREARMLEFT4,		GBLOWN_MESO_FOREARMLEFT5,		GBLOWNAREA_LFOREARM,GBLOWNCODE_PART,	"_lforearm_b_t",
	GBLOWN_MESO_FOREARMLEFT5,		GBLOWN_MESO_FOREARMLEFT6,		GBLOWNAREA_LFOREARM,GBLOWNCODE_PART,	"_lforearm_b_b",
	GBLOWN_MESO_FOREARMLEFT6,		GBLOWN_MESO_OPENHANDLEFT,		GBLOWNAREA_LFOREARM,GBLOWNCODE_PART,	"_lbossforearm",
	GBLOWN_MESO_OPENHANDLEFT,		GBLOWN_MESO_CLOSEDHANDLEFT,		GBLOWNAREA_LHAND,	GBLOWNCODE_PART,	"_lopen",
	GBLOWN_MESO_CLOSEDHANDLEFT,		GBLOWN_MESO_OPENHANDLEFT2,		GBLOWNAREA_LHAND,	GBLOWNCODE_PART,	"_lclosed",
	GBLOWN_MESO_OPENHANDLEFT2,		GBLOWN_MESO_CLOSEDHANDLEFT2,	GBLOWNAREA_LHAND,	GBLOWNCODE_PART,	"_lbossopen",
	GBLOWN_MESO_CLOSEDHANDLEFT2,	GBLOWN_MESO_HANDBOLTLEFT,		GBLOWNAREA_LHAND,	GBLOWNCODE_PART,	"_lbossclosed",

	//shared:
	GBLOWN_MESO_HANDBOLTLEFT,		GBLOWN_MESO_NUM,				GBLOWNAREA_LHAND,	GBLOWNCODE_BOLT,	"wbolt_hand_l",

	//meso:
	GBLOWN_MESO_BICEPRIGHT,			GBLOWN_MESO_BICEPRIGHT2,		GBLOWNAREA_RBICEP,	GBLOWNCODE_PART,	"_rbossbicep",
	GBLOWN_MESO_BICEPRIGHT2,		GBLOWN_MESO_BICEPRIGHT3,		GBLOWNAREA_RBICEP,	GBLOWNCODE_PART,	"_rbicep_t_t",
	GBLOWN_MESO_BICEPRIGHT3,		GBLOWN_MESO_BICEPRIGHT4,		GBLOWNAREA_RBICEP,	GBLOWNCODE_PART,	"_rbicep_t_b",
	GBLOWN_MESO_BICEPRIGHT4,		GBLOWN_MESO_BICEPRIGHT5,		GBLOWNAREA_RBICEP,	GBLOWNCODE_PART,	"_rbicep_b_b",
	GBLOWN_MESO_BICEPRIGHT5,		GBLOWN_MESO_FOREARMRIGHT,		GBLOWNAREA_RBICEP,	GBLOWNCODE_PART,	"_rbicep_b_t",

	//meso:
	GBLOWN_MESO_FOREARMRIGHT,		GBLOWN_MESO_FOREARMRIGHT2,		GBLOWNAREA_RFOREARM,GBLOWNCODE_PART,	"_rforearm_t_t",
	GBLOWN_MESO_FOREARMRIGHT2,		GBLOWN_MESO_FOREARMRIGHT3,		GBLOWNAREA_RFOREARM,GBLOWNCODE_PART,	"_rforearm_t_c",
	GBLOWN_MESO_FOREARMRIGHT3,		GBLOWN_MESO_FOREARMRIGHT4,		GBLOWNAREA_RFOREARM,GBLOWNCODE_PART,	"_rforearm_t_b",
	GBLOWN_MESO_FOREARMRIGHT4,		GBLOWN_MESO_FOREARMRIGHT5,		GBLOWNAREA_RFOREARM,GBLOWNCODE_PART,	"_rforearm_b_t",
	GBLOWN_MESO_FOREARMRIGHT5,		GBLOWN_MESO_FOREARMRIGHT6,		GBLOWNAREA_RFOREARM,GBLOWNCODE_PART,	"_rforearm_b_b",
	GBLOWN_MESO_FOREARMRIGHT6,		GBLOWN_MESO_OPENHANDRIGHT,		GBLOWNAREA_RFOREARM,GBLOWNCODE_PART,	"_rbossforearm",
	GBLOWN_MESO_OPENHANDRIGHT,		GBLOWN_MESO_CLOSEDHANDRIGHT,	GBLOWNAREA_RHAND,	GBLOWNCODE_PART,	"_ropen",
	GBLOWN_MESO_CLOSEDHANDRIGHT,	GBLOWN_MESO_OPENHANDRIGHT2,		GBLOWNAREA_RHAND,	GBLOWNCODE_PART,	"_rclosed",
	GBLOWN_MESO_OPENHANDRIGHT2,		GBLOWN_MESO_CLOSEDHANDRIGHT2,	GBLOWNAREA_RHAND,	GBLOWNCODE_PART,	"_rbossopen",
	GBLOWN_MESO_CLOSEDHANDRIGHT2,	GBLOWN_MESO_HANDBOLTRIGHT,		GBLOWNAREA_RHAND,	GBLOWNCODE_PART,	"_rbossclosed",

	//shared:
	GBLOWN_MESO_HANDBOLTRIGHT,		GBLOWN_MESO_NUM,				GBLOWNAREA_RHAND,	GBLOWNCODE_BOLT,	"wbolt_hand_r",

	//meso:
	GBLOWN_MESO_THIGHRIGHT,			GBLOWN_MESO_THIGHRIGHT2,		GBLOWNAREA_RTHIGH,	GBLOWNCODE_PART,	"_rbossthigh",
	GBLOWN_MESO_THIGHRIGHT2,		GBLOWN_MESO_THIGHRIGHT3,		GBLOWNAREA_RTHIGH,	GBLOWNCODE_PART,	"_rtightthigh",
	GBLOWN_MESO_THIGHRIGHT3,		GBLOWN_MESO_THIGHBOLTRIGHT,		GBLOWNAREA_RTHIGH,	GBLOWNCODE_PART,	"_rbulkythigh",

	//shared:
	GBLOWN_MESO_THIGHBOLTRIGHT,		GBLOWN_MESO_THIGHRIGHTSPECIAL,	GBLOWNAREA_RTHIGH,	GBLOWNCODE_BOLT,	"abolt_thigh_r",
	GBLOWN_MESO_THIGHRIGHTSPECIAL,	GBLOWN_MESO_CALFRIGHT,			GBLOWNAREA_RTHIGH,	GBLOWNCODE_SPECIAL,	"",

	//meso:
	GBLOWN_MESO_CALFRIGHT,			GBLOWN_MESO_CALFRIGHT2,			GBLOWNAREA_RCALF,	GBLOWNCODE_PART,	"_rtightcalf",
	GBLOWN_MESO_CALFRIGHT2,			GBLOWN_MESO_CALFRIGHT3,			GBLOWNAREA_RCALF,	GBLOWNCODE_PART,	"_rcalfcuff",
	GBLOWN_MESO_CALFRIGHT3,			GBLOWN_MESO_CALFRIGHT4,			GBLOWNAREA_RCALF,	GBLOWNCODE_PART,	"_rbulkycalf",
	GBLOWN_MESO_CALFRIGHT4,			GBLOWN_MESO_CALFRIGHT5,			GBLOWNAREA_RCALF,	GBLOWNCODE_PART,	"_rbosscalf",
	GBLOWN_MESO_CALFRIGHT5,			GBLOWN_MESO_CALFBOLTRIGHT,		GBLOWNAREA_RCALF,	GBLOWNCODE_PART,	"_rcalf_t_b",

	//shared:
	GBLOWN_MESO_CALFBOLTRIGHT,		GBLOWN_MESO_FOOTRIGHT,			GBLOWNAREA_RCALF,	GBLOWNCODE_BOLT,	"abolt_calf_r",

	//meso:
	GBLOWN_MESO_FOOTRIGHT,			GBLOWN_MESO_FOOTRIGHT2,			GBLOWNAREA_RFOOT,	GBLOWNCODE_PART,	"_rbootfoot",
	GBLOWN_MESO_FOOTRIGHT2,			GBLOWN_MESO_NUM,				GBLOWNAREA_RFOOT,	GBLOWNCODE_PART,	"_rshoefoot",

	//meso:
	GBLOWN_MESO_THIGHLEFT,			GBLOWN_MESO_THIGHLEFT2,			GBLOWNAREA_LTHIGH,	GBLOWNCODE_PART,	"_lbossthigh",
	GBLOWN_MESO_THIGHLEFT2,			GBLOWN_MESO_THIGHLEFT3,			GBLOWNAREA_LTHIGH,	GBLOWNCODE_PART,	"_ltightthigh",
	GBLOWN_MESO_THIGHLEFT3,			GBLOWN_MESO_THIGHBOLTLEFT,		GBLOWNAREA_LTHIGH,	GBLOWNCODE_PART,	"_lbulkythigh",

	//shared:
	GBLOWN_MESO_THIGHBOLTLEFT,		GBLOWN_MESO_THIGHLEFTSPECIAL,	GBLOWNAREA_LTHIGH,	GBLOWNCODE_BOLT,	"abolt_thigh_l",
	GBLOWN_MESO_THIGHLEFTSPECIAL,	GBLOWN_MESO_CALFLEFT,			GBLOWNAREA_LTHIGH,	GBLOWNCODE_SPECIAL,	"",

	//meso:
	GBLOWN_MESO_CALFLEFT,			GBLOWN_MESO_CALFLEFT2,			GBLOWNAREA_LCALF,	GBLOWNCODE_PART,	"_ltightcalf",
	GBLOWN_MESO_CALFLEFT2,			GBLOWN_MESO_CALFLEFT3,			GBLOWNAREA_LCALF,	GBLOWNCODE_PART,	"_lcalfcuff",
	GBLOWN_MESO_CALFLEFT3,			GBLOWN_MESO_CALFLEFT4,			GBLOWNAREA_LCALF,	GBLOWNCODE_PART,	"_lbulkycalf",
	GBLOWN_MESO_CALFLEFT4,			GBLOWN_MESO_CALFLEFT5,			GBLOWNAREA_LCALF,	GBLOWNCODE_PART,	"_lbosscalf",
	GBLOWN_MESO_CALFLEFT5,			GBLOWN_MESO_CALFBOLTLEFT,		GBLOWNAREA_LCALF,	GBLOWNCODE_PART,	"_lcalf_t_b",

	//shared:
	GBLOWN_MESO_CALFBOLTLEFT,		GBLOWN_MESO_FOOTLEFT,			GBLOWNAREA_LCALF,	GBLOWNCODE_BOLT,	"abolt_calf_l",

	//meso:
	GBLOWN_MESO_FOOTLEFT,			GBLOWN_MESO_FOOTLEFT2,			GBLOWNAREA_LFOOT,	GBLOWNCODE_PART,	"_lbootfoot",
	GBLOWN_MESO_FOOTLEFT2,			GBLOWN_MESO_NUM,				GBLOWNAREA_LFOOT,	GBLOWNCODE_PART,	"_lshoefoot",

	GBLOWN_MESO_GROIN,				GBLOWN_MESO_NUM,				GBLOWNAREA_GROIN,	GBLOWNCODE_SPECIAL,	"",
	GBLOWN_MESO_NUM,				GBLOWN_MESO_NUM,				GBLOWNAREA_FINISH,	GBLOWNCODE_SPECIAL,	"",
};

typedef struct
{
	char		*name;
	qboolean	flimsy;//will fall off if damage occurs nearby
	qboolean	hatHead;//when i get blown off, need to switch to non-hat head (which may be the same as hat head)
	qboolean	stopsDmg; // kef -- glasses shouldn't stop bullets
} boltOnInfo_t;
//fixme--change this to include bbox info for shot prt, some kind of indication of what the monster model should do to make up for part loss (eg. change from headbald to headcrew)
boltOnInfo_t shootableParts[]=
{
//	name					flimsy	hatHead	stopsDmg
	"acc_antenna",			false,	false,	false,
	"acc_backpack_low",		false,	false,	true,
	"acc_backpack_smll",	false,	false,	true,
	"acc_box_hip",			false,	false,	true,
	"acc_box_hip_lrg",		false,	false,	true,
	"acc_box_hip_smll",		false,	false,	true,
	"acc_ecto_glasses",		true,	false,	false,
	"acc_faceplate",		false,	false,	false,
	"acc_faceplate2",		false,	false,	false,
//	"acc_gas_mask",			false,	false,	false,
	"acc_gas_mask2",		true,	false,	false,
	"acc_glasses",			true,	false,	false,
	"acc_hat_baseball",		false,	true,	false,
	"acc_hat_beret",		false,	true,	false,
	"acc_hat_flop2",		false,	true,	false,
	"acc_hat_general",		false,	true,	false,
	"acc_hat_general2",		false,	true,	false,
	"acc_hat_hard",			false,	true,	true,
	"acc_hat_tall",			false,	true,	false,
	"acc_hat_top",			false,	true,	false,
	"acc_hat_turbin",		false,	true,	false,
	"acc_helmet",			false,	true,	true,
//	"acc_helmet2",			false,	true,	false,
	"acc_holster",			false,	false,	false,
	"acc_machette",			false,	false,	false,
	"acc_nightvision2",		false,	false,	false,
	"acc_sword",			false,	false,	false,
	"w_assault_rifle",		false,	false,	true,
	"w_autoshotgun",		false,	false,	true,
	"w_bat",				false,	false,	true,
	"w_briefcase",			true,	false,	true,
	"w_flamethrower",		false,	false,	true,
	"w_kantana",			false,	false,	true,
	"w_knife",				false,	false,	true,
	"w_machete",			false,	false,	true,
	"w_machinegun",			false,	false,	true,
	"w_machinepistol",		false,	false,	true,
	"w_mpg",				false,	false,	true,
	"w_pistol1",			false,	false,	true,
	"w_pistol2",			false,	false,	true,
	"w_rocket",				false,	false,	true,
	"w_sack",				true,	false,	true,
	"w_shotgun",			false,	false,	true,
	"w_sniperrifle",		false,	false,	true,
	"",						false,	false,	false,
};

#define AI_GORETEST_PRINTF if (ai_goretest->value)gi.dprintf

int GetDamageLevel(int amount)
{
	if (amount<=0)
	{
		return 0;
	}
	if (amount<=100)
	{
		return 1;
	}
	if (amount<=200)
	{
		return 2;
	}
	return 3;
}

int GetMinimumDamageForLevel(int damageLevel)
{
	if (damageLevel<=0)
	{
		return 0;
	}
	if (damageLevel<=1)
	{
		return 1;
	}
	if (damageLevel<=2)
	{
		return 101;
	}
	return 201;
}

///find me a home
void EntToWorldMatrix(vec3_t org, vec3_t angles, Matrix4 &m);
Vect3			trStartKnarlyGilHack;

char* gznames[]=
{
	"GZ_HEAD_FRONT",
	"GZ_HEAD_BACK",
	"GZ_NECK",
	"GZ_SHLDR_RIGHT_FRONT",
	"GZ_SHLDR_RIGHT_BACK",
	"GZ_SHLDR_LEFT_FRONT",
	"GZ_SHLDR_LEFT_BACK",
	"GZ_CHEST_FRONT",
	"GZ_CHEST_BACK",
	"GZ_ARM_UPPER_RIGHT",
	"GZ_ARM_UPPER_LEFT",
	"GZ_ARM_LOWER_RIGHT",
	"GZ_ARM_LOWER_LEFT",
	"GZ_GUT_FRONT",
	"GZ_GUT_BACK",
	"GZ_GROIN",
	"GZ_LEG_UPPER_RIGHT_FRONT",
	"GZ_LEG_UPPER_RIGHT_BACK",
	"GZ_LEG_UPPER_LEFT_FRONT",
	"GZ_LEG_UPPER_LEFT_BACK",
	"GZ_LEG_LOWER_RIGHT",
	"GZ_LEG_LOWER_LEFT",
	"GZ_FOOT_RIGHT",
	"GZ_FOOT_LEFT",
	"GZ_GUT_FRONT_EXTRA",
	"GZ_GUT_BACK_EXTRA",
	"NUM_HUMANGOREZONES"
};

#define MAX_VEL_COMPONENT 50.0

/*
void blownpartremove_think(edict_t *self)
{
	int i;
	bool removeme=true;
	edict_t *client;
	vec3_t client_facing, client_to_part;
	//only remove if it won't look screwy to clients
	for (i=0 ; i<game.maxclients ; i++)
	{
		client = &g_edicts[1+i];
		if (!client->inuse)
		{
			continue;
		}
		VectorSubtract(self->s.origin, client->s.origin, client_to_part);

		//if e's greater than 800 units away, assume he can't see me
		if (VectorLengthSquared(client_to_part)>640000)
		{
			continue;
		}

		if (!gi.inPVS(self->s.origin, self->s.origin))
		{
			continue;
		}
		AngleVectors(client->s.angles, client_facing, NULL, NULL);
		VectorNormalize(client_to_part);
		//if client is looking in my general dir, don't remove for sure
		if (DotProduct(client_to_part, client_facing)>-.2)
		{
			removeme = false;
			break;
		}
	}

	self->nextthink = level.time + 15;

	if (removeme)
	{
		G_FreeEdict(self);
		return;
	}
}
*/

void blownpartremove_think(edict_t *self)
{
	int i;
	bool freeMe=true;
	edict_t *ent;
	vec3_t ent_facing, ent_to_me;

	if (!self->touch_debounce_time)
	{
		self->touch_debounce_time = level.time - 0.1;
	}

	//if it's been over 60 seconds since i was severed, git ridda me no matter what.
	if (level.time-self->touch_debounce_time<BODY_PART_MAXLIFE)
	{
		//hasn't been too long--only remove if it won't look screwy to clients
		for (i=0 ; i<game.maxclients ; i++)
		{
			ent = &g_edicts[1+i];
			if (!ent->inuse)
			{
				continue;
			}
			VectorSubtract(ent->s.origin, self->s.origin, ent_to_me);

			//if e's greater than 800 units away, assume he can't see me
			if (VectorLengthSquared(ent_to_me)>640000)
			{
				continue;
			}

			//if client can't possibly see me, will have no effect on whether i remove or no
			if (!gi.inPVS(self->s.origin, self->s.origin))
			{
				continue;
			}
			AngleVectors(ent->s.angles, ent_facing, NULL, NULL);
			VectorNormalize(ent_to_me);
			//if client is looking in my general dir, don't remove for sure
			if (DotProduct(ent_to_me, ent_facing)<.2)
			{
				freeMe = false;
				break;
			}
		}
	}

	if (freeMe)
	{
		FXA_SetFadeOut(self, BLOWN_PART_FADE);

		// remove when we are done fading.
		self->nextthink=level.time+BLOWN_PART_FADE+0.1;
		self->think = G_FreeEdict;
		return;
	}

	self->nextthink = level.time + 1.0;
}

GroundCallback			TheGroundCallback;
SetVectorCallback		TheSetVectorCallback;
FireCallBack			TheFireCallBack;
JumpCallBack			TheJumpCallBack;
ThudCallBack			TheThudCallBack;
KneeCallBack			TheKneeCallBack;
InAirCallBack			TheInAirCallBack;
FollowGenericPivot		TheFollowGenericPivot;
MouthCallback			TheMouthCallback;
FootRightCallback		TheFootRightCallback;
FootLeftCallback		TheFootLeftCallback;
MonsterSeqEndCallback	TheMonsterSeqEndCallback;
MonsterSeqBeginCallback	TheMonsterSeqBeginCallback;
MonsterSoundCallBack	TheMonsterSoundCallBack;
ThrowCallBack			TheThrowCallBack;
DropWeaponCallBack		TheDropWeaponCallBack;

EnemySoundPrecacheCallBack soundPrecacher;

LimbEOSCallback TheLimbEOSCallback;

//////////////////////////////////////////////////////////////////////////////////
//
//	shared among all organic guys!
//
//////////////////////////////////////////////////////////////////////////////////

bodyorganic_c::bodyorganic_c()
{
	useGround2=false;
	spout = -1;
	numGoreZones=0;
	gzones = NULL;
	memset(gzone_levels, 0, sizeof(gzone_levels));
}

bodyorganic_c::~bodyorganic_c()
{
	if (gzones)
	{
		delete [] gzones;
	}
}

void bodyorganic_c::InitializeGoreZoneLevels(edict_t &monster)
{
	GhoulID	tempMat=NULL;
	char zone[20];
	int i;

	if (monster.ghoulInst)
	{
		tempMat=monster.ghoulInst->GetGhoulObject()->FindMaterial("gz");
	}

	if (!tempMat)
	{
		return;
	}

	Com_sprintf(zone, 20, "gz_sing1");
	gzone_levels[0][0]=monster.ghoulInst->GetGhoulObject()->FindSkin(tempMat,zone);
	for (i=1;i<=2;i++)
	{
		Com_sprintf(zone, 20, "gz_sing%d",i+1);
		gzone_levels[0][i]=monster.ghoulInst->GetGhoulObject()->FindSkin(tempMat,zone);
	}
	gzone_levels[0][3]=monster.ghoulInst->GetGhoulObject()->FindSkin(tempMat,"gz_sing_k");

	Com_sprintf(zone, 20, "gz_multi1");
	gzone_levels[1][0]=monster.ghoulInst->GetGhoulObject()->FindSkin(tempMat,zone);
	for (i=1;i<=2;i++)
	{
		Com_sprintf(zone, 20, "gz_multi%d",i+1);
		gzone_levels[1][i]=monster.ghoulInst->GetGhoulObject()->FindSkin(tempMat,zone);
	}
	gzone_levels[1][3]=monster.ghoulInst->GetGhoulObject()->FindSkin(tempMat,"gz_multi_k");

	Com_sprintf(zone, 20, "gz_mass1");
	gzone_levels[2][0]=monster.ghoulInst->GetGhoulObject()->FindSkin(tempMat,zone);
	for (i=1;i<=2;i++)
	{
		Com_sprintf(zone, 20, "gz_mass%d",i+1);
		gzone_levels[2][i]=monster.ghoulInst->GetGhoulObject()->FindSkin(tempMat,zone);
	}
	gzone_levels[2][3]=monster.ghoulInst->GetGhoulObject()->FindSkin(tempMat,"gz_mass_k");

#if _DEBUG
	assert(gzone_levels[0][3]&&gzone_levels[1][3]&&gzone_levels[2][3]);
#endif
}
void bodyorganic_c::BlowPart(edict_t &monster, gz_blown_part *part, edict_t *dangler)
{
	GhoulID	blowpart=0;
	GhoulID blowbolt=0;
	ggOinstC *myInstance=game_ghoul.FindOInst(monster.ghoulInst);
	if (part->area==GBLOWNAREA_FINISH || !myInstance || (!dangler && (part->area != GBLOWNAREA_HEAD)))
	{
		return;
	}
	if (part->code == GBLOWNCODE_PART)
	{
		blowpart=monster.ghoulInst->GetGhoulObject()->FindPart(part->name);
		if (blowpart)
		{
			if (dangler && myInstance->GetInstPtr()->GetPartOnOff(blowpart))
			{
				dangler->ghoulInst->SetPartOnOff(blowpart,true);
			}
			myInstance->GetInstPtr()->SetPartOnOff(blowpart,false);
		}
	}
	else if (part->code == GBLOWNCODE_BOLT)
	{
		blowbolt=monster.ghoulInst->GetGhoulObject()->FindPart(part->name);
		if (blowbolt)
		{
			ggBinstC	*bolted;
			ggOinstC *blownInstance=NULL;
			if (dangler && dangler->ghoulInst)
			{
				blownInstance=game_ghoul.FindOInst(dangler->ghoulInst);
			}

			if (!stricmp(part->name, "wbolt_hand_r")||!stricmp(part->name, "wbolt_hand_l"))
			{
				DropWeapon(&monster, monster.ghoulInst);
			}

			while (bolted=myInstance->GetBoltInstance(blowbolt))
			{
				if (blownInstance&&(!monster.client || (stricmp(part->name, "wbolt_hand_r")&&stricmp(part->name, "wbolt_hand_l"))))
				{
//					AddBoltedItem(monster, blowbolt, "Enemy/bolt", bolted->GetBolteeObject()->GetSubName(), bolted->GetBolt(), blownInstance);
					AddBoltedItem(monster, blowbolt, "Enemy/bolt", bolted->GetBolteeObject()->GetSubName(), bolted->GetBolt(), blownInstance, bolted->GetBolteeObject()->GetSkinName());
				}
				myInstance->RemoveBoltInstance(bolted);
			}
		}
	}
	else if (part->code == GBLOWNCODE_SPECIAL)
	{
		BlowSpecialPart(monster, part, dangler);
	}
	BlowPart(monster, GetBlownPartForIndex(part->child_index), dangler);
}

//affect surrounding gzones when hit
void bodyorganic_c::ShakePart(edict_t &monster, gz_blown_part *part, Vect3 &Direction)
{
	GhoulID	blowbolt=0;
	ggOinstC *myInstance=game_ghoul.FindOInst(monster.ghoulInst);
	if (part->area==GBLOWNAREA_FINISH || !myInstance)
	{
		return;
	}
	if (part->code == GBLOWNCODE_BOLT)
	{
		blowbolt=monster.ghoulInst->GetGhoulObject()->FindPart(part->name);
		if (blowbolt)
		{
			ggBinstC	*bolted;

			bolted=myInstance->GetBoltInstance(blowbolt);
			if (bolted)
			{
				Matrix4 ZoneMatrix;
				Vect3	zonePos;
				monster.ghoulInst->GetBoltMatrix(level.time,ZoneMatrix,blowbolt,IGhoulInst::MatrixType::Entity);
				ZoneMatrix.GetRow(3,zonePos);
				DropBoltOn(monster, bolted, zonePos, Direction, 1);
			}
		}	
	}
	else if (part->code == GBLOWNCODE_SPECIAL)
	{
		ShakeSpecialPart(monster, part, Direction);
	}
	ShakePart(monster, GetBlownPartForIndex(part->child_index), Direction);
}

void bodyorganic_c::Drip(edict_t &monster)
{
	if (spout == -1)
	{
		FX_MakeDecalBelow(monster.s.origin,FXDECAL_BLOODPOOL,0);
	}
	else
	{
		vec3_t pos;
		VectorAdd(monster.s.origin,monster.pos1,pos);
		FX_MakeDecalBelow(pos,FXDECAL_BLOODPOOL,0);
	}
}

void bodyorganic_c::AllocateGoreZones(edict_t &monster)
{
	if (gzones)
	{
		delete [] gzones;
	}
	numGoreZones=0;
}

void bodyorganic_c::BlowZone(edict_t &monster, gz_code part, gz_code originalPart, edict_t *dangler)
{
	ggOinstC *myInstance=game_ghoul.FindOInst(monster.ghoulInst);
	if (part==numGoreZones || !myInstance)
	{
		return;
	}
	if (gzones[part].id)
	{
		//copy over gorezones that are turned on
		if (dangler && myInstance->GetInstPtr()->GetPartOnOff(gzones[part].id))
		{
			dangler->ghoulInst->SetPartOnOff(gzones[part].id,true);
		}
		//...as for gorezones that aren't turned on, consider turning them on, too
		else
		{
			//only do this if gorezones are ok!
			if (!lock_gorezones)
			{
				//pretty good chance i'll turn the part on
				if (dangler && myInstance->GetInstPtr()->GetPartOnOff(gzones[part].parent_id)/* && gi.irand(0,10)*/)
				{
					dangler->ghoulInst->SetPartOnOff(gzones[part].id,true);
					//set the skin to any of the non-armor skins
					dangler->ghoulInst->SetFrameOverride("gz", gzone_levels[gi.irand(0,2)][gi.irand(0,2)], gzones[part].id);
				}
			}
		}
		myInstance->GetInstPtr()->SetPartOnOff(gzones[part].id,false);
	}
	if (gzones[part].cap)
	{
		if (dangler && myInstance->GetInstPtr()->GetPartOnOff(gzones[part].cap))
		{
			dangler->ghoulInst->SetPartOnOff(gzones[part].cap,true);
		}
		myInstance->GetInstPtr()->SetPartOnOff(gzones[part].cap,false);
	}
	if (gzones[part].capbolt)
	{
			char		headSkinName[128];
			char		faceSkinName[128];
			GhoulID		headSkin;
			GhoulID		faceSkin;
			ggBinstC	*theNewBolt;
			

			ggBinstC	*bolted;
			ggOinstC *blownInstance=NULL;

			if (dangler && dangler->ghoulInst)
			{
				blownInstance=game_ghoul.FindOInst(dangler->ghoulInst);
			}

			while (bolted=myInstance->GetBoltInstance(gzones[part].capbolt))
			{
				if (blownInstance)
				{
					theNewBolt=AddBoltedItem(monster, gzones[part].capbolt, bolted->GetBolteeObject()/*"Enemy/bolt"*/, bolted->GetBolteeObject()->GetSubName(), bolted->GetBolt(), blownInstance);
				}

				//irritating skin craziness for half-heads
				if (theNewBolt && bolted->GetInstPtr() && bolted->GetBolteeObject() &&
					bolted->GetBolteeObject()->GetSubName() && !strcmp(bolted->GetBolteeObject()->GetSubName(), "g_half_head"))
				{
					// for the half head bolt on, we want to put the living guy's skins on the new bolt on
					
					headSkin = bolted->GetInstPtr()->GetFrameOverride("h");
					if (headSkin)
					{
						bolted->GetInstPtr()->GetGhoulObject()->GetSkinName(headSkin, headSkinName);
						theNewBolt->GetInstPtr()->SetFrameOverride("h", headSkinName); 
					}
					
					faceSkin = bolted->GetInstPtr()->GetFrameOverride("f");
					if (faceSkin)
					{
						bolted->GetInstPtr()->GetGhoulObject()->GetSkinName(faceSkin, faceSkinName);
						theNewBolt->GetInstPtr()->SetFrameOverride("f", faceSkinName); 
					}

					theNewBolt->GetInstPtr()->SetFrameOverride("gz", "gz_half_head"); 
				}

				myInstance->RemoveBoltInstance(bolted);
			}
			myInstance->GetInstPtr()->SetPartOnOff(gzones[part].capbolt,false);
//		myInstance->RemoveBolt(gzones[part].capbolt);
	}
	if (gzones[part].blownCapBolt)
	{
			ggBinstC	*bolted;
			ggOinstC *blownInstance=NULL;
			edict_t		*knife = NULL;
			vec3_t		fwd;

			if (dangler && dangler->ghoulInst)
			{
				blownInstance=game_ghoul.FindOInst(dangler->ghoulInst);
			}

			while (bolted=myInstance->GetBoltInstance(gzones[part].blownCapBolt))
			{
				if (0 == strcmp(bolted->GetBolteeObject()->GetSubName(), "w_knife"))
				{ // if the thing attached to this blownCapBolt is a knife, create a new
				  //knife entity and drop it like a spawned item
					if (knife = G_Spawn())
					{
						Matrix4		matOld;
						IGhoulInst	*oldKnife, *newKnife;

						game_ghoul.SetSimpleGhoulModel(knife, "Enemy/Bolt", "w_knife");
						if ((oldKnife = bolted->GetInstPtr()) &&
							(newKnife = knife->ghoulInst))
						{
							oldKnife->GetXForm(matOld);
							newKnife->SetXForm(matOld);
						}
						GetGhoulPosDir(monster.s.origin, monster.s.angles,
							bolted->GetInstPtr(), NULL, "tip", knife->s.origin, fwd, NULL, NULL);
						knife->spawnflags|=DROPPED_ITEM;
						I_SpawnKnife(knife);
						knife->gravity = 1.0;
//						itemDropToFloor(knife);
					}
				}
				myInstance->RemoveBoltInstance(bolted);
			}
			myInstance->GetInstPtr()->SetPartOnOff(gzones[part].blownCapBolt,false);
	}
	if (gzones[part].bolt)
	{
			ggBinstC	*bolted;
			ggOinstC *blownInstance=NULL;
			if (dangler && dangler->ghoulInst)
			{
				blownInstance=game_ghoul.FindOInst(dangler->ghoulInst);
			}

			while (bolted=myInstance->GetBoltInstance(gzones[part].bolt))
			{
				if (blownInstance)
				{
					AddBoltedItem(monster, gzones[part].bolt, "Enemy/bolt", bolted->GetBolteeObject()->GetSubName(), bolted->GetBolt(), blownInstance);
				}
				myInstance->RemoveBoltInstance(bolted);
			}
//		myInstance->RemoveBolt(gzones[part].bolt);
	}
	if (gzones[part].childzone != originalPart)
	{
		BlowZone(monster, gzones[part].childzone, originalPart,dangler);
	}
}

qboolean bodyorganic_c::VerifySequence(edict_t &monster, mmove_t *newanim, int reject_actionflags)
{
	ai_c	*ai;

	if (!newanim || newanim->actionFlags&reject_actionflags)
	{
		return false;
	}

	if (!IsAvailableSequence(monster, newanim))
	{
		return false;
	}

	//if i'm ai-less, accept the sequence here; otherwise, make sure bbox stuff'll work
	if (!monster.ai)
	{
		return true;
	}

	ai=(ai_c *)((ai_public_c *)monster.ai);

	if (newanim->suggested_action==ACTCODE_DEATH)
	{
		return ai->AttemptSetBBox(monster,newanim->bbox,true);
	}
	return ai->AttemptSetBBox(monster,newanim->bbox);
}

void bodyorganic_c::SetGoreZone(edict_t &monster, char *zonename, gz_code slot)
{
	GhoulID	tempHole;

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart(zonename);
	if (tempHole)
	{
		gzones[slot].id=tempHole;
	}
//	else
//	{
//		gi.dprintf("SetGoreZone Error! Couldn't find %s!\n",zonename);
//	}
}

void bodyorganic_c::SetGoreCap(edict_t &monster, char *capname, gz_code slot)
{
	GhoulID	tempHole;

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart(capname);
	if (tempHole)
	{
		gzones[slot].cap=tempHole;
	}
//	else
//	{
//		gi.dprintf("SetGoreCap Error! Couldn't find %s!\n",capname);
//	}
}

void bodyorganic_c::SetGoreBlownCap(edict_t &monster, char *capname, gz_code slot)
{
	GhoulID	tempHole;

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart(capname);
	if (tempHole)
	{
		gzones[slot].blownCap=tempHole;
	}
//	else
//	{
//		gi.dprintf("SetGoreBlownCap Error! Couldn't find %s!\n",capname);
//	}
}

void bodyorganic_c::SetGoreCapBolt(edict_t &monster, char *capname, gz_code slot)
{
	gzones[slot].capbolt=monster.ghoulInst->GetGhoulObject()->FindPart(capname);
//	if (!gzones[slot].capbolt)
//	{
//		gi.dprintf("SetGoreCapBolt Error! Couldn't find %s!\n",capname);
//	}
}

void bodyorganic_c::SetGoreBlownCapBolt(edict_t &monster, char *capname, gz_code slot)
{
	gzones[slot].blownCapBolt=monster.ghoulInst->GetGhoulObject()->FindPart(capname);
//	if (!gzones[slot].blownCapBolt)
//	{
//		gi.dprintf("SetGoreBlownCapBolt Error! Couldn't find %s!\n",capname);
//	}
}

void bodyorganic_c::SetGoreCapPiece(edict_t &monster, char *capname, char *boltname, gz_code slot)
{
	//it's a head--do magical hacking right here.
	if (!stricmp(capname,"g_half_head"))
	{
		GhoulID myHead=monster.ghoulInst->GetFrameOverride("h");
		//find the half-head with the same skin as me...
		if (myHead)
		{
			char headskin[256];
			monster.ghoulInst->GetGhoulObject()->GetSkinName(myHead,headskin);
			gzones[slot].capPiece=game_ghoul.FindObject("Enemy/Bolt", capname, false, headskin);
		}
		else
		{
			//eh? hey, there's supposed to be a head here...
		}
	}
	//not a head--pretend the hack doesn't exist...
	else
	{
		gzones[slot].capPiece=game_ghoul.FindObject("Enemy/Bolt", capname);
	}

	if (boltname[0] && gzones[slot].capPiece)
	{
		gzones[slot].capPieceBolt=gzones[slot].capPiece->GetMyObject()->FindPart(boltname);
	}
}

void bodyorganic_c::SetGoreBolt(edict_t &monster, char *boltname, gz_code slot)
{
	GhoulID	tempHole;

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart(boltname);
	if (tempHole)
	{
		gzones[slot].bolt=tempHole;
	}
}

void BlownPartTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	gi.sound (self, CHAN_BODY, gi.soundindex ("impact/gore/limbfall.wav"), .6, ATTN_NORM, 0);
	self->touch = NULL;	
}

void GetReadyForBlownPartAIToTakeOver(edict_t *ent)
{
	ent->nextthink = level.time + 0.1;
}

edict_t *bodyorganic_c::MakeBlownPart(edict_t &monster, gz_code part, vec3_t dir, ggOinstC **blownInst, bool includeEffects)
{
	edict_t *blownPart;
	Matrix4 m,m1,m2;
	int i;
	ggObjC		*object;
	GhoulID		BlownCapBolt;

	blownPart = G_Spawn();
	blownPart->solid = SOLID_NOT;
	blownPart->clipmask = MASK_DEADSOLID|CONTENTS_DEADMONSTER;
	blownPart->svflags |= SVF_DEADMONSTER;
	VectorScale(dir, 75, blownPart->velocity);
	blownPart->velocity[2] = gi.irand(30, 70);
	VectorSet(blownPart->avelocity, 0, 0, 0);
	object = game_ghoul.FindObject(monster.ghoulInst->GetGhoulObject());	
	blownPart->owner = &monster;
	blownPart->ai = ai_c::Create(AI_BLOWNPART, blownPart, object->GetName(), object->GetSubName());
//	blownPart->think = NULL;
	blownPart->think = GetReadyForBlownPartAIToTakeOver;
	blownPart->touch = BlownPartTouch;
	blownPart->nextthink=level.time+0.1;
	blownPart->movetype = MOVETYPE_TOUCHNOTSOLID;
	blownPart->friction = 0.5;
	blownPart->gravity =  1.0;
	blownPart->airresistance = 0.1;
	blownPart->bouyancy = 0.7;
	blownPart->elasticity = 0.5;


	//if the blown piece successfully gave itself a ghoul instance derived from the same object as the parent, just change the skins
/*	if (blownPart->ghoulInst && blownPart->ghoulInst->GetGhoulObject()==monster.ghoulInst->GetGhoulObject())
	{
		GhoulID tmat;

		//this is just for universal overrides! need to do per-part overrides too (should just be for gzones)!
		for (tmat=object->GetMyObject()->NumMaterials();tmat>0;tmat--)
		{
			blownPart->ghoulInst->SetFrameOverride(tmat, monster.ghoulInst->GetFrameOverride(tmat));
		}
	}
	else
	{
*/
	*blownInst = object->CloneInstance(blownPart, monster.ghoulInst);
//	}


	blownPart->s.renderfx = RF_GHOUL;

	if (gzones[part].blownCapBolt)
	{
		BlownCapBolt=gzones[part].blownCapBolt;
	}
	else
	{
		BlownCapBolt=gzones[part].capbolt;
	}

	if (BlownCapBolt&&gzones[part].parent_id)
	{
		if(includeEffects)
		{
			FX_BloodCloud(blownPart, BlownCapBolt, 400);
		}
		blownPart->gib_health = gzones[part].parent_id;
			
		// get the transformation from entity space to world space
		Matrix4	EntityToWorld;

		EntToWorldMatrix(monster.s.origin, monster.s.angles, EntityToWorld);

		// get the transformation from bolt space to entity space
		Matrix4	BoltToEntity;

		monster.ghoulInst->GetBoltMatrix(level.time,BoltToEntity,BlownCapBolt,IGhoulInst::MatrixType::Entity);

		// multiply to get transform from bolt space to world space
		Matrix4	BoltToWorld;

		BoltToWorld.Concat(BoltToEntity, EntityToWorld);

		// 3rd row of BoltToWorld is the world coordinates of the bolt
		vec3_t	LimbLoc;
		BoltToWorld.GetRow(3,*(Vect3 *)LimbLoc);

//		blownPart->ghoulInst->SetMyBolt(GhoulID(0)); // otherwise will interfere with computation
		blownPart->ghoulInst->SetMyBolt(monster.ghoulInst->GetMyBolt()); //ECK! will be different for players & monsters!

		blownPart->ghoulInst->Pause(level.time); // want limb to in the exact position is was during the sever process
//		blownPart->ghoulInst->AddNoteCallBack(&TheLimbEOSCallback, "EOS");

		// we are going to work in entity space.
		Matrix4 id;

		id.Identity();
		Vect3 mins,maxs;
		Vect3 center;

		//now get realistic bbox/xform info		
		blownPart->ghoulInst->GetBoundBox(level.time+.3,id,mins,maxs,gzones[part].parent_id,true);
		//gi.dprintf("id: %d  mins: %2.2f %2.2f %2.2f  maxs: %2.2f %2.2f %2.2f\n",gzones[part].parent_id,mins[0],mins[1],mins[2],maxs[0],maxs[1],maxs[2]);
		center=mins;
		center+=maxs;
		center*=.5f;

		VectorSet (blownPart->mins, mins.x() - center.x(), mins.y() - center.y(), mins.z() - center.z());
		VectorSet (blownPart->maxs, maxs.x() - center.x(), maxs.y() - center.y(), maxs.z() - center.z());

		// put limb at bolt
		VectorCopy(LimbLoc, blownPart->s.origin);

		// now put in correct orientation
		Matrix4 t1, t2, XForm, final;

		VectorClear(blownPart->s.angles);
		monster.ghoulInst->GetXForm(XForm);
		t1.Concat(XForm,EntityToWorld);
		EntToWorldMatrix(blownPart->s.origin, blownPart->s.angles, t2);
		t2.Inverse(t2);
		final.Concat(t1,t2);
		blownPart->ghoulInst->SetXForm(final);

	
		{	// Add some velocity to the body part away from the torso
			vec3_t diff;

			VectorSubtract(blownPart->s.origin, monster.s.origin, diff);
			VectorNormalize(diff);

			VectorSet(blownPart->velocity, gi.flrand(-50,50), gi.flrand(-50,50), gi.flrand(-50,50));
			VectorMA(blownPart->velocity, gi.flrand(100,200), diff, blownPart->velocity);

			blownPart->s.angles[YAW] += gi.flrand(-30,30);
		}


		// perform analysis on limbs to determine if it was shot off in a relatively vertical position
		// if it was, begin rotating the limb.  Note that this is quite hacky, and will only work
		// on creatures with roughly long limbs (as opposed to square or something).

		DoLimbRotate(blownPart);
	}
	else if (IsAvailableSequence(monster, &generic_move_stand))
	{
		blownPart->ghoulInst->Play(GetSequence(monster, &generic_move_stand),level.time,0.0,true,IGhoulInst::Hold, false, false);
		VectorCopy(monster.s.origin, blownPart->s.origin);
		blownPart->s.origin[2] += 24;
		VectorSet (blownPart->mins, -8, -8, -8);
		VectorSet (blownPart->maxs, 8, 8, 0);
	}
	else
	{
		G_FreeEdict(blownPart);
		return NULL;
	}

	blownPart->health = 300;
	gi.linkentity(blownPart);

	for (i=1;i<=monster.ghoulInst->GetGhoulObject()->NumParts();i++)
	{
		if (monster.ghoulInst->GetGhoulObject()->GetPartType(i)==ptMesh)
		{
			blownPart->ghoulInst->SetPartOnOff(i,false);
		}
	}

	if(includeEffects)
	{
//		gi.sound (&monster, CHAN_BODY, gi.soundindex ("impact/gore/spout.wav"), .8, ATTN_NORM, 0);
		FX_Bleed(blownPart, BlownCapBolt, 50);
	}

	//testing...
//	TestBoltPosition(blownPart->s.origin, blownPart->s.angles);

	return blownPart;
}

void bodyorganic_c::DoLimbRotate(edict_t * monster)
{
/*
	This fuction finds the greatest axial dimension.  If it is z, we are going to assume that this is
	an arm or a leg in an upright position and begin rotating it so that it doesn't look too screwy
	when it lands...  I hope...
*/
	float	xlength, ylength, zlength;

	xlength = monster->maxs[0] - monster->mins[0];
	ylength = monster->maxs[1] - monster->mins[1];
	zlength = monster->maxs[2] - monster->mins[2];

	if ((zlength > xlength) && (zlength > ylength))
	{
		monster->avelocity[0] = -150;
	}

	return;
}

void bodyorganic_c::UseGround2(edict_t &monster, qboolean toggleVal)
{
	GhoulID tempID;
	if (toggleVal && !useGround2)
	{
		tempID=monster.ghoulInst->GetGhoulObject()->FindPart("quake_ground");
		if (tempID)
		{
			monster.ghoulInst->RemoveMatrixCallBack(&TheGroundCallback,tempID,IGhoulInst::MatrixType::Entity);
		}
		tempID=monster.ghoulInst->GetGhoulObject()->FindPart("quake_ground2");
		if (tempID)
		{
			monster.ghoulInst->AddMatrixCallBack(&TheGroundCallback,tempID,IGhoulInst::MatrixType::Entity);
		}
	}
	else if (useGround2 && !toggleVal)
	{
		tempID=monster.ghoulInst->GetGhoulObject()->FindPart("quake_ground2");
		if (tempID)
		{
			monster.ghoulInst->RemoveMatrixCallBack(&TheGroundCallback,tempID,IGhoulInst::MatrixType::Entity);
		}
		tempID=monster.ghoulInst->GetGhoulObject()->FindPart("quake_ground");
		if (tempID)
		{
			monster.ghoulInst->AddMatrixCallBack(&TheGroundCallback,tempID,IGhoulInst::MatrixType::Entity);
		}
	}
	useGround2=toggleVal;
}

bodyorganic_c::bodyorganic_c(bodyorganic_c *orig)
: body_c(orig)
{
	int		i, j;

	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 4; j++)
		{
			gzone_levels[i][j] = orig->gzone_levels[i][j];
		}
	}
	useGround2 = orig->useGround2;
	spout = orig->spout;
	numGoreZones = 0;
	gzones = NULL;
}

void bodyorganic_c::Evaluate(bodyorganic_c *orig)
{
	useGround2 = orig->useGround2;
	spout = orig->spout;
	numGoreZones = 0;
	gzones = NULL;

	body_c::Evaluate(orig);

	AllocateGoreZones(*owner);
	RecognizeGoreZones(*owner);
	ReadGZ();
}

void bodyorganic_c::WriteGZ()
{
	int		*damages;
	int		i;

	if(numGoreZones)
	{
		damages = new int [numGoreZones];

		for(i = 0; i < numGoreZones; i++)
		{
			damages[i] = gzones[i].damage;
		}
		gi.AppendToSavegame('AIGZ', damages, numGoreZones * sizeof(int));
		delete [] damages;
	}
}

void bodyorganic_c::ReadGZ()
{
	int		*damages;
	int		i;

	if(numGoreZones)
	{
		damages = new int [numGoreZones];

		gi.ReadFromSavegame('AIGZ', damages, numGoreZones * sizeof(int));
		for(i = 0; i < numGoreZones; i++)
		{
			gzones[i].damage = damages[i];
			gzones[i].damage_level = GetDamageLevel(damages[i]);
		}

		delete [] damages;
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	human specific!
//
/////////////////////////////////////////////////////////////////////////////

void bodyhuman_c::GetLevelDeathSoundDir(char *putSuffixHere, deathvoice_code death)
{
	deathvoice_code theVoice=death;
	if (!putSuffixHere)
	{
		return;
	}

	if (theVoice==DEATHVOICE_PLAYER)
	{
		strcpy(putSuffixHere, voiceDirDeath);
		return;
	}
	putSuffixHere[0]=0;

	//mess with deathvoice code here
	switch(GetInterpCode())
	{
	default:
	case LEVCODE_UNKNOWN:
	case LEVCODE_NYC_SUBWAY:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_SKINHEAD;
		}
		break;
	case LEVCODE_AFR_TRAIN:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_ARAB;
		}
		break;
	case LEVCODE_KOS_SEWER:
	case LEVCODE_NYC_SUBWAY2:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_RUSSIAN;
		}
		break;
	case LEVCODE_KOS_BIGGUN:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_RUSSIAN;
		}
		break;
	case LEVCODE_KOS_HANGAR:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_RUSSIAN;
		}
		break;
	case LEVCODE_SIB_CANYON:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_RUSSIAN;
		}
		break;
	case LEVCODE_SIB_BASE:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_RUSSIAN;
		}
		break;
	case LEVCODE_SIB_PLANT:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_RUSSIAN;
		}
		break;
	case LEVCODE_IRQ_TOWNA:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_ARAB;
		}
		break;
	case LEVCODE_IRQ_BUNKER:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_ARAB;
		}
		break;
	case LEVCODE_IRQ_CARGO:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_ARAB;
		}
		break;
	case LEVCODE_NYC_WARE:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_SKINHEAD;
		}
		break;
	case LEVCODE_NYC_STEAM:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_SKINHEAD;
		}
		break;
	case LEVCODE_NYC_STREETS:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_SKINHEAD;
		}
		break;
	case LEVCODE_AFR_YARD:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_ARAB;
		}
		break;
	case LEVCODE_AFR_HOUSE:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_ARAB;
		}
		break;
	case LEVCODE_AFR_FACT:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_ARAB;
		}
		break;
	case LEVCODE_TOK_STREET:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_ASIAN;
		}
		break;
	case LEVCODE_TOK_OFFICE:
		if (theVoice == DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_DEFAULT;
		}
		else
		{
			theVoice=DEATHVOICE_ASIAN;
		}
		break;
	case LEVCODE_TOK_PENT:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_ASIAN;
		}
		break;
	case LEVCODE_IRQ_STREETS:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_ARAB;
		}
		break;
	case LEVCODE_IRQ_FORT:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_ARAB;
		}
		break;
	case LEVCODE_IRQ_OIL:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_ARAB;
		}
		break;
	case LEVCODE_CAS_1:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_RAIDER;
		}
		break;
	case LEVCODE_CAS_2:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_RAIDER;
		}
		break;
	case LEVCODE_CAS_3:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_RAIDER;
		}
		break;
	case LEVCODE_CAS_4:
		if (theVoice != DEATHVOICE_FEMALE)
		{
			theVoice=DEATHVOICE_RAIDER;
		}
		break;
	case LEVCODE_TUTORIAL:
	case LEVCODE_ARM_1:
	case LEVCODE_ARM_2:
	case LEVCODE_ARM_3:
		theVoice=DEATHVOICE_DEFAULT;
		touchSound = false;
		break;
	}

	switch(theVoice)
	{
	case DEATHVOICE_DEFAULT:
		break;
	case DEATHVOICE_FEMALE:
		strcpy(putSuffixHere, "enemy/dth/fem");
		break;
	case DEATHVOICE_SKINHEAD:
		strcpy(putSuffixHere, "enemy/dth/skin");
		break;
	case DEATHVOICE_ARAB:
		strcpy(putSuffixHere, "enemy/dth/arab");
		break;
	case DEATHVOICE_RUSSIAN:
		strcpy(putSuffixHere, "enemy/dth/russ");
		break;
	case DEATHVOICE_ASIAN:
		strcpy(putSuffixHere, "enemy/dth/jpn");
		break;
	case DEATHVOICE_RAIDER:
		strcpy(putSuffixHere, "enemy/dth/rad");
		break;
	}
}

bodyhuman_c::bodyhuman_c()
{
	curSoundIndex = 0;
	curVol = 0;
	curAtten = 1.0;
	emotion = EMOTION_NORMAL;
	emotion_expire=level.time;
	emotionScripted=false;
	currentmove = NULL;
	holdCode = HOLDCODE_NO;
	currentMoveFinished = true;
	bodyDamage = bodyDamageAccumulated = BODYDAMAGE_NONE;
	bodyDamageEndRecent=level.time;
	rHandOpen=false;
	lHandOpen=false;
	rWeaponShot=false;
	lWeaponShot=false;
	frame_flags = 0;
	memset(face_base, 0, 64);
	memset(curface, 0, 64);
	blink_time=level.time;
	breathe_time=level.time;
	rLastFireTime=level.time-10;
	lLastFireTime=level.time-10;
	nextGreetTime=level.time;
	numGreets = 0;
	numWakes = 0;
	curGreet = 0;
	touchSound = true;
	deathVoiceType=DEATHVOICE_DEFAULT;
	FinalAnim = false;
	FinalSound = false;
	freezeMyBolts = false;
	gutsOut = false;
	justFiredRight = false;
	memset(voiceDirGeneral, 0, 64);
	memset(voiceDirDeath, 0, 64);
	voiceDirNumber=1;
	lHandAtk = ATK_NOTHING;
	rHandAtk = ATK_NOTHING;
	armorCode = ARMOR_NONE;
	nNextShotKillsMe = NEXTSHOT_NULL;
	bDropBoltons = true;
	bBoxConfirmed = false;
	bVocalCordsWorking = true;
	grenadeInHand = false;
}

void bodyhuman_c::ShakeSpecialPart(edict_t &monster, gz_blown_part *part, Vect3 &Direction)
{
	ggOinstC *myInstance=game_ghoul.FindOInst(monster.ghoulInst);
	if (part->area==GBLOWNAREA_FINISH || !myInstance)
	{
		return;
	}
	if (part->code == GBLOWNCODE_SPECIAL)
	{
		if (part->area == GBLOWNAREA_CHEST)
		{
			ShakePart(monster, gzones[GZ_HEAD_FRONT].blowme, Direction);
			ShakePart(monster, gzones[GZ_ARM_UPPER_RIGHT].blowme, Direction);
			ShakePart(monster, gzones[GZ_ARM_UPPER_LEFT].blowme, Direction);
		}
	}
}

void bodyhuman_c::BlowSpecialPart(edict_t &monster, gz_blown_part *part, edict_t *dangler)
{
	ggOinstC *myInstance=game_ghoul.FindOInst(monster.ghoulInst);
	if (part->area==GBLOWNAREA_FINISH || !myInstance)
	{
		return;
	}
	if (part->code == GBLOWNCODE_SPECIAL)
	{
		if (part->area == GBLOWNAREA_CHEST)
		{
			BlowPart(monster, gzones[GZ_HEAD_FRONT].blowme, dangler);
			BlowPart(monster, gzones[GZ_ARM_UPPER_RIGHT].blowme, dangler);
			BlowPart(monster, gzones[GZ_ARM_UPPER_LEFT].blowme, dangler);
			BlowZone(monster, GZ_ARM_UPPER_RIGHT, GZ_ARM_UPPER_RIGHT,dangler);
			BlowZone(monster, GZ_ARM_UPPER_LEFT, GZ_ARM_UPPER_LEFT,dangler);
			BlowZone(monster, GZ_HEAD_FRONT, GZ_HEAD_FRONT,dangler);
		}
		else if (part->area == GBLOWNAREA_RTHIGH)
		{
			BlowZone(monster, GZ_LEG_LOWER_RIGHT, GZ_LEG_LOWER_RIGHT,dangler);
		}
		else if (part->area == GBLOWNAREA_LTHIGH)
		{
			BlowZone(monster, GZ_LEG_LOWER_LEFT, GZ_LEG_LOWER_LEFT,dangler);
		}
	}
}

void bodyhuman_c::UseMonster(edict_t &monster, edict_t *user)
{
	if (monster.health > 0 && level.time > nextGreetTime && (OnSameTeam(&monster, user) || GetRightHandWeapon(monster)==ATK_NOTHING))
	{
/*		ai_c		*my_ai = (ai_c *)((ai_public_c *)monster.ai);
		if ((my_ai->IsInnocent()||my_ai->GetTeam(monster)==0)&&!(monster.spawnflags & SPAWNFLAG_HOLD_POSITION))
		{	// everybody likes me, so i get pushed around; is that fair?!?!
			vec3_t from_user;
			VectorSubtract(monster.s.origin, user->s.origin, from_user);
			VectorNormalize(from_user);
			T_Damage(&monster, user, user, from_user, monster.s.origin, user->s.origin, 0, 50, 0, 0, 0.0, 0.0);
		}
*/
		// kef -- adding a new command to the scriptor would be too costly right now
		if (monster.health != 9999)
		{
			if (emotion==EMOTION_PAIN || emotion==EMOTION_AFRAID)
			{
				Emote(monster, EMOTION_FEARTALK, 2.0);
			}
			else if (emotion==EMOTION_NORMAL)
			{
				Emote(monster, EMOTION_TALK, 2.0);
			}
			VoiceGreetSound(monster, 0.9);
		}
		nextGreetTime = level.time + 10.0f;
	}
}

void bodyhuman_c::TouchMonster(edict_t &monster, edict_t *user)
{
	if (touchSound && monster.health > 0 && level.time > nextGreetTime && (OnSameTeam(&monster, user) || GetRightHandWeapon(monster)==ATK_NOTHING))
	{
		VoiceGreetSound(monster, 0.9);
		nextGreetTime = level.time + 10.0f;
	}
}

void bodyhuman_c::MakeRetreatNoise(edict_t &monster)
{
	if (currentmove && currentmove->suggested_action != ACTCODE_PAIN)
	{
//		VoiceSound("retreat", monster, 0);
	}
}

void bodyhuman_c::VoiceSound(char *base, edict_t &monster, int num, float vol)
{
	char buffer[256];
	int tindex;

	char levSuffix[256];

	if (!voiceDirGeneral[0])
	{
		return;
	}

	if ((monster.ai && monster.ai->FindScriptedDecision(&monster))||game.cinematicfreeze)
	{
		return;
	}

	GetLevelSoundSuffix(levSuffix);

	if (!monster.ghoulInst || !gzones)
	{
		return;
	}

	//no vocal cords, no scream
	if (!AreVocalCordsWorking())
	{
		return;
	}

	// no screaming after a certain point
	if (FinalSound)
	{
		return;
	}

	if (!gmonster.IsReadyForVoiceSound())
	{
		return;
	}

	voiceCode_e newVoice=VOICECODE_UNKNOWN;

	if (!stricmp(base, "backup"))
	{
		newVoice=VOICECODE_BACKUP;
	}
	else if (!stricmp(base, "throw"))
	{
		newVoice=VOICECODE_THROW;
	}
	else if (!stricmp(base, "duck"))
	{
		newVoice=VOICECODE_DUCK;
	}
	else if (!stricmp(base, "retreat"))
	{
		newVoice=VOICECODE_RETREAT;
	}
	else if (!stricmp(base, "help"))
	{
		newVoice=VOICECODE_HELP;
	}
	else if (!stricmp(base, "react"))
	{
		newVoice=VOICECODE_REACT;
	}

	if (newVoice==gmonster.GetVoiceCode())
	{
		return;
	}

	gmonster.SetVoiceSoundTime(level.time);
	gmonster.SetVoiceCode(newVoice);

	if (num==0)
	{
		Com_sprintf(buffer, 256, "%s%d/%s/%s.wav", voiceDirGeneral, voiceDirNumber, levSuffix, base);
	}
	else
	{
		Com_sprintf(buffer, 256, "%s%d/%s/%s%d.wav", voiceDirGeneral, voiceDirNumber, levSuffix, base, gi.irand(1,num));
	}

	AI_GORETEST_PRINTF("voice sound: %s\n",buffer);
	
	if (DoesVoiceSoundExist(buffer))
	{
		if (tindex=gi.soundindex (buffer))
		{
			curSoundIndex=tindex;
			curVol=vol;
			curAtten = 1.6;
		}
	}
//	gi.sound (&monster, CHAN_VOICE, curSoundIndex, vol, 1.6, 0);
}
static int lastWake;
void bodyhuman_c::VoiceWakeSound(edict_t &monster, float vol)
{
	char buffer[256];
	int tindex;

	char levSuffix[256];

	if (!voiceDirGeneral[0])
	{
		return;
	}

	if ((monster.ai && monster.ai->FindScriptedDecision(&monster))||game.cinematicfreeze)
	{
		return;
	}

	GetLevelSoundSuffix(levSuffix);

	//no vocal cords, no scream
	if (!AreVocalCordsWorking())
	{
		return;
	}
	if (monster.health <= 0 || FinalSound || numWakes <= 0)
	{
		return;
	}

	if (!gmonster.IsReadyForWakeSound())
	{
		return;
	}
	gmonster.SetWakeSoundTime(level.time);

	int curWake;

	//do unrandomizing trickery here
	if (lastWake > 0 && lastWake <= numWakes && numWakes >= 2)
	{
		curWake = gi.irand(1,numWakes-1);
		if (curWake >= lastWake)
		{
			curWake+=1;
		}
	}
	else
	{
		curWake = gi.irand(1,numWakes);
	}
	lastWake = curWake;

	Com_sprintf(buffer, 256, "%s%d/%s/Wake%d.wav", voiceDirGeneral, voiceDirNumber, levSuffix, curWake);

	AI_GORETEST_PRINTF("wake sound: %s\n",buffer);
	
	if (tindex=gi.soundindex (buffer))
	{
		curSoundIndex=tindex;
		curVol=vol;
		curAtten = 1.6;
	}
}
static int lastGreet;
void bodyhuman_c::VoiceGreetSound(edict_t &monster, float vol)
{
	char buffer[256];
	int tindex;

	char levSuffix[256];

	if (!voiceDirGeneral[0])
	{
		return;
	}

	if ((monster.ai && monster.ai->FindScriptedDecision(&monster))||game.cinematicfreeze)
	{
		return;
	}

	GetLevelSoundSuffix(levSuffix);

	//no vocal cords, no scream
	if (!AreVocalCordsWorking())
	{
		return;
	}
	if (monster.health <= 0 || FinalSound || numGreets <= 0)
	{
		return;
	}

	if (curGreet == 0)
	{
		//do unrandomizing trickery here
		if (lastGreet > 0 && lastGreet <= numGreets && numGreets >= 2)
		{
			curGreet = gi.irand(1,numGreets-1);
			if (curGreet >= lastGreet)
			{
				curGreet+=1;
			}
		}
		else
		{
			curGreet = gi.irand(1,numGreets);
		}
	}
	lastGreet = curGreet;

	Com_sprintf(buffer, 256, "%s%d/%s/Greet%d.adp", voiceDirGeneral, voiceDirNumber, levSuffix, curGreet);

	AI_GORETEST_PRINTF("greet sound: %s\n",buffer);
	
	if (tindex=gi.soundindex (buffer))
	{
		curSoundIndex=tindex;
		curVol=vol;
		curAtten = 1.6;
	}
}

void bodyhuman_c::VoiceGeneralSound(char *base, edict_t &monster, int num, float vol)
{
	char buffer[256];
	int tindex;

	char levSuffix[256];

//	GetLevelSoundSuffix(levSuffix);
	GetLevelDeathSoundDir(levSuffix, deathVoiceType);

	assert(gzones);//NPM: I got a crash here
	//no vocal cords, no scream
	if (!AreVocalCordsWorking()&&stricmp(base, "choke"))
	{
		return;
	}

	// no screaming after a certain point
	if (FinalSound || levSuffix[0]==0)
	{
		return;
	}

	Com_sprintf(buffer, 256, "%s/%s%d.wav", levSuffix, base, gi.irand(1,num));

	AI_GORETEST_PRINTF("general voice sound: %s\n",buffer);
	
	if (tindex=gi.soundindex (buffer))
	{
		// increase the player's loudness
//		Com_Printf("%s %s making noise\n", monster.classname, monster.targetname?monster.targetname:"someguy");
		IncreaseSpawnIntensity(.05);

		curSoundIndex=tindex;
		curVol=vol;
		curAtten = 2.0;
	}
//	gi.sound (&monster, CHAN_VOICE, curSoundIndex, vol, 1.6, 0);
}

void bodyhuman_c::VoiceSoundCache(char *base, edict_t &monster, int num)
{
	char buffer[256];
	int i;

	char levSuffix[256];

	if (!voiceDirGeneral[0])
	{
		return;
	}

	GetLevelSoundSuffix(levSuffix);

	if(ai_loadAllBolts)
	{
		for(int j = 1; j <= voiceDirNumber; j++)
		{
			if (num==0)
			{
				Com_sprintf(buffer, 256, "%s%d/%s/%s.wav", voiceDirGeneral, j, levSuffix, base);

				AI_GORETEST_PRINTF("voice cache: %s\n",buffer);
				
				if (DoesVoiceSoundExist(buffer))
				{
					gi.soundindex(buffer);
				}
			}
			else
			{
				for (i=1;i<=num;i++)
				{
					Com_sprintf(buffer, 256, "%s%d/%s/%s%d.wav", voiceDirGeneral, j, levSuffix, base, i);

					AI_GORETEST_PRINTF("voice cache: %s\n",buffer);
					
					if (DoesVoiceSoundExist(buffer))
					{
						gi.soundindex(buffer);
					}
				}
			}
		}
	}
	else
	{
		if (num==0)
		{
				Com_sprintf(buffer, 256, "%s%d/%s/%s.wav", voiceDirGeneral, voiceDirNumber, levSuffix, base);

				AI_GORETEST_PRINTF("voice cache: %s\n",buffer);
				
				if (DoesVoiceSoundExist(buffer))
				{
					gi.soundindex(buffer);
				}
		}
		else
		{
			for (i=1;i<=num;i++)
			{
				Com_sprintf(buffer, 256, "%s%d/%s/%s%d.wav", voiceDirGeneral, voiceDirNumber, levSuffix, base, i);

				AI_GORETEST_PRINTF("voice cache: %s\n",buffer);
				
				if (DoesVoiceSoundExist(buffer))
				{
					gi.soundindex(buffer);
				}
			}
		}
	}
}

void bodyhuman_c::VoiceWakeSoundCache(edict_t &monster)
{
	char buffer[256];
	int i;

	char levSuffix[256];

	if (!voiceDirGeneral[0])
	{
		return;
	}

	GetLevelSoundSuffix(levSuffix);

	if (numWakes <= 0)
	{
		qboolean lastWake=false;
		i=0;
		while (!lastWake)
		{
			i++;
			Com_sprintf(buffer, 256, "sound/%s%d/%s/Wake%d.wav", voiceDirGeneral, voiceDirNumber, levSuffix,  i);

			//sound has level-specific override...
			if (!DoesVoiceSoundExist(buffer))
			{
				lastWake = true;
			}
		}

		//no sounds!
		if (i<=1)
		{
			return;
		}

		numWakes=i-1;
	}

	if(ai_loadAllBolts)
	{
		for(int j = 1; j <= voiceDirNumber; j++)
		{
			for (i=1;i<=numWakes;i++)
			{
				Com_sprintf(buffer, 256, "%s%d/%s/Wake%d.wav", voiceDirGeneral, j, levSuffix, i);

				AI_GORETEST_PRINTF("voice cache: %s\n",buffer);
				
				gi.soundindex(buffer);
			}
		}
	}
	else
	{
		for (i=1;i<=numWakes;i++)
		{
			Com_sprintf(buffer, 256, "%s%d/%s/Wake%d.wav", voiceDirGeneral, voiceDirNumber, levSuffix, i);

			AI_GORETEST_PRINTF("voice cache: %s\n",buffer);
			
			gi.soundindex(buffer);
		}
	}
}

void bodyhuman_c::VoiceGreetSoundCache(edict_t &monster)
{
	char buffer[256];
	int i;

	char levSuffix[256];

	if (!voiceDirGeneral[0])
	{
		return;
	}

	GetLevelSoundSuffix(levSuffix);

	if (numGreets <= 0)
	{
		qboolean lastWake=false;
		i=0;
		while (!lastWake)
		{
			i++;
			Com_sprintf(buffer, 256, "sound/%s%d/%s/Greet%d.adp", voiceDirGeneral, voiceDirNumber, levSuffix,  i);

			//sound has level-specific override...
			if (!DoesVoiceSoundExist(buffer))
			{
				lastWake = true;
			}
		}

		//no sounds!
		if (i<=1)
		{
			return;
		}

		numGreets=i-1;
	}

	if(ai_loadAllBolts)
	{
		for(int j = 1; j <= voiceDirNumber; j++)
		{
			for (i=1;i<=numGreets;i++)
			{
				Com_sprintf(buffer, 256, "%s%d/%s/Greet%d.adp", voiceDirGeneral, j, levSuffix, i);

				AI_GORETEST_PRINTF("voice cache: %s\n",buffer);
				
				gi.soundindex(buffer);
			}
		}
	}
	else
	{
		for (i=1;i<=numGreets;i++)
		{
			Com_sprintf(buffer, 256, "%s%d/%s/Greet%d.adp", voiceDirGeneral, voiceDirNumber, levSuffix, i);

			AI_GORETEST_PRINTF("voice cache: %s\n",buffer);
			
			gi.soundindex(buffer);
		}
	}
}

void bodyhuman_c::VoiceGeneralSoundCache(char *base, edict_t &monster, int num)
{
	char buffer[256];
	int i;

	char levSuffix[256];

//	GetLevelSoundSuffix(levSuffix);
	GetLevelDeathSoundDir(levSuffix, deathVoiceType);

	if (levSuffix[0]==0)
	{
		return;
	}

	for (i=1;i<=num;i++)
	{
		Com_sprintf(buffer, 256, "%s/%s%d.wav", levSuffix, base, i);

		//sound has level-specific override...
//		if (DoesVoiceSoundExist(buffer))
//		{
			gi.soundindex(buffer);
//		}
		//no level-specific override, use default sounds
//		else
//		{
//			Com_sprintf(buffer, 256, "%sGen/%s%d.wav", voiceDirGeneral, base, i);
//			gi.soundindex(buffer);
//		}

		AI_GORETEST_PRINTF("general voice cache: %s\n",buffer);
	}

}

void bodyhuman_c::SetVoiceDirectories(edict_t &monster, const char *base, int possible_varieties, deathvoice_code death, char *deathdir)
{
	assert(possible_varieties >= 1);

	deathVoiceType = death;
	if (death==DEATHVOICE_PLAYER)
	{
		if (deathdir)
		{
			strcpy(voiceDirDeath, deathdir);
		}
		else
		{
			voiceDirDeath[0]=0;
		}
	}
	else
	{
		voiceDirDeath[0]=0;
	}

	if (!base || !base[0])
	{
		voiceDirGeneral[0]=0;
	}
	else
	{
		sprintf(voiceDirGeneral, "Enemy/%s", base);
	}
	
	voiceDirNumber=gi.irand(1,possible_varieties);

	if(ai_loadAllBolts)
	{
		voiceDirNumber = possible_varieties;
	}
}

void bodyhuman_c::SetRightHandWeapon(edict_t &monster, attacks_e newRightWeap)
{
	ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);
	ggBinstC	*boltedptr;
	bool		noAttack=true;
	
	if (!myInstance)
	{
		return;
	}

	boltedptr=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r"));
	if (boltedptr)
	{
		myInstance->RemoveBoltInstance(boltedptr);
	}

	switch(newRightWeap)
	{
	case ATK_PISTOL2:
		AddWeaponTypeForPrecache(SFW_PISTOL2);
		noAttack=!AddBoltedItem(monster, "wbolt_hand_r", "Enemy/Bolt", "w_pistol2", "to_wbolt_hand_r", myInstance);
		break;
	case ATK_PISTOL1:
		AddWeaponTypeForPrecache(SFW_PISTOL1);
		noAttack=!AddBoltedItem(monster, "wbolt_hand_r", "Enemy/Bolt", "w_pistol1", "to_wbolt_hand_r", myInstance);
		break;
	case ATK_SNIPER:
	case ATK_SNIPER_ALT:
		AddWeaponTypeForPrecache(SFW_SNIPER);
		noAttack=!AddBoltedItem(monster, "wbolt_hand_r", "Enemy/Bolt", "w_sniperrifle", "to_wbolt_hand_r", myInstance);
		break;
	case ATK_MACHINEGUN:
	case ATK_MACHINEGUN_ALT:
		AddWeaponTypeForPrecache(SFW_MACHINEGUN);
		noAttack=!AddBoltedItem(monster, "wbolt_hand_r", "Enemy/Bolt", "w_machinegun", "to_wbolt_hand_r", myInstance);
		break;
	case ATK_KNIFE:
	case ATK_KNIFE_ALT:
		AddWeaponTypeForPrecache(SFW_KNIFE);
		noAttack=!AddBoltedItem(monster, "wbolt_hand_r", "Enemy/Bolt", "w_knife", "to_wbolt_hand_r", myInstance);
		break;
	case ATK_FLAMEGUN:
	case ATK_FLAMEGUN_ALT:
		AddWeaponTypeForPrecache(SFW_FLAMEGUN);
		noAttack=!AddBoltedItem(monster, "wbolt_hand_r", "Enemy/Bolt", "w_flamethrower", "to_wbolt_hand_r", myInstance);
		break;
	case ATK_ASSAULTRIFLE:
		AddWeaponTypeForPrecache(SFW_ASSAULTRIFLE);
		noAttack=!AddBoltedItem(monster, "wbolt_hand_r", "Enemy/Bolt", "w_assault_rifle", "to_wbolt_hand_r", myInstance);
		break;
	case ATK_MACHINEPISTOL:
		// 1/11/00 kef -- don't put Hawk's machinepistol in the pak file
		if(strcmp(monster.classname, "m_x_mmerc"))
		{
			AddWeaponTypeForPrecache(SFW_MACHINEPISTOL);
		}
		noAttack=!AddBoltedItem(monster, "wbolt_hand_r", "Enemy/Bolt", "w_machinepistol", "to_wbolt_hand_r", myInstance);
		break;
	case ATK_AUTOSHOTGUN:
	case ATK_AUTOSHOTGUN_ALT:
		AddWeaponTypeForPrecache(SFW_AUTOSHOTGUN);
		noAttack=!AddBoltedItem(monster, "wbolt_hand_r", "Enemy/Bolt", "w_autoshotgun", "_to_wbolt_hand_r", myInstance);
		break;
	case ATK_SHOTGUN:
		AddWeaponTypeForPrecache(SFW_SHOTGUN);
		noAttack=!AddBoltedItem(monster, "wbolt_hand_r", "Enemy/Bolt", "w_shotgun", "to_wbolt_hand_r", myInstance);
		break;
	case ATK_ROCKET:
		AddWeaponTypeForPrecache(SFW_ROCKET);
		noAttack=!AddBoltedItem(monster, "wbolt_hand_r", "Enemy/Bolt", "w_rocket", "to_wbolt_hand_r", myInstance);
		break;
	case ATK_MICROWAVE:
	case ATK_MICROWAVE_ALT:
	case ATK_DEKKER:
		AddWeaponTypeForPrecache(SFW_MICROWAVEPULSE);
		noAttack=!AddBoltedItem(monster, "wbolt_hand_r", "Enemy/Bolt", "w_mpg", "_to_wbolt_hand_r", myInstance);
		break;
	default:
	case ATK_NOTHING:
		break;
	}

	//only set rHandAtk if i was successful bolting the thing on.
	if (noAttack)
	{
		rHandAtk = ATK_NOTHING;//unset whatever was there before, because we just removed it
	}
	else
	{
		rHandAtk = newRightWeap;
	}
}

void bodyhuman_c::SetLeftHandWeapon(edict_t &monster, attacks_e newLeftWeap)
{
	lHandAtk = newLeftWeap;
	ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);
	ggBinstC	*boltedptr;
	
	if (!myInstance)
	{
		return;
	}

	boltedptr=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"));
	if (boltedptr)
	{
		myInstance->RemoveBoltInstance(boltedptr);
	}

	if (lHandAtk==ATK_MACHETE)
	{
		if (!AddBoltedItem(monster, "wbolt_hand_l", "Enemy/Bolt", "w_machete", "to_wbolt_hand_l", myInstance))
		{
			lHandAtk=ATK_NOTHING;
		}
	}
	//thrown item--make sure i've got the anims avail...
	else
	{
		if(!IsAvailableSequence(monster, &generic_move_throw_gren)
			&& !IsAvailableSequence(monster, &generic_move_lob_grenade_crouch)
			&& !IsAvailableSequence(monster, &generic_move_throwstar))
		{
//			if (monster.targetname)
//			{
//				gi.dprintf("Attention! Canceling thrown weapon for %s--no anim support!\n",monster.targetname);
//			}
//			else if (monster.classname)
//			{
//				gi.dprintf("Attention! Canceling thrown weapon for %s--no anim support!\n",monster.classname);
//			}
//			else
//			{
//				gi.dprintf("Attention! Canceling thrown weapon for unknown--no anim support!\n");
//			}
			lHandAtk=ATK_NOTHING;
		}
	}
}

int bodyhuman_c::UseLeftHandAttack(edict_t &monster, edict_t *target)
{
	//ahem, machetes go in the left hand...
	attacks_e	myLeftHand=GetLeftHandWeapon(monster);
	if(myLeftHand == ATK_NOTHING)return 0;
	if(!(monster.spawnflags & SPAWNFLAG_HAS_PROJECTILE)&&(myLeftHand != ATK_THROWSTAR))return 0;
	if(!target)return 0;
	if(gi.flrand(0,1) > .1 /*&&(myLeftHand != ATK_THROWSTAR)*//*for testing*/)return 0;

	CRadiusContent rad(target->s.origin, 200, 1, 1);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		edict_t *search = rad.foundEdict(i);

		if (myLeftHand == ATK_THROWSTAR && search == &monster)
			continue;

		if(OnSameTeam(&monster, search))
		{
			return 0;
		}
	}

	//fixme - what about the ceiling?
	// can't fix everything, but let's at least not ram the wall
	vec3_t checkDir, checkSpot;
	trace_t tr;

	AngleVectors(monster.s.angles, checkDir, 0, 0);

	VectorMA(monster.s.origin, 96, checkDir, checkSpot);
	gi.trace(monster.s.origin, 0, 0, checkSpot, &monster, MASK_PLAYERSOLID, &tr);
	if(tr.fraction < 1.0)
	{	//?
		return 0;
	}

	return 1;
}


mmove_t *bodyhuman_c::GetSequenceForDisability(edict_t &monster, int reject_actionflags)
{
	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);
	float blindLevel=0;
	mmove_t	*auxmove;

	if (the_ai)
	{
		blindLevel=the_ai->GetSenseMutedLevel(sight_mask);
	}

	//okay, i'm blind. now what?
//	if (currentmove && currentmove->bbox == BBOX_PRESET_STAND)
//	{
		if (blindLevel >= smute_degree_severe)
		{
			//ooh, not showing the blinded yet. so let's go!
			if (currentmove && currentmove->bodyPosition != BODYPOS_BLINDED)
			{
				if(VerifySequence(monster, &generic_move_blindedto, reject_actionflags))
				{
					return &generic_move_blindedto;
				}
			}
			if(VerifySequence(monster, &generic_move_blindedloop, reject_actionflags))
			{
				return &generic_move_blindedloop;
			}
		}
		//ooh, still showing the blinded. stop already!
		if (currentmove && currentmove->bodyPosition == BODYPOS_BLINDED)
		{
			if(VerifySequence(monster, &generic_move_blindedfrom, reject_actionflags))
			{
				return &generic_move_blindedfrom;
			}
		}
//	}

	if (Skill_NoPain(&monster))
	{
		bodyDamage&=~BODYDAMAGE_RECENT;
	}
	//owee, i'm hurt
	else if (bodyDamage & BODYDAMAGE_RECENT && currentmove && currentmove->bbox==BBOX_PRESET_STAND)
	{
		//randomly get tired of this shtick(sp?)--fixme: should store time, or something more reliable
//		if ((gi.flrand(0.0,10.0)*Skill_DisablityModifier(&monster)<1.0))
		if (level.time > bodyDamageEndRecent)
		{
			bodyDamage&=~BODYDAMAGE_RECENT;
		}
		//hit in leg
		if (bodyDamage & (BODYDAMAGE_RIGHTLEG))
		{
			if (IsAvailableSequence(monster, &generic_move_hop_lleg))
			{
				if (currentmove!=&generic_move_hop_lleg/*||currentMoveFinished*/)
				{
					VoiceGeneralSound("Writhe", monster, WRITHENUMBER);
				}
				return &generic_move_hop_lleg;
			}
		}
		if (bodyDamage & (BODYDAMAGE_LEFTLEG))
		{
			if (IsAvailableSequence(monster, &generic_move_hop_rleg))
			{
				if (currentmove!=&generic_move_hop_rleg/*||currentMoveFinished*/)
				{
					VoiceGeneralSound("Writhe", monster, WRITHENUMBER);
				}
				return &generic_move_hop_rleg;
			}
		}
		if (bodyDamage & (BODYDAMAGE_RIGHTARM))
		{
			if (IsAvailableSequence(monster, &generic_move_hurt_rarm))
			{
				if (currentmove!=&generic_move_hurt_rarm/*||currentMoveFinished*/)
				{
					VoiceGeneralSound("Writhe", monster, WRITHENUMBER);
				}
				return &generic_move_hurt_rarm;
			}
		}
		if (bodyDamage & (BODYDAMAGE_LEFTARM))
		{
			if (IsAvailableSequence(monster, &generic_move_hurt_larm))
			{
				if (currentmove!=&generic_move_hurt_larm/*||currentMoveFinished*/)
				{
					VoiceGeneralSound("Writhe", monster, WRITHENUMBER);
				}
				return &generic_move_hurt_larm;
			}
		}
	}


	//okay, i want to be able to be startled anywhere, so i'll call spasticness a disability
	if (currentmove && currentmove->bbox == BBOX_PRESET_STAND && currentmove->bodyPosition == BODYPOS_IDLE
		&& the_ai && the_ai->HasTarget())
	{
		//only be startled sometimes?
		if (the_ai->GetStartleability()/*&& gi.irand(0,3)*/)
		{
			the_ai->SetStartleability(false);

			//only play alert sound if perception was immediate, and enemy is client...(fixme?)
			sensedEntInfo_t sensed;
			the_ai->GetSensedClientInfo(0, sensed);
			if (!sensed.ent || sensed.time > level.time - 7.0)
			{
				if (currentmove && currentmove->suggested_action != ACTCODE_PAIN)
				{
					VoiceWakeSound(monster, 0.9);
				}
			}

			if(VerifySequence(monster, &generic_move_startled, reject_actionflags))
			{


				return &generic_move_startled;
			}
		}
	}


	//danger! if i need to reload, won't play ideal_move!
	if (GetBestWeapon(monster)!=ATK_NOTHING)
	{
		if (MustReloadRWeapon(monster))
		{
			justFiredRight = false;
			if (auxmove=GetSequenceForReload(monster/*, dest, face, seqtype, goal_bbox, ideal_move, reject_actionflags*/))
			{
				return auxmove;
			}
		}
		else if (justFiredRight)
		{
			justFiredRight = false;
			if (auxmove=GetPostShootSequence(monster))
			{
				return auxmove;
			}
		}
	}

	//just came from startled--consider reloading
	//fixme: force reloading here, or earlier (this is hacky)?
	if (currentmove && currentmove==&generic_move_startled && IsAnimationFinished() 
		&& GetBestWeapon(monster)!=ATK_NOTHING)
	{
		if (gi.irand(0,1))
		{
			if (auxmove=GetSequenceForReload(monster))
			{
				return auxmove;
			}
		}
	}

	//eh, nothing going on here. what's on tv?
	return NULL;
}

mmove_t	*bodyhuman_c::GetSequenceForTransition(edict_t &monster, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, int reject_actionflags)
{
	if (!currentmove||goal_bbox==BBOX_PRESET_NUMBER||goal_bbox==currentmove->bbox)
	{
		return NULL;
	}
	gunvariety_t	rightHandWeapVariety=GetGunVariety(GetRightHandWeapon(monster));
	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);
	bool	isAggressive=monster.client||(the_ai&&the_ai->getTarget()&&!OnSameTeam(the_ai->getTarget(),&monster)&&rightHandWeapVariety!=GUNVARIETY_NONE&&emotion!=EMOTION_AFRAID);

	switch(currentmove->bbox)
	{
	case BBOX_PRESET_CROUCH:
		if(goal_bbox == BBOX_PRESET_STAND)
		{
			if (GetBestWeapon(monster)==ATK_NOTHING && VerifySequence(monster, &generic_move_crouch_cower_up, reject_actionflags))
			{
				return &generic_move_crouch_cower_up;
			}
			if((rightHandWeapVariety&GUNVARIETY_MRS)&&VerifySequence(monster, &generic_move_duck_up_mrs, reject_actionflags))
			{
				return &generic_move_duck_up_mrs;
			}
			if((rightHandWeapVariety&GUNVARIETY_LAUNCHER)&&VerifySequence(monster, &generic_move_duck_up_l, reject_actionflags))
			{
				return &generic_move_duck_up_l;
			}
			if(VerifySequence(monster, &generic_move_duck_up, reject_actionflags))
			{
				return &generic_move_duck_up;
			}
		}
		break;
	case BBOX_PRESET_STAND:
		if(goal_bbox == BBOX_PRESET_CROUCH)
		{
			if(isAggressive && (rightHandWeapVariety & GUNVARIETY_PISTOL) && VerifySequence(monster, &generic_move_crouchdwnpistol1h, reject_actionflags))
			{
				return &generic_move_crouchdwnpistol1h;
			}

			if (GetBestWeapon(monster)==ATK_NOTHING && VerifySequence(monster, &generic_move_crouch_cower_dwn, reject_actionflags))
			{
				return &generic_move_crouch_cower_dwn;
			}

			if (IsAvailableSequence(monster, &generic_move_roll_to_crouch) && !(monster.spawnflags&SPAWNFLAG_HOLD_POSITION) && !(monster.flags&FL_NO_KNOCKBACK))
			{
				vec3_t to_dest, forward;
				float to_dest_dist;
				VectorSubtract(dest, monster.s.origin, to_dest);
				AngleVectors(monster.s.angles, forward, NULL, NULL);
				to_dest_dist=VectorNormalize(to_dest);
				if (to_dest_dist>2 && DotProduct(to_dest, forward)>0.5 && VerifySequence(monster, &generic_move_roll_to_crouch, reject_actionflags))
				{
					return &generic_move_roll_to_crouch;
				}
			}

			if((rightHandWeapVariety&GUNVARIETY_MRS)&&VerifySequence(monster, &generic_move_duck_down_mrs, reject_actionflags))
			{
				return &generic_move_duck_down_mrs;
			}
			if((rightHandWeapVariety&GUNVARIETY_LAUNCHER)&&VerifySequence(monster, &generic_move_duck_down_l, reject_actionflags))
			{
				return &generic_move_duck_down_l;
			}
			if(VerifySequence(monster, &generic_move_duck_down, reject_actionflags))
			{
				return &generic_move_duck_down;
			}
		}
		else if (goal_bbox == BBOX_PRESET_PRONE)
		{
			if(VerifySequence(monster, &generic_move_go_prone, reject_actionflags))
			{
				return &generic_move_go_prone;
			}
		}
		break;
	case BBOX_PRESET_LAYSIDE:
		if (monster.spawnflags & SPAWNFLAG_HOLD_POSITION)
		{
			return NULL;
		}		
		if(VerifySequence(monster, &generic_move_prone_to_stand, reject_actionflags))
		{
			monster.s.angles[YAW]-=90.0f;
			monster.s.angles[YAW]=anglemod(monster.s.angles[YAW]);
			return &generic_move_prone_to_stand;
		}
		break;
	case BBOX_PRESET_PRONE:
		if (monster.spawnflags & SPAWNFLAG_HOLD_POSITION)
		{
			return NULL;
		}		
		if(VerifySequence(monster, &generic_move_prone_to_stand, reject_actionflags))
		{
			return &generic_move_prone_to_stand;
		}
		break;
	case BBOX_PRESET_LAYBACK:
		if (monster.spawnflags & SPAWNFLAG_HOLD_POSITION)
		{
			return NULL;
		}		
		if(VerifySequence(monster, &generic_move_upfromback, reject_actionflags))
		{
			VoiceGeneralSound("Getup", monster, 1);
			return &generic_move_upfromback;
		}
		break;
	}
	//nothing appropriate found. oh well.
	return NULL;
}

mmove_t	*bodyhuman_c::GetSequenceForRunAimStrafe(edict_t &monster, vec3_t to_dest_dir, vec3_t forward, vec3_t right, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	gunvariety_t	rightHandWeapVariety=GetGunVariety(GetRightHandWeapon(monster));

	vec3_t velocityDir;

	if (monster.ai)
	{
		VectorCopy(monster.ai->velocity, velocityDir);
		VectorNormalize(velocityDir);
		if (DotProduct(velocityDir, to_dest_dir)<0.25)
		{
			return NULL;
		}
	}

	if (fabs(DotProduct(to_dest_dir, forward))<0.8)
	{
		//go right...
		if (DotProduct(to_dest_dir, right)<0)
		{
			if (rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_raimstrafr_mrs, reject_actionflags))
			{
				return &generic_move_raimstrafr_mrs;
			}
			if (rightHandWeapVariety&GUNVARIETY_LAUNCHER && VerifySequence(monster, &generic_move_raimstrafr_l, reject_actionflags))
			{
				return &generic_move_raimstrafr_l;
			}
			if (VerifySequence(monster, &generic_move_raimstrafr_p, reject_actionflags))
			{
				return &generic_move_raimstrafr_p;
			}
		}
		//go left...
		else
		{
			if (rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_raimstrafl_mrs, reject_actionflags))
			{
				return &generic_move_raimstrafl_mrs;
			}
			if (rightHandWeapVariety&GUNVARIETY_LAUNCHER && VerifySequence(monster, &generic_move_raimstrafl_l, reject_actionflags))
			{
				return &generic_move_raimstrafl_l;
			}
			if (VerifySequence(monster, &generic_move_raimstrafl_p, reject_actionflags))
			{
				return &generic_move_raimstrafl_p;
			}
		}
	}
	return NULL;
}

mmove_t	*bodyhuman_c::GetSequenceForWalkAimStrafe(edict_t &monster, vec3_t to_dest_dir, vec3_t forward, vec3_t right, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	gunvariety_t	rightHandWeapVariety=GetGunVariety(GetRightHandWeapon(monster));

	if (fabs(DotProduct(to_dest_dir, forward))<0.8)
	{
		//go right...
		if (DotProduct(to_dest_dir, right)<0)
		{

			if (rightHandWeapVariety&GUNVARIETY_MS && VerifySequence(monster, &generic_move_waimstrafr_ms, reject_actionflags))
			{
				return &generic_move_waimstrafr_ms;
			}
			if (rightHandWeapVariety&GUNVARIETY_RIFLE && VerifySequence(monster, &generic_move_waimstrafr_r, reject_actionflags))
			{
				return &generic_move_waimstrafr_r;
			}
			if (rightHandWeapVariety&GUNVARIETY_LAUNCHER && VerifySequence(monster, &generic_move_waimstrafr_l, reject_actionflags))
			{
				return &generic_move_waimstrafr_l;
			}
			if (VerifySequence(monster, &generic_move_waimstrafr_p, reject_actionflags))
			{
				return &generic_move_waimstrafr_p;
			}

		}
		//go left...
		else
		{
			if (rightHandWeapVariety&GUNVARIETY_MS && VerifySequence(monster, &generic_move_waimstrafl_ms, reject_actionflags))
			{
				return &generic_move_waimstrafl_ms;
			}
			if (rightHandWeapVariety&GUNVARIETY_RIFLE && VerifySequence(monster, &generic_move_waimstrafl_r, reject_actionflags))
			{
				return &generic_move_waimstrafl_r;
			}
			if (rightHandWeapVariety&GUNVARIETY_LAUNCHER && VerifySequence(monster, &generic_move_waimstrafl_l, reject_actionflags))
			{
				return &generic_move_waimstrafl_l;
			}
			if (VerifySequence(monster, &generic_move_waimstrafl_p, reject_actionflags))
			{
				return &generic_move_waimstrafl_p;
			}
		}

	}
	return NULL;
}

bool bodyhuman_c::IsInStationaryDeathAnim()
{
	if ( (currentmove == &generic_move_death_stumble_2frnt ) ||
		 (currentmove == &generic_move_death_violent ) ||
		 (currentmove == &generic_move_deaththrown ) ||
		 (currentmove == &generic_move_death_tumble_toback ) ||
		 (currentmove == &generic_move_death_sinback_tofront ) ||
		 (currentmove == &generic_move_death_shldrright ) ||
		 (currentmove == &generic_move_death_shldrleft ) )
	{
		return false;
	}
	return true;
}

mmove_t	*bodyhuman_c::GetSequenceForMovement(edict_t &monster, vec3_t dest, vec3_t face, vec3_t org, vec3_t ang, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);
	vec3_t	to_dest,to_dest_dir,to_face,to_face_dir;
	float	angle_dist_to_dest,angle_dist_to_face;
	vec3_t	forward,right;
	float	tdot;

	gunvariety_t	rightHandWeapVariety=GetGunVariety(GetRightHandWeapon(monster));

	//to restrict when guys use their aim anims...
	bool	isAggressive=monster.client||(the_ai&&the_ai->getTarget()&&!OnSameTeam(the_ai->getTarget(),&monster)&&rightHandWeapVariety!=GUNVARIETY_NONE&&emotion!=EMOTION_AFRAID);

	if (ideal_move && VerifySequence(monster, ideal_move, reject_actionflags))
	{
		return ideal_move;
	}

	if (monster.spawnflags & SPAWNFLAG_HOLD_POSITION)
	{
		return GetSequenceForStand(monster, vec3_origin, vec3_origin, seqtype, goal_bbox, ideal_move, reject_actionflags);
	}

	//if i'm disabled, i'm disabled. what can you do about something like that?
	mmove_t	*auxmove;
	if (auxmove=GetSequenceForDisability(monster))
	{
		return auxmove;
	}

	if (auxmove=GetSequenceForTransition(monster, dest, face, seqtype, goal_bbox))
	{
		return auxmove;
	}

	if (monster.ai)
	{
		//hit in leg
		if (bodyDamageAccumulated & (BODYDAMAGE_RIGHTLEG))
		{
			if (IsAvailableSequence(monster, &generic_move_limp_lleg))
			{
				return &generic_move_limp_lleg;
			}
		}
		if (bodyDamageAccumulated & (BODYDAMAGE_LEFTLEG))
		{
			if (IsAvailableSequence(monster, &generic_move_limp_rleg))
			{
				return &generic_move_limp_rleg;
			}
		}
	}

	if (currentmove&&(currentmove->bbox==BBOX_PRESET_PRONE ||currentmove->bbox==BBOX_PRESET_ALLFOURS))
	{
		if(VerifySequence(monster, &generic_move_praim_mrs, reject_actionflags))
		{
			return &generic_move_praim_mrs;
		}
	}

	if(currentmove&&currentmove->bbox == BBOX_PRESET_LAYSIDE)
	{
		return currentmove;
	}

	//get the direction i was facing, so i can see how out of my way dest is
	AngleVectors(ang, forward, right, NULL);

	VectorSubtract(dest, org, to_dest);
	VectorCopy(to_dest, to_dest_dir);
	to_dest_dir[2]=0;
	VectorNormalize(to_dest_dir);
	angle_dist_to_dest = DotProduct (forward, to_dest_dir) + 1;

	VectorSubtract(face, org, to_face);
	VectorCopy(to_face, to_face_dir);
	VectorNormalize(to_face_dir);
	angle_dist_to_face = DotProduct (forward, to_face_dir) + 1;

	if (currentmove&&currentmove->bbox==BBOX_PRESET_CROUCH)
	{
		//yikes! i'm trapped down here!

		//roll right
		if (DotProduct(to_dest_dir, right)>0.75)
		{
			if(VerifySequence(monster, &generic_move_rollrt_p, reject_actionflags))
			{
				return &generic_move_rollrt_p;
			}
		}
		//roll left
		else if (DotProduct(to_dest_dir, right)<-0.75)
		{
			if(VerifySequence(monster, &generic_move_rolllt_p, reject_actionflags))
			{
				return &generic_move_rolllt_p;
			}

		}
		//going forward...
		else if (DotProduct(to_dest_dir, forward)>0.0)
		{
			if (isAggressive)
			{
				if ((rightHandWeapVariety&GUNVARIETY_MRS) && VerifySequence(monster, &generic_move_crouch_run_mrs, reject_actionflags))
				{
					return &generic_move_crouch_run_mrs;
				}
				if ((rightHandWeapVariety&GUNVARIETY_LAUNCHER) && VerifySequence(monster, &generic_move_crouch_launcher_run_aim, reject_actionflags))
				{
					return &generic_move_crouch_launcher_run_aim;
				}
				if ((rightHandWeapVariety&GUNVARIETY_KNIFE) && VerifySequence(monster, &generic_move_crouch_knife_run_aim, reject_actionflags))
				{
					return &generic_move_crouch_knife_run_aim;
				}
				if(VerifySequence(monster, &generic_move_crouch_run_p, reject_actionflags))
				{
					return &generic_move_crouch_run_p;
				}
			}
		}
		//going backward...
		else// if (DotProduct(to_dest_dir, forward)>0.0)
		{
			if (isAggressive)
			{
				if ((rightHandWeapVariety&GUNVARIETY_MRS) && VerifySequence(monster, &generic_move_crouch_rbk_mrs, reject_actionflags))
				{
					return &generic_move_crouch_rbk_mrs;
				}
				if ((rightHandWeapVariety&GUNVARIETY_LAUNCHER) && VerifySequence(monster, &generic_move_crouch_launcher_runbk_aim, reject_actionflags))
				{
					return &generic_move_crouch_launcher_runbk_aim;
				}
				if ((rightHandWeapVariety&GUNVARIETY_KNIFE) && VerifySequence(monster, &generic_move_crouch_knife_runbk_aim, reject_actionflags))
				{
					return &generic_move_crouch_knife_runbk_aim;
				}
				if(VerifySequence(monster, &generic_move_crouch_rbk_p, reject_actionflags))
				{
					return &generic_move_crouch_rbk_p;
				}
			}
		}

		if (isAggressive)
		{
			if((rightHandWeapVariety&GUNVARIETY_RIFLE) && VerifySequence(monster, &generic_move_caim_r, reject_actionflags))
			{
				return &generic_move_caim_r;
			}
			if((rightHandWeapVariety&GUNVARIETY_MS) && VerifySequence(monster, &generic_move_caim_ms, reject_actionflags))
			{
				return &generic_move_caim_ms;
			}
			if((rightHandWeapVariety&GUNVARIETY_LAUNCHER) && VerifySequence(monster, &generic_move_caim_l, reject_actionflags))
			{
				return &generic_move_caim_l;
			}
			if (rightHandWeapVariety&GUNVARIETY_PISTOL && VerifySequence(monster, &generic_move_chaim2, reject_actionflags))
			{
				return &generic_move_chaim2;
			}
		}
	}

	//check for stairs here

	//this is the new way of doing it. yippee!!
/*	if (the_ai)
	{
		int		EnemyInfoType;
		CAIPathNode *curNode;
		CAIPathNode *nextNode;
		vec3_t		EnemyInfoSource, EnemyInfoDest, EnemyInfoDir;
		vec3_t		EnemyInfoMins, EnemyInfoMaxs;//bounds of current path
		int		i;
		bool		acceptInfoPoint=true;

		EnemyInfoType=the_ai->GetCurrentEnemyInfoPoint();

		curNode=aiPoints.getNode(the_ai->getPathData().curNode);
		nextNode=aiPoints.getNode(the_ai->getPathData().nextNode);

		if (curNode && nextNode)
		{
			curNode->getPos(EnemyInfoSource);
			nextNode->getPos(EnemyInfoDest);
			for (i=0;i<2;i++)
			{
				if (EnemyInfoSource[i]>EnemyInfoDest[i])
				{
					EnemyInfoMins[i]=EnemyInfoDest[i];
					EnemyInfoMaxs[i]=EnemyInfoSource[i];
				}
				else
				{
					EnemyInfoMaxs[i]=EnemyInfoDest[i];
					EnemyInfoMins[i]=EnemyInfoSource[i];
				}
			}
			VectorSubtract(EnemyInfoDest, EnemyInfoSource, EnemyInfoDir);
			EnemyInfoDir[2]=0;
			VectorNormalize(EnemyInfoDir);
*/
			//going down stairs?
/*			if (the_ai->getPathData().typeOfPath==LINK_STAIR_DOWN)
			{
				for (i=0;i<=2;i++)
				{
					//reject if outside the boundaries of the staricase
					if (monster.s.origin[i]-EnemyInfoMaxs[i]>32&&monster.s.origin[i]-EnemyInfoMins[i]<-32)
					{
						acceptInfoPoint=false;
					}
				}
				//reject if not going the same direction as the staircase
				if (DotProduct(to_dest_dir, EnemyInfoDir)<0.75)
				{
					acceptInfoPoint=false;
				}
				if (acceptInfoPoint)
				{
					if((rightHandWeapVariety&GUNVARIETY_MRS)&&VerifySequence(monster, &generic_move_rdwnstairs_mrs, reject_actionflags))
					{
						return &generic_move_rdwnstairs_mrs;
					}
					if((rightHandWeapVariety&GUNVARIETY_LAUNCHER)&&VerifySequence(monster, &generic_move_rdwnstairs_l, reject_actionflags))
					{
						return &generic_move_rdwnstairs_l;
					}
					if(VerifySequence(monster, &generic_move_rdwnstairs_p, reject_actionflags))
					{
						return &generic_move_rdwnstairs_p;
					}
				}
			}


			//going up stairs?
			if (the_ai->getPathData().typeOfPath==LINK_STAIR_UP)
			{
				for (i=0;i<=2;i++)
				{
					//reject if outside the boundaries of the staricase
					if (monster.s.origin[i]-EnemyInfoMaxs[i]>32&&monster.s.origin[i]-EnemyInfoMins[i]<-32)
					{
						acceptInfoPoint=false;
					}
				}
				//reject if not going the same direction as the staircase
				if (DotProduct(to_dest_dir, EnemyInfoDir)<0.75)
				{
					acceptInfoPoint=false;
				}
				if (acceptInfoPoint)
				{
					if((rightHandWeapVariety&GUNVARIETY_MRS)&&VerifySequence(monster, &generic_move_rupstairs_mrs, reject_actionflags))
					{
						return &generic_move_rupstairs_mrs;
					}
					if((rightHandWeapVariety&GUNVARIETY_LAUNCHER)&&VerifySequence(monster, &generic_move_rupstairs_l, reject_actionflags))
					{
						return &generic_move_rupstairs_l;
					}
					if(VerifySequence(monster, &generic_move_rupstairs_p, reject_actionflags))
					{
						return &generic_move_rupstairs_p;
					}
				}
			}*/

/*		}
	}*/
	//fixme: this check is just plain silly
	//prefer walking here
	if ((angle_dist_to_face * VectorLength(to_dest) < 10)||(GetArmorCode()==ARMOR_FULL))
	{

		tdot=DotProduct (right, to_dest_dir);

		if (GetBestWeapon(monster)!=ATK_NOTHING)
		{
			float dist_to_dest = VectorLength(to_dest);
			if (angle_dist_to_face < 0.1 && dist_to_dest < 10 && dist_to_dest > 0)
			{
				if(rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_wturn180_mrs, reject_actionflags))
				{
					return &generic_move_wturn180_mrs;
				}
			}
			if (((angle_dist_to_face < 1.9 && currentmove&&(currentmove->bodyPosition==BODYPOS_WALKTURN))||
				(angle_dist_to_face < 1.2)) && dist_to_dest < 10 && dist_to_dest > 0)
			{
				if (tdot > 0.0)
				{
					if(rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_wturn90r_mrs, reject_actionflags))
					{
						return &generic_move_wturn90r_mrs;
					}
					if(VerifySequence(monster, &generic_move_wturn90r_p, reject_actionflags))
					{
						return &generic_move_wturn90r_p;
					}
				}
				else
				{
					if(rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_wturn90l_mrs, reject_actionflags))
					{
						return &generic_move_wturn90l_mrs;
					}
					if(VerifySequence(monster, &generic_move_wturn90l_p, reject_actionflags))
					{
						return &generic_move_wturn90l_p;
					}
				}
			}

			if (isAggressive)
			{
				//wanna strafewalk...
				if (auxmove = GetSequenceForWalkAimStrafe(monster, to_dest_dir, forward, right, dest, face, seqtype, goal_bbox, ideal_move, reject_actionflags))
				{
					return auxmove;
				}
				//walk back...
				if (DotProduct(to_dest_dir, forward)<-0.75)
				{
					if (rightHandWeapVariety&GUNVARIETY_RIFLE && VerifySequence(monster, &generic_move_wbackaim_r, reject_actionflags))
					{
						return &generic_move_wbackaim_r;
					}
					if (rightHandWeapVariety&GUNVARIETY_MS && VerifySequence(monster, &generic_move_wbackaim_ms, reject_actionflags))
					{
						return &generic_move_wbackaim_ms;
					}
					if (rightHandWeapVariety&GUNVARIETY_LAUNCHER && VerifySequence(monster, &generic_move_wbackaim_l, reject_actionflags))
					{
						return &generic_move_wbackaim_l;
					}
					if (VerifySequence(monster, &generic_move_wbackaim_p, reject_actionflags))
					{
						return &generic_move_wbackaim_p;
					}
				}
			}

			if(rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_walk_mrs, reject_actionflags))
			{
				return &generic_move_walk_mrs;
			}
			if(rightHandWeapVariety&GUNVARIETY_LAUNCHER && VerifySequence(monster, &generic_move_walk_l, reject_actionflags))
			{
				return &generic_move_walk_l;
			}
			if(VerifySequence(monster, &generic_move_walk_attitude, reject_actionflags))
			{
				return &generic_move_walk_attitude;
			}
			if(VerifySequence(monster, &generic_move_walk, reject_actionflags))
			{
				return &generic_move_walk;
			}
		}
		//unarmed
		else
		{
			if (((angle_dist_to_face < 1.9 && currentmove && (currentmove->bodyPosition==BODYPOS_WALKTURN))||
				(angle_dist_to_face < 1.2)) && VectorLength(to_dest) < 10)
			{
				if (tdot > 0.0)
				{
					if(VerifySequence(monster, &generic_move_wturn90r_p, reject_actionflags))
					{
						return &generic_move_wturn90r_p;
					}
				}
				else
				{
					if(VerifySequence(monster, &generic_move_wturn90l_p, reject_actionflags))
					{
						return &generic_move_wturn90l_p;
					}
				}
			}
			//wanna strafewalk...
			if (isAggressive)
			{
				if (auxmove = GetSequenceForWalkAimStrafe(monster, to_dest_dir, forward, right, dest, face, seqtype, goal_bbox, ideal_move, reject_actionflags))
				{
					return auxmove;
				}
			}

			//walk backwerd...
			if (DotProduct(to_dest_dir, forward)<-0.75)
			{
				if (isAggressive && VerifySequence(monster, &generic_move_wbackaim_p, reject_actionflags))
				{
					return &generic_move_wbackaim_p;
				}
			}
			if(VerifySequence(monster, &generic_move_walk, reject_actionflags))
			{
				return &generic_move_walk;
			}
			if (GetBestWeapon(monster)==ATK_NOTHING)
			{
				if(VerifySequence(monster, &generic_move_runscared, reject_actionflags))
				{
					return &generic_move_runscared;
				}
			}
		}

		if(rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_walk_jog_mrs, reject_actionflags))
		{
			return &generic_move_walk_jog_mrs;
		}
		if(rightHandWeapVariety&GUNVARIETY_PISTOL && VerifySequence(monster, &generic_move_walk_jog_gunup_p, reject_actionflags))
		{
			return &generic_move_walk_jog_gunup_p;
		}
//		if(rightHandWeapVariety&GUNVARIETY_PISTOL && VerifySequence(monster, &generic_move_walk_jog_gundwn_p, reject_actionflags))
//		{
//			return &generic_move_walk_jog_gundwn_p;
//		}
		if(rightHandWeapVariety&GUNVARIETY_PISTOL && VerifySequence(monster, &generic_move_run_gunup, reject_actionflags))
		{
			return &generic_move_run_gunup;
		}
		if(rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_run_mrs, reject_actionflags))
		{
			return &generic_move_run_mrs;
		}
		if(VerifySequence(monster, &generic_move_run, reject_actionflags))
		{
			return &generic_move_run;
		}

		if (isAggressive)
		{
			if (rightHandWeapVariety&GUNVARIETY_PK && VerifySequence(monster, &generic_move_waim_p, reject_actionflags))
			{
				return &generic_move_waim_p;
			}
			if (rightHandWeapVariety&GUNVARIETY_RIFLE && VerifySequence(monster, &generic_move_waim_r, reject_actionflags))
			{
				return &generic_move_waim_r;
			}
			if (rightHandWeapVariety&GUNVARIETY_MS && VerifySequence(monster, &generic_move_waim_ms, reject_actionflags))
			{
				return &generic_move_waim_ms;
			}
			if (rightHandWeapVariety&GUNVARIETY_LAUNCHER && VerifySequence(monster, &generic_move_waim_l, reject_actionflags))
			{
				return &generic_move_waim_l;
			}
		}

		//yike! i don't know how to move!!
		if(rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_stand_mrs, reject_actionflags))
		{
			return &generic_move_stand_mrs;
		}
		if(VerifySequence(monster, &generic_move_stand, reject_actionflags))
		{
			return &generic_move_stand;
		}
		if(/*rightHandWeapVariety&GUNVARIETY_PK &&*/ VerifySequence(monster, &generic_move_alert_p, reject_actionflags))
		{
			return &generic_move_alert_p;
		}
		return &generic_move_stand;
	}
	//prefer running here
	else
	{
		if (isAggressive)
		{
			//wanna straferun...
			if (auxmove = GetSequenceForRunAimStrafe(monster, to_dest_dir, forward, right, dest, face, seqtype, goal_bbox, ideal_move, reject_actionflags))
			{
				return auxmove;
			}
			if (auxmove = GetSequenceForWalkAimStrafe(monster, to_dest_dir, forward, right, dest, face, seqtype, goal_bbox, ideal_move, reject_actionflags))
			{
				return auxmove;
			}
		}

		if (GetBestWeapon(monster)==ATK_NOTHING)
		{
			if(VerifySequence(monster, &generic_move_runscared, reject_actionflags))
			{
				return &generic_move_runscared;
			}
		}
		tdot=DotProduct (right, to_dest_dir);
		if (isAggressive)
		{
			//run forward...
			if (DotProduct(to_dest_dir, forward)>0)
			{
				if (rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_raim_mrs, reject_actionflags))
				{
					return &generic_move_raim_mrs;
				}
				if (rightHandWeapVariety&GUNVARIETY_PISTOL && VerifySequence(monster, &generic_move_raim_p2, reject_actionflags))
				{
					return &generic_move_raim_p2;
				}
				if (rightHandWeapVariety&GUNVARIETY_PK && VerifySequence(monster, &generic_move_raim_p1, reject_actionflags))
				{
					return &generic_move_raim_p1;
				}
				if (rightHandWeapVariety&GUNVARIETY_LAUNCHER && VerifySequence(monster, &generic_move_raim_l, reject_actionflags))
				{
					return &generic_move_raim_l;
				}
			}
			//run back...
			else
			{
				if (rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_rbackaim_mrs, reject_actionflags))
				{
					return &generic_move_rbackaim_mrs;
				}
				if (rightHandWeapVariety&GUNVARIETY_PISTOL && VerifySequence(monster, &generic_move_rbackaim_p2, reject_actionflags))
				{
					return &generic_move_rbackaim_p2;
				}
				if (rightHandWeapVariety&GUNVARIETY_LAUNCHER && VerifySequence(monster, &generic_move_rbackaim_l, reject_actionflags))
				{
					return &generic_move_rbackaim_l;
				}
			}
		}
		if(rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_run_mrs, reject_actionflags))
		{
			return &generic_move_run_mrs;
		}
		if(VerifySequence(monster, &generic_move_run, reject_actionflags))
		{
			return &generic_move_run;
		}
		if(rightHandWeapVariety&GUNVARIETY_PISTOL && VerifySequence(monster, &generic_move_run_gunup, reject_actionflags))
		{
			return &generic_move_run_gunup;
		}
		if(rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_walk_jog_mrs, reject_actionflags))
		{
			return &generic_move_walk_jog_mrs;
		}
		if(rightHandWeapVariety&GUNVARIETY_PISTOL && VerifySequence(monster, &generic_move_walk_jog_gunup_p, reject_actionflags))
		{
			return &generic_move_walk_jog_gunup_p;
		}
//		if(rightHandWeapVariety&GUNVARIETY_PISTOL && VerifySequence(monster, &generic_move_walk_jog_gundwn_p, reject_actionflags))
//		{
//			return &generic_move_walk_jog_gundwn_p;
//		}
		if(rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_walk_mrs, reject_actionflags))
		{
			return &generic_move_walk_mrs;
		}
		if(rightHandWeapVariety&GUNVARIETY_LAUNCHER && VerifySequence(monster, &generic_move_walk_l, reject_actionflags))
		{
			return &generic_move_walk_l;
		}
		if(VerifySequence(monster, &generic_move_walk, reject_actionflags))
		{
			return &generic_move_walk;
		}

		if (isAggressive)
		{
			if (rightHandWeapVariety&GUNVARIETY_PK && VerifySequence(monster, &generic_move_waim_p, reject_actionflags))
			{
				return &generic_move_waim_p;
			}
			if (rightHandWeapVariety&GUNVARIETY_RIFLE && VerifySequence(monster, &generic_move_waim_r, reject_actionflags))
			{
				return &generic_move_waim_r;
			}
			if (rightHandWeapVariety&GUNVARIETY_MS && VerifySequence(monster, &generic_move_waim_ms, reject_actionflags))
			{
				return &generic_move_waim_ms;
			}
			if (rightHandWeapVariety&GUNVARIETY_LAUNCHER && VerifySequence(monster, &generic_move_waim_l, reject_actionflags))
			{
				return &generic_move_waim_l;
			}
		}

		//yike! i don't know how to move!!
		if(rightHandWeapVariety&GUNVARIETY_MRS && VerifySequence(monster, &generic_move_stand_mrs, reject_actionflags))
		{
			return &generic_move_stand_mrs;
		}
		if(VerifySequence(monster, &generic_move_stand, reject_actionflags))
		{
			return &generic_move_stand;
		}
		if(/*rightHandWeapVariety&GUNVARIETY_PK &&*/ VerifySequence(monster, &generic_move_alert_p, reject_actionflags))
		{
			return &generic_move_alert_p;
		}
		return &generic_move_stand;
	}
}

#define MAX_PRONE_DIST_SQUARED 65000

mmove_t *bodyhuman_c::GetSequenceForDodge(edict_t &monster, vec3_t dest, vec3_t face, edict_t *target, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int leftSide, int reject_actionflags)
{
/*	if (currentmove && currentmove->suggested_action==ACTCODE_DODGE)
	{
		return currentmove;
	}*/

	vec3_t dif;

//	if (currentmove && currentmove->suggested_action != ACTCODE_PAIN)
//	{
//		VoiceSound("duck", monster, 0);
//	}

	if (monster.spawnflags & SPAWNFLAG_HOLD_POSITION)
	{
//		return GetSequenceForStand(monster, vec3_origin, vec3_origin, seqtype, goal_bbox, ideal_move, reject_actionflags);
		return NULL;
	}

	if(target)
	{
		VectorSubtract(target->s.origin, monster.s.origin, dif);
	}
	else
	{
		VectorClear(dif);
	}

	if(VectorLengthSquared(dif) > MAX_PRONE_DIST_SQUARED)
	{
		//	generic_move_go_prone
		// if running, dive 'cause it's cool - fixme

		float rVal = gi.flrand(0,1);

//		if (/*!leftSide && rVal < 0.25 && */VerifySequence(monster, &generic_move_jumpthrowstars_bk, reject_actionflags))
//		{
//			return &generic_move_jumpthrowstars_bk;
//		}
//		if (/*!leftSide && rVal < 0.25 && */VerifySequence(monster, &generic_move_jumpthrowstars, reject_actionflags))
//		{
//			return &generic_move_jumpthrowstars;
//		}
		if(rVal < 0.25)
		{
			if(VerifySequence(monster, &generic_move_backflip, reject_actionflags))
			{
				return &generic_move_backflip;
			}
		}
		if(rVal < .25)
		{
			if(VerifySequence(monster, &generic_move_roll_to_crouch, reject_actionflags))
			{
				return &generic_move_roll_to_crouch;
			}
		}
		if(rVal < .5)
		{
			if(VerifySequence(monster, &generic_move_dive_toprone, reject_actionflags))
			{
				return &generic_move_dive_toprone;
			}
		}

		if(leftSide == -1)
		{
			if(VerifySequence(monster, &generic_move_rollrt_p, reject_actionflags))
			{
				return &generic_move_rollrt_p;
			}
		}
		else if(leftSide == 1)
		{
			if(VerifySequence(monster, &generic_move_rolllt_p, reject_actionflags))
			{
				return &generic_move_rolllt_p;
			}

		}
	}
//	if (/*!leftSide && !gi.irand(0,10) && */VerifySequence(monster, &generic_move_jumpthrowstars_bk, reject_actionflags))
//	{
//		return &generic_move_jumpthrowstars_bk;
//	}
//	if (/*!leftSide && !gi.irand(0,10) && */VerifySequence(monster, &generic_move_jumpthrowstars, reject_actionflags))
//	{
//		return &generic_move_jumpthrowstars;
//	}
	if(leftSide == -1)
	{
		if(VerifySequence(monster, &generic_move_rollrt_p, reject_actionflags))
		{
			return &generic_move_rollrt_p;
		}
	}
	else if(leftSide == 1)
	{
		if(VerifySequence(monster, &generic_move_rolllt_p, reject_actionflags))
		{
			return &generic_move_rolllt_p;
		}
	}
	if(VerifySequence(monster, &generic_move_backflip, reject_actionflags))
	{
		return &generic_move_backflip;
	}
	if(VerifySequence(monster, &generic_move_roll_to_crouch, reject_actionflags))
	{
		return &generic_move_roll_to_crouch;
	}

//	return currentmove;
	return NULL;
}

mmove_t	*bodyhuman_c::GetSequenceForStand(edict_t &monster, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);
	gunvariety_t	rightHandWeapVariety=GetGunVariety(GetRightHandWeapon(monster));

	if (ideal_move && VerifySequence(monster, ideal_move, reject_actionflags))
	{
		return ideal_move;
	}

	//if i'm disabled, i'm disabled. what can you do about something like that?
	mmove_t	*auxmove;
	if (auxmove=GetSequenceForDisability(monster, reject_actionflags))
	{
		return auxmove;
	}

	//hit in leg--can consolidate these into simple bodyDamage&BODYDAMAGE_ANY chack?
	if (bodyDamage & BODYDAMAGE_ANY)
	{
		if (GetBestWeapon(monster)==ATK_NOTHING&&(monster.ai || goal_bbox==BBOX_PRESET_CROUCH))
		{
			if (auxmove=GetSequenceForTransition(monster, dest, face, seqtype, BBOX_PRESET_CROUCH, reject_actionflags))
			{
				return auxmove;
			}
			if (currentmove && (currentmove==&generic_move_crouch_cower_mid2 || &generic_move_crouch_cower_mid) &&
				!IsAnimationFinished())
			{
				return currentmove;
			}
			if((gi.irand(0,2)||!VerifySequence(monster, &generic_move_crouch_cower_mid2, reject_actionflags))&&
				VerifySequence(monster, &generic_move_crouch_cower_mid, reject_actionflags))
			{
				return &generic_move_crouch_cower_mid;
			}
			if(VerifySequence(monster, &generic_move_crouch_cower_mid2, reject_actionflags))
			{
				return &generic_move_crouch_cower_mid2;
			}
		}
		// guys look bad with this anim - you can wander around them and they just stand there
/*		if (GetRightHandWeapon(monster)!=ATK_NOTHING&&(monster.ai || goal_bbox==BBOX_PRESET_STAND)&&IsAvailableSequence(monster, &generic_move_hurt_aim))
		{
			return &generic_move_hurt_aim;
		}*/
	}

	if (auxmove=GetSequenceForTransition(monster, dest, face, seqtype, goal_bbox, reject_actionflags))
	{
		return auxmove;
	}

	float aimCone;
	float tempAng;

	if (the_ai)
	{
		tempAng = anglemod(the_ai->ideal_angles[YAW]-monster.s.angles[YAW]);
		if (currentmove)
		{
			aimCone = the_ai->GetAimConeDegrees(monster, currentmove->bbox);
		}
		else
		{
			aimCone = the_ai->GetAimConeDegrees(monster, BBOX_PRESET_STAND);
		}
	}
	else
	{
		tempAng = 0;//uh??
		aimCone = 10;//eh?
	}

	if (currentmove && (currentmove->suggested_action==ACTCODE_STAND/* || currentmove->suggested_action==ACTCODE_ATTACK*/)
		&& (!(currentmove->actionFlags&ACTFLAG_OPTATTACK))&&(!IsAnimationFinished()))
	{
//		if (!/*(currentmove->suggested_action==ACTCODE_ATTACK) || */(monster.client || (the_ai && the_ai->HasTarget())))
		//if i have a target, or i need to turn, then forget whatever animation i'm in now
		if (!(monster.client || (the_ai && the_ai->HasTarget()))&&!(tempAng > aimCone && tempAng < 360.0-aimCone))
		{
			return currentmove; // ask steve about this when back from vacation
		}
	}

	if (currentmove&&(currentmove->bbox==BBOX_PRESET_PRONE ||currentmove->bbox==BBOX_PRESET_ALLFOURS))
	{
		if(VerifySequence(monster, &generic_move_praim_mrs, reject_actionflags))
		{
			return &generic_move_praim_mrs;
		}
	}

	if(currentmove&&currentmove->bbox == BBOX_PRESET_CROUCH)
	{
		if (GetBestWeapon(monster)==ATK_NOTHING && emotion == EMOTION_AFRAID)
		{
			if(gi.irand(0,1)&&VerifySequence(monster, &generic_move_crouch_cower_mid, reject_actionflags))
			{
				return &generic_move_crouch_cower_mid;
			}
			if(VerifySequence(monster, &generic_move_crouch_cower_mid2, reject_actionflags))
			{
				return &generic_move_crouch_cower_mid2;
			}
		}
		if(rightHandWeapVariety&GUNVARIETY_RIFLE && VerifySequence(monster, &generic_move_caim_r, reject_actionflags))
		{
			return &generic_move_caim_r;
		}
		if(rightHandWeapVariety&GUNVARIETY_MS && VerifySequence(monster, &generic_move_caim_ms, reject_actionflags))
		{
			return &generic_move_caim_ms;
		}
		if((rightHandWeapVariety&GUNVARIETY_LAUNCHER) && VerifySequence(monster, &generic_move_caim_l, reject_actionflags))
		{
			return &generic_move_caim_l;
		}
		if (IsAvailableSequence(monster, &generic_move_duck_shoot) && VerifySequence(monster, &generic_move_chaim2, reject_actionflags))
		{
			return &generic_move_chaim2;
		}
	}

	if(currentmove&&currentmove->bbox == BBOX_PRESET_LAYSIDE)
	{
		return currentmove;
	}

	if(currentmove&&currentmove->bbox == BBOX_PRESET_LAYBACK)
	{
		if (auxmove=GetSequenceForTransition(monster, dest, face, seqtype, BBOX_PRESET_STAND, reject_actionflags))
		{
			return auxmove;
		}

		//er, try scooching?
		if (VerifySequence(monster, &generic_move_layback_scooch, reject_actionflags))
		{
			if (currentmove != &generic_move_layback_scooch)
			{
				VoiceGeneralSound("Writhe",monster,WRITHENUMBER);
			}
			return &generic_move_layback_scooch;
		}
		if (VerifySequence(monster, &generic_move_layback_die_shoot, reject_actionflags))
		{
			return &generic_move_layback_die_shoot;
		}

		//forget it--count me dead
		if (VerifySequence(monster, &generic_move_death_lbshotdeath, reject_actionflags))
		{
			return &generic_move_death_lbshotdeath;
		}

		return currentmove;//ah! yikes!
	}

	vec3_t forward, right, to_dest_dir;
	VectorSubtract(face, monster.s.origin, to_dest_dir);
	VectorNormalize(to_dest_dir);
	AngleVectors(monster.s.angles, forward, right, NULL);

	if (the_ai && level.time-the_ai->getLastTargetTime()<10.0 && the_ai->HasHadTarget())
	{
		//i'm outside the aim cone...use ideal_angles for aiming
		if (tempAng > aimCone && tempAng < 360.0-aimCone)
		{
			if (rightHandWeapVariety&GUNVARIETY_MS&&VerifySequence(monster, &generic_move_ashuffle_ms, reject_actionflags))
			{
				return &generic_move_ashuffle_ms;
			}
			if (rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_ashuffle_r, reject_actionflags))
			{
				return &generic_move_ashuffle_r;
			}
			if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_ashuffle_p2, reject_actionflags))
			{
				return &generic_move_ashuffle_p2;
			}
			if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_ashuffle_pcb, reject_actionflags))
			{
				return &generic_move_ashuffle_pcb;
			}
			if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_ashuffle_l, reject_actionflags))
			{
				return &generic_move_ashuffle_l;
			}
		}

		if (rightHandWeapVariety&GUNVARIETY_MRS)
		{
			if (rightHandWeapVariety&GUNVARIETY_MS&&VerifySequence(monster, &generic_move_staim_ms, reject_actionflags))
			{
				return &generic_move_staim_ms;
			}
			if (rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_staim_r, reject_actionflags))
			{
				return &generic_move_staim_r;
			}
			if(VerifySequence(monster, &generic_move_alert_mrs, reject_actionflags))
			{
				return &generic_move_alert_mrs;
			}
		}
		else if (rightHandWeapVariety==GUNVARIETY_NONE)
		{
			if(emotion == EMOTION_AFRAID && VerifySequence(monster, &generic_move_fear_wave, reject_actionflags) && !gi.irand(0,10))
			{
				return &generic_move_fear_wave;
			}
			if(emotion == EMOTION_AFRAID && VerifySequence(monster, &generic_move_cower, reject_actionflags))
			{
				return &generic_move_cower;
			}
		}

		if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staim_p2, reject_actionflags))
		{
			return &generic_move_staim_p2;
		}
		if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staim_pcb, reject_actionflags))
		{
			return &generic_move_staim_pcb;
		}
		if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_staim_l, reject_actionflags))
		{
			return &generic_move_staim_l;
		}
		if(VerifySequence(monster, &generic_move_alert_p, reject_actionflags))
		{
			return &generic_move_alert_p;
		}

	}
	if (the_ai && level.time-the_ai->getLastTargetTime()<20.0 && the_ai->HasHadTarget())
	{
		//i'm outside the aim cone...use ideal_angles for aiming
		if (tempAng > aimCone && tempAng < 360.0-aimCone)
		{
			if (rightHandWeapVariety&GUNVARIETY_MS&&VerifySequence(monster, &generic_move_ashuffle_ms, reject_actionflags))
			{
				return &generic_move_ashuffle_ms;
			}
			if (rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_ashuffle_r, reject_actionflags))
			{
				return &generic_move_ashuffle_r;
			}
			if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_ashuffle_p2, reject_actionflags))
			{
				return &generic_move_ashuffle_p2;
			}
			if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_ashuffle_pcb, reject_actionflags))
			{
				return &generic_move_ashuffle_pcb;
			}
			if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_ashuffle_l, reject_actionflags))
			{
				return &generic_move_ashuffle_l;
			}
		}

		if (rightHandWeapVariety&GUNVARIETY_MRS)
		{
			if((rand()%15) && VerifySequence(monster, &generic_move_atkpause1_mrs, reject_actionflags))
			{
				return &generic_move_atkpause1_mrs;
			}
			if(VerifySequence(monster, &generic_move_atkpause2_mrs, reject_actionflags))
			{
				return &generic_move_atkpause2_mrs;
			}
			if (rightHandWeapVariety&GUNVARIETY_MS&&VerifySequence(monster, &generic_move_staim_ms, reject_actionflags))
			{
				return &generic_move_staim_ms;
			}
			if (rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_staim_r, reject_actionflags))
			{
				return &generic_move_staim_r;
			}
			if(VerifySequence(monster, &generic_move_alert_mrs, reject_actionflags))
			{
				return &generic_move_alert_mrs;
			}
		}
		else if (rightHandWeapVariety==GUNVARIETY_NONE)
		{
			if(emotion == EMOTION_AFRAID && VerifySequence(monster, &generic_move_fear_wave, reject_actionflags) && !gi.irand(0,10))
			{
				return &generic_move_fear_wave;
			}
			if(emotion == EMOTION_AFRAID && VerifySequence(monster, &generic_move_cower, reject_actionflags))
			{
				return &generic_move_cower;
			}
		}

		if((rand()%15) && VerifySequence(monster, &generic_move_atkpause1_p, reject_actionflags))
		{
			return &generic_move_atkpause1_p;
		}
		if(VerifySequence(monster, &generic_move_atkpause2_p, reject_actionflags))
		{
			return &generic_move_atkpause2_p;
		}
		if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staim_p2, reject_actionflags))
		{
			return &generic_move_staim_p2;
		}
		if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staim_pcb, reject_actionflags))
		{
			return &generic_move_staim_pcb;
		}
		if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_staim_l, reject_actionflags))
		{
			return &generic_move_staim_l;
		}
		if(VerifySequence(monster, &generic_move_alert_p, reject_actionflags))
		{
			return &generic_move_alert_p;
		}

	}
	if (the_ai && !the_ai->HasHadTarget())
	{
		//mgun, rifle moves
		if (rightHandWeapVariety&GUNVARIETY_MRS)
		{
			if(!(rand()%5) && currentmove != &generic_move_standlook_mrs && VerifySequence(monster, &generic_move_standlook_mrs, reject_actionflags))
			{
				return &generic_move_standlook_mrs;
			}
			if(!(rand()%3) && currentmove != &generic_move_standstretch_mrs && VerifySequence(monster, &generic_move_standstretch_mrs, reject_actionflags))
			{
				return &generic_move_standstretch_mrs;
			}
			if(VerifySequence(monster, &generic_move_stand_mrs, reject_actionflags))
			{
				return &generic_move_stand_mrs;
			}
		}

		if(!(rand()%6) && currentmove != &generic_move_standlook && VerifySequence(monster, &generic_move_standlook, reject_actionflags))
		{
			return &generic_move_standlook;
		}
		if(!(gi.irand(0,50)) && currentmove != &generic_move_ishift && VerifySequence(monster, &generic_move_ishift, reject_actionflags))
		{
			return &generic_move_ishift;
		}
		if(!(rand()%4) && currentmove != &generic_move_standstretch && VerifySequence(monster, &generic_move_standstretch, reject_actionflags))
		{
			return &generic_move_standstretch;
		}
		if(VerifySequence(monster, &generic_move_stand, reject_actionflags))
		{
			return &generic_move_stand;
		}
		if(rightHandWeapVariety&GUNVARIETY_PK && VerifySequence(monster, &generic_move_alert_p, reject_actionflags))
		{
			return &generic_move_alert_p;
		}
	}
	else if (the_ai)
	{
		//i'm outside the aim cone...use ideal_angles for aiming
		if (tempAng > aimCone && tempAng < 360.0-aimCone)
		{
			if (rightHandWeapVariety&GUNVARIETY_MS&&VerifySequence(monster, &generic_move_ashuffle_ms, reject_actionflags))
			{
				return &generic_move_ashuffle_ms;
			}
			if (rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_ashuffle_r, reject_actionflags))
			{
				return &generic_move_ashuffle_r;
			}
			if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_ashuffle_p2, reject_actionflags))
			{
				return &generic_move_ashuffle_p2;
			}
			if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_ashuffle_pcb, reject_actionflags))
			{
				return &generic_move_ashuffle_pcb;
			}
			if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_ashuffle_l, reject_actionflags))
			{
				return &generic_move_ashuffle_l;
			}
		}

		if (rightHandWeapVariety&GUNVARIETY_MRS)
		{
			if(!(rand()%13) && currentmove != &generic_move_alertlkrt_mrs && VerifySequence(monster, &generic_move_alertlkrt_mrs, reject_actionflags))
			{
				return &generic_move_alertlkrt_mrs;
			}
			if(!(rand()%11) && currentmove != &generic_move_alerttrnlt_mrs && VerifySequence(monster, &generic_move_alerttrnlt_mrs, reject_actionflags))
			{
				return &generic_move_alerttrnlt_mrs;
			}
			if(VerifySequence(monster, &generic_move_alert_mrs, reject_actionflags))
			{
				return &generic_move_alert_mrs;
			}
		}
		else if (rightHandWeapVariety==GUNVARIETY_NONE)
		{
			if(emotion == EMOTION_AFRAID && VerifySequence(monster, &generic_move_fear_wave, reject_actionflags) && !gi.irand(0,10))
			{
				return &generic_move_fear_wave;
			}
			if(emotion == EMOTION_AFRAID && VerifySequence(monster, &generic_move_cower, reject_actionflags))
			{
				return &generic_move_cower;
			}
		}

		if((!(rand()%15)) && currentmove != &generic_move_alertlklt_p && VerifySequence(monster, &generic_move_alertlklt_p, reject_actionflags))
		{
			return &generic_move_alertlklt_p;
		}
		if((!(rand()%9)) && currentmove != &generic_move_alerttrnrt_p && VerifySequence(monster, &generic_move_alerttrnrt_p, reject_actionflags))
		{
			return &generic_move_alerttrnrt_p;
		}
		if( VerifySequence(monster, &generic_move_alert_p, reject_actionflags))
		{
			return &generic_move_alert_p;
		}
	}

	//should not be getting here!!!
	if (rightHandWeapVariety&GUNVARIETY_MRS)
	{
		if(VerifySequence(monster, &generic_move_stand_mrs, reject_actionflags))
		{
			return &generic_move_stand_mrs;
		}
	}
	if(VerifySequence(monster, &generic_move_stand, reject_actionflags))
	{
		return &generic_move_stand;
	}
	if (rightHandWeapVariety&GUNVARIETY_MRS)
	{
		if(VerifySequence(monster, &generic_move_walk_mrs, reject_actionflags))
		{
			return &generic_move_walk_mrs;
		}
	}
	if(VerifySequence(monster, &generic_move_walk, reject_actionflags))
	{
		return &generic_move_walk;
	}
//	if(rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_alert_p, reject_actionflags))
//	{
//		return &generic_move_alert_p;
//	}
	if(rightHandWeapVariety&GUNVARIETY_MRS&&VerifySequence(monster, &generic_move_alert_mrs, reject_actionflags))
	{
		return &generic_move_alert_mrs;
	}

	//aiming up--use as aim and attack
	if (to_dest_dir[2]>0.25)
	{
		if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staimup_p, reject_actionflags))
		{
			return &generic_move_staimup_p;
		}
		if (rightHandWeapVariety&GUNVARIETY_MRS&&VerifySequence(monster, &generic_move_staimup_mrs, reject_actionflags))
		{
			return &generic_move_staimup_mrs;
		}
		if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_staimup_l, reject_actionflags))
		{
			return &generic_move_staimup_l;
		}
	}
	//aiming down
	if (to_dest_dir[2]<-0.25)
	{
		if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staimdn_p, reject_actionflags))
		{
			return &generic_move_staimdn_p;
		}
		if (rightHandWeapVariety&GUNVARIETY_MRS&&VerifySequence(monster, &generic_move_staimdn_mrs, reject_actionflags))
		{
			return &generic_move_staimdn_mrs;
		}
		if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_staimdn_l, reject_actionflags))
		{
			return &generic_move_staimdn_l;
		}
	}

	if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staim_p2, reject_actionflags))
	{
		return &generic_move_staim_p2;
	}
	if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staim_pcb, reject_actionflags))
	{
		return &generic_move_staim_pcb;
	}
	if (rightHandWeapVariety&GUNVARIETY_MS&&VerifySequence(monster, &generic_move_staim_ms, reject_actionflags))
	{
		return &generic_move_staim_ms;
	}
	if (rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_staim_r, reject_actionflags))
	{
		return &generic_move_staim_r;
	}
	if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_staim_l, reject_actionflags))
	{
		return &generic_move_staim_l;
	}
	if(/*rightHandWeapVariety&GUNVARIETY_KNIFE&&*/VerifySequence(monster, &generic_move_alert_p, reject_actionflags))
	{
		return &generic_move_alert_p;
	}

	if (dm->isDM())
	{
		return currentmove;
	}
	return &generic_move_stand;
}

mmove_t	*bodyhuman_c::GetSequenceForJump(edict_t &monster, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	gunvariety_t	rightHandWeapVariety=GetGunVariety(GetRightHandWeapon(monster));
	if (ideal_move && VerifySequence(monster, ideal_move, reject_actionflags))
	{
		return ideal_move;
	}

	//if i'm disabled, i'm disabled. what can you do about something like that?
	mmove_t	*auxmove;
	if (auxmove=GetSequenceForDisability(monster, reject_actionflags))
	{
		return auxmove;
	}

	if (auxmove=GetSequenceForTransition(monster, dest, face, seqtype, goal_bbox, reject_actionflags))
	{
		return auxmove;
	}

	if((rightHandWeapVariety&GUNVARIETY_LAUNCHER)&&VerifySequence(monster, &generic_move_jumpstrup_jump_l, reject_actionflags))
	{
		return &generic_move_jumpstrup_jump_l;
	}

	if(VerifySequence(monster, &generic_move_jumpstrup_jump, reject_actionflags))
	{
		return &generic_move_jumpstrup_jump;
	}
	if(VerifySequence(monster, &generic_move_jumpup_jump, reject_actionflags))
	{
		return &generic_move_jumpup_jump;
	}
	return GetSequenceForStand(monster, dest, face, seqtype, goal_bbox, ideal_move, reject_actionflags);
}

mmove_t	*bodyhuman_c::GetPostShootSequence(edict_t &monster, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	if (!currentmove || currentmove->bbox != BBOX_PRESET_STAND)
	{
		return NULL;
	}

	attacks_e		rightHandWeapAttack=GetRightHandWeapon(monster);
	gunvariety_t	rightHandWeapVariety=GetGunVariety(rightHandWeapAttack);
	
	if (currentmove->bbox == BBOX_PRESET_STAND)
	{
		if (rightHandWeapAttack==ATK_SHOTGUN&&VerifySequence(monster, &generic_move_scock_s, reject_actionflags))
		{
			return &generic_move_scock_s;
		}
		if (rightHandWeapAttack==ATK_SNIPER&&VerifySequence(monster, &generic_move_scock_r, reject_actionflags))
		{
			return &generic_move_scock_r;
		}
	}

	if (currentmove->bbox == BBOX_PRESET_CROUCH)
	{
		if (rightHandWeapAttack==ATK_SHOTGUN&&VerifySequence(monster, &generic_move_ccock_s, reject_actionflags))
		{
			return &generic_move_ccock_s;
		}
	}

	return NULL;
}

mmove_t	*bodyhuman_c::GetSequenceForReload(edict_t &monster, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t	*auxmove;

	if (currentmove && currentmove->suggested_action != ACTCODE_PAIN)
	{
//		VoiceSound("reload", monster, 0);
	}

//	if (auxmove=GetSequenceForTransition(monster, dest, face, seqtype, BBOX_PRESET_STAND, reject_actionflags))
	if (auxmove=GetSequenceForTransition(monster, dest, face, seqtype, goal_bbox, reject_actionflags))
	{
		return auxmove;
	}

	gunvariety_t	rightHandWeapVariety=GetGunVariety(GetRightHandWeapon(monster));
	vec3_t	to_dest, to_dest_dir;
//	vec3_t	to_face, to_face_dir;
	vec3_t	forward, right, up;
	float	to_dest_dist;
//	float	to_face_dist;

	if (VectorCompare(dest, vec3_origin)||VectorCompare(dest, monster.s.origin))
	{
		VectorCopy(vec3_origin, to_dest);
		VectorCopy(vec3_origin, to_dest_dir);
		to_dest_dist=0;
	}
	else
	{
		VectorSubtract(dest, monster.s.origin, to_dest);
		VectorCopy(to_dest, to_dest_dir);
		to_dest_dist = VectorNormalize(to_dest_dir);
	}

	AngleVectors(monster.s.angles, forward, right, up);
/*
	if (VectorCompare(face, vec3_origin)||VectorCompare(face, monster.s.origin))
	{
		VectorCopy(vec3_origin, to_face);
		VectorCopy(vec3_origin, to_face_dir);
		to_face_dist=0;
	}
	else
	{
		VectorSubtract(face, monster.s.origin, to_face);
		VectorCopy(to_face, to_face_dir);
		to_face_dist = VectorNormalize(to_face_dir);
	}
*/
	ReloadRWeapon(monster);
	
	if (rightHandWeapVariety&GUNVARIETY_MS)
	{
		if (goal_bbox==BBOX_PRESET_CROUCH)
		{
			//want to move and reload
			if (to_dest_dist > 1)
			{
				if (DotProduct(forward, to_dest_dir)<-0.5 && VerifySequence(monster, &generic_move_reload_crun_bk_ms, reject_actionflags))
				{
					return &generic_move_reload_crun_bk_ms;
				}
				if (VerifySequence(monster, &generic_move_reload_crun_fwd_ms, reject_actionflags))
				{
					return &generic_move_reload_crun_fwd_ms;
				}
			}
			if (VerifySequence(monster, &generic_move_reload_c_ms, reject_actionflags))
			{
				return &generic_move_reload_c_ms;
			}
		}
		//want to run and reload
		if (to_dest_dist > 200)
		{
			if (DotProduct(right, to_dest_dir) > 0.5 && VerifySequence(monster, &generic_move_reload_run_rt_ms, reject_actionflags))
			{
				return &generic_move_reload_run_rt_ms;
			}
			if (DotProduct(right, to_dest_dir) < -0.5 && VerifySequence(monster, &generic_move_reload_run_lt_ms, reject_actionflags))
			{
				return &generic_move_reload_run_lt_ms;
			}
			if (DotProduct(forward, to_dest_dir)<-0.5 && VerifySequence(monster, &generic_move_reload_run_bk_ms, reject_actionflags))
			{
				return &generic_move_reload_run_bk_ms;
			}
			if (VerifySequence(monster, &generic_move_reload_run_fwd_ms, reject_actionflags))
			{
				return &generic_move_reload_run_fwd_ms;
			}
		}
		//want to walk and reload
		if (to_dest_dist > 1)
		{
			if (DotProduct(right, to_dest_dir) > 0.5 && VerifySequence(monster, &generic_move_reload_walk_rt_ms, reject_actionflags))
			{
				return &generic_move_reload_walk_rt_ms;
			}
			if (DotProduct(right, to_dest_dir) < -0.5 && VerifySequence(monster, &generic_move_reload_walk_lt_ms, reject_actionflags))
			{
				return &generic_move_reload_walk_lt_ms;
			}
			if (DotProduct(forward, to_dest_dir)<-0.5 && VerifySequence(monster, &generic_move_reload_walk_bk_ms, reject_actionflags))
			{
				return &generic_move_reload_walk_bk_ms;
			}
			if (VerifySequence(monster, &generic_move_reload_walk_fwd_ms, reject_actionflags))
			{
				return &generic_move_reload_walk_fwd_ms;
			}
		}
		if (VerifySequence(monster, &generic_move_reload_ms, reject_actionflags))
		{
			return &generic_move_reload_ms;
		}
	}
	if (rightHandWeapVariety&GUNVARIETY_PISTOL)
	{
		if (goal_bbox==BBOX_PRESET_CROUCH)
		{
			//want to move and reload
			if (to_dest_dist > 1)
			{
				if (DotProduct(forward, to_dest_dir)<-0.5 && VerifySequence(monster, &generic_move_reload_crun_bk_p, reject_actionflags))
				{
					return &generic_move_reload_crun_bk_p;
				}
				if (VerifySequence(monster, &generic_move_reload_crun_fwd_p, reject_actionflags))
				{
					return &generic_move_reload_crun_fwd_p;
				}
			}
			if (VerifySequence(monster, &generic_move_reload_c_p, reject_actionflags))
			{
				return &generic_move_reload_c_p;
			}
		}
		//want to run and reload
		if (to_dest_dist > 200)
		{
			if (DotProduct(right, to_dest_dir) > 0.5 && VerifySequence(monster, &generic_move_reload_run_rt_p, reject_actionflags))
			{
				return &generic_move_reload_run_rt_p;
			}
			if (DotProduct(right, to_dest_dir) < -0.5 && VerifySequence(monster, &generic_move_reload_run_lt_p, reject_actionflags))
			{
				return &generic_move_reload_run_lt_p;
			}
			if (DotProduct(forward, to_dest_dir)<-0.5 && VerifySequence(monster, &generic_move_reload_run_bk_p, reject_actionflags))
			{
				return &generic_move_reload_run_bk_p;
			}
			if (VerifySequence(monster, &generic_move_reload_run_fwd_p, reject_actionflags))
			{
				return &generic_move_reload_run_fwd_p;
			}
		}
		//want to walk and reload
		if (to_dest_dist > 1)
		{
			if (DotProduct(right, to_dest_dir) > 0.5 && VerifySequence(monster, &generic_move_reload_walk_rt_p, reject_actionflags))
			{
				return &generic_move_reload_walk_rt_p;
			}
			if (DotProduct(right, to_dest_dir) < -0.5 && VerifySequence(monster, &generic_move_reload_walk_lt_p, reject_actionflags))
			{
				return &generic_move_reload_walk_lt_p;
			}
			if (DotProduct(forward, to_dest_dir)<-0.5 && VerifySequence(monster, &generic_move_reload_walk_bk_p, reject_actionflags))
			{
				return &generic_move_reload_walk_bk_p;
			}
			if (VerifySequence(monster, &generic_move_reload_walk_fwd_p, reject_actionflags))
			{
				return &generic_move_reload_walk_fwd_p;
			}
		}
		if (VerifySequence(monster, &generic_move_reload_p, reject_actionflags))
		{
			return &generic_move_reload_p;
		}
		if (VerifySequence(monster, &generic_move_reload_p1, reject_actionflags))
		{
			return &generic_move_reload_p1;
		}
	}
	if (rightHandWeapVariety&GUNVARIETY_RIFLE)
	{
		if (goal_bbox==BBOX_PRESET_CROUCH)
		{
			//want to move and reload
			if (to_dest_dist > 1)
			{
				if (DotProduct(forward, to_dest_dir)<-0.5 && VerifySequence(monster, &generic_move_reload_crun_bk_r, reject_actionflags))
				{
					return &generic_move_reload_crun_bk_r;
				}
				if (VerifySequence(monster, &generic_move_reload_crun_fwd_r, reject_actionflags))
				{
					return &generic_move_reload_crun_fwd_r;
				}
			}
			if (VerifySequence(monster, &generic_move_reload_c_r, reject_actionflags))
			{
				return &generic_move_reload_c_r;
			}
		}
		//want to run and reload
		if (to_dest_dist > 200)
		{
			if (DotProduct(right, to_dest_dir) > 0.5 && VerifySequence(monster, &generic_move_reload_run_rt_r, reject_actionflags))
			{
				return &generic_move_reload_run_rt_r;
			}
			if (DotProduct(right, to_dest_dir) < -0.5 && VerifySequence(monster, &generic_move_reload_run_lt_r, reject_actionflags))
			{
				return &generic_move_reload_run_lt_r;
			}
			if (DotProduct(forward, to_dest_dir)<-0.5 && VerifySequence(monster, &generic_move_reload_run_bk_r, reject_actionflags))
			{
				return &generic_move_reload_run_bk_r;
			}
			if (VerifySequence(monster, &generic_move_reload_run_fwd_r, reject_actionflags))
			{
				return &generic_move_reload_run_fwd_r;
			}
		}
		//want to walk and reload
		if (to_dest_dist > 1)
		{
			if (DotProduct(right, to_dest_dir) > 0.5 && VerifySequence(monster, &generic_move_reload_walk_rt_r, reject_actionflags))
			{
				return &generic_move_reload_walk_rt_r;
			}
			if (DotProduct(right, to_dest_dir) < -0.5 && VerifySequence(monster, &generic_move_reload_walk_lt_r, reject_actionflags))
			{
				return &generic_move_reload_walk_lt_r;
			}
			if (DotProduct(forward, to_dest_dir)<-0.5 && VerifySequence(monster, &generic_move_reload_walk_bk_r, reject_actionflags))
			{
				return &generic_move_reload_walk_bk_r;
			}
			if (VerifySequence(monster, &generic_move_reload_walk_fwd_r, reject_actionflags))
			{
				return &generic_move_reload_walk_fwd_r;
			}
		}
		if (VerifySequence(monster, &generic_move_reload_r, reject_actionflags))
		{
			return &generic_move_reload_r;
		}
	}
	if (rightHandWeapVariety&GUNVARIETY_LAUNCHER)
	{
		if (goal_bbox==BBOX_PRESET_CROUCH)
		{
			//want to move and reload
			if (to_dest_dist > 1)
			{
				if (DotProduct(forward, to_dest_dir)<-0.5 && VerifySequence(monster, &generic_move_reload_crun_bk_l, reject_actionflags))
				{
					return &generic_move_reload_crun_bk_l;
				}
				if (VerifySequence(monster, &generic_move_reload_crun_fwd_l, reject_actionflags))
				{
					return &generic_move_reload_crun_fwd_l;
				}
			}
			if (VerifySequence(monster, &generic_move_reload_c_l, reject_actionflags))
			{
				return &generic_move_reload_c_l;
			}
		}
		//want to run and reload
		if (to_dest_dist > 200)
		{
			if (DotProduct(right, to_dest_dir) > 0.5 && VerifySequence(monster, &generic_move_reload_run_rt_l, reject_actionflags))
			{
				return &generic_move_reload_run_rt_l;
			}
			if (DotProduct(right, to_dest_dir) < -0.5 && VerifySequence(monster, &generic_move_reload_run_lt_l, reject_actionflags))
			{
				return &generic_move_reload_run_lt_l;
			}
			if (DotProduct(forward, to_dest_dir)<-0.5 && VerifySequence(monster, &generic_move_reload_run_bk_l, reject_actionflags))
			{
				return &generic_move_reload_run_bk_l;
			}
			if (VerifySequence(monster, &generic_move_reload_run_fwd_l, reject_actionflags))
			{
				return &generic_move_reload_run_fwd_l;
			}
		}
		//want to walk and reload
		if (to_dest_dist > 1)
		{
			if (DotProduct(right, to_dest_dir) > 0.5 && VerifySequence(monster, &generic_move_reload_walk_rt_l, reject_actionflags))
			{
				return &generic_move_reload_walk_rt_l;
			}
			if (DotProduct(right, to_dest_dir) < -0.5 && VerifySequence(monster, &generic_move_reload_walk_lt_l, reject_actionflags))
			{
				return &generic_move_reload_walk_lt_l;
			}
			if (DotProduct(forward, to_dest_dir)<-0.5 && VerifySequence(monster, &generic_move_reload_walk_bk_l, reject_actionflags))
			{
				return &generic_move_reload_walk_bk_l;
			}
			if (VerifySequence(monster, &generic_move_reload_walk_fwd_l, reject_actionflags))
			{
				return &generic_move_reload_walk_fwd_l;
			}
		}
		if (VerifySequence(monster, &generic_move_reload_l, reject_actionflags))
		{
			return &generic_move_reload_l;
		}
	}
	return NULL;
}

mmove_t	*bodyhuman_c::GetSequenceForAttack(edict_t &monster, vec3_t dest, vec3_t face, edict_t *target, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);
	vec3_t to_dest;
	float	to_dest_dist;//heh. well, i was already normalizing the dest dir, so i'll reuse the result
	mmove_t	*auxmove;
	gunvariety_t	rightHandWeapVariety=GetGunVariety(GetRightHandWeapon(monster));

	qboolean rhandReady;

	if (ideal_move && VerifySequence(monster, ideal_move, reject_actionflags))
	{
		return ideal_move;
	}

	//if i'm disabled, i'm disabled. what can you do about something like that?
	if (auxmove=GetSequenceForDisability(monster))
	{
		return auxmove;
	}

	//if i'm not facing near my target, don't shoot
	vec3_t to_targ,my_facing;
	VectorSubtract(face, monster.s.origin, to_targ);
	VectorNormalize(to_targ);

	if (the_ai)
	{
		//the_ai->GetAimVector(my_facing);
		AngleVectors(monster.s.angles, my_facing, NULL, NULL);
	}
	else if (monster.client)
	{
		AngleVectors(monster.client->ps.viewangles, my_facing, NULL, NULL);
	}
	else
	{
		AngleVectors(monster.s.angles, my_facing, NULL, NULL);
	}

	if (bodyDamage & (BODYDAMAGE_ANY))
	{
		//if i need to turn, don't bother aiming--limp around
		if (DotProduct(to_targ, my_facing)<0.85)
		{
			if (bodyDamageAccumulated & (BODYDAMAGE_RIGHTLEG))
			{
				if (VerifySequence(monster, &generic_move_limp_lleg, reject_actionflags))
				{
					return &generic_move_limp_lleg;
				}
			}
			if (bodyDamageAccumulated & (BODYDAMAGE_LEFTLEG))
			{
				if (VerifySequence(monster, &generic_move_limp_rleg, reject_actionflags))
				{
					return &generic_move_limp_rleg;
				}
			}
		}
		if (GetBestWeapon(monster)==ATK_NOTHING)
		{
			if (auxmove=GetSequenceForTransition(monster, dest, face, seqtype, BBOX_PRESET_CROUCH, reject_actionflags))
			{
				return auxmove;
			}
			if (currentmove && (currentmove==&generic_move_crouch_cower_mid2 || &generic_move_crouch_cower_mid) &&
				!IsAnimationFinished())
			{
				return currentmove;
			}
			if((gi.irand(0,2)||!VerifySequence(monster, &generic_move_crouch_cower_mid2, reject_actionflags))&&
				VerifySequence(monster, &generic_move_crouch_cower_mid, reject_actionflags))
			{
				return &generic_move_crouch_cower_mid;
			}
			if(VerifySequence(monster, &generic_move_crouch_cower_mid2, reject_actionflags))
			{
				return &generic_move_crouch_cower_mid2;
			}
		}
		// This is being removed because it looks awful and also not smart when behind the man
/*		if (GetRightHandWeapon(monster)!=ATK_NOTHING&&IsAvailableSequence(monster, &generic_move_hurt_atk))
		{
			return &generic_move_hurt_atk;
		}*/
	}

	if (auxmove=GetSequenceForTransition(monster, dest, face, seqtype, goal_bbox, reject_actionflags))
	{
		return auxmove;
	}

	//set whether my right hand gun is ready
	rhandReady = RightHandWeaponReady(monster);
	if (DotProduct(to_targ, my_facing)<MAX_FIRE_WEDGE)//fixme - does this vary according to the bbox?
	{
		rhandReady=false;
	}

	//if i need to turn a lot, do a shuffle--eek! if face and dest are the same, this is WASTEFUL!
	vec3_t to_dest_dir, forward, right;
	VectorSubtract(dest, monster.s.origin, to_dest_dir);

	//pay no heed to up/down when it comes to shuffle! BUT BEWARE! this isn't really to my dest dir anymore!
	//also, not real distance to dest either!
	to_dest_dir[2]=0;

	to_dest_dist=VectorNormalize(to_dest_dir);
	AngleVectors(monster.s.angles, forward, right, NULL);

	if (currentmove&&(currentmove->bbox==BBOX_PRESET_PRONE ||currentmove->bbox==BBOX_PRESET_ALLFOURS))
	{
		if(VerifySequence(monster, &generic_move_prfire_mrs, reject_actionflags))
		{
			return &generic_move_prfire_mrs;
		}
	}

	if(currentmove&&currentmove->bbox == BBOX_PRESET_LAYSIDE)
	{
		return currentmove;
	}

	if(currentmove&&currentmove->bbox == BBOX_PRESET_LAYBACK)
	{
		if (auxmove=GetSequenceForTransition(monster, dest, face, seqtype, BBOX_PRESET_STAND, reject_actionflags))
		{
			return auxmove;
		}

		if (VerifySequence(monster, &generic_move_layback_die_shoot, reject_actionflags))
		{
			return &generic_move_layback_die_shoot;
		}
		//er, try scooching?
		if (VerifySequence(monster, &generic_move_layback_scooch, reject_actionflags))
		{
			if (currentmove != &generic_move_layback_scooch)
			{
				VoiceGeneralSound("Writhe",monster,WRITHENUMBER);
			}
			return &generic_move_layback_scooch;
		}

		//forget it--count me dead
		if (VerifySequence(monster, &generic_move_death_lbshotdeath, reject_actionflags))
		{
			return &generic_move_death_lbshotdeath;
		}

		return currentmove;//ah! yikes!
	}

	if(goal_bbox == BBOX_PRESET_STAND)
	{
		if (currentmove&&currentmove->bbox==BBOX_PRESET_CROUCH)
		{
			//yikes! i'm trapped down here!
			if(rhandReady)
			{
				if(rightHandWeapVariety&GUNVARIETY_MACHINE&&VerifySequence(monster, &generic_move_cfire_m, reject_actionflags))
				{
					return &generic_move_cfire_m;
				}
				if(rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_cfire_r, reject_actionflags))
				{
					return &generic_move_cfire_r;
				}
				if(rightHandWeapVariety&GUNVARIETY_SHOTGUN&&VerifySequence(monster, &generic_move_cfire_s, reject_actionflags))
				{
					return &generic_move_cfire_s;
				}
				if (rightHandWeapVariety&GUNVARIETY_PNONAUTO&&VerifySequence(monster, &generic_move_duck_shoot, reject_actionflags))
				{
					return &generic_move_duck_shoot;
				}
				if (rightHandWeapVariety&GUNVARIETY_PAUTO&&VerifySequence(monster, &generic_move_caim_p_auto, reject_actionflags))
				{
					return &generic_move_caim_p_auto;
				}
				if((rightHandWeapVariety&GUNVARIETY_LAUNCHER) && VerifySequence(monster, &generic_move_crouch_launcher_fire, reject_actionflags))
				{
					return &generic_move_crouch_launcher_fire;
				}
			}

			if(rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_caim_r, reject_actionflags))
			{
				return &generic_move_caim_r;
			}
			if(rightHandWeapVariety&GUNVARIETY_MS&&VerifySequence(monster, &generic_move_caim_ms, reject_actionflags))
			{
				return &generic_move_caim_ms;
			}
			if((rightHandWeapVariety&GUNVARIETY_LAUNCHER) && VerifySequence(monster, &generic_move_caim_l, reject_actionflags))
			{
				return &generic_move_caim_l;
			}
			if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_chaim2, reject_actionflags))
			{
				return &generic_move_chaim2;
			}
		}

		VectorSubtract(dest, monster.s.origin, to_dest);

		float aimCone;
		float tempAng;
		if (the_ai)
		{
			tempAng = anglemod(the_ai->ideal_angles[YAW]-monster.s.angles[YAW]);
			if (currentmove)
			{
				aimCone = the_ai->GetAimConeDegrees(monster, currentmove->bbox);
			}
			else
			{
				aimCone = the_ai->GetAimConeDegrees(monster, BBOX_PRESET_STAND);
			}
		}
		else
		{
			tempAng = 0;//uh??
			aimCone = 10;//eh?
		}

		//i'm outside the aim cone...use ideal_angles for aiming
		if (tempAng > aimCone && tempAng < 360.0-aimCone)
		{
			if (rightHandWeapVariety&GUNVARIETY_MS&&VerifySequence(monster, &generic_move_ashuffle_ms, reject_actionflags))
			{
				return &generic_move_ashuffle_ms;
			}
			if (rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_ashuffle_r, reject_actionflags))
			{
				return &generic_move_ashuffle_r;
			}
			if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_ashuffle_p2, reject_actionflags))
			{
				return &generic_move_ashuffle_p2;
			}
			if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_ashuffle_pcb, reject_actionflags))
			{
				return &generic_move_ashuffle_pcb;
			}
			if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_ashuffle_l, reject_actionflags))
			{
				return &generic_move_ashuffle_l;
			}
		}

		//wanna run..
		//fixme: if i'm already running, be more lenient here
		if ((to_dest_dist>20)&&(GetArmorCode()!=ARMOR_FULL))
		{
			if (auxmove = GetSequenceForRunAimStrafe(monster, to_dest_dir, forward, right, dest, face, seqtype, goal_bbox, ideal_move, reject_actionflags))
			{
				return auxmove;
			}
			if (auxmove = GetSequenceForWalkAimStrafe(monster, to_dest_dir, forward, right, dest, face, seqtype, goal_bbox, ideal_move, reject_actionflags))
			{
				return auxmove;
			}
			//run forward...
			if (DotProduct(to_dest_dir, forward)>0)
			{
//				if (rightHandWeapVariety&GUNVARIETY_PISTOL && VerifySequence(monster, &generic_move_raim_p1, reject_actionflags))
//				{
//					return &generic_move_raim_p1;
//				}

				if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_raim_p2, reject_actionflags))
				{
					return &generic_move_raim_p2;
				}
/*				if (rightHandWeapVariety&GUNVARIETY_PK&&VerifySequence(monster, &generic_move_raim_p1, reject_actionflags))
				{
					return &generic_move_raim_p1;
				}
*/				if (rightHandWeapVariety&GUNVARIETY_KNIFE&&VerifySequence(monster, &generic_move_playerstrunswipe, reject_actionflags))
				{
					return &generic_move_playerstrunswipe;
				}
				if (rightHandWeapVariety&GUNVARIETY_MRS&&VerifySequence(monster, &generic_move_raim_mrs, reject_actionflags))
				{
					return &generic_move_raim_mrs;
				}
				if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_raim_l, reject_actionflags))
				{
					return &generic_move_raim_l;
				}
			}
			//run back...
			else
			{
				if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_rbackaim_p2, reject_actionflags))
				{
					if (currentmove && currentmove->suggested_action != ACTCODE_PAIN)
					{
//						VoiceSound("backup", monster, 0);
					}
					return &generic_move_rbackaim_p2;
				}
				if (rightHandWeapVariety&GUNVARIETY_MRS&&VerifySequence(monster, &generic_move_rbackaim_mrs, reject_actionflags))
				{
					if (currentmove && currentmove->suggested_action != ACTCODE_PAIN)
					{
//						VoiceSound("backup", monster, 0);
					}
					return &generic_move_rbackaim_mrs;
				}
				if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_rbackaim_l, reject_actionflags))
				{
					if (currentmove && currentmove->suggested_action != ACTCODE_PAIN)
					{
//						VoiceSound("backup", monster, 0);
					}
					return &generic_move_rbackaim_l;
				}
			}
		}

		// er, is this a good spot to check for grenadeness
		if(UseLeftHandAttack(monster, target))
		{
			if(VerifySequence(monster, &generic_move_throw_gren, reject_actionflags))
			{
				if (currentmove && currentmove->suggested_action != ACTCODE_PAIN)
				{
//					VoiceSound("throw", monster, 0);
				}
				return &generic_move_throw_gren;
			}
			if(VerifySequence(monster, &generic_move_throwstar, reject_actionflags))
			{
				if (currentmove && currentmove->suggested_action != ACTCODE_PAIN)
				{
//					VoiceSound("throw", monster, 0);
				}
				return &generic_move_throwstar;
			}
		}


		//wanna walk..
		//fixme: if i'm already walking, be more lenient here
		if (to_dest_dist>10||(currentmove && currentmove->bodyPosition==BODYPOS_AIMWALK && to_dest_dist>5))
		{
			//wanna strafewalk...
			if (auxmove = GetSequenceForWalkAimStrafe(monster, to_dest_dir, forward, right, dest, face, seqtype, goal_bbox, ideal_move, reject_actionflags))
			{
				return auxmove;
			}

			//walk forward...
			if (DotProduct(to_dest_dir, forward)>0)
			{
				if (rightHandWeapVariety&GUNVARIETY_PK&&VerifySequence(monster, &generic_move_waim_p, reject_actionflags))
				{
					return &generic_move_waim_p;
				}
				if (rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_waim_r, reject_actionflags))
				{
					return &generic_move_waim_r;
				}
				if (rightHandWeapVariety&GUNVARIETY_MS&&VerifySequence(monster, &generic_move_waim_ms, reject_actionflags))
				{
					return &generic_move_waim_ms;
				}
				if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_waim_l, reject_actionflags))
				{
					return &generic_move_waim_l;
				}
			}
			//walk back...
			else
			{
				if (rightHandWeapVariety&GUNVARIETY_PK&&VerifySequence(monster, &generic_move_wbackaim_p, reject_actionflags))
				{
					if (currentmove && currentmove->suggested_action != ACTCODE_PAIN)
					{
//						VoiceSound("backup", monster, 0);
					}
					return &generic_move_wbackaim_p;
				}
				if (rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_wbackaim_r, reject_actionflags))
				{
					if (currentmove && currentmove->suggested_action != ACTCODE_PAIN)
					{
//						VoiceSound("backup", monster, 0);
					}
					return &generic_move_wbackaim_r;
				}
				if (rightHandWeapVariety&GUNVARIETY_MS&&VerifySequence(monster, &generic_move_wbackaim_ms, reject_actionflags))
				{
					if (currentmove && currentmove->suggested_action != ACTCODE_PAIN)
					{
//						VoiceSound("backup", monster, 0);
					}
					return &generic_move_wbackaim_ms;
				}
				if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_wbackaim_l, reject_actionflags))
				{
					if (currentmove && currentmove->suggested_action != ACTCODE_PAIN)
					{
//						VoiceSound("backup", monster, 0);
					}
					return &generic_move_wbackaim_l;
				}
			}
		}

		//if i've got a knife, make sure i slash...
		if (rightHandWeapVariety&GUNVARIETY_KNIFE&&rhandReady && VerifySequence(monster, &generic_move_slash_kp, reject_actionflags))
		{
			return &generic_move_slash_kp;
		}

		//aiming up--use as aim and attack
		if (to_targ[2]>0.25)
		{
			if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staimup_p, reject_actionflags))
			{
				return &generic_move_staimup_p;
			}
			if (rightHandWeapVariety&GUNVARIETY_MRS&&VerifySequence(monster, &generic_move_staimup_mrs, reject_actionflags))
			{
				return &generic_move_staimup_mrs;
			}
			if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_staimup_l, reject_actionflags))
			{
				return &generic_move_staimup_l;
			}
		}
		//aiming down
		if (to_targ[2]<-0.25)
		{
			if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staimdn_p, reject_actionflags))
			{
				return &generic_move_staimdn_p;
			}
			if (rightHandWeapVariety&GUNVARIETY_MRS&&VerifySequence(monster, &generic_move_staimdn_mrs, reject_actionflags))
			{
				return &generic_move_staimdn_mrs;
			}
			if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_staimdn_l, reject_actionflags))
			{
				return &generic_move_staimdn_l;
			}
		}

		if (rightHandWeapVariety&GUNVARIETY_PISTOL&&rhandReady && (!gi.irand(0,5)) && VerifySequence(monster, &generic_move_attack_attitude, reject_actionflags))
		{
			return &generic_move_attack_attitude;
		}
		if (rightHandWeapVariety&GUNVARIETY_PNONAUTO&&rhandReady && VerifySequence(monster, &generic_move_attack, reject_actionflags))
		{
			return &generic_move_attack;
		}
		if (rightHandWeapVariety&GUNVARIETY_PNONAUTO&&rhandReady && VerifySequence(monster, &generic_move_attackb, reject_actionflags))
		{
			return &generic_move_attackb;
		}
		if (rightHandWeapVariety&GUNVARIETY_KNIFE&&rhandReady && VerifySequence(monster, &generic_move_slash_kp, reject_actionflags))
		{
			return &generic_move_slash_kp;
		}
		if (rightHandWeapVariety&GUNVARIETY_PNONAUTO&&rhandReady && VerifySequence(monster, &generic_move_firecb, reject_actionflags))
		{
			return &generic_move_firecb;
		}
		if (rightHandWeapVariety&GUNVARIETY_PAUTO&&VerifySequence(monster, &generic_move_caim_p_auto, reject_actionflags))
		{
			return &generic_move_staim_p_auto;
		}
		if (rightHandWeapVariety&GUNVARIETY_MACHINE&&rhandReady && (GetRightHandWeapon(monster)==ATK_FLAMEGUN || (!gi.irand(0,2)||(gi.irand(0,5)
			&&currentmove==&generic_move_atksweep_mgun))) && VerifySequence(monster, &generic_move_atksweep_mgun, reject_actionflags))
		{	//flamethrower guys need to always use this one
			return &generic_move_atksweep_mgun;
		}
		if (rightHandWeapVariety&GUNVARIETY_MACHINE&&rhandReady && VerifySequence(monster, &generic_move_attack_mgun, reject_actionflags))
		{
			return &generic_move_attack_mgun;
		}
		if (rightHandWeapVariety&GUNVARIETY_RIFLE&&rhandReady && VerifySequence(monster, &generic_move_attack_rif, reject_actionflags))
		{
			return &generic_move_attack_rif;
		}
		if (rightHandWeapVariety&GUNVARIETY_SHOTGUN&&rhandReady && VerifySequence(monster, &generic_move_attack_sgun, reject_actionflags))
		{
			return &generic_move_attack_sgun;
		}
		if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&rhandReady && VerifySequence(monster, &generic_move_attack_l, reject_actionflags))
		{
			return &generic_move_attack_l;
		}
	}
	if (currentmove&&currentmove->bbox==BBOX_PRESET_CROUCH)
	{
		// ??
		if(UseLeftHandAttack(monster, target))
		{
			if(VerifySequence(monster, &generic_move_lob_grenade_crouch, reject_actionflags))
			{
				return &generic_move_lob_grenade_crouch;
			}
		}

		if (to_dest_dist>10||(currentmove && currentmove->bodyPosition==BODYPOS_AIMWALK && to_dest_dist>5))
		{
			//going forward
			if (DotProduct(to_dest_dir, forward)>0)
			{
				if (rightHandWeapVariety&GUNVARIETY_KNIFE&&VerifySequence(monster, &generic_move_playercrunswipe, reject_actionflags))
				{
					return &generic_move_playercrunswipe;
				}
			}
			//going backward
			else
			{
				if (rightHandWeapVariety&GUNVARIETY_KNIFE&&VerifySequence(monster, &generic_move_playercrunswipe, reject_actionflags))
				{
					return &generic_move_playercrunswipe;
				}
			}
		}

		if(rhandReady)
		{
			if(rightHandWeapVariety&GUNVARIETY_MACHINE&&VerifySequence(monster, &generic_move_cfire_m, reject_actionflags))
			{
				return &generic_move_cfire_m;
			}
			if(rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_cfire_r, reject_actionflags))
			{
				return &generic_move_cfire_r;
			}
			if(rightHandWeapVariety&GUNVARIETY_SHOTGUN&&VerifySequence(monster, &generic_move_cfire_s, reject_actionflags))
			{
				return &generic_move_cfire_s;
			}
			if (rightHandWeapVariety&GUNVARIETY_PNONAUTO&&VerifySequence(monster, &generic_move_duck_shoot, reject_actionflags))
			{
				return &generic_move_duck_shoot;
			}
			if (rightHandWeapVariety&GUNVARIETY_PAUTO&&VerifySequence(monster, &generic_move_caim_p_auto, reject_actionflags))
			{
				return &generic_move_caim_p_auto;
			}
			if((rightHandWeapVariety&GUNVARIETY_LAUNCHER) && VerifySequence(monster, &generic_move_crouch_launcher_fire, reject_actionflags))
			{
				return &generic_move_crouch_launcher_fire;
			}
		}
		if(rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_caim_r, reject_actionflags))
		{
			return &generic_move_caim_r;
		}
		if(rightHandWeapVariety&GUNVARIETY_MS&&VerifySequence(monster, &generic_move_caim_ms, reject_actionflags))
		{
			return &generic_move_caim_ms;
		}
		if((rightHandWeapVariety&GUNVARIETY_LAUNCHER) && VerifySequence(monster, &generic_move_caim_l, reject_actionflags))
		{
			return &generic_move_caim_l;
		}
		if (rightHandWeapVariety&GUNVARIETY_PISTOL && IsAvailableSequence(monster, &generic_move_duck_shoot) && 
			VerifySequence(monster, &generic_move_chaim2, reject_actionflags))
		{
			return &generic_move_chaim2;
		}
	}
	//fixme: this check is just plain silly; also, the animation is silly (in most sits, anyhow)
//	if (!VectorCompare(face, vec3_origin) && DotProduct (forward, to_targ)<0.95)
//	{
//		if (rightHandWeapVariety&GUNVARIETY_MS&&VerifySequence(monster, &generic_move_ashuffle_ms, reject_actionflags))
//		{
//			return &generic_move_ashuffle_ms;
//		}
//		if (rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_ashuffle_r, reject_actionflags))
//		{
//			return &generic_move_ashuffle_r;
//		}
//		if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_ashuffle_p2, reject_actionflags))
//		{
//			return &generic_move_ashuffle_p2;
//		}
//		if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_ashuffle_pcb, reject_actionflags))
//		{
//			return &generic_move_ashuffle_pcb;
//		}
//	}


	//aiming up
	if (to_targ[2]>0.25)
	{
		if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staimup_p, reject_actionflags))
		{
			return &generic_move_staimup_p;
		}
		if (rightHandWeapVariety&GUNVARIETY_MRS&&VerifySequence(monster, &generic_move_staimup_mrs, reject_actionflags))
		{
			return &generic_move_staimup_mrs;
		}
		if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_staimup_l, reject_actionflags))
		{
			return &generic_move_staimup_l;
		}
	}
	//aiming down
	if (to_targ[2]<-0.25)
	{
		if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staimdn_p, reject_actionflags))
		{
			return &generic_move_staimdn_p;
		}
		if (rightHandWeapVariety&GUNVARIETY_MRS&&VerifySequence(monster, &generic_move_staimdn_mrs, reject_actionflags))
		{
			return &generic_move_staimdn_mrs;
		}
		if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_staimdn_l, reject_actionflags))
		{
			return &generic_move_staimdn_l;
		}
	}

	//fixme--check which stance i was in--if firing 2-handed, f'rinstance, aim 2-handed
	if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staim_p2, reject_actionflags))
	{
		return &generic_move_staim_p2;
	}
	if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staim_pcb, reject_actionflags))
	{
		return &generic_move_staim_pcb;
	}
	if (rightHandWeapVariety&GUNVARIETY_MS&&VerifySequence(monster, &generic_move_staim_ms, reject_actionflags))
	{
		return &generic_move_staim_ms;
	}
	if (rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_staim_r, reject_actionflags))
	{
		return &generic_move_staim_r;
	}
	if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_staim_l, reject_actionflags))
	{
		return &generic_move_staim_l;
	}

	//ack! shouldn't get here!
	if (rightHandWeapVariety&GUNVARIETY_MRS)
	{
		if((rand()%15) && VerifySequence(monster, &generic_move_atkpause1_mrs, reject_actionflags))
		{
			return &generic_move_atkpause1_mrs;
		}
		if(VerifySequence(monster, &generic_move_atkpause2_mrs, reject_actionflags))
		{
			return &generic_move_atkpause2_mrs;
		}
		if (rightHandWeapVariety&GUNVARIETY_MS&&VerifySequence(monster, &generic_move_staim_ms, reject_actionflags))
		{
			return &generic_move_staim_ms;
		}
		if (rightHandWeapVariety&GUNVARIETY_RIFLE&&VerifySequence(monster, &generic_move_staim_r, reject_actionflags))
		{
			return &generic_move_staim_r;
		}
		if(VerifySequence(monster, &generic_move_alert_mrs, reject_actionflags))
		{
			return &generic_move_alert_mrs;
		}
	}
	else if (rightHandWeapVariety==GUNVARIETY_NONE)
	{
		if(emotion == EMOTION_AFRAID && VerifySequence(monster, &generic_move_fear_wave, reject_actionflags) && !gi.irand(0,10))
		{
			return &generic_move_fear_wave;
		}
		if(emotion == EMOTION_AFRAID && VerifySequence(monster, &generic_move_cower, reject_actionflags))
		{
			return &generic_move_cower;
		}
	}

	if((rand()%15) && VerifySequence(monster, &generic_move_atkpause1_p, reject_actionflags))
	{
		return &generic_move_atkpause1_p;
	}
	if(VerifySequence(monster, &generic_move_atkpause2_p, reject_actionflags))
	{
		return &generic_move_atkpause2_p;
	}
	if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staim_p2, reject_actionflags))
	{
		return &generic_move_staim_p2;
	}
	if (rightHandWeapVariety&GUNVARIETY_PISTOL&&VerifySequence(monster, &generic_move_staim_pcb, reject_actionflags))
	{
		return &generic_move_staim_pcb;
	}
	if (rightHandWeapVariety&GUNVARIETY_LAUNCHER&&VerifySequence(monster, &generic_move_staim_l, reject_actionflags))
	{
		return &generic_move_staim_l;
	}
	if(VerifySequence(monster, &generic_move_alert_p, reject_actionflags))
	{
		return &generic_move_alert_p;
	}

	if (GetBestWeapon(monster)!=ATK_NOTHING)
	{
		if(VerifySequence(monster, &generic_move_stand_mrs, reject_actionflags))
		{
			return &generic_move_stand_mrs;
		}
	}
	if(VerifySequence(monster, &generic_move_stand, reject_actionflags))
	{
		return &generic_move_stand;
	}
	if(VerifySequence(monster, &generic_move_alert_p, reject_actionflags))
	{
		return &generic_move_alert_p;
	}
	return &generic_move_stand;
}

mmove_t	*bodyhuman_c::GetSequenceForTrainDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t		*animation = NULL;
	
	if (currentmove!=&generic_move_tumble_front)
	{
		VoiceGeneralSound("Scream", monster, SCREAMNUMBER); 
	}
	FinalAnim = true;
	FinalSound = true;
	
	if(IsAvailableSequence(monster, &generic_move_tumble_front))
	{
		animation = &generic_move_tumble_front;
	}
	return animation;
}

mmove_t	*bodyhuman_c::GetSequenceForParentalLockDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t		*animation = NULL;

	// FIXME:  until it is decided how to handle the deathanim lock, I'll just fudge some crap in here
	if(currentmove && currentmove->suggested_action==ACTCODE_DEATH)
	{
		// fixme.  This currently has an annoying side effect.  If you happen to turn on the lock_deaths cvar
		// WHILE a guy is in a looping death anim, he'll continue to do it forever.
		return currentmove;
	}

	if (currentmove && currentmove->bbox==BBOX_PRESET_SIT && VerifySequence(monster, &generic_move_death_lbshotdeath, reject_actionflags))
	{
		animation = &generic_move_death_lbshotdeath;
		FinalSound = true;
	}
	else if(VerifySequence(monster, &generic_move_death_fallforward, reject_actionflags))
	{
		VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
		animation = &generic_move_death_fallforward;
		FinalSound = true;
	}

	return animation;
}


mmove_t	*bodyhuman_c::GetSequenceForDeathFromFront(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, vec3_t to_dam, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t		*animation = NULL;


	switch(gi.irand(0,2))
	{

	case 0:
	default:
		if (IsAvailableSequence(monster, &generic_move_death_chestfront))
		{
			VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
			animation = &generic_move_death_chestfront;
			FinalSound = true;
		}
		break;
	case 1:
		if (IsAvailableSequence(monster, &generic_move_death_violent))
		{
			VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
			animation = &generic_move_death_violent;
			FinalSound = true;
		}
	case 2:
//	if ((monster.health < -30) && (!gi.irand(0,2)) && VerifySequence(monster, &generic_move_deaththrown, reject_actionflags))
// *** FIXME ***
		if (IsAvailableSequence(monster, &generic_move_deaththrown))
		{
			VoiceGeneralSound("Scream", monster, SCREAMNUMBER);

//		VectorScale(to_dam, -200, to_dam);
//		VectorAdd(monster.velocity, to_dam, monster.velocity);
//		monster.velocity[2]=50;//people have complained about the height of this at times...
//		monster.maxs[2]-=26;
//		monster.s.origin[2]+=26;
			animation = &generic_move_deaththrown;
			FinalSound = true;
			FinalAnim = true;
		}

	break;
	}

	return animation;
}

mmove_t	*bodyhuman_c::GetSequenceForDeathFromBack(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, vec3_t to_dam, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t		*animation = NULL;

	switch(gi.irand(0,0))
	{
	case 0:
	default:
		if (IsAvailableSequence(monster, &generic_move_death_sinback_tofront))
		{
			VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
			animation = &generic_move_death_sinback_tofront;
			FinalSound = true;
			FinalAnim = true;
		}
		break;
	}
	return animation;
}

mmove_t	*bodyhuman_c::GetSequenceForFacingBasedDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{

	vec3_t		forward, to_shooter;
	mmove_t		*animation = NULL;

	AngleVectors(monster.s.angles, forward, NULL, NULL);

	if (inflictor)
	{
		VectorSubtract(inflictor->s.origin, monster.s.origin, to_shooter);
		VectorNormalize(to_shooter);
	}
	else
	{
		VectorCopy(vec3_up, to_shooter);
	}

	if (DotProduct(to_shooter, forward) >= 0)
	{ // fixme not working!
		animation = GetSequenceForDeathFromFront(monster, inflictor, attacker, damage, point, dflags, to_shooter, goal_bbox, ideal_move);
	}
	else
	{
		animation = GetSequenceForDeathFromBack(monster, inflictor, attacker, damage, point, dflags, to_shooter, goal_bbox, ideal_move);
	}
	return animation;
}

mmove_t	*bodyhuman_c::GetSequenceForCatchAllDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t		*animation = NULL;

	switch (gi.irand(0,0))
	{ // random deaths with no particular code support
	default:
	case 0:
		VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
		if (IsAvailableSequence(monster, &generic_move_death_long))
		{
			animation = &generic_move_death_long;
			FinalSound = true;
			FinalAnim = true;
		}
		break;
	}
	return animation;
}

mmove_t	*bodyhuman_c::GetSequenceForGoreZoneDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t		*animation = NULL;

	if (gzones[spout].blowme->area == GBLOWNAREA_NECK)
	{
		if (IsAvailableSequence(monster, &generic_move_deadthroat))
		{
			GhoulID tempBolt;

			tempBolt=monster.ghoulInst->GetGhoulObject()->FindPart("sbolt_mouth");
			if (tempBolt)
			{
//				gi.sound (&monster, CHAN_BODY, gi.soundindex ("impact/gore/spout.wav"), .8, ATTN_NORM, 0);
				FX_Bleed(&monster, tempBolt, 100); 
			}
			VoiceGeneralSound("Choke", monster, 2);
			FinalSound = true;
			animation = &generic_move_deadthroat;

			if (!game.cinematicfreeze)
			{
				level.throatShots++;
			}
		}
		else
		{
			gi.dprintf("eek. That hurt my neck.\n");
		}
	}
	else if (gzones[spout].blowme->area == GBLOWNAREA_HEAD)
	{
		if (IsAvailableSequence(monster, &generic_move_death_fallforward))
		{
			animation = &generic_move_death_fallforward; // use for head shot
			FinalSound = true;
			if (!game.cinematicfreeze)
			{
				level.headShots++;
			}
		}
	}
	else if (gzones[spout].blowme->area == GBLOWNAREA_LFOOT)
	{
		if (IsAvailableSequence(monster, &generic_move_death_legleft))
		{
			VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
			animation = &generic_move_death_legleft;
		}
		else
		{
			gi.dprintf("eek. That hurt my left foot.\n");
		}
	}
	else if (gzones[spout].blowme->area == GBLOWNAREA_RFOOT)
	{
		if (IsAvailableSequence(monster, &generic_move_death_legright))
		{
			VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
			animation =  &generic_move_death_legright;
		}
		 else
		{
			gi.dprintf("eek. That hurt my right foot.\n");
		}
	}
	else if (gzones[spout].blowme->area == GBLOWNAREA_GROIN)//ick.
	{
		if (IsAvailableSequence(monster, &generic_move_deadgroin))
		{
			VoiceGeneralSound("Crotch", monster, 2);
			animation = &generic_move_deadgroin;
			if (!game.cinematicfreeze)
			{
				level.nutShots++;
			}
		}
		else
		{
			gi.dprintf("eek. That hurt my groin.\n");
		}
	}
	else if (gzones[spout].blowme->area == GBLOWNAREA_RTHIGH || gzones[spout].blowme->area == GBLOWNAREA_RCALF)
	{
		if (IsAvailableSequence(monster, &generic_move_death_legright))
		{
			VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
			animation = &generic_move_death_legright;
		}
		 else
		{
			gi.dprintf("Couldn't find right leg death.\n");
		}
	}
	else if (gzones[spout].blowme->area == GBLOWNAREA_LTHIGH || gzones[spout].blowme->area == GBLOWNAREA_LCALF)
	{
		if (IsAvailableSequence(monster, &generic_move_death_legleft))
		{
			VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
			animation = &generic_move_death_legleft;
		}
		else
		{
			gi.dprintf("Couldn't find left leg death.\n");
		}
	}
//	else if (gzones[spout].childzone == GZ_SHLDR_LEFT_FRONT)//yick. this should really be right front shoulder
	else if (gzones[spout].blowme->area == GBLOWNAREA_RFSHLD || gzones[spout].blowme->area == GBLOWNAREA_LBSHLD)
	{
		if (IsAvailableSequence(monster, &generic_move_death_shldrright))
		{
			VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
			animation = &generic_move_death_shldrright;
		}
		else
		{
			gi.dprintf("Couldn't find right shoulder death.\n");
		}
	}
//	else if (gzones[spout].childzone == GZ_SHLDR_RIGHT_BACK)//yick. this should really be left front shoulder
	else if (gzones[spout].blowme->area == GBLOWNAREA_RBSHLD || gzones[spout].blowme->area == GBLOWNAREA_LFSHLD)
	{
		if (IsAvailableSequence(monster, &generic_move_death_shldrleft))
		{
			VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
			animation = &generic_move_death_shldrleft;
		}
		else
		{
			gi.dprintf("Couldn't find left shoulder death.\n");
		}
	}
	else if (spout == GZ_GUT_FRONT)
	{
		if (IsAvailableSequence(monster, &generic_move_death_gut_toside))
		{
			VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
			animation = &generic_move_death_gut_toside;
		}
		else
		{
			gi.dprintf("Couldn't find gut death.\n");
		}
	}
	return animation;
}

mmove_t	*bodyhuman_c::GetSequenceForRunningDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t		*animation = NULL;
	float		rnum = gi.flrand(0,1);

	if (rnum < .5)
	{
		if (IsAvailableSequence(monster, &generic_move_death_tumble_toback))
		{
			animation = &generic_move_death_tumble_toback;
		}
	}
	else
	{
		if (IsAvailableSequence(monster, &generic_move_death_stumble_2frnt))
		{
			animation = &generic_move_death_stumble_2frnt;
		}
	}	
	return animation;
}

mmove_t	*bodyhuman_c::GetSequenceForJuggleDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t		*animation = NULL;

	if (IsAvailableSequence(monster, &generic_move_death_dance))
	{
		animation = &generic_move_death_dance;
	}

	return animation;
}

mmove_t	*bodyhuman_c::GetSequenceForBurningDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t		*animation = NULL;

	static float	deathtime=0;

	// Hey!  One problem we have is that if there are several burning deaths at once, you get the "disco fever dance club",
	//		where everyone flails in perfect time.  This is not super-pretty, but a fair way of making sure we don't
	//		always use a single sequence for everyone's death.
	//		Note that the second check stems from the fact that we don't reset the counter on level load (bad also, I know).
	if (level.time - deathtime <= 0.1 && level.time - deathtime >= -0.5)
	{
		animation = GetSequenceForFacingBasedDeath(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move);
	}
	else
	{	// Die as normal.
		deathtime = level.time;
		
		if(IsAvailableSequence(monster, &generic_move_death_onfire))
		{
			animation = &generic_move_death_onfire;
			VoiceGeneralSound("Burn", monster, 1);
			FinalSound = true;
		}
	}
	return animation;
}

mmove_t	*bodyhuman_c::GetSequenceForPulseGunDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t		*animation = NULL;

	if((dflags & DT_SHOCK)&&(IsAvailableSequence(monster, &generic_move_death_pulse_gun)))
	{
		animation = &generic_move_death_pulse_gun;
		VoiceGeneralSound("Choke", monster, 2);
		FinalSound = true;
		fxRunner.exec("environ/shockdeath", &monster, 0);
	}
	else if(IsAvailableSequence(monster, &generic_move_microwave_death))
	{
		animation = &generic_move_microwave_death;
		VoiceGeneralSound("Choke", monster, 2);
		FinalSound = true;
		fxRunner.exec("environ/pulsecook", &monster, 0);
		gi.sound(&monster, CHAN_BODY, gi.soundindex("Weapons/MPG/Expand.wav"), .6, ATTN_NORM, 0);
	}

	if (monster.ghoulInst)
	{	// Set the death a random amount slower.  
		// This is to avoid a complete recompile of all the wads, which would be bad at this late juncture.
		// This also is to avoid the disco-inferno sychronized deaths.
		switch(gi.irand(0, 4))
		{
		case 1:
			monster.ghoulInst->SetSpeed(gs12Over8);
			break;
		case 2:
			monster.ghoulInst->SetSpeed(gs13Over8);
			break;
		case 3:
			monster.ghoulInst->SetSpeed(gs14Over8);
			break;
		case 4:
			monster.ghoulInst->SetSpeed(gs15Over8);
			break;
		default:
			monster.ghoulInst->SetSpeed(gs16Over8);
			break;
		}
	}

	return animation;
}

mmove_t	*bodyhuman_c::GetSequenceForProneDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t		*animation = NULL;

	if (IsAvailableSequence(monster, &generic_move_death_lfshotdeath))
	{
		VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
		FinalSound = true;
//		monster.s.angles[YAW]+=180.0;//urg. i'd normally fold over into this anim, so do that quick...
		animation = &generic_move_death_lfshotdeath;
	}
	return animation;
}

mmove_t	*bodyhuman_c::GetSequenceForCrouchDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t		*animation = NULL;

	if (currentmove == &generic_move_crouch_onknees && IsAvailableSequence(monster, &generic_move_death_cch_sonknees))
	{
		VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
		animation = &generic_move_death_cch_sonknees;
	}
	if (IsAvailableSequence(monster, &generic_move_death_crouchshot))
	{
		VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
		animation = &generic_move_death_crouchshot;
	}
	return animation;	
}

mmove_t	*bodyhuman_c::GetSequenceForPostMortem(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
		
	mmove_t		*animation = NULL;
	float		flailLimit = 3 + gi.flrand(0,3);
	float		lastHit = level.time-LastKilledTime;	
		
	//if we're dancing, check to see if we should keep dancing
	if (currentmove==&generic_move_death_dance)
	{
		if (lastHit > 0.3)
		{
			if (IsAvailableSequence(monster, &generic_move_death_fallforward))
			{
				FinalAnim = true;
				FinalSound = true;
				animation = &generic_move_death_fallforward;
			}
		}
		else // keep dancing
		{
			if (emotion == EMOTION_DEAD)
			{
				if(!(rand()%8))VoiceGeneralSound("Hit", monster, 3);
			}
			else
			{
				if(!(rand()%8))VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
			}
		}
	}
	//if we're twitching on our backs, see if we should continue
	else if ((currentmove==&generic_move_death_lbshotloop) && (lastHit > 0.3))
	{
		if (IsAvailableSequence(monster, &generic_move_death_lbshotdeath))
		{
			FinalAnim = true;
			FinalSound = true;
			animation = &generic_move_death_lbshotdeath;
		}
	}
	//if we're in a prone anim, or at the end of one that ends up prone, and we're shot, call the corpse juggle thingy
	else if (((currentmove->actionFlags & ACTFLAG_ALWAYSPRONE)||(currentMoveFinished)) && (lastHit <= .1) && ((currentmove && currentmove->bbox == BBOX_PRESET_LAYBACK)||(currentmove && currentmove->bbox == BBOX_PRESET_LAYSIDE))) //  corpse was just hit
	{
		//sucky rotation check
		if (currentmove->bbox == BBOX_PRESET_LAYSIDE)
		{
			monster.s.angles[YAW] += 180;
		}			
		
		if ((dflags & DT_JUGGLE) && IsAvailableSequence(monster, &generic_move_death_lbshotloop))
		{
			
			FinalAnim = true;
			FinalSound = true;
			animation = &generic_move_death_lbshotloop;
		}
		else if (IsAvailableSequence(monster, &generic_move_death_lbshotdeath) && currentmove!=&generic_move_death_lbshotloop)
		{
			FinalAnim = true;
			FinalSound = true;
			ForceNextAnim = true;
			animation = &generic_move_death_lbshotdeath;
		}
	}
	else if ((currentmove==&generic_move_death_lfshotloop) && (lastHit > 0.3))
	{
		if (IsAvailableSequence(monster, &generic_move_death_lfshotdeath))
		{
			FinalAnim = true;
			FinalSound = true;
			animation = &generic_move_death_lfshotdeath;
		}
	}
	else if (((currentmove->actionFlags & ACTFLAG_ALWAYSPRONE)||(currentMoveFinished)) && (lastHit <= .1) && (currentmove && currentmove->bbox == BBOX_PRESET_LAYFRONT)) //  corpse was just hit
	{
		// lame hacky rotation -- the following lines seem to depend on animator's whims.  This should be fixed after E3 with rotation motion tracks
//			if (((currentmove == &generic_move_death_shldrright)||(currentmove == &generic_move_death_shldrleft)) && IsAvailableSequence(monster, &generic_move_death_lfshotloop))
//			if ((currentmove == &generic_move_death_fallforward) && IsAvailableSequence(monster, &generic_move_death_lfshotloop))
//			{
//				monster.s.angles[YAW] += 180;
//			}
		if ((dflags & DT_JUGGLE) && IsAvailableSequence(monster, &generic_move_death_lfshotloop))
		{
			FinalAnim = true;
			FinalSound = true;
			animation = &generic_move_death_lfshotloop;
		}
		else if (IsAvailableSequence(monster, &generic_move_death_lfshotdeath) && currentmove!=&generic_move_death_lfshotloop)
		{
			FinalAnim = true;
			ForceNextAnim = true;
			FinalSound = true;
			animation = &generic_move_death_lfshotdeath;
		}
	}

	else if ((currentmove==&generic_move_death_backflail && currentMoveFinished) && (flailLimit < lastHit))
	{
		if (IsAvailableSequence(monster, &generic_move_death_backflaildie))
		{
			FinalAnim = true;
			FinalSound = true;
			animation = &generic_move_death_backflaildie;
		}			
	}
	else if ((currentmove==&generic_move_death_backrtleg && currentMoveFinished) && (flailLimit < lastHit))
	{
		if (IsAvailableSequence(monster, &generic_move_death_backrtlegdie))
		{
			FinalSound = true;
			FinalAnim = true;
			animation = &generic_move_death_backrtlegdie;
		}			
	}
	else if ((currentmove==&generic_move_death_backltleg && currentMoveFinished) && (flailLimit < lastHit))
	{
		if (IsAvailableSequence(monster, &generic_move_death_backltlegdie))
		{
			FinalSound = true;
			FinalAnim = true;
			animation = &generic_move_death_backltlegdie;
		}			
	}
	else if ((currentmove->bbox==BBOX_PRESET_LAYBACK && currentMoveFinished) && !FinalAnim)
	{
		if (currentmove == &generic_move_death_legright)
		{
			if (IsAvailableSequence(monster, &generic_move_death_tobackrtleg) && (!gi.irand(0,2))
				&& gzones[GZ_HEAD_FRONT].parent_id && monster.ghoulInst->GetPartOnOff(gzones[GZ_HEAD_FRONT].parent_id))
			{
				VoiceGeneralSound("Writhe", monster, WRITHENUMBER);
				FinalSound = true;
				animation = &generic_move_death_tobackrtleg;
			}
			else
			{
				FinalSound = true;
				FinalAnim = true;
			}

		}
		else if (currentmove == &generic_move_death_tobackrtleg)
		{
			if (IsAvailableSequence(monster, &generic_move_death_backrtleg))
			{
				FinalSound = true;
				animation = &generic_move_death_backrtleg;
			}
		}
		else if (currentmove == &generic_move_death_legleft)
		{
			if (IsAvailableSequence(monster, &generic_move_death_tobackltleg) && (!gi.irand(0,2))
				&& gzones[GZ_HEAD_FRONT].parent_id && monster.ghoulInst->GetPartOnOff(gzones[GZ_HEAD_FRONT].parent_id))
			{
				VoiceGeneralSound("Writhe", monster, WRITHENUMBER);
				FinalSound = true;
				animation = &generic_move_death_tobackltleg;
			}
			else
			{
				FinalSound = true;
				FinalAnim = true;
			}
		}
		else if (currentmove == &generic_move_death_tobackltleg)
		{
			if (IsAvailableSequence(monster, &generic_move_death_backltleg))
			{
				animation = &generic_move_death_backltleg;
			}
		}
		else if (currentmove == &generic_move_death_tobackflail)
		{
			if (IsAvailableSequence(monster, &generic_move_death_backflail))
			{
				animation = &generic_move_death_backflail;
			}
		}
		else if (!(currentmove->actionFlags & ACTFLAG_LOOPANIM))
		{
			if (IsAvailableSequence(monster, &generic_move_death_tobackflail) && (!gi.irand(0,2))
				&& gzones[GZ_HEAD_FRONT].parent_id && monster.ghoulInst->GetPartOnOff(gzones[GZ_HEAD_FRONT].parent_id))
			{
				VoiceGeneralSound("Writhe", monster, WRITHENUMBER);
				FinalSound = true;
				animation = &generic_move_death_tobackflail;
			}
			else
			{
				FinalSound = true;
				FinalAnim = true;
			}
		}
	}

	if (animation)
	{
		return animation;
	}
	return currentmove;
}


void DealWithArmor(edict_t &monster, body_c *body)
{
	int nArmorAmount = 0;
	if (nArmorAmount = (int)body->GetArmorPercentRemaining())
	{	// spawn armor pickup
		edict_t	*armor = G_Spawn();
		armor->count = (nArmorAmount<50)?50:nArmorAmount;
		VectorCopy(monster.s.origin, armor->s.origin);
		armor->s.origin[2] += (monster.maxs[2] - armor->mins[2] + 2);
		VectorCopy(monster.s.angles, armor->s.angles);
		// remove armor from corpse
		body->StripArmor();
		I_SpawnArmor(armor);
	}
}

mmove_t	*bodyhuman_c::GetSequenceForDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t	*animation = &default_death;
	
	// if we just played the microwave gun death, detonate this guy
	
	if (!lock_deaths&& currentMoveFinished && (currentmove == &generic_move_microwave_death))
	{
		vec3_t	fwd, dir, origin;
		float	knockback = 500;
		int		mod = 1;			// never gets used
		ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);

		if (myInstance && !freezeMyBolts)
		{
			freezeMyBolts = true;
			myInstance->StopAnimatingAtAllBolts();
			AngleVectors(monster.s.angles, fwd, NULL, NULL);
			VectorNegate(fwd, dir);
			VectorMA(monster.s.origin, -100, vec3_up, origin);
			Gib(monster, vec3_up, dflags|DT_NEURAL);		// For some reason the DT_NEURAL gets lost off the body dflags...
			gi.sound(&monster, CHAN_AUTO, gi.soundindex("Weapons/MPG/Burst.wav"), 1.0, ATTN_NORM, 0);
			return &generic_move_death_fallforward;
		}
		else
		{
			FinalAnim = true;
			FinalSound = true;
		}
		return currentmove;
	}

	//added the actcode_death bit because the first time i go through here i'll be playing a non-death animation; if that's finished, i don't want to go through this stuff --sfs
	if (currentMoveFinished && (currentmove && currentmove->suggested_action==ACTCODE_DEATH))
	{
		// kef -- I'm gonna assume this means he's _dead_ dead
		DealWithArmor(monster, this);
		FreeKnives(monster);

		if (!bBoxConfirmed && monster.groundentity)
		{	
			ai_c	*ai;
			ai=(ai_c *)((ai_public_c *)monster.ai);
			if(ai)
			{
				// Not in DM.		
				ai->ConfirmBBox(monster, currentmove);
				bBoxConfirmed = true;
			}
		}
	}

	//to let guys show pain through part of their death...
	if (currentMoveFinished && emotion != EMOTION_DEAD && (FinalAnim||level.time-LastKilledTime>10))
	{
		Emote(monster, EMOTION_DEAD, 999999.9);
	}

	//stop gore animation after 5 seconds of initial kill
	if (level.time-LastKilledTime<=.1)
	{
		// need this bit to make sure StopAnimatingAtAllBolts is called if post-mortem limb-severing occurs
		freezeMyBolts=false;
	}
	if (monster.ghoulInst && ((level.time-InitialKilledTime>5) || currentMoveFinished) && !freezeMyBolts)
	{
		ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);
		if (myInstance)
		{
			freezeMyBolts=true;
			myInstance->StopAnimatingAtAllBolts();
		}
	}
	
	if (monster.spawnflags & SPAWNFLAG_ARMOR_PICKUP)
	{	// if this joker's actually a corpse, just go away
		if (lock_deaths)
		{
			return &generic_move_death_fallforward;
		}
		return &generic_move_death_lfshotdeath;
	}

	// if parental lock in on, nip this in the bud
	if(lock_deaths)
	{
		animation = GetSequenceForParentalLockDeath(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move);
		return animation;
	}
		
	//if we're being passed an anim, verify it.  if it checks, use it.
	if (ideal_move && VerifySequence(monster, ideal_move, reject_actionflags))
	{
		return ideal_move;
	}

	//hack for train.  forces the tumble animation
	if (TrainDeath == true)
	{
		if (animation = GetSequenceForTrainDeath(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move))
		{
			return animation;
		}
	}

	//are we already in a death animation?
	if (currentmove && currentmove->suggested_action==ACTCODE_DEATH)
	{
		if (animation = GetSequenceForPostMortem(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move))
		{
			return animation;
		}
	}

	//if i'm crouching, do crouch die
	if (currentmove && currentmove->bbox == BBOX_PRESET_CROUCH)
	{
		if (animation = GetSequenceForCrouchDeath(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move))
		{
			return animation;
		}
	}

	//if i'm prone, do prone die
	if (currentmove && currentmove->bbox == BBOX_PRESET_PRONE)
	{
		if (animation = GetSequenceForProneDeath(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move))
		{
			return animation;
		}
	}

	//are we already in a death animation?
	if (currentmove && (currentmove->bbox == BBOX_PRESET_LAYBACK))
	{
		if (animation = GetSequenceForPostMortem(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move))
		{
			return animation;
		}
	}

	if ((dflags & DT_MANGLE) && currentmove && (currentmove->bbox == BBOX_PRESET_STAND))
	{
		if (animation = GetSequenceForC4Death(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move))
		{
			return animation;
		}
	}

	if ((dflags & DT_NEURAL) && currentmove && (currentmove->bbox == BBOX_PRESET_STAND))
	{
		if (animation = GetSequenceForPulseGunDeath(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move))
		{
			return animation;
		}
	}
		
	if ((dflags & DT_FIRE) && currentmove && (currentmove->bbox == BBOX_PRESET_STAND))
	{
		if (animation = GetSequenceForBurningDeath(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move))
		{
			return animation;
		}
	}

	if ((dflags & DT_JUGGLE)  && currentmove && (currentmove->bbox == BBOX_PRESET_STAND))
	{
		if (animation = GetSequenceForJuggleDeath(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move))
		{
			return animation;
		}
	}

	if (spout != -1)//specific deaths for specific body parts
	{
		if (animation = GetSequenceForGoreZoneDeath(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move))
		{
			return animation;
		}
	}

	if(gi.irand(0,3))
	{
		if (animation = GetSequenceForFacingBasedDeath(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move))
		{
			return animation;
		}
	}
	
	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);

	if(gi.irand(0,3) && the_ai && (DotProduct(the_ai->velocity, the_ai->velocity) > 40)) // running
	{
		if (animation = GetSequenceForRunningDeath(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move))
		{
			return animation;
		}
	}
	
	
	if (animation = GetSequenceForCatchAllDeath(monster, inflictor, attacker, damage, point, dflags, goal_bbox, ideal_move))
	{
		return animation;
	}
	
	return &default_death;
}

mmove_t	*bodyhuman_c::GetSequenceForC4Death(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	mmove_t	*animation = &generic_move_death_long;
	vec3_t	facing, toc4;

	AngleVectors (monster.s.angles, facing, NULL, NULL);
	VectorSubtract(inflictor->s.origin, monster.s.origin,  toc4);

	if ((DotProduct(facing, toc4) < 0)  && IsAvailableSequence(monster, &generic_move_death_fallforward))
	{
		animation = &generic_move_death_fallforward;
	}
	else if (IsAvailableSequence(monster, &generic_move_death_violent))
	{
		animation = &generic_move_death_violent;
	}

	FinalAnim = true;
	FinalSound = true;
	return animation;
}

mmove_t *bodyhuman_c::GetSequenceForPain(edict_t &monster, vec3_t point, float kick, int damage, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
//	if (Skill_NoPain(&monster))
//	{
//		return NULL;
//	}

	if (currentmove && currentmove->bbox == BBOX_PRESET_STAND)
	{
		if (rWeaponShot/* || lWeaponShot*/)
		{
			bool	bTooToughToDropWeapon = false;
			float	fArmorPercentLeft = GetArmorPercentRemaining();
			float	fHealthPercentLeft = (float)monster.health / (float)monster.max_health;

			ai_c *my_ai = (ai_c *)((ai_public_c *)monster.ai);

			rWeaponShot=lWeaponShot=false;
			// if this is an armored guy...
			//only let him drop his gun if he's down to < 50% armor or < 50% health.
			//if(GetArmorCode() == ARMOR_FULL)
			if(my_ai && !(my_ai->GetMySkills()->getDropWeap()))
			{	// rocket guys dropping their guns is weak
				bTooToughToDropWeapon = true;
			}
			if ( (fArmorPercentLeft > 0.50) && (fHealthPercentLeft > 0.50) )
			{
				bTooToughToDropWeapon = true;
			}
			if (!bTooToughToDropWeapon && IsAvailableSequence(monster, &generic_move_gunshotpain))
			{
				VoiceGeneralSound("Disarm", monster, 1);
				return &generic_move_gunshotpain;
			}
			if (IsAvailableSequence(monster, &generic_move_pain_rarm))
			{
				return &generic_move_pain_rarm;
			}
		}
		vec3_t toPoint, forward;
		VectorSubtract(point, monster.s.origin, toPoint);
		VectorNormalize(toPoint);
		AngleVectors(monster.s.angles, forward, NULL, NULL);

		//took a limb shot
		if (bodyDamage & BODYDAMAGE_RECENT)
		{
			if (bodyDamage & BODYDAMAGE_RIGHTLEG)
			{
				if (IsAvailableSequence(monster, &generic_move_pain_rleg))
				{
					VoiceGeneralSound("Scream",monster,SCREAMNUMBER);
					return &generic_move_pain_rleg;
				}
				else if (IsAvailableSequence(monster, &generic_move_pain_rleg_unhurt))
				{
					return &generic_move_pain_rleg_unhurt;
				}
			}
			if (bodyDamage & BODYDAMAGE_LEFTLEG)
			{
				if (IsAvailableSequence(monster, &generic_move_pain_lleg))
				{
					VoiceGeneralSound("Scream",monster,SCREAMNUMBER);
					return &generic_move_pain_lleg;
				}
				else if (IsAvailableSequence(monster, &generic_move_pain_lleg_unhurt))
				{
					return &generic_move_pain_lleg_unhurt;
				}
			}
			if (bodyDamage & BODYDAMAGE_RIGHTARM)
			{
				if (IsAvailableSequence(monster, &generic_move_pain_rarm))
				{
					VoiceGeneralSound("Scream",monster,5);
					return &generic_move_pain_rarm;
				}
				else if (IsAvailableSequence(monster, &generic_move_pain_rarm_unhurt))
				{
					return &generic_move_pain_rarm_unhurt;
				}
			}
			if (lWeaponShot || (bodyDamage & BODYDAMAGE_LEFTARM))
			{
				if (IsAvailableSequence(monster, &generic_move_pain_larm))
				{
					VoiceGeneralSound("Scream",monster,SCREAMNUMBER);
					return &generic_move_pain_larm;
				}
				else if (IsAvailableSequence(monster, &generic_move_pain_larm_unhurt))
				{
					return &generic_move_pain_larm_unhurt;
				}
			}
		}

		//hit from back
		if (DotProduct(forward, toPoint)<0)
		{
			if (IsAvailableSequence(monster, &generic_move_sunhurtfwd_mrs))
			{
				return &generic_move_sunhurtfwd_mrs;
			}
			if (IsAvailableSequence(monster, &generic_move_sunhurtfwd_p))
			{
				return &generic_move_sunhurtfwd_p;
			}
		}
		//hit from front
		else
		{
			//fixme--make this dependant on force of attack...
			if (kick>75 && !gi.irand(0,5))
			{
				if (IsAvailableSequence(monster, &generic_move_armorblown) && IsAvailableSequence(monster, &generic_move_upfromback) && (GetArmorCode() != ARMOR_FULL))
				{
					VoiceGeneralSound("Scream", monster, SCREAMNUMBER);
					return &generic_move_armorblown;
				}
			}
			if (IsAvailableSequence(monster, &generic_move_sunhurtbk_mrs))
			{
				return &generic_move_sunhurtbk_mrs;
			}
			if (IsAvailableSequence(monster, &generic_move_sunhurtbk_p))
			{
				return &generic_move_sunhurtbk_p;
			}
		}
	}

	return NULL;
}

#define ARMORVALUE_STANDARD 300
//Achtung! this won't cancel out old armor settings!
void bodyhuman_c::SetArmor(edict_t &monster, armor_code newarmor)
{
	int theArmorValue=ARMORVALUE_STANDARD;

	//PH3@r m3, 3y3 \/\/|LL h@><0r TH3 5h33@t oUT oF j00!!!!!!!!!!!1!111
	if (GetClassCode()==BODY_DEKKER)
	{
		switch (game.playerSkills.getEnemyValue())
		{
		case 0://super-easy
			theArmorValue=(int)((float)theArmorValue*0.25);
			break;
		case 1://sorta easy
			theArmorValue=(int)((float)theArmorValue*0.3333333);
			break;
		case 2://medium
			theArmorValue=(int)((float)theArmorValue*0.5);
			break;
		case 3://hard
			theArmorValue=(int)((float)theArmorValue*0.5);
			break;
		default:
			break;
		}
	}

	//no clients! they handle armor differently!
	if (monster.client)
	{
		return;
	}

	//make sure i've allocated my gzones--they'll be fiddled with, most likely
	if (numGoreZones < NUM_HUMANGOREZONES)
	{
		return;
	}

	armorCode = newarmor;

	switch (newarmor)
	{
	case ARMOR_NONE:
		gzones[GZ_HEAD_FRONT].damage = 0;
		gzones[GZ_HEAD_BACK].damage = 0;
		gzones[GZ_NECK].damage = 0;
		gzones[GZ_ARM_UPPER_RIGHT].damage =0;
		gzones[GZ_ARM_UPPER_LEFT].damage =0;
		gzones[GZ_ARM_LOWER_RIGHT].damage =0;
		gzones[GZ_ARM_LOWER_LEFT].damage =0;
		gzones[GZ_GROIN].damage =0;
		gzones[GZ_LEG_UPPER_RIGHT_FRONT].damage =0;
		gzones[GZ_LEG_UPPER_RIGHT_BACK].damage =0;
		gzones[GZ_LEG_UPPER_LEFT_FRONT].damage =0;
		gzones[GZ_LEG_UPPER_LEFT_BACK].damage =0;
		gzones[GZ_LEG_LOWER_RIGHT].damage =0;
		gzones[GZ_LEG_LOWER_LEFT].damage =0;
		gzones[GZ_FOOT_RIGHT].damage =0;
		gzones[GZ_FOOT_LEFT].damage =0;
		gzones[GZ_GUT_FRONT_EXTRA].damage =0;
		gzones[GZ_GUT_BACK_EXTRA].damage =0;
		gzones[GZ_SHLDR_RIGHT_FRONT].damage =0;
		gzones[GZ_SHLDR_RIGHT_BACK].damage =0;
		gzones[GZ_SHLDR_LEFT_FRONT].damage =0;
		gzones[GZ_SHLDR_LEFT_BACK].damage =0;
		gzones[GZ_CHEST_FRONT].damage =0;
		gzones[GZ_CHEST_BACK].damage =0;
		gzones[GZ_GUT_FRONT].damage =0;
		gzones[GZ_GUT_BACK].damage =0;
		break;
	case ARMOR_FULL:
		gzones[GZ_HEAD_FRONT].damage-=theArmorValue;
		gzones[GZ_HEAD_BACK].damage-=theArmorValue;
		gzones[GZ_NECK].damage-=theArmorValue;
		//no break! more armor coming!
	case ARMOR_CHESTLIMBS:
		gzones[GZ_ARM_UPPER_RIGHT].damage-=theArmorValue;
		gzones[GZ_ARM_UPPER_LEFT].damage-=theArmorValue;
		gzones[GZ_ARM_LOWER_RIGHT].damage-=theArmorValue;
		gzones[GZ_ARM_LOWER_LEFT].damage-=theArmorValue;
		gzones[GZ_GROIN].damage-=theArmorValue;
		gzones[GZ_LEG_UPPER_RIGHT_FRONT].damage-=theArmorValue;
		gzones[GZ_LEG_UPPER_RIGHT_BACK].damage-=theArmorValue;
		gzones[GZ_LEG_UPPER_LEFT_FRONT].damage-=theArmorValue;
		gzones[GZ_LEG_UPPER_LEFT_BACK].damage-=theArmorValue;
		gzones[GZ_LEG_LOWER_RIGHT].damage-=theArmorValue;
		gzones[GZ_LEG_LOWER_LEFT].damage-=theArmorValue;
		gzones[GZ_FOOT_RIGHT].damage-=theArmorValue;
		gzones[GZ_FOOT_LEFT].damage-=theArmorValue;
		gzones[GZ_GUT_FRONT_EXTRA].damage-=theArmorValue;
		gzones[GZ_GUT_BACK_EXTRA].damage-=theArmorValue;
		//no break! more armor coming!
	case ARMOR_CHEST:
		gzones[GZ_SHLDR_RIGHT_FRONT].damage-=theArmorValue;
		gzones[GZ_SHLDR_RIGHT_BACK].damage-=theArmorValue;
		gzones[GZ_SHLDR_LEFT_FRONT].damage-=theArmorValue;
		gzones[GZ_SHLDR_LEFT_BACK].damage-=theArmorValue;
		gzones[GZ_CHEST_FRONT].damage-=theArmorValue;
		gzones[GZ_CHEST_BACK].damage-=theArmorValue;
		gzones[GZ_GUT_FRONT].damage-=theArmorValue;
		gzones[GZ_GUT_BACK].damage-=theArmorValue;
		break;
	case ARMOR_SOMEWHAT_TOUGH:
		monster.health *= 1.5;
		monster.max_health *= 1.5;
		break;
	case ARMOR_TOUGH:
		monster.health*=2;
		monster.max_health*=2;
		break;
	}
}

armor_code bodyhuman_c::GetArmorCode() 
{
	return armorCode;
}

float bodyhuman_c::GetArmorPercentRemaining()
{
	int		nNumArmoredZones = 0;
	float	fTotalRemaining = 0, fOrigTotal = 0;

	switch (armorCode)
	{
	case ARMOR_NONE:
		// not getting any armor off of this guy
		return 0;
		break;
	case ARMOR_FULL:
		// if there's any armor left on any gore zones, add it to the total
		//(remember that armor is stored as a negative amount of damage)
		if (gzones[GZ_HEAD_FRONT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_HEAD_FRONT].damage;
		}
		if (gzones[GZ_HEAD_BACK].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_HEAD_BACK].damage;
		}
		if (gzones[GZ_NECK].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_NECK].damage;
		}
		nNumArmoredZones += 3;
		//no break! more armor coming!
	case ARMOR_CHESTLIMBS:
		// if there's any armor left on any gore zones, add it to the total
		//(remember that armor is stored as a negative amount of damage)
		if (gzones[GZ_ARM_UPPER_RIGHT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_ARM_UPPER_RIGHT].damage;
		}
		if (gzones[GZ_ARM_UPPER_LEFT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_ARM_UPPER_LEFT].damage;
		}
		if (gzones[GZ_ARM_LOWER_RIGHT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_ARM_LOWER_RIGHT].damage;
		}
		if (gzones[GZ_ARM_LOWER_LEFT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_ARM_LOWER_LEFT].damage;
		}
		if (gzones[GZ_GROIN].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_GROIN].damage;
		}
		if (gzones[GZ_LEG_UPPER_RIGHT_FRONT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_LEG_UPPER_RIGHT_FRONT].damage;
		}
		if (gzones[GZ_LEG_UPPER_RIGHT_BACK].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_LEG_UPPER_RIGHT_BACK].damage;
		}
		if (gzones[GZ_LEG_UPPER_LEFT_FRONT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_LEG_UPPER_LEFT_FRONT].damage;
		}
		if (gzones[GZ_LEG_UPPER_LEFT_BACK].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_LEG_UPPER_LEFT_BACK].damage;
		}
		if (gzones[GZ_LEG_LOWER_RIGHT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_LEG_LOWER_RIGHT].damage;
		}
		if (gzones[GZ_LEG_LOWER_LEFT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_LEG_LOWER_LEFT].damage;
		}
		if (gzones[GZ_FOOT_RIGHT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_FOOT_RIGHT].damage;
		}
		if (gzones[GZ_FOOT_LEFT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_FOOT_LEFT].damage;
		}
		if (gzones[GZ_GUT_FRONT_EXTRA].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_GUT_FRONT_EXTRA].damage;
		}
		if (gzones[GZ_GUT_BACK_EXTRA].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_GUT_BACK_EXTRA].damage;
		}
		nNumArmoredZones += 15;
		//no break! more armor coming!
	case ARMOR_CHEST:
		// if there's any armor left on any gore zones, add it to the total
		//(remember that armor is stored as a negative amount of damage)
		if (gzones[GZ_SHLDR_RIGHT_FRONT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_SHLDR_RIGHT_FRONT].damage;
		}
		if (gzones[GZ_SHLDR_RIGHT_BACK].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_SHLDR_RIGHT_BACK].damage;
		}
		if (gzones[GZ_SHLDR_LEFT_FRONT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_SHLDR_LEFT_FRONT].damage;
		}
		if (gzones[GZ_SHLDR_LEFT_BACK].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_SHLDR_LEFT_BACK].damage;
		}
		if (gzones[GZ_CHEST_FRONT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_CHEST_FRONT].damage;
		}
		if (gzones[GZ_CHEST_BACK].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_CHEST_BACK].damage;
		}
		if (gzones[GZ_GUT_FRONT].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_GUT_FRONT].damage;
		}
		if (gzones[GZ_GUT_BACK].damage < 0)
		{
			fTotalRemaining -= gzones[GZ_GUT_BACK].damage;
		}
		nNumArmoredZones += 8;
		break;
	case ARMOR_SOMEWHAT_TOUGH:
		return 0;
		break;
	case ARMOR_TOUGH:
		// not getting any armor off of this guy
		return 0;
		break;
	}
	fOrigTotal = (float)(nNumArmoredZones * ARMORVALUE_STANDARD);
	return 100 * (fTotalRemaining / fOrigTotal);
}

void bodyhuman_c::StripArmor()
{
	switch (armorCode)
	{
	case ARMOR_NONE:
		break;
	case ARMOR_FULL:
		gzones[GZ_HEAD_FRONT].damage = 0;
		gzones[GZ_HEAD_BACK].damage = 0;
		gzones[GZ_NECK].damage = 0;
		//no break! more armor coming!
	case ARMOR_CHESTLIMBS:
		gzones[GZ_ARM_UPPER_RIGHT].damage = 0;
		gzones[GZ_ARM_UPPER_LEFT].damage = 0;
		gzones[GZ_ARM_LOWER_RIGHT].damage = 0;
		gzones[GZ_ARM_LOWER_LEFT].damage = 0;
		gzones[GZ_GROIN].damage = 0;
		gzones[GZ_LEG_UPPER_RIGHT_FRONT].damage = 0;
		gzones[GZ_LEG_UPPER_RIGHT_BACK].damage = 0;
		gzones[GZ_LEG_UPPER_LEFT_FRONT].damage = 0;
		gzones[GZ_LEG_UPPER_LEFT_BACK].damage = 0;
		gzones[GZ_LEG_LOWER_RIGHT].damage = 0;
		gzones[GZ_LEG_LOWER_LEFT].damage = 0;
		gzones[GZ_FOOT_RIGHT].damage = 0;
		gzones[GZ_FOOT_LEFT].damage = 0;
		gzones[GZ_GUT_FRONT_EXTRA].damage = 0;
		gzones[GZ_GUT_BACK_EXTRA].damage = 0;
		//no break! more armor coming!
	case ARMOR_CHEST:
		gzones[GZ_SHLDR_RIGHT_FRONT].damage = 0;
		gzones[GZ_SHLDR_RIGHT_BACK].damage = 0;
		gzones[GZ_SHLDR_LEFT_FRONT].damage = 0;
		gzones[GZ_SHLDR_LEFT_BACK].damage = 0;
		gzones[GZ_CHEST_FRONT].damage = 0;
		gzones[GZ_CHEST_BACK].damage = 0;
		gzones[GZ_GUT_FRONT].damage = 0;
		gzones[GZ_GUT_BACK].damage = 0;
		break;
	case ARMOR_SOMEWHAT_TOUGH:
		break;
	case ARMOR_TOUGH:
		break;
	}
	armorCode = ARMOR_NONE;
}

void bodyhuman_c::ReloadRWeapon(edict_t &monster)
{
	switch(GetRightHandWeapon(monster))
	{
	case ATK_PISTOL2:
		rWeaponAmmo = 9;
		break;
	case ATK_PISTOL1:
		rWeaponAmmo = 18;
		break;
	case ATK_SNIPER:
		rWeaponAmmo = 6;
		break;
	case ATK_MACHINEGUN:
		//rWeaponAmmo = 50;
		//eric wants this changed
		rWeaponAmmo = 30;
		break;
	case ATK_KNIFE:
		rWeaponAmmo =  1;//huh?
		break;
	case ATK_FLAMEGUN:
		rWeaponAmmo = 100000;//eh?- no reloading for this
		break;
	case ATK_ASSAULTRIFLE:
		rWeaponAmmo = 40;
		break;
	case ATK_MACHINEPISTOL:
		rWeaponAmmo = 33;
		break;
	case ATK_AUTOSHOTGUN:
		rWeaponAmmo = 10;
		break;
	case ATK_SHOTGUN:
		rWeaponAmmo = 8;
		break;
	case ATK_ROCKET:
		rWeaponAmmo =  4;
		break;
	case ATK_MICROWAVE:
	case ATK_MICROWAVE_ALT:
	case ATK_DEKKER:
		rWeaponAmmo = 100;//eh?
		break;
	default:
	case ATK_NOTHING:
		rWeaponAmmo =  1;
		break;
	}
}

void bodyhuman_c::AdjustAimByWeapon(edict_t &monster, vec3_t aimDir)
{
	float rVal;

	switch(GetRightHandWeapon(monster))
	{
	case ATK_PISTOL2:
		rVal = 0;
		break;
	case ATK_PISTOL1:
		rVal = .1;
		break;
	case ATK_SNIPER:
		rVal = 0;
		break;
	case ATK_MACHINEGUN:
		rVal = .2;
		break;
	case ATK_KNIFE:
		rVal = 0;
		break;
	case ATK_FLAMEGUN:
		rVal = .2;
		break;
	case ATK_MICROWAVE:
	case ATK_MICROWAVE_ALT:
	case ATK_DEKKER:
		rVal = .2;
		break;
	case ATK_ROCKET:
		rVal = 0;
		break;
	case ATK_ASSAULTRIFLE:
		rVal = .3;
		break;
	case ATK_MACHINEPISTOL:
		rVal = .4;
		break;
	case ATK_AUTOSHOTGUN:
		rVal = .2;
		break;
	case ATK_SHOTGUN:
		rVal = .2;
		break;
	default:
	case ATK_NOTHING:
		rVal = 0;
		break;
	}

	aimDir[0] += gi.flrand(-rVal, rVal);
	aimDir[1] += gi.flrand(-rVal, rVal);
	aimDir[2] += gi.flrand(-rVal, rVal);
	VectorNormalize(aimDir);
}

qboolean bodyhuman_c::PlayAnimation(edict_t &monster, mmove_t *newanim, bool forceRestart)
{
	//make sure i'm using ground instead of ground2 as my ground reference
	if (forceRestart || newanim != currentmove || ForceNextAnim)
	{
		UseGround2(monster, false);
	}

	if (newanim->actionFlags & ACTFLAG_RHANDOPEN)
	{
		OpenRightHand(monster, true);
	}
	else
	{
		OpenRightHand(monster, false);
	}

	if (newanim->actionFlags & ACTFLAG_LHANDOPEN)
	{
		OpenLeftHand(monster, true);
	}
	else
	{
		OpenLeftHand(monster, false);
	}

	if (newanim->actionFlags & ACTFLAG_GRENADE)
	{
		GrenadeHand(monster, true, false);
	}
	else
	{
		GrenadeHand(monster, false, false);
	}

	if (newanim->actionFlags & ACTFLAG_PISS)
	{
		DoPiss(monster);
	}

	forceRestart |= ForceNextAnim;
	ForceNextAnim = false;

	return bodyorganic_c::PlayAnimation(monster, newanim, forceRestart);
}


void bodyhuman_c::SetFlags(edict_t & monster, int newflags)
{
	frame_flags|=newflags;
	//fixme! if the jumping isn't successful, and i stay on the ground, this will look weird!
	if (newflags & FRAMEFLAG_JUMP && currentmove->actionFlags & ACTFLAG_JUMPTOGROUND2)
	{
		UseGround2(monster, true);
	}
}

void bodyhuman_c::OpenRightHand(edict_t &monster, qboolean toggleVal)
{
	GhoulID tempID;
	if (!monster.ghoulInst)
	{
		return;
	}

	IGhoulObj *MyObject=monster.ghoulInst->GetGhoulObject();
	IGhoulInst *MyInst=monster.ghoulInst;

	if (MyObject && toggleVal && !rHandOpen)
	{
		//if i have a closed hand turned on, switch to appropriate open hand
		tempID=MyObject->FindPart("_rclosed");
		if (tempID&&MyInst->GetPartOnOff(tempID))
		{
			MyInst->SetPartOnOff(tempID,false);
			tempID=MyObject->FindPart("_ropen");
			if (tempID)
			{
				MyInst->SetPartOnOff(tempID,true);
			}
		}
		tempID=MyObject->FindPart("_rbossclosed");
		if (tempID&&MyInst->GetPartOnOff(tempID))
		{
			MyInst->SetPartOnOff(tempID,false);
			tempID=MyObject->FindPart("_rbossopen");
			if (tempID)
			{
				MyInst->SetPartOnOff(tempID,true);
			}
		}
		tempID=MyObject->FindPart("_rhand_closed");
		if (tempID&&MyInst->GetPartOnOff(tempID))
		{
			MyInst->SetPartOnOff(tempID,false);
			tempID=MyObject->FindPart("_rhand_open");
			if (tempID)
			{
				MyInst->SetPartOnOff(tempID,true);
			}
		}
	}
	else if (MyObject && rHandOpen && !toggleVal)
	{
		//if i have a open hand turned on, switch to appropriate closed hand
		tempID=MyObject->FindPart("_ropen");
		if (tempID&&MyInst->GetPartOnOff(tempID))
		{
			MyInst->SetPartOnOff(tempID,false);
			tempID=MyObject->FindPart("_rclosed");
			if (tempID)
			{
				MyInst->SetPartOnOff(tempID,true);
			}
		}
		tempID=MyObject->FindPart("_rbossopen");
		if (tempID&&MyInst->GetPartOnOff(tempID))
		{
			MyInst->SetPartOnOff(tempID,false);
			tempID=MyObject->FindPart("_rbossclosed");
			if (tempID)
			{
				MyInst->SetPartOnOff(tempID,true);
			}
		}
		tempID=MyObject->FindPart("_rhand_open");
		if (tempID&&MyInst->GetPartOnOff(tempID))
		{
			MyInst->SetPartOnOff(tempID,false);
			tempID=MyObject->FindPart("_rhand_closed");
			if (tempID)
			{
				MyInst->SetPartOnOff(tempID,true);
			}
		}
	}
	rHandOpen=toggleVal;
}


void bodyhuman_c::OpenLeftHand(edict_t &monster, qboolean toggleVal)
{
	GhoulID tempID,tempID2;
	if (!monster.ghoulInst)
	{
		return;
	}

	IGhoulObj *MyObject=monster.ghoulInst->GetGhoulObject();
	IGhoulInst *MyInst=monster.ghoulInst;

	if (MyObject && toggleVal && !lHandOpen)
	{
		//if i have a closed hand turned on, switch to appropriate open hand
		tempID=MyObject->FindPart("_lclosed");
		tempID2=MyObject->FindPart("_lopen");
		if (tempID2&&(!tempID||MyInst->GetPartOnOff(tempID)))
		{
			if (tempID)
			{
				MyInst->SetPartOnOff(tempID,false);
			}
			if (tempID2)
			{
				MyInst->SetPartOnOff(tempID2,true);
			}
		}
		tempID=MyObject->FindPart("_lbossclosed");
		tempID2=MyObject->FindPart("_lbossopen");
		if (tempID2&&(!tempID||MyInst->GetPartOnOff(tempID)))
		{
			if (tempID)
			{
				MyInst->SetPartOnOff(tempID,false);
			}
			if (tempID2)
			{
				MyInst->SetPartOnOff(tempID2,true);
			}
		}
		tempID=MyObject->FindPart("_lhand_closed");
		tempID2=MyObject->FindPart("_lhand_open");
		if (tempID2&&(!tempID||MyInst->GetPartOnOff(tempID)))
		{
			if (tempID)
			{
				MyInst->SetPartOnOff(tempID,false);
			}
			if (tempID2)
			{
				MyInst->SetPartOnOff(tempID2,true);
			}
		}
	}
	else if (MyObject && lHandOpen && !toggleVal)
	{
		//if i have a open hand turned on, switch to appropriate closed hand
		tempID=MyObject->FindPart("_lopen");
		tempID2=MyObject->FindPart("_lclosed");
		if (tempID2&&(!tempID||MyInst->GetPartOnOff(tempID)))
		{
			if (tempID)
			{
				MyInst->SetPartOnOff(tempID,false);
			}
			if (tempID2)
			{
				MyInst->SetPartOnOff(tempID2,true);
			}
		}
		tempID=MyObject->FindPart("_lbossopen");
		tempID2=MyObject->FindPart("_lbossclosed");
		if (tempID2&&(!tempID||MyInst->GetPartOnOff(tempID)))
		{
			if (tempID)
			{
				MyInst->SetPartOnOff(tempID,false);
			}
			if (tempID2)
			{
				MyInst->SetPartOnOff(tempID2,true);
			}
		}
		tempID=MyObject->FindPart("_lhand_open");
		tempID2=MyObject->FindPart("_lhand_closed");
		if (tempID2&&(!tempID||MyInst->GetPartOnOff(tempID)))
		{
			if (tempID)
			{
				MyInst->SetPartOnOff(tempID,false);
			}
			if (tempID2)
			{
				MyInst->SetPartOnOff(tempID2,true);
			}
		}
	}
	lHandOpen=toggleVal;
}


void bodyhuman_c::GrenadeHand(edict_t &monster, qboolean toggleVal, qboolean nodrop)
{
	if (!monster.ghoulInst)
	{
		return;
	}

	IGhoulObj *MyObject=monster.ghoulInst->GetGhoulObject();
	ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);

	if (MyObject && toggleVal && !grenadeInHand)
	{	//if I don't have a grenade in my hand, add one to the bolton.
		if (AddBoltedItem(monster, "wbolt_hand_l", "Items/Projectiles", "grenade", "to_wbolt_hand_l", myInstance))
		{
			grenadeInHand=true;
		}
	}
	else if (MyObject && grenadeInHand && !toggleVal)
	{	// If I have a grenade in my hand, turn it off and DROP it at my feet!  (if not player)
		ggBinstC	*boltedptr;
	
		boltedptr=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"));
		if (boltedptr)
		{
			myInstance->RemoveBoltInstance(boltedptr);
		}

		grenadeInHand=false;
	}
}


// Start the pissing effect at the correct bolt-on...
void DoPiss(edict_t &monster)
{
	if (monster.ghoulInst && !lock_textures)
	{
		fxRunner.exec("environ/takeleak", &monster, monster.ghoulInst->GetGhoulObject()->FindPart("gbolt_groin"));
	}
}


void Testboltzap(edict_t *ent)
{
	vec3_t orient;
	ent->nextthink=level.time+0.1;
	AngleVectors(ent->s.angles, orient, NULL, NULL);
	FX_MakeElectricArc(ent->s.origin, 64, orient);
}

void TestBoltPosition(vec3_t origin, vec3_t direction)
{
	edict_t *ent=G_Spawn();
	VectorCopy(origin, ent->s.origin);
	VectorCopy(direction, ent->s.angles);
	ent->think=Testboltzap;
	ent->nextthink=level.time+0.1;
}


//#define BRAIN_SCALE 1.142857142857
#define BRAIN_SCALE 1.17

void bodyhuman_c::ShowBackHoleDamage (edict_t &monster, gz_code frontCode, gz_code backCode, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int take, int knockback, int dflags, int mod)
{
	ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);
	GhoulID		tempHole=0;
	gz_info		*frontHole=NULL;

	if (take <= 0)
	{
		return;
	}
	
	if (frontCode != numGoreZones)
	{
		frontHole=&gzones[frontCode];
	}
	gz_info		*backHole=NULL;
	if (backCode != numGoreZones)
	{
		backHole=&gzones[backCode];
	}
	int i=gi.irand(0,2);
	//exit wound--massive damage, but only if guy wasn't dead before (to prevent super-fast damage-stacking on dead guys)
	if ((take > monster.health) && (monster.health > 0) && backHole && frontHole && backHole->id && backHole->parent_id && backHole != frontHole && myInstance->GetInstPtr()->GetPartOnOff(backHole->parent_id))
	{
		backHole->damage=GetMinimumDamageForLevel(3);
		if (backHole->damage_level<3)
		{
			//if this zone was already active, match up the row
			if (backHole->damage_level)
			{
					GhoulID	skinID=myInstance->GetInstPtr()->GetFrameOverride("gz",backHole->id);
					if (skinID)
					{
						for (i=0;i<4;i++)
						{
							if (skinID==gzone_levels[backHole->damage_level-1][i])
								break;
						}
					}
			}

			// 1/10/00 kef -- there seemed to be some random stompage of i here...it somehow got a value of 4, which
			//severely spooged the following frameoverride stuff. so, in a feat of empiric programming, I'm clamping
			//i to [0,3].
			if (i < 0)
			{
				i = 0;
			}
			else if (i > 3)
			{
				i = 3;
			}
			backHole->damage_level=3;
			if (gzone_levels[2][i] != NULL_GhoulID)
			{
				myInstance->GetInstPtr()->SetFrameOverride("gz",gzone_levels[2][i],backHole->id);
				if (backHole->buddyzone != numGoreZones && gzones[backHole->buddyzone].id)
				{
					gzones[backHole->buddyzone].damage=backHole->damage;
					gzones[backHole->buddyzone].damage_level=backHole->damage_level;
					monster.ghoulInst->SetPartOnOff(gzones[backHole->buddyzone].id, true);
					myInstance->GetInstPtr()->SetFrameOverride("gz",gzone_levels[2][i],gzones[backHole->buddyzone].id);
				}
			}

			if (backHole->bolt)
			{
				myInstance->RemoveBolt(backHole->bolt);

				if (frontHole==&gzones[GZ_HEAD_FRONT]&&backHole==&gzones[GZ_HEAD_BACK]&&take>monster.health&&take>80)
				{
					tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_headbeard");
					if (!tempHole || tempHole!=frontHole->parent_id)
					{
						tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_headhelmet");
						if (tempHole)
						{
							frontHole->parent_id=tempHole;
							BlowPart(monster,frontHole->blowme,NULL);
							monster.ghoulInst->SetPartOnOff(backHole->id, false);
							monster.ghoulInst->SetPartOnOff(frontHole->id, false);
							monster.ghoulInst->SetPartOnOff(frontHole->parent_id, true);
							SetGoreZone(monster, "gz_headhelmet_f", GZ_HEAD_FRONT);
							if (frontHole->id)
							{
								monster.ghoulInst->SetPartOnOff(frontHole->id, true);
								if (frontHole->damage_level>=1 && gzone_levels[frontHole->damage_level-1][i] != NULL_GhoulID)
								{
									myInstance->GetInstPtr()->SetFrameOverride("gz",gzone_levels[frontHole->damage_level-1][i],frontHole->id);
								}
							}
							SetGoreBolt(monster, "abolt_head_t", GZ_HEAD_BACK);
							if (backHole->bolt)
							{
								AddBoltedItem(monster, backHole->bolt, "Enemy/bolt", "g_brain", "to_abolt_head_t", myInstance, "", BRAIN_SCALE);
							}
							return;//don't turn the gorezone back on!
						}
					}
				}

				//bolt on exit wound
				AddBoltedItem(monster, backHole->bolt, "Enemy/bolt", "g_exit_chunky_anim1", "to_gbolt", myInstance);
				FX_BloodCloud(&monster, backHole->bolt, 100);
				if ( dflags & DT_PROJECTILE )
				{
					FX_BloodJet(&monster, point, dir);
				}
			}
		}
		monster.ghoulInst->SetPartOnOff(backHole->id, true);
	}
	else if (monster.health <= 0 && backHole && backHole->id && gi.irand(0,1))
	{	// Just do some blood...
		if (backHole->bolt)
			FX_BloodCloud(&monster, backHole->bolt, 50);
		else if (backHole->capbolt)
			FX_BloodCloud(&monster, backHole->capbolt, 50);
	}
	if ((monster.health <= 0) && (!gi.irand(0,2)) && (dflags & DT_PROJECTILE))
	{
		FX_BloodJet(&monster, point, dir);
	}
}

gutCallback theGutCallback;

bool gutCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	edict_t			*self = (edict_t*)ent;
	ggBinstC		*gutBolt=NULL;
	ai_public_c		*aipub = self->ai;
	ai_c			*ai = (ai_c*)aipub; // sigh, is this really necessary?
	ggOinstC		*myInstance = game_ghoul.FindOInst(self->ghoulInst);
	GhoulID			gutSeq=0;
	ggObjC			*gut=NULL;

	if (!ai || (ai->GetBody() && (level.time-ai->GetBody()->GetInitialKilledTime()>3)) || !ai->GetGhoulObj())
	{
		return true;
	}

	gutBolt=myInstance->GetBoltInstance(ai->GetGhoulObj()->GetMyObject()->FindPart("gbolt_gut_f"));
	gut=game_ghoul.FindObject("Enemy/bolt","guts_bolton");

	if (gut && gutBolt)
	{
		gutSeq=game_ghoul.FindObjectSequence(gut,"g_exit_guts_anim2");
		if (gutSeq)
		{
			gutBolt->PlaySequence(gutSeq, level.time);
		}
	}
	return true;
}

qboolean bodyhuman_c::ShowFrontHoleDamage (edict_t &monster, gz_code frontCode, gz_code backCode, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int take, int knockback, int dflags, int mod)
{
	ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);
	ggOinstC	*blownInst=NULL;
	Vect3		Direction;
	int			i;
	gz_info		*frontHole=NULL;
	qboolean	blewOneOff = false;
	vec3_t		debrisNorm;
	bool		bKnife = false;

	if (take <= 0)
	{
		return false;
	}
	
	assert(gzones);

	// For gib calls, essentially.
	currentGoreZone = frontCode;

	// if I'm taking head/neck damage, no vocal cords for me
	if ( (frontCode == GZ_HEAD_FRONT) || (backCode == GZ_HEAD_FRONT) ||
		 (frontCode == GZ_HEAD_BACK) || (backCode == GZ_HEAD_BACK) ||
		 (frontCode == GZ_NECK) || (backCode == GZ_NECK) )
	{
		SetVocalCordsWorking(false);
	}

	if (inflictor && inflictor->ghoulInst)
	{
		bKnife = ((254 == inflictor->elasticity) && attacker->client);
		inflictor->ghoulInst->SetUserData(inflictor);
	}
	if (frontCode != numGoreZones)
	{
		frontHole=&gzones[frontCode];
	}
	gz_info		*backHole=NULL;
	if (backCode != numGoreZones)
	{
		backHole=&gzones[backCode];
	}

	for (i=0;i<3;i++)
	{
		Direction[i]=dir[i];
	}

	// if we just hit this joker with a thrown knife, try to bolt it on
	//AddBoltedItem(edict_t &monster, GhoulID mybolt, ggObjC *gun, char *itemSeq, GhoulID itemBolt,
	//				ggOinstC *myInstance, char *itemSkinName, float scale)
	if (bKnife && frontHole)
	{
		IGhoulInst	*instKnife = inflictor->ghoulInst;
		IGhoulObj	*objKnife = (instKnife?instKnife->GetGhoulObject():NULL);
		ggOinstC	*oInstKnife = game_ghoul.FindOInst(instKnife);
		ggObjC		*ggObjKnife = NULL;
		ggBinstC	*boltedKnife = NULL;
		GhoulID		boltKnife = NULL_GhoulID;
		GhoulID		blownCapBolt = frontHole->blownCapBolt;

		// the blowncapbolts normally associated with the shoulder gore zones don't 
		//look quite right for bolting on thrown knives. hopefully these will look
		//a little better
		if ((frontCode > GZ_NECK) && (frontCode < GZ_CHEST_FRONT))
		{
			IGhoulObj	*myObj = monster.ghoulInst->GetGhoulObject();
			switch(frontCode)
			{
			case GZ_SHLDR_RIGHT_FRONT:
			case GZ_SHLDR_RIGHT_BACK:
				blownCapBolt = myObj->FindPart("gbolt_cap_rbicep_chest_tight");
				break;
			case GZ_SHLDR_LEFT_FRONT:
			case GZ_SHLDR_LEFT_BACK:
				blownCapBolt = myObj->FindPart("gbolt_cap_lbicep_chest_tight");
				break;
			}
		}
		if (NULL == blownCapBolt)
		{
			// use a default hip location if there's no blownCapBolt where the knife landed
			IGhoulObj	*myObj = monster.ghoulInst->GetGhoulObject();
			
			if (myObj)
			{
				switch(frontCode)
				{
				case GZ_FOOT_RIGHT:
					blownCapBolt = myObj->FindPart("sbolt_heel_r");
					break;
				case GZ_FOOT_LEFT:
					blownCapBolt = myObj->FindPart("sbolt_heel_l");
					break;
				case GZ_NECK:
					//blownCapBolt = gzones[GZ_HEAD_FRONT].blownCapBolt;
					blownCapBolt = myObj->FindPart("abolt_chest");
					if (blownCapBolt)
						break;
//					blownCapBolt = myObj->FindPart("sbolt_heel_l");
//					break;
				default:
					blownCapBolt = myObj->FindPart("abolt_hip_r");
					break;
				}
			}
		}

//		if (blownCapBolt)
//		{
//			IGhoulObj	*myObj = monster.ghoulInst->GetGhoulObject();
//			char rubbish[100];
//			myObj->GetPartName(blownCapBolt, rubbish);
//			gi.dprintf("bolting knife to %s\n", rubbish); 
//		}

		// we need a blownCapBolt for the knife, so if our frontHole doesn't have one, use its capBolt
		if ( oInstKnife && (ggObjKnife = oInstKnife->GetParentObject()) &&
			 objKnife && (boltKnife = objKnife->FindPart("tip")) )
		{
			boltedKnife = AddBoltedItem(monster, blownCapBolt, ggObjKnife, "w_knife",
				boltKnife, myInstance);
			if (boltedKnife && (instKnife = boltedKnife->GetInstPtr()))
			{
				// make sure the knife is aligned such that its hilt points back towards the thrower
				//(woohoo! code reuse!)
				boltonOrientation_c			boltonInfo;
//				vec3_t						vPos, vTemp;
				vec3_t						boltFaceAngs, boltFaceVec;


				//i confess total ignorance on how the orientbolton way of lining up the knife figured this
				//out, but i'm using the actual thrown knife's velocity to determine which way it should be sticking out...
				VectorCopy(inflictor->velocity, boltFaceVec);

				//theoretically, i should have to invert the velocity to get the correct direction, but it works more
				//often if i don't.  that's sad.
//				VectorInverse(boltFaceVec);

				//turn the velocity into a set of angles
				VectorNormalize(boltFaceVec);
				vectoangles(boltFaceVec, boltFaceAngs);

				//smash the pitch to 90 degrees--to make the knives stick out horizontally.  i dunno, it seemed like a good thing to do...
				boltFaceAngs[PITCH]=90.0;

				//translate the angles from degrees to radians.
				boltFaceAngs[PITCH]*=DEGTORAD;
				boltFaceAngs[YAW]*=DEGTORAD;

				Matrix4		EntityToWorld, BoltToEntity, BoltToWorld, WorldToBolt, NewWorldToBolt;
				Matrix4		matOld, matR1, matR2, matTemp, matNew, matTemp2;
				Vect3		tempVector;

				//do a bunch of calculations, all to get the WorldToBolt matrix, so we can use our world-relative rotation
				EntToWorldMatrix(monster.s.origin,monster.s.angles,EntityToWorld);
				monster.ghoulInst->GetBoltMatrix(level.time,BoltToEntity,blownCapBolt,
					IGhoulInst::MatrixType::Entity, true);
				BoltToWorld.Concat(BoltToEntity,EntityToWorld);
				WorldToBolt.Inverse(BoltToWorld);

				//take the translation off our world-to-bolt matrix--we'll use it later, since the knife's velocity
				//(and therefore our calculated angles) are relative to world; but we don't want to reverse the translation on the bolt
				WorldToBolt.SetRow(3);

				//surprisingly, the calculated yaw works best if thrown into the final pitch :/
				matR1.Rotate(PITCH, boltFaceAngs[YAW]);
				//...and the pitch only seems effective if it's thrown into the yaw.
				matR2.Rotate(YAW, boltFaceAngs[PITCH]);

				//well, either way, they need to be mashed together into one matrix.
				matTemp.Concat(matR2, WorldToBolt);

				//this was just a desperate attempt to fix things by mindlessly fiddling with the blowncapbolt's matrix.
				//surprisingly, it didn't do much good.
//				NewWorldToBolt.Identity();
//				WorldToBolt.GetRow(0, tempVector);
//				NewWorldToBolt.SetRow(1, tempVector);
//				WorldToBolt.GetRow(1, tempVector);
//				NewWorldToBolt.SetRow(2, tempVector);
//				WorldToBolt.GetRow(2, tempVector);
//				NewWorldToBolt.SetRow(0, tempVector);

				//our real rotation needs to be concatenated with the inverse of the bolt's rotation, because it's relative to world, not relative to bolt
				matNew.Concat(matR1, matTemp);

				//so that's it--whatever trash we've ended up with gets thrown on the knife.
				instKnife->SetXForm(matNew);



				//this here's the original way of bolting on the knives. i don't know why it's erratic, anymore than
				//i know why the above code is erratic.
//				GetGhoulPosDir(monster.s.origin, monster.s.angles,
//						instKnife, NULL, "tip", vPos, vTemp, NULL, NULL);
//				VectorSubtract(vPos, attacker->s.origin, vTemp);
//				boltonInfo.root = &monster;
//				boltonInfo.boltonInst = instKnife;
//				boltonInfo.boltonID = boltKnife;
//				boltonInfo.parentID = blownCapBolt;
//				boltonInfo.parentInst = monster.ghoulInst;
//				VectorAdd(vPos, vTemp, boltonInfo.vTarget);
//				boltonInfo.fMinPitch = -1000;
//				boltonInfo.fMaxPitch = 1000;
//				boltonInfo.fMinYaw = -1000;
//				boltonInfo.fMaxYaw = 1000;
//				boltonInfo.fMaxTurnSpeed = 1000;
//				boltonInfo.bUsePitch = true;
//				boltonInfo.bUseYaw = true;
//				boltonInfo.bToRoot = true;
//				if (blownCapBolt)
//				{
//					boltonInfo.OrientBolton();
//				}
			}
		}
		// AddBoltedItem created a new ggBinstC of the knife, so we can free the original.
		//HOWEVER, the thrown knife gets freed in knifeThrowCollide, shortly after returning
		//from this function. 
	}
	if (frontHole && frontHole->id && frontHole->parent_id && myInstance->GetInstPtr()->GetPartOnOff(frontHole->parent_id))
	{
		int newlevel;
		if (monster.health <=0 && take < 5)
		{
			frontHole->damage+=10;
		}
		else
		{
			if (monster.health > 0 && take > monster.health)
			{
				frontHole->damage+=monster.health;
			}
			else
			{
				frontHole->damage+=take;
			}
		}
		newlevel=GetDamageLevel(frontHole->damage);
		if (!lock_gorezones)
		{
			monster.ghoulInst->SetPartOnOff(frontHole->id, true);
		}
		if (newlevel>frontHole->damage_level)
		{
			//knock off loose parts
			ShakePart(monster, frontHole->blowme, Direction);

			i = gi.irand(0,2);

			if (mod == MOD_KNIFE_SLASH)
			{
				i=3;
			}

			//if this zone was already active, match up the row
			if ((frontHole->damage_level) && (!lock_gorezones) && i!=3)
			{
				GhoulID	skinID=myInstance->GetInstPtr()->GetFrameOverride("gz",frontHole->id);
				if (skinID)
				{
					for (i=0;i<4;i++)
					{
						if (skinID==gzone_levels[frontHole->damage_level-1][i])
							break;
					}

					//eh, previous injury was knife slash: ditch it
					if (i==3)
					{
						i = gi.irand(0,2);
					}
				}
			}
			if (frontHole->bolt)
			{
				if ((monster.health > 0) && (!(dflags & DT_DIRECTIONALGORE)))
				{
//					gi.sound (&monster, CHAN_BODY, gi.soundindex ("impact/gore/spout.wav"), .8, ATTN_NORM, 0);
					FX_Bleed(&monster, frontHole->bolt, 40);
				}

				if ((frontHole==&gzones[GZ_GUT_FRONT]) && (monster.health <= take) && !gutsOut && (dflags & DT_SEVER) && (!lock_gorezones))
				{
					newlevel=2;
					gutsOut=true;
					myInstance->RemoveBolt(frontHole->bolt);
//					AddBoltedItem(monster, frontHole->bolt, "Enemy/bolt", "g_guts3", "to_gbolt_gut_f", myInstance);
					AddBoltedItem(monster, frontHole->bolt, "Enemy/bolt", "guts_bolton", "to_gbolt_gut_f", myInstance);
					
					ggObjC			*gut=NULL;
					ggBinstC		*gutBolt=NULL;
					
					gut=game_ghoul.FindObject("Enemy/bolt","guts_bolton");
					gutBolt=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("gbolt_gut_f"));
					if (gutBolt && gut)
					{
						GhoulID		gutSeq=0;
						GhoulID		tempNote=0;
					
						//changed this to false, can't cache new seqs in after instances are created --ss
						gutSeq=game_ghoul.FindObjectSequence(gut,"g_exit_guts_anim1");
						if (gutSeq)
						{
							gutBolt->PlaySequence(gutSeq, level.time, 0.0, true, IGhoulInst::Hold);
							// Setup call to twitchy gut when done playing sequence
							tempNote=gut->GetMyObject()->FindNoteToken("EOS");
							if (tempNote)
							{
								gutBolt->GetInstPtr()->AddNoteCallBack(&theGutCallback,tempNote);
							}
						}
					}
				}
			}
			if ((gzone_levels[newlevel-1][i] != NULL_GhoulID) && (!lock_gorezones))
			{
				myInstance->GetInstPtr()->SetFrameOverride("gz",gzone_levels[newlevel-1][i],frontHole->id);
				if (frontHole->buddyzone != numGoreZones && gzones[frontHole->buddyzone].id)
				{
					gzones[frontHole->buddyzone].damage=frontHole->damage;
					gzones[frontHole->buddyzone].damage_level=frontHole->damage_level;
					monster.ghoulInst->SetPartOnOff(gzones[frontHole->buddyzone].id, true);
					myInstance->GetInstPtr()->SetFrameOverride("gz",gzone_levels[newlevel-1][i],gzones[frontHole->buddyzone].id);
				}
			}
			if (!lock_gorezones)
			{
				frontHole->damage_level=newlevel;
			}
		}
		if (monster.health <= take)
		{
			if (frontHole->cap && (ai_goretest->value || (dflags & DT_SEVER) || (dflags & DT_MANGLE) || (frontHole->damage>500)))
//			if (frontHole->blownCap && (ai_goretest->value || (dflags & DT_SEVER) || (dflags & DT_MANGLE) || (frontHole->damage>500)))
			{
				qboolean	allowSever = true;
				
//				if (!(dflags & DT_MANGLE))
				{
				// don't allow torso separation unless DT_MANGLE is specified
					if (frontHole->blowme->area==GBLOWNAREA_CHEST)
					{
						allowSever = false;	
					}
				}
				if ((allowSever) && (!lock_sever))
				{
					edict_t *danglyBit = NULL;
					if (frontHole->blowme->area == GBLOWNAREA_HEAD) // we're gonna sever the head
					{
						// no head, no sound
						SetVocalCordsWorking(false);
						Emote(monster, EMOTION_DEAD, 1.0);
						// chunky exploding head goodness
						GhoulID tempBolt;
						tempBolt=monster.ghoulInst->GetGhoulObject()->FindPart("sbolt_mouth");
						if (tempBolt)
						{
							int numchunks = gi.irand(1,3);
							VectorClear(debrisNorm);
							FX_ThrowDebris(point,debrisNorm, numchunks, DEBRIS_SM, MAT_ROCK_FLESH, 0, 0, 0, SURF_BLOOD);
							numchunks = gi.irand(3,6);
							FX_ThrowDebris(point,debrisNorm, numchunks, DEBRIS_TINY, MAT_ROCK_FLESH, 0, 0, 0, SURF_BLOOD);
							FX_HeadExplosion(&monster, tempBolt);
							gi.sound (&monster, CHAN_BODY, gi.soundindex ("impact/gore/headexp.wav"), .6, ATTN_NORM, 0);
						}
					}
					UpdateFace(monster);
					if (frontHole->blowme->area != GBLOWNAREA_HEAD)
					{
						danglyBit=MakeBlownPart(monster, frontCode, dir, &blownInst);
					}
					if (frontHole->blownCapBolt && blownInst)
					{
						if (frontHole->capPiece)
						{
							if (frontHole->capPieceBolt)
							{
								AddBoltedItem(monster, frontHole->blownCapBolt, "Enemy/bolt", frontHole->capPiece->GetSubName(), frontHole->capPieceBolt, blownInst);
							}
							else
							{
								AddBoltedItem(monster, frontHole->blownCapBolt, "Enemy/bolt", frontHole->capPiece->GetSubName(), "to_gbolt", blownInst);
							}
						}
						else
						{
							AddBoltedItem(monster, frontHole->blownCapBolt, "Enemy/bolt", "g_cap1", "to_gbolt", blownInst);
						}
					}
					if (frontHole->blownCap)
					{
						if (danglyBit && danglyBit->ghoulInst)
						{
							danglyBit->ghoulInst->SetPartOnOff(frontHole->blownCap,true);
							danglyBit->ghoulInst->SetFrameOverride("c","c_blood",frontHole->blownCap);
						}
					}
//					BlowZone(monster,frontHole->childzone,frontHole->childzone,danglyBit);
					BlowZone(monster,frontCode,frontCode,danglyBit);
					BlowPart(monster,frontHole->blowme,danglyBit);
					blewOneOff = true;
					if (frontHole->cap)
					{
						myInstance->GetInstPtr()->SetFrameOverride("c","c_blood",frontHole->cap);
					}
					if (frontHole->capbolt)
					{
						if (frontHole->capPiece)
						{
							if (frontHole->capPieceBolt)
							{
								AddBoltedItem(monster, frontHole->capbolt, frontHole->capPiece, frontHole->capPiece->GetSubName(), frontHole->capPieceBolt, myInstance);
							}
							else
							{
								AddBoltedItem(monster, frontHole->capbolt, "Enemy/bolt", frontHole->capPiece->GetSubName(), "to_gbolt", myInstance);
							}
						}
						else
						{
							AddBoltedItem(monster, frontHole->capbolt, "Enemy/bolt", "g_cap1", "to_gbolt", myInstance);
						}
						if (frontHole->capPiece && !strcmp(frontHole->capPiece->GetSubName(), "g_half_head"))
						{
							// for the half head bolt on, we want to put the living guy's skins on the new bolt on
							char		headSkinName[128];
							char		faceSkinName[128];
							
							GhoulID	headSkin = monster.ghoulInst->GetFrameOverride("h");
							if (headSkin)
							{
								monster.ghoulInst->GetGhoulObject()->GetSkinName(headSkin, headSkinName);
								myInstance->GetBoltInstance(frontHole->capbolt)->GetInstPtr()->SetFrameOverride("h", headSkinName, ""); 
							}
							
							GhoulID	faceSkin = monster.ghoulInst->GetFrameOverride("f");
							if (faceSkin)
							{
								monster.ghoulInst->GetGhoulObject()->GetSkinName(faceSkin, faceSkinName);
								myInstance->GetBoltInstance(frontHole->capbolt)->GetInstPtr()->SetFrameOverride("f", faceSkinName, ""); 
							}

							myInstance->GetBoltInstance(frontHole->capbolt)->GetInstPtr()->SetFrameOverride("gz", "gz_half_head"); 
						}
						FX_BloodCloud(&monster, frontHole->capbolt, 100);
					}

					if ((danglyBit) || (frontHole->blowme->area == GBLOWNAREA_HEAD))
					{
						if (frontHole->parent_id)
						{
							myInstance->GetInstPtr()->SetPartOnOff(frontHole->parent_id,false);
						}
						if (frontHole->id)
						{
							myInstance->GetInstPtr()->SetPartOnOff(frontHole->id,false);
						}
						if (backHole && backHole->id)
						{
							myInstance->GetInstPtr()->SetPartOnOff(backHole->id,false);
						}
						if (frontHole->cap)
						{
							myInstance->GetInstPtr()->SetPartOnOff(frontHole->cap,true);
						}
					}
				}
			}
		}
		else
		{
			//Fixme - this currently overrides all other sounds :/
			VoiceGeneralSound("Hit", monster, 3);
		}
	}
	if ((spout == -1) && monster.health < take)//fixme: this should only be used once, so no callback jive please.
	{
		if (frontHole->capbolt)
		{
			monster.ghoulInst->AddMatrixCallBack(&TheSetVectorCallback,frontHole->capbolt,IGhoulInst::MatrixType::Entity);
			spout = frontHole - gzones;
		}
		else if (frontHole->bolt)
		{
			monster.ghoulInst->AddMatrixCallBack(&TheSetVectorCallback,frontHole->bolt,IGhoulInst::MatrixType::Entity);
			spout = frontHole - gzones;
		}
		else
		{
			VectorClear(monster.pos1);
			spout = frontHole - gzones;
		}
	}

	if ((monster.health < take) && frontHole && frontHole->id /*&& gi.irand(0,1)*/)
	{	// Just do some blood, If the character is dead, I don't believe we have yet spawned blood...
		if (frontHole->bolt)
			FX_BloodCloud(&monster, frontHole->bolt, 50);
		else if (frontHole->capbolt)
			FX_BloodCloud(&monster, frontHole->capbolt, 50);
	}
	
	return blewOneOff;
}

void bodyhuman_c::HealGoreZones(edict_t &monster, int damagedHealth, int healedHealth)
{	// repair a certain number of gore zones based on how badly damaged we were
	//and how much better we are now that we're healed.
	ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);

	// for starters, heal all of 'em
	if (myInstance && monster.ghoulInst)
	{
		for (int j = 0; j < numGoreZones; j++)
		{
			if (gzones[j].id)
			{
				gzones[j].damage=0;
				gzones[j].damage_level=0;
				monster.ghoulInst->SetPartOnOff(gzones[j].id, false);
			}
		}
	}
}

#define MAX_FRONTCODES	5

extern int bulletTraceHitCheck(trace_t *tr, vec3_t start, vec3_t end, int clipMask);

void DropKnife(edict_t &monster, edict_t *knife)
{
	vec3_t vTemp;
	Matrix4 m;

	if (NULL == knife->ghoulInst)
	{
		return;
	}
	VectorScale(knife->pos1, -1, vTemp); // velocity's in pos1
	VectorNormalize(vTemp); // ouch
	VectorMA(monster.s.origin, 40, vTemp, knife->s.origin);
	VectorClear(knife->velocity);
	// make sure it's laying flat
	knife->ghoulInst->GetXForm(m);
	m.Rotate(0, M_PI*-0.5);
	m.CalcFlags();
	knife->ghoulInst->SetXForm(m);
	knife->gravity = 1.0;
	if(dm->isDM())
	{	// can't have these piling up...
		knife->think = G_FreeEdict;
		knife->nextthink = level.time + 30.0;
	}
	// stop that infernal spinning!
	VectorClear(knife->avelocity);
	knife->owner = NULL;
	knife->spawnflags|=DROPPED_ITEM;
	I_SpawnKnife(knife);
	knife->elasticity = 253; // flag this entity so knifeThrowCollide won't free it
}


qboolean CheckBackStab(edict_t &monster, vec3_t origin)
{
	vec3_t myFacing, dmgFacing;

	AngleVectors(monster.s.angles, myFacing, NULL, NULL);

	VectorSubtract(monster.s.origin, origin, dmgFacing);
	VectorNormalize(dmgFacing);

	if(DotProduct(myFacing, dmgFacing) > .707)
	{//nearly colinear - being hit from behind
		return true;
	}

	return false;
}


int bodyhuman_c::HandleNearMiss(edict_t &monster, edict_t *knife)
{
	Matrix4		EntityToWorld, WorldToEnt, BoltToEntity;
	Vect3		vEntKnife, vKnife(*(Vect3*)&knife->s.origin), vEntBolt;
	vec3_t		vDist;
	float		fDist = 0, fBestDist = 999999;
	int			nBestZone = -1;

	EntToWorldMatrix(monster.s.origin, monster.s.angles, EntityToWorld);
	WorldToEnt.Inverse(EntityToWorld);
	// get position of knife (at time of collision with bbox) with respect to entity
	WorldToEnt.XFormPoint(vEntKnife, vKnife);

	for (int j = 0; j < numGoreZones; j++)
	{
		if (gzones[j].bolt)
		{
			// get position of bolt with respect to entity
			monster.ghoulInst->GetBoltMatrix(level.time,BoltToEntity,gzones[j].bolt,
				IGhoulInst::MatrixType::Entity);
			BoltToEntity.GetRow(3, vEntBolt);
			VectorSubtract(*(vec3_t*)(&vEntBolt), *(vec3_t*)(&vEntKnife), vDist);
			// get distance from current bolt to knife
			fDist = VectorLengthSquared(vDist);
			if (dm->isDM() &&
				((j == GZ_HEAD_FRONT) || (j == GZ_HEAD_BACK) || (j == GZ_NECK)) )
			{	//	make headshots harder in deathmatch
				if (fDist < 2500)
				{
					fDist *= 1.3f;
					if (fDist < fBestDist)
					{	// current bolt is the closest one yet 
						fBestDist = fDist;
						nBestZone = j;
					}
				}
			}
			else
			{
				if (fDist < fBestDist)
				{	// current bolt is the closest one yet 
					fBestDist = fDist;
					nBestZone = j;
				}
			}
		}
	}
	return nBestZone;
}


int bodyhuman_c::ResolveKnifeImpact(edict_t &monster, edict_t *inflictor, edict_t *attacker, int *nNearMissZone, vec3_t dir, vec3_t point, vec3_t origin, int *dflags)
{
	// need to perform special trace here, treating thrown knife like a bullet
	trace_t	tr;
	vec3_t	endPos, firePoint, fwd;

	// knife velocity is stored in pos1 (cuz knife->velocity is zero after impact)
	VectorCopy(inflictor->pos1, fwd);
	VectorAdd(attacker->s.origin, attacker->client->ps.viewoffset, firePoint);
//	VectorCopy(attacker->s.origin, firePoint);
	VectorMA(firePoint, 2, fwd, endPos); // gives a length of 1600

	// 1/9/00 kef -- I'm fairly certain this won't screw anything up, and I _need_ to make this change so that
	//NugBuddies and LeanBuddies will work, so I'm passing in 'attacker' instead of 'inflictor'
	gi.polyTrace(firePoint, NULL, NULL, endPos, attacker, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER|CONTENTS_SHOT,
		&tr, bulletTraceHitCheck);
	if (0 == bullet_numHits)
	{	// entered this guy's bbox but missed him. in a perfect world we'd be able to allow 
		//the knife to fly along its merry way til it actually hits something, but for now i'll
		//just drop it here
		if ( (*nNearMissZone = HandleNearMiss(monster, inflictor)) == -1)
		{	// couldn't find a zone anywhere near the knife
			return 0;
		}
		// make headshots a little tougher in deathmatch
		if ( dm->isDM() && (*nNearMissZone < GZ_SHLDR_RIGHT_FRONT) )
		{
			vec3_t monsterFwd;
			bool	bAttackerInFront = false;

			if (0 == *nNearMissZone && (gi.irand(0, 1) != 0)) // GZ_HEAD_FRONT
			{
				*nNearMissZone = GZ_SHLDR_RIGHT_FRONT;
			}
			else if (1 == *nNearMissZone && (gi.irand(0, 1) != 0)) // GZ_HEAD_BACK
			{
				*nNearMissZone = GZ_SHLDR_RIGHT_BACK;
			}
			else if ((2 == *nNearMissZone) && (gi.irand(0,1) != 0))// GZ_NECK
			{
				AngleVectors(monster.s.angles, monsterFwd, NULL, NULL);
				bAttackerInFront = (DotProduct(monsterFwd, fwd) < 0);
				*nNearMissZone = bAttackerInFront?GZ_CHEST_FRONT:GZ_CHEST_BACK;
			}
		}
	}
	VectorCopy(fwd, dir);
	VectorCopy(endPos, point);
	VectorCopy(firePoint, origin);
	*dflags = DT_PROJECTILE;

	return 69;
}

void PrintGoreZones(gz_code *listOfZones, int numOfZones)
{
	for (int i = 0; i < numOfZones; i++)
	{
		switch(listOfZones[i])
		{
		case GZ_HEAD_FRONT:
			gi.dprintf("GZ_HEAD_FRONT ");
			break;
		case GZ_HEAD_BACK:
			gi.dprintf("GZ_HEAD_BACK ");
			break;
		case GZ_NECK:
			gi.dprintf("GZ_NECK ");
			break;
		case GZ_SHLDR_RIGHT_FRONT:
			gi.dprintf("GZ_SHLDR_RIGHT_FRONT ");
			break;
		case GZ_SHLDR_RIGHT_BACK:
			gi.dprintf("GZ_SHLDR_RIGHT_BACK ");
			break;
		case GZ_SHLDR_LEFT_FRONT:
			gi.dprintf("GZ_SHLDR_LEFT_FRONT ");
			break;
		case GZ_SHLDR_LEFT_BACK:
			gi.dprintf("GZ_SHLDR_LEFT_BACK ");
			break;
		case GZ_CHEST_FRONT:
			gi.dprintf("GZ_CHEST_FRONT ");
			break;
		case GZ_CHEST_BACK:
			gi.dprintf("GZ_CHEST_BACK ");
			break;
		case GZ_ARM_UPPER_RIGHT:
			gi.dprintf("GZ_ARM_UPPER_RIGHT ");
			break;
		case GZ_ARM_UPPER_LEFT:
			gi.dprintf("GZ_ARM_UPPER_LEFT ");
			break;
		case GZ_ARM_LOWER_RIGHT:
			gi.dprintf("GZ_ARM_LOWER_RIGHT ");
			break;
		case GZ_ARM_LOWER_LEFT:
			gi.dprintf("GZ_ARM_LOWER_LEFT ");
			break;
		case GZ_GUT_FRONT:
			gi.dprintf("GZ_GUT_FRONT ");
			break;
		case GZ_GUT_BACK:
			gi.dprintf("GZ_GUT_BACK ");
			break;
		case GZ_GROIN:
			gi.dprintf("GZ_GROIN ");
			break;
		case GZ_LEG_UPPER_RIGHT_FRONT:
			gi.dprintf("GZ_LEG_UPPER_RIGHT_FRONT ");
			break;
		case GZ_LEG_UPPER_RIGHT_BACK:
			gi.dprintf("GZ_LEG_UPPER_RIGHT_BACK ");
			break;
		case GZ_LEG_UPPER_LEFT_FRONT:
			gi.dprintf("GZ_LEG_UPPER_LEFT_FRONT ");
			break;
		case GZ_LEG_UPPER_LEFT_BACK:
			gi.dprintf("GZ_LEG_UPPER_LEFT_BACK ");
			break;
		case GZ_LEG_LOWER_RIGHT:
			gi.dprintf("GZ_LEG_LOWER_RIGHT ");
			break;
		case GZ_LEG_LOWER_LEFT:
			gi.dprintf("GZ_LEG_LOWER_LEFT ");
			break;
		case GZ_FOOT_RIGHT:
			gi.dprintf("GZ_FOOT_RIGHT ");
			break;
		case GZ_FOOT_LEFT:
			gi.dprintf("GZ_FOOT_LEFT ");
			break;
		case GZ_GUT_FRONT_EXTRA:
			gi.dprintf("GZ_GUT_FRONT_EXTRA ");
			break;
		case GZ_GUT_BACK_EXTRA:
			gi.dprintf("GZ_GUT_BACK_EXTRA ");
			break;
		}
	}
	gi.dprintf("\n");
}

// going to add to listOfFrontCodes <nChoices> random selections from the list presented in listOfChoices
void AddRandomGoreZones(gz_code	*listOfFrontCodes, int *numOfFrontCodes, gz_code *listOfChoices, int numOfChoices, int nChoices)
{
	int newChoice = 0, swapVal = -1;

	if ((nChoices < 1) || (numOfChoices < 1))
	{
		return;
	}
	if (nChoices > numOfChoices)
	{
		nChoices = numOfChoices;
	}
	for (int i = 0; i < nChoices; i++)
	{
		newChoice = gi.irand(i, numOfChoices-1);
		swapVal = listOfChoices[i];
		listOfChoices[i] = listOfChoices[newChoice];
		listOfChoices[newChoice] = swapVal;
		listOfFrontCodes[(*numOfFrontCodes)++]=listOfChoices[i];
	}
//	PrintGoreZones(listOfChoices, nChoices);

}

void ShotgunStand(edict_t &monster, vec3_t origin, vec3_t point, gz_code *backCode, gz_code	*listOfFrontCodes, int *numOfFrontCodes)
{
	vec3_t	myFacing, dmgFacing, mySide, myUp, preNormFacing;
	bool	bHitFromBehind = false;
	int		headShotChance = 10; // default to one-in-ten chance
	bool	bShotsAimingUp = false;
	float	monsterHeadHeight = monster.s.origin[2] + monster.maxs[2];
	gz_code	listOfChoices[NUM_HUMANGOREZONES];

//	memset(listOfChoices, 9, sizeof(gz_code)*NUM_HUMANGOREZONES);

// just here for reference...
//
// 	InitBBoxPreset(BBOX_PRESET_STAND,		-16, -16, -32, 16, 16, 41); // scaled-up 8/7ths
//
	VectorCopy(monster.s.angles, preNormFacing);
	preNormFacing[2] = 0;
	AngleVectors(preNormFacing, myFacing, mySide, myUp);

	VectorSubtract(monster.s.origin, origin, dmgFacing);
	// make sure you set bShotsAimingUp _before_ you normalize the dmgFacing
	bShotsAimingUp = (dmgFacing[2] > 0);
	dmgFacing[2] = 0;
	VectorNormalize(dmgFacing);

	// if our shot is coming from above, higher chance for a headshot. if it's coming from below and hits
	//near the top of our bbox, higher chance then, too.
	if (origin[2] > monster.s.origin[2])
	{ // shot's coming from above
		headShotChance = 6;
	}
	else if (bShotsAimingUp)
	{
		// 'point' is where the damage intersects monster's bbox
		if (abs(point[2]-monsterHeadHeight) < 10)
		{
			headShotChance = 4;
		}
	}
	bHitFromBehind = (DotProduct(myFacing, dmgFacing) > .707);

	if (point[2] > monster.s.origin[2])
	{	// hit above the belt
		if(bHitFromBehind)
		{//nearly colinear - being hit from behind
			*backCode=GZ_CHEST_FRONT;
			// hit at least one gz
			listOfChoices[0] = GZ_CHEST_BACK;
			listOfChoices[1] = GZ_SHLDR_LEFT_BACK;
			listOfChoices[2] = GZ_SHLDR_RIGHT_BACK;
			listOfChoices[3] = GZ_GUT_BACK;
			memset(&listOfChoices[4], GZ_GUT_BACK, NUM_HUMANGOREZONES - 4);
			AddRandomGoreZones(listOfFrontCodes, numOfFrontCodes, listOfChoices, 4, gi.irand(1, 4));
			// small chance of hitting the head
			if (0 == gi.irand(0, headShotChance))
			{
				listOfFrontCodes[(*numOfFrontCodes)++]=GZ_HEAD_BACK;
			}
		}
		else if(DotProduct(myFacing, dmgFacing) < -.707)
		{//in the front - ow!
			*backCode=GZ_CHEST_BACK;
			listOfChoices[0] = GZ_CHEST_FRONT;
			listOfChoices[1] = GZ_SHLDR_LEFT_FRONT;
			listOfChoices[2] = GZ_SHLDR_RIGHT_FRONT;
			listOfChoices[3] = GZ_GUT_FRONT;
			memset(&listOfChoices[4], GZ_GUT_FRONT, NUM_HUMANGOREZONES - 4);
			AddRandomGoreZones(listOfFrontCodes, numOfFrontCodes, listOfChoices, 4, gi.irand(1, 4));
			// small chance of hitting the head
			if (0 == gi.irand(0, headShotChance))
			{
				listOfFrontCodes[(*numOfFrontCodes)++]=GZ_HEAD_FRONT;
			}
		}
		else if(DotProduct(mySide, dmgFacing) > 0)
		{//um...
			*backCode=GZ_CHEST_BACK;
			listOfChoices[0] = GZ_CHEST_FRONT;
			listOfChoices[1] = GZ_ARM_UPPER_LEFT;
			listOfChoices[2] = GZ_GUT_BACK;
			listOfChoices[3] = GZ_SHLDR_LEFT_BACK;
			memset(&listOfChoices[4], GZ_SHLDR_LEFT_BACK, NUM_HUMANGOREZONES - 4);
			AddRandomGoreZones(listOfFrontCodes, numOfFrontCodes, listOfChoices, 4, gi.irand(1, 4));
			// small chance of hitting the head
			if (0 == gi.irand(0, headShotChance))
			{
				listOfFrontCodes[(*numOfFrontCodes)++]=GZ_HEAD_FRONT;
			}
		}
		else
		{//er...
			*backCode=GZ_CHEST_BACK;
			listOfChoices[0] = GZ_CHEST_FRONT;
			listOfChoices[1] = GZ_ARM_UPPER_RIGHT;
			listOfChoices[2] = GZ_GUT_BACK;
			listOfChoices[3] = GZ_SHLDR_RIGHT_BACK;
			memset(&listOfChoices[4], GZ_SHLDR_RIGHT_BACK, NUM_HUMANGOREZONES - 4);
			AddRandomGoreZones(listOfFrontCodes, numOfFrontCodes, listOfChoices, 4, gi.irand(1, 4));
			// small chance of hitting the head
			if (0 == gi.irand(0, headShotChance))
			{
				listOfFrontCodes[(*numOfFrontCodes)++]=GZ_HEAD_FRONT;
			}
		}
	}
	else
	{	// hit below the belt
		if(DotProduct(myFacing, dmgFacing) < 0) // from front
		{
			*backCode=GZ_LEG_UPPER_RIGHT_BACK;
			listOfChoices[0] = GZ_LEG_UPPER_RIGHT_FRONT;
			listOfChoices[1] = GZ_LEG_LOWER_RIGHT;
			listOfChoices[2] = GZ_LEG_LOWER_LEFT;
			listOfChoices[3] = GZ_LEG_UPPER_LEFT_FRONT;
			memset(&listOfChoices[4], GZ_LEG_UPPER_LEFT_FRONT, NUM_HUMANGOREZONES - 4);
			AddRandomGoreZones(listOfFrontCodes, numOfFrontCodes, listOfChoices, 4, gi.irand(1, 4));
		}
		else // from back
		{
			*backCode=GZ_LEG_UPPER_RIGHT_FRONT;
			listOfChoices[0] = GZ_LEG_UPPER_RIGHT_BACK;
			listOfChoices[1] = GZ_LEG_LOWER_RIGHT;
			listOfChoices[2] = GZ_LEG_LOWER_LEFT;
			listOfChoices[3] = GZ_LEG_UPPER_LEFT_BACK;
			memset(&listOfChoices[4], GZ_LEG_UPPER_LEFT_BACK, NUM_HUMANGOREZONES - 4);
			AddRandomGoreZones(listOfFrontCodes, numOfFrontCodes, listOfChoices, 4, gi.irand(1, 4));
		}
	}
}

void ShotgunCrouch(edict_t &monster, vec3_t origin, vec3_t point, gz_code *backCode, gz_code	*listOfFrontCodes, int *numOfFrontCodes)
{
	vec3_t	myFacing, dmgFacing, mySide, myUp, preNormFacing;
	bool	bHitFromBehind = false;
	int		headShotChance = 10; // default to one-in-ten chance
	bool	bShotsAimingUp = false;
	float	monsterHeadHeight = monster.s.origin[2] + monster.maxs[2];
	gz_code	listOfChoices[NUM_HUMANGOREZONES];

// just here for reference...
//
//	InitBBoxPreset(BBOX_PRESET_CROUCH,		-16, -16, -16, 16, 16, 28);
//
	VectorCopy(monster.s.angles, preNormFacing);
	preNormFacing[2] = 0;
	AngleVectors(preNormFacing, myFacing, mySide, myUp);

	// dmgFacing is the vector from the attacker to the intersection of the damage vector and the monster's bbox
	VectorSubtract(point, origin, dmgFacing);
	// make sure you set bShotsAimingUp _before_ you normalize the dmgFacing
	bShotsAimingUp = (dmgFacing[2] > 0);
	dmgFacing[2] = 0;
	VectorNormalize(dmgFacing);

	// if our shot is coming from above, higher chance for a headshot. if it's coming from below and hits
	//near the top of our bbox, higher chance then, too.
	if (origin[2] > monster.s.origin[2])
	{ // shot's coming from above
		headShotChance = 6;
	}
	else if (bShotsAimingUp)
	{
		// 'point' is where the damage intersects monster's bbox
		if (abs(point[2]-monsterHeadHeight) < 10)
		{
			headShotChance = 4;
		}
	}
	bHitFromBehind = (DotProduct(myFacing, dmgFacing) > .707);

	if(bHitFromBehind)
	{//nearly colinear - being hit from behind
		*backCode=GZ_CHEST_FRONT;
		listOfChoices[0] = GZ_CHEST_BACK;
		listOfChoices[1] = GZ_SHLDR_LEFT_BACK;
		listOfChoices[2] = GZ_SHLDR_RIGHT_BACK;
		listOfChoices[3] = GZ_GUT_BACK;
		memset(&listOfChoices[4], GZ_GUT_BACK, NUM_HUMANGOREZONES - 4);
		AddRandomGoreZones(listOfFrontCodes, numOfFrontCodes, listOfChoices, 4, gi.irand(1, 4));
		// small chance of hitting the head
		if (0 == gi.irand(0, headShotChance))
		{
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_HEAD_BACK;
		}
	}
	else if(DotProduct(myFacing, dmgFacing) < -.707)
	{//in the front - ow!
		*backCode=GZ_CHEST_BACK;
		listOfChoices[0] = GZ_CHEST_FRONT;
		listOfChoices[1] = GZ_SHLDR_LEFT_FRONT;
		listOfChoices[2] = GZ_SHLDR_RIGHT_FRONT;
		listOfChoices[3] = GZ_GUT_FRONT;
		listOfChoices[4] = GZ_LEG_UPPER_RIGHT_FRONT;
		memset(&listOfChoices[5], GZ_LEG_LOWER_LEFT, NUM_HUMANGOREZONES - 5);
		AddRandomGoreZones(listOfFrontCodes, numOfFrontCodes, listOfChoices, 5, gi.irand(1, 5));
		// small chance of hitting the head
		if (0 == gi.irand(0, headShotChance))
		{
			if (*numOfFrontCodes >= MAX_FRONTCODES)
			{
				listOfFrontCodes[MAX_FRONTCODES-1]=GZ_HEAD_FRONT;
			}
			else
			{
				listOfFrontCodes[(*numOfFrontCodes)++]=GZ_HEAD_FRONT;
			}
		}
	}
	else if(DotProduct(mySide, dmgFacing) > 0)
	{//um...
		*backCode=GZ_CHEST_BACK;
		listOfChoices[0] = GZ_CHEST_FRONT;
		listOfChoices[1] = GZ_SHLDR_LEFT_FRONT;
		listOfChoices[2] = GZ_LEG_UPPER_LEFT_FRONT;
		listOfChoices[3] = GZ_ARM_LOWER_LEFT;
		listOfChoices[4] = GZ_SHLDR_LEFT_BACK;
		memset(&listOfChoices[5], GZ_LEG_LOWER_LEFT, NUM_HUMANGOREZONES - 5);
		AddRandomGoreZones(listOfFrontCodes, numOfFrontCodes, listOfChoices, 5, gi.irand(1, 5));
		// small chance of hitting the head
		if (0 == gi.irand(0, headShotChance))
		{
			if (*numOfFrontCodes >= MAX_FRONTCODES)
			{
				listOfFrontCodes[MAX_FRONTCODES-1]=GZ_HEAD_FRONT;
			}
			else
			{
				listOfFrontCodes[(*numOfFrontCodes)++]=GZ_HEAD_FRONT;
			}
		}
	}
	else
	{//er...
		*backCode=GZ_CHEST_BACK;
		listOfChoices[0] = GZ_CHEST_FRONT;
		listOfChoices[1] = GZ_SHLDR_RIGHT_FRONT;
		listOfChoices[2] = GZ_LEG_UPPER_RIGHT_FRONT;
		listOfChoices[3] = GZ_ARM_LOWER_RIGHT;
		listOfChoices[4] = GZ_SHLDR_RIGHT_BACK;
		memset(&listOfChoices[5], GZ_LEG_LOWER_RIGHT, NUM_HUMANGOREZONES - 5);
		AddRandomGoreZones(listOfFrontCodes, numOfFrontCodes, listOfChoices, 5, gi.irand(1, 5));
		// small chance of hitting the head
		if (0 == gi.irand(0, headShotChance))
		{
			if (*numOfFrontCodes >= MAX_FRONTCODES)
			{
				listOfFrontCodes[MAX_FRONTCODES-1]=GZ_HEAD_FRONT;
			}
			else
			{
				listOfFrontCodes[(*numOfFrontCodes)++]=GZ_HEAD_FRONT;
			}
		}
	}
}

void ShotgunProne(edict_t &monster, vec3_t origin, vec3_t point, gz_code *backCode, gz_code	*listOfFrontCodes, int *numOfFrontCodes)
{
}

void ShotgunLayBack(edict_t &monster, vec3_t origin, vec3_t point, gz_code *backCode, gz_code	*listOfFrontCodes, int *numOfFrontCodes)
{
}

void bodyhuman_c::ChooseShotgunGoreZones(edict_t &monster, vec3_t origin, vec3_t point, gz_code *backCode, gz_code	*listOfFrontCodes, int *numOfFrontCodes)
{
	// 'point' is supposedly the worldspace intersection of the damage vector and monster's bbox

	vec3_t myFacing, dmgFacing, mySide, myUp, preNormFacing;
	bool bHitFromBehind = false;

	VectorCopy(monster.s.angles, preNormFacing);
	preNormFacing[2] = 0;
	AngleVectors(preNormFacing, myFacing, mySide, myUp);

	VectorSubtract(monster.s.origin, origin, dmgFacing);
	dmgFacing[2] = 0;
	VectorNormalize(dmgFacing);

	bHitFromBehind = (DotProduct(myFacing, dmgFacing) > .707);

	// if our magic shot hit a gore zone, that's where we want to focus our damage
	if (s_FrontCode != NUM_HUMANGOREZONES)
	{
		switch (s_FrontCode)
		{
		case GZ_HEAD_FRONT:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_NECK;
			break;
		case GZ_HEAD_BACK:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_NECK;
			break;
		case GZ_NECK:
			if (bHitFromBehind)
			{
				listOfFrontCodes[(*numOfFrontCodes)++]=GZ_HEAD_BACK;
			}
			else
			{
				listOfFrontCodes[(*numOfFrontCodes)++]=GZ_HEAD_FRONT;
			}
			break;
		case GZ_SHLDR_RIGHT_FRONT:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_LEFT_FRONT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_CHEST_FRONT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_ARM_UPPER_RIGHT;
			break;
		case GZ_SHLDR_RIGHT_BACK:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_LEFT_BACK;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_CHEST_BACK;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_ARM_UPPER_RIGHT;
			break;
		case GZ_SHLDR_LEFT_FRONT:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_RIGHT_FRONT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_CHEST_FRONT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_ARM_UPPER_LEFT;
			break;
		case GZ_SHLDR_LEFT_BACK:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_RIGHT_BACK;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_CHEST_BACK;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_ARM_UPPER_LEFT;
			break;
		case GZ_CHEST_FRONT:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_RIGHT_FRONT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_LEFT_FRONT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GUT_FRONT;
			break;
		case GZ_CHEST_BACK:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_RIGHT_BACK;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_LEFT_BACK;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GUT_BACK;
			break;
		case GZ_ARM_UPPER_RIGHT:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_RIGHT_FRONT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_ARM_LOWER_RIGHT;
			break;
		case GZ_ARM_UPPER_LEFT:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_LEFT_FRONT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_ARM_LOWER_LEFT;
			break;
		case GZ_ARM_LOWER_RIGHT:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_ARM_UPPER_RIGHT;
			break;
		case GZ_ARM_LOWER_LEFT:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_ARM_UPPER_LEFT;
			break;
		case GZ_GUT_FRONT:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_RIGHT_FRONT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_CHEST_FRONT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GROIN;
			break;
		case GZ_GUT_BACK:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_LEFT_BACK;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_CHEST_BACK;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_RIGHT_BACK;
			break;
		case GZ_GROIN:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GROIN;
			break;
		case GZ_LEG_UPPER_RIGHT_FRONT:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GROIN;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_LEG_LOWER_RIGHT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GUT_FRONT;
			break;
		case GZ_LEG_UPPER_RIGHT_BACK:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GROIN;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_LEG_LOWER_RIGHT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GUT_BACK;
			break;
		case GZ_LEG_UPPER_LEFT_FRONT:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GROIN;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_LEG_LOWER_LEFT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GUT_FRONT;
			break;
		case GZ_LEG_UPPER_LEFT_BACK:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GROIN;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_LEG_LOWER_LEFT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GUT_BACK;
			break;
		case GZ_LEG_LOWER_RIGHT:
			if (bHitFromBehind)
			{
				listOfFrontCodes[(*numOfFrontCodes)++]=GZ_LEG_UPPER_RIGHT_BACK;
			}
			else
			{
				listOfFrontCodes[(*numOfFrontCodes)++]=GZ_LEG_UPPER_RIGHT_FRONT;
			}
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_FOOT_RIGHT;
			break;
		case GZ_LEG_LOWER_LEFT:
			if (bHitFromBehind)
			{
				listOfFrontCodes[(*numOfFrontCodes)++]=GZ_LEG_UPPER_LEFT_BACK;
			}
			else
			{
				listOfFrontCodes[(*numOfFrontCodes)++]=GZ_LEG_UPPER_LEFT_FRONT;
			}
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_FOOT_LEFT;
			break;
		case GZ_FOOT_RIGHT:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_LEG_LOWER_RIGHT;
			break;
		case GZ_FOOT_LEFT:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_LEG_LOWER_LEFT;
			break;
		case GZ_GUT_FRONT_EXTRA:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_RIGHT_FRONT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_CHEST_FRONT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GROIN;
			break;
		case GZ_GUT_BACK_EXTRA:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_LEFT_BACK;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_CHEST_BACK;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_RIGHT_BACK;
			break;
		}
	}
	else
	{
		// 12/29/99 kef -- sadly, we need to check in here if the guy is standing, crouched, or prone
		if (NULL == currentmove)
		{
			ShotgunStand(monster, origin, point, backCode, listOfFrontCodes, numOfFrontCodes);
		}
		else
		{
			switch(currentmove->bbox)
			{
				case BBOX_PRESET_STAND:
					ShotgunStand(monster, origin, point, backCode, listOfFrontCodes, numOfFrontCodes);
					break;
				case BBOX_PRESET_CROUCH:
					ShotgunCrouch(monster, origin, point, backCode, listOfFrontCodes, numOfFrontCodes);
					break;
				case BBOX_PRESET_PRONE:
				case BBOX_PRESET_LAYFRONT:
					ShotgunCrouch(monster, origin, point, backCode, listOfFrontCodes, numOfFrontCodes);
					break;
				case BBOX_PRESET_LAYBACK:
				case BBOX_PRESET_LAYSIDE:
					ShotgunCrouch(monster, origin, point, backCode, listOfFrontCodes, numOfFrontCodes);
					break;
				default: // no clue what to do here
					ShotgunStand(monster, origin, point, backCode, listOfFrontCodes, numOfFrontCodes);
					break;
			}
		}
	}
	s_FrontCode = NUM_HUMANGOREZONES;
}

void bodyhuman_c::ChooseDirectionalGoreZones(edict_t &monster, vec3_t origin, gz_code *backCode, gz_code	*listOfFrontCodes, int *numOfFrontCodes)
{
	vec3_t myFacing, dmgFacing, mySide;

	AngleVectors(monster.s.angles, myFacing, mySide, 0);

	VectorSubtract(monster.s.origin, origin, dmgFacing);
	VectorNormalize(dmgFacing);

	if(DotProduct(myFacing, dmgFacing) > .707)
	{//nearly colinear - being hit from behind
		*backCode=GZ_CHEST_FRONT;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_CHEST_BACK;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_LEFT_BACK;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_RIGHT_BACK;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GUT_BACK;
	}
	else if(DotProduct(myFacing, dmgFacing) < -.707)
	{//in the front - ow!
		*backCode=GZ_CHEST_BACK;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_CHEST_FRONT;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_LEFT_FRONT;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_RIGHT_FRONT;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GUT_FRONT;
	}
	else if(DotProduct(mySide, dmgFacing) > 0)
	{//um...
		*backCode=GZ_CHEST_BACK;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_CHEST_FRONT;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_ARM_UPPER_LEFT;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GUT_BACK;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_LEFT_BACK;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GUT_FRONT;

	}
	else
	{//er...
		*backCode=GZ_CHEST_BACK;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_CHEST_FRONT;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_ARM_UPPER_RIGHT;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GUT_BACK;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_SHLDR_RIGHT_BACK;
		listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GUT_FRONT;
	}
}

int bodyhuman_c::GetProjectileFrontHole(edict_t &monster, edict_t *attacker, vec3_t origin, vec3_t dir, vec3_t point, gz_code *backCode,
										int nNearMissZone, bool bKnife, ggOinstC *myInstance, ggBinstC	*boltPiece,
										vec3_t ExitPos, Vect3 EntryPos, Matrix4	ZoneMatrix, Vect3 zonePos, Vect3 Direction,
										int *take, ggBinstC *oldLeftWeap, ggBinstC *oldRightWeap, int *numOfFrontCodes,
										gz_code *frontCode, bool *EntryPosIsValid, gz_code *listOfFrontCodes)
{
	int		i, j,backHoleHitNum;
	float	bestGoreDist, tdist;
	float	curGoreDist;
	vec3_t	tvec;

	
	//NO HITS!!!
	if (bullet_numHits == 0 && (nNearMissZone == -1) )
	{
		//if i'm a client, try again :(
		if (monster.client && monster.ghoulInst)
		{
		}
		if (bullet_numHits == 0)
		{
//				gi.dprintf("Missed!\n");
			return 0;
		}
	}

	AI_GORETEST_PRINTF("%d hits!\n", bullet_numHits);
	if (!bKnife)
	{
		//get exit wound
		//doing exit wound first, so i can get multiple entry wounds and stop consideration at the exit wound--the other way around wouldn't work: only want one exit wound.
		for (i=bullet_numHits-1;i>=0;i--)
		{
			//hit my instance
			if (bullet_Hits[i].Inst==myInstance->GetInstPtr())
			{
				//hit something
				if (bullet_Hits[i].Mesh)
				{
					boltPiece=NULL;//don't blow stuff off that got hit after this.

				
					//don't already have a backhole
					if (*backCode==numGoreZones)
					{
						VectorScale(*(vec3_t*)(&bullet_EntDir),bullet_Hits[i].Distance,ExitPos);
						Vec3AddAssign(*(vec3_t*)(&bullet_EntStart), ExitPos);
						AI_GORETEST_PRINTF("Exit wound position: %2.2f %2.2f %2.2f\n",ExitPos[0],ExitPos[1],ExitPos[2]);
						tdist=bestGoreDist=9999999;
						for (j=0;j<numGoreZones;j++)
						{
							//hit the part that this zone is attached to--should still check to make sure shot is entry, and find the best zone on this part.
							if (bullet_Hits[i].Mesh==gzones[j].parent_id)
							{
								AI_GORETEST_PRINTF("considering %s for backHole\n",gznames[j]);
								if (!gzones[j].bolt)
								{
									if (*backCode==numGoreZones || tdist>bullet_Hits[i].Distance)
									{
										tdist=bullet_Hits[i].Distance;
										*backCode = (gz_code)j;
										AI_GORETEST_PRINTF("Setting backHole to %s\n",gznames[j]);
									}
								}
								else
								{
									//is this a better match than what's already in fronthole?
									bullet_Hits[i].Inst->GetBoltMatrix(level.time,ZoneMatrix,gzones[j].bolt,IGhoulInst::MatrixType::Entity);
									ZoneMatrix.GetRow(3,zonePos);
									AI_GORETEST_PRINTF("Zone's bolt position: %2.2f %2.2f %2.2f\n",zonePos[0],zonePos[1],zonePos[2]);
									VectorSubtract(ExitPos,*(vec3_t*)(&zonePos),tvec);
									curGoreDist = VectorLengthSquared(tvec);
									if (*backCode==numGoreZones || curGoreDist<bestGoreDist)
									{
										tdist=bullet_Hits[i].Distance;
										*backCode = (gz_code)j;
										bestGoreDist = curGoreDist;
										AI_GORETEST_PRINTF("Setting backHole to %s\n",gznames[j]);
									}
								}
							}
						}
						if (*backCode!=numGoreZones)
						{
							// tdist is distance from gun point to enemy 
							VectorScale(dir,tdist,ExitPos);
							VectorAdd(origin,ExitPos,ExitPos);
							backHoleHitNum=i;
						}
					}
				}
			}
			//hit a bolt-on
			else// if (backCode==numGoreZones)
			{
				boltPiece = myInstance->FindBoltInstance(bullet_Hits[i].Inst);
			}
		}
	} // if (!bKnife)
	else
	{
		backHoleHitNum = bullet_numHits - 1;
	}

	bool	hitEnemyAfterBoltOn = true;
	
	//if the first thing this shot hit was a bolt-on, try to blow it off.
	if (boltPiece && boltPiece->GetBolterBolt())
	{
		hitEnemyAfterBoltOn = false;
		
		int k;
		VectorScale(dir,tdist,*(vec3_t*)(&EntryPos));
		for (k=0;k<3;k++)
		{
			EntryPos[k]+=point[k];
		}
		if (SpecialActionBoltOn(monster, boltPiece, EntryPos, Direction, *take))
		{	// e.g. shooting the air tanks off of a Serbian soldier
			return 99999;
		}
		else
		{
			bool	bTooToughToDropWeapon = false, bHitAWeapon = false;
			float	fArmorPercentLeft = GetArmorPercentRemaining();
			float	fHealthPercentLeft = (float)monster.health / (float)monster.max_health;

			ai_c *my_ai = (ai_c *)((ai_public_c *)monster.ai);

			// if this is an armored guy...
			//only let him drop his gun if he's down to < 50% armor or < 50% health.
			if(my_ai && !(my_ai->GetMySkills()->getDropWeap()))
			{	// rocket guys dropping their guns is weak
				bTooToughToDropWeapon = true;
			}
			if ( (fArmorPercentLeft > 0.50) && (fHealthPercentLeft > 0.50) )
			{
				bTooToughToDropWeapon = true;
			}
			bHitAWeapon = (GetWeaponType(boltPiece) != SFW_EMPTYSLOT);
			if(monster.flags & FL_IAMTHEBOSS)
			{	// the boss hates it when you shoot his weapon.  So do so.
			}
			else if ( (bHitAWeapon && !bTooToughToDropWeapon) ||
				 (!bHitAWeapon) ||
				 dm->isDM())
			{
				*take = DropBoltOn(monster, boltPiece, EntryPos, Direction, *take, attacker);

			}
			else if (bHitAWeapon && bTooToughToDropWeapon)
			{	// if we hit a weapon but this guy is too tough to drop it yet, return
				//some minimal amount of damage
				return *take*.25;
			}
		}

		//did i get a weapon shot out of my hands???
		if(oldLeftWeap&&oldLeftWeap!=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l")))
		{
			lWeaponShot=true;
		}
		if (oldRightWeap&&oldRightWeap!=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r")))
		{
			rWeaponShot=true;
		}

		if (take == 0) // this is purty hacky, right hyaw...
		{
			return 1;
		}
	}

	//get entrance wound--stop at one!
	for (i=0;i<bullet_numHits&&i<backHoleHitNum&&*numOfFrontCodes<1;i++)
	{
		//hit my instance
		if (bullet_Hits[i].Inst==myInstance->GetInstPtr())
		{
			//hit something
			if (bullet_Hits[i].Mesh)
			{
				hitEnemyAfterBoltOn = true;
				
				VectorScale(*(vec3_t*)(&bullet_EntDir),bullet_Hits[i].Distance,*(vec3_t*)(&EntryPos));
				EntryPos += bullet_EntStart;
				AI_GORETEST_PRINTF("Entry wound position: %2.2f %2.2f %2.2f\n",EntryPos[0],EntryPos[1],EntryPos[2]);
				tdist=bestGoreDist=9999999;
				for (j=0;j<numGoreZones;j++)
				{
					//hit the part that this zone is attached to--should still check to make sure shot is entry, and find the best zone on this part.
					if (bullet_Hits[i].Mesh==gzones[j].parent_id)
					{
						AI_GORETEST_PRINTF("considering %s for frontHole\n",gznames[j]);
						if (!gzones[j].bolt)
						{
							if (*frontCode==numGoreZones || tdist>bullet_Hits[i].Distance)
							{
								tdist=bullet_Hits[i].Distance;
								*frontCode = (gz_code)j;
								AI_GORETEST_PRINTF("Setting frontHole to %s\n",gznames[j]);
							}
						}
						else
						{
							//is this a better match than what's already in fronthole?
							bullet_Hits[i].Inst->GetBoltMatrix(level.time,ZoneMatrix,gzones[j].bolt,IGhoulInst::MatrixType::Entity);
							ZoneMatrix.GetRow(3,zonePos);
							AI_GORETEST_PRINTF("Candidate position: %2.2f %2.2f %2.2f\n",zonePos[0],zonePos[1],zonePos[2]);
							VectorSubtract(*(vec3_t*)(&EntryPos),*(vec3_t*)(&zonePos),tvec);
							curGoreDist = VectorLengthSquared(tvec);
							if ( bKnife && dm->isDM() &&
								 ((*frontCode == GZ_HEAD_FRONT) || (*frontCode == GZ_HEAD_BACK) || (*frontCode == GZ_NECK)) )
							{	// make knife headshots harder in deathmatch
								curGoreDist *= 1.3f;
								if (*frontCode==numGoreZones || curGoreDist<bestGoreDist)
								{
									tdist=bullet_Hits[i].Distance;
									*frontCode = (gz_code)j;
									bestGoreDist = curGoreDist;
									AI_GORETEST_PRINTF("Setting frontHole to %s\n",gznames[j]);
								}
							}
							else
							{
								if (*frontCode==numGoreZones || curGoreDist<bestGoreDist)
								{
									tdist=bullet_Hits[i].Distance;
									*frontCode = (gz_code)j;
									bestGoreDist = curGoreDist;
									AI_GORETEST_PRINTF("Setting frontHole to %s\n",gznames[j]);
								}
							}
						}
					}
				}
				if (*frontCode!=numGoreZones)
				{
					int k;

					VectorScale(dir,tdist*(7/8),*(vec3_t*)(&EntryPos));
					for (k=0;k<3;k++)
					{
						EntryPos[k]+=point[k];
					}
					for (k=0;k<3;k++)
					{
						if (EntryPos[k]<monster.s.origin[k]+monster.mins[k])
						{
							EntryPos[k]=monster.s.origin[k]+monster.mins[k];
						}
						if (EntryPos[k]>monster.s.origin[k]+monster.maxs[k])
						{
							EntryPos[k]=monster.s.origin[k]+monster.maxs[k];
						}
					}
					*EntryPosIsValid=true;

					listOfFrontCodes[(*numOfFrontCodes)++]=*frontCode;
				}
			}
		}
	}
	if (!hitEnemyAfterBoltOn)
	{
		return 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	// MESSY PLAYER HEAD HACK BEGIN
	///////////////////////////////////////////////////////////////////////////////////////////

	//HOO BOY! LOOKIT DIS! YOU FUCKIN HOODHEADS THINK YOU'RE SO SPECIAL NOW, RIGHT?
	if (monster.client && (*numOfFrontCodes == 1) && (*frontCode == GZ_HEAD_BACK) 
		&& (gzones[*frontCode].parent_id == monster.ghoulInst->GetGhoulObject()->FindPart("_HOODHEAD")))
	{
		Matrix4 ToEnt,ToWorld;
		Vect3 transform;
		int tempReturn;
		GhoulID tempHole;

		monster.ghoulInst->SetPartOnOff("_HOODHEAD", false);
		monster.ghoulInst->SetPartOnOff("_BALDHEAD", true);
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_baldhead");
		if (tempHole)
		{
			gzones[GZ_HEAD_BACK].parent_id=tempHole;
		}

		EntToWorldMatrix(monster.s.origin,monster.s.angles,ToWorld);
		//but we want to put the ray into ent space, need inverse
		ToEnt.Inverse(ToWorld);
		
		VectorCopy(origin,*((vec3_t*)&transform));
		ToEnt.XFormPoint(bullet_EntStart,transform);
		ToEnt.XFormVect(bullet_EntDir,*(Vect3 *)dir);
		bullet_EntDir.Norm();
		bullet_numHits = monster.ghoulInst->RayTrace(level.time,bullet_EntStart,bullet_EntDir,bullet_Hits,20);

		//OH MY LORD I LIKE THIS! Also, it should be safe, as the hood head is now turned off--won't come back here infinitely
		tempReturn = GetProjectileFrontHole(monster, attacker, origin, dir, point, backCode,
										nNearMissZone, bKnife, myInstance, boltPiece,
										ExitPos, EntryPos, ZoneMatrix, zonePos, Direction,
										take, oldLeftWeap, oldRightWeap, numOfFrontCodes,
										frontCode, EntryPosIsValid, listOfFrontCodes);

		monster.ghoulInst->SetPartOnOff("_HOODHEAD", true);
		monster.ghoulInst->SetPartOnOff("_BALDHEAD", false);
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_hoodhead");
		if (tempHole)
		{
			gzones[GZ_HEAD_BACK].parent_id=tempHole;
		}
		return tempReturn;
	}

	//HOO BOY! LOOKIT DIS! YOU FUCKIN MASKHEAD ASSHOLES THINK YOU'RE SPECIAL AS WELL, YES?
	if (monster.client && *numOfFrontCodes == 1 && ((*frontCode == GZ_HEAD_BACK)||(*frontCode == GZ_HEAD_FRONT))  &&
		(gzones[*frontCode].parent_id == monster.ghoulInst->GetGhoulObject()->FindPart("_MASKHEAD")))
	{
		Matrix4 ToEnt,ToWorld;
		Vect3 transform;
		int tempReturn;
		GhoulID tempHole;

		monster.ghoulInst->SetPartOnOff("_MASKHEAD", false);
		monster.ghoulInst->SetPartOnOff("_BALDHEAD", true);
		monster.ghoulInst->SetPartOnOff("_COMFACE", true);
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_baldhead");
		if (tempHole)
		{
			gzones[GZ_HEAD_BACK].parent_id=tempHole;
		}
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_COMFACE");
		if (tempHole)
		{
			gzones[GZ_HEAD_FRONT].parent_id=tempHole;
		}

		EntToWorldMatrix(monster.s.origin,monster.s.angles,ToWorld);
		//but we want to put the ray into ent space, need inverse
		ToEnt.Inverse(ToWorld);
		
		VectorCopy(origin,*((vec3_t*)&transform));
		ToEnt.XFormPoint(bullet_EntStart,transform);
		ToEnt.XFormVect(bullet_EntDir,*(Vect3 *)dir);
		bullet_EntDir.Norm();
		bullet_numHits = monster.ghoulInst->RayTrace(level.time,bullet_EntStart,bullet_EntDir,bullet_Hits,20);

		//OH MY LORD I LIKE THIS! Also, it should be safe, as the hood head is now turned off--won't come back here infinitely
		tempReturn = GetProjectileFrontHole(monster, attacker, origin, dir, point, backCode,
										nNearMissZone, bKnife, myInstance, boltPiece,
										ExitPos, EntryPos, ZoneMatrix, zonePos, Direction,
										take, oldLeftWeap, oldRightWeap, numOfFrontCodes,
										frontCode, EntryPosIsValid, listOfFrontCodes);

		monster.ghoulInst->SetPartOnOff("_MASKHEAD", true);
		monster.ghoulInst->SetPartOnOff("_BALDHEAD", false);
		monster.ghoulInst->SetPartOnOff("_COMFACE", false);
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_MASKHEAD");
		if (tempHole)
		{
			gzones[GZ_HEAD_BACK].parent_id=tempHole;
			gzones[GZ_HEAD_FRONT].parent_id=tempHole;
		}
		return tempReturn;
	}

	//sfs--while I'm making a mess here, might as well ignore shots that only hit a mohawk
	if (monster.client && (*numOfFrontCodes == 0)
		&& (bullet_Hits[0].Mesh == monster.ghoulInst->GetGhoulObject()->FindPart("_MOHAWK")))
	{
		return 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	// MESSY PLAYER HEAD HACK END
	///////////////////////////////////////////////////////////////////////////////////////////

	return -1;
}



extern int hackGlobalGibCalls;

int bodyhuman_c::ShowDamage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb)
{
	vec3_t		bodyloc;//, footorigin;
	gz_code		listOfFrontCodes[MAX_FRONTCODES];
	int			numOfFrontCodes=0;
	gz_code		frontCode=numGoreZones;
	gz_code		backCode=numGoreZones;
	vec3_t		forward, right, up;
	vec3_t		to_impact;
	ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);
	int			i;
	int			take;
	Vect3		EntryPos;
	bool		EntryPosIsValid=false;
	vec3_t		ExitPos;
	Vect3		zonePos;
	Vect3		Direction;
	Matrix4		ZoneMatrix;
	ggBinstC	*oldLeftWeap, *oldRightWeap;
	ggBinstC	*boltPiece=NULL;//contender for being shot off
	qboolean	oneLimbSevered=false;
	vec3_t		debrisNorm;
	bool		bKnife = false;
	bool		bArmor = false; // need to know this to handle the knife properly
	int			nNearMissZone = -1;

	//first, store off what i got in my hands...
	oldLeftWeap=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"));
	oldRightWeap=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r"));

	// if I'm taking damage from the neural pulse-a-majiggy, disable my vocal cords
	if (dflags & DT_NEURAL)
	{
		SetVocalCordsWorking(false);
	}

	// if we're the target of a scripted kill...well...we're gonna die right the heck now
	switch(nNextShotKillsMe)
	{
	case NEXTSHOT_EXPLODE_MY_NUG:
		{
			dflags |= DT_SEVER;
			// no break...intentional.
		}
	case NEXTSHOT_KILLS:
		{
			damage = 9999;

			// I suppose that bad things might happen if this flag is on and we get shot again
			nNextShotKillsMe = NEXTSHOT_NULL;
			break;
		}
	default:
		break;
	}
	
	for (i=0;i<3;i++)
	{
		Direction[i]=dir[i];
	}

	if (inflictor)
	{
		bKnife = ((254 == inflictor->elasticity) && attacker->client);
	}

	if((dflags & DT_ENEMY_ROCKET) && (GetArmorCode() == ARMOR_FULL))
	{	//these guys are immune to their own attack, at least for now.  Maybe we adjust later?
		return 0;
	}

	//if unknown type of damage, don't even bother, just accept the value

	// 12/29/99 kef -- added DT_SEVER to this check. is that bad? sue me.
	if( !(dflags & (DT_PROJECTILE|DT_MELEE|DT_DIRECTIONALGORE|DT_MANGLE|DT_SEVER)) &&
		!bKnife)
	{
		// if we fell, and realistic damage is on, take leg damage
		if (dflags & DT_FALL) 
		{
			VoiceGeneralSound("hit", monster, 3);
			if ( dm->dmRule_REALISTIC_DAMAGE() )
			{
				incMovescale(&monster,0.1);
			}
		}
		return damage;//other tests?  I dunno - knife should be here maybe too
	}
	else if (bKnife)
	{
		int knifeDamage = 69;
		// determine gore zone that the knife will affect.  Nearmiss things are handled here too.
		knifeDamage = ResolveKnifeImpact(monster, inflictor, attacker, &nNearMissZone, dir, point, origin, &dflags);
		if (knifeDamage == 0)
		{
			return 0;
		}
	}

	// For backstabs, we shall check to see if the attack came from behind.
	if (dflags & DT_BACKSTAB)
	{
		if (CheckBackStab(monster, origin))
			damage *= 2.5;
	}

	take = damage;

	//shotgun shot--plop on generic damage for now
	// 12/29/99 kef -- Dan and I -- mostly Dan, in case this whole thing goes south -- changed this. 
	//using the DT_PROJECTILE check cuz we don't want the first magic shot
	//from the shotgun attack (see fireSpas()) to go through ChooseShotgunGoreZones, only the actual damage that comes after
	//said magic shot.
	if ( ((dflags & DT_SHOTGUN) && !(dflags & DT_PROJECTILE)) || 
		 ((dflags & DT_BACKSTAB) && (0 == bullet_numHits)) )
	{
		//new, very random zone-chooser.
		ChooseShotgunGoreZones(monster, origin, point, &backCode, listOfFrontCodes, &numOfFrontCodes);
		// sad
		if (dflags & DT_BACKSTAB)
		{	// a knife...only use one gorezone
			numOfFrontCodes = 1;
			if ((listOfFrontCodes[0] <= GZ_NECK) || (listOfFrontCodes[0] >= GZ_LEG_LOWER_RIGHT))
			{	// dunno what to do here. punt.
				listOfFrontCodes[0] = GZ_CHEST_BACK;
			}
		}
	}
	else if(dflags & DT_MANGLE)
	{
		// do stuff
		if(dflags & DT_DIRECTIONALGORE)
		{
			// sort out what sort of damage should be shown based on which side is being pelted
			// note that all of this is kind of a 2d representation of everything - good enough for now, I s'pose =/

			ChooseDirectionalGoreZones(monster, origin, &backCode, listOfFrontCodes, &numOfFrontCodes);
		}
	}
	else if(dflags & DT_DIRECTIONALGORE)
	{
		// sort out what sort of damage should be shown based on which side is being pelted
		// note that all of this is kind of a 2d representation of everything - good enough for now, I s'pose =/

		ChooseDirectionalGoreZones(monster, origin, &backCode, listOfFrontCodes, &numOfFrontCodes);
	}
	//projectile--go through the trace and get the best frontHole
	else
	{
		int returnDamage = -1;
		returnDamage = GetProjectileFrontHole(monster, attacker, origin, dir, point, &backCode, nNearMissZone, bKnife, myInstance,
			boltPiece, ExitPos, EntryPos, ZoneMatrix, zonePos, Direction, &take, oldLeftWeap, oldRightWeap, &numOfFrontCodes,
			&frontCode, &EntryPosIsValid, listOfFrontCodes);
		// 12/29/99 kef -- shotguns may want to know if our "magic shot" hits a gore zone
		s_FrontCode = frontCode;
		if (returnDamage == 0)
		{
			return 0;
		}
	}

	//make adjustment for armor here--is this a good way to handle it? or should players & enemies handle armor the same?
	if (monster.client)
	{
		int bodydmg = 0;

		if(dflags&DAMAGE_NO_ARMOR)
		{
			// Ignore protective effects of armor for armor-piercing weapons
			bodydmg=take;
		}
		else if(!(dm->isDM()) || !(dm->dmRule_REALISTIC_DAMAGE()) || (numOfFrontCodes == 0))
		{
			// Always adjust damage due to protective effects of armor in single player and non-realistic DM modes 
			bodydmg=monster.client->inv->adjustDamageByArmor(take, penetrate, absorb);
		}
		else
		{
			// Determine whether armor is protecting for each frontCode in Realistic DM
			int damagePerZone = (int)((float)take / (float)numOfFrontCodes);
			
			for(int goreCount = 0; goreCount < numOfFrontCodes; goreCount++)
			{
				if(dm->IsUnprotectedGoreZone(listOfFrontCodes[goreCount]))
				{
					bodydmg += damagePerZone;
				}
				else
				{
					bodydmg += monster.client->inv->adjustDamageByArmor(damagePerZone, penetrate, absorb);
				}
			}
		}

		if (bodydmg)
		{
			monster.flags |= FL_BODY_DAMAGE;
		}
		if (bodydmg < take)
		{
			bArmor = true;
			monster.flags |= FL_ARMOR_DAMAGE;
			take = bodydmg;
			if (bodydmg<=0)
			{
				if (EntryPosIsValid)
				{
					vec3_t sparkdir;
					AI_GORETEST_PRINTF("spraying sparks from %2.2f %2.2f %2.2f; monster pos %2.2f %2.2f %2.2f\n",EntryPos[0],EntryPos[1],EntryPos[2],monster.s.origin[0],monster.s.origin[1],monster.s.origin[2]);
					VectorSubtract(origin,point,sparkdir);
					VectorNormalize(sparkdir);
					VectorScale(dir,-1.0f,sparkdir);
					fxRunner.setDir(sparkdir);
					if(absorb)
					{
						fxRunner.exec("weakspark", *((vec3_t*)&EntryPos));//eh, using same spark as walls now--hope that's ok
					}
					else if(penetrate)
					{
						fxRunner.exec("armorhit", *((vec3_t*)&EntryPos));//eh, using same spark as walls now--hope that's ok
					}
					else
					{
						fxRunner.exec("spark2", *((vec3_t*)&EntryPos));//eh, using same spark as walls now--hope that's ok
					}
					gi.sound (&monster, CHAN_AUTO, gi.soundindex ("impact/armor/hit.wav"), .6, ATTN_NORM, 0);
				}
				if (bKnife)
				{ // drop the knife where it hit the guy
					DropKnife(monster, inflictor);
				}
				return 0;
			}
		}
	}

	// if we're here because of a near miss from a thrown knife, fake a couple of things
	if (nNearMissZone != -1)
	{
		numOfFrontCodes = 1;
		listOfFrontCodes[0] = nNearMissZone;
	}

	//update for monster armor here. if Mr. Monster's taking mangle damage, though, (e.g. c4) his 
	//armor don't mean squat.
	if (monster.ai && !(dflags & DT_MANGLE))
	{
		//go thru all front holes, distributing damage amongst the armor
		for (i=0;i<numOfFrontCodes;i++)
		{
			frontCode=listOfFrontCodes[i];
			//make sure hole is valid
			if (frontCode >= 0 && frontCode < numGoreZones)
			{
				//if fronthole has armor, evaluate
				if (gzones[frontCode].damage<0)
				{
					// kef -- I _think_ getting in here means we hit armor
					bArmor = true;

					//first, show the armor damage (this may immediately switch over to bloodiness, but that should be ok)
					if (gzones[frontCode].id&&myInstance->GetInstPtr()->SetFrameOverride("gz", "gz_armor", gzones[frontCode].id))
					{
						myInstance->GetInstPtr()->SetPartOnOff(gzones[frontCode].id, true);
					}

					//update buddy zone, if any
					if (gzones[frontCode].buddyzone<numGoreZones && gzones[gzones[frontCode].buddyzone].id
						&& myInstance->GetInstPtr()->SetFrameOverride("gz", "gz_armor", gzones[gzones[frontCode].buddyzone].id))
					{
						myInstance->GetInstPtr()->SetPartOnOff(gzones[gzones[frontCode].buddyzone].id, true);
					}

					// this handles the special properties some of the weapons have regarding armor piercing / absorbtion
					float penetrateDmg = 0;
					if(gzones[frontCode].damage < 0)
					{
						take -= (take * absorb);
						//penetrateDmg = take * penetrate;
						//take -= penetrateDmg;
					}

					float tempDmg;//to handle armor piercers
					tempDmg = take;
					if(penetrate > 0)
					{
						tempDmg *= 2;
					}

					//all damage covered
					if (tempDmg <= -gzones[frontCode].damage)
					{
						bodyDamage&=~BODYDAMAGE_RECENT;
						gzones[frontCode].damage+=tempDmg;

						if (EntryPosIsValid)
						{
							vec3_t sparkdir;
							AI_GORETEST_PRINTF("spraying sparks from %2.2f %2.2f %2.2f; monster pos %2.2f %2.2f %2.2f\n",EntryPos[0],EntryPos[1],EntryPos[2],monster.s.origin[0],monster.s.origin[1],monster.s.origin[2]);
							VectorSubtract(origin,point,sparkdir);
							VectorNormalize(sparkdir);
							VectorScale(dir,-1.0f,sparkdir);
							fxRunner.setDir(sparkdir);
							if(absorb)
							{
								fxRunner.exec("weakspark", *((vec3_t*)&EntryPos));//eh, using same spark as walls now--hope that's ok
							}
							else if(penetrate)
							{
								fxRunner.exec("armorhit", *((vec3_t*)&EntryPos));//eh, using same spark as walls now--hope that's ok
							}
							else
							{
								fxRunner.exec("spark2", *((vec3_t*)&EntryPos));//eh, using same spark as walls now--hope that's ok
							}
							gi.sound (&monster, CHAN_AUTO, gi.soundindex ("impact/armor/hit.wav"), .6, ATTN_NORM, 0);
//							FX_MakeSparks(*((vec3_t*)&EntryPos), sparkdir, 1);
						}
						// kef -- handle thrown knives properly
						if (bKnife)
						{	// just drop the knife where it hit the guy
							DropKnife(monster, inflictor);
						}
						return 0;
					}
					//not all damage covered
					else
					{
						tempDmg+=gzones[frontCode].damage;
						gzones[frontCode].damage=0;
					}
					if(penetrate > 0)
					{	// if we were destroying armor a second ago, return to our appropriate damage amount
						tempDmg *= .5;
					}
					take = tempDmg;
				}
			}
		}
	}

	// kef -- if we get here with a boltPiece but numFrontCodes is 0, we hit a bolton but
	//didn't hit any part of this monster's body. we've already dropped the bolton so just
	//return 0 damage.
	if ( (numOfFrontCodes == 0) && (boltPiece) )
	{
		return 0;
	}
	// throw some fleshy debris chunks for the kiddies
	if (!VectorCompare(point, vec3_origin))
	{
		int numchunks = gi.irand(0,5);
		VectorClear(debrisNorm);
		FX_ThrowDebris(point,debrisNorm, numchunks, DEBRIS_TINY, MAT_ROCK_FLESH, 0, 0, 0, SURF_BLOOD);
	}

	if (EntryPosIsValid)
	{
		AI_GORETEST_PRINTF("spraying blood from %2.2f %2.2f %2.2f; monster pos %2.2f %2.2f %2.2f\n",EntryPos[0],EntryPos[1],EntryPos[2],monster.s.origin[0],monster.s.origin[1],monster.s.origin[2]);
		FX_MakeBloodSpray(ExitPos, dir, 200, 1, &monster);
	}
	
	//for each of the front holes, scale the damage (this should cover massive headshot damage, and lower armshot damage)
	if (!(dflags&DT_MANGLE))		// If I'm mangling, do full damage.
	{
		float newtake, totaltake=0;

		for (i=0;i<numOfFrontCodes;i++)
		{
			frontCode=listOfFrontCodes[i];
			//make sure hole is valid
			if (frontCode >= 0 && frontCode < numGoreZones)
			{
				//delay the gibbing as long as we can...
				if (monster.health <= 0)
				{
					if (take > 0)
					{
						if (gzones[frontCode].cap)
						{
							totaltake += 3;
						}
						else
						{
							totaltake += 12;
						}
					}
				}
				else
				{
					newtake=(int)((float)take*gzones[frontCode].damScale);
					if (dm->isDM() && dm->dmRule_REALISTIC_DAMAGE() && dm->IsLimbZone(frontCode))
					{
						// can't kill a guy with limbshots in realstic DM
						newtake = min(newtake, (int)(monster.health	* .33));
					}
					totaltake += newtake;
				}
			}
			else
			{	// boy, this is not so good
				assert(false);
			}
		}

		// Average the damage out between the codes.  Multiple codes are mainly found for shotguns.  
		// Note that mangles don't use this code.  --Pat
		if (numOfFrontCodes>0)
		{
			take=totaltake/(float)numOfFrontCodes;
		}
	}

	AngleVectors(monster.s.angles, forward, right, up);
	VectorSubtract(point, monster.s.origin, to_impact);
	VectorNormalize(to_impact);

	Emote(monster, EMOTION_PAIN, 10.0);
				
	VectorSubtract(point, monster.s.origin, bodyloc);

	if (monster.health > 0 && take > monster.health+150)
	{
		take = monster.health + 150;
	}

	//DON'T ADJUST TAKE AFTER THIS POINT!!!!!!! I CHECK TO SEE IF I'M DEAD HERE!!!!!

	//dying--drop weapons
	if (monster.health <= take)
	{
		ggBinstC	*bolted;
		vec3_t		handPos;
		int			safeIters=0;
		Emote(monster, EMOTION_PAIN, 99999.0);
		VectorSet(handPos,0,0,0);

		// this is a little sad, but we can't drop boltons if we were hit by a train
		if (bDropBoltons)
		{
			// drop what's in left hand
			monster.ghoulInst->GetBoltMatrix(level.time,ZoneMatrix,monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"),IGhoulInst::MatrixType::Entity);
	//		EntToWorldMatrix(monster.s.origin, monster.s.angles, ZoneMatrix);
			ZoneMatrix.GetRow(3,zonePos);
			handPos[0]=zonePos[0];
			handPos[1]=zonePos[1];
			handPos[2]=zonePos[2];
			while (myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"))&&safeIters++<10)
			{
				bolted=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"));
				DropBoltOn(monster, bolted, zonePos, Direction, take);
			}

			//drop what's in right hand
			monster.ghoulInst->GetBoltMatrix(level.time,ZoneMatrix,monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r"),IGhoulInst::MatrixType::Entity);
	//		EntToWorldMatrix(monster.s.origin, monster.s.angles, ZoneMatrix);
			ZoneMatrix.GetRow(3,zonePos);
			handPos[0]=zonePos[0];
			handPos[1]=zonePos[1];
			handPos[2]=zonePos[2];
			safeIters=0;
			while (myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r"))&&safeIters++<10)
			{
				bolted=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r"));
				DropBoltOn(monster, bolted, zonePos, Direction, take);
			}

			//lose any headgear
			monster.ghoulInst->GetBoltMatrix(level.time,ZoneMatrix,monster.ghoulInst->GetGhoulObject()->FindPart("abolt_head_t"),IGhoulInst::MatrixType::Entity);
	//		EntToWorldMatrix(monster.s.origin, monster.s.angles, ZoneMatrix);
			ZoneMatrix.GetRow(3,zonePos);
			handPos[0]=zonePos[0];
			handPos[1]=zonePos[1];
			handPos[2]=zonePos[2];
			safeIters=0;
			while (myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("abolt_head_t"))&&safeIters++<10)
			{
				bolted=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("abolt_head_t"));
				DropBoltOn(monster, bolted, zonePos, Direction, take);
			}
		} // if (bDropBoltons)

		if (((dflags & DT_MANGLE) && (monster.health-take < -50)) || (monster.health-take < -750))
		{
			hackGlobalGibCalls++; // only up the Gib count if we're smashing living guys
			Gib(monster, dir, dflags);
			if(dflags & DT_FIRE)
			{	//fireballs
				Ignite(&monster, inflictor, 2);
			}
		}

		// right, then...we're dead. so find out how we died and, perhaps, silence our vocal cords.
		if (AreVocalCordsWorking())
		{
			if (dflags & DT_STEALTHY)
			{	// killed by a stealthy weapon so don't scream.
				SetVocalCordsWorking(false);
			}
		}

		// Shut off associated looping sounds too...
		monster.s.sound = 0;
		monster.s.sound_data = 0;
	}

	int		hasArmor = 0;

	if (monster.client)
	{
		hasArmor=(monster.client->inv->getArmorCount()>0);
	}

	for (i=0;i<numOfFrontCodes;i++)
	{
		frontCode=listOfFrontCodes[i];

		assert( (frontCode >= 0) && (frontCode < NUM_HUMANGOREZONES) );
//		if (!Skill_NoPain(&monster))
//		{
			// remember leg injury
			if (frontCode==GZ_LEG_LOWER_RIGHT || frontCode == GZ_FOOT_RIGHT ||
				frontCode == GZ_LEG_UPPER_RIGHT_FRONT || frontCode == GZ_LEG_UPPER_RIGHT_BACK)
			{
				bodyDamage = BODYDAMAGE_RIGHTLEG|BODYDAMAGE_RECENT;
				bodyDamageAccumulated |= BODYDAMAGE_RIGHTLEG;
				if (dm->dmRule_REALISTIC_DAMAGE())
				{	// reduce our movement due to leg damage
					incMovescale(&monster,0.1);
				}
				bodyDamageEndRecent = level.time + gi.flrand(5.0,7.5)*Skill_DisablityModifier(&monster);
			}	
			if (frontCode==GZ_LEG_LOWER_LEFT || frontCode == GZ_FOOT_LEFT ||
				frontCode == GZ_LEG_UPPER_LEFT_FRONT || frontCode == GZ_LEG_UPPER_LEFT_BACK)
			{
				bodyDamage = BODYDAMAGE_LEFTLEG|BODYDAMAGE_RECENT;
				bodyDamageAccumulated |= BODYDAMAGE_LEFTLEG;
				if (dm->dmRule_REALISTIC_DAMAGE())
				{	// reduce our movement due to leg damage
					incMovescale(&monster,0.1);
				}
				bodyDamageEndRecent = level.time + gi.flrand(5.0,7.5)*Skill_DisablityModifier(&monster);
			}	
			//remember arm injury too
			if (frontCode == GZ_ARM_UPPER_RIGHT || frontCode == GZ_ARM_LOWER_RIGHT)
			{
				bodyDamage = BODYDAMAGE_RIGHTARM|BODYDAMAGE_RECENT;
				bodyDamageAccumulated |= BODYDAMAGE_RIGHTARM;
				bodyDamageEndRecent = level.time + gi.flrand(5.0,7.5)*Skill_DisablityModifier(&monster);
			}
			if (frontCode == GZ_ARM_UPPER_LEFT || frontCode == GZ_ARM_LOWER_LEFT)
			{
				bodyDamage = BODYDAMAGE_LEFTARM|BODYDAMAGE_RECENT;
				bodyDamageAccumulated |= BODYDAMAGE_LEFTARM;
				bodyDamageEndRecent = level.time + gi.flrand(5.0,7.5)*Skill_DisablityModifier(&monster);
			}
//		}

		if (!oneLimbSevered)
		{
			// kef -- handle armor properly in case of throwing knife
			if (bArmor && bKnife)
			{
				DropKnife(monster, inflictor);
				// setting inflictor to NULL will prevent ShowFrontHoleDamage from sticking the knife in our boy. it
				//shouldn't affect anything else in that fn, though.
				inflictor = NULL;
			}
			if(!(dm->isDM()) || !hasArmor || (!(dm->dmRule_REALISTIC_DAMAGE()) || dm->IsUnprotectedGoreZone(frontCode)))
			{
//				We don't want to ShowFrontHoleDamage if we are in Realistic DM and we have armor on
				oneLimbSevered=ShowFrontHoleDamage (monster, frontCode, backCode, inflictor, attacker, dir, point, origin, take, knockback, dflags, mod);
			}
		}
	}

	if (!lock_gorezones)
	{
		ShowBackHoleDamage (monster, frontCode, backCode, inflictor, attacker, dir, ExitPos, origin, take, knockback, dflags, mod);
	}
	return take;
}


//void bodyhuman_c::Emote(edict_t &monster, emotion_index new_emotion, float emotion_duration, qboolean scripted_emoting)
//{
	//allow overriding of current emotion if: 1) new is higher priority, or 2) current emotion is about to expire anyway
//	if (new_emotion < emotion && emotion_expire-level.time > 0.25)
//	{
//		return;
//	}
//	emotion=new_emotion;
//	emotion_expire = level.time+emotion_duration;
//}

emotion_index bodyhuman_c::GetMood(edict_t &monster)
{
	return emotion;
}

//fixme: this could probably be better done with an array of ghoulid's, generated upon face setting
void bodyhuman_c::UpdateFace(edict_t &monster)
{
	char tempface[64];
	bool	foundAcceptableFace=false;//currently only using this for talking, so it's still fairly smooth at lower priorities.

	bodyorganic_c::UpdateFace(monster);

	if (curSoundIndex)
	{
		gi.sound (&monster, CHAN_VOICE, curSoundIndex, curVol, curAtten, 0);
		curSoundIndex=0;
	}

	//make sure the base for the face is valid
	if (!face_base[0])
	{
//		gi.dprintf("monster face not specified--no emotion!\n");

		//still breathe
		if (breathe_time < level.time)
		{
			if (monster.health <= 0)
			{
				breathe_time=level.time + 99999.9;
			}
			else
			{
				breathe_time=level.time + gi.flrand(2.5F, 4.5F);
			}

			lev_interp_code myInterpCode = GetInterpCode();
			if ((myInterpCode==LEVCODE_NYC_SUBWAY2)||(myInterpCode==LEVCODE_SIB_BASE)||
				(myInterpCode==LEVCODE_SIB_CANYON)||(myInterpCode==LEVCODE_SIB_PLANT))
			{
				fxRunner.exec("enemy/breathe", &monster, monster.ghoulInst->GetGhoulObject()->FindPart("sbolt_mouth"));
			}
		}

		return;
	}

	//determine current expression
	//fixme: allow guys to writhe around
	if (emotion==EMOTION_DEAD)//dead
	{
		strcpy(tempface,"d");
	}
	else if (emotion==EMOTION_PAIN)
	{
		strcpy(tempface,"p");
	}
	else if (emotion==EMOTION_MEAN)
	{
		strcpy(tempface,"m");
	}
	else if (emotion==EMOTION_AFRAID)
	{
		strcpy(tempface,"f");
	}
	//fix dis shit! just getting it all in and working first...
	else if (emotion==EMOTION_TALK)
	{
		if (!gi.irand(0,3))
		{
			Com_sprintf(tempface,64, "%snt3",face_base);
			if (monster.ghoulInst->GetGhoulObject()->FindSkin(monster.ghoulInst->GetGhoulObject()->FindMaterial("f"), tempface))
			{
				strcpy(tempface,"nt3");
				foundAcceptableFace=true;
			}
		}

		if (!foundAcceptableFace && !gi.irand(0,2))
		{
			Com_sprintf(tempface,64, "%snt2",face_base);
			if (monster.ghoulInst->GetGhoulObject()->FindSkin(monster.ghoulInst->GetGhoulObject()->FindMaterial("f"), tempface))
			{
				strcpy(tempface,"nt2");
				foundAcceptableFace=true;
			}
		}

		if (!foundAcceptableFace && !gi.irand(0,1))
		{
			Com_sprintf(tempface,64, "%snt1",face_base);
			if (monster.ghoulInst->GetGhoulObject()->FindSkin(monster.ghoulInst->GetGhoulObject()->FindMaterial("f"), tempface))
			{
				strcpy(tempface,"nt1");
				foundAcceptableFace=true;
			}
		}

		if (!foundAcceptableFace && blink_time<level.time)
		{
			blink_time=level.time + gi.flrand(3.0F, 5.0F);
			strcpy(tempface,"b");
		}

		if (!foundAcceptableFace)
		{
			strcpy(tempface,"n");
		}

//		switch (gi.irand(0,8))
//		{
//		case 0:
//			if (blink_time<level.time)
//			{
//				blink_time=level.time + gi.flrand(3.0F, 5.0F);
//				strcpy(tempface,"b");
//				break;
//			}
//		case 1:
//			strcpy(tempface,"n");
//			break;
//		default:
//			Com_sprintf(tempface, sizeof(tempface), "nt%d", gi.irand(1,3));
//			break;
//		}
	}
	else if (emotion==EMOTION_MEANTALK)
	{
		if (!gi.irand(0,3))
		{
			Com_sprintf(tempface,64, "%smt3",face_base);
			if (monster.ghoulInst->GetGhoulObject()->FindSkin(monster.ghoulInst->GetGhoulObject()->FindMaterial("f"), tempface))
			{
				strcpy(tempface,"mt3");
				foundAcceptableFace=true;
			}
		}

		if (!foundAcceptableFace && !gi.irand(0,2))
		{
			Com_sprintf(tempface,64, "%smt2",face_base);
			if (monster.ghoulInst->GetGhoulObject()->FindSkin(monster.ghoulInst->GetGhoulObject()->FindMaterial("f"), tempface))
			{
				strcpy(tempface,"mt2");
				foundAcceptableFace=true;
			}
		}

		if (!foundAcceptableFace && !gi.irand(0,1))
		{
			Com_sprintf(tempface,64, "%smt1",face_base);
			if (monster.ghoulInst->GetGhoulObject()->FindSkin(monster.ghoulInst->GetGhoulObject()->FindMaterial("f"), tempface))
			{
				strcpy(tempface,"mt1");
				foundAcceptableFace=true;
			}
		}

		if (!foundAcceptableFace)
		{
			strcpy(tempface,"m");
		}
	}
	else if (emotion==EMOTION_FEARTALK)
	{
		if (!gi.irand(0,3))
		{
			Com_sprintf(tempface,64, "%sft3",face_base);
			if (monster.ghoulInst->GetGhoulObject()->FindSkin(monster.ghoulInst->GetGhoulObject()->FindMaterial("f"), tempface))
			{
				strcpy(tempface,"ft3");
				foundAcceptableFace=true;
			}
		}

		if (!foundAcceptableFace && !gi.irand(0,2))
		{
			Com_sprintf(tempface,64, "%sft2",face_base);
			if (monster.ghoulInst->GetGhoulObject()->FindSkin(monster.ghoulInst->GetGhoulObject()->FindMaterial("f"), tempface))
			{
				strcpy(tempface,"ft2");
				foundAcceptableFace=true;
			}
		}

		if (!foundAcceptableFace && !gi.irand(0,1))
		{
			Com_sprintf(tempface,64, "%sft1",face_base);
			if (monster.ghoulInst->GetGhoulObject()->FindSkin(monster.ghoulInst->GetGhoulObject()->FindMaterial("f"), tempface))
			{
				strcpy(tempface,"ft1");
				foundAcceptableFace=true;
			}
		}

		if (!foundAcceptableFace)
		{
			strcpy(tempface,"f");
		}
	}
	else if (blink_time<level.time)
	{
//		gi.dprintf("blink!\n");
		blink_time=level.time + gi.flrand(3.0F, 5.0F);
		strcpy(tempface,"b");
	}
	else
	{
/*		if (level.time < nextGreetTime-7)
		{
			Com_sprintf(tempface, sizeof(tempface), "nt%d", gi.irand(1,3));
		}
		else
*/		{
			strcpy(tempface,"n");
		}
	}

	//check if expression has changed; if so, swap pages
	if (strcmp(tempface,curface))
	{
		strcpy(curface,tempface);
		Com_sprintf(tempface,64, "%s%s",face_base,curface);

		if(!monster.ghoulInst->SetFrameOverride("f",tempface))
		{
//			gi.dprintf("ACK! monster face skin %s not found!\n", tempface);
			//default to normal face
			Com_sprintf(tempface, 64, "%sn",face_base);
			monster.ghoulInst->SetFrameOverride("f",tempface);
		}
	}

	if (breathe_time < level.time)
	{
		switch(emotion)
		{
		case EMOTION_DEAD:
			breathe_time=level.time + 99999.9;
			return;//i'm dead--no breathing!
		case EMOTION_PAIN:
			breathe_time=level.time + gi.flrand(1.0F, 1.5F);
			break;
		case EMOTION_MEAN:
			breathe_time=level.time + gi.flrand(1.25F, 2.0F);
			break;
		case EMOTION_AFRAID:
			breathe_time=level.time + gi.flrand(1.0F, 1.5F);
			break;
		default:
			breathe_time=level.time + gi.flrand(2.5F, 4.5F);
			break;
		}

		//heh. this should be replaced with something clever.
//		if (level.mapname[0]=='s'&& level.mapname[1]=='i'&& level.mapname[2]=='b')
		lev_interp_code myInterpCode = GetInterpCode();
		if ((myInterpCode==LEVCODE_NYC_SUBWAY2)||(myInterpCode==LEVCODE_SIB_BASE)||
			(myInterpCode==LEVCODE_SIB_CANYON)||(myInterpCode==LEVCODE_SIB_PLANT))
		{
			fxRunner.exec("enemy/breathe", &monster, monster.ghoulInst->GetGhoulObject()->FindPart("sbolt_mouth"));
		}
	}
}

void bodyhuman_c::SetFace(edict_t &monster, char *face)
{
	strcpy(face_base, face);
}

void bodyhuman_c::PrecacheGore(edict_t &monster)
{
	ggObjC	*cacheObj;
	//cache my own personal half-head
	if (monster.ghoulInst)
	{
		GhoulID myHead=monster.ghoulInst->GetFrameOverride("h");
		if (myHead)
		{
			char headskin[256];
			monster.ghoulInst->GetGhoulObject()->GetSkinName(myHead,headskin);
			cacheObj=game_ghoul.FindObject("Enemy/bolt", "g_half_head", false, headskin);
			cacheObj->RegisterSkin("h", headskin);
			if (cacheObj)
			{
				char faceskin[256];
				cacheObj->RegisterSkin("gz", "gz_half_head");
				//i have a changeable face
				if (face_base[0])
				{
					Com_sprintf(faceskin, 256, "%sd", face_base);
					cacheObj->RegisterSkin("f", faceskin);
				}
				//no changeable face--register whatever static face i got
				else
				{
					GhoulID myFace=monster.ghoulInst->GetFrameOverride("f");
					if (myFace)
					{
						monster.ghoulInst->GetGhoulObject()->GetSkinName(myFace,faceskin);
						cacheObj->RegisterSkin("f", faceskin);
					}
				}
				cacheObj->RegistrationLock();
			}
		}
	}
}

void bodyhuman_c::SetRootBolt(edict_t &monster)
{
	GhoulID	tempID;
	GhoulID tempBolt;
	GhoulID	tempNote;

	char buffer[256];
	int tindex;

	PrecacheGore(monster);

	//cache sounds
	for (tindex = 1; tindex < 4; tindex++)
	{
		Com_sprintf(buffer, 256, "impact/gore/fall%d.wav", tindex);
		gi.soundindex (buffer);
	}
	for (tindex = 1; tindex < 4; tindex++)
	{
		Com_sprintf(buffer, 256, "impact/gore/impact%d.wav", tindex);
		gi.soundindex (buffer);
	}
	gi.soundindex ("impact/gore/limbfall.wav");
//	gi.soundindex ("impact/gore/spout.wav"); - sound is now embedded in effect
	gi.soundindex ("impact/gore/sizzle.wav");
	gi.soundindex ("impact/gore/hitflesh.wav");
	gi.soundindex ("impact/gore/headexp.wav");

	VoiceGeneralSoundCache("Burn",monster,1);
	VoiceGeneralSoundCache("Choke",monster,2);
	VoiceGeneralSoundCache("Crotch",monster,2);
	VoiceGeneralSoundCache("Hit",monster,3);
	VoiceGeneralSoundCache("Scream",monster,SCREAMNUMBER);
	VoiceGeneralSoundCache("Writhe",monster,WRITHENUMBER);
//	VoiceGeneralSoundCache("Fly",monster,2);
	VoiceGeneralSoundCache("Limp",monster,3);
	VoiceGeneralSoundCache("Getup",monster,1);
	VoiceGeneralSoundCache("Disarm",monster,1);

//	VoiceSoundCache("backup", monster, 0);
//	VoiceSoundCache("throw", monster, 0);
//	VoiceSoundCache("duck", monster, 0);
//	VoiceSoundCache("help", monster, 0);
//	VoiceSoundCache("retreat", monster, 0);
//	VoiceSoundCache("reload", monster, 0);

	VoiceWakeSoundCache(monster);
	VoiceGreetSoundCache(monster);

	entDebrisToCache[CLGHL_CHUNKROCKTINY] = DEBRIS_YES;

	
	gi.soundindex ("Weapons/gren/throw.wav");
	gi.soundindex ("impact/armor/hit.wav");

	gi.effectindex("gore/stumpblood");
	gi.effectindex("gore/headcloud");
	gi.effectindex("gore/bloodcloud");
	gi.effectindex("gore/bloodcloud_s");
	gi.effectindex("gore/noblood");
	gi.effectindex("gore/noblood_s");
	gi.effectindex("gore/gibbody");
	gi.effectindex("spark2");
	gi.effectindex("weakspark");
	gi.effectindex("armorhit");
	gi.effectindex("gore/bloodpool");
	gi.effectindex("gore/danglyblood");
	gi.effectindex("gore/hugeburst2");
	gi.effectindex("environ/gb_exitw");

	if (level.mapname[0]=='s'&& level.mapname[1]=='i'&& level.mapname[2]=='b')
	{
		gi.effectindex("enemy/breathe");
	}

	CacheAttack(GetLeftHandWeapon(monster));
	CacheAttack(GetRightHandWeapon(monster));

	AllocateGoreZones(monster);

	if (!monster.ghoulInst)
	{
		return;
	}

	RecognizeGoreZones(monster);

	//going to allow players to run callback routines, i think...
	if (monster.client)
	{
		tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("EOS");
		if (tempNote)
		{
			monster.ghoulInst->AddNoteCallBack(&TheMonsterSeqEndCallback,tempNote);
		}
	}
	else
	{
		tempBolt=monster.ghoulInst->GetGhoulObject()->FindPart("quake_origin");
		if (tempBolt)
		{
			monster.ghoulInst->AddMatrixCallBack(&TheFollowGenericPivot,tempBolt,IGhoulInst::MatrixType::JacobianEntity);
		}
		else
		{
			gi.dprintf("WARNING: monster origin not found--wacky!!\n");
		}

		tempBolt=monster.ghoulInst->GetGhoulObject()->FindPart("sbolt_heel_r");
		if (tempBolt)
		{
			monster.ghoulInst->AddMatrixCallBack(&TheFootRightCallback,tempBolt,IGhoulInst::MatrixType::Entity);
		}

		tempBolt=monster.ghoulInst->GetGhoulObject()->FindPart("sbolt_heel_l");
		if (tempBolt)
		{
			monster.ghoulInst->AddMatrixCallBack(&TheFootLeftCallback,tempBolt,IGhoulInst::MatrixType::Entity);
		}

		tempID=monster.ghoulInst->GetGhoulObject()->FindPart("quake_ground");
		if (tempID)
		{
			monster.ghoulInst->AddMatrixCallBack(&TheGroundCallback,tempID,IGhoulInst::MatrixType::Entity);
		}
		tempBolt=monster.ghoulInst->GetGhoulObject()->FindPart("sbolt_mouth");
		if (tempBolt)
		{
			monster.ghoulInst->AddMatrixCallBack(&TheMouthCallback,tempBolt,IGhoulInst::MatrixType::Entity);
		}
		else
		{
			gi.dprintf("WARNING: monster mouth not found--sight will be defective!!\n");
		}

		tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("BOS");
		if (tempNote)
		{
			monster.ghoulInst->AddNoteCallBack(&TheMonsterSeqBeginCallback,tempNote);
		}

		tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("EOS");
		if (tempNote)
		{
			monster.ghoulInst->AddNoteCallBack(&TheMonsterSeqEndCallback,tempNote);
		}

		tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("jump");
		if (tempNote)
		{
			monster.ghoulInst->AddNoteCallBack(&TheJumpCallBack,tempNote);
		}

		tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("thud");
		if (tempNote)
		{
			monster.ghoulInst->AddNoteCallBack(&TheThudCallBack,tempNote);
		}

		tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("knee");
		if (tempNote)
		{
			monster.ghoulInst->AddNoteCallBack(&TheKneeCallBack,tempNote);
		}

		tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("inair");
		if (tempNote)
		{
			monster.ghoulInst->AddNoteCallBack(&TheInAirCallBack,tempNote);
		}

		tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("fire");
		if (tempNote)
		{
			monster.ghoulInst->AddNoteCallBack(&TheFireCallBack,tempNote);
		}

		tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("sound");
		if (tempNote)
		{
			monster.ghoulInst->GetGhoulObject()->FireAllNoteCallBacks(&soundPrecacher,tempNote);
			monster.ghoulInst->AddNoteCallBack(&TheMonsterSoundCallBack,tempNote);
		}

		tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("throw");
		if (tempNote)
		{
			monster.ghoulInst->AddNoteCallBack(&TheThrowCallBack,tempNote);
		}

		tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("dropweapon");
		if (tempNote)
		{
			monster.ghoulInst->AddNoteCallBack(&TheDropWeaponCallBack,tempNote);
		}

		ReloadRWeapon(monster);
	}
}

attacks_e bodyhuman_c::GetWeaponAtBolt(edict_t &monster, char *boltname)
{
	GhoulID bolt;
	ggBinstC *weapon;

	if (!monster.ghoulInst)
	{
		return ATK_NOTHING;
	}
	bolt = monster.ghoulInst->GetGhoulObject()->FindPart(boltname);
	weapon = game_ghoul.FindOInst(monster.ghoulInst)->GetBoltInstance(bolt);
	return GetWeaponAttackType(weapon);
}

attacks_e bodyhuman_c::GetRightHandWeapon(edict_t &monster)
{
/*	GhoulID bolt=0;
	ggBinstC *weapon=NULL;

	if (monster.client)
	{
		bolt = monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r");
		weapon = game_ghoul.FindOInst(monster.ghoulInst)->GetBoltInstance(bolt);
		if (!weapon)
		{
			return ATK_NOTHING;
		}
	}
*/
	return rHandAtk;
//	return GetWeaponAtBolt(monster, "wbolt_hand_r");
}

attacks_e bodyhuman_c::GetLeftHandWeapon(edict_t &monster)
{
	return lHandAtk;
	//return GetWeaponAtBolt(monster, "wbolt_hand_l");
}

attacks_e bodyhuman_c::GetBestWeapon(edict_t &monster)
{
	attacks_e left, right;
	left = GetLeftHandWeapon(monster);
	right = GetRightHandWeapon(monster);
	if (left>right)
	{
		return left;
	}
	return right;
}

qboolean bodyhuman_c::RightHandWeaponReady(edict_t &monster)
{
	if(GetRightHandWeapon(monster) == ATK_MICROWAVE && rLastFireTime < 0)
	{
		rLastFireTime = level.time - 1;//make 'em pause
		return 0;
	}
	//return (level.time-rLastFireTime > FireInterval(GetRightHandWeapon(monster)));
	return (level.time-rLastFireTime >
		((FireInterval(GetRightHandWeapon(monster),rWeaponAmmo) 
		* game.playerSkills.getHesitation()) 
		* ((bodyDamage) ? 2:1)
		* ((gmonster.GetClosestEnemy() == &monster) ? .75:1)));//closest guy gets to attack extra quickly
}

void bodyhuman_c::DropWeapon(edict_t *ent, IGhoulInst *me)
{
	vec3_t	fwd;
	Vect3		vPos, vDir;

	rHandAtk = ATK_NOTHING;

	if (!ent || !ent->ghoulInst)
	{
		return;
	}
	me=ent->ghoulInst;
	ggBinstC	*oldLeftWeap = NULL, *oldRightWeap = NULL;
	ggOinstC	*myInstance = game_ghoul.FindOInst(me);
	GhoulID		idLeft = NULL_GhoulID, idRight = NULL_GhoulID;
	
	if (NULL == myInstance)
	{
		return;
	}
	AngleVectors(ent->s.angles, fwd, NULL, NULL);
	VectorNegate(fwd, *(vec3_t*)&vDir);
	if(oldLeftWeap = myInstance->GetBoltInstance(idLeft = me->GetGhoulObject()->FindPart("wbolt_hand_l")))
	{	// get the position of the bolted-on item
		GetGhoulPosDir(ent->s.origin, ent->s.angles, oldLeftWeap->GetInstPtr(), NULL, "TO_WBOLT_HAND_L",
			*(vec3_t*)&vPos, fwd, NULL, NULL);
		// drop it
		DropBoltOn(*ent, oldLeftWeap, vPos, vDir, 2);
	}
	if (oldRightWeap = myInstance->GetBoltInstance(idRight = me->GetGhoulObject()->FindPart("wbolt_hand_r")))
	{	// get the position of the bolted-on item
		GetGhoulPosDir(ent->s.origin, ent->s.angles, oldRightWeap->GetInstPtr(), NULL, "TO_WBOLT_HAND_R",
			*(vec3_t*)&vPos, fwd, NULL, NULL);
		// drop it
		DropBoltOn(*ent, oldRightWeap, vPos, vDir, 2);
	}
}


// defines for null targeting
#define HEAD		1
//#define NECK		2
#define LSHOULDER	3
#define RSHOULDER	4
#define CHEST		5
#define GUT			6
#define GROIN		7
#define GUNHAND		8
#define NUG_O_POPPIN		9

#define FLAMETHROW_MAX_WEDGE 0

void bodyhuman_c::FireRightHandWeapon(edict_t &monster, bool isLethal, int NullTarget, qboolean forceFireRate)
{
	vec3_t forward;
	Matrix4 firebolt,rotate,final;
	edict_t *targ;

	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);

	if(!the_ai)
	{
		return;
	}

	float pausing = ((FireInterval(GetRightHandWeapon(monster),rWeaponAmmo) * game.playerSkills.getHesitation()) * ((bodyDamage) ? 2.5:1)* ((gmonster.GetClosestEnemy() == &monster) ? .5:1));

	if (forceFireRate && level.time-rLastFireTime <
		((FireInterval(GetRightHandWeapon(monster),rWeaponAmmo) * game.playerSkills.getHesitation()) * ((bodyDamage) ? 2.5:1)* ((gmonster.GetClosestEnemy() == &monster) ? .5:1)))
	{	//injured people fire a lot less often because, well, they're injured and very unhappy
		return;
	}

	IGhoulInst *my_inst = monster.ghoulInst;
	if (!my_inst)
	{
		return;
	}

	GhoulID my_hand = monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r");

	if (!my_hand)
	{
		return;
	}

	ggBinstC *my_weapon = game_ghoul.FindOInst(monster.ghoulInst)->GetBoltInstance(my_hand);

	if (!my_weapon)
	{
		return;
	}

	GhoulID myFlash = my_weapon->GetBolteeObject()->GetMyObject()->FindPart("flash");
	if (!myFlash)
	{
		return;
	}

	targ = the_ai->getTarget();

	rWeaponAmmo-=1;

	//this ensures that it's from exactly the right spot (particularly height wise) - but what about the bbox?  It must be inside!

	vec3_t fpos;
	//GetGhoulPosDir(monster.s.origin, monster.s.angles, my_weapon->GetInstPtr(), NULL, "flash", fpos, fdir, NULL, NULL);
	VectorCopy(monster.s.origin, fpos);
	fpos[2] += MONSTER_SHOOT_HEIGHT;

	int rWeap = GetRightHandWeapon(monster);
	
	if ((isLethal || NullTarget) && targ) 
	{
		vec3_t	TargetPoint;
		
		VectorCopy(targ->s.origin, TargetPoint);
		if (NullTarget) // Try to get a null's point
		{
			Matrix4	EntityToWorld;
			Matrix4	BoltToEntity;
			Matrix4	BoltToWorld;
			GhoulID	Bolt;

			switch (NullTarget)
			{
			case HEAD: // fixme rearrange this so switch only assigns correct bolt
				Bolt = targ->ghoulInst->GetGhoulObject()->FindPart("sbolt_mouth");
				break;
//				case NECK:
//					Bolt = targ->ghoulInst->GetGhoulObject()->FindPart("gbolt_cap_head_chest"); // don't think this works
//					break;
			case LSHOULDER:
				Bolt = targ->ghoulInst->GetGhoulObject()->FindPart("gbolt_shoulder_lf");
				break;
			case RSHOULDER:
				Bolt = targ->ghoulInst->GetGhoulObject()->FindPart("gbolt_shoulder_rf");
				break;
			case CHEST:
				Bolt = targ->ghoulInst->GetGhoulObject()->FindPart("gbolt_chest_f");
				break;
			case GROIN:
				Bolt = targ->ghoulInst->GetGhoulObject()->FindPart("gbolt_groin");
				break;
			case GUT:
				Bolt = targ->ghoulInst->GetGhoulObject()->FindPart("gbolt_gut_f");
				break;
			case GUNHAND:
				Bolt = targ->ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r");
				DropWeapon(targ, targ->ghoulInst);
				break;
			case NUG_O_POPPIN: // this means explode the head like a melon
				Bolt = targ->ghoulInst->GetGhoulObject()->FindPart("sbolt_mouth");
				break;
			default:
				break;
			}
			if (Bolt && NullTarget)
			{
				// get the transformation from entity space to world space
				EntToWorldMatrix(targ->s.origin, targ->s.angles, EntityToWorld);
				// get the transformation from bolt space to entity space
				targ->ghoulInst->GetBoltMatrix(level.time,BoltToEntity,Bolt,IGhoulInst::MatrixType::Entity);
				// multiply to get transform from bolt space to world space
				BoltToWorld.Concat(BoltToEntity, EntityToWorld);
				// 3rd row of BoltToWorld is the world coordinates of the bolt
				BoltToWorld.GetRow(3,*(Vect3 *)TargetPoint);
			}
		}
		VectorSubtract(TargetPoint, fpos, forward); // firing vector will be targetted point (defaults to origin)

		gi.dprintf("attacking, targetpoint: %2.3f %2.3f %2.3f\n", TargetPoint[0], TargetPoint[1], TargetPoint[2]);
			
		ai_c *target_ai=(ai_c*)((ai_public_c*)targ->ai);
		bodyorganic_c* targetbody = NULL;
		if (isLethal && target_ai)
		{
//				target_ai->GetBody()->SetArmor(*the_ai->getTarget(), ARMOR_NONE); // turn target's armor off on a scripted kill		

			// rather than turn a guy's armor off when he's scripted to die, we'll set his
			//"next shot's gonna kill me" flag
			if ( targetbody = ((bodyorganic_c*)target_ai->GetBody()) )
			{
				if (NullTarget == NUG_O_POPPIN)
				{
					targetbody->NextShotsGonnaKillMe(NEXTSHOT_EXPLODE_MY_NUG); // thought I was kidding about the name, didn't you?
				}
				else
				{
					targetbody->NextShotsGonnaKillMe(NEXTSHOT_KILLS); // thought I was kidding about the name, didn't you?
				}
			}
		}
	}
	else
	{
		if(targ)
		{	//because shooting out of your back, while being an extremely useful talent,
			// is not something our guys can do.

			vec3_t to_targ,my_facing;
			VectorSubtract(targ->s.origin, monster.s.origin, to_targ);
			to_targ[2] = 0;
			VectorNormalize(to_targ);

			AngleVectors(monster.s.angles, my_facing, NULL, NULL);
			my_facing[2] = 0;
			VectorNormalize(my_facing);
			
			if ((!game.cinematicfreeze)&&
				((DotProduct(to_targ, my_facing)<MAX_FIRE_WEDGE && rWeap != ATK_FLAMEGUN)||(DotProduct(to_targ, my_facing)<FLAMETHROW_MAX_WEDGE))&&
				(!(the_ai->GetMiscFlags() & MISC_FLAG_FIRINGBLIND)))//fixme - does this vary according to the bbox?
			{
				rWeaponAmmo+=1;//didn't use it after all
				return;
			}
		}

		if(currentmove->actionFlags&ACTFLAG_FIREFROMGUN)
		{
			GetGhoulPosDir(monster.s.origin, monster.s.angles, my_weapon->GetInstPtr(), NULL, "flash", fpos, forward, 0, 0);
		}
		else
		{
			the_ai->GetAimVector(forward);
		}
	}

	justFiredRight = true;

	if(targ && (!(the_ai->GetMiscFlags() & MISC_FLAG_FIRINGBLIND)))
	{
		if((rWeap == ATK_MICROWAVE)||(rWeap == ATK_MICROWAVE_ALT)
			||(rWeap == ATK_ROCKET)||(rWeap == ATK_FLAMEGUN)||(rWeap == ATK_AUTOSHOTGUN))
		{	//projectiles shouldn't miss - they look really bad and are also poor for gameplay
			vec3_t	targOrigin;
			VectorCopy(targ->s.origin, targOrigin);
			// kef -- there's probably a better way of altering our target position based on the
			//target's crouchingness, but...
			if (targ && targ->client && (targ->client->ps.pmove.pm_flags & PMF_DUCKED))
			{
				targOrigin[2] -= 16;
			}

			VectorSubtract(targOrigin, fpos, forward);
			forward[2] = targOrigin[2] - monster.s.origin[2];
			VectorNormalize(forward);
		}
		else if((rWeap == ATK_SHOTGUN))
		{	// so this doesn't miss by nearly as much as most other weapons
			vec3_t fd;
			VectorScale(forward, .6, forward);
			VectorSubtract(targ->s.origin, fpos, fd);
			VectorNormalize(fd);
			VectorMA(forward, .4, fd, forward);
		}
	}

	if(targ)
	{	//wake my pals up
		PlayerNoise(targ, targ->s.origin, AI_SENSETYPE_SOUND_WEAPON, NULL, 300, gmonster.GetClientNode());
	}

	if(the_ai->GetMiscFlags() & MISC_FLAG_FIRINGBLIND)
	{
		forward[0] += gi.flrand(-.15, .15);
		forward[1] += gi.flrand(-.15, .15);
		forward[2] += gi.flrand(-.15, .15);

		VectorNormalize(forward);
	}

	weapons.attack(GetRightHandWeapon(monster), &monster, fpos, forward, my_weapon->GetInstPtr(), myFlash);

	if(targ && targ->client)
	{	// we know what to do here!
		SetActionMusic(*targ);
	}

	//hey! if e's not dead yet and i wanna kill im, finish im off!
	if (isLethal && targ && targ->health>0)
	{
//			weapons.attack(ATK_MAGICBULLET, &monster, fpos, forward, my_weapon->GetInstPtr());
	}

	rLastFireTime = level.time;

//	Com_Printf("A: %8.3f   B: %8.3f\nC: %8.3f   D: %8.3f\n",
//		set1Time/1000.0, set2Time/1000.0, set3Time/1000.0, set4Time/1000.0);
}

#define THROW_UP (30*M_PI/180)
#define MAX_THROW_ANG 80
#define THROW_ERROR 20
#define MIN_THROW_DIST 200
#define THROWCONE .707
// if we throw a bit high it looks better

void bodyhuman_c::ThrowProjectile(edict_t &monster)
{
	// need a bunch more checks in here and stuff

	attacks_e	myLeftHand = GetLeftHandWeapon(monster);

	if (myLeftHand == ATK_MACHETE || myLeftHand == ATK_NOTHING)
	{
		return;
	}

	vec3_t fwd;

	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);

	if(!the_ai)
	{
		return;
	}
	
	if(!the_ai->getTarget())return;

	vec3_t dif;
	VectorSubtract(the_ai->getTarget()->s.origin, monster.s.origin, dif);
	if(myLeftHand != ATK_THROWSTAR && VectorLength(dif) < MIN_THROW_DIST)
	{	// screw this!
		return;
	}

	CRadiusContent rad(the_ai->getTarget()->s.origin, 175, 1, 1);

	for(int i = 0; i < rad.getNumFound(); i++)
	{	// don't throw now!
		edict_t *search = rad.foundEdict(i);

		if (myLeftHand == ATK_THROWSTAR && search == &monster)
			continue;

		if(OnSameTeam(&monster, search))
		{
			return;
		}
	}

	vec3_t fpos;
	float vel;

	GetGhoulPosDir(monster.s.origin, monster.s.angles, monster.ghoulInst, NULL, "wbolt_hand_l", fpos, 0, 0, 0);

	the_ai->GetAimVector(fwd);

	fwd[2] = 0;
	VectorNormalize(fwd);//we are going to just grab the z component here

	//throwing stars go straight, everything else gets lobbed
	if (myLeftHand == ATK_THROWSTAR)
	{
		vel = 500;
		VectorCopy(monster.s.origin, fpos);
		VectorSubtract(the_ai->getTarget()->s.origin, fpos, fwd);

		VectorNormalize(fwd);
	}
	else
	{
		vec3_t temp;
		VectorSubtract(the_ai->getTarget()->s.origin, monster.s.origin, temp);
		temp[2] = 0;

		VectorNormalize(temp);

		if(DotProduct(temp, fwd) < THROWCONE)
		{
			return;
		}

		VectorScale(temp, 2, temp);
		VectorAdd(fwd, temp, fwd);
		VectorNormalize(fwd);

		vec3_t dist;

		float hDif = the_ai->getTarget()->s.origin[2] - monster.s.origin[2] + gi.flrand(0, THROW_ERROR);
		VectorSubtract(the_ai->getTarget()->s.origin, monster.s.origin, dist);
		float distf = VectorLength(dist);
		float throwAng = asin(hDif/distf) + THROW_UP;
		if(throwAng > MAX_THROW_ANG*M_PI/180)throwAng = MAX_THROW_ANG*M_PI/180;//irk

		float throwFwd = cos(throwAng);
		float throwUp = sin(throwAng);

		VectorScale(fwd, throwFwd, fwd);
		fwd[2] = throwUp;//make this go at a good angle

		vel = sqrt((-800*-800*-1*distf*distf)/(-2*hDif*-800*throwFwd*throwFwd + 2*-800*distf*throwUp*throwFwd));
	}

	weapons.attack(GetLeftHandWeapon(monster), &monster, fpos, fwd, 0, &vel);

	gi.sound (&monster, CHAN_AUTO, gi.soundindex ("Weapons/gren/throw.wav"), .6, ATTN_NORM, 0);
}

/*
// Don't block any longer
void boltDropNotSolid(edict_t *self)
{
	if (!(strncmp (self->classname, "item_weapon", 11)==0))	// Non-weapons aren't solid
	{
		self->movetype = MOVETYPE_NONE;
		self->solid = SOLID_NOT;
	}
	self->think = blownpartremove_think;
	self->nextthink = level.time + 30.0;

}
*/

// Don't block any longer
void boltDropNotSolid(edict_t *self)
{
	self->touch_debounce_time = level.time;
	self->think = blownpartremove_think;
	self->nextthink = level.time + 30.0;

	if (!(strncmp (self->classname, "item_weapon", 11)==0))	// Non-weapons aren't solid
	{
		self->movetype = MOVETYPE_NONE;
		self->solid = SOLID_NOT;
		self->nextthink = level.time + 0.1;
	}
}

int bodyhuman_c::DropBoltOn(edict_t &monster, ggBinstC *boltedptr, Vect3 &pos, Vect3 &Direction, int damage, edict_t *attacker)
{
	weapons_t boltedWeapon = GetWeaponType(boltedptr);
	Pickup	*pickup = NULL;
	GhoulID inworldBolt;
	edict_t *ent;
	vec3_t shootOut;
	vec3_t dir;
	int i,weaponFlg;
	char *boltedname;

	ggOinstC *myInstance=game_ghoul.FindOInst(monster.ghoulInst);
	if (!myInstance)
	{
		return damage;
	}

	ggObjC		*myObject=myInstance->GetParentObject();

	if (!myObject)
	{
		return damage;
	}

	if (!bDropBoltons)
	{	// we're not allowed to drop stuff (probably cuz we were hit by a train)
		return damage;
	}

	for (i=0;i<3;i++)
	{
		dir[i]=Direction[i];
	}

	if (!boltedptr || !boltedptr->GetBolteeObject())
	{
		return damage;
	}

	if(monster.client)
	{
		// Players can only drop weapons. And don't do this when dying - in
		// that case, it *needs* to be done elsewhere.

		if(dm->dmRule_REALISTIC_DAMAGE() && attacker && attacker->client/*&&(monster.health>damage)*/)
		{
			// Some weapons are not as likely to cause weapondrops
			switch(attacker->client->inv->getCurWeaponType())
			{
			case SFW_PISTOL1:
			case SFW_PISTOL2:
			case SFW_SNIPER:
				break; // always cause a drop with these weapons
			case SFW_SHOTGUN:
			case SFW_MACHINEPISTOL:
			case SFW_ASSAULTRIFLE:
			case SFW_MACHINEGUN:
			case SFW_AUTOSHOTGUN:
				if (gi.flrand(0,1) < .8)
				{
					return (damage * gi.flrand(0,1));
				}
				break; // much reduced (20%) chance to cause drop with these weapons, some damage is also absorbed by the weapon
			case SFW_KNIFE:
			case SFW_ROCKET:
			case SFW_FLAMEGUN:
			case SFW_MICROWAVEPULSE:
			default:
				return damage;
				break; // these weapons never cause drops
			}
			
			
			// Can only shoot weapon out of player's hands - anything else, just return.

			boltedname = boltedptr->GetBolteeObject()->GetSubName();

			char putBoltNameHere[100];

			monster.ghoulInst->GetGhoulObject()->GetPartName(boltedptr->GetBolterBolt(),putBoltNameHere);
			
			if(!stricmp(putBoltNameHere,"wbolt_hand_r") && dm->IsShootableWeapon(boltedname))
			{
				monster.client->inv->addCommand("weaponlose");
				return 0;
			}
		}

		return(damage);
	}
	
	if(strcmp(monster.classname, "m_x_mmerc") == 0)
	{
		// Cant' shoot stuff off of Hawk. and I think he's supposed to be invincible, too.

		return 0;
	}

	boltedname = boltedptr->GetBolteeObject()->GetSubName();

	for (i=0;*shootableParts[i].name;i++)
	{
		if (!stricmp(shootableParts[i].name,boltedname))
		{
			if ((shootableParts[i].flimsy)||(damage>1))
			{
				// If it's a weapon they spawn a little different - they need touch functions.
				weaponFlg = false;
				if (boltedWeapon!=SFW_EMPTYSLOT)
				{
					if (pickup = thePickupList.GetPickupFromType(PU_WEAPON, boltedWeapon))
					{
						if (boltedptr->GetBolterBolt())
						{
							char putBoltNameHere[100];

							monster.ghoulInst->GetGhoulObject()->GetPartName(boltedptr->GetBolterBolt(),putBoltNameHere);
							if (!stricmp(putBoltNameHere, "wbolt_hand_r"))
							{
								rHandAtk = ATK_NOTHING;
							}
							else if (!stricmp(putBoltNameHere, "wbolt_hand_l"))
							{
								lHandAtk = ATK_NOTHING;
							}
						}
						weaponFlg = true;
						// if anyone has the "no_wound" spawnflag don't let 'em drop weapons
						if (monster.spawnflags & SPAWNFLAG_NO_WOUND)
						{
							return damage;
						}
					}
				}
				// Spawn the thingy.
				ent = G_Spawn();
				VectorCopy(monster.s.origin,ent->s.origin);
				ent->owner = &monster;

				boltedptr->GetBolteeObject()->CloneInstance(ent, boltedptr->GetInstPtr());
				ent->s.renderfx = RF_GHOUL;

				inworldBolt=boltedptr->GetBolteeObject()->GetMyObject()->FindPart("inworld");
				if (inworldBolt)
				{
					ent->ghoulInst->SetMyBolt(inworldBolt);
				}
				else
				{
					ent->ghoulInst->SetMyBolt(boltedptr->GetInstPtr()->GetMyBolt());
				}

				if (weaponFlg)
				{
					I_Spawn (ent, pickup);
				}

				//move it!
				ent->velocity[2]+=300;
				VectorScale(dir,150,ent->velocity);
				VectorSubtract(*(vec3_t*)(&pos), monster.s.origin, shootOut);
				VectorNormalize(shootOut);
				VectorScale(shootOut, 150, shootOut);
				VectorAdd(ent->velocity, shootOut, ent->velocity);

				// Spawn at a random angle, so we don't get nice rows of boltons.
				ent->s.angles[YAW] = gi.flrand(-180, 180);

				// Was item already spawned by I_Spawn
				if (!weaponFlg)
				{
					ent->movetype = MOVETYPE_BOUNCE;
					ent->solid = SOLID_BBOX;
					ent->clipmask = MASK_MONSTERSOLID|MASK_PLAYERSOLID; 
					VectorSet (ent->mins, -1, -1, -1);
					VectorSet (ent->maxs, 1, 1, 1);
					ent->health = 10000;
					ent->takedamage = DAMAGE_NO;
					BboxRotate(ent);
					gi.linkentity (ent);
				}

				ent->nextthink = level.time + 5;
				ent->think = boltDropNotSolid;

				myInstance->RemoveBoltInstance(boltedptr);
				
				if(shootableParts[i].hatHead) // we're losing a hat, make sure we've got a good
					// head on our shoulders
				{
					GhoulID newHeadType = 0;
					GhoulID	headType = 0;
					GhoulID newFaceType = 0;
					GhoulID faceType = 0;
					
					headType = monster.ghoulInst->GetGhoulObject()->FindPart("_headhelmet");
			// hey, guess what, ecto guys with helmets have _headhelmets and meso guys
			// have _helmetheads!  Yeah, artists!
			// Also, here's where we take care of swapping gore zones.
					if (headType && monster.ghoulInst->GetPartOnOff(headType)) // got an ecto helmet
					{
						newHeadType = monster.ghoulInst->GetGhoulObject()->FindPart("_headbald");
						if (newHeadType)
						{
							monster.ghoulInst->SetPartOnOff(headType, false);
							monster.ghoulInst->SetPartOnOff(newHeadType, true);
							gzones[GZ_HEAD_FRONT].parent_id=gzones[GZ_HEAD_BACK].parent_id=newHeadType;
							SetGoreZone(monster, "gz_headbald_f", GZ_HEAD_FRONT);
							SetGoreZone(monster, "gz_headbald_b", GZ_HEAD_BACK);
						}
					}
					else
					{
						headType = monster.ghoulInst->GetGhoulObject()->FindPart("_helmethead");

						if (headType && monster.ghoulInst->GetPartOnOff(headType)) // got a meso helmet
						{
							newHeadType = monster.ghoulInst->GetGhoulObject()->FindPart("_baldhead");
							if (newHeadType)
							{
								monster.ghoulInst->SetPartOnOff(headType, false);
								monster.ghoulInst->SetPartOnOff(newHeadType, true);
								gzones[GZ_HEAD_BACK].parent_id=newHeadType;
								SetGoreZone(monster, "gz_baldhead_b", GZ_HEAD_BACK);
								faceType = monster.ghoulInst->GetGhoulObject()->FindPart("_helmetface");
								newFaceType = monster.ghoulInst->GetGhoulObject()->FindPart("_comface");
								if (faceType && newFaceType)
								{
									monster.ghoulInst->SetPartOnOff(faceType, false);
									monster.ghoulInst->SetPartOnOff(newFaceType, true);
									gzones[GZ_HEAD_FRONT].parent_id=newFaceType;
									SetGoreZone(monster, "gz_comface", GZ_HEAD_FRONT);
								}
							}
						}
					}
				}
//				return damage*0.1;

				if (attacker && attacker->client && (attacker->client->inv->getCurWeaponType() == SFW_SNIPER))
				{
					// no bolt-ons stop player-fired sniper rifle rounds
					return damage;
				}
				return (shootableParts[i].stopsDmg?0:damage);
			} // if ((shootableParts[i].flimsy)||(damage>1))
			else
			{
//				return damage;
				return 0;
			}
		}
	}
	return damage;
}

void bodyhuman_c::RightStep(edict_t &monster)
{
	if (bodyDamageAccumulated & BODYDAMAGE_RIGHTLEG && currentmove == &generic_move_limp_lleg)
	{
		if (gi.irand(0,1))
		{
			VoiceGeneralSound("Limp", monster, 3, 0.75);
		}
	}
}

void bodyhuman_c::LeftStep(edict_t &monster)
{
	if (bodyDamageAccumulated & BODYDAMAGE_LEFTLEG && currentmove == &generic_move_limp_rleg)
	{
		if (gi.irand(0,1))
		{
			VoiceGeneralSound("Limp", monster, 3, 0.75);
		}
	}
}

// anyone for blowing off an air tank?
int bodyhuman_c::SpecialActionBoltOn(edict_t &monster, ggBinstC *boltedptr, Vect3 &pos, Vect3 &Direction, int damage)
{
	char *boltedname = NULL;
	ggOinstC *myInstance=game_ghoul.FindOInst(monster.ghoulInst);

	if (!myInstance)
	{
		return false;
	}

	if (boltedptr && boltedptr->GetBolteeObject() &&
		(boltedname = boltedptr->GetBolteeObject()->GetSubName()))
	{	// now special case anything and everything that could be attached to the entity in question
		if (!stricmp("acc_tanks", boltedname))
		{	// we hit air tanks. blow 'em up.
			myInstance->RemoveBoltInstance(boltedptr);
			// damage the guy first so RadiusBurn will blacken 'im like a Cajun chicken breast
			//(this is a very demented business in which I find myself working)
			monster.health = 0;

			T_RadiusDamage (&monster, &monster, 250, &monster, 175, MOD_EXPLOSIVE, DT_FIRE);
			RadiusBurn(&monster, 175); // since this has a check to make sure guys that are still living dont blacken, this should be ok

			gmonster.RadiusDeafen(&monster, 200, 200);

			fxRunner.exec("weapons/world/airexplode", &monster);

			ShakeCameras (*((vec3_t*)&pos), 100, 100*2, DEFAULT_JITTER_DELTA);
			return true;
		}
	}
	return false;
}

void bodyhuman_c::Gib2(edict_t &monster, vec3_t dir, int dflags)
{
	vec3_t debrisNorm;

// first some chunks
	int numchunks = gi.irand(10,20);
	FX_ThrowDebris(monster.s.origin, debrisNorm, numchunks, DEBRIS_TINY, MAT_ROCK_FLESH, 0, 0, 0, SURF_BLOOD);
	numchunks = gi.irand(5,10);
	FX_ThrowDebris(monster.s.origin,debrisNorm, numchunks, DEBRIS_SM, MAT_ROCK_FLESH, 0, 0, 0, SURF_BLOOD);
	numchunks = gi.irand(0,2);
	FX_ThrowDebris(monster.s.origin, debrisNorm, numchunks, DEBRIS_MED, MAT_ROCK_FLESH, 0, 0, 0, SURF_BLOOD);
//	monster.ghoulInst->SetAllPartsOnOff(false);
	RecursiveTurnOff(monster.ghoulInst);
	monster.ghoulInst->TurnCallBacksOff();
	monster.solid = SOLID_NOT;
	HoldAnimation(monster, HOLDCODE_GIBBED, level.time);
}

int hackGlobalSpawnedBitsForThisGuy;

void bodyhuman_c::Gib(edict_t &monster, vec3_t dir, int dflags)
{
	gz_info		*frontHole=NULL;
	gz_info		*backHole=NULL;
	gz_code		frontCode;
	gz_code		backCode;
	int			totalsevers=0;
	vec3_t		throwdir;

	SetVocalCordsWorking(false);

	if(lock_sever)
	{
		return;
	}

	if (monster.gibbed)
	{
		return;
	}
  
	//make me fall down so giblets look ok--VERY IMPORTANT to only switch to non-death animations, as death animations are NOT EVER to be used without a guarantee of death action--should be safe here, but just don't do it!
	if(currentmove && ((currentmove->bbox == BBOX_PRESET_STAND)||(currentmove->bbox == BBOX_PRESET_CROUCH)))
	{
		vec3_t forward;
		AngleVectors(monster.s.angles, forward, NULL, NULL);

		//fall forward
		if (DotProduct(forward, dir)>0)
		{
//			if(IsAvailableSequence(monster, &generic_move_dive_toprone))
//				SetAnimation(monster, &generic_move_dive_toprone);

			if (IsAvailableSequence(monster, &generic_move_fakedeath_lfshotdeath))
					SetAnimation(monster, &generic_move_fakedeath_lfshotdeath);
		}

		//fall back (note we can get here even if shot in back, if dive to prone isn't available
//		if (IsAvailableSequence(monster, &generic_move_armorblown))
//				SetAnimation(monster, &generic_move_armorblown);

		if(IsAvailableSequence(monster, &generic_move_fakedeath_lbshotdeath))
			SetAnimation(monster, &generic_move_fakedeath_lbshotdeath);
	}

	if (!game.cinematicfreeze)
	{
		level.gibs++;
	}

	hackGlobalSpawnedBitsForThisGuy = 0;

	if (!(lock_blood) && (hackGlobalGibCalls < 4))
	{
		if(dflags & DT_NEURAL)
		{
			monster.ghoulInst->SetTint(1.0, 0.3, 0.0, 1.0);

			fxRunner.exec("gore/mpgsplatter", monster.s.origin);
		}
		else
		{
			fxRunner.exec("gore/hugeburst2", monster.s.origin);
		}
	}

	Emote(monster, EMOTION_DEAD, 1.0);
	UpdateFace(monster);

/*
	//sfs--testing backpack bidness s'more
	if (gzones[GZ_CHEST_FRONT].parent_id&&monster.ghoulInst&&monster.ghoulInst->GetPartOnOff(gzones[GZ_CHEST_FRONT].parent_id))
	{
		frontHole =	&gzones[GZ_CHEST_FRONT];
		frontCode =  GZ_CHEST_FRONT;
		backHole  = &gzones[GZ_CHEST_BACK];
		backCode  =  GZ_CHEST_BACK;

		SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
	}
*/

/*	ShowFrontHoleDamage (monster, GZ_CHEST_FRONT, GZ_CHEST_BACK, inflictor, attacker, dir, point, origin, 80, knockback, dflags, mod);
	ShowFrontHoleDamage (monster, GZ_HEAD_FRONT, GZ_HEAD_BACK, inflictor, attacker, dir, point, origin, 80, knockback, dflags, mod);
	ShowFrontHoleDamage (monster, GZ_SHLDR_RIGHT_FRONT, GZ_SHLDR_RIGHT_BACK, inflictor, attacker, dir, point, origin, 80, knockback, dflags, mod);
	ShowFrontHoleDamage (monster, GZ_SHLDR_LEFT_FRONT, GZ_SHLDR_LEFT_BACK, inflictor, attacker, dir, point, origin, 80, knockback, dflags, mod);
	ShowFrontHoleDamage (monster, GZ_ARM_UPPER_RIGHT, GZ_ARM_UPPER_RIGHT, inflictor, attacker, dir, point, origin, 80, knockback, dflags, mod);
	ShowFrontHoleDamage (monster, GZ_ARM_UPPER_LEFT, GZ_ARM_UPPER_LEFT, inflictor, attacker, dir, point, origin, 80, knockback, dflags, mod);
	ShowFrontHoleDamage (monster, GZ_GUT_FRONT, GZ_GUT_BACK, inflictor, attacker, dir, point, origin, 80, knockback, dflags, mod);
	ShowFrontHoleDamage (monster, GZ_GUT_BACK, GZ_GUT_FRONT, inflictor, attacker, dir, point, origin, 80, knockback, dflags, mod);
	ShowFrontHoleDamage (monster, GZ_LEG_UPPER_RIGHT_FRONT, GZ_LEG_UPPER_RIGHT_BACK, inflictor, attacker, dir, point, origin, 80, knockback, dflags, mod);
	ShowFrontHoleDamage (monster, GZ_LEG_UPPER_RIGHT_BACK, GZ_LEG_UPPER_RIGHT_FRONT, inflictor, attacker, dir, point, origin, 80, knockback, dflags, mod);
	ShowFrontHoleDamage (monster, GZ_HEAD_BACK, GZ_HEAD_FRONT, inflictor, attacker, dir, point, origin, 80, knockback, dflags, mod);
	ShowFrontHoleDamage (monster, GZ_SHLDR_RIGHT_BACK, GZ_SHLDR_RIGHT_FRONT, inflictor, attacker, dir, point, origin, 80, knockback, dflags, mod);
	ShowFrontHoleDamage (monster, GZ_SHLDR_LEFT_BACK, GZ_SHLDR_LEFT_FRONT, inflictor, attacker, dir, point, origin, 80, knockback, dflags, mod);
	ShowFrontHoleDamage (monster, GZ_CHEST_BACK, GZ_CHEST_FRONT, inflictor, attacker, dir, point, origin, 80, knockback, dflags, mod);
*/

	// If this is mangle damage, just gib anyway...
	currentGoreZone = GZ_CHEST_FRONT;

	switch(currentGoreZone)
	{
	case GZ_HEAD_FRONT:
	case GZ_HEAD_BACK:
	case GZ_NECK:
		// Sever head
		frontHole =	&gzones[GZ_HEAD_FRONT];
		frontCode =  GZ_HEAD_FRONT;
		backHole  = &gzones[GZ_HEAD_BACK];
		backCode  =  GZ_HEAD_BACK;
		SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
		break;

	case GZ_ARM_UPPER_RIGHT:
		frontHole =	&gzones[GZ_ARM_UPPER_RIGHT];
		frontCode =  GZ_ARM_UPPER_RIGHT;
		backHole  = &gzones[GZ_ARM_UPPER_RIGHT];
		backCode  =  GZ_ARM_UPPER_RIGHT;
		SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
		break;
	
	case GZ_ARM_UPPER_LEFT:
		frontHole =	&gzones[GZ_ARM_UPPER_LEFT];
		frontCode =  GZ_ARM_UPPER_LEFT;
		backHole  = &gzones[GZ_ARM_UPPER_LEFT];
		backCode  =  GZ_ARM_UPPER_LEFT;
		SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
		break;
	
	case GZ_ARM_LOWER_RIGHT:
		frontHole =	&gzones[GZ_ARM_LOWER_RIGHT];
		frontCode =  GZ_ARM_LOWER_RIGHT;
		backHole  = &gzones[GZ_ARM_LOWER_RIGHT];
		backCode  =  GZ_ARM_LOWER_RIGHT;
		SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
		break;
	
	case GZ_ARM_LOWER_LEFT:
		frontHole =	&gzones[GZ_ARM_LOWER_LEFT];
		frontCode =  GZ_ARM_LOWER_LEFT;
		backHole  = &gzones[GZ_ARM_LOWER_LEFT];
		backCode  =  GZ_ARM_LOWER_LEFT;
		SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
		break;
	
	case GZ_LEG_UPPER_RIGHT_FRONT:
	case GZ_LEG_UPPER_RIGHT_BACK:
		frontHole =	&gzones[GZ_LEG_UPPER_RIGHT_FRONT];
		frontCode =  GZ_LEG_UPPER_RIGHT_FRONT;
		backHole  = &gzones[GZ_LEG_UPPER_RIGHT_BACK];
		backCode  =  GZ_LEG_UPPER_RIGHT_BACK;
		SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
		break;
	
	case GZ_LEG_UPPER_LEFT_FRONT:
	case GZ_LEG_UPPER_LEFT_BACK:
		frontHole =	&gzones[GZ_LEG_UPPER_LEFT_FRONT];
		frontCode =  GZ_LEG_UPPER_LEFT_FRONT;
		backHole  = &gzones[GZ_LEG_UPPER_LEFT_BACK];
		backCode  =  GZ_LEG_UPPER_LEFT_BACK;
		SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
		break;
	
	case GZ_LEG_LOWER_RIGHT:
	case GZ_FOOT_RIGHT:
		frontHole =	&gzones[GZ_ARM_LOWER_RIGHT];
		frontCode =  GZ_ARM_LOWER_RIGHT;
		backHole  = &gzones[GZ_ARM_LOWER_RIGHT];
		backCode  =  GZ_ARM_LOWER_RIGHT;
		SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
		break;
	
	case GZ_LEG_LOWER_LEFT:
	case GZ_FOOT_LEFT:
		frontHole =	&gzones[GZ_ARM_LOWER_LEFT];
		frontCode =  GZ_ARM_LOWER_LEFT;
		backHole  = &gzones[GZ_ARM_LOWER_LEFT];
		backCode  =  GZ_ARM_LOWER_LEFT;
		SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
		break;
	
	case GZ_SHLDR_RIGHT_FRONT:
	case GZ_SHLDR_RIGHT_BACK:
	case GZ_SHLDR_LEFT_FRONT:
	case GZ_SHLDR_LEFT_BACK:
	case GZ_CHEST_FRONT:
	case GZ_CHEST_BACK:
	case GZ_GUT_FRONT:
	case GZ_GUT_BACK:
	case GZ_GROIN:
	case GZ_GUT_FRONT_EXTRA:
	case GZ_GUT_BACK_EXTRA:
	default:
		// Randomly remove a bunch of limbs and turn the body into goo.
		// pop the head
		if (gzones[GZ_HEAD_FRONT].parent_id&&monster.ghoulInst&&monster.ghoulInst->GetPartOnOff(gzones[GZ_HEAD_FRONT].parent_id))
		{
			if(gi.irand(0,1))
			{
				throwdir[0] = dir[0] + gi.flrand(-1.5, 1.5);
				throwdir[1] = dir[1] + gi.flrand(-1.5, 1.5);
				throwdir[2] = dir[2] + gi.flrand(1.0, 2.0);

				frontHole =	&gzones[GZ_HEAD_FRONT];
				frontCode =  GZ_HEAD_FRONT;
				backHole  = &gzones[GZ_HEAD_BACK];
				backCode  =  GZ_HEAD_BACK;

				SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
				totalsevers++;
			}
		}
		else
		{
			totalsevers++;
		}

		if (gzones[GZ_ARM_UPPER_LEFT].parent_id&&monster.ghoulInst&&monster.ghoulInst->GetPartOnOff(gzones[GZ_ARM_UPPER_LEFT].parent_id))
		{
			if(gi.irand(0,1))
			{
				throwdir[0] = dir[0] + gi.flrand(-1.5, 1.5);
				throwdir[1] = dir[1] + gi.flrand(-1.5, 1.5);
				throwdir[2] = dir[2] + gi.flrand(1.0, 2.0);

				frontHole =	&gzones[GZ_ARM_UPPER_LEFT];
				frontCode =  GZ_ARM_UPPER_LEFT;
				backHole  = &gzones[GZ_ARM_UPPER_LEFT];
				backCode  =  GZ_ARM_UPPER_LEFT;

				SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
				totalsevers++;
			}
		}
		else
		{
			totalsevers++;
		}

		if (gzones[GZ_ARM_UPPER_RIGHT].parent_id&&monster.ghoulInst&&monster.ghoulInst->GetPartOnOff(gzones[GZ_ARM_UPPER_RIGHT].parent_id))
		{
			if(gi.irand(0,1))
			{
				throwdir[0] = dir[0] + gi.flrand(-1.5, 1.5);
				throwdir[1] = dir[1] + gi.flrand(-1.5, 1.5);
				throwdir[2] = dir[2] + gi.flrand(1.0, 2.0);

				frontHole =	&gzones[GZ_ARM_UPPER_RIGHT];
				frontCode =  GZ_ARM_UPPER_RIGHT;
				backHole  = &gzones[GZ_ARM_UPPER_RIGHT];
				backCode  =  GZ_ARM_UPPER_RIGHT;

				SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
				totalsevers++;
			}
		}
		else
		{
			totalsevers++;
		}

		if (gzones[GZ_ARM_LOWER_LEFT].parent_id&&monster.ghoulInst&&monster.ghoulInst->GetPartOnOff(gzones[GZ_ARM_LOWER_LEFT].parent_id))
		{
			if(gi.irand(0,1))
			{
				throwdir[0] = dir[0] + gi.flrand(-1.5, 1.5);
				throwdir[1] = dir[1] + gi.flrand(-1.5, 1.5);
				throwdir[2] = dir[2] + gi.flrand(1.0, 2.0);

				frontHole =	&gzones[GZ_ARM_LOWER_LEFT];
				frontCode =  GZ_ARM_LOWER_LEFT;
				backHole  = &gzones[GZ_ARM_LOWER_LEFT];
				backCode  =  GZ_ARM_LOWER_LEFT;

				SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
				totalsevers++;
			}
		}
		else
		{
			totalsevers++;
		}

		if (gzones[GZ_ARM_LOWER_RIGHT].parent_id&&monster.ghoulInst&&monster.ghoulInst->GetPartOnOff(gzones[GZ_ARM_LOWER_RIGHT].parent_id))
		{
			if(gi.irand(0,1))
			{
				throwdir[0] = dir[0] + gi.flrand(-1.5, 1.5);
				throwdir[1] = dir[1] + gi.flrand(-1.5, 1.5);
				throwdir[2] = dir[2] + gi.flrand(1.0, 2.0);

				frontHole =	&gzones[GZ_ARM_LOWER_RIGHT];
				frontCode =  GZ_ARM_LOWER_RIGHT;
				backHole  = &gzones[GZ_ARM_LOWER_RIGHT];
				backCode  =  GZ_ARM_LOWER_RIGHT;

				SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
				totalsevers++;
			}
		}
		else
		{
			totalsevers++;
		}

		if (gzones[GZ_LEG_UPPER_LEFT_FRONT].parent_id&&monster.ghoulInst&&monster.ghoulInst->GetPartOnOff(gzones[GZ_LEG_UPPER_LEFT_FRONT].parent_id))
		{
			if(gi.irand(0,1))
			{
				throwdir[0] = dir[0] + gi.flrand(-1.5, 1.5);
				throwdir[1] = dir[1] + gi.flrand(-1.5, 1.5);
				throwdir[2] = dir[2] + gi.flrand(1.0, 2.0);

				frontHole =	&gzones[GZ_LEG_UPPER_LEFT_FRONT];
				frontCode =  GZ_LEG_UPPER_LEFT_FRONT;
				backHole  = &gzones[GZ_LEG_UPPER_LEFT_BACK];
				backCode  =  GZ_LEG_UPPER_LEFT_BACK;

				SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
				totalsevers++;
			}
		}
		else
		{
			totalsevers++;
		}

		if (gzones[GZ_LEG_UPPER_RIGHT_FRONT].parent_id&&monster.ghoulInst&&monster.ghoulInst->GetPartOnOff(gzones[GZ_LEG_UPPER_RIGHT_FRONT].parent_id))
		{
			if(gi.irand(0,1))
			{
				throwdir[0] = dir[0] + gi.flrand(-1.5, 1.5);
				throwdir[1] = dir[1] + gi.flrand(-1.5, 1.5);
				throwdir[2] = dir[2] + gi.flrand(1.0, 2.0);

				frontHole =	&gzones[GZ_LEG_UPPER_RIGHT_FRONT];
				frontCode =  GZ_LEG_UPPER_RIGHT_FRONT;
				backHole  = &gzones[GZ_LEG_UPPER_RIGHT_BACK];
				backCode  =  GZ_LEG_UPPER_RIGHT_BACK;

				SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
				totalsevers++;
			}
		}
		else
		{
			totalsevers++;
		}

		if (gzones[GZ_LEG_LOWER_LEFT].parent_id&&monster.ghoulInst&&monster.ghoulInst->GetPartOnOff(gzones[GZ_LEG_LOWER_LEFT].parent_id))
		{
			if(gi.irand(0,1))
			{
				throwdir[0] = dir[0] + gi.flrand(-1.5, 1.5);
				throwdir[1] = dir[1] + gi.flrand(-1.5, 1.5);
				throwdir[2] = dir[2] + gi.flrand(1.0, 2.0);

				frontHole =	&gzones[GZ_LEG_LOWER_LEFT];
				frontCode =  GZ_LEG_LOWER_LEFT;
				backHole  = &gzones[GZ_LEG_LOWER_LEFT];
				backCode  =  GZ_LEG_LOWER_LEFT;

				SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
				totalsevers++;
			}
		}
		else
		{
			totalsevers++;
		}

		if (gzones[GZ_LEG_LOWER_RIGHT].parent_id&&monster.ghoulInst&&monster.ghoulInst->GetPartOnOff(gzones[GZ_LEG_LOWER_RIGHT].parent_id))
		{
			if(gi.irand(0,1))
			{
				throwdir[0] = dir[0] + gi.flrand(-1.5, 1.5);
				throwdir[1] = dir[1] + gi.flrand(-1.5, 1.5);
				throwdir[2] = dir[2] + gi.flrand(1.0, 2.0);

				frontHole =	&gzones[GZ_LEG_LOWER_RIGHT];
				frontCode =  GZ_LEG_LOWER_RIGHT;
				backHole  = &gzones[GZ_LEG_LOWER_RIGHT];
				backCode  =  GZ_LEG_LOWER_RIGHT;

				SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
				totalsevers++;
			}
		}
		else
		{
			totalsevers++;
		}

		// A severed chest looks MIGHTY dumb... :-/
		if(!gi.irand(0,2))
		{
			if (gzones[GZ_CHEST_FRONT].parent_id&&monster.ghoulInst&&monster.ghoulInst->GetPartOnOff(gzones[GZ_CHEST_FRONT].parent_id))
			{
				throwdir[0] = dir[0] + gi.flrand(-1.5, 1.5);
				throwdir[1] = dir[1] + gi.flrand(-1.5, 1.5);
				throwdir[2] = dir[2] + gi.flrand(1.0, 2.0);

				frontHole =	&gzones[GZ_CHEST_FRONT];
				frontCode =  GZ_CHEST_FRONT;
				backHole  = &gzones[GZ_CHEST_BACK];
				backCode  =  GZ_CHEST_BACK;

				SeverRoutine(monster, throwdir, frontCode, backCode, frontHole, backHole);
			}
		}


		if ((totalsevers>6) || (dflags&(DT_MANGLE|DT_NEURAL)))
		{	// Destroy the torso.
/*			// Throw big chunks
			FX_ThrowDebris(monster.s.origin, dir, gi.flrand(3,4), DEBRIS_MED, MAT_ROCK_FLESH, 0, 0, 0, SURF_BLOOD);
			// Throw smaller chunks
			FX_ThrowDebris(monster.s.origin, dir, gi.flrand(4,5), DEBRIS_SM, MAT_ROCK_FLESH, 0, 0, 0, SURF_BLOOD);
			// Throw tiny chunks
			FX_ThrowDebris(monster.s.origin, dir, gi.flrand(4,6), DEBRIS_TINY, MAT_ROCK_FLESH, 0, 0, 0, SURF_BLOOD);
*/

			monster.gibbed = true;
			monster.solid = SOLID_NOT;
			fxRunner.exec("gore/gibbody", &monster);

			// Now just remove all remaining bits, I hate having a severed groin remain forever...
			if (monster.ghoulInst)
			{
				monster.ghoulInst->SetOnOff(false, level.time);
				game_ghoul.FindOInst(monster.ghoulInst)->RemoveAllBolts();
			}
		}
		break;
	}
}


void danglyTouch (edict_t *ent, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if (!lock_blood)
	{
		if(other)
		{
			if(other == world)
			{
				if(VectorLength(ent->velocity) > 2)
				{
					vec3_t myDir;

					VectorScale(plane->normal, -1, myDir);
					fxRunner.setDir(myDir);
					fxRunner.exec("gore/danglyblood", ent->s.origin);
				}
			}
		}
	}

	VectorSet(ent->avelocity, 0, 0, 0);
}

void bodyhuman_c::SeverRoutine(edict_t &monster, vec3_t dir, gz_code frontCode, gz_code backCode, gz_info *frontHole, gz_info *backHole, bool forceSever)
{
	gz_info		*blownPiece=NULL;
	edict_t		*danglyBit=0;
	ggOinstC	*blownInst=0;
	ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);

	if(lock_sever)
	{
		return;
	}

	if (frontHole->blowme->area != GBLOWNAREA_HEAD)
	{
		//make it less likely to even spawn danglybits after a certain number are gibbed
		if (hackGlobalGibCalls < 5 && hackGlobalSpawnedBitsForThisGuy < 2)
		{
			danglyBit=MakeBlownPart(monster, frontCode, dir, &blownInst, false);
		}
	}
	
/*
	if (!danglyBit)
	{	// couldn't blow off some part
		return;
	}
	*/

	//if there's no cap, I can't allow the part to be blown off
	//oop, forgot about all the self-capping biz for armorchests, and there's some other stuff too, I'm sure
	//should be safe if we check both cap and capbolt, and make sure suspect guys like mask head don't set either one.
	if (!frontHole->cap&&!frontHole->capbolt)
	{
		return;
	}

	if (danglyBit && blownInst)
	{
		// put proper caps on
		if (frontHole->blownCapBolt && blownInst)
		{
			if (frontHole->capPiece)
			{
				if (frontHole->capPieceBolt)
				{
					AddBoltedItem(monster, frontHole->blownCapBolt, "Enemy/bolt", frontHole->capPiece->GetSubName(), frontHole->capPieceBolt, blownInst);
				}
				else
				{
					AddBoltedItem(monster, frontHole->blownCapBolt, "Enemy/bolt", frontHole->capPiece->GetSubName(), "to_gbolt", blownInst);
				}
			}
			else
			{
				AddBoltedItem(monster, frontHole->blownCapBolt, "Enemy/bolt", "g_cap1", "to_gbolt", blownInst);
			}
		}
		if (frontHole->blownCap)
		{
			danglyBit->ghoulInst->SetPartOnOff(frontHole->blownCap,true);
			danglyBit->ghoulInst->SetFrameOverride("c","c_blood",frontHole->blownCap);
		}
	}
//	BlowZone(monster,frontHole->childzone,frontHole->childzone,danglyBit);
	BlowZone(monster,frontCode,frontCode,danglyBit);
	BlowPart(monster, frontHole->blowme, danglyBit);
/*
	if (frontHole->parent_id)
	{
		myInstance->GetInstPtr()->SetPartOnOff(frontHole->parent_id,false);
	}
	if (frontHole->id)
	{
		myInstance->GetInstPtr()->SetPartOnOff(frontHole->id,false);
	}
	if (backHole && backHole->id)
	{
		myInstance->GetInstPtr()->SetPartOnOff(backHole->id,false);
	}
*/
	// kef -- this has cause an assert in GhoulInst::SetPartOnOff() from grenades and rockets
	if (frontHole->cap)
	{
		myInstance->GetInstPtr()->SetPartOnOff(frontHole->cap,true);
		myInstance->GetInstPtr()->SetFrameOverride("c","c_blood",frontHole->cap);
	}
	if (frontHole->capbolt)
	{
		if (frontHole->capPiece)
		{
/*
			if (frontHole->capPieceBolt)
			{
				AddBoltedItem(monster, frontHole->capbolt, "Enemy/bolt", frontHole->capPiece->GetSubName(), frontHole->capPieceBolt, myInstance);
			}
			else
			{
				AddBoltedItem(monster, frontHole->capbolt, "Enemy/bolt", frontHole->capPiece->GetSubName(), "to_gbolt", myInstance);
			}
*/
			if (frontHole->capPieceBolt)
			{
				AddBoltedItem(monster, frontHole->capbolt, frontHole->capPiece, frontHole->capPiece->GetSubName(), frontHole->capPieceBolt, myInstance);
			}
			else
			{
				AddBoltedItem(monster, frontHole->capbolt, "Enemy/bolt", frontHole->capPiece->GetSubName(), "to_gbolt", myInstance);
			}
			if (!strcmp(frontHole->capPiece->GetSubName(), "g_half_head"))
			{
				if (myInstance->GetBoltInstance(frontHole->capbolt))
				{
					// for the half head bolt on, we want to put the living guy's skins on the new bolt on
					char		headSkinName[128];
					char		faceSkinName[128];
					
					GhoulID	headSkin = monster.ghoulInst->GetFrameOverride("h");
					if (headSkin)
					{
						monster.ghoulInst->GetGhoulObject()->GetSkinName(headSkin, headSkinName);
						myInstance->GetBoltInstance(frontHole->capbolt)->GetInstPtr()->SetFrameOverride("h", headSkinName); 
					}
					
					GhoulID	faceSkin = monster.ghoulInst->GetFrameOverride("f");
					if (faceSkin)
					{
						monster.ghoulInst->GetGhoulObject()->GetSkinName(faceSkin, faceSkinName);
						myInstance->GetBoltInstance(frontHole->capbolt)->GetInstPtr()->SetFrameOverride("f", faceSkinName); 
					}

					myInstance->GetBoltInstance(frontHole->capbolt)->GetInstPtr()->SetFrameOverride("gz", "gz_half_head"); 
				}
			}
		}
		else
		{
			AddBoltedItem(monster, frontHole->capbolt, "Enemy/bolt", "g_cap1", "to_gbolt", myInstance);
		}
		if (monster.health>0)
		{
//			gi.sound (&monster, CHAN_BODY, gi.soundindex ("impact/gore/spout.wav"), .8, ATTN_NORM, 0);
//			FX_Bleed(&monster, frontHole->capbolt, 40);
		}
		else
		{
//			FX_BloodCloud(&monster, frontHole->capbolt, 100);
		}
	}

	if (danglyBit)
	{
		if(((hackGlobalSpawnedBitsForThisGuy >= 2) || (hackGlobalGibCalls > 2)) && !forceSever)
		{
			G_FreeEdict(danglyBit);//this could be opitmized =/
			return;
		}
		else
		{
			VectorScale(dir, gi.flrand(100, 200), danglyBit->velocity);
//			danglyBit->avelocity[YAW] = gi.flrand(-750, 750);
			danglyBit->touch = danglyTouch;

			if (frontHole->blowme->area != GBLOWNAREA_HEAD)
			{
				hackGlobalSpawnedBitsForThisGuy++;
			}

			if (forceSever)
			{
				danglyBit->nextthink=level.time+60.0;
			}
		}
	}
}

void bodyhuman_c::AllocateGoreZones(edict_t &monster)
{
	//eliminate old gore zones...
	bodyorganic_c::AllocateGoreZones(monster);
	numGoreZones=NUM_HUMANGOREZONES;
	gzones=new gz_info[numGoreZones];
}

void bodyhuman_c::SetGoreZoneDamageScale(edict_t &monster)
{
	if (numGoreZones<NUM_HUMANGOREZONES)
	{
		gi.dprintf("couldn't setup gzone damage scales!!!\n");
		return;
	}
	gzones[GZ_HEAD_FRONT].damScale=10.0f;
	gzones[GZ_HEAD_BACK].damScale=10.0f;
	gzones[GZ_NECK].damScale=10.0f;
	gzones[GZ_SHLDR_RIGHT_FRONT].damScale=1.0f;
	gzones[GZ_SHLDR_RIGHT_BACK].damScale=1.0f;
	gzones[GZ_SHLDR_LEFT_FRONT].damScale=1.0f;
	gzones[GZ_SHLDR_LEFT_BACK].damScale=1.0f;
	gzones[GZ_CHEST_FRONT].damScale=1.0f;
	gzones[GZ_CHEST_BACK].damScale=1.0f;
	gzones[GZ_ARM_UPPER_RIGHT].damScale=0.5f;
	gzones[GZ_ARM_UPPER_LEFT].damScale=0.5f;
	gzones[GZ_ARM_LOWER_RIGHT].damScale=0.5f;
	gzones[GZ_ARM_LOWER_LEFT].damScale=0.5f;
	gzones[GZ_GUT_FRONT].damScale=1.0f;
	gzones[GZ_GUT_BACK].damScale=1.0f;
	gzones[GZ_GROIN].damScale=1.0f;
	gzones[GZ_LEG_UPPER_RIGHT_FRONT].damScale=0.5f;
	gzones[GZ_LEG_UPPER_RIGHT_BACK].damScale=0.5f;
	gzones[GZ_LEG_UPPER_LEFT_FRONT].damScale=0.5f;
	gzones[GZ_LEG_UPPER_LEFT_BACK].damScale=0.5f;
	gzones[GZ_LEG_LOWER_RIGHT].damScale=0.5f;
	gzones[GZ_LEG_LOWER_LEFT].damScale=0.5f;
	gzones[GZ_FOOT_RIGHT].damScale=0.5f;
	gzones[GZ_FOOT_LEFT].damScale=0.5f;
	gzones[GZ_GUT_FRONT_EXTRA].damScale=1.0f;
	gzones[GZ_GUT_BACK_EXTRA].damScale=1.0f;
}

int bodyhuman_c::IsMovementImpeded(void)
{
	if (dm->isDM())
	{	// this is kind of cheesy, but returning flags won't mean a whole lot in w_fire.cpp
		//
		// 0 = no movement-impeding damage
		//-1 = left leg damaged
		// 1 = right leg damaged
		if (bodyDamageAccumulated & BODYDAMAGE_LEFTLEG)
		{
			return -1;
		}
		else if (bodyDamageAccumulated & BODYDAMAGE_RIGHTLEG)
		{
			return 1;
		}
		return 0;
	}
	else
	{	// this is standard single player stuff. only used by decisions (at least when I wrote this, anyway).
		return bodyDamage & BODYDAMAGE_ANY;
	}
}

int bodyhuman_c::IsAimImpeded(void)
{
	if (dm->isDM())
	{	// this is kind of cheesy, but returning flags won't mean a whole lot in w_fire.cpp
		//
		// 0 = no aim-impeding damage
		//-1 = left arm damaged
		// 1 = right arm damaged
		if (bodyDamageAccumulated & BODYDAMAGE_LEFTARM)
		{
			return -1;
		}
		else if (bodyDamageAccumulated & BODYDAMAGE_RIGHTARM)
		{
			return 1;
		}
		return 0;
	}
	return 0;
}

void bodyhuman_c::RemoveLimbDamage(void)
{
	bodyDamageAccumulated = BODYDAMAGE_NONE;
}

void bodyhuman_c::AssignFrontAndBackGZones(gz_info **frontHole, gz_code *frontCode, gz_info **backHole, gz_code *backCode)
{
	*frontHole =	&gzones[GZ_CHEST_FRONT];
	*frontCode =  GZ_CHEST_FRONT;
	*backHole  = &gzones[GZ_CHEST_BACK];
	*backCode  =  GZ_CHEST_BACK;
}


bodyhuman_c::bodyhuman_c(bodyhuman_c *orig)
: bodyorganic_c(orig)
{
	int		i;

  	curSoundIndex		= orig->curSoundIndex;      
  	curVol				= orig->curVol;     
	curAtten			= orig->curAtten;
	for(i = 0; i < 64; i++)
	{
  		voiceDirGeneral[i] = orig->voiceDirGeneral[i];
  		voiceDirDeath[i] = orig->voiceDirDeath[i];
	}
  	voiceDirNumber		= orig->voiceDirNumber;     
	for(i = 0; i < 64; i++)
	{
  		face_base[i]		= orig->face_base[i];      
	}
	for(i = 0; i < 64; i++)
	{
  		curface[i]			= orig->curface[i];         
	}
  	blink_time			= orig->blink_time;         
  	breathe_time		= orig->breathe_time;         
  	rLastFireTime		= orig->rLastFireTime;      
  	lLastFireTime		= orig->lLastFireTime;
	nextGreetTime		= orig->nextGreetTime;
	numGreets			= orig->numGreets;
	numWakes			= orig->numWakes;
	curGreet			= orig->curGreet;
	touchSound			= orig->touchSound;
  	rHandOpen			= orig->rHandOpen;          
  	lHandOpen			= orig->lHandOpen;          
  	grenadeInHand		= orig->grenadeInHand;          
  	bodyDamage			= orig->bodyDamage;
  	bodyDamageAccumulated	= orig->bodyDamageAccumulated;
	bodyDamageEndRecent	= orig->bodyDamageEndRecent;
	FinalAnim			= orig->FinalAnim;          
  	FinalSound			= orig->FinalSound;         
  	rWeaponShot			= orig->rWeaponShot;        
  	lWeaponShot			= orig->lWeaponShot;        
  	rWeaponAmmo			= orig->rWeaponAmmo;        
  	freezeMyBolts		= orig->freezeMyBolts;      
  	gutsOut				= orig->gutsOut;
	justFiredRight		= orig->justFiredRight;
	lHandAtk			= orig->lHandAtk;
	rHandAtk			= orig->rHandAtk;
	deathVoiceType		= orig->deathVoiceType;
	armorCode			= orig->armorCode;
	nNextShotKillsMe	= orig->nNextShotKillsMe;
	bDropBoltons		= orig->bDropBoltons;
	bBoxConfirmed		= orig->bBoxConfirmed;
	bVocalCordsWorking  = orig->bVocalCordsWorking;
}

void bodyhuman_c::Evaluate(bodyhuman_c *orig)
{
	int		i;

  	curSoundIndex		= orig->curSoundIndex;      
  	curVol				= orig->curVol;   
	curAtten			= orig->curAtten;
	for(i = 0; i < 64; i++)
	{
  		voiceDirGeneral[i] = orig->voiceDirGeneral[i];
  		voiceDirDeath[i] = orig->voiceDirDeath[i];
	}
  	voiceDirNumber		= orig->voiceDirNumber;     
	for(i = 0; i < 64; i++)
	{
  		face_base[i]		= orig->face_base[i];      
	}
	for(i = 0; i < 64; i++)
	{
  		curface[i]			= orig->curface[i];         
	}
  	blink_time			= orig->blink_time;         
  	breathe_time		= orig->breathe_time;         
  	rLastFireTime		= orig->rLastFireTime;      
  	lLastFireTime		= orig->lLastFireTime;      
	nextGreetTime		= orig->nextGreetTime;
	numGreets			= orig->numGreets;
	numWakes			= orig->numWakes;
	curGreet			= orig->curGreet;
	touchSound			= orig->touchSound;
  	rHandOpen			= orig->rHandOpen;          
  	lHandOpen			= orig->lHandOpen;          
  	grenadeInHand		= orig->grenadeInHand;          
  	bodyDamage			= orig->bodyDamage;         
  	bodyDamageAccumulated	= orig->bodyDamageAccumulated;         
	bodyDamageEndRecent	= orig->bodyDamageEndRecent;
  	FinalAnim			= orig->FinalAnim;          
  	FinalSound			= orig->FinalSound;         
  	rWeaponShot			= orig->rWeaponShot;        
  	lWeaponShot			= orig->lWeaponShot;        
  	rWeaponAmmo			= orig->rWeaponAmmo;        
  	freezeMyBolts		= orig->freezeMyBolts;      
  	gutsOut				= orig->gutsOut;            
	justFiredRight		= orig->justFiredRight;
	rHandAtk			= orig->rHandAtk;
	lHandAtk			= orig->lHandAtk;
 	deathVoiceType		= orig->deathVoiceType;
	armorCode			= orig->armorCode;
	nNextShotKillsMe	= orig->nNextShotKillsMe;
	bDropBoltons		= orig->bDropBoltons;
	bBoxConfirmed		= orig->bBoxConfirmed;
	bVocalCordsWorking  = orig->bVocalCordsWorking;

	bodyorganic_c::Evaluate(orig);
}

bool bodyhuman_c::FreeKnife(edict_t &monster, ggBinstC* bolted, GhoulID boltKnife)
{
	edict_t		*knife = NULL;
	Matrix4	EntityToWorld;
	Matrix4	BoltToEntity, KnifeToEntity;
	Matrix4	KnifeToWorld;

	EntToWorldMatrix(monster.s.origin, monster.s.angles, EntityToWorld);

	if (0 == strcmp(bolted->GetBolteeObject()->GetSubName(), "w_knife"))
	{ // if the thing attached to this bolt is a knife, create a new
	  //knife entity and put it where the bolted-on knife was
		if (knife = G_Spawn())
		{
			Matrix4		matOld, matTip, TipToWorld;
			IGhoulInst	*oldKnife, *newKnife;
			GhoulID		boltTip;

			game_ghoul.SetSimpleGhoulModel(knife, "Enemy/Bolt", "w_knife");
			if ((oldKnife = bolted->GetInstPtr()) &&
				(newKnife = knife->ghoulInst))
			{
				oldKnife->GetXForm(matOld);

				monster.ghoulInst->GetBoltMatrix(level.time,BoltToEntity,boltKnife,
					IGhoulInst::MatrixType::Entity);
				boltTip = knife->ghoulInst->GetGhoulObject()->FindPart("tip");
				knife->ghoulInst->GetBoltMatrix(level.time,matTip,boltTip,
					IGhoulInst::MatrixType::Entity);
				matTip.Inverse(matTip);

				// concat with the xform of the knife itself
				KnifeToEntity.Concat(matOld, BoltToEntity);

				KnifeToWorld.Concat(KnifeToEntity, EntityToWorld);
				TipToWorld.Concat(matTip, KnifeToWorld);

				// 3rd row of KnifeToWorld is the world coordinates of the knife
				TipToWorld.GetRow(3,*(Vect3 *)knife->s.origin);

				// having retrieved the origin of the matrix, zero it out so it doesn't 
				//get applied again
				Vect3 zero(0,0,0);
				TipToWorld.SetRow(3, zero);

				// orient the knife
				//KnifeToWorld.Scale(1.0);

				// make sure the only orientation we're giving the knife is due to it's xform
				VectorClear(knife->s.angles);
//				Matrix4 matOrig(KnifeToWorld), matRot;
//				matRot.Rotate(1, M_PI*0.5);
//				KnifeToWorld.Concat(matOrig, matRot);
				newKnife->SetXForm(TipToWorld);
				knife->movetype = MOVETYPE_DAN;
				knife->solid = SOLID_BBOX;
				knife->clipmask = CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER|CONTENTS_SHOT|CONTENTS_WINDOW;
				VectorSet (knife->mins, -4, -2, -4);
				VectorSet (knife->maxs, 3, 2, 4);
				knife->health = 10000;
				gi.linkentity(knife);
			}
			knife->spawnflags|=DROPPED_ITEM;
			I_SpawnKnife(knife);
			knife->gravity = 1.0;
		}
		return true;
	}
	return false;
}

void bodyhuman_c::FreeKnives(edict_t &monster)
{
	// need to remove all bolted-on knives and replace them with actual knife edicts
	ggBinstC	*bolted;
	ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);

	if (NULL == myInstance)
	{
		return;
	}
	for (int i = 0; i < numGoreZones; i++)
	{
		if (gzones[i].blownCapBolt)
		{
				while (bolted=myInstance->GetBoltInstance(gzones[i].blownCapBolt))
				{
					if (FreeKnife(monster, bolted, gzones[i].blownCapBolt))
					{
						myInstance->RemoveBoltInstance(bolted);
						gi.dprintf("FreeKnives() -- gzones[%d].blownCapBolt\n", i);
					}
				}
				//myInstance->GetInstPtr()->SetPartOnOff(gzones[i].blownCapBolt,false);
		}
	}
	// search default locations for knives
	GhoulID defaultBolt = monster.ghoulInst->GetGhoulObject()->FindPart("abolt_hip_r");
	if (bolted=myInstance->GetBoltInstance(defaultBolt))
	{
		if (FreeKnife(monster, bolted, defaultBolt))
		{
			myInstance->RemoveBoltInstance(bolted);
			gi.dprintf("FreeKnives() -- abolt_hip_r\n");
		}
	}
	defaultBolt = monster.ghoulInst->GetGhoulObject()->FindPart("sbolt_heel_r");
	if (bolted=myInstance->GetBoltInstance(defaultBolt))
	{
		if (FreeKnife(monster, bolted, defaultBolt))
		{
			myInstance->RemoveBoltInstance(bolted);
			gi.dprintf("FreeKnives() -- sbolt_heel_r\n");
		}
	}
	defaultBolt = monster.ghoulInst->GetGhoulObject()->FindPart("sbolt_heel_l");
	if (bolted=myInstance->GetBoltInstance(defaultBolt))
	{
		if (FreeKnife(monster, bolted, defaultBolt))
		{
			myInstance->RemoveBoltInstance(bolted);
			gi.dprintf("FreeKnives() -- sbolt_heel_l\n");
		}
	}
	defaultBolt = monster.ghoulInst->GetGhoulObject()->FindPart("gbolt_cap_rbicep_chest_tight");
	if (bolted=myInstance->GetBoltInstance(defaultBolt))
	{
		if (FreeKnife(monster, bolted, defaultBolt))
		{
			myInstance->RemoveBoltInstance(bolted);
			gi.dprintf("FreeKnives() -- gbolt_cap_rbicep_chest_tight\n");
		}
	}
	defaultBolt = monster.ghoulInst->GetGhoulObject()->FindPart("gbolt_cap_lbicep_chest_tight");
	if (bolted=myInstance->GetBoltInstance(defaultBolt))
	{
		if (FreeKnife(monster, bolted, defaultBolt))
		{
			myInstance->RemoveBoltInstance(bolted);
			gi.dprintf("FreeKnives() -- gbolt_cap_lbicep_chest_tight\n");
		}
	}
}

void bodyhuman_c::UpdateSoundsByWeapon(edict_t &monster, ai_c &which_ai)
{
	// don't do the sound if we have a target or if we're playing a cinematic
/*	if( (!which_ai.getTarget()) &&
		!(game.cinematicfreeze) )
	{
		if(gi.irand(1, 160) == 1)
		{
			char *mySound = 0;
			switch(GetRightHandWeapon(monster))
			{
			case ATK_FLAMEGUN:
				break;
			case ATK_MACHINEGUN:
				mySound = "Weapons/MGun/Rattle.wav";
				break;
			case ATK_PISTOL1:
				mySound = "weapons/gpistol/cock.wav";
				break;
			case ATK_PISTOL2:
				if(gi.irand(0,1))
				{
					mySound = "weapons/dpistol/adjust.wav";
				}
				else
				{
					mySound = "weapons/dpistol/cock.wav";
				}
				break;
			case ATK_SHOTGUN:
				mySound = "weapons/shotgun/cock.wav";
				break;
			case ATK_SNIPER:
				mySound = "Weapons/Sniper/Cock.wav";
				break;
			case ATK_ASSAULTRIFLE:
				if(gi.irand(0,1))
				{
					mySound = "weapons/ipistol/catch.wav";
				}
				else
				{
					mySound = "weapons/dpistol/spinready.wav";
				}
				break;
			case ATK_MACHINEPISTOL:
				mySound = "weapons/ipistol/cock.wav";
				break;
			}
			if(mySound)
			{
				gi.sound (&monster, CHAN_WEAPON, gi.soundindex (mySound), .7, 2, 0);
			}
		}
	}*/

	if(GetRightHandWeapon(monster) != ATK_FLAMEGUN && GetRightHandWeapon(monster) != ATK_MICROWAVE && GetRightHandWeapon(monster) != ATK_MICROWAVE_ALT)
	{
		return;//if not used below, don't bother
	}

	//FIXME!!!FIXME!!! GROSSLY SLOW!

	IGhoulInst *my_inst = monster.ghoulInst;
	if (!my_inst)
	{
		return;
	}

	GhoulID my_hand = monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r");

	if (!my_hand)
	{
		return;
	}

	ggBinstC *my_weapon = game_ghoul.FindOInst(monster.ghoulInst)->GetBoltInstance(my_hand);

	if (!my_weapon)
	{
		return;
	}

	GhoulID flash = my_weapon->GetBolteeObject()->GetMyObject()->FindPart("flash");

	float size;

	if (flash)
	{
		switch(GetRightHandWeapon(monster))
		{
		case ATK_FLAMEGUN:
			fxRunner.execWithInst("weapons/othermz/flamecharge", &monster, my_weapon->GetInstPtr(), flash);
			if(level.time-rLastFireTime < .5)
			{
				monster.s.sound = gi.soundindex("Weapons/flamegun/Fire.wav");
		 		monster.s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
			}
			else
			{
				monster.s.sound = 0;
				monster.s.sound_data = 0;
			}
			break;
		case ATK_SNIPER:
	/*		if(level.time-rLastFireTime > .2)
			{
				if(VectorLengthSquared(monster.intend_velocity) < 10)
				{	// time ta zoom, I say.
					gi.sound (&monster, CHAN_WEAPON, gi.soundindex ("Weapons/Sniper/zoom.wav"), .7, 1, 0);
				}
			}*/
			break;
		case ATK_MICROWAVE_ALT:
		case ATK_MICROWAVE:
			//hmm
			if(level.time-rLastFireTime > 1.0)
			{
				size = .1 + ((level.time-rLastFireTime) / 6.0)*.9;
				if(size > 1.0)
				{
					size = 1.0;
				}
				fxRunner.setScale(size);
				fxRunner.execWithInst("weapons/othermz/mpgcharge", &monster, my_weapon->GetInstPtr(), flash);

				if(!(((int)(level.time*5))%5))
				{
					fxRunner.exec("weapons/othermz/mpgcharge2", monster.s.origin);
				}
			}
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
//	ecto-specific
//
///////////////////////////////////////////////////////////////////////////////////////////////

bodyecto_c::bodyecto_c()
: bodyhuman_c()
{
}

void bodyecto_c::RecognizeGoreZones(edict_t &monster)
{
	GhoulID	tempHole = 0;
	int i;

	InitializeGoreZoneLevels(monster);

	for (i=0;i<numGoreZones;i++)
	{
		gzones[i].damage=gzones[i].damage_level=0;
		gzones[i].id=gzones[i].parent_id=gzones[i].cap=gzones[i].blownCap=NULL_GhoulID;
		gzones[i].blownCapBolt=gzones[i].capbolt=gzones[i].bolt=gzones[i].capPieceBolt=NULL;
		gzones[i].capPiece=NULL;
		gzones[i].blowme=&ecto_blown_parts[GBLOWN_ECTO_NUM];
		gzones[i].buddyzone=gzones[i].childzone=numGoreZones;
	}

	SetGoreZoneDamageScale(monster);

	SetGoreBlownCap(monster, "_Cap_head_chest", GZ_HEAD_FRONT);
	SetGoreBlownCap(monster, "_Cap_head_chest", GZ_HEAD_BACK);
	SetGoreBlownCap(monster, "_Cap_rbicep_chest", GZ_ARM_UPPER_RIGHT);
	SetGoreBlownCap(monster, "_Cap_lbicep_chest", GZ_ARM_UPPER_LEFT);
	if (gi.flrand(0,1) < .7)
	{
		SetGoreCapPiece(monster, "g_neck_torn_anim2", "to_gbolt_Cap_chest_head", GZ_HEAD_BACK);
		SetGoreCapPiece(monster, "g_neck_torn_anim2", "to_gbolt_Cap_chest_head", GZ_HEAD_FRONT);
	}
	else
	{
		SetGoreCapPiece(monster, "g_half_head", "to_gbolt_Cap_chest_head", GZ_HEAD_BACK);
		SetGoreCapPiece(monster, "g_half_head", "to_gbolt_Cap_chest_head", GZ_HEAD_FRONT);
	}
	SetGoreBolt(monster, "gbolt_head_b", GZ_HEAD_BACK);
	SetGoreBolt(monster, "gbolt_head_f", GZ_HEAD_FRONT);

	gzones[GZ_HEAD_BACK].blowme=gzones[GZ_HEAD_FRONT].blowme=&ecto_blown_parts[GBLOWN_ECTO_HEADBALD];
	gzones[GZ_CHEST_FRONT].blowme=gzones[GZ_CHEST_BACK].blowme=&ecto_blown_parts[GBLOWN_ECTO_CHEST];

	gzones[GZ_SHLDR_RIGHT_FRONT].blowme=&ecto_blown_parts[GBLOWN_ECTO_SHOULD_RF];
	gzones[GZ_SHLDR_RIGHT_BACK].blowme=&ecto_blown_parts[GBLOWN_ECTO_SHOULD_RB];
	gzones[GZ_SHLDR_LEFT_FRONT].blowme=&ecto_blown_parts[GBLOWN_ECTO_SHOULD_LF];
	gzones[GZ_SHLDR_LEFT_BACK].blowme=&ecto_blown_parts[GBLOWN_ECTO_SHOULD_LB];

	gzones[GZ_ARM_LOWER_RIGHT].blowme=&ecto_blown_parts[GBLOWN_ECTO_FOREARMRIGHT];
	gzones[GZ_ARM_LOWER_LEFT].blowme=&ecto_blown_parts[GBLOWN_ECTO_FOREARMLEFT];
	gzones[GZ_LEG_LOWER_LEFT].blowme=&ecto_blown_parts[GBLOWN_ECTO_CALFLEFT];
	gzones[GZ_LEG_LOWER_RIGHT].blowme=&ecto_blown_parts[GBLOWN_ECTO_CALFRIGHT];
	gzones[GZ_ARM_UPPER_RIGHT].blowme=&ecto_blown_parts[GBLOWN_ECTO_BICEPRIGHT];
	gzones[GZ_ARM_UPPER_LEFT].blowme=&ecto_blown_parts[GBLOWN_ECTO_BICEPLEFT];

	gzones[GZ_HEAD_FRONT].childzone=GZ_HEAD_BACK;
	gzones[GZ_HEAD_BACK].childzone=GZ_HEAD_FRONT;

	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips", GZ_SHLDR_RIGHT_FRONT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips", GZ_SHLDR_LEFT_FRONT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips", GZ_SHLDR_RIGHT_BACK);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips", GZ_SHLDR_LEFT_BACK);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips", GZ_CHEST_FRONT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips", GZ_CHEST_BACK);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_head_chest", GZ_HEAD_FRONT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_head_chest", GZ_HEAD_BACK);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_rbicep_chest", GZ_ARM_UPPER_RIGHT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_lbicep_chest", GZ_ARM_UPPER_LEFT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_lforearm_lbicep", GZ_ARM_LOWER_LEFT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_rforearm_rbicep", GZ_ARM_LOWER_RIGHT);

	SetGoreCapBolt(monster, "gbolt_Cap_hips_chest", GZ_CHEST_FRONT);
	SetGoreCapBolt(monster, "gbolt_Cap_hips_chest", GZ_CHEST_BACK);
	SetGoreCapBolt(monster, "gbolt_Cap_chest_head", GZ_HEAD_FRONT);
	SetGoreCapBolt(monster, "gbolt_Cap_chest_head", GZ_HEAD_BACK);
	SetGoreCapBolt(monster, "gbolt_Cap_chest_rbicep", GZ_ARM_UPPER_RIGHT);
	SetGoreCapBolt(monster, "gbolt_Cap_chest_lbicep", GZ_ARM_UPPER_LEFT);
	SetGoreCapBolt(monster, "gbolt_Cap_lbicep_lforearm", GZ_ARM_LOWER_LEFT);
	SetGoreCapBolt(monster, "gbolt_Cap_rbicep_rforearm", GZ_ARM_LOWER_RIGHT);
	SetGoreCap(monster, "_Cap_lbicep_lforearm", GZ_ARM_LOWER_LEFT);
	SetGoreCap(monster, "_Cap_rbicep_rforearm", GZ_ARM_LOWER_RIGHT);
	SetGoreBolt(monster, "gbolt_shoulder_rf", GZ_SHLDR_RIGHT_FRONT);
	SetGoreBolt(monster, "gbolt_shoulder_lf", GZ_SHLDR_LEFT_FRONT);
	SetGoreBolt(monster, "gbolt_shoulder_rb", GZ_SHLDR_RIGHT_BACK);
	SetGoreBolt(monster, "gbolt_shoulder_lb", GZ_SHLDR_LEFT_BACK);
	SetGoreBolt(monster, "gbolt_chest_f", GZ_CHEST_FRONT);
	SetGoreBolt(monster, "gbolt_chest_b", GZ_CHEST_BACK);
	gzones[GZ_CHEST_BACK].childzone=GZ_CHEST_FRONT;
	gzones[GZ_CHEST_FRONT].childzone=GZ_SHLDR_RIGHT_FRONT;
	gzones[GZ_SHLDR_RIGHT_FRONT].childzone=GZ_SHLDR_LEFT_FRONT;
	gzones[GZ_SHLDR_LEFT_FRONT].childzone=GZ_SHLDR_RIGHT_BACK;
	gzones[GZ_SHLDR_RIGHT_BACK].childzone=GZ_SHLDR_LEFT_BACK;
	gzones[GZ_SHLDR_LEFT_BACK].childzone=GZ_CHEST_BACK;

	SetGoreBolt(monster, "gbolt_gut_f", GZ_GUT_FRONT);
	SetGoreBolt(monster, "gbolt_gut_b", GZ_GUT_BACK);
	SetGoreCapPiece(monster, "g_guts_lrg", "to_gbolt_Cap_chest_hips", GZ_CHEST_FRONT);
	SetGoreCapPiece(monster, "g_guts_lrg", "to_gbolt_Cap_chest_hips", GZ_CHEST_BACK);

	// elbow caps
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_ARM_LOWER_RIGHT);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_ARM_LOWER_LEFT);
	
	// shoulder caps
	SetGoreCapPiece(monster, "g_arm_torn_anim1", "to_gbolt_Cap_chest_rbicep", GZ_ARM_UPPER_RIGHT);
	SetGoreCapPiece(monster, "g_arm_torn_anim1", "to_gbolt_Cap_chest_lbicep", GZ_ARM_UPPER_LEFT);

	//hip caps
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_UPPER_RIGHT_FRONT);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_UPPER_RIGHT_BACK);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_UPPER_LEFT_FRONT);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_UPPER_LEFT_BACK);

	// knee caps (ha ha)
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_LOWER_RIGHT);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_LOWER_LEFT);

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_headbald");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_HEAD_FRONT].parent_id=gzones[GZ_HEAD_BACK].parent_id=tempHole;
		SetGoreZone(monster, "gz_headbald_f", GZ_HEAD_FRONT);
		SetGoreZone(monster, "gz_headbald_b", GZ_HEAD_BACK);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_headcrew");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_HEAD_FRONT].parent_id=gzones[GZ_HEAD_BACK].parent_id=tempHole;
			SetGoreZone(monster, "gz_headcrew_f", GZ_HEAD_FRONT);
			SetGoreZone(monster, "gz_headcrew_b", GZ_HEAD_BACK);
		}
		else
		{
			tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_headbeard");
			if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
			{
				gzones[GZ_HEAD_FRONT].parent_id=gzones[GZ_HEAD_BACK].parent_id=tempHole;
				SetGoreZone(monster, "gz_headbeard_f", GZ_HEAD_FRONT);
				SetGoreZone(monster, "gz_headbeard_b", GZ_HEAD_BACK);
			}
			else
			{
				tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_headhelmet");
				if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
				{
					gzones[GZ_HEAD_FRONT].parent_id=gzones[GZ_HEAD_BACK].parent_id=tempHole;
					SetGoreZone(monster, "gz_headhelmet_f", GZ_HEAD_FRONT);
				}
			}
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_chest");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_SHLDR_RIGHT_FRONT].parent_id=gzones[GZ_SHLDR_RIGHT_BACK].parent_id=
			gzones[GZ_SHLDR_LEFT_FRONT].parent_id=gzones[GZ_SHLDR_LEFT_BACK].parent_id=
			gzones[GZ_CHEST_FRONT].parent_id=gzones[GZ_CHEST_BACK].parent_id=tempHole;
		SetGoreCap(monster, "_Cap_chest_head", GZ_HEAD_FRONT);
		SetGoreCap(monster, "_Cap_chest_head", GZ_HEAD_BACK);
		SetGoreCap(monster, "_Cap_chest_rbicep", GZ_ARM_UPPER_RIGHT);
		SetGoreCap(monster, "_Cap_chest_lbicep", GZ_ARM_UPPER_LEFT);
		SetGoreCap(monster, "_Cap_hips_chest", GZ_CHEST_FRONT);
		SetGoreCap(monster, "_Cap_hips_chest", GZ_CHEST_BACK);
		SetGoreBlownCap(monster, "_Cap_chest_hips", GZ_CHEST_FRONT);
		SetGoreBlownCap(monster, "_Cap_chest_hips", GZ_CHEST_BACK);
		SetGoreZone(monster, "gz_chest_rfshldr", GZ_SHLDR_RIGHT_FRONT);
		SetGoreZone(monster, "gz_chest_lfshldr", GZ_SHLDR_LEFT_FRONT);
		SetGoreZone(monster, "gz_chest_rbshldr", GZ_SHLDR_RIGHT_BACK);
		SetGoreZone(monster, "gz_chest_lbshldr", GZ_SHLDR_LEFT_BACK);
		SetGoreZone(monster, "gz_chest_f", GZ_CHEST_FRONT);
		SetGoreZone(monster, "gz_chest_b", GZ_CHEST_BACK);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_coatchest");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_SHLDR_RIGHT_FRONT].parent_id=gzones[GZ_SHLDR_RIGHT_BACK].parent_id=
				gzones[GZ_SHLDR_LEFT_FRONT].parent_id=gzones[GZ_SHLDR_LEFT_BACK].parent_id=
				gzones[GZ_CHEST_FRONT].parent_id=gzones[GZ_CHEST_BACK].parent_id=tempHole;
			SetGoreCap(monster, "_Cap_coatchest_head", GZ_HEAD_FRONT);
			SetGoreCap(monster, "_Cap_coatchest_head", GZ_HEAD_BACK);
			SetGoreCap(monster, "_Cap_coatchest_rbicep", GZ_ARM_UPPER_RIGHT);
			SetGoreCap(monster, "_Cap_coatchest_lbicep", GZ_ARM_UPPER_LEFT);
			SetGoreCap(monster, "_Cap_coathips_coatchest", GZ_CHEST_FRONT);
			SetGoreCap(monster, "_Cap_coathips_coatchest", GZ_CHEST_BACK);
			SetGoreBlownCap(monster, "_Cap_coatchest_coathips", GZ_CHEST_FRONT);
			SetGoreBlownCap(monster, "_Cap_coatchest_coathips", GZ_CHEST_BACK);
			SetGoreZone(monster, "gz_coatchest_rfshldr", GZ_SHLDR_RIGHT_FRONT);
			SetGoreZone(monster, "gz_coatchest_lfshldr", GZ_SHLDR_LEFT_FRONT);
			SetGoreZone(monster, "gz_coatchest_rbshldr", GZ_SHLDR_RIGHT_BACK);
			SetGoreZone(monster, "gz_coatchest_lbshldr", GZ_SHLDR_LEFT_BACK);
			SetGoreZone(monster, "gz_coatchest_f", GZ_CHEST_FRONT);
			SetGoreZone(monster, "gz_coatchest_b", GZ_CHEST_BACK);
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rbicep");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_ARM_UPPER_RIGHT].parent_id=tempHole;
		gzones[GZ_ARM_UPPER_RIGHT].childzone=GZ_ARM_LOWER_RIGHT;
		SetGoreZone(monster, "gz_rbicep", GZ_ARM_UPPER_RIGHT);
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lbicep");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_ARM_UPPER_LEFT].parent_id=tempHole;
		gzones[GZ_ARM_UPPER_LEFT].childzone=GZ_ARM_LOWER_LEFT;
		SetGoreZone(monster, "gz_lbicep", GZ_ARM_UPPER_LEFT);
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lforearm");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_ARM_LOWER_LEFT].parent_id=tempHole;
		SetGoreZone(monster, "gz_lforearm", GZ_ARM_LOWER_LEFT);
		SetGoreBlownCap(monster, "_Cap_lforearm_lbicep", GZ_ARM_LOWER_LEFT);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lforearmcuff");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_ARM_LOWER_LEFT].parent_id=tempHole;
			SetGoreZone(monster, "gz_lforearmcuff", GZ_ARM_LOWER_LEFT);
			SetGoreBlownCap(monster, "_Cap_lforearmcuff_lbicep", GZ_ARM_LOWER_LEFT);
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rforearm");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_ARM_LOWER_RIGHT].parent_id=tempHole;
		SetGoreZone(monster, "gz_rforearm", GZ_ARM_LOWER_RIGHT);
		SetGoreBlownCap(monster, "_Cap_rforearm_rbicep", GZ_ARM_LOWER_RIGHT);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rforearmcuff");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_ARM_LOWER_RIGHT].parent_id=tempHole;
			SetGoreZone(monster, "gz_rforearmcuff", GZ_ARM_LOWER_RIGHT);
			SetGoreBlownCap(monster, "_Cap_rforearmcuff_rbicep", GZ_ARM_LOWER_RIGHT);
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_coathips");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_GUT_FRONT].parent_id=
			gzones[GZ_GUT_BACK].parent_id=
			gzones[GZ_LEG_UPPER_RIGHT_FRONT].parent_id=
			gzones[GZ_LEG_UPPER_RIGHT_BACK].parent_id=
			gzones[GZ_LEG_UPPER_LEFT_FRONT].parent_id=
			gzones[GZ_LEG_UPPER_LEFT_BACK].parent_id=tempHole;
		SetGoreZone(monster, "gz_coathips_fgut", GZ_GUT_FRONT);
		SetGoreZone(monster, "gz_coathips_bgut", GZ_GUT_BACK);
		SetGoreZone(monster, "gz_coathips_rbthigh", GZ_LEG_UPPER_RIGHT_BACK);
		SetGoreZone(monster, "gz_coathips_rfthigh", GZ_LEG_UPPER_RIGHT_FRONT);
		SetGoreZone(monster, "gz_coathips_lbthigh", GZ_LEG_UPPER_LEFT_BACK);
		SetGoreZone(monster, "gz_coathips_lfthigh", GZ_LEG_UPPER_LEFT_FRONT);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_hips");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_GUT_FRONT].parent_id=tempHole;
			gzones[GZ_GUT_BACK].parent_id=tempHole;
			SetGoreZone(monster, "gz_hips_fgut", GZ_GUT_FRONT);
			SetGoreZone(monster, "gz_hips_bgut", GZ_GUT_BACK);
			SetGoreCapBolt(monster, "gbolt_Cap_hips_rthigh", GZ_LEG_UPPER_RIGHT_FRONT);
			SetGoreCapBolt(monster, "gbolt_Cap_hips_rthigh", GZ_LEG_UPPER_RIGHT_BACK);
			SetGoreCapBolt(monster, "gbolt_Cap_hips_lthigh", GZ_LEG_UPPER_LEFT_FRONT);
			SetGoreCapBolt(monster, "gbolt_Cap_hips_lthigh", GZ_LEG_UPPER_LEFT_BACK);
			SetGoreBlownCapBolt(monster, "gbolt_Cap_rthigh_hips", GZ_LEG_UPPER_RIGHT_FRONT);
			SetGoreBlownCapBolt(monster, "gbolt_Cap_rthigh_hips", GZ_LEG_UPPER_RIGHT_BACK);
			SetGoreBlownCapBolt(monster, "gbolt_Cap_lthigh_hips", GZ_LEG_UPPER_LEFT_FRONT);
			SetGoreBlownCapBolt(monster, "gbolt_Cap_lthigh_hips", GZ_LEG_UPPER_LEFT_BACK);
			SetGoreCap(monster, "_Cap_hips_rthigh", GZ_LEG_UPPER_RIGHT_FRONT);
			SetGoreCap(monster, "_Cap_hips_rthigh", GZ_LEG_UPPER_RIGHT_BACK);
			SetGoreCap(monster, "_Cap_hips_lthigh", GZ_LEG_UPPER_LEFT_FRONT);
			SetGoreCap(monster, "_Cap_hips_lthigh", GZ_LEG_UPPER_LEFT_BACK);
		}
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rthigh");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_LEG_UPPER_RIGHT_BACK].parent_id=
				gzones[GZ_LEG_UPPER_RIGHT_FRONT].parent_id=tempHole;
			SetGoreZone(monster, "gz_rfthigh", GZ_LEG_UPPER_RIGHT_FRONT);
			SetGoreZone(monster, "gz_rbthigh", GZ_LEG_UPPER_RIGHT_BACK);
			SetGoreCap(monster, "_Cap_rthigh_rcalf", GZ_LEG_LOWER_RIGHT);
			SetGoreCapBolt(monster, "gbolt_Cap_rthigh_rcalf", GZ_LEG_LOWER_RIGHT);
			SetGoreBlownCapBolt(monster, "gbolt_Cap_rcalf_rthigh", GZ_LEG_LOWER_RIGHT);
			SetGoreBlownCap(monster, "_Cap_rthigh_hips", GZ_LEG_UPPER_RIGHT_FRONT);
			SetGoreBlownCap(monster, "_Cap_rthigh_hips", GZ_LEG_UPPER_RIGHT_BACK);
			gzones[GZ_LEG_UPPER_RIGHT_FRONT].childzone=GZ_LEG_UPPER_RIGHT_BACK;
			gzones[GZ_LEG_UPPER_RIGHT_BACK].childzone=GZ_LEG_UPPER_RIGHT_FRONT;
			gzones[GZ_LEG_UPPER_RIGHT_FRONT].blowme=&ecto_blown_parts[GBLOWN_ECTO_THIGHRIGHT];
			gzones[GZ_LEG_UPPER_RIGHT_BACK].blowme=&ecto_blown_parts[GBLOWN_ECTO_THIGHRIGHT];
		}
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lthigh");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_LEG_UPPER_LEFT_BACK].parent_id=
				gzones[GZ_LEG_UPPER_LEFT_FRONT].parent_id=tempHole;
			SetGoreZone(monster, "gz_lfthigh", GZ_LEG_UPPER_LEFT_FRONT);
			SetGoreZone(monster, "gz_lbthigh", GZ_LEG_UPPER_LEFT_BACK);
			SetGoreCap(monster, "_Cap_lthigh_lcalf", GZ_LEG_LOWER_LEFT);
			SetGoreCapBolt(monster, "gbolt_Cap_lthigh_lcalf", GZ_LEG_LOWER_LEFT);
			SetGoreBlownCapBolt(monster, "gbolt_Cap_lcalf_lthigh", GZ_LEG_LOWER_LEFT);
			SetGoreBlownCap(monster, "_Cap_lthigh_hips", GZ_LEG_UPPER_LEFT_FRONT);
			SetGoreBlownCap(monster, "_Cap_lthigh_hips", GZ_LEG_UPPER_LEFT_BACK);
			gzones[GZ_LEG_UPPER_LEFT_FRONT].childzone=GZ_LEG_UPPER_LEFT_BACK;
			gzones[GZ_LEG_UPPER_LEFT_BACK].childzone=GZ_LEG_UPPER_LEFT_FRONT;
			gzones[GZ_LEG_UPPER_LEFT_FRONT].blowme=&ecto_blown_parts[GBLOWN_ECTO_THIGHLEFT];
			gzones[GZ_LEG_UPPER_LEFT_BACK].blowme=&ecto_blown_parts[GBLOWN_ECTO_THIGHLEFT];
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lcalf");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_LEG_LOWER_LEFT].parent_id=tempHole;
		SetGoreZone(monster, "gz_lcalf", GZ_LEG_LOWER_LEFT);
		SetGoreBlownCap(monster, "_Cap_lcalf_lthigh", GZ_LEG_LOWER_LEFT);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lcalfcuff");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_LEG_LOWER_LEFT].parent_id=tempHole;
			SetGoreZone(monster, "gz_lcalfcuff", GZ_LEG_LOWER_LEFT);
			SetGoreBlownCap(monster, "_Cap_lcalfcuff_lthigh", GZ_LEG_LOWER_LEFT);
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rcalf");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_LEG_LOWER_RIGHT].parent_id=tempHole;
		SetGoreZone(monster, "gz_rcalf", GZ_LEG_LOWER_RIGHT);
		SetGoreBlownCap(monster, "_Cap_rcalf_rthigh", GZ_LEG_LOWER_RIGHT);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rcalfcuff");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_LEG_LOWER_RIGHT].parent_id=tempHole;
			SetGoreZone(monster, "gz_rcalfcuff", GZ_LEG_LOWER_RIGHT);
			SetGoreBlownCap(monster, "_Cap_rcalfcuff_rthigh", GZ_LEG_LOWER_RIGHT);
		}
	}
}

gz_blown_part *bodyecto_c::GetBlownPartForIndex(blown_index theIndex)
{
	if (theIndex < 0 || theIndex > GBLOWN_ECTO_NUM)
	{
		return &ecto_blown_parts[GBLOWN_ECTO_NUM];
	}
	return &ecto_blown_parts[theIndex];
}

bodyecto_c::bodyecto_c(bodyecto_c *orig)
: bodyhuman_c(orig)
{
}

void bodyecto_c::Evaluate(bodyecto_c *orig)
{
	bodyhuman_c::Evaluate(orig);
}

void bodyecto_c::Read()
{
	char	loaded[sizeof(bodyecto_c)];

	gi.ReadFromSavegame('AIBE', loaded, sizeof(bodyecto_c));
	Evaluate((bodyecto_c *)loaded);
}

void bodyecto_c::Write()
{
	bodyecto_c	*savable;

	savable = new bodyecto_c(this);
	gi.AppendToSavegame('AIBE', savable, sizeof(*this));
	delete savable;

	WriteGZ();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
//	meso-specific
//
///////////////////////////////////////////////////////////////////////////////////////////////

bodymeso_c::bodymeso_c()
: bodyhuman_c()
{
}

void bodymeso_c::RecognizeGoreZones(edict_t &monster)
{
	GhoulID	tempHole = 0;
	int i;

	InitializeGoreZoneLevels(monster);

	for (i=0;i<numGoreZones;i++)
	{
		gzones[i].damage=gzones[i].damage_level=0;
		gzones[i].id=gzones[i].parent_id=gzones[i].cap=gzones[i].blownCap=NULL_GhoulID;
		gzones[i].blownCapBolt=gzones[i].capbolt=gzones[i].bolt=gzones[i].capPieceBolt=NULL;
		gzones[i].capPiece=NULL;
		gzones[i].blowme=&meso_blown_parts[GBLOWN_MESO_NUM];
		gzones[i].buddyzone=gzones[i].childzone=numGoreZones;
	}

	SetGoreZoneDamageScale(monster);

	if (gi.flrand(0,1) < .7)
	{
		SetGoreCapPiece(monster, "g_neck_torn_anim2", "to_gbolt_Cap_chest_head", GZ_HEAD_BACK);
		SetGoreCapPiece(monster, "g_neck_torn_anim2", "to_gbolt_Cap_chest_head", GZ_HEAD_FRONT);
	}
	else
	{
		SetGoreCapPiece(monster, "g_half_head", "to_gbolt_Cap_chest_head", GZ_HEAD_BACK);
		SetGoreCapPiece(monster, "g_half_head", "to_gbolt_Cap_chest_head", GZ_HEAD_FRONT);
	}
	SetGoreBolt(monster, "gbolt_head_b", GZ_HEAD_BACK);
	SetGoreBolt(monster, "gbolt_head_f", GZ_HEAD_FRONT);

	gzones[GZ_HEAD_BACK].blowme=gzones[GZ_HEAD_FRONT].blowme=&meso_blown_parts[GBLOWN_MESO_HEADBALD];
	gzones[GZ_CHEST_FRONT].blowme=gzones[GZ_CHEST_BACK].blowme=&meso_blown_parts[GBLOWN_MESO_TIGHTCHEST];

	gzones[GZ_SHLDR_RIGHT_FRONT].blowme=&meso_blown_parts[GBLOWN_MESO_SHOULD_RF];
	gzones[GZ_SHLDR_RIGHT_BACK].blowme=&meso_blown_parts[GBLOWN_MESO_SHOULD_RB];
	gzones[GZ_SHLDR_LEFT_FRONT].blowme=&meso_blown_parts[GBLOWN_MESO_SHOULD_LF];
	gzones[GZ_SHLDR_LEFT_BACK].blowme=&meso_blown_parts[GBLOWN_MESO_SHOULD_LB];

	gzones[GZ_ARM_UPPER_RIGHT].blowme=&meso_blown_parts[GBLOWN_MESO_BICEPRIGHT];
	gzones[GZ_ARM_UPPER_LEFT].blowme=&meso_blown_parts[GBLOWN_MESO_BICEPLEFT];
	gzones[GZ_ARM_LOWER_RIGHT].blowme=&meso_blown_parts[GBLOWN_MESO_FOREARMRIGHT];
	gzones[GZ_ARM_LOWER_LEFT].blowme=&meso_blown_parts[GBLOWN_MESO_FOREARMLEFT];
	gzones[GZ_LEG_UPPER_RIGHT_FRONT].blowme=&meso_blown_parts[GBLOWN_MESO_THIGHRIGHT];
	gzones[GZ_LEG_UPPER_RIGHT_BACK].blowme=&meso_blown_parts[GBLOWN_MESO_THIGHRIGHT];
	gzones[GZ_LEG_UPPER_LEFT_FRONT].blowme=&meso_blown_parts[GBLOWN_MESO_THIGHLEFT];
	gzones[GZ_LEG_UPPER_LEFT_BACK].blowme=&meso_blown_parts[GBLOWN_MESO_THIGHLEFT];
	gzones[GZ_GROIN].blowme=&meso_blown_parts[GBLOWN_MESO_GROIN];
	gzones[GZ_FOOT_LEFT].blowme=&meso_blown_parts[GBLOWN_MESO_FOOTLEFT];//icky--don't really want to blow this off, but need something in here that's easy to compare
	gzones[GZ_FOOT_RIGHT].blowme=&meso_blown_parts[GBLOWN_MESO_FOOTRIGHT];//icky--don't really want to blow this off, but need something in here that's easy to compare
	gzones[GZ_NECK].blowme=&meso_blown_parts[GBLOWN_MESO_NECK];//icky--don't really want to blow this off, but need something in here that's easy to compare
	gzones[GZ_LEG_LOWER_LEFT].blowme=&meso_blown_parts[GBLOWN_MESO_CALFLEFT];
	gzones[GZ_LEG_LOWER_RIGHT].blowme=&meso_blown_parts[GBLOWN_MESO_CALFRIGHT];

	gzones[GZ_HEAD_FRONT].childzone=GZ_HEAD_BACK;
	gzones[GZ_HEAD_BACK].childzone=GZ_HEAD_FRONT;

	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips_tight", GZ_SHLDR_RIGHT_FRONT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips_tight", GZ_SHLDR_LEFT_FRONT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips_tight", GZ_SHLDR_RIGHT_BACK);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips_tight", GZ_SHLDR_LEFT_BACK);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips_tight", GZ_CHEST_FRONT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips_tight", GZ_CHEST_BACK);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_head_neck", GZ_HEAD_FRONT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_head_neck", GZ_HEAD_BACK);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_rbicep_chest_tight", GZ_ARM_UPPER_RIGHT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_lbicep_chest_tight", GZ_ARM_UPPER_LEFT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_lforearm_lbicep_t", GZ_ARM_LOWER_LEFT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_rforearm_rbicep_t", GZ_ARM_LOWER_RIGHT);

	SetGoreCapBolt(monster, "gbolt_Cap_hips_chest_tight", GZ_CHEST_FRONT);
	SetGoreCapBolt(monster, "gbolt_Cap_hips_chest_tight", GZ_CHEST_BACK);
	SetGoreCapBolt(monster, "gbolt_Cap_neck_head", GZ_HEAD_FRONT);
	SetGoreCapBolt(monster, "gbolt_Cap_neck_head", GZ_HEAD_BACK);
	SetGoreCapBolt(monster, "gbolt_Cap_chest_rbicep_tight", GZ_ARM_UPPER_RIGHT);
	SetGoreCapBolt(monster, "gbolt_Cap_chest_lbicep_tight", GZ_ARM_UPPER_LEFT);
	SetGoreCapBolt(monster, "gbolt_Cap_lbicep_lforearm_t", GZ_ARM_LOWER_LEFT);
	SetGoreCapBolt(monster, "gbolt_Cap_rbicep_rforearm_t", GZ_ARM_LOWER_RIGHT);

	SetGoreCapBolt(monster, "gbolt_Cap_rthigh_rcalf_tight", GZ_LEG_LOWER_RIGHT);
	SetGoreCapBolt(monster, "gbolt_Cap_lthigh_lcalf_tight", GZ_LEG_LOWER_LEFT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_lcalf_lthigh_tight", GZ_LEG_LOWER_LEFT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_rcalf_rthigh_tight", GZ_LEG_LOWER_RIGHT);

	SetGoreCapBolt(monster, "gbolt_Cap_hips_rthigh_tight", GZ_LEG_UPPER_RIGHT_FRONT);
	SetGoreCapBolt(monster, "gbolt_Cap_hips_rthigh_tight", GZ_LEG_UPPER_RIGHT_BACK);
	SetGoreCapBolt(monster, "gbolt_Cap_hips_lthigh_tight", GZ_LEG_UPPER_LEFT_FRONT);
	SetGoreCapBolt(monster, "gbolt_Cap_hips_lthigh_tight", GZ_LEG_UPPER_LEFT_BACK);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_rthigh_hips_tight", GZ_LEG_UPPER_RIGHT_FRONT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_rthigh_hips_tight", GZ_LEG_UPPER_RIGHT_BACK);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_lthigh_hips_tight", GZ_LEG_UPPER_LEFT_FRONT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_lthigh_hips_tight", GZ_LEG_UPPER_LEFT_BACK);

	SetGoreBolt(monster, "gbolt_shoulder_rf", GZ_SHLDR_RIGHT_FRONT);
	SetGoreBolt(monster, "gbolt_shoulder_lf", GZ_SHLDR_LEFT_FRONT);
	SetGoreBolt(monster, "gbolt_shoulder_rb", GZ_SHLDR_RIGHT_BACK);
	SetGoreBolt(monster, "gbolt_shoulder_lb", GZ_SHLDR_LEFT_BACK);
	SetGoreBolt(monster, "gbolt_chest_f", GZ_CHEST_FRONT);
	SetGoreBolt(monster, "gbolt_chest_b", GZ_CHEST_BACK);
	gzones[GZ_CHEST_BACK].childzone=GZ_CHEST_FRONT;
	gzones[GZ_CHEST_FRONT].childzone=GZ_SHLDR_RIGHT_FRONT;
	gzones[GZ_SHLDR_RIGHT_FRONT].childzone=GZ_SHLDR_LEFT_FRONT;
	gzones[GZ_SHLDR_LEFT_FRONT].childzone=GZ_SHLDR_RIGHT_BACK;
	gzones[GZ_SHLDR_RIGHT_BACK].childzone=GZ_SHLDR_LEFT_BACK;
	gzones[GZ_SHLDR_LEFT_BACK].childzone=GZ_CHEST_BACK;

	gzones[GZ_LEG_UPPER_RIGHT_FRONT].childzone=gzones[GZ_LEG_UPPER_RIGHT_FRONT].buddyzone=GZ_LEG_UPPER_RIGHT_BACK;
	gzones[GZ_LEG_UPPER_RIGHT_BACK].childzone=gzones[GZ_LEG_UPPER_RIGHT_BACK].buddyzone=GZ_LEG_UPPER_RIGHT_FRONT;

	gzones[GZ_LEG_UPPER_LEFT_FRONT].childzone=gzones[GZ_LEG_UPPER_LEFT_FRONT].buddyzone=GZ_LEG_UPPER_LEFT_BACK;
	gzones[GZ_LEG_UPPER_LEFT_BACK].childzone=gzones[GZ_LEG_UPPER_LEFT_BACK].buddyzone=GZ_LEG_UPPER_LEFT_FRONT;

	gzones[GZ_ARM_UPPER_RIGHT].childzone=GZ_ARM_LOWER_RIGHT;

	gzones[GZ_ARM_UPPER_LEFT].childzone=GZ_ARM_LOWER_LEFT;

	gzones[GZ_GROIN].buddyzone=GZ_GUT_FRONT;
	gzones[GZ_GUT_FRONT].buddyzone=GZ_GUT_FRONT_EXTRA;
	gzones[GZ_GUT_BACK].buddyzone=GZ_GUT_BACK_EXTRA;

	gzones[GZ_FOOT_RIGHT].buddyzone=GZ_LEG_LOWER_RIGHT;
	gzones[GZ_FOOT_LEFT].buddyzone=GZ_LEG_LOWER_LEFT;

	SetGoreBolt(monster, "gbolt_gut_f", GZ_GUT_FRONT);
	SetGoreBolt(monster, "gbolt_gut_b", GZ_GUT_BACK);

	SetGoreBolt(monster, "gbolt_groin", GZ_GROIN);

	SetGoreBolt(monster, "sbolt_heel_l", GZ_FOOT_LEFT);//icky--totally inappropriate use of this bolt--but should be safe.

	SetGoreBolt(monster, "sbolt_heel_r", GZ_FOOT_RIGHT);//icky--totally inappropriate use of this bolt--but should be safe.

	SetGoreBolt(monster, "gbolt_cap_head_neck", GZ_NECK);//icky--totally inappropriate use of this bolt--but should be safe.

	// bottom of torso cap
	SetGoreCapPiece(monster, "g_guts_lrg", "to_gbolt_Cap_chest_hips", GZ_CHEST_FRONT);
	SetGoreCapPiece(monster, "g_guts_lrg", "to_gbolt_Cap_chest_hips", GZ_CHEST_BACK);

	// elbow caps
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_ARM_LOWER_RIGHT);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_ARM_LOWER_LEFT);
	
	// shoulder caps
	SetGoreCapPiece(monster, "g_arm_torn_anim1", "to_gbolt_Cap_chest_rbicep", GZ_ARM_UPPER_RIGHT);
	SetGoreCapPiece(monster, "g_arm_torn_anim1", "to_gbolt_Cap_chest_lbicep", GZ_ARM_UPPER_LEFT);

	//hip caps
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_UPPER_RIGHT_FRONT);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_UPPER_RIGHT_BACK);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_UPPER_LEFT_FRONT);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_UPPER_LEFT_BACK);

	// knee caps (ha ha)
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_LOWER_RIGHT);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_LOWER_LEFT);

	
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_baldhead");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_HEAD_BACK].parent_id=tempHole;
		SetGoreZone(monster, "gz_baldhead_b", GZ_HEAD_BACK);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_crewhead");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_HEAD_BACK].parent_id=tempHole;
			SetGoreZone(monster, "gz_crewhead_b", GZ_HEAD_BACK);
		}
		else
		{
			tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_beardhead");
			if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
			{
				gzones[GZ_HEAD_BACK].parent_id=tempHole;
				SetGoreZone(monster, "gz_beardhead_b", GZ_HEAD_BACK);
			}
			else
			{
				tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_hoodhead");
				if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
				{
					gzones[GZ_HEAD_BACK].parent_id=tempHole;
					SetGoreZone(monster, "gz_hoodhead_b", GZ_HEAD_BACK);
				}
				else
				{
					tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_bosshead");
					if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
					{
						gzones[GZ_HEAD_BACK].parent_id=tempHole;
						SetGoreZone(monster, "gz_bosshead_b", GZ_HEAD_BACK);
					}
					else
					{
						tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_maskhead");
						if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
						{
							gzones[GZ_HEAD_BACK].parent_id=gzones[GZ_HEAD_FRONT].parent_id=tempHole;
							SetGoreZone(monster, "gz_maskhead_b", GZ_HEAD_BACK);
							SetGoreZone(monster, "gz_maskhead_f", GZ_HEAD_FRONT);
						}
						else
						{
							tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_helmethead");
							if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
							{
								gzones[GZ_HEAD_BACK].parent_id=tempHole;
							}
						}
					}
				}
			}
		}
	}


	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_comface");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_HEAD_FRONT].parent_id=tempHole;
		SetGoreZone(monster, "gz_comface", GZ_HEAD_FRONT);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_beardface");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_HEAD_FRONT].parent_id=tempHole;
			SetGoreZone(monster, "gz_beardface", GZ_HEAD_FRONT);
		}
		else
		{
			tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_helmetface");
			if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
			{
				gzones[GZ_HEAD_FRONT].parent_id=tempHole;
				SetGoreZone(monster, "gz_helmetface", GZ_HEAD_FRONT);
			}
			else
			{
				tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_bossface");
				if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
				{
					gzones[GZ_HEAD_FRONT].parent_id=tempHole;
					SetGoreZone(monster, "gz_bossface", GZ_HEAD_FRONT);
				}
			}
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_neck");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_NECK].parent_id=tempHole;
		SetGoreCap(monster, "_Cap_neck_head", GZ_HEAD_FRONT);
		SetGoreCap(monster, "_Cap_neck_head", GZ_HEAD_BACK);
		SetGoreBlownCap(monster, "_Cap_head_neck", GZ_HEAD_FRONT);
		SetGoreBlownCap(monster, "_Cap_head_neck", GZ_HEAD_BACK);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_bossneck");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_NECK].parent_id=tempHole;
			SetGoreCap(monster, "_Cap_neck_head_boss", GZ_HEAD_FRONT);
			SetGoreCap(monster, "_Cap_neck_head_boss", GZ_HEAD_BACK);
			SetGoreBlownCap(monster, "_Cap_head_neck_boss", GZ_HEAD_FRONT);
			SetGoreBlownCap(monster, "_Cap_head_neck_boss", GZ_HEAD_BACK);
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_tightchest");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_SHLDR_RIGHT_FRONT].parent_id=gzones[GZ_SHLDR_RIGHT_BACK].parent_id=
			gzones[GZ_SHLDR_LEFT_FRONT].parent_id=gzones[GZ_SHLDR_LEFT_BACK].parent_id=
			gzones[GZ_CHEST_FRONT].parent_id=gzones[GZ_CHEST_BACK].parent_id=tempHole;
		SetGoreCap(monster, "_Cap_chest_rbicep_tight", GZ_ARM_UPPER_RIGHT);
		SetGoreCap(monster, "_Cap_chest_lbicep_tight", GZ_ARM_UPPER_LEFT);
		SetGoreBlownCap(monster, "_Cap_chest_hips_tight", GZ_CHEST_FRONT);
		SetGoreBlownCap(monster, "_Cap_chest_hips_tight", GZ_CHEST_BACK);
		SetGoreZone(monster, "gz_tightchest_rfshldr", GZ_SHLDR_RIGHT_FRONT);
		SetGoreZone(monster, "gz_tightchest_lfshldr", GZ_SHLDR_LEFT_FRONT);
		SetGoreZone(monster, "gz_tightchest_rbshldr", GZ_SHLDR_RIGHT_BACK);
		SetGoreZone(monster, "gz_tightchest_lbshldr", GZ_SHLDR_LEFT_BACK);
		SetGoreZone(monster, "gz_tightchest_f", GZ_CHEST_FRONT);
		SetGoreZone(monster, "gz_tightchest_b", GZ_CHEST_BACK);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_bulkychest");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_SHLDR_RIGHT_FRONT].parent_id=gzones[GZ_SHLDR_RIGHT_BACK].parent_id=
				gzones[GZ_SHLDR_LEFT_FRONT].parent_id=gzones[GZ_SHLDR_LEFT_BACK].parent_id=
				gzones[GZ_CHEST_FRONT].parent_id=gzones[GZ_CHEST_BACK].parent_id=tempHole;
			SetGoreCap(monster, "_Cap_chest_rbicep_bulky", GZ_ARM_UPPER_RIGHT);
			SetGoreCap(monster, "_Cap_chest_lbicep_bulky", GZ_ARM_UPPER_LEFT);
			SetGoreBlownCap(monster, "_Cap_chest_hips_bulky", GZ_CHEST_FRONT);
			SetGoreBlownCap(monster, "_Cap_chest_hips_bulky", GZ_CHEST_BACK);
			SetGoreZone(monster, "gz_bulkychest_rfshldr", GZ_SHLDR_RIGHT_FRONT);
			SetGoreZone(monster, "gz_bulkychest_lfshldr", GZ_SHLDR_LEFT_FRONT);
			SetGoreZone(monster, "gz_bulkychest_rbshldr", GZ_SHLDR_RIGHT_BACK);
			SetGoreZone(monster, "gz_bulkychest_lbshldr", GZ_SHLDR_LEFT_BACK);
			SetGoreZone(monster, "gz_bulkychest_f", GZ_CHEST_FRONT);
			SetGoreZone(monster, "gz_bulkychest_b", GZ_CHEST_BACK);
		}
		else
		{
			tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_chest_b_t");
			if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
			{
				gzones[GZ_SHLDR_RIGHT_FRONT].parent_id=gzones[GZ_SHLDR_RIGHT_BACK].parent_id=
					gzones[GZ_SHLDR_LEFT_FRONT].parent_id=gzones[GZ_SHLDR_LEFT_BACK].parent_id=
					gzones[GZ_CHEST_FRONT].parent_id=gzones[GZ_CHEST_BACK].parent_id=tempHole;
				SetGoreCap(monster, "_Cap_chest_rbicep_bulky", GZ_ARM_UPPER_RIGHT);
				SetGoreCap(monster, "_Cap_chest_lbicep_bulky", GZ_ARM_UPPER_LEFT);
				SetGoreBlownCap(monster, "_Cap_chest_hips_tight", GZ_CHEST_FRONT);
				SetGoreBlownCap(monster, "_Cap_chest_hips_tight", GZ_CHEST_BACK);
				SetGoreZone(monster, "gz_chest_b_t_rfshldr", GZ_SHLDR_RIGHT_FRONT);
				SetGoreZone(monster, "gz_chest_b_t_lfshldr", GZ_SHLDR_LEFT_FRONT);
				SetGoreZone(monster, "gz_chest_b_t_rbshldr", GZ_SHLDR_RIGHT_BACK);
				SetGoreZone(monster, "gz_chest_b_t_lbshldr", GZ_SHLDR_LEFT_BACK);
				SetGoreZone(monster, "gz_chest_b_t_f", GZ_CHEST_FRONT);
				SetGoreZone(monster, "gz_chest_b_t_b", GZ_CHEST_BACK);
			}
			else
			{
				tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_bosschest");
				if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
				{
					gzones[GZ_SHLDR_RIGHT_FRONT].parent_id=gzones[GZ_SHLDR_RIGHT_BACK].parent_id=
						gzones[GZ_SHLDR_LEFT_FRONT].parent_id=gzones[GZ_SHLDR_LEFT_BACK].parent_id=
						gzones[GZ_CHEST_FRONT].parent_id=gzones[GZ_CHEST_BACK].parent_id=tempHole;
					SetGoreCap(monster, "_Cap_chest_rbicep_boss", GZ_ARM_UPPER_RIGHT);
					SetGoreCap(monster, "_Cap_chest_lbicep_boss", GZ_ARM_UPPER_LEFT);
					SetGoreBlownCap(monster, "_Cap_chest_hips_boss", GZ_CHEST_FRONT);
					SetGoreBlownCap(monster, "_Cap_chest_hips_boss", GZ_CHEST_BACK);
					SetGoreZone(monster, "gz_bosschest_rfshldr", GZ_SHLDR_RIGHT_FRONT);
					SetGoreZone(monster, "gz_bosschest_lfshldr", GZ_SHLDR_LEFT_FRONT);
					SetGoreZone(monster, "gz_bosschest_rbshldr", GZ_SHLDR_RIGHT_BACK);
					SetGoreZone(monster, "gz_bosschest_lbshldr", GZ_SHLDR_LEFT_BACK);
					SetGoreZone(monster, "gz_bosschest_f", GZ_CHEST_FRONT);
					SetGoreZone(monster, "gz_bosschest_b", GZ_CHEST_BACK);
				}
					else
					{
					tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_armorchest");
					if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
					{
						gzones[GZ_SHLDR_RIGHT_FRONT].parent_id=gzones[GZ_SHLDR_RIGHT_BACK].parent_id=
							gzones[GZ_SHLDR_LEFT_FRONT].parent_id=gzones[GZ_SHLDR_LEFT_BACK].parent_id=
							gzones[GZ_CHEST_FRONT].parent_id=gzones[GZ_CHEST_BACK].parent_id=tempHole;
						SetGoreBlownCap(monster, "_Cap_chest_hips_bulky", GZ_CHEST_FRONT);
						SetGoreBlownCap(monster, "_Cap_chest_hips_bulky", GZ_CHEST_BACK);
						SetGoreZone(monster, "gz_armorchest_rfshldr", GZ_SHLDR_RIGHT_FRONT);
						SetGoreZone(monster, "gz_armorchest_lfshldr", GZ_SHLDR_LEFT_FRONT);
						SetGoreZone(monster, "gz_armorchest_rbshldr", GZ_SHLDR_RIGHT_BACK);
						SetGoreZone(monster, "gz_armorchest_lbshldr", GZ_SHLDR_LEFT_BACK);
						SetGoreZone(monster, "gz_armorchest_f", GZ_CHEST_FRONT);
						SetGoreZone(monster, "gz_armorchest_b", GZ_CHEST_BACK);
					}
					else
					{
						tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_maskchest");
						if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
						{
							gzones[GZ_CHEST_FRONT].parent_id=gzones[GZ_CHEST_BACK].parent_id=tempHole;
							SetGoreCap(monster, "_Cap_chest_rbicep_bulky", GZ_ARM_UPPER_RIGHT);
							SetGoreCap(monster, "_Cap_chest_lbicep_bulky", GZ_ARM_UPPER_LEFT);
							SetGoreBlownCap(monster, "_Cap_chest_hips_bulky", GZ_CHEST_FRONT);
							SetGoreBlownCap(monster, "_Cap_chest_hips_bulky", GZ_CHEST_BACK);
							SetGoreZone(monster, "gz_maskchest_f", GZ_CHEST_FRONT);
							SetGoreZone(monster, "gz_maskchest_b", GZ_CHEST_BACK);

							//no blown off heads!  and get off my lawn, you damn kids!
							SetGoreCapBolt(monster, "no_gore_for_me_please", GZ_HEAD_FRONT);
							SetGoreCapBolt(monster, "no_gore_for_me_please", GZ_HEAD_BACK);
						}
						tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_maskneck");
						if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
						{
							gzones[GZ_SHLDR_RIGHT_FRONT].parent_id=gzones[GZ_SHLDR_RIGHT_BACK].parent_id=
								gzones[GZ_SHLDR_LEFT_FRONT].parent_id=gzones[GZ_SHLDR_LEFT_BACK].parent_id=tempHole;
							SetGoreZone(monster, "gz_maskneck_rfshldr", GZ_SHLDR_RIGHT_FRONT);
							SetGoreZone(monster, "gz_maskneck_lfshldr", GZ_SHLDR_LEFT_FRONT);
							SetGoreZone(monster, "gz_maskneck_rbshldr", GZ_SHLDR_RIGHT_BACK);
							SetGoreZone(monster, "gz_maskneck_lbshldr", GZ_SHLDR_LEFT_BACK);
						}
						else
						{
							tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_hoodneck");
							if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
							{
								gzones[GZ_SHLDR_RIGHT_FRONT].parent_id=gzones[GZ_SHLDR_RIGHT_BACK].parent_id=
									gzones[GZ_SHLDR_LEFT_FRONT].parent_id=gzones[GZ_SHLDR_LEFT_BACK].parent_id=tempHole;
								SetGoreZone(monster, "gz_hoodneck_rfshldr", GZ_SHLDR_RIGHT_FRONT);
								SetGoreZone(monster, "gz_hoodneck_lfshldr", GZ_SHLDR_LEFT_FRONT);
								SetGoreZone(monster, "gz_hoodneck_rbshldr", GZ_SHLDR_RIGHT_BACK);
								SetGoreZone(monster, "gz_hoodneck_lbshldr", GZ_SHLDR_LEFT_BACK);
							}
						}
					}
				}
			}
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rbicep_b_b");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_ARM_UPPER_RIGHT].parent_id=tempHole;
		SetGoreZone(monster, "gz_rbicep_b_b", GZ_ARM_UPPER_RIGHT);
		SetGoreBlownCap(monster, "_Cap_rbicep_chest_bulky", GZ_ARM_UPPER_RIGHT);
		SetGoreCap(monster, "_Cap_rbicep_rforearm_b", GZ_ARM_LOWER_RIGHT);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rbicep_b_t");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_ARM_UPPER_RIGHT].parent_id=tempHole;
			SetGoreZone(monster, "gz_rbicep_b_t", GZ_ARM_UPPER_RIGHT);
			SetGoreBlownCap(monster, "_Cap_rbicep_chest_bulky", GZ_ARM_UPPER_RIGHT);
			SetGoreCap(monster, "_Cap_rbicep_rforearm_t", GZ_ARM_LOWER_RIGHT);
		}
		else
		{
			tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rbicep_t_b");
			if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
			{
				gzones[GZ_ARM_UPPER_RIGHT].parent_id=tempHole;
				SetGoreZone(monster, "gz_rbicep_t_b", GZ_ARM_UPPER_RIGHT);
				SetGoreBlownCap(monster, "_Cap_rbicep_chest_tight", GZ_ARM_UPPER_RIGHT);
				SetGoreCap(monster, "_Cap_rbicep_rforearm_b", GZ_ARM_LOWER_RIGHT);
			}
			else
			{
				tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rbicep_t_t");
				if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
				{
					gzones[GZ_ARM_UPPER_RIGHT].parent_id=tempHole;
					SetGoreZone(monster, "gz_rbicep_t_t", GZ_ARM_UPPER_RIGHT);
					SetGoreBlownCap(monster, "_Cap_rbicep_chest_tight", GZ_ARM_UPPER_RIGHT);
					SetGoreCap(monster, "_Cap_rbicep_rforearm_t", GZ_ARM_LOWER_RIGHT);
				}
				else
				{
					tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rbossbicep");
					if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
					{
						gzones[GZ_ARM_UPPER_RIGHT].parent_id=tempHole;
						SetGoreZone(monster, "gz_rbossbicep", GZ_ARM_UPPER_RIGHT);
						SetGoreBlownCap(monster, "_Cap_rbicep_chest_boss", GZ_ARM_UPPER_RIGHT);
						SetGoreCap(monster, "_Cap_rbicep_rforearm_boss", GZ_ARM_LOWER_RIGHT);
					}
				}
			}
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lbicep_b_b");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_ARM_UPPER_LEFT].parent_id=tempHole;
		SetGoreZone(monster, "gz_lbicep_b_b", GZ_ARM_UPPER_LEFT);
		SetGoreBlownCap(monster, "_Cap_lbicep_chest_bulky", GZ_ARM_UPPER_LEFT);
		SetGoreCap(monster, "_Cap_lbicep_lforearm_b", GZ_ARM_LOWER_LEFT);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lbicep_b_t");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_ARM_UPPER_LEFT].parent_id=tempHole;
			SetGoreZone(monster, "gz_lbicep_b_t", GZ_ARM_UPPER_LEFT);
			SetGoreBlownCap(monster, "_Cap_lbicep_chest_bulky", GZ_ARM_UPPER_LEFT);
			SetGoreCap(monster, "_Cap_lbicep_lforearm_t", GZ_ARM_LOWER_LEFT);
		}
		else
		{
			tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lbicep_t_b");
			if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
			{
				gzones[GZ_ARM_UPPER_LEFT].parent_id=tempHole;
				SetGoreZone(monster, "gz_lbicep_t_b", GZ_ARM_UPPER_LEFT);
				SetGoreBlownCap(monster, "_Cap_lbicep_chest_tight", GZ_ARM_UPPER_LEFT);
				SetGoreCap(monster, "_Cap_lbicep_lforearm_b", GZ_ARM_LOWER_LEFT);
			}
			else
			{
				tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lbicep_t_t");
				if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
				{
					gzones[GZ_ARM_UPPER_LEFT].parent_id=tempHole;
					SetGoreZone(monster, "gz_lbicep_t_t", GZ_ARM_UPPER_LEFT);
					SetGoreBlownCap(monster, "_Cap_lbicep_chest_tight", GZ_ARM_UPPER_LEFT);
					SetGoreCap(monster, "_Cap_lbicep_lforearm_t", GZ_ARM_LOWER_LEFT);
				}
				else
				{
					tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lbossbicep");
					if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
					{
						gzones[GZ_ARM_UPPER_LEFT].parent_id=tempHole;
						SetGoreZone(monster, "gz_lbossbicep", GZ_ARM_UPPER_LEFT);
						SetGoreBlownCap(monster, "_Cap_lbicep_chest_boss", GZ_ARM_UPPER_LEFT);
						SetGoreCap(monster, "_Cap_lbicep_lforearm_boss", GZ_ARM_LOWER_LEFT);
					}
				}
			}
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lforearm_b_b");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_ARM_LOWER_LEFT].parent_id=tempHole;
		SetGoreZone(monster, "gz_lforearm_b_b", GZ_ARM_LOWER_LEFT);
		SetGoreBlownCap(monster, "_Cap_lforearm_lbicep_b", GZ_ARM_LOWER_LEFT);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lforearm_b_t");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_ARM_LOWER_LEFT].parent_id=tempHole;
			SetGoreZone(monster, "gz_lforearm_b_t", GZ_ARM_LOWER_LEFT);
			SetGoreBlownCap(monster, "_Cap_lforearm_lbicep_b", GZ_ARM_LOWER_LEFT);
		}
		else
		{
			tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lforearm_t_t");
			if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
			{
				gzones[GZ_ARM_LOWER_LEFT].parent_id=tempHole;
				SetGoreZone(monster, "gz_lforearm_t_t", GZ_ARM_LOWER_LEFT);
				SetGoreBlownCap(monster, "_Cap_lforearm_lbicep_t", GZ_ARM_LOWER_LEFT);
			}
			else
			{
				tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lforearm_t_b");
				if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
				{
					gzones[GZ_ARM_LOWER_LEFT].parent_id=tempHole;
					SetGoreZone(monster, "gz_lforearm_t_b", GZ_ARM_LOWER_LEFT);
					SetGoreBlownCap(monster, "_Cap_lforearm_lbicep_t", GZ_ARM_LOWER_LEFT);
				}
				else
				{
					tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lforearm_t_c");
					if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
					{
						gzones[GZ_ARM_LOWER_LEFT].parent_id=tempHole;
						SetGoreZone(monster, "gz_lforearm_t_c", GZ_ARM_LOWER_LEFT);
						SetGoreBlownCap(monster, "_Cap_lforearm_lbicep_t", GZ_ARM_LOWER_LEFT);
					}
					else
					{
						tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lbossforearm");
						if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
						{
							gzones[GZ_ARM_LOWER_LEFT].parent_id=tempHole;
							SetGoreZone(monster, "gz_lbossforearm", GZ_ARM_LOWER_LEFT);
							SetGoreBlownCap(monster, "_Cap_lforearm_lbicep_boss", GZ_ARM_LOWER_LEFT);
						}
					}
				}
			}
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rforearm_b_b");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_ARM_LOWER_RIGHT].parent_id=tempHole;
		SetGoreZone(monster, "gz_rforearm_b_b", GZ_ARM_LOWER_RIGHT);
		SetGoreBlownCap(monster, "_Cap_rforearm_rbicep_b", GZ_ARM_LOWER_RIGHT);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rforearm_b_t");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_ARM_LOWER_RIGHT].parent_id=tempHole;
			SetGoreZone(monster, "gz_rforearm_b_t", GZ_ARM_LOWER_RIGHT);
			SetGoreBlownCap(monster, "_Cap_rforearm_rbicep_b", GZ_ARM_LOWER_RIGHT);
		}
		else
		{
			tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rforearm_t_t");
			if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
			{
				gzones[GZ_ARM_LOWER_RIGHT].parent_id=tempHole;
				SetGoreZone(monster, "gz_rforearm_t_t", GZ_ARM_LOWER_RIGHT);
				SetGoreBlownCap(monster, "_Cap_rforearm_rbicep_t", GZ_ARM_LOWER_RIGHT);
			}
			else
			{
				tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rforearm_t_b");
				if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
				{
					gzones[GZ_ARM_LOWER_RIGHT].parent_id=tempHole;
					SetGoreZone(monster, "gz_rforearm_t_b", GZ_ARM_LOWER_RIGHT);
					SetGoreBlownCap(monster, "_Cap_rforearm_rbicep_t", GZ_ARM_LOWER_RIGHT);
				}
				else
				{
					tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rforearm_t_c");
					if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
					{
						gzones[GZ_ARM_LOWER_RIGHT].parent_id=tempHole;
						SetGoreZone(monster, "gz_rforearm_t_c", GZ_ARM_LOWER_RIGHT);
						SetGoreBlownCap(monster, "_Cap_rforearm_rbicep_t", GZ_ARM_LOWER_RIGHT);
					}
					else
					{
						tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rbossforearm");
						if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
						{
							gzones[GZ_ARM_LOWER_RIGHT].parent_id=tempHole;
							SetGoreZone(monster, "gz_rbossforearm", GZ_ARM_LOWER_RIGHT);
							SetGoreBlownCap(monster, "_Cap_rforearm_rbicep_boss", GZ_ARM_LOWER_RIGHT);
						}
					}
				}
			}
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_coathips");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_GUT_FRONT].parent_id=
			gzones[GZ_GUT_BACK].parent_id=
			gzones[GZ_GROIN].parent_id=
										tempHole;
		SetGoreZone(monster, "gz_coathips_lfthigh", GZ_GUT_FRONT_EXTRA);
		SetGoreZone(monster, "gz_coathips_lbthigh", GZ_GUT_BACK_EXTRA);
		SetGoreZone(monster, "gz_coathips_rfthigh", GZ_GUT_FRONT);
		SetGoreZone(monster, "gz_coathips_rbthigh", GZ_GUT_BACK);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_tighthips");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_GUT_FRONT].parent_id=
				gzones[GZ_GROIN].parent_id=
				gzones[GZ_GUT_BACK].parent_id=tempHole;
			SetGoreZone(monster, "gz_tighthips_f", GZ_GUT_FRONT);
			SetGoreZone(monster, "gz_tighthips_b", GZ_GUT_BACK);
			SetGoreCap(monster, "_Cap_hips_chest_tight", GZ_CHEST_FRONT);
			SetGoreCap(monster, "_Cap_hips_chest_tight", GZ_CHEST_BACK);
			SetGoreCap(monster, "_Cap_hips_rthigh_tight", GZ_LEG_UPPER_RIGHT_FRONT);
			SetGoreCap(monster, "_Cap_hips_rthigh_tight", GZ_LEG_UPPER_RIGHT_BACK);
			SetGoreCap(monster, "_Cap_hips_lthigh_tight", GZ_LEG_UPPER_LEFT_FRONT);
			SetGoreCap(monster, "_Cap_hips_lthigh_tight", GZ_LEG_UPPER_LEFT_BACK);
		}
		else
		{
			tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_bulkyhips");
			if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
			{
				gzones[GZ_GUT_FRONT].parent_id=
					gzones[GZ_GROIN].parent_id=
					gzones[GZ_GUT_BACK].parent_id=tempHole;
				SetGoreZone(monster, "gz_bulkyhips_fgut", GZ_GUT_FRONT);
				SetGoreZone(monster, "gz_bulkyhips_bgut", GZ_GUT_BACK);
				SetGoreCap(monster, "_Cap_hips_chest_bulky", GZ_CHEST_FRONT);
				SetGoreCap(monster, "_Cap_hips_chest_bulky", GZ_CHEST_BACK);
				SetGoreCap(monster, "_Cap_hips_rthigh_bulky", GZ_LEG_UPPER_RIGHT_FRONT);
				SetGoreCap(monster, "_Cap_hips_rthigh_bulky", GZ_LEG_UPPER_RIGHT_BACK);
				SetGoreCap(monster, "_Cap_hips_lthigh_bulky", GZ_LEG_UPPER_LEFT_FRONT);
				SetGoreCap(monster, "_Cap_hips_lthigh_bulky", GZ_LEG_UPPER_LEFT_BACK);
			}
			else
			{
				tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_bosships");
				if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
				{
					gzones[GZ_GUT_FRONT].parent_id=
						gzones[GZ_GROIN].parent_id=
						gzones[GZ_GUT_BACK].parent_id=tempHole;
					SetGoreZone(monster, "gz_bosships_fgut", GZ_GUT_FRONT);
					SetGoreZone(monster, "gz_bosships_bgut", GZ_GUT_BACK);
					SetGoreCap(monster, "_Cap_hips_chest_boss", GZ_CHEST_FRONT);
					SetGoreCap(monster, "_Cap_hips_chest_boss", GZ_CHEST_BACK);
					SetGoreCap(monster, "_Cap_hips_rthigh_boss", GZ_LEG_UPPER_RIGHT_FRONT);
					SetGoreCap(monster, "_Cap_hips_rthigh_boss", GZ_LEG_UPPER_RIGHT_BACK);
					SetGoreCap(monster, "_Cap_hips_lthigh_boss", GZ_LEG_UPPER_LEFT_FRONT);
					SetGoreCap(monster, "_Cap_hips_lthigh_boss", GZ_LEG_UPPER_LEFT_BACK);
				}
			}
		}
	}
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rtightthigh");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_LEG_UPPER_RIGHT_BACK].parent_id=
			gzones[GZ_LEG_UPPER_RIGHT_FRONT].parent_id=tempHole;
		SetGoreZone(monster, "gz_rtightthigh_f", GZ_LEG_UPPER_RIGHT_FRONT);
		SetGoreZone(monster, "gz_rtightthigh_b", GZ_LEG_UPPER_RIGHT_BACK);
		SetGoreCap(monster, "_Cap_rthigh_rcalf_tight", GZ_LEG_LOWER_RIGHT);
		SetGoreBlownCap(monster, "_Cap_rthigh_hips_tight", GZ_LEG_UPPER_RIGHT_FRONT);
		SetGoreBlownCap(monster, "_Cap_rthigh_hips_tight", GZ_LEG_UPPER_RIGHT_BACK);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rbulkythigh");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_LEG_UPPER_RIGHT_BACK].parent_id=
				gzones[GZ_LEG_UPPER_RIGHT_FRONT].parent_id=tempHole;
			SetGoreZone(monster, "gz_rbulkythigh_f", GZ_LEG_UPPER_RIGHT_FRONT);
			SetGoreZone(monster, "gz_rbulkythigh_b", GZ_LEG_UPPER_RIGHT_BACK);
			SetGoreCap(monster, "_Cap_rthigh_rcalf_bulky", GZ_LEG_LOWER_RIGHT);
			SetGoreBlownCap(monster, "_Cap_rthigh_hips_bulky", GZ_LEG_UPPER_RIGHT_FRONT);
			SetGoreBlownCap(monster, "_Cap_rthigh_hips_bulky", GZ_LEG_UPPER_RIGHT_BACK);
		}
		else
		{
			tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rbossthigh");
			if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
			{
				gzones[GZ_LEG_UPPER_RIGHT_BACK].parent_id=
					gzones[GZ_LEG_UPPER_RIGHT_FRONT].parent_id=tempHole;
				SetGoreZone(monster, "gz_rbossthigh_f", GZ_LEG_UPPER_RIGHT_FRONT);
				SetGoreZone(monster, "gz_rbossthigh_b", GZ_LEG_UPPER_RIGHT_BACK);
				SetGoreCap(monster, "_Cap_rthigh_rcalf_boss", GZ_LEG_LOWER_RIGHT);
				SetGoreBlownCap(monster, "_Cap_rthigh_hips_boss", GZ_LEG_UPPER_RIGHT_FRONT);
				SetGoreBlownCap(monster, "_Cap_rthigh_hips_boss", GZ_LEG_UPPER_RIGHT_BACK);
			}
		}
	}
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_ltightthigh");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_LEG_UPPER_LEFT_BACK].parent_id=
			gzones[GZ_LEG_UPPER_LEFT_FRONT].parent_id=tempHole;
		SetGoreZone(monster, "gz_ltightthigh_f", GZ_LEG_UPPER_LEFT_FRONT);
		SetGoreZone(monster, "gz_ltightthigh_b", GZ_LEG_UPPER_LEFT_BACK);
		SetGoreCap(monster, "_Cap_lthigh_lcalf_tight", GZ_LEG_LOWER_LEFT);
		SetGoreBlownCap(monster, "_Cap_lthigh_hips_tight", GZ_LEG_UPPER_LEFT_FRONT);
		SetGoreBlownCap(monster, "_Cap_lthigh_hips_tight", GZ_LEG_UPPER_LEFT_BACK);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lbulkythigh");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_LEG_UPPER_LEFT_BACK].parent_id=
				gzones[GZ_LEG_UPPER_LEFT_FRONT].parent_id=tempHole;
			SetGoreZone(monster, "gz_lbulkythigh_f", GZ_LEG_UPPER_LEFT_FRONT);
			SetGoreZone(monster, "gz_lbulkythigh_b", GZ_LEG_UPPER_LEFT_BACK);
			SetGoreCap(monster, "_Cap_lthigh_lcalf_bulky", GZ_LEG_LOWER_LEFT);
			SetGoreBlownCap(monster, "_Cap_lthigh_hips_bulky", GZ_LEG_UPPER_LEFT_FRONT);
			SetGoreBlownCap(monster, "_Cap_lthigh_hips_bulky", GZ_LEG_UPPER_LEFT_BACK);
		}
		else
		{
			tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lbossthigh");
			if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
			{
				gzones[GZ_LEG_UPPER_LEFT_BACK].parent_id=
					gzones[GZ_LEG_UPPER_LEFT_FRONT].parent_id=tempHole;
				SetGoreZone(monster, "gz_lbossthigh_f", GZ_LEG_UPPER_LEFT_FRONT);
				SetGoreZone(monster, "gz_lbossthigh_b", GZ_LEG_UPPER_LEFT_BACK);
				SetGoreCap(monster, "_Cap_lthigh_lcalf_boss", GZ_LEG_LOWER_LEFT);
				SetGoreBlownCap(monster, "_Cap_lthigh_hips_boss", GZ_LEG_UPPER_LEFT_FRONT);
				SetGoreBlownCap(monster, "_Cap_lthigh_hips_boss", GZ_LEG_UPPER_LEFT_BACK);
			}
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_ltightcalf");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_LEG_LOWER_LEFT].parent_id=tempHole;
		SetGoreZone(monster, "gz_ltightcalf", GZ_LEG_LOWER_LEFT);
		SetGoreBlownCap(monster, "_Cap_lcalf_lthigh_tight", GZ_LEG_LOWER_LEFT);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lcalf_t_b");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_LEG_LOWER_LEFT].parent_id=tempHole;
			SetGoreZone(monster, "gz_lcalf_t_b", GZ_LEG_LOWER_LEFT);
			SetGoreBlownCap(monster, "_Cap_lcalf_lthigh_tight", GZ_LEG_LOWER_LEFT);
		}
		else
		{
			tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lbulkycalf");
			if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
			{
				gzones[GZ_LEG_LOWER_LEFT].parent_id=tempHole;
				SetGoreZone(monster, "gz_lbulkycalf", GZ_LEG_LOWER_LEFT);
				SetGoreBlownCap(monster, "_Cap_lcalf_lthigh_bulky", GZ_LEG_LOWER_LEFT);
			}
			else
			{
				tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lbosscalf");
				if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
				{
					gzones[GZ_LEG_LOWER_LEFT].parent_id=tempHole;
					SetGoreZone(monster, "gz_lbosscalf", GZ_LEG_LOWER_LEFT);
					SetGoreBlownCap(monster, "_Cap_lcalf_lthigh_boss", GZ_LEG_LOWER_LEFT);
				}
				else
				{
					tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lcalfcuff");
					if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
					{
						gzones[GZ_LEG_LOWER_LEFT].parent_id=tempHole;
						SetGoreZone(monster, "gz_lcalfcuff", GZ_LEG_LOWER_LEFT);
						SetGoreBlownCap(monster, "_Cap_lcalf_lthigh_tight", GZ_LEG_LOWER_LEFT);
					}
				}
			}
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rtightcalf");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_LEG_LOWER_RIGHT].parent_id=tempHole;
		SetGoreZone(monster, "gz_rtightcalf", GZ_LEG_LOWER_RIGHT);
		SetGoreBlownCap(monster, "_Cap_rcalf_rthigh_tight", GZ_LEG_LOWER_RIGHT);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rcalf_t_b");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_LEG_LOWER_RIGHT].parent_id=tempHole;
			SetGoreZone(monster, "gz_rcalf_t_b", GZ_LEG_LOWER_RIGHT);
			SetGoreBlownCap(monster, "_Cap_rcalf_rthigh_tight", GZ_LEG_LOWER_RIGHT);
		}
		else
		{
			tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rbulkycalf");
			if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
			{
				gzones[GZ_LEG_LOWER_RIGHT].parent_id=tempHole;
				SetGoreZone(monster, "gz_rbulkycalf", GZ_LEG_LOWER_RIGHT);
				SetGoreBlownCap(monster, "_Cap_rcalf_rthigh_bulky", GZ_LEG_LOWER_RIGHT);
			}
			else
			{
				tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rbosscalf");
				if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
				{
					gzones[GZ_LEG_LOWER_RIGHT].parent_id=tempHole;
					SetGoreZone(monster, "gz_rbosscalf", GZ_LEG_LOWER_RIGHT);
					SetGoreBlownCap(monster, "_Cap_rcalf_rthigh_boss", GZ_LEG_LOWER_RIGHT);
				}
				else
				{
					tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rcalfcuff");
					if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
					{
						gzones[GZ_LEG_LOWER_RIGHT].parent_id=tempHole;
						SetGoreZone(monster, "gz_rcalfcuff", GZ_LEG_LOWER_RIGHT);
						SetGoreBlownCap(monster, "_Cap_rcalf_rthigh_tight", GZ_LEG_LOWER_RIGHT);
					}
				}
			}
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lbootfoot");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_FOOT_LEFT].parent_id=tempHole;
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lshoefoot");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_FOOT_LEFT].parent_id=tempHole;
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rbootfoot");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_FOOT_RIGHT].parent_id=tempHole;
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rshoefoot");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_FOOT_RIGHT].parent_id=tempHole;
		}
	}
}

gz_blown_part *bodymeso_c::GetBlownPartForIndex(blown_index theIndex)
{
	if (theIndex < 0 || theIndex > GBLOWN_MESO_NUM)
	{
		return &meso_blown_parts[GBLOWN_MESO_NUM];
	}
	return &meso_blown_parts[theIndex];
}

bodymeso_c::bodymeso_c(bodymeso_c *orig)
: bodyhuman_c(orig)
{
}

void bodymeso_c::Evaluate(bodymeso_c *orig)
{
	bodyhuman_c::Evaluate(orig);
}

void bodymeso_c::Read()
{
	char	loaded[sizeof(bodymeso_c)];

	gi.ReadFromSavegame('AIBM', loaded, sizeof(bodymeso_c));
	Evaluate((bodymeso_c *)loaded);
}

void bodymeso_c::Write()
{
	bodymeso_c	*savable;

	savable = new bodymeso_c(this);
	gi.AppendToSavegame('AIBM', savable, sizeof(*this));
	delete savable;

	WriteGZ();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
//	female-specific
//
///////////////////////////////////////////////////////////////////////////////////////////////

bodyfemale_c::bodyfemale_c()
: bodyhuman_c()
{
}

void bodyfemale_c::RecognizeGoreZones(edict_t &monster)
{
	GhoulID	tempHole = 0;
	int i;

	InitializeGoreZoneLevels(monster);

	for (i=0;i<numGoreZones;i++)
	{
		gzones[i].damage=gzones[i].damage_level=0;
		gzones[i].id=gzones[i].parent_id=gzones[i].cap=gzones[i].blownCap=NULL_GhoulID;
		gzones[i].blownCapBolt=gzones[i].capbolt=gzones[i].bolt=gzones[i].capPieceBolt=NULL;
		gzones[i].capPiece=NULL;
		gzones[i].blowme=&female_blown_parts[GBLOWN_FEMALE_NUM];
		gzones[i].buddyzone=gzones[i].childzone=numGoreZones;
	}

	SetGoreZoneDamageScale(monster);

	if (gi.flrand(0,1) < .7)
	{
		SetGoreCapPiece(monster, "g_neck_torn_anim2", "to_gbolt_Cap_chest_head", GZ_HEAD_BACK);
		SetGoreCapPiece(monster, "g_neck_torn_anim2", "to_gbolt_Cap_chest_head", GZ_HEAD_FRONT);
	}
	else
	{
		SetGoreCapPiece(monster, "g_half_head", "to_gbolt_Cap_chest_head", GZ_HEAD_BACK);
		SetGoreCapPiece(monster, "g_half_head", "to_gbolt_Cap_chest_head", GZ_HEAD_FRONT);
	}
	SetGoreBolt(monster, "gbolt_head_b", GZ_HEAD_BACK);
	SetGoreBolt(monster, "gbolt_head_f", GZ_HEAD_FRONT);

	gzones[GZ_HEAD_BACK].blowme=gzones[GZ_HEAD_FRONT].blowme=&female_blown_parts[GBLOWN_FEMALE_HEAD];
	gzones[GZ_CHEST_FRONT].blowme=gzones[GZ_CHEST_BACK].blowme=&female_blown_parts[GBLOWN_FEMALE_CHEST];

	gzones[GZ_SHLDR_RIGHT_FRONT].blowme=&female_blown_parts[GBLOWN_FEMALE_SHOULD_RF];
	gzones[GZ_SHLDR_RIGHT_BACK].blowme=&female_blown_parts[GBLOWN_FEMALE_SHOULD_RB];
	gzones[GZ_SHLDR_LEFT_FRONT].blowme=&female_blown_parts[GBLOWN_FEMALE_SHOULD_LF];
	gzones[GZ_SHLDR_LEFT_BACK].blowme=&female_blown_parts[GBLOWN_FEMALE_SHOULD_LB];

	gzones[GZ_ARM_LOWER_RIGHT].blowme=&female_blown_parts[GBLOWN_FEMALE_FOREARMRIGHT];
	gzones[GZ_ARM_LOWER_LEFT].blowme=&female_blown_parts[GBLOWN_FEMALE_FOREARMLEFT];
	gzones[GZ_LEG_LOWER_LEFT].blowme=&female_blown_parts[GBLOWN_FEMALE_CALFLEFT];
	gzones[GZ_LEG_LOWER_RIGHT].blowme=&female_blown_parts[GBLOWN_FEMALE_CALFRIGHT];
	gzones[GZ_ARM_UPPER_RIGHT].blowme=&female_blown_parts[GBLOWN_FEMALE_BICEPRIGHT];
	gzones[GZ_ARM_UPPER_LEFT].blowme=&female_blown_parts[GBLOWN_FEMALE_BICEPLEFT];
	gzones[GZ_GROIN].blowme=&female_blown_parts[GBLOWN_FEMALE_GROIN];
	gzones[GZ_FOOT_LEFT].blowme=&female_blown_parts[GBLOWN_FEMALE_FOOTLEFT];//icky--don't really want to blow this off, but need something in here that's easy to compare
	gzones[GZ_FOOT_RIGHT].blowme=&female_blown_parts[GBLOWN_FEMALE_FOOTRIGHT];//icky--don't really want to blow this off, but need something in here that's easy to compare
	gzones[GZ_NECK].blowme=&female_blown_parts[GBLOWN_FEMALE_NECK];//icky--don't really want to blow this off, but need something in here that's easy to compare

	gzones[GZ_HEAD_FRONT].childzone=GZ_HEAD_BACK;
	gzones[GZ_HEAD_BACK].childzone=GZ_HEAD_FRONT;

	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips_tight", GZ_SHLDR_RIGHT_FRONT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips_tight", GZ_SHLDR_LEFT_FRONT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips_tight", GZ_SHLDR_RIGHT_BACK);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips_tight", GZ_SHLDR_LEFT_BACK);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips_tight", GZ_CHEST_FRONT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_chest_hips_tight", GZ_CHEST_BACK);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_neck_chest", GZ_HEAD_FRONT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_neck_chest", GZ_HEAD_BACK);

//	SetGoreBlownCapBolt(monster, "gbolt_Cap_rbicep_chest_tight", GZ_ARM_UPPER_RIGHT);
//	SetGoreBlownCapBolt(monster, "gbolt_Cap_lbicep_chest_tight", GZ_ARM_UPPER_LEFT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_rbicep_chest_tight", GZ_ARM_UPPER_LEFT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_lbicep_chest_tight", GZ_ARM_UPPER_RIGHT);

	SetGoreBlownCapBolt(monster, "gbolt_Cap_lforearm_lbicep_tight", GZ_ARM_LOWER_LEFT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_rforearm_rbicep_tight", GZ_ARM_LOWER_RIGHT);

	SetGoreCapBolt(monster, "gbolt_Cap_hips_chest_tight", GZ_CHEST_FRONT);
	SetGoreCapBolt(monster, "gbolt_Cap_hips_chest_tight", GZ_CHEST_BACK);
	SetGoreCapBolt(monster, "gbolt_Cap_chest_neck", GZ_HEAD_FRONT);
	SetGoreCapBolt(monster, "gbolt_Cap_chest_neck", GZ_HEAD_BACK);

//	SetGoreCapBolt(monster, "gbolt_Cap_chest_rbicep_tight", GZ_ARM_UPPER_RIGHT);
//	SetGoreCapBolt(monster, "gbolt_Cap_chest_lbicep_tight", GZ_ARM_UPPER_LEFT);
	SetGoreCapBolt(monster, "gbolt_Cap_chest_rbicep_tight", GZ_ARM_UPPER_LEFT);
	SetGoreCapBolt(monster, "gbolt_Cap_chest_lbicep_tight", GZ_ARM_UPPER_RIGHT);

	SetGoreCapBolt(monster, "gbolt_Cap_lbicep_lforearm_tight", GZ_ARM_LOWER_LEFT);
	SetGoreCapBolt(monster, "gbolt_Cap_rbicep_rforearm_tight", GZ_ARM_LOWER_RIGHT);

	SetGoreCapBolt(monster, "gbolt_Cap_rthigh_rcalf_tight", GZ_LEG_LOWER_RIGHT);
	SetGoreCapBolt(monster, "gbolt_Cap_lthigh_lcalf_tight", GZ_LEG_LOWER_LEFT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_lcalf_lthigh_tight", GZ_LEG_LOWER_LEFT);
	SetGoreBlownCapBolt(monster, "gbolt_Cap_rcalf_rthigh_tight", GZ_LEG_LOWER_RIGHT);

	SetGoreBolt(monster, "gbolt_shoulder_rf", GZ_SHLDR_RIGHT_FRONT);
	SetGoreBolt(monster, "gbolt_shoulder_lf", GZ_SHLDR_LEFT_FRONT);
	SetGoreBolt(monster, "gbolt_shoulder_rb", GZ_SHLDR_RIGHT_BACK);
	SetGoreBolt(monster, "gbolt_shoulder_lb", GZ_SHLDR_LEFT_BACK);
	SetGoreBolt(monster, "gbolt_chest_f", GZ_CHEST_FRONT);
	SetGoreBolt(monster, "gbolt_chest_b", GZ_CHEST_BACK);
	gzones[GZ_CHEST_BACK].childzone=GZ_CHEST_FRONT;
	gzones[GZ_CHEST_FRONT].childzone=GZ_SHLDR_RIGHT_FRONT;
	gzones[GZ_SHLDR_RIGHT_FRONT].childzone=GZ_SHLDR_LEFT_FRONT;
	gzones[GZ_SHLDR_LEFT_FRONT].childzone=GZ_SHLDR_RIGHT_BACK;
	gzones[GZ_SHLDR_RIGHT_BACK].childzone=GZ_SHLDR_LEFT_BACK;
	gzones[GZ_SHLDR_LEFT_BACK].childzone=GZ_CHEST_BACK;

	gzones[GZ_LEG_UPPER_RIGHT_FRONT].childzone=gzones[GZ_LEG_UPPER_RIGHT_FRONT].buddyzone=GZ_LEG_UPPER_RIGHT_BACK;
	gzones[GZ_LEG_UPPER_RIGHT_BACK].childzone=gzones[GZ_LEG_UPPER_RIGHT_BACK].buddyzone=GZ_LEG_UPPER_RIGHT_FRONT;

	gzones[GZ_LEG_UPPER_LEFT_FRONT].childzone=gzones[GZ_LEG_UPPER_LEFT_FRONT].buddyzone=GZ_LEG_UPPER_LEFT_BACK;
	gzones[GZ_LEG_UPPER_LEFT_BACK].childzone=gzones[GZ_LEG_UPPER_LEFT_BACK].buddyzone=GZ_LEG_UPPER_LEFT_FRONT;

	gzones[GZ_ARM_UPPER_RIGHT].childzone=GZ_ARM_LOWER_RIGHT;

	gzones[GZ_ARM_UPPER_LEFT].childzone=GZ_ARM_LOWER_LEFT;

	gzones[GZ_GROIN].buddyzone=GZ_GUT_FRONT;
	gzones[GZ_GUT_FRONT].buddyzone=GZ_GUT_FRONT_EXTRA;
	gzones[GZ_GUT_BACK].buddyzone=GZ_GUT_BACK_EXTRA;

	gzones[GZ_FOOT_RIGHT].buddyzone=GZ_LEG_LOWER_RIGHT;
	gzones[GZ_FOOT_LEFT].buddyzone=GZ_LEG_LOWER_LEFT;

	SetGoreBolt(monster, "gbolt_gut_f", GZ_GUT_FRONT);
	SetGoreBolt(monster, "gbolt_gut_b", GZ_GUT_BACK);

	SetGoreBolt(monster, "gbolt_groin", GZ_GROIN);

	SetGoreBolt(monster, "sbolt_heel_l", GZ_FOOT_LEFT);//icky--totally inappropriate use of this bolt--but should be safe.

	SetGoreBolt(monster, "sbolt_heel_r", GZ_FOOT_RIGHT);//icky--totally inappropriate use of this bolt--but should be safe.

	SetGoreBolt(monster, "gbolt_cap_head_neck", GZ_NECK);//icky--totally inappropriate use of this bolt--but should be safe.

	// bottom of torso cap
	SetGoreCapPiece(monster, "g_guts_lrg", "to_gbolt_Cap_chest_hips", GZ_CHEST_FRONT);
	SetGoreCapPiece(monster, "g_guts_lrg", "to_gbolt_Cap_chest_hips", GZ_CHEST_BACK);

	// elbow caps
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_ARM_LOWER_RIGHT);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_ARM_LOWER_LEFT);
	
	// shoulder caps
	SetGoreCapPiece(monster, "g_arm_torn_anim1", "to_gbolt_Cap_chest_rbicep", GZ_ARM_UPPER_RIGHT);
	SetGoreCapPiece(monster, "g_arm_torn_anim1", "to_gbolt_Cap_chest_lbicep", GZ_ARM_UPPER_LEFT);

	//hip caps
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_UPPER_RIGHT_FRONT);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_UPPER_RIGHT_BACK);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_UPPER_LEFT_FRONT);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_UPPER_LEFT_BACK);

	// knee caps (ha ha)
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_LOWER_RIGHT);
	SetGoreCapPiece(monster, "g_bone_anim1", "to_gbolt", GZ_LEG_LOWER_LEFT);

	
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_crewhair");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_HEAD_BACK].parent_id=tempHole;
		SetGoreZone(monster, "gz_crewhair_b", GZ_HEAD_BACK);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_MEDMHAIRMHEAD");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_HEAD_BACK].parent_id=tempHole;
			SetGoreZone(monster, "gz_MEDMHAIRMHEAD_b", GZ_HEAD_BACK);
		}
	}


	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_comface");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_HEAD_FRONT].parent_id=tempHole;
		SetGoreZone(monster, "gz_comface", GZ_HEAD_FRONT);
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_neck");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_NECK].parent_id=tempHole;
		SetGoreCap(monster, "_Cap_chest_neck", GZ_HEAD_FRONT);
		SetGoreCap(monster, "_Cap_chest_neck", GZ_HEAD_BACK);
		SetGoreBlownCap(monster, "_Cap_neck_chest", GZ_HEAD_FRONT);
		SetGoreBlownCap(monster, "_Cap_neck_chest", GZ_HEAD_BACK);
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_tightchest");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_SHLDR_RIGHT_FRONT].parent_id=gzones[GZ_SHLDR_RIGHT_BACK].parent_id=
			gzones[GZ_SHLDR_LEFT_FRONT].parent_id=gzones[GZ_SHLDR_LEFT_BACK].parent_id=
			gzones[GZ_CHEST_FRONT].parent_id=gzones[GZ_CHEST_BACK].parent_id=tempHole;

//		SetGoreCap(monster, "_Cap_chest_rbicep_tight", GZ_ARM_UPPER_RIGHT);
//		SetGoreCap(monster, "_Cap_chest_lbicep_tight", GZ_ARM_UPPER_LEFT);
		SetGoreCap(monster, "_Cap_chest_rbicep_tight", GZ_ARM_UPPER_LEFT);
		SetGoreCap(monster, "_Cap_chest_lbicep_tight", GZ_ARM_UPPER_RIGHT);

		SetGoreBlownCap(monster, "_Cap_chest_hips_tight", GZ_CHEST_FRONT);
		SetGoreBlownCap(monster, "_Cap_chest_hips_tight", GZ_CHEST_BACK);
		SetGoreZone(monster, "gz_tightchest_rfshldr", GZ_SHLDR_RIGHT_FRONT);
		SetGoreZone(monster, "gz_tightchest_lfshldr", GZ_SHLDR_LEFT_FRONT);
		SetGoreZone(monster, "gz_tightchest_rbshldr", GZ_SHLDR_RIGHT_BACK);
		SetGoreZone(monster, "gz_tightchest_lbshldr", GZ_SHLDR_LEFT_BACK);
		SetGoreZone(monster, "gz_tightchest_fgut", GZ_CHEST_FRONT);
		SetGoreZone(monster, "gz_tightchest_bgut", GZ_CHEST_BACK);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_armorchest");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_SHLDR_RIGHT_FRONT].parent_id=gzones[GZ_SHLDR_RIGHT_BACK].parent_id=
				gzones[GZ_SHLDR_LEFT_FRONT].parent_id=gzones[GZ_SHLDR_LEFT_BACK].parent_id=
				gzones[GZ_CHEST_FRONT].parent_id=gzones[GZ_CHEST_BACK].parent_id=tempHole;
			SetGoreBlownCap(monster, "_Cap_chest_hips_tight", GZ_CHEST_FRONT);
			SetGoreBlownCap(monster, "_Cap_chest_hips_tight", GZ_CHEST_BACK);
			SetGoreZone(monster, "gz_armorchest_rfshldr", GZ_SHLDR_RIGHT_FRONT);
			SetGoreZone(monster, "gz_armorchest_lfshldr", GZ_SHLDR_LEFT_FRONT);
			SetGoreZone(monster, "gz_armorchest_rbshldr", GZ_SHLDR_RIGHT_BACK);
			SetGoreZone(monster, "gz_armorchest_lbshldr", GZ_SHLDR_LEFT_BACK);
			SetGoreZone(monster, "gz_armorchest_f", GZ_CHEST_FRONT);
			SetGoreZone(monster, "gz_armorchest_b", GZ_CHEST_BACK);
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rtightbicep");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_ARM_UPPER_RIGHT].parent_id=tempHole;
		SetGoreZone(monster, "gz_rtightbicep", GZ_ARM_UPPER_RIGHT);

//		SetGoreBlownCap(monster, "_Cap_rbicep_chest_tight", GZ_ARM_UPPER_RIGHT);
		SetGoreBlownCap(monster, "_Cap_lbicep_chest_tight", GZ_ARM_UPPER_RIGHT);

//		SetGoreCap(monster, "_Cap_rbicep_rforearm_tight", GZ_ARM_LOWER_RIGHT);
		SetGoreCap(monster, "_Cap_lbicep_lforearm_tight", GZ_ARM_LOWER_RIGHT);
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_ltightbicep");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_ARM_UPPER_LEFT].parent_id=tempHole;
		SetGoreZone(monster, "gz_ltightbicep", GZ_ARM_UPPER_LEFT);

//		SetGoreBlownCap(monster, "_Cap_lbicep_chest_tight", GZ_ARM_UPPER_LEFT);
		SetGoreBlownCap(monster, "_Cap_rbicep_chest_tight", GZ_ARM_UPPER_LEFT);

//		SetGoreCap(monster, "_Cap_lbicep_lforearm_tight", GZ_ARM_LOWER_LEFT);
		SetGoreCap(monster, "_Cap_rbicep_rforearm_tight", GZ_ARM_LOWER_LEFT);
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_ltightforearm");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_ARM_LOWER_LEFT].parent_id=tempHole;
		SetGoreZone(monster, "gz_ltightforearm", GZ_ARM_LOWER_LEFT);
		SetGoreBlownCap(monster, "_Cap_lforearm_lbicep_tight", GZ_ARM_LOWER_LEFT);
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rtightforearm");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_ARM_LOWER_RIGHT].parent_id=tempHole;
		SetGoreZone(monster, "gz_rtightforearm", GZ_ARM_LOWER_RIGHT);
		SetGoreBlownCap(monster, "_Cap_rforearm_rbicep_tight", GZ_ARM_LOWER_RIGHT);
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_dresships");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_GUT_FRONT].parent_id=
			gzones[GZ_GUT_BACK].parent_id=
			gzones[GZ_LEG_UPPER_RIGHT_FRONT].parent_id=
			gzones[GZ_LEG_UPPER_RIGHT_BACK].parent_id=
			gzones[GZ_LEG_UPPER_LEFT_FRONT].parent_id=
			gzones[GZ_LEG_UPPER_LEFT_BACK].parent_id=tempHole;
		SetGoreZone(monster, "gz_dresships_fgut", GZ_GUT_FRONT);
		SetGoreZone(monster, "gz_dresships_bgut", GZ_GUT_BACK);
		SetGoreZone(monster, "gz_dresships_rbthigh", GZ_LEG_UPPER_RIGHT_BACK);
		SetGoreZone(monster, "gz_dresships_rfthigh", GZ_LEG_UPPER_RIGHT_FRONT);
		SetGoreZone(monster, "gz_dresships_lbthigh", GZ_LEG_UPPER_LEFT_BACK);
		SetGoreZone(monster, "gz_dresships_lfthigh", GZ_LEG_UPPER_LEFT_FRONT);
	}
	else
	{
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_tighthips");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_GUT_FRONT].parent_id=tempHole;
			gzones[GZ_GUT_BACK].parent_id=tempHole;
			SetGoreZone(monster, "gz_tighthips_fgut", GZ_GUT_FRONT);
			SetGoreZone(monster, "gz_tighthips_bgut", GZ_GUT_BACK);
			SetGoreCapBolt(monster, "gbolt_Cap_hips_rthigh_tight", GZ_LEG_UPPER_RIGHT_FRONT);
			SetGoreCapBolt(monster, "gbolt_Cap_hips_rthigh_tight", GZ_LEG_UPPER_RIGHT_BACK);
			SetGoreCapBolt(monster, "gbolt_Cap_hips_lthigh_tight", GZ_LEG_UPPER_LEFT_FRONT);
			SetGoreCapBolt(monster, "gbolt_Cap_hips_lthigh_tight", GZ_LEG_UPPER_LEFT_BACK);
			SetGoreBlownCapBolt(monster, "gbolt_Cap_rthigh_hips_tight", GZ_LEG_UPPER_RIGHT_FRONT);
			SetGoreBlownCapBolt(monster, "gbolt_Cap_rthigh_hips_tight", GZ_LEG_UPPER_RIGHT_BACK);
			SetGoreBlownCapBolt(monster, "gbolt_Cap_lthigh_hips_tight", GZ_LEG_UPPER_LEFT_FRONT);
			SetGoreBlownCapBolt(monster, "gbolt_Cap_lthigh_hips_tight", GZ_LEG_UPPER_LEFT_BACK);
			SetGoreCap(monster, "_Cap_hips_rthigh_tight", GZ_LEG_UPPER_RIGHT_FRONT);
			SetGoreCap(monster, "_Cap_hips_rthigh_tight", GZ_LEG_UPPER_RIGHT_BACK);
			SetGoreCap(monster, "_Cap_hips_lthigh_tight", GZ_LEG_UPPER_LEFT_FRONT);
			SetGoreCap(monster, "_Cap_hips_lthigh_tight", GZ_LEG_UPPER_LEFT_BACK);
		}
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rtightthigh");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_LEG_UPPER_RIGHT_BACK].parent_id=
				gzones[GZ_LEG_UPPER_RIGHT_FRONT].parent_id=tempHole;
			SetGoreZone(monster, "gz_rtightthigh_f", GZ_LEG_UPPER_RIGHT_FRONT);
			SetGoreZone(monster, "gz_rtightthigh_b", GZ_LEG_UPPER_RIGHT_BACK);
			SetGoreCap(monster, "_Cap_rthigh_rcalf_tight", GZ_LEG_LOWER_RIGHT);
			SetGoreCapBolt(monster, "gbolt_Cap_rthigh_rcalf_tight", GZ_LEG_LOWER_RIGHT);
			SetGoreBlownCapBolt(monster, "gbolt_Cap_rcalf_rthigh_tight", GZ_LEG_LOWER_RIGHT);
			SetGoreBlownCap(monster, "_Cap_rthigh_hips_tight", GZ_LEG_UPPER_RIGHT_FRONT);
			SetGoreBlownCap(monster, "_Cap_rthigh_hips_tight", GZ_LEG_UPPER_RIGHT_BACK);
			gzones[GZ_LEG_UPPER_RIGHT_FRONT].childzone=GZ_LEG_UPPER_RIGHT_BACK;
			gzones[GZ_LEG_UPPER_RIGHT_BACK].childzone=GZ_LEG_UPPER_RIGHT_FRONT;
			gzones[GZ_LEG_UPPER_RIGHT_FRONT].blowme=&female_blown_parts[GBLOWN_FEMALE_THIGHRIGHT];
			gzones[GZ_LEG_UPPER_RIGHT_BACK].blowme=&female_blown_parts[GBLOWN_FEMALE_THIGHRIGHT];
		}
		tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_ltightthigh");
		if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
		{
			gzones[GZ_LEG_UPPER_LEFT_BACK].parent_id=
				gzones[GZ_LEG_UPPER_LEFT_FRONT].parent_id=tempHole;
			SetGoreZone(monster, "gz_ltightthigh_f", GZ_LEG_UPPER_LEFT_FRONT);
			SetGoreZone(monster, "gz_ltightthigh_b", GZ_LEG_UPPER_LEFT_BACK);
			SetGoreCap(monster, "_Cap_lthigh_lcalf_tight", GZ_LEG_LOWER_LEFT);
			SetGoreCapBolt(monster, "gbolt_Cap_lthigh_lcalf_tight", GZ_LEG_LOWER_LEFT);
			SetGoreBlownCapBolt(monster, "gbolt_Cap_lcalf_lthigh_tight", GZ_LEG_LOWER_LEFT);
			SetGoreBlownCap(monster, "_Cap_lthigh_hips_tight", GZ_LEG_UPPER_LEFT_FRONT);
			SetGoreBlownCap(monster, "_Cap_lthigh_hips_tight", GZ_LEG_UPPER_LEFT_BACK);
			gzones[GZ_LEG_UPPER_LEFT_FRONT].childzone=GZ_LEG_UPPER_LEFT_BACK;
			gzones[GZ_LEG_UPPER_LEFT_BACK].childzone=GZ_LEG_UPPER_LEFT_FRONT;
			gzones[GZ_LEG_UPPER_LEFT_FRONT].blowme=&female_blown_parts[GBLOWN_FEMALE_THIGHLEFT];
			gzones[GZ_LEG_UPPER_LEFT_BACK].blowme=&female_blown_parts[GBLOWN_FEMALE_THIGHLEFT];
		}
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_ltightcalf");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_LEG_LOWER_LEFT].parent_id=tempHole;
		SetGoreZone(monster, "gz_ltightcalf", GZ_LEG_LOWER_LEFT);
		SetGoreBlownCap(monster, "_Cap_lcalf_lthigh_tight", GZ_LEG_LOWER_LEFT);
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rtightcalf");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_LEG_LOWER_RIGHT].parent_id=tempHole;
		SetGoreZone(monster, "gz_rtightcalf", GZ_LEG_LOWER_RIGHT);
		SetGoreBlownCap(monster, "_Cap_rcalf_rthigh_tight", GZ_LEG_LOWER_RIGHT);
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_lheels");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_FOOT_LEFT].parent_id=tempHole;
	}

	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rheels");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_FOOT_RIGHT].parent_id=tempHole;
	}
}

gz_blown_part *bodyfemale_c::GetBlownPartForIndex(blown_index theIndex)
{
	if (theIndex < 0 || theIndex > GBLOWN_FEMALE_NUM)
	{
		return &female_blown_parts[GBLOWN_FEMALE_NUM];
	}
	return &female_blown_parts[theIndex];
}

bodyfemale_c::bodyfemale_c(bodyfemale_c *orig)
: bodyhuman_c(orig)
{
}

void bodyfemale_c::Evaluate(bodyfemale_c *orig)
{
	bodyhuman_c::Evaluate(orig);
}

void bodyfemale_c::Read()
{
	char	loaded[sizeof(bodyfemale_c)];

	gi.ReadFromSavegame('AIBF', loaded, sizeof(bodyfemale_c));
	Evaluate((bodyfemale_c *)loaded);
}

void bodyfemale_c::Write()
{
	bodyfemale_c	*savable;

	savable = new bodyfemale_c(this);
	gi.AppendToSavegame('AIBF', savable, sizeof(*this));
	delete savable;

	WriteGZ();
}

