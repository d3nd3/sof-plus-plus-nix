#include "g_local.h"
#include "ai_private.h"
#include "ds.h"
#include "ai_pathfinding.h"

#define PATH_FAILURE_ACQUIRE_DELAY	1.0//time i'll wait after i hit a bad path face until i look for path again
#define PATH_FAILURE_REFAIL_DELAY	1.0//time i'll wait after i hit a bad path face until i consider re-initiating badpath procedures

#define PATH_DECISION_LOSETARGETTIME .5//time it'll take me after losing track of target to give up on attack
#define POINTCOMBAT_DECISION_LOSETARGETTIME 15.0//time it'll take me after losing track of target to abondon combat point


#if _DEBUG
extern char *decName;
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//			Brush-Using Helper Functions--currently assuming doors, but should be fleshed out for elevators, etc.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Use_DoorTrigger(edict_t *self, edict_t *other, edict_t *activator);
void button_use (edict_t *self, edict_t *other, edict_t *activator);
void plDoorUse(edict_t *self, edict_t *other, edict_t *activator);
void door_secret_use (edict_t *self, edict_t *other, edict_t *activator);
void useable_use (edict_t *self,edict_t *other,edict_t *activator);
void Use_Multi(edict_t *self, edict_t *other, edict_t *activator);

qboolean IsUseableBrush(edict_t *what)
{
	if (!what)
	{
		return false;
	}
	if (what->plUse == Use_DoorTrigger || what->plUse == button_use || what->plUse == plDoorUse ||
		what->plUse == door_secret_use || what->plUse == useable_use || what->plUse == Use_Multi)
	{
		return true;
	}
	return false;
}

//icky!!!!!!!!!! this is defined elsewhere!
#define	STATE_TOP			0
#define	STATE_BOTTOM		1
#define STATE_UP			2
#define STATE_DOWN			3

#define DOOR_NOMONSTER		0x0008

qboolean NotBadDoor(edict_t *what, float when, edict_t *by_whom)
{
	if(!what)
	{
		return true;
	}
	if (!IsUseableBrush(what))
	{
		return true;
	}
	if(!what->plUse)
	{
		return true;
	}
	if(!(what->spawnflags & DOOR_NOMONSTER))
	{
		return true;
	}

	return false;
}

qboolean IsBrushUseableNow(edict_t *what, float when, edict_t *by_whom)
{
	if (!IsUseableBrush(what))
	{
		return false;
	}
	//don't use a door that's in motion, or already open.
	if (what->plUse == plDoorUse && what->moveinfo.state != STATE_BOTTOM)
	{
		return false;
	}
	if (what->plUse == plDoorUse && (what->spawnflags & DOOR_NOMONSTER))
	{
		return false;  //no good
	}
	if (what->last_move_time + 5.0 < level.time)
	{
		return true;
	}
	return false;
}

//returns whether or not the useInfo fields were set successfully
qboolean GetUseableBrushInfo(vec3_t normal, edict_t *what, brushUseInfo_t &useInfo, edict_t *by_whom)
{
	vec3_t forward;

	if (normal)
	{
		VectorCopy(normal, forward);
		VectorInverse(forward);
	}
	else
	{
		AngleVectors(by_whom->s.angles, forward, NULL, NULL);
	}

	VectorCopy(by_whom->s.origin, useInfo.usePos);
	VectorCopy(forward, useInfo.useDir);
	useInfo.useType=USETYPE_MISC;
	useInfo.useEnt = what;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

decision_c *decision_c::NewClassForCode(int code)
{
	switch (code)
	{
	case BASE_DECISION:
		return new base_decision();
	case PURSUE_DECISION:
		return new pursue_decision();
	case POINTCOMBAT_DECISION:
		return new pointcombat_decision();
	case SEARCH_DECISION:
		return new search_decision();
	case DODGE_DECISION:
		return new dodge_decision();
	case RETREAT_DECISION:
		return new retreat_decision();
	case PATHIDLE_DECISION:
		return new pathidle_decision();
	case PATHCOMBAT_DECISION:
		return new pathcombat_decision();
	case SCRIPTED_DECISION:
		return new scripted_decision();
	case DEKKER1_DECISION:
		return new dekker1_decision();
	case DEKKER2_DECISION:
		return new dekker2_decision();
	case DEKKER3_DECISION:
		return new dekker3_decision();
	case DEKKER4_DECISION:
		return new dekker4_decision();
	case DECISION:
		gi.dprintf("ERROR: only allowed to use leaves of the decision class tree\n");
		break;
	default:
		gi.dprintf("ERROR: invalid decision class code: %d\n", code);
		break;
	}
	return new decision_c();
}

decision_c::decision_c(decision_c *orig)
{
	last_dilution_time = orig->last_dilution_time;
	priority		   = orig->priority;
	priority_base	   = orig->priority_base;     
	validity		   = orig->validity;          
	timeout_time	   = orig->timeout_time;      
	allow_timeout	   = orig->allow_timeout;     
	lastClearTime	   = orig->lastClearTime;     
	isClear			   = orig->isClear;           
}

void decision_c::Evaluate(decision_c *orig)
{
	last_dilution_time = orig->last_dilution_time;
	priority		   = orig->priority;
	priority_base	   = orig->priority_base;     
	validity		   = orig->validity;          
	timeout_time	   = orig->timeout_time;      
	allow_timeout	   = orig->allow_timeout;     
	lastClearTime	   = orig->lastClearTime;     
	isClear			   = orig->isClear;           
}

decision_c::decision_c(int priority_root, float timeout, edict_t* ScriptOwner)
{
	priority = 0;
	priority_base = priority_root;
	validity = 0.5;
	last_dilution_time = level.time;

	//nathan addedness
	lastClearTime = level.time;
	isClear = false;

	//disallow decisions timing out in 0.1 secs or less--plugging in values less than 0.1 makes decision last forever
	if (timeout >= 0.1)
	{
		timeout_time = timeout + level.time;
		allow_timeout = true;
	}
	else
	{
		timeout_time = level.time + 999999999;
		allow_timeout = false;
	}
}

qboolean decision_c::IsAdversary(edict_t &monster, edict_t *otherGuy)
{
	return (!OnSameTeam(&monster, otherGuy));
}

action_c *decision_c::AddActionForSequence(ai_c &which_ai, edict_t &monster, mmove_t *move, vec3_t dest, vec3_t face, edict_t *target, action_c *owneraction, bool shouldKill, int NullTarget, float timeoutTime)
{
	float lastingTime=timeoutTime;
	action_c *newaction=NULL;
	if (!move)//no move--forget it
	{
		return NULL;
	}
	switch(move->suggested_action)
	{
	default:
	case ACTCODE_STAND:
		//moves with the fullanim flag on them get long actions, so they can complete their anim without interruption
		if (move->actionFlags&ACTFLAG_FULLANIM)
		{
			if (lastingTime<=0.01)
			{
				lastingTime = 4.0f;
			}
			newaction=which_ai.DefaultAction(this, owneraction, move, dest, face, NULL, lastingTime, true);
		}
		else
		{
			//for moves that have the optattack flag, use as attack if a target is passed in; otherwise, use as move
			if (move->actionFlags&ACTFLAG_OPTATTACK&&target)
			{
				if (lastingTime<=0.01)
				{
					lastingTime = 0.25f;
				}
				newaction=which_ai.AttackAction(this, owneraction, move, NULL, dest, face, lastingTime, false);
				break;
			}
			newaction=which_ai.DefaultAction(this, owneraction, move, dest, face);
		}
		break;
	case ACTCODE_ATTACK:
		if (target && (target->health > 0))
		{
			if (lastingTime<=0.01)
			{
				lastingTime = 0.25f;
			}
			newaction=which_ai.AttackAction(this, owneraction, move, target, dest, face, lastingTime, true, shouldKill, NullTarget);
		}
		else
		{
			if (lastingTime<=0.01)
			{
				lastingTime = 0.25f;
			}
			newaction=which_ai.AttackAction(this, owneraction, move, NULL, dest, face, lastingTime, true, shouldKill, NullTarget);
		}
		break;
	case ACTCODE_MOVE:
		if (move->actionFlags&ACTFLAG_FULLANIM)
		{
			if (lastingTime<=0.01)
			{
				lastingTime = 0.2f;
			}
			newaction=which_ai.WalkAction(this, owneraction, move, dest, face, lastingTime, true);
		}
		else
		{
			//for moves that have the optattack flag, use as attack if a target is passed in; otherwise, use as move
			if (move->actionFlags&ACTFLAG_OPTATTACK&&target)
			{
				if (lastingTime<=0.01)
				{
					lastingTime = 0.25f;
				}
				newaction=which_ai.AttackAction(this, owneraction, move, NULL, dest, face, lastingTime, true);
				break;
			}
			if (lastingTime<=0.01)
			{
				lastingTime = 0.5f;
			}
			newaction=which_ai.WalkAction(this, owneraction, move, dest, face, lastingTime);
		}
		break;
	case ACTCODE_JUMP:
		if (lastingTime<=0.01)
		{
			lastingTime = 1.5f;
		}
		newaction=which_ai.JumpAction(this, owneraction, move, dest, face, lastingTime);
		break;
	case ACTCODE_FALL:
		if (lastingTime<=0.01)
		{
			lastingTime = 0.4f;
		}
		newaction=which_ai.FallAction(this, owneraction, move, move, dest, face, lastingTime);
		break;
	case ACTCODE_PAIN:
		if (lastingTime<=0.01)
		{
			lastingTime = 4.0f;
		}
		newaction=which_ai.DefaultAction(this, owneraction, move, dest, face, NULL, lastingTime, true);
		break;
	case ACTCODE_DEATH:
		lastingTime = 9999.0f;
		newaction=which_ai.DeathAction(this, owneraction, move, monster, NULL, NULL, lastingTime, vec3_origin);
		break;
	case ACTCODE_SCRIPTRELEASE:
		newaction=which_ai.EndScriptAction(this);
		break;
	}
	if (newaction)
	{
/*		if (shouldKill && (move->suggested_action == ACTCODE_ATTACK))
		{
			((attack_action*)newaction)->kill = true;
		}		
*/		which_ai.NewAction(newaction, &monster);
	}
	return newaction;
}

qboolean decision_c::ClearShot(ai_c &which_ai, edict_t &monster, vec3_t goalpos, edict_t *ignore, bbox_preset testbbox, float *orgPos, int *blockingGuy)
{
	trace_t tr;
	vec3_t testpos, trashmaxs;
	vec3_t	targpos;

//	if(lastClearTime > level.time - .2)
//	{	//good frequency?  Bad frequency?  I dunno...  Nevermind - this can blow me.
//		return isClear;
//	}

	which_ai.GetBBoxPreset(testbbox, testpos, trashmaxs);
	
	if(orgPos)
	{
		testpos[0]=orgPos[0];
		testpos[1]=orgPos[1];
		testpos[2]=orgPos[2]+monster.mins[2]-testpos[2]+MONSTER_SHOOT_HEIGHT;//?  Guys often seem to not be able to shoot past each other - this helps...  :/ ?
	}
	else
	{
		testpos[0]=monster.s.origin[0];
		testpos[1]=monster.s.origin[1];
		testpos[2]=monster.s.origin[2]+monster.mins[2]-testpos[2]+MONSTER_SHOOT_HEIGHT;//?  Guys often seem to not be able to shoot past each other - this helps...  :/ ?
	}

	if(testbbox == BBOX_PRESET_CROUCH)
	{
		testpos[2] -= 16;//irk
	}

	VectorCopy(goalpos, targpos);
	targpos[2] += MONSTER_SHOOT_HEIGHT;//same problem for player as for monsters - we need ta do something about this :/

	if(which_ai.GetBody() && ((which_ai.GetBody()->GetRightHandWeapon(monster)==ATK_ROCKET)||
		(which_ai.GetBody()->GetRightHandWeapon(monster)==ATK_AUTOSHOTGUN)))
	{
		vec3_t shotMin = {-6,-6,-6};
		vec3_t shotMax = {6,6,6};
		//gi.trace(testpos,shotMin,shotMax,targpos,&monster,CONTENTS_SOLID, &tr);
		gi.trace(testpos,shotMin,shotMax,targpos,&monster,MASK_SHOT, &tr);
	}
	else
	{
		//gi.trace(testpos,NULL,NULL,targpos,&monster,CONTENTS_SOLID, &tr);
		gi.trace(testpos,NULL,NULL,targpos,&monster,MASK_SHOT, &tr);
	}

	lastClearTime = level.time;

	if ((tr.fraction>0.95 && !tr.startsolid && !tr.allsolid)||tr.ent==ignore)
	//if (tr.ent==ignore)
	{	//why was the second test here?  I am confused =(
		//now see if it hit any of tha folks
/*		vec3_t dif;
		VectorSubtract(targpos, testpos, dif);
		float len = VectorLength(dif);
		CRadiusContent rad(testpos, len, 1, 0, 1);
		for(int i = 0; i < rad.getNumFound(); i++)
		{	//if this passes too close to another guy, 'tis no good
			if(rad.foundEdict(i) != ignore && rad.foundEdict(i) != &monster)
			{
				if(pointLineIntersect(testpos, targpos, rad.foundEdict(i)->s.origin, 16))
				{
					isClear = false;
					return false;
				}
			}
		}*/
		if(blockingGuy)
		{
			*blockingGuy = 0;
		}

		isClear = true;
		return true;
	}

	if(blockingGuy)
	{
		*blockingGuy = tr.ent - g_edicts;
	}


	isClear = false;
	return false;
}

mmove_t *decision_c::GetSequenceForActionCode(ai_c &which_ai, edict_t &monster, action_code the_code, vec3_t dest, vec3_t face, edict_t *target, mmove_t *preferred_move, int reject_actionflags)
{
	switch (the_code)
	{
	default:
		gi.dprintf("Invalid action code: %d!\n",the_code);
	case ACTCODE_STAND:
		return GetSequenceForStand(which_ai, monster, dest, face, preferred_move, reject_actionflags);
	case ACTCODE_MOVE:
		return GetSequenceForMovement(which_ai, monster, dest, face, preferred_move, reject_actionflags);
	case ACTCODE_ATTACK:
		return GetSequenceForAttack(which_ai, monster, dest, face, target, preferred_move, reject_actionflags);

	//eek! temporary! fixme soon!
	case ACTCODE_JUMP:
		return GetSequenceForStand(which_ai, monster, dest, face, preferred_move, reject_actionflags);
	case ACTCODE_PAIN:
		return GetSequenceForStand(which_ai, monster, dest, face, preferred_move, reject_actionflags);
	case ACTCODE_DEATH:
		return GetSequenceForStand(which_ai, monster, dest, face, preferred_move, reject_actionflags);
	case ACTCODE_FALL:
		return GetSequenceForStand(which_ai, monster, dest, face, preferred_move, reject_actionflags);
	case ACTCODE_SCRIPTRELEASE:
		return ScriptReleaseMove;
	}
}

mmove_t *decision_c::GetSequenceForStand(ai_c &which_ai, edict_t &monster, vec3_t dest, vec3_t face, mmove_t *preferred_move, int reject_actionflags)
{
	if (which_ai.GetBody())
	{
		if (preferred_move)
		{
			return which_ai.GetBody()->GetSequenceForStand(monster, dest, face, ACTSUB_NORMAL, preferred_move->bbox, preferred_move, reject_actionflags);
		}
		else
		{
			return which_ai.GetBody()->GetSequenceForStand(monster, dest, face, ACTSUB_NORMAL, BBOX_PRESET_NUMBER, NULL, reject_actionflags);
		}
	}
	return NULL;
}

float decision_c::GetGroupPosition(ai_c &which_ai, edict_t &monster, vec3_t relativeTo)
{
	return 0;
}

qboolean decision_c::AtGroupFront(ai_c &which_ai, edict_t &monster, vec3_t dest, vec3_t face)
{
	if (!which_ai.GetMove())
	{
		return false;
	}

	//check to see if guy is inside crouching aim cone
	float aimCone = which_ai.GetAimConeDegrees(monster, BBOX_PRESET_CROUCH);
	float tempAng = anglemod(which_ai.ideal_angles[YAW]-monster.s.angles[YAW]);

	//i'm outside the aim cone...don't even!
	if (tempAng > aimCone && tempAng < 360.0-aimCone)
	{
		return false;
	}

	if(which_ai.getTarget())
	{
		vec3_t dif;

//		VectorSubtract(monster.s.origin, which_ai.getTarget()->s.origin, dif);

		vec3_t	targ_pos;
		which_ai.getTargetPos(targ_pos);
		VectorSubtract(monster.s.origin, targ_pos, dif);

		if(VectorLengthSquared(dif) < (64*64))
		{	// if my target is too close to me, I don't want to be ducked - I want to back up
			return false;
		}
	}

	if (rand()&63)
	{
		return (which_ai.GetMove()->bbox==BBOX_PRESET_CROUCH);
	}
	else
	{
		return (which_ai.GetMove()->bbox!=BBOX_PRESET_CROUCH);
	}
}

mmove_t *decision_c::GetSequenceForAttack(ai_c &which_ai, edict_t &monster, vec3_t dest, vec3_t face, edict_t *target, mmove_t *preferred_move, int reject_actionflags)
{
	body_c *body = which_ai.GetBody();

	if (!body)
	{
		return NULL;
	}

	//if((!AtGroupFront(which_ai, monster, dest, face)||!ClearShot(which_ai, monster, face, target, BBOX_PRESET_CROUCH))||(monster.flags & FL_IAMTHEBOSS))
	{
		return body->GetSequenceForAttack(monster, dest, face, target, ACTSUB_NORMAL, BBOX_PRESET_STAND, preferred_move, reject_actionflags);
	}
	//return body->GetSequenceForAttack(monster, dest, face, target, ACTSUB_NORMAL, BBOX_PRESET_CROUCH, preferred_move, reject_actionflags);
}

mmove_t *decision_c::GetSequenceForJump(ai_c &which_ai, edict_t &monster, vec3_t dest, vec3_t face, mmove_t *preferred_move, int reject_actionflags)
{
	if (which_ai.GetBody())
	{
		return which_ai.GetBody()->GetSequenceForJump(monster, dest, face, ACTSUB_NORMAL, BBOX_PRESET_CROUCH, preferred_move, reject_actionflags);
	}
	return NULL;
}

mmove_t *decision_c::GetSequenceForMovement(ai_c &which_ai, edict_t &monster, vec3_t dest, vec3_t face, mmove_t *preferred_move, int reject_actionflags)
{
	if (which_ai.GetBody())
	{
		return which_ai.GetBody()->GetSequenceForMovement(monster, dest, face, monster.s.origin, monster.s.angles, ACTSUB_NORMAL, BBOX_PRESET_STAND, preferred_move, reject_actionflags);
	}
	return NULL;
}


mmove_t *decision_c::GetSequenceForDodge(ai_c &which_ai, edict_t &monster, vec3_t dest, vec3_t face, edict_t *target, mmove_t *preferred_move, int leftSide, int reject_actionflags)
{
	body_c *body = which_ai.GetBody();

	if (!body)
	{
		return NULL;
	}

	return body->GetSequenceForDodge(monster, dest, face, target, ACTSUB_NORMAL, BBOX_PRESET_STAND, preferred_move, leftSide, reject_actionflags);
}


qboolean decision_c::IsTimedOut(void)
{
	if (allow_timeout && timeout_time < level.time)
	{
		return true;
	}
	else
	{
		return false;
	}
}

qboolean decision_c::Consider(ai_c &which_ai, edict_t &monster)
{
	priority = 1;

#ifdef _DEBUG
	decName = "decision";
#endif

	return IsTimedOut();
}


void decision_c::Perform(ai_c &which_ai, edict_t &monster)
{
	vec3_t facedir;
	vec3_t facepos;

	AngleVectors(monster.s.angles,facedir,NULL,NULL);
	VectorAdd(monster.s.origin, facedir, facepos);
	AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster,facepos,facepos), facepos, facepos);
}

void decision_c::UpdateValidity(void)
{//edge toward neutral
	float dilution_interval;

	//for larger time intervals, edge further toward neutral
	dilution_interval = level.time - last_dilution_time;

	//only accept time intervals between 0 & 1
	//(1 sec should be long enough for max interval--if it's exceeded, won't matter too much)
	if (dilution_interval < 0)
	{
		dilution_interval = 0;
	}
	if (dilution_interval > 1)
	{
		dilution_interval = 1;
	}

	//weight previous validity tween .9 (for 1 sec interval) and 1.0 (for 0 sec interval),
	//with neutralizing weighted the rest (from 0.0 to 0.1)
	dilution_interval *= 0.1;
	validity = validity*(1-dilution_interval)+0.5*dilution_interval;
	last_dilution_time = level.time;
}

/**********************************************************************************
 **********************************************************************************/

path_decision::path_decision(edict_t *goalent, int priority_root, float timeout, edict_t* ScriptOwner)
:decision_c(priority_root, timeout)
{
	last_jump_time = level.time;
	path_fail_time = level.time - PATH_FAILURE_REFAIL_DELAY;

	VectorClear(path_nextpoint.pos);
	VectorClear(path_nextpoint.direction);
	path_nextpoint.targetEnt = NULL;
	path_nextpoint.valid = false;
	path_nextpoint.temp_type = 0;
	path_nextpoint.time = level.time - 1000.0F;

	path_goalentity = goalent;
	path_goal_updatedtime = level.time - 1000.0F;
	VectorClear(aimWanderVec);
	VectorClear(path_goalpos);
	VectorClear(path_fail_dir);

	//nathan added junk
	VectorClear(newMoveDir);
	lastCheckTime = 0;
	lastDuckInvalidateTime = 0;
	lastDuckValidTime = 0;
	stuckNonIdeal = false;
	nonIdealReason = NI_TOOFAR;
	blockingEnemy = 0;
}

qboolean path_decision::Consider(ai_c &which_ai, edict_t &monster)
{
#ifdef _DEBUG
	decName = "pathdecision";
#endif

	//don't automatically do this?
	SetGoalPosition(which_ai, monster);

	//no path, but i do have a goal position
	if (GetGoalPosition(which_ai,monster,NULL)&&which_ai.GetBody()&&which_ai.GetBody()->GetBestWeapon(monster)!=ATK_NOTHING)
	{
		priority = 2.5+floor(validity+0.5);
	}
	//no goal position
	else
	{
		priority = 1;
	}

	UpdateValidity();

	//if decision can time out, and its time has expired, then let ai get rid of it
	return IsTimedOut();
}

void path_decision::Perform(ai_c &which_ai, edict_t &monster)
{
	// this is problematic

/*	if((which_ai.GetPriority() != PRIORITY_HIGH)&&(path_goalentity == level.sight_client))
	{	// no more than a set number of guys can actively attack the player at once.
		// rework this?
		return;
	}*/
	AddAction(which_ai, monster);
}


void path_decision::SetGoalPosition(ai_c &which_ai, edict_t &monster)
{
	vec3_t temp_v;
	sensedEntInfo_t	sensed_client, sensed_monster;
	which_ai.GetSensedClientInfo(smask_all, sensed_client);
	which_ai.GetSensedMonsterInfo(smask_all, sensed_monster);
	//if i'm already after someone, keep at 'im
	if (path_goalentity && (path_goalentity->ai || path_goalentity->client)
		&& path_goalentity != sensed_client.ent
		&& path_goalentity != sensed_monster.ent && path_goalentity->inuse
		&& path_goalentity->health > 0)
	{
		return;
	}

	//give preference to attacking player by default
	if (sensed_client.ent && sensed_client.ent->health>0 && 
		(fabs(path_goal_updatedtime - sensed_client.time)<0.01 || path_goal_updatedtime < sensed_client.time)&&
//		(sensed_client.time + 10 > level.time) &&
		IsAdversary(monster,sensed_client.ent))
	{
		path_goalentity = sensed_client.ent;
		VectorCopy(sensed_client.pos,path_goalpos);
		path_goal_updatedtime = sensed_client.time;

		VectorAdd(sensed_client.ent->mins,sensed_client.ent->maxs,temp_v);
		VectorScale(temp_v,0.5,temp_v);
		VectorAdd(path_goalpos,temp_v,path_goalpos);
	}
	else
	{
		if (sensed_monster.ent && sensed_monster.ent->inuse &&
			(fabs(path_goal_updatedtime - sensed_monster.time)<0.01 || path_goal_updatedtime < sensed_monster.time)&&
//			(sensed_monster.time + 10 > level.time) &&
			IsAdversary(monster,sensed_monster.ent))
		{
			path_goalentity = sensed_monster.ent;
			VectorCopy(sensed_monster.pos,path_goalpos);
			path_goal_updatedtime = sensed_monster.time;

			VectorAdd(sensed_monster.ent->mins,sensed_monster.ent->maxs,temp_v);
			VectorScale(temp_v,0.5,temp_v);
			VectorAdd(path_goalpos,temp_v,path_goalpos);
		}
	}
}

qboolean path_decision::GetGoalPosition(ai_c &which_ai, edict_t &monster, vec3_t gohere)
{
	//copy ai's enemy_lastseen_pos into gohere vec
	if (gohere)//pass in NULL if just want to know whether we have goal position
	{
		VectorCopy(path_goalpos, gohere);
	}

	//check the decision's goalentity for whether i have goal pos, not ai's enemy
	if (path_goalentity)
	{
		//make sure goal entity is still valid
		if ((!path_goalentity->client && !path_goalentity->ai) || !path_goalentity->inuse
			|| path_goalentity->health <= 0 || level.time-path_goal_updatedtime>PATH_DECISION_LOSETARGETTIME)
		{
			path_goalentity = NULL;
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

//depending on type of path point path_nextpoint is, add appropriate action
qboolean path_decision::AddSpecialPathAction(ai_c &which_ai, edict_t &monster)
{
	vec3_t facedir,facevec;
	vec3_t facepos;

	//if i don't have a current pathpoint, i'm here by accident--just throw a default action on my stack
	if (!path_nextpoint.valid)
	{
		return false;
	}

	VectorSubtract(path_nextpoint.pos, monster.s.origin, facedir);
	VectorCopy(facedir,facevec);
	VectorNormalize(facedir);

	//next point in path is a temppoint--check its spawnflags to see what kinda point it is
	//i'm considering all temp points to have special actions associated withem
	//this temp point is already stale
	if (level.time - path_nextpoint.time > 1.0)
	{
		AngleVectors(monster.s.angles,facedir,NULL,NULL);
		VectorAdd(monster.s.origin, facedir, facepos);
		AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,monster.s.origin,facepos), monster.s.origin, facepos);
		return true;
	}
	//jumping path point--assuming path points are close enough together that z difference greater than stepheight requires jump
	if (path_nextpoint.temp_type==1)
	{
		AddActionForSequence(which_ai, monster, GetSequenceForJump(which_ai,monster,path_nextpoint.pos,path_nextpoint.pos), path_nextpoint.pos, path_nextpoint.pos);
		return true;
	}
	//falling path point--assuming path points are close enough together that z difference greater than stepheight requires fall
	if (path_nextpoint.temp_type==2)
	{
		AddActionForSequence(which_ai, monster, &generic_move_jumpdrop, path_nextpoint.pos, path_nextpoint.pos);
		return true;
	}
	//use a brush
	if (path_nextpoint.temp_type==3)
	{
		//make sure i know what brush to use, and it's got a valid use function
		if (path_nextpoint.targetEnt && path_nextpoint.targetEnt->plUse)
		{
			body_c *body = which_ai.GetBody();

			path_nextpoint.targetEnt->plUse(path_nextpoint.targetEnt, &monster, &monster);
			//urk, do an appropriate thing here
			VectorScale(path_nextpoint.direction, 20, facepos);
			VectorAdd(facepos, monster.s.origin, facepos);
			if (body)
			{
				AddActionForSequence(which_ai, monster, body->GetSequenceForStand(monster, facepos, facepos, ACTSUB_NORMAL, BBOX_PRESET_STAND, &generic_move_opendoor), facepos, facepos);
			}
			else
			{
				AddActionForSequence(which_ai, monster, &generic_move_opendoor, facepos, facepos);
			}
			return true;
		}
		return false;
	}

	//?
	return false;

	//normal walking point
	VectorScale(facedir,(VectorLength(which_ai.velocity)+10)*0.1,facepos);
	VectorAdd(facepos, monster.s.origin, facepos);
	AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,facepos,facepos), facepos, facepos);
	return true;
}

