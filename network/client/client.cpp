#include "socket_utils.h"
#include <cstddef>
#include <cstdio>
#include <string>
#include <sys/socket.h>
#include <thread>

void listen_and_print_thread(int socket_fd);
void listen_and_print(int socket_fd);

int main() {

  socket_utils sock_u;

  struct addrinfo *server_info =
      sock_u.create_address("127.0.0.1", sock_u.port.c_str());

  int client_socket = sock_u.create_socket(server_info);

  int conn =
      connect(client_socket, server_info->ai_addr, server_info->ai_addrlen);

  if (conn == 0)
    std::cout << "success\n";

  std::string client_name;
  std::cout << "enter your name\n";
  std::getline(std::cin, client_name);

  std::string buffer_send;

  listen_and_print_thread(client_socket);

  // char buffer[1024];

  std::string message_with_name;

  while (true) {
    std::getline(std::cin, buffer_send);

    message_with_name = client_name + ": " + buffer_send;

    if (buffer_send == "exit")
      break;

    int client_send = send(client_socket, message_with_name.c_str(),
                           message_with_name.length(), 0);
  }

  close(client_socket);
  shutdown(client_socket, SHUT_RDWR);
}

void listen_and_print_thread(int socket_fd) {
  std::thread(listen_and_print, socket_fd).detach();
}

void listen_and_print(int socket_fd) {

  char buffer[1024];

  while (true) {
    int server_recv = recv(socket_fd, buffer, sizeof(buffer), 0);
    if (server_recv > 0) {
      buffer[server_recv] = 0;
      std::cout << "message -> " << buffer << std::endl;

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
