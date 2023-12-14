#include "g_local.h"
#include "fields.h"
#include "strip.h"
#include "..\strings\singleplr.h"

extern float s_levelStatusBegin;	// declared in g_cmds.cpp

//  trigger_...
void SP_trigger_activate (edict_t *ent);
void SP_trigger_always (edict_t *ent);
void SP_trigger_countdown (edict_t *ent);
void SP_trigger_counter (edict_t *ent);
void SP_trigger_elevator (edict_t *ent);
void SP_trigger_gravity (edict_t *ent);
void SP_trigger_hurt (edict_t *ent);
void SP_trigger_key (edict_t *ent);
void SP_trigger_monsterjump (edict_t *ent);
void SP_trigger_multiple (edict_t *ent);
void SP_trigger_once (edict_t *ent);
void SP_trigger_push (edict_t *ent);
void SP_trigger_relay (edict_t *ent);
void SP_trigger_useable (edict_t *ent);
void SP_trigger_heal (edict_t *ent);
void SP_trigger_movedown (edict_t *ent);
void SP_trigger_keep_using (edict_t *ent);
void SP_trigger_safezone (edict_t *ent);
void SP_trigger_sound (edict_t *ent);
void SP_trigger_objectives (edict_t *ent);

spawn_t	triggerSpawns[] =
{
	//  trigger..
	{"trigger_activate",				SP_trigger_activate},
	{"trigger_always",					SP_trigger_always},
	{"trigger_countdown",				SP_trigger_countdown},
	{"trigger_counter",					SP_trigger_counter},
	{"trigger_elevator",				SP_trigger_elevator},
	{"trigger_gravity",					SP_trigger_gravity},
	{"trigger_hurt",					SP_trigger_hurt},
	{"trigger_key",						SP_trigger_key},
	{"trigger_monsterjump",				SP_trigger_monsterjump},
	{"trigger_multiple",				SP_trigger_multiple},
	{"trigger_once",					SP_trigger_once},
	{"trigger_push",					SP_trigger_push},
	{"trigger_relay",					SP_trigger_relay},
	{"trigger_useable",					SP_trigger_useable},
	{"trigger_heal",					SP_trigger_heal},
	{"trigger_movedown",				SP_trigger_movedown},
	{"trigger_keep_using",				SP_trigger_keep_using},
	{"trigger_safezone",				SP_trigger_safezone},
	{"trigger_sound",					SP_trigger_sound},
	{"trigger_objectives",				SP_trigger_objectives},
	{NULL,								NULL},
};

#define			TRIG_START_OFF			1

#define			TRIG_MULT_TRIGGERED		4
#define			TRIG_MULT_USEKEY		8

#define			TRIG_CNTDWN_ANYUSE		2
#define			TRIG_CNTDWN_REUSEABLE	4

#define			TRIG_HURT_IGNORE_ARMOR	64

#define			TRIGGER_HEAL_DEFAULT	25

void InitTrigger (edict_t *self)
{
	if (!VectorCompare (self->s.angles, vec3_origin))
		G_SetMovedir (self->s.angles, self->movedir);

	self->solid = SOLID_TRIGGER;
	self->movetype = MOVETYPE_NONE;
	gi.setmodel (self, self->model);
	self->svflags = SVF_NOCLIENT;
}


// the wait time has passed, so set back up for another activation
void multi_wait (edict_t *ent)
{
	ent->nextthink = 0;
}


// the trigger was just activated
// ent->activator should be set to the activator so it can be held through a delay
// so wait for the delay time before firing
void multi_trigger (edict_t *ent)
{
	if (ent->nextthink)
		return;		// already been triggered

	G_UseTargets (ent, ent->activator);

	if (ent->wait > 0)	
	{
		ent->think = multi_wait;
		ent->nextthink = level.time + ent->wait;
	}
	else
	{	// we can't just remove (self) here, because this is a touch function
		// called while looping through area links...
		ent->touch = NULL;
		ent->nextthink = level.time + FRAMETIME;
		ent->think = G_FreeEdict;
	}
}

