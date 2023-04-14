#include "common.h"

#include <curl/curl.h>

#define MAP_POOL "https://raw.githubusercontent.com/plowsof/sof1maps/main"

/*
	This is called after http is completed.
*/
void (*HTTP_CONTINUE_CB)(void) = NULL;

enum enum_dl_status download_status;

bool http_in_progress = false;
// Track which zip files have been downloaded & extracted.
std::list<std::string> httpdl_cache_list;

std::vector<FileData> zip_content;
int partial_blob_100_cb_written = 0;

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

/*
map_saving is true here
*/
bool beginHttpDL(std::string * mapname, void * callback)
{
	
	if ( HTTP_CONTINUE_CB ) {
		// SOFPPNIX_DEBUG("Http Download already in progress!!\n");
		return false;
	}
	/*
		This mapname is fixed before its sent to this function
		It has to be in the form "dm/mapname.zip"
	*/

	// SOFPPNIX_DEBUG("Requested start DL for map : %s\n",mapname->c_str());

	HTTP_CONTINUE_CB = callback;

	// if ( searchHttpCache( std::string(relative_file_path_name->c_str()) ) ) {
	// 	SOFPPNIX_DEBUG("Refusing to download a map zip that is in your cache\n");
	// 	return false;
	// }
	download_status = DS_UNCERTAIN;
	std::thread httpdl(httpdl_thread_get,new std::string(*mapname));
	httpdl.detach();
	return true;
}

/*
	resets download_status to DS_UNCERTAIN

	SO FAR A VERY POINTLESS VARIABLE - because we always continue anyway.
*/
void isHTTPdone(void)
{
	// SOFPPNIX_DEBUG("HUH? : %08X\n",HTTP_CONTINUE_CB);
	if ( HTTP_CONTINUE_CB ) {
		if ( download_status != DS_UNCERTAIN ) {
			// Called on failure or success.
			// SOFPPNIX_DEBUG("HTTP IS DONE AND RESOLVE !!\n");
			download_status = DS_UNCERTAIN;

			// some resolve
			HTTP_CONTINUE_CB();

			HTTP_CONTINUE_CB = NULL;
		}
	}
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
	

	// SOFPPNIX_DEBUG("httpdl_writecb : %08X %i %i %08X\n", ptr,size,nmemb,userdata);

	static long http_status;
	http_status = 0;
	if ( !http_status )
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
	// SOFPPNIX_DEBUG("Http status code httpdl_writecb : %li\n",http_status);
	if (http_status == 200) {
		if ( local_new_file == NULL ) {
			create_file_dir_if_not_exists(url);
			local_new_file = fopen(url, "wb");
			if ( local_new_file == NULL ) {
				SOFPPNIX_DEBUG("httpdl_error: could not create the file locally.\n");
				return -1;
			}
		}
		// open already
		// SOFPPNIX_DEBUG("Writing because response code = %li\n",http_status);
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
			SOFPPNIX_DEBUG("httpdl_writecb: COMPLETION DETECTED!\n");
		}
		#endif
		return written;
	}

	// Stop downloading
	return -1;
}


#include <zlib.h>

