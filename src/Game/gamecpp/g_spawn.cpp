
#include "g_local.h"
#include "g_spawn.h"
#include "fields.h"
#include "ai_pathfinding.h"
#include "p_body.h"
#include "..\qcommon\configstring.h"

#if TIME_ENTS
	#include <windows.h>
#endif

void CalculatePlayerEntryWeapons(int *weaponsAvailable);

byte	entSoundsToCache[NUM_CLSFX];
char	entDebrisToCache[NUM_CLGHL];

//	1/12/99 sfs -- this should fix the crummy randomness in pakfile creating
static	cvar_t	*fs_createpak;

extern spawn_t			environSpawns[];
extern spawn_t			funcSpawns[];
extern spawn_t			triggerSpawns[];
extern spawn_t			targetSpawns[];
extern spawn_t			lightSpawns[];
extern spawn_t			bosniaSpawns[];
extern spawn_t			castleSpawns[];
extern spawn_t			iraqSpawns[];
extern spawn_t			genericSpawns[];
extern spawn_t			newyorkSpawns[];
extern spawn_t			siberiaSpawns[];
extern spawn_t			tokyoSpawns[];
extern spawn_t			ugandaSpawns[];
extern spawn_t			dmSpawns[];

void CleanUpGame (void);

//  info_...
//void SP_info_merc_start (edict_t *ent);
void SP_info_notnull (edict_t *self);
void SP_info_null (edict_t *self); 
void SP_info_player_start (edict_t *ent);
void SP_info_player_deathmatch (edict_t *ent);
void SP_info_player_team1 (edict_t *ent);
void SP_info_player_team2 (edict_t *ent);
//void SP_info_player_coop (edict_t *ent);
void SP_info_player_intermission (edict_t *ent);

int ai_loadAllBolts = 0;


// misc non-id stuff
void SP_func_remote_camera(edict_t *Self);
void SP_misc_ctf_base (edict_t *self);


// misc_ id stuff
void SP_path_corner (edict_t *self);
void SP_point_combat (edict_t *self);
void SP_misc_gib_arm (edict_t *self);
void SP_misc_gib_leg (edict_t *self);
void SP_misc_gib_head (edict_t *self);

void SP_spawner(edict_t *ent);
void SP_spawner_monster(edict_t *ent);
void SP_spawner_boosterpack(edict_t *ent);

//  worldspawn...
void SP_worldspawn (edict_t *ent);

//  testing things
void SP_ghoul_model (edict_t *ent);
void SP_test_model (edict_t *ent);
void SP_test_rj (edict_t *ent);

extern void SP_script_runner (edict_t *ent);

void InitEntSoundCaching(void);
void InitEntMetalSoundCaching(void);
void InitEntDebrisCaching(void);
void CacheEntDebrisAndSounds(edict_t *ent);
void CacheAllDebris(void);
void CacheAllWallEffects(void);
void CacheAllSounds(void);

spawn_t	spawns[] =
{
	//  info_...
//	{"info_merc_start",					SP_info_merc_start},
	{"info_null",						SP_info_null},
	{"info_notnull",					SP_info_notnull},
//	{"info_player_coop",				SP_info_player_coop},
	{"info_player_deathmatch",			SP_info_player_deathmatch},
	{"info_player_team1",				SP_info_player_team1},
	{"info_player_team2",				SP_info_player_team2},
	{"info_player_intermission",		SP_info_player_intermission},
	{"info_player_start",				SP_info_player_start},
	
	//  misc_nonid stuff
	{"func_remote_camera",				SP_func_remote_camera},
	{"misc_ctf_base",					SP_misc_ctf_base},

	//  misc_ id stuff
	{"path_corner",						SP_path_corner},
	{"point_combat",					SP_point_combat},

	//  monster_...
	{"m_x_romulan",					SP_m_x_romulan},
	{"m_x_tank",					SP_m_x_tank},
	{"m_x_snowcat",					SP_m_x_snowcat},
	{"m_x_husky",					SP_m_x_husky},
	{"m_x_rottweiler",				SP_m_x_rottweiler},	
	{"m_x_germanshep",				SP_m_x_germanshep},
	{"m_x_raiderdog",				SP_m_x_raiderdog},
	{"m_x_bull",					SP_m_x_bull},
	{"m_x_chopper_black",			SP_m_x_chopper_black},
	{"m_x_chopper_green",			SP_m_x_chopper_green},
	{"m_x_chopper_white",			SP_m_x_chopper_white},
	{"m_x_hind",					SP_m_x_hind},
	{"m_x_mcharacter",				SP_m_x_mcharacter},
	{"m_x_mcharacter_snow",			SP_m_x_mcharacter_snow},
	{"m_x_mcharacter_desert",		SP_m_x_mcharacter_desert},
	{"m_x_mmerc",					SP_m_x_mmerc},
	{"m_x_mhurtmerc",				SP_m_x_mhurtmerc},
	{"m_x_msam",					SP_m_x_msam},
	{"m_x_miraqboss",				SP_m_x_miraqboss},
	{"m_x_mraiderboss1",			SP_m_x_mraiderboss1},
	{"m_x_mraiderboss2",			SP_m_x_mraiderboss2},
	{"m_x_ftaylor",					SP_m_x_ftaylor},
	{"m_x_mskinboss",				SP_m_x_mskinboss},

	{"m_sib_mtrooper1a",			SP_m_sib_mtrooper1a},
	{"m_sib_mtrooper1b",			SP_m_sib_mtrooper1b},
	{"m_sib_mtrooper2",				SP_m_sib_mtrooper2},
	{"m_sib_mguard1",				SP_m_sib_mguard1},
	{"m_sib_fguard2",				SP_m_sib_fguard2},
	{"m_sib_mguard3",				SP_m_sib_mguard3},
	{"m_sib_mguard4",				SP_m_sib_mguard4},
	{"m_sib_mcleansuit",			SP_m_sib_mcleansuit},
	{"m_sib_eofficer",				SP_m_sib_eofficer},
	{"m_sib_mmechanic",				SP_m_sib_mmechanic},
	{"m_sib_escientist1",			SP_m_sib_escientist1},
	{"m_sib_fscientist2",			SP_m_sib_fscientist2},

	{"m_afr_msoldier1a",			SP_m_afr_msoldier1a},
	{"m_afr_msoldier1b",			SP_m_afr_msoldier1b},
	{"m_afr_msoldier2",				SP_m_afr_msoldier2},
	{"m_afr_msoldier3",				SP_m_afr_msoldier3},
	{"m_afr_msniper",				SP_m_afr_msniper},
	{"m_afr_ecommander",			SP_m_afr_ecommander},
	{"m_afr_eworker",				SP_m_afr_eworker},
	{"m_afr_mbrute",				SP_m_afr_mbrute},
	{"m_afr_mrocket",				SP_m_afr_mrocket},

	{"m_kos_mgrunt1",				SP_m_kos_mgrunt1},
	{"m_kos_mgrunt2",				SP_m_kos_mgrunt2},
	{"m_kos_mgrunt3",				SP_m_kos_mgrunt3},
	{"m_kos_msniper1a",				SP_m_kos_msniper1a},
	{"m_kos_msniper1b",				SP_m_kos_msniper1b},
	{"m_kos_mcomtroop",				SP_m_kos_mcomtroop},
	{"m_kos_eofficer",				SP_m_kos_eofficer},
	{"m_kos_mbrute1a",				SP_m_kos_mbrute1a},
	{"m_kos_mbrute1b",				SP_m_kos_mbrute1b},
	{"m_kos_mmechanic",				SP_m_kos_mmechanic},
	{"m_kos_mrebel",				SP_m_kos_mrebel},
	{"m_kos_mklaleader",			SP_m_kos_mklaleader},
	{"m_kos_erefugee",				SP_m_kos_erefugee},

	{"m_tok_mman1",					SP_m_tok_mman1},
	{"m_tok_mman2",					SP_m_tok_mman2},
	{"m_tok_mhench1",				SP_m_tok_mhench1},
	{"m_tok_mhench2",				SP_m_tok_mhench2},
	{"m_tok_mkiller",				SP_m_tok_mkiller},
	{"m_tok_fassassin",				SP_m_tok_fassassin},
	{"m_tok_mninja",				SP_m_tok_mninja},
	{"m_tok_mbrute",				SP_m_tok_mbrute},
	{"m_tok_fwoman1",				SP_m_tok_fwoman1},
	{"m_tok_fwoman2",				SP_m_tok_fwoman2},

	{"m_irq_msoldier1",				SP_m_irq_msoldier1},
	{"m_irq_msoldier2a",			SP_m_irq_msoldier2a},
	{"m_irq_msoldier2b",			SP_m_irq_msoldier2b},
	{"m_irq_mrepguard1a",			SP_m_irq_mrepguard1a},
	{"m_irq_mrepguard1b",			SP_m_irq_mrepguard1b},
	{"m_irq_mpolice",				SP_m_irq_mpolice},
	{"m_irq_eofficer",				SP_m_irq_eofficer},
	{"m_irq_mcommander",			SP_m_irq_mcommander},
	{"m_irq_mbrute1a",				SP_m_irq_mbrute1a},
	{"m_irq_mbrute1b",				SP_m_irq_mbrute1b},
	{"m_irq_mbodyguard",			SP_m_irq_mbodyguard},
	{"m_irq_mrocket",				SP_m_irq_mrocket},
	{"m_irq_msaddam",				SP_m_irq_msaddam},
	{"m_irq_moilworker",			SP_m_irq_moilworker},
	{"m_irq_eman1",					SP_m_irq_eman1},
	{"m_irq_mman2",					SP_m_irq_mman2},
	{"m_irq_fwoman1",				SP_m_irq_fwoman1},
	{"m_irq_fwoman2",				SP_m_irq_fwoman2},

	{"m_cas_mraider1",				SP_m_cas_mraider1},
	{"m_cas_mraider2a",				SP_m_cas_mraider2a},
	{"m_cas_mraider2b",				SP_m_cas_mraider2b},
	{"m_cas_mbrute",				SP_m_cas_mbrute},
	{"m_cas_ffemale",				SP_m_cas_ffemale},
	{"m_cas_mrocket",				SP_m_cas_mrocket},

	{"m_nyc_mskinhead1",			SP_m_nyc_mskinhead1},
	{"m_nyc_mskinhead2a",			SP_m_nyc_mskinhead2a},
	{"m_nyc_mskinhead2b",			SP_m_nyc_mskinhead2b},
	{"m_nyc_eskinhead3",			SP_m_nyc_eskinhead3},
	{"m_nyc_fskinchick",			SP_m_nyc_fskinchick},
	{"m_nyc_mpunk",					SP_m_nyc_mpunk},
	{"m_nyc_epunk",					SP_m_nyc_epunk},
	{"m_nyc_mswat",					SP_m_nyc_mswat},
	{"m_nyc_mswatleader",			SP_m_nyc_mswatleader},
	{"m_nyc_estockbroker",			SP_m_nyc_estockbroker},
	{"m_nyc_ebum",					SP_m_nyc_ebum},
	{"m_nyc_etourist",				SP_m_nyc_etourist},
	{"m_nyc_mpolitician",			SP_m_nyc_mpolitician},
	{"m_nyc_fwoman",				SP_m_nyc_fwoman},
	

	{"script_runner",					SP_script_runner},

	{"spawner",							SP_spawner},
	{"spawner_monster",					SP_spawner_monster},
	{"spawner_boosterpack",				SP_spawner_boosterpack},

	// worldspawn
	{"worldspawn",						SP_worldspawn},
	
	//  testing stuff
	{"ghoul_model",						SP_ghoul_model},
	{"test_model",						SP_test_model},
	{"test_rj",							SP_test_rj},


	{NULL,								NULL}
};

