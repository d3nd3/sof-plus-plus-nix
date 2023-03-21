#include "g_local.h"
#include "Module.h"
#include "Tokenizer.h"

//  ****  STUFF FOR TOKENIZER  ****

extern keywordArray_t		Keywords[];
extern keywordArray_t		Symbols[];

enum
{
	// Keywords
	TK_MONSTERS = TK_USERDEF,
	TK_MONSTER,
	TK_LOCATION,
	TK_NAME,
	TK_CLASS,
	TK_ENEMY_GRUNT,
	TK_ENEMY_LEADER,
	TK_ENEMY_ANIMAL,
	TK_ENEMY_BOSS,
	TK_NPC,
	TK_NPC_ANIMAL,
	TK_SPECIAL,

	// SYMBOLS
	TK_LBRACE,
	TK_RBRACE,
	TK_UNDERSCORE,
};

keywordArray_t Keywords[] =
{
	"monsters",			TK_MONSTERS,
	"monster",			TK_MONSTER,
	"location",			TK_LOCATION,
	"name",				TK_NAME,
	"class",			TK_CLASS,
	"ENEMY_GRUNT",		TK_ENEMY_GRUNT,
	"ENEMY_LEADER",		TK_ENEMY_LEADER,
	"ENEMY_ANIMAL",		TK_ENEMY_ANIMAL,
	"ENEMY_BOSS",		TK_ENEMY_BOSS,
	"NPC",				TK_NPC,
	"NPC_ANIMAL",		TK_NPC_ANIMAL,
	"SPECIAL",			TK_SPECIAL,

	NULL,				TK_EOF,
};

keywordArray_t Symbols[] =
{
	"{",				TK_LBRACE,
	"}",				TK_RBRACE,
	"_",				TK_UNDERSCORE,
	NULL,				TK_EOF,
};


// **** END OF TOKENIZER STUFF ****

// ======  CMonsterStats Functions ====================

CMonsterStats::CMonsterStats(edict_t *Monster, int KilledValue, int SurvivalValue, monsterclass myClass)
{
	if (!Monster)
	{
		delete this;
		return;
	}

	m_KilledValue = KilledValue;
	m_SurvivalValue = SurvivalValue;
	m_MonsterClass = myClass;
	m_MonsterID = Monster->s.number;
}

void CMonsterStats::Delete ()
{
	delete this;
	return;
}

bool CMonsterStats::MatchMonsterID(edict_t *ent)
{
	if (m_MonsterID == ent->s.number )
	{
		return true;
	}
	return false;
}

// ======  CPlayerStats Functions =====================

void CPlayerStats::Init(long CashInBank, long CashOnHand)
{
	m_CashToBeAwarded = 0;
	m_CashInBank = CashInBank;
	m_CashOnHand = CashOnHand;
}

// ============== CNonPlayer functions ===========================

CNonPlayer::CNonPlayer(void)
{
	m_MonsterName = NULL;
	m_KilledValue = 0;
	m_SurvivalValue = 0;
	m_MonsterClass = ENEMY_GRUNT;
}

CNonPlayer::CNonPlayer(const char* name)
{
	m_MonsterName = (char*)malloc(strlen(name) + 1);
	strcpy(m_MonsterName, name);
	m_KilledValue = 0;
	m_SurvivalValue = 0;
	m_MonsterClass = ENEMY_GRUNT;
}

void CNonPlayer::Delete()
{
	free(m_MonsterName);
	m_MonsterName = NULL;

	delete this;
}

