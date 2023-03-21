#include "q_shared.h"
#include "..\ghoul\ighoul.h"
#include "w_public.h"
#include "w_types.h"
#include "w_weapons.h"
#include "w_utils.h"
#include "player.h"
#include "../gamecpp/game.h"

extern player_com_import_t	picom;
extern player_cl_import_t	picl;
extern player_sv_import_t	pisv;

extern int isClient;
extern int fireEvent;
extern int altfireEvent;

// noiserad does NOT belong here...

attackInfoExtra_t attacks[] =
{
	//ammo			noiseRad	waver		waverMax	kickamount	AttackID
	{AMMO_NONE,		0.0,		0.0,		0.0,		0.0,		ATK_NOTHING},			//SFW_EMPTYSLOT
	{AMMO_KNIFE,	0.0,		0.0,		0.0,		0.0,		ATK_KNIFE},				//SFW_KNIFE,
	{AMMO_KNIFE,	0.0,		0.0,		0.0,		0.0,		ATK_KNIFE_ALT},			//SFW_KNIFE,
	{AMMO_44,		900.0,		0.2,		0.8,		5.0,		ATK_PISTOL2},			//SFW_PISTOL2,
	{AMMO_9MM,		600.0,		0.3,		1.5,		1.0,		ATK_PISTOL1},			//SFW_PISTOL1,
	{AMMO_9MM,		10.0,		0.6,		3.5,		0.0,		ATK_MACHINEPISTOL},		//SFW_MACHINEPISTOL,
	{AMMO_556,		800.0,		0.4,		1.8,		0.0,		ATK_ASSAULTRIFLE},		//SFW_ASSAULTRIFLE,
	{AMMO_556,		0.0,		0.0,		0.0,		2.0,		ATK_SNIPER},			//SFW_SNIPER, ->sniper rifle
	{AMMO_NONE,		0.0,		0.0,		0.0,		0.0,		ATK_SNIPER_ALT},		//SFW_SNIPER, ->sniper rifle
	{AMMO_SLUG,		1200.0,		1.5,		1.5,		4.0,		ATK_AUTOSHOTGUN},		//SFW_AUTOSHOTGUN, ->shotgun
	{AMMO_SLUG,		0.0,		0.0,		0.0,		4.0,		ATK_AUTOSHOTGUN_ALT},	//SFW_AUTOSHOTGUN, ->shotgun
	{AMMO_SHELLS,	1200.0,		0.0,		0.0,		5.0,		ATK_SHOTGUN},			//SFW_SHOTGUN, ->shotgun
	{AMMO_556,		1500.0,		0.1,		0.4,		0.0,		ATK_MACHINEGUN},		//SFW_MACHINEGUN,	-> machinegun
	{AMMO_556,		500.0,		0.0,		0.0,		0.0,		ATK_MACHINEGUN_ALT},	//SFW_MACHINEGUN,	-> machinegun
	{AMMO_ROCKET,	0.0,		0.0,		0.0,		0.0,		ATK_ROCKET},			//SFW_ROCKET,	-> rocketlauncher
	{AMMO_ROCKET,	0.0,		0.0,		0.0,		0.0,		ATK_ROCKET_ALT},		//SFW_ROCKET,	-> rocketlauncher
	{AMMO_MWAVE,	0.0,		0.0,		0.0,		0.0,		ATK_MICROWAVE},			//SFW_MICROWAVEPULSE,
	{AMMO_MWAVE,	0.0,		0.0,		0.0,		0.0,		ATK_MICROWAVE_ALT},		//SFW_MICROWAVEPULSE,
	{AMMO_FTHROWER,	0.0,		0.0,		0.0,		0.0,		ATK_FLAMEGUN},			//SFW_FLAMETHROWER,
	{AMMO_FTHROWER,	0.0,		0.0,		0.0,		0.0,		ATK_FLAMEGUN_ALT},		//SFW_FLAMETHROWER,
};

WeapSoundCallBack		theWeapSoundCallback;
GunFireCallBack			theGunFireCallback;
AltfireCallBack			theAltfireCallback;
EOSCallBack				theEOSCallback;
KnifeCleanCallBack		theKnifeCleanCallback;
MinimiReloadCallBack	theMinimiReloadCallback;
EffectCallBack			theEffectCallback;