void Use_Multi (edict_t *ent, edict_t *other, edict_t *activator)
{
	ent->last_move_time = level.time;
	ent->activator = activator;
	multi_trigger (ent);
}

void Touch_Multi (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if(other->client || (other->flags & FL_LEAN_PLAYER))
	{
		if (self->spawnflags & 2)
			return;
	}
	else if (other->svflags & SVF_MONSTER)
	{
		if (!(self->spawnflags & 1))
			return;
	}
	else
		return;

	if (!VectorCompare(self->movedir, vec3_origin))
	{
		vec3_t	forward;

		AngleVectors(other->s.angles, forward, NULL, NULL);
		if (DotProduct(forward, self->movedir) < 0)
			return;
	}

	self->activator = other;
	multi_trigger (self);
}

/*QUAKED trigger_multiple (.5 .5 .5) ? MONSTER NOT_PLAYER TRIGGERED USEKEY
Variable sized repeatable trigger.  Must be targeted at one or more entities.
If "delay" is set, the trigger waits some time after activating before firing.
"wait" : Seconds between triggerings. (.2 default)
sounds
1)the "talk" beep
2)the "talk" beep 
3)the "talk" beep

set "message" to text string
"sp_message" is for a string package ID (numerical)

If they are NOT targeted, then there are three flags you need to be aware of:

The MONSTER flag must be set if you want a monster to be able to activate the trigger by walking into it.

The NO_PLAYER flag must be set if you do NOT want the player to be able to activate the
 trigger by walking into it.

The TRIGGERED flag allows a trigger to be disabled until something else activates it.
After this activation, the trigger will be activated when something (depending on how
the first two flags are set) steps into it.  Since this flag requires the trigger to 
have a targetname, this flag will obviously override the "I can't be activated by anything
but my targeter" condition once it has been triggered.
*/
void trigger_enable (edict_t *self, edict_t *other, edict_t *activator)
{
	self->solid = SOLID_TRIGGER;
	self->use = Use_Multi;
	if (!(self->spawnflags & TRIG_MULT_USEKEY))
	{
		self->touch = Touch_Multi;
	}
	gi.linkentity (self);
}

void SP_trigger_multiple (edict_t *ent)
{
/*	if (ent->sounds == 1)
		ent->noise_index = gi.soundindex ("misc/secret.wav");
	else */
	if ((ent->sounds >= 1) && (ent->sounds <= 3))
		ent->noise_index = gi.soundindex ("misc/talk.wav");
/*	else if (ent->sounds == 3)
		ent->noise_index = gi.soundindex ("misc/trigger1.wav");
*/	
	if (!ent->wait)
		ent->wait = 0.2;

// Commented out because the only time it's going to have a targetname is if it's TRIG_MULT_TRIGGERED
//	if (!ent->targetname)
//	{
		if(ent->spawnflags & TRIG_MULT_USEKEY)
		{
			ent->plUse = Use_Multi;
			ent->last_move_time = level.time - 10.0;//safe for monsters to use me...
			ent->touch = NULL;
		}
		else
		{
			ent->touch = Touch_Multi;
		}
//	}

	ent->movetype = MOVETYPE_NONE;
	ent->svflags |= SVF_NOCLIENT;


	if (ent->spawnflags & TRIG_MULT_TRIGGERED)
	{
		ent->solid = SOLID_NOT;
		ent->use = trigger_enable;
	}
	else
	{
		ent->solid = SOLID_TRIGGER;
		ent->use = Use_Multi;
	}

	if (!VectorCompare(ent->s.angles, vec3_origin))
		G_SetMovedir (ent->s.angles, ent->movedir);

	gi.setmodel (ent, ent->model);
	gi.linkentity (ent);
}


/*QUAKED trigger_once (.5 .5 .5) ? MONSTER NOT_PLAYER  TRIGGERED USEKEY
Triggers once, then removes itself.
You must set the key "target" to the name of another object in the level that has a matching "targetname".

The MONSTER flag must be set if you want a monster to be able to activate the trigger by walking into it.

The NO_PLAYER flag must be set if you do NOT want the player to be able to activate the
 trigger by walking into it.

If TRIGGERED, this trigger must be triggered before it is live.

sounds
 1)	secret
 2)	beep beep
 3)	large switch
 4)

"message"	string to be displayed when triggered
"sp_message" is for a string package ID (numerical)
*/

