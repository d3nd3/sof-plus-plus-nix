#include "g_local.h"
#include "..\qcommon\ef_flags.h"

gameghoul_c game_ghoul;

///////////////////////////////////////////////////////////////////
//
//		game_ghoul_boltinstance_c
//		two real inworld things bolted together:the bolter is the
//		only one considered a real instance, for now, to reduce complexity
//		and because early tests seemed to indicate ghoul doesn't like it
//		when bolter gets removed and boltee sticks around. still left
//		this class in charge of destroying that instance, for possible flexibility.
///////////////////////////////////////////////////////////////////

ggBinstC::ggBinstC(ggBinstC *orig)
{
	bBoltedToOinst = orig->bBoltedToOinst;
	bolter.MakeIndex(orig->bolter);
	bolter2.MakeIndex(orig->bolter2);

	bolter_bolt = orig->bolter_bolt;

	scale = orig->scale;

	instance = NULL;

	bolt = orig->bolt;
	sequence = orig->sequence;
	object.MakeIndex(orig->object);
}

void ggBinstC::Evaluate(ggBinstC *orig)
{
	bBoltedToOinst = orig->bBoltedToOinst;
	bolter.MakePtr(*(int *)&orig->bolter);
	bolter2.MakePtr(*(int *)&orig->bolter2);
	bolter_bolt = orig->bolter_bolt;
	scale = orig->scale;
	bolt = orig->bolt;
	sequence = orig->sequence;
	object.MakePtr(*(int *)&orig->object);

	instance = LoadGhoulInst(object->GetMyObject(), object->GetName());
	// kef
	if (bBoltedToOinst)
	{
		instance->SetUserData(bolter->GetInstEdict());
		bolter->GetInstPtr()->Bolt(bolter_bolt, instance, bolt);
		bolter->AddBInst(this);
	}
	else
	{
		instance->SetUserData(bolter2->GetEdict());
		bolter2->GetInstPtr()->Bolt(bolter_bolt,instance,bolt);
		bolter2->AddBInst(this);
	}
}

void ggBinstC::Read()
{
	char		loaded[sizeof(ggBinstC)];

	gi.ReadFromSavegame('GGBI', loaded + GGBINST_SAVE_START, sizeof(ggBinstC) - GGBINST_SAVE_START);
	Evaluate((ggBinstC *)loaded);
}

void ggBinstC::AddBInst(ggBinstC *ggbinst)
{
	bolts.PushBack(ggbinst);
}

void ggBinstC::Write()
{
	ggBinstC	*savable;
	byte		*save_start;

	savable = new ggBinstC(this);
	save_start = (byte *)savable;
	gi.AppendToSavegame('GGBI', save_start + GGBINST_SAVE_START, sizeof(*this) - GGBINST_SAVE_START);
	savable->instance = NULL;
	delete savable;

	if(instance)
	{
		SaveGhoulInst(instance, object->GetName());
	}
}

void ggBinstC::ChangeOwnerTo(edict_t *newowner)
{
	List<gg_binst_c_ptr>::Iter	ibi;
	int i;
	assert(newowner);
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--)
	{
		(*ibi)->ChangeOwnerTo(newowner);
	}
	if (instance)
	{
		instance->SetUserData(newowner);
	}
}

ggBinstC::ggBinstC(ggOinstC *papa, GhoulID papaBolt, ggObjC *Obj, GhoulID Bolt, char *newskin, float newscale)
{
	bBoltedToOinst = true;
	bolter = papa;
	bolter_bolt = papaBolt;

	scale = newscale;

	Matrix4 oldM, scaleM, newM;

	bolt = Bolt;
	sequence = NULL;
	object = Obj;
	if (!papa || !papaBolt || !Obj || !Bolt)
	{
		return;
	}
	instance = Obj->GetMyObject()->NewInst();
	instance->SetUserData(papa->GetInstEdict());

	bolter->GetInstPtr()->Bolt(papaBolt,instance,Bolt);

	if (scale>1.00001||scale<0.99999)
	{
		instance->GetXForm(oldM);
		scaleM.Identity();
		scaleM.Scale(scale);
		newM.Concat(oldM,scaleM);
		instance->SetXForm(newM);
	}

	if (newskin)
	{
		GhoulID	tempMat=0;
		char	matname[100];

		strcpy(matname, Obj->GetSubName());

		//fixme: this is slow, but allows various material names.
		tempMat=Obj->GetMyObject()->FindMaterial(matname);
		if (!tempMat)
		{
			tempMat=Obj->FindMaterialForSkin(newskin);
		}
		if (tempMat)
		{
			instance->SetFrameOverride(tempMat,newskin);
		}
	}
}

ggBinstC::ggBinstC(ggBinstC *papa, GhoulID papaBolt, ggObjC *Obj, GhoulID Bolt, char *newskin, float newscale)
{
	bBoltedToOinst = false;
	bolter2 = papa;
	bolter_bolt = papaBolt;

	scale = newscale;

	Matrix4 oldM, scaleM, newM;

	bolt = Bolt;
	sequence = NULL;
	object = Obj;
	if (!papa || !papaBolt || !Obj || !Bolt)
	{
		return;
	}
	instance = Obj->GetMyObject()->NewInst();
	instance->SetUserData(papa->GetEdict());

	bolter2->GetInstPtr()->Bolt(papaBolt,instance,Bolt);

	if (scale>1.00001||scale<0.99999)
	{
		instance->GetXForm(oldM);
		scaleM.Identity();
		scaleM.Scale(scale);
		newM.Concat(oldM,scaleM);
		instance->SetXForm(newM);
	}

	if (newskin)
	{
		GhoulID	tempMat=0;
		char	matname[100];

		strcpy(matname, Obj->GetSubName());

		//fixme: this is slow, but allows various material names.
		tempMat=Obj->GetMyObject()->FindMaterial(matname);
		if (!tempMat)
		{
			tempMat=Obj->FindMaterialForSkin(newskin);
		}
		if (tempMat)
		{
			instance->SetFrameOverride(tempMat,newskin);
		}
	}
}

