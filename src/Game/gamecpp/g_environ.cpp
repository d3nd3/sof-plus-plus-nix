#include "g_local.h"
#include "fields.h"
#include "ai_private.h"	// so we know what a bodyhuman_c is

#define DEFAULT(a, b) ((a) ? (a):(b))

#define FIRE_SILENT 2

#define CHUNKS_TRIGGER_SPAWN		(1<<0)


void SetGenericEffectInfo(edict_t *ent)
{	// get some good defaults here...
	ent->movetype = MOVETYPE_NONE;
	if(ent->solid != SOLID_TRIGGER)
	{
		ent->solid = SOLID_NOT;
	}
	ent->clipmask = MASK_MONSTERSOLID|MASK_PLAYERSOLID; 
	VectorSet (ent->mins, -5, -5, -7);//?
	VectorSet (ent->maxs, 5, 5, 12);
	ent->s.modelindex = 0;
	BboxRotate(ent);
	gi.linkentity (ent);
}

void SetGenericEffectInfoVBBox(edict_t *ent)
{
	SetGenericEffectInfo(ent);

	gi.setmodel (ent, ent->model);
	ent->s.modelindex = 0;

	ent->s.origin[0] = (ent->absmax[0] + ent->absmin[0])*.5;
	ent->s.origin[1] = (ent->absmax[1] + ent->absmin[1])*.5;
	ent->s.origin[2] = (ent->absmax[2] + ent->absmin[2])*.5;
}

void SetGenericEffectInfoVBBoxTop(edict_t *ent)
{
	SetGenericEffectInfo(ent);

	gi.setmodel (ent, ent->model);
	ent->s.modelindex = 0;

	ent->s.origin[0] = (ent->absmax[0] + ent->absmin[0])*.5;
	ent->s.origin[1] = (ent->absmax[1] + ent->absmin[1])*.5;
	ent->s.origin[2] = ent->absmax[2];
}


//----------------------------------------------------------------------------------------------------

/*QUAKED environ_snow (0 1 0) (-64 -64 0) (64 64 96) START_OFF
*/

void SP_environ_snow (edict_t *ent)
{
	SetGenericEffectInfo(ent);
	fxRunner.execContinualEffect("environ/snowchute", ent);
}

/*QUAKED environ_player_snow (0 1 0) (-12 -12 -12) (12 12 12)
"count" whether this should turn the snow on or off
*/

void SnowUse(edict_t *ent, edict_t *other, edict_t *activator)
{
	if(ent->count)
	{
		fxRunner.execContinualEffect("environ/playersnow", activator);
	}
	else
	{
		fxRunner.stopContinualEffect("environ/playersnow", activator);
	}
}

void SP_environ_player_snow(edict_t *ent)
{
	SetGenericEffectInfo(ent);
	ent->use = SnowUse;
}

//----------------------------------------------------------------------------------------------------

/*QUAKED environ_steamgen (0 1 0) (-12 -12 -12) (12 12 12) START_OFF
"height" size of the puffs - (default is 35)
"wait"	 time in seconds the steam should blow... set to zero for constant steam...(default is 0)
"delay"  time in seconds the steam should stop... set to zero for steam to go once
"dmg"	amount of damage the steam generator does to folks per second of exposure(def is 0)

Be certain to set the "angles" key or you will die

*/

#define START_OFF 1

void SteamUse(edict_t *ent, edict_t *other, edict_t *activator)
{
	if(fxRunner.hasEffect("environ/normalsteam", ent))
	{
		fxRunner.stopContinualEffect("environ/normalsteam", ent);
		ent->air_finished = 0;
	}
	else
	{
		fxRunner.execContinualEffect("environ/normalsteam", ent, 0, ent->health/35);
		ent->air_finished = level.time + ent->wait;
	}
	ent->nextthink = level.time + .1;
}

void SteamThink2(edict_t *ent)
{
	if(fxRunner.hasEffect("environ/normalsteam", ent))
	{
		fxRunner.stopContinualEffect("environ/normalsteam", ent);
		if(ent->delay)
		{
			ent->air_finished = level.time + ent->delay;
		}
		else
		{
			ent->air_finished = 0;
		}
	}
	else
	{
		fxRunner.execContinualEffect("environ/normalsteam", ent, 0, ent->health/35);
		ent->air_finished = level.time + ent->wait;
	}
}

void SteamThink(edict_t *ent)
{
	if(fxRunner.hasEffect("environ/normalsteam", ent))
	{
		if(ent->dmg)
		{	// do the damage here in a cone of some size... bleah
			T_ConeDamage(ent, ent, ent->dmg, ent, 96 * ent->health / 35.0, 0, ent->movedir, .7);//radius isn't gonna always be right...
		}
	}

	// air_finished is used as a seperate think function timer...
	if(ent->air_finished)
	{
		if(ent->air_finished < level.time)
		{
			SteamThink2(ent);
		}
	}
	ent->nextthink = level.time + .1;
}

void SP_environ_steamgen (edict_t *ent)
{
	SetGenericEffectInfo(ent);

	ent->health = st.height;

	if (ent->health == 0)
	{
		ent->health=35;
	}

	if(!(ent->spawnflags & START_OFF))
	{
		fxRunner.execContinualEffect("environ/normalsteam", ent, 0, ent->health/35);
	}

	AngleVectors(ent->s.angles, ent->movedir, 0, 0);

	ent->use = SteamUse;

	ent->think = SteamThink;
	ent->nextthink = level.time + .1;

	if(ent->wait)
	{
		ent->air_finished = level.time + ent->wait;
	}
	else
	{
		ent->air_finished = 0;
	}

	if(ent->spawnflags & START_OFF)
	{
		ent->air_finished = 0;
	}
}

