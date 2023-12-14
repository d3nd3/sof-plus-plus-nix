#include "g_local.h"
#include "ai_private.h"
#include "m_generic.h"
#include "m_female.h"

#define FEMALE_POUCH_SCALE 1.125
#define FEMALE_HAT_SCALE 1.125
#define FEMALE_GLASSES_SCALE 1.0
#define FEMALE_WEAPON_SCALE 1.0 
#define FEMALE_OTHER_SCALE 1.125


void generic_female_ai::AddBody(edict_t *monster)
{
	if (!monster)
	{
		return;
	}
	body = new bodyfemale_c();
}

const char *generic_female_ai::GetInterpFile(char *theclass, char *subclass)
{
	lev_interp_code curLevel = GetInterpCode();
	
	switch (curLevel)
	{
	case LEVCODE_NYC_SUBWAY://tsr1
	case LEVCODE_SIB_CANYON://sib1
	case LEVCODE_SIB_BASE://sib2
	case LEVCODE_NYC_WARE://nyc1
		return "fem_pistol";//no npc-only or player-only seqs

	case LEVCODE_ARM_1://arm1
	case LEVCODE_ARM_2://arm2
	case LEVCODE_ARM_3://arm3
		return "fem_arm";//no player-only or pistol seqs

	case LEVCODE_CAS_1://ger1

	//taylor is said to be here, so this may need specialness...
	case LEVCODE_CAS_3://ger3
	case LEVCODE_CAS_4://ger4
		return "fem_shot";//no npc-only or player-only seqs

	case LEVCODE_SIB_PLANT://sib3
	case LEVCODE_NYC_STREETS://nyc3
	case LEVCODE_TOK_STREET://jpn1
	case LEVCODE_TOK_OFFICE://jpn2
		return "fem_pisnpc";//no player-only seqs

	case LEVCODE_IRQ_TOWNA://irq1a
	case LEVCODE_IRQ_BUNKER:
	case LEVCODE_IRQ_CARGO:
	case LEVCODE_IRQ_STREETS://irq2a
	case LEVCODE_IRQ_FORT://irq2b
	case LEVCODE_IRQ_OIL:
		return "fem_npc";//no player-only or armed seqs--should be very small


	case LEVCODE_TUTORIAL:
	case LEVCODE_NYC_SUBWAY2:
	case LEVCODE_AFR_TRAIN:
	case LEVCODE_KOS_SEWER:
	case LEVCODE_KOS_BIGGUN:
	case LEVCODE_KOS_HANGAR:
	case LEVCODE_NYC_STEAM:
	case LEVCODE_AFR_YARD:
	case LEVCODE_AFR_HOUSE:
	case LEVCODE_AFR_FACT:
	case LEVCODE_TOK_PENT:
	case LEVCODE_CAS_2:
	case LEVCODE_UNKNOWN:
		gi.dprintf("WARNING: no level .ghb for female--using female.ghb!\n");
		break;
	}
	return "female";
}

void skinchick_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_nycskinchick",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycskinchick",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_15_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_15_",VARIETY_PRIORITY_HIGHEST);
}

void skinchick_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC *myInstance;

	generic_female_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL1);

	body->ApplySkin(*monster,  myInstance, "b", "b_nycskinchick");
	body->ApplySkin(*monster,  myInstance, "a", "a_nycskinchick");

	body->ApplySkin(*monster,  myInstance, "h", "h_w_15_c");
	body->SetFace(*monster,"f_w_15_");

	body->SetVoiceDirectories(*monster, "skf", 1, DEATHVOICE_FEMALE);
	body->SetRootBolt(*monster);
}

void nycwoman_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_nycwomanw",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycwomanw",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_18_m",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_18_",VARIETY_PRIORITY_HIGHEST);

	RegisterSkin("b","b_nycwomanb",VARIETY_PRIORITY_LOW);
	RegisterSkin("a","a_nycwomanb",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_b_8_c",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_b_8_",VARIETY_PRIORITY_LOW);
}

