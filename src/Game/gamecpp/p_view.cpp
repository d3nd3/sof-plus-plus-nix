
#include "g_local.h"
#include "q_sh_interface.h"
#include "w_weapons.h"

#include "ai_body.h"
#include "ai_bodynoghoul.h"

#include "p_body.h"
#include "fields.h"

#include "g_monster.h"
#include "..\qcommon\ef_flags.h"

void G_UpdateFrameEffects(edict_t *ent);

static	edict_t		*current_player;
static	gclient_t	*current_client;

static	vec3_t	forward, Right, up;
float	xyspeed;

float	bobmove;
int		bobcycle;		// odd cycles are right foot going forward
float	bobfracsin;		// sin(bobfrac*M_PI)


/*
===============
SV_CalcRoll

===============
*/
float SV_CalcRoll (vec3_t angles, vec3_t velocity)
{
	float	sign;
	float	side;
	float	value;
	
	side = DotProduct (velocity, Right);
	sign = side < 0 ? -1 : 1;
	side = fabs(side);
	
	value = sv_rollangle->value;

	if (side < sv_rollspeed->value)
		side = side * value / sv_rollspeed->value;
	else
		side = value;
	
	return side*sign;
	
}

/*
===============
P_DamageFeedback

Handles color blends and view kicks
===============
*/

void P_ResetDamageFlags(edict_t *player)
{
	if (player->flags & FL_BODY_DAMAGE)
	{
		player->flags -= FL_BODY_DAMAGE;
	}
	if (player->flags & FL_ARMOR_DAMAGE)
	{
		player->flags -= FL_ARMOR_DAMAGE;
	}
}

extern void remove_camera(edict_t *Self);

void P_DamageFeedback (edict_t *player)
{
	gclient_t	*client;
	float		side;
	float		realcount, count, kick;
	int			r, l;
	static		vec3_t	bcolor={1.0,0.0,0.0};

	client = player->client;

	// Players must also update burning and the like.
	G_UpdateFrameEffects(player);

	// Total points of damage shot at the player this frame?

	if((count=client->damage_blood+client->damage_armor)==0)
	{
		// Didn't take any damage.

		return;
	}

	// Start a pain animation if still in the player model.
	// fixme:  when it is time to start worrying about player animation, check
	// the damage flags if we are going to distinguish between armor and body damage.
	if (client->anim_priority < ANIM_PAIN && player->s.modelindex == 255)
	{
		static int		i;

		client->anim_priority = ANIM_PAIN;

		//get forward...
		vec3_t dest, face;
		AngleVectors(client->ps.viewangles, face, NULL, NULL);
		VectorMA(player->s.origin, 100, face, face);
		VectorCopy(player->velocity, dest);
		VectorNormalize(dest);
		VectorMA(player->s.origin, 100, dest, dest);

		//best to do this elsewhere, so undamaged thrownback anim will play too --ss
/*
		PB_PlaySequenceForPain(player, vec3_origin, dest, face);
*/
	}

	// Play an apropriate pain sound.
	// fixme: distinguish between pain and armor sounds.
	if ((level.time > player->pain_debounce_time) && !(player->flags & FL_GODMODE))
	{
		r = 1 + (rand()&1);
		player->pain_debounce_time = level.time + 0.7;
		if (player->health < 25)
			l = 25;
		else if (player->health < 50)
			l = 50;
		else if (player->health < 75)
			l = 75;
		else
			l = 100;
//		gi.sound (player, CHAN_VOICE, gi.soundindex(va("*pain%i_%i.wav", l, r)), .6, ATTN_NORM, 0);
	}

	// Cap damage count.

	realcount=count;
	if(count<10)
		count=10;	// always make a visible effect

	// The total alpha of the blend is always proportional to count.

	if (client->damage_alpha < 0)
		client->damage_alpha = 0;
	client->damage_alpha += count*0.01;
	if (client->damage_alpha < 0.2)
		client->damage_alpha = 0.2;
	if (client->damage_alpha > 0.6)
		client->damage_alpha = 0.6;		// don't go too saturated

	// The blend will vary based on how much was absorbed by different armors.

	if(client->damage_blood)
		VectorScale(bcolor,(float)client->damage_blood/realcount,client->damage_blend);

	//
	// Calculate view angle kicks.
	//

	kick = abs(client->damage_knockback);

	if (kick && player->health > 0)	// kick of 0 means no view adjust at all
	{
		kick = kick * 100 / player->health;

		if (kick < count*0.5)
			kick = count*0.5; 
		if (kick > 50)
			kick = 50;

		vec3_t	v;

		VectorSubtract (client->damage_from, player->s.origin, v);
		VectorNormalize (v);
		
		side = DotProduct (v, Right);
		client->v_dmg_roll = -0.35*kick*side;
		
		side = -DotProduct (v, forward);
		client->v_dmg_pitch = 0.35*kick*side;

		client->v_dmg_time = level.time + DAMAGE_TIME;
	}

	//
	// Clear totals.
	//

	client->damage_blood = 0;
	client->damage_knockback = 0;
	client->damage_armor = 0;

	P_ResetDamageFlags (player);

	// If we're in a remote camera view, get out of it.

	if (client->ps.remote_id >= 0)
	{
		edict_t	*camera;

		camera = NULL;

		do
		{
			camera = G_Find(camera,FOFS(classname),"func_remote_camera");
			
			if(!camera)
			{	
				// No quiero un access violacion.

				break;
			}

			if (camera->s.number == client->ps.remote_id)
			{
				remove_camera(camera);
				break;
			}
		}while(1);
	}
}




/*
===============
SV_CalcViewOffset

Auto pitching on slopes?

  fall from 128: 400 = 160000
  fall from 256: 580 = 336400
  fall from 384: 720 = 518400
  fall from 512: 800 = 640000
  fall from 640: 960 = 

  damage = deltavelocity*deltavelocity  * 0.0001

===============
*/

edict_t *MakeLeanEnt(edict_t *ent)
{
	edict_t *newEnt, *tempEnt = NULL;

	newEnt = G_Spawn();

	newEnt->movetype = MOVETYPE_NONE;
	newEnt->solid = SOLID_NOT;
	newEnt->clipmask = MASK_MONSTERSOLID|MASK_PLAYERSOLID; 
	newEnt->svflags = (SVF_BUDDY|SVF_DEADMONSTER);
	VectorSet (newEnt->mins, -12, -12, -16);//?
	VectorSet (newEnt->maxs, 12, 12, 26);
	newEnt->s.modelindex = 0;
	gi.linkentity (newEnt);
	// we might already have a buddy stored in our owner's targetEnt
	tempEnt = ent;
	while (tempEnt->targetEnt && (tempEnt->targetEnt != ent))
	{
		tempEnt = tempEnt->targetEnt;	// owner keeps track of us
	}
	tempEnt->targetEnt = newEnt;
	newEnt->targetEnt = ent;	// keep track of our owner
	newEnt->flags |= FL_LEAN_PLAYER;
	newEnt->owner = ent;		// this will keep us from shooting our own buddy (very bad)

	return(newEnt);
}

edict_t *MakeNugBuddy(edict_t *ent)
{
	edict_t	*newEnt = G_Spawn(), *tempEnt = NULL;

	newEnt->movetype = MOVETYPE_NONE;
	newEnt->solid = SOLID_NOT;
	newEnt->clipmask = MASK_MONSTERSOLID|MASK_PLAYERSOLID; 
	newEnt->svflags = (SVF_BUDDY|SVF_DEADMONSTER);
	VectorSet (newEnt->mins, -12, -12, 0);
	VectorSet (newEnt->maxs, 12, 12, 8);
	newEnt->s.modelindex = 0;
	gi.linkentity (newEnt);
	// we might already have a buddy stored in our owner's targetEnt
	tempEnt = ent;
	while (tempEnt->targetEnt && (tempEnt->targetEnt != ent))
	{
		tempEnt = tempEnt->targetEnt;	// owner keeps track of us
	}
	tempEnt->targetEnt = newEnt;
	newEnt->targetEnt = ent;	// keep track of our owner
	newEnt->flags |= FL_NUGBUDDY;
	newEnt->owner = ent;		// this will keep us from shooting our own buddy (very bad)

	return newEnt;
}

