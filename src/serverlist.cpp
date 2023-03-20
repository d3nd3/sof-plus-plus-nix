#include "common.h"

#include <list>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

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
*/
void GetServerList(void)
{
 
  orig_Com_Printf("GetServerList!\n");
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    orig_Com_Printf("Error: Failed to create socket.\n");
    return 1;
  }

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

  char buffer[1400];
  std::memset(buffer, 0, sizeof(buffer));
  if (recv(sockfd, buffer, sizeof(buffer), 0) < 0) {
    error("Error: Failed to receive message.\n");
  }

  std::string received_message(buffer);

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

  std::string response = std::string("\\gamename\\sofretail\\gamever\\1.6\\location\\0\\validate\\") + hashed_key + "\\final\\\\queryid\\1.1\\";
  if (send(sockfd, response.c_str(), response.length(), 0) < 0) {
    error("Error: Failed to send response.\n");
  }

  response = std::string("\\list\\cmp\\gamename\\sofretail\\final\\");
  if (send(sockfd, response.c_str(), response.length(), 0) < 0) {
    error("Error: Failed to send response.\n");
  }

  std::memset(buffer, 0, sizeof(buffer));
  int bytes_returned = recv(sockfd, buffer, sizeof(buffer), 0);
  if ( bytes_returned < 0) {
    error("Error: Failed to receive message.\n");
  }

  // orig_Com_Printf("%.*s\n", bytes_returned, buffer);
  close(sockfd);

  // there is a \final\ at the end. So ignore last 7 bytes
  orig_Com_Printf("size : %i\n",bytes_returned);
  // Ignore the first 7 bytes
  unsigned char* data = buffer;
  if (bytes_returned <= 7 || (bytes_returned-7) % 6 != 0 ) {
      error("Failure getting Server List , bytes returned not correct\n");
  }

  // Parse the buffer and create netadr_t structures
  std::vector<netadr_t> addrs;
  for (int i = 0; i < bytes_returned - 7; i += 6) {
      netadr_t addr;
      addr.ip[0] = data[i];
      addr.ip[1] = data[i + 1];
      addr.ip[2] = data[i + 2];
      addr.ip[3] = data[i + 3];
      addr.port = (data[i + 4] << 8) | data[i + 5];
      addrs.push_back(addr);
  }
  // std::cout << addrs.size() << '\n';
  // Print the netadr_t structures
  for (int i = 0; i < addrs.size(); i++) {
      std::cout << "Addr " << i << ": "
                << (int)addrs[i].ip[0] << "."
                << (int)addrs[i].ip[1] << "."
                << (int)addrs[i].ip[2] << "."
                << (int)addrs[i].ip[3] << ":"
                << addrs[i].port << std::endl;


    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      orig_Com_Printf("Error: Failed to create socket.\n");
      return 1;
    }

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

    char buffer[1400];
    std::memset(buffer, 0, sizeof(buffer));
    if (recv(sockfd, buffer, sizeof(buffer), 0) < 0) {
      error("Error: Failed to receive message.\n");
    }


    // menu_AddServer
  }

}

/*
 menu_system 0x80 size.
 Created in M_PushMenu.

 Seg fault on menu free?
*/

void my_AddServer(void * menu_system, netadr_t adr, char * serverdata)
{
    
    // orig_Com_Printf((static_cast<std::string*>(static_cast<void*>(serverdata)))->c_str());


    orig_Com_Printf("%s\n",serverdata);
    orig_Com_Printf("hexdump before\n");
    hexdump(menu_system, menu_system+0x80);
    //mapping of a string to a palette
    //map<
        //basic_string<char,string_char_traits<char>,__default_alloc_template<true,0>>,
        //paletteRGBA_c,less<basic_string<char,string_char_traits<char>,__default_alloc_template<true,0>>
        //>,__default_alloc_template<true,0>
        //>
    //>
    // NVM THAT IS RECT.
    // 0x44 = DTINT? 0x080E99FF .. specifically dtint?
    orig_Com_Printf("+0x44 == %s\n",*(char*)(menu_system+0x44));
    // 0x48 = ALIGNMENT? RIGHT/CENTER/LEFT 0x080E9A0D
    orig_Com_Printf("+0x48 == %s\n",*(char*)(menu_system+0x48));


    // segfault in menu_system destructor if this is 0
    // i think its a pointer?
    // set players?
    // std::list<string>

    // std::string * huh = new std::string("0");
   
    // *(int*)(menu_system+0x44) = huh;

    // *(int*)(menu_system+0x48) = huh;

    orig_AddServer(menu_system,adr,serverdata);

    orig_Com_Printf("hexdump after\n");
    hexdump(menu_system, menu_system+0x80);
}


void my_SendToMasters(void * arg1,void * arg2)
{
    orig_Com_Printf("Attemping to send to masters!\n");

    char * string = orig_SV_StatusString();

    // Com_Printf ("Sending heartbeat to %s\n", NET_AdrToString (master_adr[i]));
    // Netchan_OutOfBandPrint (NS_SERVER, master_adr[i], "heartbeat\n%s", string);
    
}