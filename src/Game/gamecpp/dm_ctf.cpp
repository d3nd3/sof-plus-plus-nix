// *****************************************************************************
// dm_ctf.cpp
// *****************************************************************************

#include "g_local.h"
#include "dm_private.h"
#include "w_weapons.h"
#include "dm_ctf.h"
#include "p_body.h"
#include "../strings/dm_generic.h"
#include "..\qcommon\configstring.h"

const int DF_CTF_NO_HEALTH			= (1<<0);
const int DF_CTF_NO_ITEMS			= (1<<1);
const int DF_CTF_WEAPONS_STAY		= (1<<2);
const int DF_CTF_NO_FALLING			= (1<<3);
const int DF_CTF_REALISTIC_DAMAGE	= (1<<4);
const int DF_CTF_SAME_LEVEL			= (1<<5);
const int DF_CTF_NOWEAPRELOAD		= (1<<6);
const int DF_CTF_NO_FRIENDLY_FIRE	= (1<<8);
const int DF_CTF_FORCE_RESPAWN		= (1<<9);
const int DF_CTF_NO_ARMOR			= (1<<10);
const int DF_CTF_INFINITE_AMMO		= (1<<11);
const int DF_CTF_SPINNINGPICKUPS	= (1<<12);
const int DF_CTF_BULLET_WPNS_ONLY	= (1<<13);
const int DF_CTF_FORCEJOIN			= (1<<14);

void hurt_touch(edict_t *self, edict_t *other, cplane_t *plane, struct mtexinfo_s *surf);
int TeamCompare(void const *a, void const *b);

/*
==================
dmctf_c::FlagCaptured
==================
*/

void dmctf_c::FlagCaptured(team_t team)
{
	if (team == TEAM1)
		blue_collected++;
	else
		red_collected++;
}

/*
==================
dmctf_c::ClearFlagCount
==================
*/

void dmctf_c::ClearFlagCount(void)
{
	blue_collected = red_collected = 0;
	// clear out anyone with a flag next to them on the score board.
	gi.configstring(CS_CTF_BLUE_STAT, "d");
	gi.configstring(CS_CTF_RED_STAT, "d");
	// Reset the ctf_loops_count.
	gi.cvar_setvalue("ctf_loops_count",  0.0);
	gi.cvar_setvalue("ctf_flag_captured",  0.0);
}

/*
==================
dmctf_c::HitByAttacker
==================
*/
void dmctf_c::HitByAttacker(edict_t *body, edict_t *attacker)
{
	// if the attacker isn't a player, exit
	if (!attacker->client)
		return;

	// are we a flag carrier? If so, set the time inside of my attacker, so if he is killed soon, the player that kills HIM gets a bonus for protecting me
	if (body->ctf_flags)
	{
		attacker->ctf_hurt_carrier = level.time;
	}
}

/*
==================
CountSkinsInFile
 
given a pointer to a GPL file, count the number of skins for any given team name
==================
*/
int CountSkinsInFile(char *teamname, char *buffer)
{
	// Loop through and validate skin names - see if they are attached to this team we are checking against
	TeamInfoC  		team;
	char	*s;
	int	count = 0;

	s=buffer;

	while(s)
	{
		char  shortname[100];
		char  defModName[100];

		strcpy(shortname,COM_Parse(&s));

		if(strlen(shortname))
		{
			// we have a skin name, go load it in and see what team its on
			Com_sprintf(defModName, sizeof(defModName), "%s.gpm", shortname);
			IPlayerModelInfoC *aPlayer=gi.NewPlayerModelInfo(defModName);
			// if we are on the team name provided, inc the counter
			aPlayer->GetTeamInfo(team);
			if (!(stricmp(teamname, team.name)))
			{
				count++;
			}
			delete aPlayer;
		}
	}
	return count;
}

/*
==================
dmctf_c::NumOfSkins
==================
*/
int dmctf_c::NumOfSkins(char *teamname)
{
	int		i,length ;
	char	*buffer;
	char	custom_filename[MAX_QPATH];

	// load in the ctf GPL file, that contains all the skins allowed for CTF
	if((length=gi.FS_LoadFile("ghoul/pmodels/ctf.gpl",(void **)&buffer))==-1)
	{
		gi.error("*************************\nCould not open ctf.gpl\n*************************\n");
		return 0;
	}

	i = CountSkinsInFile(teamname, buffer);

	// Clean up...

	gi.FS_FreeFile(buffer);

	// if we have a secondary GPL file...
	if (sv_altpmodlistfile->string[0])
	{
		Com_sprintf(custom_filename, sizeof(custom_filename),"%s.gpl", sv_altpmodlistfile->string);
		// now look through the secondary skin file
		if((length=gi.FS_LoadFile(custom_filename,(void **)&buffer))==-1)
		{
			return i;
		}

		i += CountSkinsInFile(teamname, buffer);

		// Clean up...

		gi.FS_FreeFile(buffer);
	}

	return i;
}

/*
==================
dmctf_c::levelInit
==================
*/

