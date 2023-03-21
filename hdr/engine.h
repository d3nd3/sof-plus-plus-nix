/*typedef char bool;
#define false 0x00
#define true 0x01
*/
#pragma once

extern unsigned int * ghoulmain;
extern unsigned int * clientinst;
extern unsigned int * objinst;


extern char SOFREESP[32768];
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


#define SV_CONFIGSTRINGS 0x203A2374


#define SV_CLIENT 0x203FEC94 //pointer to current player being parsed
#define CLIENT_BASE 0x20396EEC
#define SIZE_OF_CLIENT 0xd2ac
#define CLIENT_USERINFO 0x04
#define CLIENT_ENT 0x298
#define CLIENT_NETMESSAGE 0x52b5 //could be wrong i think its 0x52b4
#define CLIENT_NETCHAN 0x526C
#define CLIENT_NETCHAN_IP 0x5284
#define CLIENT_MSEC

#define STUFFTEXT 0xD

#define GCLIENT_BASE 0x5015D6C4
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
#define GCLIENT_PING 0xC8
#define GCLIENT_SHOWSCORES 0x470
#define GCLIENT_MOVESCALE 0x348 //other movescale is 26 offset , doesnt set
#define GCLIENT_CHASETARGET 0x328

#define GCLIENT_LATCHED_BUTTONS 0x484
#define GCLIENT_BUTTONS 0x47C


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


