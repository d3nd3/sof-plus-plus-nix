
//body--handle all super-basic animation stuff
#include "g_local.h"
#include "matrix4.h"

#include "ai_body.h"
#include "ai_bodynoghoul.h"
#include "ai_bodyhuman.h"
#include "ai_bodydekker.h"
#include "ai_bodydog.h"
#include "ai_bodycow.h"
#include "ai.h"

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

extern int ai_loadAllBolts;

body_c *body_c::NewClassForCode(int code)
{
	switch (code)
	{
	default:
	case BODY:		
	case BODY_HUMAN:
	case BODY_DOG:
	case BODY_ORGANIC:
		gi.dprintf("ERROR: invalid body class code: %d\n",code);
	case BODY_MESO:
		return new bodymeso_c();
	case BODY_ECTO:
		return new bodyecto_c();
	case BODY_HUSKY:
		return new bodyhusky_c();
	case BODY_ROTT:
		return new bodyrott_c();
	case BODY_FEMALE:
		return new bodyfemale_c();
	case BODY_HELI:
		return new body_heli();
	case BODY_TANK:
		return new body_tank();
	case BODY_SNOWCAT:
		return new body_snowcat();
	case BODY_COW:
		return new bodycow_c();
	case BODY_DEKKER:
		return new bodydekker_c();
	}
}

//static GroundTransformationCallBack TheGroundTransformationCallBack;

body_c::body_c()
{
	currentmove = NULL;
	owner = NULL;
	currentMoveFinished = true;
	holdCode = HOLDCODE_NO;
	frame_flags = 0;
	emotion_expire=level.time;
	emotion = EMOTION_NORMAL;
	emotionScripted=false;
	rLastFireTime=level.time-10;
	lLastFireTime=level.time-10;
	InitialKilledTime = 0.0;
	LastKilledTime = 0.0;
	lastStartTime=level.time;
	LastDFlags = 0;
	ForceNextAnim = false;
	TrainDeath = false;
}

void body_c::FinishMove(edict_t &monster)
{
//	gi.dprintf("finishing anim at %f\n",level.time);
	currentMoveFinished = true;
}

void body_c::HoldAnimation(edict_t &monster, hold_code whyHold, float holdTime)
{
	GhoulID curAnim;

	curAnim = GetSequence(monster, currentmove);
	if (!curAnim)
	{
		return;
	}
	monster.ghoulInst->Pause(holdTime);
	holdCode = whyHold;
}

void body_c::ResumeAnimation(edict_t &monster)
{
	GhoulID curAnim;

	if ((holdCode == HOLDCODE_NO)  || (holdCode == HOLDCODE_GIBBED))
	{
		return;
	}

	curAnim = GetSequence(monster, currentmove);
	if (!curAnim)
	{
		return;
	}

	//fixme: ghoul glitch makes motion track go nuts while held
//	VectorClear(monster.intend_velocity);
	monster.ghoulInst->Resume(level.time);
	holdCode = HOLDCODE_NO;
}

qboolean body_c::IsAnimationHeld(hold_code queryCode)
{
	if (queryCode == HOLDCODE_NO)
	{
		return (holdCode != HOLDCODE_NO);
	}
	return (holdCode == queryCode);
}


qboolean body_c::IsAnimationFinished()
{
	if (!currentmove)
	{
		return true;
	}
	if (currentmove->actionFlags & ACTFLAG_LOOPANIM && !(currentmove->actionFlags&ACTFLAG_FULLANIM))
	{
		return true;
	}
	return currentMoveFinished;

}

//super-secret version of IsAnimationFinished--doesn't make exceptions for looping animations.
//use sparingly--i only added this for path_corners, so you specify a cluster of animations (including looping ones)
//	and it could wait until each one finishes before going to the next.
qboolean body_c::HasAnimationHitEOS()
{
	if (!currentmove)
	{
		return true;
	}
	return currentMoveFinished;
}

qboolean body_c::IsAnimationReversed()
{
	if (currentmove)
	{
		return currentmove->actionFlags & ACTFLAG_REVERSEANIM;
	}
	return false;
}

GhoulID body_c::GetSequence(edict_t &monster, mmove_t *newanim)
{
	ggObjC	*myObject;
	if (monster.ghoulInst && newanim && newanim->ghoulSeqName && (myObject=game_ghoul.FindObject(monster.ghoulInst->GetGhoulObject())))
	{
		char seqName[GGHOUL_OBJ_NAME_LEN];
		Com_sprintf(seqName, GGHOUL_OBJ_NAME_LEN, "ghoul/%s/%s.ghl",myObject->GetName(),newanim->ghoulSeqName);
		return monster.ghoulInst->GetGhoulObject()->FindSequence(seqName);
	}
	return NULL_GhoulID;
}

qboolean body_c::IsAvailableSequence(edict_t &monster, mmove_t *newanim)
{
	if(monster.ai)
	{
		ai_c *the_ai = (ai_c*)((ai_public_c*)monster.ai);

		if(the_ai)
		{
			ggObjC *myObj = the_ai->GetGhoulObj();
			if(myObj)
			{
				if(!strncmp(myObj->GetName(), "enemy/meso", strlen("enemy/meso")) ||
					!strncmp(myObj->GetName(), "enemy/ecto", strlen("enemy/ecto")) ||
					!strncmp(myObj->GetName(), "enemy/female", strlen("enemy/female")))
				{
					return myObj->IsSequenceAvailable(newanim - MMoves);
				}
			}
		}
	}
	return (GetSequence(monster, newanim)!=NULL_GhoulID);
}

