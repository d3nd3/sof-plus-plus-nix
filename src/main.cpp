#include <stdio.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <sys/auxv.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <limits.h>

#include <cstring>
#include <iostream>
#include <cstdlib>

#include <string>
#include <vector>
#include <sstream>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>


typedef int bool;
typedef void edict_t;
typedef void usercmd_t;
typedef int qboolean;
typedef unsigned char byte;

typedef enum {NA_LOOPBACK, NA_BROADCAST, NA_IP, NA_IPX, NA_BROADCAST_IPX} netadrtype_t;
typedef enum {NS_CLIENT, NS_SERVER} netsrc_t;
typedef struct netadr_s
{
    netadrtype_t    type; //0
    byte    ip[4]; //4
    byte    ipx[10]; //8

    unsigned short  port; //12
    // char * description; //14
} netadr_t;


typedef struct game_export_s {
    // the init function will only be called when a game starts,
    // not each time a level is loaded.  Persistant data for clients
    // and the server can be allocated in init
    int APIVERSION;
    void        (*Init) (void);
    void        (*Shutdown) (void);

    // each new level entered will cause a call to SpawnEntities
    void        (*SpawnEntities) (char *mapname, char *entstring, char *spawnpoint);

    // Read/Write Game is for storing persistant cross level information
    // about the world state and the clients.
    // WriteGame is called every time a level is exited.
    // ReadGame is called on a loadgame.
    void        (*WriteGame) (bool autosave);
    bool        (*ReadGame) (bool autosave);

    // ReadLevel is called after the default map information has been
    // loaded with SpawnEntities
    void        (*WriteLevel) (void);
    void        (*ReadLevel) (void);

    void        (*ClientThink) (edict_t *ent, usercmd_t *cmd);
    qboolean    (*ClientConnect) (edict_t *ent, char *userinfo);

    //NEW
    void    (*ClientPreConnect) (void*);

    void        (*ClientUserinfoChanged) (edict_t *ent, char *userinfo, bool not_first_time);

    void        (*ClientDisconnect) (edict_t *ent);
    void        (*ClientBegin) (edict_t *ent);

    void        (*ClientCommand) (edict_t *ent);
    void        (*ResetCTFTeam) (edict_t *ent);

    int         (*GameAllowASave) (void);

    void        (*SavesLeft) (void);
    void        (*GetGameStats) (void);

    void        (*UpdateInven) (void);
    const char  *(*GetDMGameName) (void);

    byte        (*GetCinematicFreeze) (void);
    void        (*SetCinematicFreeze) (byte cf);

    float       (*RunFrame) (int serverframe);
} game_export_t;

void memoryUnprotect(void * addr);
void memoryProtect(void * addr);

//commands
void testaddserver(void);

void my_CL_PingServers(void);
void (*orig_CL_PingServers)(void) = 0x080C5DE8;

void GetServerList(void);

void my_AddServer(void * menu_system, netadr_t adr, char * serverdata);
void (*orig_AddServer)(void * menu_system, netadr_t adr,char * serverdata) = NULL;

void * (*orig_GetRefAPI ) (void* rimp ) = NULL;
void * GetRefAPI(void* rimp);

void * (*orig_GetGameAPI) (void * import) = NULL;
void *GetGameAPI (void *import);

void * (*orig_Sys_GetGameAPI)(void * params) = NULL;
game_export_t * my_Sys_GetGameAPI (void *params);

void (*orig_ShutdownGame)(void) = NULL;
void my_ShutdownGame(void);

void (*orig_Qcommon_Init)(int one, char** two) = NULL;
void my_Qcommon_Init(int one , char ** two);


char * (*orig_getenv)(char * name) = NULL;

char * (*orig_SV_StatusString)(void) = 0x080A97A4;


void (*orig_FX_Init)(void) = NULL;
void my_FX_Init(void);

void (*orig_Cmd_AddCommand)(char * cmd, void * callback) = NULL;
void my_Cmd_AddCommand(char * cmd, void * callback);

void (*orig_Cmd_RemoveCommand)(char * cmd);
void my_Cmd_RemoveCommand(char * cmd);

