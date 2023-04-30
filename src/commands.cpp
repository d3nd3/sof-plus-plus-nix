#include "common.h"


void CreateCommands(void)
{
	my_Cmd_AddCommand("++nix_test",&cmd_nix_test);
	my_Cmd_AddCommand("++nix_client_state",&cmd_nix_client_state);
	my_Cmd_AddCommand("++nix_client_map",&cmd_nix_client_map);


	// command to output checksum
	my_Cmd_AddCommand("++nix_checksum",&cmd_nix_checksum);

	
	orig_Cmd_AddCommand("++nix_draw_string",(void*)nix_draw_string);
	orig_Cmd_AddCommand("++nix_draw_string2",(void*)nix_draw_string2);
	orig_Cmd_AddCommand("++nix_draw_altstring",(void*)nix_draw_altstring);
	orig_Cmd_AddCommand("++nix_draw_clear",(void*)nix_draw_clear);

	orig_Cmd_AddCommand("++nix_spackage_register",(void*)nix_spackage_register);
	orig_Cmd_AddCommand("++nix_spackage_print_id",(void*)nix_spackage_print_id);
	orig_Cmd_AddCommand("++nix_spackage_print_ref",(void*)nix_spackage_print_ref);
	orig_Cmd_AddCommand("++nix_spackage_print_obit",(void*)nix_spackage_print_obit);
	orig_Cmd_AddCommand("++nix_spackage_print_string",(void*)nix_spackage_print_string);
	orig_Cmd_AddCommand("++nix_spackage_list",(void*)nix_spackage_list);
	
	orig_Cmd_AddCommand("++nix_spackage_gettext",(void*)nix_spackage_gettext);
	
}

std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
	std::vector<std::string> substrings;
	size_t start = 0;
	size_t end = 0;
	while ((end = str.find(delimiter, start)) != std::string::npos) {
		substrings.push_back(str.substr(start, end - start));
		start = end + delimiter.length();
	}
	substrings.push_back(str.substr(start));
	return substrings;
}

int hexStringToByteArray(const std::string& hexString, unsigned char* buffer, size_t bufferSize) {
	std::vector<std::string> hexBytes = split(hexString, " ");
	if (hexBytes.size() > bufferSize) {
		throw std::invalid_argument("Input string length exceeds buffer size");
	}
	for (size_t i = 0; i < hexBytes.size(); i++) {
		buffer[i] = static_cast<unsigned char>(std::stoi(hexBytes[i], nullptr, 16));
	}
	return hexBytes.size();
}

typedef struct usercmd_S
{
	byte	msec; // 10
	byte	buttons; // [+1]
	byte	lightlevel;		// light level the player is standing on
	byte	lean;			// -1 or 1 [+3]
	short	angles[3]; // [+4/6/8]
	short	forwardmove; // 0A
	short	sidemove; // 0C
	short	upmove; // 0E
	
	float	fireEvent;
	float	altfireEvent;
} usercmd_n;

