#include "common.h"

/*
	Goal : balance the big guns so they can be used more often on larger levels.

	A waver of degees(Angles) has increased distance delta the further the range.

	wf.ent->client check is to see if the shooter is a player, not an npc.

	DAMAGE_NO_ARMOR( In Realistic_Damage() .44 and sniper), drowning and fire.

	vec3_t vec3_origin = {0,0,0};

	IDEAS:
		Remove camera shake from slugThrower.
		Remove range randomness from shotty.
		Nerf the magic shot damage from shotty.
		Nerf first shot not having waver or nerf waver.

		Make python functions for settings instead of cvars.

	shotgun fireSpas():

		Deals 12 damage to World Object ents.

		Has a magic bullet before shooting the 12 ray traces.
		fireBullet(wf);
			this bullet has DT_PROJECTILE flag.
			It damages with the damage as knockback value which is 40 for shotgun
			range of magic bullet == 256. ( not random unlike the pelts )

		7th and 8th argument of t_damage are damage and knockback.
		if some of the 12 raytraces from cone hit same ent multiple times.
		the damage dealt to the ent is scaled with that.
		14*numHits[i]
		npc deal less damage : 10*numHits[i]

		Cone of damage is 2x wider than it is high.
		with a random distance between 256 and 512.


	MagicBullet For shotgun scaling:
		max shotgun range = 256
		0-125 range = max damage
		125 - 256 range = scaled by distance [100% -> 50%]

	So the pellets have a range of 256-512.
	The magic bullet range of 256.

	TLDR: Shotgun.
	If the player is hit at range 128, magic_bullet = max damage, and all pelts reach target.
	If the player is hit at range 128-256, magic bullet dmg scaled, and all pelts reach target.
	If the player is hit  at 256-512 , magic bullet doesn't hit, some pelts miss some pelts hit.
	At 384, 50% of pelts hit, > 384 scale towards 0% pelts hit, < 384 scale towards 100% pelts hit.

	DmgScaling for nonshotgun:
		0-1.5 x WeaponRange = max damage
		1.5 x WeaponRange -> 3 x WeaponRange = scaled damage [100% -> 50%]

		


	if you are crouched it calls ShotgunCrouch()
		which uses correct angle to calculate bShotsAimingUp, whereas ShotgunStand()
		uses origin to claculate bShotsAimingUp.

	Here it rolls between 1-5 gore zones, perhaps increasing chance you average lower.
	But for this penalty no longer checks if you aiming below belt.

	The magic bullet hits first, indicating a gore location after calling GetProjectileFrontHole.
	The FIRST pelt that hits uses this location in ChooseShotgunGoreZones to indicate whether to apply gore?

	So magic bullet controls 'where to apply gore'
	a pelt that hit controls 'if gore applies'


	10.0f
		----------10-----------
			GZ_HEAD_FRONT [10.0f]
			GZ_HEAD_BACK [10.0f]
			GZ_NECK [10.0f]

	1.0f
		-----------0.83333-------------
			GZ_SHLDR_RIGHT_FRONT [1.0f + 1.0f + 0.5f == 2.5/3 == 0.8333 ]
			GZ_SHLDR_RIGHT_BACK [1.0f + 1.0f + 0.5f]
			GZ_SHLDR_LEFT_FRONT [1.0f + 1.0f + 0.5f]
			GZ_SHLDR_LEFT_BACK [1.0f + 1.0f + 0.5f]

		-----------1.0--------------
			GZ_CHEST_FRONT [1.0f + 1.0f + 1.0f == 3.0/3 == 1.0]
			GZ_CHEST_BACK [1.0f + 1.0f + 1.0f]

			GZ_GUT_FRONT_EXTRA [1.0f + 1.0f + 1.0f]
			GZ_GUT_BACK_EXTRA [1.0f + 1.0f + 1.0f]
			GZ_GUT_FRONT [1.0f + 1.0f + 1.0f]
			GZ_GUT_BACK [1.0f + 1.0f + 1.0f]

			GZ_GROIN [1.0f]
	0.5f
		---------0.75------------
			GZ_ARM_UPPER_RIGHT [0.5f + 1.0f] == 1.5/2 == 0.75
			GZ_ARM_UPPER_LEFT [0.5f + 1.0f]

		---------0.5----------
			GZ_ARM_LOWER_RIGHT [0.5f]
			GZ_ARM_LOWER_LEFT [0.5f]

			GZ_FOOT_RIGHT [0.5f]
			GZ_FOOT_LEFT [0.5f]

			GZ_LEG_LOWER_RIGHT [0.5f + 0.5f]
			GZ_LEG_LOWER_LEFT [0.5f + 0.5f]
	
		-----------0.83333-------------
			GZ_LEG_UPPER_RIGHT_FRONT [1.0f + 0.5f + 1.0f] == 2.5/3 == 0.8333
			GZ_LEG_UPPER_RIGHT_BACK [1.0f + 0.5f + 1.0f]
			GZ_LEG_UPPER_LEFT_FRONT [1.0f + 0.5f + 1.0f]
			GZ_LEG_UPPER_LEFT_BACK [1.0f + 0.5f + 1.0f]

	TLDR: 
		Worst places to hit [0.5]:
			ARM
			FOOT
			LEG_LOWER
		Slightly Better [0.75]:
			ARM_UPPER
		Even Better [0.833]:
			LEG_UPPER
			SHOULDER
		Good [1x]:
			CHEST
			GUT
			GROIN
		JackPot [10x]:
			HEAD
			NECK

	hschance = +5 to multiplier on average
	14*5.
	
	CWeaponInfo::CWeaponInfo(
		char *newMFlashView,
		char *newMFlashDM,
		char *newMFlashSingle,
		float soundRange,
		float damage,
		float bSize, -> BulletSize used for PerformWallDamage()
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
		qboolean newbighit, -> FX_HandleDMShotBigHitEffects [ TE_DMSHOTBIGHIT ] -> Hit a surface with a big effect.
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
	),

-----------------------------------9MM---------------------------------------------
	CWeaponInfo(
		"weapons/playermz/pistol1", //newMFlashView
		"weapons/othermz/pistol1_p", //newMFlashDM
		"weapons/othermz/pistol1", //newMFlashSingle
		300, //soundRange
		45, //damage
		2, //bSize
		0, //dFlags
		0.03, //spawnAdd
		0.03, //enemySpawnAdd
		MOD_PISTOL1, //MeansODeath
		fireBullet, //infunc
		0, //newDPenetrate
		0, //newDAbsorb
		cachePistol1, //cacheFunc
		512, //newEfRange
		"weapons/world/tracer_s", //newtracerFX
		false, //newbighit
		ATK_PISTOL1 //newIndex
	),
----------------------------------.44------------------------------------------------
	CWeaponInfo(
		"weapons/playermz/pistol2", //newMFlashView
		"weapons/othermz/pistol2_p", //newMFlashDM
		"weapons/othermz/pistol2", //newMFlashSingle
		700, //soundRange
		120, //damage
		5, //bSize
		DT_SEVER, //dFlags
		0.08, //spawnAdd
		0.08, //enemySpawnAdd
		MOD_PISTOL2, //MeansODeath
		fireBullet, //infunc
		0.5, //newDPenetrate
		0, //newDAbsorb
		cachePistol2, //cacheFunc
		800, //newEfRange
		"weapons/world/tracer_l", //newtracerFX
		true, //newbighit
		ATK_PISTOL2 //newIndex
	),
*/