#define MAX_DEFINED_TEAMS 6
void dmctf_c::levelInit(void)
{
	// Precache string package.
	char	*teamnames[MAX_DEFINED_TEAMS] =
	{
	"The Order",
	"MeatWagon",
	"Ministry of Sin",
	"Red Guard",
	"The December Brigade",
	"The Order"
	};

	gi.SP_Register("dm_ctf");

	// Precache flags.

	game_ghoul.FindObject("items/ctf_flag", "flag_hold_idle");
	game_ghoul.FindObject("items/ctf_flag", "flag_run");
	
	// Precache sounds.

	gi.soundindex("dm/ctf/flaggrab.wav");
	gi.soundindex("dm/ctf/flagret.wav");
	gi.soundindex("dm/ctf/flagbase.wav");
	gi.soundindex("dm/ctf/triumphant.wav");
	gi.soundindex("player/flaglp.wav");

	// Make certain all the weapons given to clients at respawn are precached.

	// View weapons.

	AddWeaponTypeForPrecache(SFW_KNIFE);
	AddWeaponTypeForPrecache(SFW_PISTOL1);
	AddWeaponTypeForPrecache(SFW_HURTHAND);
	AddWeaponTypeForPrecache(SFW_THROWHAND);

	// Bolt-ons / pickups. Eugh... but necessary to precache. Knife doesn't
	// need to be precached for some reason - it's always there.. hmmm.

	edict_t *ent=G_Spawn();
	
	I_Spawn(ent,thePickupList.GetPickupFromType(PU_WEAPON,SFW_PISTOL1));
	
	G_FreeEdict(ent);

	// are the two teams the same ?
	if (!stricmp(ctf_team_blue->string, ctf_team_red->string))
	{
		// look for which one the red team is
		for (int i=0; i<MAX_DEFINED_TEAMS; i++)
		{
			// if we are the same, then choose the next team
			if (!stricmp(ctf_team_red->string, teamnames[i]))
			{
				// this should never happen since the first and last teamname are the same entry, but you can never be too careful
				if (i+1 != MAX_DEFINED_TEAMS)
				{
					gi.cvar_set("ctf_team_red", teamnames[i+1]);
					Com_Printf("CTF Teams are the same - Resetting Team Red to %s\n", ctf_team_red->string);
					break;
				}
			}
		}
		// we didn't find it in the list - all we can do here is pick a random team and hope it's not the same as blue
		if (i == MAX_DEFINED_TEAMS)
		{
		  	gi.cvar_set("ctf_team_red", teamnames[gi.irand(0,MAX_DEFINED_TEAMS-1)]);
			Com_Printf("CTF Teams are the same - Resetting Team Red to %s\n", ctf_team_red->string);
		}
	}

	// Set the number of blue or red skin.gpm files there are. We have to do this
	// here, since we don't want to keep doing this everytime someone joins the
	// game, yet we require the num of blue and red skin files to randomise people
	// later.
	num_blue_skins = NumOfSkins(ctf_team_blue->string); 
	num_red_skins = NumOfSkins(ctf_team_red->string); 

	// Set the right team names for the client to be able to set the colors right
	// on the team icons.
	gi.configstring(CS_CTF_BLUE_TEAM, ctf_team_blue->string);
	gi.configstring(CS_CTF_RED_TEAM, ctf_team_red->string);
}

/*
==================
dmctf_c::checkItemSpawn
==================
*/

int	dmctf_c::checkItemSpawn(edict_t *ent,Pickup **pickup)
{

	if(dmRule_NO_HEALTH())
	{
		if ((*pickup)->GetType() == PU_HEALTH)
		{
			G_FreeEdict (ent);
			return(0);
		}
	}

	if(dmRule_NO_ITEMS())
	{
		if ((*pickup)->GetType() == PU_INV)
		{
			G_FreeEdict (ent);
			return(0);
		}
	}

	if(dmRule_NO_ARMOR())
	{
		if ((*pickup)->GetType() == PU_ARMOR)
		{
			G_FreeEdict (ent);
			return(0);
		}
	}
	
	if(dmRule_INFINITE_AMMO())
	{
		if ((*pickup)->GetType() == PU_AMMO)
		{
			G_FreeEdict (ent);
			return(0);
		}
	}

	if(dmRule_BULLET_WPNS_ONLY())
	{
		if ( ((*pickup)->GetType() == PU_WEAPON) && (!(*pickup)->IsBulletWpn()) ||
			 ((*pickup)->GetType() == PU_AMMO) && (!(*pickup)->IsBulletAmmo())	)
		{
			G_FreeEdict (ent);
			return(0);
		}
	}

	return(-1);
}


/*
==================
dmctf_c::dmRule_xxx
==================
*/

int	dmctf_c::dmRule_NO_HEALTH(void)
{
	return((int)dmflags->value&DF_CTF_NO_HEALTH);
}

int	dmctf_c::dmRule_NO_ITEMS(void)
{
	return((int)dmflags->value&DF_CTF_NO_ITEMS);
}

int	dmctf_c::dmRule_WEAPONS_STAY(void)
{
	return((int)dmflags->value&DF_CTF_WEAPONS_STAY);
}

int	dmctf_c::dmRule_NO_FALLING(void)
{
	return((int)dmflags->value&DF_CTF_NO_FALLING);
}

int	dmctf_c::dmRule_REALISTIC_DAMAGE(void)
{
	return((int)dmflags->value&DF_CTF_REALISTIC_DAMAGE);
}

int	dmctf_c::dmRule_SAME_LEVEL(void)
{
	return((int)dmflags->value&DF_CTF_SAME_LEVEL);
}

int	dmctf_c::dmRule_NOWEAPRELOAD(void)
{
	return((int)dmflags->value&DF_CTF_NOWEAPRELOAD);
}

int	dmctf_c::dmRule_NO_FRIENDLY_FIRE(void)
{
	return((int)dmflags->value&DF_CTF_NO_FRIENDLY_FIRE);
}

int	dmctf_c::dmRule_FORCE_RESPAWN(void)
{
	return((int)dmflags->value&DF_CTF_FORCE_RESPAWN);
}

int	dmctf_c::dmRule_NO_ARMOR(void)
{
	return((int)dmflags->value&DF_CTF_NO_ARMOR);
}

int	dmctf_c::dmRule_INFINITE_AMMO(void)
{
	return((int)dmflags->value&DF_CTF_INFINITE_AMMO);
}

int	dmctf_c::dmRule_SPINNINGPICKUPS(void)
{
	return((int)dmflags->value&DF_CTF_SPINNINGPICKUPS);
}

int	dmctf_c::dmRule_BULLET_WPNS_ONLY(void)
{
	return((int)dmflags->value&DF_CTF_BULLET_WPNS_ONLY);
}

int	dmctf_c::dmRule_FORCEJOIN(void)
{
	return((int)dmflags->value&DF_CTF_FORCEJOIN);
}

