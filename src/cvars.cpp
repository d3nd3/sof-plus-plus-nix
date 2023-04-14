#include "common.h"

cvar_t * maxclients = NULL;
cvar_t * sv_public = NULL;
cvar_t * gamespyport = NULL;
cvar_t * hostport = NULL;
void CreateCvars(void)
{
	// If the variable already exists, the value will not be set
	sv_public = orig_Cvar_Get("public", "0", NULL, NULL);
	maxclients = orig_Cvar_Get("maxclients", "16", CVAR_LATCH | CVAR_SERVERINFO, NULL);
	gamespyport = orig_Cvar_Get("gamespyport", "0", CVAR_NOSET, NULL);
	hostport = orig_Cvar_Get("hostport", "0", CVAR_NOSET, NULL);
}