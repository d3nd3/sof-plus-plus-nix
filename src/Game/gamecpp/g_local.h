// g_local.h -- local definitions for game module

// Prevent multiple inclusion. Why do id never do this? Sloppy.
#pragma once

#include "q_shared.h"
#include "../qcommon/mathlib.h"

// define GAME_INCLUDE so that game.h does not define the
// short, server-visible gclient_t and edict_t structures,
// because we define the full size ones in this file
#define	GAME_INCLUDE
#include "game.h"

#include "list.h"
//#include "g_sf_local.h"

#include "fx_effects.h"
#include "q_sh_fx.h"
#include "../ghoul/ighoul.h"
#include "g_ghoul.h"
#include "dm.h"
#include "g_obj.h"


#include "../player/w_public.h"
#include "../qcommon/w_types.h"
#include "../player/player.h"

#include "../player/w_weapons.h"

#include "CWeaponInfo.h"

#include "ai_public.h"

//#include "g_econ.h"
#include "../qcommon/fields.h"
#include "g_skilllevels.h"

// #include "..\server\ipath.h"

// the "gameversion" client command will print this plus compile date
#define	GAMEVERSION	"base"

//============================================================================

// protocol bytes that can be directly added to messages
#define	svc_temp_entity		1
#define	svc_unused			2
#define	svc_inventory		3
#define svc_sound_info		4
#define svc_effect			5
#define svc_equip			6

//==================================================================

// view pitching times
#define DAMAGE_TIME		0.5
#define	FALL_TIME		0.3


// edict->spawnflags
// these are set with checkboxes on each entity in the map editor
#define	SPAWNFLAG_NOT_EASY			0x00000100
#define	SPAWNFLAG_NOT_MEDIUM		0x00000200
#define	SPAWNFLAG_NOT_HARD			0x00000400
#define	SPAWNFLAG_NOT_DEATHMATCH	0x00000800
#define	SPAWNFLAG_NOT_COOP			0x00001000

#define TIME_ENTS (0)

typedef struct
{
	char			*name;
	void			(*spawn)(edict_t *ent);
#if TIME_ENTS
	unsigned long	time_used;
	int				count;
#endif	// TIME_ENTS
} spawn_t;


// edict->flags
//  FIXME:  get rid of old q2 flags to save some space here, eh?
#define	FL_FLY					0x00000001
#define	FL_SWIM					0x00000002	// implied immunity to drowining
#define FL_IMMUNE_LASER			0x00000004
#define	FL_INWATER				0x00000008
#define	FL_GODMODE				0x00000010
#define	FL_NOTARGET				0x00000020
#define FL_IMMUNE_SLIME			0x00000040
#define FL_IMMUNE_LAVA			0x00000080
#define	FL_PARTIALGROUND		0x00000100	// not all corners are valid
#define	FL_WATERJUMP			0x00000200	// player jumping out of water
#define	FL_TEAMSLAVE			0x00000400	// not the first on the team
#define FL_NO_KNOCKBACK			0x00000800
#define FL_POWER_ARMOR			0x00001000	// power armor (if any) is active
#define FL_BODY_DAMAGE			0x00002000	// set when player takes physical damage
#define FL_ARMOR_DAMAGE			0x00004000  // set when armor is damaged
#define FL_LEAN_PLAYER			0x00008000  // set when player is leaning
#define FL_SPAWNED_IN			0x00010000	// stuff with this flag wasn't originally here
#define FL_CINEMATIC_CULL		0x00020000	// cull this joker out of current cinematic
#define FL_IAMTHEBOSS			0x00040000	// convient way to tell who is Dekker, or Dekker like
#define FL_NUGBUDDY				0x00100000  // set when player is crouching in deathmatch
#define FL_WEAPONBUDDY			0x00200000  // hangs out in front of player, encloses weapon and arms
#define FL_THROWN_WPN			0x00400000	// c4, knives, grenades...
#define FL_PICKUP				0x00800000	// flag an edict -- 'this is a pickup'

#define FL_RESPAWN				0x80000000	// used for item respawning

#define	FRAMETIME		0.1


#define MELEE_DISTANCE	80

#define BODY_QUEUE_SIZE		8

typedef enum
{
	DAMAGE_NO,
	DAMAGE_YES,			// will take damage if hit
	DAMAGE_AIM,			// auto targeting recognizes this
	DAMAGE_YES_BUT_NOT_IN_CAMERA // kef -- Rick made me do it
} damage_t;

typedef enum
{
	MISSION_NONE = 0,
	MISSION_ACCOMPLISHED,
	MISSION_EXIT,
	MISSION_OBJECTIVES,
} mission_msg_t;

//deadflag
#define DEAD_NO					0
#define DEAD_DYING				1
#define DEAD_DEAD				2
#define DEAD_RESPAWNABLE		3

//range
#define RANGE_MELEE				0
#define RANGE_NEAR				1
#define RANGE_MID				2
#define RANGE_FAR				3

//gib types
#define GIB_ORGANIC				0
#define GIB_METALLIC			1

// handedness values
#define RIGHT_HANDED			0
#define LEFT_HANDED				1
#define CENTER_HANDED			2

// Time between death and allowing a respawn
#define RESPAWN_DELAY			4.0f

// game.serverflags values
#define SFL_CROSS_TRIGGER_1		0x00000001
#define SFL_CROSS_TRIGGER_2		0x00000002
#define SFL_CROSS_TRIGGER_3		0x00000004
#define SFL_CROSS_TRIGGER_4		0x00000008
#define SFL_CROSS_TRIGGER_5		0x00000010
#define SFL_CROSS_TRIGGER_6		0x00000020
#define SFL_CROSS_TRIGGER_7		0x00000040
#define SFL_CROSS_TRIGGER_8		0x00000080
#define SFL_CROSS_TRIGGER_MASK	0x000000ff