void SP_trigger_once(edict_t *ent)
{
	// make old maps work because I messed up on flag assignments here
	// triggered was on bit 1 when it should have been on bit 4
/*	if (ent->spawnflags & 1)
	{
		vec3_t	v;

		VectorMA (ent->mins, 0.5, ent->size, v);
		ent->spawnflags &= ~1;
		ent->spawnflags |= 4;
		gi.dprintf("fixed TRIGGERED flag on %s at %s\n", ent->classname, vtos(v));
	}
*/
	ent->wait = -1;
	SP_trigger_multiple (ent);
}

/*QUAKED trigger_relay (.5 .5 .5) (-8 -8 -8) (8 8 8)
This fixed size trigger cannot be touched, it can only be fired by other events.
*/
void trigger_relay_use (edict_t *self, edict_t *other, edict_t *activator)
{
	G_UseTargets (self, activator);
}

void SP_trigger_relay (edict_t *self)
{
	self->use = trigger_relay_use;
}


/*
==============================================================================

trigger_key

==============================================================================
*/

/*QUAKED trigger_key (.5 .5 .5) (-8 -8 -8) (8 8 8)
A relay trigger that only fires it's targets if player has the proper key.
Use "item" to specify the required key, for example "key_data_cd"
*/
void trigger_key_use (edict_t *self, edict_t *other, edict_t *activator)
{
//	int			index;

//	if (!self->item)
		return;
/*	if (!activator->client)
		return;

	index = ITEM_INDEX(self->item);
	if (!activator->client->pers.inventory[index])
	{
		if (level.time < self->touch_debounce_time)
			return;
		self->touch_debounce_time = level.time + 5.0;
		gi.centerprintf (activator, "You need the %s", self->item->pickup_name);
		gi.sound (activator, CHAN_AUTO, gi.soundindex ("misc/keytry.wav"), .6, ATTN_NORM, 0);
		return;
	}

	gi.sound (activator, CHAN_AUTO, gi.soundindex ("misc/keyuse.wav"), .6, ATTN_NORM, 0);
	if (coop->value)
	{
		int		player;
		edict_t	*ent;

		for (player = 1; player <= game.maxclients; player++)
		{
			ent = &g_edicts[player];
			if (!ent->inuse)
				continue;
			if (!ent->client)
				continue;
			ent->client->pers.inventory[index] = 0;
		}
	}
	else
	{
		activator->client->pers.inventory[index]--;
	}

	G_UseTargets (self, activator);

	self->use = NULL;*/
}

void SP_trigger_key (edict_t *self)
{
	if (!st.item)
	{
		gi.dprintf("no key item for trigger_key at %s\n", vtos(self->s.origin));
		return;
	}
//	self->item = FindItemByClassname (st.item);

/*	if (!self->item)
	{
		gi.dprintf("item %s not found for trigger_key at %s\n", st.item, vtos(self->s.origin));
		return;
	}*/

	if (!self->target)
	{
		gi.dprintf("%s at %s has no target\n", self->classname, vtos(self->s.origin));
		return;
	}

	gi.soundindex ("misc/keytry.wav");
	gi.soundindex ("misc/keyuse.wav");

	self->use = trigger_key_use;
}


/*
==============================================================================

trigger_counter

==============================================================================
*/

/*QUAKED trigger_counter (.5 .5 .5) ? nomessage
Acts as an intermediary for an action that takes multiple inputs.

If nomessage is not set, t will print "1 more.. " etc when triggered and "sequence complete" when finished.

After the counter has been triggered "count" times (default 2), it will fire all of it's targets and remove itself.
*/

