// m_heliai.h

#ifndef M_HELIAI_H
#define M_HELIAI_H

////// kef -- 2/18/99 originally stored in m_heli.h //////////////

// these make it a more responsive, though necessarily loses some fluidity to do so...
//
#if 0
//
// This is the original behaviour-control define block, graceful but slow...
//
#define HELI_COBRA_MAXSPEED				40.0f	
#define HELI_COBRA_ACCEL				((float)((HELI_COBRA_MAXSPEED/10.0f)/5.0f))		// ie takes 5 seconds to reach max speed
#define HELI_COBRA_IDEAL_LANDING_HEIGHT (56*4)	// roughly 4 man heights above landing pad, before beginning vert descent
#define HELI_COBRA_MAXPITCH				45.0f
#define HELI_COBRA_PITCHADJSPEED		((float)((HELI_COBRA_MAXPITCH/10.0f)/4.0f))	// ie takes 4 seconds to reach max pitch
#define HELI_COBRA_MAXPITCH_DECEL		(HELI_COBRA_MAXPITCH/3)	
#define HELI_COBRA_PITCHADJSPEED_DECEL	((float)((HELI_COBRA_MAXPITCH_DECEL/10.0f)/4.0f))	// ie takes 4 seconds to reach max pitch
#define HELI_COBRA_MAXROLL				20	
#define HELI_COBRA_MAXROLL_DECEL		8
//
#define HELI_MAX_FIRE_PITCH 40
#define HELI_COBRA_DIPFIRE_PITCHADJSPEED 4	// faster than normal pitch adjust speed
#define HELI_COBRA_ROLLADJSPEED_LEVELOUT 4
#define HELI_YAWADJ_SPEED (9/2)	// value of 9 @ 10fps = 1 second to turn 90 degrees, now halved for playability
#define HELI_ROLLADJ_SPEED  1	//2
#define HELISTRAFE_ROLLADJ_SPEED 2
#define HELI_MAX_LANDING_SPEED 10.0f
#define HELI_COBRA_MIN_HEAD2DESTSPEED 1

#else
//
// this version provides a much more responsive (for gameplay pace), but less realistic, heli behaviour...
//
#define HELI_COBRA_MAXSPEED				80.0f	
#define HELI_COBRA_ACCEL				((float)((HELI_COBRA_MAXSPEED/10.0f)/3.0f))		// ie takes 3 seconds to reach max speed
#define HELI_COBRA_IDEAL_LANDING_HEIGHT (56*4)	// roughly 4 man heights above landing pad, before beginning vert descent
#define HELI_COBRA_MAXPITCH				45.0f
#define HELI_COBRA_PITCHADJSPEED		((float)((HELI_COBRA_MAXPITCH/10.0f)/3.0f))	// ie takes 3 seconds to reach max pitch
#define HELI_COBRA_MAXPITCH_DECEL		(HELI_COBRA_MAXPITCH/3)	
#define HELI_COBRA_PITCHADJSPEED_DECEL	((float)((HELI_COBRA_MAXPITCH_DECEL/10.0f)/2.0f))	// ie takes 2 seconds to reach max pitch
#define HELI_COBRA_MAXROLL				40	
#define HELI_COBRA_MAXROLL_DECEL		16
//
#define HELI_MAX_FIRE_PITCH 40
#define HELI_COBRA_DIPFIRE_PITCHADJSPEED 5	// faster than normal pitch adjust speed
#define HELI_COBRA_ROLLADJSPEED_LEVELOUT 4
#define HELI_YAWADJ_SPEED (9)	// value of 9 @ 10fps = 1 second to turn 90 degrees, now halved for playability
#define HELI_ROLLADJ_SPEED  2
#define HELISTRAFE_ROLLADJ_SPEED 3.5
#define HELI_MAX_LANDING_SPEED 10.0f
#define HELI_COBRA_MIN_HEAD2DESTSPEED 2

