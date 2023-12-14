// g_obj.h -- genric object information

// included for entSoundsToCache[], an array for entities to mark which client sfx they need cached
#include "g_spawn.h"

#ifndef _G_OBJ_H_
#define _G_OBJ_H_

#define SMSEQ_HOLD			0
#define SMSEQ_LOOP			1
#define SMSEQ_HOLDFRAME		2


typedef struct objParts_s
{
	char *partnull;					// Null on parent the part is bolted to
	int  partnum;					// Which field on ent struct to use
	void	(*pain)(edict_t *ent,int partlost, vec3_t boltPos);	// Update function when part is destroyed
} objParts_t;


typedef struct objBreak_s
{
	int partCnt;	// Number of parts on object
	objParts_t	*boltPartData;		// Pointer to first of bolted on parts array data
} objBreak_t;

typedef struct modelSpawnData_s
{
	char		*dir;
	char		*file;
	int			surfaceType;
	int			material;
	int			health;
	solid_t		solid;
	char		*materialfile;
	int			debrisCnt;			// Amount of debris to throw
	float		debrisScale;		// Size of debris to throw
	objBreak_t	*objBreakData;		// Pointer to objBreak array member
} modelSpawnData_t;

// achtung! every entry in this list (except OIT_HEADER and OIT_MAX) has to be accounted for
//in FreeEdictUserData()
typedef enum
{
	OIT_HEADER = 0,
	OIT_BOLTINST,
	OIT_FLIP,
	OIT_MAX					// keep this at the end of the enum list, pleez
} objInfoType_e;

#define MAX_BOLTINSTINFO	6

// objInfo_c is the base class for any class referred to
//in the objInfoType_e list
class objInfo_c
{
public:
	int					nInfoType;					
	edict_t				*head;
public:
	objInfo_c() { nInfoType = OIT_HEADER; head = NULL; }
	objInfo_c(objInfo_c *orig);
	objInfo_c(edict_t* ent) { nInfoType = OIT_HEADER; head = NULL; }
	virtual ~objInfo_c() { head = NULL; }

			void		Evaluate(objInfo_c *orig);
	virtual void		Destroy();
};

class boltInstInfo_c : public objInfo_c
{
protected:
	int					onOff;						// heckuva name, huh? one bit per bolton.
	int					nNum;						// number of boltons attached to the edict
	static int			nMax;						// right now it's 6
public:
	boltInstInfo_c() { head = NULL; Reset(); }
	boltInstInfo_c(boltInstInfo_c *orig);
	boltInstInfo_c(edict_t *ent):objInfo_c(ent) { nInfoType = OIT_BOLTINST; onOff = 0; nNum = 0; nMax = MAX_BOLTINSTINFO;}
	boltInstInfo_c(int n,edict_t *ent):objInfo_c(ent) { nInfoType = OIT_BOLTINST; onOff = 0; nNum = n; nMax = MAX_BOLTINSTINFO; for(int i = 0; i < n; i++) onOff |= (1<<i); }
	~boltInstInfo_c();
	virtual void		Destroy();
	// for unfortunate reasons, indices here are 1-indexed
	bool	AddBolt(bool bOn = true);
	int		GetNum(void) { return nNum; }
	void	TurnOn(int n) { if (n <= nMax && n >= 1) onOff |= (1<<(n-1)); }
	void	TurnOff(int n) { if (n <= nMax && n >= 1) onOff &= ~(1<<(n-1)); }
	bool	IsOn(int n) { if (n <= nMax && n >= 1) return (onOff & (1<<(n-1))) != 0; return false;}
	void	Reset();
	void	Evaluate(boltInstInfo_c *orig);
};

class objFlipInfo_c : public objInfo_c
{
public:
	enum flipDir_e
	{
		FLIPDIR_UNK = 0, // not set yet, I guess
		FLIPDIR_F,
		FLIPDIR_B,
		FLIPDIR_L,
		FLIPDIR_R
	};
	flipDir_e			dir;
	float				startTime;
	float				ground;
	float				radius;
	float				initRadiusAng;
	vec3_t				preFlipAngles;
	vec3_t				flipOrigin;
	vec3_t				fwd;
	void				(*lastThinkFn)(edict_t *ent);
public:
	objFlipInfo_c() { head = NULL; Reset();}
	objFlipInfo_c(objFlipInfo_c *orig);
	objFlipInfo_c(edict_t *ent);
	virtual void		Destroy();
	bool				Update();
	void				Reset();
	void				Evaluate(objFlipInfo_c *orig);
};

// an instance of this class is pointed to by edict_t::objInfo
class baseObjInfo_c
{
protected:
	std::vector<gg_binst_c_ptr>	bolts;
#define BOI_SAVE_START		offsetof(baseObjInfo_c, owner)
	edict_t					*owner;
	boltInstInfo_c			boltInstInfo;
	objFlipInfo_c			flipInfo;
public:
	baseObjInfo_c(edict_t *ent) {owner = ent; boltInstInfo.head = NULL; flipInfo.head = NULL;}
	baseObjInfo_c(baseObjInfo_c *orig);
	~baseObjInfo_c();

