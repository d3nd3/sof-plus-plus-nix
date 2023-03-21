#include "g_local.h"
#include "matrix4.h"
#include "ai_private.h"
#include "callback.h"

#if 1

gz_blown_part cow_blown_parts[GBLOWN_COW_NUM+1] =
{
	//
	// cow
	//

	GBLOWN_COW_NUM,		GBLOWN_COW_NUM,				GBLOWNAREA_FINISH,	GBLOWNCODE_SPECIAL,	"",
};

//fixme--change this to include bbox info for shot prt, some kind of indication of what the monster model should do to make up for part loss (eg. change from headbald to headcrew)
extern Vect3 trStartKnarlyGilHack;

///find me a home
void EntToWorldMatrix(vec3_t org, vec3_t angles, Matrix4 &m);

int GetMinimumDamageForLevel(int damageLevel);

// from ai_bodyhuman.cpp
extern void blownpartremove_think(edict_t *self);

char* gzcownames[]=
{
	"GZ_HEAD",
	"GZ_CHEST",
	"GZ_FRONT_R",
	"GZ_FRONT_L",
	"GZ_BACK_R",
	"GZ_BACK_L",
	"NUM_COWGOREZONES"
};

#define MAX_VEL_COMPONENT 50.0

GroundCallback			TheCowGroundCallback;
SetVectorCallback		TheCowSetVectorCallback;
FireCallBack			TheCowFireCallBack;
FollowGenericPivot		TheCowFollowGenericPivot;
MouthCallback			TheCowMouthCallback;
FootRightCallback		TheCowFootRightCallback;
FootLeftCallback		TheCowFootLeftCallback;
MonsterSeqEndCallback	TheCowMonsterSeqEndCallback;
MonsterSeqBeginCallback	TheCowMonsterSeqBeginCallback;

//static GroundTransformationCallBack TheGroundTransformationCallBack;

// for use when the cow's melee attack hits a target
/*
void CowAttackSuccess(edict_t *self, void* data)
{
	bodycow_c*body = NULL;
	if (self && self->ai)
	{
		body=(bodycow_c*)((ai_c*)((ai_public_c*)self->ai))->GetBody();
		((ai_c*)((ai_public_c*)self->ai))->MeleeFrame(*self);		
	}
	if (body)
	{
		body->SetLastAttackTime(*(float*)data);
	}
}
*/
void bodycow_c::PlayMooSound(edict_t &monster)
{
	if (level.time - m_fLastMooSoundTime > 30)
	{
		m_fLastMooSoundTime = level.time;

		switch(gi.irand(0,1))
		{
		case 0:
			gi.sound (&monster, CHAN_BODY, gi.soundindex ("Enemy/Cow/Moo1.wav"), .8, ATTN_NORM, 0);
			break;
		case 1:
			gi.sound (&monster, CHAN_BODY, gi.soundindex ("Enemy/Cow/Moo2.wav"), .8, ATTN_NORM, 0);
			break;
		}
	}
}

void bodycow_c::PlayPainSound(edict_t &monster)
{
	if ( (level.time - m_fLastPainSoundTime > .5) && (monster.health > 0) )
	{
		m_fLastPainSoundTime  = level.time;
		gi.sound (&monster, CHAN_BODY, gi.soundindex ("Enemy/Cow/Die.wav"), .9, ATTN_NORM, 0);
	}
}


bodycow_c::bodycow_c()
{
	emotion = EMOTION_NORMAL;
	emotion_expire=level.time;
	emotionScripted=false;
	currentmove = NULL;
	holdCode = HOLDCODE_NO;
	currentMoveFinished = true;
	frame_flags = 0;
	fJumpDist = 85.0f;
	fJumpSpeed = 100.0f;
	fLastAttackTime = 0;
	fLastHeadTurnTime = 0;
	bTurnedAfterAttack = false;
	FinalAnim = false;
	FinalSound = false;
	nNextShotKillsMe = NEXTSHOT_NULL;
	m_fLastMooSoundTime = 0;
	m_fLastPainSoundTime = 0;
	m_fLastDeathSoundTime = 0;
}
#if 0
mmove_t	*bodycow_c::GetSequenceForMovement(edict_t &monster, vec3_t dest, vec3_t face, vec3_t org, vec3_t ang,  actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);

	mmove_t* returnAnim = NULL;

	if(!currentMoveFinished)
	{
		return currentmove;
	}

	if (IsAvailableSequence(monster, &generic_cow_move_walk))
	{
		return &generic_cow_move_walk;
	}


