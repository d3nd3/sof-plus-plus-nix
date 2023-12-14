#include "q_shared.h"
#include "..\ghoul\ighoul.h"
#include "w_public.h"
#include "w_types.h"
#include "w_weapons.h"
#include "w_utils.h"
#include "player.h"
#include "../gamecpp/game.h"

extern player_com_import_t	picom;
extern player_cl_import_t	picl;
extern player_sv_import_t	pisv;

extern int isClient;

//------------------------------------------------------------------------
//
//						SetWSkin
//
//------------------------------------------------------------------------

void SetWSkin(sharedEdict_t *self, char *matName, char *skinName)
{
	IGhoulObj	*obj = inven(*self)->getCurWeapon()->getWeaponInfo()->getGhoulObj();
	
	if(!obj)
		return;

	GhoulID		mat = obj->FindMaterial(matName);
	
	if(!mat)
		return;

	GhoulID skin = obj->FindSkin(mat, skinName);

	if(!skin)
		return;

	if(!inven(*self)->getViewModel())
		return;

	inven(*self)->getViewModel()->SetFrameOverride(mat, skin);
}

//------------------------------------------------------------------------
//
//						AddNoteCallbacks
//
//------------------------------------------------------------------------

void AddNoteCallbacks(IGhoulInst *inst, IGhoulObj *obj)
{
	GhoulID id;

	id = obj->FindNoteToken("sound");
	if(id)
	{
		inst->AddNoteCallBack(&theWeapSoundCallback, id);
	}

	id = obj->FindNoteToken("fire");
	if(id)
	{
		inst->AddNoteCallBack(&theGunFireCallback, id);
	}

	id = obj->FindNoteToken("altfire");
	if(id)
	{
		inst->AddNoteCallBack(&theAltfireCallback, id);
	}

	id = obj->FindNoteToken("eos");
	if(id)
	{
		inst->AddNoteCallBack(&theEOSCallback, id);
	}

	id = obj->FindNoteToken("cleanblood");
	if(id)
	{
		inst->AddNoteCallBack(&theKnifeCleanCallback, id);
	}

	id = obj->FindNoteToken("shellbullets");
	if(id)
	{
		inst->AddNoteCallBack(&theMinimiReloadCallback, id);
	}

	id = obj->FindNoteToken("effect");
	if(id)
	{
		inst->AddNoteCallBack(&theEffectCallback, id);
	}
}

//------------------------------------------------------------------------
//
//						SetVWeapGhoulClient
//
//------------------------------------------------------------------------

void SetVWeapGhoulClient(sharedEdict_t *self, char *modelName)
{
	IGhoulInst	*inst;
	IGhoulObj	*obj;

	// Destroy previous gun instance.

	if(inst = inven(*self)->getViewModel())
		inst->Destroy();

	// Now create a new gun instance by cloning the server owned gun instance.
	// Hence it will be PRIVATE to the client - which is what we want, else the
	// server owned instance will interfere with our predicted gun.

	inst=self->edict->client->ps.gun->Clone(true);

	inven(*self)->setViewModel(inst);

	// Add our callbacks and set the callback's user data.

	obj=inst->GetGhoulObject();

	if(!inven(*self)->getCurWeapon()->getWeaponInfo()->getGhoulObj())
		inven(*self)->getCurWeapon()->getWeaponInfo()->setGhoulObj(obj);

	// register all of the possible note tokens for weapons...

	AddNoteCallbacks(inst, obj);

	inst->SetUserData(self->inv);

	// Flag the server owned gun instance so that we know it is no longer new.

	self->edict->client->ps.gun->SetUserData(self->inv);
}

//------------------------------------------------------------------------
//
//						ReadyVWeapGhoulServer
//
//------------------------------------------------------------------------

void ReadyVWeapGhoulServer(sharedEdict_t *self, char *modelName)
{
	char	buffer[256];
	float	mat[16];

	picom.Com_Sprintf(buffer, sizeof(buffer), "Weapon/Inview/%s", modelName);

	IGhoulObj *obj;

	obj = inven(*self)->getReadiedWeapon()->getWeaponInfo()->getGhoulObj();

	if(!obj)
	{
		obj = ((IGhoul *)pisv.GetGhoul())->NewObj();

		if(!obj)
			return;

		picom.RegisterGSQSequences(buffer, modelName, obj);

		obj->RegisterEverything();

		inven(*self)->getReadiedWeapon()->getWeaponInfo()->setGhoulObj(obj);
	}

	IGhoulInst *inst = obj->NewInst();

	inven(*self)->setReadyModel(inst);
	
	// register all of the possible note tokens for weapons...

	AddNoteCallbacks(inst, obj);

	//fixme - temp for the demo
	GhoulID	part;

	part = obj->FindPart("_R_FOREARM");
	if(part)inst->SetPartOnOff(part, false);
	part = obj->FindPart("_L_FOREARM");
	if(part)inst->SetPartOnOff(part, false);
	part = obj->FindPart("_R_SLEEVE_FOREARM");
	if(part)inst->SetPartOnOff(part, true);
	part = obj->FindPart("_L_SLEEVE_FOREARM");
	if(part)inst->SetPartOnOff(part, true);

	SetWSkin(self, "arms", "arms_sleeve");

	inst->GetXForm(mat);
	mat[0]=0;
	mat[1]=1;
	mat[4]=-1;
	mat[5]=0;
	mat[14]=0;
	inst->SetXForm(mat);

	self->edict->client->ps.gun=inst;
	inst->SetUserData(self->inv);
}

//------------------------------------------------------------------------
//
//						SetReadiedVWeapGhoulServer
//
//------------------------------------------------------------------------