bool CNonPlayer::ParseNonPlayerData(CTokenizer* Tokenizer)
{
	CToken* Token;

//  Put these lines in once we have different missions <---  now difficulty levels
/*	if (game.GameStats->MissionNumber)
	{
		for(int count = 0; count < ((game.GameStats->MissionNumber)*2); count++)
		{
			Token = Tokenizer->GetToken();
			if (Token->GetType() != TK_INT)
			{
				Tokenizer->Error("Warning: Strange Values found while scanning for killed value");	
			}
		}
	}
*/
	Token = Tokenizer->GetToken();
	if (Token->GetType() != TK_INT)
	{
		Tokenizer->Error("Killed value not found after monster name");
		Tokenizer->PutBackToken(Token);
		return false;
	}
	m_KilledValue = Token->GetIntValue();
	Token->Delete();
	Token = Tokenizer->GetToken();
	if (Token->GetType() != TK_INT)
	{
		Tokenizer->Error("Survival value not found after Killed Value");
		Tokenizer->PutBackToken(Token);
		return false;
	}
	m_SurvivalValue = Token->GetIntValue();
	Token->Delete();
	Token = Tokenizer->GetToken();
	while (Token->GetType() != TK_CLASS)
	{
		if (Token->GetType() != TK_INT)
		{
			Tokenizer->Error("Unable to find monster class!");
			Tokenizer->PutBackToken (Token);
			return false;
		}
		Token->Delete();
		Token = Tokenizer->GetToken();
	}
	Token->Delete();
	Token = Tokenizer->GetToken();
	switch (Token->GetType())
	{
	case TK_ENEMY_GRUNT:
		m_MonsterClass = ENEMY_GRUNT;
		break;
	case TK_ENEMY_LEADER:
		m_MonsterClass = ENEMY_LEADER;
		break;
	case TK_ENEMY_ANIMAL:
		m_MonsterClass = ENEMY_ANIMAL;
		break;
	case TK_ENEMY_BOSS:
		m_MonsterClass = ENEMY_BOSS;
		break;
	case TK_NPC:
		m_MonsterClass = NPC;
		break;
	case TK_NPC_ANIMAL:
		m_MonsterClass = NPC_ANIMAL;
		break;
	case TK_SPECIAL:
		m_MonsterClass = SPECIAL;
		break;
	default:
		Tokenizer->Error("Invalid monster class found!");
		Tokenizer->PutBackToken (Token);
		return false;
	}
	Token->Delete ();
	return true;  //Yay!
}

// ============== CLocation functions ============================

CLocation::CLocation(void)
{
	m_LocationName = NULL;
}

CLocation::CLocation(const char* name)
{
	m_LocationName = (char*)malloc(strlen(name) + 1);
	strcpy(m_LocationName,name);
}

void CLocation::Delete()
{
	free(m_LocationName);
	m_LocationName = NULL;
	
	List<CNonPlayer*>::Iter		NPIter;
		
	for (NPIter = m_NonPlayerList.Begin(); NPIter!=m_NonPlayerList.End(); NPIter++)
	{
		(*NPIter)->Delete();
	}

	delete this;
}

bool CLocation::RegisterNonPlayers(CTokenizer* Tokenizer)
{
	CToken*  Token = Tokenizer->GetToken();
	if (Token->GetType() != TK_LBRACE)
	{
		Tokenizer->Error("Left Brace Expected After location Name");
		Tokenizer->PutBackToken(Token);
		return false;
	}
	Token->Delete();
	Token = Tokenizer->GetToken();
	if (Token->GetType() != TK_NAME)
	{
		Tokenizer->Error ("'name' expected after left brace following location");
		Tokenizer->PutBackToken(Token);
		return false;
	}
	while (Token->GetType() == TK_NAME)
	{
		Token->Delete();
		Token = Tokenizer->GetToken();
		if (Token->GetType() != TK_STRING)
		{
			Tokenizer->PutBackToken(Token);
			Tokenizer->Error ("No monster name found after keyword 'name'");
			if (m_NonPlayerList.Begin() != m_NonPlayerList.End())
			{
				return true; // some locations were successful, so we should keep them
			}
			return false; // no successful locations yet, kill 'em all!
		}
		LPCTSTR name = Token->GetStringValue();
		CNonPlayer* NonPlayer = new CNonPlayer(name);
		Token->Delete();

		bool success = NonPlayer->ParseNonPlayerData(Tokenizer);
		if (success)
		{
			m_NonPlayerList.PushBack(NonPlayer);
		}
		else
		{
			NonPlayer->Delete();
		}
		Token = Tokenizer->GetToken();
	}
	if (Token->GetType() != TK_RBRACE)
	{
		Tokenizer->Error("No closing brace found after name lists!");
		Tokenizer->PutBackToken(Token);
		return true; // keep what we've got				
	}
	Token->Delete();
	return true;
}

