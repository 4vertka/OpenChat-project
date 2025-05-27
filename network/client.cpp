#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#define PORT "1488"

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET)
    return &((struct sockaddr_in *)sa)->sin_addr;
  return &((struct sockaddr_in6 *)sa)->sin6_addr;
}

int main() {
  struct addrinfo hints, *serverinfo, *p;
  int client_socket;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo("192.168.0.105", PORT, &hints, &serverinfo) == -1) {
    std::cerr << "getaddrinfo error\n";
    exit(EXIT_FAILURE);
  }

  for (p = serverinfo; p != NULL; p = p->ai_next) {
    if ((client_socket =
             socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      std::cerr << "socket() error\n";
      continue;
    }

    if (connect(client_socket, p->ai_addr, p->ai_addrlen) == -1) {
      close(client_socket);
      std::cerr << "connect failed\n";
      exit(EXIT_FAILURE);
    }
  }
  freeaddrinfo(serverinfo);

  std::string message;
  while (true) {
    std::getline(std::cin, message);

    std::cout << "you: " << message << '\n';

    if (send(client_socket, message.c_str(), message.size(), 0) == -1) {
      std::cerr << "send() error\n";
      exit(EXIT_FAILURE);
    }
  }

  close(client_socket);
}
