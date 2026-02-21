#include "common.h"

#ifdef DEMOSYSTEM
static void cmd_nix_match_history(void);
static void cmd_nix_demo_pause(void);
static void cmd_nix_demo_resume(void);
static void cmd_nix_demo_goto(void);
static void cmd_nix_demo_rate(void);
static void cmd_nix_demo_loop(void);
static void cmd_nix_demo_seekms(void);
static void cmd_nix_demo_nextcp(void);
static void cmd_nix_demo_prevcp(void);
static void cmd_nix_demo_seekpct(void);
static void cmd_nix_demo_bmset(void);
static void cmd_nix_demo_bmgoto(void);
static void cmd_nix_demo_bmlist(void);
static void cmd_nix_demo_bmdel(void);
static void cmd_nix_demo_bmren(void);
static void cmd_nix_demo_pov(void);
static void cmd_nix_demo_freecam(void);
static void cmd_nix_demo_pov_next(void);
static void cmd_nix_demo_pov_prev(void);
#endif

void CreateCommands(void)
{
	my_Cmd_AddCommand("++nix_test",&cmd_nix_test);
	my_Cmd_AddCommand("++nix_client_state",&cmd_nix_client_state);
	my_Cmd_AddCommand("++nix_client_map",&cmd_nix_client_map);

    #ifdef DEMOSYSTEM
    my_Cmd_AddCommand("++nix_demomap",&cmd_nix_demomap);
    my_Cmd_AddCommand("++nix_record",&cmd_nix_record);
    my_Cmd_AddCommand("++nix_stoprecord",&cmd_nix_stoprecord);
    my_Cmd_AddCommand("++nix_demo_pov",(void*)cmd_nix_demo_pov);
    my_Cmd_AddCommand("++nix_match_history",(void*)cmd_nix_match_history);
    my_Cmd_AddCommand("++nix_demo_pause",(void*)cmd_nix_demo_pause);
    my_Cmd_AddCommand("++nix_demo_resume",(void*)cmd_nix_demo_resume);
    my_Cmd_AddCommand("++nix_demo_goto",(void*)cmd_nix_demo_goto);
    my_Cmd_AddCommand("++nix_demo_rate",(void*)cmd_nix_demo_rate);
    my_Cmd_AddCommand("++nix_demo_loop",(void*)cmd_nix_demo_loop);
    my_Cmd_AddCommand("++nix_demo_seekms",(void*)cmd_nix_demo_seekms);
    my_Cmd_AddCommand("++nix_demo_nextcp",(void*)cmd_nix_demo_nextcp);
    my_Cmd_AddCommand("++nix_demo_prevcp",(void*)cmd_nix_demo_prevcp);
    my_Cmd_AddCommand("++nix_demo_seekpct",(void*)cmd_nix_demo_seekpct);
    my_Cmd_AddCommand("++nix_demo_bmset",(void*)cmd_nix_demo_bmset);
    my_Cmd_AddCommand("++nix_demo_bmgoto",(void*)cmd_nix_demo_bmgoto);
    my_Cmd_AddCommand("++nix_demo_bmlist",(void*)cmd_nix_demo_bmlist);
    my_Cmd_AddCommand("++nix_demo_bmdel",(void*)cmd_nix_demo_bmdel);
    my_Cmd_AddCommand("++nix_demo_bmren",(void*)cmd_nix_demo_bmren);
    my_Cmd_AddCommand("++nix_demo_pov_next",(void*)cmd_nix_demo_pov_next);
    my_Cmd_AddCommand("++nix_demo_pov_prev",(void*)cmd_nix_demo_pov_prev);
    my_Cmd_AddCommand("++nix_demo_freecam",(void*)cmd_nix_demo_freecam);
    #endif /* DEMOSYSTEM */

	// command to output checksum
	my_Cmd_AddCommand("++nix_checksum",&cmd_nix_checksum);

	orig_Cmd_AddCommand("++nix_spackage_register",(void*)spackage_register);
	
}