/*
==================
CheckFlagFloor
==================
*/
void CheckFlagFloor(edict_t *ent)
{
	vec3_t point;
	int	cont, num, i;
	edict_t		*touch[MAX_EDICTS], *hit;

 	ent->nextthink = level.time + 1.0;

	// is it time to reset this flag?
	if (ent->timestamp < level.time)
	{
		reset_flag(ent, NULL);
	 	gi.sound(ent,CHAN_NO_PHS_ADD,gi.soundindex("dm/ctf/flagret.wav"),1.0,ATTN_NONE,0,SND_LOCALIZE_GLOBAL);
		if (ent->ctf_flags == TEAM1)
		{
			gi.SP_Print(NULL, DM_CTF_RESPAWN_FLAG_BLUE);
		}
		else
		{
			gi.SP_Print(NULL, DM_CTF_RESPAWN_FLAG_RED);
		}
		// remove the current edict entirely, ghoul model and all
		G_FreeEdict(ent);
		return;
	}

	VectorCopy(ent->s.origin, point);
	// bring us up juuuuust a tad
	point[2] += 1.0;
	cont = gi.pointcontents(point);

	VectorAdd(ent->mins, ent->s.origin, ent->absmin);
	VectorAdd(ent->maxs, ent->s.origin, ent->absmax);

	num = gi.BoxEdicts (ent->absmin, ent->absmax, touch
		, MAX_EDICTS, AREA_TRIGGERS);

	for (i=0 ; i<num ; i++)
	{
		hit = touch[i];
		if (!hit->inuse)
			continue;
		if (!hit->touch)
			continue;
		if (hit->touch == hurt_touch)
			cont = CONTENTS_LAVA;
	}

	// if we are sitting over a pain or kill brush, reset us home.
	if (cont & (CONTENTS_LAVA|CONTENTS_SLIME) )
	{
		reset_flag(ent, NULL);
	 	gi.sound(ent,CHAN_NO_PHS_ADD,gi.soundindex("dm/ctf/flagret.wav"),1.0,ATTN_NONE,0,SND_LOCALIZE_GLOBAL);

		if (ent->ctf_flags == TEAM1)
		{
			gi.SP_Print(NULL, DM_CTF_INACCESSIBLE_BLUE);
		}
		else
		{
			gi.SP_Print(NULL, DM_CTF_INACCESSIBLE_RED);
		}
		// remove the current edict entirely, ghoul model and all
		G_FreeEdict(ent);
	}
}

/*
==================
dmctf_c::ClientDropItem
==================
*/
void dmctf_c::clientDropItem(edict_t *ent,int type,int ammoCount)
{
	Pickup	*pickup = NULL;

	if (pickup = thePickupList.GetPickupFromType(PU_INV, type))
	{
		edict_t		*dropped;
		Matrix4		ZoneMatrix;
		Vect3		zonePos;
		vec3_t		handPos,
					dir;

		// Whatever weapon player is holding, throw one off.

		dropped=G_Spawn();
		dropped->spawnflags|=DROPPED_ITEM;

		// before we spawn anything, set the count on the dropped entity so its a CTF flag if need be
		if (pickup->GetPickupListIndex() == OBJ_CTF_FLAG)
		{
			dropped->count = ent->ctf_flags;
			assert(dropped->count);
		}

		I_Spawn(dropped,pickup);

		// by this point - ctf_flags should have been set, so we can clear the count field. Only the flag spawn point has a value in the Count field
		if (pickup->GetPickupListIndex() == OBJ_CTF_FLAG)
		{
			dropped->count = 0;
		}

		dropped->enemy=ent;
		dropped->touch_debounce_time=level.time+2.0;
		dropped->touch_debounce_owner = ent;

		if (ent->ghoulInst)
		{
			ent->ghoulInst->
				GetBoltMatrix(level.time,
							  ZoneMatrix,
							  ent->ghoulInst->GetGhoulObject()->FindPart("wbolt_hand_l"),
							  IGhoulInst::MatrixType::Entity);
			
			ZoneMatrix.GetRow(3,zonePos);

			handPos[0]=zonePos[0];
			handPos[1]=zonePos[1];
			handPos[2]=zonePos[2];

			VectorAdd(ent->s.origin,handPos,dropped->s.origin);
			AngleVectors(ent->client->ps.viewangles,dir,NULL, NULL);
			dir[2] = 0;
			VectorNormalize(dir);
			VectorMA(dropped->s.origin, 20.0, dir, dropped->s.origin);
			VectorScale(dir,250.0,dropped->velocity);
		}
		else
		{

			AngleVectors(ent->client->ps.viewangles,dir,NULL,NULL);
			dir[2] = 0;
			VectorNormalize(dir);
			VectorCopy(ent->s.origin, dropped->s.origin);
			dropped->s.origin[2] += 40.0;
			VectorMA(dropped->s.origin, 20.0, dir, dropped->s.origin);
			VectorScale(dir,250.0,dropped->velocity);
		}
		dropped->velocity[2]+=150.0;
		dropped->health = ammoCount;

		if (pickup->GetPickupListIndex() == OBJ_CTF_FLAG)
		{
			// indicate to all clients the state of the flags
			if (ent->ctf_flags == TEAM1)
			{
				gi.configstring(CS_CTF_BLUE_STAT, "d");
			}
			else
			{
				gi.configstring(CS_CTF_RED_STAT, "d");
			}

			// first, rename the class on the dropped flag so its not the same as the spawned flag entity
			dropped->classname = "ctf dropped flag";
			// give us a think function that watched what we sit on, and re-sets the flag if it lands on something hurtful
			dropped->nextthink = level.time + 1.0;
			dropped->think = CheckFlagFloor;
			dropped->timestamp = level.time + CTF_RESPAWN_TIMEOUT;

			// remove flag from player
			ent->ctf_flags = 0;
			// delete the ghoul object on the players hip so we lose the flag there.
			if (ent->ghoulInst)
				PB_RemoveFlag(ent);
			// if we have the flag klaxon sound going, turn it off
			if (ent->s.sound == gi.soundindex("player/flaglp.wav"))
			{
				ent->s.sound = 0;
			}
		}
		else
		{
			clientSetDroppedItemThink(dropped);
		}
	}
}