//----------------------------------------------------------------------------------------------------

/*QUAKED environ_smokegen (0 1 0) (-12 -12 -12) (12 12 12) START_OFF
"height" size of the puffs - 60 is medium sized...
*/

void smoke_gen_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	if(fxRunner.hasEffect("environ/normalsmoke", ent))
	{
		fxRunner.stopContinualEffect("environ/normalsmoke", ent);
	}
	else
	{
		fxRunner.execContinualEffect("environ/normalsmoke", ent, 0, (60.0 / ent->health));
	}
}

void SP_environ_smokegen (edict_t *ent)
{
	SetGenericEffectInfo(ent);
	ent->health = st.height;

	if(!(ent->spawnflags & START_OFF))
	{
		fxRunner.execContinualEffect("environ/normalsmoke", ent, 0, (60.0 / ent->health));
	}

	ent->use = smoke_gen_use;
}

//----------------------------------------------------------------------------------------------------


#define ONE_SHOT 2

/*QUAKED environ_sparkgen (0 1 0) (-12 -12 -12) (12 12 12) START_OFF ONE_SHOT
delay - frequency of sparks shooting out in seconds
wait  - randomness to how often the sparks shoot out in seconds(should be less than delay)
lip	  - color of sparks... 0 = yellow, 1 = blue
use angles to determine what direction you want the little folks to go
*/

void spark_gen_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	ent->nextthink = level.time + ent->delay + (gi.flrand(0.0F, 1.0F)*(ent->wait)) - (ent->wait/2);
}

void spark_gen_think(edict_t *ent)
{
	vec3_t	dir;

	AngleVectors(ent->s.angles, dir, NULL, NULL);

	FX_MakeSparks(ent->s.origin, dir, ent->dmg);
	if(ent->spawnflags & ONE_SHOT)
	{
		ent->nextthink = 0;
	}
	else
	{
		ent->nextthink = level.time + ent->delay + (gi.flrand(0.0F, 1.0F)*(ent->wait)) - (ent->wait/2);
	}
}

void SP_environ_sparkgen (edict_t *ent)
{
	SetGenericEffectInfo(ent);

	ent->think = spark_gen_think;
	ent->dmg = st.lip;

	if((!(ent->spawnflags & START_OFF))&&(!(ent->spawnflags & ONE_SHOT)))
	{
		ent->nextthink = level.time + ent->delay + (gi.flrand(0.0F, 1.0F)*(ent->wait)) - (ent->wait/2);
	}
	else
	{
		ent->nextthink = 0;
	}
	ent->use = spark_gen_use;
}

//----------------------------------------------------------------------------------------------------

/*QUAKED environ_waterglobgen (0 1 0) ? START_OFF
Drips from a 30 by 30 region around the origin of the entity
*/

void SP_environ_waterdripper (edict_t *ent)
{
	SetGenericEffectInfoVBBoxTop(ent);
	fxRunner.execContinualEffect("environ/waterdrip", ent);
}

//----------------------------------------------------------------------------------------------------

/*QUAKED environ_raingen (0 1 0) ? START_OFF
"count" - amount of rain(100 is default)
"wait" - length of the rain pieces(90 is default)
"delay" - speed that the rain falls at(90 is default)
"lip" - light level(200 is default)
*/

void raingen_use(edict_t *ent, edict_t *other, edict_t *activator)
{
}

void SP_environ_raingen (edict_t *ent)
{
	SetGenericEffectInfoVBBox(ent);
}

/*QUAKED environ_rain_worldtrigger (0 1 0) (-12 -12 -12) (12 12 12)
"count" - amount of rain(100 is default)
"wait" - length of the rain pieces(90 is default)
*/

void rainTrigger_use(edict_t *ent, edict_t *other, edict_t *activator)
{
	edict_t		*search;

	search = NULL;
	while ((search = G_Find (search, FOFS(classname), "environ_raingen")))
	{
	}
}

void SP_environ_rain_worldtrigger(edict_t *ent)
{
	SetGenericEffectInfo(ent);
	ent->use = rainTrigger_use;
}

/*QUAKED environ_cloudtrigger (0 1 0) (-12 -12 -12) (12 12 12)
"wait" - density of clouds (default = .1)
"count" speed of the clouds (default 10)
*/

void cloudTriggerUse(edict_t *ent, edict_t *other, edict_t *activator)
{
}

void SP_environ_cloudtrigger(edict_t *ent)
{
	SetGenericEffectInfo(ent);
	ent->wait = DEFAULT(ent->wait, .1);
	ent->count = DEFAULT(ent->count, 10);
	ent->use = rainTrigger_use;
}

//----------------------------------------------------------------------------------------------------

/*QUAKED environ_windgen (0 1 0) (-12 -12 -12) (12 12 12) START_OFF
not in yet, chump
*/

//----------------------------------------------------------------------------------------------------

/*QUAKED environ_bloodrain (0 1 0) (-4 -4 -4) (4 4 4)
Rains from a 30 by 30 region. Put the bloodrain entity 
at the x-y center of the region you 
want the blood to rain on. Put it at the height you
want the rain to start at.
*/

void SP_environ_bloodrain (edict_t *ent)
{
	SetGenericEffectInfo(ent);
	fxRunner.execContinualEffect("environ/bloodrain", ent);
}

//----------------------------------------------------------------------------------------------------

/*QUAKED environ_linefountain (1 0 1) (-8 -8 -8) (8 8 8) START_OFF
count - power of the fountain (between 0 and 635)
Be certain to set the "angles" key or you will die
*/
/*
This function is designed to send a simulated particle through the world,
subject to the input physics.  It then returns the floor level.  This will
(up to 255, obviously), representing the time before the test particle smacks something.

Since velocities are expressed in units/second, they are scaled by .01.
*/

