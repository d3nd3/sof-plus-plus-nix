#include <cstring>
#include <cstdio>
#include <iostream>
#include <cstdlib>

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <list>
#include <algorithm>

#include <cerrno>

#include <thread>

#include <unistd.h>
#include <limits.h>
#include <dlfcn.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libgen.h>

#include <curl/curl.h>

#include "engine.h"
#include "fn_sigs.h"





// --------------------httpdl.cpp-----------------------
enum enum_dl_status {
	DS_UNCERTAIN,
	DS_FAILURE,
	DS_SUCCESS
};

struct FileData {
	int crc;
	std::string filename;
};


extern std::list<std::string> httpdl_cache_list;
extern CURL * curl_handle;
extern enum enum_dl_status download_status;
extern size_t httpdl_writecb(char *ptr, size_t size, size_t nmemb, void *userdata);
extern void httpdl_thread_get(std::string * rel_map_path);
extern int getHttpStatus(void);
extern bool beginHttpDL(std::string * relative_file_path_name);

extern size_t header_cb_get_content_length(char *buffer, size_t size, size_t nitems, void *userdata);
extern size_t partial_blob_100_cb(void *ptr, size_t size, size_t nmemb, void *userdata);
extern bool partialHttpBlobs(char * url);

extern bool getCentralDirectoryOffset(const char* buffer_haystack, int haystack_size,char ** found_offset);
extern void extractCentralDirectory(const char* centralDirectory, std::vector<FileData>* files);

extern bool unZipFile(char * in_zip, char * out_root );


extern bool removeItemHttpCache(const std::string& zipfile);
extern void appendItemHttpCache(const std::string& zipfile);

extern bool searchHttpCache(const std::string& target);

extern void saveHttpCache(void);
extern void loadHttpCache(void);


// --------------------exe_client.cpp-----------------------

// --------------------exe_shared.cpp-----------------------


// --------------------util.cpp-----------------------
extern void SOFPPNIX_PRINT(char * msg, ... );
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


// --------------------serverlist.cpp-----------------------
extern void GetServerList(void);
extern void my_SendToMasters(void * arg1,void * arg2);

// --------------------commands.cpp-----------------------
void CreateCommands(void);
extern void cmd_nix_client_state(void);
extern void cmd_nix_httpdl_test(void);
extern void cmd_nix_client_map(void);