// physics defines
#define ESCAPE_VELOCITY			30
#define FLOOR_STEEPNESS			0.7
#define FRICTION_MULT			600
#define AIRRESISTANCE_MULT		.1 // should be much smaller than 600

//this would be quite a few
#define MAX_NODES 1024

// edict->movetype values

// 
typedef enum
{
MOVETYPE_NONE,			// never moves
MOVETYPE_NOCLIP,		// origin and angles change with no interaction
MOVETYPE_PUSH,			// no clip to world, push on box contact
MOVETYPE_STOP,			// no clip to world, stops on box contact
MOVETYPE_WALK,			// gravity
MOVETYPE_STEP,			// gravity, special edge handling
MOVETYPE_FLY,
MOVETYPE_TOSS,			// gravity
MOVETYPE_FLYMISSILE,	// extra size to monsters
MOVETYPE_BOUNCE,
MOVETYPE_DAN,
MOVETYPE_TOUCHNOTSOLID,	// For rockets, uses "Dan" movement, but a SOLID_NOT entity still will use its touch function.
MOVETYPE_SPECTATOR,		// player ONLY - origin and angles change with no interaction except BSP stuff
} movetype_t;


// buddy stuff. not to be confused with body stuff. or, for that matter, baddy stuff. all of which is batty stuff. woo hoo hoo!
edict_t *GetBuddyOwner(edict_t *buddy);
void RemoveLeanBuddy(edict_t *owner);
void RemoveNugBuddy(edict_t *owner);
void RemoveWeaponBuddy(edict_t *owner);


//
// this structure is left intact through an entire game
// it should be initialized at dll load time, and read/written to
// the server.ssv file for savegames
//
typedef struct
{
	char			helpmessage1[512];
	char			helpmessage2[512];
	int				helpchanged;	// flash F1 icon if non 0, play sound
									// and increment only if 1, 2, or 3

	gclient_t		*clients;		// [maxclients]

	// can't store spawnpoint in level, because
	// it would get overwritten by the savegame restore
	char			spawnpoint[512];	// needed for coop respawns

	// store latched cvars here that we want to get at often
	int				maxclients;
	int				maxentities;

	// cross level triggers
	int				serverflags;
					
	bool			autosaved;
	byte			cinematicfreeze;
					
	int				guysKilled;
	int				friendliesKilled;
					
	int				throatShots;
	int				nutShots;
	int				headShots;
	int				gibs;
					
	int				savesUsed;
	int				cashEarned;
	float			playedTime;
	float			skillRating;
					
	CPlayerSkill	playerSkills;
} game_locals_t;


//
// this structure is cleared as each map is entered
// it is read/written to the level.sav file for savegames
//
typedef struct
{
	int			framenum;
	float		time;

	char		level_name[MAX_QPATH];	// the descriptive name (Outer Base, etc)
	char		mapname[MAX_QPATH];		// the server name (base1, etc)
	char		nextmap[MAX_QPATH];		// go here when fraglimit is hit

	float		spawnFrequency;
	float		playerLoudness;
	float		runningSpawnTotal;//This records roughly how active the spawning has been for gameplay reasons
	float		lastSpawn;
	float		lullTime;
	float		lastEnemyKill;
	char		clearedNodes[MAX_NODES];//this array indicates which nodes are still valid for spawning
	int			baseSong;
	float		spawnRushTime;
	float		spawnSoundTime;

	int			savesLeft;

	// intermission state
	float		intermissiontime;		// time the intermission was started
	char		*changemap;
	int			exitintermission;
	vec3_t		intermission_origin;
	vec3_t		intermission_angle;

	edict_t		*sight_client;	// changed once each frame for coop games
	edict_t		*sight_monster;	// changed once each frame for coop games

	edict_t		*sight_entity;
	int			sight_entity_framenum;
	edict_t		*sound_entity;
	int			sound_entity_framenum;
	edict_t		*sound2_entity;
	int			sound2_entity_framenum;

	int			pic_health;

	int			total_secrets;
	int			found_secrets;

	int			total_goals;
	int			found_goals;

	int			total_monsters;
	int			killed_monsters;

	edict_t		*current_entity;	// entity running from G_RunFrame
	int			body_que;			// dead bodies

	int			alertedStatus;		// extra spawning and lots more attentive regular enemies...

	int			deadHostages;		// current number of iced hostages
	int			maxDeadHostages;	// maximum dead hostages allowed without mission failure

	int			weaponsAvailable;	// used in precachish situations

	int			countdownEnded;
	int			missionStatus;		// did I win?

	int			forceHUD;			// force the HUD to draw, even if we have no weapon. (tutorial level)

	int			num_clients;		// added for timing re-spawns of items based on player counts in DM

	//all the silly stat stuff - gets added in at the end of every level
	int			guysKilled;			//added
	int			friendliesKilled;	//added

	int			throatShots;		//added
	int			nutShots;			//added
	int			headShots;			//added
	int			gibs;				//added

	int			savesUsed;			//added
	int			cashEarned;
	float		startTime;			//added
	float		skillRating;

} level_locals_t;

extern player_export_t	*pe;