#ifdef DEMOSYSTEM
static void cmd_nix_demo_pov(void)
{
    if (orig_Cmd_Argc() != 2) {
        SOFPPNIX_PRINT("usage: ++nix_demo_pov <slot|-1>");
        return;
    }
    int slot = atoi(orig_Cmd_Argv(1));
    if (!demo_system.demo_player) {
        SOFPPNIX_PRINT("demo player not active");
        return;
    }
    demo_system.demo_player->watch_slot = slot;
    SOFPPNIX_PRINT("demo pov set to %d", slot);
}

static void cmd_nix_demo_freecam(void)
{
    if (orig_Cmd_Argc() != 2) {
        SOFPPNIX_PRINT("usage: ++nix_demo_freecam <0|1>");
        return;
    }
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    int on = atoi(orig_Cmd_Argv(1));
    if (on != 0 && on != 1) on = 0;
    // Experimental: global freecam switch; proper per-viewer requires netchan->slot mapping to viewer state
    demo_system.demo_player->watch_slot = -1;
    SOFPPNIX_PRINT("demo freecam %s (POV decoupled)", on ? "on" : "off");
}

static void cmd_nix_demo_pov_next(void)
{
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    auto it = demo_system.demos.find(spawncount);
    if (it == demo_system.demos.end() || !it->second) {
        SOFPPNIX_PRINT("no demo data");
        return;
    }
    // pick next available slot key at current frame
    int cur = demo_system.demo_player->current_frame;
    auto itFrame = it->second->demo_frames_map.find(cur);
    if (itFrame == it->second->demo_frames_map.end() || itFrame->second.empty()) {
        SOFPPNIX_PRINT("no frame data");
        return;
    }
    std::vector<int> keys; keys.reserve(itFrame->second.size());
    for (const auto &kv : itFrame->second) keys.push_back(kv.first);
    std::sort(keys.begin(), keys.end());
    int curWatch = demo_system.demo_player->watch_slot;
    if (curWatch == -1) curWatch = keys.front();
    auto pos = std::find(keys.begin(), keys.end(), curWatch);
    if (pos == keys.end() || ++pos == keys.end()) demo_system.demo_player->watch_slot = keys.front();
    else demo_system.demo_player->watch_slot = *pos;
    SOFPPNIX_PRINT("demo pov set to %d", demo_system.demo_player->watch_slot);
}

static void cmd_nix_demo_pov_prev(void)
{
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    auto it = demo_system.demos.find(spawncount);
    if (it == demo_system.demos.end() || !it->second) {
        SOFPPNIX_PRINT("no demo data");
        return;
    }
    int cur = demo_system.demo_player->current_frame;
    auto itFrame = it->second->demo_frames_map.find(cur);
    if (itFrame == it->second->demo_frames_map.end() || itFrame->second.empty()) {
        SOFPPNIX_PRINT("no frame data");
        return;
    }
    std::vector<int> keys; keys.reserve(itFrame->second.size());
    for (const auto &kv : itFrame->second) keys.push_back(kv.first);
    std::sort(keys.begin(), keys.end());
    int curWatch = demo_system.demo_player->watch_slot;
    if (curWatch == -1) curWatch = keys.front();
    auto pos = std::find(keys.begin(), keys.end(), curWatch);
    if (pos == keys.begin() || pos == keys.end()) demo_system.demo_player->watch_slot = keys.back();
    else demo_system.demo_player->watch_slot = *(pos-1);
    SOFPPNIX_PRINT("demo pov set to %d", demo_system.demo_player->watch_slot);
}

static void cmd_nix_match_history(void)
{
    // List available in-memory demos (by spawncount) with minimal details
    int count = 0;
    for (const auto &kv : demo_system.demos) {
        int id = kv.first;
        const std::unique_ptr<DemoData> &d = kv.second;
        if (!d) continue;
        int frames = (int)d->demo_frames_map.size();
        if (frames == 0) continue;
        SOFPPNIX_PRINT("id %d  frames %d  map %s", id, frames, d->level_name.c_str());
        count++;
    }
    if (!count) {
        SOFPPNIX_PRINT("no in-memory demos");
    }
}

static void cmd_nix_demo_pause(void)
{
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    demo_system.demo_player->paused = true;
    SOFPPNIX_PRINT("demo paused at frame %d", demo_system.demo_player->current_frame);
}

