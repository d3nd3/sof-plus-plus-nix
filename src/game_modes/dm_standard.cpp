#include "common.h"

void dm_standard_c::levelInit(void)
{
	// always call this ! if u want to edit levelInit
	orig->levelInit();
}
void dm_standard_c::applyHooks(void)
{
	// these funcs don't exist in orig lol...
}

void dm_standard_c::removeHooks(void)
{
	// these funcs don't exist in orig lol...

}



//sof ree
//\x24\xF8\x46 \xAE\xC9\xC9
// char* dm_standard_c::getGameName(void) {
// 	return "\x17""\x24\xF8\x46""\x07""\xAE\xC9\xC9""\x01""DM";
// }