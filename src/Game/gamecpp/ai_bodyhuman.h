

extern int bullet_numHits;
extern HitRecord bullet_Hits[20];
extern Vect3 bullet_EntDir,bullet_EntStart;

typedef int gz_code;

// cos of the maximum firing wedge - this used to be 60 degs in either direction (!)
#define MAX_FIRE_WEDGE .717

enum// used for scripted kills
{
	NEXTSHOT_NULL	= 0,
	NEXTSHOT_KILLS,
	NEXTSHOT_EXPLODE_MY_NUG, 
};

typedef enum
{
	GZ_HEAD_FRONT,
	GZ_HEAD_BACK,
	GZ_NECK,
	GZ_SHLDR_RIGHT_FRONT,
	GZ_SHLDR_RIGHT_BACK,
	GZ_SHLDR_LEFT_FRONT,
	GZ_SHLDR_LEFT_BACK,
	GZ_CHEST_FRONT,
	GZ_CHEST_BACK,
	GZ_ARM_UPPER_RIGHT,
	GZ_ARM_UPPER_LEFT,
	GZ_ARM_LOWER_RIGHT,
	GZ_ARM_LOWER_LEFT,
	GZ_GUT_FRONT,
	GZ_GUT_BACK,
	GZ_GROIN,
	GZ_LEG_UPPER_RIGHT_FRONT,
	GZ_LEG_UPPER_RIGHT_BACK,
	GZ_LEG_UPPER_LEFT_FRONT,
	GZ_LEG_UPPER_LEFT_BACK,
	GZ_LEG_LOWER_RIGHT,
	GZ_LEG_LOWER_LEFT,
	GZ_FOOT_RIGHT,
	GZ_FOOT_LEFT,
	GZ_GUT_FRONT_EXTRA,
	GZ_GUT_BACK_EXTRA,
	NUM_HUMANGOREZONES
} gz_humancode;

#define blown_index int

typedef enum
{
	GBLOWNAREA_CHEST,
	GBLOWNAREA_RFSHLD,
	GBLOWNAREA_RBSHLD,		
	GBLOWNAREA_LFSHLD,		
	GBLOWNAREA_LBSHLD,		
	GBLOWNAREA_NECK,
	GBLOWNAREA_HEAD,
	GBLOWNAREA_LBICEP,
	GBLOWNAREA_LFOREARM,
	GBLOWNAREA_LHAND,
	GBLOWNAREA_RBICEP,
	GBLOWNAREA_RFOREARM,
	GBLOWNAREA_RHAND,
	GBLOWNAREA_LTHIGH,
	GBLOWNAREA_LCALF,
	GBLOWNAREA_LFOOT,
	GBLOWNAREA_RTHIGH,
	GBLOWNAREA_RCALF,
	GBLOWNAREA_RFOOT,
	GBLOWNAREA_GROIN,
	GBLOWNAREA_FINISH

} blown_area;

