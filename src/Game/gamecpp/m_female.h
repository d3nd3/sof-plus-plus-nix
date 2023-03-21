
class generic_female_ai : public generic_human_ai
{
protected:
public:
	virtual	const char	*GetInterpFile(char *theclass, char *subclass);
	virtual void	AddBody(edict_t *monster);
};


class skinchick_ai : public generic_female_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return TEAM_SKINHEADS;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_FEMALE_SKINCHICK;}//this should be different for everybody in this class family
};

class nycwoman_ai : public generic_female_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_FEMALE_NYWOMAN;}//this should be different for everybody in this class family
};

class fsibguard_ai : public generic_female_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_FEMALE_SIBGUARD;}//this should be different for everybody in this class family
};

class fsibscience_ai : public generic_female_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_FEMALE_SIBSCIENCE;}//this should be different for everybody in this class family
};

class irqwoman1_ai : public generic_female_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		IsInnocent() { return 1; }
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_FEMALE_IRQWOMAN1;}//this should be different for everybody in this class family
};

class irqwoman2_ai : public generic_female_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		IsInnocent() { return 1; }
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_FEMALE_IRQWOMAN2;}//this should be different for everybody in this class family
};

class tokwoman1_ai : public generic_female_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_FEMALE_TOKWOMAN1;}//this should be different for everybody in this class family
};

class tokwoman2_ai : public generic_female_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_FEMALE_TOKWOMAN2;}//this should be different for everybody in this class family
};

class tokassassin_ai : public generic_female_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_FEMALE_TOKASSASSIN;}//this should be different for everybody in this class family
};

class fraider_ai : public generic_female_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_FEMALE_RAIDER;}//this should be different for everybody in this class family
};

class taylor_ai : public generic_female_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_FEMALE_TAYLOR;}//this should be different for everybody in this class family
};