qboolean body_c::PlayAnimation(edict_t &monster, mmove_t *newanim, bool forceRestart)
{
	//if monster has valid ghoul model, and animseq has valid ghoul seq., then play the sequence
	GhoulID curAnim;
	bool reverseAnim=false;
	float playPos=newanim->playPos;
	bool matchAnim=(1&&(newanim->actionFlags&ACTFLAG_MATCHANIM));
	float startTime=level.time;

	//what goes on in here (GetSequence calls mostly, it looks like) seems to be expensive, so just accept it if the play call isn't going to do anything anyway
	if (newanim==currentmove && !forceRestart && !currentMoveFinished)
	{
		//just make sure i'm not paused
		ResumeAnimation(monster);
		return true;
	}

	//don't match the end of a sequence--if the new anim isn't looping, it'll never finish!!!
	if (currentmove && !(currentmove->actionFlags&ACTFLAG_LOOPANIM) && currentMoveFinished && matchAnim)
	{
		matchAnim=false;
		forceRestart=true;
	}

	if(newanim->actionFlags & ACTFLAG_FIREFROMGUN)
	{
		startTime = level.time + .1;
	}
	//SFS--Note: the contents of the next set of brackets is a trashy hack.  if we can do without it, that would be nice.
	else if (currentmove && (currentmove->actionFlags & ACTFLAG_FIREFROMGUN))//just coming from a firefromgun anim, consider screwing with angles and messing everything up
	{
		if ((newanim->suggested_action == ACTCODE_PAIN)||(newanim->suggested_action == ACTCODE_DEATH))//only bother with pains & deaths; other anims will wait till firfromguns sort theyselves out
		{
			Matrix4 m;
			vec3_t	mouthface, mouthangs;
			ai_c *my_ai = (ai_c *)((ai_public_c *)monster.ai);
			int		taxis;
			monster.ghoulInst->GetBoltMatrix(level.time, m, monster.ghoulInst->GetGhoulObject()->FindPart("sbolt_mouth"), IGhoulInst::MatrixType::Entity);

			if (my_ai)
			{
				m.GetRow(0,*(Vect3 *)mouthface);//forward vector, hopefully
				mouthface[2] = 0;
				vectoangles(mouthface,mouthangs);
				if (GetClassCode() == BODY_HUSKY || GetClassCode() == BODY_ROTT)
				{
					mouthangs[YAW] -= 90;
				}
				VectorAdd(mouthangs, monster.s.angles,mouthangs);
				for (taxis=0;taxis<3;taxis++)
				{
					mouthangs[taxis]=anglemod(mouthangs[taxis]);
				}

				VectorCopy(mouthangs, monster.s.angles);
//				VectorCopy(mouthangs, mouthface);
//				my_ai->SetLookAngles(mouthface);
//				AngleVectors(mouthangs,mouthface,NULL,NULL);
			}
		}
	}

	if (holdCode == HOLDCODE_GIBBED)
	{
		return false;
	}

	curAnim = GetSequence(monster, newanim);
	if (!curAnim)
	{
		if (monster.classname && monster.targetname)
		{
			gi.dprintf("%s %s couldn't play animation %s!\n", monster.classname, monster.targetname, newanim->ghoulSeqName);
		}
		else if (monster.classname)
		{
			gi.dprintf("Some %s couldn't play animation %s!\n", monster.classname, newanim->ghoulSeqName);
		}
		else
		{
			gi.dprintf("Unknown monster couldn't play animation %s!\n", newanim->ghoulSeqName);
		}
		return false;
	}

	if (newanim->actionFlags & ACTFLAG_MATCHABS)
	{
		//only match for similar sequences
		if (newanim!=currentmove && currentmove->suggested_action==newanim->suggested_action)
		{
			startTime=lastStartTime;
		}
	}
 
	//reverse the animation?
	if (newanim->actionFlags & ACTFLAG_REVERSEANIM)
	{
		reverseAnim = true;
		// fixme? kef -- given that you can now store the playPos as part of the mmove_t struct, 
		//I don't know if we want to just plain override it here.
		playPos = 1.0f;
	}
	
//	get the old quakeorigin -- THIS WAS A WEAK ATTEMPT TO SOLVE THE POPPING BETWEEN ANIMATIONS.  DIDN'T WORK!
/*	Matrix4		BoltMatrix;
	bool		doit = false;
	vec3_t		oldOrigin;

	if (monster.ghoulInst->GetStateSequence())
	{
		Vect3		OldQuakeOrigin;
		int			oldGhoulID;

		doit = true;
		oldGhoulID = monster.ghoulInst->GetGhoulObject()->FindPart("quake_ground");
		monster.ghoulInst->GetBoltMatrix(level.time, BoltMatrix, oldGhoulID, IGhoulInst::MatrixType::Entity);
		BoltMatrix.GetRow(3,OldQuakeOrigin);
		VectorCopy((float *)&OldQuakeOrigin, oldOrigin);
	}
*/
	//check to see if this should loop
	if (newanim->actionFlags & ACTFLAG_LOOPANIM)
	{
		monster.ghoulInst->Play(curAnim,startTime,playPos,forceRestart,IGhoulInst::Loop, matchAnim, reverseAnim);
	}
	else
	{
		monster.ghoulInst->Play(curAnim,startTime,playPos,forceRestart,IGhoulInst::Hold, matchAnim, reverseAnim);
	}

	lastStartTime=startTime;

	if (curAnim!=GetSequence(monster, currentmove)
		/*(!newanim || !currentmove || stricmp(newanim->ghoulSeqName, currentmove->ghoulSeqName))*/
		/*newanim != currentmove*/
		||forceRestart)
	{
		currentMoveFinished = false;

		//quake_ground may be drastically changed--i'll try to adjust bbox again this frame, so set the ground for that
		if (monster.ai)
		{

			//	get the new quake origin
			Matrix4		m;
			int			newGhoulID;
			vec3_t		curPos;
			ai_c		*my_ai = (ai_c *)((ai_public_c *)monster.ai);


			newGhoulID = monster.ghoulInst->GetGhoulObject()->FindPart("quake_ground");

			if (newGhoulID)
			{
				monster.ghoulInst->GetBoltMatrix(level.time, m, newGhoulID, IGhoulInst::MatrixType::Entity);
				m.GetRow(3,*(Vect3 *)curPos);
	//			if (self->groundentity)
				{
					if (my_ai)
					{
						my_ai->SetGround(curPos[2]);
					}
				}
			}
		}

	}

/*	if (doit)
	{

		//	get the new quake origin
		Vect3		NewQuakeOrigin;
		vec3_t		newOrigin;
		int			newGhoulID;

		newGhoulID = monster.ghoulInst->GetGhoulObject()->FindPart("quake_ground");

		monster.ghoulInst->GetBoltMatrix(level.time, BoltMatrix, newGhoulID, IGhoulInst::MatrixType::Entity);
		BoltMatrix.GetRow(3,NewQuakeOrigin);
		VectorCopy((float *)&NewQuakeOrigin, newOrigin);

		float change =  oldOrigin[2] - newOrigin[2];
//		monster.s.origin[2] += 5;
		monster.s.origin[2] += change;
	}
*/	SetMove(newanim);

	ResumeAnimation(monster);

	return true;
}

qboolean body_c::MatchAnimation(edict_t &monster, mmove_t *newanim)
{
	return PlayAnimation(monster, newanim, false);
}

qboolean body_c::SetAnimation(edict_t &monster, mmove_t *newanim)
{
	return PlayAnimation(monster, newanim, false);
}

qboolean body_c::ForceAnimation(edict_t &monster, mmove_t *newanim)
{
	return PlayAnimation(monster, newanim, true);
}

#define MAX_VEL_COMPONENT 50.0