//not including gorezones in this list, need to be handled separately
typedef enum
{
	//ecto:
	GBLOWN_ECTO_CHEST,				
	GBLOWN_ECTO_SHOULD_RF,
	GBLOWN_ECTO_SHOULD_RB,
	GBLOWN_ECTO_SHOULD_LF,
	GBLOWN_ECTO_SHOULDBOLT_L,	
	GBLOWN_ECTO_SHOULDBOLT_R,	
	GBLOWN_ECTO_BACKBOLT,		
	GBLOWN_ECTO_SHOULD_LB,
	GBLOWN_ECTO_COATCHEST,			

	//shared:
	GBLOWN_ECTO_CHESTFINISH,			

	//ecto:
	GBLOWN_ECTO_HEADBALD,			
	GBLOWN_ECTO_HEADCREW,			
	GBLOWN_ECTO_HEADBEARD,			
	GBLOWN_ECTO_HEADHELMET,			
	GBLOWN_ECTO_BANGLEFT,			
	GBLOWN_ECTO_BANGRIGHT,			
	GBLOWN_ECTO_HAIRMOHAWK,			
	GBLOWN_ECTO_HAIRPONYTAIL,		

	//shared:
	GBLOWN_ECTO_EARLEFT,				
	GBLOWN_ECTO_EARRIGHT,			
	GBLOWN_ECTO_HEADBOLT,			

	//ecto:
	GBLOWN_ECTO_BICEPLEFT,			

	//ecto:
	GBLOWN_ECTO_FOREARMLEFT,			
	GBLOWN_ECTO_FOREARMCUFFLEFT,		
	GBLOWN_ECTO_FOREARMCUFFCAPLEFT,	
	GBLOWN_ECTO_OPENHANDLEFT,		
	GBLOWN_ECTO_CLOSEDHANDLEFT,		

	//shared:
	GBLOWN_ECTO_HANDBOLTLEFT,		

	//ecto:
	GBLOWN_ECTO_BICEPRIGHT,			

	//ecto:
	GBLOWN_ECTO_FOREARMRIGHT,		
	GBLOWN_ECTO_FOREARMCUFFRIGHT,	
	GBLOWN_ECTO_FOREARMCUFFCAPRIGHT,	
	GBLOWN_ECTO_OPENHANDRIGHT,		
	GBLOWN_ECTO_CLOSEDHANDRIGHT,		

	//shared:
	GBLOWN_ECTO_HANDBOLTRIGHT,		

	//ecto:
	GBLOWN_ECTO_THIGHRIGHT,			

	//shared:
	GBLOWN_ECTO_THIGHBOLTRIGHT,		
	GBLOWN_ECTO_THIGHRIGHTSPECIAL,	

	//ecto:
	GBLOWN_ECTO_CALFRIGHT,			
	GBLOWN_ECTO_CALFCUFFRIGHT,		
	GBLOWN_ECTO_CALFCUFFCAPRIGHT,	

	//shared:
	GBLOWN_ECTO_CALFBOLTRIGHT,		

	//ecto:
	GBLOWN_ECTO_FOOTRIGHT,			

	//ecto:
	GBLOWN_ECTO_THIGHLEFT,			

	//shared:
	GBLOWN_ECTO_THIGHBOLTLEFT,		
	GBLOWN_ECTO_THIGHLEFTSPECIAL,	

	//ecto:
	GBLOWN_ECTO_CALFLEFT,			
	GBLOWN_ECTO_CALFCUFFLEFT,		
	GBLOWN_ECTO_CALFCUFFCAPLEFT,		

	//shared:
	GBLOWN_ECTO_CALFBOLTLEFT,		

	//ecto:
	GBLOWN_ECTO_FOOTLEFT,			
	
	GBLOWN_ECTO_GROIN,
	GBLOWN_ECTO_NUM
} ecto_blown_index;


typedef enum
{
	GBLOWN_FEMALE_CHEST,				
	GBLOWN_FEMALE_SHOULD_RF,
	GBLOWN_FEMALE_SHOULD_RB,
	GBLOWN_FEMALE_SHOULD_LF,
	GBLOWN_FEMALE_SHOULD_LB,
	GBLOWN_FEMALE_SHOULDBOLT_L,	
	GBLOWN_FEMALE_SHOULDBOLT_R,	
	GBLOWN_FEMALE_BACKBOLT,		
	GBLOWN_FEMALE_NECK,					
	GBLOWN_FEMALE_ARMORCHEST,			
	GBLOWN_FEMALE_CHESTFINISH,			

	GBLOWN_FEMALE_HEAD,					
	GBLOWN_FEMALE_FACE,					
	GBLOWN_FEMALE_CREWHAIR,				
	GBLOWN_FEMALE_MEDHAIR,				
	GBLOWN_FEMALE_BANGLEFT,				
	GBLOWN_FEMALE_BANGRIGHT,			

	GBLOWN_FEMALE_EARLEFT,				
	GBLOWN_FEMALE_EARRIGHT,				
	GBLOWN_FEMALE_HEADBOLT,				

	GBLOWN_FEMALE_BICEPLEFT,			
	GBLOWN_FEMALE_FOREARMLEFT,			
	GBLOWN_FEMALE_OPENHANDLEFT,			
	GBLOWN_FEMALE_CLOSEDHANDLEFT,		
	GBLOWN_FEMALE_HANDBOLTLEFT,			

	GBLOWN_FEMALE_BICEPRIGHT,			
	GBLOWN_FEMALE_FOREARMRIGHT,			
	GBLOWN_FEMALE_OPENHANDRIGHT,		
	GBLOWN_FEMALE_CLOSEDHANDRIGHT,		
	GBLOWN_FEMALE_HANDBOLTRIGHT,		

	GBLOWN_FEMALE_THIGHRIGHT,			
	GBLOWN_FEMALE_THIGHBOLTRIGHT,		
	GBLOWN_FEMALE_THIGHRIGHTSPECIAL,	

	GBLOWN_FEMALE_CALFRIGHT,			
	GBLOWN_FEMALE_CALFBOLTRIGHT,		
	GBLOWN_FEMALE_FOOTRIGHT,			

	GBLOWN_FEMALE_THIGHLEFT,			
	GBLOWN_FEMALE_THIGHBOLTLEFT,		
	GBLOWN_FEMALE_THIGHLEFTSPECIAL,		

	GBLOWN_FEMALE_CALFLEFT,				
	GBLOWN_FEMALE_CALFBOLTLEFT,			
	GBLOWN_FEMALE_FOOTLEFT,				

	GBLOWN_FEMALE_GROIN,				
	GBLOWN_FEMALE_NUM,					
} female_blown_index;

