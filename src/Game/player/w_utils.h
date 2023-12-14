#ifndef __W_UTILS_H_
#define __W_UTILS_H_

#include "w_weapons.h"

void SetWSkin(sharedEdict_t *self, char *matName, char *skinName);
void SetVWeapGhoul(sharedEdict_t *self, char *modelName);
void ReadyVWeapGhoulServer(sharedEdict_t *self, char *modelName);
void SetReadiedVWeapGhoulServer(sharedEdict_t *self, char *modelName);
void RunVWeapAnim(sharedEdict_t *self, char *seqName, IGhoulInst::EndCondition loopType,float timeOffset=0);
void RunReadiedVWeapAnim(sharedEdict_t *self, char *seqName, IGhoulInst::EndCondition loopType);
int ClientServerRand(int mn,int mx);

#endif // __W_UTILS_H_