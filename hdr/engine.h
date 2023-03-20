typedef int bool;
typedef void edict_t;
typedef void usercmd_t;
typedef int qboolean;
typedef unsigned char byte;

typedef enum {NA_LOOPBACK, NA_BROADCAST, NA_IP, NA_IPX, NA_BROADCAST_IPX} netadrtype_t;
typedef enum {NS_CLIENT, NS_SERVER} netsrc_t;
typedef struct netadr_s
{
    netadrtype_t    type; //0
    byte    ip[4]; //4
    byte    ipx[10]; //8

    unsigned short  port; //12
    // char * description; //14
} netadr_t;

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

    void        (*ClientThink) (edict_t *ent, usercmd_t *cmd);
    qboolean    (*ClientConnect) (edict_t *ent, char *userinfo);

    //NEW
    void    (*ClientPreConnect) (void*);

    void        (*ClientUserinfoChanged) (edict_t *ent, char *userinfo, bool not_first_time);

    void        (*ClientDisconnect) (edict_t *ent);
    void        (*ClientBegin) (edict_t *ent);

    void        (*ClientCommand) (edict_t *ent);
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