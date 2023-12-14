#include "g_local.h"
#include "ai_private.h"
#include "ds.h"
#include "..\qcommon\ef_flags.h"

//////////////////////////////////////////////////////////////////////////////

action_c *action_c::NewClassForCode(int code)
{
	switch (code)
	{
	case STAND_ACTION:
		return new stand_action();
	case PAIN_ACTION:
		return new pain_action();
	case DEATH_ACTION:
		return new death_action();
	case DEATHCORPSE_ACTION:
		return new deathcorpse_action();
	case DEATHARMOR_ACTION:
		return new deatharmor_action();
	case ENDSCRIPT_ACTION:
		return new endscript_action();
	case WALK_ACTION:
		return new walk_action();
	case DOG_WALK_ACTION:
		return new dog_walk_action();
	case JUMP_ACTION:
		return new jump_action();
	case FALL_ACTION:
		return new fall_action();
	case SURRENDER_ACTION:
		return new surrender_action();
	case CAPTURE_ACTION:
		return new capture_action();
	case SHOOT_ATTACK_ACTION:
		return new shoot_attack_action();
	case RUNNING_MELEE_ATTACK_ACTION:
		return new running_melee_attack_action();
	case HELI_ACTION:
		return new heli_action();
	case HELIMOVE_ACTION:
		return new helimove_action();
	case HELIFACE_ACTION:
		return new heliface_action();
	case HELIATTACK_ACTION:
		return new heliattack_action();
	case HELIDEATH_ACTION:
		return new helideath_action();
	case SNOWCAT_ACTION:
		return new snowcat_action();
	case TANK_ACTION:
		return new tank_action();
	case ACTION:
		gi.dprintf("ERROR: only allowed to use leaves of action class tree: %d\n",code);
		break;
	default:
		gi.dprintf("ERROR: invalid action class code: %d\n",code);
		break;
	}
	return new action_c();
}

action_c::action_c(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing, edict_t *target, float timeout, qboolean fullAnimation)
{
	anim = newanim;

	VectorCopy(facing, face);
	VectorCopy(destination, dest);
	faceUpdatedTime = level.time;
	interrupted = false;
	abortAction = false;

	owner_decision = od;
	owner_action = oa;
	if (newanim)
	{
		velScale = newanim->velScale;
		default_turn = newanim->maxTurn * 2.0f;
	}
	else
	{
		velScale = 1.0f;
		default_turn = 30;
	}
	move_mask = step_movemask;
	start_time = level.time;
	timeout_interval = timeout;
	timeout_time = start_time + timeout_interval;
	VectorClear(jump_intent);
	jump_finish_time = level.time + 999;
	waitForAnim = fullAnimation;
	SignalEvent = NULL;
	isLethal = false;
	NullTarget = 0;
	
	action_target = target;
}

action_c::action_c(void)
{
	VectorClear(face);
	VectorClear(dest);
	VectorClear(jump_intent);
	timeout_interval = 999999999999;
	abortAction = false;
	owner_decision = NULL;
	owner_action = NULL;
	velScale = 0;
	default_turn = 0.0;
	move_mask = step_movemask;
	anim = &generic_move_stand;
	action_target = NULL;
	waitForAnim=false;
	faceUpdatedTime=level.time;
	interrupted = false;
	jump_finish_time = level.time + 999;
	SignalEvent = NULL;
	isLethal = false;
	NullTarget = 0;
}

void action_c::Init(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing, edict_t *target, float timeout, qboolean fullAnimation)
{
	anim = newanim;

	VectorCopy(facing, face);
	VectorCopy(destination, dest);
	faceUpdatedTime = level.time;
	interrupted = false;
	abortAction = false;

	owner_decision = od;
	owner_action = oa;
	if (newanim)
	{
		velScale = newanim->velScale;
		default_turn = newanim->maxTurn * 2.0f;
	}
	else
	{
		velScale = 1.0f;
		default_turn = 30;
	}
	move_mask = step_movemask;
	start_time = level.time;
	timeout_interval = timeout;
	timeout_time = start_time + timeout_interval;
	VectorClear(jump_intent);
	jump_finish_time = level.time + 999;
	waitForAnim = fullAnimation;
	SignalEvent = NULL;
	isLethal = false;
	NullTarget = 0;
	
	action_target = target;
}

void action_c::Init(void)
{
	VectorClear(face);
	VectorClear(dest);
	VectorClear(jump_intent);
	timeout_interval = 999999999999;
	abortAction = false;
	owner_decision = NULL;
	owner_action = NULL;
	velScale = 0;
	default_turn = 0.0;
	move_mask = step_movemask;
	anim = &generic_move_stand;
	action_target = NULL;
	waitForAnim=false;
	faceUpdatedTime=level.time;
	interrupted = false;
	jump_finish_time = level.time + 999;
	SignalEvent = NULL;
	isLethal = false;
	NullTarget = 0;
}

void action_c::ResolveAction(ai_c &which_ai, edict_t &monster, float decision_success, float action_success)
{
	if (owner_decision)
	{
		owner_decision->ActionCompleted(*this, which_ai, monster, decision_success);
	}
	if (owner_action)
	{
		owner_action->ActionCompleted(*this, which_ai, monster, action_success);
	}
}

void action_c::SetTurnDestination(ai_c &which_ai, edict_t &monster)
{
	vec3_t v;

	if (face[0]||face[1]||face[2])
	{
		VectorSubtract (face, monster.s.origin, v);
		// uhhh.... this makes me a bit sick to my stomach...  guys shoot from a bit above their waist
		// so, bump the spot up
		v[2] -= MONSTER_SHOOT_HEIGHT;
	}
	//if no turn destination specified, turn toward movement destination
	else if (((monster.s.origin[0]-dest[0])||(monster.s.origin[1]-dest[1]))&&!VectorCompare(dest,vec3_origin))
	{
		VectorSubtract (dest, monster.s.origin, v);
	}
	else
	{
		VectorCopy(monster.s.angles,which_ai.ideal_angles);
		return;
	}

	//Enemy_Printf(&which_ai, "%f, %f, %f\n", v[0], v[1], v[2]);

	vectoangles(v, which_ai.ideal_angles);
}

//fixme: should not do this in actions!
qboolean action_c::AdjustBBox(ai_c &which_ai, edict_t &monster)
{
	if (which_ai.AttemptSetBBox(monster, anim->bbox))
	{
		return true;
	}
	return false;
}

//PerformBodilyFunctions:
//the boolean that gets returned tells caller whether to abort action (false == abort)
//the vector that gets passed in is set to reflect movement intention
qboolean action_c::PerformBodilyFunctions(ai_c &which_ai, edict_t &monster, vec3_t curDist)
{
	turninfo_s turninfo;
//				Com_Printf("action!--performing body functions!\n");
	//abortAction will be set when child scripted action fails
	if (abortAction || !which_ai.GetBody())
	{
		return false;
	}

	which_ai.GetBody()->UpdateFace(monster);
	((bodyorganic_c*)which_ai.GetBody())->UpdateSoundsByWeapon(monster, which_ai);
	InterpretFlags(NextFlags(which_ai, monster), which_ai, monster);

	AdjustBBox(which_ai, monster);

	//if i have a turn destination, turn toward it
	SetTurnDestination(which_ai, monster);//45
	NextTurn(1.0, which_ai, monster, turninfo);//70
	Turn(which_ai, monster, turninfo);//bout 70

	NextMovement(which_ai, monster, curDist);//bout 115
	return true;
}

qboolean action_c::Think(ai_c &which_ai, edict_t &monster)
{
	vec3_t mydist;

	if (!PerformBodilyFunctions(which_ai, monster, mydist))
	{
		ResolveAction(which_ai, monster, 0.0, 0.0);
		return true;
	}

	if (monster.movetype == MOVETYPE_STEP)
	{
		mydist[2]=0;
	}
	EvaluateMovement(which_ai, monster, mydist);
	if ((waitForAnim && which_ai.GetBody() && !which_ai.GetBody()->IsAnimationFinished()
		&& !which_ai.GetBody()->IsAnimationHeld(HOLDCODE_NO) && (level.time < timeout_time))
		|| (!waitForAnim && (level.time < timeout_time)))
	{
		return false;
	}
	//finish action pronto, and let owners know about it
	else
	{
		ResolveAction(which_ai, monster, 1.0, 1.0);
		return true;
	}
}

qboolean action_c::MatchAnimation(ai_c &which_ai, edict_t &monster)
{
	qboolean success;
	//just bein safe
	if (!which_ai.GetBody())
	{
		gi.dprintf("action::MatchAnimation--ai has no body!\n");
		return false;
	}
	success = which_ai.GetBody()->MatchAnimation(monster, anim);
	//my anim should always be valid, lemme know if it's not
	if (!success)
	{
//		gi.dprintf("action::MatchAnimation--sequence %s not found!\n", anim->ghoulSeqName);
	}
	return success;
}

qboolean action_c::SetAnimation(ai_c &which_ai, edict_t &monster)
{
	qboolean success;
	//just bein safe
	if (!which_ai.GetBody())
	{
		gi.dprintf("action::SetAnimation--ai has no body!\n");
		return false;
	}
	success = which_ai.GetBody()->SetAnimation(monster, anim);
	//my anim should always be valid, lemme know if it's not
	if (!success)
	{
//		gi.dprintf("action::SetAnimation--sequence %s not found!\n", anim->ghoulSeqName);
	}
	return success;
}

qboolean action_c::ForceAnimation(ai_c &which_ai, edict_t &monster)
{
	qboolean success;
	//just bein safe
	if (!which_ai.GetBody())
	{
		gi.dprintf("action::ForceAnimation--ai has no body!\n");
		return false;
	}
	success = which_ai.GetBody()->ForceAnimation(monster, anim);
	//my anim should always be valid, lemme know if it's not
	if (!success)
	{
//		gi.dprintf("action::ForceAnimation--sequence %s not found!\n", anim->ghoulSeqName);
	}
	return success;
}

void action_c::NextMovement(ai_c &which_ai, edict_t &monster, vec3_t curDist)
{
	//just bein safe
	if (!which_ai.GetBody())
	{
		gi.dprintf("action::NextMovement--ai has no body!\n");
		VectorClear(curDist);
		return;
	}
	which_ai.GetBody()->NextMovement(monster, curDist, velScale);
}

void action_c::NextTurn(float scale, ai_c &which_ai, edict_t &monster, turninfo_s &turninfo)
{
	//just bein safe
	if (!which_ai.GetBody())
	{
		gi.dprintf("action::NextTurn--ai has no body!\n");
		turninfo.max_turn=0.0;
		turninfo.min_turn=0.0;
		return;
	}
	which_ai.GetBody()->NextTurn(scale, monster, default_turn, turninfo);
}

int action_c::NextFlags(ai_c &which_ai, edict_t &monster)
{
	//just bein safe
	if (!which_ai.GetBody())
	{
		gi.dprintf("action::NextFlags--ai has no body!\n");
		return 0;
	}
	return which_ai.GetBody()->NextFlags(monster);
}

void action_c::InterpretFlags(int cur_flags, ai_c &which_ai, edict_t &monster)
{
	//if frameflags specify jump & this action is primed for jumping, then jump
	if (cur_flags & FRAMEFLAG_JUMP)
	{
		if (move_mask & jump_movemask)
		{
			Jump(which_ai, monster, jump_intent);
		}
	}

	//this ok? no keeping track of whether i've played this sound recently?
	if (cur_flags & FRAMEFLAG_LAND)
	{
//		if (monster.groundentity)
		{
//			gi.dprintf("<thud!>\n");
			switch(rand()%3)
			{
			default:
			case 0:
				gi.sound (&monster, CHAN_BODY, gi.soundindex ("impact/gore/fall1.wav"), 1.0, ATTN_NORM, 0);
				break;
			case 1:
				gi.sound (&monster, CHAN_BODY, gi.soundindex ("impact/gore/fall2.wav"), 1.0, ATTN_NORM, 0);
				break;
			case 2:
				gi.sound (&monster, CHAN_BODY, gi.soundindex ("impact/gore/fall3.wav"), 1.0, ATTN_NORM, 0);
				break;
			}
		}
	}

	//play footsteps
	if (cur_flags & FRAMEFLAG_FOOTSTEP)
	{
		FX_SetEvent(&monster, EV_FOOTSTEPLEFT);
	}

	//do an attack--call some weapon class attack routine, eventually?
	if (cur_flags & FRAMEFLAG_ATTACK)
	{
		if (which_ai.GetBody())
		{
			if (isLethal || NullTarget)
			{
				which_ai.setTarget(action_target);
			}
			which_ai.GetBody()->FirePrimaryWeapon(monster, isLethal, NullTarget);
			isLethal = false;
		}
	}

	if (cur_flags & FRAMEFLAG_THROW)
	{
		// need to call the actual attack function
		body_c* body = which_ai.GetBody();
		if (body)
		{
			body->ThrowProjectile(monster);
		}
		cur_flags &= ~FRAMEFLAG_THROW;
	}
	//frame holding--holds for random period of time, vey primitive.
	if (cur_flags & FRAMEFLAG_HOLDFRAME)
	{
	}
}

