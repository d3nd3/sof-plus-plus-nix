#include "g_local.h"
#include "ai_private.h"
#include "m_generic.h"
#include "..\qcommon\ef_flags.h"

#define MERC_WEAPON_SCALE 1.142857142857 
#define MERC_OTHER_SCALE 1.142857142857 


edict_t			*last_monster_used;

/*
void BoltOnCraziness(ai_c &which_ai, edict_t *monster)
{

	if (!which_ai.GetBody())
	{
		return;
	}

	ggOinstC *myInstance=which_ai.GetGhoulObj()->FindOInst(monster->ghoulInst);
	
	switch (rand()%3)
	{
	case 0:
		which_ai.GetBody()->AddBoltedItem(*monster, "wbolt_hand_l", "Enemy/Bolt", "w_briefcase", "to_wbolt_hand_l", myInstance);
		break;
	case 1:
		which_ai.GetBody()->SetRightHandWeapon(*monster, ATK_PISTOL2);
		break;
	default:
		which_ai.GetBody()->SetRightHandWeapon(*monster, ATK_PISTOL1);
		break;
	}
	switch (rand()%3)
	{
	case 0:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_norm");
		break;
	case 1:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_glasses", "to_abolt_head_t", myInstance, "acc_glasses_sun");
		break;
	default:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_head_t", "Enemy/Bolt", "acc_goggles", "to_abolt_head_t", myInstance);
		break;
	}
	switch (rand()%3)
	{
	case 0:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_backpack_lrg", "to_abolt_back", myInstance);
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_antenna", "to_abolt_back", myInstance);
		break;
	case 1:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_backpack_smll", "to_abolt_back", myInstance);
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_back", "Enemy/Bolt", "acc_antenna", "to_abolt_back", myInstance);
		break;
	default:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_hip_r", "Enemy/Bolt", "acc_backpack_low", "to_abolt_hip_r", myInstance);
		break;
	}
	switch (rand()%5)
	{
	case 0:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_l", myInstance);
		break;
	case 1:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip_lrg", "to_abolt_hip_l", myInstance);
		break;
	case 2:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_hip_l", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_l", myInstance);
		break;
	case 3:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_thigh_l", "Enemy/Bolt", "acc_machette", "to_abolt_thigh_l", myInstance);
		break;
	default:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_thigh_l", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_l", myInstance);
		break;
	}
	switch (rand()%6)
	{
	case 0:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_hip_smll", "to_abolt_hip_r", myInstance);
		break;
	case 1:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_hip_lrg", "to_abolt_hip_r", myInstance);
		break;
	case 2:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_hip_r", "Enemy/Bolt", "acc_box_hip", "to_abolt_hip_r", myInstance);
		break;
	case 3:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_hip_r", "Enemy/Bolt", "acc_holster", "abolt_hip_r", myInstance);
		break;
	case 4:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_thigh_r", "Enemy/Bolt", "acc_machette", "to_abolt_thigh_r", myInstance);
		break;
	default:
		which_ai.GetBody()->AddBoltedItem(*monster, "abolt_thigh_r", "Enemy/Bolt", "acc_pouch_thigh", "to_abolt_thigh_r", myInstance);
		break;
	}

	which_ai.GetBody()->AddBoltedItem(*monster, "abolt_shoulder_l", "Enemy/Bolt", "acc_spikes_shlder", "to_abolt_shoulder_l", myInstance);
	which_ai.GetBody()->AddBoltedItem(*monster, "abolt_shoulder_r", "Enemy/Bolt", "acc_spikes_shlder", "to_abolt_shoulder_r", myInstance);
}
*/
//////////////////////////////////////////////////////////////////////
//					sense
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//					decision
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//					ai
//////////////////////////////////////////////////////////////////////
void generic_human_ai::RegisterSkins(void)
{
}

void generic_human_ai::RegisterFaceSkin(const char *theMaterialName, const char *faceSkinName, expressionPriority_e expressionPriorityVal)
{
}

void generic_human_ai::RegisterSkin(const char *theMaterialName, const char *theSkinName, varietyPriority_e varietyPriorityVal)
{
	if ((varietyPriorityVal==VARIETY_PRIORITY_HIGHEST)||((float)varietyPriorityVal>(mskins_variety_limit->value-0.1)))
	{
		MyGhoulObj->RegisterSkin(theMaterialName, theSkinName);
	}
}

