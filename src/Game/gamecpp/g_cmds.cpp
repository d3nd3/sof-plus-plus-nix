#include "g_local.h"
#include "q_sh_interface.h"
#include "w_weapons.h"
#include "w_types.h"
#include "ai_path_pre.h"
#include "p_body.h"
#include "..\strings\general.h"

extern void ED_CallSpawn (edict_t *ent);
extern void sendRestartPrediction(edict_t *ent);

static int	weaponsUseAmmo,
			dropWeapons,
			nonDroppableWeaponTypes,
			weaponsReload,
			weaponReloadAutomatic,
			reloadEmptiesWeapon,
			dropInvenOnDeath,	
			selectWithNoAmmo,
			hasCTFFlag;

void saveInvenRules(inven_c *inven)
{
	weaponsUseAmmo=inven->rulesDoWeaponsUseAmmo();
	dropWeapons=inven->rulesCanDropWeapons();
	nonDroppableWeaponTypes=inven->rulesGetNonDroppableWeaponTypes();
	weaponsReload=inven->rulesDoWeaponsReload();
	weaponReloadAutomatic=inven->rulesIsWeaponReloadAutomatic();
	reloadEmptiesWeapon=inven->rulesDoReloadEmptiesWeapon();
	dropInvenOnDeath=inven->rulesCanDropInvenOnDeath();
	selectWithNoAmmo=inven->rulesCanSelectWithNoAmmo();
	hasCTFFlag=inven->hasItemType(SFE_CTFFLAG);
}

void restoreInvenRules(inven_c *inven)
{
	inven->rulesSetWeaponsUseAmmo(weaponsUseAmmo);	
	inven->rulesSetDropWeapons(dropWeapons);
	inven->rulesSetNonDroppableWeaponTypes(nonDroppableWeaponTypes);
	inven->rulesSetWeaponsReload(weaponsReload);
	inven->rulesSetWeaponReloadAutomatic(weaponReloadAutomatic);
	inven->rulesSetReloadEmptiesWeapon(reloadEmptiesWeapon);
	inven->rulesSetDropInvenOnDeath(dropInvenOnDeath);
	inven->rulesSetSelectWithNoAmmo(selectWithNoAmmo);
	if(hasCTFFlag)
		inven->addItem(SFE_CTFFLAG,1);
}

// Flood protection

qboolean CheckFlood(edict_t *ent)
{
	int					i;

	if (flood_msgs->value)
	{
        if (level.time < ent->client->flood_locktill)
		{
			gi.SP_Print(ent, GENERAL_TEXT_SHUT_UP , (short)(ent->client->flood_locktill - level.time));
            return true;
        }
        i = ent->client->flood_whenhead - flood_msgs->value + 1;
        if (i < 0)
		{
            i = (sizeof(ent->client->flood_when) / sizeof(ent->client->flood_when[0])) + i;
		}
		if (ent->client->flood_when[i] && (level.time - ent->client->flood_when[i] < flood_persecond->value))
		{
			ent->client->flood_locktill = level.time + flood_waitdelay->value;
			gi.SP_Print(ent, GENERAL_TEXT_SHUT_UP , (short)flood_waitdelay->value);
            return true;
        }
		ent->client->flood_whenhead = (ent->client->flood_whenhead + 1) % (sizeof(ent->client->flood_when) / sizeof(ent->client->flood_when[0]));
		ent->client->flood_when[ent->client->flood_whenhead] = level.time;
	}
	return false;
}


char *ClientTeam (edict_t *ent)
{
	static char	value[512];

	value[0] = 0;

	if (!ent->client)
		return value;

	switch (ent->client->resp.team)
	{
   		case TEAM1:
			return("blue");
   			break;
   
   		case TEAM2:
			return("red");
   			break;
   
   		default:
			strcpy(value, Info_ValueForKey (ent->client->pers.userinfo, "teamname"));
			return(value);
   			break;
   	}

}

bool IsInnocent(edict_t *ent)
{
	ai_c *ai1 = ( ent?(ai_c *)((ai_public_c *)ent->ai):NULL );

	if (ai1)
	{
		return !!ai1->IsInnocent();
	}
	return true;
}

