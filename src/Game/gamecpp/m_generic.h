
//initialization functions that should be useful...
void generic_monster_spawnnow (edict_t *self);
void generic_armor_spawnnow (edict_t *self);
void dekker_init (edict_t *self);
void generic_leader_init (edict_t *self);
void generic_grunt_init (edict_t *self);
void generic_heavyweapons_init (edict_t *self);
void generic_npc_init (edict_t *self);
void generic_animal_init (edict_t *self);
void generic_armor_init(edict_t *self);

typedef enum
{
	EXPRESSION_PRIORITY_LOWEST=0,
	EXPRESSION_PRIORITY_LOW,
	EXPRESSION_PRIORITY_HIGH,
	EXPRESSION_PRIORITY_HIGHEST,
} expressionPriority_e;

typedef enum
{
	VARIETY_PRIORITY_LOWEST=0,
	VARIETY_PRIORITY_LOW,
	VARIETY_PRIORITY_HIGH,
	VARIETY_PRIORITY_HIGHEST,
} varietyPriority_e;

class generic_human_ai : public ai_c
{
protected:
	virtual	void		RegisterSkins(void);
	virtual	void		RegisterGoreSkins(void);

	virtual	void		RegisterFaceSkin(const char *theMaterialName, const char *faceSkinName, expressionPriority_e expressionPriorityVal);
	virtual	void		RegisterSkin(const char *theMaterialName, const char *theSkinName, varietyPriority_e varietyPriorityVal);

	virtual	void		RegisterFaceSkins(const char *faceBase, varietyPriority_e varietyPriorityVal);
	virtual	void		RegisterTalkFaceSkins(const char *faceBase, varietyPriority_e varietyPriorityVal);
public:
	virtual	const char*	GetInterpFile(char *theclass, char *subclass);
	virtual				~generic_human_ai(){}
	virtual	void		Init(edict_t *monster, char *theclass, char* subclass);
	virtual int			GetClassCode(void){return AI_GENERIC;}//this should be different for everybody in this class family
						generic_human_ai():ai_c(){}
						generic_human_ai(generic_human_ai *orig):ai_c(orig){}
};

class playercorpse_ai : public generic_human_ai
{
protected:
	virtual	void	RegisterSkins(void){}
public:
	virtual action_c	*DeathAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
	virtual void		AddBody(edict_t *monster);
	virtual void		Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int			GetClassCode(void){return AI_PLAYERCORPSE;}//this should be different for everybody in this class family
};

class generic_meso_ai : public generic_human_ai
{
protected:
public:
	virtual	const char*	GetInterpFile(char *theclass, char *subclass);
	virtual void	AddBody(edict_t *monster);
};

class blownpart_ai : public generic_human_ai
{
private:
protected:
			float		severTime;
			bool		removeMe;
			bool		Stopped;
			bool		frozen;
			bool		bFading;
	virtual qboolean	IsFirstFrameTime(void){return false;}
public:
	virtual	const char*	GetInterpFile(char *theclass, char *subclass);
	virtual	const char*	GetObjectSkin(char *theclass, char *subclass);
	virtual void		Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int			GetClassCode(void){return AI_BLOWNPART;}//this should be different for everybody in this class family
	virtual void		Think(edict_t &monster);

						blownpart_ai():generic_human_ai(){}
						blownpart_ai(blownpart_ai *orig);
	virtual void		Evaluate(blownpart_ai *orig);
};

class generic_ghoul_dog_ai : public ai_c//generic_human_ai
{
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetTeam(edict_t &monster){return 1;}
	virtual int		GetClassCode(void){return AI_GENERIC_DOG;}//this should be different for everybody in this class family

	virtual action_c	*AttackAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t attackPos, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false);
	virtual action_c	*WalkAction(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false);

protected:
	virtual void	AddBody(edict_t *monster);
};

class dog_husky_ai : public generic_ghoul_dog_ai
{
protected:
	virtual void	AddBody(edict_t *monster);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass) {generic_ghoul_dog_ai::Init(monster, ghoulname, subclass);}
	virtual int		GetClassCode(void){return AI_DOG_HUSKY;}//this should be different for everybody in this class family
};

class dog_rottweiler_ai : public generic_ghoul_dog_ai
{
protected:
	virtual void	AddBody(edict_t *monster);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass) {generic_ghoul_dog_ai::Init(monster, ghoulname, subclass);}
	virtual int		GetClassCode(void){return AI_DOG_ROTTWEILER;}//this should be different for everybody in this class family
};

class cow_ai : public ai_c
{
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetTeam(edict_t &monster){return 100;}
	virtual int		GetClassCode(void){return AI_GENERIC_COW;}//this should be different for everybody in this class family

//	virtual action_c	*AttackAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t attackPos, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false);
//	virtual action_c	*WalkAction(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false);

protected:
	virtual void	AddBody(edict_t *monster);
};

