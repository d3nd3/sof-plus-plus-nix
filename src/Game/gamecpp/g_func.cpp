#include "g_local.h"
#include "ai_private.h"
#include "w_weapons.h"
#include "..\qcommon\ef_flags.h"

//  func_...
void SP_func_ambientSet (edict_t *ent);
void SP_func_areaportal (edict_t *ent);
void SP_func_breakable_brush (edict_t *ent);
void SP_func_trigger_brush (edict_t *ent);
void SP_func_button (edict_t *ent);
void SP_func_clock (edict_t *ent);
void SP_func_conveyor (edict_t *self);
void SP_func_door (edict_t *ent);
void SP_func_door_secret (edict_t *ent);
void SP_func_door_rotating (edict_t *ent);
void SP_func_door_rotating_smart (edict_t *ent);
void SP_func_killbox (edict_t *ent);
void SP_func_pushkillbox (edict_t *ent);
void SP_func_object (edict_t *self);
void SP_func_plat (edict_t *ent);
void SP_func_rotating (edict_t *ent);
void SP_func_timer (edict_t *self);
void SP_func_train (edict_t *ent);
void SP_func_wall (edict_t *self);
void SP_func_water (edict_t *ent);
void SP_info_null (edict_t *self); 
void SP_func_alarm(edict_t *self);
void SP_func_score (edict_t *ent);
void SP_func_camerashake(edict_t *self);
void SP_func_ignite(edict_t *self);
void SP_func_musicControl(edict_t *self);
void SP_func_fade(edict_t* self);
void SP_func_Dekker_diehere(edict_t* self);
void SP_func_Dekker_console(edict_t* self);
void SP_func_Dekker_jumphere(edict_t* self);
void SP_func_Dekker_split(edict_t* self);
void SP_func_door_portcullis (edict_t *ent);
void SP_func_hide_the_players_gun (edict_t *ent);
void SP_func_snipercam_zoom(edict_t *ent);
void SP_func_mission_status(edict_t *ent);
void SP_func_player_health(edict_t *ent);
void SP_func_player_armor(edict_t *ent);
void SP_func_player_item(edict_t *ent);

spawn_t	funcSpawns[] =
{
	{"func_ambientset",					SP_func_ambientSet},
	{"func_areaportal",					SP_func_areaportal},
	{"func_breakable_brush",			SP_func_breakable_brush},
	{"func_trigger_brush",				SP_func_trigger_brush},
	{"func_button",						SP_func_button},
	{"func_clock",						SP_func_clock},
	{"func_conveyor",					SP_func_conveyor},
	{"func_door",						SP_func_door},
	{"func_door_rotating",				SP_func_door_rotating},
	{"func_door_rotating_smart",		SP_func_door_rotating_smart},
	{"func_door_secret",				SP_func_door_secret},
	{"func_group",						SP_info_null},
	{"func_killbox",					SP_func_killbox},
	{"func_musicControl",				SP_func_musicControl},
	{"func_pushkillbox",				SP_func_pushkillbox},
	{"func_object",						SP_func_object},
	{"func_plat",						SP_func_plat},
	{"func_rotating",					SP_func_rotating},
	{"func_timer",						SP_func_timer},
	{"func_train",						SP_func_train},
	{"func_wall",						SP_func_wall},
	{"func_water",						SP_func_water},
	{"func_alarm",						SP_func_alarm},
	{"func_score",						SP_func_score},
	{"func_camerashake",				SP_func_camerashake},
	{"func_ignite",						SP_func_ignite},
	{"func_fade",						SP_func_fade},
	{"func_Dekker_diehere",				SP_func_Dekker_diehere},
	{"func_Dekker_console",				SP_func_Dekker_console},
	{"func_Dekker_jumphere",			SP_func_Dekker_jumphere},
	{"func_Dekker_split",				SP_func_Dekker_split},
	{"func_door_portcullis",			SP_func_door_portcullis},
	{"func_hide_the_players_gun",		SP_func_hide_the_players_gun},
	{"func_snipercam_zoom",				SP_func_snipercam_zoom},
	{"func_mission_status",				SP_func_mission_status},
	{"func_player_health",				SP_func_player_health},
	{"func_player_armor",				SP_func_player_armor},
	{"func_player_item",				SP_func_player_item},
	{NULL,								NULL},
};

#define CAMERA_ACTIVATOR_ONLY	1
#define CAMERA_GODMODE			2
#define CAMERA_NODELETE			4
#define CAMERA_LWANIMATE		8
#define CAMERA_CAMERA_MODEL		16


void door_use (edict_t *self, edict_t *other, edict_t *activator);
void portcullis_use (edict_t *self, edict_t *other, edict_t *activator);

/*
=========================================================

  PLATS

  movement options:

  linear
  smooth start, hard stop
  smooth start, smooth stop

  start
  end
  acceleration
  speed
  deceleration
  begin sound
  end sound
  target fired when reaching end
  wait at end

  object characteristics that use move segments
  ---------------------------------------------
  movetype_push, or movetype_stop
  action when touched
  action when blocked
  action when used
	disabled?
  auto trigger spawning


=========================================================
*/

#define PLAT_LOW_TRIGGER	1

#define	STATE_TOP			0
#define	STATE_BOTTOM		1
#define STATE_UP			2
#define STATE_DOWN			3

#define DOOR_START_OPEN		0x0001
#define DOOR_REVERSE		0x0002
#define DOOR_CRUSHER		0x0004
#define DOOR_NOMONSTER		0x0008
#define DOOR_LOCKED			0x0010
#define DOOR_TOGGLE			0x0020
#define DOOR_X_AXIS			0x0040
#define DOOR_Y_AXIS			0x0080
/*
#define x					0x0100		 
#define x					0x0200
#define x					0x0400
#define x					0x0800
#define x					0x1000
*/
#define DOOR_USE_TARGET		0x2000
#define DOOR_IGNOREHACK		0x4000 // rotating doors only
#define DOOR_DUALTARGET		0x8000

//
// Support routines for movement (changes in origin using velocity)
//

void Move_Done (edict_t *ent)
{
	VectorClear (ent->velocity);
	if (ent->moveinfo.endfunc)
	{
		ent->moveinfo.endfunc (ent);
	}
}

void Move_Final (edict_t *ent)
{
	if (ent->moveinfo.remaining_distance == 0)
	{
		Move_Done (ent);
		return;
	}

	VectorScale (ent->moveinfo.dir, ent->moveinfo.remaining_distance / FRAMETIME, ent->velocity);

	if (strcmp(ent->classname, "func_door_portcullis") == 0)
	{
		ent->moveinfo.thinkfunc = Move_Done;
	}
	else
	{
		ent->think = Move_Done;
	}
	ent->nextthink = level.time + FRAMETIME;
}

void Move_Begin (edict_t *ent)
{
	float	frames;

	if ((ent->moveinfo.speed * FRAMETIME) >= ent->moveinfo.remaining_distance)
	{
		Move_Final (ent);
		return;
	}
	VectorScale (ent->moveinfo.dir, ent->moveinfo.speed, ent->velocity);
	frames = floor((ent->moveinfo.remaining_distance / ent->moveinfo.speed) / FRAMETIME);
	ent->moveinfo.remaining_distance -= frames * ent->moveinfo.speed * FRAMETIME;
	if (strcmp(ent->classname, "func_door_portcullis") == 0)
	{	// need to indicate that, normally, this door wouldn't think 'til it's done moving
		ent->moveinfo.wait = level.time + (frames * FRAMETIME);
		ent->moveinfo.thinkfunc = Move_Done;
	}
	else
	{
		ent->nextthink = level.time + (frames * FRAMETIME);
		ent->think = Move_Final;
	}
}

void Think_AccelMove (edict_t *ent);

void Move_Calc (edict_t *ent, vec3_t dest, void(*func)(edict_t*))
{
	VectorClear (ent->velocity);
	VectorSubtract (dest, ent->s.origin, ent->moveinfo.dir);
	ent->moveinfo.remaining_distance = VectorNormalize (ent->moveinfo.dir);
	ent->moveinfo.endfunc = func;

	if (ent->moveinfo.speed == ent->moveinfo.accel && ent->moveinfo.speed == ent->moveinfo.decel)
	{
		if (level.current_entity == ((ent->flags & FL_TEAMSLAVE) ? ent->teammaster : ent))
		{
			Move_Begin (ent);
		}
		else
		{
			ent->nextthink = level.time + FRAMETIME;
			if (strcmp(ent->classname, "func_door_portcullis") == 0)
			{
				ent->moveinfo.thinkfunc = Move_Done;
			}
			else
			{
				ent->think = Move_Begin;
			}
		}
	}
	else
	{
		// accelerative
		ent->moveinfo.current_speed = 0;
		ent->think = Think_AccelMove;
		ent->nextthink = level.time + FRAMETIME;
	}
}


//
// Support routines for angular movement (changes in angle using avelocity)
//

void AngleMove_Done (edict_t *ent)
{
	VectorClear (ent->avelocity);
	if (ent->moveinfo.endfunc)
	{
		ent->moveinfo.endfunc (ent);
	}
}

void AngleMove_Final (edict_t *ent)
{
	vec3_t	move;

	if (ent->moveinfo.state == STATE_UP)
		VectorSubtract (ent->moveinfo.end_angles, ent->s.angles, move);
	else
		VectorSubtract (ent->moveinfo.start_angles, ent->s.angles, move);

	if (VectorCompare (move, vec3_origin))
	{
		AngleMove_Done (ent);
		return;
	}

	VectorScale (move, 1.0/FRAMETIME, ent->avelocity);

	ent->think = AngleMove_Done;
	ent->nextthink = level.time + FRAMETIME;
}

void AngleMove_Begin (edict_t *ent)
{
	vec3_t	destdelta;
	float	len;
	float	traveltime;
	float	frames;

	// set destdelta to the vector needed to move
	if (ent->moveinfo.state == STATE_UP)
		VectorSubtract (ent->moveinfo.end_angles, ent->s.angles, destdelta);
	else
		VectorSubtract (ent->moveinfo.start_angles, ent->s.angles, destdelta);
	
	// calculate length of vector
	len = VectorLength (destdelta);
	
	// divide by speed to get time to reach dest
	traveltime = len / ent->moveinfo.speed;

	if (traveltime < FRAMETIME)
	{
		AngleMove_Final (ent);
		return;
	}

	frames = floor(traveltime / FRAMETIME);

	// scale the destdelta vector by the time spent traveling to get velocity
	VectorScale (destdelta, 1.0 / traveltime, ent->avelocity);

	// set nextthink to trigger a think when dest is reached
	ent->nextthink = level.time + frames * FRAMETIME;
	ent->think = AngleMove_Final;
}

void AngleMove_Calc (edict_t *ent, void(*func)(edict_t*))
{
	VectorClear (ent->avelocity);
	ent->moveinfo.endfunc = func;
	if (level.current_entity == ((ent->flags & FL_TEAMSLAVE) ? ent->teammaster : ent))
	{
		AngleMove_Begin (ent);
	}
	else
	{
		ent->nextthink = level.time + FRAMETIME;
		ent->think = AngleMove_Begin;
	}
}


/*
==============
Think_AccelMove

The team has completed a frame of movement, so
change the speed for the next frame
==============
*/
#define AccelerationDistance(target, rate)	(target * ((target / rate) + 1) / 2)

void plat_CalcAcceleratedMove(moveinfo_t *moveinfo)
{
	float	accel_dist;
	float	decel_dist;

	moveinfo->move_speed = moveinfo->speed;

	if (moveinfo->remaining_distance < moveinfo->accel)
	{
		moveinfo->current_speed = moveinfo->remaining_distance;
		return;
	}

	accel_dist = AccelerationDistance (moveinfo->speed, moveinfo->accel);
	decel_dist = AccelerationDistance (moveinfo->speed, moveinfo->decel);

	if ((moveinfo->remaining_distance - accel_dist - decel_dist) < 0)
	{
		float	f;

		f = (moveinfo->accel + moveinfo->decel) / (moveinfo->accel * moveinfo->decel);
		moveinfo->move_speed = (-2 + sqrt(4 - 4 * f * (-2 * moveinfo->remaining_distance))) / (2 * f);
		decel_dist = AccelerationDistance (moveinfo->move_speed, moveinfo->decel);
	}

	moveinfo->decel_distance = decel_dist;
};

void plat_Accelerate (moveinfo_t *moveinfo)
{
	// are we decelerating?
	if (moveinfo->remaining_distance <= moveinfo->decel_distance)
	{
		if (moveinfo->remaining_distance < moveinfo->decel_distance)
		{
			if (moveinfo->next_speed)
			{
				moveinfo->current_speed = moveinfo->next_speed;
				moveinfo->next_speed = 0;
				return;
			}
			if (moveinfo->current_speed > moveinfo->decel)
				moveinfo->current_speed -= moveinfo->decel;
		}
		return;
	}

	// are we at full speed and need to start decelerating during this move?
	if (moveinfo->current_speed == moveinfo->move_speed)
		if ((moveinfo->remaining_distance - moveinfo->current_speed) < moveinfo->decel_distance)
		{
			float	p1_distance;
			float	p2_distance;
			float	distance;

			p1_distance = moveinfo->remaining_distance - moveinfo->decel_distance;
			p2_distance = moveinfo->move_speed * (1.0 - (p1_distance / moveinfo->move_speed));
			distance = p1_distance + p2_distance;
			moveinfo->current_speed = moveinfo->move_speed;
			moveinfo->next_speed = moveinfo->move_speed - moveinfo->decel * (p2_distance / distance);
			return;
		}

	// are we accelerating?
	if (moveinfo->current_speed < moveinfo->speed)
	{
		float	old_speed;
		float	p1_distance;
		float	p1_speed;
		float	p2_distance;
		float	distance;

		old_speed = moveinfo->current_speed;

		// figure simple acceleration up to move_speed
		moveinfo->current_speed += moveinfo->accel;
		if (moveinfo->current_speed > moveinfo->speed)
			moveinfo->current_speed = moveinfo->speed;

		// are we accelerating throughout this entire move?
		if ((moveinfo->remaining_distance - moveinfo->current_speed) >= moveinfo->decel_distance)
			return;

		// during this move we will accelrate from current_speed to move_speed
		// and cross over the decel_distance; figure the average speed for the
		// entire move
		p1_distance = moveinfo->remaining_distance - moveinfo->decel_distance;
		p1_speed = (old_speed + moveinfo->move_speed) / 2.0;
		p2_distance = moveinfo->move_speed * (1.0 - (p1_distance / p1_speed));
		distance = p1_distance + p2_distance;
		moveinfo->current_speed = (p1_speed * (p1_distance / distance)) + (moveinfo->move_speed * (p2_distance / distance));
		moveinfo->next_speed = moveinfo->move_speed - moveinfo->decel * (p2_distance / distance);
		return;
	}

	// we are at constant velocity (move_speed)
	return;
};

