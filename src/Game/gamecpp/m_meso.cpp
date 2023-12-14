#include "g_local.h"
#include "ai_private.h"
#include "m_generic.h"
#include "m_meso.h"

#define MESO_POUCH_SCALE 1.142857142857
#define MESO_HAT_SCALE 1.142857142857
#define MESO_GLASSES_SCALE 1.142857142857
#define MESO_WEAPON_SCALE 1.0 
#define MESO_OTHER_SCALE 1.142857142857 

// NOTE
// This is now generated in code, so hand editing is probably a bad idea...
//

CAISkills enemySkills[] =
{//			  rangeMin			dodge			hesitation		cashValue				flags
 //						rangeMax		accuracy		aimTime				turnSpeed
	CAISkills(192.00,	512.00,	0.60,	2.50,	0.50,	2.00,	   64.00,	2.00,	     0),//AI_ECTO_SKINLEADER :: PISTOL1
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_ECTO_STOCKBROKER
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_ECTO_DCTOURIST
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_ECTO_NYCBUM
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_ECTO_COLPRISONER1
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_ECTO_COLPRISONER2
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_ECTO_IRAQCITIZEN
	CAISkills(57.60,	512.00,	0.20,	1.25,	0.38,	1.50,	  112.50,	1.50,	     0),//AI_ECTO_UGNCOMMANDER :: PISTOL2
	CAISkills(192.00,	512.00,	0.30,	1.25,	0.50,	3.00,	   31.25,	1.50,	     0),//AI_ECTO_SERBOFFICER :: PISTOL2
	CAISkills(192.00,	512.00,	0.30,	1.25,	0.50,	3.00,	   31.25,	1.50,	     0),//AI_ECTO_KOSREFUGEE :: PISTOL2
	CAISkills(192.00,	512.00,	0.30,	1.25,	0.38,	1.00,	  162.50,	1.50,	     0),//AI_ECTO_IRAQOFFICER :: PISTOL2
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_ECTO_FACTORY :: PISTOL2
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_ECTO_CHEMIST :: PISTOL2
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	2.50,	   42.00,	2.00,	     0),//AI_ECTO_SIBSUIT :: PISTOL1
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_ECTO_SIBSCIENCE
	CAISkills(192.00,	512.00,	0.60,	2.50,	0.50,	2.00,	   64.00,	2.00,	     0),//AI_ECTO_PUNK2 :: PISTOL1

	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_JOHN :: PISTOL1
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_JOHN_SNOW :: PISTOL1
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_JOHN_DESERT :: PISTOL1
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_HAWK :: MPISTOL
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_HURTHAWK
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_SAM
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_IRAQWORKER
	CAISkills(144.00,	192.00,	0.00,	0.25,	1.00,	2.00,	   96.00,	0.60,	    16),//AI_MESO_NYCPUNK :: SHOTGUN
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_AMU
	CAISkills(192.00,	512.00,	0.70,	1.25,	0.38,	1.00,	  275.00,	1.50,	     5),//AI_MESO_RAIDERBOSS :: PISTOL2
	CAISkills(192.00,	384.00,	0.28,	0.50,	0.75,	1.00,	 2750.00,	0.60,	    37),//AI_MESO_RAIDERBOSS2 :: MICRO
	CAISkills(256.00,	512.00,	0.18,	0.50,	0.75,	1.00,	  546.00,	1.50,	    10),//AI_MESO_IRAQSOLDIER1 :: SNIPER
	CAISkills(192.00,	384.00,	0.18,	0.13,	0.75,	2.00,	  403.00,	1.00,	    16),//AI_MESO_IRAQSOLDIER2 :: ASSAULT
	CAISkills(192.00,	384.00,	0.18,	0.25,	1.13,	2.00,	  936.00,	1.00,	    16),//AI_MESO_IRAQSOLDIER2B :: MACHINEG
	CAISkills(192.00,	384.00,	0.18,	0.13,	0.75,	2.00,	  403.00,	1.00,	    16),//AI_MESO_IRAQREPGUARD :: ASSAULT
	CAISkills(192.00,	384.00,	0.18,	0.25,	1.13,	2.00,	  936.00,	1.00,	    16),//AI_MESO_IRAQREPGUARDB :: MACHINEG
	CAISkills(192.00,	512.00,	0.30,	1.25,	0.38,	1.00,	  162.50,	1.50,	     0),//AI_MESO_IRAQPOLICE :: PISTOL2
	CAISkills(192.00,	512.00,	0.30,	1.25,	0.38,	1.00,	  162.50,	1.50,	     0),//AI_MESO_IRAQCOMMANDER :: PISTOL2
	CAISkills(192.00,	384.00,	0.18,	0.13,	0.75,	2.00,	  403.00,	1.00,	    16),//AI_MESO_IRAQBRUTEA :: ASSAULT
	CAISkills(32.00,	128.00,	0.00,	0.50,	0.00,	0.00,	  832.00,	4.00,	   176),//AI_MESO_IRAQBRUTEB :: FLAME
	CAISkills(256.00,	512.00,	0.18,	0.50,	0.75,	1.00,	  546.00,	1.50,	    10),//AI_MESO_IRAQBODYGUARD :: SNIPER
	CAISkills(128.00,	512.00,	0.00,	0.50,	0.75,	1.00,	 1105.00,	0.70,	    53),//AI_MESO_IRAQROCKET :: ROCKET
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_IRAQSADDAM
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_IRAQMAN2
	CAISkills(76.80,	512.00,	0.12,	0.50,	0.75,	1.50,	  378.00,	1.50,	    10),//AI_MESO_UGNSNIPER :: SNIPER
	CAISkills(57.60,	512.00,	0.20,	2.50,	0.38,	1.50,	   90.00,	2.00,	     0),//AI_MESO_UGNSOLDIER1 :: PISTOL1
	CAISkills(43.20,	192.00,	0.00,	0.25,	0.75,	1.50,	  135.00,	0.60,	    16),//AI_MESO_UGNSOLDIER1B :: SHOTGUN
	CAISkills(43.20,	192.00,	0.00,	0.25,	0.75,	1.50,	  135.00,	0.60,	    16),//AI_MESO_UGNSOLDIER2 :: SHOTGUN
	CAISkills(9.60,	128.00,	0.00,	0.50,	0.00,	0.00,	  576.00,	4.00,	   176),//AI_MESO_UGNSOLDIER3 :: FLAME
	CAISkills(192.00,	512.00,	0.60,	2.50,	0.50,	2.00,	   64.00,	2.00,	     0),//AI_MESO_NYCSWATGUY :: PISTOL1
	CAISkills(192.00,	512.00,	0.60,	2.50,	0.50,	2.00,	   64.00,	2.00,	     0),//AI_MESO_NYCSWATLEADER :: PISTOL1
	CAISkills(192.00,	384.00,	0.42,	0.25,	1.13,	2.00,	 1584.00,	1.00,	    21),//AI_MESO_RAIDER1 :: MACHINEG
	CAISkills(192.00,	384.00,	0.42,	0.13,	0.75,	2.00,	  748.00,	1.00,	     5),//AI_MESO_RAIDER2A :: MPISTOL
	CAISkills(192.00,	384.00,	0.42,	0.50,	0.75,	2.00,	 1364.00,	1.00,	     5),//AI_MESO_RAIDER2B :: AUTO
	CAISkills(192.00,	384.00,	0.28,	0.50,	0.75,	1.00,	 2750.00,	0.60,	    37),//AI_MESO_RAIDERBRUTE :: MICRO
	CAISkills(128.00,	512.00,	0.00,	0.50,	0.75,	1.00,	 1870.00,	0.70,	    53),//AI_MESO_RAIDERROCKET :: ROCKET
	CAISkills(192.00,	384.00,	0.18,	0.13,	1.00,	5.00,	  142.80,	1.00,	     0),//AI_MESO_SIBTROOPER2 :: MPISTOL
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	2.50,	   42.00,	2.00,	     0),//AI_MESO_SIBCLEANSUIT :: PISTOL1
	CAISkills(144.00,	192.00,	0.00,	0.25,	1.00,	3.00,	   37.50,	0.60,	    16),//AI_MESO_SERBGRUNT1 :: SHOTGUN
	CAISkills(192.00,	384.00,	0.18,	0.13,	1.00,	6.00,	   77.50,	1.00,	    16),//AI_MESO_SERBGRUNT2 :: ASSAULT
	CAISkills(192.00,	384.00,	0.18,	0.13,	1.00,	6.00,	   77.50,	1.00,	    16),//AI_MESO_SERBGRUNT3 :: ASSAULT
	CAISkills(256.00,	512.00,	0.18,	0.50,	1.00,	3.00,	  105.00,	1.50,	    10),//AI_MESO_SERBSNIPER1A :: SNIPER
	CAISkills(256.00,	512.00,	0.18,	0.50,	1.00,	3.00,	  105.00,	1.50,	    10),//AI_MESO_SERBSNIPER1B :: SNIPER
	CAISkills(192.00,	512.00,	0.30,	1.25,	0.50,	3.00,	   31.25,	1.50,	     0),//AI_MESO_SERBCOMTROOP :: PISTOL2
	CAISkills(192.00,	384.00,	0.18,	0.13,	1.00,	6.00,	   77.50,	1.00,	    16),//AI_MESO_SERBBRUTE1A :: ASSAULT
	CAISkills(192.00,	384.00,	0.18,	0.13,	1.00,	6.00,	   77.50,	1.00,	    16),//AI_MESO_SERBBRUTE1B :: ASSAULT
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_SERBMECHANIC
	CAISkills(192.00,	384.00,	0.18,	0.13,	1.00,	6.00,	   77.50,	1.00,	    16),//AI_MESO_KOSREBEL :: ASSAULT
	CAISkills(192.00,	384.00,	0.18,	0.13,	1.00,	6.00,	   77.50,	1.00,	    16),//AI_MESO_KOSKLAGUY :: ASSAULT
	CAISkills(144.00,	192.00,	0.00,	0.25,	1.00,	2.00,	   96.00,	0.60,	    16),//AI_MESO_SKINHEAD1 :: SHOTGUN
	CAISkills(192.00,	512.00,	0.60,	2.50,	0.50,	2.00,	   64.00,	2.00,	     0),//AI_MESO_SKINHEAD2A :: PISTOL1
	CAISkills(192.00,	384.00,	0.36,	0.13,	1.00,	4.00,	  198.40,	1.00,	    16),//AI_MESO_SKINHEAD2B :: ASSAULT
	CAISkills(192.00,	512.00,	0.60,	1.25,	0.50,	2.00,	   80.00,	1.50,	     0),//AI_MESO_SKINHEADBOSS :: PISTOL2
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_MALEPOLITICIAN
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_TOKMALEHOSTAGE
	CAISkills(192.00,	512.00,	1.00,	2.50,	0.38,	1.00,	  110.00,	2.00,	     0),//AI_MESO_TOKHENCH1 :: PISTOL1
	CAISkills(192.00,	384.00,	0.60,	0.13,	0.75,	2.00,	  341.00,	1.00,	    16),//AI_MESO_TOKHENCH2 :: ASSAULT
	CAISkills(192.00,	384.00,	0.40,	0.50,	0.75,	1.00,	 1375.00,	0.60,	    32),//AI_MESO_TOKKILLER :: MICRO
	CAISkills(192.00,	384.00,	0.60,	0.13,	0.75,	2.00,	  374.00,	1.00,	     0),//AI_MESO_TOKNINJA :: MPISTOL
	CAISkills(192.00,	384.00,	0.60,	0.13,	0.75,	2.00,	  341.00,	1.00,	    16),//AI_MESO_TOKBRUTE :: ASSAULT
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_JAPANSUIT
	CAISkills(57.60,	512.00,	0.20,	1.25,	0.38,	1.50,	  112.50,	1.50,	     0),//AI_MESO_UGNBRUTE :: PISTOL2
	CAISkills(38.40,	512.00,	0.00,	0.50,	0.75,	1.50,	  765.00,	0.70,	    53),//AI_MESO_UGNROCKET :: ROCKET
	CAISkills(192.00,	384.00,	0.18,	0.13,	1.00,	5.00,	  130.20,	1.00,	    16),//AI_MESO_SIBTROOPER1A :: ASSAULT
	CAISkills(256.00,	512.00,	0.18,	0.50,	1.00,	2.50,	  176.40,	1.50,	    10),//AI_MESO_SIBTROOPER1B :: SNIPER
	CAISkills(192.00,	384.00,	0.18,	0.13,	1.00,	5.00,	  130.20,	1.00,	    16),//AI_MESO_SIBGUARD :: ASSAULT
	CAISkills(192.00,	384.00,	0.18,	0.13,	1.00,	5.00,	  142.80,	1.00,	     0),//AI_MESO_SIBGUARD3 :: MPISTOL
	CAISkills(192.00,	384.00,	0.18,	0.13,	1.00,	5.00,	  130.20,	1.00,	    16),//AI_MESO_SIBGUARD4 :: ASSAULT
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_MESO_SIBMECH

	CAISkills(192.00,	512.00,	0.60,	2.50,	0.50,	2.00,	   64.00,	2.00,	     0),//AI_FEMALE_SKINCHICK :: PISTOL1
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_FEMALE_NYWOMAN
	CAISkills(192.00,	384.00,	0.18,	0.13,	1.00,	5.00,	  142.80,	1.00,	     0),//AI_FEMALE_SIBGUARD :: MPISTOL
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_FEMALE_SIBSCIENCE
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_FEMALE_IRQWOMAN1
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_FEMALE_IRQWOMAN2
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_FEMALE_TOKWOMAN1
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_FEMALE_TOKWOMAN2
	CAISkills(192.00,	512.00,	1.00,	2.50,	0.38,	1.00,	  110.00,	2.00,	     0),//AI_FEMALE_TOKASSASSIN :: PISTOL1
	CAISkills(32.00,	128.00,	0.00,	0.50,	0.00,	0.00,	 1408.00,	4.00,	   181),//AI_FEMALE_RAIDER :: FLAME
	CAISkills(192.00,	512.00,	0.30,	2.50,	0.50,	3.00,	   25.00,	2.00,	     0),//AI_FEMALE_TAYLOR :: ASSAULT
};


