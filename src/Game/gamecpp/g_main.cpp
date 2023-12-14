#include "g_local.h"
#include "p_body.h"
#include "w_weapons.h"
#include "ai_pathfinding.h"
#include "../strings/singleplr.h"
#include "../strings/dm_generic.h"
#include "dm_ctf.h"

#if TIME_ENTS
	#include <windows.h>
	#include "timing.h"
#endif

game_locals_t	game;
level_locals_t	level;
game_import_t	gi;
game_export_t	globals;
spawn_temp_t	st;
player_export_t	*pe=NULL;

char		level_ai_name[MAX_QPATH];		// the over-ride for the AI per-level

int	sm_meat_index;
int meansOfDeath;

edict_t		*g_edicts;
IGhoul *TheGhoul;

cvar_t	*deathmatch;
cvar_t	*ctf_loops;
cvar_t	*ctf_loops_count;
cvar_t	*ctf_flag_captured;
cvar_t	*ctf_team_red;
cvar_t	*ctf_team_blue;

//cvar_t	*coop;
cvar_t	*dmflags;
cvar_t	*skill;
cvar_t	*fraglimit;
cvar_t	*timelimit;
cvar_t	*password;
cvar_t	*spectator_password;
cvar_t	*maxclients;
cvar_t	*maxspectators;
cvar_t	*sv_maplist;
cvar_t	*maxentities;
cvar_t	*dedicated;
cvar_t	*sv_pmodlistfile;
cvar_t	*sv_altpmodlistfile;
cvar_t	*sv_suicidepenalty;

cvar_t	*mskins_expression_limit;
cvar_t	*mskins_variety_limit;

cvar_t	*freezeworld;

cvar_t	*thirdpersoncam;
cvar_t	*followenemy;
cvar_t	*sv_maxvelocity;
cvar_t	*sv_gravity;
cvar_t	*sv_gravityx;
cvar_t	*sv_gravityy;
cvar_t	*sv_gravityz;

cvar_t	*ai_freeze;//monsters halt
cvar_t	*ai_goretest;//sets health really high
cvar_t	*ai_pathtest;//adds omniscience to guys
cvar_t	*ai_highpriority;

cvar_t	*ei_show;
cvar_t	*aipoints_show;
cvar_t	*ai_nonodes;
cvar_t	*aidec_show;
cvar_t	*ai_dumb;
cvar_t	*ai_editdir;

cvar_t	*flood_msgs;
cvar_t	*flood_persecond;
cvar_t	*flood_waitdelay;
cvar_t	*flood_killdelay;

cvar_t	*ghl_specular;
cvar_t	*ghl_light_method;
cvar_t	*ghl_precache_verts;
cvar_t	*ghl_precache_texture;
cvar_t	*ghl_no_server_update;

cvar_t	*sv_rollspeed;
cvar_t	*sv_rollangle;

cvar_t	*run_pitch;
cvar_t	*run_roll;
cvar_t	*bob_up;
cvar_t	*bob_pitch;
cvar_t	*bob_roll;

cvar_t	*log_file_counter;
cvar_t	*log_file_mode;
cvar_t	*log_file_name;
cvar_t	*log_file_header;
cvar_t	*log_file_footer;
cvar_t	*log_file_line_header;

cvar_t	*camera_viewdist;

cvar_t	*sv_cheats;
cvar_t	*nodamage;
cvar_t	*gl_pictip;

cvar_t	*sk_spawning;
cvar_t	*sk_saving;
cvar_t	*sk_toughness;
cvar_t	*sk_maxencum;

cvar_t	*ai_spawnfrequency;
cvar_t	*ai_maxcorpses;
cvar_t	*ai_corpselife;

cvar_t	*sv_jumpcinematic;		// set by player. means we're skipping the cinematic

cvar_t	*sv_restartlevel;		// when the player dies, space bar restarts the level

cvar_t	*stealth;				// show/hide stealth meter

cvar_t	*pickupinfo;			// do we want to track pickup information for the next level?
cvar_t	*weaponarena;			// weapon testing -- sets all pickups in the game to the objtype_t in weaponarena

cvar_t	*g_movescale;

void SpawnEntities (char *mapname, char *entities, char *spawnpoint);
void ClientThink (edict_t *ent, usercmd_t *cmd);
qboolean ClientConnect (edict_t *ent, char *userinfo);
void ClientUserinfoChanged (edict_t *ent, char *userinfo, bool not_first_time);
void ClientDisconnect (edict_t *ent);
void ClientBegin (edict_t *ent);
void ClientCommand (edict_t *ent);
void RunEntity (edict_t *ent);
void WriteGame (bool autosave);
bool ReadGame (bool autosave);
void WriteLevel (void);
void ReadLevel (void);
float G_RunFrame (int sframe);

void UpdateWorldSpawningByFrame(void);
void CheckDeadHostageCount(edict_t *player);

extern void Game_ShutdownPlayer (void);
extern int *Game_InitPlayer(void);

int G_GameAllowASave(void);
void G_SavesLeft(void);

bool	lock_textures;

//
// Callback should the skill level change
//

