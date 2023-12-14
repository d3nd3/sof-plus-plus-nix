#include "g_local.h"
#include "ai_private.h"
#include "m_generic.h"
#include "m_ecto.h"
#include "m_meso.h"
#include "m_female.h"
#include "m_heliai.h"
#include "m_tankai.h"
#include "m_snowcatai.h"
#include "fields.h"
#include "ai_pathfinding.h"
#include "..\strings\singleplr.h"

#if _DEBUG
char *decName;
#endif

static ai_c *AICurrentlyThinking=0;

qboolean IsLevelNamed (const char * thisname)
{
	if (!stricmp(thisname, level.mapname))
	{
		return true;
	}
	char altname[100];
	Com_sprintf(altname, sizeof(altname), "final/%s", thisname);
	if (!stricmp(thisname, level.mapname))
	{
		return true;
	}
	// fixme: kef -- eventually, want this chunk out of here. right now, though, the
	//designers really need it
	if (strstr(level.mapname, thisname))
	{
		return true;
	}

	if (!stricmp(thisname, level_ai_name))
	{	// over-ride
		return true;
	}

	return false;
}

void GetLevelSoundSuffix(char *putSuffixHere)
{
	if (!putSuffixHere)
	{
		return;
	}
	putSuffixHere[0]=0;
	switch(GetInterpCode())
	{
	default:
	case LEVCODE_TUTORIAL:
		strcpy(putSuffixHere, "tut1");
		break;
	case LEVCODE_UNKNOWN:
	case LEVCODE_NYC_SUBWAY:
		strcpy(putSuffixHere, "TSR1");
		break;
	case LEVCODE_NYC_SUBWAY2:
		strcpy(putSuffixHere, "TSR2");
		break;
	case LEVCODE_AFR_TRAIN:
		strcpy(putSuffixHere, "trn1");
		break;
	case LEVCODE_KOS_SEWER:
		strcpy(putSuffixHere, "kos1");
		break;
	case LEVCODE_KOS_BIGGUN:
		strcpy(putSuffixHere, "kos2");
		break;
	case LEVCODE_KOS_HANGAR:
		strcpy(putSuffixHere, "kos3");
		break;
	case LEVCODE_SIB_CANYON:
		strcpy(putSuffixHere, "sib1");
		break;
	case LEVCODE_SIB_BASE:
		strcpy(putSuffixHere, "sib2");
		break;
	case LEVCODE_SIB_PLANT:
		strcpy(putSuffixHere, "sib3");
		break;
	case LEVCODE_IRQ_TOWNA:
		strcpy(putSuffixHere, "irq1a");
		break;
	case LEVCODE_IRQ_BUNKER:
		strcpy(putSuffixHere, "irq2a");
		break;
	case LEVCODE_IRQ_CARGO:
		strcpy(putSuffixHere, "irq3a");
		break;
	case LEVCODE_NYC_WARE:
		strcpy(putSuffixHere, "nyc1");
		break;
	case LEVCODE_NYC_STEAM:
		strcpy(putSuffixHere, "nyc2");
		break;
	case LEVCODE_NYC_STREETS:
		strcpy(putSuffixHere, "nyc3");
		break;
	case LEVCODE_AFR_YARD:
		strcpy(putSuffixHere, "sud1");
		break;
	case LEVCODE_AFR_HOUSE:
		strcpy(putSuffixHere, "sud2");
		break;
	case LEVCODE_AFR_FACT:
		strcpy(putSuffixHere, "sud3");
		break;
	case LEVCODE_TOK_STREET:
		strcpy(putSuffixHere, "jpn1");
		break;
	case LEVCODE_TOK_OFFICE:
		strcpy(putSuffixHere, "jpn2");
		break;
	case LEVCODE_TOK_PENT:
		strcpy(putSuffixHere, "jpn3");
		break;
	case LEVCODE_IRQ_STREETS:
		strcpy(putSuffixHere, "irq1b");
		break;
	case LEVCODE_IRQ_FORT:
		strcpy(putSuffixHere, "irq2b");
		break;
	case LEVCODE_IRQ_OIL:
		strcpy(putSuffixHere, "irq3b");
		break;
	case LEVCODE_CAS_1:
		strcpy(putSuffixHere, "ger1");
		break;
	case LEVCODE_CAS_2:
		strcpy(putSuffixHere, "ger2");
		break;
	case LEVCODE_CAS_3:
		strcpy(putSuffixHere, "ger3");
		break;
	case LEVCODE_CAS_4:
		strcpy(putSuffixHere, "ger4");
		break;
	case LEVCODE_ARM_1:
		strcpy(putSuffixHere, "arm1");
		break;
	case LEVCODE_ARM_2:
		strcpy(putSuffixHere, "arm2");
		break;
	case LEVCODE_ARM_3:
		strcpy(putSuffixHere, "arm3");
		break;
	}
}

lev_interp_code GetInterpCode(void)
{
	if (IsLevelNamed("tut1"))
	{
		return LEVCODE_TUTORIAL;
	}
	if (IsLevelNamed("tsr1"))
	{
		return LEVCODE_NYC_SUBWAY;
	}
	if (IsLevelNamed("tsr2"))
	{
		return LEVCODE_NYC_SUBWAY2;
	}
	if (IsLevelNamed("arm1"))
	{
		return LEVCODE_ARM_1;
	}
	if (IsLevelNamed("arm2"))
	{
		return LEVCODE_ARM_2;
	}
	if (IsLevelNamed("arm3"))
	{
		return LEVCODE_ARM_3;
	}
	if (IsLevelNamed("trn1"))
	{
		return LEVCODE_AFR_TRAIN;
	}
	if (IsLevelNamed("sib2"))
	{
		return LEVCODE_SIB_BASE;
	}
	if (IsLevelNamed("kos1"))
	{
		return LEVCODE_KOS_SEWER;
	}
	if (IsLevelNamed("kos2"))
	{
		return LEVCODE_KOS_BIGGUN;
	}
	if (IsLevelNamed("kos3"))
	{
		return LEVCODE_KOS_HANGAR;
	}
	if (IsLevelNamed("sib1"))
	{
		return LEVCODE_SIB_CANYON;
	}
	if (IsLevelNamed("sib3"))
	{
		return LEVCODE_SIB_PLANT;
	}
	if (IsLevelNamed("irq1a"))
	{
		return LEVCODE_IRQ_TOWNA;
	}
	if (IsLevelNamed("irq2a"))
	{
		return LEVCODE_IRQ_BUNKER;
	}
	if (IsLevelNamed("irq3a"))
	{
		return LEVCODE_IRQ_CARGO;
	}
	if (IsLevelNamed("nyc1"))
	{
		return LEVCODE_NYC_WARE;
	}
	if (IsLevelNamed("nyc2"))
	{
		return LEVCODE_NYC_STEAM;
	}
	if (IsLevelNamed("nyc3"))
	{
		return LEVCODE_NYC_STREETS;
	}
	if (IsLevelNamed("sud1"))
	{
		return LEVCODE_AFR_YARD;
	}
	if (IsLevelNamed("sud2"))
	{
		return LEVCODE_AFR_HOUSE;
	}
	if (IsLevelNamed("sud3"))
	{
		return LEVCODE_AFR_FACT;
	}
	if (IsLevelNamed("jpn1"))
	{
		return LEVCODE_TOK_STREET;
	}
	if (IsLevelNamed("jpn2"))
	{
		return LEVCODE_TOK_OFFICE;
	}
	if (IsLevelNamed("jpn3"))
	{
		return LEVCODE_TOK_PENT;
	}
	if (IsLevelNamed("irq1b"))
	{
		return LEVCODE_IRQ_STREETS;
	}
	if (IsLevelNamed("irq2b"))
	{
		return LEVCODE_IRQ_FORT;
	}
	if (IsLevelNamed("irq3b"))
	{
		return LEVCODE_IRQ_OIL;
	}
	if (IsLevelNamed("ger1"))
	{
		return LEVCODE_CAS_1;
	}
	if (IsLevelNamed("ger2"))
	{
		return LEVCODE_CAS_2;
	}
	if (IsLevelNamed("ger3"))
	{
		return LEVCODE_CAS_3;
	}
	if (IsLevelNamed("ger4"))
	{
		return LEVCODE_CAS_4;
	}
	return LEVCODE_UNKNOWN;
}