extern short TestforFloor (vec3_t origin, vec3_t vel, vec3_t accel, edict_t* ignore)
{
	vec3_t		startpoint, midpoint,endpoint;
	edict_t*	obstacle = NULL;
	trace_t		tr1, tr2;
	int			floor;
	int			accelcount;

	VectorCopy(origin, startpoint);
	VectorScale(vel, .005, vel);
	VectorScale(accel, .2, accel);
	accelcount = 0;
	while (obstacle == NULL)
	{
		accelcount++;
		if (accelcount >= 20) //each pass through this loop simulates .01 seconds
			//accelcount should be adjusted to simulate how often acceleration
			//is going to be applied.  For normal physics, this is every .1 seconds,
			//so accelcount would be 10.
		{
			VectorAdd(vel, accel, vel);
			accelcount = 0;
		}
		VectorAdd(startpoint, vel, midpoint);
		VectorAdd(midpoint, vel, endpoint);
		gi.trace (startpoint, NULL, NULL, midpoint, ignore, MASK_SOLID | MASK_WATER, &tr1);
		gi.trace (midpoint, NULL, NULL, endpoint, ignore, MASK_SOLID| MASK_WATER, &tr2);
		if (tr1.fraction<1)
		{
			obstacle = tr1.ent;
			floor = tr1.endpos[2];
		}
		else if (tr2.fraction<1)//to avoid round-off problems
		{
			obstacle = tr2.ent;
			floor = tr2.endpos[2];
		}
		VectorCopy(endpoint, startpoint);
	}
	return floor;
}

void linefountainUse(edict_t *ent, edict_t *other, edict_t *activator)
{
	if(fxRunner.hasEffect("environ/stream", ent))
	{
		fxRunner.stopContinualEffect("environ/stream", ent);
	}
	else
	{
		fxRunner.execContinualEffect("environ/stream", ent, 0, ent->count/50);
	}
}

void SP_environ_linefountain (edict_t *ent)
{
	SetGenericEffectInfo(ent);
	ent->use = linefountainUse;

	if(!(ent->spawnflags & START_OFF))
	{
		fxRunner.execContinualEffect("environ/stream", ent, 0, ent->count/50);
	}
	else
	{
		gi.effectindex("environ/stream");
	}
}

//----------------------------------------------------------------------------------------------------

/*QUAKED environ_splashfountain (0 1 0) (-8 -8 -8) (8 8 8)
Nope, doesn't work yet, so don't use it ;(
*/
void SP_environ_splashfountain (edict_t *ent)
{
	vec3_t			vel;

	AngleVectors(ent->s.angles, vel, NULL, NULL);
	VectorScale(vel, ent->count, vel);

	SetGenericEffectInfo(ent);
}

/*QUAKED environ_lightning (0 1 0) (-8 -8 -8) (8 8 8) START_OFF
random = how often the lightning should strike, roughly, in seconds... - default 10...

triggering this will turn it off and on
*/

void LighteningGenUse(edict_t *ent, edict_t *other, edict_t *activator)
{
	if(ent->nextthink != 0)
	{
		ent->nextthink = 0;
	}
	else
	{
		ent->nextthink = level.time + gi.flrand(ent->random * .5, ent->random * 1.5);
	}
}

void LightningGenThink(edict_t *ent)
{
	FX_MakeSkyLightning();
	ent->nextthink = level.time + gi.flrand(ent->random * .5, ent->random * 1.5);
}

void SP_environ_lightning(edict_t *ent)
{
	SetGenericEffectInfo(ent);
	ent->think = LightningGenThink;
	ent->use = LighteningGenUse;
	ent->nextthink = 0;
	ent->random = DEFAULT(ent->random, 10);
	if(ent->spawnflags & START_OFF)return;
	ent->nextthink = level.time + gi.flrand(ent->random * .5, ent->random * 1.5);
}

/*QUAKED environ_emergency_lights (0 1 0) (-8 -8 -8) (8 8 8)
*/

void EmergencyLightStutter(edict_t *ent);

void EmergencyLightReturn(edict_t *ent)
{
	ent->think = EmergencyLightStutter;
	ent->nextthink = level.time + gi.flrand(2.0, 10.0);
	SetSkyColor((float)ent->health *.01, 0, 0);
}

void EmergencyLightStutter(edict_t *ent)
{
	ent->nextthink = level.time + .1;
	ent->think = EmergencyLightReturn;
	SetSkyColor((float)ent->health * gi.flrand(.001, .005), 0, 0);
}

void EmergencyLightRedGlow(edict_t *ent)
{
	ent->health += 10;
	ent->nextthink = level.time + .1;
	SetSkyColor((float)ent->health *.01, 0, 0);
	if(ent->health > 70)
	{
		ent->think = EmergencyLightStutter;
		ent->nextthink = level.time + gi.flrand(2.0, 10.0);
	}
}

void EmergencyLightFade(edict_t *ent)
{
	ent->health -= 10;
	ent->nextthink = level.time + .1;
	SetSkyColor((float)ent->health *.01, (float)ent->health *.01, (float)ent->health *.01);
	if(ent->health < 10)
	{
		ent->think = EmergencyLightRedGlow;
	}
}

void emergency_lights_use(edict_t *ent, edict_t *other, edict_t *activator)
{
	ent->think = EmergencyLightFade;
	ent->nextthink = level.time + .1;
}

void SP_environ_emergency_lights(edict_t *ent)
{
	SetGenericEffectInfo(ent);
	ent->think = NULL;
	ent->health = 100;
	SetSkyColor(1.0, 1.0, 1.0);
	ent->use = emergency_lights_use;
}

