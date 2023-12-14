// qcommon.h -- definitions common between client and server, but not game.dll

#ifndef __QCOMMON
#define __QCOMMON

#include "../gamecpp/q_shared.h"
#include "pmove.h"
#include "vect3.h"

//============================================================================

void SZ_Init (sizebuf_t *buf, byte *data, int length);
void SZ_Clear (sizebuf_t *buf);
void *SZ_GetSpace (sizebuf_t *buf, int length);
void SZ_Write (sizebuf_t *buf, const void *data, int length);
void SZ_Print (sizebuf_t *buf, char *data);	// strcats onto the sizebuf

//============================================================================

struct usercmd_s;
struct entity_state_s;

void MSG_WriteChar (sizebuf_t *sb, int c);
void MSG_WriteByte (sizebuf_t *sb, int c);
void MSG_WriteShort (sizebuf_t *sb, int c);
void MSG_WriteWord (sizebuf_t *sb, int c);
void MSG_WriteLong (sizebuf_t *sb, int c);
void MSG_WriteFloat (sizebuf_t *sb, float f);
void MSG_WriteString (sizebuf_t *sb, char *s);
void MSG_WriteCoord (sizebuf_t *sb, float f);
void MSG_WritePos (sizebuf_t *sb, vec3_t pos);
void MSG_WriteAngle (sizebuf_t *sb, float f);
void MSG_WriteAngle16 (sizebuf_t *sb, float f);
void MSG_WriteDeltaUsercmd (sizebuf_t *sb, struct usercmd_s *from, struct usercmd_s *cmd);
void WriteBits(sizebuf_t *msg, int bits);
void MSG_WriteDeltaEntity (struct entity_state_s *from, struct entity_state_s *to
						   , sizebuf_t *msg, qboolean force);
void MSG_WriteDir (sizebuf_t *sb, vec3_t vector);
void MSG_WriteData (sizebuf_t *sb, unsigned char *data, int size);

int GetNearestByteNormal(vec3_t dir);

void	MSG_BeginReading (sizebuf_t *sb);

int		MSG_ReadChar (sizebuf_t *sb);
int		MSG_ReadByte (sizebuf_t *sb);
int		MSG_ReadShort (sizebuf_t *sb);
int		MSG_ReadWord (sizebuf_t *sb);
int		MSG_ReadLong (sizebuf_t *sb);
float	MSG_ReadFloat (sizebuf_t *sb);
char	*MSG_ReadString (sizebuf_t *sb);
char	*MSG_ReadStringLine (sizebuf_t *sb);

float	MSG_ReadCoord (sizebuf_t *sb);
void	MSG_ReadPos (sizebuf_t *sb, vec3_t pos);
float	MSG_ReadAngle (sizebuf_t *sb);
float	MSG_ReadAngle16 (sizebuf_t *sb);
void	MSG_ReadDeltaUsercmd (sizebuf_t *sb, struct usercmd_s *from, struct usercmd_s *cmd);

void	MSG_ReadDir (sizebuf_t *sb, vec3_t vector);
void	MSG_ReadData (sizebuf_t *sb, unsigned char *buffer, int size);

//============================================================================

extern	short	BigShort (short l);
extern	int		BigLong (int l);
extern	float	BigFloat (float l);

//============================================================================


int	COM_Argc (void);
char *COM_Argv (int arg);	// range and null checked
void COM_ClearArgv (int arg);
int COM_CheckParm (char *parm);
void COM_AddParm (char *parm);

void COM_Init (void);
void COM_InitArgv (int argc, char **argv);

char *CopyString (const char *in);

//============================================================================

void Info_Print (char *s);


/* crc.h */

void CRC_Init(unsigned short *crcvalue);
void CRC_ProcessByte(unsigned short *crcvalue, byte data);
unsigned short CRC_Value(unsigned short crcvalue);
int CRC_Block (byte *start, int count);




/*
==============================================================

PROTOCOL

==============================================================
*/

// protocol.h -- communications protocols
// changed to 33 for 1.07f.

#define	PROTOCOL_VERSION	33

/*
---------------------------------------------
Legend to help explain PORT designations -MW.

Ims	= id master server
Q2c = Q2 game client
Q2s = Q2 game server

where -> indicates direction of comms.
---------------------------------------------
*/

