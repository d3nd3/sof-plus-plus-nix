#ifndef _GAME_H_
#define _GAME_H_

// game.h -- game dll information visible to server

#include "../qcommon/GSQFile.h"
#include "../qcommon/snd_common.h"

#define	GAME_API_VERSION	3

// edict->svflags

#define	SVF_NOCLIENT			0x00000001	// don't send entity to clients, even if it has effects
#define	SVF_DEADMONSTER			0x00000002	// treat as CONTENTS_DEADMONSTER for collision
#define	SVF_MONSTER				0x00000004	// treat as CONTENTS_MONSTER for collision
#define	SVF_DOORHACK_X			0x00000008	// kef --	you've got to believe me when I say this is better 
#define	SVF_DOORHACK_Y			0x00000010	//			than the alternative
#define	SVF_DOORHACK_Z			0x00000020	//
#define	SVF_ISHIDING			0x00000040	// the entity is hiding in shadows
#define SVF_BUDDY				0x00000080	// LeanBuddy(TM) and NugBuddy(TM) use this to get shot
#define SVF_ALWAYS_SEND			0x00000100	// always transmit to the client, even if out of PVS
#define SVF_FAKE_CLIENT			0x00000200	// not a real connection, so ignore any network writes


// edict->solid values

typedef enum
{
SOLID_NOT,			// no interaction with other objects
SOLID_TRIGGER,		// only touch when inside, after moving
SOLID_BBOX,			// touch on edge
SOLID_BSP,			// bsp clip, touch on edge
SOLID_GHOUL,		// use ghoul BoxTrace
SOLID_CORPSE,		// solid type for humanoid corpses only
//
SOLID_NOMATCH = -1	// special value initial ent prev-solid field to ensure non-match, DO NOT USE ELSEWHERE!!!!
} solid_t;

//===============================================================

// link_t is only used for entity area links now
typedef struct link_s
{
	struct link_s	*prev, *next;
} link_t;

#define	MAX_ENT_CLUSTERS	64


typedef struct edict_s edict_t;
typedef struct gclient_s gclient_t;
typedef struct sharedEdict_s sharedEdict_t;
class inven_c;
class itemSlot_c;

#ifndef GAME_INCLUDE

struct gclient_s
{
	player_state_t	ps;		// communicated by server to clients
	int				ping;
	// the game dll can add anything it wants after
	// this point in the structure
};


struct edict_s
{
	entity_state_t	s;
	struct gclient_s	*client;
	qboolean	inuse;
	int			linkcount;

	// FIXME: move these fields to a server private sv_entity_t
	link_t		area;				// linked to a division node or leaf
	
	int			num_clusters;		// if -1, use headnode instead
	short		clusternums[MAX_ENT_CLUSTERS];
	int			headnode;			// unused if num_clusters != -1
	int			areanum, areanum2;

	//================================

	int			svflags;			// SVF_NOCLIENT, SVF_DEADMONSTER, SVF_MONSTER, etc
	vec3_t		mins, maxs;
	vec3_t		absmin, absmax, size;
	solid_t		solid;
	int			clipmask;
	edict_t		*owner;

	/// GHOUL
	void		*ghoulInst;

	// new fields for checking if origin or mins/maxs have changed, and therefore should we relink entity?
	vec3_t		prev_mins,prev_maxs;
	vec3_t		prev_origin;
	solid_t		prev_solid;
	vec3_t		prev_angles;

	// the game dll can add anything it wants after
	// this point in the structure
};

#endif		// GAME_INCLUDE

//===============================================================