int CLocation::GetDefaultKilledValue(CTokenizer* Tokenizer)
{
	CToken *Token;
	List<CNonPlayer*>::Iter		NPIter;
	int	killedValue = DEFAULT_KILLED_VALUE;

	Token = Tokenizer->GetToken();
	if (Token->GetType() != TK_UNDERSCORE)
	{
		Tokenizer->Error("Underscore expected after location!");
		Token->Delete();
		return killedValue;
	}
	Token->Delete();
	Token = Tokenizer->GetToken((UINT)0, TKF_NOUNDERSCOREINIDENTIFIER);	
	for (NPIter = m_NonPlayerList.Begin(); NPIter!=m_NonPlayerList.End(); NPIter++)
	{
		if (!(stricmp((*NPIter)->GetMonsterName(), Token->GetStringValue())))
		{
			killedValue = (*NPIter)->GetKilledValue();
			Token->Delete();
			return killedValue;
		}
	}
	Tokenizer->Error("Economy.dat does not contain a monster entity name!");
	Token->Delete();
	return killedValue;
}

int CLocation::GetDefaultSurvivalValue(CTokenizer* Tokenizer)
{
	CToken *Token = 0;
	List<CNonPlayer*>::Iter		NPIter;
	int	SurvivalValue = DEFAULT_SURVIVAL_VALUE;

	Token = Tokenizer->GetToken();
	if (Token->GetType() != TK_UNDERSCORE)
	{
		Tokenizer->Error("Underscore expected after location!");
		Token->Delete();
		return SurvivalValue;
	}
	Token->Delete();
	Token = Tokenizer->GetToken((UINT)0, TKF_NOUNDERSCOREINIDENTIFIER);	
	for (NPIter = m_NonPlayerList.Begin(); NPIter!=m_NonPlayerList.End(); NPIter++)
	{
		if (!(stricmp((*NPIter)->GetMonsterName(), Token->GetStringValue())))
		{
			SurvivalValue = (*NPIter)->GetSurvivalValue();
			Token->Delete();
			return SurvivalValue;
		}
	}
	Tokenizer->Error("Economy.dat does not contain a monster entity name!");
	Token->Delete();
	return SurvivalValue;
}

monsterclass CLocation::GetDefaultMonsterClass(CTokenizer* Tokenizer)
{
	CToken *Token = 0;
	List<CNonPlayer*>::Iter		NPIter;
	monsterclass MonsterClass = DEFAULT_MONSTER_CLASS;

	Token = Tokenizer->GetToken();
	if (Token->GetType() != TK_UNDERSCORE)
	{
		Tokenizer->Error("Underscore expected after location!");
		Token->Delete();
		return MonsterClass;
	}
	Token->Delete();
	Token = Tokenizer->GetToken((UINT)0, TKF_NOUNDERSCOREINIDENTIFIER);	
	for (NPIter = m_NonPlayerList.Begin(); NPIter!=m_NonPlayerList.End(); NPIter++)
	{
		if (!(stricmp((*NPIter)->GetMonsterName(), Token->GetStringValue())))
		{
			MonsterClass = (*NPIter)->GetMonsterClass();
			Token->Delete();
			return MonsterClass;
		}
	}
	Tokenizer->Error("Economy.dat does not contain a monster entity name!");
	Token->Delete();
	return MonsterClass;
}

// ============== CMonsterList functions =========================

CMonsterList::CMonsterList()
{
//	m_LocationList = NULL;
}

void CMonsterList::Delete()
{
	List<CLocation*>::Iter		LocIter;
		
	for (LocIter = m_LocationList.Begin(); LocIter!=m_LocationList.End(); LocIter++)
	{
		(*LocIter)->Delete();
	}
	
	delete this;
}

