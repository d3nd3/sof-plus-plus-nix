#include "common.h"

#include "decodekey.h"

#include <cmath>



netadr_t my_netadr = {
	.type = NA_IP, // assuming you want to use IP address type
	.ip = {192, 168, 0, 1}, // assuming your IP address is 192.168.0.1
	.ipx = {0}, // assuming you don't need to use IPX address
	.port = 12345 // assuming you want to use port number 12345
};

//orig_Cmd_Argv(1),orig_Cmd_Argv(2)
/*
	// linux: SOF+%16s+%8s+%i/%i/%i+%s+%i+%i\n
	// windows: SOF+%16s+%8s+%i/%i/%i+%s+%s+%i\n

	char t[1024] = "SOF+hostname1+dm/abc+0/0/8+some1+0+0\n";

	Called by pingservers. By serverbox Menu.

	// SOF+hostname+mapname+num_players/NON_PLAYER_COUNT/max_players+$GAME_CVAR+$str(deathmatch)+sv_violence
	// linux: SOF+%16s+%8s+%i/%i/%i+%s+%i+%i\n
	// windows: SOF+%16s+%8s+%i/%i/%i+%s+%s+%i\n

	Linux seems to treat the $GAME_CVAR as "game-type". Crashes on empty string.
*/


int gs_select_sock = 0;
// Custom-style master server. Hard-coded, intentionally not part of sv_masters.
netadr_t sof1svmaster_ip;
// a gs_item struct that contains a netadr_t and a boolean hinting if status received
typedef struct gs_item_s {
	netadr_t addr;

std::chrono::steady_clock::time_point queryTime;
std::chrono::steady_clock::time_point lastSentTime;
} gs_item_t;


// a std vector of gs_items
std::vector<gs_item_t> gs_items;

static std::chrono::steady_clock::time_point next_gamespy_heartbeat;
static unsigned int gamespy_query_id = 1;

static int socketPort(int fd, int fallback)
{
	struct sockaddr_in address = {};
	socklen_t length = sizeof(address);
	return fd >= 0 && getsockname(fd, (struct sockaddr *)&address, &length) == 0
		? ntohs(address.sin_port) : fallback;
}

static int gamespyBoundPort(void)
{
	return socketPort(gs_select_sock,
		gamespyport ? c_string_to_int(gamespyport->string) : 0);
}

static int serverBoundPort(void)
{
	// The engine stores the dedicated IP socket at ip_sockets[1].
	int fd = *(const int *)0x085568ac;
	return socketPort(fd, hostport ? c_string_to_int(hostport->string) : 0);
}

// These globals are fixed in the non-PIE SoF executable.
static sizebuf_t *const engine_net_message = (sizebuf_t *)0x084237F0;
static netadr_t *const engine_net_from = (netadr_t *)0x084237DC;

#define GAMESPY_MASTER_PORT "27900"

static std::vector<std::string> splitMasterList(const std::string &list);

static bool resolveMasterServer(const std::string &target, netadr_t *out)
{
	std::string address = target;
	if (address.find(':') == std::string::npos)
		address += ":" GAMESPY_MASTER_PORT;
	std::memset(out, 0, sizeof(*out));
	if (!orig_NET_StringToAdr(&address[0], out))
		return false;
	return out->ip[0] || out->ip[1] || out->ip[2] || out->ip[3];
}

static void sendGamespyMasterPacket(char *data)
{
	std::string masters = sv_masters && sv_masters->string ? sv_masters->string : "";
	if (masters.empty() && _sp_sv_master_gamespy && _sp_sv_master_gamespy->string)
		masters = _sp_sv_master_gamespy->string;
	if (masters.empty())
		return;
	for (const std::string &target : splitMasterList(masters)) {
		netadr_t master;
		if (!resolveMasterServer(target, &master)) {
			SOFPPNIX_PRINT("Unable to resolve master server %s\n", target.c_str());
			continue;
		}
		struct sockaddr_in address;
		NetadrToSockadr(&master, &address);
		if (sendto(gs_select_sock, data, strlen(data), 0,
		           (struct sockaddr *)&address, sizeof(address)) < 0)
			error_exit("Failed to send Gamespy packet: %s\n", std::strerror(errno));
	}
}

static void sendCtrlGamespyPacket(char *data)
{
	struct sockaddr_in address;
	NetadrToSockadr(&sof1svmaster_ip, &address);
	if (sendto(gs_select_sock, data, strlen(data), 0,
	           (struct sockaddr *)&address, sizeof(address)) < 0)
		error_exit("Failed to send CTRL packet: %s\n", std::strerror(errno));
}

static void sendCtrlHostPacket(char *data)
{
	orig_NET_SendPacket(NS_SERVER, strlen(data), data, sof1svmaster_ip);
}

static void sendGamespyHeartbeat(void)
{
	char data[128];
	snprintf(data, sizeof(data), "\\heartbeat\\%d\\gamename\\sofretail%s",
	         gamespyBoundPort(), "\\statechanged\\");
	sendGamespyMasterPacket(data);
	next_gamespy_heartbeat =
		std::chrono::steady_clock::now() + std::chrono::seconds(150);
}

static bool readGamespySecureChallenge(const unsigned char *data, size_t length,
                                       std::string &challenge)
{
	const char *prefixes[] = {"\\status\\secure\\", "\\secure\\"};
	for (const char *prefix : prefixes) {
		size_t start = strlen(prefix);
		if (length <= start || memcmp(data, prefix, start) != 0)
			continue;
		size_t end = start;
		while (end < length && data[end] != 0 && data[end] != '\\')
			++end;
		challenge.assign((const char *)data + start, end - start);
		return !challenge.empty();
	}
	return false;
}

static int statusSpInt(cvar_t *cvar, int fallback = 0)
{
	return cvar ? (int)cvar->value : fallback;
}

static const char *statusSpString(cvar_t *cvar)
{
	return cvar && cvar->string ? cvar->string : "";
}

