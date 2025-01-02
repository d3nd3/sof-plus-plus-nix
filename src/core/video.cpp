#include <SDL/SDL.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"


typedef const SDL_VideoInfo* (*SDL_GetVideoInfo_t)(void);
typedef SDL_Rect** (*SDL_ListModes_t)(SDL_PixelFormat *format, Uint32 flags);
typedef int (*SDL_Init_t)(Uint32 flags);
typedef void (*SDL_Quit_t)(void);
typedef char *(*SDL_GetError_t)(void);


qboolean VID_GetModeInfo(int *width, int *height, int mode);

/*
Calls GetRefAPI from ref_gl.so
also passes in pointers for ref_gl library. ri


--NOT USED--
*/
qboolean my_VID_LoadRefresh(char *name)
{
    SOFPPNIX_DEBUG("my_VID_LoadRefresh\n");
    
    qboolean ret = orig_VID_LoadRefresh(name);

    //Don't need any of this for now. (fortunately)
    return ret;
    /*
        acquire base address.
    */
    Dl_info info;
    void *handle = dlopen(NULL, RTLD_LAZY);
    if (handle == NULL) {
        error_exit("Failed to load current library\n");
    }
    // Sample a ref export function pointer to 'taste' load address of where it lives.
    int rc = dladdr((void*)(*(unsigned int*)(0x08646BE0+4)), &info);
    if (rc == 0) {
        error_exit("Failed to get current library information\n");
    }
    base_addr = info.dli_fbase;
    SOFPPNIX_DEBUG("Video Base address: %08X\n", base_addr);
    dlclose(handle);

    

    return ret;
}


/*void * GetRefAPI (void * rimp ) {

}*/


/*
V_RenderView() calls CalcFov() for client.
cl.refdef.fov_y = CalcFov (cl.refdef.fov_x, cl.refdef.width, cl.refdef.height);

0x829d480 + 0x1A74 = 0x829eef4
0x829d480 + 0x1A64 = 0x829eee4

0x08310FE0 + 0x1A64 = 0x8312a44
*/

//Mouse sensitivity scales with fov In SoF, because hard-coded into IN_MouseMove (For sniper zoom).
float * fov_x = 0x8312a40;
float * fov_y = 0x8312a44;

#define DEG_TO_RAD(angle_in_degrees) ((angle_in_degrees) * M_PI / 180.0)
#define RAD_TO_DEG(angle_in_radians) ((angle_in_radians) * 180.0 / M_PI)

/*
    It is being reset somewhere else back to : 95 0.

    void R_RenderView (refdef_t *fd)
    {
        if (r_norefresh->value)
            return;

        r_newrefdef = *fd;


    typedef struct
    {
        int         x, y, width, height;// in virtual screen coordinates
        float       fov_x, fov_y;
        float       vieworg[3];
        float       viewangles[3];
        float       blend[4];           // rgba 0-1 full screen blend
        float       time;               // time is uesed to auto animate
        int         rdflags;            // RDF_UNDERWATER, etc

        byte        *areabits;          // if not NULL, only areas with set bits will be drawn

        lightstyle_t    *lightstyles;   // [MAX_LIGHTSTYLES]

        int         num_entities;
        entity_t    *entities;

        int         num_dlights;
        dlight_t    *dlights;

        int         num_particles;
        particle_t  *particles;
    } refdef_t;

    The cl.refdef_t structure is passed to R_RenderFrame/View by reference.
    Then the ref library copies the structure into its own memory space.
    Somehow the ref library is resetting its 'copy' to 95.0 0. I am not sure how.

    Seems ref library is only reading the values though.

*/
// We calculate hFov from fixed vFov.
float hFOV_degrees;
float vFOV_degrees = 78.6;
void CalcFovWide(int width, int height)
{
    float aspect_ratio;
    float vFOV_radians, hFOV_radians;

    aspect_ratio = (float)width/height;

    // Convert vertical FOV from degrees to radians
    vFOV_radians = DEG_TO_RAD(vFOV_degrees);

    // Calculate horizontal FOV in radians
    hFOV_radians = 2.0 * atanf(tanf(vFOV_radians / 2.0) * aspect_ratio);

    // Convert horizontal FOV from radians to degrees
    hFOV_degrees = RAD_TO_DEG(hFOV_radians);

    // Output the result
    SOFPPNIX_DEBUG("Current:%f %f Horizontal FOV for aspect ratio %.2f: %.2f degrees\n",*fov_x,*fov_y, aspect_ratio, hFOV_degrees);

}

