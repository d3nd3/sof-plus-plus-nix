
typedef enum
{
	DECISION,
	BASE_DECISION,
	PURSUE_DECISION,
	POINTCOMBAT_DECISION,
	SEARCH_DECISION,
	DODGE_DECISION,
	RETREAT_DECISION,
	PATHIDLE_DECISION,
	PATHCOMBAT_DECISION,
	SCRIPTED_DECISION,
	ORDER_DECISION,
	DEKKER1_DECISION,
	DEKKER2_DECISION,
	DEKKER3_DECISION,
	DEKKER4_DECISION,
};


#define LEADERFLAG_RECRUIT	0x00000001//leader will look for a new group to lead if he doesn't have one
#define LEADERFLAG_PHYSICAL	0x00000002//leader physically leads--formations assume leader as front member

#define ORDER_PRIORITY_ROOT	25
#define SCRIPTED_PRIORITY_ROOT	25//fixme: script decision shouldn't be worrying about adding its own actions at all usually--should just worry about recovering from actions that didn't work?????

typedef enum
{
	NONE = 0,
	ATTACK,
	MOVE,
	FOLLOW,
	WAIT,
} goal_e;

typedef enum
{
	NI_IDEALPOSITION,
	NI_TOOFAR,
	NI_TOOCLOSE,
	NI_NOTCLEAR,
	NI_FLEEING,
	NI_DUCKING,				//used in certain fleeing cases
	NI_DODGING,				//used in certain fleeing cases
	NI_TOOFAR_SEARCHING,	//this one won't attack
} nonIdealSituation;

/**********************************************************************************
 **********************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////
///////	base decision branch
///////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class decision_c
{
private:
protected:
			float			last_dilution_time;
	//priority is a combination of whether immediate conditions
	//are favorable for this decision, and whether this decision has a track record for working
			int				priority;
			int				priority_base;

	//validity keeps track of how successful this decision has been in the past;
	//considering monsters won't last too long, this will be weighted pretty heavily toward
	//most recent result, and will slowly edge into neutral territory;
	//range is 0 to 1
			float			validity;
			
			float			timeout_time;
			qboolean		allow_timeout;
			
			qboolean		ClearShot(ai_c &which_ai, edict_t &monster, vec3_t goalpos, edict_t *ignore, bbox_preset testbbox, float *orgPos = 0, int *blockingGuy = 0);
			float			lastClearTime;
			qboolean		isClear;

	//errr, not so sure i dig this. --ss
			float			GetGroupPosition(ai_c &which_ai, edict_t &monster, vec3_t relativeTo);
	virtual	qboolean		AtGroupFront(ai_c &which_ai, edict_t &monster, vec3_t dest, vec3_t face);
							
	virtual void			UpdateValidity(float adjustValue){validity = validity*0.75+adjustValue*0.25;}//weight heavily toward new value
	virtual void			UpdateValidity(void);

	virtual	qboolean		IsAdversary(edict_t &monster, edict_t *otherGuy);
							
			qboolean		IsTimedOut(void);
public:

	virtual mmove_t			*GetSequenceForMovement(ai_c &which_ai, edict_t &monster, vec3_t dest, vec3_t face, mmove_t *preferred_move=NULL, int reject_actionflags=0);
	virtual mmove_t			*GetSequenceForStand(ai_c &which_ai, edict_t &monster, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, mmove_t *preferred_move=NULL, int reject_actionflags=0);
	virtual mmove_t			*GetSequenceForJump(ai_c &which_ai, edict_t &monster, vec3_t dest, vec3_t face, mmove_t *preferred_move=NULL, int reject_actionflags=0);
	virtual mmove_t			*GetSequenceForAttack(ai_c &which_ai, edict_t &monster, vec3_t dest, vec3_t face, edict_t *target, mmove_t *preferred_move=NULL, int reject_actionflags=0);
	virtual mmove_t			*GetSequenceForDodge(ai_c &which_ai, edict_t &monster, vec3_t dest, vec3_t face, edict_t *target, mmove_t *preferred_move=NULL, int leftSide = 0, int reject_actionflags=0);

	//will return one of the above, depending on the code passed in.
	virtual	mmove_t			*GetSequenceForActionCode(ai_c &which_ai, edict_t &monster, action_code the_code, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, edict_t *target=NULL, mmove_t *preferred_move=NULL, int reject_actionflags=0);

	virtual	action_c		*AddActionForSequence(ai_c &which_ai, edict_t &monster, mmove_t *move, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, edict_t *target=NULL, action_c *owneraction=NULL, bool shouldKill = false, int NullTarget = 0, float timeoutTime = 0.0f);
	virtual action_c		*ForceNextActionForSequence(ai_c &which_ai, edict_t &monster, mmove_t *move, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, edict_t *target=NULL, action_c *owneraction=NULL){return NULL;};

							decision_c(int priority_root = 0, float timeout = 0, edict_t* ScriptOwner = NULL);
	virtual					~decision_c(void) {}


	//ScriptEnt is the entity containing a script for use with scripting
			int				Priority(void) { return priority+priority_base; }

	virtual qboolean		Consider(ai_c &which_ai, edict_t &monster);
	virtual void			Perform(ai_c &which_ai, edict_t &monster);
	virtual void			ActionCompleted(action_c &which_action, ai_c &which_ai, edict_t &monster, float percent_success) {}
	virtual void			FinishScript(){return;};


	virtual qboolean		SafeToRemove(edict_t &monster){return true;}

	//HEY!!! THIS BETTER BE FALSE UNLESS YOU'RE A SCRIPTED DECISION!!!!!
	virtual	bool			MatchScriptEnt(edict_t* Entity){return false;} 

	virtual void			SetGoalType(goal_e theGoal){};
	virtual void			SetGoalTarget(edict_t *theTarget){};
	virtual void			SetGoalLocation(vec3_t theLocation){};
	virtual void			SetGoalTime(float theTime){};

	virtual goal_e			GetGoalType(){return NONE;};
	virtual edict_t*		GetGoalTarget(){return NULL;};
	virtual void			GetGoalLocattion(vec3_t theLocation){};
	virtual float			GetGoalTime(){return 0;};
	virtual void			UsePathfinding(ai_c &which_ai, edict_t &monster){};
	virtual void			SetInfoForDodge(vec3_t start, vec3_t end){}
	virtual void			SetInfoForReply(vec3_t start, vec3_t end){}
	virtual void			AddFear(float amount, vec3_t center){}


							decision_c(decision_c *orig);
			void			Evaluate(decision_c *orig);
	virtual void			Write() { assert(0); }
	virtual void			Read() { assert(0); }

	virtual int				GetClassCode(void){return DECISION;}//this should be different for everybody in this class family
static		decision_c		*NewClassForCode(int code);
};

/**********************************************************************************
 **********************************************************************************/