void Think_AccelMove (edict_t *ent)
{
	ent->moveinfo.remaining_distance -= ent->moveinfo.current_speed;

	if (ent->moveinfo.current_speed == 0)		// starting or blocked
		plat_CalcAcceleratedMove(&ent->moveinfo);

	plat_Accelerate (&ent->moveinfo);

	// will the entire move complete on next frame?
	if (ent->moveinfo.remaining_distance <= ent->moveinfo.current_speed)
	{
		Move_Final (ent);
		return;
	}

	VectorScale (ent->moveinfo.dir, ent->moveinfo.current_speed*10, ent->velocity);
	ent->nextthink = level.time + FRAMETIME;
	ent->think = Think_AccelMove;
}


void plat_go_down (edict_t *ent);

void plat_hit_top (edict_t *ent)
{
	if (!(ent->flags & FL_TEAMSLAVE))
	{

		if (ent->moveinfo.sound_end != -1)
			gi.sound (ent, CHAN_NO_PHS_ADD+CHAN_VOICE, ent->moveinfo.sound_end, 1,ATTN_NORM, 0);
		ent->s.sound = 0;
	}
	ent->moveinfo.state = STATE_TOP;

	ent->think = plat_go_down;
	ent->nextthink = level.time + 3;
}

void plat_hit_bottom (edict_t *ent)
{
	if (!(ent->flags & FL_TEAMSLAVE))
	{
		if (ent->moveinfo.sound_end != -1)
			gi.sound (ent, CHAN_NO_PHS_ADD+CHAN_VOICE, ent->moveinfo.sound_end, 1, ATTN_NORM, 0);
		ent->s.sound = 0;
	}
	ent->moveinfo.state = STATE_BOTTOM;
}

void plat_go_down (edict_t *ent)
{
	if (!(ent->flags & FL_TEAMSLAVE))
	{
		if (ent->moveinfo.sound_start != -1)
			gi.sound (ent, CHAN_NO_PHS_ADD+CHAN_VOICE, ent->moveinfo.sound_start, 1, ATTN_NORM, 0);
		if(ent->moveinfo.sound_middle != -1)
		{
			ent->s.sound = ent->moveinfo.sound_middle;
		 	ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		}
	}
	ent->moveinfo.state = STATE_DOWN;
	Move_Calc (ent, ent->moveinfo.end_origin, plat_hit_bottom);
}

void plat_go_up (edict_t *ent)
{
	if (!(ent->flags & FL_TEAMSLAVE))
	{
		if (ent->moveinfo.sound_start != -1)
			gi.sound (ent, CHAN_NO_PHS_ADD+CHAN_VOICE, ent->moveinfo.sound_start, 1, ATTN_NORM, 0);
		if(ent->moveinfo.sound_middle != -1)
		{
			ent->s.sound = ent->moveinfo.sound_middle;
			ent->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		}
	}
	ent->moveinfo.state = STATE_UP;
	Move_Calc (ent, ent->moveinfo.start_origin, plat_hit_top);
}

void plat_blocked (edict_t *self, edict_t *other)
{
	if (!(other->svflags & SVF_MONSTER) && (!other->client) )
	{
		// give it a chance to go away on it's own terms (like gibs)
		T_Damage (other, self, self, vec3_origin, other->s.origin, other->s.origin, 100000, 1, 0, MOD_CRUSH);
		// if it's still there, nuke it
		if (other)
			BecomeExplosion1 (other);
		return;
	}

	T_Damage (other, self, self, vec3_origin, other->s.origin, other->s.origin, self->dmg, 1, 0, MOD_CRUSH);

	if (self->moveinfo.state == STATE_UP)
		plat_go_down (self);
	else if (self->moveinfo.state == STATE_DOWN)
		plat_go_up (self);
}


void Use_Plat (edict_t *ent, edict_t *other, edict_t *activator)
{ 
	if (ent->think)
		return;		// already down
	plat_go_down (ent);
}


void Touch_Plat_Center (edict_t *ent, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if (!other->client)
		return;
		
	if (other->health <= 0)
		return;

	ent = ent->enemy;	// now point at the plat, not the trigger
	if (ent->moveinfo.state == STATE_BOTTOM)
		plat_go_up (ent);
	else if (ent->moveinfo.state == STATE_TOP)
		ent->nextthink = level.time + 1;	// the player is still on the plat, so delay going down
}

void plat_spawn_inside_trigger (edict_t *ent)
{
	edict_t	*trigger;
	vec3_t	tmin, tmax;

//
// middle trigger
//	
	trigger = G_Spawn();
	trigger->touch = Touch_Plat_Center;
	trigger->movetype = MOVETYPE_NONE;
	trigger->solid = SOLID_TRIGGER;
	trigger->enemy = ent;
	
	tmin[0] = ent->mins[0] + 25;
	tmin[1] = ent->mins[1] + 25;
	tmin[2] = ent->mins[2];

	tmax[0] = ent->maxs[0] - 25;
	tmax[1] = ent->maxs[1] - 25;
	tmax[2] = ent->maxs[2] + 8;

	tmin[2] = tmax[2] - (ent->pos1[2] - ent->pos2[2] + st.lip);

	if (ent->spawnflags & PLAT_LOW_TRIGGER)
		tmax[2] = tmin[2] + 8;
	
	if (tmax[0] - tmin[0] <= 0)
	{
		tmin[0] = (ent->mins[0] + ent->maxs[0]) *0.5;
		tmax[0] = tmin[0] + 1;
	}
	if (tmax[1] - tmin[1] <= 0)
	{
		tmin[1] = (ent->mins[1] + ent->maxs[1]) *0.5;
		tmax[1] = tmin[1] + 1;
	}
	
	VectorCopy (tmin, trigger->mins);
	VectorCopy (tmax, trigger->maxs);

	gi.linkentity (trigger);
}

void platInit(edict_t *ent)
{
	ent->moveinfo.sound_start = SND_getBModelSoundIndex(ent->soundName, 0);
	ent->moveinfo.sound_middle = SND_getBModelSoundIndex(ent->soundName, 1);
	ent->moveinfo.sound_end = SND_getBModelSoundIndex(ent->soundName, 2);

	ent->think = NULL;
}

/*QUAKED func_plat (0 .5 .8) ? PLAT_LOW_TRIGGER
speed	default 150

Plats are always drawn in the extended position, so they will light correctly.

If the plat is the target of another trigger or button, it will start out disabled in the extended position until it is trigger, when it will lower and become a normal plat.

"speed"	overrides default 200.
"accel" overrides default 500
"lip"	overrides default 8 pixel lip

If the "height" key is set, that will determine the amount the plat moves, instead of being implicitly determoveinfoned by the model's height.

SoundName - Name of the sound type of the plat

Set "sounds" to one of the following:
1) base fast
2) chain slow
*/
void SP_func_plat (edict_t *ent)
{
	VectorClear (ent->s.angles);
	ent->solid = SOLID_BSP;
	ent->movetype = MOVETYPE_PUSH;

	gi.setmodel (ent, ent->model);

	ent->blocked = plat_blocked;

	if (!ent->speed)
		ent->speed = 20;
	else
		ent->speed *= 0.1;

	if (!ent->accel)
		ent->accel = 5;
	else
		ent->accel *= 0.1;

	if (!ent->decel)
		ent->decel = 5;
	else
		ent->decel *= 0.1;

	if (!ent->dmg)
		ent->dmg = 2;

	if (!st.lip)
		st.lip = 8;

	// pos1 is the top position, pos2 is the bottom
	VectorCopy (ent->s.origin, ent->pos1);
	VectorCopy (ent->s.origin, ent->pos2);
	if (st.height)
		ent->pos2[2] -= st.height;
	else
		ent->pos2[2] -= (ent->maxs[2] - ent->mins[2]) - st.lip;

	ent->use = Use_Plat;

	plat_spawn_inside_trigger (ent);	// the "start moving" trigger	

	if (ent->targetname)
	{
		ent->moveinfo.state = STATE_UP;
	}
	else
	{
		VectorCopy (ent->pos2, ent->s.origin);
		gi.linkentity (ent);
		ent->moveinfo.state = STATE_BOTTOM;
	}

	ent->moveinfo.speed = ent->speed;
	ent->moveinfo.accel = ent->accel;
	ent->moveinfo.decel = ent->decel;
	ent->moveinfo.wait = ent->wait;
	VectorCopy (ent->pos1, ent->moveinfo.start_origin);
	VectorCopy (ent->s.angles, ent->moveinfo.start_angles);
	VectorCopy (ent->pos2, ent->moveinfo.end_origin);
	VectorCopy (ent->s.angles, ent->moveinfo.end_angles);

	//this will probably break at first, I believe

	ent->think = platInit;
	ent->nextthink = level.time + .1;
}

//====================================================================

/*QUAKED func_rotating (0 .5 .8) ? START_ON REVERSE X_AXIS Y_AXIS TOUCH_PAIN STOP ANIMATED ANIMATED_FAST
You need to have an origin brush as part of this entity.  The center of that brush will be
the point around which it is rotated. It will rotate around the Z axis by default.  You can
check either the X_AXIS or Y_AXIS box to change that.

"speed" determines how fast it moves; default value is 100.
"dmg"	damage to inflict when blocked (2 default)
"soundname" this will determine the sound that is played while the object rotates

REVERSE will cause the it to rotate in the opposite direction.
STOP mean it will stop moving instead of pushing entities
*/

void rotating_blocked (edict_t *self, edict_t *other)
{
	T_Damage (other, self, self, vec3_origin, other->s.origin, other->s.origin, self->dmg, 1, 0, MOD_CRUSH);
}

void rotating_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if (self->avelocity[0] || self->avelocity[1] || self->avelocity[2])
		T_Damage (other, self, self, vec3_origin, other->s.origin, other->s.origin, self->dmg, 1, 0, MOD_CRUSH);
}

void rotating_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (!VectorCompare (self->avelocity, vec3_origin))
	{
		self->s.sound = 0;
		VectorClear (self->avelocity);
		self->touch = NULL;
	}
	else
	{
		if(self->moveinfo.sound_middle != -1)
		{
			self->s.sound = self->moveinfo.sound_middle;
			self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		}
		VectorScale (self->movedir, self->speed, self->avelocity);
		if (self->spawnflags & 16)
			self->touch = rotating_touch;
	}
}

void rotateInit(edict_t *ent)
{
	ent->moveinfo.sound_middle = SND_getBModelSoundIndex(ent->soundName, 0);
	ent->think = NULL;
}

void SP_func_rotating (edict_t *ent)
{
	ent->solid = SOLID_BSP;
	if (ent->spawnflags & 32)
		ent->movetype = MOVETYPE_STOP;
	else
		ent->movetype = MOVETYPE_PUSH;

	// set the axis of rotation
	VectorClear(ent->movedir);
	if (ent->spawnflags & 4)
		ent->movedir[2] = 1.0;
	else if (ent->spawnflags & 8)
		ent->movedir[0] = 1.0;
	else // Z_AXIS
		ent->movedir[1] = 1.0;

	// check for reverse rotation
	if (ent->spawnflags & 2)
		VectorNegate (ent->movedir, ent->movedir);

	if (!ent->speed)
		ent->speed = 100;
	if (!ent->dmg)
		ent->dmg = 2;

	ent->use = rotating_use;
	if (ent->dmg)
		ent->blocked = rotating_blocked;

	if (ent->spawnflags & 1)
		ent->use (ent, NULL, NULL);

	if (ent->spawnflags & 64)
		ent->s.effects |= EF_ANIM_ALL;
	if (ent->spawnflags & 128)
		ent->s.effects |= EF_ANIM_ALLFAST;

	gi.setmodel (ent, ent->model);
	gi.linkentity (ent);

	ent->think = rotateInit;
	ent->nextthink = level.time + .1;
}

/*
======================================================================

BUTTONS

======================================================================
*/

/*QUAKED func_button (0 .5 .8) ? AUTOUSE NOTQUAD
When a button is touched, it moves some distance in the direction of it's angle, triggers all of it's targets, waits some time, then returns to it's original position where it can be triggered again.
Buttons cannot be used by player if they have a targetname.

AUTOUSE - Not implemented
QUAD - Use frames 0-1 when up, 2-3 when down

"angle"		determines the opening direction
"target"	all entities with a matching targetname will be used
"speed"		override the default 40 speed
"wait"		override the default 1 second wait (-1 = never return)
"lip"		override the default 4 pixel lip remaining at end of move
"health"	if set, the button must be killed instead of touched
"soundname"	sounds to be used for this button...
"sp_message" is for a string package ID (numerical)
*/

#define NOTQUAD	2

void button_done (edict_t *self)
{
	self->moveinfo.state = STATE_BOTTOM;
	self->s.effects &= ~EF_ANIM23;
	self->s.effects |= EF_ANIM01;
}

void button_return (edict_t *self)
{
	self->moveinfo.state = STATE_DOWN;

	Move_Calc (self, self->moveinfo.start_origin, button_done);

	self->s.frame = 0;

	if (self->health)
		self->takedamage = DAMAGE_YES;
}

void button_wait (edict_t *self)
{
	self->moveinfo.state = STATE_TOP;
	self->s.effects &= ~EF_ANIM01;
	self->s.effects |= EF_ANIM23;

	G_UseTargets (self, self->activator);
	self->s.frame = 1;
	if (self->moveinfo.wait >= 0)
	{
		self->nextthink = level.time + self->moveinfo.wait;
		self->think = button_return;
	}
}

bool button_fire (edict_t *self)
{
	if (self->moveinfo.state == STATE_UP || self->moveinfo.state == STATE_TOP)
		return false;

	self->moveinfo.state = STATE_UP;
	if ((self->moveinfo.sound_start != -1) && !(self->flags & FL_TEAMSLAVE))
		gi.sound (self, CHAN_NO_PHS_ADD+CHAN_VOICE, self->moveinfo.sound_start, 1, ATTN_NORM, 0);
	Move_Calc (self, self->moveinfo.end_origin, button_wait);
	return true;
}

void button_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (!self->soundName)
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("Ambient/Buttons/HiTech/Elevator1.wav"), .6, ATTN_NORM, 0);
	}
 	self->activator = activator;
	if (button_fire (self))
	{
		// want to be able to assign an sp_message to a button
		if (self->sp_message)
		{
			gi.SP_Print(other, self->sp_message);
			gi.sound (other, CHAN_AUTO, gi.soundindex ("Misc/Talk.wav"), 1, ATTN_NORM, 0);//FIXME
		}
	}
	self->last_move_time = level.time;
}

void button_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if (!other->client)
		return;

	if (other->health <= 0)
		return;

	self->activator = other;
	button_fire (self);
}

void button_killed (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	self->activator = attacker;
	self->health = self->max_health;
	self->takedamage = DAMAGE_NO;
	button_fire (self);
}

void buttonInit(edict_t *ent)
{
	ent->moveinfo.sound_start = SND_getBModelSoundIndex(ent->soundName, 0);
	ent->think = NULL;
}