void body_c::NextMovement(edict_t &monster, vec3_t curDist, float scale)
{
	vec3_t forward,right,up,tempdist,distaccum;
	int i;

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

#define TURNDIST_MIN 64
#define TURNDIST_MAX 192

void body_c::NextTurn(float scale, edict_t &monster, float default_turn, turninfo_s &turninfo)
{
//	return scale * default_turn + VectorLength(monster.intend_avelocity)*FRAMETIME;
	ai_c	*the_ai=NULL;

	if (monster.ai)
	{
		the_ai = (ai_c*)((ai_public_c*)monster.ai);
	}

	if (!currentmove || !the_ai)
	{
		turninfo.max_turn = /*scale * */default_turn /*+ VectorLength(monster.intend_avelocity)*FRAMETIME*/;
		turninfo.min_turn = /*scale * */-default_turn /*+ VectorLength(monster.intend_avelocity)*FRAMETIME*/;
	}
	else
	{
		float scale = 1.0;

		if(the_ai->getTarget()&&monster.health>0)
		{
			vec3_t dif;
			vec3_t targ_pos;

			the_ai->getTargetPos(targ_pos);

			VectorSubtract(monster.s.origin, targ_pos, dif);
			float dist = VectorLength(dif);

			if(the_ai->getTarget()->client)
			{	// turn faster the closer a player is
				if(dist < TURNDIST_MIN)
				{
					scale = 2.0;
				}
				else if(dist > TURNDIST_MAX)
				{
					scale = 1.0;
				}
				else
				{	// the internal value runs from 0 up close to 1 far away
					scale = 2.0 - (dist - TURNDIST_MIN)/(TURNDIST_MAX - TURNDIST_MIN);
				}

				scale *= the_ai->GetMySkills()->getTurnSpeed();//different guys turn at different speeds
			}
			else
			{	// turn faster the closer the guy is - more severe than for regular player turning
				if(dist < TURNDIST_MIN)
				{
					scale = 3.0;
				}
				else if(dist > TURNDIST_MAX)
				{
					scale = 1.0;
				}
				else
				{	// the internal value runs from 0 up close to 1 far away
					scale = 3.0 - (dist - TURNDIST_MIN)/(TURNDIST_MAX - TURNDIST_MIN) * 2.0;
				}
			}

		}

		VectorScale(monster.intend_avelocity,0.1,monster.intend_avelocity);
		VectorRadiansToDegrees(monster.intend_avelocity, monster.intend_avelocity);
		turninfo.max_turn = currentmove->maxTurn + monster.intend_avelocity[YAW] * scale;
		turninfo.min_turn = currentmove->minTurn + monster.intend_avelocity[YAW] * scale;
//		if (monster.intend_avelocity[YAW] > 1.0 && ai_pathtest->value)
//		{
//			gi.dprintf("screwy turn value:  %3.3f degrees!\n", monster.intend_avelocity[YAW]*RADTODEG);
//		}
		VectorClear(monster.intend_avelocity);
	}
}

int body_c::NextFlags(edict_t &monster)
{
	int returnflags=frame_flags;

	frame_flags=0;
	return returnflags;
}

void body_c::Emote(edict_t &monster, emotion_index new_emotion, float emotion_duration, qboolean scripted_emoting)
{
	//allow overriding of current emotion if: 1) new is higher priority, or 2) current emotion is about to expire anyway
	//--also, make sure I don't interrupt scripted emotions with non-scripted emotions.
//	if (((new_emotion<emotion) && (emotion_expire-level.time>0.25) && (!scripted_emoting || emotion==EMOTION_DEAD))
//		|| (emotionScripted && !scripted_emoting && new_emotion!=EMOTION_DEAD))
	if (/*((emotion_expire-level.time>0.25) && (!scripted_emoting || emotion==EMOTION_DEAD))
		|| */(emotionScripted && !scripted_emoting && new_emotion!=EMOTION_DEAD) ||
		(!scripted_emoting&&(emotion==EMOTION_DEAD||emotion==EMOTION_PAIN)&&(new_emotion!=EMOTION_DEAD)))
	{
		return;
	}
	emotion=new_emotion;
	emotion_expire = level.time+emotion_duration;
	emotionScripted=scripted_emoting;
}

emotion_index body_c::GetMood(edict_t &monster)
{
	return emotion;
}

//fixme: this could probably be better done with an array of ghoulid's, generated upon face setting
void body_c::UpdateFace(edict_t &monster)
{
	if (level.time > emotion_expire)
	{
		emotion=EMOTION_NORMAL;
		emotionScripted=false;
	}
}

qboolean body_c::ApplySkin(edict_t &monster, ggOinstC *myInstance, char *matName, char *skinName)
{
	//fixme? go off myInstance, or get rid of it?
	return monster.ghoulInst->SetFrameOverride(matName, skinName);
}

//yurk, fixme: these arguments are trash! they're worse than delaware!
qboolean body_c::AddRandomBoltedItem(float percent_chance, edict_t &monster, char *myBolt, char *itemClass, char *itemSeq, char *itemBolt, ggOinstC *myInstance, char *itemSkinName, float scale)
{
	if ((gi.flrand(0.0, 1.0) < percent_chance)||(ai_loadAllBolts))
	{
		if(ai_loadAllBolts)
		{
			AddBoltedItem(monster, myBolt, itemClass, itemSeq, itemBolt, myInstance, itemSkinName, scale);
			return false;
		}
		return (NULL != AddBoltedItem(monster, myBolt, itemClass, itemSeq, itemBolt, myInstance, itemSkinName, scale));
	}
	return false;
}

//yurk, fixme: these arguments are trash! they're worse than delaware!
ggBinstC *body_c::AddBoltedItem(edict_t &monster, GhoulID mybolt, ggObjC *gun, char *itemSeq, GhoulID itemBolt, ggOinstC *myInstance, char *itemSkinName, float scale)
{
	GhoulID gunSeq=0;

	if (gun && itemBolt)
	{
		if (itemSkinName && itemSkinName[0])
		{
			gun->RegisterSkin(itemSkinName);
		}

		gun->RegistrationLock();

		gunSeq=game_ghoul.FindObjectSequence(gun,itemSeq);

		if (gunSeq)
		{
			//fixme? just call this off myInstance? for more flexibility, a game_ghoul func could be made that goes off the edict
			return game_ghoul.AddBoltInstance(myInstance, mybolt, gun, itemBolt, gunSeq, itemSkinName, scale);
		}
		else
		{
			return game_ghoul.AddBoltInstance(myInstance, mybolt, gun, itemBolt, itemSkinName, scale);
		}
	}
	return NULL;
}

//yurk, fixme: these arguments are trash! they're worse than delaware!
ggBinstC *body_c::AddBoltedItem(edict_t &monster, GhoulID mybolt, char *itemClass, char *itemSeq, GhoulID itemBolt, ggOinstC *myInstance, char *itemSkinName, float scale)
{
	ggObjC	*gun=NULL;
	char accessoryName[GGHOUL_ID_NAME_LEN];
	
	GhoulID gunSeq=0;

//	sprintf(accessoryName,"simple%s",itemSeq);
	strcpy(accessoryName,itemSeq);

	gun=game_ghoul.FindObject(itemClass,accessoryName,false,itemSkinName);

	if (gun && itemBolt)
	{
		if (itemSkinName && itemSkinName[0])
		{
			gun->RegisterSkin(itemSkinName);
		}

		gun->RegistrationLock();

		gunSeq=game_ghoul.FindObjectSequence(gun,itemSeq);

		if (gunSeq)
		{
			//fixme? just call this off myInstance? for more flexibility, a game_ghoul func could be made that goes off the edict
			return game_ghoul.AddBoltInstance(myInstance, mybolt, gun, itemBolt, gunSeq, itemSkinName, scale);
		}
		else
		{
			return game_ghoul.AddBoltInstance(myInstance, mybolt, gun, itemBolt, itemSkinName, scale);
		}
	}
	return NULL;
}

//yurk, fixme: these arguments are trash! they're worse than delaware!
ggBinstC *body_c::AddBoltedItem(edict_t &monster, GhoulID mybolt, char *itemClass, char *itemSeq, char *itemBolt, ggOinstC *myInstance, char *itemSkinName, float scale)
{
	ggObjC	*gun=NULL;
	GhoulID gunBolt=0;
	char accessoryName[GGHOUL_ID_NAME_LEN];
	
	GhoulID gunSeq=0;

//	sprintf(accessoryName,"simple%s",itemSeq);
	strcpy(accessoryName,itemSeq);

	gun=game_ghoul.FindObject(itemClass,accessoryName,false,itemSkinName);

	if (gun)
	{
		if (itemSkinName && itemSkinName[0])
		{
			gun->RegisterSkin(itemSkinName);
		}

		gun->RegistrationLock();

		gunSeq=game_ghoul.FindObjectSequence(gun,itemSeq);

		if (gunSeq)
		{
			gunBolt=gun->GetMyObject()->FindPart(itemBolt);
			if (gunBolt)
			{
				//fixme? just call this off myInstance? for more flexibility, a game_ghoul func could be made that goes off the edict
				return game_ghoul.AddBoltInstance(myInstance, mybolt, gun, gunBolt, gunSeq, itemSkinName, scale);
			}
		}
		else
		{
			gunBolt=gun->GetMyObject()->FindPart(itemBolt);
			if (gunBolt)
			{
				return game_ghoul.AddBoltInstance(myInstance, mybolt, gun, gunBolt, itemSkinName, scale);
			}
		}
	}
	return NULL;
}
//yurk, fixme: these arguments are trash! they're worse than delaware!
ggBinstC *body_c::AddBoltedItem(edict_t &monster, char *myBolt, char *itemClass, char *itemSeq, char *itemBolt, ggOinstC *myInstance, char *itemSkinName, float scale)
{
	if (monster.ghoulInst&&monster.ghoulInst->GetGhoulObject())
	{
		GhoulID tbolt=monster.ghoulInst->GetGhoulObject()->FindPart(myBolt);
		if(tbolt)
		{
			return AddBoltedItem(monster, tbolt, itemClass, itemSeq, itemBolt, myInstance, itemSkinName, scale);
		}
	}
	return NULL;
}

//this is 'cording to design doc, hope that's ok
float body_c::FireInterval(attacks_e weaponType, int curAmmo)
{
	//just pickin' values that feel good in the game now
	switch(weaponType)
	{
	case ATK_PISTOL2:
		//return gi.flrand(1.125F, 1.375F);
		//return 4.0/9.0;
		return 1.5;
	case ATK_PISTOL1:
		//return gi.flrand(0.75F, 1.25F);
//		return gi.flrand(0.075F, 0.125F);
		//return 5.0/18.0;
		return .8;
	case ATK_SNIPER:
		//return gi.flrand(0.9F, 1.1F);
		return 2.5;
	case ATK_MACHINEGUN:
		//return gi.flrand(0.054F, 0.066F);
		return gi.flrand(0.5F, 1.0F); // this is essentially the pause between bursts //.01;
	case ATK_KNIFE:
		return gi.flrand(1.35F, 1.65F);
	case ATK_FLAMEGUN:
		//return gi.flrand(0.75, 1.25F);
		//return gi.flrand(0.0774F, 0.0946F);
		return .01;
//		return gi.flrand(0.0225F, 0.0275F);
	case ATK_ASSAULTRIFLE:
		//return gi.flrand(0.0774F, 0.0946F);
		return gi.flrand(.1F, .5F); // little bursts for SMG fire.01;
	case ATK_MACHINEPISTOL:
		//return gi.flrand(0.045F, 0.055F);
		return .01;
	case ATK_AUTOSHOTGUN:
		//this will cause it to fire in 3 shot bursts - not accurate for the weapon, but cool for enemies
		if((curAmmo%3) == 1)
		{
			return 2.0;
		}
		else
		{
			return .6;
		}
		return 1.0;//projectiles, so these need to be dodged for funness
	case ATK_SHOTGUN:
		//return gi.flrand(2.25F, 2.75F);
		//return 5.0/8.0;
		//return 4.0;
		return 4.0;
	case ATK_ROCKET:
		//return gi.flrand(2, 3);
		return 4.0;
	case ATK_MICROWAVE:
	case ATK_MICROWAVE_ALT:
		return 6.0;
	case ATK_DEKKER:
		//return gi.flrand(6.25F, 8.75F);
		return 2.5;
	case ATK_NOTHING:
		return 0;
	}
	return 0;
}

weapons_t body_c::GetWeaponType(ggBinstC *weapon)
{
	if (!weapon)
	{
		return SFW_EMPTYSLOT;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_pistol2"))
	{
		return SFW_PISTOL2;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_pistol1"))
	{
		return SFW_PISTOL1;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_sniperrifle"))
	{
		return SFW_SNIPER;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_machinegun"))
	{
		return SFW_MACHINEGUN;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_knife"))
	{
		return SFW_KNIFE;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_bat"))
	{
		return SFW_KNIFE;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_flamethrower"))
	{
		return SFW_FLAMEGUN;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_assault_rifle"))
	{
		return SFW_ASSAULTRIFLE;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_machinepistol"))
	{
		return SFW_MACHINEPISTOL;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_autoshotgun"))
	{
		return SFW_AUTOSHOTGUN;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_kantana"))
	{
		return SFW_KNIFE;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_shotgun"))
	{
		return SFW_SHOTGUN;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_rocket"))
	{
		return SFW_ROCKET;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_mpg"))
	{
		return SFW_MICROWAVEPULSE;
	}
	return SFW_EMPTYSLOT;
}

attacks_e body_c::GetWeaponAttackType(ggBinstC *weapon)
{
	if (!weapon)
	{
		return ATK_NOTHING;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_pistol2"))
	{
		return ATK_PISTOL2;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_pistol1"))
	{
		return ATK_PISTOL1;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_sniperrifle"))
	{
		return ATK_SNIPER;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_machinegun"))
	{
		return ATK_MACHINEGUN;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_knife"))
	{
		return ATK_KNIFE;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_bat"))
	{
		return ATK_KNIFE;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_flamethrower"))
	{
		return ATK_FLAMEGUN;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_assault_rifle"))
	{
		return ATK_ASSAULTRIFLE;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_machinepistol"))
	{
		return ATK_MACHINEPISTOL;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_autoshotgun"))
	{
		return ATK_AUTOSHOTGUN;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_kantana"))
	{
		return ATK_KNIFE;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_shotgun"))
	{
		return ATK_SHOTGUN;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_rocket"))
	{
		return ATK_ROCKET;
	}
	if (!stricmp(weapon->GetBolteeObject()->GetSubName(),"w_mpg"))
	{
		return ATK_MICROWAVE;
	}
	return ATK_NOTHING;
}

qboolean body_c::RightHandWeaponReady(edict_t &monster)
{
	return false;
}

void body_c::FireRightHandWeapon(edict_t &monster, bool isLethal, int NullTarget, qboolean forceFireRate)
{
}

body_c::body_c(body_c *orig)
{
	*(int *)&currentmove = GetMmoveNum(orig->currentmove);
	*(int *)&owner		 = GetEdictNum(orig->owner);
	currentMoveFinished	 = orig->currentMoveFinished; 
	holdCode			 = orig->holdCode;            
	frame_flags			 = orig->frame_flags;         
	emotion_expire		 = orig->emotion_expire;
	emotion				 = orig->emotion;             
	emotionScripted		 = orig->emotionScripted;
	rLastFireTime		 = orig->rLastFireTime;    
	lLastFireTime		 = orig->lLastFireTime;    
	InitialKilledTime	 = orig->InitialKilledTime;
	LastKilledTime		 = orig->LastKilledTime;   
	lastStartTime		 = orig->lastStartTime;    
	LastDFlags			 = orig->LastDFlags;       
	ForceNextAnim		 = orig->ForceNextAnim;    
	TrainDeath			 = orig->TrainDeath;       
}

void body_c::Evaluate(body_c *orig)
{
	currentmove			 = GetMmovePtr((int)orig->currentmove);
	owner				 = GetEdictPtr((int)orig->owner);
	currentMoveFinished	 = orig->currentMoveFinished; 
	holdCode			 = orig->holdCode;            
	frame_flags			 = orig->frame_flags;         
	emotion_expire		 = orig->emotion_expire;
	emotion				 = orig->emotion;             
	emotionScripted		 = orig->emotionScripted;
	rLastFireTime		 = orig->rLastFireTime;    
	lLastFireTime		 = orig->lLastFireTime;    
	InitialKilledTime	 = orig->InitialKilledTime;
	LastKilledTime		 = orig->LastKilledTime;   
	lastStartTime		 = orig->lastStartTime;    
	LastDFlags			 = orig->LastDFlags;       
	ForceNextAnim		 = orig->ForceNextAnim;    
	TrainDeath			 = orig->TrainDeath;       
}

// ----------------------------------------------------------

/// Someone please put this in the right header
void EntToWorldMatrix(vec3_t org, vec3_t angles, Matrix4 &m);
extern Vect3			trStartKnarlyGilHack;

body_heli::body_heli(edict_t* self)
{
	m_gunBarrelInst = NULL;
	m_gunBarrelNULLInst = NULL;
	m_gunnerHeadInst = NULL;
	m_pilotHeadInst = NULL;
	m_cockpitInst = NULL;
	m_mainRotorInst = NULL;
	m_tailRotorInst = NULL;

	m_fuselageObj = NULL;
	m_gunBarrelObj = NULL;
	m_mainRotorObj = NULL;
	m_tailRotorObj = NULL;

	m_gunBolt = 0;
	m_gunNULLBolt = 0;
	m_gunnerHeadBolt = 0;
	m_pilotHeadBolt = 0;

	m_mainRotorBoltInst = NULL;
	m_tailRotorBoltInst = NULL;

	VectorClear(m_vGunBoltOffset);
	VectorClear(m_vGunnerBoltOffset);
	VectorClear(m_vPilotBoltOffset);

	m_bPilotDead = m_bGunnerDead = false;

	VectorClear(m_v3HeliDesiredPos_Start);
	VectorClear(m_v3HeliDesiredPos_Stop);
	VectorClear(m_v3HeliDesiredPos_Halfway);
	VectorClear(m_v3HeliPadRestPos);
	VectorClear(m_v3HeliFireAtCoords);
	VectorClear(m_v3HeliDesiredPos_Smart);
	VectorClear(m_v3HeliHeadStop);
	VectorClear(m_v3HeliTraceMins);
	VectorClear(m_v3HeliTraceMaxs);

	m_fHeliTurnStartYaw = 0;
	m_fHeliMoveSpeed	= 0;
	m_fHeliFireAtTimer = 0;
	m_fHeliFirePitch = 0;
	m_fHeliHoverTimer = 0;
	m_iHeliHealth = 100;
	m_fRotorSoundTimer = 0;

	m_fLastDamageTime = 0.0f;
	m_fHeliRocketFireDebounceTime = 0; 
	m_iHeliRocketsLeft = 8;

	m_nRandGunWait = 0;
	m_bGunFiring = false;
	m_bLOS = false;
	m_bRotorEOS = false;
	m_bFuselageEOS = false;

	// variables passed into script actions
	m_target = NULL;
	VectorClear(m_vPos);
	m_fArg = 0.0f;

	m_flags = 0;

	m_iLastEntFoundIndex=0;
	m_fLastGetEntTime=0;
	m_AITarget=NULL;
	m_fNewTargetTimer=0;
	m_bFiring = 0;

	owner = NULL;
	m_iHeliAITable = -1;
	m_iHeliAIIndex = 0;
	m_fSideStrafeDist = 0.0f;
	m_iSideStrafeDir = 0.0f;
	m_iWhichRandVolume = 0;
	VectorClear(m_v3HeliFireAtCoords_Original);
	m_fHeliAITimer = 0;
	m_nCanopyHealth = 1; // 1 shot to kill the pilot with the sniper rifle
	m_bCanBeSniped = true;
}

//do all inst-related stuff here, rather than in constructor: ghoulInst won't exist when body gets alloc'd
void body_heli::SetHeliEdict(edict_t *self)
{
	if (NULL == self || NULL == self->ghoulInst)
	{
		return;
	}
	m_heli = self;

	GetHeliInst()->TurnCallBacksOn(level.time);
}


int body_heli::ShowDamage(edict_t &monster, edict_t *inflictor, 
						  edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin,
						  int damage, int knockback, int dflags, int mod, float penetrate, float absorb)
{
//	vec3_t		bodyloc;//, footorigin;
//	gz_info		*frontHole=NULL;
//	gz_code		frontCode;
//	gz_info		*backHole=NULL;
//	gz_code		backCode;
//	vec3_t		bloodDir;
//	vec3_t		forward, right, up;
//	vec3_t		to_impact;
//	float		forward_back, right_left, up_down;
// kef	ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);
//	ggOinstC	*blownInst;
	int			i;
	int			take;
//	float		curGoreDist,bestGoreDist,tdist;
//	vec3_t		EntryPos,ExitPos,tvec;
	Vect3		zonePos;
	Matrix4		ZoneMatrix;
	
	Matrix4 ToEnt,ToWorld;
	EntToWorldMatrix(monster.s.origin,monster.s.angles,ToWorld);
	//but we want to put the ray into ent space, need inverse
	ToEnt.Inverse(ToWorld);
	Vect3 EntDir,EntStart;
	VectorCopy(origin,*((vec3_t*)&trStartKnarlyGilHack));
	ToEnt.XFormPoint(EntStart,trStartKnarlyGilHack);
	ToEnt.XFormVect(EntDir,*(Vect3 *)dir);

	take = 0;

	Vect3 vNormal, vImpact, vSparksNormal, vSparksPos;
	
	typedef enum
	{
		hit_NONE = 0,
		hit_COCKPIT,
		hit_PILOT,
		hit_GUNNER,
		hit_FUSELAGE
	} heliHit_e;

	heliHit_e	whatDidIHit = hit_NONE;

// kef	int numHits=myInstance->GetInstPtr()->RayTrace(level.time,EntStart,EntDir,Hits,20);
//	bullet_numHits = monster.ghoulInst->RayTrace(level.time,EntStart,EntDir,bullet_Hits,20);
	//NO HITS!!!
	if (bullet_numHits == 0)
	{
		return 0;
	}

	qboolean bSniper = false;

	if (attacker && attacker->client && attacker->client->inv)
	{
		if ( (SFW_SNIPER == attacker->client->inv->getCurWeaponType()) &&
			 (IsSnipeable()) )
		{
			bSniper = true;
		}
	}

	//get entrance wound
	for (i=0;i<bullet_numHits;i++)
	{
		//hit my instance
/*
		if (bullet_Hits[i].Inst==monster.ghoulInst)
		{
			//hit something
			take = damage;
			vNormal.Set(bullet_Hits[i].Normal[0], bullet_Hits[i].Normal[1], bullet_Hits[i].Normal[2]);
			vImpact.Set(bullet_Hits[i].ContactPoint[0], bullet_Hits[i].ContactPoint[1], bullet_Hits[i].ContactPoint[2]);
			break;
		}
		//hit a bolt-on
		else
*/
		{	// if we hit the pilot or gunner with the sniper rifle, deal with that first. 
			//otherwise, if we hit the cockpit assess the damage normally. if we hit anywhere
			//else, do 1/10 the damage (I'm totally making up that fraction)
			if (bullet_Hits[i].Inst == GetPilotInst())
			{	// we don't want to keep looping through the hits if we get here
				whatDidIHit = hit_PILOT;
				break; // this break is intentional
			}
			else if (bullet_Hits[i].Inst == GetGunnerInst())
			{	// we don't want to keep looping through the hits if we get here
				whatDidIHit = hit_GUNNER;
				break; // this break is intentional
			}
			else if (bullet_Hits[i].Inst == GetCockpitInst())
			{	// if we get here assess the damage normally
				whatDidIHit = hit_COCKPIT;
			}
			else if (whatDidIHit != hit_COCKPIT)
			{	// do 1/10 the damage
				whatDidIHit = hit_FUSELAGE;
			}
		}
	}

	switch(whatDidIHit)
	{
	case hit_PILOT:
		if (bSniper)
		{	
			if ( 0 >= --m_nCanopyHealth )
			{
				// capped the pilot
				m_bPilotDead = true;
				break;
			}
		}
		take = damage;
		break;
	case hit_GUNNER:
		if (bSniper)
		{	
			if ( 0 >= --m_nCanopyHealth )
			{
				// capped the gunner
				m_bGunnerDead = true;
				break;
			}
		}
		take = damage;
		break;
	case hit_COCKPIT:
		take = damage;
		break;
	case hit_FUSELAGE:
		take = (int)(0.1 * damage);
		break;
	}

	if (0 == take)
	{
		take = damage;
		vNormal.Set(bullet_Hits[i].Normal[0], bullet_Hits[i].Normal[1], bullet_Hits[i].Normal[2]);
		vImpact.Set(bullet_Hits[i].ContactPoint[0], bullet_Hits[i].ContactPoint[1], bullet_Hits[i].ContactPoint[2]);
	}

	if (take && (level.time - m_fLastDamageTime) > 0.3f && !bSniper)
	{
		// make sparks at point of impact
		ToWorld.XFormPoint(vSparksPos, vImpact);
		ToWorld.XFormVect(vSparksNormal, vNormal);
		FX_MakeSparks(*(vec3_t*)&vSparksPos, *(vec3_t*)&vNormal, gi.irand(0,2));
		// ricochet sounds are already precached in cacheweaponcommon()
		gi.sound(&monster, CHAN_ENT1, 	gi.soundindex(va("Weapons/FX/Rics/ric%d.wav", gi.irand(1,6))), .9, ATTN_NORM, 0);

//		gi.sound (self, CHAN_ENT1, gi.soundindex("Impact/Surfs/Metal1.wav"), .6, ATTN_NORM, 0);
		m_fLastDamageTime = level.time;
	}
	return take;
}

void body_heli::UpdateSmoke(edict_t* self, float fHealthPercentage)
{
	IGhoulObj	*obj = GetHeliInst()->GetGhoulObject();
	GhoulID		smoke1 = (obj?obj->FindPart("smoke_engine"):NULL);
	GhoulID		smoke2 = (obj?obj->FindPart("smoke_tail"):NULL);

	if (NULL == smoke1 || NULL == smoke2)
	{
		return;
	}
	if (fHealthPercentage < 0)
	{	// turn off the smoke
		fxRunner.stopContinualEffect("environ/helismoke", self, smoke1);
		fxRunner.stopContinualEffect("environ/helismoke", self, smoke2);
	}
	else if (fHealthPercentage < 0.25f)
	{
		fxRunner.editContinualEffect("environ/helismoke", self, smoke1, 2);
		fxRunner.editContinualEffect("environ/helismoke", self, smoke2, 1);
	}
	else if (fHealthPercentage < 0.5f)
	{
		fxRunner.editContinualEffect("environ/helismoke", self, smoke1, 1.5);
		fxRunner.editContinualEffect("environ/helismoke", self, smoke2, .75);
	}
	else if (fHealthPercentage < 0.75f)
	{
		if (!fxRunner.hasEffect("environ/helismoke", self, smoke1))
		{
			// prepare smoke effects for damage
			smoke2 = obj->FindPart("smoke_tail");

			fxRunner.execContinualEffect("environ/helismoke", self, smoke1);
			fxRunner.execContinualEffect("environ/helismoke", self, smoke2);
		}
		fxRunner.editContinualEffect("environ/helismoke", self, smoke1, 1);
		fxRunner.editContinualEffect("environ/helismoke", self, smoke2, 0.5);
	}
}

void body_heli::ChangeCockpitSkin(int nSkin)
{
	IGhoulObj* cockpitObj = GetCockpitInst()->GetGhoulObject();

	if (NULL == cockpitObj)
	{
		return;
	}

	GhoulID matID = cockpitObj->FindMaterial("canopy");
	GhoulID skinID = 0;

	switch(nSkin)
	{
	case HC_SKIN_GUNNER:
		{
			skinID = cockpitObj->FindSkin(matID, "glass_blood_fnt");
			break;
		}
	case HC_SKIN_PILOT:
		{
			skinID = cockpitObj->FindSkin(matID, "glass_blood_bk");
			break;
		}
	case HC_SKIN_BOTH:
		{
			skinID = cockpitObj->FindSkin(matID, "glass_bloody");
			break;
		}
	default:
		{
			break;
		}
	}
	GetCockpitInst()->SetFrameOverride(matID, skinID);
}

void body_heli::SetFiring(bool bFiring)
{
	if (m_bFiring = bFiring)
	{
		// turn on firing sequence
		if (GetBarrelInst())
		{
			SimpleModelSetSequence2(GetBarrelInst(), "barrel", SMSEQ_LOOP);
		}
	}
	else
	{
		// turn off firing sequence
		if (GetBarrelInst())
		{
			SimpleModelSetSequence2(GetBarrelInst(), "barrel_still", SMSEQ_HOLD);
		}
	}
}

body_heli::body_heli(body_heli *orig)
: body_c(orig)
{
	*(int *)&m_heli	= GetEdictNum(orig->m_heli);

	m_gunBarrelInst.MakeIndex(orig->m_gunBarrelInst);
	m_gunBarrelNULLInst.MakeIndex(orig->m_gunBarrelNULLInst);
	m_gunnerHeadInst.MakeIndex(orig->m_gunnerHeadInst);
	m_pilotHeadInst.MakeIndex(orig->m_pilotHeadInst);
	m_cockpitInst.MakeIndex(orig->m_cockpitInst);
	m_mainRotorInst.MakeIndex(orig->m_mainRotorInst);
	m_tailRotorInst.MakeIndex(orig->m_tailRotorInst);

	m_fuselageObj.MakeIndex(orig->m_fuselageObj); 
	m_gunBarrelObj.MakeIndex(orig->m_gunBarrelObj);
	m_mainRotorObj.MakeIndex(orig->m_mainRotorObj);
	m_tailRotorObj.MakeIndex(orig->m_tailRotorObj);

	m_gunBolt = orig->m_gunBolt;
	m_gunNULLBolt = orig->m_gunNULLBolt;   
	m_gunnerHeadBolt = orig->m_gunnerHeadBolt;
	m_pilotHeadBolt	= orig->m_pilotHeadBolt; 

	m_mainRotorBoltInst.MakeIndex(orig->m_mainRotorBoltInst);
	m_tailRotorBoltInst.MakeIndex(orig->m_tailRotorBoltInst);

	VectorCopy(orig->m_vGunBoltOffset, m_vGunBoltOffset);   
	VectorCopy(orig->m_vGunnerBoltOffset, m_vGunnerBoltOffset);
	VectorCopy(orig->m_vPilotBoltOffset, m_vPilotBoltOffset); 

	m_nCanopyHealth = orig->m_nCanopyHealth;
	m_bPilotDead = orig->m_bPilotDead;
	m_bGunnerDead = orig->m_bGunnerDead;
	m_bFiring = orig->m_bFiring;

	VectorCopy(orig->m_v3HeliDesiredPos_Start,m_v3HeliDesiredPos_Start);  
	VectorCopy(orig->m_v3HeliDesiredPos_Stop,m_v3HeliDesiredPos_Stop);   
	VectorCopy(orig->m_v3HeliDesiredPos_Halfway,m_v3HeliDesiredPos_Halfway);
	VectorCopy(orig->m_v3HeliPadRestPos, m_v3HeliPadRestPos);        
	VectorCopy(orig->m_v3HeliFireAtCoords, m_v3HeliFireAtCoords);      
	VectorCopy(orig->m_v3HeliDesiredPos_Smart, m_v3HeliDesiredPos_Smart);  
	VectorCopy(orig->m_v3HeliHeadStop, m_v3HeliHeadStop);          
	VectorCopy(orig->m_v3HeliTraceMins, m_v3HeliTraceMins);         
	VectorCopy(orig->m_v3HeliTraceMaxs, m_v3HeliTraceMaxs);         
	VectorCopy(orig->m_v3HeliDeathDest, m_v3HeliDeathDest);         

	m_fHeliTurnStartYaw = orig->m_fHeliTurnStartYaw;
	m_fHeliMoveSpeed = orig->m_fHeliMoveSpeed;   
	m_fHeliFireAtTimer = orig->m_fHeliFireAtTimer; 
	m_fHeliFirePitch = orig->m_fHeliFirePitch;   
	m_fHeliHoverTimer = orig->m_fHeliHoverTimer;  
	m_iHeliHealth = orig->m_iHeliHealth;      
	m_fRotorSoundTimer = orig->m_fRotorSoundTimer; 
	VectorCopy(orig->m_v3HeliFireAtCoords_Original, m_v3HeliFireAtCoords_Original);	
	m_fHeliAITimer = orig->m_fHeliAITimer;
	m_iHeliAITable = orig->m_iHeliAITable;
	m_iHeliAIIndex = orig->m_iHeliAIIndex;

	m_fLastDamageTime = orig->m_fLastDamageTime;
	m_fHeliRocketFireDebounceTime = orig->m_fHeliRocketFireDebounceTime;
	m_iHeliRocketsLeft = orig->m_iHeliRocketsLeft;

	m_nRandGunWait = orig->m_nRandGunWait;
	m_bGunFiring = orig->m_bGunFiring;
	m_bLOS = orig->m_bLOS;
	m_bRotorEOS = orig->m_bRotorEOS;
	m_bFuselageEOS = orig->m_bFuselageEOS;

	*(int *)&m_target = GetEdictNum(orig->m_target);
	VectorCopy(orig->m_vPos, m_vPos);
	m_fArg = orig->m_fArg;

	m_flags = orig->m_flags;

	m_iLastEntFoundIndex = orig->m_iLastEntFoundIndex;
	m_fLastGetEntTime = orig->m_fLastGetEntTime;	
	*(int *)&m_AITarget = GetEdictNum(orig->m_AITarget);
	m_fNewTargetTimer = orig->m_fNewTargetTimer;
	m_fSideStrafeDist = orig->m_fSideStrafeDist;
	m_iSideStrafeDir = orig->m_iSideStrafeDir;
	m_iWhichRandVolume = orig->m_iWhichRandVolume;
	m_fAIAggressiveness = orig->m_fAIAggressiveness;

	m_bCanBeSniped = orig->m_bCanBeSniped;
}

void body_heli::Evaluate(body_heli *orig)
{
	m_heli = GetEdictPtr((int)orig->m_heli);

	m_gunBarrelInst.MakePtr(*(int *)&orig->m_gunBarrelInst);
	m_gunBarrelNULLInst.MakePtr(*(int *)&orig->m_gunBarrelNULLInst);
	m_gunnerHeadInst.MakePtr(*(int *)&orig->m_gunnerHeadInst);
	m_pilotHeadInst.MakePtr(*(int *)&orig->m_pilotHeadInst);
	m_cockpitInst.MakePtr(*(int *)&orig->m_cockpitInst);
	m_mainRotorInst.MakePtr(*(int *)&orig->m_mainRotorInst);
	m_tailRotorInst.MakePtr(*(int *)&orig->m_tailRotorInst);

	m_fuselageObj.MakePtr(*(int *)&orig->m_fuselageObj); 
	m_gunBarrelObj.MakePtr(*(int *)&orig->m_gunBarrelObj);
	m_mainRotorObj.MakePtr(*(int *)&orig->m_mainRotorObj);
	m_tailRotorObj.MakePtr(*(int *)&orig->m_tailRotorObj);

	m_gunBolt = orig->m_gunBolt;
	m_gunNULLBolt = orig->m_gunNULLBolt;   
	m_gunnerHeadBolt = orig->m_gunnerHeadBolt;
	m_pilotHeadBolt	= orig->m_pilotHeadBolt; 

	m_mainRotorBoltInst.MakePtr(*(int *)&orig->m_mainRotorBoltInst);
	m_tailRotorBoltInst.MakePtr(*(int *)&orig->m_tailRotorBoltInst);

	VectorCopy(orig->m_vGunBoltOffset, m_vGunBoltOffset);   
	VectorCopy(orig->m_vGunnerBoltOffset, m_vGunnerBoltOffset);
	VectorCopy(orig->m_vPilotBoltOffset, m_vPilotBoltOffset); 

	m_nCanopyHealth = orig->m_nCanopyHealth;
	m_bPilotDead = orig->m_bPilotDead;
	m_bGunnerDead = orig->m_bGunnerDead;
	m_bFiring = orig->m_bFiring;

	VectorCopy(orig->m_v3HeliDesiredPos_Start,m_v3HeliDesiredPos_Start);  
	VectorCopy(orig->m_v3HeliDesiredPos_Stop,m_v3HeliDesiredPos_Stop);   
	VectorCopy(orig->m_v3HeliDesiredPos_Halfway,m_v3HeliDesiredPos_Halfway);
	VectorCopy(orig->m_v3HeliPadRestPos, m_v3HeliPadRestPos);        
	VectorCopy(orig->m_v3HeliFireAtCoords, m_v3HeliFireAtCoords);      
	VectorCopy(orig->m_v3HeliDesiredPos_Smart, m_v3HeliDesiredPos_Smart);  
	VectorCopy(orig->m_v3HeliHeadStop, m_v3HeliHeadStop);          
	VectorCopy(orig->m_v3HeliTraceMins, m_v3HeliTraceMins);         
	VectorCopy(orig->m_v3HeliTraceMaxs, m_v3HeliTraceMaxs);         
	VectorCopy(orig->m_v3HeliDeathDest, m_v3HeliDeathDest);         

	m_fHeliTurnStartYaw = orig->m_fHeliTurnStartYaw;
	m_fHeliMoveSpeed = orig->m_fHeliMoveSpeed;   
	m_fHeliFireAtTimer = orig->m_fHeliFireAtTimer; 
	m_fHeliFirePitch = orig->m_fHeliFirePitch;   
	m_fHeliHoverTimer = orig->m_fHeliHoverTimer;  
	m_iHeliHealth = orig->m_iHeliHealth;      
	m_fRotorSoundTimer = orig->m_fRotorSoundTimer; 
	VectorCopy(orig->m_v3HeliFireAtCoords_Original, m_v3HeliFireAtCoords_Original);	
	m_fHeliAITimer = orig->m_fHeliAITimer;
	m_iHeliAITable = orig->m_iHeliAITable;
	m_iHeliAIIndex = orig->m_iHeliAIIndex;

	m_fLastDamageTime = orig->m_fLastDamageTime;
	m_fHeliRocketFireDebounceTime = orig->m_fHeliRocketFireDebounceTime;
	m_iHeliRocketsLeft = orig->m_iHeliRocketsLeft;

	m_nRandGunWait = orig->m_nRandGunWait;
	m_bGunFiring = orig->m_bGunFiring;
	m_bLOS = orig->m_bLOS;
	m_bRotorEOS = orig->m_bRotorEOS;
	m_bFuselageEOS = orig->m_bFuselageEOS;

	m_target = GetEdictPtr((int)orig->m_target);
	VectorCopy(orig->m_vPos, m_vPos);
	m_fArg = orig->m_fArg;

	m_flags = orig->m_flags;

	m_iLastEntFoundIndex = orig->m_iLastEntFoundIndex;
	m_fLastGetEntTime = orig->m_fLastGetEntTime;	
	m_AITarget = GetEdictPtr((int)orig->m_AITarget);
	m_fNewTargetTimer = orig->m_fNewTargetTimer;
	m_fSideStrafeDist = orig->m_fSideStrafeDist;
	m_iSideStrafeDir = orig->m_iSideStrafeDir;
	m_iWhichRandVolume = orig->m_iWhichRandVolume;
	m_fAIAggressiveness = orig->m_fAIAggressiveness;

	m_bCanBeSniped = orig->m_bCanBeSniped;
	body_c::Evaluate(orig);
}

void body_heli::Read()
{
	char	loaded[sizeof(body_heli)];

	gi.ReadFromSavegame('HELI', loaded, sizeof(body_heli));
	Evaluate((body_heli *)loaded);
}

void body_heli::Write()
{
	body_heli	*savable;

	savable = new body_heli(this);
	gi.AppendToSavegame('HELI', savable, sizeof(*this));
	delete savable;
}

/////////////////////////////////////////////////////////////////////
//																   //
//                       tank stuff								   //
//																   //
/////////////////////////////////////////////////////////////////////

body_tank::body_tank(edict_t* self)
{
	m_target = NULL;
	VectorClear(m_vPos);
	m_fArg = 0.0f;

	m_tank = NULL;			// the tank body is the root object (hopefully)
	m_turretInst = NULL;		// the turret
	m_cannonInst = NULL;		// the barrel of the main gun
	m_machGunInst = NULL;		// the machine gun on the turret
	m_machGunNullInst = NULL;	// goes between machine gun and the turret

	m_tankObj = NULL;
	m_turretObj = NULL;

	m_turretBolt = NULL_GhoulID;
	m_cannonBolt = NULL_GhoulID;
	m_machGunBolt = NULL_GhoulID;
	m_machGunNullBolt = NULL_GhoulID;

	m_turretBoltInst = NULL;
	m_bDeactivated = false;
	m_bTreads = false;
	m_bTreadFire = false;
	m_fLastDamageTime = 0.0f;
	m_fMaxYawSpeed = 20.0f; // degrees
	m_fMaxFwdSpeed = 50.0f;
	m_fTurretSpeed = 0.04f; // radians
	m_fCannonSpeed = 0.03f; // radians
	m_fMachGunSpeed = 0.05f; // radians
	m_fShellFlightDist = 0.0f; // world units
	m_nMachGunHealth = 150;
	VectorClear(m_vGroundZero);
}

//do all inst-related stuff here, rather than in constructor: ghoulInst won't exist when body gets alloc'd
void body_tank::SetTankEdict(edict_t *self)
{
	if (NULL == self || NULL == self->ghoulInst)
	{
		return;
	}
	m_tank = self;
	GetTankInst()->TurnCallBacksOn(level.time);
}

body_tank::body_tank(body_tank *orig)
: body_c(orig)
{
	*(int *)&m_target = GetEdictNum(orig->m_target);
	VectorCopy(orig->m_vPos, m_vPos);
	m_fArg = orig->m_fArg;

	*(int *)&m_tank = GetEdictNum(orig->m_tank);

	m_turretInst.MakeIndex(orig->m_turretInst);
	m_cannonInst.MakeIndex(orig->m_cannonInst);
	m_machGunInst.MakeIndex(orig->m_machGunInst);
	m_machGunNullInst.MakeIndex(orig->m_machGunNullInst);

	m_tankObj.MakeIndex(orig->m_tankObj);
	m_turretObj.MakeIndex(orig->m_turretObj);

	m_turretBolt = orig->m_turretBolt;
	m_cannonBolt = orig->m_cannonBolt;
	m_machGunBolt = orig->m_machGunBolt;
	m_machGunNullBolt = orig->m_machGunNullBolt;

	m_turretBoltInst.MakeIndex(orig->m_turretBoltInst);

	m_bDeactivated = orig->m_bDeactivated;
	m_bTreads = orig->m_bTreads;
	m_bTreadFire = orig->m_bTreadFire;
	m_fLastDamageTime = orig->m_fLastDamageTime;

	m_fMaxYawSpeed = orig->m_fMaxYawSpeed;
	m_fMaxFwdSpeed = orig->m_fMaxFwdSpeed;
	m_fTurretSpeed = orig->m_fTurretSpeed;
	m_fCannonSpeed = orig->m_fCannonSpeed;
	m_fMachGunSpeed = orig->m_fMachGunSpeed;

	m_fShellFlightDist = orig->m_fShellFlightDist;
	m_nMachGunHealth = orig->m_nMachGunHealth;
	VectorCopy(orig->m_vGroundZero, m_vGroundZero);
}

void body_tank::Evaluate(body_tank *orig)
{
	m_target = GetEdictPtr((int)orig->m_target);
	VectorCopy(orig->m_vPos, m_vPos);
	m_fArg = orig->m_fArg;

	m_tank = GetEdictPtr((int)orig->m_tank);

	m_turretInst.MakePtr(*(int *)&orig->m_turretInst);
	m_cannonInst.MakePtr(*(int *)&orig->m_cannonInst);
	m_machGunInst.MakePtr(*(int *)&orig->m_machGunInst);
	m_machGunNullInst.MakePtr(*(int *)&orig->m_machGunNullInst);

	m_tankObj.MakePtr(*(int *)&orig->m_tankObj);
	m_turretObj.MakePtr(*(int *)&orig->m_turretObj);

	m_turretBolt = orig->m_turretBolt;
	m_cannonBolt = orig->m_cannonBolt;
	m_machGunBolt = orig->m_machGunBolt;
	m_machGunNullBolt = orig->m_machGunNullBolt;

	m_turretBoltInst.MakePtr(*(int *)&orig->m_turretBoltInst);

	m_bDeactivated = orig->m_bDeactivated;
	m_bTreads = orig->m_bTreads;
	m_bTreadFire = orig->m_bTreadFire;
	m_fLastDamageTime = orig->m_fLastDamageTime;

	m_fMaxYawSpeed = orig->m_fMaxYawSpeed;
	m_fMaxFwdSpeed = orig->m_fMaxFwdSpeed;
	m_fTurretSpeed = orig->m_fTurretSpeed;
	m_fCannonSpeed = orig->m_fCannonSpeed;
	m_fMachGunSpeed = orig->m_fMachGunSpeed;

	m_fShellFlightDist = orig->m_fShellFlightDist;
	m_nMachGunHealth = orig->m_nMachGunHealth;
	VectorCopy(orig->m_vGroundZero, m_vGroundZero);

	body_c::Evaluate(orig);
}

void body_tank::Read()
{
	char	loaded[sizeof(body_tank)];

	gi.ReadFromSavegame('TANK', loaded, sizeof(body_tank));
	Evaluate((body_tank *)loaded);
}

void body_tank::Write()
{
	body_tank	*savable;

	savable = new body_tank(this);
	gi.AppendToSavegame('TANK', savable, sizeof(*this));
	delete savable;
}

void body_tank::SetTreads(bool bOn)
{
	if (m_bTreads = bOn)
	{ // set the treads in motion
		SimpleModelSetSequence2(GetTankInst(), "tank", SMSEQ_LOOP);
	}
	else
	{ // stop the treads
		SimpleModelSetSequence2(GetTankInst(), "tank", SMSEQ_HOLD);
	}
}

int body_tank::ShowDamage(edict_t &monster, edict_t *inflictor, 
						  edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin,
						  int damage, int knockback, int dflags, int mod, float penetrate, float absorb)
{
	int			i;
	int			take;
	Vect3		zonePos;
	Matrix4		ZoneMatrix;
	
	Matrix4 ToEnt,ToWorld;
	EntToWorldMatrix(monster.s.origin,monster.s.angles,ToWorld);
	//but we want to put the ray into ent space, need inverse
	ToEnt.Inverse(ToWorld);
	Vect3 EntDir,EntStart;
	VectorCopy(origin,*((vec3_t*)&trStartKnarlyGilHack));
	ToEnt.XFormPoint(EntStart,trStartKnarlyGilHack);
	ToEnt.XFormVect(EntDir,*(Vect3 *)dir);

	take = 0;

	Vect3 vNormal, vImpact, vSparksNormal, vSparksPos;
	
	//NO HITS!!!
	if (bullet_numHits == 0)
	{
		return 0;
	}

	qboolean bSniper = false;

	if (attacker && attacker->client && attacker->client->inv)
	{
		// knives can't hurt tanks, silly.
		if (SFW_KNIFE == attacker->client->inv->getCurWeaponType())
		{
			return 0;
		}
		if (SFW_SNIPER == attacker->client->inv->getCurWeaponType())
		{
			bSniper = true;
		}
	}

	//get entrance wound
	for (i=0;i<bullet_numHits;i++)
	{
		//hit my instance
		if (bullet_Hits[i].Inst==monster.ghoulInst)
		{
			//hit something
			if (true /*bullet_Hits[i].Mesh*/)
			{
				take = damage;
				vNormal.Set(bullet_Hits[i].Normal[0], bullet_Hits[i].Normal[1], bullet_Hits[i].Normal[2]);
				vImpact.Set(bullet_Hits[i].ContactPoint[0], bullet_Hits[i].ContactPoint[1], bullet_Hits[i].ContactPoint[2]);
				break;
			}
		}
		//hit a bolt-on
		else
		{
			if (bullet_Hits[i].Inst == GetMachGunInst() && m_nMachGunHealth > 0)
			{
				m_nMachGunHealth -= damage;
				if (m_nMachGunHealth <= 0)
				{
					m_nMachGunHealth = 0;
					// killed the machine gun -- attach a smoke effect
					IGhoulObj *obj = bullet_Hits[i].Inst->GetGhoulObject();
					if (obj)
					{
						fxRunner.execWithInst("environ/machgun_smoke2", GetTankEdict(), 
							bullet_Hits[i].Inst, obj->FindPart("to_bolt_machine_gun"));
					}
				}
			}
			if (0 == take)
			{
				take = damage;
				vNormal.Set(bullet_Hits[i].Normal[0], bullet_Hits[i].Normal[1], bullet_Hits[i].Normal[2]);
				vImpact.Set(bullet_Hits[i].ContactPoint[0], bullet_Hits[i].ContactPoint[1], bullet_Hits[i].ContactPoint[2]);
			}
		}
	}

	if (take && (level.time - m_fLastDamageTime) > 0.3f && !bSniper)
	{
		// make sparks at point of impact
		ToWorld.XFormPoint(vSparksPos, vImpact);
		ToWorld.XFormVect(vSparksNormal, vNormal);
		FX_MakeSparks(*(vec3_t*)&vSparksPos, *(vec3_t*)&vNormal, gi.irand(0,2));
		m_fLastDamageTime = level.time;
	}
	return take;
}

void body_tank::UpdateSmoke(edict_t* self, float fHealthPercentage)
{
	// presumably we'll eventually want a visible indication of damage (like the heli)
}


/////////////////////////////////////////////////////////////////////
//																   //
//                    snowcat stuff								   //
//																   //
/////////////////////////////////////////////////////////////////////

body_snowcat::body_snowcat(edict_t* self)
:body_tank(self)
{
	m_machGun2Inst = NULL;		// the second machine gun on the turret
	m_leftHeadLightInst = NULL;
	m_rightHeadLightInst = NULL;

	m_machGun2Bolt = NULL_GhoulID;
	m_leftHeadLightBolt = NULL_GhoulID;
	m_rightHeadLightBolt = NULL_GhoulID;

	m_fMaxYawSpeed = 30.0f; // degrees
	m_fMaxFwdSpeed = 70.0f;
	m_fTurretSpeed = 0.06f+(game.playerSkills.getEnemyValue()*.01); // radians
	m_fCannonSpeed = 0.00f; // radians (no cannon for the snowcat)
	m_fMachGunSpeed = 0.00f; // radians (no separate mach gun for the snowcat)
	m_fShellFlightDist = 0.0f; // world units
	m_nMachGunHealth = 150;
}

body_snowcat::body_snowcat(body_snowcat *orig)
: body_tank(orig)
{
	m_machGun2Inst.MakeIndex(orig->m_machGun2Inst);
	m_leftHeadLightInst.MakeIndex(orig->m_leftHeadLightInst);
	m_rightHeadLightInst.MakeIndex(orig->m_rightHeadLightInst);

	m_machGun2Bolt = orig->m_machGun2Bolt;
	m_leftHeadLightBolt = orig->m_leftHeadLightBolt;
	m_rightHeadLightBolt = orig->m_rightHeadLightBolt;
}

void body_snowcat::SetTreads(bool bOn)
{
	GhoulID		cBolterBolt=0;
	IGhoulObj	*boltObj = NULL;

	if (m_bTreads = bOn)
	{ // set the treads in motion
		SimpleModelSetSequence2(GetTankInst(), "body", SMSEQ_LOOP);
		// turn on snowspray effect
		if ( m_tank->ghoulInst && (boltObj = m_tank->ghoulInst->GetGhoulObject()) )
		{
			// left 
			if (cBolterBolt = boltObj->FindPart("snow_effect_left")) 
			{
				fxRunner.execContinualEffect("environ/snowspray", m_tank, cBolterBolt);
			}
			// right 
			if (cBolterBolt = boltObj->FindPart("snow_effect_right")) 
			{
				fxRunner.execContinualEffect("environ/snowspray", m_tank, cBolterBolt);
			}
		}
	}
	else
	{ // stop the treads
		SimpleModelSetSequence2(GetTankInst(), "body", SMSEQ_HOLD);
		// turn off snowspray effect
		if ( m_tank->ghoulInst && (boltObj = m_tank->ghoulInst->GetGhoulObject()) )
		{
			// left 
			if (cBolterBolt = boltObj->FindPart("snow_effect_left"))
			{
				fxRunner.stopContinualEffect("environ/snowspray", m_tank, cBolterBolt);
			}
			// right 
			if (cBolterBolt = boltObj->FindPart("snow_effect_right"))
			{
				fxRunner.stopContinualEffect("environ/snowspray", m_tank, cBolterBolt);
			}
		}
	}
}

void body_snowcat::Evaluate(body_snowcat *orig)
{
	m_machGun2Inst.MakePtr(*(int *)&orig->m_machGun2Inst);
	m_leftHeadLightInst.MakePtr(*(int *)&orig->m_leftHeadLightInst);
	m_rightHeadLightInst.MakePtr(*(int *)&orig->m_rightHeadLightInst);

	m_machGun2Bolt = orig->m_machGun2Bolt;
	m_leftHeadLightBolt = orig->m_leftHeadLightBolt;
	m_rightHeadLightBolt = orig->m_rightHeadLightBolt;
	
	// gotta have this at the end of body_snowcat::Evaluate() cuz it calls
	//body_c::Evaluate()
	body_tank::Evaluate(orig);
}

void body_snowcat::Read()
{
	char	loaded[sizeof(body_snowcat)];

	gi.ReadFromSavegame('SCAT', loaded, sizeof(body_snowcat));
	Evaluate((body_snowcat *)loaded);
}

void body_snowcat::Write()
{
	body_snowcat	*savable;

	savable = new body_snowcat(this);
	gi.AppendToSavegame('SCAT', savable, sizeof(*this));
	delete savable;
}

void body_snowcat::UpdateSmoke(edict_t* self, float fHealthPercentage)
{
	// presumably we'll eventually want a visible indication of damage (like the heli)
}

void body_snowcat::Deactivate(bool bDeactivate)
{
	GhoulID		cBolterBolt=0;
	IGhoulObj	*boltObj = NULL;

	if (m_bDeactivated = bDeactivate)
	{	// we're deactive so turn off headlights and exhaust

		// turn off beams
		SimpleModelTurnOnOff(GetLeftHeadLightInst(), false);
		SimpleModelTurnOnOff(GetRightHeadLightInst(), false);
		// headlights
		SimpleModelRemoveObject(m_tank, "front_left");
		SimpleModelRemoveObject(m_tank, "front_right");
		// taillights
		SimpleModelRemoveObject(m_tank, "rear_left");
		SimpleModelRemoveObject(m_tank, "rear_right");
		// exhaust effects
		if (m_tank->ghoulInst && (boltObj = m_tank->ghoulInst->GetGhoulObject()) )
		{
			// left 
			if (cBolterBolt = boltObj->FindPart("exhuast_left")) // love Joe's spelling of exhaust
			{
				fxRunner.stopContinualEffect("environ/kf_exhaust01", m_tank, cBolterBolt);
			}
			// right 
			if (cBolterBolt = boltObj->FindPart("exhuast_right")) // love Joe's spelling of exhaust
			{
				fxRunner.stopContinualEffect("environ/kf_exhaust01", m_tank, cBolterBolt);
			}
		}
	}
	else
	{	// we're live so turn on headlights and exhaust

		// turn on beams
		SimpleModelTurnOnOff(GetLeftHeadLightInst(), true);
		SimpleModelTurnOnOff(GetRightHeadLightInst(), true);
		// headlights
		SimpleModelAddObject(m_tank, "front_left");
		SimpleModelAddObject(m_tank, "front_right");
		// taillights
		SimpleModelAddObject(m_tank, "rear_left");
		SimpleModelAddObject(m_tank, "rear_right");
		// exhaust effects
		if (m_tank->ghoulInst && (boltObj = m_tank->ghoulInst->GetGhoulObject()) )
		{
			// left 
			if (cBolterBolt = boltObj->FindPart("exhuast_left")) // love Joe's spelling of exhaust
			{
				fxRunner.execContinualEffect("environ/kf_exhaust01", m_tank, cBolterBolt, 0);
			}
			// right 
			if (cBolterBolt = boltObj->FindPart("exhuast_right")) // love Joe's spelling of exhaust
			{
				fxRunner.execContinualEffect("environ/kf_exhaust01", m_tank, cBolterBolt, 0);
			}
		}
	}
}
