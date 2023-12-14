#include "common.h"


void CreateCommands(void)
{
	my_Cmd_AddCommand("++nix_test",&cmd_nix_test);
	my_Cmd_AddCommand("++nix_client_state",&cmd_nix_client_state);
	my_Cmd_AddCommand("++nix_client_map",&cmd_nix_client_map);

	#if 0
	my_Cmd_AddCommand("++nix_demomap",&cmd_nix_demomap);
	my_Cmd_AddCommand("++nix_record",&cmd_nix_record);
	my_Cmd_AddCommand("++nix_stoprecord",&cmd_nix_stoprecord);
	#endif

	// command to output checksum
	my_Cmd_AddCommand("++nix_checksum",&cmd_nix_checksum);

	orig_Cmd_AddCommand("++nix_spackage_register",(void*)spackage_register);
	
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


	std::vector<signed char> * v = (std::vector<signed char>*)(out_packet+8);
	

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
	int len = hexStringToByteArray(std::string("13 00 00 00 01 20 00 05 00 fa 17 c0 42 11 00 00 f0 47 00 08 40 01 80 fe 05 b0 50 04 00 00 fc 11 00 02 50 00 a0 7f 01 2c 14 01 00 00 7f"),buffer,1024);
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

	orig_GhoulServerFlushClients();
	return;
	sizebuf_t buf;
	//sizebuf_t memset to 0.
	memset(&buf,0,sizeof(sizebuf_t));

	char	buf_data[1400];

	//buffer
	buf.data = buf_data;
	buf.data[0] = 0x00;
	buf.maxsize = 1400;

	short Len = 0;
	SOFPPNIX_DEBUG("Type : %02X",svc_ghoulreliable);
	orig_MSG_WriteByte(&buf,svc_ghoulreliable);
	orig_SZ_Write(&buf,&Len,2);

	int frameNum = stget(0x082AF680,0x10);
	int written = 0;
	//slot, frameNum, buffer, freeSpace, int* written
	int unknown = orig_GhoulPackReliable(0, frameNum, buf.data, 1400/2 - (buf.cursize-8),&written);

	SOFPPNIX_DEBUG("Unknown = %i ... Written = %i",unknown,written);

	orig_GhoulReliableHitwire(0,frameNum);

	SOFPPNIX_DEBUG("Test done");
}


#if 0
void cmd_nix_demomap(void)
{

	if ( demo_system.recording_status ) {
		SOFPPNIX_PRINT("Stop your recording first.");
		return;
	}
	//use serverstate 9 for demoActive.
	int serverstate = stget(0x082AF680,0);
	if ( serverstate == 3 ) {
		SOFPPNIX_PRINT("TODO: Clear demo and load other demo whilst playback.");
		return;
	}

	/*
		use the .dm3 extension format for files.
		else assumed spawncount selection integer id from memdump list.
	*/
	if ( !demo_system.LoadDemo() ) return;
	

	demo_system.serverstate_trigger_play = true;

	orig_SV_Map(true, "serversidedemo" , false );

	//Disable demofile
	demo_system.serverstate_trigger_play = false;
}

void cmd_nix_record(void)
{

	demo_system.demo_recorder->startRecording();
	
}

void cmd_nix_stoprecord(void)
{
	
#if 0
	//Copy/Save the buffer.
	for(int i = 0; i < 16; ++i) {
		ghoulChunksReplay[i] = ghoulChunks[i]; // Copy each vector
		//Iterate the vector to deep copy
		for (size_t j = 0; j < ghoulChunksReplay[i].size(); ++j) {
			ghoulChunksReplay[i][j].data = malloc(ghoulChunks[i][j].len);
			memcpy(ghoulChunksReplay[i][j].data, ghoulChunks[i][j].data, ghoulChunks[i][j].len);
		}
		ghoulChunksSavedReplay[i] = ghoulChunksSaved[i];
	}
#endif
	demo_system.demo_recorder->endRecording();
}
#endif