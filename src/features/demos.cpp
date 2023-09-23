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


std::map<int,demo_frame_t*> demoFrames;


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
	SOFPPNIX_DEBUG("storeDemoData! relLen=%i unrelLen=%i",relLen,unrelLen);
	int slot = netchanToSlot(netchan);
	if ( slot == -1 ) error_exit("Cannot convert netchan to slot");

	int frameNum = stget(0x082AF680,0x10);
	SOFPPNIX_DEBUG("Slot is %i\nFramenum is %i\n",slot,frameNum);

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

	//buffer
	rel->data = malloc(relLen);
	rel->data[0] = 0x00;
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

	//buffer
	//unrel->data = new char[unrelLen];
	unrel->data = malloc(unrelLen);
	unrel->data[0] = 0x00;
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
		if (itSlices == fighters.end()) return nullptr;
		return itSlices->second;
	}
}

/*
	Reads and Increments 'currentDemoFrame'.
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
	orig_SZ_Write(&chan->message,slice->relSZ->data,slice->relSZ->cursize);

	currentDemoFrame +=1;


	return slice->unrelSZ;
}

void constructDemo()
{

}