class base_decision : public decision_c
{
public:
		base_decision(int priority_root = 0, float timeout = 0, edict_t* ScriptOwner = NULL)
			:decision_c(priority_root, timeout, ScriptOwner){}

							base_decision(base_decision *orig);
			void			Evaluate(base_decision *orig);
	virtual void			Write();
	virtual void			Read();
	virtual int				GetClassCode(void) { return BASE_DECISION; }
};

/**********************************************************************************
 **********************************************************************************/

typedef enum
{
	//these are just examples...
	USETYPE_PUSH,//push a door open
	USETYPE_FLIP,//flip a switch
	USETYPE_MISC//well? do something, at least...
} useType_t;

typedef struct
{
	vec3_t		usePos;
	vec3_t		useDir;
	useType_t	useType;
	edict_t		*useEnt;
} brushUseInfo_t;

typedef struct
{
	vec3_t				pos;//get this value from the raw point info
	vec3_t				direction;//ick, this is currently only used by temp use-points
	edict_t				*targetEnt;//ick, this is currently only used by temp use-points
	bool				valid;
	unsigned			temp_type;
	float				time;
} ai_pathpoint_t;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////
///////	path decision branch
///////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//also fixme: to change waypoints so they don't have to be ents, temp point stuff has to be changed (all pretty centralized in anticipation)
class path_decision : public decision_c
{
private:
protected:
	float	last_jump_time;
	float	path_fail_time;
	vec3_t	path_fail_dir;
	ai_pathpoint_t	path_nextpoint;//the waypoint i'm currently moving toward

	edict_t *path_goalentity;//the entity i'm trying to reach--a player, generally
	vec3_t	path_goalpos;//the place i'm trying to reach--a player's origin, generally
	float	path_goal_updatedtime;

	vec3_t	newMoveDir;
	float	lastCheckTime;

	nonIdealSituation nonIdealReason;
	int		blockingEnemy;
	float	lastDuckInvalidateTime;
	float	lastDuckValidTime;

	vec3_t	aimWanderVec;//whilst i'm aiming, i think i'll take a little stroll...

