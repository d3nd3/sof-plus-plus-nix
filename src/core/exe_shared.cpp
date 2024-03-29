#include "common.h"

CURL * curl_handle = NULL;
unsigned char chktbl2[3000];
int sofreebuild_len;
char sofreebuildstring[128];

void my_Qcommon_Frame(int msec)
{
	// outputs the console buffer to stdout
	// sizebuf_t * cmd_buf = 0x083F51D0;
	// if ( cmd_buf->cursize > 0 ) {
	// 	SOFPPNIX_DEBUG("CL_Frame1cmd_buf->cursize = %i",cmd_buf->cursize);
	// 	hexdump((unsigned char*)cmd_buf->data,(unsigned char*)cmd_buf->data + cmd_buf->cursize);
	// }

	if ( dedicated->value == 1.0f ) {

		#ifdef USE_PYTHON
			// Check if the error indicator is set
			if (PyErr_Occurred()) {
				// Print the Python error message to stdout
				PyErr_PrintEx(0);
			}
		#endif	
	}
	

	isHTTPdone();

	orig_Qcommon_Frame(msec);
}

/*
A safer place to call init code because cvars work.
And its before dedicated_start, thus before game init code, which is ran each level change.

// add + commands from command line
if (!Cbuf_AddLateCommands ())
{	// if the user didn't give any commands, run default action
	if (!dedicated->value)
		Cbuf_AddText ("d1\n");
	else
		Cbuf_AddText ("dedicated_start\n"); //exec dedicated.cfg
	Cbuf_Execute ();
}
else
{	// the user asked for something explicit
	// so drop the loading plaque
	SCR_EndLoadingPlaque ();
}
*/
qboolean my_Cbuf_AddLateCommands(void)
{

	qboolean ret = orig_Cbuf_AddLateCommands();
	// if ( ret ) return ret;

	// Linux chktbl is slightly different than windows. Has some 0x80 instead of 0x00
	// memcpy(chktbl2,(void*)0x08293C80,3000);
	unsigned char * lin_chktbl = 0x08293C80;
	lin_chktbl[439] = 0x00;
	lin_chktbl[511] = 0x00;
	lin_chktbl[1591] = 0x00;
	lin_chktbl[2191] = 0x00;
	lin_chktbl[2239] = 0x00;
	lin_chktbl[2575] = 0x00;
	lin_chktbl[2587] = 0x00;
	lin_chktbl[2611] = 0x00;
	lin_chktbl[2647] = 0x00;


	createClientCvars();
	createSharedCvars();


	//------------------------PLUS PLUS NIX VERSION----------------------------
	char tmp_chr[256];
	sprintf(tmp_chr,"%hu%02hhu%02hhu.%hu",(unsigned int)&__BUILD_YEAR,(unsigned int)((&__BUILD_MONTH)-16) & 0xF,(unsigned int)(&__BUILD_DAY)-64,(unsigned int)&__BUILD_NUMBER);

	sprintf(sofreebuildstring,"++nix build %s ",tmp_chr);
	sofreebuild_len = strlen(sofreebuildstring);
	// turn it grey
	// for (int i = 0; i < sofreebuild_len; i++ ) {
	// 	*(sofreebuildstring+i) = *(sofreebuildstring+i) | 0x80;
	// }

	setCvarString(_nix_version,tmp_chr);
	SOFPPNIX_PRINT("++nix Initialised. Version :  %s",tmp_chr);

	// loadHttpCache();
	curl_global_init(CURL_GLOBAL_NOTHING);

	SOFPPNIX_PRINT("Curl Version : %s",curl_version());

	
	
	// Init has been done!
	// ------------------- CREATING COMMANDS --------------------
	// my_Cmd_AddCommand("testaddserver",&testaddserver);

	// my_Cmd_RemoveCommand("pingservers");
	// my_Cmd_AddCommand("pingservers",&my_CL_PingServers);

	CreateCommands();


	/*
		Used by client and server.
	*/
	gs_select_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (gs_select_sock < 0) {
		error_exit("Failed to create gamespy socket.\n");
	}
	// Non-blocking
	int flags = fcntl(gs_select_sock, F_GETFL, 0);
	fcntl(gs_select_sock, F_SETFL, flags | O_NONBLOCK);


	if ( dedicated->value == 1.0f ) {
		// Has to be below the gs_select_sock line above.
		serverInit();
	}

	return ret;
}
/*
	Shared Init

	QCommon_Init called dedicated_start, which calls map.
	Thus SV_InitGameProgs which calls GetGameAPI is called within Qcommon_Init.
*/
void my_Qcommon_Init(int one , char ** two) {
	// orig_Cvar_Get crashing Z_malloc before Qcommon_Init.
	
	orig_Qcommon_Init(one,two);

	// This cvar is set by dedicated_start dedicated.cfg
	// ------------------------ GAMESPY BROADCASTING -----------------------
	if ( sv_public && sv_public->value ) {
		SOFPPNIX_PRINT("Server is public.");
		
		// Bind to port
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(gamespyport->value);
		addr.sin_addr.s_addr = INADDR_ANY;

		if (bind(gs_select_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			error_exit("Unable to bind to gamespy port");
		}

		// gamespyport hostport map change -> 27900
		// master checks using src port 28904 '\\info\\' ~every 5 minutes
		// master checks using src port 28902 '\\status\\' ~every 30 seconds
		// client query server list -> 28900

		// memset 0 the master
		memset(&sof1master_ip, 0, sizeof(sof1master_ip));
		orig_NET_StringToAdr("sof1master.megalag.org:27900", &sof1master_ip);

		// orig_NET_StringToAdr("5.135.46.179:27900",&sof1master_ip);
		// orig_NET_StringToAdr("localhost:27900", &sof1master_ip);
		// orig_NET_StringToAdr("172.22.130.228:27900", &sof1master_ip);
	}
}

void my_Qcommon_Shutdown(void)
{
	#ifdef USE_PYTHON
		Py_Finalize();
	#endif
	curl_global_cleanup();

	orig_Qcommon_Shutdown();

	// close the gamespy socket
	if ( sv_public && sv_public->value || gs_select_sock ) {
		close(gs_select_sock);
	}
}


// 	std::string input = "hello world";
// 	unsigned char digest[MD4_DIGEST_LENGTH];
// 	MD4((const unsigned char*)input.c_str(), input.length(), digest);
// 	std::cout << "MD4 hash of input: ";
// 	for (int i = 0; i < MD4_DIGEST_LENGTH; i++) {
// 		printf("%02x", digest[i]);
// 	}
// 	std::cout << std::endl;
#if 1
unsigned char my_my_COM_BlockSequenceCRCByte(std::vector<unsigned char> base, unsigned int sequence) {
	
	unsigned char* p = &chktbl2[sequence % 2996];

	// max total len = 64
	if (base.size() > 60) {
		// truncate to 60 length
		base.resize(60);
	}

	base.push_back((sequence & 0xff) ^ p[0]);
	base.push_back(p[1]);
	base.push_back(((sequence >> 8) & 0xff) ^ p[2]);
	base.push_back(p[3]);

	// if ( sequence == 436 ) {
	// 	printf("p3 is : %02x\n", p[3]);
	// 	//print the vector base
	// 	for (int i = 0; i < base.size(); i++) {
	// 		printf("%02x", base[i]);
	// 	}
	// 	printf("\n");
	// }

	// MD4 hash here
	EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
	const EVP_MD* md = EVP_md4();
	unsigned int md_len;
	unsigned char md_value[EVP_MAX_MD_SIZE];
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, base.data(), base.size());
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_free(mdctx);

	// extract 4 uint32_t from the MD4 hash
	uint32_t d1 = *((uint32_t*)(md_value + 0));
	uint32_t d2 = *((uint32_t*)(md_value + 4));
	uint32_t d3 = *((uint32_t*)(md_value + 8));
	uint32_t d4 = *((uint32_t*)(md_value + 12));

	// calculate the CRC checksum
	uint32_t checksum = (d1 ^ d2 ^ d3 ^ d4) & 0xFF;

	// print checksum
	// if ( sequence == 436 ) {
	
	// 	for (int i=0; i < md_len; i++ ) {
	// 		printf("%02x", md_value[i]);
	// 	}
	// 	printf("\n");

	// 	printf("checksum = %02x\n", checksum);
	// }
	
	return checksum;
}
#else
unsigned char my_my_COM_BlockSequenceCRCByte(std::vector<unsigned char> base, unsigned int sequence) {
	static unsigned char* chktbl2 = (unsigned char*)0x08293C80;
	unsigned char* p = chktbl2 + (sequence % 2996);

	// SOFPPNIX_DEBUG("firstbyte = %02X\n",chktbl2[0]);

	// Truncate base to 60 bytes if it's longer
	if (base.size() > 60) {
		base.resize(60);
	}

	// Calculate the first 4 bytes of base
	base.push_back((sequence & 0xff) ^ p[0]);
	base.push_back(p[1]);
	base.push_back(((sequence >> 8) & 0xff) ^ p[2]);
	base.push_back(p[3]);

	// Calculate the MD4 hash of base
	MD4GPT md4;
	md4.update(&base[0], base.size());
	unsigned char md4_hash[MD4_DIGEST_LENGTH];
	md4.finalize(md4_hash);

	// Unpack the hash into 4 32-bit integers
	unsigned int d1, d2, d3, d4;
	std::memcpy(&d1, md4_hash, sizeof(unsigned int));
	std::memcpy(&d2, md4_hash + sizeof(unsigned int), sizeof(unsigned int));
	std::memcpy(&d3, md4_hash + 2 * sizeof(unsigned int), sizeof(unsigned int));
	std::memcpy(&d4, md4_hash + 3 * sizeof(unsigned int), sizeof(unsigned int));

	// XOR the 4 integers to get the checksum
	unsigned int checksum = d1 ^ d2 ^ d3 ^ d4;

	// Return the least significant byte of the checksum
	return checksum & 0xff;
}