// SOF
#define	PORT_CLIENT			28901	// Opened on Q2c. Q2s -> Q2c : frame updates etc.
#define	PORT_SERVER			28910	// Opened on Q2s. Q2c -> Q2s : client commands, user info etc.
#define	PORT_GAMESPY		28911	// Used as default port for GameSpy

extern cvar_t			*public_server;			// should heartbeats be sent
extern struct sockaddr	server_address;
extern int				server_address_length;


typedef enum ChallengeTypes
{
	CHALLENGE_FREE = 0, 
	CHALLENGE_WAITING,
	CHALLENGE_INIT,
	CHALLENGE_RESPONSE1,
	CHALLENGE_RESPONSE2,
	CHALLENGE_OK
};

//=========================================

#define	UPDATE_BACKUP	16	// copies of entity_state_t to keep buffered
							// must be power of two
#define	UPDATE_MASK		(UPDATE_BACKUP-1)



//==================
// the svc_strings[] array in cl_parse.c should mirror this
//==================

//
// server to client
//
enum svc_ops_e
{
	svc_bad,

	// these ops are known to the game dll
	svc_temp_entity,
	svc_layout,
	svc_UNUSED,
	svc_sound_info,
	svc_effect,
	svc_equip,					// buying, equipping items messages

	// the rest are private to the client and server
	svc_nop,
	svc_disconnect,
	svc_reconnect,
	svc_sound,					// <see code>
	svc_print,					// [byte] id [string] null terminated string
	svc_nameprint,				// [byte] client num, [byte] team num, [string] null terminated string
	svc_stufftext,				// [string] stuffed into client's console buffer, should be \n terminated
	svc_serverdata,				// [long] protocol ...
	svc_configstring,			// [short] [string]
	svc_spawnbaseline,		
	svc_centerprint,			// [string] to put in center of the screen
	svc_captionprint,			// line number of [string] in strings.txt file -- ACTUALLY, JUST THE STRING
	svc_download,				// [short] size [size bytes]
	svc_playerinfo,				// variable
	svc_packetentities,			// [...]
	svc_deltapacketentities,	// [...]
	svc_frame,
	svc_culledEvent,
	svc_damagetexture,			// [short] surface index [byte] level
	svc_ghoulreliable,			// [short] size [size bytes]
	svc_ghoulunreliable,		// [short] size [size bytes]
	svc_ric,					// [byte] num of RICs [...] the actual RICs
	svc_restart_predn,			// [byte] restarts prediction
	svc_rebuild_pred_inv,		// [byte] [inven_c] syncs client inven to server one
	svc_countdown,				// [byte] time to countdown
	svc_cinprint,				// 
	svc_playernamecols,			// [byte] num of bytes [string] array of (client,color) pairs
	svc_sp_print,				// [short] string index [color]
	svc_removeconfigstring,		// [short]
	svc_sp_print_data_1,		// [short] string index [byte] count of data bytes [data bytes] [color]
	svc_sp_print_data_2,		// [short] string index [short] count of data bytes [data bytes] [color]
	svc_welcomeprint,			// sent down so welcome message is displayed at entry to a deathmatch server
	svc_sp_print_obit,			// send down the obituaries. Same format as svc_sp_print_data_1, but needs to be different packet type for client filter
	svc_force_con_notify,		// force a console clear - required for CTF when the game is over
};

//==============================================

//
// client to server
//
enum clc_ops_e
{
	clc_bad,
	clc_nop, 		
	clc_move,				// [[usercmd_t]
	clc_userinfo,			// [[userinfo string]
	clc_stringcmd,			// [string] message
};

//==============================================

// plyer_state_t communication

// please delta the gunUUID when we are forced to add more bits here (or if a bit frees up)