//jumpvec is horizontal movement--change so jump accepts goal position?
void action_c::Jump (ai_c &which_ai, edict_t &monster, vec3_t jumpvec)
{
	float frameboost;
	float jump_time, vertical_vel;
	float attempted_speed;

	//only go through with jump if on ground
	if (!monster.groundentity || monster.movetype == MOVETYPE_FLY)
	{
		return;
	}

	//abort jump if this action not set up for jumping
	if ( !(move_mask & jump_movemask) )
	{
		return;
	}

	monster.groundentity = NULL;

	//add a little extra velocity, just for this frame--minimum between gravity and jumpheight
	if (which_ai.GetJumpHeight() < sv_gravity->value*FRAMETIME)
	{
		frameboost = which_ai.GetJumpHeight();
	}
	else
	{
		frameboost = sv_gravity->value*FRAMETIME;
	}

	vertical_vel = jumpvec[2];
	jumpvec[2] = 0;

	attempted_speed = VectorLength(jumpvec);
	jump_time=((vertical_vel)/(sv_gravity->value*FRAMETIME))*2.0;

	//cap horizontal velocity so i don't jump farther than i'm supposed to
	if (jump_time * attempted_speed > which_ai.GetJumpDistance())
	{
		VectorNormalize(jumpvec);
		VectorScale(jumpvec, which_ai.GetJumpDistance() / jump_time, jumpvec);
		attempted_speed=which_ai.GetJumpDistance() / jump_time;
	}

	VectorCopy(jumpvec, monster.velocity);

	//fixme: don't have to jump this high; also, store value as jump velocity
	monster.velocity[2]=vertical_vel;//fixme? May be Possible to go higher than jumpheight, i don't think so tho 

	monster.groundentity = NULL;

	//store away when i expect to finish the jump, for vel adjustment
	jump_finish_time = level.time + (jump_time*FRAMETIME);
}


qboolean action_c::Move (ai_c &which_ai, edict_t &monster, vec3_t dist)//fixme?: do i want all movement here?
					//i'd like to have movement inherited,
					// with option for ai to replace it & do special kindsa movement
					//this is an abbreviated version of what q2 monsters go thru when they move (no fly or swim)

					//option to not relink was removed--add it in again, for mvmt tests?
{
	float		move_length;
	vec3_t		oldorg, neworg, end;
	trace_t		trace;
	float		stepsize;
	vec3_t		test;
	int			contents;
	trace_t		dbtrace;

// try the move	
	VectorCopy (monster.s.origin, oldorg);
	VectorAdd (monster.s.origin, dist, neworg);


	//hmmm...i don't especially like this, but to make ai stuff useful for non-humanoids it may have to stay. :(=
	if ( monster.movetype != MOVETYPE_STEP )
	{
		int i;

		move_length = VectorLengthSquared(dist);
		//if not really moving, just touch triggers and call it a successful move
		if (move_length<=0.01)
		{
//			G_TouchTriggers (&monster);
			return true;
		}
	// try one move with vertical motion, then one without
		for (i=0 ; i<2 ; i++)
		{
			VectorAdd (monster.s.origin, dist, neworg);
			if (i == 1)
			{
				neworg[2] = monster.s.origin[2];
			}
			
			if (monster.movetype == MOVETYPE_NOCLIP)
			{
				gi.trace (monster.s.origin, monster.mins, monster.maxs, neworg, &monster, 0, &trace);
			}
			else
			{
				gi.trace (monster.s.origin, monster.mins, monster.maxs, neworg, &monster, MASK_MONSTERSOLID, &trace);
			}
			
			// fly monsters don't enter water voluntarily
			if (monster.flags & FL_FLY)
			{
				if (!monster.waterlevel)
				{
					test[0] = trace.endpos[0];
					test[1] = trace.endpos[1];
					test[2] = trace.endpos[2] + monster.mins[2] + 1;
					contents = gi.pointcontents(test);
					if (contents & MASK_WATER)
						return false;
				}
			}

			// swim monsters don't exit water voluntarily
			if (monster.flags & FL_SWIM)
			{
				if (monster.waterlevel < 2)
				{
					test[0] = trace.endpos[0];
					test[1] = trace.endpos[1];
					test[2] = trace.endpos[2] + monster.mins[2] + 1;
					contents = gi.pointcontents(test);
					if (!(contents & MASK_WATER))
						return false;
				}
			}

			if (trace.fraction == 1)
			{
				//FIXME!!!!!!!!!!!!!!! if physics weren't broken, wouldn't need this!
				//	{	// actually covered some distance
						//test only--did last trace take me into a solid?!!!!!!!!!
						if (monster.movetype != MOVETYPE_NOCLIP)
						{
							gi.trace (trace.endpos, monster.mins, monster.maxs, trace.endpos, &monster, MASK_MONSTERSOLID, &dbtrace);
							if (dbtrace.startsolid || dbtrace.allsolid)
							{
								return false;
							}
						}
				//	}
				VectorCopy (trace.endpos, monster.s.origin);
				gi.linkentity (&monster);
//				G_TouchTriggers (&monster);
				return true;
			}
		}
		
		return false;
	}

	
	//don't allow movement if caught in midair
	if (!monster.groundentity)
	{
		return false;
	}

	//sliding, don't move.
	if (VectorLengthSquared(monster.velocity)>100)
	{
		return false;
	}

	move_length = VectorLengthSquared(dist);
	//if not really moving, just touch triggers and call it a successful move
	if (move_length<=0.01)
	{
//		G_TouchTriggers (&monster);
		return true;
	}

// push down from a step height above the wished position
	if (move_mask & step_movemask)
	{
		stepsize = which_ai.GetStepHeight();
	}
	else
	{
		stepsize = 2;//turning off stepping not recommended (will jerk around on small irregularities in floor),
						//but you can if you want.
	}

	neworg[2] += stepsize;
	VectorCopy (neworg, end);
	end[2] -= stepsize*2;

	gi.trace (neworg, monster.mins, monster.maxs, end, &monster, MASK_MONSTERSOLID, &trace);

	//if the trace started intersecting w/ solid, there's no headroom to step up--try to just step over & down
	if (trace.allsolid||trace.startsolid)
	{
		neworg[2] -= stepsize*0.5;
		gi.trace (neworg, monster.mins, monster.maxs, end, &monster, MASK_MONSTERSOLID, &trace);
		if (trace.allsolid || trace.startsolid)//this intersects w/ solid too, forget it
		{
			neworg[2] -= stepsize*0.5;
			gi.trace (neworg, monster.mins, monster.maxs, end, &monster, MASK_MONSTERSOLID, &trace);
			if (trace.allsolid || trace.startsolid)//this intersects w/ solid too, forget it
			{
//				gi.dprintf("monster %s cannot move--in solid!\n",monster.targetname);
				return false;
			}
		}
	}


	// don't go in to water
	if (monster.waterlevel == 0)
	{
		test[0] = trace.endpos[0];
		test[1] = trace.endpos[1];
		test[2] = trace.endpos[2] + monster.mins[2] + 1;	
		contents = gi.pointcontents(test);

		//fixme: this shouldn't be hardcoded to avoid h2o, some monsters shouldn't mind
		if (contents & MASK_WATER)
		{
			return false;
		}
	}

//commented out, because i want the option to fall (using act_checkbottom) in all cases--
//this move rejection ignores possibility of being able to fall further than stepheight,
//even if it is a quick trivial rejection; could restore this, with a check for fall_movemask?
/*	if (trace.fraction == 1)
	{
	// if monster had the ground pulled out, go ahead and fall
		if ( monster.flags & FL_PARTIALGROUND )
		{
			VectorAdd (monster.s.origin, move, monster.s.origin);
			gi.linkentity (&monster);
			G_TouchTriggers (&monster);
			monster.groundentity = NULL;
			return true;
		}
	
		return false;		// walked off an edge
	}
*/

//DANGER!! ACHTUNG!!! just because i uncommented this doesn't mean it's guaranteed to work fine!
	//FIXME!!!!!!!!!!!!!!! if physics weren't broken, wouldn't need this!
//	{	// actually covered some distance
		//test only--did last trace take me into a solid?!!!!!!!!!
		gi.trace (trace.endpos, monster.mins, monster.maxs, trace.endpos, &monster, MASK_MONSTERSOLID, &dbtrace);
		if (dbtrace.startsolid || dbtrace.allsolid)
		{
//			VectorScale(trace.endpos,9,trace.endpos);
//			VectorAdd(trace.endpos,neworg,trace.endpos);
//			VectorScale(trace.endpos,0.1,trace.endpos);

			//AAAAAAAAHHHHH!!! FIXME!!!! 
//			gi.trace (trace.endpos, monster.mins, monster.maxs, trace.endpos, &monster, MASK_MONSTERSOLID, &dbtrace);
//			if (dbtrace.startsolid || dbtrace.allsolid)
//			{
//#if _DEBUG
//				gi.dprintf("qphysics ERROR!--moved into solid!\n");
//#endif
				return false;
//			}
		}
//	}

	//if i landed on somebody, don't do the move!
	if (trace.ent && trace.ent != monster.groundentity && (trace.ent->client || trace.ent->ai)
		&& !(trace.ent->deadflag & DEAD_DEAD)//ok to walk on corpses(!?!)
		&& !(move_mask & fall_movemask)//ok to fall on guys
		&& trace.endpos[2]>trace.ent->s.origin[2]+trace.ent->maxs[2]+monster.mins[2]-1)
	{
		return false;
	}

	//move successful, unless i walked off an edge
	VectorCopy (trace.endpos, monster.s.origin);

	// check point traces down for dangling corners
	//checkbottom checks stepheight or fallheight beneath new position
	//(depending on move_mask); if there is room to land, the move is ok
	if (!gmonster.CheckBottom (which_ai, &monster, monster.s.origin, move_mask))
	{
		if (move_mask&fall_movemask)
		{
			vec3_t newtestpos;
			VectorCopy(monster.s.origin, newtestpos);
			newtestpos[2]-=20;
			gi.trace (monster.s.origin, monster.mins, monster.maxs, newtestpos, &monster, MASK_MONSTERSOLID, &dbtrace);
			if (!dbtrace.startsolid && !dbtrace.allsolid && dbtrace.fraction>0.8)
			{
				monster.flags &= ~FL_PARTIALGROUND;
				return false;
			}
			monster.flags |= FL_PARTIALGROUND;
			return false;
		}

		if ( monster.flags & FL_PARTIALGROUND )
		{	// entity had floor mostly pulled out from underneath it
			// and is trying to correct
			gi.linkentity (&monster);
//			G_TouchTriggers (&monster);
			return true;
		}
		if (gmonster.CheckBottom (which_ai, &monster, oldorg, move_mask))
		{
			VectorCopy (oldorg, monster.s.origin);
			return false;
		}
		else
		{
			gi.linkentity (&monster);
//			G_TouchTriggers (&monster);
			return true;
		}
	}

	//since move was successful, ground not being pulled out from under me
	if ( monster.flags & FL_PARTIALGROUND )
	{
		monster.flags &= ~FL_PARTIALGROUND;
	}
	monster.groundentity = trace.ent;
	monster.groundentity_linkcount = trace.ent->linkcount;

// the move is ok
	gi.linkentity (&monster);
//	G_TouchTriggers (&monster);

	return true;
}









#define MAX_PLAYERSPEED 225
#define SPEED_DODGE_EFFECT .6
#define DUCK_DODGE_EFFECT .3
#define JUMP_DODGE_EFFECT .4

#define MISSBY 48

#define DODGE_UNKNOWN	0
#define DODGE_DOWN		1
#define DODGE_UP		2
#define DODGE_LEFT		3
#define DODGE_RIGHT		4

