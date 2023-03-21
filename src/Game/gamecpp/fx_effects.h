#ifndef __FXEFFECTS_H_
#define __FXEFFECTS_H_

#include "../ghoul/ighoul.h"
#include "q_shared.h"
#include "q_sh_fx.h"
#include "game.h"

extern game_import_t gi;

// For use on FX_MakeDecalBelow
typedef enum
{
	FXDECAL_BLOODPOOL = 0,
	FXDECAL_SCORCHMARK,
	FXDECAL_OOZE,
	FXDECAL_PIPELEFT,
	FXDECAL_PUDDLE,
	FXDECAL_WHITESCORCH,
	FXDECAL_SINKPIPE,
	FXDECAL_MAX,
};

void WriteDirExp(vec3_t val);

void FX_Init(void);

void FX_SetEvent_Data(edict_t *ent, int eventType, int data);
void FX_SetEvent(edict_t *ent, int eventType);
void FX_ClearEvent(edict_t *ent);


void FXMSG_WriteRelativePos(edict_t *ent, vec3_t endpos, vec3_t normal, struct mtexinfo_s *surf);
void FXMSG_WriteRelativePosTR(trace_t &tr);
void FXMSG_WriteRelativePos_Dir(vec3_t pos, vec3_t dir, edict_t *ignore);
void FXMSG_WriteRelativePos_TestGround(vec3_t pos, edict_t *ignore);

void FX_MakeBarrelExplode(vec3_t pos, edict_t *source);
void FX_MakeBloodSpray(vec3_t pos, vec3_t splatDir, int vel, int amount, edict_t *source);
void FX_MakeDecalBelow(vec3_t pos, int type, unsigned int lLifeTime);
void FX_MakeSparks(vec3_t pos, vec3_t dir, int type);
void FX_MakeGumballs(vec3_t pos, vec3_t dir, int type);
void FX_MakeGlob(vec3_t pos, short color, int size);
void FX_MakeExtinguisherExplode(edict_t *ent, vec3_t dirVel);//, trace_t tr);
void FX_MakeDustPuff(vec3_t pos);
void FX_MakeElectricArc(vec3_t pos, int len, vec3_t dir);
void FX_MakeBulletWhiz(edict_t *ent, vec3_t offset, vec3_t dir, int power, int len);
void FX_MakeWallDamage(trace_t &tr, vec3_t dir, int size, int markType,int debris);
void FX_MakeSkyLightning(void);
void FX_MakeRocketExplosion(edict_t *ent, vec3_t point, int size, vec3_t normal, struct mtexinfo_s *surf);
void FX_MakeGrenadeExplosion(edict_t *ent, vec3_t point, int size);
void FX_ThrowDebris(vec3_t	origin,vec3_t norm, byte numchunks, byte scale, byte skin,byte x_max,byte y_max,byte z_max, byte surfType);
void FX_ThrowGlass(vec3_t origin, byte numchunks);
void FX_C4Explosion(edict_t *ent);
void FX_Flashpack(vec3_t pos);
void FX_NeuralPulse(vec3_t pos);
void FX_MakeAutoGunMuzzleFlash(edict_t *self);
void FX_StrikeWall(vec3_t spot, byte wallType);
void FX_BubbleTrail(vec3_t start, vec3_t end);
void FX_SmokeBurst(vec3_t pos, float size);
void FX_MakeLine(vec3_t start, vec3_t end, paletteRGBA_t color, int lifetime = 1, int mcastType = MULTICAST_ALL);
void FX_WallSparks(vec3_t pos, vec3_t dir, float size);
void FX_Bleed(edict_t *ent, GhoulID boltID, int amount);
void FX_BloodCloud(edict_t *ent, GhoulID boltID, int amount);
void FX_HeadExplosion(edict_t *ent, GhoulID boltID);
void FX_BloodJet(edict_t *ent, vec3_t spot, vec3_t jetDir);
void FX_HeliGun(edict_t *ent, IGhoulInst *heli, IGhoulInst *muzzle, GhoulID heliToMuzzleBolt, GhoulID muzEnd);
void FX_WhiteBurn(edict_t *ent);
void FX_EnemyShot(edict_t *ent, IGhoulInst *gun, byte weapID);
void FX_WhiteBlast(vec3_t org);
void FX_MinimiTrail(edict_t *ent);
void FX_Explosion(vec3_t pos, byte size);
void FX_VehicleExplosion(vec3_t pos, byte size);
void FX_SmokePuff(vec3_t pos,byte red,byte green,byte blue,byte alpha);
void FX_LittleExplosion(vec3_t pos, byte size, byte bSound);
void FX_MediumExplosion(vec3_t pos, byte size, byte bSound);
void FX_LargeExplosion(vec3_t pos, byte size, byte bSound);
void FX_MakeRing(vec3_t pos, short size);
void FX_PaperCloud(vec3_t vPos, byte nCount/*0 generates random num of papers*/);
void FX_SodaCans(vec3_t vPos, vec3_t vDir, byte nSpeed, byte nCount/*0 generates random num of cans*/);
void FX_MakeCoins(vec3_t pos, vec3_t dir, int type);
void FX_FloorSpace(edict_t *ent);
void FX_DropDustBits(vec3_t spot, int amount);
void FX_MakeRicochet(vec3_t vPos);
void FX_PhoneHit(vec3_t vPos);
void FX_TrashcanHit(vec3_t vPos);
void FX_HandleDMShotEffects(vec3_t start, vec3_t dir, vec3_t end, edict_t *shooter, int tracerfx, IGhoulInst *gun, int muzzlefx);
void FX_HandleDMShotHitEffects(vec3_t start, vec3_t dir, vec3_t end, edict_t *shooter, int tracerfx, IGhoulInst *gun, int muzzlefx);
void FX_HandleDMShotBigHitEffects(vec3_t start, vec3_t dir, vec3_t end, edict_t *shooter, int tracerfx, IGhoulInst *gun, int muzzlefx);
void FX_HandleDMMuzzleFlash(vec3_t start, vec3_t dir, edict_t *shooter, IGhoulInst *gun, int muzzlefx);