void cmd_nix_checksum(void)
{
	sizebuf_t sb;
	usercmd_n * u_test = malloc(sizeof(usercmd_n));
	usercmd_n * u_empty = malloc(sizeof(usercmd_n));

	memset(u_empty,0,sizeof(usercmd_n));
	memset(u_test,0,sizeof(usercmd_n));
	memset(&sb,0,sizeof(sb));

	u_test->angles[0] = 1020;
	u_test->angles[1] = 1020;
	u_test->angles[2] = 1020;
	u_test->forwardmove = 2048;
	u_test->sidemove = 2048;
	u_test->upmove = 2048;
	u_test->msec = 100;
	u_test->buttons = 0xFF;
	u_test->lean = 0;
	u_test->lightlevel = 0xFF;
	u_test->fireEvent = 0;
	u_test->altfireEvent = 0;

	sb.data = (byte*)malloc(64);
	sb.maxsize = 64;

	// orig_MSG_WriteDeltaUsercmd(&sb,u_empty,u_test);
	void * out_packet = malloc(1024);
	void (*op_construct)(void * self, int size) = 0x081A8F00;
	op_construct(out_packet,0x64);

	orig_PAK_WriteDeltaUsercmd(out_packet,(usercmd_t*)u_empty,(usercmd_t*)u_test);


	vector<signed char> * v = (vector<signed char>*)(out_packet+8);
	

	// Iterates the vector v and print all of its elements
	for (int i = 0; i < v->size(); i++) {
		// SOFPPNIX_DEBUG("Vector %i : %02X\n",i,*(v->data()+i));
		printf("%02X",*(v->data()+i));
	}
	printf("\n%i\nBITMODE!\n",v->size());

	for ( int i = 0 ; i < v->size() * 8; i++ ) {
		int bit_to_byte = i / 8;
		int bit_to_bit = i % 8;
		if ( (*(v->data()+bit_to_byte) & (1 << bit_to_bit)) != 0 )
			printf("1");
		else
			printf("0");
	}
	printf("\n");

	// for (const auto& element : *v) {
	// 	// do something with the element
	// 	// for example, print it to the console
	// 	// std::cout << element << " ";
	// 	printf("%02X",element);
	// }
	// printf("\n%i\n",v->size());
/*
	// print every sb->data at sequence of hex bytes
	for (int i = 0; i < sb.cursize; i++) {
		// SOFPPNIX_PRINT("%02X ",sb.data[i]);
		printf("%02X ",sb.data[i]);
	}
	printf("Length = %i\n", sb.cursize);

*/

	// int seq = atoi(orig_Cmd_Argv(1));
	// char * buffer = orig_Cmd_Argv(2);
	// int len = strlen(buffer);

	int seq = 103;
	unsigned char buffer[1024];
	int len = hexStringToByteArray(string("13 00 00 00 01 20 00 05 00 fa 17 c0 42 11 00 00 f0 47 00 08 40 01 80 fe 05 b0 50 04 00 00 fc 11 00 02 50 00 a0 7f 01 2c 14 01 00 00 7f"),buffer,1024);
	// debug print seq is : input_data is :
	// SOFPPNIX_DEBUG("seq is : %i input_data is : %s\n",seq,input_data);

	// int out_seq = *(unsigned int*)((*(unsigned int*)0x0829D494) + 0x4F4);
	SOFPPNIX_DEBUG("Checksum : %02X",orig_COM_BlockSequenceCheckByte(buffer,len, seq));

	free(sb.data);
	free(out_packet);
}


/*
 The strings passed here are not part of large buffer, so copy first
*/
void my_Com_Printf(char *msg, ...) {
	#if 0
	char text[1024];
	msg = &text[0];
	strcpy(text,msg);
	// point msg back to text
	
	int len_msg = strlen(msg) + 1;
	if ( len_msg > 1 && len_msg < 1024 ) // any characters? (including null), require 1 free space.
	{
		// shift to right one. ( grows by 1 )
		// save end character ( its just a null character )
		// start at null character. ( usually -1 to make len into offset )
		// but the null character cancels out ( confusing )
		// explicitly change len to include null cahracter
		for ( char * p=msg+len_msg - 1;p>msg;p-- ) {
			*p = *(p-1);
		}
		*msg=0x03;
		*(msg+len_msg) = 0x00; // fits
	}

	// orig_Com_Printf(msg);

	void *args = __builtin_apply_args();

	__builtin_apply((void (*)())orig_Com_Printf, args, 512);
	#else
		char text[1024];
		va_list args;

		va_start(args, msg);
		vsnprintf(text, sizeof(text), msg, args);
		va_end(args);

		// char color[1024];
		
		// snprintf(color,1024,"%c%s",0x03,text);

		orig_Com_Printf(text);
		
	#endif
}


void my_Cmd_AddCommand(char * cmd, void * callback)
{
	// printf("Command Created : %s\n",cmd);

	// if ( !strcmp(cmd,"fx_save") ) {
	//     void *return_address = __builtin_return_address(0);
	//     printf("Cmd_AddCommand Return address: %p\n", return_address);
	// }
	orig_Cmd_AddCommand(cmd,callback);
}