#define	PS_M_TYPE				(1<<0)
#define	PS_M_ORIGIN				(1<<1)
#define	PS_M_VELOCITY			(1<<2)
#define	PS_M_TIME				(1<<3)
#define	PS_M_FLAGS				(1<<4)
#define	PS_M_GRAVITY			(1<<5)
#define	PS_M_DELTA_ANGLES		(1<<6)
#define	PS_VIEWOFFSET			(1<<7)
#define	PS_VIEWANGLES			(1<<8)
#define	PS_KICKANGLES			(1<<9)
#define	PS_BLEND				(1<<10)
#define	PS_FOV					(1<<11)
#define	PS_REMOTE_VIEWORIGIN	(1<<12)
#define PS_REMOTE_ID			(1<<13)
#define	PS_RDFLAGS				(1<<14)
#define	PS_REMOTE_VIEWANGLES	(1<<15)
#define PS_GUN					(1<<16)
#define PS_GUN_CLIP				(1<<17)
#define PS_GUN_AMMO				(1<<18)
#define	PS_WEAPONKICKANGLES		(1<<19)
#define PS_BOD					(1<<20)//this should be temporary--player body in progress!--ss
#define PS_PIV					(1<<21)
#define PS_CINEMATICFREEZE		(1<<22)
#define PS_MUSICID				(1<<23)
#define PS_AMBSOUNDID			(1<<24)
#define PS_DMRANK				(1<<25)
#define PS_SPECTATORID			(1<<26)
#define PS_M_MOVESCALE			(1<<27)
#define	PS_RESTART_COUNT		(1<<28)
#define	PS_BUTTONS_INHIBIT		(1<<29)
#define PS_GUN_RELOAD			(1<<30)

//==============================================

// user_cmd_t communication

// ms and light allways sent, the others are optional
#define	CM_ANGLE1 		(1 << 0)
#define	CM_ANGLE2 		(1 << 1)
#define	CM_ANGLE3 		(1 << 2)
#define	CM_FORWARD		(1 << 3)
#define	CM_SIDE			(1 << 4)
#define	CM_UP			(1 << 5)
#define	CM_BUTTONS		(1 << 6)
#define CM_16BIT		(1 << 7)
#define CM_LEAN			(1 << 8)
						  
//==============================================

// a sound without an ent or pos will be a local only sound
#define	SND_VOLUME		(1 << 0)		// a byte
#define	SND_ATTENUATION	(1 << 1)		// a byte
#define	SND_POS			(1 << 2)		// three coordinates
#define	SND_ENT			(1 << 3)		// a short 0-2: channel, 3-12: entity
#define	SND_OFFSET		(1 << 4)		// a byte, msec offset from frame start
#define	SND_HI			(1 << 5)		// indicates the 9th bit of the sound index (up to 512 sounds)

#define DEFAULT_SOUND_PACKET_VOLUME	1.0
#define DEFAULT_SOUND_PACKET_ATTENUATION 1.0

//==============================================

// entity_state_t communication

// try to pack the common update flags into the first byte

// !!! be sure to update bit_names in cl_ents.cpp
#define	U_ORIGIN1		(1<<0)
#define	U_ORIGIN2		(1<<1)
#define	U_ANGLE2		(1<<2)
#define	U_ANGLE3		(1<<3)
#define	U_ANGLE1		(1<<4)
#define	U_ORIGIN3		(1<<5)
#define	U_EVENT			(1<<6)
#define	U_MOREBITS1		(1<<7)		// read one additional byte

// second byte
#define	U_RENDERFX16	(1<<8)		// 8 + 16 = 32
#define	U_RENDERMODEL	(1<<9)		// this was defined as "(1<<21)" before, same as U_MODEL2, which was presumably wrong (slc)
#define U_RENDERFX8		(1<<10)		// fullbright, etc
#define	U_REMOVE		(1<<11)		// REMOVE this entity, don't add it
#define	U_FRAME16		(1<<12)		// frame is a short
#define	U_EFFECT		(1<<13)
#define	U_EFFECTS16		(1<<14)		// 8 + 16 = 32
#define	U_MOREBITS2		(1<<15)		// read one additional byte

// third byte
#define	U_EFFECTS8		(1<<16)		// autorotate, trails, etc
#define	U_FRAME8		(1<<17)		// frame is a byte
#define	U_SOLID			(1<<18)
#define	U_SKIN8			(1<<19)
#define	U_MODEL			(1<<20)
#define	U_SOUND			(1<<21)
#define U_SOUND_HI		(1<<22)		// Extra bit for sound field (up to 512 max sounds)
#define	U_MOREBITS3		(1<<23)		// read one additional byte

