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
@w_fire.cpp
fireBullet():
	Shoot Trace 3xRangeValue , Unless shotgun magic bullet uses fixed 256.:
		//Damage FadeOut in distance feature.
		if not Sniper:
			vlen = weaponRange*2 * tr.fraction;
			if vlen > weaponRange:
				vlen represents weaponRange*2, scaled by weaponRange*3
				tr.fraction has to be >=0.5, which is atleast 1.5*weaponRange collision distance.

				dmg = dmg * (wf.weapon->getEffectiveRange() / vlen);

				0.5 fr == 1.5x colldist == 2*0.5 == 1/1 = dmg remains.
				0.6 fr == 1.8x colldist == 2*0.6 == 1/1.2 = 0.833 dmg scale
				0.7 fr == 2.1x colldist == 2*0.7 == 1/1.4 = 0.714 dmg scale
				0.8 fr == 2.4x colldist == 2*0.8 == 1/1.6 = 0.625 dmg scale
				0.9 fr == 2.7x colldist == 2*0.9 == 1/1.8 = 0.555 dmg scale
				1.0 fr == 3.0x colldist == 2*1.0 == 1/2.0 = 0.500 dmg scale

				TLDR: If enemy is between 1.5xWeaponRange - 3.0xWeaponRange, the damage of weapon is reduced, with max reduction being 50% and least reduction being 10%


				Table for shotgun:
				0.5 fr == 1.0x colldist == 2*0.5 == 1/1 dmg remains
				0.6 fr == 1.2x colldist == 2*0.6 == 1/1.2 = 0.833 dmg scale
				0.7 fr == 1.4x colldist == 2*0.7 == 1/1.4 = 0.714 dmg scale
				0.8 fr == 1.6x colldist == 2*0.8 == 1/1.6 = 0.625 dmg sclae
				0.9 fr == 1.8x colldist == 2*0.9 == 1/1.8 = 0.555 dmg scale
				1.0 fr == 2.0x colldist == 2*1.0 == 1/2.0 = 0.500 dmg scale
fireSpas()
	fireBullet(wf);
		Shoot Trace 3xRangeValue:
			if not Sniper:
				DamageFadeOut()
--------------------------------------------------------------------------------------------------------
					t_damage()
--------------------------------------------------------------------------------------------------------
	for i in range(0,12):
--------------------------------------------------------------------------------------------------------
		t_damage()
--------------------------------------------------------------------------------------------------------

@g_combat.cpp
t_damage()
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
	If damaged an npc:

	else:
		armorStop = damage;
		//calculate armour reduced.
--------------------------------------------------------------------------------------------------------
		take = PB_Damage()
--------------------------------------------------------------------------------------------------------
		armorStop -= take;
		
		reduce health

		apply got hit effects.

		Killed ()