void CalcPlayerAim(vec3_t ang, ai_c &which_ai, edict_t &monster, edict_t &target)
{
	float	chance;
	vec3_t	dif, dir;
	float	dist;
	float	distMod;
	float	aimCone;
	float	playerHitValue = 1.0;
	vec3_t	targMoveDir;
	float	targSpeed;
	int		missDirection = DODGE_UNKNOWN;
	float	strafeValue;
	float	angAdd;
	float	missAng;
	float	awakeTimeValue;

	VectorSubtract(target.s.origin, monster.s.origin, dif);

	if(target.client->ps.pmove.pm_flags & PMF_DUCKED)
	{	//aim lower for ducking folk
		dif[2] -= 20;//?
	}

	dist = VectorNormalize(dif);
	AngleVectors(monster.s.angles, dir, 0, 0);

	awakeTimeValue = level.time - which_ai.getFirstTargetTime() - (which_ai.GetMySkills()->getHesitation()*game.playerSkills.getHesitation());

	// calculate the effect of distance on the aiming
	if(dist < 64)
	{	// closer than this, and accuracy starts getting REALLY good
		chance = 1.0;
	}
	else
	{
		if(dist < 512)
		{
			distMod = 1.0;
		}
		else
		{	// steep curve fall off
			distMod = 512 / dist;
			distMod *= distMod;
		}

		// calculate the effect of player dodging on the shooting
		playerHitValue = 1.0;
		if(target.client->ps.pmove.pm_flags & PMF_DUCKED)
		{
			playerHitValue *= 1.0 - DUCK_DODGE_EFFECT;
			missDirection = DODGE_DOWN;
		}
		if(!(target.client->ps.pmove.pm_flags & PMF_ON_GROUND))
		{
			playerHitValue *= 1.0 - JUMP_DODGE_EFFECT;
			missDirection = DODGE_UP;
		}

		//conventional maximum speed is 300, it would appear
		VectorCopy(target.velocity, targMoveDir);
		targSpeed = VectorNormalize(targMoveDir);
		//basically, moving perpendicular to someone targetting you at high speeds is a dodge
		//moving towards or away doesn't help at all
		strafeValue = 1.0 - fabs(DotProduct(targMoveDir, dir));
		playerHitValue *= 1.0 - (targSpeed/MAX_PLAYERSPEED)*SPEED_DODGE_EFFECT*strafeValue;
		if(strafeValue * (targSpeed/MAX_PLAYERSPEED) > .1)
		{	// enough strafing that we should base our misfires off of it
			vec3_t fakeSide;
			//not a proper rotation, but good enough for now
			fakeSide[0] = dif[1];
			fakeSide[1] = -dif[0];
			fakeSide[2] = dif[2];

			if(DotProduct(fakeSide, targMoveDir) > 0)
			{
				missDirection = DODGE_LEFT;
			}
			else
			{
				missDirection = DODGE_RIGHT;//?
			}
		}

		chance = which_ai.GetMySkills()->getAccuracy() * DotProduct(dif, dir) * distMod *
			game.playerSkills.getAccuracy() * playerHitValue;
	}

	if(awakeTimeValue < which_ai.GetMySkills()->getAimTime()*game.playerSkills.getHesitation())
	{
		float extraChance;
		extraChance = awakeTimeValue / (which_ai.GetMySkills()->getAimTime()*game.playerSkills.getHesitation());//not ready, I say
		if(extraChance < 0)
		{
			extraChance = 0;
		}
		if(extraChance > 1.0)
		{
			extraChance = 1.0;
		}
		chance *= extraChance;
	}

	if(dist >= 64 && dist < 128)
	{//secondary close range
		chance = chance * ((dist - 64.0)/64.0) + (1.0 - ((dist - 64.0)/64.0));
	}

	vectoangles(dif, ang);

	angAdd = atan2(MISSBY, dist) * 180/M_PI;

	ang[YAW] += gi.flrand(-3, 3);
	ang[PITCH] += gi.flrand(-3, 3);

	if(gi.flrand(0,1) > chance)
	{	// missed
		switch(missDirection)
		{
		case DODGE_UNKNOWN:
			missAng = gi.flrand(0, 3.14);
			ang[YAW] += cos(missAng) * angAdd * 1.0;
			ang[PITCH] -= sin(missAng) * angAdd * .5;
			break;
		case DODGE_UP:
			ang[PITCH] += angAdd;
			ang[YAW] += gi.flrand(-10, 10);
			break;
		case DODGE_DOWN:
			ang[PITCH] -= angAdd;
			ang[YAW] += gi.flrand(-10, 10);
			break;
		case DODGE_LEFT:
			ang[YAW] += angAdd;//? I don't know what left and right mean here =/  I"ll just make 'em work, yeah.
			ang[PITCH] += gi.flrand(-10, 10);
			break;
		case DODGE_RIGHT:
			ang[YAW] -= angAdd;
			ang[PITCH] += gi.flrand(-10, 10);
			break;
		}
	}
	else
	{
		ang[PITCH] += gi.flrand(angAdd * -.3, angAdd * .3);
		ang[YAW] += gi.flrand(angAdd * -.3, angAdd * .3);
	}

	//keep it in range
	if (which_ai.GetBody()&&which_ai.GetBody()->GetMove())
	{
		aimCone = which_ai.GetAimConeDegrees(monster, which_ai.GetBody()->GetMove()->bbox);
	}
	else
	{
		aimCone = 1.0;
	}
	while(ang[YAW] - monster.s.angles[YAW] > 180)ang[YAW] -= 360;
	while(ang[YAW] - monster.s.angles[YAW] < -180)ang[YAW] += 360;
	if(ang[YAW] - monster.s.angles[YAW] > aimCone)ang[YAW] = monster.s.angles[YAW] + aimCone;
	if(ang[YAW] - monster.s.angles[YAW] < -aimCone)ang[YAW] = monster.s.angles[YAW] - aimCone;
}

#define AIM_DEVIATION_PITCHMOD		0.5

#define AIM_DEVIATION_INITIAL		0.75
#define	AIM_DEVIATION_FINALSHAKE	0.25

#define AIMDIST_MIN 92

void getDistDeviate(vec3_t org1, vec3_t org2, int isPlayer, float *scale, float *distDeviate, CAISkills &skills)
{
	vec3_t dif;
	VectorSubtract(org2, org1, dif);
	float dist = VectorLength(dif);

	if(isPlayer)
	{	// turn faster the closer a player is
		if(dist < AIMDIST_MIN)
		{
			*scale = 2.0;
		}
		else if(dist > 512 * game.playerSkills.getAimMaxDist())
		{
			*scale = 1.0;
		}
		else
		{	// the internal value runs from 0 up close to 1 far away
			*scale = 2.0 - (dist - AIMDIST_MIN)/((512 * game.playerSkills.getAimMaxDist()) - AIMDIST_MIN);
		}
	}
	else
	{	// turn faster the closer the guy is - more severe than for regular player turning
		if(dist < AIMDIST_MIN)
		{
			*scale = 10.0;
		}
		else if(dist > 512 * game.playerSkills.getAimMaxDist())
		{
			*scale = 1.0;
		}
		else
		{	// the internal value runs from 0 up close to 1 far away
			*scale = 10.0 - (dist - AIMDIST_MIN)/((512 * game.playerSkills.getAimMaxDist()) - AIMDIST_MIN) * 10.0;
		}
	}

	if(dist > 512 * game.playerSkills.getAimMaxDist())
	{
		*distDeviate += (dist - (512 * game.playerSkills.getAimMaxDist()))/256.0;
	}
	else if(dist < AIMDIST_MIN)
	{
		*distDeviate = (dist / AIMDIST_MIN)*(dist / AIMDIST_MIN);
	}

	if(!isPlayer)
	{	//more likely to hit non-players
		*distDeviate *= .4;
	}
}

void action_c::Aim(ai_c &which_ai, edict_t &monster)
{
	//if i'm just standing around with nothing to do, don't bother aiming
	if (!which_ai.HasHadTarget())
	{
		which_ai.SetAimAngles(monster.s.angles);
		return;
	}

	assert((!which_ai.HasTarget())||(!which_ai.getTarget())||(which_ai.getTarget()->inuse < 10));
	edict_t *ed = which_ai.getTarget();
	if(which_ai.HasTarget() && ed && ed->client)
	{	// players are a bit of a special case
		vec3_t aimAngs;

		CalcPlayerAim(aimAngs, which_ai, monster, *which_ai.getTarget());

		which_ai.SetAimAngles(aimAngs);
		return;
	}

	float	ideal;
	float	current;
	float	move;
	vec3_t	my_aim;
	vec3_t	my_targ_pos;

	float	tempAng;
	float	aimCone;
	float	aimDeviation;

	//adjust for distance to target
	
	float	distScale = 1.0;
	float	distDeviate = 1.0;

	which_ai.getTargetPos(my_targ_pos);

	if(which_ai.getTarget())
	{
		getDistDeviate(my_targ_pos, monster.s.origin, (which_ai.getTarget()->client != 0), &distScale, &distDeviate, *(which_ai.GetMySkills()));
	}

	which_ai.GetAimAngles(my_aim);
	
	//yaw aim first

	if (which_ai.GetBody()&&which_ai.GetBody()->GetMove())
	{
		aimCone = which_ai.GetAimConeDegrees(monster, which_ai.GetBody()->GetMove()->bbox);
	}
	else
	{
		aimCone = 1.0;
	}
	aimDeviation = which_ai.GetAimDeviationDegrees(monster) * distDeviate * (1.0 - which_ai.GetMySkills()->getAccuracy()) * game.playerSkills.getAccuracy();
	tempAng = anglemod(which_ai.ideal_angles[YAW]-monster.s.angles[YAW]);

	//if i'm scripted to do this, do it right!
	if (owner_decision && ((owner_decision->GetClassCode() == SCRIPTED_DECISION) || (owner_decision->GetClassCode() == PATHCOMBAT_DECISION)
		|| (owner_decision->GetClassCode() == PATHIDLE_DECISION)))
	{
		VectorCopy(which_ai.ideal_angles, my_aim);
		aimDeviation = 0;
	}

	current = my_aim[YAW];

	//make sure roll is cleared out
	my_aim[ROLL]=0;


	//i'm inside the aim cone...use ideal_angles for aiming
	if (tempAng < aimCone || tempAng > 360.0-aimCone)
	{
		ideal = anglemod(which_ai.ideal_angles[YAW]+gi.flrand(-1.0,1.0)*aimDeviation*AIM_DEVIATION_INITIAL);
	}
	//i'm outside the cone--use the edge of the cone
	else if (tempAng < 180.0)
	{
		ideal = anglemod(monster.s.angles[YAW]+aimCone+gi.flrand(-1.0,1.0)*aimDeviation*AIM_DEVIATION_INITIAL);
	}
	//outside on the other side
	else
	{
		ideal = anglemod(monster.s.angles[YAW]-aimCone+gi.flrand(-1.0,1.0)*aimDeviation*AIM_DEVIATION_INITIAL);
	}


	move = ideal - current;

	//make sure we take shortest route to desired yaw
	if (ideal > current)
	{
		if (move >= 180)
		{
			move = move - 360;
		}
	}
	else
	{
		if (move <= -180)
		{
			move = move + 360;
		}
	}

	float moveScale = distScale;

	//only make the aim adjustment if there's a significant difference between ideal & 
	if (move < 30 && move > -30)
	{
		moveScale *= 0.125;
	}
	else if (move < 60 && move > -60)
	{
		moveScale *= 0.25;
	}

	if(moveScale < 1.0)
	{
		move *= moveScale;
	}



	aimDeviation *= .4;//up and down should be a bit more accurate than left and right - just looks better

	//adjust aiming yaw
	my_aim[YAW] = anglemod (current + move  + gi.flrand(-1.0,1.0)*aimDeviation*AIM_DEVIATION_FINALSHAKE);
	
	//now pitch aim--don't bother with the cone here

	current = my_aim[PITCH];
	ideal = anglemod(which_ai.ideal_angles[PITCH]+gi.flrand(-1.0,1.0)*aimDeviation*AIM_DEVIATION_INITIAL*AIM_DEVIATION_PITCHMOD);

	move = ideal - current;

	//make sure we take shortest route to desired yaw
	if (ideal > current)
	{
		if (move >= 180)
		{
			move = move - 360;
		}
	}
	else
	{
		if (move <= -180)
		{
			move = move + 360;
		}
	}

	moveScale = distScale;

	//only make the aim adjustment if there's a significant difference between ideal & 
	if (move < 15 && move > -15)
	{
		moveScale *= 0.125;
	}
	else if (move < 30 && move > -30)
	{
		moveScale *= 0.25;
	}
	
	if(moveScale < 1.0)
	{
		move *= moveScale;
	}
	
	//adjust aiming pitch
	my_aim[PITCH] = anglemod (current + move + gi.flrand(-1.0,1.0)*aimDeviation*AIM_DEVIATION_FINALSHAKE*AIM_DEVIATION_PITCHMOD); 

	which_ai.SetAimAngles(my_aim);
}