#define ADJUST_SEARCH_RANGE 100.0

void rotMe(vec3_t in, float ang, vec3_t out)
{
	vec3_t temp;
	float cosAng = cos(ang*M_PI/180);
	float sinAng = sin(ang*M_PI/180);

	temp[0] = in[0] * cosAng + in[1] * sinAng;
	temp[1] = - in[0] * sinAng + in[1] * cosAng;
	temp[2] = in[2];

	VectorCopy(temp, out);
}

void debug_drawbox(edict_t* self,vec3_t vOrigin, vec3_t vMins, vec3_t vMaxs, int nColor);

float guyCheckForMove(vec3_t start, vec3_t dir, int requestMovement, edict_t *self)
{
	vec3_t testCenter;

	VectorMA(start, 50, dir, testCenter);

	CRadiusContent rad(testCenter, 100, 1, 0);//so we get a rough, decent test spot

	VectorMA(start, 150, dir, testCenter);// the endpoint of my move test

	float distAchieved = 1.0;

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		if(rad.foundEdict(i) == self)
		{	//sillyness
			continue;
		}

	 	if(pointLineIntersect(start, testCenter, rad.foundEdict(i)->s.origin, 30))
		{
			vec3_t dif;

			VectorSubtract(rad.foundEdict(i)->s.origin, start, dif);

			float newDist = VectorLengthSquared(dif) / (150 * 150);//see how good this route is

			vec3_t goPlease;

			VectorCopy(dif, goPlease);
			VectorNormalize(goPlease);
			if(rad.foundEdict(i)->ai)
			{
				(rad.foundEdict(i)->ai)->RequestMoveOutOfWay(goPlease);
			}

			if((newDist < distAchieved) && (newDist > 0.005))//don't grab myself
			{
				distAchieved = newDist;
			}
		}
	}
	return distAchieved;
}

float checkVectForMove(vec3_t start, vec3_t dir, vec3_t min, vec3_t max, edict_t *ent, edict_t **hitEnt, int requestMovement)
{
	vec3_t end;
	trace_t walktrace;

	VectorMA(start, 60, dir, end);
	gi.trace (start, min, max, end, ent, MASK_MONSTERSOLID, &walktrace);

	if(hitEnt)
	{
		*hitEnt = walktrace.ent;
	}

	float frac = guyCheckForMove(start, dir, requestMovement, ent);

//	if(walktrace.fraction < .8)
	{
		return walktrace.fraction * frac;
	}

	return 1.0 * frac;
/*
	VectorMA(start, 32, dir, end);//???????
	VectorCopy(end, down);
	down[2] -= (24.0+32+8);// furthest a guy can consider

	gi.trace (end, vec3_origin, vec3_origin, down, ent, MASK_MONSTERSOLID, &walktrace);

	if(walktrace.fraction < 1.0)
	{	// this is what we hope for
		return 1.0;
	}

	return 0.0;//BAD*/
}

//try some different angles to run and see if they're any better
int findBestDir(vec3_t start, vec3_t dir, vec3_t outDir, float speed, edict_t &guy, ai_c &which_ai)
{	//returns whether ANY paths were found.

	float clear[5];
	vec3_t	dirs[5];
	float	firstDir;//1.0 or -1.0; randomly do left or right first 
	vec3_t	testDir;
	vec3_t	testMin;
	int		i;

//	Com_Printf("Doing bump check at %f\n", (float)level.time);

	VectorCopy(guy.mins, testMin);
	testMin[2] += 16;//eh?

	int ang = 40;

	//clear out clears, so we can safely abort the clear-checking process
	for(i = 0; i < 5; i++)
	{
		clear[i]=0.0;
	}

	//check the direct route first, and abort as soon as we got a totally clear path
	clear[0] = checkVectForMove(start, dir, testMin, guy.maxs, &guy, 0, 0);
	VectorCopy(dir, dirs[0]);
	if (clear[0]<0.99)
	{
		for(i = 1; i < 5; i++)
		{
			firstDir = (float)(gi.irand(0,1)*2-1);//1.0 or -1.0
			rotMe(dir, ang*firstDir, testDir);

			clear[i] = checkVectForMove(start, testDir, testMin, guy.maxs, &guy, 0, 0);
			VectorCopy(testDir, dirs[i]);

			if (clear[i]>0.99)//abort: we got our dir
				break;

			i++;//odd way to deal with a for loop, eh?

			rotMe(dir, ang*firstDir*-1.0, testDir);

			clear[i] = checkVectForMove(start, testDir, testMin, guy.maxs, &guy, 0, 0);
			VectorCopy(testDir, dirs[i]);

			if (clear[i]>0.99)//abort: we got our dir
				break;

			ang += 40;
			//if((i == 2)||(i == 5))clear[i] *= .7;//these are totally perp and less good
		}
	}

	//priority should be for directions that are similar to the current heading and have not much crowding
	int bestVal = -1;
	float	bestDist = 0.2;
	for(i = 0; i < 5; i++)
	{
		if(clear[i] > bestDist)
		{
			bestDist = clear[i];
			bestVal = i;
		}
	}
	if(bestVal > -1)
	{
		VectorCopy(dirs[bestVal], outDir);
		return 1;
		//FX_MakeElectricArc(start, 60, dirs[bestVal]);
	}
	else
	{	//if all else fails, well, yer screwed.  Sit still and shoot people...
		VectorClear(outDir);
		return 0;
	}

}

void path_decision::RecalcMoveVec(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	vec3_t point;
	vec3_t testMin;

	if(!path_nextpoint.valid || (aiPoints.isActive()))
	{
		VectorCopy(goalPos, point);
	}
	else
	{
		VectorCopy(path_nextpoint.pos, point);
	}

	VectorSubtract(point, monster.s.origin, newMoveDir);
	VectorNormalize(newMoveDir);

	//if my priority is low, just accept the ideal direction and be clumsy. hell, nobody'll see.
	if (which_ai.GetPriority()==PRIORITY_LOW)
	{
		return;
	}

	VectorCopy(monster.mins, testMin);
	testMin[2] += 16;

	edict_t *hitGuy;

	float clear = checkVectForMove(monster.s.origin, newMoveDir, testMin, monster.maxs, &monster, &hitGuy, 1);

	//if i'm going to hit something that's not a useable brush, adjust my course
	if(clear != 1.0 && (!hitGuy || !IsUseableBrush(hitGuy) || 
		!IsBrushUseableNow(hitGuy, level.time, &monster)))
	{
		if(!findBestDir(monster.s.origin, newMoveDir, newMoveDir, VectorLength(which_ai.velocity)+10, monster, which_ai))
		{
			//stuckNonIdeal = true;
		}
	}
}


void path_decision::GetMovementVector(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	return;
#if 0
	stuckNonIdeal = false;
	if(monster.spawnflags & SPAWNFLAG_HOLD_POSITION)
	{	//fixme - these guys need a bit of room for movement...
		VectorClear(moveVec);
		stuckNonIdeal = false;
		return;
	}

	int normalWander = 1;

	vec3_t targ;
	vec3_t dif;
	VectorSubtract(goalPos, monster.s.origin, dif);
	if((GetClassCode() == PURSUE_DECISION)&&( (monster.flags) || 
		(DotProduct(dif, dif) < which_ai.GetMySkills()->getRangeMin()*which_ai.GetMySkills()->getRangeMin())))
	{	// too close!  Just go directly away
		//if(aiPoints.isActive())
		if(0)
		{
			aiPoints.getReversePath(monster.s.origin, goalPos, &which_ai.getPathData(), &which_ai, &monster);
			VectorCopy(which_ai.getPathData().goPoint, targ);

			Enemy_Printf(&which_ai, "backup:%d\n", GetClassCode());

			normalWander = 0;

			if(which_ai.getPathData().blocked)
			{	// can't go where I want to :(
				VectorCopy(monster.s.origin, targ);
			}
		}
		else
		{
			VectorNormalize(dif);
			VectorScale(dif, -4.0, dif);
			VectorAdd(dif, monster.s.origin, targ);

			normalWander = 0;
		}
	}
	
	float *avoidDir;
	avoidDir = which_ai.GetMoveOutOfWay();
	int wanderDirSet = 0;

	if(avoidDir[0] || avoidDir[1] || avoidDir[2])
	{
		wanderDirSet = 1;
		VectorMA(monster.s.origin, 64, avoidDir, targ);
		VectorCopy(avoidDir, aimWanderVec);
		normalWander = 0;
	}

/*	if(which_ai.GetMySkills()->testFlag(AIS_NOWANDER))
	{
		normalWander = 0;
	}*/

	if(normalWander)
	{
		//adjust where i'll wander whilst aiming
		//ick, this is more expensive than it should be
/*		CRadiusContent rad(monster.s.origin, 100);
		vec3_t avoidDir;
		if(rad.getNumFound())
		{
			VectorSubtract(rad.foundEdict(0)->s.origin, monster.s.origin, avoidDir);
			for(int i = 1; i < rad.getNumFound(); i++)
			{
				vec3_t dif;

				VectorSubtract(rad.foundEdict(i)->s.origin, monster.s.origin, dif);
				if(DotProduct(dif, avoidDir) < 0)
				{//conflicting, so go nowhere
					VectorClear(avoidDir);
					break;
				}
			}
			VectorScale(avoidDir, 20, aimWanderVec);
		}
		else*/
		{
			float wanderDist = 20;
			vec3_t toGoal;
			float distToGoal;

			//get my goal vector, to make sure i don't wander too close/far to goal
			VectorSubtract(goalPos, monster.s.origin, toGoal);
			toGoal[2]=0;
			distToGoal=VectorNormalize(toGoal);

			//no wandering up or down!
			aimWanderVec[2]=0;

			float minRange = which_ai.GetMySkills()->getRangeMin();
			float maxRange = which_ai.GetMySkills()->getRangeMax();

			//keep a cap on the wandering vector
			wanderDist=VectorNormalize(aimWanderVec);
			if(!wanderDirSet)
			{
				aimWanderVec[0]+=gi.flrand(-3,3);
				aimWanderVec[1]+=gi.flrand(-3,3);

				if (wanderDist>20)
				{
					wanderDist=20;
				}
				//too close
				//if (distToGoal < 100)
				if (distToGoal < minRange)
				{
					if (distToGoal<1)
					{
						distToGoal=1;
					}
					//going wrong way--reverse direction
					if (DotProduct(aimWanderVec,toGoal)>0.0)
					{
						wanderDist*=-0.5;
						//from a 2:1 to a 1:100 ratio of totallyAway:cur, decreasing the farther i am to guy
						//VectorMA(aimWanderVec, -((101-distToGoal)/100), toGoal, aimWanderVec);
						VectorMA(aimWanderVec, -((minRange + 1 - distToGoal)/minRange), toGoal, aimWanderVec);
						VectorNormalize(aimWanderVec);
					}
					else
					{
						//going away--go faster (but i wanna go more clearly away too)
						wanderDist+=1;
						VectorInverse(toGoal);
						//from a 1:1 to a 1:100 ratio of totallyAway:cur, decreasing the farther i am to guy
						//VectorMA(aimWanderVec, ((101-distToGoal)/100), toGoal, aimWanderVec);
						VectorMA(aimWanderVec, ((minRange + 1 - distToGoal)/minRange), toGoal, aimWanderVec);
						VectorNormalize(aimWanderVec);
					}
				}
				//too far
				//if (distToGoal > 300)
				if (distToGoal > maxRange)
				{
					//if (distToGoal>349)
					//{
						//distToGoal=349;
					//}
					if (distToGoal>maxRange + 50)
					{
						distToGoal=maxRange + 50;
					}
					//going wrong way--reverse direction
					if (DotProduct(aimWanderVec,toGoal)<0.0)
					{
						wanderDist*=-0.5;
						VectorInverse(toGoal);
						//from a 1:1 to a 1:50 ratio of cur:totallyAway, decreasing the closer i am to guy
						//VectorMA(aimWanderVec, (distToGoal-299)/50, toGoal, aimWanderVec);
						VectorMA(aimWanderVec, (distToGoal - maxRange + 1)/50, toGoal, aimWanderVec);
						VectorNormalize(aimWanderVec);
					}
					else
					{
						//going toward--go faster (but i wanna go more clearly toward too)
						wanderDist+=1;
						//from a 3:1 to a 1:50 ratio of cur:totallyAway, decreasing the closer i am to guy
						//VectorMA(aimWanderVec, (distToGoal-299)/50, toGoal, aimWanderVec);
						VectorMA(aimWanderVec, (distToGoal - maxRange + 1)/50, toGoal, aimWanderVec);
						VectorNormalize(aimWanderVec);
					}
				}
			}
			VectorScale(aimWanderVec,wanderDist,aimWanderVec);
		}

		trace_t tr;

		vec3_t spot;

		VectorAdd(monster.s.origin, aimWanderVec, spot);
		VectorAdd(spot, aimWanderVec, spot);//test a bit farther to see if we strike anything

		vec3_t testmin, testmax;
		VectorAdd(spot, monster.mins, testmin);
		VectorAdd(spot, monster.maxs, testmax);
		/*if(!aiPoints.inRegion(which_ai.getPathData().curRegion, testmin, testmax))
		{
			VectorClear(aimWanderVec);
		}
		else*/
		{
			gi.trace(monster.s.origin, monster.mins, monster.maxs, spot, &monster, MASK_MONSTERSOLID, &tr);
			if(tr.fraction < 1.0)
			{
				VectorClear(aimWanderVec);//guys shouldn't wander into walls
			}
		}
		//if(aiPoints.isActive())
		if(0)
		{
			aiPoints.getPath(monster.s.origin, goalPos, &which_ai.getPathData(), &which_ai, &monster);
			VectorCopy(which_ai.getPathData().goPoint, targ);

			Enemy_Printf(&which_ai, "forward:%d(%d->%d=%d)\n", GetClassCode(),
				which_ai.getPathData().curNode, which_ai.getPathData().nextNode, which_ai.getPathData().finalNode);

			if(which_ai.getPathData().blocked)
			{	// can't go where I want to :(
				VectorCopy(monster.s.origin, targ);
			}
		}
		else
		{
			VectorCopy(goalPos, targ);
		}
	}

	if(monster.flags & FL_IAMTHEBOSS)
	{	// the boss isn't interested in normal wandering
		VectorClear(aimWanderVec);
	}

	RecalcMoveVec(which_ai, monster, targ, moveVec);
	lastCheckTime = level.time;

	VectorScale(newMoveDir, VectorLength(which_ai.velocity)+10, moveVec);

	if(!normalWander)
	{	// one and the same when you really need to back up como este
		VectorCopy(moveVec, aimWanderVec);
		VectorNormalize(aimWanderVec);
		VectorScale(aimWanderVec, 20, aimWanderVec);
	}

	VectorClear(avoidDir);
#endif
	//if 0'd this whole section here
}


qboolean path_decision::IsIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t curPos, vec3_t goalPos, vec3_t moveVec)
{
	vec3_t toGoal;
	float distToGoal;

	blockingEnemy = 0;

	vec3_t request = {0,0,0};//uhoh
	which_ai.RequestMoveOutOfWay(request);

	if(which_ai.BeingWatched())
	{
		int asdf = 9;
	}

	if(monster.spawnflags & SPAWNFLAG_HOLD_POSITION)
	{//very special case - there guys are always in an ideal position, sort of
		nonIdealReason = NI_IDEALPOSITION;
		which_ai.RequestMoveOutOfWay(vec3_origin);
		return true;
	}



	body_c *body = which_ai.GetBody();

	if (body)
	{
		if(body->TestDamage())
		{//damaged guys are never ever happy - they want to get away
			which_ai.getPathData().backingUp = 1;
			Enemy_Printf(&which_ai, "NIR - too close\n");
			nonIdealReason = NI_TOOCLOSE;
			return false;
		}
	}
		

	if (level.time - path_goal_updatedtime > PATH_DECISION_LOSETARGETTIME)
	{
		Enemy_Printf(&which_ai, "NIR - out of time\n");
		nonIdealReason = NI_TOOFAR;//?  Who cares, I suppose
		return false;
	}
	VectorSubtract(goalPos, curPos, toGoal);
	distToGoal=VectorLength(toGoal);
	if (distToGoal < 10)
	{
		which_ai.RequestMoveOutOfWay(vec3_origin);
		return true;
	}

	if(!which_ai.GetMove())
	{
		Enemy_Printf(&which_ai, "NIR - don't have a move\n");
		nonIdealReason = NI_TOOFAR;//This I find confusing
		return false;
	}

	if((distToGoal >= which_ai.GetMySkills()->getRangeMax()*.75) && (!which_ai.GetMySkills()->testFlag(AIS_WONTADVANCE)))
	{
		if(which_ai.getPathData().approaching || (distToGoal >= which_ai.GetMySkills()->getRangeMax()*1.2))
		{
			which_ai.getPathData().approaching = 1;
			Enemy_Printf(&which_ai, "NIR - too far\n");
			nonIdealReason = NI_TOOFAR;
			return false;
		}
	}
	else
	{
		which_ai.getPathData().approaching = 0;
	}

	if(stuckNonIdeal)
	{
		//I hate this ;(
		Enemy_Printf(&which_ai, "NIR - stuck nonideal\n");
		nonIdealReason = NI_TOOFAR;//?
		return false;
	}

	if(distToGoal <= which_ai.GetMySkills()->getRangeMin()*1.2)
	{	// try to avoid extremely abrupt movements
		if(which_ai.getPathData().backingUp || (distToGoal <= which_ai.GetMySkills()->getRangeMin()*.75))
		{
			which_ai.getPathData().backingUp = 1;
			Enemy_Printf(&which_ai, "NIR - too close\n");
			nonIdealReason = NI_TOOCLOSE;
			return false;
		}
	}
	else
	{
		which_ai.getPathData().backingUp = 0;
	}

	if(!which_ai.GetMove() || !ClearShot(which_ai, monster, goalPos, path_goalentity, which_ai.GetMove()->bbox, 0, &blockingEnemy))
	{
		Enemy_Printf(&which_ai, "NIR - no clear shot\n");
		nonIdealReason = NI_NOTCLEAR;
		return false;
	}

	nonIdealReason = NI_IDEALPOSITION;
	which_ai.RequestMoveOutOfWay(vec3_origin);
	return true;
}

void BuildVectorForGuys(edict_t *source, vec3_t desiredVect, float radius = 64, int *nearProblems = 0)
{
	CRadiusContent rad(source->s.origin, radius*2, 1, 1);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		if(rad.foundEdict(i) == source)
		{
			continue;
		}

		ai_c *otherai = (ai_c *)((ai_public_c *)(rad.foundEdict(i)->ai));
		if(otherai)
		{
			if(!(*otherai->GetMoveOutOfWay()))
			{	//this guy is fine - don't move for him
				continue;
			}
		}

		vec3_t dif;

		VectorSubtract(rad.foundEdict(i)->s.origin, source->s.origin, dif);
		float distAway = VectorNormalize(dif);

		*nearProblems = 1;

		if(distAway > radius)
		{
			continue;
		}

		if(DotProduct(otherai->GetMoveOutOfWay(), dif) < 0)
		{	//this guy is going somewhere else - big deal
			continue;
		}

		float mVal = distAway / radius;
		mVal *= mVal;


		if(DotProduct(dif, desiredVect) > 0)
		{
			//VectorCopy(dif, desiredVect);
			VectorScale(desiredVect, mVal, desiredVect);
			VectorMA(desiredVect, 1.0 - mVal, dif, desiredVect);
		}
		else
		{
			//VectorScale(dif, -1, desiredVect);
			VectorScale(desiredVect, mVal, desiredVect);
			VectorMA(desiredVect, -1.0 * (1.0 - mVal), dif, desiredVect);
		}
	}
	desiredVect[2] = 0;
	VectorNormalize(desiredVect);
}

void path_decision::AddActionIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	which_ai.GetMiscFlags() &= ~MISC_FLAG_FIRINGBLIND;

	vec3_t atkPos,wanderPos;
	if (path_goalentity->client)
	{
		VectorScale(path_goalentity->velocity, 0.2, atkPos);
	}
	else if (path_goalentity->ai)
	{
		VectorScale(path_goalentity->ai->velocity, 0.2, atkPos);
	}
	else
	{
		VectorClear(atkPos);
	}
	VectorAdd(goalPos, atkPos, atkPos);
	VectorCopy(monster.s.origin, wanderPos);
//	VectorAdd(monster.s.origin, aimWanderVec, wanderPos);

//	VectorClear(wanderPos);//go nowhere right now!  You love this spot!

	vec3_t wanderDir;
	VectorClear(wanderDir);
	int nearbyUnruly = 0;
//	BuildVectorForGuys(&monster, wanderDir, 96, &nearbyUnruly);
//	int mustMove = (VectorLengthSquared(wanderDir) > .1);
//	VectorMA(monster.s.origin, 32, wanderDir, wanderPos);

//	if(mustMove)
//	{
//		vec3_t request = {1,0,0};//uhoh
//		which_ai.RequestMoveOutOfWay(wanderDir);
//	}
	int mustMove = 0;

	int attack = 1;

	//no attacking if we're not ready to harm the player 
	float awakeTimeValue = level.time - which_ai.getFirstTargetTime();
	//if(awakeTimeValue < which_ai.GetMySkills()->getHesitation() && (which_ai.GetStartleability()))
	if(awakeTimeValue < which_ai.GetMySkills()->getHesitation() * ((gmonster.GetClosestEnemy() == &monster) ? .5:1))
	{	// we are groggy for a bit when we first arise
		attack = 0;
	}

/*	aiPoints.idealWanderPoint(&which_ai.getPathData(), &which_ai);
	if(!which_ai.getPathData().blocked)
	{
		VectorCopy(which_ai.getPathData().goPoint, wanderPos);
		mustMove = 1;
	}*/

	if(attack)
	{
		//Enemy_Printf(&which_ai, "Ideal attack \n");
		//AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, wanderPos, goalPos, path_goalentity), wanderPos, goalPos, path_goalentity);

		body_c *body = which_ai.GetBody();

		if (body)
		{
			//mmove_t *myMove = body->GetSequenceForAttack(monster, dest, face, target, ACTSUB_NORMAL, BBOX_PRESET_STAND, preferred_move, reject_actionflags);
			vec3_t fwd;
			vec3_t toFace;
			//which_ai.GetAimVector(fwd);
			//vertical does not matter
			AngleVectors(monster.s.angles, fwd, 0, 0);
			fwd[2] = 0;
			VectorNormalize(fwd);
			VectorSubtract(goalPos, monster.s.origin, toFace);
			toFace[2] = 0;
			VectorNormalize(toFace);
			mmove_t *myMove;

			if((!which_ai.GetMySkills()->testFlag(AIS_NODUCK)) && 
				(DotProduct(toFace, fwd) > .98) && 
				!mustMove &&
				!nearbyUnruly &&
				((lastDuckInvalidateTime > lastDuckValidTime + 3.0)||(lastDuckInvalidateTime < lastDuckValidTime)) &&
				ClearShot(which_ai, monster, goalPos, path_goalentity, BBOX_PRESET_CROUCH))
			{
				lastDuckValidTime = level.time;
				myMove = body->GetSequenceForAttack(monster, wanderPos, goalPos, path_goalentity, ACTSUB_NORMAL, BBOX_PRESET_CROUCH);
			}
			else if(ClearShot(which_ai, monster, goalPos, path_goalentity, BBOX_PRESET_STAND))
			{
				lastDuckInvalidateTime = level.time;
				myMove = body->GetSequenceForAttack(monster, wanderPos, goalPos, path_goalentity, ACTSUB_NORMAL, BBOX_PRESET_STAND);
			}
			else if((DotProduct(toFace, fwd) > .98)&&
				((lastDuckInvalidateTime > lastDuckValidTime + 3.0)||(lastDuckInvalidateTime < lastDuckValidTime))&&
				(!which_ai.GetMySkills()->testFlag(AIS_NODUCK)))
			{	//well, at the very least minimize surface area
				lastDuckValidTime = level.time;
				myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_CROUCH);
			}
			else
			{	//can't turn while crouched ;(
				lastDuckInvalidateTime = level.time;
				myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_STAND);
			}

			AddActionForSequence(which_ai, monster, myMove, wanderPos, goalPos, path_goalentity);
		}
	}
	else
	{
		body_c *body = which_ai.GetBody();

		if (body)
		{
			Enemy_Printf(&which_ai, "Ideal stand\n");
			AddActionForSequence(which_ai, monster, body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_STAND), goalPos, goalPos);
			//AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,wanderPos,goalPos), wanderPos, goalPos);
		}
	}
}