/*
	url as given by configstrings.

	the MAP_POOL provides a .zip bundle using the mapname/path.
*/
void httpdl_thread_get(std::string * rel_map_path) {
	// local_new_file handler is opened in FULL DOWNLOAD CURLOPT_WRITEFUNCTION stage.
	// other stages write to memory.

	char * map_path = rel_map_path->c_str();
	// SOFPPNIX_DEBUG("URL is : %s\n",map_path);

	char * userdir = orig_FS_Userdir();
	char write_zip_here[256];

	/*
		some maps are called maps/dm/
		others just dm/
	*/
	snprintf(write_zip_here,256,"%s/maps/%s",userdir,map_path);

	// SOFPPNIX_DEBUG("Local URL is : %s\n",write_zip_here);
	
	char remote_zip_weburl[256];
	snprintf(remote_zip_weburl,256,"%s/%s",MAP_POOL,map_path);
	// SOFPPNIX_DEBUG("Remote URL is : %s\n",remote_zip_weburl);

	/*
		-------------------------PARTIAL DOWNLOAD-----------------------------------
		IF UNABLE TO GET THE CRC FILE-LIST FROM ZIP, GIVE UP NOW.
		
		ONLY BOTHER TO REDOWNLOAD THE BIG ZIP FILE IF:
			ANY FILE DOES NOT EXIST LOCALLY OR
			ANY FILE's CRC DOES NOT MATCH.

		EITHER WAY: PROPEL FORWARD!
	*/

	if ( !partialHttpBlobs(&remote_zip_weburl[0]) ) {
		// NOT ABLE TO GET THE FILE CONTENT LENGTH AND LISTS.

		delete rel_map_path;
		download_status = DS_FAILURE;
		return;
	}

	// Use zip_content ( it was cleared and set inside ExtractCentralDirectory function )

	bool do_download = false;
	for (const auto& file_data : zip_content) {
		std::cout << "File Name: " << file_data.filename << std::endl;
		std::cout << "File CRC-32: " << std::hex << file_data.crc << std::endl;

		std::string full_file_pathname = std::string(orig_FS_Userdir()) + std::string("/") +  file_data.filename;
		// Check if file exists on disk
		std::ifstream file(full_file_pathname);
		if (!file.good()) {
			std::cerr << "File does not exist: " << full_file_pathname << std::endl;
			// We know we want to download now.
			do_download = true;
			break;
		}

		// Compute CRC of file on disk
		std::ifstream disk_file(full_file_pathname, std::ios::binary);
		unsigned int disk_crc = 0;
		char buffer[1024];
		while (disk_file.read(buffer, sizeof(buffer))) {
			disk_crc = crc32(disk_crc, buffer, sizeof(buffer));
		}
		disk_crc = crc32(disk_crc, buffer, disk_file.gcount());

		// Check if CRC values match
		if (disk_crc != file_data.crc) {
			std::cerr << "File has different CRC value: " << full_file_pathname << std::endl;
			// We know we want to download now.
			do_download = true;
			break;
		}
	}

	if ( !do_download ) 
	{
		SOFPPNIX_DEBUG("Not downloading because file not found or crc matches on disk\n");
		delete rel_map_path;
		download_status = DS_FAILURE;
		return;
	}
	// SOFPPNIX_DEBUG("PREPARING DOWNLOAD!\n");

	/*
		---------------------------FULL DOWNLOAD--------------------------------------
		WE HAVE COMPARED CRC TO EACH LOCAL DIRECTORY
		THEY ARE DIFFERENT OR NON-EXISTING, SO GO AHEAD
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
		SOFPPNIX_DEBUG("Error downloading %s : %s\n", map_path ,curl_easy_strerror(res));
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
		SOFPPNIX_DEBUG("Preparing to unzip %s\n",write_zip_here);
		// all happy?
		if ( !unZipFile( write_zip_here, userdir) )
		{
			SOFPPNIX_DEBUG("Unzip failure!\n");
			download_status = DS_FAILURE;
		} else
		{
			// keep track of the extracted zip files in a permanant file
			// appendItemHttpCache(map_path);
			// saveHttpCache();
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
	int ret = -1;

	static long http_status;
	http_status = 0;
	if ( !http_status )
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);

	if ( http_status != 200 ) return ret;

	size_t numbytes = size * nitems;
	char *header = (char *)malloc(numbytes + 1);
	memcpy(header, buffer, numbytes);
	header[numbytes] = '\0';

	// hexdump(header,header+numbytes);
	
	const char *prefix = "Content-Length: ";
	char *content_length = strcasestr(header, prefix);
	int length = 0;
	if (content_length)
	{
		// hexdump(header,header+numbytes);
		// SOFPPNIX_DEBUG("FOUND CONTENT LENGTH\n");
		content_length += strlen(prefix);
		length = atoi(content_length);
		*((int *)userdata) = length;
	}
	ret = numbytes;

	free(header);
	// SOFPPNIX_DEBUG("LENGTH WAS %i\n",length);
	return ret;
}


// pass buffer to it

size_t partial_blob_100_cb(void *ptr, size_t size, size_t nmemb, void *userdata) {

	unsigned char ** out_buffer = userdata;

	static long http_status;
	http_status = 0;
	if ( !http_status )
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);

	// SOFPPNIX_DEBUG("Status code for partial_blob_100 %i\n",http_status);
	// partial content success == 206
	if ( http_status != 206 && http_status != 200) {
		SOFPPNIX_DEBUG("RETURNING -1 Because Bad status code\n");
		return -1;
	} 


	size_t total_size = size * nmemb;
	// SOFPPNIX_DEBUG("!!!!Total Size of Partial Download!!!!! = %i , partialptr: %08X\n",total_size,ptr);

	// its growing. i assume new space is at end
	// we are supposed to free.

	char * new_mem = malloc(partial_blob_100_cb_written + total_size);
	
	// free previous mem
	if ( *out_buffer != NULL ) {
		// insert previous mem right-hand size into new mem.
		memcpy(new_mem + total_size, *out_buffer,partial_blob_100_cb_written);
			
		free( *out_buffer );
	}

	*out_buffer = new_mem;

	// save data
	memcpy(*out_buffer, ptr , total_size);
	
	// update size
	partial_blob_100_cb_written += total_size;

	return total_size;
}
/*
	Download end of file in chunks until signature is found indicating Central Directory Offset
	100 byte chunks?
*/
// struct partial_http_s {
// 	unsigned char * buffer_out;
// 	int chunk_size;
// }

int partial_Header_ContentLength(char * remote_url)
{
	int file_size = 0;
	/*
		Get the Content Length One time only.
	*/
	curl = curl_easy_init();
	if (!curl) {
		std::cerr << "Error initializing libcurl" << std::endl;
		return 0;
	}

	// ---------------HEADER----------------
	// Get the Content Length. Required to Request "Range"Download".
	
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_cb_get_content_length);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &file_size);

	// Enable header data
	// curl_easy_setopt(curl, CURLOPT_HEADER, 1);

	// Disable body data
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

	// --------------URL-----------------
	curl_easy_setopt(curl, CURLOPT_URL, remote_url);
	CURLcode res = curl_easy_perform(curl);

	curl_easy_cleanup(curl);

	return file_size;
}


