#include "common.h"

cvar_t * maxclients = NULL;
cvar_t * sv_public = NULL;
cvar_t * gamespyport = NULL;
cvar_t * hostport = NULL;
cvar_t * hostname = NULL;
cvar_t * deathmatch = NULL;
cvar_t * mapname = NULL;
cvar_t * sv_violence = NULL;
cvar_t * timelimit = NULL;
cvar_t * fraglimit = NULL;
cvar_t * dmflags = NULL;
cvar_t * g_movescale = NULL;
cvar_t * cheats = NULL;
cvar_t * ctf_loops = NULL;
cvar_t * sv_suicidepenalty = NULL;

cvar_t * _nix_deathmatch = NULL;


/*
	If the cvar already exists, the value will not be set.
	These are soft values, if not set already.
	So any other source that sets the cvar, will have priority.
*/
void CreateCvars(void)
{
	//									name					value	flags										modifiedCallback

	// Already Existing
	sv_public = orig_Cvar_Get(			"public", 				"0", 	NULL, 											NULL);
	
	gamespyport = orig_Cvar_Get(		"gamespyport", 			"0", 	CVAR_NOSET, 									NULL);
	hostport = orig_Cvar_Get(			"hostport", 			"0", 	CVAR_NOSET, 									NULL);

	dmflags = orig_Cvar_Get(			"dmflags", 				"0", 	CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 	NULL);
	maxclients = orig_Cvar_Get(			"maxclients", 			"16", 	CVAR_SERVERINFO | CVAR_LATCH , 					NULL);
	hostname = orig_Cvar_Get(			"hostname", 			"", 	CVAR_SERVERINFO | CVAR_ARCHIVE, 				NULL);
	timelimit = orig_Cvar_Get(			"timelimit", 			"0", 	CVAR_SERVERINFO | CVAR_ARCHIVE, 				NULL);
	fraglimit = orig_Cvar_Get(			"fraglimit", 			"0", 	CVAR_SERVERINFO | CVAR_ARCHIVE, 				NULL);
	g_movescale = orig_Cvar_Get(		"g_movescale", 			"1", 	CVAR_SERVERINFO | CVAR_ARCHIVE, 				NULL);
	ctf_loops = orig_Cvar_Get(			"ctf_loops", 			"3", 	CVAR_SERVERINFO | CVAR_ARCHIVE, 				NULL);
	sv_suicidepenalty = orig_Cvar_Get(	"sv_suicidepenalty", 	"2", 	CVAR_SERVERINFO | CVAR_ARCHIVE, 				NULL);



	cheats = orig_Cvar_Get(				"cheats", 				"0", 	CVAR_SERVERINFO, 								NULL);
	sv_violence = orig_Cvar_Get(		"sv_violence",			"0",	CVAR_SERVERINFO,								NULL);

	deathmatch = orig_Cvar_Get(			"deathmatch", 			"0", 	CVAR_ARCHIVE, 									NULL);
	mapname = orig_Cvar_Get(			"mapname",				"",		CVAR_NOSET,										NULL);

	_nix_deathmatch = orig_Cvar_Get( "_++nix_deathmatch","0",NULL,NULL);
}