void john_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterFaceSkins("f_john_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_xjohn",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_xjohn",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_john",VARIETY_PRIORITY_HIGHEST);

	//EH! these guys may have to be registered in the same order they appear in the .ifl!
	RegisterTalkFaceSkins("f_john_n",VARIETY_PRIORITY_HIGHEST);
	RegisterTalkFaceSkins("f_john_m",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_john_nt1",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_john_nt2",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_john_nt3",VARIETY_PRIORITY_HIGHEST);
}

void john_ai::Init(edict_t *monster, char *theclass, char* subclass)
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, theclass, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_backpack_smll", "to_abolt_back", myInstance, "acc_backpack_smll_dgrey", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_flop3", "to_abolt_head_t", myInstance, "acc_hat_flop3", 1.05);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_headset", "to_abolt_head_t", myInstance, "acc_headset", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_canteen1", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_calf_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_blck", MESO_OTHER_SCALE);

	if ( !(monster->spawnflags & SPAWNFLAG_NO_WEAPONS) )
	{
		body->SetRightHandWeapon(*monster, ATK_PISTOL1);
	}

	body->ApplySkin(*monster,  myInstance, "b", "b_xjohn");
	body->ApplySkin(*monster,  myInstance, "a", "a_xjohn");
	body->ApplySkin(*monster,  myInstance, "h", "h_john");
	body->SetFace(*monster,"f_john_");

	body->SetVoiceDirectories(*monster, "jm", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void john_snow_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterFaceSkins("f_john_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_xjohn_snow",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_xjohn_snow",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_john",VARIETY_PRIORITY_HIGHEST);

	//EH! these guys may have to be registered in the same order they appear in the .ifl!
	RegisterTalkFaceSkins("f_john_n",VARIETY_PRIORITY_HIGHEST);
	RegisterTalkFaceSkins("f_john_m",VARIETY_PRIORITY_HIGHEST);
}

void john_snow_ai::Init(edict_t *monster, char *theclass, char* subclass)
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, theclass, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_backpack_lrg", "to_abolt_back", myInstance, "acc_backpack_lrg_blk", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_snow", "to_abolt_head_t", myInstance, "acc_hat_snow", MESO_HAT_SCALE);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_headset", "to_abolt_head_t", myInstance, "acc_headset", MESO_OTHER_SCALE);
//	body->AddBoltedItem(*monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_canteen1", MESO_OTHER_SCALE);
//	body->AddBoltedItem(*monster, "abolt_calf_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_blck", MESO_OTHER_SCALE);

	if ( !(monster->spawnflags & SPAWNFLAG_NO_WEAPONS) )
	{
		body->SetRightHandWeapon(*monster, ATK_PISTOL1);
	}

	body->ApplySkin(*monster,  myInstance, "b", "b_xjohn_snow");
	body->ApplySkin(*monster,  myInstance, "a", "a_xjohn_snow");
	body->ApplySkin(*monster,  myInstance, "h", "h_john");
	body->SetFace(*monster,"f_john_");

	body->SetVoiceDirectories(*monster, "jm", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void john_desert_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterFaceSkins("f_john_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_xjohn_desert",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_xjohn_desert",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_john",VARIETY_PRIORITY_HIGHEST);

	//EH! these guys may have to be registered in the same order they appear in the .ifl!
	RegisterTalkFaceSkins("f_john_n",VARIETY_PRIORITY_HIGHEST);
	RegisterTalkFaceSkins("f_john_m",VARIETY_PRIORITY_HIGHEST);
}

void john_desert_ai::Init(edict_t *monster, char *theclass, char* subclass)
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, theclass, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_backpack_smll", "to_abolt_back", myInstance, "acc_backpack_smll", MESO_OTHER_SCALE);
//	body->AddBoltedItem(*monster, "abolt_thigh_r", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_r", myInstance, "acc_pouch_thigh_tan",MESO_POUCH_SCALE);
//	body->AddBoltedItem(*monster, "abolt_thigh_l", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_l", myInstance, "acc_pouch_thigh_tan",MESO_POUCH_SCALE);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_flop3", "to_abolt_head_t", myInstance, "acc_hat_flop3_desert", 1.05);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_headset", "to_abolt_head_t", myInstance, "acc_headset", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_canteen1", MESO_OTHER_SCALE);
//	body->AddBoltedItem(*monster, "abolt_calf_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_blck", MESO_OTHER_SCALE);

	if ( !(monster->spawnflags & SPAWNFLAG_NO_WEAPONS) )
	{
		body->SetRightHandWeapon(*monster, ATK_PISTOL1);
	}

	body->ApplySkin(*monster,  myInstance, "b", "b_xjohn_desert");
	body->ApplySkin(*monster,  myInstance, "a", "a_xjohn_desert");
	body->ApplySkin(*monster,  myInstance, "h", "h_john");
	body->SetFace(*monster,"f_john_");

	body->SetVoiceDirectories(*monster, "jm", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void hurthawk_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterFaceSkins("f_b_6_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_mercdemo1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_mercdemo1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_6_s",VARIETY_PRIORITY_HIGHEST);

	//EH! these guys may have to be registered in the same order they appear in the .ifl!
	RegisterTalkFaceSkins("f_b_6_n",VARIETY_PRIORITY_HIGHEST);
	RegisterTalkFaceSkins("f_b_6_f",VARIETY_PRIORITY_HIGHEST);
}

void hurthawk_ai::Init(edict_t *monster, char *theclass, char* subclass)
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, theclass, subclass);

//	SetSpecialBuddy(true);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->ApplySkin(*monster,  myInstance, "b", "b_mercdemo1");
	body->ApplySkin(*monster,  myInstance, "a", "a_mercdemo1");
	body->ApplySkin(*monster,  myInstance, "h", "h_b_6_s");
	body->SetFace(*monster,"f_b_6_");

	body->SetVoiceDirectories(*monster, "hawk", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void hawk_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterFaceSkins("f_b_6_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_mercdemo1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_mercdemo1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_6_s",VARIETY_PRIORITY_HIGHEST);

	//EH! these guys may have to be registered in the same order they appear in the .ifl!
	RegisterTalkFaceSkins("f_b_6_n",VARIETY_PRIORITY_HIGHEST);
	RegisterTalkFaceSkins("f_b_6_f",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_b_6_nt1",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_b_6_nt2",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_b_6_nt3",VARIETY_PRIORITY_HIGHEST);
}

void hawk_ai::Init(edict_t *monster, char *theclass, char* subclass)
{
	ggOinstC* myInstance;

	monster->flags |= FL_NO_KNOCKBACK;

	generic_meso_ai::Init(monster, theclass, subclass);

	SetSpecialBuddy(true);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_backpack_lrg", "to_abolt_back", myInstance, "acc_backpack_lrg_tan", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_sun", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_headset", "to_abolt_head_t", myInstance, "acc_headset", MESO_OTHER_SCALE);

	if ( !(monster->spawnflags & SPAWNFLAG_NO_WEAPONS) )
	{
		body->SetRightHandWeapon(*monster, ATK_MACHINEPISTOL);
	}

	body->ApplySkin(*monster,  myInstance, "b", "b_mercdemo1");
	body->ApplySkin(*monster,  myInstance, "a", "a_mercdemo1");
	body->ApplySkin(*monster,  myInstance, "h", "h_b_6_s");
	body->SetFace(*monster,"f_b_6_");

	body->SetVoiceDirectories(*monster, "hawk", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void sam_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterFaceSkins("f_sam_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_xsam",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_xsam",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_sam",VARIETY_PRIORITY_HIGHEST);

	//EH! these guys may have to be registered in the same order they appear in the .ifl!
	RegisterTalkFaceSkins("f_sam_n",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_sam_nt1",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_sam_nt2",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_sam_nt3",VARIETY_PRIORITY_HIGHEST);
}

void sam_ai::Init(edict_t *monster, char *theclass, char* subclass)
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, theclass, subclass);

	SetSpecialBuddy(true);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_norm", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_xsam");
	body->ApplySkin(*monster,  myInstance, "a", "a_xsam");
	body->ApplySkin(*monster,  myInstance, "h", "h_sam");
	body->SetFace(*monster,"f_sam_");

	body->SetVoiceDirectories(*monster, "sam", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void skinheadboss_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_nycskinboss",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycskinboss",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_8_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_8_",VARIETY_PRIORITY_HIGHEST);

	RegisterTalkFaceSkins("f_w_8_m",VARIETY_PRIORITY_HIGHEST);
}

void skinheadboss_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	scale = 1.1;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	if ( !(monster->spawnflags & SPAWNFLAG_NO_WEAPONS) )
	{
		body->SetRightHandWeapon(*monster, ATK_PISTOL2);
		body->AddBoltedItem(*monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_blck", MESO_OTHER_SCALE);
	}


	body->ApplySkin(*monster,  myInstance, "b", "b_nycskinboss");
	body->ApplySkin(*monster,  myInstance, "a", "a_nycskinboss");
	body->ApplySkin(*monster,  myInstance, "h", "h_w_8_s");
	body->SetFace(*monster,"f_w_8_");

	body->SetVoiceDirectories(*monster, "sk", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void amu_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_xamu",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_xamu",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_amu",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_amu_",VARIETY_PRIORITY_HIGHEST);

	//EH! these guys may have to be registered in the same order they appear in the .ifl!
	RegisterTalkFaceSkins("f_amu_f",VARIETY_PRIORITY_HIGHEST);
	RegisterTalkFaceSkins("f_amu_n",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_amu_ft1",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_amu_ft2",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("f","f_amu_ft3",VARIETY_PRIORITY_HIGHEST);
}

void amu_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddBoltedItem(*monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_brwn", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_beret", "to_abolt_head_t", myInstance, "acc_hat_beret_red",MESO_HAT_SCALE+0.04);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_amu_head", "to_abolt_head_t", myInstance, "rubbish", MESO_OTHER_SCALE+0.045);

	body->ApplySkin(*monster,  myInstance, "b", "b_xamu");
	body->ApplySkin(*monster,  myInstance, "a", "a_xamu");
	body->ApplySkin(*monster,  myInstance, "h", "h_amu");
	body->SetFace(*monster,"f_amu_");

	body->SetVoiceDirectories(*monster, "soldier", 2, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}


void raiderboss_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_xdekker",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_xdekker",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_dekker",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_dekker",VARIETY_PRIORITY_HIGHEST);
}


void raiderboss_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	scale = 1.1;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL2);

	body->ApplySkin(*monster,  myInstance, "b", "b_xdekker");
	body->ApplySkin(*monster,  myInstance, "a", "a_xdekker");
	body->ApplySkin(*monster,  myInstance, "h", "h_dekker");
	body->ApplySkin(*monster,  myInstance, "f", "f_dekker");

	body->SetVoiceDirectories(*monster, "soldier", 2, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
	body->SetArmor(*monster, ARMOR_FULL);
}

void raiderboss_ai::AddBody(edict_t *monster)
{
	if (!monster)
	{
		return;
	}
	body = new bodydekker_c();
}

void raiderboss2_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_xdekker",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_xdekker",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_dekker",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_dekker",VARIETY_PRIORITY_HIGHEST);
}


void raiderboss2_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	scale = 1.1;
	stepheight = 32.0F;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	monster->ghoulInst->SetSpeed(gsOne);

	if ( !(monster->spawnflags & SPAWNFLAG_NO_WEAPONS) )
	{
		body->SetRightHandWeapon(*monster, ATK_DEKKER);
		body->SetLeftHandWeapon(*monster, ATK_GRENADE);
	}

	body->ApplySkin(*monster,  myInstance, "b", "b_xdekker");
	body->ApplySkin(*monster,  myInstance, "a", "a_xdekker");
	body->ApplySkin(*monster,  myInstance, "h", "h_dekker");
	body->ApplySkin(*monster,  myInstance, "f", "f_dekker");

	body->SetVoiceDirectories(*monster, "soldier", 2, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	monster->flags |= FL_IAMTHEBOSS;

	body->SetArmor(*monster, ARMOR_FULL);
	body->SetArmor(*monster, ARMOR_TOUGH);
	monster->flags |= FL_NO_KNOCKBACK;

	gi.effectindex("environ/dekarmor1");
}

void raiderboss2_ai::Think(edict_t &monster)
{
	ai_c::Think(monster);
}

void raiderboss2_ai::AddBody(edict_t *monster)
{
	if (!monster)
	{
		return;
	}
	body = new bodydekker_c();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void skinhead1_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_nycskinhead1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycskinhead1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_nycskinhead1b",VARIETY_PRIORITY_LOW);
	RegisterSkin("a","a_nycskinhead1b",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_w_1_s",VARIETY_PRIORITY_LOWEST);
	RegisterFaceSkins("f_w_1_",VARIETY_PRIORITY_LOWEST);
	RegisterSkin("h","h_w_13_s",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_w_13_",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_w_3_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_3_",VARIETY_PRIORITY_HIGHEST);
}

void skinhead1_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC*	myInstance;
	qboolean	altSkin=false;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_SHOTGUN);
