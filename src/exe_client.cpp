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
	if ( httpdl.joinable() ) {
		orig_Com_Printf("httpdl is joinable... joining\n");
		httpdl.join();
		orig_Com_Printf("Response code was : %li : %i : %s\n",http_status,res,curl_easy_strerror(res));

		// a http download is completed.
		// Lets call RequestNextDownload
		// orig_CL_RequestNextDownload();

	}
}


/*
	The configstrings,ghoulstrings and baseline has been received.
	Now the server wants you to check if they exist locally on disk.
*/
void my_CL_Precache_f(void)
{
	orig_Com_Printf("sof++nix_DEBUG: CL_Precache_F curl_easy_init()\n");
	// curl_handle = curl_easy_init();
	orig_CL_Precache_f();
}

void my_CL_RegisterEffects(void)
{

	orig_Com_Printf("sof++nix_DEBUG: CL_RegisterEffects curl_easy_cleanup()\n");

	// if ( curl_handle != NULL ) curl_easy_cleanup(curl_handle);
	orig_CL_RegisterEffects();
}

void my_CL_Disconnect(short unknown)
{
	orig_Com_Printf("sof++nix_DEBUG: CL_Disconnect curl_easy_cleanup()\n");

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

std::thread httpdl;
qboolean my_CL_CheckOrDownloadFile(char * filepath)
{

	orig_Com_Printf("sof++nix_DEBUG CL_CheckOrDownloadFile: %s\n",filepath);
	// return orig_CL_CheckOrDownloadFile(filepath);

	if (strstr (filepath, ".."))
	{
		Com_Printf ("Refusing to download a path with ..\n");
		return true;
	}

	if (orig_FS_LoadFile (filepath, NULL,false) != -1)
	{	// it exists, no need to download
		return true;
	}

	// std::thread
	httpdl = std::thread(httpdl_thread_get,filepath);

	/*
		Tell the loop that we started a download, so just return.
	*/
	return false;

#if 0
	fp = fopen (name, "r+b");
	if (fp) { // it exists
		int len;
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);

		cls.download = fp;

		// give the server an offset to start the download
		Com_Printf ("Resuming %s\n", cls.downloadname);
		MSG_WriteByte (&cls.netchan.message, clc_stringcmd);
		MSG_WriteString (&cls.netchan.message,
			va("download %s %i", cls.downloadname, len));
	} else {
		Com_Printf ("Downloading %s\n", cls.downloadname);
		MSG_WriteByte (&cls.netchan.message, clc_stringcmd);
		MSG_WriteString (&cls.netchan.message,
			va("download %s", cls.downloadname));
	}

#endif

}