void nycwoman_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC *myInstance;
	qboolean	altskin=false;

	generic_female_ai::Init(monster, ghoulname, subclass);

	if (!body || !monster->ghoulInst)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	if (gi.irand(0,1))//caucasian
	{
		altskin = body->ApplySkin(*monster,  myInstance, "b", "b_nycwomanb")
			&& body->ApplySkin(*monster,  myInstance, "a", "a_nycwomanb");
	}
	if (altskin)
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_b_8_c");
		body->SetFace(*monster,"f_b_8_");

		//she uses crew hair...
		monster->ghoulInst->SetPartOnOff("_medmhairmhead",false);
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "b", "b_nycwomanw");
		body->ApplySkin(*monster,  myInstance, "a", "a_nycwomanw");
		body->ApplySkin(*monster,  myInstance, "h", "h_w_18_m");
		body->SetFace(*monster,"f_w_18_");

		//she uses medium hair...
		monster->ghoulInst->SetPartOnOff("_crewhair",false);
		monster->ghoulInst->SetPartOnOff("_lear",false);
		monster->ghoulInst->SetPartOnOff("_rear",false);
	}
	body->SetVoiceDirectories(*monster, "npcf", 1, DEATHVOICE_FEMALE);
	body->SetRootBolt(*monster);
}

void tokassassin_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_tokassassin",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_tokassassin",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_j_8_m",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_j_8_",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_j_7_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_j_7_",VARIETY_PRIORITY_HIGHEST);
}

void tokassassin_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC *myInstance;
	qboolean	altskin=false;

	generic_female_ai::Init(monster, ghoulname, subclass);

	if (!body || !monster->ghoulInst)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL1);
	body->SetLeftHandWeapon(*monster, ATK_THROWSTAR);

	body->ApplySkin(*monster,  myInstance, "b", "b_tokassassin");
	body->ApplySkin(*monster,  myInstance, "a", "a_tokassassin");

	if (gi.irand(0,1))
	{
		altskin=body->ApplySkin(*monster,  myInstance, "h", "h_j_8_m");
	}
	
	if (altskin)
	{
		//she uses medium hair...
		body->SetFace(*monster,"f_j_8_");
		monster->ghoulInst->SetPartOnOff("_crewhair",false);
		monster->ghoulInst->SetPartOnOff("_lear",false);
		monster->ghoulInst->SetPartOnOff("_rear",false);
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_j_7_c");
		body->SetFace(*monster,"f_j_7_");

		//she uses crew hair...
		monster->ghoulInst->SetPartOnOff("_medmhairmhead",false);
	}

	body->SetVoiceDirectories(*monster, "skinhead", 2, DEATHVOICE_FEMALE);
	body->SetRootBolt(*monster);
}

void tokwoman1_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_tokwoman1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_tokwoman1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_j_8_m",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_j_8_",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_j_7_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_j_7_",VARIETY_PRIORITY_HIGHEST);
}

void tokwoman1_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC *myInstance;
	qboolean	altskin=false;

	generic_female_ai::Init(monster, ghoulname, subclass);

	if (!body || !monster->ghoulInst)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_norm", FEMALE_GLASSES_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_tokwoman1");
	body->ApplySkin(*monster,  myInstance, "a", "a_tokwoman1");

	if (gi.irand(0,1))
	{
		altskin=body->ApplySkin(*monster,  myInstance, "h", "h_j_8_m");
	}
	if (altskin)
	{
		body->SetFace(*monster,"f_j_8_");

		//she uses medium hair...
		monster->ghoulInst->SetPartOnOff("_crewhair",false);
		monster->ghoulInst->SetPartOnOff("_lear",false);
		monster->ghoulInst->SetPartOnOff("_rear",false);
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_j_7_c");
		body->SetFace(*monster,"f_j_7_");

		//she uses crew hair...
		monster->ghoulInst->SetPartOnOff("_medmhairmhead",false);
	}

	body->SetVoiceDirectories(*monster, "npcf", 1, DEATHVOICE_FEMALE);
	body->SetRootBolt(*monster);
}

void tokwoman2_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_tokwoman2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_tokwoman2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_j_8_m",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_j_8_",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_j_7_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_j_7_",VARIETY_PRIORITY_HIGHEST);
}