field_t fields[] =
{
	{"classname", FOFS(classname), F_STRING},
	{"origin", FOFS(s.origin), F_VECTOR},
	{"model", FOFS(model), F_STRING},
	{"spawnflags", FOFS(spawnflags), F_INT},
	{"speed", FOFS(speed), F_FLOAT},
	{"accel", FOFS(accel), F_FLOAT},
	{"decel", FOFS(decel), F_FLOAT},
	{"target", FOFS(target), F_STRING},
	{"targetname", FOFS(targetname), F_STRING},
	{"scripttarget", FOFS(scripttarget), F_STRING},
	{"pathtarget", FOFS(pathtarget), F_STRING},
	{"deathtarget", FOFS(deathtarget), F_STRING},
	{"killtarget", FOFS(killtarget), F_STRING},
	{"killfacing", FOFS(killfacing), F_STRING},
	{"combattarget", FOFS(combattarget), F_STRING},
	{"soundname", FOFS(soundName), F_STRING},
	{"spawn1", FOFS(spawn1), F_STRING},
	{"spawn2", FOFS(spawn2), F_STRING},
	{"spawn3", FOFS(spawn3), F_STRING},
	{"message", FOFS(message), F_STRING},
	{"sp_message", FOFS(sp_message), F_INT},
	{"team", FOFS(team), F_STRING},
	{"wait", FOFS(wait), F_FLOAT},
	{"delay", FOFS(delay), F_FLOAT},
	{"random", FOFS(random), F_FLOAT},
	{"style", FOFS(style), F_INT},
	{"count", FOFS(count), F_INT},
	{"health", FOFS(health), F_INT},
	{"sounds", FOFS(sounds), F_INT},
	{"light", 0, F_IGNORE},
	{"dmg", FOFS(dmg), F_INT},
	{"angles", FOFS(s.angles), F_VECTOR},
	{"angle", FOFS(s.angles), F_ANGLEHACK},
	{"mass", FOFS(mass), F_INT},
	{"volume", FOFS(volume), F_FLOAT},
	{"attenuation", FOFS(attenuation), F_FLOAT},
	{"map", FOFS(map), F_STRING},
	{"skin",FOFS(s.skinnum), F_INT},
	{"alertradius", FOFS(alertRadius), F_FLOAT},
	{"personality", FOFS(personality), F_INT},

	// temp spawn vars -- only valid when the spawn function is called
	{"lip", STOFS(lip), F_INT, FFL_SPAWNTEMP},
	{"distance", STOFS(distance), F_INT, FFL_SPAWNTEMP},
	{"height", STOFS(height), F_INT, FFL_SPAWNTEMP},
	{"noise", STOFS(noise), F_STRING, FFL_SPAWNTEMP},
	{"pausetime", STOFS(pausetime), F_FLOAT, FFL_SPAWNTEMP},
	{"item", STOFS(item), F_STRING, FFL_SPAWNTEMP},
	{"gravity", STOFS(gravity), F_STRING, FFL_SPAWNTEMP},
	{"sky", STOFS(sky), F_STRING, FFL_SPAWNTEMP},
	{"skyrotate", STOFS(skyrotate), F_FLOAT, FFL_SPAWNTEMP},
	{"skyaxis", STOFS(skyaxis), F_VECTOR, FFL_SPAWNTEMP},
	{"terrainset", STOFS(terrainSet), F_STRING, FFL_SPAWNTEMP},
	{"musicSet", STOFS(musicSet), F_STRING, FFL_SPAWNTEMP},
	{"color", STOFS(color), F_VECTOR, FFL_SPAWNTEMP},
	{"endpoint", STOFS(endpoint), F_VECTOR, FFL_SPAWNTEMP},
	{"minyaw", STOFS(minyaw), F_FLOAT, FFL_SPAWNTEMP},
	{"maxyaw", STOFS(maxyaw), F_FLOAT, FFL_SPAWNTEMP},
	{"minpitch", STOFS(minpitch), F_FLOAT, FFL_SPAWNTEMP},
	{"maxpitch", STOFS(maxpitch), F_FLOAT, FFL_SPAWNTEMP},
	{"nextmap", STOFS(nextmap), F_STRING, FFL_SPAWNTEMP},
	{"script", STOFS(script), F_STRING, FFL_SPAWNTEMP},
	{"killedvalue", STOFS(killedValue), F_INT, FFL_SPAWNTEMP},
	{"survivalvalue", STOFS(survivalValue), F_INT, FFL_SPAWNTEMP},
	{"material", STOFS(material), F_INT, FFL_SPAWNTEMP},
	{"scale", STOFS(scale), F_FLOAT, FFL_SPAWNTEMP},
	{"merctype", STOFS(merctype), F_INT, FFL_SPAWNTEMP},
	{"mercnum", STOFS(mercnum), F_INT, FFL_SPAWNTEMP},
	{"waitaction1", STOFS(waitAction1), F_STRING, FFL_SPAWNTEMP},
	{"waitaction2", STOFS(waitAction2), F_STRING, FFL_SPAWNTEMP},
	{"waitaction3", STOFS(waitAction3), F_STRING, FFL_SPAWNTEMP},
	{"waitaction4", STOFS(waitAction4), F_STRING, FFL_SPAWNTEMP},
	{"waitaction5", STOFS(waitAction5), F_STRING, FFL_SPAWNTEMP},
	{"waitaction6", STOFS(waitAction6), F_STRING, FFL_SPAWNTEMP},
	{"moveaction", STOFS(moveAction), F_STRING, FFL_SPAWNTEMP},
	{"screeneffect", STOFS(screeneffect), F_INT, FFL_SPAWNTEMP},
	{"spawnfrequency", STOFS(spawnfrequency), F_FLOAT, FFL_SPAWNTEMP},
	{"parm1", STOFS(parms[0]), F_STRING, FFL_SPAWNTEMP},
	{"parm2", STOFS(parms[1]), F_STRING, FFL_SPAWNTEMP},
	{"parm3", STOFS(parms[2]), F_STRING, FFL_SPAWNTEMP},
	{"parm4", STOFS(parms[3]), F_STRING, FFL_SPAWNTEMP},
	{"parm5", STOFS(parms[4]), F_STRING, FFL_SPAWNTEMP},
	{"parm6", STOFS(parms[5]), F_STRING, FFL_SPAWNTEMP},
	{"parm7", STOFS(parms[6]), F_STRING, FFL_SPAWNTEMP},
	{"parm8", STOFS(parms[7]), F_STRING, FFL_SPAWNTEMP},
	{"parm9", STOFS(parms[8]), F_STRING, FFL_SPAWNTEMP},
	{"parm10", STOFS(parms[9]), F_STRING, FFL_SPAWNTEMP},
	{"surfacetype", STOFS(surfaceType), F_INT, FFL_SPAWNTEMP},
	{"noweapons", STOFS(noweapons), F_INT, FFL_SPAWNTEMP},
	{"setweapons", STOFS(setweapons), F_INT, FFL_SPAWNTEMP},
	{"maxDeadHostages", STOFS(maxDeadHostages), F_INT, FFL_SPAWNTEMP},
	{"ambientSet", STOFS(ambientSet), F_STRING, FFL_SPAWNTEMP},
	{"startmusic", STOFS(startmusic), F_INT, FFL_SPAWNTEMP},
	{"forceHUD", STOFS(forceHUD), F_INT, FFL_SPAWNTEMP},
	{"weaponsAvailable", STOFS(weaponsAvailable), F_INT, FFL_SPAWNTEMP},
	{"ainame", STOFS(ai_name), F_STRING, FFL_SPAWNTEMP},
	
	// BSP dummy options
	{"distcull", STOFS(dummy), F_INT, FFL_SPAWNTEMP},
	{"chopsky", STOFS(dummy), F_INT, FFL_SPAWNTEMP},

	{NULL, 0, F_INT}
};

