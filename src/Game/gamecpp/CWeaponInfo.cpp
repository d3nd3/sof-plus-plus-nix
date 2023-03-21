#include "g_local.h"
#include "CWeaponInfo.h"
#include "p_body.h"
#include "../strings/items.h"		// for the string ID's in pickupModelData
#include "callback.h"				// for respawn fadein

// determines whether or not we want to track pickup information for the next level
extern cvar_t	*pickupinfo;


void fireKnife(weaponFireInfo_t &wf);
void fireKnifeThrow(weaponFireInfo_t &wf);
void fireStarThrow(weaponFireInfo_t &wf);
void fireBullet(weaponFireInfo_t &wf);
void fireShotgun(weaponFireInfo_t &wf);
void fireSpas(weaponFireInfo_t &wf);
void fireGrenade(weaponFireInfo_t &wf);
void fireRocket(weaponFireInfo_t &wf);
void fireRocketMulti(weaponFireInfo_t &wf);
void fireMicro(weaponFireInfo_t &wf);
void firePistonWhip(weaponFireInfo_t &wf);
void fireShotgunMine(weaponFireInfo_t &wf);
void fireTearGas(weaponFireInfo_t &wf);
void firePhosphorusGrenade(weaponFireInfo_t &wf);
void fireMegaRocket(weaponFireInfo_t &wf);
void fireMicrowaveCone(weaponFireInfo_t &wf);
void fireFlameThrower(weaponFireInfo_t &wf);
void fireAutogun(weaponFireInfo_t &wf);
void fireFlameGlob(weaponFireInfo_t &wf);
void fireDogAttack(weaponFireInfo_t &wf);
void fireGrenade(weaponFireInfo_t &wf);
void fireMolitov(weaponFireInfo_t &wf);
void fireConcGrenade(weaponFireInfo_t &wf);
void fireGasGren(weaponFireInfo_t &wf);
void fireMagicBullet(weaponFireInfo_t &wf);
void fireFlashGren(weaponFireInfo_t &wf);
void fireMachete(weaponFireInfo_t &wf);

void fireDekker(weaponFireInfo_t &wf);

void fireProjTest(weaponFireInfo_t &wf);		

void cacheKnife(void);	
void cacheKnifeThrow(void);
void cacheStarThrow(void);
void cachePistol1(void);	
void cachePistol2(void);	
void cacheMPistol(void);	
void cacheAssaultR(void);	
void cacheSniper(void);	
void cacheAutoShot(void);	
void cacheConcGren(void);	
void cacheShotgun(void);	
void cacheMachinegun(void);
void cachePhosGren(void);	
void cacheRocket(void);	
void cacheRocketMulti(void);	
void cacheMicro(void);	
void cacheMicro2(void);	
void cacheDekkerMicro(void);	
void cacheFlameBlast(void);
void cacheFlameGlob(void);
void cacheGrenade(void);
void cacheMolitov(void);
void cacheWeaponCommon(void);

// *** MODs are only needed where fireBullet() is used in the table below.

CWeaponInfo wInfo[] = 
{			
	//																												soundRange		bulletSize						EnemySpawnAdd											penetrate		precachefunc				tracerEf						index
	//			inviewflash						dmflash							enemyflash									dmg				damageFlag		spawnAdd		MOD			fireFunc						absorb						efrange							bighit
	CWeaponInfo("",								"",								"",									0,		0,		3,		0,				0.00,	0.00,	0,					0,						0,		0,		0,					0,		"",						false,	ATK_NOTHING),
	CWeaponInfo("",								"weapons/othermz/knife",		"weapons/othermz/knife",			0,		22,		3,		DT_KNIFE,		0.00,	0.00,	0,					fireKnife,				0,		.25,	cacheKnife,			30,		"",						false,	ATK_KNIFE),
	CWeaponInfo("",								"",								"",									0,		30,		2,		DT_KNIFE,		0.00,	0.00,	0,					fireKnifeThrow,			0,		0,		cacheKnifeThrow,	0,		"",						false,	ATK_KNIFE_ALT),
	CWeaponInfo("weapons/playermz/pistol1",		"weapons/othermz/pistol1_p",	"weapons/othermz/pistol1",			300,	45,		2,		0,				0.03,	0.03,	MOD_PISTOL1,		fireBullet,				0,		0,		cachePistol1,		512,  "weapons/world/tracer_s",	false,	ATK_PISTOL1),
	CWeaponInfo("weapons/playermz/pistol2",		"weapons/othermz/pistol2_p",	"weapons/othermz/pistol2",			700,	120,	5,		DT_SEVER,		0.08,	0.08,	MOD_PISTOL2,		fireBullet,				0.5,	0,		cachePistol2,		800,  "weapons/world/tracer_l",	true,	ATK_PISTOL2),
	CWeaponInfo("weapons/playermz/machinepistol","weapons/othermz/machinepistol","weapons/othermz/machinepistol",	0,		30,		2,		DT_MPISTOL,		0.00,	0.00,	MOD_MPISTOL,		fireBullet,				0,		.5,		cacheMPistol,		512,  "weapons/world/tracer_s",	false,	ATK_MACHINEPISTOL),
	CWeaponInfo("weapons/playermz/assaultrifle","weapons/othermz/assaultrifle_p","weapons/othermz/assaultrifle",	600,	45,		4,		DT_JUGGLE,		0.03,	0.03,	MOD_ASSAULTRIFLE,	fireBullet,				0,		0,		cacheAssaultR,		512,  "weapons/world/tracer",	false,	ATK_ASSAULTRIFLE),
	CWeaponInfo("weapons/playermz/sniper",		"weapons/othermz/sniper",		"weapons/othermz/sniperenemy",		0,		80,		6,		DT_SNIPER,		0.00,	0.00,	MOD_SNIPERRIFLE,	fireBullet,				0.5,	0,		cacheSniper,		1600, "weapons/world/tracer_n",	false,	ATK_SNIPER),
	CWeaponInfo("",								"",								"",									0,		30,		3,		0,				0.00,	0.00,	0,					0,						0,		0,		0,					512,  "weapons/world/tracer_n",	false,	ATK_SNIPER_ALT),
	CWeaponInfo("weapons/playermz/autoshot",	"weapons/othermz/autoshot_p",	"weapons/othermz/autoshot",			400,	80,		3,		DT_AUTOSHOTGUN,	0.04,	0.04,	MOD_AUTOSHOTGUN,	fireShotgun,			0,		0,		cacheAutoShot,		512,	"",						false,	ATK_AUTOSHOTGUN),
	CWeaponInfo("weapons/playermz/autoalt",		"weapons/othermz/autoalt_p",	"weapons/othermz/autoalt",			800,	200,	3,DAMAGE_ALL_KNOCKBACK,	0.02,	0.02,	MOD_CONC_GRENADE,	fireConcGrenade,		0,		0,		cacheConcGren,		256,	"",						false,	ATK_AUTOSHOTGUN_ALT),
	CWeaponInfo("weapons/playermz/shotgun",		"weapons/othermz/shotgun_p",	"weapons/othermz/shotgun",			800,	40,		1,		DT_SHOTGUN,		0.20,	0.20,	MOD_SHOTGUN,		fireSpas,				0,		.5,		cacheShotgun,		128,	"",						false,	ATK_SHOTGUN),	//	-> damage is per pellet
	CWeaponInfo("weapons/playermz/machinegun",	"weapons/othermz/machinegun_p",	"weapons/othermz/machinegun",		1000,	100,	7,		DT_SEVER,		0.08,	0.04,	MOD_MACHINEGUN,		fireBullet,				0.25,	0,		cacheMachinegun,	1024, "weapons/world/tracer_l",	true,	ATK_MACHINEGUN),
	CWeaponInfo("weapons/playermz/phos",		"weapons/othermz/phos_p",		"weapons/othermz/phos",				500,	30,		3,		0,				0.01,	0.01,	0,					firePhosphorusGrenade,	0,		0,		cachePhosGren,		512,	"",						false,	ATK_MACHINEGUN_ALT),
	CWeaponInfo("weapons/playermz/rocket",		"weapons/othermz/rocket_p",		"weapons/othermz/rocket",			0,		30,		5,		0,				0.20,	0.20,	0,					fireRocket,				0,		0,		cacheRocket,		512,	"",						false,	ATK_ROCKET),
	CWeaponInfo("weapons/playermz/rocket_alt",	"weapons/othermz/rocket_alt_p",	"weapons/othermz/rocket_alt",		0,		15,		5,		0,				0.10,	0.10,	0,					fireRocketMulti,		0,		0,		cacheRocketMulti,	512,	"",						false,	ATK_ROCKET_ALT),
	CWeaponInfo("weapons/playermz/mpgbeam1",	"",								"",									0,		10,		3,		0,				0.00,	0.00,	MOD_MPG,			fireMicrowaveCone,		1.0,	0,		cacheMicro2,		512,	"",						false,	ATK_MICROWAVE),
	CWeaponInfo("weapons/playermz/mpg",			"weapons/othermz/mpg",			"weapons/othermz/mpgenemy",			0,		200,	3,		0,				0.00,	0.00,	MOD_MPG,			fireMicro,				1.0,	0,		cacheMicro,			512,	"",						false,	ATK_MICROWAVE_ALT),
	CWeaponInfo("weapons/playermz/flamegun",	"",								"",									0,		15,		3,		0,				0.00,	0.00,	0,					fireFlameThrower,		0,		0,		cacheFlameBlast,	512,	"",						false,	ATK_FLAMEGUN),
	CWeaponInfo("weapons/playermz/flamerelease","weapons/othermz/flamerelease",	"weapons/othermz/flamerelease",		0,		30,		3,		0,				0.30,	0.30,	0,					fireFlameGlob,			0,		0,		cacheFlameGlob,		512,	"",						false,	ATK_FLAMEGUN_ALT),
	CWeaponInfo("",								"",								"",									0,		30,		3,		0,				0.03,	0.03,	0,					fireDogAttack,			0,		0,		0,					512,	"",						false,	ATK_DOG_ATTACK),
	CWeaponInfo("",								"",								"",									0,		10,		3,		0,				0.10,	0.10,	0,					fireAutogun,			0,		0,		0,					512,  "weapons/world/tracer_l",	false,	ATK_AUTOGUN),
	CWeaponInfo("",								"",								"",									0,		30,		3,		0,				0.05,	0.05,	0,					fireGrenade,			0,		0,		cacheGrenade,		512,	"",						false,	ATK_GRENADE),
	CWeaponInfo("",								"",								"",									0,		30,		3,		0,				0.10,	0.10,	0,					fireMolitov,			0,		0,		0,/*cacheMolitov,*/	512,	"",						false,	ATK_MOLITOV),
	CWeaponInfo("",								"",								"",									0,		30,		3,		0,				0.00,	0.00,	0,					fireGasGren,			0,		0,		0,					512,	"",						false,	ATK_GASGREN),  // fixme
	CWeaponInfo("",								"",								"",									0,		30,		3,		0,				0.00,	0.00,	0,					fireMagicBullet,		0,		0,		0,					512,	"",						false,	ATK_MAGICBULLET),  // fixme
	CWeaponInfo("",								"",								"",									0,		30,		3,		0,				0.00,	0.00,	0,					fireFlashGren,			0,		0,		0,					512,	"",						false,	ATK_FLASHGREN),  // fixme
	CWeaponInfo("",								"",								"",									0,		30,		3,		0,				0.00,	0.00,	0,					fireMachete,			0,		0,		0,					512,	"",						false,	ATK_MACHETE),  // fixme
	CWeaponInfo("",								 "",							"",									1500,	180,	15,		DT_SEVER,		0.10,	0.10,	0,					fireBullet,				0,		0,		cacheAssaultR,		512,  "weapons/world/tracer_l",	false,	ATK_HELIGUN),  // fixme
	CWeaponInfo("",								"weapons/othermz/gb_dekmz",		"weapons/othermz/gb_dekmz",			0,		300,	3,		0,				0.02,	0.02,	0,					fireDekker,				0,		0,		cacheDekkerMicro,	512,  "weapons/world/tracer_l",	true,	ATK_DEKKER),  // fixme
	CWeaponInfo("",								"",								"",									0,		30,		2,		0,				0.00,	0.00,	0,					fireStarThrow,			0,		0,		cacheStarThrow,		0,		"",						false,	ATK_THROWSTAR),
};

CWeaponInfo::CWeaponInfo(char *newMFlashView, char *newMFlashDM, char *newMFlashSingle, 
						 float soundRange, float damage, float bSize, int dFlags, 
						 float spawnAdd, float enemySpawnAdd, int MeansODeath, void (*infunc)(weaponFireInfo_t &wf), 
						 float newDPenetrate, float newDAbsorb, void (*cacheFunc)(void), float newEfRange, 
						 char *newtracerFX, qboolean newbighit, int newIndex)
{
	sndRange = soundRange;
	dmg = damage;
	bulletSize = bSize;
	damageFlag = dFlags;
	MOD=MeansODeath;

	if (newMFlashView)		strcpy(mflashView,	newMFlashView);
	if (newMFlashDM)		strcpy(mflashDM,	newMFlashDM);
	if (newMFlashSingle)	strcpy(mflashSingle,newMFlashSingle);
	if (tracerFX)			strcpy(tracerFX,	newtracerFX);

	fire = infunc;
	precache = cacheFunc;
	dPenetrate = newDPenetrate;
	dAbsorb = newDAbsorb;
	bighit = newbighit;

	effectiveRange = newEfRange;

	addToSpawn = spawnAdd;
	enemyAddToSpawn = enemySpawnAdd;

	mflashEfView = 0;
	mflashEfDM = 0;
	mflashEfSingle = 0;

	tracerEf = 0;

	index = newIndex;
}

