#include "common.h"

#include <curl/curl.h>

#define MAP_POOL "https://raw.githubusercontent.com/plowsof/sof1maps/main"



enum enum_dl_status download_status;

bool http_in_progress = false;
// Track which zip files have been downloaded & extracted.
std::list<std::string> httpdl_cache_list;

// global for fclose
FILE * local_new_file = NULL;

// url is passed as userdata into write callback function
// because in there it checks the status code, only calls fopen && dir creation,  if == 200.
// curl_easy_getinfo requires access to curl pointer. (would have to pass struct in userdata otherwise).
CURL* curl = NULL;

// currently using libcurl/7.74.0 OpenSSL/1.1.1n zlib/1.2.11 brotli/1.0.9 libidn2/2.3.0 
// libpsl/0.21.0 (+libidn2/2.3.0) libssh2/1.9.0 nghttp2/1.43.0 librtmp/2.3

/*
	if ( http_in_progress ) {
		if ( download_status ) {
	
		}
		http_in_progress = false;
	}
*/
bool beginHttpDL(std::string * relative_file_path_name)
{
	SOFPPNIX_PRINT("Requested start DL for map : %s\n",relative_file_path_name->c_str());
	if ( download_status != DS_UNCERTAIN ) {
		SOFPPNIX_PRINT("Http Download already in progress!!\n");
		return false;
	}
	if ( searchHttpCache( std::string(relative_file_path_name->c_str()) ) ) {
		SOFPPNIX_PRINT("Refusing to download a map zip that is in your cache\n");
		return false;
	}
	download_status = DS_UNCERTAIN;
	std::thread httpdl(httpdl_thread_get,new std::string(*relative_file_path_name));
	httpdl.detach();
	return true;
}

/*
	resets download_status to DS_UNCERTAIN
*/
int getHttpStatus(void)
{
	enum enum_dl_status ret = DS_UNCERTAIN;
	if ( download_status != DS_UNCERTAIN ) {
		ret = download_status;
		download_status = DS_UNCERTAIN;
	}
	return ret;
}


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
	

	// SOFPPNIX_PRINT("httpdl_writecb : %08X %i %i %08X\n", ptr,size,nmemb,userdata);

	long http_status = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
	// SOFPPNIX_PRINT("RESPONSE_CODE: %li\n",http_status);
	if (http_status == 200) {
		if ( local_new_file == NULL ) {
			create_file_dir_if_not_exists(url);
			local_new_file = fopen(url, "wb");
			if ( local_new_file == NULL ) {
				SOFPPNIX_PRINT("httpdl_error: could not create the file locally.\n");
				return -1;
			}
		}
		// open already
		// SOFPPNIX_PRINT("Writing because response code = %li\n",http_status);
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
			SOFPPNIX_PRINT("httpdl_writecb: COMPLETION DETECTED!\n");
		}
		#endif
		return written;
	}

	// Stop downloading
	return -1;
}


#if 0
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
#endif

/*
	url as given by configstrings.

	the MAP_POOL provides a .zip bundle using the mapname/path.
*/
void httpdl_thread_get(std::string * rel_map_path) {

	char * map_path = rel_map_path->c_str();
	SOFPPNIX_PRINT("URL is : %s\n",map_path);

	char * userdir = orig_FS_Userdir();
	char write_zip_here[256];
	snprintf(write_zip_here,256,"%s p/%s",userdir,map_path);
	SOFPPNIX_PRINT("Local URL is : %s\n",write_zip_here);
	

	curl = curl_easy_init();
	if (!curl) {
		std::cerr << "Error initializing libcurl" << std::endl;
		return;
	}

	// Th
	char remote_zip_weburl[256];
	snprintf(remote_zip_weburl,256,"%s/%s",MAP_POOL,map_path);

	SOFPPNIX_PRINT("Remote URL is : %s\n",remote_zip_weburl);
	curl_easy_setopt(curl, CURLOPT_URL, &remote_zip_weburl);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, (size_t(*)(void*, size_t, size_t, void*))&httpdl_writecb);
	// curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_zip_here);

	// curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
	// curl_easy_setopt(curl, CURLOPT_HEADERDATA, &http_status);

	bool error = false;
	// BLOCKS!
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		SOFPPNIX_PRINT("Error downloading %s : %s\n", map_path ,curl_easy_strerror(res));
		error = true;
	}

	// Did the write callback open a file? Would had only done so if response code was 200.
	if ( local_new_file ) {
		fclose(local_new_file);
		local_new_file = NULL;
	}
	curl_easy_cleanup(curl);


	/*
		NOT SO FAST! YOU HAVE TO UNZIP IT!

		I was initially making the error checking by the callee.
		But why? I can put it all here.

		We should have a healthy zip file here. ASSUMPTION?

		download_status == false by default
	*/
	
	if ( !error ) {
		SOFPPNIX_PRINT("Preparing to unzip %s\n",write_zip_here);
		// all happy?
		if ( !unZipFile( write_zip_here, userdir) )
		{
			SOFPPNIX_PRINT("Unzip failure!\n");
			download_status = DS_FAILURE;
		} else
		{
			// keep track of the extracted zip files in a permanant file
			appendItemHttpCache(map_path);
			saveHttpCache();
			download_status = DS_SUCCESS;
		}
	} else {
		download_status = DS_FAILURE;
	}

	delete rel_map_path;
}


