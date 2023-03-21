// q_sh_fx.h

#ifndef _Q_SH_FX_
#define _Q_SH_FX_

#include "../ghoul/ighoul.h"

typedef enum
{
	RI_WORLD,
	RI_WORLD_NOSURF,
	RI_BMODEL,
	RI_ENT,
}relativeInfo_t;


#define DEBRIS_YES 'y'
#define DEBRIS_PRECACHE_IMAGE 'p'	// kef -- sorry hack. precache image for remote_cameras

// need debris to be shared so entity spawn functions can register which
//debris types need to be cached
enum
{
	CLGHL_CHUNKBRICK,
	CLGHL_CHUNKGLASSSM,
	CLGHL_CHUNKGLASSMED,
	CLGHL_CHUNKGLASSLRG,
	CLGHL_CHUNKMETALSM,
	CLGHL_CHUNKMETALMED,
	CLGHL_CHUNKMETALLRG,
	CLGHL_CHUNKROCKSM,
	CLGHL_CHUNKROCKMED,
	CLGHL_CHUNKROCKLRG,
	CLGHL_CHUNKROCKTINY,
	CLGHL_CHUNKWALLSM,
	CLGHL_CHUNKWALLMED,
	CLGHL_CHUNKWALLLRG,
	CLGHL_CHUNKWALLTINY,
	CLGHL_CHUNKWOODSM,
	CLGHL_CHUNKWOODMED,
	CLGHL_CHUNKWOODLRG,
	CLGHL_SHELL,
	CLGHL_EXPLODEBIG,
	CLGHL_EXPLODESML,
	CLGHL_CAN,
	CLGHL_PAPER,
	CLGHL_MUSHROOM,
	CLGHL_ENDOFLIST,
	NUM_CLGHL,
};

typedef struct cl_ghoul_data_s
{
	char *path;
	char *file;
	char *material;
	int	 partCnt;	// Number of parts in model
}cl_ghoul_data_t;

extern cl_ghoul_data_t cl_fxg [NUM_CLGHL];

// need decals to be shared so entity spawn functions can register which
//decals need to be cached
enum
{
	CLMOD_SMOKE = 0,
	CLMOD_BLOOD1,
	CLMOD_BLOOD2,
	CLMOD_BLOOD3,
	CLMOD_BLOODPOOL2,
	CLMOD_FOOTPRINT,
	CLMOD_FOOTPRINT2,
	CLMOD_LIGHTNING,
	CLMOD_RING,
	CLMOD_DIRT,
	CLMOD_SPARKPART,
	CLMOD_WATER,
	CLMOD_POWDER,
	CLMOD_BHOLE,
	CLMOD_BULLETWHIZ,
	CLMOD_SPARKPARTBLUE,
	CLMOD_FLARE,
	CLMOD_WATERDROP,
	CLMOD_GENERIC_DENT,
	CLMOD_RAIN,
	CLMOD_MFLASH,
	CLMOD_SHOCKBLUE,
	CLMOD_SHOCK3,
	CLMOD_SHOCK4,
	CLMOD_BOOM3,
	CLMOD_BOOM5,
	CLMOD_SCORCH,
//	CLMOD_OIL1,
//	CLMOD_OIL2,
	CLMOD_OOZE,
	CLMOD_FIREEXPLODE,
	CLMOD_WHITESCORCH,
	CLMOD_SNOW,
	CLMOD_BUBBLE,
	CLMOD_PIPELEFT,
	CLMOD_BHOLEGLASS,
	CLMOD_SLASH,
	CLMOD_RINGBLAST,
	CLMOD_BEAM,
	CLMOD_MFLASH2,
	CLMOD_MFLASH3,
	CLMOD_MFLASH4,
	CLMOD_FLARE2,
	CLMOD_SMOKE2,
	CLMOD_BHOLE_METAL,
	CLMOD_BHOLE_WD,
	CLMOD_MINIMIFLASH1,
	CLMOD_MINIMIFLASH2,
	CLMOD_MINIMIFLASH3,
	CLMOD_EXP_FIRE1,
	CLMOD_EXP_FIRE2,
	CLMOD_EXP_FIRE3,
	CLMOD_EXP_FIRE4,
	CLMOD_EXP_SMKGRN,
	CLMOD_EXP_SMKGRY,
	CLMOD_EXP_SMKWHT,
	CLMOD_EXP_FIRESTREAK,
	CLMOD_EXP_WHITESTREAK,
	CLMOD_PUDDLE,
	CLMOD_ENDOFLIST,
	NUM_CLMODS
};