void CWeaponInfo::cache(void)
{
	// Other muzzle flashes.
	if (dm->isDM())
	{	// Other player muzzle flash
		if(strcmp(mflashDM, ""))
		{
			mflashEfDM = gi.effectindex(mflashDM);
		}
	}
	else
	{	// Other enemy AI muzzle flash
		if(strcmp(mflashSingle, ""))
		{
			mflashEfSingle = gi.effectindex(mflashSingle);
		}
	}

	// in view weapon muzzle flash
	if(strcmp(mflashView, ""))
	{
		mflashEfView = gi.effectindex(mflashView);
	}

	// Tracers from other players and enemies
	if(strcmp(tracerFX, ""))
	{
		tracerEf = gi.effectindex(tracerFX);
	}

	if(precache)
	{
		precache();
	}
}

void CWeaponInfo::attack(edict_t *srcEnt, vec3_t org, vec3_t dir, IGhoulInst *gun, GhoulID myFlashBolt)
{
	if(!fire)return;
	if (srcEnt && srcEnt->client)
	{
		vec3_t dest, face;
		VectorCopy(srcEnt->velocity, dest);
		VectorAdd(dest, srcEnt->s.origin, dest);
		AngleVectors(srcEnt->client->ps.viewangles, face, NULL, NULL);
		VectorMA(srcEnt->s.origin, 100, face, face);

		PB_PlaySequenceForAttack(srcEnt, dest, face, NULL);
	}

	weaponFireInfo_t info;
	info.ent = srcEnt;
	VectorCopy(org, info.firePoint);
	VectorCopy(dir, info.fwd);
	info.weapon = this;
	info.gunPointSet = 0;
	info.inst = gun;

	if(gun && myFlashBolt)
	{
		info.gunPointSet = 1;
		GetGhoulPosDir(srcEnt->s.origin, srcEnt->s.angles, gun, myFlashBolt, 0, info.gunPoint, 0, 0, 0);
	}

	// This is a weak way to handle it, but at least a weapon can have a different spawnadd for enemies versus players
	if (srcEnt->ai)
		IncreaseSpawnIntensity(enemyAddToSpawn);
	else
		IncreaseSpawnIntensity(addToSpawn);

	fire(info);
}

void CWeaponInfo::attack(edict_t *srcEnt, vec3_t org, vec3_t dir, float vel)
{
	if(!fire)return;

	weaponFireInfo_t info;
	info.ent = srcEnt;
	VectorCopy(org, info.firePoint);
	VectorCopy(dir, info.fwd);
	info.weapon = this;
	info.vel = vel;
	info.gunPointSet = 0;

	// This is a weak way to handle it, but at least a weapon can have a different spawnadd for enemies versus players
	if (srcEnt->ai)
		IncreaseSpawnIntensity(enemyAddToSpawn);
	else
		IncreaseSpawnIntensity(addToSpawn);

	fire(info);
}


void CWeaponInfo::attack(edict_t *srcEnt, vec3_t org, vec3_t dir, void (*bodyfunc)(edict_t *ent, void* data))
{
	if(!fire)return;

	weaponFireInfo_t info;
	info.ent = srcEnt;
	VectorCopy(org, info.firePoint);
	VectorCopy(dir, info.fwd);
	info.weapon = this;
	info.hitfunc = bodyfunc;
	info.gunPointSet = 0;

	// This is a weak way to handle it, but at least a weapon can have a different spawnadd for enemies versus players
	if (srcEnt->ai)
		IncreaseSpawnIntensity(enemyAddToSpawn);
	else
		IncreaseSpawnIntensity(addToSpawn);

	fire(info);
}


float CWeaponInfo::getDmg(void)
{
	if (dm->dmRule_REALISTIC_DAMAGE())
	{
		switch(getIndex())
		{
		case ATK_SNIPER:
		case ATK_SNIPER_ALT:
			return 110; // any non-limb shot with sniper in realistic mode will kill! mwahaha
			break;
		default:
			break;
		}
	}
	return dmg;
}

int CWeaponInfo::getDmgFlags(void)
{
	if (dm->dmRule_REALISTIC_DAMAGE())
	{
		switch(getIndex())
		{
		case ATK_SNIPER:
		case ATK_SNIPER_ALT:
		case ATK_PISTOL2:
			return damageFlag|DAMAGE_NO_ARMOR; // sniper and .44 are armor piercing with realistic damage
			break;
		default:
			break;
		}
	}
	
	return damageFlag;
}

int CWeaponInfo::getMOD(void)
{
	return MOD;
}

float CWeaponInfo::getPenetrate(void)
{
	return dPenetrate;
}

float CWeaponInfo::getAbsorb(void)
{
	return dAbsorb;
}

float CWeaponInfo::getBulletSize(void)
{
	return bulletSize;
}

float CWeaponInfo::getNoiseRad(void)
{
	return sndRange;
}

int CWeaponInfo::getMFlashEfView(void)
{
	if (!mflashEfView)
	{
		if (mflashView)
		{
			mflashEfView = gi.effectindex(mflashView);
		}
	}

	return mflashEfView;
}

int CWeaponInfo::getMFlashEfDM(void)
{
	if (!mflashEfDM)
	{
		if (mflashDM)
		{
			mflashEfDM = gi.effectindex(mflashDM);
		}
	}

	return mflashEfDM;
}

int CWeaponInfo::getMFlashEfSingle(void)
{
	if (!mflashEfSingle)
	{
		if (mflashSingle)
		{
			mflashEfSingle = gi.effectindex(mflashSingle);
		}
	}

	return mflashEfSingle;
}

int CWeaponInfo::getTracerEf(void)
{
	if (!tracerEf)
	{
		if (tracerFX)
		{
			tracerEf = gi.effectindex(tracerFX);
		}
	}

	return tracerEf;
}

float CWeaponInfo::getEffectiveRange(void)
{
	if (dm->isDM())
	{
		switch(getIndex())
		{
		case ATK_SNIPER:
		case ATK_SNIPER_ALT:
			return 4800; // sniper has a really long range in dm
			break;
		default:
			break;
		}
	}
	
	return effectiveRange;
}

int CWeaponInfo::getIndex(void)
{
	return index;
}


qboolean CWeaponInfo::getBigHit(void)
{
	return bighit;
}

/***************************

  CWorldWeapons

****************************/

CWorldWeapons weapons;

void CWorldWeapons::attack(attacks_e weapID, edict_t *srcEnt, vec3_t org, vec3_t dir, IGhoulInst *gun, GhoulID myFlashBolt)
{
	//normal attacking

	assert(weapID < ATK_NUMWEAPONS);
	assert(weapID >= 0);
	assert(weaps);

	GhoulID flash = 0;

	if(gun)
	{
		if(myFlashBolt)
		{
			flash = myFlashBolt;
		}
		else
		{
			flash = gun->GetGhoulObject()->FindPart("flash");
		}
	}

	weaps[weapID].attack(srcEnt, org, dir, gun, flash);

	if(gun)
	{
		if(flash)
		{
			if (dm->isDM())
			{
				fxRunner.execWithInst(weaps[weapID].getMFlashEfDM(), srcEnt, gun, flash, MULTICAST_PHS);
			}
			else
			{
				fxRunner.execWithInst(weaps[weapID].getMFlashEfSingle(), srcEnt, gun, flash, MULTICAST_PHS);
			}
		}
	}
}

void CWorldWeapons::attack(attacks_e weapID, edict_t *srcEnt, vec3_t org, vec3_t dir, IGhoulInst *gun, float *vel)
{
	// for lobbed projectiles

	assert(weapID < ATK_NUMWEAPONS);
	assert(weapID >= 0);
	assert(weaps);

	weaps[weapID].attack(srcEnt, org, dir, *vel);
}

void CWorldWeapons::attack(attacks_e weapID, edict_t *srcEnt, vec3_t org, vec3_t dir, 
						   void (*bodyfunc)(edict_t *ent, void* data), IGhoulInst *gun)
{
	// for the dog, I believe

	assert(weapID < ATK_NUMWEAPONS);
	assert(weapID >= 0);
	assert(weaps);

	weaps[weapID].attack(srcEnt, org, dir, bodyfunc);

	if(gun)
	{
		GhoulID flash = gun->GetGhoulObject()->FindPart("flash");

		if(flash)
		{
			if (dm->isDM())
			{
				fxRunner.execWithInst(weaps[weapID].getMFlashEfDM(), srcEnt, gun, flash, MULTICAST_PHS, 1);
			}
			else
			{
				fxRunner.execWithInst(weaps[weapID].getMFlashEfSingle(), srcEnt, gun, flash, MULTICAST_PHS, 1);
			}
		}
	}
}

void W_InitWorldWeaponModels(void)
{
	//hack!  precache item shit - make specific later
	for(int e = 0; e < SFE_NUMITEMS; e++)
	{
		cacheForItem((equipment_t)e);
	}

	cacheWeaponCommon();

	weapons.setWeaps(wInfo);
}

void W_ShutdownWorldWeaponModels(void)
{
	int i;

	for(i = 0; i < SFW_NUM_WEAPONS; i++)
	{
	}
}



/****************************

  model data for class Pickup

 ****************************/

// any item added for use in single player games MUST GO IN THIS ARRAY...it's a requirement of our save/load routines.
modelSpawnData_t pickupModelData[MD_ITEM_SIZE] =
{	
							//dir						file				surfaceType			material			health						solid			materialfile   debrisCnt  debrisScale		objBreakData
/*OBJ_NONE*/				NULL,						NULL,				SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			0,			0.0,			NULL,	// OBJ_NONE
/*OBJ_AMMO_KNIFE*/			"enemy/bolt",				"w_knife",			SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			0,			0.0,			NULL,	// OBJ_AMMO_KNIFE
/*OBJ_AMMO_PISTOL*/			"Items/Pick-ups/Ammo",		"box",				SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"box",			-4,			0.0,			NULL,	// OBJ_AMMO_PISTOL,
/*OBJ_AMMO_SHOTGUN*/		"Items/Pick-ups/Ammo",		"box",				SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"box",			-4,			0.0,			NULL,	// OBJ_AMMO_SHOTGUN
/*OBJ_AMMO_PISTOL2*/		"Items/Pick-ups/Ammo",		"box",				SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"box",			-4,			0.0,			NULL,	// OBJ_AMMO_PISTOL2
/*OBJ_AMMO_AUTO*/			"Items/Pick-ups/Ammo",		"box",				SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"box",			-4,			0.0,			NULL,	// OBJ_AMMO_AUTO
/*OBJ_AMMO_MPG*/			"Items/Pick-ups/Ammo",		"box",				SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"box",			-4,			0.0,			NULL,	// OBJ_AMMO_MPG
/*OBJ_AMMO_GAS*/			"Items/Pick-ups/Ammo",		"box",				SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"box",			-4,			0.0,			NULL,	// OBJ_AMMO_GAS
/*OBJ_AMMO_ROCKET*/			"Items/Pick-ups/Ammo",		"box",				SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"box",			-4,			0.0,			NULL,	// OBJ_AMMO_ROCKET
/*OBJ_AMMO_SLUG*/			"Items/Pick-ups/Ammo",		"box",				SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"box",			-4,			0.0,			NULL,	// OBJ_AMMO_SLUG
																																																					
/*OBJ_AMMO_SINGLE_PISTOL*/	"Items/Pick-ups/Ammo_single","ammo_9mm",		SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"9mm",			-4,			0.0,			NULL,	// OBJ_AMMO_SINGLE_PISTOL,
/*OBJ_AMMO_SINGLE_SHOTGUN*/	"Items/Pick-ups/Ammo_single","ammo_shotgun",	SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"ammo_shotgun", -4,			0.0,			NULL,	// OBJ_AMMO_SINGLE_SHOTGUN
/*OBJ_AMMO_SINGLE_PISTOL2*/	"Items/Pick-ups/Ammo_single","ammo_44",			SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"ammo_44",		-4,			0.0,			NULL,	// OBJ_AMMO_SINGLE_PISTOL2,
/*OBJ_AMMO_SINGLE_AUTO*/	"Items/Pick-ups/Ammo_single","ammo_556",		SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"ammo_556",		-4,			0.0,			NULL,	// OBJ_AMMO_SINGLE_AUTO,
/*OBJ_AMMO_SINGLE_MPG*/		"Items/Pick-ups/Ammo_single","ammo_battery",	SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"ammo_battery",		-4,			0.0,			NULL,	// OBJ_AMMO_SINGLE_MPG,
/*OBJ_AMMO_SINGLE_GAS*/		"Items/Pick-ups/Ammo_single","ammo_gas",		SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"ammo_gas",		-4,			0.0,			NULL,	// OBJ_AMMO_SINGLE_GAS,
/*OBJ_AMMO_SINGLE_ROCKET*/	"Items/Pick-ups/Ammo_single","ammo_rocket",		SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"ammo_rocket",	-4,			0.0,			NULL,	// OBJ_AMMO_SINGLE_ROCKET,
/*OBJ_AMMO_SINGLE_SLUG*/	"Items/Pick-ups/Ammo_single","ammo_shotgun",	SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		"ammo_shotgun",	-4,			0.0,			NULL,	// OBJ_AMMO_SINGLE_SLUG,

/*OBJ_WPN_PISTOL2*/			"enemy/bolt",				"w_pistol2",		SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			-2,			0.0,			NULL,	// OBJ_WPN_PISTOL2
/*OBJ_WPN_PISTOL1*/			"enemy/bolt",				"w_pistol1",		SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			-2,			0.0,			NULL,	// OBJ_WPN_PISTOL1
/*OBJ_WPN_MACHINEPISTOL*/	"enemy/bolt",				"w_machinepistol",	SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			-2,			0.0,			NULL,	// OBJ_WPN_MACHINEPISTOL
/*OBJ_WPN_ASSAULTRIFLE*/	"enemy/bolt",				"w_assault_rifle",	SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			-2,			0.0,			NULL,	// OBJ_WPN_ASSAULTRIFLE
/*OBJ_WPN_SNIPER*/			"enemy/bolt",				"w_sniperrifle",	SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			-2,			0.0,			NULL,	// OBJ_WPN_SNIPER
/*OBJ_WPN_AUTOSHOTGUN*/		"enemy/bolt",				"w_autoshotgun",	SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			-6,			0.0,			NULL,	// OBJ_WPN_AUTOSHOTGUN
/*OBJ_WPN_SHOTGUN*/			"enemy/bolt",				"w_shotgun",		SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			-2,			0.0,			NULL,	// OBJ_WPN_SHOTGUN
/*OBJ_WPN_MACHINEGUN*/		"enemy/bolt",				"w_machinegun",		SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			-2,			0.0,			NULL,	// OBJ_WPN_MACHINEGUN
/*OBJ_WPN_ROCKET*/			"enemy/bolt",				"w_rocket",			SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			-2,			0.0,			NULL,	// OBJ_WPN_ROCKET
/*OBJ_WPN_MICROWAVE*/		"enemy/bolt",				"w_mpg",			SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			-2,			0.0,			NULL,	// OBJ_WPN_MICROWAVE
/*OBJ_WPN_FLAMEGUN*/		"enemy/bolt",				"w_flamethrower",	SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			-2,			0.0,			NULL,	// OBJ_WPN_FLAMEGUN

/*OBJ_ARMOR*/				"Items/Pick-ups/Armor",		"armor",			SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			0,			0.0,			NULL,	// OBJ_ARMOR
															   
/*OBJ_ITM_FLASHPACK*/		"Items/Projectiles",		"flashpak",			SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,		   -6,			0.0,			NULL,	// OBJ_ITM_FLASHPACK
/*OBJ_ITM_NEURALGRENADE		NULL,						NULL,				SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			0,			0.0,			NULL,	// OBJ_ITM_NEURALGRENADE*/
/*OBJ_ITM_C4*/				"Items/Projectiles",		"c4",				SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,		   -4,			0.0,			NULL,	// OBJ_ITM_C4
/*OBJ_ITM_LIGHTGOGGLES*/	"enemy/bolt",				"acc_nightvision",	SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,		   -4,			0.0,			NULL,	// OBJ_ITM_LIGHTGOGGLES
/*OBJ_ITM_MINE*/			"Items/Projectiles",		"claymore",			SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,		   -4,			0.0,			NULL,	// OBJ_ITM_MINE
/*OBJ_ITM_MEDKIT*/			"Items/Pick-ups/MedKit",	"medkit",			SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			0,			0.0,			NULL,	// OBJ_ITM_MEDKIT
/*OBJ_ITM_GRENADE*/			"Items/Projectiles",		"grenade",			SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,		   -2,			0.0,			NULL,	// OBJ_ITM_GRENADE
/*OBJ_CTF_FLAG*/			"Items/ctf_flag",			"flag_ground_idle",	SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			0,			0.0,			NULL,	// OBJ_ITM_CTF_FLAG
							
/*OBJ_HLTH_SM*/				"Items/Pick-ups/healthsm",	"health",			SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			0,			0.0,			NULL,	// OBJ_HLTH_SM
/*OBJ_HLTH_LRG*/			"Items/Pick-ups/healthbig",	"health",			SURF_DEFAULT,		MAT_DEFAULT,		1,							SOLID_BBOX,		NULL,			0,			0.0,			NULL,	// OBJ_HLTH_LRG

};