#endif

unsigned int saved_src;
unsigned int saved_len;
unsigned int saved_seq;
unsigned char saved_checksum;

unsigned char my_COM_BlockSequenceCheckByte(unsigned char * src , int len, int out_seq_id )
{
	
	// saved_src = (unsigned int)src;
	// saved_len = len;
	// saved_seq = out_seq_id;

	// unsigned char checksum1 =  orig_COM_BlockSequenceCheckByte(src,len,out_seq_id);
	unsigned char checksum = my_my_COM_BlockSequenceCRCByte(std::vector<unsigned char>(src, src + len), out_seq_id);

	// if ( checksum1 != checksum ) {
	// 	SOFPPNIX_DEBUG("COM_BlockSequenceCheckByte : %08X %i %i\n",src,len,out_seq_id);
	// }
	// saved_checksum = checksum;
	// int out_seq = *(unsigned int*)((*(unsigned int*)0x0829D494) + 0x4F4);
	// SOFPPNIX_PRINT("COM_BlockSequenceCheckByte : %02X %i %i",checksum,out_seq_id, out_seq);
	return checksum;
}

void my_test(void)
{
	SOFPPNIX_DEBUG("Saved : %08X %i %i checksum=%02X",saved_src,saved_len,saved_seq,saved_checksum);
	hexdump((unsigned char*)saved_src,(unsigned char*)saved_src + saved_len);

	SOFPPNIX_DEBUG("\n\n");
}

