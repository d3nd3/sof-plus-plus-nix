#include "common.h"

class DemoData {
	//connecting. data,len
	std::vector<chunk_t> activation_chunks; //initialChunks

	//svc_ghoulreliable stuff.
	bool ghoulChunksSaved[16]; // when GhoulPackReliable returns 1, sets this to true.
	std::vector<chunk_t> ghoul_chunks[16];

	//demo_frames_map : map of frames->frameData
	//frameData : map of slots->DATA{rel,unrel}
	DemoFramesMap demo_frames_map;
}

class DemoRecorder {
	int start_spawncount = 0;
	bool active = false; //recordingStatus
	char name[MAX_TOKEN_CHARS];

	bool non_delta_trigger = false; //demoWaiting

public:
	void DemoRecorder(spawncount) : start_spawncount(spawncount){
	}

	void begin(void) {
		if ( orig_Cmd_Argc() != 2 ) {
			SOFPPNIX_PRINT("Specify demo name");
			return;
		}
		//not rec
		if ( demo_system.recordingStatus ) {
			SOFPPNIX_PRINT("You are already recording.");
			return;
		}
		//not playback
		int serverstate = stget(0x082AF680,0);
		if ( serverstate == 3 ) {
			SOFPPNIX_PRINT("Cannot record during demo playback.");
			return;
		}

		demo_recorder.non_delta_trigger = true;
		
		snprintf(demo_recorder.name,MAX_TOKEN_CHARS,"%s.dm2",orig_Cmd_Argv(1));

		demo_system.recording_status = true;

		//saves initchunks
		storeServerData();

		//stoprecord saves ghoulchunks

		//all other data saved by netchan_transmit.
		SOFPPNIX_DEBUG("Recording demoname : %s!",recordingName);
	}
	void finish(void) {
		if ( !demo_system.recordingStatus ) {
			SOFPPNIX_PRINT("You must be recording to stop a recording.");
			return;
		}
		SOFPPNIX_DEBUG("Recording ended!");
		
		demo_system.recordingStatus = false;
	}
}

class DemoPlayer {
	int start_spawncount; //set by cmd_argc() or read from demofile
	bool active = false; //playbackStatus
	
	bool packet_override = false; //demoPlaybackInitiate

	int first_frame = 0; //startDemoFrame
	int current_frame = 0; //currentDemoFrame
	int final_frame = 0; //finalDemoFrame

	int watch_slot = -1; //prefferedFighter

	bool netchan_close_rel = false; //disableDefaultRelBuffer

	sizebuf_t demo_rel; //relAccumulate
	char	demo_rel_buf[1400-16]; //accum_buf

	//progress for each client/viewer
	int ghoulChunkIndex[16];
	bool ghoulLoaded[16];

public:

	void DemoPlayer()
	{
		SOFPPNIX_DEBUG("DemoPlayer Constructor()");
	}
	void begin(void) {
		/*
			Called by demo_system.PrepareLevel()
			We should have already loaded the file from disk. Validated it.
			Load from disk vs load from memory.

			Create a command to list the previous 10 entries that exist in the demos map, with their corresponding mapname and time existed.

			demomap 5435456

			demomap pastdemo
		*/

		start_spawncount

		SOFPPNIX_DEBUG("DemoPlayer: Begin()");
		
		active = true;

		//sz_init
		memset(&demo_rel,0,sizeof(sizebuf_t));
		//buffer
		demo_rel.data = demo_rel_buf;
		demo_rel.maxsize = 1400-16;

		demo_rel.allowoverflow = false;


		/*
			Maybe we can have last 10 maps data in memory.
			Requires an admin to selectively write them to disk.
			A cvar can force write to disk automatically.
		*/

		//get lowest frame
		first_frame = demo_system.demos[spawncount].begin()->first;
		current_frame = startDemoFrame;
		final_frame = demo_system.demos[spawncount].rbegin()->first;
		SOFPPNIX_DEBUG("Starting demo on frame : %i / %i",currentDemoFrame, finalDemoFrame);

	}