qboolean action_c::Turn(ai_c &which_ai, edict_t &monster, turninfo_s &turninfo)
{
	float	ideal;
	float	current;
	float	move;

	if (which_ai.IsConcentratingOnPlayer()&&monster.health>0)
	{
		if (level.sight_client)
		{
			vec3_t toPlayer;
			VectorSubtract(level.sight_client->s.origin, monster.s.origin, toPlayer);
			toPlayer[2]=0;
			VectorNormalize(toPlayer);
			vectoangles(toPlayer, monster.s.angles);
			return true;
		}
	}
	
	current = anglemod(monster.s.angles[YAW]);

//	if monster's anim is flagged as a backwards, left or right scripted action, add to the ideal_angles!
	if (owner_decision && ((owner_decision->GetClassCode() == SCRIPTED_DECISION) || (owner_decision->GetClassCode() == PATHCOMBAT_DECISION)
		|| (owner_decision->GetClassCode() == PATHIDLE_DECISION)))
	{
		if (anim->actionFlags & ACTFLAG_BACKUP)
		{
			ideal = anglemod(which_ai.ideal_angles[YAW] + 180);
		}
		else if (anim->actionFlags & ACTFLAG_LEFTSTRAFE)
		{
			ideal = anglemod(which_ai.ideal_angles[YAW] + 90);
		}
		else if (anim->actionFlags & ACTFLAG_RIGHTSTRAFE)
		{
			ideal = anglemod(which_ai.ideal_angles[YAW] - 90);
		}
		else
		{
			ideal = anglemod(which_ai.ideal_angles[YAW]);
		}
	}
	else
	{
		ideal = anglemod(which_ai.ideal_angles[YAW]);
	}

	//already facing ideal direction; consider turn successful
	if (current == ideal &&(turninfo.max_turn >= 0 && turninfo.min_turn <= 0))
	{
		Aim(which_ai, monster);
		return true;
	}

	move = ideal - current;

	//make sure we take shortest route to desired yaw
	if (ideal > current)
	{
		if (move >= 180)
		{
			move = move - 360;
		}
	}
	else
	{
		if (move <= -180)
		{
			move = move + 360;
		}
	}

	//cap turning speed to yawspeed
//	if (move > 0)
//	{
//		if (move > turninfo.max_turn)
//		{
//			move = turninfo.max_turn;
//		}
//	}
//	else
//	{
//		if (move < turninfo.min_turn)
//		{
//			move = turninfo.min_turn;
//		}
//	}

	if (move > turninfo.max_turn)
	{
		if (ai_pathtest->value && monster.health<=0)//debug breakpoint
		{
			gi.dprintf("dead monster forcing turn at %3.3f degrees.\n", turninfo.max_turn);
		}
		move = turninfo.max_turn;
	}
	else if (move < turninfo.min_turn)
	{
		if (ai_pathtest->value && monster.health<=0)//debug breakpoint
		{
			gi.dprintf("dead monster forcing turn at %3.3f degrees.\n", turninfo.min_turn);
		}
		move = turninfo.min_turn;
	}
	
	monster.s.angles[YAW] = anglemod (current + move);

	Aim(which_ai, monster);

	return true;
}

//EvaluateMovement no longer checks if we've reached goal position, just checks if we've been blocked
qboolean action_c::EvaluateMovement(ai_c &which_ai, edict_t &monster, vec3_t distance)
{
	vec3_t vel_adjust,dir_to_dest,dist_dir;
	VectorScale(which_ai.velocity, 0.9, which_ai.velocity);

	VectorCopy(distance, dist_dir);
	VectorNormalize(dist_dir);
	VectorSubtract(dest, monster.s.origin, dir_to_dest);
	VectorNormalize(dir_to_dest);

	//going exactly the wrong way. hmp.
	if (DotProduct(dist_dir, dir_to_dest)<0)
	{
//		return false;
//		VectorScale(distance,-1.0,distance);
	}

	//move forward the amount current frame specifies
	if (!Move (which_ai, monster, distance))//blocked--finish unsuccessful
	{
		//i'm stopped, dammit
		VectorClear(which_ai.velocity);
		return false;
	}

	//make sure i know if i'm stopped on purpose
	if (VectorLengthSquared(distance)<=1)
	{
		VectorClear(which_ai.velocity);
	}
	else
	{
		VectorScale(distance, 0.1, vel_adjust);
		VectorAdd(which_ai.velocity, vel_adjust, which_ai.velocity);
	}

	return true;//still walking--let ai know i'm not done yet
}

void action_c::BeginAction(ai_c &which_ai, edict_t &monster)
{
	if (!abortAction)
	{
		if (which_ai.GetBody())
		{
			if (which_ai.GetBody()->IsAnimationFinished() && anim && !(anim->actionFlags&ACTFLAG_LOOPANIM))
			{
				ForceAnimation(which_ai, monster);
			}
			else
			{
				SetAnimation(which_ai, monster);
			}
		}
		start_time = level.time;
		timeout_time = start_time + timeout_interval;
	}
}

void action_c::SetSignalEventState (qboolean done)
{
	if (SignalEvent)
	{
		SignalEvent->SetEventState(done);
	}
	return;
}

action_c::action_c(action_c *orig)
{
	*(int *)&anim		= GetMmoveNum(orig->anim);

	VectorCopy(orig->face, face);
	VectorCopy(orig->dest, dest);

	faceUpdatedTime		= orig->faceUpdatedTime;
	interrupted			= orig->interrupted;
	abortAction			= orig->abortAction;

	owner_decision.MakeIndex(orig->owner_decision);
	owner_action.MakeIndex(orig->owner_action);

	velScale			= orig->velScale;
	default_turn		= orig->default_turn;
	move_mask			= orig->move_mask;
	start_time			= orig->start_time;		 
	timeout_interval	= orig->timeout_interval;
	timeout_time		= orig->timeout_time;	 

	VectorCopy(orig->jump_intent, jump_intent);

	jump_finish_time	= orig->jump_finish_time;
	waitForAnim			= orig->waitForAnim;
	*(int *)&SignalEvent= GetEventNum(orig->SignalEvent);
	isLethal			= orig->isLethal;
	NullTarget			= orig->NullTarget;

	*(int *)&action_target = GetEdictNum(orig->action_target);
}

void action_c::Evaluate(action_c *orig)
{
	anim = GetMmovePtr((int)orig->anim);
	
	VectorCopy(orig->face, face);
	VectorCopy(orig->dest, dest);

	faceUpdatedTime		= orig->faceUpdatedTime;
	interrupted			= orig->interrupted;
	abortAction			= orig->abortAction;

	owner_decision.MakePtr(*(int *)&orig->owner_decision);
	owner_action.MakePtr(*(int *)&orig->owner_action);

	velScale			= orig->velScale;
	default_turn		= orig->default_turn;
	move_mask			= orig->move_mask;
	start_time			= orig->start_time;		 
	timeout_interval	= orig->timeout_interval;
	timeout_time		= orig->timeout_time;	 

	VectorCopy(orig->jump_intent, jump_intent);

	jump_finish_time	= orig->jump_finish_time;
	waitForAnim			= orig->waitForAnim;
	SignalEvent			= GetEventPtr((int)orig->SignalEvent);
	isLethal			= orig->isLethal;
	NullTarget			= orig->NullTarget;

	action_target = GetEdictPtr((int)orig->action_target);
}

/**********************************************************************************
 **********************************************************************************/

move_action::move_action(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing, float timeout, qboolean fullAnimation)
:action_c(od, oa, newanim, destination, facing, NULL, timeout,fullAnimation)
{
	VectorCopy(destination, dest);
}

void move_action::Init(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing, float timeout, qboolean fullAnimation)
{
	action_c::Init(od, oa, newanim, destination, facing, NULL, timeout,fullAnimation);
	VectorCopy(destination, dest);
}

qboolean move_action::Think(ai_c &which_ai, edict_t &monster)
{
	vec3_t	mydist;
	qboolean moveSuccess;

	//stack of actions has been toppled--abort! abort!
	//the only place abortAction is currently set true is when an owned action returns false--
	//and the only time actions can now own each other is when scripting decisions stack them up
	if (!PerformBodilyFunctions(which_ai, monster, mydist))
	{
		ResolveAction(which_ai, monster, 0.0, 0.0);
		return true;
	}

	//fixme: z move should be ok!
	mydist[2]=0;

	bool	bTest1 = !!(moveSuccess=EvaluateMovement(which_ai, monster, mydist)),
			bTest2 = !!waitForAnim,
			bTest3 = !!which_ai.GetBody(),
			bTest4 = !!(bTest3?which_ai.GetBody()->IsAnimationFinished():false),
			bTest5 = !!(bTest3?which_ai.GetBody()->IsAnimationHeld(HOLDCODE_NO):false),
			bTest6 = (level.time > timeout_time),
			bTest7 = !!(anim->actionFlags & ACTFLAG_FULLANIM);

	// kef 10/12/99 -- here's the old way. it would stop an anim as soon as it finished its first 
	//cycle even if the anim was supposed to be held for a long period of time
	//(e.g. "holding for 30 seconds" in a script wouldn't work)
//	if(	(!bTest1 || (bTest2 && bTest3 && (bTest4 || bTest5 || bTest6)) || (!bTest2 && bTest6)) &&
//		(!bTest7 || bTest4) )
	if(	(!bTest1 || (bTest2 && bTest4 && (bTest5 || bTest6)) || (!bTest2 && bTest6)) &&
		(!bTest7 || bTest4) )
	{//movement failed, or action timed out--abort.
		ResolveAction(which_ai, monster, 0.0, 0.0);
		return true;
	}
	//movement successful
	else
	{
		float	dist_to_goal_sq;//only in xy here
	
		//check to see if i'm close enough (only checking xy) to stop action
		dist_to_goal_sq=(monster.s.origin[0]-dest[0])*(monster.s.origin[0]-dest[0])+
			(monster.s.origin[1]-dest[1])*(monster.s.origin[1]-dest[1]);

		if ((!waitForAnim && dist_to_goal_sq <= (VectorLengthSquared(mydist)+1)*9)
			&&(!((anim->actionFlags & ACTFLAG_FULLANIM)&&(!which_ai.GetBody()->IsAnimationFinished()))))//fixme: should do a more accurate check,
						//calculating my turning radius, and checking off of that so i don't circle a point
		{//reached destination--return successful
			ResolveAction(which_ai, monster, 1.0, 1.0);
			return true;
		}

		//still going--let ai know to not get rid of me
		return false;
	}
}

move_action::move_action(move_action *orig)
: action_c(orig)
{
}

void move_action::Evaluate(move_action *orig)
{
	action_c::Evaluate(orig);
}

/**********************************************************************************
 **********************************************************************************/
pain_action::pain_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *other, float kick, int damage, float timeout, qboolean fullAnimation):
action_c(od,oa,newanim,monster.s.origin,vec3_origin,NULL,timeout,fullAnimation){}

void pain_action::BeginAction(ai_c &which_ai, edict_t &monster)
{
	monster.s.sound = 0;
	monster.s.sound_data = 0;

	start_time = level.time;
	timeout_time = start_time + timeout_interval;

	//don't start over if i've been interrupted...
	if (!interrupted)
	{
		ForceAnimation(which_ai, monster);
	}
}

void pain_action::SetTurnDestination(ai_c &which_ai, edict_t &monster)
{
	VectorCopy(monster.s.angles, which_ai.ideal_angles);
}

qboolean pain_action::Think(ai_c &which_ai, edict_t &monster)
{ 
	vec3_t mydist;

	//allow aborting of pain action, so pain can be stacked in scripted events.
	if (!PerformBodilyFunctions(which_ai, monster, mydist))
	{
		//let owners know i'm finished
		ResolveAction(which_ai, monster, 0.0, 0.0);
		return true;
	}

	EvaluateMovement(which_ai, monster, mydist);

	if ((waitForAnim && which_ai.GetBody() && !which_ai.GetBody()->IsAnimationFinished()
		&& !which_ai.GetBody()->IsAnimationHeld(HOLDCODE_NO) && (level.time < timeout_time))
		|| (!waitForAnim && (level.time < timeout_time)))
	{
		return false;
	}
	//let owners know i'm finished
	//abort previous action, tell decision that an action failed
	ResolveAction(which_ai, monster, 0.0, 0.0);

	return true;

}

pain_action::pain_action(pain_action *orig)
: action_c(orig)
{
}

void pain_action::Evaluate(pain_action *orig)
{
	action_c::Evaluate(orig);
}

void pain_action::Read()
{
	char	loaded[sizeof(pain_action)];

	gi.ReadFromSavegame('AIPA', loaded, sizeof(pain_action));
	Evaluate((pain_action *)loaded);
}