int     (*orig_Cmd_Argc) (void) = 0x081194FC;
char    *(*orig_Cmd_Argv) (int i) = 0x081194C8;
char    *(*orig_Cmd_Args) (void) = 0x081194B0;

void (*orig_Com_Printf) (char *msg, ...) = NULL;
void my_Com_Printf(char *msg,...);

char *my_Sys_GetClipboardData(void);
void my_SendToMasters(void * arg1,void * arg2);

void * createDetour(void * orig, void * mine, int size);
void callE8Patch(void * addr, void * to);


void error(const char* message) {
    std::cerr << "Error: " << message << std::endl;
    std::exit(EXIT_FAILURE);
}


void my_Qcommon_Init(int one , char ** two) {
    orig_Qcommon_Init(one,two);
    // Init has been done!
    my_Cmd_AddCommand("testaddserver",&testaddserver);

    // my_Cmd_RemoveCommand("pingservers");
    // my_Cmd_AddCommand("pingservers",&my_CL_PingServers);
}

void memoryUnprotect(void * addr)
{
    size_t page_size = getpagesize();

    void * aligned_addr = (void *)((unsigned long)addr & ~(page_size - 1));
    if (mprotect(aligned_addr, page_size, PROT_READ | PROT_WRITE) == -1) {
        error("mprotect");
    }
}

void memoryProtect(void * addr)
{
    size_t page_size = getpagesize();
    void * aligned_addr = (void *)((unsigned long)addr & ~(page_size - 1));
    if (mprotect(aligned_addr, page_size, PROT_READ ) == -1) {
        error("mprotect");
    }
}


void callE8Patch(void * addr,void * to) {

    memoryUnprotect(addr);

    unsigned char * p = addr;
    p[0] = 0xE8;
    *(int*)(p+1) = to - (int)addr - 5;

    memoryProtect(addr);
}

void * createDetour(void * orig, void * mine, int size) {
    memoryUnprotect(orig);

    /*
        THUNK SAVED. { SAVED OPCODES;JMP->ORIG+SIZE(SAVED_OPCODES) }
    */
    void * thunk = malloc(size + 5);
    // save the front of func
    memcpy(thunk, orig, size);
    // jump back
    unsigned char * p = thunk + size;
    *p = 0xE9;
    int rel_jmp = ((int)orig+size) - (int)p - 5;
    memcpy(p+1,&rel_jmp,4);

    /*
        PATCH 5 bytes start of orig
    */
    unsigned char small_jmp;
    // write the front of func
    small_jmp = 0xE9;
    rel_jmp = mine - (int)orig - 5;
    memcpy(orig,&small_jmp,1);
    memcpy(orig+1,&rel_jmp,4);

    memoryUnprotect(orig);
    // dealloc this on uninit.
    return thunk;
}

