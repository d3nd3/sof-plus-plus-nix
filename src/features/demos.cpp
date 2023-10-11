#include "common.h"

/*
TODO: Some variables like trigger_checkpoint will need to be arrays for each slot. Decide which.

my_GhoulPackReliable() - saves the ghoulChunks.

my_SV_ExecuteClientMessage() - if (trigger_checkpoint) sets cl->lastframe = -1

my_SV_SendClientDatagram() - disables trigger_checkpoint because frame is sent.

my_SV_New_f() - if ( demo_system.demo_player->active ) demo_system.demo_player->packet_override = true;

my_SV_SpawnServer - demo_system.PrepareLevel()

my_Netchan_Transmit - if ( demo_system.demo_player->netchan_close_rel )

my_Netchan_Transmit_Playback - demo_system.demo_player->playback(chan,length,data);

my_Netchan_Transmit_Save - demo_system.demo_recorder->saveNetworkData(chan,relLen,relBuf,length,data);

cmd_nix_stoprecord - demo_system.demo_recorder.endRecording();

cmd_nix_record - demo_system.demo_recorder.startRecording();


*/


/*
-------------------------------------------------------------------------
-----------------------------DEMO CHECKPOINT-----------------------------
-------------------------------------------------------------------------
*/
DemoCheckPoint::DemoCheckpoint(int timestamp) :
timestamp(timestamp)
{
	/*
		create the demo checkpoint.
	*/
}

/*
	Called when a client starts connecting in demo playback().
	If the `int activation_index` is -1, generate the vector. And begin iterating it, frame by frame.

	The dynamic data (configstrings/baselines) used to generate these chunks is saved by the checkpoint system.  The checkpoint system will also ensure that every Unrel frameXX will be undelta'ed, meaning that players can easily JUMP to different frames of the demo.
*/
void DemoCheckpoint::generate_svc_serverdata(std::vector<chunk_t>& resultVector) {

	sizebuf_t buf;
	//sizebuf_t memset to 0.
	memset(&buf,0,sizeof(sizebuf_t));
	char	buf_data[1400];

	//buffer
	buf.data = buf_data;
	buf.data[0] = 0x00;
	buf.maxsize = 1400;

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
	SOFPPNIX_DEBUG("Spawn Count = %i",start_spawncount);
	orig_MSG_WriteLong(&buf,start_spawncount);

	//attractloop type 2 = serverdemo 1 = clientdemo
	orig_MSG_WriteByte(&buf,1);

	//deathmatch
	SOFPPNIX_DEBUG("deatch  =%i",(int)(deathmatch->value));
	orig_MSG_WriteByte (&buf, (int)(deathmatch->value));

	//gamedir
	SOFPPNIX_DEBUG("gamedir = %s",orig_Cvar_VariableString("gamedir"));
	orig_MSG_WriteString(&buf,orig_Cvar_VariableString("gamedir"));

	//your slot num
	orig_MSG_WriteShort(&buf,slot);

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

				chunk_t newchunk;
				newchunk.data = malloc(buf.cursize);
				newchunk.len = buf.cursize;

				memcpy(newchunk.data,buf_data,buf.cursize);

				buf.cursize = 0;

				resultVector.push_back(newchunk);
				//demo_system.demos[start_spawncount].activation_chunks[slot].push_back(newchunk);

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
			
			chunk_t newchunk;
			newchunk.data = malloc(buf.cursize);
			newchunk.len = buf.cursize;

			memcpy(newchunk.data,buf_data,buf.cursize);

			buf.cursize = 0;
			resultVector.push_back(newchunk);
			// demo_system.demos[start_spawncount].activation_chunks[slot].push_back(newchunk);
		}

		orig_MSG_WriteByte (&buf, svc_spawnbaseline);		
		orig_MSG_WriteDeltaEntity (&nullstate, base, &buf, true);


	}
	#endif

	orig_MSG_WriteByte (&buf, svc_stufftext);

	//allow_download 0? dodge downloads? necessary?
	orig_MSG_WriteString (&buf, orig_va("set allow_download 0;precache %i; reset_predn\n",spawncount));

	// write it out
	chunk_t newchunk;
	newchunk.data = malloc(buf.cursize);
	newchunk.len = buf.cursize;

	memcpy(newchunk.data,buf_data,buf.cursize);

	buf.cursize = 0;
	//demo_system.demos[start_spawncount].activation_chunks[slot].push_back(newchunk);
	resultVector.push_back(newchunk);


	SOFPPNIX_DEBUG("End of storeServerData");
}


/*
-------------------------------------------------------------------------
-----------------------------DEMO DATA-----------------------------------
-------------------------------------------------------------------------
*/

