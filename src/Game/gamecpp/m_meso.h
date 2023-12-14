
class john_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_JOHN;}//this should be different for everybody in this class family
};

class john_snow_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_JOHN_SNOW;}//this should be different for everybody in this class family
};

class john_desert_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_JOHN_DESERT;}//this should be different for everybody in this class family
};

class hawk_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_HAWK;}//this should be different for everybody in this class family
};

class hurthawk_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_HURTHAWK;}//this should be different for everybody in this class family
};

class sam_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SAM;}//this should be different for everybody in this class family
};

class skinheadboss_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return TEAM_SKINHEADS;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SKINHEADBOSS;}//this should be different for everybody in this class family
};

class amu_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_AMU;}//this should be different for everybody in this class family
};

class raiderboss_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	AddBody(edict_t *monster);
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_RAIDERBOSS;}//this should be different for everybody in this class family
};

class raiderboss2_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual	void	Think(edict_t &monster);
	virtual void	AddBody(edict_t *monster);
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_RAIDERBOSS2;}//this should be different for everybody in this class family
};

class nypunk_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return TEAM_SKINHEADS;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_NYCPUNK;}//this should be different for everybody in this class family
};

class nyswatguy_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual int		GetClassCode(void){return AI_MESO_NYCSWATGUY;}//this should be different for everybody in this class family
};

class nyswatleader_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual int		GetClassCode(void){return AI_MESO_NYCSWATLEADER;}//this should be different for everybody in this class family
};

class malepolitician_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual int		GetClassCode(void){return AI_MESO_MALEPOLITICIAN;}//this should be different for everybody in this class family
};

class skinhead1_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return TEAM_SKINHEADS;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SKINHEAD1;}//this should be different for everybody in this class family
};

class skinhead2a_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return TEAM_SKINHEADS;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SKINHEAD2A;}//this should be different for everybody in this class family
};

class skinhead2b_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return TEAM_SKINHEADS;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SKINHEAD2B;}//this should be different for everybody in this class family
};

class ugsoldier1_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_UGNSOLDIER1;}//this should be different for everybody in this class family
};

class ugsoldier1b_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_UGNSOLDIER1B;}//this should be different for everybody in this class family
};

class ugsoldier2_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_UGNSOLDIER2;}//this should be different for everybody in this class family
};

class ugsoldier3_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_UGNSOLDIER3;}//this should be different for everybody in this class family
};

class ugsniper_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_UGNSNIPER;}//this should be different for everybody in this class family
};

class ugbrute_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_UGNBRUTE;}//this should be different for everybody in this class family
};

class ugrocket_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_UGNROCKET;}//this should be different for everybody in this class family
};

class irqsoldier1_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_IRAQSOLDIER1;}//this should be different for everybody in this class family
};

class irqsoldier2_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_IRAQSOLDIER2;}//this should be different for everybody in this class family
};

class irqsoldier2b_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_IRAQSOLDIER2B;}//this should be different for everybody in this class family
};

class irqrepgd_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_IRAQREPGUARD;}//this should be different for everybody in this class family
};

class irqrepgdb_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_IRAQREPGUARDB;}//this should be different for everybody in this class family
};

class irqpolice_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_IRAQPOLICE;}//this should be different for everybody in this class family
};

class irqbodyguard_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_IRAQBODYGUARD;}//this should be different for everybody in this class family
};

class irqcommander_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_IRAQCOMMANDER;}//this should be different for everybody in this class family
};

class irqbrutea_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_IRAQBRUTEA;}//this should be different for everybody in this class family
};

class irqbruteb_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_IRAQBRUTEB;}//this should be different for everybody in this class family
};

class irqrocket_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_IRAQROCKET;}//this should be different for everybody in this class family
};

class irqsaddam_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_IRAQSADDAM;}//this should be different for everybody in this class family
};

class irqworker_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		IsInnocent() { return 1; }
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_IRAQWORKER;}//this should be different for everybody in this class family
};

class irqman2_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		IsInnocent() { return 1; }
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_IRAQMAN2;}//this should be different for everybody in this class family
};

class raider1_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_RAIDER1;}//this should be different for everybody in this class family
};

class raider2_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_RAIDER2A;}//this should be different for everybody in this class family
};

class raider2b_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_RAIDER2B;}//this should be different for everybody in this class family
};

class raiderbrute_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_RAIDERBRUTE;}//this should be different for everybody in this class family
};

class raiderrocket_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_RAIDERROCKET;}//this should be different for everybody in this class family
};

class sibtrooper1a_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SIBTROOPER1A;}//this should be different for everybody in this class family
};

class sibtrooper1b_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SIBTROOPER1B;}//this should be different for everybody in this class family
};

class sibtrooper2_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SIBTROOPER2;}//this should be different for everybody in this class family
};

class sibguard_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SIBGUARD;}//this should be different for everybody in this class family
};

class sibguard3_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SIBGUARD3;}//this should be different for everybody in this class family
};

class sibguard4_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SIBGUARD4;}//this should be different for everybody in this class family
};

class sibcleansuit_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SIBCLEANSUIT;}//this should be different for everybody in this class family
};

class sibmech_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		IsInnocent() { return 1; }
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SIBMECH;}//this should be different for everybody in this class family
};

class serbgrunt1_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SERBGRUNT1;}//this should be different for everybody in this class family
};

class serbgrunt2_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SERBGRUNT2;}//this should be different for everybody in this class family
};

class serbgrunt3_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SERBGRUNT3;}//this should be different for everybody in this class family
};

class serbsniper1a_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SERBSNIPER1A;}//this should be different for everybody in this class family
};

class serbsniper1b_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SERBSNIPER1B;}//this should be different for everybody in this class family
};

class serbcomtroop_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SERBCOMTROOP;}//this should be different for everybody in this class family
};

class serbbrute1a_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SERBBRUTE1A;}//this should be different for everybody in this class family
};

class serbbrute1b_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SERBBRUTE1B;}//this should be different for everybody in this class family
};

class serbmechanic_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		IsInnocent() { return 1; }
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_SERBMECHANIC;}//this should be different for everybody in this class family
};

class kosrebel_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_KOSREBEL;}//this should be different for everybody in this class family
};

class kosklaguy_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_KOSKLAGUY;}//this should be different for everybody in this class family
};

class tokmalehostage_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetTeam(edict_t &monster){return 0;}
	virtual int		GetClassCode(void){return AI_MESO_TOKMALEHOSTAGE;}//this should be different for everybody in this class family
};

class tokhench1_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_TOKHENCH1;}//this should be different for everybody in this class family
};

class tokhench2_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_TOKHENCH2;}//this should be different for everybody in this class family
};

class tokkiller_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_TOKKILLER;}//this should be different for everybody in this class family
};

class tokninja_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_TOKNINJA;}//this should be different for everybody in this class family
};

class tokbrute_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual void	Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_TOKBRUTE;}//this should be different for everybody in this class family
};

class japansuit_ai : public generic_meso_ai
{
protected:
	virtual	void	RegisterSkins(void);
public:
	virtual int	 GetTeam(edict_t &monster){return 0;}
	virtual void			Init(edict_t *monster, char *ghoulname, char* subclass);
	virtual int		GetClassCode(void){return AI_MESO_JAPANSUIT;}//this should be different for everybody in this class family
};