weaponInfoExtra_t extraInfo[] =
{	// name						slotsNeeded	clipsize	attack1			attack2
	{"",						0,			-1,			&attacks[0],	&attacks[0]},	//SFW_EMPTYSLOT
	{"Knife",					0,			 0,			&attacks[1],	&attacks[2]},	//SFW_KNIFE,
	{"Pistol2",					1,			 9,			&attacks[3],	&attacks[0]},	//SFW_PISTOL2,
	{"Pistol1",					1,			18,			&attacks[4], 	&attacks[0]},	//SFW_PISTOL1,
	{"MPistol",					2,			32,			&attacks[5],	&attacks[0]},	//SFW_MACHINEPISTOL,
	{"Assaultrifle",			2,			40,			&attacks[6],	&attacks[0]},	//SFW_ASSAULTRIFLE,
	{"SniperRifle",				2,			 6,			&attacks[7],	&attacks[8]},	//SFW_SNIPER, ->sniper rifle
	{"Autoshotgun",				2,			10,			&attacks[9],	&attacks[10]},	//SFW_AUTOSHOTGUN, ->shotgun
	{"Shotgun",					2,			 8,			&attacks[11],	&attacks[0]},	//SFW_SHOTGUN, ->shotgun
	{"Machinegun",			 	2,			40,			&attacks[12],	&attacks[13]},	//SFW_MACHINEGUN,	-> machinegun
	{"Rocket",					3,			 4,			&attacks[14],	&attacks[15]},	//SFW_ROCKET,	-> rocketlauncher
	{"MPG",						3,			30,			&attacks[16],	&attacks[17]},	//SFW_MICROWAVEPULSE,
	{"Flamegun",				3,		    60,			&attacks[18],	&attacks[19]},	//SFW_FLAMETHROWER,
	{"Hurthand",				0,			 0,			&attacks[0],	&attacks[0]},	//SFW_HURTHAND,
	{"Throwhand",				0,			 0,			&attacks[0],	&attacks[0]},	//SFW_THROWHAND,
};

weaponInfo_c		noweap(&extraInfo[0]);
knifeInfo			knife(&extraInfo[1]);
pistol2Info			eagle(&extraInfo[2]); 
pistol1Info			glock(&extraInfo[3]);
machinePistolInfo	ingram(&extraInfo[4]);
assaultRifleInfo	hk53(&extraInfo[5]);
sniperInfo			sig(&extraInfo[6]);
autoshotgunInfo		jhammer(&extraInfo[7]);
shotgunInfo			spas(&extraInfo[8]);
machinegunInfo		minimi(&extraInfo[9]);
rocketInfo			m202a2(&extraInfo[10]);
microInfo			micro(&extraInfo[11]);
flamegunInfo		fthrow(&extraInfo[12]);
hurthandInfo		hhand(&extraInfo[13]);
throwhandInfo		thand(&extraInfo[14]);

weaponInfo_c *weapInfo[SFW_NUM_WEAPONS] =
{
	&noweap,
	&knife,
	&eagle,
	&glock,
	&ingram,
	&hk53,
	&sig,
	&jhammer,
	&spas,
	&minimi,
	&m202a2,
	&micro,
	&fthrow,
	&hhand,
	&thand,
};

weaponInfo_c::weaponInfo_c(weaponInfoExtra_t *w)
{
	clip = w->clipsize;
	slotsNeeded = w->slotsNeeded;
	ammoType[0] = w->a1->ammoType;
	ammoType[1] = w->a2->ammoType;
	noiseRad[0] = w->a1->noiseRad;
	noiseRad[1] = w->a2->noiseRad;
	waver[0] = w->a1->waver;
	waver[1] = w->a2->waver;
	waverMax[0] = w->a1->waverMax;
	waverMax[1] = w->a2->waverMax;

	kick[0] = w->a1->kickAmount;
	kick[1] = w->a2->kickAmount;

	strcpy(modelName, w->name);

	atkID[0] = w->a1->id;
	atkID[1] = w->a2->id;

	soundToken = NULL_GhoulID;
}

