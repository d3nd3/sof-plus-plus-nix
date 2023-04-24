// q_shared.h -- included first by ALL program modules.

// Prevent multiple inclusion. Why do id never do this? Sloppy.
#pragma once

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

// unknown pragmas are SUPPOSED to be ignored, but....
#pragma warning(disable : 4244)     // MIPS
#pragma warning(disable : 4136)     // X86
#pragma warning(disable : 4051)     // ALPHA

#pragma warning(disable : 4018)     // signed/unsigned mismatch
#pragma warning(disable : 4305)		// truncation from const double to float

// these happen a lot in level4
#pragma warning(disable : 4201)		// nameless struct/union
#pragma warning(disable : 4214)		// bit field
#pragma warning(disable : 4514)		// unreferenced inline function has been removed
#pragma warning(disable : 4100)		// unreferenced formal parm

#endif // _WIN32

#include <assert.h>
#include <math.h>
#include <stdio.h>
// #include <io.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#pragma warning(disable : 4786) 
#include <set>
#include <string>
#include <vector>
#include <list>
#include <map>
using namespace std;

#if defined _M_IX86 && !defined C_ONLY
#define id386	1
#else
#define id386	0
#endif

#if defined _M_ALPHA && !defined C_ONLY
#define idaxp	1
#else
#define idaxp	0
#endif

typedef unsigned char 		byte;
typedef unsigned short 		word;
typedef int					qboolean;

#define FLOAT_ZERO_EPSILON	0.0005f

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef M_PI
#define M_PI				3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#define DEGTORAD			0.01745329252
#define RADTODEG			57.295779513

// angle indexes
#define	PITCH				0		// up / down
#define	YAW					1		// left / right
#define	ROLL				2		// fall over

#define	ANGLE2SHORT(x)		((int)((x) * 65536 / 360) & 65535)
#define	SHORT2ANGLE(x)		((x) * (360.0 / 65536))

#define	MAX_STRING_CHARS	1024	// max length of a string passed to Cmd_TokenizeString
#define	MAX_STRING_TOKENS	80		// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_CHARS		256		// max length of an individual token

#define	MAX_QPATH			64		// max length of a quake game pathname
#define	MAX_OSPATH			128		// max length of a filesystem pathname
#define MAX_PATH			260		// Windows max filename length

#define	MAXLIGHTMAPS		4
//
// per-level limits
//
#define	MAX_CLIENTS			32		// absolute limit
#define	MAX_EDICTS			1024	// must change protocol to increase more
#define	MAX_LIGHTSTYLES		256
#define	MAX_MODELS			256		// these are sent over the net as bytes
#define	MAX_SOUNDS			356		//    (sounds can be sent over as a word index now)
#define MAX_EFPACKS			256		// so they cannot be blindly increased
#define	MAX_IMAGES			256		// 
#define MAX_STRING_PACKAGES	30
#define MAX_PLAYERICONS		16
#define MAX_GHOULFILES		2048	// just to be sure.

// game print flags
#define	PRINT_LOW			0		// pickup messages
#define	PRINT_MEDIUM		1		// death messages
#define	PRINT_HIGH			2		// critical messages
#define	PRINT_CHAT			3		// chat messages

#define	ERR_FATAL			0		// exit the entire game with a popup window
#define	ERR_DROP			1		// print to console and disconnect from game
#define	ERR_DISCONNECT		2		// don't kill server

#define	PRINT_ALL			0
#define PRINT_DEVELOPER		1		// only print when "developer 1"
#define PRINT_EXT_DEVELOPER	2		// only print when "developer & 4"
#define PRINT_ALERT			3		// Bring up a message box	

#define DEF_WIDTH			640
#define DEF_HEIGHT			480

#define GGHOUL_ID_NAME_LEN	55		// Gameghoul stuff that's needed on client and server.

// destination class for gi.multicast()
typedef enum
{
	MULTICAST_ALL,
	MULTICAST_PHS,
	MULTICAST_PVS,
	MULTICAST_ALL_R,
	MULTICAST_PHS_R,
	MULTICAST_PVS_R
} multicast_t;

typedef float vec_t;
typedef vec_t vec3_t[3];
typedef vec_t vec5_t[5];