bool shotgunStudy = false;
void my_fireSpas(weaponFireInfo_t &wf)
{
	
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
	shotgunStudy = true;
	VectorCopy(end,oldDir);
	return (*orig_SV_Trace) (start, mins, maxs, end, passent,contentmask, trace);
}




// ----------------------------SHOTGUN PATCHES------------------------------------------

/*

	20/40 == distance scaling
	14/140 == headshot REAL AIM
	77/154/217/280/343 == Aim Above Belt or win lottery

	BaseDMG scaled by fireBullet() func.
		If the player is hit at range 128, magic_bullet = max damage, and all pelts reach target.
		If the player is hit at range 128-256, magic bullet dmg scaled, and all pelts reach target.
		If the player is hit  at 256-512 , magic bullet doesn't hit, some pelts miss some pelts hit.
		At 384, 50% of pelts hit, > 384 scale towards 0% pelts hit, < 384 scale towards 100% pelts hit.

	BASEDMG + AIMDMG + LUCKDMG
	40 		+ 14 	 + 154		= 208

	20/40 + 14/140 + 77/154/217/280/343

	BaseDMG can be scaled from 40 down to 20.

	AimDMG is based upon the 1st pellet that hits. Uses BaseDMG hit location to apply some damage. If that hit location was neck/head it will apply 140 dmg, else it applies about 14 dmg. The skill here is if you get that head shot you do deal significant dmg, its 126 more dmg than if you didn't.

	LUCKDMG is the sum of all the other pelts which are assigned random gore zones. You mostly will deal 154 damage from this part of the equation. But if you are lucky you can hit 343, like once in a 1000 , once in 100 if you are on higher ground or jumping. As long as you aim above the waistline, you will can rely on luck to kill your opponent. If you aim below the waistline you will be rolled lower values of 0.5 dmg on average (legs/arm/feet). Thus making your LUCKDMG be 11*7 = 77

	Pelt count = 12
	Pelt Damage = 14
	Pelt Start Distance == 256
	Pelt End Distance == 512
	Pelt Max Waver Horizontal = 0.1
	Pelt Max Waver Vertical = 0.05
	Magic Bullet Damage = 40
	Magic Bullet Max Range == 256
	Magic Bullet Scaled Dmg Start Distance = 128
	Magic Bullet Max Dmg Scale = 50%

	40 flat dmg +

	1st pelt after magic bullet scales differently.
	bestcase:
		10 * 14 = 140 pelt dmg
	averagecase:
		14 * 1 dmg

	11 pelts:
		bestcase:
			5.5 * 14 * 11 = 847
		averagecase:
			1 * 14 * 11 = 154

		triple lucky hs roll:
			triple lucky hs roll:
				5.5 * 3 * 14 + 14 * 8 =  343

		two lucky hs roll:
			5.5*14*2 + 14*9 = 280

		one lucky hs roll:
			5.5*14 + 14*10 = 217

	best_total : 40 + 140 + 847 = 1027
	1 in 100 luck dmg : 40 + 140 + 343 = 523
	1 in 5 luck dmg : 40 + 140 + 217 = 397
	1 in 100 luck dmg ( with same elevation ) : 
	average : 40 + 14 + 154 = 208

	thats my predictions
	on shotgun damage
	so if you win lottery you deal 1027 damage
	and its not based upon where you aim
	the only part of that formula that cares about where you aim, is the 2nd number.
	the 140, and the 14


	0.2 = 20%
	0.04 = 4%
	0.008 = 0.8%
	0.0016 = 0.016%
	0.00032 = 0.0032%
	0.000064
	0.0000128
	0.00000256
	0.000000512
	0.0000001024
	0.00000002048


	20% chance 1 of your 11 pelts hs bonus dmg.
	4% chance 2 of your 11 pelts hs bonus dmg.

	To get headshot damage, a pelt has to land and your magic bullet has to aim head.
	You have max range 256 for magic bullet to connect.

	We could make the requirement for one pelt to land be less required.
*/
void patchShotgun(void)
{

	memoryAdjust(base_addr +  , )
}