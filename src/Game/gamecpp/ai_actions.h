class Event;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////
///////	stand around action branch (root branch)
///////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class action_c
{
public:
private:
protected:
	mmove_t			*anim;
	vec3_t			face,dest;
	float			faceUpdatedTime;
	bool			interrupted;
	qboolean		abortAction;
	decision_c_ptr	owner_decision;
	action_c_ptr	owner_action;
	float			velScale;
	float			default_turn;
	int				move_mask;
	float			start_time;			// set when action is first executed
	float			timeout_interval;	// passed in during construction
	float			timeout_time;		// sum of start_time + timeout_interval
//	edict_t			*action_target;
	vec3_t			jump_intent;
	float			jump_finish_time;//when i expect to finish up the jump
	qboolean		waitForAnim;
	Event			*SignalEvent;
	bool			isLethal;
	int				NullTarget;

	virtual void ResolveAction(ai_c &which_ai, edict_t &monster, float decision_success, float action_success);

	virtual void		InterpretFlags(int cur_flags, ai_c &which_ai, edict_t &monster);//trigger frame-based actions in here

	//animation stuff--should just call body funcs of the same name
	virtual qboolean	SetAnimation(ai_c &which_ai, edict_t &monster);
	virtual qboolean	ForceAnimation(ai_c &which_ai, edict_t &monster);
	virtual qboolean	MatchAnimation(ai_c &which_ai, edict_t &monster);
	virtual void		NextMovement(ai_c &which_ai, edict_t &monster, vec3_t curDist);
	virtual void		NextTurn(float scale, ai_c &which_ai, edict_t &monster, turninfo_s &turninfo);
	virtual int			NextFlags(ai_c &which_ai, edict_t &monster);

	virtual void		Jump(ai_c &which_ai, edict_t &monster, vec3_t jumpvec);
	virtual qboolean	Move(ai_c &which_ai, edict_t &monster, vec3_t dist);
	virtual qboolean	Turn(ai_c &which_ai, edict_t &monster, turninfo_s &turninfo);
	virtual	void		Aim(ai_c &which_ai, edict_t &monster);
	virtual qboolean	EvaluateMovement(ai_c &which_ai, edict_t &monster, vec3_t distance);

	virtual void		SetTurnDestination(ai_c &which_ai, edict_t &monster);

	//this guy combines bbox adjusting, frame advancing, frame event handling, turning, getting movement intention and abort check
	virtual		qboolean	PerformBodilyFunctions(ai_c &which_ai, edict_t &monster, vec3_t curDist);

	virtual qboolean	AdjustBBox(ai_c &which_ai, edict_t &monster);
public:
	edict_t			*action_target;

	action_c(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination = vec3_origin, vec3_t facing = vec3_origin, edict_t *target = NULL, float timeout = 99999999999, qboolean fullAnimation = false);
	action_c(void);
	virtual	void			Init(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination = vec3_origin, vec3_t facing = vec3_origin, edict_t *target = NULL, float timeout = 99999999999, qboolean fullAnimation = false);
	virtual	void			Init(void);
	virtual ~action_c(void) {}

	//fixme: this is a shoddy way of handling save/load
	virtual void			GetDest(vec3_t destination){VectorCopy(dest, destination);};
	virtual void			BeginAction(ai_c &which_ai, edict_t &monster);
	virtual qboolean		Think(ai_c &which_ai, edict_t &monster);
	virtual qboolean		SafeToRemove(edict_t &monster){return true;};
	virtual actiontype_e	Type(void){return atype_default;};
	virtual decision_c		*GetOwnerDecision(void){return owner_decision;};
	virtual action_c		*GetOwnerAction(void){return owner_action;};

	void					SetOwnerAction(action_c *newowner){owner_action = newowner;}//grrrr...don't really like doing this, but need to in order to set up action chains

	virtual Event*			GetSignalEvent(){return SignalEvent;};
	virtual void			SetSignalEventState(qboolean done);
	virtual void			AttachSignalEvent(Event	*theEvent){SignalEvent = theEvent;};
	virtual void			SetInterrupted(const bool newval){interrupted = newval;}

	//0.0 percent_success reserved for aborted actions
	virtual void			ActionCompleted(action_c &which_action, ai_c &which_ai, edict_t &monster, float percent_success)
							{if (percent_success < 0.001)abortAction=true; else abortAction = false;};
				
				
							action_c(action_c *orig);
			void			Evaluate(action_c *orig);
	virtual	void			Write() { assert(0); }
	virtual	void			Read() { assert(0); }

	virtual int				GetClassCode(void) { return ACTION; }
static		action_c		*NewClassForCode(int code);
};