	qboolean	stuckNonIdeal;//enemy isn't close enough to be in range, but he has no movement options, so try to shoot anyway (you know, what the hell)
								//HOWEVER - you should probably have crap accuracy because you're so far away, yeah?  I say YEAH!

	virtual void		RecalcMoveVec(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);

	virtual void		SetGoalPosition(ai_c &which_ai, edict_t &monster);
	virtual qboolean	GetGoalPosition(ai_c &which_ai, edict_t &monster, vec3_t goal);
	virtual qboolean	NextPointInPath(ai_c &which_ai, edict_t &monster, vec3_t position);//this sets path_nextpoint (and path_endpoint if path finished), checks path_nextpoint
	qboolean			EvaluatePathEndpoint(ai_c &which_ai, edict_t &monster, ai_pathpoint_t endpoint_candidate, vec3_t goalpos);//sets nothing, uses nothing other than inputs
											//returns false if path is not valid,
											//true if path is valid (or if path will be ignored)

	//fixme: when pathfinding in, should totally focus on ent avoidance
	void		TempPointCreate(vec3_t position, edict_t &monster);
	void		AvoidPointCreate(vec3_t normal, edict_t &monster);
	void		PoliteAvoidPointCreate(vec3_t normal, edict_t &monster);
	void		UsePointCreate(brushUseInfo_t &useeInfo, edict_t &monster);
	void		JumpPointCreate(vec3_t position, edict_t &monster);//subclass of temppoints made specifically for jumping over things
	void		FallPointCreate(vec3_t position, edict_t &monster);//subclass of temppoints made specifically for intentional falling
	void		PerformAvoidance(ai_c &which_ai, edict_t &monster);

	//use this to adjust movement for groups, and check pathfinding (?) to make sure direction is valid--moveVec comes in uninit'ed
	virtual void GetMovementVector(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);

	virtual qboolean IsIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t curPos, vec3_t goalPos, vec3_t moveVec);
	virtual void AddActionIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);
	virtual void AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);
	virtual void AddAction(ai_c &which_ai, edict_t &monster);
	virtual qboolean AddSpecialPathAction(ai_c &which_ai, edict_t &monster);//called from AddAction--depending on the type of path point path_nextpoint is, add appropriate action
public:
	path_decision(edict_t *goalent = NULL, int priority_root = 0, float timeout = 0, edict_t* ScriptOwner = NULL);
	virtual ~path_decision(void){}

	virtual qboolean Consider(ai_c &which_ai, edict_t &monster);

	virtual void Perform(ai_c &which_ai, edict_t &monster);
	virtual void ActionCompleted(action_c &which_action, ai_c &which_ai, edict_t &monster, float percent_success);
	virtual void	FinishScript(){return;};

					path_decision(path_decision *orig);
	virtual void	Evaluate(path_decision *orig);
};

/////////////////////////////////////////////////////////////////////////////////////////////
//pursue decision
/////////////////////////////////////////////////////////////////////////////////////////////

class pursue_decision : public path_decision
{
public:
	pursue_decision(edict_t *goalent = NULL, int priority_root = 0, float timeout = 0, edict_t* ScriptOwner = NULL)
	:path_decision(goalent, priority_root, timeout, ScriptOwner){}

					pursue_decision(pursue_decision *orig);
			void	Evaluate(pursue_decision *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return PURSUE_DECISION; }
};

/////////////////////////////////////////////////////////////////////////////////////////////
//dekker1 decision
/////////////////////////////////////////////////////////////////////////////////////////////

class dekker1_decision : public path_decision
{
protected:
	virtual	void	AddAction(ai_c &which_ai, edict_t &monster);
	virtual void AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);
	virtual void AddDekkerActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec, float speed);
public:
	dekker1_decision(edict_t *goalent = NULL, int priority_root = 10, float timeout = 0, edict_t* ScriptOwner = NULL)
	:path_decision(goalent, priority_root, timeout, ScriptOwner){}

					dekker1_decision(dekker1_decision *orig);
			void	Evaluate(dekker1_decision *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return DEKKER1_DECISION; }
	virtual qboolean Consider(ai_c &which_ai, edict_t &monster);
};

/////////////////////////////////////////////////////////////////////////////////////////////
//dekker1 decision
/////////////////////////////////////////////////////////////////////////////////////////////

