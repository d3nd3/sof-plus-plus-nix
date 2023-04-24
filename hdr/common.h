#include <cstring>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <chrono>

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <list>
#include <algorithm>
#include <unordered_map>

#include <cerrno>

#include <thread>
#include <signal.h>

#include <unistd.h>
#include <limits.h>
#include <dlfcn.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <libgen.h>

#include <curl/curl.h>

#include <openssl/evp.h>
#include <openssl/md4.h>

#include "engine.h"
#include "fn_sigs.h"


/*---------------------------------------------------------------------
-----------------------------game.cpp----------------------------
 ---------------------------------------------------------------------
*/
extern unsigned char chktbl2[3000];


/*---------------------------------------------------------------------
-----------------------------httpdl.cpp----------------------------
 ---------------------------------------------------------------------
*/
enum enum_dl_status {
	DS_UNCERTAIN,
	DS_FAILURE,
	DS_SUCCESS
};

struct FileData {
	int crc;
	std::string filename;
};

extern void (*HTTP_CONTINUE_CB)(void);

extern std::vector<FileData> zip_content;
extern std::list<std::string> httpdl_cache_list;
extern CURL * curl_handle;
extern enum enum_dl_status download_status;
extern size_t httpdl_writecb(char *ptr, size_t size, size_t nmemb, void *userdata);
extern void httpdl_thread_get(std::string * rel_map_path);
extern void isHTTPdone(void);
extern bool beginHttpDL(std::string * mapname, void * callback,bool use_join=false);

extern size_t header_cb_get_content_length(char *buffer, size_t size, size_t nitems, void *userdata);
extern size_t partial_blob_100_cb(void *ptr, size_t size, size_t nmemb, void *userdata);
extern bool partialHttpBlobs(char * url);

extern int getCentralDirectoryOffset(const char* HTTP_BLOB, int BLOB_SIZE,int &found_offset, int &abs_dir_offset);
extern void extractCentralDirectory(const char* centralDirectory,int cd_size, std::vector<FileData>* files);

extern bool unZipFile(char * in_zip, char * out_root );


extern bool removeItemHttpCache(const std::string& zipfile);
extern void appendItemHttpCache(const std::string& zipfile);

extern bool searchHttpCache(const std::string& target);

extern void saveHttpCache(void);
extern void loadHttpCache(void);


/*---------------------------------------------------------------------
-----------------------------exe_client.cpp----------------------------
 ---------------------------------------------------------------------
*/
/*---------------------------------------------------------------------
-----------------------------exe_server.cpp----------------------------
 ---------------------------------------------------------------------
*/
extern std::array<int, 32> client_framecounters;
extern std::array<usercmd_t, 32> client_last_non_skipped_cmd;
extern std::array<usercmd_t, 32> client_last_last_non_skipped_cmd;
extern std::array<usercmd_t, 32> client_last_skipped_cmd;
extern std::array<usercmd_t, 32> client_last_cmd;
extern std::array<usercmd_t, 32> client_last_last_cmd;
extern std::array<int, 32> client_toggle_state;

/*---------------------------------------------------------------------
-----------------------------exe_shared.cpp----------------------------
 ---------------------------------------------------------------------
*/

extern void my_test(void);
/*---------------------------------------------------------------------
------------------------------util.cpp---------------------------------
---------------------------------------------------------------------
*/
// print
extern void SOFPPNIX_DEBUG(char * msg, ... );
extern void SOFPPNIX_PRINT(char * msg, ... );

// memory funcs
extern void memoryUnprotect(void * addr);
extern void memoryProtect(void * addr);
extern void callE8Patch(void * addr,void * to);
extern void * createDetour(void * orig, void * mine, int size);
extern void memoryAdjust(void * addr, int size, unsigned char val);

// net
extern void NetadrToSockadr (netadr_t *a, struct sockaddr_in *s);
extern void SockadrToNetadr (struct sockaddr_in *s, netadr_t *a);

// utils
extern void error_exit(char* message,...);
extern void hexdump(void *addr_start, void *addr_end);
extern int c_string_to_int(char * in_str);
extern void create_dir_if_not_exists(const char* dir_path);
extern void create_file_dir_if_not_exists(const char* file_path);
extern int countPlayersInGame(void);
extern bool isServerEmpty(void);
extern int getPlayerSlot(void * in_client);
extern void dump_usercmd(usercmd_t& cmd);


/*---------------------------------------------------------------------
-----------------------------serverlist.cpp----------------------------
 ---------------------------------------------------------------------
*/
extern int gs_select_sock;
extern netadr_t sof1master_ip;
// client gs
extern void GetServerList(void);

// gs
extern void nonBlockingServerResponses(void);
extern void GamespyHeartbeatCtrlNotify(void);
extern void GamespyHeartbeat(void);


/*---------------------------------------------------------------------
-----------------------------commands.cpp----------------------------
 ---------------------------------------------------------------------
*/
void CreateCommands(void);
extern void cmd_nix_client_state(void);
extern void cmd_nix_test(void);
extern void cmd_nix_client_map(void);
extern void cmd_nix_checksum(void);

/*---------------------------------------------------------------------
-----------------------------cvars.cpp----------------------------
 ---------------------------------------------------------------------
*/
extern void CreateCvars(void);
extern cvar_t * maxclients;
extern cvar_t * sv_public;
extern cvar_t * gamespyport;
extern cvar_t * hostport;
extern cvar_t * hostname;
extern cvar_t * deathmatch;
extern cvar_t * mapname;
extern cvar_t * sv_violence;
extern cvar_t * timelimit;
extern cvar_t * fraglimit;
extern cvar_t * dmflags;
extern cvar_t * g_movescale;
extern cvar_t * cheats;
extern cvar_t * ctf_loops;
extern cvar_t * sv_suicidepenalty;

extern cvar_t * _nix_poorman;