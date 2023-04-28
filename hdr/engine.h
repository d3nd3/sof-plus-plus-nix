/*typedef char bool;
#define false 0x00
#define true 0x01
*/
#pragma once

extern unsigned int * ghoulmain;
extern unsigned int * clientinst;
extern unsigned int * objinst;


#include "../src/Game/gamecpp/q_shared.h"



typedef void client_t;

typedef int MatrixType;
typedef void (*xcommand_t) (void);
typedef struct cmd_function_s
{
	struct cmd_function_s	*next;
	char					*name;
	xcommand_t				function;
} cmd_function_t;


typedef enum
{
	cs_free,		// can be reused for a new connection
	cs_zombie,		// client has been disconnected, but don't reuse
					// connection for a couple seconds
	cs_connected,	// has been assigned to a client_t, but not in game yet
	cs_spawned		// client is fully in game
} client_state_t;

enum ScriptConditionT
{ 
	COND_READY,
	COND_COMPLETED, 
	COND_SUSPENDED, 
	COND_WAIT_ALL, 
	COND_WAIT_ANY,
	COND_WAIT_TIME,
};

//WHY I HAVE TO DO THIS LOL WAHT IS THIS NONSENSE!! XD
#include "../src/Game/qcommon/configstring.h"


#define stget(e,x) *(unsigned int*)(e+x)
#define stset(e,x,v) *(unsigned int*)(e+x) = v


// #define SV_CONFIGSTRINGS 0x203A2374

// #define SV_CONFIGSTRINGS 0x00732500
// #define SV_CONFIGSTRINGS 0x0824A34E
#define SV_CONFIGSTRINGS 0x82AFAD4


/*

	client_state_t	state;
	char			userinfo[MAX_INFO_STRING];		// name, etc
	int				lastframe;			// for delta compression
	usercmd_t		lastcmd;			// for filling in big drops
	int				commandMsec;		// every seconds this is reset, if user									// commands exhaust it, assume time cheating
	int				frame_latency[LATENCY_COUNTS];
	int				ping;
	int				message_size[RATE_MESSAGES];	// used to rate drop packets
	int				rate;
	int				surpressCount;		// number of messages rate supressed
	edict_t			*edict;				// EDICT_NUM(clientnum+1)
	char			name[32];			// extracted from userinfo, high bits masked
	int				messagelevel;		// for filtering printed messages
	// The datagram is written to by sound calls, prints, temp ents, etc.
	// It can be harmlessly overflowed.
	sizebuf_t		datagram;
	byte			datagram_buf[MAX_MSGLEN];
	client_frame_t	frames[UPDATE_BACKUP];	// updates can be delta'd from here
	byte			*download;			// file being downloaded
	int				downloadsize;		// total bytes (can't use EOF because of paks)
	int				downloadcount;		// bytes sent
	int				lastmessage;		// sv.framenum when packet was last received
	int				lastconnect;
	int				challenge;			// challenge of this user, randomly generated
	netchan_t		netchan;
*/
#define SV_CLIENT 0x203FEC94 //pointer to current player being parsed
#define CLIENT_BASE 0x20396EEC
#define SIZE_OF_CLIENT 0xd2ac
#define CLIENT_USERINFO 0x04
#define CLIENT_ENT 0x298
#define CLIENT_NETMESSAGE 0x52b5 //could be wrong i think its 0x52b4
#define CLIENT_NETCHAN 0x526C
#define CLIENT_NETCHAN_IP 0x5284
#define CLIENT_MSEC 0x220

#define STUFFTEXT 0xD


/*
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
	vec3_t						damage_from;							// origin for vector calculatio						
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
*/
#define GCLIENT_BASE 0x5015D6C4
#define GCLIENT_BASE_LINUX 0x5015D6C4
#define SIZE_OF_GCLIENT 0x600
#define GCLIENT_PS_BOD 0x7C
#define GCLIENT_TEAM 0x324
#define GCLIENT_INV 0x34C
#define GCLIENT_BODY 0x354
#define GCLIENT_GHOSTED 0x554
#define GCLIENT_PERS_SPECTATOR 0x2DC
#define GCLIENT_PERS_NETNAME 0x2CC
#define GCLIENT_PERS_CONNECTED 0x2F8
#define GCLIENT_RESP_SPECTATOR 0x320
#define GCLIENT_RESP_SCORE 0x308
#define GCLINT_RESP_ENTERFRAME 0x304
#define GCLIENT_RESP_CTFTEAM 0x380
#define GCLIENT_PING 0xC8
#define GCLIENT_SHOWSCORES 0x470
#define GCLIENT_MOVESCALE 0x348 //other movescale is 26 offset , doesnt set
#define GCLIENT_CHASETARGET 0x328

