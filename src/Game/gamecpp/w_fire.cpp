#include "g_local.h"
#include "w_weapons.h"
#include "CWeaponInfo.h"

edict_t	*SV_TestEntityPosition (edict_t *ent);

extern void EntToWorldMatrix(vec3_t org, vec3_t angles, Matrix4 &m);

static float s_lastHawkShooting = 0;
#define HAWKSHOOTING_DEBOUNCE	10.0

#define ENEMY_MWAVE_DELAY .4

static int ricochetSounds[6];
static int flybys[4];
static int cracks[3];

void cacheWeaponCommon(void)
{
	gi.effectindex("environ/onfireburst");
	gi.effectindex("environ/quickfireburst");
	gi.effectindex("spark");
	gi.effectindex("environ/wallspark");
	
	// since this seems to be the place to pre-cache effects files, lets add a few more
	// How'z about some force feedback effects?
	gi.effectindex("land");
	gi.effectindex("gotshot");
	gi.effectindex("exphit");
	gi.effectindex("lighthit");

	// general shell casing sounds
	gi.soundindex("weapons/fx/tink/gen1.wav");
	gi.soundindex("weapons/fx/tink/gen2.wav");

	// specifically metal shell casing sounds
	gi.soundindex("weapons/fx/tink/metal1.wav");
	gi.soundindex("weapons/fx/tink/metal2.wav");
	gi.soundindex("weapons/fx/tink/metal3.wav");	

	// ...and shotgun shell casing sounds
	gi.soundindex("weapons/fx/tink/shot1.wav");
	gi.soundindex("weapons/fx/tink/shot2.wav");
	gi.soundindex("weapons/fx/tink/shot3.wav");	

	// ...and heavy shell casing sounds, like the sniper, etc.
	gi.soundindex("weapons/fx/tink/hvy1.wav");
	gi.soundindex("weapons/fx/tink/hvy2.wav");
	gi.soundindex("weapons/fx/tink/hvy3.wav");	

	// the infamous "a weapon is being fired at you" crack sounds
	cracks[0] = gi.soundindex("weapons/fx/crack/crack1.wav");
	cracks[1] = gi.soundindex("weapons/fx/crack/crack2.wav");
	cracks[2] = gi.soundindex("weapons/fx/crack/crack3.wav");

	// the not-so-infamous "I'm smoking crack" sound
	//gi.soundindex("Keith/is/on/crack.wav");

	// people-on-fire effect
	gi.effectindex("weapons/world/airexplode");
	gi.effectindex("weapons/world/grenadeexplode");
	gi.soundindex ("Weapons/gren/throw.wav");
	gi.effectindex("weapons/world/c4explode");

	// nearby bullet sounds
	flybys[0] = gi.soundindex("Weapons/FX/FlyBy/FlyBy1.wav");
	flybys[1] = gi.soundindex("Weapons/FX/FlyBy/FlyBy2.wav");
	flybys[2] = gi.soundindex("Weapons/FX/FlyBy/FlyBy3.wav");
	flybys[3] = gi.soundindex("Weapons/FX/FlyBy/FlyBy4.wav"); 

	// ricochet sounds
	ricochetSounds[0] = gi.soundindex("Weapons/FX/Rics/ric1.wav");
	ricochetSounds[1] = gi.soundindex("Weapons/FX/Rics/ric2.wav");
	ricochetSounds[2] = gi.soundindex("Weapons/FX/Rics/ric3.wav");
	ricochetSounds[3] = gi.soundindex("Weapons/FX/Rics/ric4.wav");
	ricochetSounds[4] = gi.soundindex("Weapons/FX/Rics/ric5.wav");
	ricochetSounds[5] = gi.soundindex("Weapons/FX/Rics/ric6.wav");


	game_ghoul.FindObject("Items/Projectiles", "shell")->RegistrationLock();

	// armor pickup
	game_ghoul.FindObject("Items/Pick-ups/Armor", "armor")->RegistrationLock();
	// armor pickup sound (used in both single player and DM)
	gi.soundindex("dm/armorpu.wav");

	// health pickup sound (used in both sp and dm)
	gi.soundindex("dm/healthpu.wav");

	gi.soundindex("weapons/gpistol/cock.wav");

	// even though it's neither a weapon nor common...
	gi.soundindex("misc/talk.wav");
	gi.soundindex("misc/type.wav");

	// flash grenade
	gi.soundindex("weapons/FlshGren/FlshExp.wav");

	char meatSound[100];

	for (int i=1;i<=3;i++)
	{
		Com_sprintf(meatSound, sizeof(meatSound), "impact/gore/impact%d.wav", i);
		gi.soundindex(meatSound);
	}
	for (i=1;i<=4;i++)
	{
		Com_sprintf(meatSound, sizeof(meatSound), "impact/player/hit%d.wav", i);
		gi.soundindex(meatSound);
	}
	gi.soundindex("impact/player/armor.wav");
	gi.soundindex("player/jump.wav");

	gi.soundindex("misc/padd/warning.wav");
	gi.soundindex("misc/padd/warning2.wav");
	gi.soundindex("misc/padd/max.wav");
	gi.soundindex("weapons/knife/throwloop.wav");
}

//any extra resources used by these
void cacheKnife(void)
{
	gi.soundindex("weapons/knife/swing2.wav");
	gi.soundindex("weapons/knife/swing3.wav");
}

void cacheKnifeThrow(void)
{
	gi.soundindex("weapons/knife/throwloop.wav");
	gi.soundindex("weapons/knife/impact.wav");
	game_ghoul.FindObject("Enemy/Bolt", "w_knife")->RegistrationLock();
}

void cacheStarThrow(void)
{
	gi.soundindex("weapons/knife/throwloop.wav");
	gi.soundindex("weapons/knife/impact.wav");
	gi.effectindex("weapons/world/startrail");
	game_ghoul.FindObject("Enemy/Bolt", "w_throw_star")->RegistrationLock();
}

void cachePistol1(void)
{
//	gi.soundindex("weapons/gpistol/fire.wav");
//	gi.effectindex("weapons/playermz/shell_s");
}

void cachePistol2(void)
{
//	gi.effectindex("weapons/playermz/shell_l");
}

void cacheMPistol(void)
{
//	gi.effectindex("weapons/playermz/shell_s");
}

void cacheAssaultR(void)
{
//	gi.effectindex("weapons/playermz/shell");
}

void cacheSniper(void)
{
	gi.soundindex("weapons/sniper/zoom.wav");
//	gi.effectindex("weapons/playermz/shell");
	gi.effectindex("weapons/playermz/sniper_alt");
}

void cacheAutoShot(void)
{
	//gi.effectindex("weapons/world/hotslug");
	gi.effectindex("weapons/world/slugthrow");
	gi.effectindex("weapons/world/enemyslug");
	gi.effectindex("weapons/world/slughit");
	gi.effectindex("weapons/world/slugexplode");
}

void cacheConcGren(void)
{
	game_ghoul.FindObject("Items/Projectiles", "grenade")->RegistrationLock();
	gi.effectindex("weapons/world/sluggren");
	gi.effectindex("weapons/world/conctrail");
	gi.soundindex ("Ambient/Gen/Electric/spark2.wav");
}

void cacheShotgun(void)
{
	gi.soundindex("weapons/shotgun/fire.wav");
	gi.effectindex("gore/shotgun");
//	gi.effectindex("weapons/playermz/shellbig");
}

void cacheMachinegun(void)
{
}

void cachePhosGren(void)
{
	game_ghoul.FindObject("Items/Projectiles", "grenade")->RegistrationLock();
	gi.effectindex("weapons/world/phostrail");
	gi.soundindex("weapons/mgun/explode.wav");
	gi.soundindex("Weapons/gren/bounce.wav");
	gi.effectindex("weapons/world/phosexplode");
	gi.effectindex("weapons/world/phosburn");
}

void cacheRocket(void)
{
	game_ghoul.FindObject("Weapon/Projectiles/Rocket", "Rocket")->RegistrationLock();
	gi.effectindex("weapons/world/rocketexplode");
	gi.effectindex("weapons/world/rockettrail2");
	gi.effectindex("weapons/world/rockettrail_p");
	gi.soundindex("Weapons/Rocket/FlyLP.wav");
//	gi.effectindex("weapons/playermz/exhaust");
}

void cacheRocketMulti(void)
{
	cacheRocket();
	gi.effectindex("weapons/world/rocketexplode_alt");
	gi.effectindex("weapons/world/rocketsplit");
	gi.effectindex("weapons/world/rockettrail_alt1");
	gi.effectindex("weapons/world/rockettrail_alt2");
}

void cacheMicro(void)
{
//	gi.effectindex("weapons/world/mpgpurple");
	gi.effectindex("weapons/world/mpgburst");
//	gi.effectindex("weapons/playermz/mpgcharge");
	gi.effectindex("weapons/othermz/mpgcharge");
	gi.effectindex("weapons/othermz/mpgcharge2");
	gi.effectindex("environ/pulsecook");//cache the gib effects too
	gi.effectindex("gore/mpgsplatter");

	gi.soundindex("Weapons/MPG/Expand.wav");
	gi.soundindex("Weapons/MPG/Burst.wav");

	gi.soundindex("Weapons/MPG/alt1.wav");
	gi.soundindex("Weapons/MPG/alt2.wav");
	gi.soundindex("Weapons/MPG/alt3.wav");
	gi.soundindex("Weapons/MPG/alt4.wav");

	gi.soundindex("Weapons/MPG/charge.wav");
}

void cacheMicro2(void)
{
//	gi.effectindex("weapons/world/mpglow");
	gi.effectindex("weapons/world/mpgzap");
	gi.effectindex("weapons/othermz/mpgbeam1");
	gi.effectindex("weapons/othermz/mpgbeam2");
	gi.effectindex("environ/onfireburst");
	gi.effectindex("environ/quickfireburst");
}

void cacheDekkerMicro(void)
{
	gi.effectindex("weapons/world/gb_dek2");
	gi.effectindex("weapons/othermz/gb_dekmz");
	gi.effectindex("environ/pulsecook");//cache the gib effects too
	gi.effectindex("gore/mpgsplatter");

	gi.soundindex("Enemy/Dekker/Fire.wav");
	gi.soundindex("Weapons/MPG/Expand.wav");
	gi.soundindex("Weapons/MPG/Burst.wav");

	gi.soundindex("Weapons/MPG/alt1.wav");
	gi.soundindex("Weapons/MPG/alt2.wav");
	gi.soundindex("Weapons/MPG/alt3.wav");
	gi.soundindex("Weapons/MPG/alt4.wav");

	gi.soundindex("Weapons/MPG/charge.wav");
}

void cacheFlameBlast(void)
{
	gi.effectindex("weapons/world/fthrower");
//	gi.effectindex("weapons/world/playerfthrow");
//	gi.effectindex("weapons/playermz/fthrowidle");
//	gi.effectindex("weapons/playermz/fthrowmain");
//	gi.effectindex("weapons/playermz/fthrowupdate");
//	gi.effectindex("weapons/playermz/fthrow1");
//	gi.effectindex("weapons/playermz/fthrow2");
//	gi.effectindex("weapons/playermz/fthrow3");
	gi.effectindex("weapons/world/flamehit");
	gi.effectindex("weapons/world/flamehitwall");
	gi.effectindex("environ/onfireburst");
	gi.effectindex("environ/quickfireburst");
	gi.effectindex("weapons/othermz/flamecharge");
}

void cacheFlameGlob(void)
{
	gi.effectindex("weapons/world/fballtrail");
	gi.effectindex("weapons/playermz/flamebuildup");
	gi.effectindex("weapons/playermz/fthrowidle");
	gi.effectindex("weapons/world/fballimpact");
}

void cacheGrenade(void)
{
	game_ghoul.FindObject("Items/Projectiles", "grenade")->RegistrationLock();
}

void cacheMolitov(void)
{
	gi.effectindex("weapons/world/molitovburst");
	//gi.effectindex("weapons/world/molitovexplode");
	gi.effectindex("weapons/world/molitrail2");
	game_ghoul.FindObject("Items/Projectiles", "w_molitov")->RegistrationLock();
}







//
// This stuff gets used by all the AI for accurate damage crap.
//

HitRecord bullet_Hits[20];
int bullet_numHits;
Vect3 bullet_EntDir,bullet_EntStart;

void clearBulletTraceHits(void)
{
	bullet_numHits=0;
}

int bulletTraceHitCheck(trace_t *tr, vec3_t start, vec3_t end, int clipMask)
{
	vec3_t		dir;
	edict_t		*hit = tr->ent;
	bool		bSurfaceIsGlass = ((tr->surface->flags>>24) == SURF_GLASS),
				bSurfaceIsGlassComputer = ((tr->surface->flags>>24) == SURF_GLASS_COMPUTER),
				bMaterialIsGlass = (hit->material == MAT_GLASS),
				bContentsWindow = (tr->contents == CONTENTS_WINDOW),
				bDidntHitWorld = (hit != world);

	// 1/5/00 kef - if we hit a buddy, actually hit the buddy's owner
	if (GetBuddyOwner(tr->ent))
	{
		tr->ent = GetBuddyOwner(tr->ent);
		hit = tr->ent;
		if (!hit)
		{
			return 0;
		}
	}
	VectorSubtract(end, start, dir);
	VectorNormalize(dir);

	bullet_numHits = 0;

/*	if(tr->ent->flags & FL_LEAN_PLAYER)
	{
		hit = tr->ent->owner;
		if(!hit)return 0;
	}
*/
#ifdef __PLAYERTEST_3DPERSON
	IGhoulInst *oldInst=hit->ghoulInst;
	if (hit->client && hit->client->ps.bod && !dm->isDM())
	{
		hit->ghoulInst = hit->client->ps.bod;
	}
#endif

	if(hit->ghoulInst)
	{
		Matrix4 ToEnt,ToWorld;
		Vect3 transform;

		EntToWorldMatrix(hit->s.origin,hit->s.angles,ToWorld);
		//but we want to put the ray into ent space, need inverse
		ToEnt.Inverse(ToWorld);
		
		VectorCopy(start,*((vec3_t*)&transform));
		ToEnt.XFormPoint(bullet_EntStart,transform);
		ToEnt.XFormVect(bullet_EntDir,*(Vect3 *)dir);
		bullet_EntDir.Norm();

		bullet_numHits = hit->ghoulInst->RayTrace(level.time,bullet_EntStart,bullet_EntDir,bullet_Hits,20);

		if(!bullet_numHits)
		{	//uh oh - going through the bbox

#ifdef __PLAYERTEST_3DPERSON
	hit->ghoulInst = oldInst;
#endif
			return 0;
		}
		else
		{
			HitRecord &Hit=bullet_Hits[0];
			Vect3 t,t2;
			t=bullet_EntDir;
			t*=Hit.Distance;
			t+=bullet_EntStart;
			ToWorld.XFormPoint(t2,t);
			*(Vect3 *)tr->endpos=t2;
			t=*(Vect3 *)start;
			float d=t.Dist(*(Vect3 *)end);
			if (d>.01f)
			{
				tr->fraction=t2.Dist(t)/d;
				if (tr->fraction>.99)
					tr->fraction=.99f;
			}
			ToEnt.XFormVectTranspose(*(Vect3 *)tr->plane.normal,Hit.Normal);
		}
	}
	else if( ((bSurfaceIsGlass||bSurfaceIsGlassComputer) && bDidntHitWorld) ||
			 bMaterialIsGlass ||
			 bContentsWindow )
	{

#ifdef __PLAYERTEST_3DPERSON
	hit->ghoulInst = oldInst;
#endif
		// gotta make sure we check against our mask. this allows, amongst other things, bulletproof glass.
		if ( !(tr->contents & clipMask) )
		{
			return 0;
		}
	}

#ifdef __PLAYERTEST_3DPERSON
	hit->ghoulInst = oldInst;
#endif

	return 1;
}