//
// functions provided by the main engine
//
typedef struct
{
	// The *index functions create configstrings and some internal server state.
	
	int		(*modelindex) (const char *name);
	int		(*soundindex) (const char *name);
	int		(*effectindex) (const char *name);
	int		(*imageindex) (const char *name);

	void	(*unload_sound) (const char *name);
	qboolean (*FilterPacket) (char *from);

	void	(*CreateGhoulConfigStrings) (void);

	void	(*setmodel) (edict_t *ent, char *name);
	void	(*setrendermodel) (edict_t *ent, char *name);

	// ClientCommand parameter access.

	int		(*argc) (void);
	char	*(*argv) (int n);
	char	*(*args) (void);	// concatenation of all argv >= 1

	// Special messages.

	void	(*bprintf) (int printlevel, char *fmt, ...);
	void	(*dprintf) (char *fmt, ...);
	void	(*cprintf) (edict_t *ent, int printlevel, char *fmt, ...);
	void	(*clprintf) (edict_t *ent, edict_t *from, int color, char *fmt, ...);
	void	(*welcomeprint) (edict_t *ent);
	void	(*centerprintf) (edict_t *ent, char *fmt, ...);
	void	(*cinprintf) (edict_t *ent,int x, int y, int textspeed, char *text);
	void	(*bcaption) (int printlevel, unsigned short ID);
	void	(*captionprintf) (edict_t *ent, unsigned short ID);
	void	(*Con_ClearNotify) (void);

	void	(*sound) (edict_t *ent, int channel, int soundindex, float volume, float attenuation, float timeofs, int localize=SND_LOCALIZE_GLOBAL);
	void	(*positioned_sound) (vec3_t origin, edict_t *ent, int channel, int soundinedex, float volume, float attenuation, float timeofs, int localize=SND_LOCALIZE_GLOBAL);

	// Misc. bullshit.

	void	(*DebugGraph) (float value, int color);
	bool	(*DamageTexture) (struct mtexinfo_s *surface, int amount);
	int		(*SurfaceTypeList) (byte *mat_list, int max_size);
	void	(*Update)(float percent, bool server);

	// Network messaging.

	void	(*multicast) (vec3_t origin, multicast_t to);
	void	(*multicastignore)(vec3_t origin, multicast_t to, int ignoreID);
	void	(*unicast) (edict_t *ent, qboolean reliable);
	void	(*WriteChar) (int c);
	void	(*WriteByte) (int c);
	void	(*WriteShort) (int c);
	void	(*WriteLong) (int c);
	void	(*WriteFloat) (float f);
	void	(*WriteString) (char *s);
	void	(*WritePosition) (vec3_t pos);	// some fractional bits
	void	(*WriteDir) (vec3_t pos);		// single byte encoded, very coarse
	void	(*WriteAngle) (float f);
	
	void	(*WriteByteSizebuf)(sizebuf_t *sz,int c);
	void	(*WriteShortSizebuf)(sizebuf_t *sz,int s);
	void	(*WriteLongSizebuf)(sizebuf_t *sz,int i);
	void	(*ReliableWriteByteToClient)(byte b,int clientNum);
	void	(*ReliableWriteDataToClient)(const void *data,int length,int clientNum);

	int		(*GetNearestByteNormal) (vec3_t dir);

	void	(*sendPlayernameColors)(edict_t *ent,int len,int numClients,byte *clientColors);


	// String Package

	void		(*SP_Register)(const char *Package);
	void		(*SP_Print)(edict_t *ent, unsigned short ID, ...);
	void		(*SP_Print_Obit)(edict_t *ent, unsigned short ID, ...);
	int			(*SP_SPrint)(char *buffer, int buffer_size, unsigned short ID, ...);
	const char	*(*SP_GetStringText)(unsigned short ID);

	// Collision detection.

	qboolean (*trace) (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passent, int contentmask, trace_t *trace);
	qboolean (*polyTrace) (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passent, int contentmask, trace_t *trace, int (*PolyHitFunc)(trace_t *tr, vec3_t start, vec3_t end, int clipMask));
	int		 (*pointcontents) (vec3_t point);
	float	 (*RegionDistance) (vec3_t vert);
	qboolean (*inPVS) (vec3_t p1, vec3_t p2);
	qboolean (*inPHS) (vec3_t p1, vec3_t p2);
	void	 (*SetAreaPortalState) (int portalnum, qboolean open);
	qboolean (*AreasConnected) (int area1, int area2);

	// Stuff needed by gameghoul.

	void	*(*GetGhoul)(void);
	IPlayerModelInfoC	*(*NewPlayerModelInfo)(char *modelname);
	int		(*FindGSQFile)(char *gsqdir, char *gsqfile, void **buffer);
	bool	(*ReadGsqEntry)(int &filesize, char **tdata, char *seqname);
	void	(*PrecacheGSQFile)(char *dirname, char *gsq_file, IGhoulObj *object);
	int		(*RegisterGSQSequences)(char *gsqdir, char *subclass, IGhoulObj *object);
	void	(*TurnOffPartsFromGSQFile)(char *dirname, char *poff_file, IGhoulObj *this_object, IGhoulInst *this_inst);

	int		**isClient;

	// Config strings hold all the index strings, the lightstyles, and misc data like
	// the sky definition and cdtrack. All of the current configstrings are sent to
	// clients when they connect, and changes are sent to all connected clients.

	void	(*configstring) (int num, char *string);

	// Sizebuf stuff.

	void	(*SZ_Init)(sizebuf_t *buf, byte *data, int length);
	void	(*SZ_Clear)(sizebuf_t *buf);
	void	(*SZ_Write)(sizebuf_t *buf, const void *data, int length);

	// Anyone care to comment on this?

	void	(*error) (char *fmt, ...);

	void	(*Sys_ConsoleOutput)(char *string);
	void	*(*Sys_GetPlayerAPI)(void *parmscom,void *parmscl,void *parmssv,int isClient);
	void	(*Sys_UnloadPlayer)(int isClient);

	float	(*flrand)(float min, float max);
	int		(*irand)(int min, int max);

	// An entity will never be sent to a client or used for collision if it is not
	// passed to linkentity.  If the size, position, or solidity changes, it must be relinked.

	void	(*linkentity) (edict_t *ent);
	void	(*unlinkentity) (edict_t *ent);		// call before removing an interactive edict
	int		(*BoxEdicts) (vec3_t mins, vec3_t maxs, edict_t **list,	int maxcount, int areatype);
	void	(*Pmove) (pmove_t *pmove);		// player movement code common with client prediction

	// Managed memory allocation.

	void	*(*TagMalloc) (int size, int tag);
	void	(*TagFree) (void *block);
	void	(*FreeTags) (int tag);

	// Savegame handling

	qboolean	(*AppendToSavegame)(unsigned long chid, void *data, int length);
	int			(*ReadFromSavegame)(unsigned long chid, void *address, int length, void **addressptr = NULL);
#if (!_FINAL_) && (_RAVEN_)
	char		*(*GetLabel)(void *addr);
#endif

	// Console variable interaction.

	cvar_t	*(*cvar) (const char *var_name, const char *value, int flags, cvarcommand_t command = NULL);
	cvar_t	*(*cvar_set) (const char *var_name, const char *value);
	void	(*cvar_setvalue) (const char *var_name, float value);
	cvar_t	*(*cvar_forceset) (const char *var_name, const char *value);
	char	*(*cvar_info)(int flag);
	float	(*cvar_variablevalue)(const char *var_name);

	// General purpose filesystem routines.

	int		(*FS_LoadFile) (char *name, void **buf, bool OverridePak = false);
	void	(*FS_FreeFile) (void *buf);
	char	*(*FS_Userdir)  (void);
	void	(*FS_CreatePath) (char *path);
	int		(*FS_FileExists) (char *path);

	// Add commands to the server console as if they were typed in for map changing etc.

	void	(*AddCommandString) (const char *text);
} game_import_t;


