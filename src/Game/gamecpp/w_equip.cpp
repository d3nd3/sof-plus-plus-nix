// ************************************************************************
//
//		w_equip.cpp
//
// This is for all selectable items.
// While some of these items (like the C4) can still be considered "weapons", 
// they don't use the "weapon" code, and so it is useful (and less of a load 
// on the overburdened w_fire.cpp module) to separate "equipment" out.
// ************************************************************************

#include "g_local.h"
#include "w_weapons.h"
#include "ai_private.h"	// so we know what a bodyhuman_c is
#include "../strings/items.h"

const unsigned short	MEDKIT_HEAL_DEFAULT	 = 25;
const float				REALISTIC_MOVESCALE	 = 0.5625;


extern edict_t	*SV_TestEntityPosition (edict_t *ent);

extern void SetProjVel(edict_t &self, edict_t &newEnt, float fwd, float right, float up);
extern void SetProjPhysics(edict_t &self, float friction, float gravity, float airresistance, float bouyancy, float elasticity);

// need to know about leaning when we toss c4
extern edict_t *GetLeanBuddy(edict_t *owner);

//yurg - don't wanna make it like this but the player .dll nonsense is just too annoying to deal with
void cacheForItem(equipment_t curEquip)
{
	switch(curEquip)
	{
	case SFE_FLASHPACK:
		game_ghoul.FindObject("Items/Projectiles", "flashpak")->RegistrationLock();
		gi.soundindex("Weapons/gren/bounce.wav");
		break;
//	case SFE_NEURAL_GRENADE:
//		gi.effectindex("weapons/world/neural");
		break;
	case SFE_C4:
		game_ghoul.FindObject("Items/Projectiles", "C4")->RegistrationLock();
		gi.soundindex("Weapons/C4/c4attch.wav");
		gi.soundindex("Weapons/C4/Beep.wav");
		break;
	case SFE_LIGHT_GOGGLES:
		gi.soundindex("Weapons/Goggles/on.wav");
		gi.soundindex("Weapons/Goggles/off.wav");
		break;
	case SFE_CLAYMORE:
		game_ghoul.FindObject("Items/Projectiles", "claymore")->RegistrationLock();
		gi.effectindex("weapons/world/claymore");
		gi.soundindex("Weapons/C4/c4attch.wav");
		gi.soundindex("Weapons/Claymore/Beep.wav");
		break;
	case SFE_MEDKIT:
		break;
	case SFE_GRENADE:
		game_ghoul.FindObject("Items/Projectiles", "grenade")->RegistrationLock();
		gi.effectindex("weapons/world/grenadeexplode");
		gi.soundindex("Weapons/gren/bounce.wav");
		break;
	default:
		break;
	}
}



//------------------------------------MEDKIT-------------------------------------------
qboolean tryToHeal(edict_t *ent,int amountToHeal);

bool medkitUse(edict_t &ent, itemSlot_c &slot)
{
	if (dm->dmRule_REALISTIC_DAMAGE())
	{
		// heal leg and arm damage, but not health!
		bodyhuman_c	*body = NULL;

		if (body = (bodyhuman_c*)ent.client->body)
		{
			body->RemoveLimbDamage();
			resetMovescale(&ent, 0);
		}
		gi.sound(&ent, CHAN_ITEM, gi.soundindex("dm/healthpu.wav"), 1.0, ATTN_IDLE, 0);
		return true;
	}

	bool didWeHeal = !!tryToHeal(&ent, MEDKIT_HEAL_DEFAULT);
	
	if (!didWeHeal)
	{		
		// print the "We're at full health, we don't need this, Bozo!" message
		gi.SP_Print(&ent, ITEMS_FULL_HEALTH);
	}
	return didWeHeal;
}

//------------------------------------GOGGLES-------------------------------------------

