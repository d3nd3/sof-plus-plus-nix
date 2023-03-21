//g_ghoul
//manager for ghoul animations: alloc, dealloc and dole out to edicts that need seqs
#ifndef _G_GHOUL_H
#define _G_GHOUL_H
#include "string.h"

#define GGHOUL_OBJ_NAME_LEN 100
#define GGHOUL_OBJSUB_NAME_LEN 100
#define GGHOUL_OBJSUBSUB_NAME_LEN 100

//needed for the quickening of verifysequence - hopefully this should be more than enough, though
#define MAXIMUM_NUM_SEQUENCES	1024


//yech. fixme--find a good home for this stuff.
typedef struct
{
	int		flags;
	float	dist;
	int		yaw;
} mframe_t;

typedef enum
{
BBOX_PRESET_STAND,
BBOX_PRESET_CROUCH,
BBOX_PRESET_PRONE,
BBOX_PRESET_ALLFOURS,
BBOX_PRESET_SIT,
BBOX_PRESET_LAYFRONT,
BBOX_PRESET_LAYBACK,
BBOX_PRESET_LAYSIDE,
BBOX_PRESET_NONE,
BBOX_PRESET_NUMBER
} bbox_preset;

typedef enum
{
	ACTCODE_STAND,
	ACTCODE_MOVE,
	ACTCODE_ATTACK,
	ACTCODE_JUMP,
	ACTCODE_FALL,
	ACTCODE_PAIN,
	ACTCODE_DEATH,
	ACTCODE_DODGE,
	ACTCODE_SCRIPTRELEASE
} action_code;

typedef enum
{
	BODYPOS_UNKNOWN,//standard body positioning, or just plain unknown
	BODYPOS_AIMWALK,//walking forward, back, or strafing while aiming a gun
	BODYPOS_BLINDED,//help! i'm blind!
	BODYPOS_IDLE,	//just standing around, thinkin everything's ok
	BODYPOS_RLEGHURT,	//right leg injured, stick with that sort of anim
	BODYPOS_LLEGHURT,	//left leg injured, stick with that sort of anim
	BODYPOS_WALKTURN,
	BODYPOS_PLAYERLEANRIGHT,
	BODYPOS_PLAYERLEANLEFT,
	BODYPOS_TALKING,
} bodypos_t;

typedef struct mmove_s
{
	//ghoul monsters
	char		ghoulSeqName[GGHOUL_ID_NAME_LEN];

	action_code	suggested_action;
	//ghoul and reglar
	float		velScale;
	float		maxTurn;
	float		minTurn;
	int			actionFlags;//denotes what kind of action this anim can be associated with, provides easy way to let action know what state i end up in
	bbox_preset	bbox;
	float		playPos;	// where the animation starts playing. (0...1), usually 0.
	bodypos_t	bodyPosition;	//extra classification for the sequence so i can make a guess to what it'll transition well with
} mmove_t;
#include "pt_listpointer.h"

class ggOinstC;
class ggObjC;

class ggBinstC
{
private:
	List<gg_binst_c_ptr>	bolts;
#define GGBINST_SAVE_START	offsetof(ggBinstC, bBoltedToOinst)
	// kef
	bool					bBoltedToOinst;	// am I bolted to a ggOinstC (as opposed to a ggBinstC)?
	gg_binst_c_ptr			bolter2;		// the ggBinstC to which I'm bolted (NULL if bBoltedToOinst == true)

	//this is for keeping track of who i'm bolted to
	gg_inst_c_ptr			bolter;
	GhoulID					bolter_bolt;

	//this is the important stuff--i'm responsible for this.
	IGhoulInst				*instance;
	GhoulID					bolt;
	GhoulID					sequence;
	gg_obj_c_ptr			object;

	float					scale;

public:
	ggBinstC(ggOinstC *papa, GhoulID papabolt, ggObjC *babyObj, GhoulID babyBolt, char *newskin=NULL, float newscale = 1.0);
	ggBinstC(ggBinstC *papa, GhoulID papabolt, ggObjC *babyObj, GhoulID babyBolt, char *newskin=NULL, float newscale = 1.0);
	ggBinstC(void){instance=NULL; bBoltedToOinst = true;}
	~ggBinstC(void);

	virtual int		GetClassCode(void){return 0;}//this should be different for everybody in this class family
static	ggBinstC *NewClassForCode(int code){return new ggBinstC;}

	IGhoulInst		*GetInstPtr(void){return instance;}
	ggObjC		*GetBolteeObject(void){return object;}
	ggOinstC *GetBolter(void){if (bBoltedToOinst) return bolter; else return NULL;}
	GhoulID	GetSequence(void){return sequence;}
	GhoulID	GetBolt(void){return bolt;}
	float	GetScale(void){return scale;}