#endif

	
// fixme:Ghoul has a slight bug in that the model angles are the wrong way round, so for now...
// update: actually i think it's because the heli model seems to have a 90 degree rotation glitch or something.
// Sod it, these work anyway...
//
#define HELI_PITCH	PITCH
#define HELI_ROLL	ROLL
#define HELI_YAW	YAW


// return values for fns that aim heads or the gun
#define AIM_TRUE	1
#define AIM_FALSE	2
#define AIM_ERROR	3
// let me know if any other AIM_xxx defines are added - Ste.


// radians a head or gun may turn per think
#define AIM_MAX_TURN_DELTA	(M_PI/10)
#define AIM_MAX_TURN_EPSILON	0.01

// gunner/pilot head restrictions
#define HEAD_PITCH_UP	(M_PI/4.0)
#define HEAD_PITCH_DOWN	0
#define HEAD_YAW_CW		(M_PI*0.5)
#define HEAD_YAW_CCW	(-M_PI*0.5)
#define GUN_PITCH_UP	(M_PI/8.0)
#define GUN_PITCH_DOWN	(M_PI/4.0)
#define GUN_YAW_CW		(1)		// max yaw (clockwise looking down on heli)
#define GUN_YAW_CCW		(-1)	// min yaw (clockwise looking down on heli)

#define HEAD_PITCH_UP_DEG		(HEAD_PITCH_UP*180.0f/M_PI)
#define HEAD_PITCH_DOWN_DEG		(HEAD_PITCH_DOWN*180.0f/M_PI)
#define HEAD_YAW_CW_DEG			(HEAD_YAW_CW*180.0f/M_PI)
#define HEAD_YAW_CCW_DEG		(HEAD_YAW_CCW*180.0f/M_PI)
#define GUN_PITCH_UP_DEG		(GUN_PITCH_UP*180.0f/M_PI)
#define GUN_PITCH_DOWN_DEG		(GUN_PITCH_DOWN*180.0f/M_PI)
#define GUN_YAW_CW_DEG			(GUN_YAW_CW*180.0f/M_PI)
#define GUN_YAW_CCW_DEG			(GUN_YAW_CCW*180.0f/M_PI)



// Info: defines marked with a '#' have to insert their own pseudo-scriptevents into the script queue 
//	(typically 'goto'/'turnto' instructions) in order to accomplish their tasks.
//
#define HELIFLAG_DOINGSCRIPTEDLANDING	0x00000001	// #
#define HELIFLAG_ROTORSCHANGINGSPEED	0x00000002
#define HELIFLAG_SIDESTRAFEISTOLEFT		0x00000004
#define HELIFLAG_ROCKETSENABLED			0x00000008
#define HELIFLAG_CHAINGUNENABLED		0x00000010
#define HELIFLAG_MGUN_GHOULSEQ_RUNNING	0x00000020	// special use thingy
#define HELIFLAG_AUTOFIRE				0x00000040
#define HELIFLAG_DOINGSCRIPTEDHOVER		0x00000080	// #
#define HELIFLAG_HOVERISAGGRESIVE		0x00000100
#define HELIFLAG_DOINGSCRIPTEDFIREAT	0x00000200	// #
#define HELIFLAG_DOINGSMARTGOTO			0x00000400	// #
#define HELIFLAG_FIREDIP1DONE			0x00000800	//  space-saver bool
#define HELIFLAG_FIREDIP2DONE			0x00001000	//  ""
#define HELIFLAG_SCRIPTENDMENTIONED		0x00002000
#define HELIFLAG_ISDEAD					0x00004000
#define HELIFLAG_ISDEADACKNOWLEDGED		0x00008000
#define HELIFLAG_ISDEADFINALGOTO		0x00010000
#define HELIFLAG_PLAYINGHOVERANIM		0x00020000
#define HELIFLAG_NOTRACEDIMS			0x00040000
#define HELIFLAG_DOINGSCRIPTEDAI		0x00080000
#define HELIFLAG_PILOTDEAD				0x00100000
#define HELIFLAG_GUNNERDEAD				0x00200000
#define HELIFLAG_SCRIPTEDAIJUSTFINISHED	0x00400000	// special usage, so AI-hover knows to cut short if main AI timed out

