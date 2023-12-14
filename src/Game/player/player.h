#ifndef __PLAYER_H
#define __PLAYER_H

#include "../qcommon/GSQFile.h"
#include "../qcommon/qcommon.h"

#define	PLAYER_API_VERSION		1

typedef struct edict_s edict_t;
typedef struct sharedEdict_s sharedEdict_t;
class inven_c;
class itemSlot_c;

//
// These are the functions exported by the player dll module.
//

typedef struct
{
	// If api_version is different, the dll cannot be used.

	int			api_version;

	//

	int			*isClient;

	// Called when the dll is loaded / unloaded.

	qboolean	(*Init)(void);
	void		(*Shutdown)(void);

	// Pointer to weapInfo array.

	void		**weapInfo;

	// Fire event pointers.

	int			*fireEvent;
	int			*altfireEvent;

	// View weapon model caching. Should only ever be called from server.

	void		(*PrecacheViewWeaponModels)(int types);
	void		(*UncacheViewWeaponModels)(void);

	// Creates a new inventory.

	inven_c		*(*NewInv)(void);
	void		(*KillInv)(inven_c *inven);
	void		(*RefreshWeapon)(inven_c *inven);
} player_export_t;

//
// These are the functions imported by the player dll module when it is called
// from the client or server.
//

typedef struct
{ 
	// Cvar stuff.

	cvar_t	*(*Cvar_Get)(const char *name, const char *value, int flags, cvarcommand_t command = NULL);
	cvar_t	*(*Cvar_Set)(const char *name, const char *value );
	void	(*Cvar_SetValue)(const char *name, float value );

	// Command processing.

	int		(*Cmd_Argc) (void);
	char	*(*Cmd_Argv) (int i);
	char	*(*Cmd_Args) (void);

	// Output to console, etc.

	void	(*Com_Printf) (char *str, ...);
	void	(*Com_Sprintf) (char *dest, int size, char *fmt, ...);
	void 	(*Com_DPrintf) (char *fmt, ...);

	// File handling.

	int		(*FS_LoadFile) (char *name, void **buf, bool OverridePak = false);
	void	(*FS_FreeFile) (void *buf);
	
	// Misc

	int		(*irand)(int min, int max);

	// Ghoul stuff.

	cvar_t	*ghl_specular;
	IPlayerModelInfoC *(*NewPlayerModelInfo)(char *modelname);
	int		(*FindGSQFile)(char *gsqdir, char *gsqfile, void **buffer);
	bool	(*ReadGsqEntry)(int &filesize, char **tdata, char *seqname);
	void	(*PrecacheGSQFile)(char *dirname, char *gsq_file, IGhoulObj *object);
	int		(*RegisterGSQSequences)(char *gsqdir, char *subclass, IGhoulObj *object);
	void	(*TurnOffPartsFromGSQFile)(char *dirname, char *poff_file, IGhoulObj *this_object, IGhoulInst *this_inst);
} player_com_import_t;

//
// These are the functions imported by the player dll module when it is called
// from the client.
//

typedef struct
{ 
	// So we can grab the Ghoul pointer we need so badly.

	void	*(*GetGhoul)();

	// Getting level.time for weapon prediction.

	float	*levelTime;

	// Helper functions for weapon firing.

	void	(*FireHelper)(sharedEdict_t &sh, edict_t &ent, inven_c &inven);
	void	(*AltfireHelper)(sharedEdict_t &sh, edict_t &ent, inven_c &inven);
	void	(*WeapSoundHelper)(const void *data);
	void	(*WeapSoundKill)(void);
	void	(*WeaponEffectHelper)(const void *data);

	// Network stuff.

	int		(*ReadByte)(void);
	int		(*ReadShort)(void);
	int		(*ReadLong)(void);
	int		(*ReadByteSizebuf)(sizebuf_t *sz);
	int		(*ReadShortSizebuf)(sizebuf_t *sz);
	int		(*ReadLongSizebuf)(sizebuf_t *sz);
	void	(*ReadDataSizebuf)(sizebuf_t *sz, unsigned char *addr, int len);
	void	(*WriteByteSizebuf)(sizebuf_t *sz,int b);
	void	(*WriteShortSizebuf)(sizebuf_t *sz,int s);
	void	(*WriteLongSizebuf)(sizebuf_t *sz,int i);

	// Sizebuf stuff.

	void	(*SZ_Init)(sizebuf_t *buf, byte *data, int length);
	void	(*SZ_Clear)(sizebuf_t *buf);
	void	(*SZ_Write)(sizebuf_t *buf, const void *data, int length);
} player_cl_import_t;

