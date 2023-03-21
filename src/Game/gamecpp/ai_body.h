enum
{
	BODY,
	BODY_HUMAN,
	BODY_DOG,
	BODY_MESO,
	BODY_ECTO,
	BODY_HUSKY,
	BODY_ROTT,
	BODY_ORGANIC,
	BODY_FEMALE,
	BODY_HELI,
	BODY_TANK,
	BODY_SNOWCAT,
	BODY_COW,
	BODY_DEKKER
};

typedef enum
{
	HOLDCODE_NO,
	HOLDCODE_INAIR,
	HOLDCODE_GIBBED,
} hold_code;

typedef enum
{
	ACTSUB_ATTITUDE,
	ACTSUB_FEAR,
	ACTSUB_NORMAL,
} actionsub_code;

typedef enum
{
	ARMOR_NONE,
	ARMOR_CHEST,
	ARMOR_CHESTLIMBS,
	ARMOR_FULL,
	ARMOR_SOMEWHAT_TOUGH,//eric wants weaker flamethrower guys
	ARMOR_TOUGH
} armor_code;

enum deathvoice_code
{
	DEATHVOICE_DEFAULT,
	DEATHVOICE_SKINHEAD,
	DEATHVOICE_ARAB,
	DEATHVOICE_RUSSIAN,
	DEATHVOICE_ASIAN,
	DEATHVOICE_FEMALE,
	DEATHVOICE_RAIDER,
	DEATHVOICE_PLAYER//special deathvoice that overrides directory-chooser and specifies its directory explicitly
};

class body_c
{ 
protected:
			mmove_t			*currentmove;
			edict_t			*owner;
			qboolean		currentMoveFinished;
			hold_code		holdCode;
			int				frame_flags;
			float			emotion_expire;
			emotion_index	emotion;
			qboolean		emotionScripted;
			float			rLastFireTime;
			float			lLastFireTime;
			float			InitialKilledTime;//the time the guy is first killed
			float			LastKilledTime;//the most recent time he was killed
			float			lastStartTime;//last time i started an animation at...
			int				LastDFlags;
			bool			ForceNextAnim;
			bool			TrainDeath; // horrible awful hack put in by me...  somebody come up with something cool, please! dk

	virtual	qboolean		PlayAnimation(edict_t &monster, mmove_t *newanim, bool forceRestart);


	virtual	attacks_e		GetWeaponAtBolt(edict_t &monster, char *boltname){return ATK_NOTHING;}
			float			FireInterval(attacks_e weaponType, int curAmmo);

public:

	virtual	void			VoiceSound(char *base, edict_t &monster, int num, float vol = 1.0){}
	virtual	void			VoiceWakeSound(edict_t &monster, float vol = .6){}

