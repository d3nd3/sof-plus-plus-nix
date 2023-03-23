#include "common.h"

#include <stdio.h>
#include <curl/curl.h>

#define MAP_POOL "https://raw.githubusercontent.com/plowsof/sof1maps/main"
CURLcode res;
long http_status = 0;
FILE * local_new_file = NULL;
CURL* curl = NULL;

// currently using libcurl/7.74.0 OpenSSL/1.1.1n zlib/1.2.11 brotli/1.0.9 libidn2/2.3.0 
// libpsl/0.21.0 (+libidn2/2.3.0) libssh2/1.9.0 nghttp2/1.43.0 librtmp/2.3

/*
Your callback should return the number of bytes actually taken care of.
 If that amount differs from the amount passed to your callback function,
  it will signal an error condition to the library. 
  This will cause the transfer to get aborted and the libcurl function
   used will return CURLE_WRITE_ERROR.

You can also abort the transfer by returning CURL_WRITEFUNC_ERROR. (7.87.0)

If your callback function returns CURL_WRITEFUNC_PAUSE it will cause this
 transfer to become paused. See curl_easy_pause for further details.
*/

size_t httpdl_writecb(void *ptr, size_t size, size_t nmemb, void *userdata) {
	char * url = (char*)userdata;
	

	orig_Com_Printf("httpdl_writecb : %08X %i %i %08X\n", ptr,size,nmemb,userdata);

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
	orig_Com_Printf("RESPONSE_CODE: %li\n",http_status);
	if (http_status == 200) {
		if ( local_new_file == NULL ) {
			create_file_dir_if_not_exists(url);
			local_new_file = fopen(url, "wb");
			if ( local_new_file == NULL ) {
				orig_Com_Printf("httpdl_error: could not create the file locally.\n");
				return -1;
			}
		}
		// open already
		orig_Com_Printf("Writing because response code = %li\n",http_status);
		size_t written = fwrite(ptr, size, nmemb, local_new_file);
		#if 0
		// COMPLETION CHECK
		// IF SEEK_SET == SEEK_END
		long int pos = ftell((FILE*)local_new_file);
		fseek((FILE*)local_new_file, 0, SEEK_END);
		long int filesize = ftell((FILE*)local_new_file);
		fseek((FILE*)local_new_file, pos, SEEK_SET);

		if (filesize == pos) {
			// The entire file has been downloaded, so we can close the file
			orig_Com_Printf("httpdl_writecb: COMPLETION DETECTED!\n");
		}
		#endif
		return written;
	}

	// Stop downloading
	return -1;
}


// callback function to handle the response headers
size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata)
{
    int *http_status = (int *)userdata;
    char *http_version = NULL;

    // parse the response status line to get the status code
    if (sscanf(buffer, "HTTP/%*s %d", http_status) == 1) {
        return size * nitems;
    }

    return 0;
}

void httpdl_thread_get(const char* url) {
	http_status = 0;
	orig_Com_Printf("URL is : %s\n",url);

	char local_url[256];
	snprintf(local_url,256,"%s/%s",orig_FS_Userdir(),url);
	orig_Com_Printf("Local URL is : %s\n",local_url);
	

	curl = curl_easy_init();
	if (!curl) {
		std::cerr << "Error initializing libcurl" << std::endl;
		return;
	}

	char remote_url[256];
	snprintf(remote_url,256,"%s/%s",MAP_POOL,url);

	orig_Com_Printf("Remote URL is : %s\n",remote_url);
	curl_easy_setopt(curl, CURLOPT_URL, &remote_url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, (size_t(*)(void*, size_t, size_t, void*))&httpdl_writecb);
	// curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &local_url);

	// curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
	// curl_easy_setopt(curl, CURLOPT_HEADERDATA, &http_status);

	// BLOCKS!
	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		std::cerr << "Error downloading " << url << ": " << curl_easy_strerror(res) << std::endl;
	}

	if ( local_new_file ) {
		fclose(local_new_file);
		local_new_file = NULL;
	}

	curl_easy_cleanup(curl);
}


// ------------------------------- ZIP -------------------------------------

#if 0
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include "minizip/unzip.h"

int main() {
	const std::string zip_file_path = "path/to/zip/file.zip";
	const std::string output_dir_path = "path/to/output/dir/";
	
	// Open the zip archive for reading
	unzFile zip_file = unzOpen(zip_file_path.c_str());
	if (zip_file == NULL) {
		std::cerr << "Error: cannot open zip file." << std::endl;
		return 1;
	}
	
	// Set the current file of the zip archive to the first file
	if (unzGoToFirstFile(zip_file) != UNZ_OK) {
		std::cerr << "Error: cannot find first file in zip archive." << std::endl;
		unzClose(zip_file);
		return 1;
	}
	
	// Extract each file in the zip archive to the output directory
	char filename[256];
	do {
		// Get the filename of the current file in the zip archive
		if (unzGetCurrentFileInfo(zip_file, NULL, filename, sizeof(filename), NULL, 0, NULL, 0) != UNZ_OK) {
			std::cerr << "Error: cannot get file info for current file." << std::endl;
			unzClose(zip_file);
			return 1;
		}
		
		// Open the current file in the zip archive for reading
		if (unzOpenCurrentFile(zip_file) != UNZ_OK) {
			std::cerr << "Error: cannot open current file for reading." << std::endl;
			unzClose(zip_file);
			return 1;
		}
		
		// Create the output file in the output directory for writing
		std::string output_file_path = output_dir_path + std::string(filename);
		FILE* output_file = std::fopen(output_file_path.c_str(), "wb");
		if (output_file == NULL) {
			std::cerr << "Error: cannot create output file." << std::endl;
			unzCloseCurrentFile(zip_file);
			unzClose(zip_file);
			return 1;
		}
		
		// Extract the current file in the zip archive to the output file
		char buffer[4096];
		int bytes_read;
		do {
			bytes_read = unzReadCurrentFile(zip_file, buffer, sizeof(buffer));
			if (bytes_read < 0) {
				std::cerr << "Error: error while reading current file." << std::endl;
				std::fclose(output_file);
				std::remove(output_file_path.c_str());
				unzCloseCurrentFile(zip_file);
				unzClose(zip_file);
				return 1;
			}
			if (bytes_read > 0) {
				std::fwrite(buffer, bytes_read, 1, output_file);
			}
		} while (bytes_read > 0);
		
		// Close the output file and the current file in the zip archive
		std::fclose(output_file);
		unzCloseCurrentFile(zip_file);
	} while (unzGoToNextFile(zip_file) == UNZ_OK);
	
	// Close the zip archive
	unzClose(zip_file);
	
	return 0;
}

#endif