void generic_human_ai::RegisterGoreSkins(void)
{
	//register gorezones
	RegisterSkin("gz","gz_sing1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("gz","gz_sing2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("gz","gz_sing3",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("gz","gz_sing_k",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("gz","gz_multi1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("gz","gz_multi2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("gz","gz_multi3",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("gz","gz_multi_k",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("gz","gz_mass1",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("gz","gz_mass2",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("gz","gz_mass3",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("gz","gz_mass_k",VARIETY_PRIORITY_HIGHEST);

	//caps
	RegisterSkin("c","c_blood",VARIETY_PRIORITY_HIGHEST);
	RegisterSkin("c","c_black",VARIETY_PRIORITY_HIGHEST);
}

void generic_human_ai::RegisterFaceSkins(const char *faceBase, varietyPriority_e varietyPriorityVal)
{
	if ((varietyPriorityVal==VARIETY_PRIORITY_HIGHEST)||((float)varietyPriorityVal>(mskins_variety_limit->value-0.1)))
	{
		char tface[20];
		sprintf(tface,"%sn",faceBase);
		MyGhoulObj->RegisterSkin("f",tface);
		sprintf(tface,"%sd",faceBase);
		MyGhoulObj->RegisterSkin("f",tface);

		//pain face not vital, but good
		if ((mskins_expression_limit->value-0.1)<EXPRESSION_PRIORITY_HIGH)
		{
			sprintf(tface,"%sp",faceBase);
			MyGhoulObj->RegisterSkin("f",tface);
		}

		//fear & mean faces not vital, but good
		if ((mskins_expression_limit->value-0.1)<EXPRESSION_PRIORITY_LOW)
		{
			sprintf(tface,"%sm",faceBase);
			MyGhoulObj->RegisterSkin("f",tface);
			sprintf(tface,"%sf",faceBase);
			MyGhoulObj->RegisterSkin("f",tface);
		}

		//blink face total fluff
		if ((mskins_expression_limit->value-0.1)<EXPRESSION_PRIORITY_LOWEST)
		{
			sprintf(tface,"%sb",faceBase);
			MyGhoulObj->RegisterSkin("f",tface);
		}
	}
}

void generic_human_ai::RegisterTalkFaceSkins(const char *faceBase, varietyPriority_e varietyPriorityVal)
{
	if ((varietyPriorityVal==VARIETY_PRIORITY_HIGHEST)||((float)varietyPriorityVal>(mskins_variety_limit->value-0.1)))
	{
		char tface[20];

		//1st talk frame--highest priority
		if ((mskins_expression_limit->value-0.1)<EXPRESSION_PRIORITY_HIGH)
		{
			sprintf(tface,"%st1",faceBase);
			MyGhoulObj->RegisterSkin("f",tface);
		}

		//2nd talk frame
		if ((mskins_expression_limit->value-0.1)<EXPRESSION_PRIORITY_LOW)
		{
			sprintf(tface,"%st2",faceBase);
			MyGhoulObj->RegisterSkin("f",tface);
		}

		//3rd talk frame--least priority
		if ((mskins_expression_limit->value-0.1)<EXPRESSION_PRIORITY_LOWEST)
		{
			sprintf(tface,"%st3",faceBase);
			MyGhoulObj->RegisterSkin("f",tface);
		}
	}
}

const char *generic_human_ai::GetInterpFile(char *theclass, char *subclass)
{
	//override specially for mesos (& possibly for ectos, females)
	return NULL;
}

void generic_human_ai::Init(edict_t *monster, char *theclass, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	Matrix4	mat,mat1,mat2;

	ai_c::Init(monster, theclass, subclass);

	//fixme? for client-side physics to be a reasonable approximate, dimensions should be multiples of 8
	InitBBoxPreset(BBOX_PRESET_STAND,		-16, -16, -32, 16, 16, 41); // scaled-up 8/7ths
	InitBBoxPreset(BBOX_PRESET_CROUCH,		-16, -16, -16, 16, 16, 28);
	InitBBoxPreset(BBOX_PRESET_PRONE,		-32, -32,  -8, 32, 32, 16);
	InitBBoxPreset(BBOX_PRESET_ALLFOURS,	-8, -8,  -8, 8, 8, 0);//temporary!!!!
	InitBBoxPreset(BBOX_PRESET_SIT,			-32, -32,  -8, 32, 32, 8);
	InitBBoxPreset(BBOX_PRESET_LAYFRONT,	-41, -41, -38, 41, 41, -24);
	InitBBoxPreset(BBOX_PRESET_LAYBACK,		-41, -41, -32, 41, 41, -24);
	InitBBoxPreset(BBOX_PRESET_LAYSIDE,		-41, -41, -32, 41, 41, -24);
	InitBBoxPreset(BBOX_PRESET_NONE,		0, 0, 0, 0, 0, 0);

	//ick. if i'm a body part, use my parent's ghoulobj, not the level's monster ghoulobj
	if (monster->owner && monster->owner->ghoulInst)
	{
		ggObjC *theParentObj=game_ghoul.FindObject(monster->owner->ghoulInst->GetGhoulObject());
		 
		MyGhoulObj=game_ghoul.FindObject(theParentObj->GetName(), theParentObj->GetSubName(), false, theParentObj->GetSkinName(), theParentObj->GetBaseFile());
	}
	else
	{
		MyGhoulObj=game_ghoul.FindObject(theclass, subclass, false, GetObjectSkin(theclass,subclass), GetInterpFile(theclass,subclass));
	}

	if (!MyGhoulObj)
	{
		gi.error("Couldn't find enemy model %s/%s.\n",theclass,subclass);
	}

	RegisterSkins();

	MyGhoulObj->RegistrationLock();

	if (!body)
	{
		return;
	}

	game_ghoul.AddObjectInstance(MyGhoulObj, monster);


	//only consider scaling if guy's scale hasn't already been messed with
/*	if (scale > 0.99 && scale < 1.01)
	{
		if (!(monster->spawnflags & SPAWNFLAG_NOSCALE))
		{
			scale = (gi.flrand(0.95,1.1));
		}
		else
		{
			scale = 1.0;
		}
	}
*/
	monster->ghoulInst->GetXForm(mat);
	mat1.Identity();
	mat2=mat;
	mat1.Rotate(2,-M_PI*0.5);
	mat.Concat(mat1, mat2);
	mat2.Identity();
	mat1=mat;
	mat2.Scale(scale);
	mat.Concat(mat1, mat2);
	monster->ghoulInst->SetXForm(mat);

	//need to call setrootbolt yet! don't forget in specifc init!
}

//THIS is why i don't want this function overriden like crazy--to ensure
//that we don't load in multiple meso .ghb's on one level, it would be best
//if all mesos went through here
const char *generic_meso_ai::GetInterpFile(char *theclass, char *subclass)
{
	lev_interp_code curLevel = GetInterpCode();
	
	switch (curLevel)
	{
	case LEVCODE_TUTORIAL:
			return "meso_tut1";
	case LEVCODE_NYC_SUBWAY:
			return "meso_nyc";
	case LEVCODE_NYC_SUBWAY2:
			return "meso_tsr2";
	case LEVCODE_AFR_TRAIN:
			return "meso_train";
	case LEVCODE_SIB_BASE:
			return "meso_sib2";

	case LEVCODE_KOS_SEWER:
			return "meso_kos1";
	case LEVCODE_KOS_BIGGUN:
			return "meso_kos2";
	case LEVCODE_KOS_HANGAR:
			return "meso_kos3";
	case LEVCODE_SIB_CANYON:
			return "meso_sib1";
	case LEVCODE_SIB_PLANT:
			return "meso_sib3";

	case LEVCODE_IRQ_TOWNA:
			return "meso_irq1a";
	case LEVCODE_IRQ_BUNKER:
			return "meso_irq2a";
	case LEVCODE_IRQ_CARGO:
			return "meso_irq3a";
	case LEVCODE_NYC_WARE:
			return "meso_nyc1";
	case LEVCODE_NYC_STEAM:
			return "meso_nyc2";
	case LEVCODE_NYC_STREETS:
			return "meso_nyc3";
	case LEVCODE_AFR_YARD:
			return "meso_sud1";
	case LEVCODE_AFR_HOUSE:
			return "meso_sud2";
	case LEVCODE_AFR_FACT:
			return "meso_sud3";
	case LEVCODE_TOK_STREET:
			return "meso_tok1";
	case LEVCODE_TOK_OFFICE:
			return "meso_tok2";
	case LEVCODE_TOK_PENT:
			return "meso_tok3";
	case LEVCODE_IRQ_STREETS:
			return "meso_irq1b";

	case LEVCODE_IRQ_FORT:
			return "meso_irq2b";
	case LEVCODE_IRQ_OIL:
			return "meso_irq3b";
	case LEVCODE_CAS_1:
			return "meso_cas1";
	case LEVCODE_CAS_2:
			return "meso_cas2";
	case LEVCODE_CAS_3:
			return "meso_cas3";
	case LEVCODE_CAS_4:
			return "meso_cas4";

	case LEVCODE_ARM_1:
	case LEVCODE_ARM_2:
	case LEVCODE_ARM_3:
			return "meso_arm";


	case LEVCODE_UNKNOWN:
		break;
	}
	return "meso";
}

void generic_meso_ai::AddBody(edict_t *monster)
{
	if (!monster)
	{
		return;
	}
	body = new bodymeso_c();
}



//////////////////////////////////////////////////////////////////////
//	Specialized corpse ai
//////////////////////////////////////////////////////////////////////


void playercorpse_ai::AddBody(edict_t *monster)
{
	//fixme: check what variety player (monster->owner?) is, and add meso or female body accordingly
	if (!monster || !monster->owner || !monster->owner->client || !monster->owner->client->body)
	{
		gi.dprintf("Couldn't find player body for replication!\n");
		return;
	}
	body = body_c::NewClassForCode(monster->owner->client->body->GetClassCode());
}

#include "callback.h"
extern FollowGenericPivot		TheFollowGenericPivot;

action_c *playercorpse_ai::DeathAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	return new 	deathcorpse_action(od, oa, newanim, monster, inflictor, attacker, damage, point);
}

void playercorpse_ai::Init(edict_t *monster, char *theclass, char* subclass)
{
	ggOinstC* myInstance;

//	generic_human_ai::Init(monster, theclass, subclass);

	//i don't like how the human init goes, so here's a rough approximation of it without the ghoulobj & inst creating (icky, i know):
	if(body)
	{
		body->SetOwner(monster);
	}

	if (!body || !monster->owner)
	{
		return;
	}

	InitBBoxPreset(BBOX_PRESET_STAND,		monster->owner->mins[0]-16, monster->owner->mins[1]-16, monster->owner->mins[2], monster->owner->maxs[0]+16, monster->owner->maxs[1]+16, monster->owner->maxs[2]); // scaled-up 8/7ths
	InitBBoxPreset(BBOX_PRESET_CROUCH,		monster->owner->mins[0]-16, monster->owner->mins[1]-16, monster->owner->mins[2], monster->owner->maxs[0]+16, monster->owner->maxs[1]+16, monster->owner->maxs[2]);
	InitBBoxPreset(BBOX_PRESET_PRONE,		monster->owner->mins[0]-16, monster->owner->mins[1]-16, monster->owner->mins[2], monster->owner->maxs[0]+16, monster->owner->maxs[1]+16, monster->owner->maxs[2]);
	InitBBoxPreset(BBOX_PRESET_ALLFOURS,	monster->owner->mins[0]-16, monster->owner->mins[1]-16, monster->owner->mins[2], monster->owner->maxs[0]+16, monster->owner->maxs[1]+16, monster->owner->maxs[2]);//temporary!!!!
	InitBBoxPreset(BBOX_PRESET_SIT,			monster->owner->mins[0]-16, monster->owner->mins[1]-16, monster->owner->mins[2], monster->owner->maxs[0]+16, monster->owner->maxs[1]+16, monster->owner->maxs[2]);
	InitBBoxPreset(BBOX_PRESET_LAYFRONT,	monster->owner->mins[0]-16, monster->owner->mins[1]-16, monster->owner->mins[2], monster->owner->maxs[0]+16, monster->owner->maxs[1]+16, monster->owner->maxs[2]);
	InitBBoxPreset(BBOX_PRESET_LAYBACK,		monster->owner->mins[0]-16, monster->owner->mins[1]-16, monster->owner->mins[2], monster->owner->maxs[0]+16, monster->owner->maxs[1]+16, monster->owner->maxs[2]);
	InitBBoxPreset(BBOX_PRESET_LAYSIDE,		monster->owner->mins[0]-16, monster->owner->mins[1]-16, monster->owner->mins[2], monster->owner->maxs[0]+16, monster->owner->maxs[1]+16, monster->owner->maxs[2]);
	InitBBoxPreset(BBOX_PRESET_NONE,		monster->owner->mins[0]-16, monster->owner->mins[1]-16, monster->owner->mins[2], monster->owner->maxs[0]+16, monster->owner->maxs[1]+16, monster->owner->maxs[2]);

	game_ghoul.RemoveObjectInstances(monster);

	monster->s.renderfx=monster->owner->s.renderfx;
	monster->s.number = monster - g_edicts;

	if (monster->owner && monster->owner->client && monster->owner->client->body && monster->owner->client->body->GetMove())
	{
		body->SetRootBolt(*monster);//ick--i'm going to do this later, but to make ::Die call not freak out, i need to do it here too
		NewAction(DeathAction(NULL, NULL, monster->owner->client->body->GetMove(), *monster, NULL, NULL, 1, vec3_origin), monster, true);

		//eek!  Since I don't have an inst yet, I need to set the move by hand, and without setting an animation!
		body->SetMove(monster->owner->client->body->GetMove());

		Die (*monster, NULL, NULL, 0, vec3_origin, 0);
	}

	//dangerous, fast way of doing corpse ghoulInsts
	if (monster->owner && monster->owner->ghoulInst)
	{
		MyGhoulObj=game_ghoul.FindObject(monster->owner->ghoulInst->GetGhoulObject());
		if (MyGhoulObj)
		{
			myInstance=MyGhoulObj->FindOInst(monster->owner->ghoulInst);
			if (myInstance)
			{
				myInstance->ChangeOwnerTo(monster);
			}
		}
	}

	body->SetRootBolt(*monster);

	body->Emote(*monster, EMOTION_DEAD, 999999.9);

	monster->ghoulInst->SetMyBolt("quake_ground");

	//this fixes spinny corpses, because we're no longer bolted to quake_origin...
	GhoulID tempBolt=monster->ghoulInst->GetGhoulObject()->FindPart("quake_origin");
	monster->ghoulInst->RemoveMatrixCallBack(&TheFollowGenericPivot,tempBolt,IGhoulInst::MatrixType::JacobianEntity);

	monster->owner=NULL;

	if(myInstance)
	{
		ggBinstC *boltedptr;
		
		boltedptr=myInstance->GetBoltInstance(monster->ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r"));
		if (boltedptr)
		{
			myInstance->RemoveBoltInstance(boltedptr);
		}
	}
}



blownpart_ai::blownpart_ai(blownpart_ai *orig)
: generic_human_ai(orig)
{
	severTime = orig->severTime;
	removeMe = orig->removeMe;
	Stopped = orig->Stopped;
	frozen = orig->frozen;
	bFading = orig->bFading;
}


void blownpart_ai::Evaluate(blownpart_ai *orig)
{
	severTime = orig->severTime;
	removeMe = orig->removeMe;
	Stopped = orig->Stopped;
	frozen = orig->frozen;
	bFading = orig->bFading;

	generic_human_ai::Evaluate(orig);
}

const char *blownpart_ai::GetInterpFile(char *theclass, char *subclass)
{
	if (ent && ent->owner)
	{
		if (ent->owner->ai)
		{
			return ent->owner->ai->GetInterpFile(theclass, subclass);
		}
		else if (ent->owner->client)//FIXME--COULD BE A FEMALE!!!
		{
			if (!stricmp(theclass, "enemy/meso"))
			{
				return "meso_player";
			}
			if (!stricmp(theclass, "enemy/female"))
			{
				return "fem_play";
			}
		}
	}
	return NULL;
}

const char *blownpart_ai::GetObjectSkin(char *theclass, char *subclass)
{
	if (ent && ent->owner && ent->owner->ghoulInst)
	{
		ggObjC *theParentObj=game_ghoul.FindObject(ent->owner->ghoulInst->GetGhoulObject());
		if (theParentObj)
		{
			return theParentObj->GetSkinName();
		}
	}
	return NULL;
}

void blownpart_ai::Init(edict_t *monster, char *theclass, char* subclass)
{
	generic_human_ai::Init(monster, theclass, subclass);

	//make sure I've got the same ghoul object as who I used to be attached to
	if (monster->owner && monster->owner->ghoulInst && monster->owner->ghoulInst->GetGhoulObject() != MyGhoulObj->GetMyObject())
	{
		ggObjC *theParentObj=game_ghoul.FindObject(monster->owner->ghoulInst->GetGhoulObject());
		gi.dprintf("Warning! Blown-off limb has object%s-%s-%s; parent has %s-%s-%s\n", MyGhoulObj->GetName(), MyGhoulObj->GetSubName(), MyGhoulObj->GetSkinName(), theParentObj->GetName(), theParentObj->GetSubName(), theParentObj->GetSkinName());
		game_ghoul.RemoveObjectInstances(monster);
		MyGhoulObj=game_ghoul.FindObject(monster->owner->ghoulInst->GetGhoulObject());
		game_ghoul.AddObjectInstance(MyGhoulObj, monster);
	}

	removeMe = false;
	Stopped = false;
	frozen=false;
	bFading=false;
	severTime = 0;
}
void debug_drawbox(edict_t* self,vec3_t vOrigin, vec3_t vMins, vec3_t vMaxs, int nColor);

void blownpart_ai::Think(edict_t &monster)
{
	if (bFading)
	{
		G_FreeEdict(&monster);
		return;
	}

	if (!severTime)
	{
		severTime = level.time - 0.1;
	}

	//stop bolt animation
	if (!frozen && (level.time > severTime + 1))
	{
		if (MyGhoulObj)
		{
			ggOinstC	*myInstance;

			myInstance=MyGhoulObj->FindOInst(monster.ghoulInst);
			if (myInstance)
			{
				frozen = true;
				myInstance->StopAnimatingAtAllBolts();
			}
		}
	}

  	if (removeMe)
	{
		int i;
		bool freeMe=true;
		edict_t *ent;
		vec3_t ent_facing, ent_to_me;

		//if it's been over 60 seconds since i was severed, git ridda me no matter what.
		if (level.time-severTime<BODY_PART_MAXLIFE)
		{
			//hasn't been too long--only remove if it won't look screwy to clients
			for (i=0 ; i<game.maxclients ; i++)
			{
				ent = &g_edicts[1+i];
				if (!ent->inuse)
				{
					continue;
				}
				VectorSubtract(ent->s.origin, monster.s.origin, ent_to_me);

				//if e's greater than 800 units away, assume he can't see me
				if (VectorLengthSquared(ent_to_me)>640000)
				{
					continue;
				}

				//if client can't possibly see me, will have no effect on whether i remove or no
				if (!gi.inPVS(monster.s.origin, monster.s.origin))
				{
					continue;
				}
				AngleVectors(ent->s.angles, ent_facing, NULL, NULL);
				VectorNormalize(ent_to_me);
				//if client is looking in my general dir, don't remove for sure
				if (DotProduct(ent_to_me, ent_facing)<.2)
				{
					freeMe = false;
					break;
				}
			}
		}

		if (freeMe)
		{
			FXA_SetFadeOut(&monster, BLOWN_PART_FADE);

			// Come back when we are done fading.
			monster.nextthink=level.time+BLOWN_PART_FADE+0.1;
			bFading = true;
			return;
		}
	}


	if (!Stopped)
	{
//			gi.sound (&monster, CHAN_BODY, gi.soundindex ("impact/gore/limbfall.wav"), .6, ATTN_NORM, 0);

		if (VectorLengthSquared(monster.velocity)<10||level.time-severTime>BODY_PART_MAXLIFE)
		{
		
			VectorClear(monster.avelocity);
			VectorClear(monster.velocity);


			if (monster.avelocity[0])
			{
				monster.s.angles[0]=-90;
				monster.s.angles[2]=0;
			}
			VectorClear(monster.avelocity);
			removeMe = true;
			monster.nextthink=level.time+5;
			if (!frozen)
			{
				monster.nextthink=level.time+.1;
			}
			


			// once we've stopped, switch over to the stand anim
	//		monster.ghoulInst->Play(GetBody()->GetSequence(*this, monster, &generic_move_stand),level.time,0.0,true,IGhoulInst::Hold, false, false);
			
			Stopped = true;
			return;
		}
		
		if (monster.s.angles[0] <= -85)
		{
			monster.s.angles[0]=-90;
			monster.s.angles[2]=0;
			VectorClear(monster.avelocity);
		}	
	}

/*	if ((Stopped) && (!(irand(0,100))))
	{
		if (((bodyhuman_c*)GetBody())->VerifySequence(*this, monster, &generic_move_limb_jitter, 0))
		{
			monster.ghoulInst->Play(GetBody()->GetSequence(*this, monster, &generic_move_limb_jitter),level.time,0.0,true,IGhoulInst::Hold, false, false);
			monster.nextthink=level.time+5;
			return;
		}
	}
/*I'm taking this out for now because it does some gay bounding-box crap that really sucks when it twitches
	
*/
		monster.nextthink = level.time + .1;
}


// use the ai_c constructor designed for the heli so that it won't call AddBody(). we want
//our generic_ghoul_dog_ai constructor to call that so we'll get a dog body
void generic_ghoul_dog_ai::Init(edict_t *monster, char *theclass, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	Matrix4	mat,mat1,mat2;

	ggOinstC *myInstance;

	ai_c::Init(monster, theclass, subclass);

//	group = new group_c();

	InitBBoxPreset(BBOX_PRESET_STAND,		-24, -24, -20, 24, 24, 20);	// set for dog
	InitBBoxPreset(BBOX_PRESET_CROUCH,		-16, -16, -16, 16, 16, 8);	// set for human
	InitBBoxPreset(BBOX_PRESET_PRONE,		-32, -32,  -8, 32, 32, 8);	// set for human
	InitBBoxPreset(BBOX_PRESET_ALLFOURS,	-24, -24,  -8, 24, 24, 8);	// set for human
	InitBBoxPreset(BBOX_PRESET_SIT,			-32, -32,  -8, 32, 32, 8);	// set for human
	InitBBoxPreset(BBOX_PRESET_LAYFRONT,	-32, -32, -26, 32, 32, 8);	// set for dog
	InitBBoxPreset(BBOX_PRESET_LAYBACK,		-24, -24, -32, 24, 24, -24);	// set for human
	InitBBoxPreset(BBOX_PRESET_LAYSIDE,		-32, -32, -28, 32, 32, 16);	// set for dog
	InitBBoxPreset(BBOX_PRESET_NONE,		0, 0, 0, 0, 0, 0);	// set for dog


	MyGhoulObj=game_ghoul.FindObject(theclass,subclass);

	if (!body || !MyGhoulObj)
	{
		return;
	}

	myInstance = game_ghoul.AddObjectInstance(MyGhoulObj, monster);


	if (!(monster->spawnflags & SPAWNFLAG_NOSCALE))
	{
		scale = gi.flrand(1.0F, 1.15F);
	}
	else
	{
		scale = 1.0;
	}
	monster->ghoulInst->GetXForm(mat);
	mat1=mat2=mat;
	mat1.Rotate(2,-M_PI*0.5);
	mat2.Scale(scale);
	mat.Concat(mat1, mat2);
	monster->ghoulInst->SetXForm(mat);

//	body->SetVoiceDirectories(*monster, "soldier", 2, DEATHVOICE_DEFAULT);//heh?
	body->SetRootBolt(*monster);

	// I guess precache sounds here
	gi.soundindex("Enemy/Dog/Bark1.wav");
	gi.soundindex("Enemy/Dog/Bark2.wav");
	gi.soundindex("Enemy/Dog/Bark3.wav");
	gi.soundindex("Enemy/Dog/Bite1.wav");
	gi.soundindex("Enemy/Dog/Bite2.wav");
	gi.soundindex("Enemy/Dog/Die.wav");
	gi.soundindex("Enemy/Dog/Pain.wav");

	//set up some statistics for the dog
	CAISkills newSkills(16, 72, 0, 1, 0, 0, 200, 1.0, AIS_NOSUPPRESS|AIS_CHEATERSEARCH|AIS_NODUCK,
		ESSA_ATTACK_DIRECTION, ESFK_IGNORE, 
		ESAT_APPROACH, ESOR_APPROACH, ESTC_HOLD_POSITION, ESEX_IGNORE);//irk

	mySkills = newSkills;

}

void generic_ghoul_dog_ai::AddBody(edict_t *monster)
{
	if (!monster)
	{
		return;
	}
//	if (!body)
//	{
//		body = new bodydog_c();
//	}
}

void dog_husky_ai::AddBody(edict_t *monster)
{
	if (!monster)
	{
		return;
	}
	if (!body)
	{
		body = new bodyhusky_c();
	}
}

void dog_rottweiler_ai::AddBody(edict_t *monster)
{
	if (!monster)
	{
		return;
	}
	if (!body)
	{
		body = new bodyrott_c();
	}
}

action_c *generic_ghoul_dog_ai::AttackAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t attackPos, vec3_t facing, float timeout, qboolean fullAnimation)
{
	return new 	running_melee_attack_action(od, oa, newanim, attackTarget, attackPos, facing, timeout,fullAnimation);
}

action_c *generic_ghoul_dog_ai::WalkAction(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing, float timeout, qboolean fullAnimation)
{
	return new dog_walk_action(od, oa, newanim, destination, facing, timeout,fullAnimation);
}


// use the ai_c constructor designed for the heli so that it won't call AddBody(). we want
//our cow_ai constructor to call that so we'll get a cow body
void cow_ai::Init(edict_t *monster, char *theclass, char* subclass)//i'm torn on whether all this stuff should go here or no
{
	Matrix4	mat,mat1,mat2;

	ggOinstC *myInstance;

	ai_c::Init(monster, theclass, subclass);

//	group = new group_c();

	InitBBoxPreset(BBOX_PRESET_STAND,		-24, -14, -20, 36, 14, 20);	// set for cow
	InitBBoxPreset(BBOX_PRESET_CROUCH,		-16, -16, -16, 16, 16, 8);	// set for human
	InitBBoxPreset(BBOX_PRESET_PRONE,		-32, -32,  -8, 36, 32, 8);	// set for dog
	InitBBoxPreset(BBOX_PRESET_ALLFOURS,	-24, -24,  -8, 24, 24, 8);	// set for human
	InitBBoxPreset(BBOX_PRESET_SIT,			-32, -32,  -8, 32, 32, 8);	// set for human
	InitBBoxPreset(BBOX_PRESET_LAYFRONT,	-32, -32, -26, 32, 32, 8);	// set for dog
	InitBBoxPreset(BBOX_PRESET_LAYBACK,		-24, -24, -32, 24, 24, -24);	// set for human
	InitBBoxPreset(BBOX_PRESET_LAYSIDE,		-22, -26, -20, 36, 26, 0);	// set for cow
	InitBBoxPreset(BBOX_PRESET_NONE,		0, 0, 0, 0, 0, 0);	// set for dog


	MyGhoulObj=game_ghoul.FindObject(theclass,subclass);

	if (!body || !MyGhoulObj)
	{
		return;
	}

	myInstance = game_ghoul.AddObjectInstance(MyGhoulObj, monster);


	if (!(monster->spawnflags & SPAWNFLAG_NOSCALE))
	{
		scale = 1.5;
	}
	else
	{
		scale = 1.5;
	}
	monster->ghoulInst->GetXForm(mat);
	mat1=mat2=mat;
	mat1.Rotate(2,-M_PI*0.5);
	mat2.Scale(scale);
	mat.Concat(mat1, mat2);
	monster->ghoulInst->SetXForm(mat);

//	body->SetVoiceDirectories(*monster, "soldier", 2, DEATHVOICE_DEFAULT);//heh?
	body->SetRootBolt(*monster);

	// I guess precache sounds here
	gi.soundindex("Enemy/Cow/Moo1.wav");
	gi.soundindex("Enemy/Cow/Moo2.wav");
	gi.soundindex("Enemy/Cow/Die.wav");
}

void cow_ai::AddBody(edict_t *monster)
{
	if (!monster)
	{
		return;
	}
	if (!body)
	{
		body = new bodycow_c();
	}
}



 
/*QUAKED m_x_romulan (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
*/
/*QUAKED m_x_rottweiler (1 .5 0) (-32 -32 -23) (32 32 14) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
an animal
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this guy dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_husky (1 .5 0) (-33 -33 -28) (33 33 16) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
an animal
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_germanshep (1 .5 0) (-33 -33 -28) (33 33 16) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
an animal
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_raiderdog (1 .5 0) (-33 -33 -28) (33 33 16) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
an animal
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_bull (1 .5 0) (-30 -14 -33) (56 14 26) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
an animal
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/

/*QUAKED m_sib_mtrooper1a (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_sib_mtrooper1b (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_sib_mtrooper2 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_sib_mguard1 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_sib_fguard2 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_sib_mguard3 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_sib_mguard4 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_sib_mcleansuit (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_sib_mmechanic (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_sib_escientist1 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_sib_fscientist2 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_sib_eofficer (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_afr_ecommander (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_afr_msniper (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_afr_msoldier1a (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_afr_msoldier1b (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_afr_msoldier2 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_afr_msoldier3 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_afr_eworker (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_afr_mbrute (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_afr_mrocket (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_kos_mgrunt1 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_kos_mgrunt2 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_kos_mgrunt3 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_kos_msniper1a (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_kos_msniper1b (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_kos_mcomtroop (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_kos_eofficer (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_kos_mbrute1a (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_kos_mbrute1b (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_kos_mmechanic (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_kos_mrebel (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_kos_mklaleader (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_kos_erefugee (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_tok_mman1 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_tok_mman2 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_tok_mhench1 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_tok_mhench2 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_tok_mkiller (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_tok_fassassin (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_tok_mninja (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_tok_mbrute (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_tok_fwoman1 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_tok_fwoman2 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_msoldier2b (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_mrepguard1b (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_mcommander (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_mbrute1a (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_mbrute1b (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_mrocket (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_msaddam (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_fwoman1 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_fwoman2 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_mman2 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_mbodyguard (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_mpolice (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_msoldier1 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_msoldier2a (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_mrepguard1a (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_eofficer (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_eman1 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_irq_moilworker (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_cas_mraider1 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_cas_mraider2a (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_cas_mraider2b (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_cas_mbrute (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_cas_ffemale (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_cas_mrocket (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_nyc_mpolitician (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_nyc_etourist (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_nyc_mpunk (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/

/*QUAKED m_nyc_epunk (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/

/*QUAKED m_nyc_eskinhead3 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_nyc_mskinhead1 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_nyc_mskinhead2a (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_nyc_mskinhead2b (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_nyc_fskinchick (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_nyc_ebum (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_nyc_estockbroker (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_nyc_mswat (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_nyc_mswatleader (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_nyc_fwoman (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_mcharacter (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_mcharacter_snow (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_mcharacter_desert (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_mraiderboss1 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human with desert eagle
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_mraiderboss2 (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human with mpg
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_ftaylor (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_mskinboss (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_mmerc (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_mhurtmerc (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
hawk, all beat up
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_msam (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE FACE_PLAYER
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.
FACE_PLAYER: always faces the player. set wait to 0 to turn off, 1 to turn back on.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/
/*QUAKED m_x_miraqboss (1 .5 0) (-16 -16 -40) (16 16 40) ECON_OVERRIDE BLIND DEAF NOSCALE TRIGGER_SPAWN SENSE_MUTE START_ACTIVE HOLD_POSITION NEVER_STARTLED HOSTAGE NO_ARMOR ARMOR_PICKUP NO_WEAPONS NO_WOUND HAS_PROJECTILE
a human
--------SPAWNFLAGS----------
ECON_OVERRIDE: specify cash value
BLIND: cannot see (turns off when script finishes)
DEAF: cannot hear (turns off when script finishes)
NOSCALE: don't apply random scaling; use standard size
TRIGGER_SPAWN: doesn't show up until triggered, or mentioned in a script
SENSE_MUTE: cone of vision is severely reduced
NEVER_STARTLED: don't play startled sequence for this guy
HOSTAGE: npc's only. use in conjunction with worldspawn.maxdeadhostages
NO_ARMOR: when this meso dies it won't spawn an item_equip_armor
ARMOR_PICKUP: we're only spawning this guy as an armor-yielding corpse. MUST HAVE UNIQUE TARGETNAME!!!
NO_WEAPONS: yup. you guessed it. no weapons.
NO_WOUND HAS_PROJECTILE: this guy will never drop his weapon. you'll have to kill him.

--------FIELDS--------------
alertradius: distance from player which will automatically alert monster
*/

void generic_monster_touch(edict_t *who, edict_t *user, cplane_s *theplane, mtexinfo_s *thetexinfo)
{
	if ((user->client) && (who->movetype != MOVETYPE_NONE) && (who->movetype != MOVETYPE_BOUNCE) && (who->movetype != MOVETYPE_PUSH) && (who->movetype != MOVETYPE_STOP))
	{
		who->ai->TouchMonster(user);
	}
}

void generic_monster_use(edict_t *who, edict_t *user, edict_t *activator)
{
	if ((user->client) && (who->movetype != MOVETYPE_NONE) && (who->movetype != MOVETYPE_BOUNCE) && (who->movetype != MOVETYPE_PUSH) && (who->movetype != MOVETYPE_STOP))
	{

		last_monster_used = who;

//		if (who->ai && OnSameTeam(who, user))
//		{
//			who->ai->SetAbusedByTeam(true);
//		}
		who->ai->UseMonster(user);

//#ifdef	_DEBUG
		if (ai_pathtest->value)
		{
			vec3_t	dir;
			vec3_t	kvel;
			float	mass;

			//push the guy
			if (who->mass < 50)
			{
				mass = 50;
			}
			else
			{
				mass = who->mass;
			}

			VectorSubtract(who->s.origin, user->s.origin, dir);
			VectorNormalize(dir);

			VectorScale (dir, 50000.0 / mass, kvel);

			VectorAdd (who->velocity, kvel, who->velocity);
			who->groundentity = NULL;
			//...end pushing the guy


			edict_t *curPt=NULL;
			gi.dprintf("starting path_corner anim list:\n");
			while ((curPt = G_Find (curPt, FOFS(classname), "path_corner")))
			{
				if (curPt->wait_action1)
					gi.dprintf("%s.ghl\n",curPt->wait_action1->ghoulSeqName);
				if (curPt->wait_action2)
					gi.dprintf("%s.ghl\n",curPt->wait_action2->ghoulSeqName);
				if (curPt->wait_action3)
					gi.dprintf("%s.ghl\n",curPt->wait_action3->ghoulSeqName);
				if (curPt->wait_action4)
					gi.dprintf("%s.ghl\n",curPt->wait_action4->ghoulSeqName);
				if (curPt->wait_action5)
					gi.dprintf("%s.ghl\n",curPt->wait_action5->ghoulSeqName);
				if (curPt->wait_action6)
					gi.dprintf("%s.ghl\n",curPt->wait_action6->ghoulSeqName);
				if (curPt->move_action)
					gi.dprintf("%s.ghl\n",curPt->move_action->ghoulSeqName);
			}
			gi.dprintf("end path_corner anim list, starting point_combat:\n");
			while ((curPt = G_Find (curPt, FOFS(classname), "point_combat")))
			{
				if (curPt->wait_action1)
					gi.dprintf("%s.ghl\n",curPt->wait_action1->ghoulSeqName);
				if (curPt->wait_action2)
					gi.dprintf("%s.ghl\n",curPt->wait_action2->ghoulSeqName);
				if (curPt->wait_action3)
					gi.dprintf("%s.ghl\n",curPt->wait_action3->ghoulSeqName);
				if (curPt->wait_action4)
					gi.dprintf("%s.ghl\n",curPt->wait_action4->ghoulSeqName);
				if (curPt->wait_action5)
					gi.dprintf("%s.ghl\n",curPt->wait_action5->ghoulSeqName);
				if (curPt->wait_action6)
					gi.dprintf("%s.ghl\n",curPt->wait_action6->ghoulSeqName);
				if (curPt->move_action)
					gi.dprintf("%s.ghl\n",curPt->move_action->ghoulSeqName);
			}
			gi.dprintf("end point_combat anim list.\n");
		}
//#endif //_DEBUG
	}
}

void generic_monster_activate(edict_t *who, edict_t *user, edict_t *activator)
{
	if (who && who->ai)
	{
		//if i'm not there, make me be there
		if ((!who->ai->IsActive())&&(who->spawnflags&SPAWNFLAG_TRIGGER_SPAWN))
		{
			// Destroy Any Who Oppose Me!!!
			CRadiusContent rad(who->s.origin, 40, 1, 0, 1);

			for(int i = 0; i < rad.getNumFound(); i++)
			{
				if(rad.foundEdict(i)->flags & FL_SPAWNED_IN)
				{	// this guys just escape
					rad.foundEdict(i)->ai->Escape(*rad.foundEdict(i));
				}
/*				else // maybe do this later, but I must admit this scares me a bit
				{	// these guys just die - they are in the way
					T_Damage(rad.foundEdict(i), who, who, vec3_up, who->s.origin, who->s.origin, 1000, 0, 0, 0);//?
				}*/
			}

			who->ai->Activate(*who);
		}
		//if i'm there, make me target the activator
		else
		{
			if (who->spawnflags&SPAWNFLAG_BLIND)
			{
				who->ai->NewSense(new normalsight_sense(), who);
				who->spawnflags&=(~SPAWNFLAG_BLIND);
			}
			if (who->spawnflags&SPAWNFLAG_DEAF)
			{
				who->ai->NewSense(new sound_sense(), who);
				who->spawnflags&=(~SPAWNFLAG_DEAF);
			}

			if (!who->ai->IsActive())
			{
				who->ai->Activate(*who);
			}
			if (activator)
			{
				who->ai->SetTargetTime(level.time, activator, activator->s.origin);

				//is this necessary still?  if i'm adding the settarget time above, don't know if this is needed...
				gmonster.MakeSound(who,activator->s.origin,level.time,activator,AI_SENSETYPE_SOUND_MAGICAL);
			}
		}
	}
}

void generic_monster_init (edict_t *self)
{
	edict_t	*ent;

	if (self->target)
	{
		ent = G_PickTarget (self->target);
		if (ent && !strcmp(ent->classname, "path_corner"))
		{
			self->ai->NewDecision(new pathidle_decision(ent), self);
		}
		else if (ent && !strcmp(ent->classname, "point_combat"))
		{
			self->ai->NewDecision(new pathcombat_decision(ent), self);
			self->ai->SetStartleability(false);
		}
	}

	if (self->combattarget)
	{
		ent = G_PickTarget (self->combattarget);
		if (ent && !strcmp(ent->classname, "point_combat"))
		{
			self->ai->NewDecision(new pathcombat_decision(ent), self);
			self->ai->SetStartleability(false);
		}
	}

	if (self->spawnflags  & SPAWNFLAG_NO_ARMOR)
	{	// needs to be set if we're going to perform a scripted kill on this joker
		if ( ((ai_c*)((ai_public_c*)self->ai))->GetBody())
		{
			((bodyorganic_c*)((ai_c*)((ai_public_c*)self->ai))->GetBody())->SetArmor(*self, ARMOR_NONE);
		}
	}

	if (self->spawnflags & SPAWNFLAG_HOSTAGE)
	{	// need to know when hostages get capped
		self->ai->SetHostage(true);
	}

	if (self->spawnflags&SPAWNFLAG_NEVER_STARTLED)
	{
		self->ai->SetStartleability(false);
	}

	if (!(self->spawnflags&SPAWNFLAG_DEAF))
	{
		self->ai->NewSense(new sound_sense(), self);
	}

	if (!(self->spawnflags&SPAWNFLAG_BLIND))
	{
		self->ai->NewSense(new normalsight_sense(), self);
	}

	if (self->spawnflags&SPAWNFLAG_HOLD_POSITION)
	{
		self->flags |= FL_NO_KNOCKBACK;
	}

	//magically see guys within my alertRadius
	//default to 75
	if (self->alertRadius<0.1)
	{
		self->alertRadius=75.0f;
	}
	self->ai->NewSense(new magicsight_sense(self->alertRadius), self);

	if (ai_pathtest->value)
	{
		self->ai->NewSense(new omniscience_sense(), self);
	}

	self->nextthink = level.time + FRAMETIME;
	self->plUse = generic_monster_use;
	self->touch = generic_monster_touch;
	self->use = generic_monster_activate;
}

void dekker_init (edict_t *self)
{
	generic_monster_init(self);
	self->ai->NewDecision(new dekker1_decision(), self);
	self->ai->NewDecision(new dekker2_decision(), self);
	self->ai->NewDecision(new dekker3_decision(), self);
	self->ai->NewDecision(new dekker4_decision(), self);
	self->ai->NewDecision(new dodge_decision(), self);

	self->ai->SetRank(3);

	self->ai->NewSense(new omniscience_sense(), self);
}

void generic_leader_init (edict_t *self)
{
	generic_monster_init(self);
	self->ai->NewDecision(new pursue_decision(), self);
	self->ai->NewDecision(new pointcombat_decision(), self);
	self->ai->NewDecision(new search_decision(), self);
	self->ai->NewDecision(new retreat_decision(), self);
	self->ai->NewDecision(new dodge_decision(), self);

	self->ai->SetRank(3);
}

void generic_grunt_init (edict_t *self)
{
	generic_monster_init(self);
	self->ai->NewDecision(new pursue_decision(), self);
	self->ai->NewDecision(new pointcombat_decision(), self);
	self->ai->NewDecision(new search_decision(), self);
	self->ai->NewDecision(new retreat_decision(), self);
	self->ai->NewDecision(new dodge_decision(), self);
}

void generic_heavyweapons_init (edict_t *self)
{
	generic_monster_init(self);
	self->ai->NewDecision(new pursue_decision(), self);
	self->ai->NewDecision(new pointcombat_decision(), self);
	self->ai->NewDecision(new search_decision(), self);
	self->ai->NewDecision(new retreat_decision(), self);
	self->ai->SetRank(2);
}

void generic_enemy_npc_init (edict_t *self)
{
	generic_monster_init(self);
	self->ai->NewDecision(new pursue_decision(), self);
	self->ai->NewDecision(new pointcombat_decision(), self);
	self->ai->NewDecision(new retreat_decision(), self);
}

void generic_npc_init (edict_t *self)
{
	generic_monster_init(self);
	self->ai->NewDecision(new pursue_decision(), self);
	self->ai->NewDecision(new retreat_decision(), self);
}

void generic_animal_init (edict_t *self)
{
	generic_monster_init(self);
	self->ai->NewDecision(new pursue_decision(), self);
}

void generic_armor_init (edict_t *self)
{
	// used when a meso is being created just so he can be dead and yield armor.
	//rather morbid, actually.

	// call magic ai function here to forcefeed a deatharmor_action into the entity
	if (self->ai)
	{
		// spawn an armor pickup over his corpse
		if ( ((ai_c*)(ai_public_c*)self->ai)->GetBody() )
		{	
			edict_t	*armor = G_Spawn();
			armor->count = self->count;
			VectorCopy(self->s.origin, armor->s.origin);
			armor->s.origin[2] += (self->maxs[2] - armor->mins[2] + 2);
			VectorCopy(self->s.angles, armor->s.angles);
			// set this armor's target to the targetname of the spawning corpse so we
			//can find it later in deatharmor_action::SafeToRemove()
			armor->target = self->targetname;
			// remove armor from corpse
			((ai_c*)(ai_public_c*)self->ai)->GetBody()->StripArmor();
			I_SpawnArmor(armor); 
		}

		((ai_c*)(ai_public_c*)self->ai)->NewDeathArmorAction(*self);
	}
	self->nextthink = level.time + FRAMETIME;
}


//set think to yer specific init, then call me.
void generic_monster_spawnnow (edict_t *self)
{
	self->s.renderfx = RF_GHOUL|RF_GHOUL_SHADOW;

	//basic physics stuff:
	//FIXME!!!! GAG! this should prolly be handled differently--i hadn't noticed the switch off movetype Before think is called
	self->movetype = MOVETYPE_STEP;

	//testing flying fellows
//	self->movetype = MOVETYPE_FLY;
//	self->flags|=FL_FLY;

	//this too--case statement magnet
	self->solid = SOLID_BBOX;

	self->friction = 0.01;	// this friction value must be changed on death to alleviate corpse floating

	VectorSet (self->mins, -16, -16, -32);
	VectorSet (self->maxs, 16, 16, 41);
	self->mass = 200;
	//end basic physics stuff

	//hold off on the ai til i've had a chance to init.--ever'thing need to get its name straight
	self->svflags |= SVF_MONSTER;
	self->nextthink = level.time + FRAMETIME;

	//this stuff is fine--basic actor set-up
	self->health = self->max_health = 100;
	self->gibbed = false;

	if (ai_goretest->value)
	{
		self->health = self->max_health = 1000;
	}

	self->takedamage = DAMAGE_AIM;

	if (!(self->spawnflags & SPAWNFLAG_TRIGGER_SPAWN) && level.time < 1)
	{
		gmonster.DropToFloor (self);
	}
	self->solid = SOLID_NOT;
	
	self->viewheight = 0;


	gi.linkentity (self);
	
/*	if (self->spawnflags & SPAWNFLAG_ECON_OVERRIDE)
	{
		game.GameStats->AddMonsterToStatsList(self, st.killedValue, st.survivalValue);
	}
	else
	{
		game.GameStats->AddMonsterToStatsList(self);
	}*/
}

// call me instead of monster_spawnnow if you're spawning a meso just to have him 
//be dead and yield armor
void generic_armor_spawnnow (edict_t *self)
{
	self->s.renderfx = RF_GHOUL;

	//basic physics stuff:
	//FIXME!!!! GAG! this should prolly be handled differently--i hadn't noticed the switch off movetype Before think is called
	self->movetype = MOVETYPE_STEP;

	//this too--case statement magnet
	self->solid = SOLID_BBOX;

	self->friction = 0.01;  // this friction value must be changed on death to alleviate corpse floating

	VectorSet (self->mins, -16, -16, -32);
	VectorSet (self->maxs, 16, 16, 41);
	self->mass = 200;
	//end basic physics stuff

	//hold off on the ai til i've had a chance to init.--ever'thing need to get its name straight
	self->svflags |= SVF_MONSTER;
	self->nextthink = level.time + FRAMETIME;

	//this stuff is fine--basic actor set-up
	self->health = 0;
	self->max_health = 100;

	if (ai_goretest->value)
	{
		self->health = self->max_health = 1000;
	}

	self->takedamage = DAMAGE_AIM;

	if (!(self->spawnflags & SPAWNFLAG_TRIGGER_SPAWN) && level.time < 1)
	{
		gmonster.DropToFloor (self);
	}
	
	self->viewheight = 0;
	self->solid = SOLID_NOT;


	gi.linkentity (self);
	
}

void generic_dog_spawnnow (edict_t *self, char *subclass)
{
	char dogClass[128];
	int nAICode;// = AI_GENERIC_DOG;

	//get the ai set up right away, so caching of ghoul stuff can happen
	generic_monster_spawnnow (self);
	sprintf(dogClass,"enemy/%s", subclass);

	// there's doubtlessly a more clever way to do this...
	if (stricmp(subclass, "husky") == 0)
	{
		nAICode = AI_DOG_HUSKY;
	}
	else if (stricmp(subclass, "rottweiler") == 0)
	{
		nAICode = AI_DOG_ROTTWEILER;
	}
	self->ai = ai_c::Create(nAICode, self, dogClass, subclass);//new generic_ghoul_dog_ai(self, "enemy/husky", subclass);
}

void generic_cow_spawnnow (edict_t *self, char *subclass)
{
	char cowClass[128];
	int nAICode = AI_GENERIC_COW;

	//get the ai set up right away, so caching of ghoul stuff can happen
	generic_monster_spawnnow (self);
	sprintf(cowClass,"enemy/%s", subclass);
	self->ai = ai_c::Create(nAICode, self, cowClass, subclass);//new generic_ghoul_dog_ai(self, "enemy/husky", subclass);
}

void SP_m_x_romulan (edict_t *self)
{
	generic_monster_spawnnow(self);
	self->ai = ai_c::Create(AI_MESO_UGNSNIPER, self, "enemy/meso", "romulan");
	self->think = generic_npc_init;
}


void SP_m_x_husky (edict_t *self)
{
	generic_dog_spawnnow(self,"husky");
	VectorSet(self->mins, -33, -33, -28);
	VectorSet(self->maxs, 33, 33, 16);
	self->think = generic_animal_init;
}

void SP_m_x_rottweiler (edict_t *self)
{
	generic_dog_spawnnow(self,"rottweiler");
	VectorSet(self->mins, -26, -26, -20);
	VectorSet(self->maxs, 26, 26, 11);
	self->think = generic_animal_init;
}



void SP_m_x_germanshep (edict_t *self)
{
	SP_m_x_husky(self);
}

void SP_m_x_raiderdog (edict_t *self)
{
	SP_m_x_husky(self);
}

void SP_m_x_bull (edict_t *self)
{
	generic_cow_spawnnow(self, "bull");
	VectorSet(self->mins, -30, -14, -33);
	VectorSet(self->maxs, 56, 14, 26);
	self->think = generic_animal_init;
	BboxRotate(self);
}