//
// These are the functions imported by the player dll module when it is called
// from the server.
//

typedef struct
{ 
	// So we can grab the Ghoul pointer we need so badly.

	void	*(*GetGhoul)();

	// Getting level.time for weapon prediction.

	float	*levelTime;
	
	// Helper functions for weapon firing, reloading etc.

	void	(*FireHelper)(sharedEdict_t &sh, edict_t &ent, inven_c &inven);
	void	(*AltfireHelper)(sharedEdict_t &sh, edict_t &ent, inven_c &inven);
	void	(*ShowReload)(edict_t *ent);
	void	(*WeaponDrop)(edict_t *ent,int type, int clipSize);
	void	(*ShowItemUse)(edict_t *ent, int itemType);
	void	(*ItemDrop)(edict_t *ent,int type,int ammoCount);

	// Helper functions for caching

	void	(*CacheAttack)(int atkID);

	// Helper functions for sound stuff.

	int		(*EffectIndex)(const char *name);
	int		(*SoundIndex)(const char *name);
	void	(*Sound)(edict_t *ent, int channel, int soundindex, float volume, float attenuation, float timeofs, int localize);

	// Helper functions for ctf sound stuff.

	bool	(*DoWeHaveTheFlag)(edict_t *ent);

	// Weapon fire synching shit.

	float	(*getFireEvent)(edict_t *ent);
	void	(*clearFireEvent)(edict_t *ent);
	float	(*getAltfireEvent)(edict_t *ent);
	void	(*clearAltfireEvent)(edict_t *ent);

	// Console stuff, used for level changning

	void	(*AddCommand)(const char *name);
	char	*(*CvarInfo)(int flag);

	// Output to console, etc.

	void		(*SP_Print)(edict_t *ent, unsigned short ID, ...);
	const char	*(*SP_GetStringText)(unsigned short ID);
//	void	(*SP_ColorPrint)(char *color, edict_t *ent, unsigned short ID, ...);

	// Helper functions for equipment useage.

	bool	(*flashpackUse)(edict_t &ent, itemSlot_c &slot);
	bool	(*neuralGrenadeUse)(edict_t &ent, itemSlot_c &slot);
	bool	(*c4use)(edict_t &ent, itemSlot_c &slot);
	bool	(*claymoreuse)(edict_t &ent, itemSlot_c &slot);
	bool	(*medkituse)(edict_t &ent, itemSlot_c &slot);
	bool	(*grenadeuse)(edict_t &ent, itemSlot_c &slot);
	bool	(*goggleuse)(edict_t &ent, itemSlot_c &slot);
	
	// Network stuff.

	void	(*WriteByteSizebuf)(sizebuf_t *sz,int c);
	void	(*WriteShortSizebuf)(sizebuf_t *sz,int s);
	void	(*WriteLongSizebuf)(sizebuf_t *sz,int i);
	void	(*ReliableWriteByteToClient)(byte b,int clientNum);
	void	(*ReliableWriteDataToClient)(const void *data,int length,int clientNum);

	// Sizebuf stuff.

	void	(*SZ_Init)(sizebuf_t *buf, byte *data, int length);
	void	(*SZ_Clear)(sizebuf_t *buf);
	void	(*SZ_Write)(sizebuf_t *buf, const void *data, int length);

	// Savegame stuff

	qboolean	(*AppendToSavegame)(unsigned long chid, void *data, int length);
	int			(*ReadFromSavegame)(unsigned long chid, void *address, int length, void **addressptr = NULL);
} player_sv_import_t;

//
// This is the only function actually exported at the linker level.
//

typedef	player_export_t	(*GetPlayerAPI_t) (player_com_import_t*,player_cl_import_t*,player_sv_import_t*);

#endif // __PLAYER_H