qboolean OnSameTeam (edict_t *ent1, edict_t *ent2)
{
	char	ent1Team[512];
	char	ent2Team[512];

	qboolean	playerIsGood=true;

	ai_c		*ai1 = ( ent1?(ai_c *)((ai_public_c *)ent1->ai):NULL );
	ai_c		*ai2 = ( ent2?(ai_c *)((ai_public_c *)ent2->ai):NULL );

	if (!ent1 || !ent2)
	{
		return(false);
	}

	if (ai1 != NULL)
	{
		if (ai1->AmIAsGoodAsDead())
		{
			return false;
		}
	}
	if (ai2 != NULL)
	{
		if (ai2->AmIAsGoodAsDead())
		{
			return false;
		}
	}

	// Before checking between players, check monsters' loyalties.

	if (ent1->ai)
	{
		if (ent2->ai)
		{
			return (ent1->ai->GetTeam(*ent1)==ent2->ai->GetTeam(*ent2));
		}
		else//is ent1 (a monster) on same team as a player?
		{
			// Count team 0 as the good guys.

			if (ent2->client&&deathmatch->value)
			{
				playerIsGood=!strcmp(Info_ValueForKey (ent2->client->pers.userinfo, "teamname"),"mullins");
			}

			if ((ent1->ai->GetTeam(*ent1)&&playerIsGood) || !ent2->client || /*ent1->ai->GetAbusedByTeam() ||*/ !playerIsGood)
			{
				return(false);
			}
			else
			{
				return(true);
			}
		}
	}
	else
	{
		if (ent2->ai)//is ent1 (a monster) on same team as a player?
		{
			// Count team 0 as the good guys.

			if (ent1->client&&deathmatch->value)
			{
				playerIsGood=!strcmp(Info_ValueForKey (ent1->client->pers.userinfo, "teamname"),"mullins");
			}

			if ((ent2->ai->GetTeam(*ent2)&&playerIsGood) || /*ent2->ai->GetAbusedByTeam() ||*/ !playerIsGood)
			{
				return(false);
			}
			else
			{
				return(true);
			}
		}
	}

	if(!dm->dmRule_TEAMPLAY())
		return(false);

	strcpy (ent1Team, ClientTeam (ent1));
	strcpy (ent2Team, ClientTeam (ent2));

	if (strcmp(ent1Team, ent2Team) == 0)
		return(true);

	return(false);
}

//=================================================================================

/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/

void Cmd_God_f (edict_t *ent)
{
	char	*msg;

	if(!game.playerSkills.canCheat())
	{	// harder difficulty levels won't let you cheat
		gi.cprintf (ent, PRINT_HIGH, "Cheats not available.\n");
		return;
	}

	ent->flags ^= FL_GODMODE;
	if (!(ent->flags & FL_GODMODE) )
		msg = "godmode OFF\n";
	else
		msg = "godmode ON\n";

	gi.cprintf (ent, PRINT_HIGH, msg);
}

void Cmd_Playlevel_f(edict_t *ent)
{
#ifndef _DEMO_

	if(!game.playerSkills.canCheat())
	{	// harder difficulty levels won't let you cheat
		gi.cprintf (ent, PRINT_HIGH, "Cheats not available.\n");
		return;
	}

	sharedEdict_t	sh;

	sh.inv = (inven_c *)ent->client->inv;

	if (!sh.inv)
	{
		gi.dprintf("hey! no inventory!\n");
		return;
	}

	sh.edict = ent;
	sh.inv->setOwner(&sh);
	sh.inv->deactivateCurrentWeapon();
	sh.inv->deactivateInventory();

	saveInvenRules(sh.inv);

	W_InitInv(*ent);
	sh.inv = (inven_c *)ent->client->inv;
	sh.edict = ent;
	sh.inv->setOwner(&sh);
	sendRestartPrediction(ent);

	restoreInvenRules(sh.inv);
	sh.inv->rulesSetBestWeaponMode(Info_ValueForKey(ent->client->pers.userinfo,"bestweap"));
	
	sh.inv->clearInv(false);

	sh.inv->addWeaponType(SFW_KNIFE);
	sh.inv->addWeaponType(SFW_PISTOL1);
	sh.inv->addWeaponType(SFW_HURTHAND);
	sh.inv->addWeaponType(SFW_THROWHAND);

	AddWeaponTypeForPrecache(SFW_KNIFE);
	AddWeaponTypeForPrecache(SFW_PISTOL1);
	AddWeaponTypeForPrecache(SFW_HURTHAND);
	AddWeaponTypeForPrecache(SFW_THROWHAND);

	if(!dm->dmRule_INFINITE_AMMO())
	{
		sh.inv->addAmmoType(AMMO_9MM, 150);
		sh.inv->addAmmoType(AMMO_KNIFE, 6);
	}
	sh.inv->addItem(SFE_C4, 2);

	sh.inv->stockWeapons();

	pe->PrecacheViewWeaponModels(level.weaponsAvailable);
	
	sh.inv->selectWeapon(SFW_PISTOL1);

	// Remove any amror player may have... should be a better way really.

	PB_AddArmor(ent,ent->client->inv->getArmorCount()*-1);

	// Clear invulnerability.

	ent->flags &= ~FL_GODMODE;
	gi.cvar_setvalue("nodamage", 0);

#endif //_DEMO_
}

void DefaultWeapons(edict_t *ent);

void Cmd_DefaultWeapons(edict_t *ent)
{
	if (dm->isDM())
		return;

#ifdef _FINAL_	// kef -- need this cheat for testing 'challenging' skill level
	if(!game.playerSkills.canCheat())
	{	// harder difficulty levels won't let you cheat
		gi.cprintf (ent, PRINT_HIGH, "Cheats not available.\n");
		return;
	}
#endif

	DefaultWeapons(ent);
}

