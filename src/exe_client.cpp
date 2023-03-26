#include "common.h"
/*
typedef struct cvar_s
{
	char			*name;
	char			*string;
	char			*latched_string;	// for CVAR_LATCH vars
	int				flags;
	cvarcommand_t	command;
	qboolean		modified;	// set each time the cvar is changed
	float			value;
	struct cvar_s *	next;
} cvar_t;
*/

/*
	Strings alloced by M_Alloc in Cvar_Get // CopyString

	decimals where 1/N , where N is power of 2 can be represented exactly as floating point.
*/


void my_CL_Frame(int msec)
{

	orig_CL_Frame(msec);

	// 10 fps slow ... :) ca_connecting

	// Assume finished
	int http_status = getHttpStatus();
	switch ( http_status) {
	case DS_FAILURE:
		SOFPPNIX_PRINT(":'( Download failed!\n");
		break;
	case DS_SUCCESS:
		SOFPPNIX_PRINT(":'( Download succeeeded!\n");
		break;
	}

	/*
		HTTP DL COMPLETE.
		When the user doesn't need to download because its in their cache.
		Or another http download is in progress ( shouldn't be :/ )
		http_status is DS_UNCERTAIN in these cases but we still want to connect to server.
		NVM, precache will be called from the precache hook instad for those sitautions
	*/
	if ( http_status != DS_UNCERTAIN ) {
		// IF SUCCESS - CONTINUE - ( more files )
		// IF FAILURE - CONTINUE - ( old fashioned download )
		orig_CL_Precache_f();
	}


	// if ( !download_status ) {
		// TODO : decrement the index counter so that it tries again with normal download?
		// temporary solution:
		// only download mapname before iterate configstrings
	// }
	// a http download is completed.
	// Lets call RequestNextDownload
	// orig_CL_RequestNextDownload();
}


/*
	The configstrings,ghoulstrings and baseline has been received.
	Now the server wants you to check if they exist locally on disk.

	This function calls CL_RequestNextDownload() then returns.
*/
void my_CL_Precache_f(void)
{
	SOFPPNIX_PRINT("CL_Precache_F curl_easy_init()\n");
	
	// configstrings CS_NAME = *(unsigned int*)(0x0829D480)+0x1EFC+0x48

	// CM_LoadMap mapname = 0x829DB40

	// configstrings ALTERNATE = *(unsigned int*)(0x0829D480) + 0x2844

	char * mapname = *(unsigned int*)(0x0829D480) + 0x2844;
	
	std::string filePath(mapname);
	std::string newExtension = ".zip";
	size_t lastDotIndex = filePath.find_last_of(".");
	if (lastDotIndex != std::string::npos) {
		filePath.replace(lastDotIndex, newExtension.length(), newExtension);
	}

	if ( filePath.compare(0,5,"maps/") == 0 )
		filePath.erase(0,5);
	
	// Thread might not be created, check return value of this function to see if it created thread
	// Start the download. Rely upon CL_Frame code to run the preacache.
	if ( ! beginHttpDL(&filePath) ) {
		// ALREADY IN CACHE?
		orig_CL_Precache_f();
	}

	// mapname is set inside CL_Precache @CL_LoadMap
	// Actually its not set until the very last call of CL_RequestNextDownload()
	// Which is far into future :P

	
}

void my_CL_RegisterEffects(void)
{

	SOFPPNIX_PRINT("CL_RegisterEffects curl_easy_cleanup()\n");

	// if ( curl_handle != NULL ) curl_easy_cleanup(curl_handle);
	orig_CL_RegisterEffects();
}

void my_CL_Disconnect(short unknown)
{
	SOFPPNIX_PRINT("CL_Disconnect curl_easy_cleanup()\n");

	// if ( curl_handle != NULL ) curl_easy_cleanup(curl_handle);
	orig_CL_Disconnect(unknown);
}

/*
	CL_RequestNextDownload() loop will iteratively call this.
	Its important that we call CL_RequestNextDownload() again to flow back into the loop.
	Yes, the callback function will call CL_RequestNextDownload().
	Every Curl response determines whether it flows back into CL_RequestNextDownload or hangs(disconnect).

	Returns TRUE: when it doens't allow download.
		If the path contains ".." returns TRUE.
		If the FS_LoadFile != -1 returns TRUE ( exists ).

	Sets up a .tmp file in the location where the file should exist.
	Opens the file and sends the "download %s" to the server. (SV_BeginDownload).


	Don't mess with the cl_download cvars if you are using http.
*/

#if 0

qboolean my_CL_CheckOrDownloadFile(char * filepath)
{

	SOFPPNIX_PRINT("CL_CheckOrDownloadFile: %s\n",filepath);
	// return orig_CL_CheckOrDownloadFile(filepath);

	if (strstr (filepath, ".."))
	{
		SOFPPNIX_PRINT ("Refusing to download a path with ..\n");
		return true;
	}

	if (orig_FS_LoadFile (filepath, NULL,false) != -1)
	{	// it exists, no need to download
		return true;
	}

	// Initiate download of mapfile.zip
	// Even if they already have it?
	// Difficult problem. Need a file list?
	// Under which condition do I initiate download of the mapname .zip ?
	// Bulk download vs individual file download.

	// Perhaps a httpdl_history_cache.txt
	// Contains a list of zips downloaded.

	// If the zip_path exists in that file, it doesn't download.
	
	httpdl = std::thread(httpdl_thread_get,filepath);

	/*
		Tell the loop that we started a download, so just return.
	*/
	return false;

}
#endif