// spawn_temp_t is only used to hold entity field values that
// can be set from the editor, but aren't actualy present
// in edict_t during gameplay
typedef struct
{
	// world vars
	char		*sky;
	float		skyrotate;
	vec3_t		skyaxis;
	char		*musicSet;
	char		*terrainSet;
	char		*nextmap;

	vec3_t		color;
	vec3_t		endpoint;

	float		spawnfrequency;

	int			lip;
	int			distance;
	int			height;
	char		*noise;
	float		pausetime;
	char		*item;
	char		*gravity;

	float		minyaw;
	float		maxyaw;
	float		minpitch;
	float		maxpitch;
	int			killedValue;
	int			survivalValue;	
	int			material;
	float		scale;

	int			merctype;
	int			mercnum;

	char*		waitAction1;
	char*		waitAction2;
	char*		waitAction3;
	char*		waitAction4;
	char*		waitAction5;
	char*		waitAction6;

	char*		moveAction;

	int			screeneffect;

	char		*script;
	char		*parms[16];
	int			surfaceType;

	int			noweapons;		// for levels like the armory where the player gets no weapons
	int			setweapons;		// for levels like TRN1 and TSR1 where we set the player's inventory

	int			maxDeadHostages; // max number of dead hostages allowed before mission failure
	char		*ambientSet;
	int			startmusic;
	int			forceHUD;
	int			weaponsAvailable; // designers can set what needs to be precached (tutorial level)

	char		*ai_name;

	int			dummy;				// for assigning fields used by the bsp process, but never ingame
} spawn_temp_t;


typedef struct
{
	// fixed data
	vec3_t		start_origin;
	vec3_t		start_angles;
	vec3_t		end_origin;
	vec3_t		end_angles;

	int			sound_start;
	int			sound_middle;
	int			sound_end;

	float		accel;
	float		speed;
	float		decel;
	float		distance;

	float		wait;

	// state data
	int			state;
	vec3_t		dir;
	float		current_speed;
	float		move_speed;
	float		next_speed;
	float		remaining_distance;
	float		decel_distance;
	void		(*endfunc)(edict_t *);
	void		(*thinkfunc)(edict_t *);
} moveinfo_t;

extern	game_locals_t	game;
extern	level_locals_t	level;
extern	game_import_t	gi;
extern	game_export_t	globals;
extern	spawn_temp_t	st;

extern char		level_ai_name[MAX_QPATH];		// the over-ride for the AI per-level

extern	int	sm_meat_index;

#define MOD_UNKNOWN				0
#define MOD_KNIFE_SLASH			1
#define MOD_KNIFE_THROWN		2
#define MOD_PISTOL1				3
#define MOD_PISTOL2				4
#define MOD_MPISTOL				5
#define MOD_ASSAULTRIFLE		6
#define MOD_SNIPERRIFLE			7
#define MOD_AUTOSHOTGUN			8
#define MOD_SHOTGUN				9
#define MOD_MACHINEGUN			10
#define MOD_PHOS_GRENADE		11
#define MOD_ROCKET				12
#define MOD_ROCKET_SPLASH		13
#define MOD_MPG					14
#define MOD_FLAMEGUN			15
#define MOD_FLAMEGUN_NAPALM		16
#define MOD_C4					17
#define MOD_CLAYMORE			18
#define MOD_NEURAL_GRENADE		19
#define MOD_WATER				20
#define MOD_SLIME				21
#define MOD_CRUSH				22
#define MOD_TELEFRAG			23
#define MOD_FALLING				24
#define MOD_SUICIDE				25
#define MOD_EXPLOSIVE			26
#define MOD_FIRE				27
#define MOD_LAVA				28
#define MOD_EXIT				29
#define MOD_BARBWIRE			30
#define MOD_DOGBITE				31
#define MOD_TRIGGER_HURT		33
#define MOD_SPLASH				34
#define MOD_TARGET_LASER		35
#define	MOD_STAR_THROWN			36
#define MOD_GRENADE				37
#define MOD_CONC_GRENADE		38

#define MOD_FRIENDLY_FIRE		0x8000000

extern	int	meansOfDeath;

extern	edict_t	*g_edicts;
extern	IGhoul	*TheGhoul;

extern	cvar_t	*freezeworld;
extern	cvar_t	*maxentities;
extern	cvar_t	*deathmatch;
extern	cvar_t	*ctf_loops;
extern	cvar_t	*ctf_loops_count;
extern	cvar_t	*ctf_flag_captured;
extern	cvar_t	*ctf_team_red;
extern	cvar_t	*ctf_team_blue;

extern	cvar_t	*mskins_expression_limit;
extern	cvar_t	*mskins_variety_limit;

extern	cvar_t	*dmflags;
extern	cvar_t	*skill;
extern	cvar_t	*fraglimit;
extern	cvar_t	*timelimit;
extern	cvar_t	*password;
extern	cvar_t	*spectator_password;
extern	cvar_t	*dedicated;
extern	cvar_t	*sv_pmodlistfile;
extern	cvar_t	*sv_altpmodlistfile;
extern	cvar_t	*sv_suicidepenalty;

extern	cvar_t	*sv_gravity;
extern	cvar_t	*sv_gravityx;
extern	cvar_t	*sv_gravityy;
extern	cvar_t	*sv_gravityz;
extern	cvar_t	*sv_maxvelocity;
extern	cvar_t	*followenemy;
extern	cvar_t	*thirdpersoncam;	// hereticii-style camera if followenemy is -1

extern cvar_t	*ai_freeze;			// monsters halt
extern cvar_t	*ai_goretest;		// sets health really high
extern cvar_t	*ai_pathtest;		// adds omniscience to guys
extern cvar_t	*ai_highpriority;	// makes all guys really expensive (turns off culling)

extern cvar_t	*ei_show;
extern cvar_t	*aipoints_show;
extern cvar_t	*ai_nonodes;
extern cvar_t	*aidec_show;
extern cvar_t	*ai_dumb;
extern cvar_t	*ai_maxcorpses;
extern cvar_t	*ai_corpselife;

extern cvar_t	*ai_spawnfrequency;