void CalcFireDirection(edict_t *self, vec3_t fwd, float waverVal, float maxWaver, attacks_e atkID)
{
	vec3_t	fireAngs;
	float	speed;
	float	waverAmount = 0;
	float	multifireWaver;

	VectorCopy(self->client->ps.viewangles, fireAngs);
	// kef -- add in our sniper rifle's kick (clamp to 90 cuz you might point straight up and fire?)
	if (atkID == ATK_SNIPER)
	{
		VectorAdd(fireAngs, self->client->weaponkick_angles, fireAngs);
		if (fireAngs[PITCH] < -90)
		{
			fireAngs[PITCH] = -90;
		}
	}

	// 50000 is roughly 225, a standard running speed, squared
	speed = DotProduct(self->velocity, self->velocity);
	speed *= 3.0 / 50000.0;//max possible miss fire is 30 degrees
//	waverAmount += speed;
	multifireWaver = ((inven_c *)self->client->inv)->getAnimInfo()->getFireLoop() * waverVal;//cap this at something?getFireLoop() * waverVal;//cap this at something?
	//multifireWaver = wAnim(*self)->getFireLoop() * waverVal;//cap this at something?
	if(multifireWaver > maxWaver)multifireWaver = maxWaver;
	waverAmount += multifireWaver;

	// in realistic dm mode, running affects your aim. so does arm damage. so does using the sniper rifle
	//
	// 1/25/00 kef -- also, we want the sniper rifle to waver in all dm modes
	if(dm->dmRule_REALISTIC_DAMAGE() || (atkID == ATK_SNIPER) )
	{
		waverAmount += dm->clientCalculateWaver(*self, (int)atkID);
	}

	fireAngs[0] += gi.flrand(-waverAmount, waverAmount);
	fireAngs[1] += gi.flrand(-waverAmount, waverAmount);
	fireAngs[2] += gi.flrand(-waverAmount, waverAmount);
	AngleVectors(fireAngs, fwd, NULL, NULL);
}

void SetProjVel(edict_t &self, edict_t &newEnt, float fwd, float right, float up)
{
	vec3_t fwdV, rightV, upV;

	if(self.client)
	{
		AngleVectors(self.client->ps.viewangles, fwdV, rightV, upV);
	}
	else
	{
		AngleVectors(self.s.angles, fwdV, rightV, upV);
	}
	VectorScale(fwdV, fwd, newEnt.velocity);
	VectorMA(newEnt.velocity, right, rightV, newEnt.velocity);
	VectorMA(newEnt.velocity, up, upV, newEnt.velocity);
}

void SetProjPhysics(edict_t &self, float friction, float gravity, float airresistance, float bouyancy, float elasticity)
{
	self.movetype = MOVETYPE_TOUCHNOTSOLID;
	self.solid = SOLID_NOT;
	self.clipmask = MASK_PROJ2; 
	self.friction = friction;
	self.gravity = gravity;
	self.airresistance = airresistance;
	self.bouyancy = bouyancy;
	self.elasticity = elasticity;
}


//--------------------------------------------------------------------------------------------
//
//					Various Weapon Help Funcs
//
//--------------------------------------------------------------------------------------------

#define COS_OF_15DEG .9659

void PerformWallDamage(trace_t tr, vec3_t dir, int size, int markType,int debris)
{
	if((tr.ent->client)||(tr.ent->ai)||(dm->isDM()))
	{
		return;
	}
	FX_MakeWallDamage(tr, dir, size, markType,debris);
}

void AlertEnemiesForDodge(weaponFireInfo_t &wf, vec3_t end, int sourceNode)
{
	vec3_t dir, start;

	VectorCopy(wf.firePoint, start);
	CRadiusContent rad(start, 800, 1, sourceNode);


	VectorSubtract(end, start, dir);
	VectorNormalize(dir);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		vec3_t enemyDir;

		if(rad.foundEdict(i)->ai)
		{
			VectorSubtract(rad.foundEdict(i)->s.origin, start, enemyDir);
			VectorNormalize(enemyDir);

			float dp = DotProduct(dir, enemyDir);

			if(dp > .9397)//cos of 20 degrees
			{
				// kef -- want Hawk to yell, "watch where you're shooting, you stupid honky!" or something like that
				if ( (strcmp(rad.foundEdict(i)->classname, "m_x_mmerc") == 0) &&
					 (level.time - s_lastHawkShooting > HAWKSHOOTING_DEBOUNCE) )
				{
					s_lastHawkShooting = level.time;
					// play sound here
					gi.sound(rad.foundEdict(i), CHAN_VOICE, gi.soundindex("impact/Hawk/LookOut.adp"), .9, .3, 0);
				}
				else
				{
					rad.foundEdict(i)->ai->RegisterShotForDodge(start, end, wf.ent);
				}
			}
			else if(dp > 0)//45 degrees
			{
				if(gi.inPVS(rad.foundEdict(i)->s.origin, start) || gi.inPVS(rad.foundEdict(i)->s.origin, end))
				{
					rad.foundEdict(i)->ai->RegisterShotForReply(start, end, wf.ent);
				}
			}

		}
	}
}



/*
============
ProjMoveToStart
============
*/
edict_t	*ProjMoveToStart(edict_t *missile, vec3_t from, vec3_t to)
{
	trace_t	trace;
	int		mask;

	if (missile->clipmask)
		mask = missile->clipmask;
	else
		mask = MASK_PROJ2;
	gi.trace (from, missile->mins, missile->maxs, to, missile->owner, mask, &trace);
	
	if (trace.startsolid || trace.allsolid)
	{
		VectorCopy(from, missile->s.origin);
		gi.linkentity(missile);
		return g_edicts;
	}
	else if (trace.fraction < 0.99)
	{
		VectorCopy(trace.endpos, missile->s.origin);
		gi.linkentity(missile);
		return trace.ent;
	}
	else
	{
		VectorCopy(to, missile->s.origin);
		gi.linkentity(missile);
		return NULL;
	}
}





//--------------------------------------------------------------------------------------------
//
//					Various Weapon Firing Funcs
//
//--------------------------------------------------------------------------------------------

void fireKnife(weaponFireInfo_t &wf)
{
	trace_t			tr;
	vec3_t			tempPos, tempPos2;
	int				weaponDamage;

	weaponDamage = wf.weapon->getDmg();

	VectorMA(wf.firePoint, 48, wf.fwd, tempPos);
	VectorMA(wf.firePoint, -16, wf.fwd, tempPos2);

	gi.polyTrace(wf.firePoint, NULL, NULL, tempPos, wf.ent, MASK_PROJ, &tr, bulletTraceHitCheck);
	gi.trace (tempPos2, NULL, NULL, tempPos, wf.ent, MASK_SHOT| MASK_WATER, &tr);

	// 1/17/00 kef - if we hit a buddy, actually hit the buddy's owner
	if (GetBuddyOwner(tr.ent))
	{
		tr.ent = GetBuddyOwner(tr.ent);
	}

	if(tr.fraction < 1.0)
	{
		PerformWallDamage(tr, wf.fwd, 6, WMRK_SLASH,false);//fixme
		
		PlayerNoise(wf.ent, tr.endpos, AI_SENSETYPE_SOUND_WEAPON, NULL, 60, 0);
	}

	if(tr.ent)
	{
		if(tr.ent->takedamage && !tr.ent->gibbed)
		{
			T_Damage (tr.ent, wf.ent, wf.ent, wf.fwd, tr.endpos, tempPos2, weaponDamage, 1, (DT_MELEE|DT_BACKSTAB), MOD_KNIFE_SLASH, wf.weapon->getPenetrate(), wf.weapon->getAbsorb());

			if(!(lock_blood))
			{
				//make the knife bloody only if you hit something that bleeds
				if((wf.ent->client)&&((tr.ent->client)||(tr.ent->ai)))
				{
					sharedEdict_t sh;

					sh.inv=(inven_c *)wf.ent->client->inv;
					sh.edict=wf.ent;
					sharedEdict_t *saved=sh.inv->getOwner();
					sh.inv->setOwner(&sh);
					sh.inv->setBloodyWeapon(1);
					sh.inv->setOwner(saved);
				}
			}
		}

	}
	clearBulletTraceHits();
}

void handleBulletFX(weaponFireInfo_t &wf, trace_t &tr)
{
	if(!dm->isDM())
	{
		if(!(wf.ent->client))
		{	//fixme - this should really just cull out of the shooter client.  Also whiz sounds?
			if(level.sight_client)
			{
				if(tr.ent == level.sight_client)
				{
					switch(gi.irand(1,2))
					{
					case 1:
						FX_MakeBulletWhiz(level.sight_client, vec3_up, vec3_origin, 100, 220);
						break;
					case 2:
						gi.sound (level.sight_client, CHAN_AUTO, cracks[gi.irand(0,2)], 1.0, 0, gi.flrand(.1, .25));
						break;
					}
				}
				else if(pointLineIntersect(wf.firePoint, tr.endpos, level.sight_client->s.origin, 128) || (tr.ent == level.sight_client))
				{
					SetActionMusic(*level.sight_client);

					//make whizzing noise...
					switch(gi.irand(1,3))
					{
					case 1:
						gi.positioned_sound (tr.endpos, g_edicts, 0, ricochetSounds[gi.irand(0,5)], 1.0, .5, 0);
						break;
					case 2:
						FX_MakeBulletWhiz(level.sight_client, vec3_up, vec3_origin, 100, 220);
						break;
					case 3:
						gi.sound (level.sight_client, CHAN_AUTO, cracks[gi.irand(0,2)], 1.0, 0, gi.flrand(.1, .25));
						break;
					}
				}
				else
				{
					if(gi.flrand(0, 1) < .3)
					{
						gi.positioned_sound (tr.endpos, g_edicts, 0, ricochetSounds[gi.irand(0,5)], 1.0, .5, 0);
					}
				}
			}
		}

		if(tr.ent == world)
		{
			if((!(wf.ent->client))||(dm->isDM()))
			{
				if(wf.weapon->getTracerEf())
				{
					if(rand()&1)
					{
						fxRunner.setPos2(tr.endpos);
						if(wf.gunPointSet)
						{
							fxRunner.exec(wf.weapon->getTracerEf(), wf.gunPoint);
						}
						else
						{
							fxRunner.exec(wf.weapon->getTracerEf(), wf.firePoint);
						}
					}
				}
			}
			else
			{
				AlertEnemiesForDodge(wf, tr.endpos, gmonster.GetClientNode());
			}
		}
		if(tr.fraction < 1.0)
		{
			PlayerNoise(wf.ent, tr.endpos, AI_SENSETYPE_SOUND_WEAPON, NULL, 60, 1, wf.weapon->getDmgFlags()&DT_STEALTHY);
			
		}
	}
	else
	{
	}
}

void bulletStrikeSpecialSurfaces(weaponFireInfo_t &wf, trace_t &tr)
{
	trace_t tr2;

	gi.trace(wf.firePoint, NULL, NULL, tr.endpos, wf.ent, CONTENTS_WATER|CONTENTS_WINDOW, &tr2);

	if(tr2.contents & CONTENTS_WINDOW)
	{	//bmodels?
//		bool	bValidName = (tr2.surface && tr2.surface->textureinfo && tr2.surface->textureinfo->name)?true:false;

		//I only want this to work with glass 'cause all other surfaces kinda suck for being shot through....
		if( ((tr2.surface->flags>>24) == SURF_GLASS) ||
			((tr2.surface->flags>>24) == SURF_GLASS_COMPUTER) ||
			(tr2.ent->material == MAT_GLASS)/* ||
			(bValidName && strstr(tr2.surface->textureinfo->name,"grate"))*/ ) // I am so, so sorry.
		{
			T_Damage (tr2.ent, wf.ent, wf.ent, wf.fwd, tr2.endpos, wf.firePoint, wf.weapon->getDmg(), 1, DT_PROJECTILE|wf.weapon->getDmgFlags(), wf.weapon->getMOD());

			PlayerNoise(wf.ent, tr2.endpos, AI_SENSETYPE_SOUND_WEAPON, NULL, 60, 0);
			PerformWallDamage(tr2, wf.fwd, wf.weapon->getBulletSize(), WMRK_BULLETHOLE,gi.irand(0,1));//fixme
			IncreaseSpawnIntensity(.2);//breakin' windows is noisy
		}
	}
	else if(tr2.contents & CONTENTS_WATER)
	{
//		fxRunner.setDir(tr2.plane.normal);
		fxRunner.exec("environ/bulletsplash", tr2.endpos);
	}
	//also handle bubble trailness
}

extern int evilHackForInvisAttack;//make weapons do no damage if field is set.  Sigh.

CWeaponInfo wInfo[];

float shootTime = 0;