edict_t *MakeWeaponBuddy(edict_t *ent)
{
	edict_t	*newEnt = G_Spawn(), *tempEnt = NULL;

	newEnt->movetype = MOVETYPE_NONE;
	newEnt->solid = SOLID_NOT;
	newEnt->clipmask = MASK_MONSTERSOLID|MASK_PLAYERSOLID; 
	newEnt->svflags = (SVF_BUDDY|SVF_DEADMONSTER);
	VectorSet (newEnt->mins, -12, -12, -4);
	VectorSet (newEnt->maxs, 12, 12, 4);
	newEnt->s.modelindex = 0;
	gi.linkentity (newEnt);
	// we might already have a buddy stored in our owner's targetEnt
	tempEnt = ent;
	while (tempEnt->targetEnt && (tempEnt->targetEnt != ent))
	{
		tempEnt = tempEnt->targetEnt;	// owner keeps track of us
	}
	tempEnt->targetEnt = newEnt;
	newEnt->targetEnt = ent;	// keep track of our owner
	newEnt->flags |= FL_WEAPONBUDDY;
	newEnt->owner = ent;		// this will keep us from shooting our own buddy (very bad)

	return newEnt;
}

edict_t *GetLeanBuddy(edict_t *owner)
{
	edict_t *lean = owner;

	while (lean->targetEnt && (lean->targetEnt != owner))
	{
		if (lean->targetEnt->flags & FL_LEAN_PLAYER)
		{
			return lean->targetEnt;
		}
		lean = lean->targetEnt;
	}
	// didn't have a lean buddy?!?
	return NULL;
}

edict_t *GetNugBuddy(edict_t *owner)
{
	edict_t *nug = owner;

	while (nug->targetEnt && (nug->targetEnt != owner))
	{
		if (nug->targetEnt->flags & FL_NUGBUDDY)
		{
			return nug->targetEnt;
		}
		nug = nug->targetEnt;
	}
	// didn't have a nug buddy?!?
	return NULL;
}

edict_t *GetWeaponBuddy(edict_t *owner)
{
	edict_t *wpn = owner;

	while (wpn->targetEnt && (wpn->targetEnt != owner))
	{
		if (wpn->targetEnt->flags & FL_WEAPONBUDDY)
		{
			return wpn->targetEnt;
		}
		wpn = wpn->targetEnt;
	}
	// didn't have a weapon buddy?!?
	return NULL;
}

void UpdateWeaponBuddy(edict_t *owner)
{
	edict_t		*wpn = owner?GetWeaponBuddy(owner):NULL;
	vec3_t		fwd, right, up, boxOrigin, muzzle, viewangles;
	float		boxBottom = 15,	// bottom of the bbox, distance from origin
				boxTop = 42,	// top of the box, dist from origin
				boxHalfWidth = 4,//half of the width of the box
				boxLength = 48; // length of the box


	if (NULL == wpn)
	{
		return;
	}
	VectorCopy(owner->client->ps.viewangles, viewangles);
	if (owner->client->ps.pmove.pm_flags & PMF_DUCKED)
	{
		boxBottom -= 16;
		boxTop -= 16;
		if (viewangles[0] > 28)
		{
			viewangles[0] = 28;
		}
		else if (viewangles[0] < -5)
		{
			viewangles[0] = -5;
		}
	}
	else
	{
		if (viewangles[0] > 28)
		{
			viewangles[0] = 28;
		}
		else if (viewangles[0] < -50)
		{
			viewangles[0] = -50;
		}
	}

	AngleVectors(viewangles, fwd, right, up);
	// for now, create one giant bbox that'll encompass all weapons, taking into account
	//standing and crouching. later, maybe break that down into smaller bboxes for standing vs. crouching,
	//for pistols vs. mach guns vs. launcher vs. sniper

	// consider the origin of the box to be the player's origin
	VectorCopy(owner->s.origin, boxOrigin);
	VectorCopy(boxOrigin, wpn->s.origin);
	// find the end of the weapon (basically) and build the box off of that and the origin
	VectorMA(boxOrigin, boxLength, fwd, muzzle);
	// find mins[2] and maxs[2] (tricky cuz player could be pointing up or down)
	if (fwd[2] > 0)
	{	// pointing up
		wpn->mins[2] = boxOrigin[2] + boxBottom;
		wpn->maxs[2] = muzzle[2]+32;
	}
	else
	{	// pointing down
		wpn->mins[2] = muzzle[2];
		wpn->maxs[2] = boxOrigin[2] + boxTop;
	}
	if (fwd[0] > 0)
	{	// x-value of the muzzle is our maxs[0]
		wpn->maxs[0] = muzzle[0];
		// x-value of our origin, then, is our mins[0]
		wpn->mins[0] = boxOrigin[0];
	}
	else
	{	// x-value of the muzzle is our mins[0]
		wpn->mins[0] = muzzle[0];
		// x-value of our origin, then, is our maxs[0]
		wpn->maxs[0] = boxOrigin[0];
	}
	if (fwd[1] > 0)
	{	// y-value of the muzzle is our maxs[1]
		wpn->maxs[1] = muzzle[1];
		// y-value of our origin, then, is our mins[1]
		wpn->mins[1] = boxOrigin[1];
	}
	else
	{	// y-value of the muzzle is our mins[1]
		wpn->mins[1] = muzzle[1];
		// y-value of our origin, then, is our maxs[1]
		wpn->maxs[1] = boxOrigin[1];
	}
	// we now have our bbox in worldspace coords. put it relative to our boxOrigin.
	VectorSubtract(wpn->maxs, boxOrigin, wpn->maxs);
	VectorSubtract(wpn->mins, boxOrigin, wpn->mins);
	gi.linkentity(wpn);
}

// returns the owner of any type of buddy
edict_t *GetBuddyOwner(edict_t *buddy)
{
	bool	bBuddy = buddy?( !!(buddy->svflags & SVF_BUDDY) ):false;
	edict_t *ent = buddy?buddy->targetEnt:NULL;

	if (!bBuddy)
	{
		return NULL;
	}
	while (ent)
	{
		if ( !(ent->svflags & SVF_BUDDY) )
		{	// found our owner
			return ent;
		}
		ent = ent->targetEnt;
	}
	// bad
	return NULL;
}

void RemoveLeanBuddy(edict_t *owner)
{
	edict_t *tempEnt = owner, *lean = NULL;

	while (tempEnt->targetEnt && (tempEnt->targetEnt != owner))
	{
		if (tempEnt->targetEnt->flags & FL_LEAN_PLAYER)
		{ //tempEnt->targetEnt is our lean buddy
			lean = tempEnt->targetEnt;
			if ((tempEnt == owner) && (lean == tempEnt))
			{
				owner->targetEnt = NULL;
			}
			else
			{
				tempEnt->targetEnt = lean->targetEnt;
			}
			G_FreeEdict(lean);
			return;
		}
		tempEnt = tempEnt->targetEnt;
	}
	// didn't have a lean buddy
}

void RemoveNugBuddy(edict_t *owner)
{
	edict_t *tempEnt = owner, *nug = NULL;

	while (tempEnt->targetEnt && (tempEnt->targetEnt != owner))
	{
		if (tempEnt->targetEnt->flags & FL_NUGBUDDY)
		{ //tempEnt->targetEnt is our NugBuddy
			nug = tempEnt->targetEnt;
			if ((tempEnt == owner) && (nug == tempEnt))
			{
				owner->targetEnt = NULL;
			}
			else
			{
				tempEnt->targetEnt = nug->targetEnt;
			}
			G_FreeEdict(nug);
			return;
		}
		tempEnt = tempEnt->targetEnt;
	}
	// didn't have a nug buddy
}