// save/load stuff requires that all single player model data be stored in one giant array (pickupModelData)
//that gets referenced in g_save.cpp. since this low poly model is only for deathmatch, we don't have to store
//it in pickupModelData
modelSpawnData_t lowPolyArmorModel =
{
	"Items/Pick-ups/Armor",		"armordm",			SURF_DEFAULT,		MAT_DEFAULT,		1,			SOLID_BBOX,		NULL,			0,	0.0,			NULL
};

/****************************


class Pickup 

	any item the player can pick up during either a single player or multiplayer game is a Pickup.

 ****************************/

PickupRespawnCallback thePickupRespawnCallback;

bool PickupRespawnCallback::Execute(IGhoulInst *me,void *ent,float time,const void *matrix)
{	// when a pickup respawns it plays a sequence in which it fades into view. at the end of that sequence, 
	//this function is called, meaning it's time to play the pickup's normal, one-frame sequence.
	edict_t			*self = (edict_t*)ent;
	IGhoulInst		*ghoulInst = self?self->ghoulInst:NULL;
	PickupInst		*pickupInst = thePickupList.GetPickupInstFromEdict(self);

	if (pickupInst)
	{
		pickupInst->RespawnCallback();
	}
	return true;
}


Pickup::Pickup()
{
	memset(m_spawnName, 0, MAXCHARSFORPICKUPNAME);
	m_type = PU_UNKNOWN;
	m_spawnParm = SFW_EMPTYSLOT;		// this is kind of arbitrary since it could be one of several enums. i just want it 0 for now.
	m_defaultCount = 0;
	m_defaultMaxCount = -1;
	m_respawnTime = 30;
	m_pickupListIndex = OBJ_NONE;
	memset(m_worldName, 0, MAXCHARSFORPICKUPNAME);
	m_pickupStringIndex = m_ammoPickupStringIndex = m_itemFullStringIndex = 0;
	m_modelSpawnData = NULL;

	memset(m_skin, 0, MAXCHARSFORPICKUPNAME);
	m_scale = 1.0;
	VectorClear(m_bboxAdjustMins);
	m_lowPolyModelSpawnData = NULL;
	memset(m_fadeInSeq, 0, MAXCHARSFORPICKUPNAME);
	m_droppedStringIndex = 0;
}

Pickup::Pickup(	char		*yourSpawnNameHere,
				pickup_t	yourTypeHere,
				int			yourSpawnParmHere,
				float		yourRespawnTimeHere,
				int			yourDefaultCountHere,
				int			yourDefaultMaxCountHere,
				objType_t	yourPickupListIndexHere,
				char		*yourWorldNameHere,
				int			yourPickupStringIndexHere,
				int			yourAmmoPickupStringIndexHere,
				int			yourItemFullStringIndexHere
				)
{
	strncpy(m_spawnName, yourSpawnNameHere, MAXCHARSFORPICKUPNAME);
	m_type = yourTypeHere;
	m_spawnParm = yourSpawnParmHere;
	m_respawnTime = yourRespawnTimeHere;
	m_defaultCount = yourDefaultCountHere;
	m_defaultMaxCount = yourDefaultMaxCountHere;
	m_pickupListIndex = yourPickupListIndexHere;
	strncpy(m_worldName, yourWorldNameHere, MAXCHARSFORPICKUPNAME);
	m_pickupStringIndex = yourPickupStringIndexHere;
	m_ammoPickupStringIndex = yourAmmoPickupStringIndexHere;
	m_itemFullStringIndex = yourItemFullStringIndexHere;
	m_modelSpawnData = NULL;

	memset(m_skin, 0, MAXCHARSFORPICKUPNAME);
	m_scale = 1.0;
	VectorClear(m_bboxAdjustMins);
	m_lowPolyModelSpawnData = NULL;
	memset(m_fadeInSeq, 0, MAXCHARSFORPICKUPNAME);
	m_droppedStringIndex = 0;
}

Pickup::~Pickup()
{
	m_modelSpawnData = NULL;
	m_lowPolyModelSpawnData = NULL;
}

// here's where we set up our modelData pointer. we could maybe put some file verification stuff in here...you know,
//return something helpful if the file isn't where we were told it'd be, that sort of thing.
int Pickup::Init()
{
	m_modelSpawnData = &pickupModelData[m_pickupListIndex];
	return true;
}

char *Pickup::GetWorldName()
{
	return m_worldName;
}
void Pickup::SetFadeInSeq(char *seq)
{
	if (seq)
	{
		strncpy(m_fadeInSeq, seq, MAXCHARSFORPICKUPNAME);
	}
}

void Pickup::SetGSQ(char *gsq)
{
	if (gsq)
	{
		if (m_modelSpawnData && m_modelSpawnData->dir)
		{	// precache fade-in seqs
			ggObjC* obj = game_ghoul.FindObject(m_modelSpawnData->dir, gsq);
			obj = NULL;
		}
	}
}

int Pickup::GetDefaultMaxCount()
{
	// no maximum in deathmatch
	return (dm->isDM()?-1:m_defaultMaxCount);
}

/****************************

class MedKitPickup  

****************************/

MedKitPickup::MedKitPickup(	char		*yourSpawnNameHere,
							pickup_t	yourTypeHere,
							int			yourSpawnParmHere,
							float		yourRespawnTimeHere,
							int			yourDefaultCountHere,
							int			yourDefaultMaxCountHere,
							objType_t	yourPickupListIndexHere,
							char		*yourWorldNameHere,
							int			yourPickupStringIndexHere,
							int			yourAmmoPickupStringIndexHere,
							int			yourItemFullStringIndexHere
						)
{
	strncpy(m_spawnName, yourSpawnNameHere, MAXCHARSFORPICKUPNAME);
	m_type = yourTypeHere;
	m_spawnParm = yourSpawnParmHere;
	m_respawnTime = yourRespawnTimeHere;
	m_defaultCount = yourDefaultCountHere;
	m_defaultMaxCount = yourDefaultMaxCountHere;
	m_pickupListIndex = yourPickupListIndexHere;
	strncpy(m_worldName, yourWorldNameHere, MAXCHARSFORPICKUPNAME);
	m_pickupStringIndex = yourPickupStringIndexHere;
	m_ammoPickupStringIndex = yourAmmoPickupStringIndexHere;
	m_itemFullStringIndex = yourItemFullStringIndexHere;
	m_modelSpawnData = NULL;

	memset(m_skin, 0, MAXCHARSFORPICKUPNAME);
	m_scale = 1.0;
	VectorClear(m_bboxAdjustMins);
	m_lowPolyModelSpawnData = NULL;
	memset(m_fadeInSeq, 0, MAXCHARSFORPICKUPNAME);
}

MedKitPickup::~MedKitPickup()
{
	m_modelSpawnData = NULL;
	m_lowPolyModelSpawnData = NULL;
}

int MedKitPickup::GetDefaultMaxCount()
{
	if (dm->dmRule_REALISTIC_DAMAGE())
	{
		return 1;
	}
	return m_defaultMaxCount;
}
  
/****************************

class LightGogglesPickup

****************************/

LightGogglesPickup::LightGogglesPickup(	char		*yourSpawnNameHere,
										pickup_t	yourTypeHere,
										int			yourSpawnParmHere,
										float		yourRespawnTimeHere,
										int			yourDefaultCountHere,
										int			yourDefaultMaxCountHere,
										objType_t	yourPickupListIndexHere,
										char		*yourWorldNameHere,
										int			yourPickupStringIndexHere,
										int			yourAmmoPickupStringIndexHere,
										int			yourItemFullStringIndexHere
									)
{
	strncpy(m_spawnName, yourSpawnNameHere, MAXCHARSFORPICKUPNAME);
	m_type = yourTypeHere;
	m_spawnParm = yourSpawnParmHere;
	m_respawnTime = yourRespawnTimeHere;
	m_defaultCount = yourDefaultCountHere;
	m_defaultMaxCount = yourDefaultMaxCountHere;
	m_pickupListIndex = yourPickupListIndexHere;
	strncpy(m_worldName, yourWorldNameHere, MAXCHARSFORPICKUPNAME);
	m_pickupStringIndex = yourPickupStringIndexHere;
	m_ammoPickupStringIndex = yourAmmoPickupStringIndexHere;
	m_itemFullStringIndex = yourItemFullStringIndexHere;
	m_modelSpawnData = NULL;

	memset(m_skin, 0, MAXCHARSFORPICKUPNAME);
	m_scale = 1.0;
	VectorClear(m_bboxAdjustMins);
	m_lowPolyModelSpawnData = NULL;
	memset(m_fadeInSeq, 0, MAXCHARSFORPICKUPNAME);
}

int LightGogglesPickup::GetDefaultMaxCount()
{
	// always 100 whether singleplayer or deathmatch
	return m_defaultMaxCount;
}


/****************************

class WeaponPickup  

****************************/

WeaponPickup::WeaponPickup(	char		*yourSpawnNameHere,
							pickup_t	yourTypeHere,
							int			yourSpawnParmHere,
							float		yourRespawnTimeHere,
							int			yourDefaultCountHere,
							int			yourDefaultMaxCountHere,
							objType_t	yourPickupListIndexHere,
							char		*yourWorldNameHere,
							int			yourPickupStringIndexHere,
							int			yourAmmoPickupStringIndexHere,
							int			yourItemFullStringIndexHere,
							int			yourWpnFlagsHere
						)
{
	strncpy(m_spawnName, yourSpawnNameHere, MAXCHARSFORPICKUPNAME);
	m_type = yourTypeHere;
	m_spawnParm = yourSpawnParmHere;
	m_respawnTime = yourRespawnTimeHere;
	m_defaultCount = yourDefaultCountHere;
	m_defaultMaxCount = yourDefaultMaxCountHere;
	m_pickupListIndex = yourPickupListIndexHere;
	strncpy(m_worldName, yourWorldNameHere, MAXCHARSFORPICKUPNAME);
	m_pickupStringIndex = yourPickupStringIndexHere;
	m_ammoPickupStringIndex = yourAmmoPickupStringIndexHere;
	m_itemFullStringIndex = yourItemFullStringIndexHere;
	m_modelSpawnData = NULL;
	m_wpnFlags = yourWpnFlagsHere;

	memset(m_skin, 0, MAXCHARSFORPICKUPNAME);
	m_scale = 1.0;
	VectorClear(m_bboxAdjustMins);
	m_lowPolyModelSpawnData = NULL;
	memset(m_fadeInSeq, 0, MAXCHARSFORPICKUPNAME);
}

/****************************

class AmmoPickup  

****************************/

