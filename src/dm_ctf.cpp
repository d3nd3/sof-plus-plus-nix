#include "common.h"

void dm_ctf_c::levelInit(void)
{
	// always call this ! if u want to edit levelInit
	orig->levelInit();
}


void dm_ctf_c::applyHooks(void)
{
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