void SP_func_button (edict_t *ent)
{
	vec3_t	abs_movedir;
	float	dist;

	gi.soundindex("Ambient/Buttons/HiTech/Elevator1.wav");

	G_SetMovedir (ent->s.angles, ent->movedir);
	ent->movetype = MOVETYPE_STOP;
	ent->solid = SOLID_BSP;
	gi.setmodel (ent, ent->model);

	if (!ent->speed)
		ent->speed = 40;
	if (!ent->accel)
		ent->accel = ent->speed;
	if (!ent->decel)
		ent->decel = ent->speed;

	if (!ent->wait)
		ent->wait = 3;
	if (!st.lip)
		st.lip = 4;

	VectorCopy (ent->s.origin, ent->pos1);
	abs_movedir[0] = fabs(ent->movedir[0]);
	abs_movedir[1] = fabs(ent->movedir[1]);
	abs_movedir[2] = fabs(ent->movedir[2]);
	dist = abs_movedir[0] * ent->size[0] + abs_movedir[1] * ent->size[1] + abs_movedir[2] * ent->size[2] - st.lip;
	VectorMA (ent->pos1, dist, ent->movedir, ent->pos2);

	ent->use = button_use;

	if (!ent->targetname)
	{
		ent->plUse = button_use;
		ent->last_move_time = level.time - 10.0;//safe for monsters to use me...
	}

	ent->s.effects |= EF_ANIM01;

	if (ent->spawnflags & NOTQUAD)
	{
		ent->s.effects |= EF_NOTQUAD;
	}

	if (ent->health)
	{
		ent->max_health = ent->health;
		ent->die = button_killed;
		ent->takedamage = DAMAGE_YES;
	}
	else if (! ent->targetname && (ent->spawnflags & 1))
		ent->touch = button_touch;

	ent->moveinfo.state = STATE_BOTTOM;

	ent->moveinfo.speed = ent->speed;
	ent->moveinfo.accel = ent->accel;
	ent->moveinfo.decel = ent->decel;
	ent->moveinfo.wait = ent->wait;
	VectorCopy (ent->pos1, ent->moveinfo.start_origin);
	VectorCopy (ent->s.angles, ent->moveinfo.start_angles);
	VectorCopy (ent->pos2, ent->moveinfo.end_origin);
	VectorCopy (ent->s.angles, ent->moveinfo.end_angles);

	gi.linkentity (ent);

	ent->think = buttonInit;
	ent->nextthink = level.time + .1;

}

/*QUAKED func_water (0 .5 .8) ? START_OPEN
func_water is a moveable water brush.  It must be targeted to operate.  Use a non-water texture at your own risk.

START_OPEN causes the water to move to its destination when spawned and operate in reverse.

"angle"		determines the opening direction (up or down only)
"speed"		movement speed (25 default)
"wait"		wait before returning (-1 default, -1 = TOGGLE)
"lip"		lip remaining at end of move (0 default)
"soundName"	(yes, these need to be changed) - this has got to be fixed later...
0)	no sound
1)	water
2)	lava
*/

void SP_func_water (edict_t *self)
{
	vec3_t	abs_movedir;

	G_SetMovedir (self->s.angles, self->movedir);
	self->movetype = MOVETYPE_PUSH;
	self->solid = SOLID_BSP;
	gi.setmodel (self, self->model);

	switch (self->sounds)
	{
		default:
			break;

		case 1: // water
			self->moveinfo.sound_start = gi.soundindex  ("world/mov_watr.wav");
			self->moveinfo.sound_end = gi.soundindex  ("world/stp_watr.wav");
			break;

		case 2: // lava
			self->moveinfo.sound_start = gi.soundindex  ("world/mov_watr.wav");
			self->moveinfo.sound_end = gi.soundindex  ("world/stp_watr.wav");
			break;
	}

	// calculate second position
	VectorCopy (self->s.origin, self->pos1);
	abs_movedir[0] = fabs(self->movedir[0]);
	abs_movedir[1] = fabs(self->movedir[1]);
	abs_movedir[2] = fabs(self->movedir[2]);
	self->moveinfo.distance = abs_movedir[0] * self->size[0] + abs_movedir[1] * self->size[1] + abs_movedir[2] * self->size[2] - st.lip;
	VectorMA (self->pos1, self->moveinfo.distance, self->movedir, self->pos2);

	// if it starts open, switch the positions
	if (self->spawnflags & DOOR_START_OPEN)
	{
		VectorCopy (self->pos2, self->s.origin);
		VectorCopy (self->pos1, self->pos2);
		VectorCopy (self->s.origin, self->pos1);
	}

	VectorCopy (self->pos1, self->moveinfo.start_origin);
	VectorCopy (self->s.angles, self->moveinfo.start_angles);
	VectorCopy (self->pos2, self->moveinfo.end_origin);
	VectorCopy (self->s.angles, self->moveinfo.end_angles);

	self->moveinfo.state = STATE_BOTTOM;

	if (!self->speed)
		self->speed = 25;
	self->moveinfo.accel = self->moveinfo.decel = self->moveinfo.speed = self->speed;

	if (!self->wait)
		self->wait = -1;
	self->moveinfo.wait = self->wait;

	self->use = door_use;

	if (self->wait == -1)
		self->spawnflags |= DOOR_TOGGLE;

	self->classname = "func_door";

	gi.linkentity (self);
}


#define TRAIN_START_ON		1
#define TRAIN_TOGGLE		2
#define TRAIN_BLOCK_STOPS	4

/*QUAKED func_train (0 .5 .8) ? START_ON TOGGLE BLOCK_STOPS
Trains are moving platforms that players can ride.
The targets origin specifies the min point of the train at each corner.
The train spawns at the first target it is pointing at.
If the train is the target of a button or trigger, it will not begin moving until activated.
speed	default 100
dmg		default	2
"soundname" sound for the train

*/
void train_next (edict_t *self);

void train_blocked (edict_t *self, edict_t *other)
{
	if (!(other->svflags & SVF_MONSTER) && (!other->client) )
	{
		// give it a chance to go away on it's own terms (like gibs)
		T_Damage (other, self, self, vec3_origin, other->s.origin, other->s.origin, 100000, 1, DAMAGE_NO_KNOCKBACK|DT_MANGLE, MOD_CRUSH);
		// if it's still there, nuke it
		if (other)
			BecomeExplosion1 (other);
		return;
	}

	if (level.time < self->touch_debounce_time)
		return;

	if (!self->dmg)
		return;
	self->touch_debounce_time = level.time + 0.5;
	T_Damage (other, self, self, vec3_origin, other->s.origin, other->s.origin, self->dmg, 1, DAMAGE_NO_KNOCKBACK|DT_MANGLE, MOD_CRUSH);
}

void train_wait (edict_t *self)
{
	if (self->target_ent->pathtarget)
	{
		char	*savetarget;
		edict_t	*ent;

		ent = self->target_ent;
		savetarget = ent->target;
		ent->target = ent->pathtarget;
		G_UseTargets (ent, self->activator);
		ent->target = savetarget;

		// make sure we didn't get killed by a killtarget
		if (!self->inuse)
			return;
	}

	if (self->moveinfo.wait)
	{
		if (self->moveinfo.wait > 0)
		{
			self->nextthink = level.time + self->moveinfo.wait;
			self->think = train_next;
		}
		else if (self->spawnflags & TRAIN_TOGGLE)  // && wait < 0
		{
			train_next (self);
			self->spawnflags &= ~TRAIN_START_ON;
			VectorClear (self->velocity);
			self->nextthink = 0;
		}

		if (!(self->flags & FL_TEAMSLAVE))
		{
			if (self->moveinfo.sound_end != -1)
				gi.sound (self, CHAN_NO_PHS_ADD+CHAN_VOICE, self->moveinfo.sound_end, 1, ATTN_NORM, 0);
			self->s.sound = 0;
		}
	}
	else
	{
		train_next (self);
	}
	
}

void train_next (edict_t *self)
{
	edict_t		*ent;
	vec3_t		dest;
	qboolean	first;

	first = true;
again:
	if (!self->target)
	{
//		gi.dprintf ("train_next: no next target\n");
		return;
	}

	ent = G_PickTarget (self->target);
	if (!ent)
	{
		gi.dprintf ("train_next: bad target %s\n", self->target);
		return;
	}

	self->target = ent->target;

	// check for a teleport path_corner
	if (ent->spawnflags & 1)
	{
		if (!first)
		{
			gi.dprintf ("connected teleport path_corners, see %s at %s\n", ent->classname, vtos(ent->s.origin));
			return;
		}
		first = false;
		VectorSubtract (ent->s.origin, self->mins, self->s.origin);
		gi.linkentity (self);
		goto again;
	}

	self->moveinfo.wait = ent->wait;
	self->target_ent = ent;

	if (!(self->flags & FL_TEAMSLAVE))
	{
		if (self->moveinfo.sound_start != -1)
			gi.sound (self, CHAN_NO_PHS_ADD+CHAN_VOICE, self->moveinfo.sound_start, 1, ATTN_NORM, 0);
		if (self->moveinfo.sound_middle != -1)
		{
			self->s.sound = self->moveinfo.sound_middle;
			self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		}
	}

	VectorSubtract (ent->s.origin, self->mins, dest);
	self->moveinfo.state = STATE_TOP;
	VectorCopy (self->s.origin, self->moveinfo.start_origin);
	VectorCopy (dest, self->moveinfo.end_origin);
	Move_Calc (self, dest, train_wait);
	self->spawnflags |= TRAIN_START_ON;
}

void train_resume (edict_t *self)
{
	edict_t	*ent;
	vec3_t	dest;

	ent = self->target_ent;

	VectorSubtract (ent->s.origin, self->mins, dest);
	self->moveinfo.state = STATE_TOP;
	VectorCopy (self->s.origin, self->moveinfo.start_origin);
	VectorCopy (dest, self->moveinfo.end_origin);
	Move_Calc (self, dest, train_wait);
	self->spawnflags |= TRAIN_START_ON;
}

void func_train_find (edict_t *self)
{
	edict_t *ent;

	self->moveinfo.sound_start = SND_getBModelSoundIndex(self->soundName, 0);
	self->moveinfo.sound_middle = SND_getBModelSoundIndex(self->soundName, 1);
	self->moveinfo.sound_end = SND_getBModelSoundIndex(self->soundName, 2);

	if (!self->target)
	{
		gi.dprintf ("train_find: no target\n");
		return;
	}
	ent = G_PickTarget (self->target);
	if (!ent)
	{
		gi.dprintf ("train_find: target %s not found\n", self->target);
		return;
	}
	self->target = ent->target;

	VectorSubtract (ent->s.origin, self->mins, self->s.origin);
	gi.linkentity (self);

	// if not triggered, start immediately
	if (!self->targetname)
		self->spawnflags |= TRAIN_START_ON;

	if (self->spawnflags & TRAIN_START_ON)
	{
		self->nextthink = level.time + FRAMETIME;
		self->think = train_next;
		self->activator = self;
	}
}

void train_use (edict_t *self, edict_t *other, edict_t *activator)
{
	self->activator = activator;

	if (self->spawnflags & TRAIN_START_ON)
	{
		if (!(self->spawnflags & TRAIN_TOGGLE))
			return;
		self->spawnflags &= ~TRAIN_START_ON;
		VectorClear (self->velocity);
		self->nextthink = 0;
	}
	else
	{
		if (self->target_ent)
			train_resume(self);
		else
			train_next(self);
	}
}

void SP_func_train (edict_t *self)
{
	self->movetype = MOVETYPE_PUSH;

	VectorClear (self->s.angles);
	self->blocked = train_blocked;
	if (self->spawnflags & TRAIN_BLOCK_STOPS)
		self->dmg = 0;
	else
	{
		if (!self->dmg)
			self->dmg = 100;
	}
	self->solid = SOLID_BSP;
	gi.setmodel (self, self->model);

	if (!self->speed)
		self->speed = 100;

	self->moveinfo.speed = self->speed;
	self->moveinfo.accel = self->moveinfo.decel = self->moveinfo.speed;

	self->use = train_use;
	self->gravity = 0;
	self->friction = 0;
	self->airresistance = 0;
	gi.linkentity (self);

	if (self->target)
	{
		// start trains on the second frame, to make sure their targets have had
		// a chance to spawn
		self->nextthink = level.time + FRAMETIME;
		self->think = func_train_find;
	}
	else
	{
		gi.dprintf ("func_train without a target at %s\n", vtos(self->absmin));
	}
}


/*QUAKED trigger_elevator (0.3 0.1 0.6) (-8 -8 -8) (8 8 8)
*/
void trigger_elevator_use (edict_t *self, edict_t *other, edict_t *activator)
{
	edict_t *target;

	if (self->movetarget->nextthink)
	{
//		gi.dprintf("elevator busy\n");
		return;
	}

	if (!other->pathtarget)
	{
		gi.dprintf("elevator used with no pathtarget\n");
		return;
	}

	target = G_PickTarget (other->pathtarget);
	if (!target)
	{
		gi.dprintf("elevator used with bad pathtarget: %s\n", other->pathtarget);
		return;
	}

	self->movetarget->target_ent = target;
	train_resume (self->movetarget);
}

void trigger_elevator_init (edict_t *self)
{
	if (!self->target)
	{
		gi.dprintf("trigger_elevator has no target\n");
		return;
	}
	self->movetarget = G_PickTarget (self->target);
	if (!self->movetarget)
	{
		gi.dprintf("trigger_elevator unable to find target %s\n", self->target);
		return;
	}
	if (strcmp(self->movetarget->classname, "func_train") != 0)
	{
		gi.dprintf("trigger_elevator target %s is not a train\n", self->target);
		return;
	}

	self->use = trigger_elevator_use;
	self->svflags = SVF_NOCLIENT;

}

void SP_trigger_elevator (edict_t *self)
{
	self->think = trigger_elevator_init;
	self->nextthink = level.time + FRAMETIME;
}


/*QUAKED func_timer (0.3 0.1 0.6) (-8 -8 -8) (8 8 8) START_ON
"wait"			base time between triggering all targets, default is 1
"random"		wait variance, default is 0

so, the basic time between firing is a random time between
(wait - random) and (wait + random)

"delay"			delay before first firing when turned on, default is 0

"pausetime"		additional delay used only the very first time
				and only if spawned with START_ON

These can used but not touched.
*/
void func_timer_think (edict_t *self)
{
	G_UseTargets (self, self->activator);
	self->nextthink = level.time + self->wait + gi.flrand(-self->random, self->random);
}

void func_timer_use (edict_t *self, edict_t *other, edict_t *activator)
{
	self->activator = activator;

	// if on, turn it off
	if (self->nextthink)
	{
		self->nextthink = 0;
		return;
	}

	// turn it on
	if (self->delay)
		self->nextthink = level.time + self->delay;
	else
		func_timer_think (self);
}

void SP_func_timer (edict_t *self)
{
	if (!self->wait)
		self->wait = 1.0;

	self->use = func_timer_use;
	self->think = func_timer_think;

	if (self->random >= self->wait)
	{
		self->random = self->wait - FRAMETIME;
		gi.dprintf("func_timer at %s has random >= wait\n", vtos(self->s.origin));
	}

	if (self->spawnflags & 1)
	{
		self->nextthink = level.time + 1.0 + st.pausetime + self->delay + self->wait + gi.flrand(-self->random, self->random);
		self->activator = self;
	}

	self->svflags = SVF_NOCLIENT;
}


/*QUAKED func_conveyor (0 .5 .8) ? START_ON TOGGLE
Conveyors are stationary brushes that move what's on them.
The brush should be have a surface with at least one current content enabled.
speed	default 100
*/