AmmoPickup::AmmoPickup(		char		*yourSpawnNameHere,
							pickup_t	yourTypeHere,
							int			yourSpawnParmHere,
							float		yourRespawnTimeHere,
							int			yourDefaultCountHere,
							int			yourDefaultMaxCountHere,
							objType_t	yourPickupListIndexHere,
							char		*yourWorldNameHere,
							int			yourPickupStringIndexHere,
							int			yourAmmoPickupStringIndexHere,
							int			yourItemFullStringIndexHere,
							int			yourAmmoFlagsHere
						)
{
	strncpy(m_spawnName, yourSpawnNameHere, MAXCHARSFORPICKUPNAME);
	m_type = yourTypeHere;
	m_spawnParm = yourSpawnParmHere;
	m_respawnTime = yourRespawnTimeHere;
	m_defaultCount = yourDefaultCountHere;
	m_defaultMaxCount = yourDefaultMaxCountHere;
	m_pickupListIndex = yourPickupListIndexHere;
	strncpy(m_worldName, yourWorldNameHere, MAXCHARSFORPICKUPNAME);
	m_pickupStringIndex = yourPickupStringIndexHere;
	m_ammoPickupStringIndex = yourAmmoPickupStringIndexHere;
	m_itemFullStringIndex = yourItemFullStringIndexHere;
	m_modelSpawnData = NULL;
	m_ammoFlags = yourAmmoFlagsHere;

	memset(m_skin, 0, MAXCHARSFORPICKUPNAME);
	m_scale = 1.0;
	VectorClear(m_bboxAdjustMins);
	m_lowPolyModelSpawnData = NULL;
	memset(m_fadeInSeq, 0, MAXCHARSFORPICKUPNAME);
}

/****************************


class PickupInst 


  **************************/

PickupInst::PickupInst(edict_t *ent)
{
	m_edict = ent;
	m_pickup = thePickupList.GetPickupIndexFromEdict(ent);
	m_fadeStatus = fading_NONE;
}

PickupInst::~PickupInst()
{
	m_edict = NULL;
}

IGhoulInst	*PickupInst::GetGhoulInst()
{
	return (m_edict?m_edict->ghoulInst:NULL);
}

void PickupInst::FadeIn()
{
	IGhoulInst *inst = GetGhoulInst();

	if (GetPickup()->GetFadeInSeq() && inst && 
		((m_fadeStatus == fading_FALSE) || (m_fadeStatus == fading_NONE)) )
	{
		GhoulID		tempNote=0;

		// play the sequence
		if (SimpleModelSetSequence2(inst, GetPickup()->GetFadeInSeq(), SMSEQ_HOLD))
		{
			// set a callback to switch back to normal sequence at the end of the fadein
			tempNote=inst->GetGhoulObject()->FindNoteToken("EOS");
			if (tempNote)
			{
				inst->AddNoteCallBack(&thePickupRespawnCallback,tempNote);
			}
			m_fadeStatus = fading_TRUE; // begin fading
		}
	}
}

void PickupInst::RespawnCallback()
{
	IGhoulInst	*ghoulInst = GetGhoulInst();

	if (ghoulInst && GetPickup())
	{
		if (m_fadeStatus != fading_TRUE)
		{	// already reached this callback for this particular inst. so leave.
			return;
		}
		if (GetPickup()->GetModelName())
		{
			if (GetPickup()->GetPickupListIndex() == OBJ_ITM_C4)
			{	// c4 shouldn't blink
				SimpleModelSetSequence2(ghoulInst, GetPickup()->GetModelName(), SMSEQ_HOLD);
			}
			else if (GetPickup()->GetType() == PU_ARMOR)
			{	// use special low-poly armor model
				SimpleModelSetSequence(m_edict, GetPickup()->GetLowPolyModel()->file, SMSEQ_HOLD);
			}
			else
			{
				SimpleModelSetSequence2(ghoulInst, GetPickup()->GetModelName(), SMSEQ_LOOP);
			}
		}
	}
	// I'm finished fading in. please remove my callback.
	m_fadeStatus = fading_REMOVECALLBACK;

	// Oh, and, by the way, you may want to pick me up at some point...
	m_edict->solid = SOLID_TRIGGER;
}

bool PickupInst::RemoveCallBack()
{
	GhoulID			tempNote=0;
	IGhoulInst		*ghoulInst = NULL;

	if (ghoulInst = GetGhoulInst())
	{
		tempNote=ghoulInst->GetGhoulObject()->FindNoteToken("EOS");
	}
	m_fadeStatus = fading_FALSE;
	if (tempNote)
	{
		ghoulInst->RemoveNoteCallBack(&thePickupRespawnCallback,tempNote);
		return true;
	}
	return false;
}

bool PickupInst::EdictCompare(edict_t *ent)
{
	return (m_edict == ent);
}


/****************************

  class PickupInfo

****************************/

PickupInfo::PickupInfo(objType_t type)
{
	m_objType = type;
}

PickupInfo::~PickupInfo()
{
	list<PickupInstInfo*>::iterator		it;

	for (it = m_insts.begin(); it != m_insts.end(); it++)
	{
		delete (PickupInstInfo*)(*it);
	}
	m_insts.clear();
}

PickupInstInfo *PickupInfo::AddInst(PickupInst *pickupInst, int type /*PickupInstInfo::type_LOADTIME*/)
{
	PickupInstInfo	*newInfo = NULL;

	// is this guy already in our list?
	if (newInfo = Find(pickupInst))
	{	// yup. return it.
		return newInfo;
	}
	// wasn't in our list already, so add it
	newInfo = new PickupInstInfo(pickupInst, type);
	m_insts.push_back(newInfo);
	return newInfo;
}

PickupInstInfo *PickupInfo::Find(edict_t *ent)
{
	list<PickupInstInfo*>::iterator	it;

	for(it = m_insts.begin(); it != m_insts.end(); it++)
	{
		if ((*it)->Compare(ent))
		{
			return (PickupInstInfo*)(*it);
		}
	}
	return NULL;
}

PickupInstInfo *PickupInfo::Find(PickupInst *pickupInst)
{
	list<PickupInstInfo*>::iterator	it;

	for(it = m_insts.begin(); it != m_insts.end(); it++)
	{
		if ((*it)->Compare(pickupInst))
		{
			return (PickupInstInfo*)(*it);
		}
	}
	return NULL;
}

bool PickupInfo::PickedUp(PickupInst *pickupInst, float fTime)
{
	PickupInstInfo	*info = Find(pickupInst);

	if (NULL == info)
	{
		return false;
	}
	info->PickedUp(fTime);
	return true;
}

bool PickupInfo::SetRespawnTime(PickupInst *pickupInst, float fTime)
{
	PickupInstInfo	*info = Find(pickupInst);

	if (NULL == info)
	{
		return false;
	}
	info->SetRespawnTime(fTime);
	return true;
}

bool PickupInfo::Touch(PickupInst *pickupInst, float fTime)
{
	PickupInstInfo	*info = Find(pickupInst);

	if (NULL == info)
	{
		return false;
	}
	info->Touch(fTime);
	return true;
}

/****************************

  class PickupInstInfo

****************************/

PickupInstInfo::PickupInstInfo(PickupInst *inst, int type /*type_LOADTIME*/)
{
	m_pickUpInst = inst;
	m_pickedUpCount = 0;
	m_lastRespawnTime = 0;
	m_lastPickedUpTime = 0.0;
	m_totalPickedUpDelay = 0.0;
	m_avgPickedUpDelay = 0.0;
	m_totalRespawnPickUpDelay = 0.0;
	m_avgRespawnPickUpDelay = 0.0;
	m_touchCount = 0;
	m_lastTouchTime = 0.0;
	m_totalTouchDelay = 0.0;
	m_avgTouchDelay = 0.0;
	m_type = type;
}

PickupInstInfo::~PickupInstInfo()
{
}

void PickupInstInfo::PickedUp(float fTime)
{
	float delay = fTime - m_lastPickedUpTime;

	m_totalPickedUpDelay += delay;
	m_pickedUpCount++;
	m_avgPickedUpDelay = m_totalPickedUpDelay/m_pickedUpCount;

	m_totalRespawnPickUpDelay += (fTime - m_lastRespawnTime);
	m_avgRespawnPickUpDelay = m_totalRespawnPickUpDelay/m_pickedUpCount;

	m_lastPickedUpTime = fTime;
}

void PickupInstInfo::Touch(float fTime)
{
	m_touchCount++;
	m_totalTouchDelay += (fTime - m_lastTouchTime);
	m_avgTouchDelay = m_totalTouchDelay/m_touchCount;
	m_lastTouchTime = fTime;
}

bool PickupInstInfo::Compare(PickupInst* pickupInst)
{
	return (m_pickUpInst == pickupInst);
}

bool PickupInstInfo::Compare(edict_t *ent)
{
	edict_t *myEnt = m_pickUpInst?m_pickUpInst->GetEdict():NULL;
	
	return (myEnt && (myEnt == ent));
}

/****************************

  class PickupInfoList

****************************/

PickupInfoList::PickupInfoList()
{
	memset(m_failedMsgs, 0, event_MAXEVENTS);
	m_infoFile = NULL;
	m_bInfoFileOpened = false;
}

PickupInfoList::~PickupInfoList()
{
	map<objType_t, PickupInfo*>::iterator	it;

	for (it = m_info.begin(); it != m_info.end(); it++)
	{
		delete (PickupInfo*)((*it).second);
	}
	m_info.clear();

	if (m_infoFile)
	{
		fclose(m_infoFile);
		m_infoFile = NULL;
	}
}

PickupInfo* PickupInfoList::AddPickupInst(PickupInst *pickupInst, int type /*type_LOADTIME*/)
{
	map<objType_t, PickupInfo*>::iterator	it;
	objType_t								objType = OBJ_NONE;

	if ((NULL == pickupInst) || (NULL == pickupInst->GetPickup()))
	{
		return NULL;
	}
	it = m_info.find(objType = pickupInst->GetPickup()->GetPickupListIndex());
	if (it != m_info.end())
	{	// add this inst to our list of insts for this type of pickup
		(PickupInfo*)((*it).second)->AddInst(pickupInst, type);
		return (PickupInfo*)((*it).second);
	}
	else
	{	// not in our list yet, so add it.
		m_info[objType] = new PickupInfo(objType);

		// now add the instance of this PickupInfo.
		it = m_info.find(objType);
		if (it == m_info.end())
		{   // couldn't add to map? bad bad bad.
			return NULL;
		}
		(PickupInfo*)((*it).second)->AddInst(pickupInst, type);
		return (PickupInfo*)m_info[objType];
	}
}

bool PickupInfoList::HandleEvent(int nEvent, PickupInst *pickupInst)
{
	bool bRetVal = false;
	switch(nEvent)
	{
	case event_NONE:
		{
			m_failedMsgs[event_NONE]++;
			break;
		}
	case event_CREATE_LOADTIME:
		{
			bRetVal = (NULL != AddPickupInst(pickupInst,PickupInstInfo::type_LOADTIME));
			break;
		}
	case event_CREATE_RUNTIME:
		{
			bRetVal = (NULL != AddPickupInst(pickupInst,PickupInstInfo::type_RUNTIME));
			break;
		}
	case event_PICKEDUP:
		{
			PickupInfo *info = AddPickupInst(pickupInst,PickupInstInfo::type_RUNTIME);
			if (NULL == info)
			{
				m_failedMsgs[event_PICKEDUP]++;
				break;
			}
			info->PickedUp(pickupInst, level.time);
			bRetVal = true;
			break;
		}
	case event_RESPAWN:
		{
			PickupInfo *info = AddPickupInst(pickupInst,PickupInstInfo::type_RUNTIME);
			if (NULL == info)
			{
				m_failedMsgs[event_RESPAWN]++;
				break;
			}
			info->SetRespawnTime(pickupInst, level.time);
			bRetVal = true;
			break;
		}
	case event_TOUCHED:
		{
			PickupInfo *info = AddPickupInst(pickupInst,PickupInstInfo::type_RUNTIME);
			if (NULL == info)
			{
				m_failedMsgs[event_TOUCHED]++;
				break;
			}
			info->Touch(pickupInst, level.time);
			bRetVal = true;
			break;
		}
	default:
		{
			m_failedMsgs[event_UNKNOWN]++;
			break;
		}
	}
	return bRetVal;
}

PickupInstInfo *PickupInfoList::Find(PickupInst *pickupInst)
{
	map<objType_t, PickupInfo*>::iterator	it;
	PickupInstInfo							*info = NULL;

	if ( (NULL == pickupInst) || (NULL == pickupInst->GetPickup()) )
	{
		return NULL;
	}
	it = m_info.find(pickupInst->GetPickup()->GetPickupListIndex());
	if (it == m_info.end())
	{
		return NULL;
	}
	return (PickupInstInfo*)((*it).second)->Find(pickupInst);
}

int PickupInfoList::GetPickedUpCount(PickupInst *pickupInst)
{
	PickupInstInfo	*info = Find(pickupInst);

	if (NULL == info)
	{
		return -1;
	}
	return info->GetPickedUpCount();
}

float PickupInfoList::GetAvgPickedUpDelay(PickupInst *pickupInst)
{
	PickupInstInfo	*info = Find(pickupInst);

	if (NULL == info)
	{
		return -1;
	}
	return info->GetAvgPickedUpDelay();
}

float PickupInfoList::GetAvgRespawnPickUpDelay(PickupInst *pickupInst)
{
	PickupInstInfo	*info = Find(pickupInst);

	if (NULL == info)
	{
		return -1;
	}
	return info->GetAvgRespawnPickUpDelay();
}

float PickupInfoList::GetAvgTouchDelay(PickupInst *pickupInst)
{
	PickupInstInfo	*info = Find(pickupInst);

	if (NULL == info)
	{
		return -1;
	}
	return info->GetAvgTouchDelay();
}

int	PickupInfoList::GetTouchCount(PickupInst *pickupInst)
{
	PickupInstInfo	*info = Find(pickupInst);

	if (NULL == info)
	{
		return -1;
	}
	return info->GetTouchCount();
}

int	PickupInfoList::GetType(PickupInst *pickupInst)
{
	PickupInstInfo	*info = Find(pickupInst);

	if (NULL == info)
	{
		return -1;
	}
	return info->GetType();
}

void PickupInfoList::WriteLine(char *text)
{
	if (NULL == m_infoFile)
	{
		char	name[MAX_QPATH];

		Com_sprintf (name, sizeof(name), "%s/pickups.log", gi.FS_Userdir());
		if (m_bInfoFileOpened)
		{	// file's already been started so append to it
			m_infoFile = fopen(name, "a");
		}
		else
		{	// file hasn't been created yet, so create one...overwrite any previous file
			m_infoFile = fopen(name, "w");
			m_bInfoFileOpened = true;
		}
		if (NULL == m_infoFile)
		{
			return;
		}
	}
	fprintf(m_infoFile, "%s", text);
	fflush(m_infoFile);		// force it to save every time
	fclose(m_infoFile);
	m_infoFile = NULL;
}