class dekker2_decision : public dekker1_decision
{
protected:
//	virtual void GetMovementVector(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);
	//ARGHHH!--this was supposed to be a simple override to adjust dekker's speed for the various stages, and now it's a huge bloated mess.
	virtual void	AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);
	virtual	void	AddAction(ai_c &which_ai, edict_t &monster);
public:
	dekker2_decision(edict_t *goalent = NULL, int priority_root = 8, float timeout = 0, edict_t* ScriptOwner = NULL)
	:dekker1_decision(goalent, priority_root, timeout, ScriptOwner){}

					dekker2_decision(dekker2_decision *orig);
			void	Evaluate(dekker2_decision *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return DEKKER2_DECISION; }
	virtual qboolean Consider(ai_c &which_ai, edict_t &monster);
};

/////////////////////////////////////////////////////////////////////////////////////////////
//dekker3 decision
/////////////////////////////////////////////////////////////////////////////////////////////


class dekker3_decision : public path_decision
{
protected:
	qboolean		reachedDest;

	virtual void AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);
	virtual qboolean IsIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t curPos, vec3_t goalPos, vec3_t moveVec);
	virtual qboolean	GetGoalPosition(ai_c &which_ai, edict_t &monster, vec3_t goal);
	virtual	void	AddAction(ai_c &which_ai, edict_t &monster);
public:
	dekker3_decision(edict_t *goalent = NULL, int priority_root = 5, float timeout = 0, edict_t* ScriptOwner = NULL)
	:path_decision(goalent, priority_root, timeout, ScriptOwner){reachedDest=false;}

					dekker3_decision(dekker3_decision *orig);
			void	Evaluate(dekker3_decision *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return DEKKER3_DECISION; }
	virtual qboolean Consider(ai_c &which_ai, edict_t &monster);
};

/////////////////////////////////////////////////////////////////////////////////////////////
//dekker4 decision
/////////////////////////////////////////////////////////////////////////////////////////////


class dekker4_decision : public path_decision
{
protected:
	qboolean		reachedDest;

	virtual void AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);
	virtual qboolean IsIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t curPos, vec3_t goalPos, vec3_t moveVec);
	virtual qboolean	GetGoalPosition(ai_c &which_ai, edict_t &monster, vec3_t goal);
	virtual	void	AddAction(ai_c &which_ai, edict_t &monster);
public:
	dekker4_decision(edict_t *goalent = NULL, int priority_root = 2, float timeout = 0, edict_t* ScriptOwner = NULL)
	:path_decision(goalent, priority_root, timeout, ScriptOwner){reachedDest=false;}

					dekker4_decision(dekker4_decision *orig);
			void	Evaluate(dekker4_decision *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return DEKKER4_DECISION; }
	virtual qboolean Consider(ai_c &which_ai, edict_t &monster);
};

/////////////////////////////////////////////////////////////////////////////////////////////
//path decision refitted to use combat points
/////////////////////////////////////////////////////////////////////////////////////////////

class pointcombat_decision : public path_decision
{
protected:
	int		current_point;
	vec3_t	current_point_pos;
	vec3_t	current_point_dest;
	int		current_point_type;
	int		current_point_subtype;
	vec3_t	current_point_dir;
	int		last_point_trans;//time of the last transition for points that oscillate
	int		last_consider_time;//some guys don't think to do this all that often

	virtual void		SetGoalPosition(ai_c &which_ai, edict_t &monster);
	virtual qboolean	GetGoalPosition(ai_c &which_ai, edict_t &monster, vec3_t goal);

	virtual qboolean IsIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t curPos, vec3_t goalPos, vec3_t moveVec);
//	virtual void AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);
	virtual void AddActionIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);
public:
	pointcombat_decision(edict_t *goalent = NULL, int priority_root = 0, float timeout = 0, edict_t* ScriptOwner = NULL);
//	virtual ~pointcombat_decision(void){path_decision::~path_decision();}

	virtual qboolean Consider(ai_c &which_ai, edict_t &monster);
	void			 GetMovementVector(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);

					pointcombat_decision(pointcombat_decision *orig);
			void	Evaluate(pointcombat_decision *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return POINTCOMBAT_DECISION; }
};

/**********************************************************************************
 **********************************************************************************/