ggBinstC::~ggBinstC(void)
{
	List<gg_binst_c_ptr>::Iter ibi;
	// kef
	for (ibi=bolts.Begin(); ibi != bolts.End(); ibi++)
	{
		(*ibi).Destroy();
	}
	bolts.Erase(bolts.Begin(), bolts.End());

	if (instance)
	{
		// kef
		if (bBoltedToOinst)
		{
			if (bolter)
			{
				bolter->GetInstPtr()->UnBolt(instance);
			}
		}
		else
		{
			if (bolter2)
			{
				bolter2->GetInstPtr()->UnBolt(instance);
			}
		}
		instance->Destroy();
	}
}

void ggBinstC::PlaySequence(GhoulID Seq,float Now,float PlayPos,bool Restart,IGhoulInst::EndCondition ec, bool MatchCurrentPos, bool reverseAnim)
{
	sequence = Seq;
	if (sequence && instance)
	{
		instance->Play(Seq, Now, PlayPos, Restart, ec, MatchCurrentPos, reverseAnim);
	}
}

edict_t	*ggBinstC::GetEdict(void)
{
	if (bBoltedToOinst)
	{
		if (bolter)
		{
			return bolter->GetInstEdict();
		}
	}
	else
	{
		if (bolter2)
		{
			return bolter2->GetEdict();
		}
	}
	return NULL;
}

void ggBinstC::RemoveBolt(GhoulID bolted_to)
{
	List<gg_binst_c_ptr>::Iter ibi;
	List<gg_binst_c_ptr>::Iter	tibi;
	int i;
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--)
	{
		tibi = ibi++;
		if((* tibi)->GetBolterBolt()==bolted_to)
		{
			(*tibi).Destroy();
			bolts.Erase(tibi);
		}
	}
}

void ggBinstC::StopAnimatingAtBolt(GhoulID mybolt)
{
	List<gg_binst_c_ptr>::Iter ibi;
	int i;
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--,ibi++)
	{
		if((*ibi)->GetBolterBolt()==mybolt)
		{
			(*ibi)->GetInstPtr()->Pause(level.time);
		}
	}
}

void ggBinstC::StopAnimatingAtBolt(char *mybolt)
{
	List<gg_binst_c_ptr>::Iter ibi;
	int i;
	GhoulID namedbolt=object->GetMyObject()->FindPart(mybolt);
	if (namedbolt)
	{
		for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--,ibi++)
		{
			if((*ibi)->GetBolterBolt()==namedbolt)
			{
				(*ibi)->GetInstPtr()->Pause(level.time);
			}
		}
	}
}

void ggBinstC::StopAnimatingAtAllBolts(void)
{
	List<gg_binst_c_ptr>::Iter ibi;
	int i;
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--,ibi++)
	{
		(*ibi)->GetInstPtr()->Pause(level.time);
	}
}

void ggBinstC::RemoveBoltInstance(ggBinstC *boltee)
{
	List<gg_binst_c_ptr>::Iter ibi;
	List<gg_binst_c_ptr>::Iter	tibi;
	int i;
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--)
	{
		tibi = ibi++;
		if((* tibi)==boltee)
		{
			(*tibi).Destroy();
			bolts.Erase(tibi);
		}
	}
}

ggBinstC *ggBinstC::AddBoltInstance(GhoulID mybolt, ggObjC *babyObj, GhoulID babyBolt, char *boltskin, float newscale)
{
	gg_binst_c_ptr new_bolt;
	gg_binst_c_ptr its_me=this;

	new_bolt = new ggBinstC(its_me, mybolt, babyObj, babyBolt, boltskin, newscale);

	bolts.PushBack(new_bolt);

	babyObj->RegistrationLock();

	return new_bolt;
}

ggBinstC *ggBinstC::GetBoltInstance(GhoulID mybolt)
{
	List<gg_binst_c_ptr>::Iter ibi;
	List<gg_binst_c_ptr>::Iter	tibi;
	int i;
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--)
	{
		tibi = ibi++;
		if((* tibi)->GetBolterBolt()==mybolt)
		{
			return (*tibi);
		}
	}
	return NULL;
}