typedef spawn_t *spawnPtr;

spawnPtr spawnLists[] =
{
	environSpawns,
	funcSpawns,
	triggerSpawns,
	targetSpawns,
	lightSpawns,
	bosniaSpawns,
	castleSpawns,
	iraqSpawns,
	genericSpawns,
	newyorkSpawns,
	siberiaSpawns,
	tokyoSpawns,
	ugandaSpawns,
	dmSpawns,
	spawns,
	NULL
};

#if TIME_ENTS

void ClearTimings(bool DoCount)
{
	spawnPtr		*table;
	spawn_t			*s;

	for(table = spawnLists; *table; table++)
	{
		for (s=*table; s->name ; s++)
		{
			s->time_used = 0;
			if (DoCount)
			{
				s->count = 0;
			}
		}
	}
}

void PrintTimings(void)
{
	spawnPtr		*table;
	spawn_t			*s;
	char			temp[1024];
	unsigned long total=0;
	multimap<long,pair<char *,int> > sortit;

	for(table = spawnLists; *table; table++)
	{
		for (s=*table; s->name ; s++)
		{
			if (s->time_used)
			{
				sortit.insert(pair<long,pair<char *,int> >(-s->time_used,pair<char *,int>(s->name, s->count)));
				total+=s->time_used;
			}
		}
	}
	if (total)
	{
		OutputDebugString("Entity Timing Report\n");
		OutputDebugString("================================\n");
		multimap<long,pair<char *,int> >::iterator i;
		for (i=sortit.begin();i!=sortit.end();i++)
		{
			float p=100.0f*float(-(*i).first)/float(total);
			sprintf(temp,"%4.1f\t%3d\t%s\n",p,(*i).second.second,(*i).second.first);
			OutputDebugString(temp);
		}
		OutputDebugString("================================\n");
	}
}

#endif 
 
/*
===============
ED_CallSpawn

Finds the spawn function for the entity and calls it
===============
*/
void ED_CallSpawn (edict_t *ent)
{
	spawnPtr		*table;
	spawn_t			*s;
	Pickup			*pickup = NULL;

	if (!ent->classname)
	{
		gi.dprintf ("ED_CallSpawn: NULL classname\n");
		return;
	}

	if (ent->classname[0] == '_')
	{
		G_FreeEdict(ent);
		return;
	}

	// Don't spawn fluff objects.
	if (gl_pictip->value)
	{
		if ((strncmp (ent->classname, "misc_", 5)==0) && (ent->spawnflags & SF_FLUFF))
		{
			G_FreeEdict(ent);
			return;
		}
	}

	// Check item spawn functions.
	if (pickup = thePickupList.GetPickupFromEdict(ent))
	{
		if (dm->checkItemSpawn(ent, &pickup))
		{
			I_Spawn(ent, pickup);
		}
		return;
	}

	// Check normal spawn functions.
	for(table = spawnLists; *table; table++)
	{
		for (s=*table; s->name ; s++)
		{
			if (!strcmp(s->name, ent->classname))
			{	
				// Found it.
				s->spawn (ent);
#if TIME_ENTS
				ent->owner_spawn = s;
				s->count++;
#endif	
				return;
			}
		}
	}

	// No spawn func == no exist.
	gi.dprintf ("%s doesn't have a spawn function\n", ent->classname);
	G_FreeEdict(ent);
}

/*
=============
ED_NewString
=============
*/
char *ED_NewString (char *string)
{
	char	*newb, *new_p;
	int		i,l;
	
	l = strlen(string) + 1;

	newb = (char*)gi.TagMalloc (l, TAG_LEVEL);

	new_p = newb;

	for (i=0 ; i< l ; i++)
	{
		if (string[i] == '\\' && i < l-1)
		{
			i++;
			if (string[i] == 'n')
				*new_p++ = '\n';
			else
				*new_p++ = '\\';
		}
		else
			*new_p++ = string[i];
	}
	
	return newb;
}

/*
===============
ED_ParseField

Takes a key/value pair and sets the binary values
in an edict
===============
*/
void ED_ParseField (char *key, char *value, edict_t *ent)
{
	field_t	*f;
	byte	*b;
	float	v;
	vec3_t	vec;

	for (f=fields ; f->name ; f++)
	{
		if (!stricmp(f->name, key))
		{	// found it
			if (f->flags & FFL_SPAWNTEMP)
				b = (byte *)&st;
			else
				b = (byte *)ent;

			switch (f->type)
			{
			case F_STRING:
				*(char **)(b+f->ofs) = ED_NewString (value);
				break;
			case F_VECTOR:
				sscanf (value, "%f %f %f", &vec[0], &vec[1], &vec[2]);
				((float *)(b+f->ofs))[0] = vec[0];
				((float *)(b+f->ofs))[1] = vec[1];
				((float *)(b+f->ofs))[2] = vec[2];
				break;
			case F_INT:
				*(int *)(b+f->ofs) = atoi(value);
				break;
			case F_SHORT:
				*(short *)(b+f->ofs) = (short)atoi(value);
				break;
			case F_FLOAT:
				*(float *)(b+f->ofs) = atof(value);
				break;
			case F_ANGLEHACK:
				v = atof(value);
				((float *)(b+f->ofs))[0] = 0;
				((float *)(b+f->ofs))[1] = v;
				((float *)(b+f->ofs))[2] = 0;
				break;
			case F_IGNORE:
				break;
			}
			return;
		}
	}
	gi.dprintf ("%s is not a field\n", key);
}

/*
====================
ED_ParseEdict

Parses an edict out of the given string, returning the new position
ed should be a properly initialized empty edict.
====================
*/
char *ED_ParseEdict (char *data, edict_t *ent)
{
	qboolean	init;
	char		keyname[256];
	char		*com_token;

	init = false;
	memset (&st, 0, sizeof(st));

// go through all the dictionary pairs
	while (1)
	{	
	// parse key
		com_token = COM_Parse (&data);
		if (com_token[0] == '}')
			break;
		if (!data)
			gi.error ("ED_ParseEntity: EOF without closing brace");

		strncpy (keyname, com_token, sizeof(keyname)-1);
		
	// parse value	
		com_token = COM_Parse (&data);
		if (!data)
			gi.error ("ED_ParseEntity: EOF without closing brace");

		if (com_token[0] == '}')
			gi.error ("ED_ParseEntity: closing brace without data");

		init = true;	

	// keynames with a leading underscore are used for utility comments,
	// and are immediately discarded by quake
		if (keyname[0] == '_')
			continue;

		ED_ParseField (keyname, com_token, ent);
	}

	if (!init)
		memset (ent, 0, sizeof(*ent));

	return data;
}


/*
================
G_FindTeams

Chain together all entities with a matching team field.

All but the first will have the FL_TEAMSLAVE flag set.
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams (void)
{
	edict_t	*e, *e2, *chain;
	int		i, j;
	int		c, c2;

	c = 0;
	c2 = 0;
	for (i=1, e=g_edicts+i ; i < globals.num_edicts ; i++,e++)
	{
		if (!e->inuse)
			continue;
		if (!e->team)
			continue;
		if (e->flags & FL_TEAMSLAVE)
			continue;
		chain = e;
		e->teammaster = e;
		c++;
		c2++;
		for (j=i+1, e2=e+1 ; j < globals.num_edicts ; j++,e2++)
		{
			if (!e2->inuse)
				continue;
			if (!e2->team)
				continue;
			if (e2->flags & FL_TEAMSLAVE)
				continue;
			if (!strcmp(e->team, e2->team))
			{
				c2++;
				chain->teamchain = e2;
				e2->teammaster = e;
				chain = e2;
				e2->flags |= FL_TEAMSLAVE;
			}
		}
	}

	gi.dprintf ("%i teams with %i entities\n", c, c2);
}

void HackPrecacheLevelSounds(void);


void RegisterCommonStringPackets(void)
{
	gi.SP_Register("items");
	gi.SP_Register("weapons");
	gi.SP_Register("general");
}


/*
==============
SpawnEntities

Creates a server's entity / program execution context by
parsing textual entity definitions out of an ent file.
==============
*/

