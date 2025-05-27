#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

class socket_utils {
public:
  struct addrinfo *create_address(const char *ip, const char *port);
  int create_socket(struct addrinfo *server_info);

  std::string port = "1488";
};
