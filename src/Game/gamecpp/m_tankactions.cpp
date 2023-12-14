// m_tankactions.cpp

#include "g_local.h"
#include "ai_private.h"
#include "m_tankai.h"
 
//#include <windows.h>	// for OutputDebugString() only, do not leave in!


/**********************************************************************************
 **********************************************************************************/
tank_action::tank_action(decision_c *od, action_c *oa, ai_c* which_ai, 
								 mmove_t *newanim, int nCommand, vec3_t vPos, 
								 edict_t* target, float fArg, int nID)
:action_c(od, oa, newanim, vPos, vPos, NULL, 999999, true)
{
	generic_ghoul_tank_ai* ai = (generic_ghoul_tank_ai*)(ai_public_c*)which_ai;
	body_tank* body = NULL;

	if (ai)
	{
		body = ai->GetTankBody();
	}
	if (!body)
	{
		return;
	}
	m_nCommand = nCommand;
	action_target = target;
	m_think = NULL;
	m_nextthink = NULL;

	// save our id (uniquely determined by the ai)
	m_nID = nID;
	// save our script params
	m_fArg = fArg;
	VectorCopy(vPos, dest);
	action_target = target;

	// set our body params
	body->m_target = target;
	VectorCopy(vPos, body->m_vPos);
	body->m_fArg = fArg;
	
	switch(m_nCommand)
	{
	case generic_ghoul_tank_ai::tse_GOTOCOORDS:
		{
			m_think = TankW_GotoCoords;
			break;
		}
	case generic_ghoul_tank_ai::tse_FIRECANNONATCOORDS:
		{
			m_think = TankW_FireCannonAtCoords;
			break;
		}
	case generic_ghoul_tank_ai::tse_MACHGUNAUTO:
		{
			m_think = TankW_MachGunAuto;
			break;
		}
	case generic_ghoul_tank_ai::tse_DIE:
		{
			m_think = TankW_Die;
			break;
		}
	case generic_ghoul_tank_ai::tse_AIMTURRET:
		{
			m_think = TankW_AimTurret;
			break;
		}
	default:
		break;
	}
}

qboolean tank_action::Think(ai_c &which_ai, edict_t &monster)
{
	float fHealthPercentage = 1.0f;
	generic_ghoul_tank_ai* ai = (generic_ghoul_tank_ai*)(ai_public_c*)&which_ai;
	body_tank* body = NULL;

	if (ai)
	{
		body = ai->GetTankBody();
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
		bRet = ai->GetRetVal();

		fHealthPercentage = (float)monster.health/(float)monster.max_health;
		body->UpdateSmoke(&monster, fHealthPercentage);

	}

	return bRet;
}


// overriding this fn is a quick way of keeping action_c from screwing with
//our tank's sequence
qboolean tank_action::SetAnimation(ai_c &which_ai, edict_t &monster)
{
	//just bein safe
	if (!which_ai.GetBody())
	{
		gi.dprintf("tank_action::SetAnimation--ai has no body!\n");
		return false;
	}
	return true;
}

// overriding this fn is a quick way of keeping action_c from screwing with
//our tank's sequence
qboolean tank_action::ForceAnimation(ai_c &which_ai, edict_t &monster)
{
	//just bein safe
	if (!which_ai.GetBody())
	{
		gi.dprintf("tank_action::ForceAnimation--ai has no body!\n");
		return false;
	}
	return true;
}

tank_action::tank_action(tank_action *orig)
: action_c(orig)
{
	m_nID = orig->m_nID;    
	m_nCommand = orig->m_nCommand;
	m_fArg = orig->m_fArg;   

	*(int *)&m_think = GetThinkNum(orig->m_think);
	*(int *)&m_nextthink = GetThinkNum(orig->m_nextthink);
}

void tank_action::Evaluate(tank_action *orig)
{
	m_nID = orig->m_nID;    
	m_nCommand = orig->m_nCommand;
	m_fArg = orig->m_fArg;   

	m_think = (void (*)(edict_t *self))GetThinkPtr((int)orig->m_think);
	m_nextthink = (void (*)(edict_t *self))GetThinkPtr((int)orig->m_nextthink);

	action_c::Evaluate(orig);
}

void tank_action::Read()
{
	char	loaded[sizeof(tank_action)];

	gi.ReadFromSavegame('AITK', loaded, sizeof(tank_action));
	Evaluate((tank_action *)loaded);
}

void tank_action::Write()
{
	tank_action	*savable;

	savable = new tank_action(this);
	gi.AppendToSavegame('AITK', savable, sizeof(*this));
	delete savable;
}

