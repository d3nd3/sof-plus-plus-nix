#include "g_local.h"

void CalcFireDirection(edict_t *self, vec3_t fwd, float waverVal, float maxWaver, attacks_e atkID);
void SetProjVel(edict_t &self, edict_t&newEnt, float fwd, float right, float up);
void FireServer(sharedEdict_t &sh, edict_t &ent, inven_c &inven);
void AltfireServer(sharedEdict_t &sh, edict_t &ent, inven_c &inven);
void ShowReload(edict_t *ent);//in p_body
void WeaponDropServer(edict_t *ent,int type, int clipSize);
bool DoWeHaveTheFlag(edict_t *ent);
void ShowItemUse(edict_t *ent, int itemType);//in p_body
void ItemDropServer(edict_t *ent,int type,int ammmoCount);

bool flashpackUse(edict_t &ent, itemSlot_c &slot);
bool neuralGrenadeUse(edict_t &ent, itemSlot_c &slot);
bool c4use(edict_t &ent, itemSlot_c &slot);
bool fireGrenade(weaponFireInfo_t &wf);
bool fireMolitov(weaponFireInfo_t &wf);
bool claymoreUse(edict_t &ent, itemSlot_c &slot);
bool medkitUse(edict_t &ent, itemSlot_c &slot);
bool grenadeUse(edict_t &ent, itemSlot_c &slot);
bool goggleUse(edict_t &ent, itemSlot_c &slot);

void CacheAttack(int atkID);

float getFireEvent(edict_t *ent);
void clearFireEvent(edict_t *ent);
float getAltfireEvent(edict_t *ent);
void clearAltfireEvent(edict_t *ent);

/*
===============
Game_ShutdownPlayer

Called when either the entire server is being killed, or it is changing to a
different game directory.
===============
*/

void Game_ShutdownPlayer (void)
{
	if (!pe)
	{
		return;
	}

	pe->Shutdown ();
	gi.Sys_UnloadPlayer (0);
	pe = NULL;
}

/*
===============
Game_InitPlayer

Init the player subsystem for a new map.
===============
*/

int *Game_InitPlayer(void)
{
	player_com_import_t	picom;
	player_sv_import_t	pisv;

	if (pe)
	{
		Game_ShutdownPlayer();
	}

	picom.Cvar_Get=gi.cvar;
	picom.Cvar_Set=gi.cvar_set;
	picom.Cvar_SetValue=gi.cvar_setvalue;

	picom.Cmd_Argc=gi.argc;
	picom.Cmd_Argv=gi.argv;
	picom.Cmd_Args=gi.args;
	
	picom.Com_Printf = Com_Printf;
	picom.Com_Sprintf = Com_sprintf;
	picom.Com_DPrintf = gi.dprintf;

	picom.FS_LoadFile=gi.FS_LoadFile;
	picom.FS_FreeFile=gi.FS_FreeFile;
	
	picom.irand = gi.irand;

	picom.ghl_specular=ghl_specular;
	picom.FindGSQFile=gi.FindGSQFile;
	picom.ReadGsqEntry=gi.ReadGsqEntry;
	picom.PrecacheGSQFile=gi.PrecacheGSQFile;
	picom.RegisterGSQSequences=gi.RegisterGSQSequences;
	picom.TurnOffPartsFromGSQFile=gi.TurnOffPartsFromGSQFile;

	pisv.GetGhoul=gi.GetGhoul;
	pisv.levelTime=&level.time;
	pisv.FireHelper=&FireServer;
	pisv.AltfireHelper=&AltfireServer;
	pisv.ShowReload=&ShowReload;
	pisv.ShowItemUse=&ShowItemUse;
	pisv.WeaponDrop=&WeaponDropServer;
	pisv.DoWeHaveTheFlag=&DoWeHaveTheFlag;
	pisv.getFireEvent=getFireEvent;
	pisv.clearFireEvent=clearFireEvent;
	pisv.getAltfireEvent=getAltfireEvent;
	pisv.clearAltfireEvent=clearAltfireEvent;
	pisv.ItemDrop=&ItemDropServer;
	pisv.EffectIndex=gi.effectindex;
	pisv.CvarInfo = gi.cvar_info;
	pisv.SP_Print = gi.SP_Print;
	pisv.SP_GetStringText = gi.SP_GetStringText;
	pisv.SoundIndex=gi.soundindex;
	pisv.Sound=gi.sound;
	pisv.AddCommand = gi.AddCommandString;
	pisv.flashpackUse=&flashpackUse;
	pisv.neuralGrenadeUse=&neuralGrenadeUse;
	pisv.c4use=&c4use;
	pisv.claymoreuse=&claymoreUse;
	pisv.medkituse=&medkitUse;
	pisv.grenadeuse=&grenadeUse;
	pisv.goggleuse=&goggleUse;
	pisv.WriteByteSizebuf=gi.WriteByteSizebuf;
	pisv.WriteShortSizebuf=gi.WriteShortSizebuf;
	pisv.WriteLongSizebuf=gi.WriteLongSizebuf;
	pisv.ReliableWriteByteToClient=gi.ReliableWriteByteToClient;
	pisv.ReliableWriteDataToClient=gi.ReliableWriteDataToClient;
	pisv.SZ_Init=gi.SZ_Init;
	pisv.SZ_Clear=gi.SZ_Clear;
	pisv.SZ_Write=gi.SZ_Write;
	pisv.AppendToSavegame = gi.AppendToSavegame;
	pisv.ReadFromSavegame = gi.ReadFromSavegame;
	pisv.CacheAttack = CacheAttack;

	pe = (player_export_t *)gi.Sys_GetPlayerAPI((void *)&picom,(void *)NULL,(void *)&pisv,0);

	if (!pe)
	{
		gi.error("Game failed to load player DLL");
	}
	if (pe->api_version != PLAYER_API_VERSION)
	{
		gi.error("player is version %i, not %i", pe->api_version,PLAYER_API_VERSION);
	}

	pe->Init();

	gi.dprintf ("==== InitPlayer Complete ====\n");

	return(pe->isClient);
}

// end