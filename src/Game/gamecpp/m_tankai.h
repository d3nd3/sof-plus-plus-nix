// m_tankai.h

#ifndef _M_TANKAI_H_
#define _M_TANKAI_H_

class generic_ghoul_tank_ai : public ai_c
{
public:
enum hse_Commands
		{
			tse_NONE=0,					  //
			tse_GOTOCOORDS,
			tse_FIRECANNONATCOORDS,
			tse_MACHGUNAUTO,
			tse_DIE,
			tse_AIMTURRET
		};

protected:
#define GGTA_SAVE_START	offsetof(generic_ghoul_tank_ai, m_ScriptActionCounter)
	int			m_ScriptActionCounter;
	int			m_LastThinkingActionID;
	edict_t		*m_attacker;
	qboolean	m_bTimeToDie;
	bool		m_bMoveBackward;
	vec3_t		m_vCannonTarget;
	bool		m_bMachGunAuto;
	bool		m_bCheckMachGunLOS;
	bool		m_bRetVal;
	float		m_fLastBurst;
	vec3_t		m_vMachGunTarget;
#define GGTA_SAVE_END	(offsetof(generic_ghoul_tank_ai, m_vMachGunTarget) + sizeof(m_vMachGunTarget))

public:
								generic_ghoul_tank_ai();
	virtual						~generic_ghoul_tank_ai();
	virtual	void				Init(edict_t *self, char *ghoulname, char* subclass);
	virtual	void				Activate(edict_t &monster);
	inline virtual	body_tank	*GetTankBody(void){return (body_tank*)(body_c*)body;}
	virtual	void				Think(edict_t &monster);
	virtual void				AddBody(edict_t *monster);
	virtual int					GetClassCode(void){return AI_TANK;}//this should be different for everybody in this class family
	int							GetCurrentActionID();
	int							GetMostRecentlyAddedActionID();
	int							GetLastThinkingActionID() { return m_LastThinkingActionID; }
	virtual action_c			*TankAction(decision_c *od, action_c *oa, ai_c* ai, mmove_t *newanim,
									int nCommand, vec3_t vPos, edict_t* target, float fArg);
	virtual bool				AmIAsGoodAsDead() {return false;}

								generic_ghoul_tank_ai(generic_ghoul_tank_ai *orig);
			void				Evaluate(generic_ghoul_tank_ai *orig);
	virtual void				Write();
	virtual void				Read();

	virtual void				Pain(edict_t &monster, edict_t *other, float kick, int damage);
	virtual void				Die(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
	virtual qboolean			Damage(edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb);
	qboolean					IsTimeToDie() { return m_bTimeToDie; }
	void						TimeToDie(qboolean bTime) { m_bTimeToDie = bTime; }
	void						SetCurrentActionNextThink(void	(*think)(edict_t *entity));
	void						SetMachGunAuto(bool bOn) { m_bMachGunAuto = bOn;}
	void						MoveBackward(bool bOn) { m_bMoveBackward = bOn;}

	// action functions available for tank_actions
	bool						Tank_GotoCoords(edict_t *entity);
	bool						Tank_FireCannonAtCoords(edict_t *entity);
	bool						Tank_MachGunAuto(edict_t *entity);

	// helper functions for dealing with tank_actions
	bool						TankH_FaceCoords(edict_t *entity);
	bool						TankH_MoveForward(edict_t *entity);
	bool						TankH_AimCannon(edict_t *entity);
	bool						TankH_FireCannon(edict_t *entity);
	bool						TankH_CannonAttack(edict_t *entity);
	bool						TankH_FireMachGun(edict_t *entity);

	bool						GetRetVal() { return m_bRetVal; }
	void						SetRetVal(bool bVal) { m_bRetVal = bVal; }

protected:

	virtual int					AimMainGun(vec3_t vTarget);
	virtual bool				AimMachGun(vec3_t vTarget);
	void						SetCannonTarget(vec3_t vPos) { VectorCopy(vPos, m_vCannonTarget); }
	void						SetMachGunTarget(vec3_t vPos) { VectorCopy(vPos, m_vMachGunTarget); }
	void						GetCannonTarget(vec3_t vTarg) { VectorCopy(m_vCannonTarget, vTarg); }
	void						GetMachGunTarget(vec3_t vTarg) { VectorCopy(m_vMachGunTarget, vTarg); }
};


// wrappers for ai functions that can be called by tank_actions
void TankW_GotoCoords(edict_t *ent);
void TankW_FireCannonAtCoords(edict_t *ent);
void TankW_CannonAttack(edict_t *ent);
void TankW_MachGunAuto(edict_t *ent);
void TankW_Die(edict_t *ent);
void TankW_AimTurret(edict_t *ent);

#endif //_M_TANKAI_H_