#define HELI_MAX_ROCKETS 8	// 4 per tube, 8 fits the graphic so don't alter it, use reloads if you want more


// Heli spawn flags...
//
#define IN_AIR				1
#define ONDIE_HEAD2SPAWN	2
#define INFINITE_ROCKETS	4
#define DEBUG_WAYPOINTS		8
#define ATTACK_CHOPPER		16
#define HELI_ALWAYS_SEEN	32


// stuff for Nathan's moving terrain...
//
#define HELI_MAX_TERRAIN_LEAN 10	// 45
#define MAX_GROUND_SPEED 200

// kept as defines just in case I ever need to change them all back at once..
//	
#define _HELI_TRACEPARAM_MIN (dwHeliFlags&HELIFLAG_NOTRACEDIMS)?NULL:v3HeliTraceMins	// self->mins
#define _HELI_TRACEPARAM_MAX (dwHeliFlags&HELIFLAG_NOTRACEDIMS)?NULL:v3HeliTraceMaxs	// self->maxs

#define _HELI_TRACEPARAM_MIN2 (body->m_flags&HELIFLAG_NOTRACEDIMS)?NULL:body->m_v3HeliTraceMins	// self->mins
#define _HELI_TRACEPARAM_MAX2 (body->m_flags&HELIFLAG_NOTRACEDIMS)?NULL:body->m_v3HeliTraceMaxs	// self->maxs


// ensure angle is in range -179..0..180
//
_inline float angle2posneg (float _angle)
{
	_angle = anglemod(_angle);	// ensure known-range
	if (_angle>180)
		_angle-=360;

	return _angle;
}
//////////////////////////////////////////////////////////////////

void debug_drawbox(edict_t* self,vec3_t vOrigin, vec3_t vMins, vec3_t vMaxs, int nColor);

void Cobra2Think_TakeOff(edict_t *self);
void Cobra2_InitTurnStuff(edict_t *self, vec3_t v3);
void Cobra2Think_HeadToDest(edict_t *self);
int Cobra2AimHeadAtPos(edict_t *self, vec3_t vTarget, IGhoulInst* head, GhoulID bolt,
					   vec3_t vOffset);
int Cobra2AimGunnerAtPos(edict_t *self, vec3_t vTarget);
int Cobra2AimPilotAtPos(edict_t *self, vec3_t vTarget);
void Cobra2Think_MovementFinish(edict_t *self);
qboolean Cobra2MovementFinished(edict_t *self);
qboolean WithinOrBeyondVectors2(vec3_t v3Me, vec3_t v3Halfway, vec3_t v3End);
void Cobra2FireRockets(edict_t *self, int nLeftRockets, int nRightRockets);
void Cobra2FireGun(edict_t *self, qboolean bSporadic);
void Cobra2FireGun_Off(edict_t *self);
qboolean Cobra2_EnsureRollFinished(edict_t *self);
int Cobra2ResetHead(edict_t *self, IGhoulInst* head, GhoulID bolt, vec3_t vOffset);
int Cobra2ResetPilot(edict_t *self);
int Cobra2ResetGunner(edict_t *self);
qboolean Cobra2TargetWithinArc(vec3_t vTargetPos, vec3_t vHeliAngles, vec3_t vHeliPos);
qboolean Cobra2HasTargetLOS(edict_t *self, edict_t *target, qboolean qbAccountForArc);
int Cobra2AimGunAtPos(edict_t *self, vec3_t vTarget, qboolean bSynchGunner);
int Cobra2AimGunInDirection(edict_t *self, vec3_t vTargetDir, qboolean bSynchGunner);
void fireHeliRocket2(edict_t *self, vec3_t v3FirePoint, vec3_t v3FireDir);
int Cobra2AimGunnerInDirection(edict_t *self, vec3_t vTarget);
int Cobra2AimPilotInDirection(edict_t *self, vec3_t vTarget);
void Cobra2InitHead(edict_t *self, vec3_t vTargetDir, IGhoulInst* head, 
							 GhoulID bolt, vec3_t vOffset);
