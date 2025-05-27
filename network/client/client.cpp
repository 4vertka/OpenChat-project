#include "socket_utils.h"

int main() {

  socket_utils sock_u;

  struct addrinfo *server_info =
      sock_u.create_address("127.0.0.1", sock_u.port.c_str());

  int socketFD = sock_u.create_socket(server_info);

  int conn = connect(socketFD, server_info->ai_addr, server_info->ai_addrlen);

  if (conn == 0)
    std::cout << "success\n";

  const char *message = "hello";
  send(socketFD, message, strlen(message), 0);

  char buffer_receive[1024];
  recv(socketFD, buffer_receive, sizeof(buffer_receive), 0);

  std::cout << "response was " << buffer_receive << '\n';
}
