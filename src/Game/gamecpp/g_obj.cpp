/******************************************************
 * Miscellaneous Object Code                          *
 ******************************************************/

#include "g_local.h"
#include "g_obj.h"
#include "windows.h"

int boltInstInfo_c::nMax = MAX_BOLTINSTINFO;

//	The code I stole from Bob didn't actually properly adjust the bounding boxes. 
//	So, at Rick's prompting, I'm going to put in a cheap, lame hardcode to fix bounding
//	boxes for 90, 180, and 270 degree rotations.  It's fast and it works.
//
void breakable_brush_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void EntToWorldMatrix(vec3_t org, vec3_t angles, Matrix4 &m);

void BboxRotate(edict_t *self)
{
	vec3_t holdmins,holdmaxs,holdangs;

	VectorCopy(self->mins, holdmins);
	VectorCopy(self->maxs, holdmaxs);
	VectorCopy(self->s.angles, holdangs);

	anglemod(holdangs[0]); // replace the following lines, prevent the possibility of a loooooooong loop
/*	while(holdangs[0] < 0)
	{
		holdangs[0] += 360;
	}
	while(holdangs[0] > 359)
	{
		holdangs[0] -= 360;
	}
*/
	switch ((int)holdangs[0]) // pitch
	{
	case(90):
		self->mins[0] = holdmins[2];
		self->mins[2] = -1 * holdmaxs[0];
		self->maxs[0] = holdmaxs[2];
		self->maxs[2] = -1 * holdmins[0];		
		break;
	case(180):
		self->mins[0] = -1 * holdmaxs[0];
		self->mins[2] = -1 * holdmaxs[2];
		self->maxs[0] = -1 * holdmins[0];
		self->maxs[2] = -1 * holdmins[2];
		break;
	case(270):
		self->mins[0] = -1 * holdmaxs[2];
		self->mins[2] = holdmins[0];
		self->maxs[0] = -1 * holdmins[2];
		self->maxs[2] = holdmaxs[0];
		break;
	default:
		break;
	}

	anglemod(holdangs[1]); // replace the following lines, prevent the possibility of a loooooooong loop
/*	while(holdangs[1] < 0)
	{
		holdangs[1] += 360;
	}
	while(holdangs[1] > 359)
	{
		holdangs[1] -= 360;
	}
*/	
	switch ((int)holdangs[1]) // yaw
	{
	case(90):
		self->mins[0] = -1 * holdmaxs[1];
		self->mins[1] = holdmins[0];
		self->maxs[0] = -1 * holdmins[1];
		self->maxs[1] = holdmaxs[0];		
		break;
	case(180):
		self->mins[0] = -1 * holdmaxs[0];
		self->mins[1] = -1 * holdmaxs[1];
		self->maxs[0] = -1 * holdmins[0];
		self->maxs[1] = -1 * holdmins[1];
		break;
	case(270):
		self->mins[0] = holdmins[1];
		self->mins[1] = -1 * holdmaxs[0];
		self->maxs[0] = holdmaxs[1];
		self->maxs[1] = -1 * holdmins[0];
		break;
	default:
		break;
	}

	anglemod(holdangs[2]); // replace the following lines, prevent the possibility of a loooooooong loop
/*	while(holdangs[2] < 0)
	{
		holdangs[2] += 360;
	}
	while(holdangs[2] > 359)
	{
		holdangs[2] -= 360;
	}
*/	
	switch ((int)holdangs[2]) // roll
	{
	case(90):
		self->mins[1] = -1 * holdmaxs[2];
		self->mins[2] = holdmins[1];
		self->maxs[1] = -1 * holdmins[2];
		self->maxs[2] = holdmaxs[1];		
		break;
	case(180):
		self->mins[1] = -1 * holdmaxs[1];
		self->mins[2] = -1 * holdmaxs[2];
		self->maxs[1] = -1 * holdmins[1];
		self->maxs[2] = -1 * holdmins[2];
		break;
	case(270):
		self->mins[1] = holdmins[2];
		self->mins[2] = -1 * holdmaxs[1];
		self->maxs[1] = holdmaxs[2];
		self->maxs[2] = -1 * holdmins[1];
		break;
	default:
		break;
	}
	return;
}

void PhysicsModelInit(edict_t *self, char *rendername)
{
	self->solid = SOLID_BSP;
	gi.setmodel(self, self->model);
	gi.setrendermodel(self, rendername);
// rjr too dangerous	self->rendermodel = rendername;
	self->s.angle_diff = self->s.angles[1];
	self->s.angles[1] = 0;
}

void GhoulPhysicsModelInit(edict_t *self)
{
	self->solid = SOLID_BSP;
	gi.setmodel(self, self->model);
	self->s.angle_diff = self->s.angles[1];
	self->s.angles[1] = 0;
}

void TintModel(edict_t *self)
{
	if (!self->ghoulInst)
	{
		return;
	}
	if (st.color[0] || st.color[1] || st.color[2])
	{
		self->ghoulInst->SetTint(st.color[0],st.color[1],st.color[2],1);
	}
	return;
}

void SetSkin2(IGhoulInst* inst, char* modelName, char* modelSubname, char *matName, char *skinName)
{
	if (!inst)
	{
		Com_Printf("ERROR: trying to set skin %s on NULL ghoulInst!\n", skinName);
		return;
	}
	if (inst->SetFrameOverride(matName, skinName))
	{
		//successful. YEAH!
		return;
	}

	//this stuff just for debugging:
	if (!inst->GetGhoulObject() || !inst->GetGhoulObject()->FindMaterial(matName))
	{
		Com_Printf ("WARNING: Can't find material:%s\n",matName);
		return;
	}
	if (!inst->GetGhoulObject()->FindSkin(inst->GetGhoulObject()->FindMaterial(matName), skinName))
	{
		Com_Printf ("WARNING: Can't find skin:%s\n", skinName);
		return;
	}
	Com_Printf ("WARNING: unknown error in SetSkin2 on object %s/%s.\n", modelName, modelSubname);
}

void SetSkin(edict_t *self, char* modelName, char* modelSubname, char *matName, char *skinName, int nSetBoltonSkins)
{
	IGhoulInst*		boltInst = NULL;
	boltInstInfo_c	*boltInfo = NULL;
	baseObjInfo_c	*objInfo = NULL;
	int				i = 0;

	if (self)
	{
		// if nSetBoltonSkins is true, find all boltons and set their skin also (duh)
		SetSkin2(self->ghoulInst, modelName, modelSubname, matName, skinName);
		if (nSetBoltonSkins)
		{
			if ( (objInfo = self->objInfo) && (boltInfo = (boltInstInfo_c*)objInfo->GetInfo(OIT_BOLTINST)) )
			{
				// this may look like we're using the wrong modelName and modelSubname for this
				//bolton, but SetSkin2 only uses those names for debugging purposes -- all it
				//really needs is an IGhoulInst*, a matName, and a skinName
				for (i = 1; i <= boltInfo->GetNum(); i++)
				{
					if (boltInst = SimpleModelGetBolt(self, i))
					{
						SetSkin2(boltInst, modelName, modelSubname, matName, skinName);
					}
				}
			}
		}
	}
	else
		Com_Printf ("WARNING: Can't set skin for NULL edict\n");

	return;
}


void BecomeDebrisFinal(edict_t *self,byte numchunks,byte scale,edict_t *attacker)
{
	float			volume;
	vec3_t			origin;
	byte x_max,y_max,z_max;
	vec3_t			debrisNorm;

	if (!scale || !numchunks)
	{
		volume = self->size[0] * self->size[1] * self->size[2];

		// Calc numchunks???
		if (!numchunks)
		{
			numchunks = (byte)((volume / 9000) + 2); 
		}

		if (numchunks > 20)
			numchunks = 20;
		else if (numchunks < 10)
			numchunks = 10;

		// sending a scale of 0 will not give you the debris you want
		if (0 == scale)
		{
			scale = DEBRIS_SM;
		}
	}

	// Calc scale on breakable brushes
	if (strcmp(self->classname, "func_breakable_brush") == 0)
	{
		// Find scale of debris to throw
		if (volume > 250000)
			scale = DEBRIS_LRG;
		else if (volume > 40000)
			scale = DEBRIS_MED;
		else 
			scale = DEBRIS_SM;
	}

	VectorAdd(self->absmax,self->absmin, origin);
	VectorScale(origin, .5, origin);

	if (self->material > 0)
	{
		if (strcmp(self->classname, "func_breakable_brush") == 0)
		{
			if (self->count)
			{
				numchunks = self->count;
				scale = self->mass;
				x_max = 0;
				y_max = 0;
				z_max = 0;
			}
			else
			{
				x_max = self->size[0];
				y_max = self->size[1];
				z_max = self->size[2]*0.5;
			}
		}
		else
		{
			x_max = (byte) self->maxs[0];
			y_max = (byte) self->maxs[1];
			z_max = (byte) self->maxs[2];
		}

		VectorSubtract (attacker->s.origin, origin, debrisNorm);
		VectorNormalize(debrisNorm);
		FX_ThrowDebris(origin,debrisNorm, numchunks, scale, self->material,x_max,y_max,z_max, self->surfaceType);
	}

	if (self->message)
	{
		gi.centerprintf (attacker, "%s", self->message);
		gi.sound (attacker, CHAN_AUTO, gi.soundindex ("Misc/Talk.wav"), .6, ATTN_NORM, 0);
	}


	G_UseTargets(self,self);

	G_FreeEdict (self);
}

// specifically, spawn a cloud of papers and cans for certain objects before they get debrisified. could
//be written more generally for other objects' pre-death stuff
void SpecialBecomeDebris(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	int			i = 0;
	vec3_t		vDir = {0,0,1};

	// create a cloud of papers when this object is destroyed
	FX_PaperCloud(self->s.origin, 0);
	// ...and some coke cans
	if (attacker)
	{
		VectorSubtract(attacker->s.origin, self->s.origin, vDir);
	}
	else if (inflictor)
	{
		VectorSubtract(inflictor->s.origin, self->s.origin, vDir);
	}
	VectorNormalize(vDir);
	FX_SodaCans(self->s.origin, vDir, 100, 0);
	
	// ObjBecomeDebris() frees the edict so call it last
	ObjBecomeDebris(self, inflictor, attacker, damage, point);
}

void ObjBecomeDebris (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	byte			numchunks;
	byte			scale;

	if (self->objSpawnData)
	{
		numchunks = self->objSpawnData->debrisCnt;
		scale = self->objSpawnData->debrisScale;// * 10;
	}

	BecomeDebrisFinal(self,numchunks,scale,attacker);

}