//rubbishy save/load stuff first, to make a good first impression :(
ai_public_c *ai_public_c::NewClassForCode(int code)
{
	switch (code)
	{
	default:
	case AI_PUBLIC:
	case AI_BASE:
	case AI_GENERIC:
	case AI_GENERIC_NPC:
	case AI_GENERIC_ENEMY_NPC:
	case AI_GENERIC_DOG:
	case AI_GENERIC_MERC:
		gi.dprintf("ERROR: invalid ai class code: %d\n",code);
		return new ai_c();

	case AI_GENERIC_COW:
		return new cow_ai();
/*	case AI_GENERIC_MERC_GRUNT:
		return new merc_grunt_ai();
	case AI_GENERIC_MERC_DEMO:
		return new merc_demo_ai();
	case AI_GENERIC_MERC_SNIPER:
		return new merc_sniper_ai();
	case AI_GENERIC_MERC_MEDIC:
		return new merc_medic_ai();
	case AI_GENERIC_MERC_HEAVY:
		return new merc_heavy_ai();
	case AI_GENERIC_MERC_TECH:
		return new merc_tech_ai();
*/
	case AI_HELI:
		return new generic_ghoul_heli_ai();
	case AI_TANK:
		return new generic_ghoul_tank_ai();
	case AI_SNOWCAT:
		return new generic_ghoul_snowcat_ai();

	case AI_DOG_HUSKY:
		return new dog_husky_ai();
	case AI_DOG_ROTTWEILER:
		return new dog_rottweiler_ai();
	case AI_BLOWNPART:
		return new blownpart_ai();
	case AI_PLAYERCORPSE:
		return new playercorpse_ai();

	case AI_ECTO_SKINLEADER:
		return new skinleader_ai();
	case AI_ECTO_STOCKBROKER:
		return new stockbroker_ai();
	case AI_ECTO_DCTOURIST:
		return new tourist_ai();
	case AI_ECTO_NYCBUM:
		return new bum_ai();
	case AI_ECTO_COLPRISONER1:
		return new prisoner1_ai();
	case AI_ECTO_COLPRISONER2:
		return new prisoner2_ai();
	case AI_ECTO_IRAQCITIZEN:
		return new irqcitizen_ai();
	case AI_ECTO_UGNCOMMANDER:
		return new commander_ai();
	case AI_ECTO_SERBOFFICER:
		return new serbofficer_ai();
	case AI_ECTO_KOSREFUGEE:
		return new kosrefugee_ai();
	case AI_ECTO_IRAQOFFICER:
		return new iraqofficer_ai();
	case AI_ECTO_FACTORY:
		return new factoryworker_ai();
	case AI_ECTO_CHEMIST:
		return new chemist_ai();
	case AI_ECTO_SIBSUIT:
		return new sibsuit_ai();
	case AI_ECTO_SIBSCIENCE:
		return new sibscientist_ai();
	case AI_ECTO_PUNK2:
		return new zitpunk_ai();

	case AI_MESO_JOHN:
		return new john_ai();
	case AI_MESO_JOHN_SNOW:
		return new john_snow_ai();
	case AI_MESO_JOHN_DESERT:
		return new john_desert_ai();
	case AI_MESO_HAWK:
		return new hawk_ai();
	case AI_MESO_HURTHAWK:
		return new hurthawk_ai();
	case AI_MESO_SAM:
		return new sam_ai();
	case AI_MESO_IRAQWORKER:
		return new irqworker_ai();
	case AI_MESO_NYCPUNK:
		return new nypunk_ai();
	case AI_MESO_AMU:
		return new amu_ai();
	case AI_MESO_RAIDERBOSS:
		return new raiderboss_ai();
	case AI_MESO_RAIDERBOSS2:
		return new raiderboss2_ai();
	case AI_MESO_IRAQSOLDIER1:
		return new irqsoldier1_ai();
	case AI_MESO_IRAQSOLDIER2:
		return new irqsoldier2_ai();
	case AI_MESO_IRAQSOLDIER2B:
		return new irqsoldier2b_ai();
	case AI_MESO_IRAQREPGUARD:
		return new irqrepgd_ai();
	case AI_MESO_IRAQREPGUARDB:
		return new irqrepgdb_ai();
	case AI_MESO_IRAQPOLICE:
		return new irqpolice_ai();
	case AI_MESO_IRAQCOMMANDER:
		return new irqcommander_ai();
	case AI_MESO_IRAQBRUTEA:
		return new irqbrutea_ai();
	case AI_MESO_IRAQBRUTEB:
		return new irqbruteb_ai();
	case AI_MESO_IRAQBODYGUARD:
		return new irqbodyguard_ai();
	case AI_MESO_IRAQROCKET:
		return new irqrocket_ai();
	case AI_MESO_IRAQSADDAM:
		return new irqsaddam_ai();
	case AI_MESO_IRAQMAN2:
		return new irqman2_ai();
	case AI_MESO_UGNSNIPER:
		return new ugsniper_ai();
	case AI_MESO_UGNBRUTE:
		return new ugbrute_ai();
	case AI_MESO_UGNROCKET:
		return new ugrocket_ai();
	case AI_MESO_UGNSOLDIER1:
		return new ugsoldier1_ai();
	case AI_MESO_UGNSOLDIER1B:
		return new ugsoldier1b_ai();
	case AI_MESO_UGNSOLDIER2:
		return new ugsoldier2_ai();
	case AI_MESO_UGNSOLDIER3:
		return new ugsoldier3_ai();
	case AI_MESO_NYCSWATGUY:
		return new nyswatguy_ai();
	case AI_MESO_NYCSWATLEADER:
		return new nyswatleader_ai();
	case AI_MESO_RAIDER1:
		return new raider1_ai();
	case AI_MESO_RAIDER2A:
		return new raider2_ai();
	case AI_MESO_RAIDER2B:
		return new raider2b_ai();
	case AI_MESO_RAIDERBRUTE:
		return new raiderbrute_ai();
	case AI_MESO_RAIDERROCKET:
		return new raiderrocket_ai();
	case AI_MESO_SIBTROOPER2:
		return new sibtrooper2_ai();
	case AI_MESO_SIBCLEANSUIT:
		return new sibcleansuit_ai();
	case AI_MESO_SERBGRUNT1:
		return new serbgrunt1_ai();
	case AI_MESO_SERBGRUNT2:		
		return new serbgrunt2_ai();
	case AI_MESO_SERBGRUNT3:		
		return new serbgrunt3_ai();
	case AI_MESO_SERBSNIPER1A:		
		return new serbsniper1a_ai();
	case AI_MESO_SERBSNIPER1B:		
		return new serbsniper1b_ai();
	case AI_MESO_SERBCOMTROOP:		
		return new serbcomtroop_ai();
	case AI_MESO_SERBBRUTE1A:		
		return new serbbrute1a_ai();
	case AI_MESO_SERBBRUTE1B:		
		return new serbbrute1b_ai();
	case AI_MESO_SERBMECHANIC:		
		return new serbmechanic_ai();
	case AI_MESO_KOSREBEL:		
		return new kosrebel_ai();
	case AI_MESO_KOSKLAGUY:		
		return new kosklaguy_ai();
	case AI_MESO_SKINHEAD1:
		return new skinhead1_ai();
	case AI_MESO_SKINHEAD2A:
		return new skinhead2a_ai();
	case AI_MESO_SKINHEAD2B:
		return new skinhead2b_ai();
	case AI_MESO_SKINHEADBOSS:
		return new skinheadboss_ai();
	case AI_MESO_MALEPOLITICIAN:
		return new malepolitician_ai();
	case AI_MESO_TOKHENCH1:
		return new tokhench1_ai();
	case AI_MESO_TOKHENCH2:
		return new tokhench2_ai();
	case AI_MESO_TOKKILLER:
		return new tokkiller_ai();
	case AI_MESO_TOKNINJA:
		return new tokninja_ai();
	case AI_MESO_TOKBRUTE:
		return new tokbrute_ai();
	case AI_MESO_TOKMALEHOSTAGE:
		return new tokmalehostage_ai();
	case AI_MESO_JAPANSUIT:
		return new japansuit_ai();
	case AI_MESO_SIBTROOPER1A:
		return new sibtrooper1a_ai();
	case AI_MESO_SIBTROOPER1B:
		return new sibtrooper1b_ai();
	case AI_MESO_SIBGUARD:
		return new sibguard_ai();
	case AI_MESO_SIBGUARD3:
		return new sibguard3_ai();
	case AI_MESO_SIBGUARD4:
		return new sibguard4_ai();
	case AI_MESO_SIBMECH:
		return new sibmech_ai();

	case AI_FEMALE_SKINCHICK:
		return new skinchick_ai();
	case AI_FEMALE_NYWOMAN:
		return new nycwoman_ai();
	case AI_FEMALE_SIBGUARD:
		return new fsibguard_ai();
	case AI_FEMALE_SIBSCIENCE:
		return new fsibscience_ai();
	case AI_FEMALE_IRQWOMAN1:
		return new irqwoman1_ai();
	case AI_FEMALE_IRQWOMAN2:
		return new irqwoman2_ai();
	case AI_FEMALE_TOKWOMAN1:
		return new tokwoman1_ai();
	case AI_FEMALE_TOKWOMAN2:
		return new tokwoman2_ai();
	case AI_FEMALE_TOKASSASSIN:
		return new tokassassin_ai();
	case AI_FEMALE_RAIDER:
		return new fraider_ai();
	case AI_FEMALE_TAYLOR:
		return new taylor_ai();
	}
}

ai_public_c::ai_public_c(ai_public_c *orig)
{
	VectorCopy(orig->velocity, velocity);
	VectorCopy(orig->ideal_angles, ideal_angles);
}

void ai_public_c::Evaluate(ai_public_c *orig)
{
	VectorCopy(orig->velocity, velocity);
	VectorCopy(orig->ideal_angles, ideal_angles);
}

// ---------------------------------------------------------------------------------

ai_c::ai_c(ai_c *orig)
: ai_public_c(orig)
{
	int		i;

	lFootPos = orig->lFootPos;
	rFootPos = orig->rFootPos;

	done_firstframe = orig->done_firstframe;
	allowActionChangeTime = orig->allowActionChangeTime;

	for(i = 0; i < BBOX_PRESET_NUMBER; i++)
	{
		VectorCopy(orig->preset_mins[i], preset_mins[i]);
		VectorCopy(orig->preset_maxs[i], preset_maxs[i]);
	}

	current_bbox = orig->current_bbox;
	ground = orig->ground;
	hasHadTarget = orig->hasHadTarget;
	lastTargetTime = orig->lastTargetTime;
	firstTargetTime = orig->firstTargetTime;
	abusedByTeam = orig->abusedByTeam;
	lastNonTargetTime = orig->lastNonTargetTime;

	nextWakeReactTime = orig->nextWakeReactTime;

	nodeData = orig->nodeData;
	*(int *)&ent = GetEdictNum(orig->ent);
	*(int *)&curTarget = GetEdictNum(orig->curTarget);

	current_action.MakeIndex(orig->current_action);
	recycle_action.MakeIndex(orig->recycle_action);

	linkcount = orig->linkcount;
	isActive = orig->isActive;
	isStartleable = orig->isStartleable;
	m_bHostage = orig->m_bHostage;
	m_bSpecialBuddy = orig->m_bSpecialBuddy;
	m_bConcentratingOnPlayer = orig->m_bConcentratingOnPlayer;

	VectorCopy(orig->requestedMoveDir, requestedMoveDir);

	scale = orig->scale;
	sense_mask = orig->sense_mask;
	jumpdistance = orig->jumpdistance;
	jumpheight = orig->jumpheight;
	fallheight = orig->fallheight;
	stepheight = orig->stepheight;
	move_mask  = orig->move_mask;

	worldpriority = orig->worldpriority;
	lastCheckTime = orig->lastCheckTime;
	attentionLevel = orig->attentionLevel;
	watchState = orig->watchState;

	miscFlags = orig->miscFlags;

	lastCorpseCheck = orig->lastCorpseCheck;

	rank = orig->rank;
	
	mySkills = orig->mySkills;

	body.MakeIndex(orig->body);
	MyGhoulObj.MakeIndex(orig->MyGhoulObj);

	VectorCopy(orig->aim_angles, aim_angles);
	VectorCopy(orig->look_angles, look_angles);
}

void ai_c::Evaluate(ai_c *orig)
{
	int		i;

	lFootPos = orig->lFootPos;
	rFootPos = orig->rFootPos;

	done_firstframe = orig->done_firstframe;
	allowActionChangeTime = orig->allowActionChangeTime;

	for(i = 0; i < BBOX_PRESET_NUMBER; i++)
	{
		VectorCopy(orig->preset_mins[i], preset_mins[i]);
		VectorCopy(orig->preset_maxs[i], preset_maxs[i]);
	}

	current_bbox = orig->current_bbox;
	ground = orig->ground;
	hasHadTarget = orig->hasHadTarget;
	lastTargetTime = orig->lastTargetTime;
	firstTargetTime = orig->firstTargetTime;
	abusedByTeam = orig->abusedByTeam;
	lastNonTargetTime = orig->lastNonTargetTime;

	nextWakeReactTime = orig->nextWakeReactTime;

	nodeData = orig->nodeData;

	ent = GetEdictPtr((int)orig->ent);
	curTarget = GetEdictPtr((int)orig->curTarget);

	current_action.MakePtr(*(int *)&orig->current_action);
	recycle_action.MakePtr(*(int *)&orig->recycle_action);

	linkcount = orig->linkcount;
	isActive = orig->isActive;
	isStartleable = orig->isStartleable;
	m_bHostage = orig->m_bHostage;
	m_bSpecialBuddy = orig->m_bSpecialBuddy;
	m_bConcentratingOnPlayer = orig->m_bConcentratingOnPlayer;

	VectorCopy(orig->requestedMoveDir, requestedMoveDir);

	scale = orig->scale;
	sense_mask = orig->sense_mask;
	jumpdistance = orig->jumpdistance;
	jumpheight = orig->jumpheight;
	fallheight = orig->fallheight;
	stepheight = orig->stepheight;
	move_mask  = orig->move_mask;

	worldpriority = orig->worldpriority;
	lastCheckTime = orig->lastCheckTime;
	attentionLevel = orig->attentionLevel;
	watchState = orig->watchState;

	miscFlags = orig->miscFlags;

	lastCorpseCheck = orig->lastCorpseCheck;

	rank = orig->rank;
	mySkills = orig->mySkills;

	body.MakePtr(*(int *)&orig->body);
	MyGhoulObj.MakePtr(*(int *)&orig->MyGhoulObj);

	VectorCopy(orig->aim_angles, aim_angles);
	VectorCopy(orig->look_angles, look_angles);

	ai_public_c::Evaluate(orig);
}

