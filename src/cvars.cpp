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

cvar_t * _nix_version = NULL;
cvar_t * _nix_violence = NULL;
cvar_t ** p_nix_violence = &_nix_violence;


/*
	If the cvar already exists, the value will not be set.
	These are soft values, if not set already.
	So any other source that sets the cvar, will have priority.
*/
void CreateCvars(void)
{

	_nix_version = orig_Cvar_Get(		"_++nix_version", 		"0", 	NULL, 									NULL);


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
	sv_violence = orig_Cvar_Get(		"sv_violence",			"63",	CVAR_SERVERINFO,								NULL);

	deathmatch = orig_Cvar_Get(			"deathmatch", 			"0", 	CVAR_ARCHIVE, 									NULL);
	mapname = orig_Cvar_Get(			"mapname",				"",		CVAR_NOSET,										NULL);

	_nix_deathmatch = orig_Cvar_Get( "_++nix_deathmatch","0",NULL,NULL);

	*p_nix_violence = orig_Cvar_Get( "_++nix_violence","63",NULL,NULL);

	// repond pretending to be low violence.
    int offset = (int)&p_nix_violence - 0x829CCC8;
	SOFPPNIX_DEBUG("%d - %d",(int)&p_nix_violence,0x829CCC8);
	memoryUnprotect(0x080A9AA5);
	*(int*)0x080A9AA5 = offset;
	memoryProtect(0x080A9AA5);

}

cvar_t * findCvar(char * cvarname)
{
	cvar_t	*var;
	cvar_t * cvar_vars = *(unsigned int*)0x083FFD64;
	
	for (var=cvar_vars ; var ; var=var->next)
		if (!strcmp (cvarname, var->name))
			return var;
	return NULL;
}

/*
	Fine to call "Zone" Free. Because CopyString uses zones.
*/
void setCvarUnsignedInt(cvar_t * which,unsigned int val){
	which->modified = true;
	orig_Z_Free(which->string);
	char intstring[64];
	which->value = (float)val;
	sprintf(intstring,"%u",val);
	which->string = orig_CopyString(intstring);
}


void setCvarInt(cvar_t * which,int val){
	which->modified = true;
	orig_Z_Free(which->string);
	char intstring[64];
	which->value = (float)val;
	sprintf(intstring,"%d",val);
	which->string = orig_CopyString(intstring);
}

void setCvarByte(cvar_t * which, unsigned char val) {
	which->modified = true;
	orig_Z_Free(which->string);
	char bytestring[64];
	sprintf(bytestring,"%hhu",val);
	which->value = atof(bytestring);
	which->string = orig_CopyString(bytestring);
}


void setCvarFloat(cvar_t * which, float val) {

	which->modified = true;
	orig_Z_Free(which->string);
	char floatstring[64];
	sprintf(floatstring,"%f",val);
	which->string = orig_CopyString(floatstring);
	which->value = val;
}

void setCvarString(cvar_t * which, char * newstr) {

	which->modified = true;
	orig_Z_Free(which->string);
	
	
	which->string = orig_CopyString(newstr);
	which->value = atof(which->string);
}