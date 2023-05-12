#include "common.h"



void SOFPPNIX_DEBUG(char * msg, ... ) {
	#ifdef DEBUG
	char text[1024];
	va_list args;

	va_start(args, msg);
	vsnprintf(text, sizeof(text), msg, args);
	va_end(args);

	char color[1024];
	
	snprintf(color,1024,"%c%s%c%s\n",P_PURPLE,"[sof++nix_Debug] : ",P_YELLOW,text);

	orig_Com_Printf("%s",color);
	#endif
}

void SOFPPNIX_PRINT(char * msg, ... ) {
	char text[1024];
	va_list args;

	va_start(args, msg);
	vsnprintf(text, sizeof(text), msg, args);
	va_end(args);

	char color[1024];
	
	snprintf(color,1024,"%c%s%c%s\n",P_PINK,"[sof++nix] : ",P_TEXT,text);

	orig_Com_Printf("%s",color);
}
void shutdown_program(void);
void error_exit(char* format,...) {
	va_list args;
	va_start(args, format);
	char message[512];
	int len = vsnprintf(message, sizeof(message), format, args);
	va_end(args);

	if (len < 0 || len >= sizeof(message)) {
		// handle error
		std::cerr << "Error: message input invalid" << std::endl;
	}
	std::cerr << "Error: " << message << std::endl;

	shutdown_program();
	orig_Sys_Error("");
	std::exit(EXIT_FAILURE);
}

void shutdown_program(void)
{
	std::cout << "Shutting down..." << std::endl;


	// get the process ID
	pid_t pid = getpid();

	// get a list of all the threads in the process
	std::vector<pid_t> tids;
	FILE* fp = fopen("/proc/self/task", "r");
	if (fp != nullptr)
	{
		char buf[256];
		while (fgets(buf, sizeof(buf), fp))
		{
			pid_t tid = atoi(buf);
			if (tid != syscall(SYS_gettid))
			{
				tids.push_back(tid);
			}
		}
		fclose(fp);
	}

	// send a signal to each thread to terminate
	for (auto tid : tids)
	{
		pthread_kill(tid, SIGINT);
	}

	// wait for each thread to terminate
	for (auto tid : tids)
	{
		pthread_join(tid, NULL);
	}

	// perform any necessary cleanup
	// ...

	// std::exit(EXIT_SUCCESS);
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
		error_exit("m-unprotect");
	}
}

