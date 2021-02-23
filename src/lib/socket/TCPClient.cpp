#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "TCPClient.hpp"

TCPConnection* TCPClient::connect(const std::string ipServer, int port) {
  struct sockaddr_in socketAddress;
  socketAddress.sin_family = AF_INET;
  socketAddress.sin_port = htons(port);
  inet_pton(AF_INET, ipServer.c_str(), &socketAddress.sin_addr);
  memset(socketAddress.sin_zero, 0, sizeof(socketAddress.sin_zero));

  int socketId = socket(PF_INET, SOCK_STREAM, 0);
  int connectionStatus = ::connect(socketId, (struct sockaddr*)&socketAddress, sizeof(socketAddress));
  if (connectionStatus != 0) {
    return NULL;
  }

  return new TCPConnection(socketId, ipServer, port);
}