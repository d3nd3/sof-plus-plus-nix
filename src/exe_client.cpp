#include "common.h"
/*
typedef struct cvar_s
{
	char			*name;
	char			*string;
	char			*latched_string;	// for CVAR_LATCH vars
	int				flags;
	cvarcommand_t	command;
	qboolean		modified;	// set each time the cvar is changed
	float			value;
	struct cvar_s *	next;
} cvar_t;
*/

/*
	Strings alloced by M_Alloc in Cvar_Get // CopyString

	decimals where 1/N , where N is power of 2 can be represented exactly as floating point.
*/