// need client sound effects to be shared so entity spawn functions can register which
//sounds need to be cached

#define NUM_SPLATTERSOUNDS 3
#define NUM_FOOTSTEPS_PER_GROUP 4
#define NUM_RICOCHETSOUNDS 3
#define NUM_PASSBYSOUNDS   4
#define NUM_FLESHIMPACTS	3
#define NUM_KNIFESLASHES	3
#define NUM_GORE_FALLS		3

enum
{
	CLSFX_SPARK1 = 0,
	CLSFX_SPARK2,
	CLSFX_FOOTSTEP_METAL,
	CLSFX_FOOTSTEP_SNOW		= CLSFX_FOOTSTEP_METAL + NUM_FOOTSTEPS_PER_GROUP,
	CLSFX_FOOTSTEP_WATER	= CLSFX_FOOTSTEP_SNOW + NUM_FOOTSTEPS_PER_GROUP,
	CLSFX_FOOTSTEP_GRASS	= CLSFX_FOOTSTEP_WATER + NUM_FOOTSTEPS_PER_GROUP,
	CLSFX_FOOTSTEP_GRAVEL	= CLSFX_FOOTSTEP_GRASS + NUM_FOOTSTEPS_PER_GROUP,
	CLSFX_FOOTSTEP_SAND		= CLSFX_FOOTSTEP_GRAVEL + NUM_FOOTSTEPS_PER_GROUP,
	CLSFX_FOOTSTEP_WOOD		= CLSFX_FOOTSTEP_SAND + NUM_FOOTSTEPS_PER_GROUP,
	CLSFX_FOOTSTEP_STONE	= CLSFX_FOOTSTEP_WOOD + NUM_FOOTSTEPS_PER_GROUP,
//	CLSFX_SPLATTERS			= CLSFX_FOOTSTEP_STONE + NUM_FOOTSTEPS_PER_GROUP,
	//CLSFX_RICOCHETS			= CLSFX_SPLATTERS + NUM_SPLATTERSOUNDS,
	CLSFX_RICOCHETS			= CLSFX_FOOTSTEP_STONE + NUM_FOOTSTEPS_PER_GROUP,
	CLSFX_STONEIMPACT		= CLSFX_RICOCHETS + NUM_RICOCHETSOUNDS,
	CLSFX_WATERIMPACT,
	CLSFX_WOODIMPACT,
	CLSFX_GRASSIMPACT,
	CLSFX_GRAVELIMPACT,
	CLSFX_SANDIMPACT,
	CLSFX_SNOWIMPACT,
	CLSFX_METALIMPACT,
	CLSFX_FLESHIMPACT,
	CLSFX_PASSBY			= CLSFX_FLESHIMPACT + NUM_FLESHIMPACTS,
	CLSFX_SHOT				= CLSFX_PASSBY + NUM_PASSBYSOUNDS,
	CLSFX_THUNDER,
	CLSFX_FLASHPACK,
	CLSFX_AUTOGUNFIRE,
	CLSFX_AUTOGUNSLOW,
	CLSFX_AUTOGUNSTOP,
	CLSFX_EXPLODE,
	CLSFX_CHAIR_SQUEAK1,
	CLSFX_CHAIR_SQUEAK2,
	CLSFX_CHAIR_SQUEAK3,
	CLSFX_TRAIN_BRAKE,
	CLSFX_TRAIN_HORN,
	CLSFX_TRAIN_IMPACT,
	CLSFX_TRAIN_MOVE,
	CLSFX_KNIFE_SLASH,
	CLSFX_KNIFE_IMPACT = CLSFX_KNIFE_SLASH + NUM_KNIFESLASHES,
	CLSFX_KNIFE_THROW,
	CLSFX_LAND_GRASS,
	CLSFX_LAND_GRAVEL,
	CLSFX_LAND_METAL,
	CLSFX_LAND_SAND,
	CLSFX_LAND_SNOW,
	CLSFX_LAND_STONE,
	CLSFX_LAND_WATER,
	CLSFX_LAND_WOOD,
//	CLSFX_SHOT_EAGLE,
//	CLSFX_SHOT_INGRAM,
//	CLSFX_SHOT_JACK,
//	CLSFX_SHOT_FIREBALL,
//	CLSFX_SHOT_JACKALT,
//	CLSFX_SHOT_SNIPER,
//	CLSFX_SHOT_SPAS,
//	CLSFX_SHOT_SPASALT,
//	CLSFX_SHOT_MINIMIALT,
	CLSFX_METALSTRIKE1,
	CLSFX_METALSTRIKE2,
	CLSFX_METALSTRIKE3,
//	CLSFX_SHOT_MINIMI,
//	CLSFX_PHOSGREN,
	CLSFX_BREAK_GLASSBIG,
	CLSFX_BREAK_GLASSMED,
	CLSFX_BREAK_GLASSSMALL,
	CLSFX_BREAK_METAL,
	CLSFX_BREAK_STONE,
	CLSFX_BREAK_WOOD,
	CLSFX_ELECTRICARC,
	CLSFX_EXP_LARGE,
	CLSFX_EXP_MEDIUM,
	CLSFX_EXP_SMALL,
	CLSFX_PHONE_HIT,
	CLSFX_TRASHCAN_FLIP,
	CLSFX_METALTINK1,
	CLSFX_METALTINK2,
	CLSFX_METALTINK3,
	CLSFX_GENTINK1,
	CLSFX_GENTINK2,
	CLSFX_GORE_FALL,
	CLSFX_CRACK1 = CLSFX_GORE_FALL + NUM_GORE_FALLS,
	CLSFX_CRACK2,
	CLSFX_CRACK3,
	CLSFX_SHOT_HELI1,
	CLSFX_SHOT_HELI2,
	CLSFX_SHOT_HELI3,
	CLSFX_SHOTGUNTINK1,
	CLSFX_SHOTGUNTINK2,
	CLSFX_SHOTGUNTINK3,
	CLSFX_HEAVYTINK1,
	CLSFX_HEAVYTINK2,
	CLSFX_HEAVYTINK3,
	CLSFX_ENDOFLIST,
	NUM_CLSFX,			
};

