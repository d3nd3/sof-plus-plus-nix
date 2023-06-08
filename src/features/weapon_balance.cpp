#include "common.h"

/*
	Goal : balance the big guns so they can be used more often on larger levels.

	t_damage @g_combat.cpp
		If damaged an npc:
			Damage @ai.cpp
				ShowDamage @ai_bodyhuman.cpp
					adjustDamageByArmor(take,penetrate,absorb) @w_inven.cpp
		else:
			PB_Damage()

			void Killed (edict_t *targ, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)


	gclient_s
		int damage_blood; 		// damage taken out of health
		int damage_knockback; 	// impact damage
		int damage_armor; 		// need this for appropriate feedback
		vec3_t damage_from;		// origin for vector calculation


	Used in void P_DamageFeedback (edict_t *player) @ p_view.cpp
		// Total points of damage shot at the player this frame?
		int count=client->damage_blood + client->damage_armor
		if(!count)
		{
			// Didn't take any damage.
			return;
		}

	void T_Damage (
		edict_t *targ,
		edict_t *inflictor,
		edict_t *attacker,
		vec3_t dir,
		vec3_t point,
		vec3_t origin,
		int damage,
		int knockback,
		int dflags,
		int mod,
		float penetrate,
		float absorb
	)

		targ->takedamage == DAMAGE_NO || DAMAGE_YES_BUT_NOT_IN_CAMERA) && bInCameraMode
			return
		if nodamage cvar == 1:
			npc damage dealt to player = 1
		else
			damage from ai is reducecd by 2 by default.

		if nodamage cvar == 2:
			npc damage dealt to player and other npc == 0

		targ->flags & FL_LEAN_PLAYER ( NO DAMAGE LEAN BUDDY )
			return

		if no friendly fire:
			damage = 0;
			dflags|=DAMAGE_NO_TEAMDAMAGE;

		singleplayer:
			scale damage by difficulty settings.
			if damage between 0-1 , round up to 1.

		Handle knockback of non-players ( player knockback handled in ClientEndServerFrame() @p_view.cpp P_DamageFeedback() @p_view.cpp )
			DamageKnockBack(targ, dflags, knockback, dir);


		if ( (targ->flags & FL_GODMODE) && !(dflags & DAMAGE_NO_PROTECTION) )
			take = 0

		// Teamplay and temporary invulnerability (on respawn) can prevent damage
		// being applied, but DAMAGE_NO_PROTECTION (e.g. as in telefragging, will
		// always inflict damage)
		DamagingAPlayer:
			armorStop = damage;
			take = PB_Damage(targ, inflictor, attacker, dir, point, origin, damage, knockback, dflags, mod, penetrate, absorb);
			armorStop -= take;
		else:
			take = damage

		So PB_Damage function calculates how much HEALTH damage the player took.

		eg. 
			Initial damage = 90.
			PB_Damage returns 30.
			armorStop represents how much armour is removed this frame.
			armour removed = 90 - 30 = 60.
			30 of the damage goes into health. 60 of the damage goes into armour.

		healthDmg + armourDmg = totalDmg.

	

	PB_Damage()
		damage < 0
			return

		invalid_target or isNonPlayer:
			PlayPlayerHurtSound(targ, damage, dflags, mod);
			return damage;

		invalid body or player_state_bod:
			return;

		if health > 0:
			save_time_as_initalKilledTime

		save_time_as_lastKilledTime
		Time lag between when you had health last and you died.

		hadArmor.
		int RayTrace( 	float Time 		,const Vect3 &start 	,const Vect3 &dir  	,HitRecord *Hits 	,int MaxHits)
						level.time, 	bullet_EntStart			,bullet_EntDir 		,bullet_Hits		,20

		//HUGE FUNCTION int bodyhuman_c::ShowDamage @ ai_bodyhuman.cpp
		take = targ->client->body->ShowDamage(

		//extra damage for females
		if (take > 0 && targ->client->body->GetClassCode()==BODY_FEMALE)
		{
//			gi.dprintf("More damage, girlie! Start take: %d, ", take);
			take+=(float)take*0.5;
//			gi.dprintf("End take: %d!\n", take);
		}

		if targ->health > take:
			//Survived
			Play pain animation at location.

		else:
			//Is Dead


	CWeaponInfo::CWeaponInfo(
		char *newMFlashView,
		char *newMFlashDM,
		char *newMFlashSingle,
		float soundRange,
		float damage,
		float bSize,
		int dFlags,
		float spawnAdd, -> Shooting makes more NPC spawn.
		float enemySpawnAdd, -> Shooting makes more NPC spawn.
		int MeansODeath,
		void (*infunc)(weaponFireInfo_t &wf),
		float newDPenetrate,
		float newDAbsorb,
		void (*cacheFunc)(void)
		float newEfRange,
		char *newtracerFX,
		qboolean newbighit,
		int newIndex
	)
------------------------------------------MPG----------------------------------------------
	CWeaponInfo(
		"weapons/playermz/mpgbeam1", //newMFlashView
		"", //newMFlashDM
		"", //newMFlashSingle
		0, //soundRange
		10, //damage
		3, //bSize
		0, //dFlags
		0.00, //spawnAdd
		0.00, //enemySpawnAdd
		MOD_MPG, //MeansODeath
		fireMicrowaveCone, //infunc
		1.0, //newDPenetrate
		0, //newDAbsorb
		cacheMicro2, //cacheFunc
		512, //newEfRange
		"", //newtracerFX
		false, //newbighit
		ATK_MICROWAVE //newIndex
		),
	CWeaponInfo(
		"weapons/playermz/mpg", //newMFlashView
		"weapons/othermz/mpg", //newMFlashDM
		"weapons/othermz/mpgenemy", //newMFlashSingle
		0, //soundRange
		200, //damage
		3, //bSize
		0, //dFlags
		0.00, //spawnAdd
		0.00, //enemySpawnAdd
		MOD_MPG, //MeansODeath
		fireMicro, //infunc
		1.0, //newDPenetrate
		0, //newDAbsorb
		cacheMicro, //cacheFunc
		512, //newEfRange
		"", //newtracerFX
		false, //newbighit
		ATK_MICROWAVE_ALT //newIndex
	),
------------------------------------------SHOTGUN----------------------------------------------
	CWeaponInfo(
		"weapons/playermz/shotgun", //newMFlashView
		"weapons/othermz/shotgun_p", //newMFlashDM
		"weapons/othermz/shotgun", //newMFlashSingle
		800, //soundRange
		40, //damage
		1, //bSize
		DT_SHOTGUN, //dFlags
		0.20, //spawnAdd
		0.20, //enemySpawnAdd
		MOD_SHOTGUN, //MeansODeath
		fireSpas, //infunc
		0, //newDPenetrate
		.5, //newDAbsorb
		cacheShotgun, //cacheFunc
		128, //newEfRange
		"", //newtracerFX
		false, //newbighit
		ATK_SHOTGUN //newIndex
		),	//	-> damage is per pellet
*/