ggBinstC *ggBinstC::FindBoltInstance(IGhoulInst * boltInst)
{
	List<gg_binst_c_ptr>::Iter ibi;
	List<gg_binst_c_ptr>::Iter	tibi;
	int i;
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--)
	{
		tibi = ibi++;
		if((* tibi)->GetInstPtr()==boltInst)
		{
			return (*tibi);
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////
//
//	ggOinstC
//	an instance of an object--this is what you see in the game, an instance
//	playing a sequence
//////////////////////////////////////////////////////////////

ggOinstC::ggOinstC(ggOinstC *orig)
{
	TheInst = NULL;
	*(int *)&my_edict = GetEdictNum(orig->my_edict);
	my_obj.MakeIndex(orig->my_obj);
	sequence = orig->sequence;
}

void ggOinstC::Evaluate(ggOinstC *orig)
{
	my_edict = GetEdictPtr((int)orig->my_edict);
	my_obj.MakePtr(*(int *)&orig->my_obj);
	sequence = orig->sequence;

	TheInst = LoadGhoulInst(my_obj->GetMyObject(), my_obj->GetName());

	TheInst->SetUserData(my_edict);
	my_edict->ghoulInst = TheInst;

	my_obj->AddOInst(this, TheInst);
}

void ggOinstC::Read()
{
	char		loaded[sizeof(ggOinstC)];

	gi.ReadFromSavegame('GGOI', loaded + GGOINSTC_SAVE_START, sizeof(ggOinstC) - GGOINSTC_SAVE_START);
	Evaluate((ggOinstC *)loaded);
}

void ggOinstC::Write()
{
	ggOinstC					*savable;
	byte						*save_start;
	List<gg_binst_c_ptr>::Iter	abolt;
	gg_binst_c_ptr				temp;

	savable = new ggOinstC(this);
	save_start = (byte *)savable;
	gi.AppendToSavegame('GGOI', save_start + GGOINSTC_SAVE_START, sizeof(*this) - GGOINSTC_SAVE_START);
	savable->my_edict = NULL;
	delete savable;

	if(TheInst)
	{
		SaveGhoulInst(TheInst, my_obj->GetName());
	}
}

void ggOinstC::AddBInst(ggBinstC *ggbinst)
{
	bolts.PushBack(ggbinst);
}

ggOinstC::ggOinstC(ggObjC *this_object, edict_t *whose)
{
	char	temp[MAX_PATH];
	char name[GGHOUL_OBJ_NAME_LEN];
	char subclass[GGHOUL_OBJSUB_NAME_LEN];

	if (this_object)
	{
		TheInst = this_object->GetMyObject()->NewInst();
	}
	else
	{
		TheInst = NULL;
	}

	if (!TheInst)
	{
		return;
	}
	sequence = 0;

/*	if(whose->ghoulInst) - this should be valid in the future, but at the moment it breaks weapon switching :(
	{
		whose->ghoulInst->Destroy();
		whose->ghoulInst = 0;//Is this safe?  It would seem to be based on the test above...
		//assert(0);
	}*/

	my_obj = this_object;
	my_edict = whose;
	if (whose)
	{
		my_edict->ghoulInst = TheInst;
		my_edict->ghoulInst->SetUserData(my_edict);
	}

	strcpy(name, this_object->GetName());
	strcpy(subclass, this_object->GetSubName());

	Com_sprintf(temp, MAX_PATH, "%s_poff",subclass);

	gi.TurnOffPartsFromGSQFile(name, temp, this_object->GetMyObject(), TheInst);
}

void ggOinstC::ChangeOwnerTo(edict_t *newowner)
{
	List<gg_binst_c_ptr>::Iter	ibi;
	int i;
	assert(newowner);
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--)
	{
		(*ibi)->ChangeOwnerTo(newowner);
	}
	if (my_edict)
	{
		my_edict->ghoulInst=NULL;
	}
	if (my_edict->client->ps.bod)
	{
		my_edict->client->ps.bod=NULL;
	}
	my_edict = newowner;
	if (my_edict)
	{
		my_edict->ghoulInst = TheInst;
		my_edict->ghoulInst->SetUserData(my_edict);
	}
}

ggOinstC::ggOinstC(ggObjC *this_object, edict_t *whose, IGhoulInst *clonedInst)
{
	char name[GGHOUL_OBJ_NAME_LEN];
	char subclass[GGHOUL_OBJSUB_NAME_LEN];

	if (this_object && clonedInst)
	{
		TheInst = clonedInst->Clone(false);
	}
	else
	{
		TheInst = NULL;
	}

	if (!TheInst)
	{
		return;
	}

	sequence = 0;
/*	if(whose->ghoulInst) - this should be valid in the future, but at the moment it breaks weapon switching :(
	{
		whose->ghoulInst->Destroy();
		whose->ghoulInst = 0;//Is this safe?  It would seem to be based on the test above...
		//assert(0);
	}*/

	my_obj = this_object;
	my_edict = whose;
	my_edict->ghoulInst = TheInst;
	my_edict->ghoulInst->SetUserData(my_edict);

	strcpy(name, this_object->GetName());
	strcpy(subclass, this_object->GetSubName());
}

ggOinstC::~ggOinstC(void)
{
	List<gg_binst_c_ptr>::Iter ibi;
	List<gg_binst_c_ptr>::Iter	tibi;
	int i;
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--)
	{
		tibi = ibi++;
		(*tibi).Destroy();
	}
	if (my_edict/* && my_edict->ghoulInst==TheInst*/)
	{
		my_edict->ghoulInst=0;
	}
	if (TheInst)
	{
		TheInst->Destroy();
	}
}


void ggOinstC::RemoveAllBolts(void)
{
	List<gg_binst_c_ptr>::Iter ibi;
	List<gg_binst_c_ptr>::Iter	tibi;
	int i;
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--)
	{
		tibi = ibi++;
		(*tibi).Destroy();
		bolts.Erase(tibi);
	}
}

void ggOinstC::RemoveBolt(GhoulID bolted_to)
{
	List<gg_binst_c_ptr>::Iter ibi;
	List<gg_binst_c_ptr>::Iter	tibi;
	int i;
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--)
	{
		tibi = ibi++;
		if((* tibi)->GetBolterBolt()==bolted_to)
		{
			(*tibi).Destroy();
			bolts.Erase(tibi);
		}
	}
}

