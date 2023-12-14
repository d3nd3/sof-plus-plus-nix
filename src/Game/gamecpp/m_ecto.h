
class generic_ecto_ai : public generic_human_ai
{
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual	const char	*GetInterpFile(char *theclass, char *subclass);
	virtual void	AddBody(edict_t *monster);
};


class skinleader_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return TEAM_SKINHEADS;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_SKINLEADER;}//this should be different for everybody in this class family
};

class stockbroker_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_STOCKBROKER;}//this should be different for everybody in this class family
};

class tourist_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_DCTOURIST;}//this should be different for everybody in this class family
};

class zitpunk_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return TEAM_SKINHEADS;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_PUNK2;}//this should be different for everybody in this class family
};

class bum_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		IsInnocent() { return 1; }
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_NYCBUM;}//this should be different for everybody in this class family
};

class prisoner1_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_COLPRISONER1;}//this should be different for everybody in this class family
};

class prisoner2_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_COLPRISONER2;}//this should be different for everybody in this class family
};

class irqcitizen_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		IsInnocent() { return 1; }
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_IRAQCITIZEN;}//this should be different for everybody in this class family
};

class commander_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_UGNCOMMANDER;}//this should be different for everybody in this class family
};

class serbofficer_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_SERBOFFICER;}//this should be different for everybody in this class family
};

class kosrefugee_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_KOSREFUGEE;}//this should be different for everybody in this class family
};

class iraqofficer_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_IRAQOFFICER;}//this should be different for everybody in this class family
};

class factoryworker_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_FACTORY;}//this should be different for everybody in this class family
};

class chemist_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_CHEMIST;}//this should be different for everybody in this class family
};

class sibsuit_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_SIBSUIT;}//this should be different for everybody in this class family
};

class sibscientist_ai : public generic_ecto_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_ECTO_SIBSCIENCE;}//this should be different for everybody in this class family
};