void RemoveWeaponBuddy(edict_t *owner)
{
	edict_t *tempEnt = owner, *wpn = NULL;

	while (tempEnt->targetEnt && (tempEnt->targetEnt != owner))
	{
		if (tempEnt->targetEnt->flags & FL_WEAPONBUDDY)
		{ //tempEnt->targetEnt is our WeaponBuddy
			wpn = tempEnt->targetEnt;
			if ((tempEnt == owner) && (wpn == tempEnt))
			{
				owner->targetEnt = NULL;
			}
			else
			{
				tempEnt->targetEnt = wpn->targetEnt;
			}
			G_FreeEdict(wpn);
			return;
		}
		tempEnt = tempEnt->targetEnt;
	}
	// didn't have a wpn buddy
}

void debug_drawbox(edict_t* self,vec3_t vOrigin, vec3_t vMins, vec3_t vMaxs, int nColor);

void SV_CalcViewOffset1stPerson(edict_t *ent)
{
	float		*angles;
	float		bob;
	float		ratio;
	float		delta;
	vec3_t		v;
	vec3_t		fwd, right, up, test, test2, test3;
	vec3_t		min = {-8, -8, -8};
	vec3_t		max = {8, 8, 8};
	trace_t		tr;

	//===================================
	// Calculate kick_angles and weapon_kick angles.
	//===================================

	angles = ent->client->ps.kick_angles;
	
	VectorClear (angles);

	if (ent->deadflag)
	{
		// If dead, fix the angle and don't add any weapon kick.

		VectorClear (ent->client->ps.weaponkick_angles);

		ent->client->ps.viewangles[ROLL] = 40;
		ent->client->ps.viewangles[PITCH] = -15;
		ent->client->ps.viewangles[YAW] = ent->client->killer_yaw;
	}
	else
	{
		// Calc angles from weapon kick.

		VectorCopy (ent->client->weaponkick_angles,ent->client->ps.weaponkick_angles);

		// Add angles based on damage kick.
		ratio = (ent->client->v_dmg_time - level.time) / DAMAGE_TIME;
		if (ratio < 0)
		{
			ratio = 0;
			ent->client->v_dmg_pitch = 0;
			ent->client->v_dmg_roll = 0;
		}

		angles[PITCH] += ratio * ent->client->v_dmg_pitch;
		angles[ROLL] += ratio * ent->client->v_dmg_roll;

		// Add pitch based on fall kick.

		ratio = (ent->client->fall_time - level.time) / FALL_TIME;
		if (ratio < 0)
			ratio = 0;
		angles[PITCH] += ratio * ent->client->fall_value;

		// Add angles based on velocity.

		delta = DotProduct (ent->velocity, forward);
		angles[PITCH] += delta*run_pitch->value;
		
		delta = DotProduct (ent->velocity, Right);
		angles[ROLL] += delta*run_roll->value;

		// Add angles based on bob.

		delta = bobfracsin * bob_pitch->value * xyspeed;
		if ((ent->client->ps.pmove.pm_flags & (PMF_DUCKED|PMF_ON_GROUND))== (PMF_DUCKED|PMF_ON_GROUND))
			delta *= 6;		// crouching
		angles[PITCH] += delta;
		delta = bobfracsin * bob_roll->value * xyspeed;
		if ((ent->client->ps.pmove.pm_flags & (PMF_DUCKED|PMF_ON_GROUND))== (PMF_DUCKED|PMF_ON_GROUND))
			delta *= 6;		// crouching
		if (bobcycle & 1)
			delta = -delta;
		angles[ROLL] += delta;
	}

	//===================================
	// Calculate kick_origin.
	//===================================

	// Base origin.

	VectorClear (v);

	// Add view height.

	v[2] += ent->viewheight;

	// Add fall height.

	ratio = (ent->client->fall_time - level.time) / FALL_TIME;
	if (ratio < 0)
		ratio = 0;
	v[2] -= ratio * ent->client->fall_value * 0.4;

	// Add bob height.

	bob = bobfracsin * xyspeed * bob_up->value;
	if (bob > 6)
		bob = 6;
	
	//gi.DebugGraph (bob *2, 255);
	
	v[2] += bob;

	// Add kick offset.

	VectorAdd (v, ent->client->kick_origin, v);

	// Absolutely bound offsets so that the view can never be outside the player box.

	if (v[0] < -14)
		v[0] = -14;
	else if (v[0] > 14)
		v[0] = 14;
	if (v[1] < -14)
		v[1] = -14;
	else if (v[1] > 14)
		v[1] = 14;
	if (v[2] < -11)
		v[2] = -11;
	else if (v[2] > 40)
		v[2] = 40;

	//===================================
	// Handle player leaning left / right - alters kick_angles[ROLL].
	//===================================

	AngleVectors(ent->client->ps.viewangles, fwd, right, up);

	if((ent->viewside)&&(ent->groundentity)&&(DotProduct(ent->velocity, ent->velocity) < 30.0)&&(ent->health > 0))
	{
		// Can only do this while not moving quickly.

		VectorMA(v, -30 * ent->viewside, right, test);
		VectorMA(test, -5, up, test);
		VectorAdd(ent->s.origin, v, test2);
		VectorAdd(ent->s.origin, test, test3);
		gi.trace (test2, min, max, test3, ent, MASK_PLAYERSOLID, &tr);
		VectorCopy(tr.endpos, v);
		VectorSubtract(v, ent->s.origin, v);

		angles[ROLL] -= 8*tr.fraction * ent->viewside;
	}

	if(ent->viewside)
	{	
		edict_t *lean = GetLeanBuddy(ent);
		if(lean)
		{
			// If we're leaning already, update our buddy.
			VectorCopy(tr.endpos, lean->s.origin);
			gi.linkentity(lean);
		}
		else
		{
			// Otherwise make our buddy.
			lean = MakeLeanEnt(ent);
		}

	}
	else
	{
		if(GetLeanBuddy(ent))
		{
			// No more leaning - destroy the buddy.
			RemoveLeanBuddy(ent);
		}
	}

	// 1/4/00 kef -- we need a buddy on top of our bbox if we're crouched. and only create it for deathmatch.
#if 1 // keith has fixed. let him know if something further needs to be done.
	if (dm->isDM() && ent && ent->client)
	{
		if (ent->deadflag != DEAD_DEAD)//ent->health > 0)
		{
			// nug buddy (for crouching)
			if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
			{
				// we're crouching in deathmatch. if we already have a lean buddy, store our NugBuddy(TM) in our
				//lean buddy's targetEnt. otherwise, store it in _our_ targetEnt.
				edict_t *nug = GetNugBuddy(ent);
				if (nug)
				{	// already have a nug buddy. update its position.
					vec3_t fwd;
					AngleVectors(ent->s.angles, fwd, NULL, NULL);
					VectorCopy(ent->s.origin, nug->s.origin);
					nug->s.origin[2] += ent->maxs[2];
					VectorMA(nug->s.origin, 8, fwd, nug->s.origin);
					gi.linkentity(nug);
				}
				else
				{	// don't have a nug buddy yet. make one.
					nug = MakeNugBuddy(ent);
					// put it where we want it
					vec3_t fwd;
					AngleVectors(ent->s.angles, fwd, NULL, NULL);
					VectorCopy(ent->s.origin, nug->s.origin);
					nug->s.origin[2] += ent->maxs[2];
					VectorMA(nug->s.origin, 8, fwd, nug->s.origin);
					gi.linkentity(nug);
				}
			}
			else
			{
				// we aren't crouching now, but if we were last frame, we need to get rid of our NugBuddy(TM)
				if (GetNugBuddy(ent))
				{	// already have a nug buddy. make it go away.
					RemoveNugBuddy(ent);
				}
			}

			// weapon buddy (always on)
			edict_t *wpn = GetWeaponBuddy(ent);
			if (wpn)
			{	// we've already got a weapon buddy...update its position
				if (ent->client->pers.spectator)
				{	// no weapon buddy for spectators
					RemoveWeaponBuddy(ent);
				}
				else
				{
					UpdateWeaponBuddy(ent);
				}
			}
			else if (!ent->client->pers.spectator)
			{	// need to create a weapon buddy
				MakeWeaponBuddy(ent);
				UpdateWeaponBuddy(ent);
			}
		} // if (ent->health > 0)
	}// if (dm->isDM() && ent && ent->client)
#endif // keith has fixed. let him know if something further needs to be done.

	// if we're dead, remove all buddies
	if (ent->deadflag == DEAD_DEAD)//ent->health <= 0)
	{
		// make sure our goggles are off
		ent->client->goggles_on = false;

		RemoveLeanBuddy(ent);
		RemoveNugBuddy(ent);
		RemoveWeaponBuddy(ent);
	}
/*	else
	{
		if (!(ent->flags & FL_GODMODE))
		{
			if (GetLeanBuddy(ent))
			{
				debug_drawbox(GetLeanBuddy(ent), NULL, NULL, NULL, 0xFFFFFFFF);
			}
			if (GetWeaponBuddy(ent))
			{
				debug_drawbox(GetWeaponBuddy(ent), NULL, NULL, NULL, 0xFFFFFFFF);
			}
			if (GetNugBuddy(ent))
			{
				debug_drawbox(GetNugBuddy(ent), NULL, NULL, NULL, 0xFFFFFFFF);
			}
		}
	}
*/

/*
	// Removed this stuff at RJ's request. It needs to be re-implemented if it's going
	// to work properly with prediction and also, not clip through walls. -MW

	//===================================
	// Handle pivoting around waist when looking down - sets viewoffset.
	//===================================

  	float	pitchLen;

	fwd[2] = 0;
	VectorNormalize(fwd);

	if ((ent->client->ps.pmove.pm_flags & (PMF_DUCKED|PMF_ON_GROUND))== (PMF_DUCKED|PMF_ON_GROUND))
	{
		pitchLen = 12;
	}
	else
	{
		pitchLen = 24;
	}

	float scale = .05 + ((ent->client->ps.viewangles[PITCH]+90)/180)*.95;

	pitchLen *= scale;
	
	VectorCopy(v, test);
	test[2] -= pitchLen;
	VectorMA(test, sin(ent->client->ps.viewangles[PITCH] * M_PI/180) * pitchLen, fwd, test);
	test[2] += cos(ent->client->ps.viewangles[PITCH] * M_PI/180) * pitchLen;
	VectorAdd(ent->s.origin, v, test2);
	VectorAdd(ent->s.origin, test, test3);
	gi.trace (test2, min, max, test3, ent, MASK_PLAYERSOLID, &tr);
	VectorCopy(tr.endpos, v);
	VectorSubtract(v, ent->s.origin, v);
*/

	VectorCopy (v, ent->client->ps.viewoffset);
	//Com_Printf("viewoffset is: %f, %f, %f\n", v[0], v[1], v[2]);
}