//=============================================

char *COM_SkipPath (char *pathname);
void COM_StripExtension (char *in, char *out);
void COM_FileBase (char *in, char *out);
void COM_FilePath (char *in, char *out);
void COM_DefaultExtension (char *path, char *extension);
char *COM_FileExtension (char *in);

char *COM_Parse (char **data_p);
// data is an in/out parm, returns a parsed out token

void Com_sprintf (char *dest, int size, char *fmt, ...);

//=============================================

#define LittleLong(l)		(l)
#define	LittleFloat(l)		(l)
#define LittleShort(l)		(l)

short BigShort(short l);
int BigLong (int l);
float BigFloat (float l);

void Swap_Init (void);
char *va(char *format, ...);

//=============================================

//
// key / value info strings
//
#define	MAX_INFO_KEY		64
#define	MAX_INFO_VALUE		64
#define	MAX_INFO_STRING		512

char *Info_ValueForKey (const char *s, const char *key);
const char *Info_NextKey(const char *s, char *key, char *value);
void Info_RemoveKey (char *s, char *key);
void Info_SetValueForKey (char *s, char *key, char *value);
bool Info_Validate (char *s);

/*
==============================================================

SYSTEM SPECIFIC

==============================================================
*/

int Sys_Milliseconds (void);
void Sys_Mkdir (char *path);

/*
** pass in an attribute mask of things you wish to REJECT
*/
char *Sys_FindFirst (char *path, unsigned musthave, unsigned canthave, struct fileinfo_s *fi = NULL );
char *Sys_FindNext ( unsigned musthave, unsigned canthave, struct fileinfo_s *fi = NULL );
void Sys_FindClose (void);

// this is only here so the functions in q_shared.c and q_shwin.c can link
void Sys_Error (char *error, ...);
void Com_Printf (char *msg, ...);

/*
==========================================================

CVARS (console variables)

==========================================================
*/

typedef struct cvar_s cvar_t;

typedef void (*cvarcommand_t) (cvar_t *cvar);

#define	CVAR_ARCHIVE	0x00000001	// set to cause it to be saved to vars.rc
#define	CVAR_USERINFO	0x00000002	// added to userinfo  when changed
#define	CVAR_SERVERINFO	0x00000004	// added to serverinfo when changed
#define	CVAR_NOSET		0x00000008	// don't allow change from console at all,
									// but can be set from the command line
#define	CVAR_LATCH		0x00000010	// save changes until server restart
#define	CVAR_INT		0x00000020	// value is locked to integral
#define CVAR_PLAIN		0x00000040	// can't increment, decrement, or toggle this guy
#define CVAR_INTERNAL	0x00000080	// can only be set internally by the code, never by the user in any way directly

#define CVAR_WEAPON		0x00000100	// this cvar defines possession of a weapon
#define CVAR_ITEM		0x00000200	//  "	  "	   "		  "     "  " item
#define CVAR_AMMO		0x00000400	//  "	  "	   "		  "     "  " quantity of ammo
#define CVAR_MISC		0x00000800
#define CVAR_PARENTAL	0x00001000	// this cvar is stored in the registry through special means

#define CVAR_MENU_MASK	(CVAR_WEAPON | CVAR_ITEM | CVAR_AMMO | CVAR_MISC)
#define CVAR_INFO		(CVAR_MENU_MASK | CVAR_USERINFO | CVAR_SERVERINFO)

// nothing outside the Cvar_*() functions should modify these fields!
typedef struct cvar_s
{
	char			*name;
	char			*string;
	char			*latched_string;	// for CVAR_LATCH vars
	int				flags;
	cvarcommand_t	command;
	qboolean		modified;	// set each time the cvar is changed
	float			value;
	struct cvar_s *	next;
} cvar_t;

// these need to be seen by all modules (including server because of code like "BoxOnTraceSide()" that exist in both), and GL
extern cvar_t *use_amd3d;	// user-adjustable
extern cvar_t *use_mmx;		//
extern cvar_t *cpu_mmx;		// *not* user-adjustable
extern cvar_t *cpu_amd3d;	//

// Quick version of float to long (trunc/round undefined)