void BecomeDebris (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	BecomeDebrisFinal(self,0,0,attacker);
}


void LightBecomeDebris (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	// Turn off light
//	gi.configstring (CS_LIGHTS+self->style, "a");	// FIXME - lights should go out when shot
	
	BecomeDebris (self,inflictor,attacker,damage,point);
}

void ObjectStopMove (edict_t *ent)
{
	ent->friction = 1;	// Don't move
}

void SimpleModelTouch (edict_t *ent, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t	movedir;

	if (!other->client) 
		return;

	// This should really be changed to reflect mass
	VectorSubtract(ent->s.origin, other->s.origin, movedir);
	VectorNormalize(movedir);
	VectorScale(movedir, 100, ent->velocity);
	ent->friction = 0;

	ent->think = ObjectStopMove;
	ent->nextthink = level.time + FRAMETIME;
}

void SimpleModelScale(edict_t *ent,float objscale)
{
	Matrix4 m,scale,tmp;

	if (!ent->ghoulInst)
		return;

	ent->ghoulInst->GetXForm(m);
	scale.Scale(objscale);
	tmp.Concat(scale,m);
	ent->ghoulInst->SetXForm(tmp);
}

void SimpleModelInit(edict_t *ent, solid_t solid)
{
	Matrix4 ToWorldNoOrigin;
	Vect3	mins,maxs;

	// make sure the edict_t::objInfo field is properly inited
	if (!ent->objInfo)
	{ // create a baseObjInfo_c for our edict
		baseObjInfo_c	*newInfo = new baseObjInfo_c(ent);
		ent->objInfo = newInfo;
	}
	// putting this in stops the game asserting later on during a server cliptrace if you call this
	//	function on a model that failed to load it's GHL file - slc.
	//
	if (solid == SOLID_GHOUL && !ent->ghoulInst)
	{
		gi.dprintf("ERROR: attempting to set SOLID_GHOUL without ghoulInst (model '%s')!\n", ent->classname);
		solid = SOLID_NOT;
	}

	// Set bounding box to match orientation of object
	BboxRotate(ent);

	ent->solid=solid;
	ent->clipmask = MASK_MONSTERSOLID|MASK_PLAYERSOLID; 

	// Lights are a little different
	if (strncmp (ent->classname, "light", 5)==0)
	{
		// Set the takedamage field to reflect INVINCIBLE setting
		if (ent->spawnflags & SF_LIGHT_VULNERABLE)
			ent->takedamage = DAMAGE_YES;
		else
			ent->takedamage = DAMAGE_NO;

		// Can be damaged but no death function?
		if ((ent->takedamage == DAMAGE_YES) && (!ent->die))
		{
			ent->die = LightBecomeDebris;
		}

		// Set the movetype
		if (!(ent->spawnflags & SF_LIGHT_PUSHABLE))
			ent->movetype = MOVETYPE_NONE;	// Can't be moved
		else
		{
			if (!ent->touch)
				ent->touch = SimpleModelTouch;

			ent->movetype = MOVETYPE_DAN;	// Can be pushed around
		}
	}
	else
	{
		// Set the takedamage field to reflect INVINCIBLE setting
		if (ent->spawnflags & SF_INVULNERABLE)
			ent->takedamage = DAMAGE_NO;
		else
			ent->takedamage = DAMAGE_YES;

		// Can be damaged but no death function?
		if ((ent->takedamage == DAMAGE_YES) && (!ent->die))
		{
			ent->die = BecomeDebris;
		}

		// Set the movetype
		if (ent->spawnflags & SF_NOPUSH)
			ent->movetype = MOVETYPE_NONE;	// Can't be moved
		else
		{
			if (!ent->touch)
				ent->touch = SimpleModelTouch;

			ent->movetype = MOVETYPE_DAN;	// Can be pushed around
		}

	}


	EntToWorldMatrix(ent->s.origin,ent->s.angles, ToWorldNoOrigin);
	ToWorldNoOrigin.SetRow(3,Vect3(0.0f,0.0f,0.0f));
	ToWorldNoOrigin.CalcFlags();

	if (solid == SOLID_GHOUL)
	{
		if (ent->ghoulInst)
		{
			ent->ghoulInst->GetBoundBox(level.time,ToWorldNoOrigin,mins,maxs);

			*(Vect3 *)ent->mins=mins;
			*(Vect3 *)ent->maxs=maxs;
		}
		else
		{
			gi.dprintf("ERROR: couldn't find ghoul object for %s!\n", ent->classname);
		}
	}

	TintModel(ent);

	gi.linkentity (ent);
}

void SimpleModelInit2(edict_t *ent, modelSpawnData_t *modelData,char *skinname,char *partname)
{
	Matrix4 ToWorldNoOrigin;
	Vect3	mins,maxs;
	float	volume = (ent->maxs[0] - ent->mins[0]) * (ent->maxs[1] - ent->mins[1]) * (ent->maxs[2] - ent->mins[2]);

	if (NULL == modelData)
	{
		return;
	}
	// make sure the edict_t::objInfo field is properly inited
	if (!ent->objInfo)
	{ // create a baseObjInfo_c for our edict
		baseObjInfo_c	*newInfo = new baseObjInfo_c(ent);
		ent->objInfo = newInfo;
	}
	game_ghoul.SetSimpleGhoulModel (ent,modelData->dir,modelData->file,skinname,modelData->materialfile);

	// Set skin, if there is one
	if (skinname)
		SetSkin(ent, modelData->dir,modelData->file, modelData->materialfile, skinname, 0);

	// Set partname, if there is one
	if (partname)
		SimpleModelSetObject(ent,partname);

	if (!ent->surfaceType)
		ent->surfaceType = modelData->surfaceType;
	if (!ent->material)
		ent->material = modelData->material;
	if (!ent->health)
		ent->health = modelData->health;
	if (!ent->solid)
		ent->solid = modelData->solid;

	// putting this in stops the game asserting later on during a server cliptrace if you call this
	//	function on a model that failed to load it's GHL file - slc.
	//
	if (ent->solid == SOLID_GHOUL && !ent->ghoulInst)
	{
		gi.dprintf("ERROR: attempting to set SOLID_GHOUL without ghoulInst (model '%s')!\n", ent->classname);
		ent->solid = SOLID_NOT;
	}

	// Set bounding box to match orientation of object
	BboxRotate(ent);

	ent->clipmask = MASK_MONSTERSOLID|MASK_PLAYERSOLID; 

	ent->objSpawnData = (modelData);


	if (st.scale)
		SimpleModelScale(ent,st.scale);

	// Lights are a little different
	if (strncmp (ent->classname, "light", 5)==0)
	{
		// Set the takedamage field to reflect INVINCIBLE setting
		if (ent->spawnflags & SF_LIGHT_VULNERABLE)
			ent->takedamage = DAMAGE_YES;
		else
			ent->takedamage = DAMAGE_NO;

		// Can be damaged but no death function?
		if ((ent->takedamage == DAMAGE_YES) && (!ent->die))
		{
			ent->die = LightBecomeDebris;
		}

		// Set the movetype
		if (!(ent->spawnflags & SF_LIGHT_PUSHABLE))
			ent->movetype = MOVETYPE_NONE;	// Can't be moved
		else
		{
			if (!ent->touch)
				ent->touch = SimpleModelTouch;

			ent->movetype = MOVETYPE_DAN;	// Can be pushed around
		}
	}
	else
	{
		// Set the takedamage field to reflect INVINCIBLE setting
		if (ent->spawnflags & SF_INVULNERABLE)
			ent->takedamage = DAMAGE_NO;
		else
			ent->takedamage = DAMAGE_YES;

		// Can be damaged but no death function?
		if ((ent->takedamage == DAMAGE_YES) && (!ent->die))
		{
			ent->die = ObjBecomeDebris;
		}

		// Set the movetype
		if (ent->spawnflags & SF_NOPUSH)
			ent->movetype = MOVETYPE_NONE;	// Can't be moved
		else
		{
			if (!ent->touch)
				ent->touch = SimpleModelTouch;

			ent->movetype = MOVETYPE_DAN;	// Can be pushed around
		}

	}


	EntToWorldMatrix(ent->s.origin,ent->s.angles, ToWorldNoOrigin);
	ToWorldNoOrigin.SetRow(3,Vect3(0.0f,0.0f,0.0f));
	ToWorldNoOrigin.CalcFlags();

	if (ent->solid == SOLID_GHOUL)
	{
		if (ent->ghoulInst)
		{
			ent->ghoulInst->GetBoundBox(level.time,ToWorldNoOrigin,mins,maxs);

			*(Vect3 *)ent->mins=mins;
			*(Vect3 *)ent->maxs=maxs;
		}
		else
		{
			gi.dprintf("ERROR: couldn't find ghoul object for %s!\n", ent->classname);
		}
	}

	TintModel(ent);

	gi.linkentity (ent);

	ent->mass = 800;
}

void SimpleModelSetObject(edict_t *ent,char *partname)
{
	GhoulID part = NULL_GhoulID;
	gg_obj_c_ptr MyGhoulObj;

	if (!ent->ghoulInst)
	{
		return;
	}

	MyGhoulObj=game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());

	if (MyGhoulObj)
	{
		if (MyGhoulObj->GetMyObject())
		{
			part=MyGhoulObj->GetMyObject()->FindPart(partname);
		}
		if (ent->ghoulInst && part != NULL_GhoulID)
		{
			ent->ghoulInst->SetAllPartsOnOff(false);
			ent->ghoulInst->SetPartOnOff(part, true);
		}
	}
}

void SimpleModelRemoveObject(edict_t *ent,char *partname)
{
	GhoulID part = NULL_GhoulID;
	gg_obj_c_ptr MyGhoulObj;

	if (!ent->ghoulInst)
	{
		return;
	}

	MyGhoulObj=game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());

	if (MyGhoulObj && MyGhoulObj->GetMyObject())
	{
		part=MyGhoulObj->GetMyObject()->FindPart(partname);
	}
	if (ent->ghoulInst && part != NULL_GhoulID)
	{
		ent->ghoulInst->SetPartOnOff(part, false);
	}
	else
	{
		gi.dprintf("ERROR: SimpleModelRemoveObject - can find part %s!\n", partname);
	}
}

void SimpleModelRemoveObject2(IGhoulInst* inst,char *partname)
{
	GhoulID part = NULL_GhoulID;
	gg_obj_c_ptr MyGhoulObj;

	if (!inst)
	{
		return;
	}

	MyGhoulObj=game_ghoul.FindObject(inst->GetGhoulObject());

	if (MyGhoulObj && MyGhoulObj->GetMyObject())
	{
		part=MyGhoulObj->GetMyObject()->FindPart(partname);
	}
	if (inst && part != NULL_GhoulID)
	{
		inst->SetPartOnOff(part, false);
	}
	else
	{
		gi.dprintf("ERROR: SimpleModelRemoveObject - can find part %s!\n", partname);
	}
}

