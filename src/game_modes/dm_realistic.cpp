#include "common.h"

float dm_realistic_c::clientGetMovescale(edict_t *ent)
{ 
	// orig_Com_Printf("Client Movescale has been CHANGED!!!\n");
	// Base move minus any reduction for being crippled.
	return(g_movescale->value-(g_movescale->value*ent->client->moveScale));
}