/****************************

  class CratePickupInstList

  tracks all of the pickups that are going to spring forth from a busted-open crate
 ****************************/

void CratePickupInstList::Create(char *name)
{
	edict_t			*t = NULL;

	if (name)
	{
		if (!Compare(name))
		{
			Clear();
			while ((t = G_Find (t, FOFS(targetname), name)))
			{
				Push(thePickupList.GetPickupInstFromEdict(t));
			}
		}
	}
}

/****************************


class PickupList 

	the queen mother list of items.

 ****************************/

PickupList::PickupList()
{
	// if you want to see a list of all available pickups, type the command listpickups at the console

															//spawnName						type		spawnParm			respawnTime	defaultCount	defaultMaxCount		index					worldName (FIXME)					pickupstringindex				ammopickupstringindex			itemfullstringindex				flags
	m_pickups[OBJ_NONE] =				new Pickup				();
	m_pickups[OBJ_AMMO_KNIFE] =			new AmmoPickup			("item_ammo_knife",				PU_AMMO,	AMMO_KNIFE,			30,			1,				-1,					OBJ_AMMO_KNIFE,			"Knife",							ITEMS_KNIFE_PICKUP,				ITEMS_KNIFE_PICKUP,				ITEMS_KNIFE_FULL,				AmmoPickup::ammoFlag_BULLET);
	m_pickups[OBJ_AMMO_PISTOL] =		new AmmoPickup			("item_ammo_pistol",			PU_AMMO,	AMMO_9MM,			30,			30,				-1,					OBJ_AMMO_PISTOL,		"9mm Ammo",							ITEMS_9MM_AMMO_PICKUP,			ITEMS_9MM_AMMO_PICKUP,			ITEMS_9MM_AMMO_FULL,			AmmoPickup::ammoFlag_BULLET);
	m_pickups[OBJ_AMMO_SHOTGUN] =		new AmmoPickup			("item_ammo_shotgun",			PU_AMMO,	AMMO_SHELLS,		30,			5,				-1,					OBJ_AMMO_SHOTGUN,		"12 gauge shells",					ITEMS_12_GAUGE_AMMO_PICKUP,		ITEMS_12_GAUGE_AMMO_PICKUP,		ITEMS_12_GAUGE_AMMO_FULL,		AmmoPickup::ammoFlag_BULLET);
	m_pickups[OBJ_AMMO_PISTOL2] =		new AmmoPickup			("item_ammo_pistol2",			PU_AMMO,	AMMO_44,			30,			5,				-1,					OBJ_AMMO_PISTOL2,		".44 Ammo",							ITEMS_44_AMMO_PICKUP,			ITEMS_44_AMMO_PICKUP,			ITEMS_44_AMMO_FULL,				AmmoPickup::ammoFlag_BULLET);
	m_pickups[OBJ_AMMO_AUTO] =			new AmmoPickup			("item_ammo_auto",				PU_AMMO,	AMMO_556,			30,			30,				-1,					OBJ_AMMO_AUTO,			"5.56 Ammo",						ITEMS_556_AMMO_PICKUP,			ITEMS_556_AMMO_PICKUP,			ITEMS_556_AMMO_FULL,			AmmoPickup::ammoFlag_BULLET);
	m_pickups[OBJ_AMMO_MPG] =			new AmmoPickup			("item_ammo_battery",			PU_AMMO,	AMMO_MWAVE,			30,			30,				-1,					OBJ_AMMO_MPG,			"Battery",							ITEMS_BATTERY_PICKUP,			ITEMS_BATTERY_PICKUP,			ITEMS_BATTERY_FULL);
	m_pickups[OBJ_AMMO_GAS] =			new AmmoPickup			("item_ammo_gas",				PU_AMMO,	AMMO_FTHROWER,		30,			30,				-1,					OBJ_AMMO_GAS,			"Flamethrower Gas",					ITEMS_GAS_PICKUP,				ITEMS_GAS_PICKUP,				ITEMS_GAS_FULL);
	m_pickups[OBJ_AMMO_ROCKET] =		new AmmoPickup			("item_ammo_rocket",			PU_AMMO,	AMMO_ROCKET,		30,			5,				-1,					OBJ_AMMO_ROCKET,		"NM-22 Rockets",					ITEMS_ROCKET_AMMO_PICKUP,		ITEMS_ROCKET_AMMO_PICKUP,		ITEMS_ROCKET_AMMO_FULL);
	m_pickups[OBJ_AMMO_SLUG] =			new AmmoPickup			("item_ammo_slug",				PU_AMMO,	AMMO_SLUG,			30,			10,				-1,					OBJ_AMMO_SLUG,			"Slugs",							ITEMS_SLUGS_PICKUP,				ITEMS_SLUGS_PICKUP,				ITEMS_SLUGS_FULL);

	m_pickups[OBJ_AMMO_SINGLE_PISTOL] = new AmmoPickup			("item_ammo_sp_pistol",			PU_AMMO,	AMMO_9MM,			30,			30,				-1,					OBJ_AMMO_SINGLE_PISTOL,	"9mm Ammo",							ITEMS_9MM_AMMO_PICKUP,			ITEMS_9MM_AMMO_PICKUP,			ITEMS_9MM_AMMO_FULL,			AmmoPickup::ammoFlag_BULLET);
	m_pickups[OBJ_AMMO_SINGLE_SHOTGUN]= new AmmoPickup			("item_ammo_sp_shotgun",		PU_AMMO,	AMMO_SHELLS,		30,			5,				-1,					OBJ_AMMO_SINGLE_SHOTGUN,"12 gauge shells",					ITEMS_12_GAUGE_AMMO_PICKUP,		ITEMS_12_GAUGE_AMMO_PICKUP,		ITEMS_12_GAUGE_AMMO_FULL,		AmmoPickup::ammoFlag_BULLET);
	m_pickups[OBJ_AMMO_SINGLE_PISTOL2] =new AmmoPickup			("item_ammo_sp_pistol2",		PU_AMMO,	AMMO_44,			30,			5,				-1,					OBJ_AMMO_SINGLE_PISTOL2,".44 Ammo",							ITEMS_44_AMMO_PICKUP,			ITEMS_44_AMMO_PICKUP,			ITEMS_44_AMMO_FULL,				AmmoPickup::ammoFlag_BULLET);
	m_pickups[OBJ_AMMO_SINGLE_AUTO]=	new AmmoPickup			("item_ammo_sp_auto",			PU_AMMO,	AMMO_556,			30,			30,				-1,					OBJ_AMMO_SINGLE_AUTO,	"5.56 Ammo",						ITEMS_556_AMMO_PICKUP,			ITEMS_556_AMMO_PICKUP,			ITEMS_556_AMMO_FULL,			AmmoPickup::ammoFlag_BULLET);
	m_pickups[OBJ_AMMO_SINGLE_MPG] =	new AmmoPickup			("item_ammo_sp_battery",		PU_AMMO,	AMMO_MWAVE,			30,			30,				-1,					OBJ_AMMO_SINGLE_MPG,	"Battery",							ITEMS_BATTERY_PICKUP,			ITEMS_BATTERY_PICKUP,			ITEMS_BATTERY_FULL);
	m_pickups[OBJ_AMMO_SINGLE_GAS] =	new AmmoPickup			("item_ammo_sp_gas",			PU_AMMO,	AMMO_FTHROWER,		30,			30,				-1,					OBJ_AMMO_SINGLE_GAS,	"Flamethrower Gas",					ITEMS_GAS_PICKUP,				ITEMS_GAS_PICKUP,				ITEMS_GAS_FULL);
	m_pickups[OBJ_AMMO_SINGLE_ROCKET] = new AmmoPickup			("item_ammo_sp_rocket",			PU_AMMO,	AMMO_ROCKET,		30,			5,				-1,					OBJ_AMMO_SINGLE_ROCKET,	"NM-22 Rockets",					ITEMS_ROCKET_AMMO_PICKUP,		ITEMS_ROCKET_AMMO_PICKUP,		ITEMS_ROCKET_AMMO_FULL);
	m_pickups[OBJ_AMMO_SINGLE_SLUG]=	new AmmoPickup			("item_ammo_sp_slug",			PU_AMMO,	AMMO_SLUG,			30,			10,				-1,					OBJ_AMMO_SINGLE_SLUG,	"Slugs",							ITEMS_SLUGS_PICKUP,				ITEMS_SLUGS_PICKUP,				ITEMS_SLUGS_FULL);

	m_pickups[OBJ_WPN_PISTOL2] =		new WeaponPickup		("item_weapon_pistol2",			PU_WEAPON,	SFW_PISTOL2,		30,			0,				-1,					OBJ_WPN_PISTOL2,		".44 Pistol",						ITEMS_44_PISTOL_PICKUP,			ITEMS_44_AMMO_PICKUP,			ITEMS_44_AMMO_FULL,				WeaponPickup::wpnFlag_BULLET);
	m_pickups[OBJ_WPN_PISTOL1] =		new WeaponPickup		("item_weapon_pistol1",			PU_WEAPON,	SFW_PISTOL1,		30,			0,				-1,					OBJ_WPN_PISTOL1,		"9mm Pistol",						ITEMS_9MM_PISTOL_PICKUP,		ITEMS_9MM_AMMO_PICKUP,			ITEMS_9MM_AMMO_FULL,			WeaponPickup::wpnFlag_BULLET);
	m_pickups[OBJ_WPN_MACHINEPISTOL] =	new WeaponPickup		("item_weapon_machinepistol",	PU_WEAPON,	SFW_MACHINEPISTOL,	30,			0,				-1,					OBJ_WPN_MACHINEPISTOL,	"9mm Suppressed SMG",				ITEMS_9MM_SMG_PICKUP,			ITEMS_9MM_AMMO_PICKUP,			ITEMS_9MM_AMMO_FULL,			WeaponPickup::wpnFlag_BULLET);
	m_pickups[OBJ_WPN_ASSAULTRIFLE] =	new WeaponPickup		("item_weapon_assault_rifle",	PU_WEAPON,	SFW_ASSAULTRIFLE,	30,			0,				-1,					OBJ_WPN_ASSAULTRIFLE,	"5.56 SMG",							ITEMS_556_SMG_PICKUP,			ITEMS_556_AMMO_PICKUP,			ITEMS_556_AMMO_FULL,			WeaponPickup::wpnFlag_BULLET);
	m_pickups[OBJ_WPN_SNIPER] =			new WeaponPickup		("item_weapon_sniper_rifle",	PU_WEAPON,	SFW_SNIPER,			30,			0,				-1,					OBJ_WPN_SNIPER,			"5.56 Sniper Rifle",				ITEMS_556_SNIPER_PICKUP,		ITEMS_556_AMMO_PICKUP,			ITEMS_556_AMMO_FULL,			WeaponPickup::wpnFlag_BULLET);
	m_pickups[OBJ_WPN_AUTOSHOTGUN] =	new WeaponPickup		("item_weapon_autoshotgun",		PU_WEAPON,	SFW_AUTOSHOTGUN,	60,			0,				-1,					OBJ_WPN_AUTOSHOTGUN,	"Repeating Slug Thrower",			ITEMS_SLUGTHROWER_PICKUP,		ITEMS_SLUGS_PICKUP,				ITEMS_SLUGS_FULL);
	m_pickups[OBJ_WPN_SHOTGUN] =		new WeaponPickup		("item_weapon_shotgun",			PU_WEAPON,	SFW_SHOTGUN,		30,			0,				-1,					OBJ_WPN_SHOTGUN,		"12 Gauge Shotgun",					ITEMS_12_GAUGE_SHOTGUN_PICKUP,	ITEMS_12_GAUGE_AMMO_PICKUP,		ITEMS_12_GAUGE_AMMO_FULL,		WeaponPickup::wpnFlag_BULLET);
	m_pickups[OBJ_WPN_MACHINEGUN] =		new WeaponPickup		("item_weapon_machinegun",		PU_WEAPON,	SFW_MACHINEGUN,		30,			0,				-1,					OBJ_WPN_MACHINEGUN,		"Heavy Machinegun",					ITEMS_HEAVY_MACHINEGUN_PICKUP,	ITEMS_556_AMMO_PICKUP,			ITEMS_556_AMMO_FULL,			WeaponPickup::wpnFlag_BULLET);
	m_pickups[OBJ_WPN_ROCKET] =			new WeaponPickup		("item_weapon_rocketlauncher",	PU_WEAPON,	SFW_ROCKET,			75,			0,				-1,					OBJ_WPN_ROCKET,			"M-1943 Rocket Launcher",			ITEMS_ROCKET_LAUNCHER_PICKUP,	ITEMS_ROCKET_AMMO_PICKUP,		ITEMS_ROCKET_AMMO_FULL);
	m_pickups[OBJ_WPN_MICROWAVE] =		new WeaponPickup		("item_weapon_microwavepulse",	PU_WEAPON,	SFW_MICROWAVEPULSE,	90,			0,				-1,					OBJ_WPN_MICROWAVE,		"Microwave Pulse",					ITEMS_MICROWAVE_PULSE_PICKUP,	ITEMS_BATTERY_PICKUP,			ITEMS_BATTERY_FULL);
	m_pickups[OBJ_WPN_FLAMEGUN] =		new WeaponPickup		("item_weapon_flamethrower",	PU_WEAPON,	SFW_FLAMEGUN,		60,			0,				-1,					OBJ_WPN_FLAMEGUN,		"L-32 Flame Gun",					ITEMS_FLAME_GUN_PICKUP,			ITEMS_GAS_PICKUP,				ITEMS_GAS_FULL);
	m_pickups[OBJ_ARMOR] =				new Pickup				("item_equip_armor",			PU_ARMOR,	0,					30,			50,				-1,					OBJ_ARMOR,				"Armor",							ITEMS_ARMOR_PICKUP,				ITEMS_ARMOR_PICKUP,				ITEMS_ARMOR_ALREADY_FULL);
	m_pickups[OBJ_ITM_FLASHPACK] =		new Pickup				("item_equip_flashpack",		PU_INV,		SFE_FLASHPACK,		30,			1,				6,					OBJ_ITM_FLASHPACK,		"Flashpack",						ITEMS_FLASHPACK_PICKUP,			ITEMS_FLASHPACK_PICKUP,			ITEMS_FLASHPACK_FULL);
//	m_pickups[OBJ_ITM_NEURALGRENADE] =	new Pickup				("item_equip_neural_grenade",	PU_INV,		SFE_NEURAL_GRENADE,	30,			1,				-1,					OBJ_ITM_NEURALGRENADE,	"Neural Pulse Grenade",				ITEMS_NEURALGRENADE_PICKUP,		ITEMS_NEURALGRENADE_PICKUP,		ITEMS_NEURALGRENADE_FULL);
	m_pickups[OBJ_ITM_C4] =				new Pickup				("item_equip_c4",				PU_INV,		SFE_C4,				30,			1,				4,					OBJ_ITM_C4,				"C4",								ITEMS_C4_PICKUP,				ITEMS_C4_PICKUP,				ITEMS_C4_FULL);
	m_pickups[OBJ_ITM_LIGHTGOGGLES] =	new LightGogglesPickup	("item_equip_light_goggles",	PU_INV,		SFE_LIGHT_GOGGLES,	30,			100,			100,				OBJ_ITM_LIGHTGOGGLES,	"Light Amplification Goggles",		ITEMS_LIGHTGOGGLES_PICKUP,		ITEMS_LIGHTGOGGLES_PICKUP,		ITEMS_LIGHTGOGGLES_FULL);
	m_pickups[OBJ_ITM_MINE] =			new Pickup				("item_equip_claymore",			PU_INV,		SFE_CLAYMORE,		30,			1,				-1,					OBJ_ITM_MINE,			"Mine",								ITEMS_CLAYMORE_PICKUP,			ITEMS_CLAYMORE_PICKUP,			ITEMS_CLAYMORE_FULL);
	m_pickups[OBJ_ITM_MEDKIT] =			new MedKitPickup		("item_equip_medkit",			PU_INV,		SFE_MEDKIT,			30,			1,			 	2,					OBJ_ITM_MEDKIT,			"MedKit",							ITEMS_MEDKIT_PICKUP,			ITEMS_MEDKIT_PICKUP,			ITEMS_MEDKIT_FULL);
	m_pickups[OBJ_ITM_GRENADE] =		new Pickup				("item_equip_grenade",			PU_INV,		SFE_GRENADE,		30,			1,				6,					OBJ_ITM_GRENADE,		"Grenade",							ITEMS_GRENADE_PICKUP,			ITEMS_GRENADE_PICKUP,			ITEMS_GRENADE_FULL);
	m_pickups[OBJ_CTF_FLAG] =			new Pickup				("item_ctf_flag",				PU_INV,		SFE_CTFFLAG,		30,			1,				-1,					OBJ_CTF_FLAG,			"Flag",								ITEMS_CTF_FLAG_PICKUP,			ITEMS_CTF_FLAG_PICKUP_2,		ITEMS_CTF_FLAG_PICKUP);
	m_pickups[OBJ_HLTH_SM] =			new Pickup				("item_health_small",			PU_HEALTH,	SFH_SMALL_HEALTH,	30,			25,				-1,					OBJ_HLTH_SM,			"Small Health",						ITEMS_HEALTH_PICKUP,			ITEMS_HEALTH_PICKUP,			ITEMS_HEALTH_PICKUP);
	m_pickups[OBJ_HLTH_LRG] =			new Pickup				("item_health_large",			PU_HEALTH,	SFH_LARGE_HEALTH,	30,			50,				-1,					OBJ_HLTH_LRG,			"Large Health",						ITEMS_HEALTH_PICKUP,			ITEMS_HEALTH_PICKUP,			ITEMS_HEALTH_PICKUP);

	m_pickupInfoList = NULL;

	m_bDisplayedWeaponArenaHelp = false;
}