void RecursiveTurnOff(IGhoulInst* inst)
{
    int		i,num=inst->GetNumChildren();
	GhoulID junkID;

	if (!inst)
	{
		return;
	}
    inst->SetAllPartsOnOff(false);
    for (i=0;i<num;i++)
	{
        RecursiveTurnOff(inst->GetChild(i,junkID));
	}
}

void SimpleModelTurnOnOff(IGhoulInst* inst, bool bOnOff)
{
	if (inst)
	{
		inst->SetAllPartsOnOff(bOnOff);
	}
}

void SimpleModelAddObject2(IGhoulInst* inst,char *partname)
{
	GhoulID part = NULL_GhoulID;
	gg_obj_c_ptr MyGhoulObj;

	if (!inst)
	{
		return;
	}

	MyGhoulObj=game_ghoul.FindObject(inst->GetGhoulObject());

	if (MyGhoulObj && MyGhoulObj->GetMyObject())
	{
		part=MyGhoulObj->GetMyObject()->FindPart(partname);
	}
	if (inst && part != NULL_GhoulID)
	{
		inst->SetPartOnOff(part, true);
	}
}

void SimpleModelAddObject(edict_t *ent,char *partname)
{
	GhoulID part = NULL_GhoulID;
	gg_obj_c_ptr MyGhoulObj;

	if (!ent->ghoulInst)
	{
		return;
	}

	MyGhoulObj=game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());

	if (MyGhoulObj && MyGhoulObj->GetMyObject())
	{
		part=MyGhoulObj->GetMyObject()->FindPart(partname);
	}
	if (ent->ghoulInst && part != NULL_GhoulID)
	{
		ent->ghoulInst->SetPartOnOff(part, true);
	}
}

void SimpleModelSetSequence(edict_t *ent,char *seqname,int loopFlag)
{
	ggObjC *object=NULL;
	GhoulID objectSeq=0;

	if (!ent->ghoulInst)
	{
		return;
	}

	object = game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());

	//changed this to false, can't cache new seqs in after instances are created --ss
	objectSeq = game_ghoul.FindObjectSequence(object,seqname);

	if (ent && ent->ghoulInst && objectSeq)
	{
		if (!loopFlag)
			ent->ghoulInst->Play(objectSeq,level.time,0.0f,true,IGhoulInst::Hold);
		else
			ent->ghoulInst->Play(objectSeq,level.time,0.0f,true,IGhoulInst::Loop);
	}
}

GhoulID SimpleModelSetSequence2(IGhoulInst* inst,char *seqname,int loopFlag)
{
	ggObjC *object=0;
	GhoulID objectSeq=0;

	if (!inst)
	{
		return 0;
	}

	object = game_ghoul.FindObject(inst->GetGhoulObject());

	//changed this to false, can't cache new seqs in after instances are created --ss
	objectSeq = game_ghoul.FindObjectSequence(object,seqname);

	if (inst && objectSeq)
	{
		if (0 == loopFlag)
		{
			inst->Play(objectSeq,level.time,0.0f,true,IGhoulInst::Hold);
		}
		else if (1 == loopFlag)
		{
			inst->Play(objectSeq,level.time,0.0f,true,IGhoulInst::Loop);
		}
		else if (2 == loopFlag)
		{
			inst->Play(objectSeq,level.time,0.0f,true,IGhoulInst::HoldFrame);
		}
		return objectSeq;
	}
	return 0;
}


void SimpleModelSetOriginRelative(edict_t *original,edict_t *dupe,float addforward,float addright,float addup)
{
	vec3_t	holdangles,forward,right;

	VectorCopy(original->s.origin,dupe->s.origin);
	VectorCopy(original->s.angles,dupe->s.angles);
	dupe->s.origin[2] += addup;

	VectorCopy(original->s.angles, holdangles);
	holdangles[1] = original->s.angle_diff; 

	AngleVectors(holdangles, forward, right, NULL);
	VectorMA(dupe->s.origin, addforward, forward, dupe->s.origin);
	VectorMA(dupe->s.origin, addright, right, dupe->s.origin);

}

// Boltee is the object being added on
// Bolter is the object the Boltee is being added on to
ggBinstC	*SimpleModelAddBolt2(IGhoulInst* pBolterInst,modelSpawnData_t &bolterModelData,char*bolterBoltName,
						modelSpawnData_t &bolteeModelData,char *bolteeBoltName,char *skinName)
{
	ggObjC		*bolterObj = NULL;
	ggObjC		*bolteeObj = NULL;
	GhoulID		bolterBolt = NULL_GhoulID;
	GhoulID		bolteeBolt = NULL_GhoulID;
	GhoulID		bolteeSeq = NULL_GhoulID;
	ggBinstC	*cBolteeBolted = NULL;
	ggOinstC	*bolterOInstance = NULL;	
	IGhoulObj*	pBolterObj = NULL, *pBolteeObj = NULL;
	char* matFile = NULL;

	// is there a material file for the object getting bolted on
	if (bolteeModelData.materialfile)
	{
		matFile = bolteeModelData.materialfile;
	}
	else if (bolterModelData.materialfile) // how bout for the base object, then?
	{
		matFile = bolterModelData.materialfile;
	}

	bolterObj = game_ghoul.FindObject(pBolterInst->GetGhoulObject());

	//if a skinname is specified, only register that skin; otherwise register everything
	if (skinName&&skinName[0])
	{
		bolteeObj = game_ghoul.FindObject(bolteeModelData.dir,bolteeModelData.file, false, skinName);//make sure i have unique object for this skin
		if (bolteeObj)
		{
			bolteeObj->RegisterSkin(matFile, skinName);//actually register the skin.
		}
	}
	else
	{
		bolteeObj = game_ghoul.FindObject(bolteeModelData.dir,bolteeModelData.file);//registers all skins
	}

	if (bolteeObj)
	{
		bolteeObj->RegistrationLock();
	}

	if (bolterObj && bolteeObj) 
	{
		bolterOInstance = bolterObj->FindOInst(pBolterInst);
		pBolterObj = bolterObj->GetMyObject();
		pBolteeObj = bolteeObj->GetMyObject();

		if (bolterOInstance && pBolterObj && pBolteeObj)
		{
			bolterBolt = pBolterObj->FindPart(bolterBoltName);

			bolteeSeq = game_ghoul.FindObjectSequence(bolteeObj,bolteeModelData.file);

			bolteeBolt = pBolteeObj->FindPart(bolteeBoltName);

			if (bolterBolt && bolteeBolt && bolteeSeq)
			{
				cBolteeBolted = bolterOInstance->AddBoltInstance(bolterBolt, bolteeObj, bolteeBolt);
				if (cBolteeBolted)
				{
					if (matFile && skinName)
					{
						cBolteeBolted->GetInstPtr()->SetFrameOverride(matFile,skinName);
					}

					cBolteeBolted->PlaySequence(bolteeSeq,level.time,0.0,true,
						IGhoulInst::HoldFrame,false,false);
				}
			}
			else
			{
				if (!bolterBolt)
					gi.dprintf("ERROR: SimpleModelAddBolt - can't find bolt %s!\n", bolterBoltName);

				if (!bolteeBolt)
					gi.dprintf("ERROR: SimpleModelAddBolt - can't find bolt %s!\n", bolteeBoltName);
			}
		}
	}		
	else
	{
		if (!bolterObj)
			gi.dprintf("ERROR: SimpleModelAddBolt - can't find object %s!\n", bolterModelData.file);

		if (!bolteeObj)
			gi.dprintf("ERROR: SimpleModelAddBolt - can't find object %s!\n", bolteeModelData.file);

	}

	return(cBolteeBolted);
}

//
// SimpleModelAddBolt
//
// -bolting an object to an entity
// -Boltee is the object being added on
// -Bolter is the object the Boltee is being added on to
ggBinstC	*SimpleModelAddBolt(edict_t *ent,modelSpawnData_t &bolterModelData,char*bolterBoltName,
						modelSpawnData_t &bolteeModelData,char *bolteeBoltName,char *skinName)
{
	ggBinstC		*newBinst = NULL;
	
	if (!ent || !ent->ghoulInst)
	{
		return NULL;
	}

	if (ent->objInfo && (newBinst = SimpleModelAddBolt2(ent->ghoulInst, bolterModelData, bolterBoltName,
						bolteeModelData, bolteeBoltName, skinName)) )
	{
		ent->objInfo->AddBolt(newBinst);
	}
	return newBinst;
}