void fireBullet(weaponFireInfo_t &wf)
{
	trace_t	tr;
	vec3_t	endPos;
	bool	bTraceEntIsADoor = false;
	int		othermz, tracerfx;
	float dmg;

	dmg = wf.weapon->getDmg();

	// Check to make sure the gun barrel isn't through a wall.
	gi.trace(wf.ent->s.origin, NULL, NULL, wf.firePoint, wf.ent, MASK_PROJ, &tr);

	// If the trace was successful, start the "real one".
	if (tr.fraction > 0.99)
	{
		if(wf.weapon->getEffectiveRange() == 128)
		{	//irk - keep this from being too powerful in the hands of mere mortals - this is the shotgun
			VectorMA(wf.firePoint, 256, wf.fwd, endPos);
		}
		else
		{
			VectorMA(wf.firePoint, wf.weapon->getEffectiveRange()*3, wf.fwd, endPos);
		}

		gi.polyTrace(wf.firePoint, NULL, NULL, endPos, wf.ent, MASK_PROJ, &tr, bulletTraceHitCheck);

		// If realistic damage is set, use the varying waver rather than the varying damage scale for sniper rifle
		//
		// 1/25/00 kef -- Kramer asked me to use the full-damage-only-there's-waver-when-not-zoomed stuff in
		//standard dm as well as realistic
		//
		// 1/26/00 kef -- and then, mere hours later, I am asked to make this behavior available in singleplay, too.

		// 2/1/00 dk -- this should never be sniper behavior
	/*	if( (wf.weapon->getDmgFlags()&DT_SNIPER) && (dm->isDM() && !dm->dmRule_REALISTIC_DAMAGE()) )
		{	// ew - make sniper weak when not zoomed in
			if(wf.ent->client)
			{
				vec3_t dist;
				float fdist;
				float dscale;

				VectorSubtract(tr.endpos, wf.firePoint, dist);
				fdist = VectorLength(dist);

				dscale = 1.0 - (fdist / 500.0);
				if(dscale > 1.0)dscale = 1.0;
				float thresh = .2 + .8 * ((95.0 - wf.ent->client->ps.fov)/95.0);
				if(dscale < thresh)dscale = thresh;
				dmg *= dscale;
	//			Com_Printf("Dscale is %f(%f)\n", dscale, dmg*dscale);
			}
		}
		else
	*/
		if (!(wf.weapon->getDmgFlags()&DT_SNIPER)) // sniper rifle damage NOT changed by range
		{	// take weapon effective range into account
			float vlen = wf.weapon->getEffectiveRange()*2 * tr.fraction;

			if(vlen > wf.weapon->getEffectiveRange())
			{
				dmg *= wf.weapon->getEffectiveRange() / vlen;
			}
		}
	}

	// we need to keep the client from putting bullethole decals on things like glass doors, cuz
	//the decals don't move when the door does. if we pass in WMRK_NONE instead of WMRK_BULLETHOLE, that
	//should do it. so check here and make sure what we just shot wasn't a door of some sort.
	if (tr.ent && tr.ent->classname)
	{
		if (strstr(tr.ent->classname, "door"))
		{
			bTraceEntIsADoor = true;
		}
	}

	if(!dm->isDM())
	{
		if (bTraceEntIsADoor)
		{	// don't place a bullethole decal
			PerformWallDamage(tr, wf.fwd, wf.weapon->getBulletSize(), WMRK_NONE,gi.irand(0,1));
		}
		else
		{
			PerformWallDamage(tr, wf.fwd, wf.weapon->getBulletSize(), WMRK_BULLETHOLE,gi.irand(0,1));
		}
		handleBulletFX(wf, tr);
	}
	else
	{//extra lean special death match version of the above
		gi.DamageTexture(tr.surface, 25);
		othermz = wf.weapon->getMFlashEfDM();
		tracerfx = wf.weapon->getTracerEf();
		if (tr.ent==world)
		{
			if (wf.weapon->getBigHit())
			{
				FX_HandleDMShotBigHitEffects(wf.firePoint, wf.fwd, tr.endpos, wf.ent, tracerfx, wf.inst, othermz);
			}
			else
			{
				FX_HandleDMShotHitEffects(wf.firePoint, wf.fwd, tr.endpos, wf.ent, tracerfx, wf.inst, othermz);
			}
		}
		else
		{
			FX_HandleDMShotEffects(wf.firePoint, wf.fwd, tr.endpos, wf.ent, tracerfx, wf.inst, othermz);
		}
	}

	if(!evilHackForInvisAttack)
	{	// sigh sigh sigh
		if( !game.cinematicfreeze &&
			(OnSameTeam(wf.ent, tr.ent)&&(!wf.ent->client))&&((gi.irand(0,10)>5)||(!pointLineIntersect(wf.firePoint, level.sight_client->s.origin, tr.endpos, 12))))
		{	//most of the time, we don't want to hit our chums... we look dumb doing that
		}	//still, we need to let the player force it occasionally so he can feel cool
		else
		{
			T_Damage (tr.ent, wf.ent, wf.ent, wf.fwd, tr.endpos, wf.firePoint, dmg, dmg, DT_PROJECTILE|wf.weapon->getDmgFlags(), wf.weapon->getMOD(), wf.weapon->getPenetrate(), wf.weapon->getAbsorb());
		}
	}
	bulletStrikeSpecialSurfaces(wf, tr);

	VectorCopy(tr.endpos, wf.endPos);
	clearBulletTraceHits();
}


void DelayedTouchFunction(edict_t *self)
{
	self->touch(self, g_edicts, NULL, NULL);
}



// These are for the small rockets that get split off from the big one in the altfire.
void SlugCollide(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t fwd;

	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	AngleVectors(self->s.angles, fwd, 0, 0);

	// could've hit a buddy
	if (GetBuddyOwner(other))
	{
		other = GetBuddyOwner(other);
	}

	if(surf)
	{
		vec3_t impactDir;
		trace_t tr;

		if(surf->flags & SURF_SKY)
		{
			G_FreeEdict(self);
			return;
		}

		//I only want this to work with glass 'cause all other surfaces kinda suck for being shot through....
		if( ((surf->flags>>24) == SURF_GLASS) ||
			((surf->flags>>24) == SURF_GLASS_COMPUTER) ||
			(other->material == MAT_GLASS))
		{
			T_Damage (other, self, self->owner, fwd, self->s.origin, self->s.origin, 1000, 1, DT_PROJECTILE|DT_MANGLE, MOD_ROCKET);

			PlayerNoise(self->owner, self->s.origin, AI_SENSETYPE_SOUND_WEAPON, NULL, 60, 0);
			IncreaseSpawnIntensity(.2);//breakin' windows is noisy
		}

		// Damage any textures we might have collided with...
		tr.plane = *plane;
		tr.surface = surf;
		tr.ent = other;
		VectorCopy(self->s.origin, tr.endpos);

		// Make sparky's here...
		VectorScale(plane->normal, -1, impactDir);
		VectorNormalize(impactDir);

		// I'm not going to use PerformWallDamage because it doesn't do anything in deathmatch
		FX_MakeWallDamage(tr, impactDir, 10, WMRK_NONE, false);//fixme
	}

	// Damage to the hit target is 100, knockback 100
	T_Damage (other, self, self->owner, fwd, self->s.origin, self->s.origin, 100, 100, DT_SEVER, MOD_AUTOSHOTGUN);

	// Damage to others is 60 max, radius 80
	T_RadiusDamage(self, self->owner, 60, other, 80, MOD_AUTOSHOTGUN, DT_MANGLE|DT_DIRECTIONALGORE);

	if (plane)
	{
		fxRunner.setDir(plane->normal);
	}
	else
	{
		fxRunner.setDir(fwd);
	}
	fxRunner.exec("weapons/world/slugexplode", self->s.origin);

//	BlindingLight(self->s.origin, 1000, 0.7, 0.5);

	G_FreeEdict(self);
}


void fireShotgun(weaponFireInfo_t &wf)
{
	edict_t *missile;

	if (dm->isDM())
	{
		// Send off muzzle flash effects.
		FX_HandleDMMuzzleFlash(wf.firePoint, wf.fwd, wf.ent, wf.inst, wf.weapon->getMFlashEfDM());
	}

	missile = G_Spawn();
	SetProjPhysics(*missile, 1.0, 0, 0, .7, 0);

	// Enemy slugs are different than player slugs...
	if(wf.ent->ai)
	{
		VectorScale(wf.fwd, 600.0, missile->velocity);
	}
	else
	{
		VectorScale(wf.fwd, 1200.0, missile->velocity);
	}
	VectorSet (missile->mins, -1, -1, -1);
	VectorSet (missile->maxs, 1, 1, 1);
	missile->health = 100;
	missile->think = G_FreeEdict;
	missile->nextthink = level.time + 5.0;
	missile->touch = SlugCollide;
	missile->gravity = 0;
	vectoangles(wf.fwd, missile->s.angles);
	missile->owner = wf.ent;
	//invisible?

	fxRunner.setDir(wf.fwd);
	if(wf.ent->ai)
	{
		fxRunner.exec("weapons/world/enemyslug", wf.firePoint);
	}
	else
	{
		fxRunner.exec("weapons/world/slugthrow", wf.firePoint);
	}

	if (ProjMoveToStart(missile, wf.ent->s.origin, wf.firePoint))
	{
		missile->think = DelayedTouchFunction;		
		missile->nextthink = level.time + 0.1;
	}
}

#define SHOTGUN_CONE .866
#define SHOTGUN_SPREAD 5.0/180.0*M_PI

void fireSpas(weaponFireInfo_t &wf)
{
	vec3_t	oldDir;
	vec3_t	side;
	vec3_t	up;

	vec3_t ang;
	trace_t tr;

	int numGuysHit = 0;

	// kef -- a little strange? definitely.
	typedef struct 
	{
		edict_t	*guy;
		vec3_t	dmgPoint;
	} hitguyinfo_t;

	hitguyinfo_t hitGuys[12];
	int numHits[12];

	// Make sure that we aren't shooting through a wall.
	gi.trace(wf.ent->s.origin, NULL, NULL, wf.firePoint, wf.ent, MASK_PROJ, &tr);	
	
	if (tr.fraction < 0.99)
	{	// We hit a wall before even reaching the gun barrel.
		VectorCopy(wf.ent->s.origin, wf.firePoint);		// Let's fire the bullets from the origin then.
	}

	vectoangles(wf.fwd, ang);
	AngleVectors(ang, wf.fwd, side, up);

	VectorCopy(wf.fwd, oldDir);

	vec3_t safeFwd;

	VectorCopy(wf.fwd, safeFwd);

	fireBullet(wf);

	for(int i = 0; i < 12; i++)
	{
		VectorMA(wf.fwd, gi.flrand(-.1, .1), side, oldDir);
		VectorMA(oldDir, gi.flrand(-.05, .05), up, oldDir);

		VectorNormalize(oldDir);
		VectorMA(wf.firePoint, gi.irand(256, 512), oldDir, oldDir);

		gi.trace(wf.firePoint, 0, 0, oldDir, wf.ent, MASK_PROJ, &tr);
		// This used to use PROJ2, which meant that pellets would be stopped by windows, and more importantly, fences.		
		// If a soul can tell me why this should be, please tell me.  --Pat
//		gi.trace(wf.firePoint, 0, 0, oldDir, wf.ent, MASK_PROJ2, &tr);

		if(tr.fraction < .99)
		{
			if(tr.ent && (tr.ent->ai || tr.ent->client))
			{
				int test = 0;
				int found = 0;

				while((test < numGuysHit)&&(!found))
				{
					if(hitGuys[test].guy == tr.ent)
					{
						numHits[test]++;
						found = 1;
					}
					test++;
				}
				if(!found)
				{
					hitGuys[test].guy = tr.ent;
					VectorCopy(tr.endpos, hitGuys[test].dmgPoint);
					numHits[test] = 1;
					numGuysHit++;
				}
			}
			else
			{
				// 12/29/99 kef -- removed DT_DIRECTIONALGORE from the T_Damage calls. shotgun is now DT_SEVER.
				//also, the fifth argument (wasn't that a movie) to T_Damage() used to be wf.firePoint. given that
				//the arg in question supposedly represents the worldspace intersection of the damage vector and 
				//the target's bbox, I felt that the use of tr.endpos was warranted.
				T_Damage(tr.ent, wf.ent, wf.ent, wf.fwd, tr.endpos, wf.firePoint, 12, 100,
					wf.weapon->getDmgFlags(), wf.weapon->getMOD(), wf.weapon->getPenetrate(), wf.weapon->getAbsorb());
			}
		}
	}

	for(i = 0; i < numGuysHit; i++)
	{
		if(wf.ent && wf.ent->client)
		{
			// 12/29/99 kef -- removed DT_DIRECTIONALGORE from the T_Damage calls. shotgun is now DT_SEVER.
			//also, the fifth argument (wasn't that a movie) to T_Damage() used to be wf.firePoint. given that
			//the arg in question supposedly represents the worldspace intersection of the damage vector and 
			//the target's bbox, I felt that the use of tr.endpos was warranted.
			T_Damage(hitGuys[i].guy, wf.ent, wf.ent, wf.fwd, hitGuys[i].dmgPoint, wf.firePoint, 14*numHits[i], 100,
				wf.weapon->getDmgFlags(), wf.weapon->getMOD(), wf.weapon->getPenetrate(), wf.weapon->getAbsorb());
		}
		else
		{	// not the same?  I dont' care
			if(OnSameTeam(wf.ent, hitGuys[i].guy))
			{
				if((gi.irand(1,10) < 3)&&(!pointLineIntersect(wf.firePoint, level.sight_client->s.origin, hitGuys[i].guy->s.origin, 16)))
				{	//I'm tired of guys always shooting each other
					// 12/29/99 kef -- removed DT_DIRECTIONALGORE from the T_Damage calls. shotgun is now DT_SEVER.
					//also, the fifth argument (wasn't that a movie) to T_Damage() used to be wf.firePoint. given that
					//the arg in question supposedly represents the worldspace intersection of the damage vector and 
					//the target's bbox, I felt that the use of tr.endpos was warranted.
					T_Damage(hitGuys[i].guy, wf.ent, wf.ent, wf.fwd, hitGuys[i].dmgPoint, wf.firePoint, 10*numHits[i], 100,
						wf.weapon->getDmgFlags(), wf.weapon->getMOD(), wf.weapon->getPenetrate(), .2);//armor not as useful
				}
			}
			else
			{
//				Hey, shouldn't we be using a tr.endpos of some sort here instead of wf.firePoint?
				// 12/29/99 kef -- removed DT_DIRECTIONALGORE from the T_Damage calls. shotgun is now DT_SEVER.
				//also, the fifth argument (wasn't that a movie) to T_Damage() used to be wf.firePoint. given that
				//the arg in question supposedly represents the worldspace intersection of the damage vector and 
				//the target's bbox, I felt that the use of tr.endpos was warranted.
				T_Damage(hitGuys[i].guy, wf.ent, wf.ent, wf.fwd, hitGuys[i].dmgPoint, wf.firePoint, 10*numHits[i], 100,
					wf.weapon->getDmgFlags()|DAMAGE_NO_KNOCKBACK, wf.weapon->getMOD(), wf.weapon->getPenetrate(), .2);//armor not as useful
			}
		}
		//
		// 2/10/00 kef -- this gore effect doesn't make sense if you shoot a tank, heli, or armored guy.
		//				pluswhich, I think we've got enough blood and gore effects these days to get by
		//				without this one. If we decide we want it back, put it in bodyhuman::ShowDamage()
/*
		vec3_t efDir;

		VectorCopy(oldDir, efDir);
		VectorNormalize(efDir);
		VectorScale(efDir, -1, efDir);

		vec3_t spot;

		VectorMA(hitGuys[i].guy->s.origin, 16, efDir, spot);

		if(!(lock_blood))
		{
			if((!(dm->dmRule_NO_FRIENDLY_FIRE()))||(!OnSameTeam (hitGuys[i].guy, wf.ent))||
				(!dm->isDM() && hitGuys[i].guy->client))//don't do this is single player as it is expensive
			{
				fxRunner.setDir(efDir);
				fxRunner.exec("gore/shotgun", hitGuys[i].guy);
			}
		}
		*/
	}

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_SPAS);
	gi.WritePosition(wf.firePoint);
	VectorScale(safeFwd, 32.0, oldDir);
	gi.WritePosition(oldDir);//fixme - maybe angles/pitch?
	gi.multicast(wf.firePoint, MULTICAST_PHS);
}