static bool readStatusSpQuery(std::string &query, bool &includePlayers)
{
	if (!engine_net_message || !engine_net_message->data ||
	    engine_net_message->cursize <= 4)
		return false;

	const char *text = (char *)engine_net_message->data + 4;
	size_t length = 0;
	while (length < (size_t)engine_net_message->cursize - 4 && text[length])
		++length;

	std::istringstream input(std::string(text, length));
	std::string command, variant;
	if (!(input >> command >> variant) ||
	    command != "status" || variant != "sp")
		return false;

	std::vector<std::string> args;
	std::string arg;
	while (input >> arg)
		args.push_back(arg);

	query = args.size() && args[0].size() == 4 ? args[0] : "";
	includePlayers = args.size() < 3 || (std::atoi(args[2].c_str()) & 1);
	return true;
}

static std::string makeStatusSpResponse(const std::string &query,
                                         bool includePlayers)
{
	std::ostringstream output;
	auto field = [&output](const auto &value) { output << '\\' << value; };
	const int players = maxclients ? countPlayersInGame() : 0;
	const int clients = statusSpInt(maxclients);

	field(query); field(1); field("sp"); field(20140531); field((long)time(NULL));
	field(7488); field(4513); field(players); field(0); field(0); field(0);
	field(0); field(150); field(0); output << '\n';

	field(query); field(2); field(statusSpInt(cheats)); field(1); field(800);
	field(statusSpInt(ctf_loops, 10)); field(statusSpInt(deathmatch));
	field(statusSpInt(dmflags)); field(statusSpInt(fraglimit));
	output << std::fixed << std::setprecision(6)
	       << '\\' << (g_movescale ? g_movescale->value : 1.0f);
	field(statusSpString(hostname)); field(statusSpString(mapname));
	field(clients); field(clients); field(statusSpInt(sv_suicidepenalty, 2));
	field(statusSpInt(sv_violence, 63));
	output << '\\' << (timelimit ? timelimit->value : 0.0f) << '\n';

	int record = 2;
	if (includePlayers) {
		for (int slot = 0; slot < clients; ++slot) {
			void *client = getClientX(slot);
			if (*(int *)client != cs_spawned)
				continue;

			field(query); field(++record); field(slot); field(""); field("");
			field(0); field(0); field(0); field(0); field(12); field(0);
			field(163); field(70); field(""); field(""); field("John Mullinsqqu");
			field(36); field(0); field(0); field(0); field(""); field(0);
			field(0); field(0); field(0); field("");
			output << '\n';
		}
	}
	field(query); field(++record); output << "\n\n";
	return output.str();
}

void my_SV_ConnectionlessPacket(void)
{
	std::string query;
	bool includePlayers = false;
	if (!sv_public || !sv_public->value ||
	    !readStatusSpQuery(query, includePlayers)) {
		orig_SV_ConnectionlessPacket();
		return;
	}

	std::string response = makeStatusSpResponse(query, includePlayers);
	char format[] = "print\n%s";
	orig_Netchan_OutOfBandPrint(NS_SERVER, *engine_net_from, format,
	                             response.c_str());
}

static bool gamespyStartsWith(const unsigned char *data, size_t length,
                              const char *prefix)
{
	size_t prefixLength = strlen(prefix);
	return length >= prefixLength && !memcmp(data, prefix, prefixLength);
}

static const char *gamespyGameType(void)
{
	switch (std::lround(deathmatch ? deathmatch->value : 0)) {
	case 1: return "DM";
	case 2: return "Assassin";
	case 3: return "Arsenal";
	case 4: return "CTF";
	case 5: return "Realistic";
	case 6: return "Control";
	case 7: return "CTB";
	default: return "Unknown";
	}
}

static std::string gamespyInfoFields(void)
{
	std::ostringstream out;
	out << "\\hostname\\" << statusSpString(hostname)
	    << "\\hostport\\" << serverBoundPort()
	    << "\\mapname\\" << statusSpString(mapname)
	    << "\\gametype\\" << gamespyGameType()
	    << "\\numplayers\\" << countPlayersInGame()
	    << "\\maxplayers\\" << statusSpInt(maxclients)
	    << "\\gamemode\\openplaying"
	    << "\\violence\\" << statusSpInt(sv_violence, 0);
	return out.str();
}

static std::string gamespyRulesFields(void)
{
	std::ostringstream out;
	out << "\\timelimit\\" << statusSpInt(timelimit)
	    << "\\fraglimit\\" << statusSpInt(fraglimit)
	    << "\\dmflags\\" << statusSpInt(dmflags)
	    << "\\movescale\\";
	if (g_movescale && g_movescale->value == 1.0f)
		out << "100%";
	else
		out << (int)((g_movescale ? g_movescale->value : 1.0f) * 100) << '%';
	out << "\\cheats\\" << (statusSpInt(cheats) ? "enabled" : "disabled")
	    << "\\ctf_loops\\" << statusSpInt(ctf_loops, 10)
	    << "\\suicide_penalty\\" << statusSpInt(sv_suicidepenalty, 2);
	return out.str();
}

static std::string gamespyBasicFields(void)
{
	return "\\gamename\\sofretail\\gamever\\1.07f\\location\\1";
}

static std::string gamespyPlayerFields(void)
{
	std::ostringstream out;
	for (int i = 0; i < countPlayersInGame(); ++i) {
		out << "\\player_" << i << "\\John Mullinsqqu"
		    << "\\frags_" << i << "\\0"
		    << "\\ping_" << i << "\\38"
		    << "\\skin_" << i << "\\dekker"
		    << "\\team_" << i << "\\The Order";
	}
	return out.str();
}

static std::string gamespyFinish(const std::string &fields, unsigned int query)
{
	return fields + "\\final\\\\queryid\\" +
		std::to_string(query) + ".1";
}