PickupList::~PickupList()
{
	list<PickupInst*>::iterator	it;

	for (int i = 0; i < MAX_ITM_OBJS; i++)
	{
		delete m_pickups[i];
		m_pickups[i] = NULL;
	}

	for (it = m_pickupInsts.begin(); it != m_pickupInsts.end(); it++)
	{
		delete (*it);
	}
	m_pickupInsts.clear();

	delete m_pickupInfoList;
	m_pickupInfoList = NULL;
}

// set up the modelSpawnData for all of our pickups.
int PickupList::PreSpawnInit()
{
	int retVal = true;

	for (int i = 0; i < MAX_ITM_OBJS; i++)
	{
		if (m_pickups[i]->Init())
		{
			// there's some per-model special stuff that has to happen before we can use each item...setting up
			//a skin, making some bbox modifications, etc.
			SetPickupSkin(i);
			ModifyPickupBBox(i);
			SetPickupScale(i);
			SetPickupLowPolyModel(i);
			SetGSQ(i);
			SetDroppedStringIndex(i);
		}
		else	
		{// something failed in the init'ing of this item's modelSpawnData. file missing?
			retVal = false;
		}
	}

	m_pickupInsts.clear();

	if (pickupinfo->value)
	{
		m_pickupInfoList = new PickupInfoList;
	}

	return retVal;
}

int PickupList::PostSpawnInit()
{
	list<PickupInst*>::iterator	it;
	edict_t						*ent = NULL;

	// for starters, check if any pickups are inside of crates

	for (it = m_pickupInsts.begin(); it != m_pickupInsts.end(); it++)
	{
		if ( (ent = (*it)->GetEdict()) &&
			 (ent->spawnflags & SF_PICKUP_START_CRATED) )
		{
			InitCrated(ent);
		}
	}

	return true;
}

void PickupList::Destroy()
{
	// kef -- see if we want to dump our pickup list to a file
	if (pickupinfo->value > 1)
	{
		DumpList(PickupList::print_FILE);
	}
	delete m_pickupInfoList;
	m_pickupInfoList = NULL;

	m_crateList.Destroy();

	// DO NOT call this before the above DumpList.
	UnregisterAll();
}

void PickupList::WriteLine(int printDest, char *outLine, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, outLine);
	vsprintf (text, outLine, argptr);
	va_end (argptr);

	if (print_CONSOLE == printDest)
	{
		Com_Printf(text);
	}
	else if (print_FILE == printDest)
	{
		if (m_pickupInfoList)
		{
			m_pickupInfoList->WriteLine(text);
		}
	}
}

// output the contents of m_pickups to the console. output version, too?
void PickupList::DumpList(int printDest/*print_CONSOLE*/)
{
	list<PickupInst*>::iterator	it;
	int							i = -1;
	edict_t						*ent = NULL;
	Pickup						*pickup = NULL;
	char						buf[256], buf2[32];
	int							nClassname = 6,
								nIndex = 32,
								nCount = 39,
								nLocation = 46,
								nCurPos = 0;
	int							gametime = (int)(level.time - level.startTime);
	int							gamehours=0, gameminutes=0, gameseconds=0;

	gamehours = (gametime)/(int)3600;
	gameminutes = ((gametime) - (gamehours*3600))/(int)60;
	gameseconds = (gametime) - (gamehours*3600 + gameminutes*60);
	memset(buf, 0, 64);
	WriteLine(printDest, "//\n// begin pickup list -- map:%s, elapsed gametime: %d:%s%d:%s%d\n//\n", level.mapname,
		gamehours, (gameminutes<10?"0":""), gameminutes, (gameseconds<10?"0":""), gameseconds);
	WriteLine(printDest,"#     classname                 index  count  location\n\n");
	if (m_pickupInfoList)
	{	// output info for each pickupinst
		WriteLine(printDest, "                                                                 type of   # of times  average  average  # of times  average\n");
		WriteLine(printDest, "                                                                 creation   picked up   pickup  respawn   touched     touch \n");
		WriteLine(printDest, "                                                                                        delay   pickup                delay \n\n");
	}
	for (it = m_pickupInsts.begin(), i = 0; it != m_pickupInsts.end(); it++, i++)
	{
		ent = (*it)->GetEdict();
		pickup = (*it)->GetPickup();
		// print the number
		strcpy(buf, itoa(i, buf2, 10));
		strcat(buf, ":"); // add 1 for the :
		nCurPos = strlen(buf2)+1;
		// get to the position for printing the classname
		while (nCurPos < nClassname)
		{
			strcat(buf, " ");
			nCurPos++;
		}
		// print the classname
		strncat(buf, ent->classname, 32);
		nCurPos += strlen(ent->classname);
		// get to the position for printing the index
		while (nCurPos < nIndex)
		{
			strcat(buf, " ");
			nCurPos++;
		}
		// print the pickup index
		strcat(buf, itoa(pickup->GetPickupListIndex(), buf2, 10));
		nCurPos += strlen(buf2);
		// get to the position for printing the count
		while (nCurPos < nCount)
		{
			strcat(buf, " ");
			nCurPos++;
		}
		// print the count for the object
		strcat(buf, itoa(ent->count, buf2, 10));
		nCurPos += strlen(buf2);
		// get to the position for printing the location
		while (nCurPos < nLocation)
		{
			strcat(buf, " ");
			nCurPos++;
		}
		strcpy(buf2, vtos(ent->s.origin));
		strcat(buf, buf2);
		nCurPos += strlen(buf2);
		
		if (m_pickupInfoList)
		{	// output the info for this pickupinst
			int			nInfo0 = nLocation + 23,
						nInfo1 = nInfo0 + 10,
						nInfo2 = nInfo1 + 11,
						nInfo3 = nInfo2 + 9,
						nInfo4 = nInfo3 + 9,
						nInfo5 = nInfo4 + 12;
			

			// get to the position for printing the creation type
			while (nCurPos < nInfo0)
			{
				strcat(buf, " ");
				nCurPos++;
			}
			// print the pickup count
			if (m_pickupInfoList->GetType(*it) == PickupInstInfo::type_LOADTIME)
			{
				strcat(buf, "Design");
				nCurPos += strlen("Design");
			}
			else
			{
				strcat(buf, "Dropped");
				nCurPos += strlen("Dropped");
			}

			// get to the position for printing the pickup count
			while (nCurPos < nInfo1)
			{
				strcat(buf, " ");
				nCurPos++;
			}
			// print the pickup count
			strcat(buf, itoa(m_pickupInfoList->GetPickedUpCount(*it), buf2, 10));
			nCurPos += strlen(buf2);

			// get to the position for printing the avg pickup delay
			while (nCurPos < nInfo2)
			{
				strcat(buf, " ");
				nCurPos++;
			}
			// print the pickup count
			strcat(buf, itoa(m_pickupInfoList->GetAvgPickedUpDelay(*it), buf2, 10));
			nCurPos += strlen(buf2);

			// get to the position for printing the avg respawn-pickup delay
			while (nCurPos < nInfo3)
			{
				strcat(buf, " ");
				nCurPos++;
			}
			// print the pickup count
			strcat(buf, itoa(m_pickupInfoList->GetAvgRespawnPickUpDelay(*it), buf2, 10));
			nCurPos += strlen(buf2);

			// get to the position for printing the number of times touched
			while (nCurPos < nInfo4)
			{
				strcat(buf, " ");
				nCurPos++;
			}
			// print the pickup count
			strcat(buf, itoa(m_pickupInfoList->GetTouchCount(*it), buf2, 10));
			nCurPos += strlen(buf2);

			// get to the position for printing the number of avg touch delay
			while (nCurPos < nInfo5)
			{
				strcat(buf, " ");
				nCurPos++;
			}
			// print the pickup count
			strcat(buf, itoa(m_pickupInfoList->GetAvgTouchDelay(*it), buf2, 10));
			nCurPos += strlen(buf2);
		}
		WriteLine(printDest, "%s\n", buf);
	}
	WriteLine(printDest, "//\n// end pickup list\n//\n");
}

// called every frame, this fn polls pickups for removal of used callbacks. G_FreeEdict() calls our Unregister fn
//for every PickupInst getting freed 
int PickupList::FrameUpdate()
{
	list<PickupInst*>::iterator	it;

	for (it = m_pickupInsts.begin(); it != m_pickupInsts.end(); it++)
	{
		switch((*it)->GetFadeStatus())
		{
		case PickupInst::fading_REMOVECALLBACK:
			{	// this inst has finished fading in so remove its callback.
				(*it)->RemoveCallBack();
				HandleInfoEvent(PickupInfoList::event_RESPAWN, (*it)->GetEdict());
				break;
			}
		default:
			break;
		}
	}
	return true;
}

float PickupList::WeaponArena(float cvarVal)
{
	return cvarVal;
}

void PickupList::WeaponArenaHelp()
{
	if (!m_bDisplayedWeaponArenaHelp)
	{
		m_bDisplayedWeaponArenaHelp = true;
		Com_Printf("*\n* weaponarena usage\n*\n");
		for (int i = OBJ_NONE; i < MAX_ITM_OBJS; i++)
		{
			Com_Printf("%d: %s\n", i, m_pickups[i]->GetSpawnName());
		}
		Com_Printf("*\n* end weaponarena usage\n*\n\n");
	}
}