#pragma warning (disable:4035)
/*long Q_ftol( float f )
{
	static int tmp;
	
	asm("fld dword ptr [%esp+4]\n\t"
		"fistp tmp"
		"mov %eax, tmp"
		"ret");
	
	return 0;
}
*/

_inline float Clamp(float src, float min, float max)
{
	if(src > max)
	{
		src = max;
	}
	if(src < min)
	{
		src = min;
	}
	return(src);
}

_inline int ClampI(int src, int min, int max)
{
	if(src > max)
	{
		src = max;
	}
	if(src < min)
	{
		src = min;
	}
	return(src);
}

/*
==========================================================

Definres required to access the bsp tree

==========================================================
*/

// lower bits are stronger, and will eat weaker brushes completely
#define	CONTENTS_SOLID			0x00000001		// an eye is never valid in a solid
#define	CONTENTS_WINDOW			0x00000002		// translucent, but not watery
#define	CONTENTS_AUX			0x00000004
#define	CONTENTS_LAVA			0x00000008
#define	CONTENTS_SLIME			0x00000010
#define	CONTENTS_WATER			0x00000020
#define	CONTENTS_MIST			0x00000040		// 64
#define CONTENTS_SHOT			0x00000080
#define CONTENTS_FOG			0x00000100

#define	CONTENTS_AREAPORTAL		0x8000
#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000

// currents can be added to any other contents, and may be mixed
#define	CONTENTS_CURRENT_0		0x40000
#define	CONTENTS_CURRENT_90		0x80000
#define	CONTENTS_CURRENT_180	0x100000
#define	CONTENTS_CURRENT_270	0x200000
#define	CONTENTS_CURRENT_UP		0x400000
#define	CONTENTS_CURRENT_DOWN	0x800000

#define	CONTENTS_ORIGIN			0x1000000	// removed before bsping an entity

#define	CONTENTS_MONSTER		0x2000000	// should never be on a brush, only in game
#define	CONTENTS_DEADMONSTER	0x4000000
#define	CONTENTS_DETAIL			0x8000000	// brushes to be added after vis leafs
#define	CONTENTS_TRANSLUCENT	0x10000000	// auto set if any surface has trans
#define	CONTENTS_LADDER			0x20000000

#define	VISIBLE_CONTENTS_MASK	(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_AUX|CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER|CONTENTS_MIST|CONTENTS_FOG)

// content masks
#define	MASK_ALL				(-1)
#define	MASK_SOLID				(CONTENTS_SOLID | CONTENTS_WINDOW)
#define	MASK_PLAYERSOLID		(CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER)
#define	MASK_DEADSOLID			(CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW)
#define	MASK_MONSTERSOLID		(CONTENTS_SOLID | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER)
#define	MASK_WATER				(CONTENTS_WATER | CONTENTS_LAVA | CONTENTS_SLIME)
#define	MASK_OPAQUE				(CONTENTS_SOLID | CONTENTS_SLIME | CONTENTS_LAVA)
#define	MASK_SHOT				(CONTENTS_SOLID | CONTENTS_MONSTER | CONTENTS_DEADMONSTER | CONTENTS_SHOT)
#define MASK_CURRENT			(CONTENTS_CURRENT_0 | CONTENTS_CURRENT_90 | CONTENTS_CURRENT_180 | CONTENTS_CURRENT_270 | CONTENTS_CURRENT_UP | CONTENTS_CURRENT_DOWN)

// gi.BoxEdicts() can return a list of either solid or trigger entities
// FIXME: eliminate AREA_ distinction?
#define	AREA_SOLID		1
#define	AREA_TRIGGERS	2

#define	SURF_LIGHT				0x00000001		// value will hold the light strength
#define	SURF_SLICK				0x00000002		// effects game physics
#define	SURF_SKY				0x00000004		// don't draw, but add to skybox
#define	SURF_WARP				0x00000008		// turbulent water warp
#define	SURF_TRANS33_DEPREC		0x00000010		// Used to clarify view in the editor - not used in game
#define	SURF_TRANS66_DEPREC		0x00000020		// Deprecated - recycle
#define	SURF_FLOWING			0x00000040		// scroll towards angle
#define	SURF_NODRAW				0x00000080		// don't bother referencing the texture
#define	SURF_NO_DETAIL			0x00000400	// face doesn't draw the detail texture normally assigned to it
#define	SURF_ALPHA_TEXTURE		0x00000800		// texture has alpha in it, and should show through in bsp process
#define	SURF_ANIMSPEED			0x00001000		// value will hold the anim speed in fps
#define SURF_UNDULATE			0x00002000		// rock surface up and down...
#define SURF_SKYREFLECT			0x00004000		// liquid will somewhat reflect the sky - not quite finished....
#define SURF_MAP				0x00008000		// used for the auto-map
#define SURF_REGION				0x00010000
#define SURF_SIMPLE_SPHERICAL	0x00020000

