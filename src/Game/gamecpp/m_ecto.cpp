#include "g_local.h"
#include "ai_private.h"
#include "m_generic.h"
#include "m_ecto.h"

#define ECTO_POUCH_SCALE 1.125
#define ECTO_HAT_SCALE 1.125
#define ECTO_GLASSES_SCALE 1.0
#define ECTO_WEAPON_SCALE 1.0 
#define ECTO_OTHER_SCALE 1.125

#define ECTO_GLASSES_NAME "acc_ecto_glasses"

void generic_ecto_ai::AddBody(edict_t *monster)
{
	if (!monster)
	{
		return;
	}
	body = new bodyecto_c();
}

const char *generic_ecto_ai::GetInterpFile(char *theclass, char *subclass)
{
	lev_interp_code curLevel = GetInterpCode();
	
	switch (curLevel)
	{
	case LEVCODE_NYC_SUBWAY:
		return "ecto_tsr1";//no npc-only or player-only seqs
	case LEVCODE_KOS_SEWER:
		return "ecto_kos1";//super-simple refugee for script
	case LEVCODE_TUTORIAL:
		return "ecto_tut1";
	case LEVCODE_ARM_1:
	case LEVCODE_ARM_2:
	case LEVCODE_ARM_3:
		return "ecto_arm";//super-simple fellas for armory
	case LEVCODE_SIB_BASE:
		return "ecto_sib2";
	case LEVCODE_AFR_YARD:
	case LEVCODE_AFR_HOUSE:
	case LEVCODE_AFR_FACT:
		return "ecto_sud";
	case LEVCODE_IRQ_TOWNA:
	case LEVCODE_NYC_SUBWAY2:
	case LEVCODE_SIB_CANYON:
	case LEVCODE_NYC_WARE:
	case LEVCODE_CAS_1:
	case LEVCODE_CAS_3:
	case LEVCODE_CAS_4:
	case LEVCODE_SIB_PLANT:
	case LEVCODE_NYC_STREETS:
	case LEVCODE_TOK_STREET:
	case LEVCODE_TOK_OFFICE:
	case LEVCODE_IRQ_STREETS:
	case LEVCODE_IRQ_FORT:
	case LEVCODE_AFR_TRAIN:
	case LEVCODE_KOS_BIGGUN:
	case LEVCODE_KOS_HANGAR:
	case LEVCODE_IRQ_BUNKER:
	case LEVCODE_IRQ_CARGO:
	case LEVCODE_NYC_STEAM:
	case LEVCODE_TOK_PENT:
	case LEVCODE_IRQ_OIL:
	case LEVCODE_CAS_2:
	case LEVCODE_UNKNOWN:
		gi.dprintf("WARNING: no level .ghb for ecto--using ecto.ghb!\n");
		break;
	}
	return "ecto";
}

void generic_ecto_ai::Init(edict_t *monster, char *ghoulname, char *subclass)
{
	generic_human_ai::Init(monster, ghoulname, subclass);

/*	scale *= 0.9f;

	Matrix4 mat, mat1, mat2;

	monster->ghoulInst->GetXForm(mat);
	mat2.Identity();
	mat1=mat;
	mat2.Scale(0.9f);
	mat.Concat(mat1, mat2);
	monster->ghoulInst->SetXForm(mat);
*/
}

void skinleader_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_nycskinhleader",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycskinhleader",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_nycskinhleader2",VARIETY_PRIORITY_LOW);
	RegisterSkin("a","a_nycskinhleader2",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_w_12_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_12_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_1_s",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_w_1_",VARIETY_PRIORITY_LOW);
}

