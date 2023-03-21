#include "g_local.h"
#include "matrix4.h"
#include "ai_private.h"
#include "callback.h"


gz_blown_part husky_blown_parts[GBLOWN_HUSKY_NUM+1] =
{
	//
	// husky
	//

	// front left leg gets blown off by itself
	GBLOWN_HUSKY_LLEG_FRONT,GBLOWN_HUSKY_NUM,				GBLOWNAREA_LFOREARM,GBLOWNCODE_PART,	"_HUSKY_LLEG_FRONT",

	// front right leg gets blown off by itself
	GBLOWN_HUSKY_RLEG_FRONT,GBLOWN_HUSKY_NUM,				GBLOWNAREA_RFOREARM,GBLOWNCODE_PART,	"_HUSKY_RLEG_FRONT",

	// back left leg gets blown off by itself
	GBLOWN_HUSKY_LLEG_BACK,	GBLOWN_HUSKY_NUM,				GBLOWNAREA_LCALF,	GBLOWNCODE_PART,	"_HUSKY_LLEG_BACK",

	// back right leg gets blown off at the hip
	GBLOWN_HUSKY_RHIP,		GBLOWN_HUSKY_RLEG_BACK,			GBLOWNAREA_RTHIGH,	GBLOWNCODE_PART,	"_HUSKY_RHIP",
	GBLOWN_HUSKY_RLEG_BACK,	GBLOWN_HUSKY_NUM,				GBLOWNAREA_RCALF,	GBLOWNCODE_PART,	"_HUSKY_RLEG_BACK",
	GBLOWN_HUSKY_NUM,		GBLOWN_HUSKY_NUM,				GBLOWNAREA_FINISH,	GBLOWNCODE_SPECIAL,	"",
};
	//
	// rottweiler
	//
gz_blown_part rott_blown_parts[GBLOWN_ROTT_NUM+1] =
{

	// front left leg gets blown off by itself
	GBLOWN_ROTT_LLEG_FRONT,	GBLOWN_ROTT_NUM,				GBLOWNAREA_LFOREARM,GBLOWNCODE_PART,	"_ROT_LLEG_FRONT",

	// front right leg gets blown off by itself
	GBLOWN_ROTT_RLEG_FRONT,	GBLOWN_ROTT_NUM,				GBLOWNAREA_RFOREARM,GBLOWNCODE_PART,	"_ROT_RLEG_FRONT",

	// back left leg gets blown off by itself
	GBLOWN_ROTT_LLEG_BACK,	GBLOWN_ROTT_NUM,				GBLOWNAREA_LCALF,	GBLOWNCODE_PART,	"_ROT_LLEG_BACK",

	// back right leg gets blown off at the hip
	GBLOWN_ROTT_RHIP,		GBLOWN_ROTT_RLEG_BACK,			GBLOWNAREA_RTHIGH,	GBLOWNCODE_PART,	"_ROT_RLEG_BACK",
	GBLOWN_ROTT_RLEG_BACK,	GBLOWN_ROTT_NUM,				GBLOWNAREA_RCALF,	GBLOWNCODE_PART,	"_ROT_RLEG_BACK",
	GBLOWN_ROTT_NUM,		GBLOWN_ROTT_NUM,				GBLOWNAREA_FINISH,	GBLOWNCODE_SPECIAL,	"",
};

//fixme--change this to include bbox info for shot prt, some kind of indication of what the monster model should do to make up for part loss (eg. change from headbald to headcrew)
extern Vect3 trStartKnarlyGilHack;

///find me a home
void EntToWorldMatrix(vec3_t org, vec3_t angles, Matrix4 &m);

int GetMinimumDamageForLevel(int damageLevel);

// from ai_bodyhuman.cpp
extern void blownpartremove_think(edict_t *self);

char* gzdognames[]=
{
	"GZ_DOG_LHIP",
	"GZ_DOG_RHIP",
	"GZ_DOG_HEAD",
	"GZ_DOG_FRONT",
	"GZ_DOG_RCHEST",
	"GZ_DOG_LCHEST",
	"NUM_DOGGOREZONES"
};

#define MAX_VEL_COMPONENT 50.0

GroundCallback			TheDogGroundCallback;
SetVectorCallback		TheDogSetVectorCallback;
FireCallBack			TheDogFireCallBack;
JumpCallBack			TheDogJumpCallBack;
ThudCallBack			TheDogThudCallBack;
KneeCallBack			TheDogKneeCallBack;
InAirCallBack			TheDogInAirCallBack;
FollowGenericPivot		TheDogFollowGenericPivot;
MouthCallback			TheDogMouthCallback;
FootRightCallback		TheDogFootRightCallback;
FootLeftCallback		TheDogFootLeftCallback;
MonsterSeqEndCallback	TheDogMonsterSeqEndCallback;
MonsterSeqBeginCallback	TheDogMonsterSeqBeginCallback;

//static GroundTransformationCallBack TheGroundTransformationCallBack;