//
// ComplexModelAddBolt
//
// -bolting an object to an existing bolted-on object
// -Boltee is the object being added on
// -Bolter is the object the Boltee is being added on to
ggBinstC	*ComplexModelAddBolt(ggBinstC* bInst,modelSpawnData_t &bolterModelData,char*bolterBoltName,
						modelSpawnData_t &bolteeModelData,char *bolteeBoltName,char *skinName)
{
	ggObjC		*bolterObj = NULL;
	ggObjC		*bolteeObj = NULL;
	GhoulID		bolterBolt;
	GhoulID		bolteeBolt;
	GhoulID		bolteeSeq;
	ggBinstC	*cBolteeBolted = NULL;
	IGhoulObj*	pBolterObj = NULL, *pBolteeObj = NULL;
	edict_t		*root = NULL;
	char* matFile = NULL;

	// is there a material file for the object getting bolted on
	if (bolteeModelData.materialfile)
	{
		matFile = bolteeModelData.materialfile;
	}
	else if (bolterModelData.materialfile) // how bout for the base object, then?
	{
		matFile = bolterModelData.materialfile;
	}

	if (bInst && bInst->GetInstPtr())
	{
		bolterObj = game_ghoul.FindObject(bInst->GetInstPtr()->GetGhoulObject());
	}

	//if a skinname is specified, only register that skin; otherwise register everything
	if (skinName&&skinName[0])
	{
		bolteeObj = game_ghoul.FindObject(bolteeModelData.dir,bolteeModelData.file, false, skinName);//make sure i have unique object for this skin
		if (bolteeObj)
		{
			bolteeObj->RegisterSkin(matFile, skinName);//actually register the skin.
		}
	}
	else
	{
		bolteeObj = game_ghoul.FindObject(bolteeModelData.dir,bolteeModelData.file);//registers all skins
	}

	if (bolteeObj)
	{
		bolteeObj->RegistrationLock();
	}

	if (bolterObj && bolteeObj) 
	{
		pBolterObj = bolterObj->GetMyObject();
		pBolteeObj = bolteeObj->GetMyObject();

		if (pBolterObj && pBolteeObj)
		{
			bolterBolt = pBolterObj->FindPart(bolterBoltName);

			bolteeSeq = game_ghoul.FindObjectSequence(bolteeObj,bolteeModelData.file);

			bolteeBolt = pBolteeObj->FindPart(bolteeBoltName);

			if (bolterBolt && bolteeBolt && bolteeSeq)
			{
				cBolteeBolted = bInst->AddBoltInstance(bolterBolt, bolteeObj, bolteeBolt);
				if (cBolteeBolted)
				{
					if (matFile && skinName)
					{
						cBolteeBolted->GetInstPtr()->SetFrameOverride(matFile,skinName);
					}

					cBolteeBolted->PlaySequence(bolteeSeq,level.time,0.0,true,
						IGhoulInst::HoldFrame,false,false);

					// add this bolton to our root entity's list of boltons
					if ( (root = cBolteeBolted->GetEdict()) && (root->objInfo) )
					{
						root->objInfo->AddBolt(cBolteeBolted);
					}
					else
					{
						gi.dprintf("ERROR: ComplexModelAddBolt - can't find root edict for %s!\n", bolterBoltName);
					}
				}
			}
			else
			{
				if (!bolterBolt)
					gi.dprintf("ERROR: ComplexModelAddBolt - can't find bolt %s!\n", bolterBoltName);

				if (!bolteeBolt)
					gi.dprintf("ERROR: SimpleModelAddBolt - can't find bolt %s!\n", bolteeBoltName);
			}
		}
	}		
	else
	{
		if (!bolterObj)
			gi.dprintf("ERROR: ComplexModelAddBolt - can't find object %s!\n", bolterModelData.file);

		if (!bolteeObj)
			gi.dprintf("ERROR: ComplexModelAddBolt - can't find object %s!\n", bolteeModelData.file);

	}

	return(cBolteeBolted);
}

ggBinstC *SimpleModelFindBolt(edict_t *ent, modelSpawnData_t bolterModelData, char*bolterBoltName)
{
	if (!ent->ghoulInst)
	{
		return NULL;
	}

	ggObjC*		bolterObj = game_ghoul.FindObject(ent->ghoulInst->GetGhoulObject());
	GhoulID		bolterBolt = 0;
	ggOinstC*	bolterOInst = NULL;
	ggBinstC*	boltee = NULL;
	IGhoulObj*	pBolterObj = NULL;

	if (bolterObj && (pBolterObj = bolterObj->GetMyObject()) )
	{
		bolterBolt = pBolterObj->FindPart(bolterBoltName);
		if (bolterBolt)
		{
			bolterOInst = bolterObj->FindOInst(ent->ghoulInst);
			if (bolterOInst)
			{
				boltee = bolterOInst->GetBoltInstance(bolterBolt);
			}
			else
			{
				gi.dprintf("ERROR: SimpleModelFindBolt - can't find instance of %s!\n", bolterModelData.file);
			}
		}
		else
		{
			gi.dprintf("ERROR: SimpleModelFindBolt - can't find bolt %s!\n", bolterBoltName);
		}
	}
	else
	{
		gi.dprintf("ERROR: SimpleModelFindBolt - can't find object %s!\n", bolterModelData.file);
	}

	return boltee;
}

IGhoulInst*	SimpleModelGetBolt(edict_t *ent, int nBolt)
{
	baseObjInfo_c	*objInfo = NULL;

	if (objInfo = ent->objInfo)
	{
		return objInfo->GetBolt(nBolt);
	}
	return NULL;
}

bool WithinFOV(edict_t *source, vec3_t target, float halfFOV)
{
	// FIXME
	// note: this function essentially duplicated in g_monster.cpp infront.  optimize at some point?
	// EXCEPT! we are ignoring the z component!
	vec3_t		forward, vec;
	float		threshold, dot;

	AngleVectors (source->s.angles, forward, NULL, NULL);

	VectorSubtract (target, source->s.origin, vec);
	forward[2] = 0;
	VectorNormalize (forward);
	vec[2] = 0;
	VectorNormalize (vec);
	dot = DotProduct (vec, forward);

	threshold = cos (halfFOV * DEGTORAD);

	Com_Printf("Threshold %f  Dot %f\n", threshold, dot);

	if (dot < threshold)
	{
		return false;
	}

	return true;
}

edict_t	*SV_TestEntityPosition (edict_t *ent);

void GrabStuffOnTop (edict_t *self)
{
	edict_t		*check, *block;
	int			e;
	vec3_t		move;
	
	
	check = g_edicts+1;
	for (e = 1; e < globals.num_edicts; e++, check++)
	{
		if (!check->inuse)
			continue;
		if (check->movetype == MOVETYPE_PUSH
		|| check->movetype == MOVETYPE_STOP
		|| check->movetype == MOVETYPE_NONE
		|| check->movetype == MOVETYPE_NOCLIP)
			continue;

		if (!check->area.prev)
			continue;		// not linked in anywhere
		if (check->groundentity != self)
			continue;
	// something is on top of me, add my velocity to it
		VectorScale(self->velocity, .1, move);
		VectorAdd(check->s.origin, move, check->s.origin);

		block = SV_TestEntityPosition (check);
		if (!block)
		{	// pushed ok
			gi.linkentity (check);
			// impact?
			continue;
		}

		// if it is ok to leave in the old position, do it
		// this is only relevent for riding entities, not pushed
		// FIXME: this doesn't acount for rotation
		VectorSubtract (check->s.origin, move, check->s.origin);
	}
}



// make object flip around when hit
void Obj_painflip (edict_t *ent,edict_t *other,int damage)
{
	vec3_t falldir;
	float fVecScale = damage;
	float f;

	VectorSubtract(ent->s.origin, other->s.origin, falldir);
	VectorNormalize(falldir);
	VectorScale(falldir, f = gi.flrand(4.0,10.0) * fVecScale, ent->velocity);
	ent->velocity[2] = gi.flrand(4.0,6.0) * fVecScale;	// Just a little push up

	ent->avelocity[1] = gi.flrand(4.0,6.0) * fVecScale;
}

// make object roll around when hit
void Obj_painroll (edict_t *ent,edict_t *other,int damage,int axis)
{
	vec3_t falldir;

	VectorSubtract(ent->s.origin, other->s.origin, falldir);
	VectorNormalize(falldir);
	VectorScale(falldir, gi.flrand(10.0,15.0) * damage, ent->velocity);
	ent->avelocity[axis] = falldir[axis] * 200;
}





void Obj_tipoverthink (edict_t *self)
{
	if (self->health <= 0)	
	{
		return;
	}

	if ((self->s.angles[0] >= 90) || (self->s.angles[0] <= -90) ||
		(self->s.angles[2] >= 90) || (self->s.angles[2] <= -90))
	{
		if (self->s.angles[0] >= 90)
		{
			self->s.angles[0] = 90;
		}
		if (self->s.angles[0] <= -90)
		{
			self->s.angles[0] = -90;
		}
		if (self->s.angles[2] >= 90)
		{
			self->s.angles[2] = 90;
		}
		if (self->s.angles[2] <= -90)
		{
			self->s.angles[2] = -90;
		}
		VectorClear(self->avelocity);
		if (self->health < 25)
		{
			self->nextthink = level.time + .3;
		}
		else
		{
			self->nextthink = 0;
		}
		return;
	}
	self->avelocity[0] *= 1.6;
	self->avelocity[2] *= 1.6;
	self->nextthink = level.time + .1;
}

void Obj_tipover (edict_t *ent, edict_t *other, int damage)
{
	vec3_t	falldir;
	vec3_t	prelimavel;
	float	angle;

	VectorSubtract(ent->s.origin, other->s.origin, falldir);
	VectorNormalize(falldir);
	angle = 360 - ent->s.angle_diff;
	angle = NormalizeAngle(angle);
	angle *= DEGTORAD;
	prelimavel[0] = falldir[0] * -70;
	prelimavel[2] = falldir[1] * 70;
	ent->avelocity[0] = -1*(prelimavel[0] * cos(angle) + prelimavel[2] * sin(angle));
	ent->avelocity[2] = -1*(prelimavel[2] * cos(angle) + prelimavel[0] * -1 * sin(angle));
	VectorScale(falldir, damage*6, ent->velocity);
	ent->velocity[2] = 150;
	ent->elasticity = .7;

	ent->think = Obj_tipoverthink;
	ent->nextthink = level.time + .1;
	ent->elasticity = .7;

}

void Obj_explode(edict_t *ent,vec3_t *pos,int radiusBurn,int blindingLight)
{
	// Always do the damage BEFORE burning, 'cause those that aren't dead won't burn.
	T_RadiusDamage (ent, ent, 100, ent, 100, 0);
	if (radiusBurn)
		RadiusBurn(ent, 100);

	gmonster.RadiusDeafen(ent, 200, 200);

	fxRunner.exec("weapons/world/airexplode", ent->s.origin);
	FX_C4Explosion(ent);

	if(blindingLight)
		BlindingLight(*pos, ent->health*10, 0.9, 0.5);

	ShakeCameras (ent->s.origin, 100, 100*2, DEFAULT_JITTER_DELTA);
}


void Obj_partbreaksetup (int bpd_enum,modelSpawnData_t *modelData,
						 objParts_t *objBoltPartsData,objBreak_t *objBreak)
{
	int indextotal,i;

	indextotal = 0;

	// Find proper part index
	for (i=0;i< bpd_enum;++i)
	{
		indextotal += objBreak[i].partCnt;
	}

	// Point to first part of object in array
	modelData->objBreakData	= &objBreak[bpd_enum];
	modelData->objBreakData->boltPartData = &objBoltPartsData[indextotal];
}

void Obj_DefaultPain(edict_t *self,int partLost, vec3_t boltPos)
{
	vec3_t	debrisNorm;

	// Throw debris and make it go away
	VectorClear(debrisNorm);
	FX_ThrowDebris(boltPos,debrisNorm, 5, DEBRIS_SM, self->material, 0,0,0, self->surfaceType);
	FX_SmokePuff(boltPos,120,120,120,200);
}