void skinleader_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC *myInstance;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	if (!(monster->spawnflags & SPAWNFLAG_NO_WEAPONS))
	{
		body->SetRightHandWeapon(*monster, ATK_PISTOL1);
//		body->SetLeftHandWeapon(*monster, ATK_MOLITOV);
		body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_blck", ECTO_OTHER_SCALE);
	}

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", ECTO_GLASSES_NAME, "to_abolt_head_t", myInstance, "acc_glasses_sun", ECTO_GLASSES_SCALE);

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_baseball", "to_abolt_head_t_bkwd", myInstance, "acc_hat_baseball_grcamo", ECTO_OTHER_SCALE);


	bool gotleather = false;
	if (gi.irand(0,1))
	{
		if (body->ApplySkin(*monster,  myInstance, "b", "b_nycskinhleader2"))
		{
			gotleather=true;
			body->ApplySkin(*monster,  myInstance, "a", "a_nycskinhleader2");
		}
	}
	if (!gotleather)
	{
		body->ApplySkin(*monster,  myInstance, "b", "b_nycskinhleader");
		body->ApplySkin(*monster,  myInstance, "a", "a_nycskinhleader");
	}

	bool newface = false;

	if ((gi.irand(0,1)) && body->ApplySkin(*monster,  myInstance, "h", "h_w_1_s"))
	{
		body->SetFace(*monster,"f_w_1_");
		newface = true;
	}
	if (!newface)
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_w_12_s");

		body->SetFace(*monster,"f_w_12_");
	}
	body->SetVoiceDirectories(*monster, "sk", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	gi.effectindex("environ/onfireburst");
}

void zitpunk_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterFaceSkins("f_w_12_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_nycpunk2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycpunk2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_12_s",VARIETY_PRIORITY_HIGHEST);
}

void zitpunk_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	if (!(monster->spawnflags & SPAWNFLAG_NO_WEAPONS))
	{
		body->SetRightHandWeapon(*monster, ATK_PISTOL1);
	}

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", ECTO_GLASSES_NAME, "to_abolt_head_t", myInstance, "acc_glasses_sun", ECTO_GLASSES_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "a", "a_nycpunk2");
	body->ApplySkin(*monster,  myInstance, "b", "b_nycpunk2");

	body->ApplySkin(*monster,  myInstance, "h", "h_w_12_s");
	body->SetFace(*monster,"f_w_12_");

	body->SetVoiceDirectories(*monster, "sk", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void stockbroker_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_nycmalestockbroker",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycmalestockbroker",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycmalestockbrokerb",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_b_1_s",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_b_1_",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_w_2_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_2_",VARIETY_PRIORITY_HIGHEST);
}

void stockbroker_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC*	 myInstance;
	qboolean	altskin=false;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);


	body->AddRandomBoltedItem(0.5, *monster, "wbolt_hand_l", "Enemy/Bolt", "w_briefcase", "to_wbolt_hand_l", myInstance, "rubbish", ECTO_WEAPON_SCALE);

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", ECTO_GLASSES_NAME, "to_abolt_head_t", myInstance, "acc_glasses_norm", ECTO_GLASSES_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_nycmalestockbroker");

	if (!strcmp(subclass,"stockbroke2"))//is a black stockbroker
	{
		altskin=body->ApplySkin(*monster,  myInstance, "a", "a_nycmalestockbrokerb")
			&& body->ApplySkin(*monster,  myInstance, "h", "h_b_1_s");
	}
	if (altskin)
	{
		body->SetFace(*monster,"f_b_1_");
	}
	else
	{
		GhoulID bang;
		bang = MyGhoulObj->GetMyObject()->FindPart("_lhairbang");
		if (rand()%2 && bang)
		{
			myInstance->GetInstPtr()->SetPartOnOff(bang, false);
		}
		bang = MyGhoulObj->GetMyObject()->FindPart("_rhairbang");
		if (rand()%2 && bang)
		{
			myInstance->GetInstPtr()->SetPartOnOff(bang, false);
		}

		body->ApplySkin(*monster,  myInstance, "a", "a_nycmalestockbroker");
		body->ApplySkin(*monster,  myInstance, "h", "h_w_2_c");
		body->SetFace(*monster,"f_w_2_");
	}

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void bum_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_nycbum",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycbum",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("hb","hb_h_1_b",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_hb_1_",VARIETY_PRIORITY_HIGHEST);
}