static std::string gamespyStatusResponse(bool secure,
                                         const std::string &challenge,
                                         unsigned int query)
{
	std::string fields = gamespyBasicFields() +
		gamespyInfoFields() + gamespyRulesFields() + gamespyPlayerFields();
	if (secure) {
		std::vector<uint8_t> key = ValidateKey("iVn3a3", challenge);
		return fields + "\\validate\\" +
			std::string(key.begin(), key.end()) +
			"\\final\\\\queryid\\" + std::to_string(query) + ".1";
	}
	return gamespyFinish(fields, query);
}

static bool handleGamespyQuery(const unsigned char *data, int length,
                               const struct sockaddr_in &from)
{
	if (!sv_public || !sv_public->value)
		return false;

	std::string challenge;
	const bool secure = readGamespySecureChallenge(data, length, challenge);
	const bool status = gamespyStartsWith(data, length, "\\status\\");
	const unsigned int query = ++gamespy_query_id;
	auto send = [&from](const std::string &response) {
		if (sendto(gs_select_sock, response.data(), response.size(), 0,
		           (const struct sockaddr *)&from, sizeof(from)) < 0)
			error_exit("sendto() failed responding to Gamespy query");
	};

	if (secure && !status) {
		std::vector<uint8_t> key = ValidateKey("iVn3a3", challenge);
		send("\\validate\\" + std::string(key.begin(), key.end()) +
		     "\\final\\\\queryid\\" + std::to_string(query) + ".1");
		return true;
	}
	if (status) {
		send(gamespyStatusResponse(secure, challenge, query));
		return true;
	}
	if (gamespyStartsWith(data, length, "\\info\\\\rules\\")) {
		send(gamespyFinish(gamespyInfoFields() + gamespyRulesFields(), query));
		return true;
	}
	if (gamespyStartsWith(data, length, "\\info\\")) {
		send(gamespyFinish(gamespyInfoFields(), query));
		return true;
	}
	if (gamespyStartsWith(data, length, "\\basic\\")) {
		send(gamespyFinish(gamespyBasicFields(), query));
		return true;
	}
	if (gamespyStartsWith(data, length, "\\rules\\")) {
		send(gamespyFinish(gamespyRulesFields(), query));
		return true;
	}
	if (gamespyStartsWith(data, length, "\\players\\")) {
		send(gamespyFinish(gamespyPlayerFields(), query));
		return true;
	}
	if (gamespyStartsWith(data, length, "\\echo\\")) {
		std::string response((const char *)data, length);
		response += "\\final\\\\queryid\\" + std::to_string(query) + ".1";
		send(response);
		return true;
	}
	if (gamespyStartsWith(data, length, "\\packets\\")) {
		const std::string id = std::to_string(query);
		send(gamespyBasicFields() + "\\queryid\\" + id + ".1");
		send(gamespyInfoFields() + "\\queryid\\" + id + ".2");
		send(gamespyRulesFields() + "\\queryid\\" + id + ".3");
		send("\\final\\\\queryid\\" + id + ".4");
		return true;
	}
	return false;
}


#define CLIENT_MASTER_PORT 28900
#define CLIENT_MASTER_TIMEOUT_SEC 5

// Split on spaces and commas so "a b,c" style lists both work.
static std::vector<std::string> splitMasterList(const std::string &list)
{
	std::vector<std::string> entries;
	std::string current;
	for (char c : list) {
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == ',') {
			if (!current.empty()) {
				entries.push_back(current);
				current.clear();
			}
		} else {
			current += c;
		}
	}
	if (!current.empty())
		entries.push_back(current);
	return entries;
}

static bool parseMasterEntry(const std::string &entry, std::string &host, int &port)
{
	host.clear();
	port = CLIENT_MASTER_PORT;
	std::string::size_type colon = entry.find(':');
	if (colon == std::string::npos) {
		host = entry;
	} else {
		host = entry.substr(0, colon);
		try {
			port = std::stoi(entry.substr(colon + 1));
		} catch (...) {
			return false;
		}
	}
	return !host.empty() && port > 0 && port < 65536;
}

// Candidates to try in order. A manually set cl_master goes first,
// then every cl_masters entry, so a timeout falls through to the next.
static std::vector<std::string> clientMasterCandidates(void)
{
	std::string list = DEFAULT_CL_MASTERS;
	if (cl_masters && cl_masters->string && *cl_masters->string)
		list = cl_masters->string;
	std::vector<std::string> entries = splitMasterList(list);
	std::vector<std::string> candidates;
	if (cl_master && cl_master->string && *cl_master->string)
		candidates.push_back(cl_master->string);
	for (const std::string &entry : entries) {
		if (candidates.empty() || entry != candidates[0])
			candidates.push_back(entry);
	}
	return candidates;
}

// Connect with a timeout so a dead master does not hang the client.
static int connectMasterTimeout(const char *host, int port)
{
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo(host, NULL, &hints, &res) != 0)
		return -1;
	struct sockaddr_in server_addr;
	std::memcpy(&server_addr, res->ai_addr, res->ai_addrlen);
	server_addr.sin_port = htons(port);
	freeaddrinfo(res);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		return -1;

	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	int rc = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (rc < 0 && errno != EINPROGRESS) {
		close(sockfd);
		return -1;
	}
	if (rc < 0) {
		fd_set writefds;
		FD_ZERO(&writefds);
		FD_SET(sockfd, &writefds);
		struct timeval timeout = { CLIENT_MASTER_TIMEOUT_SEC, 0 };
		rc = select(sockfd + 1, NULL, &writefds, NULL, &timeout);
		if (rc <= 0) {
			close(sockfd);
			return -1;
		}
		int err = 0;
		socklen_t len = sizeof(err);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &len) < 0 || err != 0) {
			close(sockfd);
			return -1;
		}
	}
	fcntl(sockfd, F_SETFL, flags);
	struct timeval timeout = { CLIENT_MASTER_TIMEOUT_SEC, 0 };
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
	return sockfd;
}