void ai_c::Read()
{
	int				i, count;
	int				*index_list;
	char			loaded[sizeof(ai_c)];

	gi.ReadFromSavegame('AIAI', loaded, AI_SAVE_SIZE);
	Evaluate((ai_c *)loaded);

	gi.ReadFromSavegame('AISL', &count, sizeof(count));
	if(count)
	{
		index_list = new int [count];
		gi.ReadFromSavegame('AISE', index_list, count * sizeof(int));
		for(i = 0; i < count; i++)
		{
			senses.push_back(senseL.GetPointerFromIndex(index_list[i]));
		}
		delete [] index_list;
	}

	gi.ReadFromSavegame('AIDL', &count, sizeof(count));
	if(count)
	{
		index_list = new int [count];
		gi.ReadFromSavegame('AIDE', index_list, count * sizeof(int));
		for(i = 0; i < count; i++)
		{
			decisions.push_back(decisionL.GetPointerFromIndex(index_list[i]));
		}
		delete [] index_list;
	}

	gi.ReadFromSavegame('AIAL', &count, sizeof(count));
	if(count)
	{
		index_list = new int [count];
		gi.ReadFromSavegame('AIAE', index_list, count * sizeof(int));
		for(i = 0; i < count; i++)
		{
			actions.push_back(actionL.GetPointerFromIndex(index_list[i]));
		}
		delete [] index_list;
	}

	if(ent->ai)
	{
		ent->ai = this;
	}
}

void ai_c::Write()
{
	ai_c						*savable;
	list<action_c_ptr>::iterator	ia;
	list<sense_c_ptr>::iterator		is;
	list<decision_c_ptr>::iterator	id;
	int							count, i;
	int							*index_list;

	savable = new ai_c(this);
	gi.AppendToSavegame('AIAI', savable, AI_SAVE_SIZE);
	savable->current_action = NULL;
	savable->recycle_action = NULL;
	savable->body = NULL;
	savable->MyGhoulObj = NULL;
	delete savable;

	count = senses.size();
	gi.AppendToSavegame('AISL', &count, sizeof(count));
	if(count)
	{
		index_list = new int [count];
		for (is = senses.begin(), i = 0; is != senses.end(); is++, i++)
		{
			index_list[i] = (*is).GetIndex();
		}	
		gi.AppendToSavegame('AISE', index_list, count * sizeof(int));
		delete index_list;
	}

	count = decisions.size();
	gi.AppendToSavegame('AIDL', &count, sizeof(count));
	if(count)
	{
		index_list = new int [count];
		for (id = decisions.begin(), i = 0; id != decisions.end(); id++, i++)
		{
			index_list[i] = (*id).GetIndex();
		}
		gi.AppendToSavegame('AIDE', index_list, count * sizeof(int));
		delete index_list;
	}

	count = actions.size();
	gi.AppendToSavegame('AIAL', &count, sizeof(count));
	if(count)
	{
		index_list = new int [count];
		for (ia = actions.begin(), i = 0; ia != actions.end(); ia++, i++)
		{
			index_list[i] = (*ia).GetIndex();
		}
		gi.AppendToSavegame('AIAE', index_list, count * sizeof(int));
		delete index_list;
	}
}

void ai_c::AddBody(edict_t *monster)
{
	if (!monster)
	{
		return;
	}
//	body = new bodyhuman_c();
}

void ai_c::UseMonster(edict_t *user)
{
	if (ent && OnSameTeam(ent, user))
	{
		SetAbusedByTeam(true);
	}
	if (body)
	{
		body->UseMonster(*ent, user);
	}
}

void ai_c::TouchMonster(edict_t *user)
{
	if (body)
	{
		body->TouchMonster(*ent, user);
	}
}

void ai_c::Init(edict_t *monster, char *ghoulname, char *subname)
{
	if(body)
	{
		body->SetOwner(monster);
	}
}

//ACHTUNG! need to have rf_ghoul set on edict by now!
ai_c::ai_c(edict_t *monster)
{
	int		i;

	VectorClear(velocity);
	VectorClear(ideal_angles);

	for(i = 0; i < BBOX_PRESET_NUMBER; i++)
	{
		VectorClear(preset_mins[i]);
		VectorClear(preset_maxs[i]);
	}

	lFootPos = 0;
	rFootPos = 0;
	done_firstframe = false;
	allowActionChangeTime = 0.0F;

	current_bbox = BBOX_PRESET_NUMBER;
	ground = 0.0F;
	hasHadTarget = false;
	lastTargetTime = 0.0F;
	firstTargetTime = 0.0F;
	lastNonTargetTime = 0.0F;
	abusedByTeam = false;

	nextWakeReactTime = -20.0F;

	//constructors?  Who needs constructors?  I'm really regretting making this a plain vanilla struct
	VectorClear(nodeData.curSpot);
	VectorClear(nodeData.goPoint);
	nodeData.curNode = 0;
	nodeData.curRegion = 0;
	nodeData.nextNode = 0;
	nodeData.finalNode = 0;
	nodeData.blocked = 0;
	nodeData.lastNode = 0;
	nodeData.lastNode2 = 0;
	nodeData.lastNode3 = 0;
	nodeData.backingUp = 0;
	nodeData.approaching = 0;
	nodeData.corner1 = -1;
	nodeData.corner2 = -1;
	nodeData.corner3 = -1;
	nodeData.corner4 = -1;
	VectorClear(nodeData.lastDir);
	nodeData.lastDirSetTime = 0;
	// End of nodeData setup
	
	ent = monster;
	curTarget = NULL;
	current_action = NULL;
	recycle_action = NULL;
	linkcount = 0;
	isActive = false;
	isStartleable = true;
	m_bHostage = false;
	m_bSpecialBuddy = false;
	m_bConcentratingOnPlayer = false;
	VectorClear(requestedMoveDir);
	scale = 1.0F;
	sense_mask = alarm_mask|sight_mask|sound_mask;
	jumpdistance = 100.0F;
	jumpheight = 50.0F;
	fallheight = 1000000.0F;
	stepheight = 24.0F;
	move_mask = jump_movemask|fall_movemask|step_movemask;
	worldpriority = PRIORITY_LOW;
	lastCheckTime = 0;
	attentionLevel = ATTENTION_IDLE;
	watchState = 0;	
	lastCorpseCheck = 0;
	miscFlags = 0;
	rank = 0;//grunt
	body = NULL;
	MyGhoulObj = NULL;

	VectorClear(aim_angles);
	if (monster)
	{
		SetLookAngles(monster->s.angles);
	}
	//eek, no ent yet, set me to trash
	else
	{
		SetLookVector(vec3_up);
	}
}

ai_c::~ai_c(void)
{
	list<action_c_ptr>::iterator	ia;
	list<sense_c_ptr>::iterator		is;
	list<decision_c_ptr>::iterator	id;

	if (body)
	{
		body.Destroy();
	}

	// lists will clean up their nodes during their destruction
	// huh? should have list-eating separated out
	for (is=senses.begin();is != senses.end();is++)
	{
		(*is).Destroy();
	}

	for (ia=actions.begin();ia!=actions.end();ia++)
	{
		(*ia).Destroy();
	}

	for (id=decisions.begin();id!=decisions.end();id++)
	{
		(*id).Destroy();
	}

	if (current_action)
	{
		current_action.Destroy();
	}

	if (recycle_action)
	{
		recycle_action.Destroy();
	}
}

float ai_c::GetAimConeDegrees(edict_t &monster, bbox_preset bbox)
{
	//if i'm crouching or prone, lemme aim wider than if i'm standing.
	if (bbox==BBOX_PRESET_CROUCH||bbox==BBOX_PRESET_PRONE)
	{
		return 45.0;
	}
	return 20.0;
}

float ai_c::GetAimDeviationDegrees(edict_t &monster)
{
	CRadiusContent rad(monster.s.origin, 200, 1, 0);
	//got pals around, be careful.
	if (rad.getNumFound())
	{
		return 5.0/(float)rad.getNumFound();
	}
	//nobody's around, can be sloppy
	return 20.0;
}

qboolean ai_c::IsFirstFrameTime(void)
{
	return !done_firstframe;
}

void ai_c::FirstFrame(edict_t *monster)
{
	done_firstframe = true;
}

bool ai_c::AmIAsGoodAsDead()
{
	bodyorganic_c *body = (bodyorganic_c*)GetBody();

	if (body)
	{
		return body->IsNextShotGonnaKillMe();
	}
	return false;
}

qboolean ai_c::IsArmed(edict_t &monster)
{
	if (body)
	{
		return (body->GetBestWeapon(monster)!=ATK_NOTHING); 
	}
	return false;
}

qboolean ai_c::SafeToRemove(edict_t &monster)
{
	list<action_c_ptr>::iterator	ia;
	list<sense_c_ptr>::iterator		is;
	list<decision_c_ptr>::iterator	id;

	// go thru all lists, make sure everything is good to go

	// go thru senses, make sure everything is good to go
	for (is=senses.begin();is != senses.end();is++)
	{
		if (!(*is)->SafeToRemove(monster))
		{
			return false;
		}
	}

	// go thru actions, make sure everything is good to go
	for (ia=actions.begin();ia!=actions.end();ia++)
	{
		if (!(*ia)->SafeToRemove(monster))
		{
			return false;
		}
	}

	// go thru decisions, make sure everything is good to go
	for (id=decisions.begin();id!=decisions.end();id++)
	{
		if (!(*id)->SafeToRemove(monster))
		{
			return false;
		}
	}

	// check my current action, make sure everything is good to go
	if (current_action)
	{
		if (!current_action->SafeToRemove(monster))
		{
			return false;
		}
	}

	//pay no attention to recycle_action (it's not really there)

	return true;//everything returned ok for removal
}

void ai_c::NewSense(sense_c *new_sense, edict_t *monster)
{
	if (monster->deadflag == DEAD_DEAD)
	{
		//just in case the new sense is in the pointer class list already:
		sense_c_ptr bad_sense = new_sense;
		bad_sense.Destroy();
	}
	else
	{
		assert(senses.size()<30);
		senses.push_back(new_sense);
	}
}

void ai_c::NewAction(action_c *new_action, edict_t *monster, qboolean activatenow)
{
	if (activatenow && !isActive)
	{
		Activate(*monster);
	}
/*	if (monster->deadflag == DEAD_DEAD)
	{
		//just in case the new action is in the pointer class list already:
		action_c_ptr bad_action = new_action;
		bad_action.Destroy();
	}
	else
	{
*/		actions.push_back(new_action);
//	}
}

void ai_c::NewNextAction(action_c *new_action, edict_t *monster, qboolean activatenow)
{
	if (activatenow && !isActive)
	{
		Activate(*monster);
	}
/*	if (monster->deadflag == DEAD_DEAD)
	{
		//just in case the new action is in the pointer class list already:
		action_c_ptr bad_action = new_action;
		bad_action.Destroy();
	}
	else
	{
*/		actions.push_front(new_action);
//	}
}


void ai_c::NewDecision(decision_c *new_decision, edict_t *monster)
{
	//eek!script stuff expects this decision to stick around!  should be safe to allow it--death action shouldn't ever finish!
//	if (monster->deadflag == DEAD_DEAD)
//	{
//		//just in case the new decision is in the pointer class list already:
//		decision_c_ptr bad_decision = new_decision;
//		bad_decision.Destroy();
//	}
//	else
//	{
		decisions.push_back(new_decision);
//	}
}

void ai_c::NewCurrentAction(action_c *new_action, edict_t &monster)
{
	if (current_action)
	{
		current_action->SetInterrupted(true);
		actions.push_front(current_action);
	}

	current_action = new_action;
	current_action->BeginAction(*this,monster);
}

void ai_c::NewDeathArmorAction(edict_t &monster)
{
		NewCurrentAction(	DeathArmorAction(	NULL, NULL, NULL,
												monster, NULL, NULL,
												0, vec3_origin),
							monster);
}

void ai_c::NextAction(edict_t &monster)
{
	list<action_c_ptr>::iterator	ia;

	if (current_action)
	{
		//save whatever i was doing to be recycled (cuts down on allocation costs)
		if (recycle_action)
		{
			//only save off one action per guy for recycling
			recycle_action.Destroy();
		}
		recycle_action=current_action;
		current_action=NULL;
	}

	if (actions.size())
	{
		ia = actions.begin();
		current_action = *ia;
		current_action->BeginAction(*this,monster);
		actions.erase(ia);
	}
}