	IGhoulInst*				GetBolt(int nBolt);
	objInfo_c*				GetInfo(int nType);
	void					ResetInfo(int nType);
	bool					AddBolt(ggBinstC *newBolt);
	void					Read(void);
	void					Write(void);
	void					Evaluate(baseObjInfo_c *orig);
};

class boltonOrientation_c
{
public:
	enum retCode_e
	{
		ret_TRUE = 1,
		ret_FALSE,
		ret_ERROR
	};
	edict_t			*root;			// the ultimate root of the bolton (parent, grandparent, whatever)
	IGhoulInst		*boltonInst;	// instance of the bolted-on object that's getting oriented
	GhoulID			boltonID;		// id of the bolted-on object
	IGhoulInst		*parentInst;	// instance of the bolted-on object's immediate parent (may be the root)
	GhoulID			parentID;		// id of the bolted-on object's immediate parent (only if not the root)
	vec3_t			vTarget;		// where you'd like the bolton to be pointing
	float			fMinPitch;		// limits the bolton's pitch w.r.t. its immediate parent
	float			fMaxPitch;		// limits the bolton's pitch w.r.t. its immediate parent
	float			fMinYaw;		// limits the bolton's yaw w.r.t. its immediate parent
	float			fMaxYaw;		// limits the bolton's yaw w.r.t. its immediate parent
	float			fMaxTurnSpeed;	// how fast the bolton can turn (max rotational speed, really)
	float			fRetPitch;		// the post-rotation pitch of the rotated bolt (wrt the parent)
	float			fRetYaw;		// the post-rotation yaw of the rotated bolt (wrt the parent)
	bool			bUsePitch;		// rotate bolton vertically
	bool			bUseYaw;		// rotate bolton horizontally
	bool			bToRoot;		// true		== get bolt matrix wrt to root 
									// false	== get bolt matrix wrt to parent 
public:
	boltonOrientation_c();
	boltonOrientation_c::retCode_e				OrientBolton();
	boltonOrientation_c::retCode_e				OrientEnt();
protected:
	void			GetAngles(Vect3 &pos, Matrix4 &mat, float &angle1, float &angle2);
};

void		LightBecomeDebris (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void		ObjBecomeDebris (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void		SpecialBecomeDebris (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);

void		SimpleModelInit2(edict_t *ent, modelSpawnData_t *modelData,char *skinname,char *partname);

void		SimpleModelSetObject(edict_t *ent,char *partname);
void		SimpleModelSetSequence(edict_t *ent,char *seqname,int loopflag);
GhoulID		SimpleModelSetSequence2(IGhoulInst* inst,char *seqname,int loopFlag);
void		SimpleModelScale(edict_t *ent,float objscale);
void		SimpleModelRemoveObject(edict_t *ent,char *partname);
void		SimpleModelRemoveObject2(IGhoulInst* inst,char *partname);
void		RecursiveTurnOff(IGhoulInst* inst);
void		SimpleModelTurnOnOff(IGhoulInst* inst, bool bOnOff);
ggBinstC*	SimpleModelAddBolt(edict_t *ent,modelSpawnData_t &bolterModelData,char*bolterBoltName,
						modelSpawnData_t &bolteeModelData,char *bolteeBoltName,char *skinName);
ggBinstC	*ComplexModelAddBolt(ggBinstC* bInst,modelSpawnData_t &bolterModelData,char*bolterBoltName,
						modelSpawnData_t &bolteeModelData,char *bolteeBoltName,char *skinName);
ggBinstC*	SimpleModelFindBolt(edict_t *ent, modelSpawnData_t bolterModelData, char*bolterBoltName);
IGhoulInst*	SimpleModelGetBolt(edict_t *ent, int nBolt);
void		SimpleModelAddObject(edict_t *ent,char *partname);
void		SimpleModelAddObject2(IGhoulInst* inst,char *partname);

void		SetSkin(edict_t *self, char* modelName, char* modelSubname, char *matName, char *skinName, int nSetBoltonSkins);
void		SetSkin2(IGhoulInst* inst, char* modelName, char* modelSubname, char *matName, char *skinName);

void		GrabStuffOnTop (edict_t *self);

void		breakable_brush_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);

void		Obj_painflip (edict_t *ent,edict_t *other,int damage);
void		Obj_tipover (edict_t *ent, edict_t *other, int damage);

void		Obj_painroll (edict_t *ent,edict_t *other,int damage,int axis);
void		Obj_partpain (edict_t *self, edict_t *other, float kick, int damage, vec3_t wherehit);

void		Obj_explode(edict_t *ent,vec3_t *pos,int radiusBurn,int blindingLight);
void		Obj_partbreaksetup (int bpd_enum,modelSpawnData_t *modelData,
						 objParts_t *objBoltPartsData,objBreak_t *objBreak);
void		Obj_partkill (edict_t *self,int boltNum,int debrisFlag);

void		FlipObject(edict_t *ent,edict_t *other, edict_t *activator); // intended to be used as a plUse fn

#endif //_G_OBJ_H