int weaponInfo_c::useAmmo1(sharedEdict_t &ent, int *skipIdle)
{
	weapon_c *weap = inven(ent)->getCurWeapon();

	if(getAmmoType(0) == AMMO_NONE)
		return 1;
	
	if(!inven(ent)->rulesDoWeaponsUseAmmo())
		return 1;

	if(!weap->getClip())
	{
		if(inven(ent)->rulesIsWeaponReloadAutomatic())
			inven(ent)->reload();
		else
			inven(ent)->dryfire();

		return 0;
	}

	weap->setClip(weap->getClip() - 1);
	return 1;
}

int weaponInfo_c::useAmmo2(sharedEdict_t &ent, int *skipIdle)
{
	weapon_c *weap = inven(ent)->getCurWeapon();

	if(getAmmoType(1) == AMMO_NONE)
		return 1;

	if(!inven(ent)->rulesDoWeaponsUseAmmo())
		return 1;

	if(!weap->getClip())
	{
		if(inven(ent)->rulesIsWeaponReloadAutomatic())
			inven(ent)->reload();
		else
			inven(ent)->dryfire();

		return 0;
	}

	weap->setClip(weap->getClip() - 1);
	return 1;
}

int weaponInfo_c::checkReloadAtEOS(sharedEdict_t &ent)
{
	if((wAnim(ent)->getAnimType() == WANIM_FIRE)||(wAnim(ent)->getAnimType() == WANIM_ALTFIRE))
	{
		if(!inven(ent)->rulesDoWeaponsUseAmmo())
			return 0;

		if(!inven(ent)->getCurClip())
		{
			if(inven(ent)->rulesIsWeaponReloadAutomatic())
			{
				inven(ent)->reload();
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}

	return 0;
}

void weaponInfo_c::precacheResources()
{
	if(getAtkID(0) != ATK_NOTHING)
	{
		pisv.CacheAttack((int)getAtkID(0));
	}
	if(getAtkID(1) != ATK_NOTHING)
	{
		pisv.CacheAttack((int)getAtkID(1));
	}
}
//------------------------------------------------------------------------

weapon_c::weapon_c(void)
{
	curClip = 0;
	cannotFire = 0;
	type = SFW_EMPTYSLOT;
}

weapon_c::weapon_c(weaponInfo_c *wInfo)
{
	curClip = getWeaponInfo()->getClipSize();
	cannotFire = 0;
	type = SFW_EMPTYSLOT;
}

void weapon_c::setType(int newType, int startWithFullClip)
{
	type = newType;

	if(startWithFullClip == -1)
	{
		curClip = getWeaponInfo()->getClipSize();
	}
	else if(startWithFullClip)
	{
		curClip = startWithFullClip;
	}
	else
	{
		curClip = 0;
	}
}

int weapon_c::handleAmmo(sharedEdict_t &ent, int mode, int *skipIdle)
{
	return((mode) ? getWeaponInfo()->useAmmo2(ent, skipIdle) : getWeaponInfo()->useAmmo1(ent, skipIdle));
}

weaponInfo_c *weapon_c::getWeaponInfo(void)
{
	return weapInfo[type];
}

int weapon_c::canSelect(sharedEdict_t &ent,int withNoAmmo)
{
	if(type == SFW_EMPTYSLOT)
		return 0;

	if(isClient)
	{
		// Client is always able to select any weapon.
		return(1);
	}

	if(!withNoAmmo)
	{
		if((curClip == 0)&&(inven(ent)->rulesDoWeaponsUseAmmo()))
		{
			if((inven(ent)->hasAmmo(weapInfo[type]->getAmmoType(0)) == 0)&&(!weapInfo[type]->canSelectWithNoAmmo()))
				return 0;
		}
	}

	return(1);
}

//---------------------------------------------------------------------------------------------
//
//									wrapper funcs
//
//---------------------------------------------------------------------------------------------

void Fire(sharedEdict_t &sh, edict_t &ent, inven_c &inven)
{
	if(!inven.getCurWeapon()->getWeaponInfo()->atMomentOfFire(sh))
		return;
	
	if(wAnim(sh)->getAnimType() != WANIM_FIRE)
		return;

	if(!isClient)
		pisv.FireHelper(sh, ent, inven);
	else
		picl.FireHelper(sh, ent, inven);

	sh.weaponkick_angles[PITCH]=-(inven.getCurWeapon()->getWeaponInfo()->getKick(0));
}

void Altfire(sharedEdict_t &sh, edict_t &ent, inven_c &inven)
{
	if(!inven.getCurWeapon()->getWeaponInfo()->atMomentOfFire(sh))
		return;

	if(wAnim(sh)->getAnimType() != WANIM_ALTFIRE)
		return;

	if(!isClient)
		pisv.AltfireHelper(sh, ent, inven);
	else
		picl.AltfireHelper(sh, ent, inven);

	sh.weaponkick_angles[PITCH]=-(inven.getCurWeapon()->getWeaponInfo()->getKick(1));
}

void EOS(sharedEdict_t &ent, inven_c &inven)
{
	if(inven.getCurWeapon()->getWeaponInfo()->handleEOS(ent))
	{
		// Special end of sequence stuff handled by weapon specific EOS.
		return;
	}

	if(inven.getCurWeapon()->getWeaponInfo()->checkReloadAtEOS(ent))
	{
		// Do we need to reload now?
		return;
	}

	switch(wAnim(ent)->getAnimType())
	{
		case WANIM_NORMAL:
		case WANIM_RELOAD:
			inven.idle();
			break;

		case WANIM_FIRE:
			if(!(inven.pendingCommand()))
			{
				qboolean doAttack=false;

				if(!isClient)
				{
					if(!inven.isClientPredicting())
					{
						if(ent.attack)
							doAttack=true;
					}
					else
					{
						if((inven.getCurWeaponID()==SFW_FLAMEGUN)||
						   (inven.getCurWeaponID()==SFW_KNIFE)||
						   (inven.getCurWeaponID()==SFW_MICROWAVEPULSE))
						{
							// Yuck!! Sigh!!

							if(ent.attack)
								doAttack=true;
						}
						else
						{
							if(pisv.getFireEvent(inven.getOwner()->edict))
								doAttack=true;
						}
					}
				}
				else
				{
					if(ent.attack)
						doAttack=true;
				}

				if(doAttack)
				{
					inven.getAnimInfo()->incFireLoop();
					int skipIdle = 0;
					if(!inven.getCurWeapon()->handleAmmo(ent, 0, &skipIdle))
					{
						if(!skipIdle)
						{
							inven.idle();
						}
						return;
					}

					if(!isClient)
					{
						if(inven.isClientPredicting())
						{
							if((inven.getCurWeaponID()==SFW_FLAMEGUN)||
							   (inven.getCurWeaponID()==SFW_KNIFE)||
							   (inven.getCurWeaponID()==SFW_ASSAULTRIFLE)||
							   (inven.getCurWeaponID()==SFW_MACHINEPISTOL)||
							   (inven.getCurWeaponID()==SFW_MICROWAVEPULSE))
							{
								// Yuck!! Sigh!!

								RunVWeapAnim(&ent,
											 inven.getCurWeapon()->getWeaponInfo()->getFireSeq(ent),
											 inven.getCurWeapon()->getWeaponInfo()->getLoopType());
							}
							else
							{
								RunVWeapAnim(&ent,
											 inven.getCurWeapon()->getWeaponInfo()->getFireSeq(ent),
											 inven.getCurWeapon()->getWeaponInfo()->getLoopType(),
											 pisv.getFireEvent(inven.getOwner()->edict));
							}
							
							pisv.clearFireEvent(inven.getOwner()->edict);
						}
						else
						{
							RunVWeapAnim(&ent,
										 inven.getCurWeapon()->getWeaponInfo()->getFireSeq(ent),
										 inven.getCurWeapon()->getWeaponInfo()->getLoopType());
						}
					}
					else
					{
						RunVWeapAnim(&ent,
									 inven.getCurWeapon()->getWeaponInfo()->getFireSeq(ent),
									 inven.getCurWeapon()->getWeaponInfo()->getLoopType());
					
						fireEvent=1;
					}

					// Some weapons don't just loop their anim.
					inven.getCurWeapon()->getWeaponInfo()->handleFireLoop(ent);

					return;
				}
			}
			inven.getAnimInfo()->clearFireLoop();
			inven.idle();
			break;

		case WANIM_ALTFIRE:
			if(!inven.pendingCommand())
			{
				qboolean doAltAttack=false;

				if(!isClient)
				{
					if(!inven.isClientPredicting())
					{
						if(ent.altattack)
							doAltAttack=true;
					}
					else
					{
						if(pisv.getAltfireEvent(inven.getOwner()->edict))
							doAltAttack=true;
					}
				}
				else
				{
					if(ent.altattack)
						doAltAttack=true;
				}

				if(doAltAttack)
				{
					inven.getAnimInfo()->incFireLoop();
					int skipIdle = 0;
					if(!inven.getCurWeapon()->handleAmmo(ent, 1, &skipIdle))
					{
						if(!skipIdle)
						{
							inven.idle();
						}
						return;
					}

					if(!isClient)
					{
						if(inven.isClientPredicting())
						{
							if(inven.getCurWeaponID()==SFW_ROCKET)
							{
								// Yuck!! Sigh!!

								RunVWeapAnim(&ent,
											 inven.getCurWeapon()->getWeaponInfo()->getAltFireSeq(ent),
											 IGhoulInst::Hold);
							}
							else
							{
								RunVWeapAnim(&ent,
											 inven.getCurWeapon()->getWeaponInfo()->getAltFireSeq(ent),
											 IGhoulInst::Hold,
											 pisv.getAltfireEvent(inven.getOwner()->edict));
							}

							pisv.clearAltfireEvent(inven.getOwner()->edict);
						}
						else
						{
							RunVWeapAnim(&ent,
										 inven.getCurWeapon()->getWeaponInfo()->getAltFireSeq(ent),
										 IGhoulInst::Hold);
						}
					}
					else
					{
						RunVWeapAnim(&ent,
									 inven.getCurWeapon()->getWeaponInfo()->getAltFireSeq(ent),
									 IGhoulInst::Hold);
				
						altfireEvent=1;
					}

					// Some weapons don't just loop their anim!
					inven.getCurWeapon()->getWeaponInfo()->handleAltfireLoop(ent);

					return;
				}
			}
			inven.getAnimInfo()->clearFireLoop();
			inven.idle();
			break;

		case WANIM_PUTAWAY:
			// Set new weapon to take out.
			inven.setPendingChange();
			break;

		case WANIM_LOSE:
			inven.idle();
			break;
	}
}

//------------------------------------------------------------------------

bool WeapSoundCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	inven_c *inv = (inven_c *)user;
	sharedEdict_t *ent = inv->getOwner();

	if(isClient)
		picl.WeapSoundHelper(data);

	return true;
}

bool GunFireCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	inven_c *inv = (inven_c *)user;
	sharedEdict_t *ent = inv->getOwner();

	Fire(*ent, *ent->edict, *inv);

	if (data && *((char *)data)=='1')
	{	// Use ammo during this fire.
		if (!inv->getCurWeapon()->getWeaponInfo()->useAmmo1(*ent))
		{
			EOS(*ent, *inv);
		}
	}

	return true;
}

