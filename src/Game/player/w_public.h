#ifndef __W_PUBLIC_H_
#define __W_PUBLIC_H_

typedef enum
{
	ATK_NOTHING,
	ATK_KNIFE,
	ATK_KNIFE_ALT,
	ATK_PISTOL1,
	ATK_PISTOL2,
	ATK_MACHINEPISTOL,
	ATK_ASSAULTRIFLE,
	ATK_SNIPER,
	ATK_SNIPER_ALT,
	ATK_AUTOSHOTGUN,
	ATK_AUTOSHOTGUN_ALT,
	ATK_SHOTGUN,
	ATK_MACHINEGUN,
	ATK_MACHINEGUN_ALT,
	ATK_ROCKET,
	ATK_ROCKET_ALT,
	ATK_MICROWAVE,
	ATK_MICROWAVE_ALT,
	ATK_FLAMEGUN,
	ATK_FLAMEGUN_ALT,
	ATK_DOG_ATTACK,
	ATK_AUTOGUN,
	ATK_GRENADE,
	ATK_MOLITOV,
	ATK_GASGREN,
	ATK_MAGICBULLET,
	ATK_FLASHGREN,
	ATK_MACHETE,
	ATK_HELIGUN,
	ATK_DEKKER,
	ATK_THROWSTAR,
	ATK_NUMWEAPONS
}attacks_e;

class invPub_c
{
private:
public:
	virtual void		clearInv(bool){}
	virtual void		removeSpecificItem(int type){}

	virtual void		handlePlayerDeath(void){}
	virtual void		frameUpdate(void){}

	virtual int			addCommand(char *string,qboolean internal=false){return 0;}
	virtual void		stockWeapons(void){}

	virtual int			selectWeapon(int type){return 0;}
	virtual void		selectWeaponSlot(int weaponNum){}
	virtual void		selectBestWeapon(void){}
	virtual void		takeOutWeapon(int type){}
	virtual void		takeOutWeaponSlot(int weaponNum){}
	virtual void		takeOutBestWeapon(void){}

	virtual int			addAmmoType(int type, int amount){return 0;}
	virtual int			addAmmoByGunType(int type, int amount){return 0;}
	virtual int			stripAmmoFromGun(int type, int amount){return 0;}
	virtual int			addWeaponType(int type, int fullClip = 0){return 0;}
	virtual	int			getEncumbrance(void){return 0;};
	virtual	int			getEncumbranceByType(int weapon){return 0;};
	virtual int			addItem(int type, int amount, int maxAmount = -1){return 0;}
	virtual int			addArmor(int amount){return 0;}
	virtual int			hasItemType(int type){return 0;}

	virtual int			hasWeaponType(int type){return 0;}
	virtual int			getCurWeaponType(void){return 0;}
	virtual	int			getBestWeaponType(void){return 0;}
	virtual int			getCurItemType(void){return 0;}
	virtual int			getCurItemAmount(void){return 0;}
	virtual int			getCurAmmo(void){return 0;}
	virtual int			getCurClip(void){return 0;}
	virtual int			getClipMax(void){return 0;}
	virtual int			getClipMaxByType(int weapon){return 0;}
	virtual int			adjustDamageByArmor(int initDamage, float penetrate, float absorb){return 0;}
	virtual int			getArmorCount(void){return 0;}

	virtual int			scopeIsActive(void){return 0;}
	virtual	void		setBloodyWeapon(int isBloody){}
	virtual int			inDisguise(void){return 0;}
	virtual void		becomeDisguised(void){}

	virtual int			rulesDoWeaponsUseAmmo(void){return 0;}
	virtual void		rulesSetWeaponsUseAmmo(int useAmmo){}
	virtual	int			rulesCanDropWeapons(void){return 0;}
	virtual void		rulesSetDropWeapons(int dropWeapons){}
	virtual int			rulesGetNonDroppableWeaponTypes(void){return 0;}
	virtual void		rulesSetNonDroppableWeaponTypes(int nonDropMask){}
	virtual int			rulesDoWeaponsReload(void){return 0;}
	virtual void		rulesSetWeaponsReload(int weaponsReload){}
	virtual	int			rulesIsWeaponReloadAutomatic(void){return 0;}
	virtual void		rulesSetWeaponReloadAutomatic(int reloadAutomatic){}
	virtual int			rulesDoReloadEmptiesWeapon(void){return 0;}
	virtual void		rulesSetReloadEmptiesWeapon(int reloadEmptiesWeapon){}
	virtual char		*rulesGetBestWeaponMode(void){return "";}
	virtual void		rulesSetBestWeaponMode(char *bestWeaponMode){}
	virtual	int			rulesCanDropInvenOnDeath(void){return 0;}
	virtual	void		rulesSetDropInvenOnDeath(int dropInvenOnDeath){}
	virtual	int			rulesCanSelectWithNoAmmo(void){return 0;}
	virtual	void		rulesSetSelectWithNoAmmo(int selectWithNoAmmo){}
	virtual	int			rulesCanFreelySelectWeapon(void){return 0;}
	virtual	void		rulesSetFreelySelectWeapon(int freelySelectWeapon){}

	virtual int			removeCurrentWeapon(void){return 0;}
	virtual void		deactivateCurrentWeapon(void){}
	virtual void		deactivateInventory(void){}
	virtual int			countWeapons(void){return 0;}
	virtual void		setOwner(void *owner){}
	virtual void		dropAllWeapons(void){}

	virtual void		handleFall(void){}

	virtual int			extractInvFromMenu(int *weaponsAvailable, int *health){return 0;}
	virtual void		buildInvForMenu(int includeClipAmmo, int health){}

	virtual int			extractInvFromString(int *weaponsAvailable,char *invString){return(0);}
	virtual void		buildInvString(char *string,qboolean xtractWeapons,qboolean xtractAmmo,qboolean xtractItems,qboolean xtractArmor){}

	virtual void		clientClearRICs(void){}
	virtual void		clientReadRICs(void){}
	virtual void		clientProcessRICs(void){}
	virtual void		serverClearRICs(void){}
	virtual void		serverWriteRICs(void){}

	virtual void		setClientPredicting(int predicting){}
	virtual int			isClientPredicting(void){return 0;}

	virtual void		Write(void){}
	virtual void		Read(void){}
	virtual	void		NetRead(struct sizebuf_s *net_message_ptr){}
	virtual	void		NetWrite(int clientnum){}
};

#endif // __W_PUBLIC_H_