void AdjustVectorByGuys(edict_t *source, vec3_t desiredVect, float radius = 96)
{
	CRadiusContent rad(source->s.origin, radius, 1, 1);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		if(rad.foundEdict(i) == source)
		{
			continue;
		}

		vec3_t dif;

		VectorSubtract(rad.foundEdict(i)->s.origin, source->s.origin, dif);
		float distAway = VectorNormalize(dif);

		if(DotProduct(desiredVect, dif) < 0)
		{
			continue;
		}

		//now do the incorrect rotation
		float temp = dif[0];
		dif[0] = dif[1];
		dif[1] = -temp;

		float mVal = distAway / radius;
		mVal *= mVal;


		if(DotProduct(dif, desiredVect) > 0)
		{
			//VectorCopy(dif, desiredVect);
			VectorScale(desiredVect, mVal, desiredVect);
			VectorMA(desiredVect, 1.0 - mVal, dif, desiredVect);
		}
		else
		{
			//VectorScale(dif, -1, desiredVect);
			VectorScale(desiredVect, mVal, desiredVect);
			VectorMA(desiredVect, -1.0 * (1.0 - mVal), dif, desiredVect);
		}
	}
	desiredVect[2] = 0;
	VectorNormalize(desiredVect);
}


void path_decision::AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	vec3_t actionGoal;
	float move_dist_sq;

//	move_dist_sq = VectorLengthSquared(moveVec);
	move_dist_sq = 26;

	if(which_ai.BeingWatched())
	{
		int asdf = 9;
	}

	int attack = 1;

	if(nonIdealReason == NI_TOOFAR_SEARCHING)
	{
		attack = 0;
	}

	if(nonIdealReason == NI_TOOFAR && which_ai.GetMySkills()->testFlag(AIS_ATTACKONLYINRANGE))
	{
		attack = 0;
	}

	//no attacking if we're not ready to harm the player 
	float awakeTimeValue = level.time - which_ai.getFirstTargetTime();
	//if(awakeTimeValue < which_ai.GetMySkills()->getHesitation() && (which_ai.GetStartleability()))
	if(awakeTimeValue < which_ai.GetMySkills()->getHesitation() * ((gmonster.GetClosestEnemy() == &monster) ? .5:1))//the closest guy gets to attack quicker
	{	// we are groggy for a bit when we first arise
		attack = 0;
	}

	which_ai.GetMiscFlags() &= ~MISC_FLAG_FIRINGBLIND;

	if(nonIdealReason == NI_DODGING)
	{
		mmove_t *dodgeMove;

		dodgeMove=GetSequenceForDodge(which_ai, monster, goalPos, goalPos, path_goalentity, 0, gi.irand(0,1)?-1:1);
		if(dodgeMove)
		{
			AddActionForSequence(which_ai, monster, dodgeMove, goalPos, goalPos);
			return;
		}
		else
		{
			nonIdealReason = NI_DUCKING;
		}
	}

	if(nonIdealReason == NI_DUCKING)
	{
		body_c *body = which_ai.GetBody();

		if (body)
		{
			mmove_t *myMove;

			myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_CROUCH, &generic_move_crouch_cower_mid);

			if(myMove)
			{
				AddActionForSequence(which_ai, monster, myMove, goalPos, goalPos, path_goalentity);
				return;
			}
			else
			{
				nonIdealReason = NI_FLEEING;
			}
		}
		else
		{
			nonIdealReason = NI_FLEEING;
		}
	}

	int fastStrafe = 0;
	if(which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_mrs) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_l) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_p))
	{
		fastStrafe = 1;
	}

	int fastRetreat = 0;
	if(which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_mrs) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_p2) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_l))
	{
		fastRetreat = 1;
	}

	int proceedForward = 1;

	if((25>move_dist_sq) ||
		(monster.spawnflags & SPAWNFLAG_HOLD_POSITION) ||
		((which_ai.GetPriority() != PRIORITY_HIGH)&&(path_goalentity == level.sight_client)))
	{
		Enemy_Printf(&which_ai, "Non ideal stuck");
		AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster,goalPos,goalPos), goalPos, goalPos);
		proceedForward = 0;
	}
	else if(nonIdealReason == NI_TOOCLOSE)
	{
		proceedForward = 0;
		int backup = 0;
		vec3_t toAvoid, toTarg, looker;
		if(aiPoints.isActive())
		{
			aiPoints.getReversePath(monster.s.origin, goalPos, &which_ai.getPathData(), &which_ai, &monster, which_ai.getTarget());

			if(which_ai.getPathData().blocked)
			{	// can't go where I want to :(
				backup = 0;
			}
			else
			{
				Enemy_Printf(&which_ai, "Via %d->%d = %d\n", which_ai.getPathData().curNode,
					which_ai.getPathData().nextNode, which_ai.getPathData().finalNode);

				VectorSubtract(which_ai.getPathData().goPoint, monster.s.origin, toAvoid);
				toAvoid[2] = 0;
				float toAvoidLen = VectorNormalize(toAvoid);
/*				if(VectorLength(toAvoid) < .01)
				{
					VectorCopy(goalPos, actionGoal);
				}
				else*/
				{
					VectorMA(monster.s.origin, 10, toAvoid, actionGoal);
				}

				//if(toAvoidLen < .05)
				if(toAvoidLen < 10)
				{
					backup = 0;
				}
				else
				{
					if(which_ai.GetMySkills()->testFlag(AIS_NOATTACKONRETREAT) ||
						(which_ai.GetMove() && ClearShot(which_ai, monster, toAvoid, path_goalentity, which_ai.GetMove()->bbox, actionGoal)))
					{	// if we can still shoot once we backup, we like this spot - otherwise no
						backup = 1;
					}
					else
					{
						backup = 0;
					}
				}
				VectorMA(monster.s.origin, 40, toAvoid, actionGoal);
			}
		}
		else
		{
			backup = 0;

		}

		if(backup)
		{
			
			VectorSubtract(actionGoal, monster.s.origin, toAvoid);
			toAvoid[2] = 0;
			VectorNormalize(toAvoid);

			AdjustVectorByGuys(&monster, toAvoid);

			which_ai.RequestMoveOutOfWay(toAvoid);

			VectorSubtract(goalPos, monster.s.origin, toTarg);
			toTarg[2] = 0;
			float distToTarg = VectorNormalize(toTarg);

			if(DotProduct(toAvoid, toTarg) < -.717)
			{	//move in a direction opposite of where I want to face
				if(!fastRetreat)
				{
					VectorScale(toAvoid, 4.0, actionGoal);
					VectorAdd(monster.s.origin, actionGoal, actionGoal);
				}

				VectorScale(toAvoid, -2440, looker);
				VectorAdd(monster.s.origin, looker, looker);
			}
			else if(DotProduct(toAvoid, toTarg) > .717)
			{	//move in a direction towards where I face
				VectorScale(toAvoid, 2440, looker);
				VectorAdd(monster.s.origin, looker, looker);
			}
			else
			{
				if(!fastStrafe)
				{
					VectorScale(toAvoid, 12.0, actionGoal);
					VectorAdd(monster.s.origin, actionGoal, actionGoal);
				}

				//set it to the side now - for strafing
				toAvoid[2] = toAvoid[1];
				toAvoid[1] = -toAvoid[0];
				toAvoid[0] = toAvoid[2];
				toAvoid[2] = 0;

				if(DotProduct(toAvoid, toTarg) > 0)
				{
					VectorScale(toAvoid, 2440, looker);
					VectorAdd(monster.s.origin, looker, looker);
				}
				else
				{
					VectorScale(toAvoid, -2440, looker);
					VectorAdd(monster.s.origin, looker, looker);
				}
			}

			if(aipoints_show->value)
			{
				paletteRGBA_t	col = {250, 250, 250, 250};
				//FX_MakeLine(node[startNode].getPos(), node[nextNode].getPos(), col, 1);
				FX_MakeLine(which_ai.getPathData().goPoint, monster.s.origin, col, 1);
				FX_MakeRing(which_ai.getPathData().goPoint, 8);
				paletteRGBA_t	col2 = {0, 0, 0, 250};
				FX_MakeLine(actionGoal, monster.s.origin, col2, 1);
			}

			vec3_t testShoot;

			VectorSubtract(goalPos, monster.s.origin, testShoot);
			VectorNormalize(testShoot);
			VectorMA(monster.s.origin, 64, testShoot, testShoot);

			if(attack && (!which_ai.GetMySkills()->testFlag(AIS_NOATTACKONRETREAT)) && ClearShot(which_ai, monster, testShoot, path_goalentity, which_ai.GetMove()->bbox))
			{
				//AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, actionGoal, goalPos, path_goalentity), actionGoal, goalPos, path_goalentity);
				AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, actionGoal, looker, path_goalentity), actionGoal, looker, path_goalentity);
			}
			else
			{	//just back up a bit - can't shoot from here
				//oh dear.  gsfMovement is fairly different from gsfAttack... I did not know =/
				if(!fastRetreat)
				{
					VectorScale(toAvoid, 4.0, actionGoal);
					VectorAdd(monster.s.origin, actionGoal, actionGoal);
				}
				AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,looker), actionGoal, looker);
			}
		}
		else
		{
			if(attack && ClearShot(which_ai, monster, goalPos, path_goalentity, which_ai.GetMove()->bbox))
			{	//can't move, but at least I can shoot!
				AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, monster.s.origin, goalPos, path_goalentity), monster.s.origin, goalPos, path_goalentity);
			}
			else
			{	//Stand around and do nothing - too close and not clear.  Sad.
				if(which_ai.GetMySkills()->testFlag(AIS_WONTADVANCE))
				{
					AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster,goalPos,goalPos), goalPos, goalPos);
				}
				else
				{
					proceedForward = 1;
				}
			}
		}
	}

	if(blockingEnemy)
	{	//this is only the case if clearshot is obstructed by another guy
		vec3_t dif;
		VectorSubtract(goalPos, monster.s.origin, dif);
		dif[2] = 0;
		float goalDist = VectorNormalize(dif);

		if(goalDist > 1)
		{
			dif[2] = dif[1];
			dif[1] = dif[0]*-1;
			dif[0] = dif[2];
			dif[2] = 0;

			VectorMA(monster.s.origin, 64, dif, dif);

			trace_t tr;

			gi.trace(monster.s.origin, monster.mins, monster.maxs, dif, &monster, MASK_MONSTERSOLID, &tr);

			vec3_t lookSpot;
			VectorCopy(goalPos, lookSpot);

			if(nonIdealReason == NI_FLEEING)
			{
				VectorCopy(dif, lookSpot);
			}
		
			if(tr.fraction > .99 && !tr.allsolid && !tr.startsolid)
			{
				AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,dif,lookSpot), dif, lookSpot);
			}
			else
			{
				AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster,goalPos,goalPos), goalPos, goalPos);
			}
			proceedForward = 0;

			if(aipoints_show->value)
			{
				paletteRGBA_t	col = {0, 0, 250, 250};
				//FX_MakeLine(node[startNode].getPos(), node[nextNode].getPos(), col, 1);
				FX_MakeLine(dif, monster.s.origin, col, 1);
			}
		}
	}

	if(proceedForward)// if(nonIdealReason == NI_TOOFAR)//make this catchall - why not
	{
		if(aiPoints.isActive())
		{
			aiPoints.getPath(monster.s.origin, goalPos, &which_ai.getPathData(), &which_ai, &monster, 1, which_ai.getTarget());

/*			vec3_t sep;
			float len;
			VectorSubtract(which_ai.getPathData().goPoint, monster.s.origin, sep);
			len = VectorLength(sep);

			if(nonIdealReason == NI_TOOFAR_SEARCHING && which_ai.getPathData().curNode == which_ai.getPathData().nextNode && len < 48)
			{
				VectorCopy(monster.s.origin, which_ai.getPathData().goPoint);
			}*/

			Enemy_Printf(&which_ai, "Via %d->%d = %d\n", which_ai.getPathData().curNode,
				which_ai.getPathData().nextNode, which_ai.getPathData().finalNode);

			if(which_ai.getPathData().blocked)
			{	// can't go where I want to :(
				if(attack && which_ai.GetMove() && ClearShot(which_ai, monster, goalPos, path_goalentity, which_ai.GetMove()->bbox))
				{
					AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, monster.s.origin, goalPos, path_goalentity), monster.s.origin, goalPos, path_goalentity);
				}
				else
				{
					AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster,goalPos,goalPos), goalPos, goalPos);
				}
			}
			else
			{
				vec3_t toAvoid, toTarg, looker;

				VectorSubtract(which_ai.getPathData().goPoint, monster.s.origin, toAvoid);
				toAvoid[2] = 0;
				VectorNormalize(toAvoid);

				VectorScale(toAvoid, 10.0, actionGoal);
				VectorAdd(monster.s.origin, actionGoal, actionGoal);

				VectorSubtract(goalPos, monster.s.origin, toTarg);
				toTarg[2] = 0;
				float distToTarg = VectorNormalize(toTarg);

				if((nonIdealReason == NI_FLEEING)||(!fastStrafe)||(distToTarg > 448))
				{	// good for folks who can't strafe
					VectorScale(toAvoid, 500, looker);
					VectorAdd(monster.s.origin, looker, looker);

				}
				else
				{
					if(DotProduct(toAvoid, toTarg) < -.717)
					{
						if(!fastRetreat)
						{
							VectorScale(toAvoid, 4.0, actionGoal);
							VectorAdd(monster.s.origin, actionGoal, actionGoal);
						}

						VectorScale(toAvoid, -2440, looker);
						VectorAdd(monster.s.origin, looker, looker);
					}
					else if(DotProduct(toAvoid, toTarg) > .717)
					{
						VectorScale(toAvoid, 2440, looker);
						VectorAdd(monster.s.origin, looker, looker);
					}
					else
					{
						if(!fastStrafe)
						{
							VectorScale(toAvoid, 12.0, actionGoal);
							VectorAdd(monster.s.origin, actionGoal, actionGoal);
						}

						//set it to the side now - for strafing
						toAvoid[2] = toAvoid[1];
						toAvoid[1] = -toAvoid[0];
						toAvoid[0] = toAvoid[2];
						toAvoid[2] = 0;

						if(DotProduct(toAvoid, toTarg) > 0)
						{
							VectorScale(toAvoid, 2440, looker);
							VectorAdd(monster.s.origin, looker, looker);
						}
						else
						{
							VectorScale(toAvoid, -2440, looker);
							VectorAdd(monster.s.origin, looker, looker);
						}
					}
				}

				vec3_t dif;
				VectorSubtract(monster.s.origin, actionGoal, dif);
				dif[2] = 0;
				vec3_t dif2;
				VectorSubtract(monster.s.origin, goalPos, dif2);
				dif2[2] = 0;
				if((VectorLengthSquared(dif) > 2)&&((VectorLengthSquared(dif2) > (50*50))||((nonIdealReason == NI_FLEEING)))&&((!which_ai.GetMySkills()->testFlag(AIS_WONTADVANCE))||(nonIdealReason == NI_FLEEING)))
				{
					AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,looker), actionGoal, looker);
				}
				else
				{
					if(nonIdealReason == NI_FLEEING)
					{	//get down an' cower
						body_c *body = which_ai.GetBody();

						if (body)
						{
							mmove_t *myMove;

							myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_CROUCH, &generic_move_crouch_cower_mid);

							if(!myMove)
							{
								myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_CROUCH, &generic_move_crouch_cower_mid2);
							}

							if(myMove)
							{
								AddActionForSequence(which_ai, monster, myMove, goalPos, goalPos, path_goalentity);
							}
						}
					}
					else
					{
						if(attack && which_ai.GetMove() && ClearShot(which_ai, monster, goalPos, path_goalentity, which_ai.GetMove()->bbox))
						{	//can't move, but at least I can shoot! - fixme - some guys shouldn't do this
							AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, monster.s.origin, goalPos, path_goalentity), monster.s.origin, goalPos, path_goalentity);
						}
						else
						{
							AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster,actionGoal,goalPos), actionGoal, goalPos);
						}
					}
					//AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,looker), actionGoal, looker);
				}

				if(aipoints_show->value)
				{
					VectorScale(toAvoid, 40.0, actionGoal);
					VectorAdd(monster.s.origin, actionGoal, actionGoal);

					paletteRGBA_t	col = {250, 250, 250, 250};
					paletteRGBA_t	col2 = {0, 0, 0, 250};
					//FX_MakeLine(node[startNode].getPos(), node[nextNode].getPos(), col, 1);
					FX_MakeLine(which_ai.getPathData().goPoint, monster.s.origin, col, 1);
					FX_MakeRing(which_ai.getPathData().goPoint, 4);
					FX_MakeLine(actionGoal, monster.s.origin, col2, 1);
				}
			}
		}
		else
		{
			VectorSubtract(goalPos, monster.s.origin, actionGoal);
			VectorNormalize(actionGoal);
			VectorScale(actionGoal, 10.0, actionGoal);
			VectorAdd(monster.s.origin, actionGoal, actionGoal);
			AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,actionGoal), actionGoal, actionGoal);
		}
	}
}

void path_decision::AddAction(ai_c &which_ai, edict_t &monster)
{
	vec3_t gohere,moveVec;
	qboolean	isIdealPos;
	body_c		*the_body=which_ai.GetBody();

	//if i'm in midair, shouldn't bother with trying to walk to next point
	//this is just here for safety
	if (!monster.groundentity && monster.movetype != MOVETYPE_FLY)
	{
		AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster));
		return;
	}

	//have a target position, so go there--may have a path here!!!
	if (GetGoalPosition(which_ai,monster,gohere))
	{
		if (the_body && which_ai.HasTarget() && gmonster.GetWakeSoundTime()<level.time-10.0
			&& the_body->GetMove() && the_body->GetMove()->suggested_action != ACTCODE_PAIN)
		{
//			the_body->VoiceWakeSound(monster, 0.8);

			//eck! this is expensive!
//			PlayerNoise(which_ai.getTarget(), monster.s.origin, AI_SENSETYPE_SOUND_WAKEUP, NULL, 500);
		}

		//if i'm after a player or enemy monster, get mean! (but make sure they're on a different team than mine)
		if (path_goalentity && (path_goalentity->ai || path_goalentity->client) && IsAdversary(monster, path_goalentity))
		{
				if(which_ai.GetBody()&&which_ai.GetBody()->GetBestWeapon(monster)==ATK_NOTHING)
				{
					which_ai.Emote(monster, EMOTION_AFRAID, 5.0);//fixme--this decision is too general to do this here!!
				}
				else
				{
					which_ai.Emote(monster, EMOTION_MEAN, 5.0);//fixme--this decision is too general to do this here!!
				}
			which_ai.SetTargetTime(path_goal_updatedtime, path_goalentity, gohere);
		}
		GetMovementVector(which_ai, monster, gohere, moveVec);

//		Com_Printf("Go %f %f %f\n", moveVec[0], moveVec[1], moveVec[2]);

		isIdealPos=IsIdealPosition(which_ai, monster, monster.s.origin, gohere, moveVec);

		//if i have a path, go to the next point in it--may want to change this to Inch toward next path point
		if (path_nextpoint.valid)
		{
			//if my path point is for some special action, do it
			if (!isIdealPos && AddSpecialPathAction(which_ai, monster))
			{
				return;
			}
		}

		if (isIdealPos)
		{
			AddActionIdealPosition(which_ai, monster, gohere, moveVec);
		}
		else
		{
			AddActionNonIdealPosition(which_ai, monster, gohere, moveVec);
		}
	}
	//no goal position
	else
	{
		decision_c::Perform(which_ai, monster);
	}
}

//careful--used to be able to call this with no goal in mind, now it takes whatever goalpos
//yer plug in seriously
qboolean path_decision::EvaluatePathEndpoint(ai_c &which_ai, edict_t &monster,
											 ai_pathpoint_t endpoint_candidate, vec3_t goalpos)
{
	//make sure i have a path!
	if (!endpoint_candidate.valid)
	{
		return false;
	}
	
	//simple distance check, weighted heavier in z
	if (IsIdealPosition(which_ai, monster, endpoint_candidate.pos, goalpos, vec3_origin))
	{
		return true;
	}
	return false;
}

static bool made_puffs;

void path_think(edict_t *who)
{
	FX_MakeSparks(who->s.origin, vec3_up, 2);
	who->nextthink=level.time+0.5;
}

void path_think_2(edict_t *who)
{
	FX_MakeSparks(who->s.origin, vec3_up, 1);
	who->nextthink=level.time+0.5;
}


qboolean path_decision::NextPointInPath(ai_c &which_ai, edict_t &monster, vec3_t position)
{//this sets path_nextpoint

	//are we traveling a path with another point to spare? if so, grab that
	//next point.
	if (path_nextpoint.valid && level.time-path_nextpoint.time < 5)
	{
		qboolean	closeEnough = false;
		vec3_t dist;

		//consider me--haven't timed out yet
		if (level.time - path_nextpoint.time < 1.0)
		{
			VectorSubtract(position, path_nextpoint.pos, dist);
			dist[2]=0;
			closeEnough=VectorLengthSquared(dist)<2500;

			//still haven't hit the waypoint, keep goin at it
			if (!closeEnough)
			{
	//			gi.dprintf("NextPointInPath: moving to point\n");
				return true;
			}
		}

	}
//	gi.dprintf("NextPointInPath: path dried up\n");
	path_nextpoint.valid = false;
	return false;
}

void path_decision::TempPointCreate(vec3_t position, edict_t &monster)
{
	VectorCopy(position, path_nextpoint.pos);

//	if (ai_pathtest->value)
//	{
//		gi.dprintf("% creating temp point at time %f\n",monster.classname,level.time);
//	}
	path_nextpoint.valid = 1;
	path_nextpoint.time = level.time;
}

void path_decision::AvoidPointCreate(vec3_t normal, edict_t &monster)
{
	vec3_t	newdir,tempdir,right, curfacing;
	vec3_t	finalpos;
	float	yaw, whichway;

	if (path_nextpoint.valid && (level.time - path_nextpoint.time < 1.0))
	{
		return;
	}

//	gi.dprintf("warning! creating temp point!\n");
	//get the direction i was facing, so i know which side of the normal is more desirable
	yaw = monster.s.angles[YAW] * M_PI * 2 / 360;//fixme: don't know this is direction i was moving
	
	curfacing[0] = cos(yaw);
	curfacing[1] = sin(yaw);
	curfacing[2] = 0;

	//find a vector parallel to plane normal
	//Fixme?--this doesn't work in 3d, but cheap & shouldn't be a problem for walking creatures
	//er, could zero out normal's z, then normalize (making sure length n/ 0)
	right[0] = normal[1];
	right[1] = normal[0]*-1.0;
	right[2] = normal[2];

	//this random stuff makes it more likely i'll try to go with the grain of whatever i hit

	if ((path_nextpoint.valid)||(level.time-path_fail_time<PATH_FAILURE_REFAIL_DELAY))
	{
//		whichway=0;

		whichway = gi.flrand(-1.0F, 1.0F);
		whichway = whichway*whichway*whichway*whichway;
		if (!(rand()&3)&&(level.time-path_fail_time<PATH_FAILURE_REFAIL_DELAY))
		{
			VectorCopy(path_fail_dir,curfacing);
		}
	}
	else

	{
		whichway = gi.flrand(0.0F, 2.0F);
		whichway = 1.0-(whichway/**whichway*0.5*/);
	}

	//random check that'll usually choose to go to the same side of the normal
	//that i was already going, but not always (so i'll Eventually get out of any
	//corner :P )
	if (DotProduct (curfacing, right) <= whichway)//don't turn around now
	{
		right[0] *= -1.0;
		right[1] *= -1.0;
	}

	//go 40 units parallel to normal, and 10 with the normal
	VectorScale(right,VectorLength(monster.ai->velocity)+30,newdir);
	VectorScale(normal,(VectorLength(monster.ai->velocity)+30)*0.5,tempdir);//back up a bit too!
	VectorAdd(monster.s.origin,newdir,newdir);
	VectorAdd(tempdir,newdir,finalpos);

	//spawn an edict for tempgoal usage
	TempPointCreate(finalpos,monster);
	//make sure spawn was successful
	path_nextpoint.temp_type=0;//code for ordinary tempgoal
}

void path_decision::PoliteAvoidPointCreate(vec3_t normal, edict_t &monster)
{
	vec3_t	tempdir;
	vec3_t	finalpos;

	if (path_nextpoint.valid && (level.time - path_nextpoint.time < 1.0))
	{
		return;
	}

	//go 70 units with the normal
	VectorScale(normal,70,tempdir);//back up a bit too!
	VectorAdd(monster.s.origin,tempdir,finalpos);

	//spawn an edict for tempgoal usage
	TempPointCreate(finalpos,monster);
	//make sure spawn was successful
	path_nextpoint.temp_type=0;//code for ordinary tempgoal
}

void path_decision::JumpPointCreate(vec3_t position, edict_t &monster)
{
//	gi.dprintf("warning! creating jump point!\n");
	TempPointCreate(position,monster);
	path_nextpoint.temp_type=1;//code for jumpgoal
}

void path_decision::FallPointCreate(vec3_t position, edict_t &monster)
{
//	gi.dprintf("warning! creating fall point!\n");
	TempPointCreate(position,monster);
	path_nextpoint.temp_type=2;//code for fallgoal
}


void path_decision::UsePointCreate(brushUseInfo_t &useeInfo, edict_t &monster)
{
//	gi.dprintf("warning! creating fall point!\n");
	TempPointCreate(useeInfo.usePos,monster);
	VectorCopy(useeInfo.useDir, path_nextpoint.direction);
	path_nextpoint.targetEnt=useeInfo.useEnt;
	path_nextpoint.temp_type=3;//code for usegoal
}