void bum_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_flop", "to_abolt_head_t", myInstance, "acc_hat_flop_brwn", ECTO_HAT_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_nycbum");
	body->ApplySkin(*monster,  myInstance, "a", "a_nycbum");
	body->ApplySkin(*monster,  myInstance, "hb", "hb_h_1_b");
	body->SetFace(*monster,"f_hb_1_");

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void tourist_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_dcmaletourist",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_dcmaletourist",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_4_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_4_",VARIETY_PRIORITY_HIGHEST);
}

void tourist_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);


	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", ECTO_GLASSES_NAME, "to_abolt_head_t", myInstance, "acc_glasses_norm", ECTO_GLASSES_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_dcmaletourist");
	body->ApplySkin(*monster,  myInstance, "a", "a_dcmaletourist");
	body->ApplySkin(*monster,  myInstance, "h", "h_w_4_s");

	body->SetFace(*monster,"f_w_4_");

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void commander_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_ugncommander",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_ugncommander",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_2_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_b_2_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_1_s",VARIETY_PRIORITY_LOWEST);
	RegisterFaceSkins("f_b_1_",VARIETY_PRIORITY_LOWEST);
}

void commander_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL2);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_general", "to_abolt_head_t", myInstance, "acc_hat_general_brwn", ECTO_HAT_SCALE);

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", ECTO_GLASSES_NAME, "to_abolt_head_t", myInstance, "acc_glasses_sun", ECTO_GLASSES_SCALE);
	body->AddRandomBoltedItem(0.5, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_brwn");
	body->AddRandomBoltedItem(0.5, *monster, "abolt_thigh_l", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_l", myInstance, "acc_pouch_thigh_tan", ECTO_POUCH_SCALE);
	body->AddRandomBoltedItem(0.5, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_brwn", ECTO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_ugncommander");
	body->ApplySkin(*monster,  myInstance, "a", "a_ugncommander");

	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_b_1_s"))
	{
		body->SetFace(*monster,"f_b_1_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_b_2_s");
		body->SetFace(*monster,"f_b_2_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void factoryworker_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_ugnfactoryworker",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_ugnfactoryworker",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_4_c",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_b_4_",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_b_3_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_b_3_",VARIETY_PRIORITY_HIGHEST);
}

void factoryworker_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL2);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_top", "to_abolt_head_t", myInstance, "acc_hat_top_wht", ECTO_HAT_SCALE);
	body->AddRandomBoltedItem(0.5, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_blck", ECTO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.5, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_r", myInstance, "acc_box_hip_smll_blck", ECTO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_ugnfactoryworker");
	body->ApplySkin(*monster,  myInstance, "a", "a_ugnfactoryworker");

	if (gi.irand(0,1) && body->ApplySkin(*monster,  myInstance, "h", "h_b_4_c"))
	{
		body->SetFace(*monster,"f_b_4_");
		monster->ghoulInst->SetPartOnOff("_headcrew",false);
//		monster->ghoulInst->SetPartOnOff("_headbald",false);
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_b_3_s");
		body->SetFace(*monster,"f_b_3_");
		monster->ghoulInst->SetPartOnOff("_headcrew",false);
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}


void irqcitizen_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqmalecitizen",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqmalecitizen",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_2_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_2_",VARIETY_PRIORITY_HIGHEST);
//	MyGhoulObj->RegisterSkin("hb","hb_h_1_b");
//	RegisterFaceSkins("f_hb_1_");
}

void irqcitizen_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", ECTO_GLASSES_NAME, "to_abolt_head_t", myInstance, "acc_glasses_norm", ECTO_GLASSES_SCALE);

	if (!body->AddRandomBoltedItem(0.3333333, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_turbin", "to_abolt_head_t", myInstance, "acc_hat_turbin_wht", ECTO_HAT_SCALE))
	{
		body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_top", "to_abolt_head_t", myInstance, "acc_hat_top_wht", ECTO_HAT_SCALE);
	}

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_irqmalecitizen");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqmalecitizen");
//	if (gi.irand(0,1))
//	{
		myInstance->GetInstPtr()->SetPartOnOff("_headbeard",false);
		body->ApplySkin(*monster,  myInstance, "h", "h_m_2_s");
		body->SetFace(*monster,"f_m_2_");
//	}
//	else
//	{
//		myInstance->GetInstPtr()->SetPartOnOff("_headbald",false);
//		body->ApplySkin(*monster,  myInstance, "hb", "hb_h_1_b");
//		body->SetFace(*monster,"f_hb_1_");
//	}

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void prisoner1_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_colmaleprisoner1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_colmaleprisoner1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("hb","hb_h_1_b",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_hb_1_",VARIETY_PRIORITY_HIGHEST);
}

void prisoner1_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_flop", "to_abolt_head_t", myInstance, "acc_hat_flop_brwn", ECTO_HAT_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_colmaleprisoner1");
	body->ApplySkin(*monster,  myInstance, "a", "a_colmaleprisoner1");
	body->ApplySkin(*monster,  myInstance, "hb", "hb_h_1_b");
	body->SetFace(*monster,"f_hb_1_");

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void prisoner2_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_colmaleprisoner2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_colmaleprisoner2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_h_1_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_h_1_",VARIETY_PRIORITY_HIGHEST);
}