//not including gorezones in this list, need to be handled separately
typedef enum
{
	//meso:
	GBLOWN_MESO_TIGHTCHEST,			
	GBLOWN_MESO_MASKCHEST,			
	GBLOWN_MESO_BULKYCHEST,			
	GBLOWN_MESO_ARMORCHEST,			
	GBLOWN_MESO_BOSSCHEST,			
	GBLOWN_MESO_BTCHEST,				
	GBLOWN_MESO_NECK,				
	GBLOWN_MESO_BOSSNECK,
	GBLOWN_MESO_MASKNECK,			
	GBLOWN_MESO_HOODNECK,			
	GBLOWN_MESO_HEADCAP_NECK,		
	GBLOWN_MESO_HEADCAP_BOSSNECK,	
	GBLOWN_MESO_SHOULD_RF,
	GBLOWN_MESO_SHOULD_RB,
	GBLOWN_MESO_SHOULD_LF,
	GBLOWN_MESO_SHOULD_LB,
	GBLOWN_MESO_SHOULDBOLT_L,	
	GBLOWN_MESO_SHOULDBOLT_R,	
	GBLOWN_MESO_BACKBOLT,		
	GBLOWN_MESO_CHEST_RARMCAP3,		
	GBLOWN_MESO_CHEST_LARMCAP3,		
	GBLOWN_MESO_RARMCAP_CHEST3,		
	GBLOWN_MESO_LARMCAP_CHEST3,		
	GBLOWN_MESO_CHEST_RARMCAP4,		
	GBLOWN_MESO_CHEST_LARMCAP4,		
	GBLOWN_MESO_RARMCAP_CHEST4,		
	GBLOWN_MESO_LARMCAP_CHEST4,		

	//shared:
	GBLOWN_MESO_CHESTFINISH,			

	//meso:
	GBLOWN_MESO_HEADBALD,			
	GBLOWN_MESO_HEADCREW,			
	GBLOWN_MESO_HEADBEARD,			
	GBLOWN_MESO_HEADHELMET,			
	GBLOWN_MESO_HEADHOOD,			
	GBLOWN_MESO_HEADMASK,			
	GBLOWN_MESO_HEADBOSS,			
	GBLOWN_MESO_COMFACE,				
	GBLOWN_MESO_BEARDFACE,			
	GBLOWN_MESO_HELMETFACE,			
	GBLOWN_MESO_BOSSFACE,			
	GBLOWN_MESO_BANGLEFT,			
	GBLOWN_MESO_BANGRIGHT,			
	GBLOWN_MESO_HAIRMOHAWK,			
	GBLOWN_MESO_HAIRPONYTAIL,		

	//shared:
	GBLOWN_MESO_EARLEFT,				
	GBLOWN_MESO_EARRIGHT,		
	GBLOWN_MESO_BOSSEARLEFT,
	GBLOWN_MESO_BOSSEARRIGHT,
	GBLOWN_MESO_HEADBOLT,			

	//meso:
	GBLOWN_MESO_BICEPLEFT,			
	GBLOWN_MESO_BICEPLEFT2,			
	GBLOWN_MESO_BICEPLEFT3,			
	GBLOWN_MESO_BICEPLEFT4,			
	GBLOWN_MESO_BICEPLEFT5,			

	//meso:
	GBLOWN_MESO_FOREARMLEFT,		
	GBLOWN_MESO_FOREARMLEFT2,		
	GBLOWN_MESO_FOREARMLEFT3,		
	GBLOWN_MESO_FOREARMLEFT4,		
	GBLOWN_MESO_FOREARMLEFT5,		
	GBLOWN_MESO_FOREARMLEFT6,		
	GBLOWN_MESO_OPENHANDLEFT,		
	GBLOWN_MESO_CLOSEDHANDLEFT,		
	GBLOWN_MESO_OPENHANDLEFT2,		
	GBLOWN_MESO_CLOSEDHANDLEFT2,		

	//shared:
	GBLOWN_MESO_HANDBOLTLEFT,		

	//meso:
	GBLOWN_MESO_BICEPRIGHT,			
	GBLOWN_MESO_BICEPRIGHT2,			
	GBLOWN_MESO_BICEPRIGHT3,			
	GBLOWN_MESO_BICEPRIGHT4,			
	GBLOWN_MESO_BICEPRIGHT5,			

	//meso:
	GBLOWN_MESO_FOREARMRIGHT,		
	GBLOWN_MESO_FOREARMRIGHT2,		
	GBLOWN_MESO_FOREARMRIGHT3,		
	GBLOWN_MESO_FOREARMRIGHT4,		
	GBLOWN_MESO_FOREARMRIGHT5,		
	GBLOWN_MESO_FOREARMRIGHT6,		
	GBLOWN_MESO_OPENHANDRIGHT,		
	GBLOWN_MESO_CLOSEDHANDRIGHT,	
	GBLOWN_MESO_OPENHANDRIGHT2,		
	GBLOWN_MESO_CLOSEDHANDRIGHT2,	

	//shared:
	GBLOWN_MESO_HANDBOLTRIGHT,		

	//meso:
	GBLOWN_MESO_THIGHRIGHT,			
	GBLOWN_MESO_THIGHRIGHT2,			
	GBLOWN_MESO_THIGHRIGHT3,			

	//shared:
	GBLOWN_MESO_THIGHBOLTRIGHT,		
	GBLOWN_MESO_THIGHRIGHTSPECIAL,	

	//meso:
	GBLOWN_MESO_CALFRIGHT,			
	GBLOWN_MESO_CALFRIGHT2,			
	GBLOWN_MESO_CALFRIGHT3,			
	GBLOWN_MESO_CALFRIGHT4,			
	GBLOWN_MESO_CALFRIGHT5,			

	//shared:
	GBLOWN_MESO_CALFBOLTRIGHT,		

	//meso:
	GBLOWN_MESO_FOOTRIGHT,			
	GBLOWN_MESO_FOOTRIGHT2,			

	//meso:
	GBLOWN_MESO_THIGHLEFT,			
	GBLOWN_MESO_THIGHLEFT2,			
	GBLOWN_MESO_THIGHLEFT3,			

	//shared:
	GBLOWN_MESO_THIGHBOLTLEFT,		
	GBLOWN_MESO_THIGHLEFTSPECIAL,	

	//meso:
	GBLOWN_MESO_CALFLEFT,			
	GBLOWN_MESO_CALFLEFT2,			
	GBLOWN_MESO_CALFLEFT3,			
	GBLOWN_MESO_CALFLEFT4,			
	GBLOWN_MESO_CALFLEFT5,			

	//shared:
	GBLOWN_MESO_CALFBOLTLEFT,		

	//meso:
	GBLOWN_MESO_FOOTLEFT,			
	GBLOWN_MESO_FOOTLEFT2,
	
	GBLOWN_MESO_GROIN,
	GBLOWN_MESO_NUM
} meso_blown_index;

