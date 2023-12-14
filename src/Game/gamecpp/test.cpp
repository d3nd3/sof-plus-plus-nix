#include "g_local.h"
#include "callback.h"
#include "..\qcommon\ef_flags.h"
#include <windows.h>

static bool testSeqEnded;
static int testSeqCounter;

TestSeqEndCallback theTestSeqEndCallback;

bool TestSeqEndCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	edict_t *self=(edict_t *)ent;

	testSeqEnded = true;
	return true;
}

void ghoul_model_think (edict_t *self)
{
	GhoulID cshootSeq=0;
	ggObjC *cshoot=0;
	GhoulID	tempNote;

	//see if my end-of sequence callback has fired--if so, get a new sequence
	if (testSeqEnded)
	{
		gi.dprintf("my sequence has ended! hooha!\n");

		//get my object again, to find sequences & notes
		cshoot=game_ghoul.FindObject("enemy/ecto","officer");

		if (testSeqCounter > 6)
		{
			//no new sequence--want to get rid of the callback, just to show how to do it if ya want.
		}
		else
		{
			//pick a new sequence
			switch (testSeqCounter%2)
			{
			case 0:
				//this is stretching anim
				cshootSeq = game_ghoul.FindObjectSequence(cshoot,"std_istretch_n_a_a");
				break;
			case 1:
			default:
				//this is a simple breathe anim
				cshootSeq = game_ghoul.FindObjectSequence(cshoot,"std_istand_n_a_n");
				break;
			}
			testSeqCounter++;
		}

		//if my new sequence is valid, play it
		if (cshootSeq)
		{
			self->ghoulInst->Play(cshootSeq,level.time,0.0f,true,IGhoulInst::Hold);
			testSeqEnded = false;
		}
		else
		{
			//see if i've registered the end-of-sequence note
			tempNote=cshoot->GetMyObject()->FindNoteToken("EOS");

			//remove the end-of-sequence callback
			if (tempNote)
			{
				testSeqEnded = false;
				self->ghoulInst->RemoveNoteCallBack(&theTestSeqEndCallback,tempNote);

				gi.dprintf("getting rid of seq-end callback\n");

				//play whatever my first sequence is, just to prove there's no seq-end callback there
				cshootSeq = game_ghoul.FindObjectSequence(cshoot,"std_ilooking_n_a_a");
				if (cshootSeq)
				{
					self->ghoulInst->Play(cshootSeq,level.time,0.0f,true,IGhoulInst::Loop);
					testSeqEnded = false;
				}
			}
		}
	}
	self->nextthink = level.time + FRAMETIME;
}

void SP_ghoul_model (edict_t *ent)
{
	ggObjC *cshoot=0;
	ggObjC *cgun=0;
	GhoulID cshootSeq=0;
	GhoulID cshootBolt=0;
	GhoulID gunSeq=0;
	GhoulID gunBolt=0;
	ggOinstC *t2;
	ggBinstC	*gunBolted;
	GhoulID	tempNote;

	ent->movetype = MOVETYPE_STEP;
	ent->solid = SOLID_BBOX;
	ent->s.renderfx = RF_GHOUL;

	cshoot = game_ghoul.FindObject("enemy/ecto","officer");
	cgun = game_ghoul.FindObject("enemy/bolt","w_briefcase");
	cshootSeq = game_ghoul.FindObjectSequence(cshoot,"std_ilooking_n_a_a");
	gunSeq = game_ghoul.FindObjectSequence(cgun,"w_briefcase");
	cshootBolt = cshoot->GetMyObject()->FindPart("wbolt_hand_l");
	gunBolt = cgun->GetMyObject()->FindPart("to_wbolt_hand_l");

	t2 = game_ghoul.AddObjectInstance(cshoot, ent);

	ent->ghoulInst=t2->GetInstPtr();
	ent->ghoulInst->SetUserData(ent);

	if (cshootSeq)
	{
		ent->ghoulInst->Play(cshootSeq,level.time,0.0f,true,IGhoulInst::Hold);

		//see if i've registered the end-of-sequence note
		tempNote=cshoot->GetMyObject()->FindNoteToken("EOS");

		//add the end-of-sequence callback
		if (tempNote)
		{
			testSeqEnded = false;
			testSeqCounter = 0;
			ent->ghoulInst->AddNoteCallBack(&theTestSeqEndCallback,tempNote);
		}
	}

	if (gunSeq && cgun && gunBolt && t2 && cshootBolt)
	{
		gunBolted=t2->AddBoltInstance(cshootBolt, cgun, gunBolt);
		gunBolted->PlaySequence(gunSeq,level.time,0.0,true,IGhoulInst::HoldFrame,false,false);
	}

	VectorSet (ent->mins, -16, -16, -32);
	VectorSet (ent->maxs, 16, 16, 16);

	ent->think = ghoul_model_think;
	ent->nextthink = level.time + FRAMETIME;

	gi.linkentity (ent);
}



class TestCallback :public IGhoulCallBack
{
public:
	bool Execute(IGhoulInst *me,void *ent,float time,const void *matrix);
};


TestCallback theTestCallback;

bool TestCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{
	static DWORD	last;

	edict_t *self=(edict_t *)ent;

//	Com_Printf("Diff = %10.2f\n",level.time);
//	last = timeGetTime();

	return true;
}



/*QUAKED test_rj (1 .5 0) (-16 -16 -16) (16 16 16)
RJ testing model
*/

void SP_test_rj (edict_t *ent)
{

	return;

	ent->movetype = MOVETYPE_STEP;
	ent->solid = SOLID_BBOX;

	game_ghoul.SetSimpleGhoulModel(ent, "rj", "rj");
//	game_ghoul.SetSimpleGhoulModel(ent, "objects/newyork/gumball", "gumball");
	if (ent->ghoulInst)
	{
		ent->ghoulInst->AddNoteCallBack(&theTestCallback,"rj");
	}


	gi.linkentity (ent);

/*	ggObjC *cshoot=0;
	ggObjC *cgun=0;
	GhoulID cshootSeq=0;
	GhoulID cshootBolt=0;
	GhoulID gunSeq=0;
	GhoulID gunBolt=0;
	ggOinstC *t2;
	GhoulID	tempNote;

	ent->movetype = MOVETYPE_STEP;
	ent->solid = SOLID_BBOX;
	ent->s.renderfx = RF_GHOUL;

	cshoot = game_ghoul.FindObject("rj","rj");
	cshootSeq = game_ghoul.FindObjectSequence(cshoot,"rj");

	t2 = game_ghoul.AddObjectInstance(cshoot, ent);

	ent->ghoulInst=t2->GetInstPtr();
	ent->ghoulInst->SetUserData(ent);

	if (cshootSeq)
	{
		ent->ghoulInst->Play(cshootSeq,level.time,0.0f,true,IGhoulInst::Hold);

		//see if i've registered the end-of-sequence note
		tempNote=cshoot->GetMyObject()->FindNoteToken("RJ");

		//add the end-of-sequence callback
		if (tempNote)
		{
			testSeqEnded = false;
			testSeqCounter = 0;
			ent->ghoulInst->AddNoteCallBack(&theTestCallback,tempNote);
		}
	}

	VectorSet (ent->mins, -16, -16, -32);
	VectorSet (ent->maxs, 16, 16, 16);

	gi.linkentity (ent);*/
}



