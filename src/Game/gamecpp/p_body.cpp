//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
///
///	player body
///
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


#include "g_local.h"
#include "ai_private.h"
#include "p_body.h"
#include "m_generic.h"
#include "..\qcommon\ef_flags.h"

#ifdef __PLAYERTEST_3DPERSON

#define THIRDPERSON_HACK true

#else

#define THIRDPERSON_HACK false

#endif

//////////////////////////////////////////////////////////////////////
//	Local-use functions
//////////////////////////////////////////////////////////////////////

static bool BodyPreUse(edict_t *ent)
{
#ifdef __PLAYERTEST_3DPERSON
	//for testing purposes only
	if (ent && ent->client && ent->client->ps.bod)
	{
		ent->ghoulInst = ent->client->ps.bod;
		return ent->ghoulInst!=NULL;
	}
	return false;
#else
	//use this line for real--the rest is for testing
	return true;
#endif
}

static void BodyPostUse(edict_t *ent)
{
#ifdef __PLAYERTEST_3DPERSON
	if (ent)
	{
		ent->ghoulInst = NULL;
	}
#endif
}

static void AdjustDestForSpeed(edict_t *ent, vec3_t dest)
{
	vec3_t to_dest;
	VectorSubtract(dest, ent->s.origin, to_dest);
	to_dest[2]=0;
	if (VectorNormalize(to_dest)>0.1)
	{
		if (ent->client->running)
		{
			VectorScale(to_dest, 100.0f, to_dest);
		}
		else
		{
			VectorScale(to_dest, 1.0f, to_dest);
		}
		to_dest[2]=dest[2]-ent->s.origin[2];
		VectorAdd(to_dest, ent->s.origin, dest);
	}
}

static GhoulSpeed GetAnimSpeedForMove(edict_t *ent, vec3_t to_dest, float moveScale)
{
	//yuck. the damn scales were not implemented meaning the same thing. now i'm dizzy (well, dizzier). --sfs
	float adjustedMoveScale=1.0/moveScale;

//	gi.dprintf("hello! player speed scale: %f!\n", adjustedMoveScale);

	if (adjustedMoveScale > 1.875)
	{
//		gi.dprintf("hello! player anim scale: gs16Over8!\n");
		return gs16Over8;
	}
	else if (adjustedMoveScale > 1.75)
	{
//		gi.dprintf("hello! player anim scale: gs15Over8!\n");
		return gs15Over8;
	}
	else if (adjustedMoveScale > 1.625)
	{
//		gi.dprintf("hello! player anim scale: gs14Over8!\n");
		return gs14Over8;
	}
	else if (adjustedMoveScale > 1.5)
	{
//		gi.dprintf("hello! player anim scale: gs13Over8!\n");
		return gs13Over8;
	}
	else if (adjustedMoveScale > 1.375)
	{
//		gi.dprintf("hello! player anim scale: gs12Over8!\n");
		return gs12Over8;
	}
	else if (adjustedMoveScale > 1.25)
	{
//		gi.dprintf("hello! player anim scale: gs11Over8!\n");
		return gs11Over8;
	}
	else if (adjustedMoveScale > 1.125)
	{
//		gi.dprintf("hello! player anim scale: gs10Over8!\n");
		return gs10Over8;
	}
	else if (ent->client->moveScale > 1.0f)
	{
//		gi.dprintf("hello! player anim scale: gs9Over8!\n");
		return gs9Over8;
	}
	else if (adjustedMoveScale > 0.875)
	{
//		gi.dprintf("hello! player anim scale: gsOne!\n");
		return gsOne;
	}
	else if (adjustedMoveScale > 0.75)
	{
//		gi.dprintf("hello! player anim scale: gs7Over8!\n");
		return gs7Over8;
	}
	else if (adjustedMoveScale > 0.625)
	{
//		gi.dprintf("hello! player anim scale: gs6Over8!\n");
		return gs6Over8;
	}
	else if (adjustedMoveScale > 0.5)
	{
//		gi.dprintf("hello! player anim scale: gs5Over8!\n");
		return gs5Over8;
	}
	else
	{
//		gi.dprintf("hello! player anim scale: gs4Over8!\n");
		return gs4Over8;
	}
}

static GhoulSpeed GetAnimSpeedForNonMove(edict_t *ent, vec3_t to_dest, float moveScale)
{
//	return SetAnimSpeedForNonMove(ent, dest, moveScale);
	return gsOne;
}

static void SetAnimSpeed(edict_t *ent, vec3_t dest, mmove_t *move)
{
	if (ent->ghoulInst && ent->client)
	{
		float tempMoveScale = /*ent->client->moveScale * */dm->clientGetMovescale(ent);
		vec3_t to_dest;
		VectorSubtract(dest, ent->s.origin, to_dest);
		to_dest[2]=0;

		if (VectorNormalize(to_dest)>0.1)
		{
			if (ent->client->running)
			{
				ent->ghoulInst->SetSpeed(GetAnimSpeedForMove(ent, to_dest, tempMoveScale));
				VectorScale(to_dest, 100.0f, to_dest);
			}
			else
			{
				//slow down crouchmove anims
				if (move->bbox==BBOX_PRESET_CROUCH && (move->actionFlags&ACTFLAG_LOOPANIM))
				{
					tempMoveScale*=0.75;
				}

				ent->ghoulInst->SetSpeed(GetAnimSpeedForMove(ent, to_dest, tempMoveScale));
				VectorScale(to_dest, 1.0f, to_dest);
			}
			to_dest[2]=dest[2]-ent->s.origin[2];
//			VectorAdd(to_dest, ent->s.origin, dest);
		}
		else
		{
//			ent->ghoulInst->SetSpeed(gs16Over8);
			ent->ghoulInst->SetSpeed(GetAnimSpeedForNonMove(ent, to_dest, tempMoveScale));
		}
	}
}

static void RegisterPlayerGameSkins(ggObjC *MyGhoulObj, char *facebase)
{
	//gorezones
	MyGhoulObj->RegisterSkin("gz","gz_sing1");
	MyGhoulObj->RegisterSkin("gz","gz_sing2");
	MyGhoulObj->RegisterSkin("gz","gz_sing3");
	MyGhoulObj->RegisterSkin("gz","gz_sing_k");
	MyGhoulObj->RegisterSkin("gz","gz_multi1");
	MyGhoulObj->RegisterSkin("gz","gz_multi2");
	MyGhoulObj->RegisterSkin("gz","gz_multi3");
	MyGhoulObj->RegisterSkin("gz","gz_multi_k");
	MyGhoulObj->RegisterSkin("gz","gz_mass1");
	MyGhoulObj->RegisterSkin("gz","gz_mass2");
	MyGhoulObj->RegisterSkin("gz","gz_mass3");
	MyGhoulObj->RegisterSkin("gz","gz_mass_k");
	//caps
	MyGhoulObj->RegisterSkin("c","c_blood");
	MyGhoulObj->RegisterSkin("c","c_black");

	//faces--note that normal face is NOT precached--it's expected in the skin-setting section!
	if (facebase && facebase[0])
	{
		char facetemp[100];

		Com_sprintf(facetemp, 100, "%sd",facebase);
		MyGhoulObj->RegisterSkin("f", facetemp);
		
		//pain face not vital, but good
		if ((mskins_expression_limit->value-0.1)<EXPRESSION_PRIORITY_HIGH)
		{
			sprintf(facetemp,"%sp",facebase);
			MyGhoulObj->RegisterSkin("f",facetemp);
		}

		//blink face total fluff
		if ((mskins_expression_limit->value-0.1)<EXPRESSION_PRIORITY_LOWEST)
		{
			sprintf(facetemp,"%sb",facebase);
			MyGhoulObj->RegisterSkin("f",facetemp);
		}
	}
}

static bbox_preset GetGoalBBox(edict_t *ent)
{
	if (ent && ent->client && ent->client->ps.pmove.pm_flags & PMF_DUCKED)
	{
		return BBOX_PRESET_CROUCH;
	}
	return BBOX_PRESET_STAND;
}