static void cmd_nix_demo_resume(void)
{
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    demo_system.demo_player->paused = false;
    SOFPPNIX_PRINT("demo resumed");
}

static void cmd_nix_demo_goto(void)
{
    if (orig_Cmd_Argc() != 2) {
        SOFPPNIX_PRINT("usage: ++nix_demo_goto <frame>");
        return;
    }
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    int target = atoi(orig_Cmd_Argv(1));
    if (target == 0 && strcmp(orig_Cmd_Argv(1), "0") != 0) {
        SOFPPNIX_PRINT("invalid frame");
        return;
    }
    if (target < demo_system.demo_player->first_frame) target = demo_system.demo_player->first_frame;
    if (target > demo_system.demo_player->final_frame) target = demo_system.demo_player->final_frame;
    demo_system.demo_player->current_frame = target;
    SOFPPNIX_PRINT("demo current frame %d", demo_system.demo_player->current_frame);
}

static void cmd_nix_demo_rate(void)
{
    if (orig_Cmd_Argc() != 2) {
        SOFPPNIX_PRINT("usage: ++nix_demo_rate <step>");
        return;
    }
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    int step = atoi(orig_Cmd_Argv(1));
    if (step == 0) step = 1;
    if (step < 0) step = 1;
    if (step > 32) step = 32; // cap to prevent runaway fast-forward
    demo_system.demo_player->frame_step = step;
    SOFPPNIX_PRINT("demo frame step %d", step);
}

// Optional: loop toggle
static void cmd_nix_demo_loop(void)
{
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    demo_system.demo_player->loop = !demo_system.demo_player->loop;
    SOFPPNIX_PRINT("demo loop %s", demo_system.demo_player->loop ? "on" : "off");
}

static void cmd_nix_demo_seekms(void)
{
    if (orig_Cmd_Argc() != 2) {
        SOFPPNIX_PRINT("usage: ++nix_demo_seekms <ms>");
        return;
    }
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    int target_ms = atoi(orig_Cmd_Argv(1));
    auto it = demo_system.demos.find(spawncount);
    if (it == demo_system.demos.end() || !it->second) {
        SOFPPNIX_PRINT("no demo data");
        return;
    }
    std::unique_ptr<DemoData> &d = it->second;
    int new_frame = demo_system.demo_player->first_frame;
    if (!d->checkpoints.empty()) {
        int best_ts = -1;
        for (const auto &kv : d->checkpoints) {
            int fr = kv.first;
            DemoCheckpoint *cp = kv.second.get();
            if (!cp) continue;
            int ts = cp->timestamp;
            if (ts <= target_ms && ts > best_ts) {
                best_ts = ts;
                new_frame = fr;
            }
        }
        if (best_ts < 0) {
            // pick earliest checkpoint
            int min_ts = INT_MAX;
            for (const auto &kv : d->checkpoints) {
                DemoCheckpoint *cp = kv.second.get();
                if (!cp) continue;
                if (cp->timestamp < min_ts) {
                    min_ts = cp->timestamp;
                    new_frame = kv.first;
                }
            }
        }
        SOFPPNIX_PRINT("seek %d ms -> frame %d (checkpoint)", target_ms, new_frame);
    } else {
        // fallback: estimate 50ms per frame
        int guess = demo_system.demo_player->first_frame + (target_ms / 50);
        if (guess < demo_system.demo_player->first_frame) guess = demo_system.demo_player->first_frame;
        if (guess > demo_system.demo_player->final_frame) guess = demo_system.demo_player->final_frame;
        new_frame = guess;
        SOFPPNIX_PRINT("no checkpoints; guessed frame %d", new_frame);
    }
    demo_system.demo_player->current_frame = new_frame;
}