edict_t*	WorldSpawnScriptRunner;
extern float walldamagetime;		// rjr: yeah, yucky extern here, but reserving real words for what I feel...

void SpawnEntities (char *mapname, char *entities, char *spawnpoint)
{
	edict_t		*ent;
	int			inhibit;
	char		*com_token;
	int			i;

	WorldSpawnScriptRunner = NULL;
	
	PreserveClientData ();

	ShutdownScripts();
	gi.FreeTags (TAG_LEVEL);

	memset (&level, 0, sizeof(level));
	G_FreeAllEdicts();

	gmonster.Init();

	strncpy (level.mapname, mapname, sizeof(level.mapname)-1);
	strncpy (game.spawnpoint, spawnpoint, sizeof(game.spawnpoint)-1);

	// set client fields on player ents
	for (i=0 ; i<game.maxclients ; i++)
	{
		g_edicts[i+1].health = 100;
		g_edicts[i+1].client = game.clients + i;
	}

	ent = NULL;
	inhibit = 0;
	game.cinematicfreeze = 0;

	gi.Update(0.05f, true);
	//cache all the ghoul stuff we're going to need for this level
	game_ghoul.LevelPrecache();

	gi.Update(0.075f, true);
	W_InitWorldWeaponModels();
	
	// init list of client sfx and debris models needed for entities
	InitEntSoundCaching();

	InitEntMetalSoundCaching(); // this is necessary because the routine that is supposed to read in textures isn't 
								// properly reading them in on ladders.  Since I doubt that routine is going to be 
								// fixed (which is the proper solution), I poop this here.

	InitEntDebrisCaching();

	FX_Init();
	InitDeathmatchSystem();

	gi.Update(0.1f, true);
	RegisterCommonStringPackets();

	aiPoints.load();

	gi.Update(0.15f, true);
	PB_PrecacheAllPlayerModels();

	// 12/27/99 kef -- just need to call this sometime before we start making calls to ED_CallSpawn. it inits
	//our global list of pickup items. see CWeaponInfo.h.
	thePickupList.PreSpawnInit();

#if TIME_ENTS
	ClearTimings(true);
#endif	// _DEBUG

	fs_createpak = gi.cvar("fs_createpak", "0", 0);
	if (fs_createpak && fs_createpak->value)
	{
		ai_loadAllBolts=1;
	}
// parse ents
	while (1)
	{
		// parse the opening brace	
		com_token = COM_Parse (&entities);
		if (!entities)
			break;
		if (com_token[0] != '{')
			gi.error ("ED_LoadFromFile: found %s when expecting {",com_token);

		if (!ent)
			ent = g_edicts;
		else
			ent = G_Spawn ();
		entities = ED_ParseEdict (entities, ent);

		// remove things (except the world) from different skill levels or deathmatch
		if (ent != g_edicts)
		{
			if (dm->isDM())
			{
				if ( ent->spawnflags & SPAWNFLAG_NOT_DEATHMATCH )
				{
					G_FreeEdict (ent);	
					inhibit++;
					continue;
				}
			}

			else
			{

				if ((((game.playerSkills.getEnemyValue() == 0) || (game.playerSkills.getEnemyValue() == 1)) && (ent->spawnflags & SPAWNFLAG_NOT_EASY)) ||
					(((game.playerSkills.getEnemyValue() == 2)) && (ent->spawnflags & SPAWNFLAG_NOT_MEDIUM)) ||
					(((game.playerSkills.getEnemyValue() == 3) || (game.playerSkills.getEnemyValue() == 4)) && (ent->spawnflags & SPAWNFLAG_NOT_HARD)))
				{
						G_FreeEdict (ent);	
						inhibit++;
						continue;
				}
				// 12/27/99 kef -- here is where we should remove fluff objects...currently done in ED_CallSpawn, I think
			}


			ent->spawnflags &= ~(SPAWNFLAG_NOT_EASY|SPAWNFLAG_NOT_MEDIUM|SPAWNFLAG_NOT_HARD|SPAWNFLAG_NOT_DEATHMATCH);
//			ent->spawnflags &= ~(SPAWNFLAG_NOT_DEATHMATCH);
		}

		// put skill level item check here -- dk wubba

		ED_CallSpawn (ent);
		// cache necessary debris models based on ent's material
		CacheEntDebrisAndSounds(ent);
	}
	
	// 1/31/00 kef -- just need to call this sometime after spawning all pickups. see CWeaponInfo.h.
	thePickupList.PostSpawnInit();


	ai_loadAllBolts=0;

	// Gametype specific initialisation.

	dm->levelInit();

	gi.Update(0.25f, true);
	CacheAllDebris();
	CacheAllSounds();
	CacheAllWallEffects();

	gi.dprintf ("%i entities inhibited\n", inhibit);

	G_FindTeams ();

	PlayerTrail_Init ();

	gi.Update(0.28f, true);
	SndInitWorldSounds();

	CalculatePlayerEntryWeapons(&level.weaponsAvailable);

	gi.Update(0.30f, true);
	
	// Clear out all the (now invalid) pointers to the weapon ghoul objects.
	pe->UncacheViewWeaponModels();

	// level.weaponsAvailable - this will take care of everything enemies are 
	// carrying / items lying around - players come later.
	pe->PrecacheViewWeaponModels(level.weaponsAvailable);

	gi.CreateGhoulConfigStrings();
	gi.Update(0.33f, true);

	walldamagetime = -999999.0;
}


//===================================================================

void PrecacheSurfaceTypes(void)
{
	byte List[256], count;

	count = gi.SurfaceTypeList(List, sizeof(List));

	while(count)
	{
		count--;

		switch(List[count])
		{
		case SURF_DEFAULT:
			break;
		case SURF_METAL:
			break;
		case SURF_SAND_YELLOW:
			break;
		case SURF_SAND_WHITE:
			break;
		case SURF_SAND_LBROWN:
			break;
		case SURF_SAND_DBROWN:
			break;

		case SURF_GRAVEL_GREY:
			break;
		case SURF_GRAVEL_DBROWN:
			break;
		case SURF_GRAVEL_LBROWN:
			break;

		case SURF_SNOW:
			break;

		case SURF_LIQUID_BLUE:
			break;
		case SURF_LIQUID_GREEN:
			break;
		case SURF_LIQUID_ORANGE:
			break;
		case SURF_LIQUID_BROWN:
			break;

		case SURF_WOOD_LBROWN:
			break;
		case SURF_WOOD_DBROWN:
			break;
		case SURF_WOOD_LGREY:
			break;

		case SURF_STONE_LGREY:			
			break;
		case SURF_STONE_DGREY:			
			break;
		case SURF_STONE_LBROWN:			
			break;
		case SURF_STONE_DBROWN:			
			break;
		case SURF_STONE_WHITE:			
			break;
		case SURF_STONE_GREEN:			
			break;
		case SURF_STONE_RED:			
			break;
		case SURF_STONE_BLACK:			
			break;

		case SURF_GRASS_GREEN:
			break;
		case SURF_GRASS_BROWN:
			break;

		case SURF_LIQUID_RED:
			break;

		case SURF_METAL_STEAM:
			break;
		case SURF_METAL_WATERJET:
			break;
		case SURF_METAL_OILSPURT:
			break;
		case SURF_METAL_CHEMSPURT:
			break;
		case SURF_METAL_COMPUTERS:
			break;

		case SURF_SNOW_LBROWN:
			break;
		case SURF_SNOW_GREY:
			break;
		case SURF_BLOOD:
			break;
		case SURF_LIQUID_BLACK:
			break;
		case SURF_GLASS:
			break;
		case SURF_GLASS_COMPUTER:
			break;
		case SURF_SODAMACHINE:
			break;
		case SURF_PAPERWALL:
			break;
		case SURF_NEWSPAPER_DAMAGE:
			break;


		}
	}
}

void PrecacheSpawningEnemies(edict_t *ent)
{
	edict_t *newGuy;
	char *curSpawn;
	int oldLoadAllBolts=ai_loadAllBolts;

	ai_loadAllBolts = 1;

	for(int i = 0; i < 3; i++)
	{
		switch(i)
		{
		case 0:
			curSpawn = ent->spawn1;
			break;
		case 1:
			curSpawn = ent->spawn2;
			break;
		case 2:
			curSpawn = ent->spawn3;
			break;
		}

		if(!curSpawn)
		{
			continue;
		}

		if(!strcmp(curSpawn, ""))
		{
			continue;
		}

		newGuy = G_Spawn();

		newGuy->classname = curSpawn;
		ED_CallSpawn (newGuy);
		G_FreeEdict(newGuy);
	}
	ai_loadAllBolts = oldLoadAllBolts;
}


