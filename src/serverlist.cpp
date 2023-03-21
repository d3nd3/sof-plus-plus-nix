#include "common.h"

#include <list>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/tcp.h>

#include <sys/time.h>

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
void GetServerList(void)
{
 
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    orig_Com_Printf("Error: Failed to create socket.\n");
    return 1;
  }

  // int timeout = 2000; // user timeout in milliseconds [ms]
  // setsockopt(sockfd, SOL_TCP, TCP_USER_TIMEOUT, (char*) &timeout, sizeof(timeout));

  // Set the maximum segment size for outgoing TCP packets
  // int maxseg = 1400;
  // if (setsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &maxseg, sizeof(maxseg)) < 0) {
  //     error("Error: Failed to set maximum segment size.\n");
  // }

  struct addrinfo hints;
  struct addrinfo *res;
  std::memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  int status = getaddrinfo("sof1master.megalag.org", nullptr, &hints, &res);
  if (status != 0) {
    error("Error: Failed to resolve hostname.\n");
    return 1;
  }

  struct sockaddr_in server_addr;
  std::memcpy(&server_addr, res->ai_addr, res->ai_addrlen);
  server_addr.sin_port = htons(28900);

  freeaddrinfo(res);

  if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    error("Error: Failed to connect to server.\n");
  }

  const char* message = "IDQD";
  if (send(sockfd, message, std::strlen(message), 0) < 0) {
    error("Error: Failed to send message.\n");
  }

  unsigned char buffer[1400];
  std::memset(buffer, 0, sizeof(buffer));
  int bytes_returned = recv(sockfd, buffer, sizeof(buffer), 0);
  if (bytes_returned < 0) {
    error("Error: Failed to receive message.\n");
  }

  std::string received_message((char*)buffer,bytes_returned);

  // std::cout << "MSG1: " << received_message << '\n';

  // Split the message string by the backslash character '\'
  std::vector<std::string> parts;
  std::istringstream iss(received_message);
  std::string part;
  while (std::getline(iss, part, '\\')) {
    parts.push_back(part);
  }

  // std::cout << "SIZE: " << parts.size();

  std::string realkey = parts[parts.size() - 1];

  // Hash the key using the provided hash function
  // char * ckey = gsseckey(NULL,key.c_str(),"iVn3a3",2);
  std::vector<uint8_t> key = ValidateKey(std::string("iVn3a3"),realkey);
  std::string hashed_key( key.begin(), key.end() );

  // std::cout << "HASHED_KEY: " << hashed_key << '\n';

  std::string response = "\\gamename\\sofretail\\gamever\\1.6\\location\\0\\validate\\" + std::string("ABCDEFGH") + "\\final\\\\queryid\\1.1\\";
  char * c = response.c_str();
  // ssize_t send(int sockfd, const void *buf, size_t len, int flags);
  if (send(sockfd, c, response.size(), 0) < 0) {
    error("Error: Failed to send response.\n");
  }

  response = "\\list\\cmp\\gamename\\sofretail\\final\\";
  c = response.c_str();
  if (send(sockfd, c, response.size(), 0) < 0) {
    error("Error: Failed to send response.\n");
  }
  std::memset(buffer, 0, sizeof(buffer));
  // ssize_t recv(int sockfd, void *buf, size_t len, int flags);

  int r = 0;
  bytes_returned = 0;
  while ( true ) {
    r = recv(sockfd, buffer + bytes_returned, sizeof(buffer) - bytes_returned, 0);
    if ( r < 0) {
      error("Error: Failed to receive message.\n");
    } else {
      if ( r == 0 )
        break;
    }
    bytes_returned += r;
  }
  close(sockfd);
  // orig_Com_Printf("%.*s\n", bytes_returned, buffer);
  
  // there is a \final\ at the end. So ignore last 7 bytes

  // hexdump(buffer,buffer+bytes_returned);
  // Ignore the first 7 bytes
  unsigned char* data = buffer;
  if (bytes_returned <= 7 || (bytes_returned-7) % 6 != 0 ) {
      error("Failure getting Server List , bytes returned not correct\n");
  }
  // Parse the buffer and create netadr_t structures
  std::vector<netadr_t> addrs;
  for (int i = 0; i < bytes_returned - 7; i += 6) {
      netadr_t addr;
      addr.type = NA_IP;
      addr.ip[0] = data[i];
      addr.ip[1] = data[i + 1];
      addr.ip[2] = data[i + 2];
      addr.ip[3] = data[i + 3];
      addr.port = (data[i + 5] << 8) | data[i + 4];
      addrs.push_back(addr);
  }
  // --------------UDP MODE----------------
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    orig_Com_Printf("Error: Failed to create socket.\n");
    return 1;
  }

  struct timeval timeout = {1, 200000}; // set timeout for 2 seconds

  // set receive UDP message timeout
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));

  for (auto it = addrs.begin(); it != addrs.end();) {
      std::cout << "Addr " << ": "
                << (int)it->ip[0] << "."
                << (int)it->ip[1] << "."
                << (int)it->ip[2] << "."
                << (int)it->ip[3] << ":"
                << htons(it->port) << std::endl;

      struct sockaddr_in gs_server_info;
      NetadrToSockadr(&(*it),&gs_server_info);

      // send twice just in case?
      if (sendto(sockfd, "\\info\\", 6, 0,(struct sockaddr*)&gs_server_info,sizeof(gs_server_info)) < 0) {
        std::cerr << "Error: Failed to send response: " << std::strerror(errno) << std::endl;
        error("Error: Failed to send response.\n");
      }

      if (sendto(sockfd, "\\info\\", 6, 0,(struct sockaddr*)&gs_server_info,sizeof(gs_server_info)) < 0) {
        std::cerr << "Error: Failed to send response: " << std::strerror(errno) << std::endl;
        error("Error: Failed to send response.\n");
      }

      if (sendto(sockfd, "\\info\\", 6, 0,(struct sockaddr*)&gs_server_info,sizeof(gs_server_info)) < 0) {
        std::cerr << "Error: Failed to send response: " << std::strerror(errno) << std::endl;
        error("Error: Failed to send response.\n");
      }

      if (sendto(sockfd, "\\info\\", 6, 0,(struct sockaddr*)&gs_server_info,sizeof(gs_server_info)) < 0) {
        std::cerr << "Error: Failed to send response: " << std::strerror(errno) << std::endl;
        error("Error: Failed to send response.\n");
      }

      std::memset(buffer, 0, sizeof(buffer));
      bytes_returned = 0;
      struct sockaddr_in client_addr;
      socklen_t client_addr_len = sizeof(client_addr);
      bool gonext = false;
      while ( true ) {
        r = recvfrom(sockfd, buffer, 1400, 0,(struct sockaddr*)&client_addr,&client_addr_len);
        if ( r < 0 && errno == EAGAIN || errno == EWOULDBLOCK ) {
            gonext = true;
            it = addrs.erase(it);
            break;
        }
        // Not the correct sender. this is throwing packets away
        if (client_addr.sin_family == AF_INET &&
               client_addr.sin_port != gs_server_info.sin_port ||
               client_addr.sin_addr.s_addr != gs_server_info.sin_addr.s_addr ) {
          continue;
        }
      
        if ( r < 0) {
            std::cerr << std::strerror(errno) << std::endl;
            error("Error: Failed to receive message.\n");
        }
        bytes_returned += r;
        // GOT DATA: go Process
        break;
      }
      if ( gonext ) {
        ++it;
        continue;
      }
      
      /*
        Process the response (just trying to get their hostport?)
      */
      std::string input((char*)buffer,bytes_returned);
      std::istringstream ss(input);
      std::string token;
      int hostport = 0;
      gonext=false;
      while (std::getline(ss, token, '\\')) {
          if (token == "hostport") {
              std::getline(ss, token, '\\');
              hostport = std::stoi(token);
              it->port = htons(hostport);
              gonext=true;
              break;
          }
      }

      if ( gonext ) {
        ++it;
        continue;
      }
      
      it = addrs.erase(it);
  }
  close(sockfd);
  for (auto it = addrs.begin(); it != addrs.end(); ++it) {
    char * request = orig_va("info %i",33);
    orig_Netchan_OutOfBandPrint(NS_CLIENT,*it,request);
  }
}

/*
 menu_system 0x80 size.
 Created in M_PushMenu.

 Seg fault on menu free?
*/
bool isNumberLessThan100(const std::string& input) {
    try {
        int num = std::stoi(input);
        return num < 100;
    } catch (const std::invalid_argument& e) {
        return false;
    }
}
void my_menu_AddServer(netadr_t addr,char *data)
{
  
    // *(char*)(data + strlen(data) + 1 - 8) = '\n';
    *(char*)(data + strlen(data) + 1 - 8) = 0x00;
    
    // orig_Com_Printf("BOO: %s\n",data);
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


void my_SendToMasters(void * arg1,void * arg2)
{
    orig_Com_Printf("Attemping to send to masters!\n");

    char * string = orig_SV_StatusString();

    // Com_Printf ("Sending heartbeat to %s\n", NET_AdrToString (master_adr[i]));
    // Netchan_OutOfBandPrint (NS_SERVER, master_adr[i], "heartbeat\n%s", string);
    
}