// some pickups, like ammo boxes, use the same model but a different skin
void PickupList::SetPickupSkin(int listIndex)
{
	switch(listIndex)
	{
	case OBJ_NONE:
		break;
	case OBJ_AMMO_KNIFE:
		break;
	case OBJ_AMMO_PISTOL:
		m_pickups[listIndex]->SetSkin("pistolbox");
		break;
	case OBJ_AMMO_SHOTGUN:
		m_pickups[listIndex]->SetSkin("shotgunbox");
		break;
	case OBJ_AMMO_PISTOL2:
		m_pickups[listIndex]->SetSkin("pistol2box");
		break;
	case OBJ_AMMO_AUTO:
		m_pickups[listIndex]->SetSkin("autobox");
		break;
	case OBJ_AMMO_MPG:
		m_pickups[listIndex]->SetSkin("batterybox");
		break;
	case OBJ_AMMO_GAS:
		m_pickups[listIndex]->SetSkin("gasbox");
		break;
	case OBJ_AMMO_ROCKET:
		m_pickups[listIndex]->SetSkin("rocketbox");
		break;
	case OBJ_AMMO_SLUG:
		m_pickups[listIndex]->SetSkin("slugbox");
		break;

	case OBJ_AMMO_SINGLE_PISTOL:
		m_pickups[listIndex]->SetSkin("ammo_9mm_single");
		break;
	case OBJ_AMMO_SINGLE_SHOTGUN:
		m_pickups[listIndex]->SetSkin("ammo_00_single");
		break;
	case OBJ_AMMO_SINGLE_PISTOL2:
		m_pickups[listIndex]->SetSkin("ammo_44_single");
		break;
	case OBJ_AMMO_SINGLE_AUTO:
		m_pickups[listIndex]->SetSkin("ammo_556_single");
		break;
	case OBJ_AMMO_SINGLE_MPG:
		m_pickups[listIndex]->SetSkin("ammo_bat");
		break;
	case OBJ_AMMO_SINGLE_GAS:
		m_pickups[listIndex]->SetSkin("ammo_gas_single");
		break;
	case OBJ_AMMO_SINGLE_ROCKET:
		m_pickups[listIndex]->SetSkin("ammo_rocket_single");
		break;
	case OBJ_AMMO_SINGLE_SLUG:
		m_pickups[listIndex]->SetSkin("ammo_slug_single");
		break;

	case OBJ_WPN_PISTOL2:
		break;
	case OBJ_WPN_PISTOL1:
		break;
	case OBJ_WPN_MACHINEPISTOL:
		break;
	case OBJ_WPN_ASSAULTRIFLE:
		break;
	case OBJ_WPN_SNIPER:
		break;
	case OBJ_WPN_AUTOSHOTGUN:
		break;
	case OBJ_WPN_SHOTGUN:
		break;
	case OBJ_WPN_MACHINEGUN:
		break;
	case OBJ_WPN_ROCKET:
		break;
	case OBJ_WPN_MICROWAVE:
		break;
	case OBJ_WPN_FLAMEGUN:
		break;
	case OBJ_ARMOR:
		break;
	case OBJ_ITM_FLASHPACK:
		break;
//	case OBJ_ITM_NEURALGRENADE:
//		break;
	case OBJ_ITM_C4:
		break;
	case OBJ_ITM_LIGHTGOGGLES:
		break;
	case OBJ_ITM_MINE:
		break;
	case OBJ_ITM_MEDKIT:
		break;
	case OBJ_ITM_GRENADE:
		break;
	case OBJ_CTF_FLAG:
		break;
	case OBJ_HLTH_SM:
		break;
	case OBJ_HLTH_LRG:
		break;
	}
}

// some pickups need help laying down on flat surfaces
void PickupList::ModifyPickupBBox(int listIndex)
{
	vec3_t adjMins = {0,0,0};

	switch(listIndex)
	{
	case OBJ_AMMO_PISTOL:
	case OBJ_AMMO_SHOTGUN:
	case OBJ_AMMO_PISTOL2:
	case OBJ_AMMO_AUTO:
	case OBJ_AMMO_MPG:
	case OBJ_AMMO_GAS:
	case OBJ_AMMO_ROCKET:
	case OBJ_AMMO_SLUG:
		adjMins[2] = -6;
		break;

	case OBJ_AMMO_SINGLE_PISTOL:
		adjMins[2] = 0;
		break;
	case OBJ_AMMO_SINGLE_SHOTGUN:
		adjMins[2] = 1;
		break;
	case OBJ_AMMO_SINGLE_PISTOL2:
		break;
	case OBJ_AMMO_SINGLE_AUTO:
		break;
	case OBJ_AMMO_SINGLE_MPG:
		break;
	case OBJ_AMMO_SINGLE_GAS:
		break;
	case OBJ_AMMO_SINGLE_ROCKET:
		adjMins[2] = -0.5;
		break;
	case OBJ_AMMO_SINGLE_SLUG:
		adjMins[2] = 1;
		break;

	case OBJ_WPN_PISTOL2:
	case OBJ_WPN_PISTOL1:
		adjMins[2] = -1.5;
		break;
	case OBJ_WPN_MACHINEPISTOL:
		adjMins[2] = -3.0;
		break;
	case OBJ_WPN_ASSAULTRIFLE:
		adjMins[2] = -2.0;
		break;
	case OBJ_WPN_SNIPER:
		adjMins[2] = -2.0;
		break;
	case OBJ_WPN_AUTOSHOTGUN:
		adjMins[2] = -8.5;
		break;
	case OBJ_WPN_SHOTGUN:
		adjMins[2] = -3.0;
		break;
	case OBJ_WPN_MACHINEGUN:
		adjMins[2] = -3.0;
		break;
	case OBJ_WPN_ROCKET:
		adjMins[2] = -10.5;
		break;
	case OBJ_WPN_MICROWAVE:
		adjMins[2] = -8.0;
		break;
	case OBJ_WPN_FLAMEGUN:
		adjMins[2] = -2.5;
		break;

	case OBJ_ARMOR:
		break;

	case OBJ_ITM_FLASHPACK:
		adjMins[2] = -9;
		break;
//	case OBJ_ITM_NEURALGRENADE: // 12/28/99 who knows? we don't have a model yet.
//		break;
	case OBJ_ITM_C4:
		adjMins[2] = -5.0;
		break;
	case OBJ_ITM_LIGHTGOGGLES:
		adjMins[2] = -4.5;
		break;
	case OBJ_ITM_MINE:
		adjMins[2] = -5.0;
		break;
	case OBJ_ITM_MEDKIT:
		adjMins[2] = 0.5;
		break;
	case OBJ_ITM_GRENADE:
		adjMins[2] = 0;
		break;

	case OBJ_CTF_FLAG:
		break;

	case OBJ_HLTH_SM:
		break;
	case OBJ_HLTH_LRG:
		break;
	}
	m_pickups[listIndex]->SetBBoxAdjustMins(adjMins);
}

// a lot of pickups get scaled differently
void PickupList::SetPickupScale(int listIndex)
{
	switch(listIndex)
	{
	case OBJ_NONE:
		break;
	case OBJ_AMMO_KNIFE:
		// knives use the default scale of 1 because we don't want them growing when someone dies with
		//knives stuck in them
		break;
	case OBJ_AMMO_PISTOL:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_AMMO_SHOTGUN:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_AMMO_PISTOL2:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_AMMO_AUTO:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_AMMO_MPG:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_AMMO_GAS:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_AMMO_ROCKET:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_AMMO_SLUG:
		m_pickups[listIndex]->SetScale(1.75);
		break;

	case OBJ_AMMO_SINGLE_PISTOL:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_AMMO_SINGLE_SHOTGUN:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_AMMO_SINGLE_PISTOL2:
		m_pickups[listIndex]->SetScale(2.00);
		break;
	case OBJ_AMMO_SINGLE_AUTO:
		m_pickups[listIndex]->SetScale(2.00);
		break;
	case OBJ_AMMO_SINGLE_MPG:
		m_pickups[listIndex]->SetScale(2.00);
		break;
	case OBJ_AMMO_SINGLE_GAS:
		m_pickups[listIndex]->SetScale(2.00);
		break;
	case OBJ_AMMO_SINGLE_ROCKET:
		m_pickups[listIndex]->SetScale(2.00);
		break;
	case OBJ_AMMO_SINGLE_SLUG:
		m_pickups[listIndex]->SetScale(2.00); // need to be bigger than OBJ_AMMO_SINGLE_SHOTGUN
		break;

	case OBJ_WPN_PISTOL2:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_WPN_PISTOL1:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_WPN_MACHINEPISTOL:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_WPN_ASSAULTRIFLE:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_WPN_SNIPER:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_WPN_AUTOSHOTGUN:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_WPN_SHOTGUN:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_WPN_MACHINEGUN:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_WPN_ROCKET:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_WPN_MICROWAVE:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_WPN_FLAMEGUN:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_ARMOR:
		m_pickups[listIndex]->SetScale(1.25);
		break;
	case OBJ_ITM_FLASHPACK:
		m_pickups[listIndex]->SetScale(1.75);
		break;
//	case OBJ_ITM_NEURALGRENADE:
//		m_pickups[listIndex]->SetScale(1.75);
//		break;
	case OBJ_ITM_C4:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_ITM_LIGHTGOGGLES:
		m_pickups[listIndex]->SetScale(2.0);
		break;
	case OBJ_ITM_MINE:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_ITM_MEDKIT:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_ITM_GRENADE:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_CTF_FLAG:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_HLTH_SM:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	case OBJ_HLTH_LRG:
		m_pickups[listIndex]->SetScale(1.75);
		break;
	}
}

// currently, only needed for armor, which uses a lower poly model in deathmatch than in single player.
void PickupList::SetPickupLowPolyModel(int listIndex)
{
	switch(listIndex)
	{
	case OBJ_NONE:
		break;
	case OBJ_AMMO_KNIFE:
		break;
	case OBJ_AMMO_PISTOL:
		break;
	case OBJ_AMMO_SHOTGUN:
		break;
	case OBJ_AMMO_PISTOL2:
		break;
	case OBJ_AMMO_AUTO:
		break;
	case OBJ_AMMO_MPG:
		break;
	case OBJ_AMMO_GAS:
		break;
	case OBJ_AMMO_ROCKET:
		break;
	case OBJ_AMMO_SLUG:
		break;
	case OBJ_AMMO_SINGLE_PISTOL:
		break;
	case OBJ_AMMO_SINGLE_SHOTGUN:
		break;
	case OBJ_WPN_PISTOL2:
		break;
	case OBJ_WPN_PISTOL1:
		break;
	case OBJ_WPN_MACHINEPISTOL:
		break;
	case OBJ_WPN_ASSAULTRIFLE:
		break;
	case OBJ_WPN_SNIPER:
		break;
	case OBJ_WPN_AUTOSHOTGUN:
		break;
	case OBJ_WPN_SHOTGUN:
		break;
	case OBJ_WPN_MACHINEGUN:
		break;
	case OBJ_WPN_ROCKET:
		break;
	case OBJ_WPN_MICROWAVE:
		break;
	case OBJ_WPN_FLAMEGUN:
		break;
	case OBJ_ARMOR:
		m_pickups[listIndex]->SetLowPolyModel(&lowPolyArmorModel);
		break;
	case OBJ_ITM_FLASHPACK:
		break;
//	case OBJ_ITM_NEURALGRENADE:
//		break;
	case OBJ_ITM_C4:
		break;
	case OBJ_ITM_LIGHTGOGGLES:
		break;
	case OBJ_ITM_MINE:
		break;
	case OBJ_ITM_MEDKIT:
		break;
	case OBJ_ITM_GRENADE:
		break;
	case OBJ_CTF_FLAG:
		break;
	case OBJ_HLTH_SM:
		break;
	case OBJ_HLTH_LRG:
		break;
	}
}

void PickupList::SetGSQ(int listIndex)
{
	switch(listIndex)
	{
	case OBJ_NONE:
		break;
	case OBJ_AMMO_KNIFE:
		break;
	case OBJ_AMMO_PISTOL:
	case OBJ_AMMO_SHOTGUN:
	case OBJ_AMMO_PISTOL2:
	case OBJ_AMMO_AUTO:
	case OBJ_AMMO_MPG:
	case OBJ_AMMO_GAS:
	case OBJ_AMMO_ROCKET:
	case OBJ_AMMO_SLUG:
		m_pickups[listIndex]->SetGSQ("box");
		m_pickups[listIndex]->SetFadeInSeq("fade_box");
		break;
	case OBJ_AMMO_SINGLE_PISTOL:
		break;
	case OBJ_AMMO_SINGLE_SHOTGUN:
		break;
	case OBJ_AMMO_SINGLE_AUTO:
		m_pickups[listIndex]->SetGSQ("ammo_556");
		m_pickups[listIndex]->SetFadeInSeq("fade_ammo_556");
		break;
	case OBJ_AMMO_SINGLE_ROCKET:
		m_pickups[listIndex]->SetGSQ("ammo_rocket");
		m_pickups[listIndex]->SetFadeInSeq("fade_ammo_rocket");
		break;
	case OBJ_WPN_PISTOL2:
		m_pickups[listIndex]->SetGSQ("pistol2");
		m_pickups[listIndex]->SetFadeInSeq("fade_pistol2");
		break;
	case OBJ_WPN_PISTOL1:
		m_pickups[listIndex]->SetGSQ("pistol1");
		m_pickups[listIndex]->SetFadeInSeq("fade_pistol1");
		break;
	case OBJ_WPN_MACHINEPISTOL:
		m_pickups[listIndex]->SetGSQ("machinepistol");
		m_pickups[listIndex]->SetFadeInSeq("fade_machinepistol");
		break;
	case OBJ_WPN_ASSAULTRIFLE:
		m_pickups[listIndex]->SetGSQ("assault_rifle");
		m_pickups[listIndex]->SetFadeInSeq("fade_assault_rifle");
		break;
	case OBJ_WPN_SNIPER:
		m_pickups[listIndex]->SetGSQ("sniperrifle");
		m_pickups[listIndex]->SetFadeInSeq("fade_sniperrifle");
		break;
	case OBJ_WPN_AUTOSHOTGUN:
		m_pickups[listIndex]->SetGSQ("autoshotgun");
		m_pickups[listIndex]->SetFadeInSeq("fade_autoshotgun");
		break;
	case OBJ_WPN_SHOTGUN:
		m_pickups[listIndex]->SetGSQ("shotgun");
		m_pickups[listIndex]->SetFadeInSeq("fade_shotgun");
		break;
	case OBJ_WPN_MACHINEGUN:
		m_pickups[listIndex]->SetGSQ("machinegun");
		m_pickups[listIndex]->SetFadeInSeq("fade_machinegun");
		break;
	case OBJ_WPN_ROCKET:
		m_pickups[listIndex]->SetGSQ("rocket");
		m_pickups[listIndex]->SetFadeInSeq("fade_rocket");
		break;
	case OBJ_WPN_MICROWAVE:
		m_pickups[listIndex]->SetGSQ("mpg");
		m_pickups[listIndex]->SetFadeInSeq("fade_mpg");
		break;
	case OBJ_WPN_FLAMEGUN:
		m_pickups[listIndex]->SetGSQ("flamethrower");
		m_pickups[listIndex]->SetFadeInSeq("fade_flamethrower");
		break;
	case OBJ_ARMOR:
		m_pickups[listIndex]->SetGSQ("armor");
		m_pickups[listIndex]->SetFadeInSeq("armorfade");
		break;
	case OBJ_ITM_FLASHPACK:
		m_pickups[listIndex]->SetGSQ("flashpak");
		m_pickups[listIndex]->SetFadeInSeq("fade_flashpak");
		break;
//	case OBJ_ITM_NEURALGRENADE:
//		break;
	case OBJ_ITM_C4:
		m_pickups[listIndex]->SetGSQ("c4");
		m_pickups[listIndex]->SetFadeInSeq("fade_c4");
		break;
	case OBJ_ITM_LIGHTGOGGLES:
		m_pickups[listIndex]->SetGSQ("nightvision");
		m_pickups[listIndex]->SetFadeInSeq("fade_nightvision");
		break;
	case OBJ_ITM_MINE:
		m_pickups[listIndex]->SetGSQ("claymore");
		m_pickups[listIndex]->SetFadeInSeq("fade_claymore");
		break;
	case OBJ_ITM_MEDKIT:
		m_pickups[listIndex]->SetGSQ("medkit");
		m_pickups[listIndex]->SetFadeInSeq("fade_medkit");
		break;
	case OBJ_ITM_GRENADE:
		m_pickups[listIndex]->SetGSQ("grenade");
		m_pickups[listIndex]->SetFadeInSeq("fade_grenade");
		break;
	case OBJ_CTF_FLAG:
		break;
	case OBJ_HLTH_SM:
		m_pickups[listIndex]->SetGSQ("health");
		m_pickups[listIndex]->SetFadeInSeq("fade_health");
		break;
	case OBJ_HLTH_LRG:
		m_pickups[listIndex]->SetGSQ("health");
		m_pickups[listIndex]->SetFadeInSeq("fade_health");
		break;
	}
}