//	body->SetLeftHandWeapon(*monster, ATK_MOLITOV);

//	GetMySkills()->setSkills(128, 192, 0, 0, .2, 0, 0, 128);
	
	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_sun", MESO_GLASSES_SCALE);

	if (gi.irand(0,1))
	{
		altSkin=body->ApplySkin(*monster,  myInstance, "b", "b_nycskinhead1b")
			&& body->ApplySkin(*monster,  myInstance, "a", "a_nycskinhead1b");
	}

	if (altSkin)
	{
		if (!body->AddRandomBoltedItem(0.125, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_baseball", "to_abolt_head_t_bkwd", myInstance, "acc_hat_baseball_grcamo", MESO_OTHER_SCALE))
			body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_baseball", "to_abolt_head_t_bkwd", myInstance, "acc_hat_baseball_dgrey", MESO_OTHER_SCALE);
	}
	else
	{
		body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_baseball", "to_abolt_head_t_bkwd", myInstance, "acc_hat_baseball_dgrey", MESO_OTHER_SCALE);
		body->ApplySkin(*monster,  myInstance, "b", "b_nycskinhead1");
		body->ApplySkin(*monster,  myInstance, "a", "a_nycskinhead1");
	}

	altSkin=false;

	if (!gi.irand(0,2))
	{
		altSkin=body->ApplySkin(*monster,  myInstance, "h", "h_w_1_s");
	}
	if (altSkin)
	{
		body->SetFace(*monster,"f_w_1_");
	}
	else
	{
		if (!gi.irand(0,1))
		{
			altSkin=body->ApplySkin(*monster,  myInstance, "h", "h_w_13_s");
		}
		if (altSkin)
		{
			body->SetFace(*monster,"f_w_13_");
		}
		else
		{
			body->ApplySkin(*monster,  myInstance, "h", "h_w_3_s");
			body->SetFace(*monster,"f_w_3_");
		}
	}

	body->SetVoiceDirectories(*monster, "sk", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
	gi.effectindex("environ/onfireburst");
	gi.effectindex("environ/takeleak");		// Skinheads can take the piss, as it were... :-/
}

void skinhead2a_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_nycskinhead2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycskinhead2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_nycskinhead2b",VARIETY_PRIORITY_LOW);
	RegisterSkin("a","a_nycskinhead2b",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_w_11_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_11_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_6_s",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_w_6_",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_w_9_s",VARIETY_PRIORITY_HIGH);
	RegisterFaceSkins("f_w_9_",VARIETY_PRIORITY_HIGH);
}

void skinhead2a_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);
	qboolean altskin=false;

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	if ( !(monster->spawnflags & SPAWNFLAG_NO_WEAPONS) )
	{
		body->SetRightHandWeapon(*monster, ATK_PISTOL1);
//		body->SetLeftHandWeapon(*monster, ATK_MOLITOV);
	}
//	GetMySkills()->setSkills(256, 512, .5, .5, 1.0, .5, .5, 512);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_sun", MESO_GLASSES_SCALE);

	if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "b", "b_nycskinhead2b"))
	{
		body->ApplySkin(*monster,  myInstance, "a", "a_nycskinhead2b");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "b", "b_nycskinhead2");
		body->ApplySkin(*monster,  myInstance, "a", "a_nycskinhead2");
	}

	if (gi.irand(0,1))
	{
		altskin=body->ApplySkin(*monster,  myInstance, "h", "h_w_6_s");
	}
	if (altskin)
	{
		body->SetFace(*monster,"f_w_6_");
	}
	else
	{
		if (!gi.irand(0,2))
		{
			altskin = body->ApplySkin(*monster,  myInstance, "h", "h_w_9_s");
		}
		if (altskin)
		{
			body->SetFace(*monster,"f_w_9_");
		}
		else
		{
			body->ApplySkin(*monster,  myInstance, "h", "h_w_11_c");
			myInstance->GetInstPtr()->SetPartOnOff("_baldhead", false);
			myInstance->GetInstPtr()->SetPartOnOff("_crewhead", true);
			body->SetFace(*monster,"f_w_11_");
		}
	}

	body->SetVoiceDirectories(*monster, "sk", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
	gi.effectindex("environ/onfireburst");
}

void skinhead2b_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_nycskinhead2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycskinhead2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_nycskinhead2b",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycskinhead2b",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_11_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_11_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_6_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_6_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_9_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_9_",VARIETY_PRIORITY_HIGHEST);
}

void skinhead2b_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;
	qboolean altskin=false;
	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);
//	body->SetLeftHandWeapon(*monster, ATK_MOLITOV);

//	GetMySkills()->setSkills(192, 384, .3, .1, .7, .7, .7, 128);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_sun", MESO_GLASSES_SCALE);

	if (rand()%2&&body->ApplySkin(*monster,  myInstance, "b", "b_nycskinhead2"))
	{
		body->ApplySkin(*monster,  myInstance, "a", "a_nycskinhead2");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "b", "b_nycskinhead2b");
		body->ApplySkin(*monster,  myInstance, "a", "a_nycskinhead2b");
	}

	if (!gi.irand(0,2))
	{
		altskin = body->ApplySkin(*monster,  myInstance, "h", "h_w_11_c");
	}
	if (altskin)
	{
			myInstance->GetInstPtr()->SetPartOnOff("_baldhead", false);
			myInstance->GetInstPtr()->SetPartOnOff("_crewhead", true);
			body->SetFace(*monster,"f_w_11_");
	}
	else
	{
		if (gi.irand(0,1))
		{
			altskin=body->ApplySkin(*monster,  myInstance, "h", "h_w_6_s");
		}
		if (altskin)
		{
			body->SetFace(*monster,"f_w_6_");
		}
		else
		{
			body->ApplySkin(*monster,  myInstance, "h", "h_w_9_s");
			body->SetFace(*monster,"f_w_9_");
		}
	}

	body->SetVoiceDirectories(*monster, "sk", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
	gi.effectindex("environ/onfireburst");
}

void nypunk_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_nycpunk1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycpunk1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_3_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_3_",VARIETY_PRIORITY_HIGHEST);
}

void nypunk_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_SHOTGUN);

//	GetMySkills()->setSkills(128, 256, .2, .3, 1.0, 0, .8, 128);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_sun",MESO_GLASSES_SCALE);
	body->AddBoltedItem(*monster, "abolt_shoulder_r", "Enemy/Bolt", "acc_spikes_shlder", "to_abolt_shoulder_r", myInstance, "acc_spikes_shlder", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_shoulder_l", "Enemy/Bolt", "acc_spikes_shlder", "to_abolt_shoulder_l", myInstance, "acc_spikes_shlder", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_nycpunk1");
	body->ApplySkin(*monster,  myInstance, "a", "a_nycpunk1");

	body->ApplySkin(*monster,  myInstance, "h", "h_w_3_s");
	body->SetFace(*monster,"f_w_3_");

	body->SetVoiceDirectories(*monster, "sk", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void nyswatguy_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_nycswat",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycswat2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_3_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_3_",VARIETY_PRIORITY_HIGHEST);
}

void nyswatguy_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body || !monster->ghoulInst)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL1);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_helmet2", "to_abolt_head_t", myInstance, "acc_helmet2", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_goggles", "to_abolt_head_t", myInstance, "acc_goggles", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_blck", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_blck", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_nycswat");

	body->ApplySkin(*monster,  myInstance, "a", "a_nycswat2");
	body->ApplySkin(*monster,  myInstance, "h", "h_w_3_s");
	body->SetFace(*monster,"f_w_3_");

	body->SetVoiceDirectories(*monster, "sw", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_CHEST);
}

void nyswatleader_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_nycswat",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_nycswat",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_4_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_b_4_",VARIETY_PRIORITY_HIGHEST);
	RegisterTalkFaceSkins("f_b_4_n",VARIETY_PRIORITY_HIGHEST);
}

void nyswatleader_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	SetSpecialBuddy(true);

	if (!body || !monster->ghoulInst)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL1);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_helmet2", "to_abolt_head_t", myInstance, "acc_helmet2", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_goggles", "to_abolt_head_t", myInstance, "acc_goggles", MESO_OTHER_SCALE);

	body->AddBoltedItem(*monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_blck", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_blck", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_nycswat");

	body->ApplySkin(*monster,  myInstance, "a", "a_nycswat");
	body->ApplySkin(*monster,  myInstance, "h", "h_b_4_c");
	body->SetFace(*monster,"f_b_4_");

	body->SetVoiceDirectories(*monster, "sw", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_CHEST);
}

void malepolitician_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_dcmalepol",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_dcmalepol",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_2_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_b_2_",VARIETY_PRIORITY_HIGHEST);
}

void malepolitician_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddRandomBoltedItem(0.5, *monster, "wbolt_hand_l", "Enemy/Bolt", "w_briefcase", "to_wbolt_hand_l", myInstance, "rubbish", MESO_WEAPON_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_norm",MESO_GLASSES_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_dcmalepol");
	body->ApplySkin(*monster,  myInstance, "a", "a_dcmalepol");

	body->ApplySkin(*monster,  myInstance, "h", "h_b_2_s");
	body->SetFace(*monster,"f_b_2_");

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ugsoldier1_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_ugnsoldier1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_ugnsoldier1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_5_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_b_5_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_4_c",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_b_4_",VARIETY_PRIORITY_LOW);
}

void ugsoldier1_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	qboolean hasHat = false;
	qboolean isBald = false;
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body || !monster->ghoulInst)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL1);

	//fixme--this hat is a screwy screwy thing indeed.
	hasHat=body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_flop2", "to_abolt_head_t", myInstance, "acc_hat_flop2_tan", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_brwn", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_brwn", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_ugnsoldier1");
	body->ApplySkin(*monster,  myInstance, "a", "a_ugnsoldier1");


	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_b_4_c"))
	{
		body->SetFace(*monster,"f_b_4_");
		isBald=false;
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_b_5_s");
		body->SetFace(*monster,"f_b_5_");
		isBald=true;
	}

	//use bald head if i'm bald or if i'm wearing a hat
	if (isBald || hasHat)
	{
		monster->ghoulInst->SetPartOnOff("_crewhead", false);
	}
	else
	{
		monster->ghoulInst->SetPartOnOff("_baldhead", false);
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void ugsoldier1b_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_ugnsoldier1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_ugnsoldier1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_5_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_b_5_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_4_c",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_b_4_",VARIETY_PRIORITY_LOW);
}

void ugsoldier1b_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	qboolean hasHat = false;
	qboolean isBald = false;
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body || !monster->ghoulInst)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	if ( !(monster->spawnflags & SPAWNFLAG_NO_WEAPONS) )
	{
		body->SetRightHandWeapon(*monster, ATK_SHOTGUN);
		body->SetLeftHandWeapon(*monster, ATK_GRENADE);
	}

	//fixme--this hat is a screwy screwy thing indeed.
	hasHat=body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_flop2", "to_abolt_head_t", myInstance, "acc_hat_flop2_tan", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_brwn", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_brwn", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_ugnsoldier1");
	body->ApplySkin(*monster,  myInstance, "a", "a_ugnsoldier1");


	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_b_4_c"))
	{
		body->SetFace(*monster,"f_b_4_");
		isBald=false;
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_b_5_s");
		body->SetFace(*monster,"f_b_5_");
		isBald=true;
	}

	//use bald head if i'm bald or if i'm wearing a hat
	if (isBald || hasHat)
	{
		monster->ghoulInst->SetPartOnOff("_crewhead", false);
	}
	else
	{
		monster->ghoulInst->SetPartOnOff("_baldhead", false);
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void ugsoldier2_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_ugnsoldier2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_ugnsoldier2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_2_s",VARIETY_PRIORITY_HIGH);
	RegisterFaceSkins("f_b_2_",VARIETY_PRIORITY_HIGH);
	RegisterSkin("h","h_b_3_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_b_3_",VARIETY_PRIORITY_HIGHEST);
}

void ugsoldier2_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_SHOTGUN);

	if (!body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_baseball", "to_abolt_head_t_fwd", myInstance, "acc_hat_baseball_tan", MESO_OTHER_SCALE))
		body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_flop2", "to_abolt_head_t", myInstance, "acc_hat_flop2_tan", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_canteen1", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_r", myInstance, "acc_box_hip_smll_brwn", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_ugnsoldier2");
	body->ApplySkin(*monster,  myInstance, "a", "a_ugnsoldier2");

	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_b_2_s"))
	{
		body->SetFace(*monster,"f_b_2_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_b_3_s");
		body->SetFace(*monster,"f_b_3_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void ugsoldier3_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_ugnsoldier3",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_ugnsoldier3",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_3_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_b_3_",VARIETY_PRIORITY_HIGHEST);
}