/*QUAKED environ_skyfade (0 1 0) (-8 -8 -8) (8 8 8) START_OFF
"color" initial color of the sky in red green blue format, from 0.0 to 2.0, with 1.0 being normal (like "1.0 1.0 1.0" for white)
"endpoint" like color, but this is the ending color
"delay" time in seconds for the fade to take
*/

void skyfadeThink(edict_t *ent)
{
	float ratio, invRatio;
	vec3_t	fromc, toc;

	if(level.time > ent->health + ent->delay)
	{
		G_FreeEdict(ent);
		return;
	}

	ratio = (level.time - ent->health)/(ent->delay);
	invRatio = 1.0 - ratio;

	VectorScale(ent->intend_avelocity, ratio, toc);
	VectorScale(ent->intend_velocity, 1.0 - ratio, fromc);

	SetSkyColor(fromc[0] + toc[0], fromc[1] + toc[1], fromc[2] + toc[2]);

	ent->nextthink = level.time + ent->delay/100.0;
}

void skyfadeUse(edict_t *ent, edict_t *other, edict_t *activator)
{
	ent->health = level.time;
	ent->think = skyfadeThink;
	ent->nextthink = level.time + ent->delay/100.0;
}

void SP_environ_skyfade(edict_t *ent)
{
//	intend_velocity;		-> used as start color
//	intend_avelocity;		-> used as final color
	SetGenericEffectInfo(ent);
	VectorCopy(st.color, ent->intend_velocity);
	VectorCopy(st.endpoint, ent->intend_avelocity);

	if(ent->spawnflags & START_OFF)
	{
		ent->think = NULL;
		ent->nextthink = 0;
	}
	else
	{
		ent->health = level.time;
		ent->think = skyfadeThink;
		ent->nextthink = level.time + ent->delay/100.0;
	}
}

/*QUAKED environ_explosion (0 1 0) (-8 -8 -8) (8 8 8) NO_BLIND NO_DAMAGE
"health" size of the explosion (default 150)
*/

#define NO_BLIND 1

void explodeUse(edict_t *ent, edict_t *other, edict_t *activator)
{
	vec3_t	pos;

	VectorCopy(ent->s.origin, pos);
	pos[2] += 10;

	if(!(ent->spawnflags & 2))
	{	// flag 2 is the no damage flag - this is more for cinematic stuff..
		T_RadiusDamage (ent, ent, ent->health*2, ent, ent->health, 0, DT_MANGLE);
		RadiusBurn(ent, ent->health*2);
	}
	gmonster.RadiusDeafen(ent, ent->health*2, ent->health*2);

	fxRunner.exec("weapons/world/airexplodemute", ent->s.origin);
	// play a good explosion sound
	float xattenuation = ATTN_NORM, xvolume = 1.0;
	switch(gi.irand(1, 3))
	{
	case 1:
		gi.positioned_sound (ent->s.origin, g_edicts, 0, gi.soundindex("impact/explosion/exp1.wav"), xvolume, xattenuation, 0);
		break;
	case 2:
		gi.positioned_sound (ent->s.origin, g_edicts, 0, gi.soundindex("impact/explosion/exp2.wav"), xvolume, xattenuation, 0);
		break;
	case 3:
		gi.positioned_sound (ent->s.origin, g_edicts, 0, gi.soundindex("impact/explosion/exp3.wav"), xvolume, xattenuation, 0);
		break;
	}
	FX_C4Explosion(ent);

	if(!(ent->spawnflags & NO_BLIND))
	{
		BlindingLight(pos, ent->health*10, 0.9, 0.5);
	}

	ShakeCameras (ent->s.origin, ent->health, ent->health*2, 100);
}

void SP_environ_explosion(edict_t *ent)
{
	SetGenericEffectInfo(ent);
	ent->use = explodeUse;
	ent->health = DEFAULT(ent->health, 150);

	// cache sound CLSFX_EXPLODE
	entSoundsToCache[CLSFX_EXPLODE] = 1;
	// cache debris CLGHL_EXPLODEBIG, CLGHL_EXPLODESML
	entDebrisToCache[CLGHL_EXPLODEBIG] = DEBRIS_YES;
	entDebrisToCache[CLGHL_EXPLODESML] = DEBRIS_YES;

	// precache possible explosion sounds
	gi.soundindex("impact/explosion/exp1.wav");
	gi.soundindex("impact/explosion/exp2.wav");
	gi.soundindex("impact/explosion/exp3.wav");
	gi.effectindex("weapons/world/airexplodemute");
}

/*QUAKED environ_smoke_burst (0 1 0) (-8 -8 -8) (8 8 8)
"health" size of the smoke burst (default 50)
*/

void smokeBurstUse(edict_t *ent, edict_t *other, edict_t *activator)
{
	FX_SmokeBurst(ent->s.origin, ent->health);
}

void SP_environ_smoke_burst(edict_t *ent)
{
	SetGenericEffectInfo(ent);
	ent->use = smokeBurstUse;
	ent->health = DEFAULT(ent->health, 50);
}

/*QUAKED environ_fire (0 1 0) (-8 -8 -8) (8 8 8) START_OFF SILENT
a fire
-------KEYS--------
health - size of the fire burst (default is 12)
count -  lifetime of the fire in 100th's of a second (default is 64)
delay - life of actual fire in seconds from being triggered (default is 0, which means don't turn off)
----SPAWNFLAGS---
START_OFF - starts off
SILENT - makes no noise (use this on fires that will always be far away from player)
*/