void func_conveyor_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->spawnflags & 1)
	{
		self->speed = 0;
		self->spawnflags &= ~1;
	}
	else
	{
		self->speed = self->count;
		self->spawnflags |= 1;
	}

	if (!(self->spawnflags & 2))
		self->count = 0;
}

void SP_func_conveyor (edict_t *self)
{
	if (!self->speed)
		self->speed = 100;

	if (!(self->spawnflags & 1))
	{
		self->count = self->speed;
		self->speed = 0;
	}

	self->use = func_conveyor_use;

	gi.setmodel (self, self->model);
	self->solid = SOLID_BSP;
	gi.linkentity (self);
}



/*QUAKED func_killbox (1 0 0) ?
Kills everything inside when fired, irrespective of protection.
*/
void use_killbox (edict_t *self, edict_t *other, edict_t *activator)
{
	KillBox (self);
}

void SP_func_killbox (edict_t *ent)
{
	gi.setmodel (ent, ent->model);
	ent->use = use_killbox;
	ent->svflags = SVF_NOCLIENT;
}

/*QUAKED func_pushkillbox (1 0 0) ?

volume - speed things are shot out at
count - angle they're shot out at

*/

void touch_pushkillbox (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{

	vec3_t	feet;
	vec3_t	up = {0,0,1};

	VectorCopy(other->s.origin, feet);
	feet[2] += other->mins[2];

	T_Damage(other, self, self, up, feet, feet, 100000, 0, 0, MOD_CRUSH);

	if (other->client)
	{
		other->movetype = MOVETYPE_DAN;
	}
	else
	{
		ai_public_c* aipub = other->ai;
		ai_c * ai;
		ai = (ai_c*)aipub; // sigh, is this really necessary?

		if (ai)
		{
			if(ai->GetBody())
			{
				ai->GetBody()->SetTrainDeath(true);
			}
		}
		other->movetype = MOVETYPE_NOCLIP;
	}
	other->velocity[0] = self->volume * cos(DEGTORAD * self->count);
	other->velocity[1] = self->volume * sin(DEGTORAD * self->count);
	other->velocity[2] = 0;
	other->elasticity = -1;
//	other->friction = 0;
//	other->gravity = 0;
	gi.linkentity(other);
	
}

void SP_func_pushkillbox (edict_t *self)
{
	gi.setmodel (self, self->model);
	self->clipmask = MASK_MONSTERSOLID|MASK_PLAYERSOLID|MASK_DEADSOLID;
	self->touch = touch_pushkillbox;
	self->solid = SOLID_BSP;
	self->movetype = MOVETYPE_NONE;
	self->svflags |= SVF_NOCLIENT;
	gi.linkentity(self);
}

/*
======================================================================

DOORS

  spawn a trigger surrounding the entire team unless it is
  already targeted by another

======================================================================
*/

/*QUAKED func_door (0 .5 .8) ? START_OPEN x CRUSHER NOMONSTER LOCKED TOGGLE ANIMATED_FAST AUTOOPEN  USE_TARGET	x  DUAL_TARGET
TOGGLE		wait in both the start and end states for a trigger event.
START_OPEN	the door to moves to its destination when spawned, and operate in reverse.  It is used to temporarily or permanently close off an area when triggered (not useful for touch or takedamage doors).
NOMONSTER	monsters will not trigger this door
LOCKED - door won't open until used by a non-player entity
USE_TARGET - door can be used by player even if it has a targetname
DUAL_TARGET - door fires target on both open AND closed, instead of just open

"message"	is printed when the door is touched if it is a trigger door and it hasn't been fired yet
"sp_message" is for a string package ID (numerical)
"angle"		determines the opening direction
"targetname" if set, no touch field will be spawned and a remote button or trigger field activates the door.
"health"	if set, door must be shot open
"speed"		movement speed (100 default)
"wait"		wait before returning (3 default, -1 = never return)
"lip"		lip remaining at end of move (8 default)
"dmg"		damage to inflict when blocked (2 default)
"soundName"	- name of the sound type for the door
*/

qboolean smart_door_side_check (edict_t *self, edict_t *activator)
{
	vec3_t		doorpoints[3], inplane[2], normal, toplayer;

	if (!activator)
	{
		return false;
	}

//		make a plane containing the origins of the origin brush, the door, and a point
//		which is the sum of movedir (slightly rearranged   (x, z, y)) and one of the others
	VectorCopy(self->s.origin, doorpoints[0]); // origin brush origin
	VectorAdd(self->s.origin, self->mins, doorpoints[1]);
	VectorMA(doorpoints[1], .5, self->size, doorpoints[1]); // door center
	doorpoints[2][0] = self->s.origin[0] + self->movedir[2];
	doorpoints[2][1] = self->s.origin[1] + self->movedir[0];
	doorpoints[2][2] = self->s.origin[2] + self->movedir[1]; // third point
	VectorSubtract(doorpoints[1],doorpoints[0],inplane[0]);
	VectorSubtract(doorpoints[2],doorpoints[0],inplane[1]);
	CrossProduct(inplane[0], inplane[1], normal);
	VectorSubtract(activator->s.origin, doorpoints[1], toplayer);
	if ( DotProduct(normal, toplayer) < 0 )
	{
		return true;
	}
	return false;
}


void door_use_areaportals (edict_t *self)
{
	edict_t	*t = NULL;

	if (!self->target)
		return;

	while ((t = G_Find (t, FOFS(targetname), self->target)))
	{
		if (stricmp(t->classname, "func_areaportal") == 0)
		{
			t->count ^= 1;		// toggle state
			gi.SetAreaPortalState (t->style, t->count);
		}
	}
}

void door_go_down (edict_t *self);

void door_hit_top (edict_t *self)
{
	if (!(self->flags & FL_TEAMSLAVE))
	{
		if (self->moveinfo.sound_end != -1)
			gi.sound (self, CHAN_NO_PHS_ADD+CHAN_VOICE, self->moveinfo.sound_end, 1, ATTN_NORM, 0);
		self->s.sound = 0;
	}
	self->moveinfo.state = STATE_TOP;
	if (self->spawnflags & DOOR_START_OPEN)
	{
		if (self->DoorIsOpen)
		{
			door_use_areaportals (self);
			self->DoorIsOpen = false;
		}
		else
		{
			self->DoorIsOpen = true;
		}
	}
	else
	{
		self->DoorIsOpen = true;
	}
	if (self->spawnflags & DOOR_TOGGLE)
		return;
	if (self->moveinfo.wait >= 0)
	{
		if(!(self->flags & FL_TEAMSLAVE))
		{	//this might be dangerous...
			if (strcmp(self->classname, "func_door_portcullis") == 0)
			{
				self->moveinfo.thinkfunc = Move_Done;
			}
			else
			{
				self->think = door_go_down;
			}
			self->nextthink = level.time + self->moveinfo.wait;
		}
	}
}

void door_hit_bottom (edict_t *self)
{
	if (!(self->flags & FL_TEAMSLAVE))
	{
		if (self->moveinfo.sound_end != -1)
			gi.sound (self, CHAN_NO_PHS_ADD+CHAN_VOICE, self->moveinfo.sound_end, 1, ATTN_NORM, 0);
		self->s.sound = 0;
	}
	self->moveinfo.state = STATE_BOTTOM;
	if (!(self->spawnflags & DOOR_START_OPEN))
	{
	
		if (self->DoorIsOpen)
		{
			door_use_areaportals (self);
			self->DoorIsOpen = false;
		}
		else
		{
			self->DoorIsOpen = true;
		}
	}
	else
	{
		self->DoorIsOpen = true;
	}
}

void door_go_down_logic(edict_t *self)
{
	if (self->max_health)
	{
		self->takedamage = DAMAGE_YES;
		self->health = self->max_health;
	}
	
	self->moveinfo.state = STATE_DOWN;
	if ( (strcmp(self->classname, "func_door") == 0) || (strcmp(self->classname, "func_door_portcullis") == 0) )
		Move_Calc (self, self->moveinfo.start_origin, door_hit_bottom);
	else if (strcmp(self->classname, "func_door_rotating_smart") == 0)
	{
		AngleMove_Calc (self, door_hit_bottom);
		VectorMakePos(self->moveinfo.end_angles, self->moveinfo.end_angles);
	}
	else if (strcmp(self->classname, "func_door_rotating") == 0)
		AngleMove_Calc (self, door_hit_bottom);

	if (self->spawnflags & DOOR_START_OPEN)
	{	
		if (!self->DoorIsOpen)
		{
			door_use_areaportals (self);
		}
	}
}

#define DOOR_FIND_RAD 96

void door_go_down (edict_t *self)
{
	//I think I'm getting into slightly dangerous territory here - but the current doors are so damn annoying, I kind of have to...

	edict_t *search = 0;
	int		close = 1;
	edict_t *source;

	// only go down if none of your components are blocked...
	// this is totally arbitrary... hope it works okay...
	for(source = self; source; source = source->teamchain)
	{
		CRadiusContent rad(source->s.origin, DOOR_FIND_RAD);

		for(int i = 0; i < rad.getNumFound(); i++)
		{
			search = rad.foundEdict(i);

			if(search->client || search->ai)
			{
				if(search->health > 0)
				{
					close = 0;
				}
			}
		}
		if(!close)
		{
			self->nextthink = level.time + 1.0;
			return;
		}
	}

	if (self->moveinfo.sound_start != -1)
		gi.sound (self, CHAN_NO_PHS_ADD+CHAN_VOICE, self->moveinfo.sound_start, 1, ATTN_NORM, 0);
	if(self->moveinfo.sound_middle != -1)
	{
		self->s.sound = self->moveinfo.sound_middle;
		self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
	}

	if (self->spawnflags & DOOR_DUALTARGET)
	{
		G_UseTargets (self, self);
	}

	// slave doors should not call this. only masters.
	if (self->teammaster && (self->teammaster != self))
	{
		return;
	}
	for(source = self; source; source = source->teamchain)
	{
		door_go_down_logic(source);
	}
}

void door_go_up (edict_t *self, edict_t *activator)
{
	if (self->moveinfo.state == STATE_UP)
		return;		// already going up

	if (self->moveinfo.state == STATE_TOP)
	{	// reset top wait time
		if (self->moveinfo.wait >= 0)
			self->nextthink = level.time + self->moveinfo.wait;
		return;
	}
	
	if (!(self->flags & FL_TEAMSLAVE))
	{
		if (self->moveinfo.sound_start != -1)
			gi.sound (self, CHAN_NO_PHS_ADD+CHAN_VOICE, self->moveinfo.sound_start, 1, ATTN_NORM, 0);
		if (self->moveinfo.sound_middle != -1)
		{
			self->s.sound = self->moveinfo.sound_middle;
			self->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
		}
	}
	self->moveinfo.state = STATE_UP;
	if ( (strcmp(self->classname, "func_door") == 0) || (strcmp(self->classname, "func_door_portcullis") == 0) )
	{
		Move_Calc (self, self->moveinfo.end_origin, door_hit_top);
	}
	else if (strcmp(self->classname, "func_door_rotating_smart") == 0)
	{
		if (smart_door_side_check(self, activator))
		{
			VectorNegate(self->moveinfo.end_angles, self->moveinfo.end_angles);
		}
		AngleMove_Calc (self, door_hit_top);
	}
	else if (strcmp(self->classname, "func_door_rotating") == 0)
	{
		AngleMove_Calc (self, door_hit_top);
	}
	G_UseTargets (self, activator);
	if (!(self->spawnflags & DOOR_START_OPEN))
	{
		if (!self->DoorIsOpen)
		{
			door_use_areaportals (self);
		}
	}
}

void door_use (edict_t *self, edict_t *other, edict_t *activator)
{
	edict_t	*ent;

	//  A non-player used it, so unlock door 
	if ((self->health != -667)&&(self->spawnflags & DOOR_LOCKED))
	{
		self->spawnflags &= ~DOOR_LOCKED;
		// if this door is chained to another, unlock both of them
		if (self->teamchain)
		{
			door_use(self->teamchain, other, activator);
		}
		return;	// That's all we want to do
	}

	if (self->spawnflags & DOOR_LOCKED)	// It's locked, silly.
		return;

	if (self->flags & FL_TEAMSLAVE)
	{
		door_use(self->teammaster, other, activator);
		return;
	}

	if (!(self->spawnflags & DOOR_USE_TARGET))	// This lets player use a door that's got a targetname
	{
		if((self->targetname)&&(self->health == -667))
		{
			return;//prevents team chain doors from being used...
		}
	}


	if (self->spawnflags & DOOR_TOGGLE)
	{
		if (self->moveinfo.state == STATE_UP || self->moveinfo.state == STATE_TOP)
		{
			// trigger all paired doors
			for (ent = self ; ent ; ent = ent->teamchain)
			{
				ent->message = NULL;
				ent->sp_message = 0;
				ent->touch = NULL;
				door_go_down (ent);
			}
			return;
		}
	}
	
	// trigger all paired doors
	for (ent = self ; ent ; ent = ent->teamchain)
	{
		ent->message = NULL;
		ent->sp_message = NULL;
		ent->touch = NULL;

		// kef -- when a door is used by a script, the 'other' will be the script_runner. when
		//the door is used by a player, the 'other' will be the player. in either case, we want 
		//to use 'other'. HOWEVER, if the door is activated by a trigger, the trigger's origin 
		//is, well, the origin, so in that case we want to use the activator. 
		//
		//BOTTOM LINE: only use 'other' when it's a script_runner.
		//
//		door_go_up (ent, activator);
		if (strcmp(other->classname, "script_runner") == 0)
		{
			door_go_up (ent, other);
		}
		else
		{
			door_go_up (ent, activator);
		}
	}
}

void plDoorUse(edict_t *self, edict_t *other, edict_t *activator)
{
	int oldHealth;

	oldHealth = self->health;
	self->health = -667;
	// check for nomonster here
	if ((self->spawnflags & DOOR_NOMONSTER) && (other->svflags & SVF_MONSTER))
		return;
	door_use(self, other, activator);
	self->last_move_time = level.time;
	self->health = oldHealth;
}

void Touch_DoorTrigger (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	int oldHealth;

	oldHealth = self->owner->health;

	if (other->health <= 0)
		return;

	if (!(other->svflags & SVF_MONSTER) && (!other->client))
		return;

	if ((self->owner->spawnflags & DOOR_NOMONSTER) && (other->svflags & SVF_MONSTER))
		return;

	if (level.time < self->touch_debounce_time)
		return;

	// if a player is touching this door trigger, let door_use know by setting the door's
	//health to some nutty value. additionally, we don't want any ai guys unlocking this
	//door early by bumping into the trigger.
	if (other && (other->client || other->ai))
	{
		self->owner->health = -667;
	}

	self->touch_debounce_time = level.time + 1.0;
	door_use (self->owner, other, other);
	self->owner->health = oldHealth;
}

void Use_DoorTrigger(edict_t *self, edict_t *other, edict_t *activator)
{
	if (other->health <= 0)
		return;

	if (!(other->svflags & SVF_MONSTER) && (!other->client))
		return;

	if ((self->owner->spawnflags & DOOR_NOMONSTER) && (other->svflags & SVF_MONSTER))
		return;

	if (level.time < self->touch_debounce_time)
		return;

	if(self->owner->targetname)
	{
		return;//needs to be triggered remotely...
	}

	self->touch_debounce_time = level.time + 1.0;
	door_use (self->owner, other, other);
	self->last_move_time = level.time;
}

void Think_CalcMoveSpeed (edict_t *self)
{
	edict_t	*ent;
	float	min;
	float	time;
	float	newspeed;
	float	ratio;
	float	dist;

	self->moveinfo.sound_start = SND_getBModelSoundIndex(self->soundName, 0);
	self->moveinfo.sound_middle = SND_getBModelSoundIndex(self->soundName, 1);
	self->moveinfo.sound_end = SND_getBModelSoundIndex(self->soundName, 2);

	if (self->flags & FL_TEAMSLAVE)
		return;		// only the team master does this

	// find the smallest distance any member of the team will be moving
	min = fabs(self->moveinfo.distance);
	for (ent = self->teamchain; ent; ent = ent->teamchain)
	{
		dist = fabs(ent->moveinfo.distance);
		if (dist < min)
			min = dist;
	}

	time = min / self->moveinfo.speed;

	// adjust speeds so they will all complete at the same time
	for (ent = self; ent; ent = ent->teamchain)
	{
		newspeed = fabs(ent->moveinfo.distance) / time;
		ratio = newspeed / ent->moveinfo.speed;
		if (ent->moveinfo.accel == ent->moveinfo.speed)
			ent->moveinfo.accel = newspeed;
		else
			ent->moveinfo.accel *= ratio;
		if (ent->moveinfo.decel == ent->moveinfo.speed)
			ent->moveinfo.decel = newspeed;
		else
			ent->moveinfo.decel *= ratio;
		ent->moveinfo.speed = newspeed;
	}
}

void Think_SpawnDoorTrigger (edict_t *ent)
{
	edict_t		*other;
	vec3_t		mins, maxs;

	ent->moveinfo.sound_start = SND_getBModelSoundIndex(ent->soundName, 0);
	ent->moveinfo.sound_middle = SND_getBModelSoundIndex(ent->soundName, 1);
	ent->moveinfo.sound_end = SND_getBModelSoundIndex(ent->soundName, 2);

	if (ent->flags & FL_TEAMSLAVE)
		return;		// only the team leader spawns a trigger

	VectorCopy (ent->absmin, mins);
	VectorCopy (ent->absmax, maxs);

	for (other = ent->teamchain ; other ; other=other->teamchain)
	{
		AddPointToBounds (other->absmin, mins, maxs);
		AddPointToBounds (other->absmax, mins, maxs);
	}

	// expand 
	mins[0] -= 60;
	mins[1] -= 60;
	maxs[0] += 60;
	maxs[1] += 60;

	other = G_Spawn ();
	VectorCopy (mins, other->mins);
	VectorCopy (maxs, other->maxs);
	other->owner = ent;
	other->solid = SOLID_TRIGGER;
	other->movetype = MOVETYPE_NONE;
	other->touch = Touch_DoorTrigger;
	other->use = Use_DoorTrigger;
	other->plUse = Use_DoorTrigger;
	ent->last_move_time = level.time - 10.0;//safe for monsters to use me...
	gi.linkentity (other);

	if (ent->spawnflags & DOOR_START_OPEN)
		door_use_areaportals (ent);

	Think_CalcMoveSpeed (ent);
}

void portcullis_think(edict_t* ent)
{	// we want the portcullis to remain closed at all times unless it has recently been
	//used by a trigger_keep_using, in which case it's allowed to keep opening. as soon
	//as it stops being used, it starts closing.

	// save this door's state. if it changes by the end of this fn, we may want
	//to use our target
	int		nOldState = ent->moveinfo.state;
	edict_t *ourTarget = NULL;

	if ((level.time - ent->last_move_time) > (2*FRAMETIME))
	{	// hasn't been used recently. start closing.
		ent->count = 0;
	}
	if (ent->count)
	{	// opening -- use ent->speed
		ent->moveinfo.accel = ent->moveinfo.decel = ent->moveinfo.speed = ent->speed;
		if ((ent->moveinfo.state == STATE_UP) ||
			(ent->moveinfo.state == STATE_TOP))
		{	// keep going with current thinking
			if (ent->moveinfo.thinkfunc)
			{
				if (ent->moveinfo.wait < level.time)
				{
					ent->moveinfo.thinkfunc(ent);
					if (ent->moveinfo.wait > 0)
					{
						ent->moveinfo.wait = 0;
					}
				}
			}
			else
			{
				door_go_up(ent, ent);
			}
		}
		else
		{	// we just got used by a trigger, so start moving up
			ent->moveinfo.thinkfunc = NULL;
			door_go_up(ent, ent);
		}
	}
	else
	{	// closing -- use ent->volume
		ent->moveinfo.accel = ent->moveinfo.decel = ent->moveinfo.speed = ent->volume;
		if (ent->moveinfo.state == STATE_DOWN)
		{	// keep going with current thinking
			if (ent->moveinfo.thinkfunc)
			{
				if (ent->moveinfo.wait < level.time)
				{
					ent->moveinfo.thinkfunc(ent);
					if (ent->moveinfo.wait > 0)
					{
						ent->moveinfo.wait = 0;
					}
				}
			}
			else
			{
				door_go_down(ent);
			}
		}
		else if (ent->moveinfo.state == STATE_BOTTOM)
		{	// don't do anything. we're closed.
		}
		else
		{	// we just stopped being used by a trigger, so start moving down
			ent->moveinfo.thinkfunc = NULL;
			door_go_down(ent);
		}
	}

	// if the door just started going up, fire our target.
	//if it just started going down, fire our target.

	// 11/2/99 kef -- changed this to "use" our target every frame, cuz our target is
	//designed to be a misc_iraq_valve with the PORTCULLIS flag, an object which can
	//figure out what to do based on the door's state.
	if (true || (ent->moveinfo.state == STATE_UP) && (ent->moveinfo.state != nOldState) )
	{	// started going up
		if (ent->target)
		{
			while ((ourTarget = G_Find (ourTarget, FOFS(targetname), ent->target)))
			{
				ourTarget->use(ourTarget, ent, ent);
			}
		}
	}
	else if ( (ent->moveinfo.state == STATE_DOWN) && (ent->moveinfo.state != nOldState) )
	{	// started going down
		if (ent->target)
		{
			while ((ourTarget = G_Find (ourTarget, FOFS(targetname), ent->target)))
			{
				ourTarget->use(ourTarget, ent, ent);
			}
		}
	}
	ent->nextthink = level.time + FRAMETIME;
}

void door_blocked  (edict_t *self, edict_t *other)
{
	vec3_t	toTarget;
	edict_t	*ent;

	if(other->s.solid == SOLID_CORPSE)
	{
		other->s.solid = SOLID_NOT;
	}

	if (!(other->svflags & SVF_MONSTER) && (!other->client) )
	{
		// give it a chance to go away on it's own terms (like gibs)
		T_Damage (other, self, self, vec3_origin, other->s.origin, other->s.origin, 100000, 1, 0, MOD_CRUSH);
		// if it's still there, nuke it
		if (other)
			BecomeExplosion1 (other);
		return;
	}

//	T_Damage (other, self, self, vec3_origin, other->s.origin, other->s.origin, self->dmg, 1, 0, MOD_CRUSH);

	if (self->spawnflags & DOOR_CRUSHER)
	{	// if there's a monster blocking the door, gib it
		if (other->ai)
		{	// calculating the direction of damage (sigh)...
			//
			// if the door moves horizontally, use [-movedir[1], movedir[0], 0] for a damage dir. if it
			//moves vertically, you're out of luck
			vec3_t vDamageDir = {-self->movedir[1], self->movedir[0], 0};
			vec3_t vDamageOrigin;
			
			VectorMA(other->s.origin, -100, vDamageDir, vDamageOrigin);
			VectorMA(other->s.origin, 50, vDamageDir, other->s.origin);
			T_Damage (other, self, self, vDamageDir, other->s.origin, vDamageOrigin, 1000/*dmg*/, 100/*knockback*/,
				DT_MANGLE, MOD_CRUSH);
		}
		return;
	}

	// ok, if the thing being hit by the door is ai, give it some knockback
	if (other->ai)
	{
		VectorSubtract(other->s.origin, self->s.origin, toTarget);
		toTarget[2] = 10;
		T_Damage (other, self, self, toTarget, other->s.origin, self->s.origin, 1/*dmg*/, 100/*knockback*/,
			0, MOD_CRUSH);
		return;
	}

// if a door has a negative wait, it would never come back if blocked,
// so let it just squash the object to death real fast
	if (self->moveinfo.wait >= 0)
	{
		if (self->moveinfo.state == STATE_DOWN)
		{
			for (ent = self->teammaster ; ent ; ent = ent->teamchain)
				door_go_up (ent, other);
//				door_go_up (ent, ent->activator);
		}
		else
		{
			for (ent = self->teammaster ; ent ; ent = ent->teamchain)
				door_go_down (ent);
		}
	}
	self->nextthink = level.time + .1;
}

void door_killed (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	edict_t	*ent;

	for (ent = self->teammaster ; ent ; ent = ent->teamchain)
	{
		ent->health = ent->max_health;
		ent->takedamage = DAMAGE_NO;
	}
	door_use (self->teammaster, attacker, attacker);
}

void door_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	
	if (!other->client) // eh?
		return;

	if (level.time < self->touch_debounce_time)
		return;
	self->touch_debounce_time = level.time + 5.0;

	if (self->sp_message)
	{
		gi.SP_Print(other, self->sp_message);
		gi.sound (other, CHAN_AUTO, gi.soundindex ("Misc/Talk.wav"), 1, ATTN_NORM, 0);//FIXME
	}
	else
	{
		gi.centerprintf (other, "%s", self->message);
		gi.sound (other, CHAN_AUTO, gi.soundindex ("Misc/Talk.wav"), 1, ATTN_NORM, 0);//FIXME
	}
}