void ugsoldier3_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_FLAMEGUN);
	body->SetLeftHandWeapon(*monster, ATK_GRENADE);

	if (body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", "acc_helmet2", "to_abolt_head_t", myInstance, "acc_helmet2_tan", MESO_OTHER_SCALE))
	{
		monster->ghoulInst->SetPartOnOff("_baldhead", false);
		monster->ghoulInst->SetPartOnOff("_comface", false);
		body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_goggles", "to_abolt_head_t", myInstance, "acc_goggles",MESO_OTHER_SCALE);
	}
	else
	{
		monster->ghoulInst->SetPartOnOff("_helmethead", false);
		monster->ghoulInst->SetPartOnOff("_helmetface", false);
		body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_sun",MESO_GLASSES_SCALE);
	}

	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_tanks", "to_abolt_back", myInstance, "acc_tanks_afr", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_ugnsoldier3");
	body->ApplySkin(*monster,  myInstance, "a", "a_ugnsoldier3");

	body->ApplySkin(*monster,  myInstance, "h", "h_b_3_s");
	body->SetFace(*monster,"f_b_3_");

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_CHEST);
}

void ugsniper_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_ugnsniper",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_ugnsniper",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_5_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_b_5_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_2_s",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_b_2_",VARIETY_PRIORITY_LOW);
}

void ugsniper_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_SNIPER);
	body->SetLeftHandWeapon(*monster, ATK_GRENADE);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_baseball", "to_abolt_head_t_bkwd", myInstance, "acc_hat_baseball_tan", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_sun",MESO_GLASSES_SCALE);
	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_brwn", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_ugnsniper");
	body->ApplySkin(*monster,  myInstance, "a", "a_ugnsniper");

	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_b_2_s"))
	{
		body->SetFace(*monster,"f_b_2_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_b_5_s");
		body->SetFace(*monster,"f_b_5_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void ugbrute_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_ugbutcher",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_ugbutcher",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_b_7_s",VARIETY_PRIORITY_HIGH);
	RegisterFaceSkins("f_b_7_",VARIETY_PRIORITY_HIGH);
	RegisterSkin("h","h_b_2_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_b_2_",VARIETY_PRIORITY_HIGHEST);
}

void ugbrute_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL2);
	body->SetLeftHandWeapon(*monster, ATK_MACHETE);

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_top", "to_abolt_head_t", myInstance, "acc_hat_top_wht", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_ugbutcher");
	body->ApplySkin(*monster,  myInstance, "a", "a_ugbutcher");

	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_b_7_s"))
	{
		body->SetFace(*monster,"f_b_7_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_b_2_s");
		body->SetFace(*monster,"f_b_2_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_TOUGH);
}

void ugrocket_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_ugnrocket",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_ugnrocket",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_afrrocket",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_afrrocket",VARIETY_PRIORITY_HIGHEST);
}

