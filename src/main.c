#include <stdio.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <sys/auxv.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <limits.h>

typedef int qboolean;

typedef void * (* GetRefAPI_type ) (void* rimp );
GetRefAPI_type orig_GetRefAPI = (GetRefAPI_type) NULL;


void* (*real_dlsym)(void *handle, const char *symbol) = NULL;


char * (*orig_getenv)(char * name) = NULL;

void (*orig_Cmd_AddCommand)(char * cmd, void * callback) = NULL;
void my_Cmd_AddCommand(char * cmd, void * callback);

char *my_Sys_GetClipboardData(void);


void * createDetour(void * orig, void * mine, int size);
void callE8Patch(void * addr, void * to);

/*
    Perfect place for initiation code. Earliest possible point.
*/
char *getenv(const char *name)
{
    if ( ! orig_getenv ) {
        orig_getenv = real_dlsym(RTLD_NEXT,"getenv");
        printf("Begin\n\n\n\n\n\n\n\n\n");


        callE8Patch(0x08113315,&my_Sys_GetClipboardData);


        orig_Cmd_AddCommand = createDetour(0x08119514, &my_Cmd_AddCommand,5);
    }
    return orig_getenv(name);
}


void callE8Patch(void * addr,void * to) {
    size_t page_size = getpagesize();

    void * aligned_addr = (void *)((unsigned long)addr & ~(page_size - 1));
    if (mprotect(aligned_addr, page_size, PROT_READ | PROT_WRITE) == -1) {
        perror("mprotect");
        exit(1);
    }

    unsigned char * p = addr;
    p[0] = 0xE8;
    *(int*)(p+1) = to - addr - 5;

    if (mprotect(aligned_addr, page_size, PROT_READ ) == -1) {
        perror("mprotect");
        exit(1);
    }

}

void * createDetour(void * orig, void * mine, int size) {
    size_t page_size = getpagesize();

    void * aligned_orig = (void *)((unsigned long)orig & ~(page_size - 1));
    if (mprotect(aligned_orig, page_size, PROT_READ | PROT_WRITE) == -1) {
        perror("mprotect");
        exit(1);
    }

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
    rel_jmp = mine - orig - 5;
    memcpy(orig,&small_jmp,1);
    memcpy(orig+1,&rel_jmp,4);

    if (mprotect(aligned_orig, page_size, PROT_READ ) == -1) {
        perror("mprotect");
        exit(1);
    }

    // dealloc this on uninit.
    return thunk;
}

void __attribute__ ((constructor)) begin() {
    printf("Constructor\n");
    real_dlsym = _dl_sym(RTLD_NEXT, "dlsym");
    // real_dlsym = dlvsym(RTLD_NEXT, "dlsym", "GLIBC_2.2.5");
    // real_dlsym = __libc_dlsym(handle,symbol);
}

void* dlsym(void *handle, const char *symbol) {
    // printf("Dlsym called : %s\n", symbol);
    
    if (!real_dlsym) {
        /*
        There are two special pseudo-handles, RTLD_DEFAULT and RTLD_NEXT. The former will find the first occurrence of the desired symbol using the default library search order.
        */
        printf("ERROR: dlsym not set\n");
        return NULL;
    }
    void* result = real_dlsym(handle, symbol);
    // printf("Original dlsym returned: %p\n", result);

    // // save the original
    // if ( !strcmp(symbol,"SDL_Init" )  ) {
    //     printf("overriding SDLInit\n");
    //     orig_SDL_Init = result;
    //     result = my_SDL_Init;
    // }
    
    // return the fake
    return result;
}

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
char *my_Sys_GetClipboardData(void)
{
    printf("Get Clipboard Data !!\n");


    Display *display;
    Window owner;
    Atom selection;
    Atom targets;
    Atom property;
    int format;
    unsigned char *data = NULL;
    unsigned long length, bytes_after;

    // Connect to the X server
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Error: Failed to connect to X server\n");
        return NULL;
    }

    // Get the owner of the clipboard
    owner = XGetSelectionOwner(display, XA_PRIMARY);
    if (!owner) {
        fprintf(stderr, "Error: Clipboard not owned\n");
        XCloseDisplay(display);
        return NULL;
    }

    // Specify the selection and target atoms
    selection = XA_PRIMARY;
    targets = XInternAtom(display, "TARGETS", False);

    // Request the available targets for the selection
    XConvertSelection(display, selection, targets, None, None, CurrentTime);

    // Wait for SelectionNotify event
    XEvent event;
    do {
        XNextEvent(display, &event);
    } while (event.type != SelectionNotify || event.xselection.selection != selection);

    if (event.xselection.property == None) {
        fprintf(stderr, "Error: Clipboard selection failed\n");
        XCloseDisplay(display);
        return NULL;
    }

    // Get the property value
    property = event.xselection.property;
    if (XGetWindowProperty(display, owner, property, 0, LONG_MAX/4, False, AnyPropertyType,
                           &targets, &format, &length, &bytes_after, &data) != Success) {
        fprintf(stderr, "Error: Failed to get clipboard data\n");
        XCloseDisplay(display);
        return NULL;
    }

    // Convert data to a string
    char *text = NULL;
    if (format == 8) {
        text = strdup((char *) data);
    } else if (format == 16) {
        text = (char *) malloc((length + 1) * sizeof(char));
        for (int i = 0; i < length; i++) {
            text[i] = ((char *) data)[2 * i];
        }
        text[length] = '\0';
    } else if (format == 32) {
        text = (char *) malloc((length + 1) * sizeof(char));
        for (int i = 0; i < length; i++) {
            text[i] = ((char *) data)[4 * i];
        }
        text[length] = '\0';
    }

    XFree(data);
    XCloseDisplay(display);

    return text;

}
#endif

void my_Cmd_AddCommand(char * cmd, void * callback)
{
    // printf("Command Created : %s\n",cmd);
    return orig_Cmd_AddCommand(cmd,callback);
}



/*
Calls GetRefAPI from ref_gl.so
also passes in pointers for ref_gl library. ri
*/
/*qboolean VID_LoadRefresh(char *name){
}
*/

void * my_GetRefAPI (void * rimp ) {

}