typedef enum
{
	GBLOWNCODE_PART,
	GBLOWNCODE_BOLT,
	GBLOWNCODE_SPECIAL
} blown_code;


typedef struct
{
	blown_index	my_index;
	blown_index	child_index;
	blown_area	area;
	blown_code	code;
	char		name[30];
} gz_blown_part;

extern gz_blown_part ecto_blown_parts[];
extern gz_blown_part meso_blown_parts[];
extern gz_blown_part female_blown_parts[];

typedef struct gz_info_s
{
	GhoulID	id;
	GhoulID	parent_id;//to check if there's body available to put gore on...
	GhoulID	bolt;

	GhoulID	cap;
	GhoulID	blownCap;
	GhoulID	blownCapBolt;
	GhoulID	capbolt;
	ggObjC	*capPiece;
	GhoulID	capPieceBolt;
	gz_code	childzone;
	gz_code	buddyzone;//keep my gore levels equivalent with my buddyzone's
	gz_blown_part	*blowme;

	int		damage;//not sure whether to use raw damage or damagelevel increments or both...
	int		damage_level;

	float	damScale;//amount to scale damage, once armor is depleted

} gz_info;

#define	BODYDAMAGE_RIGHTARM		0x00000001	
#define	BODYDAMAGE_LEFTARM		0x00000002	
#define	BODYDAMAGE_RIGHTLEG		0x00000004	
#define BODYDAMAGE_LEFTLEG		0x00000008

#define	BODYDAMAGE_UNKOWNAREA	0x40000000	//i'm damaged in a definitely unkown area 
												//(ears currently qualify, also any known area not on the bodydamage list)