void PickupList::SetDroppedStringIndex(int listIndex)
{
	switch(listIndex)
	{
	case OBJ_NONE:
	case OBJ_AMMO_KNIFE:
	case OBJ_AMMO_PISTOL:
	case OBJ_AMMO_SHOTGUN:
	case OBJ_AMMO_PISTOL2:
	case OBJ_AMMO_AUTO:
	case OBJ_AMMO_MPG:
	case OBJ_AMMO_GAS:
	case OBJ_AMMO_ROCKET:
	case OBJ_AMMO_SLUG:
	case OBJ_AMMO_SINGLE_PISTOL:
	case OBJ_AMMO_SINGLE_SHOTGUN:
	case OBJ_AMMO_SINGLE_AUTO:
	case OBJ_ARMOR:
	case OBJ_ITM_FLASHPACK:
	case OBJ_ITM_C4:
	case OBJ_ITM_LIGHTGOGGLES:
	case OBJ_ITM_MINE:
	case OBJ_ITM_MEDKIT:
	case OBJ_ITM_GRENADE:
	case OBJ_CTF_FLAG:
	case OBJ_HLTH_SM:
	case OBJ_HLTH_LRG:
		break;
	case OBJ_WPN_PISTOL2:
		m_pickups[listIndex]->SetDroppedStringIndex(ITEMS_DROPPED_44PISTOL);
		break;
	case OBJ_WPN_PISTOL1:
		m_pickups[listIndex]->SetDroppedStringIndex(ITEMS_DROPPED_9MMPISTOL);
		break;
	case OBJ_WPN_MACHINEPISTOL:
		m_pickups[listIndex]->SetDroppedStringIndex(ITEMS_DROPPED_9MMSUPSMG);
		break;
	case OBJ_WPN_ASSAULTRIFLE:
		m_pickups[listIndex]->SetDroppedStringIndex(ITEMS_DROPPED_556SMG);
		break;
	case OBJ_WPN_SNIPER:
		m_pickups[listIndex]->SetDroppedStringIndex(ITEMS_DROPPED_SNIPER);
		break;
	case OBJ_WPN_AUTOSHOTGUN:
		m_pickups[listIndex]->SetDroppedStringIndex(ITEMS_DROPPED_SLUGTHROWER);
		break;
	case OBJ_WPN_SHOTGUN:
		m_pickups[listIndex]->SetDroppedStringIndex(ITEMS_DROPPED_SHOTGUN);
		break;
	case OBJ_WPN_MACHINEGUN:
		m_pickups[listIndex]->SetDroppedStringIndex(ITEMS_DROPPED_HVYMACH);
		break;
	case OBJ_WPN_ROCKET:
		m_pickups[listIndex]->SetDroppedStringIndex(ITEMS_DROPPED_ROCKETLAUNCHER);
		break;
	case OBJ_WPN_MICROWAVE:
		m_pickups[listIndex]->SetDroppedStringIndex(ITEMS_DROPPED_MPG);
		break;
	case OBJ_WPN_FLAMEGUN:
		m_pickups[listIndex]->SetDroppedStringIndex(ITEMS_DROPPED_FLAMEGUN);
		break;
	default:
		break;
	}
}


Pickup *PickupList::GetPickupFromSpawnName(char *name)
{
	if (NULL == name)
	{
		return NULL;
	}

	for (int i = 0; i < MAX_ITM_OBJS; i++)
	{
		if (0 == strcmp(name, m_pickups[i]->GetSpawnName()))
		{
			return m_pickups[i];
		}
	}
	// ctf flags change their class name frequently but they'll always begin with 'ctf'
	if (name[0] == 'c' && name[1] == 't' && name[2] == 'f')
	{	// found a ctf flag
		return m_pickups[OBJ_CTF_FLAG];
	}
	return NULL;
}

// in the dark times, we stored (index into itemSpawns[]) of an item in edict_t::dmg, so a lot of code
//looked like itemSpawns[ent->dmg]...
//
//it gives me a creepy feeling just thinking about it.
//
Pickup	*PickupList::GetPickupFromEdict(edict_t *ent)
{
	// it's unfortunate, but the quickest way (for now) to find the Pickup that corresponds to an edict_t
	//is to do strcmp's on edict_t::classname. we could go back to storing an index in edict_t::dmg, but
	//that is soooooo lame. quick, but lame.

	// putting a binary search here would be a simple way to improve things. just create an ordered list of Pickup 
	//names, like PickupList::m_pickupNames or something.

	if (NULL == ent)
	{
		return NULL;
	}
	return GetPickupFromSpawnName(ent->classname);
}

int PickupList::GetPickupIndexFromEdict(edict_t *ent)
{
	// it's unfortunate, but the quickest way (for now) to find the Pickup that corresponds to an edict_t
	//is to do strcmp's on edict_t::classname. we could go back to storing an index in edict_t::dmg, but
	//that is soooooo lame. quick, but lame.

	// putting a binary search here would be a simple way to improve things. just create an ordered list of Pickup 
	//names, like PickupList::m_pickupNames or something.

	if (NULL == ent)
	{
		return 0;
	}
	if (NULL == ent->classname)
	{
		return 0;
	}

	for (int i = 0; i < MAX_ITM_OBJS; i++)
	{
		if (0 == strcmp(ent->classname, m_pickups[i]->GetSpawnName()))
		{
			return i;
		}
	}
	// ctf flags change their class name frequently but they'll always begin with 'ctf'
	if (0 == strncmp(ent->classname, "ctf", 3))
	{	// found a ctf flag
		return OBJ_CTF_FLAG;
	}
	return 0;
}

Pickup *PickupList::GetPickupFromType(pickup_t type, int spawnParm)
{
	for (int i = 0; i < MAX_ITM_OBJS; i++)
	{
		if (m_pickups[i]->ComparePickupType(type) && m_pickups[i]->CompareSpawnParm(spawnParm))
		{
			return m_pickups[i];
		}
	}
	return NULL;
}

PickupInst *PickupList::GetPickupInstFromTargetname(char* targetname)
{
	list<PickupInst*>::iterator	it;
	edict_t						*ent = NULL;

	if (NULL == targetname)
	{
		return NULL;
	}
	for (it = m_pickupInsts.begin(); it != m_pickupInsts.end(); it++)
	{
		if ( (ent = (*it)->GetEdict()) && (0 == strcmp(targetname, ent->targetname)) )
		{
			return (PickupInst*)(*it);
		}
	}
	return NULL;
}

PickupInst *PickupList::GetPickupInstFromEdict(edict_t *ent)
{
	list<PickupInst*>::iterator	it;

	if (NULL == ent)
	{
		return NULL;
	}
	for (it = m_pickupInsts.begin(); it != m_pickupInsts.end(); it++)
	{
		if ((*it)->EdictCompare(ent))
		{
			return (PickupInst*)(*it);
		}
	}
	return NULL;
}

// adds an instance of a pickup to the list of edicts that gets polled in FrameUpdate()
void PickupList::RegisterEdict(edict_t *ent)
{
	if (ent)
	{
		list<PickupInst*>::iterator	it;

		for (it = m_pickupInsts.begin(); it != m_pickupInsts.end(); it++)
		{
			if ((*it)->EdictCompare(ent))
			{	// already registered
				return;
			}
		}
		ent->flags |= FL_PICKUP;

		m_pickupInsts.push_back(new PickupInst(ent));

		// this check probably doesn't cover all cases of runtime PickUp creation
		if (ent->spawnflags&DROPPED_ITEM)
		{
			HandleInfoEvent(PickupInfoList::event_CREATE_RUNTIME, ent);
		}
		else
		{
			HandleInfoEvent(PickupInfoList::event_CREATE_LOADTIME, ent);
		}

		// if we have a single-player pickup that can respawn we need to precache the respawn effect
		if (ent->spawnflags & SF_PICKUP_RESPAWN)
		{
			gi.effectindex("environ/irespawn");
			gi.soundindex("dm/irespawn.wav");
		}

	}
}

// remove this edict from our list-to-be-polled-by-FrameUpdate. if it ain't in the list, return false.
bool PickupList::Unregister(edict_t *ent)
{
	list<PickupInst*>::iterator	it;

	for (it = m_pickupInsts.begin(); it != m_pickupInsts.end(); it++)
	{
		if (((PickupInst*)(*it))->EdictCompare(ent))
		{	// found him. remove him.

			delete (*it);
			m_pickupInsts.erase(it);
			return true;
		}
	}
	return false;
}

bool PickupList::UnregisterAll()
{
	list<PickupInst*>::iterator	it;

	for (it = m_pickupInsts.begin(); it != m_pickupInsts.end(); it++)
	{
		delete (*it);
	}
	m_pickupInsts.clear();
	return true;
}

bool PickupList::HandleInfoEvent(int nEvent, edict_t *ent)
{
	PickupInst *pickupInst = NULL;

	if ( (NULL == m_pickupInfoList) || 
		 (NULL == (pickupInst = GetPickupInstFromEdict(ent))) )
	{
		return false;
	}

	return m_pickupInfoList->HandleEvent(nEvent, pickupInst);
}

// if this pickup starts off in a crate, set it to SOLID_NOT until the crate is broken
void PickupList::InitCrated(edict_t *ent)
{
	ent->solid = SOLID_NOT;
	// it should be invisible
	if (ent->ghoulInst)
	{
		ent->ghoulInst->SetOnOff(false, level.time);
	}
}

// de-crate any pickups targeted by this crate
void PickupList::BustCrate(edict_t *crate)
{
	PickupInst			*pickupInst = NULL;
	edict_t				*ent = NULL;

	if (crate && crate->target)
	{
		m_crateList.Create(crate->target);

		while (pickupInst = m_crateList.Pop())
		{
			if ( (ent = pickupInst->GetEdict()) &&
				 (ent->spawnflags & SF_PICKUP_START_CRATED) )
			{
				ent->solid = SOLID_TRIGGER;
				// it should be visible
				if (ent->ghoulInst)
				{
					ent->ghoulInst->SetOnOff(true, level.time);
				}
			}
		}
	}
}

Pickup *PickupInst::GetPickup()
{ 
	return thePickupList.GetPickup(m_pickup);
}

void PickupInst::Read(void)
{
	PickupInst	temp;

	gi.ReadFromSavegame('PLPI', &temp, sizeof(temp));

	m_pickup = temp.m_pickup;
	m_edict = GetEdictPtr((int)temp.m_edict);
	m_fadeStatus = temp.m_fadeStatus;
}

void PickupInst::Write(void)
{
	PickupInst	temp;

	temp.m_pickup = m_pickup;
	*(int *)&temp.m_edict = GetEdictNum(m_edict);
	temp.m_fadeStatus = m_fadeStatus;

	gi.AppendToSavegame('PLPI', &temp, sizeof(temp));
}

void PickupList::Read(void)
{
	int			i, count;
	PickupInst	*inst;

	gi.ReadFromSavegame('PLNM', &count, sizeof(count));
	for(i = 0; i < count; i++)
	{
		inst = new PickupInst;
		inst->Read();
		m_pickupInsts.push_back(inst);
	}
	gi.ReadFromSavegame('PLWA', &m_bDisplayedWeaponArenaHelp, sizeof(m_bDisplayedWeaponArenaHelp));
}

void PickupList::Write(void)
{
	list<PickupInst*>::iterator		it;
	int								count;

	count = m_pickupInsts.size();
	gi.AppendToSavegame('PLNM', &count, sizeof(count));

	for(it = m_pickupInsts.begin(); it != m_pickupInsts.end(); it++)
	{
		(*it)->Write();
	}
	gi.AppendToSavegame('PLWA', &m_bDisplayedWeaponArenaHelp, sizeof(m_bDisplayedWeaponArenaHelp));
}

// end