// fourth byte
#define	U_NUMBER16		(1<<24)		// NUMBER8 is implicit if not set
#define	U_SKIN16		(1<<25)
#define U_ANGLEDIFF		(1<<26)

/*
==============================================================

CMD

Command text buffering and command execution

==============================================================
*/

/*

Any number of commands can be added in a frame, from several different sources.
Most commands come from either keybindings or console line input, but remote
servers can also send across commands and entire text files can be execed.

The + command line options are also added to the command buffer.

The game starts with a Cbuf_AddText ("exec quake.rc\n"); Cbuf_Execute ();

*/

#define	EXEC_NOW	0		// don't return until completed
#define	EXEC_INSERT	1		// insert at current position, but don't run yet
#define	EXEC_APPEND	2		// add to end of the command buffer

void Cbuf_Init (void);
// allocates an initial text buffer that will grow as needed

void Cbuf_AddText (const char *text);
// as new commands are generated from the console or keybindings,
// the text is added to the end of the command buffer.

void Cbuf_InsertText (const char *text);
// when a command wants to issue other commands immediately, the text is
// inserted at the beginning of the buffer, before any remaining unexecuted
// commands.

void Cbuf_ExecuteText (int exec_when, const char *text);
// this can be used in place of either Cbuf_AddText or Cbuf_InsertText

void Cbuf_AddEarlyCommands (qboolean clear);
// adds all the +set commands from the command line

qboolean Cbuf_AddLateCommands (void);
// adds all the remaining + commands from the command line
// Returns true if any late commands were added, which
// will keep the demoloop from immediately starting

void Cbuf_Execute (void);
// Pulls off \n terminated lines of text from the command buffer and sends
// them through Cmd_ExecuteString.  Stops when the buffer is empty.
// Normally called once per frame, but may be explicitly invoked.
// Do not call inside a command function!

void Cbuf_CopyToDefer (void);
void Cbuf_InsertFromDefer (void);
// These two functions are used to defer any pending commands while a map
// is being loaded

//===========================================================================

/*

Command execution takes a null terminated string, breaks it into tokens,
then searches for a command or variable that matches the first token.

*/

typedef void (*xcommand_t) (void);

void	Cmd_Init (void);

void	Cmd_AddCommand (char *cmd_name, xcommand_t function);
// called by the init functions of other parts of the program to
// register commands and functions to call for them.
// The cmd_name is referenced later, so it should not be in temp memory
// if function is NULL, the command will be forwarded to the server
// as a clc_stringcmd instead of executed locally
void	Cmd_RemoveCommand (char *cmd_name);

qboolean Cmd_Exists (char *cmd_name);
// used by the cvar code to check for cvar / command name overlap

char 	*Cmd_CompleteCommand (char *partial);
char *Cmd_CompleteCommandNext (char *partial, char *last);
// attempts to match a partial command for automatic command line completion
// returns NULL if nothing fits

int		Cmd_Argc (void);
char	*Cmd_Argv (int arg);
char	*Cmd_Args (void);
// The functions that execute commands get their parameters with these
// functions. Cmd_Argv () will return an empty string, not a NULL
// if arg > argc, so string operations are allways safe.

void	Cmd_TokenizeString (const char *text, qboolean macroExpand);
// Takes a null terminated string.  Does not need to be /n terminated.
// breaks the string up into arg tokens.

void	Cmd_ExecuteString (const char *text);
// Parses a single line of text into arguments and tries to execute it
// as if it was typed at the console

void	Cmd_ForwardToServer (void);
// adds the current command line as a clc_stringcmd to the client message.
// things like godmode, noclip, etc, are commands directed to the server,
// so when they are typed in at the console, they will need to be forwarded.


/*
==============================================================

CVAR

==============================================================
*/

/*

cvar_t variables are used to hold scalar or string variables that can be changed or displayed at the console or prog code as well as accessed directly
in C code.

The user can access cvars from the console in three ways:
r_draworder			prints the current value
r_draworder 0		sets the current value to 0
set r_draworder 0	as above, but creates the cvar if not present
Cvars are restricted from having the same names as commands to keep this
interface from being ambiguous.
*/

