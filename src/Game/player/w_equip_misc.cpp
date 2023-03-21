#include "q_shared.h"
#include "..\ghoul\ighoul.h"
#include "w_public.h"
#include "w_types.h"
#include "w_weapons.h"
#include "w_utils.h"
#include "player.h"
#include "../gamecpp/game.h"
#include "w_equipment.h"

extern player_com_import_t	picom;
extern player_cl_import_t	picl;
extern player_sv_import_t	pisv;

extern int isClient;

//------------------------------------------------------------------------------------
#define GOGGLE_MAX_POWER 100
#define GOGGLE_DRAIN_TIME	0.3
#define GOGGLE_CHARGE_TIME	0.5

lightGogglesInfo::lightGogglesInfo(void):itemInfo_c(){}

void lightGogglesInfo::frameUpdate(sharedEdict_t &ent, itemSlot_c &slot)
{
	int power;
	float nextupdate;

	// Obviously we only care if the client has the goggles on.  If it isn't the client, it doesn't matter much.
	if (!isClient)
	{
		if (slot.getSlotStatus())
		{	// If the goggles are on.
			nextupdate = slot.getSlotUpdateTime();

			if (*(pisv.levelTime) > nextupdate)
			{
				power = slot.getSlotCount();
				power--;
				if (power<=0)
				{	// Out of power, turn the goggles off.
					slot.setSlotStatus(0);
					slot.setSlotCount(0);
					pisv.goggleuse(*(ent.edict), slot);
					return;		// Don't keep draining energy.
				}
				slot.setSlotCount(power);
				nextupdate = *(pisv.levelTime) + GOGGLE_DRAIN_TIME;
				slot.setSlotUpdateTime(nextupdate);
			}
		}
		else
		{	// The goggles are off.
			nextupdate = slot.getSlotUpdateTime();

			while (*(pisv.levelTime) > nextupdate)
			{
				power = slot.getSlotCount();
				if (power<GOGGLE_MAX_POWER)
				{
					power++;
					slot.setSlotCount(power);
				}
				nextupdate = *(pisv.levelTime) + GOGGLE_CHARGE_TIME;
				slot.setSlotUpdateTime(nextupdate);
			}
		}
	}
}

bool lightGogglesInfo::use(edict_t &ent, itemSlot_c &slot)
{
	if(!isClient)
	{
		if (slot.getSlotStatus())
		{
			slot.setSlotStatus(0);
			slot.setSlotUpdateTime(*(pisv.levelTime) + GOGGLE_CHARGE_TIME);
		}
		else
		{
			slot.setSlotStatus(1);
			slot.setSlotUpdateTime(*(pisv.levelTime) + GOGGLE_DRAIN_TIME);
		}
		// Let the server know that the goggles changed.

		pisv.goggleuse(ent, slot);
	}
	else
		;
	return true;
}

void lightGogglesInfo::deactivate(edict_t &ent, itemSlot_c &slot)
{
	if (!isClient)
	{
		slot.setSlotStatus(0);
		pisv.goggleuse(ent, slot);
	}

}

//------------------------------------------------------------------------------------

medkitInfo::medkitInfo(void):itemInfo_c(){}

void medkitInfo::frameUpdate(sharedEdict_t &ent, itemSlot_c &slot)
{
}

bool medkitInfo::use(edict_t &ent, itemSlot_c &slot)
{
	if(!isClient)
		return pisv.medkituse(ent, slot);
	else
		return true;
}