			GhoulID			GetSequence(edict_t &monster, mmove_t *newanim);
	//this getsequencefor... is different from the others in that it actually creates an action--the others just return what move they prefer
	//also, the parameters passed in are very subject to change; the other getsequencefor...'s are pretty much set.
	virtual	mmove_t		*GetSequenceForDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0){return NULL;}
	virtual	mmove_t		*GetSequenceForPain(edict_t &monster, vec3_t point, float kick, int damage, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0){return NULL;}

	virtual mmove_t		*GetSequenceForMovement(edict_t &monster, vec3_t dest, vec3_t face, vec3_t org, vec3_t ang, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0){return NULL;}
	virtual mmove_t		*GetSequenceForStand(edict_t &monster, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0){return NULL;}
	virtual mmove_t		*GetSequenceForJump(edict_t &monster, vec3_t dest, vec3_t face, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0){return NULL;}
	virtual mmove_t		*GetSequenceForAttack(edict_t &monster, vec3_t dest, vec3_t face, edict_t *target, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0){return NULL;}
	virtual mmove_t		*GetSequenceForDodge(edict_t &monster, vec3_t dest, vec3_t face, edict_t *target, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int leftSide = 0, int reject_actionflags=0){return NULL;}
	virtual mmove_t		*GetSequenceForReload(edict_t &monster, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0){return NULL;}
	virtual	mmove_t		*GetPostShootSequence(edict_t &monster, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0){return NULL;}

	virtual bool		IsInStationaryDeathAnim() { return false; }

	virtual void		UseMonster(edict_t &monster, edict_t *user){}
	virtual void		TouchMonster(edict_t &monster, edict_t *user){}

	virtual	void		RightStep(edict_t &monster){}
	virtual	void		LeftStep(edict_t &monster){}

	virtual	void		Drip(edict_t &monster){}

	virtual qboolean	IsAvailableSequence(edict_t &monster, mmove_t *newanim);
	virtual qboolean	MatchAnimation(edict_t &monster, mmove_t *newanim);
	virtual qboolean	SetAnimation(edict_t &monster, mmove_t *newanim);
	virtual qboolean	ForceAnimation(edict_t &monster, mmove_t *newanim);
	virtual	void		HoldAnimation(edict_t &monster, hold_code whyHold, float holdTime);
	virtual	void		ResumeAnimation(edict_t &monster);
			qboolean	ApplySkin(edict_t &monster, ggOinstC *myInstance, char *matName, char *skinName);

			void		SetMove(mmove_t *newmove){currentmove = newmove;}
	virtual	void		FinishMove(edict_t &monster);
	virtual	void		SetFlags(edict_t & monster, int newflags){frame_flags|=newflags;}

	virtual	qboolean	IsAnimationFinished();
	//this is Very similar to IsAnimationFinished, but ignores whether anim is looping or not--because of low-frame anim silliness, you normally want to use IsAnimationFinished.
	virtual	qboolean	HasAnimationHitEOS();

	virtual	qboolean	IsAnimationHeld(hold_code queryCode);
	virtual qboolean	IsAnimationReversed();
	virtual void		NextMovement(edict_t &monster, vec3_t curDist, float scale);
	virtual void		NextTurn(float scale, edict_t &monster, float default_turn, turninfo_s &turninfo);
	virtual int			NextFlags(edict_t &monster);
	mmove_t				*GetMove(void){return currentmove;}

	virtual void		SetRootBolt(edict_t &monster){}
	virtual	void		SetArmor(edict_t &monster, armor_code newarmor){}
	virtual armor_code	GetArmorCode() { return ARMOR_NONE; }
	virtual float		GetArmorPercentRemaining() { return 0; }
	virtual void		StripArmor(){}

	virtual	int			ShowDamage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb){return damage;}

	weapons_t			GetWeaponType(ggBinstC *weapon);
	attacks_e			GetWeaponAttackType(ggBinstC *weapon);

							//set my specific and general directories
	virtual	void			SetVoiceDirectories(edict_t &monster, const char *base, int possible_varieties, deathvoice_code death, char *deathdir=NULL){}

	//forceFireRate will not fire the gun if it's been fired more recently than design doc says that kind of gun should fire.
	virtual void				FirePrimaryWeapon(edict_t &monster, bool lethal = false, int NullTarget = 0, qboolean forceFireRate = false) { FireRightHandWeapon(monster, lethal, NullTarget, forceFireRate); }
	virtual	void				FireRightHandWeapon(edict_t &monster, bool isLethal = false, int NullTarget = 0, qboolean forceFireRate = false);
	virtual	qboolean			RightHandWeaponReady(edict_t &monster);
	virtual void				ThrowProjectile(edict_t &monster){}

	virtual	attacks_e			GetRightHandWeapon(edict_t &monster){return ATK_NOTHING;}
	virtual	attacks_e			GetLeftHandWeapon(edict_t &monster){return ATK_NOTHING;}

	virtual	void				SetRightHandWeapon(edict_t &monster, attacks_e newRightWeap){}
	virtual	void				SetLeftHandWeapon(edict_t &monster, attacks_e newLeftWeap){}

	virtual	attacks_e			GetBestWeapon(edict_t &monster){return ATK_NOTHING;}

			void			Emote(edict_t &monster, emotion_index new_emotion, float emotion_duration, qboolean scripted_emoting=false);
			emotion_index	GetMood(edict_t &monster);
	virtual	void			UpdateFace(edict_t &monster);
	virtual void			SetFace(edict_t &monster, char *face){}

	virtual	ggBinstC	*AddBoltedItem(edict_t &monster, GhoulID mybolt, ggObjC *gun, char *itemSeq, GhoulID itemBolt, ggOinstC *myInstance, char *itemSkinName=NULL, float scale = 1.0);
	virtual	ggBinstC	*AddBoltedItem(edict_t &monster, GhoulID myBolt, char *itemClass, char *itemSeq, GhoulID itemBolt, ggOinstC *myInstance, char *itemSkinName=NULL, float scale = 1.0);
	virtual	ggBinstC	*AddBoltedItem(edict_t &monster, GhoulID myBolt, char *itemClass, char *itemSeq, char *itemBolt, ggOinstC *myInstance, char *itemSkinName=NULL, float scale = 1.0);
	virtual	ggBinstC	*AddBoltedItem(edict_t &monster, char *myBolt, char *itemClass, char *itemSeq, char *itemBolt, ggOinstC *myInstance, char *itemSkinName=NULL, float scale = 1.0);
	virtual	qboolean	AddRandomBoltedItem(float percent_chance, edict_t &monster, char *myBolt, char *itemClass, char *itemSeq, char *itemBolt, ggOinstC *myInstance, char *itemSkinName=NULL, float scale = 1.0);
	virtual	int			DropBoltOn(edict_t &monster, ggBinstC *boltedptr, Vect3 &pos, Vect3 &Direction, int damage, edict_t *attacker = NULL){return damage;}
	virtual void		FreeKnives(edict_t &monster){}

	virtual void		SetVocalCordsWorking(bool bCords) {}

	body_c();
	virtual				~body_c(){}

	virtual	void		MakeRetreatNoise(edict_t &monster){}

	virtual float		GetLastKilledTime(){return LastKilledTime;};
	virtual float		GetInitialKilledTime(){return InitialKilledTime;};
	virtual void		SetLastKilledTime(float time){LastKilledTime = time;};
	virtual void		SetInitialKilledTime(float time){InitialKilledTime = time;};
	virtual int			GetLastDFlags(){return LastDFlags;};
	virtual void		SetLastDFlags(int flags){LastDFlags = flags;};
	virtual bool		GetTrainDeath(){return TrainDeath;};
	virtual void		SetTrainDeath(bool condition){TrainDeath = condition;};

	virtual int			TestDamage(void){return 0;}

	virtual int			GetClassCode(void){return BODY;}//this should be different for everybody in this class family
	virtual int			IsMovementImpeded(void){return 0;}
