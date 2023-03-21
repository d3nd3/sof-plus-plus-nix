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

//none of these do anything...
flashpackInfo::flashpackInfo(void):itemInfo_c()
{
}

neuralGrenadeInfo::neuralGrenadeInfo(void):itemInfo_c()
{
}

c4Info::c4Info(void):itemInfo_c()
{
}

claymoreInfo::claymoreInfo(void):itemInfo_c()
{
}

grenadeInfo::grenadeInfo(void):itemInfo_c()
{
}

flagInfo::flagInfo(void):itemInfo_c()
{
}


//------------------------------------------------------------------------------------

bool flashpackInfo::use(edict_t &ent, itemSlot_c &slot)
{
	if(!isClient)
		return pisv.flashpackUse(ent,slot);
	else
		return true;
}

//------------------------------------------------------------------------------------

bool neuralGrenadeInfo::use(edict_t &ent, itemSlot_c &slot)
{
	if(!isClient)
		return pisv.neuralGrenadeUse(ent,slot);
	else
		return true;
}

//------------------------------------------------------------------------------------

bool c4Info::use(edict_t &ent, itemSlot_c &slot)
{
	if(!isClient)
		return pisv.c4use(ent,slot);
	else
		return true;
}


//------------------------------------------------------------------------------------

bool claymoreInfo::use(edict_t &ent, itemSlot_c &slot)
{
	if(!isClient)
		return pisv.claymoreuse(ent, slot);
	else
		return true;
}

//------------------------------------------------------------------------------------

bool grenadeInfo::use(edict_t &ent, itemSlot_c &slot)
{
	if(!isClient)
		return pisv.grenadeuse(ent, slot);
	else
		return true;
}

