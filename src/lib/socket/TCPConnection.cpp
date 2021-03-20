#include <unistd.h> // close()
#include <ctime>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include "TCPConnection.hpp"

TCPConnection::TCPConnection(int socket, std::string ipAddress, int portNumber) {
  m_socketDescriptor = socket;
  m_ip = ipAddress;
  m_port = portNumber;
}

TCPConnection::~TCPConnection() {
  close(m_socketDescriptor); // Closes socket of the given descriptor
}

std::string TCPConnection::getConnectionIp() {
  return m_ip;
}

int TCPConnection::getConnectionPort() {
  return m_port;
}

ssize_t TCPConnection::send(const char* buffer, size_t length) {
  return ::send(m_socketDescriptor, buffer, length, 0);
}

ssize_t TCPConnection::send(Packet* packet) {
  packet->m_sequenceNumber = 0; // TODO
  packet->m_timestamp = std::time(NULL);
  std::string buffer = packet->serialize();
  return ::send(m_socketDescriptor, buffer.c_str(), buffer.size(), 0);
}

ssize_t TCPConnection::receive(char* buffer, size_t length) {
  return recv(m_socketDescriptor, buffer, length, 0);
}

Packet* TCPConnection::receive() {
  char buffer[1024];
  int length = recv(m_socketDescriptor, buffer, sizeof(buffer)-1, 0);
  if (length > 0) {
    buffer[length] = '\0';
    return Packet::deserialize(buffer);
  }
  return NULL;
}