/*
	// if he was sitting, stand up
	if (currentmove == &generic_dog_move_sit && 
		IsAvailableSequence(monster, &generic_cow_move_standup))
	{
		return &generic_dog_move_standup;
	}

	// this is a sad, sad approach, but we don't want the dog beginning any transitions
	//if he's attacked recently, cuz the ai has a tendency to request attack sequences for
	//several frames, then request a movement sequence for one frame, then more attack seqs, 
	//the result being that if we recommend a sniff transition in the middle of those attacks
	//the overall effect is jerky and yucky
	if (level.time - fLastAttackTime < 1.0f)
	{
		return &generic_dog_move_run;
	}

	// doing a walk rather than a sniff. if we don't have a target, 
	//every now and then turn our head.
	returnAnim = currentmove;
	if (the_ai && !the_ai->HasTarget())
	{
		// if we were sniffing, we have since lost our target. therefore transition to a walk
		if (currentmove == &generic_dog_move_sniff &&
			VerifySequence(monster, &generic_dog_move_walk_from_sniff, reject_actionflags))
		{
			returnAnim = &generic_dog_move_walk_from_sniff;
		}
		else
		{
			// if the dog is walking with a looping anim (looking straight, looking left, looking 
			//right) then we can hold off on transitioning to a different walk...looks more random.
			//if the dog has just been init'd and therefore has no current anim, jump in there and
			//give it one. if it's just finished a transition anim we _have_ to change anims.
			if ((currentmove == NULL || 
				currentmove == &generic_dog_move_walkl_from_walk ||
				currentmove == &generic_dog_move_walk_from_walkl ||
				currentmove == &generic_dog_move_walkr_from_walk ||
				currentmove == &generic_dog_move_walk_from_walkr ||
				currentmove == &generic_dog_move_walkl_walkr) || 
				((currentmove == &generic_dog_move_walk ||
				currentmove == &generic_dog_move_walkl ||
				currentmove == &generic_dog_move_walkr) &&
				!(rand()%3)))
			{
				fLastHeadTurnTime = level.time;
				// if we were walking looking straight (or if we were swiveling left
				//and right), start turning one way or the other
				if (currentmove == &generic_dog_move_walk || currentmove == &generic_dog_move_walkl_walkr)
				{
					int nRand = rand()%3;
					if (nRand == 0 && VerifySequence(monster, &generic_dog_move_walkl_from_walk, reject_actionflags))
					{
						// start turning to look left
						returnAnim = &generic_dog_move_walkl_from_walk;
					}
					else if (nRand == 1 && VerifySequence(monster, &generic_dog_move_walkr_from_walk, reject_actionflags))
					{
						// start turning to look right
						returnAnim = &generic_dog_move_walkr_from_walk;
					}
					else if (nRand == 2 && VerifySequence(monster, &generic_dog_move_walkl_walkr, reject_actionflags))
					{
						// start swiveling left and right
						returnAnim = &generic_dog_move_walkl_walkr;
					}
				}
				else if (currentmove == &generic_dog_move_walkl)
				{
					// we were looking left, so start turning to look straight
					if (VerifySequence(monster, &generic_dog_move_walk_from_walkl, reject_actionflags))
					{
						returnAnim = &generic_dog_move_walk_from_walkl;
					}
				}
				else if (currentmove == &generic_dog_move_walkr)
				{
					// we were looking right, so start turning to look straight
					if (VerifySequence(monster, &generic_dog_move_walk_from_walkr, reject_actionflags))
					{
						returnAnim = &generic_dog_move_walk_from_walkr;
					}
				}
				// if we were turning from left to straight or right to straight, now look straight
				else if (currentmove == &generic_dog_move_walk_from_walkl ||
					currentmove == &generic_dog_move_walk_from_walkr)
				{
					if (VerifySequence(monster, &generic_dog_move_walk, reject_actionflags))
					{
						returnAnim = &generic_dog_move_walk;
					}
				}
				else if (currentmove == &generic_dog_move_walkr_from_walk)
				{
					// we were turning from straight to right, so now look right
					if (VerifySequence(monster, &generic_dog_move_walkr, reject_actionflags))
					{
						returnAnim = &generic_dog_move_walkr;
					}
				}
				else if (currentmove == &generic_dog_move_walkl_from_walk)
				{
					// we were turning from straight to left, so now look right
					if (VerifySequence(monster, &generic_dog_move_walkl, reject_actionflags))
					{
						returnAnim = &generic_dog_move_walkl;
					}
				}
				else
				{
					// just have the dumb thing do its regular walk
					returnAnim = &generic_dog_move_walk;
				}
			}
			else
			{
				// based on a random number we decided not to switch animations this frame.
				//our walking anims can only be switched at the end of the anim, though, so
				//reset our currentMoveFinished boolean so that we won't try to switch until
				//we hit the current anim's EOS again
				currentMoveFinished = 0;
			}
		}
	}
	else
	{
		// we have a target, so sniff our way towards it.

		// if our current anim is NULL, or isn't sniff-related, or it's a non-looping sniff-related anim, or
		//it's a looping sniff-related anim and we feel like it, change to a new anim
		if (currentmove == NULL ||
			(currentmove != &generic_dog_move_sniff &&
			currentmove != &generic_dog_move_walk_from_sniff &&
			currentmove != &generic_dog_move_walk &&
			currentmove != &generic_dog_move_sniff_from_walk) ||
			!(currentmove->actionFlags & ACTFLAG_LOOPANIM) ||
			!(rand()%4))
		{
			if (currentmove == &generic_dog_move_sniff && 
				VerifySequence(monster, &generic_dog_move_walk_from_sniff, reject_actionflags))
			{
				returnAnim = &generic_dog_move_walk_from_sniff;
			}
			else if (currentmove == &generic_dog_move_walk_from_sniff && 
				VerifySequence(monster, &generic_dog_move_walk, reject_actionflags))
			{
				returnAnim = &generic_dog_move_walk;
			}
			else if (currentmove == &generic_dog_move_walk && 
				VerifySequence(monster, &generic_dog_move_sniff_from_walk, reject_actionflags))
			{
				returnAnim = &generic_dog_move_sniff_from_walk;
			}
			else if (currentmove == &generic_dog_move_sniff_from_walk && 
				VerifySequence(monster, &generic_dog_move_sniff, reject_actionflags))
			{
				returnAnim = &generic_dog_move_sniff;
			}
			else // our last anim was something non-sniff-related. just walk for now
			{
				returnAnim = &generic_dog_move_walk;
			}
		}
		else
		{
			// need to clear this so we'll wait for the entire current anim to finish
			//before switching to a new one
			currentMoveFinished = 0;
		}
	}
*/
	return returnAnim;
}
#endif // #if 0
mmove_t	*bodycow_c::GetSequenceForStand(edict_t &monster, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);

	if (currentmove && (currentmove->suggested_action==ACTCODE_STAND)&&(!IsAnimationFinished()))
	{
		return currentmove;
	}