void FXA_SetRotate(edict_t *targ, vec3_t angVel, IGhoulInst *bolton, IGhoulInst *bolt2 = 0, IGhoulInst *bolt3 = 0, IGhoulInst *bolt4 = 0);
eft_rotate_t *FXA_GetRotate(edict_t *targ);
void FXA_RemoveRotate(edict_t *targ);

void FXA_SetFadeOut(edict_t *targ, float duration);
void FXA_SetFadeIn(edict_t *targ, float duration);
int	 FXA_CheckFade(edict_t *targ);		// Returns false if the object should be removed.



class CFXSender
{
private:
	int			sendFlags;
	int			flags;

	short		scale;
	byte		numElements;
	vec3_t		pos2;
	vec3_t		dir;
	vec3_t		min;
	vec3_t		max;
	short		lifeTime;
	byte		radius;

	void		clear(void);
	void		sendData(vec3_t source, multicast_t castType, edict_t *ignoreClient);
	int			findEffectID(int effectID, edict_t *ent, GhoulID bolt = 0);
	int			findEffectID(char *name, edict_t *ent, GhoulID bolt = 0)
					{findEffectID(gi.effectindex(name), ent, bolt);}

public:
	CFXSender(void){flags = 0;}
	// data transmission
	void		setScale(float val){assert(val > -256); assert(val < 256); scale = val * 128; flags |= EFF_SCALE;}
	void		setNumElements(float val){assert(val >= 0);assert(val < 256); numElements = val; flags |= EFF_NUMELEMS;}
	void		setPos2(vec3_t in){VectorCopy(in, pos2); flags |= EFF_POS2;}
	void		setDir(vec3_t in){VectorCopy(in, dir); flags |= EFF_DIR;}
	void		setMin(vec3_t in){VectorCopy(in, min); flags |= EFF_MIN;}
	void		setMax(vec3_t in){VectorCopy(in, max); flags |= EFF_MAX;}
	void		setLifetime(float val){assert(val > -256); assert(val < 256); lifeTime = val * 128; flags |= EFF_LIFETIME;}
	void		setRadius(float val){assert(val >= 0); assert(val < 256); radius = val; flags |= EFF_RADIUS;}
	void		setPos2AsWallEndPoint(void){sendFlags |= EFAT_POSTOWALL;}

