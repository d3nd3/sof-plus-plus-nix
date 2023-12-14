
typedef enum
{
	GZ_DOG_HEAD,
	GZ_DOG_LCHEST,
	GZ_DOG_RCHEST,
	GZ_DOG_FRONT,
	GZ_DOG_LLEG_FRONT,
	GZ_DOG_RLEG_FRONT,
	GZ_DOG_LHIP,
	GZ_DOG_RHIP,
	GZ_DOG_LLEG_BACK,
	GZ_DOG_RLEG_BACK,
	NUM_DOGGOREZONES
} gz_dogcode;

//not including gorezones in this list, need to be handled separately
typedef enum
{
    GBLOWN_HUSKY_LLEG_FRONT,
    GBLOWN_HUSKY_RLEG_FRONT,
    GBLOWN_HUSKY_LLEG_BACK,
	GBLOWN_HUSKY_RHIP,
    GBLOWN_HUSKY_RLEG_BACK,
	GBLOWN_HUSKY_NUM,
} husky_blown_index;

	// rottweiler (or rise of the triad)
typedef enum
{
    GBLOWN_ROTT_LLEG_FRONT,
    GBLOWN_ROTT_RLEG_FRONT,
    GBLOWN_ROTT_LLEG_BACK,
	GBLOWN_ROTT_RHIP,
    GBLOWN_ROTT_RLEG_BACK,
	GBLOWN_ROTT_NUM,
} rott_blown_index;


extern gz_blown_part husky_blown_parts[GBLOWN_HUSKY_NUM+1];
extern gz_blown_part rott_blown_parts[GBLOWN_ROTT_NUM+1];


class bodydog_c: public bodyorganic_c
{
protected:
			qboolean			bTurnedAfterAttack;
			qboolean			FinalAnim; // When set, the current anim or series of anims is intended to be the last ones played.  also has sound implications
			qboolean			FinalSound;


			float			fJumpDist;	// preferred dist from target at which we'll do a jump-attack
			float			fJumpSpeed;	// magnitude of our jump-attack's velocity
			float			fLastAttackTime;
			float			fLastHeadTurnTime;
			float			m_fLastBarkSoundTime;
			float			m_fLastBiteSoundTime;
			float			m_fLastPainSoundTime;

	virtual	void			RecognizeGoreZones(edict_t &monster);

	virtual	qboolean		PlayAnimation(edict_t &monster, mmove_t *newanim, bool forceRestart);
	virtual void			PlayBarkSound(edict_t &monster);
	virtual void			PlayBiteSound(edict_t &monster);
	virtual void			PlayPainSound(edict_t &monster);

			//fixme: these should be the RecognizegoreZones for different classes--AddBody stuff needs to get resolved
			void			RecognizeHuskyGoreZones(edict_t &monster);
			void			RecognizeRottweilerGoreZones(edict_t &monster);
	virtual	void			SetGoreZoneDamageScale(edict_t &monster);

			virtual void	BiteMe(edict_t &monster, qboolean forceFireRate);
public:
	virtual	mmove_t		*GetSequenceForDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual mmove_t		*GetSequenceForMovement(edict_t &monster, vec3_t dest, vec3_t face, vec3_t org, vec3_t ang,  actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual mmove_t		*GetSequenceForStand(edict_t &monster, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual mmove_t		*GetSequenceForJump(edict_t &monster, vec3_t dest, vec3_t face, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual mmove_t		*GetSequenceForAttack(edict_t &monster, vec3_t dest, vec3_t face, edict_t *target, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);

	virtual void		NextMovement(edict_t &monster, vec3_t curDist, float scale);
	virtual void		FinishMove(edict_t &monster);		

	virtual void		SetRootBolt(edict_t &monster);

	virtual	int			ShowDamage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb);

	virtual	attacks_e	GetBestWeapon(edict_t &monster);
	virtual void		FirePrimaryWeapon(edict_t &monster, bool lethal = false, int NullTarget = 0, qboolean forceFireRate = false) { BiteMe(monster, forceFireRate); }

	bodydog_c();

	virtual int			GetClassCode(void){return BODY_DOG;}//this should be different for everybody in this class family

	float				GetLastAttackTime() { return fLastAttackTime; }
	void				SetLastAttackTime(float fTime) { fLastAttackTime = fTime; }

						bodydog_c(bodydog_c *orig);
	virtual void		Evaluate(bodydog_c *orig);
};


class bodyhusky_c : public bodydog_c
{
protected:
	virtual	void			RecognizeGoreZones(edict_t &monster);
	virtual	gz_blown_part	*GetBlownPartForIndex(blown_index theIndex);
	virtual	void			AllocateGoreZones(edict_t &monster);
public:
						bodyhusky_c();
	virtual int			GetClassCode(void){return BODY_HUSKY;}//this should be different for everybody in this class family

						bodyhusky_c(bodyhusky_c *orig);
	virtual void		Evaluate(bodyhusky_c *orig);
	virtual void		Write();
	virtual void		Read();
};

class bodyrott_c : public bodydog_c
{
protected:
	virtual	void			RecognizeGoreZones(edict_t &monster);
	virtual	gz_blown_part	*GetBlownPartForIndex(blown_index theIndex);
	virtual	void			AllocateGoreZones(edict_t &monster);
public:
						bodyrott_c();
	virtual int			GetClassCode(void){return BODY_ROTT;}//this should be different for everybody in this class family

						bodyrott_c(bodyrott_c *orig);
	virtual void		Evaluate(bodyrott_c *orig);
	virtual void		Write();
	virtual void		Read();
};