bool shotgunStudy = false;
void my_fireSpas(weaponFireInfo_t &wf)
{
	shotgunStudy = true;
	orig_fireSpas(wf);
	shotgunStudy = false;
}

/*
void exec(char *name, edict_t *ent, multicast_t castType = MULTICAST_PHS, edict_t *ignoreClient = 0)
{
	exec(gi.effectindex(name), ent, castType, ignoreClient);
}

CFXSender::exec(void * self,int, float *, multicast_t, edict_s *)

Inspired from :
//
		// 2/10/00 kef -- this gore effect doesn't make sense if you shoot a tank, heli, or armored guy.
		//				pluswhich, I think we've got enough blood and gore effects these days to get by
		//				without this one. If we decide we want it back, put it in bodyhuman::ShowDamage()
Inside fireSpas().
*/
void * memfxRunner = NULL; 
vec3_t	oldDir;
void applyShotgunGore(edict_t * ent)
{
	vec3_t efDir;

	VectorCopy(oldDir, efDir);
	VectorNormalize(efDir);
	VectorScale(efDir, -1, efDir);

	vec3_t spot;
	VectorMA(ent->s.origin, 16, efDir, spot);

	// fxRunner.setDir(efDir);
	// VectorCopy(in, dir); flags |= EFF_DIR;
	VectorCopy(efDir, memfxRunner+0x18);
	int flags = stget(memfxRunner,0x4);
	stset(memfxRunner,0x4,flags|EFF_DIR);

	// fxRunner.exec("gore/shotgun", ent);
	orig_CFXSender_exec(memfxRunner,orig_SV_EffectIndex("gore/shotgun"),ent,MULTICAST_PHS,0);
}

/*
	Attempt to re-enable shotgun gore that was removed.
*/

// Step1: Obtaining oldDir.

qboolean fake_SV_Trace (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passent, int contentmask, trace_t *trace) {
	VectorCopy(end,oldDir);
	return (*orig_SV_Trace) (start, mins, maxs, end, passent,contentmask, trace);
}
