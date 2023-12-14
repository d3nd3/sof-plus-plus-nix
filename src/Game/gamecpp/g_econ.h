#define		DEFAULT_KILLED_VALUE	100
#define		DEFAULT_SURVIVAL_VALUE	0
#define		DEFAULT_MONSTER_CLASS	ENEMY_GRUNT

typedef enum
{
	ENEMY_GRUNT,	 // these should be set for statistical record keeping
	ENEMY_LEADER,
	ENEMY_ANIMAL,
	ENEMY_BOSS,
	NPC,
	NPC_ANIMAL,
	SPECIAL,
} monsterclass;

class CTokenizer;

class CMonsterStats
{

private:
	int					m_MonsterID;
	int					m_KilledValue;
	int					m_SurvivalValue;
	monsterclass		m_MonsterClass;

public:
			CMonsterStats(edict_t* Monster = NULL, int KilledValue = DEFAULT_KILLED_VALUE, int SurvivalValue = DEFAULT_SURVIVAL_VALUE, monsterclass myClass = ENEMY_GRUNT);
	void	Delete();		

	int		GetKilledValue(){return m_KilledValue;};
	void	SetKilledValue(int amount){m_KilledValue = amount;};

	int		GetSurvivalValue(){return m_SurvivalValue;};
	void	SetSurvivalValue(int amount){m_SurvivalValue = amount;};

	monsterclass	GetMonsterClass(){return m_MonsterClass;};
	void			SetMonsterClass(monsterclass myClass){m_MonsterClass = myClass;};

	bool	MatchMonsterID(edict_t *ent);
};


#define		PLAYER_STARTING_CASH_IN_BANK	10000
#define		PLAYER_STARTING_CASH_IN_HAND	0

class CPlayerStats
{
private:
	long	m_CashToBeAwarded;
	long	m_CashOnHand;
	long	m_CashInBank;
	bool	m_Buymode;

public:
	void	Init(long CashInBank = PLAYER_STARTING_CASH_IN_BANK, long CashOnHand = PLAYER_STARTING_CASH_IN_HAND);

	void	SetBuyModeOnOff(bool On){m_Buymode = On;}
	bool	GetBuyModeOnOff(void){return m_Buymode;}

	long	GetCashToBeAwarded(){return m_CashToBeAwarded;};
	void	SetCashToBeAwarded(long amount){m_CashToBeAwarded = amount;};
	void	AdjustCashToBeAwarded(long amount){m_CashToBeAwarded += amount;};

	long	GetCashOnHand(){return m_CashOnHand;};
	void	SetCashOnHand(long amount){m_CashOnHand = amount;};
	void	AdjustCashOnHand(long amount){m_CashOnHand += amount;};

	long	GetCashInBank(){return m_CashInBank;};
	void	SetCashInBank(long amount){m_CashInBank = amount;};
	void	AdjustCashInBank(long amount){m_CashInBank += amount;};
};

class CNonPlayer
{
private:
	char*					m_MonsterName;
	int						m_KilledValue;
	int						m_SurvivalValue;
	monsterclass			m_MonsterClass;

public:
					CNonPlayer(void);
					CNonPlayer(const char* name);
	void			Delete();
	bool			ParseNonPlayerData(CTokenizer* Tokenizer);
	char*			GetMonsterName(){return m_MonsterName;};
	int				GetKilledValue(){return m_KilledValue;};
	int				GetSurvivalValue(){return m_SurvivalValue;};
	monsterclass	GetMonsterClass(){return m_MonsterClass;};
};


class CLocation
{	
private:
	List<CNonPlayer *>			m_NonPlayerList;
	char*						m_LocationName;

public:
					CLocation(void);
					CLocation(const char* name);
	void			Delete();
	bool			RegisterNonPlayers(CTokenizer* Tokenizer);
	int				GetDefaultKilledValue(CTokenizer* Tokenizer);
	int				GetDefaultSurvivalValue(CTokenizer* Tokenizer);
	monsterclass	GetDefaultMonsterClass(CTokenizer* Tokenizer);
	char*			GetLocationName(){return m_LocationName;};
};

class CMonsterList
{
private:
	List<CLocation *>		m_LocationList;


public:
					CMonsterList();
	void			Delete();
	bool			RegisterLocations(CTokenizer* Tokenizer);
	int				GetDefaultKilledValue(CTokenizer* Tokenizer);
	int				GetDefaultSurvivalValue(CTokenizer* Tokenizer);
	monsterclass	GetDefaultMonsterClass(CTokenizer* Tokenizer);

};

class CEconomyRegistry
{
private:
	CMonsterList*	m_MonsterList;
//	CObjectList *	m_ObjectList;
//  etc....   Should probably change these to <map> templates once I understand them...

void	RegisterMonsters(CTokenizer* Tokenizer);

public:
					CEconomyRegistry(void);
	void			Delete();
	
	CMonsterList*	GetMonsterList(void){return m_MonsterList;};
	int				GetDefaultKilledValue(edict_t* ent);
	int				GetDefaultSurvivalValue(edict_t* ent);
	monsterclass	GetDefaultMonsterClass(edict_t* ent);
};

// make sure this thing is deallocated when the game is quit
class CGameStats
{
private:
	List<CMonsterStats *>		m_MonsterStats;
	CEconomyRegistry*			m_EconomyRegistry;
	
	void			FragmentEntClassname(edict_t* ent, char* ClassName);

public:
								CGameStats(void);
	void			Delete();
	int				GetDefaultKilledValue(edict_t *ent);
	int				GetDefaultSurvivalValue(edict_t *ent);
	monsterclass	GetDefaultMonsterClass(edict_t *ent);
	CMonsterStats*	GetMonsterStats(edict_t *ent);
	void			AddEconomyRegistry(void);
	void			AddMonsterToStatsList(edict_t *ent, float killedValue = 12345678, float survivalValue = 12345678);
	void			BuyModeImpulse(edict_t *ent, int impulse);//currently, this will call exitbuymode with the right impuls
	void			EnterBuyMode(edict_t *ent);
	void			ExitBuyMode(edict_t *ent);
	bool			CanBuy(edict_t *ent);
};

CPlayerStats *GetPlayerStats(edict_t *ent);