//this func assumes i've already done pre-use and will do post-use outside
static mmove_t *GetLeaningSequence(edict_t *ent, vec3_t dest, vec3_t face, bbox_preset goal_bbox)
{
	mmove_t *curMove;
	qboolean isLeaningLeft, isLeaningRight;
	qboolean wasLeaningLeft, wasLeaningRight;
	attacks_e curWeapon;
	bbox_preset curBbox;
	if (!ent || !ent->client || !ent->client->body || !(curMove=ent->client->body->GetMove()))
	{
		return NULL;
	}

//	gi.dprintf("lean start...");

	curBbox=curMove->bbox;
	curWeapon = ent->client->body->GetRightHandWeapon(*ent);
	isLeaningLeft = ent->viewside > 0.0 && ent->client->ps.kick_angles[ROLL] < -5.0;
	isLeaningRight = ent->viewside < 0.0 && ent->client->ps.kick_angles[ROLL] > 5.0;
	wasLeaningLeft = (curMove->bodyPosition==BODYPOS_PLAYERLEANLEFT);
	wasLeaningRight = (curMove->bodyPosition==BODYPOS_PLAYERLEANRIGHT);

	if (curBbox == BBOX_PRESET_STAND)
	{
		if (isLeaningLeft && curBbox == goal_bbox)//leaning left
		{
			if (wasLeaningLeft)
			{
				if (!ent->client->body->IsAnimationFinished()&&!(curMove->actionFlags&ACTFLAG_LOOPANIM))
				{
					return curMove;
				}
				else if (curWeapon == ATK_ROCKET)
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanlt_mid_l);
				}
				else
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanlt_mid_a);
				}
			}
			else
			{
				if (curWeapon == ATK_ROCKET)
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanlt_to_l);
				}
				else
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanlt_to_a);
				}
			}
		}
		else if (isLeaningRight && curBbox == goal_bbox)//leaning right
		{
			if (wasLeaningRight)
			{
				if (!ent->client->body->IsAnimationFinished()&&!(curMove->actionFlags&ACTFLAG_LOOPANIM))
				{
					return curMove;
				}
				else if (curWeapon == ATK_ROCKET)
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanrt_mid_l);
				}
				else
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanrt_mid_a);
				}
			}
			else
			{
				if (curWeapon == ATK_ROCKET)
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanrt_to_l);
				}
				else
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanrt_to_a);
				}
			}
		}
		else
		{
			if (wasLeaningLeft)
			{
				if (curWeapon == ATK_ROCKET)
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanlt_frm_l);
				}
				else
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanlt_frm_a);
				}
			}
			if (wasLeaningRight)
			{
				if (curWeapon == ATK_ROCKET)
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanrt_frm_l);
				}
				else
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanrt_frm_a);
				}
			}
		}
	}

	if (curBbox == BBOX_PRESET_CROUCH)
	{
		if (isLeaningLeft && curBbox == goal_bbox)//leaning left
		{
			if (wasLeaningLeft)
			{
				if (!ent->client->body->IsAnimationFinished()&&!(curMove->actionFlags&ACTFLAG_LOOPANIM))
				{
					return curMove;
				}
				else if (curWeapon == ATK_ROCKET)
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanclt_mid_l);
				}
				else
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanclt_mid_a);
				}
			}
			else
			{
				if (curWeapon == ATK_ROCKET)
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanclt_to_l);
				}
				else
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanclt_to_a);
				}
			}
		}
		else if (isLeaningRight && curBbox == goal_bbox)//leaning right
		{
			if (wasLeaningRight)
			{
				if (!ent->client->body->IsAnimationFinished()&&!(curMove->actionFlags&ACTFLAG_LOOPANIM))
				{
					return curMove;
				}
				else if (curWeapon == ATK_ROCKET)
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleancrt_mid_l);
				}
				else
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleancrt_mid_a);
				}
			}
			else
			{
				if (curWeapon == ATK_ROCKET)
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleancrt_to_l);
				}
				else
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleancrt_to_a);
				}
			}
		}
		else
		{
			if (wasLeaningLeft)
			{
				if (curWeapon == ATK_ROCKET)
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanclt_frm_l);
				}
				else
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleanclt_frm_a);
				}
			}
			if (wasLeaningRight)
			{
				if (curWeapon == ATK_ROCKET)
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleancrt_frm_l);
				}
				else
				{
					return ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_playerleancrt_frm_a);
				}
			}
		}
	}
	return NULL;
}


static bool PB_PrecachePlayer(char *playerName)
{
	char	modname[100];

	Com_sprintf(modname, 100, "%s.gpm", playerName);


	IPlayerModelInfoC *aPlayer=gi.NewPlayerModelInfo(modname);
	if (!aPlayer || !aPlayer->IsLoaded())
	{
		//what player?
		delete aPlayer;
		return false;
	}

	{
		char	portname[100];
		Com_sprintf(portname,100, "menus/players/%s.rmf",playerName);
		TheGhoul->AddFileForDownLoad(portname);
		Com_sprintf(portname,100, "ghoul/pmodels/%s.gpm",playerName);
		TheGhoul->AddFileForDownLoad(portname);
		Com_sprintf(portname,100, "ghoul/pmodels/portraits/%s.m32",playerName);
		TheGhoul->AddFileForDownLoad(portname);
		TeamInfoC team;
		aPlayer->GetTeamInfo(team);
		Com_sprintf(portname,100, "ghoul/pmodels/teamicons/%s.m32",team.name);
		TheGhoul->AddFileForDownLoad(portname);
		Com_sprintf(portname,100, "pics/menus/teamicons/%s.m32",team.name);
		TheGhoul->AddFileForDownLoad(portname);
	}

	ggObjC	*MyGhoulObj;
	ggOinstC* myInstance;

	Matrix4	mat,mat1,mat2;

	SkinInfoC tempSkin;
	BoltOnInfoC tempBoltOn;
	IPlayerModelInfoC	*boltModel;

	edict_t *tempEnt=G_Spawn();
	body_c	*tempBody=0;
	//this is where the model info should be really gotten
	int counter;

	//register stuff here.


	//game-stuff: allocate appropriate body type
	if (!stricmp(aPlayer->GetGameGHB(),"fem_play"))
	{
		tempBody=new bodyfemale_c();
	}
	else
	{
		tempBody=new bodymeso_c();
	}

	//this creates (or finds, if it's there already) the ghoul object, registers the sequences we'll need
	// and remembers which _poff file we'll need (to turn parts off when we create an inst)

	MyGhoulObj=game_ghoul.FindObject(aPlayer->GetGHBDir(),aPlayer->GetSeqGSQ(),false,modname,aPlayer->GetGameGHB());
	
	//game-stuff:  make sure the body we created is connected to ent
	if(tempBody)
	{
		tempBody->SetOwner(tempEnt);
	}

	//if creating (or finding) the object failed, we're screwed.
	if (!MyGhoulObj)
	{
		gi.error("Couldn't find player model %s/%s.\n",aPlayer->GetGHBDir(),aPlayer->GetSeqGSQ());
	}

	//this could get complicated--what with the checking for repeat skins--so only try & register if it's not locked
	if (!MyGhoulObj->IsRegistrationLocked())
	{
		//game-stuff:  register gorezone, cap and alternate face textures.
		RegisterPlayerGameSkins(MyGhoulObj, aPlayer->GetFaceSet());

		MyGhoulObj->RegisterSkin("b", "b_dm_armor");

		//YEE-HA!  check this code out!  it makes skins not register twice.
		//the story is i couldn't figure out how to make a multi-dimensional, dynamically-sized array, So i just make a big one-dimensional array and use different pieces of it as though they were separate arrays. yick.
		if (aPlayer->GetNumSkins()>0)
		{
			int		counter2;
			char	*skinnames=0;
			char	*matnames=0;
			bool	registerThisOne;

			skinnames = new char[aPlayer->GetNumSkins()*100];
			matnames = new char[aPlayer->GetNumSkins()*100];
			//make sure all the skins are registered before we go and create any instances
			for (counter = 1; counter <= aPlayer->GetNumSkins(); counter++)
			{
				registerThisOne=true;
				if (aPlayer->GetSkin(counter, tempSkin))
				{
					for (counter2=0;counter2<counter-1;counter2++)
					{
						if ((skinnames[counter2*100]&&matnames[counter2*100]
							&&(!stricmp(&skinnames[counter2*100],tempSkin.skinName))
							&&(!stricmp(&matnames[counter2*100],tempSkin.matName)))
							||(!stricmp("b_dm_armor",tempSkin.skinName)))
						{
							registerThisOne=false;
							break;
						}
					}

					if (registerThisOne)
					{
						//achtung! when setting skins on specific parts, there could be repeats--bad when registering!!!
						MyGhoulObj->RegisterSkin(tempSkin.matName, tempSkin.skinName);
					}
					else
					{
						gi.dprintf("Narrowly avoiding re-register on mat \"%s\", skin \"%s\"\n", tempSkin.matName, tempSkin.skinName);
					}
					strcpy(&skinnames[(counter-1)*100],tempSkin.skinName);
					strcpy(&matnames[(counter-1)*100],tempSkin.matName);
				}
				else
				{
					skinnames[(counter-1)*100]=0;
					matnames[(counter-1)*100]=0;
					//ERROR!
				}
			}
			delete [] skinnames;
			delete [] matnames;
			skinnames=0;
			matnames=0;
		}

		//game_ghoul has this nice lock on it, so I don't have to worry about
		//registering stuff after, say, an inst has been created and then destroyed
		MyGhoulObj->RegistrationLock();
	}

	//make the ghoulInst
	myInstance = game_ghoul.AddObjectInstance(MyGhoulObj, tempEnt);

	tempEnt->s.renderfx = RF_GHOUL;

	//don't bother setting bolt to quake_ground.

	//don't bother messing w/ the xform--never see this guy anyway.

	//game-stuff: if the body-creation failed, we're screwed
	if (!tempBody)
	{
		delete aPlayer;
		G_FreeEdict(tempEnt);
		return false;
	}


	//don't bother toggling extra parts from file

	//don't bother setting skins from file

	//don't bother remembering which set of faces to use

	//cache all the bolt stuff from file
	for (counter = aPlayer->GetNumBoltOns(); counter>0; counter--)
	{
		if (aPlayer->GetBoltOn(counter, tempBoltOn)&&(boltModel=gi.NewPlayerModelInfo(tempBoltOn.childModelInfo)))
		{
			{
				char	portname[100];
				Com_sprintf(portname,100, "ghoul/pmodels/%s",tempBoltOn.childModelInfo);
				TheGhoul->AddFileForDownLoad(portname);
			}
			if (boltModel->IsLoaded()&&boltModel->GetGameGHB()[0])
			{
				boltModel->GetSkin(1, tempSkin);
				tempBody->AddBoltedItem(*tempEnt, tempBoltOn.parentBolt, boltModel->GetGHBDir(), boltModel->GetGameGHB(), tempBoltOn.childBolt, myInstance, tempSkin.skinName, tempBoltOn.scale);
			}
			else
			{
				//couldn't load bolt-on info file!
			}
			delete boltModel;
			boltModel=NULL;
		}
		else
		{
			//ERROR!
		}
	}

	//game-stuff:  add voice stuff
	VoiceInfoC tempVoiceInfo;
	aPlayer->GetVoiceInfo(tempVoiceInfo);
	tempBody->SetVoiceDirectories(*tempEnt, "", 1, DEATHVOICE_PLAYER, tempVoiceInfo.voiceDir);

	tempBody->SetRootBolt(*tempEnt);

	myInstance->GetInstPtr()->SetUserData(tempEnt);

	delete aPlayer;
	delete tempBody;
	G_FreeEdict(tempEnt);

	return true;
}