void ugrocket_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ROCKET);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_faceplate", "to_abolt_head_t", myInstance, "acc_faceplate_afr", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_back", "Enemy/Bolt", "acc_backpack_smll", "to_abolt_back", myInstance, "acc_backpack_smll_tan", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_ugnrocket");
	body->ApplySkin(*monster,  myInstance, "a", "a_ugnrocket");

	body->ApplySkin(*monster,  myInstance, "h", "h_afrrocket");
	body->ApplySkin(*monster,  myInstance, "f", "f_afrrocket");

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_FULL);
	// if this changes, update fx_surfaces.cpp also. 
	/*
	for (int i = 1; i <= 3; i++)
	{
		gi.soundindex(va("enemy/rocket/foot%d.wav", i));
	}
	*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void serbgrunt1_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_kosgrunt1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_kosgrunt1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("md","md_serbgrunt1",VARIETY_PRIORITY_HIGHEST);
}

void serbgrunt1_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_SHOTGUN);
	body->SetLeftHandWeapon(*monster, ATK_GASGREN);

	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_tanks", "to_abolt_back", myInstance, "acc_tanks_kos", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_canteen1", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.25, *monster, "abolt_thigh_l", "Enemy/Bolt", "acc_puch_thigh", "to_abolt_thigh_l", myInstance, "acc_pouch_thigh_green", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");

	body->ApplySkin(*monster,  myInstance, "b", "b_kosgrunt1");
	body->ApplySkin(*monster,  myInstance, "a", "a_kosgrunt1");
	body->ApplySkin(*monster,  myInstance, "md", "md_serbgrunt1");

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void serbgrunt2_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_kosgrunt2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_kosgrunt2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("md","md_serbgrunt2",VARIETY_PRIORITY_HIGHEST);
}

void serbgrunt2_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);
	body->SetLeftHandWeapon(*monster, ATK_GASGREN);

	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_tanks", "to_abolt_back", myInstance, "acc_tanks_kos", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_r", myInstance, "acc_box_hip_canteen1", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.25, *monster, "abolt_thigh_l", "Enemy/Bolt", "acc_puch_thigh", "to_abolt_thigh_l", myInstance, "acc_pouch_thigh_tan", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");

	body->ApplySkin(*monster,  myInstance, "b", "b_kosgrunt2");
	body->ApplySkin(*monster,  myInstance, "a", "a_kosgrunt2");
	body->ApplySkin(*monster,  myInstance, "md", "md_serbgrunt2");

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void serbgrunt3_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_kosgrunt3",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_kosgrunt3",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_h_6_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_h_6_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_h_3_s",VARIETY_PRIORITY_LOWEST);
	RegisterFaceSkins("f_h_3_",VARIETY_PRIORITY_LOWEST);
	RegisterSkin("h","h_w_5_c",VARIETY_PRIORITY_HIGH);
	RegisterFaceSkins("f_w_5_",VARIETY_PRIORITY_HIGH);
}

void serbgrunt3_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_helmet", "to_abolt_head_t", myInstance, "acc_helmet_green1", MESO_HAT_SCALE);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_gas_mask2", "to_abolt_head_t", myInstance, "acc_gas_mask2a", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_back", "Enemy/Bolt", "acc_backpack_low", "to_abolt_back", myInstance, "acc_backpack_low_tan", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_canteen1", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");

	body->ApplySkin(*monster,  myInstance, "b", "b_kosgrunt3");
	body->ApplySkin(*monster,  myInstance, "a", "a_kosgrunt3");

	if (!gi.irand(0,2)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_3_s"))
	{
		body->SetFace(*monster,"f_w_3_");
	}
	else
	{
		if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_5_c"))
		{
			body->SetFace(*monster,"f_w_5_");
		}
		else
		{
			body->ApplySkin(*monster,  myInstance, "h", "h_h_6_s");
			body->SetFace(*monster,"f_h_6_");
		}
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void serbsniper1a_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_kossnipe",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_kossnipe",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_h_6_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_h_6_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_10_s",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_w_10_",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_w_2_c",VARIETY_PRIORITY_LOWEST);
	RegisterFaceSkins("f_w_2_",VARIETY_PRIORITY_LOWEST);
}

void serbsniper1a_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_SNIPER);
	body->SetLeftHandWeapon(*monster, ATK_GASGREN);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_helmet2", "to_abolt_head_t", myInstance, "acc_helmet2_green", MESO_HAT_SCALE);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_gas_mask2", "to_abolt_head_t", myInstance, "acc_gas_mask2a", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_goggles", "to_abolt_head_t", myInstance, "acc_goggles", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");

	body->ApplySkin(*monster,  myInstance, "b", "b_kossnipe");
	body->ApplySkin(*monster,  myInstance, "a", "a_kossnipe");

	if (!gi.irand(0,2)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_2_c"))
	{
		body->SetFace(*monster,"f_w_2_");
	}
	else
	{
		if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_10_s"))
		{
			body->SetFace(*monster,"f_w_10_");
		}
		else
		{
			body->ApplySkin(*monster,  myInstance, "h", "h_h_6_s");
			body->SetFace(*monster,"f_h_6_");
		}
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_CHEST);
}

void serbsniper1b_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_kossnipe",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_kossnipe",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_h_6_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_h_6_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_10_s",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_w_10_",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_w_2_c",VARIETY_PRIORITY_LOWEST);
	RegisterFaceSkins("f_w_2_",VARIETY_PRIORITY_LOWEST);
}

void serbsniper1b_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_SNIPER);
	body->SetLeftHandWeapon(*monster, ATK_GASGREN);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_helmet2", "to_abolt_head_t", myInstance, "acc_helmet2_green", MESO_HAT_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_goggles", "to_abolt_head_t", myInstance, "acc_goggles", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");

	body->ApplySkin(*monster,  myInstance, "b", "b_kossnipe");
	body->ApplySkin(*monster,  myInstance, "a", "a_kossnipe");

	if (!gi.irand(0,2)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_2_c"))
	{
		body->SetFace(*monster,"f_w_2_");
	}
	else
	{
		if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_10_s"))
		{
			body->SetFace(*monster,"f_w_10_");
		}
		else
		{
			body->ApplySkin(*monster,  myInstance, "h", "h_h_6_s");
			body->SetFace(*monster,"f_h_6_");
		}
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_CHEST);
}

void serbcomtroop_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_koscomtroop",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_koscomtroop",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_19_s",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_w_19_",VARIETY_PRIORITY_LOW);
	RegisterSkin("h","h_w_4_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_4_",VARIETY_PRIORITY_HIGHEST);
}

void serbcomtroop_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL2);
	body->SetLeftHandWeapon(*monster, ATK_GRENADE);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_helmet", "to_abolt_head_t", myInstance, "acc_helmet_green1", MESO_HAT_SCALE);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_gas_mask2", "to_abolt_head_t", myInstance, "acc_gas_mask2a", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_backpack_lrg", "to_abolt_back", myInstance, "acc_backpack_lrg_grn", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_antenna", "to_abolt_back", myInstance, "acc_antenna", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");

	body->ApplySkin(*monster,  myInstance, "b", "b_koscomtroop");
	body->ApplySkin(*monster,  myInstance, "a", "a_koscomtroop");

	if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_19_s"))
	{
		body->SetFace(*monster,"f_w_19_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_w_4_s");
		body->SetFace(*monster,"f_w_4_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void serbbrute1a_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_kosbrute",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_kosbrute",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_h_6_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_h_6_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_3_s",VARIETY_PRIORITY_HIGH);
	RegisterFaceSkins("f_w_3_",VARIETY_PRIORITY_HIGH);
	RegisterSkin("h","h_w_5_c",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_w_5_",VARIETY_PRIORITY_LOW);
}

void serbbrute1a_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);
	body->SetLeftHandWeapon(*monster, ATK_GRENADE);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_gas_mask2", "to_abolt_head_t", myInstance, "acc_gas_mask2a", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.1, *monster, "abolt_back", "Enemy/Bolt", "acc_backpack_lrg", "to_abolt_back", myInstance, "acc_backpack_lrg_brwn", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_brwn", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_beret", "to_abolt_head_t", myInstance, "acc_hat_beret_red",MESO_HAT_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");

	body->ApplySkin(*monster,  myInstance, "b", "b_kosbrute");
	body->ApplySkin(*monster,  myInstance, "a", "a_kosbrute");

	if (!gi.irand(0,2)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_5_c"))
	{
		body->SetFace(*monster,"f_w_5_");
	}
	else
	{
		if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_3_s"))
		{
			body->SetFace(*monster,"f_w_3_");
		}
		else
		{
			body->ApplySkin(*monster,  myInstance, "h", "h_h_6_s");
			body->SetFace(*monster,"f_h_6_");
		}
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_TOUGH);
}

void serbbrute1b_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_kosbrute",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_kosbrute",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_h_6_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_h_6_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_3_s",VARIETY_PRIORITY_HIGH);
	RegisterFaceSkins("f_w_3_",VARIETY_PRIORITY_HIGH);
	RegisterSkin("h","h_w_5_c",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_w_5_",VARIETY_PRIORITY_LOW);
}

void serbbrute1b_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);
	body->SetLeftHandWeapon(*monster, ATK_GRENADE);

	body->AddRandomBoltedItem(0.1, *monster, "abolt_back", "Enemy/Bolt", "acc_backpack_lrg", "to_abolt_back", myInstance, "acc_backpack_lrg_brwn", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_brwn", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_beret", "to_abolt_head_t", myInstance, "acc_hat_beret_red",MESO_HAT_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");

	body->ApplySkin(*monster,  myInstance, "b", "b_kosbrute");
	body->ApplySkin(*monster,  myInstance, "a", "a_kosbrute");

	if (!gi.irand(0,2)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_5_c"))
	{
		body->SetFace(*monster,"f_w_5_");
	}
	else
	{
		if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_3_s"))
		{
			body->SetFace(*monster,"f_w_3_");
		}
		else
		{
			body->ApplySkin(*monster,  myInstance, "h", "h_h_6_s");
			body->SetFace(*monster,"f_h_6_");
		}
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_TOUGH);
}

void serbmechanic_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_kosmechanic",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_kosmechanic",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_4_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_4_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","hb_h_2_b",VARIETY_PRIORITY_HIGH);
	RegisterFaceSkins("f_hb_1_",VARIETY_PRIORITY_HIGH);
}

void serbmechanic_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_hard", "to_abolt_head_t", myInstance, "acc_hat_hard_kos", MESO_HAT_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");

	body->ApplySkin(*monster,  myInstance, "b", "b_kosmechanic");
	body->ApplySkin(*monster,  myInstance, "a", "a_kosmechanic");

	if(gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "hb_h_2_b"))
	{
		myInstance->GetInstPtr()->SetPartOnOff("_baldhead", false);
		myInstance->GetInstPtr()->SetPartOnOff("_comface", false);
		body->SetFace(*monster,"f_hb_1_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_w_4_s");
		myInstance->GetInstPtr()->SetPartOnOff("_beardhead", false);
		myInstance->GetInstPtr()->SetPartOnOff("_beardface", false);
		body->SetFace(*monster,"f_w_4_");
	}

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void kosrebel_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_kosrebel1",VARIETY_PRIORITY_HIGH);
	RegisterSkin("a","a_kosrebel1",VARIETY_PRIORITY_HIGH);
	RegisterSkin("b","b_kosrebel2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_kosrebel2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_19_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_19_",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("h","h_w_20_c",VARIETY_PRIORITY_HIGHEST);
//	RegisterFaceSkins("f_w_20_",VARIETY_PRIORITY_HIGHEST);
//	RegisterTalkFaceSkins("f_w_20_n",VARIETY_PRIORITY_HIGHEST);
}

void kosrebel_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	bool isbald=!!gi.irand(0,1);
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);
//	body->SetLeftHandWeapon(*monster, ATK_MOLITOV);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_back", "Enemy/Bolt", "acc_backpack_low", "to_abolt_back", myInstance, "acc_backpack_low_dgrey", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_canteen1", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");

	if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "b", "b_kosrebel1"))
	{
		body->ApplySkin(*monster,  myInstance, "a", "a_kosrebel1");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "b", "b_kosrebel2");
		body->ApplySkin(*monster,  myInstance, "a", "a_kosrebel2");
	}

//	if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_19_s"))
//	{
//		body->SetFace(*monster,"f_w_19_");
//		isbald=true;
//	}
//	else
//	{
//		body->ApplySkin(*monster,  myInstance, "h", "h_w_20_c");
//		body->SetFace(*monster,"f_w_20_");
//	}
	body->ApplySkin(*monster,  myInstance, "h", "h_w_19_s");
	body->SetFace(*monster,"f_w_19_");
	isbald=true;


	if (body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_helmet", "to_abolt_head_t", myInstance, "acc_helmet_green1", MESO_HAT_SCALE))
	{
		myInstance->GetInstPtr()->SetPartOnOff("_crewhead", false);
		myInstance->GetInstPtr()->SetPartOnOff("_baldhead", false);
		myInstance->GetInstPtr()->SetPartOnOff("_comface", false);
	}
	else
	{
		myInstance->GetInstPtr()->SetPartOnOff("_helmethead", false);
		myInstance->GetInstPtr()->SetPartOnOff("_helmetface", false);
		isbald=body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_baseball", "to_abolt_head_t_fwd", myInstance, "acc_hat_baseball_grcamo", MESO_OTHER_SCALE)||isbald;
		if (isbald)
		{
			myInstance->GetInstPtr()->SetPartOnOff("_crewhead", false);
		}
		else
		{
			myInstance->GetInstPtr()->SetPartOnOff("_baldhead", false);
		}
	}

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
	gi.effectindex("environ/onfireburst");
}

void kosklaguy_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_kosrebel2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_kosrebel2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_20_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_20_",VARIETY_PRIORITY_HIGHEST);
	RegisterTalkFaceSkins("f_w_20_n",VARIETY_PRIORITY_HIGHEST);
}

void kosklaguy_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	bool isbald=!!gi.irand(0,1);
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	SetSpecialBuddy(true);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);
//	body->SetLeftHandWeapon(*monster, ATK_MOLITOV);

	body->AddBoltedItem(*monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_canteen1", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_baseball", "to_abolt_head_t_fwd", myInstance, "acc_hat_baseball_grcamo", MESO_OTHER_SCALE)||isbald;

	body->ApplySkin(*monster,  myInstance, "c", "c_black");

	body->ApplySkin(*monster,  myInstance, "b", "b_kosrebel2");
	body->ApplySkin(*monster,  myInstance, "a", "a_kosrebel2");

	body->ApplySkin(*monster,  myInstance, "h", "h_w_20_c");
	body->SetFace(*monster,"f_w_20_");

	myInstance->GetInstPtr()->SetPartOnOff("_helmethead", false);
	myInstance->GetInstPtr()->SetPartOnOff("_helmetface", false);
	myInstance->GetInstPtr()->SetPartOnOff("_crewhead", false);

	body->SetVoiceDirectories(*monster, "", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
	gi.effectindex("environ/onfireburst");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sibtrooper1a_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_sibtrooper2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_sibtrooper2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("hd","hd_sibtroop",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_sibtroop",VARIETY_PRIORITY_HIGHEST);
}

void sibtrooper1a_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_gas_mask", "to_abolt_head_t", myInstance, "acc_gas_mask", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_backpack_lrg", "to_abolt_back", myInstance, "acc_backpack_lrg_wht", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_antenna", "to_abolt_back", myInstance, "acc_antenna", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_sibtrooper2");
	body->ApplySkin(*monster,  myInstance, "a", "a_sibtrooper2");

	body->ApplySkin(*monster,  myInstance, "hd", "hd_sibtroop");
	body->ApplySkin(*monster,  myInstance, "f", "f_sibtroop");
//	body->SetFace(*monster,"f_sibtroop_");//eh?

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void sibtrooper1b_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_sibtrooper2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_sibtrooper2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("hd","hd_sibtroop",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_sibtroop",VARIETY_PRIORITY_HIGHEST);
}

void sibtrooper1b_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_SNIPER);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_gas_mask", "to_abolt_head_t", myInstance, "acc_gas_mask", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_backpack_smll", "to_abolt_back", myInstance, "acc_backpack_smll_wht", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_blck", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_sibtrooper2");
	body->ApplySkin(*monster,  myInstance, "a", "a_sibtrooper2");

	body->ApplySkin(*monster,  myInstance, "hd", "hd_sibtroop");
	body->ApplySkin(*monster,  myInstance, "f", "f_sibtroop");
//	body->SetFace(*monster,"f_sibtroop_");//eh?

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void sibtrooper2_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_sibtrooper2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_sibtrooper2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("hd","hd_sibtroop",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_sibtroop2",VARIETY_PRIORITY_HIGHEST);
}

void sibtrooper2_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetLeftHandWeapon(*monster, ATK_FLASHGREN);
	body->SetRightHandWeapon(*monster, ATK_MACHINEPISTOL);

	body->AddBoltedItem(*monster, "abolt_hip_r", "Enemy/Bolt", "acc_backpack_low", "to_abolt_hip_r", myInstance, "acc_backpack_low_dgrey", MESO_OTHER_SCALE);
	body->AddBoltedItem(*monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_blck", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_sibtrooper2");
	body->ApplySkin(*monster,  myInstance, "a", "a_sibtrooper2");

	body->ApplySkin(*monster,  myInstance, "hd", "hd_sibtroop");
	body->ApplySkin(*monster,  myInstance, "f", "f_sibtroop2");
//	body->SetFace(*monster,"f_sibtroop2_");//eh?

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void sibguard_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_sibguard1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_sibguard1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_16_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_16_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_10_s",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_w_10_",VARIETY_PRIORITY_LOW);
}

void sibguard_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetLeftHandWeapon(*monster, ATK_FLASHGREN);
	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);

	body->AddRandomBoltedItem(0.5, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_baseball", "to_abolt_head_t", myInstance, "acc_hat_baseball_sib", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.5, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_blk", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_sibguard1");
	body->ApplySkin(*monster,  myInstance, "a", "a_sibguard1");

	if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_10_s"))
	{
		body->SetFace(*monster,"f_w_10_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_w_16_s");
		body->SetFace(*monster,"f_w_16_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void sibguard3_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_sibguard1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_sibguard1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_16_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_16_",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("h","h_w_10_s",VARIETY_PRIORITY_LOW);
//	RegisterFaceSkins("f_w_10_",VARIETY_PRIORITY_LOW);
	RegisterTalkFaceSkins("f_w_16_n",VARIETY_PRIORITY_HIGHEST);
}

void sibguard3_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetLeftHandWeapon(*monster, ATK_FLASHGREN);
	body->SetRightHandWeapon(*monster, ATK_MACHINEPISTOL);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_tall", "to_abolt_head_t", myInstance, "acc_hat_tall_gry", MESO_HAT_SCALE);
	body->AddRandomBoltedItem(0.5, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_blk", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_sibguard1");
	body->ApplySkin(*monster,  myInstance, "a", "a_sibguard1");

//	if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_10_s"))
//	{
//		body->SetFace(*monster,"f_w_10_");
//	}
//	else
//	{
		body->ApplySkin(*monster,  myInstance, "h", "h_w_16_s");
		body->SetFace(*monster,"f_w_16_");
//	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void sibguard4_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_sibguard4",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_sibguard4",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_21_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_w_21_",VARIETY_PRIORITY_HIGHEST);
}

void sibguard4_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetLeftHandWeapon(*monster, ATK_FLASHGREN);
	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_sibguard4");
	body->ApplySkin(*monster,  myInstance, "a", "a_sibguard4");

	body->ApplySkin(*monster,  myInstance, "h", "h_w_21_s");
	body->SetFace(*monster,"f_w_21_");

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void sibcleansuit_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_sibclean",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_sibclean",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("md","md_clean",VARIETY_PRIORITY_HIGHEST);
}

void sibcleansuit_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL1);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_r", myInstance, "acc_box_hip_smll_blck", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");

	//fixme: first one is real one, other two are secret dancer-guys
//	switch(gi.irand(0,2))
//	{
//	default:
//	case 0:
		body->ApplySkin(*monster,  myInstance, "b", "b_sibclean");
		body->ApplySkin(*monster,  myInstance, "a", "a_sibclean");
		body->ApplySkin(*monster,  myInstance, "md", "md_clean");
//		break;
//	case 1:
//		body->ApplySkin(*monster,  myInstance, "b", "b_sibclean2");
//		body->ApplySkin(*monster,  myInstance, "a", "a_sibclean2");
//		body->ApplySkin(*monster,  myInstance, "md", "md_clean2");
//		break;
//	case 2:
//		body->ApplySkin(*monster,  myInstance, "b", "b_sibclean3");
//		body->ApplySkin(*monster,  myInstance, "a", "a_sibclean3");
//		body->ApplySkin(*monster,  myInstance, "md", "md_clean3");
//		break;
//	}

	body->SetVoiceDirectories(*monster, "cln", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void sibmech_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_sibmechanic",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_sibmechanic",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_w_19_s",VARIETY_PRIORITY_HIGH);
	RegisterFaceSkins("f_w_19_",VARIETY_PRIORITY_HIGH);
	RegisterSkin("h","hb_h_2_b",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_hb_1_",VARIETY_PRIORITY_HIGHEST);
}

void sibmech_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_hard", "to_abolt_head_t", myInstance, "acc_hat_hard_sib", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");

	body->ApplySkin(*monster,  myInstance, "b", "b_sibmechanic");
	body->ApplySkin(*monster,  myInstance, "a", "a_sibmechanic");

	if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_w_19_s"))
	{
		myInstance->GetInstPtr()->SetPartOnOff("_beardhead", false);
		myInstance->GetInstPtr()->SetPartOnOff("_beardface", false);
		body->SetFace(*monster,"f_w_19_");
	}
	else
	{
		myInstance->GetInstPtr()->SetPartOnOff("_baldhead", false);
		myInstance->GetInstPtr()->SetPartOnOff("_comface", false);
		body->ApplySkin(*monster,  myInstance, "h", "hb_h_2_b");
		body->SetFace(*monster,"f_hb_1_");
	}

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void irqsoldier1_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqsoldier1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqsoldier1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_3_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_3_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_1_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_1_",VARIETY_PRIORITY_HIGHEST);
}

void irqsoldier1_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_SNIPER);
	body->SetLeftHandWeapon(*monster, ATK_GRENADE);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_beret", "to_abolt_head_t", myInstance, "acc_hat_beret_red",MESO_HAT_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_sun",MESO_GLASSES_SCALE);
	body->AddRandomBoltedItem(0.25, *monster, "abolt_thigh_r", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_r", myInstance, "acc_pouch_thigh_green",MESO_POUCH_SCALE);
	body->AddRandomBoltedItem(0.25, *monster, "abolt_thigh_l", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_l", myInstance, "acc_pouch_thigh_green",MESO_POUCH_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_irqsoldier1");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqsoldier1");

	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_m_3_s"))
	{
		body->SetFace(*monster,"f_m_3_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_m_1_s");
		body->SetFace(*monster,"f_m_1_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void irqsoldier2_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqsoldier2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqsoldier2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_3_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_3_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_4_c",VARIETY_PRIORITY_HIGH);
	RegisterFaceSkins("f_m_4_",VARIETY_PRIORITY_HIGH);
}

void irqsoldier2_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_helmet", "to_abolt_head_t", myInstance, "acc_helmet_tan1",MESO_HAT_SCALE);
	body->AddBoltedItem(*monster, "abolt_hip_r", "Enemy/Bolt", "acc_backpack_low", "to_abolt_hip_r", myInstance, "acc_backpack_low_tan", MESO_OTHER_SCALE);

	if (!body->AddRandomBoltedItem(0.1, *monster, "abolt_thigh_r", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_r", myInstance, "acc_pouch_thigh_tan",MESO_POUCH_SCALE))
	{
		body->AddRandomBoltedItem(0.15, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_r", myInstance, "acc_box_hip_smll_brwn", MESO_OTHER_SCALE);
	}
	if (!body->AddRandomBoltedItem(0.1, *monster, "abolt_thigh_l", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_l", myInstance, "acc_pouch_thigh_tan",MESO_POUCH_SCALE))
	{
		body->AddRandomBoltedItem(0.15, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_brwn", MESO_OTHER_SCALE);
	}

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_irqsoldier2");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqsoldier2");

	//use crew head here!
	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_m_4_c"))
	{
		body->SetFace(*monster,"f_m_4_");
	}
	//use bald head here!
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_m_3_s");
		body->SetFace(*monster,"f_m_3_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void irqsoldier2b_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqsoldier2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqsoldier2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_3_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_3_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_4_c",VARIETY_PRIORITY_HIGH);
	RegisterFaceSkins("f_m_4_",VARIETY_PRIORITY_HIGH);
}

void irqsoldier2b_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_MACHINEGUN);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_helmet", "to_abolt_head_t", myInstance, "acc_helmet_tan1",MESO_HAT_SCALE);
	body->AddBoltedItem(*monster, "abolt_hip_r", "Enemy/Bolt", "acc_backpack_low", "to_abolt_hip_r", myInstance, "acc_backpack_low_tan", MESO_OTHER_SCALE);

	if (!body->AddRandomBoltedItem(0.1, *monster, "abolt_thigh_r", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_r", myInstance, "acc_pouch_thigh_tan",MESO_POUCH_SCALE))
	{
		body->AddRandomBoltedItem(0.15, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_r", myInstance, "acc_box_hip_smll_brwn", MESO_OTHER_SCALE);
	}
	if (!body->AddRandomBoltedItem(0.1, *monster, "abolt_thigh_l", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_l", myInstance, "acc_pouch_thigh_tan",MESO_POUCH_SCALE))
	{
		body->AddRandomBoltedItem(0.15, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_brwn", MESO_OTHER_SCALE);
	}

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_irqsoldier2");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqsoldier2");

	//use crew head here!
	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_m_4_c"))
	{
		body->SetFace(*monster,"f_m_4_");
	}
	//use bald head here!
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_m_3_s");
		body->SetFace(*monster,"f_m_3_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void irqrepgd_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqrepguard",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqrepguard",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_5_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_5_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_1_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_1_",VARIETY_PRIORITY_HIGHEST);
}


void irqrepgd_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);
	body->SetLeftHandWeapon(*monster, ATK_GRENADE);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_helmet", "to_abolt_head_t", myInstance, "acc_helmet_green1",MESO_HAT_SCALE);
	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_backpack_smll", "to_abolt_back", myInstance, "acc_backpack_smll_green", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.15, *monster, "abolt_thigh_r", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_r", myInstance, "acc_pouch_thigh_dark1",MESO_POUCH_SCALE);
	body->AddRandomBoltedItem(0.15, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_hip_lrg", "to_abolt_hip_r", myInstance, "acc_box_hip_lrg_green", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.15, *monster, "abolt_thigh_l", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_l", myInstance, "acc_pouch_thigh_dark1",MESO_POUCH_SCALE);
	body->AddRandomBoltedItem(0.15, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_lrg", "to_abolt_hip_l", myInstance, "acc_box_hip_lrg_green", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_irqrepguard");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqrepguard");

	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_m_5_s"))
	{
		body->SetFace(*monster,"f_m_5_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_m_1_s");
		body->SetFace(*monster,"f_m_1_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void irqrepgdb_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqrepguard",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqrepguard",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_5_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_5_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_1_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_1_",VARIETY_PRIORITY_HIGHEST);
}


void irqrepgdb_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_MACHINEGUN);
	body->SetLeftHandWeapon(*monster, ATK_GASGREN);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_helmet", "to_abolt_head_t", myInstance, "acc_helmet_green1",MESO_HAT_SCALE);
	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_backpack_smll", "to_abolt_back", myInstance, "acc_backpack_smll_green", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.15, *monster, "abolt_thigh_r", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_r", myInstance, "acc_pouch_thigh_dark1",MESO_POUCH_SCALE);
	body->AddRandomBoltedItem(0.15, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_hip_lrg", "to_abolt_hip_r", myInstance, "acc_box_hip_lrg_green", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.15, *monster, "abolt_thigh_l", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_l", myInstance, "acc_pouch_thigh_dark1",MESO_POUCH_SCALE);
	body->AddRandomBoltedItem(0.15, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_lrg", "to_abolt_hip_l", myInstance, "acc_box_hip_lrg_green", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_irqrepguard");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqrepguard");

	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_m_5_s"))
	{
		body->SetFace(*monster,"f_m_5_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_m_1_s");
		body->SetFace(*monster,"f_m_1_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void irqpolice_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqpolice",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqpolice",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_5_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_5_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_4_c",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_m_4_",VARIETY_PRIORITY_LOW);
}

void irqpolice_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL2);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_helmet", "to_abolt_head_t", myInstance, "acc_helmet_wht1",MESO_HAT_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_sun",MESO_GLASSES_SCALE);
	body->AddRandomBoltedItem(0.15, *monster, "abolt_thigh_r", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_r", myInstance, "acc_pouch_thigh_tan",MESO_POUCH_SCALE);
	body->AddRandomBoltedItem(0.15, *monster, "abolt_thigh_l", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_l", myInstance, "acc_pouch_thigh_tan",MESO_POUCH_SCALE);

	if (!body->AddRandomBoltedItem(0.1, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_brwn", MESO_OTHER_SCALE))
	{
		body->AddRandomBoltedItem(0.15, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_r", myInstance, "acc_box_hip_smll_blck", MESO_OTHER_SCALE);
	}
	body->AddRandomBoltedItem(0.15, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance, "acc_box_hip_smll_blck", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_irqpolice");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqpolice");

	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_m_4_c"))
	{
		body->SetFace(*monster,"f_m_4_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_m_5_s");
		body->SetFace(*monster,"f_m_5_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void irqcommander_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqcommander",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqcommander",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_5_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_5_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","hb_h_2_b",VARIETY_PRIORITY_HIGH);
	RegisterFaceSkins("f_hb_1_",VARIETY_PRIORITY_HIGH);
}

void irqcommander_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL2);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_beret", "to_abolt_head_t", myInstance, "acc_hat_beret_grn", MESO_HAT_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_blck", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_irqcommander");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqcommander");

	if (gi.irand(0,1) && body->ApplySkin(*monster,  myInstance, "h", "hb_h_2_b"))
	{
		body->SetFace(*monster,"f_hb_1_");
		myInstance->GetInstPtr()->SetPartOnOff("_baldhead", false);
		myInstance->GetInstPtr()->SetPartOnOff("_comface", false);
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_m_5_s");
		body->SetFace(*monster,"f_m_5_");
		myInstance->GetInstPtr()->SetPartOnOff("_beardhead", false);
		myInstance->GetInstPtr()->SetPartOnOff("_beardface", false);
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void irqbrutea_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqbrute",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqbrute",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_5_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_5_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_3_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_3_",VARIETY_PRIORITY_HIGHEST);
}

void irqbrutea_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);
	body->SetLeftHandWeapon(*monster, ATK_GRENADE);

	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_backpack_lrg", "to_abolt_back", myInstance, "acc_backpack_lrg_tan", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_brwn", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.15, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_blk", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_irqbrute");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqbrute");

	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_m_5_s"))
	{
		body->SetFace(*monster,"f_m_5_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_m_3_s");
		body->SetFace(*monster,"f_m_3_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_TOUGH);
}

void irqbruteb_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqbrute",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqbrute",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_5_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_5_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_3_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_3_",VARIETY_PRIORITY_HIGHEST);
}

void irqbruteb_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_FLAMEGUN);

	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_tanks", "to_abolt_back", myInstance, "acc_tanks_irq", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_sun",MESO_GLASSES_SCALE);
	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_brwn", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.15, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_blk", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_irqbrute");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqbrute");

	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_m_5_s"))
	{
		body->SetFace(*monster,"f_m_5_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_m_3_s");
		body->SetFace(*monster,"f_m_3_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_SOMEWHAT_TOUGH);
}

void irqbodyguard_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_irqbodyguard",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqbodyguard",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_1_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_1_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_4_c",VARIETY_PRIORITY_LOWEST);
	RegisterFaceSkins("f_m_4_",VARIETY_PRIORITY_LOWEST);
}

void irqbodyguard_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_SNIPER);
	body->SetLeftHandWeapon(*monster, ATK_GRENADE);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_beret", "to_abolt_head_t", myInstance, "acc_hat_beret_blck",MESO_HAT_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "to_abolt_hip_r", myInstance, "acc_holster_blck", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.15, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_blk", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_irqbodyguard");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqbodyguard");

	if (rand()%2 && body->ApplySkin(*monster,  myInstance, "h", "h_m_4_c"))
	{
		body->SetFace(*monster,"f_m_4_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_m_1_s");
		body->SetFace(*monster,"f_m_1_");
	}

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_CHEST);
}

void irqrocket_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_irqrocket",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqrocket",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_irqrocket",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_irqrocket",VARIETY_PRIORITY_HIGHEST);
}

void irqrocket_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

//	GetMySkills()->setSkills(128, 192, 0, 0, .2, 0, 0, 0, 128);

	body->SetRightHandWeapon(*monster, ATK_ROCKET);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_faceplate", "to_abolt_head_t", myInstance, "acc_faceplate_irq", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_back", "Enemy/Bolt", "acc_backpack_smll", "to_abolt_back", myInstance, "acc_backpack_smll_tan", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_irqrocket");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqrocket");
	body->ApplySkin(*monster,  myInstance, "f", "f_irqrocket");
	body->ApplySkin(*monster,  myInstance, "h", "h_irqrocket");

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_FULL);
	// if this changes, update fx_surfaces.cpp also. 
	/*
	for (int i = 1; i <= 3; i++)
	{
		gi.soundindex(va("enemy/rocket/foot%d.wav", i));
	}
	*/
}

