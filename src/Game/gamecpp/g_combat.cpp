// g_combat.c

#include "g_local.h"
#include "q_sh_interface.h"
#include "p_body.h"

typedef struct
{
	char	*name;
} damageBoltName_t;

damageBoltName_t damageBoltList[] =
{	
"ABOLT_SHOULDER_R",
"ABOLT_SHOULDER_L",
"GBOLT_GUT_F",
"GBOLT_GROIN",
"GBOLT_CAP_RCALF_RTHIGH",
"GBOLT_CAP_LCALF_LTHIGH",
"GBOLT_CAP_RFOREARM_RBICEP",
"GBOLT_CAP_LFOREARM_LBICEP",
"WBOLT_HAND_R",
"WBOLT_HAND_L",
};

typedef enum
{
	BOLT_SHOULDER_R = 0,
	BOLT_SHOULDER_L,
	BOLT_GUT,
	BOLT_GROIN,
	BOLT_CAP_RCALF_RTHIGH,
	BOLT_CAP_LCALF_LTHIGH,
	BOLT_CAP_FOREARM_RBICEPT,
	BOLT_CAP_FOREARM_LBICEPT,
	BOLT_HAND_R,
	BOLT_HAND_L,
	MAX_BOLTS
};

/*
============
CanDamage

Returns true if the inflictor can directly damage the target. Used for explosions
and melee attacks.
============
*/

qboolean CanDamage (edict_t *targ, edict_t *inflictor)
{
	vec3_t	dest;
	trace_t	trace;

	// bmodels need special checking because their origin is (0,0,0).

	if (targ->solid == SOLID_BSP)
	{
		VectorAdd (targ->absmin, targ->absmax, dest);
		VectorScale (dest, 0.5, dest);
		gi.trace (inflictor->s.origin, vec3_origin, vec3_origin, dest, inflictor, MASK_OPAQUE, &trace);
		if (trace.fraction == 1.0)
			return true;
		if (trace.ent == targ)
			return true;
		return false;
	}
	
	gi.trace (inflictor->s.origin, vec3_origin, vec3_origin, targ->s.origin, inflictor, MASK_OPAQUE, &trace);
	if (trace.fraction == 1.0)
		return true;

	VectorCopy (targ->s.origin, dest);
	dest[0] += 15.0;
	dest[1] += 15.0;
	gi.trace (inflictor->s.origin, vec3_origin, vec3_origin, dest, inflictor, MASK_OPAQUE, &trace);
	if (trace.fraction == 1.0)
		return true;

	VectorCopy (targ->s.origin, dest);
	dest[0] += 15.0;
	dest[1] -= 15.0;
	gi.trace (inflictor->s.origin, vec3_origin, vec3_origin, dest, inflictor, MASK_OPAQUE, &trace);
	if (trace.fraction == 1.0)
		return true;

	VectorCopy (targ->s.origin, dest);
	dest[0] -= 15.0;
	dest[1] += 15.0;
	gi.trace (inflictor->s.origin, vec3_origin, vec3_origin, dest, inflictor, MASK_OPAQUE, &trace);
	if (trace.fraction == 1.0)
		return true;

	VectorCopy (targ->s.origin, dest);
	dest[0] -= 15.0;
	dest[1] -= 15.0;
	gi.trace (inflictor->s.origin, vec3_origin, vec3_origin, dest, inflictor, MASK_OPAQUE, &trace);
	if (trace.fraction == 1.0)
		return true;

	return false;
}

/*
============
Killed--should NEVER be called for creatures--handled by their ai
============
*/

void Killed (edict_t *targ, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	if (targ->health < -999)
		targ->health = -999;

	targ->enemy = attacker;

	//this drops the weapon a player's holding
	if (targ->client)
	{
		PB_Killed(targ);
	}

	// DOTHIS - add dollar value to player's dollarValue.  should we do it directly or 
	
	
	// FIXME - is there something missing here? The IF code is the same as the fallthrough code

	if (targ->movetype == MOVETYPE_PUSH || targ->movetype == MOVETYPE_STOP || targ->movetype == MOVETYPE_NONE)
	{	
		// Doors, triggers, etc..

		if (targ->die)
			targ->die (targ, inflictor, attacker, damage, point);
		return;
	}

	if (targ->die)
		targ->die (targ, inflictor, attacker, damage, point);
}