static void cmd_nix_demo_nextcp(void)
{
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    auto it = demo_system.demos.find(spawncount);
    if (it == demo_system.demos.end() || !it->second) {
        SOFPPNIX_PRINT("no demo data");
        return;
    }
    std::unique_ptr<DemoData> &d = it->second;
    if (d->checkpoints.empty()) {
        SOFPPNIX_PRINT("no checkpoints");
        return;
    }
    int cur = demo_system.demo_player->current_frame;
    int next_frame = INT_MAX;
    bool found = false;
    for (const auto &kv : d->checkpoints) {
        int fr = kv.first;
        if (fr > cur && fr < next_frame) {
            next_frame = fr;
            found = true;
        }
    }
    if (!found) {
        SOFPPNIX_PRINT("no next checkpoint");
        return;
    }
    demo_system.demo_player->current_frame = next_frame;
    SOFPPNIX_PRINT("moved to checkpoint frame %d", next_frame);
}

static void cmd_nix_demo_prevcp(void)
{
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    auto it = demo_system.demos.find(spawncount);
    if (it == demo_system.demos.end() || !it->second) {
        SOFPPNIX_PRINT("no demo data");
        return;
    }
    std::unique_ptr<DemoData> &d = it->second;
    if (d->checkpoints.empty()) {
        SOFPPNIX_PRINT("no checkpoints");
        return;
    }
    int cur = demo_system.demo_player->current_frame;
    int prev_frame = -1;
    bool found = false;
    for (const auto &kv : d->checkpoints) {
        int fr = kv.first;
        if (fr < cur && fr > prev_frame) {
            prev_frame = fr;
            found = true;
        }
    }
    if (!found) {
        SOFPPNIX_PRINT("no previous checkpoint");
        return;
    }
    demo_system.demo_player->current_frame = prev_frame;
    SOFPPNIX_PRINT("moved to checkpoint frame %d", prev_frame);
}

static void cmd_nix_demo_seekpct(void)
{
    if (orig_Cmd_Argc() != 2) {
        SOFPPNIX_PRINT("usage: ++nix_demo_seekpct <0-100>");
        return;
    }
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    int pct = atoi(orig_Cmd_Argv(1));
    if (pct < 0) pct = 0; if (pct > 100) pct = 100;
    int span = demo_system.demo_player->final_frame - demo_system.demo_player->first_frame;
    int target_frame = demo_system.demo_player->first_frame + (span * pct) / 100;

    // Prefer nearest checkpoint at/before target if available
    auto it = demo_system.demos.find(spawncount);
    if (it != demo_system.demos.end() && it->second && !it->second->checkpoints.empty()) {
        int best = demo_system.demo_player->first_frame;
        for (const auto &kv : it->second->checkpoints) {
            int fr = kv.first;
            if (fr <= target_frame && fr >= best) best = fr;
        }
        target_frame = best;
        SOFPPNIX_PRINT("seek %d%% -> frame %d (checkpoint)", pct, target_frame);
    } else {
        SOFPPNIX_PRINT("seek %d%% -> frame %d", pct, target_frame);
    }
    demo_system.demo_player->current_frame = target_frame;
}

static void cmd_nix_demo_bmset(void)
{
    if (orig_Cmd_Argc() != 2) {
        SOFPPNIX_PRINT("usage: ++nix_demo_bmset <name>");
        return;
    }
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    std::string name = orig_Cmd_Argv(1);
    if (name.empty()) {
        SOFPPNIX_PRINT("invalid name");
        return;
    }
    demo_system.demo_player->bookmarks[name] = demo_system.demo_player->current_frame;
    SOFPPNIX_PRINT("bookmark '%s' set at frame %d", name.c_str(), demo_system.demo_player->current_frame);
}

static void cmd_nix_demo_bmgoto(void)
{
    if (orig_Cmd_Argc() != 2) {
        SOFPPNIX_PRINT("usage: ++nix_demo_bmgoto <name>");
        return;
    }
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    std::string name = orig_Cmd_Argv(1);
    auto it = demo_system.demo_player->bookmarks.find(name);
    if (it == demo_system.demo_player->bookmarks.end()) {
        SOFPPNIX_PRINT("bookmark '%s' not found", name.c_str());
        return;
    }
    int target = it->second;
    if (target < demo_system.demo_player->first_frame) target = demo_system.demo_player->first_frame;
    if (target > demo_system.demo_player->final_frame) target = demo_system.demo_player->final_frame;
    demo_system.demo_player->current_frame = target;
    SOFPPNIX_PRINT("jumped to bookmark '%s' (frame %d)", name.c_str(), target);
}

