#include "common.h"
std::unordered_map<std::string,PyObject*> py_page_draw_routines;
std::string current_page[32];

std::array<char [1024],32> page_layout;
std::array<char [1024],32> hud_layout;
std::array<int,32> strip_layout_size = {0};

void page_none(edict_t * ent, edict_t * killer);
void page_scoreboard(edict_t * ent, edict_t * killer);
void page_chat(edict_t * ent, edict_t * killer);




/*
	Different views of scoreboard.
	When scoreboard is toggled, it can show alternate views with a timeout.
	Perhaps sticky option can be used.
	eg.
		.stats * == show stats sticky
		.players * == show country code
		.chat * == show chat history



	If any the layout buffer changes, a refresh of screen is required, if want to see the changes instantly.
	Else has to wait until natural refresh occurs. ( toggle of scoreboard ).

	There is a draw function for each page. They only are called on 'refresh' frames.

	The layout data is emptied every server frame, so its built up every frame too. This makes sense because the data is dynamic. Perhaps one could separate fixed data from dynamic data to save some processing.

	Or it could be one function that is called after the draw, instead of every frame. But then the data is not fed live information, could only work for the static part.  Idea of discarding what you've just drawn because its stale.

	The bold test was solved by using a sb_client string offscreen, it enables antialiased text client-side.

	The buffers are cleared after each 'refresh'. And also drawn each 'refresh' technically, too.

	Thus draw routines should only exist inside the draw hooks which are called in sync with the refreshes.

*/

void showScoreboard(edict_t * ent, unsigned int slot, int showscores,edict_t * killer=NULL, qboolean logfile = 0)
{
	
	if ( showscores) {
		// Does the routine exist in the map, is it registered by python?
		// Draw the page that is currently set, with timeout.
		// If sticky set, make permanant.
		if ( py_page_draw_routines.count(current_page[slot]) ) {
			// Draw.
			PyObject * who = createEntDict(ent);
			if ( who == NULL ) {
				error_exit("killer pointer invalid");
			}
			PyObject * who_killer = createEntDict(killer);
			if ( who_killer == NULL ) {
				error_exit("killer pointer invalid");
			} 

			PyObject* result = PyObject_CallFunction(py_page_draw_routines[current_page[slot]],"OO",who,who_killer);
			// returns None
			Py_XDECREF(result);

			Py_XDECREF(who);
			Py_XDECREF(who_killer);
		} else {
			currentGameMode->clientScoreboardMessage(ent,killer,0);
		}
	}
	// SOFPPNIX_DEBUG("%s",strip_layouts[slot]);
	

}
bool page_should_refresh[32] = {false};
/*
	called by:
		my_Cmd_Say_f
		py_player_refresh_layout
		clientConnect
*/
void refreshScreen(edict_t * ent)
{
	if ( ent == NULL ) {
		for ( int i = 0 ; i < maxclients->value;i++ ) {
			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;

			page_should_refresh[i] = true;
		}
		return;
	}
	int slot = slot_from_ent(ent);
	void * client = getClientX(slot);
	int state = *(int*)(client);
	if (state != cs_spawned )
		return;
	
	page_should_refresh[slot] = true;
}
/*
	ctf_team_sb enables anti-aliasing.
*/
void empty_reset_hud_layout(int slot)
{

	hud_layout[slot][0] = 0x00;
	sprintf(hud_layout[slot],"ctf_team_sb 0 -4000 0 0 0 0 xr %i yb -16 string \"\x07%s\" ",0 - (sofreebuild_len*8+8),sofreebuildstring);
	// sprintf(hud_layout[slot],"xr %i yb -16 string \"\x07%s\" ",0 - (sofreebuild_len*8+8),sofreebuildstring);
	strip_layout_size[slot] = strlen(hud_layout[slot]) + strlen(page_layout[slot]);

	// SOFPPNIX_DEBUG("Clearing hud layout for slot %i. lengths = %i + %i = %i",slot,strlen(hud_layout[slot]),strlen(page_layout[slot]),strip_layout_size[slot]);

}
void empty_reset_page_layout(int slot)
{
	page_layout[slot][0] = 0x00;

	strip_layout_size[slot] = strlen(hud_layout[slot]) + strlen(page_layout[slot]);
}

/*
	0 arguments
	clears screen. ( Excluding scoreboard ).

	Puts a watermark on the screen.

	Intention: Clear without remove watermark.

	dm->checkEvents - NotActive.
	clientScorebordMessage - Clear if toggle off.
	dm->clientEndFrame - overrides G_SetStats. To force Layout on.

	called by:
		xxxx showScoreboard (removed) (no longer true).xxxx

		1)clientConnect
		2)SV_RunGameFrame (early).
		3)py_player_clear_layout
	LATEST:
		I only want this called after a resfresh occured.

*/
void layout_clear(LayoutMode mode,edict_t * ent)
{
	if ( ent == NULL ) {
		// SOFPPNIX_DEBUG("Clearing screen");

		for ( int i = 0 ; i < maxclients->value;i++ ) {
			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;

			switch (mode) {
				case hud:
					empty_reset_hud_layout(i);
					break;
				case page:
					empty_reset_page_layout(i);
					break;
				default:
					error_exit("Bad layout mode");
			}
		}

		// orig_Com_Printf("Layoutstring is : %s\n",layoutstring);

		return;
	}
	int i = slot_from_ent(ent);
	// SOFPPNIX_DEBUG("Reset for slot %i",i);
	void * client = getClientX(i);
	int state = *(int*)(client);
	if (state != cs_spawned )
		return;

	switch (mode) {
		case hud:
			empty_reset_hud_layout(i);
			break;
		case page:
			empty_reset_page_layout(i);
			break;
		default:
			error_exit("Bad layout mode");
	}

}