void Cmd_WeaponTest(edict_t *ent)
{
#ifndef _DEMO_

	if(!game.playerSkills.canCheat())
	{	// harder difficulty levels won't let you cheat
		gi.cprintf (ent, PRINT_HIGH, "Cheats not available.\n");
		return;
	}

	sharedEdict_t	sh;

	sh.inv = (inven_c *)ent->client->inv;

	if (!sh.inv)
	{
		gi.dprintf("hey! no inventory!\n");
		return;
	}

	sh.edict = ent;
	sh.inv->setOwner(&sh);
	sh.inv->deactivateCurrentWeapon();
	sh.inv->deactivateInventory();

	saveInvenRules(sh.inv);

	W_InitInv(*ent);
	sh.inv = (inven_c *)ent->client->inv;
	sh.edict = ent;
	sh.inv->setOwner(&sh);
	sendRestartPrediction(ent);

	restoreInvenRules(sh.inv);
	sh.inv->rulesSetBestWeaponMode(Info_ValueForKey(ent->client->pers.userinfo,"bestweap"));
	
	sh.inv->clearInv(false);

	sh.inv->addWeaponType(SFW_KNIFE);
	sh.inv->addWeaponType(SFW_PISTOL1);
	sh.inv->addWeaponType(SFW_PISTOL2);
	sh.inv->addWeaponType(SFW_SHOTGUN);
	sh.inv->addWeaponType(SFW_SNIPER);
	sh.inv->addWeaponType(SFW_ASSAULTRIFLE);
	sh.inv->addWeaponType(SFW_HURTHAND);
	sh.inv->addWeaponType(SFW_THROWHAND);

	AddWeaponTypeForPrecache(SFW_KNIFE);
	AddWeaponTypeForPrecache(SFW_PISTOL1);
	AddWeaponTypeForPrecache(SFW_PISTOL2);
	AddWeaponTypeForPrecache(SFW_SHOTGUN);
	AddWeaponTypeForPrecache(SFW_SNIPER);
	AddWeaponTypeForPrecache(SFW_ASSAULTRIFLE);
	AddWeaponTypeForPrecache(SFW_HURTHAND);
	AddWeaponTypeForPrecache(SFW_THROWHAND);

	if(!dm->dmRule_INFINITE_AMMO())
	{
		sh.inv->addAmmoType(AMMO_9MM, 2000);
		sh.inv->addAmmoType(AMMO_KNIFE, 6);
		sh.inv->addAmmoType(AMMO_556, 2000);
		sh.inv->addAmmoType(AMMO_44, 2000);
		sh.inv->addAmmoType(AMMO_SLUG, 2000);
		sh.inv->addAmmoType(AMMO_SHELLS, 2000);
		sh.inv->addAmmoType(AMMO_ROCKET, 2000);
		sh.inv->addAmmoType(AMMO_FTHROWER, 2000);
		sh.inv->addAmmoType(AMMO_MWAVE, 2000);
	}

	sh.inv->addItem(SFE_GRENADE, 100);
	sh.inv->addItem(SFE_LIGHT_GOGGLES, 100);
	sh.inv->addItem(SFE_FLASHPACK, 100);
	sh.inv->addItem(SFE_C4, 100);

	sh.inv->stockWeapons();

	pe->PrecacheViewWeaponModels(level.weaponsAvailable);

	sh.inv->selectWeapon(SFW_PISTOL1);
#endif //_DEMO_
}