void SV_CalcViewOffset3rdPerson(edict_t *ent)
{
	edict_t			*cameraSubject;
	vec3_t			baseAngles,viewAngles;
	static float	oldYAW;
	vec3_t			fwd,right,up;
	vec3_t			focusLoc,cameraOffset,viewOrg,tempVec;
	static vec3_t	oldVieworg;
	trace_t			trace;
	vec3_t			mins={-1.0,-1.0,-1.0},maxs={1.0,1.0,1.0};

	//
	// Retrieve the entity that our 3rd person camera is chasing... the subject.
	//
	
	if(followenemy->value >= 0)
	{
		static int lastFollow = -1;
		static edict_t *lastSubject = 0;//uh oh...

		int stopChecking = 0;

		if(lastFollow == followenemy->value)
		{	//unsafe!!!
			if(lastSubject && lastSubject->inuse && (lastSubject->health > 0) && lastSubject->ai)
			{
				cameraSubject = lastSubject;
				stopChecking = 1;
			}
			else
			{
				cameraSubject = 0;
			}
		}
		
		if(!stopChecking)
		{
			int targs = 1;
			cameraSubject=0;
			for(int i = 0; (i < followenemy->value) && (targs); i++)
			{
				edict_t *start = cameraSubject;
				do
				{
					cameraSubject=G_Find (cameraSubject, FOFS(classname), "m_", 2);
					if(cameraSubject == start)
					{	// no infinite loop for you!  One year!
						cameraSubject = 0;
						targs = 0;
						break;
					}
				}while(!cameraSubject || (cameraSubject->health < 1) || (!cameraSubject->ai) || 
					(!(cameraSubject->ai->IsActive())) || (!cameraSubject->ai->HasHadTarget()));

			}
			lastFollow = followenemy->value;
			lastSubject = cameraSubject;
		}

		if(!cameraSubject)
		{
			cameraSubject = NULL;
			ent->client->CameraIs3rdPerson = false;
			ent->client->ps.remote_type = REMOTE_TYPE_FPS;
			ent->client->ps.remote_id = -1;
			ent->client->MercCameraNumber = 0;
			followenemy->value = 0;
			return;
		}
		else
		{
			cameraSubject->ai->WatchMe();
		}
		
	}
	else if (thirdpersoncam->value)
	{
			cameraSubject = ent;
	}
	else
	{
			cameraSubject = NULL;
			ent->client->CameraIs3rdPerson = false;
			ent->client->ps.remote_type = REMOTE_TYPE_FPS;
			ent->client->ps.remote_id = -1;
			ent->client->MercCameraNumber = 0;
			return;
	}

	if(!cameraSubject)
		cameraSubject=ent;

	//
	// Derive a camera offset from the player's mouse PITCH and our subject's YAW facing.
	//

	// PITCH.

	baseAngles[PITCH]=ent->client->ps.viewangles[PITCH];

	// YAW.

	baseAngles[YAW]=LerpAngle(oldYAW,cameraSubject->s.angles[YAW],0.25);
	oldYAW=baseAngles[YAW];

	// ROLL.

	baseAngles[ROLL]=0.0;
	
	// And finally, get the offset.

	AngleVectors(baseAngles,fwd,right,up);
	VectorScale(fwd,-camera_viewdist->value,cameraOffset);

	//
	// Calculate the focus point.
	//

	VectorCopy(cameraSubject->s.origin,focusLoc);
	focusLoc[2]+=cameraSubject->viewheight;

	//
	// Calculate the viewpoint after damping.
	//
	
	VectorAdd(focusLoc,cameraOffset,viewOrg);
	VectorSubtract(viewOrg,oldVieworg,tempVec);

	//making it snap to position faster, so it'll keep up --ss
	VectorScale(tempVec,0.5,tempVec);
//	VectorScale(tempVec,0.25,tempVec);

	VectorAdd(oldVieworg,tempVec,viewOrg);

	//
	// Adjust the viewpoint to prevent the LOS from being obscured by objects.
	//

	gi.trace(focusLoc,
			 mins,
			 maxs,
			 viewOrg,
			 cameraSubject,
			 MASK_SOLID,
			 &trace);

	if(trace.fraction!=1.0)
	{
		VectorCopy(trace.endpos,viewOrg);
	}

	//
	// Calculate and store away the viewangles.
	//

	VectorCopy(cameraOffset,tempVec);
	tempVec[0]=-tempVec[0];
	tempVec[1]=-tempVec[1];
	VectorNormalize(tempVec);
	vectoangles(tempVec,viewAngles);
	viewAngles[PITCH]=-viewAngles[PITCH];
	VectorCopy(viewAngles,ent->client->ps.remote_viewangles);

	//
	// Store away the the viewpoint.
	//

	VectorScale(viewOrg,8.0,ent->client->ps.remote_vieworigin);
	VectorCopy(viewOrg,oldVieworg);
}

