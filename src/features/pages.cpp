#include "common.h"
std::unordered_map<std::string,void (*)(edict_t * ent, edict_t * killer)> pages;
std::string current_page[32];

void page_none(edict_t * ent, edict_t * killer);
void page_scoreboard(edict_t * ent, edict_t * killer);
void page_chat(edict_t * ent, edict_t * killer);

void init_pages(void)
{
	for ( int i = 0; i < 32 ; i ++ ) {
		current_page[i] = "scoreboard";
	}
	
	pages["scoreboard"] = page_scoreboard;
	pages["chat"] = page_chat;
}

/*
	Different views of scoreboard.
	When scoreboard is toggled, it can show alternate views with a timeout.
	Perhaps sticky option can be used.
	eg.
		.stats * == show stats sticky
		.players * == show country code
		.chat * == show chat history
*/

void showScoreboard(edict_t * ent, unsigned int slot, int showscores,edict_t * killer=NULL, qboolean logfile = 0)
{
	// void * gclient = stget(ent, EDICT_GCLIENT);
	// int show_scores = stget(gclient, GCLIENT_SHOWSCORES);
	// SOFPPNIX_DEBUG("Scoreboard :: Page %i , SHowscores %i, Prevscores %i",page[slot],showscores,prev_showscores[slot]);	

	if ( !showscores ) {
		page_none(ent,killer);
	} else {
	
		// Draw the page that is currently set, with timeout.
		// If sticky set, make permanant.
		if ( pages.count(current_page[slot]) ) {
			// Draw.
			pages[current_page[slot]](ent,killer);
		}

	}
	// will only be true
	prev_showscores[slot] = showscores;
}
bool page_should_refresh[32] = {false};
void refreshScreen(edict_t * ent)
{
	int slot = slot_from_ent(ent);
	page_should_refresh[slot] = true;

}
void append_watermark(int slot)
{
	strip_layouts[slot][0] = 0x00;
	sprintf(strip_layouts[slot],"xr %i yb -16 string \"%s\" ",0 - (sofreebuild_len*8+8),sofreebuildstring);
	strip_layout_size[slot] = strlen(strip_layouts[slot]);
}
void page_none(edict_t * ent, edict_t * killer)
{
	int slot = slot_from_ent(ent);
	//off
	orig_SP_Print(ent,0x0700,"*");
	orig_SP_Print(ent,0x0700,strip_layouts[slot]);
}
void page_scoreboard(edict_t * ent, edict_t * killer)
{
	SOFPPNIX_DEBUG("Drawing normal scoreboard");
	int slot = slot_from_ent(ent);
	//page1 - scoreboard
	// every 32 server frames = 3.2 seconds
	// Draw Official Scoreboard
	orig_SP_Print(ent,0x0700,"*");
	
	currentGameMode->clientScoreboardMessage(ent,killer,0);
	orig_SP_Print(ent,0x0700,strip_layouts[slot]);
}

void page_chat(edict_t * ent, edict_t * killer)
{
	int slot = slot_from_ent(ent);
	//page2 - chat
	orig_SP_Print(ent,0x0700,"*");
	orig_SP_Print(ent,0x0700,strip_layouts[slot]);

	int chatLines = 7;
	int gapY = 32;
	int widthX = 60;
	// 4 lines for console print, 16 pixel alignment away from crosshair
	int startY = -120+32+16;
	// int startY = 112-4*gapY-12 - 2*gapY;
	// startY+=4;
	// int startX = -160;
	int widthPixels = widthX*8;
	int startX = -160 + (640 - 512)*0.5;
	// startX+=4;
	// 80 characters per line.

	// 256x256 next to each other.
	char tmp[128];
	snprintf(tmp,sizeof(tmp),"xv %i yv %i picn %s",startX,startY,"c/c");
	orig_SP_Print(ent,0x0700,tmp);
	snprintf(tmp,sizeof(tmp),"xv %i yv %i picn %s",startX+256,startY,"c/c");
	orig_SP_Print(ent,0x0700,tmp);

	// horizontal - 512 background
	// vertical - 32 * 8 = 256

	// 24 pixel gap at bottom
	startY += 12;
	char chat_sp[256];
	// draw help here.
	snprintf(chat_sp,256,"xv %i yv %i string \"%s\"",startX,startY,".help .p=prev .r=restore .n=next VVV");
	orig_SP_Print(ent,0x700,chat_sp);

	startY += 32;
	// line len can't be greater than 64 or escapes backgrnd.
	// startX += (512 - line_len*8)*0.5;
	startX += 4; //border
	// Colored names consume space in buffer
	int startIndex = chatVectors.size() - chatLines;
	if (startIndex < 0 ) startIndex = 0;
	for (int i = startIndex; i < chatVectors.size(); ++i) {	
		// offsetx+157,offsety+114
		//23 byte overhead per line
		//8 lines.
		// (60 + 23) * 8 = 664
		// SOFPPNIX_DEBUG("Len = %i",strlen(chatVectors[i].c_str()));
		snprintf(chat_sp,256,"xv %i yv %i string \"%s\"",startX,startY,chatVectors[i].c_str());
		// SOFPPNIX_DEBUG("Chat == %s",chatVectors[i].c_str());
		orig_SP_Print(ent,0x700,chat_sp);
		startY += 32;
	}
}