void InitPlayerSkills(void)
{
	int		skillLevel;

	skillLevel = floor(skill->value);

	// Lovely bit of "lateral thinking" code (read: hack) to let cheats be properly read in deathmatch
	if(deathmatch->value)
	{
		skillLevel = 5;
	}

	if(skillLevel < 0)
	{
		skillLevel = 0;
	}
	if(skillLevel > 5)
	{
		skillLevel = 5;
	}
	//hmm...
	if(skillLevel == 0)
	{
		//some presets here
		gi.cvar_forceset("sk_toughness", "0");
		gi.cvar_forceset("sk_spawning", "0");
		gi.cvar_forceset("sk_saving", "-1");
		gi.cvar_forceset("cheats", "1");
		gi.cvar_forceset("sk_maxencum", "0");
		
		game.playerSkills.buildCustomSkillLevels();
	}
	else if(skillLevel == 1)
	{
		//some presets here
		gi.cvar_forceset("sk_toughness", "1");
		gi.cvar_forceset("sk_spawning", "1");
		gi.cvar_forceset("sk_saving", "8");
		gi.cvar_forceset("cheats", "1");
		gi.cvar_forceset("sk_maxencum", "1");
		
		game.playerSkills.buildCustomSkillLevels();
	}
	else if(skillLevel == 2)
	{
		//some presets here
		gi.cvar_forceset("sk_toughness", "2");
		gi.cvar_forceset("sk_spawning", "2");
		gi.cvar_forceset("sk_saving", "5");
		gi.cvar_forceset("cheats", "1");
		gi.cvar_forceset("sk_maxencum", "2");
		
		game.playerSkills.buildCustomSkillLevels();
	}
	else if(skillLevel == 3)
	{
		//some presets here
		gi.cvar_forceset("sk_toughness", "4");
		gi.cvar_forceset("sk_spawning", "3");
		gi.cvar_forceset("sk_saving", "2");
		gi.cvar_forceset("cheats", "0");
		gi.cvar_forceset("sk_maxencum", "3");
		
		game.playerSkills.buildCustomSkillLevels();
	}
	else if(skillLevel == 4)
	{
		//some presets here
		gi.cvar_forceset("sk_toughness", "3");
		gi.cvar_forceset("sk_spawning", "4");
		gi.cvar_forceset("sk_saving", "0");
		gi.cvar_forceset("cheats", "0");
		gi.cvar_forceset("sk_maxencum", "4");

		game.playerSkills.buildCustomSkillLevels();
	}
	else
	{
		game.playerSkills.buildCustomSkillLevels();
	}

	game.playerSkills.setMoneyMult();
}

/*
============
InitGame

This will be called when the dll is first loaded, which
only happens when a new game is started or a save game
is loaded.
============
*/