bool CMonsterList::RegisterLocations (CTokenizer* Tokenizer)
{
	CToken* Token = Tokenizer->GetToken();
	if (Token->GetType() != TK_LBRACE)
	{
		Tokenizer->Error("Left Brace Expected After 'monsters' keyword");
		Tokenizer->PutBackToken(Token);
		return false;
	}
	else
	{
		Token->Delete();
		Token = Tokenizer->GetToken();
		if (Token->GetType() != TK_LOCATION)
		{
			Tokenizer->Error ("'location' expected after left brace");
			Tokenizer->PutBackToken(Token);
			return false;
		}
		else
		{
			while (Token->GetType() == TK_LOCATION)
			{
				Token->Delete();
				Token = Tokenizer->GetToken();
				if (Token->GetType() != TK_STRING)
				{
					Tokenizer->PutBackToken(Token);
					Tokenizer->Error ("No name found after keyword 'location'");
					if (m_LocationList.Begin() != m_LocationList.End())
					{
						return true; // some locations were successful, so we should keep them
					}
					return false; // no successful locations yet, kill 'em all!
				}
				else
				{
					LPCTSTR name = Token->GetStringValue();
					CLocation* Location = new CLocation(name);
					bool success = Location->RegisterNonPlayers(Tokenizer);
					if (success)
					{
						m_LocationList.PushBack(Location);
					}
					else
					{
						Location->Delete();
					}
				}
				Token->Delete();
				Token = Tokenizer->GetToken();
			}
		}
		if (Token->GetType() != TK_RBRACE)
		{
			Tokenizer->Error("No closing brace found after location lists!");
			Tokenizer->PutBackToken(Token);
			return true; // keep what we've got			
		}
	
	}
	Token->Delete();
	return true;
}

int CMonsterList::GetDefaultKilledValue(CTokenizer* Tokenizer)
{
	CToken *Token;
	List<CLocation*>::Iter		LocIter;
	int	killedValue = DEFAULT_KILLED_VALUE;

	Token = Tokenizer->GetToken();
	if (Token->GetType() != TK_UNDERSCORE)
	{
		Tokenizer->Error("Underscore expected after monster!");
		Token->Delete();
		return killedValue;
	}
	Token->Delete();
	Token = Tokenizer->GetToken();
	for (LocIter = m_LocationList.Begin(); LocIter!=m_LocationList.End(); LocIter++)
	{
		if (!(stricmp((*LocIter)->GetLocationName(), Token->GetStringValue())))
		{
			killedValue = (*LocIter)->GetDefaultKilledValue(Tokenizer);
			Token->Delete();
			return killedValue;
		}
	}
	Tokenizer->Error("Economy.dat does not contain a location specified in a monster name!");
	Token->Delete();
	return killedValue;
}

int CMonsterList::GetDefaultSurvivalValue(CTokenizer* Tokenizer)
{
	CToken *Token;
	List<CLocation*>::Iter		LocIter;
	int	SurvivalValue = DEFAULT_SURVIVAL_VALUE;

	Token = Tokenizer->GetToken();		
	if (Token->GetType() != TK_UNDERSCORE)
	{
		Tokenizer->Error("Underscore expected after monster!");
		Token->Delete();
		return SurvivalValue;
	}
	Token->Delete();
	Token = Tokenizer->GetToken();
	for (LocIter = m_LocationList.Begin(); LocIter!=m_LocationList.End(); LocIter++)
	{
		if (!(stricmp((*LocIter)->GetLocationName(), Token->GetStringValue())))
		{
			SurvivalValue = (*LocIter)->GetDefaultSurvivalValue(Tokenizer);
			Token->Delete();
			return SurvivalValue;
		}
	}
	Tokenizer->Error("Economy.dat does not contain a location specified in a monster name!");
	Token->Delete();
	return SurvivalValue;
}

monsterclass CMonsterList::GetDefaultMonsterClass(CTokenizer* Tokenizer)
{
	CToken *Token;
	List<CLocation*>::Iter		LocIter;
	monsterclass MonsterClass = DEFAULT_MONSTER_CLASS;

	Token = Tokenizer->GetToken();		
	if (Token->GetType() != TK_UNDERSCORE)
	{
		Tokenizer->Error("Underscore expected after monster!");
		Token->Delete();
		return MonsterClass;
	}
	Token->Delete();
	Token = Tokenizer->GetToken();
	for (LocIter = m_LocationList.Begin(); LocIter!=m_LocationList.End(); LocIter++)
	{
		if (!(stricmp((*LocIter)->GetLocationName(), Token->GetStringValue())))
		{
			MonsterClass = (*LocIter)->GetDefaultMonsterClass(Tokenizer);
			Token->Delete();
			return MonsterClass;
		}
	}
	Tokenizer->Error("Economy.dat does not contain a location specified in a monster name!");
	Token->Delete();
	return MonsterClass;
}
// ============== CEconomyRegistry functions =====================