void pain_action::Write()
{
	pain_action	*savable;

	savable = new pain_action(this);
	gi.AppendToSavegame('AIPA', savable, sizeof(*this));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/
death_action::death_action(decision_c *od, action_c *oa, mmove_t *newanim1, mmove_t *newanim2, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
:action_c(od,oa, newanim1)
{
	if (!newanim1)
	{
		anim=newanim2;
	}
	anim2 = newanim2;
	time_of_death = level.time;
	drip = false;
}

death_action::death_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
:action_c(od,oa, newanim)
{
	anim2 = NULL;
	time_of_death = level.time;
	drip = false;
}

void death_action::InterpretFlags(int cur_flags, ai_c &which_ai, edict_t &monster)
{
	if (cur_flags & FRAMEFLAG_OOZE)
	{
		vec3_t	curVec;

		VectorCopy(monster.s.origin, curVec);
		curVec[2] += 10;

		FX_MakeDecalBelow(curVec, FXDECAL_BLOODPOOL, 0);
	}
	action_c::InterpretFlags(cur_flags&~FRAMEFLAG_OOZE,which_ai,monster);
}

#define MAX_SEEN_ENEMIES 5
#define MONSTER_CORPSE_FADE		3.0F

qboolean death_action::SafeToRemove(edict_t &monster)
{
	int i;
	edict_t *client;
	vec3_t client_facing, client_to_body;

	// If it's gibbed, then all parts are turned off anyway.
	if (monster.gibbed)
	{
		return true;
	}

	// See if already fading
	if (monster.s.effects & EF_FADE)
	{
		if (FXA_CheckFade(&monster))
		{	// Remove that sucker
			return true;
		}

		//I'm in the process of fading--don't bother doing anything else until I'm done disappearing
		return false;
	}

	//if we are seeing too many guys, corpse is outta here no matter what
	if((gmonster.GetSeenEnemyCount() >= ai_maxcorpses->value)&&(!game.cinematicfreeze)&&(!OnSameTeam(&monster, level.sight_client))) 
	{
		if (level.time > time_of_death + 3)
		{
			if(monster.ghoulInst)
			{
				FXA_SetFadeOut(&monster, MONSTER_CORPSE_FADE);		// Three second fade out time.

				return false;//now we start fading
			}
			else
			{
				return true;
			}
		}
	}
	else
	{//corpse count as seen enemies too
		gmonster.AddSeenEnemy();
	}

	//don't consider removing until after a while
	if (level.time-time_of_death < 15.0 * ai_maxcorpses->value)
	{
		return false;
	}
	//only remove if it won't look screwy to clients
	for (i=0 ; i<game.maxclients ; i++)
	{
		client = &g_edicts[1+i];
		if (!client->inuse)
		{
			continue;
		}
		VectorSubtract(monster.s.origin, client->s.origin, client_to_body);

		//if e's greater than 800 units away, assume he can't see me
		if (VectorLengthSquared(client_to_body)>640000)
		{
			continue;
		}

		if (!gi.inPVS(monster.s.origin, monster.s.origin))
		{
			continue;
		}
		AngleVectors(client->s.angles, client_facing, NULL, NULL);
		VectorNormalize(client_to_body);
		//if client is looking in my general dir, don't remove for sure
		if (DotProduct(client_to_body, client_facing)>-.35) // this may seem excessive, but it since bodies are so big, 
			// we need to allow for some leeway.
		{
			time_of_death = level.time - 25;
			return false;
		}
	}

	//danger: if time_of_death gets used for anything other than in this func, replace this with something else!!!!
	time_of_death = level.time - 25;
	return true;
}

#define MAX_FLOOR_ALIGN_INCREMENT 2.5

qboolean death_action::AdjustBBox(ai_c &which_ai, edict_t &monster)
{
	if (which_ai.AttemptSetBBox(monster, anim->bbox, true))
	{
		return true;
	}
	return false;
}

void death_action::TiltToFloor(ai_c &which_ai, edict_t &monster)
{
	trace_t trace;
	vec3_t up, down, forward, ftemp, right, tangles;
	float rotateDist;
	vec3_t normal, norm_accum, norm_temp, norm_compare;

	//corpse-angling code: now in extra-balky!
	VectorCopy(monster.s.origin, norm_compare);
	norm_compare[2]+=monster.mins[2]+1;
	VectorClear(norm_accum);

	VectorCopy(monster.s.origin, down);
	VectorAdd(down, monster.mins, down);
	VectorCopy(down, up);
	up[2]+=1;
	down[2]-=20;
	gi.trace(up, vec3_origin, vec3_origin, down, &monster, MASK_DEADSOLID, &trace);
	if (trace.allsolid || trace.startsolid)
	{
		VectorSubtract(up, norm_compare, norm_temp);
	}
	else
	{
		VectorSubtract(trace.endpos, norm_compare, norm_temp);
	}
	VectorScale(norm_temp, -1.0, norm_temp);
	norm_temp[2]*=16.0;
	VectorNormalize(norm_temp);
	VectorAdd(norm_temp, norm_accum, norm_accum);

	down[0]+=monster.maxs[0]-monster.mins[0];
	up[0]+=monster.maxs[0]-monster.mins[0];
	gi.trace(up, vec3_origin, vec3_origin, down, &monster, MASK_DEADSOLID, &trace);
	if (trace.allsolid || trace.startsolid)
	{
		VectorSubtract(up, norm_compare, norm_temp);
	}
	else
	{
		VectorSubtract(trace.endpos, norm_compare, norm_temp);
	}
	VectorScale(norm_temp, -1.0, norm_temp);
	norm_temp[2]*=16.0;
	VectorNormalize(norm_temp);
	VectorAdd(norm_temp, norm_accum, norm_accum);

	down[1]+=monster.maxs[1]-monster.mins[1];
	up[1]+=monster.maxs[1]-monster.mins[1];
	gi.trace(up, vec3_origin, vec3_origin, down, &monster, MASK_DEADSOLID, &trace);
	if (trace.allsolid || trace.startsolid)
	{
		VectorSubtract(up, norm_compare, norm_temp);
	}
	else
	{
		VectorSubtract(trace.endpos, norm_compare, norm_temp);
	}
	VectorScale(norm_temp, -1.0, norm_temp);
	norm_temp[2]*=16.0;
	VectorNormalize(norm_temp);
	VectorAdd(norm_temp, norm_accum, norm_accum);

	down[0]-=monster.maxs[0]-monster.mins[0];
	up[0]-=monster.maxs[0]-monster.mins[0];
	gi.trace(up, vec3_origin, vec3_origin, down, &monster, MASK_DEADSOLID, &trace);
	if (trace.allsolid || trace.startsolid)
	{
		VectorSubtract(up, norm_compare, norm_temp);
	}
	else
	{
		VectorSubtract(trace.endpos, norm_compare, norm_temp);
	}
	VectorScale(norm_temp, -1.0, norm_temp);
	norm_temp[2]*=16.0;
	VectorNormalize(norm_temp);
	VectorAdd(norm_temp, norm_accum, norm_accum);

	VectorNormalize(norm_accum);
	VectorCopy(norm_accum, normal);

//	if (trace.fraction < 1.0)
//	{
//		VectorCopy(trace.plane.normal, normal);
		//get current orientation
		AngleVectors(monster.s.angles,forward,right,up);

		//find out how much current orientation deviates from ideal
		rotateDist = -DotProduct(forward, normal)*90.0;
		if (rotateDist > MAX_FLOOR_ALIGN_INCREMENT)
		{
			rotateDist = MAX_FLOOR_ALIGN_INCREMENT;
		}
		else if (rotateDist < -MAX_FLOOR_ALIGN_INCREMENT)
		{
			rotateDist = -MAX_FLOOR_ALIGN_INCREMENT;
		}
		//rotate me to face ideal
		VectorCopy(forward, ftemp);
		RotatePointAroundVector(forward, right, ftemp, rotateDist);

		//convert to angular form
		vectoangles(forward, monster.s.angles);

		//...now do the same for roll...

		//find out how much current orientation deviates from ideal
		rotateDist = DotProduct(right, normal)*90.0;
		if (rotateDist > MAX_FLOOR_ALIGN_INCREMENT)
		{
			rotateDist = MAX_FLOOR_ALIGN_INCREMENT;
		}
		else if (rotateDist < -MAX_FLOOR_ALIGN_INCREMENT)
		{
			rotateDist = -MAX_FLOOR_ALIGN_INCREMENT;
		}
		//rotate me to face ideal
		VectorCopy(right, ftemp);
		RotatePointAroundVector(right, forward, ftemp, rotateDist);

		//convert to angular form
		vectoangles(right, tangles);

		//combine pitch & roll calcs
		monster.s.angles[ROLL] = tangles[PITCH];
//	}

	//quick lil sliding test, to see how easy it would be to put slidy corpses in...looks good so far...
	if (normal[2]>0)
	{
		normal[2]=0;
		if (VectorLengthSquared(normal)>0.0025/*0.05*/)
		{
			VectorScale(normal, 100, normal);
			if (fabs(monster.velocity[0])<fabs(normal[0]))
			{
				monster.velocity[0]=normal[0];
			}
			if (fabs(monster.velocity[1])<fabs(normal[1]))
			{
				monster.velocity[1]=normal[1];
			}
			if (!monster.velocity[2])
			{
				monster.velocity[2]=sv_gravity->value*FRAMETIME+10;
			}
		}
	}
	else
	{
//		gi.dprintf("Warning! corpse under ground!\n");
	}
}
/*
  BLAH this was started and never finished! 
void death_action::AlternateTiltToFloor(ai_c &which_ai, edict_t &monster)
{
	vec3_t			forwardTest,middleTest,backTest;
	vec3_t			forwardGround, middleGround, backGround;
	vec3_t			forward,right,up,backward;
	float			halfHeight;
	trace_t			trace;


	//set up three start points
	halfHeight = monster.mins[2]+monster.maxs[2];
	VectorCopy(monster.s.origin, middleTest);
	middleTest[2]+=halfHeight;
	AngleVectors(monster.s.angles, forward, right, up);
	forward[2]=0;
	VectorNormalize(forward);
	VectorScale(forward,16,forward);
	VectorAdd(forward,middleTest,forwardTest);
	VectorScale(forward,-1,backward);
	VectorAdd(backward,middleTest,backTest);

	//trace down from three start points


}
*/

qboolean death_action::Think(ai_c &which_ai, edict_t &monster)
{ //go thru pain anim once, then nothing.
	vec3_t mydist;

	AdjustBBox(which_ai, monster);
//	TiltToFloor(which_ai, monster);
//	AlternateTiltToFloor(which_ai, monster);
	if (monster.groundentity)
	{
		if (which_ai.GetBody()&&which_ai.GetBody()->IsAnimationHeld(HOLDCODE_INAIR))
		{
			which_ai.LandFrame(monster);
			which_ai.GetBody()->ResumeAnimation(monster);
		}
	}
	InterpretFlags(NextFlags(which_ai, monster), which_ai, monster);

	// handle rotational motion tracks
	turninfo_s turninfo;
	SetTurnDestination(which_ai, monster);
	NextTurn(1.0, which_ai, monster, turninfo);
	Turn (which_ai, monster, turninfo);
	NextMovement(which_ai, monster, mydist);

	//special case--if 
	if (which_ai.GetBody() && which_ai.GetBody()->IsAnimationFinished())//animation done--drip
	{
		VectorClear(mydist);
		if (!drip)
		{
			which_ai.GetBody()->Drip(monster);
			drip = true;
		}
	}

	EvaluateMovement(which_ai, monster, mydist);
/*
	if(((level.time - which_ai.GetBody()->GetLastKilledTime()) > .5) && (which_ai.GetBody()->GetMove() == &generic_move_death_dance))
	{
		anim=anim2;
		SetAnimation(which_ai, monster);
	}
*/
	if (which_ai.GetBody())
	{
		anim=which_ai.GetBody()->GetSequenceForDeath(monster, &monster, &monster, 0, monster.s.origin, which_ai.GetBody()->GetLastDFlags()|DAMAGE_NO_PROTECTION);
		if (anim && anim->suggested_action != ACTCODE_DEATH)
		{
			gi.dprintf("Death action %s not really a death!\n", anim->ghoulSeqName);
		}
		which_ai.GetBody()->UpdateFace(monster);
		if (anim)
		{
			SetAnimation(which_ai, monster);
		}
		which_ai.GetBody()->SetLastDFlags(0);
	}
	return false;
}

void death_action::BeginAction(ai_c &which_ai, edict_t &monster)
{
	monster.s.sound = 0;
	monster.s.sound_data = 0;

	if (monster.health > 0)
	{
		monster.health = -1;
		monster.deadflag = DEAD_DEAD;

		monster.solid = SOLID_BBOX;
		monster.clipmask = CONTENTS_DEADMONSTER;
		monster.svflags |= SVF_DEADMONSTER;
		monster.touch = NULL;//do we need this here?
	}

	//if i'm running the thrown back animation, and i'm not being thrown back, fall back to generic thrown
	if (anim==&generic_move_deaththrown && monster.velocity[2]<10)
	{
		vec3_t to_dam;
		AngleVectors(monster.s.angles, to_dam, NULL, NULL);
		VectorScale(to_dam, -200, to_dam);
		VectorAdd(monster.velocity, to_dam, monster.velocity);
		monster.velocity[2]=100;//people have complained about the height of this at times...
		monster.maxs[2]-=26;
		monster.s.origin[2]+=26;
	}
	action_c::BeginAction(which_ai, monster);	
}

death_action::death_action(death_action *orig)
: action_c(orig)
{
	time_of_death = orig->time_of_death;
	drip = orig->drip;
	*(int *)&anim2 = GetMmoveNum(orig->anim2);
}

void death_action::Evaluate(death_action *orig)
{
	time_of_death = orig->time_of_death;
	drip = orig->drip;
	anim2 = GetMmovePtr((int)orig->anim2);

	action_c::Evaluate(orig);
}

void death_action::Read()
{
	char	loaded[sizeof(death_action)];

	gi.ReadFromSavegame('AIDA', loaded, sizeof(death_action));
	Evaluate((death_action *)loaded);
}

void death_action::Write()
{
	death_action	*savable;

	savable = new death_action(this);
	gi.AppendToSavegame('AIDA', savable, sizeof(*this));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/
deathcorpse_action::deathcorpse_action(decision_c *od, action_c *oa, mmove_t *newanim1, mmove_t *newanim2, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
:death_action(od, oa, newanim1, newanim2, monster, inflictor, attacker, damage, point)
{
}

deathcorpse_action::deathcorpse_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
:death_action(od, oa, newanim, monster, inflictor, attacker, damage, point)
{
}

void deathcorpse_action::BeginAction(ai_c &which_ai, edict_t &monster)
{
/*	
	monster.health = -1;
	monster.deadflag = DEAD_DEAD;

	monster.solid = SOLID_BBOX; // has to be BBOX or you can't shoot the body :{
	monster.clipmask = CONTENTS_DEADMONSTER;
	monster.svflags |= SVF_DEADMONSTER;
	monster.touch = NULL;//do we need this here?

	// need to play the last frame of a given death animation
	if (monster.ghoulInst)
	{
		monster.ghoulInst->Play(which_ai.GetBody()->GetSequence(monster, &generic_move_death_fallforward),
								level.time,
								1.5,		// how far into the anim to start. I'm pretty much guessing here.
								true,		// yes, restart
								IGhoulInst::EndCondition::Hold,
								false,		// no, don't match current position
								false		// no, dont' reverse the anim
								);
	}

	anim = &generic_move_death_fallforward;
	//action_c::BeginAction(which_ai, monster);	
*/
}

deathcorpse_action::deathcorpse_action(deathcorpse_action *orig)
:death_action(orig)
{
}

void deathcorpse_action::Evaluate(deathcorpse_action *orig)
{
	time_of_death = orig->time_of_death;
	drip = orig->drip;
	anim2 = GetMmovePtr((int)orig->anim2);

	action_c::Evaluate(orig);
}

void deathcorpse_action::Read()
{
	char	loaded[sizeof(deathcorpse_action)];

	gi.ReadFromSavegame('AIXA', loaded, sizeof(deathcorpse_action));
	Evaluate((deathcorpse_action *)loaded);
}

void deathcorpse_action::Write()
{
	deathcorpse_action	*savable;

	savable = new deathcorpse_action(this);
	gi.AppendToSavegame('AIXA', savable, sizeof(*this));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/
deatharmor_action::deatharmor_action(decision_c *od, action_c *oa, mmove_t *newanim1, mmove_t *newanim2, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
:death_action(od, oa, newanim1, newanim2, monster, inflictor, attacker, damage, point)
{
}

deatharmor_action::deatharmor_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
:death_action(od, oa, newanim, monster, inflictor, attacker, damage, point)
{
}

qboolean deatharmor_action::SafeToRemove(edict_t &monster)
{
	// our target is the item_equip_armor we spawned over this guy's corpse. if we can't
	//find that armor, the player got it so we can ditch this body
	edict_t	*armor = NULL;

	if (monster.targetname)
	{	// find the armor pickup in question
		do
		{
			armor = G_Find (armor, FOFS(target), monster.targetname);
		} while (armor && strcmp(armor->classname, "item_equip_armor"));
	}
	if (armor)
	{	// our armor's still here so don't remove the corpse yet
		return false;
	}
	else
	{	// our armor's gone so treat us like a regular corpse
		return death_action::SafeToRemove(monster);
	}
}

qboolean deatharmor_action::Think(ai_c &which_ai, edict_t &monster)
{
	AdjustBBox(which_ai, monster);
	/*
	// we don't want this joker's gore animating. he's dead, for heaven's sake.
	ggOinstC	*myInstance=game_ghoul.FindOInst(monster.ghoulInst);
	if (myInstance)
	{
		myInstance->StopAnimatingAtAllBolts();
	}
	*/
	//hey! i need a face!
	if (which_ai.GetBody())
	{
		which_ai.GetBody()->UpdateFace(monster);
	}
	return false;
}

void deatharmor_action::BeginAction(ai_c &which_ai, edict_t &monster)
{
	monster.health = -1;
	monster.deadflag = DEAD_DEAD;

	monster.solid = SOLID_BBOX; // has to be BBOX or you can't shoot the body :{
	monster.clipmask = CONTENTS_DEADMONSTER;
	monster.svflags |= SVF_DEADMONSTER;
	monster.touch = NULL;//do we need this here?

	// need to play the last frame of a given death animation
	if (monster.ghoulInst)
	{
		monster.ghoulInst->Play(which_ai.GetBody()->GetSequence(monster, &generic_move_death_fallforward),
								level.time,
								1.5,		// how far into the anim to start. I'm pretty much guessing here.
								true,		// yes, restart
								IGhoulInst::EndCondition::Hold,
								false,		// no, don't match current position
								false		// no, dont' reverse the anim
								);
	}

	if (which_ai.GetBody())
	{
		which_ai.GetBody()->SetVocalCordsWorking(false);
	}

	anim = &generic_move_death_fallforward;
	//action_c::BeginAction(which_ai, monster);	
}

deatharmor_action::deatharmor_action(deatharmor_action *orig)
:death_action(orig)
{
	time_of_death = orig->time_of_death;
	drip = orig->drip;
	*(int *)&anim2 = GetMmoveNum(orig->anim2);
}

void deatharmor_action::Evaluate(deatharmor_action *orig)
{
	time_of_death = orig->time_of_death;
	drip = orig->drip;
	anim2 = GetMmovePtr((int)orig->anim2);

	action_c::Evaluate(orig);
}

void deatharmor_action::Read()
{
	char	loaded[sizeof(deatharmor_action)];

	gi.ReadFromSavegame('AIXA', loaded, sizeof(deatharmor_action));
	Evaluate((deatharmor_action *)loaded);
}

void deatharmor_action::Write()
{
	deatharmor_action	*savable;

	savable = new deatharmor_action(this);
	gi.AppendToSavegame('AIXA', savable, sizeof(*this));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/

int attack_action::NextFlags(ai_c &which_ai, edict_t &monster)
{
	int myFlags=action_c::NextFlags(which_ai, monster);
	if (anim && (anim->actionFlags & ACTFLAG_OPTATTACK) && which_ai.GetBody() && which_ai.GetBody()->RightHandWeaponReady(monster))
	{
		myFlags|=FRAMEFLAG_ATTACK;
	}
	return myFlags;
}

attack_action::attack_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t gohere, vec3_t facing, float timeout, qboolean fullAnimation, bool shouldKill, int theNullTarget)
:move_action(od,oa,newanim, gohere,facing,timeout,fullAnimation)
{

	if (attackTarget)
	{
		if ((shouldKill == true) && (attackTarget->health > 1)) // kill oughta be initialized, but with the current setup I'm not sure of a convenient way to do it
		{
			attackTarget->health = 1;
			// can we get rid of armor here?

		}		
		init_health = attackTarget->health;
	}
	else
	{
		init_health = 1;
	}
	action_target = attackTarget;
	next_attack_time = level.time;
	isLethal = shouldKill;
	NullTarget = theNullTarget;
}

void attack_action::BeginAction(ai_c &which_ai, edict_t &monster)
{
	if (!abortAction)
	{
		if (anim && anim->actionFlags&ACTFLAG_LOOPANIM)
		{
			SetAnimation(which_ai, monster);
		}
		else
		{
			ForceAnimation(which_ai, monster);
		}
		move_action::BeginAction(which_ai,monster);
	}
}

qboolean attack_action::Think(ai_c &which_ai, edict_t &monster)
{
//	vec3_t	v;
	vec3_t	mydist;
	float mypercent;
	bool	bTest1 = false, bTest2 = false, bTest3 = false, bTest4 = false,
			bTest5 = false, bTest6 = false, bTest7 = false, bTest8 = false,
			bTest9 = false;

	if (!PerformBodilyFunctions(which_ai, monster, mydist))
	{
		//let owners know i'm finished
		ResolveAction(which_ai, monster, 0.0, 0.0);
		return true;
	}

	//move, but i don't care if i'm blocked by default
//	gi.dprintf("attack mvmt: %s\n",vtos(mydist));
	if (EvaluateMovement(which_ai, monster, mydist))
	{
		mypercent = 1.0;
	}
	else
	{
		mypercent = 0.0;
	}

	// kef -- and this is _after_ it was cleaned up...
	bTest1 = (!!action_target);
	bTest2 = bTest1?(action_target->health <= 0):false;
	bTest3 = bTest1?(!action_target->inuse):false;
	bTest4 = bTest1?(action_target->health <= 0):false;
	bTest5 = timeout_time < level.time;
	bTest6 = (!!which_ai.GetBody());
	bTest7 = (bTest6?(!!which_ai.GetBody()->IsAnimationFinished()):false);
	bTest8 = (bTest6?(!!which_ai.GetBody()->IsAnimationHeld(HOLDCODE_NO)):false);
	bTest9 = (!waitForAnim || (bTest6 && (bTest7 || bTest8)));
	//is action over? if animation over, or target no longer valid, then it is
	if ( (!isLethal || (bTest1 && bTest2)) &&
		 ((bTest1 && (bTest3 || bTest4)) ||	((bTest5 && bTest9))) )
	{
		float targetremains;

		// kef -- i thought it was a little easier to read if i added this extra
		//condition here rather than a dozen lines earlier. this ensures that anims
		//are finished even if a scripted kill occurs before the end of the anim
		if (bTest1 && bTest4 && !bTest7)
		{
			return false;
		}
		//was action successful? adjust percent_success accordingly
		//fixme: might take a little time to damage player (rocket, etc)
		if (!action_target || !action_target->inuse || action_target->health >= init_health)
		{
//			mypercent = 0.0;
		}
		else
		{
//			mypercent = 1.0;
		}

		//if target is destroyed, signal owner action to abort (hey, is this a good idea?).
		if (!action_target || !action_target->inuse || (action_target->health <=0))
		{
			targetremains=0.0;
		}
		else
		{
			targetremains=1.0;
		}

		//let owners know i'm finished
		ResolveAction(which_ai, monster, mypercent, targetremains);
		return true;
	}
	else
	{
		return false;
	}
}

attack_action::attack_action(attack_action *orig)
: move_action(orig)
{
	init_health = orig->init_health;
	next_attack_time = orig->next_attack_time;
}

void attack_action::Evaluate(attack_action *orig)
{
	init_health = orig->init_health;
	next_attack_time = orig->next_attack_time;

	move_action::Evaluate(orig);
}

/**********************************************************************************
 **********************************************************************************/

void jump_action::JumpToPosition(ai_c &which_ai, edict_t &monster, vec3_t jumpgoal)
{
	vec3_t horiz_vel;
	float dist_to_cover,vert_vel;
	float jump_time, jump_vel;
	float frameboost;
	vec3_t	tJumpGoal;

	//no specific jump goal--just go 64 forward
	if (VectorCompare(monster.s.origin, jumpgoal))
	{
		vec3_t	forward;
		AngleVectors(monster.s.angles, forward, NULL, NULL);
		VectorMA(monster.s.origin, 64, forward, tJumpGoal);
		VectorCopy(tJumpGoal, jumpgoal);
	}
	else
	{
		VectorCopy(jumpgoal, tJumpGoal);
	}
	//separate out direction and distance to goal
	tJumpGoal[2]-=monster.mins[2];
	VectorSubtract(tJumpGoal, monster.s.origin, horiz_vel);
	horiz_vel[2]=0;
	dist_to_cover = VectorNormalize(horiz_vel);

	//cap velocity so i don't jump farther than i'm supposed to
	if (dist_to_cover > which_ai.GetJumpDistance())
	{
		dist_to_cover = which_ai.GetJumpDistance();
	}

	//find out how long i'll be in the air
	if (which_ai.GetJumpHeight() < sv_gravity->value*FRAMETIME)
	{
		frameboost = which_ai.GetJumpHeight();
	}
	else
	{
		frameboost = sv_gravity->value*FRAMETIME;
	}
	//fixme: don't have to jump this high; also, store value as jump velocity
	//time going up
	vert_vel=sqrt(which_ai.GetJumpHeight()*2.0*sv_gravity->value)+frameboost;
	jump_time=vert_vel/(sv_gravity->value*FRAMETIME);
	//time coming down--assuming goal is not higher than jumpheight!!!!!!
	if ((monster.s.origin[2]+which_ai.GetJumpHeight()-tJumpGoal[2])>0)
	{
		if (tJumpGoal[2]>monster.s.origin[2])
		{
			vert_vel=sqrt((tJumpGoal[2]-monster.s.origin[2])*2.0*sv_gravity->value)+frameboost*2;
			jump_time=vert_vel/(sv_gravity->value*FRAMETIME);
		}
		else
		{
			jump_time+=(sqrt((monster.s.origin[2]+which_ai.GetJumpHeight()-tJumpGoal[2])
				*2.0*sv_gravity->value)+frameboost)/(sv_gravity->value*FRAMETIME);
		}
	}

	//now set my jump velocity so i'll reach horizontal dest at right time
	jump_vel =  10*dist_to_cover/jump_time;
	VectorScale(horiz_vel, jump_vel, horiz_vel);
	horiz_vel[2]=vert_vel;

	VectorCopy(horiz_vel, jump_intent);

//	Jump(which_ai, monster, horiz_vel);
}

void jump_action::BeginAction(ai_c &which_ai, edict_t &monster)
{
	JumpToPosition(which_ai, monster, dest);

	//store off horizontal velocity for restoration while i'm in air
	VectorCopy(monster.velocity, jumpvel);
	jumpvel[2]=0;
	which_ai.GetBody()->ResumeAnimation(monster);
	ForceAnimation(which_ai, monster);
}

qboolean jump_action::Think(ai_c &which_ai, edict_t &monster)
{
	vec3_t	mydist;

	//achtung!! could still be in mid-air here, so be careful
	if (!PerformBodilyFunctions(which_ai, monster, mydist))
	{
		ResolveAction(which_ai, monster, 0.0, 0.0);
		return true;
	}

	EvaluateMovement(which_ai, monster, mydist);

	//once i've landed, i'm done
	if (((which_ai.GetBody() && which_ai.GetBody()->IsAnimationFinished()) || (jump_finish_time+3 < level.time))
						&& monster.groundentity)
	{
		VectorClear(monster.velocity);
		//let owners know i'm finished
		VectorSubtract(monster.s.origin,dest,mydist);
		if (VectorLengthSquared(mydist)<900)
		{
			ResolveAction(which_ai, monster, 1.0, 1.0);
		}
		else
		{
			ResolveAction(which_ai, monster, 0.0, 1.0);
		}
		return true;
	}
	else
	{
		if (monster.groundentity || monster.movetype == MOVETYPE_FLY)
		{
			which_ai.GetBody()->ResumeAnimation(monster);
		}

		//adjust velocity to slide up a wall to a step
		//no adjust if i've come to a complete stop--be nice to physics, it zeroes out velocity when it needs to
		//to prevent getting stuck on slopes, only adjust until action times out

		//FIXME: this counts as a hack
		if ((VectorLengthSquared(monster.velocity)>0.01) && (jump_finish_time > level.time) && monster.velocity[2]>0)
		{
			vec3_t jump_nudge;

			jump_nudge[0]=(dest[0]-monster.s.origin[0])/(jump_finish_time - level.time);
			jump_nudge[1]=(dest[1]-monster.s.origin[1])/(jump_finish_time - level.time);
			jump_nudge[2]=0;

			if (VectorLengthSquared(jump_nudge)>16000000)
			{
				VectorNormalize(jump_nudge);
				VectorScale(jump_nudge,4000.0,jump_nudge);
			}
			monster.velocity[0]=jump_nudge[0];
			monster.velocity[1]=jump_nudge[1];
		}
		return false;
	}
}

jump_action::jump_action(jump_action *orig)
: move_action(orig)
{
	VectorCopy(orig->jumpvel, jumpvel);
}

void jump_action::Evaluate(jump_action *orig)
{
	VectorCopy(orig->jumpvel, jumpvel);

	move_action::Evaluate(orig);
}

void jump_action::Read()
{
	char	loaded[sizeof(jump_action)];

	gi.ReadFromSavegame('AIJA', loaded, sizeof(jump_action));
	Evaluate((jump_action *)loaded);
}

void jump_action::Write()
{
	jump_action	*savable;

	savable = new jump_action(this);
	gi.AppendToSavegame('AIJA', savable, sizeof(*this));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/

qboolean fall_action::PerformBodilyFunctions(ai_c &which_ai, edict_t &monster, vec3_t curDist)
{
	turninfo_s turninfo;
	//abortAction will be set when child scripted action fails
	if (abortAction)
	{
		return false;
	}

	which_ai.GetBody()->UpdateFace(monster);
	InterpretFlags(NextFlags(which_ai, monster), which_ai, monster);

	AdjustBBox(which_ai, monster);

	//if i have a turn destination, turn toward it
	SetTurnDestination(which_ai, monster);
	NextTurn(1.0, which_ai, monster, turninfo);
	Turn(which_ai, monster, turninfo);

	NextMovement(which_ai, monster, curDist);
	return true;
}

qboolean fall_action::Think(ai_c &which_ai, edict_t &monster)
{
	vec3_t	mydist,forward;

	//achtung!! could still be in mid-air here, so be careful
	if (!PerformBodilyFunctions(which_ai, monster, mydist))
	{
		ResolveAction(which_ai, monster, 0.0, 0.0);
		return true;
	}

	//urk--this is an ugly way to make sure i'm moving forward.
	if (!started_falling)
	{
		mydist[2]=0;

		if (VectorLengthSquared(mydist)<25 && anim==anim2)
		{
			if (dest && !VectorCompare(dest, vec3_origin))
			{
				VectorSubtract(dest, monster.s.origin, forward);
				forward[2]=0;
				VectorNormalize(forward);
				VectorScale(forward, 20.0, forward);
			}
			else
			{
				AngleVectors(monster.s.angles,forward,NULL,NULL);
				VectorScale(forward,20.0,forward);
			}
			VectorAdd(mydist,forward,mydist);
		}
	}

	//if i haven't started falling yet, and can't move, try to fall
	if (!EvaluateMovement(which_ai, monster, mydist)/*&&!(move_mask&fall_movemask)*/)
	{
		move_mask |= fall_movemask;

		if (EvaluateMovement(which_ai, monster, mydist))
		{
			started_falling = true;
			anim=anim2;
		}
		//gads, this is not good--i'm trapped
		else if ((!(monster.flags & FL_PARTIALGROUND) || monster.velocity[2]<-5)&&(!started_falling && !monster.groundentity))//if (level.time>timeout_time)
		{
			started_falling = true;
//			started_falling = false;
			anim=anim;
		}
	}

	//using timeout for first half of action, while stepmovement keeps me on ground
	if (/*timeout_time < level.time &&*/ !monster.groundentity && !started_falling)
	{
//		started_falling = true;
		anim=anim2;
	}

	//once i've landed, i'm done
	if (monster.groundentity&&started_falling)
	{
		float my_success=1.0;

		which_ai.GetBody()->ResumeAnimation(monster);
		if (which_ai.GetBody() && which_ai.GetBody()->IsAnimationFinished())
		{
			if (monster.s.origin[2]-dest[2]+monster.mins[2]>10)
			{
				my_success=0.0;
			}
			VectorClear(monster.velocity);
			//let owners know i'm finished
			ResolveAction(which_ai, monster, my_success, my_success);
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}

fall_action::fall_action(fall_action *orig)
: move_action(orig)
{
	started_falling = orig->started_falling;
	*(int *)&anim2 = GetMmoveNum(orig->anim2);
}

void fall_action::Evaluate(fall_action *orig)
{
	started_falling = orig->started_falling;
	anim2 = GetMmovePtr((int)orig->anim2);

	move_action::Evaluate(orig);
}

void fall_action::Read()
{
	char	loaded[sizeof(fall_action)];

	gi.ReadFromSavegame('AIFA', loaded, sizeof(fall_action));
	Evaluate((fall_action *)loaded);
}

void fall_action::Write()
{
	fall_action	*savable;

	savable = new fall_action(this);
	gi.AppendToSavegame('AIFA', savable, sizeof(*this));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/

surrender_action::surrender_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *surrenderTo, vec3_t gohere, vec3_t facing, float timeout, qboolean fullAnimation)
:move_action(od,oa,newanim, gohere,facing, timeout,fullAnimation)
{
	action_target = surrenderTo;
}

qboolean surrender_action::Think(ai_c &which_ai, edict_t &monster)
{
//	vec3_t	v;
	vec3_t	mydist;

	if (!PerformBodilyFunctions(which_ai, monster, mydist))
	{
		//let owners know i'm finished
		ResolveAction(which_ai, monster, 0.0, 0.0);
		return true;
	}

	//move, but i don't care if i'm blocked by default
	EvaluateMovement(which_ai, monster, mydist);

	//finish when i timeout
	if ( timeout_time < level.time)
	{
		//let owners know i'm finished--always successful
		ResolveAction(which_ai, monster, 1.0, 1.0);

		return true;
	}
	else
	{
		return false;
	}
}

surrender_action::surrender_action(surrender_action *orig)
: move_action(orig)
{
}

void surrender_action::Evaluate(surrender_action *orig)
{
	move_action::Evaluate(orig);
}

void surrender_action::Read()
{
	char	loaded[sizeof(surrender_action)];

	gi.ReadFromSavegame('AISA', loaded, sizeof(surrender_action));
	Evaluate((surrender_action *)loaded);
}

void surrender_action::Write()
{
	surrender_action	*savable;

	savable = new surrender_action(this);
	gi.AppendToSavegame('AISA', savable, sizeof(*this));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/
capture_action::capture_action(decision_c *od, action_c *oa,mmove_t *newanim,  edict_t *attackTarget, vec3_t gohere, vec3_t facing, float timeout, qboolean fullAnimation)
:move_action(od,oa,newanim,gohere,facing,timeout,fullAnimation)
{
	action_target = attackTarget;
}

qboolean capture_action::Think(ai_c &which_ai, edict_t &monster)
{
//	vec3_t	v;
	vec3_t	mydist;

	if (!PerformBodilyFunctions(which_ai, monster, mydist))
	{
		//let owners know i'm finished
		ResolveAction(which_ai, monster, 0.0, 0.0);

		return true;
	}

	//move, but i don't care if i'm blocked by default
	EvaluateMovement(which_ai, monster, mydist);

	//finish when i timeout
	if ( timeout_time < level.time)
	{
		//let owners know i'm finished--always successful??? shouldn't be.
		ResolveAction(which_ai, monster, 1.0, 1.0);

		return true;
	}
	else
	{
		return false;
	}
}

capture_action::capture_action(capture_action *orig)
: move_action(orig)
{
}

void capture_action::Evaluate(capture_action *orig)
{
	move_action::Evaluate(orig);
}

void capture_action::Read()
{
	char	loaded[sizeof(capture_action)];

	gi.ReadFromSavegame('AICA', loaded, sizeof(capture_action));
	Evaluate((capture_action *)loaded);
}

void capture_action::Write()
{
	capture_action	*savable;

	savable = new capture_action(this);
	gi.AppendToSavegame('AICA', savable, sizeof(*this));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/
endscript_action::endscript_action(decision_c *od) : action_c ()
{
	od->FinishScript();
	timeout_interval = 0;
}

qboolean endscript_action::Think(ai_c &which_ai, edict_t &monster)
{
	//add sight & sound if i spawned deaf & blind
	if (monster.spawnflags&SPAWNFLAG_BLIND)
	{
		which_ai.NewSense(new normalsight_sense(), &monster);
		monster.spawnflags&=(~SPAWNFLAG_BLIND);
	}
	if (monster.spawnflags&SPAWNFLAG_DEAF)
	{
		which_ai.NewSense(new sound_sense(), &monster);
		monster.spawnflags&=(~SPAWNFLAG_DEAF);
	}
	ResolveAction(which_ai, monster, 1.0, 1.0);
	return true;
}

endscript_action::endscript_action(endscript_action *orig)
: action_c(orig)
{
}

void endscript_action::Evaluate(endscript_action *orig)
{
	action_c::Evaluate(orig);
}

void endscript_action::Read()
{
	char	loaded[sizeof(endscript_action)];

	gi.ReadFromSavegame('AIEA', loaded, sizeof(endscript_action));
	Evaluate((endscript_action *)loaded);
}

void endscript_action::Write()
{
	endscript_action	*savable;

	savable = new endscript_action(this);
	gi.AppendToSavegame('AIEA', savable, sizeof(*this));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/
running_melee_attack_action::running_melee_attack_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t gohere, vec3_t facing, float timeout, qboolean fullAnimation)
:attack_action(od, oa, newanim, attackTarget, gohere, facing, timeout, fullAnimation)
{
	m_nActionCode = MELEE_ACTION_NONE;
}

qboolean running_melee_attack_action::Turn(ai_c &which_ai, edict_t &monster, turninfo_s &turninfo)
{
	float	ideal;
	float	current;
	float	move;
	
	current = anglemod(monster.s.angles[YAW]);
	ideal = anglemod(which_ai.ideal_angles[YAW]);

	//already facing ideal direction; consider turn successful
	if (current == ideal)
	{
		Aim(which_ai, monster);
		return true;
	}

	move = ideal - current;

	//make sure we take shortest route to desired yaw
	if (ideal > current)
	{
		if (move >= 180)
		{
			move = move - 360;
		}
	}
	else
	{
		if (move <= -180)
		{
			move = move + 360;
		}
	}

	//cap turning speed to yawspeed
	if (move > 0)
	{
		if (move > turninfo.max_turn)
		{
			move = turninfo.max_turn;
		}
	}
	else
	{
		if (move < turninfo.min_turn)
		{
			move = turninfo.min_turn;
		}
	}
	
	monster.s.angles[YAW] = anglemod (current + move);

	Aim(which_ai, monster);

	return true;
}

void running_melee_attack_action::SetTurnDestination(ai_c &which_ai, edict_t &monster)
{
	vec3_t v,temp_v;
	sensedEntInfo_t sensed_client, sensed_monster;

	 which_ai.GetSensedClientInfo(smask_all, sensed_client);
	 which_ai.GetSensedMonsterInfo(smask_all, sensed_monster);

	// once we jump we don't want to change our direction unless
	//a) we hit someone
	//b) we land
	if (m_nActionCode == MELEE_ACTION_JUMPED)
	{
		// we just jumped so don't let anyone change our facing
	}
	else if (m_nActionCode == MELEE_ACTION_HIT)
	{
		// we hit someone during our attack. we changed our facing
		//in running_melee_attack_action::InterpretFlags at the time
		//of impact, so now just keep going straight through the end
		//of this animation
	}
	else
	{
		// we aren't jumping yet and we haven't hit anyone yet so
		//face wherever the heck

		//keep in touch with the current position of my target, for greater accuracy
		if (action_target && sensed_monster.ent == action_target)
		{
			VectorCopy(sensed_monster.pos, face);
			VectorSubtract (face, monster.s.origin, v);

			v[2]-=monster.viewheight;

			VectorAdd(sensed_monster.ent->mins,sensed_monster.ent->maxs,temp_v);
			VectorScale(temp_v,0.5,temp_v);
			VectorAdd(v,temp_v,v);
		}
		else if (action_target && sensed_client.ent == action_target)
		{
			VectorCopy(sensed_client.pos, face);
			VectorSubtract (face, monster.s.origin, v);

			v[2]-=monster.viewheight;

			VectorAdd(sensed_client.ent->mins,sensed_client.ent->maxs,temp_v);
			VectorScale(temp_v,0.5,temp_v);
			VectorAdd(v,temp_v,v);
		}
		//if i have a turn destination, turn toward it--DANGER!!--this will not work if you really want to be facing 0,0,0
		else if (face[0]||face[1]||face[2])
		{
			VectorSubtract (face, monster.s.origin, v);
		}
		//if no turn destination specified, turn toward movement destination
		else
		{
			VectorSubtract (dest, monster.s.origin, v);
		}
		vectoangles(v, which_ai.ideal_angles);
	}
}

void running_melee_attack_action::InterpretFlags(int cur_flags, ai_c &which_ai, edict_t &monster)
{
	//if frameflags specify jump & this action is primed for jumping, record
	//our desired jump direction so it doesn't get changed until we land
	if (cur_flags & FRAMEFLAG_JUMP)
	{
		if (move_mask & jump_movemask)
		{
			//Jump(which_ai, monster, jump_intent);
			m_nActionCode = MELEE_ACTION_JUMPED;
		}
	}

	//this ok? no keeping track of whether i've played this sound recently?
	if (cur_flags & FRAMEFLAG_LAND)
	{
//		if (monster.groundentity)
		{
//			gi.dprintf("<thud!>\n");
			switch(rand()%3)
			{
			default:
			case 0:
				gi.sound (&monster, CHAN_BODY, gi.soundindex ("impact/gore/fall1.wav"), 1.0, ATTN_NORM, 0);
				break;
			case 1:
				gi.sound (&monster, CHAN_BODY, gi.soundindex ("impact/gore/fall2.wav"), 1.0, ATTN_NORM, 0);
				break;
			case 2:
				gi.sound (&monster, CHAN_BODY, gi.soundindex ("impact/gore/fall3.wav"), 1.0, ATTN_NORM, 0);
				break;
			}
		}
	}

	//play footsteps
	if (cur_flags & FRAMEFLAG_FOOTSTEP)
	{
		FX_SetEvent(&monster, EV_FOOTSTEPLEFT);
	}

	if (cur_flags & FRAMEFLAG_ATTACK)
	{
		// need to call the actual attack function
		body_c* body = which_ai.GetBody();
		if (body)
		{
			body->FirePrimaryWeapon(monster, isLethal, NullTarget);
			isLethal = false;
		}
	}

	if (cur_flags & FRAMEFLAG_THROW)
	{
		// need to call the actual attack function
		body_c* body = which_ai.GetBody();
		if (body)
		{
			body->ThrowProjectile(monster);
		}
		cur_flags &= ~FRAMEFLAG_THROW;
	}


	if (cur_flags & FRAMEFLAG_MELEE)
	{
		// this flag means we hit someone with our attack so we need
		//to change the direction of our motion
		m_nActionCode = MELEE_ACTION_HIT;
		if (rand()%2)
		{
			which_ai.ideal_angles[YAW] -= 20;
			if (which_ai.ideal_angles[YAW] < 0)
			{
				which_ai.ideal_angles[YAW] += 360;
			}
		}
		else
		{
			which_ai.ideal_angles[YAW] += 20;
			if (which_ai.ideal_angles[YAW] > 360)
			{
				which_ai.ideal_angles[YAW] -= 360;
			}
		}
	}

	//frame holding--holds for random period of time, vey primitive.
	if (cur_flags & FRAMEFLAG_HOLDFRAME)
	{
	}
}

running_melee_attack_action::running_melee_attack_action(running_melee_attack_action *orig)
: attack_action(orig)
{
	m_nActionCode = orig->m_nActionCode;
}

void running_melee_attack_action::Evaluate(running_melee_attack_action *orig)
{
	m_nActionCode = orig->m_nActionCode;

	attack_action::Evaluate(orig);
}

void running_melee_attack_action::Read()
{
	char	loaded[sizeof(running_melee_attack_action)];

	gi.ReadFromSavegame('AIRA', loaded, sizeof(running_melee_attack_action));
	Evaluate((running_melee_attack_action *)loaded);
}

void running_melee_attack_action::Write()
{
	running_melee_attack_action	*savable;

	savable = new running_melee_attack_action(this);
	gi.AppendToSavegame('AIRA', savable, sizeof(*this));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/
dog_walk_action::dog_walk_action(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing, float timeout, qboolean fullAnimation)
:move_action(od,oa,newanim, destination, facing, timeout,fullAnimation)
{
	m_MoveStraightAnim = newanim;
	m_MoveLeftAnim = newanim;
	m_MoveRightAnim = newanim;
}

qboolean dog_walk_action::Turn(ai_c &which_ai, edict_t &monster, turninfo_s &turninfo)
{
	float	ideal;
	float	current;
	float	move;
	
	anim = m_MoveStraightAnim;
	current = anglemod(monster.s.angles[YAW]);
	ideal = anglemod(which_ai.ideal_angles[YAW]);

	//already facing ideal direction; consider turn successful
	if (current == ideal)
	{
		Aim(which_ai, monster);
		return true;
	}

	move = ideal - current;

	// if we're turning to the left, use our 'walking left' anim, else 'walking right'
	if (move < 0)
	{
		anim = m_MoveLeftAnim;
	}
	else
	{
		anim = m_MoveRightAnim;
	}

	if (anim != m_MoveStraightAnim)
	{
		BeginAction(which_ai, monster);
	}

	//make sure we take shortest route to desired yaw
	if (ideal > current)
	{
		if (move >= 180)
		{
			move = move - 360;
		}
	}
	else
	{
		if (move <= -180)
		{
			move = move + 360;
		}
	}

	//cap turning speed to yawspeed
	if (move > 0)
	{
		if (move > turninfo.max_turn)
		{
			move = turninfo.max_turn;
		}
	}
	else
	{
		if (move < turninfo.min_turn)
		{
			move = turninfo.min_turn;
		}
	}
	
	monster.s.angles[YAW] = anglemod (current + move);

	Aim(which_ai, monster);

	return true;
}

dog_walk_action::dog_walk_action(dog_walk_action *orig)
: move_action(orig)
{
	*(int *)&m_MoveStraightAnim = GetMmoveNum(orig->m_MoveStraightAnim);
	*(int *)&m_MoveLeftAnim = GetMmoveNum(orig->m_MoveLeftAnim);
	*(int *)&m_MoveRightAnim = GetMmoveNum(orig->m_MoveRightAnim);
}

void dog_walk_action::Evaluate(dog_walk_action *orig)
{
	m_MoveStraightAnim = GetMmovePtr((int)orig->m_MoveStraightAnim);
	m_MoveLeftAnim = GetMmovePtr((int)orig->m_MoveLeftAnim);
	m_MoveRightAnim = GetMmovePtr((int)orig->m_MoveRightAnim);

	move_action::Evaluate(orig);
}

void dog_walk_action::Read()
{
	char	loaded[sizeof(dog_walk_action)];

	gi.ReadFromSavegame('AIDW', loaded, sizeof(dog_walk_action));
	Evaluate((dog_walk_action *)loaded);
}

void dog_walk_action::Write()
{
	dog_walk_action	*savable;

	savable = new dog_walk_action(this);
	gi.AppendToSavegame('AIDW', savable, sizeof(*this));
	delete savable;
}

// -------------------------------------------------------------

stand_action::stand_action(stand_action *orig)
: action_c(orig)
{
}

void stand_action::Evaluate(stand_action *orig)
{
	action_c::Evaluate(orig);
}

void stand_action::Read()
{
	char	loaded[sizeof(stand_action)];

	gi.ReadFromSavegame('AISA', loaded, sizeof(stand_action));
	Evaluate((stand_action *)loaded);
}

void stand_action::Write()
{
	stand_action	*savable;

	savable = new stand_action(this);
	gi.AppendToSavegame('AISA', savable, sizeof(*this));
	delete savable;
}

// -------------------------------------------------------------

walk_action::walk_action(walk_action *orig)
: move_action(orig)
{
}

void walk_action::Evaluate(walk_action *orig)
{
	move_action::Evaluate(orig);
}

void walk_action::Read()
{
	char	loaded[sizeof(walk_action)];

	gi.ReadFromSavegame('AIWA', loaded, sizeof(walk_action));
	Evaluate((walk_action *)loaded);
}

void walk_action::Write()
{
	walk_action	*savable;

	savable = new walk_action(this);
	gi.AppendToSavegame('AIWA', savable, sizeof(*this));
	delete savable;
}

// -------------------------------------------------------------

shoot_attack_action::shoot_attack_action(shoot_attack_action *orig)
: attack_action(orig)
{
}

void shoot_attack_action::Evaluate(shoot_attack_action *orig)
{
	attack_action::Evaluate(orig);
}

void shoot_attack_action::Read()
{
	char	loaded[sizeof(shoot_attack_action)];

	gi.ReadFromSavegame('AIAA', loaded, sizeof(shoot_attack_action));
	Evaluate((shoot_attack_action *)loaded);
}

void shoot_attack_action::Write()
{
	shoot_attack_action	*savable;

	savable = new shoot_attack_action(this);
	gi.AppendToSavegame('AIAA', savable, sizeof(*this));
	delete savable;
}

// end
