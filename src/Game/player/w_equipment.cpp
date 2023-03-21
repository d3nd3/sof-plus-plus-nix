#include "q_shared.h"
#include "..\ghoul\ighoul.h"
#include "w_public.h"
#include "w_types.h"
#include "w_weapons.h"
#include "w_utils.h"
#include "player.h"
#include "../gamecpp/game.h"
#include "w_equipment.h"
#include "../strings/items.h"


extern player_com_import_t	picom;
extern player_cl_import_t	picl;
extern player_sv_import_t	pisv;
extern int isClient;

itemInfo_c			noItem;
flashpackInfo		flashpack;
//neuralGrenadeInfo	neuralGrenade;
c4Info				c4;
lightGogglesInfo	lightGoggles;
claymoreInfo		claymore;
medkitInfo			medkit;
grenadeInfo			grenade;
flagInfo			flag;

itemInfo_c *itInfo[SFE_NUMITEMS] =
{
	&noItem,
	&flashpack,
//	&neuralGrenade,
	&c4,
	&lightGoggles,
	&claymore,
	&medkit,
	&grenade,
	&flag,
};

//------------------------------------------------------------------------------------

itemSlot_c::itemSlot_c(void)
{
	clear();
}

void itemSlot_c::useAmmo(sharedEdict_t &ent)
{
	// 1/10/00 kef -- the light goggles are different. annoying, too.
	if (itemType == SFE_LIGHT_GOGGLES)
	{
		// since lightGogglesInfo::frameUpdate() will take care of the current power (ammo) for the
		//goggles, don't update ammo here.
		if (ammo-1 <= 0)
		{
			int nBreak =1 ;
		}
		// kef -- Eric said get rid of this message for the light goggles
		/*
		if(!isClient)
		{
			pisv.SP_Print(ent.edict, ITEMS_AMMO_COUNT, ammo, itInfo[itemType]->getName());
		}
		*/
		return;
	}

	ammo--;
	if(ammo <= 0)
	{
		if(!isClient)
			pisv.SP_Print(ent.edict, ITEMS_AMMO_GONE, itInfo[itemType]->getName());

		itemType = SFE_EMPTYSLOT;
		status = 0;
		inven(ent)->getItems()->setNextCommand(PEC_ITEMNEXT);
	}
	else
	{
		if(!isClient)
			pisv.SP_Print(ent.edict, ITEMS_AMMO_COUNT, ammo, itInfo[itemType]->getName());
	}
}

void itemSlot_c::clear(void)
{
	itemType = SFE_EMPTYSLOT;
	ammo = 0;
	status = 0;
	nextupdate=0.0;
}

//------------------------------------------------------------------------------------

item_c::item_c(void)
{
	curSlotNum = 0;
	nextCommand = PEC_NOCOMMAND;
	lastUseTime = 0;
}

int	item_c::getSlotNumFromName(char *itemName)
{
	int type=-1;

	if(!stricmp (itemName,"fpak"))
		type=SFE_FLASHPACK;
	else if(!stricmp (itemName,"c4"))
		type=SFE_C4;
	else if(!stricmp (itemName,"goggles"))
		type=SFE_LIGHT_GOGGLES;
	else if(!stricmp (itemName,"medkit"))
		type=SFE_MEDKIT;
	else if(!stricmp (itemName,"grenade"))
		type=SFE_GRENADE;
	else if(!stricmp (itemName,"ctf_flag"))
		type=SFE_CTFFLAG;

	if(type<0)
		return(-2);

	for(int i = 0; i < MAXITEMS; i++)
		if(slots[i].getSlotType() == type)
			return(i);
	
	return(-1);
}

int item_c::addItemType(int type, int amount, int maxAmount)
{
	int i;

	for(i = 0; i < MAXITEMS; i++)
	{	
		if(slots[i].getSlotType() == type)
		{
			// Do some tests to see if this many can actually be held.
			if (maxAmount != -1) // Do we have a defined maximum?
			{
				if ((slots[i].getSlotCount() + amount) > maxAmount) // we can't pick up ALL of these
				{
					if(slots[i].getSlotCount() < maxAmount) // but we can pick up SOME of them
					{
						slots[i].setSlotCount(maxAmount);
						return 1;
					}
					else // nope, no room at the inn, we're full
					{
						return 0;
					}
				}
			}

			slots[i].setSlotCount(slots[i].getSlotCount() + amount);
			return 1;
		}
	}
	for(i = 0; i < MAXITEMS; i++)
	{	
		// Okay, add it to an empty slot then.


		if(slots[i].getSlotType() == SFE_EMPTYSLOT)
		{
			slots[i].setSlotType(type);

			// Do some tests to see if this many can actually be held. In this case, we're checking in case for SOME reason
			// we don't have ANY of an item, but the item pickup actually has MORE than we are allowed to carry.  Proper design
			// should render this test unnecessary, but...

			if (maxAmount != -1) // Do we have a defined maximum?
			{
				if ((slots[i].getSlotCount() + amount) > maxAmount) // we can't pick up ALL of these
				{
					if(slots[i].getSlotCount() < maxAmount) // but we can pick up SOME of them
					{
						slots[i].setSlotCount(maxAmount);
						return 1;
					}
					else // nope, no room at the inn, we're full
					{
						return 0;
					}
				}
			}
			
			slots[i].setSlotCount(amount);
			return 1;
		}
	}
	return 0;
}