qboolean CheckTeamDamage (edict_t *targ, edict_t *attacker)
{
	//FIXME make the next line real and uncomment this block
	// if ((ability to damage a teammate == OFF) && (targ's team == attacker's team))
	return(false);
}


void DamageKnockBack(edict_t *targ, int dflags, int knockback, vec3_t dir)
{
	if (targ->flags & FL_NO_KNOCKBACK)
		return;

	if (dflags & DAMAGE_NO_KNOCKBACK)
		return;

	if (!knockback)
		return;

	if ((targ->movetype != MOVETYPE_NONE) && (targ->movetype != MOVETYPE_BOUNCE) && 
		(targ->movetype != MOVETYPE_PUSH) && (targ->movetype != MOVETYPE_STOP))
	{
		float	mass;
		vec3_t	kvel, dir2;

		if (targ->mass < 50)
			mass = 50;
		else
			mass = targ->mass;

		VectorCopy(dir, dir2);		// Since we're messing with the dir, don't screw up the one we're passing by reference.
		if (dflags & DAMAGE_ALL_KNOCKBACK)
		{
			dir2[2] *= 0.1;		// The flying up in the air thing is a bit much.  Tone down the vertical push significantly.
		}
		else
		{
			dir2[2] *= 0.25;
		}
		VectorNormalize(dir2);
		VectorScale (dir2, 250.0 * (float)knockback / mass, kvel);
		VectorAdd (targ->velocity, kvel, targ->velocity);
	}
}


void ClientDamageKnockBack(edict_t *player, edict_t *attacker, int dflags, float knockback, vec3_t dir)
{
	if (player->flags & FL_NO_KNOCKBACK)
		return;

	if (dflags & DAMAGE_NO_KNOCKBACK)
		return;

	if (!knockback)
		return;

	vec3_t v;
	float kbtime;

	// If this is a player, then we might have to force things a bit, mainly for the concussion grenade.
	if (dflags & DAMAGE_ALL_KNOCKBACK || attacker==NULL)
	{	// Turn off his friction for a second.
		vec3_t dir2;

		VectorCopy(dir, dir2);
		dir2[2] *= 0.10;		// The flying up in the air thing is a bit much.
		VectorNormalize(dir2);
		
		VectorScale (dir2, 1000.0 * (float)knockback / player->mass, v);
		VectorAdd (player->velocity, v, player->velocity);
		
		// Safeguard to prevent rocket jumping.
		if (player->velocity[2] > 200)
			player->velocity[2] = 200;

		kbtime = knockback / player->mass;
		player->client->friction_time = level.time + 0.5 + kbtime*0.75;
	}
	else
	{
		VectorSubtract(attacker->s.origin, player->client->damage_from, v);
		VectorNormalize(v);
		VectorScale (v, 250.0 * (float)knockback / player->mass, v);
		v[2] *= 0.25;		// The flying up in the air thing is a bit much.
		VectorSubtract(player->velocity, v, player->velocity);
		VectorSubtract(player->intend_velocity, v, player->intend_velocity);
	}
}

/*
============
T_Damage

targ		entity that is being damaged, e.g. monster.
inflictor	entity that is causing the damage, e.g. rocket.
attacker	entity that caused the inflictor to damage, e.g. player.
dir			direction of the attack.
point		point at which the damage is being inflicted.
damage		amount of damage being inflicted.
knockback	force to be applied against targ as a result of the damage.
dflags		these flags are used to control how T_Damage works.
mod			the means of death (should the target of the damage happen to die).
============
*/

#define MAX_BODY_DAMAGE 10