typedef struct cl_effect_data_s
{
	char *filename;
}cl_effect_data_t;

extern cl_effect_data_t cl_fxs [NUM_CLSFX];


// entity_state_t->event values
// ertity events are for effects that take place reletive
// to an existing entities origin.  Very network efficient.
typedef enum
{
	EV_NONE,
	EV_ITEM_RESPAWN,
	EV_PHOSPHUR_EXPLODE,
	EV_FOOTSTEPLEFT,
	EV_FOOTSTEPRIGHT,
	EV_FOOTSTEPLEFTRUN,
	EV_FOOTSTEPRIGHTRUN,
	EV_FALLSHORT,
	EV_FALL,
	EV_FALLFAR,
	EV_OBJECT_COLLIDE_SHORT,
	EV_OBJECT_COLLIDE_MEDIUM,
	EV_OBJECT_COLLIDE_FAR,
	EV_FOOTSTEPMETALLEFT,
	EV_FOOTSTEPMETALRIGHT,
	EV_PLAYER_TELEPORT,
	EV_CAMERA_SHAKE_VERYLIGHT,
	EV_CAMERA_SHAKE_LIGHT,
	EV_CAMERA_SHAKE_MEDIUM,
	EV_CAMERA_SHAKE_HEAVY,
	EV_CAMERA_SHAKE_VERYHEAVY,
	EV_BULLETCRACK,//fixme
	EV_TINTCLEAR,
	EV_NUM
} entity_event_t;

