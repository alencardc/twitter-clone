#include <unistd.h> // close()
#include <stdlib.h> // rand()
#include <ctime>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "TCPConnection.hpp"

TCPConnection::TCPConnection(int socket, std::string ipAddress, int portNumber):
  m_socketDescriptor(socket),
  m_ip(ipAddress),
  m_port(portNumber),
  m_isClosed(false)
{
  srand(time(NULL));
  m_sequenceNumber = rand() % 16384; 
}

bool TCPConnection::isClosed() {
  return m_isClosed;
}

void TCPConnection::close() {
  m_isClosed = true;
  ::close(m_socketDescriptor); // Closes socket of the given descriptor
}

TCPConnection::~TCPConnection() {
  close(); // Closes socket of the given descriptor
}

std::string TCPConnection::getConnectionIp() {
  return m_ip;
}

int TCPConnection::getConnectionPort() {
  return m_port;
}

std::string TCPConnection::getPeerIp() {
  struct sockaddr_in addr;
  socklen_t  len = sizeof(addr);
  getpeername(m_socketDescriptor, (struct sockaddr*)&addr, &len);
  return inet_ntoa(addr.sin_addr);
}

int TCPConnection::getPeerPort() {
  struct sockaddr_in addr;
  socklen_t  len = sizeof(addr);
  getpeername(m_socketDescriptor, (struct sockaddr*)&addr, &len);
  return ntohs(addr.sin_port);
}

ssize_t TCPConnection::send(const char* buffer, size_t length) {
  return ::send(m_socketDescriptor, buffer, length, 0);
}

ssize_t TCPConnection::send(Packet* packet) {
  packet->m_sequenceNumber = m_sequenceNumber;
  m_sequenceNumber += 1; 
  packet->m_timestamp = std::time(NULL);
  std::string buffer = packet->serialize();
  return ::send(m_socketDescriptor, buffer.c_str(), buffer.size(), 0);
}

ssize_t TCPConnection::receive(char* buffer, size_t length) {
  return recv(m_socketDescriptor, buffer, length, 0);
}

bool TCPConnection::receiveAll(std::string& data, int length) {
  char buffer[length + 1];
  char* bufferPtr = buffer;
  int currLength = length;

  while (currLength > 0) {
    int bytesReceived = recv(m_socketDescriptor, bufferPtr, currLength, 0);
    if (bytesReceived < 1)
      return false;
    
    bufferPtr += bytesReceived;
    currLength -= bytesReceived;
  }
  buffer[length] = '\0';
  data = buffer;
  return true;
}

Packet* TCPConnection::receive() {

  std::string sizeStr;
  bool received = receiveAll(sizeStr, Packet::sizeFieldLength);
  if (received == true) {
    int size = std::stoul(sizeStr);
    std::string packetStr;
    received = receiveAll(packetStr, size);
    if (received == true) {
      packetStr.insert(0, sizeStr);
      Packet* pkt = Packet::deserialize(packetStr.c_str());
      return pkt;
    }
  }

  return NULL;
  /*
  if (m_buffer.empty() == false) {
    Packet *packet = Packet::deserialize(m_buffer.c_str());
    if (packet->type() != NO_TYPE) {
      m_buffer.erase(0, m_buffer.size());
      packet = breakPayload(packet);
      return packet;
    } else if(m_buffer.find("type:") != 0) {
      m_buffer.clear();
    }
  }

  int length = recv(m_socketDescriptor, buffer, sizeof(buffer)-1, 0);
  if (length > 0) {
    buffer[length] = '\0';
    Packet *packet = Packet::deserialize(buffer);
    if (packet->type() != NO_TYPE) {
      packet = breakPayload(packet);
    } else if (m_buffer.empty() == false) {
      m_buffer.append(buffer);
      packet = Packet::deserialize(m_buffer.c_str());
      if (packet->type() != NO_TYPE) {
        m_buffer.erase(0, m_buffer.size());
        packet = breakPayload(packet);
      } else {
        m_buffer.clear();
      }
    }
    return packet;
  }
  m_buffer.clear();*/
  return NULL;
}

Packet* TCPConnection::receive(int milliTimeout) {
  if (milliTimeout <= 0)
    return receive();

  if (waitForSocketEvent(milliTimeout) == true) {
    return receive();
  }
  return NULL;
}

bool TCPConnection::waitForSocketEvent(int milliTimeout) {
  fd_set sdset;
  struct timeval tv;

  tv.tv_sec = 0;
  tv.tv_usec = milliTimeout * 1000;
  FD_ZERO(&sdset);
  FD_SET(m_socketDescriptor, &sdset);
  if (select(m_socketDescriptor+1, &sdset, NULL, NULL, &tv) > 0){
    return true;
  }
  return false;
}

Packet* TCPConnection::breakPayload(Packet* packet) {
  std::string payload = packet->payload();
  if ((unsigned int)packet->length() < payload.size()) {
    m_buffer.append(payload.substr(packet->length(), std::string::npos));
    payload.erase(packet->length(), std::string::npos);
    delete packet->m_payload;
    packet->m_payload = strdup(payload.c_str());
  }
  return packet;
}