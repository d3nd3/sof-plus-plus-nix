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

bool recordingStatus = false;
char recordingName[MAX_TOKEN_CHARS];
bool thickdemo = false;
int startDemoFrame = 0;
int currentDemoFrame = 0;
int finalDemoFrame = 0;
int prefferedFighter = -1;
bool demoWaiting = false; //force server to dispatch 1 uncompressed frame.

bool demoPlaybackInitiate = false;
//every frame
std::map<int,demo_frame_t*> demoFrames;
//special data expected upon client connection.

typedef struct initChunk_s {
	char * data;
	int len;
} initChunk_t;

std::vector<initChunk_t> initialChunks;

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
	//SOFPPNIX_DEBUG("storeDemoData! relLen=%i unrelLen=%i",relLen,unrelLen);
	int slot = netchanToSlot(netchan);
	if ( slot == -1 ) error_exit("Cannot convert netchan to slot");

	void * client = getClientX(slot);
	int clientState = stget(client,0);

	//only save fully connected data.
	if ( clientState != cs_spawned ) return;

	int frameNum = stget(0x082AF680,0x10);
	//SOFPPNIX_DEBUG("Slot is %i\nFramenum is %i",slot,frameNum);

	auto itFrames = demoFrames.find(frameNum);
	// create frame if not exist
	if (itFrames == demoFrames.end())
		demoFrames[frameNum] = new demo_frame_t;


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
	//SOFPPNIX_DEBUG("AFTER UNREL WRITE");
	//save.
	demoFrames[frameNum]->fighters[slot] = newSlice;

	//SOFPPNIX_DEBUG("ENDEND");
}

void clearDemoData(void)
{
	SOFPPNIX_DEBUG("clearDemoData!");
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

/*
	Reads and Increments 'currentDemoFrame'.

	Called by my_Netchan_Transmit_Playback in exe_shared.cpp
*/
sizebuf_t * restoreNetworkBuffers(netchan_t* chan)
{
	SOFPPNIX_DEBUG("restoreNetworkBuffers!");

	fighter_slice_t * slice = getDemoFrameFighterSlice();
	if (slice == nullptr) {
		// Can't send this client any data. Because can't the slot is not saved. Change demo mode to other slot?
		//orig_SV_Nextserver();
		error_exit("Slice pointer missing");
	}

	//clear
	chan->message.cursize = 0;
	chan->message.overflowed = false;
	SOFPPNIX_DEBUG("Sending frame : %i",currentDemoFrame);
	orig_SZ_Write(&chan->message,slice->relSZ->data,slice->relSZ->cursize);

	currentDemoFrame +=1;


	return slice->unrelSZ;
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
		
		//if (stget(base,0x118) & 0x1) continue;

		//base->solid & 0x40
		//if (base->modelindex || base->solid & 0x40) {
		if ( base->number == i ) {
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

	}
	#endif

	orig_MSG_WriteByte (&buf, svc_stufftext);

	orig_MSG_WriteString (&buf, orig_va("precache %i; reset_predn\n",spawncount));

	// write it out
	initChunk_t newchunk;
	newchunk.data = malloc(buf.cursize);
	newchunk.len = buf.cursize;

	memcpy(newchunk.data,buf_data,buf.cursize);

	buf.cursize = 0;
	initialChunks.push_back(newchunk);
}


void demos_handlePlayback(netchan_t *chan, int length, byte *data)
{

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

	if ( clientState == cs_spawned ) {
		//normal svc_serverdata svc_frame etc...

		if ( currentDemoFrame > finalDemoFrame ) {
			SOFPPNIX_DEBUG("NextServer/KillServer!");
			orig_SV_Nextserver();
			//No more data to send.
			return;
		}

		sizebuf_t * unrelSZ = restoreNetworkBuffers(chan);
		orig_Netchan_Transmit(chan,unrelSZ->cursize,unrelSZ->data);

	} else {
		//connecting...
		int last_sent = stget(chan,0x10);
		int curtime = stget(0x08526624,0);
		if ( !initialChunks.empty() ) {
				//clear
				chan->message.cursize = 0;
				chan->message.overflowed = false;
				SOFPPNIX_DEBUG("Sending initialChunk");

				auto first = initialChunks.begin();
				
				orig_SZ_Write(&chan->message,first->data,first->len);
				initialChunks.erase(first);

				//eg. for any reliable comms during connecting.

				//no unreliable.
				orig_Netchan_Transmit (chan, 0, NULL);
		}
		else if ( curtime - last_sent > 1000 ) orig_Netchan_Transmit (chan, 0, NULL);
	}
}