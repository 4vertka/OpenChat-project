#include "socket_utils.h"
#include <asm-generic/socket.h>
#include <cstdio>
#include <cstdlib>
#include <netdb.h>
#include <sys/socket.h>

int main() {
  socket_utils sock_u;
  struct sockaddr_storage their_addr;
  int server_socket_fd;
  int yes = 1;

  struct addrinfo *server_info =
      sock_u.create_address("127.0.0.1", sock_u.port.c_str());

  for (addrinfo *addr = server_info; addr != NULL; addr = addr->ai_next) {
    server_socket_fd = sock_u.create_socket(addr);

    if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(int)) == -1) {
      std::cerr << "setsockopt failed\n";
      exit(EXIT_FAILURE);
    }

    if (bind(server_socket_fd, addr->ai_addr, addr->ai_addrlen) == -1) {
      close(server_socket_fd);
      std::cerr << "failed to bind socket\n";
      continue;
    } else {
      std::cout << "bind successfull\n";
    }

    break;
  }

  std::cout << "loop ended\n";

  freeaddrinfo(server_info);

  int server_socket_listen = listen(server_socket_fd, 10);

  socklen_t their_addr_size = sizeof(their_addr);
  int server_socket_accept = accept(
      server_socket_fd, (struct sockaddr *)&their_addr, &their_addr_size);

  char buffer_receive[1024];
  recv(server_socket_accept, buffer_receive, sizeof(buffer_receive), 0);

  std::cout << "reveived data: " << buffer_receive << '\n';

  close(server_socket_fd);
}