//////////////////////////////////////////////////////////////////////
//	Crazily-exported-to-player.dll functions
//////////////////////////////////////////////////////////////////////

void ShowReload(edict_t *ent)
{
	vec3_t dest, face;

	if (!ent)
	{
		return;
	}

	VectorCopy(ent->velocity, dest);
	VectorAdd(dest, ent->s.origin, dest);
	AngleVectors(ent->client->ps.viewangles, face, NULL, NULL);
	VectorMA(ent->s.origin, 100, face, face);

	PB_PlaySequenceForReload(ent, dest, face);
}

void ShowItemUse(edict_t *ent, int itemType)
{
	vec3_t dest, face;

	if (!ent)
	{
		return;
	}

	VectorCopy(ent->velocity, dest);
	VectorAdd(dest, ent->s.origin, dest);
	AngleVectors(ent->client->ps.viewangles, face, NULL, NULL);
	VectorMA(ent->s.origin, 100, face, face);

	PB_PlaySequenceForItemUse(ent, dest, face, itemType);
}

//////////////////////////////////////////////////////////////////////
//	Public-use functions
//////////////////////////////////////////////////////////////////////

void FreeEdictBoltData(edict_t *ed);

static char defModName[100];

void PB_PrecacheAllPlayerModels(void)
{
	defModName[0]=0;

	if (!THIRDPERSON_HACK && (!dm || !dm->isDM()))
	{	// only see players in deathmatch or for crazy 3rd person camera hack
		return;
	}

	int		i,length;
	char	*buffer,*s;

	// No pmodel list file specified so just return.

	if(!sv_pmodlistfile->string[0])
	{
		gi.error("*************************\nNo pmodel file specified!\n*************************\n");
				
		return;
	}

	// Ok, try to load the maplist file.

	if((length=gi.FS_LoadFile(va("%s.gpl",sv_pmodlistfile->string),(void **)&buffer))==-1)
	{
		gi.error("*************************\nCould not open %s.gpl\n*************************\n",
				  sv_pmodlistfile->string);
				
		return;
	}

	s=buffer;
	i=0;

	// Loop through and validate map names, adding valid map names to string in sv_maplist.

	while(s)
	{
		char  shortname[100];

		strcpy(shortname,COM_Parse(&s));

		if(strlen(shortname))
		{
			if (PB_PrecachePlayer(shortname)&&!defModName[0])
			{
				Com_sprintf(defModName, sizeof(defModName), "%s.gpm", shortname);
			}
		}
	}

	// Clean up...
	gi.FS_FreeFile(buffer);

	// Now do the alt model list (no worries if we can't find or load it, it's optional)...

	//if the cvar isn't even set, don't worry about it.
	if(sv_altpmodlistfile->string[0])
	{
		// Ok, try to load the maplist file.

		if((length=gi.FS_LoadFile(va("%s.gpl",sv_altpmodlistfile->string),(void **)&buffer))!=-1)
		{
			s=buffer;
			i=0;

			// Loop through and validate map names, adding valid map names to string in sv_maplist.

			while(s)
			{
				char  shortname[100];

				strcpy(shortname,COM_Parse(&s));

				if(strlen(shortname))
				{
					if (PB_PrecachePlayer(shortname)&&!defModName[0])
					{
						Com_sprintf(defModName, sizeof(defModName), "%s.gpm", shortname);
					}
				}
			}

			// Clean up...
			gi.FS_FreeFile(buffer);
		}
	}

	//eh? no default player?
	if (!defModName[0]&&PB_PrecachePlayer("mullins"))
	{
		strcpy(defModName, "mullins.gpm");
	}
}


#define PLAYER_CORPSE_FADE	1.0F


void PB_FadeCorpse(edict_t *ent)
{
	IGhoulInst *inst=ent->ghoulInst;

	// See if already fading
	if (ent->s.effects & EF_FADE)
	{
		if (FXA_CheckFade(ent))
		{	// Remove that sucker
			gi.unlinkentity (ent);

			if (inst)
				game_ghoul.RemoveObjectInstances(ent);

			// FIXME: Will probably need to do some more work here... i.e. falling bodies.

			gi.unlinkentity (ent);
			ent->s.renderfx=0;
			ent->solid=SOLID_NOT;
//			ent->owner=g_edicts;
			ent->movetype=MOVETYPE_NONE;
			ent->takedamage=DAMAGE_NO;
			ent->think=NULL;
			
			gi.linkentity(ent);
			
			return;
		}
		else
		{	// Wait a tiny bit more
			ent->nextthink=level.time+0.1F;

			return;
		}
	}

	// Else set the corpse up to fade.
	if(inst)
	{
		FXA_SetFadeOut(ent, PLAYER_CORPSE_FADE);		// Three second fade out time.

		// Come back when we are done fading.
		ent->nextthink=level.time+PLAYER_CORPSE_FADE+0.1;
	}
	else
	{	// No way to fade, just remove.

		// FIXME: Will probably need to do some more work here... i.e. falling bodies.

		gi.unlinkentity (ent);
		ent->s.renderfx=0;
		ent->solid=SOLID_NOT;
//		ent->owner=g_edicts;
		ent->movetype=MOVETYPE_NONE;
		ent->takedamage=DAMAGE_NO;
		ent->think=NULL;
		
		gi.linkentity(ent);
	}

	return;
}

/*
void CorpseTrackPostion(edict_t *ent)
{
	if (level.time-ai_corpselife->value > ent->gib_health)
	{
		ent->think = PB_FadeCorpse;
	}
	ent->nextthink=level.time+0.1;
	if (level.time-ai_corpselife->value*0.1 < ent->gib_health)
	{
		gi.dprintf("corpse at: %s\n",vtos(ent->s.origin));
	}
}
*/

/*
void PB_MakeCorpse (edict_t *ent)
{
	edict_t		*body;
	ggObjC		*parentObj;

	// grab a body que and cycle to the next one
	body = &g_edicts[(int)maxclients->value + level.body_que + 1];
	level.body_que = (level.body_que + 1) % BODY_QUEUE_SIZE;

	// FIXME: send an effect on the removed body

	BodyPreUse(ent);

	gi.unlinkentity (ent);

	// kef -- just before we get rid of this joker's body, free any
	//knives that are stuck in him.
	if (ent && ent->client && ent->client->body)
	{
		ent->client->body->FreeKnives(*ent);
	}

	game_ghoul.RemoveObjectInstances(body);

	gi.unlinkentity (body);
	VectorCopy(ent->s.angles, body->s.angles);
	body->s.angles[PITCH]=0;
	VectorCopy(ent->s.origin, body->s.origin);
	body->s.renderfx=ent->s.renderfx;
	body->s.number = body - g_edicts;

	//safe, slow way of doing corpseInsts

//	if (ent->ghoulInst && (parentObj=game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject())))
//	{
//		//HEY!!!! this isn't enough! no bolt-ons, and won't work if client's death anim doesn't end in prone-ish position!!!!
//		ggOinstC	*newInst=parentObj->CloneInstance(body, ent->ghoulInst);
//		ggOinstC	*oldInst=parentObj->FindOInst(ent->ghoulInst);
//
//		if (newInst&&oldInst)
//		{
//			oldInst->CopyBoltsTo(newInst);
//		}
//	}


	//dangerous, fast way of doing corpse ghoulInsts
	if (ent->ghoulInst && (parentObj=game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject())))
	{
		ggOinstC	*oldInst=parentObj->FindOInst(ent->ghoulInst);
		if (oldInst)
		{
			oldInst->ChangeOwnerTo(body);
		}
	}

	//fast ghoul lighting
//	body->s.renderfx |= RF_LIGHT_FASTEST;	

	body->svflags = ent->svflags;
	VectorCopy (ent->mins, body->mins);
	VectorCopy (ent->maxs, body->maxs);
	VectorCopy (ent->absmin, body->absmin);
	VectorCopy (ent->absmax, body->absmax);
	VectorCopy (ent->size, body->size);
	body->solid = ent->solid;
	body->clipmask = ent->clipmask;
	body->owner = ent->owner;
	body->movetype = ent->movetype;
	body->think = PB_FadeCorpse;
	body->nextthink=level.time+ai_corpselife->value;

//	body->think = CorpseTrackPostion;
//	body->nextthink=level.time+0.1;
//	body->gib_health=level.time;

	body->takedamage = DAMAGE_YES;

	gi.linkentity (body);

	BodyPostUse(ent);
}
*/

