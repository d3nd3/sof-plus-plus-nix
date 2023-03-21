#include "g_local.h"
#include "../strings/items.h"
#include "p_body.h"
#include "w_weapons.h"
#include "dm_ctf.h"
#include "..\qcommon\ef_flags.h"
#include "..\qcommon\configstring.h"

#define HLTH_ITEM_PICKUP 10000


extern cvar_t	*weaponarena; // for weapon testing


//-------------
// Ammo.
//-------------

/*QUAKED item_ammo_pistol (0 .3 .3) (-21 -13 -6) (21 13 6) x x x START_CRATED
Ammo for pistol1 & machinepistol
For Deathmatch Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/

/*QUAKED item_ammo_shotgun (0 .3 .3) (-21 -13 -6) (21 13 6) x x x START_CRATED
Ammo for shotgun
For Deathmatch Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/

/*QUAKED item_ammo_pistol2 (0 .3 .3) (-21 -13 -6) (21 13 6) x x x START_CRATED
Ammo for pistol2 (the .44)
For Deathmatch Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/

/*QUAKED item_ammo_auto (0 .3 .3) (-21 -13 -6) (21 13 6) x x x START_CRATED
Ammo for assault rifle, machinegun, sniper rifle
For Deathmatch Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/


/*QUAKED item_ammo_battery (0 .3 .3) (-21 -13 -6) (21 13 6) x x x START_CRATED
Ammo for Microwave Pulse Gun (MPG)
For Deathmatch Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/

/*QUAKED item_ammo_gas (0 .3 .3) (-21 -13 -6) (21 13 6) x x x START_CRATED
Ammo for Flame Thrower
For Deathmatch Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/

/*QUAKED item_ammo_rocket (0 .3 .3) (-21 -13 -6) (21 13 6) x x x START_CRATED
Ammo for Rocket Launcher
For Deathmatch Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/

/*QUAKED item_ammo_slug (0 .3 .3) (-21 -13 -6) (21 13 6) x x x START_CRATED
Ammo for Slug Thrower
For Deathmatch Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/


//
// Single Player Ammo
//



/*QUAKED item_ammo_sp_pistol (0 .3 .3) (-6 -3 0) (6 3 5) x x x START_CRATED
Ammo for pistol1 & machinepistol
For SINGLE PLAYER Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/

/*QUAKED item_ammo_sp_shotgun (0 .3 .3) (-7 -9 0) (13 7 9) x x x START_CRATED
Ammo for shotgun
For SINGLE PLAYER Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/

/*QUAKED item_ammo_sp_pistol2 (0 .3 .3) (-6 -4 0) (6 4 6) x x x START_CRATED
Ammo for pistol2 (the .44)
For SINGLE PLAYER Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/

/*QUAKED item_ammo_sp_auto (0 .3 .3) (-9 -3 0) (12 3 6) x x x START_CRATED x x x RESPAWN
Ammo for assault rifle, machinegun, sniper rifle
For SINGLE PLAYER Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/

/*QUAKED item_ammo_sp_battery (0 .3 .3) (-5 -4 0) (5 4 10) x x x START_CRATED
Ammo for Microwave Pulse Gun (MPG)
For SINGLE PLAYER Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/

/*QUAKED item_ammo_sp_gas (0 .3 .3) (-5 -3 0) (5 3 13) x x x START_CRATED
Ammo for Flame Thrower
For SINGLE PLAYER Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/

/*QUAKED item_ammo_sp_rocket (0 .3 .3) (-2 -10 -1) (8 4 15) x x x START_CRATED x x x RESPAWN
Ammo for Rocket Launcher
For SINGLE PLAYER Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/

/*QUAKED item_ammo_sp_slug (0 .3 .3) (-5 -8 -1) (11 6 9) x x x START_CRATED
Ammo for Slug Thrower
For SINGLE PLAYER Only
START_CRATED -- smash open a crate to get at this pickup
------ KEYS ------
count - ammo value (default 1)
if count < 0, the ammo value will be a random number between 0 and count*-1
*/


//-------------
// Weapons.
//-------------