void Cmd_AltWeaponTest(edict_t *ent)
{
#ifndef _DEMO_

	if(!game.playerSkills.canCheat())
	{	// harder difficulty levels won't let you cheat
		gi.cprintf (ent, PRINT_HIGH, "Cheats not available.\n");
		return;
	}

	sharedEdict_t	sh;

	sh.inv = (inven_c *)ent->client->inv;

	if (!sh.inv)
	{
		gi.dprintf("hey! no inventory!\n");
		return;
	}

	sh.edict = ent;
	sh.inv->setOwner(&sh);
	sh.inv->deactivateCurrentWeapon();
	sh.inv->deactivateInventory();

	saveInvenRules(sh.inv);

	W_InitInv(*ent);
	sh.inv = (inven_c *)ent->client->inv;
	sh.edict = ent;
	sh.inv->setOwner(&sh);
	sendRestartPrediction(ent);

	restoreInvenRules(sh.inv);
	sh.inv->rulesSetBestWeaponMode(Info_ValueForKey(ent->client->pers.userinfo,"bestweap"));

	sh.inv->clearInv(false);

	sh.inv->addWeaponType(SFW_KNIFE);
	sh.inv->addWeaponType(SFW_AUTOSHOTGUN);
	sh.inv->addWeaponType(SFW_ROCKET);
	sh.inv->addWeaponType(SFW_FLAMEGUN);
	sh.inv->addWeaponType(SFW_MICROWAVEPULSE);
	sh.inv->addWeaponType(SFW_MACHINEPISTOL);
	sh.inv->addWeaponType(SFW_MACHINEGUN);
	sh.inv->addWeaponType(SFW_HURTHAND);
	sh.inv->addWeaponType(SFW_THROWHAND);

	AddWeaponTypeForPrecache(SFW_KNIFE);
	AddWeaponTypeForPrecache(SFW_AUTOSHOTGUN);
	AddWeaponTypeForPrecache(SFW_ROCKET);
	AddWeaponTypeForPrecache(SFW_FLAMEGUN);
	AddWeaponTypeForPrecache(SFW_MICROWAVEPULSE);
	AddWeaponTypeForPrecache(SFW_MACHINEPISTOL);
	AddWeaponTypeForPrecache(SFW_MACHINEGUN);
	AddWeaponTypeForPrecache(SFW_HURTHAND);
	AddWeaponTypeForPrecache(SFW_THROWHAND);

	if(!dm->dmRule_INFINITE_AMMO())
	{
		sh.inv->addAmmoType(AMMO_9MM, 2000);
		sh.inv->addAmmoType(AMMO_KNIFE, 6);
		sh.inv->addAmmoType(AMMO_556, 2000);
		sh.inv->addAmmoType(AMMO_44, 2000);
		sh.inv->addAmmoType(AMMO_SLUG, 2000);
		sh.inv->addAmmoType(AMMO_SHELLS, 2000);
		sh.inv->addAmmoType(AMMO_ROCKET, 2000);
		sh.inv->addAmmoType(AMMO_FTHROWER, 2000);
		sh.inv->addAmmoType(AMMO_MWAVE, 2000);
	}

	sh.inv->addItem(SFE_GRENADE, 100);
	sh.inv->addItem(SFE_LIGHT_GOGGLES, 100);
	sh.inv->addItem(SFE_FLASHPACK, 100);
	sh.inv->addItem(SFE_C4, 100);

	sh.inv->stockWeapons();

	pe->PrecacheViewWeaponModels(level.weaponsAvailable);

	sh.inv->selectWeapon(SFW_AUTOSHOTGUN);
#endif //_DEMO_
}

void Cmd_GiveSniperTutorial(edict_t *ent)
{
#ifdef _OEM_
	return;
#endif

	if (dm->isDM())
		return;

	sharedEdict_t	sh;

	sh.inv = (inven_c *)ent->client->inv;

	if (!sh.inv)
	{
		gi.dprintf("hey! no inventory!\n");
		return;
	}

	sh.edict = ent;
	sh.inv->setOwner(&sh);
	sh.inv->deactivateCurrentWeapon();
	sh.inv->deactivateInventory();

	W_InitInv(*ent);
	sh.inv = (inven_c *)ent->client->inv;
	sh.edict = ent;
	sh.inv->setOwner(&sh);

	sendRestartPrediction(ent);
	sh.inv->clearInv(true);

	sh.inv->rulesSetWeaponsUseAmmo(-1);	
	sh.inv->rulesSetWeaponsReload(-1);
	sh.inv->rulesSetDropWeapons(0);
	sh.inv->rulesSetBestWeaponMode(Info_ValueForKey(ent->client->pers.userinfo,"bestweap"));

	sh.inv->addWeaponType(SFW_SNIPER);

	AddWeaponTypeForPrecache(SFW_SNIPER);

	sh.inv->addAmmoType(AMMO_556, 200);

	sh.inv->stockWeapons();

	pe->PrecacheViewWeaponModels(level.weaponsAvailable);

	sh.inv->takeOutWeapon(SFW_SNIPER);
}

void Cmd_GiveMoreTutorial(edict_t *ent)
{
#ifdef _OEM_
	return;
#endif

	if (dm->isDM())
		return;

	sharedEdict_t	sh;

	sh.inv = (inven_c *)ent->client->inv;

	if (!sh.inv)
	{
		gi.dprintf("hey! no inventory!\n");
		return;
	}

	sh.edict = ent;
	sh.inv->setOwner(&sh);
	sh.inv->deactivateCurrentWeapon();
	sh.inv->deactivateInventory();

	W_InitInv(*ent);
	sh.inv = (inven_c *)ent->client->inv;
	sh.edict = ent;
	sh.inv->setOwner(&sh);

	sendRestartPrediction(ent);
	sh.inv->clearInv(true);

	sh.inv->rulesSetWeaponsUseAmmo(-1);	
	sh.inv->rulesSetWeaponsReload(-1);
	sh.inv->rulesSetDropWeapons(0);
	sh.inv->rulesSetBestWeaponMode(Info_ValueForKey(ent->client->pers.userinfo,"bestweap"));

	sh.inv->addWeaponType(SFW_PISTOL2);

	AddWeaponTypeForPrecache(SFW_PISTOL2);

	sh.inv->addAmmoType(AMMO_44, 90);

	sh.inv->stockWeapons();

	pe->PrecacheViewWeaponModels(level.weaponsAvailable);

	sh.inv->takeOutWeapon(SFW_PISTOL2);
}

// if a trigger_objectives was activated in this level and we have since checked our objectives screen, this
//command will turn off the "check your objectives" message
void Cmd_TurnOffMissionMsg(edict_t *ent)
{
	if (dm->isDM())
		return;

	level.missionStatus = MISSION_NONE;
}