#define	BODYDAMAGE_RECENT		0x80000000	//i've been hit recently--disable me in addition to pain animation
	
#define BODYDAMAGE_ANY			0xffffffff	//for convenient checking
#define BODYDAMAGE_NONE			0x00000000	//for convenient checking

class bodyorganic_c: public body_c
{
protected:
			qboolean		useGround2;
			int				spout;
			GhoulID			gzone_levels[3][4];
			int				numGoreZones;
			gz_info			*gzones;

	virtual	void			SetGoreZoneDamageScale(edict_t &monster){}
	virtual	void			InitializeGoreZoneLevels(edict_t &monster);
	virtual	gz_blown_part	*GetBlownPartForIndex(blown_index theIndex){return &ecto_blown_parts[GBLOWN_ECTO_NUM];}
	virtual	void			BlowSpecialPart(edict_t &monster, gz_blown_part *part, edict_t *dangler){}
	virtual	void			ShakeSpecialPart(edict_t &monster, gz_blown_part *part, Vect3 &Direction){}
			void			UseGround2(edict_t &monster, qboolean toggleVal);

			void			BlowPart(edict_t &monster, gz_blown_part *part, edict_t *dangler);
			void			ShakePart(edict_t &monster, gz_blown_part *part, Vect3 &Direction);
			void			BlowZone(edict_t &monster, gz_code part, gz_code originalPart, edict_t *dangler);

			void			SetGoreZone(edict_t &monster, char *zonename, gz_code slot);
			void			SetGoreCap(edict_t &monster, char *capname, gz_code slot);
			void			SetGoreBlownCap(edict_t &monster, char *capname, gz_code slot);
			void			SetGoreBlownCapBolt(edict_t &monster, char *capname, gz_code slot);
			void			SetGoreCapBolt(edict_t &monster, char *capname, gz_code slot);
			void			SetGoreCapPiece(edict_t &monster, char *capname, char *boltname, gz_code slot);
			void			SetGoreBolt(edict_t &monster, char *boltname, gz_code slot);

			edict_t			*MakeBlownPart(edict_t &monster, gz_code part, vec3_t dir, ggOinstC **blownInst, bool includeEffects = true);
			void			DoLimbRotate(edict_t *monster );

	//NEED to call this before RecognizeGoreZones, AND not after!			
	virtual	void			AllocateGoreZones(edict_t &monster);
	virtual	void			RecognizeGoreZones(edict_t &monster){}

	virtual	void			PrecacheGore(edict_t &monster){}

	virtual bool			FreeKnife(edict_t &monster, ggBinstC* bolted, GhoulID boltKnife) { return false; }
	virtual int				HandleNearMiss(edict_t &monster, edict_t *knife){ return -1; }
public:
	bodyorganic_c();
	virtual					~bodyorganic_c();

	virtual	qboolean		VerifySequence(edict_t &monster, mmove_t *newanim, int reject_actionflags);
	virtual	void			Drip(edict_t &monster);
	virtual int				GetClassCode(void){return BODY_ORGANIC;}//this should be different for everybody in this class family

			void			WriteGZ(void);
			void			ReadGZ(void);
							bodyorganic_c(bodyorganic_c *orig);
	virtual void			Evaluate(bodyorganic_c *orig);
	virtual void			FreeKnives(edict_t &monster) { return;}
	virtual void			DropWeapon(edict_t *ent, IGhoulInst *me) { return; }
	virtual	void			NextShotsGonnaKillMe(int bKill) {return;} 
	virtual	bool			IsNextShotGonnaKillMe() {return false;} 
	virtual void			CanDropBoltons(bool bDrop) { return; }
	virtual	void			SetArmor(edict_t &monster, armor_code newarmor) { return; }
	virtual	void			UpdateSoundsByWeapon(edict_t &monster, ai_c &which_ai) {}
};