/*QUAKED item_weapon_pistol2 (0 .3 .3) (-16 -5 -2) (8 8 3) x x x START_CRATED
Weapon pickup for pistol2
If player already has weapon, ammo is given
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_weapon_pistol1 (0 .3 .3) (-12 -12 -2) (5 7 2) x x x START_CRATED
Weapon pickup for pistol1
If player already has weapon, ammo is given
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_weapon_machinepistol (0 .3 .3) (-15 -14 -3) (15 15 4) x x x START_CRATED
Weapon pickup for machine pistol
If player already has weapon, ammo is given
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_weapon_assault_rifle (0 .3 .3) (-16 -7 -2) (16 11 2) x x x START_CRATED
Weapon pickup for assault rifle
If player already has weapon, ammo is given
START_CRATED -- smash open a crate to get at this pickup
*/
/*QUAKED item_weapon_sniper_rifle (0 .3 .3) (-36 -8 -2) (36 16 3) x x x START_CRATED
Weapon pickup sniper rifle
If player already has weapon, ammo is given
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_weapon_autoshotgun (0 .3 .3) (-37 -10 -9) (25 11 9) x x x START_CRATED
Weapon pickup for auto shotgun
If player already has weapon, ammo is given
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_weapon_shotgun (0 .3 .3) (-28 -4 -3) (24 9 3) x x x START_CRATED
Weapon pickup for shotgun
If player already has weapon, ammo is given
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_weapon_machinegun (0 .3 .3) (-27 -11 -3) (38 7 6) x x x START_CRATED
Weapon pickup for machine gun
If player already has weapon, ammo is given
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_weapon_rocketlauncher (0 .3 .3) (-15 -13 -11) (38 17 11) x x x START_CRATED
Weapon pickup for rocket launcher
If player already has weapon, ammo is given
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_weapon_microwavepulse (0 .3 .3) (-22 -12 -9) (44 14 7) x x x START_CRATED
Weapon pickup for Microwave Pulse Gun (MPG)
If player already has weapon, ammo is given
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_weapon_flamethrower (0 .3 .3) (-20 -6 -3) (29 10 3) x x x START_CRATED
Weapon pickup for Flame Thrower
If player already has weapon, ammo is given
START_CRATED -- smash open a crate to get at this pickup
*/

//-------------
// Armor.
//-------------

/*QUAKED item_equip_armor (0 .3 .3) (-11 -9 -1) (11 9 5) x x x START_CRATED
count - armor value (default 50, max of 100)
if count < 0, the armor value will be a random number between 0 and count*-1
START_CRATED -- smash open a crate to get at this pickup
*/


//-------------
// Inventory items.
//-------------

/*QUAKED item_equip_flashpack (0 .3 .3) (-5 -6 -9) (5 6 9) x x x START_CRATED
START_CRATED -- smash open a crate to get at this pickup
*/

/* item_equip_neural_grenade (0 .3 .3) (-8 -8 -8) (8 8 8) x x x START_CRATED
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_equip_c4 (0 .3 .3) (-2 -12 -11) (16 12 13) x x x START_CRATED
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_equip_light_goggles (0 .3 .3) (-5 -7 -4) (5 2 1) x x x START_CRATED
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_equip_claymore (0 .3 .3) (-4 -13 -6) (3 11 8) x x x START_CRATED
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_equip_medkit (0 .3 .3) (-11 -8 -0) (11 9 10) x x x START_CRATED
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_equip_grenade (0 .3 .3) (-3 -4 0) (4 4 8) x x x START_CRATED
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_ctf_flag (0 .3 .3) (-4 -13 -6) (3 11 8) x x x START_CRATED
count - team counter - 1 or 2
START_CRATED -- smash open a crate to get at this pickup
*/

//-------------
// Health.
//-------------

/*QUAKED item_health_small (0 .3 .3) (-7 -7 0) (7 7 7) x x x START_CRATED
count - health restored on pickup (default 10)
START_CRATED -- smash open a crate to get at this pickup
*/

/*QUAKED item_health_large (0 .3 .3) (-18 -4 -0) (18 4 26) x x x START_CRATED
count - health restored on pickup (default 50)
START_CRATED -- smash open a crate to get at this pickup
*/



// 12/27/99 kef -- replaced itemSpawnInfo_t itemSpawns[] with PickupList thePickupList.
PickupList thePickupList;


/*
===============
G_CalcRespawnTime

I made this in case we wanna do some kinda dynamic type respawn timing. Should
be called from game main loop somewhere (i.e. once per frame). -MW
===============
*/

void G_CalcPickupRespawnTime(void)
{
	int count,i;
	
	for(count=0,i=0;i<game.maxclients;i++)
	{
		if((&g_edicts[1+i])->inuse)
			count++;
	}

	// FIXME: some kinda formula here using count. Then store result away in the
	// game structure.
	//game.PickupRespawnTime=30.0+((game.maxclients-count)*2.0);
}

/*
===============
itemAmmoTouch
===============
*/

void itemAmmoTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	Pickup		*pickup = thePickupList.GetPickupFromEdict(self);

	if(pickup && (other->client)&&(other->health>0))
	{
		int nAmount = self->count * game.playerSkills.getAmmoMod();

		if(pickup->GetPickupListIndex() == OBJ_AMMO_KNIFE)
		{
			// We're modifying the amount of ammo you recover by the current skill level
			// of the game, but that's a little weird when you _know_ you're only picking
			// up _one_ knife.

			nAmount=1;
		}

		if(other->client->inv->addAmmoType(pickup->GetSpawnParm(),nAmount))
		{
			thePickupList.HandleInfoEvent(PickupInfoList::event_PICKEDUP, self);
			gi.sound(other, CHAN_ITEM, gi.soundindex("weapons/gpistol/cock.wav"), 1.0, ATTN_IDLE, 0);	// LOUD --PAT
			gi.SP_Print(other, pickup->GetPickupStringIndex());

			// Respawn or delete this entity?

			if( ((deathmatch->value)&&(!(self->spawnflags&DROPPED_ITEM))) ||
				(self->spawnflags & SF_PICKUP_RESPAWN) )
			{
				dm->setRespawn(self,pickup->GetRespawnTime());
			}
			else
			{
				G_FreeEdict(self);
			}
		}
		else
		{
			if(level.time>self->touch_debounce_time)
			{
				thePickupList.HandleInfoEvent(PickupInfoList::event_TOUCHED, self);
				gi.SP_Print(other, pickup->GetItemFullStringIndex());
				self->touch_debounce_time = level.time + 5.0;
			}
		}
	}
}

/*
===============
itemWeaponTouch
===============
*/

void itemWeaponTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	Pickup		*pickup = thePickupList.GetPickupFromEdict(self);

	if(pickup && (other->client)&&(other->health>0))
	{
		// If the player touching me is the same player that *just* dropped me,
		// they ain't gonna want to pick me up again so soon.

		if((self->spawnflags&DROPPED_ITEM)&&(self->enemy&&(self->enemy==other))&&(level.time<self->touch_debounce_time))
			return;

		// Determine how much ammo is in the weapon's clip.

		int clipAmount=(self->spawnflags&DROPPED_ITEM)?self->health:-1;

		// What happens next depends on whether we already have this weapon type
		// in our inventory and whether we are encumbered.

		int hasWeapon=0;
		
		hasWeapon=other->client->inv->hasWeaponType(pickup->GetSpawnParm());

		int encumbered=0;
			
		if((dm->getMaxEncumbrance()>0)&&
		   ((other->client->inv->getEncumbrance()+other->client->inv->getEncumbranceByType(pickup->GetSpawnParm()))>dm->getMaxEncumbrance()))
		{
			encumbered=1;
		}

		if((!hasWeapon)&&(!encumbered))
		{
			thePickupList.HandleInfoEvent(PickupInfoList::event_PICKEDUP, self);
			if (clipAmount == CLIP_SHOULD_BE_FILLED) // this is a weapon that was dropped over 30 seconds ago in realistic DM,
			{										 // so we need to refill it.  It's ugly, yes...
				clipAmount = other->client->inv->getClipMaxByType(pickup->GetSpawnParm());
			}
			other->client->inv->addWeaponType(pickup->GetSpawnParm(), clipAmount);

			// Auto switch to this weapon if it's the best one?

			if(other->client->pers.bestweap[0]&&dm->allowAutoWeaponSwitch())
			{
				sharedEdict_t sh;

				sh.inv=(inven_c *)other->client->inv;
				sh.edict=other;
				sh.inv->setOwner(&sh);

				if(pickup->GetSpawnParm()==sh.inv->getBestWeaponType())
					other->client->inv->takeOutBestWeapon();	
			}

			// Sound and info display.

			gi.sound(other, CHAN_ITEM, gi.soundindex("weapons/gpistol/cock.wav"), 1.0, ATTN_IDLE, 0);	// LOUD --PAT
			gi.SP_Print(other, pickup->GetPickupStringIndex());

			if(dm->dmRule_WEAPONS_STAY()&&(!(self->spawnflags&DROPPED_ITEM)))
				return;

			// Respawn or delete this entity?

			if((deathmatch->value)&&(!(self->spawnflags&DROPPED_ITEM)))
			{
				dm->setRespawn(self,pickup->GetRespawnTime());
			}
			else
			{
				G_FreeEdict(self);
			}
		}
		else
		{
			if((!hasWeapon)&&encumbered)
			{
				if(level.time>self->touch_debounce_time)
				{
					// Couldn't carry another weapon.
					thePickupList.HandleInfoEvent(PickupInfoList::event_TOUCHED, self);

					// if we're in single player, we want to let the player take the ammo from the weapon,
					// even if he can't carry the weapon
					if (!deathmatch->value)
					{
						if(other->client->inv->
							stripAmmoFromGun(pickup->GetSpawnParm(),(self->spawnflags & DROPPED_ITEM) ? self->health:0))
						{
							thePickupList.HandleInfoEvent(PickupInfoList::event_PICKEDUP, self);

							if (self->health)
							{
								gi.sound(other, CHAN_ITEM, gi.soundindex("weapons/gpistol/cock.wav"), 1.0, ATTN_IDLE, 0);	// LOUD --PAT
								gi.SP_Print(other, pickup->GetAmmoPickupStringIndex());
								self->health = 0; // get rid of ammo in the now-empty weapon
							}
						}
					}

					gi.SP_Print(other,ITEMS_ENCUMBERED);
					self->touch_debounce_time = level.time + 5.0;
				}

				return;
			}
			
			// We can't take this weapon so try to take the ammo from it (we discard the weapon though).
			// Note: in DM, extra ammo can only be gained from dropped weapons if DF_WEAPONS_STAY is off.
			if((deathmatch->value)&&dm->dmRule_WEAPONS_STAY()&&(!(self->spawnflags&DROPPED_ITEM)))
			{
				// In DM, if DF_WEAPONS_STAY is on, extra ammo can only be gained
				// from dropped weapons.
				if (level.time > self->touch_debounce_time)
				{
					thePickupList.HandleInfoEvent(PickupInfoList::event_TOUCHED, self);
					self->touch_debounce_time = level.time + 5.0;
				}
				
				return;
			}
			// Double note: can't pick up duplicate weapons in realDM
			else if((dm->isDM()) && (deathmatch->value == DM_REAL))
			{
				if (level.time>self->touch_debounce_time)
				{
					thePickupList.HandleInfoEvent(PickupInfoList::event_TOUCHED, self);
					gi.SP_Print(other, ITEMS_ALREADY_HAVE_THIS);
					self->touch_debounce_time = level.time + 5.0;
					return;
				}
				else
				{
					return;
				}
			} 
			else
			{		
				// Ok, grab the ammo.

				if(other->client->inv->
					addAmmoByGunType(pickup->GetSpawnParm(),(self->spawnflags & DROPPED_ITEM) ? self->health:0))
				{
					// OK, added the ammo so we're outa here!
					thePickupList.HandleInfoEvent(PickupInfoList::event_PICKEDUP, self);

					gi.sound(other, CHAN_ITEM, gi.soundindex("weapons/gpistol/cock.wav"), 1.0, ATTN_IDLE, 0);	// LOUD --PAT
					gi.SP_Print(other, pickup->GetAmmoPickupStringIndex());

					if((deathmatch->value)&&(!(self->spawnflags&DROPPED_ITEM)))
					{
						// In DM, I will respawn if I'm not a dropped weapon.

						dm->setRespawn(self,pickup->GetRespawnTime());
					}
					else
					{
						G_FreeEdict(self);
					}
				}
				else
				{
					if(level.time>self->touch_debounce_time)
					{
						thePickupList.HandleInfoEvent(PickupInfoList::event_TOUCHED, self);
						gi.SP_Print(other, pickup->GetItemFullStringIndex());
						self->touch_debounce_time = level.time + 5.0;
					}
				}
			}
		}
	}
}

