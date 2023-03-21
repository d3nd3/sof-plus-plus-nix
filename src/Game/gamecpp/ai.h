#ifndef __AI_H
#define __AI_H

void		vectoangles(vec3_t in, vec3_t out);//urk. the body of the stuff that uses this should be in .cpp, but for now, it's more convenient in here

#define SPAWNFLAG_ECON_OVERRIDE		(1<<0)
#define SPAWNFLAG_BLIND				(1<<1)
#define SPAWNFLAG_DEAF				(1<<2)
#define SPAWNFLAG_NOSCALE			(1<<3)
#define SPAWNFLAG_TRIGGER_SPAWN		(1<<4)
#define	SPAWNFLAG_SENSE_MUTE		(1<<5)
#define SPAWNFLAG_START_ACTIVE		(1<<6)
#define SPAWNFLAG_HOLD_POSITION		(1<<7)
//
// bits 8 through 12 (0-indexed) are used in g_local.h for difficulty settings, deathmatch, and coop
//
#define SPAWNFLAG_NEVER_STARTLED	(1<<13)
#define SPAWNFLAG_HOSTAGE			(1<<14)	
#define SPAWNFLAG_NO_ARMOR			(1<<15)	
#define SPAWNFLAG_ARMOR_PICKUP		(1<<16)
#define SPAWNFLAG_NO_WEAPONS		(1<<17)
#define SPAWNFLAG_NO_WOUND			(1<<18)
#define SPAWNFLAG_HAS_PROJECTILE	(1<<19)
#define SPAWNFLAG_FACE_PLAYER		(1<<20)

#define MISC_FLAG_FIRINGBLIND		0x00000001

//
// Rick sez we've got up to bit 20
//


#define AIS_NODROPWEAPON			0x00000001
#define AIS_WONTADVANCE				0x00000002
#define AIS_NOKNOCKOVER				0x00000004
#define AIS_NOATTACKONRETREAT		0X00000008
#define AIS_NODUCK					0x00000010
#define AIS_NOSUPPRESS				0x00000020
#define AIS_CHEATERSEARCH			0x00000040
#define AIS_ATTACKONLYINRANGE		0x00000080

typedef enum
{
	LEVCODE_UNKNOWN,
	LEVCODE_TUTORIAL,
	LEVCODE_NYC_SUBWAY,
	LEVCODE_NYC_SUBWAY2,
	LEVCODE_AFR_TRAIN,
	LEVCODE_KOS_SEWER,
	LEVCODE_KOS_BIGGUN,
	LEVCODE_KOS_HANGAR,
	LEVCODE_SIB_CANYON,
	LEVCODE_SIB_BASE,
	LEVCODE_SIB_PLANT,
	LEVCODE_IRQ_TOWNA,
	LEVCODE_IRQ_BUNKER,
	LEVCODE_IRQ_CARGO,
	LEVCODE_NYC_WARE,
	LEVCODE_NYC_STEAM,
	LEVCODE_NYC_STREETS,
	LEVCODE_AFR_YARD,
	LEVCODE_AFR_HOUSE,
	LEVCODE_AFR_FACT,
	LEVCODE_TOK_STREET,
	LEVCODE_TOK_OFFICE,
	LEVCODE_TOK_PENT,
	LEVCODE_IRQ_STREETS,
	LEVCODE_IRQ_FORT,
	LEVCODE_IRQ_OIL,
	LEVCODE_CAS_1,
	LEVCODE_CAS_2,
	LEVCODE_CAS_3,
	LEVCODE_CAS_4,
	LEVCODE_ARM_1,
	LEVCODE_ARM_2,
	LEVCODE_ARM_3,
} lev_interp_code;

//enemy styles
enum
{
	EST_AGGRESSIVE,
	EST_EVASIVE,
	EST_SNEAKY,
	EST_TACTICAL,
	EST_COWARDLY,
	EST_CAUTIOUS,
	EST_NUM_STYLES
};