void SV_CalcViewOffset (edict_t *ent)
{	
	//for testing right now - 1 and -1 will toggle this on and off, but it would definitely break other stuff...
	if(followenemy->value >= 1)
	{
		ent->client->CameraIs3rdPerson = 1;
		ent->client->ps.remote_type = REMOTE_TYPE_TPS;
	}
	else if(followenemy->value == -1)
	{
		if (thirdpersoncam->value)
		{
			ent->client->CameraIs3rdPerson = 1;
			ent->client->ps.remote_type = REMOTE_TYPE_TPS;
		}
		else
		{
			ent->client->CameraIs3rdPerson = 0;
			ent->client->ps.remote_type = REMOTE_TYPE_FPS;
		}
	}

	if(!ent->client->CameraIs3rdPerson)
	{
		SV_CalcViewOffset1stPerson(ent);
	}
	else
	{
		//yech.  i need viewoffset set, and i'm too lazy to dig around for it.
		SV_CalcViewOffset1stPerson(ent);

		SV_CalcViewOffset3rdPerson(ent);
	}
}

/*
=============
SV_AddBlend
=============
*/
void SV_AddBlend (float r, float g, float b, float a, float *v_blend)
{
	float	a2, a3;

	if (a <= 0)
		return;
	a2 = v_blend[3] + (1-v_blend[3])*a;	// new total alpha
	a3 = v_blend[3]/a2;		// fraction of color from old

	v_blend[0] = v_blend[0]*a3 + r*(1-a3);
	v_blend[1] = v_blend[1]*a3 + g*(1-a3);
	v_blend[2] = v_blend[2]*a3 + b*(1-a3);
	v_blend[3] = a2;
}

/*
=============
SV_CalcBlend
=============
*/
void SV_CalcBlend (edict_t *ent)
{
	int		contents;
	vec3_t	vieworg;
	static float murkiness = 0;

	if(!(rand()%3))
	{	// this should be vel driven...
		murkiness += gi.flrand(-0.15F, 0.15F);
		if(murkiness < 0)murkiness = 0;
		if(murkiness > 1.0)murkiness = 1.0;
	}

	ent->client->ps.blend[0] = ent->client->ps.blend[1] = 
		ent->client->ps.blend[2] = ent->client->ps.blend[3] = 0;

	// add for contents
	if (ent->client->ps.remote_type == REMOTE_TYPE_FPS)
	{
		VectorAdd (ent->s.origin, ent->client->ps.viewoffset, vieworg);
		contents = gi.pointcontents (vieworg);
	}
	else if (ent->client->ps.remote_type == REMOTE_TYPE_TPS)
	{
		contents = 0;
	}
	else
	{
		contents = gi.pointcontents (ent->s.origin);
	}

	if (contents & (CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER) )
		ent->client->ps.rdflags |= RDF_UNDERWATER;
	else
		ent->client->ps.rdflags &= ~RDF_UNDERWATER;

	if ((ent->client->pers.spectator) && (contents & (CONTENTS_LAVA)))
		SV_AddBlend (1.0, 0.3, 0.0, 0.6, ent->client->ps.blend);
	else if (!(ent->client->pers.spectator) && (contents & (CONTENTS_SOLID|CONTENTS_LAVA)))
		SV_AddBlend (1.0, 0.3, 0.0, 0.6, ent->client->ps.blend);
	else if (contents & CONTENTS_SLIME)
		SV_AddBlend (0.0, 0.1, 0.05, 0.6, ent->client->ps.blend);
	else if (contents & CONTENTS_WATER)
		//SV_AddBlend (0.03, 0.08, 0.03, .85 + murkiness*.1, ent->client->ps.blend);
		SV_AddBlend (0.03, 0.08, 0.03, .2, ent->client->ps.blend);

	// add for damage
	if (ent->client->damage_alpha > 0)
		SV_AddBlend (ent->client->damage_blend[0],ent->client->damage_blend[1]
		,ent->client->damage_blend[2], ent->client->damage_alpha, ent->client->ps.blend);

	if (ent->client->bonus_alpha > 0)
		SV_AddBlend (0.85, 0.7, 0.3, ent->client->bonus_alpha, ent->client->ps.blend);

	if (ent->client->blinding_alpha > 0)
	{
		// The color of the blend should be different with goggles than without.
		if (ent->client->goggles_on)
		{
			SV_AddBlend (0.5, 1.0, 0.5, ent->client->blinding_alpha, ent->client->ps.blend);
		}
		else
		{
			SV_AddBlend (1.0, 1.0, 1.0, ent->client->blinding_alpha, ent->client->ps.blend);
		}
	}

	if (ent->client->fade_alpha > 0)
	{
		SV_AddBlend (ent->client->fade_rgb[0], ent->client->fade_rgb[1], ent->client->fade_rgb[2],
			ent->client->fade_alpha, ent->client->ps.blend);
	}

	if (ent->client->gas_blend)
	{
		SV_AddBlend(198.0/255.0, 171/255.0, 20.0/255.0, ent->client->gas_blend, ent->client->ps.blend);
	}

	if (ent->health < 0)
	{
		float addVal = -ent->health / 100.0;

//		SV_AddBlend(0, 0, 0, addVal, ent->client->ps.blend);

		ent->health -= 2;
		if(ent->health < -100)ent->health = -100;//is this safe at all?
	}

	// drop the damage value
	ent->client->damage_alpha -= 0.06;
	if (ent->client->damage_alpha < 0)
		ent->client->damage_alpha = 0;

	// drop the bonus value
	ent->client->bonus_alpha -= 0.1;
	if (ent->client->bonus_alpha < 0)
		ent->client->bonus_alpha = 0;

	if (ent->client->blinding_alpha > 0.995)
		ent->client->blinding_alpha = 0.995;

	// The delta is applied in such a way that the alpha fades slowly when the screen is almost white, 
	// then quicker as it approaches zero.
	ent->client->blinding_alpha -= (1.0 - ent->client->blinding_alpha) * ent->client->blinding_alpha_delta;

	if (ent->client->blinding_alpha < 0)
	{
		ent->client->blinding_alpha = 0;
	}

	ent->client->gas_blend *= .88;
	if(ent->client->gas_blend < .3)
	{
		ent->client->gas_blend -= .06;
	}
	if (ent->client->gas_blend < 0)
	{
		ent->client->gas_blend = 0;
	}

	// Add the goggles render flag if the goggles are on, and we are not in a remote or cinematic camera.
	if (ent->client->goggles_on && ent->client->ps.remote_type <= REMOTE_TYPE_TPS)
	{
		ent->client->ps.rdflags |= RDF_GOGGLES;
	}
	else
	{
		ent->client->ps.rdflags &= ~RDF_GOGGLES;
	}
}