void InitGame (void)
{
	gi.dprintf ("==== InitGame ====\n");

	//FIXME: sv_ prefix is wrong for these
	sv_rollspeed = gi.cvar ("sv_rollspeed", "200", 0);
	sv_rollangle = gi.cvar ("sv_rollangle", "2", 0);
	sv_maxvelocity = gi.cvar ("sv_maxvelocity", "2000", 0);
	sv_gravity = gi.cvar ("sv_gravity", "800", 0);
	sv_gravityx = gi.cvar ("sv_gravityx", "0", 0);
	sv_gravityy = gi.cvar ("sv_gravityy", "0", 0);
	sv_gravityz = gi.cvar ("sv_gravityz", "1", 0);
	g_movescale = gi.cvar ("g_movescale", "1.0", CVAR_SERVERINFO | CVAR_ARCHIVE);

	ai_freeze = gi.cvar ("ai_freeze", "0", 0);
	ai_goretest = gi.cvar ("ai_goretest", "0", 0);
	ai_pathtest = gi.cvar ("ai_pathtest", "0", 0);
	ai_highpriority = gi.cvar ("ai_highpriority", "0", 0);

	ei_show = gi.cvar ("ei_show", "0", 0);
	aipoints_show = gi.cvar ("aipoints_show", "0", 0);
	ai_nonodes = gi.cvar ("ai_nonodes", "0", 0);
	aidec_show = gi.cvar ("aidec_show", "0", 0);
	ai_dumb = gi.cvar ("ai_dumb", "0", 0);
	ai_editdir = gi.cvar ("ai_editdir", "p:", CVAR_ARCHIVE);
	ai_maxcorpses = gi.cvar("ai_maxcorpses", "5", CVAR_ARCHIVE);
	ai_corpselife = gi.cvar("ai_corpselife", "5", CVAR_ARCHIVE);
	ai_spawnfrequency = gi.cvar("ai_spawnfrequency", "1.0", CVAR_MISC);
	
	ghl_specular = gi.cvar ("ghl_specular", "1", CVAR_ARCHIVE);
	ghl_light_method = gi.cvar ("ghl_light_method", "2", CVAR_ARCHIVE);
	ghl_precache_verts = gi.cvar ("ghl_precache_verts", "0", 0);
	ghl_precache_texture = gi.cvar ("ghl_precache_texture", "1", 0);
	ghl_no_server_update = gi.cvar ("ghl_no_server_update", "0", 0);

	followenemy = gi.cvar("followenemy","0",0);
	thirdpersoncam = gi.cvar("thirdpersoncam","0",0);

	// noset vars
	dedicated = gi.cvar ("dedicated", "0", CVAR_NOSET);

	// latched vars
	sv_cheats = gi.cvar ("cheats", "0", CVAR_SERVERINFO | CVAR_LATCH);
	gi.cvar ("gamename", GAMEVERSION , CVAR_SERVERINFO | CVAR_LATCH);
	gi.cvar ("gamedate", __DATE__ , CVAR_SERVERINFO | CVAR_LATCH);

	maxclients = gi.cvar ("maxclients", "4", CVAR_SERVERINFO | CVAR_LATCH);
	maxspectators = gi.cvar ("maxspectators", "4", CVAR_SERVERINFO| CVAR_LATCH);
	deathmatch = gi.cvar ("deathmatch", "0", CVAR_SERVERINFO | CVAR_LATCH);
	maxentities = gi.cvar ("maxentities", "1024", CVAR_LATCH);
	ctf_loops = gi.cvar("ctf_loops", "3", CVAR_ARCHIVE | CVAR_SERVERINFO);
	ctf_loops_count = gi.cvar("ctf_loops_count", "0", 0);
	// just to be sure
	ctf_loops_count->value = 0;
	ctf_flag_captured = gi.cvar("ctf_flag_captured", "0", 0);
	ctf_team_red = gi.cvar("ctf_team_red", "MeatWagon", CVAR_ARCHIVE | CVAR_SERVERINFO);
	ctf_team_blue = gi.cvar("ctf_team_blue", "The Order", CVAR_ARCHIVE | CVAR_SERVERINFO);

	mskins_expression_limit = gi.cvar ("mskins_expression_limit", "0", CVAR_LATCH | CVAR_ARCHIVE);
	mskins_variety_limit = gi.cvar ("mskins_variety_limit", "0", CVAR_LATCH | CVAR_ARCHIVE);

	// change anytime vars
	dmflags = gi.cvar ("dmflags", "0", CVAR_SERVERINFO | CVAR_ARCHIVE);
	fraglimit = gi.cvar ("fraglimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE);
	timelimit = gi.cvar ("timelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE);
	sv_suicidepenalty = gi.cvar ("sv_suicidepenalty", "2", CVAR_SERVERINFO | CVAR_ARCHIVE);
	password = gi.cvar ("password", "", CVAR_USERINFO);
	spectator_password = gi.cvar ("spectator_password", "", CVAR_USERINFO);

	nodamage = gi.cvar ("nodamage", "0", 0);

	sk_spawning = gi.cvar ("sk_spawning", "2", CVAR_LATCH);
	sk_saving = gi.cvar ("sk_saving", "2", CVAR_LATCH);
	sk_toughness = gi.cvar ("sk_toughness", "2", CVAR_LATCH);
	sk_maxencum = gi.cvar ("sk_maxencum", "0", CVAR_LATCH);
	skill = gi.cvar ("skill", "2", CVAR_LATCH | CVAR_SERVERINFO);

	freezeworld = gi.cvar("freezeworld", "0", 0);
	sv_maplist = gi.cvar ("sv_maplist", "", 0);

#ifdef _OEMMP_
	sv_pmodlistfile = gi.cvar ("sv_pmodlistfile", "ghoul/pmodels/oemlist", CVAR_LATCH | CVAR_ARCHIVE);
#elif _DEMO_
	sv_pmodlistfile = gi.cvar ("sv_pmodlistfile", "ghoul/pmodels/demo", CVAR_LATCH | CVAR_ARCHIVE);
#else
	sv_pmodlistfile = gi.cvar ("sv_pmodlistfile", "ghoul/pmodels/everybody", CVAR_LATCH | CVAR_ARCHIVE);
#endif //_OEMMP_

	sv_altpmodlistfile = gi.cvar ("sv_altpmodlistfile", "custom", CVAR_LATCH | CVAR_ARCHIVE);

	run_pitch = gi.cvar ("run_pitch", "0.002", 0);
	run_roll = gi.cvar ("run_roll", "0.005", 0);
	bob_up  = gi.cvar ("bob_up", "0.005", 0);
	bob_pitch = gi.cvar ("bob_pitch", "0.002", 0);
	bob_roll = gi.cvar ("bob_roll", "0.002", 0);

	lock_deaths = gi.cvar_variablevalue("lock_deaths") != 0.0;
	lock_blood = gi.cvar_variablevalue("lock_blood") != 0.0;
	lock_sever = gi.cvar_variablevalue("lock_sever") != 0.0;
	lock_gorezones = gi.cvar_variablevalue("lock_gorezones") != 0.0;
	lock_textures = gi.cvar_variablevalue("lock_textures") != 0.0;

	camera_viewdist = gi.cvar ("camera_viewdist", "60.0", 0);

	log_file_counter = gi.cvar("log_file_counter", "0", CVAR_ARCHIVE);
	log_file_name = gi.cvar("log_file_name", "", CVAR_ARCHIVE);
	log_file_footer = gi.cvar("log_file_footer", "", CVAR_ARCHIVE);
	log_file_header = gi.cvar("log_file_header", "", CVAR_ARCHIVE);
	log_file_line_header = gi.cvar("log_file_line_header", "", CVAR_ARCHIVE);
	log_file_mode = gi.cvar("log_file_mode", "a", CVAR_ARCHIVE);

	sv_jumpcinematic = gi.cvar("sv_jumpcinematic", "0", 0);

	sv_restartlevel = gi.cvar("sv_restartlevel", "0", 0);

	stealth = gi.cvar ("stealth", "1", 0);

	pickupinfo = gi.cvar ("pickupinfo", "0", CVAR_SERVERINFO | CVAR_LATCH);
	weaponarena = gi.cvar ("weaponarena", "0", CVAR_SERVERINFO | CVAR_LATCH);

	Com_sprintf (game.helpmessage1, sizeof(game.helpmessage1), "");
	Com_sprintf (game.helpmessage2, sizeof(game.helpmessage2), "");

	// initialize all entities for this game
	game.maxentities = maxentities->value;
	g_edicts =  (edict_s*)gi.TagMalloc (game.maxentities * sizeof(g_edicts[0]), TAG_GAME);
	globals.edicts = g_edicts;
	globals.max_edicts = game.maxentities;

	// initialize all clients for this game
	game.maxclients = (int)maxclients->value;
	game.clients = (gclient_s*)gi.TagMalloc (game.maxclients * sizeof(game.clients[0]), TAG_GAME);
	globals.num_edicts = game.maxclients+1;

	flood_msgs = gi.cvar ("flood_msgs", "4", CVAR_ARCHIVE);
	flood_persecond = gi.cvar ("flood_persecond", "4", CVAR_ARCHIVE);
	flood_waitdelay = gi.cvar ("flood_waitdelay", "10", CVAR_ARCHIVE);
	flood_killdelay = gi.cvar ("flood_killdelay", "10", CVAR_ARCHIVE);

	game.cinematicfreeze = 0;

	cpu_mmx		= gi.cvar ("cpu_mmx",	"0", 0);	
	cpu_amd3d	= gi.cvar ("cpu_amd3d", "0", 0);		
	use_mmx		= gi.cvar ("use_mmx",   "1", 0);	
	use_amd3d	= gi.cvar ("use_amd3d", "1", 0);

	gl_pictip = gi.cvar ("gl_pictip", "0", 0);

	// Init the skill level data
	InitPlayerSkills();

	// Load in and initialise the player dll
	(*gi.isClient) = Game_InitPlayer();
}

//===================================================================
void PB_KillBody(edict_t &ent);
void W_KillInv(edict_t &ent);

void CleanUpGame (void)
{
	int i;

	gi.dprintf ("==== ShutdownGame ====\n");

	if(pe)
	{
		pe->UncacheViewWeaponModels();
	}

//	game.GameStats->Delete();

	ShutdownScripts();

	// Delete each client's inventory object.
	// And get rid of everybody's bodies, too!
	
	for (i = 0; i < game.maxclients; i++)
	{
		if(g_edicts[i + 1].client)
		{
			PB_KillBody(g_edicts[i + 1]);
			W_KillInv(g_edicts[i + 1]);

			delete g_edicts[i + 1].client->dmInfo;
			g_edicts[i + 1].client->ps.gun = 0;
		}
	}

	// clear out any pickup info from this level...BEFORE freeing edicts, please.
	thePickupList.Destroy();

	//clean up ai stuff that was created without exe's knowledge
	G_FreeAllEdicts();

	// Shutdown player dll.
	(*gi.isClient) = 0;
	Game_ShutdownPlayer();

	//clean up all the ghoul stuff we've been using
	W_ShutdownWorldWeaponModels();

	DeleteDeathmatchSystem();
	aiPoints.clean();
//	EnemyInfo.cleanup();	

#if TIME_ENTS
	PrintTimings();
#endif
}

void ShutdownGame (void)
{
	CleanUpGame();
	gi.FreeTags (TAG_LEVEL);
	gi.FreeTags (TAG_GAME);
}

void G_SetCinematicFreeze(byte cf)
{
	game.cinematicfreeze = cf;
}

byte G_GetCinematicFreeze(void)
{
	return(game.cinematicfreeze);
}

void G_GetGameStats(void)
{
	int		hours, minutes, seconds;
	int		thours, tminutes, tseconds;
	float	total;

	total = game.playedTime + (level.time - level.startTime);
	if(!total)
	{
		return;
	}
	gi.cvar_set("stat_playedtime_base", va("%f", total));

	tseconds = (int)total % 60;
	total /= 60;
	tminutes = (int)total % 60;
	total /= 60;
	thours = total;

	total = (level.time - level.startTime);
	seconds = (int)total % 60;
	total /= 60;
	minutes = (int)total % 60;
	total /= 60;
	hours = total;

	gi.cvar_set("stat_guyskilled", va("%d " P_ORANGE "(%d)", level.guysKilled, game.guysKilled + level.guysKilled));
	gi.cvar_set("stat_friendlieskilled", va("%d " P_ORANGE "(%d)", level.friendliesKilled, game.friendliesKilled + level.friendliesKilled));
	gi.cvar_set("stat_throatshots", va("%d " P_ORANGE "(%d)", level.throatShots, game.throatShots + level.throatShots));
	gi.cvar_set("stat_nutshots", va("%d " P_ORANGE "(%d)", level.nutShots, game.nutShots + level.nutShots));
	gi.cvar_set("stat_headshots", va("%d " P_ORANGE "(%d)", level.headShots, game.headShots + level.headShots));
	gi.cvar_set("stat_gibs", va("%d " P_ORANGE "(%d)", level.gibs, game.gibs + level.gibs));
	gi.cvar_set("stat_savesused", va("%d " P_ORANGE "(%d)", level.savesUsed, game.savesUsed + level.savesUsed));
	gi.cvar_set("stat_playedtime", va("%d.%02d.%02d " P_ORANGE "(%d.%02d.%02d)", hours, minutes, seconds, thours, tminutes, tseconds));
	gi.cvar_set("stat_cashearned", va("%d " P_ORANGE "(%d)", level.cashEarned, game.cashEarned + level.cashEarned));
}

void G_UpdateInven(void)
{
	if(!dm->isDM())
	{	// let the server know what kind of goodies we're coming out of here with - yeah!
		sharedEdict_t	sh;

		// Clear out any existing weapons
		gi.cvar_setvalue("knife", 0.0f);
		gi.cvar_setvalue("pistol1", 0.0f);
		gi.cvar_setvalue("pistol2", 0.0f);
		gi.cvar_setvalue("shotgun", 0.0f);
		gi.cvar_setvalue("sniper", 0.0f);
		gi.cvar_setvalue("assault", 0.0f);
		gi.cvar_setvalue("mpistol", 0.0f);
		gi.cvar_setvalue("machinegun", 0.0f);
		gi.cvar_setvalue("slugger", 0.0f);
		gi.cvar_setvalue("flamegun", 0.0f);
		gi.cvar_setvalue("rocket", 0.0f);
		gi.cvar_setvalue("mpg", 0.0f);

		gi.cvar_setvalue("grenade", 0.0f);
		gi.cvar_setvalue("fpak", 0.0f);
		gi.cvar_setvalue("goggles", 0.0f);
		gi.cvar_setvalue("c4", 0.0f);
		gi.cvar_setvalue("medkit", 0.0f);

		sh.inv = (inven_c *)g_edicts[1].client->inv;

		if (sh.inv)
		{
			sh.edict = &g_edicts[1];
			sh.inv->setOwner(&sh);
			sh.inv->buildInvForMenu(0, g_edicts[1].health);
		}
	}
}

const char *G_GetDMGameName(void)
{
	if(dm)
	{
		return(dm->getGameName());
	}
	return("Unknown");
}

void ResetCTFTeam(edict_t *ent)
{
	if (ent->client) 
		if(ent->client->resp.team)
		{
			ent->client->resp.team = NOTEAM;
		}
}

/*
=================
GetGameAPI

Returns a pointer to the structure with all entry points
and global variables
=================
*/

game_export_t *GetGameAPI (game_import_t *import)
{
	gi = *import;

	TheGhoul=(IGhoul *)gi.GetGhoul();

	{
		cvar_t	*ghl_mip;
		ghl_mip=gi.cvar("ghl_mip", "0", CVAR_ARCHIVE);
		int mip=floor(ghl_mip->value+0.5f);
		if (mip<0)
			mip=0;
		if (mip>2)
			mip=2;
		TheGhoul->SetMip(mip);
	}

	globals.apiversion = GAME_API_VERSION;
	globals.Init = InitGame;
	globals.Shutdown = ShutdownGame;
	globals.SpawnEntities = SpawnEntities;

	globals.WriteGame = WriteGame;
	globals.ReadGame = ReadGame;
	globals.WriteLevel = WriteLevel;
	globals.ReadLevel = ReadLevel;

	globals.ClientThink = ClientThink;
	globals.ClientConnect = ClientConnect;
	globals.ClientUserinfoChanged = ClientUserinfoChanged;
	globals.ClientDisconnect = ClientDisconnect;
	globals.ClientBegin = ClientBegin;
	globals.ClientCommand = ClientCommand;
	globals.ResetCTFTeam = ResetCTFTeam;

	globals.GameAllowASave = G_GameAllowASave;
	globals.SavesLeft = G_SavesLeft;
	globals.GetGameStats = G_GetGameStats;
	globals.UpdateInven = G_UpdateInven;
	globals.GetDMGameName = G_GetDMGameName;
	globals.GetCinematicFreeze = G_GetCinematicFreeze;
	globals.SetCinematicFreeze = G_SetCinematicFreeze;

	globals.RunFrame = G_RunFrame;

	globals.edict_size = sizeof(edict_t);

	return &globals;
}

#ifndef GAME_HARD_LINKED
// this is only here so the functions in q_shared.c and q_shwin.c can link
void Sys_Error (char *error, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	vsprintf (text, error, argptr);
	va_end (argptr);

	gi.error (ERR_FATAL, "%s", text);
}

void Com_Printf (char *msg, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	vsprintf (text, msg, argptr);
	va_end (argptr);

	gi.dprintf ("%s", text);
}

#endif

void Enemy_Printf (ai_public_c *which_ai, char *msg, ...)
{
	va_list		argptr;
	char		text[1024];

	if(!which_ai)
	{
		return;
	}

	if(which_ai->BeingWatched() == false)
	{
		return;
	}

	va_start (argptr, msg);
	vsprintf (text, msg, argptr);
	va_end (argptr);

	gi.dprintf ("%s", text);
}

//======================================================================


/*
=================
ClientEndServerFrames
=================
*/
void ClientEndServerFrames (void)
{
	int		i;
	edict_t	*ent;

	// calc the player views now that all pushing
	// and damage has been added
	for (i=0 ; i<(int)maxclients->value ; i++)
	{
		ent = g_edicts + 1 + i;
		if (!ent->inuse || !ent->client)
			continue;
		ClientEndServerFrame (ent);
	}

}

/*
=================
CreateTargetChangeLevel

Returns a pointer to newly created target changelevel entity.
=================
*/

edict_t *CreateTargetChangeLevel(char *mapname)
{
	edict_t *ent;

	ent = G_Spawn ();
	ent->classname = "target_changelevel";
	Com_sprintf(level.nextmap, sizeof(level.nextmap), "%s", mapname);
	ent->map = level.nextmap;
	
	return(ent);
}

/*
=================
EndDMLevel

The timelimit or fraglimit has been exceeded
=================
*/
void EndDMLevel (void)
{
	edict_t	*ent;
	char	*s, *t, *f;
	static const char *seps = " ,\n\r";


	// Stay on current level if DM flag tells us to do so.
	if(dm->dmRule_SAME_LEVEL())
	{
		BeginIntermission(CreateTargetChangeLevel(level.mapname));
		
		return;
	}

	// If we have a maplist, then look at that to see what map to load next.

	if (*sv_maplist->string)
	{
		s = strdup(sv_maplist->string);
		f = NULL;
		t = strtok(s, seps);
		
		while (t != NULL)
		{
			if (stricmp(t, level.mapname) == 0)
			{
				// Current level map is in the maplist, so go to the next map in
				// the maplist.

				t = strtok(NULL, seps);
				
				if (t == NULL)
				{ 
					// Current map is at end of the maplist, so try to loop back
					// to the first map in the maplist.

					if (f == NULL)
					{
						// But there isn't a first one, so just run the current
						// level map again.

						BeginIntermission (CreateTargetChangeLevel (level.mapname) );
					}
					else
					{
						BeginIntermission (CreateTargetChangeLevel (f) );
					}
				}
				else
				{
					BeginIntermission (CreateTargetChangeLevel (t) );
				}

				free(s);
				
				return;
			}

			if (!f)
			{
				f = t;
			}
			
			t = strtok(NULL, seps);
		}

		if(f)
		{
			BeginIntermission(CreateTargetChangeLevel(f));

			free(s);

			return;
		}
		else
			free(s);
	}

	if (level.nextmap[0])
	{
		// Go to a specific map.
		
		BeginIntermission (CreateTargetChangeLevel (level.nextmap) );
	}
	else
	{	
		// Search for a changelevel.

		ent = G_Find (NULL, FOFS(classname), "target_changelevel");
		
		if (!ent)
		{	
			// The map designer didn't include a changelevel, so create a fake
			// entity that goes back to the same level.

			BeginIntermission (CreateTargetChangeLevel (level.mapname) );
			
			return;
		}
		
		BeginIntermission (ent);
	}
}

/*
=================
CheckDMRules
=================
*/

void CheckDMRules (void)
{
	int			i;
	gclient_t	*cl;

	if (level.intermissiontime)
		return;

	if (!dm->isDM())
		return;

	// if we are done with this CTF game, end it.
	if ((deathmatch->value == DM_CTF) && (ctf_flag_captured->value))
	{
		gi.SP_Print (NULL, DM_CTF_END_GAME);
		EndDMLevel ();
		return;
	}

	if (timelimit->value)
	{
		// kef -- actually check versus the time each client joined, not against
		//level.time
		for (i=0 ; i<(int)maxclients->value ; i++)
		{
			if (!g_edicts[i+1].inuse)
				continue;
			if ( (g_edicts[i+1].client) && 
				 ((level.framenum - g_edicts[i+1].client->resp.enterframe) >= (timelimit->value * 600)) )
			{
				gi.SP_Print (NULL, DM_GENERIC_TIME_LIMIT_HIT, (short)timelimit->value);
				EndDMLevel ();
				return;
			}
		}
	}

	if (fraglimit->value)
	{
		
		if(dm->dmRule_TEAMPLAY())
		{
			struct	TeamInfo_s	teams[MAX_CLIENTS];
			int		score = 0, j, total_teams = 0;
			edict_t	*cl_ent;

			for (i=0 ; i<(int)maxclients->value ; i++)
			{
				cl_ent=g_edicts+1+i;

				if(!cl_ent->inuse)
				{
					continue;
				}

				// in a team game, we want to compare the team score to the fraglimit
				score=game.clients[i].resp.score;

				for(j=0;j<total_teams;j++)
				{
					if (OnSameTeam(g_edicts + 1 + teams[j].rep, cl_ent))
					{
						teams[j].score += score;
						break;
					}
				}

				if (j == total_teams)
				{
					teams[total_teams].rep = i;
					teams[total_teams].score = score;
					total_teams++;
				}
			}
			
			for (j=0;j<total_teams;j++)
			{
				if (teams[j].score >= fraglimit->value)
				{
					gi.SP_Print (NULL, DM_GENERIC_FRAG_LIMIT_HIT, (short)fraglimit->value);
					EndDMLevel ();
					return;
				}
			}
		}
		else
		{
			for (i=0 ; i<(int)maxclients->value ; i++)
			{
				cl = game.clients + i;
				if (!g_edicts[i+1].inuse)
					continue;

				if (cl->resp.score >= fraglimit->value)
				{
					gi.SP_Print (NULL, DM_GENERIC_FRAG_LIMIT_HIT, (short)fraglimit->value);
					EndDMLevel ();
					return;
				}
			}
		}
	}
}

/*
======================
FadeOutAfterDeath

  fade screen to black after single player death
======================
*/

bool FadeOutAfterDeath(void)
{
	int i = 0;
	bool	bRGBFinished = false, bAFinished = false;

	for (i = 0; i <3; i++)
	{
		if (g_edicts[1].client->fade_rgb[i] < 1.0f)
		{
			g_edicts[1].client->fade_rgb[i] = 0.0f;
			bRGBFinished = true;
		}
	}
	if (g_edicts[1].client->fade_alpha > 0.95)
	{
		g_edicts[1].client->fade_alpha = 1.0f;
		bAFinished = true;
	}
	else if (g_edicts[1].client->fade_alpha < 1.0f)
	{
		g_edicts[1].client->fade_alpha += 0.02f;
	}
	return (bRGBFinished && bAFinished);
}

/*
=============
ExitLevel
=============
*/


void ExitLevel (void)
{
	int		i;
	edict_t	*ent;
	char	command [256];

	Com_sprintf (command, sizeof(command), "gamemap \"%s\"\n", level.changemap);
	gi.AddCommandString (command);
	level.changemap = NULL;
	level.exitintermission = 0;
	level.intermissiontime = 0;
	ClientEndServerFrames ();

	// clear some things before going to next level

	dm->ClearFlagCount();

	// make sure we clear any flags related to being in a cinematic
	game.cinematicfreeze = 0;
	gi.cvar_set("sv_jumpcinematic","0");
	for (i=0 ; i<(int)maxclients->value ; i++)
	{
		ent = g_edicts + 1 + i;
		if (!ent->inuse)
			continue;
		if (ent->health > ent->client->pers.max_health)
		{
			ent->health = ent->client->pers.max_health;
		}
		ent->client->ps.gun = 0;
	}

	//sfs--need to register that ghoul models aren't available for weapon; this should also be different from disconnect, probably
	W_ShutdownWorldWeaponModels();
}

/*
==================
gamerules_c::clientRankings
==================
*/

typedef struct scoreboardSort_s
{
	int		sortedClients[MAX_CLIENTS];
	int		sortedScores[MAX_CLIENTS];
	int		sortedCount;
} scoreboardSort_t;

void clientRankings(void)
{
	scoreboardSort_t	scoreboardSort;
	edict_t				*cl_ent;
	int					cl_score,
						i,j,k;
	
	memset((void *)&scoreboardSort,0,sizeof(scoreboardSort));

	// Sort all the clients by score from highest to lowest.

	for(i=0;i<game.maxclients;i++)
	{
		cl_ent=g_edicts+1+i;

		if(!cl_ent->inuse)
			continue;

		// Spectators don't contribute to rankings.

		if(cl_ent->client->resp.spectator)
			continue;

		cl_score=game.clients[i].resp.score;

		for(j=0;j<scoreboardSort.sortedCount;j++)
		{
			if(cl_score>scoreboardSort.sortedScores[j])
				break;
		}

		for(k=scoreboardSort.sortedCount;k>j;k--)
		{
			scoreboardSort.sortedClients[k]=scoreboardSort.sortedClients[k-1];
			scoreboardSort.sortedScores[k]=scoreboardSort.sortedScores[k-1];
		}
		
		scoreboardSort.sortedClients[j]=i;
		scoreboardSort.sortedScores[j]=cl_score;
		scoreboardSort.sortedCount++;
	}

	// Set DM ranking for each client.

	for(i=0;i<scoreboardSort.sortedCount;i++)
	{
		cl_ent=g_edicts+1+scoreboardSort.sortedClients[i];
		cl_ent->client->ps.dmRank=i+1;
		cl_ent->client->ps.dmRankedPlyrs=scoreboardSort.sortedCount;
	}
}

/*
================
G_RunFrame

Advances the world by 0.1 seconds
================
*/

float G_RunFrame (int serverframe)
{
	int			i;
	edict_t		*ent;
#if TIME_ENTS
	timing_c	timing;
#endif	// _DEBUG

	if(freezeworld->value)
	{
		return level.time;
	}
#ifndef _FINAL_
//	Com_Printf(P_CYAN "%s\n", gi.GetLabel(&G_RunFrame));
#endif

	level.framenum++;
//	level.framenum=serverframe;
	level.time = level.framenum*FRAMETIME;

	// choose a client for monsters to target this frame
	gmonster.SetSightClient ();
	gmonster.UpdateEnemyPriority();

	UpdateWorldSpawningByFrame();

	// check for restart if the player's dead
	if (!dm->isDM())
	{
		if (g_edicts[1].client && g_edicts[1].health <= 0)
		{
			if (FadeOutAfterDeath())
			{	// finished fading out...go to load menu
				gi.AddCommandString ("respawn\n");
			}
		}
	}

	// exit intermissions

	if (level.exitintermission)
	{
		ExitLevel ();
		return level.time;
	}

	if ((serverframe<2&&ghl_precache_verts->value>0.0f)||(serverframe<1&&ghl_precache_texture->value>0.0f))
	{
		ent = &g_edicts[0];
		for (i=0 ; i<globals.num_edicts ; i++, ent++)
		{
			if (!ent->inuse)
				continue;
			if (ent->ghoulInst)
				ent->ghoulInst->GetGhoulObject()->PreCache(ghl_specular->value>0.0f&&ghl_light_method->value>0.0f);
//				ent->ghoulInst->PreCache(ghl_specular->value>0.0f&&ghl_light_method->value>0.0f,serverframe==0&&ghl_precache_texture->value>0.0f,ghl_precache_verts->value>0.0f);
		}
	}

	TheGhoul->BeginServerFrame();
	//
	// treat each object in turn
	// even the world gets a chance to think
	//
	level.num_clients = 0;

	ent = &g_edicts[0];
	for (i=0 ; i<globals.num_edicts ; i++, ent++)
	{
		if (!ent->inuse)
			continue;
		// If we are a client edict, increment the level.num_clients value
		if	((i) && (i <= (int)maxclients->value))
			level.num_clients++;

#if TIME_ENTS
		timing.Start();
#endif

		level.current_entity = ent;


		// if the ground entity moved, make sure we are still on it
		if ((ent->groundentity) && (ent->groundentity->linkcount != ent->groundentity_linkcount))
		{
			ent->groundentity = NULL;
			if ( !(ent->flags & (FL_SWIM|FL_FLY)) && (ent->ai) )
			{
				gmonster.CheckGround (ent);
			}
		}

		if (i > 0 && i <= (int)maxclients->value)
		{
			// have we allowed too many hostages to die?
			CheckDeadHostageCount(ent);

			ClientBeginServerFrame (ent);
#if TIME_ENTS
			if (ent->owner_spawn)
			{
				unsigned long l=(unsigned long)timing.End() / 1000;
				ent->owner_spawn->time_used += l;
				if (l>3*400)
				{
					char temp[1024];
					sprintf(temp,"%s took ~%d ms in the game loop\n",ent->owner_spawn->name,(int)l/400);
					OutputDebugString(temp);
				}
			}
#endif
			//ss added to allow players to use callbacks (for dm)
			PB_GhoulUpdate(*ent);

			continue;
		}

		if (ent->ghoulInst&&ghl_no_server_update->value<0.5f)
		{
			ent->ghoulInst->ServerUpdate(level.time);
		}

		G_RunEntity (ent);

#if TIME_ENTS
		if (ent->owner_spawn)
		{
			unsigned long l=(unsigned long)timing.End() / 1000;
			ent->owner_spawn->time_used += l;;
			if (l>3*400)
			{
				char temp[1024];
				sprintf(temp,"%s took ~%d ms in the game loop\n",ent->owner_spawn->name,(int)l/400);
				OutputDebugString(temp);
			}
		}
#endif
	}

	dm->checkEvents();

	ProcessScripts ();

	// See if it is time to end a deathmatch.
	CheckDMRules ();

	TheGhoul->EndServerFrame();

	// Build the playerstate_t structures for all players.
	ClientEndServerFrames ();

	// Generate DM rankings.

	if(dm->isDM())
		clientRankings();

	return level.time;
}






















// if we've allowed too many hostages to die in this level, we fail the mission.
void CheckDeadHostageCount(edict_t *player)
{
	if ((level.maxDeadHostages != -1) &&
		(level.deadHostages >= level.maxDeadHostages) &&
		player->client)
	{
		player_die(player, NULL, NULL, 99999, vec3_origin);
	}
}

int G_GameAllowASave(void)
{
	if(!level.savesLeft)
	{
		gi.SP_Print(NULL, SINGLEPLR_SAVES_NO_MORE);
		return(0);
	}

	level.savesLeft--;
	level.savesUsed++;

	if(level.savesLeft == 1)
	{
		gi.SP_Print(NULL, SINGLEPLR_SAVES_ONE_LEFT);
	}
	else if(level.savesLeft > 0)
	{
		gi.SP_Print(NULL, SINGLEPLR_SAVES_N_LEFT, level.savesLeft);
	}
	else if(level.savesLeft == 0)
	{
		gi.SP_Print(NULL, SINGLEPLR_SAVES_USED_UP);
	}
	return(1);
}

void G_SavesLeft(void)
{
	cvar_t	*savesleft;

	if(level.savesLeft < 0)
	{
		gi.cvar_set("menu_savesleft", gi.SP_GetStringText(SINGLEPLR_UNLIMITED));
	}
	else if(level.savesLeft > 0)
	{
		gi.cvar_setvalue("menu_savesleft", level.savesLeft);
	}
	else
	{
		gi.cvar_set("menu_savesleft", "0");
	}
	savesleft = gi.cvar("menu_savesleft", "", 0);
	gi.dprintf("Saves left : %s\n", savesleft->string);
}

// end