void T_Damage (edict_t *targ, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb)
{
	gclient_t	*client;
	int			take;
//	int			bodydmg;
	Matrix4		ZoneMatrix;
	vec3_t		vec_to_other;
	float		dirToFace,holdDir;
	vec3_t		myfacing;
	int			armorStop = 0;
	bool		bInCameraMode = (g_edicts[1].client && (REMOTE_TYPE_CAMERA == g_edicts[1].client->RemoteCameraType) );

//	vec3_t	boltPos[MAX_BOLTS];
//	int i;

	if ( (targ->takedamage == DAMAGE_NO) ||
		 ((targ->takedamage == DAMAGE_YES_BUT_NOT_IN_CAMERA) && bInCameraMode) )
	{
		// kef -- DAMAGE_YES_BUT_NOT_IN_CAMERA is here so that you can't destroy a breakable brush
		//while viewing through a camera 
		return;
	}
//	if(!dm->canDamage(edict_t *targ, edict_t *inflictor, edict_t *attacker))
//		return;

	// Adjust damage so monsters don't hurt players too much, but let 'em hurt
	// each other plenty.

	if (attacker && attacker->ai && targ->client)
	{
		if(nodamage->value==1)
		{
			damage = 1+(1.0/game.playerSkills.getPlayerDamageMod());
		}
		else
		{
			damage = (int)((float)damage*0.5);
		}
	}

	// Extra meaning to nodamage to let monsters last longer against each other.
	// If attacker is monster, and target is either monster or human, reduce damage.

	if (nodamage->value==2&&((attacker && attacker->ai) && (targ->client || targ->ai)))
	{
		damage = 0;
	}

	if (targ->ai)
	{
		// kef -- I'm sorry to have to do this, but Sabre needs to be invincible if his count isn't 100
		// dpk -- woo hoo, Dekker too!
		bool	bSabre = targ->classname && (0 == strcmp("m_x_mskinboss", targ->classname));
		bool	bDekker = targ->classname && ((0 == strcmp("m_x_mraiderboss1", targ->classname)));

		if ((!bSabre && !bDekker) || (targ->count == 100))
		{
			int prehealth = targ->health;

			targ->ai->Damage(*targ,inflictor,attacker,dir,point,origin,damage,knockback,dflags,mod, penetrate, absorb);
		}
		return;
	}

	if(targ->flags & FL_LEAN_PLAYER)
	{
		if(!(targ = targ->owner))
			return;
	}

	// ACHTUNG!!!! Assuming everything that gets past here is not a monster
	// (they should ALL have ais)!!!

	// Friendly fire avoidance... if enabled you can't hurt teammates (but you
	// can hurt yourself). Knockback still occurs.

	if ((targ != attacker) && dm->isDM() && dm->dmRule_TEAMPLAY())
	{
		if (OnSameTeam (targ, attacker))
		{
			if(dm->dmRule_NO_FRIENDLY_FIRE())
			{
				// No actual damage or damage effects like blood... we only want
				// the knockback effect.

				damage = 0;
				dflags|=DAMAGE_NO_TEAMDAMAGE;
			}
			else
				mod |= MOD_FRIENDLY_FIRE;
		}
	}

	meansOfDeath = mod;

	client = targ->client;

	// 12/30/99 kef -- it seemed wholly unreasonable to be checking the skill level of the game in deathmatch
	if(client && !dm->isDM())
	{
		int newDmg = (int)((float)damage * (float)game.playerSkills.getPlayerDamageMod());

		if ((damage > 0) && (newDmg < 1))
		{
			damage = 1;
		}
		else
		{
			damage = newDmg;
		}
	}

	VectorNormalize(dir);

	// Player knockback will be handled when player damage is handled in P_DamageFeedback(),
	// so just deal with knockback for non-player entities.

	if (!client)
	{
		DamageKnockBack(targ, dflags, knockback, dir);
	}

	// Check for godmode...

	if ( (targ->flags & FL_GODMODE) && !(dflags & DAMAGE_NO_PROTECTION) )
	{
		// ...and we are in godmode, so set the damage to zero.

		take = 0;
	}
	else if (client)
	{
		// Teamplay and temporary invulnerability (on respawn) can prevent damage
		// being applied, but DAMAGE_NO_PROTECTION (e.g. as in telefragging, will
		// always inflict damage).

		if(((!(dflags&DAMAGE_NO_TEAMDAMAGE)||(!OnSameTeam(targ,attacker))) && (client->respawn_invuln_time<level.time)) || 
		   (dflags&DAMAGE_NO_PROTECTION))
		{
			//eek! fixme! this expects no changes in damage amt taken after this point!
			armorStop = damage;
			take = PB_Damage(targ, inflictor, attacker, dir, point, origin, damage, knockback, dflags, mod, penetrate, absorb);
			armorStop -= take;
		}
		else
			take=0;
	}
	else
	{
		take = damage;
	}

	// Team damage avoidance.

	if (!(dflags & DAMAGE_NO_PROTECTION) && CheckTeamDamage (targ, attacker))
		return;

	// Handle damage effects like knockback etc.

	if (client)
	{
		if(take > 0)
		{	
			client->damage_blood += take;
			client->damage_knockback += knockback;

			// Spray some blood.

			vec3_t newDir;

			VectorScale(dir, -1.0, newDir);
			if (mod != MOD_FIRE)
			{
				FX_MakeBloodSpray(point, dir, 200, (!(rand()%6)) ? 1:0, targ);
			}

			// Add knockback effect.

			VectorCopy (point, client->damage_from);
			ClientDamageKnockBack(targ,attacker,dflags,knockback,dir);
		}
		else if (!(targ->flags & FL_GODMODE))
		{	// If no damage taken, still knock back.
			client->damage_knockback += knockback;

			// Add knockback effect.

			VectorCopy (point, client->damage_from);
			ClientDamageKnockBack(targ,attacker,dflags,knockback,dir);
		}


		client->damage_armor += armorStop;
	}

	// Actually do the damage.
	
	if (take)
	{
		targ->health -= take;

		// clever stuff for force feedback
		if (client)
		{
			if (mod & (MOD_KNIFE_SLASH|MOD_KNIFE_THROWN))
			{
				fxRunner.exec("lighthit",targ); 
			}
			else
			if (mod & (MOD_PISTOL1|MOD_PISTOL2|MOD_MPISTOL|MOD_ASSAULTRIFLE|MOD_SNIPERRIFLE|MOD_AUTOSHOTGUN|MOD_SHOTGUN|MOD_MACHINEGUN|MOD_MPG|MOD_FLAMEGUN))
			{
				fxRunner.exec("gotshot",targ); 
			}
			else
			if ((mod & (MOD_C4|MOD_EXPLOSIVE|MOD_ROCKET|MOD_ROCKET_SPLASH|MOD_PHOS_GRENADE|MOD_GRENADE|MOD_CONC_GRENADE))
				&& (take > 5))
			{
				// try mixing effects
				fxRunner.exec("exphit",targ); 
				fxRunner.exec("land",targ); 
			}
		}

		if (targ->health <= 0)
		{
			if (client)
				targ->flags |= FL_NO_KNOCKBACK;

			if(targ->respawnFunc)
			{
				InitiateRespawn(targ);
			}
		
			Killed (targ, inflictor, attacker, take, point);
			
			return;
		}
	}

	if(!client)
	{
		if (targ->pain)
			targ->pain (targ, attacker, knockback, take, point);
	}

	// Add to the damage inflicted on a player this frame. The total will be
	// turned into screen blends and view angle kicks at the end of the frame.


	// FIXME: some T_Damage calls get 0,0,0 passed to them as damage location...
	// should address this!!!

	// Hit damage location.

	if (client)
	{
		vec3_t	bodyloc;

		if (!point[0] && !point[1] && !point[2])
		{
			Com_Printf("Error: T_Damage - NULL location passed\n");	
			return;
		}

		// Something from all over?

		if ((dflags & DT_WATER) || (dflags & DT_FIRE) || (dflags & DT_FALL))
		{
			if (dflags & DT_FALL)
				client->ps.damageDir = CLDAM_B_BRACKET;
			else
				client->ps.damageDir = CLDAM_ALL_BRACKET;

			// All damaged.
//			client->ps.damageLoc = CLDAM_HEAD | CLDAM_L_ARM | CLDAM_L_LEG | CLDAM_R_ARM | CLDAM_R_LEG | CLDAM_TORSO;
		}
		else
		{
			VectorSubtract(point, targ->s.origin, bodyloc);

/*			for (i=0;i<MAX_BOLTS;++i)
			{
				targ->ghoulInst->GetBoltMatrix(level.time,ZoneMatrix,targ->ghoulInst->GetGhoulObject()->FindPart(damageBoltList[i].name),IGhoulInst::MatrixType::Entity);
				ZoneMatrix.GetRow(3,*(Vect3*)boltPos[i]);
			}

			// This needs to be expanded to look at all bolts.
			if (point[2] > boltPos[BOLT_SHOULDER_R][2])	// Head shot
			{
				client->ps.damageLoc = CLDAM_HEAD;
			}
			else
				client->ps.damageLoc = CLDAM_TORSO;
*/
			if (attacker)
			{
				VectorSubtract(attacker->s.origin,targ->s.origin,vec_to_other);
				vectoangles(vec_to_other,myfacing);
				dirToFace = anglemod(myfacing[YAW]);
				holdDir = dirToFace - targ->s.angles[YAW]; 
				holdDir = anglemod(holdDir);

				if ((holdDir < 30) || (holdDir > 340))
				{	
					// In front.

					client->ps.damageDir = CLDAM_ALL_BRACKET;
				}
				else if ((holdDir > 150) && (holdDir < 210))
				{
					// Behind you.

					client->ps.damageDir = CLDAM_ALL_BRACKET;
				}
				else if (holdDir < 180)
				{
					// To left.

					client->ps.damageDir = CLDAM_L_BRACKET;
				}
				else
				{
					// To right.

					client->ps.damageDir = CLDAM_R_BRACKET;
				}

				// I'm assuming this is the best place to put this - I want to do this DMrule if we have just been hit 
				// but not killed by someone else.
				if (dm->isDM())
					dm->HitByAttacker(targ, attacker);

			}

			VectorSubtract(point, targ->s.origin, bodyloc);
		}
	}

	if (client)
	{
		if((take > 10) || (targ->health < 25))
			SetActionMusic(*targ);
	}
}