// Full TCP exchange with one master. False on any failure, never exits.
static bool fetchServerListFromMaster(const std::string &host, int port, std::vector<unsigned char> &out)
{
	out.clear();
	int sockfd = connectMasterTimeout(host.c_str(), port);
	if (sockfd < 0) {
		SOFPPNIX_PRINT("Error: Failed to connect to master server %s:%d.\n", host.c_str(), port);
		return false;
	}

	const char* message = "IDQD";
	if (send(sockfd, message, std::strlen(message), 0) < 0) {
		SOFPPNIX_PRINT("Error: Failed to send challenge to master server %s.\n", host.c_str());
		close(sockfd);
		return false;
	}

	unsigned char challenge[1400];
	int challenge_len = recv(sockfd, challenge, sizeof(challenge), 0);
	if (challenge_len <= 0) {
		SOFPPNIX_PRINT("Error: No challenge from master server %s.\n", host.c_str());
		close(sockfd);
		return false;
	}

	std::string received_message((char*)challenge, challenge_len);

	// Split the message string by the backslash character '\'
	std::vector<std::string> parts;
	std::istringstream iss(received_message);
	std::string part;
	while (std::getline(iss, part, '\\')) {
		parts.push_back(part);
	}
	if (parts.empty()) {
		SOFPPNIX_PRINT("Error: Bad challenge from master server %s.\n", host.c_str());
		close(sockfd);
		return false;
	}
	std::string realkey = parts[parts.size() - 1];

	std::vector<uint8_t> key = ValidateKey(std::string("iVn3a3"), realkey);
	std::string hashed_key(key.begin(), key.end());

	std::string response = "\\gamename\\sofretail\\gamever\\1.07f\\location\\0\\validate\\" + hashed_key + "\\final\\\\queryid\\1.1\\";
	if (send(sockfd, response.c_str(), response.size(), 0) < 0) {
		SOFPPNIX_PRINT("Error: Failed to validate with master server %s.\n", host.c_str());
		close(sockfd);
		return false;
	}

	response = "\\list\\cmp\\gamename\\sofretail\\final\\";
	if (send(sockfd, response.c_str(), response.size(), 0) < 0) {
		SOFPPNIX_PRINT("Error: Failed to request list from master server %s.\n", host.c_str());
		close(sockfd);
		return false;
	}

	unsigned char chunk[1400];
	while (true) {
		int r = recv(sockfd, chunk, sizeof(chunk), 0);
		if (r < 0) {
			SOFPPNIX_PRINT("Error: Failed to receive list from master server %s.\n", host.c_str());
			close(sockfd);
			return false;
		}
		if (r == 0)
			break;
		out.insert(out.end(), chunk, chunk + r);
	}
	close(sockfd);
	return true;
}

/*
The port should already be binded somewhere else.
*/
void GetServerList(void)
{
	/*
-------------------------------------------------------------------
		STAGE 1 : IP LIST
		\\list\cmp\gamename\sofretail\final\
-------------------------------------------------------------------
	*/
	std::vector<unsigned char> raw;
	std::string used_master;
	for (const std::string &candidate : clientMasterCandidates()) {
		std::string host;
		int port;
		if (!parseMasterEntry(candidate, host, port))
			continue;
		if (!fetchServerListFromMaster(host, port, raw)) {
			SOFPPNIX_PRINT("Master server %s timed out, trying next.\n", candidate.c_str());
			continue;
		}
		if (raw.size() <= 7 || (raw.size() - 7) % 6 != 0) {
			SOFPPNIX_PRINT("Master server %s returned a bad server list, trying next.\n", candidate.c_str());
			continue;
		}
		used_master = candidate;
		break;
	}
	if (used_master.empty()) {
		SOFPPNIX_PRINT("Error: No master server reachable.\n");
		return;
	}
	if (cl_master && (!cl_master->string || used_master != cl_master->string)) {
		std::string active = used_master;
		setCvarString(cl_master, &active[0]);
	}
	// there is a \final\ at the end. So ignore last 7 bytes
	// hexdump(raw.data(),raw.data()+raw.size());

	// Ignore the first 7 bytes
	unsigned char* data = raw.data();
	size_t bytes_returned = raw.size();
	auto now = std::chrono::steady_clock::now();
	// Parse the buffer and create gamespy structs that contain netadr_t
	// They are global and will be dealt with each frame
	for (size_t i = 0; i < bytes_returned - 7; i += 6) {

		gs_item_t unacked_server;

		netadr_t addr;
		addr.type = NA_IP;
		addr.ip[0] = data[i];
		addr.ip[1] = data[i + 1];
		addr.ip[2] = data[i + 2];
		addr.ip[3] = data[i + 3];
		// netadr_t.port is stored in network order; NetadrToSockadr copies it.
		addr.port = (data[i + 5] << 8) | data[i + 4];

		unacked_server.addr = addr;

		unacked_server.queryTime = now;
		unacked_server.lastSentTime = now;

		// SOFPPNIX_DEBUG("ip is : %d.%d.%d.%d:%d\n",addr.ip[0],addr.ip[1],addr.ip[2],addr.ip[3],htons(addr.port));

		// store the netadr_t in the gs_items vector
		gs_items.push_back(unacked_server);


	}

/*
--------------------------------------------------------------------------
------------------------------GAMESPY PORT--------------------------------
--------------------------------------------------------------------------
*/
	// for ( auto item : gs_items ) {
	// 	std::cout << "item !" << std::endl;
	// }

	// Initiate the responses, (send out all queries)
	for (std::vector<gs_item_t>::iterator it = gs_items.begin(); it != gs_items.end();) {
		struct sockaddr_in gs_server_info;
		netadr_t * na = &(it->addr);

		NetadrToSockadr(na,&gs_server_info);

		// ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
		if (sendto(gs_select_sock, "\\status\\", 8, 0,(struct sockaddr*)&gs_server_info,sizeof(gs_server_info)) < 0) {
			error_exit("Failed to send response. ErrMsg = %s\n",std::strerror(errno));
		}
		it++;
	}
	// Handle all responses on the gamespy port.
	nonBlockingServerResponses();
}