void path_decision::PerformAvoidance(ai_c &which_ai, edict_t &monster)
{
	vec3_t		normal, my_facing;

	vec3_t		endorg;
	vec3_t		checkorg;
	trace_t		walktrace;
	float		yaw;
	bool		avoided = false;

	VectorClear(aimWanderVec);

	//i'm in the air--can't really do any avoidance
	if (!monster.groundentity && monster.movetype != MOVETYPE_FLY)
	{
		return;
	}

	yaw = monster.s.angles[YAW] * M_PI * 2 / 360;//fixme: don't know this is direction i was moving
	
	//use my facing direction
	endorg[0] = cos(yaw)*25;
	endorg[1] = sin(yaw)*25;
	endorg[2] = 0;

	// try the move	
	VectorCopy (monster.s.origin, checkorg);
	VectorScale (endorg, 0.04, my_facing);
	VectorAdd (checkorg, endorg, endorg);

	//do another trace to get info on which way to go to avoid obstacle
	gi.trace (checkorg, monster.mins, monster.maxs, endorg, &monster, MASK_MONSTERSOLID, &walktrace);

	checkorg[2]+=which_ai.GetJumpHeight();
	endorg[2]+=which_ai.GetJumpHeight();


	//no jumping or falling here, bub!

	//hmm... if trace was successful, something's screwy--use monster's angles reversed as the normal for a temp point
	if (walktrace.fraction == 1)
	{
		AngleVectors(monster.s.angles,normal,NULL,NULL);
		VectorScale(normal,-1.0,normal);
		AvoidPointCreate(normal, monster);
	}

	//if trace hit architecture or entity unlike monster, do temppoint avoidance for sure
	//fixme: check if ent with ai is good guy or bad guy
	else if (walktrace.ent == path_goalentity)
	{
//		NextPointInPath(which_ai, monster, monster.s.origin);
	}
	else if ((!walktrace.ent) || (!walktrace.ent->ai))
	{
		//open doors in the way--fixme--sophisticate this, add special action.
		if (walktrace.ent && IsUseableBrush(walktrace.ent))
		{
			if (IsBrushUseableNow(walktrace.ent, level.time, &monster))
			{
				brushUseInfo_t useInfo;
				if (GetUseableBrushInfo(walktrace.plane.normal, walktrace.ent, useInfo, &monster))
				{
					UsePointCreate(useInfo, monster);
				}
			}
			//it's useable--do a special avoid here, to not block up doorways etc.
			else
			{
				PoliteAvoidPointCreate(walktrace.plane.normal, monster);
			}
		}
		//not a useable brush--just avoid it
		else
		{
			AvoidPointCreate(walktrace.plane.normal, monster);
		}
	}
	//hit a monster--not sure if it's an ally or not

	else
	{
		float whichway;
		vec3_t other_facing;

		whichway = gi.flrand(0.0F, 2.0F);
		whichway = 1.0-whichway;

		//calculate direction other guy is facing
		AngleVectors(walktrace.ent->s.angles,other_facing,NULL,NULL);

		//this would skew results more in favor of not turning
//		whichway = 1.0-(whichway*whichway*0.5);

		//sometimes yield to higher ranked monsters, never yield to lower ranked guys
		//random check that'll usually ignore friendly collision if we're going the same direction
		if ((walktrace.ent->ai->GetRank() - monster.ai->GetRank() >= gi.flrand(-20.0F, 0.0F))
			&&(DotProduct (my_facing, other_facing) <= whichway))//facing each other
		{
			vec3_t edictnorm;

			//instead of using trace norm, calc a more natural-looking norm using the position of the 2 edicts
			VectorSubtract(monster.s.origin, walktrace.ent->s.origin, edictnorm);
			edictnorm[0]+=gi.flrand(-15.0F, 15.0F);
			edictnorm[1]+=gi.flrand(-15.0F, 15.0F);
			if (!VectorCompare(edictnorm,vec3_origin))
			{
				VectorNormalize(edictnorm);

				AvoidPointCreate(edictnorm, monster);
			}
		}
		//going same way as chum--do nothing? need to stall for time, right?
		else
		{
//			NextPointInPath(which_ai, monster, monster.s.origin);
		}
	}

}


void path_decision::ActionCompleted(action_c &which_action, ai_c &which_ai, edict_t &monster, float percent_success)
{//go to next node in path, or set up temp goal for avoidance

	if (percent_success > 0.25)
	{
		NextPointInPath(which_ai, monster, monster.s.origin);
	}
	//action completed unsuccessfully--set up temp waypoint for avoidance
	else
	{
		PerformAvoidance(which_ai, monster);
	}

	if (which_action.Type() == atype_attack)//update validity of attacking here.
	{
		return;
	}

	//based on success of this last action, update how valid this decision is.
	UpdateValidity(percent_success);
}

path_decision::path_decision(path_decision *orig)
: decision_c(orig)
{
	last_jump_time = orig->last_jump_time;
	path_fail_time = orig->path_fail_time;
	VectorCopy(orig->path_fail_dir, path_fail_dir);

	path_nextpoint = orig->path_nextpoint;
	*(int *)&path_nextpoint.targetEnt = GetEdictNum(orig->path_nextpoint.targetEnt);

	*(int *)&path_goalentity = GetEdictNum(orig->path_goalentity);
	VectorCopy(orig->path_goalpos, path_goalpos);
	path_goal_updatedtime = orig->path_goal_updatedtime;

	VectorCopy(orig->newMoveDir, newMoveDir);
	lastCheckTime = orig->lastCheckTime;
	lastDuckInvalidateTime = orig->lastDuckInvalidateTime;
	lastDuckValidTime = orig->lastDuckValidTime;

	VectorCopy(orig->aimWanderVec, aimWanderVec);

	stuckNonIdeal = orig->stuckNonIdeal;
	nonIdealReason = orig->nonIdealReason;
	blockingEnemy = orig->blockingEnemy;
}

void path_decision::Evaluate(path_decision *orig)
{
	last_jump_time = orig->last_jump_time;
	path_fail_time = orig->path_fail_time;
	VectorCopy(orig->path_fail_dir, path_fail_dir);

	path_nextpoint = orig->path_nextpoint;
	path_nextpoint.targetEnt = GetEdictPtr((int)orig->path_nextpoint.targetEnt);

	path_goalentity = GetEdictPtr((int)orig->path_goalentity);

	VectorCopy(orig->path_goalpos, path_goalpos);
	path_goal_updatedtime = orig->path_goal_updatedtime;

	VectorCopy(orig->newMoveDir, newMoveDir);
	lastCheckTime = orig->lastCheckTime;
	lastDuckInvalidateTime = orig->lastDuckInvalidateTime;
	lastDuckValidTime = orig->lastDuckValidTime;

	VectorCopy(orig->aimWanderVec, aimWanderVec);

	stuckNonIdeal = orig->stuckNonIdeal;
	nonIdealReason = orig->nonIdealReason;
	blockingEnemy = orig->blockingEnemy;

	decision_c::Evaluate(orig);
}

/**********************************************************************************
 **********************************************************************************/

pathcorner_decision::pathcorner_decision(edict_t *goalent, int priority_root, float timeout, edict_t *ScriptOwner)
: path_decision(goalent, priority_root, timeout, ScriptOwner)
{
	curWaitActionNum = 1;
	reactivate_time = 0.0f;
	oldTarget = NULL;
	firstActionAtWait = true;
	curMoveAction = NULL;
	isAtCorner = false;
}

void pathcorner_decision::GetMovementVector(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	float distToGoalPos;
	VectorSubtract(goalPos, monster.s.origin, newMoveDir);
	distToGoalPos=VectorNormalize(newMoveDir);
	if (distToGoalPos>3)
	{
		VectorScale(newMoveDir, 3, moveVec);
	}
	else
	{
		VectorScale(newMoveDir, distToGoalPos, moveVec);
	}
}

mmove_t *GetWaitAction(edict_t *self, int actionNum);
mmove_t *GetMoveAction(edict_t *self);

void pathcorner_decision::FinishPathPoint(ai_c &which_ai, edict_t &monster)
{
	edict_t *oldCorner=path_goalentity;
	curWaitActionNum=1;
//	while(path.path->GetPoint(path_nextpoint.point)){}
	path_nextpoint.valid = 0;
	isAtCorner=false;
	if (!path_goalentity)
	{
		return;
	}
	if (ai_pathtest->value)
	{
		gi.dprintf("Finishing pathpoint: time %f\n",level.time);
	}
	if (ai_pathtest->value)
	{
		gi.dprintf("%d getting next point: old--%s, looking for %s!\n",monster.s.number,path_goalentity->targetname,path_goalentity->target);
	}
	mmove_t	*tMove=GetMoveAction(path_goalentity);
	if (tMove)
	{
		curMoveAction=tMove;
	}
	//use the path_corner's pathtarget
	if (path_goalentity->pathtarget)
	{
		char *savetarget;
		sensedEntInfo_t	my_client;

		savetarget = path_goalentity->target;
		path_goalentity->target = path_goalentity->pathtarget;
		which_ai.GetSensedClientInfo(smask_all, my_client);
		//use any client i know about as activator
		if (my_client.ent)
		{
			G_UseTargets (path_goalentity, my_client.ent);
		}
		else
		{
			G_UseTargets (path_goalentity, &monster);
		}
		path_goalentity->target = savetarget;
	}

	if (path_goalentity->target)
	{
		path_goalentity=G_PickTarget(path_goalentity->target);
	}
	else
	{
		path_goalentity = NULL;
	}
	if (path_goalentity)
	{
		VectorSubtract(path_goalentity->s.origin, monster.s.origin, path_fail_dir);
		VectorNormalize(path_fail_dir);
	}
	else
	{
		//now check to see if i should hold my position here.
		if (oldCorner && (oldCorner->spawnflags&1) && stricmp(oldCorner->classname, "path_corner"))
		{
			monster.spawnflags|=SPAWNFLAG_HOLD_POSITION;
			monster.flags |= FL_NO_KNOCKBACK;
		}
	}
}

void pathcorner_decision::ActionCompleted(action_c &which_action, ai_c &which_ai, edict_t &monster, float percent_success)
{//go to next node in path, or set up temp goal for avoidance
	if (which_action.Type() == atype_attack)//update validity of attacking here.
	{
		return;
	}

	//based on success of this last action, update how valid this decision is.
	UpdateValidity(percent_success);
	
	//action completed successfully--go to next waypoint (no matter if i'm not following a path)
	if (path_goalentity)
	{
		if (isAtCorner)
		{
			if (!path_nextpoint.valid)
			{
				//check if i have waitactions left, if point has spawnflag 2 on that means i'm finished
				body_c	*body=which_ai.GetBody();
				mmove_t *newmove=GetWaitAction(path_goalentity, curWaitActionNum);

				//passed the end of the wait action list
				if (!newmove)
				{
					//run thru anims once only
					if (path_goalentity->spawnflags&2)
					{
						reactivate_time = level.time-1;
					}
				}

				if (reactivate_time <= level.time)
				{
					FinishPathPoint(which_ai, monster);
				}
			}
		}
		else
		{
			if (percent_success > 0.25)
			{
				NextPointInPath(which_ai, monster, monster.s.origin);
			}
			//action completed unsuccessfully--set up temp waypoint for avoidance
			else
			{
				PerformAvoidance(which_ai, monster);
			}
		}
	}
}

qboolean pathcorner_decision::IsIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t curPos, vec3_t goalPos, vec3_t moveVec)
{
	if(ai_dumb->value)
	{//show folks what we are up to
		fxRunner.exec("edit/ignore", &monster);
	}

	vec3_t toGoal;
	float distToGoalSq;
	VectorSubtract(goalPos, curPos, toGoal);
	distToGoalSq=VectorLengthSquared(toGoal);
	//am i closer than 30 from goal position
	if( isAtCorner || (distToGoalSq < 2500 && distToGoalSq > 2) )
	{
//		trace_t tr;
		vec3_t toCur;
		//test to see if i can shoot goalpos--should i be able to get this info from sense somehow?
		VectorSubtract(curPos, monster.s.origin, toCur);
//		if(toCur[0]*toCur[0]+toCur[1]*toCur[1]+toCur[2]*toCur[2] < 100)
//		{
//			gi.trace(monster.s.origin,NULL,NULL,goalPos,&monster,MASK_SHOT, &tr);
//			if (tr.fraction>0.95||tr.ent==path_goalentity)
//			{
				if (path_goalentity && oldTarget != path_goalentity)
				{
					if (ai_pathtest->value)
					{
						gi.dprintf("waiting: time %f\n",level.time);
					}
					oldTarget=path_goalentity;
					if (path_goalentity->wait == -1 || (path_goalentity->spawnflags&2))
					{
						reactivate_time=level.time+9999999999;
					}
					else
					{
						reactivate_time=level.time+path_goalentity->wait;
					}
					firstActionAtWait=true;
					isAtCorner=true;

					if (path_goalentity->spawnflags&4)
					{
						which_ai.SetConcentratingOnPlayer(true);
						monster.spawnflags|=SPAWNFLAG_HOLD_POSITION;
						monster.flags |= FL_NO_KNOCKBACK;
						curMoveAction=NULL;
					}
				}
				return true;
//			}
//		}
//		else
//		{
//			return true;
//		}
	}
	return false;
}

void pathcorner_decision::AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	vec3_t actionGoal,tGoal;

	VectorCopy(moveVec, actionGoal);
	VectorAdd(monster.s.origin,actionGoal,actionGoal);
	VectorAdd(moveVec,monster.s.origin,tGoal);

	//if i have an animation in mind, only accept guys with the same actflags
	if (curMoveAction)
	{
		mmove_t	*newMove=GetSequenceForMovement(which_ai,monster,tGoal,tGoal,curMoveAction,~(curMoveAction->actionFlags));
		if (newMove != curMoveAction)
		{
			if (which_ai.GetBody() && which_ai.GetBody()->IsAvailableSequence(monster, curMoveAction))
			{
				gi.dprintf("path-traversal: couldn't set %s for %s %s (but it's available!), using %s!\n",
					curMoveAction->ghoulSeqName, monster.classname, monster.targetname, newMove->ghoulSeqName);
			}
			else
			{
				gi.dprintf("path-traversal: %s not available for %s %s, using %s!\n",
					curMoveAction->ghoulSeqName, monster.classname, monster.targetname, newMove->ghoulSeqName);
			}
		}
		AddActionForSequence(which_ai, monster, newMove, actionGoal, actionGoal);
	}
	//if i don't have some specific animation in mind, don't allow backing up or strafing
	else
	{
		AddActionForSequence(which_ai, monster, 
			GetSequenceForMovement(which_ai,monster,tGoal,tGoal,curMoveAction,ACTFLAG_BACKUP|ACTFLAG_LEFTSTRAFE|ACTFLAG_RIGHTSTRAFE), 
			actionGoal, actionGoal);
	}
}

void pathcorner_decision::AddActionIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	body_c	*body=which_ai.GetBody();
	mmove_t *newmove=NULL;
	vec3_t	forward;
	sensedEntInfo_t aClient;

	which_ai.GetSensedClientInfo(smask_all, aClient);
	if (aClient.ent && !OnSameTeam(aClient.ent, &monster))
	{
		which_ai.SetTargetTime(aClient.time, aClient.ent, aClient.pos);
	}

	if (path_goalentity)
	{
		AngleVectors(path_goalentity->s.angles, forward, NULL, NULL);
		VectorScale(forward, 10000, forward);
		VectorAdd(forward, monster.s.origin, forward);
	}
	else
	{
		VectorClear(forward);
	}

	if (!body)
	{
		gi.dprintf("path-traversal: no body!\n");
		which_ai.NewAction(which_ai.DefaultAction(this, NULL, GetSequenceForStand(which_ai,monster,goalPos,forward), goalPos, forward, NULL, 0, true), &monster);
		return;
	}

	if (!firstActionAtWait && body->HasAnimationHitEOS())
	{
		curWaitActionNum++;
	}

	newmove=GetWaitAction(path_goalentity, curWaitActionNum);

	//passed the end of the wait action list
	if (!newmove)
	{
		//run thru anims once only
		if (path_goalentity->spawnflags&2)
		{
			newmove=GetWaitAction(path_goalentity, 1);
		}
		else
		{
			curWaitActionNum=1;
			newmove=GetWaitAction(path_goalentity, curWaitActionNum);
		}
	}

	mmove_t *newStand=body->GetSequenceForStand(monster,goalPos,forward,ACTSUB_NORMAL,BBOX_PRESET_STAND,newmove);

	forward[2]+=MONSTER_SHOOT_HEIGHT;

	//if i want to be attacking here, try to aim at a client
	if (newmove && newmove->suggested_action==ACTCODE_ATTACK)
	{
		sensedEntInfo_t	sensed_client;
		which_ai.GetSensedClientInfo(smask_all, sensed_client);

		//only do it if i really saw a client, and s/he/it's really an enemy...
		if (sensed_client.ent && IsAdversary(monster, sensed_client.ent))
		{
			VectorCopy(sensed_client.pos, forward);
		}
	}

	if (newmove && newStand != newmove)
	{
		if (which_ai.GetBody() && which_ai.GetBody()->IsAvailableSequence(monster, newmove))
		{
			gi.dprintf("path-traversal: couldn't set %s for %s %s (but it's available!), using %s!\n",
				newmove->ghoulSeqName, monster.classname, monster.targetname, newStand->ghoulSeqName);
		}
		else
		{
			gi.dprintf("path-traversal: %s not available for %s %s, using %s!\n",
				newmove->ghoulSeqName, monster.classname, monster.targetname, newStand->ghoulSeqName);
		}
	}

	which_ai.NewAction(which_ai.DefaultAction(this, NULL, newStand, goalPos, forward, NULL, 0, true), &monster);

	firstActionAtWait=false;
}

void pathcorner_decision::SetGoalPosition(ai_c &which_ai, edict_t &monster)
{
	if (path_goalentity && path_goalentity->inuse)
	{
		if (IsIdealPosition(which_ai, monster, monster.s.origin, path_goalentity->s.origin, vec3_origin))
		{
			if (!path_goalentity->target&&reactivate_time <= level.time)
			{
				FinishPathPoint(which_ai, monster);
//				path_goalentity=NULL;
			}
			else if (path_goalentity->wait <= 0.1 && path_goalentity->wait >= -0.1 && !(path_goalentity->spawnflags&2))
			{
				FinishPathPoint(which_ai, monster);
			}
		}
		if (path_goalentity)
		{
			VectorCopy(path_goalentity->s.origin, path_goalpos);
		}
	}
}

