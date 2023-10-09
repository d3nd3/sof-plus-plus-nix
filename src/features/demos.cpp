#include "common.h"

/*
demo header
4 - frameNum
  1 - NumActiveNonSpecs
    1 - slotNum
    2 - relLen
    X - relData
    2 - unLen
    X - unData
4 - frameNum...

The functions aim to output this.  Need to store this data invidually in memory.

typedef struct demo_s {

	demo_frame_t 
} demo_t;
*/

bool playbackStatus = false;
bool recordingStatus = false;
char recordingName[MAX_TOKEN_CHARS];
bool thickdemo = false;
bool serverdemo = false;
int startDemoFrame = 0;
int currentDemoFrame = 0;
int finalDemoFrame = 0;
int prefferedFighter = -1;
bool demoWaiting = false; //force server to dispatch 1 uncompressed frame.

bool disableDefaultRelBuffer = false;


bool demoPlaybackInitiate = false;


sizebuf_t relAccumulate;
char	accum_buf[1400-16];

//every frame
std::map<int,demo_frame_t*> demoFrames;
//special data expected upon client connection.



std::vector<initChunk_t> initialChunks;

bool ghoulChunksSaved[16];
std::vector<initChunk_t> ghoulChunks[16];

//copied over upon stoprecord.
std::vector<initChunk_t> ghoulChunksReplay[16];
bool ghoulChunksSavedReplay[16];


//progress for each client/viewer
int ghoulChunkIndex[16];
bool ghoulLoaded[16];

int netchanToSlot(void * inChan)
{
	void *one = stget(0x0829D134,0);
	void * svs_clients = stget(one,0x10C);

	int clientAddr = (inChan - CLIENT_NETCHAN) - (int)svs_clients;

	if ( clientAddr % 0xd2ac ) return -1;
	return (clientAddr / 0xd2ac);
}