/*
===============
itemEquipTouch
===============
*/

void itemEquipTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	Pickup		*pickup = thePickupList.GetPickupFromEdict(self);

	if(pickup && (other->client)&&(other->health>0))
	{
		// If the player touching me is the same player that *just* dropped me,
		// they ain't gonna want to pick me up again so soon.

		if((self->spawnflags&DROPPED_ITEM)&&(self->enemy&&(self->enemy==other))&&(level.time<self->touch_debounce_time))
			return;

		// Determine how many of these / how much ammo.

		int amount=(self->spawnflags&DROPPED_ITEM)?self->health:self->count;

		// Try to add items(s).

		if(other->client->inv->addItem(pickup->GetSpawnParm(),amount,pickup->GetDefaultMaxCount()))
		{
			thePickupList.HandleInfoEvent(PickupInfoList::event_PICKEDUP, self);
			gi.sound(other, CHAN_ITEM, gi.soundindex("weapons/gpistol/cock.wav"), 1.0, ATTN_IDLE, 0);	// LOUD --PAT
			gi.SP_Print(other, pickup->GetPickupStringIndex());

			// Respawn or delete this entity?

			if( (deathmatch->value)&&(!(self->spawnflags&DROPPED_ITEM)) )
			{
				dm->setRespawn(self,pickup->GetRespawnTime());
			}
			else
			{
				G_FreeEdict(self);
			}
		}
		else
		{
			if(level.time>self->touch_debounce_time)
			{
				thePickupList.HandleInfoEvent(PickupInfoList::event_TOUCHED, self);
				gi.SP_Print(other, pickup->GetItemFullStringIndex());
				self->touch_debounce_time = level.time + 5.0;
			}
		}
	}
}

/*
===============
itemArmorTouch
===============
*/

void itemArmorTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	Pickup		*pickup = thePickupList.GetPickupFromEdict(self);

	if(pickup && (other->client)&&(other->health>0))
	{
		if(PB_AddArmor(other, self->count * game.playerSkills.getArmorMod()))
		{		
			thePickupList.HandleInfoEvent(PickupInfoList::event_PICKEDUP, self);
			gi.sound(other, CHAN_ITEM, gi.soundindex("dm/armorpu.wav"), 1.0, ATTN_IDLE, 0);	// LOUD --PAT
			gi.SP_Print(other, pickup->GetPickupStringIndex());
			
			if((deathmatch->value)&&(!(self->spawnflags&DROPPED_ITEM)))
			{
				dm->setRespawn(self,pickup->GetRespawnTime());
			}
			else
			{
				G_FreeEdict(self);
			}
		}
		else
		{
			if(level.time>self->touch_debounce_time)
			{
				thePickupList.HandleInfoEvent(PickupInfoList::event_TOUCHED, self);
				gi.SP_Print(other, ITEMS_ARMOR_ALREADY_FULL);
				self->touch_debounce_time = level.time + 5.0;
			}
		}
	}
}

int reset_flag(edict_t *self, edict_t *owner)
{
 	edict_t *home = NULL;
 	Matrix4 matPickup;
 	float newScale = 1.75f;
	char	*flagname;
	Pickup			*pickup = thePickupList.GetPickupFromSpawnName("ctf");

 	// determine which flag it is
 	if (self->ctf_flags == TEAM1)
 	{
 		flagname="ctf_flag_blue";
 	}
 	else
 	{
 		flagname="ctf_flag_red";
 	}

 	// if we are looking at ourselves, then we are at the flags original location, so exit
 	if (!strcmp(flagname, self->classname))
 	{
 		self->touch_debounce_time = level.time + 5.0;
		self->touch_debounce_owner = owner;
 		return 1;
 	}

 	// ok, this flag is not at home, so find the home edict for this entity
 	home = G_Find (home, FOFS(classname), flagname);
 	if (!home)
 		return 1;

 	// found it - now put a ghoul model of the flag at the home entity
	SimpleModelInit2(home,pickup->GetModelSpawnData(),flagname,NULL);

 	IGhoulInst *pickupInst = home->ghoulInst;

	if (pickupInst)
	{
		pickupInst->GetXForm(matPickup);
		matPickup.Scale(newScale);
		pickupInst->SetXForm(matPickup);

 		pickupInst->SetMyBolt("quake_ground");
 		home->mins[2] = 0;
 		pickupInst->SetFrameOverride("ctf_flag", flagname);
	}
 	// indicate to everyone that this entity has a flag attached to it.
 	home->ctf_flags = self->ctf_flags;
 	home->solid = SOLID_TRIGGER;
	home->s.renderfx |= RF_GHOUL_SHADOW;

	// indicate to all clients the state of the flags
	if (home->ctf_flags == TEAM1)
	{
		gi.configstring(CS_CTF_BLUE_STAT, "h");
	}
	else
	{
		gi.configstring(CS_CTF_RED_STAT, "h");
	}

	return 0;
}