/*
============
T_RadiusDamage

T_RadiusDamage does (damage) points of damage at inflictor_origin distance and falls off
linearly until (radius) distance from this point.
============
*/
int hackGlobalGibCalls;

void T_RadiusDamage (edict_t *inflictor, edict_t *attacker, float damage, edict_t *ignore, float radius, int mod, int dflags)
{
	float	points;
	edict_t	*ent = NULL;
	vec3_t	v;
	vec3_t	dir;
	vec3_t	inflictor_origin;
	vec3_t	hitOrg;
	float	len;
	trace_t	tr;

//	This computes a pseudo origin for bmodels, such as func_breakable_brush, which
//	don't have an origin.
	FindCorrectOrigin(inflictor, inflictor_origin); 

//	look for targets within (radius) from inflictor_origin

	CRadiusContent rad(inflictor_origin, radius);

	hackGlobalGibCalls = 0;

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		ent = rad.foundEdict(i);

		if (ent == ignore)continue;
		if (!ent->takedamage || ent->gibbed)continue;
		if(ent == world)continue;
		if(ent->svflags & SVF_BUDDY)continue;//these are only affected by direct damage

		FindCorrectOrigin(ent, hitOrg);

		VectorSubtract (inflictor_origin, hitOrg, v);

		len = VectorLength(v);

		if(len > radius)continue;

		// This is a strange thing, but basically if you're already on fire, you shouldn't get burned by other guys.
		if (mod == MOD_FIRE && ent->burntime > level.time)
			continue;

		points = damage * (1.0 - (len / radius));
		if (CanDamage (ent, inflictor))
		{
			gi.trace (inflictor_origin, NULL, NULL, hitOrg, inflictor, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER, &tr);
			VectorSubtract (tr.endpos , inflictor_origin, dir);
			if (dflags & DAMAGE_ALL_KNOCKBACK)
			{
				T_Damage (ent, inflictor, attacker, dir, tr.endpos, inflictor_origin, (int)points*0.05, (int)points, DAMAGE_RADIUS|dflags, mod);
			}
			else
			{
				T_Damage (ent, inflictor, attacker, dir, tr.endpos, inflictor_origin, (int)points, (int)points, DAMAGE_RADIUS|dflags, mod);
			}
		}

	}
}