//	if (currentmove == &generic_cow_move_idle)
	{
		if (!(rand()%60))
		{
			if (VerifySequence(monster, &generic_cow_move_moo, reject_actionflags))
			{	// moo
				PlayMooSound(monster);
				return &generic_cow_move_moo;
			}
		}
		else if (!(rand()%60))
		{
			if (VerifySequence(monster, &generic_cow_move_looking, reject_actionflags))
			{	// look around once in a while
				return &generic_cow_move_looking;
			}
		}
	}
	return &generic_cow_move_idle;
}

mmove_t	*bodycow_c::GetSequenceForAttack(edict_t &monster, vec3_t dest, vec3_t face, edict_t *target, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	fLastAttackTime = level.time;

	return NULL;
}

mmove_t *bodycow_c::GetSequenceForPain(edict_t &monster, vec3_t point, float kick, int damage, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	if (VerifySequence(monster, &generic_cow_move_pain, reject_actionflags))
	{	// Ow! Hey, stop that!
		return &generic_cow_move_pain;
	}
	return currentmove;
}

mmove_t	*bodycow_c::GetSequenceForDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);
	mmove_t		*animation = &generic_dog_move_walk;

	// don't interrupt our first dying animation
	if ( (currentmove->suggested_action==ACTCODE_DEATH) && 
		!(currentmove->actionFlags&ACTFLAG_ALWAYSPRONE) &&
		!currentMoveFinished)
	{
		return currentmove;
	}

	return &generic_cow_move_die_right;

/*
	//are we already in a death animation?
	if (currentmove && currentmove->suggested_action==ACTCODE_DEATH)
	{
		float	lastHit = level.time-LastKilledTime;	
		
		//if we're twitching on our backs, see if we should continue
		if ((currentmove==&generic_dog_move_death_lbshotloop2) && (lastHit > 0.3))
		{
			if (IsAvailableSequence(monster, &generic_dog_move_death_lbshotdeath))
			{
				FinalAnim = true;
				FinalSound = true;
				animation = &generic_dog_move_death_lbshotdeath;
			}
		}
		//if we're in a prone anim, or at the end of one that ends up prone, and we're shot, call the corpse juggle thingy
		else if (((currentmove->actionFlags & ACTFLAG_ALWAYSPRONE)||(currentMoveFinished)) && (lastHit <= .1)) //  corpse was just hit
		{
			if ((dflags & DT_JUGGLE) && IsAvailableSequence(monster, &generic_dog_move_death_lbshotloop2))
			{
				FinalAnim = true;
				FinalSound = true;
				// check for laying on left side vs. right side
				if ( (currentmove == &generic_dog_move_deathside) ||
					 (currentmove == &generic_dog_move_death_lbshotdeath) ||
					 (currentmove == &generic_dog_move_death_lbshotloop2) )
				{
					animation = &generic_dog_move_death_lbshotloop2;
				}
				else
				{
					animation = &generic_dog_move_death_lbshotloop2;
				}
			}
			else if (IsAvailableSequence(monster, &generic_dog_move_death_lbshotdeath) && currentmove!=&generic_dog_move_death_lbshotloop)
			{
				FinalAnim = true;
				FinalSound = true;
				ForceNextAnim = true;
				// ideally, there'd be a check here similar to the one above, cuz the anim
				//we play here should be determined by how we died. sadly, we don't
				//currently have a shot-after-dead anim to be played if we died running
				animation = &generic_dog_move_death_lbshotdeath;
			}
			else
			{
				animation = currentmove;
			}
		}
		else
		{
			animation = currentmove;
		}

	}
	else
	{
		// first check if we've taken a _lot_ of damage (c4, etc.)
		if (damage > 90)
		{
			// blown backward from a lot of damage
			vec3_t	vDmg;
			animation = &generic_dog_move_death_back;
			// make sure we're blown away from whatever did the damage
			if (inflictor)
			{
				VectorSubtract(monster.s.origin, inflictor->s.origin, vDmg);
				vDmg[2] = 0;
				VectorNegate(vDmg, vDmg);
				vectoangles(vDmg, monster.s.angles);
			}
		}
		else if (currentmove == &generic_dog_move_run && IsAvailableSequence(monster, &generic_dog_move_deathrun))
		{
			animation = &generic_dog_move_deathrun;
		}
		else if (IsAvailableSequence(monster, &generic_dog_move_deathside))
		{
			animation = &generic_dog_move_deathside;
		}
	}

	return animation;
*/
}

void bodycow_c::RecognizeGoreZones(edict_t &monster)
{
	RecognizeCowGoreZones(monster);
}