/*QUAKED worldspawn (0 0 0) ?

Only used for the world.
"sky"	environment map name
"skyaxis"	vector axis for rotating sky
"skyrotate"	speed of rotation in degrees/second
"sounds"	music cd track number
"gravity"	800 is default gravity
"message"	text to print at user logon
"musicset"		filename of the musicset for this level
"terrainset"	filename of the terrainset for this level
"distcull"      sets the maximum distance for vis (100 is good)
"chop"          sets the surface patch size for lighting
"chopsky"       sets the surface patch size for sky
"chopwarp"      sets the surface patch size for warp surfaces
"script"		filename of a script to run at map loadtime
"raffel"        set this to 1 if you want your map to run slower
"jersey"		set this to 1 to make all enemies talk funny
"screeneffect"	sets the style of general screen effect
"spawn1"
"spawn2"
"spawn3"        types of guys that will be spawned into the world from combat
"spawnfrequency"  0..1 (from no guys to very likely) - default is 0
"noweapons"		for levels like the armory where the player gets no weapons. set to 1 for no weapons.
"setweapons"		set the player's inventory. For TRN1 and TSR1 use 1. overridden by noweapons.
"maxDeadHostages"	number of dead hostages allowed before you fail this mission
"ambientset"		filename of the ambient set for this level
"startmusic"		set if you want to force a specific song at startup
"forceHUD"			set this for the tutorial level. draws HUD even with no weapon.
"weaponsAvailable"  magic number for precaching weapons. ask a programmer. (68 for tutorial level)
"ainame"            over-ride for per-level enemy set
*/