/*
any commands stuff on same frame as map change command
are preserved in seperate buffer
until first client connects
affects eg.:
	exec hidebot.cfg
	exec bot/mapcfgs/default.cfg
*/
void my_Cbuf_Execute(void)
{
	// SOFPPNIX_DEBUG("Cbuf_Execute");
	// char * cmd_text = 0x083F5200;

	// char * str = cmd_text;
	// sizebuf_t * cmd_buf = 0x083F51D0;
	// for (int i = 1; i < cmd_buf->cursize; i++) {
	// 	if (cmd_text[i] == 0x0A) {
	// 		// use memcpy to copy the string to a null-terminated buffer
	// 		char buf[0x100];
	// 		memset(buf, 0, sizeof(buf));
	// 		// +1 to count its self , offset to length.
	// 		int len = (cmd_text + i - 1 + 1 ) - str;
	// 		std::memcpy(buf, str, len);
	// 		buf[len] = 0;
	// 		// print every byte in the string
	// 		// for (int j = 0; j < len; j++) {
	// 		// 	printf("%02X ", str[j]);
	// 		// }
	// 		// printf("\n");
	// 		hexdump(buf,buf+len);
	// 		str = cmd_text + i + 1;
	// 	}
	// }

	// char * defer_text = 0x083F7200;
	// str = defer_text;
	// for (int i = 1; i < 0x2000; i++) {
	// 	if (defer_text[i] == 0x0A || defer_text[i] == 0x00) {
	// 		// use memcpy to copy the string to a null-terminated buffer
	// 		char buf[0x100];
	// 		int len = (defer_text + i - 1 + 1 ) - str;
	// 		std::memcpy(buf, str, len);
	// 		buf[len] = 0;
	// 		SOFPPNIX_DEBUG("Defer Text: ");
	// 		hexdump(buf,buf+len);
	// 		str = defer_text + i + 1;

	// 		if ( defer_text[i] == 0x00 ) break;
	// 	}
	// }	
	orig_Cbuf_Execute();
}