void trigger_counter_use(edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->count == 0)
		return;
	
	self->count--;

	if (self->count)
	{
		if (! (self->spawnflags & 1))
		{
			gi.SP_Print(activator, SINGLEPLR_MORE_TO_GO, self->count);
			gi.sound (activator, CHAN_AUTO, gi.soundindex ("Misc/Talk.wav"), .6, ATTN_NORM, 0);
		}
		return;
	}
	
	if (! (self->spawnflags & 1))
	{
		gi.SP_Print(activator, SINGLEPLR_SEQUENCE_COMPLETED);
		gi.sound (activator, CHAN_AUTO, gi.soundindex ("Misc/Talk.wav"), .6, ATTN_NORM, 0);
	}
	self->activator = activator;
	multi_trigger (self);
}

void SP_trigger_counter (edict_t *self)
{
	self->wait = -1;
	if (!self->count)
		self->count = 2;

	self->use = trigger_counter_use;
}


/*
==============================================================================

trigger_countdown

==============================================================================
*/

void trigger_countdown_use (edict_t *self, edict_t *other, edict_t *activator);

void trigger_countdown_remove (edict_t *self)
{
	G_UseTargets(self, self->activator);
	G_FreeEdict (self);
}

void trigger_countdown_think(edict_t *self)
{
	// send ( start time + countdown length - current time )
	float fTimeRemaining = self->elasticity + self->count - level.time;

	gi.WriteByte(svc_countdown);
	if (fTimeRemaining <= 0.0f)
	{	// finished counting down
		fTimeRemaining = 0.0f;
		gi.WriteLong(0);	// if I need to, maybe send a byte code after this zero to indicate
							//which message to display. right now it just says, "mission failed"
		gi.multicast(self->s.origin, MULTICAST_ALL);

		if (!(self->spawnflags & TRIG_CNTDWN_REUSEABLE))
		{
			// set a level-wide variable here to let the client know we need to display a message
			level.countdownEnded = 1;
			trigger_countdown_remove(self);
		}
		else
		{
			G_UseTargets(self, self->activator);
			trigger_countdown_use(self, self, self);
		}
	}
	else
	{	// still counting
		gi.WriteLong((int)fTimeRemaining);
		gi.multicast(self->s.origin, MULTICAST_ALL);
		self->nextthink = level.time + 1;
	}
}

void trigger_countdown_use (edict_t *self, edict_t *other, edict_t *activator)
{
	int count = 0;

	if ((other && other->client) || (self->spawnflags & TRIG_CNTDWN_ANYUSE))
	{
		if (self->think)
		{	// if we've already set a think function, that means we're now toggling it off
			self->elasticity = 0;
			// clean up and remove the trigger
			gi.WriteByte(svc_countdown);
			gi.WriteLong(0);
			gi.multicast(self->s.origin, MULTICAST_ALL);

			if (!(self->spawnflags & TRIG_CNTDWN_REUSEABLE))
			{
				self->count = 0;
				G_FreeEdict (self);
			}
			else
			{
				self->solid = SOLID_TRIGGER;
				self->think = NULL;
			}
			return;
		}
		self->nextthink = level.time + 1;
		// save start time
		self->elasticity = level.time;

		count = (int)self->count;

		gi.WriteByte(svc_countdown);
		gi.WriteLong(count);
		gi.multicast(other->s.origin, MULTICAST_ALL);

		self->activator = activator;
		self->solid = SOLID_NOT; 
		self->think = trigger_countdown_think;
		self->nextthink = level.time + 1;
	}
}

void trigger_countdown_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if ((other->client) || (self->spawnflags & TRIG_CNTDWN_ANYUSE))
	{
		trigger_countdown_use(self,other,other);
	}
}

/*QUAKED trigger_countdown (.5 .5 .5) ? START_OFF ANY_USE REUSEABLE
Produces a timer on screen that counts backwards.  When done it fires all of its targets and removes itself. Is activated by touch or use.
------ KEYS ------
count - # to start counting backwards from (default 10, can be MUCH bigger than 255)
------ SPAWNFLAGS ------
START_OFF - trigger is turned off so it can't be touched (use trigger_activate to make 'touchable')
*/
void SP_trigger_countdown (edict_t *self)
{
	if (!self->count)
		self->count = 10;

	InitTrigger (self);

	if (self->spawnflags & TRIG_START_OFF)
	{	
		self->solid = SOLID_NOT;
	}
	else
	{
		self->solid = SOLID_TRIGGER;
	}

	self->touch = trigger_countdown_touch;
	self->use = trigger_countdown_use;
	self->think = NULL;		// this needs to be NULL until we actually use the trigger

	gi.linkentity (self);

}
/*
==============================================================================

trigger_always

==============================================================================
*/