class bodyhuman_c: public bodyorganic_c
{
protected:
			int				curSoundIndex;
			float			curVol;
			float			curAtten;
			char			voiceDirGeneral[64];
			char			voiceDirDeath[64];//really only for players--could be moved to client struct?
			int				voiceDirNumber;
			char			face_base[64];
			char			curface[64];
			float			blink_time;
			float			breathe_time;
			float			rLastFireTime;
			float			lLastFireTime;
			float			nextGreetTime;
			int				numGreets;
			int				numWakes;
			int				curGreet;
			qboolean		touchSound;
			qboolean		rHandOpen;
			qboolean		lHandOpen;
			qboolean		grenadeInHand;
			int				bodyDamage;//bitfield for restricting animations
			int				bodyDamageAccumulated;//bitfield for affecting movement and aim in realistic dm
			float			bodyDamageEndRecent;
			qboolean		FinalAnim; // When set, the current anim or series of anims is intended to be the last ones played.  also has sound implications
			qboolean		FinalSound;
			qboolean		rWeaponShot;
			qboolean		lWeaponShot;
			int				rWeaponAmmo;
			qboolean		freezeMyBolts;
			qboolean		gutsOut;
			qboolean		justFiredRight;//used for cocking shotguns and rifles
			attacks_e		lHandAtk;
			attacks_e		rHandAtk;
			deathvoice_code	deathVoiceType;
			armor_code		armorCode;		// how much armor this joker starts off with
			int				nNextShotKillsMe;	// used for scripted kills
			bool			bDropBoltons;
			bool			bBoxConfirmed; // whether I've made a last-ditch attempt at a normal post-mortem bbox
			bool			bVocalCordsWorking;

	virtual	qboolean		PlayAnimation(edict_t &monster, mmove_t *newanim, bool forceRestart);

	virtual	void			GetLevelDeathSoundDir(char *putSuffixHere, deathvoice_code death);

							//play sound from my specific voice directory (e.g. "enemy/skinhead2")
			void			VoiceGreetSound(edict_t &monster, float vol = .6);
							//play sound from my general voice directory (e.g. "enemy/skinhead")
			void			VoiceGeneralSound(char *base, edict_t &monster, int num, float vol = 0.8);

							//cache sound for my specific voice directory (e.g. "enemy/skinhead2")
			void			VoiceSoundCache(char *base, edict_t &monster, int num);
			void			VoiceWakeSoundCache(edict_t &monster);
			void			VoiceGreetSoundCache(edict_t &monster);
							//cache sound for my general voice directory (e.g. "enemy/skinhead")
			void			VoiceGeneralSoundCache(char *base, edict_t &monster, int num);

							//set my specific and general directories
	virtual	void			SetVoiceDirectories(edict_t &monster, const char *base, int possible_varieties, deathvoice_code death, char *deathdir=NULL);

	virtual	void			RightStep(edict_t &monster);
	virtual	void			LeftStep(edict_t &monster);


	virtual	void			OpenRightHand(edict_t &monster, qboolean toggleVal);
			void			OpenLeftHand(edict_t &monster, qboolean toggleVal);
			void			GrenadeHand(edict_t &monster, qboolean toggleVal, qboolean nodrop);
			
	virtual	void			SetGoreZoneDamageScale(edict_t &monster);

	virtual int				TestDamage(void){return bodyDamage & BODYDAMAGE_ANY;}

	virtual	void			RecognizeGoreZones(edict_t &monster){}

			qboolean		ShowFrontHoleDamage (edict_t &monster, gz_code frontCode, gz_code backCode, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int take, int knockback, int dflags, int mod);
			void			ShowBackHoleDamage (edict_t &monster, gz_code frontCode, gz_code backCode, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int take, int knockback, int dflags, int mod);

	virtual	attacks_e		GetWeaponAtBolt(edict_t &monster, char *boltname);

	//casual (non-pain, non-death) GetSequence funcs should call this before getting wise on their own--
	//it should get seqs for blindness, limping/pulling in guts, etc.
	virtual	mmove_t			*GetSequenceForDisability(edict_t &monster, int reject_actionflags=0);
	virtual	void			AllocateGoreZones(edict_t &monster);

	virtual	void			PrecacheGore(edict_t &monster);

	virtual	void			BlowSpecialPart(edict_t &monster, gz_blown_part *part, edict_t *dangler);
	virtual	void			ShakeSpecialPart(edict_t &monster, gz_blown_part *part, Vect3 &Direction);

	virtual bool			FreeKnife(edict_t &monster, ggBinstC* bolted, GhoulID boltKnife);
	virtual int				HandleNearMiss(edict_t &monster, edict_t *knife);
			int				ResolveKnifeImpact(edict_t &monster, edict_t *inflictor, edict_t *attacker, 
								int *nNearMissZone, vec3_t dir, vec3_t point, vec3_t origin, int *dflags);
			void			ChooseShotgunGoreZones(edict_t &monster, vec3_t origin, vec3_t point, gz_code *backCode,
								gz_code	*listOfFrontCodes, int *numOfFrontCodes);
			void			ChooseDirectionalGoreZones(edict_t &monster, vec3_t origin, gz_code *backCode,
								gz_code	*listOfFrontCodes, int *numOfFrontCodes);
			int				GetProjectileFrontHole(edict_t &monster, edict_t *attacker, vec3_t origin, vec3_t dir, vec3_t point,
								gz_code *backCode, int nNearMissZone, bool bKnife, ggOinstC *myInstance,
								ggBinstC *boltPiece, vec3_t ExitPos, Vect3 EntryPos, Matrix4 ZoneMatrix, Vect3 zonePos,
								Vect3 Direction, int *take, ggBinstC *oldLeftWeap, ggBinstC *oldRightWeap, int *numOfFrontCodes,
								gz_code *frontCode, bool *EntryPosIsValid, gz_code *listOfFrontCodes);




public:
	virtual	void			VoiceSound(char *base, edict_t &monster, int num, float vol = 0.8);
	virtual	void			VoiceWakeSound(edict_t &monster, float vol = 0.4);