qboolean pathcorner_decision::GetGoalPosition(ai_c &which_ai, edict_t &monster, vec3_t gohere)
{
	//copy ai's enemy_lastseen_pos into gohere vec
	if (gohere)//pass in NULL if just want to know whether we have goal position
	{
		VectorCopy(path_goalpos, gohere);
	}

	//check the decision's goalentity for whether i have goal pos, not ai's enemy
	if (path_goalentity)
	{
		//make sure goal entity is still valid
		if (!path_goalentity->inuse)
		{
			path_goalentity = NULL;
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}


qboolean pathcorner_decision::Consider(ai_c &which_ai, edict_t &monster)
{
#ifdef _DEBUG
	decName = "pathcornerdecision";
#endif

	//don't automatically do this?
	SetGoalPosition(which_ai, monster);

	//have a goal position
	if (GetGoalPosition(which_ai,monster,NULL))
	{
		priority = 1.1;
	}
	//no goal position--get rid of me
	else
	{
		priority = 1;
		return true;
	}

	UpdateValidity();

	//if decision can time out, and its time has expired, then let ai get rid of it
	return (IsTimedOut()||which_ai.HasTarget());
}

pathcorner_decision::pathcorner_decision(pathcorner_decision *orig)
: path_decision(orig)
{
	reactivate_time = orig->reactivate_time;
	*(int *)&oldTarget = GetEdictNum(orig->oldTarget);
	curWaitActionNum = orig->curWaitActionNum;
	firstActionAtWait = orig->firstActionAtWait;
	isAtCorner = orig->isAtCorner;
	*(int *)&curMoveAction = GetMmoveNum(orig->curMoveAction);
}

void pathcorner_decision::Evaluate(pathcorner_decision *orig)
{
	reactivate_time = orig->reactivate_time;
	oldTarget = GetEdictPtr((int)orig->oldTarget);
	curWaitActionNum = orig->curWaitActionNum;
	firstActionAtWait = orig->firstActionAtWait;
	isAtCorner = orig->isAtCorner;
	curMoveAction = GetMmovePtr((int)orig->curMoveAction);

	path_decision::Evaluate(orig);
}

/**********************************************************************************
 **********************************************************************************/

void pathcombat_decision::FinishPathPoint(ai_c &which_ai, edict_t &monster)
{
	edict_t *oldGoal=path_goalentity;

	pathcorner_decision::FinishPathPoint(which_ai, monster);
	
	//now check to see if i should hold my position here.
	if (oldGoal && (oldGoal->spawnflags&1))
	{
		monster.spawnflags|=SPAWNFLAG_HOLD_POSITION;
		monster.flags |= FL_NO_KNOCKBACK;
	}
}

void pathcombat_decision::GetMovementVector(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	float distToGoalPos;
	VectorSubtract(goalPos, monster.s.origin, newMoveDir);
	distToGoalPos=VectorNormalize(newMoveDir);
	if (distToGoalPos>VectorLength(which_ai.velocity)+15)
	{
		VectorScale(newMoveDir, VectorLength(which_ai.velocity)+15, moveVec);
	}
	else
	{
		VectorScale(newMoveDir, distToGoalPos, moveVec);
	}
	if (path_nextpoint.valid && level.time-path_nextpoint.time<1.0)
	{
		VectorSubtract(path_nextpoint.pos, monster.s.origin, moveVec);
		RecalcMoveVec(which_ai, monster, path_nextpoint.pos, moveVec);
	}
}

qboolean pathcombat_decision::Consider(ai_c &which_ai, edict_t &monster)
{
#ifdef _DEBUG
	decName = "pathcombatdecision";
#endif

	//don't automatically do this?
	SetGoalPosition(which_ai, monster);

//	AcquirePath(which_ai, monster);

	//have a goal position
	if (GetGoalPosition(which_ai,monster,NULL))
	{
		if (which_ai.HasHadTarget())
		{
			priority = 5;
		}
		else
		{
			priority = 0;
		}
	}
	//no goal position
	else
	{
		priority = 0;
		return true;
	}

	UpdateValidity();

	//if decision can time out, and its time has expired, then let ai get rid of it
	return IsTimedOut();
}

pathcombat_decision::pathcombat_decision(pathcombat_decision *orig)
: pathcorner_decision(orig)
{
}

void pathcombat_decision::Evaluate(pathcombat_decision *orig)
{
	pathcorner_decision::Evaluate(orig);
}

void pathcombat_decision::Read()
{
	char	loaded[sizeof(pathcombat_decision)];

	gi.ReadFromSavegame('AIPD', loaded, sizeof(pathcombat_decision));
	Evaluate((pathcombat_decision *)loaded);
}

void pathcombat_decision::Write()
{
	pathcombat_decision	*savable;

	savable = new pathcombat_decision(this);
	gi.AppendToSavegame('AIPD', savable, sizeof(*this));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/
/*
void pointcombat_decision::AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	vec3_t actionGoal,tGoal;
	float moveDist;
	float moveSpeed=VectorLength(which_ai.velocity)+5;

	//override moveVec, achtung!! ignoring pathfinding stuff!!
//	VectorSubtract(goalPos, monster.s.origin, moveVec);

//	if (moveSpeed > 10)
//	{
//		moveSpeed = 10;
//	}
//	if (ai_pathtest->value)
//	{
//		gi.dprintf("nonideal adding action : goalpos %s, path_goalpos %s.\n",vtos(goalPos),vtos(path_goalpos));
//	}

	//override whatever moveVec was set as.
//	VectorSubtract(goalPos, monster.s.origin, moveVec);
	moveDist = VectorNormalize(moveVec);
	//scale my moveVec to be reasonable.
//	if (moveSpeed > moveDist)
//	{
//		moveSpeed = moveDist;
//	}
	VectorScale(moveVec, moveSpeed, actionGoal);
	VectorAdd(monster.s.origin,actionGoal,actionGoal);
	VectorAdd(moveVec,monster.s.origin,tGoal);
	AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,tGoal,tGoal), actionGoal, actionGoal);
//	which_ai.NewAction(which_ai.WalkAction(this, NULL, GetSequenceForMovement(which_ai,monster,tGoal,tGoal), actionGoal, actionGoal, 0.2), &monster);
}
*/
pointcombat_decision::pointcombat_decision(edict_t *goalent, int priority_root, float timeout, edict_t *ScriptOwner)
: path_decision(goalent, priority_root, timeout, ScriptOwner)
{
	current_point = 0;
	VectorClear(current_point_pos);
	VectorClear(current_point_dest);
	current_point_type = 0;
	current_point_subtype = 0;
	last_point_trans = 0;
	last_consider_time = 0;
	VectorClear(current_point_dir);
}

qboolean pointcombat_decision::IsIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t curPos, vec3_t goalPos, vec3_t moveVec)
{
	vec3_t toGoal;
	float distToGoalSq;
	VectorSubtract(goalPos, curPos, toGoal);
	distToGoalSq=VectorLengthSquared(toGoal);

	//am i closer than 50 from combat point
	//if(distToGoalSq < 2500 && distToGoalSq > 2)
	if(distToGoalSq < 400 && distToGoalSq > 2)//closer than 20
	{
		return true;
	}
	return false;
}

int pointBetweenSpheres(vec3_t start, float startrad, vec3_t end, float endrad, vec3_t testpoint);

void pointcombat_decision::GetMovementVector(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	// these are extremely specific - just go straight to it.

	vec3_t targ;

	stuckNonIdeal = false;

	if(aiPoints.isActive())
	{
		float rad = aiPoints.getNode(which_ai.getPathData().curNode)->getRadius();

		if(rad < 40)
		{
			rad = 40;
		}

		// it's only safe to go once I'm between these two points
		// otherwise, head to my node first
		if(pointBetweenSpheres(aiPoints.getNode(which_ai.getPathData().curNode)->getPos(), rad,
			goalPos, 30, monster.s.origin))
		{
			VectorCopy(goalPos, targ);
		}
		else
		{
			VectorCopy(aiPoints.getNode(which_ai.getPathData().curNode)->getPos(), targ);
		}
		/*if(aipoints_show->value)
		{
			paletteRGBA_t	col = {250, 0, 0, 250};
			//FX_MakeLine(node[startNode].getPos(), node[nextNode].getPos(), col, 1);
			FX_MakeLine(targ, monster.s.origin, col, 1);
			FX_MakeRing(targ, 12);
		}*/
	}
	else
	{
		VectorCopy(goalPos, targ);
	}

	RecalcMoveVec(which_ai, monster, targ, moveVec);
	lastCheckTime = level.time;

	VectorScale(newMoveDir, VectorLength(which_ai.velocity)+10, moveVec);
}


void pointcombat_decision::AddActionIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	body_c *body = which_ai.GetBody();
/*
	//do specialized attacking for combat point here!
	//if (current_point != NO_COMBAT_POINT)
	if (which_ai.getPathData().currentPointStyle)
	{
		//if (current_point_type == EWI_POINT_DUCK)
		if (which_ai.getPathData().currentPointStyle == EPS_DUCK)
		{
//			gi.dprintf("duck!\n");
			AddActionForSequence(which_ai, monster, body->GetSequenceForAttack(monster, path_goalpos, path_goalpos, path_goalentity, ACTSUB_NORMAL, BBOX_PRESET_CROUCH), path_goalpos, path_goalpos, path_goalentity);
			return;
		}
		//else if (current_point_type == EWI_POINT_LEAN)
		else if (which_ai.getPathData().currentPointStyle == EPS_LEAN)
		{	// nathan fixme - which side is buddy leaning on now?
			//fixme: use current_point_pos here somehow
			if (last_point_trans > level.time - gi.flrand(3.0, 6.0))
			{
				vec3_t dif;
				VectorScale(current_point_dir, -1, dif);
				vectoangles(dif, monster.s.angles);//!!!!!!!

				if(which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_wallidle_pk))
				{
					which_ai.NewAction(which_ai.DefaultAction(this, NULL, GetSequenceForStand(which_ai, monster,path_goalpos,path_goalpos, &generic_move_wallidle_pk), path_goalpos, path_goalpos, NULL, 0, true), &monster);
					return;
				}
				if(which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_wallidle_mrs))
				{
					which_ai.NewAction(which_ai.DefaultAction(this, NULL, GetSequenceForStand(which_ai, monster,path_goalpos,path_goalpos, &generic_move_wallidle_mrs), path_goalpos, path_goalpos, NULL, 0, true), &monster);
					return;
				}

				which_ai.NewAction(which_ai.DefaultAction(this, NULL, GetSequenceForStand(which_ai, monster,path_goalpos,path_goalpos), path_goalpos, path_goalpos, NULL, 0, true), &monster);
			}
			else
			{
				vec3_t dif;
				VectorScale(current_point_dir, -1, dif);
				vectoangles(dif, monster.s.angles);//!!!!!!!
				last_point_trans = level.time;
//				VectorCopy(which_ai.getPathData().combatPoint, monster.s.origin);
				if (!(which_ai.getPathData().combatPointLeft))
				{
					if (which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_fcornerr_m2))
					{
						AddActionForSequence(which_ai, monster, body->GetSequenceForAttack(monster, monster.s.origin, path_goalpos, path_goalentity, ACTSUB_NORMAL, BBOX_PRESET_STAND,&generic_move_fcornerr_m2), path_goalpos, path_goalpos, path_goalentity);
						return;
					}
					if (which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_fcornerr_p))
					{
						AddActionForSequence(which_ai, monster, body->GetSequenceForAttack(monster, monster.s.origin, path_goalpos, path_goalentity, ACTSUB_NORMAL, BBOX_PRESET_STAND,&generic_move_fcornerr_p), path_goalpos, path_goalpos, path_goalentity);
						return;
					}
				}
				else
				{
					if (which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_fcornerl_m2))
					{
						AddActionForSequence(which_ai, monster, body->GetSequenceForAttack(monster, monster.s.origin, path_goalpos, path_goalentity, ACTSUB_NORMAL, BBOX_PRESET_STAND,&generic_move_fcornerl_m2), path_goalpos, path_goalpos, path_goalentity);
						return;
					}
					if (which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_fcornerl_p))
					{
						AddActionForSequence(which_ai, monster, body->GetSequenceForAttack(monster, monster.s.origin, path_goalpos, path_goalentity, ACTSUB_NORMAL, BBOX_PRESET_STAND,&generic_move_fcornerl_p), path_goalpos, path_goalpos, path_goalentity);
						return;
					}
				}

				gi.dprintf("couldn't run corner-peeking animation!\n");
				AddActionForSequence(which_ai, monster, body->GetSequenceForAttack(monster, monster.s.origin, path_goalpos, path_goalentity, ACTSUB_NORMAL, BBOX_PRESET_STAND), path_goalpos, path_goalpos, path_goalentity);
			}
			return;
		}
		else if (which_ai.getPathData().currentPointStyle == EPS_COVER)
		//else if (current_point_type == EWI_POINT_COVER)
		{
			if(body->GetMove()->bbox == BBOX_PRESET_CROUCH)
			{	//what to do when ducking
				if(last_point_trans > level.time - gi.flrand(4.0, 6.0))
				{
					AddActionForSequence(which_ai, monster, body->GetSequenceForStand(monster, monster.s.origin, path_goalpos, ACTSUB_NORMAL, BBOX_PRESET_CROUCH), path_goalpos, path_goalpos, path_goalentity);
				}
				else
				{
					last_point_trans = level.time;
					AddActionForSequence(which_ai, monster, body->GetSequenceForAttack(monster, monster.s.origin, path_goalpos, path_goalentity, ACTSUB_NORMAL, BBOX_PRESET_STAND), path_goalpos, path_goalpos, path_goalentity);
				}
			}
			else
			{	//what to do when standing
				if(last_point_trans > level.time - gi.flrand(4.0, 6.0))
				{
					AddActionForSequence(which_ai, monster, body->GetSequenceForAttack(monster, monster.s.origin, path_goalpos, path_goalentity, ACTSUB_NORMAL, BBOX_PRESET_STAND), path_goalpos, path_goalpos, path_goalentity);
				}
				else
				{
					last_point_trans = level.time;
					AddActionForSequence(which_ai, monster, body->GetSequenceForStand(monster, monster.s.origin, path_goalpos, ACTSUB_NORMAL, BBOX_PRESET_CROUCH), path_goalpos, path_goalpos, path_goalentity);
				}

			}
			
			return;
			//AddActionForSequence(which_ai, monster, body->GetSequenceForAttack(which_ai, monster, path_goalpos, path_goalpos, path_goalentity, ACTSUB_NORMAL, BBOX_PRESET_CROUCH), path_goalpos, path_goalpos, path_goalentity);
		}
	}
	gi.dprintf("unknown combat point type!\n");
	which_ai.NewAction(which_ai.DefaultAction(this, NULL, GetSequenceForStand(which_ai, monster,goalPos,goalPos), goalPos, goalPos, NULL, 0, true), &monster);*/
}

void pointcombat_decision::SetGoalPosition(ai_c &which_ai, edict_t &monster)
{
/*	path_decision::SetGoalPosition(which_ai, monster);
	if (path_goalentity && path_goalentity->inuse)
	{
		which_ai.getPathData().combatPointsAllowed = EPS_DIRECTED;

		//if i don't have lean anims, don't look for a lean point...
		if (which_ai.GetBody() && !which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_fcornerl_m2)
			&&!which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_fcornerl_p))
		{
			which_ai.getPathData().combatPointsAllowed &= ~EPS_LEAN;
		}

		aiPoints.getCombatPoint(&which_ai.getPathData(), monster.s.origin, path_goalentity->s.origin, current_point_dir);
	}*/
}

qboolean pointcombat_decision::GetGoalPosition(ai_c &which_ai, edict_t &monster, vec3_t gohere)
{
	//copy ai's enemy_lastseen_pos into gohere vec
	if (gohere)//pass in NULL if just want to know whether we have goal position
	{
		//VectorCopy(current_point_dest, gohere);
//		VectorCopy(which_ai.getPathData().combatPoint, gohere);
	}

	//check the decision's goalentity for whether i have goal pos, not ai's enemy
	if (path_goalentity)
	{
		//make sure goal entity is still valid
		if ((!path_goalentity->client && !path_goalentity->ai) || !path_goalentity->inuse
			|| path_goalentity->health <= 0 || level.time-path_goal_updatedtime>POINTCOMBAT_DECISION_LOSETARGETTIME
			|| (which_ai.GetBody()&&which_ai.GetBody()->GetBestWeapon(monster)==ATK_NOTHING))
		{
			path_goalentity = NULL;
			return false;
		}
		//return current_point!=NO_COMBAT_POINT;
		return false;
//		return which_ai.getPathData().currentPointStyle;
	}
	else
	{
		return false;
	}
}


qboolean pointcombat_decision::Consider(ai_c &which_ai, edict_t &monster)
{
#ifdef _DEBUG
	decName = "pointcombatdecision";
#endif

//	AcquirePath(which_ai, monster);

	// put these back in if I ever get them working =/
	priority = .1;
	return IsTimedOut();

	if ((which_ai.GetBody())&&(which_ai.GetBody()->IsMovementImpeded()))
	{
		priority = .1;
	}
	else if(monster.spawnflags & SPAWNFLAG_HOLD_POSITION)
	{	// um, these guys can't use this...
		priority = .1;
	}
	else
	{
		//don't automatically do this?
		SetGoalPosition(which_ai, monster);

		if (GetGoalPosition(which_ai,monster,NULL))
		{
			priority = 5.0;
		}
		//no goal position--no priority
		else
		{
			priority = 0.1;
		}
	}

	UpdateValidity();

	//if decision can time out, and its time has expired, then let ai get rid of it
	return IsTimedOut();
}

pointcombat_decision::pointcombat_decision(pointcombat_decision *orig)
: path_decision(orig)
{
	current_point = orig->current_point;
	VectorCopy(orig->current_point_pos, current_point_pos);
	VectorCopy(orig->current_point_dest, current_point_dest);
	current_point_type = orig->current_point_type;
	current_point_subtype = orig->current_point_subtype;
	last_point_trans = orig->last_point_trans;
	last_consider_time = orig->last_consider_time;
	VectorCopy(orig->current_point_dir, current_point_dir);
}

void pointcombat_decision::Evaluate(pointcombat_decision *orig)
{
	current_point = orig->current_point;
	VectorCopy(orig->current_point_pos, current_point_pos);
	VectorCopy(orig->current_point_dest, current_point_dest);
	current_point_type = orig->current_point_type;
	current_point_subtype = orig->current_point_subtype;
	last_point_trans = orig->last_point_trans;
	last_consider_time = orig->last_consider_time;

	path_decision::Evaluate(orig);
}

void pointcombat_decision::Read()
{
	char	loaded[sizeof(pointcombat_decision)];

	gi.ReadFromSavegame('AIOD', loaded, sizeof(pointcombat_decision));
	Evaluate((pointcombat_decision *)loaded);
}

void pointcombat_decision::Write()
{
	pointcombat_decision	*savable;

	savable = new pointcombat_decision(this);
	gi.AppendToSavegame('AIOD', savable, sizeof(*this));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/

search_decision::search_decision(edict_t *goalent, int priority_root, float timeout)
: path_decision(goalent, priority_root, timeout)
{
	approach_search = true;
	pursuit_search = true;
	VectorClear(pursuit_dir);
	last_fired_time = 0;
	VectorClear(firedSpot);
	VectorClear(trail1);
	VectorClear(trail2);
	VectorClear(trail3);
}
void search_decision::ActionCompleted(action_c &which_action, ai_c &which_ai, edict_t &monster, float percent_success)
{
	path_decision::ActionCompleted(which_action, which_ai, monster, percent_success);

	//if i hit something and i'm not going to last seen pos, cancel going in same direction that target was last seen going
	if (percent_success < 0.25 && !approach_search)
	{
		pursuit_search = false;
	}
}

void search_decision::SetGoalPosition(ai_c &which_ai, edict_t &monster)
{
	path_decision::SetGoalPosition(which_ai, monster);

	vec3_t dif;
	
	VectorSubtract(path_goalpos, trail1, dif);
	if(VectorLengthSquared(dif) > (8*8))
	{
		VectorCopy(trail2, trail3);
		VectorCopy(trail1, trail2);
		VectorCopy(path_goalpos, trail1);
	}

	//i see my target; make sure i'm set up for approaching
	if (level.time-path_goal_updatedtime<0.2)
	{
		approach_search = true;
		pursuit_search = true;
	}
	//cheating time is now! update which direction my target went since i last saw him for a while
	else if (path_goalentity && level.time-path_goal_updatedtime<1.0)
	{
//		VectorSubtract(path_goalentity->s.origin, path_goalpos, pursuit_dir);
//		VectorNormalize(pursuit_dir);
	}
}

qboolean search_decision::GetGoalPosition(ai_c &which_ai, edict_t &monster, vec3_t gohere)
{
//	vec3_t forward;
//	trace_t tr;

	//copy ai's enemy_lastseen_pos into gohere vec
	if (gohere)//pass in NULL if just want to know whether we have goal position
	{
		approach_search = false;
		if (path_goalentity)
		{
			which_ai.getTargetPos(gohere);
		}
	}

	//check the decision's goalentity for whether i have goal pos, not ai's enemy
	if (path_goalentity)
	{
		//make sure goal entity is still valid
		if (!path_goalentity->inuse || path_goalentity->health <= 0 || level.time-path_goal_updatedtime>90)
		{
			path_goalentity = NULL;
			validity=0;
			return false;
		}
		validity=0.2;
		return true;
	}
	else
	{
		validity=0;
		return false;
	}
}

qboolean search_decision::IsIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t curPos, vec3_t goalPos, vec3_t moveVec)
{
	return false;
}

#define CHEAT_SHOOT_TIME 4.0
#define CHEATING_TIME 10.0

void search_decision::AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	if(which_ai.BeingWatched())
	{
		int asdf = 9;
	}

	blockingEnemy = 0;

	which_ai.setLastNonTargetTime(level.time);

	int shooting = 0;

	if(level.time < last_fired_time + 8.0 || ((path_goal_updatedtime + CHEAT_SHOOT_TIME > level.time && path_goalentity && !which_ai.GetMySkills()->testFlag(AIS_NOSUPPRESS))))
	{
		shooting = 1;
		vec3_t shot;
		int forceShoot = 0;

		if(level.time < last_fired_time + 8.0)
		{
			VectorCopy(firedSpot, shot);
		}
		else
		{
			switch(0)
			{
			case 0:
				if(VectorLengthSquared(trail3))
				{
					VectorCopy(trail3, shot);
					break;
				}
				//intentional fall through
			case 1:
				if(VectorLengthSquared(trail2))
				{
					VectorCopy(trail2, shot);
					break;
				}
				//intentional fall through
			case 2:
				if(VectorLengthSquared(trail1))
				{
					VectorCopy(trail1, shot);
					break;
				}
				//intentional fall through
			default:
				VectorCopy(path_goalpos, shot);//??
				break;
			}
			
			//forceShoot = 1;
		}

		body_c *body = which_ai.GetBody();

		if (body)
		{
			//mmove_t *myMove = body->GetSequenceForAttack(monster, dest, face, target, ACTSUB_NORMAL, BBOX_PRESET_STAND, preferred_move, reject_actionflags);
			vec3_t fwd;
			vec3_t toFace;
			//which_ai.GetAimVector(fwd);
			//vertical does not matter
			AngleVectors(monster.s.angles, fwd, 0, 0);
			fwd[2] = 0;
			VectorNormalize(fwd);
			VectorSubtract(shot, monster.s.origin, toFace);
			toFace[2] = 0;
			VectorNormalize(toFace);
			mmove_t *myMove;

	//		fxRunner.exec("weapons/world/slugexplode", shot);

			/*if((!which_ai.GetMySkills()->testFlag(AIS_NODUCK)) && 
				(DotProduct(toFace, fwd) > .98) && 
				(forceShoot || ClearShot(which_ai, monster, shot, 0, BBOX_PRESET_CROUCH)))
			{
				myMove = body->GetSequenceForAttack(monster, monster.s.origin, shot, 0, ACTSUB_NORMAL, BBOX_PRESET_CROUCH);
			}
			else */if(forceShoot || ClearShot(which_ai, monster, shot, 0, BBOX_PRESET_STAND))
			{
				myMove = body->GetSequenceForAttack(monster, monster.s.origin, shot, 0, ACTSUB_NORMAL, BBOX_PRESET_STAND);
			}
			else
			{	//well, at the very least minimize surface area
				shooting = 0;
				//myMove = body->GetSequenceForStand(monster,shot,shot,ACTSUB_NORMAL,BBOX_PRESET_CROUCH);
			}

			if(shooting)
			{
				AddActionForSequence(which_ai, monster, myMove, monster.s.origin, shot, path_goalentity);

				which_ai.GetMiscFlags() |= MISC_FLAG_FIRINGBLIND;
			}
		}
	}
	
	if(!shooting)
	{
		if(monster.spawnflags & SPAWNFLAG_HOLD_POSITION)
		{//very special case - there guys are always in an ideal position, sort of
			path_decision::AddActionIdealPosition(which_ai, monster, goalPos, moveVec);
			return;
		}

		vec3_t checkSpot;

		if((path_goal_updatedtime + CHEATING_TIME > level.time || which_ai.GetMySkills()->testFlag(AIS_CHEATERSEARCH)) && path_goalentity)
		{
			VectorCopy(path_goalentity->s.origin, checkSpot);
		}
		else if(level.time < last_fired_time + 20.0)
		{
			VectorCopy(firedSpot, checkSpot);
		}
		else
		{
			VectorCopy(goalPos, checkSpot);
		}

		vec3_t distToMe;
		VectorSubtract(checkSpot, monster.s.origin, distToMe);

		nonIdealReason = NI_TOOFAR_SEARCHING;;

//		if((VectorLengthSquared(distToMe) < (128*128))||
//			(path_goalentity && gi.inPVS(monster.s.origin, checkSpot)))
		if(VectorLengthSquared(distToMe) < (64*64))
		{	//if I am very close to my search point or I can see it but not my target, start searching
			vec3_t lookSpot;

			//we want our guy to look at each possible spot for 4 seconds, but I don't feel like adding a counter
			//the addition of monster will add a bit of randomness if two guys are at the same node (icky, though)
			if(aiPoints.getLookSpot(monster.s.origin, lookSpot, ((int)(level.time/4.0)+(int)(&monster))))
			{
				body_c *body = which_ai.GetBody();

				if (body)
				{
					mmove_t *myMove;

					myMove = body->GetSequenceForStand(monster,monster.s.origin,lookSpot,ACTSUB_NORMAL,BBOX_PRESET_STAND);

					AddActionForSequence(which_ai, monster, myMove, monster.s.origin, lookSpot, path_goalentity);

					which_ai.GetMiscFlags() |= MISC_FLAG_FIRINGBLIND;
					return;
				}
			}

			/*if(aiPoints.search(&which_ai.getPathData(), &which_ai))
			{
				VectorCopy(which_ai.getPathData().goPoint, checkSpot);
			}
			else*/
/*			{
				if(VectorLengthSquared(distToMe) < (64*64))
				{//just go where I was going anyway and hang out -  this will make me look at my targ though
					VectorSubtract(path_goalentity->s.origin, monster.s.origin, checkSpot);
					VectorNormalize(checkSpot);
					VectorMA(monster.s.origin, 3, checkSpot, checkSpot);
				}
				else
				{//do what I wasss doing anyway
				}
			}*/
		}
	
		path_decision::AddActionNonIdealPosition(which_ai, monster, checkSpot, moveVec);

	}
}


qboolean search_decision::Consider(ai_c &which_ai, edict_t &monster)
{
	path_decision::Consider(which_ai, monster);

#ifdef _DEBUG
	decName = "searchdecision";
#endif
	return IsTimedOut();
}

search_decision::search_decision(search_decision *orig)
: path_decision(orig)
{
	pursuit_search = orig->pursuit_search;
	approach_search = orig->approach_search;
	VectorCopy(orig->pursuit_dir, pursuit_dir);
	last_fired_time = orig->last_fired_time;
	VectorCopy(orig->firedSpot, firedSpot);
	VectorCopy(orig->trail1, trail1);
	VectorCopy(orig->trail2, trail2);
	VectorCopy(orig->trail3, trail3);
}

void search_decision::Evaluate(search_decision *orig)
{
	pursuit_search = orig->pursuit_search;
	approach_search = orig->approach_search;
	VectorCopy(orig->pursuit_dir, pursuit_dir);
	last_fired_time = orig->last_fired_time;
	VectorCopy(orig->firedSpot, firedSpot);
	VectorCopy(orig->trail1, trail1);
	VectorCopy(orig->trail2, trail2);
	VectorCopy(orig->trail3, trail3);


	path_decision::Evaluate(orig);
}

void search_decision::Read()
{
	char	loaded[sizeof(search_decision)];

	gi.ReadFromSavegame('AISD', loaded, sizeof(search_decision));
	Evaluate((search_decision *)loaded);
}

void search_decision::Write()
{
	search_decision	*savable;

	savable = new search_decision(this);
	gi.AppendToSavegame('AISD', savable, sizeof(*this));
	delete savable;
}

void search_decision::SetInfoForDodge(vec3_t start, vec3_t end)
{
	last_fired_time = level.time;
	VectorCopy(start, firedSpot);
}

/**********************************************************************************
 **********************************************************************************/
retreat_decision::retreat_decision(int priority_root,float timeout)
:path_decision(NULL,priority_root,timeout)
{
	//start with validity at 0--as i lose health, validity rises; priority weighted very heavily by validity
	validity = 0;
	prev_health = 0;
	haveFleeDest = false;
	fearIndex = 0;
	spookStyle = SPOOK_NOT;
	spookTime = 0;
	VectorClear(spookCenter);
}

qboolean retreat_decision::IsAdversary(edict_t &monster, edict_t *otherGuy)
{
	if (monster.ai && monster.ai->GetAbusedByTeam())//when you've been abused, everyone's an enemy. <sniff>.
	{
		return true;
	}
	return (!OnSameTeam(&monster, otherGuy));
}

#define SUCCESSFUL_RUN_LEN 1000*1000
qboolean retreat_decision::IsIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t curPos, vec3_t goalPos, vec3_t moveVec)
{
	vec3_t distFromAttacker;

	blockingEnemy = 0;

	which_ai.Emote(monster, EMOTION_AFRAID, 5.0);//fixme--this decision is too general to do this here!!

	//hey, the important thing is not disappearing in front of the player, don't really care about path_goalentity; right? --sfs
	if(level.sight_client)
	{
		VectorSubtract(level.sight_client->s.origin, monster.s.origin, distFromAttacker);

		if(VectorLengthSquared(distFromAttacker) > SUCCESSFUL_RUN_LEN)
		{
			haveFleeDest = true;
			return true;
		}
	}

	if(fearIndex > .7)
	{
		return true;
	}

	if((spookStyle == SPOOK_EXPLOSIVE || spookStyle == SPOOK_EXPLOSIVE2))
	{
		nonIdealReason = NI_TOOCLOSE;//just wander off
	}
/*	if((which_ai.GetMySkills()->getExplosive() == ESEX_DUCK) && (spookStyle == SPOOK_EXPLOSIVE || spookStyle == SPOOK_EXPLOSIVE2))
	{
		nonIdealReason = NI_DUCKING;
	}
	else if((which_ai.GetMySkills()->getExplosive() == ESEX_DODGE) && spookStyle == SPOOK_EXPLOSIVE)
	{
		nonIdealReason = NI_DODGING;
	}*/
	else
	{
		nonIdealReason = NI_FLEEING;
	}
	return false;
}

void retreat_decision::AddActionIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	if(fearIndex > .7)
	{
		body_c *body = which_ai.GetBody();

		if (body)
		{
			mmove_t *myMove;

			myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_CROUCH, &generic_move_crouch_cower_mid);

			if(!myMove)
			{
				myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_CROUCH, &generic_move_crouch_cower_mid2);
			}

			if(myMove)
			{
				AddActionForSequence(which_ai, monster, myMove, goalPos, goalPos, path_goalentity);
			}
		}
	}
	else
	{
		//if i'm doin ok, stand & shoot
	//	gi.dprintf("validity: %f, goal %s, current %s\n",validity,vtos(goalPos),vtos(monster.s.origin));
		AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster,monster.s.origin,goalPos), goalPos, goalPos, path_goalentity);
		//consider removing myself here?
		if(haveFleeDest)
		{
			which_ai.SetAttentionLevel(ATTENTION_ESCAPED);//this is my cue that I have gotten the hell out of dodge, as it were
		}
	}
}

#define DEATH_SPOOK_TIME 4.0
#define EXPLODE_TIME 2.0

qboolean retreat_decision::Consider(ai_c &which_ai, edict_t &monster)
{
//	vec3_t gohere;

#ifdef _DEBUG
	decName = "retreatdecision";
#endif

	if(which_ai.GetPriority() != PRIORITY_HIGH)
	{
		fearIndex = 0;
		return false;//if we're not so important anymore, disregard our previous fleeing
	}

	//update where i'm going--goal position is opposite of where i'll be going
	SetGoalPosition(which_ai, monster);

	if (!prev_health||level.time-path_goal_updatedtime>90)
	{
		prev_health = monster.health;
		validity = 0;
		priority = 1;
		fearIndex = 0;
		return false;
	}

	//if i'm marked to hold position, don't bother with any of this
	if (monster.spawnflags & SPAWNFLAG_HOLD_POSITION)
	{
		validity = 0.0;
	}

	sensedEntInfo_t sensed_client, sensed_monster;
	which_ai.GetSensedClientInfo(smask_all, sensed_client);
	which_ai.GetSensedMonsterInfo(smask_all, sensed_monster);

	//fixme - readd later
/*	if(spookStyle == SPOOK_DEAD_FRIEND && level.time - spookTime < DEATH_SPOOK_TIME && gi.irand(0,1) && (!(monster.spawnflags & SPAWNFLAG_HOLD_POSITION)))
	{//my friend died, so I react
		validity=1.0f;
		priority = 4.9;//?
	}
	else*/ if((spookStyle == SPOOK_EXPLOSIVE || spookStyle == SPOOK_EXPLOSIVE2) && level.time - spookTime < EXPLODE_TIME && (!(monster.spawnflags & SPAWNFLAG_HOLD_POSITION)))
	{//I am afraid of explosives
		validity=1.0f;
		priority = 4.9;
	}
	else
	{
		validity = 1.0f;
		priority = 4.9;
		if ((which_ai.HasHadTarget())&&
			(which_ai.GetBody() && which_ai.GetBody()->GetRightHandWeapon(monster)==ATK_NOTHING))//do I have someone to run away from?
		{
			validity=1.0f;
			priority = 4.9;//?
		}
		else
		{
			priority = 1.0;
		}
	}

	fearIndex -= .003;
	if(fearIndex < 0)
	{
		fearIndex = 0;
	}

	//lowest possible priority should be lower than path_decision's lowest --sfs
	//priority = 1.9+(floor(8*validity+0.5) * (which_ai.GetMySkills()->getCowardice()+0.5));

	//if decision can time out, and its time has expired, then let ai get rid of it
	return IsTimedOut();
}

void retreat_decision::AddFear(float amount, vec3_t center)
{
	if(amount >= spookStyle)
	{
		spookTime = level.time;
		spookStyle = amount;
		VectorCopy(center, spookCenter);
	}
}

void retreat_decision::SetInfoForDodge(vec3_t start, vec3_t end)
{	// getting shot at is frightening...
	if(fearIndex < 1.0)
	{
		fearIndex += .4;
		if(fearIndex >= 1.0)
		{
			fearIndex = 1.0;
		}
	}
}

