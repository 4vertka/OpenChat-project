#include "socket_utils.h"
#include <cstring>

struct addrinfo *socket_utils::create_address(const char *ip,
                                              const char *port) {

  struct addrinfo hints, *server_info;
  std::memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int get_addr_info = getaddrinfo(ip, port, &hints, &server_info);
  return server_info;
}

int socket_utils::create_socket(struct addrinfo *server_info) {

  int socketFD = socket(server_info->ai_family, server_info->ai_socktype,
                        server_info->ai_protocol);

  return socketFD;
}