bool AltfireCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	inven_c *inv = (inven_c *)user;
	sharedEdict_t *ent = inv->getOwner();

	Altfire(*ent, *ent->edict, *inv);
	return true;
}

bool EOSCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	inven_c *inv = (inven_c *)user;
	sharedEdict_t *ent = inv->getOwner();

	EOS(*ent, *inv);
	return true;
}

bool KnifeCleanCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	inven_c *inv = (inven_c *)user;
	sharedEdict_t *ent = inv->getOwner();

	inv->getCurWeapon()->getWeaponInfo()->handleSpecialNotes((char *)data, *ent);

	return true;
}

bool MinimiReloadCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	inven_c *inv = (inven_c *)user;
	sharedEdict_t *ent = inv->getOwner();

	inv->getCurWeapon()->getWeaponInfo()->handleSpecialNotes((char *)data, *ent);

	return true;
}

bool EffectCallBack::Execute(IGhoulInst *me,void *user,float now,const void *data)
{
	inven_c *inv = (inven_c *)user;
	sharedEdict_t *ent = inv->getOwner();

	if(isClient)
		picl.WeaponEffectHelper(data);

	return true;
}


class EffectPrecacheCallBack: public IGhoulCallBack
{
	public:virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data)
	{
		char effectstr[256], *effectname;

		strcpy(effectstr, (char *)data);
		effectname = strtok(effectstr, " ");
		pisv.EffectIndex(effectname);
		return true;
	}
};