	// burst effects
	void		exec(int effectID, vec3_t pos, multicast_t castType = MULTICAST_PHS, edict_t *ignoreClient = 0);
	void		exec(int effectID, edict_t *ent, multicast_t castType = MULTICAST_PHS, edict_t *ignoreClient = 0);
	void		exec(int effectID, edict_t *ent, GhoulID bolt, multicast_t castType = MULTICAST_PHS, int usesAltAxis = 0, edict_t *ignoreClient = 0);
	void		execWithInst(int effectID, edict_t *ent, IGhoulInst *inst, GhoulID bolt, multicast_t castType = MULTICAST_PHS, int usesAltAxis = 0, edict_t *ignoreClient = 0);
	void		execFromRandomBolt(int effectID, edict_t *ent, multicast_t castType = MULTICAST_PHS, int usesAltAxis = 0, edict_t *ignoreClient = 0);

	void		exec(char *name, vec3_t pos, multicast_t castType = MULTICAST_PHS, edict_t *ignoreClient = 0)
						{exec(gi.effectindex(name), pos, castType, ignoreClient);}
	void		exec(char *name, edict_t *ent, multicast_t castType = MULTICAST_PHS, edict_t *ignoreClient = 0)
						{exec(gi.effectindex(name), ent, castType, ignoreClient);}
	void		exec(char *name, edict_t *ent, GhoulID bolt, multicast_t castType = MULTICAST_PHS, int usesAltAxis = 0, edict_t *ignoreClient = 0)
						{exec(gi.effectindex(name), ent, bolt, castType, usesAltAxis, ignoreClient);}
	void		execWithInst(char *name, edict_t *ent, IGhoulInst *inst, GhoulID bolt, multicast_t castType = MULTICAST_PHS, int usesAltAxis = 0, edict_t *ignoreClient = 0)
						{execWithInst(gi.effectindex(name), ent, inst, bolt, castType, usesAltAxis, ignoreClient);}
	void		execFromRandomBolt(char *name, edict_t *ent, multicast_t castType = MULTICAST_PHS, int usesAltAxis = 0, edict_t *ignoreClient = 0)
						{execFromRandomBolt(gi.effectindex(name), ent, castType, usesAltAxis, ignoreClient);}
	// continual stuff
	void		execContinualEffect(int effectID, edict_t *ent, GhoulID bolt = 0, float size = 0);
	void		execContinualEffectFromRandomBolt(int effectID, edict_t *ent, float size = 0);
	void		stopContinualEffect(int effectID, edict_t *ent, GhoulID bolt = 0);
	void		editContinualEffect(int effectID, edict_t *ent, GhoulID bolt = 0, float newSize = 0);
	void		clearContinualEffects(edict_t *ent);
	float		getEffectSizeVal(int effectID, edict_t *ent, GhoulID bolt = 0);
	int			hasEffect(int effectID, edict_t *ent, GhoulID bolt = 0);

	void		execContinualEffect(char *name, edict_t *ent, GhoulID bolt = 0, float size = 0)
						{execContinualEffect(gi.effectindex(name), ent, bolt, size);}
	void		execContinualEffectFromRandomBolt(char *name, edict_t *ent, float size = 0)
						{execContinualEffectFromRandomBolt(gi.effectindex(name), ent, size);}
	void		stopContinualEffect(char *name, edict_t *ent, GhoulID bolt = 0)
						{stopContinualEffect(gi.effectindex(name), ent, bolt);}
	void		editContinualEffect(char *name, edict_t *ent, GhoulID bolt = 0, float newSize = 0)
						{editContinualEffect(gi.effectindex(name), ent, bolt, newSize);}
	float		getEffectSizeVal(char *name, edict_t *ent, GhoulID bolt = 0)
						{return getEffectSizeVal(gi.effectindex(name), ent, bolt);}
	int			hasEffect(char *name, edict_t *ent, GhoulID bolt = 0)
						{return hasEffect(gi.effectindex(name), ent, bolt);}

};

extern CFXSender fxRunner;

#endif