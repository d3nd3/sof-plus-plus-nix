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


/*
	url as given by configstrings.

	the MAP_POOL provides a .zip bundle using the mapname/path.
*/
void httpdl_thread_get(std::string * rel_map_path) {
	// local_new_file handler is opened in FULL DOWNLOAD CURLOPT_WRITEFUNCTION stage.
	// other stages write to memory.

	char * map_path = rel_map_path->c_str();
	SOFPPNIX_PRINT("URL is : %s\n",map_path);

	char * userdir = orig_FS_Userdir();
	char write_zip_here[256];
<<<<<<< Updated upstream
	/*
		some maps are called maps/dm/
		others just dm/
	*/
	snprintf(write_zip_here,256,"%s/maps/%s",userdir,map_path);
=======
	snprintf(write_zip_here,256,"%s/%s",userdir,map_path);
>>>>>>> Stashed changes
	SOFPPNIX_PRINT("Local URL is : %s\n",write_zip_here);
	
	char remote_zip_weburl[256];
	snprintf(remote_zip_weburl,256,"%s/%s",MAP_POOL,map_path);
	SOFPPNIX_PRINT("Remote URL is : %s\n",remote_zip_weburl);

	/*
		-------------------------PARTIAL DOWNLOAD-----------------------------------
	*/
	// CALL CRC CHECKS HERE
	// GET CONTENT LENGTH
	// GET 100 BYTE BLOBS FROM END
	// IF ANY OF CRC DO NOT MATCH JUST DOWNLOAD ENTIRE ZIP.


	if ( !partialHttpBlobs(&remote_zip_weburl[0]) ) {
		delete rel_map_path;
		download_status = DS_FAILURE;
		return;
	}
	/*
		---------------------------FULL DOWNLOAD--------------------------------------
	*/

	curl = curl_easy_init();
	if (!curl) {
		std::cerr << "Error initializing libcurl" << std::endl;
		return;
	}
	// --------------URL-----------------
	curl_easy_setopt(curl, CURLOPT_URL, &remote_zip_weburl);


	// ---------------BODY------------------
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, (size_t(*)(void*, size_t, size_t, void*))&httpdl_writecb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_zip_here);


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
	------------------UNZIP-----------------------
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



// userdata = int length
// return length.
size_t header_cb_get_content_length(char *buffer, size_t size, size_t nitems, void *userdata)
{
	size_t numbytes = size * nitems;
	char *header = (char *)malloc(numbytes + 1);
	memcpy(header, buffer, numbytes);
	header[numbytes] = '\0';

	SOFPPNIX_PRINT("%s\n",header);

	// Parse the Content-Length header
	const char *prefix = "Content-Length: ";
	char *content_length = strcasestr(header, prefix);
	int length = 0;
	if (content_length)
	{
		SOFPPNIX_PRINT("FOUND CONTENT LENGTH\n");
		content_length += strlen(prefix);
		SOFPPNIX_PRINT("%s\n",content_length);
		length = atoi(content_length);
		*((int *)userdata) = length;
	} else {
		SOFPPNIX_PRINT("FOUND CONTENT LENGTH\n");
	}

	free(header);
	SOFPPNIX_PRINT("LENGTH WAS %i\n",length);
	return -1;
}