#define GCLIENT_LATCHED_BUTTONS 0x484
#define GCLIENT_BUTTONS 0x47C
#define GCLIENT_PS_PM_DELTA 0x14
#define GCLIENT_PS_PM_ORIGIN 0x4


#define SIZE_OF_EDICT 0x464
#define EDICT_BASE 0x5015CCA0

#define EDICT_GCLIENT 0x74
#define EDICT_HEALTH 0x2EC
#define EDICT_SOLID 0x158
#define EDICT_CLASSNAME 0x1B4
#define EDICT_TARGETNAME 0x1C8
#define EDICT_SCRIPT 0x454
#define EDICT_S_ORIGIN 0x4
#define EDICT_S_ANGLES 0x10
#define EDICT_INUSE 0x78
#define EDICT_GHOULINST 0x164
#define EDICT_PS_GUN 0x6C
#define EDICT_CLIPMASK 0x15C
#define EDICT_MINS 0x11C
#define EDICT_MAXS 0x128
#define EDICT_MODEL 0x1A4
#define EDICT_S_SKINNUM 0x30
#define EDICT_ENEMY 0x3240
#define EDICT_CTFFLAGS 0x3CC
#define EDICT_SURFACETYPE 0x3C0
#define EDICT_MATERIAL 0x31C
#define EDICT_TOUCH 0x2C0
#define EDICT_USE 0x2C4
#define EDICT_PLUSE 0x2C8
#define EDICT_PAIN 0x2CC
#define EDICT_DIE 0x2D0
#define EDICT_THINK 0x2B8
#define EDICT_NEXTTHINK 0x2B4
#define EDICT_FLAGS 0x1A0
#define EDICT_COUNT 0x318
#define EDICT_DELAY 0x364
#define EDICT_OWNER 0x160


#define CSCRIPT_SCRIPTCONDITION 0x108

#include "../src/Game/gamecpp/g_local.h"


// #include "../src/Game/gamecpp/g_obj.h"

extern bool GhoulGetInst(int slot);
extern void GhoulSetTint(float r,float g,float b,float alpha);
extern void GhoulSetTintOnAll(float r,float g,float b,float alpha);
extern void GhoulAddNoteCallBack(IGhoulCallBack *c,GhoulID Token=0);
extern GhoulID GhoulFindSequence(const char *Filename);
extern bool GhoulPlay(GhoulID Seq,float Now,float PlayPos,bool Restart,IGhoulInst::EndCondition ec, bool MatchCurrentPos, bool reverseAnim);
extern unsigned int * ghoulmain;
extern unsigned int * clientinst;
extern void GhoulGetXform(Matrix4 *m);
extern void GhoulSetXform(Matrix4 *m);
extern unsigned short GhoulFindPart(const char * partname);
extern bool GhoulGetObject(void);
extern bool GhoulInstFromID(unsigned short ID);
extern void PrintFunctionAddr(void);
extern bool GhoulBoltMatrix(float Time,Matrix4 &m,unsigned short GhoulID,MatrixType kind,bool ToRoot);
extern void GhoulBoundBox(float Time,const Matrix4 &ToWorld,Vect3 &mins,Vect3 &maxs,GhoulID Part,bool Accurate);
extern ggObjC *GhoulFindObject(const char* name, const char* subname, bool allSkins=true, const char *skinname=NULL, const char *basefile=NULL, bool dontMakeNew=false);
extern ggObjC *GhoulFindObjectSmall(IGhoulObj *curObject);
extern GhoulID GhoulFindNoteToken(const char *Token);

//from matrix.cpp lol i modified it for these, from gutils.cpp lol sofsdk
extern void EntToWorldMatrix(vec3_t org, vec3_t angles, Matrix4 &m);
extern int GetGhoulPosDir(vec3_t sourcePos, vec3_t sourceAng, GhoulID partID, vec3_t pos, vec3_t dir, vec3_t right, vec3_t up);
extern void Vec3AddAssign(vec3_t value, vec3_t addTo);
// extern void VectorCopy(vec3_t in, vec3_t out);
extern void VectorScale (vec3_t in, vec_t scale, vec3_t out);
extern void AngleVectors (vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);