/*QUAKED trigger_always (.5 .5 .5) (-8 -8 -8) (8 8 8)
This trigger will always fire.  It is activated by the world.
*/
void SP_trigger_always (edict_t *ent)
{
	// we must have some delay to make sure our use targets are present
	if (ent->delay < 0.2)
		ent->delay = 0.2;
	G_UseTargets(ent, ent);
}


/*
==============================================================================

trigger_activate

==============================================================================
*/
void trigger_activate_use (edict_t *self, edict_t *other, edict_t *activator)
{
	edict_t *t;

	if (self->target)
	{
		t = NULL;
		while ((t = G_Find (t, FOFS(targetname), self->target)))
		{
			if (t == self)
			{
				gi.dprintf ("WARNING: Entity used itself.\n");
			}
			else
			{
				if (t->solid == SOLID_NOT)
				{
					t->solid = SOLID_TRIGGER;
				}
				else
				{
					t->solid = SOLID_NOT;
				}

				gi.linkentity (t);

			}

			if (!self->inuse)
			{
				gi.dprintf("entity was removed while using targets\n");
				return;
			}
		}
	}

}


/*QUAKED trigger_activate (.5 .5 .5) ?
Will make a trigger active if it is inactive, or inactive if it's active
*/
void SP_trigger_activate (edict_t *ent)
{
	InitTrigger (ent);

	ent->use = trigger_activate_use;

	gi.linkentity (ent);
}

/*
==============================================================================

trigger_push

==============================================================================
*/

// kef -- defining this is here is _so_ not good. now we've got to define any other flags
//for this trigger right here also, in stark defiance of the ones at the top of this file.
#define PUSH_ONCE		1
#define PUSH_START_OFF	2

void trigger_push_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if (self->spawnflags & PUSH_START_OFF)
	{	// if this trigger hasn't been 'used' yet, it hasn't been turned on so it shouldn't push
		if (0 == (self->count % 2))
		{
			return;
		}
	}
	if (strcmp(other->classname, "grenade") == 0)
	{
		VectorScale (self->movedir, self->speed * 10, other->velocity);
	}
	else if (other->health > 0)
	{
		// kef -- I should have my programming license revoked for this but I'm afraid of
		//using a more general check for fear of breaking something else
		if (other->targetname && (0 == strcmp("sib2_heli",other->targetname)))
		{
			return;
		}
		VectorScale (self->movedir, self->speed * 10, other->velocity);

		if (other->client)
		{
			// don't take falling damage immediately from this
			VectorCopy (other->velocity, other->client->oldvelocity);
			if (other->fly_sound_debounce_time < level.time)
			{
				other->fly_sound_debounce_time = level.time + 1.5;
			}
		}
	}
	if (self->spawnflags & PUSH_ONCE)
		G_FreeEdict (self);
}

// this should only be called if the trigger has the START_OFF spawnflag
void trigger_push_use(edict_t *self, edict_t *other, edict_t *activator)
{
	self->count++;
}

/*QUAKED trigger_push (.5 .5 .5) ? PUSH_ONCE START_OFF
Pushes the player
"speed"		defaults to 1000

PUSH_ONCE		you guessed it. only works once.
START_OFF		'use' this trigger to turn on the pushing. 'use' it again to turn it off. rinse. repeat.
*/
void SP_trigger_push (edict_t *self)
{
	InitTrigger (self);
	self->touch = trigger_push_touch;
	if (!self->speed)
		self->speed = 1000;

	if (self->spawnflags & PUSH_START_OFF)
	{
		self->use = trigger_push_use;
	}
	// count is used in conjunction with start_off
	self->count = 0;
	gi.linkentity (self);
}


/*
==============================================================================

trigger_hurt

==============================================================================
*/

#define PLAYER_ONLY	32