char *getFireEffect(int size)
{
	if(size < 4)
	{
		return "environ/firesmall2";
	}
	else if(size < 8)
	{
		return "gbfire";
	}
	else if(size < 14)
	{
		return "gbfire";
	}
	else if(size < 20)
	{
		return "gbfire";
	}
	else if(size < 30)
	{
		return "environ/firelarge2";
	}
	else
	{
		return "environ/firelarge3";
	}
}


void fire_use(edict_t *ent, edict_t *other, edict_t *activator)
{
	if(ent->wait)
	{
		fxRunner.stopContinualEffect(getFireEffect(ent->health), ent);
		ent->s.sound = 0;
		ent->wait = 0;
	}
	else
	{
		if (!(ent->spawnflags & FIRE_SILENT))
		{
			if (ent->health > 20)
				ent->s.sound = gi.soundindex("Ambient/Gen/FireFX/FireBig.wav");
			else if (ent->health > 10)
				ent->s.sound = gi.soundindex("Ambient/Gen/FireFX/FireMed.wav");
			else
				ent->s.sound = gi.soundindex("Ambient/Gen/FireFX/FireSmall.wav");
			ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		}
		fxRunner.execContinualEffect(getFireEffect(ent->health), ent);
		ent->max_health = level.time;
		ent->wait = 1;
	}
}

void FireThink(edict_t *ent)
{
	if((ent->delay)&&(level.time - ent->max_health < ent->delay))
	{
	}

	ent->nextthink = level.time + .1;
}

void environ_fire_touch (edict_t *ent, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	vec3_t		up = {0,0,1};
	vec3_t		point;

//	if ((other->s.origin[2] + other->mins[2]) > ent->s.origin[2])
	{
		VectorSet(point, other->s.origin[0], other->s.origin[1], other->absmin[2]);
		T_Damage(other, ent, ent, up, point, point, 1, 0, DT_FIRE|DAMAGE_NO_ARMOR, MOD_FIRE);	
	}
	return;
}


void SP_environ_fire(edict_t *ent)
{
	SetGenericEffectInfo(ent);

	ent->delay = DEFAULT(ent->delay, 0);
	ent->health = DEFAULT(ent->health, 12);
	ent->count = DEFAULT(ent->count, 64);

	ent->use = fire_use;

	if (ent->spawnflags & START_OFF)
	{
		gi.effectindex(getFireEffect(ent->health));
		ent->wait = 0;
	}
	else
	{
		fxRunner.execContinualEffect(getFireEffect(ent->health), ent);
		ent->wait = 1;
		if (!(ent->spawnflags & FIRE_SILENT))
		{
			if (ent->health > 20)
				ent->s.sound = gi.soundindex("Ambient/Gen/FireFX/FireBig.wav");
			else if (ent->health > 10)
				ent->s.sound = gi.soundindex("Ambient/Gen/FireFX/FireMed.wav");
			else
				ent->s.sound = gi.soundindex("Ambient/Gen/FireFX/FireSmall.wav");
	 		ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		}
	}

	if(ent->delay)
	{
		ent->think = FireThink;
		ent->nextthink = level.time + .1;
	}
	ent->max_health = level.time;//this is gross.

	gi.soundindex("Ambient/Gen/FireFX/FireBig.wav");
	gi.soundindex("Ambient/Gen/FireFX/FireMed.wav");
	gi.soundindex("Ambient/Gen/FireFX/FireSmall.wav");
	gi.effectindex("environ/firesmall2");
	gi.effectindex("gbfire");
	gi.effectindex("environ/firelarge2");
	gi.effectindex("environ/firelarge3");
}

/*QUAKED environ_linetrap (0 1 0) (-8 -8 -8) (8 8 8) SINGLE_TRIGGER START_OFF DAMAGE
A red laser beam trip wire.  Runs from point origin till it hits something.
------ SPAWNFLAGS ------
SINGLE_TRIGGER - goes away after tripped once
START_OFF - line won't turn on until triggered
DAMAGE - do damage
------ KEYS ------
"angles" - Direction of the beam from where it starts...
dmg - amount of damage done by beam (default 2)
*/

#define SINGLE_TRIGGER		1
#define LINETRAP_STARTOFF	2
#define LINETRAP_DAMAGE		4

void lineTrapUse(edict_t *ent, edict_t *other, edict_t *activator)
{
	if(ent->nextthink)
	{
		ent->nextthink = 0;
	}
	else
	{
		ent->nextthink = level.time + .1;
	}
}


void linetrapThink(edict_t *ent)
{
	trace_t			tr;

	gi.trace (ent->s.origin, NULL, NULL, ent->pos1, ent, MASK_PLAYERSOLID, &tr);
	if(tr.fraction < 1.0)
	{
		if(tr.ent != world)
		{
			G_UseTargets (ent, tr.ent);
			if(ent->spawnflags & SINGLE_TRIGGER)
			{
				ent->nextthink = 0;
				ent->think = 0;
			}
		}
	}

	if (ent->spawnflags & LINETRAP_DAMAGE)
	{
		if (tr.ent->client)
		{
			T_Damage (tr.ent, ent, ent, vec3_origin, tr.ent->s.origin, tr.ent->s.origin, ent->dmg, ent->dmg, 0, MOD_TRIGGER_HURT);
		}
	}


	if (ent->think)
	{
		fxRunner.setPos2(ent->pos1);
		fxRunner.exec("environ/tripbeam", ent->s.origin);
		ent->nextthink = level.time + .1;
	}
}

