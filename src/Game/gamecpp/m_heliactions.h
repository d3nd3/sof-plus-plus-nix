/**********************************************************************************
 **********************************************************************************/

class heli_action : public action_c
{
private:
protected:
	int		m_nID;
	int		m_nCommand;
	float	m_fArg;

	void	(*m_think)(edict_t *self);
	void	(*m_nextthink)(edict_t *self);

	virtual qboolean	SetAnimation(ai_c &which_ai, edict_t &monster);
	virtual qboolean	ForceAnimation(ai_c &which_ai, edict_t &monster);

	virtual qboolean	InsertPseudoAction(body_heli* body, edict_t* self);

public:

	heli_action(decision_c *od, action_c *oa, mmove_t *newanim, int nCommand, vec3_t vPos, edict_t* target, float fArg);
	heli_action(void):action_c(){ m_nID = -1;}
	virtual ~heli_action(void) {}

	virtual int		GetClassCode(void){return HELI_ACTION;}//this should be different for everybody in this class family

	virtual actiontype_e Type(void){return atype_walk;}
	virtual qboolean	Think(ai_c &which_ai, edict_t &monster);
	virtual void SetThink(void	(*think)(edict_t *self)) { m_think = think; }
	virtual void SetNextThink(void	(*think)(edict_t *self)) { m_nextthink = think; }

	virtual int GetID() { return m_nID; }

			heli_action(heli_action *orig);
			void Evaluate(heli_action *orig);
	virtual void Write(void);
	virtual void Read(void);
};

/**********************************************************************************
 **********************************************************************************/

class helimove_action : public heli_action
{
private:
protected:

public:

	helimove_action(decision_c *od, action_c *oa, ai_c* which_ai, mmove_t *newanim, int nCommand, vec3_t vPos, edict_t* target, float fArg, int nID);
	helimove_action(void):heli_action(){}
	virtual ~helimove_action(void) {}

	virtual int		GetClassCode(void){return HELIMOVE_ACTION;}//this should be different for everybody in this class family

	virtual actiontype_e Type(void){return atype_walk;}
};

/**********************************************************************************
 **********************************************************************************/

class heliface_action : public heli_action
{
private:
protected:

public:

	heliface_action(decision_c *od, action_c *oa, ai_c* which_ai, mmove_t *newanim, int nCommand, vec3_t vPos, edict_t* target, float fArg, int nID);
	heliface_action(void):heli_action(){}
	virtual ~heliface_action(void) {}

	virtual int		GetClassCode(void){return HELIFACE_ACTION;}//this should be different for everybody in this class family

	virtual actiontype_e Type(void){return atype_walk;}
};

/**********************************************************************************
 **********************************************************************************/

class heliattack_action : public heli_action
{
private:
protected:

public:

	heliattack_action(decision_c *od, action_c *oa, ai_c* which_ai, mmove_t *newanim, int nCommand, vec3_t vPos, edict_t* target, float fArg, int nID);
	heliattack_action(void):heli_action(){}
	virtual ~heliattack_action(void) {}

	virtual int		GetClassCode(void){return HELIATTACK_ACTION;}//this should be different for everybody in this class family

	virtual actiontype_e Type(void){return atype_walk;}
};

/**********************************************************************************
 **********************************************************************************/

class helideath_action : public heli_action
{
private:
protected:

public:

	helideath_action(decision_c *od, action_c *oa, ai_c* which_ai, mmove_t *newanim, int nCommand, vec3_t vPos, edict_t* target, float fArg, int nID);
	helideath_action(void):heli_action(){}
	virtual ~helideath_action(void) {}

	virtual int		GetClassCode(void){return HELIDEATH_ACTION;}//this should be different for everybody in this class family

	virtual actiontype_e Type(void){return atype_walk;}
};