extern cvar_t	*flood_msgs;
extern cvar_t	*flood_persecond;
extern cvar_t	*flood_waitdelay;
extern cvar_t	*flood_killdelay;

extern cvar_t	*ghl_specular;
extern cvar_t	*ghl_light_method;
extern cvar_t	*ghl_precache_verts;
extern cvar_t	*ghl_precache_texture;

extern cvar_t	*sv_rollspeed;
extern cvar_t	*sv_rollangle;

extern cvar_t	*run_pitch;
extern cvar_t	*run_roll;
extern cvar_t	*bob_up;
extern cvar_t	*bob_pitch;
extern cvar_t	*bob_roll;

extern cvar_t	*log_file_counter;
extern cvar_t	*log_file_mode;
extern cvar_t	*log_file_name;
extern cvar_t	*log_file_header;
extern cvar_t	*log_file_footer;
extern cvar_t	*log_file_line_header;

extern cvar_t	*camera_viewdist;

extern cvar_t	*sv_cheats;
extern cvar_t	*maxclients;
extern cvar_t	*maxspectators;

extern cvar_t	*nodamage;

extern cvar_t	*sk_spawning;
extern cvar_t	*sk_saving;
extern cvar_t	*sk_toughness;
extern cvar_t	*sk_maxencum;

extern cvar_t	*gl_pictip;

extern cvar_t	*g_movescale;

//----Parental lock cvars----//
extern bool		lock_deaths;
extern bool		lock_blood;
extern bool		lock_gorezones;
extern bool		lock_sever;
extern bool		lock_textures;


// totally stolen from Heretic2
#ifdef __cplusplus
extern "C"
{
#endif
	extern	cvar_t			*sv_jumpcinematic;
#ifdef __cplusplus
}
#endif

// when the player dies, space bar restarts the level
extern	cvar_t			*sv_restartlevel;

extern cvar_t	*stealth;			// show/hide stealth meter


#define world	(&g_edicts[0])
#define thePlayer	(&g_edicts[1])


// item spawnflags
//#define ITEM_NO_TOUCH				0x00000002		// totally unused
// 6 bits reserved for editor flags
// 8 bits used as power cube id bits for coop games
#define DROPPED_ITEM				0x00010000
//#define	DROPPED_PLAYER_ITEM		0x00020000		// totally unused
//#define ITEM_TARGETS_USED			0x00040000		// totally unused

//
// g_save.c
//

#define MMOVE_SCRIPT_SIZE	1
#define MMOVE_HUMAN_SIZE	540
#define MMOVE_DOG_SIZE		26
#define MMOVE_COW_SIZE		6

#define MD_BOSNIA_SIZE		14
#define MD_CASTLE_SIZE		15
#define MD_GENERIC_SIZE		121
#define MD_IRAQ_SIZE		15
#define MD_ITEM_SIZE		41
#define MD_LIGHT_SIZE		23
#define MD_NY_SIZE			5
#define MD_SIBERIA_SIZE		11
#define MD_TOKYO_SIZE		12
#define MD_UGANDA_SIZE		15
#define MD_HELI_SIZE		12
#define MD_TANK_SIZE		5		
#define MD_SNOWCAT_SIZE		3

void EnumerateFields(struct field_s *fields, byte *data, unsigned long chid, int len);
void EvaluateFields(struct field_s *fields, byte *data, unsigned long chid, int len);
IGhoulInst *LoadGhoulInst(IGhoulObj *obj, const char *name);
void SaveGhoulInst(IGhoulInst *inst, const char *name);

int GetEdictNum(edict_t *e);
int GetMmoveNum(mmove_t *mm);
int GetEventNum(class Event *ev);
int GetThinkNum(void *func);

edict_t *GetEdictPtr(int e);
mmove_t *GetMmovePtr(int mm);
class Event *GetEventPtr(int ev);
void *GetThinkPtr(int func);

//
// g_cmds.c
//
void Cmd_Help_f (edict_t *ent);
void Cmd_Score_f (edict_t *ent);

//
// g_utils.c
//
qboolean	KillBox (edict_t *ent);
void	G_ProjectSource (vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);
edict_t *G_Find (edict_t *from, int fieldofs, char *match, int charsToCompare = 0);
edict_t *oldfindradius (edict_t *from, vec3_t org, float rad);
edict_t *findradius (edict_t *from, vec3_t org, float rad, int nAreatype = AREA_SOLID);
edict_t *performTriggerSearch(edict_t *ent, vec3_t org, float rad);
edict_t *performEntitySearch(edict_t *ent, vec3_t org, float rad);
edict_t *G_PickTarget (char *targetname);
void	G_UseTargets (edict_t *ent, edict_t *activator);
void	G_SetMovedir (vec3_t angles, vec3_t movedir);
int		pointLineIntersect(vec3_t start, vec3_t end, vec3_t point, float rad);//returns whether a give point is between the planes start and end
																				// and no more than rad from said vector

void G_InitEdict (edict_t *e);
edict_t *G_Spawn (void);
void G_FreeEdict (edict_t *e);
void G_FreeAllEdicts (void);
void G_Explode(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);

void	G_TouchTriggers (edict_t *ent);
void	G_TouchSolids (edict_t *ent);

char	*G_CopyString (char *in);

float	*tv (float x, float y, float z);
char	*vtos (vec3_t v);

float vectoyaw (vec3_t vec);
void vectoangles (vec3_t vec, vec3_t angles);

void InitiateRespawn(edict_t *ent);
void SetForRespawn(edict_t *ent, void (*spawnFunc)(edict_t *), float respawnDuration);

int GetGhoulPosDir(vec3_t sourcePos, vec3_t sourceAng, IGhoulInst *inst,
					   GhoulID partID, char *name, vec3_t pos, vec3_t dir, vec3_t right, vec3_t up);