// attached effects
typedef enum
{
	AE_NOTHING = 0,
	AE_STEAM,
	AE_ONFIRE,
	AE_WATERSTREAM,
	AE_LIGHTNING,
	AE_SMOKE,
	AE_BLOODRAIN,
	AE_FOUNTAIN,
	AE_WATERDROPS,
	AE_SHAKE,
	AE_SPRINKLER,
	AE_HELISMOKE,
	AE_HUMANFLAME,
	AE_INFERNO,
	AE_CSIDE_ROTATE,//yuck
	AE_NUM_AES
} attached_effect_t;

// If this list is updated, be sure to update bin_nt/sof.mat
enum
{
	SURF_NONE = -1,
	SURF_DEFAULT = 0,
	SURF_METAL_ALSO,					// 2/10/00 kef -- rick wants METAL and METAL_STEAM switched
	SURF_SAND_YELLOW,
	SURF_SAND_WHITE,
	SURF_SAND_LBROWN,
	SURF_SAND_DBROWN,		// 5

	SURF_GRAVEL_GREY,
	SURF_GRAVEL_DBROWN,
	SURF_GRAVEL_LBROWN,

	SURF_SNOW,

	SURF_LIQUID_BLUE,		// 10
	SURF_LIQUID_GREEN,
	SURF_LIQUID_ORANGE,
	SURF_LIQUID_BROWN,

	SURF_WOOD_LBROWN,
	SURF_WOOD_DBROWN,		// 15
	SURF_WOOD_LGREY,

	SURF_STONE_LGREY,			// chunks.tga
	SURF_STONE_DGREY,	// chunksdrkclgry.tga
	SURF_STONE_LBROWN,		// chunksbrwn.tga
	SURF_STONE_DBROWN,		// 20                chunksbrwn.tga
	SURF_STONE_WHITE,		// chunkswht.tga
	SURF_STONE_GREEN,		// chunkgreen.tga
	SURF_STONE_RED,			// chunkred.tga
	SURF_STONE_BLACK,		// chunkblk.tga

	SURF_GRASS_GREEN,		// 25
	SURF_GRASS_BROWN,

	SURF_LIQUID_RED,

	SURF_METAL,							// 2/10/00 kef -- rick wants METAL and METAL_STEAM switched
	SURF_METAL_WATERJET,
	SURF_METAL_OILSPURT,	// 30
	SURF_METAL_CHEMSPURT,
	SURF_METAL_COMPUTERS,

	SURF_SNOW_LBROWN,
	SURF_SNOW_GREY,
	SURF_BLOOD,				// 35
	SURF_LIQUID_BLACK,
	SURF_GLASS,
	SURF_GLASS_COMPUTER,
	SURF_SODAMACHINE,
	SURF_PAPERWALL,			// 40
	SURF_NEWSPAPER_DAMAGE,
	SURF_METAL_STEAM,

	SURF_NUM
};


// Available materials to throw when an object dies.
enum
{
	MAT_NONE = -1,
	MAT_DEFAULT = 0,

	MAT_BRICK_BROWN,
	MAT_BRICK_DBROWN,
	MAT_BRICK_LBROWN,
	MAT_BRICK_LGREY,
	MAT_BRICK_DGREY,
	MAT_BRICK_RED,

	MAT_GLASS,

	MAT_METAL_LGREY,
	MAT_METAL_DGREY,
	MAT_METAL_RUSTY,
	MAT_METAL_SHINY,

	MAT_ROCK_BROWN,
	MAT_ROCK_LBROWN,
	MAT_ROCK_DBROWN,
	MAT_ROCK_LGREY,
	MAT_ROCK_DGREY,

	MAT_WOOD_LBROWN,
	MAT_WOOD_DBROWN,
	MAT_WOOD_DGREY,
	MAT_WOOD_LGREY,

	MAT_WALL_BLACK,
	MAT_WALL_BROWN,
	MAT_WALL_DARKBROWN,
	MAT_WALL_LIGHTBROWN,
	MAT_WALL_GREY,
	MAT_WALL_DARKGREY,
	MAT_WALL_LIGHTGREY,
	MAT_WALL_GREEN,
	MAT_WALL_ORANGE,
	MAT_WALL_RED,
	MAT_WALL_WHITE,
	MAT_ROCK_FLESH,
	MAT_WALL_STRAW,
	MAT_ROCK_SNOW,