/*
==================
dmctf_c::AssignTeam
==================
*/
void	dmctf_c::FindSkin(char *teamname, int skinnum)
{
	int		i,length ;
	char	*buffer,*s;
	TeamInfoC  		team;
	char	custom_filename[MAX_QPATH];

	// load in the ctf GPL file, that contains all the skins allowed for CTF
	if((length=gi.FS_LoadFile("ghoul/pmodels/ctf.gpl",(void **)&buffer))==-1)
	{
		gi.error("*************************\nCould not open ctf.gpl\n*************************\n");
		return ;
	}

	i = 0; 
	s=buffer;

	// Loop through and validate skin names.
	while(s)
	{
		char  defModName[100];

		strcpy(NewSkin,COM_Parse(&s));

		if(strlen(NewSkin))
		{
			// we have a skin name, go load it in and see what team its on
			Com_sprintf(defModName, sizeof(defModName), "%s.gpm", NewSkin);
			IPlayerModelInfoC *aPlayer=gi.NewPlayerModelInfo(defModName);
			// if we are on the team name provided, inc the counter
			aPlayer->GetTeamInfo(team);
			delete aPlayer;
			if (!(stricmp(teamname, team.name)))
			{
				// is this the file we are looking for?
				if (i == skinnum)
				{
					gi.FS_FreeFile(buffer);
					return ;
				}
				i++;
			}
		}
	}
	// Clean up...

	gi.FS_FreeFile(buffer);

	// load in the custom GPL file, that contains all the skins allowed for CTF
	// if we have a secondary GPL file...
	if (sv_altpmodlistfile->string[0])
	{
		Com_sprintf(custom_filename, sizeof(custom_filename),"%s.gpl", sv_altpmodlistfile->string);
		if((length=gi.FS_LoadFile(custom_filename,(void **)&buffer))==-1)
		{
			return ;
		}

		s=buffer;

		// Loop through and validate skin names.
		while(s)
		{
			char  defModName[100];

			strcpy(NewSkin,COM_Parse(&s));

			if(strlen(NewSkin))
			{
				// we have a skin name, go load it in and see what team its on
				Com_sprintf(defModName, sizeof(defModName), "%s.gpm", NewSkin);
				IPlayerModelInfoC *aPlayer=gi.NewPlayerModelInfo(defModName);
				// if we are on the team name provided, inc the counter
				aPlayer->GetTeamInfo(team);
				delete aPlayer;
				if (!(stricmp(teamname, team.name)))
				{
					// is this the file we are looking for?
					if (i == skinnum)
					{
						gi.FS_FreeFile(buffer);
						return ;
					}
					i++;
				}
			}
		}
		// Clean up...

		gi.FS_FreeFile(buffer);
	}
	gi.error("FORCEJOIN: Looking for skin that doesn't exit. - teamname %s - skin number %d\n", teamname, skinnum );
}


/*
==================
dmctf_c::AssignTeam
==================
*/
team_t dmctf_c::CompareSkin(char *skinname, char* teamname) 
{
	int		length ;
	char	*buffer,*s;
	TeamInfoC  		team;
	char	custom_filename[MAX_QPATH];
	
	// if we haven't even selected the right team, dump out immediately
	if ((stricmp(ctf_team_blue->string, teamname)) && (stricmp(ctf_team_red->string, teamname)))
	{
		return NOTEAM;
	}

	// load in the ctf GPL file, that contains all the skins allowed for CTF
	if((length=gi.FS_LoadFile("ghoul/pmodels/ctf.gpl",(void **)&buffer))==-1)
	{
		gi.error("*************************\nCould not open ctf.gpl\n*************************\n");
		return NOTEAM;
	}

	s=buffer;

   	while(s)
   	{
   		char  defModName[100];

   		strcpy(NewSkin,COM_Parse(&s));
   		if(strlen(NewSkin))
   		{
   			if (!(stricmp(NewSkin, skinname)))
   			{
   				// we have a skin name, go load it in and see what team its on
   				Com_sprintf(defModName, sizeof(defModName), "%s.gpm", NewSkin);
   				IPlayerModelInfoC *aPlayer=gi.NewPlayerModelInfo(defModName);
   				// if we are on the team name provided, then we found both team and skin
   				aPlayer->GetTeamInfo(team);
   				delete aPlayer;
   				if (!(stricmp(teamname, team.name)))
   				{
   					// Clean up...
					if (!(stricmp(team.name, ctf_team_blue->string)))
					{
						gi.FS_FreeFile(buffer);
	   					return TEAM1;		
					}
					else
					{
						gi.FS_FreeFile(buffer);
						return TEAM2;
					}
   				}
   			}
   		}
   	}

	// Clean up...
	gi.FS_FreeFile(buffer);
	
	// load in the custom GPL file, that contains all the skins allowed for CTF
	// if we have a secondary GPL file...
	if (sv_altpmodlistfile->string[0])
	{
		Com_sprintf(custom_filename, sizeof(custom_filename),"%s.gpl", sv_altpmodlistfile->string);
		if((length=gi.FS_LoadFile(custom_filename,(void **)&buffer))==-1)
		{
			return NOTEAM;
		}

		s=buffer;

   		while(s)
   		{
   			char  defModName[100];

   			strcpy(NewSkin,COM_Parse(&s));
   			if(strlen(NewSkin))
   			{
   				if (!(stricmp(NewSkin, skinname)))
   				{
   					// we have a skin name, go load it in and see what team its on
   					Com_sprintf(defModName, sizeof(defModName), "%s.gpm", NewSkin);
   					IPlayerModelInfoC *aPlayer=gi.NewPlayerModelInfo(defModName);
   					// if we are on the team name provided, then we found both team and skin
   					aPlayer->GetTeamInfo(team);
   					delete aPlayer;
   					if (!(stricmp(teamname, team.name)))
   					{
   						// Clean up...
						if (!(stricmp(team.name, ctf_team_blue->string)))
						{
							gi.FS_FreeFile(buffer);
	   						return TEAM1;		
						}
						else
						{
							gi.FS_FreeFile(buffer);
							return TEAM2;
						}
   					}
   				}
   			}
   		}
		// Clean up...
		gi.FS_FreeFile(buffer);
	}

	return NOTEAM;
}