void ai_c::EvaluateDecisions(edict_t &monster)
{
	list<decision_c_ptr>::iterator	id, pick, t_id;
	int						priority, max_priority;
	bool					found;
	int						i,dec_siz;
	

#ifdef _DEBUG
	char *chosenDec;
#endif

	dec_siz=decisions.size();

	if (dec_siz==0)
	{
		return;
	}

	max_priority = 0;
	found = false;
	for (id=decisions.begin(), i=dec_siz;i>0;i--)
	{
		if((*id)->Consider(*this, monster))//decision has let me know it's used up, trash it.
		{
			t_id = id;
			id--;
			(*t_id).Destroy();
			decisions.erase(t_id);
		}
		else
		{
			priority = (*id)->Priority();

			if (priority > max_priority)
			{
				pick = id;
				max_priority = priority;
				found = true;
#ifdef _DEBUG
				chosenDec = decName;// global string pointer that has the name of the chosen decision
									// I have this in here because I want to see what guys are doing when they
									// are standing around looking less than clever...  They shouldn't do that :)
#endif
			}
		}
		id++;
	}

	if (found)
	{
#ifdef _DEBUG
		if(aidec_show->value)
		{
			Enemy_Printf(this, "I have chosen to do a %s; priority of %d; my priority is %s\n", chosenDec, max_priority, (worldpriority)?"low":"high");
//			gi.dprintf("I have chosen to do a %s; priority of %d\n", chosenDec, max_priority);
		}
#endif
		(*pick)->Perform(*this, monster);

		NextAction(monster);

	}
}

void ai_c::EvaluateSenses(edict_t &monster)
{
	list<sense_c_ptr>::iterator	is,t_is;
	sense_c *ptr = NULL;
	int i;

	sensedEntInfo_t temp;

	if((worldpriority == PRIORITY_LOW)&&(lastCheckTime > level.time - 2.0))
	{	// if yer not important, yer not allowed to look around much
		return;
	}

	for (is=senses.begin(),i=senses.size();i>0;i--)
	{
		if ((*is)->Evaluate(sense_mask,*this, monster))//sense resolved, ditch it
		{
			(*is)->UpdateSensedClient(sense_mask, temp);
			if (temp.ent && (body->GetBestWeapon(monster) != ATK_NOTHING))
			{
				int n = 1;
				ptr = (*is);
			}
			t_is = is;
			(*is).Destroy();
			is++;
			senses.erase(t_is);
		}
		else
		{
			(*is)->UpdateSensedClient(sense_mask, temp);
			if (temp.ent && (body->GetBestWeapon(monster) != ATK_NOTHING))
			{
				int n = 1;
				ptr = (*is);
			}
			is++;
		}
	}

	lastCheckTime = level.time + 1.0;

	sensedEntInfo_t	sensed_client, sensed_monster;
	GetSensedClientInfo(smask_all, sensed_client);
	GetSensedMonsterInfo(smask_all, sensed_monster);

	//as the sensed_client always gets preference
	if (sensed_client.ent && sensed_client.ent->health>0)
	{
		if(sensed_client.time < level.time - 1.0)
		{
			firstTargetTime = level.time + gi.flrand(-.2,.2);
		}
	}

}

void ai_c::MuteSenses(unsigned mask, float degree, smute_recovery recovery_code, float recovery_time)
{
	list<sense_c_ptr>::iterator	is;

	for (is=senses.begin();is!=senses.end();is++)
	{
		(*is)->Mute(mask,degree,recovery_code,recovery_time);
	}
}

void ai_c::RegisterSenseEvent(unsigned mask, vec3_t event_origin, float event_time, edict_t *event_edict, ai_sensetype_e event_code)
{
	list<sense_c_ptr>::iterator	is;

	for (is=senses.begin();is!=senses.end();is++)
	{
		assert(event_edict);
		(*is)->RegisterSenseEvent(mask,event_origin,event_time,event_edict,event_code);
	}
}

float ai_c::GetSenseMutedLevel(unsigned mask)
{
	list<sense_c_ptr>::iterator	is;
	float cur, best=0;

	for (is=senses.begin();is!=senses.end();is++)
	{
		cur = (*is)->GetMutedLevel(mask);
		if (cur>best)
		{
			best = cur;
		}
	}
	return best;
}

void ai_c::GetSensedClientInfo(unsigned mask, sensedEntInfo_t &sensedEnt)
{
	list<sense_c_ptr>::iterator	is;

	sensedEnt.ent=NULL;
	VectorClear(sensedEnt.pos);
	sensedEnt.time=-99999999;
	sensedEnt.senseType=AI_SENSETYPE_UNKNOWN;
/*
	if(getTarget())
	{	
		if(getTarget() == level.sight_client)
		{
			//just to see
			sensedEnt.time = level.time - .1;
			sensedEnt.ent = level.sight_client;
			lastTargetTime = level.time - .1;
			VectorCopy(level.sight_client->s.origin, sensedEnt.pos);
			return;
		}
	}
*/
	for (is=senses.begin();is!=senses.end();is++)
	{
		(*is)->UpdateSensedClient(mask,sensedEnt);

		if (sensedEnt.senseType==AI_SENSETYPE_SOUND_WAKEUP)
		{
			nextWakeReactTime=sensedEnt.time+12.0F;
			sensedEnt.senseType=AI_SENSETYPE_SOUND_INVESTIGATE;
			(*is)->ChangeClientSenseType(AI_SENSETYPE_SOUND_INVESTIGATE);
		}
	}

}

void ai_c::GetSensedMonsterInfo(unsigned mask, sensedEntInfo_t &sensedEnt)
{
	list<sense_c_ptr>::iterator	is;

	sensedEnt.ent=NULL;
	VectorClear(sensedEnt.pos);
	sensedEnt.time=-99999999;

	for (is=senses.begin();is!=senses.end();is++)
	{
		(*is)->UpdateSensedMonster(mask,sensedEnt);
	}
}

qboolean ai_c::getTargetPos(vec3_t putPosHere)
{
	sensedEntInfo_t sensedEnt;

	if (curTarget)
	{
		if (curTarget->client)
		{
			GetSensedClientInfo(smask_all, sensedEnt);
		}
		else
		{
			GetSensedMonsterInfo(smask_all, sensedEnt);
		}
		if (sensedEnt.ent==curTarget)
		{
			VectorCopy(sensedEnt.pos, putPosHere);
			return true;
		}
	}

	//couldn't find the guy...
//	VectorClear(putPosHere);
	VectorCopy(ent->s.origin, putPosHere);
	return false;
}

// kef
#include "m_heliai.h"

void ai_c::Activate(edict_t &monster)
{
	if (!isActive)
	{
		isActive=true;
		monster.movetype = MOVETYPE_STEP;
		monster.spawnflags&=~SPAWNFLAG_TRIGGER_SPAWN;
		monster.solid = SOLID_BBOX;
		attentionLevel = ATTENTION_ALERT;

		//grr, stupid dekker...
		if (GetClassCode()==AI_MESO_RAIDERBOSS2)
		{
			trace_t tr;
			gi.trace(monster.s.origin, monster.mins, monster.maxs, monster.s.origin, &monster, MASK_MONSTERSOLID, &tr);
			if (tr.allsolid)
			{
				monster.s.origin[0]+=32.0;
				gi.trace(monster.s.origin, monster.mins, monster.maxs, monster.s.origin, &monster, MASK_MONSTERSOLID, &tr);
				if (tr.allsolid)
				{
					monster.s.origin[0]-=64.0;
				}
			}
		}

		gi.linkentity(&monster);
	}
}

void ai_c::Think(edict_t &monster)
{
//	vec3_t testpos;
//	VectorCopy(monster.s.origin,testpos);
//	testpos[2]+=monster.maxs[2];
//	FX_MakeDustPuff(testpos);
	monster.nextthink = level.time + FRAMETIME;

	if(BeingWatched())
	{
		int asdf = 9;
	}

	// if this joker is fleeing and he has the infamous NO_WOUND spawnflag, make
	//sure he fires his killtarget
	if (monster.spawnflags & SPAWNFLAG_NO_WOUND)
	{	
		bool		bFleeing = false;
		decision_c	*curDecision = current_action?current_action->GetOwnerDecision():NULL;

		bFleeing = (curDecision && (curDecision->GetClassCode() == RETREAT_DECISION));
		if (bFleeing)
		{
			monster.spawnflags &= ~SPAWNFLAG_NO_WOUND;
			edict_t *t = NULL;
			if (t = G_Find (t, FOFS(targetname), monster.killtarget))
			{
				t->use(t, &monster, &monster);
				monster.killtarget = NULL;
			}
		}
	}
	if ( (monster.spawnflags & SPAWNFLAG_FACE_PLAYER) &&
		 (1 == monster.wait) )
	{	// if wait == 1, face the player
		vec3_t vPlayer, vTemp;
		VectorCopy(g_edicts[1].s.origin, vPlayer);
		VectorSubtract(vPlayer, monster.s.origin, vTemp);
		vTemp[2] = 0;
		vectoangles(vTemp, monster.s.angles);
	}


	// kef
	if (game.cinematicfreeze)
	{	// if we're in a cinematic, only let appropriate people think

		// using 255 here as a flag. poor, I know, but if you hate it that much, change it yourself.
		if ( (monster.count != 255) &&
			 (HasHadTarget() || (monster.flags & FL_SPAWNED_IN)) )
		{	// this guy is not being used by the current cinematic so he doesn't get to think 
			monster.flags |= FL_CINEMATIC_CULL;
			monster.nextthink = -(monster.nextthink - level.time); 
			// neither does he get to interact with the world
			monster.solid = SOLID_NOT;
			// he shouldn't even be visible
			if (monster.ghoulInst)
			{
				monster.ghoulInst->SetOnOff(false, level.time);
			}
			return;
		}
	}


	if(ai_dumb->value)
	{
		if(monster.spawnflags & SPAWNFLAG_BLIND)
		{
			fxRunner.exec("cantsee", &monster);
		}
		if(monster.spawnflags & SPAWNFLAG_DEAF)
		{
			fxRunner.exec("canthear", &monster);
		}
		/*if(monster.spawnflags & SPAWNFLAG_HOLD_POSITION)
		{
			fxRunner.exec("environ/firelarge2", &monster);
		}*/
	}


//	debug_drawbox(&monster, NULL, NULL, NULL, 0);
/*	if(worldpriority == PRIORITY_LOW)
	{
		monster.ghoulInst->TurnMatrixCallBacksOff();
	}
	else
	{
		monster.ghoulInst->TurnMatrixCallBacksOn(level.time);
	}*/ // this doesn't seem to work so well :(

	if (ai_freeze&&ai_freeze->value)
	{
		return;
	}

	if(ai_goretest&&ai_goretest->value&&!(monster.spawnflags&SPAWNFLAG_TRIGGER_SPAWN))
	{
		Activate(monster);
	}

	if (!isActive)
	{
		if (monster.spawnflags&SPAWNFLAG_START_ACTIVE)
		{
			Activate(monster);
		}
		else
		{
			if((lastCorpseCheck < level.time)  && !(monster.spawnflags&SPAWNFLAG_TRIGGER_SPAWN))
			{
				if(CheckForCorpseActivate(monster))
				{
					Activate(monster);
				}

				lastCorpseCheck = level.time + 1.0;
			}

			if(!isActive)
			{
				vec3_t tdist;
				if (level.sight_client && !(monster.spawnflags&SPAWNFLAG_TRIGGER_SPAWN))
				{
					VectorSubtract(monster.s.origin, level.sight_client->s.origin, tdist);
					if (VectorLengthSquared(tdist)<640000)
					{
						Activate(monster);
					}
					else
					{
						return;
					}
				}
				else
				{
					return;
				}
			}
		}
	}

	EvaluateSenses(monster);

	if (!current_action)
	{	// do some thinking - this is not correct
		if (actions.size())
		{
			NextAction(monster);
		}
		else // aren't any more actions
		{
			EvaluateDecisions(monster);
		}
	}

	if (current_action)
	{
		if ((nextWakeReactTime >= level.time-1.0F) && (nextWakeReactTime < level.time)
			&& body)
		{
//			body->VoiceSound("react", monster, 0);
			nextWakeReactTime=level.time-20.0F;
		}

		AICurrentlyThinking=this;
		int val = current_action->Think(*this, monster);
		AICurrentlyThinking=0;

		if (val)
		{ 
			NextAction(monster);
		}
	}

	if (monster.linkcount != linkcount)
	{
		linkcount = monster.linkcount;
		gmonster.CheckGround (&monster);
	}
	gmonster.CatagorizePosition (&monster);
	gmonster.WorldEffects (&monster);//leaving this in for drowning, lava damage, etc., but it should prolly be handled in ai class somewhere

	//camera stuff for debugging
	watchState = false;

	if ((worldpriority == PRIORITY_LOW)&&(monster.flags & FL_SPAWNED_IN))
	{	// guys who are spawned in but too far away are silly and worth nothing
		Escape(monster);
	}

	if (attentionLevel == ATTENTION_ESCAPED)
	{	// yeah - makes no sense, huh.  Fun.
		Escape(monster);
	}

	//if monster is dead, consider removing it
	if (monster.health <= 0)
	{	// kef -- moved to body_human_c
		//DealWithArmor(monster, GetBody());
		//get ai to poll actions, decisions, & senses to check if removal is ok
		if (SafeToRemove(monster))
		{	
			// if we do this too quickly we may wind up removing him before and bolted-on knives
			//can be turned into knife pickups. so if he still has any sticking out of him, 
			//free them now.
			if (GetBody())
			{
				((bodyorganic_c*)GetBody())->FreeKnives(monster);
			}
			G_FreeEdict (&monster);
			return;
		}
	}
}