	MATERIAL_NUM
};

// Possible marks weapons can make
typedef enum 
{
	DEBRIS_NONE = 0,
	DEBRIS_SM,
	DEBRIS_MED,
	DEBRIS_LRG,
	DEBRIS_TINY,
};

// Possible marks weapons can make
typedef enum 
{
	WMRK_NONE = 0,
	WMRK_BULLETHOLE,
	WMRK_SLASH,
};

// temp entity events
//
// Temp entity events are for things that happen
// at a location seperate from any existing entity.
// Temporary entity messages are explicitly constructed
// and broadcast.
typedef enum
{
	TE_ORANGE_SPARKS,
	TE_DUST_PUFF,
	TE_EXTINGUISHER_EXPLODE,
	TE_BLOODSPRAY,
	TE_BLOODPOOL,
	TE_EXPLODE,
	TE_WALLDAMAGE,
	TE_WALLSEVEREDAMAGE,
	TE_ELECTRIC_ARC,
	TE_WHIZBULLET,
	TE_BLUE_SPARKS,
	TE_WATER_GLOB,
	TE_WATER_SHOCK,
	TE_BARREL_EXPLODE,
	TE_BARREL_SPARKS,
	TE_SCORCHMARK,
	TE_SKY_LIGHTNING,
	TE_OOZE,
	TE_DEBRIS,
	TE_C4EXPLODE,
	TE_FLASHPACK,
	TE_NEURALPULSE,
	TE_AUTOGUNMUZZLEFLASH,
	TE_WALLSTRIKE,
	TE_SHOTGUN,
	TE_UNDERWATER_BULLET,
	TE_SMOKE_BURST,
	TE_TEST_LINE,
	TE_PIPELEFT,
	TE_WHITESCORCH,
	TE_SINKPIPE,
	TE_GUMBALLS,
	TE_WALLSPARKS,
	TE_SHOTGUNMINE,
	TE_SPAS,
	TE_BLOODSQUIRT,
	TE_HELIGUN,
	TE_WHITEBURN,
	TE_WHITEBLAST,
	TE_MINIMISMOKE,
	TE_EXPLOSION,
	TE_BLOODCLOUD,
	TE_HEADEXPLOSION,
	TE_BLOODJET,
	TE_VEHICLEEXPLOSION,
	TE_SMOKE_PUFF,
	TE_LITTLEEXPLOSION,
	TE_MAKERING,
	TE_PAPERCLOUD,
	TE_COINS,
	TE_SODACANS,
	TE_FLOORSPACE,
	TE_DUSTBITS,
	TE_PUDDLE,
	TE_RICOCHET,
	TE_MEDIUMEXPLOSION,
	TE_LARGEEXPLOSION,
	TE_TRACER,
	TE_PHONEHIT,
	TE_TRASHCANHIT,
	TE_DMSHOT,
	TE_DMSHOTHIT,
	TE_DMSHOTBIGHIT,
	TE_DMMUZZLEFLASH,
	TE_TESTBOX,
	TE_NUM
} temp_event_t;

typedef struct eft_steam_s
{
	unsigned short	color;
	byte			dir;
	byte			size;
	byte			duration;
	byte			speed;
	byte			variance;
	byte			smokeType;
	byte			offset[3];	// (-128)
	byte			more; // these next three generate "more" additional smokes
	byte			moreDir; // originating "moreDist" from the first one in the direction
	byte			moreDist; // of moreDir
}eft_steam_t;

typedef struct eft_stream_s
{
	byte			vel[3];		// (-128) * 5
	byte			offset[3];	// (-128)
	byte			duration;	// how long each water glob lasts - in hundreths of a second...
	byte			width;
	byte			color[3];	// color of the stream
	byte			randomness;
	short			floor;
}eft_stream_t;

typedef struct eft_flame_s
{
	byte			zoffset;	// (-128)
	byte			size;
	byte			lifeTime;	// Life time in hundreths of a second...
}eft_flame_t;

typedef struct eft_rain_t
{
	short			xrange;
	short			yrange;
	short			zrange;
	short			rainPerSec;
	byte			lightLevel;
	byte			rainSpeed;
	byte			wind;
	byte			rainLen;
}eft_rain_t;