void EntToWorldMatrix(vec3_t org, vec3_t angles, Matrix4 &m);

void Ignite(edict_t *target, edict_t *damager, float dmgAmount); // burn up guys
void Electrocute(edict_t *target, edict_t *damager); // zap guys
void RadiusBurn(edict_t *source, float radius);//burn up LOTS of guys ;)

//
// g_combat.c
//
qboolean OnSameTeam (edict_t *ent1, edict_t *ent2);
qboolean CanDamage (edict_t *targ, edict_t *inflictor);
void T_Damage (edict_t *targ, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate = 0.0, float absorb = 0.0);
void T_RadiusDamage (edict_t *inflictor, edict_t *attacker, float damage, edict_t *ignore, float radius, int mod, int dflags = 0);
void T_RadiusDamage2 (edict_t *inflictor, edict_t *attacker, vec3_t damagepoint, float maxdamage, float mindamage, 
					  float damageradius, edict_t *ignore, int mod, int dflags=0);
void T_ConeDamage(edict_t *inflictor, edict_t *attacker, float damage, edict_t *ignore, float radius, int mod, vec3_t coneDir, float range);

// damage flags 
// FIXME: Still need to evaluate if these are flexible enough / put all the right ones in t_damage() calls!!! -MW.

#define DAMAGE_RADIUS			0x00000001	// damage was indirect
#define DAMAGE_NO_ARMOR			0x00000002	// armour does not protect from this damage,armor not effected
#define DAMAGE_ENERGY			0x00000004	// damage is from an energy based weapon
#define DAMAGE_NO_KNOCKBACK		0x00000008	// do not affect velocity, just view angles
#define DAMAGE_BULLET			0x00000010  // damage is from a bullet (used for ricochets)
#define DAMAGE_NO_PROTECTION	0x00000020  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_NO_TEAMDAMAGE	0x00000040	// don't want to hurt team-mates
#define DAMAGE_ALL_KNOCKBACK	0x00000080	// Knockback-oriented weapon.  Minimal damage.
#define DT_MELEE				0x00000100	// damage from hand held & thrown melee weapons
#define DT_PROJECTILE			0x00000200	// damage from high velocity projectile weapons
#define DT_EXPLODE				0x00000400	// damage from explosive force
#define DT_NEURAL				0x00000800	// damage from neuralweapons, e.g. like Rush Limbaugh, N'Sync and Tele-Tubbies.
#define DT_FIRE					0x00001000	// damage from fire (tastes like burning)
#define DT_FALL					0x00002000	// damage from falling impact
#define DT_WATER				0x00004000	// damage from drowning in water or slime
#define DT_MANGLE				0x00008000	// damage from massive mangling / crushing, e.g. complete dismemberment
#define DT_JUGGLE				0x00010000	// damage is from a rapid-fire projectile weapon capable of making guys do the dancy death
#define DT_SEVER				0x00020000	// damage can result in severed limbs (but not torsos)	
#define DT_BLUNT				0x00040000	// damage is from a blunt weapon
#define DT_LONGRANGE			0x00080000	// yucky - signifies that the weapon shoots VERY far (basically for the sniper)
#define DT_DIRECTIONALGORE		0x00100000	// damage comes in a direction, but not at a spot
#define DT_SHOCK				0x00200000	// zap zap
#define DT_BACKSTAB				0x00400000	// damage is increased when victim is hit from behind
#define DT_ENEMY_ROCKET			0x00800000
#define DT_STEALTHY				0x01000000
#define DT_WATERZAP				0x02000000	// will do LOTS of damage to someone in water

// 12/29/99 kef -- moved these jokers from CWeaponInfo.cpp
#define DT_AUTOSHOTGUN (DT_SEVER|DT_MANGLE)
#define DT_SHOTGUN (DT_SEVER)
#define DT_SNIPER (DT_LONGRANGE|DT_STEALTHY)
#define DT_MPISTOL (DT_JUGGLE|DT_STEALTHY)
#define DT_KNIFE (DT_STEALTHY)

// Various default values.

#define DEFAULT_BULLET_HSPREAD	300
#define DEFAULT_BULLET_VSPREAD	500
#define DEFAULT_SHOTGUN_HSPREAD	1000
#define DEFAULT_SHOTGUN_VSPREAD	500
#define DEFAULT_DM_SHOTGUN_CNT	12
#define DEFAULT_SHOTGUN_COUNT	12
#define DEFAULT_SSHOTGUN_COUNT	20
#define DEFAULT_JITTER_DELTA	30	//12.5

//
// g_misc.c
//
void ThrowHead (edict_t *self, char *gibname, int damage, int type);
void ThrowClientHead (edict_t *self, int damage);
void ThrowGib (edict_t *self, char *gibname, int damage, int type);
void BecomeExplosion1(edict_t *self);
void ShakeCameras (vec3_t origin, int amount, int radius, int delta);
void BlindingLight(vec3_t origin, int intensity, float maxalpha, float delta);
void TearGasEnt(edict_t *ent, float intensity);//intense is from 0 to 1
void StopMusic(edict_t *ent, short numSecs);
void RestartMusic(edict_t *ent);
void StartMusic(edict_t *ent, byte songID);
void SetActionMusic(edict_t &ent);
void UpdateMusicLevel(edict_t *ent);
void SetSkyColor(float red, float blue, float green);
void FindCorrectOrigin(edict_t *ent, vec3_t origin);
void LightInit (edict_t *self);
void SndInitWorldSounds(void);
int SND_getBModelSoundIndex(char *name, int spot);
void S_RestoreSoundsFromSave(void);