// ************************
// Grenade weapon/item
// ************************

void grenadeExplode(edict_t *self)
{
	vec3_t pos;

	IncreaseSpawnIntensity(.4);
	VectorCopy(self->s.origin, pos);
	pos[2] += 10;

	T_RadiusDamage2(self, self->owner, pos, 200, 40, 200, NULL, MOD_GRENADE, DT_MANGLE|DT_DIRECTIONALGORE);

	fxRunner.exec("weapons/world/grenadeexplode", pos);
	BlindingLight(pos, 2000, 0.8, 0.5);
	gmonster.RadiusDeafen(self, 130, 250);

	G_FreeEdict(self);
}


void grenadeTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	// could've hit a buddy
	if (GetBuddyOwner(other))
	{
		other = GetBuddyOwner(other);
	}

	// In DM, the grenade should explode upon impact.
	if (dm->isDM())
	{
		if (other->client)	// but only on players.
		{
			grenadeExplode(self);
			return;
		}
	}

	if((VectorLengthSquared(self->velocity) > 100)&&surf)
	{
		switch(surf->flags>>24)
		{
			case SURF_SAND_YELLOW:
			case SURF_SAND_WHITE:
			case SURF_SAND_LBROWN:
			case SURF_SAND_DBROWN:
			case SURF_SNOW:
			case SURF_LIQUID_BLUE:
			case SURF_LIQUID_GREEN:
			case SURF_LIQUID_ORANGE:
			case SURF_LIQUID_BROWN:
			case SURF_GRASS_GREEN:
			case SURF_GRASS_BROWN:
			case SURF_LIQUID_RED:
			case SURF_SNOW_LBROWN:
			case SURF_SNOW_GREY:
			case SURF_LIQUID_BLACK:
			case SURF_PAPERWALL:

				break;
		
			default:
				gi.sound(self, CHAN_VOICE, gi.soundindex("Weapons/gren/bounce.wav"), .6, 2, 0);
		}
	}
}


void grenadeThink(edict_t *self)
{
	if (self->health <= 1)
	{
		if (game.cinematicfreeze)
		{
			G_FreeEdict(self);
			return;
		}
		grenadeExplode(self);
	}
	else
	{
		gmonster.SpookEnemies(self->s.origin, 160, SPOOK_EXPLOSIVE);		//hahaha, scare them!
		self->health--;
		self->nextthink = level.time + 0.1;	// 2 seconds with 21 health.
	}
}


// Generic grenade creation code, for item or weapon.
void createGrenade(edict_t *owner, vec3_t loc, vec3_t vel, qboolean enemy)
{
	edict_t *grenade;

	grenade = G_Spawn();
	game_ghoul.SetSimpleGhoulModel(grenade, "Items/Projectiles", "grenade");

	if (enemy)
	{	// The AI needs a fairly non-bouncy grenade to calculate where it will explode.
		// Friction, gravity, airresistance, buoyancy, elasticity
		SetProjPhysics(*grenade, .7, 1.0, 0, 0, .15);
	}
	else
	{	// The player grenade should bounce around a bit for additional distance and a little more uncertainty.
		// Friction, gravity, airresistance, buoyancy, elasticity
		SetProjPhysics(*grenade, 0.7, 1.0, 0, 0, 0.5);
	}

	VectorCopy(loc, grenade->s.origin);
	VectorCopy(vel, grenade->velocity);
	VectorSet (grenade->mins, -2, -2, -3);
	VectorSet (grenade->maxs, 2, 2, 3);
	grenade->health = 21;
	grenade->think = grenadeThink;
	grenade->nextthink = level.time + 0.1;	// 2 seconds with 21 health.
	grenade->gravity = 1.0;

//	VectorCopy(wf.ent->s.angles, grenade->s.angles);	// Who cares what angle the grenade is at?
	gi.linkentity(grenade);
	grenade->owner = owner;
	grenade->touch = grenadeTouch;
	grenade->flags = FL_THROWN_WPN;
	VectorSet(grenade->avelocity, gi.irand(-300, 300), gi.irand(-300, 300), gi.irand(-300, 300));

	// Play grenade throwing sound
	gi.sound(owner, CHAN_VOICE, gi.soundindex("Weapons/gren/throw.wav"), .6, 1, 0);//fixme - bounce

	if(SV_TestEntityPosition(grenade))
	{
		grenade->touch(grenade, g_edicts, 0, 0);
	}
}

// Grenades are weapons when used by enemies.
void fireGrenade(weaponFireInfo_t &wf)
{
	vec3_t	vel;

	VectorScale(wf.fwd, wf.vel, vel);
	createGrenade(wf.ent, wf.firePoint, vel, true);
}


// *************************
// Rocket launcher weapon
// *************************

void MissileRemove(edict_t *self)
{
	G_FreeEdict(self);
}


// Player rocket.
void MissileCollide(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t fwd;

	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	AngleVectors(self->s.angles, fwd, 0, 0);

	// could've hit a buddy
	if (GetBuddyOwner(other))
	{
		other = GetBuddyOwner(other);
	}

	if(surf)
	{
		vec3_t impactDir;
		trace_t tr;

		if(surf->flags & SURF_SKY)
		{
			G_FreeEdict(self);
			return;
		}

		//I only want this to work with glass 'cause all other surfaces kinda suck for being shot through....
		if( ((surf->flags>>24) == SURF_GLASS) ||
			((surf->flags>>24) == SURF_GLASS_COMPUTER) ||
			(other->material == MAT_GLASS))
		{
			T_Damage (other, self, self->owner, fwd, self->s.origin, self->s.origin, 1000, 1, DT_PROJECTILE|DT_MANGLE, MOD_ROCKET);

			PlayerNoise(self->owner, self->s.origin, AI_SENSETYPE_SOUND_WEAPON, NULL, 128, 0);
			IncreaseSpawnIntensity(.2);//breakin' windows is noisy
		}

		// Damage any textures we might have collided with...
		tr.plane = *plane;
		tr.surface = surf;
		tr.ent = other;
		VectorCopy(self->s.origin, tr.endpos);

		// Make sparky's here...
		VectorScale(plane->normal, -1, impactDir);
		VectorNormalize(impactDir);

		// I'm not going to use PerformWallDamage because it doesn't do anything in deathmatch
		FX_MakeWallDamage(tr, impactDir, 20, WMRK_NONE, false);//fixme
	}

	IncreaseSpawnIntensity(.4);

	// Damage to the hit target is 250.
	T_Damage (other, self, self->owner, fwd, self->s.origin, self->s.origin, 250, 300, DT_MANGLE, MOD_ROCKET);

	// Damage to others is 225 max, 32 min, radius 175
	T_RadiusDamage2(self, self->owner, self->s.origin, 225, 32, 175, other, MOD_ROCKET_SPLASH, DT_MANGLE|DT_DIRECTIONALGORE);

	fxRunner.setDir(fwd);
	fxRunner.exec("weapons/world/rocketexplode", self->s.origin);
	gmonster.RadiusDeafen(self, 150, 250);
	BlindingLight(self->s.origin, 1500, 0.7, 0.5);

	G_FreeEdict(self);
}


void MissileCollide2(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{	// this is the enemy rocket
	vec3_t fwd;

	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	AngleVectors(self->s.angles, fwd, 0, 0);

	// could've hit a buddy
	if (GetBuddyOwner(other))
	{
		other = GetBuddyOwner(other);
	}

	if(surf)
	{
		vec3_t impactDir;
		trace_t tr;

		if(surf->flags & SURF_SKY)
		{
			G_FreeEdict(self);
			return;
		}

		//I only want this to work with glass 'cause all other surfaces kinda suck for being shot through....
		if( ((surf->flags>>24) == SURF_GLASS) ||
			((surf->flags>>24) == SURF_GLASS_COMPUTER) ||
			(other->material == MAT_GLASS))
		{
			T_Damage (other, self, self->owner, fwd, self->s.origin, self->s.origin, 1000, 1, DT_PROJECTILE|DT_MANGLE, MOD_ROCKET);

			PlayerNoise(self->owner, self->s.origin, AI_SENSETYPE_SOUND_WEAPON, NULL, 128, 0);
			IncreaseSpawnIntensity(.2);//breakin' windows is noisy
		}

		// Damage any textures we might have collided with...
		tr.plane = *plane;
		tr.surface = surf;
		tr.ent = other;
		VectorCopy(self->s.origin, tr.endpos);

		// Make sparky's here...
		VectorScale(plane->normal, -1, impactDir);
		VectorNormalize(impactDir);

		// I'm not going to use PerformWallDamage because it doesn't do anything in deathmatch
		FX_MakeWallDamage(tr, impactDir, 20, WMRK_NONE, false);//fixme
	}

	IncreaseSpawnIntensity(.2);

	// Damage to target and others is 225 max, 32 min, radius 175
	T_RadiusDamage2(self, self->owner, self->s.origin, 225, 32, 175, NULL, MOD_ROCKET_SPLASH, DT_MANGLE|DT_ENEMY_ROCKET|DT_DIRECTIONALGORE);

	fxRunner.setDir(fwd);
	fxRunner.exec("weapons/world/rocketexplode", self->s.origin);
	gmonster.RadiusDeafen(self, 150, 250);

	G_FreeEdict(self);
}


void MissileCountdown(edict_t *self)
{
	self->health--;

	// The check on the velocity is admittedly lame, but there are rockets getting stuck that are still thinking.
	if (self->health <= 0)
	{
		G_FreeEdict(self);
	}
	else if (Vec3IsZero(self->velocity))
	{
		self->touch(self, g_edicts, NULL, NULL);
	}
	else
	{
		self->nextthink = level.time + 0.1;
	}
}

extern void DamageKnockBack(edict_t *targ, int dflags, int knockback, vec3_t dir);

void rocketKickback(edict_t *player, vec3_t dir, float amount)
{
	float pitch, time1, time2;
	vec3_t knockdir;

	if (!(player->client))
		return;

	pitch = player->client->v_dmg_pitch - amount;
	time1 = level.time + (amount/30.0);
	time2 = player->client->v_dmg_time + (amount/30.0);
	
	if (pitch < -20)
		player->client->v_dmg_pitch = -20;
	else
		player->client->v_dmg_pitch = pitch;

	if (time1 > time2)
		player->client->v_dmg_time = time1;
	else
		player->client->v_dmg_time = time2;

	VectorScale(dir, -1.0, knockdir);

	// Blow the player back just a little bit.
	DamageKnockBack(player, 0, amount*12, knockdir);
}

#define ROCKET_SPEED_PLAYER	(800.0F)
#define ROCKET_SPEED_ENEMY (550.0F)

void fireRocket(weaponFireInfo_t &wf)
{
	edict_t *missile;
	trace_t tr2;
	vec3_t testmins={-2, -2, -2}, testmaxs={2, 2, 2};

	if (dm->isDM())
	{
		// Send off muzzle flash effects.
		FX_HandleDMMuzzleFlash(wf.firePoint, wf.fwd, wf.ent, wf.inst, wf.weapon->getMFlashEfDM());
	}

	missile = G_Spawn();
	SetProjPhysics(*missile, 0, 0, 0, 0, 0);

	VectorScale(wf.fwd, ROCKET_SPEED_PLAYER, missile->velocity);
	VectorSet (missile->mins, -1, -1, -1);
	VectorSet (missile->maxs, 1, 1, 1);

	// Find the location from which to fire this weapon.
//	VectorCopy(wf.firePoint, missile->s.origin);

	// Trace from the player origin to the fire point.  Stop if we hit anything.
	gi.trace(wf.ent->s.origin, testmins, testmaxs, wf.firePoint, wf.ent, MASK_PROJ, &tr2);
	VectorCopy(tr2.endpos, missile->s.origin);
	
	missile->health = 100;	// 10 seconds of time to count down
	missile->think = MissileCountdown;
	missile->nextthink = level.time + 0.1;
	missile->touch = MissileCollide;
	vectoangles(wf.fwd, missile->s.angles);
	gi.linkentity(missile);
	missile->owner = wf.ent;

	game_ghoul.SetSimpleGhoulModel(missile, "Weapon/Projectiles/Rocket", "Rocket");

	// Enemy rockets are different than player rockets...
	if(wf.ent->ai)
	{
		fxRunner.execContinualEffect("weapons/world/rockettrail2", missile);

		VectorScale(wf.fwd, ROCKET_SPEED_ENEMY, missile->velocity);//enemy rockets are a bit different
		missile->touch = MissileCollide2;
	}
	else
	{
		fxRunner.execContinualEffect("weapons/world/rockettrail_p", missile);	// Faster player rocket effect.
	}

	missile->s.sound = gi.soundindex("Weapons/Rocket/FlyLP.wav");
	missile->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

	if(SV_TestEntityPosition(missile))
	{
		missile->think = DelayedTouchFunction;		
		missile->nextthink = level.time + 0.1;
	}

	// Players should have their view kicked back.
	rocketKickback(wf.ent, wf.fwd, 12.0);
}


// Each rocket in the altfire for the launcher will launch and then split in two, resulting in a spread.

#define ROCKETALT_LAUNCHSPEED	700.0F
#define ROCKETALT_SPLITTIME		0.2F
#define ROCKETALT_SPLITSPEED	800.0F

// These are for the small rockets that get split off from the big one in the altfire.
void MissileSmallCollide(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t fwd;

	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	// could've hit a buddy
	if (GetBuddyOwner(other))
	{
		other = GetBuddyOwner(other);
	}

	AngleVectors(self->s.angles, fwd, 0, 0);

	if(surf)
	{
		vec3_t impactDir;
		trace_t tr;

		if(surf->flags & SURF_SKY)
		{
			G_FreeEdict(self);
			return;
		}

		//I only want this to work with glass 'cause all other surfaces kinda suck for being shot through....
		if( ((surf->flags>>24) == SURF_GLASS) ||
			((surf->flags>>24) == SURF_GLASS_COMPUTER) ||
			(other->material == MAT_GLASS))
		{
			T_Damage (other, self, self->owner, fwd, self->s.origin, self->s.origin, 1000, 1, DT_PROJECTILE|DT_MANGLE, MOD_ROCKET);

			PlayerNoise(self->owner, self->s.origin, AI_SENSETYPE_SOUND_WEAPON, NULL, 60, 0);
			IncreaseSpawnIntensity(.2);//breakin' windows is noisy
		}

		// Damage any textures we might have collided with...
		tr.plane = *plane;
		tr.surface = surf;
		tr.ent = other;
		VectorCopy(self->s.origin, tr.endpos);

		// Make sparky's here...
		VectorScale(plane->normal, -1, impactDir);
		VectorNormalize(impactDir);

		// I'm not going to use PerformWallDamage because it doesn't do anything in deathmatch
		FX_MakeWallDamage(tr, impactDir, 10, WMRK_NONE, false);//fixme
	}

	IncreaseSpawnIntensity(.2);

	// Damage to the hit target is 150.
	T_Damage (other, self, self->owner, fwd, self->s.origin, self->s.origin, 150, 175, DT_MANGLE, MOD_ROCKET);

	// Damage to others is 150 max, 32 min, radius 125
	T_RadiusDamage2(self, self->owner, self->s.origin, 200, 32, 150, other, MOD_ROCKET_SPLASH, DT_MANGLE|DT_DIRECTIONALGORE);

	if (plane)
	{
		fxRunner.setDir(plane->normal);
	}
	else
	{
		fxRunner.setDir(fwd);
	}
	fxRunner.exec("weapons/world/rocketexplode_alt", self->s.origin);
	gmonster.RadiusDeafen(self, 100, 150);
	BlindingLight(self->s.origin, 1000, 0.7, 0.5);

	G_FreeEdict(self);
}


// Take missle and split it into two.
void MissileSplit(edict_t *ent)
{
	vec3_t fwd, up;
	vec3_t newdir;
	edict_t *missile;

	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(ent);
		return;
	}

	ent->s.angles[ROLL] += gi.flrand(0, 180);		// Spin in a random direction.
	AngleVectors(ent->s.angles, fwd, NULL, up);

	// Spawn two missiles, one on either side of the current missile
	for (int i=0; i<2; i++)
	{
		missile = G_Spawn();

		switch(i)
		{
		case 0:	// Fire to one side...
			VectorMA(ent->s.origin, 6.0, up, missile->s.origin);
			VectorMA(ent->velocity, 48.0, up, missile->velocity);
			break;
		default:	// ...then to the other.
			VectorMA(ent->s.origin, -6.0, up, missile->s.origin);
			VectorMA(ent->velocity, -48.0, up, missile->velocity);
			break;
		}

		SetProjPhysics(*missile, 0, 0, 0, 0, 0);
		VectorSet (missile->mins, -1, -1, -1);
		VectorSet (missile->maxs, 1, 1, 1);
		missile->health = 100;	// 10 seconds of time to count down
		missile->think = MissileCountdown;
		missile->nextthink = level.time + 0.1;
		missile->touch = MissileSmallCollide;
		VectorCopy(missile->velocity, newdir);
		VectorNormalize(newdir);
		vectoangles(newdir, missile->s.angles);
		
		missile->owner = ent->owner;

		game_ghoul.SetSimpleGhoulModel(missile, "Weapon/Projectiles/Rocket", "Rocket");

		// Make these into small rockets.
		Matrix4 m;
		IGhoulInst *inst = missile->ghoulInst;

		inst->GetXForm(m);
		m.Scale(0.5);
		inst->SetXForm(m);

		gi.linkentity(missile);

		fxRunner.execContinualEffect("weapons/world/rockettrail_alt2", missile);	// Faster smaller rockets

		missile->s.sound = gi.soundindex("Weapons/Rocket/FlyLP.wav");
		missile->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

		if(SV_TestEntityPosition(missile))
		{
			missile->think = DelayedTouchFunction;		
			missile->nextthink = level.time + 0.1;
		}
	}

	// Do the effect for splitting.
	fxRunner.setDir(fwd);
	fxRunner.exec("weapons/world/rocketsplit", ent->s.origin);
	G_FreeEdict(ent);
}


