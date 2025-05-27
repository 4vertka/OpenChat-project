#include "socket_utils.h"
#include <string>
#include <sys/socket.h>

int main() {

  socket_utils sock_u;

  struct addrinfo *server_info =
      sock_u.create_address("127.0.0.1", sock_u.port.c_str());

  int client_socket = sock_u.create_socket(server_info);

  int conn =
      connect(client_socket, server_info->ai_addr, server_info->ai_addrlen);

  if (conn == 0)
    std::cout << "success\n";

  std::string buffer_send;

  while (true) {
    std::getline(std::cin, buffer_send);

    if (buffer_send == "exit")
      break;

    int client_send =
        send(client_socket, buffer_send.c_str(), buffer_send.length(), 0);
  }

  close(client_socket);
  shutdown(client_socket, SHUT_RDWR);
}