/*
 menu_system 0x80 size.
 Created in M_PushMenu.

 Seg fault on menu free?
 SOF+hostname+mapname+num_players/NON_PLAYER_COUNT/max_players+$GAME_CVAR+$str(deathmatch)+sv_violence
	REQURIED TO TRANSFORM THE GAMEMODE TYPE
*/
void my_menu_AddServer(netadr_t addr,char *data)
{
	// SOFPPNIX_DEBUG("my_menu_AddServer\n");
	// I think this is removing +localhost

	// *(char*)(data + strlen(data) + 1 - 8) = '\n';
	*(char*)(data + strlen(data) + 1 - 8) = 0x00;
	
	// SOFPPNIX_PRINT("BOO: %s\n",data);
	/*
	  If the server which is giving us this data is a WINDOWS server.
	  Translate the DM string into an integer. Does it matter? Maybe doens't matter.
	*/
	std::string input(data);
	
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = input.find("+");
	while (end != std::string::npos) {
		tokens.push_back(input.substr(start, end - start));
		start = end + 1;
		end = input.find("+", start);
	}
	tokens.push_back(input.substr(start));

	if ( tokens.size() >= 3 ) {

		// Shift dm up into the $GAME token
		// Set dm to a number.
		std::string * t = &tokens[tokens.size() - 2];
		std::string * t2 = &tokens[tokens.size() - 3];
		*t2 = *t;
		if ( *t == std::string("DM") ) *t = "1";
		else if ( *t == std::string("Assassin") ) *t = "2";
		else if ( *t == std::string("Arsenal") ) *t = "3";
		else if ( *t == std::string("CTF") ) *t = "4";
		else if ( *t == std::string("Realistic") ) *t = "5";
		else if ( *t == std::string("Control") ) *t = "6";
		else if ( *t == std::string("CTB") ) *t = "7";
		else *t = "8";
	}

	std::string output;
	for (size_t i = 0; i < tokens.size(); i++) {
		output += tokens[i];
		if (i != tokens.size() - 1) {
			output += "+";
		}
	}
	orig_menu_AddServer(addr,output.c_str());
}


/*
[sof++nix] : BOO: Free Server Mode - CTF (unclaimed)+dm/sibctf1+0/0/32++CTF+63+local+
[sof++nix] : BOO:    (#1)SERVER+dm/irqctf1+0/0/32++CTF+63+local+
[sof++nix] : BOO:           Lag DM+dm/jpndm2+0/0/32++DM+63+local+
[sof++nix] : BOO: EURO Bastards+dm/jpnctf1+8/0/16++CTF+63+local+
[sof++nix] : BOO: Free Server Mode - DM (unclaimed)+dm/irqdesertdm+0/0/32++DM+63+local+
[sof++nix] : BOO: [$$] , !CTF! & [�#] SerVer+dm/gerctf1+0/0/32++CTF+63+local+
[sof++nix] : BOO: Arsenal-Megalag[UT-Sounds]+dm/jpnctb1+0/0/32++Arsenal+63+local+
[sof++nix] : BOO:          UT 1995+dm/deuce_arena+0/0/32++DM+63+local+
[sof++nix] : BOO:        Mad Onion+dm/madrange+0/0/32++DM+63+local+
[sof++nix] : BOO: [KK] - Clan Server+dm/irqdm1+0/0/32++DM+32+local+
[sof++nix] : BOO: SOF.exe - Online Multiplayer CTF+dm/lostway+0/0/32++CTF+32+local+
[sof++nix] : BOO: [KK] - [DM] Server+dm/chaos_castle+0/0/32++DM+32+local+
[sof++nix] : BOO: [KK] - [F.S.M] Server - Join as Spectator!+dm/[r&b]time2die+0/0/32++DM+32+local+
[sof++nix] : BOO: ChAoS Unlimited Fire DM Fiber+dm/chaos_arena_5+0/0/16++DM+32+local+
[sof++nix] : BOO:    MHP-Clan.de+dm/sibctb2+0/0/16++DM+32+local+
[sof++nix] : BOO: ChAoS Unlimited Fire SP Fiber+   irq3a+0/0/16++DM+32+local+
[sof++nix] : BOO: Bastard's Corner+dm/sudctf1+0/0/16++CTF+63+local+
[sof++nix] : BOO: Bastard's Corner DM+dm/jpnctb1+0/0/16++DM+63+local+
[sof++nix] : BOO: Assassin Bastards+dm/suddm1+0/0/16++Assassin+63+local+
[sof++nix] : BOO: Bastards on the Bunker+dm/dreamstate2+0/0/16++Team CTB+63+local+
[sof++nix] : BOO: After The Burial(lobby)+dm/mhp_uba+0/0/12++DM+63+local+
[sof++nix] : BOO: Cooperative Mission+dm/coop_streets2_rdam+0/0/12++COOP+63+local+
[sof++nix] : BOO: Cooperative Mission (Single Player)+    tsr1+0/0/12++COOP+63+local+
[sof++nix] : BOO: * * * S H M O O  ' S   G A M E * * * +dm/irqctb2+0/0/12++DM+32+local+
[sof++nix] : BOO: CGZA - SoF Deathmatch+dm/irqctb1+0/0/16++DM+32+local+
[sof++nix] : BOO: Temple Of Assassin BR - ***https://discord.gg/Ys4Fmj6dEB+dm/conbeach+0/0/16++DM+32+local+

*/