void __attribute__ ((constructor)) begin() {
    printf("Constructor\n");


    callE8Patch(0x08113315,&my_Sys_GetClipboardData);



///////////////////////////////////////////////////////// info %s 33!
    // Receive info packets from protocol 33

    
    memoryUnprotect(0x080A99D7);
    *(unsigned char*)0x080A99D7 = 0x21;
    memoryProtect(0x080A99D7);

    // Send info packets with protocol 33
    memoryUnprotect(0x080C5E7C);
    *(unsigned char*)0x080C5E7C = 0x21;
    memoryProtect(0x080C5E7C);

    memoryUnprotect(0x080C5F02);
    *(unsigned char*)0x080C5F02 = 0x21;
    memoryProtect(0x080C5F02);

    memoryUnprotect(0x080C5FE4);
    *(unsigned char*)0x080C5FE4 = 0x21;
    memoryProtect(0x080C5FE4);

    memoryUnprotect(0x080C51B1);
    *(unsigned char*)0x080C51B1 = 0x21;
    memoryProtect(0x080C51B1);

////////////////////////////////////////////////////////////////////// WON DISABLE
    // NOP WON Stuff?

    //Qcommon_Init ... cWON_General:: cWon_General.
    memoryUnprotect(0x0811ED2B);
    memset(0x0811ED2B,0x90,5);
    memoryProtect(0x0811ED2B);

    //cWon_General Destructor
    memoryUnprotect(0x08121212);
    memset(0x08121212,0x90,5);
    memoryProtect(0x08121212);

    memoryUnprotect(0x0811CD3E);
    memset(0x0811CD3E,0x90,5);
    memoryProtect(0x0811CD3E);

    //Qcommon_INit ... cWON_General:: Register Commands 
    memoryUnprotect(0x0811ED39);
    memset(0x0811ED39,0x90,5);
    memoryProtect(0x0811ED39);

    //SV_InitGame ... cWon_Server
    memoryUnprotect(0x080A89A3);
    memset(0x080A89A3,0x90,5);
    memoryProtect(0x080A89A3);


    //CL_Init ... cWon_Client
    memoryUnprotect(0x80C89C0);
    memset(0x80C89C0,0x90,5);
    memoryProtect(0x80C89C0);

    //CL_Init ... cWon_Client:RegisterCommands
    memoryUnprotect(0x080C89CE);
    memset(0x080C89CE,0x90,5);
    memoryProtect(0x080C89CE);


    //won_markdisconnection called in menus
    memoryUnprotect(0x0825EF65);
    memset(0x0825EF65, 0x00,1);
    memoryProtect(0x0825EF65);


    //NewAuthorititveRequest
    memoryUnprotect(0x080A9CD9);
    memset(0x080A9CD9,0x90,5);
    memoryProtect(0x080A9CD9);

    memoryUnprotect(0x080A9DA7);
    memset(0x080A9DA7,0x90,5);
    memoryProtect(0x080A9DA7);

    //FreeAUthorititiveReqwuest
    memoryUnprotect(0x080A9CBF);
    memset(0x080A9CBF,0x90,5);
    memoryProtect(0x080A9CBF);

    memoryUnprotect(0x080A9D8D);
    memset(0x080A9D8D,0x90,5);
    memoryProtect(0x080A9D8D);

    memoryUnprotect(0x080AA315);
    memset(0x080AA315,0x90,5);
    memoryProtect(0x080AA315);

    memoryUnprotect(0x080AA71D);
    memset(0x080AA71D,0x90,5);
    memoryProtect(0x080AA71D);

    memoryUnprotect(0x080AA746);
    memset(0x080AA746,0x90,5);
    memoryProtect(0x080AA746);

    //cWon removeServer
    memoryUnprotect(0x080ACA22);
    memset(0x080ACA22,0x90,5);
    memoryProtect(0x080ACA22);

    memoryUnprotect(0x080AD008);
    memset(0x080AD008,0x90,5);
    memoryProtect(0x080AD008);

    //cWon Server Destructor
    memoryUnprotect(0x08201402);
    memset(0x08201402,0x90,5);
    memoryProtect(0x08201402);

    memoryUnprotect(0x080ACB06);
    memset(0x080ACB06,0x90,5);
    memoryProtect(0x080ACB06);


    //cWon Client Destructor
    memoryUnprotect(0x080C8B6A);
    memset(0x080C8B6A,0x90,5);
    memoryProtect(0x080C8B6A);



//////////////////////////////////////////////////////////// MASTER SERVER LIST
    // Don't reset Servers during PIng?
    // memoryUnprotect(0x080C5DFA);
    // memset(0x080C5DFA,0x90,5);
    // memoryProtect(0x080C5DFA);


    // Temporary allow pass
    memoryUnprotect(0x080E4F99);
    memset(0x080E4F99,0x74,1);
    memoryProtect(0x080E4F99);


    // CL_PingServers_f -> allow pass ( no_won check )
    memoryUnprotect(0x080C6037);
        memset(0x080C6037,0x90,2);
    memoryProtect(0x080C6037);
    // Implement our GetServers
    callE8Patch(0x080C6045,&GetServerList);

///////////////////////////////////////////////////////// SERVER TALK TO MASTER
    // Enable Server To Send Master Heartbeats
    callE8Patch(0x80ABF6C, &my_SendToMasters);
    

////////////////////////////////////////////////////
    
    orig_Cmd_AddCommand = createDetour(0x08119514, &my_Cmd_AddCommand,5);
    orig_Cmd_RemoveCommand = createDetour(0x081195BC , &my_Cmd_RemoveCommand, 5);
    orig_Com_Printf = createDetour(0x0811C8F4, &my_Com_Printf,9);

    // research double "fx" commands creation.
    orig_FX_Init = createDetour(0x081439F8, &my_FX_Init,5);

    orig_Sys_GetGameAPI = createDetour(0x08209C50,&my_Sys_GetGameAPI,9);

    orig_Qcommon_Init = createDetour(0x0811E6E8,&my_Qcommon_Init,5);

    orig_AddServer = createDetour(0x080E4F50,&my_AddServer,9);
}

