#ifndef _P_BODY_H

#define _P_BODY_H


void PB_KillBody(edict_t &ent);
bool PB_InitBody(edict_t &ent,char *userinfo);

void PB_GhoulUpdate(edict_t &ent);

void PB_PlaySequenceForDeath(edict_t *ent, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point, int dflags);
void PB_PlaySequenceForPain(edict_t *ent, vec3_t point, vec3_t dest=vec3_origin, vec3_t face=vec3_origin);

void PB_PlaySequenceForMovement(edict_t *ent, vec3_t dest, vec3_t face, vec3_t org, vec3_t ang);
void PB_PlaySequenceForStand(edict_t *ent, vec3_t dest=vec3_origin, vec3_t face=vec3_origin);
void PB_PlaySequenceForJump(edict_t *ent, vec3_t dest, vec3_t face);
void PB_PlaySequenceForAttack(edict_t *ent, vec3_t dest, vec3_t face, edict_t *target);
void PB_PlaySequenceForReload(edict_t *ent, vec3_t dest, vec3_t face);
void PB_PlaySequenceForItemUse(edict_t *ent, vec3_t dest, vec3_t face, int itemType);
void PB_PlaySequenceForGesture(edict_t *ent, int gestNum);

void PB_GetActualSkinName(edict_t *ent, char *putSkinHere);
void PB_GetActualTeamName(edict_t *ent, char *putTeamHere);

void UpdatePlayerFace(edict_t *ent);
void UpdatePlayerWeapon(edict_t *ent);

void PB_Killed(edict_t *ent);
int PB_Damage (edict_t *targ, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t origin, int damage, int knockback, int dflags, int mod, float penetrate, float absorb);
int PB_AddArmor(edict_t *ent, int amount);

void PB_MakeCorpse (edict_t *ent);

void PB_PrecacheAllPlayerModels(void);

void PB_RepairSkin(edict_t *ent, int damagedHealth);
void PB_AddFlag(edict_t *ent, char *FlagSkinName, char* FlagGhoulFile);
void PB_RemoveFlag(edict_t *ent);
#endif