//enemy style shot at
enum
{
	ESSA_RETREAT_TO_COVER,
	ESSA_FLEE_TO_COVER,
	ESSA_ATTACK_DIRECTION
};

//enemy style friends killed
enum
{
	ESFK_IGNORE,
	ESFK_RETREAT_TO_COVER,
	ESFK_RETREAT_TO_ALLIES,
	ESFK_RETREAT_TO_AMBUSH
};

//enemy style attacked
enum
{
	ESAT_DUCK,
	ESAT_APPROACH,
	ESAT_CIRCLE,
	ESAT_COVER_AND_ATTACK,
	ESAT_COVER_AND_AMBUSH
};

//enemy style out of range
enum
{
	ESOR_APPROACH,
	ESOR_HOLD_POSITION,
	ESOR_ATTACK_IF_POSSIBLE
};

//enemy style too close
enum
{
	ESTC_ROLL_TO_SIDE,
	ESTC_RETREAT,
	ESTC_HOLD_POSITION,
	ESTC_CIRCLE
};

//enemy style explosive
enum
{	//huh huh, huh huh
	ESEX_RETREAT,
	ESEX_DUCK,
	ESEX_DODGE,
	ESEX_IGNORE
};

typedef struct tempStyle_s
{
	int			shotAt;
	int			friendsKilled;
	int			attacked;
	int			outOfRange;
	int			tooClose;
	int			explosive;
}tempStyle_t;

class CAISkills
{
protected:
	float		idealRangeMin;
	float		idealRangeMax;
	float		dodgeAbility;
	float		shootingAccuracy;
	float		hesitation; // way to make guys less certain about attacking
	float		aimTime;
	float		cashValue;
	int			flags;
	float		turnSpeed;

	int			shotAt;
	int			friendsKilled;
	int			attacked;
	int			outOfRange;
	int			tooClose;
	int			explosive;
public:
	CAISkills(void);
	CAISkills(float rangeMin, float rangeMax, float dodge, float accuracy, 
		float newHesitation, float newAimTime, float newCashValue, float newTurnSpeed,float newFlags, 
			int newShotAt = 0, int newFriendsKilled = 0, int newAttacked = 0, int newOutOfRange = 0,
			int newTooClose = 0, int newExplosive = 0);

	void CAISkills::operator=(CAISkills &that);

	float		getRangeMin(void){return idealRangeMin;}
	float		getRangeMax(void){return idealRangeMax;}
	float		getDodge(void){return dodgeAbility;}
	float		getAccuracy(void){return shootingAccuracy;}
	float		getHesitation(void){return hesitation;}
	float		getAimTime(void){return aimTime;}
	float		getCashValue(void){return cashValue;}
	float		getDropWeap(void){return (flags & AIS_NODROPWEAPON) == 0;}
	float		getTurnSpeed(void){return turnSpeed;}
	int			testFlag(int flag){return (flags & flag);}

	// rangeMin and Max are in terms of world units, dodge, accuracy, mobility, tactics, and cowardice are from 0 to 1,
	// and aimMax is in world units.  absoluteMin is in world units.
	void		setSkills(float rangeMin, float rangeMax, float dodge, float accuracy, 
					float newHesitation, float newAimTime, float newCashValue, float newTurnSpeed, float newFlags);

	int			getShotAt(void){return shotAt;}
	int			getFriendsKilled(void){return friendsKilled;}
	int			getAttacked(void){return attacked;}
	int			getOutOfRange(void){return outOfRange;}
	int			getTooClose(void){return tooClose;}
	int			getExplosive(void){return explosive;}

	void		setStyles(int newShotAt, int newFriendsKilled, int newAttacked, int newOutOfRange,
					int newTooClose, int newExplosive);

	void		setStyles(int styleNum);
	void		InitStyles(edict_t *monster);

	void		adjustAccuracy(float val){shootingAccuracy *= val;}

};

// external table of skills so that guys know what they are
extern CAISkills enemySkills[];

