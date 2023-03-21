// m_heliactions.cpp

#include "g_local.h"
#include "ai_private.h"
#include "m_heliai.h"
//#include "m_heli.h"
 
//#include <windows.h>	// for OutputDebugString() only, do not leave in!


/**********************************************************************************
 **********************************************************************************/
heli_action::heli_action(decision_c *od, action_c *oa, mmove_t *newanim,
						 int nCommand, vec3_t vPos, edict_t* target, float fArg)
:action_c(od, oa, newanim, vPos, vPos, NULL, 999999, true)
{
	m_nCommand = nCommand;
	action_target = target;
	m_fArg = fArg;
	m_think = NULL;
	m_nextthink = NULL;
	m_nID = -1;
}

qboolean heli_action::Think(ai_c &which_ai, edict_t &monster)
{
	float fHealthPercentage = 1.0f;
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)&which_ai;
	body_heli* body = NULL;

	if (ai)
	{
		body = ai->GetHeliBody();
	}
	if (!body)
	{
		return false;
	}
	qboolean bRet = true;
	if (m_think)
	{
		// set our script params in the body's member variables
		body->m_fArg = m_fArg;
		VectorCopy(dest, body->m_vPos);
		body->m_target = action_target;

		// this past think may have updated our next think
		if (m_nextthink && m_nextthink != m_think)
		{
			m_think = m_nextthink;
			m_nextthink = NULL;
		}

		m_think(&monster);

		fHealthPercentage = (float)monster.health/(float)monster.max_health;
		body->UpdateSmoke(&monster, fHealthPercentage);

		// kef -- dunno if I like this. probly a cleaner way...
		bRet = ((generic_ghoul_heli_ai*)&which_ai)->IsActionDone();
	}

	// check our need for creating a pseudo-action if our current action 
	//completed or if we just reached zer0 health
	if (bRet ||
		((body->m_flags & HELIFLAG_ISDEAD) && !(body->m_flags & HELIFLAG_ISDEADACKNOWLEDGED)))
	{
		// going to emulate Ste's CobraThink_Think here so we can "insert"
		//pseudo-actions into the action queue

		// if it's on the pad, explode
		if (((body->m_flags & HELIFLAG_ISDEAD) && !(body->m_flags & HELIFLAG_ISDEADACKNOWLEDGED)) &&
			fabs(monster.s.origin[0]) - fabs(body->m_v3HeliPadRestPos[0]) < 0.1f &&
			fabs(monster.s.origin[1]) - fabs(body->m_v3HeliPadRestPos[1]) < 0.1f &&
			fabs(monster.s.origin[2]) - fabs(body->m_v3HeliPadRestPos[2]) < 0.1f )
		{
			ai->TimeToDie(true);
			ResolveAction(which_ai, monster, 1.0, 1.0);
		}
		else if(InsertPseudoAction(body, &monster))
		{
			// inserted a pseudo-action, so we want the heli ai to believe
			//we're still executing the same action
			bRet = false;
		}
		else
		{
			ResolveAction(which_ai, monster, 1.0, 1.0);
		}

		// just finished an action so reset our variable
		((generic_ghoul_heli_ai*)&which_ai)->ActionIsDone(false);
	}
	return bRet;
}


// overriding this fn is a quick way of keeping action_c from screwing with
//our heli's sequence
qboolean heli_action::SetAnimation(ai_c &which_ai, edict_t &monster)
{
	//just bein safe
	if (!which_ai.GetBody())
	{
		gi.dprintf("heli_action::SetAnimation--ai has no body!\n");
		return false;
	}
	return true;
}

// overriding this fn is a quick way of keeping action_c from screwing with
//our heli's sequence
qboolean heli_action::ForceAnimation(ai_c &which_ai, edict_t &monster)
{
	//just bein safe
	if (!which_ai.GetBody())
	{
		gi.dprintf("heli_action::ForceAnimation--ai has no body!\n");
		return false;
	}
	return true;
}

