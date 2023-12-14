#ifndef _AI_SENSES_H
#define _AI_SENSES_H

typedef struct
{
	vec3_t	heardLocation;
	vec3_t	origin;
	edict_t	*ent;
	ai_sensetype_e		code;
	float	time;
} aiSoundInfo;

//keep a static list of 10 sounds--that should be plenty, and can overwrite less important sounds if a superimportant one comes in
#define MAX_AISOUNDS 10

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
#define SENSE				0
#define ALARM_SENSE			1
#define SIGHT_SENSE			2
#define NORMALSIGHT_SENSE	3
#define SOUND_SENSE			4
#define OMNISCIENCE_SENSE	5
#define	MAGICSIGHT_SENSE	6

class sense_c
{
protected:
	vec3_t			origin;
	float			time;

	sensedEntInfo_t	sensed_monster;
	sensedEntInfo_t	sensed_client;

	smute_recovery	mute_recovercode;
	float			mute_starttime;
	float			mute_endtime;
	float			mute_degree;

	virtual	int		Range(edict_t *monster, edict_t *other);
	virtual	int		Range(edict_t *monster, vec3_t where);
	virtual bool	MaskOK(unsigned mask){return true;}
	virtual	void	RegisterEvent(vec3_t event_origin, float event_time, edict_t *event_edict, ai_sensetype_e event_code){}
	virtual	float	CurrentMutedLevel(void);

public:
	sense_c(vec3_t new_origin, float new_time);
	sense_c(void);
	virtual ~sense_c(void) {}

	virtual void	UpdateSensedClient(unsigned mask, sensedEntInfo_t &best_ent);
	virtual void	UpdateSensedMonster(unsigned mask, sensedEntInfo_t &best_ent);

	virtual	void	ChangeClientSenseType(ai_sensetype_e newType){sensed_client.senseType=newType;}
	virtual	void	ChangeMonsterSenseType(ai_sensetype_e newType){sensed_monster.senseType=newType;}

	virtual	float	GetMutedLevel(unsigned mask){if (MaskOK(mask))return CurrentMutedLevel();return 0;}//mwahahahaha! read THIS, muhfuh!
	virtual void	Mute(unsigned mask, float degree, smute_recovery recovery_code, float recovery_time){}
	virtual	void	RegisterSenseEvent(unsigned mask, vec3_t event_origin, float event_time, edict_t *event_edict, ai_sensetype_e event_code)
						{if (MaskOK(mask))RegisterEvent(event_origin,event_time,event_edict,event_code);}

	virtual qboolean Evaluate(unsigned mask, ai_c &owner_ai, edict_t &monster) {return true;}//returns whether this
																			//sense is resolved or not
	virtual qboolean	SafeToRemove(edict_t &monster){return true;}

	static	sense_c *NewClassForCode(int code);

							sense_c(sense_c *orig);
			void			Evaluate(sense_c *orig);
	virtual void			Write() { assert(0); }
	virtual void			Read() { assert(0); }
	virtual int				GetClassCode(void) {return SENSE; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	sound sense
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

class sound_sense : public sense_c
{
private:
	aiSoundInfo	sounds[MAX_AISOUNDS];
	int			numsounds;
protected:
	virtual bool	MaskOK(unsigned mask){return (1 && (mask&sound_mask));}
	virtual	void	RegisterEvent(vec3_t event_origin, float event_time, edict_t *event_edict, ai_sensetype_e event_code);
public:
	sound_sense(void):sense_c(){numsounds=0;}
	virtual ~sound_sense(void){}

	virtual void	Mute(unsigned mask, float degree, smute_recovery recovery_code, float recovery_time);
	
	virtual qboolean Evaluate(unsigned mask, ai_c &owner_ai, edict_t &monster);

					sound_sense(sound_sense *orig);
			void	Evaluate(sound_sense *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return SOUND_SENSE; }
};

class alarm_sense : public sense_c
{
protected:
	float	radius;
	virtual bool	MaskOK(unsigned mask){return (1 && (mask&alarm_mask));}

public:
	alarm_sense(vec3_t new_origin, float new_time, float new_radius);
	alarm_sense(void):sense_c(){}
	virtual ~alarm_sense(void){}

	virtual qboolean Evaluate(unsigned mask, ai_c &owner_ai, edict_t &monster);

					alarm_sense(alarm_sense *orig);
			void	Evaluate(alarm_sense *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return ALARM_SENSE; }
};

class sight_sense : public sense_c
{
protected:
	virtual bool	MaskOK(unsigned mask){return (1 && (mask&sight_mask));}
public:
	sight_sense(void):sense_c(){}

	virtual ~sight_sense(void){}
	virtual void	Mute(unsigned mask, float degree, smute_recovery recovery_code, float recovery_time);

			void	PoorVisibilityUpdate(ai_c &owner_ai, edict_t *monster, edict_t *sight_ent, float leeway_time);
	virtual void	Look(ai_c &owner_ai, edict_t *monster, edict_t *sight_ent);
	virtual qboolean Evaluate(unsigned mask, ai_c &owner_ai, edict_t &monster);

					sight_sense(sight_sense *orig);
			void	Evaluate(sight_sense *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return SIGHT_SENSE; }
};

class normalsight_sense : public sight_sense
{
public:
	normalsight_sense(void):sight_sense() {}

					normalsight_sense(normalsight_sense *orig);
			void	Evaluate(normalsight_sense *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return NORMALSIGHT_SENSE; }
};

//a testing class only
class omniscience_sense : public sight_sense
{
protected:
public:
	omniscience_sense(void):sight_sense() {}
	virtual void	Look(ai_c &owner_ai, edict_t *monster, edict_t *sight_ent);

					omniscience_sense(omniscience_sense *orig);
			void	Evaluate(omniscience_sense *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return OMNISCIENCE_SENSE; }
	virtual void	Mute(unsigned mask, float degree, smute_recovery recovery_code, float recovery_time);
};

class magicsight_sense : public sight_sense
{
protected:
	float			distSq;
public:
	magicsight_sense(void):sight_sense() {distSq=0;}
	magicsight_sense(float newDist):sight_sense() {distSq=newDist*newDist;}
	virtual void	Look(ai_c &owner_ai, edict_t *monster, edict_t *sight_ent);

					magicsight_sense(magicsight_sense *orig);
			void	Evaluate(magicsight_sense *orig);
	virtual void	Write();
	virtual void	Read();
	virtual int		GetClassCode(void) { return MAGICSIGHT_SENSE; }
	virtual void	Mute(unsigned mask, float degree, smute_recovery recovery_code, float recovery_time);
};

#endif