// for use when the dog's melee attack hits a target
void DogAttackSuccess(edict_t *self, void* data)
{
	bodydog_c*body = NULL;
	if (self && self->ai)
	{
		body=(bodydog_c*)((ai_c*)((ai_public_c*)self->ai))->GetBody();
		((ai_c*)((ai_public_c*)self->ai))->MeleeFrame(*self);		
	}
	if (body)
	{
		body->SetLastAttackTime(*(float*)data);
	}
}

void blowndogpart_think(edict_t *self)
{
	if (VectorLengthSquared(self->velocity)<10)
	{
		gi.sound (self, CHAN_VOICE, gi.soundindex ("impact/gore/limbfall.wav"), .6, ATTN_NORM, 0);
		VectorClear(self->avelocity);
		self->s.angles[ROLL]=-90;
		//self->s.angles[2]=0;
		self->think = blownpartremove_think;
		self->nextthink=level.time+45;
		return;
	}
	self->nextthink=level.time+0.1;
}

void bodydog_c::PlayBarkSound(edict_t &monster)
{
	if (level.time - m_fLastBarkSoundTime > .5)
	{
		m_fLastBarkSoundTime = level.time;

		switch(gi.irand(0,2))
		{
		case 0:
			gi.sound (&monster, CHAN_BODY, gi.soundindex ("Enemy/Dog/Bark1.wav"), .8, ATTN_NORM, 0);
			break;
		case 1:
			gi.sound (&monster, CHAN_BODY, gi.soundindex ("Enemy/Dog/Bark2.wav"), .8, ATTN_NORM, 0);
			break;
		case 2:
			gi.sound (&monster, CHAN_BODY, gi.soundindex ("Enemy/Dog/Bark3.wav"), .8, ATTN_NORM, 0);
			break;
		}
	}
}

void bodydog_c::PlayBiteSound(edict_t &monster)
{
	if (level.time - m_fLastBiteSoundTime > .5)
	{
		m_fLastBiteSoundTime  = level.time;
		switch(gi.irand(0,1))
		{
		case 0:
			gi.sound (&monster, CHAN_BODY, gi.soundindex ("Enemy/Dog/Bite1.wav"), .8, ATTN_NORM, 0);
			break;
		case 1:
			gi.sound (&monster, CHAN_BODY, gi.soundindex ("Enemy/Dog/Bite2.wav"), .8, ATTN_NORM, 0);
			break;
		}
	}
}

void bodydog_c::PlayPainSound(edict_t &monster)
{
	if ( (monster.health > 0) && (level.time - m_fLastPainSoundTime > .5) )
	{
		m_fLastPainSoundTime  = level.time;
		gi.sound (&monster, CHAN_BODY, gi.soundindex ("Enemy/Dog/Die.wav"), .8, ATTN_NORM, 0);
	}
}

bodydog_c::bodydog_c()
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
	m_fLastBarkSoundTime = 0;
	m_fLastBiteSoundTime = 0;
	m_fLastPainSoundTime = 0;
}

void bodydog_c::SetGoreZoneDamageScale(edict_t &monster)
{
	if (numGoreZones<NUM_DOGGOREZONES)
	{
		gi.dprintf("couldn't setup gzone damage scales!!!\n");
		return;
	}

	gzones[GZ_DOG_HEAD].damScale=10.0f;
	gzones[GZ_DOG_LCHEST].damScale=1.0f;
	gzones[GZ_DOG_RCHEST].damScale=1.0f;
	gzones[GZ_DOG_FRONT].damScale=1.0f;
	gzones[GZ_DOG_LLEG_FRONT].damScale=1.0f;
	gzones[GZ_DOG_RLEG_FRONT].damScale=1.0f;
	gzones[GZ_DOG_LHIP].damScale=1.0f;
	gzones[GZ_DOG_RHIP].damScale=1.0f;
	gzones[GZ_DOG_LLEG_BACK].damScale=1.0f;
	gzones[GZ_DOG_RLEG_BACK].damScale=1.0f;
}

mmove_t	*bodydog_c::GetSequenceForMovement(edict_t &monster, vec3_t dest, vec3_t face, vec3_t org, vec3_t ang,  actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);

	mmove_t* returnAnim = NULL;

	if(!currentMoveFinished)
	{
		return currentmove;
	}
	// if he was sitting, stand up
	if (currentmove == &generic_dog_move_sit && 
		IsAvailableSequence(monster, &generic_dog_move_standup))
	{
		return &generic_dog_move_standup;
	}

	// this is a sad, sad approach, but we don't want the dog beginning any transitions
	//if he's attacked recently, cuz the ai has a tendency to request attack sequences for
	//several frames, then request a movement sequence for one frame, then more attack seqs, 
	//the result being that if we recommend a sniff transition in the middle of those attacks
	//the overall effect is jerky and yucky