/*
	1) GET CONTENT LENGTH
	2) DOWNLOAD CHUNKS FROM END
	3) STORE CRC
	4) COMPARE CRC LOCALLY
*/


bool partialHttpBlobs(char * remote_url)
{
	bool error = false;

	/*
		GET CONTENT LENGTH
	*/
	int file_size = partial_Header_ContentLength(remote_url);
	if ( file_size <= 0 ) {
		SOFPPNIX_DEBUG("The content length had no value\n");
		return false;
	}
	SOFPPNIX_DEBUG("File Size of %s == %i\n",remote_url,file_size);

	/*
		Let http data cb call malloc
	*/
	int CHUNK_SIZE = 100;
	int current_blob_size = CHUNK_SIZE;
	// this pointer is sent to the callback func, they will malloc it and control its size.
	// size will be set in global var. partial_blob_100_cb_written
	unsigned char * blob = NULL;
	int upper = file_size;
	bool FORCED_RANGE = false;
	int CD_SIZE = 0;
	do {
		// reset written count for memcpy tracking.
		partial_blob_100_cb_written = 0;

		curl = curl_easy_init();
		if (!curl) {
			std::cerr << "Error initializing libcurl" << std::endl;
			return;
		}
		int lower;
		if ( !FORCED_RANGE )
			lower = upper - CHUNK_SIZE;
		
		char range[64];
		snprintf(range,64,"%i-%i",lower,upper);
		if ( !FORCED_RANGE )
			upper = lower - 1;

		curl_easy_setopt(curl, CURLOPT_RANGE, range);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, partial_blob_100_cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &blob );

		// --------------URL-----------------
		curl_easy_setopt(curl, CURLOPT_URL, remote_url);
		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			SOFPPNIX_DEBUG("Error downloading %s : %s\n", remote_url ,curl_easy_strerror(res));
			error = true;
		}
		curl_easy_cleanup(curl);

		/*
			EXITTING
		*/
		if ( error ) {
			SOFPPNIX_DEBUG("HTTP Error whilst finding the Central Directory in any HTTP blob\n");
			free(blob);
			return false;
		}

		if ( FORCED_RANGE ) {
			extractCentralDirectory(blob,CD_SIZE,&zip_content);

			free(blob);
			return true;
		}


		int CENTRAL_DIRECTORY_OFFSET;
		// ensure full size search even though searched that area in case of overlap,cut off from 100 chunking
		

		int ABSOLUTE_DIR_OFFSET;
		if ( CD_SIZE = getCentralDirectoryOffset(blob,partial_blob_100_cb_written,CENTRAL_DIRECTORY_OFFSET,ABSOLUTE_DIR_OFFSET) ) {
			if ( CENTRAL_DIRECTORY_OFFSET > 0 ) {
				// SOFPPNIX_DEBUG("POSITIVE OFFSET, found Central Directory! %08X\n",CENTRAL_DIRECTORY_OFFSET);

				extractCentralDirectory(blob + CENTRAL_DIRECTORY_OFFSET,CD_SIZE,&zip_content);

				free(blob);
				return true;
			} else {
				// SOFPPNIX_DEBUG("NEGATIVE OFFSET %i %i\n",ABSOLUTE_DIR_OFFSET,ABSOLUTE_DIR_OFFSET + CD_SIZE);
				// ITS NOT HERE, GO REQUEST IT WITH EXACT RANGE!
				FORCED_RANGE = true;
				lower = ABSOLUTE_DIR_OFFSET;
				upper = ABSOLUTE_DIR_OFFSET + CD_SIZE;

				continue;
			}
		}

		// lower - upper , within FILE_LEN bounds. [0 - FILELEN]
		// FITS? UPPER SHOULDN't DECREASE, it just follows LOWER
		if ( upper<CHUNK_SIZE )
			break;
	} while ( true );

	// After iterating
	// SOFPPNIX_DEBUG("Cannot find the Central Directory in any HTTP blob\n");
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
		SOFPPNIX_DEBUG("Error corrupt zip %s?\n",in_zip);
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
			SOFPPNIX_DEBUG("Error corrupt zip %s?\n",in_zip);
			closeAndDeleteZipFile(unz_file,in_zip);
			return false;
		}
		
		// SOFPPNIX_DEBUG("Processing file : %s : size : %lu\n",filename,file_info.uncompressed_size);
		
		//----------------------OPEN READ FILE----------------------
		int ret = unzOpenCurrentFile(unz_file);
		if ( ret != UNZ_OK) {

			SOFPPNIX_DEBUG("Error %i opening \"%s\" , corrupt zip %s?\n",ret,filename,in_zip);
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
			SOFPPNIX_DEBUG("Cannot write to : %s : when extracting : %s\n",output_file_path.c_str(),in_zip);
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
				SOFPPNIX_DEBUG("This zip is corrupt? cannot read correctly : %s\n",in_zip);
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
			
			// SOFPPNIX_DEBUG("WARNING: Partial extraction of : %s\n",in_zip);
			// SOFPPNIX_DEBUG("This zip might have polluted your directories\n");

			// success? :P
			break;
			// closeAndDeleteZipFile(zip_file);
			// return false;
		}
	} while (true);
	// break if nextFile != UNZ_OK

	closeAndDeleteZipFile(unz_file,in_zip);

	SOFPPNIX_DEBUG("Successfully extracted and deleted : %s\n",in_zip);
	return true;
}