static void cmd_nix_demo_bmlist(void)
{
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    if (demo_system.demo_player->bookmarks.empty()) {
        SOFPPNIX_PRINT("no bookmarks");
        return;
    }
    for (const auto &kv : demo_system.demo_player->bookmarks) {
        SOFPPNIX_PRINT("%s -> %d", kv.first.c_str(), kv.second);
    }
}

static void cmd_nix_demo_bmdel(void)
{
    if (orig_Cmd_Argc() != 2) {
        SOFPPNIX_PRINT("usage: ++nix_demo_bmdel <name>");
        return;
    }
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    std::string name = orig_Cmd_Argv(1);
    auto it = demo_system.demo_player->bookmarks.find(name);
    if (it == demo_system.demo_player->bookmarks.end()) {
        SOFPPNIX_PRINT("bookmark '%s' not found", name.c_str());
        return;
    }
    demo_system.demo_player->bookmarks.erase(it);
    SOFPPNIX_PRINT("deleted bookmark '%s'", name.c_str());
}

static void cmd_nix_demo_bmren(void)
{
    if (orig_Cmd_Argc() != 3) {
        SOFPPNIX_PRINT("usage: ++nix_demo_bmren <old> <new>");
        return;
    }
    if (!demo_system.demo_player || !demo_system.demo_player->active) {
        SOFPPNIX_PRINT("no active demo");
        return;
    }
    std::string oldn = orig_Cmd_Argv(1);
    std::string newn = orig_Cmd_Argv(2);
    if (newn.empty()) {
        SOFPPNIX_PRINT("invalid new name");
        return;
    }
    auto it = demo_system.demo_player->bookmarks.find(oldn);
    if (it == demo_system.demo_player->bookmarks.end()) {
        SOFPPNIX_PRINT("bookmark '%s' not found", oldn.c_str());
        return;
    }
    if (demo_system.demo_player->bookmarks.find(newn) != demo_system.demo_player->bookmarks.end()) {
        SOFPPNIX_PRINT("bookmark '%s' already exists", newn.c_str());
        return;
    }
    int frame = it->second;
    demo_system.demo_player->bookmarks.erase(it);
    demo_system.demo_player->bookmarks[newn] = frame;
    SOFPPNIX_PRINT("renamed bookmark '%s' -> '%s'", oldn.c_str(), newn.c_str());
}

std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
	std::vector<std::string> substrings;
	size_t start = 0;
	size_t end = 0;
	while ((end = str.find(delimiter, start)) != std::string::npos) {
		substrings.push_back(str.substr(start, end - start));
		start = end + delimiter.length();
	}
	substrings.push_back(str.substr(start));
	return substrings;
}

int hexStringToByteArray(const std::string& hexString, unsigned char* buffer, size_t bufferSize) {
	std::vector<std::string> hexBytes = split(hexString, " ");
	if (hexBytes.size() > bufferSize) {
		throw std::invalid_argument("Input string length exceeds buffer size");
	}
	for (size_t i = 0; i < hexBytes.size(); i++) {
		buffer[i] = static_cast<unsigned char>(std::stoi(hexBytes[i], nullptr, 16));
	}
	return hexBytes.size();
}

typedef struct usercmd_S
{
	byte	msec; // 10
	byte	buttons; // [+1]
	byte	lightlevel;		// light level the player is standing on
	byte	lean;			// -1 or 1 [+3]
	short	angles[3]; // [+4/6/8]
	short	forwardmove; // 0A
	short	sidemove; // 0C
	short	upmove; // 0E
	
	float	fireEvent;
	float	altfireEvent;
} usercmd_n;