extern	cvar_t	*cvar_vars;

void Cvar_SetInternal(bool new_internal);

cvar_t *Cvar_Get (const char *var_name, const char *value, int flags, cvarcommand_t	command = NULL);
// creates the variable if it doesn't exist, or returns the existing one
// if it exists, the value will not be changed, but flags will be ORed in
// that allows variables to be unarchived without needing bitflags

cvar_t 	*Cvar_Set (const char *var_name, const char *value);
// will create the variable if it doesn't exist

cvar_t *Cvar_ForceSet (const char *var_name, const char *value);
// will set the variable even if NOSET or LATCH

cvar_t 	*Cvar_FullSet (const char *var_name, const char *value, int flags);

void	Cvar_SetValue (const char *var_name, float value);
// expands value to a string and calls Cvar_Set

float	Cvar_VariableValue (const char *var_name);
// returns 0 if not defined or non numeric

char	*Cvar_VariableString (const char *var_name);
// returns an empty string if not defined

char 	*Cvar_CompleteVariable (char *partial);
char *Cvar_CompleteVariableNext (char *partial, char *last);
// attempts to match a partial variable name for command line completion
// returns NULL if nothing fits

void	Cvar_GetLatchedVars (void);
// any CVAR_LATCHED variables that have been set will now take effect

qboolean Cvar_Command (void);
// called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
// command.  Returns true if the command was a variable reference that
// was handled. (print or change)

void 	Cvar_WriteVariables (const char *path);
// appends lines containing "set variable value" for all variables
// with the archive flag set to true.

void	Cvar_Init (void);

// returns an info string containing all the flags
// Flags can be CVAR_USERINFO, CVAR_SERVERINFO, CVAR_WEAPON, CVAR_ITEM, CVAR_AMMO, CVAR_MISC
char	*Cvar_Info (int flags);

extern	qboolean	userinfo_modified;
// this is set each time a CVAR_USERINFO variable is changed
// so that the client knows to send it to the server

/*
==============================================================

NET

==============================================================
*/

// net.h -- quake's interface to the networking layer

#define	PORT_ANY	-1

#define	MAX_MSGLEN_MAX		(16384)		// max length of any message
extern int MaxMsgLen; // current max msg len

#define	PACKET_HEADER	10			// two ints and a short

typedef enum {NA_LOOPBACK, NA_BROADCAST, NA_IP, NA_IPX, NA_BROADCAST_IPX} netadrtype_t;

typedef enum {NS_CLIENT, NS_SERVER} netsrc_t;

typedef struct netadr_s
{
	netadrtype_t	type;

	byte	ip[4];
	byte	ipx[10];

	unsigned short	port;
} netadr_t;

void		NET_Init (void);
void		NET_Shutdown (void);

void		NET_Config (qboolean multiplayer);

qboolean	NET_GetPacket (netsrc_t sock, netadr_t *net_from, sizebuf_t *net_message);
void		NET_SendPacket (netsrc_t sock, int length, void *data, netadr_t to);

bool		NET_CompareAdr (netadr_t a, netadr_t b);
qboolean	NET_CompareBaseAdr (netadr_t a, netadr_t b);
qboolean	NET_IsLocalAddress (netadr_t adr);
char		*NET_AdrToString (netadr_t a);
qboolean	NET_StringToAdr (char *s, netadr_t *a);

int			NET_GetConnections (int sock);
qboolean	NET_SendData (int sock, int length, void *data);
qboolean	NET_GetData (int sock, sizebuf_t *net_message);
qboolean	NET_CloseConnection (int sock);

//============================================================================

#define	OLD_AVG		0.99		// total = oldtotal*OLD_AVG + new*(1-OLD_AVG)

#define	MAX_LATENT	32

