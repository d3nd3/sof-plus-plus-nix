#if 1

typedef enum
{
	GZ_COW_HEAD,
	GZ_COW_CHEST,
	GZ_COW_R_FRONT,
	GZ_COW_L_FRONT,
	GZ_COW_R_BACK,
	GZ_COW_L_BACK,
	NUM_COWGOREZONES
} gz_cowcode;

//not including gorezones in this list, need to be handled separately
typedef enum
{
/*  GBLOWN_COW_LLEG_FRONT,
    GBLOWN_COW_RLEG_FRONT,
    GBLOWN_COW_LLEG_BACK,
	GBLOWN_COW_RHIP,
    GBLOWN_COW_RLEG_BACK,*/
	GBLOWN_COW_NUM = 0
} cow_blown_index;


extern gz_blown_part cow_blown_parts[GBLOWN_COW_NUM+1];


class bodycow_c: public bodyorganic_c
{
protected:
			qboolean			bTurnedAfterAttack;
			qboolean			FinalAnim; // When set, the current anim or series of anims is intended to be the last ones played.  also has sound implications
			qboolean			FinalSound;
			int					nNextShotKillsMe;	// used for scripted kills


			float			fJumpDist;	// preferred dist from target at which we'll do a jump-attack
			float			fJumpSpeed;	// magnitude of our jump-attack's velocity
			float			fLastAttackTime;
			float			fLastHeadTurnTime;
			float			m_fLastMooSoundTime;
			float			m_fLastPainSoundTime;
			float			m_fLastDeathSoundTime;

	virtual	void			RecognizeGoreZones(edict_t &monster);
	virtual void			PlayMooSound(edict_t &monster);
	virtual void			PlayPainSound(edict_t &monster);

	virtual	qboolean		PlayAnimation(edict_t &monster, mmove_t *newanim, bool forceRestart);

			//fixme: these should be the RecognizegoreZones for different classes--AddBody stuff needs to get resolved
			void			RecognizeCowGoreZones(edict_t &monster);

			virtual	void	AllocateGoreZones(edict_t &monster);
			virtual void	BiteMe(edict_t &monster, qboolean forceFireRate);
public:
						bodycow_c();
	virtual	mmove_t		*GetSequenceForDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
//	virtual mmove_t		*GetSequenceForMovement(edict_t &monster, vec3_t dest, vec3_t face, vec3_t org, vec3_t ang,  actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual mmove_t		*GetSequenceForStand(edict_t &monster, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual mmove_t		*GetSequenceForAttack(edict_t &monster, vec3_t dest, vec3_t face, edict_t *target, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForPain(edict_t &monster, vec3_t point, float kick, int damage, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);

	virtual void		NextMovement(edict_t &monster, vec3_t curDist, float scale);
	virtual void		FinishMove(edict_t &monster);		

	virtual void		SetRootBolt(edict_t &monster);

	virtual	int			ShowDamage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb);

	virtual	attacks_e	GetBestWeapon(edict_t &monster);
	virtual void		FirePrimaryWeapon(edict_t &monster, bool lethal = false, int NullTarget = 0, qboolean forceFireRate = false) { BiteMe(monster, forceFireRate); }

	// bodydog_c();

	virtual int			GetClassCode(void){return BODY_COW;}//this should be different for everybody in this class family

	float				GetLastAttackTime() { return fLastAttackTime; }
	void				SetLastAttackTime(float fTime) { fLastAttackTime = fTime; }
	virtual	void		NextShotsGonnaKillMe(int nKill) {nNextShotKillsMe = nKill;} 
	virtual	bool		IsNextShotGonnaKillMe() {return !!nNextShotKillsMe;} 

						bodycow_c(bodycow_c *orig);
	virtual void		Evaluate(bodycow_c *orig);
	virtual void		Write();
	virtual void		Read();
};


#endif