void irqsaddam_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqsaddam",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqsaddam",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_saddam",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_saddam_",VARIETY_PRIORITY_HIGHEST);
	RegisterTalkFaceSkins("f_saddam_n",VARIETY_PRIORITY_HIGHEST);
}

void irqsaddam_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

//	body->SetRightHandWeapon(*monster, ATK_PISTOL2);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_beret", "to_abolt_head_t", myInstance, "acc_hat_beret_blck",MESO_HAT_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_irqsaddam");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqsaddam");
	body->ApplySkin(*monster,  myInstance, "h", "h_saddam");
	body->SetFace(*monster,"f_saddam_");

	body->SetVoiceDirectories(*monster, "so", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void irqworker_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqworker",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqworker",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_2_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_2_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_1_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_1_",VARIETY_PRIORITY_HIGHEST);
}

void irqworker_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	bool newface=false;
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_norm",MESO_GLASSES_SCALE);

	if (!body->AddRandomBoltedItem(0.1, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_turbin", "to_abolt_head_t", myInstance, "acc_hat_turbin_wht",MESO_HAT_SCALE))
	{
		body->AddRandomBoltedItem(0.85, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_hard", "to_abolt_head_t", myInstance, "acc_hat_hard_wht",MESO_HAT_SCALE);
	}

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_irqworker");
	body->ApplySkin(*monster,  myInstance, "a", "a_irqworker");

	if ((rand()%2) && body->ApplySkin(*monster,  myInstance, "h", "h_m_2_s"))
	{
		body->SetFace(*monster,"f_m_2_");
		newface = true;
	}
	if (!newface)
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_m_1_s");
		body->SetFace(*monster,"f_m_1_");
	}

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void irqman2_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_irqman2a",VARIETY_PRIORITY_LOW);
	RegisterSkin("a","a_irqman2a",VARIETY_PRIORITY_LOW);
	RegisterSkin("b","b_irqman2b",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_irqman2b",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_1_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_1_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_m_2_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_m_2_",VARIETY_PRIORITY_HIGHEST);
//	RegisterSkin("h","hb_h_2_b",VARIETY_PRIORITY_LOWEST);
//	RegisterFaceSkins("f_hb_1_",VARIETY_PRIORITY_LOWEST);
}

void irqman2_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

//	body->AddRandomBoltedItem(0.5, *monster, "wbolt_hand_l", "Enemy/Bolt", "w_sack", "to_wbolt_hand_l", myInstance, "rubbish", MESO_WEAPON_SCALE);

	if (!body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_turbin", "to_abolt_head_t", myInstance, "acc_hat_turbin_wht",MESO_HAT_SCALE))
	{
		if (!body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_top", "to_abolt_head_t", myInstance, "acc_hat_top",MESO_HAT_SCALE))
		{
			body->AddRandomBoltedItem(0.33333, *monster, "abolt_head_t", "Enemy/Bolt", "acc_hat_top", "to_abolt_head_t", myInstance, "acc_hat_top_wht",MESO_HAT_SCALE);
		}
	}

	body->AddRandomBoltedItem(0.15, *monster, "abolt_thigh_r", "Enemy/Bolt", "acc_machette", "to_abolt_thigh_r", myInstance, "acc_machette",MESO_POUCH_SCALE);

	if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "b", "b_irqman2a"))
	{
		body->ApplySkin(*monster,  myInstance, "a", "a_irqman2a");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "b", "b_irqman2b");
		body->ApplySkin(*monster,  myInstance, "a", "a_irqman2b");
	}

