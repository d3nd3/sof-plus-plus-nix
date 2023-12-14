#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <cstring>

#include <minizip/unzip.h>

using namespace std;


void centralDirectory(void)
{

	// Open the zip file in binary mode
	std::ifstream zip_file("8ab.zip", std::ios::binary);

	// Set the buffer size for reading data from the file
	const int BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];

	// Find the end of the zip file by seeking to the end and reading the central directory offset
	zip_file.seekg(0, std::ios::end);
	int file_size = zip_file.tellg();
	int offset = 0;
	while (offset < file_size) {
		// Read a buffer of data from the end of the file
		int size_to_read = std::min(file_size - offset, BUFFER_SIZE);
		zip_file.seekg(-size_to_read, std::ios::cur);
		zip_file.read(buffer, size_to_read);
		int read_count = zip_file.gcount();

		// Search the buffer for the central directory signature
		for (int i = read_count - 4; i >= 0; i--) {
			if (memcmp(buffer + i, "\x50\x4b\x05\x06", 4) == 0) {
				// Found the central directory signature
				int central_directory_size = *((int*)(buffer + i + 12));
				int central_directory_offset = *((int*)(buffer + i + 16));
				std::cout << "Central Directory Offset: " << central_directory_offset << std::endl;

				// Iterate through each file in the central directory and output its CRC value and file name
				zip_file.seekg(central_directory_offset, std::ios::beg);
				for (int j = 0; j < central_directory_size; ) {
					zip_file.read(buffer, BUFFER_SIZE);
					int read_count = zip_file.gcount();
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
							k += file_name_length + extra_field_length + comment_length + 46;
							j += file_name_length + extra_field_length + comment_length + 46;
						} else {
							k++;
							j++;
						}
					}
				}

				// Exit the loop
				offset = file_size;
				break;
			}
		}

		// Move the file position back by the amount read to read the next buffer
		zip_file.seekg(-read_count, std::ios::cur);
		offset += read_count;
	}

	// Close the zip file
	zip_file.close();

}

int main() {
	centralDirectory();
	return 0;
	std::cout << std::unitbuf;
	CURL* curl = curl_easy_init();

	cout << "super early" << endl;
	if (curl) {
		// Set the URL to the zip file
		curl_easy_setopt(curl, CURLOPT_URL, "https://raw.githubusercontent.com/plowsof/sof1maps/main/dm/bc_base.zip");


		cout << "early" << endl;
		char * ofile_name = "response.zip";
		// ofstream outfile(ofile_name, std::ios::out | std::ios::binary);
		// if (!outfile.is_open()) {
		// 	std::cerr << "error" << endl;
		// 	return 1;
		// }
		FILE * fp = fopen(ofile_name, "wb");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

		cout << "middle" << endl;
		// Perform the request
		CURLcode res = curl_easy_perform(curl);
		cout << "after" << endl;
		if (res != CURLE_OK) {
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}

		curl_easy_cleanup(curl);
		// outfile.close();
		// cout << "removing " << string(ofile_name) << endl;
		// remove(ofile_name);

		fclose(fp);
	}

	return 0;
}