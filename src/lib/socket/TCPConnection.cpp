#include <unistd.h> // close()
#include <stdlib.h> // rand()
#include <ctime>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "TCPConnection.hpp"

TCPConnection::TCPConnection(int socket, std::string ipAddress, int portNumber):
  m_socketDescriptor(socket),
  m_ip(ipAddress),
  m_port(portNumber)
{
  srand(time(NULL));
  m_sequenceNumber = rand() % 16384; 
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
  packet->m_sequenceNumber = m_sequenceNumber;
  m_sequenceNumber += 1; 
  packet->m_timestamp = std::time(NULL);
  std::string buffer = packet->serialize();
  return ::send(m_socketDescriptor, buffer.c_str(), buffer.size(), 0);
}

ssize_t TCPConnection::receive(char* buffer, size_t length) {
  return recv(m_socketDescriptor, buffer, length, 0);
}

Packet* TCPConnection::receive() {
  char buffer[RECV_BUFFER_SIZE];

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
  m_buffer.clear();
  return NULL;
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