/* cow gore zones

	GZ_COW_HEAD,
	GZ_COW_CHEST,
	GZ_COW_R_FRONT,
	GZ_COW_L_FRONT,
	GZ_COW_R_BACK,
	GZ_COW_L_BACK,
	NUM_COWGOREZONES
*/
void bodycow_c::RecognizeCowGoreZones(edict_t &monster)
{
	GhoulID tempHole = 0;
	int i;

	InitializeGoreZoneLevels(monster);

	for (i=0;i<numGoreZones;i++)
	{
		gzones[i].damage=gzones[i].damage_level=0;
		gzones[i].id=gzones[i].parent_id=gzones[i].cap=gzones[i].blownCap=NULL_GhoulID;
		gzones[i].blownCapBolt=gzones[i].capbolt=gzones[i].bolt=gzones[i].capPieceBolt=0;
		gzones[i].capPiece=NULL;
		gzones[i].blowme=&cow_blown_parts[GBLOWN_COW_NUM];
		gzones[i].childzone=numGoreZones;
	}

	// gore zones and parent_id's (parent_id's are derived from mesh names)
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_body_1");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_COW_HEAD].parent_id=tempHole;
		SetGoreZone(monster, "GZ_HEAD", GZ_COW_HEAD);
		gzones[GZ_COW_CHEST].parent_id=tempHole;
		SetGoreZone(monster, "GZ_CHEST", GZ_COW_CHEST);
		gzones[GZ_COW_R_FRONT].parent_id=tempHole;
		SetGoreZone(monster, "GZ_FRONT_R", GZ_COW_R_FRONT);
		gzones[GZ_COW_L_FRONT].parent_id=tempHole;
		SetGoreZone(monster, "GZ_FRONT_L", GZ_COW_L_FRONT);
		gzones[GZ_COW_R_BACK].parent_id=tempHole;
		SetGoreZone(monster, "GZ_BACK_R", GZ_COW_R_BACK);
		gzones[GZ_COW_L_BACK].parent_id=tempHole;
		SetGoreZone(monster, "GZ_BACK_L", GZ_COW_L_BACK);
	}
/*	

	// blown caps are the chunks of meat on the blown off part that get turned on at the 
	//point of separation
	SetGoreBlownCap(monster, "_CAP_HUSKY_RLEG_RSHOLDR", GZ_DOG_RLEG_FRONT);
	SetGoreBlownCap(monster, "_CAP_HUSKY__LLEG_LSHOLDR", GZ_DOG_LLEG_FRONT);
	SetGoreBlownCap(monster, "_CAP_HUSKY_RHIP_BODY", GZ_DOG_RHIP);
	SetGoreBlownCap(monster, "_CAP_HUSKY_LLEG_LHIP", GZ_DOG_LLEG_BACK);

	// blown cap bolts are the points at which the separation occurs when a part is
	//blown off (used as the origin for various blood-related effects). they point from
	//the separated part toward the parent (e.g. from the leg to the hip)
	//
	//the dog doesn't have any.
	//

	// caps are the chunks of meat on the parent part that get turned on at the
	//point of separation
	SetGoreCap(monster, "_CAP_HUSKY_TORSO_RSHOLDR", GZ_DOG_RLEG_FRONT);
	SetGoreCap(monster, "_CAP_HUSKY_TORSO_LSHOLDR", GZ_DOG_LLEG_FRONT);
	SetGoreCap(monster, "_CAP_HUSKY_TORSO_RHIP", GZ_DOG_RHIP);
	SetGoreCap(monster, "_CAP_HUSKY_LHIP_LLEG", GZ_DOG_LLEG_BACK);

	// cap bolts are the points at which the separation occurs when a part is 
	//blown off (used as the origin for various blood-related effects). they point from
	//the parent toward the separated part (e.g. from the hip to the leg)
	SetGoreCapBolt(monster, "GBOLT_CAP_HUSKY_TORSO_RSHOLDR", GZ_DOG_RLEG_FRONT);
	SetGoreCapBolt(monster, "GBOLT_CAP_HUSKY_TORSO_LSHOLDR", GZ_DOG_LLEG_FRONT);
	SetGoreCapBolt(monster, "GBOLT_CAP_HUSKY_TORSO_RHIP", GZ_DOG_RHIP);
	SetGoreCapBolt(monster, "GBOLT_CAP_HUSKY_LHIP_LLEG", GZ_DOG_LLEG_BACK);
*/
	// bolts are used for hit location detection (which bolt is the dmg nearest?) and
	//placing exit wounds (probably not for the dog, though)
	SetGoreBolt(monster, "gbolt_head", GZ_COW_HEAD);
	SetGoreBolt(monster, "gbolt_chest", GZ_COW_CHEST);
	SetGoreBolt(monster, "gbolt_front_r", GZ_COW_R_FRONT);
	SetGoreBolt(monster, "gbolt_front_l", GZ_COW_L_FRONT);
	SetGoreBolt(monster, "gbolt_back_r", GZ_COW_R_BACK);
	SetGoreBolt(monster, "gbolt_back_l", GZ_COW_L_BACK);
/*
	// blowme's are the parts that'll get blown off when an area is hit. the possible
	//values are in the first column of dogblown_parts[], which are entries in dogblown_index.
	gzones[GZ_DOG_LLEG_FRONT].blowme = &husky_blown_parts[GBLOWN_HUSKY_LLEG_FRONT];
	gzones[GZ_DOG_RLEG_FRONT].blowme = &husky_blown_parts[GBLOWN_HUSKY_RLEG_FRONT];
	gzones[GZ_DOG_LLEG_BACK].blowme = &husky_blown_parts[GBLOWN_HUSKY_LLEG_BACK];
	gzones[GZ_DOG_RHIP].blowme = &husky_blown_parts[GBLOWN_HUSKY_RHIP];
	gzones[GZ_DOG_RLEG_BACK].blowme = &husky_blown_parts[GBLOWN_HUSKY_RHIP];

	// childzone's are used for turning off decals and maybe caps when subparts are 
	//blown off of parts (e.g. lower arm blown off then upper arm blown off).
	gzones[GZ_DOG_RHIP].childzone = GZ_DOG_RLEG_BACK;
	gzones[GZ_DOG_RLEG_BACK].childzone = GZ_DOG_RHIP;

	// a little weirdness here to make the right hip and right leg react the same
	//when shot (they're both s'posed to fly off, attached to each other, when shot)
	SetGoreBlownCap(monster, "_CAP_HUSKY_RHIP_BODY", GZ_DOG_RLEG_BACK);
	SetGoreCap(monster, "_CAP_HUSKY_TORSO_RHIP", GZ_DOG_RLEG_BACK);
	SetGoreCapBolt(monster, "GBOLT_CAP_HUSKY_TORSO_RHIP", GZ_DOG_RLEG_BACK);
*/
}