//working on getting player corpses to use ai--may ditch this attempt as too much work/complexity for too little payoff
void DoNothing(edict_t *ent)
{
	ent->nextthink=level.time+0.1F;
}
/*

//ai-using corpses--if this worked, we'd be usin' it!
void PB_MakeCorpse (edict_t *ent)
{
	edict_t		*body;
//	ggObjC		*parentObj;

	// grab a body que and cycle to the next one
	body = &g_edicts[(int)maxclients->value + level.body_que + 1];
	level.body_que = (level.body_que + 1) % BODY_QUEUE_SIZE;

	// FIXME: send an effect on the removed body

	BodyPreUse(ent);

	gi.unlinkentity (ent);

	gi.unlinkentity (body);

//	body->owner = ent->owner;
	body->owner = ent;

	if (body->ai)
	{
		body->ai.Destroy();
		body->ai=NULL;
	}

	ent->mass=1000;


	body->svflags = ent->svflags;
	VectorCopy (ent->mins, body->mins);
	VectorCopy (ent->maxs, body->maxs);

	VectorCopy (ent->absmin, body->absmin);
	VectorCopy (ent->absmax, body->absmax);
	VectorCopy (ent->size, body->size);
	VectorCopy (ent->s.origin, body->s.origin);
	VectorCopy(ent->s.angles, body->s.angles);
	body->s.angles[PITCH]=0;
	body->s.renderfx=ent->s.renderfx;
	body->s.number = body - g_edicts;

//	body->solid = ent->solid;
//	body->clipmask = ent->clipmask;
//	body->movetype = ent->movetype;
	body->solid = SOLID_BBOX;
	body->clipmask = CONTENTS_DEADMONSTER;
	body->movetype = MOVETYPE_STEP;

//	body->think = PB_FadeCorpse;
	body->think = DoNothing;
	body->nextthink=level.time+0.1F;

	body->takedamage = DAMAGE_YES;

	body->ai = ai_c::Create(AI_PLAYERCORPSE, body, "", "");

	gi.linkentity (body);

	BodyPostUse(ent);
}
*/
//non-ai-usin corpses--this'll be slowly converted to use ai, hopefully
void PB_MakeCorpse (edict_t *ent)
{
	edict_t		*body;

	// grab a body que and cycle to the next one
	body = &g_edicts[(int)maxclients->value + level.body_que + 1];
	level.body_que = (level.body_que + 1) % BODY_QUEUE_SIZE;

	// FIXME: send an effect on the removed body

	BodyPreUse(ent);

	gi.unlinkentity (ent);

	// kef -- just before we get rid of this joker's body, free any
	//knives that are stuck in him.
	if (ent && ent->client && ent->client->body)
	{
		ent->client->body->FreeKnives(*ent);
	}

	// if this edict is a complex model of some sort we need to get rid of its
	//bolton-related info
	//eh, copied from freeedict, just in case -- sfs
	FreeEdictBoltData(body);

	game_ghoul.RemoveObjectInstances(body);

	if (body->ai)
	{
		body->ai.Destroy();
	}

	gi.unlinkentity (body);
	VectorCopy(ent->s.angles, body->s.angles);
	body->s.angles[PITCH]=0;
	VectorCopy(ent->s.origin, body->s.origin);
	body->s.renderfx=ent->s.renderfx;
	body->s.number = body - g_edicts;

	//fast ghoul lighting
//	body->s.renderfx |= RF_LIGHT_FASTEST;	

	body->svflags = ent->svflags;
	VectorCopy (ent->mins, body->mins);
	VectorCopy (ent->maxs, body->maxs);
	VectorCopy (ent->absmin, body->absmin);
	VectorCopy (ent->absmax, body->absmax);
	VectorCopy (ent->size, body->size);
	body->solid = ent->solid;
	body->clipmask = ent->clipmask;
	body->owner = ent->owner;
	body->movetype = ent->movetype;
	body->think = PB_FadeCorpse;
	body->nextthink=level.time+ai_corpselife->value;

	body->takedamage = DAMAGE_YES;

	body->owner = ent;
	body->think = DoNothing;
	body->nextthink=level.time+0.1F;
	body->ai = ai_c::Create(AI_PLAYERCORPSE, body, "", "");


	// If the player was burning, burn the new body too...
	if (ent->burntime > level.time + 1.2)
	{
		if(body->ghoulInst)
		{
			fxRunner.exec("environ/onfireburst", body, 0);
		}

		body->burntime = level.time + 4.0;
		body->burninflictor = ent->burninflictor;
	}

	// If the player was phosburning, continue the phosburn.
	if (ent->phosburntime > level.time + 0.5)
	{
		if(body->ghoulInst)
		{
			fxRunner.exec("weapons/world/phosburn", body, 0);
		}

		body->burntime = level.time + 2.0;
		body->burninflictor = ent->burninflictor;
	}

	gi.linkentity (body);

	BodyPostUse(ent);
}


void PB_GhoulUpdate(edict_t &ent)
{
	Matrix4 m;

//normally goes by origin, which doesn't (i don't think) change, so we're all good
	if (ent.ghoulInst)
	{
		ent.ghoulInst->ServerUpdate(level.time);
	}

	//this one's only for the third-person camera hack thingy--view goes up & down for crouching
#ifdef __PLAYERTEST_3DPERSON
	if (ent.client && ent.client->ps.bod && !ent.ghoulInst)
	{
		ent.ghoulInst=ent.client->ps.bod;
		ent.ghoulInst->ServerUpdate(level.time);
		ent.ghoulInst->GetXForm(m);
		if (ent.client&&ent.client->ps.pmove.pm_flags & PMF_DUCKED)
		{
			if (m[3][2]<-26.0)
			{
				m[3][2]=-24.0f;
				ent.ghoulInst->SetXForm(m);
			}
		}
		else
		{
			if (m[3][2]>-26.0)
			{
				m[3][2]=-32.0f;
				ent.ghoulInst->SetXForm(m);
			}
		}
		ent.ghoulInst=NULL;
	}
#endif
}

void PB_KillBody(edict_t &ent)
{
	if(ent.client->body)
	{
		delete ent.client->body;
	}

#ifdef __PLAYERTEST_3DPERSON
	if (ent.client && ent.client->ps.bod && !ent.ghoulInst)
	{
		ent.ghoulInst=ent.client->ps.bod;
	}
#endif

	ent.client->ps.bod=NULL;

	game_ghoul.RemoveObjectInstances(&ent);

	ent.client->body=NULL;

	ent.s.renderfx &= ~(RF_GHOUL);
}

void PB_GetActualSkinName(edict_t *ent, char *putSkinHere)
{
	if (!THIRDPERSON_HACK && !dm->isDM())
	{	// only see players in deathmatch or for crazy 3rd person camera hack
		strcpy(putSkinHere, "mullins");
		return;
	}
	if (ent && ent->client && ent->client->body && BodyPreUse(ent))
	{
		if (ent->ghoulInst)
		{
			ggObjC *myGhoulObj=game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());
			if (myGhoulObj)
			{
				char				modname[100];
				int					i;
				
				Com_sprintf(modname, 100, "%s", myGhoulObj->GetSkinName());
				for (i=0;modname[i]!=0 && modname[i]!='.' && i < 100;i++);
				modname[i]=0;

				strcpy(putSkinHere, modname);
				return;
			}
		}
		BodyPostUse(ent);
	}

	strcpy(putSkinHere, "mullins");
}

void PB_GetActualTeamName(edict_t *ent, char *putTeamHere)
{
	if (!THIRDPERSON_HACK && !dm->isDM())
	{	// only see players in deathmatch or for crazy 3rd person camera hack
		strcpy(putTeamHere, "mullins");
		return;
	}
	if (ent && ent->client && ent->client->body && BodyPreUse(ent))
	{
		if (ent->ghoulInst)
		{
			ggObjC *myGhoulObj=game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());
			if (myGhoulObj)
			{
				char	modname[100];
				IPlayerModelInfoC	*myModel;
				
				Com_sprintf(modname, 100, "%s", myGhoulObj->GetSkinName());

				myModel=gi.NewPlayerModelInfo(modname);

				if (myModel && myModel->IsLoaded())
				{
					TeamInfoC tteam;
					myModel->GetTeamInfo(tteam);

					//a real team: return it
					if (stricmp(tteam.name, "Noteam"))
					{
						strcpy(putTeamHere, tteam.name);
					}
					//not a real team: return the skin
					else
					{
						int					i;
						
						Com_sprintf(modname, 100, "%s", myGhoulObj->GetSkinName());
						for (i=0;modname[i]!=0 && modname[i]!='.' && i < 100;i++);
						modname[i]=0;

						strcpy(putTeamHere, modname);
					}

					delete myModel;
					return;
				}
				delete myModel;
				myModel=NULL;
			}
		}
		BodyPostUse(ent);
	}

	strcpy(putTeamHere, "mullins");
}

