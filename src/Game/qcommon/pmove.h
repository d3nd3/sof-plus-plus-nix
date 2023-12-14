#pragma once

// player_state_t is the information needed in addition to pmove_state_t
// to rendered a view.  There will only be 10 player_state_t sent each second,
// but the number of pmove_state_t changes will be relative to client
// frame rates

typedef struct
{
	pmove_state_t	pmove;					// for prediction
	
	// These fields do not need to be communicated bit-precise.

	vec3_t		viewangles;					// for fixed views
	vec3_t		viewoffset;					// add to pmovestate->origin
	vec3_t		kick_angles;				// add to view direction to get render angles, set by falling, pain effects, etc
	vec3_t		weaponkick_angles;			// add to view direction to get render angles, set by weapons

	// For remote camera views.			

	vec3_t		remote_vieworigin,
				remote_viewangles;
	int			remote_id;
	byte		remote_type;

	// View weapon stuff.

	IGhoulInst	*gun;
	short		gunUUID;					// n/w friendly GHOUL instance 'pointer'
	short		gunType;					// SFW_xxx
	short		gunClip;					// ammo in clip	
	short		gunAmmo;					// total ammo for gun
	byte		gunReload;					// reloaded gun
	byte		restart_count;				// needed to detect restart of weapon predn.
	byte		buttons_inhibit;				// mask of buttons to inhibit (for weapon CP)

	//apologies to all for the mess--testing player body technique --ss
	IGhoulInst	*bod;
	short		bodUUID;					// bleah

	// View effect stuff.

	float		blend[4];					// rgba full screen effect
	float		fov;						// horizontal field of view
	int			rdflags;					// refdef flags

	// sound state stuff
	short		soundID;					// General ambient sounds
	byte		musicID;					// ID of the music to play

	// Staus bar stuff.

	short		damageLoc;					// for damage location 
	short		damageDir;					// for damage direction for brackets
	short		stats[MAX_STATS];			// fast status bar updates
	byte		dmRank;						// Rank in DM.
	byte		dmRankedPlyrs;				// No of ranked players in DM (i.e. excl. spectators).
	byte		spectatorId;				// Index [1-(MAXCLIENTS-1)] of who we're chasing in spectator chasecam.

	byte		cinematicfreeze;			// Shows if currently in a cinematic
} player_state_t;

// end