void SP_environ_linetrap(edict_t *ent)
{
	SetGenericEffectInfo(ent);

	vec3_t	end;
	vec3_t	dir;
	trace_t	tr;

	AngleVectors(ent->s.angles, dir, NULL, NULL);
	VectorMA(ent->s.origin, 2000, dir, end);

	gi.trace (ent->s.origin, NULL, NULL, end, ent, MASK_PLAYERSOLID, &tr);

	if(tr.fraction > .99)return;
	VectorCopy(tr.endpos, ent->pos1);

	if(ent->spawnflags & LINETRAP_STARTOFF)
	{
		ent->nextthink = 0;
	}
	else
	{
		ent->nextthink = level.time + .1;
	}
	
	if (ent->spawnflags & LINETRAP_DAMAGE)
	{
		if (!ent->dmg)
		{
			ent->dmg = 2;
		}
	}

	ent->think = linetrapThink;
	ent->use = lineTrapUse;
}

/*QUAKED environ_inferno (0 1 0) ? START_OFF
"count" intensity(default 100)
"delay" windspeed (default 0)

"angles" is the direction of the wind
*/

void infernoUse(edict_t *ent, edict_t *other, edict_t *activator)
{
}

void infernoThink(edict_t *ent)
{
	ent->nextthink = level.time + .3;

	RadiusBurn(ent, ent->health);
}

void infernoTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{	//stolen from triggerhurt::touch
}

void SP_environ_inferno (edict_t *ent)
{
//	eft_rain_t *rain;

	ent->solid = SOLID_TRIGGER;
	SetGenericEffectInfoVBBox(ent);

//	ent->movetype = MOVETYPE_NONE;
//	gi.setmodel (ent, ent->model);

	ent->health = 100;//kef -- totally guessing  

	// need a sound or something?

//	ent->think = infernoThink;
//	ent->nextthink = level.time + .3;

	ent->use = infernoUse;
	ent->classname = "environ_inferno";

	ent->touch = infernoTouch;
	gi.effectindex("environ/onfireburst");
}


/*QUAKED environ_invisible_attack (0 1 0) (-8 -8 -8) (8 8 8) START_OFF NO_DAMAGE
"count" is the attack type (I'll have a list somewhere for you guys to get the right attack - or just ask me)(default is 3, which is the glock)
"delay" is how long between shots in seconds (default is .2)
"random" how much the shots should waver, in degrees (default is 5)
"angles" is the direction of the attack
"wait" of -1 will pop the head off of "target"

trigger this to turn it on and off
*/

int evilHackForInvisAttack = 0;//make weapons do no damage if field is set.  Sigh.

// only used when you want an invis_attack to shoot some poor slob in the head
void invis_attack_headshot(edict_t *ent)
{
	vec3_t	TargetPoint;
	Matrix4	EntityToWorld;
	Matrix4	BoltToEntity;
	Matrix4	BoltToWorld;
	GhoulID	Bolt = NULL_GhoulID;
	vec3_t fpos, forward;
	float vel = 500.0;
	edict_t	*myTarget = NULL;

	VectorCopy(ent->s.origin, fpos);	
	if (ent->target && (myTarget = G_PickTarget(ent->target)))
	{
		VectorCopy(myTarget->s.origin, TargetPoint);
		Bolt = myTarget->ghoulInst->GetGhoulObject()->FindPart("sbolt_mouth");
		if (Bolt)
		{
			// get the transformation from entity space to world space
			EntToWorldMatrix(myTarget->s.origin, myTarget->s.angles, EntityToWorld);
			// get the transformation from bolt space to entity space
			myTarget->ghoulInst->GetBoltMatrix(level.time,BoltToEntity,Bolt,IGhoulInst::MatrixType::Entity);
			// multiply to get transform from bolt space to world space
			BoltToWorld.Concat(BoltToEntity, EntityToWorld);
			// 3rd row of BoltToWorld is the world coordinates of the bolt
			BoltToWorld.GetRow(3,*(Vect3 *)TargetPoint);
			VectorSubtract(TargetPoint, fpos, forward); // firing vector will be targetted point (defaults to origin)

			ai_c *target_ai=(ai_c*)((ai_public_c*)myTarget->ai);
			bodyorganic_c* targetbody = NULL;
			if (target_ai)
			{
				if ( targetbody = ((bodyorganic_c*)target_ai->GetBody()) )
				{
					targetbody->NextShotsGonnaKillMe(NEXTSHOT_EXPLODE_MY_NUG); 
					weapons.attack((attacks_e)ent->count, ent, ent->s.origin, forward, 0, &vel);
				}
			}
		}
	}
	G_FreeEdict(ent);
}

void invis_attack_use(edict_t *ent, edict_t *other, edict_t *activator)
{
	// if wait == -1 then fire this attack once to ventilate the cranium of our target
	if (-1 == ent->wait)
	{
		ent->nextthink = 0;
		ent->think = NULL;
		invis_attack_headshot(ent);
	}
	else
	{
		if(ent->health)
		{
			ent->health = 0;
			ent->nextthink = 0;
		}
		else
		{
			ent->health = 1;
			ent->nextthink = level.time + ent->delay;
		}
	}
}

void invis_attack_think(edict_t *ent)
{
	vec3_t dir;
	vec3_t tempAng;

	float vel = 500.0;

	if (-1 == ent->wait)
	{
		ent->nextthink = 0;
		ent->think = NULL;
		invis_attack_headshot(ent);
		return;
	}

	ent->nextthink = level.time + ent->delay;

	VectorCopy(ent->s.angles, tempAng);
	for(int i = 0; i < 3; i++)tempAng[i] += gi.flrand(-ent->random, ent->random);

	if(ent->spawnflags & 2)
	{//no damage
		evilHackForInvisAttack = 1;
	}

	AngleVectors(tempAng, dir, 0, 0);
	weapons.attack((attacks_e)ent->count, ent, ent->s.origin, dir, 0, &vel);

	if(ent->spawnflags & 2)
	{
		evilHackForInvisAttack = 0;
	}
}