EffectPrecacheCallBack effectPrecacher;



//------------------------------------------------------------------------

class SoundPrecacheCallBack: public IGhoulCallBack
{
	public:virtual bool Execute(IGhoulInst *me,void *user,float now,const void *data)
	{
		char buffer[256], soundname[256], *soundtok, *numtok;
		int i,c;

		strcpy(soundname, (char *)data);
		soundtok = strtok(soundname, " ");
		numtok = strtok(NULL, " ");
		if (!numtok)
		{
			c=0;
		}
		else
		{
			c=atoi(numtok);
		}

		if (c>0)
		{
			for (i=1; i<=c; i++)
			{
				picom.Com_Sprintf(buffer, sizeof(buffer), "%s%d.wav", soundtok, i);
				pisv.SoundIndex(buffer);
			}
		}
		else
		{
			picom.Com_Sprintf(buffer, sizeof(buffer), "%s.wav", soundtok);
			pisv.SoundIndex(buffer);
		}
		return true;
	}
};

SoundPrecacheCallBack soundPrecacher;

//------------------------------------------------------------------------

void W_UncacheViewWeaponModels(void)
{
	int i;

	for(i = 0; i < SFW_NUM_WEAPONS; i++)
	{
		if(weapInfo[i]->getGhoulObj())
		{
			weapInfo[i]->setGhoulObj(NULL);
		}
	}
}