// ------------------------------- ZIP -------------------------------------

#include <minizip/unzip.h>

void closeAndDeleteZipFile(unzFile * unz_file,char * zip)
{
	unzClose(unz_file);
	std::remove(zip);
}

/*
By default overwrites any files that already exist with same path
*/
bool unZipFile(char * in_zip, char * out_root )
{
	// ------------------------- OPEN ZIP FILE ------------------------
	unzFile unz_file = unzOpen(in_zip);
	if (unz_file == NULL) {
		std::cerr << "Error: cannot open zip file." << std::endl;
		return false;
	}

	// Set the current file of the zip archive to the first file
	if (unzGoToFirstFile(unz_file) != UNZ_OK) {
		SOFPPNIX_PRINT("Error corrupt zip %s?\n",in_zip);
		closeAndDeleteZipFile(unz_file,in_zip);
		return false;
	}

	// Extract each file in the zip archive to the output directory
	char filename[MAX_PATH];
	do {
		// Get info about the first file in the archive
		unz_file_info file_info;

		// Get the filename of the current file in the zip archive
		if (unzGetCurrentFileInfo(unz_file, &file_info, filename, MAX_PATH, NULL, 0, NULL, 0) != UNZ_OK) {
			SOFPPNIX_PRINT("Error corrupt zip %s?\n",in_zip);
			closeAndDeleteZipFile(unz_file,in_zip);
			return false;
		}
		
		SOFPPNIX_PRINT("Processing file : %s : size : %lu\n",filename,file_info.uncompressed_size);
		
		//----------------------OPEN READ FILE----------------------
		if (unzOpenCurrentFile(unz_file) != UNZ_OK) {
			SOFPPNIX_PRINT("Error opening %s, corrupt zip %s?\n",filename,in_zip);
			closeAndDeleteZipFile(unz_file,in_zip);
			return false;
		}
		
		// FULL DEST PATH CREATION
		
		std::string output_file_path = out_root + std::string("/") + std::string(filename);
		create_file_dir_if_not_exists(output_file_path.c_str());

		//----------------------OPEN WRITE FILE----------------------
		FILE* file_to_be_written = std::fopen(output_file_path.c_str(), "wb");
		if (file_to_be_written == NULL) {
			//----------------------CLOSE READ FILE--------------------
			unzCloseCurrentFile(unz_file);

			closeAndDeleteZipFile(unz_file,in_zip);
			// CANNOT READ A FILE ERROR!
			SOFPPNIX_PRINT("Cannot write to : %s : when extracting : %s\n",output_file_path.c_str(),in_zip);
			return false;
		}
		
		// Extract the current file in the zip archive to the output file
		char buffer[4096];
		int bytes_read;
		do {
			bytes_read = unzReadCurrentFile(unz_file, buffer, sizeof(buffer));
			if (bytes_read < 0) {
				// -------------------ERROR-------------------
				std::cerr << "Error: error while reading current file." << std::endl;
				//----------------------CLOSE WRITE FILE--------------------
				std::fclose(file_to_be_written);
				std::remove(output_file_path.c_str());
				//----------------------CLOSE READ FILE--------------------
				unzCloseCurrentFile(unz_file);

				closeAndDeleteZipFile(unz_file,in_zip);

				// CANNOT READ A FILE ERROR!
				SOFPPNIX_PRINT("This zip is corrupt? cannot read correctly : %s\n",in_zip);
				return false;
			}
			// Write the bytes
			if (bytes_read > 0) {
				std::fwrite(buffer, bytes_read, 1, file_to_be_written);
			}
		} while (bytes_read > 0);
		// if bytes_read <= 0 break.

		//----------------------CLOSE WRITE FILE--------------------
		std::fclose(file_to_be_written);
		// no deletion because its success.

		//----------------------CLOSE READ FILE--------------------
		unzCloseCurrentFile(unz_file);

		// maybe this is just to find the end of the zip ? :D not error.
		if ( unzGoToNextFile(unz_file) != UNZ_OK ) {
			
			// SOFPPNIX_PRINT("WARNING: Partial extraction of : %s\n",in_zip);
			// SOFPPNIX_PRINT("This zip might have polluted your directories\n");

			// success? :P
			break;
			// closeAndDeleteZipFile(zip_file);
			// return false;
		}
	} while (true);
	// break if nextFile != UNZ_OK

	closeAndDeleteZipFile(unz_file,in_zip);

	SOFPPNIX_PRINT("Successfully extracted and deleted : %s\n",in_zip);
	return true;
}