int getCentralDirectoryOffset(const char* HTTP_BLOB, int BLOB_SIZE,int &found_offset, int &abs_dir_offset)
{
	// SOFPPNIX_DEBUG("SEARCHING FOR CENTAL DIRECTORY OFFSET\n");
	for ( int i = BLOB_SIZE-4; i >=0; i-- ) {
		// End of Central Directory SIGNATURE
		// SUrely can just subtract size to get the start?
		if (memcmp(HTTP_BLOB + i, "\x50\x4b\x05\x06", 4) == 0) {
			
			/*
				Just because we have found the end signature, doesnt mean
				we have collected enough data.
				MATH!

			*/

			abs_dir_offset = *((int*)(HTTP_BLOB + i + 16));
			// SOFPPNIX_DEBUG("Central Directory Offset: %i\n", abs_dir_offset);

			int central_directory_size = *((int*)(HTTP_BLOB + i + 12));
			// SOFPPNIX_DEBUG("Central Directory Size = %i\n",central_directory_size);

			found_offset = i - central_directory_size;
			// i can be negative.
			return central_directory_size;
		}
	}
	return 0;
}
// Define a struct for the central directory file header
#pragma pack(push, 1) // Ensure no padding is added
struct CDFileHeader {
  uint32_t signature; // 0x02014b50
  uint16_t versionMadeBy;
  uint16_t versionNeeded;
  uint16_t flags;
  uint16_t compressionMethod;
  uint16_t lastModTime;
  uint16_t lastModDate;
  uint32_t crc32;
  uint32_t compressedSize;
  uint32_t uncompressedSize;
  uint16_t fileNameLength;
  uint16_t extraFieldLength;
  uint16_t commentLength;
  uint16_t diskNumberStart;
  uint16_t internalAttributes;
  uint32_t externalAttributes;
  uint32_t relativeOffset; // Offset of local file header
};
 #pragma pack(pop) // Restore default alignment


