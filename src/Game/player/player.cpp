// player.cpp : Defines the entry point for the DLL application.
//

#include "q_shared.h"
#include "..\ghoul\ighoul.h"
#include "w_public.h"
#include "w_types.h"
#include "w_weapons.h"
#include "w_utils.h"
#include "player.h"

player_com_import_t	picom;
player_cl_import_t	picl;
player_sv_import_t	pisv;
player_export_t	pe;

int isClient;

extern inven_c *W_NewInv(void);
extern void W_KillInv(inven_c *inven);
extern void RefreshWeapon(inven_c *inven);

extern weaponInfo_c *weapInfo[SFW_NUM_WEAPONS];
extern int fireEvent;
extern int altfireEvent;

qboolean P_Init(void)
{	
	return(true);
}

void P_Shutdown(void)
{
}

void InitExports(void)
{
	pe.api_version = PLAYER_API_VERSION;

	pe.Init=P_Init;
	pe.Shutdown=P_Shutdown;
	pe.isClient=&isClient;
	pe.weapInfo=(void **)weapInfo;
	pe.fireEvent=&fireEvent;
	pe.altfireEvent=&altfireEvent;
	pe.PrecacheViewWeaponModels=W_PrecacheViewWeaponModels;
	pe.UncacheViewWeaponModels=W_UncacheViewWeaponModels;
	pe.NewInv=W_NewInv;
	pe.KillInv=W_KillInv;
	pe.RefreshWeapon=RefreshWeapon;
}

player_export_t *GetPlayerClientAPI(player_com_import_t *pimpcom,player_cl_import_t *pimpcl)
{
	picom=*pimpcom;
	picl=*pimpcl;

	InitExports();

	return(&pe);
}

player_export_t *GetPlayerServerAPI(player_com_import_t *pimpcom,player_sv_import_t *pimpsv)
{
	picom=*pimpcom;
	pisv=*pimpsv;

	InitExports();

	return(&pe);
}