//approach takes precedence over pursuit, pursuit takes precedence over wandering
//approach==go to where target last was, stop when i get there
//pursuit==go the direction that target was last seen going, stop when i hit something
class search_decision : public path_decision
{
protected:
	qboolean pursuit_search, approach_search;
	vec3_t pursuit_dir;
	float last_fired_time;
	vec3_t firedSpot;
	vec3_t trail1, trail2, trail3;
	virtual void		SetGoalPosition(ai_c &which_ai, edict_t &monster);
	virtual qboolean	GetGoalPosition(ai_c &which_ai, edict_t &monster, vec3_t goal);
	virtual	void		AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);
	virtual qboolean IsIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t curPos, vec3_t goalPos, vec3_t moveVec);
public:
	search_decision(edict_t *goalent = NULL, int priority_root = 0, float timeout = 0);
	virtual ~search_decision(void){}

	virtual void ActionCompleted(action_c &which_action, ai_c &which_ai, edict_t &monster, float percent_success);

					search_decision(search_decision *orig);
			void	Evaluate(search_decision *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return SEARCH_DECISION; }

	virtual	qboolean	Consider(ai_c &which_ai, edict_t &monster);
	virtual void	SetInfoForDodge(vec3_t start, vec3_t end);
	virtual void	SetInfoForReply(vec3_t start, vec3_t end){SetInfoForDodge(start, end);}
};

/**********************************************************************************
 **********************************************************************************/
//this is a path_decision that should look for cover & hop around sideways
class dodge_decision : public path_decision
{
private:
	int	prev_health;
protected:
	float last_dodge_time;
	float last_shotat_time;
	int	  dodgeSide;
	vec3_t shotStart;//need this info to determine which way to dodge
	vec3_t shotEnd;//need this info to determine which way to dodge
	virtual void AddAction(ai_c &which_ai, edict_t &monster);
public:
	dodge_decision(int priority_root = 0, float timeout = 0);
	virtual ~dodge_decision(void){}

	virtual qboolean Consider(ai_c &which_ai, edict_t &monster);
	virtual void ActionCompleted(action_c &which_action, ai_c &which_ai, edict_t &monster, float percent_success);

					dodge_decision(dodge_decision *orig);
			void	Evaluate(dodge_decision *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return DODGE_DECISION; }
	virtual void	SetInfoForDodge(vec3_t start, vec3_t end);
};


/**********************************************************************************
 **********************************************************************************/
//this is a path_decision that should look for a path Away from target
class retreat_decision : public path_decision
{
private:
	int prev_health;
	float fearIndex;
	bool haveFleeDest;

	int	spookStyle;
	int	spookTime;
	vec3_t spookCenter;

protected:
	virtual qboolean	GetGoalPosition(ai_c &which_ai, edict_t &monster, vec3_t goal);
	virtual qboolean IsIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t curPos, vec3_t goalPos, vec3_t moveVec);
	virtual void AddActionIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);
	virtual	qboolean		IsAdversary(edict_t &monster, edict_t *otherGuy);
public:
	retreat_decision(int priority_root = 0, float timeout = 0);
	virtual ~retreat_decision(void){}

	virtual qboolean Consider(ai_c &which_ai, edict_t &monster);
	virtual void	AddFear(float amount, vec3_t center);
	virtual void	SetInfoForDodge(vec3_t start, vec3_t end);

			
					retreat_decision(retreat_decision *orig);
			void	Evaluate(retreat_decision *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return RETREAT_DECISION; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////
///////	pathcorner-traversing decision branch
///////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////
//path decision refitted to follow path_corner trail--gets rid of itself if path_corner trail ends.
/////////////////////////////////////////////////////////////////////////////////////////////
class pathcorner_decision : public path_decision
{
protected:
	float reactivate_time;
	edict_t	*oldTarget;
	int		curWaitActionNum;
	qboolean	firstActionAtWait;
	qboolean	isAtCorner;
	mmove_t	*curMoveAction;//store off the moveAction for path_corners when i pass them

	virtual void		SetGoalPosition(ai_c &which_ai, edict_t &monster);
	virtual qboolean	GetGoalPosition(ai_c &which_ai, edict_t &monster, vec3_t goal);

	virtual void GetMovementVector(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);

	virtual qboolean IsIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t curPos, vec3_t goalPos, vec3_t moveVec);
	virtual void AddActionNonIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);
	virtual void AddActionIdealPosition(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);
	virtual	void	FinishPathPoint(ai_c &which_ai, edict_t &monster);
public:
	pathcorner_decision(edict_t *goalent=NULL, int priority_root=0, float timeout=0, edict_t* ScriptOwner=NULL);

	virtual ~pathcorner_decision(void){
		// path_decision::~path_decision();
	}

	virtual void	ActionCompleted(action_c &which_action, ai_c &which_ai, edict_t &monster, float percent_success);

	virtual qboolean Consider(ai_c &which_ai, edict_t &monster);

					pathcorner_decision(pathcorner_decision *orig);
	virtual void	Evaluate(pathcorner_decision *orig);
};