typedef struct mtexinfo_s
{
	float					vecs[2][4];
	int						flags;
	int						numframes;
	struct mtexinfo_s		*next;		// animation chain
	struct image_s			*image;
	struct image_s			*origimage;
	// Required for server/client loading
	struct ctextureinfo_s	*textureinfo;
	struct ctextureinfo_s	*origtextureinfo;
	int						value;
	int						Damage;
	int						DamageLevel;
} mtexinfo_t;

// plane_t structure
// !!! if this is changed, it must be changed in asm code too !!!
typedef struct cplane_s
{
	vec3_t	normal;
	float	dist;
	byte	type;			// for fast side tests
	byte	signbits;		// signx + (signy<<1) + (signz<<1)
} cplane_t;

// a trace is returned when a box is swept through the world
typedef struct
{
	bool				allsolid;	// if true, plane is not valid
	bool				startsolid;	// if true, the initial point was in a solid area
	float				fraction;	// time completed, 1.0 = didn't hit anything
	vec3_t				endpos;		// final position
	cplane_t			plane;		// surface normal at impact
	struct mtexinfo_s	*surface;	// surface hit
	int					contents;	// contents on other side of surface hit
	struct edict_s		*ent;		// not set by CM_*() functions
	int					leaf_num;	// leaf the hit took place in
} trace_t;


//==============================================

typedef struct aEffectInfo_s
{
	short	effectId;
	short	bolt;//actually a ghoul ID
	short	size;
}aEffectInfo_t;

#define NUM_EFFECTS 4

// entity_state_t is the information conveyed from the server
// in an update message about entities that the client will
// need to render in some way

typedef struct entity_state_s
{
	int			number;			// edict index

	vec3_t		origin;
	vec3_t		angles;
	vec_t		angle_diff;
	int			modelindex;
	int			renderindex;
	int			frame;
	int			skinnum;
	int			effects;
	int			renderfx;
	int			solid;			// For client side prediction, (bits 0-7) is x/y radius
								// (bits 8-15) is z down distance, (bits16-23) is z up.
								// The msb is unused (and unsent).
								// gi.linkentity sets this properly
	int			sound;			// for looping sounds, to guarantee shutoff
	int			sound_data;		// data for volume on looping sound, as well as attenuation
	int			event;			// impulse events -- muzzle flashes, footsteps, etc
	int			event2;			// events only go out for a single frame, they
	int			event3;			// are automatically cleared each frame - 3 just in case...
	int			data;			// data for the event FX
	int			data2;			// most of the time, these are
	int			data3;			// empty

	aEffectInfo_t	effectData[NUM_EFFECTS];
} entity_state_t;

//==============================================

class IGhoulInst;
#define	MAX_STATS 		16	// see q_sh_interface.h

// pmove_state_t is the information necessary for client side movement
// prediction
typedef enum 
{
	// can accelerate and turn
	PM_NORMAL,
	PM_NOCLIP,
	PM_SPECTATOR,
	// no acceleration or turning
	PM_DEAD,
	PM_GIB,		// different bounding box
	PM_FREEZE,
	PM_CAMERA_FREEZE,
	PM_SPECTATOR_FREEZE,
} pmtype_t;

// pmove->pm_flags
#define	PMF_DUCKED			1
#define	PMF_JUMP_HELD		2
#define	PMF_ON_GROUND		4
#define	PMF_TIME_WATERJUMP	8	// pm_time is waterjump
#define	PMF_TIME_LAND		16	// pm_time is time before rejump
#define	PMF_TIME_TELEPORT	32	// pm_time is non-moving time
#define	PMF_FATIGUED		64	// player is fatigued and cannot run
#define PMF_NO_PREDICTION	128	// temporarily disables prediction (used for grappling hook)