static void slideFix(void)
{
	/*
		Longer you are in the air, the faster you fall.

		pml.velocity[2] -= pm->s.gravity * pml.frametime;
		OVERRIDING THIS.

		if the applied velocity[2] due to gravity, is 

		if frametime * sv_gravity * frametime < 0.125 slide ON
		if frametime * sv_gravity * frametime < 0.125 slide OFF

		
		extratime += msec;
		cls.frametime = extratime/1000.0;

		cls.frametime is frametime in seconds.

		delta_velocity = accel * frametime
		delta_origin = delta_velocity * frametime

		gravity is accel here.
	*/

	// void * pml = stget(0x0829DD2C,0);
	void * pml = 0x084278C0;
	float * vel_y = (void*)pml + 0x14;
	float * frametime = pml + 0x3C;
	int * ladder = (void*)pml + 0x68;


	// void * pm = stget(0x0829DD28,0);
	void * pm = 0x084278A0;
	edict_t ** groundEntity = pm + 0xE4;
	
	//if ( _nix_slidefix->value == 1.0f ) {
	if ( !(*groundEntity) && !(*ladder) )
	{
		// Velocity here already includes the calculation:
		// pml.velocity[2] -= pm->s.gravity * pml.frametime
		// If the calculated "ORIGIN offset" is between <0 and -0.125>
		// Clamp it to -0.125001
		// So sets a minimum distance of a move downwards.
		if (*vel_y < 0 && *vel_y * *frametime > -0.125 )
		{
			// SOFPPNIX_DEBUG("slideFix : %f %f",*vel_y,*frametime);
			
			//delta_velocity = accel * frametime
			//delta_origin = delta_velocity * frametime
			*vel_y = -0.125001 / *frametime;
			// oriign_offset = frametime*frametime*sv_gravity
		}	
	}
	//}
}

void my_PM_StepSlideMove(int num)
{
	slideFix();
	orig_PM_StepSlideMove(num);
}