qboolean heli_action::InsertPseudoAction(body_heli* body, edict_t* self)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)self->ai;

	// if we've been given the script command to turn ai on, check here
	//to see if we should turn it off and start executing more script cmds

	body->m_flags &= ~HELIFLAG_SCRIPTEDAIJUSTFINISHED;	// regardless

	if (body->m_flags & HELIFLAG_DOINGSCRIPTEDAI &&
		((ai && level.time > body->m_fHeliAITimer && ai->GetActionSize()) ||
		 (body->m_flags & HELIFLAG_ISDEAD))  )
	{
		body->m_flags &= ~(	HELIFLAG_DOINGSCRIPTEDAI |
							HELIFLAG_DOINGSMARTGOTO |
							HELIFLAG_DOINGSCRIPTEDFIREAT);
		body->m_flags |= HELIFLAG_SCRIPTEDAIJUSTFINISHED;	// so hover code can pick it up
	}

	if ((body->m_flags & HELIFLAG_ISDEAD) && !(body->m_flags & HELIFLAG_ISDEADACKNOWLEDGED))
	{
		body->m_flags |= HELIFLAG_ISDEADACKNOWLEDGED;
		if (self->spawnflags & ONDIE_HEAD2SPAWN)
		{
			Cobra2_LegaliseVector(self, body->m_v3HeliDeathDest, body->m_v3HeliDeathDest, true);	// qboolean qbUseWorldClip
			VectorCopy(body->m_v3HeliDeathDest, dest);
			m_think = Cobra2GotoCoordsHelper;
		}
		else
		{
			// store the current forward vector in body->m_v3HeliDeathDest
			vec3_t	vF, vR, vU;

			AngleVectors(self->s.angles, vF, vR, vU);
			VectorNegate(vR, body->m_v3HeliDeathDest);
			body->m_v3HeliDeathDest[2] = 0;
			body->m_fHeliHoverTimer = level.time + 2.0f;
			m_think = m_nextthink = Cobra2_OutOfControl;
		}
		return true;
	}

	if (body->m_flags & HELIFLAG_DOINGSMARTGOTO)	// this one MUST be before the 'HELIFLAG_DOINGSCRIPTEDLANDING' check
	{
		m_think = Cobra2Think_SmartHeadToDest;
		return true;
	}

	if (body->m_flags & HELIFLAG_DOINGSCRIPTEDLANDING)
	{
		m_nextthink = m_think = Cobra2Think_Land;
		body->m_flags &= ~HELIFLAG_DOINGSCRIPTEDLANDING;
		return true;
	}

	if (body->m_flags & HELIFLAG_DOINGSCRIPTEDFIREAT)
	{
		m_think = Cobra2Think_GotoFireAt;
		return true;
	}

	if (body->m_flags & HELIFLAG_DOINGSCRIPTEDHOVER)
	{
		m_think = Cobra2Think_Hover;
		return true;
	}

	if (body->m_flags & HELIFLAG_DOINGSCRIPTEDAI)
	{
		Cobra2Think_AI(self);
		return true;
	}

	return false;
}

heli_action::heli_action(heli_action *orig)
: action_c(orig)
{
	m_nID = orig->m_nID;    
	m_nCommand = orig->m_nCommand;
	m_fArg = orig->m_fArg;   

	*(int *)&m_think = GetThinkNum(orig->m_think);
	*(int *)&m_nextthink = GetThinkNum(orig->m_nextthink);
}

void heli_action::Evaluate(heli_action *orig)
{
	m_nID = orig->m_nID;    
	m_nCommand = orig->m_nCommand;
	m_fArg = orig->m_fArg;   

	m_think = (void (*)(edict_t *self))GetThinkPtr((int)orig->m_think);
	m_nextthink = (void (*)(edict_t *self))GetThinkPtr((int)orig->m_nextthink);

	action_c::Evaluate(orig);
}

void heli_action::Read()
{
	char	loaded[sizeof(heli_action)];;

	gi.ReadFromSavegame('AIHE', loaded, sizeof(heli_action));
	Evaluate((heli_action *)loaded);
}

void heli_action::Write()
{
	heli_action	*savable;

	savable = new heli_action(this);
	gi.AppendToSavegame('AIHE', savable, sizeof(*this));
	delete savable;
}

/**********************************************************************************
 **********************************************************************************/
helimove_action::helimove_action(decision_c *od, action_c *oa, ai_c* which_ai, 
								 mmove_t *newanim, int nCommand, vec3_t vPos, 
								 edict_t* target, float fArg, int nID)
