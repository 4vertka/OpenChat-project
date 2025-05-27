#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#define PORT "1488"
#define BACKLOG 10

// thread that handles client connections
void handle_client_connection(int client_socket) {

  char data_buffer[256];
  while (true) {
    memset(data_buffer, 0, sizeof(data_buffer));
    int bytes_received =
        recv(client_socket, data_buffer, sizeof(data_buffer), 0);
    if (bytes_received <= 0) {
      std::cout << "client disconnected\n";
      break;
    }

    data_buffer[bytes_received] = '\0';
    std::cout << "message from client: " << data_buffer << '\n';
  }

  close(client_socket);
}

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET)
    return &((struct sockaddr_in *)sa)->sin_addr;
  return &((struct sockaddr_in6 *)sa)->sin6_addr;
}

void creating_listening_accepting() {
  struct addrinfo hints, *serverinfo, *p;
  struct sockaddr_storage their_storage;
  char data_buffer[256];
  char s[INET6_ADDRSTRLEN];
  int server_socket, client_socket;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((getaddrinfo(nullptr, PORT, &hints, &serverinfo)) != 0) {
    std::cerr << "getaddrinfo error\n";
    exit(EXIT_FAILURE);
  }

  for (p = serverinfo; p != NULL; p = p->ai_next) {
    if ((server_socket =
             socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      std::cerr << "socket() error\n";
      exit(EXIT_FAILURE);
    }
    if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1) {
      close(server_socket);
      std::cerr << "bind error\n";
      continue;
    }
    break;
  }

  freeaddrinfo(serverinfo);

  if (p == NULL) {
    std::cerr << "server failed to bind\n";
    exit(1);
  }

  if (listen(server_socket, BACKLOG) == -1) {
    std::cerr << "listen error\n";
    exit(EXIT_FAILURE);
  }
  std::cout << "server is listening\n";

  while (true) {
    socklen_t addr_size = sizeof(their_storage);
    client_socket =
        accept(server_socket, (struct sockaddr *)&their_storage, &addr_size);

    if (client_socket == -1) {
      std::cerr << "accept error\n";
      continue;
    }

    inet_ntop(their_storage.ss_family,
              get_in_addr((struct sockaddr *)&their_storage), s, sizeof(s));
    std::cout << "server : connection from " << s << '\n';

    std::thread(handle_client_connection, client_socket).detach();
  }

  close(server_socket);
}

int main() { creating_listening_accepting(); }