/*QUAKED trigger_hurt (.5 .5 .5) ? START_OFF x SILENT NO_PROTECTION SLOW PLAYER_ONLY IGNORE_ARMOR
Any entity that touches this will be hurt.
------ KEYS ------
"dmg"  - points of damage per server frame (default 5) whole numbers only
------ SPAWNFLAGS ------
START_OFF - won't fire until trigger_activate is used
x      - not in use
SILENT - does nothing yet
NO_PROTECTION - *nothing* stops the damage
SLOW   - changes the damage rate to once per second
PLAYER_ONLY	- trigger will only affect player
IGNORE_ARMOR - this damage ignores armor
*/
void hurt_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->solid == SOLID_NOT)
		self->solid = SOLID_TRIGGER;
	else
		self->solid = SOLID_NOT;

	gi.linkentity (self);

//	if (!(self->spawnflags & 2))
//		self->use = NULL;
}


void hurt_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	int		dflags;

	if ((!other->takedamage) || ((self->spawnflags & PLAYER_ONLY) && (!other->client)))
		return;

	if (self->spawnflags & 16)
	{
		if ((self->timestamp > level.time) && ((self->timestamp - level.time) != 1))
		{
			// can't hurt stuff this frame
			return;
		}
		else
		{
			self->timestamp = level.time + 1;
		}
	}
	else
		self->timestamp = level.time + FRAMETIME;

	if (!(self->spawnflags & 4))
	{
		if ((level.framenum % 10) == 0)
			gi.sound (other, CHAN_AUTO, self->noise_index, .6, ATTN_NORM, 0);
	}

	if (self->spawnflags & 8)
		dflags = DAMAGE_NO_PROTECTION;
	else
		dflags = 0;

	if (self->spawnflags & TRIG_HURT_IGNORE_ARMOR)
	{
		dflags |= DAMAGE_NO_ARMOR;
	}
	T_Damage (other, self, self, vec3_origin, other->s.origin, other->s.origin, self->dmg, self->dmg, dflags, MOD_TRIGGER_HURT);
}

void SP_trigger_hurt (edict_t *self)
{
	InitTrigger (self);

	self->touch = hurt_touch;

	if (!self->dmg)
		self->dmg = 5;

	if (self->spawnflags & TRIG_START_OFF)
		self->solid = SOLID_NOT;
	else
		self->solid = SOLID_TRIGGER;

//	if (self->spawnflags & 2)
	self->use = hurt_use;

	gi.linkentity (self);
}


/*
==============================================================================

trigger_gravity

==============================================================================
*/

/*QUAKED trigger_gravity (.5 .5 .5) ? 
Changes the touching entites gravity to
the value of "gravity".  1.0 is standard
gravity for the level.
*/

void trigger_gravity_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	other->gravity = self->gravity;
}

void SP_trigger_gravity (edict_t *self)
{
	if (st.gravity == 0)
	{
		gi.dprintf("trigger_gravity without gravity set at %s\n", vtos(self->s.origin));
		G_FreeEdict  (self);
		return;
	}

	InitTrigger (self);

	self->gravity = atoi(st.gravity);
	self->touch = trigger_gravity_touch;
}


/*
==============================================================================

trigger_monsterjump

==============================================================================
*/

/*QUAKED trigger_monsterjump (.5 .5 .5) ?
Walking monsters that touch this will jump in the direction of the trigger's angle
"speed" default to 200, the speed thrown forward
"height" default to 200, the speed thrown upwards
*/

void trigger_monsterjump_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if (other->flags & (FL_FLY | FL_SWIM) )
		return;
	if (other->svflags & SVF_DEADMONSTER)
		return;
	if ( !(other->svflags & SVF_MONSTER))
		return;

// set XY even if not on ground, so the jump will clear lips
	other->velocity[0] = self->movedir[0] * self->speed;
	other->velocity[1] = self->movedir[1] * self->speed;
	
	if (!other->groundentity)
		return;
	
	other->groundentity = NULL;
	other->velocity[2] = self->movedir[2];
}