/*
==================
dmctf_c::AssignTeam
==================
*/
void dmctf_c::AssignTeam(edict_t *who, char *userinfo)
{
	edict_t	*player;
	int		i;
	int		team1count = 0, team2count = 0;
	int		playernum;
	bool	killed_him = false;

	char skin[1000],teamname[1000];
	char newskin[1000],newteamname[1000];

	// what we want to become
	Com_sprintf(teamname,sizeof(teamname),"%s", Info_ValueForKey(userinfo,"teamname"));
	// sanity checking
	if (strlen(teamname) > 128)
		gi.dprintf("CTF AssignTeam: teamname exceeded 128 - thought you'd like to know\n");
	Com_sprintf(skin,sizeof(skin),"%s", Info_ValueForKey(userinfo,"skin"));
	// sanity checking
	if (strlen(skin) > 128)
		gi.dprintf("CTF AssignTeam: skin exceeded 128 - thought you'd like to know\n");

	// what we are currently
	PB_GetActualSkinName(who,newskin);
	// sanity checking
	if (strlen(newskin) > 128)
		gi.dprintf("CTF AssignTeam: newskin exceeded 128 - thought you'd like to know\n");

	PB_GetActualTeamName(who,newteamname);
	// sanity checking
	if (strlen(newteamname) > 128)
		gi.dprintf("CTF AssignTeam: newteamname exceeded 128 - thought you'd like to know\n");


	// if we already have the skin we wanted, plus we have a team, just dip out, we don't need to do anything.
	if((!stricmp(newskin,skin)) && (!stricmp(newteamname, teamname)) && who->client->resp.team)
		return;

	// only do this if the FORCEJOIN flag is NOT set.
	if (!dmRule_FORCEJOIN())
	{
		team_t		new_team;

		// decide if the skin selection is appropriate
		new_team = CompareSkin(Info_ValueForKey(userinfo,"skin"), Info_ValueForKey(userinfo,"teamname"));

		// if this is not the first time through then we are already playing so in case we change teams, we'd better gib the player
		if (who->client->resp.team && (who->client->resp.team != new_team) && (who->client->pers.connected) && (who->inuse))
		{
			gi.dprintf("Start AssignTeamKillPlayer %s\n", who->client->pers.netname);
			gi.dprintf("   new_team: %d\n", new_team);
			gi.dprintf("   old_team: %d\n", who->client->resp.team);
			gi.dprintf("   teamname: %s\n", teamname);
			gi.dprintf("   skin: %s\n", skin);

			who->flags &= ~FL_GODMODE;
			who->health = 0;
			meansOfDeath = MOD_SUICIDE;
			player_die (who, who, who, 100000, vec3_origin);
			// Don't even bother waiting for death frames before respawning.
			who->deadflag = DEAD_DEAD;
			// since we are changing teams, zero his score
			who->client->resp.score = 0;
			killed_him = true;
			gi.dprintf("End AssignTeamKillPlayer\n");
		}

		if (new_team)
		{
			gi.dprintf("Start AssignTeamCreatePlayer %s\n", who->client->pers.netname);
			gi.dprintf("   new_team: %d\n", new_team);
			gi.dprintf("   teamname: %s\n", teamname);
			gi.dprintf("   skin: %s\n", skin);

			who->client->resp.team = new_team;
			// has to be done
			PB_InitBody(*who,userinfo);
			// just to be sure, lets force those new skins to old ones
			playernum = who-g_edicts-1;
			gi.configstring(CS_PLAYERSKINS+playernum,va("%s\\%s\\%s",who->client->pers.netname,teamname,skin));
			gi.dprintf("End AssignTeamCreatePlayer\n");
			return;
		}
		// other wise fall through, put up a message and do the random team assign
		gi.SP_Print(who, DM_CTF_BAD_SKIN);

	}
	// ok - we are playing a FORCEJOIN game. If we are already have a team, dump us out. We shouldn't be messing with skins if we've already joined a FORCE JOIN game
	else
	if (who->client->resp.team)
	{
		// tell player he can't change his skin
		gi.SP_Print(who, DM_CTF_NO_CHANGE);

		// reset skin if its changed
		if(stricmp(newskin,skin))
		{
			skin[0]='*';
			skin[1]=0;
			strcat(skin+1,newskin);	//FIXME:
			Info_SetValueForKey(userinfo,"skin",newskin);
		}

		// reset team if its changed
		if(stricmp(newteamname,teamname))
		{
			teamname[0]='*';
			teamname[1]=0;
			strcat(teamname+1,newteamname);	//FIXME:
			Info_SetValueForKey(userinfo,"teamname",newteamname);
		}

		// just to be sure, lets force those new skins to old ones
		playernum = who-g_edicts-1;
		gi.configstring(CS_PLAYERSKINS+playernum,va("%s\\%s\\%s",who->client->pers.netname,teamname,skin));

		return;
	}

   	// count how many are on each team
   	for (i = 1; i <= (int)maxclients->value; i++) 
   	{
   		player = &g_edicts[i];

   		if (player == who)
   			continue;

   		switch (player->client->resp.team) 
   		{
   		case TEAM1:
   			team1count++;
   			break;
   		case TEAM2:
   			team2count++;
   		}
   	}

   	// assign a team to this player
   	if (team1count < team2count)
   		who->client->resp.team = TEAM1;
   	else
	if (team2count < team1count)
   		who->client->resp.team = TEAM2;
   	else
	if (gi.irand(0,1))
   		who->client->resp.team = TEAM1;
   	else
   		who->client->resp.team = TEAM2;

	// ok -now a team has been assigned, lets assign a skin
	if (who->client->resp.team == TEAM1)
	{
		teamname[0]='*';
		teamname[1]=0;
		strcat(teamname+1,ctf_team_blue->string); 
		Info_SetValueForKey(userinfo,"teamname",ctf_team_blue->string);
		FindSkin(ctf_team_blue->string, gi.irand(0,num_blue_skins-1));
		skin[0]='*';
		skin[1]=0;
		strcat(skin+1,NewSkin);	
		Info_SetValueForKey(userinfo,"skin",NewSkin);
		gi.SP_Print(who, DM_CTF_ASSIGN_BLUE, ctf_team_blue->string,NewSkin);
	}
	else
	{
		teamname[0]='*';
		teamname[1]=0;
		strcat(teamname+1,ctf_team_red->string); 
		Info_SetValueForKey(userinfo,"teamname",ctf_team_red->string);
		FindSkin(ctf_team_red->string, gi.irand(0,num_red_skins-1));
		skin[0]='*';
		skin[1]=0;
		strcat(skin+1,NewSkin);	
		Info_SetValueForKey(userinfo,"skin",NewSkin);
		gi.SP_Print(who, DM_CTF_ASSIGN_RED, ctf_team_red->string,NewSkin);
	}

	playernum = who-g_edicts-1;
	gi.configstring(CS_PLAYERSKINS+playernum,va("%s\\%s\\%s",who->client->pers.netname,teamname,skin));
	PB_InitBody(*who,userinfo);

}