static	body_c	 		*NewClassForCode(int code);
	
			void		SetOwner(edict_t *own) { owner = own; }

						body_c(body_c *orig);
	virtual void		Evaluate(body_c *orig);
	virtual void		Write(void) { assert(0); }
	virtual void		Read(void) { assert(0); }
};

// skins for bloodying the cockpit after the gunner or pilot gets sniped
#define HC_SKIN_GUNNER	1
#define HC_SKIN_PILOT	2
#define HC_SKIN_BOTH	3


class body_heli : public body_c
{
private:
	edict_t				*m_heli;
	gg_binst_c_ptr		m_gunBarrelInst;
	gg_binst_c_ptr		m_gunBarrelNULLInst;
	gg_binst_c_ptr		m_gunnerHeadInst;
	gg_binst_c_ptr		m_pilotHeadInst;
	gg_binst_c_ptr		m_cockpitInst;
	gg_binst_c_ptr		m_mainRotorInst;
	gg_binst_c_ptr		m_tailRotorInst;
					
	gg_obj_c_ptr		m_fuselageObj;
	gg_obj_c_ptr		m_gunBarrelObj;
	gg_obj_c_ptr		m_mainRotorObj;
	gg_obj_c_ptr		m_tailRotorObj;

	GhoulID				m_gunBolt;
	GhoulID				m_gunNULLBolt;
	GhoulID				m_gunnerHeadBolt;
	GhoulID				m_pilotHeadBolt;

	gg_binst_c_ptr		m_mainRotorBoltInst;
	gg_binst_c_ptr		m_tailRotorBoltInst;

	vec3_t				m_vGunBoltOffset;
	vec3_t				m_vGunnerBoltOffset;
	vec3_t				m_vPilotBoltOffset;

	int					m_nCanopyHealth;	// for use with sniper rifle shots
	qboolean			m_bPilotDead;
	qboolean			m_bGunnerDead;
	bool				m_bFiring;		// used by Cobra2_ToggleGun

	bool				m_bCanBeSniped;
public:
	// kef -- gotta make a lot of the heli's member variables public for now cuz
	//it'd take about 57 man-months to change Ste's movement code to access them
	//through public functions
	vec3_t				m_v3HeliDesiredPos_Start;
	vec3_t				m_v3HeliDesiredPos_Stop;
	vec3_t				m_v3HeliDesiredPos_Halfway;
	vec3_t				m_v3HeliPadRestPos;
	vec3_t				m_v3HeliFireAtCoords;
	vec3_t				m_v3HeliDesiredPos_Smart;
	vec3_t				m_v3HeliHeadStop;
	vec3_t				m_v3HeliTraceMins;
	vec3_t				m_v3HeliTraceMaxs;
	vec3_t				m_v3HeliDeathDest;
						