typedef struct
{
	qboolean	fatal_error;

	netsrc_t	sock;

	int			dropped;			// between last packet and previous

	int			last_received;		// for timeouts
	int			last_sent;			// for retransmits

	netadr_t	remote_address;
	int			qport;				// qport value to write when transmitting

// sequencing variables
	int			incoming_sequence;
	int			incoming_acknowledged;
	int			incoming_reliable_acknowledged;	// single bit

	int			incoming_reliable_sequence;		// single bit, maintained local

	int			outgoing_sequence;
	int			reliable_sequence;			// single bit
	int			last_reliable_sequence;		// sequence number of last send

// reliable staging and holding areas
	sizebuf_t	message;		// writing buffer to send to server
	byte		message_buf[MAX_MSGLEN_MAX-16];		// leave space for header

// message is copied to this buffer when it is first transfered
	int			reliable_length;
	byte		reliable_buf[MAX_MSGLEN_MAX-16];	// unacked reliable message
} netchan_t;

extern	netadr_t	net_from;
extern	sizebuf_t	net_message;
extern	byte		net_message_buffer[MAX_MSGLEN_MAX];


void Netchan_Init (void);
void Netchan_Setup (netsrc_t sock, netchan_t *chan, netadr_t adr, int qport);

qboolean Netchan_NeedReliable (netchan_t *chan);
void Netchan_Transmit (netchan_t *chan, int length, byte *data);
void Netchan_OutOfBand (int net_socket, netadr_t adr, int length, byte *data);
void Netchan_OutOfBandPrint (int net_socket, netadr_t adr, char *format, ...);
qboolean Netchan_Process (netchan_t *chan, sizebuf_t *msg);

qboolean Netchan_CanReliable (netchan_t *chan);


/*
==============================================================

CMODEL

==============================================================
*/


#include "../qcommon/qfiles.h"

void 		CM_RefreshMapData(qboolean clientload);
int			CM_NumClusters (void);
int			CM_NumInlineModels (void);
char		*CM_EntityString (void);

// creates a clipping hull for an arbitrary box
int			CM_HeadnodeForBox (vec3_t mins, vec3_t maxs);


// returns an ORed contents mask
int			CM_PointContents (vec3_t p, int headnode);
int			CM_TransformedPointContents (vec3_t p, int headnode, vec3_t origin, vec3_t angles);

float		CM_ShadowTrace(vec3_t start,vec3_t end);

trace_t		CM_BoxTrace (vec3_t start, vec3_t end,
						  vec3_t mins, vec3_t maxs,
						  int headnode, int brushmask);
trace_t		CM_TransformedBoxTrace (vec3_t start, vec3_t end,
						  vec3_t mins, vec3_t maxs,
						  int headnode, int brushmask,
						  vec3_t origin, vec3_t angles);

byte		*CM_ClusterPVS (int cluster);
byte		*CM_ClusterPHS (int cluster);

int			CM_PointLeafnum (vec3_t p);

// call with topnode set to the headnode, returns with topnode
// set to the first node that splits the box
int			CM_BoxLeafnums (vec3_t mins, vec3_t maxs, int *boxlist, int listsize, int *topnode);

int			CM_LeafContents (int leafnum);
int			CM_LeafCluster (int leafnum);
int			CM_LeafArea (int leafnum);

void		CM_SetAreaPortalState (int portalnum, qboolean open);
qboolean	CM_AreasConnected (int area1, int area2);

int			CM_WriteAreaBits (byte *buffer, int area);
qboolean	CM_HeadnodeVisible (int headnode, byte *visbits);

void		CM_WritePortalState ();
void		CM_ReadPortalState ();

void		CM_UnloadMapLumps(void);
void		CM_ClearMap(void);
bool		CM_MapDataValid(void);
void		CM_CreateMapChecksum(char *name, char *checkstring);
bsplumps_t	*CM_GetLumpbase(char *name, int lump, void (*callback)(void *cm_out, void *cm_in) = NULL);

int			CM_BoxLeafnums_headnode (vec3_t mins, vec3_t maxs, int *boxlist, int listsize, int headnode, int *topnode);	
void		CM_TestInLeaf (int leafnum);	

qboolean	PF_inPVS (vec3_t p1, vec3_t p2);
float		SV_CheckRegionDistance(vec3_t vert);

/*
==============================================================

PLAYER MOVEMENT CODE

Common between server and client so prediction matches

==============================================================
*/

#define MAX_CMD_PACKETSIZE	64

void Pmove (pmove_t *pmove);