/*
============
T_RadiusDamage2

T_RadiusDamage2 does (maxdamage) points of damage to anything that overlaps the (damagepoint), 
scaled down to (mindamage) at (damageradius) distance from the center.
============
*/
#define MAX_TARGET_RADIUS 16
void T_RadiusDamage2 (edict_t *inflictor, edict_t *attacker, vec3_t damagepoint, float maxdamage, float mindamage, 
					  float damageradius, edict_t *ignore, int mod, int dflags)
{
	float	points;
	edict_t	*ent = NULL;
	vec3_t	v;
	vec3_t	dir;
	vec3_t	hitOrg;
	float	len;
	trace_t	tr;

	CRadiusContent rad(damagepoint, damageradius);

	hackGlobalGibCalls = 0;

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		ent = rad.foundEdict(i);

		if (ent == ignore)continue;
		if (!ent->takedamage || ent->gibbed)continue;
		if(ent == world)continue;
		if(ent->svflags & SVF_BUDDY)continue;//these are only affected by direct damage

		FindCorrectOrigin(ent, hitOrg);

		VectorSubtract (damagepoint, hitOrg, v);

		len = VectorLength(v);

		points = mindamage + (maxdamage-mindamage) * (1.0 - (len / damageradius));

		if (CanDamage (ent, inflictor))
		{
			gi.trace (damagepoint, NULL, NULL, hitOrg, inflictor, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER, &tr);
			VectorSubtract (tr.endpos , damagepoint, dir);
			if (dflags & DAMAGE_ALL_KNOCKBACK)
			{
				T_Damage (ent, inflictor, attacker, dir, tr.endpos, damagepoint, (int)points*0.05, (int)points, DAMAGE_RADIUS|dflags, mod);
			}
			else
			{
				T_Damage (ent, inflictor, attacker, dir, tr.endpos, damagepoint, (int)points, (int)points, DAMAGE_RADIUS|dflags, mod);
			}
		}
	}
}