void my_Cmd_RemoveCommand(char * cmd)
{
	orig_Cmd_RemoveCommand(cmd);
}

/*
typedef enum
{
	ca_uninitialized,
	ca_disconnected=1,
	ca_connecting=7,
	ca_connected,
	ca_active=8
} connstate_t;
*/
void cmd_nix_client_state(void)
{
	int** connected_state = 0x0829D494;

	SOFPPNIX_PRINT("Connected state is %i",**connected_state);
}

void cmd_nix_client_map(void)
{
	unsigned int add= 0x0829D480 + 0x1F44;
	char ** mapname = add;

	SOFPPNIX_PRINT("Mapname is %s",*mapname);
}


void fake_cb(void) {
SOFPPNIX_DEBUG("Fake cb");
}
void cmd_nix_test(void)
{
	// int out = orig_SPSP_FindStringID(*(unsigned int*)(iter+0x10),ref_string);
	
	SOFPPNIX_DEBUG("orig_Cmd_Argv(1) : %s",orig_Cmd_Argv(1));
	int out = orig_SP_GetStringID(orig_Cmd_Argv(1) );

	SOFPPNIX_DEBUG("%04X",out);
}

/*
	0 arguments
	clears screen. ( Excluding scoreboard ).

	Puts a watermark on the screen.

	Intention: Clear without remove watermark.

	CheckEvents ( clear in case ClientMessage not called. )
	ClientMessage ( clear + draw )
	ClientEndFrame ( custom draw )
*/
void nix_draw_clear(void)
{
	int c = orig_Cmd_Argc() - 1;
	char * one = orig_Cmd_Argv(1);
	if ( !strcmp(one,"-h" ) ) {
		SOFPPNIX_PRINT(
			"Clears all interface text on screen"
		);
		return;
	}
	if ( c != 0 ) {
		SOFPPNIX_PRINT("++nix_draw_clear -h");
		return;
	}
	SOFPPNIX_DEBUG("Clearing screen");
	layoutstring_len = 0;
	layoutstring[0] = 0x00;
	sprintf(layoutstring,"xr %i yb -16 string \"%s\" ",0 - (sofreebuild_len*8+8),sofreebuildstring);
	layoutstring_len = strlen(layoutstring);
	// orig_Com_Printf("Layoutstring is : %s\n",layoutstring);
}
/*
yb yBottom 0 = bottom of screen, up=negative, down=positive
yt yTop 0 = top of screen, up=negative, down=positive
yv centered

xl xLeft
xr xRight
xv centered

3 arguments - x y msg
*/
void nix_draw_string(void)
{
	int c = orig_Cmd_Argc() - 1;
	char * one = orig_Cmd_Argv(1);
	if ( !strcmp(one,"-h" ) ) {
		SOFPPNIX_PRINT(
			"Draw a string at offset x y with message\n"
			"----------------------------\n"
			"arg1 -> offsetx\n"
			"arg2 -> offsety\n"
			"arg3 -> message\n"
		);
		return;
	}
	if ( c != 3 ) {
		SOFPPNIX_PRINT("++nix_draw_string -h");
		return;
	}
	int offsetx = atoi(orig_Cmd_Argv(1));
	int offsety = atoi(orig_Cmd_Argv(2));
	char * message = orig_Cmd_Argv(3);
	int len = strlen(message);

	if ( layoutstring_len + len <= 1024 ) {
		char newstring[256];
		sprintf(newstring,"xv %i yv %i string \"%s\" ",offsetx,offsety,message);
		int newlen = strlen(newstring);
		strcat(layoutstring, newstring);
		layoutstring_len+=newlen;
	} else {
		SOFPPNIX_PRINT("Cant draw this , run out of space");
	}
}
/*
3 arguments - x y msg
*/
void nix_draw_string2(void)
{
	int c = orig_Cmd_Argc() - 1;
	char * one = orig_Cmd_Argv(1);
	if ( !strcmp(one,"-h" ) ) {
		SOFPPNIX_PRINT(
			"Draw a string at offset x y with message\n"
			"----------------------------\n"
			"arg1 -> offsetx\n"
			"arg2 -> offsety\n"
			"arg3 -> message\n"
		);
		return;
	}
	if ( c != 3 ) {
		SOFPPNIX_PRINT("++nix_draw_string2 -h");
		return;
	}
	int offsetx = atoi(orig_Cmd_Argv(1));
	int offsety = atoi(orig_Cmd_Argv(2));
	char * message = orig_Cmd_Argv(3);
	int len = strlen(message);

	if ( layoutstring_len + len <= 1024 ) {
		char newstring[256];

		sprintf(newstring,"%s ",message);
		int newlen = strlen(newstring);
		strcat(layoutstring, newstring);
		layoutstring_len+=newlen;
	} else {
		SOFPPNIX_PRINT("Cant draw this , run out of space");
	}
}