/*
=================
P_FallingDamage
=================
*/
void P_FallingDamage (edict_t *ent)
{
	float	delta;
	int		damage;
	vec3_t	dir;

	if (ent->s.modelindex != 255)
		return;		// not in the player model

	if (ent->movetype == MOVETYPE_NOCLIP)
		return;

	if ((ent->client->oldvelocity[2] < 0) && (ent->velocity[2] > ent->client->oldvelocity[2]) && (!ent->groundentity))
	{
		delta = ent->client->oldvelocity[2];
	}
	else
	{
		if (!ent->groundentity)
			return;
		delta = ent->velocity[2] - ent->client->oldvelocity[2];
	}
	delta = delta*delta * 0.0001;

	// never take falling damage if completely underwater
	if (ent->waterlevel == 3)
		return;
	if (ent->waterlevel == 2)
		delta *= 0.25;
	if (ent->waterlevel == 1)
		delta *= 0.5;

	if (delta < 1)
		return;

	//just put this in for singleplayer, so that client prediction doesn't have to be involved
	if(!dm->isDM())
	{
		sharedEdict_t sh;

		sh.inv = (inven_c *)ent->client->inv;
		sh.edict = ent;
		sh.attack = (ent->client->latched_buttons|ent->client->buttons)&BUTTON_ATTACK;
		sh.altattack = (ent->client->latched_buttons|ent->client->buttons)&BUTTON_ALTATTACK;

		sh.inv->setOwner(&sh);

		sh.inv->handleFall();
	}

	// make sure force feedback does landing effect
	fxRunner.exec("land", ent);

	if (delta < 15)
	{
		FX_SetEvent(ent, EV_FALLSHORT);
		return;
	}

	ent->client->fall_value = delta*0.5;
	if (ent->client->fall_value > 40)
		ent->client->fall_value = 40;
	ent->client->fall_time = level.time + FALL_TIME;

	if (delta > 12)
	{
		if (ent->health > 0)
		{
			if (delta >= 55)
				FX_SetEvent(ent, EV_FALLFAR);
			else
				FX_SetEvent(ent, EV_FALL);
		}
		ent->pain_debounce_time = level.time;	// no normal pain sound
		damage = (delta-24)*2;
		if(damage > 0)
		{
			if (damage < 1)
				damage = 1;
			VectorSet (dir, 0, 0, 1);

			if (!dm->isDM() || (dm->dmRule_REALISTIC_DAMAGE() || !dm->dmRule_NO_FALLING()) )
			{
				T_Damage (ent, world, world, dir, ent->s.origin, ent->s.origin, damage, 0, DAMAGE_NO_ARMOR|DT_FALL, MOD_FALLING);
			}
		}
	}
	else
	{
		FX_SetEvent(ent, EV_FALLSHORT);
		return;
	}
}

/*
=============
PlayerNoise
=============
*/

int w_lastNoiseTime = 0;

void PlayerNoise(edict_t *who, vec3_t where, ai_sensetype_e type, edict_t *focus, float rad, int nodeId, int stealthy, int suppress)
{
	edict_t		*ent=NULL;

	if (dm->isDM())
		return;

	if (who->flags & FL_NOTARGET)
		return;

	if (type == AI_SENSETYPE_SOUND_WHIZ)
	{
		if (focus)
		{
			gmonster.MakeSound(focus,where,level.time,who,type);
		}
	}
	else
	{
		CRadiusContent radList(where, rad, 1, nodeId);

		for(int i = 0; i < radList.getNumFound(); i++)
		{
			ent = radList.foundEdict(i);

			if(!ent->ai)
			{
				continue;
			}
			if(ent->deadflag == DEAD_DEAD)
			{
				continue;
			}

			if(stealthy)
			{	//these are really more of sight events, but the sound sense still models it better
				edict_t *check = who;

				if(focus)
				{
					check = focus;
				}
				if(!gmonster.Infront(ent, check))
				{
					continue;
				}
				if(!gmonster.Visible(ent, check))
				{
					continue;
				}
			}

			gmonster.MakeSound(ent,where,level.time,who,type);

			if(suppress)
			{
				ent->ai->RegisterShotForReply(who->s.origin, who->s.origin, who);
			}
		}

		//now check for sound triggers as well - stealthy won't work on these... maybe add some more rules here?
		if(!stealthy)
		{
			for(i = gmonster.GetActiveSoundTriggers() - 1; i > -1; i--)
			{	//warning - saves?
				edict_t *test = &g_edicts[gmonster.GetSoundTrigger(i)];

				if(test&&test->use)
				{	//this should remove the sound trigger
					vec3_t dif;
					VectorSubtract(test->s.origin, where, dif);
					if(VectorLengthSquared(dif) < test->health)
					{
						test->use(test, ent, ent);
					}
				}
			}
		}
	}

	w_lastNoiseTime = 0;
}

/*
=============
P_WorldEffects
=============
*/
void P_WorldEffects (void)
{
	int			waterlevel, old_waterlevel;

	if (current_player->movetype == MOVETYPE_NOCLIP)
	{
		current_player->air_finished = level.time + 12;	// don't need air
		return;
	}

	waterlevel = current_player->waterlevel;
	old_waterlevel = current_client->old_waterlevel;
	current_client->old_waterlevel = waterlevel;

	//
	// if just entered a water volume, play a sound
	//
	if (!old_waterlevel && waterlevel)
	{
//		PlayerNoise(current_player, current_player->s.origin, PNOISE_SELF);
/*		if (current_player->watertype & CONTENTS_LAVA)
			gi.sound (current_player, CHAN_BODY, gi.soundindex("player/lava_in.wav"), .6, ATTN_NORM, 0);
		else if (current_player->watertype & CONTENTS_SLIME)
			gi.sound (current_player, CHAN_BODY, gi.soundindex("player/watr_in.wav"), .6, ATTN_NORM, 0);
		else if (current_player->watertype & CONTENTS_WATER)
*/		{
			gi.sound (current_player, CHAN_BODY, gi.soundindex("player/land/water.wav"), .6, ATTN_NORM, 0);
		}
		current_player->flags |= FL_INWATER;

		// clear damage_debounce, so the pain sound will play immediately
		current_player->damage_debounce_time = level.time - 1;
	}

	//
	// if just completely exited a water volume, play a sound
	//
	if (old_waterlevel && ! waterlevel)
	{
//		PlayerNoise(current_player, current_player->s.origin, PNOISE_SELF);
//		gi.sound (current_player, CHAN_BODY, gi.soundindex("player/watr_out.wav"), .6, ATTN_NORM, 0);
		current_player->flags &= ~FL_INWATER;
	}

	//
	// check for head just going under water
	//
	if (old_waterlevel != 3 && waterlevel == 3)
	{
//		gi.sound (current_player, CHAN_BODY, gi.soundindex("player/underwtr/underwtr.wav"), .6, ATTN_NORM, 0);//?

		// kef -- don't start the underwater sound if we've already got a looping sound playing.
		if (0 == current_player->s.sound)
		{
			current_player->s.sound = gi.soundindex("player/underwtr/underwtr.wav");
			current_player->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		}
	}

	//
	// check for head just coming out of water
	//
	if (old_waterlevel == 3 && waterlevel != 3)
	{
		// kef -- we need to kill the sound we started when we entered the water
		if (current_player->s.sound == gi.soundindex("player/underwtr/underwtr.wav"))
		{
			current_player->s.sound = 0;
		}
		if (current_player->air_finished < level.time)
		{	// gasp for air
//			gi.sound (current_player, CHAN_VOICE, gi.soundindex("player/gasp1.wav"), .6, ATTN_NORM, 0);
//			PlayerNoise(current_player, current_player->s.origin, PNOISE_SELF);
		}
		else  if (current_player->air_finished < level.time + 11)
		{	// just break surface
//			gi.sound (current_player, CHAN_VOICE, gi.soundindex("player/gasp2.wav"), .6, ATTN_NORM, 0);
		}
	}

	//
	// check for drowning
	//
	if (waterlevel == 3)
	{
		// if out of air, start drowning
		if (current_player->air_finished < level.time)
		{	// drown!
			if (current_player->client->next_drown_time < level.time 
				&& current_player->health > 0)
			{
				current_player->client->next_drown_time = level.time + 1;

				// take more damage the longer underwater
				current_player->dmg += 2;
				if (current_player->dmg > 15)
					current_player->dmg = 15;

				// play a gurp sound instead of a normal pain sound
				if (current_player->health <= current_player->dmg)
					gi.sound (current_player, CHAN_VOICE, gi.soundindex("player/drown1.wav"), .6, ATTN_NORM, 0);
//				else if (rand()&1)
//					gi.sound (current_player, CHAN_VOICE, gi.soundindex("*gurp1.wav"), .6, ATTN_NORM, 0);
//				else
//					gi.sound (current_player, CHAN_VOICE, gi.soundindex("*gurp2.wav"), .6, ATTN_NORM, 0);

				current_player->pain_debounce_time = level.time;

				T_Damage (current_player, world, world, vec3_origin, current_player->s.origin, current_player->s.origin, current_player->dmg, 0, DAMAGE_NO_ARMOR|DT_WATER, MOD_WATER);
			}
		}
	}
	else
	{
		current_player->air_finished = level.time + 12;
		current_player->dmg = 2;
	}

	//
	// check for sizzle damage
	//
	if (waterlevel && (current_player->watertype&(CONTENTS_LAVA|CONTENTS_SLIME)) )
	{
		if (current_player->watertype & CONTENTS_LAVA)
		{
			if (current_player->health > 0
				&& current_player->pain_debounce_time <= level.time)
			{
				char meatSound[100];
				Com_sprintf(meatSound, sizeof(meatSound), "impact/player/hit%d.wav", gi.irand(1,4));
				gi.sound (current_player, CHAN_VOICE, gi.soundindex(meatSound), 1.0, ATTN_NORM, 0);

				current_player->pain_debounce_time = level.time + 1;
			}

			T_Damage (current_player, world, world, vec3_origin, current_player->s.origin, current_player->s.origin, 3*waterlevel, 0, DT_FIRE, MOD_LAVA);
		}

		if (current_player->watertype & CONTENTS_SLIME)
		{
			T_Damage (current_player, world, world, vec3_origin, current_player->s.origin, current_player->s.origin, 1*waterlevel, 0, DT_WATER, MOD_SLIME);
		}
	}
}