	virtual void		FreeKnives(edict_t &monster);
	virtual	mmove_t		*GetSequenceForDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForPostMortem(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForCrouchDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForProneDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForPulseGunDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForBurningDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForJuggleDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForGoreZoneDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForRunningDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForFacingBasedDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForDeathFromFront(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, vec3_t to_dam, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForDeathFromBack(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, vec3_t to_dam, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForCatchAllDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForParentalLockDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForTrainDeath(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForC4Death(edict_t &monster, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForPain(edict_t &monster, vec3_t point, float kick, int damage, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual mmove_t		*GetSequenceForMovement(edict_t &monster, vec3_t dest, vec3_t face, vec3_t org, vec3_t ang,  actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual mmove_t		*GetSequenceForStand(edict_t &monster, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual mmove_t		*GetSequenceForJump(edict_t &monster, vec3_t dest, vec3_t face, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual mmove_t		*GetSequenceForAttack(edict_t &monster, vec3_t dest, vec3_t face, edict_t *target, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetSequenceForTransition(edict_t &monster, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, int reject_actionflags=0);
	virtual mmove_t		*GetSequenceForDodge(edict_t &monster, vec3_t dest, vec3_t face, edict_t *target, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int leftSide = 0, int reject_actionflags=0);
	virtual mmove_t		*GetSequenceForReload(edict_t &monster, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);
	virtual	mmove_t		*GetPostShootSequence(edict_t &monster, vec3_t dest=vec3_origin, vec3_t face=vec3_origin, actionsub_code seqtype=ACTSUB_NORMAL, bbox_preset goal_bbox=BBOX_PRESET_NUMBER, mmove_t *ideal_move=NULL, int reject_actionflags=0);

	virtual	mmove_t		*GetSequenceForRunAimStrafe(edict_t &monster, vec3_t to_dest_dir, vec3_t forward, vec3_t right, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags);
	virtual	mmove_t		*GetSequenceForWalkAimStrafe(edict_t &monster, vec3_t to_dest_dir, vec3_t forward, vec3_t right, vec3_t dest, vec3_t face, actionsub_code seqtype, bbox_preset goal_bbox, mmove_t *ideal_move, int reject_actionflags);

	virtual bool		IsInStationaryDeathAnim();

	virtual void		SetRootBolt(edict_t &monster);
	virtual	void		SetArmor(edict_t &monster, armor_code newarmor);
	virtual armor_code	GetArmorCode();
	virtual float		GetArmorPercentRemaining();
	virtual void		StripArmor();

	virtual	void		MakeRetreatNoise(edict_t &monster);

	virtual	void		NextShotsGonnaKillMe(int nKill) {nNextShotKillsMe = nKill;} 
	virtual	bool		IsNextShotGonnaKillMe() {return !!nNextShotKillsMe;} 
	virtual void		CanDropBoltons(bool bDrop) { bDropBoltons = bDrop; }

	virtual void			UseMonster(edict_t &monster, edict_t *user);
	virtual void			TouchMonster(edict_t &monster, edict_t *user);

	virtual	int			ShowDamage (edict_t &monster, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb);

	//forceFireRate will not fire the gun if it's been fired more recently than design doc says that kind of gun should fire.
	virtual	void				FireRightHandWeapon(edict_t &monster, bool isLethal = false, int NullTarget = 0, qboolean forceFireRate = false);
	virtual	qboolean			RightHandWeaponReady(edict_t &monster);
	virtual void				ThrowProjectile(edict_t &monster);

	virtual	attacks_e			GetRightHandWeapon(edict_t &monster);
	virtual	attacks_e			GetLeftHandWeapon(edict_t &monster);
	virtual	attacks_e			GetBestWeapon(edict_t &monster);
	virtual	int					UseLeftHandAttack(edict_t &monster, edict_t *target);

	virtual	void				SetRightHandWeapon(edict_t &monster, attacks_e newRightWeap);
	virtual	void				SetLeftHandWeapon(edict_t &monster, attacks_e newLeftWeap);

	virtual	void				SetFlags(edict_t & monster, int newflags);

//	virtual	void				Emote(edict_t &monster, emotion_index new_emotion, float emotion_duration, qboolean scripted_emoting=false);
	virtual	emotion_index		GetMood(edict_t &monster);
	virtual	void				UpdateFace(edict_t &monster);
	virtual	void				UpdateSoundsByWeapon(edict_t &monster, ai_c &which_ai);
	virtual void				SetFace(edict_t &monster, char *face);
	virtual	int					DropBoltOn(edict_t &monster, ggBinstC *boltedptr, Vect3 &pos, Vect3 &Direction, int damage, edict_t *attacker = NULL);
	virtual void				DropWeapon(edict_t *ent, IGhoulInst *me);
	virtual	int					SpecialActionBoltOn(edict_t &monster, ggBinstC *boltedptr, Vect3 &pos, Vect3 &Direction, int damage);

	virtual	qboolean			MustReloadRWeapon(edict_t &monster){return (rWeaponAmmo<=0);}
	virtual	void				ReloadRWeapon(edict_t &monster);
	virtual	void				AdjustAimByWeapon(edict_t &monster, vec3_t aimDir);
	virtual	void				Gib(edict_t &monster, vec3_t dir, int dflags);
	virtual	void				Gib2(edict_t &monster, vec3_t dir, int dflags);

	virtual void				SetVocalCordsWorking(bool bCords) { bVocalCordsWorking = bCords; }
	virtual bool				AreVocalCordsWorking() { return bVocalCordsWorking; }
	bodyhuman_c();

	virtual int			GetClassCode(void){return BODY_HUMAN;}//this should be different for everybody in this class family

						bodyhuman_c(bodyhuman_c *orig);
	virtual void		Evaluate(bodyhuman_c *orig);

	virtual int			IsMovementImpeded(void);
	virtual int			IsAimImpeded(void);
	virtual void		RemoveLimbDamage(void);
	virtual void		AssignFrontAndBackGZones(gz_info **frontHole, gz_code *frontCode, gz_info **backHole, gz_code *backCode);
			void		SeverRoutine(edict_t &monster, vec3_t dir, gz_code frontCode, gz_code backCode, gz_info *frontHole, gz_info *backHole, bool forceSever = false);
	virtual void		HealGoreZones(edict_t &monster, int damagedHealth, int healedHealth);	

};

class bodyecto_c : public bodyhuman_c
{
protected:
	virtual	void			RecognizeGoreZones(edict_t &monster);
	virtual	gz_blown_part	*GetBlownPartForIndex(blown_index theIndex);
	virtual	void			OpenRightHand(edict_t &monster, qboolean toggleVal){}
	virtual	void			OpenLeftHand(edict_t &monster, qboolean toggleVal){}
	virtual	void			GrenadeHand(edict_t &monster, qboolean toggleVal, qboolean nodrop){}
public:
						bodyecto_c();
	virtual int			GetClassCode(void){return BODY_ECTO;}//this should be different for everybody in this class family

						bodyecto_c(bodyecto_c *orig);
	virtual void		Evaluate(bodyecto_c *orig);
	virtual void		Write();
	virtual void		Read();
};

class bodymeso_c : public bodyhuman_c
{
protected:
	virtual	void			RecognizeGoreZones(edict_t &monster);
	virtual	gz_blown_part	*GetBlownPartForIndex(blown_index theIndex);
public:
						bodymeso_c();
	virtual int			GetClassCode(void){return BODY_MESO;}//this should be different for everybody in this class family

						bodymeso_c(bodymeso_c *orig);
	virtual void		Evaluate(bodymeso_c *orig);
	virtual void		Write();
	virtual void		Read();
};

class bodyfemale_c : public bodyhuman_c
{
protected:
	virtual	void			RecognizeGoreZones(edict_t &monster);
	virtual	gz_blown_part	*GetBlownPartForIndex(blown_index theIndex);
public:
						bodyfemale_c();
	virtual int			GetClassCode(void){return BODY_FEMALE;}//this should be different for everybody in this class family

						bodyfemale_c(bodyfemale_c *orig);
	virtual void		Evaluate(bodyfemale_c *orig);
	virtual void		Write();
	virtual void		Read();
};