/*
	0 arguments
	clears screen. ( Excluding scoreboard ).

	Puts a watermark on the screen.

	Intention: Clear without remove watermark.

	dm->checkEvents - NotActive.
	clientScorebordMessage - Clear if toggle off.
	dm->clientEndFrame - overrides G_SetStats. To force Layout on.
*/
void layout_clear(edict_t * ent)
{
	if ( ent == NULL ) {
		// SOFPPNIX_DEBUG("Clearing screen");

		for ( int i = 0 ; i < maxclients->value;i++ ) {
			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;

			append_watermark(i);

			edict_t * ent = stget(client,CLIENT_ENT);
			refreshScreen(ent);
		}

		// orig_Com_Printf("Layoutstring is : %s\n",layoutstring);

		return;
	}
	append_watermark(slot_from_ent(ent));
	refreshScreen(ent);
}

void append_layout_image(edict_t * ent,int offsetx, int offsety, char * img_path)
{
	char newstring[256];
	snprintf(newstring,256,"xv %i yv %i picn \"%s\" ",offsetx+160,offsety+120,img_path);
	
	int newlen = strlen(newstring);
	// broadcast
	if ( ent == NULL ) {
		for ( int i = 0 ; i < maxclients->value;i++ ) {
			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;

			if ( strip_layout_size[i] + newlen <= 1024 ) {
				// append.
				strcat(strip_layouts[i], newstring);
				strip_layout_size[i]+=newlen;
			} else {
				SOFPPNIX_PRINT("Cant draw this , run out of space");
			}
			edict_t * ent = stget(client,CLIENT_ENT);
			refreshScreen(ent);
		}
		return;
	}

	// individual client
	int i = ent->s.skinnum;
	if ( strip_layout_size[i] + newlen <= 1024 ) {
		// append.
		strcat(strip_layouts[i], newstring);
		strip_layout_size[i]+=newlen;
	} else {
		SOFPPNIX_PRINT("Cant draw this , run out of space");
	}
	refreshScreen(ent);
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
void append_layout_string(edict_t * ent,int offsetx, int offsety, char * message, qboolean gray)
{

	int len = strlen(message);
	if ( gray ) {
		for (int i = 0; i < len; i++ ) {
			*(message+i) = *(message+i) | 0x80;
		}
	}

	char newstring[256];
	if ( ! gray ) {
		snprintf(newstring,256,"xv %i yv %i string \"%s\" ",offsetx+160,offsety+112,message);
	} else {
		snprintf(newstring,256,"xv %i yv %i altstring \"%s\" ",offsetx+160,offsety+112,message);
	}
	
	int newlen = strlen(newstring);

	// broadcast
	if ( ent == NULL ) {

		for ( int i = 0 ; i < maxclients->value;i++ ) {
			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;

			if ( strip_layout_size[i] + newlen <= 1024 ) {
				strcat(strip_layouts[i], newstring);
				strip_layout_size[i]+=newlen;
			} else {
				SOFPPNIX_PRINT("Cant draw this , run out of space");
			}
			edict_t * ent = stget(client,CLIENT_ENT);
			refreshScreen(ent);
		}
		
		return;
	}

	int i = ent->s.skinnum;
	if ( strip_layout_size[i] + newlen <= 1024 ) {
		strcat(strip_layouts[i], newstring);
		strip_layout_size[i]+=newlen;
	} else {
		SOFPPNIX_PRINT("Cant draw this , run out of space");
	}
	refreshScreen(ent);
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
void append_layout_direct(edict_t * ent,char * message)
{
	int len = strlen(message);

	char newstring[256];
	snprintf(newstring,256,"%s ",message);
	int newlen = strlen(newstring);

	if ( ent == NULL ) {
		for ( int i = 0 ; i < maxclients->value;i++ ) {
			void * client = getClientX(i);
			int state = *(int*)(client);
			if (state != cs_spawned )
				continue;
			if ( strip_layout_size[i] + newlen <= 1024 ) {
				strcat(strip_layouts[i], newstring);
				strip_layout_size[i]+=newlen;
			} else {
				SOFPPNIX_PRINT("Cant draw this , run out of space");
			}
			edict_t * ent = stget(client,CLIENT_ENT);
			refreshScreen(ent);
		}
		
		return;
	}
	int i = ent->s.skinnum;
	if ( strip_layout_size[i] + newlen <= 1024 ) {
		strcat(strip_layouts[i], newstring);
		strip_layout_size[i]+=newlen;
	} else {
		SOFPPNIX_PRINT("Cant draw this , run out of space");
	}
	refreshScreen(ent);
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
void spackage_register(void)
{
	int c = orig_Cmd_Argc() - 1;
	char * one = orig_Cmd_Argv(1);
	if ( !strcmp(one,"-h" ) ) {
		SOFPPNIX_PRINT(
			"Register a .sp string package file that the client will download\n"
			"----------------------------\n"
			"arg1 -> name of file including .sp extension\n"
			"eg. ++nix_spackage_register mystringpackage.sp\n"
		);
		return;
	}
	if ( c != 1 ) {
		SOFPPNIX_PRINT("++nix_spackage_register -h");
		return;
	}

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
