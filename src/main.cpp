
#include <stdio.h>
#include <dlfcn.h>

typedef int qboolean;

typedef void * (* GetRefAPI_type ) (void* rimp );
// GetRefAPI_type orig_GetRefAPI = (GetRefAPI_type)

void start() {
	printf("HELLO WORLD");
}
void * loadOriginal(char * lib_name,char * func_name) {

    void *handle;
    void* func;
    const char *error;

    handle = dlopen("lib_name", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error: %s\n", dlerror());
        return NULL;
    }

    func = (void*)dlsym(handle, func_name);
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "Error: %s\n", error);
        dlclose(handle);
        return NULL;
    }

    dlclose(handle);

    return func;
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