//useable leaf of basic action
class stand_action : public action_c
{
public:
	stand_action(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination = vec3_origin, vec3_t facing = vec3_origin, edict_t *target = NULL, float timeout = 99999999999, qboolean fullAnimation = false):
	action_c(od, oa, newanim, destination, facing, target, timeout, fullAnimation){};
	stand_action(void):action_c(){};

						stand_action(stand_action *orig);
			void		Evaluate(stand_action *orig);
	virtual	void		Write();
	virtual	void		Read();

	virtual int			GetClassCode(void) { return STAND_ACTION; }
};

/**********************************************************************************
 **********************************************************************************/

class pain_action : public action_c
{
private:
protected:
	virtual void		SetTurnDestination(ai_c &which_ai, edict_t &monster);

public:

	pain_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *other, float kick, int damage, float timeout=999999999999, qboolean fullAnimation = false);
	pain_action(void):action_c(){}
	virtual ~pain_action(void) {}

	virtual actiontype_e Type(void){return atype_pain;}
	virtual void		BeginAction(ai_c &which_ai, edict_t &monster);
	virtual qboolean	Think(ai_c &which_ai, edict_t &monster);

						pain_action(pain_action *orig);
			void		Evaluate(pain_action *orig);
	virtual	void		Write();
	virtual	void		Read();

	virtual int			GetClassCode(void) { return PAIN_ACTION; }
};

/**********************************************************************************
 **********************************************************************************/

class death_action : public action_c
{
protected:
	float			time_of_death;
	bool			drip;
	mmove_t			*anim2;

protected:
	virtual void		InterpretFlags(int cur_flags, ai_c &which_ai, edict_t &monster);//trigger frame-based actions in here
	virtual qboolean	AdjustBBox(ai_c &which_ai, edict_t &monster);
			void		TiltToFloor(ai_c &which_ai, edict_t &monster);
			void		AlternateTiltToFloor(ai_c &which_ai, edict_t &monster);

public:

	death_action(decision_c *od, action_c *oa, mmove_t *newanim1, mmove_t *newanim2, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
	death_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
	death_action(void):action_c(){drip = false;}
	virtual ~death_action(void) {}

	virtual actiontype_e Type(void){return atype_death;}
	virtual qboolean	SafeToRemove(edict_t &monster);
	virtual qboolean	Think(ai_c &which_ai, edict_t &monster);
	virtual void		BeginAction(ai_c &which_ai, edict_t &monster);

						death_action(death_action *orig);
			void		Evaluate(death_action *orig);
	virtual	void		Write();
	virtual	void		Read();

	virtual int			GetClassCode(void) { return DEATH_ACTION; }
};


/**********************************************************************************
 **********************************************************************************/

class deathcorpse_action : public death_action
{
private:

protected:

public:

	deathcorpse_action(decision_c *od, action_c *oa, mmove_t *newanim1, mmove_t *newanim2, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
	deathcorpse_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
	deathcorpse_action(void):death_action(){}
	virtual ~deathcorpse_action(void) {}

	virtual void		BeginAction(ai_c &which_ai, edict_t &monster);

						deathcorpse_action(deathcorpse_action *orig);
			void		Evaluate(deathcorpse_action *orig);
	virtual	void		Write();
	virtual	void		Read();

	virtual int			GetClassCode(void) { return DEATHCORPSE_ACTION; }
};

/**********************************************************************************
 **********************************************************************************/

class deatharmor_action : public death_action
{
private:

protected:

public:

	deatharmor_action(decision_c *od, action_c *oa, mmove_t *newanim1, mmove_t *newanim2, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
	deatharmor_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
	deatharmor_action(void):death_action(){}
	virtual ~deatharmor_action(void) {}

//	virtual actiontype_e Type(void){return atype_deatharmor;}
	virtual qboolean	SafeToRemove(edict_t &monster);
	virtual qboolean	Think(ai_c &which_ai, edict_t &monster);
	virtual void		BeginAction(ai_c &which_ai, edict_t &monster);

						deatharmor_action(deatharmor_action *orig);
			void		Evaluate(deatharmor_action *orig);
	virtual	void		Write();
	virtual	void		Read();

	virtual int			GetClassCode(void) { return DEATHARMOR_ACTION; }
};


/**********************************************************************************
 **********************************************************************************/

class endscript_action : public action_c
{
private:
protected:
public:
	endscript_action(decision_c *od);
	endscript_action(void):action_c(){}
	virtual ~endscript_action(void){}