/*
	\gamename\sofretail\
	gamever\1.07f\
	location\1\
	hostname\CGZA - SoF Deathmatch\
	hostport\17242\
	mapname\dm/suddm1\
	gametype\DM\
	numplayers\0\
	maxplayers\16\
	gamemode\openplaying\
	violence\32\
	timelimit\15\
	fraglimit\0\
	dmflags\4356\
	movescale\100%\
	cheats\disabled\
	ctf_loops\10\
	suicide_penalty\2
	\final\\queryid\1734.1

	SERVERLIST ACQUIRED FROM TCP CONNECTION TO MASTER SERVER
	THEN A UDP CONNECTION IS MADE TO EACH SERVER IN THAT LIST, ON THEIR GAMESPY PORT
	THE PAYLOAD IS \STATUS\

	THEN THE RESPONSE IS PARSED AND ADDED TO THE SERVER LIST

	HOWEVER THE COMMAND SERVERSTATUS IS ADDING THEM ALSO.
	IT SENDS status\n to the standard hostport on udp.
	It uses the `print` pathway back to client, which calls M_AddServer eventually.
	This pathway is better because includes ping. I think it should be used?


	Will call select before recv on gamespy port udp
	Lightweight once a frame
	Uses socket : gs_select_sock that already sent queries out, waiting on response
*/
void nonBlockingServerResponses(void)
{
	unsigned char buffer[1400];
	struct sockaddr_in in_addr;
	socklen_t addrlen = sizeof(in_addr);
	fd_set readfds;
	struct timeval timeout;

	if ( gs_items.size() == 0 && (sv_public && !sv_public->value || !sv_public) ) {
		// no items to process and not public
		return;
	}

	// gs_items.size > 0 || public->value

	// Set up the file descriptor set and timeout values for select
	FD_ZERO(&readfds);
	FD_SET(gs_select_sock, &readfds);
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	// Call select to wait for data to become available on the socket
	select(gs_select_sock + 1, &readfds, NULL, NULL, &timeout);


	// Check if data is available to read
	while (FD_ISSET(gs_select_sock, &readfds)) {
		// Data is available, so receive it and process it
		int recv_len = recvfrom(gs_select_sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&in_addr, &addrlen);
		if (recv_len > 0) {
			if (!handleGamespyQuery(buffer, recv_len, in_addr)) {
			std::string secureChallenge;
			bool secureQuery = sv_public && sv_public->value &&
				readGamespySecureChallenge(buffer, recv_len, secureChallenge);

			// Stock SoF answers a secure challenge separately, then answers
			// the following status query with an ordinary status packet.
			if (sv_public && sv_public->value && secureQuery) {
				std::vector<uint8_t> key = ValidateKey("iVn3a3", secureChallenge);
				std::string response = "\\validate\\" +
					std::string(key.begin(), key.end()) +
					"\\final\\\\queryid\\" +
					std::to_string(++gamespy_query_id) + ".1";
				if (sendto(gs_select_sock, response.data(), response.size(), 0,
				           (struct sockaddr *)&in_addr, sizeof(in_addr)) < 0)
					error_exit("sendto() failed responding to secure query");
			} else if (sv_public && sv_public->value && recv_len >= 8 &&
			           !strncmp((char *)buffer, "\\status\\", 8)) {
				// received every 30 seconds... (if info completed)
				// SOFPPNIX_DEBUG("Got a \\status\\ query from %s:%d\n", inet_ntoa(in_addr.sin_addr), ntohs(in_addr.sin_port));
				// TODO: send back a response
				// == BASIC + INFO + RULES + PLAYERS

				const char* gamename = "sofretail";
				const char* gamever = "1.07f";
				int location = 1;
				const char* in_hostname = hostname->string;
				int in_hostport = serverBoundPort();
				const char* in_mapname = mapname->string;
				const char* gametype = "Unknown";
				switch ( std::lround(deathmatch->value) ) {
					case 1: gametype = "DM"; break;
					case 2: gametype = "Assassin"; break;
					case 3: gametype = "Arsenal"; break;
					case 4: gametype = "CTF"; break;
					case 5: gametype = "Realistic"; break;
					case 6: gametype = "Control"; break;
					case 7: gametype = "CTB"; break;
				}
				int numplayers = countPlayersInGame();
				int maxplayers = c_string_to_int(maxclients->string);
				const char* gamemode = "openplaying";
				int violence = c_string_to_int(sv_violence->string);
				int in_timelimit = c_string_to_int(timelimit->string);
				int in_fraglimit = c_string_to_int(fraglimit->string);
				int in_dmflags = c_string_to_int(dmflags->string);
				std::string movescale = g_movescale->value == 1.0f
					? "100%" : std::to_string((int)(g_movescale->value * 100)) + "%";

				const char* in_cheats = cheats->value == 1.0f ? "enabled" : "disabled";
				int in_ctf_loops = std::lround(ctf_loops->value);
				int suicide_penalty = std::lround(sv_suicidepenalty->value);


				char queryid[64];
				snprintf(queryid, sizeof(queryid), "%d.1", ++gamespy_query_id);
				// static const char * queryid = "3.1";

				std::string player_data;
				for (int i = 0; i < numplayers; i++) {
					// NAME
					std::string player_num = "player_" + std::to_string(i);
					std::string name = "John Mullinsqqu";

					// FRAGS
					std::string frags_num = "frags_" + std::to_string(i);
					// TODO : GET FRAGS PER PLAYER
					int frags_val = 0;

					// PING
					std::string ping_num = "ping_" + std::to_string(i);
					// TODO : GET PING PER PLAYER
					int ping_val = 38;

					// SKIN
					std::string skin_num = "skin_" + std::to_string(i);
					// TODO : GET SKIN
					std::string skin_val = "dekker";

					// TEAMNAME
					std::string team_num = "team_" + std::to_string(i);
					// TODO : GET TEAMNAME
					std::string team_val = "The Order";

					// Append player data to string
					player_data += player_num + "\\" + name + "\\" + frags_num + "\\" + std::to_string(frags_val) + "\\" + ping_num + "\\" + std::to_string(ping_val) + "\\" + skin_num + "\\" + skin_val + "\\" + team_num + "\\" + team_val;

					// Add delimiter if there are more players
					if (i < numplayers - 1) {
						player_data += "|";
					}
				}

				// std::cout << "Player data: " << player_data << std::endl;
				
				// Generate the response string using snprintf
				std::string player_suffix =
					player_data.empty() ? "" : player_data + "\\";
				char response[1024];
				snprintf(response, sizeof(response), "\\gamename\\%s\\"
												"gamever\\%s\\"
												"location\\%d\\"
												"hostname\\%s\\"
												"hostport\\%d\\"
												"mapname\\%s\\"
												"gametype\\%s\\"
												"numplayers\\%d\\"
												"maxplayers\\%d\\"
												"gamemode\\%s\\"
												"violence\\%d\\"
												"timelimit\\%d\\"
												"fraglimit\\%d\\"
												"dmflags\\%d\\"
												"movescale\\%s\\"
												"cheats\\%s\\"
												"ctf_loops\\%d\\"
												"suicide_penalty\\%d\\"
												"%s"
												"final\\\\queryid\\%s",
					gamename, gamever, location, in_hostname, in_hostport, in_mapname, gametype,
					numplayers, maxplayers, gamemode, violence, in_timelimit, in_fraglimit,
					in_dmflags, movescale.c_str(), in_cheats, in_ctf_loops,
					suicide_penalty, player_suffix.c_str(), queryid);
				
				// player_0\John Mullinsqqu\frags_0\0\ping_0\38\skin_0\dekker\team_0\The Order
				// std::cout << response << std::endl;

				std::string responseString(response);
				if ( sendto(gs_select_sock, responseString.data(), responseString.size(),
				            0, (struct sockaddr *)&in_addr, sizeof(in_addr)) < 0 ) {
					error_exit("sendto() failed responding to \\info\\ query try public 0 if continued error");
				}
				
			} else if ( sv_public && sv_public->value && recv_len >= 6 &&
			            !strncmp((char*)buffer, "\\info\\", 6) ) {
				// received every 5 minutes... (if on the radar)
				// SOFPPNIX_DEBUG("Got a \\info\\ query from %s:%d\n", inet_ntoa(in_addr.sin_addr), ntohs(in_addr.sin_port));
				// prepare data to send back as response

				// \hostname\Example\hostport\8080\mapname\dm/suddm2\gametype\DM\numplayers\0\maxplayers\16\gamemode\openplaying\violence\0\final\\queryid\6.1

				const char* in_hostname = hostname->string;
				int in_hostport = serverBoundPort();
				const char* in_mapname = mapname->string;
				const char* gametype = "Unknown";
				
				switch ( std::lround(deathmatch->value) ) {
					case 1: gametype = "DM"; break;
					case 2: gametype = "Assassin"; break;
					case 3: gametype = "Arsenal"; break;
					case 4: gametype = "CTF"; break;
					case 5: gametype = "Realistic"; break;
					case 6: gametype = "Control"; break;
					case 7: gametype = "CTB"; break;
				}

				int numplayers = countPlayersInGame();
				int maxplayers = c_string_to_int(maxclients->string);
				const char* gamemode = "openplaying";
				int violence = c_string_to_int(sv_violence->string);

				// const char* queryid = "3.1";
				char queryid[64];
				snprintf(queryid, sizeof(queryid), "%d.1", ++gamespy_query_id);
				// Generate the response string using snprintf
				char response[1024];
				snprintf(response, sizeof(response), "\\hostname\\%s\\"
												"hostport\\%d\\"
												"mapname\\%s\\"
												"gametype\\%s\\"
												"numplayers\\%d\\"
												"maxplayers\\%d\\"
												"gamemode\\%s\\"
												"violence\\%d\\"
												"final\\\\queryid\\%s",
					in_hostname, in_hostport, in_mapname, gametype,
					numplayers, maxplayers, gamemode, violence, queryid);

				// std::cout << response << std::endl;

				// Send the response back to the in_addr that sent the query
				if ( sendto(gs_select_sock, response, strlen(response), 0,
				            (struct sockaddr *)&in_addr, sizeof(in_addr)) < 0 ) {
					error_exit("sendto() failed responding to \\info\\ query try public 0 if continued error");
				}
			} else {
				// Its not a \\status\\ or \\info\\ query
				// Its another server's response to our outgoing \\status\\ request.
				// OBJECTIVE: get the hostport of the server(currently only have the gs port)
				// : Then query them on hostport with 'status'
				// Did the message come from one of the servers that we sent a \\status\\ query to earlier?
				for (int i = 0; i < gs_items.size();) {
					struct sockaddr_in gs_item_info;
					netadr_t * na = &gs_items[i].addr;
					NetadrToSockadr(na,&gs_item_info);
					if ( in_addr.sin_port == gs_item_info.sin_port && in_addr.sin_addr.s_addr == gs_item_info.sin_addr.s_addr ) {
						// Process the message if it has '\final\' in it
						if (strstr((char*)buffer, "\\final\\")) {
							// SOFPPNIX_DEBUG("Processed a server! %.*s\n", recv_len, buffer);
							// hexdump(buffer,buffer+recv_len);

							// Get the string excluding the '\\final\\'
							std::string in_status((char*)buffer,recv_len);
							std::string::size_type final_pos = in_status.find("\\final\\");
							std::string before_final = in_status.substr(0, final_pos);
							std::unordered_map<std::string, std::string> dictionary;
							std::string::size_type pos = 0;

							if (before_final[pos] == '\\') {
								pos++;
							}
							while (pos < before_final.length()) {
								// Convert the slash separated string into a key-value pair
								std::string::size_type next_pos = before_final.find('\\', pos);
								if (next_pos == std::string::npos || next_pos == before_final.length() - 1) {
									// The input string does not contain a proper key-value pair format
									break;
								}
								std::string key = before_final.substr(pos, next_pos - pos);
								pos = next_pos + 1;
								next_pos = before_final.find('\\', pos);
								if (next_pos == std::string::npos) {
									// The input string does not contain a proper key-value pair format
									break;
								}
								std::string value = before_final.substr(pos, next_pos - pos);
								pos = next_pos + 1;
								if (!key.empty() && !value.empty()) {
									dictionary[key] = value;
								}
							}

							// Print the contents of the dictionary
							// for (const auto& kv : dictionary) {
							// 	std::cout << kv.first << " = " << kv.second << std::endl;
							// }

							// SOF+hostname+mapname+num_players/NON_PLAYER_COUNT/max_players+$GAME_CVAR+$str(deathmatch)+sv_violence
							// SOF+CGZA - SoF Deathmatch+dm/suddm1+0/10/16++
							// In linux, the game $GAME cvar is used as GAMETYPE
							int dm = 0;
							if ( dictionary["gametype"].compare("DM") == 0 ) dm = 1;
							else if ( dictionary["gametype"].compare("Assassin") == 0 ) dm = 2;
							else if ( dictionary["gametype"].compare("Arsenal") == 0 ) dm = 3;
							else if ( dictionary["gametype"].compare("CTF") == 0 ) dm = 4;
							else if ( dictionary["gametype"].compare("Realistic") == 0 ) dm = 5;
							else if ( dictionary["gametype"].compare("Control") == 0 ) dm = 6;
							else if ( dictionary["gametype"].compare("CTB") == 0 ) dm = 7;

							char formedData[256];
							snprintf(formedData, 256, "%s+%s+%s/0/%s+%s+%i+%s",
									dictionary["hostname"].c_str(),
									dictionary["mapname"].c_str(),
									dictionary["numplayers"].c_str(),
									dictionary["maxplayers"].c_str(),
									dictionary["gametype"].c_str(),
									dm,
									dictionary["violence"].c_str());

							gs_items[i].addr.port = std::stoi(dictionary["hostport"]);
							// It will call menu_addserver when the connect packet returns...
							// orig_menu_AddServer(gs_items[i].addr,formedData);
							snprintf(formedData,256,"serverstatus %i.%i.%i.%i:%i\n",
								gs_items[i].addr.ip[0],
								gs_items[i].addr.ip[1],
								gs_items[i].addr.ip[2],
								gs_items[i].addr.ip[3],
								gs_items[i].addr.port);
							// std::cout << formedData << std::endl;
							orig_Cmd_ExecuteString(formedData);

							// Job done hostport has been received, clean-up
							gs_items.erase(gs_items.begin() + i);

							// The message can only match one server:port :)
							// breaks out of for loop
						}
						break;
					}
					i++;
				}// for gs_items
			}// not a status request
			}
		} // data recv
			
			
		// Call select again to wait for more data to become available on the socket
		FD_ZERO(&readfds);
		FD_SET(gs_select_sock, &readfds);
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		select(gs_select_sock + 1, &readfds, NULL, NULL, &timeout);
	}//while

	// resend or give up

	auto now = std::chrono::steady_clock::now();
	for (int i = 0; i < gs_items.size();) {
		gs_item_t * item = &gs_items[i];
		auto elapsed_ms_total = std::chrono::duration_cast<std::chrono::milliseconds>(now - item->queryTime).count();
		if ( elapsed_ms_total > 5000 ) {
			// std::cout << "removing item" << std::endl;
			// erase element from gs_items
			gs_items.erase(gs_items.begin() + i);
			continue;
		}
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - item->lastSentTime).count();
		if ( elapsed_ms > 500 ) {
			// std:cout << "resending!!" << std::endl;
			// Query the server
			struct sockaddr_in gs_server_info;
			netadr_t * na = &(item->addr);
			NetadrToSockadr(na,&gs_server_info);
			// ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
			if (sendto(gs_select_sock, "\\status\\", 8, 0,(struct sockaddr*)&gs_server_info,sizeof(gs_server_info)) < 0) {
				std::cerr << "Error: Failed to send response: " << std::strerror(errno) << std::endl;
				error_exit("Failed to send response.\n");
			}

			item->lastSentTime = now;
		}
		i++;
	}

	// SOFPPNIX_DEBUG("Outside\n");
}