// 1/17/99 kef -- this really _should_ be a member of the level_locals_t struct, but adding it would require
//everyone to rebuild the project cuz it'd touch g_local.h
float		s_levelStatusBegin = 0;

void Cmd_TurnOnMissionMsg(edict_t *ent)
{
	if (dm->isDM())
		return;

	level.missionStatus = MISSION_OBJECTIVES;
	s_levelStatusBegin = level.time;
}

// spew all pickups to the console
void Cmd_ListPickups(edict_t *ent)
{
	if(!game.playerSkills.canCheat())
	{	// harder difficulty levels won't let you cheat
		gi.cprintf (ent, PRINT_HIGH, "Cheats not available.\n");
		return;
	}

	thePickupList.DumpList();
}

/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/

// because I need this variable for actual game reasons, this silly variable is introduced
int notargglobalcheck = 0;

void Cmd_Notarget_f (edict_t *ent)
{
	char	*msg;

	if(!game.playerSkills.canCheat())
	{	// harder difficulty levels won't let you cheat
		gi.cprintf (ent, PRINT_HIGH, "Cheats not available.\n");
		return;
	}

	ent->flags ^= FL_NOTARGET;
	if (!(ent->flags & FL_NOTARGET) )
	{
		notargglobalcheck = 0;
		msg = "notarget OFF\n";
	}
	else
	{
		notargglobalcheck = 1;
		msg = "notarget ON\n";
	}

	gi.cprintf (ent, PRINT_HIGH, msg);
}


/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f (edict_t *ent)
{
	char	*msg;

	if(!game.playerSkills.canCheat())
	{	// harder difficulty levels won't let you cheat
		gi.cprintf (ent, PRINT_HIGH, "Cheats not available.\n");
		return;
	}

	if (ent->movetype == MOVETYPE_NOCLIP)
	{
		ent->movetype = MOVETYPE_WALK;
		msg = "noclip OFF\n";
	}
	else
	{
		ent->movetype = MOVETYPE_NOCLIP;
		msg = "noclip ON\n";
	}

	gi.cprintf (ent, PRINT_HIGH, msg);
}


/*
=================
Cmd_Kill_f
=================
*/

void Cmd_Kill_f (edict_t *ent)
{
	// Can't suicide too soon or while still invulnerable after respawning.

	if(ent->client->flood_nextkill > level.time)
	{
		gi.SP_Print(ent, GENERAL_TEXT_NOKILL , (short)(ent->client->flood_nextkill - level.time) + 1);
		return;
	}

	if((level.time - ent->client->respawn_time) < 5)
		return;

	if(ent->client->respawn_invuln_time>=level.time)
		return;

	// Ensure we can be harmed and set up our means of death and then kill us.

	ent->flags &= ~FL_GODMODE;
	ent->health = 0;
	meansOfDeath = MOD_SUICIDE;
	player_die (ent, ent, ent, 100000, vec3_origin);
	
	// Don't even bother waiting for death frames before respawning.

	ent->deadflag = DEAD_DEAD;
	respawn (ent);
   	// Set up the next valid suicide time.
   	ent->client->flood_nextkill = level.time + flood_killdelay->value;
}

/*
=================
Cmd_KillMonsters_f
=================
*/

extern edict_t	*last_monster_used;

void Cmd_KillMonsters_f (edict_t *ent)
{
	if(!game.playerSkills.canCheat())
	{	// harder difficulty levels won't let you cheat
		gi.cprintf (ent, PRINT_HIGH, "Cheats not available.\n");
		return;
	}

	int		num = 0;
	edict_t	*curTest = g_edicts;

	for (curTest = g_edicts; curTest < &g_edicts[globals.num_edicts] ; curTest++)
	{
		if(!curTest->inuse)
			continue;

		if ((curTest->ai)&&(curTest->health >= 0)&&last_monster_used!=curTest)
		{
			// Do magical damage.

			T_Damage(curTest,ent,ent,vec3_origin,curTest->s.origin,ent->s.origin,100000,0,DAMAGE_NO_PROTECTION,MOD_SUICIDE);
			num++;
		}
	}

	gi.dprintf("killed %d monsters\n",num);
}

/*
=================
Cmd_PutAway_f
=================
*/
void Cmd_PutAway_f (edict_t *ent)
{
	ent->client->showscores = false;
	ent->client->showhelp = false;
	ent->client->showinventory = false;
}

/*
===================
Cmd_SpawnEntity_f

Spawn an entity.
===================
*/

extern char *ED_NewString (char *string);

void Cmd_SpawnEntity_f(edict_t *ent)
{
	if(!game.playerSkills.canCheat())
	{	// harder difficulty levels won't let you cheat
		gi.cprintf (ent, PRINT_HIGH, "Cheats not available.\n");
		return;
	}

	edict_t	*newent;
	vec3_t	forward,up;

	gi.cprintf(ent, PRINT_HIGH, "Spawning : %s\n", gi.argv(1));

	newent = G_Spawn();
	newent->classname = ED_NewString(gi.argv(1));
	AngleVectors(ent->s.angles, forward, NULL, up);
	VectorScale(forward, 100, forward);
	VectorScale(up, 50, up);
	VectorAdd(ent->s.origin, forward, newent->s.origin);
	VectorAdd(newent->s.origin, up, newent->s.origin);
	newent->s.angles[YAW]=ent->s.angles[YAW];
	ED_CallSpawn(newent);
}