void extractCentralDirectory(const char* centralDirectory,int cd_size, std::vector<FileData>* files)
{
	files->clear();
	#if 0
	// Iterate through each file in the central directory and output its CRC value and file name
	int k = 0;
	while ( k < cd_size ) {
		SOFPPNIX_DEBUG("ADDRESS == %08X\n",centralDirectory + k);
		if (memcmp(centralDirectory + k, "\x50\x4b\x01\x02", 4) == 0) {
			SOFPPNIX_DEBUG("Found a magic signature for FILE !\n");
			int crc32 = *((int*)(centralDirectory + k + 16));
			int file_name_length = *((short*)(centralDirectory + k + 28));
			int extra_field_length = *((short*)(centralDirectory + k + 30));
			int comment_length = *((short*)(centralDirectory + k + 32));
			int file_offset = *((int*)(centralDirectory + k + 42));
			std::string file_name(centralDirectory + k + 46, file_name_length);
			std::cout << "File Name: " << file_name << std::endl;
			std::cout << "File CRC-32: " << std::hex << crc32 << std::endl;
			files->push_back({ crc32, file_name });
			SOFPPNIX_DEBUG("FIlenameLength = %i :: extra_Field_len = %i :: comment_len = %i\n",file_name_length,extra_field_length,comment_length);
			k += file_name_length + extra_field_length + comment_length + 46;
			// ye its because i have never encountered a for loop that also increases its index HAHA.
		} else
		k++;
	} //central directory loop
	#endif

	int k = 0;
	while ( k < cd_size ) {
		// Cast the pointer to the struct type
		CDFileHeader* header = (CDFileHeader*)(centralDirectory + k);
		// Check the signature
		if (header->signature == 0x02014b50) {
			// Get the file name from the header
			std::string file_name((char*)(header + 1), (size_t)header->fileNameLength);
			files->push_back({ header->crc32, file_name });
			// Advance the pointer by the size of the header and the variable fields
			k += sizeof(CDFileHeader) + header->fileNameLength + header->extraFieldLength + header->commentLength;
		} else {
			k++;
		}
	}
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
	SOFPPNIX_DEBUG("Appending %s\n",zipfile.c_str());
	// Convert the new line to lower case for case-insensitive comparison 
	std::string lower_zipfile = zipfile;
	std::transform(lower_zipfile.begin(), lower_zipfile.end(), lower_zipfile.begin(), ::tolower);
	// Find the position where the new line should be inserted in a sorted order using lower_bound algorithm 
	auto pos = std::lower_bound(httpdl_cache_list.begin(), httpdl_cache_list.end(), lower_zipfile);
	// Insert the new line at that position using insert method of list container 
	httpdl_cache_list.insert(pos, zipfile);

	for (const auto& azip : httpdl_cache_list) {
		SOFPPNIX_DEBUG("%s\n", azip.c_str());
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
	SOFPPNIX_DEBUG("Saving "HTTPDL_CACHE_NAME"\n");
	std::ofstream diskcache(HTTPDL_CACHE_NAME, std::ios::out);

	if (diskcache.is_open()) {
		for (const auto& azip : httpdl_cache_list) {
			diskcache << azip << "\n";
		}
		diskcache.close();
	}
	else {
		// Print an error message if the file cannot be opened
		SOFPPNIX_DEBUG("Unable to saveHttpCache "HTTPDL_CACHE_NAME"\n");
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
		SOFPPNIX_DEBUG("No "HTTPDL_CACHE_NAME" found, maps will be re-downloaded\n");
	}
}