	void			ChangeOwnerTo(edict_t *newowner);

	GhoulID	GetBolterBolt(void){return bolter_bolt;}
	void	PlaySequence(GhoulID Seq,float Now,float PlayPos=0.0f,bool Restart=true,
							IGhoulInst::EndCondition ec=IGhoulInst::Loop, bool MatchCurrentPos=false,
							bool reverseAnim=false);

	// kef
	ggBinstC		*GetBolter2(void){if (bBoltedToOinst) return NULL; else return bolter2;}
	edict_t			*GetEdict(void);
	bool			IsBoltedToOinst(void) { return bBoltedToOinst; }
	ggBinstC		*AddBoltInstance(GhoulID mybolt, ggObjC *babyObj, GhoulID babybolt, char *boltskin=NULL, float newscale = 1.0);
	ggBinstC		*GetBoltInstance(GhoulID mybolt);
	void			StopAnimatingAtBolt(GhoulID mybolt);
	void			StopAnimatingAtBolt(char *mybolt);
	void			StopAnimatingAtAllBolts(void);
	ggBinstC		*FindBoltInstance(IGhoulInst *boltInst);
	void			RemoveBolt(GhoulID bolted_to);
	void			RemoveBoltInstance(ggBinstC *boltee);

					ggBinstC(ggBinstC *orig);
			void	Evaluate(ggBinstC *orig);
	virtual void	Write();
	virtual void	Read();
			void	AddBInst(ggBinstC *ggbinst);
};

class ggOinstC
{
private:
	List<gg_binst_c_ptr>	bolts;
#define GGOINSTC_SAVE_START		offsetof(ggOinstC, TheInst)
	IGhoulInst		*TheInst;
	edict_t			*my_edict;
	gg_obj_c_ptr	my_obj;
	GhoulID			sequence;
public:
	ggOinstC(ggObjC *this_object, edict_t *whose, IGhoulInst *clonedInst);
	ggOinstC(ggObjC *this_object, edict_t *whose);
	ggOinstC(void){my_edict = NULL; TheInst = NULL;}
	~ggOinstC(void);
	ggBinstC		*AddBoltInstance(GhoulID mybolt, ggObjC *babyObj, GhoulID babybolt, char *boltskin=NULL, float scale = 1.0);
	ggBinstC		*GetBoltInstance(GhoulID mybolt);
	void			AddBInst(ggBinstC *ggbinst);

	void			StopAnimatingAtBolt(GhoulID mybolt);
	void			StopAnimatingAtBolt(char *mybolt);
	void			StopAnimatingAtAllBolts(void);

	void			ChangeOwnerTo(edict_t *newowner);

	void			CopyBoltsTo(ggOinstC *recipient);

	ggBinstC		*FindBoltInstance(IGhoulInst *boltInst);

	void			RemoveBolt(GhoulID bolted_to);
	void			RemoveAllBolts(void);
	void			RemoveBoltInstance(ggBinstC *boltee);

	IGhoulInst		*GetInstPtr(void){return TheInst;}
	edict_t			*GetInstEdict(void){return my_edict;}
	ggObjC			*GetParentObject(void){return my_obj;}
	void			PlaySequence(GhoulID Seq,float Now,float PlayPos=0.0f,bool Restart=true,
							IGhoulInst::EndCondition ec=IGhoulInst::Loop, bool MatchCurrentPos=false,
							bool reverseAnim=false);

	void			SetEdictInst(void);

	void			ClearMyEdict(void){my_edict = NULL;}

	virtual int		GetClassCode(void){return 0;}//this should be different for everybody in this class family
static	ggOinstC		*NewClassForCode(int code){return new ggOinstC;}

					ggOinstC(ggOinstC *orig);
			void	Evaluate(ggOinstC *orig);
	virtual void	Write();
	virtual void	Read();
};

/////////////////////////////////////////////////////////
//ghoul object
/////////////////////////////////////////////////////////
class ggObjC
{
private:
	map<IGhoulInst *,gg_inst_c_ptr>		instances;
	list<string>						registered_skins;
#define GGOBJC_SAVE_START	offsetof(ggObjC, this_object)
	IGhoulObj	*this_object;
	bool		registration_locked;
	bool		register_all_skins;

	char		availableSequences[MAXIMUM_NUM_SEQUENCES];//don't feel like making this dynamic

	char name[GGHOUL_OBJ_NAME_LEN];
	char subname[GGHOUL_OBJSUB_NAME_LEN];

	//for simple models: this will be the only skin to register!
	char skinname[GGHOUL_OBJSUBSUB_NAME_LEN];
	char basefile[GGHOUL_OBJSUB_NAME_LEN];

public:
	ggObjC(void);
	ggObjC(const char* newname, const char* newsubname, bool allSkins, const char* newskinname, const char* newbasefile);
	~ggObjC(void);

