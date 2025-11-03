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


Question: Can spawncount ever not match demo_recorder->start_spawncount?
Answer: Only by the few code that exists between the line that sets spawncount in SV_SpawnServer and the return of the function, before DemoSystem::PrepareLevel() is called. So, effectively, no.

Question: Is there a purpose for recording the spawncount in the recorder?
Answer: No, because the lifetime of the record and playback classes matches the spawncount lifetime.

DemoSystem::demo_system
	DemoRecorder demo_recorder -> deleted & created by DemoSystem::PrepareLevel(), called after SV_SpawnServer
	DemoPlayer demo_player
	DemoData demos[spawncount]
		DemoFramesMap demo_frames_map[framenum]
		DemoCheckpoint checkpoints[framenum]
		chunk_t activation_chunks[]
		chunk_t ghoul_chunks[]
		level_name
		deathmatch
		game_dir

*/


/*
-------------------------------------------------------------------------
-----------------------------DEMO CHECKPOINT-----------------------------
-------------------------------------------------------------------------
*/
DemoCheckpoint::DemoCheckpoint(int timestamp) :
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
void DemoCheckpoint::generate_svc_serverdata(int slot, std::vector<chunk_t>& resultVector) {

	sizebuf_t buf;
	//sizebuf_t memset to 0.
	memset(&buf,0,sizeof(sizebuf_t));
	char	buf_data[1400];

	//buffer
	buf.data = buf_data;
	buf.data[0] = 0x00;
	buf.maxsize = 1400;

	//server_t;
    void * sv = (void*)0x082AF680;
    char * levelName = (char*)((unsigned char*)sv + 0x454);

	int index = 0;
    char * configstrings = (char*)((unsigned char*)sv + 0x454);

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
                newchunk.data = (char*)malloc(buf.cursize);
				newchunk.len = buf.cursize;

				memcpy(newchunk.data,buf_data,buf.cursize);

				buf.cursize = 0;

				resultVector.push_back(newchunk);
				//demo_system.demos[spawncount].activation_chunks[slot].push_back(newchunk);

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
        base = (entity_state_t*)((unsigned char*)sv + 0x37D14 + i*0x74);
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
    newchunk.data = (char*)malloc(buf.cursize);
			newchunk.len = buf.cursize;

			memcpy(newchunk.data,buf_data,buf.cursize);

			buf.cursize = 0;
			resultVector.push_back(newchunk);
			// demo_system.demos[spawncount].activation_chunks[slot].push_back(newchunk);
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
    newchunk.data = (char*)malloc(buf.cursize);
    newchunk.len = buf.cursize;

	memcpy(newchunk.data,buf_data,buf.cursize);

	buf.cursize = 0;
	//demo_system.demos[spawncount].activation_chunks[slot].push_back(newchunk);
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
            if ( chunk.data != NULL ) free((void*)chunk.data);
		}
		activation_chunks[i].clear();

        for ( auto& chunk : ghoul_chunks[i] ) {
			//correctly free partial written chunk of previous client.
            if ( chunk.data != NULL ) free((void*)chunk.data);
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
                delete[] (packet->relSZ->data);
            }
            if ( packet->unrelSZ != nullptr ) {
                delete[] (packet->unrelSZ->data);
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
DemoRecorder::DemoRecorder()
{

}

DemoRecorder::~DemoRecorder()
{
	if ( demo_system.recording_status ) {
		endRecording();
	}
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
	SOFPPNIX_PRINT("Recording started (id %d)", spawncount);

	//DemoData already created by DemoSystem::PrepareLevel() (SV_SpawnServer)
	// Assuming 'checkpoints' is your unordered_map
    auto it = demo_system.demos.find(spawncount);
    if (it == demo_system.demos.end()) {
		//Created in DemoSystem::PrepareLevel() (SV_spawnServer)
		SOFPPNIX_DEBUG("Warning. The DemoData does not exist yet.");
		return;
	}

	//Checkpoint creation should be handled in sv_writeframe function. Not here.
	//Although triggering of the checkpoint creation can work here.
    // First checkpoint will be created by frame writer elsewhere
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
    auto itFrames = demo_system.demos[spawncount]->demo_frames_map.find(framenum);

    if (itFrames == demo_system.demos[spawncount]->demo_frames_map.end())
        demo_system.demos[spawncount]->demo_frames_map[framenum] = SlotPacketDataMap();
	else
		error_exit("frame already exists, why?");
	#endif

	std::unique_ptr<slot_packetdata_t> packet = std::make_unique<slot_packetdata_t>();

    if (relLen) {
        packet->relSZ = std::make_unique<sizebuf_t>();
        memset(packet->relSZ.get(), 0, sizeof(sizebuf_t));
        packet->relSZ->data = std::make_unique<char[]>(relLen).release(); 
        packet->relSZ->maxsize = relLen;
        packet->relSZ->cursize = 0;
        orig_SZ_Write(packet->relSZ.get(), relData, relLen);
    }

    if (unrelLen) {
        packet->unrelSZ = std::make_unique<sizebuf_t>();
        memset(packet->unrelSZ.get(), 0, sizeof(sizebuf_t));
        packet->unrelSZ->data = std::make_unique<char[]>(unrelLen).release();
        packet->unrelSZ->maxsize = unrelLen;
        packet->unrelSZ->cursize = 0;
        orig_SZ_Write(packet->unrelSZ.get(), unrelData, unrelLen);
    }

    demo_system.demos[spawncount]->demo_frames_map[framenum][slot] = std::move(packet);
}
/*
Called by ++nix_stoprecording or demo_system::preloadLevel (same place recording is started).

Ends the recording + writes to disk?
*/
void DemoRecorder::endRecording(void) {
	if ( !demo_system.recording_status ) {
		SOFPPNIX_PRINT("You must be recording to stop a recording.");
		return;
	}
	SOFPPNIX_DEBUG("Recording ended!");
	SOFPPNIX_PRINT("Recording ended (id %d). Use ++nix_demomap %d to play.", spawncount, spawncount);
	
	writeToDisk();
	demo_system.recording_status = false;
}

void DemoRecorder::onClientFullyConnected(void)
{

}
/*
	if end of recording mostly occurs at end of level, shouldn't be a performance issue.

	I was thinking of recording names to be auto created based on level name and timestamp.
	Perhaps it could be optional to provide a name.

	Providing a record command that doesn't write to disk might also be nice for temporary recordings, where the intention is to playback the demo one time only.
*/
void DemoRecorder::writeToDisk(void)
{
    auto it = demo_system.demos.find(spawncount);
    if (it == demo_system.demos.end()) return;
    std::unique_ptr<DemoData>& demo_data = it->second; 

    auto write_u32 = [](std::ofstream &f, uint32_t v){ f.write((const char*)&v, sizeof(v)); };
    auto write_str = [&](std::ofstream &f, const std::string &s){ uint32_t n=(uint32_t)s.size(); write_u32(f,n); if(n) f.write(s.data(), n); };
    auto strip_ext = [](const std::string &name){ size_t p=name.find_last_of('.'); if(p==std::string::npos) return name; return name.substr(0,p); };

    std::string out_path = std::string("demos/") + strip_ext(std::string(recording_name)) + ".ssdm";
    create_file_dir_if_not_exists(out_path.c_str());
    std::ofstream f(out_path, std::ios::binary|std::ios::out|std::ios::trunc);
    if (!f.is_open()) {
        SOFPPNIX_PRINT("failed to open %s", out_path.c_str());
        return;
    }

    f.write("SSDM1",5);
    write_str(f, demo_data->level_name);
    write_u32(f, (uint32_t)demo_data->deathmatch);
    write_str(f, demo_data->game_dir);

    int first = 0, last = 0;
    if (!demo_data->demo_frames_map.empty()) {
        first = demo_data->demo_frames_map.begin()->first;
        last  = demo_data->demo_frames_map.rbegin()->first;
    }
    write_u32(f, (uint32_t)first);
    write_u32(f, (uint32_t)last);

    write_u32(f, (uint32_t)demo_data->checkpoints.size());
    for (auto &cpkv : demo_data->checkpoints) {
        write_u32(f, (uint32_t)cpkv.first);
        write_u32(f, (uint32_t)cpkv.second->timestamp);
    }

    write_u32(f, (uint32_t)demo_data->demo_frames_map.size());
    for (auto &frkv : demo_data->demo_frames_map) {
        int fr = frkv.first;
        SlotPacketDataMap &slots = frkv.second;
        write_u32(f, (uint32_t)fr);
        write_u32(f, (uint32_t)slots.size());
        for (auto &skv : slots) {
            int slot = skv.first;
            slot_packetdata_t *pkt = skv.second.get();
            write_u32(f, (uint32_t)slot);
            uint32_t rlen = (pkt->relSZ && pkt->relSZ->cursize > 0 ? (uint32_t)pkt->relSZ->cursize : 0u);
            write_u32(f, rlen);
            if (rlen) f.write(pkt->relSZ->data, rlen);
            uint32_t ulen = (pkt->unrelSZ && pkt->unrelSZ->cursize > 0 ? (uint32_t)pkt->unrelSZ->cursize : 0u);
            write_u32(f, ulen);
            if (ulen) f.write(pkt->unrelSZ->data, ulen);
        }
    }

    f.close();
    SOFPPNIX_PRINT("saved demo to %s", out_path.c_str());
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
    if (demo_system.demos.find(spawncount) == demo_system.demos.end() || demo_system.demos[spawncount]->demo_frames_map.empty()) {
        SOFPPNIX_DEBUG("Demo has no frames");
        active = false;
        return;
    }
    first_frame = demo_system.demos[spawncount]->demo_frames_map.begin()->first;
    current_frame = first_frame;
    final_frame = demo_system.demos[spawncount]->demo_frames_map.rbegin()->first;
	SOFPPNIX_DEBUG("Starting demo on frame : %i / %i",first_frame, final_frame);
    paused = false;
    frame_step = 1;
    bookmarks.clear();

    // Reset per-viewer progress arrays
    for (int i = 0; i < 16; ++i) {
        ghoulChunkIndex[i] = 0;
        ghoulLoaded[i] = false;
    }

}

/*
demo_system.demo_player->packet_override - after New
*/
void DemoPlayer::playback(netchan_t *chan, int length, byte *data)
{
    
    if (!demo_system.demos.count(spawncount) || !demo_system.demos[spawncount]) return;
	int slot = netchanToSlot(chan);
	if ( slot == -1 ) error_exit("Cannot convert netchan to slot");
    if (slot < 0 || slot >= 16) return;

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
	
    std::vector<chunk_t>& start_chunks = demo_system.demos[spawncount]->activation_chunks[slot];
    if ( start_chunks.empty() ) {
        // Build activation chunks from nearest checkpoint (or synthesize from current server state)
        auto &demoData = demo_system.demos[spawncount];
        int cpFrame = current_frame;
        if (!demoData->checkpoints.empty()) {
            int best = demo_player->first_frame; // fallback
            for (const auto &kv : demoData->checkpoints) {
                int fr = kv.first; if (fr <= current_frame && fr >= best) best = fr;
            }
            cpFrame = best;
        }
        DemoCheckpoint cp(0);
        cp.generate_svc_serverdata(slot, start_chunks);
    }
	if ( !start_chunks.empty() ) {
		
		/*
			client response rate was low whilst connecting. patched it.
		*/

			//netchan_t->reliable_length
		int rel_len = stget(chan,0x404c);
			//netchan_t->message.cursize
		int cur_size = stget(chan,0x54);

        // POC: send initial chunk as an unreliable payload
        SOFPPNIX_DEBUG("Sending initialChunk");
        auto first = start_chunks.begin();
        SOFPPNIX_DEBUG("Connect-Packet-Size = %i",first->len);
        my_Netchan_Transmit (chan, first->len, (byte*)first->data);
        if ( first->data ) free(first->data);
        start_chunks.erase(first);
	}//initChunks empty
	else {
		//normal svc_serverdata svc_frame etc...
        // SOFPPNIX_DEBUG("SPAWNED...");

		if ( !ghoulLoaded[slot] ) {
			// POC: skip ghoul phase if none recorded
			bool anyGhoul = false;
			for ( int i = 0; i<16;i++ ){
				if ( !demo_system.demos[spawncount]->ghoul_chunks[i].empty() ) { anyGhoul = true; break; }
			}
			if ( !anyGhoul ) {
				ghoulLoaded[slot] = true;
			} else {
				//send the ghoul chunks if present
				int ghoulSlot = 0;
				for ( int i = 0; i<16;i++ ){
					if ( !demo_system.demos[spawncount]->ghoul_chunks[i].empty() ) { ghoulSlot = i; break; }
				}
                if ( ghoulChunkIndex[slot] < (int)demo_system.demos[spawncount]->ghoul_chunks[ghoulSlot].size() ) {
                    // POC: send ghoul chunk as an unreliable payload
                    chunk_t& ghl = demo_system.demos[spawncount]->ghoul_chunks[ghoulSlot][ghoulChunkIndex[slot]];
                    my_Netchan_Transmit (chan, ghl.len, (byte*)ghl.data);
                    ghoulChunkIndex[slot]++;
				} else {
					ghoulLoaded[slot] = true;
				}
			}

		} else { // ghouLoaded - begin Frames

			if ( current_frame > final_frame ) {
                if ( loop ) {
                    current_frame = first_frame;
                } else {
                    // No more data to send
                    return;
                }
			}

            auto itFrame = demo_system.demos[spawncount]->demo_frames_map.find(current_frame);
            if (itFrame == demo_system.demos[spawncount]->demo_frames_map.end() || itFrame->second.empty()) {
                // No data for this frame; advance cautiously
                current_frame += frame_step;
                return;
            }
            SlotPacketDataMap& slots_packetdata_map = itFrame->second;
            slot_packetdata_t* targetPerspective = nullptr;
            // Auto POV cycling per viewer if configured
            if (demo_viewer_auto_interval_ms[slot] > 0) {
                int curtime = stget(0x08526624,0);
                if (demo_viewer_last_cycle_ms[slot] == 0) demo_viewer_last_cycle_ms[slot] = curtime;
                if (curtime - demo_viewer_last_cycle_ms[slot] >= demo_viewer_auto_interval_ms[slot]) {
                    std::vector<int> keys; keys.reserve(slots_packetdata_map.size());
                    for (const auto &kv : slots_packetdata_map) keys.push_back(kv.first);
                    std::sort(keys.begin(), keys.end());
                    int curWatch = demo_viewer_watch_slot[slot];
                    if (curWatch == -1 && !keys.empty()) curWatch = keys.front();
                    auto pos = std::find(keys.begin(), keys.end(), curWatch);
                    if (pos == keys.end() || ++pos == keys.end()) demo_viewer_watch_slot[slot] = keys.front();
                    else demo_viewer_watch_slot[slot] = *pos;
                    demo_viewer_last_cycle_ms[slot] = curtime;
                }
            }

            // Per-viewer POV selection
            int desiredWatch = demo_viewer_watch_slot[slot];
            if ( desiredWatch == -1 ) {
                int minSlot = INT_MAX;
                for (const auto &kv : slots_packetdata_map) {
                    if (kv.first < minSlot) minSlot = kv.first;
                }
                if (minSlot != INT_MAX) targetPerspective = slots_packetdata_map[minSlot].get();
            } else {
                auto itSlots = slots_packetdata_map.find(desiredWatch);
                if (itSlots == slots_packetdata_map.end()) {
                    int minSlot = INT_MAX;
                    for (const auto &kv : slots_packetdata_map) {
                        if (kv.first < minSlot) minSlot = kv.first;
                    }
                    if (minSlot != INT_MAX) targetPerspective = slots_packetdata_map[minSlot].get();
                } else {
                    targetPerspective = itSlots->second.get();
                }
            }

			if (targetPerspective == nullptr) {
				// No perspective available for this frame; skip forward
				current_frame += frame_step;
				return;
			}

			#if 0
			if ( chan->message.cursize )
				hexdump(chan->message.data,chan->message.data + chan->message.cursize);
			#endif

            if ( paused ) {
                return;
            }
            // SOFPPNIX_DEBUG("Sending frame : %i",current_frame);

            // Send unrel if present; otherwise fall back to rel
            if (targetPerspective->unrelSZ && targetPerspective->unrelSZ->cursize > 0) {
				my_Netchan_Transmit(chan,targetPerspective->unrelSZ->cursize,targetPerspective->unrelSZ->data);
            } else if (targetPerspective->relSZ && targetPerspective->relSZ->cursize > 0) {
				my_Netchan_Transmit(chan,targetPerspective->relSZ->cursize,targetPerspective->relSZ->data);
            } else {
                // No payload for this perspective; advance and try next
                current_frame += frame_step;
                return;
            }

            // optional debug dump of first unrel chunk can be added via cvar
            current_frame += frame_step;
		}
	}
}


DemoSystem demo_system;
int demo_viewer_watch_slot[32] = { -1 };
bool demo_viewer_freecam[32] = { false };
int demo_viewer_auto_interval_ms[32] = { 0 };
int demo_viewer_last_cycle_ms[32] = { 0 };
DemoSystem::DemoSystem()
: demo_recorder( std::make_unique<DemoRecorder>() ),
demo_player( std::make_unique<DemoPlayer>() ),
serverstate_trigger_play(false),
recording_status(false)
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

	// Reset the unique_ptr's to get a clean object.
	demo_recorder.reset(new DemoRecorder());
	demo_player.reset(new DemoPlayer());

	// demo playback activation. ++nix_demomap -> sv_spawnserver -> new -> initate.
	// protects our code from being ran during ss_demo, normal demomap.
	if ( serverstate_trigger_play )  {
		// Move selected demo data (recorded or loaded) to current spawncount for playback
        if ( selected_demo_spawncount != -1 ) {
			auto it = demos.find(selected_demo_spawncount);
			if ( it != demos.end() ) {
				// Move ownership into the new spawncount key
				demos[spawncount] = std::move(it->second);
                // Remove the old placeholder key
                demos.erase(it);
			}
			selected_demo_spawncount = -1;
		}
		// Begin playback if we have any data
		demo_player->begin();
	}

	//server_t;
	void * sv = 0x082AF680;
	char * levelName = sv + 0x454;
	// Load data vs Save data. Recording vs Playback.
	// Only create fresh demo storage when NOT in playback path
	if ( !serverstate_trigger_play ) {
		// Data exists independently for each level. Should be enough to clear the index of the map for the deconstructors to cleanup. Because using vectors.
		// Record some form of the level every round (ghoul_reliable may be stored here)
		demos[spawncount] = std::make_unique<DemoData>(levelName,(int)(deathmatch->value),orig_Cvar_VariableString((char*)"gamedir"));
	}

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
        SOFPPNIX_PRINT("usage: ++nix_demomap <id|file>");
        return false;
    }

    char * arg = orig_Cmd_Argv(1);

    // numeric id -> pick in-memory demo
    bool all_digits = true;
    for ( char *p = arg; *p; ++p ) { if (*p < '0' || *p > '9') { all_digits = false; break; } }
    if ( all_digits ) {
        int id = atoi(arg);
        if ( demos.find(id) == demos.end() ) {
            SOFPPNIX_PRINT("no in-memory demo id %d", id);
            return false;
        }
        selected_demo_spawncount = id;
        return true;
    }

    // file-based load (.ssdm) - lightweight container
    std::string path = arg;
    if (path.find('/') == std::string::npos) path = std::string("demos/") + path;
    if (path.find('.') == std::string::npos) path += ".ssdm";

    std::ifstream f(path, std::ios::binary|std::ios::in);
    if (!f.is_open()) {
        SOFPPNIX_PRINT("cannot open demo file: %s", path.c_str());
        return false;
    }
    char magic[5];
    f.read(magic,5);
    if (f.gcount()!=5 || strncmp(magic,"SSDM1",5)!=0) {
        SOFPPNIX_PRINT("unsupported demo file: %s", path.c_str());
        return false;
    }
    auto read_u32 = [](std::ifstream &fin, uint32_t &v){ fin.read((char*)&v,sizeof(v)); return (bool)fin; };
    auto read_str = [&](std::ifstream &fin, std::string &out){ uint32_t n=0; if(!read_u32(fin,n)) return false; out.resize(n); if(n) fin.read(&out[0], n); return (bool)fin; };

    std::string level_name; std::string game_dir;
    uint32_t dm=0, first=0, last=0;
    if (!read_str(f, level_name)) { SOFPPNIX_PRINT("malformed SSDM: level_name"); return false; }
    if (!read_u32(f, dm)) { SOFPPNIX_PRINT("malformed SSDM: deathmatch"); return false; }
    if (!read_str(f, game_dir)) { SOFPPNIX_PRINT("malformed SSDM: game_dir"); return false; }
    if (!read_u32(f, first)) { SOFPPNIX_PRINT("malformed SSDM: first_frame"); return false; }
    if (!read_u32(f, last)) { SOFPPNIX_PRINT("malformed SSDM: last_frame"); return false; }

    auto data = std::make_unique<DemoData>(level_name,(int)dm,game_dir);

    // checkpoints
    uint32_t cpcount=0; if(!read_u32(f,cpcount)) { SOFPPNIX_PRINT("malformed SSDM: checkpoint_count"); return false; }
    for (uint32_t i=0;i<cpcount;i++) {
        uint32_t fr=0, ts=0; if(!read_u32(f,fr)) { SOFPPNIX_PRINT("malformed SSDM: checkpoint frame"); return false; } if(!read_u32(f,ts)) { SOFPPNIX_PRINT("malformed SSDM: checkpoint timestamp"); return false; }
        auto cp = std::make_unique<DemoCheckpoint>((int)ts);
        data->checkpoints[(int)fr] = std::move(cp);
    }

    // frames
    uint32_t fcount=0; if(!read_u32(f,fcount)) { SOFPPNIX_PRINT("malformed SSDM: frame_count"); return false; }
    for (uint32_t i=0;i<fcount;i++) {
        uint32_t fr=0, slotsz=0; if(!read_u32(f,fr)) { SOFPPNIX_PRINT("malformed SSDM: frame id"); return false; } if(!read_u32(f,slotsz)) { SOFPPNIX_PRINT("malformed SSDM: slot_count"); return false; }
        for (uint32_t s=0;s<slotsz;s++) {
            uint32_t slot=0; if(!read_u32(f,slot)) { SOFPPNIX_PRINT("malformed SSDM: slot id"); return false; }
            auto pkt = std::make_unique<slot_packetdata_t>();
            // rel
            uint32_t rlen=0; if(!read_u32(f,rlen)) { SOFPPNIX_PRINT("malformed SSDM: rel_len"); return false; }
            if (rlen) {
                pkt->relSZ = std::make_unique<sizebuf_t>();
                memset(pkt->relSZ.get(), 0, sizeof(sizebuf_t));
                pkt->relSZ->data = new char[rlen];
                pkt->relSZ->maxsize = (int)rlen;
                pkt->relSZ->cursize = (int)rlen;
                f.read(pkt->relSZ->data, rlen);
            }
            // unrel
            uint32_t ulen=0; if(!read_u32(f,ulen)) { SOFPPNIX_PRINT("malformed SSDM: unrel_len"); return false; }
            if (ulen) {
                pkt->unrelSZ = std::make_unique<sizebuf_t>();
                memset(pkt->unrelSZ.get(), 0, sizeof(sizebuf_t));
                pkt->unrelSZ->data = new char[ulen];
                pkt->unrelSZ->maxsize = (int)ulen;
                pkt->unrelSZ->cursize = (int)ulen;
                f.read(pkt->unrelSZ->data, ulen);
            }
            data->demo_frames_map[(int)fr][(int)slot] = std::move(pkt);
        }
    }

    f.close();

    int temp_id = -2; // reserved key for loaded-from-disk demo
    demos[temp_id] = std::move(data);
    selected_demo_spawncount = temp_id;
    SOFPPNIX_PRINT("loaded demo: %s", path.c_str());
    return true;
}