// the non-WSL solution also works on WSL. So not really needed.
#ifdef WSL
char *my_Sys_GetClipboardData(void) {
    char cmd[512] = "powershell.exe -noprofile -command Get-Clipboard";
    // char cmd[512] = "cmd.exe /c %SystemRoot%\\\\SysWOW64\\\\WindowsPowerShell\\\\v1.0\\\\powershell.exe -noprofile -command Get-Clipboard";
    // char cmd[512] = "cmd.exe /c echo %SystemRoot%";
    char output[4096] = {0};
    char *text = malloc(1);
    text[0] = 0x00;

    // Open a pipe to read the output of the command
    FILE *fp = popen(cmd, "r");
    if (!fp) {
        fprintf(stderr, "Error: Failed to run command\n");
        return NULL;
    }

    // Read the output of the command into a buffer
    while (fgets(output, sizeof(output), fp) != NULL) {
        // Concatenate the output to the text buffer
        text = realloc(text, strlen(text) + strlen(output) + 1);
        strcat(text, output);
    }

    // Close the pipe
    pclose(fp);

    return text;
}
#else
char *
 show_utf8_prop(Display *dpy, Window w, Atom p)
{
    char * data = NULL;
    Atom da, incr, type;
    int di;
    unsigned long size, dul;
    unsigned char *prop_ret = NULL;

    /* Dummy call to get type and size. */
    XGetWindowProperty(dpy, w, p, 0, 0, False, AnyPropertyType,
                       &type, &di, &dul, &size, &prop_ret);
    XFree(prop_ret);

    incr = XInternAtom(dpy, "INCR", False);
    if (type == incr)
    {
        printf("Data too large and INCR mechanism not implemented\n");
        return;
    }

    /* Read the data in one go. */
    printf("Property size: %lu\n", size);

    XGetWindowProperty(dpy, w, p, 0, size, False, AnyPropertyType,
                       &da, &di, &dul, &dul, &prop_ret);
    printf("%s", prop_ret);

    if ( size > 0 ) {
        data = malloc(size);
        strncpy(data,prop_ret,size);    
    }
    
    // fflush(stdout);
    XFree(prop_ret);

    /* Signal the selection owner that we have successfully read the
     * data. */
    XDeleteProperty(dpy, w, p);
    return data;
}

char *my_Sys_GetClipboardData(void)
{
    printf("Get Clipboard Data !!\n");

  
    Display *dpy;
    Window owner, target_window, root;
    int screen;
    Atom sel, target_property, utf8;
    XEvent ev;
    XSelectionEvent *sev;

    dpy = XOpenDisplay(NULL);
    if (!dpy)
    {
        fprintf(stderr, "Could not open X display\n");
        return 1;
    }

    screen = DefaultScreen(dpy);
    root = RootWindow(dpy, screen);

    sel = XInternAtom(dpy, "CLIPBOARD", False);
    utf8 = XInternAtom(dpy, "UTF8_STRING", False);

    owner = XGetSelectionOwner(dpy, sel);
    if (owner == None)
    {
        printf("'CLIPBOARD' has no owner\n");
        return 1;
    }
    printf("0x%lX\n", owner);

    /* The selection owner will store the data in a property on this
     * window: */
    target_window = XCreateSimpleWindow(dpy, root, -10, -10, 1, 1, 0, 0, 0);

    /* That's the property used by the owner. Note that it's completely
     * arbitrary. */
    target_property = XInternAtom(dpy, "PENGUIN", False);

    /* Request conversion to UTF-8. Not all owners will be able to
     * fulfill that request. */
    
    XConvertSelection(dpy, sel, utf8, target_property, target_window,
                      CurrentTime);
    char * data = NULL;
    for (;;)
    {
        XNextEvent(dpy, &ev);
        switch (ev.type)
        {
            case SelectionNotify:
                sev = (XSelectionEvent*)&ev.xselection;
                if (sev->property == None)
                {
                    printf("Conversion could not be performed.\n");
                    XDestroyWindow(dpy,target_window);
                    XCloseDisplay(dpy);
                    return NULL;
                }
                else
                {
                    data = show_utf8_prop(dpy, target_window, target_property);
                    XDestroyWindow(dpy,target_window);
                    XCloseDisplay(dpy);
                    return data;
                }
                break;
        }
    }
}
#endif