/*
    Detect when this special value is not 95?

    Called by CL_Frame() -> CL_PredictWeapons()

    This is _before_ SCR_UpdateScreen() which calls V_RenderView() and calculates the dynamic vFOV usually, from fixed 95.0 hFOV.
*/
bool overrideVfov = false;
void my_CalcFov(void)
{
    static float * lerpFov = 0x083D741C;
    orig_CalcFov();

    if ( *lerpFov == 95.0f ) {
        *lerpFov = hFOV_degrees;
        overrideVfov = true;
        // SOFPPNIX_DEBUG("LerpFov is : %f\n",*lerpFov);
    } else {
        SOFPPNIX_DEBUG("NOT_95 is : %f\n",*lerpFov);
    }
}

/*
    Conditionally set vFOV (We want fixed, unless using sniper)
*/
void my_V_RenderView( float stereo_separation )
{
    orig_V_RenderView(stereo_separation);
    if ( overrideVfov ) {
        *fov_y = vFOV_degrees;
        overrideVfov = false;
    }
}

/*
    Called by CL_ParseServerData and CL_Disconnect, but CL_Disconnect is inlined.
*/
//NOT USED--
void my_CL_ClearState(void)
{
    //NOT USED--
    float fovx_before = *fov_x;
    float fovy_before = *fov_y;
    orig_CL_ClearState();

    //restore fov?
    // *fov_x = fovx_before;
    // *fov_y = fovy_before;
}
//NOT USED--
void my_CL_Disconnect(short unknown)
{
    float fovx_before = *fov_x;
    float fovy_before = *fov_y;
    orig_CL_Disconnect(unknown);
    //restore fov?
    // *fov_x = fovx_before;
    // *fov_y = fovy_before;
}


/*
    Warning: You can't call the original VID_GetModeInfo because
    it has some awful stack dependent machine code compiled into it.
    I don't know what it expects, but its ugly.

    Re-implement it instead.

    Called by R_Init() -> R_SetMode() -> GLimp_SetMode()

    R_BeginFrame() -> if (gl_mode->modified) ref->modified (Reloads vid_ref).
*/
qboolean my_VID_GetModeInfo(int *width, int *height, int mode)
{

    SOFPPNIX_DEBUG("VID_GetModeInfo %08X\n", orig_VID_GetModeInfo);

    // Function pointers
    static SDL_GetVideoInfo_t SDL_GetVideoInfo = (SDL_GetVideoInfo_t)dlsym(RTLD_DEFAULT, "SDL_GetVideoInfo");
    static SDL_ListModes_t SDL_ListModes = (SDL_ListModes_t)dlsym(RTLD_DEFAULT, "SDL_ListModes");
    // SDL_Init_t SDL_Init = (SDL_Init_t)dlsym(RTLD_DEFAULT, "SDL_Init");
    static SDL_Quit_t SDL_Quit = (SDL_Quit_t)dlsym(RTLD_DEFAULT, "SDL_Quit");
    static SDL_GetError_t SDL_GetError = (SDL_GetError_t)dlsym(RTLD_DEFAULT, "SDL_GetError");

    // Check for errors
    char *error;
    if ((error = dlerror()) != NULL) {
        error_exit("Failed to get function addresses: %s\n", error);
    }

    #if 0
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    #endif
    // Get the current video information
    const SDL_VideoInfo *videoInfo = SDL_GetVideoInfo();
    if (!videoInfo) {
        error_exit("Failed to get video information: %s\n", SDL_GetError());
    }

    // Get the list of available video modes
    SDL_Rect **modes = SDL_ListModes(NULL, SDL_FULLSCREEN);
    if (modes == (SDL_Rect **)0) {
        error_exit("No modes available\n");
    }


    //Why??? [This still has to set the mode I think??] TODO: Update vid_modes.
    if (modes == (SDL_Rect **)-1) {
        error_exit("All resolutions available\n");
        return false;
    }

    // Count the number of available modes
    int count = 0;
    SOFPPNIX_DEBUG("VID_GetModeInfo: Printing Modes\n");
    while (modes[count]) {
        SOFPPNIX_DEBUG("%i %i\n",modes[count]->w,modes[count]->h);
        count++;
    }
    SOFPPNIX_DEBUG("VID_GetModeInfo: --------\n");

    if (mode < 0 || mode >= count) {

        SOFPPNIX_DEBUG("VID_GetModeInfo: Mode is not valid\n");
        return false;
    }

    if (mode == 0) {
        *width = videoInfo->current_w;
        *height = videoInfo->current_h;

        CalcFovWide(*width, *height);
        SOFPPNIX_DEBUG("Setting monitor mode (%i %i) because gl_mode 0\n",*width, *height);
        return true;
    }

    /*
        Else return the width and height of the mode specified.
    */
    *width = modes[mode]->w;
    *height = modes[mode]->h;

    CalcFovWide(*width, *height);

    SOFPPNIX_DEBUG("Setting to resolution: %i %i\n", *width, *height);
    return true;
}