void SP_environ_invisible_attack(edict_t *ent)
{
	SetGenericEffectInfo(ent);

	ent->delay = DEFAULT(ent->delay, .2);
	ent->count = DEFAULT(ent->count, 3);
	ent->random = DEFAULT(ent->random, 5);
	ent->think = invis_attack_think;
	ent->use = invis_attack_use;//er - this is good to have
	if(ent->spawnflags & START_OFF)
	{
		ent->health = 0;
		ent->nextthink = 0;
	}
	else
	{
		ent->health = 1;	//used as a flag as to whether the thingy is active or not
		ent->nextthink = level.time + ent->delay;
	}
}

/*QUAKED environ_trainarm_spawn (0 1 0) (-8 -8 -8) (8 8 8)
*/

#define MASK_PROJ CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER|CONTENTS_SHOT

void debug_drawbox(edict_t* self,vec3_t vOrigin, vec3_t vMins, vec3_t vMaxs, int nColor);

void trainarm_think(edict_t *ent)
{	// you want hardcoded?  you can't handle the hardcoded!
	ent->nextthink = level.time + .1;

	if(ent->s.origin[0] < -4000)
	{
		G_FreeEdict(ent);
		return;
	}

	vec3_t searchPos;

	VectorCopy(ent->s.origin, searchPos);
	searchPos[1] += 128;
	searchPos[2] += 240;//this is getting more hardcoded by the minute.  Wow.

	vec3_t	tmin = {-512, -64, 0};
	vec3_t	tmax = {512, 64, 64};

	trace_t	tr;

	gi.trace(searchPos, tmin, tmax, searchPos, NULL, MASK_SHOT, &tr);//what mask should be used here?

	if(tr.ent != world)
	{
		//do obscene amounts of damage
		T_Damage (tr.ent, ent, ent, vec3_up, tr.ent->s.origin, tr.ent->s.origin, 999, 100, 0, MOD_CRUSH);
	}
}

void trainarm_spawn(edict_t *ent, edict_t *other, edict_t *activator)
{
	edict_t *newEnt;

	newEnt = G_Spawn();
	newEnt->movetype = MOVETYPE_PUSH;
	newEnt->solid = SOLID_BBOX;
	VectorCopy(ent->s.origin, newEnt->s.origin);
	newEnt->velocity[0] = -3500;
	VectorSet (newEnt->mins, -4, -2, -4);
	VectorSet (newEnt->maxs, 3, 2, 4);
	newEnt->health = 100;
	newEnt->think = trainarm_think;
	newEnt->nextthink = level.time + .1;

	newEnt->s.origin[2] += 1;

	game_ghoul.SetSimpleGhoulModel(newEnt, "objects/uganda/train_arm", "arm");

	IGhoulInst *inst;

	inst = newEnt->ghoulInst;

	Matrix4 orig, sc, rot, trans, temp;
	sc.Scale(1.5);
	rot.Rotate(0, 0, 3*M_PI/2);
	trans.Translate(0, 128, 176);
	temp.Concat(sc, rot);
	orig.Concat(temp, trans);
	inst->SetXForm(orig);
}

void SP_environ_trainarm_spawn(edict_t *ent)
{
	SetGenericEffectInfo(ent);

	ent->use = trainarm_spawn;
}

/*QUAKED environ_dustsource (0 1 0) (-8 -8 -8) (8 8 8) MAKEASBITS
"delay" frequency at which the dust should fall - set to -1 to make trigger only.  Default is 10
"health" size of the dust puffs to poop out... - default is 5
*/

void dustsource_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	vec3_t down = {0, 0, -1};
	trace_t tr;
	vec3_t testEnd;

	if(ent->spawnflags & 1)
	{
		VectorCopy(ent->s.origin, testEnd);
		testEnd[2] += 512;
		gi.trace(ent->s.origin, vec3_origin, vec3_origin, testEnd, ent, MASK_SOLID, &tr);

		FX_MakeWallDamage(tr, down, ent->health, WMRK_BULLETHOLE,true);
	}
	else
	{
		FX_DropDustBits(ent->s.origin, ent->health);
	}
}

void dustsource_think (edict_t *ent)
{
	ent->use(ent, 0, 0);
	ent->nextthink = level.time + gi.flrand(ent->delay * .8, ent->delay * 1.2);
}

void SP_environ_dustsource(edict_t *ent)
{
	SetGenericEffectInfo(ent);

	ent->think = dustsource_think;
	ent->delay = DEFAULT(ent->delay, 10);
	ent->health = DEFAULT(ent->health, 5);
	ent->use = dustsource_use;

	if(ent->health == -1)
	{
		ent->nextthink = 0;
	}
	else
	{
		ent->nextthink = level.time + gi.flrand(ent->delay * .8, ent->delay * 1.2);
	}
}

/*QUAKED environ_effect (0 1 0) (-8 -8 -8) (8 8 8)
"soundName" name of the effect to execute
look for more parameters here in the future
*/

void effect_use(edict_t *ent, edict_t *other, edict_t *activator)
{
	vec3_t dir;

	AngleVectors(ent->s.angles, dir, 0, 0);
	fxRunner.setDir(dir);
	fxRunner.exec(ent->soundName, ent->s.origin);
}

void SP_environ_effect (edict_t *ent)
{
	SetGenericEffectInfo(ent);

	gi.effectindex(ent->soundName);

	ent->use = effect_use;

	if(ent->soundName && strstr(ent->soundName, "gore") && lock_blood)
	{	// no gore effect for you
		G_FreeEdict(ent);
	}
}

/*QUAKED environ_effect_continual (0 1 0) (-8 -8 -8) (8 8 8) START_OFF
"soundName" name of the effect to execute
use to trigger this on and off
*/