void tokwoman2_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC *myInstance;
	qboolean	altskin=false;

	generic_female_ai::Init(monster, ghoulname, subclass);

	if (!body || !monster->ghoulInst)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_norm", FEMALE_GLASSES_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_tokwoman2");
	body->ApplySkin(*monster,  myInstance, "a", "a_tokwoman2");

	if (gi.irand(0,1))
	{
		altskin=body->ApplySkin(*monster,  myInstance, "h", "h_j_8_m");
	}
	if (altskin)
	{
		body->SetFace(*monster,"f_j_8_");

		//she uses medium hair...
		monster->ghoulInst->SetPartOnOff("_crewhair",false);
		monster->ghoulInst->SetPartOnOff("_lear",false);
		monster->ghoulInst->SetPartOnOff("_rear",false);
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_j_7_c");
		body->SetFace(*monster,"f_j_7_");

		//she uses crew hair...
		monster->ghoulInst->SetPartOnOff("_medmhairmhead",false);
	}

	body->SetVoiceDirectories(*monster, "npcf", 1, DEATHVOICE_FEMALE);
	body->SetRootBolt(*monster);
}

void fsibguard_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_sibguard2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_sibguard2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_18_m",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_w_18_",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_w_17_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_17_",VARIETY_PRIORITY_HIGHEST);
}

void fsibguard_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC *myInstance;
	qboolean	altskin=false;

	generic_female_ai::Init(monster, ghoulname, subclass);

	if (!body || !monster->ghoulInst)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_MACHINEPISTOL);

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_baseball", "to_abolt_head_t", myInstance, "acc_hat_baseball_sib", FEMALE_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_sibguard2");
	body->ApplySkin(*monster,  myInstance, "a", "a_sibguard2");

	if (gi.irand(0,1))
	{
		altskin=body->ApplySkin(*monster,  myInstance, "h", "h_w_18_m");
	}
	if (altskin)
	{
		body->SetFace(*monster,"f_w_18_");

		//she uses medium hair...
		monster->ghoulInst->SetPartOnOff("_crewhair",false);
		monster->ghoulInst->SetPartOnOff("_lear",false);
		monster->ghoulInst->SetPartOnOff("_rear",false);
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_w_17_c");
		body->SetFace(*monster,"f_w_17_");

		//she uses crew hair...
		monster->ghoulInst->SetPartOnOff("_medmhairmhead",false);
	}
	body->SetVoiceDirectories(*monster, "", 1, DEATHVOICE_FEMALE);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_CHEST);
}

void fsibscience_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_sibscientist2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_sibscientist2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_18_m",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_w_18_",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_w_17_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_17_",VARIETY_PRIORITY_HIGHEST);
}

void fsibscience_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC *myInstance;
	qboolean	altskin=false;

	generic_female_ai::Init(monster, ghoulname, subclass);

	if (!body || !monster->ghoulInst)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->ApplySkin(*monster,  myInstance, "b", "b_sibscientist2");
	body->ApplySkin(*monster,  myInstance, "a", "a_sibscientist2");

	if (gi.irand(0,2))
	{
		altskin=body->ApplySkin(*monster,  myInstance, "h", "h_w_18_m");
	}

	if (altskin)
	{
		body->SetFace(*monster,"f_w_18_");
		if(gi.irand(0,1))
		{
			monster->ghoulInst->SetPartOnOff("_crewhair",false);
			monster->ghoulInst->SetPartOnOff("_lear",false);
			monster->ghoulInst->SetPartOnOff("_rear",false);
		}
		else
		{
			monster->ghoulInst->SetPartOnOff("_medmhairmhead",false);
			monster->ghoulInst->SetPartOnOff("_lbang",false);
			monster->ghoulInst->SetPartOnOff("_rbang",false);
		}
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_w_17_c");
		body->SetFace(*monster,"f_w_17_");
		//she uses crew hair...
		monster->ghoulInst->SetPartOnOff("_medmhairmhead",false);
		monster->ghoulInst->SetPartOnOff("_lbang",false);
		monster->ghoulInst->SetPartOnOff("_rbang",false);
	}
	body->SetVoiceDirectories(*monster, "", 1, DEATHVOICE_FEMALE);
	body->SetRootBolt(*monster);
}

void irqwoman1_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqwoman1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqwoman1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_irqwoman1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_irqwoman1",VARIETY_PRIORITY_HIGHEST);
}

void irqwoman1_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC *myInstance;

	generic_female_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->ApplySkin(*monster,  myInstance, "b", "b_irqwoman1");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqwoman1");
	body->ApplySkin(*monster,  myInstance, "h", "h_irqwoman1");
	body->ApplySkin(*monster,  myInstance, "f", "f_irqwoman1");

	body->SetVoiceDirectories(*monster, "npcf", 1, DEATHVOICE_FEMALE);
	body->SetRootBolt(*monster);
}