void SP_worldspawn (edict_t *ent)
{
	int		nWeapons = -1;

	ent->movetype = MOVETYPE_PUSH;
	ent->solid = SOLID_BSP;
	ent->inuse = true;			// since the world doesn't use G_Spawn()
	ent->s.modelindex = 1;		// world model is always index 1

	level.spawnFrequency = st.spawnfrequency;
	level.playerLoudness = 0;//defaults to this
	level.spawnRushTime = -999;
	level.spawnSoundTime = 0;
	level.lullTime = 0;
	level.lastSpawn = 0;
	level.weaponsAvailable = 0;
	level.lastEnemyKill = 0;
	memset(level.clearedNodes, 0, MAX_NODES);
	if (st.ai_name)
	{
		strcpy(level_ai_name, st.ai_name);
	}
	else
	{
		level_ai_name[0] = 0;
	}

	level.forceHUD = st.forceHUD;
	level.weaponsAvailable = st.weaponsAvailable;
	if (st.noweapons)
	{
		nWeapons = 0;
	}
	else if (st.setweapons)
	{
		nWeapons = st.setweapons;
	}
	if (nWeapons >= 0)
	{
		// oh	my	gosh	this	is	so	lame
		if (!ent->objInfo)
		{ // create a baseObjInfo_c for our edict
			baseObjInfo_c	*newInfo = new baseObjInfo_c(ent);
			
			// somewhere in the newInfo struct note the fact that this is a "no weapons" level
			if (ent->objInfo = newInfo)
			{
				objFlipInfo_c *pFlipInfo = (objFlipInfo_c*)newInfo->GetInfo(OIT_FLIP);
				switch(nWeapons)
				{
				case 0:
					pFlipInfo->radius = -1;		// tried to make this lamer. couldn't.
					break;
				case 1:
					pFlipInfo->radius = -2;
					break;
				default:
					break;
				}
			}
		}
	}

	if (st.maxDeadHostages)
	{	// how many hostages are allowed to die before you fail this mission
		level.maxDeadHostages = st.maxDeadHostages;
	}
	else
	{	// if there's no value here, save a good flag-value
		level.maxDeadHostages = -1;
	}

	// init the status of our countdown timer (if there isn't one in this level, who cares?)
	level.countdownEnded = 0;
	// just starting the mission...no status yet
	level.missionStatus = MISSION_NONE;

	//---------------

	// reserve some spots for dead player bodies for coop / deathmatch
	InitBodyQue ();

	// set configstrings for items
//	SetItemNames ();

	if (st.nextmap)
		strcpy (level.nextmap, st.nextmap);

	// make some data visible to the server

	if (ent->message && ent->message[0])
	{
		gi.configstring (CS_NAME, ent->message);
		strncpy (level.level_name, ent->message, sizeof(level.level_name));
	}
	else
		strncpy (level.level_name, level.mapname, sizeof(level.level_name));

	if (st.sky && st.sky[0])
		gi.configstring (CS_SKY, st.sky);
	else
		gi.configstring (CS_SKY, "none");

	if (st.terrainSet && st.terrainSet[0])
	{
		gi.configstring (CS_TERRAINNAME, va("%s", st.terrainSet));
	}

	gi.configstring (CS_SKYROTATE, va("%f", st.skyrotate) );

	gi.configstring (CS_SKYAXIS, va("%f %f %f",
		st.skyaxis[0], st.skyaxis[1], st.skyaxis[2]) );

	gi.configstring (CS_CDTRACK, va("%i", ent->sounds) );

	gi.configstring (CS_MAXCLIENTS, va("%i", (int)(maxclients->value) ) );

	gi.configstring (CS_SCREENEFFECT, va("%i", (int)(st.screeneffect) ) );

	if (st.musicSet && st.musicSet[0])
	{
		gi.configstring (CS_MUSICSET, va("%s", st.musicSet) );
	}

	// set ambient sound set name.
	// if its blank, then force it to "sounds/sound.amb"
	if (st.ambientSet && st.ambientSet[0])
	{
		gi.configstring (CS_AMBSET, va("%s", st.ambientSet));
	}
	else
	{
		gi.configstring (CS_AMBSET, "sound/sound.ams");
	}

	if(st.startmusic)
	{
		level.baseSong = st.startmusic;
	}
	else
	{
		level.baseSong = 0;
	}

	if(st.script)
	{
/*		edict_t	*runner;

		runner = 
		runner->classname = "script_runner";

		ED_CallSpawn(runner);

		runner->use(runner, runner, runner);
*/
	
		WorldSpawnScriptRunner = G_Spawn();
		WorldSpawnScriptRunner->classname = "script_runner";
		ED_CallSpawn(WorldSpawnScriptRunner);	
	}

	//---------------


	// help icon for statusbar
//	level.pic_health = gi.imageindex ("pics/status/i_health");
	level.alertedStatus = 0;

	//sniper scope images
	gi.imageindex ("pics/scope/arrows128_1");
	gi.imageindex ("pics/scope/arrows128_2");
	gi.imageindex ("pics/scope/arrows128_3");
	gi.imageindex ("pics/scope/arrows128_4");


	if (!st.gravity)
		gi.cvar_set("sv_gravity", "800");
	else
		gi.cvar_set("sv_gravity", st.gravity);

//
// Setup light animation tables. 'a' is total darkness, 'z' is doublebright.
//

	// 0 normal
	gi.configstring(CS_LIGHTS+0, "m");
	
	// 1 FLICKER (first variety)
	gi.configstring(CS_LIGHTS+1, "mmnmmommommnonmmonqnmmo");
	
	// 2 SLOW STRONG PULSE
	gi.configstring(CS_LIGHTS+2, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcb");
	
	// 3 CANDLE (first variety)
	gi.configstring(CS_LIGHTS+3, "mmmmmaaaaammmmmaaaaaabcdefgabcdefg");
	
	// 4 FAST STROBE
	gi.configstring(CS_LIGHTS+4, "mamamamamama");
	
	// 5 GENTLE PULSE 1
	gi.configstring(CS_LIGHTS+5,"jklmnopqrstuvwxyzyxwvutsrqponmlkj");
	
	// 6 FLICKER (second variety)
	gi.configstring(CS_LIGHTS+6, "nmonqnmomnmomomno");
	
	// 7 CANDLE (second variety)
	gi.configstring(CS_LIGHTS+7, "mmmaaaabcdefgmmmmaaaammmaamm");
	
	// 8 CANDLE (third variety)
	gi.configstring(CS_LIGHTS+8, "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa");
	
	// 9 SLOW STROBE (fourth variety)
	gi.configstring(CS_LIGHTS+9, "aaaaaaaazzzzzzzz");
	
	// 10 FLUORESCENT FLICKER
	gi.configstring(CS_LIGHTS+10, "mmamammmmammamamaaamammma");

	// 11 SLOW PULSE NOT FADE TO BLACK
	gi.configstring(CS_LIGHTS+11, "abcdefghijklmnopqrrqponmlkjihgfedcba");

	// 12 FAST PULSE FOR JEREMY
	gi.configstring(CS_LIGHTS+12, "mkigegik");

	// 13 Special sky lightning on client - don't muck with this, por favor...
	gi.configstring(CS_LIGHTS+CL_WORLDSKYLIGHT, "ex");


	// styles 32-62 are assigned by the light program for switchable lights

	// 63 testing
	gi.configstring(CS_LIGHTS+63, "a");

	gi.cvar_set("timescale", "1.0");//in case a script was interrupted...

	// precache stuff - this should probably be handled better...
	PrecacheSurfaceTypes();

	PrecacheSpawningEnemies(ent);
}

void SetSkyColor(float red, float blue, float green)
{
	gi.configstring (CS_SKYCOLOR, va("%f %f %f", red, green, blue) );
}

void InitEntMetalSoundCaching(void)
{
	int i = 0;	

	entSoundsToCache[CLSFX_LAND_METAL] = 1;

	for (i = CLSFX_METALSTRIKE1; i <= CLSFX_METALSTRIKE3; i++)
	{
		entSoundsToCache[i] = 1;
	}

	for (i = 0; i < NUM_FOOTSTEPS_PER_GROUP; i++)
	{
		entSoundsToCache[CLSFX_FOOTSTEP_METAL + i] = 1;
	}
}

void InitEntSoundCaching(void)
{
	int i = 0, stepsnd = -1;
	byte List[256], count;

	memset(entSoundsToCache, 0, sizeof(byte)*NUM_CLSFX);

	// world geometry sounds (ricochets, wood splintering, etc)
	count = gi.SurfaceTypeList(List, sizeof(List));

	while(count)
	{
		count--;

		//	references to CLSFX_METALIMPACT have been commented out because we're forcing Metal sound Precaching anyway :/
		switch(List[count])
		{
		case SURF_DEFAULT:
			//CLSFX_FOOTSTEP_STONE,	CLSFX_STONEIMPACT,	CLSFX_LAND_STONE,
			stepsnd = CLSFX_FOOTSTEP_STONE;
			entSoundsToCache[CLSFX_STONEIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_STONE] = 1;
			break;
		case SURF_METAL:
			//CLSFX_FOOTSTEP_METAL,	CLSFX_METALIMPACT,	CLSFX_LAND_METAL,
			stepsnd = CLSFX_FOOTSTEP_METAL;
//			entSoundsToCache[CLSFX_METALIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_METAL] = 1;
			break;
		case SURF_SAND_YELLOW:
			//CLSFX_FOOTSTEP_SAND,		CLSFX_SANDIMPACT,	CLSFX_LAND_SAND,
			stepsnd = CLSFX_FOOTSTEP_SAND;
			entSoundsToCache[CLSFX_SANDIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_SAND] = 1;
			break;
		case SURF_SAND_WHITE:
			//CLSFX_FOOTSTEP_SAND,		CLSFX_SANDIMPACT,	CLSFX_LAND_SAND,
			stepsnd = CLSFX_FOOTSTEP_SAND;
			entSoundsToCache[CLSFX_SANDIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_SAND] = 1;
			break;
		case SURF_SAND_LBROWN:
			//CLSFX_FOOTSTEP_SAND,		CLSFX_SANDIMPACT,	CLSFX_LAND_SAND,
			stepsnd = CLSFX_FOOTSTEP_SAND;
			entSoundsToCache[CLSFX_SANDIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_SAND] = 1;
			break;
		case SURF_SAND_DBROWN:
			//CLSFX_FOOTSTEP_SAND,		CLSFX_SANDIMPACT,	CLSFX_LAND_SAND,
			stepsnd = CLSFX_FOOTSTEP_SAND;
			entSoundsToCache[CLSFX_SANDIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_SAND] = 1;
			break;
		case SURF_GRAVEL_GREY:
			//CLSFX_FOOTSTEP_GRAVEL,	CLSFX_GRAVELIMPACT,	CLSFX_LAND_GRAVEL
			stepsnd = CLSFX_FOOTSTEP_GRAVEL;
			entSoundsToCache[CLSFX_GRAVELIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_GRAVEL] = 1;
			break;
		case SURF_GRAVEL_DBROWN:
			//CLSFX_FOOTSTEP_GRAVEL,	CLSFX_GRAVELIMPACT,	CLSFX_LAND_GRAVEL
			stepsnd = CLSFX_FOOTSTEP_GRAVEL;
			entSoundsToCache[CLSFX_GRAVELIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_GRAVEL] = 1;
			break;
		case SURF_GRAVEL_LBROWN:
			//CLSFX_FOOTSTEP_GRAVEL,	CLSFX_GRAVELIMPACT,	CLSFX_LAND_GRAVEL
			stepsnd = CLSFX_FOOTSTEP_GRAVEL;
			entSoundsToCache[CLSFX_GRAVELIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_GRAVEL] = 1;
			break;
		case SURF_SNOW:
			//CLSFX_FOOTSTEP_SNOW,		CLSFX_SNOWIMPACT,	CLSFX_LAND_SNOW,
			stepsnd = CLSFX_FOOTSTEP_SNOW;
			entSoundsToCache[CLSFX_SNOWIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_SNOW] = 1;
			break;
		case SURF_LIQUID_BLUE:
			//CLSFX_FOOTSTEP_WATER,	CLSFX_WATERIMPACT,	CLSFX_LAND_WATER,
			stepsnd = CLSFX_FOOTSTEP_WATER;
			entSoundsToCache[CLSFX_WATERIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_WATER] = 1;
			break;
		case SURF_LIQUID_GREEN:
			//CLSFX_FOOTSTEP_WATER,	CLSFX_WATERIMPACT,	CLSFX_LAND_WATER,
			stepsnd = CLSFX_FOOTSTEP_WATER;
			entSoundsToCache[CLSFX_WATERIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_WATER] = 1;
			break;
		case SURF_LIQUID_ORANGE:
			//CLSFX_FOOTSTEP_WATER,	CLSFX_WATERIMPACT,	CLSFX_LAND_WATER,
			stepsnd = CLSFX_FOOTSTEP_WATER;
			entSoundsToCache[CLSFX_WATERIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_WATER] = 1;
			break;
		case SURF_LIQUID_BROWN:
			//CLSFX_FOOTSTEP_WATER,	CLSFX_WATERIMPACT,	CLSFX_LAND_WATER,
			stepsnd = CLSFX_FOOTSTEP_WATER;
			entSoundsToCache[CLSFX_WATERIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_WATER] = 1;
			break;
		case SURF_WOOD_LBROWN:
			//CLSFX_FOOTSTEP_WOOD,		CLSFX_WOODIMPACT,	CLSFX_LAND_WOOD,
			stepsnd = CLSFX_FOOTSTEP_WOOD;
			entSoundsToCache[CLSFX_WOODIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_WOOD] = 1;
			break;
		case SURF_WOOD_DBROWN:
			//CLSFX_FOOTSTEP_WOOD,		CLSFX_WOODIMPACT,	CLSFX_LAND_WOOD,
			stepsnd = CLSFX_FOOTSTEP_WOOD;
			entSoundsToCache[CLSFX_WOODIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_WOOD] = 1;
			break;
		case SURF_WOOD_LGREY:
			//CLSFX_FOOTSTEP_WOOD,		CLSFX_WOODIMPACT,	CLSFX_LAND_WOOD,
			stepsnd = CLSFX_FOOTSTEP_WOOD;
			entSoundsToCache[CLSFX_WOODIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_WOOD] = 1;
			break;
		case SURF_STONE_LGREY:
			//CLSFX_FOOTSTEP_STONE,	CLSFX_STONEIMPACT,	CLSFX_LAND_STONE,
			stepsnd = CLSFX_FOOTSTEP_STONE;
			entSoundsToCache[CLSFX_STONEIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_STONE] = 1;
			break;
		case SURF_STONE_DGREY:
			//CLSFX_FOOTSTEP_STONE,	CLSFX_STONEIMPACT,	CLSFX_LAND_STONE,
			stepsnd = CLSFX_FOOTSTEP_STONE;
			entSoundsToCache[CLSFX_STONEIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_STONE] = 1;
			break;
		case SURF_STONE_LBROWN:
			//CLSFX_FOOTSTEP_STONE,	CLSFX_STONEIMPACT,	CLSFX_LAND_STONE,
			stepsnd = CLSFX_FOOTSTEP_STONE;
			entSoundsToCache[CLSFX_STONEIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_STONE] = 1;
			break;
		case SURF_STONE_DBROWN:
			//CLSFX_FOOTSTEP_STONE,	CLSFX_STONEIMPACT,	CLSFX_LAND_STONE,
			stepsnd = CLSFX_FOOTSTEP_STONE;
			entSoundsToCache[CLSFX_STONEIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_STONE] = 1;
			break;
		case SURF_STONE_WHITE:
			//CLSFX_FOOTSTEP_STONE,	CLSFX_STONEIMPACT,	CLSFX_LAND_STONE,
			stepsnd = CLSFX_FOOTSTEP_STONE;
			entSoundsToCache[CLSFX_STONEIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_STONE] = 1;
			break;
		case SURF_STONE_GREEN:
			//CLSFX_FOOTSTEP_STONE,	CLSFX_STONEIMPACT,	CLSFX_LAND_STONE,
			stepsnd = CLSFX_FOOTSTEP_STONE;
			entSoundsToCache[CLSFX_STONEIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_STONE] = 1;
			break;
		case SURF_STONE_RED:
			//CLSFX_FOOTSTEP_STONE,	CLSFX_STONEIMPACT,	CLSFX_LAND_STONE,
			stepsnd = CLSFX_FOOTSTEP_STONE;
			entSoundsToCache[CLSFX_STONEIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_STONE] = 1;
			break;
		case SURF_STONE_BLACK:
			//CLSFX_FOOTSTEP_STONE,	CLSFX_STONEIMPACT,	CLSFX_LAND_STONE,
			stepsnd = CLSFX_FOOTSTEP_STONE;
			entSoundsToCache[CLSFX_STONEIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_STONE] = 1;
			break;
		case SURF_GRASS_GREEN:
			//CLSFX_FOOTSTEP_GRASS,	CLSFX_GRASSIMPACT,	CLSFX_LAND_GRASS,
			stepsnd = CLSFX_FOOTSTEP_GRASS;
			entSoundsToCache[CLSFX_GRASSIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_GRASS] = 1;
			break;
		case SURF_GRASS_BROWN:
			//CLSFX_FOOTSTEP_GRASS,	CLSFX_GRASSIMPACT,	CLSFX_LAND_GRASS,
			stepsnd = CLSFX_FOOTSTEP_GRASS;
			entSoundsToCache[CLSFX_GRASSIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_GRASS] = 1;
			break;
		case SURF_LIQUID_RED:
			//CLSFX_FOOTSTEP_WATER,	CLSFX_WATERIMPACT,	CLSFX_LAND_WATER,
			stepsnd = CLSFX_FOOTSTEP_WATER;
			entSoundsToCache[CLSFX_WATERIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_WATER] = 1;
			break;
		case SURF_METAL_STEAM:
			//CLSFX_FOOTSTEP_METAL,	CLSFX_METALIMPACT,	CLSFX_LAND_METAL,
			stepsnd = CLSFX_FOOTSTEP_METAL;
//			entSoundsToCache[CLSFX_METALIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_METAL] = 1;
			break;
		case SURF_METAL_WATERJET:
			//CLSFX_FOOTSTEP_METAL,	CLSFX_METALIMPACT,	CLSFX_LAND_METAL,
			stepsnd = CLSFX_FOOTSTEP_METAL;
//			entSoundsToCache[CLSFX_METALIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_METAL] = 1;
			break;
		case SURF_METAL_OILSPURT:
			//CLSFX_FOOTSTEP_METAL,	CLSFX_METALIMPACT,	CLSFX_LAND_METAL,
			stepsnd = CLSFX_FOOTSTEP_METAL;
//			entSoundsToCache[CLSFX_METALIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_METAL] = 1;
			break;
		case SURF_METAL_CHEMSPURT:
			//CLSFX_FOOTSTEP_METAL,	CLSFX_METALIMPACT,	CLSFX_LAND_METAL,
			stepsnd = CLSFX_FOOTSTEP_METAL;
//			entSoundsToCache[CLSFX_METALIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_METAL] = 1;
			break;
		case SURF_METAL_COMPUTERS:
			//CLSFX_FOOTSTEP_METAL,	CLSFX_METALIMPACT,	CLSFX_LAND_METAL,
			//CLSFX_SPARK1, CLSFX_EXP_SMALL
			stepsnd = CLSFX_FOOTSTEP_METAL;
//			entSoundsToCache[CLSFX_METALIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_METAL] = 1;
			// if damaged, the surface requires certain sounds...
			entSoundsToCache[CLSFX_SPARK1] = 1;
			entSoundsToCache[CLSFX_SPARK2] = 1;
			entSoundsToCache[CLSFX_EXP_SMALL] = 1;
			break;
		case SURF_SNOW_LBROWN:
			//CLSFX_FOOTSTEP_STONE,	CLSFX_STONEIMPACT,	CLSFX_LAND_STONE,
			stepsnd = CLSFX_FOOTSTEP_STONE;
			entSoundsToCache[CLSFX_STONEIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_STONE] = 1;
			break;
		case SURF_SNOW_GREY:
			//CLSFX_FOOTSTEP_STONE,	CLSFX_STONEIMPACT,	CLSFX_LAND_STONE,
			stepsnd = CLSFX_FOOTSTEP_STONE;
			entSoundsToCache[CLSFX_STONEIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_STONE] = 1;
			break;
		case SURF_BLOOD:
			//CLSFX_STONEIMPACT,		CLSFX_STONEIMPACT,	CLSFX_LAND_STONE,
			stepsnd = CLSFX_FOOTSTEP_STONE;
			entSoundsToCache[CLSFX_STONEIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_STONE] = 1;
			break;
		case SURF_LIQUID_BLACK:
			//CLSFX_FOOTSTEP_WATER,	CLSFX_WATERIMPACT,	CLSFX_LAND_WATER,
			stepsnd = CLSFX_FOOTSTEP_WATER;
			entSoundsToCache[CLSFX_WATERIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_WATER] = 1;
			break;
		case SURF_GLASS:
			//CLSFX_FOOTSTEP_METAL,	CLSFX_METALIMPACT,	CLSFX_LAND_METAL,
			//CLSFX_BREAK_GLASSSMALL
			stepsnd = CLSFX_FOOTSTEP_METAL;
//			entSoundsToCache[CLSFX_METALIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_METAL] = 1;
			// if damaged, the surface requires a certain sound...
			entSoundsToCache[CLSFX_BREAK_GLASSSMALL] = 1;
			break;
		case SURF_GLASS_COMPUTER:
			//CLSFX_FOOTSTEP_METAL,	CLSFX_METALIMPACT,	CLSFX_LAND_METAL,
			//CLSFX_BREAK_GLASSSMALL
			stepsnd = CLSFX_FOOTSTEP_METAL;
//			entSoundsToCache[CLSFX_METALIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_METAL] = 1;
			// if damaged, the surface requires a certain sound...
			entSoundsToCache[CLSFX_BREAK_GLASSSMALL] = 1;
			break;
		case SURF_SODAMACHINE:
			//CLSFX_FOOTSTEP_METAL,	CLSFX_METALIMPACT,	CLSFX_LAND_METAL,
			stepsnd = CLSFX_FOOTSTEP_METAL;
//			entSoundsToCache[CLSFX_METALIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_METAL] = 1;
			break;
		case SURF_PAPERWALL:
			//CLSFX_FOOTSTEP_WOOD,		CLSFX_WOODIMPACT,	CLSFX_LAND_WOOD,
			stepsnd = CLSFX_FOOTSTEP_WOOD;
			entSoundsToCache[CLSFX_WOODIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_WOOD] = 1;
			break;
		case SURF_NEWSPAPER_DAMAGE:
			//CLSFX_FOOTSTEP_METAL,	CLSFX_METALIMPACT,	CLSFX_LAND_METAL,
			stepsnd = CLSFX_FOOTSTEP_METAL;
//			entSoundsToCache[CLSFX_METALIMPACT] = 1;
			entSoundsToCache[CLSFX_LAND_METAL] = 1;
			break;
		}

		if (stepsnd >= 0)
		{
			for (i = 0; i < NUM_FOOTSTEPS_PER_GROUP; i++)
			{
				entSoundsToCache[stepsnd + i] = 1;
			}
		}
		stepsnd = -1;
	}

	entSoundsToCache[CLSFX_SPARK1] = 1;
	entSoundsToCache[CLSFX_SPARK2] = 1;

}

void InitEntDebrisCaching(void)
{
	int i = 0;

	memset(entDebrisToCache, 'n', sizeof(char)*NUM_CLGHL);

	// there may be some debris models that we'll always want to cache, but I
	//can't think of 'em right now

}

void CacheEntDebrisAndSounds(edict_t *ent)
{
	// cache the debris that this ent turns into when it breaks/dies/explodes
	switch(ent->material)
	{
	case MAT_BRICK_BROWN:
	case MAT_BRICK_LBROWN:
	case MAT_BRICK_LGREY:
	case MAT_BRICK_DGREY:
	case MAT_BRICK_RED:
		entDebrisToCache[CLGHL_CHUNKBRICK] = DEBRIS_YES;
		break;

	case MAT_GLASS:
		entDebrisToCache[CLGHL_CHUNKGLASSSM] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKGLASSMED] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKGLASSLRG] = DEBRIS_YES;
		break;

	case MAT_METAL_LGREY:
	case MAT_METAL_DGREY:
	case MAT_METAL_RUSTY:
	case MAT_METAL_SHINY:
		entDebrisToCache[CLGHL_CHUNKMETALSM] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKMETALMED] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKMETALLRG] = DEBRIS_YES;
		break;

	case MAT_ROCK_BROWN:
	case MAT_ROCK_LBROWN:
	case MAT_ROCK_DBROWN:
	case MAT_ROCK_LGREY:
	case MAT_ROCK_DGREY:
	case MAT_ROCK_FLESH:
		entDebrisToCache[CLGHL_CHUNKROCKSM] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKROCKMED] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKROCKLRG] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKROCKTINY] = DEBRIS_YES;
		break;

	case MAT_WOOD_LBROWN:
	case MAT_WOOD_DBROWN:
	case MAT_WOOD_DGREY:
	case MAT_WOOD_LGREY:
		entDebrisToCache[CLGHL_CHUNKWOODSM] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKWOODMED] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKWOODLRG] = DEBRIS_YES;
		break;

	case MAT_WALL_BLACK:
	case MAT_WALL_BROWN:
	case MAT_WALL_DARKBROWN:
	case MAT_WALL_LIGHTBROWN:
	case MAT_WALL_GREY:
	case MAT_WALL_DARKGREY:
	case MAT_WALL_LIGHTGREY:
	case MAT_WALL_GREEN:
	case MAT_WALL_ORANGE:
	case MAT_WALL_RED:
	case MAT_WALL_WHITE:
	case MAT_WALL_STRAW:
		entDebrisToCache[CLGHL_CHUNKWALLSM] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKWALLMED] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKWALLLRG] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKWALLTINY] = DEBRIS_YES;
		break;
	case MAT_ROCK_SNOW:
		entDebrisToCache[CLGHL_CHUNKROCKSM] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKROCKMED] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKROCKLRG] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKROCKTINY] = DEBRIS_YES;
		break;
	}

