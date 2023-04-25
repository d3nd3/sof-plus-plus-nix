#include "common.h"

// void dm_ctf_c::levelInit(void)
// {
// 	static void (*o)(void * self) = stget(stget(orig,4)+8+0x10,0);
	
	

// 	o(orig);
// }


void dm_ctf_c::applyHooks(void)
{
	SOFPPNIX_DEBUG("Need level init to be called\n");
	// orig->applyHooks();
}

void dm_ctf_c::removeHooks(void)
{
	// orig->removeHooks();
}


//sof ree
//\x24\xF8\x46 \xAE\xC9\xC9
// char* dm_standard_c::getGameName(void) {
// 	return "\x17""\x24\xF8\x46""\x07""\xAE\xC9\xC9""\x01""DM";
// }