/*	if (level.time - fLastAttackTime < 1.0f)
	{
		return &generic_dog_move_run;
	}*/

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

		//?
		return GetSequenceForAttack(monster, dest, face, the_ai->getTarget(), seqtype, goal_bbox, ideal_move, reject_actionflags);

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

	return returnAnim;
}

mmove_t	*bodydog_c::GetSequenceForStand(edict_t &monster, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);

	if (currentmove && (currentmove->suggested_action==ACTCODE_STAND)&&(!IsAnimationFinished()))
	{
		return currentmove;
	}

	if (currentmove == &generic_dog_move_sitdown &&
		VerifySequence(monster, &generic_dog_move_sit, reject_actionflags))
	{
		// if our current anim is 'sitting down', and we got here, that means
		//we just finished sitting down so go to 'sitting'
		return &generic_dog_move_sit;
	}
	else if (currentmove == &generic_dog_move_sniff &&
		VerifySequence(monster, &generic_dog_move_walk_from_sniff, reject_actionflags))
	{
		return &generic_dog_move_walk_from_sniff;
	}

	if(currentmove != &generic_dog_move_sit)
	{
		if (!(rand()%5) && VerifySequence(monster, &generic_dog_move_sitdown, reject_actionflags))
		{
			return &generic_dog_move_sitdown;
		}
	}
	else
	{
		if (!(rand()%5) && VerifySequence(monster, &generic_dog_move_standup, reject_actionflags))
		{
			return &generic_dog_move_standup;
		}
		else
		{
			return currentmove;
		}
	}

	if (the_ai && the_ai->HasHadTarget())
	{
		if(!(rand()%3) && currentmove != &generic_dog_move_bark && VerifySequence(monster, &generic_dog_move_bark, reject_actionflags))
		{
			PlayBarkSound(monster);
			return &generic_dog_move_bark;
		}
	}
	if(VerifySequence(monster, &generic_dog_move_pant, reject_actionflags))
	{
		return &generic_dog_move_pant;
	}

	return &generic_dog_move_walk;
}

mmove_t	*bodydog_c::GetSequenceForJump(edict_t &monster, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);
	return &generic_dog_move_walk;
}

mmove_t	*bodydog_c::GetSequenceForAttack(edict_t &monster, vec3_t dest, vec3_t face, edict_t *target, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);
	vec3_t vToTarget, vForward;
	float fToTarget;
	qboolean bInFront = false;

	fLastAttackTime = level.time;

	// make sure the pooch finishes his attack
	if (currentmove==&generic_dog_move_attack && !currentMoveFinished)
	{
		return currentmove;
	}

	// if he was sitting, stand up
	if (currentmove == &generic_dog_move_sit && 
		IsAvailableSequence(monster, &generic_dog_move_standup))
	{
		return &generic_dog_move_standup;
	}
	else if (currentmove == &generic_dog_move_sniff &&
		VerifySequence(monster, &generic_dog_move_walk_from_sniff, reject_actionflags))
	{
		return &generic_dog_move_walk_from_sniff;
	}

	if (the_ai)
	{
		the_ai->GetAimVector(vForward);
	}
	else if (monster.client)
	{
		AngleVectors(monster.client->ps.viewangles, vForward, NULL, NULL);
	}
	else
	{
		AngleVectors(monster.s.angles, vForward, NULL, NULL);
	}

/*	Com_Printf("%f, %f, %f\n", vForward[0], vForward[1], vForward[2]);
	VectorSubtract(face, monster.s.origin, vForward);
	VectorNormalize(vForward);*/

	if (target)
	{
		VectorSubtract(target->s.origin, monster.s.origin, vToTarget);
		fToTarget = VectorNormalize(vToTarget);
		bInFront = (DotProduct(vToTarget, vForward) > 0.5);
	}
	if (bInFront)						// if we're pointed at our target...
	{ 	
		if ((fToTarget <= (fJumpDist*1.2f)) &&	//...and we're close enough...
			(fToTarget >= (fJumpDist*0.8f)))		//...but not too close
		{
			return &generic_dog_move_attack;
		}
		else if (fToTarget > (fJumpDist*1.2f)) // too far away. run towards target 
		{
			if (0 == gi.irand(0, 3))
			{
				PlayBarkSound(monster);
			}
			return &generic_dog_move_run;
		}
		else // (fToTarget < (fJumpDist*0.8f)) too close
		{
			// same attack as above but it starts 40% into the anim
			if (&generic_dog_move_attack2 != currentmove)
			{	// play attack sound at beginning of attack
				PlayBarkSound(monster);
			}
			return &generic_dog_move_attack2;
		}
	}
	if (0 == gi.irand(0, 19))
	{
		PlayBarkSound(monster);
	}
	return &generic_dog_move_walk;
	//return &generic_dog_move_run;
}

mmove_t	*bodydog_c::GetSequenceForDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags)
{
	ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);
	mmove_t		*animation = &generic_dog_move_walk;

	// don't interrupt our first dying animation
	if (currentmove && (currentmove->suggested_action==ACTCODE_DEATH) && 
		!(currentmove->actionFlags&ACTFLAG_ALWAYSPRONE) &&
		!currentMoveFinished)
	{
		return currentmove;
	}

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
}

