#ifndef __W_EQUIPMENT_H_
#define __W_EQUIPMENT_H_

#include "w_weapons.h"
#include "../strings/items.h"

class itemInfo_c
{
private:
public:
	itemInfo_c(void){}
	~itemInfo_c(void){}

	virtual void	setupModelInfo(void){};
	virtual void	frameUpdate(sharedEdict_t &ent, itemSlot_c &slot){}
	virtual bool	use(edict_t &ent, itemSlot_c &slot){return true;}
	virtual void	deactivate(edict_t &ent, itemSlot_c &slot){}
	virtual int		handleDamage(sharedEdict_t &ent, int curDamage, itemSlot_c &slot){return curDamage;}
	virtual char	*getName(void){return "";}
	virtual int		getNameIndex(void){return 0;}
	virtual int		itemExistsAsUnique(void){return 0;}
};

extern itemInfo_c *itInfo[SFE_NUMITEMS];

//------------------------------------------------------------------------------------
//
//							Item Stuff
//
//------------------------------------------------------------------------------------

class flashpackInfo : public itemInfo_c
{
private:
public:
			flashpackInfo(void);
	bool	use(edict_t &ent, itemSlot_c &slot);
	char	*getName(void){return "Flash Packs";} // naughty!  should have been string packaged!
	int		getNameIndex(void){return ITEMS_FLASHPACK;}
};

class neuralGrenadeInfo : public itemInfo_c
{
private:
public:
			neuralGrenadeInfo(void);
	bool	use(edict_t &ent, itemSlot_c &slot);
	char	*getName(void){return "Neural Pulse Grenades";}
	int		getNameIndex(void){return ITEMS_NEURAL_GRENADE;}
};

class c4Info : public itemInfo_c
{
private:
public:
			c4Info(void);
	bool	use(edict_t &ent, itemSlot_c &slot);
	char	*getName(void){return "C4";}
	int		getNameIndex(void){return ITEMS_C4;}

};

class lightGogglesInfo : public itemInfo_c
{
private:
public:
			lightGogglesInfo(void);
	void	frameUpdate(sharedEdict_t &ent, itemSlot_c &slot);
	bool	use(edict_t &ent, itemSlot_c &slot);
	void	deactivate(edict_t &ent, itemSlot_c &slot);
	char	*getName(void){return "Light Amplification Goggles";}
	int		itemExistsAsUnique(void){return 1;}
	int		getNameIndex(void){return ITEMS_LIGHT_GOGGLES;}

};

class claymoreInfo : public itemInfo_c
{
private:
public:
	claymoreInfo(void);
	bool use(edict_t &ent, itemSlot_c &slot);
	char *getName(void){return "Claymore";}
	int		getNameIndex(void){return ITEMS_CLAYMORE;}

};

class medkitInfo : public itemInfo_c
{
private:
public:
	medkitInfo(void);
	void frameUpdate(sharedEdict_t &ent, itemSlot_c &slot);
	bool use(edict_t &ent, itemSlot_c &slot);
	char *getName(void){return "Medkit";}
	int		getNameIndex(void){return ITEMS_MEDKIT;}
};

class grenadeInfo : public itemInfo_c
{
private:
public:
	grenadeInfo(void);
	bool use(edict_t &ent, itemSlot_c &slot);
	char *getName(void){return "Grenade";}
	int		getNameIndex(void){return ITEMS_GRENADE;}
};

class flagInfo : public itemInfo_c
{
private:
public:
	flagInfo(void);
	bool use(edict_t &ent, itemSlot_c &slot){return true;}
	char *getName(void){return "Flag";}
	int itemExistsAsUnique(void){return 1;}
	int		getNameIndex(void){return ITEMS_CTF_FLAG;}
};



#endif // __W_EQUIPMENT_H_