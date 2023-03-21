
//ai_public

//ai include file for everyone's enjoyment


// noise types for PlayerNoise
typedef enum
{
	AI_SENSETYPE_UNKNOWN,

	AI_SENSETYPE_SOUND_SELF,
	AI_SENSETYPE_SOUND_WEAPON,
	AI_SENSETYPE_SOUND_IMPACT,
	AI_SENSETYPE_SOUND_WHIZ,
	AI_SENSETYPE_SOUND_MAGICAL,
	AI_SENSETYPE_SOUND_INVESTIGATE,
	AI_SENSETYPE_SOUND_WAKEUP,

	AI_SENSETYPE_SIGHT_CLEAR,
	AI_SENSETYPE_SIGHT_OBSTRUCTED,

	AI_SENSETYPE_MAGIC
} ai_sensetype_e;

#include "g_monster.h"


//ai class codes:
typedef enum
{
AI_PUBLIC,				
AI_BASE,					
AI_GENERIC,				
AI_GENERIC_NPC,			
AI_GENERIC_ENEMY_NPC,	
AI_GENERIC_DOG,			
AI_GENERIC_COW,			
AI_GENERIC_MERC,			

/*AI_GENERIC_MERC_GRUNT,	
AI_GENERIC_MERC_DEMO,	
AI_GENERIC_MERC_SNIPER,	
AI_GENERIC_MERC_MEDIC,	
AI_GENERIC_MERC_HEAVY,	
AI_GENERIC_MERC_TECH,	
*/
AI_HELI,					
AI_TANK,					
AI_SNOWCAT,					

AI_DOG_HUSKY,			
AI_DOG_ROTTWEILER,		
AI_BLOWNPART,
AI_PLAYERCORPSE,		

AI_ECTO_SKINLEADER,		
AI_ECTO_STOCKBROKER,		
AI_ECTO_DCTOURIST,		
AI_ECTO_NYCBUM,			
AI_ECTO_COLPRISONER1,	
AI_ECTO_COLPRISONER2,	
AI_ECTO_IRAQCITIZEN,		
AI_ECTO_UGNCOMMANDER,	
AI_ECTO_SERBOFFICER,		
AI_ECTO_KOSREFUGEE,		
AI_ECTO_IRAQOFFICER,		
AI_ECTO_FACTORY,			
AI_ECTO_CHEMIST,			
AI_ECTO_SIBSUIT,			
AI_ECTO_SIBSCIENCE,		
AI_ECTO_PUNK2,			

AI_MESO_JOHN,		
AI_MESO_JOHN_SNOW,		
AI_MESO_JOHN_DESERT,		
AI_MESO_HAWK,		
AI_MESO_HURTHAWK,		
AI_MESO_SAM,		
AI_MESO_IRAQWORKER,		
AI_MESO_NYCPUNK,			
AI_MESO_AMU,		
AI_MESO_RAIDERBOSS,		
AI_MESO_RAIDERBOSS2,		
AI_MESO_IRAQSOLDIER1,	
AI_MESO_IRAQSOLDIER2,	
AI_MESO_IRAQSOLDIER2B,	
AI_MESO_IRAQREPGUARD,	
AI_MESO_IRAQREPGUARDB,	
AI_MESO_IRAQPOLICE,		
AI_MESO_IRAQCOMMANDER,
AI_MESO_IRAQBRUTEA,
AI_MESO_IRAQBRUTEB,
AI_MESO_IRAQBODYGUARD,
AI_MESO_IRAQROCKET,
AI_MESO_IRAQSADDAM,
AI_MESO_IRAQMAN2,
AI_MESO_UGNSNIPER,		
AI_MESO_UGNSOLDIER1,		
AI_MESO_UGNSOLDIER1B,	
AI_MESO_UGNSOLDIER2,		
AI_MESO_UGNSOLDIER3,		
AI_MESO_NYCSWATGUY,		
AI_MESO_NYCSWATLEADER,		
AI_MESO_RAIDER1,			
AI_MESO_RAIDER2A,			
AI_MESO_RAIDER2B,			
AI_MESO_RAIDERBRUTE,			
AI_MESO_RAIDERROCKET,			
AI_MESO_SIBTROOPER2,		
AI_MESO_SIBCLEANSUIT,	
AI_MESO_SERBGRUNT1,		
AI_MESO_SERBGRUNT2,		
AI_MESO_SERBGRUNT3,		
AI_MESO_SERBSNIPER1A,		
AI_MESO_SERBSNIPER1B,		
AI_MESO_SERBCOMTROOP,		
AI_MESO_SERBBRUTE1A,		
AI_MESO_SERBBRUTE1B,		
AI_MESO_SERBMECHANIC,		
AI_MESO_KOSREBEL,		
AI_MESO_KOSKLAGUY,		
AI_MESO_SKINHEAD1,		
AI_MESO_SKINHEAD2A,		
AI_MESO_SKINHEAD2B,		
AI_MESO_SKINHEADBOSS,	
AI_MESO_MALEPOLITICIAN,	
AI_MESO_TOKMALEHOSTAGE,	
AI_MESO_TOKHENCH1,	
AI_MESO_TOKHENCH2,	
AI_MESO_TOKKILLER,	
AI_MESO_TOKNINJA,	
AI_MESO_TOKBRUTE,	
AI_MESO_JAPANSUIT,		
AI_MESO_UGNBRUTE,		
AI_MESO_UGNROCKET,		
AI_MESO_SIBTROOPER1A,	
AI_MESO_SIBTROOPER1B,	
AI_MESO_SIBGUARD,		
AI_MESO_SIBGUARD3,		
AI_MESO_SIBGUARD4,		
AI_MESO_SIBMECH,			

AI_FEMALE_SKINCHICK,		
AI_FEMALE_NYWOMAN,		
AI_FEMALE_SIBGUARD,		
AI_FEMALE_SIBSCIENCE,	
AI_FEMALE_IRQWOMAN1,		
AI_FEMALE_IRQWOMAN2,		
AI_FEMALE_TOKWOMAN1,		
AI_FEMALE_TOKWOMAN2,		
AI_FEMALE_TOKASSASSIN,		
AI_FEMALE_RAIDER,
AI_FEMALE_TAYLOR,		
} AI_CLASS_CODES;