qboolean ai_c::Damage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb)
{
	// fixme: centralize skill level stuff
	int			take;

	// friendly fire avoidance--allowing possibility of non-clients on your team
	// if enabled you can't hurt teammates (but you can hurt yourself)
	// knockback still occurs
	if (&monster != attacker)
	{
		//fixme: come up with method of determining whether to take damage or not
		if (OnSameTeam (&monster, attacker))
		{
			if ((attacker->client) && (mod != MOD_TELEFRAG))
			{
				SetAbusedByTeam(true);
			}
			else
			{

//			if(dm->dmRule_NO_FRIENDLY_FIRE())
//			{
//				damage = 0; dk disabled this because we want friendly fire on for demo
//			}
//			else
//			{
//				mod |= MOD_FRIENDLY_FIRE;
//			}
			}
		}
	}

	//HACKITY HACK!
	if (mod == MOD_TELEFRAG)
	{
		return false;
	}

//	if (!attacker)// || !attacker->client)
//	{
//		damage = 0;
//	}

	VectorNormalize(dir);

// bonus damage for suprising a monster
	if (!(dflags & DAMAGE_RADIUS) && (attacker->client) && (!monster.enemy) && (monster.health > 0))
	{
		damage *= 2;
	}

	if (monster.flags & FL_NO_KNOCKBACK)
	{
		knockback = 0;
	}


	take = damage;

//	if (take)
//	{
		//throw up visible signs o' damage-gorezores, bloodspray, etc.
		//notice that i've been shot, here
		RegisterSenseEvent(smask_all, attacker->s.origin, level.time, attacker, AI_SENSETYPE_SOUND_WEAPON);
		if (body && take)
		{
			hasHadTarget=true;
			take=damage=body->ShowDamage(monster, inflictor, attacker, dir, point, origin, damage, knockback, dflags, mod, penetrate, absorb);
		}
//	}

		//this used to be only if i took damage; now, knockback applies regardless.--ss
// do the damage
		// figure momentum add
		if (!(dflags & DAMAGE_NO_KNOCKBACK))
		{
			if ((knockback) && 
				(monster.movetype != MOVETYPE_NONE) &&
				(monster.movetype != MOVETYPE_BOUNCE) &&
				(monster.movetype != MOVETYPE_PUSH) &&
				(monster.movetype != MOVETYPE_NOCLIP) &&
				(monster.movetype != MOVETYPE_STOP))
			{
				vec3_t	kvel;
				float	mass;
				vec3_t	kbdir;

				if (monster.mass < 50)
				{
					mass = 50;
				}
				else
				{
					mass = monster.mass;
				}

//				Bit of Knockback Fakery here.
//				dir[2] += 0.3; // might this work?

				VectorCopy(dir, kbdir);		// NOTE:  WE MUST DO THIS TO AVOID MODIFYING THE VECTOR WE PASSED HERE BY REFERENCE.
				if (dflags&DAMAGE_ALL_KNOCKBACK)
				{
					kbdir[2] *= 0.2;
				}
				else
				{
					kbdir[2] += 0.3; // might this work?
				}
				VectorNormalize(kbdir);
				VectorScale (kbdir, 500.0 * ((float)knockback) / mass, kvel);

//				Com_Printf("dir: X: %f Y: %f Z: %f \n", dir[0], dir[1], dir[2]);
//				Com_Printf("Knockback: X: %f Y: %f Z: %f \n", kvel[0], kvel[1], kvel[2]);

				VectorAdd (monster.velocity, kvel, monster.velocity);
// dk			monster.groundentity = NULL;
			}
		}

	if (take)
	{
		if (attacker && attacker->client && monster.health > 0 && IsHostage())
		{
			gi.SP_Print(attacker, SINGLEPLR_HOSTAGE_HIT);
		}

		monster.health = monster.health - take;

	}

	//even if i didn't do damage this time, count this as a new kill time if guy is dead--so guys with armor will keep twitching when they're dead --ss
	if (monster.health <= 0)
	{

		monster.flags |= FL_NO_KNOCKBACK;
		Die (monster, inflictor, attacker, take, point, dflags);
		return true;
	}

	if(body->GetArmorCode() != ARMOR_FULL || take)
	{	//armor guys should not wince unless they are hurt
		Pain (monster, attacker, point, knockback, take);
	}

	if (take)
	{
		return true;
	}

	return false;//no hit!
}

void	ai_c::Die(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags)
{
	if (AICurrentlyThinking==this)
	{
		// cannot die while thinking....::thinks are on the stack and still executing.
 		monster.health = 1;
		return;
	}
	if (monster.health > 0)
	{
		monster.health = 0;
	}

	if (monster.health < -999)
	{
		monster.health = -999;
	}


	PlayerNoise(attacker, monster.s.origin, AI_SENSETYPE_SOUND_INVESTIGATE, &monster, 600, nodeData.curNode, dflags & (DT_STEALTHY), 1);

	if(level.sight_client)
	{
		if(!OnSameTeam(level.sight_client, &monster))
		{
			if(monster.deadflag != DEAD_DEAD)
			{
				gmonster.SpookEnemies(monster.s.origin, 128, SPOOK_DEAD_FRIEND);
			}
		}
	}

	if(monster.deadflag != DEAD_DEAD)
	{
		if(!game.cinematicfreeze)
		{
			if(!OnSameTeam(level.sight_client, &monster))
			{
				level.guysKilled++;
				level.cashEarned += mySkills.getCashValue() * game.playerSkills.getMoney();
			}
			else
			{
				level.friendliesKilled++;
				//level.cashEarned -= mySkills.getCashValue() * 10 * game.playerSkills.getMoney();
			}
		}
	}

	if (monster.killtarget && (monster.deadflag != DEAD_DEAD))
	{
		qboolean useThatTarget=true;

		//if killfacing set, i might Not want to use that target!
		if (monster.killfacing)
		{
			vec3_t	forward, tokillpoint;
			edict_t	*target = NULL;
			useThatTarget=false;

			AngleVectors(monster.s.angles, forward, NULL, NULL);
			target = G_Find(target, FOFS(targetname), monster.killfacing);
			if (target)
			{
				VectorSubtract(target->s.origin, monster.s.origin, tokillpoint);
				tokillpoint[2] = 0;
				forward[2] = 0;
				VectorNormalize(tokillpoint);
				VectorNormalize(forward);
				if (DotProduct(tokillpoint, forward) > .96) // within 15 degrees either way
				{
					useThatTarget=true;
				}
			}
			else
			{
				gi.dprintf("Couldn't find killfacing ent %s--will use killtarget anyway!\n",monster.killfacing);
			}
		}

		if (useThatTarget)
		{
			edict_t *t = NULL;
			qboolean	foundScript=false;
			qboolean	foundTarget=false;
			while ((t = G_Find (t, FOFS(targetname), monster.killtarget)))
			{
				foundTarget=true;
				t->use(t, &monster, &monster);
				if (!strcmp(t->classname, "script_runner") && (t->spawnflags&1))
				{
					foundScript=true;
					gi.dprintf("%s is running a WILL_KILL_USER script, and won't die right now!\n", monster.classname);
					monster.health=monster.max_health;

					//clear out all actions, so the script stuff happens immediately
					list<action_c_ptr>::iterator	ia;

					for (ia=actions.begin();ia!=actions.end();ia++)
					{
						(*ia).Destroy();
					}
					while (actions.size())
					{
						actions.pop_front();
					}
					if (current_action)
					{
						current_action.Destroy();
					}
					if (recycle_action)
					{
						recycle_action.Destroy();
					}
				}
			}
			if (foundScript)
			{
				VectorClear(monster.velocity);
				return;
			}
			else if (!foundTarget)
			{
				gi.dprintf("Couldn't find killtarget %s!\n",monster.killtarget);
			}
		}
	}

/*	if(!(monster.svflags & SVF_DEADMONSTER))
	{
		if(gi.irand(0, 3) == 1)
		{	// drop a bit of armor every so often
			//item_equip_kevlar
			edict_t *armor = MakeItem("item_equip_kevlar", monster.s.origin);
			armor->count = gi.irand(10, 40);//hmm... hardcodedness is mine - we've really got to improve this end up stuff
			
			armor->movetype = MOVETYPE_DAN;
			armor->friction = 1.0;
			armor->gravity = 1.0;
			armor->airresistance = 0.0;
			armor->bouyancy = .7;
			armor->elasticity = 0.3;

			armor->velocity[2] = 600;// hmm...
		}
	}*/

//	monster.solid = SOLID_BBOX;
	monster.solid = SOLID_CORPSE;

	monster.clipmask = CONTENTS_DEADMONSTER|MASK_DEADSOLID;
	monster.svflags |= SVF_DEADMONSTER;
	monster.movetype = MOVETYPE_STEP; // in case a guy is killed while NO_CLIP in scripting
	monster.friction = .5;

	monster.enemy = attacker;//for awarding credit?
		// CHECKME possible issues:
		// If monster is killed by barrel, should we chain to find who was the barrel's killer?
		// what about being clever and using architecture to kill monsters (like 16 ton weights)

	if (GetBody())
	{
		if (!GetBody()->GetInitialKilledTime())
		{
			GetBody()->SetInitialKilledTime(level.time);
		}
		GetBody()->SetLastKilledTime(level.time);
		GetBody()->SetLastDFlags(dflags);
	}
	if ( (monster.deadflag != DEAD_DEAD) || ((monster.spawnflags & SPAWNFLAG_ARMOR_PICKUP) && (current_action && current_action->GetClassCode()== DEATHARMOR_ACTION)))
	{
		vec3_t					facedir;
		list<action_c_ptr>::iterator	ia;
		list<sense_c_ptr>::iterator		is;
		list<decision_c_ptr>::iterator	id;


		// kef -- if this chunk of code gets called more than once per entity, that's bad
		if ( (strstr(monster.classname, "swat")) &&
			 (attacker->client) &&
			 !(monster.spawnflags & SPAWNFLAG_ARMOR_PICKUP) )
		{ // if the player kills a cop, game over.
			if (level.maxDeadHostages==-1)
			{
				level.maxDeadHostages=0;
			}
			level.deadHostages=level.maxDeadHostages+1;
		}
		else if (IsHostage())
		{	// keep track of number of dead hostages in this level
			level.deadHostages++;
		}
		else if (IsSpecialBuddy()&&attacker->client)
		{
			//yick.  well, it's an easy way to fail.
			if (level.maxDeadHostages==-1)
			{
				level.maxDeadHostages=0;
			}
			level.deadHostages=level.maxDeadHostages+1;
		}

/*		if (game.GameStats->CanBuy(attacker))
		{
			//changed this to be conditional so it wouldn't crash --ss
			if(CMonsterStats	*MonsterStats = game.GameStats->GetMonsterStats(&monster))
			{
				GetPlayerStats(attacker)->AdjustCashToBeAwarded(MonsterStats->GetKilledValue());
			}
			else
			{
				gi.dprintf("Error: no stats for monster!\n");
			}
		// this stuff gets put into bank account at "end" of mission
		}*/

//fixme: do need the npc check, but do it a good way
//		if (!(monster.monsterinfo.aiflags & AI_GOOD_GUY))
//		{
			level.killed_monsters++;
/*			if (coop->value && attacker->client)
			{
				attacker->client->resp.score++;
			}*/
//		}

		// huh? should have list-eating separated out?
		for (is=senses.begin();is != senses.end();is++)
		{
			(*is).Destroy();
		}

		for (ia=actions.begin();ia!=actions.end();ia++)
		{
			(*ia).Destroy();
		}

		for (id=decisions.begin();id!=decisions.end();id++)
		{
			(*id).Destroy();
		}


		//now ditch nodes
		while (senses.size())
		{
			senses.pop_front();
		}

		while (actions.size())
		{
			actions.pop_front();
		}

		while (decisions.size())
		{
			decisions.pop_front();
		}

		if (current_action && !(monster.spawnflags & SPAWNFLAG_ARMOR_PICKUP) )
		{
			current_action.Destroy();
		}

		if (recycle_action)
		{
			recycle_action.Destroy();
		}

		PickDeathAnim(monster, inflictor, attacker, damage, point, dflags);

		monster.touch = NULL;//do we need this here?
		monster.deadflag = DEAD_DEAD;

		// kef -- cut off any sounds he's making
		gi.sound(&monster, CHAN_VOICE, 0 , .6, ATTN_NORM, 0);

		VectorSubtract(point,monster.s.origin,facedir);
		VectorCopy(monster.s.angles, ideal_angles);
		gi.linkentity (&monster);
	}
}