/*
===============
itemFlagTouch
===============
*/
void itemFlagTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	Pickup		*pickup = thePickupList.GetPickupFromEdict(self);
	char		*flagname;
	edict_t	*e2;


	if(pickup && (other->client)&&(other->health>0)) 
	{
		// this type of shit is getting complicated, so I gave it a second If. If it's ineffiecient, sue me.
		if ((other != self->touch_debounce_owner) || (level.time > self->touch_debounce_time))
		{
			// only pick up the flag if it's not ours and it has a flag attached to it
			if (!(self->ctf_flags & other->client->resp.team))
			{
   				if(other->client->inv->addItem(pickup->GetSpawnParm(),1))
   				{
					char config[3];

					thePickupList.HandleInfoEvent(PickupInfoList::event_PICKEDUP, self);
					// indicate which flag we have
					other->ctf_flags = self->ctf_flags;
   					gi.sound(other, CHAN_ITEM, gi.soundindex("weapons/gpistol/cock.wav"), 1.0, ATTN_IDLE, 0);	// LOUD --PAT

					// remove the trigger from this object
					self->solid = SOLID_NOT;
					
					// firstly, remove the ghoul object from the flag spawn edict
					// be safe - shouldn't need this but better safe than sorry
					if (self->objInfo)
					{
						delete self->objInfo;
						self->objInfo = NULL;
					}
					game_ghoul.RemoveObjectInstances(self);
					self->ghoulInst = NULL;
					// have to do this or GHOUL complains
					self->s.renderfx &= ~(RF_GHOUL|RF_GHOUL_SHADOW);

					// delimit the string correctly
					config[0] = 'r';
					config[1] = other->s.number;
					config[2] = 0;
					// now add a flag to the player
					if (self->ctf_flags == TEAM1)
					{
						flagname = "ctf_flag_blue";
						gi.configstring(CS_CTF_BLUE_STAT, config);
						gi.SP_Print(NULL, DM_CTF_CAPTURE_FLAG_BLUE , other->s.number);
					}
					else
					{
						flagname = "ctf_flag_red";
						gi.configstring(CS_CTF_RED_STAT, config);
						gi.SP_Print(NULL, DM_CTF_CAPTURE_FLAG_RED , other->s.number);
					}

					// don't delete this edict, since we will need it to re-position the flag onto should we need to
					self->ctf_flags = 0;

					PB_AddFlag(other, flagname, "flag_hold_idle");

					// if we have a looping sound going already, don't interrupt it
					if (!(other->s.sound))
					{
						other->s.sound = gi.soundindex("player/flaglp.wav");
						other->s.sound_data = (255 & ENT_VOL_MASK) | SND_LOOPATTN;
					}

					// send out the sound appropriate to each client dependant on which team we are on
					for (int i=1; i<=game.maxclients;i++)
					{
						e2 = &g_edicts[i];
						if (!e2->inuse)
							continue;
								
						if (!e2->client)
							continue;

						//other wise, just a smaller bonus
						if (e2->client->resp.team == other->client->resp.team)
						{
							// triumphant
							gi.sound(e2,CHAN_VOICE,gi.soundindex("dm/ctf/triumphant.wav"),1.0,ATTN_NORM,0,SND_LOCALIZE_CLIENT);
						}
						else
						{
							// oh shit
							gi.sound(e2,CHAN_VOICE,gi.soundindex("dm/ctf/flaggrab.wav"),1.0,ATTN_NORM,0,SND_LOCALIZE_CLIENT);
						}
					}


					if (stricmp(flagname, self->classname))
					{
						// remove the flag edict if it's not a flag spawn edict
						G_FreeEdict(self);
					}

   				}
   				else
   				{
   					if(level.time>self->touch_debounce_time)
   					{
   						gi.SP_Print(other, pickup->GetItemFullStringIndex());
   						self->touch_debounce_time = level.time + 5.0;
   					}
   				}
			}

			// else, if its still an entity with a flag and its not at its base location, return it there
			else if (self->ctf_flags)
			{
				thePickupList.HandleInfoEvent(PickupInfoList::event_TOUCHED, self);

				if (reset_flag(self, other))
					return;

				// give ourselves a bonus for returning the flag
				other->client->resp.score += CTF_RECOVERY_BONUS;
				if (self->ctf_flags == TEAM1)
				{
					gi.SP_Print(NULL, DM_CTF_RECOVER_FLAG_BLUE , other->s.number, CTF_RECOVERY_BONUS);
				}
				else
				{
					gi.SP_Print(NULL, DM_CTF_RECOVER_FLAG_RED , other->s.number, CTF_RECOVERY_BONUS);
				}

				gi.sound(other,CHAN_NO_PHS_ADD,gi.soundindex("dm/ctf/flagret.wav"),1.0,ATTN_NONE,0,SND_LOCALIZE_GLOBAL);

				// remove the current edict entirely, ghoul model and all
				G_FreeEdict(self);
			}
		}
	}
}

/*
===============
itemCashTouch
===============
*/

void itemCashTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
/*	if((other->client)&&(other->health>0))
	{
		if(CPlayerStats *PlayerStats = GetPlayerStats(other))
		{
			PlayerStats->AdjustCashOnHand(self->count);

			gi.sound(other, CHAN_ITEM, gi.soundindex("weapons/gpistol/cock.wav"), .6, ATTN_NORMAL, 0);
			gi.cprintf(other,PRINT_MEDIUM,"Picked up %d %s\n", self->count,"dollars");

			if((deathmatch->value)&&(!(self->spawnflags&DROPPED_ITEM)))
				SetRespawn(self,30.0);	// FIXME: propper time delay?
			else
				G_FreeEdict(self);
		}
	}*/
}

/*
===============
tryToHeal

FIXME (kinda): This will be needed as a helper imported by the player dll.
===============
*/
extern	qboolean tryToHeal(edict_t *ent,int amountToHeal);


qboolean tryToHeal(edict_t *ent,int amountToHeal)
{
	int damagedHealth = ent->health;
	if(ent->health<ent->max_health)
	{
		ent->health+=amountToHeal;

		if(ent->health>ent->max_health)
			ent->health=ent->max_health;

		gi.sound(ent, CHAN_ITEM, gi.soundindex("dm/healthpu.wav"), 1.0, ATTN_IDLE, 0);	// LOUD --PAT

		PB_RepairSkin(ent, damagedHealth);
		ent->client->moveScale=0.0;
		
		return(true);
	}

	return(false);
}

/*
===============
itemHealthTouch
===============
*/