void fireRocketMulti(weaponFireInfo_t &wf)
{	
	int rocketid=-1;
	inven_c *invweapon;
	vec3_t testmins={-2, -2, -2}, testmaxs={2, 2, 2};
	vec3_t up={0, 0, 1.0}, right={0, -1.0, 0};

	// The ID for the rocket (which determines where in the spread it goes) 
	// is based off how many rockets are left in the clip.
	if (wf.ent->client)
	{
		if (dm->isDM())
		{
			// Send off muzzle flash effects.
			FX_HandleDMMuzzleFlash(wf.firePoint, wf.fwd, wf.ent, wf.inst, wf.weapon->getMFlashEfDM());
		}

		invweapon = (inven_c *)(wf.ent->client->inv);
		if (invweapon)
		{
			rocketid = invweapon->getCurClip();

			// Since we were not provided with a nice right vector, we must make one.
			// BTW, we make it here because a side vector isn't used for the default weapon firing mode.
			CrossProduct(wf.fwd, up, right);
		}
	}

	// Add a little more randomness...
	wf.fwd[0] += gi.flrand(-.1, .1);
	wf.fwd[1] += gi.flrand(-.1, .1);
	wf.fwd[2] += gi.flrand(-.1, .1);

	VectorNormalize(wf.fwd);

	// Fire Slow rocket that will split into two fast little ones.

	edict_t *missile;
	trace_t tr2;

	missile = G_Spawn();
	SetProjPhysics(*missile, 0, 0, 0, 0, 0);
	VectorScale(wf.fwd, ROCKETALT_LAUNCHSPEED, missile->velocity);
	VectorSet (missile->mins, -1, -1, -1);
	VectorSet (missile->maxs, 1, 1, 1);

	// Find the location from which to fire this weapon.
//	VectorCopy(wf.firePoint, missile->s.origin);

	// Trace from the player origin to the fire point.  Stop if we hit anything.
	gi.trace(wf.ent->s.origin, testmins, testmaxs, wf.firePoint, wf.ent, MASK_PROJ, &tr2);
	VectorCopy(tr2.endpos, missile->s.origin);
	
	missile->health = 100;
	missile->think = MissileSplit;
	missile->nextthink = level.time + ROCKETALT_SPLITTIME;
	missile->touch = MissileCollide;	// If it hits something before splitting, treat like a normal rocket.
	vectoangles(wf.fwd, missile->s.angles);
	gi.linkentity(missile);
	missile->owner = wf.ent;

	game_ghoul.SetSimpleGhoulModel(missile, "Weapon/Projectiles/Rocket", "Rocket");

	// Initial trail for the slower rocket.
	fxRunner.execContinualEffect("weapons/world/rockettrail_alt1", missile);

	missile->s.sound = gi.soundindex("Weapons/Rocket/FlyLP.wav");
	missile->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

	if(SV_TestEntityPosition(missile))
	{
		missile->think = DelayedTouchFunction;		
		missile->nextthink = level.time + 0.1;
	}

	// Players should have their view kicked back.
	rocketKickback(wf.ent, wf.fwd, 6.0);
}

void KnifeThrowCollide(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t	impactDir, vTemp;
	trace_t	tr;
	Matrix4 m;
	bool	bHitBModel = false;

	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	if(surf)
	{
		if(surf->flags & SURF_SKY)
		{
			G_FreeEdict(self);
			return;
		}
	}
	// kef -- I think it's best for all involved if, as soon as we get into this function, we stop
	//the knife's spinning. there've been unsubstantiated reports of knives stopping in mid-air but
	//still spinning.
	VectorClear(self->avelocity);

	// Handle non-surface collisions...

	if (other == self->owner)
	{	// don't let our own knives hit us
		return;
	}
	if(self->s.sound == 0)
		return;//must've stopped moving...

	self->s.sound = 0;

	// could've hit a buddy
	if (GetBuddyOwner(other))
	{
		other = GetBuddyOwner(other);
	}
	if(other)
	{
		bHitBModel = ((other->solid == SOLID_BSP) && (other != &g_edicts[0]));
		if(other->takedamage)
		{
			self->elasticity = 254; // flag this as being a knife
			T_Damage (other, self, self->owner, self->movedir, self->s.origin, self->s.origin, 30, 0, DT_MELEE|DT_STEALTHY, MOD_KNIFE_THROWN);
			
			if (other->ai || other->client) 
			{
				// 253 means the knife hit armor and was dropped via I_Spawn. don't free it.

				if (253 == self->elasticity)
				{
					// Very important to reset this. heh.

					self->elasticity = 0;
				}
				else
				{
					// Just let me go to the great knife graveyard in tha sky.

					self->gravity = 0.0;
					G_FreeEdict(self);
				}

				return;
			}
		}
		
		// Handle bmodel collisions.

		if( bHitBModel || (other->takedamage && (!other->ai)) )
		{
			// Handle non-ai entities like bmodels.

			if (plane && plane->normal)
			{
				tr.plane = *plane;
				tr.surface = surf;
				tr.ent = other;
				FX_WallSparks(self->s.origin, plane->normal, 3);
				//VectorScale(plane->normal, -1, impactDir);
				//PerformWallDamage(tr, impactDir, 6, WMRK_NONE,false);//fixme
				VectorMA(self->s.origin, 20, plane->normal, self->s.origin);
			}
			else
			{
				VectorScale(self->velocity, -1, vTemp);
				VectorNormalize(vTemp); // ouch
				VectorMA(self->s.origin, 40, vTemp, self->s.origin);
			}

			// Zero velocity

			VectorClear(self->velocity);

			// Make sure it's laying flat.

			self->ghoulInst->GetXForm(m);
			m.Rotate(0, M_PI*-0.5);
			m.CalcFlags();
			self->ghoulInst->SetXForm(m);
			self->gravity = 1.0;
			self->owner = NULL;

			// DM handles things a bit different.

			if(dm->isDM())
			{	
				// Can't have knives piling up everywhere.

				self->think = G_FreeEdict;

				if(dm->dmRule_INFINITE_AMMO())
					self->nextthink = level.time + 10.0;
				else
					self->nextthink = level.time + 30.0;
			}
						
			// Turn the knife into a spawned knife waiting to be picked up.

			self->spawnflags|=DROPPED_ITEM;			
			I_SpawnKnife(self);
			self->elasticity=0;

			return;
		}
	}

	// Otherwise, must've hit a wall.

	if(plane && plane->normal)
	{
		FX_WallSparks(self->s.origin, plane->normal, 3);

//		if(Vec3IsZero(self->velocity))
		{
			vec3_t wallnormal;

			tr.plane = *plane;
			tr.surface = surf;
			tr.ent = other;
			VectorCopy(self->s.origin, tr.endpos);

			// Make sparky's here...

			VectorScale(plane->normal, -1, impactDir);
			VectorNormalize(impactDir);

			// Force pitch to match the wall's normal--is this right for floors?

			vectoangles(impactDir, wallnormal);
			self->s.angles[YAW] = 0;
			self->s.angles[PITCH] = 0;//wallnormal[PITCH];
			self->s.angles[ROLL] = 0;//gi.irand(0, 360);

			PerformWallDamage(tr, impactDir, 6, WMRK_NONE,false);//fixme

			// Zero velocity

			VectorClear(self->velocity);

			// Push the knife out from the wall a bit.

			VectorMA(self->s.origin, 5, plane->normal, self->s.origin);

			self->gravity = 0;

			// DM handles things a bit different.

			if(dm->isDM())
			{	
				// Can't have knives piling up everywhere.

				self->think = G_FreeEdict;
				
				if(dm->dmRule_INFINITE_AMMO())
					self->nextthink = level.time + 10.0;
				else
					self->nextthink = level.time + 30.0;
			}

			// Turn the knife into a spawned knife waiting to be picked up.

			self->spawnflags|=DROPPED_ITEM;
			I_SpawnKnife(self);

			if (self->ghoulInst)
			{
				// I would like to make sure the blade is oriented properly wrt to the axis of rotation. But I don't know how.
				self->s.angles[PITCH] = wallnormal[PITCH];
				self->s.angles[YAW] = wallnormal[YAW];
				self->ghoulInst->GetXForm(m);
				m.Rotate(2, M_PI*-0.5);
				m.CalcFlags();
				self->ghoulInst->SetXForm(m);
			}
			self->owner = NULL;
			self->elasticity=0;
		}
	}
}

void fireKnifeThrow(weaponFireInfo_t &wf)
{
	edict_t *knife;

	if (game.cinematicfreeze)
	{
		return;
	}
	knife = G_Spawn();
	knife->owner = wf.ent;
	SetProjPhysics(*knife, 1.0, .05, 0, .7, .4);
	VectorMA(wf.firePoint, 10, wf.fwd, knife->s.origin);
	VectorScale(wf.fwd, 800.0, knife->velocity);
	VectorSet (knife->mins, -1, -1, -1);
	VectorSet (knife->maxs, 1, 1, 1);
	knife->health = 31337;
	knife->think = G_FreeEdict;
	knife->nextthink = level.time + 30;
	knife->touch = KnifeThrowCollide;
	knife->flags = FL_THROWN_WPN;
	VectorCopy(wf.ent->s.angles, knife->s.angles);
	knife->avelocity[PITCH] = -360 * 6;
	gi.linkentity(knife);

	// I guess knives should obey gravity (shouldn't be able to throw them through the sky)
	knife->gravity = 0.7;

	knife->owner = wf.ent;
	// save velocity so that, after impact, we know from whence the knife came
	VectorCopy(knife->velocity, knife->pos1);

	knife->s.sound = gi.soundindex("weapons/knife/throwloop.wav");
 	knife->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

	VectorCopy(knife->velocity, knife->movedir);
	VectorNormalize(knife->movedir);

	game_ghoul.SetSimpleGhoulModel(knife, "Enemy/Bolt", "w_knife");

	knife->s.angles[ROLL] += 90;

	Matrix4 m;

	IGhoulInst *inst = knife->ghoulInst;
	inst->GetXForm(m);
	m.Scale(2.0);
	inst->SetXForm(m);

	{	// start off inside of something?
		trace_t	trace;
		int		mask;

		if (knife->clipmask)
			mask = knife->clipmask;
		else
			mask = MASK_SOLID;
		gi.trace (knife->s.origin, knife->mins, knife->maxs, knife->s.origin, knife, mask, &trace);
	
		if (trace.ent && (trace.ent->svflags & SVF_BUDDY) && (trace.ent->owner != knife->owner) )
		{
			KnifeThrowCollide(knife, GetBuddyOwner(trace.ent), NULL, NULL);
		}
	}
}

