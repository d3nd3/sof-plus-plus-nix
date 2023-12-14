#pragma once

//
// fields are needed for spawning from the entity string
// and saving / loading games
//
#define FFL_SPAWNTEMP		1
#define MAX_GHOULINST_SIZE	16384

#define	FOFS(x) (int)&(((edict_t *)0)->x)
#define	STOFS(x) (int)&(((spawn_temp_t *)0)->x)
#define	LLOFS(x) (int)&(((level_locals_t *)0)->x)
#define	CLOFS(x) (int)&(((gclient_t *)0)->x)

typedef enum
{
	F_INT, 
	F_SHORT,
	F_FLOAT,
	F_STRING,			// string
	F_VECTOR,
	F_ANGLEHACK,		// One var, presumed to be index 1 of vector
	F_NULL,				// A ptr to null out
	F_EDICT,			// Edict pointer handling
	F_ITEM,				// Item pointer handling
	F_MMOVE,			// Mmove pointer handling
	F_OSD,				// ObjectSpawnData object handling
	F_CLIENT,			// Client pointer handling
	F_BOOLPTR,			// Generic pointer that is recreated later		

	F_THINK_F,			// All types of function pointers
	F_BLOCKED_F,
	F_TOUCH_F,
	F_USE_F,
	F_PLUSE_F,
	F_PAIN_F,
	F_DIE_F,
	F_RESPAWN_F,

	F_IGNORE
} fieldtype_t;

typedef struct
{
	char	*name;
	int		ofs;
	fieldtype_t	type;
	int		flags;
} field_t;

extern field_t fields[];

// end