typedef struct eft_snow_s
{
	short			width;		
	short			length;		
	short			height;		
	byte			snowPerSec;
	byte			speed;
	byte			lightLevel;
	byte			sideSpeed;
}eft_snow_t;

typedef struct eft_lightning_s
{
	unsigned short	color;
	byte			dir;
	byte			len;
	byte			freq;
	byte			thickness;
}eft_lightning_t;

typedef struct eft_shake_s
{
	byte			amount;
}
eft_shake_t;

enum animtype
{
	loop,
	backandforth,
};

enum dir
{
	forth,
	back,
};

typedef struct eft_anim_s
{
	byte		startframe;
	byte		endframe;
	byte		fps; // frames per second
	byte		animtype;
	byte		dir;
}eft_anim_t;

typedef struct eft_bloodrain_s
{
	byte		offset[3]; // (-128)
	byte		xrange;
	byte		yrange;
}eft_bloodrain_t;

typedef struct eft_waterdrops_s
{
	byte		xrange;
	byte		yrange;
	byte		frequency;//per sec
	byte		size;
}eft_waterdrops_t;

typedef struct eft_missilesmoke_s
{
	byte bob;
}eft_missilesmoke_t;

typedef struct eft_teargas_s
{
	byte		thickness;
}eft_teargas_t;

typedef struct eft_sprinkler_s
{
	byte			vel[3];		// 
	byte			offset[3];	// 
	byte			duration;	// how long each water lasts - in hundreths of a second...
	byte			width;
	byte			color[3];	// color of the stream
	byte			randomness;
	short			floor;
}eft_sprinkler_t;

typedef struct eft_helismoke_s
{
	GhoulUUID		instID;
	GhoulID			bolt1;//does this work right?
	GhoulID			bolt2;
	byte			smoke1Strength;
	byte			smoke2Strength;
}eft_helismoke_t;

typedef struct eft_flamethrow_s
{
	GhoulUUID		inst;
}eft_flamethrow_t;

typedef struct eft_humanflame_s
{
	GhoulUUID		instID;
	byte			intense;
	byte			timeleft; // in tenths of seconds
}eft_humanflame_t;

typedef struct eft_tripbeam_s
{
	paletteRGBA_t	pal;
	short			offset[3];
	byte			width;
}eft_tripbeam_t;

typedef struct eft_inferno_s
{
	byte			intensity;
	byte			winddir;
	byte			windspeed;
	short			xrange;
	short			yrange;
	short			zrange;
}eft_inferno_t;

typedef struct eft_shadow_s
{
	byte			size;
}eft_shadow_t;

typedef struct eft_rotate_s
{
	short			buffer;//fixing a problem through hackery.  Sigh.
	short			xRot;
	short			yRot;
	short			zRot;
	short			boltonID;
	short			boltonID2;
	short			boltonID3;
	short			boltonID4;
}eft_rotate_t;


typedef struct eft_fade_s
{
	long int		startTime;		// what time did the fade begin?
	short			duration;		// How many milliseconds does it take to fade?  Negative = fade in.
}eft_fade_t;


typedef struct eft_marker_s
{
	short			markerID;
	byte			priority;
}eft_marker_t;

typedef struct eft_normal_s
{
	unsigned char b[29];
}eft_normal_t;


// effect fields that can be sent
#define EFF_SCALE			0x01
#define EFF_NUMELEMS		0x02
#define EFF_POS2			0x04
#define EFF_DIR				0x08
#define EFF_MIN				0x10
#define EFF_MAX				0x20
#define EFF_LIFETIME		0x40
#define EFF_RADIUS			0x80
							
#define EFAT_POS			0x01
#define EFAT_ENT			0x02
#define EFAT_BOLT			0x04
#define EFAT_BOLTANDINST	0x08	
#define EFAT_POSTOWALL		0x20	// pos2 gets frame updates
#define EFAT_ALTAXIS		0x40	// uses different axis for ghoul stuff
#define EFAT_HASFLAGS		0x80

#endif _Q_SH_FX_