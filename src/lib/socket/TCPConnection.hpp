#ifndef __TCPConnection_hpp__
#define __TCPConnection_hpp__

#include <string>
#include "lib/packet/Packet.hpp"


class TCPConnection {
  int m_socketDescriptor;
  std::string m_ip;
  int m_port;
  int m_sequenceNumber;

  public:
    friend class TCPClient;
    friend class TCPServer;

    std::string getConnectionIp();
    int getConnectionPort();

    ssize_t send(const char* buffer, size_t length);
    ssize_t send(Packet* packet);
    ssize_t receive(char* buffer, size_t length);
    Packet* receive();

    ~TCPConnection();
  
  private:
    TCPConnection(int socket, std::string ipAddress, int portNumber);
};

#endif //__TCPConnection_hpp__