qboolean retreat_decision::GetGoalPosition(ai_c &which_ai, edict_t &monster, vec3_t gohere)
{
	//copy ai's enemy_lastseen_pos into gohere vec
	int spooked = 0;
	vec3_t avoid;

/*	if(level.time - spookTime < EXPLODE_TIME && (spookStyle == SPOOK_EXPLOSIVE || spookStyle == SPOOK_EXPLOSIVE2))
	{
		spooked = 1;
		VectorCopy(spookCenter, avoid);
	}
	else
	{*/
		VectorCopy(path_goalpos, avoid);
//	}

	if(level.time - spookTime < EXPLODE_TIME && (spookStyle == SPOOK_EXPLOSIVE || spookStyle == SPOOK_EXPLOSIVE2))
	{
		VectorCopy(avoid, gohere);
	}
	else
	{
		if (gohere)//pass in NULL if just want to know whether we have goal position
		{
			haveFleeDest = true;
			if(OnSameTeam(level.sight_client, &monster))
			{
				if (spooked || !aiPoints.getNearestFleePoint(&which_ai.getPathData(), gohere, which_ai.GetTeam(monster)))
				{
					aiPoints.getReversePath(monster.s.origin, avoid, &which_ai.getPathData(), &which_ai, &monster, which_ai.getTarget());
					VectorCopy(which_ai.getPathData().goPoint, gohere);

					if(which_ai.getPathData().blocked)
					{	// can't go where I want to :(
						VectorCopy(monster.s.origin, gohere);
						haveFleeDest = false;
					}
				}
			}
			else
			{	// monsters just run where they can
				aiPoints.getReversePath(monster.s.origin, avoid, &which_ai.getPathData(), &which_ai, &monster, which_ai.getTarget());
				VectorCopy(which_ai.getPathData().goPoint, gohere);

				if(which_ai.getPathData().blocked)
				{	// can't go where I want to :(
					VectorCopy(monster.s.origin, gohere);
					haveFleeDest = false;
				}

			}
		}
	}

	//check the decision's goalentity for whether i have goal pos, not ai's enemy
	if (path_goalentity)
	{
		//make sure goal entity is still valid
		if (!path_goalentity->inuse || path_goalentity->health <= 0 || level.time-path_goal_updatedtime>90)
		{
			path_goalentity = NULL;
//			return false;
			return true;
		}
		return true;
	}
	else
	{
//		return false;
		return true;
	}
}

retreat_decision::retreat_decision(retreat_decision *orig)
: path_decision(orig)
{
	prev_health = orig->prev_health;
	haveFleeDest = orig->haveFleeDest;
	fearIndex = orig->fearIndex;
	spookStyle = orig->spookStyle;
	spookTime = orig->spookTime;
	VectorCopy(orig->spookCenter, spookCenter);
}

void retreat_decision::Evaluate(retreat_decision *orig)
{
	prev_health = orig->prev_health;
	haveFleeDest = orig->haveFleeDest;
	fearIndex = orig->fearIndex;
	spookStyle = orig->spookStyle;
	spookTime = orig->spookTime;
	VectorCopy(orig->spookCenter, spookCenter);

	path_decision::Evaluate(orig);
}

void retreat_decision::Read()
{
	char	loaded[sizeof(retreat_decision)];

	gi.ReadFromSavegame('AIRD', loaded, sizeof(retreat_decision));
	Evaluate((retreat_decision *)loaded);
}

void retreat_decision::Write()
{
	retreat_decision	*savable;

	savable = new retreat_decision(this);
	gi.AppendToSavegame('AIRD', savable, sizeof(*this));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/

dodge_decision::dodge_decision(int priority_root, float timeout)
: path_decision(NULL, priority_root, timeout)
{
	//start with validity at 0--as i lose health, validity rises; priority weighted very heavily by validity
	validity = 0;
	prev_health = 0;
	last_dodge_time = level.time;
	last_shotat_time = level.time;
	dodgeSide = 0;

	VectorClear(shotStart);
	VectorClear(shotEnd);
}

#define ROLL_DIST 64

qboolean dodge_decision::Consider(ai_c &which_ai, edict_t &monster)
{
//	vec3_t gohere;

	body_c	*the_body=which_ai.GetBody();

#ifdef _DEBUG
	decName = "dodgedecision";
#endif

	if (which_ai.GetBody())
	{
		if (which_ai.GetBody()->IsMovementImpeded())
		{
			priority = .1;
			return IsTimedOut();
		}
	}

	if(monster.spawnflags & SPAWNFLAG_HOLD_POSITION)
	{
		priority = .1;
		return IsTimedOut();
	}

	if(!which_ai.getTarget())
	{
		priority = .1;
		return IsTimedOut();
	}

	SetGoalPosition(which_ai, monster);

	if(level.time < last_dodge_time + 3.0)
	{
		priority = .1;
	}
	else if(last_shotat_time < level.time - .3)
	{
		priority = .1;
	}
	else if(path_goal_updatedtime + .2 < level.time)
	{
		priority = .1;
	}
	else if(gi.flrand(0, 1) > which_ai.GetMySkills()->getDodge() * game.playerSkills.getDodge())
	{	// this allows us to specify how much a particular guy will roll
		if (the_body && the_body->GetMove() && the_body->GetMove()->suggested_action != ACTCODE_PAIN)
		{
//			the_body->VoiceSound("duck", monster, 0);
		}
		priority = .1;
		last_shotat_time = 0;//so we don't get delayed dodging
	}
	else
	{
		vec3_t angs;
		vec3_t fwd, right, up;
		vec3_t shotDir;

		if (the_body && the_body->GetMove() && the_body->GetMove()->suggested_action != ACTCODE_PAIN)
		{
//			the_body->VoiceSound("duck", monster, 0);
		}

		which_ai.GetAimAngles(angs);
		AngleVectors(angs, fwd, right, up);
		
		VectorSubtract(monster.s.origin, shotStart, shotDir);
		VectorNormalize(shotDir);

		if(DotProduct(shotDir, fwd) > -.707)
		{	// cosine of 45 degrees
			priority = .1;
		}
		else
		{
			vec3_t sideDir;

			VectorSubtract(shotEnd, shotStart, sideDir);
			float temp = sideDir[1];
			sideDir[1] = sideDir[0];
			sideDir[0] = -temp;

			vec3_t myDir;
			VectorSubtract(monster.s.origin, shotStart, myDir);
			
			if(DotProduct(sideDir, myDir) > 0)
			{
				dodgeSide = -1;
			}
			else
			{
				dodgeSide = 1;
			}

			vec3_t rollEnd;

			VectorMA(monster.s.origin, dodgeSide * -ROLL_DIST, right, rollEnd);

			trace_t tr;
			vec3_t min = {-16, -16, -16};
			vec3_t max = {16, 16, 16};

			gi.trace(monster.s.origin, min, max, rollEnd, &monster, MASK_SOLID, &tr);

			if(tr.fraction < 1.0)
			{
				dodgeSide = 0;
				
				VectorMA(monster.s.origin, ROLL_DIST, fwd, rollEnd);
				gi.trace(monster.s.origin, min, max, rollEnd, &monster, MASK_SOLID, &tr);

				if(tr.fraction < 1.0)
				{
					dodgeSide = 666;//mark this for below
				}
			}

			if(dodgeSide != 666)//hehe
			{
				priority = 4;
			}
			else
			{
				priority = .1;
				last_shotat_time = 0;//so we don't get delayed dodging
			}
		}
	}

	//if decision can time out, and its time has expired, then let ai get rid of it
	return IsTimedOut();
}


void dodge_decision::AddAction(ai_c &which_ai, edict_t &monster)
{//go exactly opposite of where my normal pursue would take me
	last_dodge_time = level.time;

	//ick? will this screw guys up? dunno my path_goalpos here...

	if (path_goalentity && (path_goalentity->ai || path_goalentity->client) && IsAdversary(monster, path_goalentity))
	{
		which_ai.SetTargetTime(path_goal_updatedtime, path_goalentity, path_goalpos);
	}

	if (path_nextpoint.valid)
	{
		if(AddSpecialPathAction(which_ai, monster))
		{
			return;
		}
	}
	else
	{
		//fixme: this has been changed to allow jumps, but goalpos should be adjusted in that case as well
		vec3_t dodgeGoal;
		mmove_t	*dodgeMove;

		if(path_goalentity)
		{
			VectorCopy(path_goalentity->s.origin, dodgeGoal);
		}
		else
		{
			VectorCopy(monster.s.origin, dodgeGoal);
		}
		dodgeMove=GetSequenceForDodge(which_ai, monster, dodgeGoal, dodgeGoal, path_goalentity, 0, dodgeSide);
		if (dodgeMove && dodgeMove->actionFlags&ACTFLAG_BACKUP)
		{
			which_ai.GetAimVector(dodgeGoal);
			VectorInverse(dodgeGoal);
			VectorScale(dodgeGoal,200.0f,dodgeGoal);
			VectorAdd(dodgeGoal,monster.s.origin,dodgeGoal);
		}
		AddActionForSequence(which_ai, monster, dodgeMove, dodgeGoal, dodgeGoal);
	}

	last_dodge_time = level.time;
}

void dodge_decision::ActionCompleted(action_c &which_action, ai_c &which_ai, edict_t &monster, float percent_success)
{//go to next node in path, or set up temp goal for avoidance
//	edict_t *oldtarget = path_nextpoint;

	if (which_action.Type() == atype_attack)//update validity of attacking here.
	{
		return;
	}

	//based on success of this last action, update how valid this decision is.
	UpdateValidity(percent_success);
	
	//action completed successfully--go to next waypoint (no matter if i'm not following a path)
	if (percent_success > 0.25)
	{
		NextPointInPath(which_ai, monster, monster.s.origin);
	}
	//action completed unsuccessfully--don't bother setting up avoidance points
	else
	{
		NextPointInPath(which_ai, monster, monster.s.origin);
//		PerformAvoidance(which_ai, monster);
	}
}

dodge_decision::dodge_decision(dodge_decision *orig)
: path_decision(orig)
{
	validity = orig->validity;
	prev_health = orig->prev_health;
	last_dodge_time = orig->last_dodge_time;
	last_shotat_time = orig->last_shotat_time;
	dodgeSide = 0;

	VectorCopy(orig->shotStart, shotStart);
	VectorCopy(orig->shotEnd, shotEnd);
}

void dodge_decision::Evaluate(dodge_decision *orig)
{
	validity = orig->validity;
	prev_health = orig->prev_health;
	last_dodge_time = orig->last_dodge_time;
	last_shotat_time = orig->last_shotat_time;
	dodgeSide = 0;

	VectorCopy(orig->shotStart, shotStart);
	VectorCopy(orig->shotEnd, shotEnd);

	path_decision::Evaluate(orig);
}

void dodge_decision::Read()
{
	char	loaded[sizeof(dodge_decision)];

	gi.ReadFromSavegame('AIDD', loaded, sizeof(dodge_decision));
	Evaluate((dodge_decision *)loaded);
}

void dodge_decision::Write()
{
	dodge_decision	*savable;

	savable = new dodge_decision(this);
	gi.AppendToSavegame('AIDD', savable, sizeof(*this));
	delete savable;
}

void dodge_decision::SetInfoForDodge(vec3_t start, vec3_t end)
{
	last_shotat_time = level.time;

	VectorCopy(start, shotStart);
	VectorCopy(end, shotEnd);
}

/**********************************************************************************
 **********************************************************************************/
scripted_decision::scripted_decision(edict_t *goalent, int priority_root, float timeout, edict_t* ScriptOwner)
:path_decision(goalent,priority_root,timeout)
{
	scriptDone=false;
	ScriptEnt=ScriptOwner;
	self=NULL;
	ignorePreferred =false;
	orderStartTime=level.time;
	lastMove = NULL;
	VectorClear(oldPosition);
	VectorCopy(vec3_up, actDestDir);
	VectorClear(actDest);
}


bool scripted_decision::MatchScriptEnt(edict_t* Entity)
{
	if (Entity == ScriptEnt)
	{
		return true;
	}
	return false;
}

void scripted_decision::ActionCompleted(action_c &which_action, ai_c &which_ai, edict_t &monster, float percent_success)
{
	// if there is a variable to be signaled, signal it!
//	if (which_action.GetSignalEvent())
//	{
//		which_action.SetSignalEventState(true);		// signal it!
//	}	
	path_decision::ActionCompleted(which_action, which_ai, monster, percent_success);
	self = &monster;

	list<scriptOrder_c *>::iterator curOrder = orders.begin();

	if (curOrder==orders.end())
	{
		return;
	}

	qboolean isCloseEnoughToDest=false;


	// kef -- trying desperately to clean up these boolean checks to the point where
	//a rational individual can actually debug them

	bool	bKillOrder = (*curOrder)->kill,
			bTimedOut = (level.time >= (*curOrder)->holdTime+orderStartTime),
			bShootAttack = which_action.GetClassCode()==SHOOT_ATTACK_ACTION,
			bRunningMeleeAttack = which_action.GetClassCode()==RUNNING_MELEE_ATTACK_ACTION,
			bPreferredMove = !!(*curOrder)->preferredMove,
			bActCodeFall = (*curOrder)->actionType==ACTCODE_FALL,
			bActCodeJump = (*curOrder)->actionType==ACTCODE_JUMP,
			bActCodeMove = (*curOrder)->actionType==ACTCODE_MOVE,
			bFullAnim = bPreferredMove ? !!((*curOrder)->preferredMove->actionFlags & ACTFLAG_FULLANIM) : false,
			bAnimFinished = which_ai.GetBody() ? !!which_ai.GetBody()->IsAnimationFinished() : false,
			bAnimateHold = !!((*curOrder)->flags & ANIMATE_HOLD);


	if ((*curOrder)->actionType==ACTCODE_MOVE)
	{
		vec3_t distToDest;
		float closeEnough = 121.0;

		if (ignorePreferred)//been encountering problems...
		{
			closeEnough=monster.mins[0]*monster.mins[0]+monster.mins[1]*monster.mins[1];
		}

		VectorSubtract(actDest, monster.s.origin, distToDest);
		distToDest[2]=0;
		if (VectorLengthSquared(distToDest)<closeEnough)
		{
			isCloseEnoughToDest=true;
		}
		else
		{
			VectorNormalize(distToDest);
			float degreePassedDest=DotProduct(distToDest, actDestDir);
			if (degreePassedDest<0.0f)
			{
				isCloseEnoughToDest=true;
			}
		}
	}

	//done with order--move along...
		//...if i've timed out...
	if (!bKillOrder && bTimedOut ||
		//...or i performed a kill order with the real potential of killing...
		(bKillOrder && (bShootAttack||bRunningMeleeAttack)) ||
		//...or i'm falling
		(!bKillOrder && bPreferredMove && bActCodeFall)||
		//...or i'm jumping
		(!bKillOrder && bPreferredMove && bActCodeJump)||
		//...or i've finished a simple animation and no hold time is specified...
		(!bKillOrder && bPreferredMove && !(bActCodeMove && !bFullAnim) && bAnimFinished && (!(bAnimateHold)))||
		//...or i've gotten Really close to a move target...
		(isCloseEnoughToDest))
	{
		if ((*curOrder)->SignalEvent)
		{
			(*curOrder)->SignalEvent->SetEventState(true);
		}
		ignorePreferred = false;
		orderStartTime = level.time;
		lastMove = (*curOrder)->preferredMove;
		delete (*curOrder);
		orders.erase(curOrder);
		VectorCopy(actDest, oldPosition);
		curOrder=orders.begin();

		if (curOrder != orders.end()&&(*curOrder)->Emotion)
		{
			which_ai.Emote(monster, (emotion_index) (*curOrder)->Emotion, (*curOrder)->holdTime, true);
			delete (*curOrder);
			orders.erase(curOrder);
			curOrder=orders.begin();
		}

		if (curOrder != orders.end())
		{
			if ((*curOrder)->absoluteDest)
			{
				VectorCopy((*curOrder)->destVec, actDest);
			}
			else
			{
				VectorAdd((*curOrder)->destVec, monster.s.origin, actDest);
			}
			actDest[2]=monster.s.origin[2];
			VectorSubtract(actDest, monster.s.origin, actDestDir);
			VectorNormalize(actDestDir);
//			gi.dprintf("new actdest: %4.3f %4.3f %4.3f; my pos: %4.3f %4.3f %4.3f\n",actDest[0],actDest[1],actDest[2],monster.s.origin[0],monster.s.origin[1],monster.s.origin[2]);
		}
	}
	//hey, something's wrong! remember that i hit stuff...
	else if (percent_success < 0.5)
	{
		ignorePreferred = true;
	}
}

void scripted_decision::Perform(ai_c &which_ai, edict_t &monster)
{

	if (orders.size()>0)
	{
		list<scriptOrder_c *>::iterator tcurOrder = orders.begin();
		if (tcurOrder != orders.end()&&(*tcurOrder)->Emotion)
		{
			which_ai.Emote(monster, (emotion_index) (*tcurOrder)->Emotion, (*tcurOrder)->holdTime, true);
			delete (*tcurOrder);
			orders.erase(tcurOrder);
			tcurOrder=orders.begin();
		}
	}

	if(ai_dumb->value)
	{
		fxRunner.exec("scripted", &monster);
	}

	if (orders.size()<=0)
	{
		if(ai_dumb->value)
		{
			fxRunner.exec("confusion", &monster);
		}

//		gi.dprintf("Script decision has run out of instructions--standing like an idiot!\n");

		mmove_t *idiotMove = &generic_move_stand;

		//i just finished up a move that's good for idling in...
		if (lastMove && lastMove->bodyPosition == BODYPOS_IDLE && lastMove->actionFlags & ACTFLAG_LOOPANIM)
		{
			idiotMove = lastMove;
		}
		//i'm in the dark here--i'll just stand around and breathe...
		else
		{
			if (which_ai.GetBody())
			{
				// 1/3/00 kef -- using a different anim when idling inbetween talking stints
				if (which_ai.GetBody()->GetMove() && 
					(which_ai.GetBody()->GetMove()->bodyPosition == BODYPOS_TALKING))
				{	// I was talking, so let's go to std_ietalkpose_n_a_n
					idiotMove = &generic_move_talkpose;
				}
				else
				{
					switch (which_ai.GetBody()->GetRightHandWeapon(monster))
					{
					case ATK_ROCKET:
						idiotMove = &generic_move_staim_l;
						break;
					case ATK_ASSAULTRIFLE:
					case ATK_SNIPER:
					case ATK_AUTOSHOTGUN:
					case ATK_MACHINEGUN:
					case ATK_SHOTGUN:
					case ATK_MICROWAVE:
					case ATK_MICROWAVE_ALT:
					case ATK_FLAMEGUN:
					case ATK_DEKKER:
						idiotMove = &generic_move_stand_mrs;
						break;
					default:
						break;
					}
				}
			}
		}

		mmove_t *realIdiotMove = GetSequenceForStand(which_ai, monster, vec3_origin, vec3_origin, idiotMove/*NULL*/, 0);

		if (idiotMove && realIdiotMove && idiotMove!=realIdiotMove)
		{
			gi.dprintf("Botched idiocy! Couldn't set %s, using %s", idiotMove->ghoulSeqName, realIdiotMove->ghoulSeqName);
		}

		AddActionForSequence(which_ai, monster, realIdiotMove,
			vec3_origin, vec3_origin, NULL, NULL);
		return;
	}
	list<scriptOrder_c *>::iterator curOrder = orders.begin();
	mmove_t		*prefMove;

	//if i just finished doing a specific stand action with no hold value, consider it finished
	if (((*curOrder)->preferredMove && (*curOrder)->actionType==ACTCODE_STAND) && !((*curOrder)->flags & ANIMATE_HOLD) &&
		which_ai.GetBody() && ((*curOrder)->preferredMove==which_ai.GetBody()->GetMove())&&
		(which_ai.GetBody()->IsAnimationFinished()))
	{
		if ((*curOrder)->SignalEvent)
		{
			(*curOrder)->SignalEvent->SetEventState(true);
		}
		ignorePreferred = false;
		orderStartTime = level.time;
		lastMove = (*curOrder)->preferredMove;
		delete (*curOrder);
		orders.erase(curOrder);
		VectorCopy(actDest, oldPosition);
		curOrder=orders.begin();

		if (curOrder != orders.end()&&(*curOrder)->Emotion)
		{
			which_ai.Emote(monster, (emotion_index) (*curOrder)->Emotion, (*curOrder)->holdTime, true);
			delete (*curOrder);
			orders.erase(curOrder);
			curOrder=orders.begin();
		}

		if (curOrder == orders.end())
		{
			path_decision::Perform(which_ai, monster);
			return;
		}
		if ((*curOrder)->absoluteDest)
		{
			VectorCopy((*curOrder)->destVec, actDest);
		}
		else
		{
			VectorAdd((*curOrder)->destVec, monster.s.origin, actDest);
		}
		actDest[2]=monster.s.origin[2];
		VectorSubtract(actDest, monster.s.origin, actDestDir);
		VectorNormalize(actDestDir);
//		gi.dprintf("new actdest: %4.3f %4.3f %4.3f; my pos: %4.3f %4.3f %4.3f\n",actDest[0],actDest[1],actDest[2],monster.s.origin[0],monster.s.origin[1],monster.s.origin[2]);
	}

	if (VectorCompare(oldPosition,vec3_origin))
	{
		VectorCopy(monster.s.origin, oldPosition);

		if ((*curOrder)->absoluteDest)
		{
			VectorCopy((*curOrder)->destVec, actDest);
		}
		else
		{
			VectorAdd((*curOrder)->destVec, monster.s.origin, actDest);
		}
		actDest[2]=monster.s.origin[2];
		VectorSubtract(actDest, monster.s.origin, actDestDir);
		VectorNormalize(actDestDir);
//		gi.dprintf("new actdest: %4.3f %4.3f %4.3f; my pos: %4.3f %4.3f %4.3f\n",actDest[0],actDest[1],actDest[2],monster.s.origin[0],monster.s.origin[1],monster.s.origin[2]);
	}

//	if (ignorePreferred)
//	{
//		prefMove = NULL;
//	}
//	else
//	{
		prefMove = (*curOrder)->preferredMove;
//	}

	vec3_t	actFace;

	if (VectorCompare((*curOrder)->turnVec, vec3_origin))
	{
		VectorClear(actFace);
	}
	else
	{
		VectorScale((*curOrder)->turnVec, 1000.0, actFace);
		VectorAdd(actFace, monster.s.origin, actFace);
	}

	if ((!(*curOrder)->absoluteDest)&&VectorCompare((*curOrder)->destVec,vec3_origin))
	{
		VectorCopy(monster.s.origin, actDest);

		VectorCopy(vec3_up, actDestDir);
	}

	//if my order has a target entity, set im to be dest and face
	edict_t *orderTarget = (*curOrder)->target;
	bool	bTargetIsANotNull = orderTarget?( (orderTarget->classname && (0 == strcmp(orderTarget->classname, "info_notnull"))) ):false;
	// don't bother checking the health if it's an info_notnull. just use it.
	if ( orderTarget && orderTarget->inuse &&
		((orderTarget->health>0) || bTargetIsANotNull) )
	{
		VectorCopy(orderTarget->s.origin, actDest);
		VectorCopy(orderTarget->s.origin, actFace);

		VectorSubtract(actDest, monster.s.origin, actDestDir);
		VectorNormalize(actDestDir);
	}

	vec3_t shortDest;

	VectorSubtract(actDest, monster.s.origin, shortDest);
	if (VectorLengthSquared(shortDest)>0)
	{
		float distToGo=VectorNormalize(shortDest);
		//don't break up the motion for jumping actions
		if ((distToGo > (*curOrder)->speed*0.2)&&(*curOrder)->actionType!=ACTCODE_JUMP&&(*curOrder)->actionType!=ACTCODE_FALL)
		{
			distToGo=(*curOrder)->speed*0.2;
		}
		VectorScale(shortDest, distToGo, shortDest);
	}
	VectorAdd(shortDest, monster.s.origin, shortDest);

	mmove_t *newMove=GetSequenceForActionCode(which_ai, monster, (*curOrder)->actionType, shortDest, actFace, NULL, prefMove);

	if (prefMove && newMove && newMove != prefMove)
	{
		if (which_ai.GetBody() && which_ai.GetBody()->IsAvailableSequence(monster, prefMove))
		{
			gi.dprintf("script anim: couldn't set %s (but it's available!), using %s!\n", prefMove->ghoulSeqName, newMove->ghoulSeqName);
		}
		else
		{
			gi.dprintf("script anim: %s not available for %s, using %s!\n", prefMove->ghoulSeqName, monster.classname, newMove->ghoulSeqName);
		}
	}

	float actionHoldTime=0.0;
	if ((*curOrder)->flags & ANIMATE_HOLD)
	{
		actionHoldTime=(*curOrder)->holdTime;
	}

	//hey! do these still need to be different? COULD add target elsewise...
	if ((*curOrder)->kill || (*curOrder)->NullTarget)
	{
		AddActionForSequence(which_ai, monster, newMove,
			shortDest, actFace, (*curOrder)->target, NULL, (*curOrder)->kill, (*curOrder)->NullTarget, actionHoldTime);
	}
	else
	{
		AddActionForSequence(which_ai, monster, newMove,
			shortDest, actFace, NULL, NULL, false, (*curOrder)->NullTarget, actionHoldTime);
	}
}

void scripted_decision::AddOrder(ai_c &which_ai, edict_t &monster, scriptOrder_c &this_order)
{
	if (orders.size()==0)
	{
		//if it's an emotion animate command, just do it (only if we're at the top of the orders list currently)
		if (this_order.Emotion)
		{
			which_ai.Emote(monster, (emotion_index) this_order.Emotion, this_order.holdTime, true);
			return;
		}

		ignorePreferred =false;
		orderStartTime=level.time;
		VectorClear(oldPosition);
		VectorCopy(vec3_up, actDestDir);
		VectorClear(actDest);
	}

	scriptOrder_c *newOrder = new scriptOrder_c();
	newOrder->absoluteDest=this_order.absoluteDest;
	newOrder->actionType=this_order.actionType;
	VectorCopy(this_order.destVec,newOrder->destVec);
	newOrder->holdTime=this_order.holdTime;
	newOrder->preferredMove=this_order.preferredMove;
	VectorCopy(this_order.turnVec,newOrder->turnVec);
	newOrder->flags=this_order.flags;
	newOrder->SignalEvent=this_order.SignalEvent;
	newOrder->speed=this_order.speed;
	newOrder->target=this_order.target;
	newOrder->kill=this_order.kill;
	newOrder->NullTarget = this_order.NullTarget;
	newOrder->Emotion = this_order.Emotion;

	orders.insert(orders.end(),newOrder);
}


scripted_decision::~scripted_decision(void)
{
	list<scriptOrder_c *>::iterator curOrder = orders.begin();
	path_decision::~path_decision();
	while(curOrder!=orders.end())
	{
		delete (*curOrder);
		orders.erase(curOrder++);
	}
}

scripted_decision::scripted_decision(scripted_decision *orig)
: path_decision(orig)
{
	scriptDone = orig->scriptDone;
	*(int *)&self = GetEdictNum(orig->self);
	ignorePreferred = orig->ignorePreferred;
	orderStartTime = orig->orderStartTime;
	VectorCopy(orig->oldPosition, oldPosition);
	VectorCopy(orig->actDest, actDest);
	VectorCopy(orig->actDestDir, actDestDir);
	*(int *)&lastMove = GetMmoveNum(orig->lastMove);
	*(int *)&ScriptEnt = GetEdictNum(orig->ScriptEnt);
}

void scripted_decision::Evaluate(scripted_decision *orig)
{
	scriptDone = orig->scriptDone;
	self = GetEdictPtr((int)orig->self);
	ignorePreferred = orig->ignorePreferred;
	orderStartTime = orig->orderStartTime;
	VectorCopy(orig->oldPosition, oldPosition);
	VectorCopy(orig->actDest, actDest);
	VectorCopy(orig->actDestDir, actDestDir);
	lastMove = GetMmovePtr((int)orig->lastMove);
	ScriptEnt = GetEdictPtr((int)orig->ScriptEnt);

	path_decision::Evaluate(orig);
}

void scripted_decision::Read()
{
	char				loaded[sizeof(scripted_decision)];
	int					i;
	scriptOrder_c		*temp;
	int					count;

	gi.ReadFromSavegame('AISD', loaded, SCRIPTED_DECISION_END);
	Evaluate((scripted_decision *)loaded);

	gi.ReadFromSavegame('AION', &count, sizeof(count));
	if(count)
	{
		for(i = 0; i < count; i++)
		{
			temp = new scriptOrder_c();
			temp->Read();
			orders.push_back(temp);
		}
	}
}

void scripted_decision::Write()
{
	scripted_decision				*savable;
	int								count;
	list<scriptOrder_c *>::iterator	it;

	savable = new scripted_decision(this);
	gi.AppendToSavegame('AISD', savable, SCRIPTED_DECISION_END);
	delete savable;

	count = orders.size();
	gi.AppendToSavegame('AION', &count, sizeof(count));
	if(count)
	{
		for(it = orders.begin(); it != orders.end(); it++)
		{
			(*it)->Write();
		}
	}
}

// --------------------------------------------------------------

scriptOrder_c::scriptOrder_c(void)
{
}

scriptOrder_c::scriptOrder_c(scriptOrder_c *orig)
{
	*(int *)&preferredMove = GetMmoveNum(orig->preferredMove);
	holdTime = orig->holdTime;
	VectorCopy(orig->turnVec, turnVec);
	VectorCopy(orig->destVec, destVec);
	absoluteDest = orig->absoluteDest;
	kill = orig->kill;
	actionType = orig->actionType;
	flags = orig->flags;
	*(int *)&SignalEvent = GetEventNum(orig->SignalEvent);
	speed = orig->speed;
	*(int *)&target = GetEdictNum(orig->target);
	NullTarget = orig->NullTarget;
	Emotion = orig->Emotion;
}

void scriptOrder_c::Evaluate(scriptOrder_c *orig)
{
	preferredMove = GetMmovePtr((int)orig->preferredMove);
	holdTime = orig->holdTime;
	VectorCopy(orig->turnVec, turnVec);
	VectorCopy(orig->destVec, destVec);
	absoluteDest = orig->absoluteDest;
	kill = orig->kill;
	actionType = orig->actionType;
	flags = orig->flags;
	SignalEvent = GetEventPtr((int)orig->SignalEvent);
	speed = orig->speed;
	target = GetEdictPtr((int)orig->target);
	NullTarget = orig->NullTarget;
	Emotion = orig->Emotion;
}

void scriptOrder_c::Read()
{
	char	loaded[sizeof(scriptOrder_c)];

	gi.ReadFromSavegame('SDSO', loaded, sizeof(scriptOrder_c));
	Evaluate((scriptOrder_c *)loaded);
}

void scriptOrder_c::Write()
{
	scriptOrder_c	*savable;

	savable = new scriptOrder_c(this);
	gi.AppendToSavegame('SDSO', savable, sizeof(*this));
	delete savable;
}

// --------------------------------------------------------------

base_decision::base_decision(base_decision *orig)
: decision_c(orig)
{
}

void base_decision::Evaluate(base_decision *orig)
{
	decision_c::Evaluate(orig);
}

void base_decision::Read()
{
	char	loaded[sizeof(base_decision)];

	gi.ReadFromSavegame('AIBD', loaded, sizeof(base_decision));
	Evaluate((base_decision *)loaded);
}

void base_decision::Write()
{
	base_decision	*savable;

	savable = new base_decision(this);
	gi.AppendToSavegame('AIBD', savable, sizeof(*this));
	delete savable;
}

// --------------------------------------------------------------

pursue_decision::pursue_decision(pursue_decision *orig)
: path_decision(orig)
{
}

void pursue_decision::Evaluate(pursue_decision *orig)
{
	path_decision::Evaluate(orig);
}

void pursue_decision::Read()
{
	char	loaded[sizeof(pursue_decision)];

	gi.ReadFromSavegame('AIPD', loaded, sizeof(pursue_decision));
	Evaluate((pursue_decision *)loaded);
}

void pursue_decision::Write()
{
	pursue_decision	*savable;

	savable = new pursue_decision(this);
	gi.AppendToSavegame('AIPD', savable, sizeof(*this));
	delete savable;
}

// --------------------------------------------------------------

dekker1_decision::dekker1_decision(dekker1_decision *orig)
: path_decision(orig)
{
}

void dekker1_decision::Evaluate(dekker1_decision *orig)
{
	path_decision::Evaluate(orig);
}

void dekker1_decision::Read()
{
	char	loaded[sizeof(dekker1_decision)];

	gi.ReadFromSavegame('AIPD', loaded, sizeof(dekker1_decision));
	Evaluate((dekker1_decision *)loaded);
}

void dekker1_decision::Write()
{
	dekker1_decision	*savable;

	savable = new dekker1_decision(this);
	gi.AppendToSavegame('AIPD', savable, sizeof(*this));
	delete savable;
}

qboolean dekker1_decision::Consider(ai_c &which_ai, edict_t &monster)
{
	path_decision::Consider(which_ai, monster);
	return (monster.health<monster.max_health);
}

void dekker1_decision::AddAction(ai_c &which_ai, edict_t &monster)
{
	vec3_t	toPlayer;
	if (level.sight_client)
	{
		VectorSubtract(level.sight_client->s.origin, monster.s.origin, toPlayer);
		if (VectorNormalize(toPlayer)<70)
		{
//			gi.dprintf("Welcome to my home page!!!!!!!!I hurt you!\n");
			T_Damage(level.sight_client, &monster, &monster, toPlayer, level.sight_client->s.origin, monster.s.origin, 5, 0, DT_WATERZAP, MOD_MPG, 0.5, 0.5);
		}
	}
	fxRunner.exec("environ/dekarmor1", &monster);//MWAHAHA! I SUCK! --sfs

//	monster.ghoulInst->SetSpeed(gsOne);
	path_decision::AddAction(which_ai, monster);
}

void dekker1_decision::AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	AddDekkerActionNonIdealPosition(which_ai, monster, goalPos, moveVec, 3.0);
}