//	if(!gi.irand(0,2)&&body->ApplySkin(*monster,  myInstance, "h", "hb_h_2_b"))
//	{
//		myInstance->GetInstPtr()->SetPartOnOff("_baldhead", false);
//		myInstance->GetInstPtr()->SetPartOnOff("_comface", false);
//		body->SetFace(*monster,"f_hb_1_");
//	}
//	else
//	{
		if(gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_m_2_s"))
		{
		myInstance->GetInstPtr()->SetPartOnOff("_beardhead", false);
		myInstance->GetInstPtr()->SetPartOnOff("_beardface", false);
		body->SetFace(*monster,"f_m_2_");
		}
		else
		{
			body->ApplySkin(*monster,  myInstance, "h", "h_m_1_s");
			myInstance->GetInstPtr()->SetPartOnOff("_beardhead", false);
			myInstance->GetInstPtr()->SetPartOnOff("_beardface", false);
			body->SetFace(*monster,"f_m_1_");
		}
//	}

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tokhench1_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_tokhench1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_tokhench1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_j_5_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_j_5_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_j_2_s",VARIETY_PRIORITY_LOWEST);
	RegisterFaceSkins("f_j_2_",VARIETY_PRIORITY_LOWEST);
	RegisterSkin("h","h_j_6_s",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_j_6_",VARIETY_PRIORITY_LOW);
}

void tokhench1_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_PISTOL1);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_sun",MESO_GLASSES_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_tokhench1");
	body->ApplySkin(*monster,  myInstance, "a", "a_tokhench1");

	if(!gi.irand(0,2)&&body->ApplySkin(*monster,  myInstance, "h", "h_j_2_s"))
	{
		myInstance->GetInstPtr()->SetPartOnOff("_crewhead", false);
		if (gi.irand(0,1))
		{
			myInstance->GetInstPtr()->SetPartOnOff("_ponytail", false);
		}
		body->SetFace(*monster,"f_j_2_");
	}
	else
	{
		if(gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_j_6_s"))
		{
			myInstance->GetInstPtr()->SetPartOnOff("_crewhead", false);
			if (gi.irand(0,1))
			{
				myInstance->GetInstPtr()->SetPartOnOff("_ponytail", false);
			}
			body->SetFace(*monster,"f_j_6_");
		}
		else
		{
			body->ApplySkin(*monster,  myInstance, "h", "h_j_5_c");
			myInstance->GetInstPtr()->SetPartOnOff("_baldhead", false);
			body->SetFace(*monster,"f_j_5_");
		}
	}

	body->SetVoiceDirectories(*monster, "ya", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void tokhench2_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_tokhench2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_tokhench2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_j_3_s",VARIETY_PRIORITY_HIGH);
	RegisterFaceSkins("f_j_3_",VARIETY_PRIORITY_HIGH);
	RegisterSkin("h","h_j_4_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_j_4_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_j_6_s",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_j_6_",VARIETY_PRIORITY_LOW);
}

void tokhench2_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_sun",MESO_GLASSES_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_tokhench2");
	body->ApplySkin(*monster,  myInstance, "a", "a_tokhench2");

	if(gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_j_6_s"))
	{
		myInstance->GetInstPtr()->SetPartOnOff("_crewhead", false);
		body->SetFace(*monster,"f_j_6_");
	}
	else
	{
		if(!gi.irand(0,2)&&body->ApplySkin(*monster,  myInstance, "h", "h_j_3_s"))
		{
			myInstance->GetInstPtr()->SetPartOnOff("_baldhead", false);
			myInstance->GetInstPtr()->SetPartOnOff("_ponytail", false);
			body->SetFace(*monster,"f_j_3_");
		}
		else
		{
			body->ApplySkin(*monster,  myInstance, "h", "h_j_4_s");
			myInstance->GetInstPtr()->SetPartOnOff("_crewhead", false);
			myInstance->GetInstPtr()->SetPartOnOff("_ponytail", false);
			body->SetFace(*monster,"f_j_4_");
		}
	}

	body->SetVoiceDirectories(*monster, "ya", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void tokkiller_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_tokkiller",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_tokkiller",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_j_5_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_j_5_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_j_2_s",VARIETY_PRIORITY_LOWEST);
	RegisterFaceSkins("f_j_2_",VARIETY_PRIORITY_LOWEST);
}

void tokkiller_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_MICROWAVE_ALT);

	body->ApplySkin(*monster,  myInstance, "b", "b_tokkiller");
	body->ApplySkin(*monster,  myInstance, "a", "a_tokkiller");

	if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_j_2_s"))
	{
		myInstance->GetInstPtr()->SetPartOnOff("_crewhead", false);
		body->SetFace(*monster,"f_j_2_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_j_5_c");
		myInstance->GetInstPtr()->SetPartOnOff("_baldhead", false);
		body->SetFace(*monster,"f_j_5_");
	}

	body->SetVoiceDirectories(*monster, "ya", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_CHEST);
}

void tokninja_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_tokninja",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_tokninja",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_ninja",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_ninja",VARIETY_PRIORITY_HIGHEST);
}

void tokninja_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

//	body->SetLeftHandWeapon(*monster, ATK_FLASHGREN);
	body->SetLeftHandWeapon(*monster, ATK_THROWSTAR);
	body->SetRightHandWeapon(*monster, ATK_MACHINEPISTOL);

	body->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_sword", "to_abolt_back", myInstance, "acc_sword", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_tokninja");
	body->ApplySkin(*monster,  myInstance, "a", "a_tokninja");

	body->ApplySkin(*monster,  myInstance, "h", "h_ninja");
	body->ApplySkin(*monster,  myInstance, "f", "f_ninja");

	body->SetVoiceDirectories(*monster, "ni", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void tokbrute_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_tokbrute",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_tokbrute",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_j_3_s",VARIETY_PRIORITY_HIGH);
	RegisterFaceSkins("f_j_3_",VARIETY_PRIORITY_HIGH);
	RegisterSkin("h","h_j_4_s",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_j_4_",VARIETY_PRIORITY_HIGHEST);
}

void tokbrute_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ASSAULTRIFLE);

	body->ApplySkin(*monster,  myInstance, "b", "b_tokbrute");
	body->ApplySkin(*monster,  myInstance, "a", "a_tokbrute");

	if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_j_3_s"))
	{
		body->SetFace(*monster,"f_j_3_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_j_4_s");
		body->SetFace(*monster,"f_j_4_");
	}

	body->SetVoiceDirectories(*monster, "ya", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_TOUGH);
}

void tokmalehostage_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_tokman1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_tokman1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_j_1_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_j_1_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_j_2_s",VARIETY_PRIORITY_LOWEST);
	RegisterFaceSkins("f_j_2_",VARIETY_PRIORITY_LOWEST);
}

void tokmalehostage_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_norm",MESO_GLASSES_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_tokman1");
	body->ApplySkin(*monster,  myInstance, "a", "a_tokman1");

	if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_j_2_s"))
	{
		myInstance->GetInstPtr()->SetPartOnOff("_crewhead", false);
		myInstance->GetInstPtr()->SetPartOnOff("_lbang", false);
		myInstance->GetInstPtr()->SetPartOnOff("_rbang", false);
		body->SetFace(*monster,"f_j_2_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_j_1_c");
		myInstance->GetInstPtr()->SetPartOnOff("_baldhead", false);
		if (gi.irand(0,1))
		{
			myInstance->GetInstPtr()->SetPartOnOff("_lbang", false);
		}
		if (gi.irand(0,1))
		{
			myInstance->GetInstPtr()->SetPartOnOff("_rbang", false);
		}
		body->SetFace(*monster,"f_j_1_");
	}

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void japansuit_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_tokmhostage",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_tokmhostage",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_j_5_c",VARIETY_PRIORITY_HIGHEST);
	RegisterFaceSkins("f_j_5_",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_j_6_s",VARIETY_PRIORITY_LOW);
	RegisterFaceSkins("f_j_6_",VARIETY_PRIORITY_LOW);
}

void japansuit_ai::Init(edict_t *monster, char *ghoulname, char* subclass)
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_norm",MESO_GLASSES_SCALE);

	body->ApplySkin(*monster,  myInstance, "b", "b_tokmhostage");
	body->ApplySkin(*monster,  myInstance, "a", "a_tokmhostage");

	if (gi.irand(0,1)&&body->ApplySkin(*monster,  myInstance, "h", "h_j_6_s"))
	{
		myInstance->GetInstPtr()->SetPartOnOff("_crewhead", false);
		myInstance->GetInstPtr()->SetPartOnOff("_lbang", false);
		body->SetFace(*monster,"f_j_6_");
	}
	else
	{
		body->ApplySkin(*monster,  myInstance, "h", "h_j_5_c");
		myInstance->GetInstPtr()->SetPartOnOff("_baldhead", false);
		if (gi.irand(0,1))
		{
			myInstance->GetInstPtr()->SetPartOnOff("_lbang", false);
		}
		body->SetFace(*monster,"f_j_5_");
	}

	body->SetVoiceDirectories(*monster, "npc", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void raider1_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_dcraider1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_dcraider1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_raider1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_raider2",VARIETY_PRIORITY_HIGHEST);
}

void raider1_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	if ( !(monster->spawnflags & SPAWNFLAG_NO_WEAPONS) )
	{
		body->SetRightHandWeapon(*monster, ATK_MACHINEGUN);
	}

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_goggles", "to_abolt_head_t", myInstance, "acc_goggles", MESO_OTHER_SCALE);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_back", "Enemy/Bolt", "acc_backpack_smll", "to_abolt_back", myInstance, "acc_backpack_smll_dgrey", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.3333, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_blk", MESO_OTHER_SCALE);
	body->AddRandomBoltedItem(0.3333, *monster, "abolt_thigh_l", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_l", myInstance, "acc_pouch_thigh_blck",MESO_POUCH_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_dcraider1");
	body->ApplySkin(*monster,  myInstance, "a", "a_dcraider1");

	body->ApplySkin(*monster,  myInstance, "h", "h_raider1");
	body->ApplySkin(*monster,  myInstance, "f", "f_raider2");

	body->SetVoiceDirectories(*monster, "ra", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);
}

void raider2_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_dcraider2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_dcraider2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_raider2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_raider2",VARIETY_PRIORITY_HIGHEST);
}

void raider2_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_MACHINEPISTOL);
	body->SetLeftHandWeapon(*monster, ATK_GRENADE);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_nightvision2", "to_abolt_head_t", myInstance, "acc_nightvision", MESO_OTHER_SCALE);
	if (!body->AddRandomBoltedItem(0.25, *monster, "abolt_back", "Enemy/Bolt", "acc_backpack_smll", "to_abolt_back", myInstance, "acc_backpack_smll_dgrey", MESO_OTHER_SCALE))
	{
		if (body->AddRandomBoltedItem(0.3333, *monster, "abolt_back", "Enemy/Bolt", "acc_backpack_lrg", "to_abolt_back", myInstance, "acc_backpack_lrg_blk", MESO_OTHER_SCALE))
		{
			body->AddRandomBoltedItem(0.5, *monster, "abolt_back", "Enemy/Bolt", "acc_antenna", "to_abolt_back", myInstance, "acc_antenna", MESO_OTHER_SCALE);
		}
	}
	body->AddRandomBoltedItem(0.3333, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_blk", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_dcraider2");
	body->ApplySkin(*monster,  myInstance, "a", "a_dcraider2");

	body->ApplySkin(*monster,  myInstance, "h", "h_raider2");
	body->ApplySkin(*monster,  myInstance, "f", "f_raider2");

	body->SetVoiceDirectories(*monster, "ra", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_CHESTLIMBS);
}

void raider2b_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_dcraider2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_dcraider2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_raider2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_raider2",VARIETY_PRIORITY_HIGHEST);
}

void raider2b_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_AUTOSHOTGUN);

	body->AddRandomBoltedItem(0.25, *monster, "abolt_head_t", "Enemy/Bolt", "acc_nightvision2", "to_abolt_head_t", myInstance, "acc_acc_nightvision", MESO_OTHER_SCALE);

	if (!body->AddRandomBoltedItem(0.25, *monster, "abolt_back", "Enemy/Bolt", "acc_backpack_smll", "to_abolt_back", myInstance, "acc_backpack_smll_dgrey", MESO_OTHER_SCALE))
	{
		if (body->AddRandomBoltedItem(0.3333, *monster, "abolt_back", "Enemy/Bolt", "acc_backpack_lrg", "to_abolt_back", myInstance, "acc_backpack_lrg_blk", MESO_OTHER_SCALE))
		{
			body->AddRandomBoltedItem(0.5, *monster, "abolt_back", "Enemy/Bolt", "acc_antenna", "to_abolt_back", myInstance, "acc_antenna", MESO_OTHER_SCALE);
		}
	}
	body->AddRandomBoltedItem(0.3333, *monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance, "acc_box_hip_blk", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_dcraider2");
	body->ApplySkin(*monster,  myInstance, "a", "a_dcraider2");

	body->ApplySkin(*monster,  myInstance, "h", "h_raider2");
	body->ApplySkin(*monster,  myInstance, "f", "f_raider2");

	body->SetVoiceDirectories(*monster, "ra", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_CHESTLIMBS);
}

void raiderbrute_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("b","b_dcbrute",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_dcbrute",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_raider4",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_raider4",VARIETY_PRIORITY_HIGHEST);
}

void raiderbrute_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_MICROWAVE_ALT);
	body->SetLeftHandWeapon(*monster, ATK_GRENADE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_dcbrute");
	body->ApplySkin(*monster,  myInstance, "a", "a_dcbrute");

	body->ApplySkin(*monster,  myInstance, "h", "h_raider4");
	body->ApplySkin(*monster,  myInstance, "f", "f_raider4");

	body->SetVoiceDirectories(*monster, "ra", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_TOUGH);
}