void memoryProtect(void * addr)
{
	size_t page_size = getpagesize();
	void * aligned_addr = (void *)((unsigned long)addr & ~(page_size - 1));
	if (mprotect(aligned_addr, page_size, PROT_READ ) == -1) {
		error_exit("m-protect");
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
	// printf("processing detour : %08X\n",orig);
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

	memoryProtect(orig);
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

// std::stoi
int c_string_to_int(char * in_str) {
	std::string str(in_str);
	size_t pos;
	int num;

	try {
		num = std::stoi(str, &pos);
	}
	catch(const std::invalid_argument& e) {
		error_exit("c_string_to_int Invalid argument");
	}
	catch(const std::out_of_range& e) {
		error_exit("c_string_to_int Out of range");
	}
	if (pos != str.length()) {
		error_exit("c_string_to_int Trailing characters");
	}
	return num;
}

void * getClientX(int slot)
{
	void *one = stget(0x0829D134,0);
	void * svs_clients = stget(one,0x10C);
	return svs_clients + slot * 0xd2ac;
}

int countPlayersInGame(void)
{
	int player_count = 0;
	for ( int i = 0 ; i < maxclients->value;i++ ) {
		void * client_t = getClientX(i);
		int state = *(int*)(client_t);
		if (state != cs_spawned )
			continue;
		player_count +=1;
	}

	return player_count;
}

int getPlayerSlot(void * in_client)
{
	void * svs_clients = *(unsigned int*)(*(unsigned int*)(0x0829D134) + 0x10C);

	int addr_diff = (int)in_client - (int)svs_clients;
	if ( addr_diff < 0 || addr_diff % 0xd2ac != 0)
		return -1;
	int slot = addr_diff / 0xd2ac;
	if ( slot < 0 || slot >= maxclients->value)
		return -1;
	return slot;
}

bool isServerEmpty(void)
{
	void * svs_clients = *(unsigned int*)(*(unsigned int*)(0x0829D134) + 0x10C);
	for ( int i = 0 ; i < maxclients->value;i++ ) {
		void * client_t = svs_clients + i * 0xd2ac;
		int state = *(int*)(client_t);
		if (state != cs_spawned )
			continue;
		return false;
	
	}
	return true;
}

void dump_usercmd(usercmd_t& cmd)
{
	std::cout << "usercmd_t {" << std::endl;
	std::cout << "  msec: " << static_cast<int>(cmd.msec) << std::endl;
	std::cout << "  buttons: " << static_cast<int>(cmd.buttons) << std::endl;
	std::cout << "  lightlevel: " << static_cast<int>(cmd.lightlevel) << std::endl;
	std::cout << "  lean: " << static_cast<int>(cmd.lean) << std::endl;
	std::cout << "  angles: {" << static_cast<int>(cmd.angles[0]) << ", "
			  << static_cast<int>(cmd.angles[1]) << ", "
			  << static_cast<int>(cmd.angles[2]) << "}" << std::endl;
	std::cout << "  forwardmove: " << static_cast<int>(cmd.forwardmove) << std::endl;
	std::cout << "  sidemove: " << static_cast<int>(cmd.sidemove) << std::endl;
	std::cout << "  upmove: " << static_cast<int>(cmd.upmove) << std::endl;
	std::cout << "  fireEvent: "  << cmd.fireEvent << std::endl;
	std::cout << "  altfireEvent: " << cmd.altfireEvent << std::endl;
	std::cout << "}" << std::endl;
}

vec_t VectorNormalize (vec3_t v)
{
	float	length, ilength;

	length = DotProduct(v, v);
	length = sqrt (length);		// FIXME

	if (length)
	{
		ilength = 1/length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}
		
	return length;

}

float NormalizeAngle (float angle)
{
// FIXME:  make this use fmod
	while (angle >= 360)
	{
		angle -= 360;
	}
	while (angle < 0)
	{
		angle += 360;
	}
	return angle;
}


vec_t VectorLength(vec3_t v)
{
	float	length;
	
	length = DotProduct(v, v);
	length = sqrt (length);		// FIXME

	return length;
}

static vec_t VectorSeparation(vec3_t v0, vec3_t v1)
{
	vec3_t	v;
	float	length;

	VectorSubtract(v0, v1, v);
	length = DotProduct(v, v);
	length = sqrt (length);		// FIXME

	return length;
}

// A helper template to unpack an array into a variadic function call
template <typename F, typename T, size_t N, size_t... Is>
void array_to_variadic_impl(F f,edict_t * ent, short id, const std::array<T, N>& arr, std::index_sequence<Is...>) {
  f(ent,id,arr[Is]...); // expand the array elements as arguments
}

// A wrapper template to deduce the array size and generate an index sequence
template <typename F, typename T, size_t N>
void array_to_variadic(F f,edict_t * ent, short id, const std::array<T, N>& arr) {
  array_to_variadic_impl(f,ent,id, arr, std::make_index_sequence<N>{}); // make an index sequence from 0 to N-1
}

// A variadic function to print some integers
void print_ints(void * x, ...) {
  std::cout << x;
  va_list args;
  va_start(args, x);
  int y;
  while ((y = va_arg(args, int)) != 0) { // assume 0 is the sentinel value
	std::cout << " " << y;
  }
  va_end(args);
  std::cout << "\n";
}
int arraySize = 0;
std::array<void*,10> formatString(const std::string& format, const std::vector<std::string>& inputs) {
	arraySize = 0;
	std::array<void*,10> result;
	std::istringstream ss(format);
	std::string token;
	int i = 0;
	// std::cout << "before getline: " << format << std::endl;
	// print all inputs
	// for (auto& input : inputs) {
	// 	std::cout << "input: " << input << std::endl;
	// }

	bool skip_first = false;
	while (std::getline(ss, token, '%')) {
		// skip first token
		if ( !skip_first ) {
			skip_first = true;
			continue;
		}
		if (token.empty()) continue;
		char type = token[0];
		switch (type) {
			case 'd': {
				int value = int(std::stoi(inputs[i]));
				result[arraySize++] = (void*)value;
				break;
			}
			case 'p': {
				std::cout << "p " << i << std::endl;
				result[arraySize++] = (void*)char(std::stoi(inputs[i]));
				break;
			}
			case 's': {
				std::cout << "string! : " << inputs[i].c_str() << std::endl;
				result[arraySize++] = inputs[i].c_str();
				break;
			}
			case 'h': {
				if (token[1] == 'd') {
					short value = short(std::stoi(inputs[i]));
					result[arraySize++] = (void*)value;
				} else if (token[1] == 'u') {
					unsigned short value = std::stoul(inputs[i]);
					result[arraySize++] = (void*)value;
				}
				break;
			}
		}
		i++;
	}
	//print array result using sofppdebug
	SOFPPNIX_DEBUG("ARRAY SIZE: %d",arraySize);
	for(int i = 0; i < arraySize; i++) {
		SOFPPNIX_DEBUG("ARRAY[%d]: %d",i,result[i]);
	}

	return result;
}

void SP_PRINT_MULTI(edict_t * ent, short id, std::string& format, std::vector<std::string>& inputs) {

	std::array<void*,10> args = formatString(format, inputs);
	array_to_variadic(orig_SP_Print,ent,id, args);
}

void crc_checksum(const char * data,std::string & checksum, int length)
{
	unsigned int crc = crc32(0L, NULL, 0);
	crc = crc32(crc, reinterpret_cast<const Bytef*>(data), length);
	std::stringstream ss;
	ss << std::hex << std::setfill('0') << std::setw(8) << crc;
	checksum = ss.str();
}


int changesize(FILE *fp, off_t size)
{
	int filedes = fileno(fp);
	return ftruncate(filedes, size);
}

void* fast_realloc(void * buffer,int size)
{
	void * tmp = realloc(buffer,size);
	if ( tmp == NULL ) {
		error_exit("realloc failed");
	}
	return tmp;
}

// Convert a string to lowercase
std::string toLowercase(const std::string& str) {
	std::string result(str);
	for (auto& c : result) {
		c = std::tolower(c);
	}
	return result;
}

// Search for a case-insensitive substring within a string
bool strcasestr(const std::string& str, const std::string& substr) {
	std::string str_lower = toLowercase(str);
	std::string substr_lower = toLowercase(substr);
	return (strstr(str_lower.c_str(), substr_lower.c_str()) != nullptr);
}

/*
	ensure minimum input size of 16.
*/
void my_itoa(int i, char * out_str) {
	snprintf(out_str, 16, "%d", i);
}

/*

--------------------LINKED LISTS-------------------------
GOOD FOR OFTEN ADDING DELETING ELEMENTS
THUS MEMORY MANAGEMENT.
	list_t *list = list_create();
	int *a = malloc(sizeof(int));
	int *b = malloc(sizeof(int));
	int *c = malloc(sizeof(int));
	*a = 1;
	*b = 2;
	*c = 3;
	list_append(list, a);
*/

/* Initialize a new linked list */
list_t *list_create() {
	list_t *list = malloc(sizeof(list_t));
	if (!list) {
		fprintf(stderr, "Error: Unable to allocate memory for list_t\n");
		exit(EXIT_FAILURE);
	}
	memset(list, 0, sizeof(list_t));
	return list;
}

/* Destroy a linked list and free all its memory */
void list_destroy(list_t *list) {
	if (!list) {
		return;
	}
	list_node_t *node = list->head;
	while (node) {
		list_node_t *next = node->next;
		free(node);
		node = next;
	}
	free(list);
}

/* Create a new node with the given data */
list_node_t *list_node_create(void *data) {
	list_node_t *node = malloc(sizeof(list_node_t));
	if (!node) {
		fprintf(stderr, "Error: Unable to allocate memory for list_node_t\n");
		exit(EXIT_FAILURE);
	}
	memset(node, 0, sizeof(list_node_t));
	node->data = data;
	return node;
}

/* Add a node to the end of the linked list */
void list_append(list_t *list, void *data) {
	list_node_t *node = list_node_create(data);
	if (!list->head) {
		/* Empty list */
		list->head = node;
		list->tail = node;
	} else {
		/* Non-empty list */
		list->tail->next = node;
		node->prev = list->tail;
		list->tail = node;
	}
	list->count++;
}

/* Remove a node from the linked list */
void list_remove(list_t *list, list_node_t *node) {
	if (!node) {
		return;
	}
	if (node == list->head) {
		/* Remove the head node */
		list->head = node->next;
	} else {
		node->prev->next = node->next;
	}
	if (node == list->tail) {
		/* Remove the tail node */
		list->tail = node->prev;
	} else {
		node->next->prev = node->prev;
	}
	free(node);
	list->count--;
}

/* Print the linked list */
void list_print(list_t *list) {
	printf("List (count=%ld):\n", list->count);
	list_node_t *node = list->head;
	while (node) {
		printf("%p -> ", node->data);
		node = node->next;
	}
	printf("NULL\n");
}


void LoadTextFile(const char *fileName, char **text)
{
	FILE *ifp; //a file pointer for file read/write
	long length; //the length of the file

	//opening the file
	ifp = fopen(fileName, "r");
	if (ifp == NULL)
	{
		error_exit("Can't open input file \"%s\"!\n", fileName);
	}
	else
	{
		//getting the length of the file
		fseek(ifp, 0, SEEK_END);
		length = ftell(ifp);
		fseek(ifp, 0, SEEK_SET);

		//allocating memory for the string
		*text = malloc(length + 1);

		//reading the file into the string
		fread(*text, 1, length, ifp);

		//closing the file
		fclose(ifp);

		//adding a null terminator
		(*text)[length] = '\0';
	}
}