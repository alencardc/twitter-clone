#ifndef __UDPSocket_hpp__
#define __UDPSocket_hpp__

#include <string>
#include "lib/packet/Packet.hpp"

#define SEND_TRIES 3
#define RECV_BUFFER_SIZE 400
// Timeout in milliseconds
#define DEFAULT_TIMEOUT 800


class UDPSocket {
  int m_socketDescriptor;
  std::string m_ip;
  int m_port;
  
  public:
    ~UDPSocket();
    static UDPSocket* create();
    bool bind(std::string ip, int port);
    bool sendTo(Packet* packet, std::string destIp, int destPort, bool wait=true);
    Packet* receiveFrom(std::string* senderIp, int* senderPort, bool wait=false);
    void close();

    std::string ip();
    int port();
  
  private:
    UDPSocket();
};

#endif // __UDPSocket_hpp__