void SP_trigger_monsterjump (edict_t *self)
{
	if (!self->speed)
		self->speed = 200;
	if (!st.height)
		st.height = 200;
	if (self->s.angles[YAW] == 0)
		self->s.angles[YAW] = 360;
	InitTrigger (self);
	self->touch = trigger_monsterjump_touch;
	self->movedir[2] = st.height;
}

/*
==============================================================================

trigger_useable

==============================================================================
*/

/*QUAKED trigger_useable (.5 .5 .5) ?
A trigger that fires its targets only when the player hits the use key. 
----------KEY---------
targetname -- name of a sound to be played when the trigger is used. make sure 
you put the ".wav" at the end
*/
void useable_use (edict_t *self,edict_t *other,edict_t *activator)
{
	self->last_move_time = level.time;
	G_UseTargets (self, activator);
	if (self->targetname && strstr(self->targetname,"wav"))
	{	// play a sound
//		gi.sound (self, CHAN_AUTO, gi.soundindex(self->targetname), .6, ATTN_NORM, 0);
		gi.sound (self, CHAN_AUTO, gi.soundindex (self->targetname), 1, ATTN_NORM, 0);//FIXME
	}
}

void SP_trigger_useable (edict_t *ent)
{
	InitTrigger (ent);
	ent->plUse = useable_use;
	ent->last_move_time = level.time - 10.0;//safe for monsters to use me...
	gi.linkentity (ent);
	// precache any sound we're supposed to play
	if (ent->targetname && strstr(ent->targetname,"wav"))
	{	
		gi.soundindex(ent->targetname);
	}
}

/*
==============================================================================

trigger_heal

==============================================================================
*/

/*QUAKED trigger_heal (.5 .5 .5) ?
A trigger that heals the user only when the player hits the use key. 
----------KEY---------
health -- the number of points to be healed (defaults to 15)
*/

qboolean tryToHeal(edict_t *ent,int amountToHeal);

void trigger_heal_spent (edict_t *self,edict_t *other,edict_t *activator)
{
	gi.sound (self, CHAN_AUTO, gi.soundindex ("misc/hlthstation/empty.wav"), 1, ATTN_NONE, 0);
}

void trigger_heal_use (edict_t *self,edict_t *other,edict_t *activator)
{
	if (tryToHeal(other, self->health))
	{
		gi.sound (self, CHAN_AUTO, gi.soundindex ("misc/hlthstation/health.wav"), 1, ATTN_NONE, 0);
		self->plUse = trigger_heal_spent;
		G_UseTargets (self, activator);
	}
	gi.sound (self, CHAN_AUTO, gi.soundindex ("misc/hlthstation/full.wav"), 1, ATTN_NONE, 0);
}

void SP_trigger_heal (edict_t *ent)
{
	InitTrigger (ent);
	ent->plUse = trigger_heal_use;
	if (!ent->health)
	{
		ent->health = TRIGGER_HEAL_DEFAULT;
	}
	gi.linkentity (ent);
	// precache any sound we're supposed to play
	gi.soundindex("misc/hlthstation/health.wav");
	gi.soundindex("misc/hlthstation/full.wav");
	gi.soundindex("misc/hlthstation/empty.wav");

}



/*
==============================================================================

trigger_movedown

==============================================================================
*/
/*QUAKED trigger_movedown (.5 .5 .5) ?
A trigger that forces thing downward.  Meant to be used in conjunction with pushkill brush.
*/
void trigger_movedown_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	other->velocity[2] = -200;
}

void SP_trigger_movedown (edict_t *ent)
{
	InitTrigger(ent);
	ent->touch = trigger_movedown_touch;
	gi.linkentity (ent);
}

/*QUAKED trigger_keep_using (.5 .5 .5) ?
A trigger that keeps firing as long as the player hits the use key and keeps touching it.
*/
void keep_using_use(edict_t *self,edict_t *other,edict_t *activator)
{
	if (other->client)
	{	// the player just used this trigger, so we're cool to start using our target
		self->count = 1;
	}
}

void keep_using_touch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{	// mark the last time the player touched this trigger (providing the player has used us)
	if (self->count)
	{
		self->last_move_time = level.time;
	}
}