void CEconomyRegistry::RegisterMonsters(CTokenizer* Tokenizer)
{
	CToken*	Token;
	Tokenizer->ScanUntilToken(TK_MONSTERS);
	Token = Tokenizer->GetToken();
	if (Token->GetType() == TK_EOF)
	{
		Tokenizer->Error("No Monsters to Register for Economy!");
	}
	else
	{
		m_MonsterList = new CMonsterList();	
		bool success =	m_MonsterList->RegisterLocations(Tokenizer);
		if (!success)
		{
			m_MonsterList->Delete();
		}
	}
	Token->Delete();
}

int CEconomyRegistry::GetDefaultKilledValue(edict_t *ent)
{
	int killedValue = DEFAULT_KILLED_VALUE; 

	CTokenizer* Tokenizer = CTokenizer::Create(TKF_NOUNDERSCOREINIDENTIFIER);

	Tokenizer->SetKeywords(Keywords);
	Tokenizer->SetSymbols(Symbols);
	Tokenizer->AddParseStream((unsigned char *)ent->classname, strlen(ent->classname));

	CToken* Token = Tokenizer->GetToken();
	switch(Token->GetType())
	{
	case TK_MONSTER:
		killedValue = m_MonsterList->GetDefaultKilledValue(Tokenizer);
		break;
	default:
		Tokenizer->Error("Invalid entity category in economy.dat!");
		break;
	}
	Token->Delete();
	Tokenizer->Delete();
	return killedValue;
}

int CEconomyRegistry::GetDefaultSurvivalValue(edict_t *ent)
{
	int SurvivalValue = DEFAULT_SURVIVAL_VALUE; 

	CTokenizer* Tokenizer = CTokenizer::Create(TKF_NOUNDERSCOREINIDENTIFIER);

	Tokenizer->SetKeywords(Keywords);
	Tokenizer->SetSymbols(Symbols);
	Tokenizer->AddParseStream((unsigned char *)ent->classname, strlen(ent->classname));

	CToken* Token = Tokenizer->GetToken();
	switch(Token->GetType())
	{
	case TK_MONSTER:
		SurvivalValue = m_MonsterList->GetDefaultSurvivalValue(Tokenizer);
		break;
	default:
		Tokenizer->Error("Invalid entity category in economy.dat!");
		break;
	}
	Token->Delete();
	Tokenizer->Delete();
	return SurvivalValue;
}

monsterclass CEconomyRegistry::GetDefaultMonsterClass(edict_t *ent)
{
	monsterclass MonsterClass = DEFAULT_MONSTER_CLASS; 

	CTokenizer* Tokenizer = CTokenizer::Create(TKF_NOUNDERSCOREINIDENTIFIER);

	Tokenizer->SetKeywords(Keywords);
	Tokenizer->SetSymbols(Symbols);
	Tokenizer->AddParseStream((unsigned char *)ent->classname, strlen(ent->classname));

	CToken* Token = Tokenizer->GetToken();
	switch(Token->GetType())
	{
	case TK_MONSTER:
		MonsterClass = m_MonsterList->GetDefaultMonsterClass(Tokenizer);
		break;
	default:
		Tokenizer->Error("Invalid entity category in economy.dat!");
		break;
	}
	Token->Delete();
	Tokenizer->Delete();
	return MonsterClass;
}

CEconomyRegistry::CEconomyRegistry()
{
// do parsing of economy.dat here
	byte*		econInfo;
	UINT		flags = 0x00;
	int			flength;

	
	flength = gi.FS_LoadFile ("economy.dat", (void**)&econInfo);
		
	CTokenizer* Tokenizer = CTokenizer::Create();

	if (flength <= 0)
	{
		Tokenizer->Error("Error trying to open file economy.dat in base directory!");
		Tokenizer->Delete();
		return;
	}

	Tokenizer->SetKeywords(Keywords);
	Tokenizer->SetSymbols(Symbols);
	Tokenizer->AddParseStream(econInfo, flength);

	m_MonsterList = NULL;
	RegisterMonsters(Tokenizer);
//	RegisterObjects, etc...

	Tokenizer->Delete();

	gi.FS_FreeFile(econInfo);
}

void CEconomyRegistry::Delete()
{
	if (m_MonsterList)
	{
		m_MonsterList->Delete();
		m_MonsterList = NULL;
	}

	delete this;
}

