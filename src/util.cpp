#include "common.h"


void SOFPPNIX_PRINT(char * msg, ... ) {
	char text[1023];
	va_list args;

	va_start(args, msg);
	vsnprintf(text, sizeof(text), msg, args);
	va_end(args);

	char color[1024];
	
	snprintf(color,1024,"%c%s%s",0x02,"sof++nix_Debug: ",text);

	orig_Com_Printf(color);
}

void error(const char* message) {
	perror(message);
	std::cerr << "Error: " << message << std::endl;
	std::exit(EXIT_FAILURE);
}

void hexdump(void *addr_start, void *addr_end) {
	unsigned char *p = (unsigned char *)addr_start;
	unsigned char *q = (unsigned char *)addr_end;
	int offset = 0;
	while (p < q) {
		printf("%08x: ", offset);
		for (int i = 0; i < 16 && p + i < q; i++) {
			printf("%02x ", *(p + i));
		}
		for (int i = 16 - (q - p); i > 0; i--) {
			printf("   ");
		}
		for (int i = 0; i < 16 && p + i < q; i++) {
			printf("%c", *(p + i) >= 0x20 && *(p + i) < 0x7f ? *(p + i) : '.');
		}
		printf("\n");
		p += 16;
		offset += 16;
	}
}

void memoryAdjust(void * addr, int size, unsigned char val)
{
	memoryUnprotect(addr);
	memset(addr,val,size);
	memoryProtect(addr);
}

void memoryUnprotect(void * addr)
{
	size_t page_size = getpagesize();

	void * aligned_addr = (void *)((unsigned long)addr & ~(page_size - 1));
	if (mprotect(aligned_addr, page_size, PROT_READ | PROT_WRITE) == -1) {
		error("mprotect");
	}
}

void memoryProtect(void * addr)
{
	size_t page_size = getpagesize();
	void * aligned_addr = (void *)((unsigned long)addr & ~(page_size - 1));
	if (mprotect(aligned_addr, page_size, PROT_READ ) == -1) {
		error("mprotect");
	}
}


void callE8Patch(void * addr,void * to) {

	memoryUnprotect(addr);

	unsigned char * p = addr;
	p[0] = 0xE8;
	*(int*)(p+1) = to - (int)addr - 5;

	memoryProtect(addr);
}

void * createDetour(void * orig, void * mine, int size) {
	memoryUnprotect(orig);

	/*
		THUNK SAVED. { SAVED OPCODES;JMP->ORIG+SIZE(SAVED_OPCODES) }
	*/
	void * thunk = malloc(size + 5);
	// save the front of func
	memcpy(thunk, orig, size);
	// jump back
	unsigned char * p = thunk + size;
	*p = 0xE9;
	int rel_jmp = ((int)orig+size) - (int)p - 5;
	memcpy(p+1,&rel_jmp,4);

	/*
		PATCH 5 bytes start of orig
	*/
	unsigned char small_jmp;
	// write the front of func
	small_jmp = 0xE9;
	rel_jmp = mine - (int)orig - 5;
	memcpy(orig,&small_jmp,1);
	memcpy(orig+1,&rel_jmp,4);

	memoryUnprotect(orig);
	// dealloc this on uninit.
	return thunk;
}

// qcommon from game source exports it.
bool        NET_CompareAdr (netadr_t a, netadr_t b)
{
	if (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3] && a.port == b.port)
		return true;
	return false;
}

void NetadrToSockadr (netadr_t *a, struct sockaddr_in *s)
{
	memset (s, 0, sizeof(*s));

	if (a->type == NA_BROADCAST)
	{
		s->sin_family = AF_INET;

		// s->sin_port = htons(a->port);
		s->sin_port = a->port;
		*(int *)&s->sin_addr = -1;
	}
	else if (a->type == NA_IP)
	{
		s->sin_family = AF_INET;

		*(int *)&s->sin_addr = *(int *)&a->ip;
		// s->sin_port = htons(a->port);
		s->sin_port = a->port;
	}
}

void SockadrToNetadr (struct sockaddr_in *s, netadr_t *a)
{
	*(int *)&a->ip = *(int *)&s->sin_addr;
	a->port = ntohs(s->sin_port);
	a->type = NA_IP;
}

// Recursively create directories
void create_dirs_recursively(const char* path) {
	char* path_copy = strdup(path);
	char* current_dir = strtok(path_copy, "/");
	char current_path[1024];
	
	if ( path[0] == '/' ) {
		current_path[0] = '/';
		current_path[1] = '\0';
	} else
	{
		current_path[0] = '\0';
	}
	while (current_dir != nullptr) {
		strcat(current_path, current_dir);
		strcat(current_path, "/");
		create_dir_if_not_exists(current_path);
		current_dir = strtok(nullptr, "/");
	}
	free(path_copy);
}

// Create a directory if it does not exist
void create_dir_if_not_exists(const char* dir_path) {
	struct stat st = {0};
	if (stat(dir_path, &st) == -1) {
		// Directory does not exist, create it
		mkdir(dir_path, 0755);
	}
}

// Create a directory for a file if it does not exist
void create_file_dir_if_not_exists(const char* file_path) {
	char* dir_path = strdup(file_path);
	char* last_slash = strrchr(dir_path, '/');
	if (last_slash != nullptr) {
		*last_slash = '\0';
		create_dirs_recursively(dir_path);
	}
	free(dir_path);
}