// m_snowcatai.h

#ifndef _M_SNOWCATAI_H_
#define _M_SNOWCATAI_H_
#include "m_tankai.h"

class generic_ghoul_snowcat_ai : public generic_ghoul_tank_ai//ai_c
{
public:
enum hse_Commands
		{
			tse_NONE=0,					  //
			tse_GOTOCOORDS,
			tse_FIRECANNONATCOORDS,
			tse_MACHGUNAUTO
		};

protected:
#define GGSA_SAVE_START	offsetof(generic_ghoul_snowcat_ai, m_ScriptActionCounter)
/*	int			m_ScriptActionCounter;
	int			m_LastThinkingActionID;
	edict_t		*m_attacker;
	qboolean	m_bTimeToDie;
	vec3_t		m_vCannonTarget;
	bool		m_bMachGunAuto;
	bool		m_bCheckMachGunLOS;
	float		m_fLastBurst;*/
//	vec3_t		m_vMachGunTarget;
#define GGSA_SAVE_END	(offsetof(generic_ghoul_snowcat_ai, m_vMachGunTarget) + sizeof(m_vMachGunTarget))


public:
								generic_ghoul_snowcat_ai();
	virtual						~generic_ghoul_snowcat_ai();
	virtual	void				Init(edict_t *self, char *ghoulname, char* subclass);
	virtual	void				Activate(edict_t &monster);
	inline virtual	body_snowcat	*GetSnowcatBody(void){return (body_snowcat*)(body_c*)body;}
	virtual	void				Think(edict_t &monster);
	virtual void				AddBody(edict_t *monster);
	virtual int					GetClassCode(void){return AI_SNOWCAT;}//this should be different for everybody in this class family
	int							GetCurrentActionID();
	int							GetMostRecentlyAddedActionID();
	int							GetLastThinkingActionID() { return m_LastThinkingActionID; }
//	virtual action_c			*TankAction(decision_c *od, action_c *oa, ai_c* ai, mmove_t *newanim,
//									int nCommand, vec3_t vPos, edict_t* target, float fArg);

								generic_ghoul_snowcat_ai(generic_ghoul_snowcat_ai *orig);

	virtual void				Pain(edict_t &monster, edict_t *other, float kick, int damage);
	virtual void				Die(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
	virtual qboolean			Damage(edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb);
	qboolean					IsTimeToDie() { return m_bTimeToDie; }
	void						TimeToDie(qboolean bTime) { m_bTimeToDie = bTime; }
	void						SetCurrentActionNextThink(void	(*think)(edict_t *entity));
	void						SetMachGunAuto(bool bOn) { m_bMachGunAuto = bOn;}

	// action functions available for snowcat_actions
	bool						Snowcat_GotoCoords(edict_t *entity);
	bool						Snowcat_FireCannonAtCoords(edict_t *entity);
	bool						Snowcat_MachGunAuto(edict_t *entity);

	// helper functions for dealing with snowcat_actions
	bool						SnowcatH_FaceCoords(edict_t *entity);
	bool						SnowcatH_MoveForward(edict_t *entity);
	bool						SnowcatH_AimCannon(edict_t *entity);
	bool						SnowcatH_FireCannon(edict_t *entity);
	bool						SnowcatH_FireMachGun(edict_t *entity);

protected:

	virtual int					AimMainGun(vec3_t vTarget);
	virtual bool				AimMachGun(vec3_t vTarget);
	void						SetCannonTarget(vec3_t vPos) { VectorCopy(vPos, m_vCannonTarget); }
	void						SetMachGunTarget(vec3_t vPos) { VectorCopy(vPos, m_vMachGunTarget); }
	void						GetCannonTarget(vec3_t vTarg) { VectorCopy(m_vCannonTarget, vTarg); }
	void						GetMachGunTarget(vec3_t vTarg) { VectorCopy(m_vMachGunTarget, vTarg); }
};


// wrappers for ai functions that can be called by snowcat_actions
void SnowcatW_GotoCoords(edict_t *ent);
void SnowcatW_FireCannonAtCoords(edict_t *ent);
void SnowcatW_MachGunAuto(edict_t *ent);

#endif //_M_SNOWCATAI_H_