// kef -- have to keep some of the precaching stuff out for now cuz it crashes final/nyc with
// an index overflow in the soundlist
#if 1
	// some surfaceTypes have various client fx associated with their death. these
	//fx tend to generate a lot of chunks. also, the sounds needed by the client function
	//HandleDebris() are precached here.
	switch(ent->surfaceType)
	{
	case SURF_DEFAULT:
		// MakeDefaultWallEffect -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_WOOD_LBROWN:
		// MakeDefaultWallEffect -- no chunks 
		entSoundsToCache[CLSFX_BREAK_WOOD] = 1;
		break;
	case SURF_WOOD_DBROWN:
		// MakeDefaultWallEffect -- no chunks
		entSoundsToCache[CLSFX_BREAK_WOOD] = 1;
		break;
	case SURF_WOOD_LGREY:
		// MakeDefaultWallEffect -- no chunks
		entSoundsToCache[CLSFX_BREAK_WOOD] = 1;
		break;
	case SURF_STONE_LGREY:
		// MakeDefaultWallEffect -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_STONE_DGREY:
		// MakeDefaultWallEffect -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_STONE_LBROWN:
		// MakeDefaultWallEffect -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_STONE_DBROWN:
		// MakeDefaultWallEffect -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_STONE_WHITE:
		// MakeDefaultWallEffect -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_STONE_GREEN:
		// MakeDefaultWallEffect -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_STONE_RED:
		// MakeDefaultWallEffect -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_STONE_BLACK:
		// MakeDefaultWallEffect -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_GRASS_GREEN:
		// MakeDefaultWallEffect -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_GRASS_BROWN:
		// MakeDefaultWallEffect -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_METAL:
		// MakeSparks -- no chunks 
		entSoundsToCache[CLSFX_BREAK_METAL] = 1;
		break;
	case SURF_SAND_YELLOW:
		// MakeSplatter -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_SAND_WHITE:
		// MakeSplatter -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_SAND_LBROWN:
		// MakeSplatter -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_SAND_DBROWN:
		// MakeSplatter -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_GRAVEL_GREY:
		// MakeSplatter -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_GRAVEL_DBROWN:
		// MakeSplatter -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_GRAVEL_LBROWN:
		// MakeSplatter -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_SNOW:
		// MakeSplatter -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_SNOW_LBROWN:
		// MakeSplatter -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_SNOW_GREY:
		// MakeSplatter -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_LIQUID_BLUE:
		// MakeSplatWater -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_LIQUID_GREEN:
		// MakeSplatWater -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_LIQUID_ORANGE:
		// MakeSplatWater -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_LIQUID_BROWN:
		// MakeSplatWater -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_LIQUID_RED:
		// MakeSplatWater -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_LIQUID_BLACK:
		// MakeSplatWater -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_METAL_STEAM:
		// MakeShortSteam -- no chunks
		entSoundsToCache[CLSFX_BREAK_METAL] = 1;
		break;
	case SURF_METAL_WATERJET:
		// MakeLiquidSpurt -- no chunks
		entSoundsToCache[CLSFX_BREAK_METAL] = 1;
		break;
	case SURF_METAL_OILSPURT:
		// MakeLiquidSpurt -- no chunks
		entSoundsToCache[CLSFX_BREAK_METAL] = 1;
		break;
	case SURF_METAL_CHEMSPURT:
		// MakeLiquidSpurt -- no chunks
		entSoundsToCache[CLSFX_BREAK_METAL] = 1;
		break;
	case SURF_METAL_COMPUTERS:
		// MakeComputerDamage -- calls ThrowChunks with surfaceType SURF_STONE_DGREY
		//CLSFX_SPARK1, CLSFX_EXP_SMALL
		entDebrisToCache[CLGHL_CHUNKWALLSM] = DEBRIS_YES;
		entSoundsToCache[CLSFX_BREAK_METAL] = 1;
		entSoundsToCache[CLSFX_SPARK1] = 1;
		entSoundsToCache[CLSFX_EXP_SMALL] = 1;
		break;
	case SURF_BLOOD:
		// MakeBloodSplatter -- no chunks
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	case SURF_GLASS:
		// MakeSplatGlass -- throws some glass chunks
		//CLSFX_BREAK_GLASSSMALL,CLSFX_BREAK_GLASSMED,CLSFX_BREAK_GLASSBIG
		entDebrisToCache[CLGHL_CHUNKGLASSSM] = DEBRIS_YES;
		entSoundsToCache[CLSFX_BREAK_GLASSSMALL] = 1;
		entSoundsToCache[CLSFX_BREAK_GLASSMED] = 1;
		entSoundsToCache[CLSFX_BREAK_GLASSBIG] = 1;
		break;
	case SURF_GLASS_COMPUTER:
		// MakeComputerGlassDamage
		//CLSFX_BREAK_GLASSSMALL,CLSFX_BREAK_GLASSMED,CLSFX_BREAK_GLASSBIG
		entDebrisToCache[CLGHL_CHUNKGLASSSM] = DEBRIS_YES;
		entDebrisToCache[CLGHL_CHUNKWALLSM] = DEBRIS_YES;
		entSoundsToCache[CLSFX_BREAK_GLASSSMALL] = 1;
		entSoundsToCache[CLSFX_BREAK_GLASSMED] = 1;
		entSoundsToCache[CLSFX_BREAK_GLASSBIG] = 1;
		break;
	case SURF_SODAMACHINE:
		// MakeSodaDamage -- you guessed it...spews cans
		entDebrisToCache[CLGHL_CAN] = DEBRIS_YES;
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;

	case SURF_PAPERWALL:
		// MakePaperWallDamage
		//CLSFX_WOODIMPACT
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;

	case SURF_NEWSPAPER_DAMAGE:
		// MakeNewspaperDamage 
		entDebrisToCache[CLGHL_PAPER] = DEBRIS_YES;
		entSoundsToCache[CLSFX_BREAK_STONE] = 1;
		break;
	}