	/*
	demo_system.demo_player->packet_override - after New
	*/
	void demos_handlePlayback(netchan_t *chan, int length, byte *data)
	{
		
		int slot = netchanToSlot(chan);
		if ( slot == -1 ) error_exit("Cannot convert netchan to slot");

		void * client = getClientX(slot);
		int clientState = stget(client,0);

		if ( clientState <= cs_zombie ) {
		//ignore unconnected clients.
			return;
		}

		int frameNum = stget(0x082AF680,0x10);
		int last_sent = stget(chan,0x10);
		int curtime = stget(0x08526624,0);
		//do we have to something send?
		
		std::vector<chunk_t>& start_chunks = demo_system.demos[spawncount][frameNum].activation_chunks;
		if ( !start_chunks.empty() ) {
			
			/*
				client response rate was low whilst connecting. patched it.
			*/

				//netchan_t->reliable_length
			int rel_len = stget(chan,0x404c);
				//netchan_t->message.cursize
			int cur_size = stget(chan,0x54);

			//If new reliable sent now, swap buffers.
			if ( !rel_len ) {
				// new reliable can be sent this frame.
				SOFPPNIX_DEBUG("Sending initialChunk");

				auto first = start_chunks.begin();

				SOFPPNIX_DEBUG("Connect-Packet-Size = %i",first->len);

				orig_SZ_Write(&demo_rel,first->data,first->len);

				start_chunks.erase(first);

				netchan_close_rel = true;
				my_Netchan_Transmit (chan, 0, NULL);
				chan->message.cursize = 0;

			} else {
					//attempt resend of lost packets
				if ( curtime - last_sent > 1000 ) {
					SOFPPNIX_DEBUG("1 Second passed...");
					netchan_close_rel = true;
					my_Netchan_Transmit (chan, 0, NULL);
				}
			}
		}//initChunks empty
		else {
			//normal svc_serverdata svc_frame etc...
			SOFPPNIX_DEBUG("SPAWNED...");

			if ( !ghoulLoaded[slot] ) {
				int ghoulSlot = 0;

				//Handle ghoulChunks before sending any frames.
				bool validGhoul = false;
				for ( int i = 0; i<16;i++ ){
					if ( ghoulChunksSavedReplay[i] ) {
						validGhoul = true;
						ghoulSlot = i;
						break;
					}
				}
				if ( !validGhoul ) {
					SOFPPNIX_DEBUG("This demo doesn't have ghoul data");
					orig_SV_Nextserver();
					return;
				}
				//send the ghoul chunks.
				if ( ghoulChunkIndex[slot] < ghoulChunksReplay[ghoulSlot].size() ) {
					//netchan_t->reliable_length
					int rel_len = stget(chan,0x404c);
					//netchan_t->message.cursize
					//int cur_size = stget(chan,0x54);

					//If new reliable sent now, swap buffers.
					if ( !rel_len ) {

						initChunk_t& ghl = ghoulChunksReplay[ghoulSlot][ghoulChunkIndex[slot]];
						SOFPPNIX_DEBUG("Ghoul-Packet: %i/%i , Size = %i",ghoulChunkIndex[slot],ghoulChunksReplay[ghoulSlot].size(),ghl.len);
						orig_SZ_Write(&demo_rel,ghl.data,ghl.len);

						ghoulChunkIndex[slot]++;
					} 
					netchan_close_rel = true;
					my_Netchan_Transmit (chan, 0, NULL);
					
				} else {
					//all ghoul chunks sent
					ghoulLoaded[slot] = true;
					SOFPPNIX_DEBUG("Ghoul Sent to the viewer!");
				}

			} else { //ghouLoaded - begin Frames

				if ( current_frame > final_frame ) {
					//SOFPPNIX_DEBUG("NextServer/KillServer!");
					//orig_SV_Nextserver();

					//orig_Cmd_ExecuteString("killserver\n");
					//No more data to send.
					return;
				}

				//reliable is supposed to accumulate over time.
				//but must be careful it doesnt' accumulate non-demo data.
				//patch sv_multicast to prevent this, temporarily.

				//For now, disable accumulation, and have potentially

				//Unreliable data needs to be sent every 10 ticks...

				SlotPacketDataMap& slots_packetdata_map = demo_system.demos[spawncount].demo_frames_map[current_frame];
				slot_packetdata_t* targetPerspective = nullptr;
				//Player Perspective of this slot
				if ( watch_slot == -1 ) targetPerspective = slots_packetdata_map.begin()->second;
				else {
					auto itSlots = slots_packetdata_map.find(watch_slot);
					if (itSlots == slots_packetdata_map.end()) 
						targetPerspective = slots_packetdata_map.begin()->second;
					else
						targetPerspective = itSlots->second;
				}

				if (targetPerspective == nullptr) {
					// Can't send this client any data. Because can't the slot is not saved. Change demo mode to other slot?
					//orig_SV_Nextserver();
					error_exit("targetPerspective pointer missing");
				}
				#if 0
				if ( chan->message.cursize )
					hexdump(chan->message.data,chan->message.data + chan->message.cursize);
				#endif
				SOFPPNIX_DEBUG("Sending frame : %i",current_frame);

				#if 0
				//skip rel on delta frame.
				if ( current_frame != startDemoFrame )
					orig_SZ_Write(&chan->message,targetPerspective->relSZ->data,targetPerspective->relSZ->cursize);
				#else
				//if ( current_frame != startDemoFrame )
				orig_SZ_Write(&demo_rel,targetPerspective->relSZ->data,targetPerspective->relSZ->cursize);
				#endif

				if ( ghoulUnrelFrame == current_frame ) {
					SOFPPNIX_DEBUG("playback: Dumping ghl unrel first chunk");
					hexdump(targetPerspective->unrelSZ->data,targetPerspective->unrelSZ->data+targetPerspective->unrelSZ->cursize);
				}
				current_frame +=1;

				//netchan_t->reliable_length
				int rel_len = stget(chan,0x404c);

				netchan_close_rel = true;
				my_Netchan_Transmit(chan,targetPerspective->unrelSZ->cursize,targetPerspective->unrelSZ->data);
			}
		}
	}
}


