#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "TCPServer.hpp"
#include <stdio.h>

TCPServer::TCPServer(const std::string serverIp, int portNumber) {
  listeningSocket = 0;
  listening = false;
  ip = serverIp;
  port = portNumber;
}

TCPServer::~TCPServer() {
  if (listeningSocket > 0) {
    close(listeningSocket);
  }
}

bool TCPServer::start() {
  if (listening == true) {
    return false;
  }

  struct sockaddr_in socketAddress;
  socketAddress.sin_family = AF_INET;
  socketAddress.sin_port = htons(port);
  inet_pton(AF_INET, ip.c_str(), &socketAddress.sin_addr);
  memset(socketAddress.sin_zero, 0, sizeof(socketAddress.sin_zero));

  listeningSocket = socket(PF_INET, SOCK_STREAM, 0);
  printf("Socket server: %d\n", listeningSocket);
  int bindStatus = bind(listeningSocket, (struct sockaddr*)&socketAddress, sizeof(socketAddress));
  if (bindStatus != 0) {
    printf("Unable to bind server socket.\n");
    return false;
  }
  
  int listenStatus = listen(listeningSocket, CONN_WAIT_LIST_SIZE);
  if (listenStatus != 0) {
    printf("Unable to listen to socket.\n");
    return false;
  }
  
  listening = true;
  return true;
}

TCPConnection* TCPServer::accept() {
  if (listening == false) {
    return NULL;
  }

  struct sockaddr_in socketAddress;
  socklen_t size = sizeof(socketAddress);
  memset(&socketAddress, 0, sizeof(socketAddress));
  int handleSocketId = ::accept(listeningSocket, (struct sockaddr*) &socketAddress, &size);

  if (handleSocketId < 0) {
    printf("Unable to create a socket to handle connection!\n");
    return NULL;
  }

  char ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &socketAddress.sin_addr, ip, sizeof(ip));

  return new TCPConnection(handleSocketId, std::string(ip), ntohs(socketAddress.sin_port));
}