void ggOinstC::CopyBoltsTo(ggOinstC *recipient)
{
	List<gg_binst_c_ptr>::Iter ibi;
	ggBinstC	*curBolt;
	int i;
	if (!recipient)
	{
		return;
	}
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--)
	{
		curBolt = (*ibi);
		if (curBolt)
		{
			ggObjC* boltObj=(*ibi)->GetBolteeObject();
			ggBinstC* newBolt;
			newBolt=recipient->AddBoltInstance(curBolt->GetBolterBolt(), boltObj, curBolt->GetBolt(), /*char pointer for skin!*/NULL, curBolt->GetScale());\
			if (newBolt)
			{
				GhoulID tmat;
				newBolt->PlaySequence(curBolt->GetSequence(), level.time);//looping or not? dunno!!

				//replicate all frame overrides!
				for (tmat=boltObj->GetMyObject()->NumMaterials();tmat>0;tmat--)
				{
					if (curBolt->GetInstPtr()->GetFrameOverride(tmat))
					{
						newBolt->GetInstPtr()->SetFrameOverride(tmat, curBolt->GetInstPtr()->GetFrameOverride(tmat));
					}
				}
			}
		}
		ibi++;
	}
}

void ggOinstC::StopAnimatingAtBolt(GhoulID mybolt)
{
	List<gg_binst_c_ptr>::Iter ibi;
	int i;
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--,ibi++)
	{
		if((*ibi)->GetBolterBolt()==mybolt)
		{
			(*ibi)->GetInstPtr()->Pause(level.time);
		}
	}
}

void ggOinstC::StopAnimatingAtBolt(char *mybolt)
{
	List<gg_binst_c_ptr>::Iter ibi;
	int i;
	GhoulID namedbolt=my_obj->GetMyObject()->FindPart(mybolt);
	if (namedbolt)
	{
		for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--,ibi++)
		{
			if((*ibi)->GetBolterBolt()==namedbolt)
			{
				(*ibi)->GetInstPtr()->Pause(level.time);
			}
		}
	}
}

void ggOinstC::StopAnimatingAtAllBolts(void)
{
	List<gg_binst_c_ptr>::Iter ibi;
	int i;
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--,ibi++)
	{
		(*ibi)->GetInstPtr()->Pause(level.time);
	}
}

void ggOinstC::RemoveBoltInstance(ggBinstC *boltee)
{
	List<gg_binst_c_ptr>::Iter ibi;
	List<gg_binst_c_ptr>::Iter	tibi;
	int i;
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--)
	{
		tibi = ibi++;
		if((* tibi)==boltee)
		{
			(*tibi).Destroy();
			bolts.Erase(tibi);
		}
	}
}

ggBinstC *ggOinstC::AddBoltInstance(GhoulID mybolt, ggObjC *babyObj, GhoulID babyBolt, char *boltskin, float scale)
{
	gg_binst_c_ptr new_bolt;
	gg_inst_c_ptr its_me=this;

	new_bolt = new ggBinstC(its_me, mybolt, babyObj, babyBolt, boltskin, scale);

	bolts.PushBack(new_bolt);

	babyObj->RegistrationLock();

	return new_bolt;
}

ggBinstC *ggOinstC::GetBoltInstance(GhoulID mybolt)
{
	List<gg_binst_c_ptr>::Iter ibi;
	List<gg_binst_c_ptr>::Iter	tibi;
	int i;
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--)
	{
		tibi = ibi++;
		if((* tibi)->GetBolterBolt()==mybolt)
		{
			return (*tibi);
		}
	}
	return NULL;
}

ggBinstC *ggOinstC::FindBoltInstance(IGhoulInst * boltInst)
{
	List<gg_binst_c_ptr>::Iter ibi;
	List<gg_binst_c_ptr>::Iter	tibi;
	int i;
	for (ibi=bolts.Begin(),i = bolts.Size();i>0;i--)
	{
		tibi = ibi++;
		if((* tibi)->GetInstPtr()==boltInst)
		{
			return (*tibi);
		}
	}
	return NULL;
}

void ggOinstC::PlaySequence(GhoulID Seq,float Now,float PlayPos,bool Restart,IGhoulInst::EndCondition ec, bool MatchCurrentPos, bool reverseAnim)
{
	sequence = Seq;
	if (Seq)
	{
		TheInst->Play(Seq, Now, PlayPos, Restart, ec, MatchCurrentPos, reverseAnim);
	}
}

void ggOinstC::SetEdictInst(void)
{
	if (my_edict)
	{
		my_edict->ghoulInst = TheInst;
	}
}


/////////////////////////////////////////////////////////
//ghoul object
//		the object, plus all of its parts, bolts, & seqs for reference
/////////////////////////////////////////////////////////

ggObjC::ggObjC(ggObjC *orig)
{
	assert(orig->this_object);

	this_object = NULL;
	registration_locked = orig->registration_locked;
	register_all_skins = orig->register_all_skins;

	// Use strncpy to pad out the fields with nulls and to prevent any overflows
	strncpy(name, orig->name, GGHOUL_OBJ_NAME_LEN);
	strncpy(subname, orig->subname, GGHOUL_OBJSUB_NAME_LEN);
	strncpy(skinname, orig->skinname, GGHOUL_OBJSUBSUB_NAME_LEN);
	strncpy(basefile, orig->basefile, GGHOUL_OBJSUB_NAME_LEN);

	memcpy(availableSequences, orig->availableSequences, MAXIMUM_NUM_SEQUENCES);
}