/*
==================
dmctf_c::FindFlagEdict
==================
*/

edict_t	*dmctf_c::FindFlagEdict(int team)
{
	edict_t	*other;
	int		i;

	// firstly, look through all the players to see if anyone has it
	for(i=1;i<=globals.max_edicts;i++)
	{
		other=&g_edicts[i];
	
		if(!other->inuse)
			continue;

		// found it?
		if (other->ctf_flags == team)
			return other;
	}
	// ok, couldn't find it - Uh oh.
	assert(0);

	return	NULL;
}


/*
==================
dmctf_c::clientDie
==================
*/

void dmctf_c::clientDie(edict_t &ent, edict_t &inflictor, edict_t &killer)
{
	// record this value, since we are about to lose it if when we drop the flag
	int	ctf_flags = ent.ctf_flags;
	int		ent_team = ent.client->resp.team;

	// drop the flag if we have it
	if(ent.client->inv->hasItemType(SFE_CTFFLAG))
	{
		clientDropItem(&ent, SFE_CTFFLAG, 0);
		// reset our flag count to 0 again
		ent.client->inv->removeSpecificItem(SFE_CTFFLAG);
	}

	// Lose a frag?
	if((!killer.client)||(&killer==&ent))
	{	
		// I was killed by non player or killed myself.

		if (&killer==&ent)
		{
			// the killer is me
			ent.client->resp.score -= sv_suicidepenalty->value;
		}
		else
		{
			ent.client->resp.score--;
		}

		return;
	}

	// Must've been killed by another player.

	if(meansOfDeath & MOD_FRIENDLY_FIRE)
	{
		killer.client->resp.score--;
		// was I the flag carrier? if so, knock off 20 points for killing me
		if (ctf_flags)
		{
			killer.client->resp.score -= 19;
		}
	}
	else
	{
		edict_t	*flag = NULL;

		killer.client->resp.score++;
		// if we just hurt his flag carrier, give him another bonus
		if ((level.time - ent.ctf_hurt_carrier) < CTF_TIME_OUT)
		{
			if (ent_team == 2)
			{
				gi.SP_Print(NULL, DM_CTF_FRAG_HURT_CARRIER_BLUE , killer.s.number, ent.s.number, CTF_CARRIER_DANGER_PROTECT_BONUS);
			}
			else
			{
				gi.SP_Print(NULL, DM_CTF_FRAG_HURT_CARRIER_RED , killer.s.number, ent.s.number, CTF_CARRIER_DANGER_PROTECT_BONUS);
			}
			killer.client->resp.score += CTF_CARRIER_DANGER_PROTECT_BONUS;
		}
		// ok, if we held the killers team flag, give the other guy more points.
		if (ctf_flags & killer.client->resp.team)
		{
			if (ctf_flags == 2)
			{
				gi.SP_Print(NULL, DM_CTF_FRAG_CARRIER_BLUE , killer.s.number, CTF_FRAG_CARRIER_BONUS);
			}
			else
			{
				gi.SP_Print(NULL, DM_CTF_FRAG_CARRIER_RED , killer.s.number, CTF_FRAG_CARRIER_BONUS);
			}
			killer.client->resp.score += CTF_FRAG_CARRIER_BONUS;
		}
		else
		{
			// find the flag edict
			flag = FindFlagEdict(killer.client->resp.team);
			// just in case
			if (flag)
			{
				vec3_t	sub_vec;
				float	distance;

				// figure out the vector length between you and the flag
				VectorSubtract(ent.s.origin, flag->s.origin, sub_vec);
				distance = VectorLength(sub_vec);
				// if we are close enough, give us the bonus
				if (distance <= CTF_FLAG_DEFENSE_RADIUS)
				{
					if (killer.client->resp.team == 1)
					{
						gi.SP_Print(NULL, DM_CTF_PROTECT_FLAG_BLUE , killer.s.number, CTF_FLAG_DEFENSE_BONUS);
					}
					else
					{
						gi.SP_Print(NULL, DM_CTF_PROTECT_FLAG_RED , killer.s.number, CTF_FLAG_DEFENSE_BONUS);
					}
					killer.client->resp.score += CTF_FLAG_DEFENSE_BONUS;
				}
			}
		}
	}
}

/*
==================
dmctf_c::clientDisconnect
==================
*/

void dmctf_c::clientDisconnect(edict_t &ent)
{
	ent.client->resp.team = NOTEAM;
	// drop the flag if we have it
 	// if its a flag slot, and we have a flag there, drop it
	if(ent.client->inv->hasItemType(SFE_CTFFLAG))
		clientDropItem(&ent, SFE_CTFFLAG, 0);
	// close the log file if we have one open
	closeLogFile();

}

/*
==================
dmctf_c::clientRespawn
==================
*/

void dmctf_c::clientRespawn(edict_t &ent)
{
	sharedEdict_t	sh;

	sh.inv = (inven_c *)ent.client->inv;
	sh.edict = &ent;
	sh.inv->setOwner(&sh);

	sh.inv->clearInv(true);

	sh.inv->rulesSetWeaponsUseAmmo(dm->dmRule_INFINITE_AMMO()?0:-1);	
	sh.inv->rulesSetWeaponsReload(dm->dmRule_NOWEAPRELOAD()?0:-1);
	sh.inv->rulesSetBestWeaponMode(Info_ValueForKey(ent.client->pers.userinfo,"bestweap"));
	sh.inv->rulesSetDropWeapons(!dm->dmRule_WEAPONS_STAY());
	sh.inv->rulesSetNonDroppableWeaponTypes(1<<SFW_KNIFE);
	if(dm->dmRule_REALISTIC_DAMAGE())
		sh.inv->rulesSetNonDroppableWeaponTypes(1<<SFW_PISTOL1);

	sh.inv->addWeaponType(SFW_KNIFE);
	sh.inv->addWeaponType(SFW_PISTOL1);
	sh.inv->addWeaponType(SFW_HURTHAND);
	sh.inv->addWeaponType(SFW_THROWHAND);

	if(!dm->dmRule_INFINITE_AMMO())
	{
		sh.inv->addAmmoType(AMMO_KNIFE,6);	
		sh.inv->addAmmoType(AMMO_9MM,150);
	}
	
	sh.inv->selectWeapon(SFW_PISTOL1);
}


