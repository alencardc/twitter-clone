#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "UDPSocket.hpp"

UDPSocket::UDPSocket() {}

UDPSocket* UDPSocket::create() {
  int socketDescr = socket(AF_INET, SOCK_DGRAM, 0);
  if (socketDescr < 0)
    return NULL;

  //int enabled = 1;
  //setsockopt(socketDescr, SOL_SOCKET, SO_BROADCAST, &enabled, sizeof(enabled));

  UDPSocket* socket = new UDPSocket();
  socket->m_socketDescriptor = socketDescr;
  socket->m_ip = "";
  socket->m_port = -1;
  return socket;
}

bool UDPSocket::bind(std::string ip, int port) {
  m_ip = ip;
  m_port = port; 

  struct sockaddr_in socketAddress;
  socketAddress.sin_family = AF_INET;
  socketAddress.sin_port = htons(m_port);
  inet_pton(AF_INET, m_ip.c_str(), &socketAddress.sin_addr);
  memset(socketAddress.sin_zero, 0, sizeof(socketAddress.sin_zero));

  int bindStatus = ::bind(m_socketDescriptor, (struct sockaddr*)&socketAddress, sizeof(socketAddress));
  if (bindStatus != 0) {
    return false;
  }
  return true;
}

bool UDPSocket::sendTo(Packet* packet, std::string destIp, int destPort, bool wait) {
  struct sockaddr_in destAddress;
  destAddress.sin_family = AF_INET;
  destAddress.sin_port = htons(destPort);
  inet_pton(AF_INET, destIp.c_str(), &destAddress.sin_addr);
  memset(destAddress.sin_zero, 0, sizeof(destAddress.sin_zero));  

  std::string answerIp; int answerPort;
  std::string buffer = packet->serialize();
  if (wait == true) {
    int timeoutCount = 0;
    do {
      int sent = sendto(m_socketDescriptor, buffer.c_str(), buffer.size(), 0, (struct sockaddr*)&destAddress, sizeof(struct sockaddr_in));
      if (sent < 0)
        return false;
      
      Packet* response = receiveFrom(&answerIp, &answerPort);
      
      if (response != NULL && response->type() == ACK) {
        delete response;
        return true;
      }
      timeoutCount++;
    } while (timeoutCount < SEND_TRIES);
  } else {

    int sent = sendto(m_socketDescriptor, buffer.c_str(), buffer.size(), 0, (struct sockaddr*)&destAddress, sizeof(destAddress));

    if (sent > 0)
      return true;
  }

  return false;
}

Packet* UDPSocket::receiveFrom(std::string* senderIp, int* senderPort, bool wait) {
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = DEFAULT_TIMEOUT * 1000 * (wait ? 0 : 1);
  setsockopt(m_socketDescriptor, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  struct sockaddr_in senderAddr;
  socklen_t addrLength = sizeof(senderAddr);
  char buffer[RECV_BUFFER_SIZE];
  int length = recvfrom(m_socketDescriptor, buffer, sizeof(buffer), 0, (struct sockaddr *)&senderAddr, &addrLength);

  if (length > 0) {
    buffer[length] = '\0';
    Packet *packet = Packet::deserialize(buffer);
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(senderAddr.sin_addr), str, INET_ADDRSTRLEN);
    *senderIp = str;
    *senderPort = ntohs(senderAddr.sin_port);

    return packet;
  }

  tv.tv_usec = 0;
  setsockopt(m_socketDescriptor, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  return NULL;
}

std::string UDPSocket::ip() {
  return m_ip;
}
int UDPSocket::port() {
  return m_port;
}