// ============== CGameStats functions ===========================

CGameStats::CGameStats()
{
	m_EconomyRegistry = new CEconomyRegistry();
}

void CGameStats::Delete ()
{
	if (m_EconomyRegistry)
	{
		m_EconomyRegistry->Delete();
		m_EconomyRegistry = NULL;
	}

	List<CMonsterStats*>::Iter		MSIter;
	
	for (MSIter = m_MonsterStats.Begin(); MSIter!=m_MonsterStats.End(); MSIter++)
	{
		(*MSIter)->Delete();
	}

	m_MonsterStats.Erase(m_MonsterStats.Begin(),m_MonsterStats.End());
	
	delete this;
}

int	CGameStats::GetDefaultKilledValue(edict_t *ent)
{
	int KilledValue = m_EconomyRegistry->GetDefaultKilledValue(ent);
	return KilledValue;
}

int	CGameStats::GetDefaultSurvivalValue(edict_t *ent)
{
	int SurvivalValue = m_EconomyRegistry->GetDefaultSurvivalValue(ent);
	return SurvivalValue;
}

monsterclass CGameStats::GetDefaultMonsterClass(edict_t *ent)
{
	monsterclass MonsterClass = m_EconomyRegistry->GetDefaultMonsterClass(ent);
	return MonsterClass;
}

void CGameStats::AddMonsterToStatsList (edict_t *ent, float killedValue, float survivalValue)
{

	if (killedValue == 12345678)
	{
		killedValue = GetDefaultKilledValue(ent);
	}

	if (survivalValue == 12345678)
	{
		survivalValue = GetDefaultSurvivalValue(ent);
	}

	monsterclass myClass = GetDefaultMonsterClass(ent);


	CMonsterStats *NewMonsterStats = new CMonsterStats(ent, killedValue, survivalValue, myClass);

	m_MonsterStats.PushBack(NewMonsterStats);

}

CMonsterStats* CGameStats::GetMonsterStats(edict_t *ent)
{
	List<CMonsterStats *>::Iter	MSIter;

	for(MSIter = m_MonsterStats.Begin(); MSIter != m_MonsterStats.End(); MSIter++)
	{
		if ((*MSIter)->MatchMonsterID(ent))
		{
			return (*MSIter);
		}
	}
	return NULL;
}

bool CGameStats::CanBuy(edict_t *ent)
{
//	if the ent is a client, he can buy.

	//sorry to butt into this code, but this test was returning some monsters as able to buy...--ss
//	return ((ent-g_edicts-1<MAX_CLIENTS) && (ent>g_edicts));
	return (ent->client != NULL);
}