void	ai_c::Pain(edict_t &monster, edict_t *other, vec3_t point, float kick, int damage)
{
	if (monster.health > 0 && body)
	{
		mmove_t *newpain;

		if (other->client)
		{
			if (IsSpecialBuddy())//if i'm a special buddy, print a special message
			{
				gi.SP_Print(other, SINGLEPLR_BUDDY_HIT);
			}
		}

		if (newpain = body->GetSequenceForPain(monster, point, kick, damage))
		{
			NewCurrentAction(PainAction(NULL, current_action, newpain, monster, other, kick, damage, 10.0), monster);
		}
	}
}

void ai_c::Escape(edict_t &monster)
{	// sort of a ::die lite - makes guys safe to toss

	// make certain I'm out of view or life will be poor indeed
	if(gi.inPVS(monster.s.origin, level.sight_client->s.origin))
	{
		return;//don't wanna see folks disappearing for no reason
	}

	attentionLevel = ATTENTION_DISTRACTED;//? eh, I don't care.  As long as they don't get here again

	monster.health = 0;

	if (monster.killtarget && (monster.deadflag != DEAD_DEAD))
	{
		qboolean useThatTarget=true;

		//if killfacing set, i might Not want to use that target!
		if (monster.killfacing)
		{
			vec3_t	forward, tokillpoint;
			edict_t	*target = NULL;
			useThatTarget=false;

			AngleVectors(monster.s.angles, forward, NULL, NULL);
			target = G_Find(target, FOFS(targetname), monster.killfacing);
			if (target)
			{
				VectorSubtract(target->s.origin, monster.s.origin, tokillpoint);
				tokillpoint[2] = 0;
				forward[2] = 0;
				VectorNormalize(tokillpoint);
				VectorNormalize(forward);
				if (DotProduct(tokillpoint, forward) > .96) // within 15 degrees either way
				{
					useThatTarget=true;
				}
			}
			else
			{
				gi.dprintf("Couldn't find killfacing ent %s--will use killtarget anyway!\n",monster.killfacing);
			}
		}

		if (useThatTarget)
		{
			edict_t *t = NULL;
			qboolean	foundScript=false;
			qboolean	foundTarget=false;
			while ((t = G_Find (t, FOFS(targetname), monster.killtarget)))
			{
				foundTarget=true;
				t->use(t, &monster, &monster);
				if (!strcmp(t->classname, "script_runner") && (t->spawnflags&1))
				{
					foundScript=true;
					gi.dprintf("%s is running a WILL_KILL_USER script, and won't die right now!\n", monster.classname);
					monster.health=monster.max_health;

					//clear out all actions, so the script stuff happens immediately
					list<action_c_ptr>::iterator	ia;

					for (ia=actions.begin();ia!=actions.end();ia++)
					{
						(*ia).Destroy();
					}
					while (actions.size())
					{
						actions.pop_front();
					}
					if (current_action)
					{
						current_action.Destroy();
					}
					if (recycle_action)
					{
						recycle_action.Destroy();
					}
				}
			}
			if (foundScript)
			{
				VectorClear(monster.velocity);
				return;
			}
			else if (!foundTarget)
			{
				gi.dprintf("Couldn't find killtarget %s!\n",monster.killtarget);
			}
		}
	}

	monster.solid = SOLID_NOT;
	monster.clipmask = 0;
	monster.svflags |= SVF_DEADMONSTER|SVF_NOCLIENT;// I don't want see this
	monster.movetype = MOVETYPE_NONE; // in case a guy is killed while NO_CLIP in scripting
	
	if (GetBody())
	{
		GetBody()->SetInitialKilledTime(level.time - 25);//give him 5 seconds to go away
		GetBody()->SetLastDFlags(0);
	}
	if (monster.deadflag != DEAD_DEAD)
	{
		list<action_c_ptr>::iterator	ia;
		list<sense_c_ptr>::iterator		is;
		list<decision_c_ptr>::iterator	id;


		// huh? should have list-eating separated out?
		for (is=senses.begin();is != senses.end();is++)
		{
			(*is).Destroy();
		}

		for (ia=actions.begin();ia!=actions.end();ia++)
		{
			(*ia).Destroy();
		}

		for (id=decisions.begin();id!=decisions.end();id++)
		{
			(*id).Destroy();
		}


		//now ditch nodes
		while (senses.size())
		{
			senses.pop_front();
		}

		while (actions.size())
		{
			actions.pop_front();
		}

		while (decisions.size())
		{
			decisions.pop_front();
		}

		if (current_action)
		{
			current_action.Destroy();
		}

		if (recycle_action)
		{
			recycle_action.Destroy();
		}

		monster.touch = NULL;//do we need this here?
		monster.deadflag = DEAD_DEAD;

		gi.linkentity (&monster);//?  I need him around for a bit to be safe, but that is all
	}
}

void ai_c::SetGround(float groundval)
{
//	if (ent && ent->ghoulInst && !ent->ghoulInst->GetPlayingSequence())
//	{
//		return;
//	}

	ground=groundval;
}

void ai_c::InitBBoxPreset(bbox_preset preset_index, vec_t xMinVal, vec_t yMinVal, vec_t zMinVal,
				vec_t xMaxVal, vec_t yMaxVal, vec_t zMaxVal)
{
	//invalid preset index
	if (preset_index < 0 || preset_index >= BBOX_PRESET_NUMBER)
	{
		return;
	}
	
	VectorSet(preset_mins[preset_index], xMinVal, yMinVal, zMinVal);
	VectorSet(preset_maxs[preset_index], xMaxVal, yMaxVal, zMaxVal);
}

void ai_c::ConfirmBBox (edict_t &monster, mmove_t	*curMove)
{
// Hmm, do we really care if a dead guy's bbox intersects architecture?  I don't think so, so I'm going to give it a shot.
// But, we can only do this with x-y, lest he fall throught the world

	monster.mins[0] = preset_mins[current_bbox][0];
	monster.mins[1] = preset_mins[current_bbox][1];
	monster.maxs[0] = preset_maxs[current_bbox][0];
	monster.maxs[1] = preset_maxs[current_bbox][1];
}

qboolean ai_c::AttemptSetBBox (edict_t &monster, bbox_preset preset_index, qboolean forceZ)
{
	trace_t tr;

	vec3_t testpos,testmins,testmaxs;

	qboolean minorAdjust = false;

	//invalid preset index
	if (preset_index < 0 || preset_index >= BBOX_PRESET_NUMBER)
	{
		return false;
	}

	VectorCopy(monster.s.origin,testpos);
	VectorScale(preset_mins[preset_index],scale,testmins);
//	testpos[2]-=testmins[2]-monster.mins[2];
	VectorScale(preset_maxs[preset_index],scale,testmaxs);


	if (preset_index == current_bbox /*&& monster.ghoulInst && monster.ghoulInst->GetPlayingSequence()*/) 
	{

		if (monster.mins[2]-ground>15.0)
		{
			ground = monster.mins[2] - 15;
		}
		if (monster.mins[2]-ground<-15.0)
		{
			ground = monster.mins[2] + 15;
		}

		float adjust2 = fabs(monster.mins[2]-ground);
		if (!monster.groundentity && (monster.solid != SOLID_CORPSE))
		{
			return true;
		}

		//commented out the groundentity check to allow guys in air to twiddle with their bboxes
		if (/*(!monster.groundentity&&monster.movetype==MOVETYPE_STEP) ||*/ adjust2 < 1)
		{
			return true;
		}
//		gi.dprintf("minor adjustment: %f\n",monster.mins[2]-ground);
		minorAdjust = true;
//		testpos[2]=monster.s.origin[2];
		testpos[2]-=ground-monster.mins[2];
		testmaxs[2]+=ground-testmins[2];
		testmins[2]=ground;
	}
	else
	{
		float majorAdjustLen = monster.mins[2]-testmins[2];
//		gi.dprintf("major adjustment: %f\n",majorAdjustLen);
//		testpos[2]-=majorAdjustLen;
		testmaxs[2]+=majorAdjustLen;
		testmins[2]+=majorAdjustLen;
	}

// set upper bound of box to the greater of the head height of the monster or the height of the
// preset bounding box.
//	bool heightadjust = false;
	if (((monster.viewheight) > testmaxs[2]) && (fabs(monster.viewheight - testmaxs[2]) > 4))
	{
		testmaxs[2] = monster.viewheight;
//		heightadjust = true;
	}

//	if (monster.deadflag==DEAD_DEAD)
//	{
//		gi.trace(testpos, testmins, testmaxs, testpos, &monster, monster.clipmask, &tr);
//	}
//	else
	if (monster.movetype == MOVETYPE_NOCLIP) // awful badness for crazy climbing animations and such
	{
		tr.allsolid = false;
	}
	else
	{
		gi.trace(testpos, testmins, testmaxs, testpos, &monster, MASK_MONSTERSOLID, &tr);
	}

	if (!tr.allsolid)
	{
		VectorCopy(testmins, monster.mins);
		VectorCopy(testmaxs, monster.maxs);
		VectorCopy(testpos, monster.s.origin);
		gi.linkentity (&monster);

		//in case i come back here this frame, set my ground to my testmins so i don't hop back where i was
//		if (!minorAdjust)
//		{
//			ground=testmins[2];
//		}

		//early attempt at preventing spawn-sprouting
//		if (monster.ghoulInst && !monster.ghoulInst->GetPlayingSequence())
//		{
//			ground=testmins[2];
//		}

//		if (!heightadjust)
		{
			current_bbox = preset_index;
		}
		return true;
	}

	if (forceZ /*&& !minorAdjust*/)
	{
		testmins[0]=monster.mins[0];
		testmins[1]=monster.mins[1];
		testmaxs[0]=monster.maxs[0];
		testmaxs[1]=monster.maxs[1];


//		if (monster.deadflag==DEAD_DEAD)
//		{
//			gi.trace(testpos, testmins, testmaxs, testpos, &monster, monster.clipmask, &tr);
//		}
//		else
		{
			gi.trace(testpos, testmins, testmaxs, testpos, &monster, MASK_MONSTERSOLID, &tr);
		}

		if (!tr.allsolid && !tr.startsolid || (monster.deadflag & DEAD_DEAD))
		{
			VectorCopy(testmins, monster.mins);
			VectorCopy(testmaxs, monster.maxs);
			VectorCopy(testpos, monster.s.origin);
			gi.linkentity (&monster);
		
//			if (!heightadjust)
			{
				current_bbox = preset_index;//achtung! not really!
			}
			return true;
		}
#if _DEBUG
		Com_Printf("Unable to force bbox z component!\n");
#endif
	}
	return minorAdjust;
}