bool PB_InitBody(edict_t &ent,char *userinfo)
{
	int i;
	char meatSound[100];
	bool	usingDesiredPMod=true;

	bool	b_checkOldStrings=false;

	//fixme: i don't think these are called in dm--the caching should reflect that.
	{
		for (i=1;i<=3;i++)
		{
			Com_sprintf(meatSound, sizeof(meatSound), "impact/gore/impact%d.wav", i);
			gi.soundindex(meatSound);
		}
		for (i=1;i<=4;i++)
		{
			Com_sprintf(meatSound, sizeof(meatSound), "impact/player/hit%d.wav", i);
			gi.soundindex(meatSound);
		}
		gi.soundindex("impact/player/armor.wav");
		gi.soundindex("impact/gore/sizzle.wav");
	}

	if (THIRDPERSON_HACK || dm->isDM())
	{
		if (ent.client && ent.client->body)
		{
			b_checkOldStrings=true;
		}
	}

	PB_KillBody(ent);

	if (!THIRDPERSON_HACK && !dm->isDM())
	{	// only see players in deathmatch or for crazy 3rd person camera hack
		return false;
	}

	ggObjC	*MyGhoulObj;
	char	modname[100];

	ggOinstC* myInstance=0;

	Matrix4	mat,mat1,mat2;

	SkinInfoC tempSkin;
	BoltOnInfoC tempBoltOn;
	IPlayerModelInfoC	*boltModel;
	
	IPlayerModelInfoC	*myModel;
	bool hasArmor=false;
	
	if (ent.client && ent.client->inv)
	{
		hasArmor=(ent.client->inv->getArmorCount()>0);
	}

	Com_sprintf(modname, 100, "%s.gpm", Info_ValueForKey (userinfo, "skin"));

	if ((!modname || !(myModel=gi.NewPlayerModelInfo(modname)) || !myModel->IsLoaded())&&defModName[0])
	{
		//couldn't find the model file, or it's bad: load up the default guy
		if (!defModName[0])
		{
			gi.error("No default player model.\n");
		}
		strcpy(modname, defModName);
		myModel->Init(defModName);
		usingDesiredPMod=false;
	}

	//this is where the model info should be really gotten
	if (myModel)
	{
		int counter;

		//register stuff, and set up instance here.


		//game-stuff: allocate appropriate body type
		if (!stricmp(myModel->GetGameGHB(),"fem_play"))
		{
			ent.client->body=new bodyfemale_c();
		}
		else
		{
			ent.client->body=new bodymeso_c();
		}

		//this creates (or finds, if it's there already) the ghoul object, registers the sequences we'll need
		// and remembers which _poff file we'll need (to turn parts off when we create an inst)

		MyGhoulObj=game_ghoul.FindObject(myModel->GetGHBDir(),myModel->GetSeqGSQ(),false,modname,myModel->GetGameGHB(),true);

		//if finding the object failed, we're screwed.
		if (!MyGhoulObj&&usingDesiredPMod)
		{
			usingDesiredPMod=false;

			//try falling back to mullins
			if (!defModName[0])
			{
				gi.error("No default player model.\n");
			}

			strcpy(modname, defModName);
			myModel->Init(defModName);
			if (!myModel)
			{
				gi.error("Couldn't find default player model info.\n");
			}

			delete ent.client->body;
			ent.client->body=new bodymeso_c();

			//this creates (or finds, if it's there already) the ghoul object, registers the sequences we'll need
			// and remembers which _poff file we'll need (to turn parts off when we create an inst)

			MyGhoulObj=game_ghoul.FindObject(myModel->GetGHBDir(),myModel->GetSeqGSQ(),false,modname,myModel->GetGameGHB(),true);
			if (!MyGhoulObj)
			{
				gi.error("Couldn't set up default player model.\n");
			}
		}
		
		//game-stuff:  make sure the body we created is connected to ent
		if(ent.client->body)
		{
			ent.client->body->SetOwner(&ent);
		}

		//took the registration out of here--it's done in level precaching.

		//make the ghoulInst
		myInstance = game_ghoul.AddObjectInstance(MyGhoulObj, &ent);

		ent.s.renderfx = RF_GHOUL;

		//game-stuff:  we bolt players to the quake_ground, so we don't have to fiddle with their bboxes
		//actually, this might be a good idea for menus too
		ent.ghoulInst->SetMyBolt("quake_ground");

		//i think x and y are switched between max and sof; anyway, i need to rotate guys like this, or they'll face to the side
		ent.ghoulInst->GetXForm(mat);
		mat1.Identity();
		mat2=mat;
		mat1.Rotate(2,-M_PI*0.5);
		mat.Concat(mat1, mat2);

		//adjust for quake_ground--it's at the guy's feet, so we gotta pop im back up
		mat[3][2]=-24.0f;

		ent.ghoulInst->SetXForm(mat);

		//game-stuff: if the body-creation failed, we're screwed
		if (!ent.client->body)
		{
			delete myModel;
			return false;
		}


		//toggle extra parts from file
		ExtraPartToggleInfoC tempPartToggle;
		for (counter = myModel->GetNumPartToggles(); counter>0; counter--)
		{
			if (myModel->GetPartToggle(counter, tempPartToggle))
			{
				ent.ghoulInst->SetPartOnOff(tempPartToggle.partName, tempPartToggle.partState);
			}
			else
			{
				//ERROR!
			}
		}

		//set skins from file
		for (counter = myModel->GetNumSkins(); counter>0; counter--)
		{
			if (myModel->GetSkin(counter, tempSkin))
			{
				if (tempSkin.partName[0])
				{
					//set skin for specific part--doesn't go through body, but does the same thing the body would do anyway.
					ent.ghoulInst->SetFrameOverride(tempSkin.matName, tempSkin.skinName, tempSkin.partName);
				}
				else
				{
					ent.client->body->ApplySkin(ent,  myInstance, tempSkin.matName, tempSkin.skinName);
				}
			}
			else
			{
				//ERROR!
			}
		}

		//now set armor (this should prolly be moved elsewhere
		if (hasArmor)
		{
			//female stuff
			ent.ghoulInst->SetPartOnOff("_tightchest", 0);
			//meso stuff
			ent.ghoulInst->SetPartOnOff("_bosschest", 0);
			ent.ghoulInst->SetPartOnOff("_maskchest", 0);
			ent.ghoulInst->SetPartOnOff("_bulkychest", 0);
			ent.ghoulInst->SetPartOnOff("_chest_b_t", 0);
			ent.ghoulInst->SetPartOnOff("_armorchest", 1);

			if (ent.ghoulInst->GetPartOnOff("_bosships"))
			{
				ent.ghoulInst->SetPartOnOff("_CAP_CHEST_HIPS_BULKY", 1);
				ent.ghoulInst->SetPartOnOff("_CAP_HIPS_CHEST_BOSS", 1);
			}
			if (ent.ghoulInst->GetPartOnOff("_tighthips"))
			{
				ent.ghoulInst->SetPartOnOff("_CAP_CHEST_HIPS_BULKY", 1);
			}
			//stuff fer everybody
			ent.ghoulInst->SetPartOnOff("_armorchest", 1);
			ent.ghoulInst->SetFrameOverride("b", "b_dm_armor", "_armorchest");
		}

		//game-stuff:  remember which set of faces to use
		ent.client->body->SetFace(ent,myModel->GetFaceSet());


		//bolt on stuff from file
		for (counter = myModel->GetNumBoltOns(); counter>0; counter--)
		{
			if (myModel->GetBoltOn(counter, tempBoltOn)&&(boltModel=gi.NewPlayerModelInfo(tempBoltOn.childModelInfo)))
			{
				if (boltModel->IsLoaded()&&boltModel->GetGameGHB()[0])
				{
					boltModel->GetSkin(1, tempSkin);
					ent.client->body->AddBoltedItem(ent, tempBoltOn.parentBolt, boltModel->GetGHBDir(), boltModel->GetGameGHB(), tempBoltOn.childBolt, myInstance, tempSkin.skinName, tempBoltOn.scale);
				}
				else
				{
					//couldn't load bolt-on info file!
				}
				delete boltModel;
				boltModel=NULL;
			}
			else
			{
				//ERROR!
			}
		}

		//game-stuff:  add voice stuff
		VoiceInfoC tempVoiceInfo;
		myModel->GetVoiceInfo(tempVoiceInfo);
		ent.client->body->SetVoiceDirectories(ent, "", 1, DEATHVOICE_PLAYER, tempVoiceInfo.voiceDir);

		//make sure the specified teamname is right, too
		TeamInfoC tteam;
		myModel->GetTeamInfo(tteam);
		usingDesiredPMod = usingDesiredPMod && !stricmp(tteam.name,Info_ValueForKey (userinfo, "teamname"));
	}

	delete myModel;

	//game-stuff:  this sets up a lot of stuff for the body
	ent.client->body->SetRootBolt(ent);

	//clients seem to be immune to their own ghoulinsts, so i made it worse than it is. so there. --ss
	BodyPostUse(&ent);
	ent.client->ps.bod=myInstance->GetInstPtr();
	myInstance->GetInstPtr()->SetUserData(&ent);

	if (b_checkOldStrings)
	{
		//not really desired model, but everyone should know about it already
		if (!stricmp(ent.client->oldSkinRequest, Info_ValueForKey (userinfo, "skin"))
			&&!stricmp(ent.client->oldTeamnameRequest, Info_ValueForKey (userinfo, "teamname"))
			&&!strcmp(ent.client->oldNetName, ent.client->pers.netname))
		{
			usingDesiredPMod=true;
		}
		//this may be what we want, but make sure everybody knows, because info has changed
		else
		{
			usingDesiredPMod=false;
		}
	}
	//didn't have a body at all before--make sure new info gets sent around
	else
	{
		usingDesiredPMod=false;
	}

	if (ent.health <= 0)
	{
		ent.client->body->SetAnimation(ent, &generic_move_death_lbshotdeath);
	}
	else
	// now add a ctf flag to the player if we need to
	if (deathmatch->value == DM_CTF)
	{
   		if (ent.ctf_flags == TEAM1)
   		{
	   		PB_AddFlag(&ent, "ctf_flag_blue", "flag_hold_idle");
   		}
   		else
   		if (ent.ctf_flags == TEAM2)
   		{
	   		PB_AddFlag(&ent, "ctf_flag_red", "flag_hold_idle");
   		}
	}

	Com_sprintf(ent.client->oldNetName, sizeof(ent.client->oldNetName), "%s", ent.client->pers.netname);
	Com_sprintf(ent.client->oldSkinRequest, sizeof(ent.client->oldSkinRequest), "%s", Info_ValueForKey (userinfo, "skin"));
	Com_sprintf(ent.client->oldTeamnameRequest, sizeof(ent.client->oldTeamnameRequest), "%s", Info_ValueForKey (userinfo, "teamname"));
	return usingDesiredPMod;
}