// this structure needs to be communicated bit-accurate
// from the server to the client to guarantee that
// prediction stays in sync, so no floats are used.
// if any part of the game code modifies this struct, it
// will result in a prediction error of some degree.
typedef struct
{
	pmtype_t	pm_type;

	short		origin[3];		// 12.3
	short		velocity[3];	// 12.3
	byte		pm_flags;		// ducked, jump_held, etc
	byte		pm_time;		// each unit = 8 ms
	short		gravity;
	short		delta_angles[3];// add to command angles to get view direction
								// changed by spawns, rotating objects, and teleporters
	byte		moveScale;		// Global movescale used to facilitate different moverates,
								// limping etc. Will not change very often.
} pmove_state_t;

// Testing new movepeeds -MW.
#define _MOVESPEEDS_

//
// button bits
//
#define	BUTTON_ATTACK		1
#define	BUTTON_USE			2
#define BUTTON_ALTATTACK	4
#define BUTTON_WEAP3		8
#define BUTTON_WEAP4		16
#define BUTTON_RUN			32
#define BUTTON_ACTION		64
#define	BUTTON_ANY			128			// any key whatsoever

// usercmd_t is sent to the server each client frame
typedef struct usercmd_s
{
	byte	msec;
	byte	buttons; // :: 1
	byte	lightlevel;		// light level the player is standing on :: 2
	char	lean;			// -1 or 1 :: 3
	short	angles[3]; // :: 45 67 89
	short	forwardmove; // ab
	short	sidemove; // cd
	short	upmove; // ef
	float	fireEvent; // 10
	float	altfireEvent; // 14
} usercmd_t;


#define	MAXTOUCH	32

typedef struct
{
	// state (in / out)
	pmove_state_t	s;

	// command (in)
	usercmd_t		cmd;
	qboolean		snapinitial;	// if s has been changed outside pmove

	// results (out)
	int			numtouch;
	struct edict_s	*touchents[MAXTOUCH];

	vec3_t		viewangles;			// clamped
	float		viewheight;

	vec3_t		mins, maxs;			// bounding box size

	struct edict_s	*groundentity;
	int			watertype;
	int			waterlevel;

	// callbacks to test the world
	trace_t		(*trace) (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end);
	int			(*pointcontents) (vec3_t point);
	int			(*checkOnOtherPlayer)(unsigned int groundEntity);

	float		friction_loss;
} pmove_t;

//==============================================

// remote_type types
#define REMOTE_TYPE_FPS			0x00		// normal fps view -- WOW, it sucks that this is zero...
#define REMOTE_TYPE_TPS			0x01		// third person view
#define REMOTE_TYPE_LETTERBOX	0x02		// show's black boxes above and below (letterbox)
#define REMOTE_TYPE_CAMERA		0x04		// displays a camera graphic border
#define REMOTE_TYPE_NORMAL		0x08		// remote view, nothing fancy
#define REMOTE_TYPE_SNIPER		0x10		// sniper scope

// memory tags to allow dynamic memory to be cleaned up
#define	TAG_TEMP				764			// temporary storage while loading
#define	TAG_GAME				765			// clear when unloading the dll
#define	TAG_LEVEL				766			// clear when loading a new level
#define	TAG_MAP					767			// clear when loading a new level

#define STANDARD_POINTLIGHTDIR	255

// lightstyle that has been consumed for my own nefarious purposes as a sky light effect thing...
#define CL_WORLDSKYLIGHT		13

typedef struct sizebuf_s
{
	bool	allowoverflow;	// if false, do a Com_Error
	bool	overflowed;		// set to true if the buffer size failed
	byte	*data;
	int		maxsize;
	int		cursize;
	int		readcount;
} sizebuf_t;

typedef struct paletteRGB_s
{
	struct
	{
		byte r,g,b;
	};
} paletteRGB_t;

typedef struct paletteRGBA_s
{
	union
	{
		struct
		{
			byte r,g,b,a;
		};
		unsigned c;
		byte c_array[4];
	};
} paletteRGBA_t;

// end