/*
===============
G_SetClientEffects
===============
*/
void G_SetClientEffects (edict_t *ent)
{

	// Can somebody tell me why this was done?  --Pat
//	ent->s.effects = 0;


//	ent->s.renderfx = 0;

	if (ent->health <= 0 || level.intermissiontime)
		return;

	// show cheaters!!!
	if (ent->flags & FL_GODMODE)
	{
	}
}


/*
===============
G_SetClientEvent
===============
*/
void G_SetClientEvent (edict_t *ent)
{
	int newEvent;

	if(ent->groundentity)
	{
		if((int)(current_client->bobtime+bobmove) != bobcycle)
		{
			newEvent = EV_FOOTSTEPLEFT;
			if(xyspeed > 225)newEvent += 2;
			if(bobcycle & 1)newEvent += 1;
			FX_SetEvent(ent, newEvent);
		}
	}
}

/*
===============
G_SetClientSound
===============
*/
void G_SetClientSound (edict_t *ent)
{
	char	*weap;

	// fixme - this crap should be obliterated

	if (ent->client->resp.game_helpchanged != game.helpchanged)
	{
		ent->client->resp.game_helpchanged = game.helpchanged;
		ent->client->resp.helpchanged = 1;
	}

	// help beep (no more than three times)
	if (ent->client->resp.helpchanged && ent->client->resp.helpchanged <= 3 && !(level.framenum&63) )
	{
		ent->client->resp.helpchanged++;
		gi.sound (ent, CHAN_VOICE, gi.soundindex ("misc/pc_up.wav"), .6, ATTN_STATIC, 0);
	}


	weap = "";

	// why was this here? No looping sounds would have ever worked with this here.
//	ent->s.sound = 0;
}

/*
===============
G_SetClientFrame
===============
*/
#define FRAME_DeathB03        	104
#define FRAME_DeathB08        	109

void G_SetClientFrame (edict_t *ent)
{
	gclient_t	*client;
	qboolean	duck, run;

	if (ent->s.modelindex != 255)
		return;		// not in the player model

	client = ent->client;

	if (client->ps.pmove.pm_flags & PMF_DUCKED)
		duck = true;
	else
		duck = false;
	if (xyspeed)
		run = true;
	else
		run = false;

	// check for stand/duck and stop/go transitions
	if (duck != client->anim_duck && client->anim_priority < ANIM_DEATH)
		goto newanim;
	if (run != client->anim_run && client->anim_priority == ANIM_BASIC)
		goto newanim;
	if (!ent->groundentity && client->anim_priority <= ANIM_WAVE)
		goto newanim;

	if (ent->s.frame < client->anim_end)
	{	// continue an animation
		ent->s.frame++;
		return;
	}

	if (client->anim_priority == ANIM_DEATH)
	{
		PB_PlaySequenceForDeath(ent, NULL, NULL, 0, ent->s.origin, 0);
		return;		// stay there
	}
	if (client->anim_priority == ANIM_JUMP)
	{
		if (!ent->groundentity)
			return;		// stay there
		ent->client->anim_priority = ANIM_WAVE;
		ent->s.frame = FRAME_DeathB03;
		ent->client->anim_end = FRAME_DeathB08;
		return;
	}

newanim:
	// return to either a running or standing frame
	client->anim_priority = ANIM_BASIC;
	client->anim_duck = duck;
	client->anim_run = run;

	if (!ent->groundentity)
	{
		vec3_t dest, face;
		AngleVectors(client->ps.viewangles, face, NULL, NULL);
		VectorMA(ent->s.origin, 100, face, face);
		VectorCopy(ent->velocity, dest);
		VectorMA(ent->s.origin, 0.025, dest, dest);

		PB_PlaySequenceForJump(ent, dest, face);
	}
	else if (run)
	{	// running
		vec3_t dest, face;
		AngleVectors(client->ps.viewangles, face, NULL, NULL);
		VectorMA(ent->s.origin, 100, face, face);
		VectorCopy(ent->velocity, dest);
		VectorMA(ent->s.origin, 0.025, dest, dest);

		PB_PlaySequenceForMovement(ent, dest, face, ent->s.origin, ent->s.angles);
	}
	else
	{	// standing
		vec3_t dest, face;
		AngleVectors(client->ps.viewangles, face, NULL, NULL);
		VectorMA(ent->s.origin, 100, face, face);
		VectorCopy(ent->velocity, dest);
		VectorNormalize(dest);
		VectorMA(ent->s.origin, 100, dest, dest);

		PB_PlaySequenceForStand(ent, dest, face);
	}
}

/*
==============
flushPlayernameColors
==============
*/

void flushPlayernameColors(edict_t &ent)
{
	if(!ent.client)
		return;

	memset(ent.client->playernameColors,0,sizeof(ent.client->playernameColors));
	memset(ent.client->oldPlayernameColors,0,sizeof(ent.client->oldPlayernameColors));
}

/*
==============
rleClientColors

Accepts an array of client indexes 'client' sand an array of color indexes
'colors' where there is 1-1 mapping between each array entry. Performs an RLE
derived encoding on clients and colors to combine them into the array 
'rleClientColors'.E.g.: 

clients: 0,1,2,3,5,6. colors: 3,7,7,7,12,12. rleClientColors: (3,0), (7|START_RANGE,1,3), (12|START_RANGE,5,6).
A saving of 4 bytes as opposed to sending un-encoded byte pairs: (3,0), (7,1), (7,2), (7,3), (12,5), (12,6).
==============
*/

int rleClientColors(int c,byte *clients,byte *colors,byte *rleClientColors)
{
	#define START_RANGE 128
	
	int i,ii,
		rli,
		length;

	i=rli=length=0;

	while(i<c)		
	{
		rleClientColors[rli]=colors[i];
		rli++;
		rleClientColors[rli]=clients[i];

		ii=i;

		while(ii<(c-1))
		{		
			if(((clients[ii]+1)==clients[ii+1])&&(colors[ii]==colors[ii+1]))		
				ii++;
			else
				break;

		};

		if(ii!=i)
		{
			rleClientColors[rli-1]|=START_RANGE;
			rli++;
			rleClientColors[rli]=clients[ii];
			rli++;
		}
		else
			rli++;

		i=ii+1;
	}

	return(rli);
}

/*
==============
sendPlayernameColors
==============
*/

//#define _MARCUSTEST_