void ai_c::Emote(edict_t &monster, emotion_index new_emotion, float emotion_duration, qboolean scripted_emoting)
{
	if (body)
	{
		body->Emote(monster,new_emotion,emotion_duration,scripted_emoting);
	}
}

emotion_index ai_c::GetMood(edict_t &monster)
{
	if (body)
	{
		return body->GetMood(monster);
	}
	return EMOTION_NORMAL;
}

void ai_c::FinishMove(edict_t &monster)
{
	if (body)
	{
		body->FinishMove(monster);
	}
}

void ai_c::InAirFrame(edict_t &monster, float frameTime)
{
	if (body)
	{
		body->HoldAnimation(monster, HOLDCODE_INAIR, frameTime);
	}
}

void ai_c::JumpFrame(edict_t &monster)
{
	if (body)
	{
		body->SetFlags(monster, FRAMEFLAG_JUMP);
	}
}

void ai_c::LandFrame(edict_t &monster)
{
	if (body)
	{
		body->SetFlags(monster, FRAMEFLAG_LAND);
	}
}

void ai_c::AttackFrame(edict_t &monster)
{
	if (body)
	{
		body->SetFlags(monster, FRAMEFLAG_ATTACK);
	}
}

void ai_c::ThrowFrame(edict_t &monster)
{
	if (body)
	{
		body->SetFlags(monster, FRAMEFLAG_THROW);
	}
}

void ai_c::MeleeFrame(edict_t &monster)
{
	if (body)
	{
		body->SetFlags(monster, FRAMEFLAG_MELEE);
	}
}

void ai_c::HandleCallBack(IGhoulInst *me,void *user,float now,const void *data)
{
	edict_t *ent = (edict_t *)user;

	if (!strcmp((char*)data,"rstep"))
	{
		FX_SetEvent(ent, EV_FOOTSTEPRIGHT);
	}
	else if (!strcmp((char*)data,"lstep"))
	{
		FX_SetEvent(ent, EV_FOOTSTEPLEFT);
	}
	else if (!strcmp((char*)data,"ooze"))
	{
		vec3_t	curVec;

		VectorCopy(ent->s.origin, curVec);
		curVec[2] += 10;

		FX_MakeDecalBelow(curVec,FXDECAL_BLOODPOOL,0);
	}
}

mmove_t *ai_c::GetMove(void)
{
	if (body)
	{
		return body->GetMove();
	}
	return NULL;
}

void ai_c::SetMove(mmove_t *newmove)
{
	if (body)
	{
		body->SetMove(newmove);
	}
}

//if you pass in NULL, will return any old scripted decision
scripted_decision *ai_c::FindScriptedDecision(edict_t* ScriptEntity)
{
	list<decision_c_ptr>::iterator	IterDecision;

	for(IterDecision = decisions.begin(); IterDecision != decisions.end(); IterDecision++)
	{
		if ((*IterDecision)->MatchScriptEnt(ScriptEntity) || (!ScriptEntity && (*IterDecision)->GetClassCode()==SCRIPTED_DECISION))
		{
			return (scripted_decision*)((decision_c*)(*IterDecision));
		}
	}
	return NULL;	
}

void ai_c::CancelScripting(edict_t* ScriptEntity)
{
	list<decision_c_ptr>::iterator	IterDecision;
	list<action_c_ptr>::iterator	IterAction, IterAction2;

	//find the scripting decision
	for(IterDecision = decisions.begin(); IterDecision != decisions.end(); IterDecision++)
	{
		//ok, got the decision...
		if ((*IterDecision)->MatchScriptEnt(ScriptEntity) || (!ScriptEntity && (*IterDecision)->GetClassCode()==SCRIPTED_DECISION))
		{
			//get rid of all the actions in the action queue that are owned by the scripted decision
			for(IterAction = actions.begin(); IterAction != actions.end(); )
			{
				if (ent && (*IterAction) && (*IterAction)->GetOwnerDecision()==(*IterDecision))
				{
					for(IterAction2 = actions.begin(); IterAction2 != actions.end(); IterAction2++)
					{
						if ((*IterAction2)->GetOwnerAction()==(*IterAction))
						{
							(*IterAction2)->SetOwnerAction(NULL);
						}
					}
					if (current_action && current_action->GetOwnerAction()==(*IterAction))
					{
						current_action->SetOwnerAction(NULL);
					}
					(*IterAction).Destroy();
					actions.erase(IterAction++);
				}
				else
				{
					IterAction++;
				}
			}

			//if the current_action is owned by the scripted decision, get rid of it too
			if (ent && current_action && current_action->GetOwnerDecision()==(*IterDecision))
			{
				NextAction(*ent);
			}

			//ok, there shouldn't be any pointers left to the scripted decision; get rid of it.
			(*IterDecision).Destroy();
			decisions.erase(IterDecision);

			break;//this will be messy if a scriptent has more than one decision, but that shouldn't ever happen...
		}
	}
}

decision_c *ai_c::FindOrderDecision()
{
	list<decision_c_ptr>::iterator	IterDecision;

	for(IterDecision = decisions.begin(); IterDecision != decisions.end(); IterDecision++)
	{
		if ((*IterDecision)->GetClassCode() == ORDER_DECISION)
		{
			return (*IterDecision);
		}
	}
	return NULL;	
}

bool ai_c::GetLastActionDestination(vec3_t Destination)
{
//  The function should return the goal position for the lastest action with such a position
	list<action_c_ptr>::iterator	IterAction;

	for(IterAction = (--actions.end()); IterAction != actions.end(); IterAction--)
	{
		if (*IterAction)
		{
			(*IterAction)->GetDest(Destination);
		}
		else 
		{
			continue;
		}
		if (!(VectorCompare(Destination, vec3_origin)))
		{
			return true;
		}
	}
	// check the current action
	if (current_action)
	{
		current_action->GetDest(Destination);
	}
	if (!(VectorCompare(Destination, vec3_origin)))
	{
		return true;
	}
	return false;
}

void ai_c::PickDeathAnim(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags)
{
	mmove_t *mydeath;
	if (body)
	{
		mydeath = body->GetSequenceForDeath(monster, inflictor, attacker, damage, point, dflags);
		if (mydeath && mydeath->suggested_action != ACTCODE_DEATH)
		{
			gi.dprintf("Death action %s not really a death!\n", mydeath->ghoulSeqName);
		}
		if (!mydeath)
		{
			mydeath=&generic_move_death_long;
		}
	}

	if (monster.spawnflags & SPAWNFLAG_ARMOR_PICKUP)
	{
		NewCurrentAction(new deathcorpse_action(NULL, NULL, mydeath, monster, inflictor, attacker, damage, point), monster);
	}
	else
	{
		NewCurrentAction(DeathAction(NULL, NULL, mydeath, monster, inflictor, attacker, damage, point), monster);
	}
}

action_c *ai_c::GetLastAction()
{
	list<action_c_ptr>::iterator	ActionIter;

	ActionIter = actions.end();
	ActionIter--;

	return (*ActionIter);
}

void ai_c::RemoveOldDecisions (int code)
{
	list<decision_c_ptr>::iterator	DecisionIter;

	for (DecisionIter = decisions.begin();DecisionIter!=decisions.end();DecisionIter++)
	{
		if ((*DecisionIter)->GetClassCode() == code)
		{
			list<action_c_ptr>::iterator	ActionIter;

			// eliminate old order actions
			for (ActionIter=actions.begin();ActionIter!=actions.end();ActionIter++)
			{
				if ((*ActionIter)->GetOwnerDecision() == (*DecisionIter))
				{
					(*ActionIter).Destroy();
				}
			}
			(*DecisionIter).Destroy();
		}
	}
}

//eek, has ballooned into a large blimpy thing...
void ai_c::SetTargetTime(float now, edict_t *target, vec3_t position)
{
	//if i'm first being alerted, notify other folks!
	if (!hasHadTarget && ent)
	{
		//HACK ALERT! ...but only if i'm close to the guy...
		vec3_t topos;
		VectorSubtract(position, ent->s.origin, topos);
//		if (VectorLengthSquared(topos)<250000)
		if (VectorLengthSquared(topos)<1000000)//this is tweaked to work on trn1
		{
			if (isStartleable)
			{
				SetStartleability(false);

				//only play alert sound if perception was immediate, and enemy is client...(fixme?)
				if (target->client && body)
				{
					body->VoiceWakeSound(*ent, 0.9);
//					PlayerNoise(target, position, AI_SENSETYPE_SOUND_WAKEUP, NULL, 500);
					PlayerNoise(target, ent->s.origin, AI_SENSETYPE_SOUND_WAKEUP, NULL, 500);
				}
			}
		}
	}

	hasHadTarget=true;
	if (now>lastTargetTime)
	{
		lastTargetTime=now;
	}
	if (ent && GetClassCode()!=AI_MESO_RAIDERBOSS2 && GetClassCode()!=AI_MESO_SKINHEADBOSS)
	{
		ent->spawnflags&=~SPAWNFLAG_SENSE_MUTE;

		//give me senses--i'm after somebody!
		if (ent->spawnflags&SPAWNFLAG_BLIND)
		{
			NewSense(new normalsight_sense(), ent);
			ent->spawnflags&=(~SPAWNFLAG_BLIND);
		}
		if (ent->spawnflags&SPAWNFLAG_DEAF)
		{
			NewSense(new sound_sense(), ent);
			ent->spawnflags&=(~SPAWNFLAG_DEAF);
		}
	}

	assert((!target)||(target->inuse < 10));
	curTarget = target;

}

void ai_c::UpdatePathPosition(edict_t *myEnt)
{
	if(aiPoints.isActive())
	{
		aiPoints.linkEnemyIntoPath(ent->s.origin, &nodeData);
	}
}

