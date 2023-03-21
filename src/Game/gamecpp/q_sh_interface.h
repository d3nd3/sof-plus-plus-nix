
// q_sh_interface.h -- included by interface program modules


// Flags
#define STAT_FLAG_SCOPE					0x0001
#define STAT_FLAG_INFOTICKER			0x0002
#define STAT_FLAG_WIND					0x0004
#define STAT_FLAG_MISSIONFAIL			0x0008
#define STAT_FLAG_HIT					0x0010
#define STAT_FLAG_TEAM					0x0020
#define STAT_FLAG_COUNTDOWN				0x0040
#define STAT_FLAG_MISSIONACC			0x0080
#define STAT_FLAG_MISSIONEXIT			0x0100
#define STAT_FLAG_SHOW_STEALTH			0x0200
#define STAT_FLAG_OBJECTIVES			0x0400
#define STAT_FLAG_HIDECROSSHAIR			0x0800

// player_state->stats[] indexes
#define STAT_INV_TYPE			0	// cleared each frame
#define	STAT_HEALTH				1
#define	STAT_CLIP_AMMO			2
#define	STAT_AMMO				3
#define	STAT_CLIP_MAX			4
#define	STAT_ARMOR				5
#define	STAT_WEAPON				6
#define	STAT_INV_COUNT			7	// # of current inventory item
#define	STAT_FLAGS				8
#define	STAT_LAYOUTS			9
#define	STAT_FRAGS				10
#define STAT_STEALTH			11
#define STAT_FORCEHUD			12	// draw the HUD regardless of current weapon (tutorial level)

// UNUSED I BELIEVE
#define STAT_CASH2				12	// cash in bank
#define STAT_CASH3				13	// cash to be awarded upon completion of mission
#define STAT_DAMAGELOC			14	// last damage done to player
#define STAT_DAMAGEDIR			15	// direction of the damage bracket on interface

// Bit flags for client damage location
#define CLDAM_HEAD		0x00000001
#define CLDAM_L_ARM		0x00000002
#define CLDAM_L_LEG		0x00000004
#define CLDAM_R_ARM		0x00000008
#define CLDAM_R_LEG		0x00000010
#define CLDAM_TORSO		0x00000020

// Index for client damage locations
#define CL_HEAD			0
#define CL_L_ARM		1
#define CL_L_LEG		2
#define CL_R_ARM		3
#define CL_R_LEG		4
#define CL_TORSO		5
#define MAX_CLDAMAGE	6

// Bit flags for client damage brackets
#define CLDAM_T_BRACKET		0x00000001	// Top bracket
#define CLDAM_B_BRACKET		0x00000002	// Bottom bracket
#define CLDAM_R_BRACKET		0x00000004	// Right bracket
#define CLDAM_L_BRACKET		0x00000008	// Left bracket
#define CLDAM_ALL_BRACKET	0x00000010	// All brackets