// pass buffer to it
size_t partial_blob_100_cb(void *ptr, size_t size, size_t nmemb, void *userdata) {
	char * url = (char*)userdata;

	unsigned char * out_buffer = userdata;

	long http_status = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);

	if (http_status == 200) {
		
		size_t total_size = size * nmemb;
		SOFPPNIX_PRINT("Total Size of Partial Download = %i\n",total_size);
		if (total_size != 100 ) return -1;

		size_t written = 0;
		char *buf = (char *)ptr;

		memcpy(out_buffer , buf, 100);

		return 100;
	}
	
	// Stop downloading
	return -1;
}
/*
	Download end of file in chunks until signature is found indicating Central Directory Offset
	100 byte chunks?
*/
bool partialHttpBlobs(char * remote_url)
{
	bool error = false;

	/*
		Get the Content Length One time only.
	*/
	curl = curl_easy_init();
	if (!curl) {
		std::cerr << "Error initializing libcurl" << std::endl;
		return;
	}

	// ---------------HEADER----------------
	// Get the Content Length. Required to Request "Range"Download".
	int file_size = 0;
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_cb_get_content_length);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &file_size);

	// Enable header data
	curl_easy_setopt(curl, CURLOPT_HEADER, 1);

	// --------------URL-----------------
	curl_easy_setopt(curl, CURLOPT_URL, remote_url);
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		SOFPPNIX_PRINT("Error downloading %s : %s\n", remote_url ,curl_easy_strerror(res));
		error = true;
	}
	// URL using bad/illegal format or missing URL
	// 
	curl_easy_cleanup(curl);

	if ( file_size <= 0 || error ) {
		SOFPPNIX_PRINT("The content length had no value\n");
		return false;
	}
	SOFPPNIX_PRINT("File Size of %s == %i\n",remote_url,file_size);

	int current_blob_size = 100;
	unsigned char * blob = malloc(current_blob_size);
	if ( blob == NULL ) return false;
	for ( int upper = file_size; upper>=100; upper=upper-100 ) {
		/*
			extract last 100*n bytes from remote zip.
			append them to a buffer array. ( they're order is incorrect )
		*/
		int lower = upper - 100;
		char range[64];
		snprintf(range,64,"%i-%i",lower,upper);
		curl_easy_setopt(curl, CURLOPT_RANGE, range);

		// fetch 100 bytes of data from end of file
		// performs memcpy(blob,data,100)
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, partial_blob_100_cb);
		unsigned char * recent_blob = blob + (current_blob_size - 100);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, recent_blob );


		// --------------URL-----------------
		curl_easy_setopt(curl, CURLOPT_URL, remote_url);
		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			SOFPPNIX_PRINT("Error downloading %s : %s\n", remote_url ,curl_easy_strerror(res));
			error = true;
		}
		curl_easy_cleanup(curl);

		/*
			EXITTING
		*/
		if ( error ) {
			SOFPPNIX_PRINT("HTTP Error whilst finding the Central Directory in any HTTP blob\n");
			free(blob);
			return false;
		}

		char * cd_offset;
		if ( getCentralDirectoryOffset(recent_blob,100,&cd_offset) ) {
			SOFPPNIX_PRINT("GOOD, found Central Directory!\n");

			// Acquire local CRC.
			// Need file - list.

			std::vector<FileData> zip_content;
			extractCentralDirectory(cd_offset,&zip_content);


			for (const auto& file_data : zip_content) {
				std::cout << "File Name: " << file_data.filename << std::endl;
				std::cout << "File CRC-32: " << std::hex << file_data.crc << std::endl;
			}


			// good?
			free(blob);
			return true;
		}

		/*
			CONTINUING
		*/
		// go around again.
		current_blob_size += 100;
		unsigned char * larger_blob = malloc(current_blob_size);
		// move the current data to the right allowing 100 bytes free left
		memcpy(larger_blob + (current_blob_size-100),blob, current_blob_size-100);
		free(blob);
		blob = larger_blob;
	}

	// After iterating
	SOFPPNIX_PRINT("Cannot find the Central Directory in any HTTP blob\n");
	return false;
}


/*  -------------------------------------------------------------------------
	------------------------------- ZIP -------------------------------------
	-------------------------------------------------------------------------
*/

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
		int ret = unzOpenCurrentFile(unz_file);
		if ( ret != UNZ_OK) {

			SOFPPNIX_PRINT("Error %i opening \"%s\" , corrupt zip %s?\n",ret,filename,in_zip);
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


bool getCentralDirectoryOffset(const char* buffer_haystack, int haystack_size,char * found_offset)
{
	// Set the buffer size for reading data from the buffer array
	const int BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];

	// Find the end of the buffer array by seeking to the end and reading the central directory offset
	int offset = 0;
	while (offset < haystack_size) {
		// Read a buffer of data from the end of the buffer array
		int size_to_read = std::min(haystack_size - offset, BUFFER_SIZE);
		memcpy(buffer, buffer_haystack + haystack_size - offset - size_to_read, size_to_read);
		int read_count = size_to_read;

		// Search backwards
		// Search the buffer for the central directory signature
		for (int i = read_count - 4; i >= 0; i--) {
			if (memcmp(buffer + i, "\x50\x4b\x05\x06", 4) == 0) {
				*found_offset = haystack_size - offset - size_to_read + i;
				return true;
			} // if found magic signature
		} // buffer loop

		// Move the buffer position back by the amount read to read the next buffer
		offset += read_count;
	}
	return false;
}

void extractCentralDirectory(const char* centralDirectory, std::vector<FileData>* files){
	const int BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];

	// Found the central directory signature
	int central_directory_size = *((int*)(centralDirectory + 12));
	int central_directory_offset = *((int*)(centralDirectory + 16));
	std::cout << "Central Directory Offset: " << central_directory_offset << std::endl;

	// Iterate through each file in the central directory and output its CRC value and file name
	for (int j = 0; j < central_directory_size;) {
		memcpy(centralDirectory, centralDirectory + central_directory_offset + j, std::min(BUFFER_SIZE, central_directory_size - j));
		int read_count = std::min(BUFFER_SIZE, central_directory_size - j);
		int k = 0;
		while (k < read_count) {
			if (memcmp(buffer + k, "\x50\x4b\x01\x02", 4) == 0) {
				int crc32 = *((int*)(buffer + k + 16));
				int file_name_length = *((short*)(buffer + k + 28));
				int extra_field_length = *((short*)(buffer + k + 30));
				int comment_length = *((short*)(buffer + k + 32));
				int file_offset = *((int*)(buffer + k + 42));
				std::string file_name(buffer + k + 46, file_name_length);
				std::cout << "File Name: " << file_name << std::endl;
				std::cout << "File CRC-32: " << std::hex << crc32 << std::endl;
				files->push_back({ crc32, file_name });
				k += file_name_length + extra_field_length + comment_length + 46;
				j += file_name_length + extra_field_length + comment_length + 46;
			}
			else {
				k++;
				j++;
			}
		}
	} //central directory loop
}


/*  -------------------------------------------------------------------------
	-------------------------CACHE MAP DL HISTORY ----------------------------
	-------------------------------------------------------------------------
*/

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