void prisoner2_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_colmaleprisoner2");
	body->ApplySkin(*monster,  myInstance, "a", "a_colmaleprisoner2");
	body->ApplySkin(*monster,  myInstance, "h", "h_h_1_c");
	body->SetFace(*monster,"f_h_1_");

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void iraqofficer_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqofficer",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqofficer",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_1_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_1_",VARIETY_PRIORITY_HIGHEST);
}

void iraqofficer_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL2);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_beret", "to_abolt_head_t", myInstance, "acc_hat_beret_blck", ECTO_HAT_SCALE);

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", ECTO_GLASSES_NAME, "to_abolt_head_t", myInstance, "acc_glasses_sun", ECTO_GLASSES_SCALE);
	body->AddRandomBoltedItem(0.5, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_brwn", ECTO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.5, *monster, "abolt_thigh_l", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_l", myInstance, "acc_pouch_thigh_blck", ECTO_POUCH_SCALE);
	body->AddRandomBoltedItem(0.5, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_brwn", ECTO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_irqofficer");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqofficer");

	body->ApplySkin(*monster,  myInstance, "h", "h_m_1_s");
	body->SetFace(*monster,"f_m_1_");

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void serbofficer_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_kosofficer",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_kosofficer",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_5_c",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_w_5_",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_w_2_c",VARIETY_PRIORITY_LOWEST);
	RegisterFaceSkins("f_w_2_",VARIETY_PRIORITY_LOWEST);
	RegisterSkin("h","h_h_6_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_h_6_",VARIETY_PRIORITY_HIGHEST);
}