void itemHealthTouch(edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	Pickup		*pickup = thePickupList.GetPickupFromEdict(self);

	if(pickup && (other->client)&&(other->health>0))
	{
		if(tryToHeal(other,self->count))
		{
			gi.sound(other, CHAN_ITEM, gi.soundindex("dm/healthpu.wav"), 1.0, ATTN_IDLE, 0);	// LOUD --PAT
			gi.SP_Print(other, pickup->GetPickupStringIndex());

			thePickupList.HandleInfoEvent(PickupInfoList::event_PICKEDUP, self);
			if((dm->isDM())&&(!(self->spawnflags&DROPPED_ITEM)))
			{
				dm->setRespawn(self,pickup->GetRespawnTime());
			}
			else
			{
				G_FreeEdict(self);
			}
		}
		thePickupList.HandleInfoEvent(PickupInfoList::event_TOUCHED, self);
	}
}

/*
===============
I_Spawn
===============
*/

void I_Spawn(edict_t *ent, Pickup *pickup)
{
	char *skinname = NULL;

#ifndef _FINAL_
	if (thePickupList.WeaponArena(weaponarena->value))
	{	// the number stored in weaponarena->value is an objType_t, an index into thePickupList.
		//turn all pickups into the type represented by weaponarena->value.
		if ( ((weaponarena->value) > OBJ_NONE) && ((weaponarena->value) < MAX_ITM_OBJS) )
		{
			pickup = thePickupList.GetPickup(weaponarena->value);
		}
		else
		{
			// display help if value is invalid
			thePickupList.WeaponArenaHelp();
		}
	}
#endif // _FINAL_

	// Classname.

	ent->classname = pickup->GetSpawnName();

	// for some reason we get flags in a non dm game, so lets check them here, and deal with it
	if (!stricmp("item_ctf_flag", ent->classname) && !dm->isDM())
	{
		gi.dprintf ("No flags in normal singleplay\n");
		G_FreeEdict(ent);
		return;
	}

	// Ok, we want this item in the world, so go ahead and fill in the blanks.

	VectorSet (ent->mins,-8,-8,-8); // we'll adjust mins[2] after we scale the pickup
	VectorSet (ent->maxs,8,8,8);

	ent->spawnflags|=SF_INVULNERABLE;
	
	// If this is a thrown knife that just stuck in something, handle it differently.

	if (ent->elasticity=255)
		ent->elasticity=0;

	// kef -- don't change the edict's solidness after thePickupList.RegisterEdict() because it
	//may get set within that fn, like if the pickup starts off in a crate
	ent->solid = SOLID_TRIGGER;

	// Figure out the skiname and make it fullbright.

	skinname=pickup->GetSkin();

	// if we are a CTF Flag, decide which color it should be
	if (pickup->GetType() == PU_INV && pickup->GetPickupListIndex() == OBJ_CTF_FLAG)
	{
		if (ent->count == TEAM1)
		{
			skinname="ctf_flag_blue";
			gi.configstring(CS_CTF_BLUE_STAT, "h");
		}
		else
		{
			skinname="ctf_flag_red";
			gi.configstring(CS_CTF_RED_STAT, "h");
		}
		// make it so we can find these entities again should we want to relocate the flag
		ent->classname = skinname;
	}

	if(pickup->GetModelSpawnData() && pickup->GetModelSpawnData()->dir)
	{
		SimpleModelInit2(ent,pickup->GetModelSpawnData(),skinname,NULL);
		if ( (pickup->GetType() == PU_ARMOR) && (dm->isDM()) )
		{	// use special low-poly armor model
			SimpleModelSetSequence(ent, pickup->GetLowPolyModel()->file, SMSEQ_HOLD);
		}
		// c4 pickups aren't supposed to blink
		if (pickup->GetPickupListIndex() == OBJ_ITM_C4)
		{
			SimpleModelSetSequence(ent, pickup->GetModelSpawnData()->file, SMSEQ_HOLD);
		}
	}
	else
	{
		gi.dprintf("No model specified for %s, can't spawn\n", ent->classname);
	}

	// thePickupList already determined the scale for the pickups depending on type.

	IGhoulInst *pickupInst = ent->ghoulInst;

	if (pickupInst)
	{
		Matrix4 matPickup;
		vec3_t	adjMins = {0,0,0};
		float	newScale = pickup->GetScale();

		if (newScale != 1)
		{
			pickupInst->GetXForm(matPickup);
			matPickup.Scale(newScale);
			pickupInst->SetXForm(matPickup);
		}
		if (pickup->GetPickupListIndex() == OBJ_CTF_FLAG)
		{
			pickupInst->SetMyBolt("quake_ground");
			ent->mins[2] = 0;
			pickupInst->SetFrameOverride("ctf_flag", skinname);
			// indicate to everyone that this entity has a flag attached to it.
			ent->ctf_flags = ent->count;
			VectorSet (ent->mins,-12,-12,-10);
			VectorSet (ent->maxs,12,12,30);
			ent->s.renderfx |= RF_GHOUL_SHADOW;
		}
		VectorScale(ent->mins, newScale, ent->mins);
		VectorScale(ent->maxs, newScale, ent->maxs);
		pickup->GetBBoxAdjustMins(adjMins);
		ent->mins[2] = adjMins[2];
	}

	st.color[0]=255;
	st.color[1]=255;
	st.color[2]=255;

	// 12/28/99 kef -- this is going to go away very soon

	ent->dmg=(pickup->GetPickupListIndex());

	// Weapons should be precached.

	if(pickup->GetType() == PU_WEAPON)
	{
		AddWeaponTypeForPrecache(pickup->GetSpawnParm());
	}

	// What's this??
	// whatever it is, we don't want to do it if its a flag entity
	if (!(pickup->GetType() == PU_INV && pickup->GetPickupListIndex() == OBJ_CTF_FLAG))
	{
		if(ent->count==0)
			ent->count=pickup->GetDefaultCount();
		else if(ent->count<0)
			ent->count=gi.irand(0,ent->count*-1);
	}

	// Set up touch function.

	ent->touch_debounce_time=0.0;

	switch(pickup->GetType())
	{
		case PU_INV:
			if (pickup->GetPickupListIndex() == OBJ_CTF_FLAG)
			{
				ent->touch = itemFlagTouch;
			}
			else
			{
				ent->touch = itemEquipTouch;
			}
			break;

		case PU_AMMO:
			ent->touch = itemAmmoTouch;
			// kef -- knives should lay flat when dropped
			if (false && pickup->GetPickupListIndex() == OBJ_AMMO_KNIFE)
			{
				Matrix4 m;
				ent->ghoulInst->GetXForm(m);
				m.Rotate(0, M_PI*0.5);
				m.CalcFlags();
				ent->ghoulInst->SetXForm(m);
			}
			break;

		case PU_WEAPON:
			ent->touch = itemWeaponTouch;
			break;

		case PU_ARMOR:		
			if (ent->count > 100)
			{
				// Make sure no more than 100 pts of armor.

				ent->count = 100;
			}
			ent->touch = itemArmorTouch;
			break;

		case PU_CASH:
			ent->touch = itemCashTouch;
			break;

		case PU_HEALTH:
			ent->touch = itemHealthTouch;
			break;

		default:
			break;
	}

	if(dm->isDM()&&((pickup->GetType()==PU_ARMOR)||(pickup->GetType() == PU_WEAPON))&&(!(ent->spawnflags&DROPPED_ITEM))&&dm->dmRule_SPINNINGPICKUPS())
	{
		ent->s.effects|=EF_SIMPLEROTATE;
		if (pickup->GetType()==PU_WEAPON)
		{
			if (pickup->GetSpawnParm() == SFW_MACHINEGUN) // heavy MG is oriented differently
			{
				ent->s.angles[2]=-90.0;
			}
			else
			{
				ent->s.angles[2]=90.0;
			}
		}
		ent->s.origin[2]+=6;
		ent->gravity=0.0;
	}

	gi.linkentity(ent);

	// 1/12/00 kef -- register this gem with thePickupList so we can check up on it each frame (as necessary).
	//you can only register it _after_ you've set its classname, cuz that's how thePickupList knows which 
	//Pickup to associate with this edict_t.
	thePickupList.RegisterEdict(ent);

	// be careful...checkItemAfterSpawn might just free the edict
	if (dm->isDM())
	{
		dm->checkItemAfterSpawn(ent, pickup);
	}

}