/*
3 arguments - x y msg
*/
void nix_draw_altstring(void)
{
	int c = orig_Cmd_Argc() - 1;
	char * one = orig_Cmd_Argv(1);
	if ( !strcmp(one,"-h" ) ) {
		SOFPPNIX_PRINT(
			"Draw a grey string at offset x y with message\n"
			"----------------------------\n"
			"arg1 -> offsetx\n"
			"arg2 -> offsety\n"
			"arg3 -> message\n"
		);
		return;
	}
	if ( c != 3 ) {
		SOFPPNIX_PRINT("++nix_draw_altstring -h");
		return;
	}
	int offsetx = atoi(orig_Cmd_Argv(1));
	int offsety = atoi(orig_Cmd_Argv(2));
	char * message = orig_Cmd_Argv(3);
	int len = strlen(message);
	if ( layoutstring_len + len <= 1024 ) {
		for (int i = 0; i < len; i++ ) {
				*(message+i) = *(message+i) | 0x80;
		}

		char newstring[256];
		sprintf(newstring,"xv %i yv %i altstring \"%s\" ",offsetx,offsety,message);
		int newlen = strlen(newstring);
		strcat(layoutstring, newstring);
		layoutstring_len+=newlen;
	} else {
		SOFPPNIX_PRINT("Cant draw this , run out of space");
	}
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
void nix_spackage_list(void)
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
void nix_spackage_register(void)
{
	SOFPPNIX_DEBUG("nix_spackage_register");
	
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
	SOFPPNIX_DEBUG("in_file_path %s",in_file_path.c_str());
	// If the file does not exist inside USER. Treat normally.
	FILE * IN_FILE = fopen(in_file_path.c_str(),"r+");
	if ( !IN_FILE ) {
		SOFPPNIX_DEBUG("Detected not in user!");
		char * shorten= strchr(one,'.');
		if ( shorten ) {
			*shorten = 0x00;	
		}
		SOFPPNIX_DEBUG("Registering %s!",one);
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
				// print the id
				SOFPPNIX_DEBUG("ID: %i",id);

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
		SOFPPNIX_DEBUG("CRC: %s\n",crc_file.c_str());
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
		SOFPPNIX_DEBUG("Registering %s",new_file_path.c_str());

		orig_SP_Register(registerFileName.c_str());

		next_available_ID++;
		if ( mybuffer )
			free(mybuffer);
	}
	
}

/*
4 arguments - ent/0, file,whichString,WhatPrint
nix_print_id SOFREE CHEER
*/
void nix_spackage_print_ref(void)
{
	int c = orig_Cmd_Argc() - 1;
	SOFPPNIX_DEBUG("nix_spackage_print_ref %i",c);

	char * one = orig_Cmd_Argv(1);
	if ( !strcmp(one,"-h" ) ) {
		SOFPPNIX_PRINT(
			"Print an entry from a .sp string package file\n"
			"----------------------------\n"
			"arg1 -> recipient ent handle or 0 for broadcast\n"
			"arg2 -> file REFERENCE string\n"
			"arg3 -> id REFERENCE string\n"
			"eg. sf_sv_spackage_print_ref SOFREE CHEER ...\n"
		);
		return;
	}
	if ( c < 3 ) {
		SOFPPNIX_PRINT("++nix_spackage_print_ref -h");
		return;
	}

	edict_t * ent = atoi(orig_Cmd_Argv(1));

	char temp[64];
	sprintf(temp,"%s_%s",orig_Cmd_Argv(2),orig_Cmd_Argv(3));
	
	short package_id = orig_SP_GetStringID(temp);

	if ( package_id != -1 ) {
		SOFPPNIX_DEBUG("package_id %i",package_id);
		char * cfmt = stget(orig_SP_GetStringText(package_id),0x0C);
		// std::cout << test;
		std::string fmt(cfmt);

		std::vector<std::string> inputs;
		for ( int i = 0; i < c-3; i++ ) {
			inputs.push_back(std::string(orig_Cmd_Argv(i+4)));
			SOFPPNIX_DEBUG("input %s",inputs[i].c_str());
		}
		SP_PRINT_MULTI(ent,package_id,fmt,inputs);
	} else {
		orig_Com_Printf("Error ++nix_spackage_print_ref : cant find the string refs %s %s\n",orig_Cmd_Argv(2),orig_Cmd_Argv(3));
	}
}

//typedef void (*SP_Print)(edict_t *ent, unsigned short ID, ...);
// who,num
void nix_spackage_print_id(void)
{
	int c = orig_Cmd_Argc() - 1;
	char * one = orig_Cmd_Argv(1);
	if ( !strcmp(one,"-h" ) ) {
		SOFPPNIX_PRINT(
			"Print an entry from a .sp string package file\n"
			"----------------------------\n"
			"arg1 -> ent handle or 0 for broadcast\n"
			"arg2 -> ID of stringpackage file (found inside the file)\n"
			"arg3 -> Index number of the text you want to print\n"
			"arg4+ -> Extra strings for passing data into the output\n"
			"eg. sf_sv_spackage_print_id 18 25 ...\n"
			"NB. %d = integer %p = byte %s = string %hd = signed_short %hu = unsigned short\n"
		);
		return;
	}
	if ( c < 3 ) {
		SOFPPNIX_PRINT("sf_sv_spackage_print_id -h");
		return;
	}

	edict_t * ent = atoi(orig_Cmd_Argv(1));
	unsigned char file_id = atoi(orig_Cmd_Argv(2));
	unsigned char index = atoi(orig_Cmd_Argv(3));
	unsigned short package_id = file_id << 8;
	package_id |= index;
	
	// orig_SP_SPrint(format,1024,final);
	std::string fmt = orig_SP_GetStringText(package_id);

	std::vector<std::string> inputs;
	for ( int i = 0; i < c-3; i++ ) {
		inputs.push_back(orig_Cmd_Argv(i+4));
	}

	SP_PRINT_MULTI(ent,package_id,fmt,inputs);
}
// typedef void (*SP_Print_Obit)(edict_t *ent, unsigned short ID, ...);
// who,num
void nix_spackage_print_obit(void)
{
	unsigned int ed_base = *(unsigned int*)EDICT_BASE;
	orig_SP_Print(ed_base + (atoi(orig_Cmd_Argv(1))+1)*SIZE_OF_EDICT,atoi(orig_Cmd_Argv(2)));
}
// typedef int (*SP_SPrint)(char *buffer, int buffer_size, unsigned short ID, ...);

// who,msg,len
void nix_spackage_print_string(void)
{
	orig_SP_Print((edict_t*)orig_Cmd_Argv(2),(edict_t*)atoi(orig_Cmd_Argv(3)),(edict_t*)atoi(orig_Cmd_Argv(1)));
}
// typedef const char	*(*SP_GetStringText)(unsigned short ID);
void nix_spackage_gettext(void)
{
	char out[2048] = "echo ";
	strcat(out,orig_SP_GetStringText(atoi(orig_Cmd_Argv(1))));
	orig_Cbuf_AddText(out);	
}