void door_spawn_areaportal_think (edict_t *ent)
{
	bool	bAutoOpen = (ent->classname && (0 == strcmp("func_door", ent->classname)) && (ent->spawnflags & 128));
	bool	bPortcullis = ent->classname && (0 == strcmp("func_door_portcullis", ent->classname));

	if (ent->spawnflags & DOOR_START_OPEN)
	{
		door_use_areaportals (ent);
	}

	// I know this looks a little silly and redundant, but I don't want to spend the time
	//figuring out the proper logical representation of these conditions. basically, I want 
	//doors marked as auto-open to open automatically, no matter what. is that so much to ask?
	if (bAutoOpen)
	{
		ent->think = Think_SpawnDoorTrigger;
	}
	else if (bPortcullis)
	{	// needs to go back to its own think function
		ent->think = portcullis_think;
	}
	else
	{	// such a mess. such, such a mess.
//		if (ent->health || ent->targetname || (!bAutoOpen))
			ent->think = Think_CalcMoveSpeed;
//		else
//			ent->think = Think_SpawnDoorTrigger;
	}

	ent->nextthink = level.time + FRAMETIME;

}
void SP_func_door (edict_t *ent)
{
	vec3_t	abs_movedir;

	G_SetMovedir (ent->s.angles, ent->movedir);
	ent->movetype = MOVETYPE_PUSH;
	ent->solid = SOLID_BSP;
	gi.setmodel (ent, ent->model);

	ent->blocked = door_blocked;
	ent->use = door_use;

	if ((!ent->targetname) || (ent->spawnflags & DOOR_USE_TARGET))
	{
		ent->plUse = plDoorUse;
		ent->last_move_time = level.time - 10.0;//safe for monsters to use me...
	}

	if (!ent->speed)
		ent->speed = 100;
	if (dm->isDM())
		ent->speed *= 2;

	if (!ent->accel)
		ent->accel = ent->speed;
	if (!ent->decel)
		ent->decel = ent->speed;

	if (!ent->wait)
		ent->wait = 3;
	if (!st.lip)
		st.lip = 8;
	if (!ent->dmg)
		ent->dmg = 2;

	// calculate second position
	VectorCopy (ent->s.origin, ent->pos1);
	abs_movedir[0] = fabs(ent->movedir[0]);
	abs_movedir[1] = fabs(ent->movedir[1]);
	abs_movedir[2] = fabs(ent->movedir[2]);
	ent->moveinfo.distance = abs_movedir[0] * ent->size[0] + abs_movedir[1] * ent->size[1] + abs_movedir[2] * ent->size[2] - st.lip;
	VectorMA (ent->pos1, ent->moveinfo.distance, ent->movedir, ent->pos2);

	// if it starts open, switch the positions
	ent->DoorIsOpen = false;
	if (ent->spawnflags & DOOR_START_OPEN)
	{
		VectorCopy (ent->pos2, ent->s.origin);
		VectorCopy (ent->pos1, ent->pos2);
		VectorCopy (ent->s.origin, ent->pos1);
		ent->DoorIsOpen = true;
	}

	ent->moveinfo.state = STATE_BOTTOM;

	if (ent->health)
	{
		ent->takedamage = DAMAGE_YES;
		ent->die = door_killed;
		ent->max_health = ent->health;
	}
	else if (ent->targetname && (ent->message || ent->sp_message))
	{
		gi.soundindex ("misc/talk.wav");
		ent->touch = door_touch;
	}
	
	ent->moveinfo.speed = ent->speed;
	ent->moveinfo.accel = ent->accel;
	ent->moveinfo.decel = ent->decel;
	ent->moveinfo.wait = ent->wait;
	VectorCopy (ent->pos1, ent->moveinfo.start_origin);
	VectorCopy (ent->s.angles, ent->moveinfo.start_angles);
	VectorCopy (ent->pos2, ent->moveinfo.end_origin);
	VectorCopy (ent->s.angles, ent->moveinfo.end_angles);

//	if (ent->spawnflags & 16)
//		ent->s.effects |= EF_ANIM_ALL;
//	if (ent->spawnflags & 64)
//		ent->s.effects |= EF_ANIM_ALLFAST;

	// to simplify logic elsewhere, make non-teamed doors into a team of one
	if (!ent->team)
		ent->teammaster = ent;

	gi.linkentity (ent);

	ent->think = door_spawn_areaportal_think;
	ent->nextthink = level.time + FRAMETIME;

}


/*QUAKED func_door_rotating (0 .5 .8) ? START_OPEN REVERSE CRUSHER NOMONSTER LOCKED TOGGLE X_AXIS Y_AXIS USE_TARGET IGNORE_HACK
TOGGLE causes the door to wait in both the start and end states for a trigger event.

START_OPEN	the door to moves to its destination when spawned, and operate in reverse.  It is used to temporarily or permanently close off an area when triggered (not useful for touch or takedamage doors).
NOMONSTER	monsters will not trigger this door
LOCKED - door won't open until used by a non-player entity
USE_TARGET - door can be used by player even if it has a targetname
IGNORE_HACK	- use this flag if the door's origin is not at the edge (hope you're happy, Zuk)

You need to have an origin brush as part of this entity.  The center of that brush will be
the point around which it is rotated. It will rotate around the Z axis by default.  You can
check either the X_AXIS or Y_AXIS box to change that.

"distance" is how many degrees the door will be rotated.
"speed" determines how fast the door moves; default value is 100.

REVERSE will cause the door to rotate in the opposite direction.

"message"	is printed when the door is touched if it is a trigger door and it hasn't been fired yet
"sp_message" is for a string package ID (numerical)
"angle"		determines the opening direction
"targetname" if set, no touch field will be spawned and a remote button or trigger field activates the door.
"health"	if set, door must be shot open
"speed"		movement speed (100 default)
"wait"		wait before returning (3 default, -1 = never return)
"dmg"		damage to inflict when blocked (2 default)
"soundName"	- sound type for the door
*/

