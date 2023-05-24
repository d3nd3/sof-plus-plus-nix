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

#include <regex>

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

#include <dirent.h>

#include <libgen.h>

#include <iomanip>

#include <curl/curl.h>
#include <zlib.h>

#include <openssl/evp.h>
#include <openssl/md4.h>

#include <bsd/string.h>

#include <python3.9/Python.h>

#include "engine.h"
#include "fn_sigs.h"

#define stget(e,x) *(unsigned int*)((void*)e+x)
#define stset(e,x,v) *(unsigned int*)((void*)e+x) = v


/*---------------------------------------------------------------------
-----------------------------main.cpp----------------------------
 ---------------------------------------------------------------------
*/
extern const char * SOFREESP;
/*---------------------------------------------------------------------
-----------------------------game.cpp----------------------------
 ---------------------------------------------------------------------
*/
extern void *base_addr;
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
extern void serverInit(void);
extern void public_rcon_command();
typedef void (*rcon_cmd_type)(void);
extern std::unordered_map<std::string,rcon_cmd_type> cmd_map;
extern void rcon_fraglimit(void);
extern void rcon_timelimit(void);
extern void rcon_deathmatch(void);


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
extern void slideFix(void);
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
extern void crc_checksum(const char * data,std::string & checksum, int length);
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
extern void * getClientX(int slot);

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
-----------------------------python.cpp----------------------------
 ---------------------------------------------------------------------
*/
extern PyObject *pGlobals;
extern void pythonInit(void);
extern void list_refcount(void);
extern PyObject* register_callback(const char * name ,std::vector<PyObject*> &callbacks,PyObject* args);


/*---------------------------------------------------------------------
-----------------------------py_api/memory.cpp----------------------------
 ---------------------------------------------------------------------
*/
extern PyMappingMethods entity_mapping_methods;
// extern PyGetSetDef *entity_getset_list;

class MemoryItem {
public:
	MemoryItem(std::string name,std::string type,unsigned int offset,std::string info) {
		this->name = name;
		this->type = type;
		this->offset = offset;
		this->info = info;
	}

	virtual void get(void* baseAddress, PyObject* &value)=0;

	
	virtual void get(void* baseAddress, short* &value);
	virtual void get(void* baseAddress, float* &value);
	virtual void get(void* baseAddress, char &value);
	virtual void get(void* baseAddress, int &value);
	virtual void get(void* baseAddress, short &value);
	virtual void get(void* baseAddress, float &value);
	virtual void get(void* baseAddress, void* &value);
	virtual void get(void* baseAddress, std::string &value);

	virtual void set(void* baseAddress,short* value);
	virtual void set(void* baseAddress,float* value);
	virtual void set(void* baseAddress,char value);
	virtual void set(void* baseAddress,int value);
	virtual void set(void* baseAddress,short value);
	virtual void set(void* baseAddress,float value);
	virtual void set(void* baseAddress,void * value);
	virtual void set(void* baseAddress,std::string value);

	virtual void set(void* baseAddress, PyObject* value)=0;

	std::string name;
	std::string type;
	unsigned int offset;
	std::string info;

public:
	static void initMemoryItems(void);
	static std::unordered_map<std::string, MemoryItem*> entityProperties;
};
/*---------------------------------------------------------------------
--------------------------py_api/decorators.cpp------------------------
 ---------------------------------------------------------------------
*/
extern std::vector<std::vector<PyObject*>*> decorators;
extern std::vector<PyObject*> player_die_callbacks;
extern std::vector<PyObject*> player_connect_callbacks;
extern std::vector<PyObject*> player_disconnect_callbacks;
extern std::vector<PyObject*> player_respawn_callbacks;
extern std::vector<PyObject*> player_say_callbacks;

extern std::vector<PyObject*> frame_early_callbacks;
extern std::vector<PyObject*> map_spawn_callbacks;

extern void removeDecoratorCallbacks(void);

/*---------------------------------------------------------------------
--------------------------py_api/entity.cpp------------------------
 ---------------------------------------------------------------------
*/
extern PyObject* createEntDict(edict_t * c_ent);
typedef struct {
	// order matters here. dict is at top.
	// PyObject_HEAD ( not required for inherited original types )
	PyDictObject dict;
	// Custom fields here.
	edict_t * c_ent;
} EntDict;

void initEntDictType(void);
/*---------------------------------------------------------------------
--------------------------py_api/ppnix.cpp------------------------
 ---------------------------------------------------------------------
*/
extern std::vector<PyObject*> ppnix_delayedRun_callbacks;



/*---------------------------------------------------------------------
--------------------------py_api/player.cpp------------------------
 ---------------------------------------------------------------------
*/

extern void player_spackage_print_id(edict_t * ent, unsigned char file_id,unsigned char string_index, char * fmt, ...);
extern void player_spackage_print_ref(edict_t * ent, char * file_ref, char * string_ref, char * fmt, ...);

extern void nix_draw_string_direct(edict_t * ent,char * message);
extern void nix_draw_string(edict_t * ent,int offsetx, int offsety, char * message, bool gray);
extern void nix_draw_clear(edict_t * ent);
/*---------------------------------------------------------------------
-----------------------------commands.cpp----------------------------
 ---------------------------------------------------------------------
*/
void CreateCommands(void);
extern void cmd_nix_client_state(void);
extern void cmd_nix_test(void);
extern void cmd_nix_client_map(void);
extern void cmd_nix_checksum(void);
extern void nix_draw_clear(edict_t * ent);


/*---------------------------------------------------------------------
-----------------------------cvars.cpp----------------------------
 ---------------------------------------------------------------------
*/
extern void createServerCvars(void);
extern void createClientCvars(void);
extern void createSharedCvars(void);

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
extern cvar_t * _nix_violence;
extern cvar_t ** p_nix_violence;

extern cvar_t * _nix_slidefix;

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
extern std::array<char [1024],32> strip_layouts;
extern std::array<int,32> strip_layout_size;

extern void fixupClassesForLinux(void);
#include "deathmatch.h"

/*
---------------------------------------------------------------------------
-----------------------------dm_always.cpp---------------------------------
---------------------------------------------------------------------------
*/
extern bool prev_showscores[32];