void cmd_nix_checksum(void)
{
	sizebuf_t sb;
	usercmd_n * u_test = malloc(sizeof(usercmd_n));
	usercmd_n * u_empty = malloc(sizeof(usercmd_n));

	memset(u_empty,0,sizeof(usercmd_n));
	memset(u_test,0,sizeof(usercmd_n));
	memset(&sb,0,sizeof(sb));

	u_test->angles[0] = 1020;
	u_test->angles[1] = 1020;
	u_test->angles[2] = 1020;
	u_test->forwardmove = 2048;
	u_test->sidemove = 2048;
	u_test->upmove = 2048;
	u_test->msec = 100;
	u_test->buttons = 0xFF;
	u_test->lean = 0;
	u_test->lightlevel = 0xFF;
	u_test->fireEvent = 0;
	u_test->altfireEvent = 0;

	sb.data = (byte*)malloc(64);
	sb.maxsize = 64;

	// orig_MSG_WriteDeltaUsercmd(&sb,u_empty,u_test);
	void * out_packet = malloc(1024);
	void (*op_construct)(void * self, int size) = 0x081A8F00;
	op_construct(out_packet,0x64);

	orig_PAK_WriteDeltaUsercmd(out_packet,(usercmd_t*)u_empty,(usercmd_t*)u_test);


	std::vector<signed char> * v = (std::vector<signed char>*)(out_packet+8);
	

	// Iterates the vector v and print all of its elements
	for (int i = 0; i < v->size(); i++) {
		// SOFPPNIX_DEBUG("Vector %i : %02X\n",i,*(v->data()+i));
		printf("%02X",*(v->data()+i));
	}
	printf("\n%i\nBITMODE!\n",v->size());

	for ( int i = 0 ; i < v->size() * 8; i++ ) {
		int bit_to_byte = i / 8;
		int bit_to_bit = i % 8;
		if ( (*(v->data()+bit_to_byte) & (1 << bit_to_bit)) != 0 )
			printf("1");
		else
			printf("0");
	}
	printf("\n");

	// for (const auto& element : *v) {
	// 	// do something with the element
	// 	// for example, print it to the console
	// 	// std::cout << element << " ";
	// 	printf("%02X",element);
	// }
	// printf("\n%i\n",v->size());
/*
	// print every sb->data at sequence of hex bytes
	for (int i = 0; i < sb.cursize; i++) {
		// SOFPPNIX_PRINT("%02X ",sb.data[i]);
		printf("%02X ",sb.data[i]);
	}
	printf("Length = %i\n", sb.cursize);

*/

	// int seq = atoi(orig_Cmd_Argv(1));
	// char * buffer = orig_Cmd_Argv(2);
	// int len = strlen(buffer);

	int seq = 103;
	unsigned char buffer[1024];
	int len = hexStringToByteArray(std::string("13 00 00 00 01 20 00 05 00 fa 17 c0 42 11 00 00 f0 47 00 08 40 01 80 fe 05 b0 50 04 00 00 fc 11 00 02 50 00 a0 7f 01 2c 14 01 00 00 7f"),buffer,1024);
	// debug print seq is : input_data is :
	// SOFPPNIX_DEBUG("seq is : %i input_data is : %s\n",seq,input_data);

	// int out_seq = *(unsigned int*)((*(unsigned int*)0x0829D494) + 0x4F4);
	SOFPPNIX_DEBUG("Checksum : %02X",orig_COM_BlockSequenceCheckByte(buffer,len, seq));

	free(sb.data);
	free(out_packet);
}


/*
 The strings passed here are not part of large buffer, so copy first
*/
void my_Com_Printf(char *msg, ...) {
	#if 0
	char text[1024];
	msg = &text[0];
	strcpy(text,msg);
	// point msg back to text
	
	int len_msg = strlen(msg) + 1;
	if ( len_msg > 1 && len_msg < 1024 ) // any characters? (including null), require 1 free space.
	{
		// shift to right one. ( grows by 1 )
		// save end character ( its just a null character )
		// start at null character. ( usually -1 to make len into offset )
		// but the null character cancels out ( confusing )
		// explicitly change len to include null cahracter
		for ( char * p=msg+len_msg - 1;p>msg;p-- ) {
			*p = *(p-1);
		}
		*msg=0x03;
		*(msg+len_msg) = 0x00; // fits
	}

	// orig_Com_Printf(msg);

	void *args = __builtin_apply_args();

	__builtin_apply((void (*)())orig_Com_Printf, args, 512);
	#else
		char text[1024];
		va_list args;

		va_start(args, msg);
		vsnprintf(text, sizeof(text), msg, args);
		va_end(args);

		// char color[1024];
		
		// snprintf(color,1024,"%c%s",0x03,text);

		orig_Com_Printf(text);
		
	#endif
}