void SP_func_door_rotating (edict_t *ent)
{
	VectorClear (ent->s.angles);

	// set the axis of rotation
	VectorClear(ent->movedir);
	if (ent->spawnflags & DOOR_X_AXIS)
	{
		ent->movedir[2] = 1.0;
		if (!(ent->spawnflags & DOOR_IGNOREHACK))
		{
			ent->svflags |= SVF_DOORHACK_X;
		}
	}
	else if (ent->spawnflags & DOOR_Y_AXIS)
	{
		ent->movedir[0] = 1.0;
		if (!(ent->spawnflags & DOOR_IGNOREHACK))
		{
			ent->svflags |= SVF_DOORHACK_Y;
		}
	}
	else // Z_AXIS
	{
		ent->movedir[1] = 1.0;
		if (!(ent->spawnflags & DOOR_IGNOREHACK))
		{
			ent->svflags |= SVF_DOORHACK_Z;
		}
	}

	// check for reverse rotation
	if (ent->spawnflags & DOOR_REVERSE)
		VectorNegate (ent->movedir, ent->movedir);

	if (!st.distance)
	{
//		gi.dprintf("%s at %s with no distance set\n", ent->classname, vtos(ent->s.origin));
		st.distance = 90;
	}

	VectorCopy (ent->s.angles, ent->pos1);
	VectorMA (ent->s.angles, st.distance, ent->movedir, ent->pos2);
	ent->moveinfo.distance = st.distance;

	ent->movetype = MOVETYPE_PUSH;
	ent->solid = SOLID_BSP;
	gi.setmodel (ent, ent->model);

	ent->blocked = door_blocked;
	ent->use = door_use;
	if ((!ent->targetname) || (ent->spawnflags & DOOR_USE_TARGET))
	{
		ent->plUse = plDoorUse;
		ent->last_move_time = level.time - 10.0;//safe for monsters to use me...
	}

	if (!ent->speed)
		ent->speed = 100;
	if (!ent->accel)
		ent->accel = ent->speed;
	if (!ent->decel)
		ent->decel = ent->speed;

	if (!ent->wait)
		ent->wait = 3;
	if (!ent->dmg)
		ent->dmg = 2;

	// if it starts open, switch the positions
	if (ent->spawnflags & DOOR_START_OPEN)
	{
		VectorCopy (ent->pos2, ent->s.angles);
		VectorCopy (ent->pos1, ent->pos2);
		VectorCopy (ent->s.angles, ent->pos1);
		VectorNegate (ent->movedir, ent->movedir);
	}

	if (ent->health)
	{
		ent->takedamage = DAMAGE_YES;
		ent->die = door_killed;
		ent->max_health = ent->health;
	}
	
	if (ent->targetname && (ent->message || ent->sp_message))
	{
		gi.soundindex ("misc/talk.wav");
		ent->touch = door_touch;
	}

	ent->moveinfo.state = STATE_BOTTOM;
	ent->moveinfo.speed = ent->speed;
	ent->moveinfo.accel = ent->accel;
	ent->moveinfo.decel = ent->decel;
	ent->moveinfo.wait = ent->wait;
	VectorCopy (ent->s.origin, ent->moveinfo.start_origin);
	VectorCopy (ent->pos1, ent->moveinfo.start_angles);
	VectorCopy (ent->s.origin, ent->moveinfo.end_origin);
	VectorCopy (ent->pos2, ent->moveinfo.end_angles);

//	if (ent->spawnflags & 16)
//		ent->s.effects |= EF_ANIM_ALL;

	// to simplify logic elsewhere, make non-teamed doors into a team of one
	if (!ent->team)
		ent->teammaster = ent;

	gi.linkentity (ent);

	ent->think = door_spawn_areaportal_think;
	ent->nextthink = level.time + FRAMETIME;
}

/*QUAKED func_door_rotating_smart (0 .5 .8) ? START_OPEN REVERSE CRUSHER NOMONSTER LOCKED TOGGLE X_AXIS Y_AXIS USE_TARGET IGNORE_HACK

TOGGLE causes the door to wait in both the start and end states for a trigger event.

START_OPEN	the door to moves to its destination when spawned, and operate in reverse.  It is used to temporarily or permanently close off an area when triggered (not useful for touch or takedamage doors).
NOMONSTER	monsters will not trigger this door
LOCKED - door won't open until used by a non-player entity
USE_TARGET - door can be used by player even if it has a targetname
IGNORE_HACK	- use this flag if the door's origin is not at the edge (hope you're happy, Zuk)

You need to have an origin brush as part of this entity.  The center of that brush will be
the point around which it is rotated. It will rotate around the Z axis by default.  You can
check either the X_AXIS or Y_AXIS box to change that.

"distance" is how many degrees the door will be rotated.
"speed" determines how fast the door moves; default value is 100.

The door will open in the direction opposite that of its activator.

"message"	is printed when the door is touched if it is a trigger door and it hasn't been fired yet
"sp_message" is for a string package ID (numerical)
"angle"		determines the opening direction
"targetname" if set, no touch field will be spawned and a remote button or trigger field activates the door.
"health"	if set, door must be shot open
"speed"		movement speed (100 default)
"wait"		wait before returning (3 default, -1 = never return)
"dmg"		damage to inflict when blocked (2 default)
"soundName"	- Sound type for the door
*/

void SP_func_door_rotating_smart (edict_t *ent)
{
	VectorClear (ent->s.angles);

	ent->classname = "func_door_rotating_smart";

	// set the axis of rotation
	VectorClear(ent->movedir);
	if (ent->spawnflags & DOOR_X_AXIS)
	{
		ent->movedir[2] = 1.0;
		if (!(ent->spawnflags & DOOR_IGNOREHACK))
		{
			ent->svflags |= SVF_DOORHACK_X;
		}
	}
	else if (ent->spawnflags & DOOR_Y_AXIS)
	{
		ent->movedir[0] = 1.0;
		if (!(ent->spawnflags & DOOR_IGNOREHACK))
		{
			ent->svflags |= SVF_DOORHACK_Y;
		}
	}
	else // Z_AXIS
	{
		ent->movedir[1] = 1.0;
		if (!(ent->spawnflags & DOOR_IGNOREHACK))
		{
			ent->svflags |= SVF_DOORHACK_Z;
		}
	}

	// check for reverse rotation
//	if (ent->spawnflags & DOOR_REVERSE)
//		VectorNegate (ent->movedir, ent->movedir);

	if (!st.distance)
	{
//		gi.dprintf("%s at %s with no distance set\n", ent->classname, vtos(ent->s.origin));
		st.distance = 90;
	}

	VectorCopy (ent->s.angles, ent->pos1);
	VectorMA (ent->s.angles, st.distance, ent->movedir, ent->pos2);
	ent->moveinfo.distance = st.distance;

	ent->movetype = MOVETYPE_PUSH;
	ent->solid = SOLID_BSP;
	gi.setmodel (ent, ent->model);

	ent->blocked = door_blocked;
	ent->use = door_use;

	if ((!ent->targetname) || (ent->spawnflags & DOOR_USE_TARGET))
	{
		ent->plUse = plDoorUse;
		ent->last_move_time = level.time - 10.0;//safe for monsters to use me...
	}

	if (!ent->speed)
		ent->speed = 100;
	if (!ent->accel)
		ent->accel = ent->speed;
	if (!ent->decel)
		ent->decel = ent->speed;

	if (!ent->wait)
		ent->wait = 3;
	if (!ent->dmg)
		ent->dmg = 2;

	// if it starts open, switch the positions
	if (ent->spawnflags & DOOR_START_OPEN)
	{
		VectorCopy (ent->pos2, ent->s.angles);
		VectorCopy (ent->pos1, ent->pos2);
		VectorCopy (ent->s.angles, ent->pos1);
		VectorNegate (ent->movedir, ent->movedir);
	}

	if (ent->health)
	{
		ent->takedamage = DAMAGE_YES;
		ent->die = door_killed;
		ent->max_health = ent->health;
	}
	
	if (ent->targetname && (ent->message || ent->sp_message))
	{
		gi.soundindex ("misc/talk.wav");
		ent->touch = door_touch;
	}

	ent->moveinfo.state = STATE_BOTTOM;
	ent->moveinfo.speed = ent->speed;
	ent->moveinfo.accel = ent->accel;
	ent->moveinfo.decel = ent->decel;
	ent->moveinfo.wait = ent->wait;
	VectorCopy (ent->s.origin, ent->moveinfo.start_origin);
	VectorCopy (ent->pos1, ent->moveinfo.start_angles);
	VectorCopy (ent->s.origin, ent->moveinfo.end_origin);
	VectorCopy (ent->pos2, ent->moveinfo.end_angles);

//	if (ent->spawnflags & 16)
//		ent->s.effects |= EF_ANIM_ALL;

	// to simplify logic elsewhere, make non-teamed doors into a team of one
	if (!ent->team)
		ent->teammaster = ent;

	gi.linkentity (ent);

	ent->think = door_spawn_areaportal_think;
	ent->nextthink = level.time + FRAMETIME;
}

/*QUAKED func_door_secret (0 .5 .8) ? always_shoot 1st_left 1st_down
A secret door.  Slide back and then to the side.

open_once		doors never closes
1st_left		1st move is left of arrow
1st_down		1st move is down from arrow
always_shoot	door is shootebale even if targeted

"angle"		determines the direction
"dmg"		damage to inflic when blocked (default 2)
"wait"		how long to hold in the open position (default 5, -1 means hold)
*/

#define SECRET_ALWAYS_SHOOT	1
#define SECRET_1ST_LEFT		2
#define SECRET_1ST_DOWN		4

void door_secret_move1 (edict_t *self);
void door_secret_move2 (edict_t *self);
void door_secret_move3 (edict_t *self);
void door_secret_move4 (edict_t *self);
void door_secret_move5 (edict_t *self);
void door_secret_move6 (edict_t *self);
void door_secret_done (edict_t *self);

void door_secret_use (edict_t *self, edict_t *other, edict_t *activator)
{
	// make sure we're not already moving
	if (!VectorCompare(self->s.origin, vec3_origin))
		return;

	Move_Calc (self, self->pos1, door_secret_move1);
	door_use_areaportals (self);
}

void door_secret_move1 (edict_t *self)
{
	self->nextthink = level.time + 1.0;
	self->think = door_secret_move2;
}

void door_secret_move2 (edict_t *self)
{
	Move_Calc (self, self->pos2, door_secret_move3);
}

void door_secret_move3 (edict_t *self)
{
	if (self->wait == -1)
		return;
	self->nextthink = level.time + self->wait;
	self->think = door_secret_move4;
}

void door_secret_move4 (edict_t *self)
{
	Move_Calc (self, self->pos1, door_secret_move5);
}

void door_secret_move5 (edict_t *self)
{
	self->nextthink = level.time + 1.0;
	self->think = door_secret_move6;
}

void door_secret_move6 (edict_t *self)
{
	Move_Calc (self, vec3_origin, door_secret_done);
}

void door_secret_done (edict_t *self)
{
	if (!(self->targetname) || (self->spawnflags & SECRET_ALWAYS_SHOOT))
	{
		self->health = 0;
		self->takedamage = DAMAGE_YES;
	}
	door_use_areaportals (self);
}

void door_secret_blocked  (edict_t *self, edict_t *other)
{
	if (!(other->svflags & SVF_MONSTER) && (!other->client) )
	{
		// give it a chance to go away on it's own terms (like gibs)
		T_Damage (other, self, self, vec3_origin, other->s.origin, other->s.origin, 100000, 1, 0, MOD_CRUSH);
		// if it's still there, nuke it
		if (other)
			BecomeExplosion1 (other);
		return;
	}

	if (level.time < self->touch_debounce_time)
		return;
	self->touch_debounce_time = level.time + 0.5;

	T_Damage (other, self, self, vec3_origin, other->s.origin, other->s.origin, self->dmg, 1, 0, MOD_CRUSH);
}

void door_secret_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	self->takedamage = DAMAGE_NO;
	door_secret_use (self, attacker, attacker);
}

void SP_func_door_secret (edict_t *ent)
{
	vec3_t	forward, right, up;
	float	side;
	float	width;
	float	length;

//	ent->moveinfo.sound_start = gi.soundindex  ("doors/dr1_strt.wav");
//	ent->moveinfo.sound_middle = gi.soundindex  ("doors/dr1_mid.wav");
//	ent->moveinfo.sound_end = gi.soundindex  ("doors/dr1_end.wav");

	ent->movetype = MOVETYPE_PUSH;
	ent->solid = SOLID_BSP;
	gi.setmodel (ent, ent->model);

	ent->blocked = door_secret_blocked;
	ent->use = door_secret_use;
	ent->plUse = door_secret_use;
	ent->last_move_time = level.time - 10.0;//safe for monsters to use me...

	if (!(ent->targetname) || (ent->spawnflags & SECRET_ALWAYS_SHOOT))
	{
		ent->health = 0;
		ent->takedamage = DAMAGE_YES;
		ent->die = door_secret_die;
	}

	if (!ent->dmg)
		ent->dmg = 2;

	if (!ent->wait)
		ent->wait = 5;

	ent->moveinfo.accel =
	ent->moveinfo.decel =
	ent->moveinfo.speed = 50;

	// calculate positions
	AngleVectors (ent->s.angles, forward, right, up);
	VectorClear (ent->s.angles);
	side = 1.0 - (ent->spawnflags & SECRET_1ST_LEFT);
	if (ent->spawnflags & SECRET_1ST_DOWN)
		width = fabs(DotProduct(up, ent->size));
	else
		width = fabs(DotProduct(right, ent->size));
	length = fabs(DotProduct(forward, ent->size));
	if (ent->spawnflags & SECRET_1ST_DOWN)
		VectorMA (ent->s.origin, -1 * width, up, ent->pos1);
	else
		VectorMA (ent->s.origin, side * width, right, ent->pos1);
	VectorMA (ent->pos1, length, forward, ent->pos2);

	if (ent->health)
	{
		ent->takedamage = DAMAGE_YES;
		ent->die = door_killed;
		ent->max_health = ent->health;
	}
	else if (ent->targetname && (ent->message || ent->sp_message))
	{
		gi.soundindex ("misc/talk.wav");
		ent->touch = door_touch;
	}
	
	ent->classname = "func_door";

	gi.linkentity (ent);
}