int CobraResetGunner(edict_t *self);
void Cobra2Think_SetFireAt(edict_t *self, vec3_t v3FireDest, float fTime);
void Cobra2Think_GotoFireAt(edict_t *self);
void Cobra2Think_FireAt(edict_t *self);
void Cobra2Think_DipFireAt(edict_t *self);
void Cobra2Think_SetDumbFlyDest(edict_t *self, vec3_t v3);
void Cobra2Think_SetSmartFlyDest(edict_t *self, vec3_t v3);
void Cobra2Think_SmartHeadToDest(edict_t *self);
void Cobra2Think_SetTurnDest(edict_t *self, vec3_t v3);
void Cobra2Think_SetSidestrafeDest(edict_t *self, float fXStrafeDist);
void Cobra2Think_Hover(edict_t *self);
void Cobra2Think_AI(edict_t *self);
void Cobra2_SetAIParams(edict_t *self, float fTime);
void Cobra2_SetHoverParams(edict_t *self, float fTime, qboolean qbAggressive);
void Cobra2Think_SidestrafeToDest(edict_t *self);
void Cobra2Think_Land(edict_t *self);
void Cobra2Think_OnPad(edict_t *self);
void Cobra2Think_Repair(edict_t *self);
void Cobra2Think_TurnToFace(edict_t *self);
qboolean Cobra2_TurnToFace(edict_t *self, vec3_t v3WorldPos, qboolean qbRoll, qboolean qbYAWDictatesFiring, float *pfDegreesToGo);
vec3_t *Cobra2Think_GetRandomPatrolDestPos(edict_t *self);
void Cobra2Think_SetPilotlookDest(edict_t *self, vec3_t v3);
void Cobra2Think_SetGunnerlookDest(edict_t *self, vec3_t v3);
void Cobra2Think_PilotLookat(edict_t *self);
void Cobra2Think_GunnerLookat(edict_t *self);
int  _Cobra2Fire_Rockets(edict_t *self, int iRocketsToForceFire);
void _Cobra2Fire_Chaingun(edict_t *self);
int  Cobra2_Fire(edict_t *self, qboolean qbInihibitRockets);
void Cobra2_Fire_OFF(edict_t *self);
void Cobra2_Explode(edict_t *self);
qboolean Cobra2_VecWithinWorld(edict_t *self, vec3_t v3);
void Cobra2_LegaliseVector(edict_t *self, vec3_t v3in, vec3_t v3Out, qboolean qbUseWorldClip);
void Cobra2_ClipVecToWorld(edict_t *self, vec3_t v3);
void Cobra2_TerrainSpeed2GhoulAngles(edict_t *self, vec3_t v3GroundSpeed);
void Cobra2_OutOfControl(edict_t* self);
void Cobra2Touch(edict_t *self, edict_t *other, cplane_t *plane, struct mtexinfo_s *surf);
qboolean Cobra2TargetEntValid(edict_t *self, edict_t *target);
edict_t *Cobra2_GetEntToTarget(edict_t *self);