void raiderrocket_ai::RegisterSkins(void)
{
	RegisterGoreSkins();
	RegisterSkin("gz","gz_armor",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("b","b_casrocket",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("a","a_casrocket",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("h","h_raider3",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("f","f_raider3",VARIETY_PRIORITY_HIGHEST);
}

void raiderrocket_ai::Init(edict_t *monster, char *ghoulname, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	ggOinstC* myInstance;

	generic_meso_ai::Init(monster, ghoulname, subclass);

	if (!body)
	{
		return;
	}

	myInstance = MyGhoulObj->FindOInst(monster->ghoulInst);

	body->SetRightHandWeapon(*monster, ATK_ROCKET);

	body->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_faceplate2", "to_abolt_head_t", myInstance, "acc_faceplate2", MESO_OTHER_SCALE);

	body->ApplySkin(*monster,  myInstance, "c", "c_black");
	body->ApplySkin(*monster,  myInstance, "b", "b_casrocket");
	body->ApplySkin(*monster,  myInstance, "a", "a_casrocket");

	body->ApplySkin(*monster,  myInstance, "h", "h_raider3");
	body->ApplySkin(*monster,  myInstance, "f", "f_raider3");

	body->SetVoiceDirectories(*monster, "ra", 1, DEATHVOICE_DEFAULT);
	body->SetRootBolt(*monster);

	body->SetArmor(*monster, ARMOR_FULL);
	// if this changes, update fx_surfaces.cpp also. 
	/*
	for (int i = 1; i <= 3; i++)
	{
		gi.soundindex(va("enemy/rocket/foot%d.wav", i));
	}
	*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SP_m_x_mmerc(edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_HAWK, self, "enemy/meso", "hawk");
	self->think = generic_grunt_init;

	gi.soundindex("impact/Hawk/LookOut.adp");//Hawk gets pissed when you shoot at him
}

void SP_m_x_mhurtmerc(edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_HURTHAWK, self, "enemy/meso", "hawk");
	self->think = generic_grunt_init;
}

void SP_m_x_msam(edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SAM, self, "enemy/meso", "sam");
	self->think = generic_grunt_init;
}

void SP_m_x_mcharacter(edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_JOHN, self, "enemy/meso", "john");
	self->think = generic_grunt_init;
}

void SP_m_x_mcharacter_snow(edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_JOHN_SNOW, self, "enemy/meso", "johnsnow");
	self->think = generic_grunt_init;
}

void SP_m_x_mcharacter_desert(edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_JOHN_DESERT, self, "enemy/meso", "johndesert");
	self->think = generic_grunt_init;
}

void SP_m_x_mskinboss (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SKINHEADBOSS, self, "enemy/meso", "skinboss");
	self->think = generic_leader_init;
}

void SP_m_x_mraiderboss1 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_RAIDERBOSS, self, "enemy/meso", "dekker1");
	self->think = generic_leader_init;
}

void SP_m_x_mraiderboss2 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_RAIDERBOSS2, self, "enemy/meso", "dekker2");
	self->think = dekker_init;
}

void SP_m_x_miraqboss(edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_AMU, self, "enemy/meso", "amu");
	self->think = generic_leader_init;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void SP_m_nyc_mskinhead1 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SKINHEAD1, self, "enemy/meso", "skinhead1");
	self->think = generic_grunt_init;
}

void SP_m_nyc_mskinhead2a (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SKINHEAD2A, self, "enemy/meso", "skinhead2");
	self->think = generic_grunt_init;
}

void SP_m_nyc_mskinhead2b (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SKINHEAD2B, self, "enemy/meso", "skinhead2b");
	self->think = generic_grunt_init;
}

void SP_m_nyc_mswat (edict_t *self)
{
	// check the "i'm only here for my armor" flag now, cuz it does some different stuff straightaway
	if (self->spawnflags & SPAWNFLAG_ARMOR_PICKUP)
	{
		generic_armor_spawnnow(self);
		self->ai = ai_c::Create(AI_MESO_NYCSWATGUY, self, "enemy/meso", "nyswatguy");
		self->think = generic_armor_init;
	}
	else
	{
		generic_monster_spawnnow(self);
		self->ai = ai_c::Create(AI_MESO_NYCSWATGUY, self, "enemy/meso", "nyswatguy");
		self->think = generic_grunt_init;
	}
}

void SP_m_nyc_mswatleader (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_NYCSWATLEADER, self, "enemy/meso", "nyswatlead");
	self->think = generic_grunt_init;
}

void SP_m_nyc_mpolitician (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_MALEPOLITICIAN, self, "enemy/meso", "politician");
	self->think = generic_npc_init;
}

void SP_m_nyc_mpunk (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_NYCPUNK, self, "enemy/meso", "nypunk");
	self->think = generic_grunt_init;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void SP_m_afr_msoldier1a (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_UGNSOLDIER1, self, "enemy/meso", "ugsoldier1");
	self->think = generic_grunt_init;
}

void SP_m_afr_msoldier1b (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_UGNSOLDIER1B, self, "enemy/meso", "ugsoldier1b");
	self->think = generic_grunt_init;
}

void SP_m_afr_msoldier2 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_UGNSOLDIER2, self, "enemy/meso", "ugsoldier2");
	self->think = generic_grunt_init;
}

void SP_m_afr_msoldier3 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_UGNSOLDIER3, self, "enemy/meso", "ugsoldier3");
	self->think = generic_grunt_init;
}

void SP_m_afr_msniper (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_UGNSNIPER, self, "enemy/meso", "ugsniper");
	self->think = generic_leader_init;
}

void SP_m_afr_mbrute (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_UGNBRUTE, self, "enemy/meso", "butch");
	self->think = generic_leader_init;
}

void SP_m_afr_mrocket (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->flags |= FL_NO_KNOCKBACK;
	self->ai = ai_c::Create(AI_MESO_UGNROCKET, self, "enemy/meso", "ugrocket");
	self->think = generic_leader_init;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void SP_m_kos_mgrunt1 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SERBGRUNT1, self, "enemy/meso", "serbgrunt");
	self->think = generic_grunt_init;
}

void SP_m_kos_mgrunt2 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SERBGRUNT2, self, "enemy/meso", "serbgrunt2");
	self->think = generic_grunt_init;
}

void SP_m_kos_mgrunt3 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SERBGRUNT3, self, "enemy/meso", "serbgrunt3");
	self->think = generic_grunt_init;
}

void SP_m_kos_msniper1a (edict_t *self)
{
	// check the "i'm only here for my armor" flag now, cuz it does some different stuff straightaway
	if (self->spawnflags & SPAWNFLAG_ARMOR_PICKUP)
	{
		generic_armor_spawnnow(self);
		self->ai = ai_c::Create(AI_MESO_SERBSNIPER1A, self, "enemy/meso", "serbsniper");
		self->think = generic_armor_init;
	}
	else
	{
		generic_monster_spawnnow(self);
		self->ai = ai_c::Create(AI_MESO_SERBSNIPER1A, self, "enemy/meso", "serbsniper");
		self->think = generic_grunt_init;
	}
}

void SP_m_kos_msniper1b (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SERBSNIPER1B, self, "enemy/meso", "serbsniper");
	self->think = generic_grunt_init;
}

void SP_m_kos_mcomtroop (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SERBCOMTROOP, self, "enemy/meso", "serbcomtroop");
	self->think = generic_grunt_init;
}

void SP_m_kos_mbrute1a (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SERBBRUTE1A, self, "enemy/meso", "serbbrute");
	self->think = generic_grunt_init;
}

void SP_m_kos_mbrute1b (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SERBBRUTE1B, self, "enemy/meso", "serbbrute");
	self->think = generic_grunt_init;
}

void SP_m_kos_mmechanic (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SERBMECHANIC, self, "enemy/meso", "serbmechanic");
	self->think = generic_npc_init;
}

void SP_m_kos_mrebel (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_KOSREBEL, self, "enemy/meso", "kosrebel");
	self->think = generic_grunt_init;
}

void SP_m_kos_mklaleader (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_KOSKLAGUY, self, "enemy/meso", "kosklaguy");
	self->think = generic_grunt_init;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void SP_m_sib_mtrooper1a (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SIBTROOPER1A, self, "enemy/meso", "sibtroop1a");
	self->think = generic_grunt_init;
}

void SP_m_sib_mtrooper1b (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SIBTROOPER1B, self, "enemy/meso", "sibtroop1b");
	self->think = generic_grunt_init;
}

void SP_m_sib_mtrooper2 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SIBTROOPER2, self, "enemy/meso", "sibtroop2");
	self->think = generic_grunt_init;
}

void SP_m_sib_mguard1 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SIBGUARD, self, "enemy/meso", "sibguard");
	self->think = generic_grunt_init;
}

void SP_m_sib_mguard3 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SIBGUARD3, self, "enemy/meso", "sibguard3");
	self->think = generic_grunt_init;
}

void SP_m_sib_mguard4 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SIBGUARD4, self, "enemy/meso", "sibguard4");
	self->think = generic_grunt_init;
}

void SP_m_sib_mcleansuit (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SIBCLEANSUIT, self, "enemy/meso", "sibclean");
	self->think = generic_grunt_init;
}

void SP_m_sib_mmechanic (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_SIBMECH, self, "enemy/meso", "sibmech");
	self->think = generic_grunt_init;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void SP_m_irq_msoldier1 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_IRAQSOLDIER1, self, "enemy/meso", "irqsoldier1");
	self->think = generic_grunt_init;
}

void SP_m_irq_msoldier2a (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_IRAQSOLDIER2, self, "enemy/meso", "irqsoldier2");
	self->think = generic_grunt_init;
}

void SP_m_irq_msoldier2b (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_IRAQSOLDIER2B, self, "enemy/meso", "irqsoldier2b");
	self->think = generic_grunt_init;
}

void SP_m_irq_mrepguard1a (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_IRAQREPGUARD, self, "enemy/meso", "irqrepgd");
	self->think = generic_grunt_init;
}

void SP_m_irq_mrepguard1b (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_IRAQREPGUARDB, self, "enemy/meso", "irqrepgdb");
	self->think = generic_grunt_init;
}

void SP_m_irq_mpolice (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_IRAQPOLICE, self, "enemy/meso", "irqpolice");
	self->think = generic_leader_init;
}

void SP_m_irq_mcommander (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_IRAQCOMMANDER, self, "enemy/meso", "irqcommand");
	self->think = generic_leader_init;
}

void SP_m_irq_mbrute1a (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_IRAQBRUTEA, self, "enemy/meso", "irqbrute");
	self->think = generic_leader_init;
}

void SP_m_irq_mbrute1b (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_IRAQBRUTEB, self, "enemy/meso", "irqbruteb");
	self->think = generic_leader_init;
}

void SP_m_irq_mbodyguard (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_IRAQBODYGUARD, self, "enemy/meso", "irqbguard");
	self->think = generic_leader_init;
}

void SP_m_irq_mrocket (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->flags |= FL_NO_KNOCKBACK;
	self->ai = ai_c::Create(AI_MESO_IRAQROCKET, self, "enemy/meso", "irqrocket");
	self->think = generic_leader_init;
}

void SP_m_irq_msaddam (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_IRAQSADDAM, self, "enemy/meso", "saddam");
	self->think = generic_leader_init;
}

void SP_m_irq_moilworker (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_IRAQWORKER, self, "enemy/meso", "irqworker");
	self->think = generic_npc_init;
}

void SP_m_irq_mman2 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_IRAQMAN2, self, "enemy/meso", "irqman2");
	self->think = generic_leader_init;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void SP_m_tok_mhench1 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_TOKHENCH1, self, "enemy/meso", "tokhench1");
	self->think = generic_grunt_init;
}

void SP_m_tok_mhench2 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_TOKHENCH2, self, "enemy/meso", "tokhench2");
	self->think = generic_grunt_init;
}

void SP_m_tok_mkiller (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_TOKKILLER, self, "enemy/meso", "tokkiller");
	self->think = generic_grunt_init;
}

void SP_m_tok_mninja (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_TOKNINJA, self, "enemy/meso", "tokninja");
	self->think = generic_grunt_init;
}

void SP_m_tok_mbrute (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_TOKBRUTE, self, "enemy/meso", "tokbrute");
	self->think = generic_grunt_init;
}

void SP_m_tok_mman1 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_JAPANSUIT, self, "enemy/meso", "japansuit");
	self->think = generic_npc_init;
}

void SP_m_tok_mman2 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_TOKMALEHOSTAGE, self, "enemy/meso", "tokhostage");
	self->think = generic_npc_init;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void SP_m_cas_mraider1 (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_RAIDER1, self, "enemy/meso", "dcraider1");
	self->think = generic_grunt_init;
}

void SP_m_cas_mraider2a (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_RAIDER2A, self, "enemy/meso", "dcraider2");
	self->think = generic_grunt_init;
}

void SP_m_cas_mraider2b (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_RAIDER2B, self, "enemy/meso", "dcraider2b");
	self->think = generic_grunt_init;
}

void SP_m_cas_mbrute (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_RAIDERBRUTE, self, "enemy/meso", "dcraiderbrute");
	self->think = generic_grunt_init;
}

void SP_m_cas_mrocket (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->flags |= FL_NO_KNOCKBACK;
	self->ai = ai_c::Create(AI_MESO_RAIDERROCKET, self, "enemy/meso", "dcraiderrocket");
	self->think = generic_grunt_init;
}

/////////////////////////////////////////////////////////////////////////////////////////////