void CGameStats::BuyModeImpulse(edict_t *ent, int impulse)
{
	if (!CanBuy(ent))
	{
		return;
	}
	CPlayerStats *PlayerStats = GetPlayerStats(ent);
	if (!PlayerStats->GetBuyModeOnOff())
	{
		return;
	}
	switch(impulse)
	{
	case 133:
		ExitBuyMode(ent);
		break;
	case 134:
		if(PlayerStats->GetCashInBank() < 1)
		{
			gi.centerprintf(ent,"not enough cash!\n");
			return;
		}
		PlayerStats->AdjustCashInBank(-1); // -1 is hardcoded price...  hacked in for now - FIXME
		//everyone better have a client at this point...
		ent->client->inv->addWeaponType(SFW_AUTOSHOTGUN, 1);
		break;
	case 135:
		if(PlayerStats->GetCashInBank() < 10)
		{
			gi.centerprintf(ent,"not enough cash!\n");
			return;
		}
		PlayerStats->AdjustCashInBank(-10);
		ent->client->inv->addWeaponType(SFW_MACHINEGUN, 1);
		break;
	case 136:
		if(PlayerStats->GetCashInBank() < 6000)
		{
			gi.centerprintf(ent,"not enough cash!\n");
			return;
		}
		break;
	case 164:
		if(PlayerStats->GetCashInBank() < 10)
		{
			gi.centerprintf(ent,"not enough cash!\n");
			return;
		}
		PlayerStats->AdjustCashInBank(-10);
		ent->client->inv->addAmmoType(AMMO_9MM, 100);
		break;
	case 165:
		if(PlayerStats->GetCashInBank() < 100)
		{
			gi.centerprintf(ent,"not enough cash!\n");
			return;
		}
		PlayerStats->AdjustCashInBank(-100);
//		ent->client->inv->addAmmoType(AMMO_GRENADE, 20);
		break;
	case 166:
		if(PlayerStats->GetCashInBank() < 500)
		{
			gi.centerprintf(ent,"not enough cash!\n");
			return;
		}
		PlayerStats->AdjustCashInBank(-500);
		ent->client->inv->addAmmoType(AMMO_ROCKET, 20);
		break;
	case 194:
		if(PlayerStats->GetCashInBank() < 30)
		{
			gi.centerprintf(ent,"not enough cash!\n");
			return;
		}
		PlayerStats->AdjustCashInBank(-30);
		ent->client->inv->addItem(SFE_FLASHPACK, 3);
		break;
	case 195:
		if(PlayerStats->GetCashInBank() < 40)
		{
			gi.centerprintf(ent,"not enough cash!\n");
			return;
		}
		PlayerStats->AdjustCashInBank(-40);
		ent->client->inv->addItem(SFE_C4, 3);
		break;
	case 196:
		if(PlayerStats->GetCashInBank() < 40)
		{
			gi.centerprintf(ent,"not enough cash!\n");
			return;
		}
		PlayerStats->AdjustCashInBank(-40);
		ent->client->inv->addItem(SFE_NEURAL_GRENADE, 3);
		break;
	case 197:
		if(PlayerStats->GetCashInBank() < 40)
		{
			gi.centerprintf(ent,"not enough cash!\n");
			return;
		}
		break;
	case 198:
		if(PlayerStats->GetCashInBank() < 40)
		{
			gi.centerprintf(ent,"not enough cash!\n");
			return;
		}
		PlayerStats->AdjustCashInBank(-40);
		ent->client->inv->addItem(SFE_LIGHT_GOGGLES, 1);
		break;
	case 199:
		if(PlayerStats->GetCashInBank() < 40)
		{
			gi.centerprintf(ent,"not enough cash!\n");
			return;
		}
		PlayerStats->AdjustCashInBank(-40);
		ent->client->inv->addArmor(100);
		break;
	}
}

void CGameStats::EnterBuyMode(edict_t *ent)
{
	CPlayerStats *PlayerStats;

	if (!CanBuy(ent))
	{
		return;
	}

	PlayerStats = GetPlayerStats(ent);
	PlayerStats->SetBuyModeOnOff (true);


	gi.WriteByte(svc_equip);
	gi.WriteByte(1);//equip code
	gi.WriteByte(3);//how many weapons to offer for sale
	gi.WriteString("jackhammer");//weap name
	gi.WriteLong(1);//weap price
	gi.WriteString("minimi");//weap name
	gi.WriteLong(10);//weap price
	gi.WriteString("m305a1");//weap name
	gi.WriteLong(6000);//weap price
	gi.WriteByte(3);//dif't ammo types for sale
	gi.WriteString("9mm");//weap name
	gi.WriteLong(10);//weap price
	gi.WriteString("grenades");//weap name
	gi.WriteLong(100);//weap price
	gi.WriteString("rockets");//weap name
	gi.WriteLong(500);//weap price
	gi.WriteByte(6);//dif't items for sale
	gi.WriteString("flashpacks");//weap name
	gi.WriteLong(30);//weap price
	gi.WriteString("c4 explosive");//weap name
	gi.WriteLong(40);//weap price
	gi.WriteString("neural grenades");//weap name
	gi.WriteLong(40);//weap price
	gi.WriteString("robot grenades");//weap name
	gi.WriteLong(40);//weap price
	gi.WriteString("light goggles");//weap name
	gi.WriteLong(200);//weap price
	gi.WriteString("armor");//weap name
	gi.WriteLong(40);//weap price
	gi.unicast (ent, true);
}

void CGameStats::ExitBuyMode(edict_t *ent)
{
		if (!CanBuy(ent))
	{
		return;
	}
	
	CPlayerStats *PlayerStats = GetPlayerStats(ent);
		
	PlayerStats->SetBuyModeOnOff(false);
	gi.WriteByte(svc_equip);
	gi.WriteByte(0);
	gi.unicast (ent, true);
}