class pathidle_decision : public pathcorner_decision
{
public:
	pathidle_decision(edict_t *goalent=NULL, int priority_root=0, float timeout=0, edict_t* ScriptOwner=NULL)
		:pathcorner_decision(goalent, priority_root, timeout, ScriptOwner){}

					pathidle_decision(pathidle_decision *orig);
			void	Evaluate(pathidle_decision *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return PATHIDLE_DECISION; }
};

/////////////////////////////////////////////////////////////////////////////////////////////
//path_corner decision refitted to follow point_combat trail--gets rid of itself if point_combat trail ends.
/////////////////////////////////////////////////////////////////////////////////////////////

class pathcombat_decision : public pathcorner_decision
{
protected:
	virtual void	GetMovementVector(ai_c &which_ai, edict_t &monster, vec3_t goalPos, vec3_t moveVec);
	virtual	void	FinishPathPoint(ai_c &which_ai, edict_t &monster);
public:
	virtual qboolean Consider(ai_c &which_ai, edict_t &monster);
	pathcombat_decision(edict_t *goalent=NULL, int priority_root=0, float timeout=0, edict_t* ScriptOwner=NULL):pathcorner_decision(goalent,priority_root,timeout,ScriptOwner){}

					pathcombat_decision(pathcombat_decision *orig);
			void	Evaluate(pathcombat_decision *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return PATHCOMBAT_DECISION; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////
///////	scripted/queued order decision branch
///////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/**********************************************************************************
 **********************************************************************************/
//this is a path_decision that doesn't necessarily use any path stuff
//although, it Could use the path stuff to attack a civilian, run to an elevator, etc.

class scriptOrder_c
{
public:
	mmove_t		*preferredMove;//NULL if no preference...
	float		holdTime;//time to hold (for idles, static stuff)
	vec3_t		turnVec;
	vec3_t		destVec;
	qboolean	absoluteDest;//whether the destination is absolute or relative.
	bool		kill;//whether an attack order should be an automatic kill
	action_code	actionType;//what do i want to be doing here?
	int			flags;
	Event		*SignalEvent;
	float		speed;
	edict_t		*target;
	int			NullTarget; // targeted null in a script
	int			Emotion;

	scriptOrder_c(void);
	scriptOrder_c(scriptOrder_c *orig);
	void Evaluate(scriptOrder_c *orig);
	void Write(void);
	void Read(void);
};

class scripted_decision : public path_decision
{
private:
protected:
			qboolean	scriptDone;
			edict_t		*self;
			qboolean	ignorePreferred;
			float		orderStartTime;
			vec3_t		oldPosition;//where i should be at the start of current order
			vec3_t		actDest;
			vec3_t		actDestDir;//for more reliable testing of whether i've gotten close enough to destination--this is the vector to actDest when I start out;
									//when the vector to the actDest is closer to the inverse of this than to this, i've passed my destination
			mmove_t		*lastMove;

			edict_t			*ScriptEnt;
#define SCRIPTED_DECISION_END	(offsetof(scripted_decision, ScriptEnt) + sizeof(ScriptEnt))

	list<scriptOrder_c *>		orders;
//	virtual void AddAction(ai_c &which_ai, edict_t &monster);
public:
						scripted_decision(edict_t *goalent = NULL, int priority_root = SCRIPTED_PRIORITY_ROOT, float timeout = 0, edict_t* ScriptOwner = NULL);
	virtual				~scripted_decision(void);

	virtual void		Perform(ai_c &which_ai, edict_t &monster);
	virtual void		FinishScript(){scriptDone = true;};
	virtual qboolean	Consider(ai_c &which_ai, edict_t &monster){priority=15; return scriptDone;};
	virtual	bool 		MatchScriptEnt(edict_t* Entity);

	virtual	void		AddOrder(ai_c &which_ai, edict_t &monster, scriptOrder_c &this_order);

	virtual void		ActionCompleted(action_c &which_action, ai_c &which_ai, edict_t &monster, float percent_success);

						scripted_decision(scripted_decision *orig);
			void		Evaluate(scripted_decision *orig);
	virtual void		Read();
	virtual void		Write();
	virtual int			GetClassCode(void) { return SCRIPTED_DECISION; }
};

// end