/*
==================
dmctf_c::clientScoreboardMessage
==================
*/
#define BOARD_X_OFFSET 180
void dmctf_c::clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file)
{
	int					i,j,k;
	int					sorted[2][MAX_CLIENTS];
	int					sortedscores[2][MAX_CLIENTS];
	struct TeamInfo_s	teams[MAX_CLIENTS];
	int					score,total[2],total_teams, real_total[2], team;
	int					x,y,top,z;
	gclient_t			*cl;
	edict_t				*cl_ent;

	total[0] = total[1] =total_teams = 0;
	teams[0].teamname = teams[1].teamname = 0;
	memset(sortedscores, 0, sizeof(sortedscores));

	// Is logging to a log-file enabled?

	if(log_file)
		openLogFile();

	// Sort all the clients by score from highest to lowest.

	for(i=0;i<game.maxclients;i++)
	{
		cl_ent=g_edicts+1+i;

		if(!cl_ent->inuse)
			continue;

		// Spectators get added separately if there's any room left.

		if(cl_ent->client->resp.spectator)
			continue;

		// get our score and team
		score = game.clients[i].resp.score;
		team = game.clients[i].resp.team -1;

		// figure out where we are in this teams list
		for(j=0;j<total[team];j++)
		{
			if(score > sortedscores[team][j])
			{
				break;
			}
		}

		// shuffle us a hole to put the new score in
		for(k=total[team];k>j;k--)
		{
			sorted[team][k]=sorted[team][k-1];
			sortedscores[team][k]=sortedscores[team][k-1];
		}
		
		// insert the score
		sorted[team][j]=i;
		sortedscores[team][j]=score;
		total[team]++;

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
			teams[total_teams].teamname = Info_ValueForKey (cl_ent->client->pers.userinfo, "teamname");
			if (!stricmp(teams[total_teams].teamname, ctf_team_blue->string))
			{
				teams[total_teams].flags = blue_collected;
				teams[total_teams].team = TEAM1;
			}
			else
			{
				teams[total_teams].flags = red_collected;
				teams[total_teams].team = TEAM2;
			}
			total_teams++;
		}
	}

	real_total[0]=total[0];
	real_total[1]=total[1];

	if(total[0]>12)
	{
		total[0]=12;
	}
	if(total[1]>12)
	{
		total[1]=12;
	}
	
	// Clear the client's layout.

	top=32;
	gi.SP_Print(ent,DM_GENERIC_LAYOUT_RESET);

	// Handle team aggregate scores.

   	top = -64;

   	for(i=0;i<total_teams;i++)
   	{
   		
   		x=BOARD_X_OFFSET*(teams[i].team - 1);
   		
   		gi.SP_Print(ent,DM_CTF_LAYOUT_SCOREBOARD_TEAM,
   					(short)x,(short)top,
   					teams[i].rep,
   					(short)teams[i].score,
					(short)teams[i].flags);

   	}

	for (z=0; z<2; z++)
	{
   		// Write the scores to the open log-file if we have one.

   		if(filePtr)
   		{
			if(teams[z].teamname)
			{
   				fprintf(filePtr,"%sTeam %s\n",log_file_line_header->string,teams[z].teamname);
   				fprintf(filePtr,"%sScore %i\n",log_file_line_header->string,teams[z].score);
   				fprintf(filePtr,"%sFlag Captures %i\n\n",log_file_line_header->string,teams[z].flags);
			}
   		}

		top = 0;

		y = top;

		// Send the scores for all active players to the client's layout.

		for(i=0;i<total[z];i++)
		{
			cl=&game.clients[sorted[z][i]];
			cl_ent=g_edicts+1+sorted[z][i];
			
			x=(cl->resp.team -1) * BOARD_X_OFFSET;
			
			gi.SP_Print(ent,DM_GENERIC_LAYOUT_CTF_CLIENT,
						(short)x,(short)y,
						sorted[z][i],
						(short)cl->resp.score,
						(unsigned short)cl->ping,
						(unsigned short)((level.framenum-cl->resp.enterframe)/600),
						(short)cl_ent->ctf_flags);

			y+=32;
		}

		// Write the scores to the open log-file if we have one.

		if(filePtr)
		{
			for(i=0;i<real_total[z];i++)
			{
				cl=&game.clients[sorted[z][i]];
				cl_ent=g_edicts+1+sorted[z][i];
				
				fprintf(filePtr,"%sClient %s\n",log_file_line_header->string,cl_ent->client->pers.netname);
				fprintf(filePtr,"%sScore %i\n",log_file_line_header->string,cl->resp.score);
				fprintf(filePtr,"%sPing %i\n",log_file_line_header->string,cl->ping);
				fprintf(filePtr,"%sTime %i\n%s\n",log_file_line_header->string,(level.framenum-cl->resp.enterframe)/600,log_file_line_header->string);
			}
		}
	}

	// Send the scores for all spectators to the client's layout - if there's
	// any room left on the scoreboard.

	i=(total[0]>total[1])?total[0]:total[1];

	if(i<12)
	{
		y=top=i*32;
		i=0;
		j=0;

		while((j<game.maxclients)&&(i<12))
		{
			cl=&game.clients[j];
			cl_ent=g_edicts+1+j;

			if((!cl_ent->inuse)||(!cl_ent->client->resp.spectator))
			{
				j++;
				continue;
			}

			x=(i>=3)?BOARD_X_OFFSET:0;
			
			if(i==3)
				y=top;

			gi.SP_Print(ent,DM_GENERIC_LAYOUT_SCOREBAORD_SPECTATOR,
						(short)x,(short)y,
						j,
						(unsigned short)cl->ping,
						(unsigned short)((level.framenum-cl->resp.enterframe)/600));

			y+=32;
			i++;
			j++;
		}
	}

	// Write the spectators to the open log-file if we have one.

	if(filePtr)
	{
		for(i=0;i<game.maxclients;i++)
		{
			cl=&game.clients[i];
			cl_ent=g_edicts+1+i;

			if((!cl_ent->inuse)||(!cl_ent->client->resp.spectator))
				continue;
				
			fprintf(filePtr,"%sClient %s\n",log_file_line_header->string,cl_ent->client->pers.netname);
			fprintf(filePtr,"%s %s\n",log_file_line_header->string,"spectator");
			fprintf(filePtr,"%sPing %i\n",log_file_line_header->string,cl->ping);
			fprintf(filePtr,"%sTime %i\n%s\n",log_file_line_header->string,(level.framenum-cl->resp.enterframe)/600,log_file_line_header->string);
		}
	}

	// Close the open log file if we have one.

	closeLogFile();

}