DemoSystem demo_sytem;
DemoSystem::DemoSystem()
	: demo_recorder( std::make_unique<DemoRecorder>() ),
 		demo_player( std::make_unique<DemoPlayer>() ),
 		serverstate_trigger_play(false)
{
 			SOFPPNIX_DEBUG("DemoSystem Constructor");
}

/*
Append to the demo_system.demos map using svs.spawncount on each level load.
Another map that stores unique_ptr's to another map: SlotPacketDataMap.
*/
void DemoSystem::Initialise() {
	SOFPPNIX_DEBUG("Initiating DemoSystem");

}

DemosPerLevel* DemoSystem::getDemos() {
	return demos;
}
/*
create entry into demos with svs.spawncount as key.

Called by sv_spawnserver.
*/
void DemoSystem::PrepareLevel() {
	SOFPPNIX_DEBUG("Summoning recorder and player controls...")

	//server_static_t;
	void * svs = 0x082A2540;
	int spawncount = stget(svs,0x108);

	//Reset the unique_ptr's to get a clean object.
	//Provide spawncount to them.
	demo_recorder.reset(new DemoRecorder(spawncount));
	demo_player.reset();

	//demo playback activation. ++nix_demomap -> sv_spawnserver -> new -> initate.
	//protects our code from being ran during ss_demo, normal demomap.
	if ( serverstate_trigger_play )  {
		demo_player->begin();
	}

	//Data exists independently for each level. Should be enough to clear the index of the map for the deconstructors to cleanup. Because using vectors.
	DemoData data;
	demos[svs.spawncount] = data;
}

/*
		use the .dm3 extension format for files.
		else assumed spawncount selection integer id from memdump list.
	*/
void DemoSystem::LoadDemo() {
	SOFPPNIX_DEBUG("LoadDemo()");

	if ( orig_Cmd_Argc() != 2 ) {
		SOFPPNIX_PRINT("Specify a demoname.dm3 file or an ID from ++nix_match_history");
		return;
	}

	char * demoID = orig_Cmd_Argv(1);
}



int netchanToSlot(void * inChan)
{
	void *one = stget(0x0829D134,0);
	void * svs_clients = stget(one,0x10C);

	int clientAddr = (inChan - CLIENT_NETCHAN) - (int)svs_clients;

	if ( clientAddr % 0xd2ac ) return -1;
	return (clientAddr / 0xd2ac);
}