void PB_PlaySequenceForGesture(edict_t *ent, int gestNum)
{
	if (!THIRDPERSON_HACK && !dm->isDM())
	{	// only see players in deathmatch or for crazy 3rd person camera hack
		return;
	}

	bbox_preset goal_bbox=GetGoalBBox(ent);

	if (ent && ent->client && ent->client->body && BodyPreUse(ent))
	{
		mmove_t *painanim=NULL;
		mmove_t *idealanim=NULL;
		switch(gestNum)
		{
		default:
			gi.cprintf (ent, PRINT_HIGH, "Unrecognized wave value: %d, trying to play 'go'.\n", gestNum);
		case 1:
			idealanim=&generic_move_playersignal_go;
			break;
		case 2:
			idealanim=&generic_move_playersignal_stop;
			break;
		case 3:
			idealanim=&generic_move_playersignal_taunt;
			break;
		case 4:
			if (ent->client->body->IsAvailableSequence(*ent, &generic_move_touchnuts))
			{
				idealanim=&generic_move_touchnuts;
			}
			else
			{
				idealanim=&generic_move_pentium_dance;
			}
			break;
		}
		assert(idealanim);

		if (!painanim)
		{
			painanim=ent->client->body->GetSequenceForStand(*ent, vec3_origin, vec3_origin, ACTSUB_NORMAL, goal_bbox, idealanim);
		}
		
		if (painanim)
		{
			SetAnimSpeed(ent, ent->s.origin, painanim);
			ent->client->body->SetAnimation(*ent, painanim);
		}
		BodyPostUse(ent);
	}
}

static int curPlayerDeath;

void PB_PlaySequenceForDeath(edict_t *ent, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags)
{
	if (!THIRDPERSON_HACK && !dm->isDM())
	{	// only see players in deathmatch or for crazy 3rd person camera hack
		return;
	}

	bbox_preset goal_bbox=GetGoalBBox(ent);

	if (ent && ent->client && ent->client->body && BodyPreUse(ent))
	{
		mmove_t *painanim;

//		while (1)
//		{
//			curPlayerDeath++;
//			if (curPlayerDeath > MMOVE_HUMAN_SIZE-1 || curPlayerDeath < 0)
//			{
//				curPlayerDeath = 0;
//			}
//			if (MMoves[curPlayerDeath].suggested_action == ACTCODE_DEATH && ent->client->body->IsAvailableSequence(*ent, &MMoves[curPlayerDeath]))
//				break;
//		}

//		if (painanim=ent->client->body->GetSequenceForDeath(*ent, inflictor, attacker, damage, point, dflags, goal_bbox, &MMoves[curPlayerDeath]))
		if (painanim=ent->client->body->GetSequenceForDeath(*ent, inflictor, attacker, damage, point, dflags, goal_bbox))
		{
			SetAnimSpeed(ent, ent->s.origin, painanim);
			if (painanim && painanim->suggested_action != ACTCODE_DEATH)
			{
				gi.dprintf("Death action %s not really a death!\n", painanim->ghoulSeqName);
			}
/*
			if (painanim != &MMoves[curPlayerDeath])
			{
				gi.dprintf("Death action %s doesn't work--using %s!\n", MMoves[curPlayerDeath].ghoulSeqName, painanim->ghoulSeqName);
			}
			else
			{
				gi.dprintf("All clear for death action %s!\n", painanim->ghoulSeqName);
			}
*/
			ent->client->body->SetAnimation(*ent, painanim);
		}
		BodyPostUse(ent);
	}
}

void PB_PlaySequenceForPain(edict_t *ent, vec3_t point, vec3_t dest, vec3_t face)
{
	if (!THIRDPERSON_HACK && !dm->isDM())
	{	// only see players in deathmatch or for crazy 3rd person camera hack
		return;
	}

	bbox_preset goal_bbox=GetGoalBBox(ent);

	//eek! first pass at player body!
	if (ent && ent->client && ent->client->body && ent->health>0 && BodyPreUse(ent))
	{
		mmove_t *painanim;
		if (painanim=ent->client->body->GetSequenceForPain(*ent, point, 1.0f /*kick*/, 1 /*damage*/, dest, face, ACTSUB_NORMAL, goal_bbox))
		{
			SetAnimSpeed(ent, dest, painanim);
			ent->client->body->ForceAnimation(*ent, painanim);
		}
		BodyPostUse(ent);
	}
}

void PB_PlaySequenceForMovement(edict_t *ent, vec3_t dest, vec3_t face, vec3_t org, vec3_t ang)
{
	if (!THIRDPERSON_HACK && !dm->isDM())
	{	// only see players in deathmatch or for crazy 3rd person camera hack
		return;
	}

//	gi.dprintf("looking for move...\n");
	bbox_preset goal_bbox=GetGoalBBox(ent);

	AdjustDestForSpeed(ent, dest);

	if (ent && ent->client && ent->client->body && 
		(!ent->client->body->GetMove() || !(!ent->client->body->IsAnimationFinished()&&ent->client->body->GetMove()->actionFlags&ACTFLAG_FULLANIM)) && 
		ent->health>0 && BodyPreUse(ent))
	{
		mmove_t *painanim;
		if (!(painanim=GetLeaningSequence(ent, dest, face, goal_bbox)))
		{
			painanim=ent->client->body->GetSequenceForMovement(*ent, dest, face, org, ang, ACTSUB_NORMAL, goal_bbox);
		}
		if (painanim)
		{
			SetAnimSpeed(ent, dest, painanim);
			if (painanim==ent->client->body->GetMove()&&ent->client->body->IsAnimationFinished()&&!(ent->client->body->GetMove()->actionFlags&ACTFLAG_LOOPANIM))
			{
				ent->client->body->ForceAnimation(*ent, painanim);
			}
			else
			{
//				gi.dprintf("setting anim %s; old anim %s\n",painanim->ghoulSeqName,ent->client->body->GetMove()->ghoulSeqName);
				ent->client->body->SetAnimation(*ent, painanim);
			}
		}
		BodyPostUse(ent);
	}
}

void PB_PlaySequenceForStand(edict_t *ent, vec3_t dest, vec3_t face)
{
	if (!THIRDPERSON_HACK && !dm->isDM())
	{	// only see players in deathmatch or for crazy 3rd person camera hack
		return;
	}

//	gi.dprintf("looking for stand...\n");
	bbox_preset goal_bbox=GetGoalBBox(ent);

	if (ent && ent->client && ent->client->body && 
		(!ent->client->body->GetMove() || !(!ent->client->body->IsAnimationFinished()&&ent->client->body->GetMove()->actionFlags&ACTFLAG_FULLANIM)) && 
		ent->health>0 && BodyPreUse(ent))
	{
		mmove_t *painanim;

		if (!(painanim=GetLeaningSequence(ent, dest, face, goal_bbox)))
		{
			painanim=ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox);
		}

		if (painanim)
		{
//			gi.dprintf("gonna play %s!\n", painanim->ghoulSeqName);

			//oh shit! i'm floppin around! stand up now!
			if (painanim==&generic_move_death_lbshotdeath && ent->health > 0)
			{
//				gi.dprintf("hup!\n");
				painanim=&generic_move_alert_p;
			}

			SetAnimSpeed(ent, dest, painanim);
			if (painanim==ent->client->body->GetMove()&&ent->client->body->IsAnimationFinished())
			{
				ent->client->body->ForceAnimation(*ent, painanim);
			}
			else
			{
//				gi.dprintf("setting anim %s; old anim %s\n",painanim->ghoulSeqName,ent->client->body->GetMove()->ghoulSeqName);
				ent->client->body->SetAnimation(*ent, painanim);
			}
		}
		BodyPostUse(ent);
	}
}

void PB_PlaySequenceForReload(edict_t *ent, vec3_t dest, vec3_t face)
{
	if (!THIRDPERSON_HACK && !dm->isDM())
	{	// only see players in deathmatch or for crazy 3rd person camera hack
		return;
	}

//	gi.dprintf("looking for reload...\n");
	bbox_preset goal_bbox=GetGoalBBox(ent);

	AdjustDestForSpeed(ent, dest);

	if (ent && ent->client && ent->client->body && 
		ent->health>0 && /*goal_bbox == BBOX_PRESET_STAND &&*/ BodyPreUse(ent))
	{
		mmove_t *painanim;
		if (painanim=ent->client->body->GetSequenceForReload(*ent, dest, face, ACTSUB_NORMAL, goal_bbox))
		{
			SetAnimSpeed(ent, dest, painanim);
			ent->client->body->SetAnimation(*ent, painanim);
		}
		BodyPostUse(ent);
	}
}