void serbofficer_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;


	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL2);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_general2", "to_abolt_head_t", myInstance, "acc_hat_general2_grn", ECTO_HAT_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_brwn", ECTO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_kosofficer");
	body->ApplySkin(*monster,  myInstance, "a", "a_kosofficer");

	if (!gi.irand(0,2)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_2_c"))
	{
		body->SetFace(*monster,"f_w_2_");
	}
	else if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_5_c"))
	{
		body->SetFace(*monster,"f_w_5_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_h_6_s");
		body->SetFace(*monster,"f_h_6_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void kosrefugee_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_kosrefugee",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_kosrefugee",VARIETY_PRIORITY_HIGHEST);
//	MyGhoulObj->RegisterSkin("hb","hb_h_1_b");
//	RegisterFaceSkins("f_hb_1_");
	RegisterSkin("h","h_w_4_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_4_",VARIETY_PRIORITY_HIGHEST);
}

void kosrefugee_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL2);
//	body->SetLeftHandWeapon(*monster, ATK_MOLITOV);

	body->AddRandomBoltedItem(0.2, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_baseball", "to_abolt_head_t_fwd", myInstance, "acc_hat_baseball_tan", ECTO_HAT_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_brwn", ECTO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_kosrefugee");
	body->ApplySkin(*monster,  myInstance, "a", "a_kosrefugee");

//	if (gi.irand(0,1))
//	{
		myInstance->GetInstPtr()->SetPartOnOff("_headbeard",false);
		body->ApplySkin(*monster,  myInstance, "h", "h_w_4_s");
		body->SetFace(*monster,"f_w_4_");
//	}
//	else
//	{
//		myInstance->GetInstPtr()->SetPartOnOff("_headbald",false);
//		body->ApplySkin(*monster,  myInstance, "hb", "hb_h_1_b");
//		body->SetFace(*monster,"f_hb_1_");
//		break;
//	}

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
	gi.effectindex("environ/onfireburst");
}

void chemist_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_colchemist",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_colchemist",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_h_1_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_h_1_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_h_2_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_h_2_",VARIETY_PRIORITY_HIGHEST);
}

void chemist_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL2);

	if (!body->AddRandomBoltedItem(0.3333333, *monster, "abolt_head_t", "Enemy/Bolt", ECTO_GLASSES_NAME, "to_abolt_head_t", myInstance, "acc_glasses_norm", ECTO_GLASSES_SCALE))
	{
		body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", ECTO_GLASSES_NAME, "to_abolt_head_t", myInstance, "acc_glasses_sun", ECTO_GLASSES_SCALE);
	}

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_top", "to_abolt_head_t", myInstance, "acc_hat_top_wht", ECTO_HAT_SCALE);

	body->AddRandomBoltedItem(0.5, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_blck", ECTO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.5, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_r", myInstance, "acc_box_hip_smll_blck", ECTO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_colchemist");
	body->ApplySkin(*monster,  myInstance, "a", "a_colchemist");

	if (!strcmp(subclass,"chemist2")&&body->ApplySkin(*monster,  myInstance, "h", "h_h_1_c"))
	{
		body->SetFace(*monster,"f_h_1_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_h_2_s");
		body->SetFace(*monster,"f_h_2_");
	}

	body->SetVoiceDirectories(*monster, "soldier", 2, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void sibsuit_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_sibsuit",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_sibsuit",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_5_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_5_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_3_",VARIETY_PRIORITY_HIGH);
	RegisterSkin("h","h_w_3_s",VARIETY_PRIORITY_HIGH);
}

void sibsuit_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	qboolean imBald;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL1);

	body->AddRandomBoltedItem(0.5, *monster, "wbolt_hand_l", "Enemy/Bolt", "w_briefcase", "to_wbolt_hand_l", myInstance, "rubbish", ECTO_WEAPON_SCALE);

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", ECTO_GLASSES_NAME, "to_abolt_head_t", myInstance, "acc_glasses_norm", ECTO_GLASSES_SCALE);

	imBald=(NULL != body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_tall", "to_abolt_head_t", myInstance, "acc_hat_tall_gry", ECTO_HAT_SCALE));

	body->AddRandomBoltedItem(0.5, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_blck", ECTO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.5, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_holster", "to_abolt_hip_r", myInstance, "acc_holster_blck", ECTO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_sibsuit");
	body->ApplySkin(*monster,  myInstance, "a", "a_sibsuit");

	if (!strcmp(subclass,"sibsuit2") && body->ApplySkin(*monster,  myInstance, "h", "h_w_3_s"))
	{
		imBald=true;
		body->SetFace(*monster,"f_w_3_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_w_5_c");
		body->SetFace(*monster,"f_w_5_");
	}

	if (imBald)
	{
		myInstance->GetInstPtr()->SetPartOnOff("_headcrew",false);
	}
	else
	{
		myInstance->GetInstPtr()->SetPartOnOff("_headbald",false);
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void sibscientist_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_sibscientist",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_sibscientist",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_2_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_2_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_1_",VARIETY_PRIORITY_LOWEST);
	RegisterSkin("h","h_w_1_s",VARIETY_PRIORITY_LOWEST);
}

void sibscientist_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_ecto_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", ECTO_GLASSES_NAME, "to_abolt_head_t", myInstance, "acc_glasses_norm", ECTO_GLASSES_SCALE);

	body->AddRandomBoltedItem(0.5, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_blck", ECTO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.5, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_r", myInstance, "acc_box_hip_smll_blck", ECTO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_sibscientist");
	body->ApplySkin(*monster,  myInstance, "a", "a_sibscientist");

	if (!strcmp(subclass,"sibscience2")&&body->ApplySkin(*monster,  myInstance, "h", "h_w_1_s"))
	{
		body->SetFace(*monster,"f_w_1_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_w_2_c");
		body->SetFace(*monster,"f_w_2_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void SP_m_nyc_estockbroker (edict_t *self)
{
	generic_monster_spawnnow(self);
	if (rand()%2)
	{
		self->ai = ai_c::Create(AI_ECTO_STOCKBROKER, self, "enemy/ecto", "stockbroke");
	}
	else
	{
		self->ai = ai_c::Create(AI_ECTO_STOCKBROKER, self, "enemy/ecto", "stockbroke2");
	}
	self->think = generic_npc_init;
}

void SP_m_nyc_etourist (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->think = generic_npc_init;
	self->ai = ai_c::Create(AI_ECTO_DCTOURIST, self, "enemy/ecto", "tourist");
}

void SP_m_nyc_epunk (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->think = generic_grunt_init;
	self->ai = ai_c::Create(AI_ECTO_PUNK2, self, "enemy/ecto", "zitpunk");
}

void SP_m_nyc_eskinhead3 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_ECTO_SKINLEADER, self, "enemy/ecto", "skinleader");
	self->think = generic_grunt_init;
}

void SP_m_afr_eworker (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_ECTO_FACTORY, self, "enemy/ecto", "factory");
	self->think = generic_grunt_init;
}

void SP_m_kos_eofficer (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_ECTO_SERBOFFICER, self, "enemy/ecto", "serbofficer");
	self->think = generic_leader_init;
}

void SP_m_kos_erefugee (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_ECTO_KOSREFUGEE, self, "enemy/ecto", "kosrefugee");
	self->think = generic_leader_init;
}