/*================
  CaptureLimitHit
================*/

bool dmctf_c::CaptureLimitHit(int limit)
{
	if (!limit)
	{
		return false;
	}
	if (blue_collected >= limit)
	{
		return true;
	}
	if (red_collected >= limit)
	{
		return true;
	}
	return false;
}


// NOT PART OF THE RULES CLASS - was in g_misc.c but for the sake of consistency, I'm adding it here.

//=================================================================================

void find_flag(char *flag_class_type, team_t team)
{
	edict_t		*flag = NULL;
	int			i;
	Pickup			*pickup = thePickupList.GetPickupFromSpawnName("ctf");

	// firstly, look through all the players to see if anyone has it
	for(i=1;i<=globals.max_edicts;i++)
	{
		flag=&g_edicts[i];
	
		if(!flag->inuse)
			continue;

		// found it?
		if (flag->ctf_flags == team && stricmp(flag_class_type, flag->classname))
			break;

	}
	if (i== globals.max_edicts+1)
		return;

	// ok, did we find a dropped flag?
	if (!stricmp(flag->classname, "ctf dropped flag"))
	{
		reset_flag(flag, NULL);
		G_FreeEdict(flag);
	}
	// what about a flag attached to a player?
	else
	{
		reset_flag(flag, flag);
		// remove flag from player
		flag->ctf_flags = 0;
  		// delete the ghoul object on the players hip so we lose the flag there.
		if (flag->ghoulInst)
			PB_RemoveFlag(flag);
  		// if we have the flag klaxon sound going, turn it off
		if (flag->s.sound == gi.soundindex("player/flaglp.wav"))
		{
			flag->s.sound = 0;
		}
		// reset our flag count to 0 again
		flag->client->inv->removeSpecificItem(SFE_CTFFLAG);
	}
}

void ctf_base_touch (edict_t *self, edict_t *other, cplane_t *plane, mtexinfo_t *surf)
{
	if (!other->client)
		return;
	// determine if we have a flag, and if we do, compare it to the base we standing on - if they aren't the same, we are done
	if (!(other->ctf_flags) || (other->ctf_flags == self->count))
	{
		// nope
		return;
	}
	if (!(ctf_flag_captured->value))
	{
	 	edict_t *home = NULL;
		char	*flagname;

		// first thing we have to do is determine if our flag is still on the spawn spot
		if (self->count == TEAM1)
		{
 			flagname="ctf_flag_blue";
		}
		else
		{
 			flagname="ctf_flag_red";
		}
		// find our flag spawn point
	 	home = G_Find (home, FOFS(classname), flagname);
		if (home)
		{
			// if we don't have a flag there, just return
			if (!home->ctf_flags)
			{
 				return;
			}
		}

		gi.sound(other,CHAN_NO_PHS_ADD,gi.soundindex("dm/ctf/flagbase.wav"),1.0,ATTN_NONE,0,SND_LOCALIZE_GLOBAL);
		dm->FlagCaptured(other->client->resp.team);

   		// give us a big old bonus.
		if (other->ctf_flags == 1)
		{
			gi.SP_Print(NULL, DM_CTF_END_GAME_BONUS_BLUE, other->s.number, CTF_CAPTURE_BONUS);
		}
		else
		{
			gi.SP_Print(NULL, DM_CTF_END_GAME_BONUS_RED, other->s.number, CTF_CAPTURE_BONUS);
		}
		other->client->resp.score += CTF_CAPTURE_BONUS;

		// now, decide if we should just reset the flag, or if this map is over
		gi.cprintf(NULL, PRINT_HIGH,"Loops count %.2f, count %.2f\n",ctf_loops_count->value+1  ,ctf_loops->value);
		if (ctf_loops->value && dm->CaptureLimitHit(ctf_loops->value))	//0 is a value too.
		{
			// otherwise this game is over.
			gi.cvar_setvalue("ctf_flag_captured",  1.0);
			// remove any center screen messages we should.
			gi.Con_ClearNotify ();
		}
		// ok, we aren't over yet
		else
		{

			gi.cvar_setvalue("ctf_loops_count",  ctf_loops_count->value+ 1.0);
			// now reset both flags
			// find the blue flag
			find_flag("ctf_flag_blue", TEAM1);
			// find the red flag
			find_flag("ctf_flag_red", TEAM2);
		}
	}
}


/*QUAKED misc_ctf_base (1 0 0) (-32 -32 -24) (32 32 -16)
Stepping onto this base in possesion of the correct flag will end the current CTF game.
count = team base. 1 = team 1, 2 = team 2
*/
void SP_misc_ctf_base (edict_t *ent)
{
	// only give us these if we are playing deathmatch, and its a CTF game
	if (dm->isDM() && deathmatch->value == DM_CTF)
	{
		ent->touch = ctf_base_touch;
		ent->solid = SOLID_TRIGGER;
		VectorSet (ent->mins, -32, -32, -24);
		VectorSet (ent->maxs, 32, 32, -16);
		gi.linkentity (ent);
	}
	else
	{
		G_FreeEdict (ent);
	}
}