void PB_PlaySequenceForItemUse(edict_t *ent, vec3_t dest, vec3_t face, int itemType)
{
	if (!THIRDPERSON_HACK && !dm->isDM())
	{	// only see players in deathmatch or for crazy 3rd person camera hack
		return;
	}

//	gi.dprintf("looking for reload...\n");
	bbox_preset goal_bbox=GetGoalBBox(ent);

	AdjustDestForSpeed(ent, dest);

	if (ent && ent->client && ent->client->body && 
		ent->health>0 && goal_bbox == BBOX_PRESET_STAND && BodyPreUse(ent))
	{
		mmove_t *painanim;
		mmove_t	*desiredanim=NULL;
		switch (itemType)
		{
		case SFE_FLASHPACK:
			desiredanim=&generic_move_throwitem_player;
			break;
//		case SFE_NEURAL_GRENADE:
//			desiredanim=&generic_move_throwitem_player;
			break;
		case SFE_C4:
			desiredanim=&generic_move_throwitem_player;
			break;
		case SFE_CLAYMORE:
			desiredanim=&generic_move_throwitem_player;
			break;
		case SFE_GRENADE:
			desiredanim=&generic_move_throwitem_player;
			break;
		case SFE_MEDKIT:
		case SFE_LIGHT_GOGGLES:
		case SFE_EMPTYSLOT:
		default:
			break;
		}
		if (desiredanim)
		{
			if (painanim=ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, desiredanim))
			{
				SetAnimSpeed(ent, dest, painanim);
				ent->client->body->ForceAnimation(*ent, painanim);
			}
		}
		BodyPostUse(ent);
	}
}

void PB_PlaySequenceForJump(edict_t *ent, vec3_t dest, vec3_t face)
{
	if (!THIRDPERSON_HACK && !dm->isDM())
	{	// only see players in deathmatch or for crazy 3rd person camera hack
		return;
	}

//	gi.dprintf("looking for jump...\n");
	bbox_preset goal_bbox=GetGoalBBox(ent);


	AdjustDestForSpeed(ent, dest);

	//eek! first pass at player body!
	if (ent && ent->client && ent->client->body && ent->health>0 &&
				(!ent->client->body->GetMove() || !(!ent->client->body->IsAnimationFinished()&&ent->client->body->GetMove()->suggested_action==ACTCODE_PAIN))
				&&BodyPreUse(ent))
	{
		mmove_t *painanim;
		vec3_t	spot;
		vec3_t	forward;
		vec3_t	flatforward;
		trace_t	trace;
		bool ladder = false;

		// check for ladder--nice bag of shit, yes?
		AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);
		flatforward[0] = forward[0];
		flatforward[1] = forward[1];
		flatforward[2] = 0;
		VectorNormalize (flatforward);

		VectorMA (ent->s.origin, 1, flatforward, spot);
		gi.trace(ent->s.origin, ent->mins, ent->maxs, spot, ent, MASK_PLAYERSOLID, &trace);
		if ((trace.fraction < 1) && (trace.contents & CONTENTS_LADDER))
		{
			ladder = true;
		}
		//if it's not a ladder: gimme a little safety buffer for falling down before i start in on the anim...
		else if (dest[2]-ent->s.origin[2]<1 && dest[2]-ent->s.origin[2]>-5)
		{
			BodyPostUse(ent);
			return;
		}

		if (ladder)
		{
			if (ent->velocity[2] > 2.0)
			{
				painanim=ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_ladder_up);
				SetAnimSpeed(ent, dest, painanim);

				if (painanim==ent->client->body->GetMove()&&ent->client->body->IsAnimationFinished()&&!(ent->client->body->GetMove()->actionFlags&ACTFLAG_LOOPANIM))
				{
					ent->client->body->ForceAnimation(*ent, painanim);
				}
				else
				{
					ent->client->body->SetAnimation(*ent, painanim);
				}
			}
			else if (ent->velocity[2] < -2.0)
			{
				painanim=ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_ladder_down);
				SetAnimSpeed(ent, dest, painanim);

				if (painanim==ent->client->body->GetMove()&&ent->client->body->IsAnimationFinished()&&!(ent->client->body->GetMove()->actionFlags&ACTFLAG_LOOPANIM))
				{
					ent->client->body->ForceAnimation(*ent, painanim);
				}
				else
				{
					ent->client->body->SetAnimation(*ent, painanim);
				}
			}
			else
			{
				painanim=ent->client->body->GetSequenceForStand(*ent, dest, face, ACTSUB_NORMAL, goal_bbox);
				SetAnimSpeed(ent, dest, painanim);

				if (painanim)
				{
					if (painanim==ent->client->body->GetMove()&&ent->client->body->IsAnimationFinished())
					{
						ent->client->body->ForceAnimation(*ent, painanim);
					}
					else
					{
						ent->client->body->SetAnimation(*ent, painanim);
					}
				}
			}
		}
		else
		{
			vec3_t flatvel;
			VectorCopy(ent->velocity, flatvel);
			flatvel[2]=0;
			if (VectorNormalize(flatvel)>250 && DotProduct(flatvel, flatforward)>0.25)
			{
				painanim=ent->client->body->GetSequenceForJump(*ent, dest, face, ACTSUB_NORMAL, goal_bbox, &generic_move_jump_runfwd);
			}
			else
			{
				painanim=ent->client->body->GetSequenceForJump(*ent, dest, face, ACTSUB_NORMAL, goal_bbox);
			}

			if (painanim)
			{
				SetAnimSpeed(ent, dest, painanim);
				ent->client->body->SetAnimation(*ent, painanim);
			}
		}
		BodyPostUse(ent);
	}
}

void PB_PlaySequenceForAttack(edict_t *ent, vec3_t dest, vec3_t face, edict_t *target)
{
	if (!THIRDPERSON_HACK && !dm->isDM())
	{	// only see players in deathmatch or for crazy 3rd person camera hack
		return;
	}

//	gi.dprintf("looking for attack...\n");
	bbox_preset goal_bbox=GetGoalBBox(ent);

	AdjustDestForSpeed(ent, dest);

	//eek! first pass at player body!
	if (ent && ent->client && ent->client->body && ent->health>0 && 
		(!ent->client->body->GetMove() || !(!ent->client->body->IsAnimationFinished()&&ent->client->body->GetMove()->suggested_action==ACTCODE_PAIN))
		&& BodyPreUse(ent))
	{
		mmove_t *painanim;
		if (!(painanim=GetLeaningSequence(ent, dest, face, goal_bbox)))
		{
			painanim=ent->client->body->GetSequenceForAttack(*ent, dest, face, target, ACTSUB_NORMAL, goal_bbox);
		}
		if (painanim)
		{
			SetAnimSpeed(ent, dest, painanim);
			if (painanim->actionFlags&ACTFLAG_MATCHANIM)
			{
				ent->client->body->MatchAnimation(*ent, painanim);
			}
			else if (painanim->suggested_action==ACTCODE_ATTACK)
			{
				ent->client->body->ForceAnimation(*ent, painanim);
			}
			else
			{
				ent->client->body->SetAnimation(*ent, painanim);
			}
		}
		BodyPostUse(ent);
	}
}

void UpdatePlayerFace(edict_t *ent)
{
	if (ent && ent->client && ent->client->body)
	{
		BodyPreUse(ent);
//		gi.dprintf("player height: %f\n",ent->maxs[2]-ent->mins[2]);
		ent->client->body->UpdateFace(*ent);
		BodyPostUse(ent);
	}
}

void UpdatePlayerWeapon(edict_t *ent)
{
	if (!THIRDPERSON_HACK && !dm->isDM())
	{	// only see players in deathmatch or for crazy 3rd person camera hack
		return;
	}

	if (ent && ent->client && ent->client->body && ent->health > 0)
	{
		attacks_e newWeap = ATK_NOTHING;
		switch(ent->client->inv->getCurWeaponType())
		{
		case SFW_KNIFE:
			newWeap=ATK_KNIFE;
			break;
		case SFW_PISTOL2:
			newWeap=ATK_PISTOL2;
			break;
		case SFW_PISTOL1:
			newWeap=ATK_PISTOL1;
			break;
		case SFW_MACHINEPISTOL:
			newWeap=ATK_MACHINEPISTOL;
			break;
		case SFW_SNIPER:
			newWeap=ATK_SNIPER;
			break;
		case SFW_SHOTGUN:
			newWeap=ATK_SHOTGUN;
			break;
		case SFW_MACHINEGUN:
			newWeap=ATK_MACHINEGUN;
			break;
		case SFW_ASSAULTRIFLE:
			newWeap=ATK_ASSAULTRIFLE;
			break;
		case SFW_AUTOSHOTGUN:
			newWeap=ATK_AUTOSHOTGUN;
			break;
		case SFW_ROCKET:
			newWeap=ATK_ROCKET;
			break;
		case SFW_EMPTYSLOT:
			newWeap=ATK_NOTHING;
			break;
		case SFW_MICROWAVEPULSE:
			newWeap=ATK_MICROWAVE;
			break;
		case SFW_FLAMEGUN:
			newWeap=ATK_FLAMEGUN;
			break;
		default:
			break;
		}
		BodyPreUse(ent);
		if (newWeap != ent->client->body->GetRightHandWeapon(*ent))
		{
			ent->client->body->SetRightHandWeapon(*ent, newWeap);
		}
		BodyPostUse(ent);

		 // if we are carrying a CTF flag, decide whether it should be waving or not
		if (ent->ctf_flags)
		{
			IGhoulObj *game_obj = ent->ghoulInst->GetGhoulObject();
			ggObjC *client_obj = game_ghoul.FindObject(game_obj);
			ggOinstC *client_inst = client_obj->FindOInst(ent->ghoulInst);
			char	*flagname;
			vec3_t	vel, forward;

			// we are moving forward - set flag to flutter if it isn't already
			// this is un-necessarily complicated, but it should work.
			VectorCopy (ent->velocity, vel);
			VectorNormalize (vel);
			
			AngleVectors(ent->s.angles, forward, NULL, NULL);
			if (DotProduct(vel, forward) > 0.5)
			{
				GhoulID hip = game_obj->FindPart("abolt_hip_r");
				ggBinstC *bolt = client_inst->GetBoltInstance(hip);
				if (bolt)
				{
					ggObjC *bolton = bolt->GetBolteeObject();
					if (stricmp("flag_run", bolton->GetSubName()))
					{
						// remove exiting flag
						client_inst->RemoveBoltInstance(bolt);
						// now add a flag to the player
						if (ent->ctf_flags == 1)
						{
							flagname = "ctf_flag_blue";
						}
						else
						{
							flagname = "ctf_flag_red";
						}
						PB_AddFlag(ent, flagname, "flag_run");
					}

				}
			}
			// not moving forward - set droop
			else
			{
				GhoulID hip = game_obj->FindPart("abolt_hip_r");
				ggBinstC *bolt = client_inst->GetBoltInstance(hip);
				if (bolt)
				{
					ggObjC *bolton = bolt->GetBolteeObject();
					if (stricmp("flag_hold_idle", bolton->GetSubName()))
					{
						// remove exiting flag
						client_inst->RemoveBoltInstance(bolt);
						// now add a flag to the player
						if (ent->ctf_flags == 1)
						{
							flagname = "ctf_flag_blue";
						}
						else
						{
							flagname = "ctf_flag_red";
						}
						PB_AddFlag(ent, flagname, "flag_hold_idle");
					}

				}
			}
		}
	}
}

