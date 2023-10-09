#include "common.h"

// 32kb
// char floadfilebuffer[32768];
// sofplus uses 16,17,18
// i try to preserve sofreeS as using 7.. in a levelInit hook earlier than spawnentities after whcih sofplus uses on map_begin
int valid_ID[97] = {7,15,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,56,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,196,197,198,199,201,202,204,209,240,245,249,250,254,255};
int next_available_ID = 0;
/*
0 arguments
2 optional arguments
substring
verbosity

Iterates stringpackage configstrings.
*/
void spackage_list(void)
{
	int c = orig_Cmd_Argc() - 1;
	if ( !strcmp(orig_Cmd_Argv(1) ,"-h" ) ) {
		SOFPPNIX_PRINT(
			"Lists all registered string packages\n"
			"----------------------------\n"
			"takes optional argument as a search for substring\n"
			"and another optional argument for verbosity\n"
			"fills _++nix_spackage_X with the results\n"
			"eg. ++nix_spackage_list general verbose\n"
		);
		return;
	}
	if ( c > 2 ) {
		SOFPPNIX_PRINT("++nix_spackage_list -h");
		return;
	}

	char * verbose = NULL;
	char * substring = NULL;
	for ( int i = 1; i <= c; i++ ) {
		if ( !strcmp(orig_Cmd_Argv(i),"verbose") ) {
			verbose = orig_Cmd_Argv(i);
		} else if ( substring == NULL ) {
			substring = orig_Cmd_Argv(i);
		}
	}
	SOFPPNIX_DEBUG("verbose %s",verbose);
	SOFPPNIX_DEBUG("substring %s",substring);

	int i;
	cvar_t * savecvar = NULL;
	int count = 0;
	char fileline[256];
	for (i=1 ; i<MAX_STRING_PACKAGES ; i++) {
		if ( *(unsigned char*)(SV_CONFIGSTRINGS+(CS_STRING_PACKAGES+i)*MAX_QPATH) )
		{
			char line[65];
			// sprintf(line,"%s",(SV_CONFIGSTRINGS+(CS_SOUNDS+i)*MAX_QPATH));
			strcpy(line,SV_CONFIGSTRINGS+(CS_STRING_PACKAGES+i)*MAX_QPATH);
			
			// Only show ones that match the substring
			if ( substring != NULL ) {
				if ( strcasestr(std::string(line),std::string( substring ) ) ) {
					if ( verbose ) 
						SOFPPNIX_PRINT("%s",line);

					char tmpname[64];
					sprintf(tmpname,"%s%i","_++nix_spackage_",count+1);
					savecvar = orig_Cvar_Get( tmpname,"",0,NULL);
					setCvarString(savecvar,line);
					count++;
				}
			} else {
				// Show EVERY one
				// no arguments supplied
				// get the first 2 lines of the file?
				if ( verbose ) {
					char fname[64];
					sprintf(fname,"strip/%s.sp",line);
					unsigned char * filedata;

					SOFPPNIX_DEBUG("fname %s",fname);
					// Does the string package exist?
					int ret = orig_FS_LoadFile(fname,&filedata,false);
					if ( ret != -1 ) {
						char * d = filedata;
						char * second_line = NULL;
						
						int count = 1;
						// it will break when d=='\n'
						while ( *d != '\n' &&  count < ret ) {
							d++;
							count++;
						}
						// d == '\n'
						if ( count < ret) {
							d++;
							count++;
							second_line = d;
						}
						while ( *d != '\n' && count < ret) {
							d++;
							count++;
						}
						*d=0x00;
						
						SOFPPNIX_PRINT("%s : %s",line,second_line);

						orig_Z_Free(filedata);
					}
				}
				
				char tmpname[64];
				sprintf(tmpname,"%s%i","_++nix_spackage_",i);
				savecvar = orig_Cvar_Get( tmpname,"",0,NULL);
				setCvarString(savecvar,line);
				count++;
			}
			
		}
	}
	cvar_t * finalcount = orig_Cvar_Get( "_++nix_spackage_0","",0,NULL);
	setCvarInt(finalcount,count);
	
	// Cmd_CalcFreeImageSlots();
}
// there is a newline at the end of each string
// Use regex to extract `ID [0-9]\r\n` from line
void extractID(char * line) {
	// define the regex pattern, it should match 1+ numbers and 0+ \r or \n
	std::regex r("ID [0-9]+[\r\n]*");
	// define a string containing the text to search
	std::string s(line);
	// try to match the regular expression with the string
	std::smatch m;
	// if there is a match
	if (std::regex_search(s, m, r)) {
		// print the match
		std::cout << m.str() << std::endl;
	}
}

