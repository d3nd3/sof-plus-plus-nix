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

cvar_t * _nix_slidefix = NULL;
cvar_t * _nix_obit_timeout = NULL;
cvar_t * _nix_obit_len_max = NULL;
cvar_t * _nix_fall_dmg_scale = NULL;
cvar_t * _nix_public_rcon = NULL;
cvar_t * _nix_serverside_demos = NULL;

cvar_t * _nix_snd_no_feet = NULL;
cvar_t * _nix_snd_no_jump = NULL;
cvar_t * _nix_snd_no_weap_touch = NULL;

cvar_t * _nix_extra_shotgun_gore = NULL;

cvar_t * _nix_py_killfeed = NULL;
cvar_t * _nix_py_pages = NULL;


void slidefix_modified(cvar_t * c)
{
	if ( c->value == 1.0f ){
		SOFPPNIX_PRINT("Enabling slidefix feature");
		#if 0
		callE8Patch(0x0812E643,&my_PM_StepSlideMove);
		#else
		memoryAdjust(0x0812E83A,1,0xEB);
		#endif
	}
	else {
		//RESTORE.
		SOFPPNIX_PRINT("Disabling slidefix feature");
		#if 0
		char * p = 0x0812E643;
		memoryUnprotect(p);
		*p = 0xE8;
		*(p+1) = 0x74;
		*(p+2) = 0xF3;
		*(p+3) = 0xFF;
		*(p+4) = 0xFF;
		memoryProtect(p);
		#else
		memoryAdjust(0x0812E83A,1,0x72);
		#endif
	}
}

void public_rcon_modified(cvar_t * c)
{
	if ( c->value == 1.0f ) {
		SOFPPNIX_PRINT("Enabling public rcon feature");
		//---------------------RCON PUBLIC PASS COMMAND FILTER--------------------------------
		callE8Patch(0x080AB1A4,&public_rcon_command);
		memoryAdjust(0x080AAFC9,1,0xEB);
		memoryAdjust(0x080AAF9F,1,0xEB);
		memoryAdjust(0x080AB0B8,2,NOP);
		memoryAdjust(0x080AB0D6,1,0xEB);
	}
	else {
		//RESTORE.
		SOFPPNIX_PRINT("Disabling public rcon feature");
		char * p = 0x080AB1A4;
		memoryUnprotect(p);
		*p = 0xE8;
		*(p+1) = 0xDF;
		*(p+2) = 0xD1;
		*(p+3) = 0x06;
		*(p+4) = 0x00;
		memoryProtect(p);

		memoryAdjust(0x080AAFC9,1,0x75);
		memoryAdjust(0x080AAF9F,1,0x75);

		memoryAdjust(0x080AB0B8,1,0x74);
		memoryAdjust(0x080AB0B9,1,0x1E);

		memoryAdjust(0x080AB0D6,1,0x74);
	}
	
}

void serverside_demos_modified(cvar_t * c)
{
	if ( c->value == 1.0f ) {
		SOFPPNIX_PRINT("Enabling serverside demos feature");
		//---------------------------ALLOW CONNECT TO ATTRACTLOOP---------------------------------
			memoryAdjust(0x080AA860,1,0xEB);

			memoryAdjust(0x080AFC6E,1,0x90);
			memoryAdjust(0x080AFC6F,1,0xE9);


		//---------------------------NETCHAN PLAYBACK / RECORD--------------------------
			callE8Patch(0x080AFBF4,&my_Netchan_Transmit_Save); //Outgoing
			callE8Patch(0x080AFDD8,&my_Netchan_Transmit_Playback); //DemoPlayback.

		//---------------------------PATCH NETCHAN_TRANSMIT (seperate rel buffer)---------------------------
			memoryAdjust(0x0812B05A,1,0x55);
			callE8Patch(0x0812B05B,&my_Netchan_Patch);
			memoryAdjust(0x0812B060,1,0x83);
			memoryAdjust(0x0812B061,1,0xc4);
			memoryAdjust(0x0812B062,1,0x4);//cleanup stack
			memoryUnprotect(0x0812B063);
			unsigned char * p = 0x0812B063;
			p[0] = 0xE9;
			*(int*)(p+1) = 0x0812B090 - (int)0x0812B063 - 5;
			memoryProtect(0x0812B063);
	}
	else
	{
		//RESTORE.
		SOFPPNIX_PRINT("Disabling serverside demos feature");

		//---------------------------ALLOW CONNECT TO ATTRACTLOOP---------------------------------
		memoryAdjust(0x080AA860,1,0x74);

		memoryAdjust(0x080AFC6E,1,0x0F);
		memoryAdjust(0x080AFC6F,1,0x85);

		//---------------------------NETCHAN PLAYBACK / RECORD--------------------------
		char * p = 0x080AFBF4;
		memoryUnprotect(p);
		*p = 0xE8;
		*(p+1) = 0xD3;
		*(p+2) = 0xB3;
		*(p+3) = 0x07;
		*(p+4) = 0x00;
		memoryProtect(p);

		//---------------------------PATCH NETCHAN_TRANSMIT (seperate rel buffer)---------------------------
		memoryAdjust(0x0812B05A,1,0x85);
		p = 0x0812B05B;
		memoryUnprotect(p);
		*p = 0xC0;
		*(p+1) = 0x75;
		*(p+2) = 0x32;
		*(p+3) = 0x8B;
		*(p+4) = 0x4D;
		memoryProtect(p);

		p = 0x0812B060;
		memoryUnprotect(p);
		*p = 0x54;
		*(p+1) = 0x85;
		*(p+2) = 0xC9;
		memoryProtect(p);

		p = 0x0812B063;
		memoryUnprotect(p);
		*p = 0x74;
		*(p+1) = 0x2B;
		*(p+2) = 0x8D;
		*(p+3) = 0x95;
		*(p+4) = 0x50;
		memoryProtect(p);

	}
}