static void FlingNearbyObjects(edict_t *self)
{
	objInfo_c	*infoPtr = self->objInfo->GetInfo(OIT_BOLTINST);
	edict_t		*flingee = NULL;

	if (NULL == infoPtr)
	{	//	nowhere to store information
		return;
	}
	if (infoPtr->head == self)
	{	// ooh, I've got an idea. let's _not_ go into an infinite loop.
		return;
	}
	while (flingee = infoPtr->head)
	{
		// flingee is an edict that we need to fling away from self
		VectorSubtract(flingee->s.origin, self->s.origin, flingee->velocity);
		VectorNormalize(flingee->velocity);
		VectorScale(flingee->velocity, gi.flrand(75, 125), flingee->velocity);
		flingee->s.angles[YAW] += gi.flrand(-20, 20);
		/*
		AngleVectors(self->s.angles, flingee->velocity, right, NULL);
		VectorScale(flingee->velocity, gi.flrand(-150, 150), flingee->velocity);
		VectorMA(flingee->velocity, gi.flrand(-150, 150), right, flingee->velocity);
		*/
		flingee->velocity[2] += 50;
		flingee->s.origin[2] += 15;
		// find the next node in the list
		if (NULL == flingee->objInfo)
		{	// next node doesn't have an objInfo
			infoPtr->head = NULL;
			return;
		}
		// now that the flingee is no longer touching self, remove it from the list
		infoPtr->head = NULL;
		infoPtr = flingee->objInfo->GetInfo(OIT_BOLTINST);
	}
}

void Obj_partpain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit)
{

	Matrix4			ToWorldNoOrigin;
	Vect3			mins,maxs;
	vec3_t			boltPos;
	ggOinstC*		myInstance;
	ggObjC			*MyGhoulObj;
	GhoulID			bolterBolt;
	Matrix4			BoltToEntity;
	Matrix4			BoltToWorld;
	IGhoulInst		*BoltInst = NULL;
	int				i;
	objParts_t		*boltPartData;
	objParts_t		*holdboltPartData;
	boltInstInfo_c	*info = NULL;

	// Some checks
	if (!self->objSpawnData)
		return;
	if (!self->objSpawnData->objBreakData)
		return;
	if (!self->objSpawnData->objBreakData->boltPartData)
		return;
	if ( !self->objInfo || !(info = (boltInstInfo_c*)self->objInfo->GetInfo(OIT_BOLTINST)) )
	{
		return;
	}

	boltPartData = self->objSpawnData->objBreakData->boltPartData;

	MyGhoulObj=game_ghoul.FindObject(self->ghoulInst->GetGhoulObject());
	myInstance = MyGhoulObj->FindOInst(self->ghoulInst);

	EntToWorldMatrix(self->s.origin,self->s.angles, ToWorldNoOrigin);
	ToWorldNoOrigin.SetRow(3,Vect3(0.0f,0.0f,0.0f));
	ToWorldNoOrigin.CalcFlags();

	holdboltPartData = self->objSpawnData->objBreakData->boltPartData;

	// Loop through all parts bolted to this object
	for(i=0;i<self->objSpawnData->objBreakData->partCnt;++i,++boltPartData,++holdboltPartData)
	{
		BoltInst = SimpleModelGetBolt(self, holdboltPartData->partnum);

		if (!BoltInst || !info->IsOn(holdboltPartData->partnum))	// This part is already gone
			continue;

		bolterBolt = MyGhoulObj->GetMyObject()->FindPart(boltPartData->partnull);

		self->ghoulInst->GetBoltMatrix(level.time,BoltToEntity,bolterBolt,
			IGhoulInst::MatrixType::Entity);

		BoltToEntity.SetRow(3,Vect3(0.0f,0.0f,0.0f));
		BoltToEntity.CalcFlags();
		BoltToWorld.Concat(BoltToEntity,ToWorldNoOrigin);

		// Get part bounding box
		BoltInst->GetBoundBox(level.time,BoltToWorld,mins,maxs);

		GetGhoulPosDir(self->s.origin, self->s.angles, self->ghoulInst,
						   NULL, boltPartData->partnull, boltPos, NULL, NULL, NULL);

		// Get bolted part's bounding box (extend it by 2 pixels for any minor discrepancies)
		Vec3AddAssign(boltPos,*(vec3_t*)(&mins));
		mins[0] -= 2;
		mins[1] -= 2;
		mins[2] -= 2;

		Vec3AddAssign(boltPos,*(vec3_t*)(&maxs));
		maxs[0] += 2;
		maxs[1] += 2;
		maxs[2] += 2;

		// Is it a hit
		if ((wherehit[2] >= mins[2]) && (wherehit[2] <= maxs[2]))
		{

			if ((wherehit[1] >= mins[1]) && (wherehit[1] <= maxs[1]))
			{

				if ((wherehit[0] >= mins[0]) && (wherehit[0] <= maxs[0]))
				{
					//myInstance->RemoveBoltInstance(BoltInst);
					SimpleModelTurnOnOff(BoltInst, false);
					info->TurnOff(holdboltPartData->partnum);

					// Does this part have a pain function???
					if (boltPartData->pain)
					{
						boltPartData->pain(self,boltPartData->partnum, boltPos);
					}
					else
					{
						Obj_DefaultPain(self, boltPartData->partnum, boltPos);
					}
					
					break;	// Found a piece to blow up, no need to look further
				}
			}
		}	
	}
	// if we have any objects stored off of our head ptr, that hopefully means they 
	//were put there because we need to move them when this edict gets hurt. like, say,
	//if there are things sitting on a table
	FlingNearbyObjects(self);

}




void Obj_partkill (edict_t *self,int boltNum,int debrisFlag)
{

	Matrix4			ToWorldNoOrigin;
	Vect3			mins,maxs;
	vec3_t			boltPos;
	ggOinstC*		myInstance = NULL;
	ggObjC			*MyGhoulObj = NULL;
	Matrix4			BoltToEntity;
	Matrix4			BoltToWorld;
	objParts_t		*boltPartData;
	IGhoulInst		*BoltInst = NULL;
	boltInstInfo_c	*info = NULL;
	vec3_t			debrisNorm;

	if ( !self->objInfo || !(info = (boltInstInfo_c*)self->objInfo->GetInfo(OIT_BOLTINST)) )
	{
		return;
	}

	if ( !(BoltInst = SimpleModelGetBolt(self, boltNum)) )
	{
		return;
	}

	// Some checks
	if (!self->objSpawnData)
		return;
	if (!self->objSpawnData->objBreakData)
		return;
	if (!self->objSpawnData->objBreakData->boltPartData)
		return;
	if (!self->ghoulInst)
	{
		return;
	}

	boltPartData = self->objSpawnData->objBreakData->boltPartData;

	MyGhoulObj=game_ghoul.FindObject(self->ghoulInst->GetGhoulObject());
	if (MyGhoulObj)
	{
		myInstance = MyGhoulObj->FindOInst(self->ghoulInst);
	}

	GetGhoulPosDir(self->s.origin, self->s.angles, BoltInst,
					   NULL, boltPartData->partnull, boltPos, NULL, NULL, NULL);

	// Throw debris and make it go away
	if (debrisFlag)
	{
		VectorClear(debrisNorm);
		FX_ThrowDebris(boltPos,debrisNorm, 5, DEBRIS_SM, self->material, 0,0,0, self->surfaceType);
		FX_SmokePuff(boltPos,120,120,120,200);
	}

	if (myInstance)
	{
		//myInstance->RemoveBoltInstance(BoltInst);
		SimpleModelTurnOnOff(BoltInst, false);
		info->TurnOff(boltNum);
	}
}

boltonOrientation_c::boltonOrientation_c()
{
	root = NULL;			
	boltonInst = NULL;	
	boltonID = 0;		
	parentInst = NULL;
	parentID = 0;
	VectorClear(vTarget);		
	fMinPitch = 0;		
	fMaxPitch = 0;		
	fMinYaw = 0;		
	fMaxYaw = 0;		
	fMaxTurnSpeed = 0;	
	fRetPitch = 0;		
	fRetYaw = 0;		
	bUsePitch = false;		
	bUseYaw = false;		
	bToRoot = true;	
}