void ggObjC::Evaluate(ggObjC *orig)
{
	bool	registration_locked_temp;
	char	temp[MAX_PATH];

	this_object = NULL;
	registration_locked = orig->registration_locked;
	register_all_skins = orig->register_all_skins;

	// Use strncpy to pad out the fields with nulls and to prevent any overflows
	strncpy(name, orig->name, GGHOUL_OBJ_NAME_LEN);
	strncpy(subname, orig->subname, GGHOUL_OBJSUB_NAME_LEN);
	strncpy(skinname, orig->skinname, GGHOUL_OBJSUBSUB_NAME_LEN);
	strncpy(basefile, orig->basefile, GGHOUL_OBJSUB_NAME_LEN);
	memcpy(availableSequences, orig->availableSequences, MAXIMUM_NUM_SEQUENCES);
	registration_locked_temp = registration_locked;
	registration_locked = false;

	// Reconstruct IGhoulObj (if it had one)
	this_object = TheGhoul->NewObj();

	if (basefile[0])
	{
		Com_sprintf(temp, MAX_PATH, "ghoul/%s/%s.ghl", name, basefile);
		this_object->RegisterSequence(temp);
	}
	gi.RegisterGSQSequences(name, subname, this_object);

	if(!register_all_skins)
	{
		int		count, i;
		char	*mat, *skin;

		this_object->RegisterEverything(false);

		gi.ReadFromSavegame('GGON', &count, sizeof(count));
		for(i = 0; i < count / 2; i++)
		{
			gi.ReadFromSavegame('STRG', NULL, 0, (void **)&mat);
			gi.ReadFromSavegame('STRG', NULL, 0, (void **)&skin);

			RegisterSkin(mat, skin);

			gi.TagFree((void *)mat);
			gi.TagFree((void *)skin);
		}
	}
	else
	{
		this_object->RegisterEverything(true);
	}
	registration_locked = registration_locked_temp;

	game_ghoul.AddObject(this_object, this);
}

void ggObjC::Read()
{
	char	loaded[sizeof(ggObjC)];

	gi.ReadFromSavegame('GGOB', loaded + GGOBJC_SAVE_START, sizeof(ggObjC) - GGOBJC_SAVE_START);
	Evaluate((ggObjC *)loaded);
}

void ggObjC::Write()
{
	ggObjC	*savable;
	byte	*save_start;

	savable = new ggObjC(this);
	save_start = (byte *)savable;
	gi.AppendToSavegame('GGOB', save_start + GGOBJC_SAVE_START, sizeof(*this) - GGOBJC_SAVE_START);
	delete savable;

	if(!register_all_skins)
	{
		int						count;
		list<string>::iterator	it;

		count = registered_skins.size();
		gi.AppendToSavegame('GGON', &count, sizeof(count));

		for(it = registered_skins.begin(); it != registered_skins.end(); it++)
		{
			gi.AppendToSavegame('STRG', (void *)(*it).c_str(), (*it).size() + 1);
		}
	}
}

ggObjC::ggObjC(void)
{
	this_object = NULL;
	registration_locked = false;
	register_all_skins = false;
}

ggObjC::ggObjC(const char* newname, const char* newsubname, bool allSkins, const char* newskinname, const char* newbasefile)
{
	// Make sure all strings are null
	memset(availableSequences, 0, MAXIMUM_NUM_SEQUENCES);
	memset(name, 0, GGHOUL_OBJ_NAME_LEN);
	memset(subname, 0, GGHOUL_OBJSUB_NAME_LEN);
	memset(skinname, 0, GGHOUL_OBJSUBSUB_NAME_LEN);
	memset(basefile, 0, GGHOUL_OBJSUB_NAME_LEN);

	registration_locked = false;
	register_all_skins = allSkins;
	this_object = TheGhoul->NewObj();
	strcpy(name, newname);
	strcpy(subname, newsubname);

	if (newskinname == NULL || newskinname[0]==0)
	{
		skinname[0]=0;
	}
	else
	{
		strcpy(skinname, newskinname);
	}

	if (newbasefile == NULL || newbasefile[0]==0)
	{
		basefile[0]=0;
	}
	else
	{
		char	temp[MAX_PATH];
		strcpy(basefile, newbasefile);
		sprintf(temp, "ghoul/%s/%s.ghl", name, basefile);
		this_object->RegisterSequence(temp);
	}
	//precache sequences according to name & subname.


	gi.RegisterGSQSequences(name, subname, this_object);

	if (register_all_skins)
	{
		this_object->RegisterEverything(true);

		//registered everything, might as well lock it here
		RegistrationLock();
	}
	else
	{
		this_object->RegisterEverything(false);

//		if (skinname[0])
//		{
//			RegisterSkin(skinname);
//		}
	}

	//this just sets up the ggSeqC list
//	LoadSubClassSequences(subname);
}

ggObjC::~ggObjC(void)
{
	map<IGhoulInst*,gg_inst_c_ptr>::iterator ii;

	for (ii=instances.begin();ii != instances.end(); ii++)
	{
		(*ii).second.Destroy();
	}

	if (this_object)
	{
		this_object->Destroy();
	}
}

void ggObjC::RegistrationLock(void)
{
//	char	temp[MAX_PATH];
	if (this_object && !registration_locked)
	{
//		This is no longer necessary, as we are creating an inst for the purpose of making a baseline
//		Com_sprintf(temp, MAX_PATH, "%s_poff",subname);
//		gi.PrecacheGSQFile(name, temp, this_object);

		ggOinstC *baseline=AddInstance(0);
		this_object->PreCache(ghl_specular->value>0.0f&&ghl_light_method->value>0.0f);
		RemoveInstance(baseline);
	}
	registration_locked = true;
}