void bodydog_c::RecognizeGoreZones(edict_t &monster)
{
}

/* doggie gore zones

	GZ_DOG_HEAD,
	GZ_DOG_LCHEST,
	GZ_DOG_RCHEST,
	GZ_DOG_FRONT,
	GZ_DOG_LLEG_FRONT,
	GZ_DOG_RLEG_FRONT,
	GZ_DOG_LHIP,
	GZ_DOG_RHIP,
	GZ_DOG_LLEG_BACK,
	GZ_DOG_RLEG_BACK,
*/
void bodydog_c::RecognizeHuskyGoreZones(edict_t &monster)
{
	GhoulID tempHole = 0;
	int i;

	InitializeGoreZoneLevels(monster);

	SetGoreZoneDamageScale(monster);
	for (i=0;i<numGoreZones;i++)
	{
		gzones[i].damage=gzones[i].damage_level=0;
		gzones[i].id=gzones[i].parent_id=gzones[i].cap=gzones[i].blownCap=NULL_GhoulID;
		gzones[i].blownCapBolt=gzones[i].capbolt=gzones[i].bolt=gzones[i].capPieceBolt=0;
		gzones[i].capPiece=NULL;
		gzones[i].blowme=&husky_blown_parts[GBLOWN_HUSKY_NUM];
		gzones[i].childzone=numGoreZones;
	}

	// gore zones and parent_id's (parent_id's are derived from mesh names)
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_husky_torso");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_DOG_HEAD].parent_id=tempHole;
		SetGoreZone(monster, "GZ_HEAD", GZ_DOG_HEAD);
		gzones[GZ_DOG_LCHEST].parent_id=tempHole;
		SetGoreZone(monster, "GZ_LCHEST", GZ_DOG_LCHEST);
		gzones[GZ_DOG_RCHEST].parent_id=tempHole;
		SetGoreZone(monster, "GZ_RCHEST", GZ_DOG_RCHEST);
		gzones[GZ_DOG_FRONT].parent_id=tempHole;
		SetGoreZone(monster, "GZ_FRONT", GZ_DOG_FRONT);
	}
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_husky_lleg_front");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_DOG_LLEG_FRONT].parent_id=tempHole;
	}
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_husky_rleg_front");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_DOG_RLEG_FRONT].parent_id=tempHole;
	}
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_husky_lhip");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_DOG_LHIP].parent_id=tempHole;
		SetGoreZone(monster, "GZ_LHIP", GZ_DOG_LHIP);
	}
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_husky_rhip");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_DOG_RHIP].parent_id=tempHole;
		SetGoreZone(monster, "GZ_RHIP", GZ_DOG_RHIP);
	}
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_husky_lleg_back");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_DOG_LLEG_BACK].parent_id=tempHole;
	}
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_husky_rleg_back");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_DOG_RLEG_BACK].parent_id=tempHole;
	}
	

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

	// bolts are used for hit location detection (which bolt is the dmg nearest?) and
	//placing exit wounds (probably not for the dog, though)
	SetGoreBolt(monster, "gbolt_head", GZ_DOG_HEAD);
	SetGoreBolt(monster, "gbolt_front", GZ_DOG_FRONT);
	SetGoreBolt(monster, "gbolt_lchest", GZ_DOG_LCHEST);
	SetGoreBolt(monster, "gbolt_rchest", GZ_DOG_RCHEST);

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
}