// helper fns for movement thinks
void Cobra2LandHelper(edict_t *self);
void Cobra2GotoCoordsHelper(edict_t *self);
void Cobra2GotoRelEntHelper(edict_t *self);
void Cobra2GotoRelHelper(edict_t *self);
void Cobra2StrafeHelper(edict_t *self);
void Cobra2HoverHelper(edict_t *self);
void Cobra2HamburgerHelper(edict_t *self);
void Cobra2FaceRelEntHelper(edict_t* self);
void Cobra2FaceAbsHelper(edict_t* self);
void Cobra2FaceAbsDirHelper(edict_t* self);
void Cobra2FaceRelCoordsHelper(edict_t* self);
void Cobra2HeadFaceRelEntHelper(edict_t* self);
void Cobra2HeadFaceRelCoordsHelper(edict_t* self);
void Cobra2HeadFaceAbsCoordsHelper(edict_t* self);
void Cobra2FireAtRelEntHelper(edict_t* self);
void Cobra2FireAtAbsHelper(edict_t* self);
void Cobra2FireAtRelHelper(edict_t* self);
void Cobra2RocketsEnableHelper(edict_t* self);
void Cobra2ChainGunEnableHelper(edict_t* self);
void Cobra2AutoFireHelper(edict_t* self);
void Cobra2WorldHelper(edict_t* self);
void Cobra2RearmHelper(edict_t* self);
void Cobra2DeathDestHelper(edict_t* self);
void Cobra2HealthHelper(edict_t* self);
void Cobra2TraceDimsHelper(edict_t* self);
void Cobra2AIHelper(edict_t* self);


class heliWaypoint
{
public:
	vec3_t			m_v;
	vec3_t			m_volMins;
	vec3_t			m_volMaxs;
	heliWaypoint*	m_next;

	heliWaypoint(){ VectorClear(m_v); VectorClear(m_volMins); VectorClear(m_volMaxs); m_next = NULL;}
	heliWaypoint(vec3_t v){ VectorCopy(v, m_v); VectorClear(m_volMins); VectorClear(m_volMaxs); m_next = NULL;}
	virtual ~heliWaypoint(){}
	heliWaypoint(heliWaypoint *orig);

	// the mins and maxs for a waypoint are in world coords, not relative to m_v
	void SetVolume(vec3_t vMins, vec3_t vMaxs) {VectorCopy(vMins, m_volMins);VectorCopy(vMaxs, m_volMaxs);}
	void GetVolumeMins(vec3_t vRet) { VectorCopy(m_volMins, vRet); }
	void GetVolumeMaxs(vec3_t vRet) { VectorCopy(m_volMaxs, vRet); }
	qboolean IsPoint() { if (m_volMins[0] || m_volMins[1] || m_volMins[2] || m_volMaxs[0] || m_volMaxs[1] || m_volMaxs[2]) return false; return true;}
};

