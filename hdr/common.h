#include <cstring>
#include <iostream>
#include <cstdlib>

#include <string>
#include <vector>
#include <sstream>

#include "engine.h"
#include "fn_sigs.h"

extern void error(const char* message);
extern void hexdump(void *addr_start, void *addr_end);
extern void memoryUnprotect(void * addr);
extern void memoryProtect(void * addr);
extern void callE8Patch(void * addr,void * to);
extern void * createDetour(void * orig, void * mine, int size);
extern void memoryAdjust(void * addr, int size, unsigned char val);


extern void GetServerList(void);
extern void my_SendToMasters(void * arg1,void * arg2);
extern void my_AddServer(void * menu_system, netadr_t adr, char * serverdata);