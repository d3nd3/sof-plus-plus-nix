// *****************************************************************************
// dm_ctf.h
// ------------
// Deathmatch stuff that's for the private use of dm_ctf.cpp.
// *****************************************************************************

#define CTF_VERSION					0.00
#define CTF_VSTRING2(x)				#x
#define CTF_VSTRING(x)				CTF_VSTRING2(x)
#define CTF_STRING_VERSION			CTF_VSTRING(CTF_VERSION)

#define CTF_TEAM1_SKIN	"ctf_r"
#define CTF_TEAM2_SKIN	"ctf_b"

#define CTF_CAPTURE_BONUS		15	// what you get for capture
#define CTF_TEAM_BONUS			10	// what your team gets for capture
#define CTF_RECOVERY_BONUS		2	// what you get for recovery
#define CTF_FLAG_BONUS			1	// what you get for picking up enemy flag
#define CTF_FRAG_CARRIER_BONUS	3	// what you get for fragging enemy flag carrier

#define CTF_CARRIER_DANGER_PROTECT_BONUS	2	// bonus for fraggin someone who has recently hurt your flag carrier
#define CTF_FLAG_DEFENSE_BONUS				1	// bonus for fraggin someone while either you or your target are near your flag or flag carrier

#define CTF_FLAG_DEFENSE_RADIUS				400.0  // area considered 'close' to flag or flag carrier

#define CTF_TIME_OUT						7.0		// in seconds
#define CTF_RESPAWN_TIMEOUT					150.0	// in seconds - time when dropped flag will return to base

#define PACKAGE_DM_CTF	0x0a

enum
{
	DM_CTF_CAPTURE_FLAG_RED = 0x0a00,
	DM_CTF_RECOVER_FLAG_RED = 0x0a01,
	DM_CTF_FRAG_CARRIER_RED = 0x0a02,
	DM_CTF_FRAG_HURT_CARRIER_RED = 0x0a03,
	DM_CTF_PROTECT_FLAG_RED = 0x0a04,
	DM_CTF_END_GAME_BONUS_RED = 0x0a05,
	DM_CTF_END_GAME_TEAM_BONUS = 0x0a06,
	DM_CTF_INACCESSIBLE_RED = 0x0a07,
	DM_CTF_ASSIGN_RED = 0x0a08,
	DM_CTF_BAD_SKIN = 0x0a09,
	DM_CTF_NO_CHANGE = 0x0a0a,
	DM_CTF_RESPAWN_FLAG_RED = 0x0a0b,
	DM_CTF_LAYOUT_SCOREBOARD_TEAM = 0x0a0c,
	DM_CTF_END_GAME = 0x0a0d,
	DM_CTF_CAPTURE_FLAG_BLUE = 0x0a0e,
	DM_CTF_RECOVER_FLAG_BLUE = 0x0a0f,
	DM_CTF_FRAG_CARRIER_BLUE = 0x0a10,
	DM_CTF_FRAG_HURT_CARRIER_BLUE = 0x0a11,
	DM_CTF_PROTECT_FLAG_BLUE = 0x0a12,
	DM_CTF_END_GAME_BONUS_BLUE = 0x0a13,
	DM_CTF_INACCESSIBLE_BLUE = 0x0a14,
	DM_CTF_RESPAWN_FLAG_BLUE = 0x0a15,
	DM_CTF_ASSIGN_BLUE = 0x0a16,
};