class generic_ghoul_heli_ai : public ai_c
{
// findme: helicommands
public:
enum hse_Commands
		{
		hse_NONE=0,					  //
		hse_TAKEOFF,               	  //									// should only be called when you know heli is landed
		hse_LAND,					  //									// can be called from anywhere in map (when in air)
		hse_REPAIR,					  //									// it'd be better to do this on pad, but in truth can even be called in-flight
		hse_REARM,					  //									// refills heli with max rockets, instantly (see also INFINITE_ROCKETS spawn flag)
		hse_GOTO_COORDS,			  //<vector>
		hse_GOTOREL_ENTITY,			  //<vector>	<entity>
		hse_GOTOREL_ENT_X,			  // 			<entity>	<float>
		hse_GOTOREL_ENT_Y,			  //			<entity>	<float>
		hse_GOTOREL_ENT_Z,			  //			<entity>	<float>
		hse_MOVEREL,				  //<vector>
		hse_PAUSE,					  //						<float>		// script pause supercedes this
		hse_FACE_RELENT,			  //<vector>	<entity>
		hse_FACE_ABSCOORDS,			  //<vector>
		hse_FACE_ABSDIR,			  //<vector>							face a direction given relative to the world
		hse_FACE_RELCOORDS,			  //<vector>
		hse_PILOT_FACERELENT,		  //<vector>	<entity>
		hse_PILOT_FACERELCOORDS,	  //<vector>
		hse_PILOT_FACEABSCOORDS,	  //<vector>
		hse_GUNNER_FACERELENT,		  //<vector>	<entity>
		hse_GUNNER_FACERELCOORDS,	  //<vector>
		hse_GUNNER_FACEABSCOORDS,	  //<vector>
		hse_STRAFE_RT,				  //						<float>
		hse_STRAFE_LT,				  // 						<float>
		hse_ROCKETS_ENABLE,			  //
		hse_ROCKETS_DISABLE,		  //
		hse_CHAINGUN_ENABLE,		  //
		hse_CHAINGUN_DISABLE,		  //
		hse_FIREAT_RELENT,			  //<vector>	<entity>	<float>
		hse_FIREAT_ABSCOORDS,		  //<vector>				<float>
		hse_FIREAT_RELCOORDS,		  //<vector>				<float>
		hse_AUTOFIRE_ON,			  //
		hse_AUTOFIRE_OFF,			  //
		hse_HOVER_PASSIVE,			  // 						<float>
		hse_HOVER_AGGRESSIVE,		  //						<float>
		hse_SET_WORLDMINS,			  //<vector>
		hse_SET_WORLDMAXS,			  //<vector>
		hse_SET_MAXHEALTH,			  //						<float>
		hse_SET_HEALTH,				  // 						<float>
		hse_SET_DEATHDEST,			  //<vector>							// set this to change default (pad) crash position for death
		hse_SET_TRACEDIMS,			  //						<float>		// arg is a 0/1 bool. set to 0 to turn off trace-checking in flight if you want to script for a really close skim
		hse_AI,						  //						<float>		// arg is time in seconds to do internal AI before reading next script command. Use 0 as arg to end a script if you want to wait for another script to start
		hse_WAYPOINT,				  //<vector>							// hint position for heli to fire from during internal AI
		hse_VOLUME,					  //<vector>							// use this straight after WAYPOINT arg to set dims (transforms a waypoint into a suggested-flying-area volume). Args are radius,s eg Z 150 would be 300 high
		hse_VOLUMEMINS,					  //<vector>							// use this straight after WAYPOINT arg to set dims (transforms a waypoint into a suggested-flying-area volume). Arg is in world coords
		hse_VOLUMEMAXS,					  //<vector>							// use this straight after WAYPOINT arg to set dims (transforms a waypoint into a suggested-flying-area volume). Arg is in world coords
		hse_DEBUG,
		hse_CHANGE_SKIN,
		hse_CHANGE_BODY,
		hse_OUT_OF_CONTROL			  //									// Keith's internal use only, do not use in scripts
		};

protected:
#define GGHA_SAVE_START	offsetof(generic_ghoul_heli_ai, m_bActionDone)
	qboolean	m_bActionDone;
	qboolean	m_bTimeToDie;
	qboolean	m_bPilotDead;
	qboolean	m_bGunnerDead;
	edict_t*	m_attacker;
	int			m_ScriptActionCounter;
	int			m_LastThinkingActionID;

	static float fHeliXWorldMin2;
	static float fHeliXWorldMax2;
	static float fHeliYWorldMin2;
	static float fHeliYWorldMax2;
	static float fHeliZWorldMin2;
	static float fHeliZWorldMax2;



	// kef -- hate exposing this, but the dumb scripted ai stuff needs it
	int				m_nActionSize;

	heliWaypoint*	m_waypoints;
	int				m_nNumWaypoints;
	int				m_nNumWaypointPts;
	int				m_nNumWaypointVols;
#define GGHA_SAVE_END	(offsetof(generic_ghoul_heli_ai, m_nNumWaypointVols) + sizeof(m_nNumWaypointVols))
	virtual void	AddBody(edict_t *monster);
	virtual void	Pain(edict_t &monster, edict_t *other, float kick, int damage);
	virtual void	Die(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);

	virtual void	CrewMemberKilled(edict_t* self, int nSkin);
	virtual void	BuildHeliModel(edict_t *self, char *ghoulname, char* subclass);

public:
					generic_ghoul_heli_ai() {}
	virtual	void	Init(edict_t *self, char *ghoulname, char* subclass);
	virtual ~generic_ghoul_heli_ai();
	virtual int		GetClassCode(void){return AI_HELI;}//this should be different for everybody in this class family
	virtual	void	Activate(edict_t &monster);
	inline virtual body_heli			*GetHeliBody(void){return (body_heli*)(body_c*)body;}