void dekker1_decision::AddDekkerActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec, float speed)
{
	vec3_t actionGoal;
	float move_dist_sq;

//	move_dist_sq = VectorLengthSquared(moveVec);
	move_dist_sq = 26;

	if(which_ai.BeingWatched())
	{
		int asdf = 9;
	}

	int attack = 1;

	if(nonIdealReason == NI_TOOFAR_SEARCHING)
	{
		attack = 0;
	}

	//no attacking if we're not ready to harm the player 
	float awakeTimeValue = level.time - which_ai.getFirstTargetTime();
	//if(awakeTimeValue < which_ai.GetMySkills()->getHesitation() && (which_ai.GetStartleability()))
	if(awakeTimeValue < which_ai.GetMySkills()->getHesitation() * ((gmonster.GetClosestEnemy() == &monster) ? .5:1))//the closest guy gets to attack quicker
	{	// we are groggy for a bit when we first arise
		attack = 0;
	}

	which_ai.GetMiscFlags() &= ~MISC_FLAG_FIRINGBLIND;

	if(nonIdealReason == NI_DODGING)
	{
		mmove_t *dodgeMove;

		dodgeMove=GetSequenceForDodge(which_ai, monster, goalPos, goalPos, path_goalentity, 0, gi.irand(0,1)?-1:1);
		if(dodgeMove)
		{
			AddActionForSequence(which_ai, monster, dodgeMove, goalPos, goalPos);
			return;
		}
		else
		{
			nonIdealReason = NI_DUCKING;
		}
	}

	if(nonIdealReason == NI_DUCKING)
	{
		body_c *body = which_ai.GetBody();

		if (body)
		{
			mmove_t *myMove;

			myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_CROUCH, &generic_move_crouch_cower_mid);

			if(myMove)
			{
				AddActionForSequence(which_ai, monster, myMove, goalPos, goalPos, path_goalentity);
				return;
			}
			else
			{
				nonIdealReason = NI_FLEEING;
			}
		}
		else
		{
			nonIdealReason = NI_FLEEING;
		}
	}

	int fastStrafe = 0;
	if(which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_mrs) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_l) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_p))
	{
		fastStrafe = 1;
	}

	int fastRetreat = 0;
	if(which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_mrs) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_p2) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_l))
	{
		fastRetreat = 1;
	}

	int proceedForward = 1;

	if((25>move_dist_sq) ||
		(monster.spawnflags & SPAWNFLAG_HOLD_POSITION) ||
		((which_ai.GetPriority() != PRIORITY_HIGH)&&(path_goalentity == level.sight_client)))
	{
		Enemy_Printf(&which_ai, "Non ideal stuck");
		AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster,goalPos,goalPos), goalPos, goalPos);
		proceedForward = 0;
	}
	else if(nonIdealReason == NI_TOOCLOSE)
	{
		proceedForward = 0;
		int backup = 0;
		vec3_t toAvoid, toTarg, looker;
		if(aiPoints.isActive())
		{
			aiPoints.getReversePath(monster.s.origin, goalPos, &which_ai.getPathData(), &which_ai, &monster, which_ai.getTarget());

			if(which_ai.getPathData().blocked)
			{	// can't go where I want to :(
				backup = 0;
			}
			else
			{
				Enemy_Printf(&which_ai, "Via %d->%d = %d\n", which_ai.getPathData().curNode,
					which_ai.getPathData().nextNode, which_ai.getPathData().finalNode);

				VectorSubtract(which_ai.getPathData().goPoint, monster.s.origin, toAvoid);
				toAvoid[2] = 0;
				float toAvoidLen = VectorNormalize(toAvoid);
/*				if(VectorLength(toAvoid) < .01)
				{
					VectorCopy(goalPos, actionGoal);
				}
				else*/
				{
					VectorMA(monster.s.origin, speed, toAvoid, actionGoal);
				}

				//if(toAvoidLen < .05)
				if(toAvoidLen < 10)
				{
					backup = 0;
				}
				else
				{
					if(which_ai.GetMySkills()->testFlag(AIS_NOATTACKONRETREAT) ||
						(which_ai.GetMove() && ClearShot(which_ai, monster, toAvoid, path_goalentity, which_ai.GetMove()->bbox, actionGoal)))
					{	// if we can still shoot once we backup, we like this spot - otherwise no
						backup = 1;
					}
					else
					{
						backup = 0;
					}
				}
				VectorMA(monster.s.origin, speed, toAvoid, actionGoal);
			}
		}
		else
		{
			backup = 0;

		}

		if(backup)
		{
			
			VectorSubtract(actionGoal, monster.s.origin, toAvoid);
			toAvoid[2] = 0;
			VectorNormalize(toAvoid);

			AdjustVectorByGuys(&monster, toAvoid);

			which_ai.RequestMoveOutOfWay(toAvoid);

			VectorSubtract(goalPos, monster.s.origin, toTarg);
			toTarg[2] = 0;
			float distToTarg = VectorNormalize(toTarg);

			if(DotProduct(toAvoid, toTarg) < -.717)
			{	//move in a direction opposite of where I want to face
				if(!fastRetreat)
				{
					VectorScale(toAvoid, 4.0, actionGoal);
					VectorAdd(monster.s.origin, actionGoal, actionGoal);
				}

				VectorScale(toAvoid, -speed, looker);
				VectorAdd(monster.s.origin, looker, looker);
			}
			else if(DotProduct(toAvoid, toTarg) > .717)
			{	//move in a direction towards where I face
				VectorScale(toAvoid, speed, looker);
				VectorAdd(monster.s.origin, looker, looker);
			}
			else
			{
				if(!fastStrafe)
				{
					VectorScale(toAvoid, 12.0, actionGoal);
					VectorAdd(monster.s.origin, actionGoal, actionGoal);
				}

				//set it to the side now - for strafing
				toAvoid[2] = toAvoid[1];
				toAvoid[1] = -toAvoid[0];
				toAvoid[0] = toAvoid[2];
				toAvoid[2] = 0;

				if(DotProduct(toAvoid, toTarg) > 0)
				{
					VectorScale(toAvoid, speed, looker);
					VectorAdd(monster.s.origin, looker, looker);
				}
				else
				{
					VectorScale(toAvoid, -speed, looker);
					VectorAdd(monster.s.origin, looker, looker);
				}
			}

			if(aipoints_show->value)
			{
				paletteRGBA_t	col = {250, 250, 250, 250};
				//FX_MakeLine(node[startNode].getPos(), node[nextNode].getPos(), col, 1);
				FX_MakeLine(which_ai.getPathData().goPoint, monster.s.origin, col, 1);
				FX_MakeRing(which_ai.getPathData().goPoint, 8);
				paletteRGBA_t	col2 = {0, 0, 0, 250};
				FX_MakeLine(actionGoal, monster.s.origin, col2, 1);
			}

			vec3_t testShoot;

			VectorSubtract(goalPos, monster.s.origin, testShoot);
			VectorNormalize(testShoot);
			VectorMA(monster.s.origin, 64, testShoot, testShoot);

			if(attack && (!which_ai.GetMySkills()->testFlag(AIS_NOATTACKONRETREAT)) && ClearShot(which_ai, monster, testShoot, path_goalentity, which_ai.GetMove()->bbox))
			{
				//AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, actionGoal, goalPos, path_goalentity), actionGoal, goalPos, path_goalentity);
				AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, actionGoal, looker, path_goalentity), actionGoal, looker, path_goalentity);
			}
			else
			{	//just back up a bit - can't shoot from here
				//oh dear.  gsfMovement is fairly different from gsfAttack... I did not know =/
				if(!fastRetreat)
				{
					VectorScale(toAvoid, 4.0, actionGoal);
					VectorAdd(monster.s.origin, actionGoal, actionGoal);
				}
				AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,looker), actionGoal, looker);
			}
		}
		else
		{
			if(attack && ClearShot(which_ai, monster, goalPos, path_goalentity, which_ai.GetMove()->bbox))
			{	//can't move, but at least I can shoot!
				AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, monster.s.origin, goalPos, path_goalentity), monster.s.origin, goalPos, path_goalentity);
			}
			else
			{	//Stand around and do nothing - too close and not clear.  Sad.
				if(which_ai.GetMySkills()->testFlag(AIS_WONTADVANCE))
				{
					AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster,goalPos,goalPos), goalPos, goalPos);
				}
				else
				{
					proceedForward = 1;
				}
			}
		}
	}

	if(blockingEnemy)
	{	//this is only the case if clearshot is obstructed by another guy
		vec3_t dif;
		VectorSubtract(goalPos, monster.s.origin, dif);
		dif[2] = 0;
		float goalDist = VectorNormalize(dif);

		if(goalDist > 1)
		{
			dif[2] = dif[1];
			dif[1] = dif[0]*-1;
			dif[0] = dif[2];
			dif[2] = 0;

			VectorMA(monster.s.origin, 64, dif, dif);

			trace_t tr;

			gi.trace(monster.s.origin, monster.mins, monster.maxs, dif, &monster, MASK_MONSTERSOLID, &tr);

			vec3_t lookSpot;
			VectorCopy(goalPos, lookSpot);

			if(nonIdealReason == NI_FLEEING)
			{
				VectorCopy(dif, lookSpot);
			}
		
			if(tr.fraction > .99 && !tr.allsolid && !tr.startsolid)
			{
				AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,dif,lookSpot), dif, lookSpot);
			}
			else
			{
				AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster,goalPos,goalPos), goalPos, goalPos);
			}
			proceedForward = 0;

			if(aipoints_show->value)
			{
				paletteRGBA_t	col = {0, 0, 250, 250};
				//FX_MakeLine(node[startNode].getPos(), node[nextNode].getPos(), col, 1);
				FX_MakeLine(dif, monster.s.origin, col, 1);
			}
		}
	}

	if(proceedForward)// if(nonIdealReason == NI_TOOFAR)//make this catchall - why not
	{
		if(aiPoints.isActive())
		{
			aiPoints.getPath(monster.s.origin, goalPos, &which_ai.getPathData(), &which_ai, &monster, 1, which_ai.getTarget());

/*			vec3_t sep;
			float len;
			VectorSubtract(which_ai.getPathData().goPoint, monster.s.origin, sep);
			len = VectorLength(sep);

			if(nonIdealReason == NI_TOOFAR_SEARCHING && which_ai.getPathData().curNode == which_ai.getPathData().nextNode && len < 48)
			{
				VectorCopy(monster.s.origin, which_ai.getPathData().goPoint);
			}*/

			Enemy_Printf(&which_ai, "Via %d->%d = %d\n", which_ai.getPathData().curNode,
				which_ai.getPathData().nextNode, which_ai.getPathData().finalNode);

			if(which_ai.getPathData().blocked)
			{	// can't go where I want to :(
				if(attack && which_ai.GetMove() && ClearShot(which_ai, monster, goalPos, path_goalentity, which_ai.GetMove()->bbox))
				{
					AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, monster.s.origin, goalPos, path_goalentity), monster.s.origin, goalPos, path_goalentity);
				}
				else
				{
					AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster,goalPos,goalPos), goalPos, goalPos);
				}
			}
			else
			{
				vec3_t toAvoid, toTarg, looker;

				VectorSubtract(which_ai.getPathData().goPoint, monster.s.origin, toAvoid);
				toAvoid[2] = 0;
				VectorNormalize(toAvoid);

				VectorScale(toAvoid, speed, actionGoal);
				VectorAdd(monster.s.origin, actionGoal, actionGoal);

				VectorSubtract(goalPos, monster.s.origin, toTarg);
				toTarg[2] = 0;
				float distToTarg = VectorNormalize(toTarg);

				if((nonIdealReason == NI_FLEEING)||(!fastStrafe)||(distToTarg > 448))
				{	// good for folks who can't strafe
					VectorScale(toAvoid, 40, looker);
					VectorAdd(monster.s.origin, looker, looker);
				}
				else
				{
					if(DotProduct(toAvoid, toTarg) < -.717)
					{
						if(!fastRetreat)
						{
							VectorScale(toAvoid, 4.0, actionGoal);
							VectorAdd(monster.s.origin, actionGoal, actionGoal);
						}

						VectorScale(toAvoid, -speed, looker);
						VectorAdd(monster.s.origin, looker, looker);
					}
					else if(DotProduct(toAvoid, toTarg) > .717)
					{
						VectorScale(toAvoid, speed, looker);
						VectorAdd(monster.s.origin, looker, looker);
					}
					else
					{
						if(!fastStrafe)
						{
							VectorScale(toAvoid, 12.0, actionGoal);
							VectorAdd(monster.s.origin, actionGoal, actionGoal);
						}

						//set it to the side now - for strafing
						toAvoid[2] = toAvoid[1];
						toAvoid[1] = -toAvoid[0];
						toAvoid[0] = toAvoid[2];
						toAvoid[2] = 0;

						if(DotProduct(toAvoid, toTarg) > 0)
						{
							VectorScale(toAvoid, speed, looker);
							VectorAdd(monster.s.origin, looker, looker);
						}
						else
						{
							VectorScale(toAvoid, -speed, looker);
							VectorAdd(monster.s.origin, looker, looker);
						}

//						VectorScale(toAvoid, speed, looker);
//						VectorAdd(monster.s.origin, looker, looker);
					}
				}

				vec3_t dif;
				VectorSubtract(monster.s.origin, actionGoal, dif);
				dif[2] = 0;
				vec3_t dif2;
				VectorSubtract(monster.s.origin, goalPos, dif2);
				dif2[2] = 0;
				if((VectorLengthSquared(dif) > 2)&&((VectorLengthSquared(dif2) > (50*50))||((nonIdealReason == NI_FLEEING)))&&((!which_ai.GetMySkills()->testFlag(AIS_WONTADVANCE))||(nonIdealReason == NI_FLEEING)))
				{
					AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,looker), actionGoal, looker);
				}
				else
				{
					if(nonIdealReason == NI_FLEEING)
					{	//get down an' cower
						body_c *body = which_ai.GetBody();

						if (body)
						{
							mmove_t *myMove;

							myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_CROUCH, &generic_move_crouch_cower_mid);

							if(!myMove)
							{
								myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_CROUCH, &generic_move_crouch_cower_mid2);
							}

							if(myMove)
							{
								AddActionForSequence(which_ai, monster, myMove, goalPos, goalPos, path_goalentity);
							}
						}
					}
					else
					{
						if(attack && ClearShot(which_ai, monster, goalPos, path_goalentity, which_ai.GetMove()->bbox))
						{	//can't move, but at least I can shoot! - fixme - some guys shouldn't do this
							AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, monster.s.origin, goalPos, path_goalentity), monster.s.origin, goalPos, path_goalentity);
						}
						else
						{
							AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster,actionGoal,goalPos), actionGoal, goalPos);
						}
					}
					//AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,looker), actionGoal, looker);
				}

				if(aipoints_show->value)
				{
					VectorScale(toAvoid, 40.0, actionGoal);
					VectorAdd(monster.s.origin, actionGoal, actionGoal);

					paletteRGBA_t	col = {250, 250, 250, 250};
					paletteRGBA_t	col2 = {0, 0, 0, 250};
					//FX_MakeLine(node[startNode].getPos(), node[nextNode].getPos(), col, 1);
					FX_MakeLine(which_ai.getPathData().goPoint, monster.s.origin, col, 1);
					FX_MakeRing(which_ai.getPathData().goPoint, 4);
					FX_MakeLine(actionGoal, monster.s.origin, col2, 1);
				}
			}
		}
		else
		{
			VectorSubtract(goalPos, monster.s.origin, actionGoal);
			VectorNormalize(actionGoal);
			VectorScale(actionGoal, speed, actionGoal);
			VectorAdd(monster.s.origin, actionGoal, actionGoal);
			AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,actionGoal), actionGoal, actionGoal);
		}
	}
}


// --------------------------------------------------------------

dekker2_decision::dekker2_decision(dekker2_decision *orig)
: dekker1_decision(orig)
{
}

void dekker2_decision::Evaluate(dekker2_decision *orig)
{
	dekker1_decision::Evaluate(orig);
}

void dekker2_decision::Read()
{
	char	loaded[sizeof(dekker2_decision)];

	gi.ReadFromSavegame('AIPD', loaded, sizeof(dekker2_decision));
	Evaluate((dekker2_decision *)loaded);
}

void dekker2_decision::Write()
{
	dekker2_decision	*savable;

	savable = new dekker2_decision(this);
	gi.AppendToSavegame('AIPD', savable, sizeof(*this));
	delete savable;
}

qboolean dekker2_decision::Consider(ai_c &which_ai, edict_t &monster)
{
	dekker1_decision::Consider(which_ai, monster);
	return (monster.health<((float)(monster.max_health))*0.75);
}

void dekker2_decision::AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	AddDekkerActionNonIdealPosition(which_ai, monster, goalPos, moveVec, 100.0);
}

void dekker2_decision::AddAction(ai_c &which_ai, edict_t &monster)
{
	vec3_t	toPlayer;
	if (level.sight_client)
	{
		VectorSubtract(level.sight_client->s.origin, monster.s.origin, toPlayer);
		if (VectorNormalize(toPlayer)<70)
		{
//			gi.dprintf("Welcome to my home page!!!!!!!!I hurt you!\n");
			T_Damage(level.sight_client, &monster, &monster, toPlayer, level.sight_client->s.origin, monster.s.origin, 5, 0, DT_WATERZAP, MOD_MPG, 0.5, 0.5);
		}
	}
	fxRunner.exec("environ/dekarmor1", &monster);//MWAHAHA! I SUCK! --sfs

	monster.ghoulInst->SetSpeed(gs4Over8);
	dekker1_decision::AddAction(which_ai, monster);
}

// --------------------------------------------------------------

dekker3_decision::dekker3_decision(dekker3_decision *orig)
: path_decision(orig)
{
	reachedDest=orig->reachedDest;
}

void dekker3_decision::AddAction(ai_c &which_ai, edict_t &monster)
{
	vec3_t	toPlayer;
	if (level.sight_client)
	{
		VectorSubtract(level.sight_client->s.origin, monster.s.origin, toPlayer);
		if (VectorNormalize(toPlayer)<70)
		{
//			gi.dprintf("Welcome to my home page!!!!!!!!I hurt you!\n");
			T_Damage(level.sight_client, &monster, &monster, toPlayer, level.sight_client->s.origin, monster.s.origin, 5, 0, DT_WATERZAP, MOD_MPG, 0.5, 0.5);
		}
	}
	fxRunner.exec("environ/dekarmor1", &monster);//MWAHAHA! I SUCK! --sfs

	monster.ghoulInst->SetSpeed(gsOne);
	path_decision::AddAction(which_ai, monster);
}

void dekker3_decision::Evaluate(dekker3_decision *orig)
{
	reachedDest=orig->reachedDest;
	path_decision::Evaluate(orig);
}

void dekker3_decision::Read()
{
	char	loaded[sizeof(dekker3_decision)];

	gi.ReadFromSavegame('AIPD', loaded, sizeof(dekker3_decision));
	Evaluate((dekker3_decision *)loaded);
}

void dekker3_decision::Write()
{
	dekker3_decision	*savable;

	savable = new dekker3_decision(this);
	gi.AppendToSavegame('AIPD', savable, sizeof(*this));
	delete savable;
}

qboolean dekker3_decision::Consider(ai_c &which_ai, edict_t &monster)
{
	path_decision::Consider(which_ai, monster);

	return reachedDest;
}

