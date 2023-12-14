#ifndef __W_TYPES_H
#define __W_TYPES_H

// 12/27/99 kef -- enum objType_t (OBJ_NONE...MAX_ITM_OBJS) can be used as indices into the
//array of items known as thePickupList which represents every item the player can pick up...armor,
//weapons, health, single player or multiplayer. the declaration of thePickupList is, sadly, in CWeaponInfo.h
typedef enum
{
	OBJ_NONE = 0,
	OBJ_AMMO_KNIFE,	// please don't EVER make this equal to anything less than 1. search for OBJ_AMMO_KNIFE - 1 and find out why.
	OBJ_AMMO_PISTOL,//these are now grossly misnamed - but they get the job done
	OBJ_AMMO_SHOTGUN,
	OBJ_AMMO_PISTOL2, // this is .44 ammo
	OBJ_AMMO_AUTO,
	OBJ_AMMO_MPG,
	OBJ_AMMO_GAS,
	OBJ_AMMO_ROCKET,
	OBJ_AMMO_SLUG,

	// single player only
	OBJ_AMMO_SINGLE_PISTOL,
	OBJ_AMMO_SINGLE_SHOTGUN,
	OBJ_AMMO_SINGLE_PISTOL2,
	OBJ_AMMO_SINGLE_AUTO,
	OBJ_AMMO_SINGLE_MPG,
	OBJ_AMMO_SINGLE_GAS,
	OBJ_AMMO_SINGLE_ROCKET,
	OBJ_AMMO_SINGLE_SLUG,

	OBJ_WPN_PISTOL2,
	OBJ_WPN_PISTOL1,
	OBJ_WPN_MACHINEPISTOL,
	OBJ_WPN_ASSAULTRIFLE,
	OBJ_WPN_SNIPER,
	OBJ_WPN_AUTOSHOTGUN,
	OBJ_WPN_SHOTGUN,
	OBJ_WPN_MACHINEGUN,
	OBJ_WPN_ROCKET,
	OBJ_WPN_MICROWAVE,
	OBJ_WPN_FLAMEGUN,

	OBJ_ARMOR,

	OBJ_ITM_FLASHPACK,
//	OBJ_ITM_NEURALGRENADE,
	OBJ_ITM_C4,
	OBJ_ITM_LIGHTGOGGLES,
	OBJ_ITM_MINE,
	OBJ_ITM_MEDKIT,
	OBJ_ITM_GRENADE,
	OBJ_CTF_FLAG,

	OBJ_HLTH_SM,
	OBJ_HLTH_LRG,

	MAX_ITM_OBJS
} objType_t;


// 12/27/99 kef -- these are used by thePickupList (specifically, by class Pickup). see CWeaponInfo.h
typedef enum
{
	PU_UNKNOWN = -1,
	PU_AMMO,
	PU_WEAPON,
	PU_ARMOR,
	PU_INV,
	PU_CASH,
	PU_HEALTH,
} pickup_t;

typedef enum
{
	AMMO_NONE = 0,
	AMMO_KNIFE,
	AMMO_44,
	AMMO_9MM,
	AMMO_SHELLS,
	AMMO_556,
	AMMO_ROCKET,
	AMMO_MWAVE,
	AMMO_FTHROWER,
	AMMO_SLUG,
	AMMO_MACHPIS9MM,
	MAXAMMOS
} ammos_t;

// for weapon selection

typedef enum
{
	SFW_EMPTYSLOT = 0,
	SFW_KNIFE,
	SFW_PISTOL2,
	SFW_PISTOL1,
	SFW_MACHINEPISTOL,
	SFW_ASSAULTRIFLE,
	SFW_SNIPER,
	SFW_AUTOSHOTGUN,
	SFW_SHOTGUN,
	SFW_MACHINEGUN,
	SFW_ROCKET,
	SFW_MICROWAVEPULSE,
	SFW_FLAMEGUN,
	SFW_HURTHAND,
	SFW_THROWHAND,
	SFW_NUM_WEAPONS
} weapons_t;

typedef enum
{
	SFE_EMPTYSLOT = 0,
	SFE_FLASHPACK,
//	SFE_NEURAL_GRENADE,
	SFE_C4,
	SFE_LIGHT_GOGGLES,
	SFE_CLAYMORE,
	SFE_MEDKIT,
	SFE_GRENADE,
	SFE_CTFFLAG,
	SFE_NUMITEMS
} equipment_t;
/*
typedef enum
{
	CTF_FLAG = 0,
} ctf_t;
*/
typedef enum
{
	SFC_CASH = 0,
} cash_t;

typedef enum
{
	SFH_SMALL_HEALTH = 0,
	SFH_LARGE_HEALTH,
} health_t;

#endif // __W_TYPES_H