// OrientBolton will pitch and/or yaw a bolton of a ghoulInst, limiting the rotation
//speed and bolton's orientation with respect to its parent
boltonOrientation_c::retCode_e boltonOrientation_c::OrientBolton()
{
	retCode_e	bRet = ret_TRUE;

	if (!root || !root->ghoulInst || (!bUseYaw && !bUsePitch) || !parentInst)
	{
		return ret_ERROR;
	}

	Matrix4		EntityToWorld, BoltToEntity, BoltToWorld, WorldToBolt,
				ParentToEntity, ParentToWorld, WorldToParent;
	Matrix4		matOld, matR1, matR2, matTemp, matNew;
	float		angle1 = 0, angle2 = 0;
	float		fOldAngle1 = 0, fOldAngle2 = 0, parentAng1 = 0, parentAng2 = 0,
				boltToParentAng1 = 0, boltToParentAng2 = 0;
	float		fMin1 = fMinYaw, fMax1 = fMaxYaw, fMin2 = fMinPitch, fMax2 = fMaxPitch;
	float		fDelta1 = 0.0f, fDelta2 = 0.0f;
	Vect3		vTemp, vBoltPos, vParentPos, TargetPos, TargetPosInBoltSpace, TargetPosInParentSpace;
	float		XZ = 0, XY = 0;
	IGhoulInst* parent = boltonInst->GetParent();

	EntToWorldMatrix(root->s.origin,root->s.angles,EntityToWorld);
	boltonInst->GetBoltMatrix(level.time,BoltToEntity,boltonID,
		IGhoulInst::MatrixType::Entity, bToRoot);
	BoltToWorld.Concat(BoltToEntity,EntityToWorld);
	WorldToBolt.Inverse(BoltToWorld);
	TargetPos = *(Vect3*)vTarget;
	GetAngles(TargetPos, WorldToBolt, angle1, angle2);

	// need to know the bolt's parent's relationship to the world so we can
	//properly limit the range of motion of the bolt
	if (parent != root->ghoulInst)
	{
		parentInst->GetBoltMatrix(level.time, ParentToEntity, parentID,
			IGhoulInst::MatrixType::Entity, true);
		ParentToWorld.Concat(ParentToEntity, EntityToWorld);
	}
	else
	{
		ParentToWorld = EntityToWorld;
	}
	// need to know the bolt's relationship to its parent so we can properly
	//limit the range of motion
	BoltToWorld.GetRow(0, vTemp);
	BoltToWorld.GetRow(3, vBoltPos);
	WorldToParent.Inverse(ParentToWorld);
	// to prevent pitch weirdness account for disparity in height of bolt origin and parent origin
	ParentToWorld.GetRow(3, vParentPos);
	vBoltPos.Set(vBoltPos.x(), vBoltPos.y(), vParentPos.z());
	vTemp *= 100; // make sure minor floating point issues don't spooj our calculations
	vTemp += vBoltPos;
	GetAngles(vTemp, WorldToParent, boltToParentAng1, boltToParentAng2);
	//limit the angles here with respect to the parent

	GetAngles(TargetPos, WorldToParent, parentAng1, parentAng2);

	// ensure the traversal is in the shortest direction
	if (parentAng1 > M_PI)
	{
		parentAng1 = -(2*M_PI - parentAng1);
	}
	else if (parentAng1 < -M_PI)
	{
		parentAng1 = 2*M_PI + parentAng1;
	}
	if (parentAng2 > M_PI)
	{
		parentAng2 = -(2*M_PI - parentAng2);
	}
	else if (parentAng2 < -M_PI)
	{
		parentAng2 = 2*M_PI + parentAng2;
	}
	if (boltToParentAng1 > M_PI)
	{
		boltToParentAng1 = -(2*M_PI - boltToParentAng1);
	}
	else if (boltToParentAng1 < -M_PI)
	{
		boltToParentAng1 = 2*M_PI + boltToParentAng1;
	}
	if (boltToParentAng2 > M_PI)
	{
		boltToParentAng2 = -(2*M_PI - boltToParentAng2);
	}
	else if (boltToParentAng2 < -M_PI)
	{
		boltToParentAng2 = 2*M_PI + boltToParentAng2;
	}

	if ((parentAng1 < fMin1 - FLOAT_ZERO_EPSILON) && (bUseYaw))
	{
		// the angle we've been told to reach is outside our limit, so set the angle to our limit
		bRet = ret_ERROR;
		angle1 = fMin1 - boltToParentAng1;
	}
	else if ((parentAng1 > fMax1 + FLOAT_ZERO_EPSILON)  && (bUseYaw))
	{
		// the angle we've been told to reach is outside our limit, so set the angle to our limit
		bRet = ret_ERROR;
		angle1 = fMax1 - boltToParentAng1;
	}
	if ((parentAng2 < fMin2 - FLOAT_ZERO_EPSILON)  && (bUsePitch))
	{
		// the angle we've been told to reach is outside our limit, so set the angle to our limit
		bRet = ret_ERROR;
		angle2  = fMin2 - boltToParentAng2;
	}
	else if ((parentAng2 > fMax2 + FLOAT_ZERO_EPSILON)  && (bUsePitch))
	{
		// the angle we've been told to reach is outside our limit, so set the angle to our limit
		bRet = ret_ERROR;
		angle2  = fMax2 - boltToParentAng2;
	}

	boltonInst->GetXForm(matOld);

	fDelta1 = ((angle1 > 0.01) || (angle1 < -0.01))?angle1:0;
	fDelta2 = ((angle2 > 0.01) || (angle2 < -0.01))?angle2:0;

	if ((fDelta1 > fMaxTurnSpeed) && (bUseYaw))
	{
		fDelta1 = fMaxTurnSpeed;
		// our max turning speed has clamped our motion
		//angle1 = fDelta1 + fOldAngle1;
		bRet = ret_FALSE;
	}
	else if ((fDelta1 < -fMaxTurnSpeed) && (bUseYaw))
	{
		fDelta1 = -fMaxTurnSpeed;
		// our max turning speed has clamped our motion
		//angle1 = fDelta1 + fOldAngle1;
		bRet = ret_FALSE;
	}
	if ((fDelta2 > fMaxTurnSpeed) && (bUsePitch))
	{
		fDelta2 = fMaxTurnSpeed;
		// our max turning speed has clamped our motion
		//angle2 = fDelta2 + fOldAngle2;
		bRet = ret_FALSE;
	}
	else if ((fDelta2 < -fMaxTurnSpeed) && (bUsePitch))
	{
		fDelta2 = -fMaxTurnSpeed;
		// our max turning speed has clamped our motion
		//angle2 = fDelta2 + fOldAngle2;
		bRet = ret_FALSE;
	}

	// if we're outside of our limit, return to our limit
	if ((boltToParentAng1 - fMax1) > 0.0001)
	{
		fDelta1 = ((boltToParentAng1 - fMax1)>fMaxTurnSpeed)?-fMaxTurnSpeed:(fMax1 - boltToParentAng1);
	}
	else if ((fMin1 - boltToParentAng1) > 0.0001)
	{
		fDelta1 = ((fMin1 - boltToParentAng1)>fMaxTurnSpeed)?fMaxTurnSpeed:(fMin1 - boltToParentAng1);
	}
	if ((boltToParentAng2 - fMax2) > 0.0001)
	{
		fDelta2 = ((boltToParentAng2 - fMax2)>fMaxTurnSpeed)?-fMaxTurnSpeed:(fMax2 - boltToParentAng2);
	}
	else if ((fMin2 - boltToParentAng2) > 0.0001)
	{
		fDelta2 = ((fMin2 - boltToParentAng2)>fMaxTurnSpeed)?fMaxTurnSpeed:(fMin2 - boltToParentAng2);
	}
	if (!bUseYaw)
	{
		fDelta1 = angle1 = 0;
	}
	if (!bUsePitch)
	{
		fDelta2 = angle2 = 0;
	}
	// set our return values
	fRetYaw = boltToParentAng1 + fDelta1;
	fRetPitch = boltToParentAng2 + fDelta2;

	if (fDelta1 < 0.001 && fDelta1 > -0.001 &&
		fDelta2 < 0.001 && fDelta2 > -0.001)
	{
		// if our current return code is an error, we're at the limit of our motion. if
		//it's not an error, we've reached our destination
		if (bRet != ret_ERROR)
		{
			bRet = ret_TRUE;
		}
	}
	matR1.Rotate(1, fDelta2);
	matR2.Rotate(2, fDelta1);
	matTemp.Concat(matR1, matOld);
	matNew.Concat(matR2, matTemp);
	boltonInst->SetXForm(matNew);
	return bRet;
}

// OrientInst will pitch/yaw an entity, limiting its rotation speed and orientation 
//with respect to the world
boltonOrientation_c::retCode_e boltonOrientation_c::OrientEnt()
{
	boltonOrientation_c::retCode_e			bRet = ret_TRUE;

	if (!root || !root->ghoulInst || (!bUseYaw && !bUsePitch))
	{
		return ret_ERROR;
	}

	Matrix4		EntityToWorld, WorldToEntity, WorldToParent;
	float		angle1 = 0, angle2 = 0, boltToParentAng1 = 0, boltToParentAng2 = 0,
				parentAng1 = 0, parentAng2 = 0;
	float		fOldAngle1 = 0, fOldAngle2 = 0;
	float		fMin1 = fMinYaw, fMax1 = fMaxYaw, fMin2 = fMinPitch, fMax2 = fMaxPitch;
	float		fDelta1 = 0.0f, fDelta2 = 0.0f;
	Vect3		TargetPos;
	float		XZ = 0, XY = 0;
	bool		bOldUsePitch = bUsePitch, bOldUseYaw = bUseYaw;

	EntToWorldMatrix(root->s.origin,root->s.angles,EntityToWorld);
	TargetPos = *(Vect3*)vTarget;
	WorldToEntity.Inverse(EntityToWorld);
	GetAngles(TargetPos, WorldToEntity, angle1, angle2);

	// our "parent" is actually the absolute world axes at the location of the entity
	WorldToParent.Translate(-root->s.origin[0],-root->s.origin[1],-root->s.origin[2]);
	GetAngles(TargetPos, WorldToParent, parentAng1, parentAng2);

	boltToParentAng1 = -DEGTORAD*root->s.angles[YAW];
	boltToParentAng2 = -DEGTORAD*root->s.angles[PITCH];

	// ensure the traversal is in the shortest direction
	if (parentAng1 > M_PI)
	{
		parentAng1 = -(2*M_PI - parentAng1);
	}
	else if (parentAng1 < -M_PI)
	{
		parentAng1 = 2*M_PI + parentAng1;
	}
	if (parentAng2 > M_PI)
	{
		parentAng2 = -(2*M_PI - parentAng2);
	}
	else if (parentAng2 < -M_PI)
	{
		parentAng2 = 2*M_PI + parentAng2;
	}

	if ((parentAng1 < fMin1 - FLOAT_ZERO_EPSILON) && (bUseYaw))
	{
		// the angle we've been told to reach is outside our limit, so set the angle to our limit
		bRet = ret_ERROR;
		angle1 = fMin1 - boltToParentAng1;
	}
	else if ((parentAng1 > fMax1 + FLOAT_ZERO_EPSILON)  && (bUseYaw))
	{
		// the angle we've been told to reach is outside our limit, so set the angle to our limit
		bRet = ret_ERROR;
		angle1 = fMax1 - boltToParentAng1;
	}
	if ((parentAng2 < fMin2 - FLOAT_ZERO_EPSILON)  && (bUsePitch))
	{
		// the angle we've been told to reach is outside our limit, so set the angle to our limit
		bRet = ret_ERROR;
		angle2 = fMin2 - boltToParentAng2;
	}
	else if ((parentAng2 > fMax2 + FLOAT_ZERO_EPSILON)  && (bUsePitch))
	{
		// the angle we've been told to reach is outside our limit, so set the angle to our limit
		bRet = ret_ERROR;
		angle2 = fMax2 - boltToParentAng2;
	}


	fOldAngle1 = boltToParentAng1;
	fOldAngle2 = boltToParentAng2;
	fDelta1 = angle1;
	fDelta2 = angle2;

	if ((fDelta1 > fMaxTurnSpeed) && (bUseYaw))
	{
		fDelta1 = fMaxTurnSpeed;
		// our max turning speed has clamped our motion
		bRet = ret_FALSE;
	}
	else if ((fDelta1 < -fMaxTurnSpeed) && (bUseYaw))
	{
		fDelta1 = -fMaxTurnSpeed;
		// our max turning speed has clamped our motion
		bRet = ret_FALSE;
	}
	if ((fDelta2 > fMaxTurnSpeed) && (bUsePitch))
	{
		fDelta2 = fMaxTurnSpeed;
		// our max turning speed has clamped our motion
		bRet = ret_FALSE;
	}
	else if ((fDelta2 < -fMaxTurnSpeed) && (bUsePitch))
	{
		fDelta2 = -fMaxTurnSpeed;
		// our max turning speed has clamped our motion
		bRet = ret_FALSE;
	}

	if (bUseYaw)
	{
		angle1 = fDelta1 + fOldAngle1;
		root->s.angles[YAW] = -angle1*RADTODEG;
	}
	if (bUsePitch)
	{
		angle2 = fDelta2 + fOldAngle2;
		root->s.angles[PITCH] = -angle2*RADTODEG;
	}
	return bRet;
}