void W_PrecacheViewWeaponModels(int types)
{
	// types is a bitflag of all the weapon types that should be precached

	char buffer[256];

	for(int i = SFW_EMPTYSLOT + 1; i < SFW_NUM_WEAPONS; i++)
	{
		if(!(types & (1<<i)))continue;

#ifdef _OEM_
		if((i == SFW_MACHINEPISTOL)||(i == SFW_AUTOSHOTGUN)||(i == SFW_MACHINEGUN)
			|| (i == SFW_ROCKET) || (i == SFW_MICROWAVEPULSE) || (i == SFW_FLAMEGUN))
		{	// none of these in the oem
			continue;
		}
#endif
#ifdef _DEMO_
		if((i == SFW_MACHINEPISTOL)||(i == SFW_AUTOSHOTGUN)||(i == SFW_MACHINEGUN)
			|| (i == SFW_MICROWAVEPULSE) || (i == SFW_FLAMEGUN))
		{	// none of these in the demo
			continue;
		}
#endif

		picom.Com_Sprintf(buffer, sizeof(buffer), "Weapon/Inview/%s", extraInfo[i].name);

		IGhoulObj *obj;

		if(weapInfo[i]->getGhoulObj())continue;//only need to precache if this weapon isn't already loaded, of course

		obj = ((IGhoul *)pisv.GetGhoul())->NewObj();

		if(!obj)
		{
			// This is real bad - should not happen right?

			assert(obj);
			continue;
		}

		picom.RegisterGSQSequences(buffer, extraInfo[i].name, obj);
		obj->RegisterEverything();
		obj->PreCache(picom.ghl_specular->value>0.0f);

		// Precache all the weapon sounds.

		GhoulID id = obj->FindNoteToken("sound");

		if(id)
		{		
			obj->FireAllNoteCallBacks(&soundPrecacher,id);
		}

		// Precache all effect calls.

		id = obj->FindNoteToken("effect");

		if(id)
		{	
			obj->FireAllNoteCallBacks(&effectPrecacher,id);
		}

		// Precache all other resources.

		weapInfo[i]->setGhoulObj(obj);
		weapInfo[i]->precacheResources();
	}
}