void my_Cmd_AddCommand(char * cmd, void * callback)
{
    // printf("Command Created : %s\n",cmd);

    if ( !strcmp(cmd,"fx_save") ) {
        void *return_address = __builtin_return_address(0);
        printf("Cmd_AddCommand Return address: %p\n", return_address);
    }
    orig_Cmd_AddCommand(cmd,callback);
}

void my_Cmd_RemoveCommand(char * cmd)
{
    orig_Cmd_RemoveCommand(cmd);
}

/*
 The strings passed here are not part of large buffer, so copy first
*/
void my_Com_Printf(char *msg, ...) {
    char text[1024];
    strcpy(text,msg);
    // point msg back to text
    msg = &text[0];
    int len_msg = strlen(msg) + 1;
    if ( len_msg > 1 && len_msg < 1024 ) // any characters? (including null), require 1 free space.
    {
        // shift to right one. ( grows by 1 )
        // save end character ( its just a null character )
        // start at null character. ( usually -1 to make len into offset )
        // but the null character cancels out ( confusing )
        // explicitly change len to include null cahracter
        for ( char * p=msg+len_msg - 1;p>msg;p-- ) {
            *p = *(p-1);
        }
        *msg=0x06;
        *(msg+len_msg) = 0x00; // fits
    }
    // orig_Com_Printf(msg);
    void *args = __builtin_apply_args();
    __builtin_apply((void (*)())orig_Com_Printf, args, 1000);
}

void my_FX_Init(void)
{
    void *return_address = __builtin_return_address(0);
    printf("FX_Init Return address: %p\n", return_address);

    orig_FX_Init();
}

void my_SendToMasters(void * arg1,void * arg2)
{
    orig_Com_Printf("Attemping to send to masters!\n");

    char * string = orig_SV_StatusString();

    // Com_Printf ("Sending heartbeat to %s\n", NET_AdrToString (master_adr[i]));
    // Netchan_OutOfBandPrint (NS_SERVER, master_adr[i], "heartbeat\n%s", string);
    
}


/*
Calls GetRefAPI from ref_gl.so
also passes in pointers for ref_gl library. ri
*/
/*qboolean VID_LoadRefresh(char *name){
}
*/

/*void * GetRefAPI (void * rimp ) {

}*/

game_export_t * game_exports = NULL;
game_export_t * my_Sys_GetGameAPI (void *params) {

    /*
        This function will call dlopen and pass params to GetGameAPI.
        Its some functions that the game library imports.
    */
    game_exports = orig_Sys_GetGameAPI(params);
    /*
        gamex86.so is loaded here
        ret is a list of function address which are exported to the main executable.
    */

    orig_ShutdownGame = createDetour(game_exports->Shutdown, my_ShutdownGame,5);

    // sv_map -> sv_spawnserver -> spawnentities -> fx_init (remove complain message)
    orig_Cmd_RemoveCommand("fx_save");
    orig_Cmd_RemoveCommand("fx_load");

    return game_exports;
}

void my_ShutdownGame(void)
{
    orig_Com_Printf("Shutting down game!!!\n");
    orig_ShutdownGame();


    orig_Cmd_RemoveCommand("fx_save");
    orig_Cmd_RemoveCommand("fx_load");
    /*
        free all detour mallocs
    */
    free(orig_ShutdownGame);
}

