#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "TCPServer.hpp"
#include <stdio.h>

TCPServer::TCPServer(const std::string serverIp, int portNumber) {
  m_listeningSocket = 0;
  m_listening = false;
  m_ip = serverIp;
  m_port = portNumber;
}

TCPServer::~TCPServer() {
  if (m_listeningSocket > 0) {
    close(m_listeningSocket);
  }
}

bool TCPServer::start() {
  if (m_listening == true) {
    return false;
  }

  struct sockaddr_in socketAddress;
  socketAddress.sin_family = AF_INET;
  socketAddress.sin_port = htons(m_port);
  inet_pton(AF_INET, m_ip.c_str(), &socketAddress.sin_addr);
  memset(socketAddress.sin_zero, 0, sizeof(socketAddress.sin_zero));

  m_listeningSocket = socket(PF_INET, SOCK_STREAM, 0);
  int bindStatus = bind(m_listeningSocket, (struct sockaddr*)&socketAddress, sizeof(socketAddress));
  if (bindStatus != 0) {
    printf("Unable to bind server socket.\n");
    return false;
  }
  
  int listenStatus = listen(m_listeningSocket, CONN_WAIT_LIST_SIZE);
  if (listenStatus != 0) {
    printf("Unable to listen to socket.\n");
    return false;
  }
  
  m_listening = true;
  return true;
}

TCPConnection* TCPServer::accept() {
  if (m_listening == false) {
    return NULL;
  }

  struct sockaddr_in socketAddress;
  socklen_t size = sizeof(socketAddress);
  memset(&socketAddress, 0, sizeof(socketAddress));
  int handleSocketId = ::accept(m_listeningSocket, (struct sockaddr*) &socketAddress, &size);

  if (handleSocketId < 0) {
    printf("Unable to create a socket to handle connection!\n");
    return NULL;
  }

  char ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &socketAddress.sin_addr, ip, sizeof(ip));

  return new TCPConnection(handleSocketId, std::string(ip), ntohs(socketAddress.sin_port));
}

TCPConnection* TCPServer::accept(int milliTimeout) {
  if (m_listening == false) {
    return NULL;
  }

  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = milliTimeout * 1000;
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(m_listeningSocket, &readfds);

  if (select(m_listeningSocket+1, &readfds, NULL, NULL, &tv) > 0) {
    struct sockaddr_in socketAddress;
    socklen_t size = sizeof(socketAddress);
    memset(&socketAddress, 0, sizeof(socketAddress));
    int handleSocketId = ::accept(m_listeningSocket, (struct sockaddr*) &socketAddress, &size);

    if (handleSocketId < 0) {
      printf("Unable to create a socket to handle connection!\n");
      return NULL;
    }

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &socketAddress.sin_addr, ip, sizeof(ip));

    return new TCPConnection(handleSocketId, std::string(ip), ntohs(socketAddress.sin_port));
  }
  return NULL;
}

std::string TCPServer::ip() {
  return m_ip;
}

int TCPServer::port() {
  return m_port;
}