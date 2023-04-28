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


#include <stdarg.h>

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

#include <execinfo.h>

#include <libgen.h>

#include <iomanip>

#include <curl/curl.h>
#include <zlib.h>

#include <openssl/evp.h>
#include <openssl/md4.h>

#include "engine.h"
#include "fn_sigs.h"


/*---------------------------------------------------------------------
-----------------------------main.cpp----------------------------
 ---------------------------------------------------------------------
*/
extern const char * SOFREESP;
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



/*---------------------------------------------------------------------
-----------------------------exe_shared.cpp----------------------------
 ---------------------------------------------------------------------
*/
extern char __BUILD_YEAR;
extern char __BUILD_MONTH;
extern char __BUILD_DAY;
extern char __BUILD_NUMBER;
extern int next_available_ID;
extern int sofreebuild_len;
extern char sofreebuildstring[128];
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
extern void dump_usercmd(usercmd_t& cmd);

// utils
extern void error_exit(char* message,...);
extern void hexdump(void *addr_start, void *addr_end);
extern void create_dir_if_not_exists(const char* dir_path);
extern void create_file_dir_if_not_exists(const char* file_path);
extern void crc_checksum(const char * data,std::string & checksum);
extern int changesize(FILE *fp, off_t size);
extern void* fast_realloc(void * buffer,int size);
extern std::string toLowercase(const std::string& str);
extern bool strcasestr(const std::string& str, const std::string& substr);
extern void LoadTextFile(const char *fileName, char **text);

// linked lists

/* Define a struct to represent each element in the linked list */
typedef struct list_node {
    struct list_node *next;
    struct list_node *prev;
    void *data;
} list_node_t;

/* Define a struct to represent the linked list itself */
typedef struct list {
    list_node_t *head;
    list_node_t *tail;
    size_t count;
} list_t;

/* Initialize a new linked list */
extern list_t *list_create();
extern void list_destroy(list_t *list);
extern list_node_t *list_node_create(void *data);
extern void list_append(list_t *list, void *data);
extern void list_remove(list_t *list, list_node_t *node);
extern void list_print(list_t *list);

// sof specific
extern int countPlayersInGame(void);
extern bool isServerEmpty(void);
extern int getPlayerSlot(void * in_client);

// math
extern void my_itoa(int i, char * out_str);
extern int c_string_to_int(char * in_str);
extern vec_t VectorNormalize (vec3_t v);
extern float NormalizeAngle (float angle);
extern vec_t VectorLength (const vec3_t v);
static vec_t VectorSeparation(vec3_t v0, vec3_t v1);


// A variadic function to print some integers
extern void print_ints(void * x, ...);
extern int arraySize;
extern std::array<void*,10> formatString(const std::string& format, const std::vector<std::string>& inputs);
extern void SP_PRINT_MULTI(edict_t * ent, short id, std::string& format, std::vector<std::string>& inputs);

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

extern void nix_draw_clear(void);
extern void nix_draw_string(void);
extern void nix_draw_string2(void);
extern void nix_draw_altstring(void);
extern void nix_spackage_list(void);
extern void nix_spackage_register(void);
extern void nix_spackage_print_ref(void);
extern void nix_spackage_print_id(void);
extern void nix_spackage_print_obit(void);
extern void nix_spackage_print_string(void);
extern void nix_spackage_gettext(void);

/*---------------------------------------------------------------------
-----------------------------cvars.cpp----------------------------
 ---------------------------------------------------------------------
*/
extern void CreateCvars(void);
extern cvar_t * _nix_version;
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

extern cvar_t * _nix_deathmatch;

extern cvar_t * findCvar(char * cvarname);
extern void setCvarUnsignedInt(cvar_t * which,unsigned int val);
extern void setCvarInt(cvar_t * which,int val);
extern void setCvarByte(cvar_t * which, unsigned char val);
extern void setCvarFloat(cvar_t * which, float val);
extern void setCvarString(cvar_t * which, char * newstr);

/*---------------------------------------------------------------------
-----------------------------detect.cpp----------------------------
 ---------------------------------------------------------------------
*/

extern std::array<short, 32> detect_max_yaw;
extern std::array<short, 32> detect_max_pitch;
extern std::array<edict_t*, 32> distractor;
extern void validateInputs(int playerSlot, usercmd_t * now, usercmd_t * old);
extern void spawnDistraction(edict_t * ent,int slot);


/*---------------------------------------------------------------------
-----------------------------deathmatch.cpp----------------------------
 ---------------------------------------------------------------------
*/
#define stget(e,x) *(unsigned int*)((void*)e+x)
#define stset(e,x,v) *(unsigned int*)((void*)e+x) = v
extern void fixupClassesForLinux(void);
#include "deathmatch.h"