/*
===============
I_SpawnKnife
===============
*/

void I_SpawnKnife(edict_t *ent)
{
	Pickup	*pickup = thePickupList.GetPickup(OBJ_AMMO_KNIFE);

	if (pickup)
	{
		I_Spawn(ent,pickup);

		// don't want these jokers piling up
		ent->think = G_FreeEdict;
		
		if(dm->dmRule_INFINITE_AMMO())
		{
			ent->nextthink = level.time + 10.0;
		}
		else
		{
			ent->nextthink = level.time + 30.0;
		}
	}
}

/*
===============
I_SpawnArmor
===============
*/

void I_SpawnArmor(edict_t *ent)
{
	Pickup	*pickup = thePickupList.GetPickup(OBJ_ARMOR);

	if (pickup)
	{
		I_Spawn(ent,pickup);
	}
	// cuz the armor falls through the corpse (ARGH!!! WHY WHY WHY WHY WHY?!?!?!!!??)
	ent->mins[2] -= 10;
}

/*
===============
MakeItem
===============
*/

edict_t *MakeItem(char *name, vec3_t origin)
{
	Pickup	*pickup = thePickupList.GetPickupFromSpawnName(name);
	edict_t	*ent=G_Spawn();

	if (pickup && ent)
	{
		I_Spawn(ent,pickup);
	}
	VectorCopy(origin,ent->s.origin);

	return(ent);
}