DemoData::DemoData(std::string level_name,int deathmatch, std::string game_dir) :
level_name(level_name),deathmatch(deathmatch),game_dir(game_dir)
{
	SOFPPNIX_DEBUG("DemoData() Constructor.");
}
DemoData::~DemoData() {
	for ( int i=0;i<16;i++ ) {
		for ( auto& chunk : activation_chunks[i] ) {
			//correctly free partial written chunk of previous client.
			if ( chunk.data != NULL ) free(chunk.data);
		}
		activation_chunks[i].clear();

		for ( auto& chunk : ghoul_chunks[i] ) {
			//correctly free partial written chunk of previous client.
			if ( chunk.data != NULL ) free(chunk.data);
		}
		ghoul_chunks[i].clear();
	}

	//map of frames->map of slots->packetdata
	for(auto& frameIt : demo_frames_map) {
		int framenum = frameIt.first;
		SlotPacketDataMap& slotPacketMap = frameIt.second;

		for(auto& slotIt : slotPacketMap) {
			std::unique_ptr<slot_packetdata_t>& packet = slotIt.second;

			if ( packet->relSZ != nullptr ) {
				delete[] packet->relSZ->data;
			}
			if ( packet->unrelSZ != nullptr ) {
				delete[] packet->unrelSZ->data;
			}
		}
	}
	demo_frames_map.clear(); //trigger unique_ptr chain cleanup.
}

/*
---------------------------------------------------------------------------
-----------------------------DEMO RECORDER---------------------------------
---------------------------------------------------------------------------
*/
DemoRecorder::DemoRecorder(int spawncount) : start_spawncount(spawncount){
}


/*
SVC_DirectConnect - builds sv_client , client_t.

Called by cmd_nix_record()
*/
void DemoRecorder::startRecording(void) {
	if ( orig_Cmd_Argc() != 2 ) {
		SOFPPNIX_PRINT("Specify demo name");
		return;
	}
	//not rec
	if ( demo_system.recording_status ) {
		SOFPPNIX_PRINT("You are already recording.");
		return;
	}
	//not playback
	int serverstate = stget(0x082AF680,0);
	if ( serverstate == 3 ) {
		SOFPPNIX_PRINT("Cannot record during demo playback.");
		return;
	}
	snprintf(recording_name,MAX_TOKEN_CHARS,"%s.dm2",orig_Cmd_Argv(1));
	demo_system.recording_status = true;
	//all other data saved by netchan_transmit.
	SOFPPNIX_DEBUG("Recording demoname : %s!",recording_name);

	//DemoData already created by DemoSystem::PrepareLevel() (SV_SpawnServer)
	// Assuming 'checkpoints' is your unordered_map
	auto it = demos.find(start_spawncount);
	if (it == demos.end()) {
		//Created in DemoSystem::PrepareLevel() (SV_spawnServer)
		SOFPPNIX_DEBUG("Warning. The DemoData does not exist yet.");
		return;
	}

	//Checkpoint creation should be handled in sv_writeframe function. Not here.
	//Although triggering of the checkpoint creation can work here.
	demos[start_spawncount]->checkpoints[]
	DemoData.checkpoints[framenum] = std::make_unique<DemoCheckpoint>(/* constructor arguments */);
	/*
		Essentially the first checkpoint.
	*/

	for ( int i = 0;i < 16; i++ ) {
		trigger_checkpoint[i] = true;
		//this will trigger all slots to be sent a base frame.
		//which is recorded on its way out.

		//all base frames will also be checkpoints. so this mechanism has to be repeated often.
		#if 0
		int state = *(int*)(getClientX(i));
		if ( state == cs_spawned ) {
			//saves initchunks
		}
		#endif
	}
	

}


void DemoRecorder::saveNetworkData(void * netchan, int relLen, unsigned char * relData, int unrelLen, unsigned char * unrelData)
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

	#if 0
	auto itFrames = demo_system.demos[spawncount].demo_frames_map.find(framenum);

	if (itFrames == demo_system.demos[spawncount].demo_frames_map.end())
		demo_system.demos[spawncount].demo_frames_map[framenum] = std::make_unique<demo_frame_t>();
	else
		error_exit("frame already exists, why?");
	#endif

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

	demo_system.demos[spawncount].demo_frames_map[framenum][slot] = std::move(packet);
}

void DemoRecorder::endRecording(void) {
	if ( !demo_system.recording_status ) {
		SOFPPNIX_PRINT("You must be recording to stop a recording.");
		return;
	}
	SOFPPNIX_DEBUG("Recording ended!");
	
	demo_system.recording_status = false;
}

void DemoRecorder::onClientFullyConnected(void)
{

}