//
// functions exported by the game subsystem
//
typedef struct game_export_s {
    // the init function will only be called when a game starts,
    // not each time a level is loaded.  Persistant data for clients
    // and the server can be allocated in init
    int APIVERSION;
    void        (*Init) (void);
    void        (*Shutdown) (void);

    // each new level entered will cause a call to SpawnEntities
    void        (*SpawnEntities) (char *mapname, char *entstring, char *spawnpoint);

    // Read/Write Game is for storing persistant cross level information
    // about the world state and the clients.
    // WriteGame is called every time a level is exited.
    // ReadGame is called on a loadgame.
    void        (*WriteGame) (bool autosave);
    bool        (*ReadGame) (bool autosave);

    // ReadLevel is called after the default map information has been
    // loaded with SpawnEntities
    void        (*WriteLevel) (void);
    void        (*ReadLevel) (void);
    // void	(*RefreshWeapon)(edict_t &ent); windows only?

        //NEW
    void    (*ClientPreConnect) (void*);
    qboolean    (*ClientConnect) (edict_t *ent, char *userinfo);
	void        (*ClientBegin) (edict_t *ent);

	void        (*ClientUserinfoChanged) (edict_t *ent, char *userinfo, bool not_first_time);
    void        (*ClientDisconnect) (edict_t *ent);

    void        (*ClientCommand) (edict_t *ent);
    void        (*ClientThink) (edict_t *ent, usercmd_t *cmd);
    
    void        (*ResetCTFTeam) (edict_t *ent);

    int         (*GameAllowASave) (void);

    void        (*SavesLeft) (void);
    void        (*GetGameStats) (void);

    void        (*UpdateInven) (void);
    const char  *(*GetDMGameName) (void);

    byte        (*GetCinematicFreeze) (void);
    void        (*SetCinematicFreeze) (byte cf);

    float       (*RunFrame) (int serverframe);


} game_export_t;

game_export_t *GetGameApi (game_import_t *import);

#endif // _GAME_H_