int	item_c::hasItemType(int type)
{
	for(int i = 0; i < MAXITEMS; i++)
	{	
		if(slots[i].getSlotType() == type)
			return 1;
	}

	return(0);
}

void item_c::HandleInput(sharedEdict_t &ent)
{
	int startSlot;

	if(nextCommand == PEC_NOCOMMAND)
		return;

	startSlot = curSlotNum;

	if(nextCommand == PEC_DROP)
	{
		if(getCurSlot()->getSlotType()!=SFE_EMPTYSLOT)
		{
			// Handle deactivation of item, where required.
			
			itInfo[getCurSlot()->getSlotType()]->deactivate(*(ent.edict), *getCurSlot());

			// Some items like the light-goggles are unique in the inventory but
			// have ammo / charge so we need to ensure the dropped item has this much ammo.
			// Other items like flahsbangs exist in multiples and we just drop one at a time.

			int amount=(itInfo[getCurSlot()->getSlotType()]->itemExistsAsUnique())?getCurSlot()->getSlotCount():1;

			// Drop an in-world entity.

			if(!isClient)
				pisv.ItemDrop(inven(ent)->getOwner()->edict,getCurSlot()->getSlotType(),amount);
		
			if(itInfo[getCurSlot()->getSlotType()]->itemExistsAsUnique())
			{
				// Claer the item slot 'cos we just dropped a unique item (e.g. light-goggles).

				getCurSlot()->clear();

				// Select the next item slot.

				nextCommand = PEC_ITEMNEXT;
			}
			else
			{
				// Decrement the ammo count for the slot by 1 as we just dropped one :).

				getCurSlot()->useAmmo(ent);
			}

		}
	}
	
	if((nextCommand >= PEC_USEITEM)&&(nextCommand < PEC_USEITEMFIRST + MAXITEMS))
	{
		if(!isClient)
		{
			if(lastUseTime + ITEM_USE_TIME > *pisv.levelTime)
			{	
				// Can't mash on the item_c use button.

				return;
			}
		}
		else
		{
			if(lastUseTime + ITEM_USE_TIME > *picl.levelTime)
			{	
				// Can't mash on the item_c use button.

				return;
			}
		}

		itemSlot_c *slot;

		if(nextCommand == PEC_USEITEM)
		{
			slot=getCurSlot();
		}
		else
		{
			slot=&slots[nextCommand-PEC_USEITEMFIRST];
		}

		if(slot->getSlotCount())
		{
			if(ent.inv->inDisguise())
			{
				// If we try to use anything other than medkit whilst disguised,
				// we toggle outa disguise!

				if(slot->getSlotType() != SFE_MEDKIT)
					ent.inv->becomeDisguised();
			}

			// Can't use the CTF FLAG!!
			if (slot->getSlotType() == SFE_CTFFLAG)
				return;

			if (!isClient)
				pisv.ShowItemUse(ent.edict, slot->getSlotType());

			int lastCmd=nextCommand;

			if (itInfo[slot->getSlotType()]->use(*(ent.edict), *slot))
				slot->useAmmo(ent);

			if((nextCommand==PEC_ITEMNEXT)&&(lastCmd>PEC_USEITEM)&&(slot!=getCurSlot()))
			{
				// PEC_ITEMNEXT can be issued if all slot's ammo was used. Don't
				// want this though if a shortcut was used and it wasn't the current
				// slot.

				nextCommand=PEC_NOCOMMAND;
			}
		}

		if(!isClient)
			lastUseTime = *pisv.levelTime;
		else
			lastUseTime = *picl.levelTime;
	}
	
	if(nextCommand == PEC_ITEMNEXT)
	{
		do
		{
			curSlotNum++;
			if(curSlotNum >= MAXITEMS)
			{
				curSlotNum = 0;
			}
		}while((curSlotNum != startSlot)&&(getCurSlot()->getSlotType() == SFE_EMPTYSLOT));
	}
	else if(nextCommand == PEC_ITEMPREV)
	{
		do
		{
			curSlotNum--;
			if(curSlotNum < 0)
			{
				curSlotNum = MAXITEMS - 1;
			}
		}while((curSlotNum != startSlot)&&(getCurSlot()->getSlotType() == SFE_EMPTYSLOT));
	}

	nextCommand = PEC_NOCOMMAND;
}

void item_c::frameUpdate(sharedEdict_t &ent)
{
	int i;

	HandleInput(ent);

	for(i = 0; i < MAXITEMS; i++)
	{
		itInfo[slots[i].getSlotType()]->frameUpdate(ent, slots[i]);
	}
}

int item_c::handleDamage(sharedEdict_t &ent, int initDamage)
{
	int i;
	int curDamage = initDamage;

	for(i = 0; i < MAXITEMS; i++)
	{	
		// FIXME: Will this change?

		curDamage = itInfo[slots[i].getSlotType()]->handleDamage(ent, curDamage, slots[i]);
	}
	return curDamage;
}

void item_c::deactivate(sharedEdict_t &ent)
{
	for(int i = 0; i < MAXITEMS; i++)
	{
		itInfo[slots[i].getSlotType()]->deactivate(*(ent.edict), *getCurSlot());		
	}
}
