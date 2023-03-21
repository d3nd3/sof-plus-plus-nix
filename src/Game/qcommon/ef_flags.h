#pragma once

// entity_state_t->effects
// Effects are things handled on the client side (lights, particles, frame animations)
// that happen constantly on the given entity.
// An entity that has effects will be sent to the client
// even if it has a zero index model.
#define	EF_ROTATE			0x00000001		// object (inc. boltons) roates with specifed ang velocities.
#define	EF_ANIM01			0x00000008		// automatically cycle between frames 0 and 1 at 2 hz
#define	EF_ANIM23			0x00000010		// automatically cycle between frames 2 and 3 at 2 hz
#define EF_ANIM_ALL			0x00000020		// automatically cycle through all frames at 2hz
#define EF_ANIM_ALLFAST		0x00000040		// automatically cycle through all frames at 10hz
#define	EF_TELEPORTER		0x00000080		// particle fountain
#define EF_AMB_SOUND_SET	0x00000100		// this object has an ambient sound set attached to it...
											// interpret the s.sound field as the set index number
#define EF_NO_BULLET		0x00000200		// bmodel leaves no bullet hole
#define EF_EXPLODING		0x00000400		// obj is exploding
#define EF_INVIS_PULSE		0x00000800		// For invulnerability
#define EF_NOTQUAD			0x00001000		// 2 frame buttons rather than the default 4
#define EF_SIMPLEROTATE		0x00002000		// For simple objects that rotate about Z with const. ang. vel.

#define EF_KILL_EFT			0x00004000		// Signal Nathan's effect system to kill all effects on this entity.
#define EF_FADE				0x00008000		// For corpse fading

// entity_state_t->renderfx flags
#define	RF_MINLIGHT			0x00000001		// allways have some light (viewmodel)
#define	RF_VIEWERMODEL		0x00000002		// don't draw through eyes, only mirrors
#define	RF_WEAPONMODEL		0x00000004		// only draw through eyes
#define	RF_FULLBRIGHT		0x00000008		// allways draw full intensity
#define	RF_DEPTHHACK		0x00000010		// for view weapon Z crunching
#define	RF_TRANSLUCENT		0x00000020
#define	RF_FRAMELERP		0x00000040

#define RF_SOUNDONLY		0x00000080		// don't draw this - sent to the client solely for sound purposes

#define	RF_CUSTOMSKIN		0x00000100		// skin is an index in image_precache
#define	RF_GLOW				0x00000200		// pulse lighting for bonus items
#define RF_BMODEL			0x00000400		// Set if this is a bmodel
#define RF_ALPHA_BMODEL		0x00000800		// Set if bmodel has alpha textures

#define RF_GHOUL_SHADOW		0x00001000		// This ghoul model should always have a shadow associated with it

#define RF_GHOUL			0x00400000		// Ghoul model
#define RF_STATICLIT		0x01000000		// world light doesn't need to be re-calced...
#define RF_LIGHTONCE		0x04000000		// New lighting options for speed reasons...
#define RF_NO_DYNA			0x08000000		// ""
#define RF_LIGHT_FASTEST	0x10000000		// ""
#define RF_LIGHT_BEST		0x20000000		// ""
#define RF_LIGHT_FROM_TINT	0x40000000		// ""

#define RF_TRANS_ANY		(RF_TRANSLUCENT | RF_ALPHA_BMODEL)

// player_state_t->refdef flags
#define	RDF_UNDERWATER		0x01		// warp the screen as apropriate
#define RDF_NOWORLDMODEL	0x02		// used for player configuration screen
#define RDF_GOGGLES			0x04		// Low light goggles.