/*
Ensure don't recursive loop since the playback also calls netchan_transmit. The hook needs to be a specific netchan_transmit hook, not a generic one.

We might have to get the client from the netchan, thats the only way. Unless we hook earlier. But don't really want to.
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

	int frameNum = stget(0x082AF680,0x10);
	//SOFPPNIX_DEBUG("Slot is %i\nFramenum is %i",slot,frameNum);
	SOFPPNIX_DEBUG("storeDemoData! frame=%i relLen=%i unrelLen=%i",frameNum,relLen,unrelLen);

	auto itFrames = demoFrames.find(frameNum);
	// create frame if not exist
	if (itFrames == demoFrames.end())
		demoFrames[frameNum] = new demo_frame_t;
	else error_exit("frame already exist, why?");


	// refuse calling this multiple time for same slot/frame.
	auto itSlices = demoFrames[frameNum]->fighters.find(slot);
	if (itSlices != demoFrames[frameNum]->fighters.end()) error_exit("double netchan for slot : %i",slot); 
	
	fighter_slice_t* newSlice = new fighter_slice_t;


	// --------------------------------REL--------------------------------
	sizebuf_t * rel = malloc(sizeof(sizebuf_t));;
	//sizebuf_t memset to 0.
	memset(rel,0,sizeof(sizebuf_t));

	//buffer - caution can be 0 len
	rel->data = malloc(relLen);
	//rel->data[0] = 0x00;
	rel->maxsize = relLen;
	
	newSlice->relSZ = rel;

	//Write the data.
	//SOFPPNIX_DEBUG("BEFORE REL WRITE : relData=%08X : CURSIZE =%i : MAXSIZE=%i",relData,newSlice->relSZ->cursize, newSlice->relSZ->maxsize);
	//if ( relLen > 0 )
	orig_SZ_Write(newSlice->relSZ,relData,relLen);
	//SOFPPNIX_DEBUG("AFTER REL WRITE");

	// --------------------------------UNREL--------------------------------
	sizebuf_t * unrel = malloc(sizeof(sizebuf_t));
	//sizebuf_t memset to 0.
	memset(unrel,0,sizeof(sizebuf_t));

	//buffer - caution can be 0 len
	//unrel->data = new char[unrelLen];
	unrel->data = malloc(unrelLen);
	//unrel->data[0] = 0x00;
	unrel->maxsize = unrelLen;
	

	newSlice->unrelSZ = unrel;

	//Write the data.
	//SOFPPNIX_DEBUG("BEFORE UNREL WRITE : CURSIZE =%i : MAXSIZE=%i",newSlice->unrelSZ->cursize, newSlice->unrelSZ->maxsize);
	//if ( unrelLen > 0 )

	orig_SZ_Write(newSlice->unrelSZ,unrelData,unrelLen);

	if ( ghoulUnrelFrame == frameNum ) {
		SOFPPNIX_DEBUG("record: Dumping ghl unrel first chunk");
		hexdump(newSlice->unrelSZ->data,newSlice->unrelSZ->data+newSlice->unrelSZ->cursize);
	}

	//SOFPPNIX_DEBUG("AFTER UNREL WRITE");
	//save.
	demoFrames[frameNum]->fighters[slot] = newSlice;

	// SOFPPNIX_DEBUG("Slice %i:%i written",frameNum,slot);

	//SOFPPNIX_DEBUG("ENDEND");
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


	for(auto& frameIt : demoFrames) {
		int key = frameIt.first;
		demo_frame_t* demoframe = frameIt.second;


		for(auto& sliceIt : demoframe->fighters) {
			fighter_slice_t * slice = sliceIt.second;
			//delete slice->relSZ->data;
			free(slice->relSZ->data);
			free(slice->relSZ);

			//delete slice->unrelSZ->data;
			free(slice->unrelSZ->data);
			free(slice->unrelSZ);
		}

		demoframe->fighters.clear();
	}
	demoFrames.clear();

	for ( auto& chunk : initialChunks ) {
		if ( chunk.data != NULL ) free(chunk.data);
	}
	initialChunks.clear();
}

// does the key exist inside the unordered_maps?
fighter_slice_t * getDemoFrameFighterSlice()
{
	
	SOFPPNIX_DEBUG("getDemoFrameFighterSlice! currentDemoFrame : %i / %i",currentDemoFrame, finalDemoFrame);
	std::map<int,fighter_slice_t*>& fighters = demoFrames[currentDemoFrame]->fighters;

	//Player Perspective of this slot
	if ( prefferedFighter == -1 ) return fighters.begin()->second;
	else {
		auto itSlices = fighters.find(prefferedFighter);
		if (itSlices == fighters.end()) return fighters.begin()->second;
		return itSlices->second;
	}
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

/*
demoPlaybackInitiate - after New
*/
void demos_handlePlayback(netchan_t *chan, int length, byte *data)
{
	//SOFPPNIX_DEBUG("demos_handlePlayback");
	int slot = netchanToSlot(chan);
	if ( slot == -1 ) error_exit("Cannot convert netchan to slot");

	void * client = getClientX(slot);
	int clientState = stget(client,0);

	//connecting should be handled fine using netchan_outofband
	if ( clientState <= cs_zombie ) {
		//SOFPPNIX_DEBUG("state is : %i",stget(client,0));
		//ignore unconnected clients.
		return;
	}

	int frameNum = stget(0x082AF680,0x10);
	int last_sent = stget(chan,0x10);
	int curtime = stget(0x08526624,0);
	//do we have to something send?
	if ( !initialChunks.empty() /*&& !(frameNum % 6)*/ ) {
		
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

				auto first = initialChunks.begin();
				
				SOFPPNIX_DEBUG("Connect-Packet-Size = %i",first->len);

				orig_SZ_Write(&relAccumulate,first->data,first->len);

				initialChunks.erase(first);

				disableDefaultRelBuffer = true;
				my_Netchan_Transmit (chan, 0, NULL);
				chan->message.cursize = 0;
				
			} else {
				//attempt resend of lost packets
				if ( curtime - last_sent > 1000 ) {
					SOFPPNIX_DEBUG("1 Second passed...");
					disableDefaultRelBuffer = true;
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
					orig_SZ_Write(&relAccumulate,ghl.data,ghl.len);

					ghoulChunkIndex[slot]++;
				} 
				disableDefaultRelBuffer = true;
				my_Netchan_Transmit (chan, 0, NULL);
				
			} else {
				//all ghoul chunks sent
				ghoulLoaded[slot] = true;
				SOFPPNIX_DEBUG("Ghoul Sent to the viewer!");
			}

		} else { //ghouLoaded - begin Frames

			if ( currentDemoFrame > finalDemoFrame ) {
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
		
			fighter_slice_t * slice = getDemoFrameFighterSlice();
			if (slice == nullptr) {
				// Can't send this client any data. Because can't the slot is not saved. Change demo mode to other slot?
				//orig_SV_Nextserver();
				error_exit("Slice pointer missing");
			}
			#if 0
			if ( chan->message.cursize )
				hexdump(chan->message.data,chan->message.data + chan->message.cursize);
			#endif
			SOFPPNIX_DEBUG("Sending frame : %i",currentDemoFrame);

			#if 0
			//skip rel on delta frame.
			if ( currentDemoFrame != startDemoFrame )
				orig_SZ_Write(&chan->message,slice->relSZ->data,slice->relSZ->cursize);
			#else
			//if ( currentDemoFrame != startDemoFrame )
				orig_SZ_Write(&relAccumulate,slice->relSZ->data,slice->relSZ->cursize);
			#endif

			if ( ghoulUnrelFrame == currentDemoFrame ) {
				SOFPPNIX_DEBUG("playback: Dumping ghl unrel first chunk");
				hexdump(slice->unrelSZ->data,slice->unrelSZ->data+slice->unrelSZ->cursize);
			}
			currentDemoFrame +=1;

			//netchan_t->reliable_length
			int rel_len = stget(chan,0x404c);


			disableDefaultRelBuffer = true;
			my_Netchan_Transmit(chan,slice->unrelSZ->cursize,slice->unrelSZ->data);
		}
	}
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