#include "common.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>

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