void StarThrowCollide(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t	impactDir;
	trace_t	tr;
	Matrix4 m;
	bool	bHitBModel = false;

	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	// could've hit a buddy
	if (GetBuddyOwner(other))
	{
		other = GetBuddyOwner(other);
	}

	if(surf)
	{
		if(surf->flags & SURF_SKY)
		{
			G_FreeEdict(self);
			return;
		}
	}

	fxRunner.stopContinualEffect("weapons/world/startrail", self);

	VectorClear(self->avelocity);

	// Handle non-surface collisions...

	if(self->s.sound == 0)
		return;//must've stopped moving...

	self->s.sound = 0;

	self->solid = SOLID_NOT;

	if(other)
	{
		bHitBModel = ((other->solid == SOLID_BSP) && (other != &g_edicts[0]));
		if(other->takedamage)
		{
			if (NULL == other->client)
			{
				self->elasticity = 254; // flag this as being a knife unless we hit a client
			}

			T_Damage (other, self, self->owner, self->movedir, self->s.origin, self->s.origin, 30, 0, DT_MELEE|DT_STEALTHY, MOD_STAR_THROWN);
			
			if (other->ai || other->client) 
			{
				G_FreeEdict(self);
				return;
			}
		}
		
		// Handle bmodel collisions.

		if( bHitBModel || other->takedamage )
		{
			// Handle non-ai entities like bmodels.

			if (plane && plane->normal)
			{
//				tr.plane = *plane;
//				tr.surface = surf;
//				tr.ent = other;
				FX_WallSparks(self->s.origin, plane->normal, 3);
//				VectorMA(self->s.origin, 20, plane->normal, self->s.origin);
			}
			else
			{
//				VectorScale(self->velocity, -1, vTemp);
//				VectorNormalize(vTemp); // ouch
//				VectorMA(self->s.origin, 40, vTemp, self->s.origin);
			}

			// Zero velocity

			VectorClear(self->velocity);

			// Make sure it's laying flat.

/*
			if (self->ghoulInst)
			{
				self->ghoulInst->GetXForm(m);
				m.Rotate(0, M_PI*-0.5);
				m.CalcFlags();
				self->ghoulInst->SetXForm(m);
			}
*/
			self->gravity = 1.0;
			self->owner = NULL;

			// Can't have stars piling up everywhere.
			self->think = G_FreeEdict;
			self->nextthink = level.time + 10.0;

			self->elasticity=0;

			return;
		}
	}

	// Otherwise, must've hit a wall.

	if(plane && plane->normal)
	{
		FX_WallSparks(self->s.origin, plane->normal, 3);

//		if(Vec3IsZero(self->velocity))
		{
			tr.plane = *plane;
			tr.surface = surf;
			tr.ent = other;
			VectorCopy(self->s.origin, tr.endpos);

			// Make sparky's here...

			VectorScale(plane->normal, -1, impactDir);
			VectorNormalize(impactDir);

			// Force pitch to match the wall's normal--is this right for floors?

//			vectoangles(impactDir, wallnormal);
//			self->s.angles[PITCH] = wallnormal[PITCH];
//			self->s.angles[ROLL] = gi.irand(0, 360);

			PerformWallDamage(tr, impactDir, 6, WMRK_NONE,false);//fixme

			// Zero velocity

			VectorClear(self->velocity);

			// Push the knife out from the wall a bit.

//			VectorMA(self->s.origin, 5, plane->normal, self->s.origin);

			// Make sure it's laying flat.

/*
			if (self->ghoulInst)
			{
				self->ghoulInst->GetXForm(m);
				m.Rotate(2, M_PI*-0.5);
				m.CalcFlags();
				self->ghoulInst->SetXForm(m);
			}
*/
			self->gravity = 0;
			self->owner = NULL;

			// Can't have stars piling up everywhere.
			self->think = G_FreeEdict;
			self->nextthink = level.time + 10.0;

			self->elasticity=0;
		}
	}
}

void fireStarThrow(weaponFireInfo_t &wf)
{
	edict_t *knife;

	knife = G_Spawn();
	knife->classname = "thrown_star";
	SetProjPhysics(*knife, 0.0, 0.0, 0, 0.0, 0.0);
	VectorCopy(wf.firePoint, knife->s.origin);
	VectorScale(wf.fwd, 500.0, knife->velocity);
	VectorSet (knife->mins, -1, -1, -1);
	VectorSet (knife->maxs, 1, 1, 1);
	knife->health = 10000;
	knife->think = G_FreeEdict;
	knife->nextthink = level.time + 3;
	knife->touch = StarThrowCollide;

	vectoangles(knife->velocity, knife->s.angles);
//	VectorCopy(wf.ent->s.angles, knife->s.angles);
//	knife->avelocity[PITCH] = -360 * 6;
	gi.linkentity(knife);

	// I guess knives should obey gravity (shouldn't be able to throw them through the sky)
//	knife->gravity = 0.7;

	knife->owner = wf.ent;
	// save velocity so that, after impact, we know from whence the knife came
	VectorCopy(knife->velocity, knife->pos1);

	knife->s.sound = gi.soundindex("Weapons/Knife/ThrowLoop.wav");
 	knife->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;

	VectorCopy(knife->velocity, knife->movedir);
	VectorNormalize(knife->movedir);

	game_ghoul.SetSimpleGhoulModel(knife, "Enemy/Bolt", "w_throw_star");

//	fxRunner.setDir(wf.fwd);
//	fxRunner.setPos2(knife->s.origin);
	fxRunner.execContinualEffect("weapons/world/startrail", knife);
//	fxRunner.exec("weapons/world/startrail", knife);

//	knife->s.angles[ROLL] = 90;
//	knife->s.angles[PITCH] = 90;

//	Matrix4 m;

//	knife->ghoulInst->GetXForm(m);
//	m.Scale(2.0);
//	knife->ghoulInst->SetXForm(m);
}

void fireTrueMicrowave(weaponFireInfo_t &wf)
{
	trace_t			tr2;// this one will determine who needs to hear a nice whizzing sound
	vec3_t			lastEndpos;
	edict_t			*buddy;
	int				maxTest = 0;
	vec3_t			end;
	vec3_t			dir, beammin={-3, -3, -3}, beammax={3,3,3};

	// First check the validity of the microwave beam's starting location...
	gi.trace(wf.ent->s.origin, beammin, beammax, wf.firePoint, wf.ent, MASK_PROJ, &tr2);
	if (tr2.startsolid)
	{	// Do nothing but the muzzle flash...
		return;
	}
	else if (tr2.fraction < 0.99)
	{	
		if (tr2.ent == world)
		{	// Don't go any farther...
			fxRunner.setDir(wf.fwd);
			fxRunner.setPos2(tr2.endpos);
			fxRunner.exec("weapons/world/mpgburst", wf.ent->s.origin);
			return;
		}
		else if (tr2.ent && tr2.ent->takedamage)
		{
			T_Damage(tr2.ent, wf.ent, wf.ent, dir, wf.firePoint, wf.firePoint, 240, 100, DT_NEURAL, MOD_MPG, wf.weapon->getPenetrate(), wf.weapon->getAbsorb());
		}
	}
	
	buddy = wf.ent;
	VectorCopy(wf.firePoint, lastEndpos);
	VectorCopy(wf.fwd, dir);		// I do this to protect the contents of wf.fwd, which could have been mangled by T_Damage, though it should be fixed now.  Sigh.  --Pat

	VectorMA(wf.firePoint, 1024, wf.fwd, end);

	do
	{
		gi.trace (lastEndpos, beammin, beammax, end, buddy, MASK_PROJ, &tr2);
		buddy = tr2.ent;

		maxTest++;

		if(!tr2.ent)continue;
		if(!tr2.ent->takedamage)continue;
		if(tr2.ent == wf.ent)continue;

//		PerformWallDamage(tr2, wf.fwd, wf.weapon->getBulletSize(), WMRK_NONE,gi.irand(0,1));

		T_Damage(tr2.ent, wf.ent, wf.ent, dir, wf.firePoint, wf.firePoint, 240, 100, DT_NEURAL, MOD_MPG, wf.weapon->getPenetrate(), wf.weapon->getAbsorb());

		VectorCopy(tr2.endpos, lastEndpos);

	}while((tr2.fraction < .999)&&(maxTest < 12)&&(tr2.ent != world));

	fxRunner.setDir(wf.fwd);
	fxRunner.setPos2(tr2.endpos);
	//fxRunner.exec("weapons/world/mpgpurple", wf.firePoint);
	fxRunner.exec("weapons/world/mpgburst", wf.firePoint);

	//vidfxRunner.exec("weapons/world/mpgnew", wf.firePoint);
}

void SetGenericEffectInfo(edict_t *ent);

void fireEnemyMWave(edict_t *self)
{
	if(self->owner && self->owner->inuse && self->owner->health > 0)
	{
		weaponFireInfo_t wf;

		VectorCopy(self->s.origin, wf.firePoint);
		VectorCopy(self->s.angles, wf.fwd);
		wf.ent = self->owner;
		wf.gunPointSet = 0;
		wf.weapon = &weapons.getWeapon(ATK_MICROWAVE_ALT);
		wf.inst = 0;

		fireTrueMicrowave(wf);
	}
	G_FreeEdict(self);
}

void fireMicro(weaponFireInfo_t &wf)
{
	if(wf.ent && wf.ent->client)
	{
		fireTrueMicrowave(wf);

		if (dm->isDM())
		{
			// Send off muzzle flash effects.
			FX_HandleDMMuzzleFlash(wf.firePoint, wf.fwd, wf.ent, wf.inst, wf.weapon->getMFlashEfDM());
		}
	}
	else
	{//there needs to be a delay on the guys because otherwise they are not fun nor are they fair
		edict_t *newEnt;

		newEnt = G_Spawn();
		SetGenericEffectInfo(newEnt);
		newEnt->nextthink = level.time + ENEMY_MWAVE_DELAY;
		newEnt->think = fireEnemyMWave;
		newEnt->owner = wf.ent;
		VectorCopy(wf.firePoint, newEnt->s.origin);
		VectorCopy(wf.fwd, newEnt->s.angles);
	}
}

void CharThink(edict_t *self)
{
	IGhoulInst *inst = self->owner->ghoulInst;

	if(!self->owner->inuse)
	{
		G_FreeEdict(self);
		return;
		//hopefully this is enough
	}

	self->nextthink = level.time + .1;

	if(inst)
	{
		float r, g, b, a;

		inst->GetTint(&r, &g, &b, &a);
		r-=.05;
		g-=.05;
		b-=.05;
		if(r < .1)r = .1;
		if(g < .1)g = .1;
		if(b < .1)b = .1;
		inst->SetTintOnAll(r, g, b, a);
		if ((FloatIsZeroEpsilon(r-.1)) && (FloatIsZeroEpsilon(g-.1)) && (FloatIsZeroEpsilon(b-.1)))
		{
			G_FreeEdict(self);
			return;
		}
	}


}

void WhiteThink(edict_t *self)
{
	edict_t *curSearch = 0;
	float dist;
	vec3_t	diff;
	vec3_t	exppos;
	trace_t tr2;

	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	//FX_WhiteBlast(self->s.origin);
	fxRunner.exec("weapons/world/phosexplode", self->s.origin);

	IncreaseSpawnIntensity(.3);

	VectorCopy(self->s.origin, exppos);
	exppos[2] += 8;
	CRadiusContent rad(exppos, 120);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		curSearch = rad.foundEdict(i);

		if(curSearch->client || curSearch->ai)
		{
			//FX_WhiteBurn(curSearch);
			//gi.sound (self, CHAN_BODY, gi.soundindex ("impact/gore/sizzle.wav"), .8, ATTN_NORM, 0);

			gi.trace(exppos, vec3_origin, vec3_origin, curSearch->s.origin, self, MASK_SOLID, &tr2);
			if (tr2.fraction < 0.99)
				continue;

			// Start a burning, make sure we're not overlapping multiple copies of the same effect.
			if (curSearch->phosburntime <= level.time)
			{
				if (curSearch->ghoulInst)
				{
					fxRunner.exec("weapons/world/phosburn", curSearch, 0);
				}

				curSearch->phosburntime = level.time + 2.0;
			}
			else
			{
				curSearch->phosburntime += 0.5;
			}
			curSearch->burninflictor = self->owner;

			VectorSubtract(self->s.origin, curSearch->s.origin, diff);
			dist = VectorLength(diff);
			T_Damage(curSearch, self, self->owner, vec3_up, curSearch->s.origin, curSearch->s.origin,
					 (int)((150.0-dist)*0.5), 1, DT_FIRE, MOD_PHOS_GRENADE);
		}
	}
	
	G_FreeEdict(self);
}

void PhosTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	// could've hit a buddy
	if (GetBuddyOwner(other))
	{
		other = GetBuddyOwner(other);
	}

	if((other->ai)||(other->client))
	{
		self->think(self);
	}
	else if(VectorLength(self->velocity))
	{
		gmonster.SpookEnemies(self->s.origin, 160, SPOOK_EXPLOSIVE);
		
		if(surf)
		{
			switch(surf->flags>>24)
			{
				case SURF_SAND_YELLOW:
				case SURF_SAND_WHITE:
				case SURF_SAND_LBROWN:
				case SURF_SAND_DBROWN:
				case SURF_SNOW:
				case SURF_LIQUID_BLUE:
				case SURF_LIQUID_GREEN:
				case SURF_LIQUID_ORANGE:
				case SURF_LIQUID_BROWN:
				case SURF_GRASS_GREEN:
				case SURF_GRASS_BROWN:
				case SURF_LIQUID_RED:
				case SURF_SNOW_LBROWN:
				case SURF_SNOW_GREY:
				case SURF_LIQUID_BLACK:
				case SURF_PAPERWALL:

					break;
			
				default:
					gi.sound(self, CHAN_VOICE, gi.soundindex("Weapons/gren/bounce.wav"), .6, 2, 0);
			}
		}
	}
}

void firePhosphorusGrenade(weaponFireInfo_t &wf)
{
	edict_t *grenade;
	vec3_t	ang;
	vec3_t	up, right;

	if (dm->isDM())
	{
		// Send off muzzle flash effects.
		FX_HandleDMMuzzleFlash(wf.firePoint, wf.fwd, wf.ent, wf.inst, wf.weapon->getMFlashEfDM());
	}

	vectoangles(wf.fwd, ang);
	AngleVectors(ang, NULL, right, up);

	grenade = G_Spawn();
	game_ghoul.SetSimpleGhoulModel(grenade, "Items/Projectiles", "grenade");

	SetProjPhysics(*grenade, 0.7, 1.0, 0, 0, 0.7);
	if(wf.gunPointSet)
	{
		VectorCopy(wf.gunPoint, grenade->s.origin);
	}
	else
	{
		VectorCopy(wf.firePoint, grenade->s.origin);
	}
	grenade->s.origin[2] -= 16;
	VectorScale(wf.fwd, 600.0, grenade->velocity);
	VectorMA(grenade->velocity, gi.flrand(100, 300), up, grenade->velocity);
	VectorSet (grenade->mins, -2, -2, -3);
	VectorSet (grenade->maxs, 2, 2, 3);
	grenade->think = WhiteThink;
	grenade->nextthink = level.time + 1.5;
	VectorCopy(wf.ent->s.angles, grenade->s.angles);
	gi.linkentity(grenade);
	grenade->owner = wf.ent;
	grenade->flags = FL_THROWN_WPN;	// Makes it deal with hitting lean buddies.
	grenade->touch = PhosTouch;

	fxRunner.execContinualEffect("weapons/world/phostrail", grenade);

	if(SV_TestEntityPosition(grenade))
	{
		grenade->touch(grenade, g_edicts, 0, 0);
	}
}

void fireMegaRocket(weaponFireInfo_t &wf)
{
}

void concGrenTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{

	edict_t *curSearch = 0;
	float dist, factor;
	vec3_t	diff;
	int	dmg;
	vec3_t	exppos;
	trace_t tr2;

	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	if(surf)
	{
		if(surf->flags & SURF_SKY)
		{
			G_FreeEdict(self);
			return;
		}
	}

	if (plane)
	{
		fxRunner.setDir(plane->normal);
	}
	fxRunner.exec("weapons/world/sluggren", self->s.origin);

	IncreaseSpawnIntensity(.1);

	if (plane)
		VectorMA(self->s.origin, 8.0, plane->normal, exppos);
	else
		VectorCopy(self->s.origin, exppos);
	CRadiusContent rad(exppos, 210);

	for(int i = 0; i < rad.getNumFound(); i++)
	{
		curSearch = rad.foundEdict(i);

		if(curSearch->client || curSearch->ai || curSearch->takedamage)
		{
			gi.trace(exppos, vec3_origin, vec3_origin, curSearch->s.origin, self, MASK_SOLID, &tr2);
			if (tr2.fraction < 0.99)
				continue;

			VectorSubtract(curSearch->s.origin, self->s.origin, diff);
			dist = VectorNormalize(diff);
			factor = 1.0 - (dist/250.0);
			dmg = 400*factor;

			if (curSearch == self->owner)
			{	// Halve the damage and effect to the owner...
				dmg *= 0.5;
			}

			if (curSearch->client)
			{	// People want it to do more damage to players, since they don't get stunned so much.
				T_Damage(curSearch, self, self->owner, diff, self->s.origin, curSearch->s.origin,
						 dmg*0.15, dmg, DAMAGE_ALL_KNOCKBACK, MOD_CONC_GRENADE);
			}
			else
			{
				T_Damage(curSearch, self, self->owner, diff, self->s.origin, curSearch->s.origin,
						 dmg*0.05, dmg*0.6, DAMAGE_ALL_KNOCKBACK, MOD_CONC_GRENADE);
			}

			// Blind, shake curSearch.
			if (curSearch->client)
			{	// Was a player!
				curSearch->client->blinding_alpha += (0.2 + factor);
				if (curSearch->client->blinding_alpha > 0.95)
					curSearch->client->blinding_alpha = 0.95;
				
				if (curSearch->client->blinding_alpha_delta <= 0 || curSearch->client->blinding_alpha_delta > 0.15)
				{
					curSearch->client->blinding_alpha_delta = 0.15;
				}

				FX_SetEvent_Data(curSearch, EV_CAMERA_SHAKE_LIGHT + (int)(3.2*factor), DEFAULT_JITTER_DELTA);
			}
			else if (curSearch->ai)
			{	// Tis a monster!
				curSearch->ai->MuteSenses(sight_mask, 25, smute_recov_linear, 25);
			}		
		}
	}
	
	G_FreeEdict(self);
}

void fireConcGrenade(weaponFireInfo_t &wf)
{
	edict_t *missile;
	vec3_t	ang;
	vec3_t	up, right;

	if (dm->isDM())
	{
		// Send off muzzle flash effects.
		FX_HandleDMMuzzleFlash(wf.firePoint, wf.fwd, wf.ent, wf.inst, wf.weapon->getMFlashEfDM());
	}

	vectoangles(wf.fwd, ang);
	AngleVectors(ang, NULL, right, up);

	missile = G_Spawn();
	//gi.setmodel(missile,"models/uganda/chainsaw/tris.fm");
	game_ghoul.SetSimpleGhoulModel(missile, "Items/Projectiles", "grenade");

	SetProjPhysics(*missile, 1.0, 1.0, 0, 0, 0.0);
	VectorScale(wf.fwd, 800, missile->velocity);
	missile->velocity[2] += 150;
	VectorSet (missile->mins, -2, -2, -3);
	VectorSet (missile->maxs, 2, 2, 3);
	missile->think = G_FreeEdict;
	missile->nextthink = level.time + 6;
	missile->gravity = 1.0;
	VectorCopy(wf.ent->s.angles, missile->s.angles);
	missile->owner = wf.ent;
	missile->avelocity[PITCH] = 1000;
	missile->health = 40;
	missile->touch = concGrenTouch;

	fxRunner.execContinualEffect("weapons/world/conctrail", missile);

	if(ProjMoveToStart(missile, wf.ent->s.origin, wf.firePoint))
	{
		missile->think = DelayedTouchFunction;		
		missile->nextthink = level.time + 0.1;
	}
}

void fireMicrowaveCone(weaponFireInfo_t &wf)
{
	trace_t			tr2;// this one will determine who needs to hear a nice whizzing sound
	vec3_t			lastEndpos;
	edict_t			*buddy;
	int				maxTest = 0;
	vec3_t			end, dir, minmove;
	int damage;

	// First check the validity of the microwave beam's starting location...
	gi.trace(wf.ent->s.origin, vec3_origin, vec3_origin, wf.firePoint, wf.ent, MASK_PROJ, &tr2);
	if (tr2.startsolid)
	{	// Do nothing but the muzzle flash...
		return;
	}
	else if (tr2.fraction < 0.99)
	{	
		if (tr2.ent == world)
		{	// Don't go any farther...
			fxRunner.setDir(wf.fwd);
			fxRunner.setPos2(tr2.endpos);
			fxRunner.exec("weapons/othermz/mpgbeam1", wf.ent->s.origin, MULTICAST_PHS, wf.ent);
			return;
		}
		else if (tr2.ent && tr2.ent->takedamage)
		{
			T_Damage(tr2.ent, wf.ent, wf.ent, dir, wf.firePoint, wf.firePoint, 20,  0/*knockback*/, DT_SHOCK, MOD_MPG, wf.weapon->getPenetrate(), wf.weapon->getAbsorb());
			Electrocute(tr2.ent, wf.ent);
		}
	}

	buddy = wf.ent;
	VectorCopy(wf.firePoint, lastEndpos);
	VectorCopy(wf.fwd, dir);		// I do this to protect the contents of wf.fwd, which could have been mangled by T_Damage, though it should be fixed now.  Sigh.  --Pat

	VectorMA(wf.firePoint, 1024, wf.fwd, end);
	VectorScale(wf.fwd, 8, minmove);

	// Because I'm a lazy bastard, subtract 8 before this test.
	VectorSubtract(lastEndpos, minmove, lastEndpos);
	do
	{
		VectorAdd(lastEndpos, minmove, lastEndpos);

		gi.trace (lastEndpos, vec3_origin, vec3_origin, end, buddy, MASK_PROJ, &tr2);
		buddy = tr2.ent;

		maxTest++;

		if(!tr2.ent)continue;
		if(tr2.ent == wf.ent)continue;
		if(!tr2.ent->takedamage)continue;

		if (tr2.ent->zapdmgtime <= level.time)
		{
			if (dm->isDM())
			{
				damage = 4;
				tr2.ent->zapdmgtime = level.time + 0.4;

				// Play a sizzle sound, but only when the burn is new...
				gi.sound (tr2.ent, CHAN_BODY, gi.soundindex ("Ambient/Gen/Electric/spark2.wav"), 1.0, ATTN_IDLE, 0);
			}
			else
			{
				damage = 2;
				tr2.ent->zapdmgtime = level.time + 0.2;
			}
		}
		else
		{	// The longer we hold it on the target, the more damage we do.
			if (dm->isDM())
			{
				damage = (tr2.ent->zapdmgtime - level.time)*30.0;
				tr2.ent->zapdmgtime += 0.4;
			}
			else
			{
				damage = (tr2.ent->zapdmgtime - level.time)*15.0;
				tr2.ent->zapdmgtime += 0.2;
			}
		}

		T_Damage(tr2.ent, wf.ent, wf.ent, dir, wf.firePoint, wf.firePoint, damage,  0/*knockback*/, DT_SHOCK, MOD_MPG, wf.weapon->getPenetrate(), wf.weapon->getAbsorb());
		Electrocute(tr2.ent, wf.ent);

		// Bring down friction just a tad so he notices...
		if (tr2.ent->client)
		{
			tr2.ent->client->friction_time = level.time + 0.8;
		}

		VectorCopy(tr2.endpos, lastEndpos);

	}while((tr2.fraction < .999)&&(maxTest < 12)&&(tr2.ent != world));

	fxRunner.setPos2(tr2.endpos);

	if(wf.gunPointSet)
	{
		fxRunner.setDir(wf.fwd);

		if ((int)(level.time*10)&0x01)
		{
			fxRunner.exec("weapons/othermz/mpgbeam1", wf.gunPoint, MULTICAST_PHS, wf.ent);
		}
		else
		{
			fxRunner.exec("weapons/othermz/mpgbeam2", wf.gunPoint, MULTICAST_PHS, wf.ent);
		}
	}
	else
	{
		vec3_t gunpoint;
		vec3_t newfwd;

		if (dm->isDM() && wf.ent->ghoulInst)
		{
			GetGhoulPosDir(wf.ent->s.origin, wf.ent->s.angles, wf.ent->ghoulInst, 0, "wbolt_hand_l", gunpoint, 0, 0, 0);
			VectorSubtract(tr2.endpos, gunpoint, newfwd);
			VectorNormalize(newfwd);
		}
		else
		{
			VectorCopy(wf.firePoint, gunpoint);
			VectorCopy(wf.fwd, newfwd);
		}
		
		fxRunner.setDir(newfwd);

		if ((int)(level.time*10)&0x01)
		{
			fxRunner.exec("weapons/othermz/mpgbeam1", gunpoint, MULTICAST_PHS, wf.ent);
		}
		else
		{
			fxRunner.exec("weapons/othermz/mpgbeam2", gunpoint, MULTICAST_PHS, wf.ent);
		}
	}
}


void fireballRemove(edict_t *self)
{
	G_FreeEdict(self);
}

void fireballBurn(edict_t *self)
{
	T_RadiusDamage (self, self->owner, self->health*2, self, 100, MOD_FLAMEGUN_NAPALM, DT_FIRE|DAMAGE_NO_ARMOR);
	if (--(self->health) <= 0)
	{
		G_FreeEdict(self);
	}
	else
	{
		self->nextthink = level.time + 0.2;
	}
}

void fireballCollide(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t fwd;

	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	if (self->health <= 0 || (plane && plane->normal[2] > 0.5))
	{
		if(surf)
		{
			if(surf->flags & SURF_SKY)
			{
				G_FreeEdict(self);
				return;
			}
		}

		IncreaseSpawnIntensity(.3);
		T_RadiusDamage (self, self->owner, 300, self, 150, MOD_FLAMEGUN_NAPALM, DT_MANGLE|DT_FIRE);
		gmonster.RadiusDeafen(self, 300, 300);

		AngleVectors(self->s.angles, fwd, NULL, NULL);
		fxRunner.clearContinualEffects(self);
		
		fxRunner.setDir(fwd);
		fxRunner.exec("weapons/world/fballimpact", self->s.origin);

		VectorClear(self->velocity);
		self->movetype = MOVETYPE_NONE;
		self->think = fireballBurn;
		self->health = 15;
		self->nextthink = level.time + 0.2;
	}
	else
	{
		self->health--;
	}
}


void flameGlobThink(edict_t *ent)
{
	G_FreeEdict(ent);
}

void fireFlameGlob(weaponFireInfo_t &wf)
{
	edict_t *fireball;

	vec3_t up, right;

	fireball = G_Spawn();
	SetProjPhysics(*fireball, 1.0, (1300.0/800.0), 0, 0, 0.3);
	fireball->clipmask = CONTENTS_SOLID|CONTENTS_WINDOW;
	VectorScale(wf.fwd, 600.0, fireball->velocity);
	VectorSet (fireball->mins, -2, -2, -3);
	VectorSet (fireball->maxs, 2, 2, 3);
	fireball->health = 5;
	fireball->think = fireballRemove;
	fireball->nextthink = level.time + 10.0;
	fireball->touch = fireballCollide;
	vectoangles(wf.fwd, fireball->s.angles);

	AngleVectors(fireball->s.angles, wf.fwd, right, up);
	VectorMA(fireball->velocity, 100, up, fireball->velocity);

	// We want to add extra velocity UP, not the relative up.  However, I am meeting this halfway.
	fireball->velocity[2] += 150;
	fireball->owner = wf.ent;

	// I don't want the client to see this
	if(ProjMoveToStart(fireball, wf.ent->s.origin, wf.firePoint))
	{
		fireball->think = DelayedTouchFunction;		
		fireball->nextthink = level.time + 0.1;
	}

	fxRunner.execContinualEffect("weapons/world/fballtrail", fireball, 0, 1.0);

	if (dm->isDM())
	{
		// Send off muzzle flash effects.
		FX_HandleDMMuzzleFlash(wf.firePoint, wf.fwd, wf.ent, wf.inst, wf.weapon->getMFlashEfDM());
	}
}