// calls showDamage, 
@p_body.cpp
PB_Damage() @p_body.cpp
	if damage < 0
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

	// In W_fire.cpp , called at the bottom of fireBullet().
		void clearBulletTraceHits(void)
		{
			bullet_numHits=0;
		}
	// bullet_numHits set by ShowDamage()->GetProjectileFrontHole() and PB_Damage() here.
	if ( bullet_numHits == 0 && proj || melee )
		int RayTrace( 			
			float Time 		,const Vect3 &start 	,const Vect3 &dir  	,HitRecord *Hits 	,int MaxHits)
		bullet_numHits = RayTrace(
			level.time, 	bullet_EntStart			,bullet_EntDir 		,bullet_Hits		,20

--------------------------------------------------------------------------------------------------------
	//int bodyhuman_c::ShowDamage @ ai_bodyhuman.cpp
	TAKE = SHOWDAMAGE()
--------------------------------------------------------------------------------------------------------

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
		Play death animation

	If armour after ShowDamage() does not equal armour before ShowDamage():
		PB_InitBody. ( removes vest )

	return health taken.

// Check If dead, apply frontcodes, adjustdmg, apply sever, sets bullet_numHits for proj
@ai_bodyhuman.cpp
ShowDamage():
	Knife Slash Damage from behind deals 2.5x damage

	DT_SHOTGUN:
		//These are pelts, Does something unique for first pelt
		ChooseShotgunGoreZones()	

		Knife DT_BACKSTAB:
			numOfFrontCodes = 1;
			if ((listOfFrontCodes[0] <= GZ_NECK) || (listOfFrontCodes[0] >= GZ_LEG_LOWER_RIGHT))
			{	// dunno what to do here. punt.
				listOfFrontCodes[0] = GZ_CHEST_BACK;
			}
	DT_MANGLE and DT_DIRECTIONALGORE:
		ChooseDirectionalGoreZones 
	DT_DIRECTIONALGORE:
		ChooseDirectionalGoreZones
	ELSE:
		// Sets bullet_numHits
		GetProjectileFrontHole -> This is recursive function. Sets bullet_numHits

	NOTE: GetProjectileFrontHole can affect the damage, because pointer to take is passed to it. And it does so under some scenarios, like if the bullet hit a weapon eg.
	
	if target == player:
		if dflags == DAMAGE_NO_ARMOR
			bodyDmg = take
		else:
--------------------------------------------------------------------------------------------------------
			bodyDmg=monster.client->inv->adjustDamageByArmor(take, penetrate, absorb);
--------------------------------------------------------------------------------------------------------

		(In realistic DM , armour is only applied to certain areas.)

		if bodyDmg:
			target.flags = FL_BODY_DAMAGE

		if bodyDmg < take:
			//If the damage was reduced
			bArmor = true;
			monster.flags |= FL_ARMOR_DAMAGE;

			if bodyDmg <= 0 :
				if absorb:
					PlayEffect(weakspark)
				else if penetrate:
					PlayEffect(armorhit)
				else:
					PlayEffect(spark2)
				PlaySound(hit.wav)

	if target == npc:
		Apply armour logic bit differently.


	return 0 dmg if hit bolton?


	Scale FrontHole Damage.
		numOfFrontCodes
			newtake=(int)((float)take*gzones[frontCode].damScale);

			In Realistic DM: Limbshots take at most : 30% of current hp
				newtake = min(newtake, (int)(monster.health	* .33));


			totaltake += newtake;

		Average Out Damage. Multiple codes are mainly found for shotguns.
		This is interesting because the numOfFrontCodes means that some areas are able
		to absorb more damage?
			take=totaltake/(float)numOfFrontCodes;


	Show Pain emotion
	Limit max damage to 150.

	if DEAD:
		Drop any hand bolt-ons.
		If they reached -750 hp, Gib them
		If they reached -50 hp and took explosive dmg, Gib them.


		Iterate Gorezone:
			Use first to apply sever:
--------------------------------------------------------------------------------------------------------
				ShowFrontHoleDamage()
--------------------------------------------------------------------------------------------------------

	return take

// Generate FrontCodes.
@ai_bodyhuman.cpp
ShotgunGoreZones():

	Will set multiple FrontCodes for each body location.

	First Pelt:
		case GZ_LEG_UPPER_RIGHT_FRONT:
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GROIN;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_LEG_LOWER_RIGHT;
			listOfFrontCodes[(*numOfFrontCodes)++]=GZ_GUT_FRONT;
		break;

		LEG_UPPER_RIGHT_FRONT:
			Will set 3 gore zones. Because the pellets are meant to affect a large area
			surrounding the target location.

	Non-first Pelts/Other pelts:
		ShotgunStand() ->
			Assign Random Gore zones ( See Below ) based on facing direction.
			#define MAX_FRONTCODES	5
			
			if you are crouched it calls ShotgunCrouch()
			else:
			ShotgunStand()
				if standing higher ground than target:
					headShotChance = 1 in 7
				else:
					if hits near top of bbox:
						headShotChance = 1 in 5

				Equal y position:
					HS_Chance = 1/11
			
				
				if hit above the belt:
					Adds between 1-4 1,2,3,4 zones
					if from above those zones are :
						if behind,front,side:
							if behind,front:
								GZ_CHEST_BACK = 1;
								GZ_SHLDR_LEFT_BACK = 1;
								GZ_SHLDR_RIGHT_BACK = 1;
								GZ_GUT_BACK = 1;

								irand(0,headShotChance):
									GZ_HEAD = 10
								4/4 = 1
								14/5 = 2.8

							else if side:
								3.5 total
								3.5 + 10 = 13.5
								13.5/5 = 2.7

								Ah say you get 10 + 1:
									10/2 = 5
					Average 2.7 dmg with  hs luck
					Max DMG 5.5 with hs luck and roll 1 zone.

					average = roll 2 zones.
						10 + 2 = 12.
						12/3 = 4 dps.
						 
				else:
					----no headshot chance----
					0.5f + 0.5f + 0.5f + 0.5f == 2.
					2/4 = 0.5.
					Average 0.5 dmg.


	How the magic bullet defines the hit location using s_FrontCode variable
		Line 9530 of ShowDamage: s_FrontCode = frontCode;
		and Line 8775 of ChooseShotgunGoreZones: switch (s_FrontCode)

	End of ChooseShotgunGoreZones:
		s_FrontCode = NUM_HUMANGOREZONES;
		// No more pelts matter. Only applies gore once.

	note: irand is inclusive of max and min

// Reduce dmg.
@w_inven.cpp
adjustDamageByArmor()

// Apply Sever.
@ai_bodyhuman.cpp
ShowFrontHoleDamage()