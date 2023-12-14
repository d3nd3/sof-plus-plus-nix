// m_snowcatactions.cpp

#include "g_local.h"
#include "ai_private.h"
#include "m_snowcatai.h"
 
//#include <windows.h>	// for OutputDebugString() only, do not leave in!


/**********************************************************************************
 **********************************************************************************/
snowcat_action::snowcat_action(decision_c *od, action_c *oa, ai_c* which_ai, 
								 mmove_t *newanim, int nCommand, vec3_t vPos, 
								 edict_t* target, float fArg, int nID)
:action_c(od, oa, newanim, vPos, vPos, NULL, 999999, true)
{
	generic_ghoul_snowcat_ai* ai = (generic_ghoul_snowcat_ai*)(ai_public_c*)which_ai;
	body_snowcat* body = NULL;

	if (ai)
	{
		body = ai->GetSnowcatBody();
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
	case generic_ghoul_snowcat_ai::tse_GOTOCOORDS:
		{
			m_think = SnowcatW_GotoCoords;
			break;
		}
	case generic_ghoul_snowcat_ai::tse_FIRECANNONATCOORDS:
		{
			m_think = SnowcatW_FireCannonAtCoords;
			break;
		}
	case generic_ghoul_snowcat_ai::tse_MACHGUNAUTO:
		{
			m_think = SnowcatW_MachGunAuto;
			break;
		}
	default:
		break;
	}
}

qboolean snowcat_action::Think(ai_c &which_ai, edict_t &monster)
{
	float fHealthPercentage = 1.0f;
	generic_ghoul_snowcat_ai* ai = (generic_ghoul_snowcat_ai*)(ai_public_c*)&which_ai;
	body_snowcat* body = NULL;

	if (ai)
	{
		body = ai->GetSnowcatBody();
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
//our snowcat's sequence
qboolean snowcat_action::SetAnimation(ai_c &which_ai, edict_t &monster)
{
	//just bein safe
	if (!which_ai.GetBody())
	{
		gi.dprintf("snowcat_action::SetAnimation--ai has no body!\n");
		return false;
	}
	return true;
}

// overriding this fn is a quick way of keeping action_c from screwing with
//our snowcat's sequence
qboolean snowcat_action::ForceAnimation(ai_c &which_ai, edict_t &monster)
{
	//just bein safe
	if (!which_ai.GetBody())
	{
		gi.dprintf("snowcat_action::ForceAnimation--ai has no body!\n");
		return false;
	}
	return true;
}

snowcat_action::snowcat_action(snowcat_action *orig)
: action_c(orig)
{
	m_nID = orig->m_nID;    
	m_nCommand = orig->m_nCommand;
	m_fArg = orig->m_fArg;   

	*(int *)&m_think = GetThinkNum(orig->m_think);
	*(int *)&m_nextthink = GetThinkNum(orig->m_nextthink);
}

void snowcat_action::Evaluate(snowcat_action *orig)
{
	m_nID = orig->m_nID;    
	m_nCommand = orig->m_nCommand;
	m_fArg = orig->m_fArg;   

	m_think = (void (*)(edict_t *self))GetThinkPtr((int)orig->m_think);
	m_nextthink = (void (*)(edict_t *self))GetThinkPtr((int)orig->m_nextthink);

	action_c::Evaluate(orig);
}

void snowcat_action::Read()
{
	char	loaded[sizeof(snowcat_action)];

	gi.ReadFromSavegame('AITK', loaded, sizeof(snowcat_action));
	Evaluate((snowcat_action *)loaded);
}

void snowcat_action::Write()
{
	snowcat_action	*savable;

	savable = new snowcat_action(this);
	gi.AppendToSavegame('AITK', savable, sizeof(*this));
	delete savable;
}