/*
==============================================================

MISC

==============================================================
*/


#define	ERR_FATAL	0		// exit the entire game with a popup window
#define	ERR_DROP	1		// print to console and disconnect from game
#define	ERR_QUIT	2		// not an error, just a normal exit

#define	EXEC_NOW	0		// don't return until completed
#define	EXEC_INSERT	1		// insert at current position, but don't run yet
#define	EXEC_APPEND	2		// add to end of the command buffer

#define	PRINT_ALL			0
#define PRINT_DEVELOPER		1	// only print when "developer & 1"
#define PRINT_EXT_DEVELOPER	2	// only print when "developer & 4"
#define PRINT_ALERT			3	// bring up a message box

void		Com_BeginRedirect (int target, char *buffer, int buffersize, void (*flush));
void		Com_EndRedirect (void);
void 		Com_Printf (char *fmt, ...);
void 		Com_DPrintf (char *fmt, ...);
void 		Com_EPrintf (char *fmt, ...);
void 		Com_Error (int code, const char *fmt, ...);
void 		Com_Quit (void);

int			Com_ServerState (void);		// this should have just been a cvar...
void		Com_SetServerState (int state);

unsigned	Com_BlockChecksum (void *buffer, int length);
byte		COM_BlockSequenceCheckByte (byte *base, int length, int sequence);

extern	cvar_t	*developer;
extern	cvar_t	*dedicated;
extern	cvar_t	*host_speeds;
extern	cvar_t	*log_stats;

extern	FILE *log_stats_file;

// host_speeds times
extern	int		time_before_game;
extern	int		time_after_game;
extern	int		time_before_ref;
extern	int		time_after_ref;

void Z_Free (void *ptr);
void Z_Touch (void *ptr);
void *Z_Malloc (int size);			// returns 0 filled memory
void *Z_TagMalloc (int size, int tag);
void Z_FreeTags (int tag);

void Qcommon_Init (int argc, char **argv);
void Qcommon_Frame (int msec);
void Qcommon_Shutdown (void);

#define NUMVERTEXNORMALS	250
extern vec3_t bytedirs[NUMVERTEXNORMALS];

// this is in the client code, but can be used for debugging from server
void SCR_DebugGraph (float v1, int c1, float v2 = 0.0, int c2 = -1, float v3 = 0.0, int c3 = -1, float v4 = 0.0, int c4 = -1, float v5 = 0.0, int c5 = -1);


/*
==============================================================

NON-PORTABLE SYSTEM SERVICES

==============================================================
*/

void	Sys_Init (void);

void	Sys_AppActivate (void);

// Loads the game dll and calls the api init function.

void	Sys_UnloadGame (void);
void	*Sys_GetGameAPI (void *parms);

// Loads the player dll and calls the api init function.

void	Sys_UnloadPlayer (int isClient);
void	*Sys_GetPlayerAPI (void *parmscom,void *parmscl,void *parmssv,int isClient);

char	*Sys_ConsoleInput (void);
void	Sys_ConsoleOutput (char *string);
void	Sys_SendKeyEvents (void);
void	Sys_Error (char *error, ...);
void	Sys_Quit (void);
char	*Sys_GetClipboardData( void );
void	Sys_CopyProtect (void);

#ifndef _FINAL_
char *GetSymbol(void *addr);
#endif

/*
==============================================================

CLIENT / SERVER SYSTEMS

==============================================================
*/

#define SG_SCR_WIDTH			128
#define SG_SCR_HEIGHT			128
#define SG_SCR_DISPLAYHEIGHT	(SG_SCR_HEIGHT * 3 / 4)

void CL_Init (void);
void CL_Drop (bool setmsg = true);
void CL_Shutdown (void);
void CL_Frame (int msec);
void SCR_BeginLoadingPlaque (qboolean noPlaque);
byte *CL_GetScreenshot();
void CL_SetScreenshot(const char *name, int w, int h, byte *data);

void SV_Init (void);
void SV_Shutdown (char *finalmsg, qboolean reconnect);
void SV_Frame (int msec);

extern	int	curtime;		// time returned by last Sys_Milliseconds

int HasAttachedEffects(entity_state_t *curState);

#endif // __QCOMMON