#endif
}

void CacheAllDebris(void)
{

	if (NUM_CLGHL > MAX_QPATH)	// Too many different types of debris
	{
		Com_Printf("ERROR: NUM_CLGHL > MAX_QPATH!\n");
		return;
	}

	gi.configstring(CS_DEBRISPRECACHE,entDebrisToCache);	// Set up string to send across 
}

void CacheAllSounds(void)
{
	int i = 0;

	// never know when we'll have water available, so make sure the 'I entered water' sound
	//is always precached
	entSoundsToCache[CLSFX_LAND_WATER] = 1;
	for (i = 0; i < NUM_CLSFX; i++)
	{
		if (entSoundsToCache[i])
		{
			gi.soundindex(cl_fxs[i].filename);
		}
	}
}

// this is less than cool
void CacheAllWallEffects(void)
{
	gi.effectindex("environ/waterspurt");
	gi.effectindex("environ/oilspurt");
	gi.effectindex("environ/chemspurt");
	gi.effectindex("environ/metal_computer");
	gi.effectindex("environ/glass_computer");
	gi.effectindex("environ/metal_steam");
	gi.effectindex("environ/bulletsplash");
	gi.effectindex("environ/splatgreen");
	gi.effectindex("environ/splatorange");
	gi.effectindex("environ/splatred");
	gi.effectindex("environ/splatbrown");
	gi.effectindex("environ/splatblack");
}

char *weaponList[] =
{	"nothing",
	"knife",
	"pistol2",
	"pistol1",
	"mpistol",
	"assault",
	"sniper",
	"slugger",
	"shotgun",
	"machinegun",
	"rocket",
	"mpg",
	"flamegun",
	0
};

void CalculatePlayerEntryWeapons(int *weaponsAvailable)
{
	char *curSpot;
	char *curTokSpot;
	char token[256];
	char temp[256];
	int  value;

	//assume curSpot is somewhere decent

	curSpot = gi.cvar_info(CVAR_WEAPON | CVAR_ITEM | CVAR_AMMO);

	if(!curSpot || (!strcmp(curSpot, "")))
	{	// if there is nothing, plug in the defaults
		return;
	}

	while(*curSpot)
	{
		curTokSpot = token;

		if(*curSpot == '\\')
		{
			curSpot++;
		}

		while(*curSpot && (*curSpot != '\\'))
		{
			*curTokSpot++ = *curSpot++;
		}
		*curTokSpot = 0;

		assert(*curSpot);

		if(*curSpot == '\\')
		{
			curSpot++;
		}

		curTokSpot = temp;
		while((*curSpot) && (*curSpot != '\\'))
		{
			*curTokSpot++ = *curSpot++;
		}
		*curTokSpot = 0;
		value = atoi(temp);

		// okay, we have the two tokens... let's do stuff with them now...


		char *testString;
		int curCheck;
		int found = 0;



		// look for weapons

		curCheck = 0;

		do
		{
			testString = weaponList[curCheck];

			if(!stricmp(token, testString))
			{	// hey, I have one of these!  Cool!
				if(value)
				{// add the weapon
					*weaponsAvailable |= (1<<curCheck);
				}
				found = 1;
				break;
			}

			curCheck++;
		}while(weaponList[curCheck]);

		if(found)
		{
			continue;
		}
	}

	return;
}