void Cmd_Go(edict_t *ent)
{
	if(!game.playerSkills.canCheat())
	{	// harder difficulty levels won't let you cheat
		gi.cprintf (ent, PRINT_HIGH, "Cheats not available.\n");
		return;
	}

	vec3_t v;

	v[0] = atof(gi.argv(1));
	v[1] = atof(gi.argv(2));
	v[2] = atof(gi.argv(3));

	VectorCopy(v, level.sight_client->s.origin);
}

int PlayerSort (void const *a, void const *b)
{
	int		anum, bnum;

	anum = *(int *)a;
	bnum = *(int *)b;

	anum = g_edicts[anum].client->ps.stats[STAT_FRAGS];
	bnum = g_edicts[bnum].client->ps.stats[STAT_FRAGS];

	if (anum < bnum)
		return -1;
	if (anum > bnum)
		return 1;
	return 0;
}

/*
=================
Cmd_Players_f
=================
*/
void Cmd_Players_f (edict_t *ent)
{
	int		i;
	int		count;
	char	small[64];
	char	large[1280];
	int		index[256];
	edict_t		*e;

	count = 0;
	for (i = 1 ; i < (int)maxclients->value ; i++)
	{
		e = &g_edicts[i];
		if (e->inuse && e->client->pers.connected)
		{
			index[count] = i;
			count++;
		}
	}

	// sort by frags
	qsort (index, count, sizeof(index[0]), PlayerSort);

	// print information
	large[0] = 0;

	for (i = 0 ; i < count ; i++)
	{
		Com_sprintf (small, sizeof(small), "%3i %s %s\n",
			g_edicts[index[i]].client->ps.stats[STAT_FRAGS],
			g_edicts[index[i]].client->pers.netname,
			Info_ValueForKey (g_edicts[index[i]].client->pers.userinfo, "ip"));
		if (strlen (small) + strlen(large) > sizeof(large) - 100 )
		{	// can't print all of them in one packet
			strcat (large, "...\n");
			break;
		}
		strcat (large, small);
	}

	gi.cprintf (ent, PRINT_HIGH, "%s\n%i players\n", large, count);
}

/*
=================
Cmd_Wave_f
=================
*/
void Cmd_Wave_f (edict_t *ent)
{
	int		i;

	i = atoi (gi.argv(1));

	// Can't wave when ducked.
	if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
		return;

	if (ent->client->anim_priority > ANIM_WAVE)
		return;

	PB_PlaySequenceForGesture(ent, i);
	// FIXME: ok, do waving here.
}

/*
==================
Cmd_TeamView_f
==================
*/
void Cmd_TeamView_f(edict_t *ent)
{
	char	*p;
	int		teamMember;

	if (gi.argc () < 2)
		return;

	p = gi.args();

	if (*p == '"')
	{
		p++;
		p[strlen(p)-1] = 0;
	}

	teamMember=atoi(p);
	ent->client->MercCameraNumber=teamMember;

	if(teamMember==0)
	{
		// Selected self again so switch to 1st person camera view.

		ent->client->CameraIs3rdPerson=false;
		ent->client->ps.remote_type = REMOTE_TYPE_FPS;
	}
	else
	{
		// Selected one of your team-members so say we want to switch to 3rd person camera view.

		ent->client->CameraIs3rdPerson=true;
		ent->client->ps.remote_type = REMOTE_TYPE_TPS;
	}
}

/*
==================
Cmd_Say_f
==================
*/
#define CHAT_LIMIT 150

void Cmd_Say_f (edict_t *ent, qboolean team, qboolean arg0)
{
	int		j;
	edict_t	*other;
	char	*p;
	char	text[2048];
	char	text2[2048];
	int		color;

	if (gi.argc () < 2 && !arg0)
		return;

	if(!dm->dmRule_TEAMPLAY())
		team = false;

	// delimit the string if we are normal play talking, so we don't send the name with it
	Com_sprintf (text2, sizeof(text), "%s: ", ent->client->pers.netname);
	text[0] = 0;

	if (arg0)
	{
		strcat (text, gi.argv(0));
		strcat (text, " ");
		strcat (text, gi.args());

		strcat (text2, gi.argv(0));
		strcat (text2, " ");
		strcat (text2, gi.args());
	}
	else
	{
		p = gi.args();

		if (*p == '"')
		{
			p++;
			p[strlen(p)-1] = 0;
		}
		strcat(text, p);

		strcat(text2, p);

	}

	// don't let text be too long for malicious reasons
	if ((strlen(text) > CHAT_LIMIT) || (strlen(text2) > CHAT_LIMIT))
		gi.cprintf(ent, PRINT_HIGH, "Chat string too long - truncated to 150 chars\n");

	if (strlen(text) > CHAT_LIMIT)
		text[CHAT_LIMIT] = 0;

	if (strlen(text2) > CHAT_LIMIT)
		text2[CHAT_LIMIT] = 0;


	strcat(text, "\n");
	strcat(text2, "\n");

	if (CheckFlood(ent))
		return;

	if (dedicated->value)
		gi.cprintf(NULL, PRINT_CHAT, "%s", text2);

	for (j = 1; j <= game.maxclients; j++)
	{
		color = 0;
		other = &g_edicts[j];
		if (!other->inuse)
			continue;
		if (!other->client)
			continue;
		if (team)
		{
			color = 1;
			if (!OnSameTeam(ent, other))
				continue;
		}

		gi.clprintf(other,ent, color, "%s", text);	
	}
}