typedef enum
{
	LINK_NORMAL,
	LINK_STAIR_UP,
	LINK_STAIR_DOWN,
	LINK_LADDER_UP,
	LINK_LADDER_DOWN,
	LINK_JUMP,
}connectType;

typedef struct nodeData_s
{
	vec3_t		curSpot;//used internally
	vec3_t		goPoint;
	int			curNode;		// node we're at
	int			curRegion;
	int			nextNode;		// node we're going to next
	int			finalNode;		// node that is ultimately our destination

	int			blocked;		// flag indicating we have been seriously impeded

	int			lastNode;		// I was here before
	int			lastNode2;		// and before
	int			lastNode3;		// and before
	
	int			backingUp;		// keep movements less jerky, please
	int			approaching;	//

	int			corner1;		// floors my corners are on
	int			corner2;
	int			corner3;
	int			corner4;

	vec3_t		lastDir;
	float		lastDirSetTime;
}nodeData_t;

lev_interp_code GetInterpCode(void);
void GetLevelSoundSuffix(char *putSuffixHere);

class ai_c: public ai_public_c
{
private:

	float	lFootPos, rFootPos;

	qboolean done_firstframe;

	float allowActionChangeTime;

	vec3_t	preset_mins[BBOX_PRESET_NUMBER];
	vec3_t	preset_maxs[BBOX_PRESET_NUMBER];
	bbox_preset		current_bbox;
	float			ground;
	qboolean		hasHadTarget;
	float			lastTargetTime;
	float			firstTargetTime;//when guys wake up, they need a bit of time to get the sleep out of their eyes, as it were...
	qboolean		abusedByTeam;
	float			lastNonTargetTime;//when guys search, set this.  Need it for aiming and such too

	float			nextWakeReactTime;

	// shared between all decisions
	nodeData_t		nodeData;

//	enemyWorldInfo_e	enemyInfo;
//	float				enemyInfoTime;

protected:
	edict_t *ent;

	edict_t *curTarget;

	action_c_ptr	current_action;
	action_c_ptr	recycle_action;
	//moved from mosterinfo: keeps track of whether my standing support has gone gone away
	int linkcount;

	qboolean		isActive;

	//when i first get a target, should i consider going through the startled sequence?
	qboolean		isStartleable;

	qboolean		m_bHostage;
	qboolean		m_bSpecialBuddy;
	qboolean		m_bConcentratingOnPlayer;

	vec3_t			requestedMoveDir;//for guys bumpn into each other.

	float	scale;
	int		sense_mask;
	float	jumpdistance;
	float	jumpheight;
	float	fallheight;
	float	stepheight;
	int		move_mask;

	int		miscFlags;

	priority_index	worldpriority;
	int				lastCheckTime;
	attention_index	attentionLevel;
	int				watchState;

	int				lastCorpseCheck;

	//the higher the rank, the more likely others will make room for me?
	int		rank;

	CAISkills		mySkills;

	body_c_ptr		body;
	gg_obj_c_ptr	MyGhoulObj;

	vec3_t			aim_angles;
	vec3_t			look_angles;
#define AI_SAVE_SIZE	(offsetof(ai_c, look_angles) + sizeof(look_angles))

	list<sense_c_ptr>		senses;
	list<action_c_ptr>		actions;
	list<decision_c_ptr>	decisions;

	virtual void	Pain(edict_t &monster, edict_t *other, vec3_t point, float kick, int damage);
	virtual void	Die(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags = 0);
	virtual void	Escape(edict_t &monster);
	void	InitBBoxPreset(bbox_preset preset_index, vec_t xMinVal, vec_t yMinVal, vec_t zMinVal,
				vec_t xMaxVal, vec_t yMaxVal, vec_t zMaxVal);
	virtual void	AddBody(edict_t *monster);

	virtual void	PickDeathAnim(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags = 0);
	
public:

			ai_c(edict_t *monster=NULL);
	virtual ~ai_c(void);