bool goggleUse(edict_t &ent, itemSlot_c &slot)
{
	if (!ent.client)
		return true;

	if (slot.getSlotStatus())
	{	// Goggles are on!
		ent.client->goggles_on = true;
		gi.sound(&ent, CHAN_VOICE, gi.soundindex("Weapons/Goggles/on.wav"), .6, ATTN_IDLE, 0);//fixme
	}
	else
	{
		ent.client->goggles_on = false;
		if (!game.cinematicfreeze)
		{
			gi.sound(&ent, CHAN_VOICE, gi.soundindex("Weapons/Goggles/off.wav"), .6, ATTN_IDLE, 0);//fixme
		}
	}
	return true;
}


//------------------------------------GRENADE-------------------------------------------

// Grenades are weapons, used by opponents as well.  For this reason, most of the functionality resides in w_fire.
extern void createGrenade(edict_t *owner, vec3_t loc, vec3_t vel, qboolean enemy);

bool grenadeUse(edict_t &ent, itemSlot_c &slot)
{
	vec3_t fwdV, rightV, upV;
	vec3_t vel, pos;
	edict_t *leanBuddy = GetLeanBuddy(&ent);

	// Decide on velocity.  Start by getting directional vectors.
	if(ent.client)
	{
		AngleVectors(ent.client->ps.viewangles, fwdV, rightV, upV);
	}
	else
	{
		AngleVectors(ent.s.angles, fwdV, rightV, upV);
	}

	VectorMA(ent.velocity,	475.0,				fwdV,	vel);
	VectorMA(vel,			gi.flrand(-15,15),	rightV, vel);
	VectorMA(vel,			gi.flrand(300,350),	upV,	vel);
	
	// kef -- take into account player leaning
	if (leanBuddy)
	{
		VectorCopy(leanBuddy->s.origin, pos);
		pos[2] = ent.s.origin[2] + ent.viewheight-20.0;
	}
	else
	{
		VectorCopy(ent.s.origin, pos);
		pos[2] += ent.viewheight-20.0;
	}

	createGrenade(&ent, pos, vel, false);
	return true;
}

//------------------------------------FLASHPACK-------------------------------------------

void flashTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	if(plane && VectorLengthSquared(self->velocity) > 100)
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("Weapons/gren/bounce.wav"), .6, 2, 0);//fixme - bounce
	}
}

void flashpackExplode(edict_t *ent)
{
	vec3_t	pos;

	// if this thing is trying to explode during a cinematic, just get rid of it
	if (!game.cinematicfreeze)
	{
		VectorCopy(ent->s.origin, pos);
		pos[2] += 15;

		ent->solid = SOLID_NOT;

		BlindingLight(pos, 10000, 0.99, 0.15);

		FX_Flashpack(pos);
		T_RadiusDamage (ent, ent->owner, 20, ent, 100, MOD_FIRE, 0);
	}
	G_FreeEdict(ent);

	//this should probably affect enemies as well...
}

void flashpackFunc(edict_t &ent)
{
	edict_t *flashpack;
	edict_t *leanBuddy = GetLeanBuddy(&ent);

	flashpack = G_Spawn();
	SetProjVel(ent, *flashpack, 400, 0, gi.flrand(200, 400));
	SetProjPhysics(*flashpack, .7, 1.0, 0, 0, .7);
	// kef -- take into account player leaning
	if (leanBuddy)
	{
		VectorCopy(leanBuddy->s.origin, flashpack->s.origin);
		flashpack->s.origin[2] = ent.s.origin[2] + ent.viewheight;
	}
	else
	{
		VectorCopy(ent.s.origin, flashpack->s.origin);
		flashpack->s.origin[2] += ent.viewheight;
	}

	VectorSet (flashpack->mins, -2, -2, -3);
	VectorSet (flashpack->maxs, 2, 2, 3);
	flashpack->health = 100;
	flashpack->think = flashpackExplode;
	flashpack->nextthink = level.time + 1.5;
	VectorSet(flashpack->avelocity, gi.irand(-300, 300), gi.irand(-300, 300), gi.irand(-300, 300));
	//gi.setmodel(flashpack,"models/objects/generic/chunks_wall/tris.fm");
	game_ghoul.SetSimpleGhoulModel(flashpack, "Items/Projectiles", "flashpak");
	gi.linkentity(flashpack);
	flashpack->owner = &ent;
	flashpack->touch = flashTouch;
	flashpack->flags = FL_THROWN_WPN;

	// Play grenade throwing sound
	gi.sound(&ent, CHAN_VOICE, gi.soundindex("Weapons/gren/throw.wav"), .6, 1, 0);//fixme - bounce

	if(SV_TestEntityPosition(flashpack))
	{
		flashpack->touch(flashpack, g_edicts, 0, 0);
	}

}