/*
	connection data to kick start the client into the game.

	Instead of store it, I want to generate it for every incoming client.

	Hmmm, this is problematic.  This data and the first base frame. Are fixed in time.
	From the moment the recording started.

	A client who connects would not be in sync with the other clients, unless they were fed every packet since the start, which would probably crash their clients.

	One option could be to never use delta frames, but that would cause a lot of lag and increased network usage. So seems bad idea.

	Another option could be checkpoints.  If we force undelta frame every X frames. We could effectively sync all clients together to nearest checkpoint.  This also gives benefit of rewind etc..
*/
void DemoRecorder::create_checkpoint(int slot)
{
	/*
		When no clients are in the server, no network data is sent. And so the frame's do not "roll".

		The frames start rolling after they are sent.

		This is a recorder function, it is marking the point that the demo was recorded. So for the first frame of the demo.  However, we should 'save data that can generate this output' every 'checkpoint', so that when a new player starts connecting, he is given a previous checkpoint as the state. When the player fully connects, the other player's frame is reset back to the checkpoint to keep everyone in sync.

		This function should be renamed to 'save checkpoint'
	*/


	/*
		Ingredients:
			Persistent to spawncount (now stored in DemoData)
				LevelName
				DeathMatch
				GameDir
			Persistent to checkpoint [key=Slotnum]
				ConfigStrings
				BaseLines
	*/

}


/*
-------------------------------------------------------------------------
-----------------------------DEMO PLAYER---------------------------------
-------------------------------------------------------------------------
*/

DemoPlayer::DemoPlayer()
{
	SOFPPNIX_DEBUG("DemoPlayer Constructor()");
}
void DemoPlayer::begin(void) {
	/*
		Called by demo_system.PrepareLevel()
		We should have already loaded the file from disk. Validated it.
		Load from disk vs load from memory.

		Create a command to list the previous 10 entries that exist in the demos map, with their corresponding mapname and time existed.

		demomap 5435456

		demomap pastdemo
	*/

	//start_spawncount

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
	first_frame = demo_system.demos[spawncount].demo_frames_map.begin()->first;
	current_frame = startDemoFrame;
	final_frame = demo_system.demos[spawncount].demo_frames_map.rbegin()->first;
	SOFPPNIX_DEBUG("Starting demo on frame : %i / %i",first_frame, final_frame);

}

/*
demo_system.demo_player->packet_override - after New
*/
void DemoPlayer::playback(netchan_t *chan, int length, byte *data)
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
	
	std::vector<chunk_t>& start_chunks = demo_system.demos[spawncount].demo_frames_map[frameNum].activation_chunks;
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
			orig_SZ_Write(&demo_rel,targetPerspective->relSZ->data,targetPerspective->relSZ->cursize);

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


DemoSystem demo_system;
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

/*
create entry into demos with svs.spawncount as key.

Called by sv_spawnserver.
*/
void DemoSystem::PrepareLevel() {
	SOFPPNIX_DEBUG("Summoning recorder and player controls...")

	//Reset the unique_ptr's to get a clean object.
	//Provide spawncount to them.
	demo_recorder.reset(new DemoRecorder(spawncount));
	demo_player.reset();

	//demo playback activation. ++nix_demomap -> sv_spawnserver -> new -> initate.
	//protects our code from being ran during ss_demo, normal demomap.
	if ( serverstate_trigger_play )  {
		demo_player->begin();
	}

	//server_t;
	void * sv = 0x082AF680;
	char * levelName = sv + 0x454;
	//Load data vs Save data. Recording vs Playback.

	//Data exists independently for each level. Should be enough to clear the index of the map for the deconstructors to cleanup. Because using vectors.
	
	//Apparently we are going to attempt record some form of the level every round despite intentions. (partly because ghoul_reliable is stored in there. plus why not.)
	demos[spawncount] = std::make_unique<DemoData>(levelName,(int)(deathmatch->value),orig_Cvar_VariableString("gamedir"));

	//This unordered_map can be cleared() if memory becomes an issue.
	//The DemoData.demo_frames_map variable does not have to be filled until actual recording begins, obviously. Just prepares DemoData.ghoul_chunks
}

/*
	use the .dm3 extension format for files.
	else assumed spawncount selection integer id from memdump list.

	Called by cmd_nix_demomap()
*/
bool DemoSystem::LoadDemo() {
	SOFPPNIX_DEBUG("LoadDemo()");

	if ( orig_Cmd_Argc() != 2 ) {
		SOFPPNIX_PRINT("Specify a demoname.dm3 file or an ID from ++nix_match_history");
		return;
	}

	char * demoID = orig_Cmd_Argv(1);

	//Should access the demos[spawncount] or create a DemoData fresh from a file.
}