:heli_action(od, oa, newanim, nCommand, vPos, target, fArg)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)which_ai;
	body_heli* body = NULL;

	if (ai)
	{
		body = ai->GetHeliBody();
	}
	if (!body)
	{
		return;
	}
	// save our id (uniquely determined by the ai)
	m_nID = nID;
	// save our script params
	m_fArg = fArg;
	VectorCopy(vPos, dest);
	action_target = target;

	switch(m_nCommand)
	{
	case generic_ghoul_heli_ai::hse_TAKEOFF:
		{
			m_think = Cobra2Think_TakeOff;
			break;
		}
	case generic_ghoul_heli_ai::hse_LAND:
		{
			VectorCopy(body->m_v3HeliPadRestPos,dest);
			m_think = Cobra2LandHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_GOTO_COORDS:
		{
			m_think = Cobra2GotoCoordsHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_GOTOREL_ENTITY:
		{
			if (target)
			{
				body->m_target = target;
				VectorCopy(vPos, body->m_vPos);
				m_think = Cobra2GotoRelEntHelper;
			}
			break;
		}
	case generic_ghoul_heli_ai::hse_GOTOREL_ENT_X:
		{
			if (target)
			{
				body->m_target = target;
				VectorSet(dest, fArg, 0, 0);
				m_think = Cobra2GotoRelEntHelper;
			}
			break;
		}
	case generic_ghoul_heli_ai::hse_GOTOREL_ENT_Y:
		{
			if (target)
			{
				body->m_target = target;
				VectorSet(dest, 0, fArg, 0);
				m_think = Cobra2GotoRelEntHelper;
			}
			break;
		}
	case generic_ghoul_heli_ai::hse_GOTOREL_ENT_Z:
		{
			if (target)
			{
				body->m_target = target;
				VectorSet(dest, 0, 0, fArg);
				m_think = Cobra2GotoRelEntHelper;
			}
			break;
		}
	case generic_ghoul_heli_ai::hse_MOVEREL:
		{
			VectorCopy(vPos, body->m_vPos);
			m_think = Cobra2GotoRelHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_STRAFE_RT:			
		{
			m_fArg = fabs(fArg);
			m_think = Cobra2StrafeHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_STRAFE_LT:
		{
			m_fArg = -fabs(fArg);
			m_think = Cobra2StrafeHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_HOVER_PASSIVE:
		{
			m_fArg = fArg;
			dest[0] = 0;
			m_think = Cobra2HoverHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_HOVER_AGGRESSIVE:
		{
			body->m_fArg = fArg;
			dest[0] = 1;
			m_think = Cobra2HoverHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_REPAIR:
		{
			m_think = Cobra2Think_Repair;
			break;
		}
	case generic_ghoul_heli_ai::hse_REARM:
		{
			m_think = Cobra2RearmHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_SET_WORLDMINS:
		{
			m_fArg = body->m_fArg = 0; // 0 == mins
			VectorCopy(vPos, body->m_vPos);
			m_think = Cobra2WorldHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_SET_WORLDMAXS:
		{
			m_fArg = body->m_fArg = 1; // 1 == maxs
			VectorCopy(vPos, body->m_vPos);
			m_think = Cobra2WorldHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_SET_DEATHDEST:
		{
			m_think = Cobra2DeathDestHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_SET_TRACEDIMS:
		{
			m_think = Cobra2TraceDimsHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_SET_HEALTH:
		{
			dest[0] = 0; // 0 == health
			m_think = Cobra2HealthHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_SET_MAXHEALTH:
		{
			dest[0] = 1;	// 1 == maxhealth
			m_think = Cobra2HealthHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_CHANGE_SKIN:
		{
			// set the skin
			if (1 == m_fArg)
			{
				SetSkin(target, "enemy/chopper", "cobrass", "cobra","m_x_chopper_black", true);
			}
			else if (2 == m_fArg)
			{
				SetSkin(target, "enemy/chopper", "cobrass", "cobra","m_x_chopper_green", true);
			}
			else
			{
				SetSkin(target, "enemy/chopper", "cobrass", "cobra","m_x_chopper_white", true);
			}
			break;
		}
	case generic_ghoul_heli_ai::hse_CHANGE_BODY:
		{
			// set the body
			if (1 == m_fArg)
			{
				ai->ChangeBody(target, "enemy/hind", "hind");
			}
			break;
		}
	case generic_ghoul_heli_ai::hse_OUT_OF_CONTROL:
		{
			// body->m_v3HeliDeathDest contains a base velocity for crashing
			VectorClear(body->m_v3HeliDeathDest);
			body->m_fHeliHoverTimer = level.time + 2.0f;
			m_think = Cobra2_OutOfControl;
			break;
		}
	case generic_ghoul_heli_ai::hse_AI:
		{
			m_think = Cobra2AIHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_WAYPOINT:
		{
			ai->AddWaypoint(vPos);
			break;
		}
	case generic_ghoul_heli_ai::hse_VOLUME:
		{
			ai->SetWaypointPtVolume(ai->GetNumWaypointPts()-1,vPos);
			break;
		}
	case generic_ghoul_heli_ai::hse_VOLUMEMINS:
		{
			// vPos is in world coords, change to rel-to-waypt-origin
			heliWaypoint* pt = ai->GetWaypointPt(ai->GetNumWaypointPts()-1);
			if (pt)
			{
				VectorCopy(vPos, pt->m_volMins);
			}
			break;
		}
	case generic_ghoul_heli_ai::hse_VOLUMEMAXS:
		{
			// vPos is in world coords, change to rel-to-waypt-origin
			vec3_t vTemp;
			heliWaypoint* pt = ai->GetWaypointPt(ai->GetNumWaypointPts()-1);
			if (pt)
			{
				VectorCopy(vPos, pt->m_volMaxs);
				VectorAdd(pt->m_volMins, pt->m_volMins, vTemp);
				VectorScale(vTemp, 0.5f, pt->m_v);
				VectorSubtract(pt->m_volMins, pt->m_v, pt->m_volMins);
				VectorSubtract(pt->m_volMaxs, pt->m_v, pt->m_volMaxs);
				ai->SetWaypointPtVolume(ai->GetNumWaypointPts()-1,pt->m_volMaxs);
			}
			break;
		}
	case generic_ghoul_heli_ai::hse_DEBUG:
		{
			// sets the proper spawnflag in ds.cpp, nothing to do here
			break;
		}
	default:
		break;
	}
}

/**********************************************************************************
 **********************************************************************************/
heliface_action::heliface_action(decision_c *od, action_c *oa, ai_c* which_ai, 
								 mmove_t *newanim, int nCommand, vec3_t vPos, 
								 edict_t* target, float fArg, int nID)
:heli_action(od, oa, newanim, nCommand, vPos, target, fArg)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)which_ai;
	body_heli* body = NULL;

	if (ai)
	{
		body = ai->GetHeliBody();
	}
	if (!body)
	{
		return;
	}
	// save our id (uniquely determined by the ai)
	m_nID = nID;
	// save our script params
	m_fArg = fArg;
	VectorCopy(vPos, dest);
	action_target = target;

	switch(m_nCommand)
	{
	case generic_ghoul_heli_ai::hse_FACE_RELENT:
		{
			if (target)
			{
				body->m_target = target;
				VectorCopy(vPos,  body->m_vPos);
				m_think = Cobra2FaceRelEntHelper;
			}
			break;
		}
	case generic_ghoul_heli_ai::hse_FACE_ABSCOORDS:
		{
			VectorCopy(vPos,  body->m_vPos);
			m_think = Cobra2FaceAbsHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_FACE_ABSDIR:
		{
			VectorCopy(vPos,  body->m_vPos);
			m_think = Cobra2FaceAbsDirHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_FACE_RELCOORDS:
		{
			VectorCopy(vPos, body->m_vPos);
			m_think = Cobra2FaceRelCoordsHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_PILOT_FACERELENT:
	case generic_ghoul_heli_ai::hse_GUNNER_FACERELENT:
		{
			if (target)
			{
				body->m_target = target;
				VectorCopy(vPos, body->m_vPos);
				if (m_nCommand == generic_ghoul_heli_ai::hse_GUNNER_FACERELENT)
				{
					m_fArg = 0; // 0 == gunner
				}
				else
				{
					m_fArg = 1; // 1 == pilot
				}
				m_think = Cobra2HeadFaceRelEntHelper;
			}
			break;
		}
	case generic_ghoul_heli_ai::hse_PILOT_FACERELCOORDS:
	case generic_ghoul_heli_ai::hse_GUNNER_FACERELCOORDS:
		{
			VectorCopy(vPos, body->m_vPos);
			if (m_nCommand == generic_ghoul_heli_ai::hse_GUNNER_FACERELCOORDS)
			{
				m_fArg = 0; // 0 == gunner
			}
			else
			{
				m_fArg = 1; // 1 == pilot
			}
			m_think = Cobra2HeadFaceRelCoordsHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_PILOT_FACEABSCOORDS:
	case generic_ghoul_heli_ai::hse_GUNNER_FACEABSCOORDS:
		{
			VectorCopy(vPos, body->m_vPos);
			if (m_nCommand == generic_ghoul_heli_ai::hse_GUNNER_FACEABSCOORDS)
			{
				m_fArg = 0; // 0 == gunner
			}
			else
			{
				m_fArg = 1; // 1 == pilot
			}
			m_think = Cobra2HeadFaceAbsCoordsHelper;
			break;
		}
	default:
		break;
	}
}

/**********************************************************************************
 **********************************************************************************/
heliattack_action::heliattack_action(decision_c *od, action_c *oa, ai_c* which_ai, 
								 mmove_t *newanim, int nCommand, vec3_t vPos, 
								 edict_t* target, float fArg, int nID)
:heli_action(od, oa, newanim, nCommand, vPos, target, fArg)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)which_ai;
	body_heli* body = NULL;

	if (ai)
	{
		body = ai->GetHeliBody();
	}
	if (!body)
	{
		return;
	}
	// save our id (uniquely determined by the ai)
	m_nID = nID;
	// save our script params
	m_fArg = fArg;
	VectorCopy(vPos, dest);
	action_target = target;

	switch(m_nCommand)
	{
	case generic_ghoul_heli_ai::hse_ROCKETS_ENABLE:
		{
			m_fArg = 1;
			m_think = Cobra2RocketsEnableHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_ROCKETS_DISABLE:
		{
			m_fArg = 0;
			m_think = Cobra2RocketsEnableHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_CHAINGUN_ENABLE:
		{
			m_fArg = 1;
			m_think = Cobra2ChainGunEnableHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_CHAINGUN_DISABLE:
		{
			m_fArg = 0;
			m_think = Cobra2ChainGunEnableHelper;
			Cobra2_Fire_OFF(body->GetHeliEdict());
			body->m_fArg = 0;
			ai->ActionIsDone(true);
			break;
		}
	case generic_ghoul_heli_ai::hse_AUTOFIRE_ON:
		{
			m_fArg = 1;
			m_think = Cobra2AutoFireHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_AUTOFIRE_OFF:
		{
			m_fArg = 0;
			m_think = Cobra2AutoFireHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_FIREAT_RELENT:
		{
			if (target)
			{
				body->m_target = target;
				VectorCopy(vPos,  body->m_vPos);
				body->m_fArg = fArg;
				m_think = Cobra2FireAtRelEntHelper;
			}
			break;
		}
	case generic_ghoul_heli_ai::hse_FIREAT_ABSCOORDS:
		{
			VectorCopy(vPos,  body->m_vPos);
			body->m_fArg = fArg;
			m_think = Cobra2FireAtAbsHelper;
			break;
		}
	case generic_ghoul_heli_ai::hse_FIREAT_RELCOORDS:
		{
			VectorCopy(vPos,  body->m_vPos);
			body->m_fArg = fArg;
			m_think = Cobra2FireAtRelHelper;
			break;
		}
	default:
		break;
	}
}



/**********************************************************************************
 **********************************************************************************/
helideath_action::helideath_action(decision_c *od, action_c *oa, ai_c* which_ai, 
								 mmove_t *newanim, int nCommand, vec3_t vPos, 
								 edict_t* target, float fArg, int nID)
:heli_action(od, oa, newanim, nCommand, vPos, target, fArg)
{
	generic_ghoul_heli_ai* ai = (generic_ghoul_heli_ai*)(ai_public_c*)which_ai;
	body_heli* body = NULL;

	if (ai)
	{
		body = ai->GetHeliBody();
	}
	if (!body)
	{
		return;
	}

	m_think = Cobra2GotoCoordsHelper;

	// save our id (uniquely determined by the ai)
	m_nID = nID;
	// save our script params
	m_fArg = fArg;
	VectorCopy(vPos, dest);
	action_target = target;

}