bool flashpackUse(edict_t &ent, itemSlot_c &slot)
{
	flashpackFunc(ent);
	return true;
}

//--------------------------------NEURAL GRENADE----------------------------------------

bool neuralGrenadeUse(edict_t &ent, itemSlot_c &slot)
{
#ifndef _OEM_
	T_RadiusDamage (&ent, &ent, 400, &ent, 256, MOD_NEURAL_GRENADE, DT_NEURAL|DT_SHOCK|DAMAGE_NO_KNOCKBACK);
	gmonster.RadiusDeafen(&ent, 400, 300);
	fxRunner.setPos2(ent.s.origin);
	fxRunner.exec("weapons/world/emitter", ent.s.origin);
#endif
	return true;
}


//---------------------------------C4/PLASTIQUE------------------------------------------

void c4Explode(edict_t *self)
{
	if(self->health < 104)
	{
		gmonster.SpookEnemies(self->s.origin, 160, SPOOK_EXPLOSIVE);//hahaha
		gi.sound(self, CHAN_VOICE, gi.soundindex("Weapons/C4/Beep.wav"), .6, ATTN_NORM, 0);
		self->health++;
		self->nextthink = level.time + .4;
	}
	else
	{
		if (!game.cinematicfreeze)
		{	// if we're in a cinematic don't do any of this...just free the edict.
			vec3_t fwd, pos;

			AngleVectors(self->s.angles, fwd, NULL, NULL);
			VectorMA(self->s.origin, 10, fwd, pos);

			T_RadiusDamage2(self, self->owner, pos, 250, 32, 200, NULL, MOD_C4, DT_MANGLE|DT_DIRECTIONALGORE);

			BlindingLight(self->s.origin, 2500, 0.8, 0.5);
			gmonster.RadiusDeafen(self, 300, 300);

			fxRunner.setDir(fwd);
			fxRunner.exec("weapons/world/c4explode", self->s.origin);
			IncreaseSpawnIntensity(.6);
		}
		G_FreeEdict(self);
	}
}

void c4Touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	if(plane)
	{
		if(plane->normal)
		{
			if(other == g_edicts)
			{
				if(self->gravity)
				{
					gmonster.SpookEnemies(self->s.origin, 160, SPOOK_EXPLOSIVE);
					gi.sound(self, CHAN_VOICE, gi.soundindex("Weapons/C4/c4attch.wav"), .6, ATTN_NORM, 0);
					self->gravity = 0;
					VectorClear(self->velocity);
					VectorClear(self->avelocity);
					if(plane)
					{
						vectoangles(plane->normal, self->s.angles);
					}
				}
			}
		}
	}
}

bool c4use(edict_t &ent, itemSlot_c &slot)
{
	edict_t *c4;
	edict_t *leanBuddy = GetLeanBuddy(&ent);

	c4 = G_Spawn();
	game_ghoul.SetSimpleGhoulModel(c4, "Items/Projectiles", "C4");
	SetProjVel(ent, *c4, 410.0, gi.flrand(-30, 30), gi.flrand(150, 180));
	SetProjPhysics(*c4, 1.0, 1.0, 0, 0, 0.0);

	// kef -- take into account player leaning
	if (leanBuddy)
	{
		VectorCopy(leanBuddy->s.origin, c4->s.origin);
		c4->s.origin[2] = ent.s.origin[2] + ent.viewheight;
	}
	else
	{
		VectorCopy(ent.s.origin, c4->s.origin);
		c4->s.origin[2] += ent.viewheight;
	}

	VectorSet (c4->mins, -2, -2, -2);
	VectorSet (c4->maxs, 2, 2, 2);
	c4->health = 100;
	c4->think = c4Explode;
	c4->nextthink = level.time + .5;
	gi.linkentity(c4);
	c4->owner = &ent;
	c4->touch = c4Touch;
	c4->flags = FL_THROWN_WPN;
	VectorSet(c4->avelocity, gi.irand(-300, 300), gi.irand(-300, 300), gi.irand(-300, 300));

	// Play grenade throwing sound
	gi.sound(&ent, CHAN_VOICE, gi.soundindex("Weapons/gren/throw.wav"), .6, 1, 0);//fixme - bounce

	//c4->physicsFlags |= PHYS_ATTACHED|PHYS_ATTACH_COPYANG;

	if(SV_TestEntityPosition(c4))
	{
		c4->touch(c4, g_edicts, 0, 0);
	}
	return true;
}