/*
	demo_recorder.demo_frames_map[frameNum] = new demo_frame_t;
	
	packet = new@slot_packetdata_t
	demo_recorder.demo_frames_map[frameNum]->slots_packetdata_map[slot] = packet;
	if relLen:
		new@sizebuf_t
		sizebuf_t->data = new@relLen
	if unrelLen:
		new@sizebuf_t
		sizebuf_t->data = new@relLen
*/
void storeDemoData(void * netchan, int relLen, unsigned char * relData, int unrelLen, unsigned char * unrelData)
{
	
	int slot = netchanToSlot(netchan);
	if ( slot == -1 ) error_exit("Cannot convert netchan to slot");

	void * client = getClientX(slot);
	int clientState = stget(client,0);

	//only save fully connected data.
	if ( clientState != cs_spawned ) {
		// SOFPPNIX_DEBUG("This client %i hasn't spawned yet.",slot);
		return;
	}

	int framenum = stget(0x082AF680, 0x10);

	auto itFrames = demo_recorder.demo_frames_map.find(framenum);

	if (itFrames == demo_recorder.demo_frames_map.end())
		demo_recorder.demo_frames_map[framenum] = std::make_unique<demo_frame_t>();
	else
		error_exit("frame already exists, why?");

	std::unique_ptr<slot_packetdata_t> packet = std::make_unique<slot_packetdata_t>();

	if (relLen) {
		packet->relSZ = std::make_unique<sizebuf_t>();
		packet->relSZ->data = std::make_unique<char[]>(relLen).release(); 
		packet->relSZ->maxsize = relLen;

		orig_SZ_Write(packet->relSZ.get(), packet->relSZ->data, relLen);
	}

	if (unrelLen) {
		packet->unrelSZ = std::make_unique<sizebuf_t>();
		packet->unrelSZ->data = std::make_unique<char[]>(unrelLen).release();
		packet->unrelSZ->maxsize = unrelLen;

		orig_SZ_Write(packet->unrelSZ.get(), packet->unrelSZ->data, unrelLen);
	}

	if ( ghoulUnrelFrame == framenum ) {
		SOFPPNIX_DEBUG("record: Dumping ghl unrel first chunk");
		hexdump(packet->unrelSZ->data,packet->unrelSZ->data+packet->unrelSZ->cursize);
	}
	demo_recorder.demo_frames_map[framenum]->slots_packetdata_map[slot] = packet;
}


/*
	Called by ++nix_record
*/

void clearDemoData(void)
{
	SOFPPNIX_DEBUG("clearDemoData!");

	for(int i = 0; i < 16; ++i) {
		for ( auto& chunk : ghoulChunksReplay[i] ) {
			if ( chunk.data != NULL ) free(chunk.data);
		}
		ghoulChunksReplay[i].clear();
		ghoulChunksSavedReplay[i] = false;
	}

	for(auto& frameIt : demo_recorder.demo_frames_map) {
		int framenum = frameIt.first;
		std::unique_ptr<demo_frame_t>& demoframe = frameIt.second;

		for(auto& slotIt : demoframe->slots_packetdata_map) {
			slot_packetdata_t * packet = slotIt.second;

			if ( packet->relSZ != nullptr ) {
				delete[] packet->relSZ->data;
			}
			if ( packet->unrelSZ != nullptr ) {
				delete[] packet->unrelSZ->data;
			}
		}
	}
	demo_recorder.demo_frames_map.clear();

	for ( auto& chunk : initialChunks ) {
		if ( chunk.data != NULL ) free(chunk.data);
	}
	initialChunks.clear();
}


void constructDemo()
{

}