	float				m_fHeliTurnStartYaw;
	float				m_fHeliMoveSpeed;
	float				m_fHeliFireAtTimer;
	float				m_fHeliFirePitch;
	float				m_fHeliHoverTimer;
	int					m_iHeliHealth;
	float				m_fRotorSoundTimer;
	vec3_t				m_v3HeliFireAtCoords_Original;//	
	float				m_fHeliAITimer;//
	int					m_iHeliAITable;//
	int					m_iHeliAIIndex;//
						
	float				m_fLastDamageTime;
	float				m_fHeliRocketFireDebounceTime;
	int					m_iHeliRocketsLeft;	// 0..HELI_MAX_ROCKETS

	int					m_nRandGunWait;
	qboolean			m_bGunFiring;
	qboolean			m_bLOS;
	qboolean			m_bRotorEOS;
	qboolean			m_bFuselageEOS;

	// variables passed into script actions
	edict_t				*m_target;
	vec3_t				m_vPos;
	float				m_fArg;
						
	int					m_flags;

	// vars for heli's internal keep-itself-busy-when-not-scripted AI...
	//
	int					m_iLastEntFoundIndex;
	float				m_fLastGetEntTime;	
	edict_t				*m_AITarget;
	float				m_fNewTargetTimer;
	float				m_fSideStrafeDist;	//
	int					m_iSideStrafeDir;	// 0,1,2,3 clockwise from North
	int					m_iWhichRandVolume;	// for AI type 3 logic
	float				m_fAIAggressiveness;	// this needs to actually be set somewhere in the spawner, should be in range 0..10

	body_heli() {}
	body_heli(edict_t* self);

	int GetFlags() { return m_flags; }
	virtual	void SetFlags(edict_t & monster, int newflags) { m_flags = newflags; }

	edict_t	*GetHeliEdict() { return m_heli; }
	void SetHeliEdict(edict_t *monster);
	void SetBarrelInst(ggBinstC* inst) { m_gunBarrelInst = inst; }
	void SetBarrelNULLInst(ggBinstC* inst) { m_gunBarrelNULLInst = inst; }
	void SetGunnerInst(ggBinstC* inst) { m_gunnerHeadInst = inst; }
	void SetPilotInst(ggBinstC* inst) { m_pilotHeadInst = inst; }
	void SetCockpitInst(ggBinstC* inst) { m_cockpitInst = inst; }
	void SetMainRotorInst(ggBinstC* inst) { m_mainRotorInst = inst; }
	void SetTailRotorInst(ggBinstC* inst) { m_tailRotorInst = inst; }

	void SetFuselageObj(ggObjC* obj) { m_fuselageObj = obj; }
	void SetBarrelObj(ggObjC* obj) { m_gunBarrelObj = obj; }
	void SetMainRotorObj(ggObjC* obj) { m_mainRotorObj = obj; }
	void SetTailRotorObj(ggObjC* obj) { m_tailRotorObj = obj; }

	void SetBarrelBolt(GhoulID bolt) { m_gunBolt = bolt; }
	void SetBarrelNULLBolt(GhoulID bolt) { m_gunNULLBolt = bolt; }
	void SetGunnerBolt(GhoulID bolt) { m_gunnerHeadBolt = bolt; }
	void SetPilotBolt(GhoulID bolt) { m_pilotHeadBolt = bolt; }

	void SetMainRotorBoltInst(ggBinstC* inst) { m_mainRotorBoltInst = inst; }
	void SetTailRotorBoltInst(ggBinstC* inst) { m_tailRotorBoltInst = inst; }

	void SetGunBoltOffset(vec3_t vOffset) { VectorCopy(vOffset, m_vGunBoltOffset); }
	void SetGunBoltOffset(float x, float y, float z) { VectorSet(m_vGunBoltOffset, x, y, z); }
	void SetGunnerBoltOffset(vec3_t vOffset) { VectorCopy(vOffset, m_vGunnerBoltOffset); }
	void SetGunnerBoltOffset(float x, float y, float z) { VectorSet(m_vGunnerBoltOffset, x, y, z); }
	void SetPilotBoltOffset(vec3_t vOffset) { VectorCopy(vOffset, m_vPilotBoltOffset); }
	void SetPilotBoltOffset(float x, float y, float z) { VectorSet(m_vPilotBoltOffset, x, y, z); }