void ggObjC::RemoveInstances(edict_t *who)
{
	map<IGhoulInst*,gg_inst_c_ptr>::iterator iteri,tempi;
	int i;

	if (!who->ghoulInst)
	{
		return;
	}

	//fixme: if we can trust there's only one inst for an edict, just go through removeInstance(ggOinstC*)
	for (iteri=instances.begin(),i=instances.size();i>0; i--)
	{
		tempi=iteri++;
		if ((*tempi).second->GetInstEdict()==who)
		{
			(*tempi).second.Destroy();
			instances.erase(tempi);
		}
	}
}

void ggObjC::RemoveInstances(void)
{
	map<IGhoulInst*,gg_inst_c_ptr>::iterator iteri,tempi;
	int i;
	//fixme: if we can trust there's only one inst for an edict, just go through removeInstance(ggOinstC*)
	for (iteri=instances.begin(),i=instances.size();i>0; i--)
	{
		tempi=iteri++;
		(*tempi).second.Destroy();
		instances.erase(tempi);
	}
}

void ggObjC::RemoveInstance(ggOinstC *inst)
{
	map<IGhoulInst*,gg_inst_c_ptr>::iterator iteri;
	iteri=instances.find(inst->GetInstPtr());
	if (iteri!=instances.end())
	{
		(*iteri).second.Destroy();
		instances.erase(iteri);
	}
}

ggOinstC *ggObjC::CloneInstance(edict_t *whose, IGhoulInst *clonedInst)
{
	gg_inst_c_ptr new_inst;

	new_inst = new ggOinstC(this, whose, clonedInst);

	//if sequence not available, forget it
	if (new_inst->GetInstPtr() == NULL)
	{
		new_inst.Destroy();
		return NULL;
	}
	instances[new_inst->GetInstPtr()] = new_inst;
	return new_inst;
}

ggOinstC *ggObjC::AddInstance(edict_t *whose)
{
	gg_inst_c_ptr new_inst;

	new_inst = new ggOinstC(this, whose);

	//if sequence not available, forget it
	if (new_inst->GetInstPtr() == NULL)
	{
		new_inst.Destroy();
		return NULL;
	}
	instances[new_inst->GetInstPtr()] = new_inst;
	return new_inst;
}

void ggObjC::AddOInst(ggOinstC *ggoinst, IGhoulInst *ghlinst)
{
	instances[ghlinst] = ggoinst;
}

ggOinstC	*ggObjC::FindOInst(IGhoulInst *findme)
{
	map<IGhoulInst*,gg_inst_c_ptr>::iterator	iid;
	ggOinstC *found_id=NULL;

	iid=instances.find(findme);
	if (iid!=instances.end())
	{
		found_id=(*iid).second;
	}
	return found_id;
}


GhoulID ggObjC::FindSequence(const char* seqname)
{
	char seqName[GGHOUL_OBJ_NAME_LEN];
	sprintf(seqName,"ghoul/%s/%s.ghl",name,seqname);
	if (registration_locked)
	{
		return this_object->FindSequence(seqName);
	}
	gi.dprintf("Tried to find sequence %s before registration finished!\n", seqname);
	return 0;
}

GhoulID ggObjC::FindMaterialForSkin(char* findskinname)
{
	GhoulID	curMat, maxMat;
	maxMat=this_object->NumMaterials();
	for (curMat=1;curMat<=maxMat;curMat++)
	{
		if (this_object->FindSkin(curMat, findskinname))
		{
			return curMat;
		}
	}
	return 0;
}

void ggObjC::SetEdictInsts(void)
{
	map<IGhoulInst*,gg_inst_c_ptr>::iterator ainst;
	for (ainst = instances.begin(); ainst != instances.end();ainst++)
	{
		(*ainst).second->SetEdictInst();
	}
}

GhoulID ggObjC::RegisterSkin(const char* matname, const char *regskinname)
{
	GhoulID		matVal;

	matVal = this_object->FindMaterial(matname);

	//material's no good!
	if (!matVal)
	{
		return 0;
	}

	//i'm done registering, so sod off
	if (registration_locked)
	{
		return this_object->FindSkin(matVal, regskinname);
	}
	registered_skins.push_back(matname);
	registered_skins.push_back(regskinname);

	return this_object->RegisterSkin(matVal, regskinname); 
}