/*
Register a string package
1 argument - sp file name.sp

ensures ID in file is equal to next_available_ID.
*/
void spackage_register(char * one)
{

	std::string in_file_path = std::string("strip/") + one;
	if ( orig_FS_LoadFile(in_file_path.c_str(),NULL,false) == -1 ) {
		error_exit("++nix_spackage_register: file %s cannot be found",in_file_path.c_str());
		return;
	}
	in_file_path = orig_FS_Userdir() + std::string("/") +  in_file_path;
	// If the file does not exist inside USER. Treat normally.
	FILE * IN_FILE = fopen(in_file_path.c_str(),"r+");
	if ( !IN_FILE ) {
		char * shorten= strchr(one,'.');
		if ( shorten ) {
			*shorten = 0x00;
		}
		orig_SP_Register(one);
		return;

	} else {
		// Inside User dir, apply checksum and ID adjustment.
		char line[512];
		char * mybuffer = NULL;
		int line_count = 0;
		bool id_is_fine = false;
		
		char * continuous_buf = NULL;
		int written = 0;
		// READ
		while ( fgets(line,512,IN_FILE) ) {

			// define the regex pattern, it should match 1+ numbers and 0+ \r or \n
			std::regex r("ID ([0-9]+)[\r\n]*");
			std::string s(line);
			std::smatch m;
			if (std::regex_search(s, m, r)) {
				// print the match
				// std::cout << m.str() << std::endl;

				// extract the ID using match ()
				int id = std::stoi(m[1]);

				// if the ID is equal to the next available ID
				if ( id != valid_ID[next_available_ID] ) {
					// Not equal, requires change.
					// adjust the ID
					snprintf(line,sizeof(line),"ID %i\n",valid_ID[next_available_ID]);
				}
			}
			int now = strlen(line)+1;
			continuous_buf = fast_realloc(continuous_buf,written + now);
			// full line is copied
			// where to start writing to. should be == written. because its written_pos + 1

			// SOFPPNIX_DEBUG("Writing %s to %p",line,continuous_buf+written);
			strlcpy(continuous_buf+written,line,now);
			// null character is ignored. and overwritten next iter
			written += now-1;

			// mybuffer is set of 512 bytes, each with a null string in it.
			mybuffer = fast_realloc(mybuffer,512*(line_count+1));
			strlcpy(mybuffer + (line_count*512),line,512);
			line_count++;
		}
		// restore a null character.
		continuous_buf[written] = 0x00;
		fclose(IN_FILE);
		/*
			TODO: Generate checksum for this file contents
			Then 
		*/

		std::string crc_file;
		// Don't include null character in checksum.
		crc_checksum(continuous_buf,crc_file,written-1);
		if (continuous_buf)
			free(continuous_buf);
		// $USER/strip/origname-DEADBEEF.sp

		// remove .sp from one
		char * shorten= strchr(one,'.');
		if ( shorten ) {
			*shorten = 0x00;
		}
		std::string registerFileName = std::string(one) + "-" + crc_file;

		std::string new_file_path = "strip/" + registerFileName + ".sp";
		// loadfile does not need userdir.
		if ( orig_FS_LoadFile(new_file_path.c_str(),NULL,false) == -1 ) {
			new_file_path = orig_FS_Userdir() + std::string("/") +  new_file_path;
			create_file_dir_if_not_exists(new_file_path.c_str());
			// create it
			SOFPPNIX_DEBUG("Creating %s",new_file_path.c_str());
			FILE * newfile = fopen(new_file_path.c_str(),"w");
			if ( newfile ) {
				int totalbytes = 0;
				for ( int i=0;i<line_count; i++ ){
					totalbytes += strlen(mybuffer + (i*512));
					fputs(mybuffer + (i*512),newfile);
				}
				fclose(newfile);
				SOFPPNIX_DEBUG("Created %s",new_file_path.c_str());
			}
		}
		// SOFPPNIX_DEBUG("Registering %s",new_file_path.c_str());

		orig_SP_Register(registerFileName.c_str());

		next_available_ID++;
		if ( mybuffer )
			free(mybuffer);
	}
	
}

/*
print_ref ent SOFREE CHEER extra_Args...
4+ arguments - 
	ent/0
	whichFile
	whichString
	WhatPrint
	ExtraArgs

*/
void spackage_print_ref(edict_t * ent, char * file_ref, char * string_ref, ...)
{
	char temp[64];
	sprintf(temp,"%s_%s",file_ref,string_ref);
	
	short package_id = orig_SP_GetStringID(temp);

	if ( package_id == -1 )
		error_exit("Error ++nix_spackage_print_ref : cant find the string refs %s %s",file_ref,string_ref);
	
	// SOFPPNIX_DEBUG("package_id %i",package_id);

	// unformmated.
	char * cfmt = stget(orig_SP_GetStringText(package_id),0x0C);
	// std::cout << test;
	std::string sfmt(cfmt);

	std::vector<std::string> inputs;
	va_list args;
	va_start(args, NULL);
	char* arg = va_arg(args, char*);
	while (arg != NULL)
	{
		inputs.push_back(std::string(arg));
		// SOFPPNIX_DEBUG("input %s", inputs.back().c_str());
		arg = va_arg(args, char*);
	}
	SP_PRINT_MULTI(ent,package_id,sfmt,inputs);
}

//typedef void (*SP_Print)(edict_t *ent, unsigned short ID, ...);
// who,num
void spackage_print_id(edict_t * ent, unsigned char file_id,unsigned char string_index, ...)
{
	unsigned short package_id = file_id << 8;
	package_id |= string_index;
	
	// unformmated.
	char * cfmt = stget(orig_SP_GetStringText(package_id),0x0C);
	std::string sfmt(cfmt);

	std::vector<std::string> inputs;
	va_list args;
	va_start(args, NULL);
	char* arg = va_arg(args, char*);
	while (arg != NULL)
	{
		inputs.push_back(std::string(arg));
		// SOFPPNIX_DEBUG("input %s", inputs.back().c_str());
		arg = va_arg(args, char*);
	}
	SP_PRINT_MULTI(ent,package_id,sfmt,inputs);;
}