static void append_to_buffer(int slot,LayoutMode mode, char * newstring)
{
	// SOFPPNIX_DEBUG("pages.cpp @Append_to_buffer : slot %i",slot);
	char * append_to;
	char * count_this;
	int * incr_this;
	switch (mode) {
		case hud:
			append_to = hud_layout[slot];
			count_this = page_layout[slot];
			break;
		case page:
			append_to = page_layout[slot];
			count_this = hud_layout[slot];
			break;
		default:
			error_exit("Bad layout mode");
	}

#if 0
	char * tmp = strdup(append_to);
	int req_size = snprintf( append_to,sizeof(hud_layout),"%s%s",tmp,newstring);
	
	if ( req_size < sizeof(hud_layout) ) {
		// Fits.
		strip_layout_size[slot] += req_size;
	} else {
		SOFPPNIX_PRINT("Warning some drawing was truncated.");
		// total is now 1024.
		strip_layout_size[slot] = strlen(count_this) + 1024; 
	}
#else
	int new_item_len = strlen(newstring);
	int freespace = 1023-strlen(append_to);
	if ( freespace < 0 ) freespace = 0;
	strncat(append_to,newstring,freespace);
	// SOFPPNIX_DEBUG("buffer now contains\n%s",append_to);
	if ( new_item_len > freespace ) {
		// SOFPPNIX_PRINT("Warning some drawing was truncated.");
		// total is now 1024.
		strip_layout_size[slot] = strlen(count_this) + 1023 + 1;
	} else
	{
		// SOFPPNIX_DEBUG("Added %i bytes to %i buffer",new_item_len,slot);
		// Fits.
		strip_layout_size[slot] += new_item_len;
	}
#endif
}
void append_layout_image(LayoutMode mode,edict_t * ent,int offsetx, int offsety, char * img_path)
{

	char newstring[256];
	snprintf(newstring,256,"xv %i yv %i picn \"%s\" ",offsetx+160,offsety+120,img_path);
	
	// broadcast
	if ( ent == NULL ) {
		for ( int i = 0 ; i < maxclients->value;i++ ) {

			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;


			append_to_buffer(i,mode,newstring);
		}
		return;
	}

	// individual client
	int i = slot_from_ent(ent);
	void * client = getClientX(i);
	int state = *(int*)(client);
	if (state != cs_spawned )
		return;
	append_to_buffer(i,mode,newstring);
}

/*
Appends to strip_layouts.
Calls RefreshScreen. ( Redraws Scoreboard ).
yb yBottom 0 = bottom of screen, up=negative, down=positive
yt yTop 0 = top of screen, up=negative, down=positive
yv centered

xl xLeft
xr xRight
xv centered

3 arguments - x y msg
*/
void append_layout_string(LayoutMode mode,edict_t * ent,int offsetx, int offsety, char * message)
{

#if 0
	int len = strlen(message);
	if ( gray ) {
		for (int i = 0; i < len; i++ ) {
			*(message+i) = *(message+i) | 0x80;
		}
	}
#endif
	char newstring[256];
	snprintf(newstring,256,"xv %i yv %i string \"%s\" ",offsetx+160,offsety+112,message);
	#if 0
	if ( ! gray ) {
		snprintf(newstring,256,"xv %i yv %i string \"%s\" ",offsetx+160,offsety+112,message);
	} else {
		snprintf(newstring,256,"xv %i yv %i altstring \"%s\" ",offsetx+160,offsety+112,message);
	}
	#endif

	// broadcast
	if ( ent == NULL ) {

		for ( int i = 0 ; i < maxclients->value;i++ ) {
			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;

			append_to_buffer(i,mode,newstring);
		}
		
		return;
	}

	int i = slot_from_ent(ent);
	void * client = getClientX(i);
	int state = *(int*)(client);
	if (state != cs_spawned )
		return;
	append_to_buffer(i,mode,newstring);
}
/*
more direct version of above.
//xl = xpos from left
//xr = xpos from right
//xv = some standardized center -160
//yt = ypos from top
//yb = ypos from bottom
//yv = some standardized center -120
//picn = a picture
//client_sb
//client_ctf_sb
//spect_sb
//team_sb
//client_conq_sb
//ctf_team_sb
//control_team_sb
//string
//altstring
//tc
//ac
*/
void append_layout_direct(LayoutMode mode,edict_t * ent,char * message)
{
	int len = strlen(message);

	char newstring[256];
	snprintf(newstring,256,"%s ",message);

	if ( ent == NULL ) {
		for ( int i = 0 ; i < maxclients->value;i++ ) {
			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;
			// hexdump(newstring,newstring+strlen(newstring));
			append_to_buffer(i,mode,newstring);
		}
		
		return;
	}
	int i = slot_from_ent(ent);
	void * client = getClientX(i);
	int state = *(int*)(client);
	if (state != cs_spawned )
		return;

	// hexdump(newstring,newstring+strlen(newstring));
	append_to_buffer(i,mode,newstring);
}

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
