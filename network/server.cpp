#include "socket_utils.h"
#include <asm-generic/socket.h>
#include <csetjmp>
#include <cstdio>
#include <cstdlib>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <vector>

struct accepted_socket {
  int accepted_socket_fd;
  struct sockaddr *address;
  int error;
  bool accepted_successfull;
};

struct accepted_socket *client_handle_thread(int server_socket);
void receive_data_client(int socket_fd);
void start_accepting_connections(int server_socket_fd);
void receive_on_separate_thread(int client_socket_fd);

int main() {
  socket_utils sock_u;
  // struct sockaddr_storage their_addr;
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

  start_accepting_connections(server_socket_fd);

  shutdown(server_socket_fd, SHUT_RDWR);
}

struct accepted_socket *client_handle_thread(int server_socket) {

  struct sockaddr_storage their_storage;

  socklen_t their_storage_size = sizeof(their_storage);
  int server_socket_accept = accept(
      server_socket, (struct sockaddr *)&their_storage, &their_storage_size);

  struct accepted_socket *accepted_socket_struct =
      (accepted_socket *)std::malloc(sizeof(accepted_socket));

  accepted_socket_struct->address = (struct sockaddr *)&their_storage;
  accepted_socket_struct->accepted_socket_fd = server_socket_accept;
  accepted_socket_struct->accepted_successfull = server_socket_accept;

  if (!accepted_socket_struct) {
    accepted_socket_struct->error = server_socket_accept;
  }

  return accepted_socket_struct;
}

void receive_data_client(int socket_fd) {

  std::vector<char> buffer_receive(1024);

  while (true) {
    std::string message{""};
    int server_recv =
        recv(socket_fd, &buffer_receive[0], buffer_receive.size(), 0);
    if (server_recv > 0) {
      message.append(buffer_receive.cbegin(),
                     buffer_receive.cend() + server_recv);
      std::cout << "data received: " << message << '\n';
    } else if (server_recv == 0) {
      std::cout << "connection closed\n";
      break;
    } else {
      std::cerr << "recv failed\n";
      break;
    }
  }

  close(socket_fd);
}

void start_accepting_connections(int server_socket_fd) {

  while (true) {
    struct accepted_socket *accepted_socket_client =
        client_handle_thread(server_socket_fd);
    receive_on_separate_thread(accepted_socket_client->accepted_socket_fd);
  }
}

void receive_on_separate_thread(int client_socket_fd) {

  std::thread(receive_data_client, client_socket_fd).detach();
}