//
// g_ptrail.c
//
void PlayerTrail_Init (void);
void PlayerTrail_Add (vec3_t spot);
void PlayerTrail_New (vec3_t spot);
edict_t *PlayerTrail_PickFirst (edict_t *self);
edict_t *PlayerTrail_PickNext (edict_t *self);
edict_t	*PlayerTrail_LastSpot (void);


//
// g_client.c
//
void respawn (edict_t *ent);
void BeginIntermission (edict_t *targ);
void PutClientInServer (edict_t *ent);
void InitClientPersistant (gclient_t *client);
void InitClientResp (gclient_t *client);
void InitBodyQue (void);
void ClientBeginServerFrame (edict_t *ent);

//
// g_items.c
//

void itemDropToFloor(edict_t *ent);
void I_Spawn(edict_t *ent, Pickup *pickup);
void I_SpawnKnife(edict_t *ent);
void I_SpawnArmor(edict_t *ent);
edict_t *MakeItem(char *name, vec3_t origin);
int reset_flag(edict_t *self, edict_t *owner);

//
// g_player.c
//
void player_pain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
void player_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);

//
// p_view.c
//

void PlayerNoise(edict_t *who, vec3_t where, ai_sensetype_e type, edict_t *focus, float rad, int nodeId = 0, int stealthy = 0, int suppress = 0);
void ClientEndServerFrame (edict_t *ent);

//
// p_hud.c
//
void MoveClientToIntermission(edict_t *ent, qboolean log_file);
void G_SetStats (edict_t *ent);
void DeathmatchScoreboardMessage (edict_t *ent, edict_t *killer, qboolean log_file);

//
// g_phys.c
//
void G_RunEntity (edict_t *ent);

//
// w_fire.cpp
//

void cacheForItem(equipment_t curEquip);//call with whatever the player has
void CacheAttack(int atkID);

//
// p_client.cpp
//

void IsPlayerTargetSameTeam(edict_t *source);
void AddWeaponTypeForPrecache(int type);
void incMovescale(edict_t *ent,float inc);
void resetMovescale(edict_t *ent,float newScale);