	static float GetHeliXWorldMin2() { return fHeliXWorldMin2; }
	static float GetHeliXWorldMax2() { return fHeliXWorldMax2; }
	static float GetHeliYWorldMin2() { return fHeliYWorldMin2; }
	static float GetHeliYWorldMax2() { return fHeliYWorldMax2; }
	static float GetHeliZWorldMin2() { return fHeliZWorldMin2; }
	static float GetHeliZWorldMax2() { return fHeliZWorldMax2; }
	static void SetHeliXWorldMin2(float val) {fHeliXWorldMin2 = val;}
	static void SetHeliXWorldMax2(float val) {fHeliXWorldMax2 = val;}
	static void SetHeliYWorldMin2(float val) {fHeliYWorldMin2 = val;}
	static void SetHeliYWorldMax2(float val) {fHeliYWorldMax2 = val;}
	static void SetHeliZWorldMin2(float val) {fHeliZWorldMin2 = val;}
	static void SetHeliZWorldMax2(float val) {fHeliZWorldMax2 = val;}

	qboolean IsActionDone() { return m_bActionDone; }
	void ActionIsDone(qboolean bDone) { m_bActionDone = bDone; }
	qboolean IsTimeToDie() { return m_bTimeToDie; }
	void TimeToDie(qboolean bTime) { m_bTimeToDie = bTime; }
	void SetCurrentActionThink(void	(*think)(edict_t *self));
	void SetCurrentActionNextThink(void	(*think)(edict_t *self));
	int GetActionSize() { return m_nActionSize; }

	// return something other than 1 so we can assassinate Amu
	virtual int	 GetTeam(edict_t &monster){return 10;}
	virtual bool				AmIAsGoodAsDead() {return false;}

	virtual	void Think(edict_t &monster);
	virtual qboolean Damage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb);

	virtual edict_t* GetAttacker() { return m_attacker; }
	void PilotCapped() { m_bPilotDead = true; }
	void GunnerCapped() { m_bGunnerDead = true; }

	virtual action_c *MoveAction(decision_c *od, action_c *oa, ai_c* ai,
		mmove_t *newanim, int nCommand, vec3_t vPos, edict_t* target, float fArg);
	virtual action_c *FaceAction(decision_c *od, action_c *oa, ai_c* ai,
		mmove_t *newanim, int nCommand, vec3_t vPos, edict_t* target, float fArg);
	virtual action_c *AttackAction(decision_c *od, action_c *oa, ai_c* ai,
		mmove_t *newanim, int nCommand, vec3_t vPos, edict_t* target, float fArg);

	int GetNumWaypoints() { return m_nNumWaypoints; }
	int GetNumWaypointPts() { return m_nNumWaypointPts; }
	int GetNumWaypointVols() { return m_nNumWaypointVols; }
	// returns number of waypoints (points and volumes) in list
	int AddWaypoint(vec3_t v); 
	// returns false if our bbox can't fit in the given volume.
	//nWay is the index of the pt that's becoming a volume.
	//v is the maxs for the volume, -v will be the volume's mins.
	qboolean SetWaypointPtVolume(int nWay, vec3_t v);

	heliWaypoint* GetWaypointPt(int nWay);
	heliWaypoint* GetWaypointVol(int nWay);

	int GetCurrentActionID();
	int GetMostRecentlyAddedActionID();
	int GetLastThinkingActionID() { return m_LastThinkingActionID; }

	void	ChangeBody(edict_t *self, char *ghoulname, char* subclass);

				 generic_ghoul_heli_ai(generic_ghoul_heli_ai *orig);
			void Evaluate(generic_ghoul_heli_ai *orig);
	virtual void Write(void);
	virtual void Read(void);
};

#endif	// #ifndef M_HELIAI_H
/////////////////////// eof ////////////////////////////