void SetReadiedVWeapGhoulServer(sharedEdict_t *self, char *modelName)
{
	IGhoulInst *inst = inven(*self)->getViewModel();

	if(inst)
		inst->Destroy();

	inven(*self)->setViewModel(inven(*self)->getReadyModel());
}

//------------------------------------------------------------------------
//
//						SetVWeapGhoulServer
//
//------------------------------------------------------------------------

void SetVWeapGhoulServer(sharedEdict_t *self, char *modelName)
{
	char	buffer[256];
	float	mat[16];

	picom.Com_Sprintf(buffer, sizeof(buffer), "Weapon/Inview/%s", modelName);

	IGhoulObj *obj;

	obj = inven(*self)->getCurWeapon()->getWeaponInfo()->getGhoulObj();

	if(!obj)
	{
		obj = ((IGhoul *)pisv.GetGhoul())->NewObj();

		if(!obj)
			return;

		picom.RegisterGSQSequences(buffer, modelName, obj);

		obj->RegisterEverything();

		inven(*self)->getCurWeapon()->getWeaponInfo()->setGhoulObj(obj);
	}

	IGhoulInst *inst = inven(*self)->getViewModel();

	if(inst)inst->Destroy();

	inst = obj->NewInst();

	inven(*self)->setViewModel(inst);

	// register all of the possible note tokens for weapons...

	AddNoteCallbacks(inst, obj);

	//fixme - temp for the demo
	GhoulID	part;

	part = obj->FindPart("_R_FOREARM");
	if(part)inst->SetPartOnOff(part, false);
	part = obj->FindPart("_L_FOREARM");
	if(part)inst->SetPartOnOff(part, false);
	part = obj->FindPart("_R_SLEEVE_FOREARM");
	if(part)inst->SetPartOnOff(part, true);
	part = obj->FindPart("_L_SLEEVE_FOREARM");
	if(part)inst->SetPartOnOff(part, true);

	SetWSkin(self, "arms", "arms_sleeve");

	inst->GetXForm(mat);
	mat[0]=0;
	mat[1]=1;
	mat[4]=-1;
	mat[5]=0;
	mat[14]=0;
	inst->SetXForm(mat);

	self->edict->client->ps.gun=inst;
	inst->SetUserData(self->inv);
}

//------------------------------------------------------------------------
//
//						SetVWeapGhoul
//
//------------------------------------------------------------------------

void SetVWeapGhoul(sharedEdict_t *self, char *modelName)
{
	if(!isClient)
		SetVWeapGhoulServer(self,modelName);
	else
		SetVWeapGhoulClient(self,modelName);
}

//------------------------------------------------------------------------
//
//						RunVWeapAnim
//
//------------------------------------------------------------------------

void RunVWeapAnim(sharedEdict_t *self, char *seqName, IGhoulInst::EndCondition loopType,float timeOffset)
{
	char		buffer[256];

	IGhoulObj	*obj = inven(*self)->getCurWeapon()->getWeaponInfo()->getGhoulObj();

	// SFW_NOWEAPON has no corresponding ghoul object.

	if(!obj)
		return;

	picom.Com_Sprintf(buffer, sizeof(buffer), "Ghoul/Weapon/Inview/%s/%s.ghl", inven(*self)->getViewModelName(), seqName);
	GhoulID seq = obj->FindSequence(buffer);
	
	if(!seq)
		return;

	wAnim(*self)->setSeqName(seqName);

	assert(inven(*self)->getViewModel());

	if(!isClient)
	{
		if(timeOffset!=0.0)
		{
			float diff=(*pisv.levelTime)-timeOffset;
			//picom.Com_DPrintf("SV - CL timediff = %f.\n",diff);
			inven(*self)->getViewModel()->Play(seq, (*pisv.levelTime)-diff, 0.0f, true, loopType);
		}
		else
		{
			inven(*self)->getViewModel()->Play(seq, *pisv.levelTime, 0.0f, true, loopType);
		}
	}
	else
	{
		inven(*self)->getViewModel()->Play(seq, *picl.levelTime, 0.0f, true, loopType);
	}
}

void RunReadiedVWeapAnim(sharedEdict_t *self, char *seqName, IGhoulInst::EndCondition loopType)
{
	char		buffer[256];

	IGhoulObj	*obj = inven(*self)->getReadiedWeapon()->getWeaponInfo()->getGhoulObj();

	// SFW_NOWEAPON has no corresponding ghoul object.

	if(!obj)
		return;

	picom.Com_Sprintf(buffer, sizeof(buffer), "Ghoul/Weapon/Inview/%s/%s.ghl", inven(*self)->getReadyModelName(), seqName);
	GhoulID seq = obj->FindSequence(buffer);
	
	if(!seq)
		return;

	wAnim(*self)->setSeqName(seqName);

	assert(inven(*self)->getReadyModel());

	if(!isClient)
		inven(*self)->getReadyModel()->Play(seq, *pisv.levelTime, 0.0f, true, loopType);
	else
		inven(*self)->getReadyModel()->Play(seq, *picl.levelTime, 0.0f, true, loopType);
}

//------------------------------------------------------------------------
//
//						ClientServerRand
//
//------------------------------------------------------------------------

int ClientServerRand(int mn,int mx)
{
	int t;

	if(mn>=mx)
		return(mn);

	if(!isClient)
		t=(int)((*pisv.levelTime)*10.0f);
	else
		t=(int)((*picl.levelTime)*10.0f);

	t=t*t;
	t&=(t>>7)^(t>>12)^(t>>4);
	t%=(1+mx-mn);

	return(t+mn);
}