void fireFlameThrower(weaponFireInfo_t &wf)
{
	// need to make the visual effect as well
	vec3_t flamemin={-1,-1,-1}, flamemax={1,1,1};

	fxRunner.setDir(wf.fwd);
	if(wf.gunPointSet)
	{
		fxRunner.exec("weapons/world/fthrower", wf.gunPoint, MULTICAST_PHS, wf.ent);
	}
	else
	{
		if (dm->isDM() && wf.ent->ghoulInst)
		{
			vec3_t gunpoint;

			GetGhoulPosDir(wf.ent->s.origin, wf.ent->s.angles, wf.ent->ghoulInst, 0, "wbolt_hand_l", gunpoint, 0, 0, 0);
			fxRunner.exec("weapons/world/fthrower", gunpoint, MULTICAST_PHS, wf.ent);
		}
		else
		{
			fxRunner.exec("weapons/world/fthrower", wf.firePoint, MULTICAST_PHS, wf.ent);
		}
	}
	
	trace_t			tr2;// this one will determine who needs to hear a nice whizzing sound
	vec3_t			lastEndpos;
	edict_t			*buddy;
	int				maxTest = 0;
	vec3_t			end, diff, dir;
	vec3_t			minmove;

	buddy = wf.ent;
	VectorCopy(wf.firePoint, lastEndpos);
	VectorCopy(wf.fwd, dir);		// I do this to protect the contents of wf.fwd, which could have been mangled by T_Damage, though it should be fixed now.  Sigh.  --Pat

	// First check the validity of the flamethrower's starting location...
	gi.trace(wf.ent->s.origin, flamemin, flamemax, wf.firePoint, wf.ent, MASK_PROJ, &tr2);
	if (tr2.startsolid)
	{	// Do nothing but the muzzle flash...
		return;
	}
	else if (tr2.fraction < 0.99)
	{	
		if (tr2.ent == world)
		{	// Don't go any farther...
			return;
		}
		else if (tr2.ent && tr2.ent->takedamage)
		{
			if (dm->isDM())
			{	// In multiplayer, damage is higher.
				T_Damage(tr2.ent, wf.ent, wf.ent, dir, wf.firePoint, wf.firePoint, 16, 0, DT_FIRE|DAMAGE_NO_KNOCKBACK, MOD_FLAMEGUN, wf.weapon->getPenetrate(), wf.weapon->getAbsorb());
			}
			else
			{
				T_Damage(tr2.ent, wf.ent, wf.ent, dir, wf.firePoint, wf.firePoint, 10, 0, DT_FIRE|DAMAGE_NO_KNOCKBACK, MOD_FLAMEGUN, wf.weapon->getPenetrate(), wf.weapon->getAbsorb());
			}
		}
	}

	if (dm->isDM())
	{	// Range of FT in DM is 325
		VectorMA(wf.firePoint, 325, wf.fwd, end);
	}
	else
	{	// Range of FT is 280
		VectorMA(wf.firePoint, 280, wf.fwd, end);
	}

	VectorScale(wf.fwd, 8, minmove);
	// Because I am lame, I'm going to subtract 8 here to counteract the first add...
	VectorSubtract(lastEndpos, minmove, lastEndpos);

	do
	{
		VectorAdd(lastEndpos, minmove, lastEndpos);

		gi.trace (lastEndpos, flamemin, flamemax, end, buddy, MASK_PROJ, &tr2);
		buddy = tr2.ent;

		maxTest++;

		if(!tr2.ent)continue;
		if(tr2.ent == wf.ent)continue;
		if(tr2.ent->solid == SOLID_BSP && ((int)(level.time*10))&0x01)		// Every other time
		{
			VectorSubtract(wf.firePoint, tr2.endpos, diff);
			if (VectorLength(diff) < 175)		// Only do a hit on a wall after 3/4 the distance...
			{
				fxRunner.setDir(tr2.plane.normal);
				fxRunner.exec("weapons/world/flamehitwall", tr2.endpos);
			}
		}
		if(!tr2.ent->takedamage)continue;

		if (dm->isDM())
		{	// In multiplayer, damage is higher.
			T_Damage(tr2.ent, wf.ent, wf.ent, dir, wf.firePoint, wf.firePoint, 16, 0, DT_FIRE|DAMAGE_NO_KNOCKBACK, MOD_FLAMEGUN, wf.weapon->getPenetrate(), wf.weapon->getAbsorb());
		}
		else
		{
			T_Damage(tr2.ent, wf.ent, wf.ent, dir, wf.firePoint, wf.firePoint, 10, 0, DT_FIRE|DAMAGE_NO_KNOCKBACK, MOD_FLAMEGUN, wf.weapon->getPenetrate(), wf.weapon->getAbsorb());
		}
		VectorCopy(tr2.endpos, lastEndpos);

		if (((int)(level.time*10))&0x01)		// Every other time
		{
			Ignite(tr2.ent, wf.ent, 1);
		}
	}while((tr2.fraction < .999)&&(maxTest < 3)&&(tr2.ent != world));
}

void fireDogAttack(weaponFireInfo_t &wf)
{
	trace_t			tr;
	vec3_t			tempPos, tempPos2;
	int				weaponDamage;
	float			fNow = level.time;

	weaponDamage = wf.weapon->getDmg();

	VectorMA(wf.firePoint, 48, wf.fwd, tempPos);
	VectorMA(wf.firePoint, -16, wf.fwd, tempPos2);

	gi.trace (tempPos2, NULL, NULL, tempPos, wf.ent, MASK_SHOT| MASK_WATER, &tr);

	if(tr.fraction < 1.0)
	{
		//PerformWallDamage(tr, wf.fwd, 6, WMRK_SLASH,false);//fixme
	}

	if(tr.ent)
	{
		// let our body know we hit someone
		if (tr.fraction < 1.0 && wf.hitfunc)
		{
			wf.hitfunc(wf.ent, &fNow);
		}

		if(tr.ent->takedamage)
		{
			T_Damage (tr.ent, wf.ent, wf.ent, wf.fwd, tr.endpos, tempPos2, weaponDamage, 100, DT_MELEE, MOD_DOGBITE);
		}
	}
}

void molitovCollide(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if(game.cinematicfreeze)
	{	// We are in a cinematic, remove all volatile objects.
		G_FreeEdict(self);
		return;
	}

	//fxRunner.exec("weapons/world/molitovexplode", self->s.origin);
	fxRunner.exec("weapons/world/molitovburst", self->s.origin);

	// Do the damage before burning people.
	T_RadiusDamage (self, self->owner, 50, self, 150, MOD_FIRE, DT_FIRE);
	RadiusBurn(self, 150);

	G_FreeEdict(self);
}

void MolitovThink(edict_t *self)
{	// this shouldn't happen
	G_FreeEdict(self);
}

void fireMolitov(weaponFireInfo_t &wf)
{
	edict_t *missile;
	vec3_t	ang;
	vec3_t	up, right;

	vectoangles(wf.fwd, ang);
	AngleVectors(ang, NULL, right, up);

	missile = G_Spawn();
	game_ghoul.SetSimpleGhoulModel(missile, "Items/Projectiles", "w_molitov");

	SetProjPhysics(*missile, 0.7, 1.0, 0, 0, 0);
	VectorCopy(wf.firePoint, missile->s.origin);
	VectorScale(wf.fwd, wf.vel, missile->velocity);
	VectorSet (missile->mins, -2, -2, -3);
	VectorSet (missile->maxs, 2, 2, 3);
	missile->think = MolitovThink;
	missile->nextthink = level.time + 4.0;
	VectorCopy(wf.ent->s.angles, missile->s.angles);
	gi.linkentity(missile);
	missile->owner = wf.ent;
//	missile->avelocity[PITCH] = 300;//?
//	missile->avelocity[YAW] = 300;//?
	missile->avelocity[ROLL] = 300;//?
	missile->touch = molitovCollide;

	//fxRunner.execContinualEffect("weapons/world/molitovtrail", missile);
	fxRunner.execContinualEffect("weapons/world/molitrail2", missile);

	if(SV_TestEntityPosition(missile))
	{
		missile->think = DelayedTouchFunction;		
		missile->nextthink = level.time + 0.1;
	}
}

void fireGasGren(weaponFireInfo_t &wf)
{	//fixme
	fireGrenade(wf);
}

void fireMagicBullet(weaponFireInfo_t &wf)
{
}

void fireAutogun(weaponFireInfo_t &wf)
{
	fireBullet(wf);
}

void flashpackFunc(edict_t &ent);

void fireFlashGren(weaponFireInfo_t &wf)
{
	flashpackFunc(*wf.ent);
}

void fireMachete(weaponFireInfo_t &wf)
{
	fireKnife(wf);
}

void fireDekker(weaponFireInfo_t &wf)
{
	trace_t			tr2;// this one will determine who needs to hear a nice whizzing sound
	vec3_t			lastEndpos;
	edict_t			*buddy;
	int				maxTest = 0;
	vec3_t			end;

	buddy = wf.ent;
	VectorCopy(wf.firePoint, lastEndpos);

	VectorMA(wf.firePoint, 1024, wf.fwd, end);

	do
	{
		gi.trace (lastEndpos, vec3_origin, vec3_origin, end, buddy, MASK_PROJ, &tr2);
		buddy = tr2.ent;

		maxTest++;

		if(!tr2.ent)continue;
		if(!tr2.ent->takedamage)continue;
		if(tr2.ent == wf.ent)continue;

		PerformWallDamage(tr2, wf.fwd, wf.weapon->getBulletSize(), WMRK_NONE,gi.irand(0,1));

		T_Damage(tr2.ent, wf.ent, wf.ent, wf.fwd, wf.firePoint, wf.firePoint, 240, 100, DT_NEURAL|DT_WATERZAP, MOD_MPG, wf.weapon->getPenetrate(), wf.weapon->getAbsorb());
		VectorCopy(tr2.endpos, lastEndpos);

	}while((tr2.fraction < .999)&&(maxTest < 12)&&(tr2.ent != world));

	fxRunner.setDir(wf.fwd);
	fxRunner.setPos2(tr2.endpos);
	fxRunner.exec("weapons/world/gb_dek2", wf.firePoint);
	gi.sound(wf.ent, CHAN_WEAPON, gi.soundindex("Enemy/Dekker/Fire.wav"), 1.0, ATTN_NONE, 0);//fixme - bounce
}



//-------------------------------------------------------------------------------------


// LineIntersectsPlane
//
// Feed it a parametric line (x0 + At, y0 + Bt, z0 + Ct) and a plane (Ax + By + Cz = D)
// and it returns the value of t for which the line intersects the plane. 
// To represent the line, use a point (vec3_t pPoint) and a vector parallel to the line
// (vec3_t vParallel), i.e. (pPoint[0] + vParallel[0]t, pPoint[1] + vParallel[1]t, pPoint[2] + vParallel[2]t).
// For the plane, you can use the plane's normal (vec3_t n) and a point on the plane (vec3_t pPlane) to
// represent the plane (n[0]*pPlane[0] + n[1]*pPlane[1] + n[2]*pPlane[2] = D).
 
float LineIntersectsPlane(vec3_t pPoint, vec3_t vParallel, vec3_t n, vec3_t pPlane)
{
	float D = DotProduct(n, pPlane);
	float P = DotProduct(pPoint, vParallel);
	float Q = DotProduct(n, n);

	return (D - P)/Q;
}

void FireServer(sharedEdict_t &sh, edict_t &ent, inven_c &inven)
{
	vec3_t			fwd;
	vec3_t			firePoint, pPoint;
	weaponInfo_c	*weaponInfo=&(*(((weaponInfo_c **)pe->weapInfo))[inven.getCurWeaponID()]);
	attacks_e		attackID = weaponInfo->getAtkID(0);
	float			noiseRad = weapons.getWeapon(attackID).getNoiseRad();

//	AngleVectors(ent.client->ps.viewangles, fwd, NULL, NULL);

	VectorAdd(ent.s.origin, ent.client->ps.viewoffset, firePoint);

	// ATTENTION!  Fwd is overwritten by this function...
	CalcFireDirection(&ent, fwd, weaponInfo->getWaver(0), weaponInfo->getWaverMax(0), attackID);

	if (weaponInfo->fireFromFlash())
	{	
		// Some weapons can't fire from the player center for visual reasons.
		// FIXME: this doesn't work at all for some reason ;(.

		vec3_t srcPoint;
		VectorCopy(firePoint, srcPoint);

		vec3_t temp;
		
		GetGhoulPosDir(firePoint, ent.client->ps.viewangles, inven.getViewModel(), 0, "flash", pPoint, temp, 0, 0);
/*		t = LineIntersectsPlane(pPoint, fwd, fwd, ent.s.origin);
		firePoint[0] = pPoint[0] + fwd[0]*t;
		firePoint[1] = pPoint[1] + fwd[1]*t;
		firePoint[2] = pPoint[2] + fwd[2]*t;
*/

		// We must project back from the fire position, into the body.
		{
			vec3_t firevect;
			float scalefactor;
			vec3_t viewpoint;

			VectorAdd(ent.s.origin, ent.client->ps.viewoffset, viewpoint);

			// Start with the vector from the firepoint to the origin.
			VectorSubtract(pPoint, viewpoint, firevect);

			// Now project the difference vector onto the fire vector.
			scalefactor = DotProduct(firevect, fwd);
			VectorMA(pPoint, -scalefactor, fwd, firePoint);
		}

		if (weaponInfo->aimAtCrosshair())
		{
			vec3_t projEnd;
			trace_t tr;

			VectorMA(srcPoint, 2048, fwd, projEnd);
			gi.trace(srcPoint, 0, 0, projEnd, &ent, MASK_PROJ, &tr);
			VectorSubtract(tr.endpos, firePoint, fwd);
			VectorNormalize(fwd);
		}
	}

	weapons.attack(weaponInfo->getAtkID(0), &ent, firePoint, fwd);


	PlayerNoise(&ent, firePoint, AI_SENSETYPE_SOUND_WEAPON, NULL, noiseRad, gmonster.GetClientNode());
}

void AltfireServer(sharedEdict_t &sh, edict_t &ent, inven_c &inven)
{
	vec3_t			fwd;
	vec3_t			firePoint;
	weaponInfo_c	*weaponInfo=&(*(((weaponInfo_c **)pe->weapInfo))[inven.getCurWeaponID()]);
	attacks_e		altAttackID = weaponInfo->getAtkID(1);
	float			noiseRad = weapons.getWeapon(altAttackID).getNoiseRad();

//	AngleVectors(ent.client->ps.viewangles, fwd, NULL, NULL);

	VectorAdd(ent.s.origin, ent.client->ps.viewoffset, firePoint);

	// ATTENTION!  Fwd is overwritten by this function...
	CalcFireDirection(&ent,fwd, weaponInfo->getWaver(1), weaponInfo->getWaverMax(1), altAttackID);

	if (weaponInfo->fireFromFlash())
	{	
		// Some weapons can't fire from the player center for visual reasons.
		vec3_t srcPoint;
		VectorCopy(firePoint, srcPoint);

		vec3_t temp;
		vec3_t pPoint;
		
		GetGhoulPosDir(firePoint, ent.client->ps.viewangles, inven.getViewModel(), 0, "flash", pPoint, temp, 0, 0);
/*		t = LineIntersectsPlane(pPoint, fwd, fwd, ent.s.origin);
		firePoint[0] = pPoint[0] + fwd[0]*t;
		firePoint[1] = pPoint[1] + fwd[1]*t;
		firePoint[2] = pPoint[2] + fwd[2]*t;
*/

		// We must project back from the fire position, into the body.
		{
			vec3_t firevect;
			float scalefactor;
			vec3_t viewpoint;

			VectorAdd(ent.s.origin, ent.client->ps.viewoffset, viewpoint);

			// Start with the vector from the firepoint to the origin.
			VectorSubtract(pPoint, viewpoint, firevect);

			// Now project the difference vector onto the fire vector.
			scalefactor = DotProduct(firevect, fwd);
			VectorMA(pPoint, -scalefactor, fwd, firePoint);
		}

		if (weaponInfo->aimAtCrosshair())
		{
			vec3_t projEnd;
			trace_t tr;

			VectorMA(srcPoint, 2048, fwd, projEnd);
			gi.trace(srcPoint, 0, 0, projEnd, &ent, MASK_PROJ, &tr);
			VectorSubtract(tr.endpos, firePoint, fwd);
			VectorNormalize(fwd);
		}
	}

	weapons.attack(weaponInfo->getAtkID(1), &ent, firePoint, fwd);

	PlayerNoise(&ent, firePoint, AI_SENSETYPE_SOUND_WEAPON, NULL, noiseRad, gmonster.GetClientNode());
}

void WeaponDropServer(edict_t *ent,int type, int clipSize)
{
	dm->clientDropWeapon(ent,type, clipSize);
}

void ItemDropServer(edict_t *ent,int type,int ammoCount)
{
	dm->clientDropItem(ent,type,ammoCount);
}

void CacheAttack(int atkID)
{
	if (atkID!=ATK_NOTHING)
	{
		weapons.getWeapon(atkID).cache();
	}
}