/*
==================
Cmd_PlayerList_f

Connect time, ping, score, name etc.
==================
*/

void Cmd_PlayerList_f(edict_t *ent)
{
	int		i;
	char	st[80],
			text[1400];
	edict_t *e2;

	*text = 0;

	for(i=0, e2=g_edicts+1; i<(int)maxclients->value; i++, e2++)
	{
		if(!e2->inuse)
			continue;

		sprintf(st,"%02d:%02d %4d %3d %s%s\n",
			    (level.framenum - e2->client->resp.enterframe) / 600,
				((level.framenum - e2->client->resp.enterframe) % 600)/10,
				e2->client->ping,
				e2->client->resp.score,
				e2->client->pers.netname,
				e2->client->resp.spectator ? " (spectator)" : "");

		if (strlen(text) + strlen(st) > sizeof(text) - 50)
		{
			sprintf(text+strlen(text), "And more...\n");
			//FIXME:
//			gi.cprintf(ent, PRINT_HIGH, "%s", text);
			return;
		}
		
		strcat(text, st);
	}
	
	//FIXME:
//	gi.cprintf(ent, PRINT_HIGH, "%s", text);
}

/*
==================
Cmd_EntList
==================
*/

typedef struct entList_s
{
	int		count;
	char	buffer[256];
}entList_t;

void Cmd_EntList(edict_t *ent2)
{
	if(!game.playerSkills.canCheat())
	{	// harder difficulty levels won't let you cheat
		gi.cprintf (ent2, PRINT_HIGH, "Cheats not available.\n");
		return;
	}

	list<entList_t>		groups;
	list<entList_t>::iterator curCheck;

	entList_t				newList;

	edict_t *ent = &g_edicts[0];
	for (int i=0 ; i<globals.num_edicts ; i++, ent++)
	{
		if(!ent->inuse)continue;

		strcpy(newList.buffer, ent->classname);
		newList.count = 1;

		int found = 0;

		if(groups.size())
		{
		
			for(curCheck = groups.begin(); (curCheck != groups.end())&&(!found); curCheck++)
			{
				if(!strcmp((*curCheck).buffer, newList.buffer))
				{
					(*curCheck).count++;
					found = 1;
				}
				else if(strcmp((*curCheck).buffer, newList.buffer) > 0)
				{	//insert in proper alphabetical order
					groups.insert(curCheck, newList);
					found = 1;
				}
			}
		}
		else
		{
			curCheck = groups.end();
			groups.insert(curCheck, newList);
		}
	}

	if(groups.size())
	{
		int funcs = 0;
		int monsters = 0;
		int total = 0;

		Com_Printf("Size Type\n");
		Com_Printf("---- --------------------------------------------------\n");
		for(curCheck = groups.begin(); curCheck != groups.end(); curCheck++)
		{
			Com_Printf("%4.d %s\n", (*curCheck).count, (*curCheck).buffer);
			total += (*curCheck).count;
			if(!strncmp((*curCheck).buffer, "func", 4))funcs += (*curCheck).count;
			if(!strncmp((*curCheck).buffer, "monster", 7))monsters += (*curCheck).count;
		}
		Com_Printf("---- --------------------------------------------------\n");
		Com_Printf("%.4d Monsters\n", monsters);
		Com_Printf("%.4d Funcs\n", funcs);
		Com_Printf("%.4d Total\n", total);
	}
	groups.clear();

}

void G_GetGameStats(void);

void Cmd_UpdateInvenFinal(edict_t *ent)
{
	if(!dm->isDM())
	{	// let the server know what kind of goodies we're coming out of here with - yeah!

		level.cashEarned += gi.cvar_variablevalue("stat_moneyadd");
		gi.cvar_setvalue("stat_moneyadd", 0.0f);

		//this should set everything up properly so we can retrieve these next level
		G_GetGameStats();

		sharedEdict_t	sh;

		sh.inv = (inven_c *)g_edicts[1].client->inv;

		if (sh.inv)
		{
			sh.edict = &g_edicts[1];
			sh.inv->setOwner(&sh);
			sh.inv->buildInvForMenu(1, ent->health);
		}

		//also add in our stats
		game.guysKilled += level.guysKilled;
		game.friendliesKilled += level.friendliesKilled;

		game.throatShots += level.throatShots;
		game.nutShots += level.nutShots;
		game.headShots += level.headShots;
		game.gibs += level.gibs;

		game.savesUsed += level.savesUsed;
		game.cashEarned += level.cashEarned;
		game.playedTime += level.time - level.startTime;

		level.guysKilled = 0;
		level.friendliesKilled = 0;

		level.throatShots = 0;
		level.nutShots = 0;
		level.headShots = 0;
		level.gibs = 0;

		level.savesUsed = 0;
		level.cashEarned = 0;
		level.time = 0;
		level.startTime = 0;
	}
}