/*
	If the cvar already exists, the value will not be set.
	These are soft values, if not set already.
	So any other source that sets the cvar, will have priority.
*/
void createServerCvars(void)
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
		sv_violence = orig_Cvar_Get(		"sv_violence",			"63",	CVAR_SERVERINFO,								NULL);

		deathmatch = orig_Cvar_Get(			"deathmatch", 			"0", 	CVAR_ARCHIVE, 									NULL);
		mapname = orig_Cvar_Get(			"mapname",				"",		CVAR_NOSET,										NULL);

		_nix_deathmatch = orig_Cvar_Get( "_++nix_deathmatch","0",NULL,NULL);

		*p_nix_violence = orig_Cvar_Get( "_++nix_violence","63",NULL,NULL);

		//slidefix
		_nix_slidefix = orig_Cvar_Get( "_++nix_slidefix","1",NULL,slidefix_modified);

		

		//fall damage.
		_nix_fall_dmg_scale = orig_Cvar_Get("_++nix_fall_dmg_scale","1",NULL,NULL);

		//shotgun gore.
		_nix_extra_shotgun_gore = orig_Cvar_Get("_++nix_extra_shotgun_gore","0",NULL,NULL);

		//rcon-access
		_nix_public_rcon = orig_Cvar_Get("_++nix_public_rcon","0",NULL,public_rcon_modified);

		_nix_serverside_demos = orig_Cvar_Get("_++nix_serverside_demos","0",NULL,serverside_demos_modified);

		//disable sounds
		_nix_snd_no_feet = orig_Cvar_Get("_++nix_snd_no_feet","0",NULL,NULL);
		_nix_snd_no_jump = orig_Cvar_Get("_++nix_snd_no_jump","0",NULL,NULL);
		_nix_snd_no_weap_touch = orig_Cvar_Get("_++nix_snd_no_weap_touch","0",NULL,NULL);


		#ifdef USE_PYTHON
			_nix_py_killfeed = orig_Cvar_Get("_++nix_py_killfeed","0",NULL,NULL);
			//related to kill-feed.
			_nix_obit_timeout = orig_Cvar_Get("_++nix_obit_timeout","10",NULL,NULL);
			_nix_obit_len_max = orig_Cvar_Get("_++nix_obit_len_max","6",NULL,NULL);


			_nix_py_pages = orig_Cvar_Get("_++nix_py_pages","0",NULL,NULL);
		#endif


		// repond pretending to be low violence.
	    int offset = (int)&p_nix_violence - 0x829CCC8;

		memoryUnprotect(0x080A9AA5);
		*(int*)0x080A9AA5 = offset;
		memoryProtect(0x080A9AA5);
}
void createClientCvars(void)
{
	//									name					value	flags										modifiedCallback
}
void createSharedCvars(void)
{
	//									name					value	flags										modifiedCallback
	_nix_version = orig_Cvar_Get(		"_++nix_version", 		"0", 	NULL, 									NULL);
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