void hexdump(void *addr_start, void *addr_end) {
    unsigned char *p = (unsigned char *)addr_start;
    unsigned char *q = (unsigned char *)addr_end;
    int offset = 0;
    while (p < q) {
        printf("%08x: ", offset);
        for (int i = 0; i < 16 && p + i < q; i++) {
            printf("%02x ", *(p + i));
        }
        for (int i = 16 - (q - p); i > 0; i--) {
            printf("   ");
        }
        for (int i = 0; i < 16 && p + i < q; i++) {
            printf("%c", *(p + i) >= 0x20 && *(p + i) < 0x7f ? *(p + i) : '.');
        }
        printf("\n");
        p += 16;
        offset += 16;
    }
}
/*
 menu_system 0x80 size.
 Created in M_PushMenu.

 Seg fault on menu free?
*/
#include <list>
void my_AddServer(void * menu_system, netadr_t adr, char * serverdata)
{
    
    // orig_Com_Printf((static_cast<std::string*>(static_cast<void*>(serverdata)))->c_str());


    orig_Com_Printf("%s\n",serverdata);
    orig_Com_Printf("hexdump before\n");
    hexdump(menu_system, menu_system+0x80);
    //mapping of a string to a palette
    //map<
        //basic_string<char,string_char_traits<char>,__default_alloc_template<true,0>>,
        //paletteRGBA_c,less<basic_string<char,string_char_traits<char>,__default_alloc_template<true,0>>
        //>,__default_alloc_template<true,0>
        //>
    //>
    // NVM THAT IS RECT.
    // 0x44 = DTINT? 0x080E99FF .. specifically dtint?
    orig_Com_Printf("+0x44 == %s\n",*(char*)(menu_system+0x44));
    // 0x48 = ALIGNMENT? RIGHT/CENTER/LEFT 0x080E9A0D
    orig_Com_Printf("+0x48 == %s\n",*(char*)(menu_system+0x48));


    // segfault in menu_system destructor if this is 0
    // i think its a pointer?
    // set players?
    // std::list<string>

    // std::string * huh = new std::string("0");
   
    // *(int*)(menu_system+0x44) = huh;

    // *(int*)(menu_system+0x48) = huh;

    orig_AddServer(menu_system,adr,serverdata);

    orig_Com_Printf("hexdump after\n");
    hexdump(menu_system, menu_system+0x80);
}

/*

COMMANDS

*/

//44
//48

netadr_t my_netadr = {
    .type = NA_IP, // assuming you want to use IP address type
    .ip = {192, 168, 0, 1}, // assuming your IP address is 192.168.0.1
    .ipx = {0}, // assuming you don't need to use IPX address
    .port = 12345 // assuming you want to use port number 12345
};

//orig_Cmd_Argv(1),orig_Cmd_Argv(2)
void testaddserver(void)
{
    orig_Com_Printf("Attempting to add server!\n");

    // char * tt = "Mad Onion+dm/madrange+0/0/32++DM+63+16+";
    // char* tt = "one+two+0/0/32++DM+63+16+EIGHT+NINE+TEN+11+TWELVE+THIRTEEN+FOURTEEN+FIFTEEN";
    // char * t = malloc(strlen(tt)+1);
    // strcpy(t,tt);

    // std::string t = "Mad Onion+dm/madrange+0+0+32++DM+63+16+";

    // std::string * t = new std::string("SOF+hostname1+mapname+0/0/8+some1+0+0\n");
    // linux: SOF+%16s+%8s+%i/%i/%i+%s+%i+%i\n
    // windows: SOF+%16s+%8s+%i/%i/%i+%s+%s+%i\n

    // requires '/' in mapname
    char t[1024] = "SOF+hostname1+dm/abc+0/0/8+some1+0+0\n";
    void (*test)(netadr_t addr,char *data) = 0x080E08B0;
    test(my_netadr,t);
}

void my_CL_PingServers(void)
{
    orig_Com_Printf("PING!\n");
    testaddserver();

    orig_CL_PingServers();
}