	virtual int		GetClassCode(void){return 0;}//this should be different for everybody in this class family
static	ggObjC *NewClassForCode(int code){return new ggObjC;}

//returns the ghoul object
	IGhoulObj *GetMyObject(void){return this_object;}

//get the name associated with this object--this will be the directory that sequences are looked for in
	char	*GetName(void){return name;}
	char	*GetSubName(void){return subname;}
	char	*GetSkinName(void){return skinname;}
	char	*GetBaseFile(void){return basefile;}

	//set all edict insts so they're right.
	void			SetEdictInsts(void);

	bool			IsRegistrationLocked(void){return registration_locked;}

	GhoulID	FindSequence(const char* seqname);
	GhoulID	FindMaterialForSkin(char *findskinname);
	ggOinstC	*FindOInst(IGhoulInst *findme);


	GhoulID			RegisterPart(const char* partname){return this_object->RegisterPart(partname);}
	GhoulID			RegisterSequence(const char* seqname){return this_object->RegisterSequence(seqname);}
	GhoulID			RegisterMaterial(const char* matname){return this_object->RegisterMaterial(matname);}
	GhoulID			RegisterSkin(const char* matname, const char *regskinname);
	GhoulID			RegisterSkin(const char *regskinname);

	void			RegistrationLock(void);

	ggOinstC		*AddInstance(edict_t *whose);
	ggOinstC		*CloneInstance(edict_t *whose, IGhoulInst *clonedInst);
	void			AddOInst(ggOinstC *ggoinst, IGhoulInst *ghlinst);
	void			RemoveInstances(void);
	void			RemoveInstances(edict_t *who);
	void			RemoveInstance(ggOinstC *inst);

	void			SetSequenceAvailable(int seq, int val){availableSequences[seq] = val;}
	int				IsSequenceAvailable(int seq){return availableSequences[seq];}
	void			SetAllSequences(int val){memset(availableSequences, val, MAXIMUM_NUM_SEQUENCES);}

					ggObjC(ggObjC *orig);
		void		Evaluate(ggObjC *orig);
virtual void		Write();
virtual void		Read();
};

class gameghoul_c
{
private:
	map<IGhoulObj*,gg_obj_c_ptr>		objects;
	ggObjC *AddObject(const char* name, const char* subname, bool allSkins, const char *skinname, const char *basefile);
	
public:
	gameghoul_c(void){}
	~gameghoul_c(void);

	//load all the objects that may be seen on current level
	void LevelPrecache(void);
	//dealloc objects?
	void LevelCleanUp(void);

//returns the ghoul object designated by name--allocate if necessary
	ggObjC		*FindObject(IGhoulObj *curObject);

				//set dontMakeNew to true if you only want an object that's already totally set up--to avoid caching stuff in the middle of a level
	ggObjC		*FindObject(const char* name, const char* subname, bool allSkins=true, const char *skinname=NULL, const char *basefile=NULL, bool dontMakeNew=false);
	void		AddObject(IGhoulObj *ghlobj, ggObjC *ggobjc);

	ggOinstC	*FindOInst(IGhoulInst *findme);

//get ghoul id for seq, part or bolt attached to object
	GhoulID		FindObjectSequence(ggObjC *curObject, const char* seqname);
	
	ggOinstC	*AddObjectInstance(ggObjC *curObject, edict_t *whose);
	void		RemoveObjectInstances(edict_t *who);
	void		RemoveObjectInstances(ggOinstC *Oinst);

	ggBinstC		*AddBoltInstance(ggOinstC *bolter, GhoulID bolterBolt, ggObjC *boltee, GhoulID bolteeBolt, GhoulID bolteeSeq, char *boltskin=NULL, float scale = 1.0);
	ggBinstC		*AddBoltInstance(ggOinstC *bolter, GhoulID bolterBolt, ggObjC *boltee, GhoulID bolteeBolt, char *boltskin=NULL, float scale = 1.0);

	// kef
	void		AddBoltInstance(ggBinstC *bolter, GhoulID bolterBolt, ggObjC *boltee, GhoulID bolteeBolt, GhoulID bolteeSeq, char *boltskin=NULL, float scale = 1.0);
	void		AddBoltInstance(ggBinstC *bolter, GhoulID bolterBolt, ggObjC *boltee, GhoulID bolteeBolt, char *boltskin=NULL, float scale = 1.0);

	//this is to be used only for simple things that will have one sequence that can be expected to handle itself
	//if you pass in a skinname, it will be the ONLY skin loaded for the model!
	void		SetSimpleGhoulModel(edict_t *ent, const char* dirname, const char *seqname, const char *skinname=NULL, const char *matname=NULL);
};

extern gameghoul_c game_ghoul;

#endif _G_GHOUL_H