/*
============
T_ConeDamage

Range should be the minimum allowed dotproduct - it defines the cone...
============
*/

void T_ConeDamage(edict_t *inflictor, edict_t *attacker, float damage, edict_t *ignore, float radius, int mod, vec3_t coneDir, float range)
{
	float	points;
	edict_t	*ent = NULL;
	vec3_t	v;
	vec3_t	inflictOrg, hitOrg;
	vec3_t	dir;
	float	len;
	float	dotVal;

	FindCorrectOrigin(inflictor, inflictOrg); 

//	look for targets within (radius) from inflictor_origin
	CRadiusContent rad(inflictOrg, radius);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		ent = rad.foundEdict(i);

		if (ent == ignore)continue;
		if (!ent->takedamage)continue;
		if (ent == world)continue;

		FindCorrectOrigin(ent, hitOrg);
		VectorSubtract (hitOrg, inflictOrg, v);
		len = VectorNormalize(v);
	
		if(len > radius)continue;

		VectorCopy(coneDir, dir);
		VectorNormalize(dir);
		dotVal = DotProduct(v, dir);

		if(dotVal < range)continue;

		points = damage * (1.0 - (len / radius));//position in cone should dictate damage too...
		points *= (dotVal - range)/(1.0 - range);
		if (CanDamage (ent, inflictor))
		{
			VectorSubtract (ent->s.origin, inflictOrg, dir);
			T_Damage (ent, inflictor, attacker, dir, inflictOrg, inflictOrg, (int)points, (int)points, DAMAGE_RADIUS, mod);
		}
	}
}