#define HTTPDL_CACHE_NAME "httpdl_cache.txt"
/*
	Call this after download complete success.
*/
void appendItemHttpCache(const std::string& zipfile) {
	SOFPPNIX_PRINT("Appending %s\n",zipfile.c_str());
	// Convert the new line to lower case for case-insensitive comparison 
	std::string lower_zipfile = zipfile;
	std::transform(lower_zipfile.begin(), lower_zipfile.end(), lower_zipfile.begin(), ::tolower);
	// Find the position where the new line should be inserted in a sorted order using lower_bound algorithm 
	auto pos = std::lower_bound(httpdl_cache_list.begin(), httpdl_cache_list.end(), lower_zipfile);
	// Insert the new line at that position using insert method of list container 
	httpdl_cache_list.insert(pos, zipfile);

	for (const auto& azip : httpdl_cache_list) {
		SOFPPNIX_PRINT("%s\n", azip.c_str());
	}
}

// The function returns true if removed and false otherwise  
bool removeItemHttpCache(const std::string& zipfile) {
	bool found = searchHttpCache(zipfile);
	if (found) {
		auto it = find(httpdl_cache_list.begin(), httpdl_cache_list.end(), zipfile);
		httpdl_cache_list.erase(it);
		return true;
	} else {
		return false;
	}
}

// A function to search for an exact match of a string in the global list using binary search algorithm 
// The function assumes that the list is sorted in ascending order 
// The function returns true if found and false otherwise 
bool searchHttpCache(const std::string& target) {
	// Convert the target string to lower case for case-insensitive comparison 
	std::string lower_target = target;
	std::transform(lower_target.begin(), lower_target.end(), lower_target.begin(), ::tolower);

	// Use binary search algorithm on the sorted list 
	auto first = httpdl_cache_list.begin();
	auto last = httpdl_cache_list.end();

	while (first != last) {
		auto middle = first;
		std::advance(middle, std::distance(first, last) /2); 
		if (*middle == lower_target) { 
			return true; 
		} else if (*middle < lower_target) { 
			first = ++middle; 
		} else { 
			last = middle; 
		}
	}

	return false; 
}

/*
	Call this after a download complete sucess.

	Creates file if not exist
*/
void saveHttpCache(void)
{
	SOFPPNIX_PRINT("Saving "HTTPDL_CACHE_NAME"\n");
	std::ofstream diskcache(HTTPDL_CACHE_NAME, std::ios::out);

	if (diskcache.is_open()) {
		for (const auto& azip : httpdl_cache_list) {
			diskcache << azip << "\n";
		}
		diskcache.close();
	}
	else {
		// Print an error message if the file cannot be opened
		SOFPPNIX_PRINT("Unable to saveHttpCache "HTTPDL_CACHE_NAME"\n");
	}
}

/*
	Only Load the cache once at startup.

	To create a file if it does not exist,
	 you need to use std::ios::out flag when opening the file stream2.
*/
void loadHttpCache(void) {
	std::ifstream diskcache(HTTPDL_CACHE_NAME);

	httpdl_cache_list.clear();
	if (diskcache.is_open()) {
		std::string line;
		while (std::getline(diskcache, line)) {
			httpdl_cache_list.push_back(line);
		}
		diskcache.close();
	} else {
		SOFPPNIX_PRINT("No "HTTPDL_CACHE_NAME" found, maps will be re-downloaded\n");
	}
}