void PlayPlayerHurtSound(edict_t *targ, int damage, int dflags, int mod)
{
	char meatSound[100];

	if (targ && targ->health<=0)
	{
		return;
	}

	if (targ->client->body)
	{
		return;
	}

	if (mod == MOD_FIRE)
	{
		gi.sound (targ, CHAN_BODY, gi.soundindex("impact/gore/sizzle.wav"), 1.0, ATTN_NORM, 0);
		return;
	}

	if (damage > 0)
	{
		Com_sprintf(meatSound, sizeof(meatSound), "impact/gore/impact%d.wav", gi.irand(1,3));
		gi.sound (targ, CHAN_BODY, gi.soundindex(meatSound), 1.0, ATTN_NORM, 0);
		Com_sprintf(meatSound, sizeof(meatSound), "impact/player/hit%d.wav", gi.irand(1,4));
		gi.sound (targ, CHAN_VOICE, gi.soundindex(meatSound), 1.0, ATTN_NORM, 0);
	}
	else
	{
		gi.sound (targ, CHAN_BODY, gi.soundindex("impact/player/armor.wav"), 1.0, ATTN_NORM, 0);
	}
}

void PB_Killed(edict_t *ent)
{
	if (!ent || !ent->client)
	{
		return;
	}

	if (!THIRDPERSON_HACK && !dm->isDM())
	{
		return;
	}
	
	if (BodyPreUse(ent))
	{
		ent->client->inv->addArmor(ent->client->inv->getArmorCount()*-1);

		if(ent->ghoulInst)
		{
			ggOinstC	*MyGhoulInst=game_ghoul.FindOInst(ent->ghoulInst);
			GhoulID		theHand;


			if (theHand=ent->ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_r"))
			{
				MyGhoulInst->RemoveBolt(theHand);
			}
			if (theHand=ent->ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"))
			{
				MyGhoulInst->RemoveBolt(theHand);
			}
		}
		BodyPostUse(ent);
	}
}

int PB_Damage (edict_t *targ, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb)
{
	if (damage <= 0)
	{
		return damage;
	}

	if (!targ || !targ->client)
	{
		PlayPlayerHurtSound(targ, damage, dflags, mod);

		return damage;
	}

	if (!targ->client->body || !targ->client->ps.bod)
	{
		if(dflags&DAMAGE_NO_ARMOR)
		{
			// Ignore protective effects of armour.
			PlayPlayerHurtSound(targ, damage, dflags, mod);

			return(damage);
		}
		else
		{
			// Adjust damage due to protective effects of armour.
			damage = (targ->client->inv->adjustDamageByArmor(damage, penetrate, absorb));
			PlayPlayerHurtSound(targ, damage, dflags, mod);

			return damage;
		}
	}

	//eh, this shouldn't matter if I don't die here, but it's gotta be set if I do
	if (targ->health > 0)
	{
		targ->client->body->SetInitialKilledTime(level.time);
	}

	targ->client->body->SetLastKilledTime(level.time);


	int take;

	if (THIRDPERSON_HACK || dm->isDM())
	{
		bool hadArmor=false;
		if (targ && targ->client && targ->client->inv)
		{
			hadArmor=(targ->client->inv->getArmorCount()>0);
		}
		BodyPreUse(targ);

		//FIXME: SS iss assuming bullets are fired to get here (think he was gonna fix this).
		//This is not much better but it's in the right direction, kinda. 

		if(bullet_numHits==0 && (dflags & (DT_PROJECTILE|DT_MELEE)))
		{
			Matrix4 ToEnt,ToWorld;
			Vect3 transform;

			EntToWorldMatrix(targ->s.origin,targ->s.angles,ToWorld);
			//but we want to put the ray into ent space, need inverse
			ToEnt.Inverse(ToWorld);
			
			VectorCopy(origin,*((vec3_t*)&transform));
			ToEnt.XFormPoint(bullet_EntStart,transform);
			ToEnt.XFormVect(bullet_EntDir,*(Vect3 *)dir);
			bullet_EntDir.Norm();
			bullet_numHits = targ->ghoulInst->RayTrace(level.time,bullet_EntStart,bullet_EntDir,bullet_Hits,20);
		}

		take = targ->client->body->ShowDamage(*targ, inflictor, attacker, dir, point, origin, damage, knockback, dflags, mod, penetrate, absorb);

		//extra damage for females
		if (take > 0 && targ->client->body->GetClassCode()==BODY_FEMALE)
		{
//			gi.dprintf("More damage, girlie! Start take: %d, ", take);
			take+=(float)take*0.5;
//			gi.dprintf("End take: %d!\n", take);
		}

		if (targ->health>take)
		{
			vec3_t dest, face;
			AngleVectors(targ->client->ps.viewangles, face, NULL, NULL);
			VectorMA(targ->s.origin, 100, face, face);
			VectorCopy(targ->velocity, dest);
			VectorMA(targ->s.origin, 0.025, dest, dest);

			PB_PlaySequenceForPain(targ, point, dest, face);
		}
		else
		{
			PB_PlaySequenceForDeath(targ, inflictor, attacker, damage, point, dflags);
		}

		BodyPostUse(targ);
		PlayPlayerHurtSound(targ, take, dflags, mod);

		bool hasArmor=false;
		if (targ && targ->client && targ->client->inv)
		{
			hasArmor=(targ->client->inv->getArmorCount()>0);
		}
		if (hadArmor!=hasArmor && targ->health>take)
		{
			//swap chests here, but don't bother if i'm dead
			PB_InitBody(*targ,targ->client->pers.userinfo);//c'mon, we can do better than this!
		}

		return take;
	}
	else
	{
		if(dflags&DAMAGE_NO_ARMOR)
		{
			// Ignore protective effects of armour.
			PlayPlayerHurtSound(targ, damage, dflags, mod);

			return(damage);
		}
		else
		{
			// Adjust damage due to protective effects of armour.
			damage = (targ->client->inv->adjustDamageByArmor(damage, penetrate, absorb));
			PlayPlayerHurtSound(targ, damage, dflags, mod);

			return damage;
		}
	}
}

int PB_AddArmor(edict_t *ent, int amount)
{
	if (!ent || !ent->client || !ent->client->inv)
	{
		return 0;
	}

	if (!THIRDPERSON_HACK && !dm->isDM())
	{
		return ent->client->inv->addArmor(amount);
	}

	bool hadArmor=(ent->client->inv->getArmorCount()>0);
	int addArmorReturned = ent->client->inv->addArmor(amount);
	bool hasArmor=(ent->client->inv->getArmorCount()>0);

	if (hadArmor!=hasArmor)
	{
		//swap chests here
		PB_InitBody(*ent,ent->client->pers.userinfo);//c'mon, we can do better than this!
	}

	return addArmorReturned;
}

void PB_RepairSkin(edict_t *ent, int damagedHealth)
{
	bodyhuman_c *body = (bodyhuman_c*)ent->client->body;

	if (body)
	{
		body->HealGoreZones(*ent, damagedHealth, ent->health);
	}
}

// add a flag to the main player - Jake
void PB_AddFlag(edict_t *ent, char *FlagSkinName, char* FlagGhoulFile)
{
  	// now bolt on a Ghoul object of the flag to the player 
	ggOinstC* myInstance;
	ggObjC* MyGhoulObj = game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());
	body_c*	body = ent->client->body;
	// find the Game Ghoul Instance of the Ghoul object hanging off my player
	myInstance = MyGhoulObj->FindOInst(ent->ghoulInst);
	body->AddBoltedItem(*ent, "abolt_hip_r", "items/ctf_flag", FlagGhoulFile, "to_abolt_hip_r", myInstance, FlagSkinName, 1.0);
}


void PB_RemoveFlag(edict_t *ent)
{
	ggOinstC	*MyGhoulInst=game_ghoul.FindOInst(ent->ghoulInst);
	GhoulID		theFlag;

	if (theFlag=ent->ghoulInst->GetGhoulObject()->FindPart("abolt_hip_r"))
	{
		MyGhoulInst->RemoveBolt(theFlag);
	}
}