int GetDamageLevel(int amount);
/*{
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
*/
int bodycow_c::ShowDamage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb)
{
	vec3_t		bodyloc;//, footorigin;
	gz_info		*frontHole=NULL;
	gz_code		frontCode;
	gz_info		*backHole=NULL;
	gz_code		backCode;
	vec3_t		forward, right, up;
	vec3_t		to_impact;
	float		forward_back, right_left, up_down;
	ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);
	ggOinstC	*blownInst;
	int			i,j;
	int			take;
	float		curGoreDist,bestGoreDist,tdist;
	Vect3		EntryPos;
	vec3_t		ExitPos,tvec;
	Vect3		zonePos;
	Vect3		Direction;
	Matrix4		ZoneMatrix;
	
	Matrix4 ToEnt,ToWorld;


	for (i=0;i<3;i++)
	{
		Direction[i]=dir[i];
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
	
	// Only show damage from projectile weapons and melee weapons, else return.

	if(!(dflags&DT_PROJECTILE|DT_MELEE))
	{
		return(damage);
	}

	take = damage;

	//NO HITS!!!

	if ( (dflags&DT_PROJECTILE) && (bullet_numHits == 0) )
	{
		return 0;
	}

	PlayPainSound(monster);

	if (ai_goretest->value)
	{
		gi.dprintf("%d hits!\n", bullet_numHits);
	}

	//get entrance wound
	for (i=0;i<bullet_numHits;i++)
	{
		//hit my instance
		if (bullet_Hits[i].Inst==myInstance->GetInstPtr())
		{
			//hit something
			if (bullet_Hits[i].Mesh)
			{
				VectorScale(*(vec3_t*)(&bullet_EntDir),bullet_Hits[i].Distance,*(vec3_t*)(&EntryPos));
				EntryPos += bullet_EntStart;
				if (ai_goretest->value)
				{
					gi.dprintf("Entry wound position: %2.2f %2.2f %2.2f\n",EntryPos[0],EntryPos[1],EntryPos[2]);
				}
				tdist=bestGoreDist=9999999;
				for (j=0;j<numGoreZones;j++)
				{
					//hit the part that this zone is attached to--should still check to make sure shot is entry, and find the best zone on this part.
					if (bullet_Hits[i].Mesh==gzones[j].parent_id)
					{
						if (ai_goretest->value)
						{
							gi.dprintf("considering %s for frontHole\n",gzcownames[j]);
						}
						if (!gzones[j].bolt)
						{
							if (!frontHole || tdist>bullet_Hits[i].Distance)
							{
								tdist=bullet_Hits[i].Distance;
								frontHole=&gzones[j];
								frontCode = (gz_cowcode)j;
								if (ai_goretest->value)
								{
									gi.dprintf("Setting frontHole to %s\n",gzcownames[j]);
								}
							}
						}
						else
						{
							//is this a better match than what's already in fronthole?
							bullet_Hits[i].Inst->GetBoltMatrix(level.time,ZoneMatrix,gzones[j].bolt,IGhoulInst::MatrixType::Entity);
							ZoneMatrix.GetRow(3,zonePos);
							if (ai_goretest->value)
							{
								gi.dprintf("Candidate position: %2.2f %2.2f %2.2f\n",zonePos[0],zonePos[1],zonePos[2]);
							}
							VectorSubtract(*(vec3_t*)(&EntryPos),*(vec3_t*)(&zonePos),tvec);
							curGoreDist = VectorLengthSquared(tvec);
							if (!frontHole || curGoreDist<bestGoreDist)
							{
								tdist=bullet_Hits[i].Distance;
								frontHole=&gzones[j];
								frontCode = (gz_cowcode)j;
								bestGoreDist = curGoreDist;
								if (ai_goretest->value)
								{
									gi.dprintf("Setting frontHole to %s\n",gzcownames[j]);
								}
							}
						}
					}
				}
				if (frontHole)
				{
					int k;
					VectorScale(dir,tdist,*(vec3_t*)(&EntryPos));
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
					if (ai_goretest->value)
					{
						gi.dprintf("spraying blood from %2.2f %2.2f %2.2f; monster pos %2.2f %2.2f %2.2f\n",EntryPos[0],EntryPos[1],EntryPos[2],monster.s.origin[0],monster.s.origin[1],monster.s.origin[2]);
					}
					FX_MakeBloodSpray(ExitPos, dir, 200, 1, &monster);
					
					break;
				}
			}
		}
		//hit a bolt-on
		else if (!frontHole)
		{
			ggBinstC	*boltPiece;
			boltPiece = myInstance->FindBoltInstance(bullet_Hits[i].Inst);
			if (boltPiece && boltPiece->GetBolterBolt())
			{
				int k;
				VectorScale(dir,tdist,*(vec3_t*)(&EntryPos));
				for (k=0;k<3;k++)
				{
					EntryPos[k]+=point[k];
				}
				take = DropBoltOn(monster, boltPiece, EntryPos, Direction, take);
				if (take <= 0)
				{
					return 0;
				}
			}
		}
	}


	//get exit wound
	for (i=bullet_numHits-1;i>=0;i--)
	{
		//hit something
		if (bullet_Hits[i].Mesh)
		{
			//hit my instance
			if (bullet_Hits[i].Inst==myInstance->GetInstPtr())
			{
				VectorScale(*(vec3_t*)(&bullet_EntDir),bullet_Hits[i].Distance,ExitPos);
				Vec3AddAssign(*(vec3_t*)(&bullet_EntStart), ExitPos);
				if (ai_goretest->value)
				{
					gi.dprintf("Exit wound position: %2.2f %2.2f %2.2f\n",ExitPos[0],ExitPos[1],ExitPos[2]);
				}
				tdist=bestGoreDist=9999999;
				for (j=0;j<numGoreZones;j++)
				{
					//hit the part that this zone is attached to--should still check to make sure shot is entry, and find the best zone on this part.
					if (bullet_Hits[i].Mesh==gzones[j].parent_id)
					{
						if (ai_goretest->value)
						{
							gi.dprintf("considering %s for backHole\n",gzcownames[j]);
						}
						if (!gzones[j].bolt)
						{
							if (!backHole || tdist>bullet_Hits[i].Distance)
							{
								tdist=bullet_Hits[i].Distance;
								backHole=&gzones[j];
								backCode = (gz_cowcode)j;
								if (ai_goretest->value)
								{
									gi.dprintf("Setting backHole to %s\n",gzcownames[j]);
								}
							}
						}
						else
						{
							//is this a better match than what's already in fronthole?
							bullet_Hits[i].Inst->GetBoltMatrix(level.time,ZoneMatrix,gzones[j].bolt,IGhoulInst::MatrixType::Entity);
							ZoneMatrix.GetRow(3,zonePos);
							if (ai_goretest->value)
							{
								gi.dprintf("Zone's bolt position: %2.2f %2.2f %2.2f\n",zonePos[0],zonePos[1],zonePos[2]);
							}
							VectorSubtract(ExitPos,*(vec3_t*)(&zonePos),tvec);
							curGoreDist = VectorLengthSquared(tvec);
							if (!backHole || curGoreDist<bestGoreDist)
							{
								tdist=bullet_Hits[i].Distance;
								backHole=&gzones[j];
								backCode = (gz_cowcode)j;
								bestGoreDist = curGoreDist;
								if (ai_goretest->value)
								{
									gi.dprintf("Setting backHole to %s\n",gzcownames[j]);
								}
							}
						}
					}
				}
				if (backHole)
				{
					VectorScale(dir,tdist,ExitPos);
					VectorAdd(point,ExitPos,ExitPos);
					break;
				}
				else
				{
					EntryPos=EntryPos;//debug breakpoint--removme!
				}
			}
			//hit a bolt-on: do something!
			else
			{
				EntryPos=EntryPos;//debug breakpoint--removme!
			}
		}
	}


	AngleVectors(monster.s.angles, forward, right, up);
	VectorSubtract(point, monster.s.origin, to_impact);
	VectorNormalize(to_impact);
	forward_back = DotProduct(forward, to_impact);
	right_left = DotProduct(right, to_impact);
	up_down = DotProduct(up, to_impact);

	Emote(monster, EMOTION_PAIN, 10.0);

	VectorSubtract(point, monster.s.origin, bodyloc);

	//dying--drop weapons
	if (false && monster.health <= take)
	{
		ggBinstC	*bolted;
		vec3_t		handPos;
		VectorSet(handPos,0,0,0);
		monster.ghoulInst->GetBoltMatrix(level.time,ZoneMatrix,monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"),IGhoulInst::MatrixType::Entity);
		ZoneMatrix.GetRow(3,zonePos);
		handPos[0]=zonePos[0];
		handPos[1]=zonePos[1];
		handPos[2]=zonePos[2];
		while (myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l")))
		{
			bolted=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"));
			DropBoltOn(monster, bolted, zonePos, Direction, take);
		}
		monster.ghoulInst->GetBoltMatrix(level.time,ZoneMatrix,monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r"),IGhoulInst::MatrixType::Entity);
		ZoneMatrix.GetRow(3,zonePos);
		handPos[0]=zonePos[0];
		handPos[1]=zonePos[1];
		handPos[2]=zonePos[2];
		while (myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r")))
		{
			bolted=myInstance->GetBoltInstance(monster.ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r"));
			DropBoltOn(monster, bolted, zonePos, Direction, take);
		}
	}


	if (frontHole&& frontHole->parent_id && myInstance->GetInstPtr()->GetPartOnOff(frontHole->parent_id))
	{
		int newlevel;
		frontHole->damage+=take;
//		newlevel=1;
		newlevel = GetDamageLevel(frontHole->damage);
		if ((frontHole->id) && (!lock_gorezones))
		{
			monster.ghoulInst->SetPartOnOff(frontHole->id, true);
		}
		if (monster.health <= damage && frontHole->cap && (ai_goretest->value || (dflags & DT_SEVER)) && (!lock_sever))
		{
			edict_t *danglyBit;
			Emote(monster, EMOTION_DEAD, 1.0);
			UpdateFace(monster);
			danglyBit=MakeBlownPart(monster, frontCode, dir, &blownInst);
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
			BlowPart(monster,frontHole->blowme,danglyBit);
			BlowZone(monster,frontHole->childzone,frontHole->childzone,danglyBit);
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
			myInstance->GetInstPtr()->SetPartOnOff(frontHole->cap,true);
			myInstance->GetInstPtr()->SetFrameOverride("c","c_blood",frontHole->cap);
			if (frontHole->capbolt)
			{
				if (frontHole->capPiece)
				{
					if (frontHole->capPieceBolt)
					{
						AddBoltedItem(monster, frontHole->capbolt, "Enemy/bolt", frontHole->capPiece->GetSubName(), frontHole->capPieceBolt, myInstance);
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
				if (monster.health>0)
				{
//					gi.sound (&monster, CHAN_BODY, gi.soundindex ("impact/gore/spout.wav"), .8, ATTN_NORM, 0);
					FX_Bleed(&monster, frontHole->capbolt, 50);
				}
				else
				{
					FX_BloodCloud(&monster, frontHole->capbolt, 100);
				}
			}
			if (spout == -1)
			{
				if (frontHole->capbolt)
				{
					monster.ghoulInst->AddMatrixCallBack(&TheCowSetVectorCallback,frontHole->capbolt,IGhoulInst::MatrixType::Entity);
					spout = frontHole - gzones;
				}
				else if (frontHole->bolt)
				{
					monster.ghoulInst->AddMatrixCallBack(&TheCowSetVectorCallback,frontHole->bolt,IGhoulInst::MatrixType::Entity);
					spout = frontHole - gzones;
				}
			}
		}
		else if (newlevel>frontHole->damage_level)
		{
			int i;
			
			//knock off loose parts
			ShakePart(monster, frontHole->blowme, Direction);

			if ((frontHole->id) && (!lock_gorezones))
			{
				i=rand()%3;

				//if this zone was already active, match up the row
				if (frontHole->damage_level) 
				{
					GhoulID	skinID=myInstance->GetInstPtr()->GetFrameOverride("gz",frontHole->id);
					if (skinID)
					{
						for (i=0;i<4;i++)
						{
							if (skinID==gzone_levels[frontHole->damage_level-1][i])
								break;
						}
					}
				}

				if (i<4 && gzone_levels[newlevel-1][i] != NULL_GhoulID)
				{
					myInstance->GetInstPtr()->SetFrameOverride("gz",gzone_levels[newlevel-1][i],frontHole->id);
				}
			}
			if (frontHole->bolt)
			{
				FX_Bleed(&monster, frontHole->bolt, 25);
				if ((frontHole==&gzones[GZ_GUT_FRONT]) && (!lock_gorezones))
//				if (newlevel==3)
				{
					myInstance->RemoveBolt(frontHole->bolt);
					AddBoltedItem(monster, frontHole->bolt, "Enemy/bolt", "g_guts", "to_gbolt_gut_f", myInstance);
				}
			}
			if (!lock_gorezones)
			{
				frontHole->damage_level=newlevel;
			}
		}
	}
	if (backHole && backHole->id && backHole->parent_id && backHole != frontHole && myInstance->GetInstPtr()->GetPartOnOff(backHole->parent_id))
	{
		int i=rand()%3;
		//exit wound--massive damage
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

			if (i<4)
			{
				myInstance->GetInstPtr()->SetFrameOverride("gz",gzone_levels[2][i],backHole->id);
			}
			backHole->damage_level=3;
		}
		if (!lock_gorezones)
		{
			monster.ghoulInst->SetPartOnOff(backHole->id, true);
		}
	}
	return take;
}

void bodycow_c::SetRootBolt(edict_t &monster)
{
	GhoulID	tempID;
	GhoulID	tempBolt;
	GhoulID	tempNote;

	// the death animations look a little weird if the cow obtains a knockback-induced velocity
	monster.flags &= ~FL_NO_KNOCKBACK;

	AllocateGoreZones(monster);
	RecognizeGoreZones(monster);
	tempBolt=monster.ghoulInst->GetGhoulObject()->FindPart("quake_origin");
	if (tempBolt)
	{
		monster.ghoulInst->AddMatrixCallBack(&TheCowFollowGenericPivot,tempBolt,IGhoulInst::MatrixType::JacobianEntity);
	}
	else
	{
		gi.dprintf("WARNING: monster origin not found--wacky!!\n");
	}

	tempID=monster.ghoulInst->GetGhoulObject()->FindPart("quake_ground");
	if (tempID)
	{
		monster.ghoulInst->AddMatrixCallBack(&TheCowGroundCallback,tempID,IGhoulInst::MatrixType::Entity);
	}
	tempBolt=monster.ghoulInst->GetGhoulObject()->FindPart("GBOLT_HEAD");
	if (tempBolt)
	{
		monster.ghoulInst->AddMatrixCallBack(&TheCowMouthCallback,tempBolt,IGhoulInst::MatrixType::Entity);
	}
	else
	{
		gi.dprintf("WARNING: monster mouth not found--sight will be defective!!\n");
	}


	tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("BOS");
	if (tempNote)
	{
		monster.ghoulInst->AddNoteCallBack(&TheCowMonsterSeqBeginCallback,tempNote);
	}

	tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("EOS");
	if (tempNote)
	{
		monster.ghoulInst->AddNoteCallBack(&TheCowMonsterSeqEndCallback,tempNote);
	}

	tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("fire");
	if (tempNote)
	{
		monster.ghoulInst->AddNoteCallBack(&TheCowFireCallBack,tempNote);
	}
}

attacks_e bodycow_c::GetBestWeapon(edict_t &monster)
{
	return ATK_DOG_ATTACK;
}

void bodycow_c::BiteMe(edict_t &monster, qboolean forceFireRate)
{
	Matrix4 boltToCow, cowToWorld, final;
	vec3_t firePoint = {0,0,0}, forward, rotateangs;
	IGhoulInst *instCow = monster.ghoulInst;
	IGhoulObj		*obj = NULL;
	GhoulID boltID = 0;
	if (instCow)
	{
		if(obj = instCow->GetGhoulObject())
		{
			boltID = obj->FindPart("GBOLT_CHEST");
		}
	}
	if (boltID)
	{
		ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);
		if (the_ai)
		{
			instCow->GetBoltMatrix(level.time, boltToCow, boltID,IGhoulInst::MatrixType::Entity);
			VectorDegreesToRadians(monster.s.angles,rotateangs);
			cowToWorld.Identity();
			cowToWorld.Rotate(rotateangs[2],rotateangs[0],-rotateangs[1]);//grr.
			final.Concat(boltToCow,cowToWorld);
			final.GetRow(3,(Vect3)firePoint);
			VectorAdd(monster.s.origin,firePoint,firePoint);

			the_ai->GetAimVector(forward);


//			weapons.attack(ATK_DOG_ATTACK, &monster, firePoint, forward, DogAttackSuccess);

			rLastFireTime = level.time;
		}
	}
}

void bodycow_c::NextMovement(edict_t &monster, vec3_t curDist, float scale)
{
	vec3_t forward,right,up,tempdist,distaccum;
	int i;
/*
	if (currentmove==&generic_dog_move_attack && (level.time - fLastAttackTime) < 2*FRAMETIME && 
		!bTurnedAfterAttack)
	{
		// turn the dog 45degrees to one side after he bites
		monster.s.angles[YAW] -= 60;
		if (monster.s.angles[YAW] < 0)
		{
			monster.s.angles[YAW] += 360;
		}
		bTurnedAfterAttack = true;
	}
	*/
	AngleVectors(monster.s.angles,forward,right,up);
	VectorScale(monster.intend_velocity, FRAMETIME, curDist);

//	VectorCopy(monster.intend_velocity, curDist);
	for (i = 0; i < 3; i++)
	{
		if (curDist[i]>MAX_VEL_COMPONENT)
		{
//			gi.dprintf("warning! capping monster %d vel component to %f (from %f)!\n", i, MAX_VEL_COMPONENT, curDist[i]);
			curDist[i]=MAX_VEL_COMPONENT;
		}
		if (curDist[i]<-MAX_VEL_COMPONENT)
		{
//			gi.dprintf("warning! capping monster %d vel component to -%f (from %f)!\n", i, MAX_VEL_COMPONENT, curDist[i]);
			curDist[i]=-MAX_VEL_COMPONENT;
		}
	}

	//plop in default vel, too--fixme, shouldn't be here
	VectorScale(forward, curDist[0]*scale, tempdist);
	VectorCopy(tempdist,distaccum);
	VectorScale(right, curDist[1]*scale, tempdist);
	VectorAdd(distaccum,tempdist,distaccum);
	VectorScale(up, curDist[2]*scale, tempdist);
	VectorAdd(distaccum,tempdist,curDist);
}

void bodycow_c::FinishMove(edict_t &monster)
{
	bodyorganic_c::FinishMove(monster);
/*	if (currentmove==&generic_dog_move_attack)
	{
		bTurnedAfterAttack = false;
	}
*/
}

qboolean bodycow_c::PlayAnimation(edict_t &monster, mmove_t *newanim, bool forceRestart)
{
	forceRestart |= ForceNextAnim;
	ForceNextAnim = false;

	return bodyorganic_c::PlayAnimation(monster, newanim, forceRestart);
}

bodycow_c::bodycow_c(bodycow_c *orig)
: bodyorganic_c(orig)
{
	bTurnedAfterAttack = orig->bTurnedAfterAttack;
	FinalAnim = orig->FinalAnim;
	FinalSound = orig->FinalSound;
	nNextShotKillsMe	= orig->nNextShotKillsMe;

	fJumpDist = orig->fJumpDist;
	fJumpSpeed = orig->fJumpSpeed;
	fLastAttackTime = orig->fLastAttackTime;
	fLastHeadTurnTime = orig->fLastHeadTurnTime;
	m_fLastMooSoundTime = orig->m_fLastMooSoundTime;
	m_fLastPainSoundTime = orig->m_fLastPainSoundTime;
	m_fLastDeathSoundTime = orig->m_fLastDeathSoundTime;
}

void bodycow_c::Evaluate(bodycow_c *orig)
{
	bTurnedAfterAttack = orig->bTurnedAfterAttack;
	FinalAnim = orig->FinalAnim;
	FinalSound = orig->FinalSound;
	nNextShotKillsMe	= orig->nNextShotKillsMe;

	fJumpDist = orig->fJumpDist;
	fJumpSpeed = orig->fJumpSpeed;
	fLastAttackTime = orig->fLastAttackTime;
	fLastHeadTurnTime = orig->fLastHeadTurnTime;
	m_fLastMooSoundTime = orig->m_fLastMooSoundTime;
	m_fLastPainSoundTime = orig->m_fLastPainSoundTime;
	m_fLastDeathSoundTime = orig->m_fLastDeathSoundTime;

	bodyorganic_c::Evaluate(orig);
}

void bodycow_c::AllocateGoreZones(edict_t &monster)
{
	//eliminate old gore zones...
	bodyorganic_c::AllocateGoreZones(monster);
	numGoreZones=NUM_COWGOREZONES;
	gzones=new gz_info[numGoreZones];
}

void bodycow_c::Read()
{
	char	loaded[sizeof(bodycow_c)];

	gi.ReadFromSavegame('AIBC', loaded, sizeof(bodycow_c));
	Evaluate((bodycow_c *)loaded);
}

void bodycow_c::Write()
{
	bodycow_c	*savable;

	savable = new bodycow_c(this);
	gi.AppendToSavegame('AIBC', savable, sizeof(*this));
	delete savable;

	WriteGZ();
}

#endif