#define alarm_mask				1
#define sight_mask				2
#define sound_mask				4

#define smask_all				0xffffffff


#define	TEAM_PUNKS				2
#define	TEAM_SKINHEADS			3
#define TEAM_INNOCENTS			4

typedef enum
{
	atype_default,
	atype_walk,
	atype_pain,
	atype_death,
	atype_attack,
	atype_jump,
	atype_fall,
	atype_surrender,
	atype_capture,
	atype_deatharmor
} actiontype_e;

typedef struct
{
	edict_t			*ent;//who
	vec3_t			pos;//where was it, last time i saw/heard it?
	float			time;//when did i last see/hear it?
	ai_sensetype_e	senseType;//what sort of sense event was it?
} sensedEntInfo_t;

typedef struct
{
	float max_turn;
	float min_turn;
} turninfo_s;

typedef enum
{
	smute_recov_none,
	smute_recov_instant,
	smute_recov_linear,
	smute_recov_exp
} smute_recovery;

#define smute_degree_severe	20.0	//sense is operating at 5%, which is severe, ok?
#define smute_degree_total	100.0	//sense is operating at 1%, which is close enough to totally muted, ok?

//possible emotions--sorted by priority.
typedef enum
{
	EMOTION_NORMAL=1,
	EMOTION_AFRAID,
	EMOTION_MEAN,
	EMOTION_PAIN,
	EMOTION_DEAD,
	EMOTION_TALK,
	EMOTION_MEANTALK,
	EMOTION_FEARTALK,
} emotion_index;

typedef enum
{
	ATTENTION_DISTRACTED,	// ignore all but the most blatant stimulus
	ATTENTION_IDLE,			// normal
	ATTENTION_ALERT,		// EXTREMELY alert - wide field of view, very sensitive to sound
	ATTENTION_ESCAPED,		// ER!!!  Um, this doesn't actually make any sense whatsoever... but I need a spot for this =(
}attention_index;

typedef enum
{
	PRIORITY_HIGH,
	PRIORITY_LOW,
}priority_index;

class ai_public_c
{
protected:
	virtual void	AddBody(edict_t *monster){}
	ai_public_c(edict_t *monster, char *subname){}
	ai_public_c(edict_t *monster){}
public:

	//leaving these vecs public for now...:(
	vec3_t	velocity;
	vec3_t	ideal_angles;

	ai_public_c(void){}
	virtual ~ai_public_c(void){}

	virtual 		qboolean IsFirstFrameTime(void){return false;}
	virtual	void	FirstFrame(edict_t *monster){}

	//adjust this to return something moderately compatible with player team types
	virtual int	 GetTeam(edict_t &monster){return 1;}
	virtual int  IsInnocent() { return 0; }