/*
This is a server specific hook of Netchan_Transmit, needed primarily for saving data for demo.
*/
void my_Netchan_Transmit_Save (netchan_t *chan, int length, byte *data)
{
	//SOFPPNIX_DEBUG("my_netchan_save");

	#if 0
	if ( demo_system.recording_status ) {

		//netchan_t->reliable_length
 		int rel_len = stget(chan,0x404c);
 		//netchan_t->message.cursize
		int cur_size = stget(chan,0x54);
		//netchan_t->message_buf;
		unsigned char * relBuf = (void*)chan+0x5C;

		int relLen = 0;
		if ( !rel_len && cur_size ) {
			// new reliable sent this frame.
			relLen = cur_size;
		}
		// SOFPPNIX_DEBUG("About to call storeDemoData");
		demo_system.demo_recorder->saveNetworkData(chan,relLen,relBuf,length,data);
	}
	#endif

	my_Netchan_Transmit(chan,length,data);
}

/*
This is a server specific hook of Netchan_Transmit, needed primarily for playback of demo, routing demo data into the network buffer to be sent out to the clients.

client state is still cs_zombie after disconnect.
*/
void my_Netchan_Transmit_Playback (netchan_t *chan, int length, byte *data)
{
	#if 0
	int serverstate = stget(0x082AF680,0);
	//sv.state = serverstate; inside SV_SpawnServer.
	if ( !demo_system.demo_player->active ) {
		//if its not 9, it will be 3. So this is normal cl_record_f behaviour.
		return my_Netchan_Transmit(chan,length,data);
	}

	//toggled by SV_New_f
	if ( !demo_system.demo_player->packet_override ) return;

	demo_system.demo_player->playback(chan,length,data);

	//SOFPPNIX_DEBUG("EndNetchan");
	#else
	return my_Netchan_Transmit(chan,length,data);
	#endif

}

void my_Netchan_Transmit (netchan_t *chan, int length, byte *data)
{
	#if 0
	//SOFPPNIX_DEBUG("Netchan_Transmit...");
	//Make Need_reliable see relAccumulate instead of chan->message
	if ( demo_system.demo_player->netchan_close_rel ) stset(chan,0x54,demo_system.demo_player->demo_rel.cursize);
	#endif
	orig_Netchan_Transmit(chan,length,data);
	
}

void my_Netchan_Patch(netchan_t *chan)
{
	#if 0
	//SOFPPNIX_DEBUG("my_Netchan_Patch");
	#if 0
	if ( demo_system.demo_player->netchan_close_rel && !stget(chan,0x404c) && demo_system.demo_player->demo_rel.cursize ) {
	
		memcpy ((void*)chan+0x4050, demo_system.demo_player->demo_rel_buf, demo_system.demo_player->demo_rel.cursize);
	
		stset(chan,0x404c,demo_system.demo_player->demo_rel.cursize);

	
		stset(chan,0x40,stget(chan,0x40)^1);

		demo_system.demo_player->demo_rel.cursize = 0;

	} else if ( !demo_system.demo_player->netchan_close_rel && !stget(chan,0x404c) && stget(chan,0x54) ) {
	
		memcpy((void*)chan+0x4050,(void*)chan+0x5c,stget(chan,0x54));
	
		//reliable_length
		stset(chan,0x404c,stget(chan,0x54));
	
		//reliable_sequence
		stset(chan,0x40,stget(chan,0x40)^1);
	
		stset(chan,0x54,0);
	}
	#else
	if ( demo_system.demo_player->netchan_close_rel && !chan->reliable_length && demo_system.demo_player->demo_rel.cursize ) {
		memcpy (chan->reliable_buf, demo_system.demo_player->demo_rel_buf, demo_system.demo_player->demo_rel.cursize);
		chan->reliable_length = demo_system.demo_player->demo_rel.cursize;
		chan->reliable_sequence ^= 1;
		demo_system.demo_player->demo_rel.cursize = 0;

	} else if ( !demo_system.demo_player->netchan_close_rel && !chan->reliable_length && chan->message.cursize ) {
		memcpy (chan->reliable_buf, chan->message_buf, chan->message.cursize);
		chan->reliable_length = chan->message.cursize;
		chan->reliable_sequence ^= 1;
		chan->message.cursize = 0;
	}
	#endif
	
	#endif
}