/*QUAKED func_remote_camera (0 0.5 0.8) (-4 -4 -4) (4 4 4) ACTIVATOR_ONLY SCRIPTED NO_DELETE LW_ANIMATE

	pathtarget		- holds the name of the camera's owner entity (if any).
	target			- holds the name of the entity to be looked at.
	style			- changes the player's FOV to assigned value while in camera if defined
	volume			- defines the cinematic mode:
		0			- normal (standard FPS, player can shoot)
		1			- third person
		2			- letterbox
		4			- monitor (graphic overlay border)
		8			- remote with no special features
		16			- sniper scope
	ACTIVATOR_ONLY	- only the activating client will see the remote camera view.
	SCRIPTED		- puts player in godmode while camera is running and makes player non-solid
	NO_DELETE		- don't delete camera
	LW_ANIMATE		- camera will animate without a target after its initial spawn,
					  but it still needs a target for its initial position
					  This should ONLY be used for one-shot cameras.  
*/
/* programmer note:  I am using decel to store the player's old FOV
*/

void remove_camera(edict_t *Self)
{
	if(Self->spawnflags&CAMERA_ACTIVATOR_ONLY)
	{
		// Just for the activator.
		if (Self->style)
		{
			// FIXME see hack note is Use function
			Self->activator->client->ps.fov = Self->decel;
		}

		Self->activator->client->RemoteCameraLockCount--;
		Self->activator->client->ps.remote_id = -1;
		Self->activator->client->ps.remote_type = REMOTE_TYPE_FPS;

		if(Self->spawnflags & CAMERA_GODMODE)
		{
			Self->activator->flags = Self->activator->flags & ~(FL_GODMODE);
			// restore client to solid state
			Self->activator->solid = SOLID_BBOX;
		}
	}
	else
	{
		// For all clients.

		int		i;
		edict_t *cl_ent;

		for(i=0;i<game.maxclients;i++)
		{
			cl_ent=g_edicts+1+i;
	
			if(!cl_ent->inuse)
				continue;
			if (Self->style)
			{
				cl_ent->client->ps.fov = Self->decel;
			}
	
			cl_ent->client->RemoteCameraLockCount--;
			cl_ent->client->ps.remote_id = -1;
			cl_ent->client->ps.remote_type = REMOTE_TYPE_FPS;
			if (Self->spawnflags & CAMERA_GODMODE)
			{
				cl_ent->flags = cl_ent->flags & ~(FL_GODMODE);
				// restore client to solid state
				cl_ent->solid = SOLID_BBOX;
			}
		}
	}

	Self->count = 0;
	
	if(!(Self->spawnflags&CAMERA_NODELETE))
	{
		G_FreeEdict(Self);
	}
}

void func_remote_camera_think(edict_t *Self)
{
	// Update the position on client(s).

	if(Self->spawnflags&CAMERA_ACTIVATOR_ONLY)
	{
	// Just for the activator.

		// 1/13/00 kef -- if I'm dead, don't do this. that would be silly.
		if (Self->activator->client)
		{
			if (Self->activator->deadflag == DEAD_DEAD)
			{
				return;
			}
		}

		if(Self->activator->client->RemoteCameraNumber==Self->s.number)
		{
			int	i;

			if (Self->spawnflags & CAMERA_CAMERA_MODEL)	// Looking through camera model
			{
				for(i=0;i<3;i++)
				{
					Self->activator->client->ps.remote_vieworigin[i]=Self->targetEnt->s.origin[i]*8.0;
				}
			}
			else
			{
				for(i=0;i<3;i++)
				{
					Self->activator->client->ps.remote_vieworigin[i]=Self->s.origin[i]*8.0;
				}
			}
		}
	}
	else
	{
		// For all clients.
		int		i;
		edict_t *cl_ent;

		for(i=0;i<game.maxclients;i++)
		{
			cl_ent=g_edicts+1+i;
	
			if(!cl_ent->inuse)
				continue;
	
			// 1/13/00 kef -- if I'm dead, don't do this. that would be silly.
			if (cl_ent->client)
			{
				if (cl_ent->deadflag == DEAD_DEAD)
				{
					continue;
				}
			}

			if(cl_ent->client->RemoteCameraNumber==Self->s.number)
			{
				int j;

				if (Self->spawnflags & CAMERA_CAMERA_MODEL)	// Looking through camera model
				{
					for(j=0;j<3;j++)
					{
						cl_ent->client->ps.remote_vieworigin[j]=Self->targetEnt->s.origin[j]*8.0;
					}
				}
				else
				{
					for(j=0;j<3;j++)
					{
						cl_ent->client->ps.remote_vieworigin[j]=Self->s.origin[j]*8.0;
					}
				}
			}
		}
	}
// ********************************************************************************************
// Find my target entity and then orientate myself to look at it.
// ********************************************************************************************
	
	if(Self->targetEnt)
	{
		// Calculate the angles from myself to my target.

		vec3_t	Forward;

		if (!(Self->spawnflags & CAMERA_LWANIMATE))
		{
			VectorSubtract(Self->targetEnt->s.origin,Self->s.origin,Forward);
			VectorNormalize(Forward);
			vectoangles(Forward,Self->s.angles);
		}


		// Update the angles on client(s).

		if(Self->spawnflags&CAMERA_ACTIVATOR_ONLY)
		{
			// Just for the activator.

			if(Self->activator->client->RemoteCameraNumber==Self->s.number)
			{
				int	i;

				if (Self->spawnflags & CAMERA_CAMERA_MODEL)	// Looking through camera model
				{
					for(i=0;i<3;i++)
					{
						Self->activator->client->ps.remote_viewangles[i]=Self->targetEnt->s.angles[i];
					}
				}
				else
				{
					for(i=0;i<3;i++)
					{
						Self->activator->client->ps.remote_viewangles[i]=Self->s.angles[i];
					}
				}
			}
		}
		else
		{
			// For all clients.

			int		i;
			edict_t *cl_ent;
	
			for(i=0;i<game.maxclients;i++)
			{
				cl_ent=g_edicts+1+i;
		
				if(!cl_ent->inuse)
					continue;

				if(cl_ent->client->RemoteCameraNumber==Self->s.number)
				{
					int j;
					
					if (Self->spawnflags & CAMERA_CAMERA_MODEL)	// Looking through camera model
					{
						for(j=0;j<3;j++)
						{
							cl_ent->client->ps.remote_viewangles[j]=Self->targetEnt->s.angles[j];
						}
					}
					else
					{
						for(j=0;j<3;j++)
						{
							cl_ent->client->ps.remote_viewangles[j]=Self->s.angles[j];
						}
					}
				}
			}
		}
	}

	Self->nextthink = level.time+FRAMETIME;
}

void Use_remote_camera(edict_t *Self,edict_t *Other,edict_t *Activator)
{
	vec3_t	Forward;

	// 1/13/00 kef -- if I'm dead, don't do this. that would be silly.
	if (Activator->client)
	{
		if (Activator->deadflag == DEAD_DEAD)
		{
			return;
		}
	}


	//	If I'm already in a remote camera view, then get out of that view, so we can get into the new one!
	if(Activator->client)
	{
		if((Activator->client->ps.remote_id >= 0) && (Activator->client->ps.remote_id != Self->s.number))
		{
			edict_t	*camera = NULL;

			do
			{
				camera = G_Find(camera,FOFS(classname),"func_remote_camera");
				if (!camera)
				{
					Com_Printf("Error in Camera Scripting!  Check your 'use' and 'remove' commands!/n");
					break;
				}
				if (camera->s.number == Activator->client->ps.remote_id)
				{
					remove_camera(camera);
					break;
				}
			}
			while(1);
		}
	}
	
	
	// If I'm already in THIS remote camera, toggle me off
	if(Self->count)
	{
	// I am a scripted camera, so free myself before returning.
	// or I am a camera that is toggled by the player use key	
		remove_camera(Self);
		return;
	}

	Self->count=1; // means I'm i use

	// ********************************************************************************************
	// Signal to client(s) that a remote camera view is active,
	// ********************************************************************************************

	if(Self->spawnflags & CAMERA_ACTIVATOR_ONLY)
	{
		if (!Activator->client)
		{
			Com_Printf("Error in Camera Scripting!  Activator (%s) is not a client!\n", Activator->classname);
			return;
		}

		// Signal to just the activator (i.e. person who was ultimately responsible for triggering the
		// remote camera) that their camera view has changed to a remote camera view..
		if (Self->style) // fov defined for this camera
		{
			Self->decel = Activator->client->ps.fov; // storage of player's old fov
			Activator->client->ps.fov = Self->style;
		}

		Self->activator=Activator;
		Self->activator->client->RemoteCameraLockCount++;
		Self->activator->client->RemoteCameraNumber=Self->s.number;
		Self->activator->client->RemoteCameraType=Self->volume;
	}
	else
	{
		// Signal to all clients that their camera view has changed to a remote camera view..
		int		i;
		edict_t *cl_ent;
	
		Self->activator=Activator;

		for(i=0;i<game.maxclients;i++)
		{
			cl_ent=g_edicts+1+i;
		
			if(!cl_ent->inuse)
				continue;
	
			if (Self->style)// fov defined for this camera
			{
//  AAAGH!  FIXME.  This will only store the last player's fov.  Other players will get
//  the first player's fov after the camera's gone!
				Self->decel = cl_ent->client->ps.fov; // storage
				cl_ent->client->ps.fov = Self->style;
			}
			cl_ent->client->RemoteCameraLockCount++;
			cl_ent->client->RemoteCameraNumber=Self->s.number;
			cl_ent->client->RemoteCameraType=Self->volume;
		}
	}

// ********************************************************************************************
// Attempt to find my owner entity (i.e. what I'm fixed to). If nothing is found, then I am a
// static camera so set up my position here (it will remain unchanged hereafter).
// ********************************************************************************************

// set up my position (which might change in scripting hereafter).

	if(Self->spawnflags&CAMERA_ACTIVATOR_ONLY)
	{
		// Just for the activator.
		int	i;

		Self->enemy=NULL;
		for(i=0;i<3;i++)
		{
			Self->activator->client->ps.remote_vieworigin[i]=Self->s.origin[i]*8.0;
		}
	}
	else
	{
		// For all clients.
		int		i,j;
		edict_t *cl_ent;

		Self->enemy=NULL;
		for(i=0;i<game.maxclients;i++)
		{
			cl_ent=g_edicts+1+i;
			if(!cl_ent->inuse)
			{
				continue;
			}
			for(j=0;j<3;j++)
			{
				cl_ent->client->ps.remote_vieworigin[j]=Self->s.origin[j]*8.0;
			}
		}
	}
// ********************************************************************************************
// Find my target entity and then orientate myself to look at it.
// ********************************************************************************************
	Self->targetEnt=G_Find(NULL,FOFS(targetname),Self->target);

	if (Self->targetEnt)	// If targeting a camera model it will follow the models angles
	{
		VectorSubtract(Self->targetEnt->s.origin,Self->s.origin,Forward);
		VectorNormalize(Forward);
		vectoangles(Forward,Self->s.angles);

		if ((strcmp(Self->targetEnt->classname,"misc_generic_security_camera")==0) || 
			(strcmp(Self->targetEnt->classname,"misc_generic_security_camera2")==0))
			Self->spawnflags |= CAMERA_CAMERA_MODEL;
	}


	// Update the angles on client(s).
	if(Self->spawnflags&CAMERA_ACTIVATOR_ONLY)
	{
		// Just for the activator.
		if(Self->activator->client->RemoteCameraNumber==Self->s.number)
		{
			int	i;

			for(i=0;i<3;i++)
			{
				Self->activator->client->ps.remote_viewangles[i]=Self->s.angles[i];
			}
		}
	}
	else
	{
		// For all clients.
		int		i;
		edict_t *cl_ent;

		for(i=0;i<game.maxclients;i++)
		{
			cl_ent=g_edicts+1+i;
			if(!cl_ent->inuse)
				continue;

			if(cl_ent->client->RemoteCameraNumber==Self->s.number)
			{
				int j;
			
				for(j=0;j<3;j++)
				{

					cl_ent->client->ps.remote_viewangles[j]=Self->s.angles[j];
				}
			}
		}
	}

	if(Self->spawnflags & CAMERA_GODMODE) // set godmode if camera is scripted
	{
		if (Self->spawnflags & CAMERA_ACTIVATOR_ONLY)
		{
			Activator->flags |= FL_GODMODE;
			// set client to non-solid
			Activator->solid = SOLID_NOT;
		}
		else
		{
			int		i;
			edict_t *cl_ent;
	

			for(i=0;i<game.maxclients;i++)
			{
				cl_ent=g_edicts+1+i;
				if(!cl_ent->inuse)
					continue;
				cl_ent->flags |= FL_GODMODE;
				// Set all clients to non-solid
				cl_ent->solid = SOLID_NOT;
			}
		}

	}
#if 0 // kef -- moved to ds.cpp/CinematicFreeze()
	// make sure this cvar-setting stuff happens _after_ setting the client's remotecameratype
	if (g_edicts[1].client)
	{
		// if we just set our camera type to letterbox, that means we just started a cinematic.
		//adjust the proper cvar.
		if ( (REMOTE_TYPE_LETTERBOX == g_edicts[1].client->RemoteCameraType) && (!game.cinematicfreeze) )
		{
			game.cinematicfreeze = 1;

			// need to make sure these commands don't stay...uh...commanded during the script
			gi.AddCommandString ("-attack\n");
			gi.AddCommandString ("-altattack\n");
			gi.AddCommandString ("-weaponExtra1\n");
			gi.AddCommandString ("-weaponExtra2\n");
			gi.AddCommandString ("-use\n");

/*	kef -- 'cinematic entities' thing is weird
			remove_non_cinematic_entites(NULL);
*/
		}
	}
#endif // kef -- moved to ds.cpp/CinematicFreeze()

	// ********************************************************************************************
	// Setup next think stuff.
	// ********************************************************************************************
	
	Self->think=func_remote_camera_think;
	Self->nextthink=level.time + FRAMETIME;


}

void SP_func_remote_camera(edict_t *Self)
{
	Self->enemy=Self->targetEnt=NULL;

	if(!Self->target)
	{
		gi.dprintf("Object 'func_remote_camera' without a target.\n");
		
		G_FreeEdict(Self);
		
		return;
	}

	Self->movetype = MOVETYPE_NONE;
	Self->solid=SOLID_NOT;
	VectorSet(Self->mins,-4,-4,-4);
	VectorSet(Self->maxs,4,4,4);
	Self->count=0;

	Self->use=Use_remote_camera;


	if (Self->volume == 4)
	{
		// kef -- need to precache the image that remote_cameras use to frame the view
		entDebrisToCache[CLGHL_ENDOFLIST] = DEBRIS_PRECACHE_IMAGE;
	}
	gi.linkentity(Self);
}

/*QUAKED func_alarm (0 .5 .8) (-8 -8 -8) (8 8 8)
Alerts everybody on the level that there is an intruder
*/

void alarm_use(edict_t *self, edict_t *other, edict_t *activator)
{
	if(!level.alertedStatus)
	{
		level.alertedStatus = 1;
		//maybe do tons of exciting things here?
	}
}