	IGhoulInst* GetHeliInst() { if (m_heli) return m_heli->ghoulInst; return NULL; }
	IGhoulInst* GetBarrelInst() { if (m_gunBarrelInst) return m_gunBarrelInst->GetInstPtr(); return NULL;}
	IGhoulInst* GetBarrelNULLInst() { if (m_gunBarrelNULLInst) return m_gunBarrelNULLInst->GetInstPtr(); return NULL;}
	IGhoulInst* GetGunnerInst() { if (m_gunnerHeadInst) return m_gunnerHeadInst->GetInstPtr(); return NULL;}
	IGhoulInst* GetPilotInst() { if (m_pilotHeadInst) return m_pilotHeadInst->GetInstPtr(); return NULL; }
	IGhoulInst* GetCockpitInst() { if (m_cockpitInst) return m_cockpitInst->GetInstPtr(); return NULL;}
	IGhoulInst* GetMainRotorInst() { if (m_mainRotorInst) return m_mainRotorInst->GetInstPtr(); return NULL;}
	IGhoulInst* GetTailRotorInst() { if (m_tailRotorInst) return m_tailRotorInst->GetInstPtr(); return NULL;}

	ggObjC* GetFuselageObj() { return m_fuselageObj; }
	ggObjC* GetBarrelObj() { return m_gunBarrelObj; }
	ggObjC* GetMainRotorObj() { return m_mainRotorObj; }
	ggObjC* GetTailRotorObj() { return m_tailRotorObj; }

	GhoulID GetBarrelBolt() { return m_gunBolt; }
	GhoulID GetBarrelNULLBolt() { return m_gunNULLBolt; }
	GhoulID GetGunnerBolt() { return m_gunnerHeadBolt; }
	GhoulID GetPilotBolt() { return m_pilotHeadBolt; }

	ggBinstC* GetMainRotorBoltInst() { return m_mainRotorBoltInst; }
	ggBinstC* GetTailRotorBoltInst() { return m_tailRotorBoltInst; }

	void GetGunBoltOffset(vec3_t vOut) { VectorCopy(m_vGunBoltOffset, vOut); }
	void GetGunnerBoltOffset(vec3_t vOut) { VectorCopy(m_vGunnerBoltOffset, vOut); }
	void GetPilotBoltOffset(vec3_t vOut) { VectorCopy(m_vPilotBoltOffset, vOut); }


	qboolean SetRotorEOS(qboolean b) { qboolean bOld = m_bRotorEOS; m_bRotorEOS = b; return bOld; }
	qboolean GetRotorEOS() { return m_bRotorEOS; }

	qboolean SetFuselageEOS(qboolean b) { qboolean bOld = m_bFuselageEOS; m_bFuselageEOS = b; return bOld; }
	qboolean GetFuselageEOS() { return m_bFuselageEOS; }

	void SetPosStart(vec3_t vPos) { VectorCopy(vPos, m_v3HeliDesiredPos_Start); }
	void SetPosStop(vec3_t vPos) { VectorCopy(vPos, m_v3HeliDesiredPos_Stop); }
	void SetPosHalfway(vec3_t vPos) { VectorCopy(vPos, m_v3HeliDesiredPos_Halfway); }
	void GetPosStart(vec3_t vPos) { VectorCopy(m_v3HeliDesiredPos_Start, vPos); }
	void GetPosStop(vec3_t vPos) { VectorCopy(m_v3HeliDesiredPos_Stop, vPos); }
	void GetPosHalfway(vec3_t vPos) { VectorCopy(m_v3HeliDesiredPos_Halfway, vPos); }

	void SetStartYaw(float fYaw) { m_fHeliTurnStartYaw = fYaw; }
	float GetStartYaw() { return m_fHeliTurnStartYaw; }