void my_Cmd_AddCommand(char * cmd, void * callback)
{
	// printf("Command Created : %s\n",cmd);

	// if ( !strcmp(cmd,"fx_save") ) {
	//     void *return_address = __builtin_return_address(0);
	//     printf("Cmd_AddCommand Return address: %p\n", return_address);
	// }
	orig_Cmd_AddCommand(cmd,callback);
}

void my_Cmd_RemoveCommand(char * cmd)
{
	orig_Cmd_RemoveCommand(cmd);
}

/*
typedef enum
{
	ca_uninitialized,
	ca_disconnected=1,
	ca_connecting=7,
	ca_connected,
	ca_active=8
} connstate_t;
*/
void cmd_nix_client_state(void)
{
	int** connected_state = 0x0829D494;

	SOFPPNIX_PRINT("Connected state is %i",**connected_state);
}

void cmd_nix_client_map(void)
{
	unsigned int add= 0x0829D480 + 0x1F44;
	char ** mapname = add;

	SOFPPNIX_PRINT("Mapname is %s",*mapname);
}


void fake_cb(void) {
SOFPPNIX_DEBUG("Fake cb");
}
void cmd_nix_test(void)
{

	float * fov_x = 0x8312a40;
	float * fov_y = 0x8312a44;

	SOFPPNIX_DEBUG("%f %f\n",*fov_x,*fov_y);
	return;
	orig_GhoulServerFlushClients();
	return;
	sizebuf_t buf;
	//sizebuf_t memset to 0.
	memset(&buf,0,sizeof(sizebuf_t));

	char	buf_data[1400];

	//buffer
	buf.data = buf_data;
	buf.data[0] = 0x00;
	buf.maxsize = 1400;

	short Len = 0;
	SOFPPNIX_DEBUG("Type : %02X",svc_ghoulreliable);
	orig_MSG_WriteByte(&buf,svc_ghoulreliable);
	orig_SZ_Write(&buf,&Len,2);

	int frameNum = stget(0x082AF680,0x10);
	int written = 0;
	//slot, frameNum, buffer, freeSpace, int* written
	int unknown = orig_GhoulPackReliable(0, frameNum, buf.data, 1400/2 - (buf.cursize-8),&written);

	SOFPPNIX_DEBUG("Unknown = %i ... Written = %i",unknown,written);

	orig_GhoulReliableHitwire(0,frameNum);

	SOFPPNIX_DEBUG("Test done");
}


void cmd_nix_demomap(void)
{

	if ( demo_system.recording_status ) {
		SOFPPNIX_PRINT("Stop your recording first.");
		return;
	}
	//use serverstate 9 for demoActive.
	int serverstate = stget(0x082AF680,0);
	if ( serverstate == 3 ) {
		SOFPPNIX_PRINT("TODO: Clear demo and load other demo whilst playback.");
		return;
	}

	/*
		use the .dm3 extension format for files.
		else assumed spawncount selection integer id from memdump list.
	*/
    if ( !demo_system.LoadDemo() ) return;
	

	demo_system.serverstate_trigger_play = true;

	orig_SV_Map(true, "serversidedemo" , false );

	//Disable demofile
	demo_system.serverstate_trigger_play = false;
}

void cmd_nix_record(void)
{

	demo_system.demo_recorder->startRecording();
	
}

void cmd_nix_stoprecord(void)
{
	
#if 0
	//Copy/Save the buffer.
	for(int i = 0; i < 16; ++i) {
		ghoulChunksReplay[i] = ghoulChunks[i]; // Copy each vector
		//Iterate the vector to deep copy
		for (size_t j = 0; j < ghoulChunksReplay[i].size(); ++j) {
			ghoulChunksReplay[i][j].data = malloc(ghoulChunks[i][j].len);
			memcpy(ghoulChunksReplay[i][j].data, ghoulChunks[i][j].data, ghoulChunks[i][j].len);
		}
		ghoulChunksSavedReplay[i] = ghoulChunksSaved[i];
	}
#endif
	demo_system.demo_recorder->endRecording();
}
#endif /* DEMOSYSTEM */