void bodydog_c::RecognizeRottweilerGoreZones(edict_t &monster)
{
	GhoulID	tempHole = 0;
	int i;

	InitializeGoreZoneLevels(monster);

	SetGoreZoneDamageScale(monster);
	for (i=0;i<numGoreZones;i++)
	{
		gzones[i].damage=gzones[i].damage_level=0;
		gzones[i].id=gzones[i].parent_id=gzones[i].cap=gzones[i].blownCap=NULL_GhoulID;
		gzones[i].blownCapBolt=gzones[i].capbolt=gzones[i].bolt=gzones[i].capPieceBolt=0;
		gzones[i].capPiece=NULL;
		gzones[i].blowme=&rott_blown_parts[GBLOWN_ROTT_NUM];
		gzones[i].childzone=numGoreZones;
	}

	// gore zones and parent_id's (parent_id's are derived from mesh names)
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rot_torso");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_DOG_HEAD].parent_id=tempHole;
		SetGoreZone(monster, "GZ_HEAD", GZ_DOG_HEAD);
		gzones[GZ_DOG_LCHEST].parent_id=tempHole;
		SetGoreZone(monster, "GZ_LCHEST", GZ_DOG_LCHEST);
		gzones[GZ_DOG_RCHEST].parent_id=tempHole;
		SetGoreZone(monster, "GZ_RCHEST", GZ_DOG_RCHEST);
		gzones[GZ_DOG_FRONT].parent_id=tempHole;
		SetGoreZone(monster, "GZ_FRONT", GZ_DOG_FRONT);
	}
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rot_lleg_front");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_DOG_LLEG_FRONT].parent_id=tempHole;
	}
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rot_rleg_front");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_DOG_RLEG_FRONT].parent_id=tempHole;
	}
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rot_torso");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_DOG_LHIP].parent_id=tempHole;
		SetGoreZone(monster, "GZ_LHIP", GZ_DOG_LHIP);
	}
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rot_rleg_back");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_DOG_RHIP].parent_id=tempHole;
		SetGoreZone(monster, "GZ_RHIP", GZ_DOG_RHIP);
	}
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rot_lleg_back");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_DOG_LLEG_BACK].parent_id=tempHole;
	}
	tempHole=monster.ghoulInst->GetGhoulObject()->FindPart("_rot_rleg_back");
	if (tempHole && monster.ghoulInst->GetPartOnOff(tempHole))
	{
		gzones[GZ_DOG_RLEG_BACK].parent_id=tempHole;
	}
	

	// blown caps are the chunks of meat on the blown off part that get turned on at the 
	//point of separation
	SetGoreBlownCap(monster, "_CAP_ROT_RLEG_SHOULDER", GZ_DOG_RLEG_FRONT);
	SetGoreBlownCap(monster, "_CAP_ROT_LLEG_SHOULDER", GZ_DOG_LLEG_FRONT);
	SetGoreBlownCap(monster, "_CAP_ROT_RLEG_HIP", GZ_DOG_RHIP);
	SetGoreBlownCap(monster, "_CAP_ROT_LLEG_HIP", GZ_DOG_LLEG_BACK);

	// blown cap bolts are the points at which the separation occurs when a part is
	//blown off (used as the origin for various blood-related effects). they point from
	//the separated part toward the parent (e.g. from the leg to the hip)
	//
	//the dog doesn't have any.
	//

	// caps are the chunks of meat on the parent part that get turned on at the
	//point of separation
	SetGoreCap(monster, "_CAP_ROT_TORSO_RSHOULDER", GZ_DOG_RLEG_FRONT);
	SetGoreCap(monster, "_CAP_ROT_TORSO_LSHOULDER", GZ_DOG_LLEG_FRONT);
	SetGoreCap(monster, "_CAP_ROT_TORSO_RHIP", GZ_DOG_RHIP);
	SetGoreCap(monster, "_CAP_ROT_TORSO_LHIP", GZ_DOG_LLEG_BACK);

	// cap bolts are the points at which the separation occurs when a part is 
	//blown off (used as the origin for various blood-related effects). they point from
	//the parent toward the separated part (e.g. from the hip to the leg)
	SetGoreCapBolt(monster, "GBOLT_CAP_ROT_TORSO_RSHOULDER", GZ_DOG_RLEG_FRONT);
	SetGoreCapBolt(monster, "GBOLT_CAP_ROT_TORSO_LSHOULDER", GZ_DOG_LLEG_FRONT);
	SetGoreCapBolt(monster, "GBOLT_CAP_ROT_TORSO_RHIP", GZ_DOG_RHIP);
	SetGoreCapBolt(monster, "GBOLT_CAP_ROT_TORSO_LHIP", GZ_DOG_LLEG_BACK);

	// bolts are used for hit location detection (which bolt is the dmg nearest?) and
	//placing exit wounds (probably not for the dog, though)
	SetGoreBolt(monster, "gbolt_head", GZ_DOG_HEAD);
	SetGoreBolt(monster, "gbolt_front", GZ_DOG_FRONT);
	SetGoreBolt(monster, "gbolt_lchest", GZ_DOG_LCHEST);
	SetGoreBolt(monster, "gbolt_rchest", GZ_DOG_RCHEST);

	// blowme's are the parts that'll get blown off when an area is hit. the possible
	//values are in the first column of dogblown_parts[], which are entries in dogblown_index.
	gzones[GZ_DOG_LLEG_FRONT].blowme = &rott_blown_parts[GBLOWN_ROTT_LLEG_FRONT];
	gzones[GZ_DOG_RLEG_FRONT].blowme = &rott_blown_parts[GBLOWN_ROTT_RLEG_FRONT];
	gzones[GZ_DOG_LLEG_BACK].blowme = &rott_blown_parts[GBLOWN_ROTT_LLEG_BACK];
	gzones[GZ_DOG_RHIP].blowme = &rott_blown_parts[GBLOWN_ROTT_RHIP];
	gzones[GZ_DOG_RLEG_BACK].blowme = &rott_blown_parts[GBLOWN_ROTT_RHIP];

	// childzone's are used for turning off decals and maybe caps when subparts are 
	//blown off of parts (e.g. lower arm blown off then upper arm blown off).
	gzones[GZ_DOG_RHIP].childzone = GZ_DOG_RLEG_BACK;
	gzones[GZ_DOG_RLEG_BACK].childzone = GZ_DOG_RHIP;

	// a little weirdness here to make the right hip and right leg react the same
	//when shot (they're both s'posed to fly off, attached to each other, when shot)
	SetGoreBlownCap(monster, "_CAP_ROT_RLEG_HIP", GZ_DOG_RLEG_BACK);
	SetGoreCap(monster, "_CAP_ROT_TORSO_RHIP", GZ_DOG_RLEG_BACK);
	SetGoreCapBolt(monster, "GBOLT_CAP_ROT_TORSO_RHIP", GZ_DOG_RLEG_BACK);
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