void boltonOrientation_c::GetAngles(Vect3 &pos, Matrix4 &mat, float &angle1, float &angle2)
{
	Vect3 posInMatrixSpace;

	mat.XFormPoint(posInMatrixSpace,pos);
	angle1 = -atan2(posInMatrixSpace.y(), posInMatrixSpace.x());
	angle2 = atan2(posInMatrixSpace.z(), sqrt(posInMatrixSpace.y()*posInMatrixSpace.y()+posInMatrixSpace.x()*posInMatrixSpace.x()));
}

// think function for applying the flipping velocity to an object
void ObjFlippingThink(edict_t *self)
{
	objInfo_c		*pHdr = NULL;
	objFlipInfo_c	*pFlip = NULL;

	// give the object a rotational velocity in the proper direction
	if (self && self->objInfo && (pFlip = (objFlipInfo_c*)self->objInfo->GetInfo(OIT_FLIP)) )
	{
		if (pFlip->Update())
		{
			// finished rotating
			if (self->think = pFlip->lastThinkFn)
			{
				self->nextthink = level.time + FRAMETIME;
			}
			// get rid of the objFlipInfo_c we created in FlipObject()
			pFlip->Destroy();
		}
		else
		{
			// not finished rotating yet
			self->nextthink = level.time + FRAMETIME;
		}
	}
}

// rotate an object 90 degrees (if possible). used for flipping tables over for cover, 
//knocking over barrels, etc.
void FlipObject(edict_t *ent,edict_t *pusher, edict_t *activator)
{
	vec3_t		fwd, right, vEnd = {0,0,0}, vPush;
	float		checkDist = 0, fDot = 0;
	trace_t		trace;
	vec3_t		vTemp;
	objFlipInfo_c *newFlip = NULL;

	if ( !ent->objInfo || !(newFlip = (objFlipInfo_c*)ent->objInfo->GetInfo(OIT_FLIP)) )
	{
		// bad
		return;
	}
	// set the current ground level underneath our object
	newFlip->ground = ent->s.origin[2] + ent->mins[2];
	VectorSet(vTemp, ent->s.origin[0], ent->s.origin[1], newFlip->ground);
	VectorCopy(ent->s.angles, newFlip->preFlipAngles);
	// determine which direction we're trying to push the object
	AngleVectors(ent->s.angles,fwd,right,NULL);
	VectorSubtract(ent->s.origin, pusher->s.origin, vPush);
	vPush[2] = 0;
	VectorNormalize(vPush); // ouch
	if ( ((fDot = DotProduct(fwd, vPush)) > .707f) ||
		 (fDot < -.707f) )
	{
		// want either the forward or backward vector of the pushed object. either way, the 
		//new height of the object is the oldmaxs[0] - oldmins[0]
		checkDist = ent->maxs[0] - ent->mins[0];
		// is there enough vertical space to flip the object?
		VectorMA(vTemp, checkDist, vec3_up, vEnd);
		gi.trace (vTemp, NULL, NULL, vEnd, ent, CONTENTS_SOLID, &trace);
		if (trace.fraction < 1.0f)
		{
			// there isn't enough overhead clearance so don't flip
			ent->objInfo->ResetInfo(OIT_FLIP);
			return;
		}
		if (fDot < 0.0)
		{
			// pushing in the direction of the backward vector
			// can we move the object in the direction of the push?
			checkDist = ent->maxs[2] - ent->mins[2];
			VectorMA(ent->s.origin, -checkDist, fwd, vEnd);
			gi.trace (ent->s.origin, NULL, NULL, vEnd, ent, CONTENTS_SOLID, &trace);
			if (trace.fraction < 1.0f)
			{
				// there isn't forward clearance so don't flip
				ent->objInfo->ResetInfo(OIT_FLIP);
				return;
			}
			// begin flipping the object backward
			newFlip->dir = objFlipInfo_c::flipDir_e::FLIPDIR_B;
			VectorNegate(fwd, newFlip->fwd);
			newFlip->radius = sqrt(ent->mins[0]*ent->mins[0] + ent->mins[2]*ent->mins[2]);
			VectorMA(vTemp, -ent->mins[0], newFlip->fwd, newFlip->flipOrigin);
			newFlip->initRadiusAng = 90 + (RADTODEG*atan2(-ent->mins[0], -ent->mins[2])); // atan(x/y)
			newFlip->lastThinkFn = ent->think;
		}
		else
		{
			// pushing in the direction of the forward vector
			// can we move the object in the direction of the push?
			checkDist = ent->maxs[2] - ent->mins[2];
			VectorMA(ent->s.origin, checkDist, fwd, vEnd);
			gi.trace (ent->s.origin, NULL, NULL, vEnd, ent, CONTENTS_SOLID, &trace);
			if (trace.fraction < 1.0f)
			{
				// there isn't forward clearance so don't flip
				ent->objInfo->ResetInfo(OIT_FLIP);
				return;
			}
			// begin flipping the object forward
			newFlip->dir = objFlipInfo_c::flipDir_e::FLIPDIR_F;
			VectorCopy(fwd, newFlip->fwd);
			newFlip->radius = sqrt(ent->maxs[0]*ent->maxs[0] + ent->mins[2]*ent->mins[2]);
			VectorMA(vTemp, ent->maxs[0], newFlip->fwd, newFlip->flipOrigin);
			newFlip->initRadiusAng = 90 + (RADTODEG*atan2(ent->maxs[0], -ent->mins[2])); // atan(x/y)
			newFlip->lastThinkFn = ent->think;
		}
	}
	else
	{
		// want either the right or left vector of the pushed object. either way, the 
		//new height of the object is the oldmaxs[1] - oldmins[1]
		checkDist = ent->maxs[1] - ent->mins[1];
		// is there enough vertical space to flip the object?
		VectorMA(vTemp, checkDist, vec3_up, vEnd);
		gi.trace (vTemp, NULL, NULL, vEnd, ent, CONTENTS_SOLID, &trace);
		if (trace.fraction < 1.0f)
		{
			// there isn't enough overhead clearance so don't flip
			ent->objInfo->ResetInfo(OIT_FLIP);
			return;
		}
		if ( (fDot = DotProduct(right, vPush)) < 0.0 )
		{
			// pushing in the direction of the left vector
			// can we move the object in the direction of the push?
			checkDist = ent->maxs[2] - ent->mins[2];
			VectorMA(ent->s.origin, -checkDist, right, vEnd);
			gi.trace (ent->s.origin, NULL, NULL, vEnd, ent, CONTENTS_SOLID, &trace);
			if (trace.fraction < 1.0f)
			{
				// there isn't forward clearance so don't flip
				ent->objInfo->ResetInfo(OIT_FLIP);
				return;
			}
			// begin flipping the object to the left
			newFlip->dir = objFlipInfo_c::flipDir_e::FLIPDIR_L;
			VectorNegate(right, newFlip->fwd);
			newFlip->radius = sqrt(ent->mins[1]*ent->mins[1] + ent->mins[2]*ent->mins[2]);
			VectorMA(vTemp, -ent->mins[1], newFlip->fwd, newFlip->flipOrigin);
			newFlip->initRadiusAng = 90 + (RADTODEG*atan2(-ent->mins[1], -ent->mins[2])); // atan(x/y)
			newFlip->lastThinkFn = ent->think;
		}
		else
		{
			// pushing in the direction of the right vector
			// can we move the object in the direction of the push?
			checkDist = ent->maxs[2] - ent->mins[2];
			VectorMA(ent->s.origin, checkDist, right, vEnd);
			gi.trace (ent->s.origin, NULL, NULL, vEnd, ent, CONTENTS_SOLID, &trace);
			if (trace.fraction < 1.0f)
			{
				// there isn't forward clearance so don't flip
				ent->objInfo->ResetInfo(OIT_FLIP);
				return;
			}
			// begin flipping the object to the right
			newFlip->dir = objFlipInfo_c::flipDir_e::FLIPDIR_R;
			VectorCopy(right, newFlip->fwd);
			newFlip->radius = sqrt(ent->maxs[1]*ent->maxs[1] + ent->mins[2]*ent->mins[2]);
			VectorMA(vTemp, ent->maxs[1], newFlip->fwd, newFlip->flipOrigin);
			newFlip->initRadiusAng = 90 + (RADTODEG*atan2(ent->maxs[1], -ent->mins[2])); // atan(x/y)
			newFlip->lastThinkFn = ent->think;
		}
	}
	// once an object is flipped, don't let the player try to flip it again
	ent->plUse = NULL;
	ent->think = ObjFlippingThink;
	ent->nextthink = level.time + FRAMETIME;
}

//
// boltInstInfo_c stuff
//


boltInstInfo_c::boltInstInfo_c(boltInstInfo_c *orig)
: objInfo_c(orig)
{
	onOff = orig->onOff;
	nNum = orig->nNum; 
	nMax = orig->nMax; 
}

void boltInstInfo_c::Evaluate(boltInstInfo_c *orig)
{
	onOff = orig->onOff;
	nNum = orig->nNum; 
	nMax = orig->nMax; 

	objInfo_c::Evaluate(orig);
}

boltInstInfo_c::~boltInstInfo_c()
{
	onOff = 0;
}

bool boltInstInfo_c::AddBolt(bool bOn /*true*/)
{
	if (nNum < nMax)
	{
		nNum++;
		if (bOn)
		{
			TurnOn(nNum);
		}
		else
		{
			TurnOff(nNum);
		}
		return true;
	}
	return false;
}

void boltInstInfo_c::Destroy()
{
	Reset();
}

void boltInstInfo_c::Reset()
{
	head = NULL;
	nInfoType = OIT_BOLTINST;
	onOff = 0;
	nNum = 0;
	nMax = MAX_BOLTINSTINFO;
}

//
// baseObjInfo_c stuff
//

baseObjInfo_c::~baseObjInfo_c()
{
	// because the elems of this list are ggBinstC's, gameghoul will (hopefully) 
	//free the objects themselves. we should only have to free the list space here.
	bolts.erase(bolts.begin(), bolts.end());
}

IGhoulInst* baseObjInfo_c::GetBolt(int nBolt)
{
	// every place in the project _except_in_here_, bolt indices are 1-indexed. argh.
	gg_binst_c_ptr binst;

	if (nBolt <= bolts.size() && nBolt > 0)
	{
		binst = bolts[nBolt-1];
		if ((ggBinstC*)binst)
		{
			return ((ggBinstC*)binst)->GetInstPtr();
		}
	}
	return NULL;
}

objInfo_c*	baseObjInfo_c::GetInfo(int nType)
{
	switch(nType)
	{
	case OIT_BOLTINST:
		{
			return (objInfo_c*)&boltInstInfo;
			break;
		}
	case OIT_FLIP:
		{
			return (objInfo_c*)&flipInfo;
			break;
		}
	}
	return NULL;
}