/*
	connection data to kick start the client into the game.
*/
void storeServerData()
{

	sizebuf_t buf;
	//sizebuf_t memset to 0.
	memset(&buf,0,sizeof(sizebuf_t));

	char	buf_data[1400];

	//buffer
	buf.data = buf_data;
	buf.data[0] = 0x00;
	buf.maxsize = 1400;

	//server_static_t;
	void * svs = 0x082A2540;
	int spawncount = stget(svs,0x108);

	//server_t;
	void * sv = 0x082AF680;
	char * levelName = sv + 0x454;

	int index = 0;
	char * configstrings = sv+0x454;

	//packet type 0x0E
	orig_MSG_WriteByte(&buf,svc_serverdata);

	//proto 33
	orig_MSG_WriteLong(&buf,PROTOCOL_VERSION);

	//spawncount - this is a unique random number to detect if the client thinks its connecting to a previous map state.
	SOFPPNIX_DEBUG("Spawn Count = %i",spawncount);
	orig_MSG_WriteLong(&buf,spawncount);

	//attractloop type 2 = serverdemo 1 = clientdemo
	orig_MSG_WriteByte(&buf,1);

	//deathmatch
	SOFPPNIX_DEBUG("deathmtach  =%i",(int)(deathmatch->value));
	orig_MSG_WriteByte (&buf, (int)(deathmatch->value));

	//gamedir
	SOFPPNIX_DEBUG("gamedir = %s",orig_Cvar_VariableString("gamedir"));
	orig_MSG_WriteString(&buf,orig_Cvar_VariableString("gamedir"));

	//your slot num
	orig_MSG_WriteShort(&buf,0);

	//full level name
	SOFPPNIX_DEBUG("level name = %s",levelName);
	orig_MSG_WriteString(&buf,levelName);

	/*
	SV_New_f:
    MSG_WriteByte (&buf, svc_stufftext); 0x0d
    MSG_WriteString (&buf,"cmd configstrings *spawncount* 0\n")
	*/

// configstrings
	for (int i=0 ; i<0xde2 ; i++)
	{
		char * thisConfigstring = configstrings+64*i;
		if ( *thisConfigstring )
		{
			if (buf.cursize + strlen (thisConfigstring) + 32 > buf.maxsize)
			{	
				// write it out

				initChunk_t newchunk;
				newchunk.data = malloc(buf.cursize);
				newchunk.len = buf.cursize;

				memcpy(newchunk.data,buf_data,buf.cursize);

				buf.cursize = 0;
				initialChunks.push_back(newchunk);

				//Pointless stufftext to force client to send back faster than every second.
				orig_MSG_WriteByte(&buf, svc_stufftext);
				orig_MSG_WriteString(&buf,"cmd a\n");
			}

			orig_MSG_WriteByte (&buf, svc_configstring);
			orig_MSG_WriteShort (&buf, i);
			orig_WriteConfigString (&buf, thisConfigstring);
		}

	}

	#if 1
	// baselines
	entity_state_t	nullstate;
	entity_state_t	*base;
	memset (&nullstate, 0, sizeof(nullstate));
	for (int i=1; i<MAX_EDICTS ; i++)
	{
		//base = &sv.baselines[i];
		base = sv + 0x37D14 + i*0x74;
		//solid, renderindex
		//if (!base->renderindex && !((base->solid & 0xFF) & 0x40) )
			//continue;
		
		if ( !base->renderindex && !base->sound_data && !base->renderfx && !orig_HasAttachedEffects(base) )
			continue;
		//if (stget(base,0x118) & 0x1) continue;

		//base->solid & 0x40
		//if (base->modelindex || base->solid & 0x40) {
		//if ( base->number == i ) {
		SOFPPNIX_DEBUG("Number = %i",base->number);
		if (buf.cursize + 64 > buf.maxsize)
			{	// write it out
				
				initChunk_t newchunk;
				newchunk.data = malloc(buf.cursize);
				newchunk.len = buf.cursize;

				memcpy(newchunk.data,buf_data,buf.cursize);

				buf.cursize = 0;
				initialChunks.push_back(newchunk);
			}

			orig_MSG_WriteByte (&buf, svc_spawnbaseline);		
			orig_MSG_WriteDeltaEntity (&nullstate, base, &buf, true);


		}
	#endif

		orig_MSG_WriteByte (&buf, svc_stufftext);

	//allow_download 0? dodge downloads? necessary?
		orig_MSG_WriteString (&buf, orig_va("set allow_download 0;precache %i; reset_predn\n",spawncount));

	// write it out
		initChunk_t newchunk;
		newchunk.data = malloc(buf.cursize);
		newchunk.len = buf.cursize;

		memcpy(newchunk.data,buf_data,buf.cursize);

		buf.cursize = 0;
		initialChunks.push_back(newchunk);


		SOFPPNIX_DEBUG("End of storeServerData");
	}




void demoResetVars(int slot) {
	//ghoul data extracted from this player connecting...
	ghoulChunksSaved[slot] = false;

	for ( auto& chunk : ghoulChunks[slot] ) {
		if ( chunk.data != NULL ) free(chunk.data);
	}
	ghoulChunks[slot].clear();

	//during demo playback, progress of ghoul sending/loading..
	ghoulChunkIndex[slot] = 0;
	ghoulLoaded[slot] = false;
}

void demoResetVarsWeak(int slot) {
	//weak - if client disconnects, preserve full ghoul data.
	//else incomplete, remove.
	if ( !ghoulChunksSaved[slot] ) {
		ghoulChunksSaved[slot] = false;

		for ( auto& chunk : ghoulChunks[slot] ) {
			if ( chunk.data != NULL ) free(chunk.data);
		}
		ghoulChunks[slot].clear();
	}

	//during demo playback, progress of ghoul sending/loading..
	ghoulChunkIndex[slot] = 0;
	ghoulLoaded[slot] = false;
}