void GamespyHeartbeat(void)
{
	if ( !sv_public || !sv_public->value || !gamespyport ||
	     gamespyBoundPort() <= 0 ) return;
	if ( std::chrono::steady_clock::now() < next_gamespy_heartbeat ) return;
	sendGamespyHeartbeat();
}

/*
	Called by Game SpawnEntities, when level changes, hostport and gamespyport and sent to the master
	Operates on both server UDP sockets.
*/
void GamespyHeartbeatCtrlNotify(void)
{
	int game_port = gamespyBoundPort();
	int host_port = serverBoundPort();
	if ( !sv_public || !sv_public->value || game_port <= 0 || host_port <= 0 ||
	     !_sp_sv_master_sofplus || !_sp_sv_master_sofplus->string ||
	     !*_sp_sv_master_sofplus->string ) return;

	// SOFPPNIX_DEBUG("Sending out GamespyHeartbeat\n");
	std::string master = _sp_sv_master_sofplus->string;
	if (master.find(':') == std::string::npos)
		master += ":" GAMESPY_MASTER_PORT;
	if (!orig_NET_StringToAdr(&master[0], &sof1svmaster_ip) ||
	    !(sof1svmaster_ip.ip[0] || sof1svmaster_ip.ip[1] ||
	      sof1svmaster_ip.ip[2] || sof1svmaster_ip.ip[3]))
		return;

	//fill in the gamespyport and hostport
	char formedData[256];
	snprintf(formedData, sizeof(formedData),
	         "\\hostport\\%d\\gamespyport\\%d\\", host_port, game_port);
	sendCtrlGamespyPacket(formedData);

	// now the hostport variant Ensure send using hostport socket
	snprintf(formedData, sizeof(formedData),
	         "\\gamespyport\\%d\\hostport\\%d\\", game_port, host_port);
	sendCtrlHostPacket(formedData);
	sendGamespyHeartbeat();
}