	virtual	int			ShowDamage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb);
	virtual void		UpdateSmoke(edict_t* self, float fHealthPercentage);

	void ResetScriptParams() {m_target = NULL; VectorSet(m_vPos,0,0,0); m_fArg = 0;}

	qboolean IsPilotDead() { return m_bPilotDead; }
	qboolean IsGunnerDead() { return m_bGunnerDead; }

	void ChangeCockpitSkin(int nSkin);
	void	SetFiring(bool bFiring);
	bool	IsFiring() { return m_bFiring; }

	void	SetSnipeability(bool bSnipeable) { m_bCanBeSniped = bSnipeable; }
	bool	IsSnipeable() { return m_bCanBeSniped; }

	virtual int			GetClassCode(void){return BODY_HELI;}//this should be different for everybody in this class family
static	body_heli 		*NewClassForCode(int code);

						body_heli(body_heli *orig);
	virtual void		Evaluate(body_heli *orig);
	virtual void		Write();
	virtual void		Read();
};

class body_tank : public body_c
{
public:
	// variables passed into script actions
	edict_t		*m_target;
	vec3_t		m_vPos;
	float		m_fArg;

protected:
	edict_t				*m_tank;			// the tank body is the root object (hopefully)
	gg_binst_c_ptr		m_turretInst;		// the turret
	gg_binst_c_ptr		m_cannonInst;		// the barrel of the main gun
	gg_binst_c_ptr		m_machGunInst;		// the machine gun on the turret
	gg_binst_c_ptr		m_machGunNullInst;	// between machine gun and the turret

	gg_obj_c_ptr		m_tankObj;
	gg_obj_c_ptr		m_turretObj;

	GhoulID				m_turretBolt;
	GhoulID				m_cannonBolt;
	GhoulID				m_machGunBolt;
	GhoulID				m_machGunNullBolt;

	gg_binst_c_ptr		m_turretBoltInst;

	bool				m_bDeactivated;
	bool				m_bTreads;
	bool				m_bTreadFire;
	float				m_fLastDamageTime;

	float				m_fMaxYawSpeed;
	float				m_fMaxFwdSpeed;
	float				m_fTurretSpeed;
	float				m_fCannonSpeed;
	float				m_fMachGunSpeed;

	float				m_fShellFlightDist;
	int					m_nMachGunHealth;
	vec3_t				m_vGroundZero;
public:
	body_tank() {}
	body_tank(edict_t* self);

	void SetTankEdict(edict_t *monster);
	void SetTurretInst(ggBinstC* inst) { m_turretInst = inst; }
	void SetCannonInst(ggBinstC* inst) { m_cannonInst = inst; }
	void SetMachGunInst(ggBinstC* inst) { m_machGunInst = inst; }
	void SetMachGunNullInst(ggBinstC* inst) { m_machGunNullInst = inst; }

	void SetTankObj(ggObjC* obj) { m_tankObj = obj; }
	void SetTurretObj(ggObjC* obj) { m_turretObj = obj; }

	void SetTurretBolt(GhoulID bolt) { m_turretBolt = bolt; }
	void SetCannonBolt(GhoulID bolt) { m_cannonBolt = bolt; }
	void SetMachGunBolt(GhoulID bolt) { m_machGunBolt = bolt; }
	void SetMachGunNullBolt(GhoulID bolt) { m_machGunNullBolt = bolt; }

	void SetTurretBoltInst(ggBinstC* inst) { m_turretBoltInst = inst; }

	edict_t*	GetTankEdict() { return m_tank; }
	IGhoulInst* GetTankInst() { if (m_tank) return m_tank->ghoulInst; return NULL; }
	IGhoulInst* GetTurretInst() { if (m_turretInst) return m_turretInst->GetInstPtr(); return NULL;}
	IGhoulInst* GetCannonInst() { if (m_cannonInst) return m_cannonInst->GetInstPtr(); return NULL;}
	IGhoulInst* GetMachGunInst() { if (m_machGunInst) return m_machGunInst->GetInstPtr(); return NULL;}
	IGhoulInst* GetMachGunNullInst() { if (m_machGunNullInst) return m_machGunNullInst->GetInstPtr(); return NULL;}

	ggObjC* GetTankObj() { return m_tankObj; }
	ggObjC* GetTurretObj() { return m_turretObj; }

	GhoulID GetTurretBolt() { return m_turretBolt; }
	GhoulID GetCannonBolt() { return m_cannonBolt; }
	GhoulID GetMachGunBolt() { return m_machGunBolt; }
	GhoulID GetMachGunNullBolt() { return m_machGunNullBolt; }

	ggBinstC* GetTurretBoltInst() { return m_turretBoltInst;}

