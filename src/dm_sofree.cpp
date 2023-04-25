#include "common.h"
void dm_sofree_c::removeHooks(void) {
	orig_Com_Printf("WHY AM I NOT BEING CALLED RMEOVE REMOVE REMOVE!!\n");
	// Scoreboard::removeHooks();
}


void dm_sofree_c::applyHooks(void) {
	// Scoreboard::applyHooks();

	
}


void dm_sofree_c::levelInit(void) {
	// Scoreboard::onMapBegin();
	
	// Menu::onMapBegin();
}

void dm_sofree_c::clientConnect(edict_t * ent) {

	// Scoreboard::clientConnect(ent);
	// Menu::generateMenu(ent);
	// Scoreboard::drawCreditImage(ent);
}
/*
40002C90 - becomedisguised
*/
void dm_sofree_c::clientRespawn(edict_t *ent) {
	#if 0

	int p = get_player_slot_from_ent(ent);

	sharedEdict_t	sh;

	sh.inv = (inven_c *)ent->client->inv;
	sh.edict = ent;
	sh.inv->setOwner(&sh);

	sh.inv->clearInv(true);

	
	//infinite ammo
	sh.inv->rulesSetWeaponsUseAmmo(0);//-1 for yes
	//reload on or off
	sh.inv->rulesSetWeaponsReload(-1);//-1 for yes
	//interesting u can force weapon switching
	sh.inv->rulesSetBestWeaponMode(orig_Info_ValueForKey(ent->client->pers.userinfo,"bestweap"));
	//if weapons stay, you cant drop weaps
	sh.inv->rulesSetDropWeapons(1);
	//which weap can't you drop?
	sh.inv->rulesSetNonDroppableWeaponTypes(1<<SFW_KNIFE);

	sh.inv->addWeaponType(SFW_KNIFE);
	sh.inv->addWeaponType(SFW_PISTOL1);
	sh.inv->addWeaponType(SFW_HURTHAND);
	sh.inv->addWeaponType(SFW_THROWHAND);

	sh.inv->selectWeapon(SFW_PISTOL1);
	

	//let the scoreboard be on always.
	// Scoreboard::clientRespawn(ent);
	// orig->clientRespawn(ent);

	//set collision?
	if ( !player_collision[p] )
		ent->solid = SOLID_TRIGGER;
	else
		ent->solid = SOLID_BBOX;


	#endif
}

//sof ree
//\x24\xF8\x46 \xAE\xC9\xC9
char* dm_sofree_c::getGameName(void) {
	return "\x17""\x24\xF8\x46""\x07""\xAE\xC9\xC9";
}
/*
called for each client.
scoreboard is sent and drawn for each client.
ClientEndServerFrame -> clientScoreboardMessage
*/
void dm_sofree_c::clientScoreboardMessage(edict_t *ent, edict_t *killer, qboolean log_file) {
	// Scoreboard::drawMe(ent,killer,log_file);
}

int	dm_sofree_c::dmRule_NO_HEALTH(void)
{
	return orig->dmRule_NO_HEALTH();
}

int	dm_sofree_c::dmRule_NO_ITEMS(void)
{
	return orig->dmRule_NO_ITEMS();
}

int	dm_sofree_c::dmRule_WEAPONS_STAY(void)
{
	return orig->dmRule_WEAPONS_STAY();
}

int	dm_sofree_c::dmRule_NO_FALLING(void)
{
	return orig->dmRule_NO_FALLING();
}

int	dm_sofree_c::dmRule_REALISTIC_DAMAGE(void)
{
	return orig->dmRule_REALISTIC_DAMAGE();
}

int	dm_sofree_c::dmRule_SAME_LEVEL(void)
{
	return orig->dmRule_SAME_LEVEL();
}

int	dm_sofree_c::dmRule_NOWEAPRELOAD(void)
{
	return orig->dmRule_NOWEAPRELOAD();
}

int	dm_sofree_c::dmRule_TEAMPLAY(void)
{
	return orig->dmRule_TEAMPLAY();
}

int	dm_sofree_c::dmRule_NO_FRIENDLY_FIRE(void)
{
	return orig->dmRule_NO_FRIENDLY_FIRE();
}

int	dm_sofree_c::dmRule_FORCE_RESPAWN(void)
{
	return orig->dmRule_FORCE_RESPAWN();
}

int	dm_sofree_c::dmRule_NO_ARMOR(void)
{
	return orig->dmRule_NO_ARMOR();
}

int	dm_sofree_c::dmRule_INFINITE_AMMO(void)
{
	return orig->dmRule_INFINITE_AMMO();
}

int	dm_sofree_c::dmRule_SPINNINGPICKUPS(void)
{
	return orig->dmRule_SPINNINGPICKUPS();
}

int	dm_sofree_c::dmRule_BULLET_WPNS_ONLY(void)
{
	return orig->dmRule_BULLET_WPNS_ONLY();
}