GhoulID ggObjC::RegisterSkin(const char *regskinname)
{
	GhoulID	matVal,tSkin;
	GhoulID	maxMat;
	char	matname[MAX_OSPATH];

	//i've got instances, so sod off
	if (registration_locked)
	{
		return 0;
	}

	maxMat = this_object->NumMaterials();
	for (matVal = 1; matVal <= maxMat; matVal++)
	{
		tSkin = this_object->RegisterSkin(matVal, regskinname);
		this_object->GetMaterialName(matVal, matname);

		registered_skins.push_back(matname);
		registered_skins.push_back(regskinname);

		if (tSkin)
		{
			return tSkin;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////
//game_ghoul_c
//		list of all ghoul objects currently in use
///////////////////////////////////////////////////////////////

gameghoul_c::~gameghoul_c(void)
{
	map<IGhoulObj *, gg_obj_c_ptr>::iterator	iob;

	for (iob = objects.begin(); iob != objects.end(); iob++)
	{
		(*iob).second->RemoveInstances();
	}
	for (iob = objects.begin(); iob != objects.end(); iob++)
	{
		(*iob).second.Destroy();
	}
}

ggOinstC	*gameghoul_c::FindOInst(IGhoulInst *findme)
{
	map<IGhoulObj*,gg_obj_c_ptr>::iterator	iid;
	ggObjC		*found_obj=NULL;

	//if the ghoul inst we're looking for is screwy (NULL or from a NULL Object), forget it.
	if (!findme || !findme->GetGhoulObject())
	{
		return NULL;
	}

	iid=objects.find(findme->GetGhoulObject());
	if (iid!=objects.end())
	{
		found_obj=(*iid).second;
	}

	//if i didn't find the object, or it's NULL, forget it.
	if (!found_obj)
	{
		return NULL;
	}

	//use the object to find the inst
	return found_obj->FindOInst(findme);
}

//fixme: don't cleanup seqs, etc. that will be used in new level for leveltransitions
void gameghoul_c::LevelCleanUp(void)
{
	map<IGhoulObj*,gg_obj_c_ptr>::iterator	iob;

	//get rid of all instances first
	for(iob = objects.begin(); iob != objects.end(); iob++)
	{
		(*iob).second->RemoveInstances();
	}
	while(objects.size())
	{
		objects.begin()->second.Destroy();
		objects.erase(objects.begin());
	}
	TheGhoul->DestroyAllObjects();
}

#include "ai_bodynoghoul.h"

ggObjC *gameghoul_c::AddObject(const char* name, const char* subname, bool allSkins, const char *skinname, const char *basefile)
{
	gg_obj_c_ptr new_ob;

	new_ob = new ggObjC(name, subname, allSkins, skinname, basefile);

	//if object not available, forget it
	if (new_ob->GetMyObject() == NULL)
	{
		new_ob.Destroy();
		return NULL;
	}

	// oh dear
	if(!strncmp(name, "enemy/meso", strlen("enemy/meso")) ||
		!strncmp(name, "enemy/ecto", strlen("enemy/ecto")) ||
		!strncmp(name, "enemy/female", strlen("enemy/female")))
	{
		mmove_t *curMove = MMoves;

		while(*(curMove->ghoulSeqName))
		{
			char seqName[GGHOUL_OBJ_NAME_LEN];
			Com_sprintf(seqName, GGHOUL_OBJ_NAME_LEN, "ghoul/%s/%s.ghl",new_ob->GetName(),curMove->ghoulSeqName);
			if(new_ob->GetMyObject()->FindSequence(seqName))
			{
				new_ob->SetSequenceAvailable(curMove - MMoves, 1);
			}
			else
			{
				new_ob->SetSequenceAvailable(curMove - MMoves, 0);
			}

			curMove++;
		}
	}
	else
	{
		new_ob->SetAllSequences(1);
	}


	objects[new_ob->GetMyObject()]=new_ob;
	return new_ob;
}

//set dontMakeNew to true if you only want an object that's already totally set up--to avoid caching stuff in the middle of a level
ggObjC *gameghoul_c::FindObject(const char* name, const char* subname, bool allSkins, const char *skinname, const char *basefile, bool dontMakeNew)
{
	map<IGhoulObj*,gg_obj_c_ptr>::iterator	iob;
	char	*curSkinName;
	char	*curBaseFile;

	for (iob=objects.begin();iob != objects.end();iob++)
	{
		//not using index for main check--compare names
		if (stricmp((*iob).second->GetName(),name))
		{
			continue;
		}

		//not using index for sub check--compare names
		if (stricmp((*iob).second->GetSubName(),subname))
		{
			continue;
		}

		//wretched extra division to allow separate objects for simple objects
		//safely assume that curSkinName is not NULL, no such assumption about skinname
		curSkinName=(*iob).second->GetSkinName();
		if ((skinname&&stricmp(curSkinName,skinname))||(!skinname&&curSkinName[0]))
		{
			continue;
		}

		//even more wretched extra extra division to allow separate objects for guys who look for the same seqs in different .ghb's
		//--this allows us to cull out parts and sequences for specific levels' enemies, mainly for memory savings
		//safely assume that curSkinName is not NULL, no such assumption about skinname
		curBaseFile=(*iob).second->GetBaseFile();
		if ((basefile&&stricmp(curBaseFile,basefile))||(!basefile&&curBaseFile[0]))
		{
			continue;
		}

		//the comparisons checked out, this object matches
		return (*iob).second;
	}
	if (dontMakeNew)//not safe to make a new obj--probably not precached & not allowed
	{
		return NULL;
	}
	return AddObject(name, subname, allSkins, skinname, basefile);
}

ggOinstC *gameghoul_c::AddObjectInstance(ggObjC *curObject, edict_t *whose)
{
	if (curObject)
	{
		return curObject->AddInstance(whose);
	}
	//object not found, return null
	return NULL;
}

//remove all instances associated with an entity--this is brute-force: instead start from edict's ghoulinst?
void	gameghoul_c::RemoveObjectInstances(edict_t *who)
{
	map<IGhoulObj*,gg_obj_c_ptr>::iterator	iob;

	if(!who)
	{
		return;
	}

	for (iob=objects.begin();iob != objects.end();iob++)
	{
		(*iob).second->RemoveInstances(who);
	}

	who->ghoulInst=NULL;
}

void	gameghoul_c::RemoveObjectInstances(ggOinstC *Oinst)
{
	Oinst->GetParentObject()->RemoveInstance(Oinst);
}

GhoulID gameghoul_c::FindObjectSequence(ggObjC *curObject, const char* seqname)
{
	if (curObject)
	{
		return curObject->FindSequence(seqname);
	}
	//object not found, return nullid
	return NULL;
}

ggObjC *gameghoul_c::FindObject(IGhoulObj *curObject)
{
	map<IGhoulObj*,gg_obj_c_ptr>::iterator	iob;

	iob = objects.find(curObject);
	if (iob==objects.end())
	{
		return NULL;
	}
	return (*iob).second;
}


void gameghoul_c::LevelPrecache(void)
{
	ggObjC *cacheObj;
	cacheObj=FindObject("Enemy/bolt", "g_arm_torn_anim1");
	cacheObj=FindObject("Enemy/bolt", "g_bone_anim1");
	cacheObj=FindObject("Enemy/bolt", "g_brain");
	cacheObj=FindObject("Enemy/bolt", "g_cap1");
	cacheObj=FindObject("Enemy/bolt", "g_cap2");
	cacheObj=FindObject("Enemy/bolt", "g_cap3");
	cacheObj=FindObject("Enemy/bolt", "g_exit_chunky_anim1");
	cacheObj=FindObject("Enemy/bolt", "guts_bolton");
	cacheObj=FindObject("Enemy/bolt", "g_exit_lrg");
	cacheObj=FindObject("Enemy/bolt", "g_exit_smll");
	cacheObj=FindObject("Enemy/bolt", "g_guts");
	cacheObj=FindObject("Enemy/bolt", "g_guts3");
	cacheObj=FindObject("Enemy/bolt", "g_neck_torn_anim2");
}

ggBinstC *gameghoul_c::AddBoltInstance(ggOinstC *bolter, GhoulID bolterBolt, ggObjC *boltee, GhoulID bolteeBolt, GhoulID bolteeSeq, char *boltskin, float scale)
{
	ggBinstC *new_bolt = NULL;

	if (boltee&&bolterBolt&&bolteeBolt&&bolteeSeq)
	{
		new_bolt = bolter->AddBoltInstance(bolterBolt, boltee, bolteeBolt, boltskin, scale);
		if (new_bolt)
		{
			new_bolt->PlaySequence(bolteeSeq,level.time,0.0,true,IGhoulInst::Loop,false,false);
		}
	}
	else
	{
		gi.dprintf("Unable to add bolt on!\n"); 
	}
	return new_bolt;
}

ggBinstC *gameghoul_c::AddBoltInstance(ggOinstC *bolter, GhoulID bolterBolt, ggObjC *boltee, GhoulID bolteeBolt, char *boltskin, float scale)
{
	ggBinstC *new_bolt = NULL;

	if (boltee&&bolterBolt&&bolteeBolt)
	{
		new_bolt = bolter->AddBoltInstance(bolterBolt, boltee, bolteeBolt, boltskin, scale);
	}
	else
	{
		gi.dprintf("Unable to add bolt on!\n"); 
	}
	return new_bolt;
}

void gameghoul_c::SetSimpleGhoulModel(edict_t *ent, const char* dirname, const char *seqname, const char *skinname, const char *matname)
{
	ggObjC *simpObj;
	GhoulID simpSeq;
	ggOinstC *simpInst;
	char	subclassname[GGHOUL_OBJSUB_NAME_LEN];
//	char	tempseqname[GGHOUL_ID_NAME_LEN];

	if (ent->ghoulInst || (ent->s.renderfx&RF_GHOUL))
	{
//		gi.dprintf("Entity already has ghoul properties--SetSimpleGhoulModel aborting!\n");
		return;
	}

//	sprintf(subclassname,"simple%s",seqname);
	strcpy(subclassname,seqname);
	if (skinname && skinname[0] && matname && matname[0])
	{
		simpObj = FindObject(dirname, subclassname, false, skinname);
		simpObj->RegisterSkin(matname, skinname);
	}
	//no skinname specified--register all skins
	else
	{
		simpObj = FindObject(dirname, subclassname);
	}
	if (!simpObj)
	{
		gi.dprintf("SetSimpleGhoulModel couldn't create ghoul object %s/%s.\n", dirname, subclassname);
		return;
	}

	simpObj->RegistrationLock();

	simpSeq = simpObj->FindSequence(seqname);//true);
	if (!simpSeq)
	{
		//sequence not there already--register it
//		if (dirname[0]==0)
//		{
//			sprintf(tempseqname,"Ghoul/%s.GHL",seqname);
//		}
//		else
//		{
//			sprintf(tempseqname,"Ghoul/%s/%s.GHL",dirname,seqname);
//		}
//		simpObj->GetMyObject()->RegisterSequence(tempseqname);
//		simpSeq = simpObj->FindSequence(seqname, true);
//		if (!simpSeq)
//		{
			gi.dprintf("SetSimpleGhoulModel couldn't find ghoul sequence %s.\n",seqname);
			return;
//		}
	}

	simpInst = simpObj->AddInstance(ent);
	if (!simpInst)
	{
		gi.dprintf("SetSimpleGhoulModel couldn't add instance of ghoul object %s.\n",dirname);
		return;
	}

	ent->ghoulInst = simpInst->GetInstPtr();
	ent->s.renderfx = RF_GHOUL;
	simpInst->PlaySequence(simpSeq, level.time);

	//tried to get gview to not loop (for testing) with this... no luck (!)
//	simpInst->PlaySequence(simpSeq, level.time, true, IGhoulInst::Hold);
}

void gameghoul_c::AddObject(IGhoulObj *ghlobj, ggObjC *ggobjc)
{
	objects[ghlobj] = ggobjc;
}