/*
=================
ClientCommand
=================
*/

void ClientCommand (edict_t *ent)
{
	char	*cmd;

	if (!ent->client)
	{
		// Not fully in game yet.

		return;
	}

	cmd = gi.argv(0);

	if (stricmp (cmd, "players") == 0)
	{
		Cmd_Players_f (ent);
		return;
	}
	
	if (stricmp (cmd, "say") == 0)
	{
		Cmd_Say_f (ent, false, false);
		return;
	}
	
	if (stricmp (cmd, "say_team") == 0)
	{
		Cmd_Say_f (ent, true, false);
		return;
	}
	
	if (stricmp (cmd, "score") == 0)
	{
		Cmd_Score_f (ent);
		return;
	}

	if (stricmp (cmd, "help") == 0)
	{
		Cmd_Help_f (ent);
		return;
	}

	if (level.intermissiontime)
		return;

	if (ent->client->ps.cinematicfreeze)
		return;

	if(!ent->client->resp.spectator)
		if(ent->client->inv && ent->client->inv->addCommand(cmd))
			return;

	if(!aiList && ((!stricmp(cmd, "loadpoint"))||(!stricmp(cmd, "destroyents"))))
	{
		aiList = new CAIListData;//er, where does this get killed?
	}

	if(aiList && aiList->usesCmd(ent, cmd))
		return;

	if (stricmp (cmd, "playlevel") == 0)
	{
		Cmd_Playlevel_f(ent);
		return;		
	}

	if (stricmp (cmd, "elbow") == 0)
//	if (stricmp (cmd, "weapontest") == 0)
	{
		Cmd_WeaponTest(ent);
		return;		
	}
	
	if (stricmp (cmd, "bigelbow") == 0)
//	if (stricmp (cmd, "altweapontest") == 0)
	{
		Cmd_AltWeaponTest(ent);
		return;
	}

	if (stricmp (cmd, "defaultweapons") == 0)
	{
		Cmd_DefaultWeapons(ent);
		return;
	}

	if (stricmp (cmd, "updateinvfinal") == 0)
	{
		Cmd_UpdateInvenFinal(ent);
		return;
	}

	if(stricmp (cmd, "go") == 0)
	{
		Cmd_Go(ent);
		return;
	}

	if (stricmp (cmd, "heretic") == 0)
//	if (stricmp (cmd, "god") == 0)
		Cmd_God_f (ent);
	else if (stricmp (cmd, "ninja") == 0)
//	else if (stricmp (cmd, "notarget") == 0)
		Cmd_Notarget_f (ent);
	else if (stricmp (cmd, "phantom") == 0)
//	else if (stricmp (cmd, "noclip") == 0)
		Cmd_Noclip_f (ent);
	else if (stricmp (cmd, "kill") == 0)
		Cmd_Kill_f (ent);
	else if (stricmp (cmd, "killallmonsters") == 0)
		Cmd_KillMonsters_f (ent);
	else if (stricmp (cmd, "putaway") == 0)
		Cmd_PutAway_f (ent);
	else if (stricmp (cmd, "wave") == 0)
		Cmd_Wave_f (ent);
	else if (stricmp (cmd, "gimme") == 0)
//	else if (stricmp (cmd, "spawn") == 0)
		Cmd_SpawnEntity_f (ent);
	else if (stricmp (cmd, "teamview") == 0)
		Cmd_TeamView_f (ent);
	else if (stricmp (cmd, "playerlist") == 0)
		Cmd_PlayerList_f(ent);
	else if (stricmp (cmd, "entlist") == 0)
		Cmd_EntList(ent);
	else if (stricmp (cmd, "givesnipertutorial") == 0)
	{
		Cmd_GiveSniperTutorial(ent);
	}
	else if (stricmp (cmd, "givemoretutorial") == 0)
	{
		Cmd_GiveMoreTutorial(ent);
	}
	else if (stricmp (cmd, "turnoffmissionmsg") == 0)
	{
		Cmd_TurnOffMissionMsg(ent);
	}
	else if (stricmp (cmd, "turnonmissionmsg") == 0)
	{
		Cmd_TurnOnMissionMsg(ent);
	}
	else if (stricmp (cmd, "listpickups") == 0)
	{
		Cmd_ListPickups(ent);
	}
	else	
	{	
		// Anything that doesn't match a command will be a chat.

		Cmd_Say_f (ent, false, true);
	}
}