void SP_func_alarm(edict_t *self)
{
	self->movetype = MOVETYPE_NONE;
	self->solid=SOLID_NOT;
	VectorSet(self->mins,-4,-4,-4);
	VectorSet(self->maxs,4,4,4);

	self->use = alarm_use;
}


/*QUAKED func_ignite (0 .5 .8) (-8 -8 -8) (8 8 8)
Roast folks
"Health" Radius of igniting
*/

void ignite_use(edict_t *self, edict_t *other, edict_t *activator)
{
	RadiusBurn(self, self->health);
}

void SP_func_ignite(edict_t *self)
{
	self->movetype = MOVETYPE_NONE;
	self->solid=SOLID_NOT;
	VectorSet(self->mins,-4,-4,-4);
	VectorSet(self->maxs,4,4,4);

	self->use = ignite_use;
}

/*QUAKED func_Dekker_console (1 .4 .4) (-8 -8 -8) (8 8 8)
Where Dekkers will go to turn on autoguns and make cool stuff happen...
*/
void SP_func_Dekker_console(edict_t* self)
{
	self->movetype = MOVETYPE_NONE;
	self->solid=SOLID_NOT;
	VectorSet(self->mins,-4,-4,-4);
	VectorSet(self->maxs,4,4,4);
}

/*QUAKED func_Dekker_diehere (1 .4 .4) (-8 -8 -8) (8 8 8)
Where Good Dekkers go when they die...
*/
void SP_func_Dekker_diehere(edict_t* self)
{
	self->movetype = MOVETYPE_NONE;
	self->solid=SOLID_NOT;
	VectorSet(self->mins,-4,-4,-4);
	VectorSet(self->maxs,4,4,4);
}

/*QUAKED func_Dekker_jumphere (1 .4 .4) (-8 -8 -8) (8 8 8)
Spot for Dekker to hop up to...
*/
void SP_func_Dekker_jumphere(edict_t* self)
{
	self->movetype = MOVETYPE_NONE;
	self->solid=SOLID_NOT;
	VectorSet(self->mins,-4,-4,-4);
	VectorSet(self->maxs,4,4,4);
}

/*QUAKED func_Dekker_split (1 .4 .4) (-8 -8 -8) (8 8 8)
Split Dekker in half...
*/
extern int hackGlobalSpawnedBitsForThisGuy;
extern int hackGlobalGibCalls;

void Dekker_split(edict_t *self, edict_t *other, edict_t *activator)
{
	// get Dekker with MPG
	edict_t	*dekker = G_Find (NULL, FOFS(classname), "m_x_mraiderboss2");
	if (dekker == NULL) // This isn't right!!!  Where is everyone?
	{
		return;
	}
	else
	{

		ai_public_c* aipub = dekker->ai;
		ai_c * ai;
		ai = (ai_c*)aipub; // sigh, is this really necessary?

		if (ai)
		{
			bodyhuman_c *dekkerBody = NULL;
			if(dekkerBody = (bodyhuman_c *)ai->GetBody())
			{
				gz_info		*frontHole=NULL;
				gz_info		*backHole=NULL;
				gz_code		frontCode = 0;
				gz_code		backCode = 0;
				vec3_t		dir = { 1, 0, 0 };

				dekkerBody->NextShotsGonnaKillMe(NEXTSHOT_KILLS);

				T_Damage(dekker, self, self, dir, dekker->s.origin, dekker->s.origin, 1000, 0, 0, MOD_CRUSH);

				dekkerBody->AssignFrontAndBackGZones(&frontHole, &frontCode, &backHole, &backCode);
				hackGlobalSpawnedBitsForThisGuy=0;
				hackGlobalGibCalls=0;
				dekkerBody->SeverRoutine(*dekker, dir, frontCode, backCode, frontHole, backHole, true);
			}
		}			
	}
}

void SP_func_Dekker_split(edict_t* self)
{
	self->movetype = MOVETYPE_NONE;
	self->solid=SOLID_NOT;
	VectorSet(self->mins,-4,-4,-4);
	VectorSet(self->maxs,4,4,4);

	self->use = Dekker_split;
}

/*QUAKED func_door_portcullis (0 .5 .8) ? x x CRUSHER 
can only be opened by a trigger_keep_using.

"message"	is printed when the door is touched if it is a trigger door and it hasn't been fired yet
"sp_message" is for a string package ID (numerical)
"angle"		determines the opening direction
"targetname" needs a name so the trigger_keep_using can open it
"wait"		wait before returning (3 default, -1 = never return)
"lip"		lip remaining at end of move (8 default)
"dmg"		damage to inflict when blocked (2 default)
"soundName"	- name of the sound type for the door

ch-ch-ch-ch-changes...
"speed"		UPWARD movement speed (20 default)
"volume"	DOWNWARD movement speed (10 default)
*/


void portcullis_use (edict_t *self, edict_t *other, edict_t *activator)
{
	self->last_move_time = level.time;
	self->count = 1;
}


void SP_func_door_portcullis(edict_t *ent)
{
	vec3_t	abs_movedir;

	G_SetMovedir (ent->s.angles, ent->movedir);
	ent->movetype = MOVETYPE_PUSH;
	ent->solid = SOLID_BSP;
	gi.setmodel (ent, ent->model);

	ent->blocked = door_blocked;
	ent->use = portcullis_use;

//	player can never use the door directly
//
//	if ((!ent->targetname) || (ent->spawnflags & DOOR_USE_TARGET))
//	{
//		ent->plUse = plDoorUse;
//		ent->last_move_time = level.time - 10.0;//safe for monsters to use me...
//	}

	if (!ent->speed)
		ent->speed = 20;
	if (!ent->volume)
		ent->volume = 10;
	if (dm->isDM())
		ent->speed *= 2;

	if (!ent->accel)
		ent->accel = ent->speed;
	if (!ent->decel)
		ent->decel = ent->speed;

	if (!st.lip)
		st.lip = 8;
	if (!ent->dmg)
		ent->dmg = 2;

	// calculate second position
	VectorCopy (ent->s.origin, ent->pos1);
	abs_movedir[0] = fabs(ent->movedir[0]);
	abs_movedir[1] = fabs(ent->movedir[1]);
	abs_movedir[2] = fabs(ent->movedir[2]);
	ent->moveinfo.distance = abs_movedir[0] * ent->size[0] + abs_movedir[1] * ent->size[1] + abs_movedir[2] * ent->size[2] - st.lip;
	VectorMA (ent->pos1, ent->moveinfo.distance, ent->movedir, ent->pos2);

	// if it starts open, switch the positions
	ent->DoorIsOpen = false;
	if (ent->spawnflags & DOOR_START_OPEN)
	{
		VectorCopy (ent->pos2, ent->s.origin);
		VectorCopy (ent->pos1, ent->pos2);
		VectorCopy (ent->s.origin, ent->pos1);
		ent->DoorIsOpen = true;
	}

	ent->count = 0; // indicates we're not currently being used by a trigger_keep_using
	ent->wait = 0;	// having a wait would be bad
	ent->moveinfo.state = STATE_BOTTOM;

//	if (ent->health)
//	{
//		ent->takedamage = DAMAGE_YES;
//		ent->die = door_killed;
//		ent->max_health = ent->health;
//	}
//	else
	if (ent->targetname && (ent->message || ent->sp_message))
	{
		gi.soundindex ("misc/talk.wav");
		ent->touch = door_touch;
	}
	
	ent->moveinfo.speed = ent->speed;
	ent->moveinfo.accel = ent->accel;
	ent->moveinfo.decel = ent->decel;
	ent->moveinfo.wait = ent->wait;
	VectorCopy (ent->pos1, ent->moveinfo.start_origin);
	VectorCopy (ent->s.angles, ent->moveinfo.start_angles);
	VectorCopy (ent->pos2, ent->moveinfo.end_origin);
	VectorCopy (ent->s.angles, ent->moveinfo.end_angles);

//	if (ent->spawnflags & 16)
//		ent->s.effects |= EF_ANIM_ALL;
//	if (ent->spawnflags & 64)
//		ent->s.effects |= EF_ANIM_ALLFAST;

	// to simplify logic elsewhere, make non-teamed doors into a team of one
	if (!ent->team)
		ent->teammaster = ent;

	gi.linkentity (ent);

	ent->think = door_spawn_areaportal_think;
	ent->nextthink = level.time + FRAMETIME;

}


/*QUAKED func_hide_the_players_gun (0 1 0) (-4 -4 -4) (4 4 4)
health -- 1 if you're disguised, 0 if you're not
if you set wait to -1 then use, count will indicate if we're currently disguised, 1 == disguised, 0 == not
*/

void f_HtPG_use (edict_t *self, edict_t *other, edict_t *activator)
{//NICE!
	if(level.sight_client)
	{
		if(level.sight_client->client)
		{
			if(level.sight_client->client->inv)
			{
				sharedEdict_t	sh;
				sh.inv = (inven_c *)level.sight_client->client->inv;
				sh.edict = level.sight_client;

				sh.inv->setOwner(&sh);

				// kef -- if wait == -1 we're querying for our disguise state, return it via
				// count == 0		not disguised
				// count == 1		disguised
				if (self->wait == -1)
				{
					self->count = sh.inv->inDisguise()?1:0;
					self->wait = 0;
				}
				else
				{
					sh.inv->becomeDisguised();
					self->health = sh.inv->inDisguise()?1:0;
				}
			}
		}
	}
}


void SP_func_hide_the_players_gun(edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid=SOLID_NOT;
	VectorSet(ent->mins,-4,-4,-4);
	VectorSet(ent->maxs,4,4,4);

	// kef -- John assures me that the client's inv is set up before edicts upon loading a saved game, so
	//we _should_ have a valid concept of disguisedness at this time
//	if(level.sight_client && level.sight_client->client && level.sight_client->client->inv)
	if (g_edicts[1].client && g_edicts[1].client->inv)
	{
		sharedEdict_t	sh;
		sh.inv = (inven_c *)g_edicts[1].client->inv;
		sh.edict = &g_edicts[1];

		sh.inv->setOwner(&sh);
		ent->health = sh.inv->inDisguise()?1:0;
	}
	else
	{ // just...heh...assume we're not disguised
		ent->health = 0;
	}
	ent->use=f_HtPG_use;

	gi.linkentity(ent);
}

void use_snipercam_zoom(edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->wait == -1)
	{	// reset to normal fov. I'm sure I shouldn't hardcode this.
		g_edicts[1].client->ps.fov = 95;
		if (ei_show->value)
		{
			Com_Printf("new fov = %d\n", (int)g_edicts[1].client->ps.fov);
		}
		return;
	}

	g_edicts[1].client->ps.fov = self->wait;
		
	if (g_edicts[1].client->ps.fov < 6)
	{
		g_edicts[1].client->ps.fov = 6;
	}

	if (g_edicts[1].client->ps.fov > 60)
	{
		g_edicts[1].client->ps.fov = 60;
	}
	if (ei_show->value)
	{
		Com_Printf("new fov = %d\n", (int)g_edicts[1].client->ps.fov);
	}
}

/*QUAKED func_snipercam_zoom (0 1 0) (-4 -4 -4) (4 4 4)

  ------------ KEYS ------------
  count		desired fov, clamped to [6,60]. -1 resets to normal fov. "use" to update.
*/

void SP_func_snipercam_zoom(edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid=SOLID_NOT;
	VectorSet(ent->mins,-4,-4,-4);
	VectorSet(ent->maxs,4,4,4);

	ent->use = use_snipercam_zoom;
	ent->count = -1;

	gi.linkentity(ent);
}

void use_mission_status(edict_t *self, edict_t *other, edict_t *activator)
{
	if (0 == strcmp("exitwarning_on", other->targetname))
	{
		level.missionStatus = MISSION_EXIT;
		return;
	}
	else if (0 == strcmp("exitwarning_off", other->targetname))
	{
		level.missionStatus = MISSION_NONE;
		return;
	}
	level.missionStatus = self->count;
}

/*QUAKED func_mission_status (0 1 0) (-4 -4 -4) (4 4 4)

  ------------ KEYS ------------
  count		actual status of the mission. 0 = nothing yet, 1 = success. "use" to update.


  ----------- NOTES ------------
  near the end of a level, set up 2 trigger_multiples to target this func_mission_status.
  one of the triggers should have a targetname of "exitwarning_on", the other should be
  called "exitwarning_off". place "exitwarning_off" a short distance from the exit. place
  "exitwarning_on" in between "exitwarning_off" and the exit itself.
*/

void SP_func_mission_status(edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid=SOLID_NOT;
	VectorSet(ent->mins,-4,-4,-4);
	VectorSet(ent->maxs,4,4,4);

	ent->use = use_mission_status;

	gi.linkentity(ent);
}

void use_player_health(edict_t *self, edict_t *other, edict_t *activator)
{
	g_edicts[1].health += self->count;
	if (g_edicts[1].health > 100)
	{
		g_edicts[1].health = 100;
	}
	else if (g_edicts[1].health < 0)
	{
		g_edicts[1].health = 0;
	}
}

/*QUAKED func_player_health (0 1 0) (-4 -4 -4) (4 4 4)

  ------------ KEYS ------------
  count					add this to the player's health when this func_player_health is used
*/

void SP_func_player_health(edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid=SOLID_NOT;
	VectorSet(ent->mins,-4,-4,-4);
	VectorSet(ent->maxs,4,4,4);

	ent->use = use_player_health;

	gi.linkentity(ent);
}

void use_player_armor(edict_t *self, edict_t *other, edict_t *activator)
{
	g_edicts[1].client->inv->addArmor(self->count);
}

/*QUAKED func_player_armor (0 1 0) (-4 -4 -4) (4 4 4)

  ------------ KEYS ------------
  count					add this to the player's armor when this func_player_armor is used
*/

void SP_func_player_armor(edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid=SOLID_NOT;
	VectorSet(ent->mins,-4,-4,-4);
	VectorSet(ent->maxs,4,4,4);

	ent->use = use_player_armor;

	gi.linkentity(ent);
}

void use_player_item(edict_t *self, edict_t *other, edict_t *activator)
{
	sharedEdict_t	sh;

	sh.inv = (inven_c *)g_edicts[1].client->inv;

	if (!self->count)
	{
		gi.dprintf("hey! you can't add zero items to the player's inventory!\n");
		return;
	}

	if ( (self->health <= 0) || (self->health > 7) )
	{
		gi.dprintf("hey! adding an invalid item type to the player's inventory!\n");
		return;
	}

	if (!sh.inv)
	{
		gi.dprintf("hey! no inventory!\n");
		return;
	}

	sh.inv->addItem(self->health, self->count);
}

/*QUAKED func_player_item (0 1 0) (-4 -4 -4) (4 4 4)

  ------------ KEYS ------------
  count				number of items to be added to player's inventory when this func_player_item is used
  health			item to be added to player's inventory when this func_player_item is used

  	SFE_FLASHPACK		1
	SFE_C4				2
	SFE_LIGHT_GOGGLES	3
	SFE_MEDKIT			5
	SFE_GRENADE			6

*/

void SP_func_player_item(edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid=SOLID_NOT;
	VectorSet(ent->mins,-4,-4,-4);
	VectorSet(ent->maxs,4,4,4);

	ent->use = use_player_item;

	gi.linkentity(ent);
}