bool baseObjInfo_c::AddBolt(ggBinstC *newBolt)
{
	boltInstInfo_c*	instInfo = NULL;

	if (newBolt)
	{
		instInfo = (boltInstInfo_c*)GetInfo(OIT_BOLTINST);
		instInfo->AddBolt();
		bolts.push_back((gg_binst_c_ptr)newBolt);
		return true;
	}
	return false;
}

void baseObjInfo_c::ResetInfo(int nType)
{
	switch(nType)
	{
	case OIT_BOLTINST:
		{
			boltInstInfo.Reset();
			break;
		}
	case OIT_FLIP:
		{
			flipInfo.Reset();
			break;
		}
	default:	// no clue what type we're talking about
		{
			break;
		}
	}
}

baseObjInfo_c::baseObjInfo_c(baseObjInfo_c *orig)
{
	boltInstInfo_c	*bii;
	objFlipInfo_c	*ofi;

	*(int *)&owner = GetEdictNum(orig->owner);

	bii = new boltInstInfo_c(&orig->boltInstInfo);
	boltInstInfo = *bii;
	delete bii;

	ofi = new objFlipInfo_c(&orig->flipInfo);
	flipInfo = *ofi;
	delete ofi;
}

void baseObjInfo_c::Evaluate(baseObjInfo_c *orig)
{
	owner = GetEdictPtr((int)orig->owner);
	boltInstInfo.Evaluate(&orig->boltInstInfo);
	flipInfo.Evaluate(&orig->flipInfo);
}

void baseObjInfo_c::Read(void)
{
	char		loaded[sizeof(baseObjInfo_c)];
	int			count, i;
	int			*index_list;

	gi.ReadFromSavegame('EDOI', loaded + BOI_SAVE_START, sizeof(baseObjInfo_c) - BOI_SAVE_START);
	Evaluate((baseObjInfo_c *)loaded);

	owner->objInfo = this;

	gi.ReadFromSavegame('EDBN', &count, sizeof(count));
	if(count)
	{
		index_list = new int [count];
		gi.ReadFromSavegame('EDBL', index_list, count * sizeof(int));
		for(i = 0; i < count; i++)
		{
			bolts.push_back(binstL.GetPointerFromIndex(index_list[i]));
		}
		delete [] index_list;
	}
}

void baseObjInfo_c::Write(void)
{
	baseObjInfo_c						*savable;
	byte								*save_start;
	int									count, i;
	vector<gg_binst_c_ptr>::iterator	it;
	int									*index_list;
	
	savable = new baseObjInfo_c(this);
	save_start = (byte *)savable;
	gi.AppendToSavegame('EDOI', save_start + BOI_SAVE_START, sizeof(*this) - BOI_SAVE_START);
	delete savable;

	count = bolts.size();
	gi.AppendToSavegame('EDBN', &count, sizeof(count));

	if(count)
	{
		index_list = new int [count];
		for(it = bolts.begin(), i = 0; it != bolts.end(); it++, i++)
		{
			index_list[i] = (*it).GetIndex();
		}
		gi.AppendToSavegame('EDBL', index_list, count * sizeof(int));
		delete [] index_list;
	}
}

//
// objInfo_c stuff
//

void objInfo_c::Evaluate(objInfo_c *orig)
{
	nInfoType = orig->nInfoType;					
	head = GetEdictPtr((int)orig->head);
}

objInfo_c::objInfo_c(objInfo_c *orig)
{
	nInfoType = orig->nInfoType;					
	*(int *)&head = GetEdictNum(orig->head);
}

void objInfo_c::Destroy()
{
	switch(nInfoType)
	{
	case OIT_BOLTINST:
		{
			((boltInstInfo_c*)this)->Destroy();
			break;
		}
	case OIT_FLIP:
		{
			((objFlipInfo_c*)this)->Destroy();
			break;
		}
	default:	// no clue what type we're talking about
		{
			delete this;
			break;
		}
	}
}

//
// objFlipInfo_c stuff
//

objFlipInfo_c::objFlipInfo_c(objFlipInfo_c *orig)
: objInfo_c(orig)
{
	dir = orig->dir;
	startTime = orig->startTime;
	ground = orig->ground;
	radius = orig->radius;
	initRadiusAng = orig->initRadiusAng;
	VectorCopy(orig->preFlipAngles, preFlipAngles);
	VectorCopy(orig->flipOrigin, flipOrigin);
	VectorCopy(orig->fwd, fwd);
	*(int *)&lastThinkFn = GetThinkNum(orig->lastThinkFn);
}

void objFlipInfo_c::Evaluate(objFlipInfo_c *orig)
{
	dir = orig->dir;
	startTime = orig->startTime;
	ground = orig->ground;
	radius = orig->radius;
	initRadiusAng = orig->initRadiusAng;
	VectorCopy(orig->preFlipAngles, preFlipAngles);
	VectorCopy(orig->flipOrigin, flipOrigin);
	VectorCopy(orig->fwd, fwd);
	lastThinkFn = (void (*)(edict_t *self))GetThinkPtr((int)orig->lastThinkFn);

	objInfo_c::Evaluate(orig);
}

objFlipInfo_c::objFlipInfo_c(edict_t *ent)
: objInfo_c(ent)
{
	nInfoType = OIT_FLIP;
	dir = FLIPDIR_UNK;
	lastThinkFn = NULL;
	VectorClear(preFlipAngles);
	VectorClear(fwd);
	VectorClear(flipOrigin);
	ground = 0;
	radius = 0;
	initRadiusAng = 0;
	startTime = level.time;
}
 
void objFlipInfo_c::Destroy()
{
	Reset();
}

void objFlipInfo_c::Reset()
{
	head = NULL;
	nInfoType = OIT_FLIP;
	dir = FLIPDIR_UNK;
	lastThinkFn = NULL;
	VectorClear(preFlipAngles);
	VectorClear(fwd);
	VectorClear(flipOrigin);
	ground = 0;
	radius = 0;
	initRadiusAng = 0;
	startTime = level.time;
}

bool objFlipInfo_c::Update()
{
	float			fInitVel = 30, // gotta be higher than 12 to overcome friction
					fDelta = 0;
	bool			bStop = false;
	vec3_t			vTemp;

	if (head)
	{
		switch(dir)
		{
		case flipDir_e::FLIPDIR_F: // pitch 90deg
			{
				// if we don't yet have a rotational velocity, start one
				if (0 == head->avelocity[PITCH])
				{
					head->avelocity[PITCH] = fInitVel;
				}
				else // we're already rotating. when do we stop?
				{
					if ( (fDelta = (head->s.angles[PITCH] - preFlipAngles[PITCH])) > 85)
					{
						// stop the rotation
						head->avelocity[PITCH] = 0;
						head->s.angles[PITCH] = preFlipAngles[PITCH] + 90;
						// make sure the object is placed properly
						VectorMA(flipOrigin, -head->mins[2], fwd, vTemp);
						VectorMA(vTemp, head->maxs[0], vec3_up, head->s.origin);
						bStop = true;
					}
					else
					{ // accelerate our rotation a little bit
						head->avelocity[PITCH] += fInitVel;
					}
				}
				break;
			}
		case flipDir_e::FLIPDIR_B: // pitch 270deg
			{
				// if we don't yet have a rotational velocity, start one
				if (0 == head->avelocity[PITCH])
				{
					head->avelocity[PITCH] = -fInitVel;
				}
				else // we're already rotating. when do we stop?
				{
					if ( (fDelta = (preFlipAngles[PITCH] - head->s.angles[PITCH])) > 85)
					{
						// stop the rotation
						head->avelocity[PITCH] = 0;
						head->s.angles[PITCH] = preFlipAngles[PITCH] - 90;
						// make sure the object is placed properly
						VectorMA(flipOrigin, -head->mins[2], fwd, vTemp);
						VectorMA(vTemp, -head->mins[0], vec3_up, head->s.origin);
						bStop = true;
					}
					else
					{ // accelerate our rotation a little bit
						head->avelocity[PITCH] -= fInitVel;
					}
				}
				break;
			}
		case flipDir_e::FLIPDIR_L: // roll 270deg
			{
				// if we don't yet have a rotational velocity, start one
				if (0 == head->avelocity[ROLL])
				{
					head->avelocity[ROLL] = -fInitVel;
				}
				else // we're already rotating. when do we stop?
				{
					if ( (fDelta = (preFlipAngles[ROLL] - head->s.angles[ROLL])) > 85)
					{
						// stop the rotation
						head->avelocity[ROLL] = 0;
						head->s.angles[ROLL] = preFlipAngles[ROLL] - 90;
						// make sure the object is placed properly
						VectorMA(flipOrigin, -head->mins[2], fwd, vTemp);
						VectorMA(vTemp, head->maxs[1], vec3_up, head->s.origin);
						bStop = true;
					}
					else
					{ // accelerate our rotation a little bit
						head->avelocity[ROLL] -= fInitVel;
					}
				}
				break;
			}
		case flipDir_e::FLIPDIR_R: // roll 90deg
			{
				// if we don't yet have a rotational velocity, start one
				if (0 == head->avelocity[ROLL])
				{
					head->avelocity[ROLL] = fInitVel;
				}
				else // we're already rotating. when do we stop?
				{
					if ( (fDelta = (head->s.angles[ROLL] - preFlipAngles[ROLL])) > 85)
					{
						// stop the rotation
						head->avelocity[ROLL] = 0;
						head->s.angles[ROLL] = preFlipAngles[ROLL] + 90;
						// make sure the object is placed properly
						VectorMA(flipOrigin, -head->mins[2], fwd, vTemp);
						VectorMA(vTemp, -head->mins[1], vec3_up, head->s.origin);
						bStop = true;
					}
					else
					{ // accelerate our rotation a little bit
						head->avelocity[ROLL] += fInitVel;
					}
				}
				break;
			}
		}
		if (!bStop)
		{
			// update our origin in the horizontal sense
			VectorScale(fwd, cos(DEGTORAD*(initRadiusAng - fDelta))*radius, vTemp);	
			// update in the vertical sense
			VectorMA(vTemp, sin(DEGTORAD*(initRadiusAng - fDelta))*radius, vec3_up, vTemp);
			// update our edict's position
			VectorAdd(vTemp, flipOrigin, head->s.origin);
		}
		else
		{
			// we stopped rotating so fix our bbox
			BboxRotate(head);
		}
		return bStop;
	}
	// not _really_ true but it'll get us out of this fn
	return true;
}