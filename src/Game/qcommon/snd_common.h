#pragma once

// sound channels
// channel 0 never willingly overrides
// other channels (1-7) allways override a playing sound on that channel
#define	CHAN_AUTO               0
#define	CHAN_WEAPON             1
#define	CHAN_VOICE              2
#define	CHAN_ITEM               3
#define	CHAN_BODY               4
#define CHAN_ENT1				5
#define CHAN_ENT2				6
// modifier flags
#define	CHAN_NO_PHS_ADD			8	// send to all clients, not just ones in PHS (ATTN 0 will also do this)
#define	CHAN_RELIABLE			16	// send by reliable message, not datagram


// sound attenuation values
#define	ATTN_NONE               0	// full volume the entire level
#define	ATTN_NORM               1
#define	ATTN_IDLE               2
#define	ATTN_STATIC             3	// diminish very rapidly with distance


#define SND_LOCALIZE_GLOBAL		0	// Default, means that the sound is projected into the world as normal.
#define SND_LOCALIZE_CLIENT		1	// Means to ONLY play the sound on the associated client.

// sound info fields
#define SIF_MUSIC_STATE		0x01
#define SIF_DESIGNER_MUSIC	0x02
#define SIF_AMB_GEN_SET		0x04
#define SIF_SOUND_EFFECT	0x08

enum
{
	SPE_NORMAL,
	SPE_WATER,
	SPE_NUM_PROC_TYPES
};

// volume mask for ent->sound_data - makes room for attn value in the lower bits
#define ENT_VOL_MASK	0xf8

enum
{
	SND_LOOPATTN,
	SND_NEARATTN,
	SND_NORMATTN,
	SND_FARATTN,
	SND_VERYFARATTN,
	SND_MAXATTN
};