void effect_continual_use(edict_t *ent, edict_t *other, edict_t *activator)
{
	if(fxRunner.hasEffect(ent->soundName, ent))
	{
		fxRunner.stopContinualEffect(ent->soundName, ent);
	}
	else
	{
		fxRunner.execContinualEffect(ent->soundName, ent);
	}
}

void SP_environ_effect_continual (edict_t *ent)
{
	SetGenericEffectInfo(ent);

	if(!(ent->spawnflags & 1))
	{
		fxRunner.execContinualEffect(ent->soundName, ent);
	}
	else
	{
		gi.effectindex(ent->soundName);
	}

	ent->use = effect_continual_use;
}

void chunk_spewer_think(edict_t *self)
{
	vec3_t	fwd;
	int		numchunks = self->count;
	int		scale = DEBRIS_SM;

	if (self->count == -1000)
	{
		G_FreeEdict(self);
		return;
	}

	AngleVectors(self->s.angles, fwd, NULL, NULL);
	if (numchunks < 0)
	{	// spew random number of chunks
		numchunks = gi.irand(3,7);
	}
	FX_ThrowDebris(self->s.origin, fwd, numchunks, scale, MAT_ROCK_FLESH, 0, 0, 0, SURF_BLOOD);

	if (self->count > 0)
	{	// wait a little bit then remove this entity
		self->count = -1000;
		self->nextthink = level.time + 3;
		return;
	}
	else
	{	//	wait -(self->count) seconds and then spew again
		self->nextthink = level.time + -(self->count) + gi.flrand(-.2, .2);
	}
}

void chunk_spewer_use(edict_t *self, edict_t *other, edict_t *activator)
{
	// don't let player use this
	if (other && other->client)
	{
		return;
	}

	chunk_spewer_think(self);
}

/*QUAKED environ_chunk_spewer (0 1 0) (-4 -4 -4) (4 4 4) TRIGGER_SPAWN
uses an info_notnull as a target, like a func_remote_camera.
----------------KEY----------------
count -- number of chunks to spew when this spewer is used. if negative, number of seconds between spewing chunks.
*/

void SP_environ_chunk_spewer(edict_t *ent)
{
	vec3_t Forward;

	ent->targetEnt = NULL;

	if(!ent->target)
	{
		gi.dprintf("Object 'environ_chunk_spewer' without a target.\n");
		
		G_FreeEdict(ent);
		
		return;
	}

	if(!ent->count)
	{
		gi.dprintf("Object 'environ_chunk_spewer' has zero chunks.\n");
		
		G_FreeEdict(ent);
		
		return;
	}

	ent->movetype = MOVETYPE_NONE;
	ent->solid=SOLID_NOT;
	VectorSet(ent->mins,-4,-4,-4);
	VectorSet(ent->maxs,4,4,4);

// ********************************************************************************************
// Find my target entity and then orient myself to look at it.
// ********************************************************************************************
	ent->targetEnt=G_Find(NULL,FOFS(targetname),ent->target);

	if (ent->targetEnt)
	{
		VectorSubtract(ent->targetEnt->s.origin,ent->s.origin,Forward);
		VectorNormalize(Forward);
		vectoangles(Forward,ent->s.angles);
	}

	if (ent->count > 0)
	{	// when this spewer is used, spew ent->count chunks and then go away
		//(we're kinda automatically TRIGGER_SPAWNED)
		ent->use = chunk_spewer_use;
		ent->nextthink = -1;
	}
	else
	{	// once this is used (if TRIGGER_SPAWNED) spew random number of chunks every -(ent->count) seconds
		if ( !(ent->spawnflags & CHUNKS_TRIGGER_SPAWN) )
		{	//	not trigger spawned so we don't need a use function
			ent->nextthink = level.time + FRAMETIME;
		}
		else
		{
			ent->use = chunk_spewer_use;
			ent->nextthink = -1;
		}
	}
	ent->think = chunk_spewer_think;
}

void SP_environ_soundgen (edict_t *ent);

spawn_t	environSpawns[] =
{
	{"environ_bloodrain",				SP_environ_bloodrain},
	{"environ_emergency_lights",		SP_environ_emergency_lights},
	{"environ_explosion",				SP_environ_explosion},
	{"environ_fire",					SP_environ_fire},
	{"environ_lightning",				SP_environ_lightning},
	{"environ_linefountain",			SP_environ_linefountain},
	{"environ_raingen",					SP_environ_raingen},
	{"environ_skyfade",					SP_environ_skyfade},
	{"environ_smokegen",				SP_environ_smokegen},
	{"environ_smoke_burst",				SP_environ_smoke_burst},
	{"environ_snow",					SP_environ_snow},
	{"environ_soundgen",				SP_environ_soundgen},
	{"environ_sparkgen",				SP_environ_sparkgen},
	{"environ_splashfountain",			SP_environ_splashfountain},
	{"environ_steamgen",				SP_environ_steamgen},
	{"environ_waterglobgen",			SP_environ_waterdripper},
	{"environ_rain_worldtrigger",		SP_environ_rain_worldtrigger},
	{"environ_cloudtrigger",			SP_environ_cloudtrigger},
	{"environ_linetrap",				SP_environ_linetrap},
	{"environ_inferno",					SP_environ_inferno},
	{"environ_invisible_attack",		SP_environ_invisible_attack},
	{"environ_trainarm_spawn",			SP_environ_trainarm_spawn},
	{"environ_dustsource",				SP_environ_dustsource},
	{"environ_effect",					SP_environ_effect},
	{"environ_effect_continual",		SP_environ_effect_continual},
	{"environ_player_snow",				SP_environ_player_snow},
	{"environ_chunk_spewer",			SP_environ_chunk_spewer},
	{NULL,								NULL}
};