void sendPlayernameColors(edict_t &ent)
{
	int i,c;
	byte clients[MAX_CLIENTS];
	byte colors[MAX_CLIENTS];
	byte clientColors[MAX_CLIENTS*2];

	if(!ent.client)
		return;

#ifdef _MARCUSTEST_
	if(!(level.framenum & 63))
	{
		gi.dprintf("sendPlayernameColors(): ");
/*
		for(i=0;i<=3;i++)
			ent.client->playernameColors[i]=6;

		for(i=4;i<=6;i++)
			ent.client->playernameColors[i]=3;

		ent.client->playernameColors[7]=21;
		ent.client->playernameColors[8]=1;

		for(i=9;i<=10;i++)
			ent.client->playernameColors[i]=19;

		ent.client->playernameColors[11]=28;
*/
		ent.client->playernameColors[0]=3;
		ent.client->playernameColors[1]=7;
		ent.client->playernameColors[2]=7;
		ent.client->playernameColors[3]=7;
		ent.client->playernameColors[5]=12;
		ent.client->playernameColors[6]=12;
	}
#endif // _MARCUSTEST_

	for(i=0,c=0;i<game.maxclients;i++)
	{
		if(ent.client->oldPlayernameColors[i]!=ent.client->playernameColors[i])
		{
			clients[c]=i;
			colors[c]=ent.client->playernameColors[i];
			
			ent.client->oldPlayernameColors[i]=ent.client->playernameColors[i];

#ifdef _MARCUSTEST_	
			gi.dprintf("[%i,%i] ",i,ent.client->playernameColors[i]);
#endif // _MARCUSTEST_

			c++;
		}
	}

#ifdef _MARCUSTEST_
	gi.dprintf("\n");
#endif // _MARCUSTEST_

	if(c) // c is number of clients/colors.
	{	
		int length=rleClientColors(c,clients,colors,clientColors);
		gi.sendPlayernameColors(&ent,length,c,clientColors);

#ifdef _MARCUSTEST_
		gi.dprintf("length=%i\n",length);
#endif // _MARCUSTEST_
	}
}

#undef _MARCUSTEST_

/*
=================
ClientEndServerFrame

Called for each player at the end of the server frame
and right after spawning
=================
*/
void ClientEndServerFrame (edict_t *ent)
{
	float	bobtime;
	int		i;

	current_player = ent;
	current_client = ent->client;

	//
	// If the origin or velocity have changed since ClientThink(),
	// update the pmove values.  This will happen when the client
	// is pushed by a bmodel or kicked by an explosion.
	// 
	// If it wasn't updated here, the view position would lag a frame
	// behind the body position when pushed -- "sinking into plats"
	//
	for (i=0 ; i<3 ; i++)
	{
		current_client->ps.pmove.origin[i] = ent->s.origin[i]*8.0;
		current_client->ps.pmove.velocity[i] = ent->velocity[i]*8.0;
	}

	// Movement scaling stuff.

	current_client->ps.pmove.moveScale=dm->clientGetMovescale(ent)*255;

	//
	// If the end of unit layout is displayed, don't give
	// the player any normal movement attributes
	//
	if (level.intermissiontime)
	{
		// FIXME: add view drifting here?
		current_client->ps.blend[3] = 0;
		current_client->ps.fov = 95;
		G_SetStats (ent);
		return;
	}

	AngleVectors (ent->client->v_angle, forward, Right, up);

	// burn from lava, etc
	P_WorldEffects ();

	//
	// set model angles from view angles so other things in
	// the world can tell which direction you are looking
	//
	if (ent->client->v_angle[PITCH] > 180)
		ent->s.angles[PITCH] = (-360 + ent->client->v_angle[PITCH])/3;
	else
		ent->s.angles[PITCH] = ent->client->v_angle[PITCH]/3;
	ent->s.angles[YAW] = ent->client->v_angle[YAW];
	ent->s.angles[ROLL] = 0;
	ent->s.angles[ROLL] = SV_CalcRoll (ent->s.angles, ent->velocity)*4;

	//
	// calculate speed and cycle to be used for
	// all cyclic walking effects
	//
	xyspeed = sqrt(ent->velocity[0]*ent->velocity[0] + ent->velocity[1]*ent->velocity[1]);

	if (xyspeed < 5)
	{
		bobmove = 0;
		current_client->bobtime = 0;	// start at beginning of cycle again
	}
	else if (ent->groundentity)
	{	// so bobbing only cycles when on ground
		if (xyspeed > 210)
		{
//			bobmove = 0.25;
			bobmove = 0.3;
		}
		//else if (xyspeed > 100)
		else if (xyspeed > 10)
		{
//			bobmove = 0.125;
			bobmove = 0.2;
		}
		else
		{
//			bobmove = 0.0625;
			bobmove = 0.1;
		}
	}
	
	bobtime = (current_client->bobtime += bobmove);

	//if ((current_client->ps.pmove.pm_flags & (PMF_DUCKED|PMF_ON_GROUND)) == (PMF_DUCKED|PMF_ON_GROUND))
	//	bobtime *= 2;

	bobcycle = (int)bobtime;
	bobfracsin = fabs(sin(bobtime*M_PI));

	// detect hitting the floor
	P_FallingDamage (ent);

	// apply all the damage taken this frame
	P_DamageFeedback (ent);

	// determine the view offsets
	SV_CalcViewOffset (ent);

	// determine the full screen color blend
	// must be after viewoffset, so eye contents can be
	// accurately determined
	// FIXME: with client prediction, the contents
	// should be determined by the client
	SV_CalcBlend (ent);

	G_SetStats (ent);

	G_SetClientEvent (ent);

	G_SetClientEffects (ent);

	G_SetClientSound (ent);

	G_SetClientFrame (ent);

	UpdatePlayerFace(ent);

	UpdatePlayerWeapon(ent);

	VectorCopy (ent->velocity, ent->client->oldvelocity);
	VectorCopy (ent->client->ps.viewangles, ent->client->oldviewangles);

	// Clear weapon kicks (eventually).

	VectorClear (ent->client->kick_origin);

	for (int clr = 0; clr < 3; clr++)
	{
		if (fabs(ent->client->weaponkick_angles[clr]) < 0.8)
		{
			ent->client->weaponkick_angles[clr] = 0;
		}
		else
		{
			ent->client->weaponkick_angles[clr] *= 0.7;
		}
	}

	// If the scoreboard is active, update it.

	if (ent->client->showscores && !(level.framenum & 31))
		dm->clientScoreboardMessage(ent,ent->enemy,false);

	// If help is active, update it.

	if (ent->client->showhelp_time>level.time && !(level.framenum & 31))
		dm->clientHelpMessage(ent);

	// ********************************************************************************************
	//  Reflect camera views mode in the client's playerstate.
	// ********************************************************************************************

	if(!current_client->CameraIs3rdPerson)
	{
		if(current_client->RemoteCameraLockCount>0)
		{
			// Remote camera view, i.e. remote_id always >= 0 (remote camera's edict number).
			ent->s.effects |= EF_KILL_EFT;

			current_client->ps.remote_id=current_client->RemoteCameraNumber;
			current_client->ps.remote_type=current_client->RemoteCameraType;
		}
		else
		{
			// 1st person view.

			current_client->ps.remote_id=-1;
		}
	}
	else
	{
		// 3rd person view.
		ent->client->ps.remote_id=0;
	}

	dm->clientEndFrame(ent);

	sendPlayernameColors(*ent);

	{
		sharedEdict_t sh;

		sh.inv = (inven_c *)ent->client->inv;
		sh.edict = ent;
		sh.inv->setOwner(&sh);

		sh.inv->serverWriteRICs();
		sh.inv->serverClearRICs();
	}

	// Ensure prediction synchronisation.

	ent->client->ps.restart_count=ent->client->restart_count;	
	
	if(!ent->client->oktofire)
		ent->client->ps.buttons_inhibit|=BUTTON_ATTACK;
	else
		ent->client->ps.buttons_inhibit&=~BUTTON_ATTACK;

	// Spectator chase target (if I have one).

	if(ent->client->ps.pmove.pm_type!=PM_SPECTATOR_FREEZE)
		ent->client->ps.spectatorId=0;

	// Let the client know if we're playing a cinematic.

	current_client->ps.cinematicfreeze = game.cinematicfreeze;
}