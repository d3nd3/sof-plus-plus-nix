#ifndef __singleplr_h
#define __singleplr_h


/********************************************************************************

single player only string packages

********************************************************************************/


#define PACKAGE_SINGLEPLR		0x06

enum
{
	SINGLEPLR_HOSTAGE_HIT = 0x0600,
	SINGLEPLR_BUDDY_HIT = 0x0601,
	SINGLEPLR_SAVES_NO_MORE = 0x0602,
	SINGLEPLR_SAVES_ONE_LEFT = 0x0603,
	SINGLEPLR_SAVES_N_LEFT = 0x0604,
	SINGLEPLR_SAVES_USED_UP = 0x0605,
	SINGLEPLR_MISSION_FAILED = 0x0606,
	SINGLEPLR_FRIENDLY_CASUALTIES = 0x0607,
	SINGLEPLR_MISSION_ACCOMPLISHED = 0x0608,
	SINGLEPLR_OBJECTIVES_UPDATED = 0x0609,
	SINGLEPLR_MISSION_EXIT = 0x060a,
	SINGLEPLR_MORE_TO_GO = 0x060b,
	SINGLEPLR_SEQUENCE_COMPLETED = 0x060c,
	SINGLEPLR_UNLIMITED = 0x060d
};


#endif // __singleplr_h
