#include "common.h"
/*
 The strings passed here are not part of large buffer, so copy first
*/
void my_Com_Printf(char *msg, ...) {
    #if 0
    char text[1024];
    msg = &text[0];
    strcpy(text,msg);
    // point msg back to text
    
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
        *msg=0x03;
        *(msg+len_msg) = 0x00; // fits
    }

    // orig_Com_Printf(msg);

    void *args = __builtin_apply_args();

    __builtin_apply((void (*)())orig_Com_Printf, args, 512);
    #else
        char text[1023];
        va_list args;

        va_start(args, msg);
        vsnprintf(text, sizeof(text), msg, args);
        va_end(args);

        char color[1024] = {0x03,0x00};
        strcat(color,text);

        orig_Com_Printf(color);
        
    #endif
}


void my_Cmd_AddCommand(char * cmd, void * callback)
{
    // printf("Command Created : %s\n",cmd);

    // if ( !strcmp(cmd,"fx_save") ) {
    //     void *return_address = __builtin_return_address(0);
    //     printf("Cmd_AddCommand Return address: %p\n", return_address);
    // }
    orig_Cmd_AddCommand(cmd,callback);
}

void my_Cmd_RemoveCommand(char * cmd)
{
    orig_Cmd_RemoveCommand(cmd);
}