//
// various g_<location>.cpp model files
//
bool WithinFOV(edict_t *source, vec3_t target, float halfFOV);
void BboxRotate(edict_t *self);
void PhysicsModelInit(edict_t *self, char *rendername);
void GhoulPhysicsModelInit(edict_t *self);
void TintModel(edict_t *self);
void SimpleModelInit(edict_t *ent, solid_t solid);
void BecomeDebris (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void SimpleModelTouch (edict_t *ent, edict_t *other, cplane_t *plane, struct mtexinfo_s *surf);

// Model Spawn flags
#define SF_INVULNERABLE				1
#define SF_NOPUSH					2
#define SF_FLUFF					64
#define SF_PICKUP_RESPAWN			128


// Light Model Spawn flags
#define SF_LIGHT_START_OFF		1
#define SF_LIGHT_VULNERABLE		2
#define SF_LIGHT_PUSHABLE		4

//
// g_main.c
//
void PreserveClientData (void);
void RestoreClientData (edict_t *ent);
void IncreaseSpawnIntensity(float amount);//values range from 0 to 1 - most events shouldn't add more than .1 or .2 (these add up quick)

//
// g_chase.c
//
void UpdateChaseCam(edict_t *ent);
void ChaseNext(edict_t *ent);
void ChasePrev(edict_t *ent);
void GetChaseTarget(edict_t *ent);

//============================================================================

// client_t->anim_priority
#define	ANIM_BASIC		0		// stand / run
#define	ANIM_WAVE		1
#define	ANIM_JUMP		2
#define	ANIM_PAIN		3
#define	ANIM_ATTACK		4
#define	ANIM_DEATH		5

void PHYS_ClearAttachList(edict_t *ed);

//
// Client data that stays across multiple level loads.
//

typedef struct
{
	char		userinfo[MAX_INFO_STRING];
	char		netname[16];
	qboolean	spectator;
	char		bestweap[16];
	int			curWeaponType;
	int			fov;

	// A loadgame will leave valid entities that just don't have a connection yet.

	qboolean	connected;

	// Values saved and restored from edicts when changing levels.

	int			health;
	int			max_health;
} client_persistant_t;

//
// Client data that stays across deathmatch respawns.
//

typedef struct
{
	int					enterframe;			// level.framenum the client entered the game
	int					score;				// frags, etc
	vec3_t				cmd_angles;			// angles sent over in the last command
	int					game_helpchanged;
	int					helpchanged;
	qboolean			spectator;			// client is a spectator
	team_t				team;				// CTF team

	// CTF stuff
	int					ctf_state;
} client_respawn_t;

//
// This structure is cleared on each PutClientInServer(), except for 'client->pers'.
//

struct gclient_s
{
	// Known to server.
	
	player_state_t				ps;										// Communicated by server to clients.
	int							ping;

	// Private to game.

	client_persistant_t			pers;
	client_respawn_t			resp;
	pmove_state_t				old_pmove;								// Dor detecting out-of-pmove changes.
	float						moveScale;								// scale down max speed for dmreal
//	CPlayerStats				m_PlayerStats;							// Money et al.
	invPub_c					*inv;
	player_dmInfo_ic			*dmInfo;

	body_c						*body;
	char						oldNetName[128];							//body needs to remember what the last name was, so corrections don't constantly get sent back
	char						oldSkinRequest[MAX_SKINNAME_LENGTH];		//body needs to remember what the last requested skin was, so corrections don't constantly get sent back
	char						oldTeamnameRequest[MAX_TEAMNAME_LENGTH];	//body needs to remember what the last requested teamname was, so corrections don't constantly get sent back
	byte						restart_count;
	
	// Client layout stuff.

	qboolean					showscores;								// Set layout stat for scores.
	qboolean					showinventory;							// Set layout stat for inventory.
	qboolean					showhelp;								// Set layout stat for help.
								
	// Action buttons.

	int							buttons;
	int							oldbuttons;
	int							latched_buttons;
	qboolean					oktofire;
	float						fireEvent;
	float						altfireEvent;

	// Damage stuff. Sum up damage over an entire frame, so shotgun blasts give a single big kick.

	int							damage_blood;							// damage taken out of health
	int							damage_knockback;						// impact damage
	int							damage_armor;							// need this for appropriate feedback
	vec3_t						damage_from;							// origin for vector calculation
								
	// View paramters.

	float						killer_yaw;								// when dead, look at killer			
	vec3_t						weaponkick_angles;						// weapon kicks
	vec3_t						kick_origin;
	float						v_dmg_roll, v_dmg_pitch, v_dmg_time;	// damage kicks
	float						fall_time, fall_value;					// for view drop on fall
	float						damage_alpha;
	float						bonus_alpha;
	float						blinding_alpha;
	float						blinding_alpha_delta;					// added by Jake so we can control how fast the drop off from the blinding effect is
	float						gas_blend;
	vec3_t						damage_blend;
	vec3_t						v_angle;								// aiming direction
	float						bobtime;								// so off-ground doesn't change it
	vec3_t						oldviewangles;
	vec3_t						oldvelocity;
	int							old_waterlevel;
	vec3_t						fade_rgb;								// fade screen to this color;
	float						fade_alpha;
	edict_t						*chase_target;							// spectator chase-cam
	qboolean					update_chase;

	// Animation vars.

	int							anim_end;
	int							anim_priority;
	qboolean					anim_duck;
	qboolean					anim_run;
	qboolean					running;
	qboolean					ghosted;								// Invulnerability effect
	qboolean					goggles_on;								// Light Intensifier goggles are on.

	// Timers.
	
	float						next_drown_time;						// Player drown debouncing.
	float						respawn_time;							// Player can respawn when time > this.
	float						respawn_invuln_time;					// Player is invulnerable until time > this.
	float						showhelp_time;							// Player help de-activates when time > this.

	// Music.

	int							musicTime;

	// What colors are used to draw other player's playernames.

	byte						playernameColors[MAX_CLIENTS];			
	byte						oldPlayernameColors[MAX_CLIENTS];
	
	//  Camera related stuff.

	int							RemoteCameraLockCount;
	int							RemoteCameraNumber;
	int							RemoteCameraType;
	int							MercCameraNumber;
	qboolean					CameraIs3rdPerson;

	// Anti flooding vars

	float				flood_locktill;			// locked from talking
	float				flood_when[10];			// when messages were said
	int					flood_whenhead;			// head pointer for when said
	float				flood_nextnamechange;	// next time for valid nick change
	float				flood_nextkill;			// next time for suicide

	float						friction_time;
};

class CScript;

struct edict_s
{
	entity_state_t	s;
	struct gclient_s	*client;	// NULL if not a player
									// the server expects the first part
									// of gclient_s to be a player_state_t
									// but the rest of it is opaque

	qboolean	inuse;
	int			linkcount;

	// FIXME: move these fields to a server private sv_entity_t
	link_t		area;				// linked to a division node or leaf
	
	int			num_clusters;		// if -1, use headnode instead
	short		clusternums[MAX_ENT_CLUSTERS];
	int			headnode;			// unused if num_clusters != -1
	int			areanum, areanum2;

	//================================

	int			svflags;
	vec3_t		mins, maxs;
	vec3_t		absmin, absmax, size;
	solid_t		solid;
	int			clipmask;
	edict_t		*owner;


	/// GHOUL

	IGhoulInst	*ghoulInst;

	// new fields for checking if origin or mins/maxs have changed, and therefore should we relink entity?
	vec3_t		prev_mins,prev_maxs;
	vec3_t		prev_origin;
	solid_t		prev_solid;
	vec3_t		prev_angles;

	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!

	//================================
	int			movetype;
	int			flags;

	char		*model;

//  rjr			not needed yet
//	char		*rendermodel;

	float		freetime;			// sv.time when the object was freed
	
	//
	// only used locally in game, not by server
	//
	char		*message;
	unsigned short	sp_message;

	char		*classname;
#if TIME_ENTS
	spawn_t		*owner_spawn;
#endif
	int			spawnflags;

	float		timestamp;

	float		angle;			// set in qe3, -1 = up, -2 = down
	char		*target;
	char		*targetname;
	char		*scripttarget;
	char		*killtarget;
	char		*killfacing;
	char		*team;
	char		*pathtarget;
	char		*deathtarget;
	char		*combattarget;
	char		*soundName;

	char		*spawn1;		//for spawners
	char		*spawn2;		
	char		*spawn3;		

	mmove_t		*wait_action1;	//for path_corners
	mmove_t		*wait_action2;	//for path_corners
	mmove_t		*wait_action3;	//for path_corners
	mmove_t		*wait_action4;	//for path_corners
	mmove_t		*wait_action5;	//for path_corners
	mmove_t		*wait_action6;	//for path_corners

	baseObjInfo_c	*objInfo;	//for ghoul objects

	edict_t		*target_ent;	//for trains, elevators
	mmove_t		*move_action;	//for path_corners

	float		speed, accel, decel;
	vec3_t		movedir;
	vec3_t		pos1, pos2;

	vec3_t		velocity;
	vec3_t		avelocity;

	//velocity and avelocity that ai can use in their intentional movement, separate from normal physics
	vec3_t		intend_velocity;
	vec3_t		intend_avelocity;

	int			mass;
	float		stopspeed;		// defaults to 100.... see friction note	

//MoveType_Dan stuff	
	
	float		friction;		// defaults to 1.0 may be used in the future
								// in conjuction with a floor friction value
	float		gravity;		// per entity gravity multiplier (1.0 is normal)
	float		airresistance;
	float		bouyancy;	// 1 will counter gravity
	float		elasticity; // 0 will not bounce, 1 is 100% elastic, >1 == flubber, -1 means the object
	// will bowl through stuff by not processing ClipVelocity_Dan
	int			physicsFlags;
	short		attachChain;
	short		attachOwner;
//MoveType_Dan stuff ends here								// use for lowgrav artifact, flares

	float		air_finished;
	edict_t		*goalentity;

	edict_t				*movetarget;
	modelSpawnData_t	*objSpawnData;	// Used by objects (chairs, tables, etc.)

	float		yaw_speed;
//	float		ideal_yaw; //sfs--moved to be ai::ideal_angles

	float		nextthink;
	void		(*think)(edict_t *self);
	void		(*blocked)(edict_t *self, edict_t *other);	//move to moveinfo?
	void		(*touch)(edict_t *self, edict_t *other, cplane_t *plane, struct mtexinfo_s *surf);
	void		(*use)(edict_t *self, edict_t *other, edict_t *activator);
	void		(*plUse)(edict_t *self, edict_t *other, edict_t *activator);
	void		(*pain)(edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);
	void		(*die)(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);

	// are all these legit?  do we need more/less of them?
	float		touch_debounce_time;		// this one is legit
	edict_t		*touch_debounce_owner;		// as is this onew
	float		pain_debounce_time;
	float		damage_debounce_time;
	float		fly_sound_debounce_time;	//move to clientinfo
	float		last_move_time;

	int			health;
	int			max_health;
	int			gib_health;
	int			deadflag;

	union
	{
		qboolean	DoorIsOpen;
		qboolean	gibbed;
	};

	char		*map;			// target_changelevel

	int			viewheight;		// height above origin where eyesight is determined
	int			viewside;		// yeah
	int			takedamage;
	int			dmg;
	int			sounds;			//make this a spawntemp var?
	int			count;
	int			material;		// debris type objects break into

	edict_t		*chain;
	edict_t		*enemy;
	edict_t		*oldenemy;
	edict_t		*activator;
	edict_t		*groundentity;
	int			groundentity_linkcount;

	int			personality;

	// As these are both edict_t * we can leave them unioned
	union
	{
		edict_t				*teamchain;
		edict_t				*targetEnt; // for camera; also used by players for their leaning buddies
	};

	edict_t		*teammaster;

	edict_t		*mynoise;		// can go in client only
	edict_t		*mynoise2;

	int			noise_index;
	int			noise_index2;
	float		volume;
	float		attenuation;

	// timing variables
	float		wait;
	float		delay;			// before firing targets
	float		random;
	float		burntime;
	float		phosburntime;
	edict_t		*burninflictor;
	float		zapfxtime;
	float		zapdmgtime;

	union
	{
		float		teleport_time;
		float		alertRadius;//for monsters--the distance-from-enemy at which they automatically wake up--regardless of infront, line-of-sight, lighting
	};

	int			watertype;
	int			waterlevel;

	short		spawnerID;
	vec3_t		spawnOrigin;
	vec3_t		spawnAngles;
	float		spawnHealth;
	void		(*respawnFunc)(edict_t *self);
	float		respawnTime;

	// move this to clientinfo?
	int			light_level;

	int			style;			// also used as areaportal number

	int			surfaceType;

	// monetary value of items/monsters
	int		killedValue;	// money given to player at end of mission for killin' stuff	
	int		survivalValue;		// money given to player at end of mission for lettin' stuff live

	int		ctf_flags;			// I hate doing this, there should be a more elegant way to do this.Anyway bit 0 = in possession of team 0 flag; bit 1 = team 1
	float	ctf_hurt_carrier;	// time since I last hurt the flag carrier
	
	// common data blocks
	moveinfo_t		moveinfo;
	ai_c_ptr		ai;
	CScript			*Script;
};

// g_spawn.cpp
#if TIME_ENTS
	void PrintTimings(void);
#endif	// _DEBUG

char *ED_NewString (char *string);
void ED_CallSpawn (edict_t *ent);

// Scripts ds.cpp
void ProcessScripts(void);
void ShutdownScripts(void);
void SaveLocals(void);
void LoadLocals(void);
void SaveGlobals(void);
void LoadGlobals(void);

void Enemy_Printf (ai_public_c *which_ai, char *msg, ...);

//#define MAX_RADIUS_FIND_SIZE 128

//fixme - needed this demo, but not for later
#define MAX_RADIUS_FIND_SIZE 1024

typedef enum
	{
		RADAREA_NONE = 0,
		RADAREA_SOLIDS,
		RADAREA_TRIGGERS,
		RADAREA_SOLIDSANDTRIGGERS
	} areatype_t;

class CRadiusContent
{
private:
	edict_t *touchlist[MAX_RADIUS_FIND_SIZE];
	int		numFound;
public:

	CRadiusContent(void){numFound = -1;}
	CRadiusContent(vec3_t center, float radius, areatype_t desiredSolidTypes);
	CRadiusContent(vec3_t center, float radius, int useAIPoints = 0, int nodeID = 0, int squashZ = 0);
	int		getNumFound(void){return numFound;}
	edict_t *foundEdict(int i){assert(i > -1); assert(i < numFound); return touchlist[i];}
	edict_t	*getFirst(void){return touchlist[0];}
	edict_t *getEnd(void){return touchlist[numFound];}

	void	addEnt(edict_t *ent){touchlist[numFound] = ent; numFound++; assert(numFound < MAX_RADIUS_FIND_SIZE);}
};