	virtual void			BeginAction(ai_c &which_ai, edict_t &monster){}
	virtual qboolean	Think(ai_c &which_ai, edict_t &monster);

						endscript_action(endscript_action *orig);
			void		Evaluate(endscript_action *orig);
	virtual	void		Write();
	virtual	void		Read();

	virtual int			GetClassCode(void) { return ENDSCRIPT_ACTION; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////
///////	move around action branch
///////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**********************************************************************************
 **********************************************************************************/

class move_action : public action_c
{
private:
protected:
public:

	move_action(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false);
	move_action(void):action_c(){}
	virtual	void		Init(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false);
	virtual ~move_action(void) {}

	virtual actiontype_e Type(void){return atype_walk;}
	virtual qboolean	Think(ai_c &which_ai, edict_t &monster);

						move_action(move_action *orig);
	virtual void		Evaluate(move_action *orig);
};

class walk_action : public move_action
{
public:

	walk_action(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false)
	:move_action(od, oa, newanim, destination, facing, timeout, fullAnimation){};
	walk_action(void):move_action(){}

						walk_action(walk_action *orig);
			void		Evaluate(walk_action *orig);
	virtual	void		Write();
	virtual	void		Read();

	virtual int			GetClassCode(void) { return WALK_ACTION; }
};

class dog_walk_action : public move_action
{
private:
	mmove_t			*m_MoveStraightAnim;
	mmove_t			*m_MoveLeftAnim;
	mmove_t			*m_MoveRightAnim;
protected:
	virtual qboolean	Turn(ai_c &which_ai, edict_t &monster, turninfo_s &turninfo);
public:
	dog_walk_action(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false);
	dog_walk_action(void):move_action(){}
	virtual ~dog_walk_action(void) {}

						dog_walk_action(dog_walk_action *orig);
			void		Evaluate(dog_walk_action *orig);
	virtual	void		Write();
	virtual	void		Read();

	virtual int			GetClassCode(void) { return DOG_WALK_ACTION; }
};

/**********************************************************************************
 **********************************************************************************/
//when i'm on ground, end action (should perform jump in BeginAction)
class jump_action : public move_action
{
private:
	vec3_t	jumpvel;//horizontal velocity of jump--constantly restored
protected:
	virtual void		JumpToPosition(ai_c &which_ai, edict_t &monster, vec3_t jumpgoal);
public:
	jump_action(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t gohere, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false)
		:move_action(od,oa,newanim,gohere,facing,timeout){move_mask = step_movemask|jump_movemask;}
	jump_action(void):move_action(){}
	virtual ~jump_action(void) {}

	virtual actiontype_e Type(void){return atype_jump;}
	virtual qboolean	Think(ai_c &which_ai, edict_t &monster);
	virtual void		BeginAction(ai_c &which_ai, edict_t &monster);

	//abort action if returning from pain--this allows for throwback for damage
	virtual void		ActionCompleted(action_c &which_action, ai_c &which_ai, edict_t &monster, float percent_success)
							{if (percent_success < 0.001||which_action.Type()==atype_pain)abortAction=true; else abortAction = false;}

						jump_action(jump_action *orig);
			void		Evaluate(jump_action *orig);
	virtual	void		Write();
	virtual	void		Read();

	virtual int			GetClassCode(void) { return JUMP_ACTION; }
};

/**********************************************************************************
 **********************************************************************************/
//two stages: first,
//when i'm on ground, end action (should perform a jump in BeginAction, so it's really a jumpyfall)
class fall_action : public move_action
{
private:
	qboolean started_falling;
	mmove_t			*anim2;
protected:
	virtual		qboolean	PerformBodilyFunctions(ai_c &which_ai, edict_t &monster, vec3_t curDist);
public:
	fall_action(decision_c *od, action_c *oa, mmove_t *newanim, mmove_t *newanim2, vec3_t gohere, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false)
		:move_action(od,oa,newanim,gohere,facing,timeout){	started_falling = false; anim2=newanim2;}
	fall_action(void):move_action(){}
	virtual ~fall_action(void) {}

	virtual actiontype_e Type(void){return atype_fall;}
	virtual qboolean	Think(ai_c &which_ai, edict_t &monster);

	virtual void		StartFallAnim(ai_c &which_ai, edict_t &monster){}

						fall_action(fall_action *orig);
			void		Evaluate(fall_action *orig);
	virtual	void		Write();
	virtual	void		Read();

	virtual int			GetClassCode(void) { return FALL_ACTION; }
};

/**********************************************************************************
 **********************************************************************************/

class surrender_action : public move_action
{
private:
public:
	surrender_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *surrenderTo, vec3_t gohere, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false);
	surrender_action(void):move_action(){}
	virtual ~surrender_action(void) {}

	virtual actiontype_e Type(void){return atype_surrender;}
	virtual qboolean	Think(ai_c &which_ai, edict_t &monster);

						surrender_action(surrender_action *orig);
			void		Evaluate(surrender_action *orig);
	virtual	void		Write();
	virtual	void		Read();

	virtual int			GetClassCode(void) { return SURRENDER_ACTION; }
};

/**********************************************************************************
 **********************************************************************************/

class capture_action : public move_action
{
private:
protected:
public:
	capture_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t gohere, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false);
	capture_action(void):move_action(){}
	virtual ~capture_action(void) {}

