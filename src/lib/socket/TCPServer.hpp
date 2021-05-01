#ifndef __TCPServer_h__
#define __TCPServer_h__

#include <string>
#include "TCPConnection.hpp"

#define CONN_WAIT_LIST_SIZE 10

class TCPServer {

  int m_listeningSocket;
  bool m_listening;
  std::string m_ip;
  int m_port;
  
  public:
    TCPServer(const std::string serverIp, int portNumber);
    ~TCPServer();

    bool isListening();
    bool start();
    TCPConnection* accept();
    TCPConnection* accept(int milliTimeout);

    std::string ip();
    int port();
  
  private: 
    TCPServer() {}
};

#endif // __TCPServer_h__