#include "decodekey.h"
void GetServerList(void)
{
 
  orig_Com_Printf("GetServerList!\n");
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    orig_Com_Printf("Error: Failed to create socket.\n");
    return 1;
  }

  struct addrinfo hints;
  struct addrinfo *res;
  std::memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  int status = getaddrinfo("sof1master.megalag.org", nullptr, &hints, &res);
  if (status != 0) {
    error("Error: Failed to resolve hostname.\n");
    return 1;
  }

  struct sockaddr_in server_addr;
  std::memcpy(&server_addr, res->ai_addr, res->ai_addrlen);
  server_addr.sin_port = htons(28900);

  freeaddrinfo(res);

  if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    error("Error: Failed to connect to server.\n");
  }

  const char* message = "IDQD";
  if (send(sockfd, message, std::strlen(message), 0) < 0) {
    error("Error: Failed to send message.\n");
  }

  char buffer[1400];
  std::memset(buffer, 0, sizeof(buffer));
  if (recv(sockfd, buffer, sizeof(buffer), 0) < 0) {
    error("Error: Failed to receive message.\n");
  }

  std::string received_message(buffer);

  // std::cout << "MSG1: " << received_message << '\n';

  // Split the message string by the backslash character '\'
  std::vector<std::string> parts;
  std::istringstream iss(received_message);
  std::string part;
  while (std::getline(iss, part, '\\')) {
    parts.push_back(part);
  }

  // std::cout << "SIZE: " << parts.size();

  std::string realkey = parts[parts.size() - 1];

  // Hash the key using the provided hash function
  // char * ckey = gsseckey(NULL,key.c_str(),"iVn3a3",2);
  std::vector<uint8_t> key = ValidateKey(std::string("iVn3a3"),realkey);
  std::string hashed_key( key.begin(), key.end() );

  // std::cout << "HASHED_KEY: " << hashed_key << '\n';

  std::string response = std::string("\\gamename\\sofretail\\gamever\\1.6\\location\\0\\validate\\") + hashed_key + "\\final\\\\queryid\\1.1\\";
  if (send(sockfd, response.c_str(), response.length(), 0) < 0) {
    error("Error: Failed to send response.\n");
  }

  response = std::string("\\list\\cmp\\gamename\\sofretail\\final\\");
  if (send(sockfd, response.c_str(), response.length(), 0) < 0) {
    error("Error: Failed to send response.\n");
  }

  std::memset(buffer, 0, sizeof(buffer));
  int bytes_returned = recv(sockfd, buffer, sizeof(buffer), 0);
  if ( bytes_returned < 0) {
    error("Error: Failed to receive message.\n");
  }

  // orig_Com_Printf("%.*s\n", bytes_returned, buffer);
  close(sockfd);

  orig_Com_Printf("size : %i\n",bytes_returned);
  // Ignore the first 7 bytes
  unsigned char* data = buffer;
  // Check if the remaining data is divisible by 6 exactly
  // bytes_returned = bytes_returned - 1;
  // std::cout << bytes_returned << '\n';
  // std::cout << bytes_returned % 6 << '\n';
  if ((bytes_returned % 6) != 0 || bytes_returned <= 0) {
      error("Failure getting Server List , bytes returned not correct\n");
  }

  // std::cout << bytes_returned/6 << '\n';

  // Parse the buffer and create netadr_t structures
  std::vector<netadr_t> addrs;
  for (int i = 0; i < bytes_returned - 1; i += 6) {
      netadr_t addr;
      addr.ip[0] = data[i];
      addr.ip[1] = data[i + 1];
      addr.ip[2] = data[i + 2];
      addr.ip[3] = data[i + 3];
      addr.port = (data[i + 4] << 8) | data[i + 5];
      addrs.push_back(addr);
  }
  // std::cout << addrs.size() << '\n';
  // Print the netadr_t structures
  for (int i = 0; i < addrs.size(); i++) {
      std::cout << "Addr " << i << ": "
                << (int)addrs[i].ip[0] << "."
                << (int)addrs[i].ip[1] << "."
                << (int)addrs[i].ip[2] << "."
                << (int)addrs[i].ip[3] << ":"
                << addrs[i].port << std::endl;
  }
}