	virtual 		qboolean IsFirstFrameTime(void);
	virtual	void	SetEnt(edict_t *monster){ent=monster;}
	virtual	void	FirstFrame(edict_t *monster);
	virtual	void	Init(edict_t *monster, char *ghoulname="enemy/meso", char *subname="");

	//the extent to which aim can deviate from edict's angles (in degrees)
	virtual	float	GetAimConeDegrees(edict_t &monster, bbox_preset bbox);

	//the extent of inaccuracy in aiming--that is, the difference between where i want to aim and where i do aim (in degrees)
	virtual	float	GetAimDeviationDegrees(edict_t &monster);

	//fixme: this is a shoddy way of handling save/load
	virtual int		GetClassCode(void){return AI_BASE;}//this should be different for everybody in this class family

	//adjust this to return something moderately compatible with player team types
	virtual int	 GetTeam(edict_t &monster){return 1;}
	// someone can be on the other team and still be innocent (iraqi civilians)
	virtual int  IsInnocent() { return 0; }
	virtual bool	AmIAsGoodAsDead();

	virtual qboolean SafeToRemove(edict_t &monster);

	virtual void			Emote(edict_t &monster, emotion_index new_emotion, float emotion_duration, qboolean scripted_emoting=false);
	virtual emotion_index	GetMood(edict_t &monster);

	virtual void			SetPriority(priority_index newp){worldpriority = newp;}
	virtual priority_index	GetPriority(void){return worldpriority;}

	virtual void			UseMonster(edict_t *user);
	virtual void			TouchMonster(edict_t *user);

	virtual void			SetAbusedByTeam(qboolean newa){abusedByTeam = newa;}
	virtual qboolean		GetAbusedByTeam(void){return abusedByTeam;}

	virtual 		void NewSense(sense_c *new_sense, edict_t *monster);

//					enemyWorldInfo_e	LastEnemyInfo(void){return enemyInfo;}
//					enemyWorldInfo_e	CurrentEnemyInfo(void){if (enemyInfoTime > level.time-0.2)return enemyInfo; else return EWI_NUM;}