void keep_using_think(edict_t *ent)
{	// if we've been used and the player hasn't stopped touching us since then, use our target.
	//if it's been more than two frames since our last touch, don't use our target right now.
	if ((level.time - ent->last_move_time) > (2 * FRAMETIME))
	{
		ent->count = 0;
	}
	else
	{	// use our target
		G_UseTargets (ent, ent);
	}
	ent->nextthink = level.time + FRAMETIME;
}

void SP_trigger_keep_using(edict_t *ent)
{
	InitTrigger (ent);
	ent->plUse = keep_using_use;
	ent->touch = keep_using_touch;
	ent->count = 0;
	ent->last_move_time = -10.0;
	ent->think = keep_using_think;
	ent->nextthink = level.time + FRAMETIME;
	gi.linkentity (ent);
}

/*
==============================================================================

trigger_safezone

==============================================================================
*/
/*QUAKED trigger_safezone (.5 .5 .5) ? START_OFF
Using this entity will toggle whether it hides players or not.
------ SPAWNFLAGS ------
START_OFF - won't hide anybody until trigger is used
*/

void safezone_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->solid == SOLID_NOT)
		self->solid = SOLID_TRIGGER;
	else
		self->solid = SOLID_NOT;

	gi.linkentity (self);
}


void safezone_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
//	if ((!other->takedamage) || ((self->spawnflags & PLAYER_ONLY) && (!other->client)))
//		return;

	if (!other->client)
		return;

//	if (self->timestamp > level.time)
//		return;

//	self->timestamp = level.time + FRAMETIME;

	other->svflags |= SVF_ISHIDING;
}

void SP_trigger_safezone (edict_t *ent)
{
	InitTrigger(ent);

	ent->touch = safezone_touch;

	if (ent->spawnflags & TRIG_START_OFF)
		ent->solid = SOLID_NOT;
	else
		ent->solid = SOLID_TRIGGER;

	ent->use = safezone_use;

	gi.linkentity (ent);
}

/*QUAKED trigger_sound (.5 .5 .5) (-4 -4 -4) (4 4 4)
Will trigger target when noisy things occur nearby (radius for activation is set by health (defaults to 128))
target will set what it should trigger, of course
*/

void SetGenericEffectInfo(edict_t *ent);

void trigger_sound_use(edict_t *self, edict_t *other, edict_t *activator)
{
	if(self->target)
	{
		G_UseTargets(self, self);
	}
	gmonster.RemoveSoundTrigger(self - g_edicts);
	G_FreeEdict(self);
}

void SP_trigger_sound (edict_t *ent)
{
	SetGenericEffectInfo(ent);//er

	if(ent->health == 0)
	{
		ent->health = 128;
	}
	ent->use = trigger_sound_use;
	ent->classname = "trigger_sound";

	gmonster.AddSoundTrigger(ent - g_edicts);

	ent->health *= ent->health;
}

/*QUAKED trigger_objectives (.5 .5 .5) ? START_OFF
Using this entity will toggle the trigger enabled/disabled. When enabled, the trigger will
display a "check your objectives screen" message.
------ SPAWNFLAGS ------
START_OFF - won't display anything until used
*/

void objectives_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->solid == SOLID_NOT)
	{
		self->solid = SOLID_TRIGGER;
	}
	else
	{
		self->solid = SOLID_NOT;
	}

	gi.linkentity (self);
}

void objectives_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if (!other->client)
	{
		return;
	}
	// display message
	level.missionStatus = MISSION_OBJECTIVES;
	s_levelStatusBegin = level.time;

	// once we show this message, we probably want this trigger to go away
	G_FreeEdict(self);
}

void SP_trigger_objectives (edict_t *ent)
{
	InitTrigger(ent);

	ent->use = objectives_use;

	if (ent->spawnflags & TRIG_START_OFF)
	{	// don't display a message yet
		ent->count = 0;
		ent->solid = SOLID_NOT;
	}
	else
	{
		ent->count = 1;
		ent->solid = SOLID_TRIGGER;
	}

	ent->touch = objectives_touch;

	gi.linkentity (ent);
}