void SP_m_irq_eofficer (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_ECTO_IRAQOFFICER, self, "enemy/ecto", "iraqofficer");
	self->think = generic_leader_init;
}


void SP_m_afr_ecommander (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_ECTO_UGNCOMMANDER, self, "enemy/ecto", "commander");
	self->think = generic_leader_init;
}

void SP_m_nyc_ebum (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_ECTO_NYCBUM, self, "enemy/ecto", "bum");
	self->think = generic_npc_init;
}

void SP_m_irq_eman1 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_ECTO_IRAQCITIZEN, self, "enemy/ecto", "irqcitizen");
	self->think = generic_npc_init;
}

void SP_m_sib_eofficer (edict_t *self)
{
	generic_monster_spawnnow(self);
	if (rand()&1)
	{
		self->ai = ai_c::Create(AI_ECTO_SIBSUIT, self, "enemy/ecto", "sibsuit1");
	}
	else
	{
		self->ai = ai_c::Create(AI_ECTO_SIBSUIT, self, "enemy/ecto", "sibsuit2");
	}
	self->think = generic_leader_init;
}

void SP_m_sib_escientist1 (edict_t *self)
{
	generic_monster_spawnnow(self);
	if (rand()&1)
	{
		self->ai = ai_c::Create(AI_ECTO_SIBSCIENCE, self, "enemy/ecto", "sibscience1");
	}
	else
	{
		self->ai = ai_c::Create(AI_ECTO_SIBSCIENCE, self, "enemy/ecto", "sibscience2");
	}
	self->think = generic_leader_init;
}