	//add action to my action list; override Activate to let ai know I want to be considered activated (scripts, etc)
	virtual 		void NewAction(action_c *new_action, edict_t *monster, qboolean activatenow=false);
	virtual 		void NewNextAction(action_c *new_action, edict_t *monster, qboolean activatenow=false);
	virtual 		void NewDecision(decision_c *new_decision, edict_t *monster);
	virtual 		void NewCurrentAction(action_c *new_action, edict_t &monster);
	virtual 		void NewDeathArmorAction(edict_t &monster);
	virtual 		void NextAction(edict_t &monster);
	virtual void EvaluateDecisions(edict_t &monster);
	virtual 		void EvaluateSenses(edict_t &monster);
	virtual	void Think(edict_t &monster);
	virtual qboolean Damage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb);

	virtual float	GetJumpDistance(void){return jumpdistance;}
	virtual float	GetJumpHeight(void){return jumpheight;}
	virtual float	GetFallHeight(void){return fallheight;}
	virtual float	GetStepHeight(void){return stepheight;}
	virtual int		GetMoveMask(void){return move_mask;}

	virtual int		GetRank(void){return rank;}
	virtual void	SetRank(int newrank){rank=newrank;}

	virtual void	GetSensedClientInfo(unsigned mask, sensedEntInfo_t &sensedEnt);
	virtual void	GetSensedMonsterInfo(unsigned mask, sensedEntInfo_t &sensedEnt);

	virtual qboolean	IsActive(void){return isActive;}
	virtual	void		Activate(edict_t &monster);

	virtual	void		SetStartleability(qboolean newVal){isStartleable=newVal;}
	virtual	qboolean	GetStartleability(void){return isStartleable;}

	virtual void		SetHostage(qboolean bHostage) { m_bHostage = bHostage; }
	virtual qboolean	IsHostage() { return m_bHostage; }

	virtual void		SetSpecialBuddy(qboolean bBuddy) { m_bSpecialBuddy = bBuddy; }
	virtual qboolean	IsSpecialBuddy() { return m_bSpecialBuddy; }

	virtual void		SetConcentratingOnPlayer(qboolean bConcentrating) { m_bConcentratingOnPlayer = bConcentrating; }
	virtual qboolean	IsConcentratingOnPlayer() { return m_bConcentratingOnPlayer; }

	//do i have a weapon?
	virtual	qboolean	IsArmed(edict_t &monster);

	virtual qboolean	HasTarget(void){return (hasHadTarget && level.time-lastTargetTime<1.0);}
	virtual	qboolean	HasHadTarget(void){return hasHadTarget;}
	virtual	void		SetTargetTime(float now, edict_t *target, vec3_t position);
	virtual edict_t		*getTarget(void){return curTarget;}
	virtual void		setTarget(edict_t *target){assert((!target)||(target->inuse < 10));curTarget = target;}
	virtual void		setFirstTargetTime(float newTime){firstTargetTime = newTime;}
	virtual float		getFirstTargetTime(void){return firstTargetTime;}
	virtual void		setLastNonTargetTime(float newTime){lastNonTargetTime = newTime;}
	virtual float		getLastNonTargetTime(void){return lastNonTargetTime;}
	virtual float		getLastTargetTime(void){return lastTargetTime;}

	virtual qboolean	getTargetPos(vec3_t putPosHere);

	virtual void	MuteSenses(unsigned mask, float degree, smute_recovery recovery_code, float recovery_time);
	virtual void	RegisterSenseEvent(unsigned mask, vec3_t event_origin, float event_time, edict_t *event_edict, ai_sensetype_e event_code);

	//go through senses asking them their muted level with this mask, and return highest response
	virtual	float	GetSenseMutedLevel(unsigned mask);

	virtual mmove_t	*GetMove(void);
	virtual void	SetMove(mmove_t *newmove);

	virtual body_c			*GetBody(void){return body;}
	virtual ggObjC			*GetGhoulObj(void){return MyGhoulObj;}

	virtual void	SetAimAngles(vec3_t newaim){VectorCopy(newaim, aim_angles);}
	virtual void	SetAimVector(vec3_t newaim){vectoangles(newaim, aim_angles);}
	virtual void	GetAimAngles(vec3_t myaim){VectorCopy(aim_angles,myaim);}
	virtual void	GetAimVector(vec3_t forward){AngleVectors(aim_angles, forward, NULL, NULL);}

	virtual void	SetLookAngles(vec3_t newaim){VectorCopy(newaim, look_angles);}
	virtual void	SetLookVector(vec3_t newaim){vectoangles(newaim, look_angles);}
	virtual void	GetLookAngles(vec3_t myaim){VectorCopy(look_angles,myaim);}
	virtual void	GetLookVector(vec3_t forward){AngleVectors(look_angles, forward, NULL, NULL);}
	virtual attention_index	GetAttention(edict_t &monster){return attentionLevel;}

	virtual	void	HandleCallBack(IGhoulInst *me,void *user,float now,const void *data);

	virtual float	GetLeftFootPos(void){return lFootPos;}
	virtual float	GetRightFootPos(void){return rFootPos;}
	virtual void	SetLeftFootPos(float newpos){lFootPos=newpos;}
	virtual void	SetRightFootPos(float newpos){rFootPos=newpos;}

	virtual	float	GetGround(void){return ground;}
	virtual	void	SetGround(float groundval);

	virtual void	FinishMove(edict_t &monster);

	//send frames to body for storage
	virtual void	InAirFrame(edict_t &monster, float frameTime);
	virtual void	JumpFrame(edict_t &monster);
	virtual void	LandFrame(edict_t &monster);
	virtual void	AttackFrame(edict_t &monster);
	virtual void	MeleeFrame(edict_t &monster);
	virtual void	ThrowFrame(edict_t &monster);

	virtual qboolean	AttemptSetBBox (edict_t &monster, bbox_preset preset_index, qboolean forceZ = false);
	virtual void		ConfirmBBox (edict_t &monster, mmove_t	*curMove);
	virtual void		GetBBoxPreset (bbox_preset preset_index, vec3_t pmins, vec3_t pmaxs){VectorCopy(preset_mins[preset_index],pmins);VectorCopy(preset_maxs[preset_index],pmaxs);}
	virtual	bbox_preset	GetBBoxPresetIndex(edict_t &monster){return current_bbox;}
	
	// Find a scripted decision
	//if you pass in NULL, will return any old scripted decision
	virtual	scripted_decision		*FindScriptedDecision(edict_t* ScriptEntity);
	virtual	void					CancelScripting(edict_t* ScriptEntity);
	// Find an order decision
	decision_c		*FindOrderDecision();

	void	RemoveOldDecisions(int code);
	virtual action_c	*GetLastAction();
	int				&GetMiscFlags(void){return miscFlags;}

	bool GetLastActionDestination(vec3_t destination);
	virtual void	SetAttentionLevel(attention_index newIndex){assert(newIndex >= ATTENTION_DISTRACTED);assert(newIndex <= ATTENTION_ESCAPED); attentionLevel = newIndex;}


	// Accessor function to Die
	virtual void	AnimDie(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point){Die(monster, inflictor, attacker, damage, point);};
	
	// Get a default action
	virtual action_c	*DefaultAction(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination = vec3_origin, vec3_t facing = vec3_origin, edict_t *target = NULL, float timeout=0/*999999999999*/, qboolean fullAnimation = false);//for standing
	virtual action_c	*WalkAction(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false);
	virtual action_c	*PainAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *other, float kick, int damage, float timeout=999999999999, qboolean fullAnimation = true);
	virtual action_c	*DeathAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
	virtual action_c	*DeathAction(decision_c *od, action_c *oa, mmove_t *newanim1, mmove_t *newanim2, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
	virtual action_c	*DeathArmorAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
	virtual action_c	*DeathArmorAction(decision_c *od, action_c *oa, mmove_t *newanim1, mmove_t *newanim2, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
	virtual action_c	*AttackAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t attackPos, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false, bool shouldKill = false, int NullTarget = 0);
	virtual action_c	*JumpAction(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false);
	virtual action_c	*FallAction(decision_c *od, action_c *oa, mmove_t *newanim, mmove_t *newanim2, vec3_t destination, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false);
	virtual action_c	*SurrenderAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *surrenderTo, vec3_t attackerPos, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false);
	virtual action_c	*CaptureAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t attackPos, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false);
	virtual action_c	*EndScriptAction(decision_c *od);

	nodeData_t		&getPathData(void){return nodeData;}

	virtual qboolean		BeingWatched(void){return watchState;}
	virtual	void		WatchMe(void){watchState = true;}

	virtual void		UpdatePathPosition(edict_t *myEnt);
	virtual int			getMyNode(void){return nodeData.curNode;}

	virtual void		RegisterShotForDodge(vec3_t start, vec3_t end, edict_t *shooter);
	virtual void	RegisterShotForReply(vec3_t start, vec3_t end, edict_t *shooter);
	virtual void		Spook(float amount, vec3_t center);

	virtual int			CheckForCorpseActivate(edict_t &monster);
	virtual void		RequestMoveOutOfWay(vec3_t moveDir);
	virtual float		*GetMoveOutOfWay(void){return requestedMoveDir;}

	virtual CAISkills	*GetMySkills(void){return &mySkills;}

	static	ai_c	*Create(int classcode, edict_t *monster=NULL, char *ghoulname="enemy/meso", char *subname="");

						ai_c(ai_c *orig);
	virtual void		Evaluate(ai_c *orig);
	virtual void		Write();
	virtual void		Read();
};

/**********************************************************************************
 **********************************************************************************/


#endif