	virtual	const char*	GetInterpFile(char *theclass, char *subclass){return NULL;}
	virtual	const char*	GetObjectSkin(char *theclass, char *subclass){return NULL;}

	virtual qboolean SafeToRemove(edict_t &monster){return true;}

	virtual void	Emote(edict_t &monster, emotion_index new_emotion, float emotion_duration, qboolean scripted_emoting=false){}
	virtual emotion_index	GetMood(edict_t &monster){return EMOTION_NORMAL;}

	virtual void	SetPriority(priority_index newp){}

	virtual void NewSense(sense_c *new_sense, edict_t *monster){}
	virtual void NewAction(action_c *new_action, edict_t *monster, qboolean activatenow){}
	virtual void NewDecision(decision_c *new_decision, edict_t *monster){}
	virtual	void Think(edict_t &monster){}
	virtual qboolean Damage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb){return false;}

	virtual qboolean	IsActive(void){return true;}
	virtual	void		Activate(edict_t &monster){}

	virtual	void		SetTargetTime(float now, edict_t *target, vec3_t position){}
	virtual void		setFirstTargetTime(float newTime){}

	virtual	void		SetStartleability(qboolean newVal){}

	virtual void		SetHostage(qboolean bHostage) {}

	virtual void			UseMonster(edict_t *user){}
	virtual void			TouchMonster(edict_t *user){}

	virtual void			SetAbusedByTeam(qboolean newa){}
	virtual qboolean		GetAbusedByTeam(void){return false;}

	virtual void	MuteSenses(unsigned mask, float degree, smute_recovery recovery_code, float recovery_time){}
	virtual void	RegisterSenseEvent(unsigned mask, vec3_t event_origin, float event_time, edict_t *event_edict, ai_sensetype_e event_code){}
	virtual void	GetLookVector(vec3_t myaim){VectorCopy(vec3_up,myaim);}
	virtual attention_index	GetAttention(edict_t &monster){return ATTENTION_IDLE;}

	virtual	qboolean	HasHadTarget(void){return 0;}

	virtual int		GetRank(void){return 0;}
	virtual void	SetRank(int newrank){}

	virtual void	RegisterShotForDodge(vec3_t start, vec3_t end, edict_t *shooter){}
	virtual void	RegisterShotForReply(vec3_t start, vec3_t end, edict_t *shooter){}
	virtual void	Spook(float amount, vec3_t center){}

	virtual void	Escape(edict_t &monster){}//not so pleased about exposing this

	// Get a default action
	virtual action_c	*DefaultAction(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination = vec3_origin, vec3_t facing = vec3_origin, edict_t *target = NULL, float timeout= 0/*999999999999*/, qboolean fullAnimation = false){return NULL;}//for standing
	virtual action_c	*WalkAction(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false){return NULL;}
	virtual action_c	*PainAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *other, float kick, int damage, float timeout=999999999999, qboolean fullAnimation = false){return NULL;}
	virtual action_c	*DeathAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point){return NULL;}
	virtual action_c	*AttackAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t attackPos, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false){return NULL;}
	virtual action_c	*JumpAction(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false){return NULL;}
	virtual action_c	*FallAction(decision_c *od, action_c *oa, mmove_t *newanim, vec3_t destination, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false){return NULL;}
	virtual action_c	*SurrenderAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *surrenderTo, vec3_t attackerPos, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false){return NULL;}
	virtual action_c	*CaptureAction(decision_c *od, action_c *oa, mmove_t *newanim, edict_t *attackTarget, vec3_t attackPos, vec3_t facing = vec3_origin, float timeout=999999999999, qboolean fullAnimation = false){return NULL;}
	virtual action_c	*EndScriptAction(decision_c *od){return NULL;};

	virtual	void		WatchMe(void){}
	virtual qboolean	BeingWatched(void){return false;}
	virtual void		RequestMoveOutOfWay(vec3_t moveDir){};

	virtual void		UpdatePathPosition(edict_t *myEnt){}
	virtual int			getMyNode(void){return 0;}

	virtual	scripted_decision		*FindScriptedDecision(edict_t* ScriptEntity){return NULL;}
	virtual	void					CancelScripting(edict_t* ScriptEntity){}


						ai_public_c(ai_public_c *orig);
	virtual void		Evaluate(ai_public_c *orig);
	virtual void		Read() { assert(0); }
	virtual void		Write() { assert(0); }
static	 ai_public_c	*NewClassForCode(int code);
static	 ai_public_c	*NewClassForCode(int code, edict_t *monster);
	virtual int			GetClassCode(void){ return AI_PUBLIC; }
};
