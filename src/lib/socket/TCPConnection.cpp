#include <unistd.h> // close()
#include <sys/types.h>
#include <sys/socket.h>
#include "TCPConnection.hpp"

TCPConnection::TCPConnection(int socket, std::string ipAddress, int portNumber) {
  socketDescriptor = socket;
  ip = ipAddress;
  port = portNumber;
}

TCPConnection::~TCPConnection() {
  close(socketDescriptor); // Closes socket of the given descriptor
}

std::string TCPConnection::getConnectionIp() {
  return ip;
}

int TCPConnection::getConnectionPort() {
  return port;
}

ssize_t TCPConnection::send(const char* buffer, size_t length) {
  return ::send(socketDescriptor, buffer, length, 0);
}

ssize_t TCPConnection::receive(char* buffer, size_t length) {
  return recv(socketDescriptor, buffer, length, 0);
}