qboolean dekker3_decision::GetGoalPosition(ai_c &which_ai, edict_t &monster, vec3_t gohere)
{
	//copy ai's enemy_lastseen_pos into gohere vec
	if (gohere)//pass in NULL if just want to know whether we have goal position
	{
		VectorCopy(path_goalpos, gohere);
	}

	if (!reachedDest)
	{
		edict_t *curPt = G_Find (NULL, FOFS(classname), "func_Dekker_console");

		if (curPt)
		{
			if (gohere)
			{
				VectorCopy(curPt->s.origin, gohere);
			}
		}
		else
		{
			return false;//there's no dekker-die position on this map--no fair!
		}
	}

	//check the decision's goalentity for whether i have goal pos, not ai's enemy
	if (path_goalentity)
	{
		//make sure goal entity is still valid
		if ((!path_goalentity->client && !path_goalentity->ai) || !path_goalentity->inuse
			|| path_goalentity->health <= 0 || level.time-path_goal_updatedtime>PATH_DECISION_LOSETARGETTIME)
		{
			path_goalentity = NULL;
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

qboolean dekker3_decision::IsIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t curPos, vec3_t goalPos, vec3_t moveVec)
{

	if(!reachedDest)
	{//very special case - there guys are always in an ideal position, sort of
		edict_t *curPt = G_Find (NULL, FOFS(classname), "func_Dekker_console");

		if (curPt)//got a die point, keep running toward it until i'm really really close
		{
			vec3_t toGoal;
			float distToGoalSq;

			VectorSubtract(curPt->s.origin, monster.s.origin, toGoal);
			toGoal[2]=0;//ignore vertical diff
			distToGoalSq=VectorLengthSquared(toGoal);
			if (distToGoalSq > 8100)
			{
				nonIdealReason = NI_TOOFAR;//too far, keep runnin
				return false;
			}
		}
		reachedDest=true;//passed the far-away-from-die-point test--i'm either close enough, or it doesn't exist.
		edict_t *curGun;

		//set up the enthralling dekker-types-something boss stage
//		curGun = G_PickTarget ("consolepoint");
//		if (curGun && !strcmp(curGun->classname, "point_combat"))
//		{
//			monster.ai->NewDecision(new pathcombat_decision(curGun), &monster);
//			monster.ai->SetStartleability(false);
//		}

		if (curGun = G_Find (NULL, FOFS(targetname), "dekkergun1"))
		{
			curGun->use(curGun, &monster, &monster);
		}
		if (curGun = G_Find (NULL, FOFS(targetname), "dekkergun2"))
		{
			curGun->use(curGun, &monster, &monster);
		}
		if (curGun = G_Find (NULL, FOFS(targetname), "dekkergun3"))
		{
			curGun->use(curGun, &monster, &monster);
		}
//		if (curGun = G_Find (NULL, FOFS(targetname), "dekkerclip"))
//		{
//			curGun->use(curGun, &monster, &monster);
//		}
	}

//	return path_decision::IsIdealPosition(which_ai, monster, curPos, goalPos, moveVec);
	return false;
}

void dekker3_decision::AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	vec3_t actionGoal;
	float move_dist_sq;

//	move_dist_sq = VectorLengthSquared(moveVec);
	move_dist_sq = 26;

	if(which_ai.BeingWatched())
	{
		int asdf = 9;
	}

	int attack = 1;

	if(nonIdealReason == NI_TOOFAR_SEARCHING)
	{
		attack = 0;
	}

	//no attacking if we're not ready to harm the player 
	float awakeTimeValue = level.time - which_ai.getFirstTargetTime();
	if(awakeTimeValue < which_ai.GetMySkills()->getHesitation() * ((gmonster.GetClosestEnemy() == &monster) ? .5:1))//the closest guy gets to attack quicker
	{	// we are groggy for a bit when we first arise
		attack = 0;
	}

	which_ai.GetMiscFlags() &= ~MISC_FLAG_FIRINGBLIND;

	if(nonIdealReason == NI_DODGING)
	{
		mmove_t *dodgeMove;

		dodgeMove=GetSequenceForDodge(which_ai, monster, goalPos, goalPos, path_goalentity, 0, gi.irand(0,1)?-1:1);
		if(dodgeMove)
		{
			AddActionForSequence(which_ai, monster, dodgeMove, goalPos, goalPos);
			return;
		}
		else
		{
			nonIdealReason = NI_DUCKING;
		}
	}

	if(nonIdealReason == NI_DUCKING)
	{
		body_c *body = which_ai.GetBody();

		if (body)
		{
			mmove_t *myMove;

			myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_CROUCH, &generic_move_crouch_cower_mid);

			if(myMove)
			{
				AddActionForSequence(which_ai, monster, myMove, goalPos, goalPos, path_goalentity);
				return;
			}
			else
			{
				nonIdealReason = NI_FLEEING;
			}
		}
		else
		{
			nonIdealReason = NI_FLEEING;
		}
	}

	int fastStrafe = 0;
	if(which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_mrs) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_l) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_p))
	{
		fastStrafe = 1;
	}

	int fastRetreat = 0;
	if(which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_mrs) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_p2) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_l))
	{
		fastRetreat = 1;
	}

	int proceedForward = 1;


	if(aiPoints.isActive())
	{
		bool doPath=true;
		aiPoints.getPath(monster.s.origin, goalPos, &which_ai.getPathData(), &which_ai, &monster, 1, which_ai.getTarget());


		Enemy_Printf(&which_ai, "Via %d->%d = %d\n", which_ai.getPathData().curNode,
			which_ai.getPathData().nextNode, which_ai.getPathData().finalNode);

		if(which_ai.getPathData().blocked)
		{	// can't go where I want to :(
//			if(attack && which_ai.GetMove() && ClearShot(which_ai, monster, goalPos, path_goalentity, which_ai.GetMove()->bbox))
//			{
//				AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, monster.s.origin, goalPos, path_goalentity), monster.s.origin, goalPos, path_goalentity);
//				doPath=false;
//			}
		}
		if (doPath)
		{
			vec3_t toAvoid, toTarg, looker;

			VectorSubtract(which_ai.getPathData().goPoint, monster.s.origin, toAvoid);
			toAvoid[2] = 0;
			VectorNormalize(toAvoid);

			VectorScale(toAvoid, 100.0, actionGoal);
			VectorAdd(monster.s.origin, actionGoal, actionGoal);

			VectorSubtract(goalPos, monster.s.origin, toTarg);
			toTarg[2] = 0;
			float distToTarg = VectorNormalize(toTarg);

			if((nonIdealReason == NI_FLEEING)||(!fastStrafe)||(distToTarg > 448))
			{	// good for folks who can't strafe
				VectorScale(toAvoid, 40, looker);
				VectorAdd(monster.s.origin, looker, looker);
			}
			else
			{
				VectorScale(toAvoid, 2440, looker);
				VectorAdd(monster.s.origin, looker, looker);
			}

			vec3_t dif;
			VectorSubtract(monster.s.origin, actionGoal, dif);
			dif[2] = 0;
			vec3_t dif2;
			VectorSubtract(monster.s.origin, goalPos, dif2);
			dif2[2] = 0;
			if((VectorLengthSquared(dif) > 2)&&((VectorLengthSquared(dif2) > (50*50))||((nonIdealReason == NI_FLEEING)))&&((!which_ai.GetMySkills()->testFlag(AIS_WONTADVANCE))||(nonIdealReason == NI_FLEEING)))
			{
				AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,looker), actionGoal, looker);
			}
			else
			{
				AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,looker), actionGoal, looker);
			}

			if(aipoints_show->value)
			{
				VectorScale(toAvoid, 40.0, actionGoal);
				VectorAdd(monster.s.origin, actionGoal, actionGoal);

				paletteRGBA_t	col = {250, 250, 250, 250};
				paletteRGBA_t	col2 = {0, 0, 0, 250};
				FX_MakeLine(which_ai.getPathData().goPoint, monster.s.origin, col, 1);
				FX_MakeRing(which_ai.getPathData().goPoint, 4);
				FX_MakeLine(actionGoal, monster.s.origin, col2, 1);
			}
		}
	}
	else
	{
		VectorSubtract(goalPos, monster.s.origin, actionGoal);
		VectorNormalize(actionGoal);
		VectorScale(actionGoal, 100.0, actionGoal);
		VectorAdd(monster.s.origin, actionGoal, actionGoal);
		AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,actionGoal), actionGoal, actionGoal);
	}
}


// --------------------------------------------------------------

dekker4_decision::dekker4_decision(dekker4_decision *orig)
: path_decision(orig)
{
	reachedDest=orig->reachedDest;
}

void dekker4_decision::AddAction(ai_c &which_ai, edict_t &monster)
{
	vec3_t	toPlayer;
	if (level.sight_client)
	{
		VectorSubtract(level.sight_client->s.origin, monster.s.origin, toPlayer);
		if (VectorNormalize(toPlayer)<70)
		{
//			gi.dprintf("Welcome to my home page!!!!!!!!I hurt you!\n");
			T_Damage(level.sight_client, &monster, &monster, toPlayer, level.sight_client->s.origin, monster.s.origin, 5, 0, DT_WATERZAP, MOD_MPG, 0.5, 0.5);
		}
	}
	fxRunner.exec("environ/dekarmor1", &monster);//MWAHAHA! I SUCK! --sfs

	monster.ghoulInst->SetSpeed(gsOne);
	path_decision::AddAction(which_ai, monster);
}

void dekker4_decision::Evaluate(dekker4_decision *orig)
{
	reachedDest=orig->reachedDest;
	path_decision::Evaluate(orig);
}

void dekker4_decision::Read()
{
	char	loaded[sizeof(dekker3_decision)];

	gi.ReadFromSavegame('AIPD', loaded, sizeof(dekker3_decision));
	Evaluate((dekker4_decision *)loaded);
}

void dekker4_decision::Write()
{
	dekker4_decision	*savable;

	savable = new dekker4_decision(this);
	gi.AppendToSavegame('AIPD', savable, sizeof(*this));
	delete savable;
}

qboolean dekker4_decision::Consider(ai_c &which_ai, edict_t &monster)
{
	path_decision::Consider(which_ai, monster);

	return false;
}

qboolean dekker4_decision::GetGoalPosition(ai_c &which_ai, edict_t &monster, vec3_t gohere)
{
	//copy ai's enemy_lastseen_pos into gohere vec
	if (gohere)//pass in NULL if just want to know whether we have goal position
	{
		VectorCopy(path_goalpos, gohere);
	}

	if (!reachedDest)
	{
		edict_t *curPt = G_Find (NULL, FOFS(classname), "func_Dekker_diehere");

		if (curPt)
		{
			if (gohere)
			{
				VectorCopy(curPt->s.origin, gohere);
			}
		}
		else
		{
			return false;//there's no dekker-die position on this map--no fair!
		}
	}

	//check the decision's goalentity for whether i have goal pos, not ai's enemy
	if (path_goalentity)
	{
		//make sure goal entity is still valid
		if ((!path_goalentity->client && !path_goalentity->ai) || !path_goalentity->inuse
			|| path_goalentity->health <= 0 || level.time-path_goal_updatedtime>PATH_DECISION_LOSETARGETTIME)
		{
			path_goalentity = NULL;
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

qboolean dekker4_decision::IsIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t curPos, vec3_t goalPos, vec3_t moveVec)
{

	if(!reachedDest)
	{//very special case - there guys are always in an ideal position, sort of
		edict_t *curPt = G_Find (NULL, FOFS(classname), "func_Dekker_diehere");

		if (curPt)//got a die point, keep running toward it until i'm really really close
		{
			vec3_t toGoal;
			float distToGoalSq;

			VectorSubtract(curPt->s.origin, monster.s.origin, toGoal);
			toGoal[2]=0;//ignore vertical diff
			distToGoalSq=VectorLengthSquared(toGoal);
			if (distToGoalSq > 4900)
			{
				nonIdealReason = NI_TOOFAR;//too far, keep runnin
				return false;
			}
		}
		reachedDest=true;//passed the far-away-from-die-point test--i'm either close enough, or it doesn't exist.
		monster.spawnflags |= SPAWNFLAG_HOLD_POSITION;
//		edict_t *curGun;
//		if (curGun = G_Find (NULL, FOFS(targetname), "dekkergun1"))
//		{
//			curGun->use(curGun, &monster, &monster);
//		}
//		if (curGun = G_Find (NULL, FOFS(targetname), "dekkergun2"))
//		{
//			curGun->use(curGun, &monster, &monster);
//		}
//		if (curGun = G_Find (NULL, FOFS(targetname), "dekkergun3"))
//		{
//			curGun->use(curGun, &monster, &monster);
//		}
//		if (curGun = G_Find (NULL, FOFS(targetname), "dekkerclip"))
//		{
//			curGun->use(curGun, &monster, &monster);
//		}
	}

	return path_decision::IsIdealPosition(which_ai, monster, curPos, goalPos, moveVec);

}

void dekker4_decision::AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	vec3_t actionGoal;
	float move_dist_sq;

//	move_dist_sq = VectorLengthSquared(moveVec);
	move_dist_sq = 26;

	if(which_ai.BeingWatched())
	{
		int asdf = 9;
	}

	int attack = 1;

	if(nonIdealReason == NI_TOOFAR_SEARCHING)
	{
		attack = 0;
	}

	//no attacking if we're not ready to harm the player 
	float awakeTimeValue = level.time - which_ai.getFirstTargetTime();
	if(awakeTimeValue < which_ai.GetMySkills()->getHesitation() * ((gmonster.GetClosestEnemy() == &monster) ? .5:1))//the closest guy gets to attack quicker
	{	// we are groggy for a bit when we first arise
		attack = 0;
	}

	which_ai.GetMiscFlags() &= ~MISC_FLAG_FIRINGBLIND;

	if(nonIdealReason == NI_DODGING)
	{
		mmove_t *dodgeMove;

		dodgeMove=GetSequenceForDodge(which_ai, monster, goalPos, goalPos, path_goalentity, 0, gi.irand(0,1)?-1:1);
		if(dodgeMove)
		{
			AddActionForSequence(which_ai, monster, dodgeMove, goalPos, goalPos);
			return;
		}
		else
		{
			nonIdealReason = NI_DUCKING;
		}
	}

	if(nonIdealReason == NI_DUCKING)
	{
		body_c *body = which_ai.GetBody();

		if (body)
		{
			mmove_t *myMove;

			myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_CROUCH, &generic_move_crouch_cower_mid);

			if(myMove)
			{
				AddActionForSequence(which_ai, monster, myMove, goalPos, goalPos, path_goalentity);
				return;
			}
			else
			{
				nonIdealReason = NI_FLEEING;
			}
		}
		else
		{
			nonIdealReason = NI_FLEEING;
		}
	}

	int fastStrafe = 0;
	if(which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_mrs) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_l) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_p))
	{
		fastStrafe = 1;
	}

	int fastRetreat = 0;
	if(which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_mrs) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_p2) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_l))
	{
		fastRetreat = 1;
	}

	int proceedForward = 1;


	if(aiPoints.isActive())
	{
		bool doPath=true;
		aiPoints.getPath(monster.s.origin, goalPos, &which_ai.getPathData(), &which_ai, &monster, 1, which_ai.getTarget());


		Enemy_Printf(&which_ai, "Via %d->%d = %d\n", which_ai.getPathData().curNode,
			which_ai.getPathData().nextNode, which_ai.getPathData().finalNode);

		if(which_ai.getPathData().blocked)
		{	// can't go where I want to :(
//			if(attack && which_ai.GetMove() && ClearShot(which_ai, monster, goalPos, path_goalentity, which_ai.GetMove()->bbox))
//			{
//				AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, monster.s.origin, goalPos, path_goalentity), monster.s.origin, goalPos, path_goalentity);
//				doPath=false;
//			}
		}
		if (doPath)
		{
			vec3_t toAvoid, toTarg, looker;

			VectorSubtract(which_ai.getPathData().goPoint, monster.s.origin, toAvoid);
			toAvoid[2] = 0;
			VectorNormalize(toAvoid);

			VectorScale(toAvoid, 100.0, actionGoal);
			VectorAdd(monster.s.origin, actionGoal, actionGoal);

			VectorSubtract(goalPos, monster.s.origin, toTarg);
			toTarg[2] = 0;
			float distToTarg = VectorNormalize(toTarg);

			if((nonIdealReason == NI_FLEEING)||(!fastStrafe)||(distToTarg > 448))
			{	// good for folks who can't strafe
				VectorScale(toAvoid, 40, looker);
				VectorAdd(monster.s.origin, looker, looker);
			}
			else
			{
				VectorScale(toAvoid, 2440, looker);
				VectorAdd(monster.s.origin, looker, looker);
			}

			vec3_t dif;
			VectorSubtract(monster.s.origin, actionGoal, dif);
			dif[2] = 0;
			vec3_t dif2;
			VectorSubtract(monster.s.origin, goalPos, dif2);
			dif2[2] = 0;
			if((VectorLengthSquared(dif) > 2)&&((VectorLengthSquared(dif2) > (50*50))||((nonIdealReason == NI_FLEEING)))&&((!which_ai.GetMySkills()->testFlag(AIS_WONTADVANCE))||(nonIdealReason == NI_FLEEING)))
			{
				AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,looker), actionGoal, looker);
			}
			else
			{
				AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,looker), actionGoal, looker);
			}

			if(aipoints_show->value)
			{
				VectorScale(toAvoid, 40.0, actionGoal);
				VectorAdd(monster.s.origin, actionGoal, actionGoal);

				paletteRGBA_t	col = {250, 250, 250, 250};
				paletteRGBA_t	col2 = {0, 0, 0, 250};
				FX_MakeLine(which_ai.getPathData().goPoint, monster.s.origin, col, 1);
				FX_MakeRing(which_ai.getPathData().goPoint, 4);
				FX_MakeLine(actionGoal, monster.s.origin, col2, 1);
			}
		}
	}
	else
	{
		VectorSubtract(goalPos, monster.s.origin, actionGoal);
		VectorNormalize(actionGoal);
		VectorScale(actionGoal, 100.0, actionGoal);
		VectorAdd(monster.s.origin, actionGoal, actionGoal);
		AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,actionGoal), actionGoal, actionGoal);
	}
}

/*
void dekker4_decision::AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec)
{
	vec3_t actionGoal;
	float move_dist_sq;

//	move_dist_sq = VectorLengthSquared(moveVec);
	move_dist_sq = 26;

	if(which_ai.BeingWatched())
	{
		int asdf = 9;
	}

	int attack = 1;

	if(nonIdealReason == NI_TOOFAR_SEARCHING)
	{
		attack = 0;
	}

	//no attacking if we're not ready to harm the player 
	float awakeTimeValue = level.time - which_ai.getFirstTargetTime();
	//if(awakeTimeValue < which_ai.GetMySkills()->getHesitation() && (which_ai.GetStartleability()))
	if(awakeTimeValue < which_ai.GetMySkills()->getHesitation() * ((gmonster.GetClosestEnemy() == &monster) ? .5:1))//the closest guy gets to attack quicker
	{	// we are groggy for a bit when we first arise
		attack = 0;
	}

	which_ai.GetMiscFlags() &= ~MISC_FLAG_FIRINGBLIND;

	if(nonIdealReason == NI_DODGING)
	{
		mmove_t *dodgeMove;

		dodgeMove=GetSequenceForDodge(which_ai, monster, goalPos, goalPos, path_goalentity, 0, gi.irand(0,1)?-1:1);
		if(dodgeMove)
		{
			AddActionForSequence(which_ai, monster, dodgeMove, goalPos, goalPos);
			return;
		}
		else
		{
			nonIdealReason = NI_DUCKING;
		}
	}

	if(nonIdealReason == NI_DUCKING)
	{
		body_c *body = which_ai.GetBody();

		if (body)
		{
			mmove_t *myMove;

			myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_CROUCH, &generic_move_crouch_cower_mid);

			if(myMove)
			{
				AddActionForSequence(which_ai, monster, myMove, goalPos, goalPos, path_goalentity);
				return;
			}
			else
			{
				nonIdealReason = NI_FLEEING;
			}
		}
		else
		{
			nonIdealReason = NI_FLEEING;
		}
	}

	int fastStrafe = 0;
	if(which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_mrs) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_l) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_raimstrafr_p))
	{
		fastStrafe = 1;
	}

	int fastRetreat = 0;
	if(which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_mrs) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_p2) ||
		which_ai.GetBody()->IsAvailableSequence(monster, &generic_move_rbackaim_l))
	{
		fastRetreat = 1;
	}

	int proceedForward = 1;

//	if(proceedForward)// if(nonIdealReason == NI_TOOFAR)//make this catchall - why not
//	{
		if(aiPoints.isActive())
		{
			aiPoints.getPath(monster.s.origin, goalPos, &which_ai.getPathData(), &which_ai, &monster, 1, which_ai.getTarget());


			Enemy_Printf(&which_ai, "Via %d->%d = %d\n", which_ai.getPathData().curNode,
				which_ai.getPathData().nextNode, which_ai.getPathData().finalNode);

			if(which_ai.getPathData().blocked)
			{	// can't go where I want to :(
				if(attack && which_ai.GetMove() && ClearShot(which_ai, monster, goalPos, path_goalentity, which_ai.GetMove()->bbox))
				{
					AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, monster.s.origin, goalPos, path_goalentity), monster.s.origin, goalPos, path_goalentity);
				}
				else
				{
					AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster,goalPos,goalPos), goalPos, goalPos);
				}
			}
			else
			{
				vec3_t toAvoid, toTarg, looker;

				VectorSubtract(which_ai.getPathData().goPoint, monster.s.origin, toAvoid);
				toAvoid[2] = 0;
				VectorNormalize(toAvoid);

				VectorScale(toAvoid, 100.0, actionGoal);
				VectorAdd(monster.s.origin, actionGoal, actionGoal);

				VectorSubtract(goalPos, monster.s.origin, toTarg);
				toTarg[2] = 0;
				float distToTarg = VectorNormalize(toTarg);

				if((nonIdealReason == NI_FLEEING)||(!fastStrafe)||(distToTarg > 448))
				{	// good for folks who can't strafe
					VectorScale(toAvoid, 40, looker);
					VectorAdd(monster.s.origin, looker, looker);
				}
				else
				{

					if(DotProduct(toAvoid, toTarg) > .717)
					{
						VectorScale(toAvoid, 2440, looker);
						VectorAdd(monster.s.origin, looker, looker);
					}
					else
					{
						if(!fastStrafe)
						{
							VectorScale(toAvoid, 12.0, actionGoal);
							VectorAdd(monster.s.origin, actionGoal, actionGoal);
						}

						//set it to the side now - for strafing
						toAvoid[2] = toAvoid[1];
						toAvoid[1] = -toAvoid[0];
						toAvoid[0] = toAvoid[2];
						toAvoid[2] = 0;

						if(DotProduct(toAvoid, toTarg) > 0)
						{
							VectorScale(toAvoid, 2440, looker);
							VectorAdd(monster.s.origin, looker, looker);
						}
						else
						{
							VectorScale(toAvoid, -2440, looker);
							VectorAdd(monster.s.origin, looker, looker);
						}
					}
				}

				vec3_t dif;
				VectorSubtract(monster.s.origin, actionGoal, dif);
				dif[2] = 0;
				vec3_t dif2;
				VectorSubtract(monster.s.origin, goalPos, dif2);
				dif2[2] = 0;
				if((VectorLengthSquared(dif) > 2)&&((VectorLengthSquared(dif2) > (50*50))||((nonIdealReason == NI_FLEEING)))&&((!which_ai.GetMySkills()->testFlag(AIS_WONTADVANCE))||(nonIdealReason == NI_FLEEING)))
				{
					AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,looker), actionGoal, looker);
				}
				else
				{
					if(nonIdealReason == NI_FLEEING)
					{	//get down an' cower
						body_c *body = which_ai.GetBody();

						if (body)
						{
							mmove_t *myMove;

							myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_CROUCH, &generic_move_crouch_cower_mid);

							if(!myMove)
							{
								myMove = body->GetSequenceForStand(monster,goalPos,goalPos,ACTSUB_NORMAL,BBOX_PRESET_CROUCH, &generic_move_crouch_cower_mid2);
							}

							if(myMove)
							{
								AddActionForSequence(which_ai, monster, myMove, goalPos, goalPos, path_goalentity);
							}
						}
					}
					else
					{
						if(attack && ClearShot(which_ai, monster, goalPos, path_goalentity, which_ai.GetMove()->bbox))
						{	//can't move, but at least I can shoot! - fixme - some guys shouldn't do this
							AddActionForSequence(which_ai, monster, GetSequenceForAttack(which_ai, monster, monster.s.origin, goalPos, path_goalentity), monster.s.origin, goalPos, path_goalentity);
						}
						else
						{
							AddActionForSequence(which_ai, monster, GetSequenceForStand(which_ai,monster,actionGoal,goalPos), actionGoal, goalPos);
						}
					}
					//AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,looker), actionGoal, looker);
				}

				if(aipoints_show->value)
				{
					VectorScale(toAvoid, 40.0, actionGoal);
					VectorAdd(monster.s.origin, actionGoal, actionGoal);

					paletteRGBA_t	col = {250, 250, 250, 250};
					paletteRGBA_t	col2 = {0, 0, 0, 250};
					//FX_MakeLine(node[startNode].getPos(), node[nextNode].getPos(), col, 1);
					FX_MakeLine(which_ai.getPathData().goPoint, monster.s.origin, col, 1);
					FX_MakeRing(which_ai.getPathData().goPoint, 4);
					FX_MakeLine(actionGoal, monster.s.origin, col2, 1);
				}
			}
		}
		else
		{
			VectorSubtract(goalPos, monster.s.origin, actionGoal);
			VectorNormalize(actionGoal);
			VectorScale(actionGoal, 100.0, actionGoal);
			VectorAdd(monster.s.origin, actionGoal, actionGoal);
			AddActionForSequence(which_ai, monster, GetSequenceForMovement(which_ai,monster,actionGoal,actionGoal), actionGoal, actionGoal);
		}
//	}
}
*/

// --------------------------------------------------------------

pathidle_decision::pathidle_decision(pathidle_decision *orig)
: pathcorner_decision(orig)
{
}

void pathidle_decision::Evaluate(pathidle_decision *orig)
{
	pathcorner_decision::Evaluate(orig);
}

void pathidle_decision::Read()
{
	char	loaded[sizeof(pathidle_decision)];

	gi.ReadFromSavegame('AIHD', loaded, sizeof(pathidle_decision));
	Evaluate((pathidle_decision *)loaded);
}

void pathidle_decision::Write()
{
	pathidle_decision	*savable;

	savable = new pathidle_decision(this);
	gi.AppendToSavegame('AIHD', savable, sizeof(*this));
	delete savable;
}

// end