	virtual void	SetTreads(bool bOn);
	bool			GetTreads() { return m_bTreads; }
	void			SetTreadFire(bool bOn) { m_bTreadFire = bOn; }
	bool			GetTreadFire() { return m_bTreadFire; }
	virtual void	Deactivate(bool bDeactivate) { m_bDeactivated = bDeactivate; }
	bool			IsDeactivated() { return m_bDeactivated; }

	virtual	int			ShowDamage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb);
	virtual void		UpdateSmoke(edict_t* self, float fHealthPercentage);

	void				ResetScriptParams() {m_target = NULL; VectorSet(m_vPos,0,0,0); m_fArg = 0;}
	float				GetMaxYawSpeed() { return m_fMaxYawSpeed; } // degrees
	float				GetMaxFwdSpeed() { return m_fMaxFwdSpeed; } // tank speed
	float				GetTurretSpeed() { return m_fTurretSpeed; }	// yaw, in radians
	float				GetCannonSpeed() { return m_fCannonSpeed; }	// pitch, in radians
	float				GetMachGunSpeed() { return m_fMachGunSpeed; }	// pitch and yaw, in radians
	float				GetCurProjectileDist() { return m_fShellFlightDist; }
	void				SetCurProjectileDist(float fDist) { m_fShellFlightDist = fDist; }
	void				GetGroundZero(vec3_t vPos) { VectorCopy(m_vGroundZero, vPos); }
	void				SetGroundZero(vec3_t vPos) { VectorCopy(vPos, m_vGroundZero); }
	bool				IsMachGunFunctioning() { return (m_nMachGunHealth > 0); }

	virtual int			GetClassCode(void){return BODY_TANK;}//this should be different for everybody in this class family
static	body_tank 		*NewClassForCode(int code);

						body_tank(body_tank *orig);
	virtual void		Evaluate(body_tank *orig);
	virtual void		Write();
	virtual void		Read();
};

class body_snowcat : public body_tank
{
private:
	gg_binst_c_ptr		m_machGun2Inst;			// the machine gun on the turret
	gg_binst_c_ptr		m_leftHeadLightInst;	// left headlight beam
	gg_binst_c_ptr		m_rightHeadLightInst;	// right headlight beam

	GhoulID				m_machGun2Bolt;
	GhoulID				m_leftHeadLightBolt;
	GhoulID				m_rightHeadLightBolt;

public:
	body_snowcat() {}
	body_snowcat(edict_t* self);

	virtual void	SetTreads(bool bOn);
	void SetMachGun2Inst(ggBinstC* inst) { m_machGun2Inst = inst; }
	void SetMachGun2Bolt(GhoulID bolt) { m_machGun2Bolt = bolt; }

	void SetLeftHeadLightInst(ggBinstC* inst) { m_leftHeadLightInst = inst; }
	void SetLeftHeadLightBolt(GhoulID bolt) { m_leftHeadLightBolt = bolt; }

	void SetRightHeadLightInst(ggBinstC* inst) { m_rightHeadLightInst = inst; }
	void SetRightHeadLightBolt(GhoulID bolt) { m_rightHeadLightBolt = bolt; }

	IGhoulInst* GetMachGun2Inst() { if (m_machGun2Inst) return m_machGun2Inst->GetInstPtr(); return NULL;}
	IGhoulInst* GetLeftHeadLightInst() { if (m_leftHeadLightInst) return m_leftHeadLightInst->GetInstPtr(); return NULL;}
	IGhoulInst* GetRightHeadLightInst() { if (m_rightHeadLightInst) return m_rightHeadLightInst->GetInstPtr(); return NULL;}

	GhoulID GetMachGun2Bolt() { return m_machGun2Bolt; }
	GhoulID GetLeftHeadLightBolt() { return m_leftHeadLightBolt; }
	GhoulID GetRightHeadLightBolt() { return m_rightHeadLightBolt; }

	virtual void		UpdateSmoke(edict_t* self, float fHealthPercentage);
	virtual void		Deactivate(bool bDeactivate);

	virtual int			GetClassCode(void){return BODY_SNOWCAT;}//this should be different for everybody in this class family
static	body_snowcat	*NewClassForCode(int code);

						body_snowcat(body_snowcat *orig);
	virtual void		Evaluate(body_snowcat *orig);
	virtual void		Write();
	virtual void		Read();
};


// end