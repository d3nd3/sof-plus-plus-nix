#include "common.h"

#include "decodekey.h"



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
netadr_t sof1master_ip;
// a gs_item struct that contains a netadr_t and a boolean hinting if status received
typedef struct gs_item_s {
	netadr_t addr;

std::chrono::steady_clock::time_point queryTime;
std::chrono::steady_clock::time_point lastSentTime;
} gs_item_t;


// a std vector of gs_items
std::vector<gs_item_t> gs_items;


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
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		SOFPPNIX_PRINT("Error: Failed to create socket.\n");
		return 1;
	}

	// int timeout = 2000; // user timeout in milliseconds [ms]
	// setsockopt(sockfd, SOL_TCP, TCP_USER_TIMEOUT, (char*) &timeout, sizeof(timeout));

	// Set the maximum segment size for outgoing TCP packets
	// int maxseg = 1400;
	// if (setsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &maxseg, sizeof(maxseg)) < 0) {
	//     error_exit("Failed to set maximum segment size.\n");
	// }

	struct addrinfo hints;
	struct addrinfo *res;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	int status = getaddrinfo("sof1master.megalag.org", NULL, &hints, &res);
	if (status != 0) {
		error_exit("Failed to resolve hostname.\n");
	}
	struct sockaddr_in server_addr;
	std::memcpy(&server_addr, res->ai_addr, res->ai_addrlen);
	server_addr.sin_port = htons(28900);
	freeaddrinfo(res);
	if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		error_exit("Failed to connect to server.\n");
	}


	const char* message = "IDQD";
	if (send(sockfd, message, std::strlen(message), 0) < 0) {
		error_exit("Failed to send message.\n");
	}

	unsigned char buffer[1400];
	std::memset(buffer, 0, sizeof(buffer));
	int bytes_returned = recv(sockfd, buffer, sizeof(buffer), 0);
	if (bytes_returned < 0) {
		error_exit("Failed to receive message.\n");
	}

	std::string received_message((char*)buffer,bytes_returned);

	// Split the message string by the backslash character '\'
	std::vector<std::string> parts;
	std::istringstream iss(received_message);
	std::string part;
	while (std::getline(iss, part, '\\')) {
		parts.push_back(part);
	}

	std::string realkey = parts[parts.size() - 1];

	// This is disabled I think. But do it anyway.
	// Hash the key using the provided hash function
	// char * ckey = gsseckey(NULL,key.c_str(),"iVn3a3",2);
	std::vector<uint8_t> key = ValidateKey(std::string("iVn3a3"),realkey);
	std::string hashed_key( key.begin(), key.end() );

	std::string response = "\\gamename\\sofretail\\gamever\\1.6\\location\\0\\validate\\" + std::string("ABCDEFGH") + "\\final\\\\queryid\\1.1\\";
	char * c = response.c_str();
	// ssize_t send(int sockfd, const void *buf, size_t len, int flags);
	if (send(sockfd, c, response.size(), 0) < 0) {
		error_exit("Failed to send response.\n");
	}

	response = "\\list\\cmp\\gamename\\sofretail\\final\\";
	c = response.c_str();
	if (send(sockfd, c, response.size(), 0) < 0) {
		error_exit("Failed to send response.\n");
	}
	std::memset(buffer, 0, sizeof(buffer));
	
	int r = 0;
	bytes_returned = 0;
	while ( true ) {
		// ssize_t recv(int sockfd, void *buf, size_t len, int flags);
		r = recv(sockfd, buffer + bytes_returned, sizeof(buffer) - bytes_returned, 0);
		if ( r < 0) {
			error_exit("Failed to receive message.\n");
		} else {
			if ( r == 0 )
				break;
		}
		bytes_returned += r;
	}
	close(sockfd);
	// there is a \final\ at the end. So ignore last 7 bytes
	// hexdump(buffer,buffer+bytes_returned);

	// Ignore the first 7 bytes
	unsigned char* data = buffer;
	if (bytes_returned <= 7 || (bytes_returned-7) % 6 != 0 ) {
		error_exit("Failure getting Server List , bytes returned not correct\n");
	}
	auto now = std::chrono::steady_clock::now();
	// Parse the buffer and create gamespy structs that contain netadr_t
	// They are global and will be dealt with each frame
	for (int i = 0; i < bytes_returned - 7; i += 6) {

		gs_item_t unacked_server;

		netadr_t addr;
		addr.type = NA_IP;
		addr.ip[0] = data[i];
		addr.ip[1] = data[i + 1];
		addr.ip[2] = data[i + 2];
		addr.ip[3] = data[i + 3];
		addr.port = (data[i + 5] << 8) | data[i + 4];

		unacked_server.addr = addr;

		unacked_server.queryTime = now;
		unacked_server.lastSentTime = now;

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

	if ( gs_items.size() == 0 && !sv_public->value ) {
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

			// OBJECTIVE: pass any '\\status\\' queries if we are server /w public 1
			if ( !strncmp((char*)buffer, "\\status\\", 8) ) {
				if ( sv_public->value  ) {
					SOFPPNIX_DEBUG("Got a \\status\\ query from %s:%d\n", inet_ntoa(in_addr.sin_addr), ntohs(in_addr.sin_port));
					// TODO: send back a response
					// == BASIC + INFO + RULES + PLAYERS
				}
			} else if ( !strncmp((char*)buffer, "\\info\\", 6) ) {
				if ( sv_public->value  ) {
					SOFPPNIX_DEBUG("Got a \\info\\ query from %s:%d\n", inet_ntoa(in_addr.sin_addr), ntohs(in_addr.sin_port));
					// prepare data to send back as response
				}
			} else {
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

/*
This is called inside SV_Frame. So only server calls it. Periodic heartbeat
I don't think its used in windows(sofplus).
But hook is in place, to silence the WON heartbeat code.(crashes). could nop.
*/
void GamespyHeartbeat(void)
{

	// \\heartbeat\\%d\\gamename\\%s
	std::cout << "Not yet implemented heartbeat" << std::endl;
}

/*
	Called by Game SpawnEntities, when level changes, hostport and gamespyport and sent to the master
	Operates on gamespyport UDP
*/
void GamespyHeartbeatCtrlNotify(void)
{
	/*
		if ( sv_public->value == 1.0f ) {
			bool isClientHere = false;
			void * svs_clients = *(unsigned int*)(*(unsigned int*)(0x0829D134) + 0x10C);
			for ( int i = 0 ; i < maxclients->value;i++ ) {
				void * client_t = svs_clients + i * 0xd2ac;
				int state = *(int*)(client_t);
				if (state != cs_spawned )
					continue;
				isClientHere = true;
				break;
			}

			if (isClientHere) {
				SOFPPNIX_DEBUG("Client is here\n");


			}
		}
	*/
	SOFPPNIX_DEBUG("Sending out GamespyHeartbeat\n");
	
	//fill in the gamespyport and hostport
	char formedData[256];
	snprintf(formedData, 256, "\\gamespyport\\%s\\hostport\\%s\\", gamespyport->string, hostport->string);

	// netadr_t to sockaddr_in
	struct sockaddr_in gs_server_info;
	NetadrToSockadr(&sof1master_ip,&gs_server_info);

	std::cout << "gamespy " << gs_select_sock << " ?" << std::endl;
	// sendto gs_server_info on gs_select_sock, Ensure send using gamespy socket
	if (sendto(gs_select_sock, formedData, strlen(formedData), 0,(struct sockaddr*)&gs_server_info,sizeof(gs_server_info)) < 0) {
		error_exit("Failed to send response. ErrMsg = %s\n",std::strerror(errno));
	}

	std::cout << "hostport" << std::endl;
	// now the hostport variant Ensure send using hostport socket
	snprintf(formedData, 256, "\\hostport\\%s\\gamespyport\\%s\\", hostport->string,gamespyport->string);
	// netsrc_t sock, int length, void *data, netadr_t to
	orig_NET_SendPacket(NS_SERVER, strlen(formedData), formedData, sof1master_ip);
	
}