int bodydog_c::ShowDamage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb)
{
	vec3_t		bodyloc;//, footorigin;
	gz_code		listOfFrontCodes[3];
	int			numOfFrontCodes=0;
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

	// play pain sound
	PlayPainSound(monster);

	if (ai_goretest->value)
	{
		gi.dprintf("%d hits!\n", bullet_numHits);
	}

	//shotgun shot--plop on generic damage for now
	if (dflags&DT_DIRECTIONALGORE)
	{
		//new, very random zone-chooser.
		vec3_t myFacing, dmgFacing, mySide, myUp;

		AngleVectors(monster.s.angles, myFacing, mySide, myUp);

		VectorSubtract(monster.s.origin, origin, dmgFacing);
		VectorNormalize(dmgFacing);

		if(DotProduct(myFacing, dmgFacing) > .707)
		{//nearly colinear - being hit from behind
			backCode=GZ_DOG_FRONT;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_LLEG_BACK;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_RLEG_BACK;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_LHIP;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_RHIP;
		}
		else if(DotProduct(myFacing, dmgFacing) < -.707)
		{//in the front - ow!
			//backCode=GZ_CHEST_BACK;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_HEAD;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_RCHEST;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_LCHEST;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_FRONT;
		}
		else if(DotProduct(mySide, dmgFacing) > 0)
		{//um...
			backCode=GZ_DOG_LLEG_FRONT;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_RCHEST;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_RLEG_FRONT;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_RHIP;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_RLEG_BACK;
		}
		else
		{//er...
			backCode=GZ_DOG_RLEG_FRONT;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_LCHEST;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_LLEG_FRONT;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_LHIP;
			listOfFrontCodes[numOfFrontCodes++]=GZ_DOG_LLEG_BACK;
		}
	}
	else
	{
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
								gi.dprintf("considering %s for frontHole\n",gzdognames[j]);
							}
							if (!gzones[j].bolt)
							{
								if (!frontHole || tdist>bullet_Hits[i].Distance)
								{
									tdist=bullet_Hits[i].Distance;
									frontHole=&gzones[j];
									frontCode = (gz_dogcode)j;
									if (ai_goretest->value)
									{
										gi.dprintf("Setting frontHole to %s\n",gzdognames[j]);
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
									frontCode = (gz_dogcode)j;
									bestGoreDist = curGoreDist;
									if (ai_goretest->value)
									{
										gi.dprintf("Setting frontHole to %s\n",gzdognames[j]);
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
								gi.dprintf("considering %s for backHole\n",gzdognames[j]);
							}
							if (!gzones[j].bolt)
							{
								if (!backHole || tdist>bullet_Hits[i].Distance)
								{
									tdist=bullet_Hits[i].Distance;
									backHole=&gzones[j];
									backCode = (gz_dogcode)j;
									if (ai_goretest->value)
									{
										gi.dprintf("Setting backHole to %s\n",gzdognames[j]);
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
									backCode = (gz_dogcode)j;
									bestGoreDist = curGoreDist;
									if (ai_goretest->value)
									{
										gi.dprintf("Setting backHole to %s\n",gzdognames[j]);
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
	}

	//for each of the front holes, scale the damage (this should cover massive headshot damage, and lower armshot damage)
	for (i=0;i<numOfFrontCodes;i++)
	{
		frontCode=listOfFrontCodes[i];
		//make sure hole is valid
		if (frontCode >= 0 && frontCode < numGoreZones)
		{
			take=(int)((float)take*gzones[frontCode].damScale);
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
//		if (monster.health <= damage && frontHole->blownCap && (ai_goretest->value || (dflags & DT_SEVER)) && (!lock_sever))
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
			if (frontHole->cap)
			{
				myInstance->GetInstPtr()->SetPartOnOff(frontHole->cap,true);
				myInstance->GetInstPtr()->SetFrameOverride("c","c_blood",frontHole->cap);
			}
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
					monster.ghoulInst->AddMatrixCallBack(&TheDogSetVectorCallback,frontHole->capbolt,IGhoulInst::MatrixType::Entity);
					spout = frontHole - gzones;
				}
				else if (frontHole->bolt)
				{
					monster.ghoulInst->AddMatrixCallBack(&TheDogSetVectorCallback,frontHole->bolt,IGhoulInst::MatrixType::Entity);
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

	// throw some fleshy debris chunks for the kiddies
	if (!VectorCompare(point, vec3_origin))
	{
		int numchunks = gi.irand(0,5);
		FX_ThrowDebris(point,vec3_origin, numchunks, DEBRIS_TINY, MAT_ROCK_FLESH, 0, 0, 0, SURF_BLOOD);
	}

	return take;
}

void bodydog_c::SetRootBolt(edict_t &monster)
{
	GhoulID	tempID;
	GhoulID	tempBolt;
	GhoulID	tempNote;

	// the death animations look a little weird if the dog obtains a knockback-induced velocity
	monster.flags &= ~FL_NO_KNOCKBACK;

	AllocateGoreZones(monster);
	RecognizeGoreZones(monster);
	tempBolt=monster.ghoulInst->GetGhoulObject()->FindPart("quake_origin");
	if (tempBolt)
	{
		monster.ghoulInst->AddMatrixCallBack(&TheDogFollowGenericPivot,tempBolt,IGhoulInst::MatrixType::JacobianEntity);
	}
	else
	{
		gi.dprintf("WARNING: monster origin not found--wacky!!\n");
	}

	tempID=monster.ghoulInst->GetGhoulObject()->FindPart("quake_ground");
	if (tempID)
	{
		monster.ghoulInst->AddMatrixCallBack(&TheDogGroundCallback,tempID,IGhoulInst::MatrixType::Entity);
	}
	tempBolt=monster.ghoulInst->GetGhoulObject()->FindPart("GBOLT_HEAD");
	if (tempBolt)
	{
		monster.ghoulInst->AddMatrixCallBack(&TheDogMouthCallback,tempBolt,IGhoulInst::MatrixType::Entity);
	}
	else
	{
		gi.dprintf("WARNING: monster mouth not found--sight will be defective!!\n");
	}


	tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("BOS");
	if (tempNote)
	{
		monster.ghoulInst->AddNoteCallBack(&TheDogMonsterSeqBeginCallback,tempNote);
	}

	tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("EOS");
	if (tempNote)
	{
		monster.ghoulInst->AddNoteCallBack(&TheDogMonsterSeqEndCallback,tempNote);
	}

	tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("jump");
	if (tempNote)
	{
		monster.ghoulInst->AddNoteCallBack(&TheDogJumpCallBack,tempNote);
	}

	tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("thud");
	if (tempNote)
	{
		monster.ghoulInst->AddNoteCallBack(&TheDogThudCallBack,tempNote);
	}

	tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("knee");
	if (tempNote)
	{
		monster.ghoulInst->AddNoteCallBack(&TheDogKneeCallBack,tempNote);
	}

	tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("inair");
	if (tempNote)
	{
		monster.ghoulInst->AddNoteCallBack(&TheDogInAirCallBack,tempNote);
	}

	tempNote=monster.ghoulInst->GetGhoulObject()->FindNoteToken("fire");
	if (tempNote)
	{
		monster.ghoulInst->AddNoteCallBack(&TheDogFireCallBack,tempNote);
	}
}

attacks_e bodydog_c::GetBestWeapon(edict_t &monster)
{
	return ATK_DOG_ATTACK;
}

void bodydog_c::BiteMe(edict_t &monster, qboolean forceFireRate)
{
	Matrix4 boltToDog, dogToWorld, final;
	vec3_t firePoint = {0,0,0}, forward, rotateangs;
	IGhoulInst *instDog = monster.ghoulInst;
	IGhoulObj		*obj = NULL;
	GhoulID boltID = 0;
	if (instDog)
	{
		if(obj = instDog->GetGhoulObject())
		{
			boltID = obj->FindPart("SBOLT_HUSKY_TONGUE");
		}
	}
	if (boltID)
	{
		ai_c *the_ai=(ai_c*)((ai_public_c*)monster.ai);
		if (the_ai)
		{
			instDog->GetBoltMatrix(level.time, boltToDog, boltID,IGhoulInst::MatrixType::Entity);
			VectorDegreesToRadians(monster.s.angles,rotateangs);
			dogToWorld.Identity();
			dogToWorld.Rotate(rotateangs[2],rotateangs[0],-rotateangs[1]);//grr.
			final.Concat(boltToDog,dogToWorld);
			final.GetRow(3,(Vect3)firePoint);
			VectorAdd(monster.s.origin,firePoint,firePoint);

			the_ai->GetAimVector(forward);


			weapons.attack(ATK_DOG_ATTACK, &monster, firePoint, forward, DogAttackSuccess);

			rLastFireTime = level.time;
		}
	}
	PlayBiteSound(monster);
}

void bodydog_c::NextMovement(edict_t &monster, vec3_t curDist, float scale)
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

void bodydog_c::FinishMove(edict_t &monster)
{
	bodyorganic_c::FinishMove(monster);
	if (currentmove==&generic_dog_move_attack)
	{
		bTurnedAfterAttack = false;
	}
}

qboolean bodydog_c::PlayAnimation(edict_t &monster, mmove_t *newanim, bool forceRestart)
{
	forceRestart |= ForceNextAnim;
	ForceNextAnim = false;

	return bodyorganic_c::PlayAnimation(monster, newanim, forceRestart);
}

bodydog_c::bodydog_c(bodydog_c *orig)
: bodyorganic_c(orig)
{
	bTurnedAfterAttack = orig->bTurnedAfterAttack;
	FinalAnim = orig->FinalAnim;
	FinalSound = orig->FinalSound;

	fJumpDist = orig->fJumpDist;
	fJumpSpeed = orig->fJumpSpeed;
	fLastAttackTime = orig->fLastAttackTime;
	fLastHeadTurnTime = orig->fLastHeadTurnTime;
	m_fLastBarkSoundTime = orig->m_fLastBarkSoundTime;
	m_fLastBiteSoundTime = orig->m_fLastBiteSoundTime;
	m_fLastPainSoundTime = orig->m_fLastPainSoundTime;
}

void bodydog_c::Evaluate(bodydog_c *orig)
{
	bTurnedAfterAttack = orig->bTurnedAfterAttack;
	FinalAnim = orig->FinalAnim;
	FinalSound = orig->FinalSound;

	fJumpDist = orig->fJumpDist;
	fJumpSpeed = orig->fJumpSpeed;
	fLastAttackTime = orig->fLastAttackTime;
	fLastHeadTurnTime = orig->fLastHeadTurnTime;

	m_fLastBarkSoundTime = orig->m_fLastBarkSoundTime;
	m_fLastBiteSoundTime = orig->m_fLastBiteSoundTime;
	m_fLastPainSoundTime = orig->m_fLastPainSoundTime;
	bodyorganic_c::Evaluate(orig);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
//	husky-specific
//
///////////////////////////////////////////////////////////////////////////////////////////////

bodyhusky_c::bodyhusky_c()
: bodydog_c()
{
}

void bodyhusky_c::RecognizeGoreZones(edict_t &monster)
{
	RecognizeHuskyGoreZones(monster);
}

gz_blown_part *bodyhusky_c::GetBlownPartForIndex(blown_index theIndex)
{
	if (theIndex < 0 || theIndex > GBLOWN_HUSKY_NUM)
	{
		return &husky_blown_parts[GBLOWN_HUSKY_NUM];
	}
	return &husky_blown_parts[theIndex];
}

void bodyhusky_c::AllocateGoreZones(edict_t &monster)
{
	//eliminate old gore zones...
	bodyorganic_c::AllocateGoreZones(monster);
	numGoreZones=NUM_DOGGOREZONES;
	gzones=new gz_info[numGoreZones];
}

bodyhusky_c::bodyhusky_c(bodyhusky_c *orig)
: bodydog_c(orig)
{
}

void bodyhusky_c::Evaluate(bodyhusky_c *orig)
{
	bodydog_c::Evaluate(orig);
}

void bodyhusky_c::Read()
{
	char	loaded[sizeof(bodyhusky_c)];

	gi.ReadFromSavegame('AIBH', loaded, sizeof(bodyhusky_c));
	Evaluate((bodyhusky_c *)loaded);
}

void bodyhusky_c::Write()
{
	bodyhusky_c	*savable;

	savable = new bodyhusky_c(this);
	gi.AppendToSavegame('AIBH', savable, sizeof(*this));
	delete savable;

	WriteGZ();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
//	rottweiler-specific
//
///////////////////////////////////////////////////////////////////////////////////////////////

bodyrott_c::bodyrott_c()
: bodydog_c()
{
}

void bodyrott_c::RecognizeGoreZones(edict_t &monster)
{
	RecognizeRottweilerGoreZones(monster);
}

gz_blown_part *bodyrott_c::GetBlownPartForIndex(blown_index theIndex)
{
	if (theIndex < 0 || theIndex > GBLOWN_ROTT_NUM)
	{
		return &rott_blown_parts[GBLOWN_ROTT_NUM];
	}
	return &rott_blown_parts[theIndex];
}

void bodyrott_c::AllocateGoreZones(edict_t &monster)
{
	//eliminate old gore zones...
	bodyorganic_c::AllocateGoreZones(monster);
	numGoreZones = NUM_DOGGOREZONES;
	gzones = new gz_info[numGoreZones];
}

bodyrott_c::bodyrott_c(bodyrott_c *orig)
: bodydog_c(orig)
{
}

void bodyrott_c::Evaluate(bodyrott_c *orig)
{
	bodydog_c::Evaluate(orig);
}

void bodyrott_c::Read()
{
	char	loaded[sizeof(bodyrott_c)];

	gi.ReadFromSavegame('AIBR', loaded, sizeof(bodyrott_c));
	Evaluate((bodyrott_c *)loaded);
}

void bodyrott_c::Write()
{
	bodyrott_c	*savable;

	savable = new bodyrott_c(this);
	gi.AppendToSavegame('AIBR', savable, sizeof(*this));
	delete savable;

	WriteGZ();
}