	virtual actiontype_e Type(void){return atype_attack;}
	virtual qboolean	Think(ai_c &which_ai, edict_t &monster);

						capture_action(capture_action *orig);
			void		Evaluate(capture_action *orig);
	virtual	void		Write();
	virtual	void		Read();

	virtual int			GetClassCode(void) { return CAPTURE_ACTION; }
};

/**********************************************************************************
 **********************************************************************************/

class attack_action : public move_action
{
private:
	int		init_health;
	float	next_attack_time;

protected:
	virtual int			NextFlags(ai_c &which_ai, edict_t &monster);
public:
	attack_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t gohere,
		vec3_t facing = vec3_origin, float timeout=0/*999999999999*/, qboolean fullAnimation = false,
		bool shouldKill = false, int theNullTarget = 0);
	attack_action(void):move_action(){}
	virtual ~attack_action(void) {}
	virtual actiontype_e Type(void){return atype_attack;}
	virtual void		BeginAction(ai_c &which_ai, edict_t &monster);
	virtual qboolean	Think(ai_c &which_ai, edict_t &monster);

						attack_action(attack_action *orig);
	virtual void		Evaluate(attack_action *orig);
};

class shoot_attack_action : public attack_action
{
public:
	shoot_attack_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t gohere, vec3_t facing = vec3_origin, float timeout=0/*999999999999*/, qboolean fullAnimation = false, bool shouldKill = false, int NullTarget = 0)
		:attack_action(od, oa, newanim, attackTarget, gohere, facing, timeout, fullAnimation, shouldKill, NullTarget){}
	shoot_attack_action(void):attack_action(){}

						shoot_attack_action(shoot_attack_action *orig);
			void		Evaluate(shoot_attack_action *orig);
	virtual	void		Write();
	virtual	void		Read();

	virtual int			GetClassCode(void) { return SHOOT_ATTACK_ACTION; }
};

/**********************************************************************************
 **********************************************************************************/

#define MELEE_ACTION_NONE	0
#define MELEE_ACTION_JUMPED 1
#define MELEE_ACTION_HIT	2

class running_melee_attack_action : public attack_action
{
private:
	int		m_nActionCode;

protected:
	virtual qboolean	Turn(ai_c &which_ai, edict_t &monster, turninfo_s &turninfo);
	virtual void		SetTurnDestination(ai_c &which_ai, edict_t &monster);
public:
	running_melee_attack_action(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t gohere, vec3_t facing = vec3_origin, float timeout=0/*999999999999*/, qboolean fullAnimation = false);
	running_melee_attack_action(void):attack_action(){}
	virtual ~running_melee_attack_action(void) {}

	virtual void		InterpretFlags(int cur_flags, ai_c &which_ai, edict_t &monster);//trigger frame-based actions in here

						running_melee_attack_action(running_melee_attack_action *orig);
			void		Evaluate(running_melee_attack_action *orig);
	virtual	void		Write();
	virtual	void		Read();

	virtual int			GetClassCode(void) { return RUNNING_MELEE_ATTACK_ACTION; }
};

// end