int ai_c::CheckForCorpseActivate(edict_t &monster)
{
	edict_t *curSearch = 0;

	CRadiusContent rad(monster.s.origin, 200);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		curSearch = rad.foundEdict(i);

		if(curSearch->deadflag == DEAD_DEAD)
		{	// is this a valid search?
			return 1;
		}
	}
	return 0;
}

void ai_c::RegisterShotForDodge(vec3_t start, vec3_t end, edict_t *shooter)
{
	list<decision_c_ptr>::iterator	id, pick, t_id;
	int						i,dec_siz;

	dec_siz=decisions.size();

	if (dec_siz==0)
	{
		return;
	}

	RegisterSenseEvent(sound_mask, start, level.time, shooter, AI_SENSETYPE_SOUND_WHIZ);

	for (id=decisions.begin(), i=dec_siz;i>0;i--)
	{
		(*id)->SetInfoForDodge(start, end);
		id++;
	}
}

void ai_c::RegisterShotForReply(vec3_t start, vec3_t end, edict_t *shooter)
{
	list<decision_c_ptr>::iterator	id, pick, t_id;
	int						i,dec_siz;

	dec_siz=decisions.size();

	if (dec_siz==0)
	{
		return;
	}

	for (id=decisions.begin(), i=dec_siz;i>0;i--)
	{
		(*id)->SetInfoForReply(start, end);
		id++;
	}
}

void ai_c::Spook(float amount, vec3_t center)
{
	list<decision_c_ptr>::iterator	id, pick, t_id;
	int						i,dec_siz;

	dec_siz=decisions.size();

	if (dec_siz==0)
	{
		return;
	}

	for (id=decisions.begin(), i=dec_siz;i>0;i--)
	{
		(*id)->AddFear(amount, center);
		id++;
	}

}

action_c *ai_c::WalkAction(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing, float timeout, qboolean fullAnimation)
{
	if (recycle_action && recycle_action->GetClassCode()==WALK_ACTION)
	{
		walk_action *returnme=(walk_action*)(action_c*)recycle_action;
		recycle_action = NULL;
		returnme->Init(od, oa, newanim, destination, facing, timeout, fullAnimation);
		return returnme;
	}

	return new walk_action(od, oa, newanim, destination, facing, timeout,fullAnimation);
}

action_c *ai_c::DefaultAction(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing, edict_t *target, float timeout, qboolean fullAnimation)
{
	if (recycle_action && recycle_action->GetClassCode()==STAND_ACTION)
	{
		stand_action *returnme=(stand_action*)(action_c*)recycle_action;
		recycle_action = NULL;
		returnme->Init(od, oa, newanim, destination, facing, target, timeout, fullAnimation);
		return returnme;
	}

	return new stand_action(od, oa, newanim, destination, facing, target, timeout,fullAnimation);
}

action_c *ai_c::PainAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *other, float kick, int damage, float timeout, qboolean fullAnimation)
{
	return new 	pain_action(od, oa, newanim, monster, other, kick, damage, timeout,fullAnimation);
}

action_c *ai_c::DeathAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	return new 	death_action(od, oa, newanim, monster, inflictor, attacker, damage, point);
}

action_c *ai_c::DeathAction(decision_c *od, action_c *oa, mmove_t *newanim1, mmove_t *newanim2, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	return new 	death_action(od, oa, newanim1, newanim2, monster, inflictor, attacker, damage, point);
}

action_c *ai_c::DeathArmorAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	return new 	deatharmor_action(od, oa, newanim, monster, inflictor, attacker, damage, point);
}

action_c *ai_c::DeathArmorAction(decision_c *od, action_c *oa, mmove_t *newanim1, mmove_t *newanim2, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	return new 	deatharmor_action(od, oa, newanim1, newanim2, monster, inflictor, attacker, damage, point);
}

action_c *ai_c::AttackAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t attackPos, vec3_t facing, float timeout, qboolean fullAnimation, bool shouldKill, int NullTarget)
{
	return new 	shoot_attack_action(od, oa, newanim, attackTarget, attackPos, facing, timeout,fullAnimation, shouldKill, NullTarget);
}

action_c *ai_c::JumpAction(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing, float timeout, qboolean fullAnimation)
{
	return new 	jump_action(od, oa, newanim, destination, facing, timeout,fullAnimation);
}

action_c *ai_c::FallAction(decision_c *od, action_c *oa, mmove_t *newanim, mmove_t *newanim2, vec3_t destination, vec3_t facing, float timeout, qboolean fullAnimation)
{
	return new 	fall_action(od, oa, newanim, newanim2, destination, facing, timeout,fullAnimation);
}

action_c *ai_c::SurrenderAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *surrenderTo, vec3_t attackerPos, vec3_t facing, float timeout, qboolean fullAnimation)
{
	return new 	surrender_action(od, oa, newanim, surrenderTo, attackerPos, facing, timeout,fullAnimation);
}

action_c *ai_c::CaptureAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t attackPos, vec3_t facing, float timeout, qboolean fullAnimation)
{
	return new 	capture_action(od, oa, newanim, attackTarget, attackPos, facing, timeout,fullAnimation);
}

action_c *ai_c::EndScriptAction(decision_c *od)
{
	return new 	endscript_action(od);
}

void ai_c::RequestMoveOutOfWay(vec3_t moveDir)
{
	VectorCopy(moveDir, requestedMoveDir);
}

ai_c *ai_c::Create(int classcode, edict_t *monster, char *ghoulname, char *subname)
{
	ai_c	*new_ai = (ai_c*)NewClassForCode(classcode);
	new_ai->SetEnt(monster);
	new_ai->AddBody(monster);
	new_ai->Init(monster, ghoulname, subname);
	if((classcode >= AI_ECTO_SKINLEADER)&&(classcode <= AI_FEMALE_TAYLOR))
	{
		new_ai->mySkills = enemySkills[classcode - AI_ECTO_SKINLEADER];//easiest way to do all this with minimal code
		if(monster)
		{
			float myVal = gi.RegionDistance(monster->s.origin);
			if(myVal != -1)
			{	//sigh... this is pretty general
				if(myVal > 1200)
				{
					new_ai->mySkills.adjustAccuracy(.66);//?
				}
				else
				{
					new_ai->mySkills.adjustAccuracy(.33);//?
				}
			}
		}
	}
	new_ai->mySkills.InitStyles(monster);
	return new_ai;
}







// CAISkills Stuff

tempStyle_t enemyActionStyles[] =
{
//	 shot at, no visibility		friend killed				attacked					out of range					too close				explosive
	{ESSA_ATTACK_DIRECTION,		ESFK_IGNORE,				ESAT_APPROACH,				ESOR_ATTACK_IF_POSSIBLE,		ESTC_HOLD_POSITION,		ESEX_IGNORE},	//EST_AGGRESSIVE
	{ESSA_RETREAT_TO_COVER,		ESFK_IGNORE,				ESAT_DUCK,					ESOR_APPROACH,					ESTC_ROLL_TO_SIDE,		ESEX_DODGE},	//EST_EVASIVE
	{ESSA_FLEE_TO_COVER,		ESFK_RETREAT_TO_AMBUSH,		ESAT_COVER_AND_AMBUSH,		ESOR_APPROACH,					ESTC_RETREAT,			ESEX_DODGE},	//EST_SNEAKY
	{ESSA_ATTACK_DIRECTION,		ESFK_RETREAT_TO_COVER,		ESAT_CIRCLE,				ESOR_APPROACH,					ESTC_CIRCLE,			ESEX_DODGE},	//EST_TACTICAL
	{ESSA_FLEE_TO_COVER,		ESFK_RETREAT_TO_COVER,		ESAT_COVER_AND_ATTACK,		ESOR_HOLD_POSITION,				ESTC_RETREAT,			ESEX_DODGE},	//EST_COWARDLY
	{ESSA_RETREAT_TO_COVER,		ESFK_RETREAT_TO_ALLIES,		ESAT_COVER_AND_ATTACK,		ESOR_HOLD_POSITION,				ESTC_RETREAT,			ESEX_DODGE},	//EST_CAUTIOUS
};

CAISkills::CAISkills(void)
{
	idealRangeMin = 100;
	idealRangeMax = 300;
	dodgeAbility = .8;

	shootingAccuracy = 1.0;

	hesitation = 0.0;
	aimTime = 0.0f;
	cashValue = 0.0f;
	turnSpeed = 1.0;

	flags = 0;

	shotAt = 0;
	friendsKilled = 0;
	attacked = 0;
	outOfRange = 0;
	tooClose = 0;
	explosive = 0;
}

void CAISkills::setSkills(float rangeMin, float rangeMax, float dodge, float accuracy,
						  float newHesitation, float newAimTime, float newCashValue, float newTurnSpeed, float newFlags)
{
	idealRangeMin = rangeMin;
	idealRangeMax = rangeMax;
	dodgeAbility = dodge;
	shootingAccuracy = accuracy;
	hesitation = newHesitation;
	aimTime = newAimTime;
	cashValue = newCashValue;
	flags = newFlags;
	turnSpeed = newTurnSpeed;
}

void CAISkills::setStyles(int newShotAt, int newFriendsKilled, int newAttacked, int newOutOfRange,
						int newTooClose, int newExplosive)
{
	shotAt = newShotAt;
	friendsKilled = newFriendsKilled;
	attacked = newAttacked;
	outOfRange = newOutOfRange;
	tooClose = newTooClose;
	explosive = newExplosive;
}

void CAISkills::setStyles(int styleNum)
{
	if(styleNum >= 0 && styleNum < EST_NUM_STYLES)
	{
		setStyles(enemyActionStyles[styleNum].shotAt, enemyActionStyles[styleNum].friendsKilled,
			enemyActionStyles[styleNum].attacked, enemyActionStyles[styleNum].outOfRange,
			enemyActionStyles[styleNum].tooClose, enemyActionStyles[styleNum].explosive);
	}
}

CAISkills::CAISkills(float rangeMin, float rangeMax, float dodge, float accuracy, 
		float newHesitation, float newAimTime, float newCashValue, float newTurnSpeed, float newFlags,
		int newShotAt, int newFriendsKilled, int newAttacked, int newOutOfRange,
			int newTooClose, int newExplosive)
{
	setSkills(rangeMin, rangeMax, dodge, accuracy, newHesitation, newAimTime, newCashValue, newTurnSpeed, newFlags);
	setStyles(newShotAt, newFriendsKilled, newAttacked, newOutOfRange, newTooClose, newExplosive);
}

void CAISkills::operator=(CAISkills &that)
{
	idealRangeMin = that.idealRangeMin;
	idealRangeMax = that.idealRangeMax;
	dodgeAbility = that.dodgeAbility;

	shootingAccuracy = that.shootingAccuracy;

	hesitation = that.hesitation;
	aimTime = that.aimTime;
	cashValue = that.cashValue;
	turnSpeed = that.turnSpeed;
	flags = that.flags;

	shotAt = that.shotAt;
	friendsKilled = that.friendsKilled;
	attacked = that.attacked;
	outOfRange = that.outOfRange;
	tooClose = that.tooClose;
	explosive = that.explosive;	

	//Eh?  BAD BAD BAD - this is naughty!  People pay me to write this kind of stuff?  Incredible!
	if((!strncmp(level.mapname, "tsr", 3))||
		(!strncmp(level.mapname, "trn", 3))||
		((!(strncmp(level.mapname, "irq", 3)))&&(level.mapname[4] == 'a')))
	{
		hesitation += .25;
		shootingAccuracy *= .75;
		aimTime += 2;
		cashValue *= .5;
	}
}

void CAISkills::InitStyles(edict_t *monster)
{
	if(!monster)
	{
		return;
	}

	if(monster->personality)
	{
		setStyles(monster->personality - 1);
	}
	else
	{
		//default it properly here later based on flags set in the skill stuff here
		setStyles(gi.irand(0, EST_NUM_STYLES-1));
	}
}