void irqwoman2_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqwoman2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqwoman2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_irqwoman2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_irqwoman2",VARIETY_PRIORITY_HIGHEST);
}

void irqwoman2_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC *myInstance;

	generic_female_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->ApplySkin(*monster,  myInstance, "b", "b_irqwoman2");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqwoman2");

	body->ApplySkin(*monster,  myInstance, "h", "h_irqwoman2");
	body->ApplySkin(*monster,  myInstance, "f", "f_irqwoman2");

	body->SetVoiceDirectories(*monster, "npcf", 1, DEATHVOICE_FEMALE);
	body->SetRootBolt(*monster);
}

void fraider_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_dcfemale",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_dcfemale",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_raider5",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_raider5",VARIETY_PRIORITY_HIGHEST);
}

void fraider_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC *myInstance;

	generic_female_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetLeftHandWeapon(*monster, ATK_FLASHGREN);
	body->SetRightHandWeapon(*monster, ATK_FLAMEGUN);

	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_tanks", "to_abolt_back", myInstance, "acc_tanks_cas", FEMALE_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_dcfemale");
	body->ApplySkin(*monster,  myInstance, "a", "a_dcfemale");

	body->ApplySkin(*monster,  myInstance, "h", "h_raider5");
	body->ApplySkin(*monster,  myInstance, "f", "f_raider5");

	body->SetVoiceDirectories(*monster, "raf", 1, DEATHVOICE_FEMALE);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_FULL);
}

void taylor_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_xtaylor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_xtaylor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_taylor",VARIETY_PRIORITY_HIGHEST);

	RegisterFaceSkins("f_taylor_", VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_taylor_n",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_taylor_b",VARIETY_PRIORITY_HIGHEST);

	RegisterTalkFaceSkins("f_taylor_n", VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_taylor_nt1",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_taylor_nt2",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_taylor_nt3",VARIETY_PRIORITY_HIGHEST);
}

void taylor_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC *myInstance;

	generic_female_ai::Init(monster, ghoulname, subclass);

	SetSpecialBuddy(true);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	if ( !(monster->spawnflags & SPAWNFLAG_NO_WEAPONS) )
	{
		body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);
	}

//	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_backpack_smll", "to_abolt_back", myInstance, "acc_backpack_smll_dgrey", FEMALE_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_xtaylor");
	body->ApplySkin(*monster,  myInstance, "a", "a_xtaylor");

	body->ApplySkin(*monster,  myInstance, "h", "h_taylor");
	body->SetFace(*monster,"f_taylor_");

	body->SetVoiceDirectories(*monster, "taylor", 1, DEATHVOICE_FEMALE);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_FULL);
}

void SP_m_nyc_fskinchick (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_FEMALE_SKINCHICK, self, "enemy/female", "skinchick");
	self->think = generic_grunt_init;
}


void SP_m_nyc_fwoman (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_FEMALE_NYWOMAN, self, "enemy/female", "nywoman");
	self->think = generic_npc_init;
}

void SP_m_sib_fguard2 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_FEMALE_SIBGUARD, self, "enemy/female", "sibguard");
	self->think = generic_grunt_init;
}

void SP_m_sib_fscientist2 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_FEMALE_SIBSCIENCE, self, "enemy/female", "sibscience");
	self->think = generic_npc_init;
}

void SP_m_irq_fwoman1 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_FEMALE_IRQWOMAN1, self, "enemy/female", "irqwoman1");
	self->think = generic_npc_init;
}

void SP_m_irq_fwoman2 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_FEMALE_IRQWOMAN2, self, "enemy/female", "irqwoman2");
	self->think = generic_npc_init;
}

void SP_m_tok_fwoman1 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_FEMALE_TOKWOMAN1, self, "enemy/female", "tokwoman1");
	self->think = generic_npc_init;
}

void SP_m_tok_fwoman2 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_FEMALE_TOKWOMAN2, self, "enemy/female", "tokwoman2");
	self->think = generic_npc_init;
}

void SP_m_cas_ffemale (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_FEMALE_RAIDER, self, "enemy/female", "raider");
	self->think = generic_grunt_init;
}

void SP_m_tok_fassassin (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_FEMALE_TOKASSASSIN, self, "enemy/female", "tokassassin");
	self->think = generic_grunt_init;
}

void SP_m_x_ftaylor (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_FEMALE_TAYLOR, self, "enemy/female", "taylor");
	self->think = generic_grunt_init;
}