//-----------------------------CLAYMORE-----------------------------------

void claymoreThink(edict_t *self)
{
	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	self->owner = g_edicts;
	CRadiusContent rad(self->s.origin, 128);

	if(self->health < 3)
	{	// play activate sound
		gi.sound(self, CHAN_VOICE, gi.soundindex("Weapons/Claymore/Beep.wav"), .6, ATTN_NORM, 0);//fixme
		self->health = 3;
	}

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		edict_t *search = rad.foundEdict(i);

		if(search->client || search->ai || (self->gib_health < 1))
		{
			if((search->health > 0)||(self->gib_health < 1))
			{
				vec3_t	pos;

				self->owner = self->targetEnt;

				VectorCopy(self->s.origin, pos);
				pos[2] += 10;

				T_RadiusDamage2(self, self->owner, self->s.origin, 300, 150, 100, NULL, MOD_CLAYMORE, DT_MANGLE|DT_DIRECTIONALGORE);
				gmonster.RadiusDeafen(self, 300, 300);

//				fxRunner.exec("weapons/world/airexplode", self->s.origin);
//				FX_C4Explosion(self);

//				BlindingLight(pos, 5000, 0.8, 0.5);
//				ShakeCameras (self->s.origin, 500, 300, 25);
				IncreaseSpawnIntensity(.4);
				fxRunner.exec("weapons/world/claymore", self->s.origin);
				G_FreeEdict(self);

				return;
			}
		}
	}

	self->gib_health--;
	self->nextthink += .2;
}

void claymore_die(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	if(!self->gib_health)
	{	//already killed myself.
		return;
	}

	self->gib_health = 0;

	self->think = claymoreThink;
	self->nextthink = level.time + 0.2;
}

void claymoreTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if(self->health < 2)
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("Weapons/C4/c4attch.wav"), .6, ATTN_NORM, 0);
		self->health = 2;
	}
}


bool claymoreUse(edict_t &ent, itemSlot_c &slot)
{
#ifndef _OEM_
	edict_t *claymore;

	claymore = G_Spawn();
	game_ghoul.SetSimpleGhoulModel(claymore, "Items/Projectiles", "claymore");
	SetProjVel(ent, *claymore, 0.0, 0.0, 0.0);
	SetProjPhysics(*claymore, 1.0, 1.0, 0, 0, 0.0);
	VectorCopy(ent.s.origin, claymore->s.origin);
	claymore->s.origin[2] += 24;
	claymore->gib_health = 200;//timer
	claymore->health = 10;
	claymore->think = claymoreThink;
	claymore->nextthink = level.time + 3.0;
	claymore->owner = &ent;
	claymore->targetEnt = &ent;
	claymore->takedamage = 1;
	claymore->die = claymore_die;
	VectorSet(claymore->s.angles, 270, 90, 90);
	claymore->clipmask = CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SHOT;
	VectorSet (claymore->mins, -4, -4, 0);
	VectorSet (claymore->maxs, 4, 4, 6);
	gi.linkentity(claymore);
	claymore->touch = claymoreTouch;
	claymore->flags = FL_THROWN_WPN;
	claymore->health = 1;
#endif
	return true;
}


bool DoWeHaveTheFlag(edict_t *ent)
{
	if (ent->ctf_flags)
		return true;
	return false;
}

