#include <cstring>
#include <iostream>
#include <cstdlib>

#include <string>
#include <vector>
#include <sstream>

#include <cerrno>

#include <thread>

#include <curl/curl.h>

#include "engine.h"


#include "fn_sigs.h"


// httpdl.cpp
extern size_t httpdl_writecb(char *ptr, size_t size, size_t nmemb, void *userdata);
extern void httpdl_thread_get(const char* url);
extern CURLcode res;
extern long http_status;

// exe_client.cpp
extern std::thread httpdl;

// exe_shared.cpp
extern CURL * curl_handle;

// util.cpp
extern void error(const char* message);
extern void hexdump(void *addr_start, void *addr_end);
extern void memoryUnprotect(void * addr);
extern void memoryProtect(void * addr);
extern void callE8Patch(void * addr,void * to);
extern void * createDetour(void * orig, void * mine, int size);
extern void memoryAdjust(void * addr, int size, unsigned char val);
extern void NetadrToSockadr (netadr_t *a, struct sockaddr_in *s);
extern void SockadrToNetadr (struct sockaddr_in *s, netadr_t *a);
extern void create_dir_if_not_exists(const char* dir_path);
extern void create_file_dir_if_not_exists(const char* file_path);

// serverlist.cpp
extern void GetServerList(void);
extern void my_SendToMasters(void * arg1,void * arg2);


// commands.cpp
void CreateCommands(void);
extern void cmd_nix_client_state(void);
extern void cmd_nix_httpdl_test(void);